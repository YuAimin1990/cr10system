#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "util_common.h"
#include "ssd.h"

#define SSD_MAX(a,b) (a>b? a:b)
#define SSD_MIN(a,b) (a<b? a:b)

static const char std_ssd_labels[32][32] =
{
    "__background__",
    "aeroplane",
    "bicycle",
    "bird",
    "boat",
    "bottle",
    "bus",
    "car",
    "cat",
    "chair",
    "cow",
    "diningtable",
    "dog",
    "horse",
    "motorbike",
    "person",
    "pottedplant",
    "sheep",
    "sofa",
    "train",
    "tvmonitor"
};

void SetDetectParam(SSD_NET_TYPE_E net_type, SSD_DECT_PARAM_S *dect_param, int class_num, int input_width, int input_height, float nms_threshold, float dect_threshold)
{
    float aspect_ratios1[6][4] = { 
        { 2.0f, 1 / 2.0f, -1.0f, -1 / 1.0f }, 
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f },
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f },
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f },
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f },
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f } 
    };
    
    float aspect_ratios2[6][4] = { 
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f },
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f },
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f },
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f },
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f },
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f } 
    };
    
    float aspect_ratios3[6][4] = {
        { 2.0f, 1 / 2.0f, -1.0f, -1 / 1.0f },
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f },
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f },
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f },
        { 2.0f, 1 / 2.0f, 3.0f, 1 / 3.0f },
        { 2.0f, 1 / 2.0f, -1.0f, -1 / 1.0f}
    };
    
    float delta_stds[4] = { 0.1f, 0.1f, 0.2f, 0.2f };
    int conf_idex[6] = { 1, 3, 5, 7, 9, 11 };
    int loc_idex[6] = { 0, 2, 4, 6, 8, 10 };
    int min_size1[6] = { 60, 105, 150, 195, 240, 285 };
    int max_size1[6] = { -1, 150, 195, 240, 285, 300 };
    int min_size2[6] = { 60, 105, 150, 195, 240, 285 };
    int max_size2[6] = { 105, 150, 195, 240, 285, 300 };
    int min_size3[6] = { 21, 45, 99, 153, 207, 261 };
    int max_size3[6] = { 45, 99, 153, 207, 261, 315 };
    float step[2][6] = { 
        { 8.0f, 16.0f, 32.0f, 64.0f, 100.0f, 300.0f },
        { 8.0f, 16.0f, 32.0f, 64.0f, 100.0f, 300.0f } 
    };
		
    switch (net_type)
    {
        case MOBILENETSSD_300:
            dect_param->class_num = class_num;
            dect_param->per_box_num = 4;
            dect_param->image_width = input_width;
            dect_param->image_height = input_height;
            dect_param->keep_box_max_num = 100;
            dect_param->nms_threshold = nms_threshold;
            dect_param->dect_threshol = dect_threshold;
            dect_param->nms_overlap_top_num = 200;
            dect_param->windows_num = 1917;
            memcpy(dect_param->delta_stds, delta_stds, sizeof(dect_param->delta_stds));
            dect_param->delta_stds_num = 4;
            memcpy(dect_param->aspect_ratios, aspect_ratios1, sizeof(dect_param->aspect_ratios));//dect_param->aspect_ratios = ;
            dect_param->aspect_ratios_num = 6;
            memcpy(dect_param->min_size, min_size1, sizeof(dect_param->min_size));
            dect_param->min_size_num = 6;
            memcpy(dect_param->max_size, max_size1, sizeof(dect_param->max_size));
            dect_param->max_size_num = 6;
            break;
        case MOBILENETSSD_PYTORCH_300:
            dect_param->class_num = class_num;
            dect_param->per_box_num = 4;
            dect_param->image_width = input_width;
            dect_param->image_height = input_height;
            dect_param->keep_box_max_num = 100;
            dect_param->nms_threshold = nms_threshold;
            dect_param->dect_threshol = dect_threshold;
            dect_param->nms_overlap_top_num = 200;
            dect_param->windows_num = 3000;
            memcpy(dect_param->delta_stds, delta_stds, sizeof(dect_param->delta_stds));
            dect_param->delta_stds_num = 4;
            memcpy(dect_param->aspect_ratios, aspect_ratios2, sizeof(dect_param->aspect_ratios));//dect_param->aspect_ratios = ;
            dect_param->aspect_ratios_num = 6;
            memcpy(dect_param->min_size, min_size2, sizeof(dect_param->min_size));
            dect_param->min_size_num = 6;
            memcpy(dect_param->max_size, max_size2, sizeof(dect_param->max_size));
            dect_param->max_size_num = 6;
            break;
        case SQUEEZENETSSD_300:
            dect_param->class_num = class_num;
            dect_param->per_box_num = 4;
            dect_param->image_width = input_width;
            dect_param->image_height = input_height;
            dect_param->keep_box_max_num = 100;
            dect_param->nms_threshold = nms_threshold;
            dect_param->dect_threshol = dect_threshold;
            dect_param->nms_overlap_top_num = 200;
            dect_param->windows_num = 8030; 
            memcpy(dect_param->conf_idex, conf_idex, sizeof(dect_param->conf_idex));
            dect_param->conf_idex_num = 6;
            memcpy(dect_param->loc_idex, loc_idex, sizeof(dect_param->loc_idex));
            dect_param->loc_idex_num = 6;
            memcpy(dect_param->delta_stds, delta_stds, sizeof(dect_param->delta_stds));
            dect_param->delta_stds_num = 4;
            memcpy(dect_param->aspect_ratios, aspect_ratios2, sizeof(dect_param->aspect_ratios));
            dect_param->aspect_ratios_num = 6;
            memcpy(dect_param->min_size, min_size3, sizeof(dect_param->min_size));
            dect_param->min_size_num = 6;
            memcpy(dect_param->max_size, max_size3, sizeof(dect_param->max_size));
            dect_param->max_size_num = 6;
            memcpy(dect_param->step, step, sizeof(dect_param->step)); 
            dect_param->step_mum = 2;
            break;
        default:
            break;
    }
}

