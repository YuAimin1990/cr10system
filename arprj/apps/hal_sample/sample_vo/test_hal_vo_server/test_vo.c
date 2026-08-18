#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include "hal_vb.h"
#include "hal_vo.h"
#include "mpi_type.h"
#include "hal_dbglog.h"
#include "hal_ge2d.h"
#include "osal.h"
#include "hal_region.h"
#include "cfg_vo.h"
#include "test_vo.h"
#include "test_vo_rpc.h"
#include "parse_pra.h"
#include "template_pra.h"

static AR_BOOL is_running = AR_TRUE;

STRU_VO_OBJ_T vo_obj = {-1};
AR_BOOL cmd_is_running[MAX_VO_PANNEL_NUM][MAX_VO_CHN_NUM] = {0};
CFG_VO_S *cfg_vo = NULL;

extern int mpp_service_init(void);
extern int mpp_service_deinit(void);
extern AR_BOOL ar_hal_sys_mpp_service_is_lib(AR_VOID);

static void print_usage(const char *prog)
{
	printf("Usage1: %s path/vo_cfg.json [-cBistIbLfFCmpM]\n", prog);
	puts("  -c --csc             set device or layer csc (e.g. -c dev 1, -c layer 1 2)\n"
	     "  -B --bg_dev          set device background color, RGB888 format (e.g -B 00ff00)\n"
	     "  -i --interface       set display interface (e.g. -i 0)\n"
	     "  -s --sub_interface   set display sub interface (e.g. -s 2)\n"
	     "  -t --timing_template set display timing template (e.g. -t 6)\n"
	     "  -I --interlace       set if interlace format (e.g. -I 0)\n"
	     "  -b --bit_count       set bit_count_per_channel (e.g. -b 8)\n"
	     "  -L --layer0          set video layer resolution (e.g. -L 1920 1080)\n"
	     "  -f --fps             set display frame rate (e.g. -f 60)\n"
	     "  -F --format          set display format (e.g. -F 3)\n"
	     "  -C --channel         set channel information (e.g. -C 1 0 ff0000 1280 720 100 50)\n"
	     "  -m --ch_map          set channel enable bitmap (e.g. -m 10f)\n"
	     "  -p --image           set channel input image information (e.g. -p 1 /usrdata/res/yuv/1280x720.yuv 1280 720)\n"
	     "  -M --mouse           set cursor (e.g. -M enable 1, -M setpos 100 200, -M loadfile /usrdata/cursor.bmp)\n");

	exit(1);
}

static void parse_opts(int argc, char *argv[], CFG_VO_S *cfg, AR_U32 *ch_map)
{
	AR_U32 chn = 0;
	int chan_id = 0;
	int index = 0;

	while (1)
	{
		static const struct option lopts[] =
		{
			{ "csc",             1, 0, 'c' }, //csc setting
			{ "bg_dev",          1, 0, 'B' }, //device background color, RGB888
			{ "interface",       1, 0, 'i' }, //ENUM_AR_HAL_VO_DEV_INTF
			{ "sub_interface",   1, 0, 's' }, //ENUM_AR_HAL_VO_DEV_SUB_INTF
			{ "timing_template", 1, 0, 't' }, //ENUM_AR_HAL_VO_DEV_TIMING_TEMPLATE
			{ "interlace",       1, 0, 'I' }, //interlace or not
			{ "bit_count",       1, 0, 'b' }, //bit_count_per_channel
			{ "layer0",          1, 0, 'L' }, //layer resolution
			{ "fps",             1, 0, 'f' }, //frame rate
			{ "format",          1, 0, 'F' }, //layer format, ENUM_AR_HAL_VO_FMT
			{ "channel",         1, 0, 'C' }, //channel information
			{ "ch_map",          1, 0, 'm' }, //channel enable bitmap
			{ "image",           1, 0, 'p' }, //input image information
			{ "mouse",           1, 0, 'M' }, //cursor
			{ NULL,              0, 0, 0   },
		};

		int  c = getopt_long(argc, argv, "c:B:i:s:t:I:b:L:f:F:C:m:p:M:", lopts, NULL);

		if (c == -1)
			break;

		if (index == 0)
		{
			index = 2;
		}

		index++;
		switch (c)
		{
		case 'c':
			if (0 == memcmp(argv[index], "dev", strlen(argv[index])))
			{
				index++;
				cfg->stVoDev.dev_csc = atoi(argv[index++]);
				printf("cfg->stVoDev.dev_csc = %d\n", cfg->stVoDev.dev_csc);
			}
			else if (0 == memcmp(argv[index], "layer", strlen(argv[index])))
			{
				index++;
				cfg->stVoDev.layer_csc_in = atoi(argv[index++]);
				cfg->stVoDev.layer_csc_out = atoi(argv[index++]);
				printf("cfg->stVoDev.layer_csc_in = %d\n", cfg->stVoDev.layer_csc_in);
				printf("cfg->stVoDev.layer_csc_out = %d\n", cfg->stVoDev.layer_csc_out);
			}
			break;
		case 'B':
			cfg->stVoDev.bg_color = strtol(argv[index++], NULL, 16);
			printf("cfg->stVoDev.bg_color = 0x%x\n", cfg->stVoDev.bg_color);
			break;
		case 'i':
			cfg->stVoDev.interface = atoi(argv[index++]);
			printf("cfg->stVoDev.interface = %d\n", cfg->stVoDev.interface);
			break;
		case 's':
			cfg->stVoDev.sub_interface = atoi(argv[index++]);
			printf("cfg->stVoDev.sub_interface = %d\n", cfg->stVoDev.sub_interface);
			break;
		case 't':
			cfg->stVoDev.timing_template = atoi(argv[index++]);
			printf("cfg->stVoDev.timing_template = %d\n", cfg->stVoDev.timing_template);
			break;
		case 'I':
			cfg->stVoDev.is_interlace = atoi(argv[index++]);
			printf("cfg->stVoDev.is_interlace = %d\n", cfg->stVoDev.is_interlace);
			break;
		case 'b':
			cfg->stVoDev.bit_count_per_channel = atoi(argv[index++]);
			printf("cfg->stVoDev.bit_count_per_channel = %d\n", cfg->stVoDev.bit_count_per_channel);
			break;
		case 'L':
			cfg->stVoDev.width = atoi(argv[index++]);
			cfg->stVoDev.height = atoi(argv[index++]);
			printf("cfg->stVoDev.width = %d\n", cfg->stVoDev.width);
			printf("cfg->stVoDev.height = %d\n", cfg->stVoDev.height);
			break;
		case 'f':
			cfg->stVoDev.fps = atoi(argv[index++]);
			printf("cfg->stVoDev.fps = %f\n", cfg->stVoDev.fps);
			break;
		case 'F':
			cfg->stVoDev.format = atoi(argv[index++]);
			printf("cfg->stVoDev.format = %d\n", cfg->stVoDev.format);
			break;
		case 'C':
			chan_id = atoi(argv[index++]);
			chn |= (1 << chan_id);
			cfg->stVoChn[chan_id].enable = 1;
			cfg->stVoChn[chan_id].priority = atoi(argv[index++]);
			cfg->stVoChn[chan_id].bg_color = strtol(argv[index++], NULL, 16);
			cfg->stVoChn[chan_id].ch_width = atoi(argv[index++]);
			cfg->stVoChn[chan_id].ch_height = atoi(argv[index++]);
			cfg->stVoChn[chan_id].ch_pos_x = atoi(argv[index++]);
			cfg->stVoChn[chan_id].ch_pos_y = atoi(argv[index++]);
			printf("cfg->stVoChn[%d].priority = %d\n", chan_id, cfg->stVoChn[chan_id].priority);
			printf("cfg->stVoChn[%d].bg_color = 0x%x\n", chan_id, cfg->stVoChn[chan_id].bg_color);
			printf("cfg->stVoChn[%d].ch_width = %d\n", chan_id, cfg->stVoChn[chan_id].ch_width);
			printf("cfg->stVoChn[%d].ch_height = %d\n", chan_id, cfg->stVoChn[chan_id].ch_height);
			printf("cfg->stVoChn[%d].ch_pos_x = %d\n", chan_id, cfg->stVoChn[chan_id].ch_pos_x);
			printf("cfg->stVoChn[%d].ch_pos_y = %d\n", chan_id, cfg->stVoChn[chan_id].ch_pos_y);
			break;
		case 'm':
			*ch_map = strtol(argv[index++], NULL, 16);
			break;
		case 'p':
			chan_id = atoi(argv[index++]);
			strcpy(cfg->stVoChn[chan_id].imagepath, argv[index++]);
			cfg->stVoChn[chan_id].image_width = atoi(argv[index++]);
			cfg->stVoChn[chan_id].image_height = atoi(argv[index++]);
			printf("cfg->stVoChn[%d].imagepath = %s\n", chan_id, cfg->stVoChn[chan_id].imagepath);
			printf("cfg->stVoChn[%d].image_width = %d\n", chan_id, cfg->stVoChn[chan_id].image_width);
			printf("cfg->stVoChn[%d].image_height = %d\n", chan_id, cfg->stVoChn[chan_id].image_height);
			break;
		case 'M':
			if (0 == memcmp(argv[index], "enable", strlen(argv[index])))
			{
				index++;
				cfg->stVoDev.cursor_enable = atoi(argv[index++]);
				printf("cursor enable = %d\n", cfg->stVoDev.cursor_enable);
			}
			else if (0 == memcmp(argv[index], "setpos", strlen(argv[index])))
			{
				index++;
				cfg->stVoDev.cursor_pos_x = atoi(argv[index++]);
				cfg->stVoDev.cursor_pos_y = atoi(argv[index++]);
				printf("cfg->stVoDev.cursor_pos_x = %d\n", cfg->stVoDev.cursor_pos_x);
				printf("cfg->stVoDev.cursor_pos_y = %d\n", cfg->stVoDev.cursor_pos_y);
			}
			else if (0 == memcmp(argv[index], "loadfile", strlen(argv[index])))
			{
				index++;
				strncpy(cfg->stVoDev.cursorpath, argv[index++], 255);
				cfg->stVoDev.cursorpath[255] = 0;
				printf("cfg->stVoDev.cursorpath = %s\n", cfg->stVoDev.cursorpath);
			}
			break;
		default:
			break;
		}
	}


	if (chn != 0)
		*ch_map &= chn;
}

