#include "sample_comm.h"
#include "osal.h"
#include "sample_vpss_vio_osd.h"

/*
 * only use 16x26 font, from 0x20 - 0x7E, occupies (16 * 26 / 8) * (0x7E - 0x1F) bytes
 */
#define FONT_WIDTH	 16
#define FONT_HEIGHT  26
#define FONT_CHAR_START 0x20
#define FONT_CHAR_END	0x7E
#define FONT_CHAR_BYTES (FONT_WIDTH * FONT_HEIGHT / 8)
#define FONT_CHAR_IS_VALID(_c)	 (((_c) >= FONT_CHAR_START) && ((_c) <= FONT_CHAR_END))

#define FONT_CHAR_COLOR_RED  (0xFF0000)
#define FONT_CHAR_COLOR_GREEN  (0x00FF00)
#define FONT_CHAR_COLOR_BLUE  (0x0000FF)


static const char* FontFile = "/usrdata/local/factory/font/font_16_26.bin";
static char* pFont = NULL;

static ar_timer_id_t redraw_timer_id = NULL;
static RGN_HANDLE redraw_rgn_hdl = 0;

static AR_U32 vpss_text_color[VPSS_MAX_GRP_NUM][VPSS_MAX_PHY_CHN_NUM] = {0};

static AR_CHAR* read_file(const AR_CHAR* name)
{
	AR_CHAR* buf = NULL;
	FILE* pfd = fopen(name, "rb");
	if(!pfd)
	{
		SAMPLE_PRT("fopen %s failed\n", name);
		return NULL;
	}

	AR_S32 ret = fseek(pfd, 0, SEEK_END);
	if(ret < 0)
	{
		SAMPLE_PRT("fseek end %s failed\n", name);
		goto READ_EXIT;
	}

	AR_S32 size = ftell(pfd);
	if(size <= 0)
	{
		SAMPLE_PRT("ftell %s failed\n", name);
		goto READ_EXIT;
	}

	buf = malloc(size);
	if(!buf)
	{
		SAMPLE_PRT("malloc %d bytes failed\n", size);
		goto READ_EXIT;
	}

	ret = fseek(pfd, 0, SEEK_SET);
	if(ret < 0)
	{
		SAMPLE_PRT("fseek head %s failed\n", name);
		free(buf);
		buf = NULL;
		goto READ_EXIT;
	}

	ret = fread(buf, size, 1, pfd);
	if(ret <= 0)
	{
		SAMPLE_PRT("fread %s failed\n", name);
	}

READ_EXIT:
	fclose(pfd);
	return buf;
}

static RGN_HANDLE get_rgn_handle(VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
	// [0, 1024)
	return VpssGrp * VPSS_MAX_PHY_CHN_NUM + VpssChn;
}

static VPSS_GRP get_vpss_group(RGN_HANDLE handle)
{
	return handle / VPSS_MAX_PHY_CHN_NUM;
}

static VPSS_CHN get_vpss_channel(RGN_HANDLE handle)
{
	return handle % VPSS_MAX_PHY_CHN_NUM;
}

static AR_VOID redraw(sigval_t val)
{
	RGN_HANDLE *rgn_hdl = (RGN_HANDLE*)val.sival_ptr;

	VPSS_GRP VpssGrp = get_vpss_group(*rgn_hdl);
	VPSS_CHN VpssChn = get_vpss_channel(*rgn_hdl);
	AR_CHAR str[64] = {0};
	AR_CHAR buf[48] = {0};
	time_t timeval;
	struct tm* ptm;

	time(&timeval);
	ptm = localtime(&timeval);
	strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y", ptm);
	sprintf(str, "chn%d %s", VpssChn, buf);
	SAMPLE_VPSS_Set_Text_Color(VpssGrp, VpssChn, FONT_CHAR_COLOR_RED);
	SAMPLE_VPSS_DrawText(VpssGrp, VpssChn, 0, 0, str);
/*
	VpssChn = 1;
	sprintf(str, "chn%d %s", VpssChn, buf);
	SAMPLE_VPSS_Set_Text_Color(VpssGrp, VpssChn, FONT_CHAR_COLOR_BLUE);
	SAMPLE_VPSS_DrawText(VpssGrp, VpssChn, 0, FONT_HEIGHT, str);
*/
}

