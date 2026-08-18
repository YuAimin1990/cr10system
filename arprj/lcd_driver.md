# LCD / VO 驱动说明

本文档说明当前 SDK 中 U-Boot 阶段 `VO` 模块初始化 LCD 与显示输出的原理、代码路径、参数含义与使用方法，目标如下：

1. 指导后续新 LCD 面板接入与移植驱动代码
2. 指导如何配置 VO 驱动
3. 指导如何使用 LCD 屏幕绘制图案或显示图像

本文重点分析 U-Boot 中 `CONFIG_VO=y` 对应的显示链路。当前平台 LCD 并非走通用 `CONFIG_LCD` / `CONFIG_VIDEO` 路径，而是走 Artosyn 自有的 `VO` 框架。

# 一、总体架构

当前平台 U-Boot 的 LCD/显示初始化链路如下：

```text
CONFIG_VO
 -> CONFIG_CMD_VO
 -> cmd/vo.c
 -> drivers/vo/vo.c
 -> drivers/vo/display_hal/display_rpc.c
 -> drivers/vo/display_core/*
 -> 具体 MIPI / DVP / Timing / Layer 显示输出
```

从职责上可分为 4 层：

- **命令层**
  - 负责解析 U-Boot 命令参数
  - 文件：`arsdk/platforms/uboot/u-boot-2020.10/cmd/vo.c`

- **VO 适配层**
  - 负责把命令参数组装为 `VO_DEV_ATTR` / `VO_LAYER_ATTR`
  - 文件：`arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/vo.c`

- **RPC / HAL 状态层**
  - 负责保存设备属性、图层属性、通道状态，并在 enable 阶段真正拉起显示
  - 文件：`arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/display_hal/display_rpc.c`

- **显示核心层**
  - 负责 display server、layer 句柄、底层显示设备初始化
  - 文件：`arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/display_core/display_top.c`

# 二、初始化入口

## 1. 编译开关

U-Boot 配置中启用：

```config
CONFIG_VO=y
```

这会使能 `CMD_VO` 及对应 `vo.o` 命令实现。

关键位置：

- `arsdk/platforms/uboot/u-boot-2020.10/cmd/Kconfig`
- `arsdk/platforms/uboot/u-boot-2020.10/cmd/Makefile`

## 2. 运行期初始化入口

U-Boot 启动过程中会调用：

- `ar_vo_init()`

对应实现位于：

- `arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/vo.c`

其核心逻辑是：

```c
int ar_vo_init(void)
{
    ar_display_init();
    return 0;
}
```

也就是说，U-Boot 阶段首先初始化 display server 与 HAL 状态机，而真正选择面板、时序、图层参数，是后面执行 `vo startdev` / `vo startlayer` 时完成的。

## 3. display server 初始化

`ar_display_init()` 位于：

- `arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/display_core/display_top.c`

作用：

- 创建 `display_server`
- 分配锁
- 初始化 `display_server->hal`
- 设置 `display_server->hal.mode = VO_HAL_MODE_MPP`

这一步相当于把 U-Boot 的显示子系统框架先搭起来。

# 三、U-Boot 中 VO 命令的用法

`vo` 命令位于：

- `arsdk/platforms/uboot/u-boot-2020.10/cmd/vo.c`

支持的主要命令：

```text
vo startdev   <dev intftype subintf sync>
vo stopdev    <dev>
vo startlayer <layer addr stride x y w h csc>
vo stoplayer  <layer>
```

## 1. `vo startdev`

用途：

- 选择输出设备
- 选择输出接口类型
- 选择具体面板或子接口
- 选择显示时序模板
- 最终初始化并使能显示设备

格式：

```sh
vo startdev <dev_id> <intf_type> <sub_intf_type> <sync_type>
```

## 2. `vo startlayer`

用途：

- 配置一个视频图层
- 指定图像所在物理地址
- 指定分辨率、stride、显示位置、颜色空间
- 把图像 buffer 入队显示

格式：

```sh
vo startlayer <layer_id> <addr> <stride> <x> <y> <w> <h> <csc>
```

# 四、VO 设备初始化原理

## 1. `startdev` 参数解析

`cmd/vo.c` 中：

```c
dev_id = simple_strtoul(argv[1], NULL, 10);
intf_type = simple_strtoul(argv[2], NULL, 10);
sub_intf_type = simple_strtoul(argv[3], NULL, 10);
sync_type = simple_strtoul(argv[4], NULL, 10);

ar_vo_dev_enable(dev_id, intf_type, sub_intf_type, sync_type);
```

