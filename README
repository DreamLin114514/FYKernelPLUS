# FYKernelPLUS 内核模块

## License
FYKernelPLUS is licensed under **GNU GPL v2**. See [LICENSE](LICENSE) for details.

## 文件结构
Makefile # 内核模块构建文件
fykernelplus.h # 头文件(核心定义)
fykernelplus-cli.c # 用户空间命令行工具
cpu_scheduler.c # CPU调度实现
gpu_manager.c # GPU管理实现
memory_manager.c # 内存管理实现
procfs_interface.c # Procfs接口实现
sysfs_interface.c # Sysfs接口实现
main.c # 模块初始化和退出

## 功能特性

### CPU 调度
- **三种工作模式**:
  - 省电模式(1): CPU频率锁定在最低
  - 均衡模式(2): CPU频率设为中间值
  - 性能模式(3): CPU频率锁定在最高
- **动态负载均衡**:
  - 自动监控CPU负载
  - 负载高时自动提升频率

### GPU 管理
- **检测方法**:
  - 通过PCI类代码检测插入的GPU设备
  - 理论上仅支持8个GPU设备
- **电源管理**:
  - 支持基本的电源状态管理
- **频率调整/超频**:
  - 为保证您的电脑不冒烟，已将此功能删除

### 内存管理
- **压力监控**:
  - 注册内存压力通知器
  - 每隔200ms监控系统内存压力
- **内存溢出处理**: 
  - 当内存压力超过高水位线时自动回收内存
  - 结合工作队列异步执行回收操作
- **内存压缩** (已在不支持的电脑上禁用):
  - 在内存紧张时自动触发内存压缩(lz4kd)

## 附加功能

### Boost功能
(不支持的设备已自动禁用):
- 可以动态启用/禁用频率提升
- 可配置提升幅度(默认10%)

⭐ **启用方法** (不支持的设备返回为"无法开启"):
`echo 1 > /sys/kernel/fykernelplus/boost`

### 终端指令
-查看版本信息: `FYKernelPLUS -v` 或 `FYKernelPLUS --version` 
-查看/更改调度模式: `FYKernelPLUS -m` 或 `FYKernelPLUS --mode` 
-帮助: `FYKernelPLUS -h` 或 `FYKernelPLUS --help` 

### 卸载模块
```
cd (模块路径)
sudo make uninstall
```
##其他说明
-GPU以及RAM调度配置使用Deepseek V1优化

##⚠ 重要警告
- 仅兼容Linux 6.x+内核
- 必须安装GCC-12+工具链
- 错误使用会导致系统不稳定！后果自负！