# artosyn_upgrade

`artosyn_upgrade` 用于对设备进行固件/镜像升级，并支持进入 clean system、升级后重启、显示升级进度等模式。

> 该工具会写入 flash/eMMC（例如 `/dev/mtd0` 等），具有破坏性，请谨慎使用。

## 构建

在 `arprj/apps/utils` 顶层执行 `make` 会自动构建；或在本目录执行：

- `make`
- `make clean`

本工具目录内自带依赖库：

- 64bit：`lib/`
- 32bit：`lib32/`

并链接 `tomcrypt/tommath/uuid`。

## 运行

### 查看帮助

```sh
./artosyn_upgrade -h
./artosyn_upgrade --help
```

### 参数（来自源码 `usage()`）

```text
Usage: artosyn_upgrade [OPTION] [image-file]

 -c, --enterclean     reboot the system and enter clean system, no image-file
 -r, --reboot         reboot system
 -h, --help           print this help
 -v, --verbose        display more detail information, such as upgrade progress,
                      and write upgrade status to /factory/upgrade_status_file if factory is a mountpoint
 -s, --sdkversion     display sdk version
 -f, --force          force upgrade if DDR change
 -m, --mmap           read file by mmap
 -p, --product        set upgrade mode: product phase, version rollback not allowed
 -M, --Miniloader     upgrade miniloader
 -u, --update_gpt     update emmc gpt, resize the last partition
```

### 示例（来自源码 `usage()`）

- 普通升级：

```sh
./artosyn_upgrade image-file
```

- 显示升级进度：

```sh
./artosyn_upgrade -v image-file
```

- 进入 clean system：

```sh
./artosyn_upgrade -c
```

- 在 clean system 中升级并重启：

```sh
./artosyn_upgrade -r image-file
```

## 升级状态文件

`-v/--verbose` 模式下会尝试写入：

- `/factory/upgrade_status`

（仅当 `/factory` 是挂载点且可写时）。

## 注意事项

- **高风险操作**：升级会写 flash/eMMC，失败可能导致设备无法启动。
- 确认 image-file 与硬件/分区布局匹配。
- `--force`/`--update_gpt`/`--Miniloader` 等选项用于特定场景，建议仅在明确知道含义时使用。
