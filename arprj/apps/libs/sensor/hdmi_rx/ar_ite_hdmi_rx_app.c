//#define ENABALE_ALL_LOG

#include "ar_ite_hdmi_rx_app.h"
#include "it_66021.h"
#include "it6602.h"
#if !defined (__linux__)
#include "reg_access.h"
#endif
#include "osal.h"
//#include "pin_share.h"
#include "hal_gpio.h"
#include "hal_vin_log.h"
#define HDMI_RX_ITE_INIT_PIN	(66)
static unsigned int g_hdmi_rx_ite_init_pin = 0xffff;
//static osMessageQueueId_t g_hdmirx_ite_msg_queue; no use ????
#if 1
uint8_t ar_ite_hdmi_rx_config(void)
{
	unsigned int i2c_setting[16];
	unsigned int g_gpio_setting[5];
    int gpio_num = 0;
	//unsigned int intr_setting[5];
#if defined(AR9341)
    g_gpio_setting[0] = 94;  /* pad */
    g_gpio_setting[1] = 0;    /* fuction */
    g_gpio_setting[2] = 3;    /* group */
    g_gpio_setting[3] = 0;    /* port */
    g_gpio_setting[4] = 0;    /* pin */
    
    i2c_setting[0] = 0x92;
    i2c_setting[1] = 0;
#endif
#if defined(PROXIMA)
    g_gpio_setting[0] = 67;  /* pad */
    g_gpio_setting[1] = 0;    /* fuction */
    g_gpio_setting[2] = 0;    /* group */
    g_gpio_setting[3] = 2;    /* port */
    g_gpio_setting[4] = 22;    /* pin */
    
    i2c_setting[0] = 0x92;
    i2c_setting[1] = 2;
#endif
    gpio_num = ar_hal_gpio_name_to_num(g_gpio_setting[2], g_gpio_setting[3], g_gpio_setting[4]);

    ar_hal_gpio_export(gpio_num);
    ar_hal_gpio_set_dir(gpio_num, 1);
    ar_hal_gpio_set_value(gpio_num, 0);
    ar_delay(100);
    ar_hal_gpio_set_value(gpio_num, 1);
    ar_always("reset hdmi rx done gpio is %d-%d-%d num:%d\n",g_gpio_setting[2],g_gpio_setting[3],g_gpio_setting[4],gpio_num);
    ar_delay(30);
    
    ar_always("i2c host %d slave addr=%x",i2c_setting[1],i2c_setting[0]);
    int ret = HDMIRX_i2c_init(i2c_setting[1], i2c_setting[0]);
	if (ret != 0) {
		ar_always("hdmi rx use i2c-%d chip addr=0x%x failed", i2c_setting[1], i2c_setting[0]);
        return 1;
	} else {
		ar_always("hdmi rx init ok");
		return 0;
	}
    return 0;
}
#else
uint8_t ar_ite_hdmi_rx_config(void)
{
	int ret = 0;
	int i = 0;
	int count;
	int offset;
	unsigned int i2c_setting[16];
	unsigned int gpio_setting[5];
	unsigned int intr_setting[5];
	//unsigned int colormode_setting[3] = {0 ,0, 0};
	void *blob = board_fdt_blob_setup();
	offset = fdt_node_offset_by_compatible(blob, 0, "hdmi_rx_cfg");
	if (offset != -1) {
        ar_always("get hdmi rx i2c node ok blob=%p\n", blob);
		count = fdtdec_get_int_array_count(blob, offset, "i2c_set", i2c_setting, 16);
		if (!count) {
			ar_err("get hdmi rx i2c_set fail");
			ar_osal_thread_exit();
		}
#if 1
		ret = fdtdec_get_int_array(blob, offset, "reset_pin", gpio_setting, 5);
		if (ret) {
			ar_info("get hdmi rx reset_pin fail");
		} else {
		    ret = fdtdec_get_int_array(blob, offset, "reset_pin", gpio_setting, 5);
    		if (ret) {
    			ar_err("get hdmi tx reset_pin fail");
    			ar_osal_thread_exit();
    		} else {
    			gpio_set_direct(gpio_setting[2],gpio_setting[3], gpio_setting[4], GPIO_DIR_OUTPUT);
    			gpio_set_val(gpio_setting[2],gpio_setting[3], gpio_setting[4], GPIO_DATA_LOW);
    			ar_delay(100);
    			gpio_set_val(gpio_setting[2],gpio_setting[3], gpio_setting[4], GPIO_DATA_HIGH);
    			ar_always("reset hdmi rx done gpio is %d-%d-%d\n",gpio_setting[2],gpio_setting[3],gpio_setting[4]);
    		}

		}
#else
        AR_SET_REG_BITS(__REG32__(0x789001c),1,1,1);
#endif
		ret = fdtdec_get_int_array_count(blob, offset, "intr_pin", intr_setting, 5);
		if (ret <= 0) {
			ar_always("!!!!!!!!!!!!!!!!!!!!get hdmi rx interrput pin fail %d\n", intr_setting[0]);
		} else {
			g_hdmi_rx_ite_init_pin = intr_setting[0];
			ar_always("!!!!!!!!!!!!!!!!!!!interrput pin=%d %d", g_hdmi_rx_ite_init_pin);
		}

	} else {
		ar_always("get hdmi rx i2c node fail blob=%p\n", blob);
		return 1;
	}

	for(i = count-1; i != 0; i--) {
		if (0 == HDMIRX_i2c_init(i2c_setting[i], i2c_setting[0]))
			break;
	}

	if (i != 0) {
		ar_always("hdmi rx use i2c-%d chip addr=0x%x", i2c_setting[i], i2c_setting[0]);
	} else {
		ar_always("hdmi rx init error");
		return 1;
	}

	return 0;
}
#endif
struct hdmi_rx_device_operations ite_hdmi_rx_ops = {
	.check_av_state = hdmi_rx_66021_get_avstate,
	.get_vformat = hdmi_rx_66021_get_vformat,
	.get_aformat = hdmi_rx_66021_get_aformat,
};


