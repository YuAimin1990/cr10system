#sdk-ars31-0.16.03 | sdk-ar9341-0.16.03
commit: dc1b51513
*---------*
* Feature *
*---------*
    [SYS]
        1. [AR9341]pll调整频率时使能deglitch功能(A53+NPU+DSP)
        2. 支持 SPL 和 UBOOT 下的 EMMC 的 HS200 模式
        3. 新增直方图统计工具
        4. 新增获取vb pool信息的接口
        5. ddr retention时支持网络功能

    [VO]
        1. 增加api支持用户订阅中断相关事件的功能
        2. 使用VO时序模板时支持自定义帧率
        3. [ARS31]dphy模拟设置成反相

    [VENC]
        1. 新增编码器设置旋转与镜像的接口

    [VDEC]
        1. 支持解码器对随机加扰的数据解码
        2. 支持解码器解码错误和统计信息上报
        3. 支持统计解码的耗时信息
        4. 解码后的帧支持cached和non-cached配置
        5. 支持通过dts配置编解码器的cjson配置文件

    [CODEC]
        1. 支持动态配置vpu timeout

    [ACODEC]
        1. 支持retention

    [GE2D]
        1. 增加usage统计

    [VIN]
        1. 支持ISP输出数字图像到buffer指定的roi区域
        2. 增加it66021 hdmirx驱动的支持

    [NPU]
        1. [AR9341] NPU Runtime 支持网络 callback 同时在ARM/DSP侧进行
        2. 增加NPU Callback耗时/注册信息统计

    [AI]
        1. 支持获取cached buffer降低cpu loading

*--------*
* Sample *
*--------*
        1. 修复单进程模式下ipcam load vi driver的问题
        2. 修复解码sample无法正常解析mp4文件问题
        3. 支持JPEG单帧送单帧取
        4. 修正红外ISP不出图的问题
        5. 修复retention失败的问题

*---------*
*   Bug   *
*---------*
    [SYS]
        1. [ARS31] 修复uboot下某些模块偶尔reset不生效的问题
        2. [ARS31]修复Nand Flash W25N02KW ECC bitflip错误
        3. 修复nand flash擦除错误
        4. 修复spl超长导致image烧录启动失败的问题
        5. [AR9341]修复SD卡100M source 100M clock拔插后失败的问题
        6. 修复emmc写数据概率性错误
        7. 解决 skip training 时 ddr 数据错误的问题
        8. 解决 emmc retry出错的问题
        9. 解决某些类型的SD卡不能自动mount的问题
        10. 解决SPL概率性出现超过容量错误

    [VO]
        1. 修复某些情况下无法disable Channel的问题

    [VENC]
        1. venc bitsteam buffer添加保护防止回踩
        2. [严重]修复编码invalid cache晚导致的图像错误

    [VDEC]
        1. 修复解码器无法解析部分rtsp网络流的问题
        2. 优化解码器占用mmz
        3. 修复多进程模式下get userdata出错的问题
        4. 修复解码器取pts不正确
        5. 修复解码face.h264 码流(IPB+usrdata)时，帧率不断下降的性能问题

    [VIN]
        1. 修复三星gn2 sensor 4K60 mipi不出图的问题
        2. 修复数字gain不生效的问题


#sdk-ars31-0.16.02 | sdk-ar9341-0.16.02
*---------*
* Feature *
*---------*
    [VDEC]
        1. 解码器支持动态设置通道宽高参数

    [NPU]
        1. 增加buidDate/buildID/NPUSize 打印信息

*--------*
* Sample *
*--------*
        1. 增加vdec动态切换分辨率的例子

*---------*
*   Bug   *
*---------*
    [SYS]
        1. [AR9341]修复安全启动时校验u-boot出错的问题
        2. 修复无法通过usb烧录spl和uboot的问题
        3. 去除sysbind的多余打印
        4. 修正板型ipcnand/nandddr4编译错误的问题

    [VIN]
        1. 修正大图出现缝隙的问题
        2. [AR9341]修正ISP模块3Ddenoise未生效的问题
        3. 播放音频时echo vb会引起mpp_service段错误
        4. 修正pipe取流内存泄露的问题

    [VENC]
        1. 修复jpeg编码出现马赛克的问题

    [VGS]
        1. 增加保护防止用户贴图时未初始化参数导致位置偏移的问题

    [VDEC]
        1. 修复解码器退出时，内存泄露的问题
        2. 修复随机加扰数据出现buffer full，导致解码失败

    [NPU]
        1. [AR9341]去除启动过程中配置NOC为700M的流程

    [VO]
        1. 修复销毁通道耗时过长的问题

    [SAMPLE]
        1. 修复sample中suspend和resume时遗漏vo流程导致失败的问题
        2. [AR9341]修复vgs/audio/venc sample打开ar_sysctl失败的问题
        3. 修复ipcam的若干问题
        4. 修复venc sample读yuv数据的问题
        5. 修复vdec的文件名字符串长度被限制为40的问题
        6. 修复test_mpp_vio的若干问题

#sdk-ars31-0.16.01
*--------*
* Sample *
*--------*
    1. IPcam case 29支持suspend/resume流程

*--------*
*  Bugs  *
*--------*
    [VENC]
    1. 修正DDR retention场景下未删除proc下venc节点和irq number导致crash的问题
    2. 修正了reset channel时的状态机错误

    [VDEC]
    1. 修正解码h26x/mjpeg时MMB泄露，堆栈异常的问题
    2. 修正解码1080/4K jpeg_to_yuv时段错误的问题

    [SAMPLE]
    1. 修正ipcam测试用例中未创建venc recv frame导致进程fail的问题
    2. 修正vgs测试用例中在DDR retention resume时未销毁vo instance导致的休眠错误