AR_S32 SAMPLE_VPSS_Init_Osd(VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
	pFont = read_file(FontFile);
	if(!pFont)
	{
		SAMPLE_PRT("init font failed\n");
		return AR_FAILURE;
	}

	redraw_rgn_hdl = get_rgn_handle(VpssGrp, VpssChn);

	// create redraw timer
	redraw_timer_id = ar_os_timer_create(redraw, (void*)&redraw_rgn_hdl, NULL);
	if(!redraw_timer_id)
	{
		SAMPLE_PRT("create timer failed\n");
		return AR_FAILURE;
	}
	// start redraw timer per 500ms
	ar_os_timer_start(redraw_timer_id, 500, 500);

	return AR_SUCCESS;
}

AR_S32 SAMPLE_VPSS_UnInit_Osd(AR_VOID)
{
	if(redraw_timer_id)
	{
		ar_os_timer_delete(redraw_timer_id);
		redraw_timer_id = NULL;
	}

	if(pFont)
	{
		free(pFont);
		pFont = NULL;
	}

	return AR_SUCCESS;
}

static AR_VOID set_rgn_copy_buf(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AR_BOOL enable)
{
	MPP_CHN_S stChn =
	{
		.enModId = AR_ID_VPSS,
		.s32DevId = VpssGrp,
		.s32ChnId = VpssChn
	};

	RGN_CHN_PARA_S stChnPara;
	stChnPara.bCopyBufEnable = enable;
	AR_MPI_RGN_Set_Chn_Para(&stChn, &stChnPara);
}

AR_S32 SAMPLE_VPSS_Add_Rgn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
	RGN_HANDLE rgn_hdl = get_rgn_handle(VpssGrp, VpssChn);
	RGN_ATTR_S stRgnAttr =
	{
		.enType = OVERLAY_RGN,
		.unAttr.stOverlay =
		{
			.enPixelFmt = PIXEL_FORMAT_ARGB_1555,
			.stSize =
			{
				.u32Width = 768,
				.u32Height = (VpssChn+1)*30
			},
			.u32CanvasNum = 2,		   //ping pong
			.u32BgColor = 0x7FFFFFFF   //Alpha = 0x7F, RGB = white
		}
	};

	AR_S32 s32Ret = AR_MPI_RGN_Create(rgn_hdl, &stRgnAttr);
	if(s32Ret)
	{
		SAMPLE_PRT("Create Region for (%d, %d) failed 0x%x\n", VpssGrp, VpssChn, s32Ret);
		return s32Ret;
	}

	MPP_CHN_S stChn =
	{
		.enModId = AR_ID_VPSS,
		.s32DevId = VpssGrp,
		.s32ChnId = VpssChn
	};
	RGN_CHN_ATTR_S stChnAttr =
	{
		.bShow = AR_TRUE,
		.enType = OVERLAY_RGN,
		.unChnAttr.stOverlayChn =
		{
			.u32BgAlpha = 0x20,
			.u32FgAlpha = 0xFF,
			.u32Layer = 0,
			.stQpInfo =
			{
				.bQpDisable = AR_FALSE,
				.bAbsQp = AR_FALSE,
				.s32Qp = 0
			},
			.stInvertColor =
			{
				.bInvColEn = AR_FALSE
			},
			.u16ColorLUT[0] = 0x0000,
			.u16ColorLUT[1] = 0xFFFF,
			.stPoint =
			{
				.s32X = 0,
				.s32Y = 0
			}
		}
	};

	set_rgn_copy_buf(VpssGrp, VpssChn, AR_FALSE);
	s32Ret = AR_MPI_RGN_AttachToChn(rgn_hdl, &stChn, &stChnAttr);
	if(s32Ret)
	{
		SAMPLE_PRT("Attach Region to (%d, %d) failed 0x%x\n", VpssGrp, VpssChn, s32Ret);
	}
	return s32Ret;
}

