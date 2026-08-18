# AR9341 CR10开发板配置文件说明

本目录包含AR9341 CR10开发板的全部配置文件，用于构建固件、配置内核、U-Boot和系统启动参数等。

## 1. 构建固件目录结构

```
cr10_config/
├── board.config       - 主板配置定义文件
├── build.config       - 构建配置文件
├── kernel/            - 内核相关配置
├── plat/              - 平台相关配置
├── scenario/          - 启动脚本和场景配置
└── uboot/             - U-Boot相关配置
```

## 2. 硬件配置文件说明

### 2.1 基础配置

#### 2.1.1 board.config

定义开发板的基本配置和文件路径:
- `gCHIP_REV`: 芯片版本号 (001)
- `gBOARD`: 开发板名称 (cr10_config)
- `gDEV_PHASE`: 开发阶段标识 (d)
- 包含各种配置文件的路径定义，如内核配置、U-Boot配置等

#### 2.1.2 build.config

包含构建系统的详细配置:
- 内核模块配置 (如 `ar_framebuffer`, `ar_sys` 等)
- HAL库配置 (如 `HAL_LIB_VO_NEED`, `HAL_LIB_VIN_NEED` 等) 
- 服务应用配置 (如 `ar_logcat`, `ar_mpp_service` 等)
- 库依赖配置 (如 `FW_LIB_CAMERA_NEED`, `FW_LIB_SENSOR_NEED` 等)
- 元素配置选项

## 3. 内核配置 (kernel/)

### 3.1 artosyn-kernel.dts

设备树源文件，定义硬件配置:
- 引入基础设备树 (`../../common/artosyn-kernel.dtsi`)
- 引入内存配置 (`artosyn-memory.dtsi`)
- 配置设备接口:
  - UART接口配置
  - I2C总线配置
  - SPI接口配置
  - SDMMC接口配置
  - 其它硬件接口参数

#### 3.2 artosyn-memory.dtsi

内存布局配置:
- 保留内存区域:
  - `icc_reserved`: 0x22200000, 大小 0x200000 (2MB)
  - `dsp_reserved`: 0x32400000, 大小 0x2000000 (32MB)
  - `system_heap_reserved`: 0x34400000, 大小 0x400000 (4MB)
- Ion内存管理配置

#### 3.3 kernel-v4.9-17.07.config

完整的内核配置文件，包含所有内核选项和驱动程序的配置。

## 4. U-Boot配置 (uboot/)

### 4.1 env_emmc.txt

U-Boot环境变量配置，包含:
- 串口通信参数: `baudrate=115200`
- 启动参数: `bootargs=console=ttyS0,115200,earlyprintk ...`
- 启动命令: `bootcmd=...`
- 网络配置: `ipaddr=192.168.199.138`, `netmask=255.255.255.0`

#### 4.2 cfg-clock.dtsi

时钟配置:
- 各种总线和设备的时钟频率配置

#### 4.3 ddr-user-modify.dtsi

DDR内存配置:
- 时序参数
- 内存容量和类型配置

#### 4.4 pin_cfg.dtsi

引脚复用配置:
- 详细的GPIO引脚功能定义
- 每个引脚的复用模式

#### 4.5 u-boot-2020.10.config

完整的U-Boot配置，定义所有U-Boot特性和驱动支持。

## 5. 平台配置 (plat/)

### 5.1 flash_layout_emmc.ini

eMMC闪存布局配置:
- `bootloader`: 大小 0x40000 (256KB)
- `vendor`: 大小 0x80000 (512KB)
- `factory`: 大小 0x80000 (512KB)
- `env`: 大小 0x80000 (512KB)
- `uboot`: 大小 0x100000 (1MB)
- `kernel`: 大小 0x3000000 (48MB)
- `dtb`: 大小 0x80000 (512KB)

#### 5.2 fs.json

文件系统配置:
- `all_rootfs`: 根文件系统组件列表
  - 包含基础根文件系统、库文件、内核模块等
- `usr_data`: 用户数据分区组件列表
  - 包含测试文件、工具、应用程序等

## 6. 启动场景配置和启动脚本 (scenario/)

### 6.1 start.sh

系统主启动脚本:
- 加载核心内核模块:
  - `ar_mpp_drv.ko`
  - `ar_osal.ko`
  - `ar_vb.ko` 等
- 启动网络服务
- 启动NPU服务
- 调用其他启动脚本

#### 6.2 start_network.sh

网络配置脚本:
- 启动网络服务
- 配置网络参数

#### 6.3 start_app.sh

应用程序启动脚本，负责启动用户应用程序。

#### 6.4 busybox-1.25.0.config

BusyBox工具集配置，定义系统中包含的命令行工具。

#### 6.5 codec_factory_cfg.cjson

编解码器工厂配置，用于音视频编解码器的参数设置。

## 7. 使用说明

1. 修改配置时应注意文件间的依赖关系
2. 修改内核或U-Boot配置后需要重新编译对应组件
3. 修改闪存布局会影响分区大小和位置，需要注意数据兼容性
4. 启动脚本修改后会影响系统启动流程和加载的模块
5. 构建固件入口脚本为仓库根目录的 `create_img.sh`，例：
   ```bash
   ./create_img.sh -f emmc -p cr10_config
   ```
6. fs 升级包覆盖：脚本会自动用 SDK 根目录的 `cr10_root_fs_upgrade.tar` 和 `cr10_usrdata_fs_upgrade.tar` 在打包前分别覆盖更新 `all_rootfs` 和 `usr_data` 分区。可用环境变量 `ROOTFS_UPGRADE_TAR` / `USERDATA_UPGRADE_TAR` 指定其它升级包或设为空禁用。详见 `plat/README.md` 的 3.2 节。

## 8. 注意事项

1. 修改引脚复用配置前应确认硬件设计文档
2. 修改时钟和DDR参数可能导致系统不稳定，请谨慎操作
3. 添加内核模块时需在build.config中启用相应选项
4. 修改网络配置前应了解目标网络环境

## 9. 调试方法

1. 通过串口查看启动日志
2. 使用`dmesg`命令查看内核日志
3. 修改启动脚本添加调试信息
4. 在关键位置添加打印语句检查执行流程
