#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <arm_neon.h>

#include "arcnn_yolov5.h"
#include "arcnn_base.h"

struct timeval tm_start;
struct timeval tm_end;

int anchors[9][2] = {{10, 13}, {16, 30}, {32, 23}, {30, 61}, {62, 45}, {59, 119}, {116, 90}, {156, 198}, {373, 326}};

void init_boxcolor(int cls_num, int *colors, unsigned char *livecolors)
{
	srand(time(NULL));
	int scopes[2] = {0, 240};
	int loop = 1000;
	int min_value = scopes[0];
	int max_value = scopes[1];
	int interval = (max_value - min_value) / cls_num;
	int *values = (int *)malloc(cls_num * sizeof(int));
	int *curAddr = colors;
	for(int i = 0; i < cls_num; i++)
		values[i] = min_value + i * interval;
	while(loop--){
		int x = rand() % cls_num;
		int y = rand() % cls_num;
		if(x != y){
			int tmp = values[x];
			values[x] = values[y];
			values[y] = tmp;
		}
		else
			continue;
	}
	for(int i = 0; i < cls_num; i++){
		curAddr[0] = values[i];
		curAddr[1] = rand() % 255;
		curAddr[2] = rand() % 255;
		livecolors[4 * i + 2] = (unsigned char)curAddr[0];
		livecolors[4 * i + 1] = (unsigned char)curAddr[1];
		livecolors[4 * i + 0] = (unsigned char)curAddr[2];
		curAddr += 3;
	}
	free(values);
}

void get_dims(int *dims1, int *dims2, AR_NPU_TENSOR_S stOutputTensor)
{
	dims1[0]=1;
	dims1[1]=3;
	dims1[2]=stOutputTensor.u32OriChannels/dims1[1];
	dims1[3]=stOutputTensor.u32Height;
	dims1[4]=stOutputTensor.u32Width;
	dims2[0]=1;
	dims2[1]=3;
	dims2[2]=dims1[3];
	dims2[3]=dims1[4];
	dims2[4]=dims1[2];
}

void reshape_proc(float *inbuf, float *outbuf, AR_NPU_TENSOR_S stOutputTensor)
{
	int orichannel = stOutputTensor.u32OriChannels;
	int w_c = orichannel * stOutputTensor.u32Height;
	for(int k = 0; k < w_c; k++){
		memcpy((void *)(outbuf + k * stOutputTensor.u32Width), (void *)(inbuf + k * stOutputTensor.u32RowStep/sizeof(float)), stOutputTensor.u32Width * sizeof(float));
	}
}

void permute_proc(float *inbuf, float *outbuf, int *indims)
{
	int b = 0, d = 0, c = 0, h = 0, w = 0;
	float value=0;
	int depth_size = indims[1] * indims[2] * indims[3] * indims[4];
	int channel_size = indims[2] * indims[3] * indims[4];
	int map_size_in = indims[3] * indims[4];
	int map_size_out = indims[4] * indims[2];
	for(b = 0; b < indims[0]; b++){
		for(d = 0; d < indims[1]; d++){
			for(c = 0; c < indims[2]; c++){
				for(h = 0; h < indims[3]; h++){
					for(w = 0; w < indims[4]; w++){
						value=inbuf[b * depth_size + d * channel_size + c * map_size_in + h * indims[4] + w];
						outbuf[b * depth_size + d * channel_size + h * map_size_out + w * indims[2] + c] = value;
					}
				}
			}
		}
	}
}

int argmax(float *ptr, int num)
{
	int idx = 0;
	float max_value = -1000000.f;
	for(int i = 0; i < num; i++){
		if(ptr[i] > max_value){
			max_value = ptr[i];
			idx = i;
		}
	}
	return idx;
}

int argmax_opt2(float *ptr, int num, int map_size)
{
	int idx = 0;
	float max_value = -1000000.f;
	int valid = 0;
	for(int i = 0; i < num; i++){
		if(ptr[valid] > max_value){
			max_value = ptr[valid];
			idx = i;
		}
		valid += map_size;
	}
	return idx;
}

int cmp(const void *a, const void *b)
{
	YOLOv5_RSLT box1 = *(YOLOv5_RSLT *)a;
	YOLOv5_RSLT box2 = *(YOLOv5_RSLT *)b;
	float diff = box1.score - box2.score;
	if(diff < 0) return 1;
	else if(diff > 0) return -1;
	return 0;
}

