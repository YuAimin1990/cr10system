#include "cfg_vo.h"

CFG_VO_S g_stVoCfg;

CFG_MAP VoDevMap[] =
{
	{"dev_id",                &(g_stVoCfg.stVoDev.dev_id),                CFG_DATA_TYPE_S32,    "0",        "rw", 0,   0,        NULL},
	{"dev_csc",               &(g_stVoCfg.stVoDev.dev_csc),               CFG_DATA_TYPE_S32,    "0",        "rw", 0,   4,        NULL},
	{"bg_color",              &(g_stVoCfg.stVoDev.bg_color),              CFG_DATA_TYPE_U32,    "0x0000ff", "rw", 0,   0xffffff, NULL},
	{"interface",             &(g_stVoCfg.stVoDev.interface),             CFG_DATA_TYPE_S32,    "0",        "rw", 0,   2,        NULL},
	{"sub_interface",         &(g_stVoCfg.stVoDev.sub_interface),         CFG_DATA_TYPE_S32,    "2",        "rw", 0,   10,       NULL},
	{"timing_template",       &(g_stVoCfg.stVoDev.timing_template),       CFG_DATA_TYPE_S32,    "6",        "rw", 0,   23,       NULL},
	{"is_interlace",          &(g_stVoCfg.stVoDev.is_interlace),          CFG_DATA_TYPE_S32,    "0",        "rw", 0,   1,        NULL},
	{"bit_count_per_channel", &(g_stVoCfg.stVoDev.bit_count_per_channel), CFG_DATA_TYPE_U32,    "8",        "rw", 8,   32,       NULL},
	
	{"layer_id",              &(g_stVoCfg.stVoDev.layer_id),              CFG_DATA_TYPE_S32,    "0",        "rw", 0,   0,        NULL},
	{"overlayer_num",         &(g_stVoCfg.stVoDev.overlayer_num),         CFG_DATA_TYPE_S32,    "0",        "rw", 0,   2,        NULL},
	{"layer_csc_in",          &(g_stVoCfg.stVoDev.layer_csc_in),          CFG_DATA_TYPE_S32,    "0",        "rw", 0,   4,        NULL},
	{"layer_csc_out",         &(g_stVoCfg.stVoDev.layer_csc_out),         CFG_DATA_TYPE_S32,    "0",        "rw", 0,   4,        NULL},
	{"width",                 &(g_stVoCfg.stVoDev.width),                 CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
	{"height",                &(g_stVoCfg.stVoDev.height),                CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
	{"pos_x",                 &(g_stVoCfg.stVoDev.pos_x),                 CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
	{"pos_y",                 &(g_stVoCfg.stVoDev.pos_y),                 CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
	{"fps",                   &(g_stVoCfg.stVoDev.fps),                   CFG_DATA_TYPE_FLOAT,  "30",       "rw", 0,   60,       NULL},
	{"format",                &(g_stVoCfg.stVoDev.format),                CFG_DATA_TYPE_S32,    "3",        "rw", 0,   3,        NULL},
	
	{"cursor_enable",         &(g_stVoCfg.stVoDev.cursor_enable),         CFG_DATA_TYPE_S32,    "0",        "rw", 0,   1,        NULL},
	{"cursor_pos_x",          &(g_stVoCfg.stVoDev.cursor_pos_x),          CFG_DATA_TYPE_S32,    "100",      "rw", 0,   3840,     NULL},
	{"cursor_pos_y",          &(g_stVoCfg.stVoDev.cursor_pos_y),          CFG_DATA_TYPE_S32,    "100",      "rw", 0,   2160,     NULL},
	{"cursorpath",            &(g_stVoCfg.stVoDev.cursorpath[0]),         CFG_DATA_TYPE_STRING, "",         "rw", 0,   256,      NULL},
	{NULL,},
};

CFG_MAP VoChnMap[CFG_MAX_VO_CHN_NUM][16] =
{
    //channel 0
	{
		{"enable",              &(g_stVoCfg.stVoChn[0].enable),              CFG_DATA_TYPE_S32,    "1",        "rw", 0,   1,        NULL},
		{"priority",            &(g_stVoCfg.stVoChn[0].priority),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   9,        NULL},
		{"bg_color",            &(g_stVoCfg.stVoChn[0].bg_color),            CFG_DATA_TYPE_U32,    "0xff0000", "rw", 0,   0xffffff, NULL},
		{"ch_width",            &(g_stVoCfg.stVoChn[0].ch_width),            CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"ch_height",           &(g_stVoCfg.stVoChn[0].ch_height),           CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"ch_pos_x",            &(g_stVoCfg.stVoChn[0].ch_pos_x),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"ch_pos_y",            &(g_stVoCfg.stVoChn[0].ch_pos_y),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"aspect_ratio_mode",   &(g_stVoCfg.stVoChn[0].aspect_ratio_mode),   CFG_DATA_TYPE_S32,    "0",        "rw", 0,   2,        NULL},
		{"aspect_ratio_width",  &(g_stVoCfg.stVoChn[0].aspect_ratio_width),  CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"aspect_ratio_height", &(g_stVoCfg.stVoChn[0].aspect_ratio_height), CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_x",  &(g_stVoCfg.stVoChn[0].aspect_ratio_pos_x),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_y",  &(g_stVoCfg.stVoChn[0].aspect_ratio_pos_y),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"image_width",         &(g_stVoCfg.stVoChn[0].image_width),         CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"image_height",        &(g_stVoCfg.stVoChn[0].image_height),        CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"imagepath",           &(g_stVoCfg.stVoChn[0].imagepath[0]),        CFG_DATA_TYPE_STRING, "",         "rw", 0,   256,      NULL},
		{NULL,},
	},
	//channel 1
	{
		{"enable",              &(g_stVoCfg.stVoChn[1].enable),              CFG_DATA_TYPE_S32,    "0",        "rw", 0,   1,        NULL},
		{"priority",            &(g_stVoCfg.stVoChn[1].priority),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   9,        NULL},
		{"bg_color",            &(g_stVoCfg.stVoChn[1].bg_color),            CFG_DATA_TYPE_U32,    "0xff0000", "rw", 0,   0xffffff, NULL},
		{"ch_width",            &(g_stVoCfg.stVoChn[1].ch_width),            CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"ch_height",           &(g_stVoCfg.stVoChn[1].ch_height),           CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"ch_pos_x",            &(g_stVoCfg.stVoChn[1].ch_pos_x),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"ch_pos_y",            &(g_stVoCfg.stVoChn[1].ch_pos_y),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"aspect_ratio_mode",   &(g_stVoCfg.stVoChn[1].aspect_ratio_mode),   CFG_DATA_TYPE_S32,    "0",        "rw", 0,   2,        NULL},
		{"aspect_ratio_width",  &(g_stVoCfg.stVoChn[1].aspect_ratio_width),  CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"aspect_ratio_height", &(g_stVoCfg.stVoChn[1].aspect_ratio_height), CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_x",  &(g_stVoCfg.stVoChn[1].aspect_ratio_pos_x),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_y",  &(g_stVoCfg.stVoChn[1].aspect_ratio_pos_y),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"image_width",         &(g_stVoCfg.stVoChn[1].image_width),         CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"image_height",        &(g_stVoCfg.stVoChn[1].image_height),        CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"imagepath",           &(g_stVoCfg.stVoChn[1].imagepath[0]),        CFG_DATA_TYPE_STRING, "",         "rw", 0,   256,      NULL},
		{NULL,},
	},
	//channel 2
	{
		{"enable",              &(g_stVoCfg.stVoChn[2].enable),              CFG_DATA_TYPE_S32,    "0",        "rw", 0,   1,        NULL},
		{"priority",            &(g_stVoCfg.stVoChn[2].priority),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   9,        NULL},
		{"bg_color",            &(g_stVoCfg.stVoChn[2].bg_color),            CFG_DATA_TYPE_U32,    "0xff0000", "rw", 0,   0xffffff, NULL},
		{"ch_width",            &(g_stVoCfg.stVoChn[2].ch_width),            CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"ch_height",           &(g_stVoCfg.stVoChn[2].ch_height),           CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"ch_pos_x",            &(g_stVoCfg.stVoChn[2].ch_pos_x),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"ch_pos_y",            &(g_stVoCfg.stVoChn[2].ch_pos_y),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"aspect_ratio_mode",   &(g_stVoCfg.stVoChn[2].aspect_ratio_mode),   CFG_DATA_TYPE_S32,    "0",        "rw", 0,   2,        NULL},
		{"aspect_ratio_width",  &(g_stVoCfg.stVoChn[2].aspect_ratio_width),  CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"aspect_ratio_height", &(g_stVoCfg.stVoChn[2].aspect_ratio_height), CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_x",  &(g_stVoCfg.stVoChn[2].aspect_ratio_pos_x),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_y",  &(g_stVoCfg.stVoChn[2].aspect_ratio_pos_y),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"image_width",         &(g_stVoCfg.stVoChn[2].image_width),         CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"image_height",        &(g_stVoCfg.stVoChn[2].image_height),        CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"imagepath",           &(g_stVoCfg.stVoChn[2].imagepath[0]),        CFG_DATA_TYPE_STRING, "",         "rw", 0,   256,      NULL},
		{NULL,},
	},
	//channel 3
	{
		{"enable",              &(g_stVoCfg.stVoChn[3].enable),              CFG_DATA_TYPE_S32,    "0",        "rw", 0,   1,        NULL},
		{"priority",            &(g_stVoCfg.stVoChn[3].priority),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   9,        NULL},
		{"bg_color",            &(g_stVoCfg.stVoChn[3].bg_color),            CFG_DATA_TYPE_U32,    "0xff0000", "rw", 0,   0xffffff, NULL},
		{"ch_width",            &(g_stVoCfg.stVoChn[3].ch_width),            CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"ch_height",           &(g_stVoCfg.stVoChn[3].ch_height),           CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"ch_pos_x",            &(g_stVoCfg.stVoChn[3].ch_pos_x),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"ch_pos_y",            &(g_stVoCfg.stVoChn[3].ch_pos_y),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"aspect_ratio_mode",   &(g_stVoCfg.stVoChn[3].aspect_ratio_mode),   CFG_DATA_TYPE_S32,    "0",        "rw", 0,   2,        NULL},
		{"aspect_ratio_width",  &(g_stVoCfg.stVoChn[3].aspect_ratio_width),  CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"aspect_ratio_height", &(g_stVoCfg.stVoChn[3].aspect_ratio_height), CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_x",  &(g_stVoCfg.stVoChn[3].aspect_ratio_pos_x),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_y",  &(g_stVoCfg.stVoChn[3].aspect_ratio_pos_y),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"image_width",         &(g_stVoCfg.stVoChn[3].image_width),         CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"image_height",        &(g_stVoCfg.stVoChn[3].image_height),        CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"imagepath",           &(g_stVoCfg.stVoChn[3].imagepath[0]),        CFG_DATA_TYPE_STRING, "",         "rw", 0,   256,      NULL},
		{NULL,},
	},
	//channel 4
	{
		{"enable",              &(g_stVoCfg.stVoChn[4].enable),              CFG_DATA_TYPE_S32,    "0",        "rw", 0,   1,        NULL},
		{"priority",            &(g_stVoCfg.stVoChn[4].priority),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   9,        NULL},
		{"bg_color",            &(g_stVoCfg.stVoChn[4].bg_color),            CFG_DATA_TYPE_U32,    "0xff0000", "rw", 0,   0xffffff, NULL},
		{"ch_width",            &(g_stVoCfg.stVoChn[4].ch_width),            CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"ch_height",           &(g_stVoCfg.stVoChn[4].ch_height),           CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"ch_pos_x",            &(g_stVoCfg.stVoChn[4].ch_pos_x),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"ch_pos_y",            &(g_stVoCfg.stVoChn[4].ch_pos_y),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"aspect_ratio_mode",   &(g_stVoCfg.stVoChn[4].aspect_ratio_mode),   CFG_DATA_TYPE_S32,    "0",        "rw", 0,   2,        NULL},
		{"aspect_ratio_width",  &(g_stVoCfg.stVoChn[4].aspect_ratio_width),  CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"aspect_ratio_height", &(g_stVoCfg.stVoChn[4].aspect_ratio_height), CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_x",  &(g_stVoCfg.stVoChn[4].aspect_ratio_pos_x),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_y",  &(g_stVoCfg.stVoChn[4].aspect_ratio_pos_y),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"image_width",         &(g_stVoCfg.stVoChn[4].image_width),         CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"image_height",        &(g_stVoCfg.stVoChn[4].image_height),        CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"imagepath",           &(g_stVoCfg.stVoChn[4].imagepath[0]),        CFG_DATA_TYPE_STRING, "",         "rw", 0,   256,      NULL},
		{NULL,},
	},
	//channel 5
	{
		{"enable",              &(g_stVoCfg.stVoChn[5].enable),              CFG_DATA_TYPE_S32,    "0",        "rw", 0,   1,        NULL},
		{"priority",            &(g_stVoCfg.stVoChn[5].priority),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   9,        NULL},
		{"bg_color",            &(g_stVoCfg.stVoChn[5].bg_color),            CFG_DATA_TYPE_U32,    "0xff0000", "rw", 0,   0xffffff, NULL},
		{"ch_width",            &(g_stVoCfg.stVoChn[5].ch_width),            CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"ch_height",           &(g_stVoCfg.stVoChn[5].ch_height),           CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"ch_pos_x",            &(g_stVoCfg.stVoChn[5].ch_pos_x),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"ch_pos_y",            &(g_stVoCfg.stVoChn[5].ch_pos_y),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"aspect_ratio_mode",   &(g_stVoCfg.stVoChn[5].aspect_ratio_mode),   CFG_DATA_TYPE_S32,    "0",        "rw", 0,   2,        NULL},
		{"aspect_ratio_width",  &(g_stVoCfg.stVoChn[5].aspect_ratio_width),  CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"aspect_ratio_height", &(g_stVoCfg.stVoChn[5].aspect_ratio_height), CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_x",  &(g_stVoCfg.stVoChn[5].aspect_ratio_pos_x),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_y",  &(g_stVoCfg.stVoChn[5].aspect_ratio_pos_y),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"image_width",         &(g_stVoCfg.stVoChn[5].image_width),         CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"image_height",        &(g_stVoCfg.stVoChn[5].image_height),        CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"imagepath",           &(g_stVoCfg.stVoChn[5].imagepath[0]),        CFG_DATA_TYPE_STRING, "",         "rw", 0,   256,      NULL},
		{NULL,},
	},
	//channel 6
	{
		{"enable",              &(g_stVoCfg.stVoChn[6].enable),              CFG_DATA_TYPE_S32,    "0",        "rw", 0,   1,        NULL},
		{"priority",            &(g_stVoCfg.stVoChn[6].priority),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   9,        NULL},
		{"bg_color",            &(g_stVoCfg.stVoChn[6].bg_color),            CFG_DATA_TYPE_U32,    "0xff0000", "rw", 0,   0xffffff, NULL},
		{"ch_width",            &(g_stVoCfg.stVoChn[6].ch_width),            CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"ch_height",           &(g_stVoCfg.stVoChn[6].ch_height),           CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"ch_pos_x",            &(g_stVoCfg.stVoChn[6].ch_pos_x),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"ch_pos_y",            &(g_stVoCfg.stVoChn[6].ch_pos_y),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"aspect_ratio_mode",   &(g_stVoCfg.stVoChn[6].aspect_ratio_mode),   CFG_DATA_TYPE_S32,    "0",        "rw", 0,   2,        NULL},
		{"aspect_ratio_width",  &(g_stVoCfg.stVoChn[6].aspect_ratio_width),  CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"aspect_ratio_height", &(g_stVoCfg.stVoChn[6].aspect_ratio_height), CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_x",  &(g_stVoCfg.stVoChn[6].aspect_ratio_pos_x),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_y",  &(g_stVoCfg.stVoChn[6].aspect_ratio_pos_y),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"image_width",         &(g_stVoCfg.stVoChn[6].image_width),         CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"image_height",        &(g_stVoCfg.stVoChn[6].image_height),        CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"imagepath",           &(g_stVoCfg.stVoChn[6].imagepath[0]),        CFG_DATA_TYPE_STRING, "",         "rw", 0,   256,      NULL},
		{NULL,},
	},
	//channel 7
	{
		{"enable",              &(g_stVoCfg.stVoChn[7].enable),              CFG_DATA_TYPE_S32,    "0",        "rw", 0,   1,        NULL},
		{"priority",            &(g_stVoCfg.stVoChn[7].priority),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   9,        NULL},
		{"bg_color",            &(g_stVoCfg.stVoChn[7].bg_color),            CFG_DATA_TYPE_U32,    "0xff0000", "rw", 0,   0xffffff, NULL},
		{"ch_width",            &(g_stVoCfg.stVoChn[7].ch_width),            CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"ch_height",           &(g_stVoCfg.stVoChn[7].ch_height),           CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"ch_pos_x",            &(g_stVoCfg.stVoChn[7].ch_pos_x),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"ch_pos_y",            &(g_stVoCfg.stVoChn[7].ch_pos_y),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"aspect_ratio_mode",   &(g_stVoCfg.stVoChn[7].aspect_ratio_mode),   CFG_DATA_TYPE_S32,    "0",        "rw", 0,   2,        NULL},
		{"aspect_ratio_width",  &(g_stVoCfg.stVoChn[7].aspect_ratio_width),  CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"aspect_ratio_height", &(g_stVoCfg.stVoChn[7].aspect_ratio_height), CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_x",  &(g_stVoCfg.stVoChn[7].aspect_ratio_pos_x),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_y",  &(g_stVoCfg.stVoChn[7].aspect_ratio_pos_y),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"image_width",         &(g_stVoCfg.stVoChn[7].image_width),         CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"image_height",        &(g_stVoCfg.stVoChn[7].image_height),        CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"imagepath",           &(g_stVoCfg.stVoChn[7].imagepath[0]),        CFG_DATA_TYPE_STRING, "",         "rw", 0,   256,      NULL},
		{NULL,},
	},
	//channel 8
	{
		{"enable",              &(g_stVoCfg.stVoChn[8].enable),              CFG_DATA_TYPE_S32,    "0",        "rw", 0,   1,        NULL},
		{"priority",            &(g_stVoCfg.stVoChn[8].priority),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   9,        NULL},
		{"bg_color",            &(g_stVoCfg.stVoChn[8].bg_color),            CFG_DATA_TYPE_U32,    "0xff0000", "rw", 0,   0xffffff, NULL},
		{"ch_width",            &(g_stVoCfg.stVoChn[8].ch_width),            CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"ch_height",           &(g_stVoCfg.stVoChn[8].ch_height),           CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"ch_pos_x",            &(g_stVoCfg.stVoChn[8].ch_pos_x),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"ch_pos_y",            &(g_stVoCfg.stVoChn[8].ch_pos_y),            CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"aspect_ratio_mode",   &(g_stVoCfg.stVoChn[8].aspect_ratio_mode),   CFG_DATA_TYPE_S32,    "0",        "rw", 0,   2,        NULL},
		{"aspect_ratio_width",  &(g_stVoCfg.stVoChn[8].aspect_ratio_width),  CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"aspect_ratio_height", &(g_stVoCfg.stVoChn[8].aspect_ratio_height), CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_x",  &(g_stVoCfg.stVoChn[8].aspect_ratio_pos_x),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   2160,     NULL},
		{"aspect_ratio_pos_y",  &(g_stVoCfg.stVoChn[8].aspect_ratio_pos_y),  CFG_DATA_TYPE_U32,    "0",        "rw", 0,   3840,     NULL},
		{"image_width",         &(g_stVoCfg.stVoChn[8].image_width),         CFG_DATA_TYPE_S32,    "1920",     "rw", 0,   3840,     NULL},
		{"image_height",        &(g_stVoCfg.stVoChn[8].image_height),        CFG_DATA_TYPE_S32,    "1080",     "rw", 0,   2160,     NULL},
		{"imagepath",           &(g_stVoCfg.stVoChn[8].imagepath[0]),        CFG_DATA_TYPE_STRING, "",         "rw", 0,   256,      NULL},
		{NULL,},
	}
};

static AR_S32 CFG_VO_LoadDefault()
{
    int i = 0;
	
    CfgLoadDefValue(VoDevMap);
	for(i = 0; i < CFG_MAX_VO_CHN_NUM; i++)
    {
        CfgLoadDefValue(VoChnMap[i]);
    }
	
	return 0;
}

static AR_S32 CFG_VO_Save(AR_CHAR *cfg_file)
{
    int i = 0;
    cJSON *root, *channel;
    char *out;
    char tmp[32] = {0};

    root = cJSON_CreateObject();//创建项目
	
	CfgAddCjson(root, "dev", VoDevMap);
	
	channel = cJSON_CreateObject();
	for(i = 0; i < CFG_MAX_VO_CHN_NUM; i++)
    {
        sprintf(tmp, "ch_%d", i);
        CfgAddCjson(channel, tmp, VoChnMap[i]);
    }
	cJSON_AddItemToObject(root, "channel", channel);

    out = cJSON_Print(root);

    int ret = CfgWriteToFile(cfg_file, out);
    if (ret != 0)
    {
        PRINT_ERR("CfgWriteToFile %s error.", cfg_file);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

//Need to re write the code.
AR_S32 CFG_VO_Load(AR_CHAR *cfg_file)
{
    int i = 0;
    char *data = NULL;
	char *tmp_path = cfg_file;
	FILE *fp = NULL;
	
	if ((tmp_path == NULL) || ((fp = fopen(tmp_path, "rb")) == NULL))
	{
		goto err;
	}
	else
	{
		fclose(fp);
	}

    data = CfgReadFromFile(tmp_path);
    if (data == NULL)
    {
        //从配置文件读取失败，则使用默认参数
        PRINT_INFO("load %s error, so to load default cfg param.\n", tmp_path);
        goto err;
    }

    cJSON *json = NULL;
    json = cJSON_Parse(data);
    if (!json)
    {
        PRINT_INFO("Error before: [%s]\n", cJSON_GetErrorPtr());
        free(data);
        goto err;
    }

    CfgParseCjson(json, "dev", VoDevMap);

    cJSON *channel = cJSON_GetObjectItem(json, "channel");
	for(i = 0; i < CFG_MAX_VO_CHN_NUM; i++)
    {
        char tmp[32] = {0};
        sprintf(tmp, "ch_%d", i);
        CfgParseCjson(channel, tmp, VoChnMap[i]);
    }

    cJSON_Delete(json);
    free(data);
    return 0;

err:
    tmp_path = malloc(32);
	if (tmp_path == NULL)
	{
		printf("malloc failed.\n");
		return 0;
	}
	else
	{
		//printf("error occured when prasing json file, use default cfg param, meanwhile save default cfg to ./cfg_vo.json\n");
		sprintf(tmp_path, "cfg_vo.json");
	}

	CFG_VO_LoadDefault();
    CFG_VO_Save(tmp_path);
    free(tmp_path);

    return 0;
}

void CFG_VO_Print()
{
    int i = 0;
	
    printf("********** vo dev *********\n");
	CfgPrintMap(VoDevMap);
	printf("********** vo dev *********\n\n");
	
    printf("********** vo chn *********\n");
	for(i = 0; i < CFG_MAX_VO_CHN_NUM; i++)
    {
        printf("channel %d:\n", i);
        CfgPrintMap(VoChnMap[i]);
    }
    printf("********** vo chn *********\n\n");
}

CFG_VO_S *CFG_VO_GetParam()
{
    return &g_stVoCfg;
}
