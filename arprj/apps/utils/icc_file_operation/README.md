# icc_file_operation

`icc_file_operation` 是一个常驻进程，用于配合 CEVA/协处理器通过 ICC（Inter-Core Communication）请求 Linux 侧进行文件操作。

它会通过 `/dev/mem` 映射共享缓冲区，并通过 `hal_icc` 收发消息，响应对端发来的：打开文件、读取、写入、关闭、获取文件长度等请求。

## 构建

在 `arprj/apps/utils` 顶层执行 `make` 会自动构建；或在本目录执行：

- `make`
- `make clean`

## 运行

```sh
./icc_file_operation
```

程序会创建一个线程并永久循环（源码中 `while(1){sleep(1);}`）。

## 协议概述（以源码为准）

- 消息类型枚举：`OPEN_FILE / GET_FILE_LEN / READ_FILE / WRITE_FILE / CLOSE_FILE`
- 默认仅处理 `core_id=0`
- msg id：`0xff001234 + core_id`
- 会向对端发送 `'R'` 表示 ready

## 注意事项

- 需要 ICC 设备/驱动与 CEVA 侧配合。
- 会访问 `/dev/mem` 并 `mmap` 数据区，通常需要 root 权限或相应权限。
- 文件路径由对端传入，务必在受控环境使用（避免任意文件读写风险）。