float calculate_iou(YOLOv5_RSLT a, YOLOv5_RSLT b)
{
	float a_area = (a.xmax - a.xmin + 1) * (a.ymax - a.ymin + 1);
	float b_area = (b.xmax - b.xmin + 1) * (b.ymax - b.ymin + 1);
	float left = GET_MAX(a.xmin, b.xmin);
	float right = GET_MIN(a.xmax, b.xmax);
	float top = GET_MAX(a.ymin, b.ymin);
	float bottom = GET_MIN(a.ymax, b.ymax);
	float width = GET_MAX(right - left + 1, 0.f);
	float height = GET_MAX(bottom - top + 1, 0.f);
	float u_area = height * width;
	float iou = u_area / (a_area + b_area - u_area);
	return iou;
}

int nms_proc(YOLOv5_RSLT *regionRslts, float nms_thr, int num, YOLOv5_RSLT *detRslts)
{
	int i = 0, j = 0;
	float iou = 0;
	int kept_num = 0;
	qsort(regionRslts, num, sizeof(YOLOv5_RSLT), cmp);
	int suppressed[MAX_NUM_REGION] = {0};
	for(i = 0; i < num; i++){
		if(suppressed[i] == 1)
			continue;
		detRslts[kept_num] = regionRslts[i];
		kept_num++;
		if(kept_num >= MAX_NUM_DET){
			printf("WRNING: too many detBoxes cause buf overflow!\n");
			return kept_num;
		}
		for(j = i + 1; j < num; j++){
			if(suppressed[j] == 1)
				continue;
			iou = calculate_iou(regionRslts[i], regionRslts[j]);
			if(iou >= nms_thr)
				suppressed[j] = 1;
		}
	}
	return kept_num;
}

float sigmoid_proc(float x)
{
	return 1.0 / (1.0 + exp(-x));
}

void sigmoid_layer(float *data, int tensor_size)
{
	for(int i = 0; i < tensor_size; i++)
		data[i] = sigmoid_proc(data[i]);
}

float muladd_layer(float x)
{
	return x * 2.f - 0.5f;
}

void slice_process(float *data, int *dims, int stride, int scale_id)
{
	int depth_size = dims[1] * dims[2] * dims[3] * dims[4];
	int channel_size = dims[2] * dims[3] * dims[4];
	int map_size = dims[3] * dims[4];
	int b = 0, d = 0, c = 0, h = 0, w = 0;
	float value = 0.f;
	for(b = 0; b < dims[0]; b++){
		for(d = 0; d < dims[1]; d++){
			for(c = 0; c < dims[2]; c++){
				for(h = 0; h < dims[3]; h++){
					for(w = 0; w < dims[4]; w++){
						value = data[b * depth_size + d * channel_size + c * map_size + h * dims[4] + w];
						if(w == 0){
							value = muladd_layer(value);
							value += h;
							value *= stride;
						}
						else if(w == 1){
							value = muladd_layer(value);
							value += c;
							value *= stride;
						}
						else if(w == 2){
							value *= 2.f;
							value = powf(value, 2);
							value *= anchors[scale_id * 3 + d][0];
						}
						else if(w == 3){
							value *= 2.f;
							value = powf(value, 2);
							value *= anchors[scale_id * 3 + d][1];
						}
						else
							break;
						data[b * depth_size + d * channel_size + c * map_size + h * dims[4] + w] = value;
					}
				}
			}
		}
	}
}