AR_S32 vo_read_oneframe(AR_CHAR filename[256], AR_U8 * pY, AR_U8 * pU, AR_U8 * pV,
                                              AR_U32 width, AR_U32 height, AR_U32 stride, AR_U32 stride2,
                                              ENUM_AR_HAL_VO_FMT enPixFrm)
{
    AR_U8 * pDst;
    AR_U32 u32UVHeight;
    AR_U32 u32Row;

    if (enPixFrm == AR_HAL_VO_FMT_YV12)
    {
        FILE *pfd = fopen(filename,"rb");

		if (pfd == AR_NULL) {
			PRINT_ERR("open file %s fail \n", filename);
			return -4;
		}

		fseek(pfd, 0, SEEK_SET);

		u32UVHeight = height / 2;

		pDst = pY;
		for ( u32Row = 0; u32Row < height; u32Row++ ) {
			if (fread( pDst, 1, width, pfd ) != width) {
				fclose(pfd);
				return -1;
			}
			pDst += stride;
		}

		pDst = pU;
		for ( u32Row = 0; u32Row < u32UVHeight; u32Row++ ) {
			if(fread( pDst, 1, width/2, pfd ) != width/2) {
				fclose(pfd);
				return -2;
			}
			pDst += stride2;
		}

		pDst = pV;
		for ( u32Row = 0; u32Row < u32UVHeight; u32Row++ ) {
			if(fread( pDst, 1, width/2, pfd ) != width/2) {
				fclose(pfd);
				return -3;
			}
			pDst += stride2;
		}

		fclose(pfd);
	}

	return AR_SUCCESS;
}

static AR_S32 sys_init(STRU_VO_OBJ_T *obj, CFG_VO_S *cfg)
{
    int              ch_num = 0;
	AR_S32           ret;
	AR_S32           y_stride;
	AR_S32           height;
	STRU_VB_CONFIG_S st_vb_config;

	memset(&st_vb_config, 0, sizeof(st_vb_config));

	y_stride = ALIGN_UP(1920, 512);
	height = ALIGN_UP(1080, 2);

	for (int chan_id = 0; chan_id < MAX_VO_CHN_NUM; chan_id++)
	{
		if (!cfg->stVoChn[chan_id].enable || !(get_vo_ext_paras()->layer_ch_map & (1 << chan_id)))
			continue;

		obj->video_frame_queue[chan_id] = ar_queue_create(6, sizeof(STRU_ADDR *), NULL);
		for (int frame_index = 0; frame_index <6; frame_index++)
		{
			STRU_ADDR *frame = ar_malloc(sizeof(STRU_ADDR));
			if (frame)
			{
				memset(frame, 0, sizeof(STRU_ADDR));
				if (ar_queue_push(obj->video_frame_queue[chan_id], &frame))
					PRINT_ERR("%d %d insert failed!\n", 0, chan_id);

			}
			else
			{
				PRINT_ERR("%d %d malloc failed!\n", 0, chan_id);
			}
		}
		PRINT_INFO("obj->video_frame_queue[%d] = %p", chan_id, obj->video_frame_queue[chan_id]);

		ch_num++;

		for (int i = 0; i < cfg->stVoDev.overlayer_num; i++)
		{
			obj->overlay_video_frame_queue[i][chan_id] = ar_queue_create(6, sizeof(STRU_ADDR *), NULL);
			for (int overlay_frame_index = 0; overlay_frame_index <6; overlay_frame_index++)
			{
				STRU_ADDR *overlay_frame = ar_malloc(sizeof(STRU_ADDR));
				if (overlay_frame)
				{
					memset(overlay_frame, 0, sizeof(STRU_ADDR));
					if (ar_queue_push(obj->overlay_video_frame_queue[i][chan_id], &overlay_frame))
						PRINT_ERR("%d %d insert failed!\n", overlay_frame_index+1, chan_id);

				}
				else
				{
					PRINT_ERR("%d %d malloc failed!\n", overlay_frame_index+1, chan_id);
				}
			}
			PRINT_INFO("obj->overlay_video_frame_queue[%d][%d] = %p\n", i, chan_id, obj->overlay_video_frame_queue[i][chan_id]);

			ch_num++;
		}

	}

	if (cfg->stVoDev.format == AR_HAL_VO_FMT_YV12)
	{
		st_vb_config.st_comm_pool[0].u64_blk_size = (y_stride * height)+(y_stride * height/4)+(y_stride * height/4);
	}
	st_vb_config.st_comm_pool[0].u32_blk_cnt = 5 * ch_num;
	st_vb_config.st_comm_pool[0].e_remap_mode = AR_VB_REMAP_MODE_NONE;
	memset(&st_vb_config.st_comm_pool[0].str_mmz_name, 0x00, sizeof(st_vb_config.st_comm_pool[0].str_mmz_name));

	PRINT_INFO("ch_num:%d, blk_cnt:%d", ch_num, 5 * ch_num);

	ret = ar_hal_vb_exit();
	if(AR_SUCCESS != ret)
		PRINT_ERR("ar_hal_vb_exit failed, ret:%02x!\n", ret);

    st_vb_config.u32_max_pool_cnt = 1;

    if (AR_SUCCESS != ar_hal_vb_set_config(&st_vb_config))
    {
        PRINT_ERR("ar_hal_vb_set_config failed!\n");
        return AR_FAILURE;
    }

    if (AR_SUCCESS != ar_hal_vb_init())
    {
        PRINT_ERR("ar_hal_vb_init failed!\n");
        return AR_FAILURE;
    }

	if ((obj->src_pool_id = ar_hal_vb_create_pool(&st_vb_config.st_comm_pool[0])) == AR_VB_INVALID_POOLID)
	{
		PRINT_ERR("ar_hal_vb_create_pool failed!\n");
			return AR_FAILURE;
	}
	PRINT_INFO("obj->src_pool_id = %d", obj->src_pool_id);

	ret = ar_hal_vb_mmap_pool(obj->src_pool_id);
	if (ret != AR_SUCCESS)
	{
		PRINT_ERR("ar_hal_vb_mmap_pool failed %d\n", ret);
		return ret;
	}

	return AR_SUCCESS;
}

