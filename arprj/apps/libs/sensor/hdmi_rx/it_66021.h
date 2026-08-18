#ifndef IT_66021_H_
#define IT_66021_H_

#if !defined (__linux__)
#include "ar_i2c.h"
#include "reg_access.h"
#else
#include "it66021_i2c.h"
#endif

#include "ar_ite_hdmi_rx_app.h"
#include "ar_hdmi_rx_app.h"

#define IT_66021_REG_BLOCK_SEL          (0x0F)

/*
#define REG_RX_P0_SYS_STATUS            (0x0A)
#define B_P0_PWR5V_DET                  (0x01)
*/
#define DLOG_INFO(...) ar_always(__VA_ARGS__);

#define ARRAY_COUNT_OF(x)                   ((sizeof(x)/sizeof(0[x])) / ((!(sizeof(x) % sizeof(0[x])))))

#define IT_66021_V_LOCKED_SET_INTERRUPT_POS                    (0x1)
#define IT_66021_V_LOCKED_SET_INTERRUPT_MASK                   (0x1)
#define IT_66021_V_LOCKED_CLEAR_INTERRUPT_POS                  (0x1)
#define IT_66021_V_LOCKED_CLEAR_INTERRUPT_MASK                 (0x1)

#define IT_66021_DE_REGEN_LCK_SET_INTERRUPT_POS                (0x0)
#define IT_66021_DE_REGEN_LCK_SET_INTERRUPT_MASK               (0x1)
#define IT_66021_DE_REGEN_LCK_CLEAR_INTERRUPT_POS              (0x0)
#define IT_66021_DE_REGEN_LCK_CLEAR_INTERRUPT_MASK             (0x1)

int HDMIRX_i2c_init(ar_i2c_component component, uint16_t subaddr);

uint8_t IT_66021_app(void);
void HDMIRX_Initial(void);
uint8_t IT_66021_GetVideoFormat(uint8_t index, uint16_t* widthPtr, uint16_t* hightPtr, uint8_t* framteratePtr, uint8_t* vic);
uint8_t IT_66021_GetAudioSampleRate(uint8_t index, uint32_t* sampleRate);
uint8_t IT_66021_WriteByte(uint8_t slv_addr, uint8_t sub_addr, uint8_t val);
uint8_t IT_66021_ReadByte(uint8_t slv_addr, uint8_t sub_addr);
uint8_t IT_66021_WriteBytes(uint8_t slv_addr, uint8_t sub_addr, uint8_t byteno, uint8_t *p_data);
uint8_t IT_66021_ReadBytes(uint8_t slv_addr, uint8_t sub_addr, uint8_t byteno, uint8_t *p_data);

void IT_66021_Set(unsigned char slv_addr, unsigned char sub_addr, unsigned char mask, unsigned char val);
void IT_Delay(uint32_t delay);
int hdmi_rx_66021_get_vformat(struct video_format_info_t *vformat);
int hdmi_rx_66021_get_aformat(struct audio_format_info_t *aformat);
int hdmi_rx_66021_get_avstate(void);

#endif

