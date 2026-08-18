# VIO命令行参数详解

本文档详细描述了`test_mpp_vio`可执行文件支持的所有命令行参数，包括其功能、取值范围、使用场景及内部实现机制。

## 核心参数

### -index <n>
**功能**: 指定要运行的示例程序索引
**取值范围**: 12-107（具体支持的索引取决于编译配置）
**实现机制**: 
- 在`main()`函数中通过`sample_find_pra_by_name()`查找参数位置
- 将字符串转换为整数，赋值给`s32Index`
- 通过switch-case语句选择对应的示例函数
**示例**: `./test_mpp_vio -index 83`

### -sensor <n>
**功能**: 选择使用的传感器索引
**取值范围**: 0-N（取决于系统连接的传感器数量）
**实现机制**:
- 解析为`vio_pra.u32Sensor`变量
- 在示例函数中通过传感器数组`g_enSnsType`选择对应传感器
- 影响VI设备配置和传感器特定参数
**示例**: `./test_mpp_vio -index 83 -sensor 0`

### -vo <n>
**功能**: 指定视频输出设备类型
**取值范围**: 
- 0: HDMI输出（默认）
- 1: MIPI输出
**实现机制**:
- 解析为`vio_pra.u32votype`变量
- 在显示初始化时，值为1调用`ar_vo_dev_init_for_mipi()`，值为0调用`ar_vo_dev_init()`
- 影响显示分辨率、同步信号和接口配置
**示例**: `./test_mpp_vio -index 83 -vo 1`

## 调试参数

### -no_sig
**功能**: 禁用信号处理（SIGINT/SIGTERM）
**实现机制**:
- 存在此参数时，跳过`signal(SIGINT, SAMPLE_VIO_HandleSig)`和`signal(SIGTERM, SAMPLE_VIO_HandleSig)`调用
- 用于在调试环境中防止Ctrl+C中断程序
**使用场景**: 自动化测试或后台运行

### -nframes <n>
**功能**: 指定处理的帧数上限
**取值范围**: 正整数（-1表示无限）
**实现机制**:
- 解析为`vio_pra.s32nframes`变量
- 在主循环中设置计数器，达到指定帧数后自动退出
- 用于性能测试和自动化验证
**示例**: `./test_mpp_vio -index 83 -nframes 100`

## 系统配置参数

### -cam_mode <n>
**功能**: 设置相机工作模式
**取值范围**:
- 0: 在线模式（online）
- 1: 离线模式（offline）
- 2: 多通道模式（multi）
- 其他值: 自动模式
**实现机制**:
- 解析为`vio_pra.cam_mode`变量
- 作为参数传递给`SAMPLE_AR_MPI_VIN_OpenDev()`函数
- 影响VI设备和ISP的工作模式配置

### -dpcm <n>
**功能**: 设置DPCM（差分脉冲编码调制）模式
**取值范围**:
- 0: 默认模式
- 1: 启用DPCM压缩
- 2: 禁用DPCM压缩
**实现机制**:
- 解析为`vio_pra.dpcm`变量
- 设置到`stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm`
- 影响视频数据的压缩和传输效率

### -load_vi_driver <n>
**功能**: 控制是否加载VI驱动
**取值范围**:
- 0: 不加载（使用库模式）
- 1: 加载驱动
**实现机制**:
- 设置全局变量`g_s32LoadVinDriver`
- 影响设备初始化和资源管理模式

## 频率配置参数

### --fre -mipi <freq> -vif <freq> -isp <freq> -pcs <freq>
**功能**: 分别设置MIPI、VIF、ISP、PCS模块的工作频率
**单位**: Hz
**实现机制**:
- 需要配合`--fre`参数使用
- 各频率值分别解析到`vio_pra.mipi_fre`、`vio_pra.vif_fre`、`vio_pra.isp_fre`、`vio_pra.pcs_fre`
- 作为参数传递给`SAMPLE_AR_MPI_VIN_OpenDev()`
**示例**: `./test_mpp_vio --fre -mipi 100000000 -vif 300000000 -isp 100000000 -pcs 100000000`

### -settle <n>
**功能**: 设置MIPI settle时间
**取值范围**: 无符号整数（具体值取决于硬件规格）
**实现机制**:
- 解析为`vio_pra.settle`变量
- 设置到`stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle`
- 影响MIPI接口的信号稳定性和传输质量

## 功能特性参数

### -feature_mask <mask>
**功能**: 设置功能特性掩码
**取值范围**: 位掩码（十六进制）
**位定义**:
- 位0: FEA_AR_MPI_ISP_GetVDTimeOut
- 位1: FEA_AR_MPI_ISP_GetAEStatistics
- 位2: FEA_AR_MPI_ISP_GetWBStatistics
- 位3: FEA_AR_MPI_ISP_GetFocusStatistics
**实现机制**:
- 解析为`vio_pra.feature_mask`变量（使用strtoul进行16进制转换）
- 设置到`stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask`
- 启用或禁用特定的ISP功能
**示例**: `./test_mpp_vio -feature_mask 0xF`（启用所有特性）

