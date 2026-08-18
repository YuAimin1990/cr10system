# Artosyn AR9341 U-Boot USB HID 升级协议规范

本文档描述 AR9341 平台 U-Boot 阶段通过 USB HID 协议进行系统映像升级的完整通信协议，供 PC 端升级工具开发使用。

源码参考：`arsdk/platforms/uboot/u-boot-2020.10/drivers/usb/gadget/f_artousb.c`

## 1. USB 设备描述

### 1.1 USB 设备信息

设备端通过 `g_dnl_register("usb_dnl_artousb")` 注册为 USB HID 设备，设备描述符定义在 `g_dnl.c` 和 `f_artousb.c` 中。

| 属性 | 值 |
|------|-----|
| idVendor | `0x1D6B`（Linux Foundation） |
| idProduct | `0x0100` |
| Manufacturer | "U-Boot" |
| Product | "USB download gadget" |
| Interface String | "Artosyn Artousb Uboot HID" |
| bDeviceClass | USB_CLASS_PER_INTERFACE (0x00) |
| bcdUSB | 0x0200 (USB 2.0) |

### 1.2 接口描述

| 属性 | 值 |
|------|-----|
| bInterfaceClass | `0x03`（USB_CLASS_HID） |
| bInterfaceSubClass | `0x00` |
| bInterfaceProtocol | `0x00` |
| bNumEndpoints | 2 |

### 1.3 端点描述

设备使用 2 个 Interrupt 端点：

| 端点 | 方向 | 传输类型 | Full-Speed MaxPacketSize | High-Speed MaxPacketSize | bInterval |
|------|------|----------|--------------------------|--------------------------|-----------|
| EP OUT | Device ← Host | Interrupt | 64 字节 | 1024 字节 | 1 |
| EP IN | Device → Host | Interrupt | 64 字节 | 1024 字节 | 1 |

> **注意**：实际 MaxPacketSize 取决于 USB 速度协商结果。Full-Speed 时为 64 字节，High-Speed 时为 1024 字节。PC 端工具应动态获取端点 MaxPacketSize。

### 1.4 HID Report Descriptor

设备暴露以下 HID Report Descriptor（定义在 `f_artousb.c:260-303`）：

```
Usage Page (Vendor Defined 0xFF00)
Usage (0x01)
Collection (Application)

  Report ID 1:  Output, 16 bytes (16 × 8 bits)
  Report ID 2:  Output, 1024 bytes (64 × 128 bits)
  Report ID 3:  Input, 4 bytes (4 × 8 bits)
  Report ID 4:  Input, 64 bytes (64 × 8 bits)

End Collection
```

### 1.5 Report ID 处理（关键）

设备端 HID Report Descriptor 定义了 Report ID 1-4，但 **U-Boot gadget 驱动在端点数据收发时不使用 Report ID 前缀**——直接通过 Interrupt 端点传输原始数据。`artousb_rx_handler`（`f_artousb.c:1361`）期望收到的第一个字节就是 `STRU_USB_HEADER.magic_num[0]`（即 `0xFF`）。

PC 端使用的 API 不同，Report ID 的处理方式也不同：

| API | Report ID 处理 | 发送缓冲区格式 |
|-----|---------------|---------------|
| Windows HID API（`WriteFile` / `ReadFile`） | Windows 自动剥离/添加 1 字节 Report ID | `[ReportID] [STRU_USB_HEADER] [payload]` |
| `HidD_SetOutputReport` | Windows 自动添加 Report ID | `[STRU_USB_HEADER] [payload]`（无需手动加 ReportID） |
| libusb / WinUSB（端点 I/O） | 无 Report ID 层，直接端点传输 | `[STRU_USB_HEADER] [payload]` |
| hidapi 库 | 内部自动处理 Report ID | 取决于具体 API 调用方式 |

**如果使用 `WriteFile` 发送数据**，必须在缓冲区最前面加 1 字节 Report ID（建议用 `0x02`，对应 Report ID 2 = 1024 字节 Output report）：