也就是说，U-Boot 环境变量中的 `vo startdev` 最终直接传入 `ar_vo_dev_enable()`。

## 2. `ar_vo_dev_enable()` 的职责

位置：

- `arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/vo.c`

其核心工作是构造 `STRU_AR_HAL_VO_DEV_ATTR`：

- `bg_color`
- `e_interface`
- `u_sub_intance`
- `e_timing_template`
- `bit_count_per_channel`

然后调用：

```c
ar_vo_rpc_dev_set_attr(dev_id, &dev_attr);
ar_vo_rpc_dev_enable(dev_id);
```

也就是说：

- 第一步：保存设备属性
- 第二步：真正 enable 设备

## 3. 设备属性结构体

位于：

- `arsdk/platforms/uboot/u-boot-2020.10/include/vo/display_hal/hal_vo.h`

关键结构体：

```c
typedef struct
{
    AR_U32 bg_color;
    STRU_AR_HAL_VO_SYNC_INFO timing_customize;
    ENUM_AR_HAL_VO_DEV_INTF e_interface;
    UNION_AR_HAL_VO_SUB_INSTANCE u_sub_intance;
    ENUM_AR_HAL_VO_DEV_TIMING_TEMPLATE e_timing_template;
    AR_U32 bit_count_per_channel;
} STRU_AR_HAL_VO_DEV_ATTR;
```

这说明一个 LCD/显示输出设备的本质由以下几项决定：

- **接口类型**
  - DVP / DP / MIPI
- **接口子类型**
  - DVP 时可表示 VGA / HDMI / LCD_16BIT / LCD_24BIT 等
  - MIPI 时可表示具体 panel 类型
- **时序模板**
  - 常规枚举时序，或用户自定义时序
- **位宽**
  - `bit_count_per_channel`

# 五、接口类型、面板类型与时序模板

## 1. 接口类型枚举

```c
AR_HAL_VO_DEV_INTF_NONE = 0
AR_HAL_VO_DEV_INTF_DVP  = 1
AR_HAL_VO_DEV_INTF_DP   = 2
AR_HAL_VO_DEV_INTF_MIPI = 3
```

当前 AR93xx 平台重点使用：

- `DVP`
- `MIPI`

## 2. DVP 子接口枚举

```c
AR_HAL_VO_DEV_SUB_INTF_NONE = 0
AR_HAL_VO_DEV_SUB_INTF_VGA
AR_HAL_VO_DEV_SUB_INTF_HDMI
AR_HAL_VO_DEV_SUB_INTF_BT656
AR_HAL_VO_DEV_SUB_INTF_BT1120
AR_HAL_VO_DEV_SUB_INTF_LCD_16BIT
AR_HAL_VO_DEV_SUB_INTF_LCD_24BIT
```

适用于并口 LCD、VGA、HDMI 等场景。

## 3. MIPI panel 枚举

```c
AR_HAL_VO_MIPITX_PANNEL_NONE = 0
AR_HAL_VO_MIPITX_PANNEL_EK79007 = 1
AR_HAL_VO_MIPITX_PANNEL_ST7701  = 2
AR_HAL_VO_MIPITX_PANNEL_ST7703  = 3
```

这说明当前 SDK 已内置至少 3 种 MIPI 屏幕 profile：

- `EK79007`
- `ST7701`
- `ST7703`

## 4. timing template 枚举

示例：

```c
AR_HAL_VO_OUTPUT_400x960_60
AR_HAL_VO_OUTPUT_720x1440_60
AR_HAL_VO_OUTPUT_1024x600_60
AR_HAL_VO_OUTPUT_800x480_60
AR_HAL_VO_OUTPUT_USER
```

其作用是告诉 VO 子系统：

- 分辨率
- 刷新率
- 行场时序
- 同步极性
- panel 工作模式

## 5. 自定义时序

如果现有模板不满足需求，可以使用：

```c
AR_HAL_VO_OUTPUT_USER
```

并通过 `STRU_AR_HAL_VO_SYNC_INFO timing_customize` 填入：

- `hpw`
- `hbp`
- `hdp`
- `hfp`
- `vpw`
- `vbp`
- `vdp`
- `vfp`
- `fps`
- `de_polarity`
- `clock_polarity`
- `hsync_polarity`
- `vsync_polarity`

这就是后续接入新面板时最关键的扩展能力之一。

# 六、现有板卡案例说明

## 1. CR10

环境变量：

