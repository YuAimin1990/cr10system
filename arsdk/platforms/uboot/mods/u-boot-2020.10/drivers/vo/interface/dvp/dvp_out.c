#include "vo/interface/display_interface.h"

#ifdef CONFIG_ARTOSYN_AR9301
#define VIDEO_IF_BASE_ADDRESS 0x01100000
#define DVP_CTRL_BASE_ADDR    0x010f0000
#define DVP_OUT_16_OFFSET_REG (VIDEO_IF_BASE_ADDRESS+0x00a4)
#define DVP_GROUP_CTRL_ADDR             (DVP_CTRL_BASE_ADDR+0x7c)
#elif defined(CONFIG_ARTOSYN_AR9311)
#define VO_CTRL_BASE_ADDR     0x8810000
#define DVP_OUT_16_OFFSET_REG     (VO_CTRL_BASE_ADDR+0x1f70)
#else
#error("vo doesn't support this chipid");
#endif

// disp
#define CGU_CTRL_BASE_ADDR 0x01070000
#define CLK_PIXEL_DISP_SEL_SHIFT	15
#define CLK_PIXEL_DISP_SEL_MASK 	(1 << CLK_PIXEL_DISP_SEL_SHIFT)

#define DVP_OUT_CSC_CFG_FILE ("/usrdata/csc_dvp_out.json")

extern display_intgerface_ops_t dvp_dev_vga;
extern display_intgerface_ops_t dvp_dev_hdmi;
extern display_intgerface_ops_t dvp_dev_hx8264;
extern display_intgerface_ops_t dvp_dev_nvp60621;
extern display_intgerface_ops_t dvp_dev_tp2803;
typedef enum
{
    CLK_PHASE_DEFAULT,                  /**<@note     default值*/
    CLK_PHASE0,                         /**<@note     VO的时钟相位为0度*/
    CLK_PHASE90,                        /**<@note     VO的时钟相位为90度，ar9341不支持*/
    CLK_PHASE180,                       /**<@note     VO的时钟相位为180度*/
    CLK_PHASE270,                       /**<@note     VO的时钟相位为270度, ar9341不支持*/
} CLK_PHASE_E;
extern display_intgerface_ops_t dvp_dev_tc3587;

static int intf_index = DVP_DEV_HDMI;

static display_intgerface_ops_t *dvp_dev_ops[]=
{
//	&dvp_dev_vga,
//	&dvp_dev_hdmi,
//	&dvp_dev_nvp60621,
//	&dvp_dev_hx8264,
//	&dvp_dev_tp2803,
//	&dvp_dev_tc3587,
};
static void init_dvp_port(int out_color_mode, ENUM_AR_HAL_PIN_MODE pin_mode, int is_bt656, int clock_polarity;);
void dvp_rgb2yuvcoeff_custom(dvp_csc_pra_t *csc);
static void dvp_out_bprgb2yuvcoeff_config(int mode);

static int get_current_dvp_dev_index(void)
{
    return intf_index;
}

static int set_dvp_interface_index(int index)
{
    ar_always("index=%d", index);
    intf_index = index;
    return 0;
}


static int get_current_dvp_dev_sync_mode(void)
{
    return SYNC_MODE_EXTERNAL;
}

static int get_current_dvp_pad_line_cfg(void)
{
    return -1;
}

static int dvp_out_check_csccfg_custom(char *filename,  char **json)
{
    return -1;
}

int dvp_out_get_json_csccfg(char *content, dvp_csc_pra_t *csc)
{
    return -1;
}

static unsigned int get_dvp_out_clock_phase(void)
{
    return 0;
}

static display_intgerface_ops_t *get_dev_ops(void)
{
    int dev_index=get_current_dvp_dev_index();
	display_intgerface_ops_t *dev_ops=NULL;
	int i=0;

	for(i=0;i<sizeof(dvp_dev_ops)/sizeof(display_intgerface_ops_t *);i++)
	{
	    dev_ops=dvp_dev_ops[i];
	    if(dev_index==dev_ops->interface_index)
	    {
              return dev_ops;
	    }
	}

    ar_err("err intf index=%d", intf_index);
	return NULL;
}