AR_S32 SAMPLE_VPSS_Remove_Rgn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
	RGN_HANDLE rgn_hdl = get_rgn_handle(VpssGrp, VpssChn);
	MPP_CHN_S stChn =
	{
		.enModId = AR_ID_VPSS,
		.s32DevId = VpssGrp,
		.s32ChnId = VpssChn
	};
	AR_S32 s32Ret = AR_MPI_RGN_DetachFromChn(rgn_hdl, &stChn);
	if(AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("AR_MPI_RGN_DetachFromChn failed 0x%x\n", s32Ret);
	}
	s32Ret = AR_MPI_RGN_Destroy(rgn_hdl);
	if(AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("AR_MPI_RGN_Destroy failed 0x%x\n", s32Ret);
	}
	return s32Ret;
}

static AR_S32 set_rgn_chn_show(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AR_BOOL bShow)
{
	AR_S32 s32Ret = AR_SUCCESS;
	RGN_CHN_ATTR_S stChnAttr = {0};
	RGN_HANDLE rgn_hdl = get_rgn_handle(VpssGrp, VpssChn);
	MPP_CHN_S stChn =
	{
		.enModId = AR_ID_VPSS,
		.s32DevId = VpssGrp,
		.s32ChnId = VpssChn
	};

	s32Ret = AR_MPI_RGN_GetDisplayAttr(rgn_hdl, &stChn, &stChnAttr);
	if(s32Ret)
	{
		SAMPLE_PRT("Get display attr of (%d, %d) failed 0x%x\n", VpssGrp, VpssChn, s32Ret);
		return s32Ret;
	}

	stChnAttr.bShow = bShow;

	s32Ret = AR_MPI_RGN_SetDisplayAttr(rgn_hdl, &stChn, &stChnAttr);
	if(s32Ret)
	{
		SAMPLE_PRT("Set display attr of (%d, %d) failed 0x%x\n", VpssGrp, VpssChn, s32Ret);
	}
	return s32Ret;
}

static AR_U16 rgb888_to_rgb555(AR_U32 u32RGB)
{
	AR_U16 RGB555 = 0;

	RGB555 = ((u32RGB >> 3) & 0x01F) /*R*/
			| ( ((u32RGB >> 11) & 0x01F) << 5) /*G*/
			| ( ((u32RGB >> 19) & 0x01F) << 10);

	return RGB555;
}

/*
static AR_VOID dump_char_dot(AR_CHAR ch, AR_U8* dotmat)
{
	SAMPLE_PRT("%c\n", ch);
	for(int h=0; h<FONT_HEIGHT; h++)
	{
		for(int w = 0; w < FONT_WIDTH; w++)
			SAMPLE_PRT("%2x ", *(dotmat+h*FONT_WIDTH+w));

		SAMPLE_PRT("\n");
	}
	SAMPLE_PRT("\n");
}
*/

static AR_U8* text_to_dotmatrix(AR_CHAR* str)
{
	if(!str)
	{
		SAMPLE_PRT("empty string ptr\n");
		return NULL;
	}

	AR_U32 str_len = strlen(str);
	if(!str_len)
	{
		SAMPLE_PRT("empty string\n");
		return NULL;
	}

	AR_U8* dotmat = malloc(str_len * FONT_CHAR_BYTES);
	if(!dotmat)
	{
		SAMPLE_PRT("malloc %d bytes fail\n", str_len * FONT_CHAR_BYTES);
		return NULL;
	}

	AR_U32 i = 0;
	AR_CHAR tmp_str;
	for(i = 0; i < str_len; i++)
	{
		tmp_str = *(str+i);
		if(!FONT_CHAR_IS_VALID(tmp_str))
		{
			SAMPLE_PRT("char %d not support\n", tmp_str);
			continue;
		}
		memcpy(dotmat + i * FONT_CHAR_BYTES, pFont + (tmp_str - FONT_CHAR_START) * FONT_CHAR_BYTES, FONT_CHAR_BYTES);
	}

	return dotmat;
}