#sdk-ars31-0.16.00 | sdk-ar9341-0.16.00
*---------*
* Feature *
*---------*
    [SYS]
    1. 优化版本发布流程，sdk发布的库和app的不再带debug信息
    2. 支持通过 ddr-user-modify 配置不同的半自动化 tuning 参数
    3. 增加脚本进行板上库文件与SDK发布的MD5值比对
    4. [ar9311] uart烧写efuse的功能
    5. [ar9311] ddr retention功能正式发布（除网口驱动）

    [VENC]
    1. 增加升帧功能
    2. 增加抽帧功能
    3. 添加I帧qp和非I帧qp统计
    4. 添加带宽统计debug信息
    5. 增加AR_MPI_VENC_GetStreamBufInfo接口获取流信息

    [VIN]
    1. 添加每个通道的亮度平均值
    2. 新增AR_MPI_ISP_GetPipeLimitAttr() 获取曝光最大值和最小值
    3. 修正dvp8路随机插拔不出图的问题
    4. 实现ch0 动态修改分辨率的功能
    5. [ars31] 红外gtm映射表和gain增加平滑处理

    [REGION]
    1. Region支持nv12格式
    2. 支持压缩格式

    [DSP]
    1. [ar9341] 添加fast9 CEAVCV库
    2. [ar9341] 封装noblock ddma 接口，并增加ping-pong使用案例

    [VGS]
    1. 增加 osd/rotation close alpha blending功能

    [VPSS]
    1. vpss group支持动态调整分辨率功能
    2. 支持nv12格式

*---------*
*   Bug   *
*---------*
    [SYS]
    1. 添加axi dma cci enable时address对齐检查
    2. 修复频繁sys int和exit后操作文件程序异常的问题
    3. 修正大于256M分区大小升级nand flash异常的问题
    4. [ar9341] 优化A53自动调频功能
    5. [ar9311] 修正emmc上烧录固件偶现失败的问题
    6. [ar9311] 修复evb reboot不生效的问题
    7. [ar9311] 修复gpio 中断重复使能产生错误

    [ACODEC]
    1. [ar9341] 播放音频时候执行mv会卡顿

    [VIN]
    1. 修正调用reshape api 后调用crop api导致的图像异常
    2. [ar9341] 修正vif上下电时序

    [VENC]
    1. 修复若干稳定性问题
    2. cat /proc/umap/hal_vdec/hal_vdec导致系统卡住
    3. rcMode=vbr模式下 gop size 与设定值有偏差

    [DSP]
    1. 解决ceva-cv算子库静态占用Dtcm空间
    2. 解决ceva-cv算子计算速度满的问题

    [VDEC]
    1. 修复若干稳定性问题
    2. vdec 获取码流timeout后 bind出现逻辑错误问题

    [NPU]
    1. 修正部分网络报内存错误的问题
    2. 修复cnn operator nchw/nhwc 转换错误
    3. 支持多输入场景下零拷贝input tensor

    [VO]
    1. 修复VO部分启停问题
    2. 修复vo的data极性无法修改的问题
    3. uboot开机画面在mpp service启动后异常
    4. [ar9311] 修复9311 clock分频比计算问题


#sdk-ars31-0.15.02
*---------*
*   Bug   *
*---------*
    [clock]
    1. 去除错误的venc clock开关操作，该操作会导致venc无法正常工作


#sdk-ars31-0.15.01 | sdk-ar9341-0.15.01
*---------*
* Feature *
*---------*
    [SYS]
    1. 合入多个cpu占有率优化的修改
    2. emmc电压域固定到1.8v
    3. 增加DDR3L内存的支持

    [VIN]
    1. 改善红外gtm算法的稳定性，避免微小的场景变化时会引起图像闪动
    2. 添加红外亮度、对比度等图像效果API
    3. 为红外sensor参数标定添加用户自定义配置
    4. AWB,AE统计中携带时间戳
    5. 添加帧中的3a信息同步
    6. 增加双cam使用单路曝光和awb模式的支持
    7. 增加通道3的crop操作
    8. 添加AEC统计跳帧
    9. 集成新的gtm算法
    10. 提供设置图像伽马参数接口

    [VDEC]
    1. 支持提取码流各帧的关键信息

    [IFC]
    1. 增加获取参数接口

    [VO]
    1. 增加hdmi 1080P50的支持

    [NPU]
    1. 增加Oonnx-Resize Callback算子
    2. 增加Callback Version 信息

    [GDC]
    1. 优化generate lut处理逻辑。用户传入lut表的情况下，不需要再计算

*--------*
* Sample *
*--------*
    1. 增加双路raw图的测试用例
    2. ir_sample将显示改为1080p30

*------*
* Bugs *
*------*
    [SYS]
    1. 修复各类由于单进程模式导致启停出现崩溃的问题
    2. 修正对时导致多媒体pipe异常的问题

    [ACODEC]
    1. 修正音频播放中部分出现崩溃的问题

    [VIN]
    1. 修正红外gtm在某些场景下溢出的问题
    2. 修正通道某些情况下帧率不准的问题

    [DSP]
    1. [ar9341] 解决ICC多线程测试中d重复收到同一条消息的问题
    2. [ar9341] 修正get_dev_info.sh脚本计算dsp频率错误

    [VENC]
    1. 修正rcMode为fixqp模式下h26x的I帧只有1帧
    2. 修正通道某些情况下帧率不准的问题
    3. 修正jpeg vbr模式min/max qp_coef设置无效的问题
    4. 修正mjpeg模式下设置RC参数crash的问题

    [SCALER]
    1. 修正等比缩放，宽高比例相同情况下缩放效果错误的问题

    [VDEC]
    1. 解决多路解码中一路失败导致其他路也停止解码的问题
    2. 解决bs buf full的问题
    3. 修正解码rtsp IPB流的时候会卡住的问题
    4. 修正多次设置后通道的全局参数不生效的问题
    5. 解决销毁通道时偶发的crash问题

    [NPU]
    1. 修正cnn operator NCHW/NHWC转换错误

    [VO]
    1. 修正外同步interlace的场景下，相邻两个vsync没有找到半行的hsync
    2. 单进程模式下，framebuffer驱动在应用启动前加载失败