static void dvp_out_base_init(void)
{
#ifdef CONFIG_ARTOSYN_AR9301
    AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR), 0, 22, 23);

    write_reg32(DVP_CTRL_BASE_ADDR + 0x90, 0x0);  // set dvp portC as output

    uint32_t    sel=0;
    unsigned int video_clk_ctrl = read_reg32(CGU_CTRL_BASE_ADDR);
    ar_always("video_clk_ctrl:%02x", video_clk_ctrl);
    sel = (video_clk_ctrl & CLK_PIXEL_DISP_SEL_MASK) >> CLK_PIXEL_DISP_SEL_SHIFT;

    if (sel == 1)
    {
        ar_always("1x dvp out clk.");
        AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR), 0,15,15); // set dvp 1x pixel clock
    }
    else
    {
        ar_always("2x dvp out clk.");
        AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR), 1,15,15); // set dvp 2x pixel clock
    }

    ar_always("reset the dvp port");

    AR_SET_REG_BITS(__REG32__(VIDEO_IF_BASE_ADDRESS + (0x3c<<2)),0,17,17);       /*dvp out reset */
    ar_delay(1);
    AR_SET_REG_BITS(__REG32__(VIDEO_IF_BASE_ADDRESS + (0x3c<<2)),1,17,17);
    AR_SET_REG_BITS(__REG32__(VIDEO_IF_BASE_ADDRESS + (0xaf<<2)),0,0,0);        /* make sure vif is power on*/
#endif
}