struct ar_hdmi_rx_driver ite_hdmi_rx_driver = {
		.name = HDMI_RX_ITE,
		.ops = &ite_hdmi_rx_ops,
};

static struct _ar_hdmi_rx ar_ite_hdmi_rx = {
	.hdmi_rx_device_driver = {
		.drv = &ite_hdmi_rx_driver,
	}
};


void hdmi_rx_66021_intr_handle(uint8_t pin,uint32_t group)
{
	#if 0 //is not used ???
	uint32_t msg = 1;
	gpio_close_interrupt(g_hdmi_rx_ite_init_pin);
	osMessageQueuePut(g_hdmirx_ite_msg_queue, &msg, 0, 0);
	#endif
	ar_always("hdmi rx ite recive interrput");
}

void * ite_hdmi_rx_app(void *param)
{
#define	FOR_TIME	(50)
	uint8_t i = FOR_TIME;
	int status = HDMI_RX_UNSTABLE;
	int statustmp = HDMI_RX_UNSTABLE;
#if 0
    gpio_set_input(g_hdmi_rx_ite_init_pin);
#endif
    if (ar_ite_hdmi_rx_config()) {
		ar_osal_thread_exit();
	}
	else
	{

		HDMIRX_Initial();

		if (0xffff == g_hdmi_rx_ite_init_pin)
		{
			while(1) {
				statustmp = IT_66021_app();
				if (status != statustmp) {
					status = statustmp;
					if (statustmp == HDMI_RX_VIDEO_O) {
						i = FOR_TIME;
						while((i>0) &&(statustmp == HDMI_RX_VIDEO_O)) {
							statustmp = IT_66021_app();
							if (status != statustmp) {
								status = statustmp;
							}
							i--;
                            ar_delay(10); // total delay = 10*FOR_TIME ms, in video only mode
						}
                        ar_ite_hdmi_rx.hdmi_rx_notify->notify_format(ar_ite_hdmi_rx.hdmi_rx_notify);
					} else {
						ar_ite_hdmi_rx.hdmi_rx_notify->notify_format(ar_ite_hdmi_rx.hdmi_rx_notify);
					}
				}
				ar_delay(50);
			}
		}
		else {

			while (1) {
			    #ifndef FPGA_SIMULATION_9301
				//if (!gpio_getpin(g_hdmi_rx_ite_init_pin))
                {
					IT6602_Interrupt();
					//ar_always("hdmi rx ite recive interrput %d", gpio_getpin(g_hdmi_rx_ite_init_pin));
				}
				#endif
		    	IT6602_fsm();
		    	statustmp = hdmi_rx_66021_get_avstate();
		    	if (status != statustmp) {
					status = statustmp;
					ar_ite_hdmi_rx.hdmi_rx_notify->notify_format(ar_ite_hdmi_rx.hdmi_rx_notify);
				}
				//ar_always("hdmi rx ite recive interrput %d", gpio_getpin(g_hdmi_rx_ite_init_pin));
				ar_delay(50);
			}
		}

	}

}
void ar_ite_hdmi_rx_init(void* arg)
{
    ar_always("init hdmi rx %s %s\n",__DATE__,__TIME__);
	ar_hdmi_rx_register_driver(&ite_hdmi_rx_driver);
	ar_hdmi_rx_register_notify(HDMI_RX_ITE, &ar_ite_hdmi_rx.hdmi_rx_notify);
	#if 0
	g_hdmirx_ite_msg_queue = osMessageQueueNew(16, sizeof(uint32_t), NULL);
	#endif

	ar_os_thread_attr_t attr;
	memset(&attr,0,sizeof(ar_os_thread_attr_t));
	attr.priority = osPriorityHigh;
	attr.name = "ite_hdmi_rx";
	if ((ar_os_thread_id_t)NULL == ar_osal_thread_new(ite_hdmi_rx_app, NULL, &attr)) {
		ar_always("create hdmi_rx_st_handler_run app error\n");
	}

}


