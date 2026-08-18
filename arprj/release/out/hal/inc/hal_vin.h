#ifndef __HAL_VIN_H__
#define __HAL_VIN_H__

/**
 * @file hal_vin.h
 * @brief 定义vin的api
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/05/10
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#include "hal_vin_type_def.h"
#include "hal_vin_sensor_lib.h"
#include "hal_vin_aec_algo_lib.h"
#include "hal_vin_awb_algo_lib.h"
#include "hal_vin_af_algo_lib.h"


/*
*@brief:
概述:
Vin：
    视频输入，指的的是9301的视频输入接口及与其对应的软件系统。在本手册中一般意义上是指软件系统。
    在9301的hw 系统中视频的数据接口主要有mipi接口和dvp 接口。一般意义上这两个接口接入的视频是camera的视频。
    所以在本使用手册中，如果提到camera 系统，或camera 指的就是vin 系统。
    Vin 即指的是hw 视频的输入，相对于其他模块，比如encoder ，那么vin 实际上是一种输出。
    在整个多媒体系统中，vin是视频的来源，所以9301的多媒体系统架构下，vin 使用的是src 的概念，比如cam_src.
    Vin 在某些场合，从软件的角度来讲，表示的是视频的输入子系统，用驱动程序来表示，在9301的sdk 中，
    系统的启动过程中，会生成一个vin的设备节点，通过这个节点来访问vin的驱动程序，完成对vin的控制。
Camera：
    如在vin的介绍中，视频的来源有mipi接口，和dvp 接口，一般意义上来讲，这两个接口通常接的是camera（摄像模组），
    而camera 的组成的核心元件是sensor（一般是指cmos 图像传感器）。Sensor 输出的接口一般有dvp的和mipi的接口。
    也就是说camera 通过mipi dvp接口连接到9301的soc 上 ，那么在vin系统下，这个camera 或者sensor，我们用dev来表示。
    所以在本手册的api 中，devid 指的就是camera 本身，或sensor 本身对应的号。
    这个号在hw设计或软件启动那个过程中被确定下来。
    正如camera 是vin的主要输入源，所以在本手册中，在某些语境下，提到camera 可以认为是vin的同意反复，
    在阅读手册或sdk的代码中，注意区分camera指的是vin 还是只是指设备。比如camera系统，指的是vin， 然后如果单独的说camea，指的是设备。
    在本手册中，或代码中，由于历史的原因，vin 和camera 被不停的交叉引用。hal的api 中，
    使用vin 的概念比较多，然后在具体的系统的编码中，camera的概念使用的比较多。
Isp
    图像信号处理器，关于isp更详细细的说明请参考hw 手册。
    Isp 接受mipi 传输来的sensor raw 数据进行处理，然后输出到内存，完成图像处理的任务。
    9301 只有一个isp hw， 软件对其进行了虚拟化，我们可以通过isp vin 设备来无差别的访问任何一个虚拟的isp 设备。所以我们的sdk 容许有多个camea 接入，然后进isp 处理。
    正如camera 提供mipi接入数据后，mipi 可以把数据直接送给isp ，也可以直接写到内存，然后isp 主动的读取。前一种模式叫行存模式，或叫直连模式，或叫在线模式。而后者叫ddr模式，离线模式。在本api手册中，这些模式概念会交替引用。
Pipe
    管道，本概念指的是一系列的连接的总和。比如输数据从sensor 输出，然后经由mipi 送给isp 然后输出到内存，然后送给用户。
    Pipe 的连接关系不是总是固定的。在不同的情况下，可能不同，比如在某些情况下，isp出来的数据可能需要经过eis 处理后然后写入内存送给用户，
    那么这个pipe 和前面的pipe 就有所不同。在本系统中，我们可以创建很多的pipe 来完成不同的功能。在本api 中，使用pipe id 来表示不同的pipe。
    每一个pipe 有一个对应的设备。通过设备可以操作pipe，比如open，close等。
    正如上节说描述，pipe是连接的总和，连接的源头就是camera。所以camera id 和 pipe id
    需要对应，本api 提供bind的接口，把dev id 和 pipe id进行bind。需要注意的是，camera 一般对应一个具体的硬件，
    但是如果pipe的源不是具体的硬件，而是一块内存数据呢，这个时候pipe 仍然是可以成立的，对应的dev 类型，本sdk 叫做isp in dev。用来构建虚拟isp。
    如果pipe的源不是内存本身，只是hw的抽象表示，而不是具体的camea的时候，那么这个输入设备就是通用输入设备。在本sdk中，
    通用输入设备用来提供一种机制让用户自己bind具体的sensor。这是通过sensor callback 机制实现的。也就是用户open 一个通用设备的pipe，
    然后调用注册sensor calllbak事件，响应具体的sensor 回调。
Stream
    在pipe的输出端，一定有buffer 被写入内存然后提交给用户。Pipe的输出末端，可以有一个或多个输出到内存的端口。
    我们把数据从camera 输出流经pipe 中间的各个模块最后到末端的过程叫做stream。也就是说，stream 的尾端一定有数据出来。
    用户可以获取这个数据，只要指定dev id， pipe id ，stream id。
    Dev id： 数据的源头
    Pipe id： 数据流动的连接关系。
    Stream id：数据流动的过程，一般也指pipe的尾端的数据输出。在使用过程中，我们不对其进行严格的区分。
    在本sdk中，pipe 是载体，通过stream 来实现自身，也就是说打开的pipe 只是一个连接的抽象，需要在pipe上create stream 来具体化连接关系。
    在打开pipe 前，必须把pipe id 和 dev id 进行bind。因为pipe 在create stream 实现之前只是一个抽象的概念，
    打开的时候只有和dev id bind 后才指定了数据的源头。也就是说打开的时候必须指定数据源头。
Stream buffer
    在pipe 上创建好的stream，不仅实现了自身，也实现了pipe，这时的pipe 不再那么抽象，变的具体起来，是实在的连接关系。
    但是stream 还没有完全完成自身，stream是数据的传递过程还没有开始。数据的载体在本sdk中表现为buffer。Stream的尾端数据要写入buffer，
    我们把这块buffer 叫做stream buffer。记录了stream的数据的各种属性及数据本身在mem中的位置。对于用户而言，在bind 了dev和pipe，
    创建好了stream，就需要准备stream的buffer 了。然后把stream buffer 交给stream。启动stream 后，让stream 流动起来。
    这是stream 就完全实现了。用户可以从stream 中获取stream buffer ，然后对buffer 进行处理。
    处理完成后要归还给stream，以达到stream 继续使用stream buffer的目的。
    有了stream buffer 后，用户可以获取stream buffer，本sdk 提供了若干用户，这个限制是由vin系统的启动选项指定的。
    用户想要获取stream buffer 必须先申请一个client id，然后在这个client id 下才能获取stream buffer。
    当然很多用户可以使用一个client id，那么谁能获取到buffer ，就看系统的调度了，属于随机范畴了。
Event
    在vin 或camera 系统的运行过程中，系统的会产生很多的事件，这些事件会通过事件机制上报到用户，
    本sdk 采用了同步事件的机制，所谓同步事件是指，上报事件的任务必须等待事件处理完成才能继续运行。
    在本api中，用户使用事件的方式是先注册事件，表示需要对这类事件处理，然后启动一个任务取get event，get 到事件后需要快速处理，
    然后relese 事件。 注意，一旦注册了事件，必须调用get release 函数对事件进行获取和释放。不然系统可能会block 在某些事件上不能继续响应，
    比如用户注册了sensor init 的事件，但是不响应，那么stream 的启动过程就会block 在等待用户响应init 事件上
    由于要求对事件的响应尽可能的快，所以，建议用户get event 后把这个事件提交到线程池中处理，从而不会block 其他事件的处理。
    考察sdk 的vin 部分的事件类型，很大一部分是回调事件，比如sensor的回调事件 ，aec af awb 的回调事件等，
    这些回调事件给了用户一个机制参与vin 系统运行过程的调整。甚至用户可以在启动选项上disable 原来的处理，而在cb 事件中完成所有的功能。
    比如在创建pipe的时候不bind 具体的hw 设备，而是bind 一个抽象的hw 设备，然后注册sensor的回调事件，
    这时候在sensor的回调事件中完成一切sensor的配置。曝光等。相同的方法可以利用3a的回调事件实现自己的3a 算法。
Pipe stream的基本流程
    Pipe line 的连接关系或者stream的数据传递过程在9301 中是如下的过程
    1 sensor 通过mipi 把数据发到mipi 线上
    2 soc 的mipi 控制器把数据从mipi phy 上采集数据然后进行协议解码
    3 解码后的数据送到vif的输入端口上
    4 vif 根据pipe的属性配置把数据送到isp 口的linebuffer上，或者ddr 内存中
    5 然后isp 从linebuffer或ddr 中获取数据进行处理
    6 isp 在处理的过程中会产生统计数据，这些统计数据通过统计端口写到ddr， 并产生中断
    7 vin 系统处理统计中断，并把结果反馈给isp 本身或者sensor 本身
    8 isp 处理完成数据后输出结果到ddr，并产生中断
    9 vin 系统把结果数据送给用户。
    10 用户使用完成后返回给vin stream，从而形成循环过程
    这里面多了一个vif这样的hw，主要用来协调mipi和isp之间的数据格式等。
    在isp的运行过程中，有一个及其重要的中断，叫做vysnc 中断，vin 系统或者camera系统
    会把这个vsyn 中断的事件发送到所有的模块，所有的模块按照需求响应这个事件。
    比如isp 本身会在vsync事件更新改变的效果参数到寄存器中 ，sensor 模块会写曝光等
抽象设备与回调事件
    在vin的系统中，系统启动后会有若干并非实际的senor的设备，主要表现为通用sensor 设备，也就是抽象设备，isp in设备两个设备。
    不论那个设备，本身设备不提供具体的运行信息，需要用户在api 上对其设置属性，在本sdk中叫做sensor 可变参数。
    当用户bind 抽象设备到pipe上后，需要设置sensor 可变参数对抽象设备进行实例化。同时用户需要注册所有的sensor dev cb 函数，
    用来响应vin启动过程中的对sensor 操作的事件。
    一般情况下，不建议用户自己使用sensor 抽象设备在app中写驱动，而是在core 进程中按照驱动模板写一个实际存在的驱动。
3a 算法与事件回调
    在9301的设计中，我们将提供机会自己可以用callback 事件机制，完成实际的3a 算法的功能，配合3a update的api。
    实现app 层实现3a 算法的功能。在启动选项中，有标志决定着，是否绕过算法库本身 ，而只有callback 事件。
    尽管如此，可以用cb event的方法，让用户调用自己的算法库，但是不建议用户这么做，如果有自己的算法库，
    那么希望用户直接按照sdk的方法添加自己的算法库到系统中。
    sdk 还提供另一种使用自己算法库的方法，那就是我们提供stats stream ，
    可以直接拿到stats 数据，用户可以直接处理最原始的stats 数据来做3a 算法。
    所以用户在自己的3a 算法的加持下，sdk 的api 容许用户自己使用效果api 随便的安排当前使用的tuning 参数，实现对isp 效果系统的深度定制
    Meta 数据与事件回调
    所谓meta 数据指的是在isp 运行过程中产生的数据，主要包括各类统计数据，3a 算法的中间数据，中断统计，motion 检测等数据，
    在本sdk中，这些数据也通过事件机制回调回去。用户可以取到这些数据做进一步的决策
效果系统工作原理
    对于isp，效果系统起着很关键的作用，完成tuing 参数的环境自适应。在sdk 中，环境的自适应是通过aec 给出的lux index，或者 gain 来触发的，
    同时一些和色彩相关的tuning 还会通过awb 给出的连续色温决策。在tuning 标定阶段，tuing 工程师会把各种亮度，
    色温下的参数标定到tuning 的一个bin文件中，系统通过aec awb的输出来决定使用那一组或某两组的插值。
    同时9301 sdk 提供屏蔽掉自动决策的过程，从而使用用户自己可以决定使用那组tuning 好的参数。用户可以根据3a算法的结果直接决策，
    也可以通过其他手段决策，比如通过ai来决策。用户可以用标定好的参数进行决策，
    也可以不是用标定好的，用户可以直接发送自己定义好的tuing 参数。
    Tuning 参数的值，基本和isp 的效果寄存器是一一对应的，具体的含义，参数范围请参考tuning 手册，本手册不提供效果参数的使用说明
属性解释
    在本系统中，出现如下几种属性
    1 启动选项
    STRU_VIN_DRIVER_START_OPT_T。 这选项是camera 服务启动前设置的，
    主要配置一些整个系统的参数，比如buffer 数量，最大sensor 数量，3a 库的加载情况，以及支持的最大的sensor的个数等
    2 publilc 属性
    这些属性是全局的，是camerra 服务启动后，第一个camera 启动前的配置。对所有的camera 生效。典型的是配置是否是多camera 模式。
    3 pipe 属性
    这个属性是单个camera的属性，对所有的stream 是一样的。典型的如是否启动hdr 模式等。
    4 stream 属性
    这个属性针对单个的stream，典型如配置stream的w h等
*/


