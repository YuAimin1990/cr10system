# ar_logcat

`ar_logcat` 用于从共享内存日志系统中轮询读取日志并打印到标准输出（类似 logcat）。

## 构建

在 `arprj/apps/utils` 顶层执行 `make` 会自动构建；或在本目录执行：

- `make`
- `make clean`

## 运行

```sh
./ar_logcat
```

程序流程（来自源码）：

- `ar_log_init()` 初始化日志读取
- 循环调用 `ar_log_shm_get_log(buf, 512)`
  - 若有数据：直接 `printf` 输出
  - 若无数据：`usleep(500ms)`

## 注意事项

- 需要系统日志共享内存机制已启用且可访问（`utils_dbglog` 相关组件）。
- 默认无参数过滤功能：会把读到的内容原样输出。
