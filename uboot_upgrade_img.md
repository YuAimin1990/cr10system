# U-Boot 系统映像升级指南

本文档描述 AR9341 平台在 U-Boot 阶段进行系统映像升级的几种方法。

涉及的 U-Boot 命令：

- `artosyn_upgrade` — 从 U 盘（USB Mass Storage）读取镜像升级
- `artosyn_hid_upgrade` — 通过 USB 线（HID 协议）由 PC 端推送镜像升级
- `artosyn_upgrade dram` — 从 DDR 内存中加载的镜像升级

升级镜像文件位于：

- `arprj/release/prebuilds/image/artosyn-upgrade-ar9341.img`

## 1. 从 U 盘升级（artosyn_upgrade）

### 1.1 命令用法

```
=> artosyn_upgrade              # 从 U 盘读取镜像升级
=> artosyn_upgrade test         # 升级测试模式，循环执行，按 Ctrl+C 退出
=> artosyn_upgrade force        # 强制升级（当 DDR 配置变化时）
=> artosyn_upgrade dram         # 从 DDR 升级（需事先将镜像加载到 IMAGE_LOAD_ADDR）
```

帮助信息源码定义在 `arsdk/platforms/uboot/u-boot-2020.10/board/artosyn/common/ar_flash.c:3630-3635`。

### 1.2 U 盘要求

- **文件系统格式**：FAT16 或 FAT32（代码中使用 `fs_set_blk_dev("usb", "0", FS_TYPE_FAT)` 设置文件系统设备）
- **镜像文件名**：U 盘根目录下必须存在文件名匹配 `artosyn-upgrade-ar9341*.img` 的镜像文件
- **镜像来源**：使用 SDK 预编译的 `arprj/release/prebuilds/image/artosyn-upgrade-ar9341.img`

### 1.3 操作步骤

1. 在 PC 上将 U 盘格式化为 FAT32
2. 将 `artosyn-upgrade-ar9341.img` 拷贝到 U 盘根目录
3. 将 U 盘插入设备的 USB 端口
4. 进入 U-Boot 命令行，执行：
   ```
   => artosyn_upgrade
   ```
5. 成功后提示 `spl upgrade finish, please reset the board...`，重启设备即可

### 1.4 升级流程（源码分析）

升级流程由 `artosyn_spl_upgrade()` 函数实现（`ar_flash.c:3505-3578`）：

1. **USB 设备初始化**（`do_upgrade_dev_init`, 行 3341）：
   - 调用 `usb_stop()` → `usb_init()` → `usb_stor_scan(1)` 扫描 USB 存储设备
   - 通过 `blk_get_devnum_by_type(IF_TYPE_USB, ...)` 获取存储设备
   - 调用 `fs_set_usb_dev()` 设置 USB 设备为 FAT 文件系统（行 3319-3339）

2. **加载升级镜像**（`do_upgrade_load_img`, 行 3375）：
   - 扫描 U 盘根目录 `/` 下的文件
   - 查找文件名匹配 `artosyn-upgrade-ar9341*.img` 的文件（行 3418）
   - 读取镜像到 `IMAGE_LOAD_ADDR`（`0x24450000`）
   - 校验镜像头部的 `img_size` 字段（行 3490）

3. **执行升级**（`do_upgrade`, 行 3550）：
   - 解析镜像头部信息（分区表、段信息等）
   - 按分区写入 flash

### 1.5 常见问题

#### 1.5.1 USB storage device not found

**现象**：

```
scanning bus usb@011c0000 for devices... 1 USB Device(s) found
scanning bus usb@01200000 for devices... 1 USB Device(s) found
scanning bus usb@01240000 for devices... 1 USB Device(s) found
       scanning usb for storage devices... 0 Storage Device(s) found
Error: USB storage device not found
```

**原因**：USB 总线检测到设备，但未被识别为 Mass Storage 设备。

**排查步骤**：

- 确认使用的是普通 U 盘（USB Mass Storage Class），非手机、MTP 设备等
- 确认 U 盘为 FAT 格式（FAT16/FAT32），NTFS 或 exFAT 无法识别
- 尝试更换其他品牌/型号的 U 盘
- 尝试不同的 USB 端口
- 确认 U 盘已正确插入，等待 2-3 秒后再执行命令

#### 1.5.2 no upgrade file find

**原因**：U 盘根目录下未找到文件名匹配 `artosyn-upgrade-ar9341*.img` 的文件。

**解决**：确认镜像文件名正确，且位于 U 盘根目录（非子目录）。

### 1.6 force 参数

当升级镜像中的 DDR 配置与当前板载 DDR 不匹配时，`do_upgrade()` 会返回 `ERR_DDR`，此时提示：

```
DDR change!!! reset or press 'f' to force upgrade......
```

可以按 `f` 键强制升级，或预先使用 `artosyn_upgrade force` 命令跳过 DDR 检查。

## 2. 通过 USB 线升级（artosyn_hid_upgrade）

### 2.1 命令用法

```
=> artosyn_hid_upgrade <USB_controller>
```

例如：

```
=> artosyn_hid_upgrade 0
```

其中参数 `0` 为 USB 控制器编号。

帮助信息源码定义在 `arsdk/platforms/uboot/u-boot-2020.10/drivers/usb/gadget/f_artousb.c:827-829`。

### 2.2 工作原理

此命令使用 USB HID 协议，设备作为 USB Device（gadget），由 PC 端工具通过 USB 线推送升级镜像。

