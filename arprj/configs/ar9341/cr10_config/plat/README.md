# AR9341 CR10 平台配置文件说明

本目录包含AR9341 CR10开发板的平台特定配置文件，主要涉及闪存布局和文件系统组成。这些配置文件在固件构建过程中被使用，决定了最终镜像的分区结构和内容。

## 1. 闪存布局配置 (`flash_layout_emmc.ini`)

此文件定义了eMMC闪存的分区结构、大小和内容。在构建过程中，此配置被用于生成最终的镜像文件。

### 1.1 基本配置

```ini
[flash]
type=emmc
size=0x1c0000000       # eMMC总容量
```

* `type`: 指定闪存类型为eMMC
* `size`: 指定eMMC总容量为0x1c0000000字节（约12GB）

### 1.2 升级配置

```ini
[upgrade]
compress=y            # 升级包是否压缩
version=sirius-0.0.0.1 # 升级包版本
```

* `compress`: 指定升级包是否进行压缩(y=是)
* `version`: 指定升级包的版本号

### 1.3 分区结构

| 分区名称 | 设备名 | 大小 | 路径 | 可升级 | 双分区 | 说明 |
|---------|--------|------|-----|-------|-------|------|
| bootloader | /dev/mmcblk0boot0 | 0x40000 (256KB) | release/prebuilds/uboot/troot_spl.img | 是 | - | SPL引导加载程序 |
| vendor | /dev/mmcblk0p1 | 0x80000 (512KB) | release/prebuilds/uboot/vendor.img | 是 | - | 供应商分区 |
| factory | /dev/mmcblk0p2 | 0x80000 (512KB) | - | 否 | - | 工厂数据分区 |
| env | /dev/mmcblk0p3 | 0x80000 (512KB) | release/prebuilds/uboot/u-boot-env | 是 | 否 | U-Boot环境变量 |
| uboot | /dev/mmcblk0p4 | 0x100000 (1MB) | release/prebuilds/uboot/uboot.img | 是 | 否 | U-Boot主程序 |
| kernel | /dev/mmcblk0p5 | 0x3000000 (48MB) | release/prebuilds/kernel/kernel.img | 是 | - | Linux内核镜像 |
| dtb | /dev/mmcblk0p6 | 0x80000 (512KB) | release/prebuilds/kernel/artosyn-kernel.dtb | 是 | - | 设备树二进制文件 |
| userapp | /dev/mmcblk0p7 | 0x10000000 (256MB) | release/prebuilds/fs/all_rootfs.ext4 | 是 | - | 根文件系统 |
| usr_data | /dev/mmcblk0p8 | 0x28000000 (640MB) | release/prebuilds/fs/usr_data.ext4 | 是 | - | 用户数据分区 |
| logo | /dev/mmcblk0p9 | 0x400000 (4MB) | configs/ar9341/cr10_config/scenario/start_logo_720x1440_stride1024.yuv | 是 | - | 启动Logo |
| backup | /dev/mmcblk0p10 | 0xC800000 (200MB) | - | 否 | - | 程序备份分区（挂载到/backup） |
| storage | /dev/mmcblk0p11 | 0x16B800000 | - | 否 | - | 存储分区（挂载到/data） |

### 1.4 分区参数说明

* `name`: 分区名称
* `size`: 分区大小（十六进制字节）
* `path`: 镜像文件路径
* `upgrade`: 是否支持升级
* `dual_part`: 是否配置为双分区（冗余备份）

### 1.5 分区配置示例

```ini
[partition3]
name=uboot
path=release/prebuilds/uboot/uboot.img
size=0x100000
upgrade=y
dual_part=n
```

## 2. 文件系统配置 (`fs.json`)

此文件定义了根文件系统和用户数据分区的组成内容，构建系统会根据此配置打包文件系统镜像。

### 2.1 根文件系统组件

```json
"all_rootfs": {
    "component_list": [
        "prebuilds/rootfs/rootfs.tar.gz:",
        "out/middlewares/lib/*.so:usr/lib",
        "out/hal/lib/*.so:usr/lib",
        "out/mpp/lib/*.so:usr/lib",
        "out/hal/bin/*:usr/bin",
        "prebuilds/ko:mod",
        "prebuilds/apps/bin/artosyn_upgrade:usr/bin",
        "<BOARD_CONFIG_DIR>/scenario/start*.sh:etc/init.d",
        "<BOARD_CONFIG_DIR>/scenario/wifi_tool/*:var"
    ]
}
```

