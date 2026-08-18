# eMMC 分区管理与备份指南

## 分区布局 (基于 flash_layout_emmc.ini)

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

## 使用 dd 管理分区

### 重要提示：写入前的检查

在写入任何分区之前，请先检查目标分区的挂载状态：

```bash
# 检查分区是否已挂载
mount | grep mmcblk0pX

# 如果需要，取消挂载（仅适用于非系统关键分区）
# umount /dev/mmcblk0pX
```

### 基本命令
```bash
# 确保所有缓存数据写入存储
sync

# 执行写入操作
dd if=源文件 of=/dev/mmcblk0pX bs=4M

# 再次同步确保数据写入完成
sync
```

### 单用户模式说明

#### 如何进入单用户模式
1. 在U-Boot启动时中断启动过程
2. 修改内核启动参数，添加 `single` 或 `init=/bin/sh`
3. 启动系统

#### 检查是否在单用户模式
```bash
# 检查运行级别
who -r  # 单用户模式显示 '1' 或 'S'

# 检查挂载情况
mount | grep -v '^sysfs\\|^proc\\|^devpts\\|^tmpfs'
# 通常只挂载了根分区，且为只读

# 检查进程数
ps aux | wc -l  # 单用户模式进程数通常 < 50
```

#### 单用户模式下的操作
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

### 写入不同分区示例

#### 1. 写入 bootloader (SPL)
```bash
dd if=troot_spl.img of=/dev/mmcblk0 bs=512 seek=2 conv=fsync
```

#### 2. 写入 vendor 分区
```bash
dd if=vendor.img of=/dev/mmcblk0p1 bs=4M
```

#### 3. 写入 uboot 分区
```bash
dd if=uboot.img of=/dev/mmcblk0p4 bs=4M
```

#### 4. 写入 kernel 分区

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

#### 5. 写入 dtb 分区
```bash
dd if=artosyn-kernel.dtb of=/dev/mmcblk0p6 bs=4M
```

#### 6. 写入 rootfs (userapp) 分区
```bash
dd if=all_rootfs.ext4 of=/dev/mmcblk0p7 bs=4M
```

#### 7. 写入用户数据分区
```bash
dd if=usr_data.ext4 of=/dev/mmcblk0p8 bs=4M
```

#### 8. 写入启动logo
```bash
dd if=start_logo.yuv of=/dev/mmcblk0p9 bs=1M
```

## 远程备份分区

### 1. 备份关键分区

#### 备份 bootloader (SPL)
```bash
ssh root@device_ip "dd if=/dev/mmcblk0 bs=512 count=512" > bootloader.bin
```

#### 备份 U-Boot 环境
```bash
ssh root@device_ip "dd if=/dev/mmcblk0p3 bs=4M" > uboot_env.bin
```

#### 备份内核和DTB
```bash
ssh root@device_ip "dd if=/dev/mmcblk0p5 bs=4M" > kernel.img
ssh root@device_ip "dd if=/dev/mmcblk0p6 bs=4M" > kernel.dtb
```

#### 备份根文件系统
```bash
# 压缩备份（推荐）
ssh root@device_ip "dd if=/dev/mmcblk0p7 bs=4M | gzip -c" > rootfs.img.gz
```

#### 备份用户数据
```bash
ssh root@device_ip "dd if=/dev/mmcblk0p8 bs=4M | gzip -c" > userdata.img.gz
```

### 2. 备份整个eMMC（不推荐，除非必要）
```bash
# 备份前16MB（包含分区表）
ssh root@device_ip "dd if=/dev/mmcblk0 bs=1M count=16" > emmc_header_16M.bin

# 备份整个eMMC（需要大量空间）
# ssh root@device_ip "dd if=/dev/mmcblk0 bs=4M | gzip -c" > full_emmc_backup.img.gz
```

### 只备份实际使用空间（BusyBox环境）

#### 方法1：使用 df 获取已用空间
```bash
# 获取已用块数（1K块）
USED_BLOCKS=$(ssh username@device_ip "df -k /dev/mmcblk0pX | tail -1 | awk '{print \$3}'")

# 计算需要读取的块数（4K对齐）
BLOCKS_TO_READ=$(( (USED_BLOCKS * 1024 / 4096 + 1) * 4096 / 1024 ))

# 备份实际使用部分
ssh username@device_ip "sudo dd if=/dev/mmcblk0pX bs=1k count=$BLOCKS_TO_READ" > partition_X.img
```

#### 方法2：使用 tune2fs（如果可用）
```bash
# 获取块大小和块数
BLOCK_SIZE=$(ssh username@device_ip "tune2fs -l /dev/mmcblk0pX 2>/dev/null | grep 'Block size' | awk '{print \$3}'")
BLOCK_COUNT=$(ssh username@device_ip "tune2fs -l /dev/mmcblk0pX 2>/dev/null | grep 'Block count' | awk '{print \$3}'")

# 计算需要读取的字节数（4K对齐）
SIZE=$(( (BLOCK_COUNT * BLOCK_SIZE / 4096) * 4096 ))

# 备份
ssh username@device_ip "sudo dd if=/dev/mmcblk0pX bs=4k count=$((SIZE/4096))" > partition_X.img
```

#### 方法3：简单估算（最通用）
```bash
# 直接备份前256MB（根据实际情况调整大小）
ssh username@device_ip "sudo dd if=/dev/mmcblk0pX bs=4M count=64" > partition_X.img
```

## 恢复备份

### 从普通备份恢复
```bash
gunzip -c backup_pX.img.gz | ssh username@device_ip "sudo dd of=/dev/mmcblk0pX bs=4M"
```

### 从部分备份恢复
```bash
# 解压并写入分区
gunzip -c partition_X.img.gz | ssh username@device_ip "sudo dd of=/dev/mmcblk0pX bs=4M"

# 检查并修复文件系统（如果支持）
ssh username@device_ip "sudo fsck -f -y /dev/mmcblk0pX"
```

## 验证备份完整性

```bash
# 在远程设备上计算哈希
remote_hash=$(ssh username@device_ip "sudo dd if=/dev/mmcblk0pX bs=4M 2>/dev/null | md5sum")

# 在本地计算备份文件的哈希
local_hash=$(md5sum backup_file)

# 比较两个哈希值
echo "远程哈希: ${remote_hash%% *}"
echo "本地哈希: ${local_hash%% *}"
```

## 注意事项

1. **权限**：需要 root 权限执行这些操作
2. **空间**：确保有足够的存储空间
3. **网络**：大文件传输时确保网络稳定
4. **验证**：始终验证备份的完整性
5. **安全**：如果包含敏感数据，考虑加密备份
6. **电源**：确保操作过程中不会断电

## 自动备份脚本

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

## 分区详细信息

### eMMC 布局 (总计 1.5GB)
- 总大小: 0x60000000 (1.5GB)
- 块大小: 512 字节

### 分区表 (从 flash_layout_emmc.ini 解析)

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

### 重要说明
1. **bootloader** 位于 eMMC 起始位置，不在分区表中
2. 所有偏移量均为十六进制
3. 升级标记为"是"的分区在系统升级时会被覆盖
4. 修改分区表或 bootloader 需要特别注意，错误的操作可能导致设备无法启动

> 注意：实际设备节点可能因系统配置而异，请使用 `lsblk` 或 `fdisk -l` 确认。