#sdk-ars31-0.15.00 | sdk-ars9341-0.15.00
*---------*
* Feature *
*---------*
    [SYS]
	1.  新增mpp_service lib模式的支持
	2.  实现linux reboot命令
	3.  增加pstore持久化存储
	4.  提供efuse接口用于烧录客户的密钥
	5.  重新打开cpu的cci，同时使能axi dma的cci
	6.  实现ar_top统计cpu占有率
	7.  CIPHER增加支持RSA、SHA、TRNG模式
	8.  实现secure boot的全流程验签（从spl到rootfs）
	9.  [ar9341] 动态调频功能的接入

    [VENC]
	1.  使用dts获取codec配置文件路径的接口
	2.  添加扩展参数接口支持开关多实例并行
	3.  添加done msg pool自动扩容机制
	4.  添加venc keyframe size配置接口

    [VIN]
	1.  在AEC信息中添加Lux Index的配置
	2.  添加api配置pipe的扩展属性
	3.  增加mipi rx虚拟通道的支持
	4.  AEC/AWB等模式修改为枚举
	6.  获取图像的时间戳单位从ms改为us
	7.  电源的clk属性中添加src字段
	8.  增加pipe属性使能非压缩，默认打开压缩
	9.  增加支持cf50压缩属性
	10. 增加通道属性以支持多种缩放模式
	11. 实现数字宽动态调整接口
	12. 新增API实现曝光策略控制
	13. 新增API实现背光控制
	14. [ars31]集成简化版的红外gtm算法
	15. [ars31]添加设置、读取红外算法tuning参数的mpp接口
	16. [ars31]集成红外gtm 5x5滤波算法
	17. [ar9311]添加acm属性api

	[VO]
	1.  重新整理DVP OUT管脚定义属性配置
	2.  [ar9311] VO增加uboot加载logo功能

	[NPU]
	1.  新增API设置每帧forward的timeout时间

	[GDC]
	1.  新增API使用通用的AR_IMG_S结构做参数

	[DSP]
	1.  支持DACU/IACU/Stack Violation exception

*--------*
* Sample *
*--------*
	1.  ipcam raw压缩可通过json关闭，方便tool输入raw图测试
	2.  npu sample中ifc stride根据ifc_register.json获取
	3.  [ars31]ipcam 增加双光融合的支持
	4.  [ars31]ipcam 添加f23，f23+307sample
	5.  [ars31]ipcam支持ov05a20+gst212w4

*------*
* Bugs *
*------*
     1. 修正若干已知bug


#sdk-ars31-0.14.03
*---------*
* Feature *
*---------*
    1. 暂无

*--------*
* Sample *
*--------*
    1. 添加红外离线灌图sample用例
    2. 添加高德gst212w4驱动及示例程序
    3. ipcam支持ov04a10+gst212w4

*------*
* Bugs *
*------*
     1. 修正若干已知bug

#sdk-ar9341-0.14.02
*---------*
* Feature *
*---------*
    [CONFIG]
    1. 新增IPCNAND和LGA版型配置

    [VIN]
    1. 支持SC2210传感器
    2. 帧信息中增加当前曝光时间
    3. 添加AEC降采样Tuning参数

*--------*
* Sample *
*--------*
    1. 增加imx307两通道出图和raw保存成文件的测试用例
    2. 修改YOLOv5示例以适配不同SoC

*------*
* Bugs *
*------*
    1. 修复若干已知bug

#sdk-ar9301-0.14.01
[Feature]
        [Sys]
        1. 编译框架支持多芯片选择（CHIP= ARS31）

        [PROC]
        1. 改善VPSS/VENC/VIN/NPU/DSP/Scaler/GDC/IFC模块调试信息可读性

        [Codec]
        1. 支持多pack的拿码流方式
        2. 优化test_mpp_vdec中解码所配置的内存参数
        3. 去除JPEG编码末尾的padding 数据

        [VIN]
        1. 新增红外传感器GST417W,GST412W,GST612W 驱动

        [VO]
        1. 新增Clock选择功能
        2. 新增相位配置功能

        [Sample]
        1. 新增双光OS04A10+GST417W IPCam示例
        2. 新增9311/ARS32 UVC 示例

#sdk-ar9301-0.13.1
[Feature]
	[Sys]
	1. 支持DDR3驱动
	2. 支持tp2803驱动 （注：EVB板需修改硬件电阻以适配）
	3. 支持LGA模组版型
	4. 新增input子系统相关的kernel config 配置， gpio button dts 配置 （详见《AR93xx_demo使用说明》第一章外设demo说明）
	5. OTA 升级支持不同版型
	6. 修复PTS时间戳

	[Codec]
	1. 支持profile可配置以及合法性校验

	[VI]
	1. 支持MIPI 多pipe 解虚拟通道
	2. 支持大图模式 （详见《AR93xx_SDK大图模式使用指南》）

	[VO]
	1. 优化VO Channel Buffer缓存机制
	2. 支持设置显示通道 (API: AR_MPI_VO_ShowChn)
	3. 支持设置隐藏通道 (API: AR_MPI_VO_hideChn)
	4. 支持 VO channel优先级配置

	[NPU]
	1. 支持 NCHW 格式 tensor 浮点定点转换
	2. 支持Callback 多输入的网络

	[DSP]
	1. 支持baremetal及freertos版本（通过编译选项选择）

[Sample]
	1. ipc_conifg_debug配置中支持nand nor多种配置
	2. IPCAM 支持单路Audio in、Audio out （EVB 版型）
	3. IPCAM 支持动态修改VPSS 镜像与翻转的操作
	4. 新增 Wifi demo （详见《AR93xx_demo使用说明》）
	5. 新增 UART demo （详见《AR93xx_demo使用说明》）
	6. 新增多媒体 demo （详见《AR93xx_demo使用说明》）
	7. 新增集成工具 （版型信息，DDR带宽统计，chipID工具等，详见《AR93xx_demo使用说明》）
	8. 新增 OS04A10 sensor binning模式驱动