```sh
vo startdev 0 3 3 3
vo startlayer 0 0x22400000 1024 0 0 720 1440 0
```

解释：

- `intf_type = 3` -> `MIPI`
- `sub_intf_type = 3` -> `ST7703`
- `sync_type = 3` -> `AR_HAL_VO_OUTPUT_720x1440_60`

所以 CR10 实际对应：

- **MIPI DSI 输出**
- **panel profile = ST7703**
- **timing = 720x1440@60**

## 2. CR20

环境变量：

```sh
vo startdev 0 3 2 2
vo startlayer 0 0x22400000 512 0 0 400 960 0
```

解释：

- `intf_type = 3` -> `MIPI`
- `sub_intf_type = 2` -> `ST7701`
- `sync_type = 2` -> `AR_HAL_VO_OUTPUT_400x960_60`

所以 CR20 实际对应：

- **MIPI DSI 输出**
- **panel profile = ST7701**
- **timing = 400x960@60**

## 3. 结论

CR10 和 CR20：

- 使用的是同一套 VO 驱动框架
- 但在框架内部选择了不同的 panel profile 和 timing template

因此技术上应理解为：

- **不是两份完全独立的显示驱动模块**
- **而是同一套 VO 显示驱动中的不同 panel 配置分支**

# 七、图层初始化与图像显示原理

## 1. `startlayer` 参数含义

```sh
vo startlayer <layer_id> <addr> <stride> <x> <y> <w> <h> <csc>
```

含义如下：

- **`layer_id`**
  - 图层编号
- **`addr`**
  - 图像物理地址
- **`stride`**
  - Y 分量 stride
- **`x/y`**
  - 显示位置
- **`w/h`**
  - 图像宽高
- **`csc`**
  - 颜色空间转换模式

## 2. 图层属性结构体

```c
typedef struct
{
    AR_S32 width;
    AR_S32 height;
    AR_S32 luma_stride;
    AR_S32 chroma_stride;
    ENUM_AR_HAL_VO_FMT format;
    AR_S32 fps;
} STRU_AR_HAL_VO_LAYER_ATTR;
```

## 3. 当前图层显示格式

`ar_vo_layer_enable()` 中固定为：

```c
layer_attr.format = AR_HAL_VO_FMT_YV12;
```

说明当前 U-Boot logo 显示默认使用：

- **YV12 三平面格式**

即：

- `pannel[0]` -> Y
- `pannel[1]` -> U
- `pannel[2]` -> V

## 4. 图像 buffer 地址计算

代码：

```c
buffer.pannel[0].buffer_pa = (void *)addr;
buffer.pannel[1].buffer_pa = (void *)addr + stride * h;
buffer.pannel[2].buffer_pa = (void *)addr + stride * h * 5 / 4;
```

这意味着输入图像必须按 YV12 内存布局组织：

- Y 面大小：`stride * h`
- U 面起始：`addr + stride * h`
- V 面起始：`addr + stride * h * 5 / 4`

因此如果要自己生成绘图 buffer，必须严格遵守这个布局。

## 5. CSC 参数

```c
0 -> bt709 limit
1 -> bt709 full
2 -> bt601 limit
3 -> bt601 full
```

通常启动 logo 场景使用：

- `0`

即：

- `BT709 limited`

# 八、VO enable 的内部工作流程

## 1. 设备 enable 流程

`ar_vo_rpc_dev_set_attr()`：

- 把 `dev_attr` 保存到 `server->hal.vo_dev.attr`

`ar_vo_rpc_dev_enable()`：

- 获取 video layer 句柄
- 调用 `ar_init_display()`
- 调用 `ar_display_dev_enable()`
- 标记 `server->hal.vo_dev.enable = AR_TRUE`

说明：

- **设备属性先保存，后 enable**
- **真正的面板初始化发生在 enable 阶段**

## 2. 图层 enable 流程

`ar_vo_rpc_layer_set_attr()`：

- 保存 layer 属性

`ar_vo_rpc_layer_set_csc()`：

- 保存或设置图层 CSC

`ar_vo_rpc_layer_enable()`：

- 检查 VO 设备是否已使能
- 获取 layer 句柄
- 计算 stride
- 调用 `ar_display_layer_enable()`
- 设置 layer CSC
- 标记图层 enable

随后 `ar_vo_layer_enable()` 再调用：

```c
ar_vo_rpc_layer_enqueue(layer_id, &buffer)
```

将实际图像入队显示。

## 3. 关键约束

从 `display_rpc.c` 可以总结出以下约束：