static void init_dvp_port(display_interface_desc_t *dvp_desc)
{
    if(!dvp_desc)
    {
        ar_err("dvp_desc is null!");
        return ;
    }

    int out_color_mode = dvp_desc->out_color_mode;
    ENUM_AR_HAL_PIN_MODE pin_mode = dvp_desc->pin_mode;
    int is_bt656 = dvp_desc->is_bt656;
    int clock_polarity = dvp_desc->clock_polarity;

    int hsync_polarity = dvp_desc->hsync_polarity;
    int vsync_polarity = dvp_desc->vsync_polarity;
    int de_polarity = dvp_desc->de_polarity;

    int mode=0;
    unsigned int value=0;

    ar_always("out_color_mode:%d pin_mode:%d is_bt656:%d clock_polarity:%d.", out_color_mode, pin_mode, is_bt656, clock_polarity);

     dvp_out_base_init();

    //dvpo
    AR_SET_REG_BITS(value, 1, 8, 11);   // unknow
    AR_SET_REG_BITS(value, 1, 0, 0);   /* enable dvp */

    if(out_color_mode==COLOR_MODE_RGB565)
    {
        ar_always("COLOR_MODE_RGB565");
        AR_SET_REG_BITS(value, 1, 3, 3);   /* enable lcd mode */
      #if defined(CONFIG_ARTOSYN_AR9301)
        AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR), 0, 20, 25);
      #endif
        if ((pin_mode.rgb565 == AR_HAL_PIN_MODE_RGB565_DEFAULT) || (pin_mode.rgb565 == AR_HAL_PIN_MODE_RGB565_BGR))
        {
        #if defined(CONFIG_ARTOSYN_AR9301)
            AR_SET_REG_BITS(value, 1, 12, 13);
            AR_SET_REG_BITS(value, 2, 14, 15);
        #elif defined(CONFIG_ARTOSYN_AR9311)
            AR_SET_REG_BITS(value, 2, 12, 13);
            AR_SET_REG_BITS(value, 2, 14, 15);
        #endif
        }
        else if (pin_mode.rgb565 == AR_HAL_PIN_MODE_RGB565_GRB)
        {
        #if defined(CONFIG_ARTOSYN_AR9301)
            AR_SET_REG_BITS(value, 2, 12, 13);
            AR_SET_REG_BITS(value, 0, 14, 15);
        #elif defined(CONFIG_ARTOSYN_AR9311)
            AR_SET_REG_BITS(value, 0, 12, 13);
            AR_SET_REG_BITS(value, 1, 14, 15);
        #endif
        }
        else
            ar_err("err rgb565 pin mode %d", pin_mode.rgb565);
    }
    else if (out_color_mode==COLOR_MODE_RGB888)
    {
    #if defined(CONFIG_ARTOSYN_AR9301)
        ar_always("COLOR_MODE_RGB888");
        AR_SET_REG_BITS(value, 1, 3, 3);   /* enable lcd mode */
        switch (pin_mode.rgb888)
        {
            case AR_HAL_PIN_MODE_RGB888_DEFAULT:
            case AR_HAL_PIN_MODE_RGB888_RGB:
                AR_SET_REG_BITS(value, 1, 12, 13);
                AR_SET_REG_BITS(value, 1, 14, 15);
                AR_SET_REG_BITS(value, 0, 16, 17);
                break;

            case AR_HAL_PIN_MODE_RGB888_GRB:
                AR_SET_REG_BITS(value, 0, 12, 13);
                AR_SET_REG_BITS(value, 0, 14, 15);
                AR_SET_REG_BITS(value, 0, 16, 17);
                break;

            case AR_HAL_PIN_MODE_RGB888_BRG:
                AR_SET_REG_BITS(value, 0, 12, 13);
                AR_SET_REG_BITS(value, 2, 14, 15);
                AR_SET_REG_BITS(value, 2, 16, 17);
                break;

            case AR_HAL_PIN_MODE_RGB888_RBG:
                AR_SET_REG_BITS(value, 2, 12, 13);
                AR_SET_REG_BITS(value, 1, 14, 15);
                AR_SET_REG_BITS(value, 2, 16, 17);
                break;

            case AR_HAL_PIN_MODE_RGB888_BGR:
                AR_SET_REG_BITS(value, 1, 12, 13);
                AR_SET_REG_BITS(value, 2, 14, 15);
                AR_SET_REG_BITS(value, 1, 16, 17);
                break;

            case AR_HAL_PIN_MODE_RGB888_GBR:
                AR_SET_REG_BITS(value, 2, 12, 13);
                AR_SET_REG_BITS(value, 0, 14, 15);
                AR_SET_REG_BITS(value, 1, 16, 17);
                break;

            default:
                ar_err("err rgb888 pin mode %d", pin_mode.rgb888);
                break;
        }
    #elif defined(CONFIG_ARTOSYN_AR9311)
        ar_err("not support RGB888");
    #endif
    }
    else if (out_color_mode==COLOR_MODE_YUV)
    {
        ar_always("COLOR_MODE_YUV");
        AR_SET_REG_BITS(value, 0, 3, 3);   /* disable lcd mode */
        if (is_bt656)
        {
        #if defined(CONFIG_ARTOSYN_AR9301)
            AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR),1,15,15); // set dvp 2x pixel clock
        #endif
            ar_always("cfg bt656 pin mode, must set 2x dvp output clock!");
            AR_SET_REG_BITS(value, 1, 1, 1);
            AR_SET_REG_BITS(value, 0, 12, 17);   /* if use DVP_CTRL_BASE_ADDR + 0x7c, should keep this reg [17:12] = 0 */

            switch (pin_mode.bt656)
            {
                case AR_HAL_PIN_MODE_BT656_DEFAULT:
                case AR_HAL_PIN_MODE_BT656_YCB_YCR_L:
                  #if defined(CONFIG_ARTOSYN_AR9301)
                    AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR), 0, 20, 21);
                  #elif defined(CONFIG_ARTOSYN_AR9311)
                    AR_SET_REG_BITS(value, 0, 12, 13);
                  #endif
                    AR_SET_REG_BITS(value, 2, 18, 19);
                    break;

                case AR_HAL_PIN_MODE_BT656_YCR_YCB_H:
                  #if defined(CONFIG_ARTOSYN_AR9301)
                    AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR), 2, 22, 23);
                  #elif defined(CONFIG_ARTOSYN_AR9311)
                    AR_SET_REG_BITS(value, 2, 14, 15);
                  #endif
                    AR_SET_REG_BITS(value, 0, 18, 19);
                    break;

                case AR_HAL_PIN_MODE_BT656_YCB_YCR_H:
                  #if defined(CONFIG_ARTOSYN_AR9301)
                    AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR), 2, 22, 23);
                  #elif defined(CONFIG_ARTOSYN_AR9311)
                    AR_SET_REG_BITS(value, 2, 14, 15);
                  #endif
                    AR_SET_REG_BITS(value, 2, 18, 19);
                    break;

                case AR_HAL_PIN_MODE_BT656_CRY_CBY_H:
                  #if defined(CONFIG_ARTOSYN_AR9301)
                    AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR), 2, 22, 23);
                  #elif defined(CONFIG_ARTOSYN_AR9311)
                    AR_SET_REG_BITS(value, 2, 14, 15);
                  #endif
                    AR_SET_REG_BITS(value, 1, 18, 19);
                    break;

                case AR_HAL_PIN_MODE_BT656_CBY_CRY_H:
                  #if defined(CONFIG_ARTOSYN_AR9301)
                    AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR), 2, 22, 23);
                  #elif defined(CONFIG_ARTOSYN_AR9311)
                    AR_SET_REG_BITS(value, 2, 14, 15);
                  #endif
                    AR_SET_REG_BITS(value, 3, 18, 19);
                    break;

                case AR_HAL_PIN_MODE_BT656_YCR_YCB_L:
                  #if defined(CONFIG_ARTOSYN_AR9301)
                    AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR), 0, 20, 21);
                  #elif defined(CONFIG_ARTOSYN_AR9311)
                    AR_SET_REG_BITS(value, 0, 12, 13);
                  #endif
                    AR_SET_REG_BITS(value, 0, 18, 19);
                    break;

                case AR_HAL_PIN_MODE_BT656_CRY_CBY_L:
                  #if defined(CONFIG_ARTOSYN_AR9301)
                    AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR), 0, 20, 21);
                  #elif defined(CONFIG_ARTOSYN_AR9311)
                    AR_SET_REG_BITS(value, 0, 12, 13);
                  #endif
                    AR_SET_REG_BITS(value, 1, 18, 19);
                    break;

                case AR_HAL_PIN_MODE_BT656_CBY_CRY_L:
                   #if defined(CONFIG_ARTOSYN_AR9301)
                    AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR), 0, 20, 21);
                   #elif defined(CONFIG_ARTOSYN_AR9311)
                     AR_SET_REG_BITS(value, 0, 12, 13);
                   #endif
                    AR_SET_REG_BITS(value, 3, 18, 19);
                    break;

                default:
                    ar_err("err bt656 pin mode %d", pin_mode.bt1120);
                    break;
            }
        }
        else
        {
            ar_always("cfg bt1120 pin mode");
            AR_SET_REG_BITS(value, 0, 1, 1);
          #if defined(CONFIG_ARTOSYN_AR9301)
            AR_SET_REG_BITS(__REG32__(DVP_GROUP_CTRL_ADDR), 0, 20, 23);
          #elif defined(CONFIG_ARTOSYN_AR9311)
            AR_SET_REG_BITS(value, 0, 12, 15);
          #endif
            switch (pin_mode.bt1120)
            {
                case AR_HAL_PIN_MODE_BT1120_DEFAULT:
                case AR_HAL_PIN_MODE_BT1120_YCB_YCR:
                    AR_SET_REG_BITS(value, 3, 18, 19);
                    break;

                case AR_HAL_PIN_MODE_BT1120_CBY_CRY:
                    AR_SET_REG_BITS(value, 2, 18, 19);
                    break;

                case AR_HAL_PIN_MODE_BT1120_CRY_CBY:
                    AR_SET_REG_BITS(value, 0, 18, 19);
                    break;

                case AR_HAL_PIN_MODE_BT1120_YCR_YCB:
                    AR_SET_REG_BITS(value, 1, 18, 19);
                    break;

                default:
                    ar_err("err bt1120 pin mode %d", pin_mode.bt1120);
                    break;
            }
        }
    }
    else
        ar_err("err color mode %d", out_color_mode);

    //set embbed
    mode=get_current_dvp_dev_sync_mode();
    if(mode==SYNC_MODE_INTERNAL)
    {
        ar_always("set to internal sync");
    	value |=(1<<7);
    }
    else
    {
       ar_always("set to external sync");
       value &=~(1<<7);
    }

    if(hsync_polarity)
        AR_SET_REG_BITS(value, 1, 4, 4);

    if(vsync_polarity)
        AR_SET_REG_BITS(value, 1, 5, 5);

    if(de_polarity)
        AR_SET_REG_BITS(value, 1, 6, 6);

    ar_always("dvp cfg 0x%x",value);
    write_reg32(DVP_OUT_16_OFFSET_REG, value);  //yuv mode
    ar_always("dvp out ctrl *0x%x=0x%x",DVP_OUT_16_OFFSET_REG, read_reg32(DVP_OUT_16_OFFSET_REG));

