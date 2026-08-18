#ifndef __AR_G711_H__
#define __AR_G711_H__

#ifdef __cplusplus
extern "C"
{
#endif

unsigned int AR_G711_AlawEncode(unsigned char *dst, short *src, unsigned int srcSize);
unsigned int AR_G711_AlawDecode(short *dst, const unsigned char *src, unsigned int srcSize);
unsigned int AR_G711_UlawEncode(unsigned char *dst, short *src, unsigned int srcSize);
unsigned int AR_G711_UlawDecode(short *dst, const unsigned char *src, unsigned int srcSize);
unsigned int AR_G711_UlawToAlaw(unsigned char *dst, const unsigned char *src, unsigned int srcSize);
unsigned int AR_G711_AlawToUlaw(unsigned char *dst, const unsigned char *src, unsigned int srcSize);
unsigned int AR_G711_Alaw_Stereo_Encode(unsigned char *dst, short *src0, short *src1, unsigned int srcSize);
unsigned int AR_G711_Ulaw_Stereo_Encode(unsigned char *dst, short *src0, short *src1, unsigned int srcSize);
unsigned int AR_G711_Stereo_AlawDecode(short *dst, const unsigned char *src, unsigned int srcSize);
unsigned int AR_G711_Stereo_UlawDecode(short *dst, const unsigned char *src, unsigned int srcSize);

#ifdef __cplusplus
}
#endif

#endif