void GetPriorBoxesSSD(SSD_DECTION_S * dection)
{
    int idx = 0;
    AR_NPU_TENSOR_S *pTensor = NULL;
    float img_height = (float)dection->dect_param.image_height;
    float img_width = (float)dection->dect_param.image_width;
    float * priorbox_tmp = dection->priorbox;
	
    for (int i = 0; i < dection->tensorNum/2; i++)
    {
        int layer_height = dection->pTensor[2*i].u32Height;
        int layer_width = dection->pTensor[2*i].u32Width;
        
        float step_h;
        float step_w;
        if (dection->dect_param.step_mum == 0)
        {
            step_h = img_height / layer_height;
            step_w = img_width / layer_width;
        }
        else
        {
            step_h = dection->dect_param.step[0][i];
            step_w = dection->dect_param.step[1][i];
        }
        for (int h = 0; h < layer_height; h++)
        {
            for (int w = 0; w < layer_width; w++)
            {
                //printf("==== h %d, w %d, layer_height %d, layer_width %d, dection->dect_param.per_box_num %d\n", h, w, layer_height,layer_width, dection->dect_param.per_box_num);
                float center_x = (w + 0.5) * step_w;
                float center_y = (h + 0.5) * step_h;
                float box_width, box_height;
                box_width = box_height = dection->dect_param.min_size[i];
    
                *priorbox_tmp = (center_x - box_width / 2.0) / img_width;
                *(priorbox_tmp+1) = (center_y - box_height / 2.0) / img_height;
                *(priorbox_tmp+2) = (center_x + box_width / 2.0) / img_width;
                *(priorbox_tmp+3) = (center_y + box_height / 2.0) / img_height;
                priorbox_tmp += 4;
				
                if (dection->dect_param.max_size[i] > 0) 
				{
                    box_width = box_height = sqrt(dection->dect_param.min_size[i] * dection->dect_param.max_size[i]);
                    
                    *priorbox_tmp = (center_x - box_width / 2.0) / img_width;
                    *(priorbox_tmp+1) = (center_y - box_height / 2.0) / img_height;
                    *(priorbox_tmp+2) = (center_x + box_width / 2.0) / img_width;
                    *(priorbox_tmp+3) = (center_y + box_height / 2.0) / img_height;
                    priorbox_tmp += 4;
                }
                for (int r = 0; r < dection->dect_param.per_box_num; r++)
				{
                    float ar = dection->dect_param.aspect_ratios[i][r];
                    if (fabs(ar - 1.) < 1e-6 || ar < 0.) 
					{
                        continue;
                    }
					
                    box_width = dection->dect_param.min_size[i] * sqrt(ar);
                    box_height = dection->dect_param.min_size[i] / sqrt(ar);
                    
                    // xmin
                    *(priorbox_tmp) = (center_x - box_width / 2.0) / img_width;
                    // ymin
                    *(priorbox_tmp+1) = (center_y - box_height / 2.0) / img_height;
                    // xmax
                    *(priorbox_tmp+2) = (center_x + box_width / 2.0) / img_width;
                    // ymax
                    *(priorbox_tmp+3) = (center_y + box_height / 2.0) / img_height;
                    priorbox_tmp += 4;
                }
            }
        }
    }
}