#ifdef CONFIG_ARTOSYN_AR9301
    ar_always("dvp group ctrl: *0x%x=0x%x", DVP_GROUP_CTRL_ADDR, read_reg32(DVP_GROUP_CTRL_ADDR));
    write_reg32(VIDEO_IF_BASE_ADDRESS + (0x40<<2),0);
    ar_always("dvp clock polarity: 0x%x", clock_polarity);
    unsigned int new_clock_polarity = read_reg32(DVP_CTRL_BASE_ADDR + 0x84);
    if(CLK_PHASE180 == clock_polarity){
        AR_SET_REG_BITS(new_clock_polarity, 1, 0, 0);
    }else{
        AR_SET_REG_BITS(new_clock_polarity, 0, 0, 0);
    }
    write_reg32(DVP_CTRL_BASE_ADDR + 0x84, new_clock_polarity);

#elif defined(CONFIG_ARTOSYN_AR9311)
    value = read_reg32(DVP_OUT_16_OFFSET_REG);
    if(clock_polarity < CLK_PHASE_DEFAULT || clock_polarity > CLK_PHASE270)
    {
        ar_err("wrong clock_polarity value:%d\n", clock_polarity);
    }
    else if(CLK_PHASE_DEFAULT == clock_polarity)
    {
        if(is_bt656)
            AR_SET_REG_BITS(value, 1, 22, 23); // select 2x pixel clock
        else
            AR_SET_REG_BITS(value, 1, 20, 21); // set defaule phase to 90

        AR_SET_REG_BITS(value, 1, 20, 21);  //set defaule phase to 90
    }
    else
    {
        if(is_bt656)
        {
            if(CLK_PHASE180 == clock_polarity)
                AR_SET_REG_BITS(value, 2, 22, 23); // select antiphase 2x pixel clock
            else
                AR_SET_REG_BITS(value, 1, 22, 23); // select 2x pixel clock
        }
        else
            AR_SET_REG_BITS(value, clock_polarity-1, 20, 21);
    }

    write_reg32(DVP_OUT_16_OFFSET_REG, value);

