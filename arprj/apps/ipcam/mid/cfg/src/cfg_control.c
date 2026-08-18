#include "cfg_control.h"
#include "cfg_common.h"

IPC_CFG_CONTROL_S g_stControlCfg;

//stop or start elements, you can add the required elements.
CFG_MAP controlMap[IPC_MAX_CONTRL_ELE_NEEDED][11] =
{
	//element 0
	{
		{"enable",     &(g_stControlCfg.stCtrCfg[0].bEnable),              CFG_DATA_TYPE_U32, "1",    "rw", 0, 1, NULL},   //whether to process element
		{"pipeline_num", &(g_stControlCfg.stCtrCfg[0].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
		{"src_ctl_enable", &(g_stControlCfg.stCtrCfg[0].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "1",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
		{"src_mod_id", &(g_stControlCfg.stCtrCfg[0].sCtrSrcEle.u32ModId),  CFG_DATA_TYPE_U32, "3",    "rw", 1, 13, NULL},
		{"src_dev_id", &(g_stControlCfg.stCtrCfg[0].sCtrSrcEle.u32DevId),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"src_chn_id", &(g_stControlCfg.stCtrCfg[0].sCtrSrcEle.u32ChnId),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[0].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "1",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
		{"dst_mod_id", &(g_stControlCfg.stCtrCfg[0].sCtrDstEle.u32ModId),  CFG_DATA_TYPE_U32, "5",    "rw", 1, 13, NULL},
		{"dst_dev_id", &(g_stControlCfg.stCtrCfg[0].sCtrDstEle.u32DevId),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_chn_id",  &(g_stControlCfg.stCtrCfg[0].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{NULL,}
	},
	//element 1
	{
		{"enable",     &(g_stControlCfg.stCtrCfg[1].bEnable),                 CFG_DATA_TYPE_U32, "1",    "rw", 0, 1, NULL},
		{"pipeline_num", &(g_stControlCfg.stCtrCfg[1].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
		{"src_ctl_enable", &(g_stControlCfg.stCtrCfg[1].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
		{"src_mod_id",   &(g_stControlCfg.stCtrCfg[1].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "5",    "rw", 1, 13, NULL},
		{"src_dev_id",     &(g_stControlCfg.stCtrCfg[1].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"src_chn_id",     &(g_stControlCfg.stCtrCfg[1].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[1].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "1",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
		{"dst_mod_id",   &(g_stControlCfg.stCtrCfg[1].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "7",    "rw", 1, 13, NULL},
		{"dst_dev_id",     &(g_stControlCfg.stCtrCfg[1].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_chn_id",     &(g_stControlCfg.stCtrCfg[1].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{NULL,}
	},
	//element 2
	{
		{"enable",     &(g_stControlCfg.stCtrCfg[2].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
		{"pipeline_num", &(g_stControlCfg.stCtrCfg[2].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
		{"src_ctl_enable", &(g_stControlCfg.stCtrCfg[2].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "1",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
		{"src_mod_id",   &(g_stControlCfg.stCtrCfg[2].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "5",    "rw", 1, 13, NULL},
		{"src_dev_id",     &(g_stControlCfg.stCtrCfg[2].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "1",    "rw", 0, 64, NULL},
		{"src_chn_id",     &(g_stControlCfg.stCtrCfg[2].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[2].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "1",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
		{"dst_mod_id",   &(g_stControlCfg.stCtrCfg[2].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "7",    "rw", 1, 13, NULL},
		{"dst_dev_id",     &(g_stControlCfg.stCtrCfg[2].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_chn_id",     &(g_stControlCfg.stCtrCfg[2].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "1",    "rw", 0, 64, NULL},
		{NULL,}
	},
	//element 3
	{
		{"enable",     &(g_stControlCfg.stCtrCfg[3].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
		{"pipeline_num", &(g_stControlCfg.stCtrCfg[3].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
		{"src_ctl_enable", &(g_stControlCfg.stCtrCfg[3].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "1",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
		{"src_mod_id",   &(g_stControlCfg.stCtrCfg[3].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "5",    "rw", 1, 13, NULL},
		{"src_dev_id",     &(g_stControlCfg.stCtrCfg[3].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "1",    "rw", 0, 64, NULL},
		{"src_chn_id",     &(g_stControlCfg.stCtrCfg[3].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "2",    "rw", 0, 64, NULL},
		{"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[3].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "1",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
		{"dst_mod_id",   &(g_stControlCfg.stCtrCfg[3].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "4",    "rw", 1, 13, NULL},
		{"dst_dev_id",     &(g_stControlCfg.stCtrCfg[3].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_chn_id",     &(g_stControlCfg.stCtrCfg[3].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{NULL,}
	},
	//element 4
	{
		{"enable",     &(g_stControlCfg.stCtrCfg[4].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
		{"pipeline_num", &(g_stControlCfg.stCtrCfg[4].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
		{"src_ctl_enable", &(g_stControlCfg.stCtrCfg[4].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
		{"src_mod_id",   &(g_stControlCfg.stCtrCfg[4].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"src_dev_id",     &(g_stControlCfg.stCtrCfg[4].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"src_chn_id",     &(g_stControlCfg.stCtrCfg[4].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[4].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
		{"dst_mod_id",   &(g_stControlCfg.stCtrCfg[4].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"dst_dev_id",     &(g_stControlCfg.stCtrCfg[4].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_chn_id",     &(g_stControlCfg.stCtrCfg[4].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{NULL,}
	},
	//element 5
	{
		{"enable",     &(g_stControlCfg.stCtrCfg[5].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
		{"pipeline_num", &(g_stControlCfg.stCtrCfg[5].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
		{"src_ctl_enable", &(g_stControlCfg.stCtrCfg[5].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
		{"src_mod_id",   &(g_stControlCfg.stCtrCfg[5].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"src_dev_id",     &(g_stControlCfg.stCtrCfg[5].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"src_chn_id",     &(g_stControlCfg.stCtrCfg[5].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[5].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
		{"dst_mod_id",   &(g_stControlCfg.stCtrCfg[5].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"dst_dev_id",     &(g_stControlCfg.stCtrCfg[5].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_chn_id",     &(g_stControlCfg.stCtrCfg[5].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{NULL,}
	},
	//element 6
	{
		{"enable",     &(g_stControlCfg.stCtrCfg[6].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
		{"pipeline_num", &(g_stControlCfg.stCtrCfg[6].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
		{"src_ctl_enable", &(g_stControlCfg.stCtrCfg[6].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
		{"src_mod_id",   &(g_stControlCfg.stCtrCfg[6].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"src_dev_id",     &(g_stControlCfg.stCtrCfg[6].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"src_chn_id",     &(g_stControlCfg.stCtrCfg[6].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[6].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
		{"dst_mod_id",   &(g_stControlCfg.stCtrCfg[6].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"dst_dev_id",     &(g_stControlCfg.stCtrCfg[6].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_chn_id",     &(g_stControlCfg.stCtrCfg[6].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{NULL,}
	},
	//element 7
	{
		{"enable",     &(g_stControlCfg.stCtrCfg[7].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
		{"pipeline_num", &(g_stControlCfg.stCtrCfg[7].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
		{"src_ctl_enable", &(g_stControlCfg.stCtrCfg[7].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
		{"src_mod_id",   &(g_stControlCfg.stCtrCfg[7].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"src_dev_id",     &(g_stControlCfg.stCtrCfg[7].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"src_chn_id",     &(g_stControlCfg.stCtrCfg[7].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[7].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
		{"dst_mod_id",   &(g_stControlCfg.stCtrCfg[7].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"dst_dev_id",     &(g_stControlCfg.stCtrCfg[7].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_chn_id",     &(g_stControlCfg.stCtrCfg[7].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{NULL,}
	},
	//element 8
	{
		{"enable",     &(g_stControlCfg.stCtrCfg[8].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
		{"pipeline_num", &(g_stControlCfg.stCtrCfg[8].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
		{"src_ctl_enable", &(g_stControlCfg.stCtrCfg[8].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
		{"src_mod_id",   &(g_stControlCfg.stCtrCfg[8].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"src_dev_id",     &(g_stControlCfg.stCtrCfg[8].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"src_chn_id",     &(g_stControlCfg.stCtrCfg[8].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[8].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
		{"dst_mod_id",   &(g_stControlCfg.stCtrCfg[8].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"dst_dev_id",     &(g_stControlCfg.stCtrCfg[8].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_chn_id",     &(g_stControlCfg.stCtrCfg[8].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{NULL,}
	},
	//element 9
	{
		{"enable",     &(g_stControlCfg.stCtrCfg[9].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
		{"pipeline_num", &(g_stControlCfg.stCtrCfg[9].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
		{"src_ctl_enable", &(g_stControlCfg.stCtrCfg[9].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
		{"src_mod_id",   &(g_stControlCfg.stCtrCfg[9].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"src_dev_id",     &(g_stControlCfg.stCtrCfg[9].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"src_chn_id",     &(g_stControlCfg.stCtrCfg[9].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[9].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
		{"dst_mod_id",   &(g_stControlCfg.stCtrCfg[9].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"dst_dev_id",     &(g_stControlCfg.stCtrCfg[9].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_chn_id",     &(g_stControlCfg.stCtrCfg[9].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{NULL,}
	},
	//element 10
	{
		{"enable",     &(g_stControlCfg.stCtrCfg[10].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
		{"pipeline_num", &(g_stControlCfg.stCtrCfg[10].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
		{"src_ctl_enable", &(g_stControlCfg.stCtrCfg[10].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
		{"src_mod_id",   &(g_stControlCfg.stCtrCfg[10].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"src_dev_id",     &(g_stControlCfg.stCtrCfg[10].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"src_chn_id",     &(g_stControlCfg.stCtrCfg[10].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[10].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
		{"dst_mod_id",   &(g_stControlCfg.stCtrCfg[10].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"dst_dev_id",     &(g_stControlCfg.stCtrCfg[10].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_chn_id",     &(g_stControlCfg.stCtrCfg[10].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{NULL,}
	},
	//element 11
	{
		{"enable",     &(g_stControlCfg.stCtrCfg[11].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
		{"pipeline_num", &(g_stControlCfg.stCtrCfg[11].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
		{"src_ctl_enable", &(g_stControlCfg.stCtrCfg[11].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
		{"src_mod_id",   &(g_stControlCfg.stCtrCfg[11].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"src_dev_id",     &(g_stControlCfg.stCtrCfg[11].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"src_chn_id",     &(g_stControlCfg.stCtrCfg[11].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[11].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
		{"dst_mod_id",   &(g_stControlCfg.stCtrCfg[11].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
		{"dst_dev_id",     &(g_stControlCfg.stCtrCfg[11].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{"dst_chn_id",     &(g_stControlCfg.stCtrCfg[11].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
		{NULL,}
	},
    //element 12
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[12].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[12].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[12].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[12].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[12].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[12].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[12].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[12].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[12].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[12].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 13
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[13].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[13].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[13].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[13].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[13].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[13].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[13].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[13].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[13].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[13].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 14
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[14].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[14].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[14].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[14].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[14].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[14].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[14].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[14].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[14].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[14].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 15
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[15].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[15].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[15].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[15].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[15].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[15].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[15].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[15].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[15].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[15].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 16
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[16].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[16].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[16].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[16].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[16].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[16].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[16].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[16].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[16].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[16].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 17
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[17].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[17].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[17].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[17].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[17].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[17].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[17].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[17].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[17].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[17].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 18
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[18].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[18].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[18].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[18].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[18].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[18].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[18].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[18].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[18].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[18].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 19
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[19].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[19].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[19].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[19].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[19].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[19].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[19].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[19].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[19].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[19].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 20
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[20].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[20].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[20].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[20].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[20].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[20].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[20].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[20].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[20].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[20].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 21
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[21].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[21].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[21].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[21].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[21].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[21].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[21].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[21].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[21].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[21].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 22
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[22].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[22].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[22].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[22].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[22].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[22].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[22].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[22].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[22].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[22].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 23
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[23].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[23].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[23].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[23].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[23].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[23].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[23].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[23].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[23].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[23].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 24
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[24].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[24].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[24].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[24].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[24].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[24].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[24].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[24].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[24].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[24].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 25
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[25].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[25].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[25].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[25].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[25].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[25].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[25].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[25].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[25].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[25].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 26
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[26].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[26].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[26].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[26].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[26].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[26].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[26].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[26].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[26].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[26].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 27
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[27].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[27].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[27].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[27].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[27].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[27].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[27].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[27].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[27].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[27].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 28
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[28].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[28].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[28].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[28].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[28].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[28].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[28].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[28].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[28].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[28].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 29
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[29].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[29].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[29].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[29].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[29].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[29].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[29].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[29].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[29].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[29].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 30
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[30].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[30].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[30].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[30].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[30].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[30].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[30].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[30].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[30].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[30].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
    //element 31
    {
        {"enable",     &(g_stControlCfg.stCtrCfg[31].bEnable),                 CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"pipeline_num", &(g_stControlCfg.stCtrCfg[31].u32PipelineNum),  CFG_DATA_TYPE_U32, "0",       "rw", 0, IPC_MAX_PIPELINE_NUM, NULL},
        {"src_ctl_enable", &(g_stControlCfg.stCtrCfg[31].sCtrSrcEle.u32Status),  CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL}, //whether to enable or disbale src element
        {"src_mod_id",   &(g_stControlCfg.stCtrCfg[31].sCtrSrcEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"src_dev_id",     &(g_stControlCfg.stCtrCfg[31].sCtrSrcEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"src_chn_id",     &(g_stControlCfg.stCtrCfg[31].sCtrSrcEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_ctl_enable",  &(g_stControlCfg.stCtrCfg[31].sCtrDstEle.u32Status), CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},//whether to enable or disbale dst element
        {"dst_mod_id",   &(g_stControlCfg.stCtrCfg[31].sCtrDstEle.u32ModId),   CFG_DATA_TYPE_U32, "0",    "rw", 1, 13, NULL},
        {"dst_dev_id",     &(g_stControlCfg.stCtrCfg[31].sCtrDstEle.u32DevId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {"dst_chn_id",     &(g_stControlCfg.stCtrCfg[31].sCtrDstEle.u32ChnId), CFG_DATA_TYPE_U32, "0",    "rw", 0, 64, NULL},
        {NULL,}
    },
};

AR_S32 IPC_CFG_CONTROL_Load(CFG_RUN_MODE_E enMode)
{
	char *data = NULL;
	data = CfgReadFromFile(CONTROL_CFG_FILE);
	if (data == NULL)
	{
		PRINT_INFO("load %s error, so to load default cfg param.\n", CONTROL_CFG_FILE);
		goto err;
	}

	cJSON *json = NULL;
	json = cJSON_Parse(data);
	if (!json)
	{
		PRINT_ERR("Error before: [%s]\n", cJSON_GetErrorPtr());
		free(data);
		goto err;
	}

	int i;
	for(i = 0; i < IPC_MAX_CONTRL_ELE_NEEDED; i++)
	{
		char tmp[32] = {0};
		sprintf(tmp, "elemenet_%d", i);

		CfgParseCjson(json, tmp, controlMap[i]);
	}

	cJSON_Delete(json);
	free(data);
	return 0;

err:
	IPC_CFG_CONTROL_LoadDefault(enMode);
	IPC_CFG_CONTROL_Save();
	return 0;
}

void IPC_CFG_CONTROL_Print()
{
	int i;
	printf("********** CONTROL ELEMENTS *********\n");
	for(i = 0; i < IPC_MAX_CONTRL_ELE_NEEDED; i ++)
	{
		printf("start/stop control elment %d:\n", i);
		CfgPrintMap(controlMap[i]);
		printf("\n");
	}
	printf("********** CONTROL ELEMENTS *********\n\n");
}

AR_S32 IPC_CFG_CONTROL_LoadDefault()
{
	int i = 0;
	for(i = 0; i < IPC_MAX_CONTRL_ELE_NEEDED; i++)
	{
		CfgLoadDefValue(controlMap[i]);
	}
	return 0;
}

AR_S32 IPC_CFG_CONTROL_Save()
{
	cJSON *root;
	char *out;
	int i = 0;

	root = cJSON_CreateObject();

	for(i = 0; i < IPC_MAX_CONTRL_ELE_NEEDED; i++)
	{
		char tmp[32] = {0};
		sprintf(tmp, "elemenet_%d", i);

		CfgAddCjson(root, tmp, controlMap[i]);
	}

	out = cJSON_Print(root);

	AR_S32 ret = CfgWriteToFile(CONTROL_CFG_FILE, out);
	if (ret != 0)
	{
		PRINT_ERR("CfgWriteToFile %s error.", CONTROL_CFG_FILE);
		return -1;
	}

	free(out);
	cJSON_Delete(root);

	return 0;
}

IPC_CFG_CONTROL_S * IPC_CFG_CONTROL_GetParam()
{
	return &g_stControlCfg;
}


