// SPDX-License-Identifier: GPL-2.0-only
/* 
 * Copyright (C) 2025 DreamLin114514 <xiao_xuan2020@163.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "fykernelplus.h"

static void memory_pressure_work(struct work_struct *work)
{
    struct fy_memory_context *ctx = &fykp_ctx.memory;
    long pressure = atomic_long_read(&ctx->pressure);
    
    if (pressure > MEMORY_PRESSURE_THRESHOLD) {
        FY_INFO("High memory pressure detected (%ld%%), reclaiming memory\n", pressure);
        
        // 尝试回收页面
        try_to_free_pages(&init_mm, 0, GFP_KERNEL);
        
        // 压缩内存
        vm_compact_memory();
    }
}

static DECLARE_WORK(mem_work, memory_pressure_work);

static void memory_release_timer(struct timer_list *timer)
{
    struct fy_memory_context *ctx = container_of(timer, struct fy_memory_context, release_timer);
    
    schedule_work(&mem_work);
    mod_timer(&ctx->release_timer, jiffies + msecs_to_jiffies(MEMORY_RELEASE_DELAY_MS));
}

static int memory_pressure_notifier(struct notifier_block *nb,
                                  unsigned long action, void *data)
{
    struct fy_memory_context *ctx = &fykp_ctx.memory;
    unsigned long pressure = *(unsigned long *)data;
    
    atomic_long_set(&ctx->pressure, pressure);
    
    if (pressure > ctx->watermark_high) {
        // 立即触发内存回收
        schedule_work(&mem_work);
    }
    
    return NOTIFY_OK;
}

static struct notifier_block memory_pressure_nb = {
    .notifier_call = memory_pressure_notifier,
};

int fy_memory_manager_init(void)
{
    struct fy_memory_context *ctx = &fykp_ctx.memory;
    
    // 初始化定时器
    timer_setup(&ctx->release_timer, memory_release_timer, 0);
    mod_timer(&ctx->release_timer, jiffies + msecs_to_jiffies(MEMORY_RELEASE_DELAY_MS));
    
    // 注册内存压力通知器
    register_memory_pressure_notifier(&memory_pressure_nb);
    
    FY_INFO("Memory manager initialized\n");
    return 0;
}

void fy_memory_manager_exit(void)
{
    struct fy_memory_context *ctx = &fykp_ctx.memory;
    
    del_timer_sync(&ctx->release_timer);
    cancel_work_sync(&mem_work);
    unregister_memory_pressure_notifier(&memory_pressure_nb);
    
    FY_INFO("Memory manager exited\n");
}