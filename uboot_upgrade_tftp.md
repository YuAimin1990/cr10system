# Artosyn AR9341 U-Boot TFTP + DDR 升级协议规范

本文档描述 AR9341 平台 U-Boot 阶段通过 TFTP 传输升级镜像到 DDR，再调用 `artosyn_upgrade dram` 命令完成系统升级的完整流程，供 PC 端升级工具开发使用。

源码参考：
- `arsdk/platforms/uboot/u-boot-2020.10/board/artosyn/common/ar_flash.c`（`do_artosyn_upgrade` / `do_upgrade`）
- `arsdk/platforms/uboot/u-boot-2020.10/board/artosyn/evb_ar9301/evb_ar9301.c`（`board_eth_init`）
- `arsdk/platforms/uboot/u-boot-2020.10/include/uboot_upgrade.h`（地址定义 / 镜像头结构）
- `arsdk/platforms/uboot/u-boot-2020.10/configs/ar9341_emmc_defconfig`（网络 / TFTP 配置）

## 1. 方案概述

### 1.1 方案原理

PC 端通过标准 TFTP 协议将升级镜像传输到设备端 DDR 指定地址，然后通过串口发送 `artosyn_upgrade dram` 命令触发升级。设备端从 DDR 读取镜像并写入 flash。

### 1.2 与 USB HID 方案对比

| 对比项 | USB HID 方案 | TFTP + DDR 方案 |
|--------|-------------|-----------------|
| PC 端复杂度 | 高（需实现自定义 HID 协议） | 低（标准 TFTP 客户端 + 串口命令） |
| 传输速度 | ~1MB/s（HID 中断端点限制） | ~10MB/s（以太网 TFTP） |
| 进度反馈 | 通过 HID IN 端点回传 | 仅串口输出（callback 为 NULL） |
| 硬件要求 | USB 线缆 | 以太网线缆 + 串口线缆 |
| 自动化难度 | 高 | 低 |

### 1.3 适用场景

- 有以太网接口的板卡
- 需要快速传输大体积镜像的场景
- PC 端工具开发资源有限、希望复用标准 TFTP 库的场景

## 2. 硬件与网络前提

### 2.1 以太网接口

设备端使用 Artosyn 自研 GMAC 控制器，通过 RGMII 接口连接 PHY 芯片。

- 驱动初始化：`board_eth_init()`（`evb_ar9301.c:26-51`）
- 配置宏：`CONFIG_ARTOSYN_GMAC=y`（`ar9341_emmc_defconfig:1122`）
- 接口模式：`PHY_INTERFACE_MODE_RGMII`
- 默认 MAC 地址：`00:50:c2:13:6f:00`（`evb_ar9301.c:33`）

### 2.2 U-Boot 网络命令支持

以下配置均已启用（`ar9341_emmc_defconfig:603-633,767-773`）：

| 配置项 | 值 | 说明 |
|--------|-----|------|
| `CONFIG_NET` | y | 网络协议栈 |
| `CONFIG_CMD_NET` | y | 网络命令 |
| `CONFIG_CMD_TFTPBOOT` | y | `tftpboot` 命令 |
| `CONFIG_CMD_DHCP` | y | `dhcp` 命令 |
| `CONFIG_CMD_PING` | y | `ping` 命令 |
| `CONFIG_CMD_MII` | y | `mii` 命令 |
| `CONFIG_TFTP_BLOCKSIZE` | 1468 | TFTP 块大小（字节） |
| `CONFIG_TFTP_WINDOWSIZE` | 1 | TFTP 窗口大小 |
| `CONFIG_NET_TFTP_VARS` | y | TFTP 变量支持 |

### 2.3 环境变量存储

- `CONFIG_ENV_IS_IN_MMC=y` — 环境变量存储在 eMMC 中（`ar9341_emmc_defconfig:751`）
- `CONFIG_SAVEENV=y` — `saveenv` 命令可用（`ar9341_emmc_defconfig:744`）
- `CONFIG_SYS_MMC_ENV_DEV=0` — 环境变量存储在 MMC 设备 0（`ar9341_emmc_defconfig:760`）

> **注意**：`CONFIG_ENV_OVERWRITE` 未启用（`ar9341_emmc_defconfig:745`），意味着某些受保护的环境变量（如 `ethaddr`）无法通过 `setenv` 修改。

## 3. 关键地址定义

地址定义在 `uboot_upgrade.h:8-11`：

