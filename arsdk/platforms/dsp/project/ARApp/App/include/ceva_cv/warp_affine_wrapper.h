/*
 * warp_affine_wrapper.h
 *
 *  Created on: 2021Äê10ÔÂ14ÈÕ
 *      Author: syli
 */

#ifndef INCLUDE_WARP_AFFINE_WRAPPER_H_
#define INCLUDE_WARP_AFFINE_WRAPPER_H_


#ifdef __cplusplus
extern "C"{
#endif


/**
   * \ src_buf[]   			Input frame buffer
   * \ dst_buf[]   			Output frame buffer
   * \ bit_depth			8,12,16
   * \ element_size			1,2
   * \ src_width			Input image width
   * \ src_height			Input image height
   * \ dst_width			Out image width
   * \ dst_height			Out image height
   * \ roi_top_left_x   	X coordinate of the ROI top-left corner
   * \ roi_top_left_y   	Y coordinate of the ROI top-left corner
   * \ roi_width        	ROI width
   * \ roi_height       	ROI height
   * \ transformation       Transformation, generated using either get_transformation_from_floating_point() or get_transformation_from_fixed_point()
   * \ fill_value           Fill value for pixels that are mapped from areas outside the input image
   * \ m					Matrix;m00,m01,m02,m10,m11,m12
   */


int cvWarpAffine(unsigned char * src_buf[],unsigned char * dst_buf[],unsigned int bit_depth,unsigned int element_size, \
			unsigned int src_width,unsigned int src_height,unsigned int dst_width,unsigned int dst_height,\
			unsigned int roi_top_left_x,unsigned int roi_top_left_y,unsigned int roi_width,unsigned int roi_height, unsigned int fill_val,float* m);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_WARP_AFFINE_WARP_AFFINE_WRAPPER_H_ */