[Bugs]
	Fixed known bugs; 修复已知bugs

[文档]
	1. 更新《AR93xx_Sensor调试指南》
	2. 新增《AR93xx_demo使用说明》
	3. 新增《AR93xx_SDK大图模式使用指南》
	4. 更新《AR93xx_DSP开发指南》
	5. 更新《AR93xx_智能视频子系统API说明》
	6. 更新《AR93xx_SDK_API_List_Customer_Check_20220721》
	7. 去除《AR93xx_ISP_Tool_Guide》（后续单独在Tuning Tool 工具包中维护）

#sdk-ar9301-0.13.0

[Feature]
	[Sys]
	1. 支持DDR3驱动
	2. 支持tp2803驱动 （注：EVB板需修改硬件电阻以适配）
	3. 支持LGA模组版型
	4. 新增input子系统相关的kernel config 配置， gpio button dts 配置 （详见《AR93xx_demo使用说明》第一章外设demo说明）
	5. OTA 升级支持不同版型

	[Codec]
	1. 支持profile可配置以及合法性校验

	[VI]
	1. 支持MIPI 多pipe 解虚拟通道
	2. 支持大图模式 （详见《AR93xx_SDK大图模式使用指南》）

	[VO]
	1. 优化VO Channel Buffer缓存机制
	2. 支持设置显示通道 (API: AR_MPI_VO_ShowChn)
	3. 支持设置隐藏通道 (API: AR_MPI_VO_hideChn)
	4. 支持 VO channel优先级配置

	[NPU]
	1. 支持 NCHW 格式 tensor 浮点定点转换

	[DSP]
	1. 支持baremetal及freertos版本（通过编译选项选择）

[Sample]
	1. ipc_conifg_debug配置中支持nand nor多种配置
	2. IPCAM 支持单路Audio in、Audio out （EVB 版型）
	3. 新增 Wifi demo （详见《AR93xx_demo使用说明》）
	4. 新增 UART demo （详见《AR93xx_demo使用说明》）
	5. 新增多媒体 demo （详见《AR93xx_demo使用说明》）
	6. 新增集成工具 （版型信息，DDR带宽统计，chipID工具等，详见《AR93xx_demo使用说明》）
	7. 新增 OS04A10 sensor binning模式驱动

[Bugs]
	Fixed known bugs; 修复已知bugs

[文档]
	1. 更新 《AR93xx_Sensor调试指南》
	2. 新增 《AR93xx_demo使用说明》
	3. 新增《AR93xx_SDK大图模式使用指南》
	4. 更新 《AR93xx_SDK_API_List_Customer_Check_20220721》

#sdk-ar9301-0.12.4

[Feature]
	[Sys]
    1. kernel下dts区分了chip和board信息 （升级此版本SDK用户需修改dts文件，具体见文档《AR93xx_Kernel_DTS修改说明》）
    2. 新增clock get api和set safe api的接口 （API: ar_clk_set_rate_safe_hz, ar_clk_get_rate_hz, ar_hal_clk_set_rate_safe_hz 和 ar_hal_clk_get_rate_hz）
    3. 新增读取efuse版本号和芯片信息的接口 （API: ar_hal_sys_get_soc_revision 和 ar_hal_sys_get_soc_id）
    4. uboot efuse读取闭源
    5. 新增ADC对ddr配置的越界检测
    6. 支持通过dts配置emmc和sd相位 (emmc 节点加上 out-phase 为反相)
	7. uboot中支持SPI驱动
	8. SPI驱动支持GPIO Burst模式(CS信号持续有效)
	9. DDR4支持1200MHz速率

	[Codec]
    1. 新增频率配置检查机制
    2. 新增codec多路优化内存占用的接口 （API: AR_MPI_VENC_SetModParam(const VENC_PARAM_MOD_S *pstModParam)）及使用示例（apps/ipcam/platform/src/pf_video.c的 IPC_PF_VENC_SetVencMsgPoolSize函数）
	3. Decoder JPEG支持Scale down
	4. 修复VENC已知Bug，解决Crash问题
	5. 修复多路编码性能问题

	[NPU]
	1. 修改API及结构体以兼容V0.10.x版本（注 V0.11.x 版本的用户需要重新编译lib或者修改代码以兼容）

	[VI]
    1. 支持Camera start vin 支持不加载Vin 驱动的选项（APP可修改Vin启动选项后再加载驱动）
    2. 支持DVP接口的配置高低8位交换 （DEV_ATTR_TP9930_BASE结构体中的bDvpHighLowByteSwap参数）
    3. 支持ISP数字Gain配置
    4. 新增了用户调用用户的3A算法的API接口
    5. 去掉了AR_ISP_Aelib_xxx和AR_ISP_Awblib_xxx 接口
	6. 修复了内存泄漏问题

	[VPSS]
    1. 新增拷贝获取通道帧数据的接口 (API: AR_MPI_VPSS_GetChnFrame_WithCopy)

	[VO]
    1. 支持推送8bit灰度图到显示
    2. 支持bt656格式输出

	[Scaler]
    1. 支持等比例缩放（对齐方式可调）(API: AR_MPI_SCALER_CropResizeRatio; )
    2. 支持SemiPlanar格式缩放 （现在支持YUV444P，YUV444SP，YUV422P，YUV422SP， YUV420P， YUVI420P，YV12，YUV420SP，NV12，NV21，RGB，RGB24，BGR，BGR24，RGBD，RGBD_INTLV, GRAY格式）

