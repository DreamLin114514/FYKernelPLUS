// SPDX-License-Identifier: GPL-2.0-only
/* 
 * Copyright (C) 2025 DreamLin114514 <xiao_xuan2020@163.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef FYKERNELPLUS_H
#define FYKERNELPLUS_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/cpufreq.h>
#include <linux/cpu.h>
#include <linux/topology.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/dma-mapping.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/atomic.h>
#include <linux/jiffies.h>
#include <linux/timer.h>

#define MOD_NAME "FYKernelPLUS"
#define MOD_VERSION "1.0.0"
#define MOD_VERSION_NUM 1000
#define MOD_AUTHOR "DreamLin"
#define MOD_DESC "Advanced CPU/GPU/Memory Manager for Linux 6.x+"

#ifdef DEBUG
#define FY_DEBUG(fmt, args...) printk(KERN_DEBUG MOD_NAME ": " fmt, ##args)
#else
#define FY_DEBUG(fmt, args...)
#endif

#define FY_INFO(fmt, args...) printk(KERN_INFO MOD_NAME ": " fmt, ##args)
#define FY_WARN(fmt, args...) printk(KERN_WARNING MOD_NAME ": " fmt, ##args)
#define FY_ERR(fmt, args...) printk(KERN_ERR MOD_NAME ": " fmt, ##args)

#define MAX_CPUS NR_CPUS
#define MAX_GPUS 8
#define GPU_PCI_CLASS 0x030000
#define MEMORY_PRESSURE_THRESHOLD 70
#define MEMORY_RELEASE_DELAY_MS 500

enum fy_work_mode {
    FY_MODE_POWERSAVE = 1,
    FY_MODE_BALANCED = 2,
    FY_MODE_PERFORMANCE = 3
};

struct fy_cpu_context {
    int policy;
    unsigned int load_threshold;
    unsigned int freq_boost;
    bool boost_enabled;
    struct cpumask affinity_mask;
};

struct fy_gpu_device {
    struct pci_dev *dev;
    unsigned int id;
    unsigned long usage;
    unsigned long max_mem;
    unsigned long used_mem;
    bool power_save;
};

struct fy_memory_context {
    atomic_long_t pressure;
    unsigned long watermark_high;
    unsigned long watermark_low;
    struct timer_list release_timer;
    struct work_struct release_work;
};

struct fy_global_context {
    struct fy_cpu_context cpu;
    struct fy_gpu_device gpus[MAX_GPUS];
    unsigned int num_gpus;
    struct fy_memory_context memory;
    struct proc_dir_entry *proc_dir;
    struct kobject *sysfs_kobj;
    struct mutex lock;
};

extern struct fy_global_context fykp_ctx;

int fy_cpu_scheduler_init(void);
void fy_cpu_scheduler_exit(void);
int fy_gpu_manager_init(void);
void fy_gpu_manager_exit(void);
int fy_memory_manager_init(void);
void fy_memory_manager_exit(void);
int fy_procfs_init(void);
void fy_procfs_exit(void);
int fy_sysfs_init(void);
void fy_sysfs_exit(void);

#endif // FYKERNELPLUS_H