| 常量 | 值 | 说明 |
|------|-----|------|
| `IMAGE_DECOMPRESS_ADDR` | `0x24400000` | 镜像解压地址 |
| `COMPR_SEG_SIZE` | `0x500000`（5MB） | 压缩段大小 |
| `IMAGE_LOAD_ADDR` | `0x24450000` | **镜像加载地址**（= `IMAGE_DECOMPRESS_ADDR + COMPR_SEG_SIZE`） |

> **关键**：TFTP 传输目标地址必须为 `0x24450000`，即 `IMAGE_LOAD_ADDR`。`artosyn_upgrade dram` 命令直接从此地址读取镜像。

## 4. 升级镜像格式

### 4.1 镜像头部结构

升级镜像为 Artosyn 自定义格式，头部结构 `struct upgrade_hdr` 定义在 `uboot_upgrade.h:30-51`，共 256 字节：

| 字段 | 偏移 | 大小 | 说明 |
|------|------|------|------|
| `magic` | 0 | 4 | 魔数，`__le32` 小端存储，数值为 `0x4152544F`。文件中的实际字节为 `4F 54 52 41`（"OTRA"）。校验代码：`__le32_to_cpu(hdr->magic) != MAGIC`（`ar_flash.c:2778`） |
| `hdr_version` | 4 | 1 | 头部版本 |
| `compressed` | 5 | 1 | 是否压缩 |
| `flashtype` | 6 | 1 | Flash 类型：0=SPI NOR, 1=eMMC, 2=SPI NAND |
| `part_status` | 7 | 1 | 分区是否变更 |
| `header_ext_size` | 8 | 2 | 扩展头大小 |
| `hash_size` | 10 | 2 | 哈希大小 |
| `sig_size` | 12 | 2 | 签名大小 |
| `sig_realsize` | 14 | 2 | 签名实际大小 |
| `img_size` | 16 | 8 | 签名后的镜像数据大小 |
| `rom_size` | 24 | 4 | ROM 代码大小 |
| `loader_size` | 28 | 4 | Bootloader 大小 |
| `partitions` | 32 | 2 | 分区数量 |
| `segments` | 34 | 2 | 段数量 |
| `object_version` | 36 | 4 | 镜像版本号 |
| `depend_version` | 40 | 4 | 依赖的最低版本号 |
| `reserve` | 44 | 20 | 保留字段 |
| `part_flag` | 64 | 64 | 分区升级标志位 |
| `sdk_version` | 128 | 128 | SDK 版本字符串 |

### 4.2 镜像数据布局

头部之后的数据布局（`ar_flash.c:3233-3238`）：

```
[upgrade_hdr (256 bytes)] [hdr_ext (32+256+header_ext_size)] [romcode (rom_size)] [bootloader (loader_size)] [partitions[]] [segments[]] [image data...]
```

- `partitions` 为 `struct part_info` 数组，每个条目 52 字节（`uboot_upgrade.h:54-60`）
- `segments` 为 `struct segment_info` 数组，每个条目 32 字节（`uboot_upgrade.h:63-68`）

### 4.3 镜像文件类型

`do_upgrade()` 通过 magic 校验（`0x4152544F`）识别 Artosyn 封装格式的镜像。只要 magic 校验通过，`do_upgrade()` 即可解析并按分区表和段表烧写。实际存在两类镜像：

| 类型 | 文件名示例 | magic 字节 | 说明 |
|------|-----------|-----------|------|
| 完整升级镜像 | `artosyn-upgrade-ar9341.img` | `4F 54 52 41`（OTRA）✓ | 含全部分区和段，整机烧写 |
| 组件升级镜像 | `uboot.nonsec.img` | `4F 54 52 41`（OTRA）✓ | 只含 U-Boot 分区，仅更新对应分区 |
| 裸二进制 | `troot_spl.nonsec.img` | `01 93 52 41` ✗ | 无 ARTO 魔数，`do_upgrade()` 校验失败，不能通过此通道烧写 |

PC 端工具可通过读取文件头部前 4 字节判断是否为合法升级镜像：
- 字节 `4F 54 52 41` → 合法 Artosyn 升级镜像，可继续传输
- 其他值 → 非法镜像，应拒绝并提示用户

### 4.4 镜像文件命名

升级镜像文件名通常为 `artosyn-upgrade-ar9341.img`，需放置在 PC 端 TFTP 服务器的根目录下。

## 5. 设备端升级命令

### 5.1 artosyn_upgrade 命令

命令定义在 `ar_flash.c:3588-3641`：

```
artosyn_upgrade              - 从 U 盘升级（默认）
artosyn_upgrade test         - 升级测试循环（CTRLC 退出）
artosyn_upgrade force        - 强制升级（忽略 DDR 变化）
artosyn_upgrade dram         - 从 DDR 升级（需先将镜像加载到 IMAGE_LOAD_ADDR）
```