/*
    @brief: g_use_rpc_call, 默认使用rpc call的形式，即vin驱动运行在和用户app 不同的进程中，如果设置0，将会运行在用户进程中
*/
int ar_hal_vin_set_rpc_call(int rpc_call);



/*
*@brief: 映射一个物理地址到虚拟地址
*@param:fd_mm 通过 dev/mem 打开的fd
        p_phy_addr： 需要映射的物理地址
        len: 映射的长度
        offset_ret: 返回页面对其偏移
*@code
*@endcode
*@return: 返回虚拟地址
*@note：实际的虚拟地址是返回虚拟地址+offset_ret
*/

AR_VOID* ar_hal_vin_map_phy_addr(AR_S32 fd_mm,AR_VOID *p_phy_addr,AR_S32 len,AR_S32 *offset_ret);

/*
*@brief: 反映射一个虚拟地址
*@param:p_addr_v ：ar_hal_vin_map_phy_addr 返回的虚拟地址
              len: ar_hal_vin_map_phy_addr 映射时的长度
*@code
*@endcode
*@return:
*/

AR_S32 ar_hal_vin_ummap_vaddr(AR_VOID *p_addr_v,AR_S32 len);

/*
*@brief:打开vin设备，在9301的sdk 中，系统的启动过程中，会生成一个vin的设备节点，通过这个节点来访问vin的驱动程序，完成对vin的控制。
*@param:no
*@code
*@endcode
*@return: >=0	成功,打开的文件描述fd
          <0	失败，
*@note：后续的api 只有部分传递的是vin的dev fd，使用时注意
*/
AR_S32 ar_hal_vin_open_vin_dev();
/*
*@brief: 关闭打开的vin dev，一般用在app 退出的时候。
*@param: fd	打开的vin设备fd， 输入
*@code
*@endcode
*@return:无
*@note:
*/
AR_VOID ar_hal_vin_close_vin_dev(AR_S32 fd);

AR_S32 ar_hal_vin_read_reg(AR_S32 fd_vin_dev,AR_U64 phy_addr,AR_U32 *val,AR_S32 start,AR_S32 end);
AR_S32 ar_hal_vin_write_reg(AR_S32 fd_vin_dev,AR_U64 phy_addr,AR_U32 val,AR_S32 start,AR_S32 end);



/*
*@brief: 本api完成获取当前设备或系统在开机的时候探测到的设备的数量，
需要注意的是，获取的设备是所有的设备。有的是实际上能对应到具体的一个真正的设备硬件的，
但有的知识虚拟的。可以通过后续的ar_hal_vin_get_dev_infor 来进行区分。
实际的dev就是有真正硬件的dev比如一个实在的imx307等，虚拟的dev不具备真正的硬件，
但是这个dev有hw的行为，比如sensor_sim, 我们可以通过sensor sim 来发送一张raw 图，
相当于一个dev。 同时虚拟的dev也可以在app 中通过回调函数和sensor 可变参数绑定一个具体的dev来执行
*@param: fd_vin_dev	打开的vin 设备fd	            输入
         p_dev_num	指向获取到的设备的数量的指针	输出

*@code
*@endcode
*@return: 0	    成功
          非0	失败

*@note:
*/
AR_S32 ar_hal_vin_get_dev_num(AR_S32 fd_vin_dev,AR_S32 *p_dev_num);
/*
*@brief: 本api 用来获取系统的dev的信息，主要是安装方向，角度，是否支持对焦，以及名字。
         这个sensor 的设备类型，比如是否是core 设备，如果是core  设备，那么其下面有几个sub dev。
         一般来讲，一旦一个设备有sub dev，那么在hw 上一定是连在soc 的mipi 上的设备不是实际的dev，
         而是一个多合一的mipi 桥接设备。 也及是说多个mipi dev 通过桥接dev 链接到soc 的mipi 上。
         soc 通过vc 来区分出来。 同理 dvp 设备也是如此
*@param:  fd	                打开的vin 设备fd	            输入
          p_sensor_dev_infor	指向获取到的设备的info的指针	输出

*@return:0	成功
         非0	失败

*@note:
*/
AR_S32 ar_hal_vin_get_dev_infor(AR_S32 fd_vin_dev,AR_S32 dev_id,STRU_SENSOR_DEV_INFO_T *p_sensor_dev_infor);
/*
*@brief:获取vin start 选项，和set 对应。
*@param:fd_vin_dev	打开的vin 设备fd	        输入
        p_opt	    指向获取到的启动选项的指针	输出
*@code
*@endcode
*@return: 0	成功
          非0	失败
*@note:
*/
AR_S32 ar_hal_vin_driver_get_start_opt(AR_S32 fd_vin_dev,STRU_VIN_DRIVER_START_OPT_T *p_opt);
/*
*@brief:设置camera 服务，也就是vin 子系统驱动的启动选项。这些选项不可在驱动启动后修改，只可以在驱动启动前进行修改。
        典型的如系统支持的dev的数量，某些中间过程buffer的数量等
*@param:fd_vin_dev	打开的vin 设备fd	        输入
        p_opt	    指向获取到的启动选项的指针	输入
*@code
*@endcode
*@return: 0	成功
          非0	失败
*@note:调用本api 必须先调用ar_hal_vin_driver_get_start_opt 获取当前的启动参数。
       本api 有且只能在vin drviver 加载的前调用，加载后不可调用。一般来讲，用户可以不考虑使用本api，
       核心进程会配置好。但是用户如果关闭了core 进程的启动选项，
       那么用户必须自己调用本api 设置启动选项，并主动调用ar_lancher 加载vin 驱动
*/
AR_S32 ar_hal_vin_driver_set_start_opt(AR_S32 fd_vin_dev,STRU_VIN_DRIVER_START_OPT_T *p_opt);
/*
*@brief: 向系统中注册sensor 设备。注册的设备在系统启动的过程中，
         会被probe。Probe 成功的设备可以通过ar_hal_vin_get_dev_num 及 ar_hal_vin_get_dev_infor 获取到具体的信息。
         成功的dev 会有一个单独的设备号。使用这个设备号就可以访问设备。
*@param: fd_vin_dev	    打开的vin 设备fd	                输入
         p_sensor_dev	指向需要注册的sensor的信息的指针	输入

*@code
*@endcode
*@return: 0	成功
          非0	失败
*@note：本api 属于vin 系统启动前的api。在vin 系统启动后不可调用
*/
AR_S32 ar_hal_vin_register_sensor_dev(AR_S32 fd_vin_dev,STRU_SENSOR_DEV_T *p_sensor_dev);
AR_S32 ar_hal_vin_register_actuator_dev(AR_S32 fd_vin_dev,STRU_ACTUATOR_DEV_T *p_actuator_dev);

