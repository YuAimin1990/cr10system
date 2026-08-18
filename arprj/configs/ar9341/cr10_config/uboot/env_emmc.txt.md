# U-Boot Environment Variables Documentation

This document describes the U-Boot environment variables used in the Artosyn 9301 board configuration.

## Environment Variable Reference

### Basic System Configuration
| Variable | Value | Description |
|----------|-------|-------------|
| `baudrate` | 115200 | Serial console baud rate |
| `board` | artosyn | Board identifier |
| `board_name` | artosyn_9301 | Specific board model name |
| `cpu` | armv8 | CPU architecture |
| `soc` | artosyn | System on Chip identifier |
| `bootdelay` | 2 | Delay in seconds before automatic boot |
| `verify` | 1 | Enable verification (1=on, 0=off) |

### Boot Configuration
| Variable | Value | Description |
|----------|-------|-------------|
| `bootargs` | console=ttyS0,115200,earlyprintk... | Linux kernel command line arguments |
| `bootcmd` | (see below) | Commands executed automatically at boot |
| `bootm_boot_mode` | nosec | Boot mode (non-secure) |
| `fdt_high` | 0xffffffffffffffff | Maximum address for device tree blob |

### Network Configuration
| Variable | Value | Description |
|----------|-------|-------------|
| `ethact` | sirius-gmac | Active Ethernet interface |
| `ethaddr` | 00:00:01:02:03:04 | MAC address for the Ethernet interface |
| `ethprime` | sirius-gmac | Primary Ethernet interface |
| `ipaddr` | 192.168.199.138 | Device IP address |
| `netmask` | 255.255.255.0 | Network mask |
| `serverip` | 192.168.199.114 | TFTP server IP address |

### Console Configuration
| Variable | Value | Description |
|----------|-------|-------------|
| `stderr` | serial | Standard error output device |
| `stdin` | serial | Standard input device |
| `stdout` | serial | Standard output device |

### Test Variable
| Variable | Value | Description |
|----------|-------|-------------|
| `test1` | 1234 | Test variable (likely for debugging) |

## Detailed Descriptions

### `bootargs` 参数详解

`bootargs` 是传递给 Linux 内核的启动参数，用于配置内核启动时的各种行为和系统设置：

```plaintext
console=ttyS0,115200,earlyprintk loglevel=8,quiet root=/dev/mmcblk0p7 rootwait ro rootfstype=ext4 gpt mem=512m flagfile=/usrdata/sirius-clean-system-flag isolcpus=3
```

#### 详细说明：

1. **控制台设置**
   - `console=ttyS0,115200`：指定控制台为第一个串口(ttyS0)，波特率115200
   - `earlyprintk`：在内核早期启动阶段就启用打印输出，方便调试启动问题

2. **日志级别**
   - `loglevel=8,quiet`：设置内核日志级别为8（调试级别），但使用quiet参数禁止大多数控制台消息

3. **根文件系统**
   - `root=/dev/mmcblk0p7`：指定根文件系统位于MMC设备的第7个分区
   - `rootwait`：如果根设备未就绪，则等待
   - `ro`：以只读方式挂载根文件系统（提高系统稳定性）
   - `rootfstype=ext4`：指定根文件系统类型为ext4

4. **系统配置**
   - `gpt`：使用GPT分区表
   - `mem=512m`：设置系统内存为512MB
   - `flagfile=/usrdata/sirius-clean-system-flag`：指定系统标志文件路径
   - `isolcpus=3`：将CPU核心3从内核调度器中隔离出来（用于特定任务或实时应用）

### `bootcmd` 参数详解

`bootcmd` 是U-Boot启动时自动执行的命令序列：

```plaintext
setenv kernel_comp_addr 0x30000000; 
setenv kernel_comp_size 0x800000; 
part number mmc 0 logo logo_part; 
part start mmc 0 $logo_part logo_start; 
part size mmc 0 $logo_part logo_size; 
mmc dev 0 0; 
part number mmc 0 kernel kernel_part; 
part start mmc 0 $kernel_part kernel_start; 
part size mmc 0 $kernel_part kernel_size; 
part number mmc 0 dtb dtb_part; 
part start mmc 0 $dtb_part dtb_start; 
part size mmc 0 $dtb_part dtb_size; 
mmc read 0x22400000 $logo_start $logo_size; 
vo startdev 0 3 3 3; 
vo startlayer 0 0x22400000 720 0 0 720 1440 0; 
mmc read 0x30000000 $kernel_start $kernel_size; 
mmc read 0x22080000 $dtb_start $dtb_size; 
bootm 0x30000000 - 0x22080000
```

#### 详细执行流程：

1. **设置内核解压参数**
   ```plaintext
   setenv kernel_comp_addr 0x30000000
   setenv kernel_comp_size 0x800000
   ```
   - 设置内核解压地址为0x30000000
   - 设置内核解压大小为8MB

2. **处理Logo分区**
   ```plaintext
   part number mmc 0 logo logo_part
   part start mmc 0 $logo_part logo_start
   part size mmc 0 $logo_part logo_size
   ```
   - 获取名为"logo"的分区号，保存到`logo_part`变量
   - 获取logo分区的起始扇区，保存到`logo_start`
   - 获取logo分区的扇区大小，保存到`logo_size`

3. **处理内核分区**
   ```plaintext
   part number mmc 0 kernel kernel_part
   part start mmc 0 $kernel_part kernel_start
   part size mmc 0 $kernel_part kernel_size
   ```
   - 获取名为"kernel"的分区号，保存到`kernel_part`
   - 获取内核分区的起始扇区，保存到`kernel_start`
   - 获取内核分区的扇区大小，保存到`kernel_size`

4. **处理设备树(DTB)分区**
   ```plaintext
   part number mmc 0 dtb dtb_part
   part start mmc 0 $dtb_part dtb_start
   part size mmc 0 $dtb_part dtb_size
   ```
   - 获取名为"dtb"的分区号，保存到`dtb_part`
   - 获取DTB分区的起始扇区，保存到`dtb_start`
   - 获取DTB分区的扇区大小，保存到`dtb_size`

5. **加载并显示Logo**
   ```plaintext
   mmc read 0x22400000 $logo_start $logo_size
   vo startdev 0 3 3 3
   vo startlayer 0 0x22400000 720 0 0 720 1440 0
   ```
   - 从MMC读取logo数据到内存地址0x22400000
   - 初始化显示设备
   - 启动显示层，显示logo图片（分辨率512x960）

6. **加载内核和设备树**
   ```plaintext
   mmc read 0x30000000 $kernel_start $kernel_size
   mmc read 0x22080000 $dtb_start $dtb_size
   ```
   - 从MMC读取内核镜像到内存地址0x30000000
   - 从MMC读取设备树到内存地址0x22080000

7. **启动内核**
   ```plaintext
   bootm 0x30000000 - 0x22080000
   ```
   - 从0x30000000启动内核
   - 使用0x22080000处的设备树
   - 中间的`-`表示不使用initramfs

## Notes
- The environment is specifically configured for an Artosyn 9301 board with MMC storage
- Network settings are pre-configured for a specific local network (192.168.199.x)
- The boot process is optimized to load a Linux kernel with specific memory and CPU isolation settings
