// SPDX-License-Identifier: GPL-2.0-only
/* 
 * Copyright (C) 2025 DreamLin114514 <xiao_xuan2020@163.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define PROC_PATH "/proc/fykernelplus/status"
#define SYSFS_MODE_PATH "/sys/kernel/fykernelplus/policy"

void show_version() {
    printf("FYKernel+ 作者:DreamLin 版本:FYKernelPLUS 1.1.1beta(1111)\n");
}

void show_modes() {
    printf("调度模式:\n");
    printf("1 省电模式\n");
    printf("2 均衡模式\n");
    printf("3 性能模式\n");
}

void set_mode(int mode) {
    if (mode < 1 || mode > 3) {
        printf("错误的模式,请输入1,2或3\n");
        exit(1);
    }

    int fd = open(SYSFS_MODE_PATH, O_WRONLY);
    if (fd < 0) {
        perror("无法访问模式控制界面");
        exit(1);
    }

    char buf[2];
    snprintf(buf, sizeof(buf), "%d", mode);
    
    if (write(fd, buf, strlen(buf)) < 0) {
        perror("设置模式失败");
        close(fd);
        exit(1);
    }

    close(fd);
    printf("模式已成功更改为 %d\n", mode);
}

void show_help() {
    printf("Usage: FYKernelPLUS [options]\n");
    printf("Options:\n");
    printf("  -v, --version    Show version information\n");
    printf("  -m, --mode [1-3] Set working mode\n");
    printf("  -m, --mode       Show available modes\n");
    printf("  -h, --help       Show this help message\n");
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        show_help();
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            show_version();
        }
        else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--mode") == 0) {
            if (i + 1 < argc && argv[i+1][0] != '-') {
                set_mode(atoi(argv[i+1]));
                i++;
            } else {
                show_modes();
            }
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            show_help();
        }
        else {
            printf("未知命令: %s\n", argv[i]);
            show_help();
            return 1;
        }
    }

    return 0;
}