static void sys_deinit(STRU_VO_OBJ_T *obj)
{
	int ret;

	if (obj->src_pool_id)
	{
		ret = ar_hal_vb_munmap_pool(obj->src_pool_id);
		if(AR_SUCCESS != ret)
			PRINT_ERR("ar_hal_vb_munmap_pool failed, ret:%02x!\n", ret);

		ret = ar_hal_vb_destroy_pool(obj->src_pool_id);
		if(AR_SUCCESS != ret)
			PRINT_ERR("ar_hal_vb_destroy_pool failed, ret:%02x!\n", ret);
	}

	ret = ar_hal_vb_exit();
	if(AR_SUCCESS != ret)
		PRINT_ERR("ar_hal_vb_exit failed, ret:%02x!\n", ret);

}

static int vo_dev_init(STRU_VO_OBJ_T *obj, CFG_VO_S *cfg)
{
    STRU_AR_HAL_VO_DEV_ATTR    dev_attr={0}, dev_attr1={0};
	ENUM_AR_HAL_VO_CSC         dev_csc=AR_HAL_VO_CSC_BT601_FULL, dev_csc1=AR_HAL_VO_CSC_BT601_FULL;
    STRU_AR_HAL_VO_LAYER_ATTR  layer_attr={0}, layer_attr1={0};
	STRU_AR_HAL_VO_LAYER_CSC   layer_csc={AR_HAL_VO_CSC_BT601_FULL, AR_HAL_VO_CSC_BT601_FULL};
    STRU_AR_HAL_VO_LAYER_CSC   layer_csc1={AR_HAL_VO_CSC_BT601_FULL, AR_HAL_VO_CSC_BT601_FULL};
	STRU_AR_HAL_VO_POS         layer_pos={0};
	STRU_AR_HAL_VO_POS         cursor_pos, cursor_pos1;
    int                        ret;

    /* init vo dev */
    obj->dev_fd = ar_hal_vo_dev_open(cfg->stVoDev.dev_id);
    if (obj->dev_fd < 0)
    {
        ret = HAL_ERR_VO_DEV_OPEN;
        goto End;
    }

    dev_attr.bg_color = cfg->stVoDev.bg_color;
    dev_attr.e_interface = cfg->stVoDev.interface;
    dev_attr.e_sub_interface = cfg->stVoDev.sub_interface;

    dev_attr.e_timing_template = cfg->stVoDev.timing_template;
	if (dev_attr.e_timing_template == AR_HAL_VO_OUTPUT_USER)
	{
		//customize timing
		dev_attr.timing_customize = get_vo_ext_paras()->dev_sync_info;
	}
    dev_attr.bit_count_per_channel = cfg->stVoDev.bit_count_per_channel;

	dev_attr.dpi_format = get_vo_ext_paras()->dev_dpi_format;
	dev_attr.color_mode = get_vo_ext_paras()->dev_color_mode;
	dev_attr.sync_mode = get_vo_ext_paras()->dev_sync_mode;
	dev_attr.pin_mode.bt656 = get_vo_ext_paras()->dev_pin_mode;

#if defined(PROXIMA)
    dev_attr.sync_mode = AR_HAL_SYNC_MODE_INTERNAL;
#endif

    ret = ar_hal_vo_dev_set_attr(obj->dev_fd, &dev_attr);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    dev_csc = cfg->stVoDev.dev_csc;
	ret = ar_hal_vo_dev_set_csc(obj->dev_fd, &dev_csc);
	if (ret != AR_HAL_VO_SUCCESS)
		goto End;

	ret = ar_hal_vo_dev_enable(obj->dev_fd);
	if (ret != AR_HAL_VO_SUCCESS)
		goto End;

	ret = ar_hal_vo_dev_get_attr(obj->dev_fd, &dev_attr1);
	if (ret != AR_HAL_VO_SUCCESS)
		goto End;
	else
	{
		if (0 != memcmp((char*)&dev_attr, (char*)&dev_attr1, sizeof(STRU_AR_HAL_VO_DEV_ATTR)))
		{
			PRINT_ERR("ar_hal_vo_dev_get_attr error\n");
			return AR_FAILURE;
		}
	}

	ret = ar_hal_vo_dev_get_csc(obj->dev_fd, &dev_csc1);
	if (ret != AR_HAL_VO_SUCCESS)
        goto End;
	else
	{
		if (dev_csc != dev_csc1)
		{
			PRINT_ERR("ar_hal_vo_dev_get_csc error\n");
			return AR_FAILURE;
		}
	}

	if(AR_HAL_VO_DEV_INTF_MIPI == dev_attr.e_interface)
	{
		set_dsi_config(DSI_PRA_DEFAULT);
		ret = ar_hal_vo_dsi_set_attr(obj->dev_fd, get_dsi_attr());
		if (AR_HAL_VO_SUCCESS != ret)
			return AR_FAILURE;

		ret = ar_hal_vo_dsi_enable(obj->dev_fd);
		if (ret != AR_HAL_VO_SUCCESS)
			return AR_FAILURE;

		lcd_gpio_init(LCD_GPIO_DEFAULT);
	}
	printf("vo dev init success!\n");

    /* init vo layer */
    obj->layer_fd = ar_hal_vo_layer_open(cfg->stVoDev.layer_id);
    if (obj->layer_fd < 0)
    {
        ret = HAL_ERR_VO_LAYER_OPEN;
        goto End;
    }
    layer_attr.width = cfg->stVoDev.width;
    layer_attr.height = cfg->stVoDev.height;
    layer_attr.luma_stride = get_vo_ext_paras()->layer_luma_stride==0 ? ALIGN_UP(cfg->stVoDev.width, 512) : get_vo_ext_paras()->layer_luma_stride;//to do
    layer_attr.chroma_stride = get_vo_ext_paras()->layer_chroma_stride ==0 ?  ALIGN_UP(cfg->stVoDev.width / 2, 256) : get_vo_ext_paras()->layer_chroma_stride;//to do
    layer_attr.format = cfg->stVoDev.format;
    layer_attr.fps = cfg->stVoDev.fps;
    layer_attr.rotation = get_vo_ext_paras()->layer_rotation;

    ret = ar_hal_vo_layer_set_attr(obj->layer_fd, &layer_attr);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    layer_csc.input = cfg->stVoDev.layer_csc_in;
	layer_csc.output = cfg->stVoDev.layer_csc_out;
	ret = ar_hal_vo_layer_set_csc(obj->layer_fd, &layer_csc);
	if (ret != AR_HAL_VO_SUCCESS)
        goto End;

	ret = ar_hal_vo_layer_enable(obj->layer_fd);
	if (ret != AR_HAL_VO_SUCCESS)
        goto End;

	ret = ar_hal_vo_layer_get_attr(obj->layer_fd, &layer_attr1);
	if (ret != AR_HAL_VO_SUCCESS)
        goto End;
	else
	{
		if (0 != memcmp((char*)&layer_attr, (char*)&layer_attr1, sizeof(STRU_AR_HAL_VO_LAYER_ATTR)))
		{
			PRINT_ERR("ar_hal_vo_layer_get_attr error\n");
			return AR_FAILURE;
		}
	}

	ret = ar_hal_vo_layer_get_csc(obj->layer_fd, &layer_csc1);
	if (ret != AR_HAL_VO_SUCCESS)
        goto End;
	else
	{
		if ((layer_csc.input != layer_csc1.input) || (layer_csc.output != layer_csc1.output))
		{
			PRINT_ERR("ar_hal_vo_layer_get_csc error\n");
			return AR_FAILURE;
		}
	}
	printf("vo layer init success!\n");

    /* init vo overlayer */
	for (int i = 0; i < cfg->stVoDev.overlayer_num; i++)
	{
		obj->overlayer_fd[i] = ar_hal_vo_layer_open(AR_HAL_VO_LAYER_ID_OVERLAY_0 + i);
		if (obj->overlayer_fd[i] < 0)
		{
			ret = HAL_ERR_VO_LAYER_OPEN;
			goto End;
		}
		layer_attr.width = cfg->stVoDev.width;
		layer_attr.height = cfg->stVoDev.height;
		layer_attr.luma_stride = get_vo_ext_paras()->layer_luma_stride==0 ? ALIGN_UP(cfg->stVoDev.width, 512) : get_vo_ext_paras()->layer_luma_stride;//to do
		layer_attr.chroma_stride = get_vo_ext_paras()->layer_chroma_stride ==0 ?  ALIGN_UP(cfg->stVoDev.width / 2, 256) : get_vo_ext_paras()->layer_chroma_stride;//to do; //to do
		layer_attr.format = cfg->stVoDev.format;
		layer_attr.fps = cfg->stVoDev.fps;
		layer_attr.rotation = get_vo_ext_paras()->layer_rotation;
		ret = ar_hal_vo_layer_set_attr(obj->overlayer_fd[i], &layer_attr);
		if (ret != AR_HAL_VO_SUCCESS)
			goto End;

		layer_csc.input = cfg->stVoDev.layer_csc_in;
		layer_csc.output = cfg->stVoDev.layer_csc_out;
		ret = ar_hal_vo_layer_set_csc(obj->overlayer_fd[i], &layer_csc);
		if (ret != AR_HAL_VO_SUCCESS)
			goto End;

		ret = ar_hal_vo_layer_enable(obj->overlayer_fd[i]);
		if (ret != AR_HAL_VO_SUCCESS)
			goto End;

		layer_pos.x = cfg->stVoDev.pos_x;
		layer_pos.y = cfg->stVoDev.pos_y;
		ret = ar_hal_vo_layer_set_pos(obj->overlayer_fd[i], &layer_pos);
		if (ret != AR_HAL_VO_SUCCESS)
			goto End;

		ret = ar_hal_vo_layer_get_attr(obj->overlayer_fd[i], &layer_attr1);
		if (ret != AR_HAL_VO_SUCCESS)
			goto End;
		else
		{
			if (0 != memcmp((char*)&layer_attr, (char*)&layer_attr1, sizeof(STRU_AR_HAL_VO_LAYER_ATTR)))
			{
				PRINT_ERR("ar_hal_vo_layer_get_attr error\n");
				return AR_FAILURE;
			}
		}

		ret = ar_hal_vo_layer_get_csc(obj->overlayer_fd[i], &layer_csc1);
		if (ret != AR_HAL_VO_SUCCESS)
			goto End;
		else
		{
			if ((layer_csc.input != layer_csc1.input) || (layer_csc.output != layer_csc1.output))
			{
				PRINT_ERR("ar_hal_vo_layer_get_csc error\n");
				return AR_FAILURE;
			}
		}
		printf("vo overlayer%d init success!\n", i);
	}

	/* init cursor */
	if (cfg->stVoDev.cursor_enable)
	{
		if (strlen(cfg->stVoDev.cursorpath) != 0)
		{
			ret = ar_hal_vo_dev_cfg_cursor_file(obj->dev_fd, cfg->stVoDev.cursorpath);
			if (ret != AR_HAL_VO_SUCCESS)
				goto End;
		}

		ret = ar_hal_vo_dev_enable_cursor(obj->dev_fd);
		if (ret != AR_HAL_VO_SUCCESS)
			goto End;

		cursor_pos.x = cfg->stVoDev.cursor_pos_x;
		cursor_pos.y = cfg->stVoDev.cursor_pos_y;
		ret = ar_hal_vo_dev_set_cursor_pos(obj->dev_fd, &cursor_pos);
		if (ret != AR_HAL_VO_SUCCESS)
			goto End;

		ret = ar_hal_vo_dev_get_cursor_pos(obj->dev_fd, &cursor_pos1);
		if (ret != AR_HAL_VO_SUCCESS)
			goto End;
		else
		{
			if ((cursor_pos.x != cursor_pos1.x) || (cursor_pos.y != cursor_pos1.y))
			{
				PRINT_ERR("ar_hal_vo_dev_get_cursor_pos error\n");
				return AR_FAILURE;
			}
		}
		printf("cursor init success!\n");
	}
	else
	{
		//default disable
		//ar_hal_vo_dev_disable_cursor(obj->dev_fd);
	}

	End:
     if (ret != AR_HAL_VO_SUCCESS)
        PRINT_ERR("ret = 0x%x\r\n", ret);

	 return ret;
}

