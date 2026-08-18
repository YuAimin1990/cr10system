# arprj/apps/utils

本目录包含一组用于 AR9341 平台的系统/调试/生产工具（utilities）。顶层 `Makefile` 会递归进入各子目录构建对应可执行程序，并将产物安装到 SDK 的 `$(OUTPUT_APPLICATION_BIN_DIR)`（通常对应 release 的 `prebuilds/apps/bin`）。

## 构建

在 `arprj/apps/utils` 目录下：

- `make`
- `make clean`

顶层 `Makefile` 会根据当前 `CHIP`、`CONFIG_BIT` 等配置决定是否包含部分子项目（例如 `ar_dsp_boot`、`dump_dspmsg`）。

## 子项目概览

以下为 `arprj/apps/utils/Makefile` 中列出的主要子项目及用途概述（以各子目录 `Makefile`/源码命名为准）。

### dump_mem

- **产物**: `dump_mem`
- **用途**: 内存转储/调试辅助工具（通常用于把指定地址范围的数据 dump 到文件，便于离线分析）。
- **构建特征**: 简单单文件程序，链接 `pthread`。

### load_mem

- **产物**: `load_mem`
- **用途**: 内存写入/加载类工具（通常用于把文件/数据写入指定地址进行调试验证）。
- **构建特征**: 简单单文件程序。

### ddr_loading

- **产物**: `ddr_loading`
- **用途**: DDR 相关加载/初始化/辅助调试工具。
- **依赖**: 典型链接 `hal_sys/osal/utils/binder_ipc/rpc_*` 等组件库。

### rtcmd

- **产物**: `rtcmd`
- **用途**: 运行时命令/调试命令工具（常用于向系统服务或中间件发送控制命令）。
- **依赖**: `utils/osal/pthread/rt`。

### servicemanager

- **产物**: `servicemanager`
- **用途**: 平台服务管理进程（负责启动/管理部分服务或提供 binder/service 管理能力）。
- **源码**: `service_manager.c`、`binder.c` 等。

### tunning_gen

- **产物**: 主要是生成调参相关的中间产物（以及可执行程序/脚本配合生成 bin）。
- **用途**: ISP/VIN tuning 参数生成/转换工具链。
- **关键文件**:
  - `convert.sh`: 生成/转换过程脚本
  - `header_to_bin.c`: 将 `inc/*.h` 中的 tuning 结构体初始化数据导出为 `.bin`
  - `inc/`: 各传感器/场景的 tuning header
  - `tuning_ext.c`: 扩展/自定义 tuning 入口（可通过 `gCUSTOM_TUNING` 覆盖）
- **常见输出**: `./bin/*.bin`（由工具生成，具体路径以脚本/源码为准）。

### artosyn_upgrade

- **产物**: `artosyn_upgrade`
- **用途**: 固件/系统升级工具（包含较完整的升级/格式化/镜像处理逻辑）。
- **依赖**: 自带 `lib/` 或 `lib32/`（根据 32/64 bit 选择），并链接 `tomcrypt/tommath/uuid` 等。

### ar_logcat

- **产物**: `ar_logcat`
- **用途**: 日志抓取/查看工具（类似 logcat 的使用体验，便于现场调试）。
- **依赖**: `utils/osal/pthread/rt`。

### bdcmd

- **产物**: `bdcmd`
- **用途**: binder/后台控制相关命令工具（通常用于与 binder_ipc 相关组件交互）。
- **依赖**: `utils/osal/pthread/rt/binder_ipc`。

### icc_file_operation

- **产物**: `icc_file_operation`
- **用途**: ICC（Inter-CPU/Inter-Core Communication）相关文件操作/调试工具。
- **依赖**: `hal_icc` 及 `utils/osal/pthread/rt`。

### ar_top

- **产物**: `ar_top`
- **用途**: 系统监控/资源查看工具（类似 top/平台版 top）。
- **依赖**: `hal_dbglog/cjson/binder_ipc` 等。

### ar_dsp_boot（仅 ar9341）

- **产物**: `ar_dsp_boot`
- **用途**: DSP 启动/加载/初始化辅助工具。
- **依赖**: `hal_dsp/mpi_dsp` 及 `rpc_*` 等。

### dump_dspmsg（仅 ar9341）

- **产物**: `dump_dspmsg`
- **用途**: DSP 消息/日志转储工具。
- **依赖**: `utils/osal/pthread/rt`。

## 备注

- 本目录工具多数用于工程调试、产线生产测试或现场诊断。不同产品线可能只使用其中一部分。
- 若需要把某个工具加入系统启动流程，建议在对应的 scenario 脚本或服务管理流程中集成，并确认依赖库/设备节点在目标系统中可用。