根文件系统包含：
* 基础根文件系统 (`rootfs.tar.gz`)
* 中间件库文件 (复制到`usr/lib`)
* HAL库文件 (复制到`usr/lib`)
* MPP库文件 (复制到`usr/lib`)
* HAL可执行文件 (复制到`usr/bin`) 
* 内核模块 (复制到`mod`)
* 系统升级工具 (复制到`usr/bin`)
* 启动脚本 (复制到`etc/init.d`)
* WiFi工具 (复制到`var`)

### 2.2 用户数据分区组件

```json
"usr_data": {
    "component_list": [
        "<BOARD_CONFIG_DIR>/scenario/run.sh:",
        "<BOARD_CONFIG_DIR>/scenario/test_vo.yuv:",
        "<BOARD_CONFIG_DIR>/scenario/test.wav:",
        "<BOARD_CONFIG_DIR>/scenario/lib/*:lib",
        "<BOARD_CONFIG_DIR>/../common/tools/*:tools",
        "prebuilds/apps/lib/*.so*:lib",
        "prebuilds/apps/bin:bin",
        "../apps/utils/tunning_gen/bin/imx307_tuning_preview_rgb.bin:local/factory/tunning/cam_imx307",
        "../apps/utils/tunning_gen/bin/imx307_tuning_preview_ir.bin:local/factory/tunning/cam_imx307",
        "<BOARD_CONFIG_DIR>/scenario/codec_factory_cfg.cjson:local/factory"
    ],
    "debug_component": [
        "../configs/common/debug/:tools"
    ]
}
```

用户数据分区包含：
* 运行脚本 (`run.sh`)
* 测试文件 (`test_vo.yuv`, `test.wav`)
* 库文件 (复制到`lib`)
* 通用工具 (复制到`tools`)
* 应用库文件 (复制到`lib`)
* 应用二进制文件 (复制到`bin`)
* 调谐工具 (复制到`local/factory/tunning/cam_imx307`)
* 编解码器工厂配置 (复制到`local/factory`)

调试模式下额外添加：
* 调试工具 (复制到`tools`)

## 3. 配置文件关系

### 3.1 构建流程中的角色

1. `flash_layout_emmc.ini`被`image.mk`使用，生成最终烧录镜像
2. `fs.json`被`fs_img.mk`使用，生成文件系统镜像

构建入口脚本是仓库根目录的 `create_img.sh`，按以下步骤顺序执行：

```
1. 选择工程配置         configs/<project>/board.config → .product_profile
2. 生成 dtb             make -f hosts/makery/make_dtb_rules.mk
3. 生成 fs img          make -f hosts/makery/fs_img.mk BOOT_MEDIA=<flashtype>
4. 生成 uboot env img   make -f hosts/makery/uboot_env.mk BOOT_MEDIA=<flashtype>
5. 生成程序/升级 img    make -f hosts/makery/image.mk BOOT_MEDIA=<flashtype>
```

调用示例：

```bash
./create_img.sh -f emmc -p cr10_config
# -f <flashtype>: nand / nor / emmc
# -p <project>:   configs/ 下的工程目录名
```

### 3.2 分区升级包覆盖 (ROOTFS_UPGRADE_TAR / USERDATA_UPGRADE_TAR)

打包 fs 前，可用外部升级 tar 覆盖更新合并后的分区内容（如更新 WiFi 驱动、证书、密钥、tuning 文件、应用等），再生成映像。

**支持分区**：通过 `mkfs.py` 中的 `FS_UPGRADE_SPECS` 表配置，每条记录为 `(fs_name 关键字, 环境变量名, 默认升级包文件名)`：

| fs_name 关键字 | 环境变量 | 默认升级包（SDK 根目录下） | 匹配的 CR10 分区 |
|---|---|---|---|
| `rootfs`   | `ROOTFS_UPGRADE_TAR`   | `cr10_root_fs_upgrade.tar`   | `all_rootfs` |
| `usr_data` | `USERDATA_UPGRADE_TAR` | `cr10_usrdata_fs_upgrade.tar` | `usr_data` |