### -suspend <n>
**功能**: 设置挂起模式
**取值范围**: 取决于具体实现
**实现机制**:
- 解析为`vio_pra.sus_mode`变量
- 用于测试设备的挂起/恢复功能

## Big Pic Demo专用参数

### -video <n>
**功能**: 设置视频模式（Big Pic Demo专用）
**实现机制**:
- 仅在示例索引46（SAMPLE_VIO_Big_Pic_Demo）中有效
- 解析为`vio_pra.s32CameraMode`变量

### -res <n>
**功能**: 设置分辨率模式（Big Pic Demo专用）
**实现机制**:
- 仅在示例索引46中有效
- 解析为`vio_pra.s32SensorRes`变量

### -hdr
**功能**: 启用HDR模式（Big Pic Demo专用）
**实现机制**:
- 仅在示例索引46中有效
- 设置`vio_pra.s32IsHdrOn = 1`

### -dir <path>
**功能**: 指定文件输入目录（Big Pic Demo专用）
**实现机制**:
- 仅在示例索引46中有效
- 设置`vio_pra.s32IsFromFile = 1`和`vio_pra.pstrDirName`

### -bigpic
**功能**: 启用大图像模式（Big Pic Demo专用）
**实现机制**:
- 仅在示例索引46中有效
- 设置`vio_pra.s32Cmd = 5`

### -preview
**功能**: 启用预览模式（Big Pic Demo专用）
**实现机制**:
- 仅在示例索引46中有效
- 设置`vio_pra.s32Cmd = 4`

### -take <n>
**功能**: 拍照模式，指定拍照帧数（Big Pic Demo专用）
**实现机制**:
- 仅在示例索引46中有效
- 设置`vio_pra.s32Cmd = 3`和`vio_pra.s32nframes`

### -stop
**功能**: 停止模式（Big Pic Demo专用）
**实现机制**:
- 仅在示例索引46中有效
- 设置`vio_pra.s32Cmd = 2`

### -server
**功能**: 服务器模式（Big Pic Demo专用）
**实现机制**:
- 仅在示例索引46中有效
- 设置`vio_pra.s32Cmd = 1`

## 示例特定参数

### 曝光控制参数
- **-exp <0/1/2/3>**: 设置曝光策略（示例索引78专用）
- **-back <0/1/2>**: 设置背光补偿模式（示例索引79专用）
  - 0: 关闭补偿
  - 1: 启用补偿
  - 2: 抑制背光

### 图像处理参数
- **-size1 <w,h>**: 设置第一个图像尺寸（示例索引81专用）
- **-size2 <w,h>**: 设置第二个图像尺寸（示例索引81专用）
- **-dis_anti <0/1>**: 禁用/启用抗锯齿（示例索引81专用）

### 传感器测试参数
- **-i2c**: I2C测试模式（特定示例）
- **-i2c1**: 备用I2C测试模式
- **-dvp_first**: DVP优先模式
- **-sns_type**: 指定传感器类型
- **-pipe_valid**: 管道有效性测试
- **-byte_swap**: 字节交换测试

### 高级配置参数
- **-lowdealy**: 低延迟模式
- **-scan_skew**: 扫描偏移测试
- **-lane**: 通道配置
- **-random_deskew**: 随机去偏移测试
- **-fps**: 帧率设置
- **-check**: 检查模式

## 参数解析机制

### 查找算法
所有参数通过`sample_find_pra_by_name()`函数查找：
```c
AR_S32 sample_find_pra_by_name(AR_CHAR *name, AR_S32 argc, AR_CHAR *argv[]) {
    for (i = 0; i < argc; i++) {
        if (!strcmp(name, argv[i])) {
            return i;  // 返回参数位置
        }
    }
    return -1;  // 未找到
}
```

### 值解析
- 整数值：使用`atoi()`转换
- 无符号长整型：使用`strtoul()`转换（支持16进制）
- 字符串：直接使用`argv[index+1]`

### 参数验证
- 大多数参数采用“存在即有效”的策略
- 参数值范围验证通常由具体示例函数负责
- 无效参数值可能导致示例函数返回错误

## 使用示例

### 基本使用
```bash
# 基本VIO示例，HDMI输出
./test_mpp_vio -index 12

# MIPI输出，处理100帧后退出
./test_mpp_vio -index 83 -vo 1 -nframes 100

# 指定传感器和相机模式
./test_mpp_vio -index 12 -sensor 0 -cam_mode 0
```

### 高级配置
```bash
# 自定义频率配置
./test_mpp_vio -index 83 --fre -mipi 100000000 -vif 300000000

# 启用特定ISP功能
./test_mpp_vio -index 83 -feature_mask 0xF

# Big Pic Demo专用参数
./test_mpp_vio -index 46 -video 1 -res 1080 -hdr -take 10
```

## 注意事项

1. **参数顺序**: 参数顺序不影响功能，但每个参数必须紧跟其值
2. **示例特定**: 部分参数仅在特定示例索引下有效
3. **硬件依赖**: 某些参数的有效性取决于硬件配置
4. **默认值**: 未指定的参数使用默认值
5. **调试模式**: 使用`-no_sig`可在调试时防止信号中断
6. **性能调优**: 频率参数需要根据硬件规格合理设置