/*
*@brief: 本api 加载vin 驱动，在准备好start 选项后，并注册完成sensor 设备后。
         用户可以调用本api 完成vin驱动的加载。
*@param:fd_vin_dev	打开的vin 设备fd	输入
*@code
*@endcode
*@return:0	    成功
         非0	失败
*@note:需要解释一下，加载驱动的时候竟然需要传入一个vin设备的fd，
       一般意义，只有驱动加载后才能有dev的节点，这里是这样的，在系统的后，
	   vin的驱动已经加载了，并创建了dev 节点。这里只是把驱动激活，
	   正如api 本身名字表现出来的，是启动驱动。为了方便，直接描述为加载驱动。前面的比启动选项，
	   注册sensor 设备等，都是相同的理解，都需要vin dev fd
*/
AR_S32 ar_hal_vin_start_vin_driver(AR_S32 fd_vin_dev);
/*
*@brief: 设置vin的公共属性，即所有pipe 共享的属性。典型的如isp的工作模式等
*@param: fd_vin_dev	打开的vin 设备fd	输入
         p_pub_prop	指向public 属性的指针	输入

*@code
*@endcode
*@return:0	成功
        非0	失败
*@note: 本api 必须在确保所有的pipe 都有相同的属性。必须在所有pipe关闭后设置
*/
AR_S32 ar_hal_vin_set_public_property(AR_S32 fd_vin_dev,STRU_VIN_DRIVER_PUBLIC_PROPERTY_T *p_pub_prop);
/*
*@brief: 获取vin的public 属性。
*@param: fd_vin_dev	打开的vin 设备fd	    输入
         p_pub_prop	指向public 属性的指针	输出
*@code
*@endcode
*@return: 0	成功
          非0	失败

*@note:
*/
AR_S32 ar_hal_vin_get_public_property(AR_S32 fd_vin_dev,STRU_VIN_DRIVER_PUBLIC_PROPERTY_T *p_pub_prop);
/*
*@brief: 本api 获取系统注册的一个通用sensor 设备。正如概述中或者vin 启动选项的描述中所提到的，
vin系统的设备有三类，第一类是真正的sensor dev。他们和具体的sensor 一一对应。
第二类是抽象的sensor dev， 第三类是isp in dev。
第一类用于一般需求，驱动程序写在core 进程
第二类设备用于特殊需求，驱动程序写在用户app
第三类设备用于直接发送raw 给isp 进行处理的case， 一般用于vc 通道下的多dev 复合情况。
也用在构建sensor 模拟器。或需要用户对raw 图像进行处理的情况
*@param:fd_vin_dev	打开的vin 设备fd	输入
        p_dev_id	指向dev id 的指针	输出
*@code
*@endcode
*@return:0	    成功
         非0	失败
*@note: 1 本api 获取的设备一定是还没有open 的设置
        2 一旦get 到后，请马上对该设备和pipe bind， 并对其打开
        3 如果有多个用户调用该设备，但是不打开，那么，这些用户可能获取到同样的id， 这样会导致使用上的冲突，产生不可预期的问题。
*/
AR_S32 ar_hal_vin_get_common_sensor_dev_id(AR_S32 fd_vin_dev,AR_S32 *p_dev_id);
/*
*@brief:本api 获取系统注册的一个isp in 设备。正如概述中或者vin 启动选项的描述中所提到的，vin系统的设备有三类，第一类是真正的sensor dev。他们和具体的sensor 一一对应。
第二类是抽象的sensor dev， 第三类是isp in dev。
第一类用于一般需求，驱动程序写在core 进程
第二类设备用于特殊需求，驱动程序写在用户app
第三类设备用于直接发送raw 给isp 进行处理的case， 一般用于vc 通道下的多dev 复合情况。也用在构建sensor 模拟器。或需要用户对raw 图像进行处理的情
*@param:fd_vin_dev	打开的vin 设备fd	输入
        p_isp_in_id	指向dev id 的指针	输出
*@code
*@endcode
*@return:0	成功
         非0	失败
*@note:1 本api 获取的设备一定是还没有open 的设置
       2 一旦get 到后，请马上对该设备和pipe bind， 并对其打开
       3 如果有多个用户调用该设备，但是不打开，那么，这些用户可能获取到同样的id， 这样会导致使用上的冲突，产生不可预期的问题。
*/
AR_S32 ar_hal_vin_get_common_sensor_isp_in_dev_id(AR_S32 fd_vin_dev,AR_S32  *p_isp_in_id);
/*
*@brief: 本api 获取已经打开的pipe，pipe的概念请参考本手册概述部分。
*@param:fd_vin_dev	打开的vin 设备fd	输入
        p_pipe_id	指向dev id 的数组	输出
        p_opend_pipe_num	打开的pipe的个数	输出
*@code
*@endcode
*@return: 0	    成功
          非0	失败
*@note:   为了避免数组的越界，请分配p_pipe_id 比较大一些（比如128）。 或者直接先获取到dev的数量，然后按照设备的数量分配这块buffer
*/
AR_S32 ar_hal_vin_get_opened_pipe(AR_S32 fd_vin_dev,AR_S32 *p_pipe_id,AR_S32 *p_opend_pipe_num);
/*
*@brief:请参看ar_hal_vin_get_opened_pipe的说明，设备和pipe的概念区别请参考概述
*@param:
*@code
*@endcode
*@return:
*@note:
*/
AR_S32 ar_hal_vin_get_opened_dev(AR_S32 fd_vin_dev,AR_S32 *p_dev_id,AR_S32 *p_opend_dev_num);
/*
*@brief: 用指定的属性打开vin 的某一个pipe。正如概述描述，pipe 只是一个连接的抽象，没有实体，
所以id 是可以任意指定的，但是打开的时候必须先bind 到dev上，比如用id为0的pipe bind 设备为6的设备。
然后打开。Bind 并打开的pipe，已经开始具体化，但是仍然没有完成，后续需要在pipe 上creat 后才能完成pipe的具体化。
另外需要注意的是，没有打开设备的api，对设备操作的api 也很少，设备的操作都是通过pipe进行的，
也就是说通过设备实例化后的pipe 代理了设备的操作。

*@param:fd_vin_dev	打开的vin 设备fd	输入
        pipe_id	需要打开的pipe的id。本id 最大16. 	输入
        p_pipe_property	指定打开属性的指针	输入
*@code
*@endcode
*@return: 0	    成功
         非0	失败

*@note:特别注意的，在open pipe前，pipe id 和 dev id 必须绑定。后续对所有dev id 的操作大部分都是对pipe id 的操作，这些事通过api的命名来区分的。
*/
AR_S32 ar_hal_vin_open_pipe(AR_S32 fd_vin_dev,AR_S32 pipe_id,STRU_VIN_PIPE_PROPERTY_T *p_pipe_property);
/*
*@brief:判断给定的pipe id 是否已经打开了
*@param:fd_vin_dev	打开的vin 设备fd	输入
        pipe_id	指定的pipe的id。本id 最大16. 	输入
        is_opened	是否打开的指针	输出
*@code
*@endcode
*@return: 0	成功
         非0	失败
*@note: 此api 判断的是pipe 是否打开，并非dev 是否被打开。
*/
AR_S32 ar_hal_vin_pipe_is_opend(AR_S32 fd_vin_dev,AR_S32 pipe_id,AR_S32 *is_opened);
/*
*@brief: 请参考ar_hal_vin_pipe_is_opend，
由于dev 和 pipe 很早就bind了，所以用户可以通过pipe的状态，直接推导出dev的状态，
也可以使用本api 直接获取。
*@param:
*@code
*@endcode
*@return:
*@note:
*/
AR_S32 ar_hal_vin_dev_is_opend(AR_S32 fd_vin_dev,AR_S32 dev_id,AR_S32 *is_opened);
/*
*@brief:获取pipe的属性
*@param:fd_vin_dev	打开的vin 设备fd	输入
        pipe_id	指定的pipe的id。本id 最大16. 	输入
        p_pipe_property	Pipe 属性的指针	输出

*@code
*@endcode
*@return:0	成功
        非0	失败
*@note:
*/
AR_S32 ar_hal_vin_get_pipe_prop(AR_S32 fd_vin_dev,AR_S32 pipe_id,STRU_VIN_PIPE_PROPERTY_T *p_pipe_property);
/*
*@brief: 设置pipe的属性
*@param:fd_vin_dev	打开的vin 设备fd	输入
        pipe_id	指定的pipe的id。本id 最大16. 	输入
        p_pipe_property	Pipe 属性的指针	输入
*@code
*@endcode
*@return:0	    成功
         非0	失败
*@note: 本api 可以在creat stream 前，任意调用，但creat stream 后不能再调用。
只要有任何一个stream 被creat， 那么本api 不可被调用

*/
AR_S32 ar_hal_vin_set_pipe_prop(AR_S32 fd_vin_dev,AR_S32 pipe_id,STRU_VIN_PIPE_PROPERTY_T *p_pipe_property);
/*
*@brief: 关闭指定的pipe
*@param:fd_vin_dev	打开的vin 设备fd	输入
        pipe_id	需要关闭的pipe的id。本id 最大16. 	输入
*@code
*@endcode
*@return: 0	    成功
         非0	失败
*@note:
*/
AR_S32 ar_hal_vin_close_pipe(AR_S32 fd_vin_dev,AR_S32 pipe_id);
/*
*@brief: Bind 设备和pipe。在open pipe 前必须bind 这个dev 到pipe。Bind 后所有对dev的操作转化为对pipe的操作
*@param:fd_vin_dev	打开的vin 设备fd	输入
        pipe_id	指定的pipe的id。本id 最大16. 	输入
        dev_id	Dev的id	输入

*@code
*@endcode
*@return: 0	    成功
         非0	失败
*@note: 需要打开pipe的时候，必须先bind dev到pipe
*/
AR_S32 ar_hal_vin_pipe_bind_to_dev(AR_S32 fd_vin_dev,AR_S32 pipe_id,AR_S32 dev_id);
/*
*@brief: 请参考ar_hal_vin_pipe_bind_to_dev，本api是对其的反向操作
*@param:
*@code
*@endcode
*@return:
*@note:
*/
AR_S32 ar_hal_vin_pipe_unbind_to_dev(AR_S32 fd_vin_dev,AR_S32 pipe_id,AR_S32 dev_id);
/*
*@brief: 在open pipe 前，必须把devid 和 pipe id 绑定。绑定后可以通过本api用pipe id 获取到bind的dev id。
*@param:fd_vin_dev	打开的vin 设备fd	输入
        pipe_id	指定的pipe的id。本id 最大16. 	输入
        p_dev_id	指向设备id的指针	输出
*@code
*@endcode
*@return: 0	成功
          非0	失败
*@note:
*/
AR_S32 ar_hal_vin_bind_get_dev_id_by_pipe_id(AR_S32 fd_vin_dev,AR_S32 pipe_id,AR_S32 *p_dev_id);
/*
*@brief:请参考ar_hal_vin_bind_get_dev_id_by_pipe_id，是其反向操作
*@param:
*@code
*@endcode
*@return:
*@note:
*/
AR_S32 ar_hal_vin_bind_get_pipe_id_by_dev_id(AR_S32 fd_vin_dev,AR_S32 dev_id,AR_S32 *p_pipe_id);
/*
*@brief:同过log的方式显示vin 系统的调试信息
*@param:fd_vin_dev	打开的vin 设备fd	输入
*@code
*@endcode
*@return: 0	成功
          非0	失败
*@note:
*/
AR_S32 ar_hal_vin_ar_pipe_show_all_log(AR_S32 fd_vin_dev);
/*
*@brief: 打开pipe设备，返回打开的设备的fd，一切关于pipe的操作，都必须在pipe的设备上完成，包括打开某一个pipe。一个pipe id 对应一个pipe 设备。
*@param:pipe_id	指定的需要打开pipe 设备的id	输入
*@code
*@endcode
*@return: >=0	成功,打开的pipe fd
          <0	失败

*@note:  这里有一个看似多余的东西在存在，也就是说fd 其实和id 是一一对应的，
但是在后续的对pipe的操作中，需要传递pipe id 和 pipe fd两者。
建议这两个值最好是匹配的，但是不匹配也是无所谓的，起根本作用的是pipe id。
Fd是用来进入驱动的一个代理而已。 也就是说你可以用pipe1的fd 传递 0的pipe id 来操作pipe0 而不会产生错误。
但是如果使用slect api的话，就必须严格对应了。 pipe1 slect 上来的事件必定是pipe1 的事件。
*/
AR_S32 ar_hal_vin_open_pipe_dev(AR_S32 pipe_id);
/*
*@brief: 关闭掉pipe 设备
*@param:fd_pipe	需要关掉的设备fd	输入
*@code
*@endcode
*@return:
*@note:
*/
AR_VOID ar_hal_vin_close_pipe_dev(AR_S32 fd_pipe);
/*
*@brief: ar_hal_vin_control, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:AR_S32 pipe_id
              AR_S32 ctl_code
              AR_VOID *p_data,
              AR_S32 size
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
*@code
*@endcode
*@return: 0:success 1:fail
*@note:
*/
AR_S32 ar_hal_vin_control(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 ctl_code,AR_VOID *p_data,AR_S32 size);

AR_S32 ar_hal_vin_set_stream_fps(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_STREAM_FPS_RATE_PRA_T *p_stream_fps);

AR_S32 ar_hal_vin_get_stream_fps(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_STREAM_FPS_RATE_PRA_T *p_stream_fps);

AR_S32 ar_hal_vin_set_stream_subsample_rate(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_STREAM_FPS_SUBSAMPLE_RATE_PRA_T *p_stream_sub_rate);

AR_S32 ar_hal_vin_get_stream_subsample_rate(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_STREAM_FPS_SUBSAMPLE_RATE_PRA_T *p_stream_sub_rate);

/*
*@brief: 在指定的pipe 上创建一个stream。正如概述提到的，
在bind dev 到pipe 后，pipe的抽象的连接关系的具体化还没有完成，
pipe需要creat stream 来具体化连接关系。创建一个stream 后，这个pipe 就有了实际的意义，
在在创建stream之前的所有对pipe的操作，都是为create stream 做准备，比如pipe 属性等。
*@param:fd_pipe	打开的pipe dev的fd	输入
pipe_id	需要在那个pipe id 上创建stream	输入
p_property	Stream的属性	输入
p_stream_id	指定creat的stream的id， 如果是-1，那么系统自己分配一个id，如果是其他，按照指定的id 生成stream。
如果指定的id 已经存在，那么返回错误	输入+输出

*@code
*@endcode
*@return: 0	成功
         非0	失败
*@note: 1在创建stream的过程中，不止stream 属性，pipe的一些属性也对stream的连接关系有着很大的影响。Sream属性只针对stream 本身，而pip 属性对所有stream都有影响。
	    2注意p_stream_id，这个值建议设置到-1，让系统自动的分配stream id，后面对stream操作直接传递就好。如果使用用户自己分配，请用户自己维护好这些id，不要用同一个id创建多次。

*/
AR_S32 ar_hal_vin_creat_stream(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_AR_STREAM_PROPERTY_T *p_property,AR_S32 *p_stream_id);
/*
*@brief:获取已经创建的stream的属性
*@param:fd_pipe	打开的pipe dev的fd	输入
pipe_id	需要在那个pipe id 上获取属性	输入
p_property	Stream的属性	输出
stream_id	需要获取的stream的属性的id	输入
*@code
*@endcode
*@return: 0	成功
         非0	失败
*@note:
*/
AR_S32 ar_hal_vin_get_stream_property(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id,STRU_AR_STREAM_PROPERTY_T *p_property);
/*
*@brief:在用户创建stream 后，可能有着对stream 添加一个私有的数据，这个api 就是把这个私有的数据添加给stream。
考察本api 可以发现，在添加私有数据的时候，有一个pos的参数，这个参数是指把这个私有数据添加到那个位置。
*@param:fd_pipe	打开的pipe dev的fd	输入
        pipe_id	指定的pipe id	输入
        stream_id	指定的stream id	输入
        p_data	需要添加的私有数据的指针，其实就是把这个指针本身加入到私有数据中	输入
        pos	加入到私有数据的那个位置 [0-----3]	输入
*@code
*@endcode
*@return: 0	成功
         非0	失败
*@note:建议用户不要使用多个stream 私有数据，如果有多个私有数据请打包成结构体。
	   本api 不对已经设置的私有数据进行判断，设置新的会覆盖旧的
	   如果多进程设置，请先调用get, 只有get 到空的私有数据的pos 才可以设置
*/
AR_S32 ar_hal_vin_get_stream_data(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id,AR_VOID **pp_data,AR_S32 pos);
/*
*@brief: 请参考ar_hal_vin_set_stream_data ，本api是其的反向操作
*@param:
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/
AR_S32 ar_hal_vin_set_stream_data(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id,AR_VOID *p_data,AR_S32 pos);
/*
*@brief: ar_hal_vin_creat_stream_man_link, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:AR_S32 pipe_id
              AR_S32 stream_id
		   STRU_AR_STREAM_PROPERTY_T *p_property,
		   STRU_LINK_PRA_T *p_link,
		   AR_S32 *p_stream_id
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/
AR_S32 ar_hal_vin_creat_stream_man_link(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_AR_STREAM_PROPERTY_T *p_property,STRU_LINK_PRA_T *p_link,AR_S32 *p_stream_id);
/*
*@brief: 本api 重新改变stream的属性，主要用在快速改变分辨率的场合。本api 只能用在已经存在的stream，
且不需要停止stream 就可以直接修改stream的核心参数。比如 w h 等从而完成快速切换分辨的需求。
*@param:fd_pipe	打开的pipe dev的fd	输入
        pipe_id	需要操作的 Pipe id	输入
        stream_id	指定的stream的id	输入
        p_property	Stream的属性	输入
*@code
*@endcode
*@return: 0	成功
         非0	失败
*@note:改变后的分辨率不能超过当时pipe 使用的sensor的输出的分辨率
	改变后的分辨率不能超过启动stream的时候使用的total w h。
	改变后的分辨率占用的内存不能超过create stream的时候的分配的内存的大小
	快速的切换在某些情况下，可能导致屏幕的瞬时闪烁
*/
AR_S32 ar_hal_vin_reshape_stream(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id,STRU_AR_STREAM_PROPERTY_T *p_property);
/*
*@brief: 删除某一个创建的stream，本api 使用在vin 的deinit 或者叫stop 过程中。当stop stream ，并删除streambuffer 后，需要调用本api取删除这个stream。
*@param:fd_pipe	打开的pipe dev的fd	输入
        pipe_id	需要操作的 Pipe id	输入
        stream_id	指定的stream 的id	输入
*@code
*@endcode
*@return: 0	成功
          非0	失败
*@note:

*/
AR_S32 ar_hal_vin_delete_stream(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id);
/*
*@brief: 准备删除stream。在调用 ar_hal_vin_delete_stream 前调用的api， 主要保证能vin 系统内部及stream相关操作已经完成了。
*@param:fd_pipe	打开的pipe dev的fd	输入
        pipe_id	需要操作的 Pipe id	输入
*@code
*@endcode
*@return:0	成功
         非0	失败
*@note: 本api 关闭了所有的stream 后才可以调用。所以不需要指定stream id
*/