void perm_slice_process(float *data, float *outdata, int *dims, int stride, int scale_id, int RowStep)
{
	int depth_size = dims[1] * dims[2] * dims[3] * RowStep;
	int channel_size = dims[2] * dims[3] * RowStep;
	int map_size = dims[3] * RowStep;
	int channel_size_out = dims[2] * dims[3] * dims[4];
	int depth_size_out = channel_size_out * dims[1];
	int map_size_out = dims[2] * dims[4];
	int b = 0, d = 0, c = 0, h = 0, w = 0;
	float value = 0.f;
	for(b = 0; b < dims[0]; b++){ //1
		for(d = 0; d < dims[1]; d++){ //3
			for(c = 0; c < dims[2]; c++){ //85
				for(h = 0; h < dims[3]; h++){ //80
					for(w = 0; w < dims[4]; w++){ //80
						value = data[b * depth_size + d * channel_size + c * map_size + h * RowStep + w];
						if(c == 0){
							value = muladd_layer(value);
							value += w;
							value *= stride;
						}
						else if(c == 1){
							value = muladd_layer(value);
							value += h;
							value *= stride;
						}
						else if(c == 2){
							value *= 2.f;
							value = powf(value, 2);
							value *= anchors[scale_id * 3 + d][0];
						}
						else if(c == 3){
							value *= 2.f;
							value = powf(value, 2);
							value *= anchors[scale_id * 3 + d][1];
						}
						outdata[b * depth_size_out + d * channel_size_out + h * map_size_out + w * dims[2] + c] = value;
					}
				}
			}
		}
	}
}

int get_regionrslts(float *data, YOLOv5_RSLT *regionRslts, int lastdim, int lenth, YOLOv5_PARAS params)
{
	int cols = lenth / lastdim;
	int rows = lastdim;
	int region_num = 0, h = 0, addrOffset = 0, clsId = 0;
	float *currentAddr = data;
	float score = 0.f;
	for(h = 0; h < cols; h++){
		addrOffset = h * rows;
		currentAddr = data + addrOffset + 5;
		clsId = argmax(currentAddr, params.class_num);
		score = currentAddr[clsId] * data[addrOffset + 4];
		if(score >= params.conf_thr){
			regionRslts[region_num].xmin = data[addrOffset] - data[addrOffset + 2] / 2.f;
			regionRslts[region_num].ymin = data[addrOffset + 1] - data[addrOffset + 3] / 2.f;
			regionRslts[region_num].xmax = data[addrOffset] + data[addrOffset + 2] / 2.f;
			regionRslts[region_num].ymax = data[addrOffset + 1] + data[addrOffset + 3] / 2.f;
			regionRslts[region_num].cls_id = clsId;
			regionRslts[region_num].score = score;
			region_num++;
		}
	}
	return region_num;
}

void get_regionrslts_opt2(float *data, YOLOv5_RSLT *regionRslts, int *region_num, AR_NPU_TENSOR_S stOutputTensor, YOLOv5_PARAS params)
{
	int cols = stOutputTensor.u32Height;
	int rows = stOutputTensor.u32Width;
	int batch = 3;
	int channels = stOutputTensor.u32OriChannels / batch;
	int h = 0, w = 0, b = 0, clsId = 0;
	int RowStep = stOutputTensor.u32RowStep / 4;
	int addrOffset = channels * cols * RowStep;
	int confPtrOffset = (channels - params.class_num) * cols * RowStep;
	int map_size = cols * RowStep;
	float *currentAddr = data;
	float *confNowPtr;
	float *dataNowPtr;
	float score = 0.f;
	int num = 0;
	for(b = 0; b < batch; b++){
		currentAddr = data + b * addrOffset;
		for(h = 0; h < cols; h++){
			for(w = 0; w < rows; w++){
				dataNowPtr = currentAddr + h * RowStep + w;
				confNowPtr = currentAddr + confPtrOffset + h * RowStep + w;
				clsId = argmax_opt2(confNowPtr, params.class_num, map_size);
				score = confNowPtr[clsId * map_size] * dataNowPtr[4 * map_size];
				if(score >= params.conf_thr){
					regionRslts[*region_num + num].xmin = dataNowPtr[0] - dataNowPtr[2 * map_size] / 2.f;
					regionRslts[*region_num + num].ymin = dataNowPtr[map_size] - dataNowPtr[3 * map_size] / 2.f;
					regionRslts[*region_num + num].xmax = dataNowPtr[0] + dataNowPtr[2 * map_size] / 2.f;
					regionRslts[*region_num + num].ymax = dataNowPtr[map_size] + dataNowPtr[3 * map_size] / 2.f;
					regionRslts[*region_num + num].cls_id = clsId;
					regionRslts[*region_num + num].score = score;
					num++;
				}
			}
		}
	}
	*region_num += num;
}

