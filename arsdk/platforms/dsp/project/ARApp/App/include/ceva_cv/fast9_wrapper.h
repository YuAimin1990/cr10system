/*
 * fast9_wrapper.h
 *
 *  Created on: 2021Äê9ÔÂ29ÈÕ
 *      Author: syli
 */

#ifndef INCLUDE_FAST9_WRAPPER_H_
#define INCLUDE_FAST9_WRAPPER_H_

#ifdef __cplusplus

extern "C"{
#endif

int cvFast9_process(unsigned char* in_image,unsigned int width,unsigned int height,unsigned short* px,unsigned short* py,unsigned short* ps,unsigned int threshold, unsigned char nms ,
				unsigned char output_list_is_external , unsigned char use_grid,unsigned int* num_points);


#ifdef __cplusplus
}

#endif



#endif /* INCLUDE_FAST9_WRAPPER_H_ */
