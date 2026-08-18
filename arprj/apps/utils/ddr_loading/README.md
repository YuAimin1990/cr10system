# ddr_loading

`ddr_loading` 是 DDR/NOC 带宽/压力测试与监控工具。程序会分配 MMZ 内存并通过 DMA 进行高速 copy（多线程），同时读取/配置一组性能监控寄存器，输出 DDR 读写带宽统计。

> 该工具涉及硬件寄存器访问、MMZ 分配、DMA 传输，建议仅在调试/测试环境使用。

## 构建

在 `arprj/apps/utils` 顶层执行 `make` 会自动构建；或在本目录执行：

- `make`
- `make clean`

## 运行

### 查看帮助

```sh
./ddr_loading --help
```

参数（来自源码 `main()`）：

- `--mode <0|1>`
  - `0`: monitor 模式（循环输出带宽）
  - `1`: test 模式（跑一次就退出）
- `--duration <ms>`
  - 默认 `5000`
- `--datasize <bytes>`
  - 说明文字写“默认 16KB”，但源码默认值是 `1000*1024`（约 1000KB）。以实际源码为准。
- `--threads <n>`
  - 默认 `4`
- `--ddrfreq <MB>`
  - 默认 `3200`
- `--nocfreq <MB>`
  - 默认 `600`

程序会捕获 `SIGINT/SIGTERM` 用于停止循环。

## 输出

运行时会打印：

- DDR bitwidth
- 参数摘要（ddrfreq/nocfreq/duration/datasize/threads）
- 每个周期的 `ddr_wr_bw_MB` / `ddr_rd_bw_MB`

## 示例

### 以 monitor 模式每 5 秒输出一次

```sh
./ddr_loading --mode 0 --duration 5000 --threads 4 --datasize 0x8000
```

### 以 test 模式跑一次

```sh
./ddr_loading --mode 1 --duration 5000
```

## 注意事项

- 会申请 MMZ 内存并进行 DMA copy，压力较大。
- 需要相关 HAL/驱动功能正常（MMZ、DMA、寄存器映射）。
- 参数单位：`ddrfreq/nocfreq` 在日志里以 `MB` 表示（按源码打印）。