### 5.2 artosyn_upgrade dram 实现细节

源码位于 `ar_flash.c:3600-3609`：

```c
} else if (argc == 2 && !strcmp(argv[1], "dram")) {
    printf("\nuboot upgrade from ddr\n");
    set_single_upgrade(true);
    ret = do_upgrade((unsigned char *)IMAGE_LOAD_ADDR, NULL);
    if (ret)
        printf("uboot upgrade failed, please check your image or udisk and upgrade again...\n");
    else
        printf("uboot upgrade finish, please reset the board...\n");
    return 0;
}
```

关键行为：
- 调用 `set_single_upgrade(true)` 设置单次升级模式
- 调用 `do_upgrade((unsigned char *)IMAGE_LOAD_ADDR, NULL)`，第二个参数 `NULL` 表示**无进度回调**
- 返回值为 0 表示成功，非 0 表示失败

### 5.3 do_upgrade 升级流程

`do_upgrade()` 位于 `ar_flash.c:3214-3312`，执行以下步骤：

1. **镜像验证**（`do_verify_image`）：验证 magic、签名、哈希
2. **DDR 配置校验**（`do_verify_ddr_setting`）：检查镜像中的 DDR 配置与当前板载是否匹配
3. **Flash 类型识别**：根据 `hdr->flashtype` 选择对应的升级操作集（MMC / NOR / NAND）
4. **初始化**（`do_upgrade_init`）：初始化 flash 设备
5. **分区检查**（`do_upgrade_sanity_check`）：校验分区信息合法性
6. **写入 ROM 代码**（`do_upgrade_rom`）：如有 `rom_size > 0`
7. **写入 Bootloader**（`do_upgrade_bootloader`）：如有 `loader_size > 0`
8. **写入段数据**（`do_upgrade_segments`）：逐段写入 flash
9. **写入 GPT 分区表**（`do_upgrade_gpt`）：更新分区表

### 5.4 错误码定义

错误码定义在 `ar_flash.c:206-215`：

| 错误码 | 值 | 说明 |
|--------|-----|------|
| `ERR_SUCCESS` | 0 | 成功 |
| `ERR_VERIFY` | -1 | 镜像验证失败 |
| `ERR_PROGRAM` | -2 | 编程失败 |
| `ERR_ERASE` | -3 | 擦除失败 |
| `ERR_FLASH_TYPE` | -4 | Flash 类型不匹配 |
| `ERR_IMG_SIZE` | -5 | 镜像大小异常 |
| `ERR_OTHER` | -6 | 其他错误 |
| `ERR_DDR` | -7 | DDR 配置不匹配 |

### 5.5 DDR 变化处理

当 `do_upgrade` 返回 `ERR_DDR`（-7）时（`ar_flash.c:3240-3245`）：

- `artosyn_upgrade dram` 模式下**不处理** DDR 变化，直接打印失败信息
- 若需强制升级，应使用 `artosyn_upgrade force dram`（先设置 force 标志再从 DDR 升级）

> **注意**：`artosyn_upgrade force` 命令仅设置 `force_upgrade = true`（`ar_flash.c:3597-3599`），但随后进入 U 盘升级流程，**不会**自动执行 dram 升级。若需强制从 DDR 升级，PC 端工具应在串口中依次执行：
> ```
> => artosyn_upgrade force
> => artosyn_upgrade dram
> ```
> 或直接使用 `artosyn_upgrade dram`，但 DDR 不匹配时会失败。`force` 标志一旦设置会持续生效直到下次重启。

## 6. 完整升级流程

### 6.1 设备端操作

#### 6.1.1 进入 U-Boot 命令行

设备上电后，在串口终端按任意键中断自动启动，进入 U-Boot 命令行提示符 `=>`。

#### 6.1.2 配置网络

**方式一：静态 IP 配置**

```
=> setenv ipaddr <设备IP>
=> setenv serverip <PC端IP>
=> setenv netmask <子网掩码>
=> saveenv
```

**方式二：DHCP 获取 IP**

```
=> dhcp
```

> **注意**：使用 DHCP 时，TFTP 服务器地址默认为 DHCP 服务器地址。如需指定不同的 TFTP 服务器，执行 `=> setenv serverip <PC端IP>`。

#### 6.1.3 验证网络连通性

```
=> ping <PC端IP>
```

期望输出：`host <PC端IP> is alive`

#### 6.1.4 TFTP 加载镜像

```
=> tftpboot 0x24450000 artosyn-upgrade-ar9341.img
```