与 `artosyn_upgrade`（U 盘升级）的区别：

- `artosyn_upgrade`：设备作为 USB Host，读取 U 盘中的镜像文件
- `artosyn_hid_upgrade`：设备作为 USB Device（HID gadget），由 PC 端工具推送镜像

### 2.3 操作步骤

1. 用 USB 线连接设备与 PC
2. 在 U-Boot 命令行执行：
   ```
   => artosyn_hid_upgrade 0
   ```
   设备显示 `usb_booting enter!!!` 并等待 PC 端发送数据
3. 在 PC 端运行 UsbUpgradeTool93 工具（位于 SDK 根目录 `UsbUpgradeTool93-V1.1.5.zip`），选择 `artosyn-upgrade-ar9341.img` 镜像文件，开始升级
4. 升级完成后设备提示 `artousb upgrade finish, please reset the board...`，重启设备即可

### 2.4 升级流程（源码分析）

升级流程由 `artosyn_hid_upgrade()` 和 `artousb_sys_upgrade_recv()` 实现（`f_artousb.c`）：

1. **设备端初始化**（`artosyn_hid_upgrade`, 行 762-805）：
   - 调用 `usb_gadget_initialize(index)` 初始化 USB gadget 控制器
   - 注册为 `usb_dnl_artousb` 设备（HID 类）
   - 进入循环等待，处理 USB 中断（行 785-793）

2. **PC 端发送启动标记**（`artousb_sys_upgrade_recv`, 行 1055-1093）：
   - PC 端发送 `"upgd"` 标记 + 镜像总长度（4 字节）
   - 设备端校验标记后分配接收缓冲区

3. **PC 端分片发送镜像数据**（行 1101-1110）：
   - 设备端将数据拷贝到缓冲区，累计偏移量
   - 当接收数据达到总长度时，标记下载完成

4. **执行升级**（`artousb_upgrade_from_ram`, 行 1023-1053）：
   - 调用 `do_upgrade()` 将接收到的镜像写入 flash
   - 同样支持 DDR 变化时的 force 升级机制

## 3. 从 DDR 升级（artosyn_upgrade dram）

### 3.1 命令用法

```
=> artosyn_upgrade dram
```

### 3.2 工作原理

此模式不从 U 盘读取镜像，而是直接从 DDR 内存地址 `IMAGE_LOAD_ADDR`（`0x24450000`）处加载镜像进行升级。

使用前需通过其他方式（如 TFTP、USB 文件传输等）将镜像加载到该地址。

### 3.3 操作步骤

1. 通过 TFTP 或其他方式将 `artosyn-upgrade-ar9341.img` 加载到 DDR 地址 `0x24450000`：
   ```
   => tftpboot 0x24450000 artosyn-upgrade-ar9341.img
   ```
2. 执行升级：
   ```
   => artosyn_upgrade dram
   ```
3. 成功后提示 `uboot upgrade finish, please reset the board...`，重启设备即可

### 3.4 关键地址定义

地址定义在 `arsdk/platforms/uboot/u-boot-2020.10/include/uboot_upgrade.h`：

- `IMAGE_DECOMPRESS_ADDR`：`0x24400000`（镜像解压地址）
- `COMPR_SEG_SIZE`：`0x500000`（5MB，压缩段大小）
- `IMAGE_LOAD_ADDR`：`IMAGE_DECOMPRESS_ADDR + COMPR_SEG_SIZE` = `0x24450000`（镜像加载地址）

## 4. 升级镜像格式

升级镜像为 Artosyn 自定义格式，包含 256 字节的签名头部。头部结构定义在 `uboot_upgrade.h:30-51`：

- **magic**：`0x4152544f`（"ARTO"）
- **flashtype**：0=SPI NOR, 1=eMMC, 2=SPI NAND
- **part_status**：分区是否变更
- **img_size**：签名后的镜像数据大小
- **partitions**：分区数量
- **segments**：段数量

镜像头部后包含分区表（`struct part_info`）和段表（`struct segment_info`），`do_upgrade()` 根据这些信息将镜像数据写入对应 flash 分区。

## 5. 升级方式对比

| 特性 | artosyn_upgrade | artosyn_hid_upgrade | artosyn_upgrade dram |
|------|----------------|---------------------|----------------------|
| 数据来源 | U 盘（FAT） | PC 端 USB 线推送 | DDR 内存 |
| USB 角色 | Host | Device（HID gadget） | 无关 |
| 需要额外工具 | 否 | UsbUpgradeTool93 | TFTP 或其他加载方式 |
| 镜像文件名要求 | `artosyn-upgrade-ar9341*.img` | 无（由工具发送） | 无（已在内存中） |
| 适用场景 | 现场升级、无 PC | 产线、有 PC 环境 | 调试、开发 |

## 6. 相关源码文件

- `arsdk/platforms/uboot/u-boot-2020.10/board/artosyn/common/ar_flash.c` — U 盘升级和 DDR 升级实现
- `arsdk/platforms/uboot/u-boot-2020.10/drivers/usb/gadget/f_artousb.c` — USB HID 升级实现
- `arsdk/platforms/uboot/u-boot-2020.10/include/uboot_upgrade.h` — 升级镜像格式定义
- `arprj/release/prebuilds/image/artosyn-upgrade-ar9341.img` — 预编译升级镜像
- `UsbUpgradeTool93-V1.1.5.zip` — PC 端 USB 升级工具