void get_regionrslts_opt2_neon(float *data,YOLOv5_RSLT *regionRslts, int *region_num, AR_NPU_TENSOR_S stOutputTensor, YOLOv5_PARAS params)
{
	AR_U32 batch = 3;
	AR_U32 width = stOutputTensor.u32Width;
	AR_U32 height = stOutputTensor.u32Height;
	AR_U32 channel = stOutputTensor.u32OriChannels / 3;
	AR_U32 RowStep = stOutputTensor.u32RowStep / 4;
	AR_U32 map_size = height * RowStep;
	AR_U32 offset = channel * height * RowStep;
	AR_U32 length = width / 4;
	AR_U32 rmlen = width - length * 4;
	float max_value = -100000.0f;
	float score;
	AR_U32 b, h, c, w, l, i, clsId, num = 0;
	AR_U32 *vclsId;
	float *vconf;
	float *confAddr;
	float *addr;
	float *nowPtr;
	float *ptr = data;
	float *valptr;
	float32x4_t v0, v1, sv0, sv1, vscores;
	uint32x4_t vidx0, vmask, vidx1;
	AR_U32 strides[5] = {map_size, 2*map_size, 3*map_size, 4*map_size, 5*map_size};
	
	for(b = 0; b < batch; b++){
		for(h = 0; h < height; h++){
			addr = ptr + h * RowStep;
			//calculate vector of length 4
			for(l = 0; l < length; l++){
				v0 = vmovq_n_f32(max_value);
				vidx0 = vmovq_n_u32(0);
				for(c = 5; c < channel; c++){
					nowPtr = addr + c * map_size;
					v1 = vld1q_f32(nowPtr);
					vmask = vcgtq_f32(v1, v0);
					vidx1 = vmovq_n_u32(c);
					vidx0 = vbslq_u32(vmask, vidx1, vidx0);
					v0 = vmaxq_f32(v1, v0);
				}
				vclsId = (AR_U32 *)(&vidx0);
				for(i = 0; i < 4; i++){
					valptr = addr + i;
					sv0 = vsetq_lane_f32(valptr[vclsId[i] * map_size], sv0, i);
				}
				confAddr = addr + strides[3];
				sv1 = vld1q_f32(confAddr);
				vscores = vmulq_f32(sv0, sv1);
				vconf = (float *)(&vscores);
				for(i = 0; i < 4; i++){
					if(vconf[i] >= params.conf_thr){
						valptr = addr + i;
						regionRslts[*region_num + num].xmin = valptr[0] - valptr[strides[1]] / 2.f;
						regionRslts[*region_num + num].ymin = valptr[map_size] - valptr[strides[2]] / 2.f;
						regionRslts[*region_num + num].xmax = valptr[0] + valptr[strides[1]] / 2.f;
						regionRslts[*region_num + num].ymax = valptr[map_size] + valptr[strides[2]] / 2.f;
						regionRslts[*region_num + num].cls_id = vclsId[i] - 5;
						regionRslts[*region_num + num].score = vconf[i];
						num++;
					}
				}
				addr += 4;
			}
			//calculate remian part
			for(w = 0; w < rmlen; w++){
				clsId = argmax_opt2(addr + strides[4], params.class_num, map_size);
				score = addr[(clsId + 5) * map_size] * addr[strides[3]];
				if(score > params.conf_thr){
					regionRslts[*region_num + num].xmin = addr[0] - addr[strides[1]] / 2.f;
					regionRslts[*region_num + num].ymin = addr[map_size] - addr[strides[2]] / 2.f;
					regionRslts[*region_num + num].xmax = addr[0] + addr[strides[1]] / 2.f;
					regionRslts[*region_num + num].ymax = addr[map_size] + addr[strides[2]] / 2.f;
					regionRslts[*region_num + num].cls_id = clsId;
					regionRslts[*region_num + num].score = score;
					num++;
				}
				addr++;
			}
		}
		ptr += offset;
	}
	*region_num += num;
}

void transform(YOLOv5_PARAS params, YOLOv5_RSLT *detRslts, int num)
{
	int i = 0;
	for(i = 0; i < num; i++){
		detRslts[i].xmin = detRslts[i].xmin * params.ratio;
		detRslts[i].ymin = detRslts[i].ymin * params.ratio;
		detRslts[i].xmax = detRslts[i].xmax * params.ratio;
		detRslts[i].ymax = detRslts[i].ymax * params.ratio;
		if(detRslts[i].xmin < 0)
			detRslts[i].xmin = 0;
		if(detRslts[i].xmax > params.img_w)
			detRslts[i].xmax = params.img_w;
		if(detRslts[i].ymin < 0)
			detRslts[i].ymin = 0;
		if(detRslts[i].ymax > params.img_h)
			detRslts[i].ymax = params.img_h;
	}
}