int ar_vo_channel_init(STRU_VO_OBJ_T *obj, CFG_VO_S *cfg)
{
    STRU_AR_HAL_VO_CHN_ATTR    chan_attr, chan_attr1;
	STRU_AR_VO_CHN_PARAM_S     chan_para; //, chan_para1;
	//STRU_AR_HAL_VO_RECT        chan_pos, chan_pos1;
    int                        ret = AR_HAL_VO_SUCCESS;

    for (int chan_id = 0; chan_id < CFG_MAX_VO_CHN_NUM; chan_id++)
    {
        if (!cfg->stVoChn[chan_id].enable  || !(get_vo_ext_paras()->layer_ch_map & (1 << chan_id)))
            continue;

        obj->chan_fd[chan_id] = ar_hal_vo_chn_open(cfg->stVoDev.layer_id, chan_id);
        if (obj->chan_fd[chan_id] < 0)
        {
            ret = HAL_ERR_VO_CHN_OPEN;
            goto End;
        }

        chan_attr.priority = cfg->stVoChn[chan_id].priority;
        chan_attr.rect.x = cfg->stVoChn[chan_id].ch_pos_x;
		chan_attr.rect.y = cfg->stVoChn[chan_id].ch_pos_y;
		chan_attr.rect.w = cfg->stVoChn[chan_id].ch_width;
		chan_attr.rect.h = cfg->stVoChn[chan_id].ch_height;
        ret = ar_hal_vo_chn_set_attr(obj->chan_fd[chan_id], &chan_attr);
        if (ret != AR_HAL_VO_SUCCESS)
            goto End;

		chan_para.st_AspectRatio.e_mode = cfg->stVoChn[chan_id].aspect_ratio_mode;
		chan_para.st_AspectRatio.st_videoRect.x = cfg->stVoChn[chan_id].aspect_ratio_pos_x;
		chan_para.st_AspectRatio.st_videoRect.y = cfg->stVoChn[chan_id].aspect_ratio_pos_y;
		chan_para.st_AspectRatio.st_videoRect.w = cfg->stVoChn[chan_id].aspect_ratio_width;
		chan_para.st_AspectRatio.st_videoRect.h = cfg->stVoChn[chan_id].aspect_ratio_height;
		chan_para.st_AspectRatio.u32_bgColor = cfg->stVoChn[chan_id].bg_color;
		ret = ar_hal_vo_chn_set_para(obj->chan_fd[chan_id], &chan_para);
        if (ret != AR_HAL_VO_SUCCESS)
            goto End;

        ret = ar_hal_vo_chn_enable(obj->chan_fd[chan_id]);
        if (ret != AR_HAL_VO_SUCCESS)
            goto End;

		ret = ar_hal_vo_chn_get_attr(obj->chan_fd[chan_id], &chan_attr1);
        if (ret != AR_HAL_VO_SUCCESS)
            goto End;
		else
		{
			if (0 != memcmp((char*)&chan_attr, (char*)&chan_attr1, sizeof(STRU_AR_HAL_VO_CHN_ATTR)))
			{
				PRINT_ERR("ar_hal_vo_chn_get_attr error\n");
				return AR_FAILURE;
			}
		}

		/*ret = ar_hal_vo_chn_get_para(obj->chan_fd[chan_id], &chan_para1);
        if (ret != AR_HAL_VO_SUCCESS)
            goto End;
		else
		{
			if (0 != memcmp((char*)&chan_para, (char*)&chan_para1, sizeof(STRU_AR_VO_CHN_PARAM_S)))
			{
				PRINT_ERR("ar_hal_vo_chn_get_para error\n");
				return AR_FAILURE;
			}
		}*/

		/*chan_pos.x = chan_attr.rect.x + 10;
		chan_pos.y = chan_attr.rect.y + 10;
		chan_pos.w = chan_attr.rect.w + 10;
		chan_pos.h = chan_attr.rect.h + 10;
		ret = ar_hal_vo_chn_set_position(obj->chan_fd[chan_id], &chan_pos);
		if (ret != AR_HAL_VO_SUCCESS)
            goto End;

		ret = ar_hal_vo_chn_get_position(obj->chan_fd[chan_id], &chan_pos);
		if (ret != AR_HAL_VO_SUCCESS)
            goto End;
		else
		{
			if (0 != memcmp((char*)&chan_pos, (char*)&chan_pos1, sizeof(STRU_AR_HAL_VO_RECT)))
			{
				PRINT_ERR("ar_hal_vo_chn_get_position error\n");
				return AR_FAILURE;
			}
		}

		chan_pos.x = chan_attr.rect.x;
		chan_pos.y = chan_attr.rect.y;
		chan_pos.w = chan_attr.rect.w;
		chan_pos.h = chan_attr.rect.h;
		ret = ar_hal_vo_chn_set_position(obj->chan_fd[chan_id], &chan_pos);
		if (ret != AR_HAL_VO_SUCCESS)
            goto End;*/

		for (int i = 0; i < cfg->stVoDev.overlayer_num; i++)
		{
			obj->overlay_chan_fd[i][chan_id] = ar_hal_vo_chn_open(AR_HAL_VO_LAYER_ID_OVERLAY_0 + i, chan_id);
			if (obj->overlay_chan_fd[i][chan_id] < 0)
			{
				ret = HAL_ERR_VO_CHN_OPEN;
				goto End;
			}

			chan_attr.priority = cfg->stVoChn[chan_id].priority;
			chan_attr.rect.x = cfg->stVoChn[chan_id].ch_pos_x + (i + 1) * cfg->stVoChn[chan_id].ch_width / 4;
			chan_attr.rect.y = cfg->stVoChn[chan_id].ch_pos_y + (i + 1) * cfg->stVoChn[chan_id].ch_height / 4;
			chan_attr.rect.w = cfg->stVoChn[chan_id].ch_width / (2 * (i + 1));
			chan_attr.rect.h = cfg->stVoChn[chan_id].ch_height / (2 * (i + 1));
			ret = ar_hal_vo_chn_set_attr(obj->overlay_chan_fd[i][chan_id], &chan_attr);
			if (ret != AR_HAL_VO_SUCCESS)
				goto End;

			chan_para.st_AspectRatio.e_mode = cfg->stVoChn[chan_id].aspect_ratio_mode;
			chan_para.st_AspectRatio.st_videoRect.x = cfg->stVoChn[chan_id].aspect_ratio_pos_x + (i + 1) * cfg->stVoChn[chan_id].aspect_ratio_width / 4;
			chan_para.st_AspectRatio.st_videoRect.y = cfg->stVoChn[chan_id].aspect_ratio_pos_y + (i + 1) * cfg->stVoChn[chan_id].aspect_ratio_height / 4;
			chan_para.st_AspectRatio.st_videoRect.w = cfg->stVoChn[chan_id].aspect_ratio_width / (2 * (i + 1));
			chan_para.st_AspectRatio.st_videoRect.h = cfg->stVoChn[chan_id].aspect_ratio_height / (2 * (i + 1));
			chan_para.st_AspectRatio.u32_bgColor = cfg->stVoChn[chan_id].bg_color;
			ret = ar_hal_vo_chn_set_para(obj->overlay_chan_fd[i][chan_id], &chan_para);
			if (ret != AR_HAL_VO_SUCCESS)
				goto End;

			ret = ar_hal_vo_chn_enable(obj->overlay_chan_fd[i][chan_id]);
			if (ret != AR_HAL_VO_SUCCESS)
				goto End;

			ret = ar_hal_vo_chn_get_attr(obj->overlay_chan_fd[i][chan_id], &chan_attr1);
			if (ret != AR_HAL_VO_SUCCESS)
				goto End;
			else
			{
				if (0 != memcmp((char*)&chan_attr, (char*)&chan_attr1, sizeof(STRU_AR_HAL_VO_CHN_ATTR)))
				{
					PRINT_ERR("ar_hal_vo_chn_get_attr error\n");
					return AR_FAILURE;
				}
			}

			/*ret = ar_hal_vo_chn_get_para(obj->overlay_chan_fd[i][chan_id], &chan_para1);
			if (ret != AR_HAL_VO_SUCCESS)
				goto End;
			else
			{
				if (0 != memcmp((char*)&chan_para, (char*)&chan_para1, sizeof(STRU_AR_VO_CHN_PARAM_S)))
				{
					PRINT_ERR("ar_hal_vo_chn_get_para error\n");
					return AR_FAILURE;
				}
			}*/

			/*chan_pos.x = chan_attr.rect.x + 10;
			chan_pos.y = chan_attr.rect.y + 10;
			chan_pos.w = chan_attr.rect.w + 10;
			chan_pos.h = chan_attr.rect.h + 10;
			ret = ar_hal_vo_chn_set_position(obj->overlay_chan_fd[i][chan_id], &chan_pos);
			if (ret != AR_HAL_VO_SUCCESS)
				goto End;

			ret = ar_hal_vo_chn_get_position(obj->overlay_chan_fd[i][chan_id], &chan_pos);
			if (ret != AR_HAL_VO_SUCCESS)
				goto End;
			else
			{
				if (0 != memcmp((char*)&chan_pos, (char*)&chan_pos1, sizeof(STRU_AR_HAL_VO_RECT)))
				{
					PRINT_ERR("ar_hal_vo_chn_get_position error\n");
					return AR_FAILURE;
				}
			}

			chan_pos.x = chan_attr.rect.x;
			chan_pos.y = chan_attr.rect.y;
			chan_pos.w = chan_attr.rect.w;
			chan_pos.h = chan_attr.rect.h;
			ret = ar_hal_vo_chn_set_position(obj->overlay_chan_fd[i][chan_id], &chan_pos);
			if (ret != AR_HAL_VO_SUCCESS)
				goto End;*/
		}
	}

   End:
    if (ret != AR_HAL_VO_SUCCESS)
        PRINT_ERR("ret = 0x%x\r\n", ret);

    return ret;
}