- 必须先 `startdev`，再 `startlayer`
- 设备已 enable 后，不能再次 set dev attr
- 图层已 enable 后，不能再次 set layer attr
- 停设备前，必须先停所有图层

建议调试顺序固定如下：

```sh
vo startdev ...
vo startlayer ...
```

关闭时：

```sh
vo stoplayer 0
vo stopdev 0
```

# 九、新 LCD 面板接入指导

这一节是后续移植最核心的内容。

## 1. 先确认接口类型

首先确认新屏属于哪类：

- **MIPI DSI 屏**
- **DVP / RGB 并口屏**
- **HDMI / VGA 外接输出**

不同接口决定：

- `e_interface` 取值
- `u_sub_intance` 的解释方式

## 2. 如果是已有 panel 类型的变种

如果新屏与 `EK79007` / `ST7701` / `ST7703` 兼容，仅参数略有差异：

- 优先复用已有 panel 类型
- 评估是否只需要新增 timing template
- 或改用 `AR_HAL_VO_OUTPUT_USER` 自定义时序

这种方式改动最小。

## 3. 如果是全新 MIPI panel

推荐步骤：

- 在 `ENUM_AR_HAL_VO_MIPI_TX_PANNEL` 中新增新的 panel 枚举
- 在显示核心或 panel 配置代码中新增该 panel 的初始化分支
- 配置对应 MIPI DSI init sequence
- 补充其默认 timing template 或走 `AR_HAL_VO_OUTPUT_USER`
- 在 `cmd/vo.c` 的帮助文本中加入说明
- 在板级 `env_emmc.txt` 中使用新的 `sub_intf_type` 与 `sync_type`

建议新增内容至少包括：

- panel ID / 名称
- 分辨率
- lane 数
- bit depth
- pixel clock
- H/V porch
- sync width
- 极性
- 初始化命令序列
- sleep out / display on 时序

## 4. 如果是全新 DVP / RGB 并口屏

推荐步骤：

- 选择 `AR_HAL_VO_DEV_INTF_DVP`
- 为 `u_sub_intance.e_sub_intf` 选择合适类型
- 若现有子类型不够，新增 DVP 子接口枚举
- 配置新的 timing template 或使用 `AR_HAL_VO_OUTPUT_USER`
- 校验数据位宽是 16bit 还是 24bit
- 校验 DE / HSYNC / VSYNC / PCLK 极性

## 5. 新面板接入最重要的 4 类参数

无论 MIPI 还是 DVP，新屏 bring-up 一般都绕不开以下 4 类参数：

- **接口参数**
  - MIPI / DVP
  - lane 数
  - bit depth

- **面板初始化参数**
  - reset 时序
  - init command sequence
  - 上电 / 休眠退出时序

- **显示时序参数**
  - width / height
  - fps
  - hsync / vsync
  - hbp / hfp / vbp / vfp

- **buffer 参数**
  - pixel format
  - stride
  - plane 布局

## 6. 推荐接入流程

建议按以下顺序执行：

- **第一步**
  - 明确屏接口和主控连接方式

- **第二步**
  - 根据屏 spec 整理时序参数

- **第三步**
  - 判断复用已有 panel 还是新增 panel enum

- **第四步**
  - 补充底层 panel init sequence

- **第五步**
  - 配置 `vo startdev`

- **第六步**
  - 制作匹配分辨率和 stride 的测试图

- **第七步**
  - 配置 `vo startlayer` 验证显示

- **第八步**
  - 验证颜色、方向、撕裂、闪屏、休眠唤醒

# 十、如何配置 VO 驱动

## 1. 板级配置入口

最常见的板级入口是：

- `arprj/configs/<chip>/<board>/uboot/env_emmc.txt`

重点字段：

- `bootcmd`

其中通常包含：

```sh
mmc read 0x22400000 $logo_start $logo_size;
vo startdev ...;
vo startlayer ...;
```

## 2. `vo startdev` 的配置原则

先确定：

- 输出接口：MIPI / DVP
- panel 类型：已有还是新增
- 时序：已有模板还是自定义

例如：

```sh
vo startdev 0 3 3 3
```

表示：

- `dev_id = 0`
- `MIPI`
- `ST7703`
- `720x1440@60`

## 3. `vo startlayer` 的配置原则

需要与 logo buffer 保持一致：

- 分辨率必须一致
- stride 必须一致
- YV12 排布必须一致
- 颜色空间与图像制作方式一致

例如：

```sh
vo startlayer 0 0x22400000 1024 0 0 720 1440 0
```

