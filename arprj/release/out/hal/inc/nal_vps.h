#ifndef __NAL_VPS_H
#define __NAL_VPS_H
#include <stddef.h>
#include <stdint.h>
#include "types.h"
#include "cio.h"
#include "nal_common.h"

enum {
    // 7.4.3.1: vps_max_layers_minus1 is in [0, 62].
    HEVC_MAX_LAYERS     = 63,
    // 7.4.3.1: vps_max_sub_layers_minus1 is in [0, 6].
    HEVC_MAX_SUB_LAYERS = 7,
    // 7.4.3.1: vps_num_layer_sets_minus1 is in [0, 1023].
    HEVC_MAX_LAYER_SETS = 1024,

    // 7.4.2.1: vps_video_parameter_set_id is u(4).
    HEVC_MAX_VPS_COUNT = 16,
    // 7.4.3.2.1: sps_seq_parameter_set_id is in [0, 15].
    HEVC_MAX_SPS_COUNT = 16,
    // 7.4.3.3.1: pps_pic_parameter_set_id is in [0, 63].
    HEVC_MAX_PPS_COUNT = 64,

    // A.4.2: MaxDpbSize is bounded above by 16.
    HEVC_MAX_DPB_SIZE = 16,
    // 7.4.3.1: vps_max_dec_pic_buffering_minus1[i] is in [0, MaxDpbSize - 1].
    HEVC_MAX_REFS     = HEVC_MAX_DPB_SIZE,

    // 7.4.3.2.1: num_short_term_ref_pic_sets is in [0, 64].
    HEVC_MAX_SHORT_TERM_REF_PIC_SETS = 64,
    // 7.4.3.2.1: num_long_term_ref_pics_sps is in [0, 32].
    HEVC_MAX_LONG_TERM_REF_PICS      = 32,

    // A.3: all profiles require that CtbLog2SizeY is in [4, 6].
    HEVC_MIN_LOG2_CTB_SIZE = 4,
    HEVC_MAX_LOG2_CTB_SIZE = 6,

    // E.3.2: cpb_cnt_minus1[i] is in [0, 31].
    HEVC_MAX_CPB_CNT = 32,

    // A.4.1: in table A.6 the highest level allows a MaxLumaPs of 35 651 584.
    HEVC_MAX_LUMA_PS = 35651584,
    // A.4.1: pic_width_in_luma_samples and pic_height_in_luma_samples are
    // constrained to be not greater than sqrt(MaxLumaPs * 8).  Hence height/
    // width are bounded above by sqrt(8 * 35651584) = 16888.2 samples.
    HEVC_MAX_WIDTH  = 16888,
    HEVC_MAX_HEIGHT = 16888,

    // A.4.1: table A.6 allows at most 22 tile rows for any level.
    HEVC_MAX_TILE_ROWS    = 22,
    // A.4.1: table A.6 allows at most 20 tile columns for any level.
    HEVC_MAX_TILE_COLUMNS = 20,

    // A.4.2: table A.6 allows at most 600 slice segments for any level.
    HEVC_MAX_SLICE_SEGMENTS = 600,

    // 7.4.7.1: in the worst case (tiles_enabled_flag and
    // entropy_coding_sync_enabled_flag are both set), entry points can be
    // placed at the beginning of every Ctb row in every tile, giving an
    // upper bound of (num_tile_columns_minus1 + 1) * PicHeightInCtbsY - 1.
    // Only a stream with very high resolution and perverse parameters could
    // get near that, though, so set a lower limit here with the maximum
    // possible value for 4K video (at most 135 16x16 Ctb rows).
    HEVC_MAX_ENTRY_POINT_OFFSETS = HEVC_MAX_TILE_COLUMNS * 135,
};