> 新增其它分区的升级只需往 `FS_UPGRADE_SPECS` 加一行，并在 `create_img.sh` 调用一次 `setup_fs_upgrade`。

`create_img.sh` 中通过 `setup_fs_upgrade` 函数统一处理：

```bash
setup_fs_upgrade ROOTFS_UPGRADE_TAR   cr10_root_fs_upgrade.tar
setup_fs_upgrade USERDATA_UPGRADE_TAR cr10_usrdata_fs_upgrade.tar
```

每条规则：默认指向 `$(dirname $0)/../../<默认文件名>`（即 SDK 根目录），文件不存在时打印告警并自动 `unset` 变量，不阻塞构建。

实际生效位置（`hosts/makery/mkfs.py` 的 `mk_fs_images()`）：

```
对 fs.json 中每个分区：
  1. combine_fs_source()  按 fs.json 合并源文件到临时目录
  2. apply_fs_upgrade()   遍历 FS_UPGRADE_SPECS，fs_name 匹配关键字且
                          对应环境变量指向的文件存在时，解压覆盖到同一临时目录
  3. creat_fs_image()     将合并后的目录制作成 ext4 镜像
```

升级包格式支持 `.tar` / `.tar.gz` / `.tgz` / `.tbz2` / `.zip` 等（由 `mkfs.py` 的 `extract()` 函数处理）。

### 3.2.1 升级包打包根目录自动剥离

`apply_fs_upgrade()` 在解压前会先用 `tar -tf` 读取升级包第一个成员路径，检测是否从根目录打包（即路径以分区挂载点前缀开头，如 `usrdata/...`）。若是，则自动追加 `--strip-components=1` 剥离该前缀，避免出现 `/usrdata/usrdata/` 这种嵌套路径。

| 分区 fs_name | 挂载点 | 检测前缀 | 从 `/` 打包(含前缀) | 从挂载点打包(无前缀) |
|---|---|---|---|---|
| `all_rootfs` | `/`       | (无) | 直接解压 | 直接解压 |
| `usr_data`   | `/usrdata`| `usrdata` | 自动 strip 1 层 | 直接解压 |

因此 `cr10_usrdata_fs_upgrade.tar` 无论从 `/` 还是 `/usrdata` 打包都能正确写入分区根，不再出现：

```
/usrdata # ls
bin          local        test.wav     tools
lib          run.sh       test_vo.yuv  usrdata      <-- 多余的嵌套目录
/usrdata # ls usrdata/
README.md  app        bin        lib        local      run.sh     tools
```

> 注：检测依据是 tar 第一个成员的路径前缀，要求升级包内路径风格一致（同一包内不要混用 `usrdata/...` 和无前缀路径）。

**常用操作**：

```bash
# 1. 使用默认升级包路径
./create_img.sh -f emmc -p cr10_config

# 2. 指定其它升级包（仅 rootfs）
ROOTFS_UPGRADE_TAR=/path/to/other.tar ./create_img.sh -f emmc -p cr10_config

# 3. 临时禁用某个分区的升级（设为空）
ROOTFS_UPGRADE_TAR= ./create_img.sh -f emmc -p cr10_config

# 4. 升级包不存在时，脚本会打印告警并自动 unset 变量，不阻塞构建
```

**特征文件验证**：

```bash
# rootfs 升级包含 etc/keys/public_key.pem
debugfs -R "stat etc/keys/public_key.pem" release/prebuilds/fs/all_rootfs.ext4

# usr_data 升级包含 usrdata/run.sh
debugfs -R "stat usrdata/run.sh" release/prebuilds/fs/usr_data.ext4
```

### 3.3 ext4 镜像生成与 make_ext4fs bug 回退

`creat_fs_image()` 对 ext2/ext3/ext4 分区生成镜像，**优先使用原始工具链**：

```
1. ./hosts/utility/bin/make_ext4fs -l <partsize>M -s -a system <sys.ext4img> <src_dir>
2. ./hosts/utility/bin/simg2img        <sys.ext4img> <out_file>
3. ./hosts/utility/bin/truncimg        <out_file>
```