说明：

- 图像起始地址：`0x22400000`
- stride：`1024`
- 显示区域：`720x1440`
- 左上角显示
- `BT709 limited`

## 4. logo 分区与图片资源

板级配置通常还会指定 logo 分区资源，例如：

- `start_logo_720x1440_stride1024.yuv`
- `start_logo_400x960_stride512.yuv`

这类文件必须满足：

- 分辨率与 `startlayer` 一致
- stride 与 `startlayer` 一致
- 图像格式符合 YV12 三平面布局

# 十一、如何使用 LCD 绘制图案

这里的“绘制图案”在当前 U-Boot VO 体系下，本质上是：

- 准备一块符合格式要求的图像 buffer
- 把它放到内存某个物理地址
- 通过 `vo startlayer` 或后续 enqueue 显示出来

## 1. 当前最直接的方法：显示一张 YV12 图片

流程：

- 准备 YV12 图片数据
- 将数据放入 DDR 指定地址
- 调用 `vo startdev`
- 调用 `vo startlayer`

示例：

```sh
mmc read 0x22400000 $logo_start $logo_size
vo startdev 0 3 3 3
vo startlayer 0 0x22400000 1024 0 0 720 1440 0
```

## 2. 自己生成纯色图案

如果想显示纯色背景、色块、测试条，可以离线或在线生成 YV12 buffer。

### 方法一：离线生成 YV12 文件

可以在 PC 端生成如下图案：

- 全红
- 全绿
- 全蓝
- 黑白棋盘格
- 灰阶条
- 彩条

再把文件烧录到 logo 分区或通过内存读入显示。

### 方法二：运行时写内存生成

如果在 U-Boot 或早期程序里能直接写目标内存，可以构造：

- Y 面：控制亮度
- U/V 面：控制色度

YV12 大致规则：

- 改变 Y -> 改变明暗
- 改变 U/V -> 改变颜色偏向

例如：

- 灰色背景：固定 Y，中性 U/V
- 黑色：低 Y，中性 U/V
- 白色：高 Y，中性 U/V

但要注意：

- stride 不一定等于 width
- UV 分量分辨率是 Y 的一半
- 必须按三平面地址布局写入

## 3. 自己绘制矩形/图案

绘制矩形的基本方法是：

- 对 Y 面指定矩形区域填充值
- 对 U/V 面对应区域填充值

例如想画一个亮色矩形：

- 提高该区域 Y 值
- 调整 U/V 形成目标颜色

由于当前 U-Boot VO 路径最终吃的是完整帧 buffer，而不是矢量绘图 API，所以“绘图”本质上是：

- **手动修改 framebuffer/buffer 内容**
- **再将整帧送给 VO layer**

## 4. 多次刷新图像

理论上可以多次调用 enqueue 更新图像，从而实现：

- 动态 logo
- 简单动画
- 进度条
- 调试测试图

需要具备：

- 新 buffer 地址
- 正确缓存刷新
- 正确的 plane 布局

当前代码中在 enqueue 前调用了：

```c
flush_dcache_all();
```

说明在 CPU 写完 buffer 后，必须保证 cache 刷新，否则显示侧可能读到旧数据。

# 十二、调试建议

## 1. 屏不亮时优先检查

- `startdev` 参数是否正确
- `sub_intf_type` 是否选对 panel
- `sync_type` 是否选对时序
- 是否先 `startdev` 再 `startlayer`
- logo 是否读入到正确地址
- `w/h/stride` 是否匹配实际图片

## 2. 图像错位或花屏时优先检查

- `stride` 是否正确
- 输入格式是否真的是 YV12
- Y/U/V plane 地址是否正确
- `logo_size` 是否足够
- 分辨率是否与 timing 匹配

## 3. 颜色异常时优先检查

- `csc` 是否选对
- 图像是否按 YV12 而不是 NV12/NV21/I420 生成
- U/V 平面顺序是否正确

## 4. 面板初始化异常时优先检查

- panel 供电时序
- reset 时序
- MIPI init 命令序列
- lane / bit depth 配置
- timing template 或自定义时序参数

# 十三、建议的维护方式

为了方便后续扩展，建议按以下方式维护 LCD/VO 适配：

- **面板枚举集中维护**
  - 新 MIPI panel 统一扩展 `ENUM_AR_HAL_VO_MIPI_TX_PANNEL`

- **时序模板集中维护**
  - 能抽象为公共模板的，尽量放入 timing template
  - 特殊屏走 `AR_HAL_VO_OUTPUT_USER`

