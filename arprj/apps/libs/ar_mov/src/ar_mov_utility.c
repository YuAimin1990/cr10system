
#include "ar_mov_utility.h"

void print_buf(AR_U8 *buf, AR_U32 len)
{
    AR_U8 i = 0;
    for(i = 0; i < len; i ++)
    {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

void hton_set_u32(void *pp, AR_U32 w)
{
    AR_U8 *p = pp;

    p[0] = ( w >> 24)&0xff;
    p[1] = ( w >> 16)&0xff;
    p[2] = ( w >> 8 )&0xff;
    p[3] = ( w >> 0 )&0xff;
}

void hton_set_u16(void *pp, AR_U16 w)
{
    AR_U8 *p = pp;

    p[0] = ( w >> 8)&0xff;
    p[1] = ( w >> 0)&0xff;
}

void hton_set_u8(void *pp, AR_U8 w)
{
    AR_U8 *p = pp;

    p[0] = ( w >> 0)&0xff;
}

void hton_set_u24(void *dst, void *src)
{
    AR_U8 *p = dst;
    AR_U8 *q = src;

    p[0] = q[2];
    p[1] = q[1];
    p[2] = q[0];
}


AR_S32 iframe_get_pps_sps(AR_U8 *buf, AR_S32 len, AR_U8 *sps, AR_U32 *sps_len, AR_U8 *pps, AR_U32* pps_len)
{
	AR_U8 *flag;
	AR_S32 offset = 0;

	AR_S32 config_len = 0;
	AR_S32 iframe_offset = 0;

	while(offset < (len - 5))
	{
		flag=(AR_U8 *)(buf + offset);

		if(flag[0]== 0 && flag[1]==0 && flag[2] == 0 && flag[3] == 1)
		{
			if((flag[4] & 0x1F) == 0x06)
			{
				config_len = offset;
				//break;
			}
			else if((flag[4] & 0x1F) == 0x05) //nal_type == I frame
			{
				if(config_len == 0)
					config_len = offset;

				iframe_offset = offset + 4;
				break;
			}
		}
		offset++;
	}

	//printf("config len = %d, iframe_offset = %d\n", config_len, iframe_offset);

	offset = 0;
	while(offset < (config_len - 5))
	{
		flag=(AR_U8 *)(buf + offset);

		if(flag[0]== 0 && flag[1]==0 && flag[2] == 0 && flag[3] == 1)
		{
			if((flag[4] & 0x1F) == 0x08) //nal_type == PPS, johnnyling 20100628
			{
				memcpy(sps, buf + 4, offset - 4);
				*sps_len = offset - 4;

				memcpy(pps, buf + offset + 4, config_len - offset - 4);
				*pps_len = config_len - offset - 4;

				return iframe_offset;
			}
		}
		offset++;
	}

	return iframe_offset;
}

static void hvcc_init(HEVCDecoderConfigurationRecord *hvcc)
{
    memset(hvcc, 0, sizeof(HEVCDecoderConfigurationRecord));
    hvcc->configurationVersion = 1;
    hvcc->lengthSizeMinusOne   = 3; // 4 bytes

    hvcc->general_profile_compatibility_flags = 0xffffffff;
    hvcc->general_constraint_indicator_flags  = 0xffffffffffff;

    hvcc->min_spatial_segmentation_idc = MAX_SPATIAL_SEGMENTATION + 1;
}

AR_S32 find_start_code(AR_U8 *buf, AR_S32 len, AR_U8 **start)
{
	AR_S32 	offset = 0;
	AR_U8 	*start1= buf;

	while(offset < (len - 5))
	{
		start1=(AR_U8 *)(buf + offset);

		if(start1[0]== 0 && start1[1]==0 && start1[2] == 0 && start1[3] == 1)
		{
			//printf("flag = %p\r\n",flag);
			*start = start1;
			return 0;
		}
		offset++;
	}
	printf("not find vps/sps/pps frame\r\n");
	return -1;
}

AR_S32 iframe_get_vps_sps_pps(AR_U8 *buf, AR_S32 len, AR_U8 *vps, AR_U32 *vps_len, AR_U8 *sps, AR_U32 *sps_len, AR_U8 *pps, AR_U32* pps_len)
{
	AR_S32 s32Ret = 0;
	AR_S32 vps_sps_pps_len = 0;
	AR_S32 len_offset = len;

	AR_U8 *start = NULL;
	AR_U8 *end = NULL;
	AR_U8 *input_buff = buf;
	AR_U8 *start_step = NULL;
	AR_S32 len_temp = len;

	//find_start_code_test(buf,len);
	while(len_offset > 0)
	{	
		s32Ret= find_start_code(input_buff, len_temp, &start); 
		if(s32Ret < 0)
		{
			printf("not find start code\r\n");
			return -1;
		}
		start_step = input_buff + 4;
		s32Ret = find_start_code(start_step, len_temp - 4, &end);
		if(s32Ret < 0)
		{
			printf("not find end code\r\n");
			return -1;
		}
		len_offset = (AR_U32)(end - start) - 4;
		
		if(((start[4] >> 1) & 0x3F) == 0x20) //vps
		{
			memcpy(vps, start + 4, len_offset);
			*vps_len = len_offset;
		}
		else if(((start[4] >> 1) & 0x3F) == 0x21) //sps
		{
			memcpy(sps, start + 4, len_offset);
			*sps_len = len_offset;
		}
		else if(((start[4] >> 1) & 0x3F) == 0x22) //pps
		{
			memcpy(pps, start + 4, len_offset);
			*pps_len = len_offset;
			vps_sps_pps_len = *pps_len + *sps_len + *vps_len;
			return vps_sps_pps_len;
		}

		input_buff = end;
		len_temp -=  len_offset;
	}
	printf("*vps_len = %d, sps_len = %d, pps_len = %d\r\n",*vps_len,*sps_len,*pps_len);
	vps_sps_pps_len = *pps_len + *sps_len + *vps_len;
	return vps_sps_pps_len;
}

void hvcc_update_ptl(HEVCDecoderConfigurationRecord *hvcc, HVCCProfileTierLevel *ptl)
{
 
	hvcc->general_profile_space = ptl->profile_space;

	if (hvcc->general_tier_flag < ptl->tier_flag)
	    hvcc->general_level_idc = ptl->level_idc;
	else
	    hvcc->general_level_idc = FFMAX(hvcc->general_level_idc, ptl->level_idc);
	hvcc->general_tier_flag = FFMAX(hvcc->general_tier_flag, ptl->tier_flag);
	hvcc->general_profile_idc = FFMAX(hvcc->general_profile_idc, ptl->profile_idc);
	hvcc->general_profile_compatibility_flags &= ptl->profile_compatibility_flags;
	hvcc->general_constraint_indicator_flags &= ptl->constraint_indicator_flags;
}

void hvcc_parse_ptl(AR_U8 *data_buff, HEVCDecoderConfigurationRecord *hvcc, AR_U8 max_sub_layers_minus1)
{
	HVCCProfileTierLevel general_ptl;
	AR_U32 profile_compatibility_flags = 0;
	AR_U64 constraint_indicator_flags = 0;

	general_ptl.profile_space               = data_buff[0] & 0xc0;    //2bit
	general_ptl.tier_flag                   = data_buff[0] & 0x20;    //1bit
	general_ptl.profile_idc                 = data_buff[0] & 0x1f;   //5bit

	profile_compatibility_flags = (data_buff[1] << 24) | (data_buff[2] << 16) | (data_buff[3] << 8) | data_buff[4];
	general_ptl.profile_compatibility_flags  = profile_compatibility_flags; //32
	//printf("general_ptl.profile_compatibility_flags = %x\r\n",general_ptl.profile_compatibility_flags);

	constraint_indicator_flags = ((AR_U64)data_buff[5] << 56);
	constraint_indicator_flags |= ((AR_U64)data_buff[6] << 48);
	constraint_indicator_flags |= ((AR_U64)data_buff[7] << 40);
	constraint_indicator_flags |= ((AR_U64)data_buff[8] << 32);
	constraint_indicator_flags |= ((AR_U64)data_buff[9] << 24);
	constraint_indicator_flags |= ((AR_U64)data_buff[10] << 16);
	general_ptl.constraint_indicator_flags = constraint_indicator_flags;	//48

	general_ptl.level_idc                   = data_buff[11] & 0xff;	//8
	//printf("general_ptl.profile_space = %d, tier_flag = %d, general_ptl.profile_idc = %d, profile_compatibility_flags = %x, constraint_indicator_flags = %llu, level_idc = %d\r\n",
	//		general_ptl.profile_space, general_ptl.tier_flag, general_ptl.profile_idc, general_ptl.profile_compatibility_flags, general_ptl.constraint_indicator_flags, general_ptl.level_idc );
	
	hvcc_update_ptl(hvcc, &general_ptl);
}

AR_S32 iframe_write_hvcc(MovWriteHandle *handle)
{
	HEVCDecoderConfigurationRecord *iframe_hvcc;
	AR_U8 vps_max_sub_layers_minus1;
	AR_U8 sps_max_sub_layers_minus1;
	AR_U8 temporalIdNested;
	
	iframe_hvcc = &(handle->hvcc);
	hvcc_init(iframe_hvcc);

	vps_max_sub_layers_minus1 = (handle->vps[3]) & 0xe;  
	iframe_hvcc->numTemporalLayers = FFMAX(iframe_hvcc->numTemporalLayers, vps_max_sub_layers_minus1 + 1);
	//printf("iframe_hvcc->numTemporalLayers = %d\r\n", iframe_hvcc->numTemporalLayers);
	hvcc_parse_ptl(handle->vps + 6, iframe_hvcc, iframe_hvcc->numTemporalLayers);

	sps_max_sub_layers_minus1 = (handle->sps[2]) & 0xe;
	iframe_hvcc->numTemporalLayers = FFMAX(iframe_hvcc->numTemporalLayers, sps_max_sub_layers_minus1 + 1);
	temporalIdNested = (handle->sps[2]) & 0x1;
	iframe_hvcc->temporalIdNested = temporalIdNested;
	hvcc_parse_ptl(handle->sps + 3, iframe_hvcc, iframe_hvcc->numTemporalLayers);

	if (iframe_hvcc->min_spatial_segmentation_idc > MAX_SPATIAL_SEGMENTATION)
        	iframe_hvcc->min_spatial_segmentation_idc = 0;

    	if (!iframe_hvcc->min_spatial_segmentation_idc)
        	iframe_hvcc->parallelismType = 0;
	
    	iframe_hvcc->avgFrameRate      = 0;
    	iframe_hvcc->constantFrameRate = 0;
	return 0;
}

