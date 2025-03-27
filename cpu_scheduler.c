// SPDX-License-Identifier: GPL-2.0-only
/* 
 * Copyright (C) 2025 DreamLin114514 <xiao_xuan2020@163.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "fykernelplus.h"

static void apply_cpu_policy(int mode)
{
    struct cpufreq_policy *policy;
    unsigned int freq;
    int cpu;
    
    for_each_online_cpu(cpu) {
        policy = cpufreq_cpu_get(cpu);
        if (!policy) continue;
            
        switch (mode) {
            case FY_MODE_PERFORMANCE:
                freq = policy->max;
                cpufreq_driver_target(policy, freq, CPUFREQ_RELATION_H);
                break;
            case FY_MODE_POWERSAVE:
                freq = policy->min;
                cpufreq_driver_target(policy, freq, CPUFREQ_RELATION_L);
                break;
            case FY_MODE_BALANCED:
            default:
                freq = (policy->max + policy->min) / 2;
                cpufreq_driver_target(policy, freq, CPUFREQ_RELATION_C);
                break;
        }
        cpufreq_cpu_put(policy);
    }
}

static int cpu_load_balance(void *data)
{
    struct fy_cpu_context *ctx = &fykp_ctx.cpu;
    unsigned long load[MAX_CPUS];
    unsigned long total_load = 0;
    int cpu, i;
    
    while (!kthread_should_stop()) {
        for_each_online_cpu(cpu) {
            load[cpu] = 0;
            for (i = 0; i < 3; i++) {
                load[cpu] += cpu_load(cpu);
                msleep(10);
            }
            load[cpu] /= 3;
            total_load += load[cpu];
        }
        
        if (ctx->boost_enabled) {
            for_each_online_cpu(cpu) {
                if (load[cpu] > ctx->load_threshold) {
                    struct cpufreq_policy *policy = cpufreq_cpu_get(cpu);
                    if (policy) {
                        unsigned int freq = min(policy->max, policy->cur + ctx->freq_boost);
                        cpufreq_driver_target(policy, freq, CPUFREQ_RELATION_H);
                        cpufreq_cpu_put(policy);
                    }
                }
            }
        }
        msleep(100);
    }
    return 0;
}

static struct task_struct *load_balance_thread;

int fy_cpu_scheduler_init(void)
{
    struct fy_cpu_context *ctx = &fykp_ctx.cpu;
    cpumask_setall(&ctx->affinity_mask);
    
    load_balance_thread = kthread_run(cpu_load_balance, NULL, "fykp_cpu_balance");
    if (IS_ERR(load_balance_thread)) {
        FY_ERR("Failed to start CPU load balance thread\n");
        return PTR_ERR(load_balance_thread);
    }
    
    apply_cpu_policy(ctx->policy);
    FY_INFO("CPU scheduler initialized\n");
    return 0;
}

void fy_cpu_scheduler_exit(void)
{
    if (load_balance_thread) {
        kthread_stop(load_balance_thread);
        load_balance_thread = NULL;
    }
    fykp_ctx.cpu.policy = FY_MODE_BALANCED;
    apply_cpu_policy(FY_MODE_BALANCED);
    FY_INFO("CPU scheduler exited\n");
}