```
PC 端 WriteFile 缓冲区：
[0x02] [FF 55 FF AA 41 52 54 4F] [header_len] [total_len] ... [payload]
  ↑       ↑ STRU_USB_HEADER.magic_num
  ↑ Report ID (Windows 自动剥离)

设备端 USB 端点实际收到：
[FF 55 FF AA 41 52 54 4F] [header_len] [total_len] ... [payload]
  ↑ STRU_USB_HEADER.magic_num (校验通过)
```

**如果未加 Report ID 前缀直接 `WriteFile`**，Windows 会将 `magic_num[0]`（`0xFF`）当作 Report ID 剥离，设备端收到的第一个字节变为 `0x55`，导致 **`magic num error!`** 错误：

```
PC 端 WriteFile 缓冲区（错误）：
[FF 55 FF AA 41 52 54 4F] [header_len] ...
  ↑ Windows 将此字节当作 Report ID 剥离

设备端 USB 端点实际收到（错误）：
[55 FF AA 41 52 54 4F] [header_len] ...
  ↑ 第一个字节是 0x55，不是 0xFF → magic num error!
```

> **建议**：优先使用 libusb 或 WinUSB 直接进行端点级读写，避免 Report ID 处理的复杂性。如果必须使用 Windows HID API，请务必在 `WriteFile` 缓冲区前加 1 字节 Report ID。

### 1.6 HID 类请求处理

设备端处理以下 HID 类请求（`f_artousb.c:472-576`）：

| 请求 | 方向 | 处理方式 |
|------|------|----------|
| GET_REPORT | IN | 返回空 report |
| GET_PROTOCOL | IN | 返回当前协议（HID_REPORT_PROTOCOL = 1） |
| SET_REPORT | OUT | STALL（不支持） |
| SET_PROTOCOL | OUT | STALL（不支持） |
| SET_IDLE | OUT | 返回空 report（正常响应） |
| GET_DESCRIPTOR(HID) | IN | 返回 HID 类描述符 |
| GET_DESCRIPTOR(REPORT) | IN | 返回 HID Report Descriptor |

## 2. 协议数据结构

### 2.1 USB 传输头部（STRU_USB_HEADER）

每个 USB 端点传输数据包的头部，定义在 `f_artousb.c:88-99`：

```c
#pragma pack(push)
#pragma pack(1)

typedef struct
{
    unsigned char   magic_num[8];     // 魔数，固定为 0xFF,0x55,0xFF,0xAA,0x41,0x52,0x54,0x4F
    unsigned int    header_len;       // 头部长度，固定为 36（sizeof(STRU_USB_HEADER)）
    unsigned int    total_len;        // 整个逻辑数据的总长度（HID_HEADER + HID payload）
    unsigned int    payload_len;      // 本分片的有效载荷长度
    unsigned int    fragment_num;     // 总分片数
    unsigned int    fragment_idx;     // 当前分片索引（从 0 开始）
    unsigned char   sub_proto;        // 子协议号，固定为 1（HID_PROTOCOL）
    unsigned char   reserved[3];      // 保留字段，全 0
    unsigned int    checksum;         // 校验和，payload 部分各字节累加和
} STRU_USB_HEADER;

#pragma pack(pop)
```

**字段说明**：

- **magic_num**：8 字节魔数，固定为 `{0xFF, 0x55, 0xFF, 0xAA, 0x41, 0x52, 0x54, 0x4F}`（即 `\xffU\xff\xaaARTO`）
- **header_len**：固定为 36 字节（即 `sizeof(STRU_USB_HEADER)`）
- **total_len**：逻辑数据总长度 = `sizeof(STRU_HID_HEADER)` + payload 数据长度
- **payload_len**：本分片中 STRU_USB_HEADER 之后的载荷长度
- **fragment_num**：逻辑数据被分成多少个 USB 传输分片
- **fragment_idx**：当前分片索引，从 0 开始
- **sub_proto**：固定为 `0x01`（HID_PROTOCOL）
- **checksum**：对 payload 部分逐字节累加得到的 32 位无符号整数

