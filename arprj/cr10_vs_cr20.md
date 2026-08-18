

主要差异（非 U-Boot 配置文件本身，而是板级/场景/环境文件）：

- 板卡标识  
  - `board.config`: gBOARD 从 `cr10_config` 改为 `cr20_config` @arprj/configs/ar9341/cr10_config/board.config#1-6 vs @arprj/configs/ar9341/cr20_config/board.config#1-6

- 启动画面及分辨率  
  - `plat/flash_layout_emmc.ini`: logo 分区文件从 `start_logo_720x1440_stride1024.yuv` 改为 `start_logo_400x960_stride512.yuv` @arprj/configs/ar9341/cr10_config/plat/flash_layout_emmc.ini#61-70 vs @arprj/configs/ar9341/cr20_config/plat/flash_layout_emmc.ini#61-70

- 文件系统打包内容  
  - `plat/fs.json`: cr10 打包 `test_vo.yuv`，cr20 改为 `test_rc_400x960_stride512.yuv`；调优文件从带通配符版本号改为固定文件名 @arprj/configs/ar9341/cr10_config/plat/fs.json#15-24 vs @arprj/configs/ar9341/cr20_config/plat/fs.json#15-24

- 启动脚本（差异最大）  
  - `scenario/run.sh`: cr10 启动流程包含 loopback/eth 配置、wifi/eth 脚本、触摸屏与 framebuffer 驱动加载（ar_framebuffer.ko 720x1440）、多路 MJPEG 推流、redis + openresty、音频服务、crond 等；cr20 精简为启动 wifi（as_ap.sh）+ 相机驱动 + test_mpp_vo，未加载 framebuffer/多服务 @arprj/configs/ar9341/cr10_config/scenario/run.sh#1-64 vs @arprj/configs/ar9341/cr20_config/scenario/run.sh#1-20

- Logo/测试资源文件  
  - cr10 有 `start_logo_720x1440_stride1024.yuv`、`test_vo.yuv`；cr20 有 `start_logo_400x960_stride512.yuv`、`test_rc_400x960_stride512.yuv`

- U-Boot 环境文本（env_emmc.txt）  
  - `bootcmd` 中 VO 启动画面分辨率不同：cr10 使用 1024 stride/720x1440，cr20 使用 512 stride/400x960；对应 logo 文件路径也不同 @arprj/configs/ar9341/cr10_config/uboot/env_emmc.txt#1-10 vs @arprj/configs/ar9341/cr20_config/uboot/env_emmc.txt#1-10

- 附带文档/脚本  
  - cr10 多出 `plat/README.md`, `plat/notes.md`, `plat/tips.md`, `scenario/run0.sh`, `scenario/run1.sh`, `uboot/env_emmc.txt.md`；cr20 无这些。
我按你的约束收敛结论：**不再把 Linux 侧 `test_mpp_vo` / framebuffer / 触摸屏差异纳入判断**，只保留 U-Boot/底层显示链路是否为不同驱动这一点。
# 修正后的结论

