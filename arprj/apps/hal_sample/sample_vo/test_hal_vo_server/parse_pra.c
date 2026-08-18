#include "parse_pra.h"

CFG_VO_EXT_S g_stVoExtCfg={0};

static int find_pra_index_by_name(char *name, int start, int argc, char *argv[])
{
    int i=0;
	for(i=start; i<argc; i++)
	{
         if(!strcmp(name,argv[i]))
         {
            //printf("find pra %s @ %d\n",name,i);
            return i;
         }
	}
	return 0;
}

static int find_pra_counts_by_name(char *name, int argc, char *argv[])
{
    int i=0;
	int counts=0;
	for(i=0; i<argc; i++)
	{
         if(!strcmp(name,argv[i]))
         {
            //printf("find pra %s @ %d\n",name,i);
            counts++;
         }
	}

	return counts;
}

void print_senior_usage(const char *prog)
{
	printf("Usage2:---The following command to set more parameters. if not set, the default is used.\n");
	printf(" %s path/vo_cfg.json [...]\n", prog);
	puts("--set_dev_pra                   Set device parameters.\n"
	     "  -dev_dev_id                   set device id\n"
	     "  -dev_bg_color                 set background color of a device\n"
	     "  -dev_interface                set vo interface type\n"
	     "  -dev_sub_interface            set vo interface type\n"
	     "  -dev_timing_template          set timing tempalate type\n"
	     "  -dev_dpi_format               set dpi format\n"
	     "  -dev_color_mode               set color mode\n"
	     "  -dev_sync_mode                set sync mode\n"
	     "  -dev_pin_mode                 set pin mode\n"
	     "  -dev_csc                      set device css\n"
	     "  -dev_interlace_filed_flag     set interlace filed flag\n"
	     "  -dev_bit_count_per_channel    set bit count per channel\n");

	puts("--set_sync_pra                  Set sync parameters, if timing_template is set to AR_HAL_VO_OUTPUT_USER.\n"
	     "  -syn_interlace_mod            set interlace mod\n"
	     "  -syn_hpw                      set hpw\n"
	     "  -syn_hbp                      set hbp\n"
	     "  -syn_hdp                      set hdp\n"
	     "  -syn_hfp                      set hfp\n"
	     "  -syn_vpw                      set vpw\n"
	     "  -syn_vbp                      set vpw\n"
	     "  -syn_vdp                      set vdp\n"
	     "  -syn_vfp                      set vfp\n"
	     "  -syn_fps                      set sync fps\n"
	     "  -syn_de_polarity              set de_polarity\n"
	     "  -syn_da_polarity              set da_polarity\n"
	     "  -syn_clock_polarity           set clock_polarity\n"
	     "  -syn_hsync_polarity           set hsync_polarity\n"
	     "  -syn_vsync_polarity           set vsync_polarity\n");

	puts("--set_layer_pra                 Set multiple layers parameters by multiple use.\n"
	     "  -layer_id                     set layer id\n"
	     "  -lay_overlayer_num            set overlayer num\n"
	     "  -layer_width                  set layer width\n"
	     "  -layer_height                 set layer height\n"
	     "  -layer_luma_stride            set luma stride\n"
	     "  -layer_chroma_stride          set chroma stride\n"
	     "  -layer_format                 set layer format\n"
	     "  -layer_fps                    set layer fps\n"
	     "  -layer_rotation               set rotation\n"
	     "  -layer_csc_in                 set layer csc of input\n"
	     "  -layer_csc_out                set layer csc of output\n"
	     "  -layer_ch_map                 set chanel map\n"
	     "  -overlay_posx                 set overlay posx\n"
	     "  -overlay_posy                 set overlay posy\n"
	     );

	puts("--set_cursor_pra                Set cursor parameters, default is diabale.\n"
	     "  -cursor_file_path             set cursor file path\n"
	     "  -cursor_posx                  set cursor posx\n"
	     "  -cursor_posy                  set cursor posy\n");

	puts("--set_channel_pra               Set multiple channels parameters by multiple use.\n"
	     "  -ch_id                        set channel id\n"
	     "  -ch_priority                  set channel priority\n"
	     "  -ch_bg_color                  set background color\n"
	     "  -ch_width                     set channel width\n"
	     "  -ch_height                    set channel height\n"
	     "  -ch_pos_x                     set channel positon of x\n"
	     "  -ch_pos_y                     set channel positon of y\n"
	     "  -ch_aspect_ratio_mode         set aspect ratio mode\n"
	     "  -ch_aspect_ratio_width        set aspect ratio width\n"
	     "  -ch_aspect_ratio_height       set aspect ratio height\n"
	     "  -ch_aspect_ratio_pos_x        set aspect ratio pos of x\n"
	     "  -ch_aspect_ratio_pos_y        set aspect ratio pos of y\n"
	     "  -ch_image_width               set channel input image width\n"
	     "  -ch_image_height              set channel input image height\n"
	     "  -ch_image_path                set channel input image path\n");
}

