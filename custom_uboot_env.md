# 自定义 U-Boot 环境变量镜像烧录方法

## 1. 方法概述

本文档描述一种在不依赖设备端 `fw_setenv` 工具的情况下，通过 PC 端生成 U-Boot 环境变量镜像并在目标设备上直接烧录的方案。

该方法适用于以下场景：
- Linux 根文件系统中未集成 `fw_printenv` / `fw_setenv` 工具
- 需要在 PC 端批量预配置环境变量后统一烧录
- 需要通过脚本或 CI/CD 流程自动化修改环境变量

## 2. 实现原理

U-Boot 在启动时会从指定的存储分区读取环境变量镜像，并在头部进行 CRC32 校验。只有当校验通过后，才会加载分区中的环境变量；否则回退到编译时内置的默认环境变量。

`mkenvimage` 是 U-Boot 源码树提供的工具，负责将纯文本格式的 `key=value` 文件转换为带 CRC32 头部的二进制镜像。在 PC 端生成该镜像后，通过 `dd` 直接写入设备对应分区，即可被 U-Boot 正确识别。

## 3. 操作步骤

### 3.1 PC 端生成环境变量镜像

#### 3.1.1 准备环境变量文本文件

创建纯文本文件（如 `env_custom.txt`），内容格式如下：

```text
baudrate=115200
bootdelay=1
board=artosyn
```

格式说明：
- 每行一个 `key=value` 对
- 空行会被跳过
- `#` 开头的行为注释，会被跳过
- 变量值中的换行需用 `\` 转义

#### 3.1.2 编译 mkenvimage 工具

如果本地没有该工具，可从 U-Boot 源码编译：

```bash
cd arsdk/platforms/uboot/u-boot-2020.10
cd tools
make mkenvimage
```

编译完成后，`mkenvimage` 位于 `tools/mkenvimage`。

#### 3.1.3 生成镜像

```bash
mkenvimage -s <env_size> -o uboot_env.bin env_custom.txt
```

参数说明：
- `-s <env_size>`：环境变量分区大小，必须与 U-Boot 编译配置 `CONFIG_ENV_SIZE` 一致
- `-o <output>`：输出镜像文件名
- `-r`：（可选）当 U-Boot 启用了 `CONFIG_SYS_REDUNDAND_ENVIRONMENT` 时添加
- `-b`：（可选）目标平台为大端序时添加
- `-p <byte>`：（可选）用指定字节填充镜像空白区域，默认为 `0xff`

> 注意：`CONFIG_ENV_SIZE` 和 `CONFIG_ENV_OFFSET` 需要与 U-Boot 编译配置保持一致
> CONFIG_ENV_SIZE=0x8000
> CONFIG_ENV_OFFSET=0xc0000

示例：

```bash
mkenvimage -s 0x8000 -o uboot_env.bin env_custom.txt
```

### 3.2 设备端烧录镜像

#### 3.2.1 确认环境变量分区

根据板级配置和启动介质，确定环境变量所在分区。对于 eMMC 启动的 cr10 板，通常为 `env` 分区。

可通过以下命令确认分区信息：

```bash
ls /dev/mmcblk0p*
```

或使用 `parted` / `gdisk` 查看分区表。

#### 3.2.2 使用 dd 写入镜像

```bash
dd if=/path/to/uboot_env.bin of=/dev/mmcblk0pX bs=4K status=progress
sync
```

其中 `/dev/mmcblk0pX` 为实际的环境变量分区设备节点。

写入完成后建议执行 `sync` 确保数据落盘。

## 4. 关键注意事项

### 4.1 镜像大小与分区大小匹配

- `mkenvimage` 的 `-s` 参数必须严格等于 U-Boot 配置中的 `CONFIG_ENV_SIZE`
- 镜像可以小于分区容量，`dd` 写入后剩余区域保持原数据，不影响 CRC 校验
- 镜像绝对不可大于分区容量，否则会导致数据截断和 CRC 校验失败

### 4.2 冗余环境变量支持

如果 U-Boot 启用了 `CONFIG_SYS_REDUNDAND_ENVIRONMENT`：
- 生成镜像时必须添加 `-r` 参数
- 部分方案会在两个独立分区各存一份 env，需分别写入

### 4.3 CRC32 校验机制

- 直接修改分区中的文本内容而不重新生成 CRC 头部，会导致 U-Boot 校验失败
- 校验失败后 U-Boot 会自动回退到编译时内置的默认环境变量
- 该操作不会导致系统变砖，但修改不会生效

### 4.4 多设备批量烧录

对于产线批量生产场景，建议：
1. 在 PC 端统一生成标准环境变量镜像
2. 通过 TFTP / Fastboot / SD 卡等方式下发到设备
3. 在设备端通过脚本自动执行 `dd` 烧录

## 5. 验证方法

### 5.1 验证镜像生成是否正确

使用 `hexdump` 查看镜像头部 4 字节是否为有效的 CRC32 值：

```bash
hexdump -C uboot_env.bin | head -n 1
```

前 4 字节应为非零的 CRC32 校验值（小端序存储）。

### 5.2 验证设备端烧录是否成功

重启设备并进入 U-Boot 命令行（如有 `bootdelay`），执行：

```uboot
printenv
```

检查输出中是否包含自定义的环境变量。

## 6. 常见问题

### 6.1 CRC 错误导致环境变量回退

**现象**：修改后重启，环境变量未生效，U-Boot 提示 `CRC Error`。

**原因**：直接修改了分区内容但未重新生成 CRC 头部。

**解决**：重新使用 `mkenvimage` 生成完整镜像并烧录。

### 6.2 镜像大小配置错误

**现象**：U-Boot 提示环境变量损坏或无法读取。

**原因**：`mkenvimage -s` 参数与 `CONFIG_ENV_SIZE` 不匹配。

**解决**：核对 U-Boot 配置中的环境变量分区大小，重新生成镜像。

## 7. 相关文件路径

- U-Boot 环境变量配置模板：`arprj/configs/ar9341/cr10_config/uboot/env_emmc.txt`
- `mkenvimage` 源码：`arsdk/platforms/uboot/u-boot-2020.10/tools/mkenvimage.c`
- `fw_env.config` 模板：`arsdk/platforms/uboot/u-boot-2020.10/tools/env/fw_env.config`