#endif
}


static void dvpo_init(display_interface_desc_t *dvp_desc)
{
	init_dvp_port(dvp_desc);
}

static int init_dvp_interface(void)
{
	display_interface_desc_t desc;
	display_intgerface_ops_t *dev_ops=get_dev_ops();
	if(!dev_ops)
	{
	   ar_err("err get dev_ops");
	   return -1;
	}
   if(dev_ops->get_display_interface_desc){
       dev_ops->get_display_interface_desc(&desc);
   }
   dvpo_init(&desc);
   if(dev_ops->init_display_interface){
       dev_ops->init_display_interface();
   }
   return 0;
}
static int reset_dvp_interface(void)
{
    display_intgerface_ops_t *dev_ops=get_dev_ops();
    if(!dev_ops)
    {
       ar_err("err get dev_ops");
       return -1;
    }
   dev_ops->reset_display_interface();
   return 0;
}
static int get_dvp_display_interface_desc(display_interface_desc_t *desc)
{
	display_intgerface_ops_t *dev_ops=get_dev_ops();
	if(!dev_ops)
	{
	   ar_err("err get dev_ops");
	   return -1;
	}
	if(dev_ops->get_display_interface_desc){
	    dev_ops->get_display_interface_desc(desc);
	}
    return 0;
}
static int get_dvp_display_interface_res_infor(display_interface_res_infor_t *res_infor)
{
	display_intgerface_ops_t *dev_ops=get_dev_ops();
	if(!dev_ops)
	{
	   ar_err("err get dev_ops");
	   return -1;
	}
	if(dev_ops->get_display_interface_res_infor){
	    dev_ops->get_display_interface_res_infor(res_infor);
	}
    return 0;
}

static int set_dvp_display_interface_res_infor(disp_res_infor_t *res_infor)
{
	display_intgerface_ops_t *dev_ops=get_dev_ops();
	if(!dev_ops)
	{
	   ar_err("err get dev_ops");
	   return -1;
	}
	if(dev_ops->set_display_interface_res_infor){
	    dev_ops->set_display_interface_res_infor(res_infor);
	}
    return 0;
}

static int set_dvp_display_interface_format(int format)
{
	display_intgerface_ops_t *dev_ops=get_dev_ops();
	if(!dev_ops)
	{
	   ar_err("err get dev_ops");
	   return -1;
	}
	if(dev_ops->set_display_interface_format){
	    dev_ops->set_display_interface_format(format);
	}
    return 0;
}

static int set_dvp_display_interface_ctl(int clt_code,void *pra,int size)
{
    display_intgerface_ops_t *dev_ops=get_dev_ops();

    switch(clt_code)
    {
        default:
        break;
    }

	if(!dev_ops)
	{
	   ar_err("err get dev_ops");
	   return -1;
	}
	if(dev_ops->set_display_interface_ctl){
	    dev_ops->set_display_interface_ctl(clt_code,pra,size);
	}
    return 0;
}
static display_intgerface_ops_t dvp_ops=
{
   .interface_index=DISPLAY_INTERFACE_DVP,
   .set_display_interface_index = set_dvp_interface_index,
   .init_display_interface=init_dvp_interface,
   .get_display_interface_desc=get_dvp_display_interface_desc,
   .get_display_interface_res_infor=get_dvp_display_interface_res_infor,
   .set_display_interface_res_infor=set_dvp_display_interface_res_infor,
   .on_display_interface=NULL,
   .off_display_interface=NULL,
   .reset_display_interface=reset_dvp_interface,
   .set_display_interface_format=set_dvp_display_interface_format,
   .set_display_interface_ctl=set_dvp_display_interface_ctl
};

display_intgerface_ops_t *get_dvp_ops(void)
{
   return &dvp_ops;
}