int params_init(void *handle, YOLOv5_PARAS *params)
{
	int ret = 0;
	AR_NPU_TENSOR_S stInputTensor ={0};
	AR_MPI_NPU_GetInputTensorParam(handle, 0, &stInputTensor);
	params->net_w = stInputTensor.u32Width;
	params->net_h = stInputTensor.u32Height;

	AR_U32 OutputTensorNum = AR_MPI_NPU_GetOutputTensorNum(handle);
	AR_NPU_TENSOR_S stOutputTensor = {0};
	int MaxFtSize = -100000;
	for(AR_U32 i=0; i <OutputTensorNum; i++ ){
		AR_MPI_NPU_GetOutputTensorParam(handle,i,&stOutputTensor);
		if(!stOutputTensor.u32OriChannels)
			stOutputTensor.u32OriChannels = stOutputTensor.u32KNormNum * stOutputTensor.u32KSizeNorm + stOutputTensor.u32KSizeLast;
		params->map_size.FtSizes[i] = stOutputTensor.u32Width*stOutputTensor.u32Height*stOutputTensor.u32OriChannels;
		params->map_size.strides[i] = params->net_w / stOutputTensor.u32Width;
		params->map_size.NetOutSize += params->map_size.FtSizes[i];
		if (params->map_size.FtSizes[i] > MaxFtSize)
			MaxFtSize = params->map_size.FtSizes[i];
		if(i == 0)
			params->class_num = (int)(stOutputTensor.u32OriChannels /3 - 5);
	}
	params->map_size.MaxFtSize = MaxFtSize;
#if !OPT_POSTPROC
	params->buf.ContReshapeBuf = (float *)malloc(MaxFtSize * sizeof(float));
#endif
	params->buf.OutDataBuf = (float *)malloc(params->map_size.NetOutSize * sizeof(float));
	params->buf.regionRslts = (YOLOv5_RSLT *)malloc(MAX_NUM_REGION * sizeof(YOLOv5_RSLT));
	params->buf.colors = (int *)malloc(params->class_num * 3 * sizeof(int));
	params->buf.livecolors = (AR_UCHAR *)malloc(params->class_num * 4 * sizeof(AR_UCHAR));
	memset(params->buf.livecolors, 0, params->class_num * 4 * sizeof(AR_UCHAR));
	init_boxcolor(params->class_num, params->buf.colors, params->buf.livecolors);
	memset(params->buf.OutDataBuf, 0, params->map_size.NetOutSize * sizeof(float));
	return ret;
}