**已知问题**：仓库自带的旧版 `make_ext4fs` 二进制在打包含升级包等内容（文件数较多、含大 ko/可执行文件）的 rootfs 时会触发

```
error: do_inode_allocate_indirect: Failed to allocate N blocks
```

实际数据远小于分区容量（如 77MB 内容 vs 256MB 分区）也能复现，是工具 bug。

**自动回退**：上述管线失败时，`mkfs.py` 会清理中间文件并改用系统 `mke2fs -d` 重新生成 raw ext4 镜像，再走 `truncimg` 打孔：

```
mke2fs -t ext4 -b 4096 -L '' -E lazy_itable_init=0 -d <src_dir> <out_file> <partsize>M
./hosts/utility/bin/truncimg <out_file>
```

- 不带升级包的常规构建：保持原 `make_ext4fs` + `simg2img` + `truncimg` 流程不变
- 触发 bug 的场景：自动回退到 `mke2fs -d`，日志可见 `make_ext4fs pipeline failed (...)，fallback to mke2fs -d`

构建主机需安装 `e2fsprogs`（提供 `mke2fs`），通常发行版默认已有。

### 3.4 相关Makefile配置

在主板配置文件中，这些文件的路径通过以下变量指定：

```
gFS_CFG        ?= $(gBOARD_BASE)/plat/fs.json
gIMAGE_INI_FILE ?= $(gBOARD_BASE)/plat/flash_layout_$(BOOT_MEDIA).ini
```

## 4. 配置修改指南

### 4.1 修改闪存布局

1. 调整分区大小时，需确保总和不超过eMMC总容量
2. 修改分区路径时，需确保对应文件存在
3. 添加新分区时，需要按顺序增加分区号
4. 建议保留原有分区结构，仅在必要时修改

### 4.2 修改文件系统组件

1. 文件格式为`源路径:目标路径`，如果目标路径为空则保持原路径
2. 修改组件列表时，确保源文件存在且路径正确
3. `<BOARD_CONFIG_DIR>`会自动替换为当前板级配置目录
4. 添加自定义应用时，应添加到`usr_data`部分

## 5. 调试技巧

### 5.1 闪存布局问题

1. 检查各分区镜像大小是否超过分区大小
2. 确认路径是否正确
3. 查看构建日志中关于镜像生成的部分

### 5.2 文件系统问题

1. 检查各组件是否成功复制到文件系统
2. 使用`tar -tvf rootfs.tar.gz`查看基础根文件系统内容
3. 修改fs.json后，重新执行`make fs_img`命令

## 6. 技术术语解释

本文档包含AR9341 CR10平台相关技术术语的详细解释。

### 6.1 引导加载相关

#### 6.1.1 SPL (Secondary Program Loader)
SPL是二级程序加载器的缩写，是U-Boot引导加载程序的第一阶段。

**主要功能**：
- 初始化最基本的硬件（如内存控制器、时钟等）
- 加载并运行主U-Boot程序
- 实现安全启动的第一阶段验证

**特点**：
- 体积小（通常<256KB）
- 运行在SoC内部SRAM中
- 功能精简，只包含必要的硬件初始化代码

#### 6.1.2 U-Boot
U-Boot（Universal Boot Loader）是一个开源的引导加载程序，广泛用于嵌入式系统。

**主要功能**：
- 硬件初始化
- 加载操作系统内核
- 提供命令行界面进行系统调试和配置
- 支持多种启动介质（eMMC、NAND、SD卡等）

#### 6.1.3 troot (Trusted Root)
在AR9341平台中，`troot`代表"Trusted Root"，是安全启动机制的一部分。

**作用**：
- 作为信任链的根
- 验证后续加载的引导加载程序和内核的完整性和真实性
- 防止未经授权的代码执行

### 6.2 存储相关

#### 6.2.1 eMMC (embedded MultiMediaCard)
嵌入式多媒体卡，是一种嵌入式非易失性存储设备。

**特点**：
- 集成闪存控制器
- 标准MMC接口
- 典型容量从4GB到256GB不等
- 支持分区管理

#### 6.2.2 分区类型