void CHW_ShortToHWC_Float(float *dst, short *src, AR_NPU_TENSOR_S *pstensor, int index)
{
    int channel = pstensor->u32OriChannels;
    int width = pstensor->u32Width;
    int height = pstensor->u32Height;
    
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            for (int c = 0; c < channel; c++)
            {
                dst[index + h*width*channel + w*channel + c] = (src[c*height*width + h*width+w]- pstensor->s32ZeroPoint)*pstensor->dScaleFactor;
            }
        }
    }
}

void CHW_CharToHWC_Float(float *dst,char *src, AR_NPU_TENSOR_S *pstensor, int index)
{
    int channel = pstensor->u32OriChannels;
    int width = pstensor->u32Width;
    int height = pstensor->u32Height;
    
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            for (int c = 0; c < channel; c++)
            {
                dst[index + h*width*channel + w*channel + c] = (src[c*height*width + h*width+w]- pstensor->s32ZeroPoint)*pstensor->dScaleFactor;
            }
        }
    }
}

void CHW_PermuteSSD(void *ssdBuff, SSD_DECTION_S * dection)
{
    int dst_offset = 0;
    AR_NPU_TENSOR_S *pTensor = NULL;

    for (int i = 0; i < dection->tensorNum / 2; i++)
    {
        pTensor = (AR_NPU_TENSOR_S *)(dection->pTensor + 2 * i + 1);
        int channel = pTensor->u32OriChannels;
        int width = pTensor->u32Width;
        int height = pTensor->u32Height;
        if(pTensor->u32Precision == 8)
        {
            char *ssdBuffTmp = (char *)ssdBuff;
            int offset = (pTensor->u32Bank * 32 * 1024 * 1024 + pTensor->u32Offset) / sizeof(char);
            char *src = ssdBuffTmp + offset;
            CHW_CharToHWC_Float(dection->scores_flat, src, pTensor, dst_offset);
        }
        else
        {
            //printf("tensor num = %d, i = %d\n", dection->tensorNum, i);
            short *ssdBuffTmp = (short *)ssdBuff;
            int offset = (pTensor->u32Bank * 32 * 1024 * 1024 + pTensor->u32Offset) / sizeof(short);
            short *src = ssdBuffTmp + offset;
            CHW_ShortToHWC_Float(dection->scores_flat, src, pTensor, dst_offset);
        }
        dst_offset += width*height*channel;
    }
	
    dst_offset = 0;
    for (int i = 0; i < dection->tensorNum / 2; i++)
    {
        pTensor = (AR_NPU_TENSOR_S *)(dection->pTensor + 2 * i);
        int channel = pTensor->u32OriChannels;
        int width = pTensor->u32Width;
        int height = pTensor->u32Height;
        if(pTensor->u32Precision == 8)
        {
            char *ssdBuffTmp = (char *)ssdBuff;
            int offset = (pTensor->u32Bank * 32 * 1024 * 1024 + pTensor->u32Offset) / sizeof(char);
            char *src = ssdBuffTmp + offset;
            CHW_CharToHWC_Float(dection->deltas_flat, src, pTensor, dst_offset);
        }
        else
        {
            short *ssdBuffTmp = (short *)ssdBuff;
            int offset = (pTensor->u32Bank * 32 * 1024 * 1024 + pTensor->u32Offset) / sizeof(short);
            short *src = ssdBuffTmp + offset;
            CHW_ShortToHWC_Float(dection->deltas_flat, src, pTensor, dst_offset);
        }
        dst_offset += width*height*channel;
    }

#if 0
    if(cnt == 0)
    {
    FILE * fp1 = NULL;
    char name[32] = {0};
    sprintf(name, "/mnt/score_brfore_%d.txt", cnt);
    fp1 = fopen(name, "wb+");
    fwrite((AR_CHAR *)dection->scores_flat, sizeof(float), dection->scores_flat_num, fp1);
    fclose(fp1);
    
    FILE * fp2 = NULL;
    char name2[32] = {0};
    sprintf(name2, "/mnt/deltas_before_%d.txt", cnt);
    fp2 = fopen(name2, "wb+");
    fwrite((AR_CHAR *)dection->deltas_flat, sizeof(float), dection->deltas_flat_num, fp2);
    fclose(fp2);
    cnt++;
    }
#endif
}