#if 0
static yuv_t rgb2yuv(rgb_t rgb)
{
   int yy,uu,vv;
   yuv_t yuv;
   yy=0.299*rgb.r+0.587*rgb.g+0.114*rgb.b;
   if(yy>255)
      yy=255;
   if(yy<0)
      yy=0;
   uu=128-0.168736*rgb.r-0.331264*rgb.g+0.5*rgb.b;
   if(uu>255)
      uu=255;
   if(uu<0)
      uu=0;
   vv=128+0.5*rgb.r-0.418688*rgb.g-0.081312*rgb.b;
   if(vv>255)
      vv=255;
   if(vv<0)
      vv=0;
   yuv.y=yy;
   yuv.u=uu;
   yuv.v=vv;
   return yuv;

}
#endif
static AR_S32 buffer_get(AR_VB_POOL u32_pool_id, AR_S32 width, AR_S32 height, AR_S32 y_stride, STRU_ADDR *addr)
{
    if ((addr->vb_blk = ar_hal_vb_get_block(u32_pool_id, y_stride*height*3/2, NULL)) == AR_VB_INVALID_HANDLE)
	{
		PRINT_ERR("ar_hal_vb_get_block failed!\n");
        return AR_FAILURE;
	}

	if ((addr->y_phy = (AR_VOID *)ar_hal_vb_handle2physaddr(addr->vb_blk)) == NULL)
	{
		ar_hal_vb_release_block(addr->vb_blk);
        return AR_FAILURE;
	}

	if (ar_hal_vb_get_blockviraddr(u32_pool_id, (AR_U64)addr->y_phy, &addr->y_vrt) != AR_SUCCESS)
	{
		ar_hal_vb_release_block(addr->vb_blk);
        return AR_FAILURE;
	}

	addr->u_phy = (AR_VOID *)((AR_U64)addr->y_phy + y_stride*height);
    addr->v_phy = (AR_VOID *)((AR_U64)addr->u_phy + y_stride*height/4);

	addr->u_vrt = (AR_VOID *)((AR_U64)addr->y_vrt + y_stride*height);
    addr->v_vrt = (AR_VOID *)((AR_U64)addr->u_vrt + y_stride*height/4);

    /*PRINT_INFO("y_addr(%p %p), u_addr(%p %p), v_addr(%p %p),\r\n",
                                              addr->y_phy, addr->y_vrt,
                                              addr->u_phy, addr->u_vrt,
                                              addr->v_phy, addr->v_vrt);*/
    return AR_SUCCESS;
}

