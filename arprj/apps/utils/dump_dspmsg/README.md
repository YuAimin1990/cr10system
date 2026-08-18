# dump_dspmsg

`dump_dspmsg` 用于从一段共享内存（默认物理地址 `0x32400000`）读取 DSP 写入的日志消息，并输出到串口/文件，或重定向到系统日志。

该工具会 `mmap /dev/mem`，**需要 root 权限**。

## 构建

在 `arprj/apps/utils` 顶层执行 `make` 会自动构建；或在本目录执行：

- `make`
- `make clean`

## 运行

```sh
./dump_dspmsg -m <mode> -f <filename> -a <print_addr>
```

### 参数（来自源码 `usage()`）

- `-m <mode>`
  - `0`: dump 到串口（`-f` 指定串口设备，如 `/dev/ttyS1`）
  - `1`: dump 到文件（`-f` 指定输出文件路径）
- `-f <filename>`
  - 串口设备或输出文件
- `-a <print_addr>`
  - DSP 日志共享内存物理地址
  - 默认 `0x32400000`
- `-r`
  - redirect：重定向到系统日志（源码中为 `redirect=1`，具体输出行为以平台集成为准）

### 示例

#### 输出到串口

```sh
./dump_dspmsg -m 0 -f /dev/ttyS1 -a 0x32400000
```

#### 输出到文件

```sh
./dump_dspmsg -m 1 -f /mnt/uhost/log.txt -a 0x32400000
```

#### 重定向

```sh
./dump_dspmsg -r
```

## 注意事项

- **需要 root**：访问 `/dev/mem`。
- `SIGINT/SIGTERM/SIGQUIT` 等信号会触发退出（源码设置了 `g_stop_flag`）。
- 地址/缓冲区布局由 DSP 固件约定；若 DSP 侧协议变化，需要同步调整工具。
