#ifndef __AR_CLK_H__
#define __AR_CLK_H__
#if 0
enum {
	AR_CLK_MOD_CORE = 0xffff0000,
	AR_CLK_MOD_ISP,
	AR_CLK_MOD_DISP,
	AR_CLK_MOD_HEVC_CORE,
	AR_CLK_MOD_HEVC_BPU,
	AR_CLK_MOD_H264,
	AR_CLK_MOD_JPEG,
	AR_CLK_MOD_MIPI,
	AR_CLK_MOD_VIF,
	AR_CLK_MOD_DMAC_CEVA,
	AR_CLK_MOD_DMAC_TOP,
	AR_CLK_MOD_M7,
	AR_CLK_MOD_BB,
	AR_CLK_MOD_BB_LDPC,
	AR_CLK_MOD_TYPEC,
	AR_CLK_MOD_GMAC,
	AR_CLK_MOD_PIX,
	AR_CLK_MOD_AU_PLL_FOR_SENSOR,
};

enum {
	AR_CLK_PLL2 = 0xffff0000,
	AR_CLK_25M,
	AR_CLK_50M,
	AR_CLK_100M,
	AR_CLK_125M,
	AR_CLK_150M,
	AR_CLK_200M,
	AR_CLK_250M,
	AR_CLK_300M,
	AR_CLK_330M,
	AR_CLK_333M,
	AR_CLK_360M,
	AR_CLK_400M,
	AR_CLK_450M,
	AR_CLK_500M,
	AR_CLK_600M,
	AR_CLK_666M,
	AR_CLK_PIXEL,
	AR_CLK_AU_PLL_FOR_SENSOR,
};
#else
#define AR_CLK_MOD_CORE        0xffff0000
#define AR_CLK_MOD_ISP         0xffff0001
#define AR_CLK_MOD_DISP        0xffff0002
#define AR_CLK_MOD_HEVC_CORE   0xffff0003
#define AR_CLK_MOD_HEVC_BPU    0xffff0004
#define AR_CLK_MOD_H264        0xffff0005
#define AR_CLK_MOD_JPEG        0xffff0006
#define AR_CLK_MOD_MIPI        0xffff0007
#define AR_CLK_MOD_VIF         0xffff0008
#define AR_CLK_MOD_DMAC_CEVA   0xffff0009
#define AR_CLK_MOD_DMAC_TOP    0xffff000a
#define AR_CLK_MOD_M7          0xffff000b
#define AR_CLK_MOD_BB          0xffff000c
#define AR_CLK_MOD_BB_LDPC     0xffff000d
#define AR_CLK_MOD_TYPEC       0xffff000e
#define AR_CLK_MOD_GMAC        0xffff000f
#define AR_CLK_MOD_PIX         0xffff0010
#define AR_CLK_MOD_AU_PLL_FOR_SENSOR 0xffff0011

#define INDEX_CLK(id)		(id - AR_CLK_PLL2)

#define AR_CLK_PLL2   0xffff0000
#define AR_CLK_25M    0xffff0001
#define AR_CLK_50M    0xffff0002
#define AR_CLK_100M   0xffff0003
#define AR_CLK_125M   0xffff0004
#define AR_CLK_150M   0xffff0005
#define AR_CLK_200M   0xffff0006
#define AR_CLK_250M   0xffff0007
#define AR_CLK_300M   0xffff0008
#define AR_CLK_330M   0xffff0009
#define AR_CLK_333M   0xffff000a
#define AR_CLK_360M   0xffff000b
#define AR_CLK_400M   0xffff000c
#define AR_CLK_450M   0xffff000d
#define AR_CLK_500M   0xffff000e
#define AR_CLK_600M   0xffff000f
#define AR_CLK_666M   0xffff0010
#define AR_CLK_PIXEL  0xffff0011
#define	AR_CLK_AU_PLL_FOR_SENSOR 0xffff0012
#define AR_CLK_20M    0xffff0013
#define AR_CLK_PLL_DLA  0xffff0014
#define AR_CLK_PLL_CEVA 0xffff0015
#define AR_CLK_PLL_ARM  0xffff0016
#define AR_CLK_OSC    0xffff0017
#define AR_CLK_800M   0xffff0018
#define AR_CLK_1000M   0xffff0019

#define AR_CLK_END  0xffff001a

#endif
#endif