static void buffer_release(STRU_ADDR *addr)
{
    ar_hal_vb_release_block(addr->vb_blk);
}

void *recycle_frame_func(STRU_VO_CHN_THREAD_PARA          *thread_info)
{
	STRU_AR_HAL_VO_DISP_BUF buffer;
	int channel_fd;
	int ret;
	STRU_ADDR* src_addr;

	channel_fd = thread_info->chn_fd;

	ar_queue_id_t src_addr_que = thread_info->layer_id == 0 ? vo_obj.video_frame_queue[thread_info->chn_id] : vo_obj.overlay_video_frame_queue[thread_info->layer_id-1][thread_info->chn_id];

	while(is_running)
	{
        if(cmd_is_running[thread_info->layer_id][thread_info->chn_id])
        {
            ret = ar_hal_vo_chn_get_frame(channel_fd, &buffer, 500);

            if (ret == AR_HAL_VO_SUCCESS)
            {
                src_addr = (STRU_ADDR *)buffer.usr_data;

                buffer_release(src_addr);
                if (ar_queue_push(src_addr_que, &src_addr))
                    PRINT_ERR("chan(%d_%d) insert failed!\n", thread_info->layer_id, thread_info->chn_id);

                /* release buffer */
                ret = ar_hal_vo_chn_release_frame(channel_fd, &buffer);
                if (ret != AR_HAL_VO_SUCCESS)
                    PRINT_ERR("release frame failed,layer_id:%d chn_id:%d, ret: %d\r\n", thread_info->layer_id, thread_info->chn_id, ret);
            }
            //else
                //PRINT_ERR("layer_%d chn_%d get frame failed, %d\r\n", thread_info->layer_id, thread_info->chn_id, ret);

            //printf("recycle lay_id:%d chan_id:%d src_addr_que:%p ar_queue_element_count:%d\n", thread_info->layer_id, thread_info->chn_id, src_addr_que, ar_queue_element_count(src_addr_que));
        }
        else
            usleep(500*1000);

    }

    ar_thread_exit();

    return 0;
}
void *send_frame_func(STRU_VO_CHN_THREAD_PARA        *thread_info)
{
	CFG_VO_S *cfg_vo = NULL;
	int chan_id, layer_id;
	AR_S32 width;
	AR_S32 height;
	AR_S32 y_stride;
	AR_S32 uv_stride;
	STRU_AR_HAL_VO_DISP_BUF buffer;
	AR_U32 frame_id = 0;
	int ret;
	uint64_t tm = 0;
	uint64_t delay_ms = 0;
	float frame_rate_ms;

	cfg_vo = CFG_VO_GetParam();
	chan_id = thread_info->chn_id;
	layer_id = thread_info->layer_id;

	width = cfg_vo->stVoChn[chan_id].image_width;
	height = cfg_vo->stVoChn[chan_id].image_height;
	y_stride = ALIGN_UP(width, 512);
	uv_stride = ALIGN_UP(width / 2, 256);

	frame_rate_ms =  1000/cfg_vo->stVoDev.fps;

	AR_VB_POOL src_pool_id = vo_obj.src_pool_id;
	ar_queue_id_t src_addr_que = (layer_id == 0 ? vo_obj.video_frame_queue[chan_id] : vo_obj.overlay_video_frame_queue[layer_id-1][chan_id]);

	while(is_running)
	{
		tm = ar_get_linux_time();

        if(cmd_is_running[thread_info->layer_id][thread_info->chn_id])
        {
            STRU_ADDR *src_addr;
            if ((ret = ar_queue_pop(src_addr_que, &src_addr)) != AR_HAL_VO_SUCCESS)
            {
                PRINT_ERR("chan(%d_%d) video_frame queue empty\n", layer_id, chan_id);
                continue;
            }

            if (buffer_get(src_pool_id, width, height, y_stride, src_addr) != AR_SUCCESS)
            {
                PRINT_ERR("buffer_get failed, send layer_id:%d chan_id:%d\n", layer_id, chan_id);
                if(ar_queue_push(src_addr_que, &src_addr))
                    PRINT_ERR(" chan(%d_%d) video_frame queue push failed\n", layer_id, chan_id);
                ar_delay(1/cfg_vo->stVoDev.fps*1000);
                continue;
            }

            //printf("send layer_id:%d chan_id:%d, src_addr_que:%p ar_queue_element_count:%d\n", layer_id, chan_id, src_addr_que, ar_queue_element_count(src_addr_que));

            vo_read_oneframe(cfg_vo->stVoChn[chan_id].imagepath, src_addr->y_vrt, src_addr->u_vrt, src_addr->v_vrt,
                             width, height, y_stride, uv_stride, cfg_vo->stVoDev.format);
#if 0
    		if ((i/10)%2)
    		{
    			memset(vo_obj.src_addr.u_vrt, 0x80, uv_stride*height/2);
    			memset(vo_obj.src_addr.v_vrt, 0x80, uv_stride*height/2);
    		}
#endif

            memset(&buffer, 0, sizeof(buffer));
            buffer.frame_id = frame_id++;
            buffer.format = cfg_vo->stVoDev.format;
            buffer.frame_width = width;
            buffer.frame_height = height;
            buffer.luma_stride = y_stride;
            buffer.chroma_stride = uv_stride;
            buffer.pannel[0].buffer_pa = src_addr->y_phy;
            buffer.pannel[0].buffer = src_addr->y_vrt;
            buffer.pannel[0].length = y_stride * height;
            if (AR_HAL_VO_FMT_YV12 == cfg_vo->stVoDev.format)
            {
                buffer.pannel_num = 3;
                buffer.pannel[1].buffer_pa = src_addr->u_phy;
                buffer.pannel[2].buffer_pa = src_addr->v_phy;
                buffer.pannel[1].buffer = src_addr->u_vrt;
                buffer.pannel[2].buffer = src_addr->v_vrt;
                buffer.pannel[1].length = y_stride * height / 4;
                buffer.pannel[2].length = y_stride * height / 4;
            }
            else
            {
                buffer.pannel_num = 1;
            }
            buffer.interlace_filed_flag = cfg_vo->stVoDev.is_interlace;
            buffer.pts = 0;
            buffer.usr_data = (AR_VOID *)src_addr;

            /* send to channel */
            ret = ar_hal_vo_chn_send_frame(thread_info->chn_fd, &buffer, 0);
            if (ret != AR_HAL_VO_SUCCESS)
            {
                buffer_release(src_addr);
                if(ar_queue_push(src_addr_que, &src_addr))
                    PRINT_ERR(" chan(%d_%d) video_frame queue push failed\n", layer_id, chan_id);
                PRINT_ERR("send frame failed %d\r\n", ret);
            }

            delay_ms = ar_get_linux_time() - tm;	/* impossible overflow*/
            delay_ms = (delay_ms < frame_rate_ms)?(frame_rate_ms-delay_ms):1;
            usleep(delay_ms*1000);
        }
        else
            usleep(500*1000);

	}

	ar_thread_exit();

	return 0;
}