AR_S32 ar_hal_vin_preprae_delete_stream(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id);
/*
*@brief: 获取系统已经创建好的stream。 一般用在多进程中其他进程想要对stream 获取buffer时的查询
*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	需要操作的 Pipe id	输入
p_stream_id	获取的stream id的数组	输出
p_num	获取到的创建好的stream的数量	输出
*@code
*@endcode
*@return:0	成功
         非0	失败
*@note: p_stream_id 数组开的大一些，比如64，以防止内存溢出
*/
AR_S32 ar_hal_vin_get_all_stream(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 *p_stream_id,AR_S32 *p_num);
/*
*@brief: 启动一个指定的steam
*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	需要操作的 Pipe id	输入
stream_id	指定的stream 的id	输入

*@code
*@endcode
*@return:0	成功
         非0	失败
*@note:	启动stream 前，确保stream 的buffer 已经被创建
	创建好的stream buffer 被推到strem 中
	如果是trigger 模式，本api 不会触发sensor 出图

*/
AR_S32 ar_hal_vin_start_stream(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id);


/*
*@brief: ar_hal_vin_get_stream_status
*@param:
           AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 stream_id
		   AR_S32 status 0:off 1 on
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/
AR_S32 ar_hal_vin_get_stream_status(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id,AR_S32 *status);

/*
*@brief: 触发sensor的运行 ，某些sensor有触发运行的模式，每操作一次trigger ，sensor 可以出若干张图像
*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	指定的pipe id	输入
p_prop	触发属性	输入

*@code
*@endcode
*@return:0	成功
         非0	失败
*@note: 使用本api 必须在pipe 属性中，设置pipe 进入触发模式
	    在stream on 后可以根据需要调用本api

*/
AR_S32 ar_hal_vin_trigger(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_AR_CAMERA_TRIGGER_PROP_T *p_prop);

AR_S32 ar_hal_vin_set_manual_crop(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_AR_CAMERA_MANUAL_CROP_PROP_T *p_prop);

AR_S32 ar_hal_vin_tuning_pra_update(AR_S32 fd_pipe,AR_S32 pipe_id);


/*
*@brief: 停止指定的stream，用在停止流程中
*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	指定的pipe id	输入
stream_id	指定的stream id
*@code
*@endcode
*@return:0	成功
         非0	失败
*@note:
*/
AR_S32 ar_hal_vin_stop_stream(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id);
/*
*@brief: 挂起一个指定的stream，相比stream ，挂起stream 没有对buffer 状态等进行传输，只是在hw上关闭相关模块，一种使用本api的地方是在有err的时候用来恢复。
其他任何需要暂时关闭stream的情况，都可以使用本api
*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	指定的pipe id	输入
stream_id	指定的stream id

*@code
*@endcode
*@return:0	成功
        非0	失败
*@note:
*/

AR_S32 ar_hal_vin_suspend_stream(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id, AR_S32 mode);
/*
*@brief: 唤醒指定的stream，和 ar_hal_vin_suspend_stream 对应。这两个api 必须派对使用。
*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	指定的pipe id	输入
stream_id	指定的stream id	输入

*@code
*@endcode
*@return:0	成功
非0	失败
*@note:
*/

AR_S32 ar_hal_vin_resume_stream(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id,AR_S32 mode);
/*
*@brief: Stream的client， 我们知道，用户创建了stream 后，目的是从stream 中获取buffer。
那么如何处理多个用户获取buffer的问题就引出了client。 用户获取stream buffer的时候，需要指定一个client 的id。
这个id 是需要通过本api creat 出来。Client的应用，可以让多进程或获取同一个stream的buffer 称为可能。
如果不同的用户用相同的client 获取buffer，那么会导致竞争，也就是说有且只有一个用户在一个是可获取到buffer。
*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	指定的pipe id	输入
stream_id	指定的stream id	输入
p_client_id	返回的创建出来的client id	输出

*@code
*@endcode
*@return: 0	成功
非0	失败
*@note:
*/
AR_S32 ar_hal_vin_create_stream_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id,AR_S32 *p_client_id);
/*
*@brief:Stream的client， 我们知道，用户创建了stream 后，目的是从stream 中获取buffer。那么如何处理多个用户获取buffer的问题就引出了client。
用户获取stream buffer的时候，需要指定一个client 的id。 这个id 是需要通过本api creat 出来。Client的应用，可以让多进程或获取同一个stream的buffer 称为可能。
如果不同的用户用相同的client 获取buffer，那么会导致竞争，也就是说有且只有一个用户在一个是可获取到buffer。
本api 用来删除ar_hal_vin_create_stream_client的生成client
*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	指定的pipe id	输入
stream_id	指定的stream id	输入
client_id	Stream的cllient id	输出

*@code
*@endcode
*@return:0	成功
非0	失败
*@note:
*/
AR_S32 ar_hal_vin_delete_stream_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id,AR_S32 client_id);
/*
*@brief: 本api 推一个stream buffer 给pipe 处理，主要用在通过isp 处理用户的raw数据。
Sdk 提供了isp in 设备，当用isp in dev 绑定pipe 启动stream 后，需要用户把要处理的raw数据发送给pipe 处理，本api 就是完成这个功能的。
在一般的程序开发中，用户需要单独启动一个线程调用ar_hal_vin_dq_stream_buffer 来取回
ar_hal_vin_q_stream_buffer 推进去的buffer，然后放到一个队列中。而另一个线程从队列中取到buffer ，并根据当前需求，填充数据，然后发送给pipe。
*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	指定的pipe id	输入
stream_id	指定的stream id	输入
p_stream_buffe	stream_buffer 信息	输入

*@code
*@endcode
*@return:0	成功
非0	失败
*@note:本api 是推buffer 给isp 处理的，构成的是isp 输入的循环过程
*/

AR_S32 ar_hal_vin_q_stream_buffer(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id,STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer);
/*
*@brief:构造isp in 数据处理buffer 循环的函数。取出推给isp in dev的已经处理完成对的buffer
*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	指定的pipe id	输入
stream_id	指定的stream id	输入
p_stream_buffe	stream_buffer 信息	输入
timeout_ms	超时参数（< 0: 阻塞模式;  0: 非阻塞模式;  > 0: 超时时间，单位毫秒）	输入

*@code
*@endcode
*@return:0	成功
非0	失败
*@note: Dq 出的buffer 是实例，不是buffer的指针。
*/

AR_S32 ar_hal_vin_dq_stream_buffer(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id,STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer, AR_S32 timeout_ms);
/*
*@brief: 在构建向isp in dev 的过程中，stream buffer 首先需要向pipe 申请，以后q 以及dq 需要使用申请出来的buffer。用此buffer 来构造循环过程
*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	指定的pipe id	输入
stream_id	指定的stream id	输入
p_stream_buffe	stream_buffer 信息的数组	输出
p_size	申请到的stream buffer的数量	输出
*@code
*@endcode
*@return:0	成功
        非0	失败
*@note:
*/

AR_S32 ar_hal_vin_requst_stream_buffer(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id,STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer,AR_S32 *p_size);
/*
*@brief: 在我们creat 好stream 后，就可以在stream 上获取stream的buffer了，为了获取stream的buffer，
用户必须为stream 分配内存，并用stream buffer 这样的实例来表示他。 本api 用来创建从表示内存的stream buffer 实例，
返回的实例中给出了stream的buffer的一些相关信息，包括内存数目，需要内存的大小等。
用户调用完本api ，需要真正关心的就是mem的大小和数目，从而根据此分配mem。
内存的分配是归于用户的，vin 系统只负责分配mem的表示stream buffer。
需要注意的是，本api 的stream buffer 是针对vin的输出的。不同于ar_hal_vin_requst_stream_buffer 是针对vin的输入的。

*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	指定的pipe id	输入
stream_id	指定的stream id	输入
p_stream_buffe	stream_buffer 信息	输出
*@code
*@endcode
*@return: 0	成功
非0	失败
*@note:本api 需要连续调用，从而creat 出若干stream buffer。
*/
AR_S32 ar_hal_vin_creat_stream_buffer(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id,STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer);
/*
*@brief: 删除stream buffer info 的索引指定的已经create 好的streambuffer。
这里需要说明的是，无论是create 返回的streambuffer 还是delete 的stream buffer，
不是vin的stream buffer 本身，而是stream buffer的表示，正如这个类型的名称所表示的一样，是streambuffer的信息。
是stream buffer的一个表示。Vin 通过这个表示中的index 找到真正的stream buffer。Vin 内部操作的是stream buffer
而不息api 传递进去的stream buffer info

*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	指定的pipe id	输入
stream_id	指定的stream id	输入
p_stream_buffe	stream_buffer 信息	输入

*@code
*@endcode
*@return:0	成功
非0	失败
*@note:需要理解的是，stream buffer的相关api，传递的stream buffer的参数中的index 是及其重要的，用户不能对其进行任何的修改，
而且也必须是create strean buffer 返回的stream Buffer 中的。

*/

AR_S32 ar_hal_vin_delete_stream_buffer(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id,STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer);
/*
*@brief:清空指定stream id 的stream buffer， 为delete stream buffer 做准备
一般用在stop stream 后，不能随时调用。
*@param:
fd_pipe	打开的pipe dev的fd	输入
pipe_id	指定的pipe id	输入
stream_id	指定的stream id	输入
*@code
*@endcode
*@return: 0	成功
非0	失败
*@note:只能在停止stream 后，删除stream buffer 前调用
*/

AR_S32 ar_hal_vin_flush_stream_buffer(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 stream_id);
/*
*@brief: ar_hal_vin_process_tuning_req, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   STRU_PIPE_SET_TUNING_REQUEST_T *p_req
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_process_tuning_req(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_VIN_MEM_T *tuning_req_mem);
/*
*@brief: ar_hal_vin_get_tuning_req, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   STRU_PIPE_SET_TUNING_REQUEST_T **pp_req
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_get_tuning_req(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_VIN_MEM_T *tuning_req_mem);
/*
*@brief: ar_hal_vin_release_tuning_req, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   STRU_PIPE_SET_TUNING_REQUEST_T *p_req
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_release_tuning_req(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_VIN_MEM_T *tuning_req_mem);
/*
*@brief:打开stream dev，对于创建好的每一个stream， 都可以打开一个stream设备。
当创建好stream buffer 并分配内存后，需要调用stream dev 先把stream buffer 推到stream上，然后start stream，然后就可以获取stream的buffer了

*@param:
pipe_id	指定的pipe id	输入
stream_id	指定的stream id	输入

*@code
*@endcode
*@return: >=0	成功打开的fd
<0	失败
*@note:
*/
AR_S32 ar_hal_vin_open_stream_dev(AR_S32 fd_pipe,AR_S32 stream_id);
/*
*@brief 关闭stream dev
*@param:
 fd_stream	Stream 设备的fd	输入
*@code
*@endcode
*@return:
*@note:
*/
AR_VOID ar_hal_vin_close_stream_dev(AR_S32 fd_stream);
/*
*@brief: 获取stream上的stream buffer，本api 是阻塞函数，只有vin 输出buffer的时候才返回
或者timeout 后返回
需要指定一个client，每一个用户建议使用不同的client id 来获取buffer。
如果使用相同的client id 获取buffer， 那么多个用户是抢占方式的。一个buffer 要么A进程获得，要么B 进程获得。

*@param:
fd_stream	指定的stream的fd	输入
pipe_id	指定的pipe id	输入
stream_id	指定的stream的id	输入
client_id	指定的clinet id	输入
p_stream_buffer	获取到的stream buffer	输出
 timeout_ms	超时时间	输入

*@code
*@endcode
*@return: 0	成功
非0	失败
*@note:获取到buffer 后，请及时返还
*/

AR_S32 ar_hal_vin_get_stream_buffer(AR_S32 fd_stream,AR_S32 pipe_id,AR_S32 stream_id,AR_S32 client_id,STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer,AR_S32 timeout_ms);
/*
*@brief:在create 完成stream buffer 后，调用本api 把所有buffer 推到pipe的stream 上。
当ar_hal_vin_get_stream_buffer 后并使用完成后，仍然使用本api 返还到stream 上。
当需要停止stream的时候，必须停止调用api 返回buffer。
删除streambuffer 前必须等待说有的buffer 已经不在vin的stream 上。

*@param:
fd_stream	指定的stream的fd	输入
pipe_id	指定的pipe id	输入
stream_id	指定的stream的id	输入
p_stream_buffer	需要返回的stream buffer	输入

*@code
*@endcode
*@return:0	成功
非0	失败
*@note:当ar_hal_vin_get_stream_buffer 后并使用完成后，及时的使用本api 返还到stream 上。
*/

