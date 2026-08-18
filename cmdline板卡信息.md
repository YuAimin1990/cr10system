
> 我在设备的linux系统中执行命令和回复如下：
```
~ # cat /proc/cmdline
console=ttyS0,115200,earlyprintk loglevel=1,quiet root=/dev/mmcblk0p7 rootwait ro rootfstype=ext4 gpt mem=512m flagfile=/usrdata/sirius-clean-system-flag isolcpus=3 revision_id=1 soc_product_id=ar9321  pd=202220 feb_id=1 jedec_id=a2f id_info=0099XHHT sn=43011222 part_info=2047
```
> 请分析每一项的数据来源

The complete `cmdline` is:

```
console=ttyS0,115200,earlyprintk loglevel=1,quiet root=/dev/mmcblk0p7 rootwait ro rootfstype=ext4 gpt mem=512m flagfile=/usrdata/sirius-clean-system-flag isolcpus=3 revision_id=1 soc_product_id=ar9321 pd=202220 feb_id=1 jedec_id=a2f id_info=0099XhHT sn=43011222 part_info=2047
```

The parameters come from **three different sources** in two stages:

---

## 1. 静态参数 — U-Boot 环境变量 `bootargs`

**来源文件**: `@/home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arprj/configs/ar9341/cr10_config/uboot/env_emmc.txt:4`

原始 `bootargs` 环境变量值为：

```
console=ttyS0,115200,earlyprintk loglevel=1,quiet root=/dev/mmcblk0p7 rootwait ro rootfstype=ext4 gpt mem=512m flagfile=/usrdata/sirius-clean-system-flag isolcpus=3
```

### 1.1 各参数说明

- **`console=ttyS0,115200`** — 指定串口 ttyS0 作为内核控制台，波特率 115200。与 env 中 `baudrate=115200` 一致。
- **`earlyprintk`** — 内核早期启动阶段即启用串口打印。
- **`loglevel=1`** — 内核日志级别设为 1（仅紧急消息）。注意 env 文档中写的是 `loglevel=8`，但实际 env_emmc.txt 中是 `loglevel=1`。
- **`quiet`** — 抑制大部分内核启动日志输出。
- **`root=/dev/mmcblk0p7`** — 根文件系统位于 eMMC 第 7 个分区。对应 `@/home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arprj/configs/ar9341/cr10_config/plat/flash_layout_emmc.ini:50-54` 中的 `[partition6]`（name=userapp），GPT 分区号从 1 开始计数，第 7 个分区即 `userapp`。
- **`rootwait`** — 等待根设备就绪后再挂载。
- **`ro`** — 以只读方式挂载根文件系统。
- **`rootfstype=ext4`** — 根文件系统类型为 ext4。
- **`gpt`** — 使用 GPT 分区表。此参数由 `regenerate_bootinfo()` 函数在检测到 eMMC 启动模式时动态追加（`@/home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/common/autoboot.c:608`）。
- **`mem=512m`** — 设置内核可用内存为 512MB。
- **`flagfile=/usrdata/sirius-clean-system-flag`** — 系统标志文件路径，用于指示系统是否需要执行清理/升级流程。在 `@/home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arprj/configs/ar9341/cr10_config/scenario/start_network_upgrade.sh:61-62` 中检查并删除此文件。
- **`isolcpus=3`** — 将 CPU 核心 3 从 Linux 调度器中隔离，专用于实时任务（如 ISP/DSP 等）。

---

## 2. SoC 芯片信息参数 — 从 eFuse 读取

**来源代码**: `@/home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/arch/arm/mach-artosyn/ar9301/ar9301.c:149-202`