void recyle_channel_buffer(int        layer_id, int chn_id)
{
    STRU_ADDR * src_addr;
    STRU_AR_HAL_VO_DISP_BUF buffer;
    int fd = -1;
	int ret = -1;

    if(0 ==layer_id)
    {
        fd = vo_obj.chan_fd[chn_id];
    }
    else
    {
        fd = vo_obj.overlay_chan_fd[layer_id-1][chn_id];
    }

    if(fd < 0)
        return;

    ar_hal_vo_chn_clear_frame(fd, AR_TRUE);
    while(1)
    {
        memset(&buffer, 0, sizeof(buffer));
        ret = ar_hal_vo_chn_get_frame(fd, &buffer, 0);
        if (ret != AR_HAL_VO_SUCCESS)
        {
            //PRINT_ERR("ar_hal_vo_chn_get_frame failed %d\n", ret);
            break;
        }

        src_addr = (STRU_ADDR *)buffer.usr_data;

        buffer_release(src_addr);
        if (ar_queue_push(vo_obj.video_frame_queue[chn_id], &src_addr))
            PRINT_ERR("chan(%d_%d) insert failed!\n", 0, chn_id);

        ret = ar_hal_vo_chn_release_frame(fd, &buffer);
        if (ret != AR_HAL_VO_SUCCESS)
        {
            //PRINT_ERR("ar_hal_vo_chn_release_frame failed %d\n", ret);
            break;
        }
    }
    printf("recycle frame, queue_element_count:%d\n", ar_queue_element_count(vo_obj.video_frame_queue[chn_id]));

}

