// SPDX-License-Identifier: GPL-2.0-only
/* 
 * Copyright (C) 2025 DreamLin114514 <xiao_xuan2020@163.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "fykernelplus.h"

static ssize_t policy_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", fykp_ctx.cpu.policy);
}

static ssize_t policy_store(struct kobject *kobj, struct kobj_attribute *attr,
                           const char *buf, size_t count)
{
    int policy;
    
    if (kstrtoint(buf, 10, &policy))
        return -EINVAL;
        
    if (policy < FY_MODE_POWERSAVE || policy > FY_MODE_PERFORMANCE)
        return -EINVAL;
        
    mutex_lock(&fykp_ctx.lock);
    fykp_ctx.cpu.policy = policy;
    apply_cpu_policy(policy);
    mutex_unlock(&fykp_ctx.lock);
    
    return count;
}

static struct kobj_attribute policy_attr = __ATTR(policy, 0644, policy_show, policy_store);
static struct attribute *fykp_attrs[] = { &policy_attr.attr, NULL };
static struct attribute_group fykp_attr_group = { .attrs = fykp_attrs, .name = "fykernelplus" };

int fy_sysfs_init(void)
{
    int ret;
    
    fykp_ctx.sysfs_kobj = kobject_create_and_add("fykernelplus", kernel_kobj);
    if (!fykp_ctx.sysfs_kobj) return -ENOMEM;
    
    ret = sysfs_create_group(fykp_ctx.sysfs_kobj, &fykp_attr_group);
    if (ret) {
        kobject_put(fykp_ctx.sysfs_kobj);
        return ret;
    }
    
    FY_INFO("Sysfs interface initialized\n");
    return 0;
}

void fy_sysfs_exit(void)
{
    if (fykp_ctx.sysfs_kobj) {
        sysfs_remove_group(fykp_ctx.sysfs_kobj, &fykp_attr_group);
        kobject_put(fykp_ctx.sysfs_kobj);
        fykp_ctx.sysfs_kobj = NULL;
    }
    FY_INFO("Sysfs interface exited\n");
}