AR_S32 SoftmaxSSD(SSD_DECTION_S * dection)
{
    int windouws_num = dection->dect_param.windows_num;
    int class_num = dection->dect_param.class_num;
    float* prob = NULL;
    for (int i = 0; i < windouws_num; ++i)
    {
        prob = dection->scores_flat + i*class_num;
        for (int j = 0; j < class_num; ++j)
        {
            dection->softmaxScore[j] = prob[j] - prob[0];
        }
        
        float e_sum = 0.0;
        for (int j = 0; j < class_num; ++j)
        {
            dection->softmaxScore[j] = exp(dection->softmaxScore[j]);
            e_sum += dection->softmaxScore[j];
        }
        
        float inv_e_sum = 1.0f / e_sum;
        for (int j = 0; j < class_num; ++j)
        {
            prob[j] = dection->softmaxScore[j] * inv_e_sum;
        }
    }

#if 0
    if(cnt == 1)
    {
        FILE * fp1 = NULL;
        char name[32] = {0};
        sprintf(name, "/mnt/score_%d.txt", cnt);
        fp1 = fopen(name, "w+");
        fwrite((AR_CHAR *)dection->scores_flat, sizeof(float), windouws_num * class_num, fp1);
        fclose(fp1);
        cnt++;
    }
#endif	
	return 0;
}

void AscendSortIndexSSD(SSD_DECTION_S * dection)
{
    int *sort = dection->decodeSort.sort;
    float *data = dection->decodeSort.values;
    int length = dection->decodeSort.num;
    for (int m = 0; m < length; m++)
    {
        sort[m] = m;
    }
	
    const int bubble_level = 8;
    struct
    {
        int lb, ub;
    } stack[48];

    int sp = 0;
    int   temp;
    int   lb_val;
    
    stack[0].lb = 0;
    stack[0].ub = length - 1;

    while (sp >= 0)
    {
        int lb = stack[sp].lb;
        int ub = stack[sp--].ub;

        for (;;)
        {
            int diff = ub - lb;
            if (diff < bubble_level)
            {
                int i, j;
                int* arr = sort + lb;
                
                for (i = diff; i > 0; i--)
                {
                    int f = 0;
                    for (j = 0; j < i; j++)
                    {
                        if (data[arr[j + 1]] < data[arr[j]])
                        {
                            temp = arr[j];
                            arr[j] = arr[j + 1];
                            arr[j + 1] = temp;
                            f = 1;
                        }
                    }
                    if (!f)
                        break;
                }
                break;
            }
            else
            {
                /* select pivot and exchange with 1st element */
                int  m = lb + (diff >> 1);
                int  i = lb + 1, j = ub;
                
                lb_val = sort[m];
                
                sort[m] = sort[lb];
                sort[lb] = lb_val;
                
                /* partition into two segments */
                for (;;)
                {
                    for (; i < j && data[sort[i]] < data[lb_val]; i++);
                    for (; j >= i && data[lb_val] < data[sort[j]]; j--);
                    
                    if (i >= j) break;
                    temp = sort[i];
                    sort[i++] = sort[j];
                    sort[j--] = temp;
                }
            
                /* pivot belongs in A[j] */
                sort[lb] = sort[j];
                sort[j] = lb_val;
            
                /* keep processing smallest segment, and stack largest*/
                if (j - lb <= ub - j)
                {
                    if (j + 1 < ub)
                    {
                        stack[++sp].lb = j + 1;
                        stack[sp].ub = ub;
                    }
                    ub = j - 1;
                }
                else
                {
                    if (j - 1 > lb)
                    {
                        stack[++sp].lb = lb;
                        stack[sp].ub = j - 1;
                    }
                    lb = j + 1;
                }
            }
		}
	}
}

