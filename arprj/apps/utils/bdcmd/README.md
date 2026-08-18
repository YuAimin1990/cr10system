# bdcmd

`bdcmd` 通过 binder IPC 向指定服务发送一段字符串命令（transaction code 固定为 `0xbdc`）。

## 构建

在 `arprj/apps/utils` 顶层执行 `make` 会自动构建；或在本目录执行：

- `make`
- `make clean`

## 运行

```sh
./bdcmd <binder_service_name> <cmd...>
```

程序行为（来自源码）：

- 第一个参数 `argv[1]` 作为 `binder_server`（service name）
- 把后续所有参数拼成命令字符串（长度上限约 1024）
- `fork()` 子进程执行：
  - `ar_ipc_create()`
  - `ar_ipc_get_service(ipc, binder_server)`
  - `ar_ipc_send_buffer(ipc, handle, 0xbdc, cmd_buf, total_len, &reply, 0)`

## 示例

```sh
./bdcmd my.service ping
./bdcmd my.service set foo=1
```

## 注意事项

- 依赖 binder IPC 基础设施和目标服务已注册。
- 如果 `get binder server failed`，说明 service name 不存在或 binder 环境未就绪。
