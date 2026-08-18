# servicemanager

`servicemanager` 是平台 binder/service 管理进程，代码来源于 Android Open Source Project 的 service manager 实现（在本平台上做了适配/裁剪）。

该进程启动后会：

- 打开 binder 设备节点：`/dev/binder`（见 `binder.c:binder_open()`）
- 通过 `BINDER_SET_CONTEXT_MGR` 把自身设置为 **context manager**（见 `service_manager.c:main()`）
- 进入 `binder_loop()` 循环，处理 service 注册/查询/列举请求

## 构建

在 `arprj/apps/utils` 顶层执行 `make` 会自动构建；或在本目录执行：

- `make`
- `make clean`

## 运行

```sh
./servicemanager
```

一般会作为系统基础服务在启动脚本中后台运行：

```sh
servicemanager &
```

## 依赖与前置条件

- **binder 设备节点**：必须存在 `/dev/binder`
- **权限**：需要具备打开 `/dev/binder` 的权限（通常 root 或具备相应权限的用户）
- **唯一性**：同一时间只能有一个进程成功成为 context manager。
  - 如果系统里已经有一个 service manager（或其它进程抢占了 context manager），本进程会失败。

## 如何验证是否启动成功

### 1) 观察启动日志

该程序的错误输出前缀一般为 `svcmgr:`。

- **成功路径**：通常不会打印太多内容，进程会进入循环阻塞在 binder 处理。
- **失败路径**：会打印明确错误（见下文“常见问题”）。

### 2) 检查进程是否存活

在系统上确认 `servicemanager` 进程持续运行（例如通过 `ps`）。

### 3) 验证 binder IPC 是否可用（间接）

当系统中其它使用 binder 的服务/客户端能够正常完成“service 注册/查询”时，通常意味着 `servicemanager` 工作正常。

也可以用 `arprj/apps/utils/bdcmd` 作为一个简单的 binder IPC 验证工具（注意：`bdcmd` 只能证明“能否找到 service 并发起一次 transaction”，并不等价于验证所有 binder 功能都正常）。

示例（将 `my.service` 替换为你系统中真实存在的 binder service name）：

```sh
./bdcmd my.service ping
```

`bdcmd` 典型输出（来自 `bdcmd` 自身的参数拼接打印）：

```text
my.service ping 
```

如果 service name 不存在（或 binder 环境未就绪），常见输出为：

```text
not.exist.service ping 
get binder server failed
```

其中 `get binder server failed` 表示 `ar_ipc_get_service()` 未能获取到该 service 的 handle：

- service 未注册 / 名称拼写错误
- `servicemanager` 未运行或 binder 驱动/设备节点异常

## 停止/重启

- **停止**：向进程发送 `SIGTERM`/`SIGKILL` 或直接 `kill`。
- **重启**：确保旧进程退出后再启动新的 `servicemanager`。

> 提醒：频繁停止/重启可能影响依赖 binder 的上层服务。

## 说明

- 负责处理 binder IPC 的 service 注册/查询/列举等请求。
- 本实现中 SELinux 相关校验逻辑已被注释/弱化（源码中 `selinux_enabled`、`check_mac_perms*` 等均返回允许）。

## 注意事项

## 常见问题与排查

### 1) `binder: cannot open device (...)`

原因：`/dev/binder` 不存在或权限不足。

处理：

- 确认 binder 驱动已加载、设备节点已创建。
- 确认运行用户对 `/dev/binder` 有读写权限。

### 2) `binder: kernel driver version (...) differs from user space version (...)`

原因：binder 内核驱动协议版本与用户态头文件不匹配。

处理：

- 确认内核与用户态 SDK/头文件版本匹配。

### 3) `svcmgr: cannot become context manager (...)`

原因：已有其它进程成为 binder context manager，或者当前进程权限不足。

处理：

- 确认系统里没有重复启动多个 `servicemanager`。
- 确认没有其它组件抢占了 context manager。

## 注意事项

- 需要 binder 驱动/设备节点就绪（具体取决于平台实现）。
- 一般不建议手动频繁启动/停止，通常由系统 init/scenario 脚本管理。