##### 6.2.2.1 bootloader分区
- **位置**：eMMC起始位置
- **内容**：SPL引导加载程序
- **大小**：256KB
- **作用**：系统上电后首先执行的代码

##### 6.2.2.2 vendor分区
- **大小**：512KB
- **内容**：供应商特定的二进制文件和配置
- **作用**：存储OEM/ODM定制内容

##### 6.2.2.3 factory分区
- **大小**：512KB
- **内容**：出厂设置和校准数据
- **特点**：通常不参与OTA升级

##### 6.2.2.4 env分区
- **大小**：512KB
- **内容**：U-Boot环境变量
- **作用**：存储启动参数和配置

### 6.3 系统镜像相关

#### 6.3.1 DTB (Device Tree Blob)
设备树二进制文件，描述硬件配置信息。

**作用**：
- 提供硬件描述信息
- 实现硬件抽象
- 支持同一内核在不同硬件平台上运行

#### 6.3.2 rootfs (Root Filesystem)
根文件系统，包含系统运行所需的所有文件和目录。

**典型内容**：
- 系统命令（/bin, /sbin）
- 配置文件（/etc）
- 库文件（/lib）
- 设备节点（/dev）
- 临时文件（/tmp）

### 6.4 安全相关

#### 6.4.1 安全启动 (Secure Boot)
一种确保只有受信任的软件可以在设备上运行的机制。

**主要功能**：
- 验证引导加载程序的数字签名
- 防止执行未经授权的代码
- 保护系统完整性

#### 6.4.2 双分区 (Dual Partition)
一种冗余设计，用于提高系统可靠性。

**特点**：
- 维护两个相同的分区
- 当前活动分区升级失败时回退到备份分区
- 常用于关键系统分区（如bootloader）

### 6.5 调试相关

#### 6.5.1 串口控制台 (UART Console)
通过串行接口与设备通信的调试接口。

**用途**：
- 查看系统启动日志
- 访问U-Boot命令行
- 调试内核启动问题

#### 6.5.2 日志级别 (Log Level)
控制日志输出详细程度的设置。

**常见级别**：
- 0: 紧急（Emergency）
- 1: 警报（Alert）
- 2: 严重（Critical）
- 3: 错误（Error）
- 4: 警告（Warning）
- 5: 通知（Notice）
- 6: 信息（Informational）
- 7: 调试（Debug）

### 6.6 网络相关

#### 6.6.1 MAC地址 (Media Access Control Address)
网络接口的物理地址。

**格式**：
- 48位（6字节）
- 通常表示为12个十六进制数字，用冒号或连字符分隔
- 示例：`00:11:22:33:44:55`

#### 6.6.2 IP地址 (Internet Protocol Address)
用于在网络中标识设备的逻辑地址。

**IPv4格式**：
- 32位地址
- 点分十进制表示法（如192.168.1.1）
- 私有地址范围：
  - 10.0.0.0 - 10.255.255.255
  - 172.16.0.0 - 172.31.255.255
  - 192.168.0.0 - 192.168.255.255

## 7. eMMC 分区管理与备份指南

### 7.1 分区布局 (基于 flash_layout_emmc.ini)

| 分区 | 设备节点 | 挂载点 | 大小 | 用途 | 升级标记 |
|------|----------|--------|------|------|----------|
| bootloader | /dev/mmcblk0boot0 | - | 256KB | 启动加载器 | 是 |
| vendor | /dev/mmcblk0p1 | - | 512KB | 厂商信息 | 是 |
| factory | /dev/mmcblk0p2 | /factory | 512KB | 出厂设置 | 否 |
| env | /dev/mmcblk0p3 | - | 512KB | U-Boot 环境变量 | 是 |
| uboot | /dev/mmcblk0p4 | - | 1MB | U-Boot 主程序 | 是 |
| kernel | /dev/mmcblk0p5 | - | 48MB | Linux 内核 | 是 |
| dtb | /dev/mmcblk0p6 | - | 512KB | 设备树 | 是 |
| userapp | /dev/mmcblk0p7 | / | 256MB | 根文件系统 | 是 |
| usr_data | /dev/mmcblk0p8 | /usrdata | 640MB | 用户数据 | 是 |
| logo | /dev/mmcblk0p9 | - | 4MB | 启动logo | 是 |
| backup | /dev/mmcblk0p10 | /backup | 200MB | 程序备份 | 否 |
| storage | /dev/mmcblk0p11 | /data | 0x16B800000 | 存储分区 | 否 |