你这个限定很关键。**如果不分析 Linux 侧 `test_mpp_vo`、[GT1X.ko](cci:7://file:arprj/release/prebuilds/ko/GT1X.ko:0:0-0:0)、`ar_framebuffer.ko` 的差异，那么当前结论可以更明确：**

- **CR10 和 CR20 在 U-Boot / 底层显示驱动框架上，看不出使用了不同驱动**
- 两者都使用：
  - 同一个 U-Boot `CONFIG_DEFAULT_DEVICE_TREE="artosyn-ar9301"`
  - 同一个 `CONFIG_VO=y`
  - 同一套 `vo` 命令链路
  - 同一个 `ar_vo_init()` / `ar_vo_dev_enable()` / `ar_vo_layer_enable()` 体系
- 因此目前更像是：
  - **同一套显示控制驱动**
  - **不同的屏参/输出模式/时序参数**

# 现在最有价值的差异点

## U-Boot 配置层面没有换驱动
两边 [u-boot-2020.10.config](cci:7://file:arprj/configs/ar9341/evb_config/uboot/u-boot-2020.10.config:0:0-0:0) 一致：

- `CONFIG_DEFAULT_DEVICE_TREE="artosyn-ar9301"`
- `CONFIG_VO=y`
- `CONFIG_VIDEO` 未启用
- `CONFIG_LCD` 未启用
- `CONFIG_DM_VIDEO` 未启用

这说明它们都不是走“某个专用 LCD driver 切换”的套路，而是走 **VO 显示输出框架**。

## U-Boot 启动参数不同
差异集中在 [env_emmc.txt](cci:7://file:arprj/configs/ar9341/cr10_config/uboot/env_emmc.txt:0:0-0:0) 的 `bootcmd`：

- **CR10**
  ```sh
  vo startdev 0 3 3 3
  vo startlayer 0 0x22400000 1024 0 0 720 1440 0
  ```

- **CR20**
  ```sh
  vo startdev 0 3 2 2
  vo startlayer 0 0x22400000 512 0 0 400 960 0
  ```

这说明实际差异在：

- **输出模式参数不同**
- **sync_type 不同**
- **sub_intf_type 不同**
- **分辨率不同**
- **stride 不同**

再结合 logo 资源：

- CR10: `start_logo_720x1440_stride1024.yuv`
- CR20: `start_logo_400x960_stride512.yuv`

可以判断这非常符合“**同驱动，不同面板参数**”的特征。

# 更准确的表述

如果要回答硬件部门那个问题，我建议你可以这样说：

- **从现有软件配置和源码路径看，CR10/CR20 没有切换成两套不同的 LCD 驱动。**
- **两者使用的是同一套 U-Boot VO 显示驱动框架。**
- **区别体现在 VO 启动参数、显示分辨率、stride、输出子类型/同步类型等面板配置上。**
- **因此更像是“同一驱动框架适配不同厂商/规格屏”，而不是“更换了不同驱动”。**

# 仍然保留的一点技术余量

我会保留一个更严谨的说法：

- 虽然**没有看到不同驱动入口**
- 但 `vo startdev 0 3 3 3` 和 `vo startdev 0 3 2 2` 里的参数，**很可能在同一个 VO 驱动内部切到了不同的面板时序/初始化 profile**

也就是说：

- **不是不同 driver**
- 但**可能是同一个 driver 内部的不同 mode / panel profile**

# 最终结论

**你这次问题的范围下，结论就是：CR10 和 CR20 实际不是不同 LCD 驱动，而是同一套 VO 驱动框架下使用了不同的显示参数/屏参。**

# 状态

已完成结论收敛：**排除 Linux 调试阶段差异后，当前判断为“同驱动，不同屏参/模式”。**


# 三、具体UBoot代码

我继续往下追 `CONFIG_VO` 对应的 U-Boot 代码，把 `vo startdev` / `vo startlayer` 的参数一路映射到具体结构体和底层调用，确认 `3/3/3` 与 `3/2/2` 分别控制了什么。


我已经拿到命令入口和 [drivers/vo/vo.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/vo.c:0:0-0:0) 的核心逻辑了，再补一层枚举定义和 RPC 分发，这样能把 `3/3/3`、`3/2/2` 精确翻译成“哪个 MIPI panel + 哪个 timing template”。

我已经把参数到结构体、再到 RPC/显示核心的路径串起来了。下面直接给你“`CONFIG_VO` 对应什么代码、环境变量里每个数字最终变成什么”的结论。
# `CONFIG_VO=y` 对应的 U-Boot 代码路径

## 1. 配置打开了什么
`CONFIG_VO=y` 打开的是 U-Boot 里的 **VO 命令和 VO 驱动框架**，不是通用 `CONFIG_LCD` / `CONFIG_VIDEO` 路径。

关键位置：

- [cmd/Kconfig](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/cmd/Kconfig:0:0-0:0)
  - `config CMD_VO`
- [cmd/Makefile](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/cmd/Makefile:0:0-0:0)
  - `obj-$(CONFIG_CMD_VO) += vo.o`
- [common/board_r.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/common/board_r.c:0:0-0:0)
  - 在启动流程里调用 [ar_vo_init()](cci:1://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/vo.c:116:0-120:1)
- [drivers/vo/vo.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/vo.c:0:0-0:0)
  - VO 设备/图层启用实现
- [drivers/vo/display_hal/display_rpc.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/display_hal/display_rpc.c:0:0-0:0)
  - 把属性写入 display server，再实际 enable

所以整体链路是：

```text
CONFIG_VO
 -> CONFIG_CMD_VO
 -> cmd/vo.c
 -> ar_vo_dev_enable() / ar_vo_layer_enable()
 -> ar_vo_rpc_*()
 -> ar_display_*()
```

# 环境变量里的 `vo` 命令如何进入代码

## 2. `bootcmd` 执行的命令入口
你关心的这两句：

```sh
vo startdev 0 3 3 3
vo startlayer 0 0x22400000 1024 0 0 720 1440 0
```

和

```sh
vo startdev 0 3 2 2
vo startlayer 0 0x22400000 512 0 0 400 960 0
```

都由 [cmd/vo.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/cmd/vo.c:0:0-0:0) 解析。

### `startdev`
[cmd/vo.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/cmd/vo.c:0:0-0:0) 中：

```c
dev_id = simple_strtoul(argv[1], NULL, 10);
intf_type = simple_strtoul(argv[2], NULL, 10);
sub_intf_type = simple_strtoul(argv[3], NULL, 10);
sync_type = simple_strtoul(argv[4], NULL, 10);

ar_vo_dev_enable(dev_id, intf_type, sub_intf_type, sync_type);
```

### `startlayer`
```c
layer_id = simple_strtoul(argv[1], NULL, 10);
addr = simple_strtoul(argv[2], NULL, 16);
stride = simple_strtoul(argv[3], NULL, 10);
x = simple_strtoul(argv[4], NULL, 10);
y = simple_strtoul(argv[5], NULL, 10);
w = simple_strtoul(argv[6], NULL, 10);
h = simple_strtoul(argv[7], NULL, 10);
csc = simple_strtoul(argv[8], NULL, 10);

ar_vo_layer_enable(layer_id, addr, stride, x, y, w, h, csc);
```

# `vo startdev` 每个参数的真实含义

## 3. [cmd/vo.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/cmd/vo.c:0:0-0:0) 已经把枚举说明写出来了
这部分很关键，命令帮助里直接写了：

- **`intftype`**
  - `0 - none`
  - `1 - dvp`
  - `2 - dp`
  - `3 - mipi_tx`

- **`subintf` for mipi_tx**
  - `1 - EK79007`
  - `2 - MIPI_DEV_ST7701`
  - `3 - MIPI_DEV_ST7703`

- **`sync`**
  - `for EK79007: 0 - 1024*600@60`
  - `for ST7701: 2 - 480*1440@60`
  - `for ST7703: 3 - 720*1440@60`

## 4. 在 [hal_vo.h](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/include/vo/display_hal/hal_vo.h:0:0-0:0) 里的正式枚举定义
[include/vo/display_hal/hal_vo.h](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/include/vo/display_hal/hal_vo.h:0:0-0:0) 里对应关系是：

### 接口类型
```c
AR_HAL_VO_DEV_INTF_NONE = 0
AR_HAL_VO_DEV_INTF_DVP  = 1
AR_HAL_VO_DEV_INTF_DP   = 2
AR_HAL_VO_DEV_INTF_MIPI = 3
```

### MIPI panel 类型
```c
AR_HAL_VO_MIPITX_PANNEL_NONE   = 0
AR_HAL_VO_MIPITX_PANNEL_EK79007 = 1
AR_HAL_VO_MIPITX_PANNEL_ST7701  = 2
AR_HAL_VO_MIPITX_PANNEL_ST7703  = 3
```

### timing template
```c
AR_HAL_VO_OUTPUT_400x960_60
AR_HAL_VO_OUTPUT_720x1440_60
AR_HAL_VO_OUTPUT_1024x600_60
...
```

注意这里注释里写 `400x960_60`，而 [cmd/vo.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/cmd/vo.c:0:0-0:0) 的 help 文案写成了 `480*1440@60` 给 ST7701。  
**从实际代码实现看，应以 [drivers/vo/vo.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/vo.c:0:0-0:0) 为准。**

# `vo startdev` 最终如何映射到设备属性

## 5. [drivers/vo/vo.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/vo.c:0:0-0:0) 是关键实现
[ar_vo_dev_enable()](cci:1://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/vo.c:5:0-46:1) 里做了真实映射：

```c
dev_attr.bg_color = 0x808080;
dev_attr.e_interface = intf_type;

if (dev_attr.e_interface == AR_HAL_VO_DEV_INTF_MIPI)
{
    dev_attr.u_sub_intance.e_mipi_pannel = sub_intf_type;
    if (sync_type == 2)
    {
        dev_attr.e_timing_template = AR_HAL_VO_OUTPUT_400x960_60;
    }
    else if (sync_type == 3)
    {
        dev_attr.e_timing_template = AR_HAL_VO_OUTPUT_720x1440_60;
    }
}
else if (dev_attr.e_interface == AR_HAL_VO_DEV_INTF_DVP)
{
    dev_attr.u_sub_intance.e_sub_intf = sub_intf_type;
}

dev_attr.bit_count_per_channel = 8;
ret = ar_vo_rpc_dev_set_attr(dev_id, &dev_attr);
ret = ar_vo_rpc_dev_enable(dev_id);
```

## 6. 所以 CR10 / CR20 在 U-Boot 里被翻译成什么

### CR10
```sh
vo startdev 0 3 3 3
```

会变成：

- **`dev_id = 0`**
- **`e_interface = 3 = AR_HAL_VO_DEV_INTF_MIPI`**
- **`e_mipi_pannel = 3 = AR_HAL_VO_MIPITX_PANNEL_ST7703`**
- **`sync_type = 3` -> `e_timing_template = AR_HAL_VO_OUTPUT_720x1440_60`**
- `bit_count_per_channel = 8`

也就是：

- **MIPI DSI**
- **panel 类型：ST7703**
- **时序模板：720x1440@60**

### CR20
```sh
vo startdev 0 3 2 2
```

会变成：

- **`dev_id = 0`**
- **`e_interface = 3 = AR_HAL_VO_DEV_INTF_MIPI`**
- **`e_mipi_pannel = 2 = AR_HAL_VO_MIPITX_PANNEL_ST7701`**
- **`sync_type = 2` -> `e_timing_template = AR_HAL_VO_OUTPUT_400x960_60`**
- `bit_count_per_channel = 8`

也就是：

- **MIPI DSI**
- **panel 类型：ST7701**
- **时序模板：400x960@60**

# 这说明了什么

## 7. 这里已经不是“只有宽高不同”了
到这一步可以更精确地下结论：

- **CR10 和 CR20 使用同一套 VO 驱动框架**
- 但在这个框架里，`sub_intf_type` 已经明确选到了**不同的 MIPI panel 类型**
  - CR10 -> `ST7703`
  - CR20 -> `ST7701`
- `sync_type` 又进一步选到了**不同的 timing template**
  - CR10 -> `720x1440_60`
  - CR20 -> `400x960_60`

所以更准确的说法是：

- **不是两套完全独立的 U-Boot 驱动模块**
- 但**确实在同一套 VO 驱动内部选用了不同 panel profile / panel driver 分支**

如果硬件部门说两块屏厂商和规格完全不同，这个结果是吻合的。

# `vo startlayer` 又做了什么

## 8. 图层参数如何映射
[drivers/vo/vo.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/vo.c:0:0-0:0) 中 [ar_vo_layer_enable()](cci:1://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/vo.c:53:0-109:1)：

```c
layer_attr.width = w;
layer_attr.height = h;
layer_attr.luma_stride = stride;
layer_attr.chroma_stride = stride / 2;
layer_attr.format = AR_HAL_VO_FMT_YV12;
layer_attr.fps = 30;
ret = ar_vo_rpc_layer_set_attr(layer_id, &layer_attr);

layer_csc.input = (ENUM_AR_HAL_VO_CSC)csc;
layer_csc.output = (ENUM_AR_HAL_VO_CSC)csc;
ret = ar_vo_rpc_layer_set_csc(layer_id, &layer_csc);

ret = ar_vo_rpc_layer_enable(layer_id);

pos.x = x;
pos.y = y;
ret = ar_vo_rpc_layer_set_position(layer_id, &pos);

buffer.frame_id = 0;
buffer.panel_count = 3;
buffer.pannel[0].buffer_pa = (void *)addr;
buffer.pannel[1].buffer_pa = (void *)addr + stride * h;
buffer.pannel[2].buffer_pa = (void *)addr + stride * h * 5 / 4;
ret = ar_vo_rpc_layer_enqueue(layer_id, &buffer);
```

所以 `startlayer` 的参数含义是：

- **`addr`**
  - YV12 图像首地址
- **`stride`**
  - Y 分量 stride
  - UV stride 自动设成 `stride / 2`
- **`w/h`**
  - 图层宽高
- **`x/y`**
  - 显示位置
- **`csc`**
  - 色彩空间转换模式
- 图像格式固定为 **`AR_HAL_VO_FMT_YV12`**

## 9. CR10 / CR20 的 `startlayer` 差异
### CR10
```sh
vo startlayer 0 0x22400000 1024 0 0 720 1440 0
```

对应：

- 图层尺寸：`720 x 1440`
- luma stride：`1024`
- chroma stride：`512`
- buffer layout 按 `720x1440` 的 YV12 解释
- csc：`0 = BT709 limited`

### CR20
```sh
vo startlayer 0 0x22400000 512 0 0 400 960 0
```

对应：

- 图层尺寸：`400 x 960`
- luma stride：`512`
- chroma stride：`256`
- buffer layout 按 `400x960` 的 YV12 解释
- csc：`0 = BT709 limited`

# RPC 层和显示核心在做什么

## 10. [ar_vo_rpc_dev_set_attr()](cci:1://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/display_hal/display_rpc.c:84:0-106:1) 只是把属性存起来
[drivers/vo/display_hal/display_rpc.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/display_hal/display_rpc.c:0:0-0:0)：

```c
server->hal.vo_dev.attr = *para;
```

然后 [ar_vo_rpc_dev_enable()](cci:1://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/display_hal/display_rpc.c:4:0-44:1) 再真正启设备：

```c
disp_handler = ar_get_display_layer(AR_HAL_VO_LAYER_ID_VIDEO_0);
ret = ar_init_display(disp_handler, AR_HAL_VO_LAYER_ID_VIDEO_0);
ret = ar_display_dev_enable();
```

也就是说：

- `startdev` 先把 panel/type/timing 填进 `server->hal.vo_dev.attr`
- 然后 enable 时使用这些属性去初始化显示设备

## 11. `startlayer` 则把 layer 属性写入 server，再启图层
同样在 [display_rpc.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/display_hal/display_rpc.c:0:0-0:0)：

- [ar_vo_rpc_layer_set_attr()](cci:1://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/display_hal/display_rpc.c:286:1-315:1)
- [ar_vo_rpc_layer_enable()](cci:1://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/display_hal/display_rpc.c:166:1-233:1)

启图层时用的是：

```c
server->hal.vo_layer[e_layer_id].attr.width
server->hal.vo_layer[e_layer_id].attr.height
server->hal.vo_layer[e_layer_id].attr.luma_stride
server->hal.vo_layer[e_layer_id].attr.chroma_stride
```

而且 layer enable 时还会取：

```c
obj->res_timing.fps
```

也就是**设备时序和图层属性是联动的**。

# 最终结论

## 结论一：`CONFIG_VO=y` 对应的是同一套 VO 驱动框架
两边都走：

- [cmd/vo.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/cmd/vo.c:0:0-0:0)
- [drivers/vo/vo.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/vo.c:0:0-0:0)
- [drivers/vo/display_hal/display_rpc.c](cci:7://file:arsdk/platforms/uboot/u-boot-2020.10/drivers/vo/display_hal/display_rpc.c:0:0-0:0)
- `drivers/vo/display_core/*`

**不是两份不同源码驱动。**

## 结论二：CR10/CR20 在这套框架内部确实选中了不同 panel 分支
环境变量不是随便填宽高，而是明确控制了：

- **CR10**
  - `intf_type=3` -> `MIPI`
  - `sub_intf_type=3` -> `ST7703`
  - `sync_type=3` -> `720x1440@60`

- **CR20**
  - `intf_type=3` -> `MIPI`
  - `sub_intf_type=2` -> `ST7701`
  - `sync_type=2` -> `400x960@60`

所以更精确的技术表述应是：

- **驱动框架相同**
- **但在 VO 驱动内部选择了不同的 MIPI panel 类型与 timing template**
- 因而可以认为：**不是不同“模块”，但确实是同一驱动中的不同 panel 驱动分支/profile**

## 结论三：`startlayer` 只是在匹配对应 panel 的图像参数
它负责把 logo 按匹配该 panel 的方式送入图层：

- CR10 -> `720x1440`, stride `1024`
- CR20 -> `400x960`, stride `512`

# 状态

已定位并解释清楚：**`CONFIG_VO=y` 对应同一套 U-Boot VO 代码；CR10/CR20 的差异具体落在 `ST7703 + 720x1440_60` 与 `ST7701 + 400x960_60` 两组 panel/timing 配置上。**