期望输出示例：
```
Using eth0 device
TFTP from server <PC端IP>; our IP address is <设备IP>
Filename 'artosyn-upgrade-ar9341.img'.
Load address: 0x24450000
Loading: ##########  10 MiB/s
done
Bytes transferred = 8388608 (800000 hex)
```

#### 6.1.5 执行升级

```
=> artosyn_upgrade dram
```

期望输出（成功）：
```
uboot upgrade from ddr
uboot upgrade finish, please reset the board...
```

期望输出（失败）：
```
uboot upgrade from ddr
uboot upgrade failed, please check your image or udisk and upgrade again...
```

#### 6.1.6 重启设备

升级成功后，手动重启设备（断电重启或按复位键）。

### 6.2 PC 端操作

#### 6.2.1 搭建 TFTP 服务器

在 PC 上启动 TFTP 服务器，将升级镜像文件（如 `artosyn-upgrade-ar9341.img`）放置在 TFTP 根目录下。

常用 TFTP 服务器软件：
- **Windows**：tftpd32 / tftpd64
- **Linux**：tftp-hpa（`sudo apt install tftpd-hpa`）
- **Python**：`pip install tftpy`（可编程 TFTP 服务器）

#### 6.2.2 配置串口通信

通过串口线缆连接 PC 与设备，使用串口终端工具（如 PuTTY、minicom）或编程库（如 pyserial）进行通信。

串口参数：
- 波特率：115200
- 数据位：8
- 停止位：1
- 校验位：None
- 流控：None

#### 6.2.3 PC 端工具自动化流程

```
1. 启动 TFTP 服务器，监听 69/udp 端口
2. 打开串口，等待设备进入 U-Boot 命令行（检测 "=>" 提示符）
3. 通过串口发送网络配置命令：
   - "setenv ipaddr <设备IP>\n"
   - "setenv serverip <PC端IP>\n"
   - "setenv netmask <子网掩码>\n"
   - "saveenv\n"
4. 通过串口发送 TFTP 加载命令：
   - "tftpboot 0x24450000 artosyn-upgrade-ar9341.img\n"
5. 等待串口输出 "Bytes transferred = " 确认传输完成
6. 通过串口发送升级命令：
   - "artosyn_upgrade dram\n"
7. 监听串口输出判断升级结果：
   - "uboot upgrade finish" → 成功
   - "uboot upgrade failed" → 失败
8. 提示用户重启设备
```

### 6.3 流程图

```
PC 端                                    设备端 (U-Boot)
  |                                          |
  |                          进入 U-Boot 命令行
  |                                          |
  |  串口发送: setenv ipaddr/serverip/...    |
  |----------------------------------------->|
  |                          saveenv 保存环境变量
  |                                          |
  |  串口发送: tftpboot 0x24450000 xxx.img   |
  |----------------------------------------->|
  |          TFTP 传输镜像数据                |
  |<========================================>|
  |                          Bytes transferred = N
  |                                          |
  |  串口发送: artosyn_upgrade dram          |
  |----------------------------------------->|
  |                          uboot upgrade from ddr
  |                          do_upgrade() 执行烧录
  |                                          |
  |                          串口输出结果:
  |                          "uboot upgrade finish, please reset..."
  |                          或
  |                          "uboot upgrade failed, ..."
  |<-----------------------------------------|
  |                                          |
  |                          手动重启设备
```

## 7. PC 端工具开发指南

### 7.1 所需组件

