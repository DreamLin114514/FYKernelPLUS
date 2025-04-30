// SPDX-License-Identifier: GPL-2.0-only
/* 
 * Copyright (C) 2025 DreamLin114514 <xiao_xuan2020@163.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef NVIDIA_OPTIMIZATION_H
#define NVIDIA_OPTIMIZATION_H

#include <linux/pci.h>

#define NVIDIA_VENDOR_ID    0x10DE
#define NVIDIA_GPU_CLASS    0x030000

enum nvidia_power_mode {
    NVIDIA_POWER_DEFAULT = 0,
    NVIDIA_POWER_SAVE,
    NVIDIA_POWER_ADAPTIVE,
    NVIDIA_POWER_PREFER_MAX
};

struct nvidia_mem_config {
    unsigned int frequency_mhz;
    unsigned int voltage_uv;
    bool ecc_enabled;
};

int nvidia_gpu_init(struct pci_dev *dev);

int nvidia_set_power_mode(struct pci_dev *dev, enum nvidia_power_mode mode);
int nvidia_get_current_power(struct pci_dev *dev, unsigned int *power_mw);

int nvidia_configure_memory(struct pci_dev *dev, struct nvidia_mem_config *cfg);
int nvidia_enable_mem_overclock(struct pci_dev *dev, bool enable);

int nvidia_get_temperature(struct pci_dev *dev, unsigned int *temp_c);

int nvidia_configure_pcie(struct pci_dev *dev, unsigned int gen, unsigned int lanes);

#endif // NVIDIA_OPTIMIZATION_H