AR_S32 ar_hal_vin_return_stream_buffer(AR_S32 fd_stream,AR_S32 pipe_id,AR_S32 stream_id,STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer);
/*
*@brief: ar_hal_vin_get_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   STRU_AR_EVENT_INFO_T *p_event_info
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/
AR_S32 ar_hal_vin_get_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,STRU_AR_EVENT_INFO_T *p_event_info);
/*
*@brief: ar_hal_vin_release_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   STRU_AR_EVENT_INFO_T *p_event_info
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_release_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,STRU_AR_EVENT_INFO_T *p_event_info);
/*
*@brief: ar_hal_vin_release_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 *p_client_id
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_create_event_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 *p_client_id);
/*
*@brief: ar_hal_vin_delete_event_client, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/
AR_S32 ar_hal_vin_delete_event_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id);
/*
*@brief: ar_hal_vin_register_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   AR_S32 event
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_register_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,AR_S32 event);
/*
*@brief: ar_hal_vin_unregister_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   AR_S32 event
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_unregister_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,AR_S32 event);
/*
*@brief: ar_hal_vin_get_sensor_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   STRU_AR_EVENT_INFO_T *p_event_info
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_get_sensor_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,STRU_AR_EVENT_INFO_T *p_event_info);
/*
*@brief: ar_hal_vin_release_sensor_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   STRU_AR_EVENT_INFO_T *p_event_info
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_release_sensor_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,STRU_AR_EVENT_INFO_T *p_event_info);
/*
*@brief: ar_hal_vin_create_sensor_cb_event_client, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 *p_client_id,
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_create_sensor_cb_event_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 *p_client_id);
/*
*@brief: ar_hal_vin_delete_sensor_cb_event_client, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/
AR_S32 ar_hal_vin_delete_sensor_cb_event_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id);
/*
*@brief: ar_hal_vin_register_sensor_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,AR_S32 event
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_register_sensor_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,AR_S32 event);
/*
*@brief: ar_hal_vin_unregister_sensor_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   AR_S32 event
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_unregister_sensor_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,AR_S32 event);
/*
*@brief: ar_hal_vin_get_aec_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   STRU_AR_EVENT_INFO_T *p_event_info
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_get_aec_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,STRU_AR_EVENT_INFO_T *p_event_info);
/*
*@brief: ar_hal_vin_release_aec_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   STRU_AR_EVENT_INFO_T *p_event_info
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_release_aec_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,STRU_AR_EVENT_INFO_T *p_event_info);
/*
*@brief: ar_hal_vin_create_aec_cb_event_client, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 *p_client_id
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/
AR_S32 ar_hal_vin_create_aec_cb_event_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 *p_client_id);
/*
*@brief: ar_hal_vin_delete_aec_cb_event_client, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_delete_aec_cb_event_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id);
/*
*@brief: ar_hal_vin_register_aec_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   AR_S32 event
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_register_aec_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,AR_S32 event);
/*
*@brief: ar_hal_vin_unregister_aec_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   AR_S32 event
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_unregister_aec_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,AR_S32 event);
/*
*@brief: ar_hal_vin_get_awb_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   STRU_AR_EVENT_INFO_T *p_event_info
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_get_awb_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,STRU_AR_EVENT_INFO_T *p_event_info);
/*
*@brief: ar_hal_vin_release_awb_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   STRU_AR_EVENT_INFO_T *p_event_info
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_release_awb_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,STRU_AR_EVENT_INFO_T *p_event_info);
/*
*@brief: ar_hal_vin_create_awb_cb_event_client, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 *p_client_id
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_create_awb_cb_event_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 *p_client_id);
/*
*@brief: ar_hal_vin_create_awb_cb_event_client, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_delete_awb_cb_event_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id);
/*
*@brief: ar_hal_vin_register_awb_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   AR_S32 event
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_register_awb_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,AR_S32 event);
/*
*@brief: ar_hal_vin_unregister_awb_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   AR_S32 event
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_unregister_awb_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,AR_S32 event);
/*
*@brief: ar_hal_vin_get_af_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   STRU_AR_EVENT_INFO_T *p_event_info
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_get_af_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,STRU_AR_EVENT_INFO_T *p_event_info);
/*
*@brief: ar_hal_vin_release_af_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   STRU_AR_EVENT_INFO_T *p_event_info
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_release_af_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,STRU_AR_EVENT_INFO_T *p_event_info);
/*
*@brief: ar_hal_vin_create_af_cb_event_client, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 *p_client_id
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_create_af_cb_event_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 *p_client_id);
/*
*@brief: ar_hal_vin_delete_af_cb_event_client, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/
AR_S32 ar_hal_vin_delete_af_cb_event_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id);
/*
*@brief: ar_hal_vin_register_af_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   AR_S32 event
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_register_af_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,AR_S32 event);
/*
*@brief: ar_hal_vin_unregister_af_cb_event, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_S32 client_id,
		   AR_S32 event
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_unregister_af_cb_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,AR_S32 event);


/**
 * @brief  ar_hal_vin_get_meta_event
 * @param AR_S32 fd_pipe
                AR_S32 pipe_id
                AR_S32 client_id
                STRU_AR_EVENT_INFO_T *p_event_info

 * @return 0 success !0 failed
 * @note null
 */

AR_S32 ar_hal_vin_get_meta_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,STRU_AR_EVENT_INFO_T *p_event_info);
/**
 * @brief  ar_hal_vin_release_meta_event
 * @param AR_S32 fd_pipe
                AR_S32 pipe_id
                AR_S32 client_id
                STRU_AR_EVENT_INFO_T *p_event_info

 * @return 0 success !0 failed
 * @note null
 */

AR_S32 ar_hal_vin_release_meta_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,STRU_AR_EVENT_INFO_T *p_event_info);
/**
 * @brief  ar_hal_vin_create_meta_event_client
 * @param AR_S32 fd_pipe
                AR_S32 pipe_id
			AR_S32 *p_client_id

 * @return 0 success !0 failed
 * @note null
 */

AR_S32 ar_hal_vin_create_meta_event_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 *p_client_id);
/**
 * @brief  ar_hal_vin_delete_meta_event_client
 * @param AR_S32 fd_pipe
                AR_S32 pipe_id
                AR_S32 client_id

 * @return 0 success !0 failed
 * @note null
 */

AR_S32 ar_hal_vin_delete_meta_event_client(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id);
/**
 * @brief  ar_hal_vin_register_meta_event
 * @param AR_S32 fd_pipe
                AR_S32 pipe_id
                AR_S32 client_id
                AR_S32 event
 * @return 0 success !0 failed
 * @note null
 */

AR_S32 ar_hal_vin_register_meta_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,AR_S32 event);
/**
 * @brief  ar_hal_vin_unregister_meta_event
 * @param AR_S32 fd_pipe
                AR_S32 pipe_id
                AR_S32 client_id
                AR_S32 event

 * @return 0 success !0 failed
 * @note null
 */

AR_S32 ar_hal_vin_unregister_meta_event(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 client_id,AR_S32 event);

/*
*@brief: 本api获取sensor dev的可变参数信息，主要是分辨率，格式，mpi等的使用情况，为修改这些参数做准备，
用户可以再app 中修改这些参数，并适配raw图或者是sensor送上来的raw数据的描述
*@param:
fd_pipe	指定的pipe的fd	输入
pipe_id	指定的pipe id	输入
p_var_pra	设备或sensor的可变参数	输出
*@code
*@endcode
*@return:0	成功
非0	失败
*@note:本api 需要的是pipe id 而不是dev id。所以调用本api前必须bind dev id 和 pipe id，然后通过pipe  id 来操作设备
	   本api 只要打开pipe fd 就可以了，不需要打开pipe。当然也可以打开pipe 后调用
*/

AR_S32 ar_hal_vin_set_sensor_var_pra(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_DEV_INFO_VAR_T *p_var_pra);
/*
*@brief: ar_hal_vin_get_sensor_dev_var_pra, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   STRU_DEV_INFO_VAR_T *p_var_pra
*@endcode
*@return: 0:success 1:faild
*@note:
*/
AR_S32 ar_hal_vin_get_sensor_var_pra(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_DEV_INFO_VAR_T *p_var_pra);
/*
*@brief: ar_hal_vin_register_aec_algo_lib, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_CHAR *p_lib_name
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_register_aec_algo_lib(AR_S32 fd_pipe,AR_S32 pipe_id,AR_CHAR *p_lib_name);
/*
*@brief: ar_hal_vin_unregister_aec_algo_lib, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_CHAR *p_lib_name
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_unregister_aec_algo_lib(AR_S32 fd_pipe,AR_S32 pipe_id,AR_CHAR *p_lib_name);
/*
*@brief: ar_hal_vin_register_awb_algo_lib, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_CHAR *p_lib_name
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_register_awb_algo_lib(AR_S32 fd_pipe,AR_S32 pipe_id,AR_CHAR *p_lib_name);
/*
*@brief: ar_hal_vin_unregister_awb_algo_lib, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_CHAR *p_lib_name
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_unregister_awb_algo_lib(AR_S32 fd_pipe,AR_S32 pipe_id,AR_CHAR *p_lib_name);
/*
*@brief: ar_hal_vin_register_af_algo_lib, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_CHAR *p_lib_name
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_register_af_algo_lib(AR_S32 fd_pipe,AR_S32 pipe_id,AR_CHAR *p_lib_name);
/*
*@brief: ar_hal_vin_unregister_af_algo_lib, detail ref to 9301_SDK_VIN_HAL_API.docx
*@param:
              AR_S32 fd_pipe: the fd opend by ar_hal_vin_open_pipe_dev
		   AR_S32 pipe_id,
		   AR_CHAR *p_lib_name: the name af algo lib
*@code
*@endcode
*@return: 0:success 1:faild
*@note:
*/

AR_S32 ar_hal_vin_unregister_af_algo_lib(AR_S32 fd_pipe,AR_S32 pipe_id,AR_CHAR *p_lib_name);


AR_S32  ar_hal_vin_aec_update(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_AEC_OUT_T *p_aec_out);
AR_S32  ar_hal_vin_awb_update(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_AWB_OUT_T *p_awb_out);
AR_S32  ar_hal_vin_af_update(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_AF_OUT_T *p_af_out);


AR_S32  ar_hal_vin_get_aec_update(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_AEC_OUT_T *p_aec_out);
AR_S32  ar_hal_vin_get_awb_update(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_AWB_OUT_T *p_awb_out);
AR_S32  ar_hal_vin_get_af_update(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_AF_OUT_T *p_af_out);


AR_S32  ar_hal_vin_vsync_update(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_AR_CAMERA_SOF_PRO_T *p_sof_prop);


AR_S32  ar_hal_vin_aec_run(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_AEC_CB_EVENT_RUN_PRA_T *run_pra);
AR_S32  ar_hal_vin_awb_run(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_AWB_CB_EVENT_RUN_PRA_T *run_pra);
AR_S32  ar_hal_vin_af_run(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_AF_CB_EVENT_RUN_PRA_T *run_pra);



/**
 * @brief  set camera aec mode,default is auto,we can use the api to change it
 * @param  fd  the fd of ar_cam_src_open
 * @param  mode  The mode of camera supported
 * @code
 * typedef enum
 * {
 *   AEC_AUTO,
 *   AEC_FULL_SWEEP, //reserved
 *   AEC_AVERGATE,  //reserved
 *   AEC_CENTER_WEIGHT, //reserved
 *   AEC_SPOT,//reserved
 *   AEC_MANUAL,
 *   AEC_LOCKED,
 *   AEC_MANUAL_UPDATE,
 * }ENUM_AEC_MODE_T;
 * @endcode
 * @return 0 success !0 failed
 * @note  only AEC_AUTO AEC_MANUAL AEC_LOCKED is supported
 */
AR_S32 ar_hal_vin_aec_set_mode(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 mode);
/**
 * @brief  set manual aec control
 * @param fd  the fd of ar_cam_src_open
 * @param aec_info  the aec info need to set
 * @code
 * typedef struct
 * {
 *   AR_S32   aec_mode;     //aec mode ref to ENUM_AEC_MODE_T
 *   AR_FLOAT gain;         //gain value of exp, min is 1 ,max value decided by your hw
 *   AR_FLOAT exp_time_us;  //the exp time of sensor ,min max value decided by your hw
 *   uint32_t luma_target; //the luma target of aec
 * }STRU_AEC_SET_INFO_T;
 * @endcode
 * @return   0 success !0 failed
 * @note  the mode of STRU_AEC_SET_INFO_T only AEC_AUTO AEC_MANUAL is supported
 */