typedef struct H265RawProfileTierLevel {
    uint8_t general_profile_space;
    uint8_t general_tier_flag;
    uint8_t general_profile_idc;

    uint8_t general_profile_compatibility_flag[32];

    uint8_t general_progressive_source_flag;
    uint8_t general_interlaced_source_flag;
    uint8_t general_non_packed_constraint_flag;
    uint8_t general_frame_only_constraint_flag;

    uint8_t general_max_12bit_constraint_flag;
    uint8_t general_max_10bit_constraint_flag;
    uint8_t general_max_8bit_constraint_flag;
    uint8_t general_max_422chroma_constraint_flag;
    uint8_t general_max_420chroma_constraint_flag;
    uint8_t general_max_monochrome_constraint_flag;
    uint8_t general_intra_constraint_flag;
    uint8_t general_one_picture_only_constraint_flag;
    uint8_t general_lower_bit_rate_constraint_flag;
    uint8_t general_max_14bit_constraint_flag;

    uint8_t general_inbld_flag;

    uint8_t general_level_idc;

    uint8_t sub_layer_profile_present_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_level_present_flag[HEVC_MAX_SUB_LAYERS];

    uint8_t sub_layer_profile_space[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_tier_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_profile_idc[HEVC_MAX_SUB_LAYERS];

    uint8_t sub_layer_profile_compatibility_flag[HEVC_MAX_SUB_LAYERS][32];

    uint8_t sub_layer_progressive_source_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_interlaced_source_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_non_packed_constraint_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_frame_only_constraint_flag[HEVC_MAX_SUB_LAYERS];

    uint8_t sub_layer_max_12bit_constraint_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_max_10bit_constraint_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_max_8bit_constraint_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_max_422chroma_constraint_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_max_420chroma_constraint_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_max_monochrome_constraint_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_intra_constraint_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_one_picture_only_constraint_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_lower_bit_rate_constraint_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_max_14bit_constraint_flag[HEVC_MAX_SUB_LAYERS];

    uint8_t sub_layer_inbld_flag[HEVC_MAX_SUB_LAYERS];

    uint8_t sub_layer_level_idc[HEVC_MAX_SUB_LAYERS];
} H265RawProfileTierLevel;

typedef struct H265RawSubLayerHRDParameters {
    uint32_t bit_rate_value_minus1[HEVC_MAX_CPB_CNT];
    uint32_t cpb_size_value_minus1[HEVC_MAX_CPB_CNT];
    uint32_t cpb_size_du_value_minus1[HEVC_MAX_CPB_CNT];
    uint32_t bit_rate_du_value_minus1[HEVC_MAX_CPB_CNT];
    uint8_t cbr_flag[HEVC_MAX_CPB_CNT];
} H265RawSubLayerHRDParameters;

typedef struct H265RawHRDParameters {
    uint8_t nal_hrd_parameters_present_flag;
    uint8_t vcl_hrd_parameters_present_flag;

    uint8_t sub_pic_hrd_params_present_flag;
    uint8_t tick_divisor_minus2;
    uint8_t du_cpb_removal_delay_increment_length_minus1;
    uint8_t sub_pic_cpb_params_in_pic_timing_sei_flag;
    uint8_t dpb_output_delay_du_length_minus1;

    uint8_t bit_rate_scale;
    uint8_t cpb_size_scale;
    uint8_t cpb_size_du_scale;

    uint8_t initial_cpb_removal_delay_length_minus1;
    uint8_t au_cpb_removal_delay_length_minus1;
    uint8_t dpb_output_delay_length_minus1;

    uint8_t fixed_pic_rate_general_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t fixed_pic_rate_within_cvs_flag[HEVC_MAX_SUB_LAYERS];
    uint16_t elemental_duration_in_tc_minus1[HEVC_MAX_SUB_LAYERS];
    uint8_t low_delay_hrd_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t cpb_cnt_minus1[HEVC_MAX_SUB_LAYERS];
    H265RawSubLayerHRDParameters nal_sub_layer_hrd_parameters[HEVC_MAX_SUB_LAYERS];
    H265RawSubLayerHRDParameters vcl_sub_layer_hrd_parameters[HEVC_MAX_SUB_LAYERS];
} H265RawHRDParameters;

typedef struct _nal_vps_t {
    uint8 vps_video_parameter_set_id;
    uint8 vps_base_layer_internal_flag;
    uint8 vps_base_layer_available_flag;
    uint8 vps_max_layers_minus1;
    uint8 vps_max_sub_layers_minus1;
    uint8 vps_temporal_id_nesting_flag;
    uint16 vps_reserved_0xffff_16bits;
    profile_tier_level_data_t profile_tier_level_data;
    //H265RawProfileTierLevel profile_tier_level;
    uint8 vps_sub_layer_ordering_info_present_flag;
    uint8 vps_max_dec_pic_buffering_minus1[HEVC_MAX_SUB_LAYERS];
    uint8 vps_max_num_reorder_pics[HEVC_MAX_SUB_LAYERS];
    uint32 vps_max_latency_increase_plus1[HEVC_MAX_SUB_LAYERS];
    uint8 vps_max_layer_id;
    uint16 vps_num_layer_sets_minus1;
    uint8 layer_id_included_flag[HEVC_MAX_LAYER_SETS][HEVC_MAX_LAYERS];
    uint8 vps_timing_info_present_flag;
    uint32 vps_num_units_in_tick;
    uint32 vps_time_scale;
    uint8 vps_poc_proportional_to_timing_flag;
    uint32 vps_num_ticks_poc_diff_one_minus1;
    uint16 vps_num_hrd_parameters;
    uint16 hrd_layer_set_idx[HEVC_MAX_LAYER_SETS];
    uint8 cprms_present_flag[HEVC_MAX_LAYER_SETS];
    //H265RawHRDParameters hrd_parameters[HEVC_MAX_LAYER_SETS];
    uint8 vps_extension_flag;
    //H265RawExtensionData extension_data;
} nal_vps_t;

void nal_vps_init();
//void print_nal_vps_parse(nal_buffer_t *pInBuffer);
void parse_vps_from_rbsp(nal_buffer_t *pInBuffer, nal_vps_t *nal_vps);
void print_vps(nal_vps_t *nal_vps);
void encode_vps_to_rbsp(nal_buffer_t *pnal_out, nal_vps_t *nal_vps);
#endif