### 2.2 HID 应用头部（STRU_HID_HEADER）

紧跟在 STRU_USB_HEADER 之后，仅在每个逻辑数据的第一个分片中出现，定义在 `f_artousb.c:101-108`：

```c
#pragma pack(push)
#pragma pack(1)

typedef struct
{
    unsigned short  port_index;   // 端口索引，标识子协议类型
    unsigned short  header_len;   // HID 头部长度，固定为 12（sizeof(STRU_HID_HEADER)）
    unsigned int    data_len;     // 应用数据长度
    unsigned char   status;       // 状态字段
    unsigned char   reserved[3];  // 保留字段，全 0
} STRU_HID_HEADER;

#pragma pack(pop)
```

**字段说明**：

- **port_index**：端口索引，用于路由到不同的回调处理函数（见 2.3 节）
- **header_len**：固定为 12 字节（即 `sizeof(STRU_HID_HEADER)`）
- **data_len**：应用层有效数据的长度（不含 HID 头部和 USB 头部）
- **status**：状态字段，设备端发送时设为 0

### 2.3 端口索引定义

端口索引定义在 `f_artousb.c:66-73`，用于区分不同的功能：

| 常量名 | 值 | 说明 |
|--------|-----|------|
| INDEX_SYS_UPGRADE | 0 | 系统映像升级 |
| INDEX_SYS_FILETRAN | 1 | 系统文件传输（未使用） |
| INDEX_SYS_FILEGET | 2 | 系统文件获取（未使用） |
| INDEX_SYS_USBCMD | 3 | 系统 USB 命令（未使用） |
| INDEX_UBOOT_FILETRAN | 4 | U-Boot 文件传输 |
| INDEX_UBOOT_FILEGET | 5 | U-Boot 文件获取 |
| INDEX_UBOOT_USBCMD | 6 | U-Boot USB 命令（未注册回调） |

当前已注册回调的端口：

| 端口 | 回调函数 | 用途 |
|------|----------|------|
| 0 (INDEX_SYS_UPGRADE) | `artousb_sys_upgrade_recv` | 系统映像升级 |
| 4 (INDEX_UBOOT_FILETRAN) | `artousb_uboot_filetran_recv` | 文件传输到 DDR |
| 5 (INDEX_UBOOT_FILEGET) | `artousb_uboot_fileget_recv` | 从 DDR 读取文件 |

## 3. 分片传输机制

### 3.1 分片规则

由于 USB Interrupt 端点的 MaxPacketSize 限制，较长的逻辑数据需要分片传输。

每个 USB 传输包的格式为：

```
[STRU_USB_HEADER (36 bytes)] [payload data]
```

其中 payload data 的最大长度为：

```
max_payload = report_length - sizeof(STRU_USB_HEADER)
```

- Full-Speed：`max_payload = 64 - 36 = 28` 字节
- High-Speed：`max_payload = 1024 - 36 = 988` 字节

### 3.2 分片计算

对于一个逻辑数据（HID_HEADER + HID payload），总分片数计算方式：

```
total_len = sizeof(STRU_HID_HEADER) + data_len    // = 12 + data_len
fragment_num = ceil(total_len / max_payload)
```

### 3.3 分片数据内容

- **第一个分片（fragment_idx = 0）**：payload 部分包含 `STRU_HID_HEADER` + 部分 HID 数据
- **后续分片（fragment_idx > 0）**：payload 部分仅包含剩余的 HID 数据（无 HID 头部）

### 3.4 设备端分片接收逻辑

设备端接收处理在 `artousb_rx_handler()`（`f_artousb.c:1361-1436`）中：

