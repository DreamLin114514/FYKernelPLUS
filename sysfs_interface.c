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

static ssize_t nvidia_power_mode_show(struct kobject *kobj, 
                                    struct kobj_attribute *attr, char *buf)
{
    struct fy_gpu_device *gpu = container_of(kobj, struct fy_gpu_device, kobj);
    unsigned int mode;
    nvidia_get_power_mode(gpu->dev, &mode);
    return sprintf(buf, "%u\n", mode);
}

static ssize_t nvidia_power_mode_store(struct kobject *kobj,
                                     struct kobj_attribute *attr,
                                     const char *buf, size_t count)
{
    struct fy_gpu_device *gpu = container_of(kobj, struct fy_gpu_device, kobj);
    unsigned int mode;
    
    if (kstrtouint(buf, 10, &mode))
        return -EINVAL;
    
    if (mode > NVIDIA_POWER_PREFER_MAX)
        return -EINVAL;
    
    nvidia_set_power_mode(gpu->dev, mode);
    return count;
}

static struct kobj_attribute nvidia_power_attr = 
    __ATTR(power_mode, 0644, nvidia_power_mode_show, nvidia_power_mode_store);

static struct attribute *nvidia_attrs[] = {
    &nvidia_power_attr.attr,
    NULL
};

static struct attribute_group nvidia_attr_group = {
    .name = "nvidia",
    .attrs = nvidia_attrs,
};

// 在GPU设备注册时添加
int register_nvidia_sysfs(struct fy_gpu_device *gpu)
{
    if (gpu->vendor_id == NVIDIA_VENDOR_ID) {
        return sysfs_create_group(&gpu->kobj, &nvidia_attr_group);
    }
    return 0;
}