AR_S32 ar_hal_vin_aec_get_mode(AR_S32 fd_pipe,AR_S32 pipe_id,AR_S32 *mode);
/**
* @brief  get aec control mode
 * @param fd  the fd of ar_cam_src_open
 * @param aec_mode
 * @code
 * typedef struct
 * {
 *   AR_S32   aec_mode;     //aec mode ref to ENUM_AEC_MODE_T
 *   AR_FLOAT gain;         //gain value of exp, min is 1 ,max value decided by your hw
 *   AR_FLOAT exp_time_us;  //the exp time of sensor ,min max value decided by your hw
 *   uint32_t luma_target; //the luma target of aec
 * }STRU_AEC_SET_INFO_T;
 * @endcode
 * @return   0 success !0 failed
 * @note  the mode of STRU_AEC_SET_INFO_T only AEC_AUTO AEC_MANUAL is supported
*/
AR_S32 ar_hal_vin_aec_manu_set(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_AEC_SET_INFO_T *p_aec_info);
/**
 * @brief set awb mode ,default is auto,we can use the api to change it
 * @param  fd  the fd of ar_cam_src_open
 * @param  mode  wb mode
 * @code
 * typedef enum
 *  {
 *		AWB_D75=0,
 *		AWB_D65,
 *		AWB_D50,
 *		AWB_NOON,
 *		AWB_CW,
 *		AWB_TL84,
 *		AWB_A,
 *		AWB_H,
 *		AWB_CUST1,
 *		AWB_CUST2,
 *		AWB_AUTO,
 *	 	AWB_MANUAL,
 *	 	AWB_MANUAL_UPDATE,
 *	}ENUM_AWB_MODE_T;
 * @endcode
 * @return  0 success !0 failed
 * @note  null
 */
AR_S32 ar_hal_vin_awb_set_mode(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 mode);
/**
 * @brief  set manual awb
 * @param  fd  the fd of ar_cam_src_open
 * @param  awb_info  The awb information
 * @code
 *	 typedef struct
 *	{
 *	  AR_S32 awb_mode;  //wb mode ,ref to ENUM_AWB_MODE_T
 *	  uint32_t cct;  //cct,such as 5000
 *	  AR_FLOAT r_gain;  //r gain of wb you want to set
 *	  AR_FLOAT g_gain;  //g gain of wb you want to set , set to 1 always suggustion
 *	  AR_FLOAT b_gain;  //b gain of wb you want to set
 *	}STRU_AWB_SET_INFO_T;
 * @endcode
 * @return   0 success !0 failed
 * @note null
 */
AR_S32 ar_hal_vin_awb_manu_set(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_AWB_SET_INFO_T *p_awb_info);
/**
 * @brief  set camera af mode,default is auto,we can use the api to change it
 * @param  fd  the fd of ar_cam_src_open
 * @param  mode  The mode of camera supported
 * @code
 * typedef enum
 * {
 *   AF_AUTO,
 *   AF_CAF,
 *   AF_MACRO, //reserved
 *   AF_INFINITE, //reserved
 *   AF_MANUAL,//reserved
 * }ENUM_AF_MODE_T;
 * @endcode
 * @return 0 success !0 failed
 * @note  only AF_AUTO AF_CAF is supported
 */
AR_S32 ar_hal_vin_af_set_mode(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 mode);
AR_S32 ar_hal_vin_af_get_mode(AR_S32 fd_pipe, AR_S32 pipe_id, AR_S32 *mode);
/**
 * @brief  get af control mode
 * @param fd  the fd of ar_cam_src_open
 * @param af_mode
 * @code
 * typedef struct
 * {
 *   AR_U32 af_mod;
 *   AR_U32 len_position;
 *   AR_U32 total_steps;
 * }STRU_AF_SET_INFO_T;
 * @endcode
 * @return   0 success !0 failed
 * @note  the mode of STRU_AF_SET_INFO_T only AF_AUTO AF_MANUAL is supported
 */
AR_S32 ar_hal_vin_af_manu_set(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_AF_SET_INFO_T *p_af_info);
/**
 * @brief  set the af stats roi
 *
 * @param  fd   the fd of ar_cam_src_open
 * @param  roi  The roi to af stats
 * @code
 * //the af use the 1 as max and 0 as min
 * //such as
 * {
 *    0.2,
 *    0.2,
 *    0.4,
 *    0.4,
 *  }
 *  //if the picture is 100x100
 *  //so the roi is
 *  //20,20, 40,40
 *  typedef struct
 *  {
 *   AR_FLOAT x;
 *   AR_FLOAT y;
 *   AR_FLOAT width;
 *   AR_FLOAT height;
 *  }STRU_CAM_ROI_T;
 * @endcode
 * @return 0 success !0 failed
 * @note   not suggesiton to use, it's a expert api
 */
AR_S32 ar_hal_vin_set_af_roi(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_CAM_ROI_T  *p_roi);
AR_S32 ar_hal_vin_trigger_focus(AR_S32 fd_pipe, AR_S32 pipe_id);

/**
 * @brief  get aec awb af information
 * @param  fd  the fd of ar_cam_src_open
 * @param  info  The information
 * @code
 *	typedef struct
 *	{
 *	    STRU_AEC_SET_INFO_T  aec_info;
 *	    STRU_AWB_SET_INFO_T  awb_info;
 *	}STRU_BASIC_3A_INFO_T;
 *	typedef struct
 *	{
 *	  AR_S32   aec_mode;     //aec mode ref to ENUM_AEC_MODE_T
 *	  AR_FLOAT gain;         //gain value of exp, min is 1 ,max value decided by your hw
 *	  AR_FLOAT exp_time_us;  //the exp time of sensor ,min max value decided by your hw
 *	  uint32_t luma_target; //the luma target of aec
 *	}STRU_AEC_SET_INFO_T;
 *	typedef struct
 *	{
 *	  AR_S32 awb_mode;  //wb mode ,ref to ENUM_AWB_MODE_T
 *	  uint32_t cct;  //cct,such as 5000
 *	  AR_FLOAT r_gain;  //r gain of wb you want to set
 *	  AR_FLOAT g_gain;  //g gain of wb you want to set , set to 1 always suggustion
 *	  AR_FLOAT b_gain;  //b gain of wb you want to set
 *	}STRU_AWB_SET_INFO_T;
 * @endcode
 * @return   0 success !0 failed
 * @note the af info is invalid now
 */
AR_S32 ar_hal_vin_get_3a_info(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_BASIC_3A_INFO_T *p_info);
/**
 * @brief  ar_cam_get_3a_info_ext, get more 3a info
 * @param  fd  the fd of ar_cam_src_open
 * @param  info  The information
 * @return   0 success !0 failed
 * @note the af info is invalid now
 */
AR_S32 ar_hal_vin_get_3a_info_ext(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_ALGO_3A_GET_INFO_T *p_info);

AR_S32 ar_hal_vin_set_deskew(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 lane_id, AR_S32 val);

/**
 * @brief trigger camera error proc
 * @param  fd  the fd of ar_cam_src_open
 * @param  err_code the error code in cam_src_error_e
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_trigger_error_proc(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 err_code);
/**
 * @brief set camera brightness
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */

AR_S32 ar_hal_vin_set_brightness(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 val);
/**
 * @brief get camera brightness
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_get_brightness(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *p_val);
/**
 * @brief set camera contrast
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_set_contrast(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 val);
/**
 * @brief get camera contrast
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_get_contrast(AR_S32 fd_pipe, AR_S32 pipe_id,AR_S32 *p_val);

/**
 * @brief set camera global contrast (only suport ir now, do not support visible light)
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_set_global_contrast(AR_S32 fd_pipe, AR_S32 pipe_id, AR_U32 val);

/**
 * @brief get camera global contrast (only suport ir now, do not support visible light)
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_get_global_contrast(AR_S32 fd_pipe, AR_S32 pipe_id, AR_U32 *p_val);

/**
 * @brief set camera defog
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_set_ltm(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 val);
/**
 * @brief get camera defog
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_get_ltm(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *p_val);


AR_S32 ar_hal_vin_ltm_algo_update(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_CAM_PRA_LTM_ALGO_UPDATE_T *update);
AR_S32 ar_hal_vin_ltm_algo_run(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_CAM_PRA_LTM_ALGO_RUN_T *run);


/**
 * @brief set camera shapeness
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_set_shapeness(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 val);
/**
 * @brief get camera shapeness
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_get_sharpness(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *p_val);

/**
 * @brief set camera detail enhance strength (only suport ir now, do not support visible light)
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_set_detail_enhance(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_DETAIL_ENH_PRA_T *p_pra);

/**
 * @brief get camera detail enhance strength (only suport ir now, do not support visible light)
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_get_detail_enhance(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_DETAIL_ENH_PRA_T *p_pra);

/**
 * @brief get camera saturability
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_set_saturability(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 val);
/**
 * @brief get camera saturability
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_get_saturability(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *p_val);


/**
 * @brief set camera hue
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_set_hue(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 val);
/**
 * @brief get camera hue
 * @param  fd  the fd of ar_cam_src_open
 * @param  val  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note    null
 */
AR_S32 ar_hal_vin_get_hue(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *p_val);
/**
 * @brief  set de3d (video denoise) strenght
 * @param  fd   the fd of ar_cam_src_open
 * @param  strength   the strength of de3d,  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note   de3d only can be used when camera input size as isp output size same
 */
AR_S32 ar_hal_vin_de3d_set_strength(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 strength);
/**
 * @brief  get de3d (video denoise) strenght
 * @param  fd   the fd of ar_cam_src_open
 * @param  strength   the strength of de3d,  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note   de3d only can be used when camera input size as isp output size same
 */
AR_S32 ar_hal_vin_de3d_get_strength(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *strength);
/**
 * @brief  set rnr (raw denoise) strenght
 * @param  fd   the fd of ar_cam_src_open
 * @param  strength   the strength of rnr,  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note   rnr only can be used when camera input size as isp output size same
 */
AR_S32 ar_hal_vin_rnr_set_strength(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 strength);
/**
 * @brief  get rnr (raw denoise) strenght
 * @param  fd   the fd of ar_cam_src_open
 * @param  strength   the strength of rnr,  0---100, 0 is the min value, and 100 is max
 * @return  0 success !0 failed
 * @note   rnr only can be used when camera input size as isp output size same
 */
AR_S32 ar_hal_vin_rnr_get_strength(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *strength);
/**
 * @brief  set flip states
 * @param  fd   the fd of ar_cam_src_open
 * @param  state   the states of flip
 *         0  disable the flip
 *         1  enable the flip
 * @return  0 success !0 failed
 * @note   make sure the sensor drvier can supported flip operation
 */
AR_S32 ar_hal_vin_flip_set_state(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 state);
/**
 * @brief  get flip states
 * @param  fd   the fd of ar_cam_src_open
 * @param  state   the states of flip
 *         0  disable the flip
 *         1  enable the flip
 * @return  0 success !0 failed
 * @note   make sure the sensor drvier can supported flip operation
 */
AR_S32 ar_hal_vin_flip_get_state(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *p_state);
/**
 * @brief  set mirror  states
 * @param  fd   the fd of ar_cam_src_open
 * @param  state   the states of mirror
 *         0  disable the mirror
 *         1  enable the mirror
 * @return  0 success !0 failed
 * @note   make sure the sensor drvier can supported flip operation
 */
AR_S32 ar_hal_vin_mirror_set_state(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 state);
/**
 * @brief  get mirror  states
 * @param  fd   the fd of ar_cam_src_open
 * @param  state   the states of mirror
 *         0  disable the mirror
 *         1  enable the mirror
 * @return  0 success !0 failed
 * @note   make sure the sensor drvier can supported flip operation
 */
AR_S32 ar_hal_vin_mirror_get_state(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *p_state);
/**
 * @brief  get banding  states
 * @param  fd   the fd of ar_cam_src_open
 * @param  state  the states of banding
 * @code
 *	typedef enum
 *	{
 *	   CAM_PRA_ANTI_BANDING_OFF,
 *	   CAM_PRA_ANTI_BANDING_50,
 *	   CAM_PRA_ANTI_BANDING_60,
 *	}antibanding_t;
 * @endcode
 * @return  0 success !0 failed
 * @note  null
 **/
AR_S32 ar_hal_vin_banding_get_state(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *p_state);
/**
 * @brief  set banding  states
 * @param  fd   the fd of ar_cam_src_open
 * @param  state  the states of banding
 * @code
 *	typedef enum
 *	{
 *	   CAM_PRA_ANTI_BANDING_OFF,
 *	   CAM_PRA_ANTI_BANDING_50,
 *	   CAM_PRA_ANTI_BANDING_60,
 *	}antibanding_t;
 * @endcode
 * @return  0 success !0 failed
 * @note  null
 */
AR_S32 ar_hal_vin_banding_set_state(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 state);
/**
 * @brief   set the ircutter states
 * @param  fd  the fd of ar_cam_src_open
 * @param  state  The states
 *         0 off
 *         1 on
 * @return 0 success !0 failed
 * @note make sure the sensor drvier can on/off the ir cutter
 */
AR_S32 ar_hal_vin_ir_cutter_set_state(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_CAM_PRA_IR_CUTTER_PRA_T state);
AR_S32 ar_hal_vin_ir_cutter_get_state(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_CAM_PRA_IR_CUTTER_PRA_T *p_state);
/**
 * @brief  set the led/flash states
 * @param  fd  the fd of ar_cam_src_open
 * @param  state  The states
 *         0 off
 *         1 on
 * @return 0 success !0 failed
 * @note make sure the sensor drvier can on/off the ir led/flash
 */