1. 校验 `magic_num` 是否匹配
2. 校验 `sub_proto` 是否为 `HID_PROTOCOL`（0x01）
3. 校验 `payload_len` 不超过 `report_length`
4. 如果 `fragment_num > 1`，检查 `fragment_idx` 连续性
5. 调用 `hid_protocol_analysis()` 处理 payload
   - 第一个分片：解析 `STRU_HID_HEADER`，根据 `port_index` 路由到对应回调
   - 后续分片：使用之前记录的 `port_index` 直接路由到回调

### 3.5 设备端接收数据长度来源（关键）

**设备端传递给回调函数的数据长度使用 USB 实际接收字节数 `read_len`，而非 `payload_len` 字段。**

具体逻辑：
- `artousb_rx_handler` 获取 `read_len = req->actual`（USB 端点实际接收到的字节数）
- 传给 `hid_protocol_analysis` 的 `len = read_len - sizeof(STRU_USB_HEADER)`（`f_artousb.c:1399-1400`）
- 对第一个分片，回调收到的 `len = len - phid_header->header_len`（`f_artousb.c:1335-1336`）
- 回调函数（如 `artousb_sys_upgrade_recv`）直接使用此 `len` 进行 `memcpy`

**PC 端必须确保**：USB 包实际发送长度 = `sizeof(STRU_USB_HEADER) + payload_len`，即 `payload_len` 字段必须与实际 payload 字节数完全一致，否则会导致数据错位或镜像损坏。

### 3.6 设备端不校验的字段

设备端接收时 **不校验** 以下字段：
- **checksum**：`artousb_rx_handler` 从不读取或验证 `checksum` 字段。PC 端可以填写任意值，但建议仍按规范计算正确值以保持兼容性
- **USB header_len**：设备端不校验 `STRU_USB_HEADER.header_len` 字段
- **HID header_len**：设备端仅校验 `STRU_HID_HEADER.header_len >= sizeof(STRU_HID_HEADER)`（12），不要求精确匹配

## 4. 系统映像升级协议（port_index = 0）

### 4.1 升级流程概述

系统映像升级分为三个阶段：

1. **启动命令**：PC 端发送升级启动命令，包含镜像总长度
2. **数据传输**：PC 端分片发送镜像数据
3. **升级执行**：设备端接收完成后自动执行升级，并通过 HID IN 端点反馈进度和结果

### 4.2 第一步：发送升级启动命令

PC 端发送一个启动命令包，应用层数据格式为：

```
"upgd" (4 bytes) + image_total_len (4 bytes, little-endian unsigned int)
```

应用层数据总长度：8 字节

设备端校验逻辑（`f_artousb.c:1062-1093`）：
- 检查应用层数据长度是否等于 `strlen("upgd") + sizeof(int)` = 8
- 检查前 4 字节是否为 `"upgd"`
- 读取后 4 字节作为镜像总长度

设备端收到启动命令后：
- 设置接收缓冲区为 `IMAGE_LOAD_ADDR`（`0x24450000`）
- 初始化 `image.offset = 0`，`image.total_len = image_total_len`
- 打印 `upgrade receive start! len:<image_total_len>`

### 4.3 第二步：发送镜像数据

PC 端将升级镜像文件分片发送，每个分片作为应用层数据直接发送（无需额外头部）。

设备端处理逻辑（`f_artousb.c:1101-1153`）：
- 每次收到数据后，将数据拷贝到 `image.pimage + image.offset`，拷贝长度为回调收到的 `len`（即 USB 实际接收字节数减去头部）
- `image.offset` 累加本次接收长度
- 当 `image.offset >= image.total_len` 时，标记下载完成，开始执行升级

> **注意**：每个逻辑包的 HID payload 不超过 `max_payload - sizeof(STRU_HID_HEADER)` 字节（High-Speed 下为 976 字节），这样每个逻辑包只需 1 个 USB 传输分片。PC 端需确保每个 USB 包实际发送字节数与 `payload_len` 字段一致。

