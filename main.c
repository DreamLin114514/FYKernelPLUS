// SPDX-License-Identifier: GPL-2.0-only
/* 
 * Copyright (C) 2025 DreamLin114514 <xiao_xuan2020@163.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "fykernelplus.h"

struct fy_global_context fykp_ctx = {
    .cpu = {
        .policy = FY_MODE_BALANCED,
        .load_threshold = 70,
        .freq_boost = 10,
        .boost_enabled = true,
        .affinity_mask = CPU_MASK_NONE,
    },
    .memory = {
        .watermark_high = MEMORY_PRESSURE_THRESHOLD,
        .watermark_low = MEMORY_PRESSURE_THRESHOLD - 20,
    },
    .lock = __MUTEX_INITIALIZER(fykp_ctx.lock),
};

static int __init fykernelplus_init(void)
{
    int ret = 0;
    
    FY_INFO("Loading FYKernelPLUS v%s\n", MOD_VERSION);
    
    ret = fy_cpu_scheduler_init();
    if (ret) goto err_cpu;
    
    ret = fy_gpu_manager_init();
    if (ret) goto err_gpu;
    
    ret = fy_memory_manager_init();
    if (ret) goto err_mem;
    
    ret = fy_procfs_init();
    if (ret) goto err_procfs;
    
    ret = fy_sysfs_init();
    if (ret) goto err_sysfs;
    
    FY_INFO("FYKernelPLUS loaded successfully\n");
    return 0;
    
err_sysfs: fy_procfs_exit();
err_procfs: fy_memory_manager_exit();
err_mem: fy_gpu_manager_exit();
err_gpu: fy_cpu_scheduler_exit();
err_cpu: return ret;
}

static void __exit fykernelplus_exit(void)
{
    FY_INFO("Unloading FYKernelPLUS\n");
    fy_sysfs_exit();
    fy_procfs_exit();
    fy_memory_manager_exit();
    fy_gpu_manager_exit();
    fy_cpu_scheduler_exit();
    FY_INFO("FYKernelPLUS unloaded\n");
}

module_init(fykernelplus_init);
module_exit(fykernelplus_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION(MOD_VERSION);
MODULE_AUTHOR(MOD_AUTHOR);
MODULE_DESCRIPTION(MOD_DESC);