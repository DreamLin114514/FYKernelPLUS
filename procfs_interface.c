// SPDX-License-Identifier: GPL-2.0-only
/* 
 * Copyright (C) 2025 DreamLin114514 <xiao_xuan2020@163.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "fykernelplus.h"

static int fykp_proc_show(struct seq_file *m, void *v)
{
    int i;
    
    mutex_lock(&fykp_ctx.lock);
    
    seq_printf(m, "FYKernelPLUS v%s\n", MOD_VERSION);
    seq_puts(m, "----------------\n");
    
    // CPU信息
    seq_puts(m, "\nCPU Scheduling:\n");
    seq_printf(m, "  Policy: %d\n", fykp_ctx.cpu.policy);
    seq_printf(m, "  Load Threshold: %u%%\n", fykp_ctx.cpu.load_threshold);
    seq_printf(m, "  Frequency Boost: %u%%\n", fykp_ctx.cpu.freq_boost);
    seq_printf(m, "  Boost Enabled: %s\n", fykp_ctx.cpu.boost_enabled ? "yes" : "no");
    
    // GPU信息
    seq_puts(m, "\nGPU Management:\n");
    seq_printf(m, "  Detected GPUs: %u\n", fykp_ctx.num_gpus);
    for (i = 0; i < fykp_ctx.num_gpus; i++) {
        seq_printf(m, "  GPU %d: %04x:%04x, Memory: %lu/%lu MB\n",
                   fykp_ctx.gpus[i].id,
                   fykp_ctx.gpus[i].dev->vendor,
                   fykp_ctx.gpus[i].dev->device,
                   fykp_ctx.gpus[i].used_mem >> 20,
                   fykp_ctx.gpus[i].max_mem >> 20);
    }
    
    // 内存信息
    seq_puts(m, "\nMemory Management:\n");
    seq_printf(m, "  Pressure: %ld%%\n", atomic_long_read(&fykp_ctx.memory.pressure));
    seq_printf(m, "  Watermarks: low=%lu%%, high=%lu%%\n",
               fykp_ctx.memory.watermark_low,
               fykp_ctx.memory.watermark_high);
    
    mutex_unlock(&fykp_ctx.lock);
    return 0;
}

static int fykp_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, fykp_proc_show, NULL);
}

static const struct proc_ops fykp_proc_fops = {
    .proc_open = fykp_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

int fy_procfs_init(void)
{
    fykp_ctx.proc_dir = proc_mkdir("fykernelplus", NULL);
    if (!fykp_ctx.proc_dir) {
        FY_ERR("Failed to create proc directory\n");
        return -ENOMEM;
    }
    
    if (!proc_create("status", 0444, fykp_ctx.proc_dir, &fykp_proc_fops)) {
        FY_ERR("Failed to create proc status file\n");
        remove_proc_entry("fykernelplus", NULL);
        return -ENOMEM;
    }
    
    FY_INFO("Procfs interface initialized\n");
    return 0;
}

void fy_procfs_exit(void)
{
    if (fykp_ctx.proc_dir) {
        remove_proc_entry("status", fykp_ctx.proc_dir);
        remove_proc_entry("fykernelplus", NULL);
        fykp_ctx.proc_dir = NULL;
    }
    
    FY_INFO("Procfs interface exited\n");
}