### 4.4 第三步：设备端执行升级

设备端接收完所有数据后（`f_artousb.c:1112-1151`）：

1. 设置 `ar_image_dl_completed = 1`
2. 调用 `artousb_upgrade_from_ram()` → `do_upgrade()` 执行 flash 烧录
3. 升级过程中通过 `artousb_show_upgrade()` 回调发送进度信息

### 4.5 设备端反馈消息

设备端通过 HID IN 端点向 PC 端发送反馈消息，使用 `hid_send_data()` 函数。

#### 4.5.1 升级进度消息

在 `do_upgrade()` 执行过程中，通过 `artousb_show_upgrade()` 回调（`f_artousb.c:1005-1021`）发送：

```
"Upgrade status: <state>, percent:<percent>\n"
```

- **state**：升级状态码（整数）
- **percent**：进度百分比（0-100）

消息长度：64 字节（缓冲区固定大小）

#### 4.5.2 升级结果消息

升级完成后发送（`f_artousb.c:1116-1149`）：

- 成功：`"upgrade complete\n"`（64 字节缓冲区）
- 失败：`"upgrade failed!(ret = <error_code>)\n"`（64 字节缓冲区）

同时设备端串口输出：
- 成功：`upgrade complete!` + `artousb upgrade finish, please reset the board...`
- 失败：`upgrade failed!(ret = <error_code>)`

#### 4.5.3 错误消息

缓冲区为空时：`"spl buffer is null\n"`（16 字节缓冲区）

### 4.6 DDR 变化处理

如果升级镜像的 DDR 配置与当前板载 DDR 不匹配，`do_upgrade()` 返回 `ERR_DDR`（-7），设备端提示：

```
DDR change!!! reset or press 'f' to force upgrade......
```

此时需要设备端串口手动按 `f` 键强制升级。PC 端工具无法直接处理此情况，需要人工介入。

## 5. 文件传输协议（port_index = 4）

### 5.1 用途

将文件数据传输到设备端 DDR 指定地址。

### 5.2 启动命令

应用层数据格式（`f_artousb.c:1169-1189`）：

```
"uftr" (4 bytes) + file_total_len (4 bytes) + ddr_addr (8 bytes, 64-bit little-endian)
```

应用层数据总长度：`4 + 4 + 8 = 16` 字节

- **file_total_len**：文件总长度（unsigned int, 4 bytes）
- **ddr_addr**：目标 DDR 地址（unsigned long, 8 bytes on 64-bit platform）

### 5.3 数据传输

启动命令后，PC 端分片发送文件数据，设备端将数据拷贝到指定 DDR 地址。

## 6. 文件获取协议（port_index = 5）

### 6.1 用途

从设备端 DDR 指定地址读取数据。

### 6.2 请求命令

应用层数据格式（`f_artousb.c:1232-1268`）：

```
"ufgt" (4 bytes) + data_len (4 bytes) + ddr_addr (8 bytes, 64-bit little-endian)
```

应用层数据总长度：`4 + 4 + 8 = 16` 字节

- **data_len**：要读取的数据长度（unsigned int, 4 bytes）
- **ddr_addr**：源 DDR 地址（unsigned long, 8 bytes）

### 6.3 设备端响应

设备端收到请求后，从指定 DDR 地址读取 `data_len` 字节数据，通过 HID IN 端点发送给 PC 端。

如果 `ddr_addr` 为 0，设备端发送错误消息：`"target addr can not be zero!\n"`

## 7. USB 传输包构造示例

### 7.1 构造升级启动命令（High-Speed 模式）

假设镜像大小为 10,000,000 字节（0x989680）。

**逻辑数据**：
- HID_HEADER (12 bytes)：port_index=0, header_len=12, data_len=8
- HID payload (8 bytes)：`"upgd"` + `\x80\x96\x98\x00`

逻辑数据总长度 = 12 + 8 = 20 字节