U-Boot 在 [misc_init_r()](cci:1://file:///home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/arch/arm/mach-artosyn/ar9301/ar9301.c:215:0-218:1) 阶段调用 [get_soc_info()](cci:1://file:///home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/arch/arm/mach-artosyn/ar9301/ar9301.c:148:0-201:1) 函数，从芯片 **eFuse** 中读取 SoC 信息并追加到 `bootargs`。

### 2.1 数据读取流程

1. [misc_init_r()](cci:1://file:///home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/arch/arm/mach-artosyn/ar9301/ar9301.c:215:0-218:1) 被调用（`ar9301.c:216-219`）
2. 调用 [get_soc_info()](cci:1://file:///home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/arch/arm/mach-artosyn/ar9301/ar9301.c:148:0-201:1)（`ar9301.c:149`）
3. 通过 `efuse_read()` 读取 512 字节 eFuse 数据到 buffer（`ar9301.c:157`）
4. 从 buffer 偏移 `AR_SOC_INFO_OFFSET`（`0x50 / 0x8 = 10` 字节）处解析 `ar_soc_info` 结构体（`ar9301.c:163`）
5. 用 `sprintf` 将各字段追加到 `bootargs`（`ar9301.c:182-191`）
6. 用 `env_set("bootargs", new_cmdline)` 更新环境变量（`ar9301.c:194`）

### 2.2 `ar_soc_info` 结构体定义

`@/home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/arch/arm/mach-artosyn/ar9301/ar9301.c:133-144`：

```c
struct ar_soc_info {
    unsigned char   revision_id;      // 芯片版本号
    char            soc_suffix;       // SoC 型号后缀
    unsigned short  soc_product_id;   // SoC 产品ID
    unsigned char   pd_year;          // 生产日期-年
    unsigned char   pd_week;          // 生产日期-周
    unsigned char   feb_id:4;         // 前端板ID (4 bit)
    unsigned char   jedec_id_bank:4;  // JEDEC ID bank (4 bit)
    unsigned char   jedec_id_code;    // JEDEC ID code
    char            id_info[8];       // ID信息字符串
    unsigned int    sn;               // 序列号
};
```

### 2.3 各参数与实际值对应

| cmdline 参数 | 实际值 | 结构体字段 | 数据来源 |
|---|---|---|---|
| `revision_id` | `1` | `info->revision_id` | eFuse 偏移 0x50 处第 0 字节 |
| `soc_product_id` | `ar9321` | `info->soc_product_id` (0x9321) + `info->soc_suffix` ('1') | eFuse — 格式为 `ar%x%c` |
| `pd` | `202220` | `info->pd_year` (0x22) + `info->pd_week` (0x22) | eFuse — 格式为 `20%x%x`，即 2022 年第 22 周 |
| `feb_id` | `1` | `info->feb_id` | eFuse — 4 bit 字段 |
| `jedec_id` | `a2f` | `info->jedec_id_bank` (0xa) + `info->jedec_id_code` (0x2f) | eFuse — 格式为 `%x%x` |
| `id_info` | `0099XhHT` | `info->id_info[0..7]` | eFuse — 8 字节 ASCII 字符串 |
| `sn` | `43011222` | `info->sn` | eFuse — 4 字节无符号整数 |

### 2.4 sprintf 格式

`@/home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/arch/arm/mach-artosyn/ar9301/ar9301.c:182-191`：

```c
sprintf(new_cmdline,
    "%s revision_id=%d soc_product_id=ar%x%c pd=20%x%x feb_id=%d jedec_id=%x%x id_info=%c%c%c%c%c%c%c%c sn=%d",
    cmdline,
    info->revision_id, info->soc_product_id, info->soc_suffix ?: ' ',
    info->pd_year, info->pd_week,
    info->feb_id, info->jedec_id_bank, info->jedec_id_code,
    info->id_info[0], ..., info->id_info[7],
    info->sn);
```

---

## 3. 分区有效性参数 — 从 GPT 分区表解析

**来源代码**: `@/home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/common/autoboot.c:374-634`

### 3.1 `part_info=2047`

在 `regenerate_bootinfo()` 函数中生成，该函数在 [autoboot_command()](cci:1://file:///home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/common/autoboot.c:635:0-665:1) 执行 `bootcmd` 之前被调用。

### 3.2 数据生成流程

1. 读取 eMMC 上的 GPT 分区表（[get_higher_version_gpt()](cci:1://file:///home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/board/artosyn/common/ar_common.c:165:0-250:1)，`autoboot.c:404`）
2. 调用 [artosyn_get_part_valid_info()](cci:1://file:///home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/board/artosyn/common/ar_common.c:300:0-317:1) 遍历所有分区（`autoboot.c:441`）
3. 检查每个 GPT 分区表项的 `attributes.fields.reserved` 字段第 44 bit（`PARTITION_VALID_SHIFT`），如果为 1 则该分区有效
4. 将有效分区的 bit 位按 `*part_info |= BIT(i - 1)` 组合成一个 bitmask
5. 追加到 bootargs：`sprintf(buf, "%s part_info=%llu", buf, part_info)`（`autoboot.c:619`）

### 3.3 值解析

`part_info=2047` = 二进制 `0b11111111111`，表示分区 0~10（共 11 个分区）全部有效。对应 [flash_layout_emmc.ini](cci:7://file:///home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arprj/configs/ar9341/cr10_config/plat/flash_layout_emmc.ini:0:0-0:0) 中的 partition0 到 partition10。

### 3.4 同阶段的其他动态修改

`regenerate_bootinfo()` 还会在此阶段：
- 根据 GPT 分区表动态修正 `root=/dev/mmcblk0pN` 中的分区号 N（`autoboot.c:542-543`），将其替换为实际有效的 `userapp` 分区号
- 在检测到 eMMC 启动模式时追加 `gpt` 参数（`autoboot.c:608`）

---

## 4. 完整数据流总结

```
env_emmc.txt (静态配置)
  └─ bootargs 基础部分
       console=ttyS0,115200,earlyprintk loglevel=1,quiet
       root=/dev/mmcblk0p7 rootwait ro rootfstype=ext4
       mem=512m flagfile=/usrdata/sirius-clean-system-flag isolcpus=3
            │
            ▼
misc_init_r() → get_soc_info()  [U-Boot 初始化阶段]
  └─ 从 eFuse 读取 ar_soc_info 结构体
       revision_id=1 soc_product_id=ar9321 pd=202220
       feb_id=1 jedec_id=a2f id_info=0099XhHT sn=43011222
            │
            ▼
autoboot_command() → regenerate_bootinfo()  [U-Boot 启动阶段]
  ├─ 从 GPT 分区表修正 root 分区号
  ├─ 追加 gpt 参数
  └─ 从 GPT 分区属性解析分区有效性 bitmask
       part_info=2047
            │
            ▼
最终 /proc/cmdline (传递给 Linux 内核)
```

# eFuse

eFuse（电子熔丝）是一种**一次性可编程（OTP）的硬件存储单元**，在芯片制造时写入，出厂后不可修改。

## 基本原理

- 每个 eFuse bit 是一个物理熔丝，编程时通过施加高电压将其"烧断"（改变电阻状态），从 0 变为 1
- 烧断后**不可逆**，因此每个 bit 只能被编程一次
- 未编程的 bit 默认值为 0，编程后变为 1

## 在 AR9301/AR9341 SoC 中的用途

从你的代码中可以看到，`@/home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/arch/arm/mach-artosyn/ar9301/ar9301.c:157` 处：

```c
ret = efuse_read((unsigned int *)buffer, AR_EFUSE_MAX_SIZE / 0x4, 0);
```

U-Boot 读取 512 字节 eFuse 数据，从中解析出以下信息：

| 字段 | 说明 | eFuse 中的内容 |
|---|---|---|
| `revision_id` | 芯片版本号 | 芯片流片/修订版本 |
| `soc_product_id` | 产品型号 | 如 ar9321 |
| `pd` (year+week) | 生产日期 | 如 2022 年第 22 周 |
| `feb_id` | 前端板 ID | 硬件板型标识 |
| `jedec_id` | JEDEC 内存 ID | DDR 颗粒厂商编码 |
| `id_info` | 芯片批次信息 | 8 字节 ASCII 字符串 |
| `sn` | 序列号 | 每颗芯片唯一 |

此外，eFuse 还存储了安全启动密钥相关信息（`@/home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/arch/arm/mach-artosyn/ar9301/ar9301.c:67-76` 中的 [secure_boot_enabled()](cci:1://file:///home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/arch/arm/mach-artosyn/ar9301/ar9301.c:66:0-75:1) 检查 `POLESTAR_EFUSE_SECURE_CONFIG`）。

## 特点总结

- **出厂烧录**：由芯片厂商在生产线测试时写入，包含每颗芯片的唯一标识
- **不可篡改**：OTP 特性使其适合存储安全密钥、设备序列号等防伪信息
- **读取方式**：U-Boot 通过 `efuse_read()` API 读取，无需文件系统支持
- **容量有限**：本 SoC 中仅 512 字节，需要紧凑布局（`ar_soc_info` 结构体从偏移 0x50 处开始）


# jedec_id

JEDEC ID 是 **JEDEC Solid State Technology Association**（固态技术协会）制定的标准内存颗粒标识编码，用于唯一识别 DDR 内存芯片的厂商和型号。

## 在 eFuse 中的存储

`@/home/richard/awork/ar9341-ar-sdk-0.16.03-00-release-20231106/zk_zam210_ar9341/arsdk/platforms/uboot/u-boot-2020.10/arch/arm/mach-artosyn/ar9301/ar9301.c:139-141`：

```c
unsigned char jedec_id_bank:4;  // JEDEC ID bank (4 bit)
unsigned char jedec_id_code;    // JEDEC ID code (8 bit)
```

你的设备中 `jedec_id=a2f`：
- **bank = 0xa**（高 4 bit）— JEDEC 厂商 bank 编号
- **code = 0x2f**（低 8 bit）— 该 bank 内的具体厂商代码

## JEDEC ID 的含义

JEDEC 标准中，内存颗粒（DDR3/DDR4 等）有一个 **SPD（Serial Presence Detect）** 信息，其中包含：

- **Bank** — 厂商分类组号，JEDEC 将厂商编码分为多个 bank（1~N），每个 bank 包含最多 256 个厂商代码
- **Code** — bank 内的具体厂商 ID，如 bank 1 中 `0x01` = AMD，`0x02` = Samsung，`0x2f` = Micron 等

## 为什么存储在 eFuse 中

这颗 SoC 将**实际焊接在板上的 DDR 颗粒的 JEDEC ID** 烧录进了 eFuse。这样做的原因：

1. **生产时记录实际用料** — 同一型号设备可能使用不同厂商的 DDR 颗粒，eFuse 记录了每台设备实际使用的颗粒厂商
2. **U-Boot/内核据此适配 DDR 配置** — 不同厂商的 DDR 可能需要不同的时序参数
3. **防伪追溯** — 记录生产物料信息，便于质量追溯

## 与其他 eFuse 字段的关系

eFuse 中的 `ar_soc_info` 结构体记录的是**整机关键物料信息**：

| 字段 | 记录的内容 |
|---|---|
| `soc_product_id` | SoC 芯片型号 |
| `feb_id` | 前端板（Front-End Board）ID |
| `jedec_id` | DDR 颗粒厂商编码 |
| `pd` | 生产日期 |
| `sn` | 设备序列号 |
| `id_info` | 批次/追溯字符串 |

这些信息在出厂测试时一次性烧入 eFuse，后续不可修改，确保每台设备的物料信息可永久追溯。