AR_S32 ar_hal_vin_led_set_state(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_CAM_PRA_LED_PRA_T state);
AR_S32 ar_hal_vin_led_get_state(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_CAM_PRA_LED_PRA_T *p_state);
/**
 * @brief  set the aec stats roi
 *
 * @param  fd   the fd of ar_cam_src_open
 * @param  roi  The roi to exp stats
 * @code
 * //the roi use the 1 as max and 0 as min
 * //such as
 * {
 *    0.2,
 *    0.2,
 *    0.4,
 *    0.4,
 *  }
 *  //if the picture is 100x100
 *  //so the roi is
 *  //20,20, 40,40
 *  typedef struct
 *  {
 *	 AR_FLOAT x;
 *	 AR_FLOAT y;
 *	 AR_FLOAT width;
 *	 AR_FLOAT height;
 *  }STRU_CAM_ROI_T;
 * @endcode
 * @return 0 success !0 failed
 * @note   not suggesiton to use, it's a expert api
 */
AR_S32 ar_hal_vin_set_ae_roi(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_CAM_ROI_T *p_roi);
AR_S32 ar_hal_vin_set_awb_roi(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_CAM_ROI_T *p_roi);

/**
 * @brief  get the aec stats roi
 *
 * @param  fd   the fd of ar_cam_src_open
 * @param  roi  The roi to exp stats
 * @code
 * //the roi use the 1 as max and 0 as min
 * //such as
 * {
 *    0.2,
 *    0.2,
 *    0.4,
 *    0.4,
 *  }
 *  //if the picture is 100x100
 *  //so the roi is
 *  //20,20, 40,40
 *  typedef struct
 *  {
 *	 AR_FLOAT x;
 *	 AR_FLOAT y;
 *	 AR_FLOAT width;
 *	 AR_FLOAT height;
 *  }STRU_CAM_ROI_T;
 * @endcode
 * @return 0 success !0 failed
 * @note   not suggesiton to use, it's a expert api
 */

AR_S32 ar_hal_vin_get_ae_roi(AR_S32 fd_pipe, AR_S32 pipe_id,STRU_CAM_ROI_T *p_roi);
AR_S32 ar_hal_vin_get_awb_roi(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_CAM_ROI_T *p_roi);

/**
 * @brief set the fps range of sensor
 * @param  fd   the fd of ar_cam_src_open
 * @param  pra  The pra of fps info
 * @code
 *	typedef struct
 *	{
 *	   uint32_t fps_min;
 *	   uint32_t fps_max;
 *	}fps_info_t;
 * @endcode
 * @return  0 success !0 failed
 * @note it is a hw relation api. make sure your sensor support fps change or fps range set
 */
AR_S32 ar_hal_vin_set_fps_range(AR_S32 fd_pipe, AR_S32 pipe_id,STRU_FPS_INFO_T *p_pra);
AR_S32 ar_hal_vin_get_fps_range(AR_S32 fd_pipe, AR_S32 pipe_id,STRU_FPS_INFO_T *p_pra);
/**
 * @brief set semsce mode
 * @param[in]  fd  the fd of ar_cam_src_open
 * @param[in]  val   The sensce mode
 * @code
 *	typedef enum
 *	{
 *	   SENSCE_MODE_AUTO,
 *	   SENSCE_MODE_OFF,
 *	}sensce_mode_t;
 *
 *
 * @endcode
 * @return   0 success !0 failed
 * @note
 */
AR_S32 ar_hal_vin_set_scene_mode(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 mode);

/**
 * @brief get scene mode
 * @param[in]  fd  the fd of ar_cam_src_open
 * @param[in]  val   The scene mode
 * @code
 *	typedef enum
 *	{
 *	   SENSCE_MODE_AUTO,
 *	   SENSCE_MODE_OFF,
 *	}sensce_mode_t;
 *
 *
 * @endcode
 * @return   0 success !0 failed
 * @note
 */
AR_S32 ar_hal_vin_get_scene_mode(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *p_mode);


/**
 * @brief  set camera sensce
 * @param[in]  fd  the fd of ar_cam_src_open
 * @param[in]  pra the sensce info
 * @code
 *	typedef struct
 *	{
 *	   AR_CHAR tuning_name[128];
 *	}CAM_PRA_SET_SENCE_pra_t;
 * @endcode
 * @return    0 success !0 failed
 * @note      SENSCE_NORMOL_HDR_OFF can not used by this api, other sensce need tuning pra supported.
 */
AR_S32 ar_hal_vin_set_sensce(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_SENSE_INFO_T *p_pra);
/**
 * @brief  ar_hal_vin_get_sensce
 * @param[in]  fd  the fd of ar_cam_src_open
 * @param[in]  pra the scene info
 * @code
 *  typedef enum
 *	{
 *	   SENSCE_NORMOL,
 *	   SENSCE_NIGHT,
 *	   SENSCE_BACK_LIGNT,
 *	   SENSCE_PORTRAIT,
 *	   SENSCE_LANDSCAPE,
 *	   SENSCE_MOTION,
 *	   SENSCE_FOOD,
 *	   SENSCE_NORMOL_HDR_OFF,
 *	}sensce_t;
 **/
AR_S32 ar_hal_vin_get_sensce(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_SENSE_INFO_T *p_pra);
/**
 * @brief   set the exp limit of current camera
 * @param fd  the fd of ar_cam_src_open
 * @param val  the exp time (us) to be limited, if val <=0 ,the limit function will be disable
 * @return   0 success !0 failed
 * @note   not suggustion to use, if need first consider to modify aec tuning pra.
 */
AR_S32 ar_hal_vin_set_exp_limit(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 val);
AR_S32 ar_hal_vin_get_exp_limit(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *val);


AR_S32 ar_hal_vin_set_exp_policy(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_EXP_POLIOCY_T *p_val);
AR_S32 ar_hal_vin_get_exp_policy(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_EXP_POLIOCY_T *p_val);

AR_S32 ar_hal_vin_set_back_light_mode(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_BLACK_LIGHT_T *p_val);
AR_S32 ar_hal_vin_get_back_light_mode(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_BLACK_LIGHT_T *p_val);

/**
 * @brief  set face aec
 * @param  fd    the fd of ar_cam_src_open
 * @param      face_roi  The face roi
 * @code
 * typedef struct
 *  {
 *   AR_FLOAT x; //start normalization x point of roi
 *   AR_FLOAT y;  //start normalization y point of roi
 *   AR_FLOAT width; //width normalization of roi
 *   AR_FLOAT height; //height normalization of roi
 *  }STRU_CAM_ROI_T;
 *
 *  typedef struct
 *  {
 *     AR_S32 face_count;  //the fcae count the face algo give
 *     STRU_CAM_ROI_T face_roi[16]; //max 16 face, the info of all the face roi info
 *     AR_FLOAT face_weight;  //face weight
 *     AR_S32  face_keep_frame_count;  //after this frame , if no face is set , the face ae will close auto
 *     AR_S32  face_target; //face luma target
 *  }STRU_FACE_AEC_T;
 * @endcode
 * @return    0 success !0 failed
 * @note  the roi is normalization
 */
AR_S32 ar_hal_vin_set_face_aec(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_FACE_AEC_T *p_face_roi);

/**
 * @brief   on off the isp sub module
 * @param  fd    the fd of ar_cam_src_open
 * @param  isp_module  The isp module
 * @code
 * enum
 * {
 *   TUNING_ISP_CROP_RAW,
 *   TUNING_ISP_BLC,
 *   TUNING_ISP_HDR_MIX,
 *   TUNING_ISP_DIGITAL_GAIN_1,
 *   TUNING_ISP_CAMPANDER,
 *   TUNING_ISP_DIGITAL_GAIN_2,
 *   TUNING_ISP_DPC,
 *   TUNING_ISP_CAC,
 *   TUNING_ISP_ATA,
 *   TUNING_ISP_RNR,
 *   TUNING_ISP_DECOMPANDER,
 *   TUNING_ISP_LSC,
 *   TUNING_ISP_AWB,
 *   TUNING_ISP_DRC,
 *   TUNING_ISP_GIC,
 *   TUNING_ISP_CFA,
 *   TUNING_ISP_DPP,
 *   TUNING_ISP_CCM1,
 *   TUNING_ISP_CCM2,
 *   TUNING_ISP_GTM1_LUT,
 *   TUNING_ISP_GAMMA,
 *   TUNING_ISP_GTM2_LUT,
 *   TUNING_ISP_3DLUT,
 *   TUNING_ISP_RGB2YUV,
 *   TUNING_ISP_CM,
 *   TUNING_ISP_CNF,
 *   TUNING_ISP_LNR_LEE,
 *   TUNING_ISP_DITHER,
 *   TUNING_ISP_DE3D,
 *   TUNING_ISP_ALGO_AEC,
 *   TUNING_ISP_ALGO_AWB,
 *   TUNING_ISP_ALGO_AF,
 *   TUNING_ISP_VFE,
 *   TUNING_ISP_SUBMODULE_MAX,
 * };
 * @endcode
 * @param  enable   0 disable 1 enable
 * @return    0 success !0 failed
 * @note please ref tuning doc to use the isp module
 */
AR_S32 ar_hal_vin_set_isp_submodule_state(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 isp_module, AR_S32 enable);

/**
 * @brief   on off the isp sub module
 * @param  fd    the fd of ar_cam_src_open
 * @param  isp_module  The isp module
 * @code
 * enum
 * {
 *   TUNING_ISP_CROP_RAW,
 *   TUNING_ISP_BLC,
 *   TUNING_ISP_HDR_MIX,
 *   TUNING_ISP_DIGITAL_GAIN_1,
 *   TUNING_ISP_CAMPANDER,
 *   TUNING_ISP_DIGITAL_GAIN_2,
 *   TUNING_ISP_DPC,
 *   TUNING_ISP_CAC,
 *   TUNING_ISP_ATA,
 *   TUNING_ISP_RNR,
 *   TUNING_ISP_DECOMPANDER,
 *   TUNING_ISP_LSC,
 *   TUNING_ISP_AWB,
 *   TUNING_ISP_DRC,
 *   TUNING_ISP_GIC,
 *   TUNING_ISP_CFA,
 *   TUNING_ISP_DPP,
 *   TUNING_ISP_CCM1,
 *   TUNING_ISP_CCM2,
 *   TUNING_ISP_GTM1_LUT,
 *   TUNING_ISP_GAMMA,
 *   TUNING_ISP_GTM2_LUT,
 *   TUNING_ISP_3DLUT,
 *   TUNING_ISP_RGB2YUV,
 *   TUNING_ISP_CM,
 *   TUNING_ISP_CNF,
 *   TUNING_ISP_LNR_LEE,
 *   TUNING_ISP_DITHER,
 *   TUNING_ISP_DE3D,
 *   TUNING_ISP_ALGO_AEC,
 *   TUNING_ISP_ALGO_AWB,
 *   TUNING_ISP_ALGO_AF,
 *   TUNING_ISP_VFE,
 *   TUNING_ISP_SUBMODULE_MAX,
 * };
 * @endcode
 * @param  enable   0 disable 1 enable
 * @return    0 success !0 failed
 * @note please ref tuning doc to use the isp module
 */

AR_S32 ar_hal_vin_get_isp_submodule_state(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 isp_module, AR_S32 *b_enable);

/**
 * @brief  set the isp drc pra
 * @param  fd  the fd of ar_cam_src_open
 * @param  mode  The mod  0 audo, 1 manu, when mode is manu, gain k1 k2 k3 will be used
 * @param  gain  The drc gain
 * @param  k1    The drc pra k1
 * @param  k2    The drc pra k2
 * @param  k3    the drc pra k3
 * @return    0 success !0 failed
 * @note please ref tuning doc to use the drc, this api only is valide when tuning pra drc is enable
 */
AR_S32 ar_hal_vin_set_drc(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 strength);
/**
 * @brief  get the isp drc pra
 * @param  fd  the fd of ar_cam_src_open
 * @param  mode  The mod  0 audo, 1 manu, when mode is manu, gain k1 k2 k3 will be used
 * @param  gain  The drc gain
 * @param  k1    The drc pra k1
 * @param  k2    The drc pra k2
 * @param  k3    the drc pra k3
 * @return    0 success !0 failed
 * @note please ref tuning doc to use the drc, this api only is valide when tuning pra drc is enable
 */
AR_S32 ar_hal_vin_get_drc(AR_S32 fd_pipe, AR_S32 pipe_id, AR_S32 *strength);
/**
 * @brief  ar_hal_vin_set_hdr_status
 * @param  fd  the fd of ar_cam_src_open
 * @param  val   0 fusion(hdr) 1:long  2 middle 3 short 4 auto
 * @return   0 success !0 failed
 * @note null
 */