由于 `max_payload = 988`（High-Speed），20 < 988，只需 1 个分片。

**USB 传输包**：

```
Offset  Length  Field               Value
0       8       magic_num           FF 55 FF AA 41 52 54 4F
8       4       header_len          24 00 00 00  (36)
12      4       total_len           14 00 00 00  (20)
16      4       payload_len         14 00 00 00  (20)
20      4       fragment_num        01 00 00 00  (1)
24      4       fragment_idx        00 00 00 00  (0)
28      1       sub_proto           01
29      3       reserved            00 00 00
32      4       checksum            (sum of payload bytes)
--- STRU_USB_HEADER ends (36 bytes) ---
36      2       port_index          00 00        (INDEX_SYS_UPGRADE)
38      2       header_len          0C 00        (12)
40      4       data_len            08 00 00 00  (8)
44      1       status              00
45      3       reserved            00 00 00
--- STRU_HID_HEADER ends (12 bytes) ---
48      4       start_mark          75 70 67 64  ("upgd")
52      4       image_total_len     80 96 98 00  (10000000)
```

总 USB 包长度 = 36 + 20 = 56 字节

### 7.2 构造镜像数据分片（High-Speed 模式）

每个镜像数据分片的应用层数据为镜像文件的原始数据。

**逻辑数据**：
- HID_HEADER (12 bytes)：port_index=0, header_len=12, data_len=<chunk_size>
- HID payload (<chunk_size> bytes)：镜像文件数据

其中 `chunk_size` 最大为 `988 - 12 = 976` 字节。

对于大于 976 字节的镜像数据，需要分为多个逻辑包发送，每个逻辑包再可能分为多个 USB 传输分片。

> **简化方案**：建议 PC 端工具每次发送不超过 `max_payload - sizeof(STRU_HID_HEADER)` 字节的镜像数据作为单个逻辑包，这样每个逻辑包只需 1 个 USB 传输分片。

### 7.3 checksum 计算方法

checksum 对 payload 部分逐字节累加（`f_artousb.c:839-850`）：

```c
unsigned int get_checksum(unsigned char * data, unsigned int len)
{
    unsigned int checksum = 0;
    while (len)
    {
        checksum += *data++;
        --len;
    }
    return checksum;
}
```

即：对 STRU_USB_HEADER 之后的所有 payload 字节，逐字节累加为 32 位无符号整数。

## 8. 完整升级流程

### 8.1 设备端操作

1. 进入 U-Boot 命令行
2. 执行 `artosyn_hid_upgrade 0`（0 为 USB 控制器编号）
3. 设备输出 `usb_booting enter!!!`，进入 USB gadget 等待状态
4. 等待 PC 端发送数据

### 8.2 PC 端操作

1. 检测 USB 设备（VID=0x1D6B, PID=0x0100）
2. 打开 HID 设备，获取 OUT 和 IN 端点
3. 读取升级镜像文件（如 `artosyn-upgrade-ar9341.img`）
4. 发送升级启动命令（port_index=0, data="upgd"+image_len）
5. 分片发送镜像数据（port_index=0, data=image_chunk）
6. 监听 IN 端点，接收升级进度和结果消息
7. 收到 `"upgrade complete\n"` 表示升级成功
8. 收到 `"upgrade failed!(ret = <code>)\n"` 表示升级失败

### 8.3 流程图