[Sample]
    1. 在apps/ipcam/增加了支持buffer内容不送到overlay中叠加形式的demo
    2. 新增音频uac service
    3. 新增uvc service设置buffer size的功能，可以通过命令行参数支持4k；

[Bugs]
    Fixed known bugs; 修复已知bugs

[文档]
    1.  更新 《AR93xx_GDC_IFC_Scaler_MPI接口开发参考》
    2.  更新 《AR93xx_DSP开发指南》
    3.  更新 《AR93xx_智能开发平台SDK编译环境使用说明》
    4.  新增 《AR93xx_DVP输入开发指南》
    5.  新增 《AR93xx_ISP_3A开发使用说明》
    6.  新增 《AR93xx_Kernel_DTS修改说明》
    7.  更新 《AR93xx_Sensor调试指南》
    8.  更新 《AR93xx_SDK_API_List_Customer_Check》
    9.  更新《AR93xx_智能视频子系统API说明》
    10. 更新《AR93xx_Sensor调试指南》

#sdk-ar9301-0.12.0

[Feature]

	[Sys]
    1. kernel下dts区分了chip和board信息 （升级此版本SDK用户需修改dts文件，具体见文档《AR93xx_Kernel_DTS修改说明》）
    2. 新增clock get api和set safe api的接口 （API: ar_clk_set_rate_safe_hz, ar_clk_get_rate_hz, ar_hal_clk_set_rate_safe_hz 和 ar_hal_clk_get_rate_hz）
    3. 新增读取efuse版本号和芯片信息的接口 （API: ar_hal_sys_get_soc_revision 和 ar_hal_sys_get_soc_id）
    4. uboot efuse读取闭源
    5. 新增ADC对ddr配置的越界检测
    6. 支持通过dts配置emmc和sd相位 (emmc 节点加上 out-phase 为反相)

	[Codec]
    1. 新增频率配置检查机制
    2. 新增codec多路优化内存占用的接口 （API: AR_MPI_VENC_SetModParam(const VENC_PARAM_MOD_S *pstModParam)）及使用示例（apps/ipcam/platform/src/pf_video.c的 IPC_PF_VENC_SetVencMsgPoolSize函数）

	[VI]
    1. 支持Camera start vin 支持不加载Vin 驱动的选项（APP可修改Vin启动选项后再加载驱动）
    2. 支持DVP接口的配置高低8位交换 （DEV_ATTR_TP9930_BASE结构体中的bDvpHighLowByteSwap参数）
    3. 支持大图模式
    4. 支持ISP数字Gain配置
    5. 新增了用户调用用户的3A算法的API接口
    6. 去掉了AR_ISP_Aelib_xxx和AR_ISP_Awblib_xxx 接口

	[VPSS]
    1. 新增拷贝获取通道帧数据的接口 (API: AR_MPI_VPSS_GetChnFrame_WithCopy)

	[VO]
    1. 支持推送8bit灰度图到显示
    2. 支持bt656格式输出

	[Scaler]
    1. 支持等比例缩放（对齐方式可调）(API: AR_MPI_SCALER_CropResizeRatio; )
    2. 支持SemiPlanar格式缩放 （现在支持YUV444P，YUV444SP，YUV422P，YUV422SP， YUV420P， YUVI420P，YV12，YUV420SP，NV12，NV21，RGB，RGB24，BGR，BGR24，RGBD，RGBD_INTLV, GRAY格式）

[Sample]
    1. 在apps/ipcam/增加了支持buffer内容不送到overlay中叠加形式的demo
    2. 新增音频uac service
    3. 新增uvc service设置buffer size的功能，可以通过命令行参数支持4k；

[Bugs]
    Fixed known bugs; 修复已知bugs

[文档]
    1. 更新 《AR93xx_GDC_IFC_Scaler_MPI接口开发参考》
    2. 新增 《AR93xx_DVP输入开发指南》
    3. 新增 《AR93xx_ISP_3A开发使用说明》
    4. 新增 《AR93xx_Kernel_DTS修改说明》
    5. 更新 《AR93xx_SDK_API_List_Customer_Check》

#sdk-ar9301-0.11.1

[Feature]

	[Sys]
	1. 支持硬件AES加解密
	2. 支持双SD卡
	3. 支持Secure Boot （仅限 EMMC）
	4. 修改了双分区升级（所有分区的双分区解析皆自动完成）
	5. 支持通过/sys/kernel/debug/对应usb接口/connect_state 来获取usb device连接状态 （1=连接到Host）
	6. USB device驱动加入WCID设备支持
	7. Uboot 支持I2C驱动

	[VENC]
	1. VBR功能优化
	2. MPI头文件中添加了动态和静态参数的说明

	[VDEC]
	1. Decoder支持多channel解码
	2. 支持 JPEG 解码

	[VI]
	1. 设置用户图片，作为无视频信号时的插入图片（API: AR_MPI_VI_SetUserPic）
	2. 启用 VI PIPE 插入用户图片（API: AR_MPI_VI_EnableUserPic)
	3. 禁用 VI PIPE 插入用户图片（API: AR_MPI_VI_DisableUserPic)
	4. 设置 VI 通道裁剪功能属性（API: AR_MPI_VI_SetChnCrop）
	5. 获取 VI 通道裁剪功能属性（API: AR_MPI_VI_GetChnCrop）
	6. 修改了配合视频信号转换芯片的Sensor热插拔接口 （API：AR_MPI_ISP_StartSnsDetect；AR_MPI_ISP_StopSnsDetect； AR_MPI_ISP_GetSnsDetectInfo）

	[VPSS]
	1. 设置 VPSS 通道展宽属性（API: AR_MPI_VPSS_SetChnSpreadAttr）
	2. 获取 VPSS 通道展宽属性（API: AR_MPI_VPSS_GetChnSpreadAttr）

	[VO]
	1. Framebuffer驱动支持buffer内容不送到overlay中叠加
	2. 支持 MIPI DSI 和 DVP HDMI 的同时显示

	[GDC]
	1. 将AR_MPI_EIS_XXXX接口改为了AR_MPI_GDC_XXXX
	2. 将AR_MPI_GDC_Transform接口的参数改为结构体数组
	3. 支持Rotate 0-360度任意角度设置

