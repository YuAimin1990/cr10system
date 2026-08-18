# dump_mem

`dump_mem` 用于从物理地址/寄存器空间读取数据，并可选择将数据 dump 到文件或以不同格式打印寄存器值。该工具直接访问 `/dev/mem`，**需要 root 权限**。

## 构建

在 `arprj/apps/utils` 顶层执行 `make` 会自动构建；或在本目录执行：

- `make`
- `make clean`

## 运行

### 查看帮助

```sh
./dump_mem --help
```

### 常用参数（来自源码 `usage()`）

- `--help`
- `--name <file>`
  - dump 文件名。
  - 未指定时默认 `/mnt/dump.bin`
- `--addr <phy_addr> <offset> <len>`
  - 从 `phy_addr + offset` 开始读取 `len` 字节，并写入 `--name` 指定的文件。
- `--reg_WRITE_APB`
  - 以 `` `WRITE_APB(32'hADDR, 32'hDATA);`` 的格式打印寄存器（便于拷贝到脚本/RTL）。
- `--reg`
  - 打印寄存器索引和值。
- `--reg_phy_addr`
  - 打印寄存器索引、物理地址和值。
- `--reg_rd_num <base> <num>`
  - 读取 `base + num*4` 起的一段寄存器并打印（内部会把 `num` 乘以 4）。
- `--reg_rd <addr> <start_bit> <end_bit>`
  - 读取地址并按 bit 域显示（配合源码逻辑）。
- `--reg_wr_num <base> <num> <data>`
  - 以 base+num 的方式写寄存器。
- `--reg_wr <addr> <data> <start_bit> <end_bit>`
  - 按 bit 域写入寄存器。
- `--dump_write_reg32`
  - 以 `write_reg32(0xADDR, 0xDATA);` 的格式打印。

> 说明：本工具部分参数名/行为较“工程内部化”，建议直接阅读 `dump_mem.c` 中参数解析逻辑确认期望。

## 示例

### dump 一段物理内存到文件

```sh
./dump_mem --name /mnt/xxx.bin --addr 0x80000000 0x0 0x1000
```

### 打印一段寄存器为 WRITE_APB 格式

```sh
./dump_mem --addr 0x06000000 0 0x100 --reg_WRITE_APB
```

## 注意事项

- **需要 root**：访问 `/dev/mem`。
- **风险**：错误的地址/写寄存器可能导致系统异常或死机。
- **对齐**：内部按页 mmap，并以 `len>>2` 进行 `fwrite`（按 4 字节写），建议 `len` 为 4 的倍数。