```
PC 端                                    设备端 (U-Boot)
  |                                          |
  |                          artosyn_hid_upgrade 0
  |                                          |
  |                          usb_booting enter!!!
  |                                          |
  |  发送升级启动命令                         |
  |  (port=0, "upgd" + image_len)            |
  |----------------------------------------->|
  |                          upgrade receive start! len:<N>
  |                                          |
  |  发送镜像数据分片 1                       |
  |  (port=0, image_data[0..chunk])          |
  |----------------------------------------->|
  |                                          |
  |  发送镜像数据分片 2                       |
  |  (port=0, image_data[chunk..2*chunk])    |
  |----------------------------------------->|
  |                                          |
  |                  ...                     |
  |                                          |
  |  发送镜像数据分片 N                       |
  |  (port=0, image_data[...end])            |
  |----------------------------------------->|
  |                          upgrade receive complete!
  |                          开始执行 do_upgrade()
  |                                          |
  |  接收进度消息                             |
  |  "Upgrade status: <s>, percent:<p>"      |
  |<-----------------------------------------|
  |                                          |
  |                  ...                     |
  |                                          |
  |  接收结果消息                             |
  |  "upgrade complete\n"                    |
  |<-----------------------------------------|
  |                          artousb upgrade finish,
  |                          please reset the board...
  |                                          |
```

## 9. PC 端工具开发指南

### 9.1 平台与库选择

- **Windows**：使用 WinUSB / HID API（如 hidapi 库）或直接使用 SetupAPI + HID 接口
- **Linux**：使用 hidraw 设备接口或 libusb
- **跨平台**：推荐使用 hidapi 库（https://github.com/libusb/hidapi）

### 9.2 设备查找

使用 VID=0x1D6B, PID=0x0100 查找 HID 设备。如果系统中有多个 U-Boot 设备，可通过接口字符串 "Artosyn Artousb Uboot HID" 进一步区分。

### 9.3 数据发送

通过 USB Interrupt OUT 端点发送数据。每个 USB 传输包格式：

```
[STRU_USB_HEADER (36 bytes)] [payload (HID_HEADER + HID data)]
```

发送步骤：

1. 构造逻辑数据 = `STRU_HID_HEADER` + 应用层数据
2. 计算分片数 `fragment_num = ceil(logical_data_len / max_payload)`
3. 对每个分片：
   - 填充 `STRU_USB_HEADER` 各字段
   - 计算 `checksum`（payload 部分逐字节累加；设备端不校验，但建议正确填写）
   - 拼接 USB_HEADER + payload
   - **确保 USB 包实际发送长度 = `sizeof(STRU_USB_HEADER) + payload_len`**
   - 通过 Interrupt OUT 端点发送

> **Report ID 注意**：详见 §1.5。如果使用 Windows HID API（`WriteFile`），需在数据前加 1 字节 Report ID（建议用 `0x02`）。如果使用 libusb/WinUSB 直接端点 I/O，则不需要 Report ID 前缀。**不加 Report ID 前缀直接 `WriteFile` 是导致 `magic num error!` 的最常见原因。**

### 9.4 数据接收

通过 USB Interrupt IN 端点接收设备端反馈。设备端发送的数据同样采用 `STRU_USB_HEADER` + payload 的分片格式。

**设备端发送分片长度差异**（`f_artousb.c:930-968`）：
- 非末尾分片：发送完整 `report_length` 字节（即端点 MaxPacketSize）
- 最后一个分片：发送 `payload_len + sizeof(STRU_USB_HEADER)` 字节（可能小于 `report_length`）

接收步骤：

1. 从 Interrupt IN 端点读取数据
2. 校验 `magic_num` 和 `sub_proto`
3. 提取 payload 数据（长度按实际读取字节数减去 36 计算）
4. 如果 `fragment_num > 1`，继续接收后续分片并拼接
5. 解析 `STRU_HID_HEADER`，根据 `port_index` 路由
6. 提取应用层数据（如进度消息、结果消息）

> **Report ID 注意**：详见 §1.5。如果使用 Windows HID API（`ReadFile`），读取的数据首字节为 Report ID，需跳过。如果使用 libusb/WinUSB 直接端点 I/O，则数据直接以 `STRU_USB_HEADER` 开头。

### 9.5 注意事项

