
/*---------------header of imx307 tuning-------------------------*/

/* version of the tuning header uint32_t header_version */
0x20220926,
/* tuning_ctl_t tuning_ctl */
{
  //float aec_trigger_tolerence;
  3,
  //float aec_trigger_tolerence_gain;
  0.3,
  //float awb_trigger_tolerence;
  50,
},
/* isp_tuning_t isp_tuning */
{
  /* isp version */
  0x00000001,
    /* isp_sub_module_raw_crop_tuning_t isp_sub_module_raw_crop_tuning */
    {
       /* enable */
       1,
       /* interpolation enable */
       0,
    },
    /* isp_sub_module_blc_tuning_t isp_sub_module_blc_tuning */
    {
       /* enable */
       0,
       /* interpolation_enable */
       1,
       /* count_ae */
       5,
       /* aec_trigger_mode_t trigger_mode : 0 gain trigger ,1 lux trigger */
       1,
       /* aec_trigger_t aec_trigger[MIDDLE_TRIGGER_COUNT] MIDDLE_TRIGGER_COUNT==5 */
       {
           /* aec trigger 0*/
           {
               0,
               370,
           },
           /* aec trigger 1*/
           {
               416,
               419,
           },
           /* aec trigger 2*/
           {
               470,
               473,
           },
           /* aec trigger 3*/
           {
               550,
               555,
           },
           /* aec trigger 4*/
           {
               580,
               585,
           },
       },
       /* isp_sub_module_blc_para para[MIDDLE_TRIGGER_COUNT] MIDDLE_TRIGGER_COUNT==5 */
       {
           /* para 0*/
           {
               961,//R
               961,//B
               961,//GR
               961,//GB
               272,//gian R
               272,//gain B
               272,//gain GR
               272,//gain GB
           },
           /* para 1*/
           {
               946,//R
               944,//B
               946,//GR
               944,//GB
               272,//gian R
               272,//gain B
               272,//gain GR
               272,//gain GB
           },
           /* para 2*/
           {
               1048,//R
               1061,//B
               1047,//GR
               1063,//GB
               274,//gian R
               274,//gain B
               274,//gain GR
               274,//gain GB
           },
           /* para 3*/
           {
               1586,//R
               1625,//B
               1591,//GR
               1624,//GB
               284,//gian R
               285,//gain B
               284,//gain GR
               285,//gain GB
           },
           /* para 4*/
           {
               2628,//R
               2601,//B
               2633,//GR
               2591,//GB
               305,//gian R
               305,//gain B
               306,//gain GR
               305,//gain GB
           },
       },
    },
    /* isp_sub_module_hdr_mix_tuning_t isp_sub_module_hdr_mix_tuning */
    {
       /* enable */
       1,
       /* interpolation_enable */
       1,
       /* count_ae */
       6,
       /* tigger_mode : 0 gain ,1 lux */
       1,
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             90,
          },
          /* aec trigger 1 */
          {
             100,
             150,
          },
          /* aec trigger 2 */
          {
             160,
             300,
          },
          /* aec trigger 3 */
          {
             310,
             400,
          },
          /* aec trigger 4 */
          {
             410,
             440,
          },
          /* aec trigger 5 */
          {
             450,
             500,
          },
       },
       /* isp_sub_module_hdr_para para[MAX_TRIGGER_COUNT] MAX_TRIGGER_COUNT==16 */
       {
           /* para 0*/
           {
              256,//blc_short_exp
              256,//blc_mid_exp
              256,//blc_long_exp
              0,//hdr_motion_en_sm
              25,//hdr_alpha_s_th_sm
              867,//hdr_exp_value_th1_sm
              682,//hdr_th2_th1_reverse_sm
              64,//hdr_noise_sp_sm
              76,//hdr_motion_mp_sm
              1,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              200,//hdr_exp_value_th1_sl
              333,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              0,//hdr_motion_mp_sl
              32,//hdr_md_th4_sl
              27,//hdr_md_th9_sl
              19,//hdr_md_th16_sl
              15,//hdr_md_th25_sl
              /* noise_profile_l[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
              },
              /* noise_profile_m[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
              },
              /* noise_profile_s[129] */
              {
                  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
                  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
              },
              0,//hdr_policy
              1,//enable_hdr_blc
              60,//mv_fix_th
              16,//mv_fix_value
              1,//mv_fix_en
              0,//use_long_exp_fix
              1,//use_mv_fix_value_fix
              1,//enable_sexp_denoise
              /* gaus_coef[4] */
              {
                 0,10,10,216,
              },
           },
           /* para 1*/
           {
              256,//blc_short_exp
              240,//blc_mid_exp
              256,//blc_long_exp
              0,//hdr_motion_en_sm
              25,//hdr_alpha_s_th_sm
              867,//hdr_exp_value_th1_sm
              682,//hdr_th2_th1_reverse_sm
              64,//hdr_noise_sp_sm
              76,//hdr_motion_mp_sm
              1,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              133,//hdr_exp_value_th1_sl
              444,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              0,//hdr_motion_mp_sl
              32,//hdr_md_th4_sl
              27,//hdr_md_th9_sl
              19,//hdr_md_th16_sl
              15,//hdr_md_th25_sl
              /* noise_profile_l[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
              },
              /* noise_profile_m[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
              },
              /* noise_profile_s[129] */
              {
                  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
                  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
              },
              0,//hdr_policy
              1,//enable_hdr_blc
              60,//mv_fix_th
              16,//mv_fix_value
              1,//mv_fix_en
              0,//use_long_exp_fix
              1,//use_mv_fix_value_fix
              1,//enable_sexp_denoise
              /* gaus_coef[4] */
              {
                 0,10,10,216,
              },
           },
           /* para 2*/
           {
              255,//blc_short_exp
              240,//blc_mid_exp
              255,//blc_long_exp
              0,//hdr_motion_en_sm
              25,//hdr_alpha_s_th_sm
              867,//hdr_exp_value_th1_sm
              682,//hdr_th2_th1_reverse_sm
              64,//hdr_noise_sp_sm
              76,//hdr_motion_mp_sm
              1,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              88,//hdr_exp_value_th1_sl
              388,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              4,//hdr_motion_mp_sl
              32,//hdr_md_th4_sl
              27,//hdr_md_th9_sl
              19,//hdr_md_th16_sl
              15,//hdr_md_th25_sl
              /* noise_profile_l[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
              },
              /* noise_profile_m[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
              },
              /* noise_profile_s[129] */
              {
                  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
                  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
              },
              0,//hdr_policy
              1,//enable_hdr_blc
              60,//mv_fix_th
              16,//mv_fix_value
              1,//mv_fix_en
              0,//use_long_exp_fix
              1,//use_mv_fix_value_fix
              1,//enable_sexp_denoise
              /* gaus_coef[4] */
              {
                 0,10,10,216,
              },
           },
           /* para 3*/
           {
              255,//blc_short_exp
              240,//blc_mid_exp
              255,//blc_long_exp
              0,//hdr_motion_en_sm
              25,//hdr_alpha_s_th_sm
              867,//hdr_exp_value_th1_sm
              682,//hdr_th2_th1_reverse_sm
              64,//hdr_noise_sp_sm
              76,//hdr_motion_mp_sm
              1,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              24,//hdr_exp_value_th1_sl
              256,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              0,//hdr_motion_mp_sl
              32,//hdr_md_th4_sl
              27,//hdr_md_th9_sl
              18,//hdr_md_th16_sl
              15,//hdr_md_th25_sl
              /* noise_profile_l[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
              },
              /* noise_profile_m[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
              },
              /* noise_profile_s[129] */
              {
                  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
                  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
              },
              0,//hdr_policy
              1,//enable_hdr_blc
              60,//mv_fix_th
              16,//mv_fix_value
              1,//mv_fix_en
              0,//use_long_exp_fix
              1,//use_mv_fix_value_fix
              1,//enable_sexp_denoise
              /* gaus_coef[4] */
              {
                 0,10,10,216,
              },
           },
           /* para 4*/
           {
              256,//blc_short_exp
              240,//blc_mid_exp
              256,//blc_long_exp
              0,//hdr_motion_en_sm
              25,//hdr_alpha_s_th_sm
              867,//hdr_exp_value_th1_sm
              682,//hdr_th2_th1_reverse_sm
              64,//hdr_noise_sp_sm
              76,//hdr_motion_mp_sm
              1,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              16,//hdr_exp_value_th1_sl
              128,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              0,//hdr_motion_mp_sl
              32,//hdr_md_th4_sl
              27,//hdr_md_th9_sl
              19,//hdr_md_th16_sl
              15,//hdr_md_th25_sl
              /* noise_profile_l[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
              },
              /* noise_profile_m[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
              },
              /* noise_profile_s[129] */
              {
                  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
                  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
              },
              0,//hdr_policy
              1,//enable_hdr_blc
              60,//mv_fix_th
              16,//mv_fix_value
              1,//mv_fix_en
              0,//use_long_exp_fix
              1,//use_mv_fix_value_fix
              1,//enable_sexp_denoise
              /* gaus_coef[4] */
              {
                 0,10,10,216,
              },
           },
           /* para 5*/
           {
              266,//blc_short_exp
              262,//blc_mid_exp
              266,//blc_long_exp
              0,//hdr_motion_en_sm
              25,//hdr_alpha_s_th_sm
              867,//hdr_exp_value_th1_sm
              682,//hdr_th2_th1_reverse_sm
              64,//hdr_noise_sp_sm
              76,//hdr_motion_mp_sm
              1,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              16,//hdr_exp_value_th1_sl
              512,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              0,//hdr_motion_mp_sl
              32,//hdr_md_th4_sl
              27,//hdr_md_th9_sl
              19,//hdr_md_th16_sl
              15,//hdr_md_th25_sl
              /* noise_profile_l[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
              },
              /* noise_profile_m[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
              },
              /* noise_profile_s[129] */
              {
                  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
                  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
              },
              0,//hdr_policy
              1,//enable_hdr_blc
              60,//mv_fix_th
              16,//mv_fix_value
              1,//mv_fix_en
              0,//use_long_exp_fix
              1,//use_mv_fix_value_fix
              1,//enable_sexp_denoise
              /* gaus_coef[4] */
              {
                 0,10,10,216,
              },
           },
       },
    },
    /* isp_sub_module_compander_tuning_t isp_sub_module_compander_tuning */
    {
       /* enable */
       1,
    },
    /* isp_sub_module_dpc_tuning_t isp_sub_module_dpc_tuning */
    {
       /* enable */
       1,
       /* interpolation_enable */
       0,
       /* count_ae */
       5,
       /* 0 for dynamic dpc, 1 for static dpc */
       0,
       /* tigger_mode : 0 gain ,1 lux */
       1,
       /* region enable */
       {
          0,
          1,
          1,
          1,
          1,
       },
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             292,
          },
          /* aec trigger 1 */
          {
             301,
             325,
          },
          /* aec trigger 2 */
          {
             348,
             365,
          },
          /* aec trigger 3 */
          {
             395,
             433,
          },
          /* aec trigger 4 */
          {
             442,
             1000,
          },
       },
       /* aec_trigger_t aec_trigger[MIDDLE_TRIGGER_COUNT] MIDDLE_TRIGGER_COUNT==5 */
       {
          /* parameter 0 */
          {
             0x0000004c,0x00000000,0x000003e8,0x00000578,0x00000640,0x000003e8,0x00000578,0x00000640,0x00000c00,0x00000028,
             0x0000003c,0x00000bb8,0x000000c8,0x00000190,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
             0x00000000,0x00000020,0x00000060,0x000000a0,0x000000e0,
          },
          /* parameter 1 */
          {
             0x00000040,0x00000000,0x00000032,0x00000046,0x00000050,0x000003e8,0x000000c8,0x00000320,0x00000064,0x0000008c,
             0x000000c8,0x000003e8,0x000000c8,0x00000320,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
             0x00000000,0x00000020,0x00000060,0x000000a0,0x000000e0,
          },
          /* parameter 2 */
          {
             0x00000026,0x00000000,0x00000032,0x00000046,0x00000050,0x000003e8,0x000000c8,0x00000320,0x00000064,0x00000096,
             0x000000c8,0x000003e8,0x000000c8,0x00000320,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
             0x00000000,0x00000020,0x00000060,0x000000a0,0x000000e0,
          },
          /* parameter 3 */
          {
             0x00000040,0x00000002,0x00000032,0x00000046,0x00000050,0x000003e8,0x000000c8,0x00000320,0x00000028,0x00000032,
             0x00000050,0x000003e8,0x000000c8,0x00000320,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
             0x00000000,0x00000020,0x00000060,0x000000a0,0x000000e0,
          },
          /* parameter 4 */
          {
             0x00000020,0x00000000,0x00000064,0x0000008c,0x000000a0,0x000003e8,0x000000c8,0x00000320,0x0000012c,0x000000c8,
             0x00000190,0x000003e8,0x000000c8,0x00000320,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
             0x00000000,0x00000020,0x00000060,0x000000a0,0x000000e0,
          },
       },
    },
    /* isp_sub_module_cac_tuning_t isp_sub_module_cac_tuning */
    {
       0,//enable
    },
    /* isp_sub_module_ata_tuning_t isp_sub_module_ata_tuning */
    {
       0,//enable
    },
    /* isp_sub_module_rnr_tuning_t isp_sub_module_rnr_tuning */
    {
       /* enable */
       1,
       /* interpolation_enable */
       1,
       /* grey mode */
       0,
       /* count_ae */
       11,
       /* tigger_mode : 0 gain ,1 lux */
       1,
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             235,
          },
          /* aec trigger 1 */
          {
             245,
             255,
          },
          /* aec trigger 2 */
          {
             265,
             290,
          },
          /* aec trigger 3 */
          {
             314,
             325,
          },
          /* aec trigger 4 */
          {
             347,
             348,
          },
          /* aec trigger 5 */
          {
             371,
             372,
          },
          /* aec trigger 6 */
          {
             384,
             395,
          },
          /* aec trigger 7 */
          {
             417,
             418,
          },
          /* aec trigger 8 */
          {
             441,
             442,
          },
          /* aec trigger 9 */
          {
             465,
             466,
          },
          /* aec trigger 10 */
          {
             500,
             501,
          },
       },
       /* isp_sub_module_rnr_para para[MIDDLE_TRIGGER_COUNT] MIDDLE_TRIGGER_COUNT==5 */
       {
           /* para 0*/
           {
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {38,38,38},
                       /* channel 2 */
                       {38,38,38},
                       /* channel 3 */
                       {38,38,38},
                       /* channel 4 */
                       {38,38,38},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {15,15,15},
                       /* channel 2 */
                       {15,15,15},
                       /* channel 3 */
                       {15,15,15},
                       /* channel 4 */
                       {15,15,15},
                   },
               },
               /* profile_t profile[RNR_CHANNEL_COUMT] */
               {
                   /* channel 1 */
                   {
                       /* noise profile */
                       {2500,1500,600},
                       /* noise profile gain */
                       {16,10,10,10},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {2500,1500,600},
                       /* noise profile gain */
                       {16,10,10,10},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {2500,1500,600},
                       /* noise profile gain */
                       {16,10,10,10},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {2500,1500,600},
                       /* noise profile gain */
                       {16,10,10,10},
                   },
               },
               /* edge_t edge */
               {
                   /* channel 1 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {16,14,12,10,8,6,4,2,0},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {16,14,12,10,8,6,4,2,0},
                   },
               },
           },
           /* para 1*/
           {
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {33,33,33},
                       /* channel 2 */
                       {33,33,33},
                       /* channel 3 */
                       {33,33,33},
                       /* channel 4 */
                       {33,33,33},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {20,20,20},
                       /* channel 2 */
                       {20,20,20},
                       /* channel 3 */
                       {20,20,20},
                       /* channel 4 */
                       {20,20,20},
                   },
               },
               /* profile_t profile[RNR_CHANNEL_COUMT] */
               {
                   /* channel 1 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,10},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,10},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,10},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,10},
                   },
               },
               /* edge_t edge */
               {
                   /* channel 1 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {16,14,12,10,8,6,4,2,1},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {16,14,12,10,8,6,4,2,1},
                   },
               },
           },
           /* para 2*/
           {
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {33,33,33},
                       /* channel 2 */
                       {33,33,33},
                       /* channel 3 */
                       {33,33,33},
                       /* channel 4 */
                       {33,33,33},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {44,44,44},
                       /* channel 2 */
                       {44,44,44},
                       /* channel 3 */
                       {44,44,44},
                       /* channel 4 */
                       {44,44,44},
                   },
               },
               /* profile_t profile[RNR_CHANNEL_COUMT] */
               {
                   /* channel 1 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
               },
               /* edge_t edge */
               {
                   /* channel 1 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {16,14,12,10,8,6,4,2,1},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {16,14,12,10,8,6,4,2,1},
                   },
               },
           },
           /* para 3*/
           {
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {33,33,33},
                       /* channel 2 */
                       {33,33,33},
                       /* channel 3 */
                       {33,33,33},
                       /* channel 4 */
                       {33,33,33},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {55,55,55},
                       /* channel 2 */
                       {55,55,55},
                       /* channel 3 */
                       {55,55,55},
                       /* channel 4 */
                       {55,55,55},
                   },
               },
               /* profile_t profile[RNR_CHANNEL_COUMT] */
               {
                   /* channel 1 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
               },
               /* edge_t edge */
               {
                   /* channel 1 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
               },
           },
           /* para 4*/
           {
               2,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {43,43,43},
                       /* channel 2 */
                       {43,43,43},
                       /* channel 3 */
                       {43,43,43},
                       /* channel 4 */
                       {43,43,43},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {35,35,35},
                       /* channel 2 */
                       {35,35,35},
                       /* channel 3 */
                       {35,35,35},
                       /* channel 4 */
                       {35,35,35},
                   },
               },
               /* profile_t profile[RNR_CHANNEL_COUMT] */
               {
                   /* channel 1 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
               },
               /* edge_t edge */
               {
                   /* channel 1 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
               },
           },
           /* para 5*/
           {
               2,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {60,60,60},
                       /* channel 2 */
                       {60,60,60},
                       /* channel 3 */
                       {60,60,60},
                       /* channel 4 */
                       {60,60,60},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {40,40,40},
                       /* channel 2 */
                       {40,40,40},
                       /* channel 3 */
                       {40,40,40},
                       /* channel 4 */
                       {40,40,40},
                   },
               },
               /* profile_t profile[RNR_CHANNEL_COUMT] */
               {
                   /* channel 1 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
               },
               /* edge_t edge */
               {
                   /* channel 1 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
               },
           },
           /* para 6*/
           {
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {88,88,88},
                       /* channel 2 */
                       {88,88,88},
                       /* channel 3 */
                       {88,88,88},
                       /* channel 4 */
                       {88,88,88},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {50,50,50},
                       /* channel 2 */
                       {50,50,50},
                       /* channel 3 */
                       {50,50,50},
                       /* channel 4 */
                       {50,50,50},
                   },
               },
               /* profile_t profile[RNR_CHANNEL_COUMT] */
               {
                   /* channel 1 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
               },
               /* edge_t edge */
               {
                   /* channel 1 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
               },
           },
           /* para 7*/
           {
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {105,105,105},
                       /* channel 2 */
                       {105,105,105},
                       /* channel 3 */
                       {105,105,105},
                       /* channel 4 */
                       {105,105,105},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {60,60,60},
                       /* channel 2 */
                       {60,60,60},
                       /* channel 3 */
                       {60,60,60},
                       /* channel 4 */
                       {60,60,60},
                   },
               },
               /* profile_t profile[RNR_CHANNEL_COUMT] */
               {
                   /* channel 1 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
               },
               /* edge_t edge */
               {
                   /* channel 1 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
               },
           },
           /* para 8*/
           {
               1,//denoise_layer
               1,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {60,60,60},
                       /* channel 2 */
                       {60,60,60},
                       /* channel 3 */
                       {60,60,60},
                       /* channel 4 */
                       {60,60,60},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {80,80,80},
                       /* channel 2 */
                       {80,80,80},
                       /* channel 3 */
                       {80,80,80},
                       /* channel 4 */
                       {80,80,80},
                   },
               },
               /* profile_t profile[RNR_CHANNEL_COUMT] */
               {
                   /* channel 1 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
               },
               /* edge_t edge */
               {
                   /* channel 1 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
               },
           },
           /* para 9*/
           {
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {165,165,165},
                       /* channel 2 */
                       {165,165,165},
                       /* channel 3 */
                       {165,165,165},
                       /* channel 4 */
                       {165,165,165},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {90,90,90},
                       /* channel 2 */
                       {90,90,90},
                       /* channel 3 */
                       {90,90,90},
                       /* channel 4 */
                       {90,90,90},
                   },
               },
               /* profile_t profile[RNR_CHANNEL_COUMT] */
               {
                   /* channel 1 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
               },
               /* edge_t edge */
               {
                   /* channel 1 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
               },
           },
           /* para 10*/
           {
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {90,90,90},
                       /* channel 2 */
                       {90,90,90},
                       /* channel 3 */
                       {90,90,90},
                       /* channel 4 */
                       {90,90,90},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {120,120,120},
                       /* channel 2 */
                       {120,120,120},
                       /* channel 3 */
                       {120,120,120},
                       /* channel 4 */
                       {120,120,120},
                   },
               },
               /* profile_t profile[RNR_CHANNEL_COUMT] */
               {
                   /* channel 1 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
               },
               /* edge_t edge */
               {
                   /* channel 1 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,8,8,10,10,12,14,14,16},
                   },
               },
           },
       },
    },
    /* isp_sub_module_decompander_tuning_t isp_sub_module_decompander_tuning */
    {
       /* enable */
       1,
    },
    /* isp_sub_module_lsc_tuning_t isp_sub_module_lsc_tuning */
    {
       0,//enable
       1,//interpolation_enable
       2,//count_ae
       4,//count_awb
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             300,
          },
          /* aec trigger 1 */
          {
             390,
             500,
          },
       },
       /* awb trigger */
       {
          /* awb trigger 0 */
          {
             2700,
             2900,
          },
          /* awb trigger 1 */
          {
             3700,
             4300,
          },
          /* awb trigger 2 */
          {
             4700,
             5300,
          },
          /* awb trigger 3 */
          {
             6200,
             6800,
          },
       },
       /* isp_sub_module_lsc_pra */
       {
          /* aec trigger 0 */
          {
             /* awb para 0 */
             {
                0.35,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.1992, 2.3433, 1.9121, 1.5298, 1.4131, 1.4233, 1.5537, 1.9023, 2.4263, 4.1050, 
                   3.1421, 2.0850, 1.6104, 1.3647, 1.2305, 1.2461, 1.3516, 1.6318, 2.1030, 3.1899, 
                   2.8296, 1.8955, 1.4897, 1.2407, 1.1377, 1.1323, 1.2539, 1.4883, 1.9395, 2.8428, 
                   2.5220, 1.7749, 1.3657, 1.1577, 1.0308, 1.0361, 1.1577, 1.3940, 1.7847, 2.6113, 
                   2.4526, 1.6831, 1.3301, 1.0957, 0.9961, 1.0024, 1.1006, 1.3301, 1.7251, 2.4331, 
                   2.4526, 1.6948, 1.3179, 1.1001, 0.9995, 0.9907, 1.1050, 1.3281, 1.7026, 2.4526, 
                   2.4761, 1.6992, 1.3330, 1.1157, 0.9922, 0.9971, 1.1094, 1.3228, 1.7305, 2.4478, 
                   2.6470, 1.7856, 1.4067, 1.1729, 1.0654, 1.0669, 1.2046, 1.4087, 1.8306, 2.6416, 
                   2.8472, 1.9229, 1.5029, 1.2622, 1.1470, 1.1445, 1.2500, 1.4946, 1.9546, 2.8765, 
                   3.3706, 2.1104, 1.6577, 1.3687, 1.2622, 1.2603, 1.4233, 1.6392, 2.1929, 3.2603, 
                   3.7437, 2.2642, 1.7798, 1.4980, 1.3921, 1.3823, 1.4985, 1.8442, 2.2637, 3.9390, 
                   3.0400, 1.9692, 1.5884, 1.3345, 1.2163, 1.2207, 1.3511, 1.5649, 2.0352, 2.9741, 
                   2.6152, 1.8320, 1.4404, 1.2212, 1.1313, 1.1309, 1.2285, 1.4644, 1.8540, 2.7119, 
                   2.3862, 1.7114, 1.3472, 1.1450, 1.0313, 1.0356, 1.1504, 1.3643, 1.7402, 2.4795, 
                   2.3403, 1.6436, 1.3008, 1.0933, 0.9976, 0.9878, 1.1060, 1.2998, 1.6680, 2.3350, 
                   2.2754, 1.6333, 1.2959, 1.0869, 0.9897, 0.9985, 1.0933, 1.3110, 1.6694, 2.3560, 
                   2.3408, 1.6431, 1.3013, 1.1055, 0.9888, 0.9937, 1.1084, 1.3154, 1.6753, 2.3394, 
                   2.4731, 1.7427, 1.3721, 1.1719, 1.0522, 1.0625, 1.1563, 1.3931, 1.7476, 2.5264, 
                   2.6890, 1.8286, 1.4668, 1.2324, 1.1372, 1.1460, 1.2505, 1.4727, 1.8833, 2.7197, 
                   3.1987, 2.0332, 1.6094, 1.3706, 1.2510, 1.2393, 1.3496, 1.6387, 2.0278, 3.1948, 
                   3.7358, 2.1665, 1.7969, 1.4893, 1.3394, 1.3931, 1.4990, 1.8223, 2.3691, 3.6719, 
                   2.8809, 1.9331, 1.5830, 1.3003, 1.2417, 1.2202, 1.3589, 1.5513, 2.0581, 2.8774, 
                   2.5972, 1.7915, 1.4209, 1.2251, 1.1172, 1.1396, 1.2104, 1.4858, 1.8511, 2.7061, 
                   2.3564, 1.6665, 1.3550, 1.1245, 1.0332, 1.0527, 1.1563, 1.3613, 1.7632, 2.4351, 
                   2.2788, 1.6338, 1.2773, 1.0908, 1.0044, 1.0063, 1.1016, 1.3271, 1.6738, 2.3555, 
                   2.2368, 1.6030, 1.3018, 1.0752, 1.0098, 0.9995, 1.1152, 1.3193, 1.6763, 2.3662, 
                   2.3076, 1.6309, 1.2998, 1.1104, 0.9790, 1.0112, 1.0996, 1.3164, 1.6553, 2.3730, 
                   2.4287, 1.7217, 1.3726, 1.1621, 1.0713, 1.0654, 1.1792, 1.3955, 1.7524, 2.5547, 
                   2.6240, 1.8091, 1.4648, 1.2363, 1.1387, 1.1479, 1.2310, 1.4722, 1.8638, 2.6611, 
                   3.0371, 2.0659, 1.5962, 1.3877, 1.2427, 1.2788, 1.3477, 1.6167, 2.0439, 3.1831, 
                },
             },
             /* awb para 1 */
             {
                0.35,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   3.8623, 2.3374, 1.7646, 1.5356, 1.3662, 1.4063, 1.5190, 1.8076, 2.3306, 3.7979, 
                   3.0981, 2.0151, 1.5991, 1.3687, 1.2480, 1.2549, 1.3589, 1.6108, 2.0391, 3.0825, 
                   2.6689, 1.8364, 1.4492, 1.2339, 1.1270, 1.1353, 1.2495, 1.4668, 1.8716, 2.6538, 
                   2.4233, 1.7310, 1.3574, 1.1680, 1.0444, 1.0435, 1.1567, 1.3818, 1.7476, 2.4971, 
                   2.3882, 1.6445, 1.3257, 1.1055, 0.9912, 1.0063, 1.1104, 1.3345, 1.6821, 2.3657, 
                   2.3521, 1.6289, 1.3159, 1.0845, 1.0044, 0.9961, 1.0957, 1.3281, 1.6689, 2.3042, 
                   2.3701, 1.6548, 1.3340, 1.1240, 1.0059, 1.0015, 1.1235, 1.3462, 1.6772, 2.4087, 
                   2.5737, 1.7217, 1.3936, 1.1807, 1.0552, 1.0674, 1.1748, 1.3945, 1.7603, 2.4639, 
                   2.7495, 1.8345, 1.4775, 1.2456, 1.1616, 1.1318, 1.2593, 1.4976, 1.8496, 2.7930, 
                   3.3662, 2.0322, 1.6572, 1.3882, 1.2690, 1.2773, 1.4038, 1.6206, 2.0801, 3.1636, 
                   3.7710, 2.2446, 1.7661, 1.5005, 1.3745, 1.3853, 1.5098, 1.8311, 2.2500, 3.8540, 
                   2.9961, 1.9717, 1.5732, 1.3252, 1.2246, 1.2305, 1.3296, 1.5869, 2.0078, 2.9424, 
                   2.6167, 1.8188, 1.4316, 1.2231, 1.1265, 1.1240, 1.2368, 1.4575, 1.8467, 2.7012, 
                   2.4092, 1.6934, 1.3467, 1.1431, 1.0317, 1.0381, 1.1348, 1.3672, 1.7104, 2.4346, 
                   2.3193, 1.6255, 1.3037, 1.0918, 0.9946, 0.9971, 1.1123, 1.3110, 1.6777, 2.3379, 
                   2.2983, 1.6143, 1.2935, 1.0786, 1.0010, 0.9941, 1.0942, 1.3066, 1.6558, 2.3184, 
                   2.3198, 1.6377, 1.3018, 1.1084, 0.9922, 0.9976, 1.1138, 1.3218, 1.6719, 2.3623, 
                   2.4609, 1.7061, 1.3745, 1.1489, 1.0605, 1.0596, 1.1650, 1.3818, 1.7456, 2.5054, 
                   2.6543, 1.8442, 1.4448, 1.2378, 1.1450, 1.1372, 1.2466, 1.4805, 1.8574, 2.7480, 
                   3.1655, 2.0215, 1.6367, 1.3633, 1.2671, 1.2534, 1.3643, 1.6113, 2.0620, 3.0894, 
                   3.7603, 2.1733, 1.7725, 1.4741, 1.3638, 1.3599, 1.4785, 1.7744, 2.2871, 3.6040, 
                   2.8887, 1.9268, 1.5713, 1.3184, 1.2144, 1.2246, 1.3398, 1.5654, 2.0156, 2.9600, 
                   2.6162, 1.7720, 1.4175, 1.2104, 1.1138, 1.1152, 1.2095, 1.4697, 1.8125, 2.7314, 
                   2.3384, 1.6890, 1.3140, 1.1489, 1.0317, 1.0410, 1.1577, 1.3467, 1.7358, 2.4141, 
                   2.3325, 1.6250, 1.3008, 1.1025, 0.9922, 0.9985, 1.0933, 1.3374, 1.6421, 2.4233, 
                   2.2217, 1.6152, 1.2661, 1.0898, 0.9961, 0.9951, 1.1050, 1.3042, 1.6479, 2.3296, 
                   2.2974, 1.6377, 1.2993, 1.1128, 0.9941, 1.0059, 1.0986, 1.3457, 1.6484, 2.4141, 
                   2.4243, 1.6885, 1.3564, 1.1553, 1.0566, 1.0605, 1.1729, 1.3799, 1.7515, 2.5034, 
                   2.6143, 1.8311, 1.4453, 1.2402, 1.1426, 1.1382, 1.2378, 1.4761, 1.8545, 2.6890, 
                   3.0015, 2.0210, 1.5894, 1.3853, 1.2480, 1.2739, 1.3687, 1.6567, 2.0400, 3.2544, 
                },
             },
             /* awb para 2 */
             {
                0.35,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0791, 2.3071, 1.8794, 1.5049, 1.3950, 1.3872, 1.5635, 1.8208, 2.4116, 4.0425, 
                   3.1987, 2.0659, 1.6440, 1.3647, 1.2593, 1.2534, 1.3638, 1.6455, 2.1035, 3.1821, 
                   2.7422, 1.8774, 1.4590, 1.2212, 1.1260, 1.1079, 1.2446, 1.4629, 1.9092, 2.8193, 
                   2.5249, 1.7588, 1.3857, 1.1650, 1.0464, 1.0562, 1.1606, 1.4067, 1.7891, 2.6040, 
                   2.4614, 1.6870, 1.3330, 1.1035, 1.0103, 0.9897, 1.1304, 1.3320, 1.7393, 2.4697, 
                   2.3799, 1.6724, 1.2988, 1.0830, 0.9878, 0.9883, 1.0928, 1.3125, 1.6870, 2.3818, 
                   2.4946, 1.7061, 1.3535, 1.1392, 1.0078, 1.0161, 1.1318, 1.3677, 1.7480, 2.5322, 
                   2.5493, 1.7900, 1.3804, 1.1616, 1.0654, 1.0586, 1.1782, 1.3984, 1.7974, 2.5869, 
                   2.8740, 1.8994, 1.5127, 1.2642, 1.1489, 1.1479, 1.2666, 1.5166, 1.9575, 2.8789, 
                   3.3486, 2.1646, 1.6880, 1.3999, 1.2983, 1.2808, 1.4082, 1.7100, 2.1621, 3.4326, 
                   3.7246, 2.2808, 1.7480, 1.5278, 1.3652, 1.3857, 1.5112, 1.8262, 2.2856, 3.7935, 
                   3.0601, 1.9766, 1.6162, 1.3364, 1.2378, 1.2466, 1.3389, 1.5986, 2.0483, 3.0044, 
                   2.6060, 1.8096, 1.4214, 1.2163, 1.1104, 1.1191, 1.2319, 1.4580, 1.8481, 2.6851, 
                   2.4604, 1.7153, 1.3770, 1.1509, 1.0483, 1.0522, 1.1533, 1.3892, 1.7456, 2.5171, 
                   2.3057, 1.6431, 1.3013, 1.0981, 0.9966, 1.0000, 1.1177, 1.3159, 1.6968, 2.3809, 
                   2.2690, 1.6211, 1.2920, 1.0757, 0.9829, 0.9976, 1.0894, 1.3145, 1.6499, 2.3667, 
                   2.3545, 1.6606, 1.3223, 1.1152, 1.0112, 1.0122, 1.1299, 1.3418, 1.7090, 2.4395, 
                   2.4199, 1.7129, 1.3667, 1.1455, 1.0503, 1.0537, 1.1636, 1.3906, 1.7383, 2.5439, 
                   2.6929, 1.8569, 1.4580, 1.2485, 1.1431, 1.1548, 1.2661, 1.4912, 1.9077, 2.7798, 
                   3.1938, 2.0444, 1.6431, 1.3701, 1.2788, 1.2588, 1.3706, 1.6611, 2.0649, 3.2065, 
                   3.7676, 2.1494, 1.7959, 1.4575, 1.3784, 1.3604, 1.4849, 1.8022, 2.2222, 3.5879, 
                   2.8955, 1.9805, 1.5562, 1.3379, 1.2290, 1.2344, 1.3447, 1.5879, 2.0161, 3.0039, 
                   2.5591, 1.7646, 1.4189, 1.1973, 1.1206, 1.1182, 1.2227, 1.4443, 1.8276, 2.6543, 
                   2.3511, 1.7134, 1.3359, 1.1577, 1.0361, 1.0483, 1.1626, 1.3647, 1.7427, 2.4536, 
                   2.2734, 1.6343, 1.2954, 1.0913, 1.0000, 0.9990, 1.1133, 1.3164, 1.6733, 2.3857, 
                   2.2153, 1.5972, 1.2783, 1.0781, 0.9951, 0.9912, 1.0894, 1.3018, 1.6392, 2.2881, 
                   2.3003, 1.6680, 1.3169, 1.1230, 1.0068, 1.0229, 1.1274, 1.3267, 1.7017, 2.4043, 
                   2.4150, 1.6978, 1.3374, 1.1655, 1.0420, 1.0562, 1.1563, 1.3745, 1.7373, 2.4756, 
                   2.6450, 1.8271, 1.4756, 1.2261, 1.1631, 1.1484, 1.2549, 1.4868, 1.8643, 2.7544, 
                   3.0923, 2.0610, 1.5869, 1.4268, 1.2461, 1.2861, 1.3608, 1.6191, 2.0957, 3.0503, 
                },
             },
             /* awb para 3 */
             {
                0.35,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0630, 2.3340, 1.8652, 1.5303, 1.3970, 1.4116, 1.5674, 1.8408, 2.4243, 4.0415, 
                   3.1001, 2.0317, 1.6133, 1.3320, 1.2319, 1.2134, 1.3462, 1.6001, 2.0596, 3.1025, 
                   2.7930, 1.8813, 1.4736, 1.2334, 1.1274, 1.1309, 1.2607, 1.4624, 1.9346, 2.7861, 
                   2.5044, 1.7524, 1.3711, 1.1455, 1.0391, 1.0278, 1.1489, 1.3799, 1.7539, 2.5664, 
                   2.4048, 1.6772, 1.3159, 1.0981, 0.9966, 0.9966, 1.1167, 1.3203, 1.7124, 2.4189, 
                   2.3779, 1.6885, 1.3145, 1.0908, 1.0049, 1.0068, 1.1006, 1.3442, 1.6987, 2.4556, 
                   2.4097, 1.6768, 1.3262, 1.1104, 1.0000, 0.9922, 1.1074, 1.3384, 1.6963, 2.4819, 
                   2.5972, 1.7876, 1.4038, 1.1777, 1.0659, 1.0864, 1.1782, 1.4307, 1.7979, 2.6611, 
                   2.7847, 1.9072, 1.4810, 1.2598, 1.1553, 1.1396, 1.2681, 1.5029, 1.9277, 2.9194, 
                   3.2676, 2.0850, 1.6733, 1.3750, 1.2549, 1.3003, 1.3608, 1.6890, 2.1235, 3.2163, 
                   3.7461, 2.2451, 1.7700, 1.4917, 1.3638, 1.3794, 1.4966, 1.8110, 2.3057, 3.7617, 
                   2.9478, 1.9429, 1.5674, 1.3184, 1.2100, 1.2153, 1.3237, 1.5664, 1.9902, 2.9966, 
                   2.6294, 1.8320, 1.4385, 1.2271, 1.1270, 1.1216, 1.2427, 1.4619, 1.8745, 2.6963, 
                   2.4106, 1.6885, 1.3511, 1.1445, 1.0220, 1.0488, 1.1318, 1.3696, 1.7217, 2.5049, 
                   2.2949, 1.6470, 1.2886, 1.0942, 0.9971, 0.9868, 1.1089, 1.3130, 1.6714, 2.3472, 
                   2.3154, 1.6304, 1.3125, 1.0850, 0.9985, 1.0093, 1.0923, 1.3149, 1.6738, 2.3672, 
                   2.2891, 1.6504, 1.2832, 1.1089, 0.9795, 0.9951, 1.1074, 1.3149, 1.6655, 2.3662, 
                   2.5103, 1.7207, 1.3818, 1.1655, 1.0566, 1.0625, 1.1694, 1.3955, 1.7598, 2.5669, 
                   2.6284, 1.8730, 1.4590, 1.2520, 1.1519, 1.1523, 1.2642, 1.4697, 1.8940, 2.7295, 
                   3.2412, 1.9746, 1.5967, 1.3696, 1.2051, 1.2505, 1.3379, 1.6270, 2.0308, 3.1763, 
                   3.6626, 2.2036, 1.7524, 1.4995, 1.3364, 1.3804, 1.4985, 1.7896, 2.3198, 3.6001, 
                   2.8735, 1.8955, 1.5376, 1.2983, 1.2036, 1.2080, 1.3091, 1.5269, 1.9756, 2.8428, 
                   2.5762, 1.7954, 1.4307, 1.2163, 1.1250, 1.1279, 1.2339, 1.4590, 1.8491, 2.6855, 
                   2.3506, 1.6670, 1.3330, 1.1313, 1.0288, 1.0454, 1.1436, 1.3599, 1.7251, 2.4292, 
                   2.2979, 1.5918, 1.2949, 1.0859, 0.9834, 0.9946, 1.1016, 1.2930, 1.6416, 2.3379, 
                   2.2417, 1.6235, 1.2881, 1.0845, 1.0112, 1.0059, 1.1143, 1.3213, 1.6694, 2.3569, 
                   2.2676, 1.6016, 1.2827, 1.0903, 0.9717, 1.0024, 1.0894, 1.3022, 1.6387, 2.3169, 
                   2.4136, 1.6929, 1.3677, 1.1523, 1.0640, 1.0674, 1.1826, 1.3862, 1.7612, 2.4966, 
                   2.6230, 1.8320, 1.4292, 1.2402, 1.1387, 1.1470, 1.2354, 1.4634, 1.8584, 2.6626, 
                   3.0088, 1.9487, 1.6074, 1.3398, 1.2295, 1.2476, 1.3828, 1.5625, 2.0811, 3.0229, 
                },
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.1992, 2.3433, 1.9121, 1.5298, 1.4131, 1.4233, 1.5537, 1.9023, 2.4263, 4.1050, 
                   3.1421, 2.0850, 1.6104, 1.3647, 1.2305, 1.2461, 1.3516, 1.6318, 2.1030, 3.1899, 
                   2.8296, 1.8955, 1.4897, 1.2407, 1.1377, 1.1323, 1.2539, 1.4883, 1.9395, 2.8428, 
                   2.5220, 1.7749, 1.3657, 1.1577, 1.0308, 1.0361, 1.1577, 1.3940, 1.7847, 2.6113, 
                   2.4526, 1.6831, 1.3301, 1.0957, 0.9961, 1.0024, 1.1006, 1.3301, 1.7251, 2.4331, 
                   2.4526, 1.6948, 1.3179, 1.1001, 0.9995, 0.9907, 1.1050, 1.3281, 1.7026, 2.4526, 
                   2.4761, 1.6992, 1.3330, 1.1157, 0.9922, 0.9971, 1.1094, 1.3228, 1.7305, 2.4478, 
                   2.6470, 1.7856, 1.4067, 1.1729, 1.0654, 1.0669, 1.2046, 1.4087, 1.8306, 2.6416, 
                   2.8472, 1.9229, 1.5029, 1.2622, 1.1470, 1.1445, 1.2500, 1.4946, 1.9546, 2.8765, 
                   3.3706, 2.1104, 1.6577, 1.3687, 1.2622, 1.2603, 1.4233, 1.6392, 2.1929, 3.2603, 
                   3.7437, 2.2642, 1.7798, 1.4980, 1.3921, 1.3823, 1.4985, 1.8442, 2.2637, 3.9390, 
                   3.0400, 1.9692, 1.5884, 1.3345, 1.2163, 1.2207, 1.3511, 1.5649, 2.0352, 2.9741, 
                   2.6152, 1.8320, 1.4404, 1.2212, 1.1313, 1.1309, 1.2285, 1.4644, 1.8540, 2.7119, 
                   2.3862, 1.7114, 1.3472, 1.1450, 1.0313, 1.0356, 1.1504, 1.3643, 1.7402, 2.4795, 
                   2.3403, 1.6436, 1.3008, 1.0933, 0.9976, 0.9878, 1.1060, 1.2998, 1.6680, 2.3350, 
                   2.2754, 1.6333, 1.2959, 1.0869, 0.9897, 0.9985, 1.0933, 1.3110, 1.6694, 2.3560, 
                   2.3408, 1.6431, 1.3013, 1.1055, 0.9888, 0.9937, 1.1084, 1.3154, 1.6753, 2.3394, 
                   2.4731, 1.7427, 1.3721, 1.1719, 1.0522, 1.0625, 1.1563, 1.3931, 1.7476, 2.5264, 
                   2.6890, 1.8286, 1.4668, 1.2324, 1.1372, 1.1460, 1.2505, 1.4727, 1.8833, 2.7197, 
                   3.1987, 2.0332, 1.6094, 1.3706, 1.2510, 1.2393, 1.3496, 1.6387, 2.0278, 3.1948, 
                   3.7358, 2.1665, 1.7969, 1.4893, 1.3394, 1.3931, 1.4990, 1.8223, 2.3691, 3.6719, 
                   2.8809, 1.9331, 1.5830, 1.3003, 1.2417, 1.2202, 1.3589, 1.5513, 2.0581, 2.8774, 
                   2.5972, 1.7915, 1.4209, 1.2251, 1.1172, 1.1396, 1.2104, 1.4858, 1.8511, 2.7061, 
                   2.3564, 1.6665, 1.3550, 1.1245, 1.0332, 1.0527, 1.1563, 1.3613, 1.7632, 2.4351, 
                   2.2788, 1.6338, 1.2773, 1.0908, 1.0044, 1.0063, 1.1016, 1.3271, 1.6738, 2.3555, 
                   2.2368, 1.6030, 1.3018, 1.0752, 1.0098, 0.9995, 1.1152, 1.3193, 1.6763, 2.3662, 
                   2.3076, 1.6309, 1.2998, 1.1104, 0.9790, 1.0112, 1.0996, 1.3164, 1.6553, 2.3730, 
                   2.4287, 1.7217, 1.3726, 1.1621, 1.0713, 1.0654, 1.1792, 1.3955, 1.7524, 2.5547, 
                   2.6240, 1.8091, 1.4648, 1.2363, 1.1387, 1.1479, 1.2310, 1.4722, 1.8638, 2.6611, 
                   3.0371, 2.0659, 1.5962, 1.3877, 1.2427, 1.2788, 1.3477, 1.6167, 2.0439, 3.1831, 
                },
             },
             /* awb para 1 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   3.8623, 2.3374, 1.7646, 1.5356, 1.3662, 1.4063, 1.5190, 1.8076, 2.3306, 3.7979, 
                   3.0981, 2.0151, 1.5991, 1.3687, 1.2480, 1.2549, 1.3589, 1.6108, 2.0391, 3.0825, 
                   2.6689, 1.8364, 1.4492, 1.2339, 1.1270, 1.1353, 1.2495, 1.4668, 1.8716, 2.6538, 
                   2.4233, 1.7310, 1.3574, 1.1680, 1.0444, 1.0435, 1.1567, 1.3818, 1.7476, 2.4971, 
                   2.3882, 1.6445, 1.3257, 1.1055, 0.9912, 1.0063, 1.1104, 1.3345, 1.6821, 2.3657, 
                   2.3521, 1.6289, 1.3159, 1.0845, 1.0044, 0.9961, 1.0957, 1.3281, 1.6689, 2.3042, 
                   2.3701, 1.6548, 1.3340, 1.1240, 1.0059, 1.0015, 1.1235, 1.3462, 1.6772, 2.4087, 
                   2.5737, 1.7217, 1.3936, 1.1807, 1.0552, 1.0674, 1.1748, 1.3945, 1.7603, 2.4639, 
                   2.7495, 1.8345, 1.4775, 1.2456, 1.1616, 1.1318, 1.2593, 1.4976, 1.8496, 2.7930, 
                   3.3662, 2.0322, 1.6572, 1.3882, 1.2690, 1.2773, 1.4038, 1.6206, 2.0801, 3.1636, 
                   3.7710, 2.2446, 1.7661, 1.5005, 1.3745, 1.3853, 1.5098, 1.8311, 2.2500, 3.8540, 
                   2.9961, 1.9717, 1.5732, 1.3252, 1.2246, 1.2305, 1.3296, 1.5869, 2.0078, 2.9424, 
                   2.6167, 1.8188, 1.4316, 1.2231, 1.1265, 1.1240, 1.2368, 1.4575, 1.8467, 2.7012, 
                   2.4092, 1.6934, 1.3467, 1.1431, 1.0317, 1.0381, 1.1348, 1.3672, 1.7104, 2.4346, 
                   2.3193, 1.6255, 1.3037, 1.0918, 0.9946, 0.9971, 1.1123, 1.3110, 1.6777, 2.3379, 
                   2.2983, 1.6143, 1.2935, 1.0786, 1.0010, 0.9941, 1.0942, 1.3066, 1.6558, 2.3184, 
                   2.3198, 1.6377, 1.3018, 1.1084, 0.9922, 0.9976, 1.1138, 1.3218, 1.6719, 2.3623, 
                   2.4609, 1.7061, 1.3745, 1.1489, 1.0605, 1.0596, 1.1650, 1.3818, 1.7456, 2.5054, 
                   2.6543, 1.8442, 1.4448, 1.2378, 1.1450, 1.1372, 1.2466, 1.4805, 1.8574, 2.7480, 
                   3.1655, 2.0215, 1.6367, 1.3633, 1.2671, 1.2534, 1.3643, 1.6113, 2.0620, 3.0894, 
                   3.7603, 2.1733, 1.7725, 1.4741, 1.3638, 1.3599, 1.4785, 1.7744, 2.2871, 3.6040, 
                   2.8887, 1.9268, 1.5713, 1.3184, 1.2144, 1.2246, 1.3398, 1.5654, 2.0156, 2.9600, 
                   2.6162, 1.7720, 1.4175, 1.2104, 1.1138, 1.1152, 1.2095, 1.4697, 1.8125, 2.7314, 
                   2.3384, 1.6890, 1.3140, 1.1489, 1.0317, 1.0410, 1.1577, 1.3467, 1.7358, 2.4141, 
                   2.3325, 1.6250, 1.3008, 1.1025, 0.9922, 0.9985, 1.0933, 1.3374, 1.6421, 2.4233, 
                   2.2217, 1.6152, 1.2661, 1.0898, 0.9961, 0.9951, 1.1050, 1.3042, 1.6479, 2.3296, 
                   2.2974, 1.6377, 1.2993, 1.1128, 0.9941, 1.0059, 1.0986, 1.3457, 1.6484, 2.4141, 
                   2.4243, 1.6885, 1.3564, 1.1553, 1.0566, 1.0605, 1.1729, 1.3799, 1.7515, 2.5034, 
                   2.6143, 1.8311, 1.4453, 1.2402, 1.1426, 1.1382, 1.2378, 1.4761, 1.8545, 2.6890, 
                   3.0015, 2.0210, 1.5894, 1.3853, 1.2480, 1.2739, 1.3687, 1.6567, 2.0400, 3.2544, 
                },
             },
             /* awb para 2 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0791, 2.3071, 1.8794, 1.5049, 1.3950, 1.3872, 1.5635, 1.8208, 2.4116, 4.0425, 
                   3.1987, 2.0659, 1.6440, 1.3647, 1.2593, 1.2534, 1.3638, 1.6455, 2.1035, 3.1821, 
                   2.7422, 1.8774, 1.4590, 1.2212, 1.1260, 1.1079, 1.2446, 1.4629, 1.9092, 2.8193, 
                   2.5249, 1.7588, 1.3857, 1.1650, 1.0464, 1.0562, 1.1606, 1.4067, 1.7891, 2.6040, 
                   2.4614, 1.6870, 1.3330, 1.1035, 1.0103, 0.9897, 1.1304, 1.3320, 1.7393, 2.4697, 
                   2.3799, 1.6724, 1.2988, 1.0830, 0.9878, 0.9883, 1.0928, 1.3125, 1.6870, 2.3818, 
                   2.4946, 1.7061, 1.3535, 1.1392, 1.0078, 1.0161, 1.1318, 1.3677, 1.7480, 2.5322, 
                   2.5493, 1.7900, 1.3804, 1.1616, 1.0654, 1.0586, 1.1782, 1.3984, 1.7974, 2.5869, 
                   2.8740, 1.8994, 1.5127, 1.2642, 1.1489, 1.1479, 1.2666, 1.5166, 1.9575, 2.8789, 
                   3.3486, 2.1646, 1.6880, 1.3999, 1.2983, 1.2808, 1.4082, 1.7100, 2.1621, 3.4326, 
                   3.7246, 2.2808, 1.7480, 1.5278, 1.3652, 1.3857, 1.5112, 1.8262, 2.2856, 3.7935, 
                   3.0601, 1.9766, 1.6162, 1.3364, 1.2378, 1.2466, 1.3389, 1.5986, 2.0483, 3.0044, 
                   2.6060, 1.8096, 1.4214, 1.2163, 1.1104, 1.1191, 1.2319, 1.4580, 1.8481, 2.6851, 
                   2.4604, 1.7153, 1.3770, 1.1509, 1.0483, 1.0522, 1.1533, 1.3892, 1.7456, 2.5171, 
                   2.3057, 1.6431, 1.3013, 1.0981, 0.9966, 1.0000, 1.1177, 1.3159, 1.6968, 2.3809, 
                   2.2690, 1.6211, 1.2920, 1.0757, 0.9829, 0.9976, 1.0894, 1.3145, 1.6499, 2.3667, 
                   2.3545, 1.6606, 1.3223, 1.1152, 1.0112, 1.0122, 1.1299, 1.3418, 1.7090, 2.4395, 
                   2.4199, 1.7129, 1.3667, 1.1455, 1.0503, 1.0537, 1.1636, 1.3906, 1.7383, 2.5439, 
                   2.6929, 1.8569, 1.4580, 1.2485, 1.1431, 1.1548, 1.2661, 1.4912, 1.9077, 2.7798, 
                   3.1938, 2.0444, 1.6431, 1.3701, 1.2788, 1.2588, 1.3706, 1.6611, 2.0649, 3.2065, 
                   3.7676, 2.1494, 1.7959, 1.4575, 1.3784, 1.3604, 1.4849, 1.8022, 2.2222, 3.5879, 
                   2.8955, 1.9805, 1.5562, 1.3379, 1.2290, 1.2344, 1.3447, 1.5879, 2.0161, 3.0039, 
                   2.5591, 1.7646, 1.4189, 1.1973, 1.1206, 1.1182, 1.2227, 1.4443, 1.8276, 2.6543, 
                   2.3511, 1.7134, 1.3359, 1.1577, 1.0361, 1.0483, 1.1626, 1.3647, 1.7427, 2.4536, 
                   2.2734, 1.6343, 1.2954, 1.0913, 1.0000, 0.9990, 1.1133, 1.3164, 1.6733, 2.3857, 
                   2.2153, 1.5972, 1.2783, 1.0781, 0.9951, 0.9912, 1.0894, 1.3018, 1.6392, 2.2881, 
                   2.3003, 1.6680, 1.3169, 1.1230, 1.0068, 1.0229, 1.1274, 1.3267, 1.7017, 2.4043, 
                   2.4150, 1.6978, 1.3374, 1.1655, 1.0420, 1.0562, 1.1563, 1.3745, 1.7373, 2.4756, 
                   2.6450, 1.8271, 1.4756, 1.2261, 1.1631, 1.1484, 1.2549, 1.4868, 1.8643, 2.7544, 
                   3.0923, 2.0610, 1.5869, 1.4268, 1.2461, 1.2861, 1.3608, 1.6191, 2.0957, 3.0503, 
                },
             },
             /* awb para 3 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0630, 2.3340, 1.8652, 1.5303, 1.3970, 1.4116, 1.5674, 1.8408, 2.4243, 4.0415, 
                   3.1001, 2.0317, 1.6133, 1.3320, 1.2319, 1.2134, 1.3462, 1.6001, 2.0596, 3.1025, 
                   2.7930, 1.8813, 1.4736, 1.2334, 1.1274, 1.1309, 1.2607, 1.4624, 1.9346, 2.7861, 
                   2.5044, 1.7524, 1.3711, 1.1455, 1.0391, 1.0278, 1.1489, 1.3799, 1.7539, 2.5664, 
                   2.4048, 1.6772, 1.3159, 1.0981, 0.9966, 0.9966, 1.1167, 1.3203, 1.7124, 2.4189, 
                   2.3779, 1.6885, 1.3145, 1.0908, 1.0049, 1.0068, 1.1006, 1.3442, 1.6987, 2.4556, 
                   2.4097, 1.6768, 1.3262, 1.1104, 1.0000, 0.9922, 1.1074, 1.3384, 1.6963, 2.4819, 
                   2.5972, 1.7876, 1.4038, 1.1777, 1.0659, 1.0864, 1.1782, 1.4307, 1.7979, 2.6611, 
                   2.7847, 1.9072, 1.4810, 1.2598, 1.1553, 1.1396, 1.2681, 1.5029, 1.9277, 2.9194, 
                   3.2676, 2.0850, 1.6733, 1.3750, 1.2549, 1.3003, 1.3608, 1.6890, 2.1235, 3.2163, 
                   3.7461, 2.2451, 1.7700, 1.4917, 1.3638, 1.3794, 1.4966, 1.8110, 2.3057, 3.7617, 
                   2.9478, 1.9429, 1.5674, 1.3184, 1.2100, 1.2153, 1.3237, 1.5664, 1.9902, 2.9966, 
                   2.6294, 1.8320, 1.4385, 1.2271, 1.1270, 1.1216, 1.2427, 1.4619, 1.8745, 2.6963, 
                   2.4106, 1.6885, 1.3511, 1.1445, 1.0220, 1.0488, 1.1318, 1.3696, 1.7217, 2.5049, 
                   2.2949, 1.6470, 1.2886, 1.0942, 0.9971, 0.9868, 1.1089, 1.3130, 1.6714, 2.3472, 
                   2.3154, 1.6304, 1.3125, 1.0850, 0.9985, 1.0093, 1.0923, 1.3149, 1.6738, 2.3672, 
                   2.2891, 1.6504, 1.2832, 1.1089, 0.9795, 0.9951, 1.1074, 1.3149, 1.6655, 2.3662, 
                   2.5103, 1.7207, 1.3818, 1.1655, 1.0566, 1.0625, 1.1694, 1.3955, 1.7598, 2.5669, 
                   2.6284, 1.8730, 1.4590, 1.2520, 1.1519, 1.1523, 1.2642, 1.4697, 1.8940, 2.7295, 
                   3.2412, 1.9746, 1.5967, 1.3696, 1.2051, 1.2505, 1.3379, 1.6270, 2.0308, 3.1763, 
                   3.6626, 2.2036, 1.7524, 1.4995, 1.3364, 1.3804, 1.4985, 1.7896, 2.3198, 3.6001, 
                   2.8735, 1.8955, 1.5376, 1.2983, 1.2036, 1.2080, 1.3091, 1.5269, 1.9756, 2.8428, 
                   2.5762, 1.7954, 1.4307, 1.2163, 1.1250, 1.1279, 1.2339, 1.4590, 1.8491, 2.6855, 
                   2.3506, 1.6670, 1.3330, 1.1313, 1.0288, 1.0454, 1.1436, 1.3599, 1.7251, 2.4292, 
                   2.2979, 1.5918, 1.2949, 1.0859, 0.9834, 0.9946, 1.1016, 1.2930, 1.6416, 2.3379, 
                   2.2417, 1.6235, 1.2881, 1.0845, 1.0112, 1.0059, 1.1143, 1.3213, 1.6694, 2.3569, 
                   2.2676, 1.6016, 1.2827, 1.0903, 0.9717, 1.0024, 1.0894, 1.3022, 1.6387, 2.3169, 
                   2.4136, 1.6929, 1.3677, 1.1523, 1.0640, 1.0674, 1.1826, 1.3862, 1.7612, 2.4966, 
                   2.6230, 1.8320, 1.4292, 1.2402, 1.1387, 1.1470, 1.2354, 1.4634, 1.8584, 2.6626, 
                   3.0088, 1.9487, 1.6074, 1.3398, 1.2295, 1.2476, 1.3828, 1.5625, 2.0811, 3.0229, 
                },
             },
          },
       },
    },
    /* isp_sub_module_drc_pra isp_sub_module_drc_tuning */
    {
       1,//enable
       1,//interpolation_enable
       6,//ae count
       1,//trigger mode : 0 gain trigger 1 lux trigger
       257,//drc_point_size
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             90,
          },
          /* aec trigger 1 */
          {
             130,
             190,
          },
          /* aec trigger 2 */
          {
             220,
             300,
          },
          /* aec trigger 3 */
          {
             320,
             400,
          },
          /* aec trigger 4 */
          {
             410,
             460,
          },
          /* aec trigger 5 */
          {
             465,
             500,
          },
       },
       /* isp_sub_module_drc_pra pra[MIDDLE_TRIGGER_COUNT] */
       {
           /* para 0*/
           {
              /* drc_low_LUT_point */
              {
                   40749,40749,37704,35523,33782,32320,31052,29930,28923,28007,27168,26393,25673,25001,24371,23779,23219,22689,22186,21708,21252,20817,20400,20001,19618,
                   19250,18896,18555,18226,17909,17602,17306,17019,16742,16473,16212,15959,15713,15474,15242,15017,14797,14583,14375,14172,13974,13782,13593,13410,13230,
                   13055,12884,12717,12553,12394,12237,12084,11934,11788,11644,11504,11366,11231,11098,10968,10841,10716,10594,10474,10356,10240,10126,10015,9905,9797,
                   9691,9587,9485,9385,9286,9189,9093,8999,8907,8816,8727,8639,8552,8467,8383,8300,8219,8138,8060,7982,7905,7830,7755,7682,7610,
                   7539,7468,7399,7331,7264,7198,7132,7068,7004,6941,6879,6818,6758,6699,6640,6582,6525,6468,6413,6358,6303,6250,6197,6145,6093,
                   6042,5992,5942,5893,5844,5796,5749,5702,5656,5610,5565,5520,5476,5433,5389,5347,5305,5263,5222,5181,5141,5101,5062,5023,4984,
                   4946,4909,4871,4835,4798,4762,4726,4691,4656,4622,4587,4554,4520,4487,4454,4422,4390,4358,4326,4295,4265,4234,4204,4174,4144,
                   4115,4086,4057,4029,4001,3973,3945,3918,3891,3864,3837,3811,3785,3759,3733,3708,3683,3658,3633,3609,3585,3561,3537,3514,3490,
                   3467,3444,3422,3399,3377,3355,3333,3311,3290,3269,3247,3227,3206,3185,3165,3145,3125,3105,3085,3066,3046,3027,3008,2989,2971,
                   2952,2934,2915,2897,2879,2862,2844,2827,2809,2792,2775,2758,2741,2725,2708,2692,2676,2660,2644,2628,2612,2596,2581,2566,2550,
                   2535,2520,2506,2491,2476,2462,2447,
              },
              /* drc_high_LUT_point */
              {
                   23217,23217,17017,13408,10966,9187,7828,6756,5891,5180,4586,4085,3657,3289,2970,2691,2446,2231,2039,1869,1717,1580,1457,1346,1246,
                   1155,1072, 997, 928, 865, 807, 754, 705, 661, 619, 581, 545, 512, 482, 454, 427, 403, 380, 359, 339, 320, 303, 287, 271, 257,
                    244, 231, 219, 208, 198, 188, 179, 170, 162, 154, 147, 140, 133, 127, 121, 115, 110, 105, 100,  96,  92,  88,  84,  80,  77,
                     73,  70,  67,  64,  62,  59,  57,  54,  52,  50,  48,  46,  44,  42,  41,  39,  38,  36,  35,  33,  32,  31,  30,  29,  27,
                     26,  25,  24,  24,  23,  22,  21,  20,  19,  19,  18,  17,  17,  16,  16,  15,  14,  14,  13,  13,  12,  12,  12,  11,  11,
                     10,  10,  10,   9,   9,   9,   8,   8,   8,   8,   7,   7,   7,   7,   6,   6,   6,   6,   5,   5,   5,   5,   5,   5,   4,
                      4,   4,   4,   4,   4,   4,   3,   3,   3,   3,   3,   3,   3,   3,   3,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
                      2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,
              },
              /* saturation */
              255,
              /* fl1 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* fl2 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* fl3 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* k1 */
              6,
              /* k2 */
              12,
              /* k3 */
              0.5,
           },
           /* para 1*/
           {
              /* drc_low_LUT_point */
              {
                   64047,64047,60423,57782,55645,53828,52236,50814,49526,48345,47254,46239,45290,44398,43556,42759,42002,41281,40593,39935,39304,38698,38116,37555,37014,
                   36492,35987,35499,35026,34567,34123,33691,33271,32863,32467,32080,31704,31337,30979,30630,30289,29956,29631,29313,29002,28698,28400,28109,27823,27544,
                   27270,27001,26738,26480,26226,25978,25734,25494,25259,25027,24800,24577,24358,24142,23930,23721,23516,23314,23115,22919,22727,22537,22351,22167,21986,
                   21807,21631,21458,21287,21119,20953,20790,20628,20469,20312,20157,20005,19854,19705,19558,19413,19270,19129,18990,18852,18716,18582,18449,18319,18189,
                   18061,17935,17810,17687,17565,17445,17326,17208,17091,16976,16863,16750,16639,16529,16420,16313,16206,16101,15997,15894,15792,15691,15591,15492,15395,
                   15298,15202,15107,15013,14921,14829,14738,14647,14558,14470,14382,14296,14210,14125,14041,13957,13875,13793,13712,13632,13552,13473,13395,13318,13241,
                   13165,13090,13015,12941,12868,12795,12723,12652,12581,12511,12442,12373,12304,12237,12170,12103,12037,11971,11907,11842,11778,11715,11652,11590,11528,
                   11467,11406,11346,11286,11227,11168,11110,11052,10995,10938,10881,10825,10769,10714,10659,10605,10551,10498,10445,10392,10340,10288,10236,10185,10134,
                   10084,10034,9984,9935,9886,9838,9789,9742,9694,9647,9600,9554,9508,9462,9416,9371,9326,9282,9238,9194,9150,9107,9064,9021,8979,
                   8937,8895,8854,8812,8771,8731,8690,8650,8610,8571,8532,8493,8454,8415,8377,8339,8301,8264,8226,8189,8153,8116,8080,8044,8008,
                   7972,7937,7902,7867,7832,7798,7763,
              },
              /* drc_high_LUT_point */
              {
                   41999,41999,33268,27820,23926,20950,18579,16636,15011,13629,12439,11404,10495,9692,8977,8337,7761,7241,6770,6341,5949,5590,5260,4957,4677,
                   4418,4178,3956,3748,3556,3376,3208,3050,2903,2765,2635,2513,2398,2290,2188,2092,2001,1915,1834,1757,1684,1615,1549,1487,1428,
                   1372,1318,1267,1218,1172,1128,1086,1045,1007, 970, 935, 902, 870, 839, 809, 781, 754, 728, 704, 680, 657, 635, 614, 594, 574,
                    555, 537, 520, 503, 487, 472, 457, 443, 429, 416, 403, 391, 379, 367, 356, 345, 335, 325, 315, 306, 297, 288, 280, 272, 264,
                    256, 249, 242, 235, 228, 222, 216, 210, 204, 198, 192, 187, 182, 177, 172, 167, 163, 158, 154, 150, 146, 142, 138, 134, 131,
                    127, 124, 121, 117, 114, 111, 108, 105, 103, 100,  97,  95,  92,  90,  88,  85,  83,  81,  79,  77,  75,  73,  71,  69,  67,
                     66,  64,  62,  61,  59,  57,  56,  54,  53,  52,  50,  49,  48,  46,  45,  44,  43,  42,  41,  40,  39,  38,  37,  36,  35,
                     34,  33,  32,  31,  30,  29,  29,  28,  27,  26,  25,  25,  24,  23,  23,  22,  21,  21,  20,  20,  19,  18,  18,  17,  17,
                     16,  16,  15,  15,  14,  14,  13,  13,  13,  12,  12,  11,  11,  10,  10,  10,   9,   9,   9,   8,   8,   8,   7,   7,   7,
                      6,   6,   6,   5,   5,   5,   5,   4,   4,   4,   4,   3,   3,   3,   3,   3,   2,   2,   2,   2,   1,   1,   1,   1,   1,
                      1,   0,   0,   0,   0,   0,   0,
              },
              /* saturation */
              200,
              /* fl1 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* fl2 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* fl3 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* k1 */
              18,
              /* k2 */
              9,
              /* k3 */
              0.5,
           },
           /* para 2*/
           {
              /* drc_low_LUT_point */
              {
                   47177,47177,46351,45693,45127,44621,44159,43732,43332,42955,42598,42257,41931,41618,41317,41027,40746,40474,40210,39953,39703,39460,39223,38991,38765,
                   38544,38327,38115,37907,37703,37504,37307,37115,36925,36739,36556,36376,36199,36025,35853,35684,35517,35352,35190,35030,34873,34717,34563,34412,34262,
                   34114,33968,33824,33681,33540,33401,33263,33127,32992,32859,32727,32597,32468,32340,32214,32088,31964,31842,31720,31600,31481,31363,31246,31130,31015,
                   30901,30788,30677,30566,30456,30347,30239,30132,30026,29920,29816,29712,29610,29508,29406,29306,29207,29108,29010,28912,28816,28720,28625,28530,28437,
                   28344,28251,28160,28069,27978,27888,27799,27711,27623,27535,27449,27362,27277,27192,27107,27023,26940,26857,26775,26693,26612,26531,26451,26371,26292,
                   26213,26135,26057,25980,25903,25827,25751,25675,25600,25526,25452,25378,25304,25232,25159,25087,25015,24944,24873,24803,24733,24663,24594,24525,24457,
                   24388,24321,24253,24186,24120,24053,23987,23922,23856,23791,23727,23662,23598,23535,23471,23409,23346,23283,23221,23160,23098,23037,22976,22916,22856,
                   22796,22736,22677,22618,22559,22500,22442,22384,22327,22269,22212,22155,22099,22042,21986,21930,21875,21820,21765,21710,21655,21601,21547,21493,21439,
                   21386,21333,21280,21227,21175,21123,21071,21019,20968,20917,20865,20815,20764,20714,20663,20613,20564,20514,20465,20416,20367,20318,20270,20221,20173,
                   20125,20077,20030,19983,19935,19889,19842,19795,19749,19703,19657,19611,19565,19520,19474,19429,19384,19340,19295,19251,19206,19162,19118,19075,19031,
                   18988,18945,18902,18859,18816,18773,18731,
              },
              /* drc_high_LUT_point */
              {
                   40745,40745,37113,34410,32212,30345,28718,27274,25977,24801,23724,22734,21817,20965,20171,19427,18728,18071,17451,16865,16309,15783,15282,14805,14351,
                   13918,13504,13108,12729,12366,12017,11683,11362,11053,10756,10470,10195,9929,9673,9426,9188,8957,8735,8519,8311,8110,7914,7725,7542,7365,
                   7193,7026,6864,6707,6554,6406,6262,6122,5986,5854,5725,5600,5478,5360,5245,5132,5023,4917,4813,4712,4613,4517,4423,4332,4243,
                   4156,4071,3988,3907,3828,3751,3676,3602,3530,3460,3391,3324,3259,3195,3132,3071,3011,2952,2895,2839,2784,2730,2678,2626,2576,
                   2527,2479,2431,2385,2340,2295,2252,2209,2167,2126,2086,2047,2008,1970,1933,1897,1861,1827,1792,1759,1726,1693,1662,1631,1600,
                   1570,1541,1512,1483,1456,1428,1402,1375,1349,1324,1299,1275,1251,1227,1204,1181,1159,1137,1115,1094,1073,1053,1032,1013, 993,
                    974, 955, 937, 919, 901, 883, 866, 849, 832, 816, 800, 784, 768, 753, 738, 723, 708, 694, 680, 666, 652, 639, 626, 613, 600,
                    587, 575, 562, 550, 538, 527, 515, 504, 493, 482, 471, 460, 450, 440, 430, 420, 410, 400, 390, 381, 372, 363, 354, 345, 336,
                    328, 319, 311, 302, 294, 286, 279, 271, 263, 256, 248, 241, 234, 227, 220, 213, 206, 199, 193, 186, 180, 173, 167, 161, 155,
                    149, 143, 137, 132, 126, 120, 115, 109, 104,  99,  94,  89,  84,  79,  74,  69,  64,  59,  55,  50,  45,  41,  37,  32,  28,
                     24,  20,  15,  11,   7,   3,   0,
              },
              /* saturation */
              150,
              /* fl1 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* fl2 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* fl3 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* k1 */
              12,
              /* k2 */
              5,
              /* k3 */
              0.6,
           },
           /* para 3*/
           {
              /* drc_low_LUT_point */
              {
                   62043,62043,60326,58971,57814,56788,55857,55000,54204,53457,52752,52083,51447,50839,50256,49696,49157,48636,48133,47646,47175,46717,46272,45839,45417,
                   45006,44606,44215,43833,43460,43095,42737,42388,42045,41709,41380,41057,40741,40430,40124,39824,39529,39240,38955,38674,38398,38127,37860,37597,37338,
                   37083,36831,36583,36339,36098,35861,35627,35396,35168,34943,34721,34502,34286,34072,33861,33653,33448,33244,33044,32845,32649,32455,32264,32074,31887,
                   31702,31519,31338,31158,30981,30806,30633,30461,30291,30123,29957,29792,29629,29468,29308,29150,28993,28838,28684,28532,28381,28232,28084,27938,27793,
                   27649,27506,27365,27225,27086,26949,26813,26678,26544,26411,26280,26149,26020,25892,25765,25639,25514,25390,25267,25145,25024,24904,24785,24667,24550,
                   24433,24318,24204,24090,23978,23866,23755,23645,23536,23428,23320,23214,23108,23003,22898,22795,22692,22590,22489,22388,22288,22189,22091,21993,21896,
                   21800,21704,21609,21515,21421,21328,21236,21144,21053,20962,20872,20783,20694,20606,20519,20432,20346,20260,20175,20090,20006,19923,19840,19757,19675,
                   19594,19513,19433,19353,19273,19195,19116,19038,18961,18884,18808,18732,18656,18581,18507,18433,18359,18286,18213,18141,18069,17998,17927,17856,17786,
                   17716,17647,17578,17509,17441,17373,17306,17239,17173,17106,17041,16975,16910,16845,16781,16717,16654,16590,16528,16465,16403,16341,16280,16218,16158,
                   16097,16037,15977,15918,15859,15800,15741,15683,15625,15568,15511,15454,15397,15341,15285,15229,15173,15118,15063,15009,14955,14901,14847,14793,14740,
                   14687,14635,14582,14530,14479,14427,14376,
              },
              /* drc_high_LUT_point */
              {
                   49154,49154,42385,37594,33858,30803,28229,26016,24087,22385,20869,19509,18283,17169,16154,15226,14373,13587,12860,12188,11563,10982,10440,9933,9460,
                   9016,8599,8208,7840,7493,7166,6857,6565,6289,6028,5780,5545,5322,5110,4909,4718,4535,4362,4196,4039,3888,3745,3607,3476,3351,
                   3231,3116,3007,2902,2801,2704,2612,2523,2437,2356,2277,2201,2129,2059,1992,1928,1865,1806,1748,1693,1640,1588,1539,1491,1445,
                   1400,1358,1316,1276,1238,1201,1165,1130,1097,1064,1033,1003, 974, 945, 918, 891, 866, 841, 817, 794, 771, 750, 728, 708, 688,
                    669, 650, 632, 615, 598, 582, 566, 550, 535, 521, 507, 493, 480, 467, 454, 442, 430, 419, 407, 397, 386, 376, 366, 356, 347,
                    338, 329, 320, 312, 304, 296, 288, 281, 273, 266, 259, 253, 246, 240, 233, 227, 221, 216, 210, 205, 199, 194, 189, 184, 180,
                    175, 170, 166, 162, 158, 153, 149, 146, 142, 138, 135, 131, 128, 124, 121, 118, 115, 112, 109, 106, 103, 100,  98,  95,  92,
                     90,  88,  85,  83,  81,  78,  76,  74,  72,  70,  68,  66,  64,  63,  61,  59,  57,  56,  54,  52,  51,  49,  48,  46,  45,
                     44,  42,  41,  40,  38,  37,  36,  35,  34,  33,  31,  30,  29,  28,  27,  26,  25,  24,  23,  22,  22,  21,  20,  19,  18,
                     17,  17,  16,  15,  14,  14,  13,  12,  12,  11,  10,  10,   9,   8,   8,   7,   7,   6,   6,   5,   5,   4,   4,   3,   3,
                      2,   2,   1,   1,   0,   0,   0,
              },
              /* saturation */
              150,
              /* fl1 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* fl2 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* fl3 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* k1 */
              8,
              /* k2 */
              8,
              /* k3 */
              0.6,
           },
           /* para 4*/
           {
              /* drc_low_LUT_point */
              {
                   28907,28907,27448,26378,25509,24767,24115,23530,22999,22511,22058,21637,21241,20869,20517,20183,19865,19561,19271,18993,18726,18469,18221,17983,17752,
                   17529,17314,17104,16902,16705,16514,16328,16147,15971,15799,15632,15468,15309,15154,15002,14853,14708,14566,14427,14290,14157,14026,13898,13773,13650,
                   13529,13410,13294,13180,13067,12957,12849,12742,12638,12535,12434,12334,12236,12140,12045,11951,11859,11769,11679,11591,11505,11419,11335,11252,11170,
                   11090,11010,10932,10854,10778,10703,10628,10555,10482,10411,10340,10270,10202,10134,10066,10000,9935,9870,9806,9743,9680,9618,9557,9497,9437,
                   9378,9320,9262,9205,9149,9093,9038,8983,8929,8875,8822,8770,8718,8667,8616,8566,8516,8467,8418,8370,8322,8275,8228,8182,8136,
                   8090,8045,8001,7956,7913,7869,7826,7784,7741,7700,7658,7617,7576,7536,7496,7456,7417,7378,7340,7301,7263,7226,7189,7152,7115,
                   7079,7043,7007,6971,6936,6902,6867,6833,6799,6765,6732,6698,6665,6633,6600,6568,6536,6505,6473,6442,6411,6381,6350,6320,6290,
                   6260,6231,6201,6172,6144,6115,6086,6058,6030,6002,5975,5947,5920,5893,5866,5840,5813,5787,5761,5735,5709,5684,5659,5633,5608,
                   5584,5559,5535,5510,5486,5462,5438,5415,5391,5368,5345,5322,5299,5276,5254,5231,5209,5187,5165,5143,5121,5100,5078,5057,5036,
                   5015,4994,4973,4953,4932,4912,4892,4871,4851,4832,4812,4792,4773,4753,4734,4715,4696,4677,4658,4640,4621,4603,4584,4566,4548,
                   4530,4512,4494,4477,4459,4442,4424,
              },
              /* drc_high_LUT_point */
              {
                   19863,19863,16145,13771,12043,10701,9617,8717,7955,7300,6730,6230,5786,5390,5035,4714,4423,4158,3917,3695,3491,3303,3129,2967,2818,
                   2678,2548,2427,2313,2207,2107,2014,1925,1842,1764,1690,1620,1554,1491,1432,1376,1322,1271,1223,1177,1133,1092,1052,1014, 978,
                    943, 910, 879, 849, 820, 792, 766, 740, 716, 692, 670, 648, 628, 608, 589, 570, 553, 536, 519, 503, 488, 473, 459, 446, 433,
                    420, 408, 396, 384, 373, 363, 353, 343, 333, 324, 315, 306, 298, 289, 282, 274, 266, 259, 252, 246, 239, 233, 227, 221, 215,
                    209, 204, 199, 193, 188, 184, 179, 174, 170, 166, 161, 157, 153, 150, 146, 142, 139, 135, 132, 129, 126, 122, 119, 116, 114,
                    111, 108, 106, 103, 100,  98,  96,  93,  91,  89,  87,  85,  83,  81,  79,  77,  75,  73,  71,  70,  68,  66,  65,  63,  62,
                     60,  59,  57,  56,  55,  53,  52,  51,  50,  48,  47,  46,  45,  44,  43,  42,  41,  40,  39,  38,  37,  36,  35,  34,  33,
                     32,  32,  31,  30,  29,  29,  28,  27,  26,  26,  25,  24,  24,  23,  22,  22,  21,  21,  20,  19,  19,  18,  18,  17,  17,
                     16,  16,  15,  15,  14,  14,  14,  13,  13,  12,  12,  11,  11,  11,  10,  10,  10,   9,   9,   9,   8,   8,   8,   7,   7,
                      7,   6,   6,   6,   5,   5,   5,   5,   4,   4,   4,   4,   3,   3,   3,   3,   2,   2,   2,   2,   2,   1,   1,   1,   1,
                      1,   0,   0,   0,   0,   0,   0,
              },
              /* saturation */
              100,
              /* fl1 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* fl2 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* fl3 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* k1 */
              8,
              /* k2 */
              8,
              /* k3 */
              0.5,
           },
           /* para 5*/
           {
              /* drc_low_LUT_point */
              {
                   19201,19201,18656,18237,17886,17578,17302,17049,16816,16598,16394,16201,16017,15843,15676,15516,15363,15215,15073,14935,14801,14672,14547,14425,14307,
                   14191,14079,13969,13862,13758,13656,13556,13459,13363,13269,13178,13088,12999,12913,12828,12744,12662,12582,12502,12424,12348,12272,12198,12125,12053,
                   11982,11912,11843,11775,11708,11642,11577,11513,11450,11387,11326,11265,11205,11145,11086,11029,10971,10915,10859,10804,10749,10695,10642,10589,10537,
                   10485,10434,10383,10333,10284,10235,10187,10139,10091,10044,9998,9951,9906,9861,9816,9772,9728,9684,9641,9598,9556,9514,9473,9432,9391,
                   9350,9310,9271,9231,9192,9153,9115,9077,9039,9002,8965,8928,8891,8855,8819,8783,8748,8713,8678,8643,8609,8575,8541,8508,8475,
                   8442,8409,8376,8344,8312,8280,8248,8217,8186,8155,8124,8094,8064,8034,8004,7974,7945,7915,7886,7858,7829,7800,7772,7744,7716,
                   7689,7661,7634,7606,7580,7553,7526,7500,7473,7447,7421,7395,7370,7344,7319,7294,7269,7244,7219,7194,7170,7146,7122,7098,7074,
                   7050,7026,7003,6980,6957,6934,6911,6888,6865,6843,6820,6798,6776,6754,6732,6710,6689,6667,6646,6625,6603,6582,6561,6541,6520,
                   6499,6479,6459,6438,6418,6398,6378,6358,6339,6319,6299,6280,6261,6241,6222,6203,6184,6166,6147,6128,6110,6091,6073,6054,6036,
                   6018,6000,5982,5964,5947,5929,5912,5894,5877,5859,5842,5825,5808,5791,5774,5757,5741,5724,5707,5691,5674,5658,5642,5626,5609,
                   5593,5577,5562,5546,5530,5514,5499,
              },
              /* drc_high_LUT_point */
              {
                   15362,15362,13458,12124,11086,10234,9513,8890,8343,7857,7420,7025,6666,6338,6035,5756,5498,5257,5033,4824,4628,4444,4270,4107,3953,
                   3807,3670,3539,3415,3297,3185,3078,2976,2879,2786,2697,2612,2531,2453,2379,2307,2238,2172,2109,2048,1989,1933,1879,1826,1776,
                   1727,1680,1635,1591,1549,1508,1468,1430,1393,1358,1323,1290,1257,1226,1195,1166,1137,1109,1082,1056,1031,1006, 982, 959, 936,
                    915, 893, 872, 852, 833, 814, 795, 777, 759, 742, 726, 709, 694, 678, 663, 649, 634, 620, 607, 594, 581, 568, 556, 544, 532,
                    521, 510, 499, 488, 478, 468, 458, 448, 439, 430, 421, 412, 403, 395, 387, 379, 371, 363, 356, 348, 341, 334, 327, 320, 314,
                    307, 301, 295, 289, 283, 277, 271, 266, 260, 255, 250, 245, 240, 235, 230, 225, 221, 216, 212, 207, 203, 199, 195, 191, 187,
                    183, 179, 175, 172, 168, 164, 161, 158, 154, 151, 148, 145, 142, 139, 136, 133, 130, 127, 124, 122, 119, 116, 114, 111, 109,
                    106, 104, 102,  99,  97,  95,  93,  90,  88,  86,  84,  82,  80,  78,  76,  75,  73,  71,  69,  67,  66,  64,  62,  61,  59,
                     57,  56,  54,  53,  51,  50,  49,  47,  46,  44,  43,  42,  40,  39,  38,  37,  35,  34,  33,  32,  31,  30,  29,  27,  26,
                     25,  24,  23,  22,  21,  20,  19,  18,  17,  17,  16,  15,  14,  13,  12,  11,  10,  10,   9,   8,   7,   7,   6,   5,   4,
                      4,   3,   2,   2,   1,   0,   0,
              },
              /* saturation */
              100,
              /* fl1 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* fl2 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* fl3 */
              {
                   0,0,0,0,0,
                   0,0,255,0,0,
                   0,0,0,0,0,
              },
              /* k1 */
              5,
              /* k2 */
              6,
              /* k3 */
              0.55,
           },
       },
       0,//enable_auto_gen
    },
    /* isp_sub_module_gic_tuning_t isp_sub_module_gic_tuning */
    {
       0,//enable
       1,//interpolation_enable
       5,//ae count
       1,//trigger mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             259,
          },
          /* aec trigger 1 */
          {
             302,
             308,
          },
          /* aec trigger 2 */
          {
             325,
             328,
          },
          /* aec trigger 3 */
          {
             370,
             372,
          },
          /* aec trigger 4 */
          {
             395,
             396,
          },
       },
       /* isp_sub_module_gic_pra pra[MIDDLE_TRIGGER_COUNT] */
       {
           /* para 0*/
           {
              0,//enable
              256,//alphfa1
              128,//alphfa_red
              0,//k_filter_power
              9830,//b_value
              10,//ge_thres
              163,//g_slope
              60,//max_thres
              96,//k_thres
              40,//limit_base_green
              40,//limit_base_red
              1400,//limit_end_green
              1600,//limit_end_red
              256,//weight_filterg
              2048,//k_diff_base
           },
           /* para 1*/
           {
              0,//enable
              256,//alphfa1
              128,//alphfa_red
              32,//k_filter_power
              9830,//b_value
              10,//ge_thres
              163,//g_slope
              60,//max_thres
              120,//k_thres
              50,//limit_base_green
              50,//limit_base_red
              1750,//limit_end_green
              2000,//limit_end_red
              256,//weight_filterg
              2048,//k_diff_base
           },
           /* para 2*/
           {
              0,//enable
              256,//alphfa1
              128,//alphfa_red
              72,//k_filter_power
              9830,//b_value
              10,//ge_thres
              163,//g_slope
              60,//max_thres
              150,//k_thres
              63,//limit_base_green
              63,//limit_base_red
              2188,//limit_end_green
              2500,//limit_end_red
              256,//weight_filterg
              2048,//k_diff_base
           },
           /* para 3*/
           {
              0,//enable
              256,//alphfa1
              128,//alphfa_red
              152,//k_filter_power
              9830,//b_value
              10,//ge_thres
              163,//g_slope
              60,//max_thres
              210,//k_thres
              88,//limit_base_green
              88,//limit_base_red
              3063,//limit_end_green
              3500,//limit_end_red
              256,//weight_filterg
              2048,//k_diff_base
           },
           /* para 4*/
           {
              0,//enable
              256,//alphfa1
              128,//alphfa_red
              232,//k_filter_power
              9830,//b_value
              10,//ge_thres
              163,//g_slope
              60,//max_thres
              270,//k_thres
              113,//limit_base_green
              113,//limit_base_red
              3938,//limit_end_green
              4500,//limit_end_red
              256,//weight_filterg
              2048,//k_diff_base
           },
       },
    },
    /* isp_sub_module_cfa_tuning_t isp_sub_module_cfa_tuning */
    {
       1,//enable
       1,//interpolation_enable
       4,//count_ae
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             160,
          },
          /* aec trigger 1 */
          {
             170,
             250,
          },
          /* aec trigger 2 */
          {
             350,
             380,
          },
          /* aec trigger 3 */
          {
             390,
             500,
          },
       },
       /* isp_sub_module_cfa_pra pra[MIDDLE_TRIGGER_COUNT] */
       {
          /* para 0 */
          {
              30,//edge_offset0
              30,//edge_offset1
              30,//edge_offset2
              30,//edge_offset3
              33,//gfilter_mid
              26,//gfilter_peak
              29,//gfilter_lrud
              75,//gfactor_1
              53,//gfactor_2
              37,//gfactor_3
              18,//gfactor_4
              500,//th1_y
              4000,//th2_y
              1000000,//th1_ny
              3000000,//th2_ny
              1,//ny_correct_en
              128,//ny_weight_factor
              0,//ny_weight_factor_1minus
              40,//ahd_th
              6,//hvwt_th
              128,//flat_hvwt_factor
              0,//flat_hvwt_factor_1minus
              64,//detail_same_factor
              64,//detail_same_factor_1minus
              128,//detail_diff_factor
              0,//detail_diff_factor_1minus
              64,//ny_same_factor
              64,//ny_same_factor_1minus
              115,//ny_diff_factor
              13,//ny_diff_factor_1minus
              1,//dpp_correct
              1,//luma_detect_en
              25,//th1_very_light
              100,//th2_y_max
              1,//rgb_diff_detect_en
              10,//th1_rgb_diff
              30,//th2_rgb_diff
              1,//edge_detect_en
              1000,//th1_edge
              6000,//th2_edge
              15,//dpp_strength
          },
          /* para 1 */
          {
              30,//edge_offset0
              30,//edge_offset1
              30,//edge_offset2
              30,//edge_offset3
              33,//gfilter_mid
              26,//gfilter_peak
              29,//gfilter_lrud
              75,//gfactor_1
              53,//gfactor_2
              37,//gfactor_3
              18,//gfactor_4
              500,//th1_y
              4000,//th2_y
              1000000,//th1_ny
              3000000,//th2_ny
              1,//ny_correct_en
              128,//ny_weight_factor
              0,//ny_weight_factor_1minus
              40,//ahd_th
              6,//hvwt_th
              128,//flat_hvwt_factor
              0,//flat_hvwt_factor_1minus
              64,//detail_same_factor
              64,//detail_same_factor_1minus
              128,//detail_diff_factor
              0,//detail_diff_factor_1minus
              64,//ny_same_factor
              64,//ny_same_factor_1minus
              115,//ny_diff_factor
              13,//ny_diff_factor_1minus
              1,//dpp_correct
              1,//luma_detect_en
              25,//th1_very_light
              100,//th2_y_max
              1,//rgb_diff_detect_en
              10,//th1_rgb_diff
              30,//th2_rgb_diff
              1,//edge_detect_en
              1000,//th1_edge
              6000,//th2_edge
              15,//dpp_strength
          },
          /* para 2 */
          {
              30,//edge_offset0
              30,//edge_offset1
              30,//edge_offset2
              30,//edge_offset3
              33,//gfilter_mid
              26,//gfilter_peak
              29,//gfilter_lrud
              75,//gfactor_1
              53,//gfactor_2
              37,//gfactor_3
              18,//gfactor_4
              500,//th1_y
              4000,//th2_y
              1000000,//th1_ny
              3000000,//th2_ny
              1,//ny_correct_en
              128,//ny_weight_factor
              0,//ny_weight_factor_1minus
              40,//ahd_th
              6,//hvwt_th
              128,//flat_hvwt_factor
              0,//flat_hvwt_factor_1minus
              64,//detail_same_factor
              64,//detail_same_factor_1minus
              128,//detail_diff_factor
              0,//detail_diff_factor_1minus
              64,//ny_same_factor
              64,//ny_same_factor_1minus
              115,//ny_diff_factor
              13,//ny_diff_factor_1minus
              1,//dpp_correct
              1,//luma_detect_en
              50,//th1_very_light
              255,//th2_y_max
              1,//rgb_diff_detect_en
              30,//th1_rgb_diff
              50,//th2_rgb_diff
              1,//edge_detect_en
              5000,//th1_edge
              10000,//th2_edge
              10,//dpp_strength
          },
          /* para 3 */
          {
              30,//edge_offset0
              30,//edge_offset1
              30,//edge_offset2
              30,//edge_offset3
              33,//gfilter_mid
              26,//gfilter_peak
              29,//gfilter_lrud
              75,//gfactor_1
              53,//gfactor_2
              37,//gfactor_3
              18,//gfactor_4
              500,//th1_y
              4000,//th2_y
              10000,//th1_ny
              900000,//th2_ny
              1,//ny_correct_en
              128,//ny_weight_factor
              0,//ny_weight_factor_1minus
              40,//ahd_th
              6,//hvwt_th
              128,//flat_hvwt_factor
              0,//flat_hvwt_factor_1minus
              64,//detail_same_factor
              64,//detail_same_factor_1minus
              128,//detail_diff_factor
              0,//detail_diff_factor_1minus
              64,//ny_same_factor
              64,//ny_same_factor_1minus
              115,//ny_diff_factor
              13,//ny_diff_factor_1minus
              1,//dpp_correct
              1,//luma_detect_en
              50,//th1_very_light
              254,//th2_y_max
              1,//rgb_diff_detect_en
              30,//th1_rgb_diff
              50,//th2_rgb_diff
              1,//edge_detect_en
              5000,//th1_edge
              10000,//th2_edge
              15,//dpp_strength
          },
       },
    },
    /* isp_sub_module_depurple_tuning_t isp_sub_module_depurple_tuning */
    {
       0,//enable
    },
    /* isp_sub_module_ccm1_tuning_t isp_sub_module_ccm1_tuning */
    {
       1,//enable
       1,//interpolation_enable
       2,//count_ae
       4,//count_awb
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             360,
          },
          /* aec trigger 1 */
          {
             400,
             510,
          },
       },
       /* awb trigger */
       {
          /* awb trigger 0 */
          {
             2500,
             2900,
          },
          /* awb trigger 1 */
          {
             3700,
             4300,
          },
          /* awb trigger 2 */
          {
             4700,
             5600,
          },
          /* awb trigger 3 */
          {
             6200,
             6800,
          },
       },
       /* isp_sub_module_ccm1_pra pra[5][7] */
       {
          /* aec trigger 0 */
          {
             /* awb para 0 */
             {
                1.7135,-0.8382,0.1247,
                -0.4394,1.507,-0.0676,
                -0.0435,-0.9672,2.0107,
             },
             /* awb para 1 */
             {
                1.4175,-0.7002,0.2827,
                -0.1885,1.5231,-0.3346,
                0.114,-0.6764,1.6624,
             },
             /* awb para 2 */
             {
                1.521,-0.1727,-0.3483,
                -0.1313,1.5343,-0.403,
                0.39,-0.99,1.6,
             },
             /* awb para 3 */
             {
                1.6937,-0.7399,0.0461,
                -0.1379,1.5585,-0.4206,
                0.1425,-0.9239,1.7814,
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                1.7135,-0.8382,0.1247,
                -0.4394,1.507,-0.0676,
                -0.0435,-0.9672,2.0107,
             },
             /* awb para 1 */
             {
                1.4175,-0.5002,0.0827,
                -0.4885,1.5231,-0.0346,
                0.114,-0.8764,1.7624,
             },
             /* awb para 2 */
             {
                1.721,-0.6727,-0.0483,
                -0.2313,1.2343,-0.003,
                0.1361,-0.8883,1.7522,
             },
             /* awb para 3 */
             {
                1.6937,-1.0399,0.3461,
                -0.1879,1.2585,-0.0706,
                0.1425,-0.9239,1.7814,
             },
          },
       },
    },
    /* isp_sub_module_ccm2_tuning_t isp_sub_module_ccm2_tuning */
    {
       0,//enable
    },
    /* isp_sub_module_gtm1_tuning_t isp_sub_module_gtm1_tuning */
    {
       0,//enable
       1,//interpolation_enable
       2,//count_ae
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             1,
             100,
          },
          /* aec trigger 1 */
          {
             180,
             370,
          },
       },
       /* isp_sub_module_gtm1_lut_pra pra[MIN_TRIGGER_COUNT] */
       {
          /* para 0 */
          {
             1,//grey_max_flag
             /* gtm1 curve */
             {
                  16,  32,  48,  64,  80,  96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 256, 272, 288, 304, 320,
                 336, 352, 368, 384, 400, 416, 432, 448, 464, 480, 496, 512, 528, 544, 560, 576, 592, 608, 624, 640,
                 656, 672, 688, 704, 720, 736, 752, 768, 784, 800, 816, 832, 848, 864, 880, 896, 912, 928, 944, 960,
                 976, 992,1008,1024,1040,1056,1072,1088,1104,1120,1136,1152,1168,1184,1200,1216,1232,1248,1264,1280,
                1296,1312,1328,1344,1360,1376,1392,1408,1424,1440,1456,1472,1488,1504,1520,1536,1552,1568,1584,1600,
                1616,1632,1648,1664,1680,1696,1712,1728,1744,1760,1776,1792,1808,1824,1840,1856,1872,1888,1904,1920,
                1936,1952,1968,1984,2000,2016,2032,2048,2064,2080,2096,2112,2128,2144,2160,2176,2192,2208,2224,2240,
                2256,2272,2288,2304,2320,2336,2352,2368,2384,2400,2416,2432,2448,2464,2480,2496,2512,2528,2544,2560,
                2576,2592,2608,2624,2640,2656,2672,2688,2704,2720,2736,2752,2768,2784,2800,2816,2832,2848,2864,2880,
                2896,2912,2928,2944,2960,2976,2992,3008,3024,3040,3056,3072,3088,3104,3120,3136,3152,3168,3184,3200,
                3216,3232,3248,3264,3280,3296,3312,3328,3344,3360,3376,3392,3408,3424,3440,3456,3472,3488,3504,3520,
                3536,3552,3568,3584,3600,3616,3632,3648,3664,3680,3696,3712,3728,3744,3760,3776,3792,3808,3824,3840,
                3856,3872,3888,3904,3920,3936,3952,3968,3984,4000,4016,4032,4048,4064,4080,4095,4095,
             },
          },
          /* para 1 */
          {
             1,//grey_max_flag
             /* gtm1 curve */
             {
                  13,  27,  40,  53,  66,  80,  93, 106, 119, 133, 146, 159, 172, 186, 199, 212, 226, 240, 255, 271,
                 289, 307, 324, 343, 362, 381, 400, 420, 439, 458, 477, 496, 515, 534, 553, 573, 592, 611, 630, 649,
                 668, 687, 706, 726, 745, 764, 783, 802, 821, 840, 860, 879, 898, 917, 936, 955, 974, 993,1013,1032,
                1051,1070,1089,1108,1127,1147,1166,1185,1204,1223,1242,1261,1280,1300,1319,1338,1357,1376,1395,1414,
                1434,1453,1472,1491,1510,1529,1548,1567,1587,1606,1625,1644,1663,1682,1701,1720,1740,1759,1778,1797,
                1816,1835,1854,1874,1893,1912,1931,1950,1969,1988,2007,2027,2046,2065,2084,2103,2122,2141,2161,2178,
                2197,2216,2234,2252,2271,2288,2308,2327,2346,2365,2384,2403,2422,2441,2458,2473,2490,2507,2525,2543,
                2561,2578,2592,2606,2619,2632,2645,2659,2673,2687,2701,2715,2728,2741,2755,2768,2783,2796,2809,2823,
                2836,2849,2862,2876,2889,2902,2915,2929,2942,2955,2968,2982,2995,3008,3021,3035,3048,3061,3075,3088,
                3101,3114,3128,3141,3154,3167,3181,3194,3207,3220,3234,3247,3260,3274,3287,3300,3313,3327,3340,3353,
                3366,3380,3393,3406,3419,3433,3446,3459,3473,3486,3499,3512,3526,3539,3552,3565,3579,3592,3605,3618,
                3632,3645,3658,3672,3685,3698,3711,3725,3738,3751,3764,3778,3791,3804,3817,3831,3844,3857,3870,3884,
                3897,3910,3924,3937,3950,3963,3977,3990,4003,4016,4030,4043,4056,4069,4083,4095,4095,
             },
          },
       },
    },
    /* isp_sub_module_gamma_lut_tuning_t isp_sub_module_gamma_lut_tuning */
    {
       1,//enable
       1,//interpolation_enable
       5,//count_ae
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             150,
          },
          /* aec trigger 1 */
          {
             160,
             180,
          },
          /* aec trigger 2 */
          {
             220,
             330,
          },
          /* aec trigger 3 */
          {
             360,
             460,
          },
          /* aec trigger 4 */
          {
             480,
             500,
          },
       },
       4096,//gamma_lut_size
       /* isp_sub_module_gamma_lut_pra pra[MIDDLE_TRIGGER_COUNT] */
       {
          /* aec trigger 0 */
          {
                   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   3,   5,   8,  11,  14,  18,  22,  26,  31,  36,  41,  46,  52,  58,  65,  72,  79,  86,  94,
                 102, 111, 119, 128, 137, 145, 154, 163, 171, 180, 189, 198, 207, 216, 225, 234, 243, 252, 261, 271, 280, 289, 299, 308, 317, 327, 336, 346, 355, 360, 366, 372,
                 378, 384, 391, 397, 403, 410, 417, 423, 430, 437, 444, 451, 458, 465, 472, 480, 487, 495, 502, 510, 518, 526, 534, 542, 550, 558, 565, 571, 578, 584, 591, 598,
                 604, 611, 618, 625, 631, 638, 645, 652, 659, 666, 672, 679, 686, 693, 700, 707, 714, 721, 728, 735, 743, 750, 757, 764, 769, 775, 781, 786, 792, 798, 804, 810,
                 816, 821, 827, 833, 838, 844, 849, 855, 860, 866, 871, 877, 882, 887, 892, 898, 903, 908, 913, 918, 923, 928, 933, 938, 942, 947, 952, 957, 961, 966, 971, 975,
                 980, 984, 989, 993, 997,1002,1006,1010,1015,1019,1023,1027,1031,1035,1039,1043,1047,1051,1055,1058,1062,1066,1069,1073,1077,1080,1084,1087,1091,1094,1098,1101,
                1104,1107,1111,1114,1117,1120,1123,1126,1129,1132,1135,1138,1141,1143,1146,1149,1151,1154,1157,1159,1162,1164,1167,1169,1171,1174,1176,1178,1181,1183,1186,1189,
                1191,1194,1196,1199,1202,1204,1207,1210,1212,1215,1217,1220,1223,1225,1228,1230,1233,1236,1238,1241,1243,1246,1248,1251,1254,1256,1259,1261,1264,1266,1269,1271,
                1274,1276,1279,1281,1284,1286,1289,1291,1294,1296,1299,1301,1304,1306,1308,1311,1313,1316,1318,1321,1323,1325,1328,1330,1333,1335,1337,1340,1342,1345,1347,1349,
                1352,1354,1356,1359,1361,1364,1366,1368,1370,1373,1375,1377,1380,1382,1384,1387,1389,1391,1394,1396,1398,1400,1403,1405,1407,1409,1412,1414,1416,1418,1421,1423,
                1425,1427,1429,1432,1434,1436,1438,1440,1443,1445,1447,1449,1451,1453,1456,1458,1460,1462,1464,1466,1468,1470,1473,1475,1477,1479,1481,1483,1485,1487,1489,1491,
                1494,1496,1498,1500,1502,1504,1506,1508,1510,1512,1514,1516,1518,1520,1522,1524,1526,1528,1530,1532,1534,1536,1538,1540,1542,1544,1546,1548,1550,1552,1553,1555,
                1557,1559,1561,1563,1565,1567,1569,1571,1573,1574,1576,1578,1580,1582,1584,1586,1588,1589,1591,1592,1594,1596,1597,1599,1601,1602,1604,1606,1607,1609,1610,1612,
                1614,1615,1617,1618,1620,1622,1623,1625,1627,1628,1630,1631,1633,1635,1636,1638,1639,1641,1643,1644,1646,1647,1649,1651,1652,1654,1655,1657,1659,1660,1662,1663,
                1665,1667,1668,1670,1671,1673,1675,1676,1678,1679,1681,1683,1684,1686,1687,1689,1690,1692,1694,1695,1697,1698,1700,1701,1703,1705,1706,1708,1709,1711,1712,1714,
                1716,1717,1719,1720,1722,1723,1725,1727,1728,1730,1731,1733,1734,1736,1737,1739,1741,1742,1744,1745,1747,1748,1750,1751,1753,1754,1756,1758,1759,1761,1762,1764,
                1765,1767,1768,1770,1771,1773,1774,1776,1778,1779,1780,1781,1782,1784,1785,1786,1787,1788,1790,1791,1792,1793,1794,1796,1797,1798,1799,1801,1802,1803,1804,1805,
                1806,1808,1809,1810,1811,1812,1814,1815,1816,1817,1818,1820,1821,1822,1823,1824,1826,1827,1828,1829,1830,1831,1833,1834,1835,1836,1837,1838,1840,1841,1842,1843,
                1844,1845,1847,1848,1849,1850,1851,1852,1854,1855,1856,1857,1858,1859,1860,1862,1863,1864,1865,1866,1867,1868,1870,1871,1872,1873,1874,1875,1876,1878,1879,1880,
                1881,1882,1883,1884,1886,1887,1888,1889,1890,1891,1892,1893,1895,1896,1897,1898,1899,1900,1901,1902,1903,1905,1906,1907,1908,1909,1910,1911,1912,1913,1914,1916,
                1917,1918,1919,1920,1921,1922,1923,1924,1925,1927,1928,1929,1930,1931,1932,1933,1934,1935,1936,1937,1939,1940,1941,1942,1943,1944,1945,1946,1947,1948,1949,1950,
                1951,1952,1954,1955,1956,1957,1958,1959,1960,1961,1962,1963,1964,1965,1966,1967,1968,1969,1970,1972,1973,1974,1975,1976,1977,1978,1979,1980,1981,1982,1983,1984,
                1985,1986,1987,1988,1989,1990,1991,1992,1993,1994,1995,1996,1997,1998,1999,2000,2002,2003,2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015,2016,2017,
                2018,2019,2020,2021,2022,2023,2024,2025,2026,2027,2028,2029,2030,2031,2032,2033,2034,2035,2036,2037,2038,2039,2040,2041,2042,2044,2045,2046,2047,2048,2049,2050,
                2051,2052,2053,2054,2055,2056,2057,2058,2059,2060,2061,2062,2063,2064,2065,2066,2067,2068,2069,2070,2071,2072,2073,2074,2075,2076,2077,2078,2079,2080,2081,2082,
                2083,2084,2085,2086,2087,2088,2089,2090,2091,2092,2093,2094,2095,2096,2097,2098,2099,2100,2101,2102,2103,2104,2105,2106,2107,2108,2109,2110,2111,2112,2113,2114,
                2115,2116,2117,2118,2119,2120,2121,2122,2123,2124,2125,2126,2127,2128,2129,2130,2131,2132,2133,2134,2135,2136,2137,2138,2138,2139,2140,2141,2142,2143,2144,2145,
                2146,2147,2148,2149,2150,2151,2152,2153,2154,2155,2156,2157,2158,2159,2160,2161,2162,2163,2164,2165,2166,2167,2168,2168,2169,2170,2171,2172,2173,2174,2175,2176,
                2177,2178,2179,2180,2181,2182,2183,2184,2185,2186,2187,2188,2188,2189,2190,2191,2192,2193,2194,2195,2196,2197,2198,2199,2200,2201,2202,2203,2204,2205,2205,2206,
                2207,2208,2209,2210,2211,2212,2213,2214,2215,2216,2217,2218,2219,2220,2220,2221,2222,2223,2224,2225,2226,2227,2228,2229,2230,2231,2232,2232,2233,2234,2235,2236,
                2237,2238,2239,2240,2241,2242,2243,2244,2244,2245,2246,2247,2248,2249,2250,2251,2252,2253,2254,2255,2255,2256,2257,2258,2259,2260,2261,2262,2263,2264,2265,2265,
                2266,2267,2268,2269,2270,2271,2272,2273,2274,2274,2275,2276,2277,2278,2279,2280,2281,2282,2283,2283,2284,2285,2286,2287,2288,2289,2290,2291,2291,2292,2293,2294,
                2295,2296,2297,2298,2299,2299,2300,2301,2302,2303,2304,2305,2306,2307,2307,2308,2309,2310,2311,2312,2313,2314,2314,2315,2316,2317,2318,2319,2320,2321,2321,2322,
                2323,2324,2325,2326,2327,2328,2328,2329,2330,2331,2332,2333,2334,2335,2335,2336,2337,2338,2339,2340,2341,2341,2342,2343,2344,2345,2346,2347,2347,2348,2349,2350,
                2351,2352,2353,2353,2354,2355,2356,2357,2358,2359,2359,2360,2361,2362,2363,2364,2364,2365,2366,2367,2368,2369,2370,2370,2371,2372,2373,2374,2375,2375,2376,2377,
                2378,2379,2380,2381,2381,2382,2383,2384,2385,2386,2386,2387,2388,2389,2390,2391,2391,2392,2393,2394,2395,2396,2396,2397,2398,2399,2400,2400,2401,2402,2403,2404,
                2405,2405,2406,2407,2408,2409,2410,2410,2411,2412,2413,2414,2414,2415,2416,2417,2418,2419,2419,2420,2421,2422,2423,2423,2424,2425,2426,2427,2427,2428,2429,2430,
                2431,2431,2432,2433,2434,2435,2435,2436,2437,2438,2439,2440,2440,2441,2442,2443,2443,2443,2444,2445,2446,2446,2447,2448,2449,2449,2450,2451,2452,2453,2453,2454,
                2455,2456,2456,2457,2458,2459,2460,2460,2461,2462,2463,2463,2464,2465,2466,2467,2467,2468,2469,2470,2470,2471,2472,2473,2473,2474,2475,2476,2477,2477,2478,2479,
                2480,2480,2481,2482,2483,2483,2484,2485,2486,2486,2487,2488,2489,2490,2490,2491,2492,2493,2493,2494,2495,2496,2496,2497,2498,2499,2499,2500,2501,2502,2502,2503,
                2504,2505,2506,2506,2507,2508,2509,2509,2510,2511,2512,2512,2513,2514,2515,2515,2516,2517,2518,2518,2519,2520,2521,2521,2522,2523,2524,2524,2525,2526,2527,2527,
                2528,2529,2530,2530,2531,2532,2533,2533,2534,2535,2536,2536,2537,2538,2539,2539,2540,2541,2542,2542,2543,2544,2545,2545,2546,2547,2548,2548,2549,2550,2551,2551,
                2552,2553,2554,2554,2555,2556,2557,2557,2558,2559,2560,2560,2561,2562,2563,2563,2564,2565,2566,2566,2567,2568,2568,2569,2570,2571,2571,2572,2573,2574,2574,2575,
                2576,2577,2577,2578,2579,2580,2580,2581,2582,2582,2583,2584,2585,2585,2586,2587,2588,2588,2589,2590,2591,2591,2592,2593,2593,2594,2595,2596,2596,2597,2598,2599,
                2599,2600,2601,2602,2602,2603,2604,2604,2605,2606,2607,2607,2608,2609,2610,2610,2611,2612,2612,2613,2614,2615,2615,2616,2617,2618,2618,2619,2620,2620,2621,2622,
                2623,2623,2624,2625,2625,2626,2627,2628,2628,2629,2630,2631,2631,2632,2633,2633,2634,2635,2636,2636,2637,2638,2638,2639,2640,2641,2641,2642,2643,2643,2644,2645,
                2646,2646,2647,2648,2648,2649,2650,2651,2651,2652,2653,2653,2654,2655,2656,2656,2657,2658,2658,2659,2660,2661,2661,2662,2663,2663,2664,2665,2665,2666,2667,2668,
                2668,2669,2670,2670,2671,2672,2673,2673,2674,2675,2675,2676,2677,2677,2678,2679,2680,2680,2681,2682,2682,2683,2684,2685,2685,2686,2687,2687,2688,2689,2689,2690,
                2691,2692,2692,2693,2694,2694,2695,2696,2696,2697,2698,2699,2699,2700,2701,2701,2702,2703,2703,2704,2705,2705,2706,2707,2708,2708,2709,2710,2710,2711,2712,2712,
                2713,2714,2715,2715,2716,2717,2717,2718,2719,2719,2720,2721,2721,2722,2723,2723,2724,2725,2726,2726,2727,2728,2728,2729,2730,2730,2731,2732,2732,2733,2734,2734,
                2735,2736,2737,2737,2738,2739,2739,2740,2741,2741,2742,2743,2743,2744,2745,2745,2746,2747,2747,2748,2749,2749,2750,2751,2752,2752,2753,2754,2754,2755,2756,2756,
                2757,2758,2758,2759,2760,2760,2761,2762,2762,2763,2764,2764,2765,2766,2766,2767,2768,2768,2769,2770,2770,2771,2772,2772,2773,2774,2774,2775,2776,2777,2777,2778,
                2779,2779,2780,2781,2781,2782,2783,2783,2784,2785,2785,2786,2787,2787,2788,2789,2789,2790,2791,2791,2792,2793,2793,2794,2795,2795,2796,2797,2797,2798,2799,2799,
                2800,2801,2801,2802,2802,2803,2804,2804,2805,2806,2806,2807,2808,2808,2809,2810,2810,2811,2812,2812,2813,2814,2814,2815,2816,2816,2817,2818,2818,2819,2820,2820,
                2821,2822,2822,2823,2824,2824,2825,2826,2826,2827,2827,2828,2829,2829,2830,2831,2831,2832,2833,2833,2834,2835,2835,2836,2837,2837,2838,2839,2839,2840,2840,2841,
                2842,2842,2843,2844,2844,2845,2846,2846,2847,2848,2848,2849,2850,2850,2851,2851,2852,2853,2853,2854,2855,2855,2856,2857,2857,2858,2859,2859,2860,2860,2861,2862,
                2862,2863,2864,2864,2865,2866,2866,2867,2867,2868,2869,2869,2870,2871,2871,2872,2873,2873,2874,2875,2875,2876,2877,2877,2878,2878,2879,2880,2880,2881,2882,2882,
                2883,2884,2884,2885,2886,2886,2887,2887,2888,2889,2889,2890,2891,2891,2892,2893,2893,2894,2894,2895,2896,2896,2897,2898,2898,2899,2900,2900,2901,2901,2902,2903,
                2903,2904,2905,2905,2906,2907,2907,2908,2908,2909,2910,2910,2911,2912,2912,2913,2913,2914,2915,2915,2916,2917,2917,2918,2919,2919,2920,2920,2921,2922,2922,2923,
                2924,2924,2925,2925,2926,2927,2927,2928,2929,2929,2930,2930,2931,2932,2932,2933,2934,2934,2935,2935,2936,2937,2937,2938,2939,2939,2940,2940,2941,2942,2942,2943,
                2944,2944,2945,2945,2946,2947,2947,2948,2949,2949,2950,2950,2951,2952,2952,2953,2953,2954,2955,2955,2956,2957,2957,2958,2958,2959,2960,2960,2961,2961,2962,2963,
                2963,2964,2965,2965,2966,2966,2967,2968,2968,2969,2969,2970,2971,2971,2972,2973,2973,2974,2974,2975,2976,2976,2977,2977,2978,2979,2979,2980,2980,2981,2982,2982,
                2983,2984,2984,2985,2985,2986,2987,2987,2988,2988,2989,2990,2990,2991,2991,2992,2993,2993,2994,2994,2995,2996,2996,2997,2997,2998,2999,2999,3000,3000,3001,3002,
                3002,3003,3003,3004,3005,3005,3006,3006,3007,3008,3008,3009,3009,3010,3011,3011,3012,3012,3013,3014,3014,3015,3015,3016,3017,3017,3018,3018,3019,3020,3020,3021,
                3021,3022,3023,3023,3024,3024,3025,3026,3026,3027,3027,3028,3029,3029,3030,3030,3031,3031,3032,3033,3033,3034,3034,3035,3036,3036,3037,3037,3038,3039,3039,3040,
                3040,3041,3042,3042,3043,3043,3044,3044,3045,3046,3046,3047,3047,3048,3049,3049,3050,3050,3051,3051,3052,3052,3053,3053,3054,3054,3055,3056,3056,3057,3057,3058,
                3058,3059,3060,3060,3061,3061,3062,3063,3063,3064,3064,3065,3065,3066,3067,3067,3068,3068,3069,3069,3070,3071,3071,3072,3072,3073,3073,3074,3075,3075,3076,3076,
                3077,3077,3078,3079,3079,3080,3080,3081,3081,3082,3083,3083,3084,3084,3085,3085,3086,3087,3087,3088,3088,3089,3089,3090,3091,3091,3092,3092,3093,3093,3094,3094,
                3095,3096,3096,3097,3097,3098,3098,3099,3100,3100,3101,3101,3102,3102,3103,3103,3104,3105,3105,3106,3106,3107,3107,3108,3108,3109,3110,3110,3111,3111,3112,3112,
                3113,3113,3114,3115,3115,3116,3116,3117,3117,3118,3118,3119,3120,3120,3121,3121,3122,3122,3123,3123,3124,3125,3125,3126,3126,3127,3127,3128,3128,3129,3129,3130,
                3131,3131,3132,3132,3133,3133,3134,3134,3135,3135,3136,3137,3137,3138,3138,3139,3139,3140,3140,3141,3141,3142,3142,3143,3144,3144,3145,3145,3146,3146,3147,3147,
                3148,3148,3149,3149,3150,3151,3151,3152,3152,3153,3153,3154,3154,3155,3155,3156,3156,3157,3158,3158,3159,3159,3160,3160,3161,3161,3162,3162,3163,3163,3164,3164,
                3165,3165,3166,3167,3167,3168,3168,3169,3169,3170,3170,3171,3171,3172,3172,3173,3173,3174,3174,3175,3175,3176,3177,3177,3178,3178,3179,3179,3180,3180,3181,3181,
                3182,3182,3183,3183,3184,3184,3185,3185,3186,3186,3187,3187,3188,3189,3189,3190,3190,3191,3191,3192,3192,3193,3193,3194,3194,3195,3195,3196,3196,3197,3197,3198,
                3198,3199,3199,3200,3200,3201,3201,3202,3202,3203,3203,3204,3204,3205,3205,3206,3206,3207,3208,3208,3209,3209,3210,3210,3211,3211,3212,3212,3213,3213,3214,3214,
                3215,3215,3216,3216,3217,3217,3218,3218,3219,3219,3220,3220,3221,3221,3222,3222,3223,3223,3224,3224,3225,3225,3226,3226,3227,3227,3228,3228,3229,3229,3230,3230,
                3231,3231,3232,3232,3233,3233,3234,3234,3235,3235,3236,3236,3237,3237,3238,3238,3239,3239,3240,3240,3241,3241,3241,3242,3242,3243,3243,3244,3244,3245,3245,3246,
                3246,3247,3247,3248,3248,3249,3249,3250,3250,3251,3251,3252,3252,3253,3253,3254,3254,3255,3255,3256,3256,3257,3257,3258,3258,3258,3259,3259,3260,3260,3261,3261,
                3262,3262,3263,3263,3264,3264,3265,3265,3266,3266,3267,3267,3268,3268,3269,3269,3270,3270,3270,3271,3271,3272,3272,3273,3273,3274,3274,3275,3275,3276,3276,3277,
                3277,3278,3278,3278,3279,3279,3280,3280,3281,3281,3282,3282,3283,3283,3284,3284,3285,3285,3286,3286,3286,3287,3287,3288,3288,3289,3289,3290,3290,3291,3291,3292,
                3292,3293,3293,3293,3294,3294,3295,3295,3296,3296,3297,3297,3298,3298,3298,3299,3299,3300,3300,3301,3301,3302,3302,3303,3303,3304,3304,3304,3305,3305,3306,3306,
                3307,3307,3308,3308,3309,3309,3309,3310,3310,3311,3311,3312,3312,3313,3313,3314,3314,3314,3315,3315,3316,3316,3317,3317,3318,3318,3318,3319,3319,3320,3320,3321,
                3321,3322,3322,3322,3323,3323,3324,3324,3325,3325,3326,3326,3326,3327,3327,3328,3328,3329,3329,3330,3330,3330,3331,3331,3332,3332,3333,3333,3334,3334,3334,3335,
                3335,3336,3336,3337,3337,3337,3338,3338,3339,3339,3340,3340,3341,3341,3341,3342,3342,3343,3343,3344,3344,3344,3345,3345,3346,3346,3347,3347,3347,3348,3348,3349,
                3349,3350,3350,3350,3351,3351,3352,3352,3353,3353,3353,3354,3354,3355,3355,3356,3356,3356,3357,3357,3358,3358,3359,3359,3359,3360,3360,3361,3361,3362,3362,3362,
                3363,3363,3364,3364,3365,3365,3365,3366,3366,3367,3367,3367,3368,3368,3369,3369,3370,3370,3370,3371,3371,3372,3372,3372,3373,3373,3374,3374,3375,3375,3375,3376,
                3376,3377,3377,3377,3378,3378,3379,3379,3379,3380,3380,3381,3381,3382,3382,3382,3383,3383,3384,3384,3384,3385,3385,3386,3386,3386,3387,3387,3388,3388,3388,3389,
                3389,3390,3390,3390,3391,3391,3392,3392,3393,3393,3393,3394,3394,3395,3395,3395,3396,3396,3397,3397,3397,3398,3398,3399,3399,3399,3400,3400,3401,3401,3401,3402,
                3402,3402,3403,3403,3404,3404,3404,3405,3405,3406,3406,3406,3407,3407,3408,3408,3408,3409,3409,3410,3410,3410,3411,3411,3412,3412,3412,3413,3413,3413,3414,3414,
                3415,3415,3415,3416,3416,3417,3417,3417,3418,3418,3419,3419,3419,3420,3420,3420,3421,3421,3422,3422,3422,3423,3423,3424,3424,3424,3425,3425,3425,3426,3426,3427,
                3427,3427,3428,3428,3428,3429,3429,3430,3430,3430,3431,3431,3431,3432,3432,3433,3433,3433,3434,3434,3435,3436,3436,3436,3437,3437,3438,3438,3438,3439,3439,3440,
                3440,3441,3441,3441,3442,3442,3443,3443,3444,3444,3444,3445,3445,3446,3446,3447,3447,3447,3448,3448,3449,3449,3449,3450,3450,3451,3451,3452,3452,3452,3453,3453,
                3454,3454,3455,3455,3455,3456,3456,3457,3457,3458,3458,3458,3459,3459,3460,3460,3461,3461,3461,3462,3462,3463,3463,3464,3464,3464,3465,3465,3466,3466,3466,3467,
                3467,3468,3468,3469,3469,3469,3470,3470,3471,3471,3472,3472,3472,3473,3473,3474,3474,3475,3475,3475,3476,3476,3477,3477,3478,3478,3478,3479,3479,3480,3480,3481,
                3481,3481,3482,3482,3483,3483,3484,3484,3484,3485,3485,3486,3486,3487,3487,3487,3488,3488,3489,3489,3490,3490,3490,3491,3491,3492,3492,3493,3493,3493,3494,3494,
                3495,3495,3496,3496,3496,3497,3497,3498,3498,3499,3499,3499,3500,3500,3501,3501,3502,3502,3502,3503,3503,3504,3504,3505,3505,3505,3506,3506,3507,3507,3508,3508,
                3508,3509,3509,3510,3510,3511,3511,3511,3512,3512,3513,3513,3514,3514,3514,3515,3515,3516,3516,3517,3517,3517,3518,3518,3519,3519,3520,3520,3520,3521,3521,3522,
                3522,3523,3523,3523,3524,3524,3525,3525,3526,3526,3526,3527,3527,3528,3528,3529,3529,3529,3530,3530,3531,3531,3532,3532,3532,3533,3533,3534,3534,3535,3535,3536,
                3536,3536,3537,3537,3538,3538,3539,3539,3539,3540,3540,3541,3541,3542,3542,3542,3543,3543,3544,3544,3545,3545,3545,3546,3546,3547,3547,3548,3548,3549,3549,3549,
                3550,3550,3551,3551,3552,3552,3552,3553,3553,3554,3554,3555,3555,3555,3556,3556,3557,3557,3558,3558,3558,3559,3559,3560,3560,3561,3561,3562,3562,3562,3563,3563,
                3564,3564,3565,3565,3565,3566,3566,3567,3567,3568,3568,3568,3569,3569,3570,3570,3571,3571,3572,3572,3572,3573,3573,3574,3574,3575,3575,3575,3576,3576,3577,3577,
                3578,3578,3579,3579,3579,3580,3580,3581,3581,3582,3582,3582,3583,3583,3584,3584,3585,3585,3585,3586,3586,3587,3587,3588,3588,3589,3589,3589,3590,3590,3591,3591,
                3592,3592,3592,3593,3593,3594,3594,3595,3595,3596,3596,3596,3597,3597,3598,3598,3599,3599,3599,3600,3600,3601,3601,3602,3602,3603,3603,3603,3604,3604,3605,3605,
                3606,3606,3607,3607,3607,3608,3608,3609,3609,3610,3610,3610,3611,3611,3612,3612,3613,3613,3614,3614,3614,3615,3615,3616,3616,3617,3617,3617,3618,3618,3619,3619,
                3620,3620,3621,3621,3621,3622,3622,3623,3623,3624,3624,3625,3625,3625,3626,3626,3627,3627,3628,3628,3629,3629,3629,3630,3630,3631,3631,3632,3632,3632,3633,3633,
                3634,3634,3635,3635,3636,3636,3636,3637,3637,3638,3638,3639,3639,3640,3640,3640,3641,3641,3642,3642,3643,3643,3644,3644,3644,3645,3645,3646,3646,3647,3647,3648,
                3648,3648,3649,3649,3650,3650,3651,3651,3652,3653,3653,3654,3654,3655,3655,3656,3656,3657,3657,3657,3658,3658,3659,3659,3660,3660,3661,3661,3662,3662,3662,3663,
                3663,3664,3664,3665,3665,3666,3666,3667,3667,3668,3668,3668,3669,3669,3670,3670,3671,3671,3672,3672,3673,3673,3674,3674,3674,3675,3675,3676,3676,3677,3677,3678,
                3678,3679,3679,3680,3680,3681,3681,3681,3682,3682,3683,3683,3684,3684,3685,3685,3686,3686,3687,3687,3688,3688,3689,3689,3689,3690,3690,3691,3691,3692,3692,3693,
                3693,3694,3694,3695,3695,3696,3696,3697,3697,3698,3698,3698,3699,3699,3700,3700,3701,3701,3702,3702,3703,3703,3704,3704,3705,3705,3706,3706,3707,3707,3708,3708,
                3709,3709,3710,3710,3711,3711,3712,3712,3712,3713,3713,3714,3714,3715,3715,3716,3716,3717,3717,3718,3718,3719,3719,3720,3720,3721,3721,3722,3722,3723,3723,3724,
                3724,3725,3725,3726,3726,3727,3727,3728,3728,3729,3729,3730,3730,3731,3731,3732,3732,3733,3733,3734,3734,3735,3735,3736,3736,3737,3737,3738,3738,3739,3739,3740,
                3740,3741,3741,3742,3742,3743,3743,3744,3744,3745,3745,3746,3746,3747,3747,3748,3748,3749,3749,3750,3750,3751,3751,3752,3752,3753,3753,3754,3754,3755,3755,3756,
                3756,3757,3757,3758,3758,3759,3759,3760,3760,3761,3761,3762,3762,3763,3763,3764,3764,3765,3765,3766,3767,3767,3768,3768,3769,3769,3770,3770,3771,3771,3772,3772,
                3773,3773,3774,3774,3775,3775,3776,3776,3777,3777,3778,3778,3779,3779,3780,3781,3781,3782,3782,3783,3783,3784,3784,3785,3785,3786,3786,3787,3787,3788,3788,3789,
                3789,3790,3790,3791,3792,3792,3793,3793,3794,3794,3795,3795,3796,3796,3797,3797,3798,3798,3799,3799,3800,3801,3801,3802,3802,3803,3803,3804,3804,3805,3805,3806,
                3806,3807,3807,3808,3809,3809,3810,3810,3811,3811,3812,3812,3813,3813,3814,3814,3815,3816,3816,3817,3817,3818,3818,3819,3819,3820,3820,3821,3821,3822,3823,3823,
                3824,3824,3825,3825,3826,3826,3827,3827,3828,3829,3829,3830,3830,3831,3831,3832,3832,3833,3833,3834,3835,3835,3836,3836,3837,3837,3838,3838,3839,3840,3840,3841,
                3841,3842,3842,3843,3843,3844,3844,3845,3846,3846,3847,3847,3848,3848,3849,3849,3850,3851,3851,3852,3852,3853,3853,3854,3855,3856,3856,3857,3858,3859,3859,3860,
                3861,3861,3862,3863,3863,3864,3865,3865,3866,3867,3868,3868,3869,3870,3870,3871,3872,3873,3873,3874,3875,3875,3876,3877,3877,3878,3879,3880,3880,3881,3882,3882,
                3883,3884,3885,3885,3886,3887,3888,3888,3889,3890,3890,3891,3892,3893,3893,3894,3895,3896,3896,3897,3898,3898,3899,3900,3901,3901,3902,3903,3904,3904,3905,3906,
                3907,3907,3908,3909,3910,3910,3911,3912,3913,3913,3914,3915,3916,3916,3917,3918,3919,3919,3920,3921,3922,3922,3923,3924,3925,3925,3926,3927,3928,3929,3929,3930,
                3931,3932,3932,3933,3934,3935,3935,3936,3937,3938,3939,3939,3940,3941,3942,3942,3943,3944,3945,3946,3946,3947,3948,3949,3950,3950,3951,3952,3953,3953,3954,3955,
                3956,3957,3957,3958,3959,3960,3961,3961,3962,3963,3964,3965,3965,3966,3967,3968,3969,3970,3970,3971,3972,3973,3974,3974,3975,3976,3977,3978,3978,3979,3980,3981,
                3982,3983,3983,3984,3985,3986,3987,3988,3988,3989,3990,3991,3992,3992,3993,3994,3995,3996,3997,3997,3998,3999,4000,4001,4002,4003,4003,4004,4005,4006,4007,4008,
                4008,4009,4010,4011,4012,4013,4014,4014,4015,4016,4017,4018,4019,4020,4020,4021,4022,4023,4024,4025,4026,4026,4027,4028,4029,4030,4031,4032,4032,4033,4034,4035,
                4036,4037,4038,4039,4039,4040,4041,4042,4043,4044,4045,4046,4046,4047,4048,4049,4050,4051,4052,4053,4054,4054,4055,4056,4057,4058,4059,4060,4061,4062,4062,4063,
                4064,4065,4066,4067,4068,4069,4070,4071,4071,4072,4073,4074,4075,4076,4077,4078,4079,4080,4081,4081,4082,4083,4084,4085,4086,4087,4088,4089,4090,4091,4092,4093,
          },
          /* aec trigger 1 */
          {
                   0,  14,  29,  43,  57,  71,  84,  97, 110, 123, 135, 147, 159, 170, 181, 192, 203, 213, 223, 233, 242, 251, 260, 269, 277, 285, 293, 300, 307, 314, 319, 324,
                 330, 335, 341, 346, 352, 357, 362, 368, 373, 379, 384, 389, 395, 400, 405, 411, 416, 421, 427, 432, 437, 442, 448, 453, 458, 463, 468, 473, 479, 484, 489, 494,
                 499, 504, 509, 514, 519, 524, 529, 534, 539, 544, 549, 554, 559, 564, 569, 574, 579, 583, 588, 593, 598, 603, 607, 612, 617, 622, 626, 632, 638, 644, 649, 655,
                 660, 666, 671, 676, 681, 687, 692, 697, 702, 707, 712, 716, 721, 726, 730, 735, 740, 744, 748, 753, 757, 761, 765, 769, 772, 777, 782, 787, 792, 796, 801, 806,
                 810, 815, 819, 824, 828, 833, 837, 842, 846, 850, 855, 859, 863, 867, 871, 875, 879, 883, 887, 891, 895, 899, 903, 907, 911, 914, 918, 922, 925, 929, 933, 936,
                 940, 944, 948, 952, 957, 961, 965, 969, 973, 978, 982, 986, 990, 994, 998,1002,1006,1010,1014,1017,1021,1025,1029,1033,1036,1040,1044,1048,1051,1055,1059,1062,
                1066,1069,1073,1076,1080,1083,1086,1090,1093,1097,1100,1103,1106,1110,1113,1116,1119,1122,1126,1129,1132,1135,1138,1141,1144,1147,1150,1153,1154,1158,1161,1164,
                1167,1170,1173,1175,1178,1181,1184,1187,1190,1193,1196,1199,1202,1204,1207,1210,1213,1216,1218,1221,1224,1227,1229,1232,1235,1238,1240,1243,1246,1248,1251,1254,
                1256,1259,1261,1264,1266,1269,1272,1274,1277,1279,1282,1284,1287,1289,1291,1294,1296,1299,1301,1304,1306,1308,1311,1313,1315,1318,1320,1322,1325,1327,1329,1331,
                1334,1336,1338,1340,1342,1345,1347,1349,1351,1353,1355,1357,1357,1358,1360,1362,1364,1366,1368,1370,1372,1374,1376,1377,1379,1381,1383,1385,1387,1389,1391,1393,
                1394,1396,1398,1400,1402,1404,1406,1407,1409,1411,1413,1415,1417,1418,1420,1422,1424,1426,1427,1429,1431,1433,1435,1437,1438,1440,1442,1444,1445,1447,1449,1451,
                1453,1454,1456,1458,1460,1461,1463,1465,1467,1468,1470,1472,1474,1475,1477,1479,1481,1482,1484,1486,1488,1489,1491,1493,1494,1496,1498,1499,1501,1503,1505,1506,
                1508,1510,1511,1513,1515,1516,1518,1520,1521,1523,1525,1526,1528,1530,1531,1533,1534,1536,1538,1539,1541,1543,1544,1546,1548,1549,1551,1552,1554,1556,1557,1559,
                1560,1562,1564,1565,1567,1568,1570,1571,1573,1575,1576,1578,1579,1581,1582,1584,1585,1587,1589,1590,1592,1593,1595,1596,1598,1599,1601,1602,1604,1605,1607,1608,
                1610,1611,1613,1614,1616,1617,1619,1620,1622,1623,1625,1626,1628,1629,1631,1632,1633,1635,1636,1638,1639,1641,1642,1644,1645,1646,1648,1649,1651,1652,1654,1655,
                1656,1658,1659,1661,1662,1663,1665,1666,1668,1669,1670,1672,1673,1674,1676,1677,1679,1680,1681,1683,1684,1685,1687,1688,1689,1691,1692,1693,1695,1696,1697,1699,
                1700,1701,1703,1704,1705,1707,1708,1709,1711,1713,1714,1715,1717,1718,1720,1721,1723,1724,1726,1727,1729,1730,1732,1733,1735,1736,1738,1739,1740,1742,1743,1745,
                1746,1748,1749,1750,1752,1753,1755,1756,1757,1759,1760,1762,1763,1764,1766,1767,1769,1770,1771,1773,1774,1775,1777,1778,1780,1781,1782,1784,1785,1786,1788,1789,
                1790,1792,1793,1794,1796,1797,1798,1800,1801,1802,1803,1805,1806,1807,1809,1810,1811,1813,1814,1815,1816,1818,1819,1820,1821,1823,1824,1825,1826,1828,1829,1830,
                1831,1833,1834,1835,1836,1838,1839,1840,1841,1842,1844,1845,1846,1847,1848,1850,1851,1852,1853,1854,1856,1857,1858,1859,1860,1861,1863,1864,1865,1866,1867,1868,
                1870,1871,1872,1873,1874,1875,1877,1878,1879,1880,1881,1882,1884,1885,1886,1887,1888,1890,1891,1892,1893,1894,1895,1897,1898,1899,1900,1901,1903,1904,1905,1906,
                1907,1908,1910,1911,1912,1913,1914,1915,1917,1918,1919,1920,1921,1923,1924,1925,1926,1927,1928,1930,1931,1932,1933,1934,1936,1937,1938,1939,1940,1942,1943,1944,
                1945,1946,1947,1949,1950,1951,1952,1953,1955,1956,1957,1958,1959,1960,1962,1963,1964,1965,1966,1968,1969,1970,1971,1972,1974,1975,1976,1977,1978,1979,1981,1982,
                1983,1984,1985,1987,1988,1989,1990,1991,1993,1994,1995,1996,1997,1998,2000,2001,2002,2003,2004,2006,2007,2008,2009,2010,2012,2013,2014,2015,2016,2017,2018,2019,
                2020,2021,2023,2024,2025,2026,2027,2028,2029,2030,2031,2033,2034,2035,2036,2037,2038,2039,2040,2041,2042,2044,2045,2046,2047,2048,2049,2050,2051,2052,2053,2055,
                2056,2057,2058,2059,2060,2061,2062,2063,2064,2065,2067,2068,2069,2070,2071,2072,2073,2074,2075,2076,2077,2079,2080,2081,2082,2083,2084,2085,2086,2087,2088,2089,
                2090,2092,2093,2094,2095,2096,2097,2098,2099,2100,2101,2102,2103,2105,2106,2107,2108,2109,2110,2111,2112,2113,2114,2115,2116,2117,2119,2120,2121,2122,2123,2124,
                2125,2126,2127,2128,2129,2130,2131,2132,2133,2135,2136,2137,2138,2139,2140,2141,2142,2143,2144,2145,2146,2147,2148,2149,2151,2152,2153,2154,2155,2156,2157,2158,
                2159,2160,2161,2162,2163,2163,2164,2165,2166,2167,2168,2169,2170,2171,2172,2173,2174,2175,2176,2177,2178,2179,2180,2181,2182,2183,2184,2185,2186,2187,2188,2189,
                2190,2191,2192,2193,2194,2195,2196,2197,2198,2199,2200,2201,2202,2203,2204,2205,2206,2207,2208,2209,2209,2210,2211,2212,2213,2214,2215,2216,2217,2218,2219,2220,
                2221,2222,2223,2224,2225,2226,2227,2228,2229,2230,2231,2232,2233,2234,2235,2236,2237,2238,2239,2239,2240,2241,2242,2243,2244,2245,2246,2247,2248,2249,2250,2251,
                2252,2253,2254,2255,2256,2257,2258,2259,2260,2261,2262,2262,2263,2264,2265,2266,2267,2268,2269,2270,2271,2272,2273,2274,2275,2276,2277,2278,2279,2280,2281,2281,
                2282,2283,2284,2285,2286,2287,2288,2289,2290,2291,2292,2293,2294,2295,2296,2297,2298,2299,2299,2300,2301,2302,2303,2304,2305,2306,2307,2308,2309,2310,2311,2312,
                2313,2314,2314,2315,2316,2317,2318,2319,2320,2321,2322,2323,2324,2325,2326,2327,2328,2329,2329,2330,2331,2332,2333,2334,2335,2336,2337,2338,2339,2340,2341,2342,
                2342,2343,2344,2345,2346,2347,2348,2349,2350,2351,2352,2353,2354,2355,2355,2356,2357,2358,2359,2360,2361,2362,2363,2364,2365,2366,2366,2367,2368,2369,2370,2371,
                2372,2373,2374,2375,2376,2377,2378,2378,2379,2380,2381,2382,2383,2384,2385,2386,2387,2388,2388,2389,2390,2391,2392,2393,2394,2395,2396,2397,2398,2399,2399,2400,
                2401,2402,2403,2404,2405,2406,2407,2408,2408,2409,2410,2411,2412,2413,2414,2415,2416,2417,2418,2418,2419,2420,2421,2422,2423,2424,2425,2426,2427,2427,2428,2429,
                2430,2431,2432,2433,2434,2435,2436,2436,2437,2438,2439,2440,2441,2442,2443,2444,2444,2445,2446,2447,2448,2449,2450,2451,2452,2452,2453,2454,2455,2456,2457,2458,
                2459,2460,2461,2461,2462,2463,2464,2465,2466,2467,2468,2468,2469,2470,2471,2472,2473,2474,2475,2476,2476,2477,2478,2479,2480,2481,2482,2483,2483,2484,2485,2486,
                2487,2488,2489,2490,2491,2491,2492,2493,2494,2495,2496,2497,2498,2498,2499,2500,2501,2502,2503,2504,2505,2505,2506,2507,2508,2509,2510,2511,2511,2512,2513,2514,
                2515,2516,2517,2518,2518,2519,2520,2521,2522,2523,2524,2524,2525,2526,2527,2528,2529,2530,2531,2531,2532,2533,2534,2535,2536,2537,2537,2538,2539,2540,2541,2542,
                2543,2543,2544,2545,2546,2547,2548,2549,2549,2550,2551,2552,2553,2554,2555,2555,2556,2557,2558,2559,2560,2561,2561,2562,2563,2564,2565,2566,2566,2567,2568,2569,
                2570,2571,2572,2572,2573,2574,2575,2576,2577,2578,2578,2579,2580,2581,2582,2583,2583,2584,2585,2586,2587,2588,2588,2589,2590,2591,2592,2593,2594,2594,2595,2596,
                2597,2598,2599,2599,2600,2601,2602,2603,2604,2604,2605,2606,2607,2608,2609,2609,2610,2611,2612,2613,2614,2614,2615,2616,2617,2618,2619,2619,2620,2621,2622,2623,
                2624,2624,2625,2626,2627,2628,2629,2629,2630,2631,2632,2633,2633,2634,2635,2636,2637,2638,2638,2639,2640,2641,2642,2643,2643,2644,2645,2646,2647,2647,2648,2649,
                2650,2651,2652,2652,2653,2654,2655,2656,2656,2657,2658,2659,2660,2661,2661,2662,2663,2664,2665,2665,2666,2667,2668,2669,2669,2670,2671,2672,2673,2674,2674,2675,
                2676,2677,2678,2678,2679,2680,2681,2682,2682,2683,2684,2685,2686,2686,2687,2688,2689,2690,2690,2691,2692,2693,2694,2694,2695,2696,2697,2698,2699,2700,2701,2702,
                2703,2703,2704,2705,2706,2706,2707,2708,2709,2709,2710,2711,2712,2713,2713,2714,2715,2716,2716,2717,2718,2719,2719,2720,2721,2722,2722,2723,2724,2725,2726,2726,
                2727,2728,2729,2729,2730,2731,2732,2732,2733,2734,2735,2735,2736,2737,2738,2738,2739,2740,2741,2741,2742,2743,2744,2744,2745,2746,2747,2747,2748,2749,2750,2750,
                2751,2752,2753,2753,2754,2755,2756,2756,2757,2758,2759,2759,2760,2761,2762,2762,2763,2764,2764,2765,2766,2767,2767,2768,2769,2770,2770,2771,2772,2773,2773,2774,
                2775,2776,2776,2777,2778,2778,2779,2780,2781,2781,2782,2783,2784,2784,2785,2786,2787,2787,2788,2789,2789,2790,2791,2792,2792,2793,2794,2795,2795,2796,2797,2797,
                2798,2799,2800,2800,2801,2802,2803,2803,2804,2805,2805,2806,2807,2808,2808,2809,2810,2810,2811,2812,2813,2813,2814,2815,2815,2816,2817,2818,2818,2819,2820,2820,
                2821,2822,2823,2823,2824,2825,2825,2826,2827,2828,2828,2829,2830,2830,2831,2832,2833,2833,2834,2835,2835,2836,2837,2837,2838,2839,2840,2840,2841,2842,2842,2843,
                2844,2845,2845,2846,2847,2847,2848,2849,2849,2850,2851,2852,2852,2853,2854,2854,2855,2856,2856,2857,2858,2858,2859,2860,2861,2861,2862,2863,2863,2864,2865,2865,
                2866,2867,2867,2868,2869,2870,2870,2871,2872,2872,2873,2874,2874,2875,2876,2876,2877,2878,2878,2879,2880,2881,2881,2882,2883,2883,2884,2885,2885,2886,2887,2887,
                2888,2889,2889,2890,2891,2891,2892,2893,2893,2894,2895,2895,2896,2897,2897,2898,2899,2899,2900,2901,2902,2902,2903,2904,2904,2905,2906,2906,2907,2908,2908,2909,
                2910,2910,2911,2912,2912,2913,2914,2914,2915,2916,2916,2917,2918,2918,2919,2920,2920,2921,2922,2922,2923,2924,2924,2925,2925,2926,2927,2927,2928,2929,2929,2930,
                2931,2931,2932,2933,2933,2934,2935,2935,2936,2937,2937,2938,2939,2939,2940,2941,2941,2942,2943,2943,2944,2944,2945,2946,2946,2947,2948,2948,2949,2950,2950,2951,
                2952,2952,2953,2954,2954,2955,2955,2956,2957,2957,2958,2959,2959,2960,2961,2961,2962,2962,2963,2964,2964,2965,2966,2966,2967,2968,2968,2969,2969,2970,2971,2971,
                2972,2973,2973,2974,2975,2975,2976,2976,2977,2978,2978,2979,2980,2980,2981,2982,2982,2983,2983,2984,2985,2985,2986,2987,2987,2988,2988,2989,2990,2990,2991,2992,
                2992,2993,2993,2994,2995,2995,2996,2996,2997,2998,2998,2999,3000,3000,3001,3001,3002,3003,3003,3004,3005,3005,3006,3006,3007,3008,3008,3009,3009,3010,3011,3011,
                3012,3012,3013,3014,3014,3015,3016,3016,3017,3017,3018,3019,3019,3020,3020,3021,3022,3022,3023,3023,3024,3025,3025,3026,3026,3027,3028,3028,3029,3029,3030,3031,
                3031,3032,3032,3033,3034,3034,3035,3035,3036,3037,3037,3038,3038,3039,3040,3040,3041,3041,3042,3043,3043,3044,3044,3045,3046,3046,3047,3047,3048,3049,3049,3050,
                3050,3051,3051,3052,3053,3053,3054,3054,3055,3056,3056,3057,3057,3058,3058,3059,3060,3060,3061,3061,3062,3063,3063,3064,3064,3065,3065,3066,3067,3067,3068,3068,
                3069,3070,3070,3071,3071,3072,3072,3073,3074,3074,3075,3075,3076,3076,3077,3078,3078,3079,3079,3080,3080,3081,3082,3082,3083,3083,3084,3084,3085,3086,3086,3087,
                3087,3088,3088,3089,3090,3090,3091,3091,3092,3092,3093,3093,3094,3095,3095,3096,3096,3097,3097,3098,3099,3100,3101,3101,3102,3103,3103,3104,3104,3105,3106,3106,
                3107,3108,3108,3109,3109,3110,3111,3111,3112,3113,3113,3114,3114,3115,3116,3116,3117,3118,3118,3119,3119,3120,3121,3121,3122,3122,3123,3124,3124,3125,3126,3126,
                3127,3127,3128,3129,3129,3130,3130,3131,3132,3132,3133,3134,3134,3135,3135,3136,3137,3137,3138,3138,3139,3140,3140,3141,3141,3142,3143,3143,3144,3144,3145,3146,
                3146,3147,3147,3148,3149,3149,3150,3150,3151,3152,3152,3153,3153,3154,3155,3155,3156,3156,3157,3158,3158,3159,3159,3160,3161,3161,3162,3162,3163,3164,3164,3165,
                3165,3166,3167,3167,3168,3168,3169,3170,3170,3171,3171,3172,3172,3173,3174,3174,3175,3175,3176,3177,3177,3178,3178,3179,3180,3180,3181,3181,3182,3182,3183,3184,
                3184,3185,3185,3186,3187,3187,3188,3188,3189,3189,3190,3191,3191,3192,3192,3193,3194,3194,3195,3195,3196,3196,3197,3198,3198,3199,3199,3200,3200,3201,3202,3202,
                3203,3203,3204,3204,3205,3206,3206,3207,3207,3208,3208,3209,3210,3210,3211,3211,3212,3212,3213,3214,3214,3215,3215,3216,3216,3217,3218,3218,3219,3219,3220,3220,
                3221,3222,3222,3223,3223,3224,3224,3225,3225,3226,3227,3227,3228,3228,3229,3229,3230,3230,3231,3232,3232,3233,3233,3234,3234,3235,3235,3236,3237,3237,3238,3238,
                3239,3239,3240,3240,3241,3242,3242,3243,3243,3244,3244,3245,3245,3246,3247,3247,3248,3248,3249,3249,3250,3250,3251,3251,3252,3253,3253,3254,3254,3255,3255,3256,
                3256,3257,3257,3258,3259,3259,3260,3260,3261,3261,3262,3262,3263,3263,3264,3265,3265,3266,3266,3267,3267,3268,3268,3269,3269,3270,3270,3271,3272,3272,3273,3273,
                3274,3274,3275,3275,3276,3276,3277,3277,3278,3278,3279,3279,3280,3281,3281,3282,3282,3283,3283,3284,3284,3285,3285,3286,3286,3287,3287,3288,3288,3289,3290,3290,
                3291,3291,3292,3292,3293,3293,3294,3294,3295,3295,3296,3296,3297,3297,3298,3298,3299,3299,3300,3300,3301,3302,3302,3303,3303,3304,3304,3305,3305,3306,3306,3307,
                3307,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,3313,3313,3314,3314,3315,3315,3316,3316,3317,3317,3318,3318,3319,3319,3320,3320,3321,3321,3322,3322,3323,
                3323,3324,3324,3325,3326,3327,3328,3328,3329,3329,3330,3330,3331,3332,3332,3333,3333,3334,3334,3335,3336,3336,3337,3337,3338,3338,3339,3339,3340,3341,3341,3342,
                3342,3343,3343,3344,3344,3345,3346,3346,3347,3347,3348,3348,3349,3349,3350,3351,3351,3352,3352,3353,3353,3354,3354,3355,3355,3356,3357,3357,3358,3358,3359,3359,
                3360,3360,3361,3362,3362,3363,3363,3364,3364,3365,3365,3366,3366,3367,3367,3368,3369,3369,3370,3370,3371,3371,3372,3372,3373,3373,3374,3374,3375,3376,3376,3377,
                3377,3378,3378,3379,3379,3380,3380,3381,3381,3382,3382,3383,3384,3384,3385,3385,3386,3386,3387,3387,3388,3388,3389,3389,3390,3390,3391,3391,3392,3392,3393,3394,
                3394,3395,3395,3396,3396,3397,3397,3398,3398,3399,3399,3400,3400,3401,3401,3402,3402,3403,3403,3404,3404,3405,3405,3406,3406,3407,3408,3408,3409,3409,3410,3410,
                3411,3411,3412,3412,3413,3413,3414,3414,3415,3415,3416,3416,3417,3417,3418,3418,3419,3419,3420,3420,3421,3421,3422,3422,3423,3423,3424,3424,3425,3425,3426,3426,
                3427,3427,3428,3428,3429,3429,3430,3430,3431,3431,3432,3432,3433,3433,3434,3434,3435,3435,3436,3436,3437,3437,3438,3438,3439,3439,3440,3440,3441,3441,3442,3442,
                3443,3443,3444,3444,3445,3445,3446,3446,3446,3447,3447,3448,3448,3449,3449,3450,3450,3451,3451,3452,3452,3453,3453,3454,3454,3455,3455,3456,3456,3457,3457,3458,
                3458,3459,3459,3459,3460,3460,3461,3461,3462,3462,3463,3463,3464,3464,3465,3465,3466,3466,3467,3467,3468,3468,3468,3469,3469,3470,3470,3471,3471,3472,3472,3473,
                3473,3474,3474,3475,3475,3475,3476,3476,3477,3477,3478,3478,3479,3479,3480,3480,3481,3481,3481,3482,3482,3483,3483,3484,3484,3485,3485,3486,3486,3486,3487,3487,
                3488,3488,3489,3489,3490,3490,3491,3491,3491,3492,3492,3493,3493,3494,3494,3495,3495,3496,3496,3496,3497,3497,3498,3498,3499,3499,3500,3500,3500,3501,3501,3502,
                3502,3503,3503,3504,3504,3504,3505,3505,3506,3506,3507,3507,3508,3508,3508,3509,3509,3510,3510,3511,3511,3511,3512,3512,3513,3513,3514,3514,3514,3515,3515,3516,
                3516,3517,3517,3518,3518,3518,3519,3519,3520,3520,3521,3521,3521,3522,3522,3523,3523,3523,3524,3524,3524,3524,3524,3525,3525,3526,3526,3527,3527,3528,3528,3529,
                3529,3530,3530,3531,3531,3532,3532,3533,3533,3534,3534,3535,3535,3536,3536,3537,3537,3538,3538,3538,3539,3539,3540,3540,3541,3541,3542,3542,3543,3543,3544,3544,
                3545,3545,3546,3546,3547,3547,3548,3548,3549,3549,3550,3550,3551,3551,3551,3552,3552,3553,3553,3554,3554,3555,3555,3556,3556,3557,3557,3558,3558,3559,3559,3560,
                3560,3561,3561,3562,3562,3563,3563,3563,3564,3564,3565,3565,3566,3566,3567,3567,3568,3568,3569,3569,3570,3570,3571,3571,3572,3572,3573,3573,3573,3574,3574,3575,
                3575,3576,3576,3577,3577,3578,3578,3579,3579,3580,3580,3581,3581,3582,3582,3583,3583,3583,3584,3584,3585,3585,3586,3586,3587,3587,3588,3588,3589,3589,3590,3590,
                3591,3591,3592,3592,3592,3593,3593,3594,3594,3595,3595,3596,3596,3597,3597,3598,3598,3599,3599,3600,3600,3601,3601,3601,3602,3602,3603,3603,3604,3604,3605,3605,
                3606,3606,3607,3607,3608,3608,3609,3609,3609,3610,3610,3611,3611,3612,3612,3613,3613,3614,3614,3615,3615,3616,3616,3617,3617,3617,3618,3618,3619,3619,3620,3620,
                3621,3621,3622,3622,3623,3623,3624,3624,3624,3625,3625,3626,3626,3627,3627,3628,3628,3629,3629,3630,3630,3631,3631,3632,3632,3632,3633,3633,3634,3634,3635,3635,
                3636,3636,3637,3637,3638,3638,3639,3639,3639,3640,3640,3641,3641,3642,3642,3643,3643,3644,3644,3645,3645,3645,3646,3646,3647,3647,3648,3648,3649,3649,3650,3650,
                3651,3651,3652,3652,3652,3653,3653,3654,3654,3655,3655,3656,3656,3657,3657,3658,3658,3658,3659,3659,3660,3660,3661,3661,3662,3662,3663,3663,3664,3664,3664,3665,
                3665,3666,3666,3667,3667,3668,3668,3669,3669,3670,3670,3670,3671,3671,3672,3672,3673,3673,3674,3674,3675,3675,3675,3676,3676,3677,3677,3678,3678,3679,3679,3680,
                3680,3681,3681,3681,3682,3682,3683,3683,3684,3684,3685,3685,3686,3686,3686,3687,3687,3688,3688,3689,3689,3690,3690,3691,3691,3692,3692,3692,3693,3693,3694,3694,
                3695,3695,3696,3696,3697,3697,3697,3698,3698,3699,3699,3700,3700,3701,3701,3702,3702,3702,3703,3703,3704,3704,3705,3705,3706,3706,3707,3707,3707,3708,3708,3709,
                3709,3710,3710,3711,3711,3712,3712,3712,3713,3713,3714,3714,3715,3715,3716,3716,3716,3717,3717,3718,3718,3719,3719,3720,3720,3721,3721,3721,3722,3722,3723,3723,
                3724,3724,3725,3725,3726,3726,3726,3727,3727,3728,3728,3729,3729,3730,3730,3730,3731,3731,3732,3732,3733,3733,3734,3734,3734,3735,3735,3736,3736,3737,3737,3738,
                3738,3739,3739,3739,3740,3740,3741,3741,3742,3742,3743,3743,3743,3744,3744,3745,3745,3746,3746,3747,3747,3747,3748,3748,3749,3749,3750,3750,3751,3751,3751,3752,
                3752,3753,3753,3754,3754,3755,3755,3755,3756,3756,3756,3757,3757,3758,3758,3759,3759,3760,3760,3761,3761,3762,3762,3763,3763,3764,3764,3764,3765,3765,3766,3766,
                3767,3767,3768,3768,3769,3769,3770,3770,3771,3771,3772,3772,3773,3773,3773,3774,3774,3775,3775,3776,3776,3777,3777,3778,3778,3779,3779,3780,3780,3781,3781,3782,
                3782,3782,3783,3783,3784,3784,3785,3785,3786,3786,3787,3787,3788,3788,3789,3789,3790,3790,3791,3791,3791,3792,3792,3793,3793,3794,3794,3795,3795,3796,3796,3797,
                3797,3798,3798,3799,3799,3800,3800,3800,3801,3801,3802,3802,3803,3803,3804,3804,3805,3805,3806,3806,3807,3807,3808,3808,3809,3809,3810,3810,3810,3811,3811,3812,
                3812,3813,3813,3814,3814,3815,3815,3816,3816,3817,3817,3818,3818,3819,3819,3820,3820,3820,3821,3821,3822,3822,3823,3823,3824,3824,3825,3825,3826,3826,3827,3827,
                3828,3828,3829,3829,3830,3830,3831,3831,3831,3832,3832,3833,3833,3834,3834,3835,3835,3836,3836,3837,3837,3838,3838,3839,3839,3840,3840,3841,3841,3842,3842,3842,
                3843,3843,3844,3844,3845,3845,3846,3846,3847,3847,3848,3848,3849,3849,3850,3850,3851,3851,3852,3852,3853,3853,3853,3854,3854,3855,3855,3856,3856,3857,3857,3858,
                3858,3859,3859,3860,3860,3861,3861,3862,3862,3863,3863,3864,3864,3865,3865,3865,3866,3866,3867,3867,3868,3868,3869,3869,3870,3870,3871,3871,3872,3872,3873,3873,
                3874,3874,3875,3875,3876,3876,3877,3877,3878,3878,3878,3879,3879,3880,3880,3881,3881,3882,3882,3883,3883,3884,3884,3885,3885,3886,3886,3887,3887,3888,3888,3889,
                3889,3890,3890,3891,3891,3891,3892,3892,3893,3893,3894,3894,3895,3895,3896,3896,3897,3897,3898,3898,3899,3899,3900,3900,3901,3901,3902,3902,3903,3903,3904,3904,
                3905,3905,3905,3906,3906,3907,3907,3908,3908,3909,3909,3910,3910,3911,3911,3912,3912,3913,3913,3914,3914,3915,3915,3916,3916,3917,3917,3918,3918,3919,3919,3920,
                3920,3921,3921,3921,3922,3922,3923,3923,3924,3924,3925,3925,3926,3926,3927,3927,3928,3928,3929,3929,3930,3930,3931,3931,3932,3932,3933,3933,3934,3934,3935,3935,
                3936,3936,3937,3937,3937,3938,3938,3939,3939,3940,3940,3941,3941,3942,3942,3943,3943,3944,3944,3945,3945,3946,3946,3947,3947,3948,3948,3949,3949,3950,3950,3951,
                3951,3952,3952,3953,3953,3954,3954,3955,3955,3956,3956,3956,3957,3957,3958,3958,3959,3959,3960,3960,3961,3961,3962,3962,3963,3963,3964,3964,3965,3965,3966,3966,
                3967,3967,3968,3968,3969,3969,3970,3970,3971,3971,3972,3972,3973,3973,3974,3974,3975,3975,3976,3976,3977,3977,3977,3978,3978,3979,3979,3980,3980,3981,3981,3982,
                3982,3983,3983,3984,3984,3985,3985,3986,3986,3987,3987,3988,3988,3989,3989,3990,3990,3991,3991,3992,3992,3993,3993,3994,3994,3995,3995,3996,3996,3997,3997,3998,
                3998,3999,3999,4000,4000,4001,4001,4002,4002,4002,4003,4003,4004,4004,4005,4005,4006,4006,4007,4007,4008,4008,4009,4009,4010,4010,4011,4011,4012,4012,4013,4013,
                4014,4014,4015,4015,4016,4016,4017,4017,4018,4018,4019,4019,4020,4020,4021,4021,4022,4022,4023,4023,4024,4024,4025,4025,4026,4026,4027,4027,4028,4028,4029,4029,
                4030,4030,4031,4031,4032,4032,4033,4033,4034,4034,4034,4035,4035,4036,4036,4037,4037,4038,4038,4039,4039,4040,4040,4041,4041,4042,4042,4043,4043,4044,4044,4045,
                4045,4046,4046,4047,4047,4048,4048,4049,4049,4050,4050,4051,4051,4052,4052,4053,4053,4054,4054,4055,4055,4056,4056,4057,4057,4058,4058,4059,4059,4060,4060,4061,
                4061,4062,4062,4063,4063,4064,4064,4065,4065,4066,4066,4067,4067,4068,4068,4069,4069,4070,4070,4071,4071,4072,4072,4073,4073,4074,4074,4075,4075,4076,4076,4077,
                4077,4078,4078,4079,4079,4080,4080,4081,4081,4082,4082,4083,4083,4084,4084,4085,4085,4086,4086,4087,4087,4088,4088,4089,4089,4090,4090,4091,4091,4092,4092,4093,
          },
          /* aec trigger 2 */
          {
                   0,  14,  29,  43,  57,  71,  84,  97, 110, 123, 135, 147, 159, 170, 181, 192, 203, 213, 223, 233, 242, 251, 260, 269, 277, 285, 293, 300, 307, 314, 319, 324,
                 330, 335, 341, 346, 352, 357, 362, 368, 373, 379, 384, 389, 395, 400, 405, 411, 416, 421, 427, 432, 437, 442, 448, 453, 458, 463, 468, 473, 479, 484, 489, 494,
                 499, 504, 509, 514, 519, 524, 529, 534, 539, 544, 549, 554, 559, 564, 569, 574, 579, 583, 588, 593, 598, 603, 607, 612, 617, 622, 626, 632, 638, 644, 649, 655,
                 660, 666, 671, 676, 681, 687, 692, 697, 702, 707, 712, 716, 721, 726, 730, 735, 740, 744, 748, 753, 757, 761, 765, 769, 772, 777, 782, 787, 792, 796, 801, 806,
                 810, 815, 819, 824, 828, 833, 837, 842, 846, 850, 855, 859, 863, 867, 871, 875, 879, 883, 887, 891, 895, 899, 903, 907, 911, 914, 918, 922, 925, 929, 933, 936,
                 940, 944, 948, 952, 957, 961, 965, 969, 973, 978, 982, 986, 990, 994, 998,1002,1006,1010,1014,1017,1021,1025,1029,1033,1036,1040,1044,1048,1051,1055,1059,1062,
                1066,1069,1073,1076,1080,1083,1086,1090,1093,1097,1100,1103,1106,1110,1113,1116,1119,1122,1126,1129,1132,1135,1138,1141,1144,1147,1150,1153,1154,1158,1161,1164,
                1167,1170,1173,1175,1178,1181,1184,1187,1190,1193,1196,1199,1202,1204,1207,1210,1213,1216,1218,1221,1224,1227,1229,1232,1235,1238,1240,1243,1246,1248,1251,1254,
                1256,1259,1261,1264,1266,1269,1272,1274,1277,1279,1282,1284,1287,1289,1291,1294,1296,1299,1301,1304,1306,1308,1311,1313,1315,1318,1320,1322,1325,1327,1329,1331,
                1334,1336,1338,1340,1342,1345,1347,1349,1351,1353,1355,1357,1357,1358,1360,1362,1364,1366,1368,1370,1372,1374,1376,1377,1379,1381,1383,1385,1387,1389,1391,1393,
                1394,1396,1398,1400,1402,1404,1406,1407,1409,1411,1413,1415,1417,1418,1420,1422,1424,1426,1427,1429,1431,1433,1435,1437,1438,1440,1442,1444,1445,1447,1449,1451,
                1453,1454,1456,1458,1460,1461,1463,1465,1467,1468,1470,1472,1474,1475,1477,1479,1481,1482,1484,1486,1488,1489,1491,1493,1494,1496,1498,1499,1501,1503,1505,1506,
                1508,1510,1511,1513,1515,1516,1518,1520,1521,1523,1525,1526,1528,1530,1531,1533,1534,1536,1538,1539,1541,1543,1544,1546,1548,1549,1551,1552,1554,1556,1557,1559,
                1560,1562,1564,1565,1567,1568,1570,1571,1573,1575,1576,1578,1579,1581,1582,1584,1585,1587,1589,1590,1592,1593,1595,1596,1598,1599,1601,1602,1604,1605,1607,1608,
                1610,1611,1613,1614,1616,1617,1619,1620,1622,1623,1625,1626,1628,1629,1631,1632,1633,1635,1636,1638,1639,1641,1642,1644,1645,1646,1648,1649,1651,1652,1654,1655,
                1656,1658,1659,1661,1662,1663,1665,1666,1668,1669,1670,1672,1673,1674,1676,1677,1679,1680,1681,1683,1684,1685,1687,1688,1689,1691,1692,1693,1695,1696,1697,1699,
                1700,1701,1703,1704,1705,1707,1708,1709,1711,1713,1714,1715,1717,1718,1720,1721,1723,1724,1726,1727,1729,1730,1732,1733,1735,1736,1738,1739,1740,1742,1743,1745,
                1746,1748,1749,1750,1752,1753,1755,1756,1757,1759,1760,1762,1763,1764,1766,1767,1769,1770,1771,1773,1774,1775,1777,1778,1780,1781,1782,1784,1785,1786,1788,1789,
                1790,1792,1793,1794,1796,1797,1798,1800,1801,1802,1803,1805,1806,1807,1809,1810,1811,1813,1814,1815,1816,1818,1819,1820,1821,1823,1824,1825,1826,1828,1829,1830,
                1831,1833,1834,1835,1836,1838,1839,1840,1841,1842,1844,1845,1846,1847,1848,1850,1851,1852,1853,1854,1856,1857,1858,1859,1860,1861,1863,1864,1865,1866,1867,1868,
                1870,1871,1872,1873,1874,1875,1877,1878,1879,1880,1881,1882,1884,1885,1886,1887,1888,1890,1891,1892,1893,1894,1895,1897,1898,1899,1900,1901,1903,1904,1905,1906,
                1907,1908,1910,1911,1912,1913,1914,1915,1917,1918,1919,1920,1921,1923,1924,1925,1926,1927,1928,1930,1931,1932,1933,1934,1936,1937,1938,1939,1940,1942,1943,1944,
                1945,1946,1947,1949,1950,1951,1952,1953,1955,1956,1957,1958,1959,1960,1962,1963,1964,1965,1966,1968,1969,1970,1971,1972,1974,1975,1976,1977,1978,1979,1981,1982,
                1983,1984,1985,1987,1988,1989,1990,1991,1993,1994,1995,1996,1997,1998,2000,2001,2002,2003,2004,2006,2007,2008,2009,2010,2012,2013,2014,2015,2016,2017,2018,2019,
                2020,2021,2023,2024,2025,2026,2027,2028,2029,2030,2031,2033,2034,2035,2036,2037,2038,2039,2040,2041,2042,2044,2045,2046,2047,2048,2049,2050,2051,2052,2053,2055,
                2056,2057,2058,2059,2060,2061,2062,2063,2064,2065,2067,2068,2069,2070,2071,2072,2073,2074,2075,2076,2077,2079,2080,2081,2082,2083,2084,2085,2086,2087,2088,2089,
                2090,2092,2093,2094,2095,2096,2097,2098,2099,2100,2101,2102,2103,2105,2106,2107,2108,2109,2110,2111,2112,2113,2114,2115,2116,2117,2119,2120,2121,2122,2123,2124,
                2125,2126,2127,2128,2129,2130,2131,2132,2133,2135,2136,2137,2138,2139,2140,2141,2142,2143,2144,2145,2146,2147,2148,2149,2151,2152,2153,2154,2155,2156,2157,2158,
                2159,2160,2161,2162,2163,2163,2164,2165,2166,2167,2168,2169,2170,2171,2172,2173,2174,2175,2176,2177,2178,2179,2180,2181,2182,2183,2184,2185,2186,2187,2188,2189,
                2190,2191,2192,2193,2194,2195,2196,2197,2198,2199,2200,2201,2202,2203,2204,2205,2206,2207,2208,2209,2209,2210,2211,2212,2213,2214,2215,2216,2217,2218,2219,2220,
                2221,2222,2223,2224,2225,2226,2227,2228,2229,2230,2231,2232,2233,2234,2235,2236,2237,2238,2239,2239,2240,2241,2242,2243,2244,2245,2246,2247,2248,2249,2250,2251,
                2252,2253,2254,2255,2256,2257,2258,2259,2260,2261,2262,2262,2263,2264,2265,2266,2267,2268,2269,2270,2271,2272,2273,2274,2275,2276,2277,2278,2279,2280,2281,2281,
                2282,2283,2284,2285,2286,2287,2288,2289,2290,2291,2292,2293,2294,2295,2296,2297,2298,2299,2299,2300,2301,2302,2303,2304,2305,2306,2307,2308,2309,2310,2311,2312,
                2313,2314,2314,2315,2316,2317,2318,2319,2320,2321,2322,2323,2324,2325,2326,2327,2328,2329,2329,2330,2331,2332,2333,2334,2335,2336,2337,2338,2339,2340,2341,2342,
                2342,2343,2344,2345,2346,2347,2348,2349,2350,2351,2352,2353,2354,2355,2355,2356,2357,2358,2359,2360,2361,2362,2363,2364,2365,2366,2366,2367,2368,2369,2370,2371,
                2372,2373,2374,2375,2376,2377,2378,2378,2379,2380,2381,2382,2383,2384,2385,2386,2387,2388,2388,2389,2390,2391,2392,2393,2394,2395,2396,2397,2398,2399,2399,2400,
                2401,2402,2403,2404,2405,2406,2407,2408,2408,2409,2410,2411,2412,2413,2414,2415,2416,2417,2418,2418,2419,2420,2421,2422,2423,2424,2425,2426,2427,2427,2428,2429,
                2430,2431,2432,2433,2434,2435,2436,2436,2437,2438,2439,2440,2441,2442,2443,2444,2444,2445,2446,2447,2448,2449,2450,2451,2452,2452,2453,2454,2455,2456,2457,2458,
                2459,2460,2461,2461,2462,2463,2464,2465,2466,2467,2468,2468,2469,2470,2471,2472,2473,2474,2475,2476,2476,2477,2478,2479,2480,2481,2482,2483,2483,2484,2485,2486,
                2487,2488,2489,2490,2491,2491,2492,2493,2494,2495,2496,2497,2498,2498,2499,2500,2501,2502,2503,2504,2505,2505,2506,2507,2508,2509,2510,2511,2511,2512,2513,2514,
                2515,2516,2517,2518,2518,2519,2520,2521,2522,2523,2524,2524,2525,2526,2527,2528,2529,2530,2531,2531,2532,2533,2534,2535,2536,2537,2537,2538,2539,2540,2541,2542,
                2543,2543,2544,2545,2546,2547,2548,2549,2549,2550,2551,2552,2553,2554,2555,2555,2556,2557,2558,2559,2560,2561,2561,2562,2563,2564,2565,2566,2566,2567,2568,2569,
                2570,2571,2572,2572,2573,2574,2575,2576,2577,2578,2578,2579,2580,2581,2582,2583,2583,2584,2585,2586,2587,2588,2588,2589,2590,2591,2592,2593,2594,2594,2595,2596,
                2597,2598,2599,2599,2600,2601,2602,2603,2604,2604,2605,2606,2607,2608,2609,2609,2610,2611,2612,2613,2614,2614,2615,2616,2617,2618,2619,2619,2620,2621,2622,2623,
                2624,2624,2625,2626,2627,2628,2629,2629,2630,2631,2632,2633,2633,2634,2635,2636,2637,2638,2638,2639,2640,2641,2642,2643,2643,2644,2645,2646,2647,2647,2648,2649,
                2650,2651,2652,2652,2653,2654,2655,2656,2656,2657,2658,2659,2660,2661,2661,2662,2663,2664,2665,2665,2666,2667,2668,2669,2669,2670,2671,2672,2673,2674,2674,2675,
                2676,2677,2678,2678,2679,2680,2681,2682,2682,2683,2684,2685,2686,2686,2687,2688,2689,2690,2690,2691,2692,2693,2694,2694,2695,2696,2697,2698,2699,2700,2701,2702,
                2703,2703,2704,2705,2706,2706,2707,2708,2709,2709,2710,2711,2712,2713,2713,2714,2715,2716,2716,2717,2718,2719,2719,2720,2721,2722,2722,2723,2724,2725,2726,2726,
                2727,2728,2729,2729,2730,2731,2732,2732,2733,2734,2735,2735,2736,2737,2738,2738,2739,2740,2741,2741,2742,2743,2744,2744,2745,2746,2747,2747,2748,2749,2750,2750,
                2751,2752,2753,2753,2754,2755,2756,2756,2757,2758,2759,2759,2760,2761,2762,2762,2763,2764,2764,2765,2766,2767,2767,2768,2769,2770,2770,2771,2772,2773,2773,2774,
                2775,2776,2776,2777,2778,2778,2779,2780,2781,2781,2782,2783,2784,2784,2785,2786,2787,2787,2788,2789,2789,2790,2791,2792,2792,2793,2794,2795,2795,2796,2797,2797,
                2798,2799,2800,2800,2801,2802,2803,2803,2804,2805,2805,2806,2807,2808,2808,2809,2810,2810,2811,2812,2813,2813,2814,2815,2815,2816,2817,2818,2818,2819,2820,2820,
                2821,2822,2823,2823,2824,2825,2825,2826,2827,2828,2828,2829,2830,2830,2831,2832,2833,2833,2834,2835,2835,2836,2837,2837,2838,2839,2840,2840,2841,2842,2842,2843,
                2844,2845,2845,2846,2847,2847,2848,2849,2849,2850,2851,2852,2852,2853,2854,2854,2855,2856,2856,2857,2858,2858,2859,2860,2861,2861,2862,2863,2863,2864,2865,2865,
                2866,2867,2867,2868,2869,2870,2870,2871,2872,2872,2873,2874,2874,2875,2876,2876,2877,2878,2878,2879,2880,2881,2881,2882,2883,2883,2884,2885,2885,2886,2887,2887,
                2888,2889,2889,2890,2891,2891,2892,2893,2893,2894,2895,2895,2896,2897,2897,2898,2899,2899,2900,2901,2902,2902,2903,2904,2904,2905,2906,2906,2907,2908,2908,2909,
                2910,2910,2911,2912,2912,2913,2914,2914,2915,2916,2916,2917,2918,2918,2919,2920,2920,2921,2922,2922,2923,2924,2924,2925,2925,2926,2927,2927,2928,2929,2929,2930,
                2931,2931,2932,2933,2933,2934,2935,2935,2936,2937,2937,2938,2939,2939,2940,2941,2941,2942,2943,2943,2944,2944,2945,2946,2946,2947,2948,2948,2949,2950,2950,2951,
                2952,2952,2953,2954,2954,2955,2955,2956,2957,2957,2958,2959,2959,2960,2961,2961,2962,2962,2963,2964,2964,2965,2966,2966,2967,2968,2968,2969,2969,2970,2971,2971,
                2972,2973,2973,2974,2975,2975,2976,2976,2977,2978,2978,2979,2980,2980,2981,2982,2982,2983,2983,2984,2985,2985,2986,2987,2987,2988,2988,2989,2990,2990,2991,2992,
                2992,2993,2993,2994,2995,2995,2996,2996,2997,2998,2998,2999,3000,3000,3001,3001,3002,3003,3003,3004,3005,3005,3006,3006,3007,3008,3008,3009,3009,3010,3011,3011,
                3012,3012,3013,3014,3014,3015,3016,3016,3017,3017,3018,3019,3019,3020,3020,3021,3022,3022,3023,3023,3024,3025,3025,3026,3026,3027,3028,3028,3029,3029,3030,3031,
                3031,3032,3032,3033,3034,3034,3035,3035,3036,3037,3037,3038,3038,3039,3040,3040,3041,3041,3042,3043,3043,3044,3044,3045,3046,3046,3047,3047,3048,3049,3049,3050,
                3050,3051,3051,3052,3053,3053,3054,3054,3055,3056,3056,3057,3057,3058,3058,3059,3060,3060,3061,3061,3062,3063,3063,3064,3064,3065,3065,3066,3067,3067,3068,3068,
                3069,3070,3070,3071,3071,3072,3072,3073,3074,3074,3075,3075,3076,3076,3077,3078,3078,3079,3079,3080,3080,3081,3082,3082,3083,3083,3084,3084,3085,3086,3086,3087,
                3087,3088,3088,3089,3090,3090,3091,3091,3092,3092,3093,3093,3094,3095,3095,3096,3096,3097,3097,3098,3099,3100,3101,3101,3102,3103,3103,3104,3104,3105,3106,3106,
                3107,3108,3108,3109,3109,3110,3111,3111,3112,3113,3113,3114,3114,3115,3116,3116,3117,3118,3118,3119,3119,3120,3121,3121,3122,3122,3123,3124,3124,3125,3126,3126,
                3127,3127,3128,3129,3129,3130,3130,3131,3132,3132,3133,3134,3134,3135,3135,3136,3137,3137,3138,3138,3139,3140,3140,3141,3141,3142,3143,3143,3144,3144,3145,3146,
                3146,3147,3147,3148,3149,3149,3150,3150,3151,3152,3152,3153,3153,3154,3155,3155,3156,3156,3157,3158,3158,3159,3159,3160,3161,3161,3162,3162,3163,3164,3164,3165,
                3165,3166,3167,3167,3168,3168,3169,3170,3170,3171,3171,3172,3172,3173,3174,3174,3175,3175,3176,3177,3177,3178,3178,3179,3180,3180,3181,3181,3182,3182,3183,3184,
                3184,3185,3185,3186,3187,3187,3188,3188,3189,3189,3190,3191,3191,3192,3192,3193,3194,3194,3195,3195,3196,3196,3197,3198,3198,3199,3199,3200,3200,3201,3202,3202,
                3203,3203,3204,3204,3205,3206,3206,3207,3207,3208,3208,3209,3210,3210,3211,3211,3212,3212,3213,3214,3214,3215,3215,3216,3216,3217,3218,3218,3219,3219,3220,3220,
                3221,3222,3222,3223,3223,3224,3224,3225,3225,3226,3227,3227,3228,3228,3229,3229,3230,3230,3231,3232,3232,3233,3233,3234,3234,3235,3235,3236,3237,3237,3238,3238,
                3239,3239,3240,3240,3241,3242,3242,3243,3243,3244,3244,3245,3245,3246,3247,3247,3248,3248,3249,3249,3250,3250,3251,3251,3252,3253,3253,3254,3254,3255,3255,3256,
                3256,3257,3257,3258,3259,3259,3260,3260,3261,3261,3262,3262,3263,3263,3264,3265,3265,3266,3266,3267,3267,3268,3268,3269,3269,3270,3270,3271,3272,3272,3273,3273,
                3274,3274,3275,3275,3276,3276,3277,3277,3278,3278,3279,3279,3280,3281,3281,3282,3282,3283,3283,3284,3284,3285,3285,3286,3286,3287,3287,3288,3288,3289,3290,3290,
                3291,3291,3292,3292,3293,3293,3294,3294,3295,3295,3296,3296,3297,3297,3298,3298,3299,3299,3300,3300,3301,3302,3302,3303,3303,3304,3304,3305,3305,3306,3306,3307,
                3307,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,3313,3313,3314,3314,3315,3315,3316,3316,3317,3317,3318,3318,3319,3319,3320,3320,3321,3321,3322,3322,3323,
                3323,3324,3324,3325,3326,3327,3328,3328,3329,3329,3330,3330,3331,3332,3332,3333,3333,3334,3334,3335,3336,3336,3337,3337,3338,3338,3339,3339,3340,3341,3341,3342,
                3342,3343,3343,3344,3344,3345,3346,3346,3347,3347,3348,3348,3349,3349,3350,3351,3351,3352,3352,3353,3353,3354,3354,3355,3355,3356,3357,3357,3358,3358,3359,3359,
                3360,3360,3361,3362,3362,3363,3363,3364,3364,3365,3365,3366,3366,3367,3367,3368,3369,3369,3370,3370,3371,3371,3372,3372,3373,3373,3374,3374,3375,3376,3376,3377,
                3377,3378,3378,3379,3379,3380,3380,3381,3381,3382,3382,3383,3384,3384,3385,3385,3386,3386,3387,3387,3388,3388,3389,3389,3390,3390,3391,3391,3392,3392,3393,3394,
                3394,3395,3395,3396,3396,3397,3397,3398,3398,3399,3399,3400,3400,3401,3401,3402,3402,3403,3403,3404,3404,3405,3405,3406,3406,3407,3408,3408,3409,3409,3410,3410,
                3411,3411,3412,3412,3413,3413,3414,3414,3415,3415,3416,3416,3417,3417,3418,3418,3419,3419,3420,3420,3421,3421,3422,3422,3423,3423,3424,3424,3425,3425,3426,3426,
                3427,3427,3428,3428,3429,3429,3430,3430,3431,3431,3432,3432,3433,3433,3434,3434,3435,3435,3436,3436,3437,3437,3438,3438,3439,3439,3440,3440,3441,3441,3442,3442,
                3443,3443,3444,3444,3445,3445,3446,3446,3446,3447,3447,3448,3448,3449,3449,3450,3450,3451,3451,3452,3452,3453,3453,3454,3454,3455,3455,3456,3456,3457,3457,3458,
                3458,3459,3459,3459,3460,3460,3461,3461,3462,3462,3463,3463,3464,3464,3465,3465,3466,3466,3467,3467,3468,3468,3468,3469,3469,3470,3470,3471,3471,3472,3472,3473,
                3473,3474,3474,3475,3475,3475,3476,3476,3477,3477,3478,3478,3479,3479,3480,3480,3481,3481,3481,3482,3482,3483,3483,3484,3484,3485,3485,3486,3486,3486,3487,3487,
                3488,3488,3489,3489,3490,3490,3491,3491,3491,3492,3492,3493,3493,3494,3494,3495,3495,3496,3496,3496,3497,3497,3498,3498,3499,3499,3500,3500,3500,3501,3501,3502,
                3502,3503,3503,3504,3504,3504,3505,3505,3506,3506,3507,3507,3508,3508,3508,3509,3509,3510,3510,3511,3511,3511,3512,3512,3513,3513,3514,3514,3514,3515,3515,3516,
                3516,3517,3517,3518,3518,3518,3519,3519,3520,3520,3521,3521,3521,3522,3522,3523,3523,3523,3524,3524,3524,3524,3524,3525,3525,3526,3526,3527,3527,3528,3528,3529,
                3529,3530,3530,3531,3531,3532,3532,3533,3533,3534,3534,3535,3535,3536,3536,3537,3537,3538,3538,3538,3539,3539,3540,3540,3541,3541,3542,3542,3543,3543,3544,3544,
                3545,3545,3546,3546,3547,3547,3548,3548,3549,3549,3550,3550,3551,3551,3551,3552,3552,3553,3553,3554,3554,3555,3555,3556,3556,3557,3557,3558,3558,3559,3559,3560,
                3560,3561,3561,3562,3562,3563,3563,3563,3564,3564,3565,3565,3566,3566,3567,3567,3568,3568,3569,3569,3570,3570,3571,3571,3572,3572,3573,3573,3573,3574,3574,3575,
                3575,3576,3576,3577,3577,3578,3578,3579,3579,3580,3580,3581,3581,3582,3582,3583,3583,3583,3584,3584,3585,3585,3586,3586,3587,3587,3588,3588,3589,3589,3590,3590,
                3591,3591,3592,3592,3592,3593,3593,3594,3594,3595,3595,3596,3596,3597,3597,3598,3598,3599,3599,3600,3600,3601,3601,3601,3602,3602,3603,3603,3604,3604,3605,3605,
                3606,3606,3607,3607,3608,3608,3609,3609,3609,3610,3610,3611,3611,3612,3612,3613,3613,3614,3614,3615,3615,3616,3616,3617,3617,3617,3618,3618,3619,3619,3620,3620,
                3621,3621,3622,3622,3623,3623,3624,3624,3624,3625,3625,3626,3626,3627,3627,3628,3628,3629,3629,3630,3630,3631,3631,3632,3632,3632,3633,3633,3634,3634,3635,3635,
                3636,3636,3637,3637,3638,3638,3639,3639,3639,3640,3640,3641,3641,3642,3642,3643,3643,3644,3644,3645,3645,3645,3646,3646,3647,3647,3648,3648,3649,3649,3650,3650,
                3651,3651,3652,3652,3652,3653,3653,3654,3654,3655,3655,3656,3656,3657,3657,3658,3658,3658,3659,3659,3660,3660,3661,3661,3662,3662,3663,3663,3664,3664,3664,3665,
                3665,3666,3666,3667,3667,3668,3668,3669,3669,3670,3670,3670,3671,3671,3672,3672,3673,3673,3674,3674,3675,3675,3675,3676,3676,3677,3677,3678,3678,3679,3679,3680,
                3680,3681,3681,3681,3682,3682,3683,3683,3684,3684,3685,3685,3686,3686,3686,3687,3687,3688,3688,3689,3689,3690,3690,3691,3691,3692,3692,3692,3693,3693,3694,3694,
                3695,3695,3696,3696,3697,3697,3697,3698,3698,3699,3699,3700,3700,3701,3701,3702,3702,3702,3703,3703,3704,3704,3705,3705,3706,3706,3707,3707,3707,3708,3708,3709,
                3709,3710,3710,3711,3711,3712,3712,3712,3713,3713,3714,3714,3715,3715,3716,3716,3716,3717,3717,3718,3718,3719,3719,3720,3720,3721,3721,3721,3722,3722,3723,3723,
                3724,3724,3725,3725,3726,3726,3726,3727,3727,3728,3728,3729,3729,3730,3730,3730,3731,3731,3732,3732,3733,3733,3734,3734,3734,3735,3735,3736,3736,3737,3737,3738,
                3738,3739,3739,3739,3740,3740,3741,3741,3742,3742,3743,3743,3743,3744,3744,3745,3745,3746,3746,3747,3747,3747,3748,3748,3749,3749,3750,3750,3751,3751,3751,3752,
                3752,3753,3753,3754,3754,3755,3755,3755,3756,3756,3756,3757,3757,3758,3758,3759,3759,3760,3760,3761,3761,3762,3762,3763,3763,3764,3764,3764,3765,3765,3766,3766,
                3767,3767,3768,3768,3769,3769,3770,3770,3771,3771,3772,3772,3773,3773,3773,3774,3774,3775,3775,3776,3776,3777,3777,3778,3778,3779,3779,3780,3780,3781,3781,3782,
                3782,3782,3783,3783,3784,3784,3785,3785,3786,3786,3787,3787,3788,3788,3789,3789,3790,3790,3791,3791,3791,3792,3792,3793,3793,3794,3794,3795,3795,3796,3796,3797,
                3797,3798,3798,3799,3799,3800,3800,3800,3801,3801,3802,3802,3803,3803,3804,3804,3805,3805,3806,3806,3807,3807,3808,3808,3809,3809,3810,3810,3810,3811,3811,3812,
                3812,3813,3813,3814,3814,3815,3815,3816,3816,3817,3817,3818,3818,3819,3819,3820,3820,3820,3821,3821,3822,3822,3823,3823,3824,3824,3825,3825,3826,3826,3827,3827,
                3828,3828,3829,3829,3830,3830,3831,3831,3831,3832,3832,3833,3833,3834,3834,3835,3835,3836,3836,3837,3837,3838,3838,3839,3839,3840,3840,3841,3841,3842,3842,3842,
                3843,3843,3844,3844,3845,3845,3846,3846,3847,3847,3848,3848,3849,3849,3850,3850,3851,3851,3852,3852,3853,3853,3853,3854,3854,3855,3855,3856,3856,3857,3857,3858,
                3858,3859,3859,3860,3860,3861,3861,3862,3862,3863,3863,3864,3864,3865,3865,3865,3866,3866,3867,3867,3868,3868,3869,3869,3870,3870,3871,3871,3872,3872,3873,3873,
                3874,3874,3875,3875,3876,3876,3877,3877,3878,3878,3878,3879,3879,3880,3880,3881,3881,3882,3882,3883,3883,3884,3884,3885,3885,3886,3886,3887,3887,3888,3888,3889,
                3889,3890,3890,3891,3891,3891,3892,3892,3893,3893,3894,3894,3895,3895,3896,3896,3897,3897,3898,3898,3899,3899,3900,3900,3901,3901,3902,3902,3903,3903,3904,3904,
                3905,3905,3905,3906,3906,3907,3907,3908,3908,3909,3909,3910,3910,3911,3911,3912,3912,3913,3913,3914,3914,3915,3915,3916,3916,3917,3917,3918,3918,3919,3919,3920,
                3920,3921,3921,3921,3922,3922,3923,3923,3924,3924,3925,3925,3926,3926,3927,3927,3928,3928,3929,3929,3930,3930,3931,3931,3932,3932,3933,3933,3934,3934,3935,3935,
                3936,3936,3937,3937,3937,3938,3938,3939,3939,3940,3940,3941,3941,3942,3942,3943,3943,3944,3944,3945,3945,3946,3946,3947,3947,3948,3948,3949,3949,3950,3950,3951,
                3951,3952,3952,3953,3953,3954,3954,3955,3955,3956,3956,3956,3957,3957,3958,3958,3959,3959,3960,3960,3961,3961,3962,3962,3963,3963,3964,3964,3965,3965,3966,3966,
                3967,3967,3968,3968,3969,3969,3970,3970,3971,3971,3972,3972,3973,3973,3974,3974,3975,3975,3976,3976,3977,3977,3977,3978,3978,3979,3979,3980,3980,3981,3981,3982,
                3982,3983,3983,3984,3984,3985,3985,3986,3986,3987,3987,3988,3988,3989,3989,3990,3990,3991,3991,3992,3992,3993,3993,3994,3994,3995,3995,3996,3996,3997,3997,3998,
                3998,3999,3999,4000,4000,4001,4001,4002,4002,4002,4003,4003,4004,4004,4005,4005,4006,4006,4007,4007,4008,4008,4009,4009,4010,4010,4011,4011,4012,4012,4013,4013,
                4014,4014,4015,4015,4016,4016,4017,4017,4018,4018,4019,4019,4020,4020,4021,4021,4022,4022,4023,4023,4024,4024,4025,4025,4026,4026,4027,4027,4028,4028,4029,4029,
                4030,4030,4031,4031,4032,4032,4033,4033,4034,4034,4034,4035,4035,4036,4036,4037,4037,4038,4038,4039,4039,4040,4040,4041,4041,4042,4042,4043,4043,4044,4044,4045,
                4045,4046,4046,4047,4047,4048,4048,4049,4049,4050,4050,4051,4051,4052,4052,4053,4053,4054,4054,4055,4055,4056,4056,4057,4057,4058,4058,4059,4059,4060,4060,4061,
                4061,4062,4062,4063,4063,4064,4064,4065,4065,4066,4066,4067,4067,4068,4068,4069,4069,4070,4070,4071,4071,4072,4072,4073,4073,4074,4074,4075,4075,4076,4076,4077,
                4077,4078,4078,4079,4079,4080,4080,4081,4081,4082,4082,4083,4083,4084,4084,4085,4085,4086,4086,4087,4087,4088,4088,4089,4089,4090,4090,4091,4091,4092,4092,4093,
          },
          /* aec trigger 3 */
          {
                   0,   3,   7,  11,  15,  19,  22,  26,  30,  34,  37,  41,  45,  49,  52,  56,  60,  63,  67,  71,  74,  78,  82,  85,  89,  93,  96, 100, 103, 107, 110, 114,
                 117, 121, 125, 128, 132, 135, 139, 142, 145, 149, 152, 156, 159, 163, 166, 169, 173, 176, 179, 183, 186, 189, 193, 196, 199, 203, 206, 209, 212, 216, 220, 224,
                 228, 232, 236, 240, 244, 248, 252, 256, 260, 264, 268, 272, 276, 280, 284, 288, 292, 297, 301, 305, 309, 313, 318, 322, 326, 331, 335, 339, 343, 348, 352, 357,
                 361, 365, 370, 374, 379, 383, 388, 392, 397, 401, 406, 411, 415, 420, 424, 429, 434, 438, 443, 448, 452, 457, 462, 467, 472, 477, 482, 487, 492, 496, 501, 506,
                 510, 515, 519, 524, 528, 532, 537, 541, 545, 550, 554, 558, 562, 566, 570, 574, 578, 582, 586, 590, 594, 598, 601, 605, 609, 612, 616, 619, 623, 626, 630, 633,
                 636, 640, 644, 648, 652, 655, 659, 663, 666, 670, 674, 677, 681, 684, 688, 691, 695, 698, 702, 705, 709, 712, 715, 719, 722, 725, 729, 732, 735, 738, 742, 745,
                 748, 751, 754, 757, 760, 763, 766, 769, 772, 775, 778, 781, 784, 787, 790, 793, 796, 799, 801, 804, 807, 810, 812, 815, 818, 820, 823, 826, 827, 831, 834, 837,
                 840, 843, 847, 850, 853, 856, 859, 862, 865, 868, 871, 874, 877, 880, 883, 886, 889, 892, 895, 897, 900, 903, 906, 909, 911, 914, 917, 920, 922, 925, 928, 930,
                 933, 935, 938, 941, 943, 946, 948, 951, 953, 956, 958, 960, 963, 965, 968, 970, 972, 975, 977, 979, 982, 984, 986, 988, 990, 993, 995, 997, 999,1001,1003,1005,
                1007,1009,1011,1014,1015,1017,1019,1021,1023,1025,1027,1029,1029,1032,1034,1037,1039,1042,1044,1047,1049,1052,1054,1056,1059,1061,1064,1066,1068,1071,1073,1075,
                1078,1080,1083,1085,1087,1090,1092,1094,1097,1099,1101,1104,1106,1108,1110,1113,1115,1117,1120,1122,1124,1126,1129,1131,1133,1135,1138,1140,1142,1144,1146,1149,
                1151,1153,1155,1157,1160,1162,1164,1166,1168,1170,1173,1175,1177,1179,1181,1183,1185,1187,1190,1192,1194,1196,1198,1200,1202,1204,1206,1208,1210,1212,1214,1216,
                1218,1220,1223,1225,1227,1229,1231,1233,1235,1237,1239,1240,1242,1244,1246,1248,1250,1252,1254,1256,1259,1261,1263,1265,1268,1270,1272,1274,1277,1279,1281,1283,
                1286,1288,1290,1292,1294,1297,1299,1301,1303,1305,1308,1310,1312,1314,1316,1319,1321,1323,1325,1327,1329,1331,1334,1336,1338,1340,1342,1344,1346,1349,1351,1353,
                1355,1357,1359,1361,1363,1365,1367,1370,1372,1374,1376,1378,1380,1382,1384,1386,1388,1390,1392,1394,1396,1398,1400,1402,1404,1406,1408,1410,1412,1414,1416,1418,
                1420,1422,1424,1426,1428,1430,1432,1434,1436,1438,1440,1442,1444,1446,1448,1450,1452,1454,1456,1457,1459,1461,1463,1465,1467,1469,1471,1473,1475,1476,1478,1480,
                1482,1484,1486,1488,1490,1491,1493,1495,1497,1498,1500,1502,1504,1506,1508,1510,1512,1514,1516,1518,1520,1522,1523,1525,1527,1529,1531,1533,1535,1537,1539,1541,
                1543,1544,1546,1548,1550,1552,1554,1556,1558,1559,1561,1563,1565,1567,1569,1571,1572,1574,1576,1578,1580,1582,1584,1585,1587,1589,1591,1593,1595,1596,1598,1600,
                1602,1604,1606,1607,1609,1611,1613,1615,1616,1618,1620,1622,1624,1626,1627,1629,1631,1633,1634,1636,1638,1640,1642,1643,1645,1647,1649,1650,1652,1654,1656,1658,
                1659,1661,1663,1665,1666,1668,1670,1672,1673,1675,1677,1679,1680,1682,1684,1685,1687,1689,1691,1692,1694,1696,1697,1699,1701,1703,1704,1706,1708,1709,1711,1713,
                1714,1716,1718,1720,1721,1723,1725,1726,1728,1730,1731,1733,1735,1736,1738,1740,1741,1743,1745,1746,1748,1750,1751,1753,1754,1756,1758,1759,1761,1763,1764,1766,
                1768,1769,1771,1772,1774,1776,1777,1779,1780,1782,1784,1785,1787,1789,1790,1792,1793,1795,1796,1798,1800,1801,1803,1804,1806,1808,1809,1811,1812,1814,1815,1817,
                1818,1820,1822,1823,1825,1826,1828,1829,1831,1832,1834,1836,1837,1839,1840,1842,1843,1845,1846,1848,1849,1851,1852,1854,1855,1857,1858,1860,1861,1863,1864,1866,
                1867,1869,1870,1872,1873,1875,1876,1878,1879,1881,1882,1884,1885,1887,1888,1889,1891,1892,1894,1895,1897,1898,1900,1901,1903,1904,1905,1907,1908,1910,1911,1912,
                1914,1915,1917,1918,1919,1921,1922,1923,1925,1926,1928,1929,1930,1932,1933,1935,1936,1937,1939,1940,1941,1943,1944,1946,1947,1948,1950,1951,1952,1954,1955,1956,
                1958,1959,1960,1962,1963,1965,1966,1967,1969,1970,1971,1973,1974,1975,1977,1978,1979,1981,1982,1983,1985,1986,1987,1989,1990,1991,1993,1994,1995,1997,1998,1999,
                2001,2002,2003,2004,2006,2007,2008,2010,2011,2012,2014,2015,2016,2017,2019,2020,2021,2023,2024,2025,2027,2028,2029,2030,2032,2033,2034,2036,2037,2038,2039,2041,
                2042,2043,2044,2046,2047,2048,2049,2051,2052,2053,2055,2056,2057,2058,2060,2061,2062,2063,2065,2066,2067,2068,2070,2071,2072,2073,2075,2076,2077,2078,2079,2081,
                2082,2083,2084,2086,2087,2088,2089,2090,2092,2093,2094,2095,2097,2098,2099,2100,2101,2103,2104,2105,2106,2107,2109,2110,2111,2112,2113,2115,2116,2117,2118,2119,
                2121,2122,2123,2124,2125,2127,2128,2129,2130,2131,2132,2134,2135,2136,2137,2138,2139,2141,2142,2143,2144,2145,2146,2148,2149,2150,2151,2152,2153,2155,2156,2157,
                2158,2159,2160,2161,2163,2164,2165,2166,2167,2168,2169,2171,2172,2173,2174,2175,2176,2177,2178,2180,2181,2182,2183,2184,2185,2186,2187,2188,2190,2191,2192,2193,
                2194,2195,2196,2197,2198,2200,2201,2202,2203,2204,2205,2206,2207,2208,2209,2210,2212,2213,2214,2215,2216,2217,2218,2219,2220,2221,2222,2223,2224,2225,2227,2228,
                2229,2230,2231,2232,2233,2234,2235,2236,2237,2238,2239,2240,2241,2242,2243,2244,2246,2247,2248,2249,2250,2251,2252,2253,2254,2255,2256,2257,2258,2259,2260,2261,
                2262,2263,2264,2265,2266,2267,2268,2269,2270,2271,2272,2273,2274,2275,2276,2277,2278,2279,2280,2281,2282,2283,2284,2285,2286,2287,2288,2289,2290,2291,2292,2293,
                2294,2295,2296,2297,2298,2299,2300,2301,2302,2303,2304,2305,2306,2307,2308,2309,2310,2310,2311,2312,2313,2314,2315,2316,2317,2318,2319,2320,2321,2322,2323,2324,
                2325,2326,2327,2328,2328,2329,2330,2331,2332,2333,2334,2335,2336,2337,2338,2339,2340,2340,2341,2342,2343,2344,2345,2346,2347,2348,2349,2350,2350,2351,2352,2353,
                2354,2355,2356,2357,2358,2358,2359,2360,2361,2362,2363,2364,2365,2366,2366,2367,2368,2369,2370,2371,2372,2373,2373,2374,2375,2376,2377,2378,2379,2379,2380,2381,
                2382,2383,2384,2385,2385,2386,2387,2388,2389,2390,2390,2391,2392,2393,2394,2395,2396,2396,2397,2398,2399,2399,2400,2401,2402,2402,2403,2404,2405,2406,2407,2408,
                2409,2409,2410,2411,2412,2413,2414,2415,2416,2416,2417,2418,2419,2420,2421,2422,2423,2423,2424,2425,2426,2426,2427,2428,2429,2429,2430,2431,2432,2433,2434,2435,
                2436,2436,2437,2438,2439,2439,2440,2441,2442,2442,2443,2444,2445,2446,2447,2448,2449,2449,2450,2451,2452,2452,2453,2454,2455,2455,2456,2457,2458,2459,2460,2461,
                2462,2462,2463,2464,2465,2465,2466,2467,2468,2468,2469,2470,2471,2471,2472,2473,2474,2474,2475,2476,2477,2478,2479,2480,2481,2481,2482,2483,2484,2484,2485,2486,
                2487,2487,2488,2489,2490,2490,2491,2492,2493,2493,2494,2495,2496,2497,2498,2499,2500,2500,2501,2502,2503,2503,2504,2505,2506,2506,2507,2508,2509,2509,2510,2511,
                2512,2512,2513,2514,2515,2515,2516,2517,2518,2518,2519,2520,2521,2521,2522,2523,2524,2524,2525,2526,2527,2527,2528,2529,2530,2530,2531,2532,2533,2533,2534,2535,
                2536,2536,2537,2538,2539,2539,2540,2541,2542,2542,2543,2544,2545,2545,2546,2547,2548,2548,2549,2550,2551,2551,2552,2553,2554,2554,2555,2556,2557,2557,2558,2559,
                2560,2560,2561,2562,2563,2563,2564,2565,2566,2566,2567,2567,2568,2569,2569,2570,2571,2571,2572,2573,2574,2574,2575,2576,2577,2577,2578,2579,2580,2580,2581,2582,
                2583,2583,2584,2585,2586,2586,2587,2588,2589,2589,2590,2590,2591,2592,2592,2593,2594,2594,2595,2596,2597,2597,2598,2599,2600,2600,2601,2601,2602,2603,2603,2604,
                2605,2605,2606,2607,2608,2608,2609,2610,2611,2611,2612,2613,2614,2614,2615,2616,2617,2617,2618,2618,2619,2620,2620,2621,2622,2622,2623,2623,2624,2625,2625,2626,
                2627,2627,2628,2629,2630,2630,2631,2632,2633,2633,2634,2634,2635,2636,2636,2637,2638,2638,2639,2639,2640,2641,2641,2642,2643,2643,2644,2645,2646,2646,2647,2648,
                2649,2649,2650,2650,2651,2652,2652,2653,2654,2654,2655,2655,2656,2657,2657,2658,2659,2659,2660,2660,2661,2662,2662,2663,2664,2664,2665,2666,2667,2667,2668,2669,
                2670,2670,2671,2671,2672,2673,2673,2674,2675,2675,2676,2676,2677,2678,2678,2679,2680,2680,2681,2681,2682,2683,2683,2684,2685,2685,2686,2686,2687,2688,2688,2689,
                2690,2690,2691,2691,2692,2693,2693,2694,2695,2695,2696,2696,2697,2698,2698,2699,2700,2700,2701,2701,2702,2703,2703,2704,2705,2705,2706,2706,2707,2707,2708,2708,
                2709,2709,2710,2710,2711,2712,2712,2713,2714,2714,2715,2715,2716,2717,2717,2718,2719,2719,2720,2720,2721,2722,2722,2723,2724,2724,2725,2725,2726,2726,2727,2727,
                2728,2728,2729,2729,2730,2731,2731,2732,2733,2733,2734,2734,2735,2736,2736,2737,2738,2738,2739,2739,2740,2740,2741,2741,2742,2742,2743,2743,2744,2745,2745,2746,
                2747,2747,2748,2748,2749,2749,2750,2750,2751,2751,2752,2752,2753,2754,2754,2755,2756,2756,2757,2757,2758,2758,2759,2759,2760,2760,2761,2761,2762,2763,2763,2764,
                2765,2765,2766,2766,2767,2767,2768,2768,2769,2769,2770,2770,2771,2772,2772,2773,2774,2774,2775,2775,2776,2776,2777,2777,2778,2778,2779,2779,2780,2780,2781,2781,
                2782,2782,2783,2783,2784,2784,2785,2785,2786,2786,2787,2787,2788,2789,2789,2790,2791,2791,2792,2792,2793,2793,2794,2794,2795,2795,2796,2796,2797,2797,2798,2798,
                2799,2799,2800,2800,2801,2801,2802,2802,2803,2803,2804,2804,2805,2805,2806,2806,2807,2807,2808,2808,2809,2809,2810,2810,2811,2811,2812,2812,2813,2813,2814,2814,
                2815,2815,2816,2816,2817,2817,2818,2818,2819,2819,2819,2820,2820,2820,2821,2821,2822,2822,2823,2823,2824,2824,2825,2825,2826,2826,2827,2827,2828,2828,2829,2829,
                2830,2830,2831,2831,2832,2832,2833,2833,2834,2834,2835,2835,2836,2836,2837,2837,2838,2838,2839,2839,2840,2840,2841,2841,2842,2842,2842,2843,2843,2843,2844,2844,
                2845,2845,2846,2846,2847,2847,2848,2848,2849,2849,2850,2850,2851,2851,2852,2852,2853,2853,2854,2854,2855,2855,2856,2856,2857,2857,2858,2858,2859,2859,2860,2860,
                2861,2861,2862,2862,2863,2863,2864,2864,2865,2865,2865,2866,2866,2866,2867,2867,2868,2868,2869,2869,2870,2870,2871,2871,2872,2872,2873,2873,2874,2874,2875,2875,
                2876,2876,2877,2877,2878,2878,2879,2879,2880,2880,2881,2881,2882,2882,2883,2883,2884,2884,2885,2885,2886,2886,2887,2887,2888,2888,2888,2889,2889,2889,2890,2890,
                2891,2891,2892,2892,2893,2893,2894,2894,2895,2895,2896,2896,2897,2897,2898,2898,2899,2899,2900,2900,2901,2901,2902,2902,2903,2903,2904,2904,2905,2905,2906,2906,
                2907,2907,2908,2908,2909,2909,2910,2910,2911,2911,2911,2912,2912,2912,2913,2913,2914,2914,2915,2915,2916,2916,2917,2917,2918,2918,2919,2919,2920,2920,2921,2921,
                2922,2922,2923,2923,2924,2924,2925,2925,2926,2926,2927,2927,2928,2928,2929,2929,2930,2930,2931,2931,2932,2932,2933,2933,2934,2934,2934,2935,2935,2935,2936,2936,
                2937,2937,2938,2938,2939,2939,2940,2940,2941,2941,2942,2942,2943,2943,2944,2944,2945,2945,2946,2946,2947,2947,2948,2948,2949,2949,2950,2950,2951,2951,2952,2952,
                2953,2953,2953,2954,2954,2954,2955,2955,2956,2956,2957,2957,2958,2958,2959,2959,2960,2960,2961,2961,2962,2962,2963,2963,2964,2964,2965,2965,2966,2966,2967,2967,
                2968,2968,2968,2969,2969,2969,2970,2970,2971,2971,2972,2972,2973,2973,2974,2974,2975,2975,2976,2976,2977,2977,2978,2978,2979,2979,2980,2980,2981,2981,2982,2982,
                2983,2983,2983,2984,2984,2984,2985,2985,2986,2986,2987,2987,2988,2988,2989,2989,2990,2990,2991,2991,2992,2992,2993,2993,2994,2994,2994,2995,2995,2995,2996,2996,
                2997,2997,2998,2998,2999,2999,3000,3000,3001,3001,3002,3002,3003,3003,3004,3004,3005,3005,3005,3006,3006,3006,3007,3007,3008,3008,3009,3009,3010,3010,3011,3011,
                3012,3012,3013,3013,3014,3014,3015,3015,3016,3016,3016,3017,3017,3017,3018,3018,3019,3019,3020,3020,3021,3021,3022,3022,3023,3023,3024,3024,3025,3025,3026,3026,
                3027,3027,3027,3028,3028,3028,3029,3029,3030,3030,3031,3031,3032,3032,3033,3033,3034,3034,3035,3035,3036,3036,3037,3037,3038,3038,3038,3039,3039,3039,3040,3040,
                3041,3041,3042,3042,3043,3043,3044,3044,3045,3045,3045,3046,3046,3046,3047,3047,3048,3048,3049,3049,3050,3050,3051,3051,3052,3052,3053,3053,3054,3054,3055,3055,
                3056,3056,3056,3057,3057,3057,3058,3058,3059,3059,3060,3060,3061,3061,3062,3062,3063,3063,3063,3064,3064,3064,3065,3065,3066,3066,3067,3067,3068,3068,3069,3069,
                3070,3070,3071,3071,3072,3072,3073,3073,3074,3074,3074,3075,3075,3075,3076,3076,3077,3077,3078,3078,3079,3079,3080,3080,3081,3081,3081,3082,3082,3082,3083,3083,
                3084,3084,3085,3085,3086,3086,3087,3087,3088,3088,3088,3089,3089,3089,3090,3090,3091,3091,3092,3092,3093,3093,3094,3094,3095,3095,3095,3096,3096,3096,3097,3097,
                3098,3098,3099,3099,3100,3100,3101,3101,3102,3102,3102,3103,3103,3103,3104,3104,3105,3105,3106,3106,3107,3107,3108,3108,3109,3109,3109,3110,3110,3110,3111,3111,
                3112,3112,3113,3113,3114,3114,3115,3115,3116,3116,3116,3117,3117,3117,3118,3118,3119,3119,3120,3120,3121,3121,3122,3122,3123,3123,3123,3124,3124,3124,3125,3125,
                3126,3126,3127,3127,3128,3128,3129,3129,3130,3130,3130,3131,3131,3131,3132,3132,3133,3133,3134,3134,3135,3135,3136,3136,3137,3137,3137,3138,3138,3138,3139,3139,
                3140,3140,3141,3141,3142,3142,3143,3143,3144,3144,3144,3145,3145,3145,3146,3146,3147,3147,3148,3148,3149,3149,3150,3150,3151,3151,3151,3152,3152,3152,3153,3153,
                3154,3154,3155,3155,3156,3156,3157,3157,3158,3158,3158,3159,3159,3159,3160,3160,3161,3161,3161,3162,3162,3162,3163,3163,3164,3164,3165,3165,3166,3166,3167,3167,
                3168,3168,3168,3169,3169,3169,3170,3170,3171,3171,3172,3172,3173,3173,3174,3174,3175,3175,3175,3176,3176,3176,3177,3177,3178,3178,3178,3179,3179,3179,3180,3180,
                3181,3181,3182,3182,3183,3183,3184,3184,3185,3185,3185,3186,3186,3186,3187,3187,3188,3188,3189,3189,3190,3190,3191,3191,3192,3192,3192,3193,3193,3193,3194,3194,
                3195,3195,3195,3196,3196,3196,3197,3197,3198,3198,3199,3199,3200,3200,3201,3201,3202,3202,3202,3203,3203,3203,3204,3204,3205,3205,3205,3206,3206,3206,3207,3207,
                3208,3208,3209,3209,3210,3210,3211,3211,3212,3212,3212,3213,3213,3213,3214,3214,3215,3215,3215,3216,3216,3216,3217,3217,3218,3218,3219,3219,3220,3220,3221,3221,
                3222,3222,3222,3223,3223,3223,3224,3224,3225,3225,3225,3226,3226,3226,3227,3227,3228,3228,3229,3229,3230,3230,3231,3231,3232,3232,3232,3233,3233,3233,3234,3234,
                3235,3235,3235,3236,3236,3236,3237,3237,3238,3238,3239,3239,3240,3240,3241,3241,3242,3242,3242,3243,3243,3243,3244,3244,3245,3245,3245,3246,3246,3246,3247,3247,
                3248,3248,3248,3249,3249,3249,3250,3250,3251,3251,3252,3252,3253,3253,3254,3254,3255,3255,3255,3256,3256,3256,3257,3257,3258,3258,3258,3259,3259,3259,3260,3260,
                3261,3261,3261,3262,3262,3262,3263,3263,3264,3264,3265,3265,3266,3266,3267,3267,3268,3268,3268,3269,3269,3269,3270,3270,3271,3271,3271,3272,3272,3272,3273,3273,
                3274,3274,3274,3275,3275,3275,3276,3276,3277,3277,3278,3278,3279,3279,3280,3280,3281,3281,3281,3282,3282,3282,3283,3283,3284,3284,3284,3285,3285,3285,3286,3286,
                3287,3287,3287,3288,3288,3288,3289,3289,3290,3290,3291,3291,3292,3292,3293,3293,3294,3294,3295,3295,3296,3296,3297,3297,3298,3298,3298,3299,3299,3299,3300,3300,
                3301,3301,3301,3302,3302,3302,3303,3303,3304,3304,3304,3305,3305,3305,3306,3306,3307,3307,3307,3308,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,3313,3313,
                3314,3314,3314,3315,3315,3315,3316,3316,3317,3317,3317,3318,3318,3318,3319,3319,3320,3320,3321,3321,3322,3322,3323,3323,3324,3324,3324,3325,3325,3325,3326,3326,
                3327,3327,3327,3328,3328,3328,3329,3329,3330,3330,3331,3331,3332,3332,3333,3333,3334,3334,3334,3335,3335,3335,3336,3336,3337,3337,3337,3338,3338,3338,3339,3339,
                3340,3340,3341,3341,3342,3342,3343,3343,3344,3344,3344,3345,3345,3345,3346,3346,3347,3347,3348,3348,3349,3349,3350,3350,3351,3351,3351,3352,3352,3352,3353,3353,
                3354,3354,3355,3355,3356,3356,3357,3357,3358,3358,3358,3359,3359,3359,3360,3360,3361,3361,3362,3362,3363,3363,3364,3364,3365,3365,3365,3366,3366,3366,3367,3367,
                3368,3368,3369,3369,3370,3370,3371,3371,3372,3372,3373,3373,3374,3374,3375,3375,3376,3376,3376,3377,3377,3377,3378,3378,3379,3379,3380,3380,3381,3381,3382,3382,
                3383,3383,3384,3384,3385,3385,3386,3386,3387,3387,3387,3388,3388,3388,3389,3389,3390,3390,3391,3391,3392,3392,3393,3393,3394,3394,3395,3395,3396,3396,3397,3397,
                3398,3398,3398,3399,3399,3399,3400,3400,3401,3401,3402,3402,3403,3403,3404,3404,3405,3405,3406,3406,3407,3407,3408,3408,3409,3409,3410,3410,3411,3411,3412,3412,
                3413,3413,3414,3414,3415,3415,3416,3416,3417,3417,3417,3418,3418,3418,3419,3419,3420,3420,3421,3421,3422,3422,3423,3423,3424,3424,3425,3425,3426,3426,3427,3427,
                3428,3428,3429,3429,3430,3430,3431,3431,3432,3432,3433,3433,3434,3434,3435,3435,3436,3436,3437,3437,3438,3438,3439,3439,3440,3440,3441,3441,3442,3442,3443,3443,
                3444,3444,3445,3445,3446,3446,3447,3447,3448,3448,3449,3449,3450,3450,3451,3451,3452,3452,3453,3453,3454,3454,3455,3455,3456,3456,3457,3457,3458,3458,3459,3459,
                3460,3460,3461,3461,3462,3462,3463,3463,3464,3464,3465,3465,3466,3466,3467,3467,3468,3468,3469,3469,3470,3470,3471,3471,3472,3472,3473,3473,3474,3475,3475,3476,
                3477,3477,3478,3478,3479,3479,3480,3480,3481,3481,3482,3482,3483,3483,3484,3484,3485,3485,3486,3486,3487,3487,3488,3488,3489,3489,3490,3490,3491,3491,3492,3492,
                3493,3493,3494,3494,3495,3496,3496,3497,3498,3498,3499,3499,3500,3500,3501,3501,3502,3502,3503,3503,3504,3504,3505,3505,3506,3506,3507,3507,3508,3508,3509,3509,
                3510,3510,3511,3511,3512,3513,3513,3514,3515,3515,3516,3516,3517,3517,3518,3518,3519,3519,3520,3520,3521,3521,3522,3522,3523,3523,3524,3524,3525,3526,3526,3527,
                3528,3528,3529,3529,3530,3530,3531,3531,3532,3532,3533,3533,3534,3535,3535,3536,3537,3537,3538,3538,3539,3539,3540,3540,3541,3541,3542,3542,3543,3543,3544,3544,
                3545,3545,3546,3546,3547,3548,3548,3549,3550,3550,3551,3551,3552,3552,3553,3553,3554,3554,3555,3555,3556,3557,3557,3558,3559,3559,3560,3560,3561,3562,3562,3563,
                3564,3564,3565,3565,3566,3566,3567,3567,3568,3568,3569,3569,3570,3571,3571,3572,3573,3573,3574,3574,3575,3575,3576,3576,3577,3577,3578,3578,3579,3580,3580,3581,
                3582,3582,3583,3583,3584,3585,3585,3586,3587,3587,3588,3588,3589,3589,3590,3590,3591,3591,3592,3592,3593,3594,3594,3595,3596,3596,3597,3597,3598,3599,3599,3600,
                3601,3601,3602,3602,3603,3603,3604,3604,3605,3605,3606,3606,3607,3608,3608,3609,3610,3610,3611,3611,3612,3613,3613,3614,3615,3615,3616,3616,3617,3618,3618,3619,
                3620,3620,3621,3621,3622,3622,3623,3623,3624,3624,3625,3625,3626,3627,3627,3628,3629,3629,3630,3630,3631,3632,3632,3633,3634,3634,3635,3635,3636,3637,3637,3638,
                3639,3639,3640,3640,3641,3642,3642,3643,3644,3644,3645,3645,3646,3647,3647,3648,3649,3649,3650,3650,3651,3652,3652,3653,3654,3654,3655,3655,3656,3657,3657,3658,
                3659,3659,3660,3660,3661,3662,3662,3663,3664,3664,3665,3665,3666,3666,3667,3667,3668,3668,3669,3669,3670,3671,3671,3672,3673,3673,3674,3675,3676,3676,3677,3678,
                3679,3679,3680,3680,3681,3682,3682,3683,3684,3684,3685,3685,3686,3687,3687,3688,3689,3689,3690,3690,3691,3692,3692,3693,3694,3694,3695,3695,3696,3697,3697,3698,
                3699,3699,3700,3700,3701,3702,3702,3703,3704,3704,3705,3705,3706,3707,3707,3708,3709,3709,3710,3710,3711,3712,3712,3713,3714,3714,3715,3715,3716,3717,3717,3718,
                3719,3719,3720,3721,3722,3722,3723,3724,3725,3725,3726,3726,3727,3728,3728,3729,3730,3730,3731,3731,3732,3733,3733,3734,3735,3735,3736,3736,3737,3738,3738,3739,
                3740,3740,3741,3742,3743,3743,3744,3745,3746,3746,3747,3747,3748,3749,3749,3750,3751,3751,3752,3752,3753,3754,3754,3755,3756,3756,3757,3758,3759,3759,3760,3761,
                3762,3762,3763,3763,3764,3765,3765,3766,3767,3767,3768,3768,3769,3770,3770,3771,3772,3772,3773,3774,3775,3775,3776,3777,3778,3778,3779,3780,3781,3782,3783,3784,
                3785,3785,3786,3787,3788,3788,3789,3790,3791,3791,3792,3793,3794,3795,3796,3797,3798,3798,3799,3800,3801,3802,3803,3804,3805,3805,3806,3807,3808,3809,3810,3811,
                3812,3812,3813,3814,3815,3816,3817,3818,3819,3819,3820,3821,3822,3823,3824,3825,3826,3826,3827,3828,3829,3830,3831,3832,3833,3834,3835,3836,3837,3838,3839,3840,
                3841,3841,3842,3843,3844,3845,3846,3847,3848,3849,3850,3851,3852,3853,3854,3855,3856,3857,3858,3859,3860,3861,3862,3863,3864,3865,3866,3867,3868,3869,3870,3871,
                3872,3873,3874,3875,3876,3877,3878,3879,3880,3881,3882,3883,3884,3885,3886,3887,3888,3889,3890,3891,3892,3893,3894,3895,3896,3897,3898,3899,3900,3901,3902,3903,
                3904,3905,3906,3907,3908,3909,3910,3911,3912,3913,3914,3915,3916,3917,3918,3919,3921,3922,3923,3924,3925,3926,3927,3928,3930,3931,3932,3933,3934,3935,3936,3937,
                3938,3939,3940,3941,3942,3943,3944,3945,3947,3948,3949,3950,3951,3952,3953,3954,3956,3957,3958,3959,3960,3961,3962,3963,3965,3966,3967,3968,3969,3970,3971,3972,
                3974,3975,3976,3977,3978,3979,3980,3981,3983,3984,3985,3986,3988,3989,3990,3991,3993,3994,3995,3996,3997,3998,3999,4000,4002,4003,4004,4005,4007,4008,4009,4010,
                4012,4013,4014,4015,4017,4018,4019,4020,4022,4023,4024,4025,4026,4027,4028,4029,4031,4032,4033,4034,4036,4037,4038,4039,4041,4042,4043,4044,4046,4047,4048,4049,
                4051,4052,4053,4054,4056,4057,4058,4059,4061,4062,4063,4065,4066,4067,4069,4070,4072,4073,4074,4075,4077,4078,4079,4080,4082,4083,4084,4086,4087,4088,4090,4091,
          },
          /* aec trigger 4 */
          {
                   0,   3,   7,  11,  15,  19,  22,  26,  30,  34,  37,  41,  45,  49,  52,  56,  60,  63,  67,  71,  74,  78,  82,  85,  89,  93,  96, 100, 103, 107, 110, 114,
                 117, 121, 125, 128, 132, 135, 139, 142, 145, 149, 152, 156, 159, 163, 166, 169, 173, 176, 179, 183, 186, 189, 193, 196, 199, 203, 206, 209, 212, 216, 220, 224,
                 228, 232, 236, 240, 244, 248, 252, 256, 260, 264, 268, 272, 276, 280, 284, 288, 292, 297, 301, 305, 309, 313, 318, 322, 326, 331, 335, 339, 343, 348, 352, 357,
                 361, 365, 370, 374, 379, 383, 388, 392, 397, 401, 406, 411, 415, 420, 424, 429, 434, 438, 443, 448, 452, 457, 462, 467, 472, 477, 482, 487, 492, 496, 501, 506,
                 510, 515, 519, 524, 528, 532, 537, 541, 545, 550, 554, 558, 562, 566, 570, 574, 578, 582, 586, 590, 594, 598, 601, 605, 609, 612, 616, 619, 623, 626, 630, 633,
                 636, 640, 644, 648, 652, 655, 659, 663, 666, 670, 674, 677, 681, 684, 688, 691, 695, 698, 702, 705, 709, 712, 715, 719, 722, 725, 729, 732, 735, 738, 742, 745,
                 748, 751, 754, 757, 760, 763, 766, 769, 772, 775, 778, 781, 784, 787, 790, 793, 796, 799, 801, 804, 807, 810, 812, 815, 818, 820, 823, 826, 827, 831, 834, 837,
                 840, 843, 847, 850, 853, 856, 859, 862, 865, 868, 871, 874, 877, 880, 883, 886, 889, 892, 895, 897, 900, 903, 906, 909, 911, 914, 917, 920, 922, 925, 928, 930,
                 933, 935, 938, 941, 943, 946, 948, 951, 953, 956, 958, 960, 963, 965, 968, 970, 972, 975, 977, 979, 982, 984, 986, 988, 990, 993, 995, 997, 999,1001,1003,1005,
                1007,1009,1011,1014,1015,1017,1019,1021,1023,1025,1027,1029,1029,1032,1034,1037,1039,1042,1044,1047,1049,1052,1054,1056,1059,1061,1064,1066,1068,1071,1073,1075,
                1078,1080,1083,1085,1087,1090,1092,1094,1097,1099,1101,1104,1106,1108,1110,1113,1115,1117,1120,1122,1124,1126,1129,1131,1133,1135,1138,1140,1142,1144,1146,1149,
                1151,1153,1155,1157,1160,1162,1164,1166,1168,1170,1173,1175,1177,1179,1181,1183,1185,1187,1190,1192,1194,1196,1198,1200,1202,1204,1206,1208,1210,1212,1214,1216,
                1218,1220,1223,1225,1227,1229,1231,1233,1235,1237,1239,1240,1242,1244,1246,1248,1250,1252,1254,1256,1259,1261,1263,1265,1268,1270,1272,1274,1277,1279,1281,1283,
                1286,1288,1290,1292,1294,1297,1299,1301,1303,1305,1308,1310,1312,1314,1316,1319,1321,1323,1325,1327,1329,1331,1334,1336,1338,1340,1342,1344,1346,1349,1351,1353,
                1355,1357,1359,1361,1363,1365,1367,1370,1372,1374,1376,1378,1380,1382,1384,1386,1388,1390,1392,1394,1396,1398,1400,1402,1404,1406,1408,1410,1412,1414,1416,1418,
                1420,1422,1424,1426,1428,1430,1432,1434,1436,1438,1440,1442,1444,1446,1448,1450,1452,1454,1456,1457,1459,1461,1463,1465,1467,1469,1471,1473,1475,1476,1478,1480,
                1482,1484,1486,1488,1490,1491,1493,1495,1497,1498,1500,1502,1504,1506,1508,1510,1512,1514,1516,1518,1520,1522,1523,1525,1527,1529,1531,1533,1535,1537,1539,1541,
                1543,1544,1546,1548,1550,1552,1554,1556,1558,1559,1561,1563,1565,1567,1569,1571,1572,1574,1576,1578,1580,1582,1584,1585,1587,1589,1591,1593,1595,1596,1598,1600,
                1602,1604,1606,1607,1609,1611,1613,1615,1616,1618,1620,1622,1624,1626,1627,1629,1631,1633,1634,1636,1638,1640,1642,1643,1645,1647,1649,1650,1652,1654,1656,1658,
                1659,1661,1663,1665,1666,1668,1670,1672,1673,1675,1677,1679,1680,1682,1684,1685,1687,1689,1691,1692,1694,1696,1697,1699,1701,1703,1704,1706,1708,1709,1711,1713,
                1714,1716,1718,1720,1721,1723,1725,1726,1728,1730,1731,1733,1735,1736,1738,1740,1741,1743,1745,1746,1748,1750,1751,1753,1754,1756,1758,1759,1761,1763,1764,1766,
                1768,1769,1771,1772,1774,1776,1777,1779,1780,1782,1784,1785,1787,1789,1790,1792,1793,1795,1796,1798,1800,1801,1803,1804,1806,1808,1809,1811,1812,1814,1815,1817,
                1818,1820,1822,1823,1825,1826,1828,1829,1831,1832,1834,1836,1837,1839,1840,1842,1843,1845,1846,1848,1849,1851,1852,1854,1855,1857,1858,1860,1861,1863,1864,1866,
                1867,1869,1870,1872,1873,1875,1876,1878,1879,1881,1882,1884,1885,1887,1888,1889,1891,1892,1894,1895,1897,1898,1900,1901,1903,1904,1905,1907,1908,1910,1911,1912,
                1914,1915,1917,1918,1919,1921,1922,1923,1925,1926,1928,1929,1930,1932,1933,1935,1936,1937,1939,1940,1941,1943,1944,1946,1947,1948,1950,1951,1952,1954,1955,1956,
                1958,1959,1960,1962,1963,1965,1966,1967,1969,1970,1971,1973,1974,1975,1977,1978,1979,1981,1982,1983,1985,1986,1987,1989,1990,1991,1993,1994,1995,1997,1998,1999,
                2001,2002,2003,2004,2006,2007,2008,2010,2011,2012,2014,2015,2016,2017,2019,2020,2021,2023,2024,2025,2027,2028,2029,2030,2032,2033,2034,2036,2037,2038,2039,2041,
                2042,2043,2044,2046,2047,2048,2049,2051,2052,2053,2055,2056,2057,2058,2060,2061,2062,2063,2065,2066,2067,2068,2070,2071,2072,2073,2075,2076,2077,2078,2079,2081,
                2082,2083,2084,2086,2087,2088,2089,2090,2092,2093,2094,2095,2097,2098,2099,2100,2101,2103,2104,2105,2106,2107,2109,2110,2111,2112,2113,2115,2116,2117,2118,2119,
                2121,2122,2123,2124,2125,2127,2128,2129,2130,2131,2132,2134,2135,2136,2137,2138,2139,2141,2142,2143,2144,2145,2146,2148,2149,2150,2151,2152,2153,2155,2156,2157,
                2158,2159,2160,2161,2163,2164,2165,2166,2167,2168,2169,2171,2172,2173,2174,2175,2176,2177,2178,2180,2181,2182,2183,2184,2185,2186,2187,2188,2190,2191,2192,2193,
                2194,2195,2196,2197,2198,2200,2201,2202,2203,2204,2205,2206,2207,2208,2209,2210,2212,2213,2214,2215,2216,2217,2218,2219,2220,2221,2222,2223,2224,2225,2227,2228,
                2229,2230,2231,2232,2233,2234,2235,2236,2237,2238,2239,2240,2241,2242,2243,2244,2246,2247,2248,2249,2250,2251,2252,2253,2254,2255,2256,2257,2258,2259,2260,2261,
                2262,2263,2264,2265,2266,2267,2268,2269,2270,2271,2272,2273,2274,2275,2276,2277,2278,2279,2280,2281,2282,2283,2284,2285,2286,2287,2288,2289,2290,2291,2292,2293,
                2294,2295,2296,2297,2298,2299,2300,2301,2302,2303,2304,2305,2306,2307,2308,2309,2310,2310,2311,2312,2313,2314,2315,2316,2317,2318,2319,2320,2321,2322,2323,2324,
                2325,2326,2327,2328,2328,2329,2330,2331,2332,2333,2334,2335,2336,2337,2338,2339,2340,2340,2341,2342,2343,2344,2345,2346,2347,2348,2349,2350,2350,2351,2352,2353,
                2354,2355,2356,2357,2358,2358,2359,2360,2361,2362,2363,2364,2365,2366,2366,2367,2368,2369,2370,2371,2372,2373,2373,2374,2375,2376,2377,2378,2379,2379,2380,2381,
                2382,2383,2384,2385,2385,2386,2387,2388,2389,2390,2390,2391,2392,2393,2394,2395,2396,2396,2397,2398,2399,2399,2400,2401,2402,2402,2403,2404,2405,2406,2407,2408,
                2409,2409,2410,2411,2412,2413,2414,2415,2416,2416,2417,2418,2419,2420,2421,2422,2423,2423,2424,2425,2426,2426,2427,2428,2429,2429,2430,2431,2432,2433,2434,2435,
                2436,2436,2437,2438,2439,2439,2440,2441,2442,2442,2443,2444,2445,2446,2447,2448,2449,2449,2450,2451,2452,2452,2453,2454,2455,2455,2456,2457,2458,2459,2460,2461,
                2462,2462,2463,2464,2465,2465,2466,2467,2468,2468,2469,2470,2471,2471,2472,2473,2474,2474,2475,2476,2477,2478,2479,2480,2481,2481,2482,2483,2484,2484,2485,2486,
                2487,2487,2488,2489,2490,2490,2491,2492,2493,2493,2494,2495,2496,2497,2498,2499,2500,2500,2501,2502,2503,2503,2504,2505,2506,2506,2507,2508,2509,2509,2510,2511,
                2512,2512,2513,2514,2515,2515,2516,2517,2518,2518,2519,2520,2521,2521,2522,2523,2524,2524,2525,2526,2527,2527,2528,2529,2530,2530,2531,2532,2533,2533,2534,2535,
                2536,2536,2537,2538,2539,2539,2540,2541,2542,2542,2543,2544,2545,2545,2546,2547,2548,2548,2549,2550,2551,2551,2552,2553,2554,2554,2555,2556,2557,2557,2558,2559,
                2560,2560,2561,2562,2563,2563,2564,2565,2566,2566,2567,2567,2568,2569,2569,2570,2571,2571,2572,2573,2574,2574,2575,2576,2577,2577,2578,2579,2580,2580,2581,2582,
                2583,2583,2584,2585,2586,2586,2587,2588,2589,2589,2590,2590,2591,2592,2592,2593,2594,2594,2595,2596,2597,2597,2598,2599,2600,2600,2601,2601,2602,2603,2603,2604,
                2605,2605,2606,2607,2608,2608,2609,2610,2611,2611,2612,2613,2614,2614,2615,2616,2617,2617,2618,2618,2619,2620,2620,2621,2622,2622,2623,2623,2624,2625,2625,2626,
                2627,2627,2628,2629,2630,2630,2631,2632,2633,2633,2634,2634,2635,2636,2636,2637,2638,2638,2639,2639,2640,2641,2641,2642,2643,2643,2644,2645,2646,2646,2647,2648,
                2649,2649,2650,2650,2651,2652,2652,2653,2654,2654,2655,2655,2656,2657,2657,2658,2659,2659,2660,2660,2661,2662,2662,2663,2664,2664,2665,2666,2667,2667,2668,2669,
                2670,2670,2671,2671,2672,2673,2673,2674,2675,2675,2676,2676,2677,2678,2678,2679,2680,2680,2681,2681,2682,2683,2683,2684,2685,2685,2686,2686,2687,2688,2688,2689,
                2690,2690,2691,2691,2692,2693,2693,2694,2695,2695,2696,2696,2697,2698,2698,2699,2700,2700,2701,2701,2702,2703,2703,2704,2705,2705,2706,2706,2707,2707,2708,2708,
                2709,2709,2710,2710,2711,2712,2712,2713,2714,2714,2715,2715,2716,2717,2717,2718,2719,2719,2720,2720,2721,2722,2722,2723,2724,2724,2725,2725,2726,2726,2727,2727,
                2728,2728,2729,2729,2730,2731,2731,2732,2733,2733,2734,2734,2735,2736,2736,2737,2738,2738,2739,2739,2740,2740,2741,2741,2742,2742,2743,2743,2744,2745,2745,2746,
                2747,2747,2748,2748,2749,2749,2750,2750,2751,2751,2752,2752,2753,2754,2754,2755,2756,2756,2757,2757,2758,2758,2759,2759,2760,2760,2761,2761,2762,2763,2763,2764,
                2765,2765,2766,2766,2767,2767,2768,2768,2769,2769,2770,2770,2771,2772,2772,2773,2774,2774,2775,2775,2776,2776,2777,2777,2778,2778,2779,2779,2780,2780,2781,2781,
                2782,2782,2783,2783,2784,2784,2785,2785,2786,2786,2787,2787,2788,2789,2789,2790,2791,2791,2792,2792,2793,2793,2794,2794,2795,2795,2796,2796,2797,2797,2798,2798,
                2799,2799,2800,2800,2801,2801,2802,2802,2803,2803,2804,2804,2805,2805,2806,2806,2807,2807,2808,2808,2809,2809,2810,2810,2811,2811,2812,2812,2813,2813,2814,2814,
                2815,2815,2816,2816,2817,2817,2818,2818,2819,2819,2819,2820,2820,2820,2821,2821,2822,2822,2823,2823,2824,2824,2825,2825,2826,2826,2827,2827,2828,2828,2829,2829,
                2830,2830,2831,2831,2832,2832,2833,2833,2834,2834,2835,2835,2836,2836,2837,2837,2838,2838,2839,2839,2840,2840,2841,2841,2842,2842,2842,2843,2843,2843,2844,2844,
                2845,2845,2846,2846,2847,2847,2848,2848,2849,2849,2850,2850,2851,2851,2852,2852,2853,2853,2854,2854,2855,2855,2856,2856,2857,2857,2858,2858,2859,2859,2860,2860,
                2861,2861,2862,2862,2863,2863,2864,2864,2865,2865,2865,2866,2866,2866,2867,2867,2868,2868,2869,2869,2870,2870,2871,2871,2872,2872,2873,2873,2874,2874,2875,2875,
                2876,2876,2877,2877,2878,2878,2879,2879,2880,2880,2881,2881,2882,2882,2883,2883,2884,2884,2885,2885,2886,2886,2887,2887,2888,2888,2888,2889,2889,2889,2890,2890,
                2891,2891,2892,2892,2893,2893,2894,2894,2895,2895,2896,2896,2897,2897,2898,2898,2899,2899,2900,2900,2901,2901,2902,2902,2903,2903,2904,2904,2905,2905,2906,2906,
                2907,2907,2908,2908,2909,2909,2910,2910,2911,2911,2911,2912,2912,2912,2913,2913,2914,2914,2915,2915,2916,2916,2917,2917,2918,2918,2919,2919,2920,2920,2921,2921,
                2922,2922,2923,2923,2924,2924,2925,2925,2926,2926,2927,2927,2928,2928,2929,2929,2930,2930,2931,2931,2932,2932,2933,2933,2934,2934,2934,2935,2935,2935,2936,2936,
                2937,2937,2938,2938,2939,2939,2940,2940,2941,2941,2942,2942,2943,2943,2944,2944,2945,2945,2946,2946,2947,2947,2948,2948,2949,2949,2950,2950,2951,2951,2952,2952,
                2953,2953,2953,2954,2954,2954,2955,2955,2956,2956,2957,2957,2958,2958,2959,2959,2960,2960,2961,2961,2962,2962,2963,2963,2964,2964,2965,2965,2966,2966,2967,2967,
                2968,2968,2968,2969,2969,2969,2970,2970,2971,2971,2972,2972,2973,2973,2974,2974,2975,2975,2976,2976,2977,2977,2978,2978,2979,2979,2980,2980,2981,2981,2982,2982,
                2983,2983,2983,2984,2984,2984,2985,2985,2986,2986,2987,2987,2988,2988,2989,2989,2990,2990,2991,2991,2992,2992,2993,2993,2994,2994,2994,2995,2995,2995,2996,2996,
                2997,2997,2998,2998,2999,2999,3000,3000,3001,3001,3002,3002,3003,3003,3004,3004,3005,3005,3005,3006,3006,3006,3007,3007,3008,3008,3009,3009,3010,3010,3011,3011,
                3012,3012,3013,3013,3014,3014,3015,3015,3016,3016,3016,3017,3017,3017,3018,3018,3019,3019,3020,3020,3021,3021,3022,3022,3023,3023,3024,3024,3025,3025,3026,3026,
                3027,3027,3027,3028,3028,3028,3029,3029,3030,3030,3031,3031,3032,3032,3033,3033,3034,3034,3035,3035,3036,3036,3037,3037,3038,3038,3038,3039,3039,3039,3040,3040,
                3041,3041,3042,3042,3043,3043,3044,3044,3045,3045,3045,3046,3046,3046,3047,3047,3048,3048,3049,3049,3050,3050,3051,3051,3052,3052,3053,3053,3054,3054,3055,3055,
                3056,3056,3056,3057,3057,3057,3058,3058,3059,3059,3060,3060,3061,3061,3062,3062,3063,3063,3063,3064,3064,3064,3065,3065,3066,3066,3067,3067,3068,3068,3069,3069,
                3070,3070,3071,3071,3072,3072,3073,3073,3074,3074,3074,3075,3075,3075,3076,3076,3077,3077,3078,3078,3079,3079,3080,3080,3081,3081,3081,3082,3082,3082,3083,3083,
                3084,3084,3085,3085,3086,3086,3087,3087,3088,3088,3088,3089,3089,3089,3090,3090,3091,3091,3092,3092,3093,3093,3094,3094,3095,3095,3095,3096,3096,3096,3097,3097,
                3098,3098,3099,3099,3100,3100,3101,3101,3102,3102,3102,3103,3103,3103,3104,3104,3105,3105,3106,3106,3107,3107,3108,3108,3109,3109,3109,3110,3110,3110,3111,3111,
                3112,3112,3113,3113,3114,3114,3115,3115,3116,3116,3116,3117,3117,3117,3118,3118,3119,3119,3120,3120,3121,3121,3122,3122,3123,3123,3123,3124,3124,3124,3125,3125,
                3126,3126,3127,3127,3128,3128,3129,3129,3130,3130,3130,3131,3131,3131,3132,3132,3133,3133,3134,3134,3135,3135,3136,3136,3137,3137,3137,3138,3138,3138,3139,3139,
                3140,3140,3141,3141,3142,3142,3143,3143,3144,3144,3144,3145,3145,3145,3146,3146,3147,3147,3148,3148,3149,3149,3150,3150,3151,3151,3151,3152,3152,3152,3153,3153,
                3154,3154,3155,3155,3156,3156,3157,3157,3158,3158,3158,3159,3159,3159,3160,3160,3161,3161,3161,3162,3162,3162,3163,3163,3164,3164,3165,3165,3166,3166,3167,3167,
                3168,3168,3168,3169,3169,3169,3170,3170,3171,3171,3172,3172,3173,3173,3174,3174,3175,3175,3175,3176,3176,3176,3177,3177,3178,3178,3178,3179,3179,3179,3180,3180,
                3181,3181,3182,3182,3183,3183,3184,3184,3185,3185,3185,3186,3186,3186,3187,3187,3188,3188,3189,3189,3190,3190,3191,3191,3192,3192,3192,3193,3193,3193,3194,3194,
                3195,3195,3195,3196,3196,3196,3197,3197,3198,3198,3199,3199,3200,3200,3201,3201,3202,3202,3202,3203,3203,3203,3204,3204,3205,3205,3205,3206,3206,3206,3207,3207,
                3208,3208,3209,3209,3210,3210,3211,3211,3212,3212,3212,3213,3213,3213,3214,3214,3215,3215,3215,3216,3216,3216,3217,3217,3218,3218,3219,3219,3220,3220,3221,3221,
                3222,3222,3222,3223,3223,3223,3224,3224,3225,3225,3225,3226,3226,3226,3227,3227,3228,3228,3229,3229,3230,3230,3231,3231,3232,3232,3232,3233,3233,3233,3234,3234,
                3235,3235,3235,3236,3236,3236,3237,3237,3238,3238,3239,3239,3240,3240,3241,3241,3242,3242,3242,3243,3243,3243,3244,3244,3245,3245,3245,3246,3246,3246,3247,3247,
                3248,3248,3248,3249,3249,3249,3250,3250,3251,3251,3252,3252,3253,3253,3254,3254,3255,3255,3255,3256,3256,3256,3257,3257,3258,3258,3258,3259,3259,3259,3260,3260,
                3261,3261,3261,3262,3262,3262,3263,3263,3264,3264,3265,3265,3266,3266,3267,3267,3268,3268,3268,3269,3269,3269,3270,3270,3271,3271,3271,3272,3272,3272,3273,3273,
                3274,3274,3274,3275,3275,3275,3276,3276,3277,3277,3278,3278,3279,3279,3280,3280,3281,3281,3281,3282,3282,3282,3283,3283,3284,3284,3284,3285,3285,3285,3286,3286,
                3287,3287,3287,3288,3288,3288,3289,3289,3290,3290,3291,3291,3292,3292,3293,3293,3294,3294,3295,3295,3296,3296,3297,3297,3298,3298,3298,3299,3299,3299,3300,3300,
                3301,3301,3301,3302,3302,3302,3303,3303,3304,3304,3304,3305,3305,3305,3306,3306,3307,3307,3307,3308,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,3313,3313,
                3314,3314,3314,3315,3315,3315,3316,3316,3317,3317,3317,3318,3318,3318,3319,3319,3320,3320,3321,3321,3322,3322,3323,3323,3324,3324,3324,3325,3325,3325,3326,3326,
                3327,3327,3327,3328,3328,3328,3329,3329,3330,3330,3331,3331,3332,3332,3333,3333,3334,3334,3334,3335,3335,3335,3336,3336,3337,3337,3337,3338,3338,3338,3339,3339,
                3340,3340,3341,3341,3342,3342,3343,3343,3344,3344,3344,3345,3345,3345,3346,3346,3347,3347,3348,3348,3349,3349,3350,3350,3351,3351,3351,3352,3352,3352,3353,3353,
                3354,3354,3355,3355,3356,3356,3357,3357,3358,3358,3358,3359,3359,3359,3360,3360,3361,3361,3362,3362,3363,3363,3364,3364,3365,3365,3365,3366,3366,3366,3367,3367,
                3368,3368,3369,3369,3370,3370,3371,3371,3372,3372,3373,3373,3374,3374,3375,3375,3376,3376,3376,3377,3377,3377,3378,3378,3379,3379,3380,3380,3381,3381,3382,3382,
                3383,3383,3384,3384,3385,3385,3386,3386,3387,3387,3387,3388,3388,3388,3389,3389,3390,3390,3391,3391,3392,3392,3393,3393,3394,3394,3395,3395,3396,3396,3397,3397,
                3398,3398,3398,3399,3399,3399,3400,3400,3401,3401,3402,3402,3403,3403,3404,3404,3405,3405,3406,3406,3407,3407,3408,3408,3409,3409,3410,3410,3411,3411,3412,3412,
                3413,3413,3414,3414,3415,3415,3416,3416,3417,3417,3417,3418,3418,3418,3419,3419,3420,3420,3421,3421,3422,3422,3423,3423,3424,3424,3425,3425,3426,3426,3427,3427,
                3428,3428,3429,3429,3430,3430,3431,3431,3432,3432,3433,3433,3434,3434,3435,3435,3436,3436,3437,3437,3438,3438,3439,3439,3440,3440,3441,3441,3442,3442,3443,3443,
                3444,3444,3445,3445,3446,3446,3447,3447,3448,3448,3449,3449,3450,3450,3451,3451,3452,3452,3453,3453,3454,3454,3455,3455,3456,3456,3457,3457,3458,3458,3459,3459,
                3460,3460,3461,3461,3462,3462,3463,3463,3464,3464,3465,3465,3466,3466,3467,3467,3468,3468,3469,3469,3470,3470,3471,3471,3472,3472,3473,3473,3474,3475,3475,3476,
                3477,3477,3478,3478,3479,3479,3480,3480,3481,3481,3482,3482,3483,3483,3484,3484,3485,3485,3486,3486,3487,3487,3488,3488,3489,3489,3490,3490,3491,3491,3492,3492,
                3493,3493,3494,3494,3495,3496,3496,3497,3498,3498,3499,3499,3500,3500,3501,3501,3502,3502,3503,3503,3504,3504,3505,3505,3506,3506,3507,3507,3508,3508,3509,3509,
                3510,3510,3511,3511,3512,3513,3513,3514,3515,3515,3516,3516,3517,3517,3518,3518,3519,3519,3520,3520,3521,3521,3522,3522,3523,3523,3524,3524,3525,3526,3526,3527,
                3528,3528,3529,3529,3530,3530,3531,3531,3532,3532,3533,3533,3534,3535,3535,3536,3537,3537,3538,3538,3539,3539,3540,3540,3541,3541,3542,3542,3543,3543,3544,3544,
                3545,3545,3546,3546,3547,3548,3548,3549,3550,3550,3551,3551,3552,3552,3553,3553,3554,3554,3555,3555,3556,3557,3557,3558,3559,3559,3560,3560,3561,3562,3562,3563,
                3564,3564,3565,3565,3566,3566,3567,3567,3568,3568,3569,3569,3570,3571,3571,3572,3573,3573,3574,3574,3575,3575,3576,3576,3577,3577,3578,3578,3579,3580,3580,3581,
                3582,3582,3583,3583,3584,3585,3585,3586,3587,3587,3588,3588,3589,3589,3590,3590,3591,3591,3592,3592,3593,3594,3594,3595,3596,3596,3597,3597,3598,3599,3599,3600,
                3601,3601,3602,3602,3603,3603,3604,3604,3605,3605,3606,3606,3607,3608,3608,3609,3610,3610,3611,3611,3612,3613,3613,3614,3615,3615,3616,3616,3617,3618,3618,3619,
                3620,3620,3621,3621,3622,3622,3623,3623,3624,3624,3625,3625,3626,3627,3627,3628,3629,3629,3630,3630,3631,3632,3632,3633,3634,3634,3635,3635,3636,3637,3637,3638,
                3639,3639,3640,3640,3641,3642,3642,3643,3644,3644,3645,3645,3646,3647,3647,3648,3649,3649,3650,3650,3651,3652,3652,3653,3654,3654,3655,3655,3656,3657,3657,3658,
                3659,3659,3660,3660,3661,3662,3662,3663,3664,3664,3665,3665,3666,3666,3667,3667,3668,3668,3669,3669,3670,3671,3671,3672,3673,3673,3674,3675,3676,3676,3677,3678,
                3679,3679,3680,3680,3681,3682,3682,3683,3684,3684,3685,3685,3686,3687,3687,3688,3689,3689,3690,3690,3691,3692,3692,3693,3694,3694,3695,3695,3696,3697,3697,3698,
                3699,3699,3700,3700,3701,3702,3702,3703,3704,3704,3705,3705,3706,3707,3707,3708,3709,3709,3710,3710,3711,3712,3712,3713,3714,3714,3715,3715,3716,3717,3717,3718,
                3719,3719,3720,3721,3722,3722,3723,3724,3725,3725,3726,3726,3727,3728,3728,3729,3730,3730,3731,3731,3732,3733,3733,3734,3735,3735,3736,3736,3737,3738,3738,3739,
                3740,3740,3741,3742,3743,3743,3744,3745,3746,3746,3747,3747,3748,3749,3749,3750,3751,3751,3752,3752,3753,3754,3754,3755,3756,3756,3757,3758,3759,3759,3760,3761,
                3762,3762,3763,3763,3764,3765,3765,3766,3767,3767,3768,3768,3769,3770,3770,3771,3772,3772,3773,3774,3775,3775,3776,3777,3778,3778,3779,3780,3781,3782,3783,3784,
                3785,3785,3786,3787,3788,3788,3789,3790,3791,3791,3792,3793,3794,3795,3796,3797,3798,3798,3799,3800,3801,3802,3803,3804,3805,3805,3806,3807,3808,3809,3810,3811,
                3812,3812,3813,3814,3815,3816,3817,3818,3819,3819,3820,3821,3822,3823,3824,3825,3826,3826,3827,3828,3829,3830,3831,3832,3833,3834,3835,3836,3837,3838,3839,3840,
                3841,3841,3842,3843,3844,3845,3846,3847,3848,3849,3850,3851,3852,3853,3854,3855,3856,3857,3858,3859,3860,3861,3862,3863,3864,3865,3866,3867,3868,3869,3870,3871,
                3872,3873,3874,3875,3876,3877,3878,3879,3880,3881,3882,3883,3884,3885,3886,3887,3888,3889,3890,3891,3892,3893,3894,3895,3896,3897,3898,3899,3900,3901,3902,3903,
                3904,3905,3906,3907,3908,3909,3910,3911,3912,3913,3914,3915,3916,3917,3918,3919,3921,3922,3923,3924,3925,3926,3927,3928,3930,3931,3932,3933,3934,3935,3936,3937,
                3938,3939,3940,3941,3942,3943,3944,3945,3947,3948,3949,3950,3951,3952,3953,3954,3956,3957,3958,3959,3960,3961,3962,3963,3965,3966,3967,3968,3969,3970,3971,3972,
                3974,3975,3976,3977,3978,3979,3980,3981,3983,3984,3985,3986,3988,3989,3990,3991,3993,3994,3995,3996,3997,3998,3999,4000,4002,4003,4004,4005,4007,4008,4009,4010,
                4012,4013,4014,4015,4017,4018,4019,4020,4022,4023,4024,4025,4026,4027,4028,4029,4031,4032,4033,4034,4036,4037,4038,4039,4041,4042,4043,4044,4046,4047,4048,4049,
                4051,4052,4053,4054,4056,4057,4058,4059,4061,4062,4063,4065,4066,4067,4069,4070,4072,4073,4074,4075,4077,4078,4079,4080,4082,4083,4084,4086,4087,4088,4090,4091,
          },
       },
       0,//exp_ratio_en
       0,//exp_ratio_inter_en
       0,//exp_ratio_count
       /* exp_ratio_trigger */
       {
       },
       /* isp_sub_module_gamma_lut_pra pra_exp_raion[MIDDLE_TRIGGER_COUNT] */
       {
       },
    },
    /* isp_sub_module_gtm2_tuning_t isp_sub_module_gtm2_tuning */
    {
       1,//enable
       1,//interpolation_enable
       5,//count_ae
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             110,
          },
          /* aec trigger 1 */
          {
             130,
             300,
          },
          /* aec trigger 2 */
          {
             320,
             350,
          },
          /* aec trigger 3 */
          {
             380,
             400,
          },
          /* aec trigger 4 */
          {
             410,
             440,
          },
       },
       /* isp_sub_module_gtm2_lut_pra pra[MAX_TRIGGER_COUNT] */
       {
          /* para 0 */
          {
             1,//enable_cdf_smooth
             0.01,//cut_ration
             0.1,//weight
             1,//ration
             1,//low_cut
             1,//enable_free_haho
             3,//filt_x
             3,//filt_y
             4,//param_filt
             64,//bin_meger
             1,//ltm_smooth
             1,//ltm_dehighlight
             0,//ltm_base
             0.4,//ltm_smooth_fork
             0.6,//ltm_smooth_spear
             1,//ltm_smooth_gamma
             0.8,//ltm_dehighlight_fork
             0.9,//ltm_dehighlight_spear
             2,//ltm_dehighlight_gamma
             0,//ltm_dark_fork
             0,//ltm_dark_spear
             0,//ltm_smooth_noise_erode_area
             0,//ltm_smooth_filter_area
             4,//soft_sample_rate
          },
          /* para 1 */
          {
             1,//enable_cdf_smooth
             0.006,//cut_ration
             0.1,//weight
             1,//ration
             1,//low_cut
             1,//enable_free_haho
             3,//filt_x
             3,//filt_y
             4,//param_filt
             64,//bin_meger
             1,//ltm_smooth
             1,//ltm_dehighlight
             0,//ltm_base
             0.4,//ltm_smooth_fork
             0.6,//ltm_smooth_spear
             1,//ltm_smooth_gamma
             0.8,//ltm_dehighlight_fork
             0.9,//ltm_dehighlight_spear
             2,//ltm_dehighlight_gamma
             0,//ltm_dark_fork
             0,//ltm_dark_spear
             0,//ltm_smooth_noise_erode_area
             0,//ltm_smooth_filter_area
             4,//soft_sample_rate
          },
          /* para 2 */
          {
             1,//enable_cdf_smooth
             0.003,//cut_ration
             0.15,//weight
             1,//ration
             1,//low_cut
             1,//enable_free_haho
             3,//filt_x
             3,//filt_y
             4,//param_filt
             64,//bin_meger
             1,//ltm_smooth
             1,//ltm_dehighlight
             0,//ltm_base
             0.4,//ltm_smooth_fork
             0.6,//ltm_smooth_spear
             1,//ltm_smooth_gamma
             0.85,//ltm_dehighlight_fork
             0.95,//ltm_dehighlight_spear
             2,//ltm_dehighlight_gamma
             0,//ltm_dark_fork
             0,//ltm_dark_spear
             0,//ltm_smooth_noise_erode_area
             0,//ltm_smooth_filter_area
             4,//soft_sample_rate
          },
          /* para 3 */
          {
             1,//enable_cdf_smooth
             0.001,//cut_ration
             0.15,//weight
             1,//ration
             1,//low_cut
             1,//enable_free_haho
             3,//filt_x
             3,//filt_y
             4,//param_filt
             64,//bin_meger
             0,//ltm_smooth
             0,//ltm_dehighlight
             0,//ltm_base
             0.4,//ltm_smooth_fork
             0.6,//ltm_smooth_spear
             1,//ltm_smooth_gamma
             0.85,//ltm_dehighlight_fork
             0.95,//ltm_dehighlight_spear
             2,//ltm_dehighlight_gamma
             0,//ltm_dark_fork
             0,//ltm_dark_spear
             0,//ltm_smooth_noise_erode_area
             0,//ltm_smooth_filter_area
             4,//soft_sample_rate
          },
          /* para 4 */
          {
             1,//enable_cdf_smooth
             0,//cut_ration
             0.15,//weight
             1,//ration
             1,//low_cut
             1,//enable_free_haho
             3,//filt_x
             3,//filt_y
             4,//param_filt
             64,//bin_meger
             0,//ltm_smooth
             0,//ltm_dehighlight
             0,//ltm_base
             0.4,//ltm_smooth_fork
             0.6,//ltm_smooth_spear
             1,//ltm_smooth_gamma
             0.85,//ltm_dehighlight_fork
             0.95,//ltm_dehighlight_spear
             2,//ltm_dehighlight_gamma
             0,//ltm_dark_fork
             0,//ltm_dark_spear
             0,//ltm_smooth_noise_erode_area
             0,//ltm_smooth_filter_area
             4,//soft_sample_rate
          },
       },
       1,//enable_tuning_mesh
       8,//mesh_w
       8,//mesh_h
    },
    /* isp_sub_module_3d_lut_tuning_t isp_sub_module_3d_lut_tuning */
    {
       0,//enable
       0,//interpolation_enable
       0,//count_ae
       0,//count_awb
       0,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
       },
       /* awb trigger */
       {
       },
       /* isp_sub_module_3d_lut_pra pra[MIN_TRIGGER_COUNT=3][MIN_TRIGGER_COUNT=3] */
       {
       },
    },
    /* isp_sub_module_rgbyuv_tuning_t isp_sub_module_rgbyuv_tuning */
    {
       1,//enable
    },
    /* isp_sub_module_cm_tuning_t isp_sub_module_cm_tuning */
    {
       1,//enable
       1,//interpolation_enable
       5,//count_ae
       7,//count_awb
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             180,
          },
          /* aec trigger 1 */
          {
             200,
             340,
          },
          /* aec trigger 2 */
          {
             380,
             430,
          },
          /* aec trigger 3 */
          {
             440,
             450,
          },
          /* aec trigger 4 */
          {
             470,
             550,
          },
       },
       /* awb trigger */
       {
          /* awb trigger 0 */
          {
             0,
             2000,
          },
          /* awb trigger 1 */
          {
             2900,
             3100,
          },
          /* awb trigger 2 */
          {
             3900,
             4000,
          },
          /* awb trigger 3 */
          {
             4900,
             5200,
          },
          /* awb trigger 4 */
          {
             5500,
             5700,
          },
          /* awb trigger 5 */
          {
             6400,
             6500,
          },
          /* awb trigger 6 */
          {
             7400,
             7500,
          },
       },
       /* isp_sub_module_cm_pra pra[MIDDLE_TRIGGER_COUNT=5][MIDDLE_TRIGGER_COUNT=7] */
       {
          /* aec trigger 0 */
          {
             /* awb para 0 */
             {
                1.5,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                1.5,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                1.5,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                1.5,//saturation
                0,//hue
             },
             /* awb para 4 */
             {
                1.5,//saturation
                0,//hue
             },
             /* awb para 5 */
             {
                1.5,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                1.5,//saturation
                0,//hue
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                1.5,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                1.5,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                1.5,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                1.5,//saturation
                0,//hue
             },
             /* awb para 4 */
             {
                1.5,//saturation
                0,//hue
             },
             /* awb para 5 */
             {
                1.5,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                1.5,//saturation
                0,//hue
             },
          },
          /* aec trigger 2 */
          {
             /* awb para 0 */
             {
                1.1,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                1.2,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                1.3,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                1.4,//saturation
                0,//hue
             },
             /* awb para 4 */
             {
                1.4,//saturation
                0,//hue
             },
             /* awb para 5 */
             {
                1.4,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                1.3,//saturation
                0,//hue
             },
          },
          /* aec trigger 3 */
          {
             /* awb para 0 */
             {
                1,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                1.1,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                1.2,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                1.3,//saturation
                0,//hue
             },
             /* awb para 4 */
             {
                1.3,//saturation
                0,//hue
             },
             /* awb para 5 */
             {
                1.3,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                1.2,//saturation
                0,//hue
             },
          },
          /* aec trigger 4 */
          {
             /* awb para 0 */
             {
                1,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                1,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                1,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                1,//saturation
                0,//hue
             },
             /* awb para 4 */
             {
                1,//saturation
                0,//hue
             },
             /* awb para 5 */
             {
                1,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                1,//saturation
                0,//hue
             },
          },
       },
    },
    /* isp_sub_module_lee_tuning_t isp_sub_module_lee_tuning */
    {
       1,//enable
       1,//interpolation_enable
       12,//count_ae
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             90,
          },
          /* aec trigger 1 */
          {
             110,
             220,
          },
          /* aec trigger 2 */
          {
             230,
             260,
          },
          /* aec trigger 3 */
          {
             260,
             280,
          },
          /* aec trigger 4 */
          {
             304,
             338,
          },
          /* aec trigger 5 */
          {
             347,
             348,
          },
          /* aec trigger 6 */
          {
             361,
             382,
          },
          /* aec trigger 7 */
          {
             392,
             395,
          },
          /* aec trigger 8 */
          {
             417,
             418,
          },
          /* aec trigger 9 */
          {
             441,
             442,
          },
          /* aec trigger 10 */
          {
             465,
             466,
          },
          /* aec trigger 11 */
          {
             500,
             501,
          },
       },
       /* isp_sub_module_lee_pra pra[MAX_TRIGGER_COUNT] */
       {
          /* para 0 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             1,//shrink enable
             1,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                -1,-2,-9,-13,//bpf(1,1:4)
                -2,-25,-114,-204,//bpf(2,1:4)
                -9,-114,-227,146,//bpf(3,1:4)
                -13,-204,146,2296,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             120,//strength_pos_edge
             120,//strength_neg_edge
             300,//overshoot_pos_edge
             -300,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             20,//limitation_for_move_strength
             111,//adjust_strength
             3,//coef_moving_c00
             21,//coef_moving_c01
             21,//coef_moving_c10
             160,//coef_moving_c11
             {0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
          /* para 1 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             1,//shrink enable
             1,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                -1,-4,-12,-16,//bpf(1,1:4)
                -4,-29,-114,-196,//bpf(2,1:4)
                -12,-114,-191,163,//bpf(3,1:4)
                -16,-196,163,2120,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             120,//strength_pos_edge
             120,//strength_neg_edge
             250,//overshoot_pos_edge
             -250,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {8,8,8,16,24,32,32,32,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             170,//adjust_strength
             3,//coef_moving_c00
             21,//coef_moving_c01
             21,//coef_moving_c10
             160,//coef_moving_c11
             {24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
          /* para 2 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             1,//shrink enable
             1,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                0,0,-4,-8,//bpf(1,1:4)
                0,-14,-80,-148,//bpf(2,1:4)
                -4,-80,-254,-84,//bpf(3,1:4)
                -8,-148,-84,2704,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             100,//strength_pos_edge
             100,//strength_neg_edge
             200,//overshoot_pos_edge
             -200,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,8,16,24,32,40,48,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             170,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
          /* para 3 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             1,//shrink enable
             1,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                0,-2,-10,-18,//bpf(1,1:4)
                -2,-30,-108,-174,//bpf(2,1:4)
                -10,-108,-238,276,//bpf(3,1:4)
                -18,-174,276,1696,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             90,//strength_pos_edge
             90,//strength_neg_edge
             120,//overshoot_pos_edge
             -150,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,0,8,16,24,32,32,32,32,32,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             220,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {16,24,32,40,48,48,48,48,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
          /* para 4 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             1,//shrink enable
             1,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                0,-2,-10,-18,//bpf(1,1:4)
                -2,-30,-108,-174,//bpf(2,1:4)
                -10,-108,-238,276,//bpf(3,1:4)
                -18,-174,276,1696,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             62,//strength_pos_edge
             72,//strength_neg_edge
             120,//overshoot_pos_edge
             -140,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,16,24,32,32,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             170,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
          /* para 5 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             1,//shrink enable
             1,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                0,-1,-4,-8,//bpf(1,1:4)
                -1,-12,-46,-87,//bpf(2,1:4)
                -4,-46,-109,-18,//bpf(3,1:4)
                -8,-87,-18,1344,//bpf(4,1:4)
             },
             {
                -1,-12,-46,-87,//hpf(1,1:4)
                -4,-46,-109,-18,//hpf(2,1:4)
                -8,-87,-18,1344,//hpf(3,1:4)
             },
             40,//strength_pos_edge
             60,//strength_neg_edge
             120,//overshoot_pos_edge
             -140,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,8,16,24,32,32,32,32,32,32,32,32,32,32,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             200,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
          /* para 6 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             1,//shrink enable
             1,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                0,-2,-14,-22,//bpf(1,1:4)
                -2,-34,-106,-156,//bpf(2,1:4)
                -14,-106,-226,346,//bpf(3,1:4)
                -22,-156,346,1344,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             45,//strength_pos_edge
             45,//strength_neg_edge
             180,//overshoot_pos_edge
             -200,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,8,16,24,32,40,40,40,40,40,40,40,40,40,40,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             200,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {32,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
          /* para 7 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             0,//shrink enable
             0,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                0,-2,-14,-22,//bpf(1,1:4)
                -2,-34,-106,-156,//bpf(2,1:4)
                -14,-106,-226,346,//bpf(3,1:4)
                -22,-156,346,1344,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             40,//strength_pos_edge
             50,//strength_neg_edge
             180,//overshoot_pos_edge
             -200,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,8,16,24,32,32,32,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             200,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
          /* para 8 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             1,//shrink enable
             1,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                0,-4,-16,-26,//bpf(1,1:4)
                -4,-40,-102,-138,//bpf(2,1:4)
                -16,-102,-212,418,//bpf(3,1:4)
                -26,-138,418,968,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             25,//strength_pos_edge
             35,//strength_neg_edge
             140,//overshoot_pos_edge
             -180,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             220,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
          /* para 9 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             1,//shrink enable
             1,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                0,-2,-12,-20,//bpf(1,1:4)
                -2,-30,-84,-114,//bpf(2,1:4)
                -12,-84,-220,206,//bpf(3,1:4)
                -20,-114,206,1496,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             20,//strength_pos_edge
             20,//strength_neg_edge
             180,//overshoot_pos_edge
             -180,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             255,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
          /* para 10 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             1,//shrink enable
             1,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                0,-4,-16,-26,//bpf(1,1:4)
                -4,-40,-102,-138,//bpf(2,1:4)
                -16,-102,-212,418,//bpf(3,1:4)
                -26,-138,418,968,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             15,//strength_pos_edge
             15,//strength_neg_edge
             180,//overshoot_pos_edge
             -180,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             255,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
          /* para 11 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             1,//shrink enable
             1,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                0,-2,-14,-24,//bpf(1,1:4)
                -2,-36,-94,-126,//bpf(2,1:4)
                -14,-94,-216,316,//bpf(3,1:4)
                -24,-126,316,1224,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             12,//strength_pos_edge
             12,//strength_neg_edge
             180,//overshoot_pos_edge
             -180,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             255,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
       },
    },
    /* isp_sub_module_cnf_tuning_t isp_sub_module_cnf_tuning */
    {
       1,//enable
       1,//interpolation_enable
       11,//count_ae
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* rgbyuv_cof */
       {
          256,//coef_r_y
          1025,//coef_r_cb
          358,//coef_r_cr
          256,//coef_g_y
          1113,//coef_g_cb
          1207,//coef_g_cr
          256,//coef_b_y
          453,//coef_b_cb
          0,//coef_b_cr
          0,//y_offset
          128,//cb_offset
          128,//cr_offset
       },
       256,//lut_size
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             235,
          },
          /* aec trigger 1 */
          {
             245,
             255,
          },
          /* aec trigger 2 */
          {
             270,
             280,
          },
          /* aec trigger 3 */
          {
             324,
             325,
          },
          /* aec trigger 4 */
          {
             347,
             348,
          },
          /* aec trigger 5 */
          {
             371,
             372,
          },
          /* aec trigger 6 */
          {
             394,
             395,
          },
          /* aec trigger 7 */
          {
             417,
             418,
          },
          /* aec trigger 8 */
          {
             441,
             442,
          },
          /* aec trigger 9 */
          {
             465,
             466,
          },
          /* aec trigger 10 */
          {
             500,
             501,
          },
       },
       /* isp_sub_module_cnf_pra pra[MAX_TRIGGER_COUNT] */
       {
          /* para 0 */
          {
             1,//enable
             1,//zoom_par
             0,//dn_level
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut0
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut1
          },
          /* para 1 */
          {
             1,//enable
             2,//zoom_par
             0,//dn_level
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut0
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut1
          },
          /* para 2 */
          {
             1,//enable
             3,//zoom_par
             0,//dn_level
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut0
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut1
          },
          /* para 3 */
          {
             1,//enable
             3,//zoom_par
             0,//dn_level
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut0
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut1
          },
          /* para 4 */
          {
             1,//enable
             3,//zoom_par
             0,//dn_level
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut0
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut1
          },
          /* para 5 */
          {
             1,//enable
             3,//zoom_par
             0,//dn_level
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut0
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut1
          },
          /* para 6 */
          {
             1,//enable
             4,//zoom_par
             0,//dn_level
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut0
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut1
          },
          /* para 7 */
          {
             1,//enable
             4,//zoom_par
             0,//dn_level
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut0
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut1
          },
          /* para 8 */
          {
             1,//enable
             5,//zoom_par
             0,//dn_level
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut0
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut1
          },
          /* para 9 */
          {
             1,//enable
             6,//zoom_par
             0,//dn_level
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut0
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut1
          },
          /* para 10 */
          {
             1,//enable
             6,//zoom_par
             0,//dn_level
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut0
             {
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             },//lut1
          },
       },
    },
    /* isp_sub_module_3d_denoise_tuning_t isp_sub_module_3d_denoise_tuning */
    {
       1,//enable
       1,//interpolation_enable
       11,//count_ae
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             90,
          },
          /* aec trigger 1 */
          {
             110,
             240,
          },
          /* aec trigger 2 */
          {
             250,
             280,
          },
          /* aec trigger 3 */
          {
             304,
             325,
          },
          /* aec trigger 4 */
          {
             339,
             350,
          },
          /* aec trigger 5 */
          {
             360,
             372,
          },
          /* aec trigger 6 */
          {
             384,
             395,
          },
          /* aec trigger 7 */
          {
             407,
             418,
          },
          /* aec trigger 8 */
          {
             431,
             452,
          },
          /* aec trigger 9 */
          {
             465,
             486,
          },
          /* aec trigger 10 */
          {
             500,
             501,
          },
       },
       /* isp_sub_module_3d_denoise_pra pra[MAX_TRIGGER_COUNT] */
       {
          /* para 0 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             10,//nr3d_npy_th1
             80,//nr3d_npy_th2
             12,//nr3d_mpy_th1
             18,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             15,//nr3d_mpc_th1
             25,//nr3d_mpc_th2
             15,//nr3d_lamda2d
             15,//nr3d_ite
             0,//nr3d_msr
             232,//nr3d_decay
             10,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             1,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             252,//nr3d_gaus_y_c23
             16383,//nr3d_tauhard_th
             0,//md_blk_ave_sel
             0,//bypass_saturation_adjust
             0,//post_process_y_filter_sel
             39,//pixel_weight_filter_c11
             57,//pixel_weight_filter_c12
             64,//pixel_weight_filter_c13
             0,//disable_1st_media_filter
             1,//dbk_h_en
             1,//dbk_v_en
             0,//dbk_wr2ddr
             0,//dbk_bypass
             8,//dbk_h_mv_diff
             12,//dbk_h_mv_str
             8,//dbk_v_mv_diff
             12,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             20,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             20,//dbk_v_yy_diff
             30,//dbk_satu
             0,//dbk_gaus_y_c11
             0,//dbk_gaus_y_c12
             4,//dbk_gaus_y_c13
             0,//dbk_gaus_y_c21
             240,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                1,2,2,2,3,4,5,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,2,4,6,8,10,12,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             16,//dbk_h_ed_thrd
             16,//dbk_v_ed_thrd
          },
          /* para 1 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             10,//nr3d_npy_th1
             80,//nr3d_npy_th2
             12,//nr3d_mpy_th1
             25,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             15,//nr3d_mpc_th1
             25,//nr3d_mpc_th2
             15,//nr3d_lamda2d
             18,//nr3d_ite
             0,//nr3d_msr
             232,//nr3d_decay
             10,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             1,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             252,//nr3d_gaus_y_c23
             16383,//nr3d_tauhard_th
             0,//md_blk_ave_sel
             0,//bypass_saturation_adjust
             0,//post_process_y_filter_sel
             39,//pixel_weight_filter_c11
             57,//pixel_weight_filter_c12
             64,//pixel_weight_filter_c13
             1,//disable_1st_media_filter
             1,//dbk_h_en
             1,//dbk_v_en
             0,//dbk_wr2ddr
             0,//dbk_bypass
             12,//dbk_h_mv_diff
             16,//dbk_h_mv_str
             12,//dbk_v_mv_diff
             16,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             16,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             16,//dbk_v_yy_diff
             15,//dbk_satu
             0,//dbk_gaus_y_c11
             3,//dbk_gaus_y_c12
             21,//dbk_gaus_y_c13
             0,//dbk_gaus_y_c21
             160,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,1,2,3,4,5,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             16,//dbk_h_ed_thrd
             16,//dbk_v_ed_thrd
          },
          /* para 2 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             10,//nr3d_npy_th1
             80,//nr3d_npy_th2
             12,//nr3d_mpy_th1
             25,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             20,//nr3d_mpc_th1
             30,//nr3d_mpc_th2
             15,//nr3d_lamda2d
             20,//nr3d_ite
             0,//nr3d_msr
             232,//nr3d_decay
             10,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             1,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             252,//nr3d_gaus_y_c23
             16383,//nr3d_tauhard_th
             0,//md_blk_ave_sel
             0,//bypass_saturation_adjust
             0,//post_process_y_filter_sel
             39,//pixel_weight_filter_c11
             57,//pixel_weight_filter_c12
             64,//pixel_weight_filter_c13
             1,//disable_1st_media_filter
             1,//dbk_h_en
             1,//dbk_v_en
             0,//dbk_wr2ddr
             0,//dbk_bypass
             14,//dbk_h_mv_diff
             16,//dbk_h_mv_str
             14,//dbk_v_mv_diff
             16,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             20,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             20,//dbk_v_yy_diff
             10,//dbk_satu
             0,//dbk_gaus_y_c11
             5,//dbk_gaus_y_c12
             26,//dbk_gaus_y_c13
             0,//dbk_gaus_y_c21
             132,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,1,2,3,4,5,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,8,8,8,8,8,8,8,8,8,8,8,8,8,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             16,//dbk_h_ed_thrd
             16,//dbk_v_ed_thrd
          },
          /* para 3 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             15,//nr3d_npy_th1
             80,//nr3d_npy_th2
             20,//nr3d_mpy_th1
             40,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             20,//nr3d_mpc_th1
             30,//nr3d_mpc_th2
             15,//nr3d_lamda2d
             25,//nr3d_ite
             0,//nr3d_msr
             232,//nr3d_decay
             10,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             1,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             252,//nr3d_gaus_y_c23
             16383,//nr3d_tauhard_th
             0,//md_blk_ave_sel
             0,//bypass_saturation_adjust
             0,//post_process_y_filter_sel
             39,//pixel_weight_filter_c11
             57,//pixel_weight_filter_c12
             64,//pixel_weight_filter_c13
             1,//disable_1st_media_filter
             1,//dbk_h_en
             1,//dbk_v_en
             0,//dbk_wr2ddr
             0,//dbk_bypass
             14,//dbk_h_mv_diff
             20,//dbk_h_mv_str
             14,//dbk_v_mv_diff
             20,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             20,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             20,//dbk_v_yy_diff
             20,//dbk_satu
             1,//dbk_gaus_y_c11
             11,//dbk_gaus_y_c12
             31,//dbk_gaus_y_c13
             1,//dbk_gaus_y_c21
             82,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,12,13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             16,//dbk_h_ed_thrd
             16,//dbk_v_ed_thrd
          },
          /* para 4 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             20,//nr3d_npy_th1
             80,//nr3d_npy_th2
             30,//nr3d_mpy_th1
             60,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             20,//nr3d_mpc_th1
             40,//nr3d_mpc_th2
             15,//nr3d_lamda2d
             28,//nr3d_ite
             0,//nr3d_msr
             232,//nr3d_decay
             10,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             1,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             252,//nr3d_gaus_y_c23
             16383,//nr3d_tauhard_th
             0,//md_blk_ave_sel
             0,//bypass_saturation_adjust
             0,//post_process_y_filter_sel
             39,//pixel_weight_filter_c11
             57,//pixel_weight_filter_c12
             64,//pixel_weight_filter_c13
             1,//disable_1st_media_filter
             1,//dbk_h_en
             1,//dbk_v_en
             0,//dbk_wr2ddr
             0,//dbk_bypass
             14,//dbk_h_mv_diff
             20,//dbk_h_mv_str
             14,//dbk_v_mv_diff
             20,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             35,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             35,//dbk_v_yy_diff
             20,//dbk_satu
             1,//dbk_gaus_y_c11
             14,//dbk_gaus_y_c12
             31,//dbk_gaus_y_c13
             3,//dbk_gaus_y_c21
             66,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,0,0,0,1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             16,//dbk_h_ed_thrd
             16,//dbk_v_ed_thrd
          },
          /* para 5 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             25,//nr3d_npy_th1
             80,//nr3d_npy_th2
             30,//nr3d_mpy_th1
             60,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             25,//nr3d_mpc_th1
             40,//nr3d_mpc_th2
             15,//nr3d_lamda2d
             28,//nr3d_ite
             0,//nr3d_msr
             250,//nr3d_decay
             10,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             1,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             252,//nr3d_gaus_y_c23
             16383,//nr3d_tauhard_th
             0,//md_blk_ave_sel
             0,//bypass_saturation_adjust
             0,//post_process_y_filter_sel
             39,//pixel_weight_filter_c11
             57,//pixel_weight_filter_c12
             64,//pixel_weight_filter_c13
             1,//disable_1st_media_filter
             1,//dbk_h_en
             1,//dbk_v_en
             0,//dbk_wr2ddr
             0,//dbk_bypass
             16,//dbk_h_mv_diff
             32,//dbk_h_mv_str
             16,//dbk_v_mv_diff
             32,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             40,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             40,//dbk_v_yy_diff
             20,//dbk_satu
             2,//dbk_gaus_y_c11
             15,//dbk_gaus_y_c12
             30,//dbk_gaus_y_c13
             4,//dbk_gaus_y_c21
             60,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,0,0,0,0,1,2,4,5,6,7,8,9,11,12,13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             16,//dbk_h_ed_thrd
             16,//dbk_v_ed_thrd
          },
          /* para 6 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             40,//nr3d_npy_th1
             110,//nr3d_npy_th2
             30,//nr3d_mpy_th1
             60,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             30,//nr3d_mpc_th1
             55,//nr3d_mpc_th2
             15,//nr3d_lamda2d
             28,//nr3d_ite
             0,//nr3d_msr
             250,//nr3d_decay
             10,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             1,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             252,//nr3d_gaus_y_c23
             16383,//nr3d_tauhard_th
             0,//md_blk_ave_sel
             0,//bypass_saturation_adjust
             0,//post_process_y_filter_sel
             39,//pixel_weight_filter_c11
             57,//pixel_weight_filter_c12
             64,//pixel_weight_filter_c13
             0,//disable_1st_media_filter
             1,//dbk_h_en
             1,//dbk_v_en
             0,//dbk_wr2ddr
             0,//dbk_bypass
             24,//dbk_h_mv_diff
             50,//dbk_h_mv_str
             24,//dbk_v_mv_diff
             50,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             43,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             43,//dbk_v_yy_diff
             20,//dbk_satu
             2,//dbk_gaus_y_c11
             15,//dbk_gaus_y_c12
             30,//dbk_gaus_y_c13
             4,//dbk_gaus_y_c21
             60,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,0,0,0,0,0,1,2,3,4,5,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             16,//dbk_h_ed_thrd
             16,//dbk_v_ed_thrd
          },
          /* para 7 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             65,//nr3d_npy_th1
             140,//nr3d_npy_th2
             40,//nr3d_mpy_th1
             70,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             33,//nr3d_mpc_th1
             51,//nr3d_mpc_th2
             15,//nr3d_lamda2d
             30,//nr3d_ite
             0,//nr3d_msr
             250,//nr3d_decay
             10,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             1,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             252,//nr3d_gaus_y_c23
             16383,//nr3d_tauhard_th
             0,//md_blk_ave_sel
             0,//bypass_saturation_adjust
             0,//post_process_y_filter_sel
             39,//pixel_weight_filter_c11
             57,//pixel_weight_filter_c12
             64,//pixel_weight_filter_c13
             0,//disable_1st_media_filter
             1,//dbk_h_en
             1,//dbk_v_en
             0,//dbk_wr2ddr
             0,//dbk_bypass
             30,//dbk_h_mv_diff
             50,//dbk_h_mv_str
             30,//dbk_v_mv_diff
             50,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             50,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             50,//dbk_v_yy_diff
             20,//dbk_satu
             2,//dbk_gaus_y_c11
             15,//dbk_gaus_y_c12
             30,//dbk_gaus_y_c13
             4,//dbk_gaus_y_c21
             60,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,0,0,0,0,0,0,0,0,1,2,3,4,4,5,6,7,8,8,9,10,11,12,12,13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             16,//dbk_h_ed_thrd
             16,//dbk_v_ed_thrd
          },
          /* para 8 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             80,//nr3d_npy_th1
             160,//nr3d_npy_th2
             50,//nr3d_mpy_th1
             70,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             60,//nr3d_mpc_th1
             70,//nr3d_mpc_th2
             20,//nr3d_lamda2d
             35,//nr3d_ite
             0,//nr3d_msr
             250,//nr3d_decay
             10,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             1,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             252,//nr3d_gaus_y_c23
             16383,//nr3d_tauhard_th
             0,//md_blk_ave_sel
             0,//bypass_saturation_adjust
             0,//post_process_y_filter_sel
             39,//pixel_weight_filter_c11
             57,//pixel_weight_filter_c12
             64,//pixel_weight_filter_c13
             0,//disable_1st_media_filter
             1,//dbk_h_en
             1,//dbk_v_en
             0,//dbk_wr2ddr
             0,//dbk_bypass
             35,//dbk_h_mv_diff
             55,//dbk_h_mv_str
             35,//dbk_v_mv_diff
             55,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             55,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             55,//dbk_v_yy_diff
             30,//dbk_satu
             2,//dbk_gaus_y_c11
             15,//dbk_gaus_y_c12
             30,//dbk_gaus_y_c13
             4,//dbk_gaus_y_c21
             60,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             16,//dbk_h_ed_thrd
             16,//dbk_v_ed_thrd
          },
          /* para 9 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             85,//nr3d_npy_th1
             170,//nr3d_npy_th2
             70,//nr3d_mpy_th1
             90,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             60,//nr3d_mpc_th1
             80,//nr3d_mpc_th2
             35,//nr3d_lamda2d
             30,//nr3d_ite
             0,//nr3d_msr
             250,//nr3d_decay
             10,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             1,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             252,//nr3d_gaus_y_c23
             16383,//nr3d_tauhard_th
             0,//md_blk_ave_sel
             0,//bypass_saturation_adjust
             0,//post_process_y_filter_sel
             39,//pixel_weight_filter_c11
             57,//pixel_weight_filter_c12
             64,//pixel_weight_filter_c13
             0,//disable_1st_media_filter
             1,//dbk_h_en
             1,//dbk_v_en
             0,//dbk_wr2ddr
             0,//dbk_bypass
             40,//dbk_h_mv_diff
             60,//dbk_h_mv_str
             40,//dbk_v_mv_diff
             60,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             60,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             60,//dbk_v_yy_diff
             40,//dbk_satu
             2,//dbk_gaus_y_c11
             15,//dbk_gaus_y_c12
             30,//dbk_gaus_y_c13
             4,//dbk_gaus_y_c21
             60,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,4,5,6,7,8,8,9,10,11,12,12,13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             16,//dbk_h_ed_thrd
             16,//dbk_v_ed_thrd
          },
          /* para 10 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             100,//nr3d_npy_th1
             200,//nr3d_npy_th2
             60,//nr3d_mpy_th1
             120,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             60,//nr3d_mpc_th1
             110,//nr3d_mpc_th2
             30,//nr3d_lamda2d
             40,//nr3d_ite
             0,//nr3d_msr
             250,//nr3d_decay
             20,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             1,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             252,//nr3d_gaus_y_c23
             16383,//nr3d_tauhard_th
             0,//md_blk_ave_sel
             0,//bypass_saturation_adjust
             0,//post_process_y_filter_sel
             39,//pixel_weight_filter_c11
             57,//pixel_weight_filter_c12
             64,//pixel_weight_filter_c13
             0,//disable_1st_media_filter
             1,//dbk_h_en
             1,//dbk_v_en
             0,//dbk_wr2ddr
             0,//dbk_bypass
             40,//dbk_h_mv_diff
             60,//dbk_h_mv_str
             40,//dbk_v_mv_diff
             60,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             60,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             60,//dbk_v_yy_diff
             50,//dbk_satu
             2,//dbk_gaus_y_c11
             15,//dbk_gaus_y_c12
             30,//dbk_gaus_y_c13
             4,//dbk_gaus_y_c21
             60,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,4,5,6,7,8,8,9,10,11,12,12,13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             16,//dbk_h_ed_thrd
             16,//dbk_v_ed_thrd
          },
       },
    },
    /* isp_sub_module_dithering_tuning_t isp_sub_module_dithering_tuning */
    {
       0,//enable
    },
    /* isp_sub_module_isp_vfe_tuning_t isp_sub_module_isp_vfe_tuning */
    {
       1,//enable
       1,//interpolation_enable
       6,//count_ae
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             90,
          },
          /* aec trigger 1 */
          {
             100,
             150,
          },
          /* aec trigger 2 */
          {
             160,
             300,
          },
          /* aec trigger 3 */
          {
             310,
             400,
          },
          /* aec trigger 4 */
          {
             410,
             440,
          },
          /* aec trigger 5 */
          {
             450,
             500,
          },
       },
       /* isp_sub_module_isp_vfe_tuning pra[MAX_TRIGGER_COUNT] */
       {
          /* para 0 */
          {
             /* isp_sub_module_hdr_mix_pra */
             {
                256,//blc_short_exp
                256,//blc_mid_exp
                256,//blc_long_exp
                0,//hdr_motion_en_sm
                25,//hdr_alpha_s_th_sm
                867,//hdr_exp_value_th1_sm
                682,//hdr_th2_th1_reverse_sm
                64,//hdr_noise_sp_sm
                76,//hdr_motion_mp_sm
                1,//hdr_motion_en_sl
                0,//hdr_alpha_s_th_sl
                200,//hdr_exp_value_th1_sl
                333,//hdr_th2_th1_reverse_sl
                0,//hdr_noise_sp_sl
                0,//hdr_motion_mp_sl
                32,//hdr_md_th4_sl
                27,//hdr_md_th9_sl
                19,//hdr_md_th16_sl
                15,//hdr_md_th25_sl
                /* noise_profile_l[129] */
                {
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                /* noise_profile_m[129] */
                {
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                /* noise_profile_s[129] */
                {
                    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
                    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                },
                0,//hdr_policy
                1,//enable_hdr_blc
                60,//mv_fix_th
                16,//mv_fix_value
                1,//mv_fix_en
                0,//use_long_exp_fix
                1,//use_mv_fix_value_fix
                1,//enable_sexp_denoise
                /* gaus_coef[4] */
                {
                   0,10,10,216,
                },
             },
             /* STRU_AI_ISP_PRA_T */
             {
                {""},//net_name
                0,//scale
                {
                   0,//k_coeff[0]
                   0,//k_coeff[1]
                },
                {
                   0,//b_coeff[0]
                   0,//b_coeff[1]
                   0,//b_coeff[2]
                },
                /* blc */
                {0,0,0,0},
             },
          },
          /* para 1 */
          {
             /* isp_sub_module_hdr_mix_pra */
             {
                256,//blc_short_exp
                240,//blc_mid_exp
                256,//blc_long_exp
                0,//hdr_motion_en_sm
                25,//hdr_alpha_s_th_sm
                867,//hdr_exp_value_th1_sm
                682,//hdr_th2_th1_reverse_sm
                64,//hdr_noise_sp_sm
                76,//hdr_motion_mp_sm
                1,//hdr_motion_en_sl
                0,//hdr_alpha_s_th_sl
                133,//hdr_exp_value_th1_sl
                444,//hdr_th2_th1_reverse_sl
                0,//hdr_noise_sp_sl
                0,//hdr_motion_mp_sl
                32,//hdr_md_th4_sl
                27,//hdr_md_th9_sl
                19,//hdr_md_th16_sl
                15,//hdr_md_th25_sl
                /* noise_profile_l[129] */
                {
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                /* noise_profile_m[129] */
                {
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                /* noise_profile_s[129] */
                {
                    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
                    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                },
                0,//hdr_policy
                1,//enable_hdr_blc
                60,//mv_fix_th
                16,//mv_fix_value
                1,//mv_fix_en
                0,//use_long_exp_fix
                1,//use_mv_fix_value_fix
                1,//enable_sexp_denoise
                /* gaus_coef[4] */
                {
                   0,10,10,216,
                },
             },
             /* STRU_AI_ISP_PRA_T */
             {
                {""},//net_name
                0,//scale
                {
                   0,//k_coeff[0]
                   0,//k_coeff[1]
                },
                {
                   0,//b_coeff[0]
                   0,//b_coeff[1]
                   0,//b_coeff[2]
                },
                /* blc */
                {0,0,0,0},
             },
          },
          /* para 2 */
          {
             /* isp_sub_module_hdr_mix_pra */
             {
                255,//blc_short_exp
                240,//blc_mid_exp
                255,//blc_long_exp
                0,//hdr_motion_en_sm
                25,//hdr_alpha_s_th_sm
                867,//hdr_exp_value_th1_sm
                682,//hdr_th2_th1_reverse_sm
                64,//hdr_noise_sp_sm
                76,//hdr_motion_mp_sm
                1,//hdr_motion_en_sl
                0,//hdr_alpha_s_th_sl
                88,//hdr_exp_value_th1_sl
                388,//hdr_th2_th1_reverse_sl
                0,//hdr_noise_sp_sl
                4,//hdr_motion_mp_sl
                32,//hdr_md_th4_sl
                27,//hdr_md_th9_sl
                19,//hdr_md_th16_sl
                15,//hdr_md_th25_sl
                /* noise_profile_l[129] */
                {
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                /* noise_profile_m[129] */
                {
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                /* noise_profile_s[129] */
                {
                    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
                    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                },
                0,//hdr_policy
                1,//enable_hdr_blc
                60,//mv_fix_th
                16,//mv_fix_value
                1,//mv_fix_en
                0,//use_long_exp_fix
                1,//use_mv_fix_value_fix
                1,//enable_sexp_denoise
                /* gaus_coef[4] */
                {
                   0,10,10,216,
                },
             },
             /* STRU_AI_ISP_PRA_T */
             {
                {""},//net_name
                0,//scale
                {
                   0,//k_coeff[0]
                   0,//k_coeff[1]
                },
                {
                   0,//b_coeff[0]
                   0,//b_coeff[1]
                   0,//b_coeff[2]
                },
                /* blc */
                {0,0,0,0},
             },
          },
          /* para 3 */
          {
             /* isp_sub_module_hdr_mix_pra */
             {
                255,//blc_short_exp
                240,//blc_mid_exp
                255,//blc_long_exp
                0,//hdr_motion_en_sm
                25,//hdr_alpha_s_th_sm
                867,//hdr_exp_value_th1_sm
                682,//hdr_th2_th1_reverse_sm
                64,//hdr_noise_sp_sm
                76,//hdr_motion_mp_sm
                1,//hdr_motion_en_sl
                0,//hdr_alpha_s_th_sl
                24,//hdr_exp_value_th1_sl
                256,//hdr_th2_th1_reverse_sl
                0,//hdr_noise_sp_sl
                0,//hdr_motion_mp_sl
                32,//hdr_md_th4_sl
                27,//hdr_md_th9_sl
                18,//hdr_md_th16_sl
                15,//hdr_md_th25_sl
                /* noise_profile_l[129] */
                {
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                /* noise_profile_m[129] */
                {
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                /* noise_profile_s[129] */
                {
                    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
                    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                },
                0,//hdr_policy
                1,//enable_hdr_blc
                60,//mv_fix_th
                16,//mv_fix_value
                1,//mv_fix_en
                0,//use_long_exp_fix
                1,//use_mv_fix_value_fix
                1,//enable_sexp_denoise
                /* gaus_coef[4] */
                {
                   0,10,10,216,
                },
             },
             /* STRU_AI_ISP_PRA_T */
             {
                {""},//net_name
                0,//scale
                {
                   0,//k_coeff[0]
                   0,//k_coeff[1]
                },
                {
                   0,//b_coeff[0]
                   0,//b_coeff[1]
                   0,//b_coeff[2]
                },
                /* blc */
                {0,0,0,0},
             },
          },
          /* para 4 */
          {
             /* isp_sub_module_hdr_mix_pra */
             {
                256,//blc_short_exp
                240,//blc_mid_exp
                256,//blc_long_exp
                0,//hdr_motion_en_sm
                25,//hdr_alpha_s_th_sm
                867,//hdr_exp_value_th1_sm
                682,//hdr_th2_th1_reverse_sm
                64,//hdr_noise_sp_sm
                76,//hdr_motion_mp_sm
                1,//hdr_motion_en_sl
                0,//hdr_alpha_s_th_sl
                16,//hdr_exp_value_th1_sl
                128,//hdr_th2_th1_reverse_sl
                0,//hdr_noise_sp_sl
                0,//hdr_motion_mp_sl
                32,//hdr_md_th4_sl
                27,//hdr_md_th9_sl
                19,//hdr_md_th16_sl
                15,//hdr_md_th25_sl
                /* noise_profile_l[129] */
                {
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                /* noise_profile_m[129] */
                {
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                /* noise_profile_s[129] */
                {
                    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
                    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                },
                0,//hdr_policy
                1,//enable_hdr_blc
                60,//mv_fix_th
                16,//mv_fix_value
                1,//mv_fix_en
                0,//use_long_exp_fix
                1,//use_mv_fix_value_fix
                1,//enable_sexp_denoise
                /* gaus_coef[4] */
                {
                   0,10,10,216,
                },
             },
             /* STRU_AI_ISP_PRA_T */
             {
                {""},//net_name
                0,//scale
                {
                   0,//k_coeff[0]
                   0,//k_coeff[1]
                },
                {
                   0,//b_coeff[0]
                   0,//b_coeff[1]
                   0,//b_coeff[2]
                },
                /* blc */
                {0,0,0,0},
             },
          },
          /* para 5 */
          {
             /* isp_sub_module_hdr_mix_pra */
             {
                266,//blc_short_exp
                262,//blc_mid_exp
                266,//blc_long_exp
                0,//hdr_motion_en_sm
                25,//hdr_alpha_s_th_sm
                867,//hdr_exp_value_th1_sm
                682,//hdr_th2_th1_reverse_sm
                64,//hdr_noise_sp_sm
                76,//hdr_motion_mp_sm
                1,//hdr_motion_en_sl
                0,//hdr_alpha_s_th_sl
                16,//hdr_exp_value_th1_sl
                512,//hdr_th2_th1_reverse_sl
                0,//hdr_noise_sp_sl
                0,//hdr_motion_mp_sl
                32,//hdr_md_th4_sl
                27,//hdr_md_th9_sl
                19,//hdr_md_th16_sl
                15,//hdr_md_th25_sl
                /* noise_profile_l[129] */
                {
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                /* noise_profile_m[129] */
                {
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                /* noise_profile_s[129] */
                {
                    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
                    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                },
                0,//hdr_policy
                1,//enable_hdr_blc
                60,//mv_fix_th
                16,//mv_fix_value
                1,//mv_fix_en
                0,//use_long_exp_fix
                1,//use_mv_fix_value_fix
                1,//enable_sexp_denoise
                /* gaus_coef[4] */
                {
                   0,10,10,216,
                },
             },
             /* STRU_AI_ISP_PRA_T */
             {
                {""},//net_name
                0,//scale
                {
                   0,//k_coeff[0]
                   0,//k_coeff[1]
                },
                {
                   0,//b_coeff[0]
                   0,//b_coeff[1]
                   0,//b_coeff[2]
                },
                /* blc */
                {0,0,0,0},
             },
          },
       },
    },
    /* isp_sub_module_scaler_tuning_t isp_sub_module_scaler_tuning */
    {
       0,//enable
    },
    /* isp_sub_module_eis_ldc_tuning_t isp_sub_module_eis_ldc_tuning */
    {
       0,//enable
    },
    /* isp_sub_module_cm2_tuning_t isp_sub_module_cm2_tuning */
    {
       1,//enable
       1,//interpolation_enable
       5,//count_ae
       1,//count_awb
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             160,
          },
          /* aec trigger 1 */
          {
             180,
             290,
          },
          /* aec trigger 2 */
          {
             310,
             340,
          },
          /* aec trigger 3 */
          {
             350,
             390,
          },
          /* aec trigger 4 */
          {
             410,
             500,
          },
       },
       /* awb trigger */
       {
          /* awb trigger 0 */
          {
             0,
             8000,
          },
       },
       /* isp_sub_module_cm_pra pra[MIDDLE_TRIGGER_COUNT=5][MIDDLE_TRIGGER_COUNT=7] */
       {
          /* aec trigger 0 */
          {
             /* awb para 0 */
             {
                1.3,//saturation
                0,//hue
                20,//y_lo_th1
                100,//y_lo_th2
                600,//y_hi_th1
                1023,//y_hi_th2
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                1.5,//saturation
                0,//hue
                20,//y_lo_th1
                100,//y_lo_th2
                600,//y_hi_th1
                1023,//y_hi_th2
             },
          },
          /* aec trigger 2 */
          {
             /* awb para 0 */
             {
                1.2,//saturation
                0,//hue
                20,//y_lo_th1
                100,//y_lo_th2
                950,//y_hi_th1
                1023,//y_hi_th2
             },
          },
          /* aec trigger 3 */
          {
             /* awb para 0 */
             {
                1,//saturation
                0,//hue
                20,//y_lo_th1
                100,//y_lo_th2
                900,//y_hi_th1
                1023,//y_hi_th2
             },
          },
          /* aec trigger 4 */
          {
             /* awb para 0 */
             {
                0.7,//saturation
                0,//hue
                20,//y_lo_th1
                100,//y_lo_th2
                900,//y_hi_th1
                1023,//y_hi_th2
             },
          },
       },
    },
    /* isp_sub_module_lsc_pra isp_sub_module_hdr_lsc_tuning */
    {
       0,//enable
       1,//interpolation_enable
       2,//count_ae
       4,//count_awb
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             300,
          },
          /* aec trigger 1 */
          {
             390,
             500,
          },
       },
       /* awb trigger */
       {
          /* awb trigger 0 */
          {
             2700,
             2900,
          },
          /* awb trigger 1 */
          {
             3700,
             4300,
          },
          /* awb trigger 2 */
          {
             4700,
             5300,
          },
          /* awb trigger 3 */
          {
             6200,
             6800,
          },
       },
       /* isp_sub_module_lsc_pra */
       {
          /* aec trigger 0 */
          {
             /* awb para 0 */
             {
                0.3,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.1992, 2.3433, 1.9121, 1.5298, 1.4131, 1.4233, 1.5537, 1.9023, 2.4263, 4.1050, 
                   3.1421, 2.0850, 1.6104, 1.3647, 1.2305, 1.2461, 1.3516, 1.6318, 2.1030, 3.1899, 
                   2.8296, 1.8955, 1.4897, 1.2407, 1.1377, 1.1323, 1.2539, 1.4883, 1.9395, 2.8428, 
                   2.5220, 1.7749, 1.3657, 1.1577, 1.0308, 1.0361, 1.1577, 1.3940, 1.7847, 2.6113, 
                   2.4526, 1.6831, 1.3301, 1.0957, 0.9961, 1.0024, 1.1006, 1.3301, 1.7251, 2.4331, 
                   2.4526, 1.6948, 1.3179, 1.1001, 0.9995, 0.9907, 1.1050, 1.3281, 1.7026, 2.4526, 
                   2.4761, 1.6992, 1.3330, 1.1157, 0.9922, 0.9971, 1.1094, 1.3228, 1.7305, 2.4478, 
                   2.6470, 1.7856, 1.4067, 1.1729, 1.0654, 1.0669, 1.2046, 1.4087, 1.8306, 2.6416, 
                   2.8472, 1.9229, 1.5029, 1.2622, 1.1470, 1.1445, 1.2500, 1.4946, 1.9546, 2.8765, 
                   3.3706, 2.1104, 1.6577, 1.3687, 1.2622, 1.2603, 1.4233, 1.6392, 2.1929, 3.2603, 
                   3.7437, 2.2642, 1.7798, 1.4980, 1.3921, 1.3823, 1.4985, 1.8442, 2.2637, 3.9390, 
                   3.0400, 1.9692, 1.5884, 1.3345, 1.2163, 1.2207, 1.3511, 1.5649, 2.0352, 2.9741, 
                   2.6152, 1.8320, 1.4404, 1.2212, 1.1313, 1.1309, 1.2285, 1.4644, 1.8540, 2.7119, 
                   2.3862, 1.7114, 1.3472, 1.1450, 1.0313, 1.0356, 1.1504, 1.3643, 1.7402, 2.4795, 
                   2.3403, 1.6436, 1.3008, 1.0933, 0.9976, 0.9878, 1.1060, 1.2998, 1.6680, 2.3350, 
                   2.2754, 1.6333, 1.2959, 1.0869, 0.9897, 0.9985, 1.0933, 1.3110, 1.6694, 2.3560, 
                   2.3408, 1.6431, 1.3013, 1.1055, 0.9888, 0.9937, 1.1084, 1.3154, 1.6753, 2.3394, 
                   2.4731, 1.7427, 1.3721, 1.1719, 1.0522, 1.0625, 1.1563, 1.3931, 1.7476, 2.5264, 
                   2.6890, 1.8286, 1.4668, 1.2324, 1.1372, 1.1460, 1.2505, 1.4727, 1.8833, 2.7197, 
                   3.1987, 2.0332, 1.6094, 1.3706, 1.2510, 1.2393, 1.3496, 1.6387, 2.0278, 3.1948, 
                   3.7358, 2.1665, 1.7969, 1.4893, 1.3394, 1.3931, 1.4990, 1.8223, 2.3691, 3.6719, 
                   2.8809, 1.9331, 1.5830, 1.3003, 1.2417, 1.2202, 1.3589, 1.5513, 2.0581, 2.8774, 
                   2.5972, 1.7915, 1.4209, 1.2251, 1.1172, 1.1396, 1.2104, 1.4858, 1.8511, 2.7061, 
                   2.3564, 1.6665, 1.3550, 1.1245, 1.0332, 1.0527, 1.1563, 1.3613, 1.7632, 2.4351, 
                   2.2788, 1.6338, 1.2773, 1.0908, 1.0044, 1.0063, 1.1016, 1.3271, 1.6738, 2.3555, 
                   2.2368, 1.6030, 1.3018, 1.0752, 1.0098, 0.9995, 1.1152, 1.3193, 1.6763, 2.3662, 
                   2.3076, 1.6309, 1.2998, 1.1104, 0.9790, 1.0112, 1.0996, 1.3164, 1.6553, 2.3730, 
                   2.4287, 1.7217, 1.3726, 1.1621, 1.0713, 1.0654, 1.1792, 1.3955, 1.7524, 2.5547, 
                   2.6240, 1.8091, 1.4648, 1.2363, 1.1387, 1.1479, 1.2310, 1.4722, 1.8638, 2.6611, 
                   3.0371, 2.0659, 1.5962, 1.3877, 1.2427, 1.2788, 1.3477, 1.6167, 2.0439, 3.1831, 
                },
             },
             /* awb para 1 */
             {
                0.3,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   3.8623, 2.3374, 1.7646, 1.5356, 1.3662, 1.4063, 1.5190, 1.8076, 2.3306, 3.7979, 
                   3.0981, 2.0151, 1.5991, 1.3687, 1.2480, 1.2549, 1.3589, 1.6108, 2.0391, 3.0825, 
                   2.6689, 1.8364, 1.4492, 1.2339, 1.1270, 1.1353, 1.2495, 1.4668, 1.8716, 2.6538, 
                   2.4233, 1.7310, 1.3574, 1.1680, 1.0444, 1.0435, 1.1567, 1.3818, 1.7476, 2.4971, 
                   2.3882, 1.6445, 1.3257, 1.1055, 0.9912, 1.0063, 1.1104, 1.3345, 1.6821, 2.3657, 
                   2.3521, 1.6289, 1.3159, 1.0845, 1.0044, 0.9961, 1.0957, 1.3281, 1.6689, 2.3042, 
                   2.3701, 1.6548, 1.3340, 1.1240, 1.0059, 1.0015, 1.1235, 1.3462, 1.6772, 2.4087, 
                   2.5737, 1.7217, 1.3936, 1.1807, 1.0552, 1.0674, 1.1748, 1.3945, 1.7603, 2.4639, 
                   2.7495, 1.8345, 1.4775, 1.2456, 1.1616, 1.1318, 1.2593, 1.4976, 1.8496, 2.7930, 
                   3.3662, 2.0322, 1.6572, 1.3882, 1.2690, 1.2773, 1.4038, 1.6206, 2.0801, 3.1636, 
                   3.7710, 2.2446, 1.7661, 1.5005, 1.3745, 1.3853, 1.5098, 1.8311, 2.2500, 3.8540, 
                   2.9961, 1.9717, 1.5732, 1.3252, 1.2246, 1.2305, 1.3296, 1.5869, 2.0078, 2.9424, 
                   2.6167, 1.8188, 1.4316, 1.2231, 1.1265, 1.1240, 1.2368, 1.4575, 1.8467, 2.7012, 
                   2.4092, 1.6934, 1.3467, 1.1431, 1.0317, 1.0381, 1.1348, 1.3672, 1.7104, 2.4346, 
                   2.3193, 1.6255, 1.3037, 1.0918, 0.9946, 0.9971, 1.1123, 1.3110, 1.6777, 2.3379, 
                   2.2983, 1.6143, 1.2935, 1.0786, 1.0010, 0.9941, 1.0942, 1.3066, 1.6558, 2.3184, 
                   2.3198, 1.6377, 1.3018, 1.1084, 0.9922, 0.9976, 1.1138, 1.3218, 1.6719, 2.3623, 
                   2.4609, 1.7061, 1.3745, 1.1489, 1.0605, 1.0596, 1.1650, 1.3818, 1.7456, 2.5054, 
                   2.6543, 1.8442, 1.4448, 1.2378, 1.1450, 1.1372, 1.2466, 1.4805, 1.8574, 2.7480, 
                   3.1655, 2.0215, 1.6367, 1.3633, 1.2671, 1.2534, 1.3643, 1.6113, 2.0620, 3.0894, 
                   3.7603, 2.1733, 1.7725, 1.4741, 1.3638, 1.3599, 1.4785, 1.7744, 2.2871, 3.6040, 
                   2.8887, 1.9268, 1.5713, 1.3184, 1.2144, 1.2246, 1.3398, 1.5654, 2.0156, 2.9600, 
                   2.6162, 1.7720, 1.4175, 1.2104, 1.1138, 1.1152, 1.2095, 1.4697, 1.8125, 2.7314, 
                   2.3384, 1.6890, 1.3140, 1.1489, 1.0317, 1.0410, 1.1577, 1.3467, 1.7358, 2.4141, 
                   2.3325, 1.6250, 1.3008, 1.1025, 0.9922, 0.9985, 1.0933, 1.3374, 1.6421, 2.4233, 
                   2.2217, 1.6152, 1.2661, 1.0898, 0.9961, 0.9951, 1.1050, 1.3042, 1.6479, 2.3296, 
                   2.2974, 1.6377, 1.2993, 1.1128, 0.9941, 1.0059, 1.0986, 1.3457, 1.6484, 2.4141, 
                   2.4243, 1.6885, 1.3564, 1.1553, 1.0566, 1.0605, 1.1729, 1.3799, 1.7515, 2.5034, 
                   2.6143, 1.8311, 1.4453, 1.2402, 1.1426, 1.1382, 1.2378, 1.4761, 1.8545, 2.6890, 
                   3.0015, 2.0210, 1.5894, 1.3853, 1.2480, 1.2739, 1.3687, 1.6567, 2.0400, 3.2544, 
                },
             },
             /* awb para 2 */
             {
                0.3,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0791, 2.3071, 1.8794, 1.5049, 1.3950, 1.3872, 1.5635, 1.8208, 2.4116, 4.0425, 
                   3.1987, 2.0659, 1.6440, 1.3647, 1.2593, 1.2534, 1.3638, 1.6455, 2.1035, 3.1821, 
                   2.7422, 1.8774, 1.4590, 1.2212, 1.1260, 1.1079, 1.2446, 1.4629, 1.9092, 2.8193, 
                   2.5249, 1.7588, 1.3857, 1.1650, 1.0464, 1.0562, 1.1606, 1.4067, 1.7891, 2.6040, 
                   2.4614, 1.6870, 1.3330, 1.1035, 1.0103, 0.9897, 1.1304, 1.3320, 1.7393, 2.4697, 
                   2.3799, 1.6724, 1.2988, 1.0830, 0.9878, 0.9883, 1.0928, 1.3125, 1.6870, 2.3818, 
                   2.4946, 1.7061, 1.3535, 1.1392, 1.0078, 1.0161, 1.1318, 1.3677, 1.7480, 2.5322, 
                   2.5493, 1.7900, 1.3804, 1.1616, 1.0654, 1.0586, 1.1782, 1.3984, 1.7974, 2.5869, 
                   2.8740, 1.8994, 1.5127, 1.2642, 1.1489, 1.1479, 1.2666, 1.5166, 1.9575, 2.8789, 
                   3.3486, 2.1646, 1.6880, 1.3999, 1.2983, 1.2808, 1.4082, 1.7100, 2.1621, 3.4326, 
                   3.7246, 2.2808, 1.7480, 1.5278, 1.3652, 1.3857, 1.5112, 1.8262, 2.2856, 3.7935, 
                   3.0601, 1.9766, 1.6162, 1.3364, 1.2378, 1.2466, 1.3389, 1.5986, 2.0483, 3.0044, 
                   2.6060, 1.8096, 1.4214, 1.2163, 1.1104, 1.1191, 1.2319, 1.4580, 1.8481, 2.6851, 
                   2.4604, 1.7153, 1.3770, 1.1509, 1.0483, 1.0522, 1.1533, 1.3892, 1.7456, 2.5171, 
                   2.3057, 1.6431, 1.3013, 1.0981, 0.9966, 1.0000, 1.1177, 1.3159, 1.6968, 2.3809, 
                   2.2690, 1.6211, 1.2920, 1.0757, 0.9829, 0.9976, 1.0894, 1.3145, 1.6499, 2.3667, 
                   2.3545, 1.6606, 1.3223, 1.1152, 1.0112, 1.0122, 1.1299, 1.3418, 1.7090, 2.4395, 
                   2.4199, 1.7129, 1.3667, 1.1455, 1.0503, 1.0537, 1.1636, 1.3906, 1.7383, 2.5439, 
                   2.6929, 1.8569, 1.4580, 1.2485, 1.1431, 1.1548, 1.2661, 1.4912, 1.9077, 2.7798, 
                   3.1938, 2.0444, 1.6431, 1.3701, 1.2788, 1.2588, 1.3706, 1.6611, 2.0649, 3.2065, 
                   3.7676, 2.1494, 1.7959, 1.4575, 1.3784, 1.3604, 1.4849, 1.8022, 2.2222, 3.5879, 
                   2.8955, 1.9805, 1.5562, 1.3379, 1.2290, 1.2344, 1.3447, 1.5879, 2.0161, 3.0039, 
                   2.5591, 1.7646, 1.4189, 1.1973, 1.1206, 1.1182, 1.2227, 1.4443, 1.8276, 2.6543, 
                   2.3511, 1.7134, 1.3359, 1.1577, 1.0361, 1.0483, 1.1626, 1.3647, 1.7427, 2.4536, 
                   2.2734, 1.6343, 1.2954, 1.0913, 1.0000, 0.9990, 1.1133, 1.3164, 1.6733, 2.3857, 
                   2.2153, 1.5972, 1.2783, 1.0781, 0.9951, 0.9912, 1.0894, 1.3018, 1.6392, 2.2881, 
                   2.3003, 1.6680, 1.3169, 1.1230, 1.0068, 1.0229, 1.1274, 1.3267, 1.7017, 2.4043, 
                   2.4150, 1.6978, 1.3374, 1.1655, 1.0420, 1.0562, 1.1563, 1.3745, 1.7373, 2.4756, 
                   2.6450, 1.8271, 1.4756, 1.2261, 1.1631, 1.1484, 1.2549, 1.4868, 1.8643, 2.7544, 
                   3.0923, 2.0610, 1.5869, 1.4268, 1.2461, 1.2861, 1.3608, 1.6191, 2.0957, 3.0503, 
                },
             },
             /* awb para 3 */
             {
                0.3,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0630, 2.3340, 1.8652, 1.5303, 1.3970, 1.4116, 1.5674, 1.8408, 2.4243, 4.0415, 
                   3.1001, 2.0317, 1.6133, 1.3320, 1.2319, 1.2134, 1.3462, 1.6001, 2.0596, 3.1025, 
                   2.7930, 1.8813, 1.4736, 1.2334, 1.1274, 1.1309, 1.2607, 1.4624, 1.9346, 2.7861, 
                   2.5044, 1.7524, 1.3711, 1.1455, 1.0391, 1.0278, 1.1489, 1.3799, 1.7539, 2.5664, 
                   2.4048, 1.6772, 1.3159, 1.0981, 0.9966, 0.9966, 1.1167, 1.3203, 1.7124, 2.4189, 
                   2.3779, 1.6885, 1.3145, 1.0908, 1.0049, 1.0068, 1.1006, 1.3442, 1.6987, 2.4556, 
                   2.4097, 1.6768, 1.3262, 1.1104, 1.0000, 0.9922, 1.1074, 1.3384, 1.6963, 2.4819, 
                   2.5972, 1.7876, 1.4038, 1.1777, 1.0659, 1.0864, 1.1782, 1.4307, 1.7979, 2.6611, 
                   2.7847, 1.9072, 1.4810, 1.2598, 1.1553, 1.1396, 1.2681, 1.5029, 1.9277, 2.9194, 
                   3.2676, 2.0850, 1.6733, 1.3750, 1.2549, 1.3003, 1.3608, 1.6890, 2.1235, 3.2163, 
                   3.7461, 2.2451, 1.7700, 1.4917, 1.3638, 1.3794, 1.4966, 1.8110, 2.3057, 3.7617, 
                   2.9478, 1.9429, 1.5674, 1.3184, 1.2100, 1.2153, 1.3237, 1.5664, 1.9902, 2.9966, 
                   2.6294, 1.8320, 1.4385, 1.2271, 1.1270, 1.1216, 1.2427, 1.4619, 1.8745, 2.6963, 
                   2.4106, 1.6885, 1.3511, 1.1445, 1.0220, 1.0488, 1.1318, 1.3696, 1.7217, 2.5049, 
                   2.2949, 1.6470, 1.2886, 1.0942, 0.9971, 0.9868, 1.1089, 1.3130, 1.6714, 2.3472, 
                   2.3154, 1.6304, 1.3125, 1.0850, 0.9985, 1.0093, 1.0923, 1.3149, 1.6738, 2.3672, 
                   2.2891, 1.6504, 1.2832, 1.1089, 0.9795, 0.9951, 1.1074, 1.3149, 1.6655, 2.3662, 
                   2.5103, 1.7207, 1.3818, 1.1655, 1.0566, 1.0625, 1.1694, 1.3955, 1.7598, 2.5669, 
                   2.6284, 1.8730, 1.4590, 1.2520, 1.1519, 1.1523, 1.2642, 1.4697, 1.8940, 2.7295, 
                   3.2412, 1.9746, 1.5967, 1.3696, 1.2051, 1.2505, 1.3379, 1.6270, 2.0308, 3.1763, 
                   3.6626, 2.2036, 1.7524, 1.4995, 1.3364, 1.3804, 1.4985, 1.7896, 2.3198, 3.6001, 
                   2.8735, 1.8955, 1.5376, 1.2983, 1.2036, 1.2080, 1.3091, 1.5269, 1.9756, 2.8428, 
                   2.5762, 1.7954, 1.4307, 1.2163, 1.1250, 1.1279, 1.2339, 1.4590, 1.8491, 2.6855, 
                   2.3506, 1.6670, 1.3330, 1.1313, 1.0288, 1.0454, 1.1436, 1.3599, 1.7251, 2.4292, 
                   2.2979, 1.5918, 1.2949, 1.0859, 0.9834, 0.9946, 1.1016, 1.2930, 1.6416, 2.3379, 
                   2.2417, 1.6235, 1.2881, 1.0845, 1.0112, 1.0059, 1.1143, 1.3213, 1.6694, 2.3569, 
                   2.2676, 1.6016, 1.2827, 1.0903, 0.9717, 1.0024, 1.0894, 1.3022, 1.6387, 2.3169, 
                   2.4136, 1.6929, 1.3677, 1.1523, 1.0640, 1.0674, 1.1826, 1.3862, 1.7612, 2.4966, 
                   2.6230, 1.8320, 1.4292, 1.2402, 1.1387, 1.1470, 1.2354, 1.4634, 1.8584, 2.6626, 
                   3.0088, 1.9487, 1.6074, 1.3398, 1.2295, 1.2476, 1.3828, 1.5625, 2.0811, 3.0229, 
                },
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.1992, 2.3433, 1.9121, 1.5298, 1.4131, 1.4233, 1.5537, 1.9023, 2.4263, 4.1050, 
                   3.1421, 2.0850, 1.6104, 1.3647, 1.2305, 1.2461, 1.3516, 1.6318, 2.1030, 3.1899, 
                   2.8296, 1.8955, 1.4897, 1.2407, 1.1377, 1.1323, 1.2539, 1.4883, 1.9395, 2.8428, 
                   2.5220, 1.7749, 1.3657, 1.1577, 1.0308, 1.0361, 1.1577, 1.3940, 1.7847, 2.6113, 
                   2.4526, 1.6831, 1.3301, 1.0957, 0.9961, 1.0024, 1.1006, 1.3301, 1.7251, 2.4331, 
                   2.4526, 1.6948, 1.3179, 1.1001, 0.9995, 0.9907, 1.1050, 1.3281, 1.7026, 2.4526, 
                   2.4761, 1.6992, 1.3330, 1.1157, 0.9922, 0.9971, 1.1094, 1.3228, 1.7305, 2.4478, 
                   2.6470, 1.7856, 1.4067, 1.1729, 1.0654, 1.0669, 1.2046, 1.4087, 1.8306, 2.6416, 
                   2.8472, 1.9229, 1.5029, 1.2622, 1.1470, 1.1445, 1.2500, 1.4946, 1.9546, 2.8765, 
                   3.3706, 2.1104, 1.6577, 1.3687, 1.2622, 1.2603, 1.4233, 1.6392, 2.1929, 3.2603, 
                   3.7437, 2.2642, 1.7798, 1.4980, 1.3921, 1.3823, 1.4985, 1.8442, 2.2637, 3.9390, 
                   3.0400, 1.9692, 1.5884, 1.3345, 1.2163, 1.2207, 1.3511, 1.5649, 2.0352, 2.9741, 
                   2.6152, 1.8320, 1.4404, 1.2212, 1.1313, 1.1309, 1.2285, 1.4644, 1.8540, 2.7119, 
                   2.3862, 1.7114, 1.3472, 1.1450, 1.0313, 1.0356, 1.1504, 1.3643, 1.7402, 2.4795, 
                   2.3403, 1.6436, 1.3008, 1.0933, 0.9976, 0.9878, 1.1060, 1.2998, 1.6680, 2.3350, 
                   2.2754, 1.6333, 1.2959, 1.0869, 0.9897, 0.9985, 1.0933, 1.3110, 1.6694, 2.3560, 
                   2.3408, 1.6431, 1.3013, 1.1055, 0.9888, 0.9937, 1.1084, 1.3154, 1.6753, 2.3394, 
                   2.4731, 1.7427, 1.3721, 1.1719, 1.0522, 1.0625, 1.1563, 1.3931, 1.7476, 2.5264, 
                   2.6890, 1.8286, 1.4668, 1.2324, 1.1372, 1.1460, 1.2505, 1.4727, 1.8833, 2.7197, 
                   3.1987, 2.0332, 1.6094, 1.3706, 1.2510, 1.2393, 1.3496, 1.6387, 2.0278, 3.1948, 
                   3.7358, 2.1665, 1.7969, 1.4893, 1.3394, 1.3931, 1.4990, 1.8223, 2.3691, 3.6719, 
                   2.8809, 1.9331, 1.5830, 1.3003, 1.2417, 1.2202, 1.3589, 1.5513, 2.0581, 2.8774, 
                   2.5972, 1.7915, 1.4209, 1.2251, 1.1172, 1.1396, 1.2104, 1.4858, 1.8511, 2.7061, 
                   2.3564, 1.6665, 1.3550, 1.1245, 1.0332, 1.0527, 1.1563, 1.3613, 1.7632, 2.4351, 
                   2.2788, 1.6338, 1.2773, 1.0908, 1.0044, 1.0063, 1.1016, 1.3271, 1.6738, 2.3555, 
                   2.2368, 1.6030, 1.3018, 1.0752, 1.0098, 0.9995, 1.1152, 1.3193, 1.6763, 2.3662, 
                   2.3076, 1.6309, 1.2998, 1.1104, 0.9790, 1.0112, 1.0996, 1.3164, 1.6553, 2.3730, 
                   2.4287, 1.7217, 1.3726, 1.1621, 1.0713, 1.0654, 1.1792, 1.3955, 1.7524, 2.5547, 
                   2.6240, 1.8091, 1.4648, 1.2363, 1.1387, 1.1479, 1.2310, 1.4722, 1.8638, 2.6611, 
                   3.0371, 2.0659, 1.5962, 1.3877, 1.2427, 1.2788, 1.3477, 1.6167, 2.0439, 3.1831, 
                },
             },
             /* awb para 1 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   3.8623, 2.3374, 1.7646, 1.5356, 1.3662, 1.4063, 1.5190, 1.8076, 2.3306, 3.7979, 
                   3.0981, 2.0151, 1.5991, 1.3687, 1.2480, 1.2549, 1.3589, 1.6108, 2.0391, 3.0825, 
                   2.6689, 1.8364, 1.4492, 1.2339, 1.1270, 1.1353, 1.2495, 1.4668, 1.8716, 2.6538, 
                   2.4233, 1.7310, 1.3574, 1.1680, 1.0444, 1.0435, 1.1567, 1.3818, 1.7476, 2.4971, 
                   2.3882, 1.6445, 1.3257, 1.1055, 0.9912, 1.0063, 1.1104, 1.3345, 1.6821, 2.3657, 
                   2.3521, 1.6289, 1.3159, 1.0845, 1.0044, 0.9961, 1.0957, 1.3281, 1.6689, 2.3042, 
                   2.3701, 1.6548, 1.3340, 1.1240, 1.0059, 1.0015, 1.1235, 1.3462, 1.6772, 2.4087, 
                   2.5737, 1.7217, 1.3936, 1.1807, 1.0552, 1.0674, 1.1748, 1.3945, 1.7603, 2.4639, 
                   2.7495, 1.8345, 1.4775, 1.2456, 1.1616, 1.1318, 1.2593, 1.4976, 1.8496, 2.7930, 
                   3.3662, 2.0322, 1.6572, 1.3882, 1.2690, 1.2773, 1.4038, 1.6206, 2.0801, 3.1636, 
                   3.7710, 2.2446, 1.7661, 1.5005, 1.3745, 1.3853, 1.5098, 1.8311, 2.2500, 3.8540, 
                   2.9961, 1.9717, 1.5732, 1.3252, 1.2246, 1.2305, 1.3296, 1.5869, 2.0078, 2.9424, 
                   2.6167, 1.8188, 1.4316, 1.2231, 1.1265, 1.1240, 1.2368, 1.4575, 1.8467, 2.7012, 
                   2.4092, 1.6934, 1.3467, 1.1431, 1.0317, 1.0381, 1.1348, 1.3672, 1.7104, 2.4346, 
                   2.3193, 1.6255, 1.3037, 1.0918, 0.9946, 0.9971, 1.1123, 1.3110, 1.6777, 2.3379, 
                   2.2983, 1.6143, 1.2935, 1.0786, 1.0010, 0.9941, 1.0942, 1.3066, 1.6558, 2.3184, 
                   2.3198, 1.6377, 1.3018, 1.1084, 0.9922, 0.9976, 1.1138, 1.3218, 1.6719, 2.3623, 
                   2.4609, 1.7061, 1.3745, 1.1489, 1.0605, 1.0596, 1.1650, 1.3818, 1.7456, 2.5054, 
                   2.6543, 1.8442, 1.4448, 1.2378, 1.1450, 1.1372, 1.2466, 1.4805, 1.8574, 2.7480, 
                   3.1655, 2.0215, 1.6367, 1.3633, 1.2671, 1.2534, 1.3643, 1.6113, 2.0620, 3.0894, 
                   3.7603, 2.1733, 1.7725, 1.4741, 1.3638, 1.3599, 1.4785, 1.7744, 2.2871, 3.6040, 
                   2.8887, 1.9268, 1.5713, 1.3184, 1.2144, 1.2246, 1.3398, 1.5654, 2.0156, 2.9600, 
                   2.6162, 1.7720, 1.4175, 1.2104, 1.1138, 1.1152, 1.2095, 1.4697, 1.8125, 2.7314, 
                   2.3384, 1.6890, 1.3140, 1.1489, 1.0317, 1.0410, 1.1577, 1.3467, 1.7358, 2.4141, 
                   2.3325, 1.6250, 1.3008, 1.1025, 0.9922, 0.9985, 1.0933, 1.3374, 1.6421, 2.4233, 
                   2.2217, 1.6152, 1.2661, 1.0898, 0.9961, 0.9951, 1.1050, 1.3042, 1.6479, 2.3296, 
                   2.2974, 1.6377, 1.2993, 1.1128, 0.9941, 1.0059, 1.0986, 1.3457, 1.6484, 2.4141, 
                   2.4243, 1.6885, 1.3564, 1.1553, 1.0566, 1.0605, 1.1729, 1.3799, 1.7515, 2.5034, 
                   2.6143, 1.8311, 1.4453, 1.2402, 1.1426, 1.1382, 1.2378, 1.4761, 1.8545, 2.6890, 
                   3.0015, 2.0210, 1.5894, 1.3853, 1.2480, 1.2739, 1.3687, 1.6567, 2.0400, 3.2544, 
                },
             },
             /* awb para 2 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0791, 2.3071, 1.8794, 1.5049, 1.3950, 1.3872, 1.5635, 1.8208, 2.4116, 4.0425, 
                   3.1987, 2.0659, 1.6440, 1.3647, 1.2593, 1.2534, 1.3638, 1.6455, 2.1035, 3.1821, 
                   2.7422, 1.8774, 1.4590, 1.2212, 1.1260, 1.1079, 1.2446, 1.4629, 1.9092, 2.8193, 
                   2.5249, 1.7588, 1.3857, 1.1650, 1.0464, 1.0562, 1.1606, 1.4067, 1.7891, 2.6040, 
                   2.4614, 1.6870, 1.3330, 1.1035, 1.0103, 0.9897, 1.1304, 1.3320, 1.7393, 2.4697, 
                   2.3799, 1.6724, 1.2988, 1.0830, 0.9878, 0.9883, 1.0928, 1.3125, 1.6870, 2.3818, 
                   2.4946, 1.7061, 1.3535, 1.1392, 1.0078, 1.0161, 1.1318, 1.3677, 1.7480, 2.5322, 
                   2.5493, 1.7900, 1.3804, 1.1616, 1.0654, 1.0586, 1.1782, 1.3984, 1.7974, 2.5869, 
                   2.8740, 1.8994, 1.5127, 1.2642, 1.1489, 1.1479, 1.2666, 1.5166, 1.9575, 2.8789, 
                   3.3486, 2.1646, 1.6880, 1.3999, 1.2983, 1.2808, 1.4082, 1.7100, 2.1621, 3.4326, 
                   3.7246, 2.2808, 1.7480, 1.5278, 1.3652, 1.3857, 1.5112, 1.8262, 2.2856, 3.7935, 
                   3.0601, 1.9766, 1.6162, 1.3364, 1.2378, 1.2466, 1.3389, 1.5986, 2.0483, 3.0044, 
                   2.6060, 1.8096, 1.4214, 1.2163, 1.1104, 1.1191, 1.2319, 1.4580, 1.8481, 2.6851, 
                   2.4604, 1.7153, 1.3770, 1.1509, 1.0483, 1.0522, 1.1533, 1.3892, 1.7456, 2.5171, 
                   2.3057, 1.6431, 1.3013, 1.0981, 0.9966, 1.0000, 1.1177, 1.3159, 1.6968, 2.3809, 
                   2.2690, 1.6211, 1.2920, 1.0757, 0.9829, 0.9976, 1.0894, 1.3145, 1.6499, 2.3667, 
                   2.3545, 1.6606, 1.3223, 1.1152, 1.0112, 1.0122, 1.1299, 1.3418, 1.7090, 2.4395, 
                   2.4199, 1.7129, 1.3667, 1.1455, 1.0503, 1.0537, 1.1636, 1.3906, 1.7383, 2.5439, 
                   2.6929, 1.8569, 1.4580, 1.2485, 1.1431, 1.1548, 1.2661, 1.4912, 1.9077, 2.7798, 
                   3.1938, 2.0444, 1.6431, 1.3701, 1.2788, 1.2588, 1.3706, 1.6611, 2.0649, 3.2065, 
                   3.7676, 2.1494, 1.7959, 1.4575, 1.3784, 1.3604, 1.4849, 1.8022, 2.2222, 3.5879, 
                   2.8955, 1.9805, 1.5562, 1.3379, 1.2290, 1.2344, 1.3447, 1.5879, 2.0161, 3.0039, 
                   2.5591, 1.7646, 1.4189, 1.1973, 1.1206, 1.1182, 1.2227, 1.4443, 1.8276, 2.6543, 
                   2.3511, 1.7134, 1.3359, 1.1577, 1.0361, 1.0483, 1.1626, 1.3647, 1.7427, 2.4536, 
                   2.2734, 1.6343, 1.2954, 1.0913, 1.0000, 0.9990, 1.1133, 1.3164, 1.6733, 2.3857, 
                   2.2153, 1.5972, 1.2783, 1.0781, 0.9951, 0.9912, 1.0894, 1.3018, 1.6392, 2.2881, 
                   2.3003, 1.6680, 1.3169, 1.1230, 1.0068, 1.0229, 1.1274, 1.3267, 1.7017, 2.4043, 
                   2.4150, 1.6978, 1.3374, 1.1655, 1.0420, 1.0562, 1.1563, 1.3745, 1.7373, 2.4756, 
                   2.6450, 1.8271, 1.4756, 1.2261, 1.1631, 1.1484, 1.2549, 1.4868, 1.8643, 2.7544, 
                   3.0923, 2.0610, 1.5869, 1.4268, 1.2461, 1.2861, 1.3608, 1.6191, 2.0957, 3.0503, 
                },
             },
             /* awb para 3 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0630, 2.3340, 1.8652, 1.5303, 1.3970, 1.4116, 1.5674, 1.8408, 2.4243, 4.0415, 
                   3.1001, 2.0317, 1.6133, 1.3320, 1.2319, 1.2134, 1.3462, 1.6001, 2.0596, 3.1025, 
                   2.7930, 1.8813, 1.4736, 1.2334, 1.1274, 1.1309, 1.2607, 1.4624, 1.9346, 2.7861, 
                   2.5044, 1.7524, 1.3711, 1.1455, 1.0391, 1.0278, 1.1489, 1.3799, 1.7539, 2.5664, 
                   2.4048, 1.6772, 1.3159, 1.0981, 0.9966, 0.9966, 1.1167, 1.3203, 1.7124, 2.4189, 
                   2.3779, 1.6885, 1.3145, 1.0908, 1.0049, 1.0068, 1.1006, 1.3442, 1.6987, 2.4556, 
                   2.4097, 1.6768, 1.3262, 1.1104, 1.0000, 0.9922, 1.1074, 1.3384, 1.6963, 2.4819, 
                   2.5972, 1.7876, 1.4038, 1.1777, 1.0659, 1.0864, 1.1782, 1.4307, 1.7979, 2.6611, 
                   2.7847, 1.9072, 1.4810, 1.2598, 1.1553, 1.1396, 1.2681, 1.5029, 1.9277, 2.9194, 
                   3.2676, 2.0850, 1.6733, 1.3750, 1.2549, 1.3003, 1.3608, 1.6890, 2.1235, 3.2163, 
                   3.7461, 2.2451, 1.7700, 1.4917, 1.3638, 1.3794, 1.4966, 1.8110, 2.3057, 3.7617, 
                   2.9478, 1.9429, 1.5674, 1.3184, 1.2100, 1.2153, 1.3237, 1.5664, 1.9902, 2.9966, 
                   2.6294, 1.8320, 1.4385, 1.2271, 1.1270, 1.1216, 1.2427, 1.4619, 1.8745, 2.6963, 
                   2.4106, 1.6885, 1.3511, 1.1445, 1.0220, 1.0488, 1.1318, 1.3696, 1.7217, 2.5049, 
                   2.2949, 1.6470, 1.2886, 1.0942, 0.9971, 0.9868, 1.1089, 1.3130, 1.6714, 2.3472, 
                   2.3154, 1.6304, 1.3125, 1.0850, 0.9985, 1.0093, 1.0923, 1.3149, 1.6738, 2.3672, 
                   2.2891, 1.6504, 1.2832, 1.1089, 0.9795, 0.9951, 1.1074, 1.3149, 1.6655, 2.3662, 
                   2.5103, 1.7207, 1.3818, 1.1655, 1.0566, 1.0625, 1.1694, 1.3955, 1.7598, 2.5669, 
                   2.6284, 1.8730, 1.4590, 1.2520, 1.1519, 1.1523, 1.2642, 1.4697, 1.8940, 2.7295, 
                   3.2412, 1.9746, 1.5967, 1.3696, 1.2051, 1.2505, 1.3379, 1.6270, 2.0308, 3.1763, 
                   3.6626, 2.2036, 1.7524, 1.4995, 1.3364, 1.3804, 1.4985, 1.7896, 2.3198, 3.6001, 
                   2.8735, 1.8955, 1.5376, 1.2983, 1.2036, 1.2080, 1.3091, 1.5269, 1.9756, 2.8428, 
                   2.5762, 1.7954, 1.4307, 1.2163, 1.1250, 1.1279, 1.2339, 1.4590, 1.8491, 2.6855, 
                   2.3506, 1.6670, 1.3330, 1.1313, 1.0288, 1.0454, 1.1436, 1.3599, 1.7251, 2.4292, 
                   2.2979, 1.5918, 1.2949, 1.0859, 0.9834, 0.9946, 1.1016, 1.2930, 1.6416, 2.3379, 
                   2.2417, 1.6235, 1.2881, 1.0845, 1.0112, 1.0059, 1.1143, 1.3213, 1.6694, 2.3569, 
                   2.2676, 1.6016, 1.2827, 1.0903, 0.9717, 1.0024, 1.0894, 1.3022, 1.6387, 2.3169, 
                   2.4136, 1.6929, 1.3677, 1.1523, 1.0640, 1.0674, 1.1826, 1.3862, 1.7612, 2.4966, 
                   2.6230, 1.8320, 1.4292, 1.2402, 1.1387, 1.1470, 1.2354, 1.4634, 1.8584, 2.6626, 
                   3.0088, 1.9487, 1.6074, 1.3398, 1.2295, 1.2476, 1.3828, 1.5625, 2.0811, 3.0229, 
                },
             },
          },
       },
    },
    /* isp_sub_module_acm_pra isp_sub_module_acm_tuning */
    {
       0,//enable
       0,//interpolation_enable
       0,//count_ae
       0,//count_awb
       0,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
       },
       /* awb trigger */
       {
       },
       /* isp_sub_module_cm_pra pra[MIDDLE_TRIGGER_COUNT=5][MIDDLE_TRIGGER_COUNT=7] */
       {
       },
    },
    /* STRU_ISP_SUB_MODULE_IR_LMS_TUNING_T STRU_ISP_SUB_MODULE_IR_LMS_PRA */
    {
       0,//enable
       0,//interpolation_enable
       0,//count_ae
       0,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
       },
       /* STRU_ISP_SUB_MODULE_IR_LMS_PRA pra[IR_LMS_AEC_TRIGGER_COUNT] */
       {
       },
    },
    /* STRU_ISP_SW_ALGO_IR_RAW_3DNR_TUNING_T STRU_ISP_SW_ALGO_IR_RAW_3DNR_TUNING */
    {
       0,//enable
       0,//interpolation_enable
       0,//count_ae
       0,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
       },
       /* STRU_ISP_SW_ALGO_IR_RAW_3DNR_PRA pra[IR_RAW_3DNR_SW_AEC_TRIGGER_COUNT] */
       {
       },
    },
    /* STRU_ISP_SW_ALGO_IR_RAW_2DNR_TUNING_T STRU_ISP_SW_ALGO_IR_RAW_2DNR_TUNING */
    {
       0,//enable
       0,//interpolation_enable
       0,//count_ae
       0,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
       },
       /* STRU_ISP_SW_ALGO_IR_2DNR_GTM_PRA pra[IR_RAW_2DNR_SW_AEC_TRIGGER_COUNT] */
       {
       },
    },
    /* STRU_ISP_SW_ALGO_IR_RAW_GTM_TUNING_T STRU_ISP_SW_ALGO_IR_RAW_GTM_TUNING */
    {
       0,//enable
       0,//interpolation_enable
       0,//count_ae
       0,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
       },
       /* STRU_ISP_SW_ALGO_IR_RAW_GTM_PRA pra[IR_RAW_GTM_SW_AEC_TRIGGER_COUNT] */
       {
       },
    },
},
/* stats_config_t stats_config */
{
},
/* aec_tuning_t aec_tuning */
{
   /* ae version */
   0x00000001,
   /* ae enable */
   1,
   /* ae start_exp_index */
   180,
   /* ae start_skip_count */
   1,
   /* ae torlerence */
   2,
   /* ae exp_table_para */
   {
      /* count */
      467,
      /* ev0_count */
      467,
      /* ev2_count */
      467,
      /* fix_fps_index */
      25,
      /* enable_isp_digital_gain */
      0,
      /* max_sensor_gain */
      1000,
      /* max_isp_gain1 */
      1,
      /* exp_table */
      {
      //1,970
      //1000,970
         {    256,      1   },         /* Gain = 1.000000 Exposure Index = 0 */
         {    264,      1   },         /* Gain = 1.031250 Exposure Index = 1 */
         {    272,      1   },         /* Gain = 1.062500 Exposure Index = 2 */
         {    281,      1   },         /* Gain = 1.097656 Exposure Index = 3 */
         {    290,      1   },         /* Gain = 1.132812 Exposure Index = 4 */
         {    299,      1   },         /* Gain = 1.167969 Exposure Index = 5 */
         {    308,      1   },         /* Gain = 1.203125 Exposure Index = 6 */
         {    318,      1   },         /* Gain = 1.242188 Exposure Index = 7 */
         {    328,      1   },         /* Gain = 1.281250 Exposure Index = 8 */
         {    338,      1   },         /* Gain = 1.320312 Exposure Index = 9 */
         {    349,      1   },         /* Gain = 1.363281 Exposure Index = 10 */
         {    360,      1   },         /* Gain = 1.406250 Exposure Index = 11 */
         {    371,      1   },         /* Gain = 1.449219 Exposure Index = 12 */
         {    383,      1   },         /* Gain = 1.496094 Exposure Index = 13 */
         {    395,      1   },         /* Gain = 1.542969 Exposure Index = 14 */
         {    407,      1   },         /* Gain = 1.589844 Exposure Index = 15 */
         {    420,      1   },         /* Gain = 1.640625 Exposure Index = 16 */
         {    433,      1   },         /* Gain = 1.691406 Exposure Index = 17 */
         {    446,      1   },         /* Gain = 1.742188 Exposure Index = 18 */
         {    460,      1   },         /* Gain = 1.796875 Exposure Index = 19 */
         {    474,      1   },         /* Gain = 1.851562 Exposure Index = 20 */
         {    489,      1   },         /* Gain = 1.910156 Exposure Index = 21 */
         {    504,      1   },         /* Gain = 1.968750 Exposure Index = 22 */
         {    260,      2   },         /* Gain = 1.015625 Exposure Index = 23 */
         {    268,      2   },         /* Gain = 1.046875 Exposure Index = 24 */
         {    277,      2   },         /* Gain = 1.082031 Exposure Index = 25 */
         {    286,      2   },         /* Gain = 1.117188 Exposure Index = 26 */
         {    295,      2   },         /* Gain = 1.152344 Exposure Index = 27 */
         {    304,      2   },         /* Gain = 1.187500 Exposure Index = 28 */
         {    314,      2   },         /* Gain = 1.226562 Exposure Index = 29 */
         {    324,      2   },         /* Gain = 1.265625 Exposure Index = 30 */
         {    334,      2   },         /* Gain = 1.304688 Exposure Index = 31 */
         {    345,      2   },         /* Gain = 1.347656 Exposure Index = 32 */
         {    356,      2   },         /* Gain = 1.390625 Exposure Index = 33 */
         {    367,      2   },         /* Gain = 1.433594 Exposure Index = 34 */
         {    379,      2   },         /* Gain = 1.480469 Exposure Index = 35 */
         {    261,      3   },         /* Gain = 1.019531 Exposure Index = 36 */
         {    269,      3   },         /* Gain = 1.050781 Exposure Index = 37 */
         {    278,      3   },         /* Gain = 1.085938 Exposure Index = 38 */
         {    287,      3   },         /* Gain = 1.121094 Exposure Index = 39 */
         {    296,      3   },         /* Gain = 1.156250 Exposure Index = 40 */
         {    305,      3   },         /* Gain = 1.191406 Exposure Index = 41 */
         {    315,      3   },         /* Gain = 1.230469 Exposure Index = 42 */
         {    325,      3   },         /* Gain = 1.269531 Exposure Index = 43 */
         {    335,      3   },         /* Gain = 1.308594 Exposure Index = 44 */
         {    259,      4   },         /* Gain = 1.011719 Exposure Index = 45 */
         {    267,      4   },         /* Gain = 1.042969 Exposure Index = 46 */
         {    276,      4   },         /* Gain = 1.078125 Exposure Index = 47 */
         {    285,      4   },         /* Gain = 1.113281 Exposure Index = 48 */
         {    294,      4   },         /* Gain = 1.148438 Exposure Index = 49 */
         {    303,      4   },         /* Gain = 1.183594 Exposure Index = 50 */
         {    313,      4   },         /* Gain = 1.222656 Exposure Index = 51 */
         {    258,      5   },         /* Gain = 1.007812 Exposure Index = 52 */
         {    266,      5   },         /* Gain = 1.039062 Exposure Index = 53 */
         {    274,      5   },         /* Gain = 1.070312 Exposure Index = 54 */
         {    283,      5   },         /* Gain = 1.105469 Exposure Index = 55 */
         {    292,      5   },         /* Gain = 1.140625 Exposure Index = 56 */
         {    301,      5   },         /* Gain = 1.175781 Exposure Index = 57 */
         {    259,      6   },         /* Gain = 1.011719 Exposure Index = 58 */
         {    267,      6   },         /* Gain = 1.042969 Exposure Index = 59 */
         {    276,      6   },         /* Gain = 1.078125 Exposure Index = 60 */
         {    285,      6   },         /* Gain = 1.113281 Exposure Index = 61 */
         {    294,      6   },         /* Gain = 1.148438 Exposure Index = 62 */
         {    260,      7   },         /* Gain = 1.015625 Exposure Index = 63 */
         {    268,      7   },         /* Gain = 1.046875 Exposure Index = 64 */
         {    277,      7   },         /* Gain = 1.082031 Exposure Index = 65 */
         {    286,      7   },         /* Gain = 1.117188 Exposure Index = 66 */
         {    258,      8   },         /* Gain = 1.007812 Exposure Index = 67 */
         {    266,      8   },         /* Gain = 1.039062 Exposure Index = 68 */
         {    274,      8   },         /* Gain = 1.070312 Exposure Index = 69 */
         {    283,      8   },         /* Gain = 1.105469 Exposure Index = 70 */
         {    260,      9   },         /* Gain = 1.015625 Exposure Index = 71 */
         {    268,      9   },         /* Gain = 1.046875 Exposure Index = 72 */
         {    277,      9   },         /* Gain = 1.082031 Exposure Index = 73 */
         {    257,     10   },         /* Gain = 1.003906 Exposure Index = 74 */
         {    265,     10   },         /* Gain = 1.035156 Exposure Index = 75 */
         {    273,     10   },         /* Gain = 1.066406 Exposure Index = 76 */
         {    256,     11   },         /* Gain = 1.000000 Exposure Index = 77 */
         {    264,     11   },         /* Gain = 1.031250 Exposure Index = 78 */
         {    272,     11   },         /* Gain = 1.062500 Exposure Index = 79 */
         {    257,     12   },         /* Gain = 1.003906 Exposure Index = 80 */
         {    265,     12   },         /* Gain = 1.035156 Exposure Index = 81 */
         {    273,     12   },         /* Gain = 1.066406 Exposure Index = 82 */
         {    260,     13   },         /* Gain = 1.015625 Exposure Index = 83 */
         {    268,     13   },         /* Gain = 1.046875 Exposure Index = 84 */
         {    257,     14   },         /* Gain = 1.003906 Exposure Index = 85 */
         {    265,     14   },         /* Gain = 1.035156 Exposure Index = 86 */
         {    273,     14   },         /* Gain = 1.066406 Exposure Index = 87 */
         {    263,     15   },         /* Gain = 1.027344 Exposure Index = 88 */
         {    271,     15   },         /* Gain = 1.058594 Exposure Index = 89 */
         {    262,     16   },         /* Gain = 1.023438 Exposure Index = 90 */
         {    270,     16   },         /* Gain = 1.054688 Exposure Index = 91 */
         {    262,     17   },         /* Gain = 1.023438 Exposure Index = 92 */
         {    270,     17   },         /* Gain = 1.054688 Exposure Index = 93 */
         {    263,     18   },         /* Gain = 1.027344 Exposure Index = 94 */
         {    257,     19   },         /* Gain = 1.003906 Exposure Index = 95 */
         {    265,     19   },         /* Gain = 1.035156 Exposure Index = 96 */
         {    260,     20   },         /* Gain = 1.015625 Exposure Index = 97 */
         {    268,     20   },         /* Gain = 1.046875 Exposure Index = 98 */
         {    263,     21   },         /* Gain = 1.027344 Exposure Index = 99 */
         {    259,     22   },         /* Gain = 1.011719 Exposure Index = 100 */
         {    267,     22   },         /* Gain = 1.042969 Exposure Index = 101 */
         {    264,     23   },         /* Gain = 1.031250 Exposure Index = 102 */
         {    261,     24   },         /* Gain = 1.019531 Exposure Index = 103 */
         {    259,     25   },         /* Gain = 1.011719 Exposure Index = 104 */
         {    257,     26   },         /* Gain = 1.003906 Exposure Index = 105 */
         {    265,     26   },         /* Gain = 1.035156 Exposure Index = 106 */
         {    263,     27   },         /* Gain = 1.027344 Exposure Index = 107 */
         {    262,     28   },         /* Gain = 1.023438 Exposure Index = 108 */
         {    261,     29   },         /* Gain = 1.019531 Exposure Index = 109 */
         {    260,     30   },         /* Gain = 1.015625 Exposure Index = 110 */
         {    260,     31   },         /* Gain = 1.015625 Exposure Index = 111 */
         {    260,     32   },         /* Gain = 1.015625 Exposure Index = 112 */
         {    260,     33   },         /* Gain = 1.015625 Exposure Index = 113 */
         {    260,     34   },         /* Gain = 1.015625 Exposure Index = 114 */
         {    261,     35   },         /* Gain = 1.019531 Exposure Index = 115 */
         {    262,     36   },         /* Gain = 1.023438 Exposure Index = 116 */
         {    256,     38   },         /* Gain = 1.000000 Exposure Index = 117 */
         {    257,     39   },         /* Gain = 1.003906 Exposure Index = 118 */
         {    259,     40   },         /* Gain = 1.011719 Exposure Index = 119 */
         {    261,     41   },         /* Gain = 1.019531 Exposure Index = 120 */
         {    257,     43   },         /* Gain = 1.003906 Exposure Index = 121 */
         {    259,     44   },         /* Gain = 1.011719 Exposure Index = 122 */
         {    261,     45   },         /* Gain = 1.019531 Exposure Index = 123 */
         {    258,     47   },         /* Gain = 1.007812 Exposure Index = 124 */
         {    261,     48   },         /* Gain = 1.019531 Exposure Index = 125 */
         {    259,     50   },         /* Gain = 1.011719 Exposure Index = 126 */
         {    257,     52   },         /* Gain = 1.003906 Exposure Index = 127 */
         {    260,     53   },         /* Gain = 1.015625 Exposure Index = 128 */
         {    259,     55   },         /* Gain = 1.011719 Exposure Index = 129 */
         {    258,     57   },         /* Gain = 1.007812 Exposure Index = 130 */
         {    257,     59   },         /* Gain = 1.003906 Exposure Index = 131 */
         {    257,     61   },         /* Gain = 1.003906 Exposure Index = 132 */
         {    257,     63   },         /* Gain = 1.003906 Exposure Index = 133 */
         {    257,     65   },         /* Gain = 1.003906 Exposure Index = 134 */
         {    257,     67   },         /* Gain = 1.003906 Exposure Index = 135 */
         {    258,     69   },         /* Gain = 1.007812 Exposure Index = 136 */
         {    259,     71   },         /* Gain = 1.011719 Exposure Index = 137 */
         {    256,     74   },         /* Gain = 1.000000 Exposure Index = 138 */
         {    257,     76   },         /* Gain = 1.003906 Exposure Index = 139 */
         {    258,     78   },         /* Gain = 1.007812 Exposure Index = 140 */
         {    256,     81   },         /* Gain = 1.000000 Exposure Index = 141 */
         {    258,     83   },         /* Gain = 1.007812 Exposure Index = 142 */
         {    257,     86   },         /* Gain = 1.003906 Exposure Index = 143 */
         {    256,     89   },         /* Gain = 1.000000 Exposure Index = 144 */
         {    258,     91   },         /* Gain = 1.007812 Exposure Index = 145 */
         {    258,     94   },         /* Gain = 1.007812 Exposure Index = 146 */
         {    258,     97   },         /* Gain = 1.007812 Exposure Index = 147 */
         {    258,    100   },         /* Gain = 1.007812 Exposure Index = 148 */
         {    258,    103   },         /* Gain = 1.007812 Exposure Index = 149 */
         {    256,    107   },         /* Gain = 1.000000 Exposure Index = 150 */
         {    257,    110   },         /* Gain = 1.003906 Exposure Index = 151 */
         {    258,    113   },         /* Gain = 1.007812 Exposure Index = 152 */
         {    257,    117   },         /* Gain = 1.003906 Exposure Index = 153 */
         {    256,    121   },         /* Gain = 1.000000 Exposure Index = 154 */
         {    258,    124   },         /* Gain = 1.007812 Exposure Index = 155 */
         {    256,    129   },         /* Gain = 1.000000 Exposure Index = 156 */
         {    256,    133   },         /* Gain = 1.000000 Exposure Index = 157 */
         {    256,    137   },         /* Gain = 1.000000 Exposure Index = 158 */
         {    257,    141   },         /* Gain = 1.003906 Exposure Index = 159 */
         {    256,    146   },         /* Gain = 1.000000 Exposure Index = 160 */
         {    257,    150   },         /* Gain = 1.003906 Exposure Index = 161 */
         {    257,    155   },         /* Gain = 1.003906 Exposure Index = 162 */
         {    257,    160   },         /* Gain = 1.003906 Exposure Index = 163 */
         {    257,    165   },         /* Gain = 1.003906 Exposure Index = 164 */
         {    257,    170   },         /* Gain = 1.003906 Exposure Index = 165 */
         {    256,    176   },         /* Gain = 1.000000 Exposure Index = 166 */
         {    257,    181   },         /* Gain = 1.003906 Exposure Index = 167 */
         {    257,    187   },         /* Gain = 1.003906 Exposure Index = 168 */
         {    257,    193   },         /* Gain = 1.003906 Exposure Index = 169 */
         {    257,    199   },         /* Gain = 1.003906 Exposure Index = 170 */
         {    257,    205   },         /* Gain = 1.003906 Exposure Index = 171 */
         {    256,    212   },         /* Gain = 1.000000 Exposure Index = 172 */
         {    257,    218   },         /* Gain = 1.003906 Exposure Index = 173 */
         {    257,    225   },         /* Gain = 1.003906 Exposure Index = 174 */
         {    257,    232   },         /* Gain = 1.003906 Exposure Index = 175 */
         {    257,    239   },         /* Gain = 1.003906 Exposure Index = 176 */
         {    257,    247   },         /* Gain = 1.003906 Exposure Index = 177 */
         {    257,    255   },         /* Gain = 1.003906 Exposure Index = 178 */
         {    256,    264   },         /* Gain = 1.000000 Exposure Index = 179 */
         {    256,    272   },         /* Gain = 1.000000 Exposure Index = 180 */
         {    256,    281   },         /* Gain = 1.000000 Exposure Index = 181 */
         {    256,    290   },         /* Gain = 1.000000 Exposure Index = 182 */
         {    256,    299   },         /* Gain = 1.000000 Exposure Index = 183 */
         {    256,    308   },         /* Gain = 1.000000 Exposure Index = 184 */
         {    256,    318   },         /* Gain = 1.000000 Exposure Index = 185 */
         {    256,    328   },         /* Gain = 1.000000 Exposure Index = 186 */
         {    256,    338   },         /* Gain = 1.000000 Exposure Index = 187 */
         {    256,    349   },         /* Gain = 1.000000 Exposure Index = 188 */
         {    256,    360   },         /* Gain = 1.000000 Exposure Index = 189 */
         {    256,    371   },         /* Gain = 1.000000 Exposure Index = 190 */
         {    256,    383   },         /* Gain = 1.000000 Exposure Index = 191 */
         {    256,    395   },         /* Gain = 1.000000 Exposure Index = 192 */
         {    256,    407   },         /* Gain = 1.000000 Exposure Index = 193 */
         {    256,    420   },         /* Gain = 1.000000 Exposure Index = 194 */
         {    256,    433   },         /* Gain = 1.000000 Exposure Index = 195 */
         {    256,    446   },         /* Gain = 1.000000 Exposure Index = 196 */
         {    256,    460   },         /* Gain = 1.000000 Exposure Index = 197 */
         {    256,    474   },         /* Gain = 1.000000 Exposure Index = 198 */
         {    256,    489   },         /* Gain = 1.000000 Exposure Index = 199 */
         {    256,    504   },         /* Gain = 1.000000 Exposure Index = 200 */
         {    256,    520   },         /* Gain = 1.000000 Exposure Index = 201 */
         {    256,    536   },         /* Gain = 1.000000 Exposure Index = 202 */
         {    256,    553   },         /* Gain = 1.000000 Exposure Index = 203 */
         {    256,    570   },         /* Gain = 1.000000 Exposure Index = 204 */
         {    256,    588   },         /* Gain = 1.000000 Exposure Index = 205 */
         {    256,    606   },         /* Gain = 1.000000 Exposure Index = 206 */
         {    256,    625   },         /* Gain = 1.000000 Exposure Index = 207 */
         {    256,    644   },         /* Gain = 1.000000 Exposure Index = 208 */
         {    256,    664   },         /* Gain = 1.000000 Exposure Index = 209 */
         {    256,    684   },         /* Gain = 1.000000 Exposure Index = 210 */
         {    256,    705   },         /* Gain = 1.000000 Exposure Index = 211 */
         {    256,    727   },         /* Gain = 1.000000 Exposure Index = 212 */
         {    256,    749   },         /* Gain = 1.000000 Exposure Index = 213 */
         {    256,    772   },         /* Gain = 1.000000 Exposure Index = 214 */
         {    256,    796   },         /* Gain = 1.000000 Exposure Index = 215 */
         {    256,    820   },         /* Gain = 1.000000 Exposure Index = 216 */
         {    256,    845   },         /* Gain = 1.000000 Exposure Index = 217 */
         {    256,    871   },         /* Gain = 1.000000 Exposure Index = 218 */
         {    256,    898   },         /* Gain = 1.000000 Exposure Index = 219 */
         {    256,    925   },         /* Gain = 1.000000 Exposure Index = 220 */
         {    256,    953   },         /* Gain = 1.000000 Exposure Index = 221 */
         {    256,    982   },         /* Gain = 1.000000 Exposure Index = 222 */
         {    256,   1012   },         /* Gain = 1.000000 Exposure Index = 223 */
         {    256,   1043   },         /* Gain = 1.000000 Exposure Index = 224 */
         {    256,   1075   },         /* Gain = 1.000000 Exposure Index = 225 */
         {    256,   1108   },         /* Gain = 1.000000 Exposure Index = 226 */
         {    256,   1142   },         /* Gain = 1.000000 Exposure Index = 227 */
         {    256,   1177   },         /* Gain = 1.000000 Exposure Index = 228 */
         {    256,   1213   },         /* Gain = 1.000000 Exposure Index = 229 */
         {    256,   1250   },         /* Gain = 1.000000 Exposure Index = 230 */
         {    256,   1288   },         /* Gain = 1.000000 Exposure Index = 231 */
         {    256,   1327   },         /* Gain = 1.000000 Exposure Index = 232 */
         {    256,   1367   },         /* Gain = 1.000000 Exposure Index = 233 */
         {    256,   1409   },         /* Gain = 1.000000 Exposure Index = 234 */
         {    256,   1452   },         /* Gain = 1.000000 Exposure Index = 235 */
         {    264,   1455   },         /* Gain = 1.031250 Exposure Index = 236 */
         {    272,   1455   },         /* Gain = 1.062500 Exposure Index = 237 */
         {    281,   1455   },         /* Gain = 1.097656 Exposure Index = 238 */
         {    290,   1455   },         /* Gain = 1.132812 Exposure Index = 239 */
         {    299,   1455   },         /* Gain = 1.167969 Exposure Index = 240 */
         {    308,   1455   },         /* Gain = 1.203125 Exposure Index = 241 */
         {    318,   1455   },         /* Gain = 1.242188 Exposure Index = 242 */
         {    328,   1455   },         /* Gain = 1.281250 Exposure Index = 243 */
         {    338,   1455   },         /* Gain = 1.320312 Exposure Index = 244 */
         {    349,   1455   },         /* Gain = 1.363281 Exposure Index = 245 */
         {    360,   1455   },         /* Gain = 1.406250 Exposure Index = 246 */
         {    371,   1455   },         /* Gain = 1.449219 Exposure Index = 247 */
         {    383,   1455   },         /* Gain = 1.496094 Exposure Index = 248 */
         {    395,   1455   },         /* Gain = 1.542969 Exposure Index = 249 */
         {    407,   1455   },         /* Gain = 1.589844 Exposure Index = 250 */
         {    420,   1455   },         /* Gain = 1.640625 Exposure Index = 251 */
         {    433,   1455   },         /* Gain = 1.691406 Exposure Index = 252 */
         {    446,   1455   },         /* Gain = 1.742188 Exposure Index = 253 */
         {    460,   1455   },         /* Gain = 1.796875 Exposure Index = 254 */
         {    474,   1455   },         /* Gain = 1.851562 Exposure Index = 255 */
         {    489,   1455   },         /* Gain = 1.910156 Exposure Index = 256 */
         {    504,   1455   },         /* Gain = 1.968750 Exposure Index = 257 */
         {    520,   1455   },         /* Gain = 2.031250 Exposure Index = 258 */
         {    536,   1455   },         /* Gain = 2.093750 Exposure Index = 259 */
         {    553,   1455   },         /* Gain = 2.160156 Exposure Index = 260 */
         {    570,   1455   },         /* Gain = 2.226562 Exposure Index = 261 */
         {    588,   1455   },         /* Gain = 2.296875 Exposure Index = 262 */
         {    606,   1455   },         /* Gain = 2.367188 Exposure Index = 263 */
         {    625,   1455   },         /* Gain = 2.441406 Exposure Index = 264 */
         {    644,   1455   },         /* Gain = 2.515625 Exposure Index = 265 */
         {    664,   1455   },         /* Gain = 2.593750 Exposure Index = 266 */
         {    684,   1455   },         /* Gain = 2.671875 Exposure Index = 267 */
         {    705,   1455   },         /* Gain = 2.753906 Exposure Index = 268 */
         {    727,   1455   },         /* Gain = 2.839844 Exposure Index = 269 */
         {    749,   1455   },         /* Gain = 2.925781 Exposure Index = 270 */
         {    772,   1455   },         /* Gain = 3.015625 Exposure Index = 271 */
         {    796,   1455   },         /* Gain = 3.109375 Exposure Index = 272 */
         {    820,   1455   },         /* Gain = 3.203125 Exposure Index = 273 */
         {    845,   1455   },         /* Gain = 3.300781 Exposure Index = 274 */
         {    871,   1455   },         /* Gain = 3.402344 Exposure Index = 275 */
         {    898,   1455   },         /* Gain = 3.507812 Exposure Index = 276 */
         {    925,   1455   },         /* Gain = 3.613281 Exposure Index = 277 */
         {    953,   1455   },         /* Gain = 3.722656 Exposure Index = 278 */
         {    982,   1455   },         /* Gain = 3.835938 Exposure Index = 279 */
         {   1012,   1455   },         /* Gain = 3.953125 Exposure Index = 280 */
         {   1043,   1455   },         /* Gain = 4.074219 Exposure Index = 281 */
         {   1075,   1455   },         /* Gain = 4.199219 Exposure Index = 282 */
         {   1108,   1455   },         /* Gain = 4.328125 Exposure Index = 283 */
         {   1142,   1455   },         /* Gain = 4.460938 Exposure Index = 284 */
         {   1177,   1455   },         /* Gain = 4.597656 Exposure Index = 285 */
         {   1213,   1455   },         /* Gain = 4.738281 Exposure Index = 286 */
         {   1250,   1455   },         /* Gain = 4.882812 Exposure Index = 287 */
         {   1288,   1455   },         /* Gain = 5.031250 Exposure Index = 288 */
         {   1327,   1455   },         /* Gain = 5.183594 Exposure Index = 289 */
         {   1367,   1455   },         /* Gain = 5.339844 Exposure Index = 290 */
         {   1409,   1455   },         /* Gain = 5.503906 Exposure Index = 291 */
         {   1452,   1455   },         /* Gain = 5.671875 Exposure Index = 292 */
         {   1496,   1455   },         /* Gain = 5.843750 Exposure Index = 293 */
         {   1541,   1455   },         /* Gain = 6.019531 Exposure Index = 294 */
         {   1588,   1455   },         /* Gain = 6.203125 Exposure Index = 295 */
         {   1636,   1455   },         /* Gain = 6.390625 Exposure Index = 296 */
         {   1686,   1455   },         /* Gain = 6.585938 Exposure Index = 297 */
         {   1737,   1455   },         /* Gain = 6.785156 Exposure Index = 298 */
         {   1790,   1455   },         /* Gain = 6.992188 Exposure Index = 299 */
         {   1844,   1455   },         /* Gain = 7.203125 Exposure Index = 300 */
         {   1900,   1455   },         /* Gain = 7.421875 Exposure Index = 301 */
         {   1957,   1455   },         /* Gain = 7.644531 Exposure Index = 302 */
         {   2016,   1455   },         /* Gain = 7.875000 Exposure Index = 303 */
         {   2077,   1455   },         /* Gain = 8.113281 Exposure Index = 304 */
         {   2140,   1455   },         /* Gain = 8.359375 Exposure Index = 305 */
         {   2205,   1455   },         /* Gain = 8.613281 Exposure Index = 306 */
         {   2272,   1455   },         /* Gain = 8.875000 Exposure Index = 307 */
         {   2341,   1455   },         /* Gain = 9.144531 Exposure Index = 308 */
         {   2412,   1455   },         /* Gain = 9.421875 Exposure Index = 309 */
         {   2485,   1455   },         /* Gain = 9.707031 Exposure Index = 310 */
         {   2560,   1455   },         /* Gain = 10.000000 Exposure Index = 311 */
         {   2637,   1455   },         /* Gain = 10.300781 Exposure Index = 312 */
         {   2717,   1455   },         /* Gain = 10.613281 Exposure Index = 313 */
         {   2799,   1455   },         /* Gain = 10.933594 Exposure Index = 314 */
         {   2883,   1455   },         /* Gain = 11.261719 Exposure Index = 315 */
         {   2970,   1455   },         /* Gain = 11.601562 Exposure Index = 316 */
         {   3060,   1455   },         /* Gain = 11.953125 Exposure Index = 317 */
         {   3152,   1455   },         /* Gain = 12.312500 Exposure Index = 318 */
         {   3247,   1455   },         /* Gain = 12.683594 Exposure Index = 319 */
         {   3345,   1455   },         /* Gain = 13.066406 Exposure Index = 320 */
         {   3446,   1455   },         /* Gain = 13.460938 Exposure Index = 321 */
         {   3550,   1455   },         /* Gain = 13.867188 Exposure Index = 322 */
         {   3657,   1455   },         /* Gain = 14.285156 Exposure Index = 323 */
         {   3767,   1455   },         /* Gain = 14.714844 Exposure Index = 324 */
         {   3881,   1455   },         /* Gain = 15.160156 Exposure Index = 325 */
         {   3998,   1455   },         /* Gain = 15.617188 Exposure Index = 326 */
         {   4118,   1455   },         /* Gain = 16.085938 Exposure Index = 327 */
         {   4242,   1455   },         /* Gain = 16.570312 Exposure Index = 328 */
         {   4370,   1455   },         /* Gain = 17.070312 Exposure Index = 329 */
         {   4502,   1455   },         /* Gain = 17.585938 Exposure Index = 330 */
         {   4638,   1455   },         /* Gain = 18.117188 Exposure Index = 331 */
         {   4778,   1455   },         /* Gain = 18.664062 Exposure Index = 332 */
         {   4922,   1455   },         /* Gain = 19.226562 Exposure Index = 333 */
         {   5070,   1455   },         /* Gain = 19.804688 Exposure Index = 334 */
         {   5223,   1455   },         /* Gain = 20.402344 Exposure Index = 335 */
         {   5380,   1455   },         /* Gain = 21.015625 Exposure Index = 336 */
         {   5542,   1455   },         /* Gain = 21.648438 Exposure Index = 337 */
         {   5709,   1455   },         /* Gain = 22.300781 Exposure Index = 338 */
         {   5881,   1455   },         /* Gain = 22.972656 Exposure Index = 339 */
         {   6058,   1455   },         /* Gain = 23.664062 Exposure Index = 340 */
         {   6240,   1455   },         /* Gain = 24.375000 Exposure Index = 341 */
         {   6428,   1455   },         /* Gain = 25.109375 Exposure Index = 342 */
         {   6621,   1455   },         /* Gain = 25.863281 Exposure Index = 343 */
         {   6820,   1455   },         /* Gain = 26.640625 Exposure Index = 344 */
         {   7025,   1455   },         /* Gain = 27.441406 Exposure Index = 345 */
         {   7236,   1455   },         /* Gain = 28.265625 Exposure Index = 346 */
         {   7454,   1455   },         /* Gain = 29.117188 Exposure Index = 347 */
         {   7678,   1455   },         /* Gain = 29.992188 Exposure Index = 348 */
         {   7909,   1455   },         /* Gain = 30.894531 Exposure Index = 349 */
         {   8147,   1455   },         /* Gain = 31.824219 Exposure Index = 350 */
         {   8392,   1455   },         /* Gain = 32.781250 Exposure Index = 351 */
         {   8644,   1455   },         /* Gain = 33.765625 Exposure Index = 352 */
         {   8904,   1455   },         /* Gain = 34.781250 Exposure Index = 353 */
         {   9172,   1455   },         /* Gain = 35.828125 Exposure Index = 354 */
         {   9448,   1455   },         /* Gain = 36.906250 Exposure Index = 355 */
         {   9732,   1455   },         /* Gain = 38.015625 Exposure Index = 356 */
         {  10024,   1455   },         /* Gain = 39.156250 Exposure Index = 357 */
         {  10325,   1455   },         /* Gain = 40.332031 Exposure Index = 358 */
         {  10635,   1455   },         /* Gain = 41.542969 Exposure Index = 359 */
         {  10955,   1455   },         /* Gain = 42.792969 Exposure Index = 360 */
         {  11284,   1455   },         /* Gain = 44.078125 Exposure Index = 361 */
         {  11623,   1455   },         /* Gain = 45.402344 Exposure Index = 362 */
         {  11972,   1455   },         /* Gain = 46.765625 Exposure Index = 363 */
         {  12332,   1455   },         /* Gain = 48.171875 Exposure Index = 364 */
         {  12702,   1455   },         /* Gain = 49.617188 Exposure Index = 365 */
         {  13084,   1455   },         /* Gain = 51.109375 Exposure Index = 366 */
         {  13477,   1455   },         /* Gain = 52.644531 Exposure Index = 367 */
         {  13882,   1455   },         /* Gain = 54.226562 Exposure Index = 368 */
         {  14299,   1455   },         /* Gain = 55.855469 Exposure Index = 369 */
         {  14728,   1455   },         /* Gain = 57.531250 Exposure Index = 370 */
         {  15170,   1455   },         /* Gain = 59.257812 Exposure Index = 371 */
         {  15626,   1455   },         /* Gain = 61.039062 Exposure Index = 372 */
         {  16095,   1455   },         /* Gain = 62.871094 Exposure Index = 373 */
         {  16578,   1455   },         /* Gain = 64.757812 Exposure Index = 374 */
         {  17076,   1455   },         /* Gain = 66.703125 Exposure Index = 375 */
         {  17589,   1455   },         /* Gain = 68.707031 Exposure Index = 376 */
         {  18117,   1455   },         /* Gain = 70.769531 Exposure Index = 377 */
         {  18661,   1455   },         /* Gain = 72.894531 Exposure Index = 378 */
         {  19221,   1455   },         /* Gain = 75.082031 Exposure Index = 379 */
         {  19798,   1455   },         /* Gain = 77.335938 Exposure Index = 380 */
         {  20392,   1455   },         /* Gain = 79.656250 Exposure Index = 381 */
         {  21004,   1455   },         /* Gain = 82.046875 Exposure Index = 382 */
         {  21635,   1455   },         /* Gain = 84.511719 Exposure Index = 383 */
         {  22285,   1455   },         /* Gain = 87.050781 Exposure Index = 384 */
         {  22954,   1455   },         /* Gain = 89.664062 Exposure Index = 385 */
         {  23643,   1455   },         /* Gain = 92.355469 Exposure Index = 386 */
         {  24353,   1455   },         /* Gain = 95.128906 Exposure Index = 387 */
         {  25084,   1455   },         /* Gain = 97.984375 Exposure Index = 388 */
         {  25837,   1455   },         /* Gain = 100.925781 Exposure Index = 389 */
         {  26613,   1455   },         /* Gain = 103.957031 Exposure Index = 390 */
         {  27412,   1455   },         /* Gain = 107.078125 Exposure Index = 391 */
         {  28235,   1455   },         /* Gain = 110.292969 Exposure Index = 392 */
         {  29083,   1455   },         /* Gain = 113.605469 Exposure Index = 393 */
         {  29956,   1455   },         /* Gain = 117.015625 Exposure Index = 394 */
         {  30855,   1455   },         /* Gain = 120.527344 Exposure Index = 395 */
         {  31781,   1455   },         /* Gain = 124.144531 Exposure Index = 396 */
         {  32735,   1455   },         /* Gain = 127.871094 Exposure Index = 397 */
         {  33718,   1455   },         /* Gain = 131.710938 Exposure Index = 398 */
         {  34730,   1455   },         /* Gain = 135.664062 Exposure Index = 399 */
         {  35772,   1455   },         /* Gain = 139.734375 Exposure Index = 400 */
         {  36846,   1455   },         /* Gain = 143.929688 Exposure Index = 401 */
         {  37952,   1455   },         /* Gain = 148.250000 Exposure Index = 402 */
         {  39091,   1455   },         /* Gain = 152.699219 Exposure Index = 403 */
         {  40264,   1455   },         /* Gain = 157.281250 Exposure Index = 404 */
         {  41472,   1455   },         /* Gain = 162.000000 Exposure Index = 405 */
         {  42717,   1455   },         /* Gain = 166.863281 Exposure Index = 406 */
         {  43999,   1455   },         /* Gain = 171.871094 Exposure Index = 407 */
         {  45319,   1455   },         /* Gain = 177.027344 Exposure Index = 408 */
         {  46679,   1455   },         /* Gain = 182.339844 Exposure Index = 409 */
         {  48080,   1455   },         /* Gain = 187.812500 Exposure Index = 410 */
         {  49523,   1455   },         /* Gain = 193.449219 Exposure Index = 411 */
         {  51009,   1455   },         /* Gain = 199.253906 Exposure Index = 412 */
         {  52540,   1455   },         /* Gain = 205.234375 Exposure Index = 413 */
         {  54117,   1455   },         /* Gain = 211.394531 Exposure Index = 414 */
         {  55741,   1455   },         /* Gain = 217.738281 Exposure Index = 415 */
         {  57414,   1455   },         /* Gain = 224.273438 Exposure Index = 416 */
         {  59137,   1455   },         /* Gain = 231.003906 Exposure Index = 417 */
         {  60912,   1455   },         /* Gain = 237.937500 Exposure Index = 418 */
         {  62740,   1455   },         /* Gain = 245.078125 Exposure Index = 419 */
         {  64623,   1455   },         /* Gain = 252.433594 Exposure Index = 420 */
         {  66562,   1455   },         /* Gain = 260.007812 Exposure Index = 421 */
         {  68559,   1455   },         /* Gain = 267.808594 Exposure Index = 422 */
         {  70616,   1455   },         /* Gain = 275.843750 Exposure Index = 423 */
         {  72735,   1455   },         /* Gain = 284.121094 Exposure Index = 424 */
         {  74918,   1455   },         /* Gain = 292.648438 Exposure Index = 425 */
         {  77166,   1455   },         /* Gain = 301.429688 Exposure Index = 426 */
         {  79481,   1455   },         /* Gain = 310.472656 Exposure Index = 427 */
         {  81866,   1455   },         /* Gain = 319.789062 Exposure Index = 428 */
         {  84322,   1455   },         /* Gain = 329.382812 Exposure Index = 429 */
         {  86852,   1455   },         /* Gain = 339.265625 Exposure Index = 430 */
         {  89458,   1455   },         /* Gain = 349.445312 Exposure Index = 431 */
         {  92142,   1455   },         /* Gain = 359.929688 Exposure Index = 432 */
         {  94907,   1455   },         /* Gain = 370.730469 Exposure Index = 433 */
         {  97755,   1455   },         /* Gain = 381.855469 Exposure Index = 434 */
         { 100688,   1455   },         /* Gain = 393.312500 Exposure Index = 435 */
         { 103709,   1455   },         /* Gain = 405.113281 Exposure Index = 436 */
         { 106821,   1455   },         /* Gain = 417.269531 Exposure Index = 437 */
         { 110026,   1455   },         /* Gain = 429.789062 Exposure Index = 438 */
         { 113327,   1455   },         /* Gain = 442.683594 Exposure Index = 439 */
         { 116727,   1455   },         /* Gain = 455.964844 Exposure Index = 440 */
         { 120229,   1455   },         /* Gain = 469.644531 Exposure Index = 441 */
         { 123836,   1455   },         /* Gain = 483.734375 Exposure Index = 442 */
         { 127552,   1455   },         /* Gain = 498.250000 Exposure Index = 443 */
         { 131379,   1455   },         /* Gain = 513.199219 Exposure Index = 444 */
         { 135321,   1455   },         /* Gain = 528.597656 Exposure Index = 445 */
         { 139381,   1455   },         /* Gain = 544.457031 Exposure Index = 446 */
         { 143563,   1455   },         /* Gain = 560.792969 Exposure Index = 447 */
         { 147870,   1455   },         /* Gain = 577.617188 Exposure Index = 448 */
         { 152307,   1455   },         /* Gain = 594.949219 Exposure Index = 449 */
         { 156877,   1455   },         /* Gain = 612.800781 Exposure Index = 450 */
         { 161584,   1455   },         /* Gain = 631.187500 Exposure Index = 451 */
         { 166432,   1455   },         /* Gain = 650.125000 Exposure Index = 452 */
         { 171425,   1455   },         /* Gain = 669.628906 Exposure Index = 453 */
         { 176568,   1455   },         /* Gain = 689.718750 Exposure Index = 454 */
         { 181866,   1455   },         /* Gain = 710.414062 Exposure Index = 455 */
         { 187322,   1455   },         /* Gain = 731.726562 Exposure Index = 456 */
         { 192942,   1455   },         /* Gain = 753.679688 Exposure Index = 457 */
         { 198731,   1455   },         /* Gain = 776.292969 Exposure Index = 458 */
         { 204693,   1455   },         /* Gain = 799.582031 Exposure Index = 459 */
         { 210834,   1455   },         /* Gain = 823.570312 Exposure Index = 460 */
         { 217160,   1455   },         /* Gain = 848.281250 Exposure Index = 461 */
         { 223675,   1455   },         /* Gain = 873.730469 Exposure Index = 462 */
         { 230386,   1455   },         /* Gain = 899.945312 Exposure Index = 463 */
         { 237298,   1455   },         /* Gain = 926.945312 Exposure Index = 464 */
         { 244417,   1455   },         /* Gain = 954.753906 Exposure Index = 465 */
         { 251750,   1455   },         /* Gain = 983.398438 Exposure Index = 466 */
      },
      /* enable_short_exp_table */
      1,
      /* short_exp_table */
      {
      //1,800
      //200,800
         {    256,      1   },         /* Gain = 1.000000 Exposure Index = 0 */
         {    264,      1   },         /* Gain = 1.031250 Exposure Index = 1 */
         {    272,      1   },         /* Gain = 1.062500 Exposure Index = 2 */
         {    281,      1   },         /* Gain = 1.097656 Exposure Index = 3 */
         {    290,      1   },         /* Gain = 1.132812 Exposure Index = 4 */
         {    299,      1   },         /* Gain = 1.167969 Exposure Index = 5 */
         {    308,      1   },         /* Gain = 1.203125 Exposure Index = 6 */
         {    318,      1   },         /* Gain = 1.242188 Exposure Index = 7 */
         {    328,      1   },         /* Gain = 1.281250 Exposure Index = 8 */
         {    338,      1   },         /* Gain = 1.320312 Exposure Index = 9 */
         {    349,      1   },         /* Gain = 1.363281 Exposure Index = 10 */
         {    360,      1   },         /* Gain = 1.406250 Exposure Index = 11 */
         {    371,      1   },         /* Gain = 1.449219 Exposure Index = 12 */
         {    383,      1   },         /* Gain = 1.496094 Exposure Index = 13 */
         {    395,      1   },         /* Gain = 1.542969 Exposure Index = 14 */
         {    407,      1   },         /* Gain = 1.589844 Exposure Index = 15 */
         {    420,      1   },         /* Gain = 1.640625 Exposure Index = 16 */
         {    433,      1   },         /* Gain = 1.691406 Exposure Index = 17 */
         {    446,      1   },         /* Gain = 1.742188 Exposure Index = 18 */
         {    460,      1   },         /* Gain = 1.796875 Exposure Index = 19 */
         {    474,      1   },         /* Gain = 1.851562 Exposure Index = 20 */
         {    489,      1   },         /* Gain = 1.910156 Exposure Index = 21 */
         {    504,      1   },         /* Gain = 1.968750 Exposure Index = 22 */
         {    260,      2   },         /* Gain = 1.015625 Exposure Index = 23 */
         {    268,      2   },         /* Gain = 1.046875 Exposure Index = 24 */
         {    277,      2   },         /* Gain = 1.082031 Exposure Index = 25 */
         {    286,      2   },         /* Gain = 1.117188 Exposure Index = 26 */
         {    295,      2   },         /* Gain = 1.152344 Exposure Index = 27 */
         {    304,      2   },         /* Gain = 1.187500 Exposure Index = 28 */
         {    314,      2   },         /* Gain = 1.226562 Exposure Index = 29 */
         {    324,      2   },         /* Gain = 1.265625 Exposure Index = 30 */
         {    334,      2   },         /* Gain = 1.304688 Exposure Index = 31 */
         {    345,      2   },         /* Gain = 1.347656 Exposure Index = 32 */
         {    356,      2   },         /* Gain = 1.390625 Exposure Index = 33 */
         {    367,      2   },         /* Gain = 1.433594 Exposure Index = 34 */
         {    379,      2   },         /* Gain = 1.480469 Exposure Index = 35 */
         {    261,      3   },         /* Gain = 1.019531 Exposure Index = 36 */
         {    269,      3   },         /* Gain = 1.050781 Exposure Index = 37 */
         {    278,      3   },         /* Gain = 1.085938 Exposure Index = 38 */
         {    287,      3   },         /* Gain = 1.121094 Exposure Index = 39 */
         {    296,      3   },         /* Gain = 1.156250 Exposure Index = 40 */
         {    305,      3   },         /* Gain = 1.191406 Exposure Index = 41 */
         {    315,      3   },         /* Gain = 1.230469 Exposure Index = 42 */
         {    325,      3   },         /* Gain = 1.269531 Exposure Index = 43 */
         {    335,      3   },         /* Gain = 1.308594 Exposure Index = 44 */
         {    259,      4   },         /* Gain = 1.011719 Exposure Index = 45 */
         {    267,      4   },         /* Gain = 1.042969 Exposure Index = 46 */
         {    276,      4   },         /* Gain = 1.078125 Exposure Index = 47 */
         {    285,      4   },         /* Gain = 1.113281 Exposure Index = 48 */
         {    294,      4   },         /* Gain = 1.148438 Exposure Index = 49 */
         {    303,      4   },         /* Gain = 1.183594 Exposure Index = 50 */
         {    313,      4   },         /* Gain = 1.222656 Exposure Index = 51 */
         {    258,      5   },         /* Gain = 1.007812 Exposure Index = 52 */
         {    266,      5   },         /* Gain = 1.039062 Exposure Index = 53 */
         {    274,      5   },         /* Gain = 1.070312 Exposure Index = 54 */
         {    283,      5   },         /* Gain = 1.105469 Exposure Index = 55 */
         {    292,      5   },         /* Gain = 1.140625 Exposure Index = 56 */
         {    301,      5   },         /* Gain = 1.175781 Exposure Index = 57 */
         {    259,      6   },         /* Gain = 1.011719 Exposure Index = 58 */
         {    267,      6   },         /* Gain = 1.042969 Exposure Index = 59 */
         {    276,      6   },         /* Gain = 1.078125 Exposure Index = 60 */
         {    285,      6   },         /* Gain = 1.113281 Exposure Index = 61 */
         {    294,      6   },         /* Gain = 1.148438 Exposure Index = 62 */
         {    260,      7   },         /* Gain = 1.015625 Exposure Index = 63 */
         {    268,      7   },         /* Gain = 1.046875 Exposure Index = 64 */
         {    277,      7   },         /* Gain = 1.082031 Exposure Index = 65 */
         {    286,      7   },         /* Gain = 1.117188 Exposure Index = 66 */
         {    258,      8   },         /* Gain = 1.007812 Exposure Index = 67 */
         {    266,      8   },         /* Gain = 1.039062 Exposure Index = 68 */
         {    274,      8   },         /* Gain = 1.070312 Exposure Index = 69 */
         {    283,      8   },         /* Gain = 1.105469 Exposure Index = 70 */
         {    260,      9   },         /* Gain = 1.015625 Exposure Index = 71 */
         {    268,      9   },         /* Gain = 1.046875 Exposure Index = 72 */
         {    277,      9   },         /* Gain = 1.082031 Exposure Index = 73 */
         {    257,     10   },         /* Gain = 1.003906 Exposure Index = 74 */
         {    265,     10   },         /* Gain = 1.035156 Exposure Index = 75 */
         {    273,     10   },         /* Gain = 1.066406 Exposure Index = 76 */
         {    256,     11   },         /* Gain = 1.000000 Exposure Index = 77 */
         {    264,     11   },         /* Gain = 1.031250 Exposure Index = 78 */
         {    272,     11   },         /* Gain = 1.062500 Exposure Index = 79 */
         {    257,     12   },         /* Gain = 1.003906 Exposure Index = 80 */
         {    265,     12   },         /* Gain = 1.035156 Exposure Index = 81 */
         {    273,     12   },         /* Gain = 1.066406 Exposure Index = 82 */
         {    260,     13   },         /* Gain = 1.015625 Exposure Index = 83 */
         {    268,     13   },         /* Gain = 1.046875 Exposure Index = 84 */
         {    257,     14   },         /* Gain = 1.003906 Exposure Index = 85 */
         {    265,     14   },         /* Gain = 1.035156 Exposure Index = 86 */
         {    273,     14   },         /* Gain = 1.066406 Exposure Index = 87 */
         {    263,     15   },         /* Gain = 1.027344 Exposure Index = 88 */
         {    271,     15   },         /* Gain = 1.058594 Exposure Index = 89 */
         {    262,     16   },         /* Gain = 1.023438 Exposure Index = 90 */
         {    270,     16   },         /* Gain = 1.054688 Exposure Index = 91 */
         {    262,     17   },         /* Gain = 1.023438 Exposure Index = 92 */
         {    270,     17   },         /* Gain = 1.054688 Exposure Index = 93 */
         {    263,     18   },         /* Gain = 1.027344 Exposure Index = 94 */
         {    257,     19   },         /* Gain = 1.003906 Exposure Index = 95 */
         {    265,     19   },         /* Gain = 1.035156 Exposure Index = 96 */
         {    260,     20   },         /* Gain = 1.015625 Exposure Index = 97 */
         {    268,     20   },         /* Gain = 1.046875 Exposure Index = 98 */
         {    263,     21   },         /* Gain = 1.027344 Exposure Index = 99 */
         {    259,     22   },         /* Gain = 1.011719 Exposure Index = 100 */
         {    267,     22   },         /* Gain = 1.042969 Exposure Index = 101 */
         {    264,     23   },         /* Gain = 1.031250 Exposure Index = 102 */
         {    261,     24   },         /* Gain = 1.019531 Exposure Index = 103 */
         {    259,     25   },         /* Gain = 1.011719 Exposure Index = 104 */
         {    257,     26   },         /* Gain = 1.003906 Exposure Index = 105 */
         {    265,     26   },         /* Gain = 1.035156 Exposure Index = 106 */
         {    263,     27   },         /* Gain = 1.027344 Exposure Index = 107 */
         {    262,     28   },         /* Gain = 1.023438 Exposure Index = 108 */
         {    261,     29   },         /* Gain = 1.019531 Exposure Index = 109 */
         {    260,     30   },         /* Gain = 1.015625 Exposure Index = 110 */
         {    260,     31   },         /* Gain = 1.015625 Exposure Index = 111 */
         {    260,     32   },         /* Gain = 1.015625 Exposure Index = 112 */
         {    260,     33   },         /* Gain = 1.015625 Exposure Index = 113 */
         {    260,     34   },         /* Gain = 1.015625 Exposure Index = 114 */
         {    261,     35   },         /* Gain = 1.019531 Exposure Index = 115 */
         {    262,     36   },         /* Gain = 1.023438 Exposure Index = 116 */
         {    256,     38   },         /* Gain = 1.000000 Exposure Index = 117 */
         {    257,     39   },         /* Gain = 1.003906 Exposure Index = 118 */
         {    259,     40   },         /* Gain = 1.011719 Exposure Index = 119 */
         {    261,     41   },         /* Gain = 1.019531 Exposure Index = 120 */
         {    257,     43   },         /* Gain = 1.003906 Exposure Index = 121 */
         {    259,     44   },         /* Gain = 1.011719 Exposure Index = 122 */
         {    261,     45   },         /* Gain = 1.019531 Exposure Index = 123 */
         {    258,     47   },         /* Gain = 1.007812 Exposure Index = 124 */
         {    261,     48   },         /* Gain = 1.019531 Exposure Index = 125 */
         {    259,     50   },         /* Gain = 1.011719 Exposure Index = 126 */
         {    257,     52   },         /* Gain = 1.003906 Exposure Index = 127 */
         {    260,     53   },         /* Gain = 1.015625 Exposure Index = 128 */
         {    259,     55   },         /* Gain = 1.011719 Exposure Index = 129 */
         {    258,     57   },         /* Gain = 1.007812 Exposure Index = 130 */
         {    257,     59   },         /* Gain = 1.003906 Exposure Index = 131 */
         {    257,     61   },         /* Gain = 1.003906 Exposure Index = 132 */
         {    257,     63   },         /* Gain = 1.003906 Exposure Index = 133 */
         {    257,     65   },         /* Gain = 1.003906 Exposure Index = 134 */
         {    257,     67   },         /* Gain = 1.003906 Exposure Index = 135 */
         {    258,     69   },         /* Gain = 1.007812 Exposure Index = 136 */
         {    259,     71   },         /* Gain = 1.011719 Exposure Index = 137 */
         {    256,     74   },         /* Gain = 1.000000 Exposure Index = 138 */
         {    257,     76   },         /* Gain = 1.003906 Exposure Index = 139 */
         {    258,     78   },         /* Gain = 1.007812 Exposure Index = 140 */
         {    256,     81   },         /* Gain = 1.000000 Exposure Index = 141 */
         {    258,     83   },         /* Gain = 1.007812 Exposure Index = 142 */
         {    257,     86   },         /* Gain = 1.003906 Exposure Index = 143 */
         {    256,     89   },         /* Gain = 1.000000 Exposure Index = 144 */
         {    258,     91   },         /* Gain = 1.007812 Exposure Index = 145 */
         {    258,     94   },         /* Gain = 1.007812 Exposure Index = 146 */
         {    258,     97   },         /* Gain = 1.007812 Exposure Index = 147 */
         {    258,    100   },         /* Gain = 1.007812 Exposure Index = 148 */
         {    258,    103   },         /* Gain = 1.007812 Exposure Index = 149 */
         {    256,    107   },         /* Gain = 1.000000 Exposure Index = 150 */
         {    257,    110   },         /* Gain = 1.003906 Exposure Index = 151 */
         {    258,    113   },         /* Gain = 1.007812 Exposure Index = 152 */
         {    257,    117   },         /* Gain = 1.003906 Exposure Index = 153 */
         {    256,    121   },         /* Gain = 1.000000 Exposure Index = 154 */
         {    258,    124   },         /* Gain = 1.007812 Exposure Index = 155 */
         {    256,    129   },         /* Gain = 1.000000 Exposure Index = 156 */
         {    256,    133   },         /* Gain = 1.000000 Exposure Index = 157 */
         {    256,    137   },         /* Gain = 1.000000 Exposure Index = 158 */
         {    257,    141   },         /* Gain = 1.003906 Exposure Index = 159 */
         {    256,    146   },         /* Gain = 1.000000 Exposure Index = 160 */
         {    257,    150   },         /* Gain = 1.003906 Exposure Index = 161 */
         {    257,    155   },         /* Gain = 1.003906 Exposure Index = 162 */
         {    257,    160   },         /* Gain = 1.003906 Exposure Index = 163 */
         {    257,    165   },         /* Gain = 1.003906 Exposure Index = 164 */
         {    257,    170   },         /* Gain = 1.003906 Exposure Index = 165 */
         {    256,    176   },         /* Gain = 1.000000 Exposure Index = 166 */
         {    257,    181   },         /* Gain = 1.003906 Exposure Index = 167 */
         {    257,    187   },         /* Gain = 1.003906 Exposure Index = 168 */
         {    257,    193   },         /* Gain = 1.003906 Exposure Index = 169 */
         {    257,    199   },         /* Gain = 1.003906 Exposure Index = 170 */
         {    257,    205   },         /* Gain = 1.003906 Exposure Index = 171 */
         {    256,    212   },         /* Gain = 1.000000 Exposure Index = 172 */
         {    257,    218   },         /* Gain = 1.003906 Exposure Index = 173 */
         {    257,    225   },         /* Gain = 1.003906 Exposure Index = 174 */
         {    257,    232   },         /* Gain = 1.003906 Exposure Index = 175 */
         {    257,    239   },         /* Gain = 1.003906 Exposure Index = 176 */
         {    257,    247   },         /* Gain = 1.003906 Exposure Index = 177 */
         {    257,    255   },         /* Gain = 1.003906 Exposure Index = 178 */
         {    256,    264   },         /* Gain = 1.000000 Exposure Index = 179 */
         {    256,    272   },         /* Gain = 1.000000 Exposure Index = 180 */
         {    256,    281   },         /* Gain = 1.000000 Exposure Index = 181 */
         {    256,    290   },         /* Gain = 1.000000 Exposure Index = 182 */
         {    256,    299   },         /* Gain = 1.000000 Exposure Index = 183 */
         {    256,    308   },         /* Gain = 1.000000 Exposure Index = 184 */
         {    256,    318   },         /* Gain = 1.000000 Exposure Index = 185 */
         {    256,    328   },         /* Gain = 1.000000 Exposure Index = 186 */
         {    256,    338   },         /* Gain = 1.000000 Exposure Index = 187 */
         {    256,    349   },         /* Gain = 1.000000 Exposure Index = 188 */
         {    256,    360   },         /* Gain = 1.000000 Exposure Index = 189 */
         {    256,    371   },         /* Gain = 1.000000 Exposure Index = 190 */
         {    256,    383   },         /* Gain = 1.000000 Exposure Index = 191 */
         {    256,    395   },         /* Gain = 1.000000 Exposure Index = 192 */
         {    256,    407   },         /* Gain = 1.000000 Exposure Index = 193 */
         {    256,    420   },         /* Gain = 1.000000 Exposure Index = 194 */
         {    256,    433   },         /* Gain = 1.000000 Exposure Index = 195 */
         {    256,    446   },         /* Gain = 1.000000 Exposure Index = 196 */
         {    256,    460   },         /* Gain = 1.000000 Exposure Index = 197 */
         {    256,    474   },         /* Gain = 1.000000 Exposure Index = 198 */
         {    256,    489   },         /* Gain = 1.000000 Exposure Index = 199 */
         {    256,    504   },         /* Gain = 1.000000 Exposure Index = 200 */
         {    256,    520   },         /* Gain = 1.000000 Exposure Index = 201 */
         {    256,    536   },         /* Gain = 1.000000 Exposure Index = 202 */
         {    256,    553   },         /* Gain = 1.000000 Exposure Index = 203 */
         {    256,    570   },         /* Gain = 1.000000 Exposure Index = 204 */
         {    256,    588   },         /* Gain = 1.000000 Exposure Index = 205 */
         {    256,    606   },         /* Gain = 1.000000 Exposure Index = 206 */
         {    256,    625   },         /* Gain = 1.000000 Exposure Index = 207 */
         {    256,    644   },         /* Gain = 1.000000 Exposure Index = 208 */
         {    256,    664   },         /* Gain = 1.000000 Exposure Index = 209 */
         {    256,    684   },         /* Gain = 1.000000 Exposure Index = 210 */
         {    256,    705   },         /* Gain = 1.000000 Exposure Index = 211 */
         {    256,    727   },         /* Gain = 1.000000 Exposure Index = 212 */
         {    256,    749   },         /* Gain = 1.000000 Exposure Index = 213 */
         {    256,    772   },         /* Gain = 1.000000 Exposure Index = 214 */
         {    256,    796   },         /* Gain = 1.000000 Exposure Index = 215 */
         {    263,    800   },         /* Gain = 1.027344 Exposure Index = 216 */
         {    271,    800   },         /* Gain = 1.058594 Exposure Index = 217 */
         {    280,    800   },         /* Gain = 1.093750 Exposure Index = 218 */
         {    289,    800   },         /* Gain = 1.128906 Exposure Index = 219 */
         {    298,    800   },         /* Gain = 1.164062 Exposure Index = 220 */
         {    307,    800   },         /* Gain = 1.199219 Exposure Index = 221 */
         {    317,    800   },         /* Gain = 1.238281 Exposure Index = 222 */
         {    327,    800   },         /* Gain = 1.277344 Exposure Index = 223 */
         {    337,    800   },         /* Gain = 1.316406 Exposure Index = 224 */
         {    348,    800   },         /* Gain = 1.359375 Exposure Index = 225 */
         {    359,    800   },         /* Gain = 1.402344 Exposure Index = 226 */
         {    370,    800   },         /* Gain = 1.445312 Exposure Index = 227 */
         {    382,    800   },         /* Gain = 1.492188 Exposure Index = 228 */
         {    394,    800   },         /* Gain = 1.539062 Exposure Index = 229 */
         {    406,    800   },         /* Gain = 1.585938 Exposure Index = 230 */
         {    419,    800   },         /* Gain = 1.636719 Exposure Index = 231 */
         {    432,    800   },         /* Gain = 1.687500 Exposure Index = 232 */
         {    445,    800   },         /* Gain = 1.738281 Exposure Index = 233 */
         {    459,    800   },         /* Gain = 1.792969 Exposure Index = 234 */
         {    473,    800   },         /* Gain = 1.847656 Exposure Index = 235 */
         {    488,    800   },         /* Gain = 1.906250 Exposure Index = 236 */
         {    503,    800   },         /* Gain = 1.964844 Exposure Index = 237 */
         {    519,    800   },         /* Gain = 2.027344 Exposure Index = 238 */
         {    535,    800   },         /* Gain = 2.089844 Exposure Index = 239 */
         {    552,    800   },         /* Gain = 2.156250 Exposure Index = 240 */
         {    569,    800   },         /* Gain = 2.222656 Exposure Index = 241 */
         {    587,    800   },         /* Gain = 2.292969 Exposure Index = 242 */
         {    605,    800   },         /* Gain = 2.363281 Exposure Index = 243 */
         {    624,    800   },         /* Gain = 2.437500 Exposure Index = 244 */
         {    643,    800   },         /* Gain = 2.511719 Exposure Index = 245 */
         {    663,    800   },         /* Gain = 2.589844 Exposure Index = 246 */
         {    683,    800   },         /* Gain = 2.667969 Exposure Index = 247 */
         {    704,    800   },         /* Gain = 2.750000 Exposure Index = 248 */
         {    726,    800   },         /* Gain = 2.835938 Exposure Index = 249 */
         {    748,    800   },         /* Gain = 2.921875 Exposure Index = 250 */
         {    771,    800   },         /* Gain = 3.011719 Exposure Index = 251 */
         {    795,    800   },         /* Gain = 3.105469 Exposure Index = 252 */
         {    819,    800   },         /* Gain = 3.199219 Exposure Index = 253 */
         {    844,    800   },         /* Gain = 3.296875 Exposure Index = 254 */
         {    870,    800   },         /* Gain = 3.398438 Exposure Index = 255 */
         {    897,    800   },         /* Gain = 3.503906 Exposure Index = 256 */
         {    924,    800   },         /* Gain = 3.609375 Exposure Index = 257 */
         {    952,    800   },         /* Gain = 3.718750 Exposure Index = 258 */
         {    981,    800   },         /* Gain = 3.832031 Exposure Index = 259 */
         {   1011,    800   },         /* Gain = 3.949219 Exposure Index = 260 */
         {   1042,    800   },         /* Gain = 4.070312 Exposure Index = 261 */
         {   1074,    800   },         /* Gain = 4.195312 Exposure Index = 262 */
         {   1107,    800   },         /* Gain = 4.324219 Exposure Index = 263 */
         {   1141,    800   },         /* Gain = 4.457031 Exposure Index = 264 */
         {   1176,    800   },         /* Gain = 4.593750 Exposure Index = 265 */
         {   1212,    800   },         /* Gain = 4.734375 Exposure Index = 266 */
         {   1249,    800   },         /* Gain = 4.878906 Exposure Index = 267 */
         {   1287,    800   },         /* Gain = 5.027344 Exposure Index = 268 */
         {   1326,    800   },         /* Gain = 5.179688 Exposure Index = 269 */
         {   1366,    800   },         /* Gain = 5.335938 Exposure Index = 270 */
         {   1407,    800   },         /* Gain = 5.496094 Exposure Index = 271 */
         {   1450,    800   },         /* Gain = 5.664062 Exposure Index = 272 */
         {   1494,    800   },         /* Gain = 5.835938 Exposure Index = 273 */
         {   1539,    800   },         /* Gain = 6.011719 Exposure Index = 274 */
         {   1586,    800   },         /* Gain = 6.195312 Exposure Index = 275 */
         {   1634,    800   },         /* Gain = 6.382812 Exposure Index = 276 */
         {   1684,    800   },         /* Gain = 6.578125 Exposure Index = 277 */
         {   1735,    800   },         /* Gain = 6.777344 Exposure Index = 278 */
         {   1788,    800   },         /* Gain = 6.984375 Exposure Index = 279 */
         {   1842,    800   },         /* Gain = 7.195312 Exposure Index = 280 */
         {   1898,    800   },         /* Gain = 7.414062 Exposure Index = 281 */
         {   1955,    800   },         /* Gain = 7.636719 Exposure Index = 282 */
         {   2014,    800   },         /* Gain = 7.867188 Exposure Index = 283 */
         {   2075,    800   },         /* Gain = 8.105469 Exposure Index = 284 */
         {   2138,    800   },         /* Gain = 8.351562 Exposure Index = 285 */
         {   2203,    800   },         /* Gain = 8.605469 Exposure Index = 286 */
         {   2270,    800   },         /* Gain = 8.867188 Exposure Index = 287 */
         {   2339,    800   },         /* Gain = 9.136719 Exposure Index = 288 */
         {   2410,    800   },         /* Gain = 9.414062 Exposure Index = 289 */
         {   2483,    800   },         /* Gain = 9.699219 Exposure Index = 290 */
         {   2558,    800   },         /* Gain = 9.992188 Exposure Index = 291 */
         {   2635,    800   },         /* Gain = 10.292969 Exposure Index = 292 */
         {   2715,    800   },         /* Gain = 10.605469 Exposure Index = 293 */
         {   2797,    800   },         /* Gain = 10.925781 Exposure Index = 294 */
         {   2881,    800   },         /* Gain = 11.253906 Exposure Index = 295 */
         {   2968,    800   },         /* Gain = 11.593750 Exposure Index = 296 */
         {   3058,    800   },         /* Gain = 11.945312 Exposure Index = 297 */
         {   3150,    800   },         /* Gain = 12.304688 Exposure Index = 298 */
         {   3245,    800   },         /* Gain = 12.675781 Exposure Index = 299 */
         {   3343,    800   },         /* Gain = 13.058594 Exposure Index = 300 */
         {   3444,    800   },         /* Gain = 13.453125 Exposure Index = 301 */
         {   3548,    800   },         /* Gain = 13.859375 Exposure Index = 302 */
         {   3655,    800   },         /* Gain = 14.277344 Exposure Index = 303 */
         {   3765,    800   },         /* Gain = 14.707031 Exposure Index = 304 */
         {   3878,    800   },         /* Gain = 15.148438 Exposure Index = 305 */
         {   3995,    800   },         /* Gain = 15.605469 Exposure Index = 306 */
         {   4115,    800   },         /* Gain = 16.074219 Exposure Index = 307 */
         {   4239,    800   },         /* Gain = 16.558594 Exposure Index = 308 */
         {   4367,    800   },         /* Gain = 17.058594 Exposure Index = 309 */
         {   4499,    800   },         /* Gain = 17.574219 Exposure Index = 310 */
         {   4634,    800   },         /* Gain = 18.101562 Exposure Index = 311 */
         {   4774,    800   },         /* Gain = 18.648438 Exposure Index = 312 */
         {   4918,    800   },         /* Gain = 19.210938 Exposure Index = 313 */
         {   5066,    800   },         /* Gain = 19.789062 Exposure Index = 314 */
         {   5218,    800   },         /* Gain = 20.382812 Exposure Index = 315 */
         {   5375,    800   },         /* Gain = 20.996094 Exposure Index = 316 */
         {   5537,    800   },         /* Gain = 21.628906 Exposure Index = 317 */
         {   5704,    800   },         /* Gain = 22.281250 Exposure Index = 318 */
         {   5876,    800   },         /* Gain = 22.953125 Exposure Index = 319 */
         {   6053,    800   },         /* Gain = 23.644531 Exposure Index = 320 */
         {   6235,    800   },         /* Gain = 24.355469 Exposure Index = 321 */
         {   6423,    800   },         /* Gain = 25.089844 Exposure Index = 322 */
         {   6616,    800   },         /* Gain = 25.843750 Exposure Index = 323 */
         {   6815,    800   },         /* Gain = 26.621094 Exposure Index = 324 */
         {   7020,    800   },         /* Gain = 27.421875 Exposure Index = 325 */
         {   7231,    800   },         /* Gain = 28.246094 Exposure Index = 326 */
         {   7448,    800   },         /* Gain = 29.093750 Exposure Index = 327 */
         {   7672,    800   },         /* Gain = 29.968750 Exposure Index = 328 */
         {   7903,    800   },         /* Gain = 30.871094 Exposure Index = 329 */
         {   8141,    800   },         /* Gain = 31.800781 Exposure Index = 330 */
         {   8386,    800   },         /* Gain = 32.757812 Exposure Index = 331 */
         {   8638,    800   },         /* Gain = 33.742188 Exposure Index = 332 */
         {   8898,    800   },         /* Gain = 34.757812 Exposure Index = 333 */
         {   9165,    800   },         /* Gain = 35.800781 Exposure Index = 334 */
         {   9440,    800   },         /* Gain = 36.875000 Exposure Index = 335 */
         {   9724,    800   },         /* Gain = 37.984375 Exposure Index = 336 */
         {  10016,    800   },         /* Gain = 39.125000 Exposure Index = 337 */
         {  10317,    800   },         /* Gain = 40.300781 Exposure Index = 338 */
         {  10627,    800   },         /* Gain = 41.511719 Exposure Index = 339 */
         {  10946,    800   },         /* Gain = 42.757812 Exposure Index = 340 */
         {  11275,    800   },         /* Gain = 44.042969 Exposure Index = 341 */
         {  11614,    800   },         /* Gain = 45.367188 Exposure Index = 342 */
         {  11963,    800   },         /* Gain = 46.730469 Exposure Index = 343 */
         {  12322,    800   },         /* Gain = 48.132812 Exposure Index = 344 */
         {  12692,    800   },         /* Gain = 49.578125 Exposure Index = 345 */
         {  13073,    800   },         /* Gain = 51.066406 Exposure Index = 346 */
         {  13466,    800   },         /* Gain = 52.601562 Exposure Index = 347 */
         {  13870,    800   },         /* Gain = 54.179688 Exposure Index = 348 */
         {  14287,    800   },         /* Gain = 55.808594 Exposure Index = 349 */
         {  14716,    800   },         /* Gain = 57.484375 Exposure Index = 350 */
         {  15158,    800   },         /* Gain = 59.210938 Exposure Index = 351 */
         {  15613,    800   },         /* Gain = 60.988281 Exposure Index = 352 */
         {  16082,    800   },         /* Gain = 62.820312 Exposure Index = 353 */
         {  16565,    800   },         /* Gain = 64.707031 Exposure Index = 354 */
         {  17062,    800   },         /* Gain = 66.648438 Exposure Index = 355 */
         {  17574,    800   },         /* Gain = 68.648438 Exposure Index = 356 */
         {  18102,    800   },         /* Gain = 70.710938 Exposure Index = 357 */
         {  18646,    800   },         /* Gain = 72.835938 Exposure Index = 358 */
         {  19206,    800   },         /* Gain = 75.023438 Exposure Index = 359 */
         {  19783,    800   },         /* Gain = 77.277344 Exposure Index = 360 */
         {  20377,    800   },         /* Gain = 79.597656 Exposure Index = 361 */
         {  20989,    800   },         /* Gain = 81.988281 Exposure Index = 362 */
         {  21619,    800   },         /* Gain = 84.449219 Exposure Index = 363 */
         {  22268,    800   },         /* Gain = 86.984375 Exposure Index = 364 */
         {  22937,    800   },         /* Gain = 89.597656 Exposure Index = 365 */
         {  23626,    800   },         /* Gain = 92.289062 Exposure Index = 366 */
         {  24335,    800   },         /* Gain = 95.058594 Exposure Index = 367 */
         {  25066,    800   },         /* Gain = 97.914062 Exposure Index = 368 */
      },
   },
   /* ae_tuning_par para */
   {
      1,/* aec_stats_type; 0:BAYER_AEC; 1:HYBRID_AEC */
      0,/* force_exp_forced */
      0.1,/* force_exp_value */
      1,/* preview_iso_enable */
      {
         49,/* outdoor_luma_target_compensated */
         30,/* default_luma_target_compensated */
         30,/* lowlight_luma_target */
         180,/* outdoor_index */
         230,/* indoor_index */
         300,/* lowlight_start_idx */
         450,/* lowlight_end_idx */
         3,/* luma_target_short */
         1,/* enable_luma _target_lut */
         7,/* count */
         /* luma target lut */
         {
            {90,70},
            {150,70},
            {200,70},
            {250,30},
            {300,30},
            {360,30},
            {400,30},
         },
      },
      5,/* snow_scene_detect.extreme_luma_target_offset */
      5,/* backlit_scene_detect.backlight_max_la_luma_target_offset */
      1,/* motion_iso_threshold */
      512,/* R_WEIGHT */
      1024,/* G_WEIGHT */
      512,/* B_WEIGHT */
      11,/* WT_Q */
      {
         40,/* fast_conv.speed */
         2,/* fast_conv.luma_tolerance */
         0,/* fast_conv.frame_skip */
         2,/* fast_conv.fine_adjust_skip */
         0,/* fast_conv.luma_nostable_torlerance */
         0,/* fast_conv.luma_nostable_torlerance_max */
         0,/* fast_conv.settled_to_nosettle_skip_count */
      },
      1,/* metering_type 0:SPOT_METERING,1:CENTER_WEIGHTED,2: SIMPLE_FRAME_AVERAGE */
      0,/* full_sweep_en ,set to 1 to fullsweep exp table */
      77.894,/* exposure_index_adj_step ,NEED 1 / log10(1.03) */
      0,/* 0: no antibanding; 1: 50Hz antibanding; 2: 60Hz antibanding */
      {
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      },
      /* hist_target_t hist_target */
      {
         0,/* hist_target_adjust_enable */
         1.2,/* outdoor_max_target_adjust_ratio */
         0.8,/* outdoor_min_target_adjust_ratio */
         1.2,/* indoor_max_target_adjust_ratio */
         0.8,/* indoor_min_target_adjust_ratio */
         1.2,/* lowlight_max_target_adjust_ratio */
         0.8,/* lowlight_min_target_adjust_ratio */
         0.5,/* target_filter_factor */
         0.6,/* hist_sat_pct */
         0.5,/* hist_dark_pct */
         200,/* hist_sat_low_ref */
         230,/* hist_sat_high_ref */
         3,/* hist_dark_low_ref */
         10,/* hist_dark_high_ref */
      },
      /* over_exp_t over_exp */
      {
         0,/* over_exp_enable */
         245,/* high_luma_region_threshold */
         1,/* outdoor_over_exp_adjust_ratio */
         1,/* indoor_over_exp_adjust_ratio */
         0.9,/* lowlight_over_exp_adjust_ratio */
         15,/* outdoor_over_exp_adjust_offset */
         12,/* indoor_over_exp_adjust_offset */
         10,/* lowlight_over_exp_adjust_offset */
         255,/* outdoor_over_exp_max_count */
         230,/* indoor_over_exp_max_count */
         144,/* lowlight_over_exp_max_count */
         120,/* outdoor_over_exp_min_count */
         100,/* indoor_over_exp_min_count */
         60,/* lowlight_over_exp_min_count */
      },
      /* hdr_tuning_exp_t hdr_tuning_exp */
      {
         /* hdr enable */
         0,
         /* enable_hdr_luma_tartget */
         0,
         /* short_exp_max_index */
         368,
         /* max_exp_ration */
         32,
         /* enble_ration_table */
         1,
         /* ration_talbe_size */
         6,
         /* over_exp_per_high */
         255,
         /* over_exp_per_low */
         245,
         /* adjust_step */
         1,
         /* ration_interp_en */
         1,
         /* ration_table_t ration_table[HDR_RATION_TALBE_SIZE] */
         {
            /* short exp index  ration0 ration1 ration2 ration3 luma_target drc gain max  drc gain min  ceva_drc_gain_max1  luma convegence th */
            {        86,           32,      1,      1,      1,       60,          4,           4,              4,                 5            },
            {       166,           32,      1,      1,      1,       60,          4,           4,              4,                 5            },
            {       266,           16,      1,      1,      1,       60,          4,           4,              4,                 5            },
            {       366,           16,      1,      1,      1,       50,          4,           4,              4,                 5            },
            {       466,           16,      1,      1,      1,       40,          6,           4,              4,                 5            },
            {       566,           16,      1,      1,      1,       30,          6,           4,              4,                 5            },
         },
         /* over_exp_bin_th: max 127 min 0 */
         60,
         /* short_exp_skip_count */
         0,
         /* current_hist_weight */
         0.5,
         /* use_averge_hist */
         0,
         /* short_target_th */
         4,
         /* enable_short_exp_luma_adjust */
         1,
         /* full_sweep_en */
         0,
         /* enable_drc_gain_adjust */
         0,
         /* drc_gain_adjust_factor */
         1.03,
         /* use_over_exp_luma */
         1,
         /* hdr_detect_t hdr_detect */
         {
            0,//en_hdr_detect
            256,//low_luma_hdr
            256,//high_luma_hdr
            256,//low_luma_normal
            256,//high_luma_normal
            50,//skip_count_hdr_detct
            1,//use_lux_index
            340,//low_lux_index_hdr
            0,//high_lux_index_hdr
            0,//low_lux_index
            0,//high_lux_index
         },
         /* en_drc_gain_advance_adjust */
         1,
         /* AEC_HDR_FAST_PARAM_t aec_hdr_fast_param */
         {
         },
         /* max_mid_exp_ration */
         0,
         /* enable_over_cut_long_exp */
         0,
         /* enable_short_gain_cut_max */
         0,
      },
      /* night_detect_t night_detect */
      {
         0,/* enable */
         0,/* lux_index_low */
         0,/* lux_index_high */
      },
      /* motion_detect_t motion_detect */
      {
         0,/* enable */
         0,/* abs_th */
      },
      /* face_aec_pra_t  face_ae */
      {
         0,/* use_face_pra */
         30,/* face_luma_target */
         0.8,/* face_weight */
         15,/* face_skip_count */
         0.5,/* filter_weight */
         0,/* en_face_bright_dark_region */
         0,/* bright_high_th */
         0,/* bright_low_th */
         0,/* dark_high_th */
         0,/* dark_low_th */
         0,/* bright_high_th_w */
         0,/* bright_low_th_w */
         0,/* dark_high_th_w */
         0,/* dark_low_th_w */
         0,/* en_face_luma_cut */
         55,/* face_luma_high */
         40,/* face_luma_low */
         0,/* enable_w_dec */
         1,/* enable_region_percent */
         0.8,/* region_percent_low_limit */
         0.9,/* region_percent_low */
         0.95,/* region_percent_high */
         1,/* region_percent_high_limit */
         0.01,/* region_percent_low_limit_w */
         0.1,/* region_percent_low_w */
         0.5,/* region_percent_high_w */
         2,/* region_percent_high_limit_w */
         0,/* use_software_face_aec_stats */
      },
      /* extreme_color_t extreme_color */
      {
         0,//extreme_color_en
         0,//extreme_color_en
         0,//extreme_color_en
         /* extreme_color_pra_t extreme_color_pra[MIDDLE_TRIGGER_COUNT] */
         {
         },
      },
      /* bright_dark_t bright_dark */
      {
         1,//bright_dark_en
         3,//bright_dark_count
         1,//bright_en
         1,//dark_en
         1,//inter_en
         /* bright_dark_pra_t bright_dark_pra[MIDDLE_TRIGGER_COUNT] */
         {
            {
               100,//lux_index
               15,//dark_th_low
               30,//dark_th_high
               230,//bright_th_low
               255,//bright_th_high
               4,//dark_th_low_w
               4,//dark_th_high_w
               1,//bright_th_low_w
               1,//bright_th_high_w
            },
            {
               160,//lux_index
               10,//dark_th_low
               30,//dark_th_high
               220,//bright_th_low
               240,//bright_th_high
               1,//dark_th_low_w
               2,//dark_th_high_w
               1,//bright_th_low_w
               1,//bright_th_high_w
            },
            {
               190,//lux_index
               15,//dark_th_low
               30,//dark_th_high
               220,//bright_th_low
               240,//bright_th_high
               1,//dark_th_low_w
               1,//dark_th_high_w
               1,//bright_th_low_w
               1,//bright_th_high_w
            },
         },
      },
      0,//aec algo type,0:fast smooth  1:fast plus
      /* AEC_FAST_PLUS_PARAM_t aec_fast_plus_param */
      {
         2,//luma_tolerance
         2,//frame_skip
         2,//ddr_frame_skip_offset
         {5,10,15},//interval_threshold[3]
         {0.65,0.55,0.45},//speed_ratio[3]
         0.35,//speed_ratio_others
      },
      2,//aec hdr algo type,0:org  1:fast  2 fast_advance
      /* aec fast plus param short */
      {
         25,/* fast_conv_short.speed */
         2,/* fast_conv_short.luma_tolerance */
         0,/* fast_conv_short.frame_skip */
         0,/* fast_conv_short.fine_adjust_skip */
      },
      /* bright_dark_t bright_dark_short */
      {
         0,//bright_dark_en
         3,//bright_dark_count
         0,//bright_en
         0,//dark_en
         1,//inter_en
         /* bright_dark_pra_t bright_dark_short_pra[MIDDLE_TRIGGER_COUNT] */
         {
            {
               0,//lux_index
               15,//dark_th_low
               30,//dark_th_high
               200,//bright_th_low
               220,//bright_th_high
               2,//dark_th_low_w
               2,//dark_th_high_w
               1,//bright_th_low_w
               1,//bright_th_high_w
            },
            {
               160,//lux_index
               15,//dark_th_low
               30,//dark_th_high
               200,//bright_th_low
               220,//bright_th_high
               12,//dark_th_low_w
               12,//dark_th_high_w
               1,//bright_th_low_w
               1,//bright_th_high_w
            },
            {
               300,//lux_index
               5,//dark_th_low
               10,//dark_th_high
               200,//bright_th_low
               220,//bright_th_high
               1,//dark_th_low_w
               1,//dark_th_high_w
               1,//bright_th_low_w
               1,//bright_th_high_w
            },
         },
      },
   },
},
/* awb_tuning_t awb_tuning */
{
   /* awb version */
   0x00000001,
   /* awb enable */
   1,
   /* interpolation enable */
   0,
   /*  num_point */
   8,
   /*  count_ae */
   1,
   /*  trigger_mode : 0 gain trigger 1 lux trigger */
   1,
   /* aec trigger */
   {
      /* aec trigger 0 */
      {
         0,
         400,
      },
   },
   /* awb_point */
   {
      { 0.4237, 0.7080 },   /* d75 */
      { 0.4497, 0.6283 },   /* d65 */
      { 0.4970, 0.5340 },   /* d50 */
      { 0.4050, 0.5230 },   /* noon */
      { 0.5550, 0.3720 },   /* cw */
      { 0.7014, 0.4020 },   /* tl84 */
      { 0.7698, 0.3129 },   /* a */
      { 0.9175, 0.2541 },   /* h */
      { 0.0000, 0.0000 },   /* custom1 */
      { 0.0000, 0.0000 },   /* custom2 */
   },
   /* awb_gain_adj */
   {
      { 1.0000, 1.0000 },   /* d75 */
      { 1.0000, 1.0000 },   /* d65 */
      { 1.0500, 1.0000 },   /* d50 */
      { 1.0500, 1.0000 },   /* noon */
      { 1.0500, 1.0000 },   /* cw */
      { 1.0000, 1.0000 },   /* tl84 */
      { 1.0000, 1.0000 },   /* a */
      { 1.0000, 1.0000 },   /* h */
      { 1.0000, 1.0000 },   /* custom1 */
      { 1.0000, 1.0000 },   /* custom2 */
   },
   /* awb cct */
   {
      7442,   /* d75 */
      6575,   /* d65 */
      5045,   /* d50 */
      5045,   /* noon */
      4050,   /* cw */
      3960,   /* tl84 */
      2892,   /* a */
      2336,   /* h */
      0,   /* custom1 */
      0,   /* custom2 */
   },
   /* distance */
   {
   },
   /* index name */
   {
      0,   /* d75 */
      1,   /* d65 */
      2,   /* d50 */
      3,   /* noon */
      4,   /* cw */
      5,   /* tl84 */
      6,   /* a */
      7,   /* h */
      8,   /* custom1 */
      9,   /* custom2 */
   },
   /* light_weight_table */
   {
   },
   /* light_weight_table2 */
   {
      /* Exp Index: 0, Outdoor_Index, Inoutdoor_Midpoint, Indoor_index */
      { 1, 1, 0, 0 },   /* d75 */
      { 3, 3, 2, 1 },   /* d65 */
      { 5, 4, 4, 3 },   /* d50 */
      { 2, 2, 2, 1 },   /* noon */
      { 1, 1, 1, 1 },   /* cw */
      { 2, 3, 3, 2 },   /* tl84 */
      { 0, 1, 1, 1 },   /* a */
      { 0, 0, 0, 0 },   /* h */
      { 0, 0, 0, 0 },   /* custom1 */
      { 0, 0, 0, 0 },   /* custom2 */
   },
   /* awb_lowlight_lut_t awb_lowlight_lut[AWB_LOWLIGHT_LUT_COUNT] */
   {
      /* awb_lowlight_lut 0 */
      {
         440,//lux index
         0,//green_rg_offset_adj
         0,//green_bg_offset_adj
         0,//outlier_dist_adj
         /* ref_point_rg_offset_adjust[AWB_MAX] */
         {
            0,   /* rg d75 */
            0,   /* rg d65 */
            0,   /* rg d50 */
            0,   /* rg noon */
            0,   /* rg cw */
            0,   /* rg tl84 */
            0,   /* rg a */
            0,   /* rg h */
            0,   /* rg custom1 */
            0,   /* rg custom2 */
         },
         /* ref_point_bg_offset_adjust[AWB_MAX] */
         {
            0,   /* bg d75 */
            0,   /* bg d65 */
            0,   /* bg d50 */
            0,   /* bg noon */
            0,   /* bg cw */
            0,   /* bg tl84 */
            0,   /* bg a */
            0,   /* bg h */
            0,   /* bg custom1 */
            0,   /* bg custom2 */
         },
      },
   },
   /* awb_tuning_para */
   {
      60,   /* outdoor_midpoint */
      150,   /* outdoor_index */
      240,   /* inoutdoor_midpoint */
      300,   /* indoor_index */
      10,   /* exposure_adjustment */
      0.75,   /* awb_mesh_stats_percent_th */
      0.5,   /* d50_d65_weighted_sample_boundary */
      0.2,   /* blue_sky_pec */
      0.15,   /* blue_sky_pec_buffer */
      8,   /* num_of_reference_point */
      /* outline */
      {
         1,   /* outline.count */
         /* outline desion */
         {
            /* outline desion region 0 */
            {
               100,   /* lux_index */
               /* outline sub desion */
               {
                  0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,
                  0.0048,0.0048,0.0048,0.0048,0.0048,0.0048,0.0048,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,
                  0.0016,
               },
               /* outline main desion */
               {
                  0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,
               },
            },
         },
      },
      1023,   /* white_stat_y_threshold_low */
      1023,   /* white_stat_y_threshold_high */
      2304,   /* dominant_cluster_threshold */
      2304,   /* white_stat_cnt_th */
      1,   /* grey_weight_day */
      0,   /* white_weight_day */
      1,   /* grey_weight_f */
      0,   /* white_weight_f */
      1,   /* grey_weight_h */
      0,   /* white_weight_h */
      1,   /* all_outlier_heuristic_flag */
      0,   /* special_bayes_en */
      0.2,   /* green_threshold */
      0.2,   /* threshold_extreme_b_percent */
      0.0064,   /* compact_to_grey_dis */
      0.45,   /* cluster_high_pec */
      0.25,   /* cluster_mid_pec */
      0.15,   /* cluster_low_pec */
      10,   /* BitsSum */
      3,   /* FrameNum */
      1,   /* enable_stats_filter */
      1,   /* stats_filter_count */
      /* stats_filter_t input_stats_filter[5] */
      {
         /* stats_filter_count 0 */
         {
            500,//lux index
            0.2,//weight
         },
      },
      1,   /* enable_gain_filter */
      1,   /* awb_gain_filter_count */
      /* stats_filter_t awb_gain_filter_weight[5] */
      {
         /* awb_gain_filter_count 0 */
         {
            500,//lux index
            0.2,//weight
         },
      },
      1,   /* enalbe_limit */
      5,   /* rg_limit_max */
      0.2,   /* rg_limit_min */
      5,   /* bg_limit_max */
      0.2,   /* bg_limit_min */
      2,   /* default_cct */
      2,   /* stats_filter_skip */
      2,   /* gain_filter_skip */
      /* awb_mesh_positon_w_table_t position_w_table */
      {
         1,   /* enable */
         8,   /* count */
         /* awb_mesh_w[16] */
         {
            60,   /* lux_index */
            /* awb_mesh_w[36][64] */
            {
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            },
            120,   /* lux_index */
            /* awb_mesh_w[36][64] */
            {
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            },
            180,   /* lux_index */
            /* awb_mesh_w[36][64] */
            {
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            },
            240,   /* lux_index */
            /* awb_mesh_w[36][64] */
            {
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            },
            300,   /* lux_index */
            /* awb_mesh_w[36][64] */
            {
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            },
            350,   /* lux_index */
            /* awb_mesh_w[36][64] */
            {
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            },
            400,   /* lux_index */
            /* awb_mesh_w[36][64] */
            {
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            },
            450,   /* lux_index */
            /* awb_mesh_w[36][64] */
            {
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            },
         },
      },
   },
   /* awb_statistics stats_awb[MIDDLE_TRIGGER_COUNT] */
   {
      /* awb_statistics 0 */
      {
         0x00000005,//coef_y_r
         0x00000009,//coef_y_g
         0x00000002,//coef_y_b
         0x00000005,//coef_cb_r
         0x0000000b,//coef_cb_g
         0x00000010,//coef_cb_b
         0x00000010,//coef_cr_r
         0x0000000d,//coef_cr_g
         0x00000003,//coef_cr_b
         0x00000004,//coef_rsh
         0x00000000,//rrBL
         0x00000000,//grBL
         0x00000000,//gbBL
         0x00000000,//grBL
         0x00000100,//rrGain
         0x00000100,//grGain
         0x00000100,//gbGain
         0x00000100,//bbGain
         0x000003e8,//Ymax
         0x00000000,//Ymin
         0x00000100,//Gray_m1
         0x00010100,//Gray_m2
         0x00000100,//Gray_m3
         0x00010100,//Gray_m4
         0x00000320,//Gray_c1
         0x00010384,//Gray_c2
         0x00010384,//Gray_c3
         0x00000384,//Gray_c4
         0x0000009e,//Green_BGmax
         0x00000036,//Green_BGmin
         0x00000086,//Green_RGmax
         0x00000000,//Green_RGmin
         0x00000156,//Green_Rmul
         0x0000010f,//Green_Bmul
         0x00001400,//ExtremeB_BGmax
         0x00001400,//ExtremeB_RGmax
         0x00001400,//ExtremeR_RGmax
         0x00001400,//ExtremeR_BGmax
         0,//zoom
         64,//hdr_rrBL
         64,//hdr_grBL
         64,//hdr_gbBL
         64,//hdr_bbBL
         256,//hdr_rrGain
         256,//hdr_grGain
         256,//hdr_gbGain
         256,//hdr_bbGain
      },
   },
   1,//enable_distance_v1
   1,//enable_awb_advace_average
   /* awb_advance_weight_average_t advance_avg */
   {
      10,//luma_count
      /* dist_w_t distance_w */
      {
         16,//distance_w.count
         /* dist_percent[MAX_DISTANCE_COUNT] */
         {
            0,
            0.0625,
            0.125,
            0.1875,
            0.25,
            0.3125,
            0.375,
            0.4375,
            0.5,
            0.5625,
            0.625,
            0.6875,
            0.75,
            0.8125,
            0.875,
            0.9375,
         },
         /* distance_w[MAX_DISTANCE_COUNT] */
         {
            16,
            15,
            14,
            13,
            12,
            11,
            10,
            9,
            8,
            7,
            6,
            5,
            4,
            3,
            2,
            1,
         },
      },
      /* hybrid_decision_w_t  hybrid_decision_w[MAX_LUMA_HYBRID_DESION_W_COUNT] */
      {
         {
            10,//exp_index
            /* hybrid_decision_w */
            {
               1,
               1,
               1,
               3,
               5,
               5,
               5,
               5,
               5,
               4,
               3,
               3,
               3,
               3,
               1,
               1,
               1,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
            },
         },
         {
            50,//exp_index
            /* hybrid_decision_w */
            {
               1,
               1,
               1,
               3,
               5,
               5,
               5,
               5,
               5,
               4,
               3,
               3,
               3,
               3,
               1,
               1,
               1,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
            },
         },
         {
            100,//exp_index
            /* hybrid_decision_w */
            {
               1,
               1,
               1,
               2,
               5,
               5,
               5,
               4,
               5,
               3,
               3,
               3,
               3,
               3,
               1,
               1,
               1,
               1,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
            },
         },
         {
            150,//exp_index
            /* hybrid_decision_w */
            {
               0,
               0,
               1,
               2,
               3,
               3,
               3,
               3,
               5,
               3,
               3,
               3,
               3,
               3,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
            },
         },
         {
            200,//exp_index
            /* hybrid_decision_w */
            {
               0,
               0,
               1,
               1,
               1,
               1,
               2,
               2,
               5,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               0.5,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
            },
         },
         {
            250,//exp_index
            /* hybrid_decision_w */
            {
               0,
               0,
               0,
               1,
               1,
               1,
               2,
               2,
               5,
               1,
               1,
               1,
               1,
               1,
               3,
               2,
               1,
               1,
               1,
               1,
               1,
               1,
               0.5,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
               0,
            },
         },
         {
            300,//exp_index
            /* hybrid_decision_w */
            {
               0,
               0,
               0,
               0,
               1,
               1,
               1,
               2,
               5,
               1,
               1,
               1,
               1,
               1,
               4,
               3,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               0,
               0,
               0,
               0,
            },
         },
         {
            350,//exp_index
            /* hybrid_decision_w */
            {
               0,
               0,
               0,
               0,
               1,
               1,
               1,
               2,
               5,
               1,
               1,
               1,
               1,
               1,
               4,
               3,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               0,
               0,
               0,
               0,
               0,
            },
         },
         {
            400,//exp_index
            /* hybrid_decision_w */
            {
               0,
               0,
               0,
               0,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               2,
               2,
               2,
               2,
               0,
               0,
            },
         },
         {
            440,//exp_index
            /* hybrid_decision_w */
            {
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               0,
               0,
            },
         },
      },
      /* mesh_luma_w_t mesh_luma */
      {
         0,//enable
         0,//dark_low_th
         0,//dark_low_th_w
         0,//dark_high_th
         0,//dark_high_th_w
         0,//bright_low_th
         0,//bright_low_th_w
         0,//bright_high_th
         0,//bright_high_th_w
      },
      1,//enable_no_grey_keep_settle_decision
      1,//enable_keep_no_grey_grey
      0.05,//keep_th_no_grey_to_grey
      0.05,//keep_th_grey_to_no_grey
      0,//enable_subsample
      4,//w_ration
      4,//h_ration
   },
   1,//enable_gain_adjust
   0,//enable_lowlight_lut
   1,//lowlight_lut_count
   0,//force_simple_grey_word
   1,//enable_init_point
   25,//use_init_point_count
   /* init point */
   {
      0.52,//rg
      0.54,//bg
   },
   /* awb_extrem_color_blue */
   {
      0,//enable
      0,//th
      0,//percent_th
      {0,0},//ref_point
   },
   /* awb_extrem_color_red */
   {
      0,//enable
      0,//th
      0,//percent_th
      {0,0},//ref_point
   },
   /* awb_misleading_t */
   {
      1,//misleading.enable
      6,//misleading.aec_count
      /* misleading region */
      {
         /* misleading region 0 */
         {
             80,//lux_index
             0,//misleading_region_count
             /* awb mesh reigon */
             {
             },
         },
         /* misleading region 1 */
         {
             140,//lux_index
             0,//misleading_region_count
             /* awb mesh reigon */
             {
             },
         },
         /* misleading region 2 */
         {
             200,//lux_index
             3,//misleading_region_count
             /* awb mesh reigon */
             {
                /* awb mesh region 0 */
                {
                    4,//point_num
                    0,//is_square
                    0.441,//min_x
                    0.266,//min_y
                    0.503,//max_x
                    0.343,//max_y
                    /* awb mesh point */
                    {
                       {0.45,0.343},
                       {0.48,0.325},
                       {0.441,0.266},
                       {0.503,0.294},
                    },
                },
                /* awb mesh region 1 */
                {
                    0,//point_num
                    0,//is_square
                    0,//min_x
                    0,//min_y
                    0,//max_x
                    0,//max_y
                    /* awb mesh point */
                    {
                       {0,0},
                       {0,0},
                       {0,0},
                       {0,0},
                    },
                },
                /* awb mesh region 2 */
                {
                    0,//point_num
                    0,//is_square
                    0,//min_x
                    0,//min_y
                    0,//max_x
                    0,//max_y
                    /* awb mesh point */
                    {
                       {0,0},
                       {0,0},
                       {0,0},
                       {0,0},
                    },
                },
             },
         },
         /* misleading region 3 */
         {
             300,//lux_index
             3,//misleading_region_count
             /* awb mesh reigon */
             {
                /* awb mesh region 0 */
                {
                    4,//point_num
                    0,//is_square
                    0.7,//min_x
                    0.368,//min_y
                    0.77,//max_x
                    0.4,//max_y
                    /* awb mesh point */
                    {
                       {0.7,0.4},
                       {0.7,0.368},
                       {0.77,0.368},
                       {0.77,0.4},
                    },
                },
                /* awb mesh region 1 */
                {
                    4,//point_num
                    0,//is_square
                    0.615,//min_x
                    0.392,//min_y
                    0.666,//max_x
                    0.51,//max_y
                    /* awb mesh point */
                    {
                       {0.619,0.51},
                       {0.666,0.478},
                       {0.666,0.392},
                       {0.615,0.418},
                    },
                },
                /* awb mesh region 2 */
                {
                    4,//point_num
                    0,//is_square
                    0.58,//min_x
                    0.354,//min_y
                    0.694,//max_x
                    0.476,//max_y
                    /* awb mesh point */
                    {
                       {0.688,0.354},
                       {0.58,0.408},
                       {0.611,0.476},
                       {0.694,0.442},
                    },
                },
             },
         },
         /* misleading region 4 */
         {
             400,//lux_index
             0,//misleading_region_count
             /* awb mesh reigon */
             {
             },
         },
         /* misleading region 5 */
         {
             500,//lux_index
             0,//misleading_region_count
             /* awb mesh reigon */
             {
             },
         },
      },
   },
   /* awb_green_region_tuning_t */
   {
      0,//green_region.enable
      0,//green_region.luma_count
      0,//green_region.enalbe_dta_light_inter
      0,//green_region.green_percent_th
      /* awb_mesh_green_region */
      {
      },
   },
   1,//enable_gain_adj_lux_wb
   /* awb_gain_adjust_lux_wb_t */
   {
      7,//count
      /* awb_gain_adjust_wb_t gain_adj[HIGH_TRIGGER_COUNT] */
      {
         150,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0000 },   /* d75 */
            { 0.9500, 1.1000 },   /* d65 */
            { 0.9500, 1.1000 },   /* d50 */
            { 1.0000, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
            { 1.0000, 0.9500 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 1.0000, 1.0000 },   /* custom1 */
            { 1.0000, 1.0000 },   /* custom2 */
         },
         240,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0000 },   /* d75 */
            { 1.0000, 1.0000 },   /* d65 */
            { 1.0000, 1.0500 },   /* d50 */
            { 1.0000, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
            { 1.0000, 0.9500 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 1.0000, 1.0000 },   /* custom1 */
            { 1.0000, 1.0000 },   /* custom2 */
         },
         270,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0000 },   /* d75 */
            { 1.0000, 1.0000 },   /* d65 */
            { 1.0000, 1.0500 },   /* d50 */
            { 1.0500, 1.0000 },   /* noon */
            { 1.0500, 1.0000 },   /* cw */
            { 1.0000, 1.0000 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 1.0000, 1.0000 },   /* custom1 */
            { 1.0000, 1.0000 },   /* custom2 */
         },
         320,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0000 },   /* d75 */
            { 1.0000, 1.0000 },   /* d65 */
            { 1.0500, 1.0000 },   /* d50 */
            { 1.0500, 1.0000 },   /* noon */
            { 1.0500, 1.0000 },   /* cw */
            { 1.0000, 1.0000 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 1.0000, 1.0000 },   /* custom1 */
            { 1.0000, 1.0000 },   /* custom2 */
         },
         350,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0000 },   /* d75 */
            { 1.0000, 1.0000 },   /* d65 */
            { 1.0500, 1.0300 },   /* d50 */
            { 1.0000, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
            { 1.0000, 1.0000 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 0.0000, 0.0000 },   /* custom1 */
            { 0.0000, 0.0000 },   /* custom2 */
         },
         420,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0000 },   /* d75 */
            { 1.0000, 1.0000 },   /* d65 */
            { 1.0500, 1.0000 },   /* d50 */
            { 1.0000, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
            { 1.0000, 1.0000 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 0.0000, 0.0000 },   /* custom1 */
            { 0.0000, 0.0000 },   /* custom2 */
         },
         445,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0000 },   /* d75 */
            { 1.0000, 1.0000 },   /* d65 */
            { 1.0000, 1.0000 },   /* d50 */
            { 1.0000, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
            { 1.0000, 1.0000 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 0.0000, 0.0000 },   /* custom1 */
            { 0.0000, 0.0000 },   /* custom2 */
         },
      },
   },
},
{
   /* af version */
   0x00000001,
   /* af enable */
   0,
   /* infinite */
   100,
   /* near */
   512,
   /* hyperfocal */
   100,
   /* init_skip */
   5,
   /* algo_type: 0-FOCUS_FULL_SWEEP, 1-FOCUS_SINGLE */
   1,
   /* fullswep_algo_tuning_t full_sweep_tuning_pra */
   {
      4,//code_per_step
      5,//skip_count_after_lens_move
      4,//disable_mv_to_max_pos
   },
   /* af_stats_type 1-AF_STATS_SHARPNES, 2-AF_STATS_FIR, 4-AF_STATS_IIR */
   1051931443,
   /* roi_t roi */
   {
      0.3,//x
      0.3,//y
      0.4,//width
      1,//height
   },
   /* stats_weight_table[AF_BLOCK_ROW*AF_BLOCK_COL] 16x9 */
   {
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 2, 3, 2, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 2, 3, 3, 3, 2, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 2, 3, 3, 3, 3, 3, 2, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 2, 3, 3, 3, 2, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 2, 3, 2, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.4013e-45, 
   },
   /* actuator_tunning_pra_t atcuator */
   {
      0,//enable
      0,//damping_enable
      0,//init_dac
      0,//infinit_dac
      512,//hyperfocal_dac
      0,//near_dac
      0,//dmp_count
      /* damping_t move_to_far_damping[16] */
      {
      },
      /* damping_t move_to_near_damping[16] */
      {
      },
   },
   /* roi_type : 0-GENERAL 1-FACE 2-TOUCH */
   2,
   /* run_mode : 0-INIT 1-CAMERA 2-VIDEO 3-SNAPSHOT */
   1,
   /* af_monit_t af_monit */
   {
      /* af_tuning_sad_t af_par_sad */
      {
         1073741824,//enable
         30,//gain_min
         2,//gain_max
         30,//ref_gain_min
         4.2039e-45,//ref_gain_max
         3,//threshold_min
         2,//threshold_max
         4,//ref_threshold_min
         5,//ref_threshold_max
         5,//frames_to_wait
      },
   },
   /* af_tuning_single_t  single_pra */
   {
      /* single_optic_t optics */
      {
         6,//far_zone
         7,//mid_zone
         8,//near_zone
         9,//init_pos
         10,//far_start_pos
         11,//near_start_pos
         12,//TAF_far_end
         13,//TAF_near_end
         14,//CAF_far_end
         15,//CAF_near_end
         16,//srch_rgn_1
         17,//srch_rgn_2
         18,//srch_rgn_3
         1,//fine_srch_rgn
      },
      /* acturator_type: 0-ACT_TYPE_CLOSELOOP, 1-ACE_TYPE_OPENLOOP */
      512,
      /* index[50] */
      {
         492, 412, 300, 100, 212, 262, 300, 100, 100, 512, 
         100, 512, 100, 512, 300, 200, 100, 50, 0, 0, 
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
      },
      40,//is_hys_comp_needed
      /* single_threshold_t hw */
      {
         1058642330,//hist_dec_dec_thres
         0.85,//drop_thres
         /* dec_dec_3frame */
         {
            0.85,
            0.85,
            0.85,
            0.85,
            0.85,
            0.85,
            0.85,
            0.85,
            0.85,
            0.9,
         },
         /* inc_dec */
         {
            0.9,
            0.9,
            0.9,
            0.9,
            0.9,
            0.9,
            0.9,
            0.9,
            0.9,
            0.85,
         },
         /* inc_dec_3frame */
         {
            0.85,
            0.85,
            0.85,
            0.85,
            0.85,
            0.85,
            0.85,
            0.85,
            0.85,
            0,
         },
         /* dec_dec */
         {
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0.93,
         },
         /* dec_dec_noise */
         {
            0.93,
            0.93,
            0.93,
            0.93,
            0.93,
            0.93,
            0.93,
            0.93,
            0.93,
            0.98,
         },
         /* flat_threshold */
         {
            0.98,
            0.98,
            0.98,
            0.98,
            0.98,
            0.98,
            0.98,
            0.98,
            0.98,
            5.60519e-44,
         },
         1064514355,//hist_inc_dec_thres
         1.05,//flat_dec_thres
         1.3,//flat_inc_thres
         1000,//macro_thres
      },
      /* BV_gain[10] */
      {
         1000,
         1000,
         1000,
         1000,
         1000,
         1000,
         1000,
         1000,
         1000,
         1.4013e-45,
      },
      18,//step_index_per_um
      /* step_size_table_t CAF_step_table */
      {
         /* step_rgn_t   Prescan_low_light */
         {
            18, 18, 18, 18, 17,
         },
         /* step_rgn_t   Prescan_normal_light */
         {
            17, 17, 17, 17, 2,
         },
         /* step_rgn_t   Finescan_low_light */
         {
            2, 2, 2, 2, 3,
         },
         /* step_rgn_t   Finescan_normal_light */
         {
            3, 3, 3, 3, 16,
         },
      },
      /* step_size_table_t TAF_step_table */
      {
         /* step_rgn_t   Prescan_low_light */
         {
            16, 16, 16, 16, 15,
         },
         /* step_rgn_t   Prescan_normal_light */
         {
            15, 15, 15, 15, 4,
         },
         /* step_rgn_t   Finescan_low_light */
         {
            4, 4, 4, 4, 4,
         },
         /* step_rgn_t   Finescan_normal_light */
         {
            4, 4, 4, 4, 3,
         },
      },
      /* skip_frame[3] */
      {
         1,//0-large
         1,//1-small
         1,//2-others
      },
   },
   1,//count_ae
   0,//trigger_mode 0-gain, 1-lux
   0,//interpolation_enable
   /* aec_trigger_t aec_trigger[MIN_TRIGGER_COUNT] */
   {
   /* aec trigger 0 */
   {
      0,
      0,
   },
   },
   /* af_stats_tuning_t af_stats_tuning_pra[MIN_TRIGGER_COUNT] */
   {
      /* para 0 */
      {
         1.12104e-44,//trigger_val not use
         8,//coef_y_b
         8,//coef_y_gb
         8,//coef_y_gr
         2,//coef_y_r
         1,//fv_mode 0-fv_max 1-fv_acc 2-sqr
         0,///ch_sel 0-R 1-GR 2-GB 3-B 4-Y
         0,//zoom_en
         0,//zoom_dwn
         1,//compander_en
         460,//lowfilter_en
         0,//high_luma_th
         0,//blc_b
         0,//blc_gb
         0,//blc_r
         0,//blc_gr
         0,//sharp_th
         0,//sharp_bitshift
         1,//sharp_sqr_bitshift
         /* sharp_filter_coff[39] */
         {
            1, 1, 1, 1, 1023, 1023, 1023, 1023, 1023, 1, 1, 1, 1, 1, 1023, 1023, 1023, 1023, 1023, 
            1, 1, 1, 1, 1, 1023, 1023, 1023, 1023, 1023, 1, 1, 1, 1, 1, 1023, 1023, 1023, 1023, 
            0, 
         },
         0,//fir_th
         0,//fir_bitshift
         1023,//fir_sqr_bitshift
         /* fir_filter_coff[3] */
         {
            2,
            1023,
            1,
         },
         1,//iir1_en
         14,//iir0_en
         14,//iir_bitshift_2
         0,//iir_bitshift_1
         10868,//iir_th
         /* iir_filter_coff[10] */
         {
            240409,
            10868,
            30536,
            247611,
            20111,
            221920,
            20111,
            28277,
            249899,
            0,
         },
         16384,//iir_th
         /* gamma[AF_STATS_TABLE_SIZE] 256 */
         {
            0, 0, 0, 32784, 0, 0, 0, 49184, 0, 0, 0, 65584, 0, 0, 0, 81984, 
            0, 0, 0, 98384, 0, 0, 0, 114784, 0, 0, 0, 131184, 0, 0, 0, 147584, 
            0, 0, 0, 163984, 0, 0, 0, 180384, 0, 0, 0, 196784, 0, 0, 0, 213184, 
            0, 0, 0, 229584, 0, 0, 0, 245984, 0, 0, 0, 262384, 0, 0, 0, 278784, 
            0, 0, 0, 295184, 0, 0, 0, 311584, 0, 0, 0, 327984, 0, 0, 0, 344384, 
            0, 0, 0, 360784, 0, 0, 0, 377184, 0, 0, 0, 393584, 0, 0, 0, 409984, 
            0, 0, 0, 426384, 0, 0, 0, 442784, 0, 0, 0, 459184, 0, 0, 0, 475584, 
            0, 0, 0, 491984, 0, 0, 0, 508384, 0, 0, 0, 524784, 0, 0, 0, 541184, 
            0, 0, 0, 557584, 0, 0, 0, 573984, 0, 0, 0, 590384, 0, 0, 0, 606784, 
            0, 0, 0, 623184, 0, 0, 0, 639584, 0, 0, 0, 655984, 0, 0, 0, 672384, 
            0, 0, 0, 688784, 0, 0, 0, 705184, 0, 0, 0, 721584, 0, 0, 0, 737984, 
            0, 0, 0, 754384, 0, 0, 0, 770784, 0, 0, 0, 787184, 0, 0, 0, 803584, 
            0, 0, 0, 819984, 0, 0, 0, 836384, 0, 0, 0, 852784, 0, 0, 0, 869184, 
            0, 0, 0, 885584, 0, 0, 0, 901984, 0, 0, 0, 918384, 0, 0, 0, 934784, 
            0, 0, 0, 951184, 0, 0, 0, 967584, 0, 0, 0, 983984, 0, 0, 0, 1000384, 
            0, 0, 0, 1016784, 0, 0, 0, 1033184, 0, 0, 0, 1048560, 0, 0, 0, 0, 
         },
         0,//high_luma_percent
      },
   },
},