void BoxTransformSSD(float* tpbox, float *priorbox, SSD_DECTION_S * dection)
{
    float box0, box1, box2, box3;
    float *delta = tpbox;
    
    float tpw0 = priorbox[0];
    float tpw1 = priorbox[1];
    float tpw2 = priorbox[2];
    float tpw3 = priorbox[3];
    
    float b_x = (tpw0 + tpw2)*0.5;
    float b_y = (tpw1 + tpw3)*0.5;
    float b_w = (tpw2 - tpw0);
    float b_h = (tpw3 - tpw1);
    
    float dx = delta[0] * dection->dect_param.delta_stds[0];
    float dy = delta[1] * dection->dect_param.delta_stds[1];
    float dw = delta[2] * dection->dect_param.delta_stds[2];
    float dh = delta[3] * dection->dect_param.delta_stds[3];
    
    
    float x = dx * b_w + b_x;
    float y = dy * b_h + b_y;
    
    float w = exp(dw) * b_w;
    float h = exp(dh) * b_h;
    
    float half_w = w * 0.5;
    float half_h = h * 0.5;
    
    box0 = x - half_w;
    box1 = y - half_h;
    box2 = x + half_w;
    box3 = y + half_h;
    
#if 1
    box0 = SSD_MIN(box0, 1.f);  delta[0] = SSD_MAX(box0, 0.f);
    box1 = SSD_MIN(box1, 1.f);  delta[1] = SSD_MAX(box1, 0.f);
    box2 = SSD_MIN(box2, 1.f);  delta[2] = SSD_MAX(box2, 0.f);
    box3 = SSD_MIN(box3, 1.f);  delta[3] = SSD_MAX(box3, 0.f);
#else
    delta[0] = box0;
    delta[1] = box1;
    delta[2] = box2;
    delta[3] = box3;
#endif
}

int NonMaxSuppressSSD(float * boxes, int num_boxes, SSD_DECTION_S * dection)
{
    int i;
    float *areas, *pool;
    //compute areas
    if(!num_boxes)
    {
        return 0;
    }
	
    areas = (float *)malloc(sizeof(float) * num_boxes);
    if(!areas)
    {
        return 0;
    }
    pool = (float *)malloc(sizeof(float) * num_boxes);
    if(!pool)
    {
        return 0;
    }
	
    for (i = 0; i < num_boxes; i++)
    {
        float * box = boxes + i*4;
        areas[i] = (box[2] - box[0])*(box[3] - box[1]);
    }

    for (i = 0; i < num_boxes; i++) 
    {
        pool[i] = i;
    }
	
    int num_pools = num_boxes;
    int num_keeps = 0;
	
    while (num_pools > 0)
    {
        int max = pool[0];
        dection->nms.keep[num_keeps] = max;
        num_keeps++;
		
        if (num_pools == 1) 
        {
            break;
        }
		
        if (num_keeps >= dection->dect_param.keep_box_max_num) 
        {
            break;
        }
		
        // remove overlapping boxes
        float * box = boxes+max*4;
        float x0 = box[0];
        float y0 = box[1];
        float x1 = box[2];
        float y1 = box[3];
        float a = areas[max];

        int num_pools1 = 0;
        for (i = 1; i < num_pools; i++)
        {
             //compute overlap
             int j = pool[i];
             box = boxes+j*4;
             float xx0 = box[0];
             float yy0 = box[1];
             float xx1 = box[2];
             float yy1 = box[3];
             float aa = areas[j];
             float w = SSD_MIN(xx1, x1) - SSD_MAX(xx0, x0);
             float h = SSD_MIN(yy1, y1) - SSD_MAX(yy0, y0);
             float intersect = SSD_MAX(0.f, w)* SSD_MAX(0.f, h);
             float overlap = intersect / (a + aa - intersect);
             
             if (overlap < dection->dect_param.nms_threshold)
             {
                 pool[num_pools1] = j;
                 num_pools1++;
             }
        }
        num_pools = num_pools1;
    }
	
    if(areas)
    {
        free(areas);
    }
    if(pool)
    {
        free(pool);
    }
	
	return num_keeps;
}