int vo_test(int argc, char *argv[])
{
	AR_U32 ch_map;
	AR_S32 ret;
	STRU_ADDR * src_addr;

	STRU_VO_CHN_THREAD_PARA thread_para[MAX_VO_CHN_NUM*AR_HAL_VO_LAYER_ID_MAX] = {0};

    if(ar_hal_sys_mpp_service_is_lib()){
        mpp_service_init();
    }

	CFG_VO_Load(argv[1]);
	//CFG_VO_Print();
	cfg_vo = CFG_VO_GetParam();

	ch_map = 0x1ff;
	if(is_use_senior_paras(argc,argv)){
		parse_paras(argc, argv);
		ch_map = get_vo_ext_paras()->layer_ch_map;
	}else{
		parse_opts(argc, argv, cfg_vo, &ch_map);
		get_vo_ext_paras()->layer_ch_map = ch_map;
	}
	printf("ch_map1 = 0x%x\n", ch_map);

	ar_hal_log_init();

	memset(&vo_obj, 0, sizeof(vo_obj));

	print_all_paras();

	if(00 != ch_map)
	{
		if ((ret = sys_init(&vo_obj, cfg_vo)) != AR_SUCCESS)
			return ret;
	}

    ret = vo_dev_init(&vo_obj, cfg_vo);
    if (ret != AR_HAL_VO_SUCCESS)
        goto __end_1;

    ret = ar_vo_channel_init(&vo_obj, cfg_vo);
    if (ret != AR_HAL_VO_SUCCESS)
        goto __end_0;

    votest_rpc_init();

	for(int chan_id = 0; chan_id < CFG_MAX_VO_CHN_NUM; chan_id++)
	{
		if (!cfg_vo->stVoChn[chan_id].enable || !(ch_map & (1 << chan_id)))
			continue;

		if ((strlen(cfg_vo->stVoChn[chan_id].imagepath) == 0) || (cfg_vo->stVoChn[chan_id].imagepath[0] == '\0'))
		{
			PRINT_ERR("channel %d imagepath is null\n", chan_id);
			continue;
		}

		//video layer
		thread_para[chan_id].layer_id = 0;
		thread_para[chan_id].chn_id = chan_id;
		thread_para[chan_id].chn_fd = vo_obj.chan_fd[chan_id];

        cmd_is_running[0][chan_id] = AR_TRUE;

		vo_obj.thread_id_send[chan_id] = ar_thread_new((ar_thread_func_t)recycle_frame_func,
													&thread_para[chan_id], 0, NULL);
		//printf("++++thread_id_send[%d]:%p\n", chan_id, vo_obj.thread_id_send[chan_id]);
		vo_obj.thread_id_recycle[chan_id] = ar_thread_new((ar_thread_func_t)send_frame_func,
													&thread_para[chan_id], 0, NULL);
		//printf("++++thread_id_recycle[%d]:%p\n", chan_id, vo_obj.thread_id_recycle[chan_id]);

		for (int i = 0; i < cfg_vo->stVoDev.overlayer_num; i++)
		{
			//over layer
			thread_para[chan_id + (i+1)*MAX_VO_CHN_NUM].layer_id = i+1;
			thread_para[chan_id + (i+1)*MAX_VO_CHN_NUM].chn_id = chan_id;
			thread_para[chan_id + (i+1)*MAX_VO_CHN_NUM].chn_fd = vo_obj.overlay_chan_fd[i][chan_id];

            cmd_is_running[i+1][chan_id] = AR_TRUE;

			vo_obj.thread_id_send[chan_id + (i+1)*MAX_VO_CHN_NUM] = ar_thread_new((ar_thread_func_t)recycle_frame_func,
														&thread_para[chan_id + (i+1)*MAX_VO_CHN_NUM], 0, NULL);

			vo_obj.thread_id_recycle[chan_id + (i+1)*MAX_VO_CHN_NUM] = ar_thread_new((ar_thread_func_t)send_frame_func,
														&thread_para[chan_id + (i+1)*MAX_VO_CHN_NUM], 0, NULL);

		}

	}


	while(is_running)
		sleep(1);

__end_0:
	for (int chan_id = 0; chan_id < CFG_MAX_VO_CHN_NUM; chan_id++)
	{
		if (!cfg_vo->stVoChn[chan_id].enable  || !(ch_map & (1 << chan_id)))
			continue;

		if (vo_obj.chan_fd[chan_id] >= 0 )
		{
			//printf("=== vo_obj.thread_id_send[%d]:%p\n", chan_id, vo_obj.thread_id_send[chan_id]);
			//printf("=== vo_obj.thread_id_recycle[%d]:%p\n", chan_id, vo_obj.thread_id_recycle[chan_id]);
			if((ch_map & (1 << chan_id)))
			{
				ar_thread_join(vo_obj.thread_id_send[chan_id]);
				ar_thread_join(vo_obj.thread_id_recycle[chan_id]);

                /* pause the channel */
                ret = ar_hal_vo_chn_pause(vo_obj.chan_fd[chan_id]);
                if (ret != AR_HAL_VO_SUCCESS)
                {
                    break;
                }

                recyle_channel_buffer(0, chan_id);

				while(1)
				{
					if ((ret = ar_queue_pop(vo_obj.video_frame_queue[chan_id], &src_addr)) != AR_SUCCESS)
					{
						break;
					}
					free(src_addr);
					src_addr = NULL;
				}
			}

			ar_hal_vo_chn_disable(vo_obj.chan_fd[chan_id]);
			ar_hal_vo_fd_close(vo_obj.chan_fd[chan_id]);
		}

		for (int i = 0; i < cfg_vo->stVoDev.overlayer_num; i++)
		{
			if (vo_obj.overlay_chan_fd[i][chan_id] >= 0)
			{
				ar_thread_join(vo_obj.thread_id_send[chan_id + (i+1)*MAX_VO_CHN_NUM]);
				ar_thread_join(vo_obj.thread_id_recycle[chan_id + (i+1)*MAX_VO_CHN_NUM]);

                /* pause the channel */
                ret = ar_hal_vo_chn_pause(vo_obj.overlay_chan_fd[i][chan_id]);
                if (ret != AR_HAL_VO_SUCCESS)
                {
                    break;
                }

                recyle_channel_buffer(i+1, chan_id);

				while(1)
				{
					if ((ret = ar_queue_pop(vo_obj.overlay_video_frame_queue[i][chan_id], &src_addr)) != AR_SUCCESS)
					{
						break;
					}
					free(src_addr);
					src_addr = NULL;
				}

				ar_hal_vo_chn_disable(vo_obj.overlay_chan_fd[i][chan_id]);
				ar_hal_vo_fd_close(vo_obj.overlay_chan_fd[i][chan_id]);
			}
		}
	}

    votest_rpc_deinit();

__end_1:

	ar_hal_vo_layer_disable(vo_obj.layer_fd);
	ar_hal_vo_fd_close(vo_obj.layer_fd);

	for (int i = 0; i < cfg_vo->stVoDev.overlayer_num; i++)
	{
		ar_hal_vo_layer_disable(vo_obj.overlayer_fd[i]);
		ar_hal_vo_fd_close(vo_obj.overlayer_fd[i]);
	}

	ar_hal_vo_dev_disable(vo_obj.dev_fd);
	ar_hal_vo_fd_close(vo_obj.dev_fd);

	sys_deinit(&vo_obj);

    if(ar_hal_sys_mpp_service_is_lib()){
        mpp_service_deinit();
    }

	PRINT_INFO("vo_test exit\n");
    return AR_SUCCESS;
}

void vo_handle_sig(AR_S32 signo)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if (SIGINT == signo || SIGTERM == signo) {
        PRINT_INFO("\033[0;31mprogram termination abnormally!\033[0;39m\n");
		is_running = false;
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, vo_handle_sig);
    signal(SIGTERM, vo_handle_sig);

	if (argc < 2 || !is_help(argc, argv, "--help", "-h"))
	{
		printf("use --help1 or -h1 to get details of simple para settings.\n");
		printf("use --help2 or -h2 to get details of senior para settings.\n");
		printf("use --help3 or -h3 to get details of some template case.\n");
		return AR_FAILURE;
    }

	if(!is_help(argc, argv, "--help1", "-h1"))
	{
		print_usage(argv[0]);
		return 0;
	}

	if(!is_help(argc, argv, "--help2", "-h2"))
	{
		print_senior_usage(argv[0]);
		return 0;
	}

	if(!is_help(argc, argv, "--help3", "-h3"))
	{
		printf("To do..\n");
		return 0;
	}

	vo_test(argc, argv);

    return 0;
}