static AR_S32 dotmatrix_to_bmp(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AR_U32 x, AR_U32 y, AR_U8* dotmat, AR_U32 str_len)
{
	AR_S32 s32Ret = AR_SUCCESS;
	RGN_HANDLE rgn_hdl = get_rgn_handle(VpssGrp, VpssChn);
	RGN_CANVAS_INFO_S stCanvasInfo = {0};

	s32Ret = AR_MPI_RGN_GetCanvasInfo(rgn_hdl, &stCanvasInfo);
	if(s32Ret)
	{
		SAMPLE_PRT("Get canvas info failed 0x%x\n", s32Ret);
		return s32Ret;
	}
/*
	else
	{
		SAMPLE_PRT("VpssGrp %d VpssChn %d canvas: phy 0x%llx stride %d w,h=%d,%d\n",
					VpssGrp, VpssChn,
					stCanvasInfo.u64PhyAddr, stCanvasInfo.u32Stride,
					stCanvasInfo.stSize.u32Width, stCanvasInfo.stSize.u32Height);
	}
*/

	// check argb
	if(stCanvasInfo.enPixelFmt != PIXEL_FORMAT_ARGB_1555)
	{
		SAMPLE_PRT("enPixelFmt %d not supported\n", stCanvasInfo.enPixelFmt);
		return AR_FAILURE;
	}

	// check width
	AR_U32 txt_width = str_len * FONT_WIDTH;
	AR_U32 txt_height = FONT_HEIGHT;
	AR_U32 canvas_width = stCanvasInfo.stSize.u32Width;
	AR_U32 canvas_height = stCanvasInfo.stSize.u32Height;
	if(txt_width > canvas_width || txt_height > canvas_height)
	{
		SAMPLE_PRT("string (%d, %d) larger than canvas (%d, %d)\n", txt_width, txt_height, canvas_width, canvas_height);
		return AR_FAILURE;
	}

	AR_U32 i = 0;
	AR_U32 w = 0;
	AR_U32 h = 0;
	AR_U32 char_off = 0, dot_off = 0;
	AR_U16* dst = NULL;
	AR_U64 row_addr = 0;
	AR_U8 bit = 0;
	AR_U8 u8ChLine = 0, update = 0;

	for(i = 0; i < str_len; i++)
	{
		char_off = i * FONT_WIDTH * 2;	// argb1555 = 2 byte
		dot_off = i * FONT_CHAR_BYTES;
		for(h = y; h < y + FONT_HEIGHT; h++)
		{
			row_addr = stCanvasInfo.u64VirtAddr + h * stCanvasInfo.u32Stride;
			for(w = x; w < x + FONT_WIDTH; w++)
			{
				dst = (AR_U16 *)(row_addr + char_off + w * 2);
				u8ChLine = *(AR_U8 *)(dotmat + dot_off + (w + (h-y) * FONT_WIDTH) / 8);
				bit = w % 8;
				update = u8ChLine & (0x080 >> bit);

				if(update)
				{
					*dst = 0x8000 | rgb888_to_rgb555(vpss_text_color[VpssGrp][VpssChn]);
				}
				else
				{
					*dst = 0;
				}
			}
		}
	}
	return AR_SUCCESS;
}

AR_VOID SAMPLE_VPSS_Set_Text_Color(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AR_U32 color)
{
	vpss_text_color[VpssGrp][VpssChn] = color;
}

AR_S32 SAMPLE_VPSS_DrawText(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AR_U32 x, AR_U32 y, AR_CHAR* str)
{
	// update rgn attr
	AR_S32 s32Ret = AR_SUCCESS;
	s32Ret = set_rgn_chn_show(VpssGrp, VpssChn, AR_TRUE);
	if(s32Ret)
	{
		return s32Ret;
	}

	// text to dotmatrix
	AR_U8* dotmat = text_to_dotmatrix(str);
	if(!dotmat)
	{
		SAMPLE_PRT("text_to_dotmatrix failed\n");
		return AR_FAILURE;
	}

	// dotmatrix to bmp
	AR_U32 str_len = strlen(str);
	s32Ret = dotmatrix_to_bmp(VpssGrp, VpssChn, x, y, dotmat, str_len);
	if(s32Ret)
	{
		SAMPLE_PRT("dotmatrix_to_bmp failed 0x%x\n", s32Ret);
		free(dotmat);
		return s32Ret;
	}

	// update cavas
	RGN_HANDLE rgn_hdl = get_rgn_handle(VpssGrp, VpssChn);
	s32Ret = AR_MPI_RGN_UpdateCanvas(rgn_hdl);
	if(s32Ret)
	{
		SAMPLE_PRT("Update canvas failed 0x%x\n", s32Ret);
		free(dotmat);
		return s32Ret;
	}

	free(dotmat);
	return AR_SUCCESS;
}