- **板级环境变量只放选择参数**
  - 板级 `env_emmc.txt` 负责选择 panel 和 timing
  - 不要把太多底层细节散落到脚本中

- **测试图资源命名规范化**
  - 建议统一命名为：
  - `start_logo_<width>x<height>_stride<stride>.yuv`

- **新屏接入至少保留 3 张测试图**
  - 纯色图
  - 彩条图
  - 网格图

这样更容易定位：

- 时序问题
- 颜色空间问题
- stride 问题
- 图像排布问题

# 十四、最终结论

当前 SDK 中 LCD 的 U-Boot 初始化原理可以概括为：

- `CONFIG_VO=y` 打开 Artosyn VO 显示框架
- `vo startdev` 负责选择接口类型、面板类型、时序模板，并 enable 显示设备
- `vo startlayer` 负责设置图层属性，并将 YV12 图像 buffer 入队显示
- 新 LCD 面板接入的核心工作，不是重写整套显示框架，而是在现有 VO 框架中：
  - 增加新的 panel 类型
  - 增加新的 timing 配置
  - 补充面板初始化序列
  - 配置正确的 `startdev` / `startlayer` 参数

因此后续接入新屏时，应优先判断：

- 是复用已有 panel profile
- 还是新增一个 panel profile
- 是复用已有 timing template
- 还是走 `AR_HAL_VO_OUTPUT_USER` 自定义时序

只要这两部分明确，VO 驱动的接入和显示验证就会清晰很多。

# 十五、新 LCD 面板接入 Checklist

这一节给出一份可直接执行的 bring-up 清单，建议实际接入新屏时逐项打勾。

## 1. 硬件信息准备

- **确认屏幕型号**
  - 面板主型号
  - 供应商型号
  - 驱动 IC 型号

- **确认接口类型**
  - MIPI DSI
  - RGB / DVP 并口
  - HDMI / VGA 转接

- **确认连接信息**
  - lane 数
  - bit depth
  - reset GPIO
  - 供电时序
  - 背光控制 GPIO / PWM

- **确认官方资料**
  - panel spec
  - init sequence
  - timing 表
  - sleep in / sleep out 时序

## 2. 软件适配决策

- **判断是否可复用已有 panel profile**
  - 与 `EK79007` 接近
  - 与 `ST7701` 接近
  - 与 `ST7703` 接近

- **判断是否可复用已有 timing template**
  - 分辨率相同
  - 刷新率相同
  - porch / sync 基本一致

- **如果不能复用**
  - 新增 panel 枚举
  - 新增 panel 初始化分支
  - 新增 timing template 或改用 `AR_HAL_VO_OUTPUT_USER`

## 3. 代码修改 Checklist

- **VO 命令帮助文本**
  - 在 `cmd/vo.c` 中补充新 panel 说明

- **panel 枚举**
  - 在 `include/vo/display_hal/hal_vo.h` 中新增 panel enum

- **VO 参数映射**
  - 在 `drivers/vo/vo.c` 中补充 `sub_intf_type` / `sync_type` 到 panel / timing 的映射

- **底层 panel init sequence**
  - 在显示核心对应实现中增加初始化分支
  - 包括 reset / sleep out / display on / gamma / lane / format 等

- **时序参数**
  - 复用模板或新增模板
  - 如模板不满足，则走 `AR_HAL_VO_OUTPUT_USER`

- **板级环境变量**
  - 在 `env_emmc.txt` 中更新 `vo startdev`
  - 在 `env_emmc.txt` 中更新 `vo startlayer`

## 4. 板级参数 Checklist

- **`startdev` 参数正确**
  - `intf_type` 正确
  - `sub_intf_type` 正确
  - `sync_type` 正确

- **`startlayer` 参数正确**
  - `addr` 正确
  - `stride` 正确
  - `w/h` 正确
  - `csc` 正确

- **logo 资源正确**
  - 分辨率匹配
  - stride 匹配
  - 格式为 YV12
  - 文件大小正确

## 5. 上屏验证 Checklist

- **基础显示验证**
  - 屏亮
  - 无花屏
  - 无偏色
  - 无明显撕裂

- **内容验证**
  - 纯色图正常
  - 彩条图正常
  - 网格图正常
  - logo 居中或位置正确

- **时序验证**
  - 刷新稳定
  - 无闪屏
  - 无抖动
  - 无半屏、偏移、裁切

- **电源与休眠验证**
  - 上电稳定
  - reset 时序正确
  - sleep out 正常
  - display on 正常