AR_S32 ar_hal_vin_set_hdr_status(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 val);
/**
 * @brief  ar_hal_vin_get_hdr_status
 * @param  fd  the fd of ar_cam_src_open
 * @param  0 fusion(hdr) 1:long  2 middle 3 short 4 auto
 * @return   0 success !0 failed
 * @note null
 */
AR_S32 ar_hal_vin_get_hdr_status(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *p_val);

/**
 * @brief  get hdr auto detected
 * @param  fd  the fd of ar_cam_src_open
 * @param  val   0 off 1 on
 * @return 0 success !0 failed
 * @note null
*/
AR_S32 ar_hal_vin_set_zoom(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_ZOOM_PRA_T *p_zoom_cfg);
AR_S32 ar_hal_vin_get_zoom(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_ZOOM_PRA_T *p_zoom_cfg);

/**
 * @brief  ar_cam_set_csc,set csc setting to isp rgb2yuv module
 * @param  fd  the fd of ar_cam_src_open
 * @param  csc_pra_t
 * @code
 *enum
 *{
 *     CAM_BT709_TYPE,
 *     CAM_BT601_TYPE,
 *};
 *typedef struct
 *{
 *   AR_S32 type;
 *   AR_S32 full_range; //0 not full range, 1 full range
 *}CAM_PRA_CSC_pra_t;

 * @endcode
 * @return   0 success !0 failed
 * @note
 */
AR_S32 ar_hal_vin_set_csc(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_CSC_PRA_T *p_csc);
/**
 * @brief  ar_cam_get_csc,get csc setting from isp rgb2yuv module
 * @param  fd  the fd of ar_cam_src_open
 * @param  csc_pra_t
 * @code
 *enum
 *{
 *     CAM_BT709_TYPE,
 *     CAM_BT601_TYPE,
 *};
 *typedef struct
 *{
 *   AR_S32 type;
 *   AR_S32 full_range; //0 not full range, 1 full range
 *}CAM_PRA_CSC_pra_t;

 * @endcode
 * @return   0 success !0 failed
 * @note
 */

AR_S32 ar_hal_vin_get_csc(AR_S32 fd_pipe,AR_S32 pipe_id, STRU_CSC_PRA_T *p_csc);

/**
 * @brief  ar_cam_de3d_get_pra,get 3d denoise pra
 * @param  fd  the fd of ar_cam_src_open
 * @param  denoise_2d_pra_t
 * @code
 * enum
 *{
 *   DENOISE_MODE_AUTO,
 *   DENOISE_MODE_MANUAL,
 *   DENOISE_MODE_EXPERT,
 *};
 * typedef struct
 *{
 *    STRU_ISP_SUB_MODULE_RNR_LAY lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT];
 *}denoise_2d_rnr_expert_pra_t;
 *
 *typedef struct
 *{
 *  AR_S32 zoom_par;
 *  AR_S32 dn_level;
 *}denoise_2d_cnf_expert_pra_t;
 *
 *typedef struct
 *{
 *  denoise_2d_cnf_expert_pra_t denoise_2d_cnf_expert_pra;
 *  denoise_2d_rnr_expert_pra_t denoise_2d_rnr_expert_pra;
 *}denoise_2d_expert_pra_t;
 *
 *enum{
 *	NOISE_TYPE_LUMA,
 *	NOISE_TYPE_CHROMA,
 *};
 *
 *typedef struct
 *{
 *  AR_S32 denoise_type;
 *  AR_S32 enable;
 *  AR_S32 mode;
 *  AR_S32 strength;
 *  denoise_2d_expert_pra_t denoise_2d_expert_pra;
 *}CAM_PRA_DENOISE_2D_pra_t;
 * @endcode
 * @return	 0 success !0 failed
 * @note
 */
AR_S32 ar_hal_vin_de2d_get_strength(AR_S32 fd_pipe, AR_S32 pipe_id,STRU_DENOISE_2D_PRA_T  *p_pra);
/**
 * @brief  ar_cam_de2d_get_pra,get 2d denoise pra
 * @param  fd  the fd of ar_cam_src_open
 * @param  denoise_2d_pra_t
 * @code
 * enum
 *{
 *   DENOISE_MODE_AUTO,
 *   DENOISE_MODE_MANUAL,
 *   DENOISE_MODE_EXPERT,
 *};
 * typedef struct
 *{
 *    STRU_ISP_SUB_MODULE_RNR_LAY lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT];
 *}denoise_2d_rnr_expert_pra_t;
 *
 *typedef struct
 *{
 *  AR_S32 zoom_par;
 *  AR_S32 dn_level;
 *}denoise_2d_cnf_expert_pra_t;
 *
 *typedef struct
 *{
 *  denoise_2d_cnf_expert_pra_t denoise_2d_cnf_expert_pra;
 *  denoise_2d_rnr_expert_pra_t denoise_2d_rnr_expert_pra;
 *}denoise_2d_expert_pra_t;
 *
 *enum{
 *	NOISE_TYPE_LUMA,
 *	NOISE_TYPE_CHROMA,
 *};
 *
 *typedef struct
 *{
 *  AR_S32 denoise_type;
 *  AR_S32 enable;
 *  AR_S32 mode;
 *  AR_S32 strength;
 *  denoise_2d_expert_pra_t denoise_2d_expert_pra;
 *}CAM_PRA_DENOISE_2D_pra_t;
 * @endcode
 * @return	 0 success !0 failed
 * @note
 */
AR_S32 ar_hal_vin_de2d_set_strength(AR_S32 fd_pipe, AR_S32 pipe_id,STRU_DENOISE_2D_PRA_T *p_pra);

/**
 * @brief  get cam sensor stats
 * @param  cam_id  the cam index
 * @param  stats: the stats of the cam sensor
 * @return 0 success !0 failed
 * @note null
 */
AR_S32 ar_hal_vin_src_get_sensor_stats(AR_S32 fd_pipe,AR_S32 pipe_id, AR_S32 *p_stats);

AR_S32 ar_hal_vin_open_dev_mem();
AR_VOID ar_hal_vin_close_dev_mem(AR_S32 fd_mm);

AR_S32 ar_hal_vin_get_pipe_limit_info(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_CAM_PRA_GET_LIMIT_INFO_PRA_T *p_info);

AR_S32 ar_hal_vin_get_pipe_raw(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_CAM_PRA_GET_PIPE_BUFFER_T *p_raw);

AR_S32 ar_hal_vin_get_pipe_run_info(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_CAM_PRA_GET_PIPE_RUN_INFO_PRA_T *p_info);

AR_S32 ar_hal_vin_get_raw_format(STRU_SENSOR_OUT_INFOR_T *p_out_infor,AR_S32 *p_bayer_format);
AR_S32 ar_hal_vin_bayer_format_to_dng_format(AR_S32 bayer_format,AR_S32 *p_dng_format);

AR_S32 ar_hal_vin_set_ir_nuc_param(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_SET_IR_NUC_PARAM_PRA_T *p_param);
AR_S32 ar_hal_vin_get_ir_nuc_param(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_SET_IR_NUC_PARAM_PRA_T *p_param);
AR_S32 ar_hal_vin_set_ir_qgg_param(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_SET_IR_QGG_PARAM_PRA_T *p_param);
AR_S32 ar_hal_vin_get_ir_qgg_param(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_SET_IR_QGG_PARAM_PRA_T *p_param);
AR_S32 ar_hal_vin_set_ir_gtm_param(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_SET_IR_GTM_PARAM_PRA_T *p_param);
AR_S32 ar_hal_vin_get_ir_gtm_param(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_SET_IR_GTM_PARAM_PRA_T *p_param);
AR_S32 ar_hal_vin_ir_algo_process(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_IR_ALGO_PROCESS_PRA_T *pra);

/*
*@brief: ar_hal_vin_open_nuc_dev, open nuc device
*@param: none
*@code
*@endcode
*@return:
        >= 0: nuc device fd
        < 0: fail
*@note:
*/
AR_S32 ar_hal_vin_open_nuc_dev(void);

/*
*@brief: ar_hal_vin_close_nuc_dev, close nuc device
*@param:
        fd_nuc: nuc device fd
*@code
*@endcode
*@return: none
*@note:
*/
AR_VOID ar_hal_vin_close_nuc_dev(AR_S32 fd_nuc);

/*
*@brief: ar_hal_vin_nuc_set_out_clk, set nuc output clock
*@param:
        fd_nuc: nuc device fd
        clk_src: nuc output clock source
        clk_div: nuc output clock divider
        clk_phase: nuc output clock clock phase
        clk_en: whether enable or disable nuc output clock
*@code
*@endcode
*@return:
        0: success
        other: fail
*@note:
*/
AR_S32 ar_hal_vin_nuc_set_out_clk(AR_S32 fd_nuc, ENUM_NUC_OUT_CLK_SRC_T clk_src,
    AR_U8 clk_div, ENUM_NUC_CLK_PHASE_T clk_phase, AR_BOOL clk_en);

/*
*@brief: ar_hal_vin_nuc_cfg, config nuc
*@param:
        fd_nuc: nuc device fd
        nuc_cfg: nuc configuration information
*@code
*@endcode
*@return:
        0: success
        other: fail
*@note:
*/
AR_S32 ar_hal_vin_nuc_cfg(AR_S32 fd_nuc, STRU_NUC_CFG_T *nuc_cfg);

/*
*@brief: ar_hal_vin_nuc_start, start nuc sending occ data to sensor
*@param:
        fd_nuc: nuc device fd
*@code
*@endcode
*@return:
        0: success
        other: fail
*@note:
*/
AR_S32 ar_hal_vin_nuc_start(AR_S32 fd_nuc);

/*
*@brief: ar_hal_vin_nuc_stop, stop nuc sending occ data to sensor
*@param:
        fd_nuc: nuc device fd
*@code
*@endcode
*@return:
        0: success
        other: fail
*@note:
*/
AR_S32 ar_hal_vin_nuc_stop(AR_S32 fd_nuc);

/*
*@brief: ar_hal_vin_nuc_cmd, send nuc cmd to sensor
*@param:
        fd_nuc: nuc device fd
        nuc_cmd: nuc cmd code
        data: nuc cmd data
        size: nuc cmd data size
*@code
*@endcode
*@return:
        0: success
        other: fail
*@note:
*/
AR_S32 ar_hal_vin_nuc_cmd(AR_S32 fd_nuc, ENUM_NUC_CMD_T nuc_cmd, AR_VOID *data, AR_S32 size);

/*
*@brief: ar_hal_vin_nuc_get_occ, get current occ data
*@param:
        fd_nuc: nuc device fd
        occ_buf: occ buffer
*@code
*@endcode
*@return:
        0: success
        other: fail
*@note:
*/
AR_S32 ar_hal_vin_nuc_get_occ(AR_S32 fd_nuc, STRU_NUC_OCC_PARAM_T *p_occ_param);

/*
*@brief: ar_hal_vin_nuc_set_occ, set occ data
*@param:
        fd_nuc: nuc device fd
        occ_buf: occ buffer
*@code
*@endcode
*@return:
        0: success
        other: fail
*@note:
*/
AR_S32 ar_hal_vin_nuc_set_occ(AR_S32 fd_nuc, STRU_NUC_OCC_PARAM_T *p_occ_param);

AR_S32 ar_hal_vin_get_ir_sns_attr(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_IR_SNS_ATTR_T *p_attr);

AR_S32 ar_hal_vin_start_ir_sns_calib(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_IR_SNS_CALIB_CFG_T *p_calib_cfg);
AR_S32 ar_hal_vin_get_ir_sns_calib_result(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_IR_SNS_CALIB_RESULT_T *p_calib_result);
AR_S32 ar_hal_vin_cancel_ir_sns_calib(AR_S32 fd_pipe, AR_S32 pipe_id);

AR_S32 ar_hal_vin_start_ir_occ_calib(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_IR_OCC_CALIB_CFG_T *p_calib_cfg);
AR_S32 ar_hal_vin_get_ir_occ_calib_result(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_IR_OCC_CALIB_RESULT_T *p_calib_result);
AR_S32 ar_hal_vin_cancel_ir_occ_calib(AR_S32 fd_pipe, AR_S32 pipe_id);

AR_S32 ar_hal_vin_start_ir_b_calib(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_IR_B_CALIB_CFG_T *p_calib_cfg);
AR_S32 ar_hal_vin_get_ir_b_calib_result(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_IR_B_CALIB_RESULT_T *p_calib_result);
AR_S32 ar_hal_vin_cancel_ir_b_calib(AR_S32 fd_pipe, AR_S32 pipe_id);

AR_S32 ar_hal_vin_ir_kb_calib(AR_S32 fd_pipe, AR_S32 pipe_id, STRU_CAM_PRA_IR_KB_CALIB_CTX_T *p_calib_ctx);

AR_S32 ar_hal_vin_set_sensor_otp(AR_S32 fd_pipe,AR_S32 pipe_id,STRU_OTP_CALLIBRATION_DATA_T *pra);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif
