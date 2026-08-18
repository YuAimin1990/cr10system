# load_mem

`load_mem` 用于通过 `/dev/mem` 直接对物理地址进行 `memset`/`memcpy`，或把文件内容加载到指定物理地址。

**需要 root 权限**。

## 构建

在 `arprj/apps/utils` 顶层执行 `make` 会自动构建；或在本目录执行：

- `make`
- `make clean`

## 运行

### 查看帮助

```sh
./load_mem --help
```

帮助信息（来自源码）：

- `--memset <dst_addr> <offset> <val> <size>`
  - 例：`--memset 0x80000000 0 0x5c 0x1000`
- `--memcpy <dst_addr> <dst_offset> <src_addr> <src_offset> <size>`
  - 例：`--memcpy 0x80000000 0 0x90000000 0 0x1000`
- `--loadmem <file.bin> <dst_addr> <offset> <max_size>`
  - 例：`--loadmem load.bin 0x90000000 0 0x1000`

### `--loadmem` 细节

- 程序会把文件路径拼成 `/mnt/<file>`（源码 `snprintf(name, 63, "/mnt/%s", argv[index+1]);`）
- `max_size` 仅作为上限：实际写入大小取 `min(file_len, max_size)`

## 示例

### 将 `/mnt/load.bin` 加载到物理地址 `0x90000000`

```sh
./load_mem --loadmem load.bin 0x90000000 0 0x100000
```

### 对一段物理内存做 memset

```sh
./load_mem --memset 0x80000000 0 0x00 0x1000
```

### 从 src 复制到 dst

```sh
./load_mem --memcpy 0x80000000 0 0x90000000 0 0x1000
```

## 注意事项

- **需要 root**：访问 `/dev/mem`。
- **风险**：写错地址可能导致系统异常/死机/数据损坏。
- **对齐**：内部通过页 mmap，对齐逻辑在源码中处理；建议 size 为 4 的倍数。