## 6. 问题定位 Checklist

- **黑屏时检查**
  - panel init sequence 是否执行
  - timing 是否正确
  - 背光是否点亮
  - `vo startdev` 是否成功
  - `vo startlayer` 是否成功

- **花屏时检查**
  - stride 是否错误
  - YV12 plane 布局是否错误
  - 分辨率是否与 panel timing 不匹配

- **偏色时检查**
  - YV12 是否写成 NV12 / NV21 / I420
  - U/V plane 顺序是否错误
  - CSC 配置是否错误

- **画面偏移时检查**
  - `x/y` 是否错误
  - porch / sync 是否错误
  - 有效显示区是否配置错误

# 十六、YV12 绘制测试图示例

当前 U-Boot VO 图层代码固定按 `YV12` 三平面输入，因此最实用的做法是：

- 在 PC 端生成 `.yuv` 测试图
- 放入 logo 分区或 DDR
- 用 `vo startlayer` 显示

下面给出推荐示例。

## 1. YV12 内存布局回顾

对于分辨率 `w x h`、亮度 stride 为 `stride` 的 YV12 图像：

- **Y plane 大小**
  - `stride * h`

- **U plane 起始地址**
  - `addr + stride * h`

- **V plane 起始地址**
  - `addr + stride * h * 5 / 4`

说明：

- Y plane 分辨率为 `w x h`
- U/V plane 分辨率为 `w/2 x h/2`
- U/V stride 通常为 `stride / 2`

## 2. Python 生成纯色 YV12 图片示例

下面脚本可以生成纯色 YV12 图片。

```python
import sys


def make_yv12_solid(width, height, stride, y_value, u_value, v_value, output):
    y_plane_size = stride * height
    uv_stride = stride // 2
    uv_height = height // 2
    uv_plane_size = uv_stride * uv_height

    y_plane = bytes([y_value]) * y_plane_size
    u_plane = bytes([u_value]) * uv_plane_size
    v_plane = bytes([v_value]) * uv_plane_size

    with open(output, "wb") as f:
        f.write(y_plane)
        f.write(u_plane)
        f.write(v_plane)


if __name__ == "__main__":
    if len(sys.argv) != 8:
        print("usage: python3 make_yv12_solid.py <w> <h> <stride> <Y> <U> <V> <output>")
        sys.exit(1)

    width = int(sys.argv[1])
    height = int(sys.argv[2])
    stride = int(sys.argv[3])
    y_value = int(sys.argv[4])
    u_value = int(sys.argv[5])
    v_value = int(sys.argv[6])
    output = sys.argv[7]

    make_yv12_solid(width, height, stride, y_value, u_value, v_value, output)
```

示例：生成 CR10 对应的灰色图：

```sh
python3 make_yv12_solid.py 720 1440 1024 128 128 128 start_logo_720x1440_stride1024_gray.yuv
```

示例：生成 CR20 对应的黑色图：

```sh
python3 make_yv12_solid.py 400 960 512 16 128 128 start_logo_400x960_stride512_black.yuv
```

## 3. Python 生成网格测试图示例

网格图有助于检查：

- stride 是否正确
- 缩放是否异常
- 图像是否拉伸
- 是否存在偏移或裁切

```python
import sys


def make_yv12_grid(width, height, stride, cell, output):
    uv_stride = stride // 2
    uv_height = height // 2

    y_plane = bytearray(stride * height)
    u_plane = bytearray(uv_stride * uv_height)
    v_plane = bytearray(uv_stride * uv_height)

    for y in range(height):
        for x in range(width):
            idx = y * stride + x
            if x % cell == 0 or y % cell == 0:
                y_plane[idx] = 235
            else:
                y_plane[idx] = 32

    for y in range(uv_height):
        for x in range(width // 2):
            idx = y * uv_stride + x
            u_plane[idx] = 128
            v_plane[idx] = 128

    with open(output, "wb") as f:
        f.write(y_plane)
        f.write(u_plane)
        f.write(v_plane)


if __name__ == "__main__":
    if len(sys.argv) != 6:
        print("usage: python3 make_yv12_grid.py <w> <h> <stride> <cell> <output>")
        sys.exit(1)

    width = int(sys.argv[1])
    height = int(sys.argv[2])
    stride = int(sys.argv[3])
    cell = int(sys.argv[4])
    output = sys.argv[5]

    make_yv12_grid(width, height, stride, cell, output)
```

示例：

```sh
python3 make_yv12_grid.py 720 1440 1024 64 start_logo_720x1440_stride1024_grid.yuv
```