int SSD_BboxDecoding(SSD_DECTION_OUTPUT_S *dets, SSD_DECTION_S * dection)
{
    SSD_DECTION_OUTPUT_S *dets_temp = dets;
    SoftmaxSSD(dection);
    int num_dets = 0;
    int c, n, i;

    for (c = 1; c < dection->dect_param.class_num; c++)
    {
        dection->decodeSort.num = 0;
        for (n = 0; n< dection->dect_param.windows_num; n++)
        {
            int scores_flat_idex = n*dection->dect_param.class_num + c;
            if (dection->scores_flat[scores_flat_idex]>dection->dect_param.dect_threshol)
            {
                dection->decodeSort.values[dection->decodeSort.num] = dection->scores_flat[scores_flat_idex];
                dection->decodeSort.index[dection->decodeSort.num] = n;
                dection->decodeSort.num++;
                //printf("Sort Num = %d, scores_flat[scores_flat_idex] = %lf, dect_param.dect_threshold = %lf\n", dection->decodeSort.num, dection->scores_flat[scores_flat_idex], dection->dect_param.dect_threshol);
            }
        }
        //printf("dection->decodeSort.num = %d, dection->dect_param.dect_threshol = %f\n", dection->decodeSort.num, dection->dect_param.dect_threshol);
        
        AscendSortIndexSSD(dection);
        
        float *boxes = NULL;
        float *boxes_tmp = NULL;
        int num_boxes = SSD_MIN(dection->decodeSort.num, dection->dect_param.nms_overlap_top_num);
        if(num_boxes)
        {
            boxes = (float *)malloc(sizeof(float) * num_boxes * 4);
            memset(boxes, 0, sizeof(float) * num_boxes * 4);
            boxes_tmp = boxes;
        }
	
        for (i = 0; i < num_boxes; i++)
        {
            int j =  dection->decodeSort.index[dection->decodeSort.sort[num_boxes - 1 - i]];
            int idex_sf = j* dection->dect_param.class_num + c;
            dection->nms.scores[i] =  dection->scores_flat[idex_sf];
            float tpbox[4] = {0.0};
            int idex_df = j* dection->dect_param.per_box_num;
            tpbox[0] =  dection->deltas_flat[idex_df];
            tpbox[1] =  dection->deltas_flat[idex_df + 1];
            tpbox[2] =  dection->deltas_flat[idex_df + 2];
            tpbox[3] =  dection->deltas_flat[idex_df + 3];
            BoxTransformSSD(tpbox, dection->priorbox+j*dection->dect_param.per_box_num, dection);
            float box_temp[4] = {0.0};
            box_temp[0] = tpbox[0];
            box_temp[1] = tpbox[1];
            box_temp[2] = tpbox[2];
            box_temp[3] = tpbox[3];
            memcpy(boxes_tmp, box_temp, sizeof(float)*4);
            boxes_tmp += 4;
        }

        SSD_DECTION_OUTPUT_S d;
        int num_keeps = NonMaxSuppressSSD(boxes, num_boxes, dection);
        //printf("num_keeps = %d\n", num_keeps);

        for (i = 0; i < num_keeps; i++)
        {
            int j = dection->nms.keep[i];
            
            float * box = boxes+j*4;
            d.x = box[0] *  dection->dect_param.image_width;
            d.y = box[1] *  dection->dect_param.image_height;
            d.w = box[2] *  dection->dect_param.image_width - d.x;
            d.h = box[3] *  dection->dect_param.image_height - d.y;
            d.classId = c;
            d.confidence =  dection->nms.scores[j];
            memcpy(dets_temp, &d, sizeof(SSD_DECTION_OUTPUT_S));
            dets_temp++;
            num_dets++;
        }

        if(boxes)
        {
            free(boxes);
            boxes = NULL;
        }
    }
	
    return num_dets;
}

AR_S32 SSD_CHW_PostProcess(void         *ssdBuff, SSD_DECTION_OUTPUT_S *dets, SSD_DECTION_S * dection)
{
    int num_dets = 0;
    SSD_DECTION_OUTPUT_S *dets_tmp = dets;
    SSD_DECTION_OUTPUT_S stdets = {0};
    int i, j;
    
    CHW_PermuteSSD(ssdBuff, dection);
    num_dets = SSD_BboxDecoding(dets, dection);
    
    for(i = 0; i < num_dets; i++)
    {
        for(j = 0; j < num_dets - i; j++)
        {
            if(dets_tmp[i].confidence < dets_tmp[j].confidence)
            {
                memcpy(&stdets, &dets_tmp[i], sizeof(SSD_DECTION_OUTPUT_S));
                memcpy(&dets_tmp[i], &dets_tmp[j], sizeof(SSD_DECTION_OUTPUT_S));
                memcpy(&dets_tmp[j], &stdets, sizeof(SSD_DECTION_OUTPUT_S));
            }
        }
    }
    
    return num_dets;
}