void parse_dev_paras(int start, int argc, char *argv[])
{
	int pra_index;
	pra_index = find_pra_index_by_name("-dev_dev_id", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.dev_id = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-dev_bg_color", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.bg_color = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-dev_interface", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.interface = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-dev_sub_interface", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.sub_interface = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-dev_timing_template", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.timing_template = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-dev_dpi_format", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_dpi_format = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-dev_color_mode", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_color_mode = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-dev_sync_mode", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_mode = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-dev_pin_mode", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_pin_mode = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-dev_csc", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.dev_csc = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-dev_interlace_filed_flag", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.is_interlace = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-dev_bit_count_per_channel", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.bit_count_per_channel = atoi(argv[pra_index + 1]);
	}

}

void parse_sync_paras(int start, int argc, char *argv[])
{
	int pra_index;

	pra_index = find_pra_index_by_name("-syn_interlace_mod", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.interlace_mod = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_hpw", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.hpw = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_hbp", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.hbp = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_hdp", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.hdp = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_hfp", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.hfp = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_vpw", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.vpw = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_vbp", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.vbp = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_vdp", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.vdp = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_vfp", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.vfp = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_fps", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.fps = atof(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_de_polarity", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.de_polarity = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_da_polarity", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.da_polarity = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_clock_polarity", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.clock_polarity = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_hsync_polarity", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.hsync_polarity = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-syn_vsync_polarity", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.dev_sync_info.vsync_polarity = atoi(argv[pra_index + 1]);
	}

}