void params_uninit(YOLOv5_PARAS *params)
{
#if !OPT_POSTPROC
	free(params->buf.ContReshapeBuf);
#endif
	free(params->buf.OutDataBuf);
	free(params->buf.regionRslts);
	free(params->buf.colors);
	free(params->buf.livecolors);
}
#if 1
int yolov5_postprocess(char *NPUOutBuf, void * handle, YOLOv5_PARAS params, YOLOv5_RSLT *detRslts)
{
	AR_U32 OutputTensorNum = AR_MPI_NPU_GetOutputTensorNum(handle);
	AR_NPU_TENSOR_S stOutputTensor;

	float *currentAddr = params.buf.OutDataBuf;
	int reshape_dims[5]={0};
	int permute_dims[5]={0};
	int region_num = 0;
    for(AR_U32 i=0; i <OutputTensorNum; i++ )
    {
        AR_MPI_NPU_GetOutputTensorParam(handle,i,&stOutputTensor);
		if(!stOutputTensor.u32OriChannels)
			stOutputTensor.u32OriChannels = stOutputTensor.u32KNormNum * stOutputTensor.u32KSizeNorm + stOutputTensor.u32KSizeLast;
        AR_CHAR * OutOffsetAddr = (AR_CHAR *)NPUOutBuf +(stOutputTensor.u32Bank*0x2000000 + stOutputTensor.u32Offset);
	 	float *outaddr = (float *)OutOffsetAddr;
		get_dims(reshape_dims, permute_dims, stOutputTensor);
		if(params.postproc_version == 2){
#if OPT_NEON_REGION
			get_regionrslts_opt2_neon(outaddr, params.buf.regionRslts, &region_num, stOutputTensor, params);
#else
			get_regionrslts_opt2(outaddr, params.buf.regionRslts, &region_num, stOutputTensor, params);
#endif
		}
		else if(params.postproc_version == 1)
			perm_slice_process(outaddr, currentAddr, reshape_dims, params.map_size.strides[i], i, stOutputTensor.u32RowStep / 4);
		else{
			reshape_proc(outaddr, params.buf.ContReshapeBuf, stOutputTensor);
			permute_proc(params.buf.ContReshapeBuf, currentAddr, reshape_dims);
			sigmoid_layer(currentAddr, params.map_size.FtSizes[i]);
			slice_process(currentAddr, permute_dims, params.map_size.strides[i], i);
    	}
		currentAddr += params.map_size.FtSizes[i];       
    }
	if(params.postproc_version != 2)
		region_num = get_regionrslts(params.buf.OutDataBuf, params.buf.regionRslts, permute_dims[4], params.map_size.NetOutSize, params);
	int kept_num = nms_proc(params.buf.regionRslts, params.nms_thr, region_num, detRslts);
	transform(params, detRslts, kept_num);
	return kept_num;	
}
#endif
#if 0
int yolov5_postprocess(char *NPUOutBuf, void * handle, YOLOv5_PARAS params, YOLOv5_RSLT *detRslts)
{
	AR_U32 OutputTensorNum = AR_MPI_NPU_GetOutputTensorNum(handle);
	AR_NPU_TENSOR_S stOutputTensor;

	float *currentAddr = params.buf.OutDataBuf;
	int reshape_dims[5]={0};
	int permute_dims[5]={0};
	float rm =0.0;
    for(AR_U32 i=0; i <OutputTensorNum; i++ )
    {
        AR_MPI_NPU_GetOutputTensorParam(handle,i,&stOutputTensor);
        AR_CHAR * OutOffsetAddr = (AR_CHAR *)NPUOutBuf +(stOutputTensor.u32Bank*0x2000000 + stOutputTensor.u32Offset);
	 	float *outaddr = (float *)OutOffsetAddr;
		get_dims(reshape_dims, permute_dims, stOutputTensor);
		gettimeofday(&tm_start, NULL);
		perm_slice_process(outaddr, currentAddr, permute_dims, params.map_size.strides[i], i, stOutputTensor.u32RowStep);
		gettimeofday(&tm_end, NULL);
		rm = (float)(((tm_end.tv_sec * 1000000 + tm_end.tv_usec) - (tm_start.tv_sec * 1000000 + tm_start.tv_usec) )*1.0/1000);
		printf("slice proc run time: %f ms.\n", rm);
		currentAddr += params.map_size.FtSizes[i];       
    }
	gettimeofday(&tm_start, NULL);
	int region_num = get_regionrslts(params.buf.OutDataBuf, params.buf.regionRslts, permute_dims[4], params.map_size.NetOutSize, params);
	gettimeofday(&tm_end, NULL);
	rm = (float)(((tm_end.tv_sec * 1000000 + tm_end.tv_usec) - (tm_start.tv_sec * 1000000 + tm_start.tv_usec) )*1.0/1000);
	printf("get regionRslts run time: %f ms.\n", rm);
	gettimeofday(&tm_start, NULL);
	int kept_num = nms_proc(params.buf.regionRslts, params.nms_thr, region_num, detRslts);
	gettimeofday(&tm_end, NULL);
	rm = (float)(((tm_end.tv_sec * 1000000 + tm_end.tv_usec) - (tm_start.tv_sec * 1000000 + tm_start.tv_usec) )*1.0/1000);
	printf("nms proc run time: %f ms.\n", rm);
	gettimeofday(&tm_start, NULL);
	transform(params, detRslts, kept_num);
	gettimeofday(&tm_end, NULL);
	rm = (float)(((tm_end.tv_sec * 1000000 + tm_end.tv_usec) - (tm_start.tv_sec * 1000000 + tm_start.tv_usec) )*1.0/1000);
	printf("transform run time: %f ms.\n", rm);
	return kept_num;	
}
#endif