void SSD_detection(SSD_NET_TYPE_E net_type, SSD_DECTION_S * dection)
{
    int class_num = 21;
    float nms_threshold = 0.45f;
    float dect_threshold = 0.5f;
    int input_width = 300;
    int input_height = 300;
    
    SetDetectParam(net_type, &dection->dect_param, class_num, input_width, input_height, nms_threshold, dect_threshold);

    dection->decodeSort.num = dection->dect_param.windows_num;
    dection->decodeSort.values = (float *)malloc(sizeof(float) * dection->decodeSort.num);
    dection->decodeSort.index = (int *)malloc(sizeof(int) * dection->decodeSort.num);
    dection->decodeSort.sort = (int *)malloc(sizeof(int) * dection->decodeSort.num);
    dection->softmaxScore_num = dection->dect_param.class_num;
    dection->softmaxScore = (float *)malloc(sizeof(float) * dection->softmaxScore_num);
    dection->nms.num = dection->dect_param.nms_overlap_top_num;
    dection->nms.keep = (int *)malloc(sizeof(int) * dection->nms.num);
    dection->nms.scores = (float *)malloc(sizeof(float) * dection->nms.num);
    dection->scores_flat_num = dection->dect_param.class_num*dection->dect_param.windows_num;
    dection->scores_flat = (float *)malloc(sizeof(float) * dection->scores_flat_num);
    dection->deltas_flat_num = dection->dect_param.per_box_num*dection->dect_param.windows_num;
    dection->deltas_flat = (float *)malloc(sizeof(float) * dection->deltas_flat_num);
    
    dection->priorbox_num = dection->dect_param.windows_num * dection->dect_param.per_box_num;
    dection->priorbox = (float *)malloc(sizeof(float) * dection->priorbox_num);
    memset(dection->priorbox, 0,  sizeof(float) * dection->priorbox_num);

    #if 0
    printf("dection->decodeSort.num = %d\n", dection->decodeSort.num);
    printf("dection->softmaxScore_num = %d\n", dection->softmaxScore_num);
    printf("dection->nms.num = %d\n", dection->nms.num);
    printf("dection->scores_flat_num = %d\n", dection->scores_flat_num);
    printf("dection->deltas_flat_num = %d\n", dection->deltas_flat_num);
    printf("dection->priorbox_num = %d\n", dection->priorbox_num);	
    #endif
    GetPriorBoxesSSD(dection);

}

void SSD_Undetection(SSD_DECTION_S * dection)
{
    if(dection->decodeSort.values){
        free(dection->decodeSort.values);
    }
    if(dection->decodeSort.index){
        free(dection->decodeSort.index);
    }
    if(dection->decodeSort.sort){
        free(dection->decodeSort.sort);
    }
    if(dection->softmaxScore){
        free(dection->softmaxScore);
    }
    if(dection->nms.keep){
        free(dection->nms.keep);
    }
    if(dection->nms.scores){
        free(dection->nms.scores);
    }
    if(dection->scores_flat){
        free(dection->scores_flat);
    }
    if(dection->deltas_flat){
        free(dection->deltas_flat);
    }
    if(dection->priorbox){
        free(dection->priorbox);
    }
}