void parse_layer_paras(int start, int argc, char *argv[])
{
	int pra_index;

	pra_index = find_pra_index_by_name("-layer_id", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.layer_id = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-lay_overlayer_num", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.overlayer_num = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-layer_width", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.width = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-layer_height", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.height = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-layer_luma_stride", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.layer_luma_stride  = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-layer_chroma_stride", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.layer_chroma_stride = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-layer_format", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.format = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-layer_fps", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.fps = atof(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-layer_rotation", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.layer_rotation = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-layer_csc_in", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.layer_csc_in = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-layer_csc_out", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.layer_csc_out = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-layer_ch_map", start, argc, argv);
	if(pra_index>0){
		g_stVoExtCfg.layer_ch_map = strtol(argv[pra_index + 1], NULL, 16);
	}

	pra_index = find_pra_index_by_name("-overlay_posx", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.pos_x = atoi(argv[pra_index + 1]);
	}

	pra_index = find_pra_index_by_name("-overlay_posy", start, argc, argv);
	if(pra_index>0){
		CFG_VO_GetParam()->stVoDev.pos_y = atoi(argv[pra_index + 1]);
	}

}

void parse_channel_paras(int start, int argc, char *argv[])
{
	int pra_index;

	pra_index = find_pra_index_by_name("-ch_id", start, argc, argv);
	if(pra_index>0){
		int chanel_id =  atoi(argv[pra_index + 1]);
		if(chanel_id <= CFG_MAX_VO_CHN_NUM){

			CFG_VO_GetParam()->stVoChn[chanel_id].enable = 1;

			pra_index = find_pra_index_by_name("-ch_priority", start, argc, argv);
			if(pra_index>0){
				CFG_VO_GetParam()->stVoChn[chanel_id].priority = atoi(argv[pra_index + 1]);
			}

			pra_index = find_pra_index_by_name("-ch_bg_color", start, argc, argv);
			if(pra_index>0){
				CFG_VO_GetParam()->stVoChn[chanel_id].bg_color = atoi(argv[pra_index + 1]);
			}

			pra_index = find_pra_index_by_name("-ch_width", start, argc, argv);
			if(pra_index>0){
				CFG_VO_GetParam()->stVoChn[chanel_id].ch_width = atoi(argv[pra_index + 1]);
			}

			pra_index = find_pra_index_by_name("-ch_height", start, argc, argv);
			if(pra_index>0){
				CFG_VO_GetParam()->stVoChn[chanel_id].ch_height = atoi(argv[pra_index + 1]);
			}

			pra_index = find_pra_index_by_name("-ch_pos_x", start, argc, argv);
			if(pra_index>0){
				CFG_VO_GetParam()->stVoChn[chanel_id].ch_pos_x = atoi(argv[pra_index + 1]);
			}

			pra_index = find_pra_index_by_name("-ch_pos_y", start, argc, argv);
			if(pra_index>0){
				CFG_VO_GetParam()->stVoChn[chanel_id].ch_pos_y = atoi(argv[pra_index + 1]);
			}

			pra_index = find_pra_index_by_name("-ch_aspect_ratio_mode", start, argc, argv);
			if(pra_index>0){
				CFG_VO_GetParam()->stVoChn[chanel_id].aspect_ratio_mode = atoi(argv[pra_index + 1]);
			}

			pra_index = find_pra_index_by_name("-ch_aspect_ratio_width", start, argc, argv);
			if(pra_index>0){
				CFG_VO_GetParam()->stVoChn[chanel_id].aspect_ratio_width = atoi(argv[pra_index + 1]);
			}

			pra_index = find_pra_index_by_name("-ch_aspect_ratio_height", start, argc, argv);
			if(pra_index>0){
				CFG_VO_GetParam()->stVoChn[chanel_id].aspect_ratio_height = atoi(argv[pra_index + 1]);
			}

			pra_index = find_pra_index_by_name("-ch_aspect_ratio_pos_x", start, argc, argv);
			if(pra_index>0){
				CFG_VO_GetParam()->stVoChn[chanel_id].aspect_ratio_pos_x = atoi(argv[pra_index + 1]);
			}

			pra_index = find_pra_index_by_name("-ch_aspect_ratio_pos_y", start, argc, argv);
			if(pra_index>0){
				CFG_VO_GetParam()->stVoChn[chanel_id].aspect_ratio_pos_y = atoi(argv[pra_index + 1]);
			}

			pra_index = find_pra_index_by_name("-ch_image_width", start, argc, argv);
			if(pra_index>0){
				CFG_VO_GetParam()->stVoChn[chanel_id].image_width = atoi(argv[pra_index + 1]);
			}

			pra_index = find_pra_index_by_name("-ch_image_height", start, argc, argv);
			if(pra_index>0){
				CFG_VO_GetParam()->stVoChn[chanel_id].image_height = atoi(argv[pra_index + 1]);
			}

			pra_index = find_pra_index_by_name("-ch_image_path", start, argc, argv);
			if(pra_index>0){
				strncpy(CFG_VO_GetParam()->stVoChn[chanel_id].imagepath, argv[pra_index + 1], 255);
				CFG_VO_GetParam()->stVoChn[chanel_id].imagepath[255] = 0;
			}

		}

	}

}

void print_all_paras()
{
	int i;

	printf("dev paras:\n");
	printf("  dev_dev_id: %d\n", CFG_VO_GetParam()->stVoDev.dev_id);
	printf("  dev_bg_color: 0x%02x\n", CFG_VO_GetParam()->stVoDev.bg_color);
	printf("  dev_interface: %d\n", CFG_VO_GetParam()->stVoDev.interface);
	printf("  dev_sub_interface: %d\n", CFG_VO_GetParam()->stVoDev.sub_interface);
	printf("  dev_timing_template: %d\n", CFG_VO_GetParam()->stVoDev.timing_template);
	printf("  dev_dpi_format: %d\n", g_stVoExtCfg.dev_dpi_format);
	printf("  dev_color_mode: %d\n", g_stVoExtCfg.dev_color_mode);
	printf("  dev_sync_mode: %d\n", g_stVoExtCfg.dev_sync_mode);
	printf("  dev_pin_mode: %d\n", g_stVoExtCfg.dev_pin_mode);
	printf("  dev_csc: %d\n", CFG_VO_GetParam()->stVoDev.dev_csc);
	printf("  dev_interlace_filed_flag: %d\n", CFG_VO_GetParam()->stVoDev.is_interlace);
	printf("  dev_bit_count_per_channel: %d\n", CFG_VO_GetParam()->stVoDev.bit_count_per_channel);

	if(AR_HAL_VO_OUTPUT_USER == CFG_VO_GetParam()->stVoDev.timing_template){
		printf("sync paras:\n");
		printf("  syn_interlace_mod:%d\n", g_stVoExtCfg.dev_sync_info.interlace_mod);
		printf("  syn_hpw:%d\n", g_stVoExtCfg.dev_sync_info.hpw);
		printf("  syn_hbp:%d\n", g_stVoExtCfg.dev_sync_info.hbp);
		printf("  syn_hdp:%d\n", g_stVoExtCfg.dev_sync_info.hdp);
		printf("  syn_hfp:%d\n", g_stVoExtCfg.dev_sync_info.hfp);
		printf("  syn_vpw:%d\n", g_stVoExtCfg.dev_sync_info.vpw);
		printf("  syn_vbp:%d\n", g_stVoExtCfg.dev_sync_info.vbp);
		printf("  syn_vdp:%d\n", g_stVoExtCfg.dev_sync_info.vdp);
		printf("  syn_vfp:%d\n", g_stVoExtCfg.dev_sync_info.vfp);
		printf("  syn_fps:%f\n", g_stVoExtCfg.dev_sync_info.fps);
		printf("  syn_de_polarity:%d\n", g_stVoExtCfg.dev_sync_info.de_polarity);
		printf("  syn_da_polarity:%d\n", g_stVoExtCfg.dev_sync_info.da_polarity);
		printf("  syn_clock_polarity:%d\n", g_stVoExtCfg.dev_sync_info.clock_polarity);
		printf("  syn_hsync_polarity:%d\n", g_stVoExtCfg.dev_sync_info.hsync_polarity);
		printf("  syn_vsync_polarity:%d\n", g_stVoExtCfg.dev_sync_info.vsync_polarity);
	}

	printf("layer paras:\n");
	printf("  layer_id:%d\n", CFG_VO_GetParam()->stVoDev.layer_id);
	printf("  lay_overlayer_num:%d\n", CFG_VO_GetParam()->stVoDev.overlayer_num);
	printf("  layer_width:%d\n", CFG_VO_GetParam()->stVoDev.width);
	printf("  layer_luma_stride:%d\n", g_stVoExtCfg.layer_luma_stride);
	printf("  layer_chroma_stride:%d\n", g_stVoExtCfg.layer_chroma_stride);
	printf("  layer_format:%d\n", CFG_VO_GetParam()->stVoDev.format);
	printf("  layer_fps:%f\n", CFG_VO_GetParam()->stVoDev.fps);
	printf("  layer_rotation:%d\n", g_stVoExtCfg.layer_rotation);
	printf("  layer_csc_in:%d\n", CFG_VO_GetParam()->stVoDev.layer_csc_in);
	printf("  layer_csc_out:%d\n", CFG_VO_GetParam()->stVoDev.layer_csc_out);
	printf("  layer_ch_map:%d\n", g_stVoExtCfg.layer_ch_map);
	if(CFG_VO_GetParam()->stVoDev.overlayer_num > 0){
		printf("  overlay_posx:%d\n", CFG_VO_GetParam()->stVoDev.pos_x);
		printf("  overlay_posy:%d\n", CFG_VO_GetParam()->stVoDev.pos_y);
	}

	if(CFG_VO_GetParam()->stVoDev.cursor_enable){
		printf("cursor paras:\n");
		printf("  cursor_file_path:%s\n", CFG_VO_GetParam()->stVoDev.cursorpath);
		printf("  cursor_pos_x:%d\n", CFG_VO_GetParam()->stVoDev.cursor_pos_x);
		printf("  cursor_pos_y:%d\n", CFG_VO_GetParam()->stVoDev.cursor_pos_y);
	}

	printf("channel paras:\n");
	for(i=0; i<CFG_MAX_VO_CHN_NUM; ++i){
		if(!CFG_VO_GetParam()->stVoChn[i].enable || !(g_stVoExtCfg.layer_ch_map & (1 << i)))
			continue;

		printf("  ch_id:%d\n", i);
		printf("  ch_priority:%d\n", CFG_VO_GetParam()->stVoChn[i].priority);
		printf("  ch_bg_color:0x%02x\n", CFG_VO_GetParam()->stVoChn[i].bg_color);
		printf("  ch_width:%d\n", CFG_VO_GetParam()->stVoChn[i].ch_width);
		printf("  ch_height:%d\n", CFG_VO_GetParam()->stVoChn[i].ch_height);
		printf("  ch_pos_x:%d\n", CFG_VO_GetParam()->stVoChn[i].ch_pos_x);
		printf("  ch_pos_y:%d\n", CFG_VO_GetParam()->stVoChn[i].ch_pos_y);
		printf("  ch_aspect_ratio_mode:%d\n", CFG_VO_GetParam()->stVoChn[i].aspect_ratio_mode);
		printf("  ch_aspect_ratio_width:%d\n", CFG_VO_GetParam()->stVoChn[i].aspect_ratio_width);
		printf("  ch_aspect_ratio_height:%d\n", CFG_VO_GetParam()->stVoChn[i].aspect_ratio_height);
		printf("  ch_aspect_ratio_pos_x:%d\n", CFG_VO_GetParam()->stVoChn[i].aspect_ratio_pos_x);
		printf("  ch_aspect_ratio_pos_y:%d\n", CFG_VO_GetParam()->stVoChn[i].aspect_ratio_pos_y);
		printf("  ch_image_width:%d\n", CFG_VO_GetParam()->stVoChn[i].image_width);
		printf("  ch_image_height:%d\n", CFG_VO_GetParam()->stVoChn[i].image_height);
		printf("  ch_image_path:%s\n", CFG_VO_GetParam()->stVoChn[i].imagepath);
	}

}
void parse_paras(int argc, char *argv[])
{
	int pra_index, layer_counts, channel_counts;
	int i;

	layer_counts = find_pra_counts_by_name("--set_layer_pra",argc,argv);

	channel_counts = find_pra_counts_by_name("--set_channel_pra",argc,argv);

	pra_index = find_pra_index_by_name("--set_dev_pra", 0, argc, argv);
	if(pra_index > 0){
		parse_dev_paras(pra_index+1, argc, argv);
	}

	pra_index = find_pra_index_by_name("--set_sync_pra",0, argc, argv);
	if(pra_index > 0){
		parse_sync_paras(pra_index+1, argc, argv);
	}

	//to do, now only video layers
	for(i=0; i<layer_counts; i++)
	{
		pra_index = find_pra_index_by_name("--set_layer_pra", i==0 ? 0 : pra_index+1, argc, argv);
		if(pra_index > 0){
			parse_layer_paras(pra_index+1, argc, argv);
		}
	}

	for(i=0; i<channel_counts; i++)
	{
		pra_index = find_pra_index_by_name("--set_channel_pra", i==0 ? 0 : pra_index+1, argc, argv);
		if(pra_index > 0){
			parse_channel_paras(pra_index+1, argc, argv);
		}
	}

	pra_index = find_pra_index_by_name("--set_cursor_pra", 0, argc, argv);
	if(pra_index > 0){
		CFG_VO_GetParam()->stVoDev.cursor_enable = 1;
		pra_index = find_pra_index_by_name("-cursor_file_path", pra_index+1, argc, argv);
		if(pra_index>0){
			strncpy(CFG_VO_GetParam()->stVoDev.cursorpath, argv[pra_index + 1], 255);
			CFG_VO_GetParam()->stVoDev.cursorpath[255] = 0;
		}

		pra_index = find_pra_index_by_name("-cursor_posx", pra_index+1, argc, argv);
		if(pra_index>0){
			CFG_VO_GetParam()->stVoDev.cursor_pos_x = atoi(argv[pra_index + 1]);
		}

		pra_index = find_pra_index_by_name("-cursor_posy", pra_index+1, argc, argv);
		if(pra_index>0){
			CFG_VO_GetParam()->stVoDev.cursor_pos_y = atoi(argv[pra_index + 1]);
		}
	}

}

int is_use_senior_paras(int argc, char *argv[])
{
    int i;
	for(i=0; i<argc; i++)
	{
         if(!strncmp("--set_", argv[i], 6))
         {
            //printf("find pra %s @ %d\n","--set_",i);
            return 1;
         }
	}

	return 0;
}

void init_ext_paras()
{
	g_stVoExtCfg.layer_ch_map = 0x1ff;
}

int is_help(int argc, char *argv[], char *help, char *h)
{
	if(find_pra_index_by_name(help, 0, argc, argv)>0 ||
		find_pra_index_by_name(h, 0, argc, argv)>0){
		return 0;
	}

	return -1;
}

CFG_VO_EXT_S *get_vo_ext_paras()
{
	return &g_stVoExtCfg;
}