> 注意：实际挂载情况可能因系统配置而异，请使用 `mount` 或 `df -h` 命令确认。

### 7.2 使用 dd 管理分区

#### 7.2.1 重要提示：写入前的检查

在写入任何分区之前，请先检查目标分区的挂载状态：

```bash
# 检查分区是否已挂载
mount | grep mmcblk0pX

# 如果需要，取消挂载（仅适用于非系统关键分区）
# umount /dev/mmcblk0pX
```

#### 7.2.2 基本命令
```bash
# 确保所有缓存数据写入存储
sync

# 执行写入操作
dd if=源文件 of=/dev/mmcblk0pX bs=4M

# 再次同步确保数据写入完成
sync
```

#### 7.2.3 单用户模式说明

##### 7.2.3.1 如何进入单用户模式
1. 在U-Boot启动时中断启动过程
2. 修改内核启动参数，添加 `single` 或 `init=/bin/sh`
3. 启动系统

##### 7.2.3.2 检查是否在单用户模式
```bash
# 检查运行级别
who -r  # 单用户模式显示 '1' 或 'S'

# 检查挂载情况
mount | grep -v '^sysfs\\|^proc\\|^devpts\\|^tmpfs'
# 通常只挂载了根分区，且为只读

# 检查进程数
ps aux | wc -l  # 单用户模式进程数通常 < 50
```

##### 7.2.3.3 单用户模式下的操作
```bash
# 1. 重新挂载根分区为读写
mount -o remount,rw /

# 2. 挂载必要的文件系统
mount -a  # 挂载 /etc/fstab 中的所有文件系统
mount /usr  # 如果需要挂载其他分区

# 3. 执行维护操作
# ...
```

> **注意**：
> 1. 对于kernel、uboot等启动相关分区，通常不需要也不应该被挂载
> 2. 对于根文件系统(/)等关键分区，建议在恢复模式或单用户模式下操作
> 3. 在单用户模式下，由于大多数服务未运行，文件系统通常没有被占用，但仍需注意：
>    - 根文件系统默认可能是只读的，需要先重新挂载为读写
>    - 其他分区默认未挂载，可以直接操作
> 4. 生产环境中更新关键分区前请务必备份

#### 7.2.4 写入不同分区示例

##### 7.2.4.1 写入 bootloader (SPL)
```bash
dd if=troot_spl.img of=/dev/mmcblk0 bs=512 seek=2 conv=fsync
```

##### 7.2.4.2 写入 vendor 分区
```bash
dd if=vendor.img of=/dev/mmcblk0p1 bs=4M
```

##### 7.2.4.3 写入 uboot 分区
```bash
dd if=uboot.img of=/dev/mmcblk0p4 bs=4M
```

##### 7.2.4.4 写入 kernel 分区

kernel分区通常不会被挂载，因为它包含的是内核二进制文件而不是文件系统。

```bash
# 检查分区是否被挂载（正常情况下应该没有输出）
mount | grep mmcblk0p5 || echo "分区未挂载，可以安全写入"

# 写入内核镜像
sync
dd if=kernel.img of=/dev/mmcblk0p5 bs=4M
sync

# 验证写入（可选）
cmp kernel.img /dev/mmcblk0p5
```

> **重要**：
> - 更新内核前建议先备份原有内核
> - 更新后建议重启设备验证新内核是否正常启动

##### 7.2.4.5 写入 dtb 分区
```bash
dd if=artosyn-kernel.dtb of=/dev/mmcblk0p6 bs=4M
```

##### 7.2.4.6 写入 rootfs (userapp) 分区
```bash
dd if=all_rootfs.ext4 of=/dev/mmcblk0p7 bs=4M
```

##### 7.2.4.7 写入用户数据分区
```bash
dd if=usr_data.ext4 of=/dev/mmcblk0p8 bs=4M
```

