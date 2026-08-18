#include "cfg_audio.h"

IPC_CFG_AUDIO_S g_stRunAudioCfg;

//Usually there's only one audio I/O, so this cfg is ok.
static CFG_MAP audioMap[] = {
	{"i2sType",        &g_stRunAudioCfg.i2sType,        CFG_DATA_TYPE_STRING, "iner",    "rw", 1, 32, "iner for imx307/415/464, extern : tp9930 "},
    {"mode",           &g_stRunAudioCfg.mode,           CFG_DATA_TYPE_S32, "0",    "rw", 0, 3, "0 disable; 1 ai; 2 ai & ao; 3 ai & aenc"},
    {"type",           &g_stRunAudioCfg.type,           CFG_DATA_TYPE_S32, "0",    "rw", 0, 2, "0 a-law PCM; 1 u-law PCM"},
    {"chans",          &g_stRunAudioCfg.chans,          CFG_DATA_TYPE_S32, "2",    "rw", 1, 2, "1 mono; 2 stereo"},
    {"sampleRate",     &g_stRunAudioCfg.sampleRate,     CFG_DATA_TYPE_S32, "32000", "rw", 8000, 96000, NULL},
    {"sampleBitWidth", &g_stRunAudioCfg.sampleBitWidth, CFG_DATA_TYPE_S32, "16",   "rw", 8, 32, NULL},
    {"inputVolume",    &g_stRunAudioCfg.inputVolume,    CFG_DATA_TYPE_S32, "46",   "rw", -2, 46, NULL},
    {"outputVolume",   &g_stRunAudioCfg.outputVolume,   CFG_DATA_TYPE_S32, "8",   "rw", -2, 8, NULL},
    {"rebootMute",      &g_stRunAudioCfg.rebootMute,    CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, "0 not mute when reboot; 1 mute when reboot"},
    {NULL,},
};

void IPC_CFG_AUDIO_Print()
{
    printf("********** Audio *********\n");
    CfgPrintMap(audioMap);
    printf("********** Audio *********\n\n");
}

int IPC_CFG_AUDIO_Save()
{
    int ret = CfgSave(AUDIO_CFG_FILE, "audio", audioMap);
    if (ret != 0) {
        PRINT_ERR("CfgSave %s error.", AUDIO_CFG_FILE);
        return -1;
    }

    return 0;
}

static void IPC_CFG_Audio_SetTP9930()
{
	IPC_CFG_AUDIO_S *pstAudioCfg =  IPC_CFG_AUDIO_GetParam();

	strcpy(pstAudioCfg->i2sType, "tp9930");
	pstAudioCfg->sampleBitWidth = 16; //i2s not support 8bit
	pstAudioCfg->sampleRate = 16000;  //for tp9930 support 8k or 16k with 4 chn, I2S should 4 couple
	pstAudioCfg->inputVolume = 14;   //input volume range (2-14)
	pstAudioCfg->chans = 1;
	pstAudioCfg->mode = 1;
}

AR_S32 IPC_CFG_AUDIO_SetDefault(CFG_RUN_MODE_E enMode)
{
	int ret = 0;
	CfgLoadDefValue(audioMap);
	
	switch(enMode)
	{
		case CFG_RUN_MODE_DVP_8X1080P_30_LOW_BW:
		case CFG_RUN_MODE_DVP_8X1080P_25_LOW_BW:
			IPC_CFG_Audio_SetTP9930();
			break;
		default:
			break;
	}

    return 0;
}

AR_S32 IPC_CFG_AUDIO_Load(CFG_RUN_MODE_E enMode)
{
    char *data = NULL;
    data = CfgReadFromFile(AUDIO_CFG_FILE);
    if (data == NULL) {
        PRINT_INFO("load %s error, so to load default cfg param.\n", AUDIO_CFG_FILE);
        goto err2;
    }

    cJSON *json = NULL;
    json = cJSON_Parse(data);
    if (!json)
    {
        PRINT_INFO("Error before: [%s]\n", cJSON_GetErrorPtr());
        free(data);
        goto err2;
    }

    CfgParseCjson(json, "audio", audioMap);

    cJSON_Delete(json);
    free(data);
    return 0;

err2:
    IPC_CFG_AUDIO_SetDefault(enMode);
    CfgSave(AUDIO_CFG_FILE, "audio", audioMap);
    return 0;
}

AR_S32 IPC_CFG_AUDIO_LoadDefault(CFG_RUN_MODE_E enMode)
{
    CfgLoadDefValue(audioMap);

    return 0;
}

IPC_CFG_AUDIO_S * IPC_CFG_AUDIO_GetParam()
{
    return &g_stRunAudioCfg;
}