- **端点大小**：必须动态获取端点 MaxPacketSize，不要硬编码。Full-Speed 为 64 字节，High-Speed 为 1024 字节
- **字节序**：所有多字节字段均为小端序（little-endian）
- **超时处理**：设备端发送数据有超时机制（`HID_TX_TIMEOUT = 1000`），PC 端接收时应设置合理的超时时间
- **进度反馈**：升级过程中设备端会持续发送进度消息，PC 端应实时显示进度条
- **Ctrl+C 退出**：设备端在等待循环中检测 Ctrl+C（`f_artousb.c:787-788`），PC 端无法通过 USB 发送 Ctrl+C 信号
- **镜像格式**：升级镜像必须为 Artosyn 格式（magic=`0x4152544F`，文件中字节为 `4F 54 52 41`），文件如 `artosyn-upgrade-ar9341.img`
- **DDR 地址长度**：文件传输/获取协议中的 DDR 地址为 8 字节（64-bit），因为 AR9341 为 ARM64 平台（`MAX_DDR_ADDRESS_LEN = sizeof(unsigned char *) = 8`）
- **checksum 非必需**：设备端接收时不校验 checksum，PC 端可填写任意值，但建议按规范计算

## 10. 常见错误排查

### 10.1 magic num error!

设备端疯狂输出 `magic num error!` 表示 USB 接收到的数据包前 8 字节不是合法的 `magic_header`（`FF 55 FF AA 41 52 54 4F`）。

**排查步骤**：

1. **检查 Report ID 前缀**（最常见原因）：
   - 如果使用 Windows HID API `WriteFile`，确认缓冲区首字节为 Report ID（`0x02`），其后才是 `STRU_USB_HEADER`
   - 如果使用 libusb/WinUSB 端点 I/O，确认缓冲区首字节为 `0xFF`（`magic_num[0]`），无 Report ID 前缀
   - 详见 §1.5

2. **检查是否构造了 USB 包头**：
   - 每个 USB 传输包必须以 `STRU_USB_HEADER`（36 字节）开头，不能直接发送镜像文件原始数据
   - 详见 §2.1 和 §7.1

3. **检查字节序**：
   - `magic_num` 是字节数组，按字节顺序发送：`FF 55 FF AA 41 52 54 4F`
   - 其他多字节字段（`header_len`、`total_len` 等）为小端序（little-endian）

4. **检查 struct packing**：
   - `STRU_USB_HEADER` 为 `#pragma pack(1)` 紧凑结构，`sizeof(STRU_USB_HEADER) = 36`
   - PC 端结构体也必须紧凑打包，不能有填充字节

### 10.2 sub_proto no match

表示 `sub_proto` 字段不为 `0x01`（`HID_PROTOCOL`）。检查 `STRU_USB_HEADER.sub_proto` 是否设置为 `0x01`。

### 10.3 size out of mtu

表示 `payload_len` 超过了端点 `report_length`。检查 `payload_len` 是否小于等于 `MaxPacketSize - sizeof(STRU_USB_HEADER)`。

### 10.4 index no match

表示分片索引不连续。检查 `fragment_idx` 是否从 0 开始递增，且无跳号或重复。

## 11. 相关源码文件

- `arsdk/platforms/uboot/u-boot-2020.10/drivers/usb/gadget/f_artousb.c` — USB HID gadget 协议实现
- `arsdk/platforms/uboot/u-boot-2020.10/drivers/usb/gadget/g_dnl.c` — USB Downloader Gadget 框架
- `arsdk/platforms/uboot/u-boot-2020.10/board/artosyn/common/ar_flash.c` — 升级烧录实现（do_upgrade）
- `arsdk/platforms/uboot/u-boot-2020.10/include/uboot_upgrade.h` — 升级镜像格式定义
- `arsdk/platforms/uboot/u-boot-2020.10/arch/arm/include/asm/arch-ar9301/f_artousb.h` — DRAM 起始地址定义
- `arsdk/platforms/uboot/u-boot-2020.10/configs/ar9341_emmc_defconfig` — USB Gadget 配置（VID/PID 等）