##### 7.2.4.8 写入启动logo
```bash
dd if=start_logo_720x1440_stride1024.yuv of=/dev/mmcblk0p9 bs=1M
```

### 7.3 远程备份分区

#### 7.3.1 备份关键分区

##### 7.3.1.1 备份 bootloader (SPL)
```bash
ssh root@device_ip "dd if=/dev/mmcblk0 bs=512 count=512" > bootloader.bin
```

##### 7.3.1.2 备份 U-Boot 环境
```bash
ssh root@device_ip "dd if=/dev/mmcblk0p3 bs=4M" > uboot_env.bin
```

##### 7.3.1.3 备份内核和DTB
```bash
ssh root@device_ip "dd if=/dev/mmcblk0p5 bs=4M" > kernel.img
ssh root@device_ip "dd if=/dev/mmcblk0p6 bs=4M" > kernel.dtb
```

##### 7.3.1.4 备份根文件系统
```bash
# 压缩备份（推荐）
ssh root@device_ip "dd if=/dev/mmcblk0p7 bs=4M | gzip -c" > rootfs.img.gz
```

##### 7.3.1.5 备份用户数据
```bash
ssh root@device_ip "dd if=/dev/mmcblk0p8 bs=4M | gzip -c" > userdata.img.gz
```

#### 7.3.2 备份整个eMMC（不推荐，除非必要）
```bash
# 备份前16MB（包含分区表）
ssh root@device_ip "dd if=/dev/mmcblk0 bs=1M count=16" > emmc_header_16M.bin

# 备份整个eMMC（需要大量空间）
# ssh root@device_ip "dd if=/dev/mmcblk0 bs=4M | gzip -c" > full_emmc_backup.img.gz
```

#### 7.3.3 只备份实际使用空间（BusyBox环境）

##### 7.3.3.1 方法1：使用 df 获取已用空间
```bash
# 获取已用块数（1K块）
USED_BLOCKS=$(ssh username@device_ip "df -k /dev/mmcblk0pX | tail -1 | awk '{print \\$3}'")

# 计算需要读取的块数（4K对齐）
BLOCKS_TO_READ=$(( (USED_BLOCKS * 1024 / 4096 + 1) * 4096 / 1024 ))

# 备份实际使用部分
ssh username@device_ip "sudo dd if=/dev/mmcblk0pX bs=1k count=$BLOCKS_TO_READ" > partition_X.img
```

##### 7.3.3.2 方法2：使用 tune2fs（如果可用）
```bash
# 获取块大小和块数
BLOCK_SIZE=$(ssh username@device_ip "tune2fs -l /dev/mmcblk0pX 2>/dev/null | grep 'Block size' | awk '{print \\$3}'")
BLOCK_COUNT=$(ssh username@device_ip "tune2fs -l /dev/mmcblk0pX 2>/dev/null | grep 'Block count' | awk '{print \\$3}'")

# 计算需要读取的字节数（4K对齐）
SIZE=$(( (BLOCK_COUNT * BLOCK_SIZE / 4096) * 4096 ))

# 备份
ssh username@device_ip "sudo dd if=/dev/mmcblk0pX bs=4k count=$((SIZE/4096))" > partition_X.img
```

##### 7.3.3.3 方法3：简单估算（最通用）
```bash
# 直接备份前256MB（根据实际情况调整大小）
ssh username@device_ip "sudo dd if=/dev/mmcblk0pX bs=4M count=64" > partition_X.img
```

### 7.4 恢复备份

#### 7.4.1 从普通备份恢复
```bash
gunzip -c backup_pX.img.gz | ssh username@device_ip "sudo dd of=/dev/mmcblk0pX bs=4M"
```

#### 7.4.2 从部分备份恢复
```bash
# 解压并写入分区
gunzip -c partition_X.img.gz | ssh username@device_ip "sudo dd of=/dev/mmcblk0pX bs=4M"

# 检查并修复文件系统（如果支持）
ssh username@device_ip "sudo fsck -f -y /dev/mmcblk0pX"
```

### 7.5 验证备份完整性