[Sample]
	1. 红外sample改善了ISP流程，改进了噪声表现
	2. IPC sample中增加了移动侦测ROI的示例
	3. IPC sample中增加了VBR 相关参数的配置 （statTime、min/max I/P Qp）
	4. VIO sample 中增加了MIPI 屏幕出图
	5. VIO sample 增加了 GC2039 sensor 的HDR模式
	6. VIO sample 增加了可指定某个case出图多少帧后自动退出
	7. GDC sample 增加了用户传入变换矩阵M，生成LUT的函数及实例 （ROT90_Matrix）
	8. GDC sample 增加了旋转任意角度的实例（ROTX_Matrix）
	9. 在mpp_sample/sample_cipher 中增加了AES的示例
	10. 在 apps/utils/添加了DDR loading tool 作为压力测试时DDR读写带宽的工具

[Bug]
	修复已知bugs

[文档]
	1. 更新 《AR93xx_SDK分区升级说明》
	2. 更新 《AR93xx_GDC_IFC_Scaler_MPI接口开发参考》
	3. 更新 《AR93xx_SDK_API_List_Customer_Check》
	4. 新增 《AR93xx_SecureBoot使用说明》

#sdk-ar9301-0.10.02

[Feature]

	[Sys]
	1. 开机时间优化
	2. 优化内存使用与内存分配
	3. 精简/dev/下的设备节点
	4. SD卡支持插拔时自动挂载，自动卸载
	5. Secure Boot

	[VENC+DEC]
	1. proc节点支持删除
	2. VENC支持 MJPEG 码控接口
	3. VENC支持VBR和FixQP
	4. VDEC支持Bind

	[VI]
	1. 支持raw 图输入模式
	2. 修正了多camera的时间戳

	[VPSS]
	1. SetChnAttr支持动态修改 宽，高，mirror, flip, rotate等参数

	[Audio]
	1. 支持音频压缩G.711a, G.711u 和 AAC

[Sample]
	1. 增加神经网络 Openpose Demo
	2. IPCam增加SC530AI Sensor

[Bug]
	修复已知bugs

#sdk-ar9301-0.09.03

[Feature]

    [VO]
    1. 获取屏幕参数以实现屏幕兼容 （API：AR_MPI_VO_Dsi_Read）
    2. DVP out 支持BT1120/BT656, ARGB565相关格式

    [VI]
    1. 优化内存使用与内存分配
    2. bfm 剩余buffer 统计
    3. 精简 log 打印信息

    [VDEC]
    1. 增加VDEC内存统计信息

    [SYS]
    1. 修正各模块warning信息
    2. 支持多种升级方式挂载
    3. 支持VIN+VPSS+VENC+Regioni+VO pipeline的启停
    4. 支持多组DDR配置可选
    5. 改正视频流时间戳

    [AUDIO]
    1. 支持8k/16k采样
    2. 支持单声道

[Sample]
    1. 移动侦测独立Sample
    2. 红外Sensor 640x512 Sample
    3. 红外Sensor IPCAM Sample
    4. UVC Sample
    5. audio sample支持单声道和rate参数可配

[Bug]
    修复已知bugs

#sdk-ar9301-0.08.02

[Feature]

    [VDEC]
    1. Decoder支持多进程取流

    [VI]
    1. 获取VI物理PIPE对接前端sensor或者AD的VC号（API：AR_MPI_VI_GetPipeVCNumber）
    2. 设置VI物理PIPE对接前端sensor或者AD的VC号（API：AR_MPI_VI_SetPipeVCNumber）
    3. 多进程取VIN frames
    4. Log重点提示VB分配不够

    [VPSS]
    1. 增加统计信息

    [SYS]
    1. 支持中断次数统计
    2. proc支持设备节点删除

[Sample]
    1. IPCam Demo支持web
    2. IPCam Demo支持DVP Scaler
    3. 支持NTSC (30fps) AHD输入
    4. ADC sample
    5. SC910gs Sensor sample

[Bug]
    修复已知bugs


#sdk-ar9301-0.07.03

[Feature]

	[VEnc]
	1. 设置编码相关的模块参数（API：AR_MPI_VENC_SetModParam）
	2. 获取编码相关的模块参数（API：AR_MPI_VENC_GetModParam）
	3. 设置 H.265 协议编码通道的 Deblocking 属性（API：AR_MPI_VENC_SetH265Dblk）
	4. 获取 H.265 协议编码通道的 Deblocking 属性（API：AR_MPI_VENC_GetH265Dblk）
	5. 设置 H.264 协议编码通道的 Deblocking 类型（API：AR_MPI_VENC_SetH264Dblk）
	6. 获取 H.264 协议编码通道的 Deblocking 类型（API：AR_MPI_VENC_GetH264Dblk）
	7. 设置 H.264 /H.265 编码通道高级跳帧参考参数（API：AR_MPI_VENC_SetRefParam）
	8. 获取 H.264 /H.265 编码通道高级跳帧参考参数（API：AR_MPI_VENC_GetRefParam）
	9. 设置 H.265 协议编码通道的 PU 属（API：AR_MPI_VENC_SetH265PredUnit）
	10. 获取 H.265 协议编码通道的 PU 属（API：AR_MPI_VENC_GetH265PredUnit）
	11. 设置 H.265 协议编码通道的 Sao 属性（API：AR_MPI_VENC_SetH265Sao）
	12. 获取 H.265 协议编码通道的 Sao 属性（API：AR_MPI_VENC_GetH265Sao）
	13. 设置通道参数（API：AR_MPI_VENC_SetChnParam）
	14. 获取通道参数（API：AR_MPI_VENC_GetChnParam）

	[VDec]
	1. 将解码通道绑定到某个视频缓存 VB 池中（API：AR_MPI_VDEC_AttachVbPool）
	2. 将解码通道从某个视频缓存 VB 池中解绑定（API：AR_MPI_VDEC_DetachVbPool）
	3. 设置解码模块参数。（API：AR_MPI_VDEC_SetModParam）
	4. 获取解码模块参数。（API：AR_MPI_VDEC_GetModParam）
	5. 支持 Bind 功能

	[VGS]
	1. 启动一个 job（API：AR_MPI_VGS_BeginJob）
	2. 取消一个 job（API：AR_MPI_VGS_CancelJob）
	3. 结束一个 job（API：AR_MPI_VGS_EndJob）
	4. 往一个已经启动的 job 添加打 OSD 任务（API：AR_MPI_VGS_AddOsdTaskArray）
	5. 往一个已经启动的 job 中添加批量划线任务（API：AR_MPI_VGS_AddDrawLineTaskArray）
	6. 往一个已经启动的 job 添加缩放任务（API：AR_MPI_VGS_AddScaleTask）
	7. VGS 支持多进程使用

	[VPSS]
	获取指定图像区域的亮度总和（API：AR_MPI_VPSS_GetRegionLuma）

	[VI]
	支持获取VI状态的指令

	[ISP]
	支持检查sensor曝光参数是否超出了定义范围并报错的功能

