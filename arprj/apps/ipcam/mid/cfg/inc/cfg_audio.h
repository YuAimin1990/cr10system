#ifndef __CFG_AUDIO_H__
#define __CFG_AUDIO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"


/***********************************/
/***       audio                 ***/
/***********************************/


typedef struct {
	AR_CHAR 			   i2sType[32];    // I2S type 
    AR_S32                 mode;           // 0 disable; 1 input; 2- input&output
    AR_S32                 type;           // 0 a-law; 1 u-law; 2 pcm; 3-adpcm
    AR_S32                 chans;          // 1
    AR_S32                 sampleRate;     //8000
    AR_S32                 sampleBitWidth; //8, 16
    AR_S32                 inputVolume;    // 100
    AR_S32                 outputVolume;   // 100
    AR_S32                 rebootMute;     // 0 enable audio when boot; 1 disable audio
} IPC_CFG_AUDIO_S;

extern AR_S32 IPC_CFG_AUDIO_Save();
extern AR_S32 IPC_CFG_AUDIO_Load(CFG_RUN_MODE_E enMode);
extern AR_S32 IPC_CFG_AUDIO_LoadDefault(CFG_RUN_MODE_E enMode);
extern void IPC_CFG_AUDIO_Print();
extern IPC_CFG_AUDIO_S * IPC_CFG_AUDIO_GetParam();

#define AUDIO_CFG_FILE "cfg_audio.json"


#ifdef __cplusplus
}
#endif
#endif