| 组件 | 说明 | 推荐库/工具 |
|------|------|------------|
| TFTP 服务器 | 向设备端提供镜像文件 | tftpd-hpa / tftpy(Python) / TftpServer.NET(C#) |
| 串口通信 | 发送 U-Boot 命令、接收输出 | pyserial(Python) / System.IO.Ports(C#) |
| 镜像文件管理 | 读取升级镜像文件 | 标准文件 I/O |

### 7.2 串口命令发送与响应等待

PC 端工具通过串口发送命令后，需等待 U-Boot 输出提示符 `=>` 确认命令执行完成。

**关键等待标志**：

| 命令 | 成功标志 | 失败标志 |
|------|----------|----------|
| `setenv` | `=>` | 无（setenv 很少失败） |
| `saveenv` | `=>`（输出 `Saving Environment to MMC...`） | 无 |
| `ping` | `host <IP> is alive` | `ping failed; host <IP> is not alive` |
| `tftpboot` | `Bytes transferred = <N>` | `T T T`（超时重试）或 `Load address...` 后无响应 |
| `artosyn_upgrade dram` | `uboot upgrade finish` | `uboot upgrade failed` |

### 7.3 TFTP 传输注意事项

- **TFTP BlockSize**：设备端配置为 1468 字节（`ar9341_emmc_defconfig:772`），PC 端 TFTP 服务器应支持此块大小
- **传输超时**：如网络不稳定，TFTP 可能出现超时重传（串口显示 `T`），PC 端工具应设置合理的超时阈值
- **镜像大小限制**：镜像加载到 `0x24450000`，需确保镜像大小不超过可用 DDR 空间。`IMAGE_DECOMPRESS_ADDR` 为 `0x24400000`，镜像解压在 `0x24400000` 处进行，因此镜像数据区不应与解压区重叠

### 7.4 网络配置注意事项

- **IP 地址冲突**：确保设备 IP 在局域网内唯一
- **防火墙**：PC 端需开放 UDP 69 端口（TFTP）以及 TFTP 数据传输端口
- **直连场景**：如 PC 与设备直连（无交换机），需将 PC 网卡设置为与设备同网段的静态 IP
- **DHCP 场景**：如使用 DHCP，确保 DHCP 服务器能分配 IP 且 TFTP 服务器 IP 与 DHCP 服务器 IP 一致，或通过 `setenv serverip` 单独指定

### 7.5 错误处理

#### 7.5.1 TFTP 传输失败

串口输出 `T T T` 表示 TFTP 超时，可能原因：
- PC 端 TFTP 服务器未启动
- 防火墙阻止 UDP 69 端口
- IP 地址或子网掩码配置错误
- 网线物理连接问题

处理方式：检查网络配置和 TFTP 服务器状态后重试。

#### 7.5.2 镜像验证失败

`do_upgrade` 返回 `ERR_VERIFY`（-1），串口输出 `Verify upgrade image fail`，可能原因：
- 镜像文件损坏或不完整
- 镜像 magic 不为 `0x4152544F`
- 签名验证失败

处理方式：重新获取正确的升级镜像文件。

#### 7.5.3 DDR 配置不匹配

`do_upgrade` 返回 `ERR_DDR`（-7），串口输出 `Verify ddr setting fail`，可能原因：
- 镜像中的 DDR 配置与当前板载 DDR 不匹配

处理方式：
1. 确认镜像是否适用于当前板卡型号
2. 如确需强制升级，在串口执行：
   ```
   => artosyn_upgrade force
   => artosyn_upgrade dram
   ```
   `force` 命令设置 `force_upgrade = true`（`ar_flash.c:3599`），后续 `do_upgrade` 将跳过 DDR 校验（`ar_flash.c:3203-3205`）

> **警告**：强制升级可能导致设备无法启动，仅在确认镜像兼容的情况下使用。

#### 7.5.4 Flash 类型不匹配

`do_upgrade` 返回 `ERR_FLASH_TYPE`（-4），可能原因：
- 镜像 `flashtype` 字段与板载 Flash 类型不一致（如 eMMC 板卡使用了 SPI NOR 镜像）

处理方式：使用与板载 Flash 类型匹配的升级镜像。

## 8. 与 USB HID 升级方案的结合

### 8.1 互为备选方案

TFTP + DDR 方案与 USB HID 方案可互为备选：
- 有以太网接口的板卡优先使用 TFTP + DDR 方案（开发简单、速度快）
- 无以太网接口的板卡使用 USB HID 方案

### 8.2 USB HID 文件传输协议替代 TFTP

USB HID 协议中的文件传输功能（`port_index = 4`，`INDEX_UBOOT_FILETRAN`）也可将镜像传输到 DDR 指定地址，随后调用 `artosyn_upgrade dram` 升级。此方式无需以太网，但需实现 USB HID 协议栈。

流程：
1. PC 端通过 USB HID 发送文件传输启动命令（`"uftr" + file_len + ddr_addr`，`ddr_addr = 0x24450000`）
2. PC 端通过 USB HID 分片发送镜像数据
3. PC 端通过串口发送 `artosyn_upgrade dram` 命令

详见 `uboot_upgrade_hid_protocol.md` 第 5 节"文件传输协议"。

## 9. 相关源码文件

| 文件路径 | 说明 |
|----------|------|
| `board/artosyn/common/ar_flash.c` | `do_artosyn_upgrade` / `do_upgrade` / 错误码定义 |
| `board/artosyn/evb_ar9301/evb_ar9301.c` | `board_eth_init` GMAC 初始化 |
| `include/uboot_upgrade.h` | `IMAGE_LOAD_ADDR` 地址定义 / `upgrade_hdr` 结构体 |
| `configs/ar9341_emmc_defconfig` | 网络 / TFTP / 环境变量配置 |
| `drivers/usb/gadget/f_artousb.c` | USB HID 升级协议（备选方案） |