[Sample]
	1. 添加了 IMX464 HDR 25fps 的设定
	2. 增加了红外Sensor + RGB sensor 双路出图，双光融合及伪彩色demo 的 Sample

[Bug]
	修复已知bugs


#sdk-ar9301-0.06.02

[Feature]

	[VDec]
	1. 创建视频解码通道 （API: AR_MPI_VDEC_CreateChn）
	2. 销毁视频解码通道 （API: AR_MPI_VDEC_DestroyChn)
	3. 获取视频解码通道属性 （API: AR_MPI_VDEC_GetChnAttr)
	4. 设置视频解码通道属性 （API: AR_MPI_VDEC_SetChnAttr)
	5. 解码器开始接收用户发送的码流 （API: AR_MPI_VDEC_StartRecvStream)
	6. 解码器停止接收用户发送的码流 （API: AR_MPI_VDEC_StopRecvStream)
	7. 查询解码通道状态 （API: AR_MPI_VDEC_QueryStatus)
	8. 获取视频解码通道的设备文件句柄 （API: AR_MPI_VDEC_GetFd)
	9. 关闭视频解码通道的设备文件句柄 （API: AR_MPI_VDEC_CloseFd)
	10. 复位视频解码通道 （API: AR_MPI_VDEC_ResetChn)
	11. 向视频解码通道发送码流数据 （API: AR_MPI_VDEC_SendStream)
	12. 获取视频解码通道的解码图像 （API: AR_MPI_VDEC_GetFrame)
	13. 释放视频解码通道的图像 （API: AR_MPI_VDEC_ReleaseFrame)

	[VEnc]
	1. 设置H.264 /H.265 编码通道高级跳帧参考参数 （API: AR_MPI_VENC_SetRefParam)
	2. 获取H.264 /H.265 编码通道高级跳帧参考参数 （API: AR_MPI_VENC_GetRefParam)

	[VPSS]
	1. 设置VPSS扩展通道的鱼眼属性 （API: AR_MPI_VPSS_SetExtChnFisheye）
	2. 获取VPSS扩展通道的鱼眼属性 （API: AR_MPI_VPSS_GetExtChnFisheye）

	[ISP]
	1. 获取AF 内部状态信息 （API: AR_MPI_ISP_QueryAfInfo)
	2. 获取AF属性 （API: AR_MPI_ISP_GetAFAttr)
	3. 设置AF属性 （API: AR_MPI_ISP_SetAFAttr)
	4. 获取白平衡内部状态信息，比如AWB增益系数，检测色温，饱和度值，CCM系数等 （API: AR_MPI_ISP_QueryWBInfo)

	[ADC] (Hal层接口)
	1. 使能ADC (ar_hal_adc_enable)
	2. 关闭ADC (API: ar_hal_adc_disable)
	3. 获取ADC原始值 (API: ar_hal_adc_get_raw)
	4. 获取ADC 电压值 (API: ar_hal_adc_get_millivolt)
	5. 获取ADC 调整后的电压值 (API: ar_hal_adc_get_millivolt_by_adjust)

[Sample]
	Sample_VPSS : 实现功能有 scale、crop、mirror、flip、rotate、fisheye
	1. 读取本地yuv文件送入vpss进行scale、crop等操作
	2. 从vpss取出yuv frame并写入文件
	3. 送yuv数据到不同的group（多线程取流）
	4. 从不同group的channel中取数据并写入文件（多线程取流）

	Sample IR sensor：红外sensor用例及红外模组的驱动

	Sample Audio：
	1. 双声道，16bit，48K下，ai的pcm读取保存和ao输出功能（数据文件形式保存）
	2. ai绑定ao
	3. ai数据获取，直接送给ao输出

	Sample IPCam：
	1. 新增支持 8x1080P@25fps AHD sensor；
	2. 新增支持 2x1080P@25fps IMX307；
	3. 支持sensor mipi id/gpio app可配；

[Plat]
	1. 支持QSPI NAND
	2. 支持Uboot开机Logo配置
	3. 支持log 直接 print 到 console

[Tool]
	1. IQ tool 抓取带3a头信息的raw 并实现实时仿真
	2. Gui tool 支持IO电源域设置

[Bug]
	1. Fixed known bugs; 修复已知bugs
	2. 增加了检查用户配置帧率与Sensor的配置帧率之间的大小关系


#sdk-ar9301-0.05.00