AR_S32 IPC_SVP_GetSsdPostProcessResult(void * handle, AR_MEM_S *stNpuOutBuffer, IPC_SSD_OUTPUT_S * pstSsdOutput)
{
	AR_S32 s32Ret = 0;
	SSD_DECTION_S dection;
	AR_U32 u32OutSize = 0;
	AR_U32 u32OutTensorNum = 0;
	AR_NPU_TENSOR_S *pstOutputTensor, *pstOutputTensorTmp;
	SSD_DECTION_OUTPUT_S *result_dect = NULL;

	AR_MPI_NPU_InvalidCachedBuff(stNpuOutBuffer);

    u32OutSize = AR_MPI_NPU_GetOutputBuffSize(handle);

	AR_CHAR * NCHWBuff = (AR_CHAR *)malloc(u32OutSize);
    memset(NCHWBuff, 0, u32OutSize);
    u32OutTensorNum = AR_MPI_NPU_GetOutputTensorNum(handle);

	pstOutputTensor = (AR_NPU_TENSOR_S *)malloc(sizeof(AR_NPU_TENSOR_S) * u32OutTensorNum);
	if(!pstOutputTensor)
	{
		PRINT_ERR("malloc output tensor failed. \n");
		return -1;
	}
	memset(pstOutputTensor, 0, sizeof(AR_NPU_TENSOR_S) * u32OutTensorNum);

	pstOutputTensorTmp = pstOutputTensor;
    for(AR_U32 i=0; i <u32OutTensorNum; i++)
    {
    	s32Ret =AR_MPI_NPU_GetOutputTensorParam(handle, i, pstOutputTensorTmp);
    	if(s32Ret<0)
    	{
            PRINT_ERR("get_output_tensor_params error. \n");
			free(NCHWBuff);
			free(pstOutputTensor);
            return -1;
    	}

   	    AR_CHAR * InBuffAddr = (AR_CHAR *)stNpuOutBuffer->u64VirtAddr +(pstOutputTensorTmp->u32Bank* 0x2000000 + pstOutputTensorTmp->u32Offset);
    	AR_CHAR * OutBuffAddr = (AR_CHAR *)NCHWBuff +(pstOutputTensorTmp->u32Bank* 0x2000000 + pstOutputTensorTmp->u32Offset);
	    AR_NPU_Outbuff_NhwcCvtNchw(InBuffAddr, OutBuffAddr, pstOutputTensorTmp);

		pstOutputTensorTmp++;
    }

	memset(&dection, 0, sizeof(SSD_DECTION_S));
	dection.tensorNum = u32OutTensorNum;
	dection.pTensor = pstOutputTensor;
	
	SSD_detection(MOBILENETSSD_300, &dection);
	
	result_dect = (SSD_DECTION_OUTPUT_S *)malloc(IPC_ALG_OSD_MAX_NUM * sizeof(SSD_DECTION_OUTPUT_S));
    if(!result_dect)
    {
        PRINT_ERR("Malloc result_dect failed.\n");
		free(pstOutputTensor);
        free(NCHWBuff);
        return -1;
    }

	AR_S32 s32NumDect = SSD_CHW_PostProcess((void *)NCHWBuff, result_dect, &dection);

#if 1
	pstSsdOutput->u32OutNum = s32NumDect;
	if(s32NumDect)
	{
    	pstSsdOutput->pstSsdInfo = (IPC_SSD_OUTPUT_INFO_S *)malloc(s32NumDect * sizeof(IPC_SSD_OUTPUT_INFO_S));
    	if(!pstSsdOutput->pstSsdInfo)
    	{
    		PRINT_ERR("Malloc ssd result buffer failed.\n");
    		free(pstOutputTensor);
            free(NCHWBuff);
    		free(result_dect);
            return -1;
    	}

		#if 1
    	SSD_DECTION_OUTPUT_S *ssdDectTmp = result_dect;
    	IPC_SSD_OUTPUT_INFO_S * ssdOutputTmp = pstSsdOutput->pstSsdInfo;
    	for(AR_U32 id=0; id < s32NumDect; id++)
    	{
    		 sprintf(ssdOutputTmp->scores, "%s:%f", std_ssd_labels[ssdDectTmp->classId], ssdDectTmp->confidence);
    		 ssdOutputTmp->x = ssdDectTmp->x;
    		 ssdOutputTmp->y = ssdDectTmp->y;
    		 ssdOutputTmp->w = ssdDectTmp->w;
    		 ssdOutputTmp->h = ssdDectTmp->h;
    
    		 ssdDectTmp++;
    		 ssdOutputTmp++;
    	}
		#endif
	}
#endif

	SSD_Undetection(&dection);

    if(result_dect)
    {
        free(result_dect);
    }
   
    if(pstOutputTensor)
    {
        free(pstOutputTensor);
    }
    if(NCHWBuff)
    {
        free(NCHWBuff);
    }
	
	return 0;
}

void IPC_SVP_ReleaseSsdPostProcessResult(IPC_SSD_OUTPUT_S * pstSsdOutput)
{
    if(pstSsdOutput->pstSsdInfo)
    {
        free(pstSsdOutput->pstSsdInfo);
		pstSsdOutput->pstSsdInfo = NULL;
    }
}