```bash
# 在远程设备上计算哈希
remote_hash=$(ssh username@device_ip "sudo dd if=/dev/mmcblk0pX bs=4M 2>/dev/null | md5sum")

# 在本地计算备份文件的哈希
local_hash=$(md5sum backup_file)

# 比较两个哈希值
echo "远程哈希: ${remote_hash%% *}"
echo "本地哈希: ${local_hash%% *}"
```

### 7.6 注意事项

1. **权限**：需要 root 权限执行这些操作
2. **空间**：确保有足够的存储空间
3. **网络**：大文件传输时确保网络稳定
4. **验证**：始终验证备份的完整性
5. **安全**：如果包含敏感数据，考虑加密备份
6. **电源**：确保操作过程中不会断电

### 7.7 自动备份脚本

```bash
#!/bin/bash
DEVICE_IP="your_device_ip"
USERNAME="your_username"
BACKUP_DIR="backup_$(date +%Y%m%d_%H%M%S)"

mkdir -p "$BACKUP_DIR"
cd "$BACKUP_DIR" || exit

for part in {1..8}; do
    echo "正在备份分区 mmcblk0p$part..."
    ssh $USERNAME@$DEVICE_IP "sudo e2image -rap /dev/mmcblk0p$part - 2>/dev/null | gzip -c" > "mmcblk0p${part}.img.gz"
    
    # 验证备份（使用md5sum替代sha256sum，因为某些BusyBox可能不支持）
    echo "验证分区 $part..."
    LOCAL_HASH=$(gzip -dc "mmcblk0p${part}.img.gz" | md5sum)
    REMOTE_HASH=$(ssh $USERNAME@$DEVICE_IP "sudo dd if=/dev/mmcblk0p$part bs=4M 2>/dev/null | md5sum")
    
    if [ "${LOCAL_HASH%% *}" = "${REMOTE_HASH%% *}" ]; then
        echo "分区 $part 备份验证成功"
    else
        echo "警告：分区 $part 备份验证失败！"
    fi
done

echo "所有分区备份完成，保存在目录: $BACKUP_DIR"
```

### 7.8 分区详细信息

#### 7.8.1 eMMC 布局
- 总大小: 0x1c0000000
- 块大小: 512 字节

#### 7.8.2 分区表 (从 flash_layout_emmc.ini 解析)

| 分区名称 | 设备节点 | 起始偏移 | 大小 | 挂载点 | 用途 | 升级标记 |
|----------|----------|----------|------|--------|------|----------|
| bootloader | /dev/mmcblk0boot0 | 0x0 | 256KB | - | 启动加载器 | 是 |
| vendor | /dev/mmcblk0p1 | 0x80000 | 512KB | - | 厂商信息 | 是 |
| factory | /dev/mmcblk0p2 | 0x100000 | 512KB | /factory | 出厂设置 | 否 |
| env | /dev/mmcblk0p3 | 0x180000 | 512KB | - | U-Boot 环境变量 | 是 |
| uboot | /dev/mmcblk0p4 | 0x200000 | 1MB | - | U-Boot 主程序 | 是 |
| kernel | /dev/mmcblk0p5 | 0x300000 | 48MB | - | Linux 内核 | 是 |
| dtb | /dev/mmcblk0p6 | 0x3300000 | 512KB | - | 设备树 | 是 |
| userapp | /dev/mmcblk0p7 | 0x3380000 | 256MB | / | 根文件系统 | 是 |
| usr_data | /dev/mmcblk0p8 | 0x13380000 | 640MB | /usrdata | 用户数据 | 是 |
| logo | /dev/mmcblk0p9 | 0x3B380000 | 4MB | - | 启动logo | 是 |
| backup | /dev/mmcblk0p10 | 0x3B780000 | 200MB | /backup | 程序备份 | 否 |
| storage | /dev/mmcblk0p11 | 0x47F80000 | 0x16B800000 | /data | 存储分区 | 否 |

#### 7.8.3 重要说明
1. **bootloader** 位于 eMMC 起始位置，不在分区表中
2. 所有偏移量均为十六进制
3. 升级标记为"是"的分区在系统升级时会被覆盖
4. 修改分区表或 bootloader 需要特别注意，错误的操作可能导致设备无法启动

> 注意：实际设备节点可能因系统配置而异，请使用 `lsblk` 或 `fdisk -l` 确认。
