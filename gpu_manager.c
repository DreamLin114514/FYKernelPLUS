// SPDX-License-Identifier: GPL-2.0-only
/* 
 * Copyright (C) 2025 DreamLin114514 <xiao_xuan2020@163.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "fykernelplus.h"

static int gpu_pci_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
    struct fy_gpu_device *gpu;
    unsigned long mem_size;
    int ret;
    
    if (fykp_ctx.num_gpus >= MAX_GPUS) {
        FY_WARN("Maximum GPU limit reached (%d)\n", MAX_GPUS);
        return -ENOSPC;
    }
    
    gpu = &fykp_ctx.gpus[fykp_ctx.num_gpus];
    
    // 启用设备
    ret = pci_enable_device(dev);
    if (ret) {
        FY_ERR("Failed to enable PCI device %04x:%04x (%d)\n",
               dev->vendor, dev->device, ret);
        return ret;
    }
    
    // 获取GPU内存信息
    mem_size = pci_resource_len(dev, 0);
    
    gpu->dev = dev;
    gpu->id = fykp_ctx.num_gpus;
    gpu->max_mem = mem_size;
    gpu->used_mem = 0;
    gpu->power_save = false;
    
    FY_INFO("Detected GPU %d: %04x:%04x (%luMB)\n",
            gpu->id, dev->vendor, dev->device, mem_size >> 20);
    
    fykp_ctx.num_gpus++;
    return 0;
}

static void gpu_pci_remove(struct pci_dev *dev)
{
    int i;
    
    for (i = 0; i < fykp_ctx.num_gpus; i++) {
        if (fykp_ctx.gpus[i].dev == dev) {
            FY_INFO("Removing GPU %d: %04x:%04x\n",
                    fykp_ctx.gpus[i].id, dev->vendor, dev->device);
                    
            // 移动数组以保持连续性
            for (; i < fykp_ctx.num_gpus - 1; i++) {
                fykp_ctx.gpus[i] = fykp_ctx.gpus[i+1];
            }
            
            fykp_ctx.num_gpus--;
            break;
        }
    }
    
    pci_disable_device(dev);
}

static struct pci_device_id gpu_pci_ids[] = {
    { PCI_DEVICE_CLASS(GPU_PCI_CLASS, PCI_ANY_ID) },
    { 0, }
};

static struct pci_driver fy_gpu_driver = {
    .name = "fykp_gpu",
    .id_table = gpu_pci_ids,
    .probe = gpu_pci_probe,
    .remove = gpu_pci_remove,
};

int fy_gpu_manager_init(void)
{
    int ret;
    
    fykp_ctx.num_gpus = 0;
    
    ret = pci_register_driver(&fy_gpu_driver);
    if (ret) {
        FY_ERR("Failed to register GPU PCI driver (%d)\n", ret);
        return ret;
    }
    
    FY_INFO("GPU manager initialized, found %d GPUs\n", fykp_ctx.num_gpus);
    return 0;
}

void fy_gpu_manager_exit(void)
{
    pci_unregister_driver(&fy_gpu_driver);
    FY_INFO("GPU manager exited\n");
}