## 4. Python 生成彩条图示例

彩条图有助于检查：

- CSC 是否正确
- U/V 平面顺序是否正确
- 是否出现偏色

下面脚本使用近似 YUV 值生成竖直彩条，重点用于调试，不追求精确色彩科学换算。

```python
import sys


COLOR_BARS = [
    (235, 128, 128),
    (210, 16, 146),
    (170, 166, 16),
    (145, 54, 34),
    (107, 202, 222),
    (81, 90, 240),
    (41, 240, 110),
    (16, 128, 128),
]


def make_yv12_color_bars(width, height, stride, output):
    uv_stride = stride // 2
    uv_height = height // 2

    y_plane = bytearray(stride * height)
    u_plane = bytearray(uv_stride * uv_height)
    v_plane = bytearray(uv_stride * uv_height)

    bar_width = max(1, width // len(COLOR_BARS))

    for y in range(height):
        for x in range(width):
            bar = min(len(COLOR_BARS) - 1, x // bar_width)
            yv, uv, vv = COLOR_BARS[bar]
            y_plane[y * stride + x] = yv

    for y in range(uv_height):
        for x in range(width // 2):
            bar = min(len(COLOR_BARS) - 1, (x * 2) // bar_width)
            yv, uv, vv = COLOR_BARS[bar]
            u_plane[y * uv_stride + x] = uv
            v_plane[y * uv_stride + x] = vv

    with open(output, "wb") as f:
        f.write(y_plane)
        f.write(u_plane)
        f.write(v_plane)


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("usage: python3 make_yv12_color_bars.py <w> <h> <stride> <output>")
        sys.exit(1)

    width = int(sys.argv[1])
    height = int(sys.argv[2])
    stride = int(sys.argv[3])
    output = sys.argv[4]

    make_yv12_color_bars(width, height, stride, output)
```

示例：

```sh
python3 make_yv12_color_bars.py 400 960 512 start_logo_400x960_stride512_colorbars.yuv
```

## 5. 把测试图用于 U-Boot 显示

典型流程：

- 生成 `.yuv` 测试图
- 替换板级 logo 资源或单独写入 logo 分区
- U-Boot 启动时执行 `mmc read`
- 执行 `vo startdev`
- 执行 `vo startlayer`

例如：

```sh
mmc read 0x22400000 $logo_start $logo_size
vo startdev 0 3 3 3
vo startlayer 0 0x22400000 1024 0 0 720 1440 0
```

## 6. 如果想在代码里动态绘制图案

思路是：

- 在 DDR 某块可用内存中准备一帧 YV12 buffer
- 用 C 代码往 Y/U/V plane 写像素值
- 刷 cache
- 调用 enqueue 显示

伪代码如下：

```c
uint8_t *y = (uint8_t *)addr;
uint8_t *u = y + stride * height;
uint8_t *v = y + stride * height * 5 / 4;

fill_y_plane(y, width, height, stride);
fill_u_plane(u, width / 2, height / 2, stride / 2);
fill_v_plane(v, width / 2, height / 2, stride / 2);

flush_dcache_all();
ar_vo_rpc_layer_enqueue(layer_id, &buffer);
```

这种方式适合做：

- 纯色背景
- 简单矩形
- 对角线
- 网格
- 进度条
- 简单动画

## 7. 推荐最少准备的测试图集合

建议每个新屏最少准备以下 5 类图：

- **黑图**
  - 检查背光、黑场、漏光

- **白图**
  - 检查亮场、坏点、条纹

- **灰图**
  - 检查均匀性、闪烁

- **网格图**
  - 检查 stride、缩放、偏移、裁切

- **彩条图**
  - 检查颜色、CSC、U/V 顺序

# 十七、建议的落地流程

如果后面要接一个全新的 LCD，建议直接按下面流程执行：

- **第一天**
  - 拿到 panel spec、init sequence、timing 表
  - 确定接口类型和供电/reset/背光控制

- **第二天**
  - 确定复用还是新增 panel profile
  - 实现底层 panel 初始化代码
  - 先上纯色图测试亮屏

- **第三天**
  - 调整 timing / stride / CSC
  - 使用网格图与彩条图校准显示质量

- **第四天**
  - 验证启动 logo、稳定性、休眠唤醒
  - 固化到板级 `env_emmc.txt` 和资源分区

这样做的优点是：

- 先解决亮屏
- 再解决正确显示
- 最后解决显示质量与稳定性

能大幅降低新屏 bring-up 的定位成本。