[Feature]

	[Vin]
	1. Sets the Raw noise reduction attributes of a VI device; 设置VI设备的Raw图降噪的参数 (API: AR_MPI_ISP_SetRnrStrengthTidyAttr)
	2. Obtains the Raw noise reduction attributes of a VI device；获取VI设备的Raw图降噪的参数 (API: AR_MPI_ISP_GetRnrStrengthTidyAttr)
	3. Checks the status of a VI pipe; 查询 VI pipe 状态 (API: AR_MPI_VI_QueryPipeStatus）
	4. Query the state of a VI channel; 查询 VI 通道状态 （API: AR_MPI_VI_QueryChnStatus）

	[ISP]
	1. Obtains the statistics of auto exposure; 获取 AE 统计信息 （API: AR_MPI_ISP_GetAEStatistics）
	2. Obtains the statistics of white balance; 获取 WB 统计信息 （API: AR_MPI_ISP_GetWBStatistics)
	3. Obtains the statistics of auto exposure; 获取AF 统计信息 （API: AR_MPI_ISP_GetFocusStatistics）
	4. Sets the attributes of auto exposure; 设置AE 曝光属性（API: AR_MPI_ISP_SetExposureAttr)
	5. Obtains the attributes of auto exposure; 获取AE 曝光属性（API: AR_MPI_ISP_GetExposureAttr)
	6. Sets the attributes of white balance; 设置白平衡属性（API: AR_MPI_ISP_SetWBAttr)
	7. Obtains the attributes of white balance; 获取白平衡属性（API: AR_MPI_ISP_GetWBAttr)
	8. Obtains the configuration of 3A statistics; 获取 ISP 3A 统计信息配置 (API: AR_MPI_ISP_GetStatisticsConfig)
	9. Sets the configuration of 3A statistics; 设置 ISP 3A 统计信息配置 （API: AR_MPI_ISP_SetStatisticsConfig)
	10. Obtains the time out info of ISP; 获取ISP 中断信息 （API: AR_MPI_ISP_GetVDTimeOut)
	11. Sets the firmware status of ISP; 设置ISP Firmware 状态 （API: AR_MPI_ISP_SetFMWState）
	12. Obtains the internal status info of auto exposure; 获取AE 内部状态信息（API: AR_MPI_ISP_QueryExposureInfo)
	13. Calculates the white balance gain under certain color temperature; 计算特定色温下的白平衡增益系数 （API: AR_MPI_ISP_CalGainByTemp)

	[VPSS]
	VPSS supports multi-process getting frames operation; VPSS支持多进程取帧

	[SYS]
	Log printing is unified by core service; 统一由 core service 进程进行log 打印

	[Plat]
	Supports pin bank voltage dynamic configuration via DTS; 支持 pin bank电压在DTS中动态配置

[Sample]
	1. Sample_vio adds IMX307 and IMX290 sensor HDR mode; 添加了IMX307和IMX290 HDR 模式的设定
	2. Sample_vio support DVP 8-ch VIF output with multi resolutions (Note: 720p only support 2560*720)； 支持了DVP 8路 VIF出图的分辨率混插（注意: 720p 只支持 2560*720）

[Bug]
	Fixed known bugs; 修复已知bugs


#sdk-ar9301-0.04.00

[Feature]

	[Vin]
	1.	Transmits RAW data through a VI pipe; 支持通过VI PIPE 发送RAW数据 (API: AR_MPI_VI_SendPipeRaw)
	2.	Improves sensor driver frame to support IMX415 and 8-ch DVP input; 改进了sensor驱动架构以支持IMX415和8路 DVP 输入

	[VPSS]
	1.	Sets the LMF parameters of the fisheye lens corresponding to a VPSS group; 设置VPSS GROUP对应的鱼眼镜头LMF参数配置 (API: AR_MPI_VPSS_SetGrpFisheyeConfig)
	2.	Obtains the LMF parameters of the fisheye lens corresponding to a VPSS group; 获取VPSS GROUP对应的鱼眼镜头LMF参数配置 (API: AR_MPI_VPSS_GetGrpFisheyeConfig)

[Sample]
	1.	Sample_vio adds IMX415 linear mode 30fps & 60fps; 添加了IMX415线性模式30fps&60fps设定
	2.	Sample_vio adds DVP sensor input; 添加了 DVP sensor输入

[Tool]
	Ar_iqtool_service improves sensor simulation which can support RAW image simulation; Ar_iqtool_service 改进了sensor仿真功能，支持Raw图的仿真

[Bug]
	Fixed known bugs; 修复已知bugs


#sdk-ar9301-0.03.00

[Feature]

    [Vin]
    1.  Sets the source of VI pipe data (API: AR_MPI_VI_SetPipeFrameSource)
    2.  Obtains the source of VI pipe data (API: AR_MPI_VI_GetPipeFrameSource)
    3.  Obtains the data of a physical VI pipe (API: AR_MPI_VI_GetPipeFrame)
    4.  Releases the VI pipe data (API: AR_MPI_VI_ReleasePipeFrame)

    [Venc]
    1.  Sets the VUI parameters of an H.265 VENC channel (API: AR_MPI_VENC_SetH265Vui)
    2.  Obtains the VUI parameter settings of an H.265 channel (API: AR_MPI_VENC_GetH265Vui)
    3.  Supports set the video usability information (VUI) parameters of an H.264 channel (API: AR_MPI_VENC_SetH264Vui)
    4.  Obtains the VUI parameter settings of an H.264 channel (API: AR_MPI_VENC_GetH264Vui)
    5.  Supports MJPEG, JPEG format
    6.  Supports performance statistical proc info

    [Region]
    1.  Supports ARGB8888，ARGB4444，ARGB1555 format (API: AR_MPI_RGN_Create)

    [NPU]
    1.  Supports usage statistics (API: AR_MPI_NPU_GetNpuUsage)


[Sample]

    1.  Sample vio - Two sensor input
    2.  Sample vio - Virtual channel mode 8 YUV format DVP output
