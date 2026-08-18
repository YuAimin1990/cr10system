/*
 * warp_affine_wrapper.h
 *
 *  Created on: 2021Äê10ÔÂ14ÈÕ
 *      Author: syli
 */

#ifndef INCLUDE_CANNY_WRAPPER_H_
#define INCLUDE_CANNY_WRAPPER_H_


#ifdef __cplusplus
extern "C"{
#endif


/**
   * \param[in] src_buf[]   Input frame buffer
   * \param[in] dst_buf[]   Output frame buffer
   * \param[in] bit_depth   Bit count per pixel
   * \param[in] src_width   Input frame width
   * \param[in] src_height  Input frame height
   * \param[in] src_stride  Input frame stride
   * \param[in] high_threshold
   * \param[in] low_threshold
   */

int cvCanny(unsigned char * src_buf,unsigned char * dst_buf,unsigned int bit_depth,
			unsigned short src_width,unsigned short src_height,unsigned short src_stride,
			unsigned short high_threshold,unsigned short low_threshold);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_CANNY_WRAPPER_H_ */
