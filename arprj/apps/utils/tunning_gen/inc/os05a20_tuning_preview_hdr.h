
/*---------------header of imx307 tuing-------------------------*/

/* version of the tuning header uint32_t header_version*/
0x20201105,
/*tuning_ctl_t tuning_ctl  */
{
  //float aec_trigger_tolerence;
  3,
  //float aec_trigger_tolerence_gain
  0.3,
  //float awb_trigger_tolerence; /
  50,
},
/*isp_tuning_t isp_tuning */
{
  /*isp version */
  0x00000001,
    /*isp_sub_module_raw_crop_tuning_t isp_sub_module_raw_crop_tuning*/
    {
       /*enable:*/
       1,
       /*interpolation enable*/
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
       1,
       /* tigger_mode : 0 gain ,1 lux */
       1,
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             800,
          },
          /* aec trigger 1 */
          {
             120,
             180,
          },
          /* aec trigger 2 */
          {
             200,
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
              64,//blc_short_exp
              64,//blc_mid_exp
              64,//blc_long_exp
              0,//hdr_motion_en_sm
              25,//hdr_alpha_s_th_sm
              867,//hdr_exp_value_th1_sm
              682,//hdr_th2_th1_reverse_sm
              64,//hdr_noise_sp_sm
              76,//hdr_motion_mp_sm
              1,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              36,//hdr_exp_value_th1_sl
              400,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              5,//hdr_motion_mp_sl
              32,//hdr_md_th4_sl
              27,//hdr_md_th9_sl
              19,//hdr_md_th16_sl
              15,//hdr_md_th25_sl
              /* noise_profile_l[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                   2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
              },
              /* noise_profile_m[129] */
              {
                   0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
                   3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
                   4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
              },
              /* noise_profile_s[129] */
              {
                  13, 16, 18, 19, 21, 23, 24, 25, 27, 28, 29, 30, 31, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55,
                  56, 56, 57, 58, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 77,
                  78, 78, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95,
              },
              0,//hdr_policy
              1,//enable_hdr_blc
              50,//mv_fix_th
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
              240,//blc_short_exp
              240,//blc_mid_exp
              240,//blc_long_exp
              0,//hdr_motion_en_sm
              25,//hdr_alpha_s_th_sm
              867,//hdr_exp_value_th1_sm
              682,//hdr_th2_th1_reverse_sm
              64,//hdr_noise_sp_sm
              76,//hdr_motion_mp_sm
              1,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              64,//hdr_exp_value_th1_sl
              512,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              10,//hdr_motion_mp_sl
              32,//hdr_md_th4_sl
              27,//hdr_md_th9_sl
              19,//hdr_md_th16_sl
              15,//hdr_md_th25_sl
              /* noise_profile_l[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                   2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
              },
              /* noise_profile_m[129] */
              {
                   0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
                   3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
                   4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
              },
              /* noise_profile_s[129] */
              {
                  13, 16, 18, 19, 21, 23, 24, 25, 27, 28, 29, 30, 31, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55,
                  56, 56, 57, 58, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 77,
                  78, 78, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95,
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
              240,//blc_short_exp
              240,//blc_mid_exp
              240,//blc_long_exp
              0,//hdr_motion_en_sm
              25,//hdr_alpha_s_th_sm
              867,//hdr_exp_value_th1_sm
              682,//hdr_th2_th1_reverse_sm
              64,//hdr_noise_sp_sm
              76,//hdr_motion_mp_sm
              1,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              64,//hdr_exp_value_th1_sl
              512,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              10,//hdr_motion_mp_sl
              32,//hdr_md_th4_sl
              27,//hdr_md_th9_sl
              19,//hdr_md_th16_sl
              15,//hdr_md_th25_sl
              /* noise_profile_l[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                   2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
              },
              /* noise_profile_m[129] */
              {
                   0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
                   3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
                   4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
              },
              /* noise_profile_s[129] */
              {
                  13, 16, 18, 19, 21, 23, 24, 25, 27, 28, 29, 30, 31, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55,
                  56, 56, 57, 58, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 77,
                  78, 78, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95,
              },
              0,//hdr_policy
              1,//enable_hdr_blc
              78,//mv_fix_th
              8,//mv_fix_value
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
              240,//blc_short_exp
              240,//blc_mid_exp
              240,//blc_long_exp
              0,//hdr_motion_en_sm
              25,//hdr_alpha_s_th_sm
              867,//hdr_exp_value_th1_sm
              682,//hdr_th2_th1_reverse_sm
              64,//hdr_noise_sp_sm
              76,//hdr_motion_mp_sm
              1,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              64,//hdr_exp_value_th1_sl
              512,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              10,//hdr_motion_mp_sl
              32,//hdr_md_th4_sl
              27,//hdr_md_th9_sl
              19,//hdr_md_th16_sl
              15,//hdr_md_th25_sl
              /* noise_profile_l[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                   2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
              },
              /* noise_profile_m[129] */
              {
                   0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
                   3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
                   4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
              },
              /* noise_profile_s[129] */
              {
                  13, 16, 18, 19, 21, 23, 24, 25, 27, 28, 29, 30, 31, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55,
                  56, 56, 57, 58, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 77,
                  78, 78, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95,
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
              240,//blc_short_exp
              240,//blc_mid_exp
              240,//blc_long_exp
              0,//hdr_motion_en_sm
              25,//hdr_alpha_s_th_sm
              867,//hdr_exp_value_th1_sm
              682,//hdr_th2_th1_reverse_sm
              64,//hdr_noise_sp_sm
              76,//hdr_motion_mp_sm
              0,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              63,//hdr_exp_value_th1_sl
              1512,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              10,//hdr_motion_mp_sl
              32,//hdr_md_th4_sl
              27,//hdr_md_th9_sl
              19,//hdr_md_th16_sl
              15,//hdr_md_th25_sl
              /* noise_profile_l[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                   2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
              },
              /* noise_profile_m[129] */
              {
                   0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
                   3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
                   4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
              },
              /* noise_profile_s[129] */
              {
                  13, 16, 18, 19, 21, 23, 24, 25, 27, 28, 29, 30, 31, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55,
                  56, 56, 57, 58, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 77,
                  78, 78, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95,
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
              262,//blc_short_exp
              262,//blc_mid_exp
              262,//blc_long_exp
              0,//hdr_motion_en_sm
              25,//hdr_alpha_s_th_sm
              867,//hdr_exp_value_th1_sm
              682,//hdr_th2_th1_reverse_sm
              64,//hdr_noise_sp_sm
              76,//hdr_motion_mp_sm
              0,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              128,//hdr_exp_value_th1_sl
              2048,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              10,//hdr_motion_mp_sl
              32,//hdr_md_th4_sl
              27,//hdr_md_th9_sl
              19,//hdr_md_th16_sl
              15,//hdr_md_th25_sl
              /* noise_profile_l[129] */
              {
                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                   2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
              },
              /* noise_profile_m[129] */
              {
                   0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
                   3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
                   4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
              },
              /* noise_profile_s[129] */
              {
                  13, 16, 18, 19, 21, 23, 24, 25, 27, 28, 29, 30, 31, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55,
                  56, 56, 57, 58, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 77,
                  78, 78, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95,
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
    /* isp_sub_module_compander_tuning_t isp_sub_module_compander_tuning*/
    {
       1,//enable
       1,//interpolation_enable
       1,//count_ae
       1,//count_awb
       1,//trigger_mode : 0 gain trigger 1 lux trigger
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
             385,
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
             0x00000040,0x00000000,0x000003e8,0x00000578,0x00000640,0x000003e8,0x000000c8,0x00000320,0x00000bb8,0x000000c8,
             0x00000190,0x000003e8,0x000000c8,0x00000320,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
             0x00000000,0x00000020,0x00000060,0x000000a0,0x000000e0,
          },
          /* parameter 2 */
          {
             0x00000040,0x00000000,0x000003e8,0x00000578,0x00000640,0x000003e8,0x000000c8,0x00000320,0x00000bb8,0x000000c8,
             0x00000190,0x000003e8,0x000000c8,0x00000320,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
             0x00000000,0x00000020,0x00000060,0x000000a0,0x000000e0,
          },
          /* parameter 3 */
          {
             0x00000020,0x00000000,0x000003e8,0x00000578,0x00000640,0x000003e8,0x000000c8,0x00000320,0x00000bb8,0x000000c8,
             0x00000190,0x000003e8,0x000000c8,0x00000320,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
             0x00000000,0x00000020,0x00000060,0x000000a0,0x000000e0,
          },
          /* parameter 4 */
          {
             0x0000000c,0x00000000,0x000003e8,0x00000578,0x00000640,0x000003e8,0x000000c8,0x00000320,0x00000bb8,0x000000c8,
             0x00000190,0x000003e8,0x000000c8,0x00000320,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
             0x00000000,0x00000020,0x00000060,0x000000a0,0x000000e0,
          },
       },
    },
    /*isp_sub_module_cac_tuning_t isp_sub_module_cac_tuning*/
    {
        0,//enable
        1,//interpolation enable
        1,//ae count
        0,//0:1/2 size  1:true center_height
		    1,// center_width ratio
		    1,// center_height ratio
        1,//trigger_mode : 0 gain trigger 1 lux trigger
        /* aec trigger */
        {
        	/*trigger 0*/
        	{
        		0,
        		345,
        	},
        	/*trigger 1*/
        	{
        	},
        	/*trigger 2*/
        	{
        	},
        	/*trigger 3*/
        	{
        	},
        	/*trigger 4*/
        	{
        	},
        },
	      /* isp_sub_module_cac_pra pra[5][7] */
        {
		   /* para 0 */
		   {
		      /* rx_p[20] */
		      {
			      -49,447,-218,1228,-428,-5424,-7057,-308,16960,
				    24,-53,-48,-142,197,60,233,-159,584,-1338,
			    },
			    /* bx_p[20] */
			    {
			      98,1995,215,-1563,-498,-60,6672,4439,-3163,-9765,
				    56,246,2908,-268,-538,-244,-270,-2557,1128,-2995,
			    },
		   },
		   /* para 1 */
		   {
		   },
		   /* para 1 */
		   {
		   },
		   /* para 1 */
		   {
		   },
		   /* para 1 */
		   {
		   },
	      },
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
       /* isp_sub_module_rnr_para para[MIDDLE_TRIGGER_COUNT] MIDDLE_TRIGGER_COUNT==5 */
       {
           /* para 0*/
           {
               1,//denoise_layer
               1,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {2,2,2},
                       /* channel 2 */
                       {2,2,2},
                       /* channel 3 */
                       {2,2,2},
                       /* channel 4 */
                       {2,2,2},
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
                       {16,10,6,3},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {2500,1500,600},
                       /* noise profile gain */
                       {16,10,6,3},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {2500,1500,600},
                       /* noise profile gain */
                       {16,10,6,3},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {2500,1500,600},
                       /* noise profile gain */
                       {16,10,6,3},
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
               1,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {4,4,4},
                       /* channel 2 */
                       {4,4,4},
                       /* channel 3 */
                       {4,4,4},
                       /* channel 4 */
                       {4,4,4},
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
           /* para 2*/
           {
               1,//denoise_layer
               1,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {8,8,8},
                       /* channel 2 */
                       {8,8,8},
                       /* channel 3 */
                       {8,8,8},
                       /* channel 4 */
                       {8,8,8},
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
               1,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
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
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {28,28,28},
                       /* channel 2 */
                       {28,28,28},
                       /* channel 3 */
                       {28,28,28},
                       /* channel 4 */
                       {28,28,28},
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
               1,//denoise_layer
               1,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {25,25,25},
                       /* channel 2 */
                       {25,25,25},
                       /* channel 3 */
                       {25,25,25},
                       /* channel 4 */
                       {25,25,25},
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
               1,//denoise_layer
               1,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {30,30,30},
                       /* channel 2 */
                       {30,30,30},
                       /* channel 3 */
                       {30,30,30},
                       /* channel 4 */
                       {30,30,30},
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
               1,//profile_enable
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
               1,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {45,45,45},
                       /* channel 2 */
                       {45,45,45},
                       /* channel 3 */
                       {45,45,45},
                       /* channel 4 */
                       {45,45,45},
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
               1,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {65,65,65},
                       /* channel 2 */
                       {65,65,65},
                       /* channel 3 */
                       {65,65,65},
                       /* channel 4 */
                       {65,65,65},
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
    /* isp_sub_module_decompader_tuning_t isp_sub_module_decompader_tuning*/
    {
       1,//enable
       1,//interpolation_enable
       1,//count_ae
       1,//count_awb
       1,//trigger_mode : 0 gain trigger 1 lux trigger
    },
    /* isp_sub_module_lsc_tuning_t isp_sub_module_lsc_tuning*/
    {
        1,//enable
        1,//interpolation enable
		    2,//count ae
        4,//awb count
		    1,//trigger_mode : 0 gain trigger 1 lux trigger
		{
     		/*aec trigger 1*/
     		{
     			0,
     			300,
     		},
			/*aec trigger 2*/
     		{
     			390,
     			500,
     		},
		},
        {
     		/*awb trigger 1*/
     		{
     			2700,
     			2900,
     		},
     		/*awb trigger 2*/
     		{
     			3700,
     			4300,
     		},
     		/*awb trigger 3*/
     		{
     			4700,
     			5300,
     		},
     		/*awb trigger 4*/
     		{
     			6200,
     			6800,
     		},
     		/*awb trigger 5*/
     		{
     			0,
     			0,
     		},
     		/*awb trigger 6*/
     		{
     			0,
     			0,
     		},
        },
        /* isp_sub_module_lsc_pra */
        {
		  /* aec trigger 0 */
          {
     		 /* para 1 */
     		 {
               	0.350000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 2 */
     		 {
               	0.350000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 3 */
     		 {
               	0.350000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 4 */
     		 {
               	0.350000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 5 */
     		 {
     		 },
     		 /* para 6 */
     		 {
     		 },
          },
		  /* aec trigger 1 */
          {
     		 /* para 1 */
     		 {
               	0.0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 2 */
     		 {
               	0.0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 3 */
     		 {
               	0.0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 4 */
     		 {
               	0.0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 5 */
     		 {
     		 },
     		 /* para 6 */
     		 {
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
             155,
          },
          /* aec trigger 1 */
          {
             160,
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
             470,
             500,
          },
       },
       /* isp_sub_module_drc_pra pra[MIDDLE_TRIGGER_COUNT] */
       {
           /* para 0*/
           {
              /* drc_low_LUT_point */
              {
                   120523,120523,116804,114027,111738,109758,107998,106405,104942,103587,102321,101131,100007,98940,97924,96954,96025,95133,94274,93446,92647,91874,91125,90399,89694,
                   89009,88343,87694,87061,86444,85842,85254,84679,84117,83567,83028,82501,81983,81476,80979,80491,80012,79541,79079,78624,78178,77738,77306,76881,76462,
                   76050,75644,75244,74850,74462,74079,73702,73330,72963,72601,72243,71891,71543,71199,70860,70525,70194,69867,69544,69225,68909,68597,68289,67984,67683,
                   67385,67090,66798,66510,66224,65942,65662,65386,65112,64841,64573,64307,64044,63783,63525,63270,63017,62766,62517,62271,62027,61786,61546,61309,61074,
                   60840,60609,60380,60153,59928,59704,59483,59263,59045,58829,58615,58403,58192,57983,57775,57569,57365,57162,56961,56762,56563,56367,56172,55978,55786,
                   55595,55406,55218,55031,54846,54662,54479,54298,54118,53939,53761,53585,53410,53236,53063,52892,52721,52552,52384,52217,52051,51886,51722,51559,51397,
                   51237,51077,50918,50761,50604,50448,50294,50140,49987,49835,49684,49534,49385,49237,49090,48943,48798,48653,48509,48366,48224,48082,47942,47802,47663,
                   47525,47388,47251,47115,46980,46846,46712,46579,46447,46316,46185,46055,45926,45798,45670,45543,45416,45290,45165,45041,44917,44794,44671,44549,44428,
                   44307,44187,44068,43949,43831,43713,43596,43480,43364,43249,43134,43020,42906,42793,42681,42569,42458,42347,42237,42127,42018,41909,41801,41693,41586,
                   41479,41373,41268,41163,41058,40954,40850,40747,40644,40542,40440,40339,40238,40138,40038,39938,39839,39740,39642,39545,39447,39350,39254,39158,39062,
                   38967,38872,38778,38684,38591,38497,38405,
              },
              /* drc_high_LUT_point */
              {
                   96021,96021,84675,76876,70855,65936,61780,58186,55025,52211,49679,47382,45284,43358,41580,39932,38399,36968,35627,34369,33185,32069,31013,30014,29067,
                   28167,27311,26496,25718,24975,24266,23587,22936,22312,21714,21139,20587,20056,19544,19052,18577,18119,17677,17250,16838,16440,16055,15682,15321,14971,
                   14633,14305,13986,13678,13378,13087,12805,12531,12264,12005,11753,11508,11269,11037,10811,10591,10377,10168,9965,9766,9573,9384,9200,9021,8846,
                   8675,8508,8345,8186,8030,7878,7730,7584,7443,7304,7168,7035,6906,6779,6654,6533,6414,6297,6183,6071,5962,5854,5749,5646,5545,
                   5447,5350,5255,5161,5070,4981,4893,4806,4722,4639,4558,4478,4399,4322,4247,4173,4100,4029,3958,3889,3822,3755,3690,3626,3563,
                   3501,3440,3380,3321,3263,3206,3150,3095,3041,2988,2936,2885,2834,2784,2735,2687,2640,2593,2547,2502,2458,2414,2371,2329,2287,
                   2246,2206,2166,2127,2088,2050,2013,1976,1939,1904,1868,1834,1800,1766,1733,1700,1668,1636,1605,1574,1544,1514,1484,1455,1426,
                   1398,1370,1343,1315,1289,1262,1236,1211,1185,1161,1136,1112,1088,1064,1041,1018, 995, 973, 951, 929, 908, 886, 865, 845, 824,
                    804, 784, 765, 745, 726, 708, 689, 671, 652, 635, 617, 599, 582, 565, 548, 532, 515, 499, 483, 468, 452, 437, 421, 406, 392,
                    377, 362, 348, 334, 320, 306, 293, 279, 266, 253, 240, 227, 214, 202, 189, 177, 165, 153, 141, 130, 118, 107,  95,  84,  73,
                     62,  52,  41,  30,  20,  10,   0,
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
              16,
              /* k2 */
              4.8,
              /* k3 */
              0.5,
           },
           /* para 1*/
           {
              /* drc_low_LUT_point */
              {
                   120121,120121,116122,113144,110692,108576,106697,104998,103440,101998,100653,99390,98198,97067,95992,94966,93983,93041,92135,91262,90419,89605,88817,88054,87313,
                   86594,85894,85213,84550,83904,83273,82657,82056,81468,80894,80331,79780,79241,78712,78194,77685,77187,76697,76216,75744,75279,74823,74374,73933,73499,
                   73072,72651,72237,71829,71427,71031,70641,70256,69877,69503,69134,68770,68411,68057,67707,67362,67021,66685,66352,66024,65699,65379,65062,64749,64440,
                   64134,63831,63532,63236,62944,62654,62368,62085,61804,61527,61253,60981,60712,60446,60182,59921,59663,59407,59154,58903,58654,58408,58164,57922,57682,
                   57445,57210,56977,56746,56517,56290,56065,55842,55620,55401,55184,54968,54754,54542,54332,54123,53916,53711,53507,53305,53105,52906,52709,52513,52319,
                   52126,51935,51745,51557,51370,51184,51000,50817,50635,50455,50276,50099,49922,49747,49573,49400,49229,49059,48890,48722,48555,48389,48225,48061,47899,
                   47738,47577,47418,47260,47103,46947,46792,46638,46485,46333,46182,46032,45882,45734,45587,45440,45295,45150,45006,44864,44722,44580,44440,44301,44162,
                   44024,43887,43751,43616,43481,43347,43214,43082,42950,42820,42690,42560,42432,42304,42177,42050,41925,41800,41675,41552,41429,41306,41185,41064,40944,
                   40824,40705,40587,40469,40352,40235,40119,40004,39889,39775,39662,39549,39436,39325,39214,39103,38993,38883,38774,38666,38558,38451,38344,38238,38132,
                   38027,37922,37818,37714,37611,37509,37407,37305,37204,37103,37003,36903,36804,36705,36607,36509,36411,36315,36218,36122,36026,35931,35837,35742,35648,
                   35555,35462,35369,35277,35186,35094,35003,
              },
              /* drc_high_LUT_point */
              {
                   93980,93980,82052,73928,67702,62649,58402,54748,51551,48716,46176,43881,41794,39884,38126,36503,34998,33597,32289,31065,29916,28836,27818,26857,25947,
                   25086,24269,23492,22753,22049,21378,20737,20125,19539,18978,18440,17925,17430,16954,16497,16058,15635,15227,14835,14456,14091,13738,13397,13068,12750,
                   12443,12145,11857,11578,11308,11046,10792,10545,10306,10075,9849,9631,9419,9212,9012,8817,8627,8442,8263,8088,7918,7753,7591,7434,7281,
                   7132,6986,6844,6706,6571,6439,6310,6185,6062,5943,5826,5712,5600,5492,5385,5281,5179,5080,4983,4888,4795,4704,4615,4527,4442,
                   4359,4277,4197,4119,4042,3967,3893,3821,3750,3681,3613,3547,3481,3417,3355,3293,3233,3174,3116,3059,3003,2948,2895,2842,2790,
                   2739,2690,2641,2593,2545,2499,2453,2409,2365,2322,2279,2238,2197,2157,2117,2078,2040,2003,1966,1930,1894,1859,1824,1791,1757,
                   1725,1692,1661,1629,1599,1569,1539,1510,1481,1453,1425,1398,1371,1344,1318,1292,1267,1242,1218,1193,1170,1146,1123,1100,1078,
                   1056,1034,1013, 992, 971, 951, 931, 911, 891, 872, 853, 834, 816, 798, 780, 762, 745, 728, 711, 694, 678, 661, 646, 630, 614,
                    599, 584, 569, 554, 540, 526, 511, 498, 484, 470, 457, 444, 431, 418, 406, 393, 381, 369, 357, 345, 333, 322, 310, 299, 288,
                    277, 266, 256, 245, 235, 225, 215, 205, 195, 185, 175, 166, 157, 147, 138, 129, 120, 112, 103,  94,  86,  78,  69,  61,  53,
                     45,  37,  30,  22,  14,   7,   0,
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
              16,
              /* k2 */
              5.2,
              /* k3 */
              0.5,
           },
           /* para 2*/
           {
              /* drc_low_LUT_point */
              {
                   119607,119607,115333,112156,109546,107297,105303,103502,101854,100329,98908,97575,96317,95127,93995,92916,91884,90894,89943,89028,88146,87294,86469,85671,84897,
                   84146,83416,82706,82015,81341,80685,80045,79419,78809,78211,77628,77056,76497,75949,75412,74885,74369,73862,73365,72877,72397,71926,71463,71008,70560,
                   70120,69687,69260,68840,68427,68020,67619,67224,66834,66450,66072,65698,65330,64967,64609,64255,63906,63562,63222,62886,62554,62227,61903,61583,61267,
                   60955,60647,60342,60040,59742,59447,59156,58867,58582,58300,58021,57745,57472,57201,56934,56669,56407,56147,55890,55636,55384,55134,54887,54642,54400,
                   54160,53922,53686,53452,53221,52992,52764,52539,52316,52095,51875,51658,51442,51228,51017,50806,50598,50391,50187,49983,49782,49582,49384,49187,48992,
                   48798,48606,48416,48226,48039,47853,47668,47485,47303,47122,46943,46765,46588,46413,46239,46066,45895,45725,45556,45388,45221,45056,44891,44728,44566,
                   44405,44245,44087,43929,43773,43617,43463,43309,43157,43006,42855,42706,42557,42410,42263,42118,41973,41830,41687,41545,41404,41264,41125,40986,40849,
                   40712,40576,40441,40307,40174,40041,39909,39779,39648,39519,39390,39262,39135,39009,38883,38758,38634,38511,38388,38266,38145,38024,37904,37785,37666,
                   37548,37431,37314,37198,37083,36968,36854,36740,36627,36515,36404,36292,36182,36072,35963,35854,35746,35638,35531,35425,35319,35214,35109,35005,34901,
                   34798,34695,34593,34492,34391,34290,34190,34090,33991,33893,33795,33697,33600,33503,33407,33312,33216,33122,33027,32934,32840,32747,32655,32563,32471,
                   32380,32289,32199,32109,32020,31931,31842,
              },
              /* drc_high_LUT_point */
              {
                   91880,91880,79415,71003,64603,59442,55128,51436,48221,45382,42849,40570,38505,36622,34896,33306,31837,30474,29205,28021,26914,25875,24898,23979,23111,
                   22292,21516,20780,20082,19419,18788,18187,17614,17066,16544,16044,15565,15107,14668,14246,13842,13453,13080,12720,12375,12042,11721,11412,11113,10825,
                   10548,10279,10020,9769,9527,9292,9065,8845,8633,8426,8226,8033,7845,7662,7485,7314,7147,6985,6827,6674,6526,6381,6240,6103,5970,
                   5841,5715,5592,5472,5356,5242,5132,5024,4919,4816,4716,4619,4523,4431,4340,4252,4165,4081,3999,3918,3840,3763,3688,3615,3543,
                   3473,3404,3337,3272,3208,3145,3084,3024,2965,2908,2851,2796,2742,2689,2638,2587,2537,2489,2441,2395,2349,2304,2260,2217,2175,
                   2133,2093,2053,2014,1976,1938,1901,1865,1830,1795,1761,1727,1694,1662,1630,1599,1569,1539,1509,1480,1452,1424,1397,1370,1343,
                   1317,1292,1267,1242,1218,1194,1171,1148,1125,1103,1081,1059,1038,1017, 997, 977, 957, 938, 919, 900, 881, 863, 845, 828, 810,
                    793, 777, 760, 744, 728, 712, 697, 681, 666, 651, 637, 623, 609, 595, 581, 567, 554, 541, 528, 516, 503, 491, 479, 467, 455,
                    443, 432, 421, 410, 399, 388, 378, 367, 357, 347, 337, 327, 317, 307, 298, 289, 280, 271, 262, 253, 244, 236, 227, 219, 211,
                    203, 195, 187, 179, 171, 164, 156, 149, 142, 135, 128, 121, 114, 107, 100,  94,  87,  81,  75,  68,  62,  56,  50,  44,  38,
                     33,  27,  21,  16,  10,   5,   0,
              },
              /* saturation */
              85,
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
              16,
              /* k2 */
              5.6,
              /* k3 */
              0.5,
           },
           /* para 3*/
           {
              /* drc_low_LUT_point */
              {
                   125689,125689,123236,121284,119607,118111,116748,115487,114309,113200,112149,111148,110192,109275,108393,107543,106721,105926,105155,104406,103678,102969,102279,101605,100948,
                   100305,99676,99061,98459,97869,97290,96722,96165,95618,95080,94552,94033,93522,93019,92525,92038,91558,91085,90619,90160,89708,89261,88821,88386,87957,
                   87534,87116,86704,86296,85893,85495,85102,84714,84330,83950,83574,83203,82836,82472,82113,81757,81405,81057,80712,80371,80033,79698,79367,79039,78713,
                   78391,78072,77756,77443,77133,76826,76521,76219,75919,75623,75328,75037,74747,74461,74176,73894,73614,73337,73061,72788,72517,72249,71982,71717,71455,
                   71194,70936,70679,70424,70171,69920,69671,69424,69179,68935,68693,68453,68214,67977,67742,67508,67276,67046,66817,66589,66364,66139,65916,65695,65475,
                   65257,65040,64824,64610,64397,64185,63975,63766,63559,63353,63148,62944,62741,62540,62340,62141,61944,61747,61552,61358,61165,60973,60782,60593,60404,
                   60217,60030,59845,59661,59477,59295,59114,58934,58755,58577,58399,58223,58048,57874,57700,57528,57356,57186,57016,56847,56680,56513,56347,56181,56017,
                   55853,55691,55529,55368,55208,55048,54890,54732,54575,54419,54263,54109,53955,53802,53649,53498,53347,53197,53048,52899,52751,52604,52457,52311,52166,
                   52022,51878,51735,51593,51451,51310,51169,51030,50890,50752,50614,50477,50340,50204,50069,49934,49800,49667,49534,49401,49270,49138,49008,48878,48749,
                   48620,48491,48364,48237,48110,47984,47858,47733,47609,47485,47362,47239,47116,46995,46873,46753,46632,46513,46393,46275,46156,46039,45921,45804,45688,
                   45572,45457,45342,45228,45114,45000,44887,
              },
              /* drc_high_LUT_point */
              {
                   106718,106718,96161,88381,82107,76820,72242,68207,64603,61351,58393,55684,53190,50883,48741,46746,44880,43132,41489,39942,38483,37104,35798,34560,33384,
                   32267,31203,30189,29222,28298,27416,26571,25762,24987,24244,23531,22846,22188,21556,20947,20361,19797,19253,18729,18223,17735,17264,16809,16370,15945,
                   15534,15136,14752,14380,14019,13670,13332,13004,12686,12377,12078,11788,11506,11233,10967,10709,10459,10215,9978,9748,9525,9307,9095,8889,8689,
                   8493,8303,8118,7938,7763,7592,7425,7263,7104,6950,6799,6653,6510,6370,6234,6101,5971,5844,5721,5600,5482,5367,5255,5145,5038,
                   4933,4831,4731,4633,4537,4444,4352,4263,4176,4090,4007,3925,3845,3767,3690,3615,3542,3470,3400,3331,3264,3198,3133,3070,3008,
                   2947,2888,2830,2773,2717,2662,2609,2556,2505,2454,2405,2356,2309,2262,2216,2171,2127,2084,2042,2001,1960,1920,1881,1842,1805,
                   1768,1732,1696,1661,1627,1593,1560,1528,1496,1465,1434,1404,1374,1345,1317,1289,1261,1234,1208,1181,1156,1131,1106,1082,1058,
                   1034,1011, 989, 967, 945, 923, 902, 881, 861, 841, 821, 802, 783, 764, 746, 728, 710, 692, 675, 658, 642, 625, 609, 593, 578,
                    563, 548, 533, 518, 504, 490, 476, 462, 449, 436, 423, 410, 397, 385, 373, 361, 349, 337, 326, 314, 303, 292, 282, 271, 261,
                    251, 240, 230, 221, 211, 202, 192, 183, 174, 165, 156, 148, 139, 131, 123, 114, 106,  98,  91,  83,  75,  68,  61,  54,  46,
                     39,  33,  26,  19,  12,   6,   0,
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
              16,
              /* k2 */
              5.6,
              /* k3 */
              0.6,
           },
           /* para 4*/
           {
              /* drc_low_LUT_point */
              {
                   127335,127335,125520,124031,122724,121540,120447,119425,118461,117546,116673,115835,115030,114253,113502,112774,112068,111381,110712,110060,109423,108801,108193,107598,107015,
                   106443,105883,105333,104792,104261,103739,103226,102721,102224,101734,101252,100777,100308,99846,99391,98941,98498,98060,97628,97201,96779,96363,95951,95544,95142,
                   94744,94351,93962,93577,93196,92819,92447,92078,91712,91351,90992,90638,90286,89938,89594,89252,88914,88578,88246,87916,87590,87266,86945,86627,86311,
                   85998,85688,85380,85075,84772,84472,84173,83878,83584,83293,83004,82717,82432,82150,81869,81591,81314,81040,80767,80496,80228,79961,79696,79433,79171,
                   78912,78654,78398,78143,77891,77640,77390,77142,76896,76652,76408,76167,75927,75688,75451,75216,74981,74749,74517,74287,74059,73832,73606,73381,73158,
                   72936,72716,72496,72278,72061,71846,71631,71418,71206,70995,70786,70577,70370,70164,69958,69754,69552,69350,69149,68949,68751,68553,68357,68161,67967,
                   67773,67581,67389,67199,67009,66821,66633,66446,66260,66076,65892,65709,65527,65345,65165,64985,64807,64629,64452,64276,64101,63927,63753,63580,63409,
                   63237,63067,62898,62729,62561,62394,62227,62062,61897,61733,61569,61407,61245,61084,60923,60763,60604,60446,60288,60131,59975,59819,59664,59510,59357,
                   59204,59051,58900,58749,58598,58449,58300,58151,58004,57856,57710,57564,57419,57274,57130,56986,56843,56701,56559,56418,56277,56137,55998,55859,55720,
                   55582,55445,55308,55172,55037,54902,54767,54633,54499,54366,54234,54102,53971,53840,53709,53579,53450,53321,53193,53065,52937,52810,52684,52558,52432,
                   52307,52182,52058,51935,51812,51689,51567,
              },
              /* drc_high_LUT_point */
              {
                   112065,112065,102717,95539,89588,84466,79955,75920,72271,68942,65885,63060,60438,57996,55713,53572,51559,49662,47871,46177,44571,43047,41599,40221,38907,
                   37654,36457,35314,34219,33171,32167,31204,30280,29392,28539,27718,26929,26169,25437,24731,24051,23395,22761,22150,21559,20988,20437,19903,19387,18888,
                   18404,17936,17483,17043,16617,16205,15804,15416,15039,14673,14318,13974,13639,13314,12998,12691,12392,12102,11820,11546,11279,11019,10766,10520,10281,
                   10047,9820,9599,9383,9173,8969,8769,8575,8386,8201,8021,7845,7674,7507,7343,7184,7029,6877,6729,6585,6444,6306,6172,6040,5912,
                   5786,5664,5544,5427,5313,5201,5092,4985,4881,4779,4679,4582,4486,4393,4301,4212,4125,4039,3955,3873,3793,3715,3638,3563,3489,
                   3417,3347,3277,3210,3144,3079,3015,2953,2892,2832,2773,2716,2660,2605,2551,2498,2446,2395,2345,2296,2249,2202,2156,2110,2066,
                   2023,1980,1938,1897,1857,1818,1779,1741,1704,1667,1632,1597,1562,1528,1495,1462,1430,1399,1368,1338,1308,1279,1250,1222,1195,
                   1168,1141,1115,1089,1064,1039,1015, 991, 968, 945, 922, 900, 878, 857, 835, 815, 794, 774, 755, 736, 717, 698, 680, 662, 644,
                    627, 609, 593, 576, 560, 544, 528, 513, 498, 483, 468, 454, 440, 426, 412, 399, 385, 372, 359, 347, 334, 322, 310, 298, 287,
                    275, 264, 253, 242, 232, 221, 211, 201, 191, 181, 171, 162, 152, 143, 134, 125, 116, 107,  99,  90,  82,  74,  66,  58,  50,
                     43,  35,  28,  21,  14,   6,   0,
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
              16,
              /* k2 */
              5.6,
              /* k3 */
              0.65,
           },
           /* para 5*/
           {
              /* drc_low_LUT_point */
              {
                   127335,127335,125520,124031,122724,121540,120447,119425,118461,117546,116673,115835,115030,114253,113502,112774,112068,111381,110712,110060,109423,108801,108193,107598,107015,
                   106443,105883,105333,104792,104261,103739,103226,102721,102224,101734,101252,100777,100308,99846,99391,98941,98498,98060,97628,97201,96779,96363,95951,95544,95142,
                   94744,94351,93962,93577,93196,92819,92447,92078,91712,91351,90992,90638,90286,89938,89594,89252,88914,88578,88246,87916,87590,87266,86945,86627,86311,
                   85998,85688,85380,85075,84772,84472,84173,83878,83584,83293,83004,82717,82432,82150,81869,81591,81314,81040,80767,80496,80228,79961,79696,79433,79171,
                   78912,78654,78398,78143,77891,77640,77390,77142,76896,76652,76408,76167,75927,75688,75451,75216,74981,74749,74517,74287,74059,73832,73606,73381,73158,
                   72936,72716,72496,72278,72061,71846,71631,71418,71206,70995,70786,70577,70370,70164,69958,69754,69552,69350,69149,68949,68751,68553,68357,68161,67967,
                   67773,67581,67389,67199,67009,66821,66633,66446,66260,66076,65892,65709,65527,65345,65165,64985,64807,64629,64452,64276,64101,63927,63753,63580,63409,
                   63237,63067,62898,62729,62561,62394,62227,62062,61897,61733,61569,61407,61245,61084,60923,60763,60604,60446,60288,60131,59975,59819,59664,59510,59357,
                   59204,59051,58900,58749,58598,58449,58300,58151,58004,57856,57710,57564,57419,57274,57130,56986,56843,56701,56559,56418,56277,56137,55998,55859,55720,
                   55582,55445,55308,55172,55037,54902,54767,54633,54499,54366,54234,54102,53971,53840,53709,53579,53450,53321,53193,53065,52937,52810,52684,52558,52432,
                   52307,52182,52058,51935,51812,51689,51567,
              },
              /* drc_high_LUT_point */
              {
                   112065,112065,102717,95539,89588,84466,79955,75920,72271,68942,65885,63060,60438,57996,55713,53572,51559,49662,47871,46177,44571,43047,41599,40221,38907,
                   37654,36457,35314,34219,33171,32167,31204,30280,29392,28539,27718,26929,26169,25437,24731,24051,23395,22761,22150,21559,20988,20437,19903,19387,18888,
                   18404,17936,17483,17043,16617,16205,15804,15416,15039,14673,14318,13974,13639,13314,12998,12691,12392,12102,11820,11546,11279,11019,10766,10520,10281,
                   10047,9820,9599,9383,9173,8969,8769,8575,8386,8201,8021,7845,7674,7507,7343,7184,7029,6877,6729,6585,6444,6306,6172,6040,5912,
                   5786,5664,5544,5427,5313,5201,5092,4985,4881,4779,4679,4582,4486,4393,4301,4212,4125,4039,3955,3873,3793,3715,3638,3563,3489,
                   3417,3347,3277,3210,3144,3079,3015,2953,2892,2832,2773,2716,2660,2605,2551,2498,2446,2395,2345,2296,2249,2202,2156,2110,2066,
                   2023,1980,1938,1897,1857,1818,1779,1741,1704,1667,1632,1597,1562,1528,1495,1462,1430,1399,1368,1338,1308,1279,1250,1222,1195,
                   1168,1141,1115,1089,1064,1039,1015, 991, 968, 945, 922, 900, 878, 857, 835, 815, 794, 774, 755, 736, 717, 698, 680, 662, 644,
                    627, 609, 593, 576, 560, 544, 528, 513, 498, 483, 468, 454, 440, 426, 412, 399, 385, 372, 359, 347, 334, 322, 310, 298, 287,
                    275, 264, 253, 242, 232, 221, 211, 201, 191, 181, 171, 162, 152, 143, 134, 125, 116, 107,  99,  90,  82,  74,  66,  58,  50,
                     43,  35,  28,  21,  14,   6,   0,
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
              16,
              /* k2 */
              5.6,
              /* k3 */
              0.65,
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
             150,
          },
          /* aec trigger 1 */
          {
             180,
             340,
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
              52,//gfilter_mid
              19,//gfilter_peak
              31,//gfilter_lrud
              75,//gfactor_1
              53,//gfactor_2
              37,//gfactor_3
              18,//gfactor_4
              500,//th1_y
              4000,//th2_y
              100000,//th1_ny
              1000000,//th2_ny
              1,//ny_correct_en
              128,//ny_weight_factor
              0,//ny_weight_factor_1minus
              70,//ahd_th
              12,//hvwt_th
              128,//flat_hvwt_factor
              0,//flat_hvwt_factor_1minus
              128,//detail_same_factor
              0,//detail_same_factor_1minus
              128,//detail_diff_factor
              0,//detail_diff_factor_1minus
              0,//ny_same_factor
              128,//ny_same_factor_1minus
              128,//ny_diff_factor
              0,//ny_diff_factor_1minus
              1,//dpp_correct
              1,//luma_detect_en
              50,//th1_very_light
              255,//th2_y_max
              1,//rgb_diff_detect_en
              10,//th1_rgb_diff
              100,//th2_rgb_diff
              1,//edge_detect_en
              5000,//th1_edge
              10000,//th2_edge
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
              10000,//th1_ny
              3000000,//th2_ny
              1,//ny_correct_en
              128,//ny_weight_factor
              0,//ny_weight_factor_1minus
              70,//ahd_th
              12,//hvwt_th
              102,//flat_hvwt_factor
              26,//flat_hvwt_factor_1minus
              128,//detail_same_factor
              0,//detail_same_factor_1minus
              128,//detail_diff_factor
              0,//detail_diff_factor_1minus
              0,//ny_same_factor
              128,//ny_same_factor_1minus
              128,//ny_diff_factor
              0,//ny_diff_factor_1minus
              1,//dpp_correct
              1,//luma_detect_en
              50,//th1_very_light
              255,//th2_y_max
              0,//rgb_diff_detect_en
              15,//th1_rgb_diff
              50,//th2_rgb_diff
              1,//edge_detect_en
              5000,//th1_edge
              10000,//th2_edge
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
              10000,//th1_ny
              900000,//th2_ny
              1,//ny_correct_en
              12,//ny_weight_factor
              116,//ny_weight_factor_1minus
              70,//ahd_th
              12,//hvwt_th
              102,//flat_hvwt_factor
              26,//flat_hvwt_factor_1minus
              128,//detail_same_factor
              0,//detail_same_factor_1minus
              128,//detail_diff_factor
              0,//detail_diff_factor_1minus
              0,//ny_same_factor
              128,//ny_same_factor_1minus
              0,//ny_diff_factor
              128,//ny_diff_factor_1minus
              1,//dpp_correct
              1,//luma_detect_en
              50,//th1_very_light
              255,//th2_y_max
              0,//rgb_diff_detect_en
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
              6,//ny_weight_factor
              122,//ny_weight_factor_1minus
              70,//ahd_th
              12,//hvwt_th
              102,//flat_hvwt_factor
              26,//flat_hvwt_factor_1minus
              128,//detail_same_factor
              0,//detail_same_factor_1minus
              128,//detail_diff_factor
              0,//detail_diff_factor_1minus
              0,//ny_same_factor
              128,//ny_same_factor_1minus
              0,//ny_diff_factor
              128,//ny_diff_factor_1minus
              1,//dpp_correct
              1,//luma_detect_en
              50,//th1_very_light
              254,//th2_y_max
              0,//rgb_diff_detect_en
              30,//th1_rgb_diff
              50,//th2_rgb_diff
              1,//edge_detect_en
              5000,//th1_edge
              10000,//th2_edge
              15,//dpp_strength
          },
       },
    },
    /*isp_sub_module_depurple_tuning_t isp_sub_module_depurple_tuning;*/
    {
    },
    /*isp_sub_module_ccm1_tuning_t isp_sub_module_ccm1_tuning;*/
    {
        1,//enable
        1,//interpolation enable
        1,//ae count
        3,//awb count
        1,//trigger_mode : 0 gain trigger 1 lux trigger
      /* aec trigger */
      {
        	/*trigger 0*/
        	{
        		0,
        		455,
        	},
        	/*trigger 1*/
        	{
        	},
        	/*trigger 2*/
        	{
        	},
        	/*trigger 3*/
        	{
        	},
        	/*trigger 4*/
        	{
        	},
      },
      /* awb trigger */
      {
        	/*trigger 0*/
        	{
        		2700,
        		2900,
        	},
        	/*trigger 1*/
        	{
        		3700,
        		4300,
        	},
        	/*trigger 2*/
        	{
        		4700,
        		5300,
        	},
        	/*trigger 3*/
        	{
        		6200,
        		6700,
        	},
        	/*trigger 4*/
        	{
        	},
        	/*trigger 5*/
        	{
        	},
        	/*trigger 6*/
        	{
        	},
      },
      /* isp_sub_module_ccm1_pra pra[5][7] */
       {
        	/*ae region 0*/
         {
                /* ccm a matrix */
                {
                     1.9855, -1.0667, 0.0812,
                     -0.5152, 1.5129, 0.0023,
                     -0.0087, -0.9356, 1.9442,
                },
                /* ccm tl84 matrix */
                {
                     2.0519, -1.1019,  0.0500,
                     -0.4379, 1.6590, -0.2211,
                     0.0435, -0.6991,  1.6555,
                },
                /* ccm d50 matrix */
                {
                     2.0501, -0.9613, -0.0887,
                     -0.3137, 1.5915, -0.2778,
                     -0.0082, -0.5219, 1.5300,
                },
                /* ccm d65 matrix */
                {
                     1.8611, -0.9086, -0.0475,
                     -0.2930, 1.4922, -0.1992,
                     0.0630, -0.6592, 1.5963,
                },
                /* ccm region 4 */
                {
                },
                /* ccm region 5 */
                {
                },
                /* ccm region 6 */
                {
                },
        	},
        	/*ae region 1*/
        	{
                /* ccm a matrix */
                {
                },
                /* ccm tl84 matrix */
                {
                },
                /* ccm d50 matrix */
                {
                },
                /* ccm d65 matrix */
                {
                },
                /* ccm region 4 */
                {
                },
                /* ccm region 5 */
                {
                },
                /* ccm region 6 */
                {
                },
        	},
        	/*ae region 2*/
        	{
                /* ccm a matrix */
                {
                },
                /* ccm tl84 matrix */
                {
                },
                /* ccm d50 matrix */
                {
                },
                /* ccm d65 matrix */
                {
                },
                /* ccm region 4 */
                {
                },
                /* ccm region 5 */
                {
                },
                /* ccm region 6 */
                {
                },
        	},
        	/*ae region 3*/
        	{
                /* ccm region 0 */
                {
                },
                /* ccm region 1 */
                {
                },
                /* ccm region 2 */
                {
                },
                /* ccm region 3 */
                {
                },
                /* ccm region 4 */
                {
                },
                /* ccm region 5 */
                {
                },
                /* ccm region 6 */
                {
                },
        	},
        	/*ae region 4*/
        	{
                /* ccm region 0 */
                {
                },
                /* ccm region 1 */
                {
                },
                /* ccm region 2 */
                {
                },
                /* ccm region 3 */
                {
                },
                /* ccm region 4 */
                {
                },
                /* ccm region 5 */
                {
                },
                /* ccm region 6 */
                {
                },
        	},
       },
    },
    /*isp_sub_module_ccm2_tuning_t isp_sub_module_ccm2_tuning;*/
    {
      0, //enable
    },
    /*isp_sub_module_gtm1_lut_tuning_t isp_sub_module_gtm1_lut_tuning;*/
    {
       /*int enable*/
     0,
     /*int interpolation_enable;*/
     1,
       /*int count_ae;*/
     2,
     /*int trigger_mode;*/
     1,
      /*aec_trigger_t aec_trigger[MIN_TRIGGER_COUNT];*/
       {
          /*aec trigger 1*/
          {
            1.000,
            100.000,
          },
          /*aec trigger 2*/
          {
            180,
            370,
          },
          /*aec trigger 3*/
          {
            435,
            550,
          },
       },
       /*isp_sub_module_gtm1_lut_pra pra[MIN_TRIGGER_COUNT];*/
       {
          /*gtm1 para 0*/
            {
                   1,
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
      /*gtm1 para 1*/
            {
                   1,
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
      /*gtm1 para 2*/
            {
                   1,
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
             400,
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
                   0,  14,  29,  43,  57,  71,  84,  97, 110, 123, 135, 147, 159, 170, 181, 192, 203, 213, 223, 233, 242, 251, 260, 269, 277, 285, 293, 300, 307, 314, 319, 326,
                 333, 340, 348, 355, 362, 369, 377, 384, 391, 398, 406, 413, 420, 427, 435, 442, 449, 456, 463, 471, 478, 485, 492, 499, 506, 514, 521, 528, 535, 542, 549, 557,
                 564, 571, 578, 585, 592, 599, 606, 614, 621, 628, 635, 642, 649, 656, 663, 670, 677, 684, 691, 698, 705, 712, 719, 726, 733, 740, 746, 752, 758, 764, 771, 777,
                 783, 789, 795, 801, 807, 813, 819, 824, 830, 836, 841, 847, 853, 858, 864, 869, 874, 880, 885, 890, 895, 900, 905, 910, 915, 920, 925, 930, 935, 940, 944, 949,
                 954, 958, 963, 967, 972, 976, 980, 985, 989, 993, 997,1001,1005,1009,1013,1017,1021,1025,1029,1032,1036,1040,1043,1047,1050,1054,1057,1060,1064,1067,1070,1073,
                1077,1081,1085,1089,1093,1097,1101,1105,1108,1112,1116,1120,1124,1128,1131,1135,1139,1143,1146,1150,1154,1157,1161,1164,1168,1172,1175,1179,1182,1186,1189,1192,
                1196,1199,1202,1206,1209,1212,1216,1219,1222,1225,1228,1232,1235,1238,1241,1244,1247,1250,1253,1256,1259,1262,1265,1267,1270,1273,1276,1279,1281,1283,1286,1288,
                1291,1293,1295,1298,1300,1303,1305,1308,1310,1313,1315,1317,1320,1322,1325,1327,1329,1332,1334,1336,1339,1341,1343,1346,1348,1350,1353,1355,1357,1360,1362,1364,
                1366,1369,1371,1373,1375,1377,1380,1382,1384,1386,1388,1391,1393,1395,1397,1399,1401,1403,1405,1408,1410,1412,1414,1416,1418,1420,1422,1424,1426,1428,1430,1432,
                1434,1436,1438,1440,1442,1444,1446,1448,1450,1452,1454,1456,1457,1458,1460,1462,1464,1465,1467,1469,1471,1472,1474,1476,1478,1479,1481,1483,1485,1486,1488,1490,
                1491,1493,1495,1497,1498,1500,1502,1503,1505,1507,1508,1510,1512,1513,1515,1517,1518,1520,1522,1523,1525,1527,1528,1530,1531,1533,1535,1536,1538,1540,1541,1543,
                1544,1546,1548,1549,1551,1552,1554,1555,1557,1559,1560,1562,1563,1565,1566,1568,1569,1571,1572,1574,1575,1577,1578,1580,1581,1583,1584,1586,1587,1589,1590,1592,
                1593,1595,1596,1598,1599,1601,1602,1604,1605,1607,1608,1609,1611,1612,1614,1615,1616,1617,1619,1620,1622,1623,1625,1626,1628,1629,1631,1632,1634,1636,1637,1639,
                1640,1642,1643,1645,1646,1648,1649,1651,1652,1654,1655,1657,1658,1659,1661,1662,1664,1665,1667,1668,1670,1671,1673,1674,1675,1677,1678,1680,1681,1683,1684,1685,
                1687,1688,1690,1691,1692,1694,1695,1697,1698,1699,1701,1702,1704,1705,1706,1708,1709,1710,1712,1713,1714,1716,1717,1718,1720,1721,1722,1724,1725,1726,1728,1729,
                1730,1732,1733,1734,1736,1737,1738,1739,1741,1742,1743,1745,1746,1747,1748,1750,1751,1752,1753,1755,1756,1757,1758,1760,1761,1762,1763,1765,1766,1767,1768,1769,
                1771,1772,1773,1774,1775,1777,1778,1779,1778,1779,1780,1781,1782,1784,1785,1786,1787,1788,1790,1791,1792,1793,1794,1796,1797,1798,1799,1801,1802,1803,1804,1805,
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
                2431,2431,2432,2433,2434,2435,2435,2436,2437,2438,2439,2440,2440,2441,2442,2443,2444,2445,2446,2447,2448,2449,2450,2451,2452,2452,2453,2454,2455,2456,2457,2458,
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
          /* aec trigger 1 */
          {
                   0,  14,  29,  43,  57,  71,  84,  97, 110, 123, 135, 147, 159, 170, 181, 192, 203, 213, 223, 233, 242, 251, 260, 269, 277, 285, 293, 300, 307, 314, 319, 326,
                 333, 340, 348, 355, 362, 369, 377, 384, 391, 398, 406, 413, 420, 427, 435, 442, 449, 456, 463, 471, 478, 485, 492, 499, 506, 514, 521, 528, 535, 542, 549, 557,
                 564, 571, 578, 585, 592, 599, 606, 614, 621, 628, 635, 642, 649, 656, 663, 670, 677, 684, 691, 698, 705, 712, 719, 726, 733, 740, 746, 752, 758, 764, 771, 777,
                 783, 789, 795, 801, 807, 813, 819, 824, 830, 836, 841, 847, 853, 858, 864, 869, 874, 880, 885, 890, 895, 900, 905, 910, 915, 920, 925, 930, 935, 940, 944, 949,
                 954, 958, 963, 967, 972, 976, 980, 985, 989, 993, 997,1001,1005,1009,1013,1017,1021,1025,1029,1032,1036,1040,1043,1047,1050,1054,1057,1060,1064,1067,1070,1073,
                1077,1081,1085,1089,1093,1097,1101,1105,1108,1112,1116,1120,1124,1128,1131,1135,1139,1143,1146,1150,1154,1157,1161,1164,1168,1172,1175,1179,1182,1186,1189,1192,
                1196,1199,1202,1206,1209,1212,1216,1219,1222,1225,1228,1232,1235,1238,1241,1244,1247,1250,1253,1256,1259,1262,1265,1267,1270,1273,1276,1279,1281,1283,1286,1288,
                1291,1293,1295,1298,1300,1303,1305,1308,1310,1313,1315,1317,1320,1322,1325,1327,1329,1332,1334,1336,1339,1341,1343,1346,1348,1350,1353,1355,1357,1360,1362,1364,
                1366,1369,1371,1373,1375,1377,1380,1382,1384,1386,1388,1391,1393,1395,1397,1399,1401,1403,1405,1408,1410,1412,1414,1416,1418,1420,1422,1424,1426,1428,1430,1432,
                1434,1436,1438,1440,1442,1444,1446,1448,1450,1452,1454,1456,1457,1458,1460,1462,1464,1465,1467,1469,1471,1472,1474,1476,1478,1479,1481,1483,1485,1486,1488,1490,
                1491,1493,1495,1497,1498,1500,1502,1503,1505,1507,1508,1510,1512,1513,1515,1517,1518,1520,1522,1523,1525,1527,1528,1530,1531,1533,1535,1536,1538,1540,1541,1543,
                1544,1546,1548,1549,1551,1552,1554,1555,1557,1559,1560,1562,1563,1565,1566,1568,1569,1571,1572,1574,1575,1577,1578,1580,1581,1583,1584,1586,1587,1589,1590,1592,
                1593,1595,1596,1598,1599,1601,1602,1604,1605,1607,1608,1609,1611,1612,1614,1615,1616,1617,1619,1620,1622,1623,1625,1626,1628,1629,1631,1632,1634,1636,1637,1639,
                1640,1642,1643,1645,1646,1648,1649,1651,1652,1654,1655,1657,1658,1659,1661,1662,1664,1665,1667,1668,1670,1671,1673,1674,1675,1677,1678,1680,1681,1683,1684,1685,
                1687,1688,1690,1691,1692,1694,1695,1697,1698,1699,1701,1702,1704,1705,1706,1708,1709,1710,1712,1713,1714,1716,1717,1718,1720,1721,1722,1724,1725,1726,1728,1729,
                1730,1732,1733,1734,1736,1737,1738,1739,1741,1742,1743,1745,1746,1747,1748,1750,1751,1752,1753,1755,1756,1757,1758,1760,1761,1762,1763,1765,1766,1767,1768,1769,
                1771,1772,1773,1774,1775,1777,1778,1779,1778,1779,1780,1781,1782,1784,1785,1786,1787,1788,1790,1791,1792,1793,1794,1796,1797,1798,1799,1801,1802,1803,1804,1805,
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
                2431,2431,2432,2433,2434,2435,2435,2436,2437,2438,2439,2440,2440,2441,2442,2443,2444,2445,2446,2447,2448,2449,2450,2451,2452,2452,2453,2454,2455,2456,2457,2458,
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
             200,
          },
          /* aec trigger 1 */
          {
             210,
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
             0.021,//cut_ration
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
          },
          /* para 1 */
          {
             1,//enable_cdf_smooth
             0.017,//cut_ration
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
          },
          /* para 2 */
          {
             1,//enable_cdf_smooth
             0.006,//cut_ration
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
          },
       },
       1,//enable_tuning_mesh
       32,//mesh_w
       32,//mesh_h
    },
    /* isp_sub_module_3d_lut_tuning_t isp_sub_module_3d_lut_tuning */
   {
       /*int enable*/
       0,
   },
    /*isp_sub_module_rgbyuv_tuning_t isp_sub_module_rgbyuv_tuning;*/
    {
      1,
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
             170,
          },
          /* aec trigger 1 */
          {
             200,
             340,
          },
          /* aec trigger 2 */
          {
             400,
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
             5000,
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
             160,
          },
          /* aec trigger 1 */
          {
             190,
             230,
          },
          /* aec trigger 2 */
          {
             240,
             250,
          },
          /* aec trigger 3 */
          {
             260,
             280,
          },
          /* aec trigger 4 */
          {
             324,
             325,
          },
          /* aec trigger 5 */
          {
             347,
             348,
          },
          /* aec trigger 6 */
          {
             371,
             372,
          },
          /* aec trigger 7 */
          {
             394,
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
                -1,-4,-11,-15,//bpf(1,1:4)
                -4,-23,-64,-95,//bpf(2,1:4)
                -11,-64,-169,-77,//bpf(3,1:4)
                -15,-95,-77,2152,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             50,//strength_pos_edge
             50,//strength_neg_edge
             400,//overshoot_pos_edge
             -400,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             200,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
                -1,-4,-11,-15,//bpf(1,1:4)
                -4,-23,-64,-95,//bpf(2,1:4)
                -11,-64,-169,-77,//bpf(3,1:4)
                -15,-95,-77,2152,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             50,//strength_pos_edge
             50,//strength_neg_edge
             400,//overshoot_pos_edge
             -400,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             200,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
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
                -1,-4,-11,-15,//bpf(1,1:4)
                -4,-23,-64,-95,//bpf(2,1:4)
                -11,-64,-169,-77,//bpf(3,1:4)
                -15,-95,-77,2152,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             48,//strength_pos_edge
             48,//strength_neg_edge
             400,//overshoot_pos_edge
             -400,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             200,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
                -1,-4,-11,-15,//bpf(1,1:4)
                -4,-23,-64,-95,//bpf(2,1:4)
                -11,-64,-169,-77,//bpf(3,1:4)
                -15,-95,-77,2152,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             46,//strength_pos_edge
             46,//strength_neg_edge
             350,//overshoot_pos_edge
             -350,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             200,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
                -1,-4,-11,-15,//bpf(1,1:4)
                -4,-23,-64,-95,//bpf(2,1:4)
                -11,-64,-169,-77,//bpf(3,1:4)
                -15,-95,-77,2152,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             42,//strength_pos_edge
             42,//strength_neg_edge
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
             {0,0,0,0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             200,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
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
                -1,-4,-11,-15,//bpf(1,1:4)
                -4,-23,-64,-95,//bpf(2,1:4)
                -11,-64,-169,-77,//bpf(3,1:4)
                -15,-95,-77,2152,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             40,//strength_pos_edge
             40,//strength_neg_edge
             230,//overshoot_pos_edge
             -230,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             200,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
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
                -1,-5,-13,-17,//bpf(1,1:4)
                -5,-28,-73,-105,//bpf(2,1:4)
                -13,-73,-166,13,//bpf(3,1:4)
                -17,-105,13,1944,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             35,//strength_pos_edge
             35,//strength_neg_edge
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
             {0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             200,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
          /* para 7 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             1,//shrink enable
             1,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                -1,-4,-12,-20,//bpf(1,1:4)
                -4,-32,-87,-124,//bpf(2,1:4)
                -12,-87,-193,182,//bpf(3,1:4)
                -20,-124,182,1576,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             30,//strength_pos_edge
             30,//strength_neg_edge
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
             {0,0,0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             200,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
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
                -1,-4,-12,-20,//bpf(1,1:4)
                -4,-32,-87,-124,//bpf(2,1:4)
                -12,-87,-193,182,//bpf(3,1:4)
                -20,-124,182,1576,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             25,//strength_pos_edge
             25,//strength_neg_edge
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
             220,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
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
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
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
          /* para 1 */
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
             180,
          },
          /* aec trigger 1 */
          {
             240,
             250,
          },
          /* aec trigger 2 */
          {
             260,
             280,
          },
          /* aec trigger 3 */
          {
             324,
             325,
          },
          /* aec trigger 4 */
          {
             349,
             350,
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
       /* isp_sub_module_3d_denoise_pra pra[MAX_TRIGGER_COUNT] */
       {
          /* para 0 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             10,//nr3d_npy_th1
             80,//nr3d_npy_th2
             10,//nr3d_mpy_th1
             15,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             15,//nr3d_mpc_th1
             30,//nr3d_mpc_th2
             2,//nr3d_lamda2d
             16,//nr3d_ite
             0,//nr3d_msr
             252,//nr3d_decay
             40,//nr3d_satu
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
             12,//dbk_h_mv_diff
             12,//dbk_h_mv_str
             12,//dbk_v_mv_diff
             12,//dbk_v_mv_str
             200,//dbk_h_yy_flat
             12,//dbk_h_yy_diff
             100,//dbk_v_yy_flat
             12,//dbk_v_yy_diff
             30,//dbk_satu
             0,//dbk_gaus_y_c11
             0,//dbk_gaus_y_c12
             1,//dbk_gaus_y_c13
             0,//dbk_gaus_y_c21
             252,//dbk_gaus_y_c23
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
                0,2,4,6,8,10,12,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             32,//dbk_h_ed_thrd
             32,//dbk_v_ed_thrd
          },
          /* para 1 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             10,//nr3d_npy_th1
             80,//nr3d_npy_th2
             10,//nr3d_mpy_th1
             25,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             20,//nr3d_mpc_th1
             45,//nr3d_mpc_th2
             2,//nr3d_lamda2d
             18,//nr3d_ite
             0,//nr3d_msr
             252,//nr3d_decay
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
             0,//dbk_gaus_y_c12
             1,//dbk_gaus_y_c13
             0,//dbk_gaus_y_c21
             252,//dbk_gaus_y_c23
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
             45,//nr3d_mpc_th2
             5,//nr3d_lamda2d
             20,//nr3d_ite
             0,//nr3d_msr
             252,//nr3d_decay
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
             15,//nr3d_mpy_th1
             30,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             20,//nr3d_mpc_th1
             45,//nr3d_mpc_th2
             5,//nr3d_lamda2d
             20,//nr3d_ite
             0,//nr3d_msr
             252,//nr3d_decay
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
             20,//nr3d_mpy_th1
             40,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             20,//nr3d_mpc_th1
             45,//nr3d_mpc_th2
             8,//nr3d_lamda2d
             22,//nr3d_ite
             0,//nr3d_msr
             250,//nr3d_decay
             10,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             10,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             216,//nr3d_gaus_y_c23
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
             0,//dbk_gaus_y_c11
             0,//dbk_gaus_y_c12
             10,//dbk_gaus_y_c13
             0,//dbk_gaus_y_c21
             216,//dbk_gaus_y_c23
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
             25,//nr3d_mpy_th1
             50,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             25,//nr3d_mpc_th1
             50,//nr3d_mpc_th2
             10,//nr3d_lamda2d
             25,//nr3d_ite
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
             40,//nr3d_mpy_th1
             80,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             40,//nr3d_mpc_th1
             80,//nr3d_mpc_th2
             12,//nr3d_lamda2d
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
             55,//nr3d_mpy_th1
             110,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             55,//nr3d_mpc_th1
             110,//nr3d_mpc_th2
             15,//nr3d_lamda2d
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
             30,//dbk_h_mv_diff
             50,//dbk_h_mv_str
             30,//dbk_v_mv_diff
             50,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             50,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             50,//dbk_v_yy_diff
             20,//dbk_satu
             0,//dbk_gaus_y_c11
             9,//dbk_gaus_y_c12
             30,//dbk_gaus_y_c13
             1,//dbk_gaus_y_c21
             98,//dbk_gaus_y_c23
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
             60,//nr3d_mpy_th1
             120,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             60,//nr3d_mpc_th1
             120,//nr3d_mpc_th2
             20,//nr3d_lamda2d
             35,//nr3d_ite
             0,//nr3d_msr
             250,//nr3d_decay
             10,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             4,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             240,//nr3d_gaus_y_c23
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
             60,//nr3d_mpy_th1
             120,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             60,//nr3d_mpc_th1
             120,//nr3d_mpc_th2
             25,//nr3d_lamda2d
             35,//nr3d_ite
             0,//nr3d_msr
             250,//nr3d_decay
             10,//nr3d_satu
             0,//nr3d_gaus_y_c11
             0,//nr3d_gaus_y_c12
             10,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             216,//nr3d_gaus_y_c23
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
             5,//dbk_gaus_y_c11
             18,//dbk_gaus_y_c12
             27,//dbk_gaus_y_c13
             8,//dbk_gaus_y_c21
             40,//dbk_gaus_y_c23
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
             120,//nr3d_mpc_th2
             30,//nr3d_lamda2d
             40,//nr3d_ite
             0,//nr3d_msr
             250,//nr3d_decay
             20,//nr3d_satu
             0,//nr3d_gaus_y_c11
             1,//nr3d_gaus_y_c12
             16,//nr3d_gaus_y_c13
             0,//nr3d_gaus_y_c21
             188,//nr3d_gaus_y_c23
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
             11,//dbk_gaus_y_c11
             18,//dbk_gaus_y_c12
             22,//dbk_gaus_y_c13
             13,//dbk_gaus_y_c21
             26,//dbk_gaus_y_c23
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
	//isp_sub_module_dithering_tuning_t isp_sub_module_dithering_tuning;
    {
    },
    //isp_sub_module_isp_vfe_tuning
    {
       /* enable */
       1,
       /* interpolation_enable */
       1,
       /* count_ae */
       2,
       /* tigger_mode : 0 gain ,1 lux */
       1,
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             100,
          },
          /* aec trigger 1 */
          {
             120,
             600,
          },
          /* aec trigger 2 */
          {
             200,
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
                 //hdr
                 {
                        240,//blc_short_exp
                        240,//blc_mid_exp
                        240,//blc_long_exp
                        0,//hdr_motion_en_sm
                        25,//hdr_alpha_s_th_sm
                        867,//hdr_exp_value_th1_sm
                        682,//hdr_th2_th1_reverse_sm
                        64,//hdr_noise_sp_sm
                        76,//hdr_motion_mp_sm
                        1,//hdr_motion_en_sl
                        0,//hdr_alpha_s_th_sl
                        36,//hdr_exp_value_th1_sl
                        400,//hdr_th2_th1_reverse_sl
                        0,//hdr_noise_sp_sl
                        5,//hdr_motion_mp_sl
                        32,//hdr_md_th4_sl
                        27,//hdr_md_th9_sl
                        19,//hdr_md_th16_sl
                        15,//hdr_md_th25_sl
                        /* noise_profile_l[129] */
                        {
                             0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                             1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                             2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                        },
                        /* noise_profile_m[129] */
                        {
                             0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
                             3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
                             4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
                        },
                        /* noise_profile_s[129] */
                        {
                            13, 16, 18, 19, 21, 23, 24, 25, 27, 28, 29, 30, 31, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55,
                            56, 56, 57, 58, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 77,
                            78, 78, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95,
                        },
                        0,//hdr_policy
                        1,//enable_hdr_blc
                        50,//mv_fix_th
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
                 //ai isp
                 {

                 }
           },
           /* para 1*/
           {
               //hdr
              {
                     240,//blc_short_exp
                     240,//blc_mid_exp
                     240,//blc_long_exp
                     0,//hdr_motion_en_sm
                     25,//hdr_alpha_s_th_sm
                     867,//hdr_exp_value_th1_sm
                     682,//hdr_th2_th1_reverse_sm
                     64,//hdr_noise_sp_sm
                     76,//hdr_motion_mp_sm
                     1,//hdr_motion_en_sl
                     0,//hdr_alpha_s_th_sl
                     64,//hdr_exp_value_th1_sl
                     512,//hdr_th2_th1_reverse_sl
                     0,//hdr_noise_sp_sl
                     10,//hdr_motion_mp_sl
                     32,//hdr_md_th4_sl
                     27,//hdr_md_th9_sl
                     19,//hdr_md_th16_sl
                     15,//hdr_md_th25_sl
                     /* noise_profile_l[129] */
                     {
                          0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                          2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                     },
                     /* noise_profile_m[129] */
                     {
                          0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
                          3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
                          4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
                     },
                     /* noise_profile_s[129] */
                     {
                         13, 16, 18, 19, 21, 23, 24, 25, 27, 28, 29, 30, 31, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55,
                         56, 56, 57, 58, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 77,
                         78, 78, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95,
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
              //ai isp
              {

              },
           }
       },
    },

    //isp_sub_module_scaler_tuning_t isp_sub_module_scaler_tuning;
    {

    },
    //isp_sub_module_eis_ldc_tuning_t isp_sub_module_eis_ldc_tuning;
    {

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
             140,
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
                2,//saturation
                0,//hue
                20,//y_lo_th1
                100,//y_lo_th2
                1022,//y_hi_th1
                1023,//y_hi_th2
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                2,//saturation
                0,//hue
                20,//y_lo_th1
                100,//y_lo_th2
                950,//y_hi_th1
                1023,//y_hi_th2
             },
          },
          /* aec trigger 2 */
          {
             /* awb para 0 */
             {
                1.6,//saturation
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
                1.4,//saturation
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
                1.3,//saturation
                0,//hue
                20,//y_lo_th1
                100,//y_lo_th2
                900,//y_hi_th1
                1023,//y_hi_th2
             },
          },
       },
    },
    /* isp_sub_module_lsc_tuning_t isp_sub_module_lsc_tuning*/
    {
        1,//enable
        1,//interpolation enable
		2,//count ae
        4,//awb count
		1,//trigger_mode : 0 gain trigger 1 lux trigger
		{
     		/*aec trigger 1*/
     		{
     			0,
     			300,
     		},
			/*aec trigger 2*/
     		{
     			390,
     			500,
     		},
		},
        {
     		/*awb trigger 1*/
     		{
     			2700,
     			2900,
     		},
     		/*awb trigger 2*/
     		{
     			3700,
     			4300,
     		},
     		/*awb trigger 3*/
     		{
     			4700,
     			5300,
     		},
     		/*awb trigger 4*/
     		{
     			6200,
     			6800,
     		},
     		/*awb trigger 5*/
     		{
     			0,
     			0,
     		},
     		/*awb trigger 6*/
     		{
     			0,
     			0,
     		},
        },
        /* isp_sub_module_lsc_pra */
        {
		  /* aec trigger 0 */
          {
     		 /* para 1 */
     		 {
               	0.300000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 2 */
     		 {
               	0.300000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 3 */
     		 {
               	0.300000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 4 */
     		 {
               	0.300000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 5 */
     		 {
     		 },
     		 /* para 6 */
     		 {
     		 },
          },
		  /* aec trigger 1 */
          {
     		 /* para 1 */
     		 {
               	0.0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 2 */
     		 {
               	0.0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 3 */
     		 {
               	0.0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 4 */
     		 {
               	0.0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
               	1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
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
     		 /* para 5 */
     		 {
     		 },
     		 /* para 6 */
     		 {
     		 },
          },
        },
    },
},
/*stats_config_t stats_config*/
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
      477,
      /* ev0_count */
      477,
      /* ev2_count */
      477,
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
      //1,1940
      //1000,1940
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
         {    256,   1496   },         /* Gain = 1.000000 Exposure Index = 236 */
         {    256,   1541   },         /* Gain = 1.000000 Exposure Index = 237 */
         {    256,   1588   },         /* Gain = 1.000000 Exposure Index = 238 */
         {    256,   1636   },         /* Gain = 1.000000 Exposure Index = 239 */
         {    256,   1686   },         /* Gain = 1.000000 Exposure Index = 240 */
         {    256,   1737   },         /* Gain = 1.000000 Exposure Index = 241 */
         {    256,   1790   },         /* Gain = 1.000000 Exposure Index = 242 */
         {    256,   1844   },         /* Gain = 1.000000 Exposure Index = 243 */
         {    256,   1900   },         /* Gain = 1.000000 Exposure Index = 244 */
         {    259,   1940   },         /* Gain = 1.011719 Exposure Index = 245 */
         {    267,   1940   },         /* Gain = 1.042969 Exposure Index = 246 */
         {    276,   1940   },         /* Gain = 1.078125 Exposure Index = 247 */
         {    285,   1940   },         /* Gain = 1.113281 Exposure Index = 248 */
         {    294,   1940   },         /* Gain = 1.148438 Exposure Index = 249 */
         {    303,   1940   },         /* Gain = 1.183594 Exposure Index = 250 */
         {    313,   1940   },         /* Gain = 1.222656 Exposure Index = 251 */
         {    323,   1940   },         /* Gain = 1.261719 Exposure Index = 252 */
         {    333,   1940   },         /* Gain = 1.300781 Exposure Index = 253 */
         {    343,   1940   },         /* Gain = 1.339844 Exposure Index = 254 */
         {    354,   1940   },         /* Gain = 1.382812 Exposure Index = 255 */
         {    365,   1940   },         /* Gain = 1.425781 Exposure Index = 256 */
         {    376,   1940   },         /* Gain = 1.468750 Exposure Index = 257 */
         {    388,   1940   },         /* Gain = 1.515625 Exposure Index = 258 */
         {    400,   1940   },         /* Gain = 1.562500 Exposure Index = 259 */
         {    412,   1940   },         /* Gain = 1.609375 Exposure Index = 260 */
         {    425,   1940   },         /* Gain = 1.660156 Exposure Index = 261 */
         {    438,   1940   },         /* Gain = 1.710938 Exposure Index = 262 */
         {    452,   1940   },         /* Gain = 1.765625 Exposure Index = 263 */
         {    466,   1940   },         /* Gain = 1.820312 Exposure Index = 264 */
         {    480,   1940   },         /* Gain = 1.875000 Exposure Index = 265 */
         {    495,   1940   },         /* Gain = 1.933594 Exposure Index = 266 */
         {    510,   1940   },         /* Gain = 1.992188 Exposure Index = 267 */
         {    526,   1940   },         /* Gain = 2.054688 Exposure Index = 268 */
         {    542,   1940   },         /* Gain = 2.117188 Exposure Index = 269 */
         {    559,   1940   },         /* Gain = 2.183594 Exposure Index = 270 */
         {    576,   1940   },         /* Gain = 2.250000 Exposure Index = 271 */
         {    594,   1940   },         /* Gain = 2.320312 Exposure Index = 272 */
         {    612,   1940   },         /* Gain = 2.390625 Exposure Index = 273 */
         {    631,   1940   },         /* Gain = 2.464844 Exposure Index = 274 */
         {    650,   1940   },         /* Gain = 2.539062 Exposure Index = 275 */
         {    670,   1940   },         /* Gain = 2.617188 Exposure Index = 276 */
         {    691,   1940   },         /* Gain = 2.699219 Exposure Index = 277 */
         {    712,   1940   },         /* Gain = 2.781250 Exposure Index = 278 */
         {    734,   1940   },         /* Gain = 2.867188 Exposure Index = 279 */
         {    757,   1940   },         /* Gain = 2.957031 Exposure Index = 280 */
         {    780,   1940   },         /* Gain = 3.046875 Exposure Index = 281 */
         {    804,   1940   },         /* Gain = 3.140625 Exposure Index = 282 */
         {    829,   1940   },         /* Gain = 3.238281 Exposure Index = 283 */
         {    854,   1940   },         /* Gain = 3.335938 Exposure Index = 284 */
         {    880,   1940   },         /* Gain = 3.437500 Exposure Index = 285 */
         {    907,   1940   },         /* Gain = 3.542969 Exposure Index = 286 */
         {    935,   1940   },         /* Gain = 3.652344 Exposure Index = 287 */
         {    964,   1940   },         /* Gain = 3.765625 Exposure Index = 288 */
         {    993,   1940   },         /* Gain = 3.878906 Exposure Index = 289 */
         {   1023,   1940   },         /* Gain = 3.996094 Exposure Index = 290 */
         {   1054,   1940   },         /* Gain = 4.117188 Exposure Index = 291 */
         {   1086,   1940   },         /* Gain = 4.242188 Exposure Index = 292 */
         {   1119,   1940   },         /* Gain = 4.371094 Exposure Index = 293 */
         {   1153,   1940   },         /* Gain = 4.503906 Exposure Index = 294 */
         {   1188,   1940   },         /* Gain = 4.640625 Exposure Index = 295 */
         {   1224,   1940   },         /* Gain = 4.781250 Exposure Index = 296 */
         {   1261,   1940   },         /* Gain = 4.925781 Exposure Index = 297 */
         {   1299,   1940   },         /* Gain = 5.074219 Exposure Index = 298 */
         {   1338,   1940   },         /* Gain = 5.226562 Exposure Index = 299 */
         {   1379,   1940   },         /* Gain = 5.386719 Exposure Index = 300 */
         {   1421,   1940   },         /* Gain = 5.550781 Exposure Index = 301 */
         {   1464,   1940   },         /* Gain = 5.718750 Exposure Index = 302 */
         {   1508,   1940   },         /* Gain = 5.890625 Exposure Index = 303 */
         {   1554,   1940   },         /* Gain = 6.070312 Exposure Index = 304 */
         {   1601,   1940   },         /* Gain = 6.253906 Exposure Index = 305 */
         {   1650,   1940   },         /* Gain = 6.445312 Exposure Index = 306 */
         {   1700,   1940   },         /* Gain = 6.640625 Exposure Index = 307 */
         {   1751,   1940   },         /* Gain = 6.839844 Exposure Index = 308 */
         {   1804,   1940   },         /* Gain = 7.046875 Exposure Index = 309 */
         {   1859,   1940   },         /* Gain = 7.261719 Exposure Index = 310 */
         {   1915,   1940   },         /* Gain = 7.480469 Exposure Index = 311 */
         {   1973,   1940   },         /* Gain = 7.707031 Exposure Index = 312 */
         {   2033,   1940   },         /* Gain = 7.941406 Exposure Index = 313 */
         {   2094,   1940   },         /* Gain = 8.179688 Exposure Index = 314 */
         {   2157,   1940   },         /* Gain = 8.425781 Exposure Index = 315 */
         {   2222,   1940   },         /* Gain = 8.679688 Exposure Index = 316 */
         {   2289,   1940   },         /* Gain = 8.941406 Exposure Index = 317 */
         {   2358,   1940   },         /* Gain = 9.210938 Exposure Index = 318 */
         {   2429,   1940   },         /* Gain = 9.488281 Exposure Index = 319 */
         {   2502,   1940   },         /* Gain = 9.773438 Exposure Index = 320 */
         {   2578,   1940   },         /* Gain = 10.070312 Exposure Index = 321 */
         {   2656,   1940   },         /* Gain = 10.375000 Exposure Index = 322 */
         {   2736,   1940   },         /* Gain = 10.687500 Exposure Index = 323 */
         {   2819,   1940   },         /* Gain = 11.011719 Exposure Index = 324 */
         {   2904,   1940   },         /* Gain = 11.343750 Exposure Index = 325 */
         {   2992,   1940   },         /* Gain = 11.687500 Exposure Index = 326 */
         {   3082,   1940   },         /* Gain = 12.039062 Exposure Index = 327 */
         {   3175,   1940   },         /* Gain = 12.402344 Exposure Index = 328 */
         {   3271,   1940   },         /* Gain = 12.777344 Exposure Index = 329 */
         {   3370,   1940   },         /* Gain = 13.164062 Exposure Index = 330 */
         {   3472,   1940   },         /* Gain = 13.562500 Exposure Index = 331 */
         {   3577,   1940   },         /* Gain = 13.972656 Exposure Index = 332 */
         {   3685,   1940   },         /* Gain = 14.394531 Exposure Index = 333 */
         {   3796,   1940   },         /* Gain = 14.828125 Exposure Index = 334 */
         {   3910,   1940   },         /* Gain = 15.273438 Exposure Index = 335 */
         {   4028,   1940   },         /* Gain = 15.734375 Exposure Index = 336 */
         {   4149,   1940   },         /* Gain = 16.207031 Exposure Index = 337 */
         {   4274,   1940   },         /* Gain = 16.695312 Exposure Index = 338 */
         {   4403,   1940   },         /* Gain = 17.199219 Exposure Index = 339 */
         {   4536,   1940   },         /* Gain = 17.718750 Exposure Index = 340 */
         {   4673,   1940   },         /* Gain = 18.253906 Exposure Index = 341 */
         {   4814,   1940   },         /* Gain = 18.804688 Exposure Index = 342 */
         {   4959,   1940   },         /* Gain = 19.371094 Exposure Index = 343 */
         {   5108,   1940   },         /* Gain = 19.953125 Exposure Index = 344 */
         {   5262,   1940   },         /* Gain = 20.554688 Exposure Index = 345 */
         {   5420,   1940   },         /* Gain = 21.171875 Exposure Index = 346 */
         {   5583,   1940   },         /* Gain = 21.808594 Exposure Index = 347 */
         {   5751,   1940   },         /* Gain = 22.464844 Exposure Index = 348 */
         {   5924,   1940   },         /* Gain = 23.140625 Exposure Index = 349 */
         {   6102,   1940   },         /* Gain = 23.835938 Exposure Index = 350 */
         {   6286,   1940   },         /* Gain = 24.554688 Exposure Index = 351 */
         {   6475,   1940   },         /* Gain = 25.292969 Exposure Index = 352 */
         {   6670,   1940   },         /* Gain = 26.054688 Exposure Index = 353 */
         {   6871,   1940   },         /* Gain = 26.839844 Exposure Index = 354 */
         {   7078,   1940   },         /* Gain = 27.648438 Exposure Index = 355 */
         {   7291,   1940   },         /* Gain = 28.480469 Exposure Index = 356 */
         {   7510,   1940   },         /* Gain = 29.335938 Exposure Index = 357 */
         {   7736,   1940   },         /* Gain = 30.218750 Exposure Index = 358 */
         {   7969,   1940   },         /* Gain = 31.128906 Exposure Index = 359 */
         {   8209,   1940   },         /* Gain = 32.066406 Exposure Index = 360 */
         {   8456,   1940   },         /* Gain = 33.031250 Exposure Index = 361 */
         {   8710,   1940   },         /* Gain = 34.023438 Exposure Index = 362 */
         {   8972,   1940   },         /* Gain = 35.046875 Exposure Index = 363 */
         {   9242,   1940   },         /* Gain = 36.101562 Exposure Index = 364 */
         {   9520,   1940   },         /* Gain = 37.187500 Exposure Index = 365 */
         {   9806,   1940   },         /* Gain = 38.304688 Exposure Index = 366 */
         {  10101,   1940   },         /* Gain = 39.457031 Exposure Index = 367 */
         {  10405,   1940   },         /* Gain = 40.644531 Exposure Index = 368 */
         {  10718,   1940   },         /* Gain = 41.867188 Exposure Index = 369 */
         {  11040,   1940   },         /* Gain = 43.125000 Exposure Index = 370 */
         {  11372,   1940   },         /* Gain = 44.421875 Exposure Index = 371 */
         {  11714,   1940   },         /* Gain = 45.757812 Exposure Index = 372 */
         {  12066,   1940   },         /* Gain = 47.132812 Exposure Index = 373 */
         {  12428,   1940   },         /* Gain = 48.546875 Exposure Index = 374 */
         {  12801,   1940   },         /* Gain = 50.003906 Exposure Index = 375 */
         {  13186,   1940   },         /* Gain = 51.507812 Exposure Index = 376 */
         {  13582,   1940   },         /* Gain = 53.054688 Exposure Index = 377 */
         {  13990,   1940   },         /* Gain = 54.648438 Exposure Index = 378 */
         {  14410,   1940   },         /* Gain = 56.289062 Exposure Index = 379 */
         {  14843,   1940   },         /* Gain = 57.980469 Exposure Index = 380 */
         {  15289,   1940   },         /* Gain = 59.722656 Exposure Index = 381 */
         {  15748,   1940   },         /* Gain = 61.515625 Exposure Index = 382 */
         {  16221,   1940   },         /* Gain = 63.363281 Exposure Index = 383 */
         {  16708,   1940   },         /* Gain = 65.265625 Exposure Index = 384 */
         {  17210,   1940   },         /* Gain = 67.226562 Exposure Index = 385 */
         {  17727,   1940   },         /* Gain = 69.246094 Exposure Index = 386 */
         {  18259,   1940   },         /* Gain = 71.324219 Exposure Index = 387 */
         {  18807,   1940   },         /* Gain = 73.464844 Exposure Index = 388 */
         {  19372,   1940   },         /* Gain = 75.671875 Exposure Index = 389 */
         {  19954,   1940   },         /* Gain = 77.945312 Exposure Index = 390 */
         {  20553,   1940   },         /* Gain = 80.285156 Exposure Index = 391 */
         {  21170,   1940   },         /* Gain = 82.695312 Exposure Index = 392 */
         {  21806,   1940   },         /* Gain = 85.179688 Exposure Index = 393 */
         {  22461,   1940   },         /* Gain = 87.738281 Exposure Index = 394 */
         {  23135,   1940   },         /* Gain = 90.371094 Exposure Index = 395 */
         {  23830,   1940   },         /* Gain = 93.085938 Exposure Index = 396 */
         {  24545,   1940   },         /* Gain = 95.878906 Exposure Index = 397 */
         {  25282,   1940   },         /* Gain = 98.757812 Exposure Index = 398 */
         {  26041,   1940   },         /* Gain = 101.722656 Exposure Index = 399 */
         {  26823,   1940   },         /* Gain = 104.777344 Exposure Index = 400 */
         {  27628,   1940   },         /* Gain = 107.921875 Exposure Index = 401 */
         {  28457,   1940   },         /* Gain = 111.160156 Exposure Index = 402 */
         {  29311,   1940   },         /* Gain = 114.496094 Exposure Index = 403 */
         {  30191,   1940   },         /* Gain = 117.933594 Exposure Index = 404 */
         {  31097,   1940   },         /* Gain = 121.472656 Exposure Index = 405 */
         {  32030,   1940   },         /* Gain = 125.117188 Exposure Index = 406 */
         {  32991,   1940   },         /* Gain = 128.871094 Exposure Index = 407 */
         {  33981,   1940   },         /* Gain = 132.738281 Exposure Index = 408 */
         {  35001,   1940   },         /* Gain = 136.722656 Exposure Index = 409 */
         {  36052,   1940   },         /* Gain = 140.828125 Exposure Index = 410 */
         {  37134,   1940   },         /* Gain = 145.054688 Exposure Index = 411 */
         {  38249,   1940   },         /* Gain = 149.410156 Exposure Index = 412 */
         {  39397,   1940   },         /* Gain = 153.894531 Exposure Index = 413 */
         {  40579,   1940   },         /* Gain = 158.511719 Exposure Index = 414 */
         {  41797,   1940   },         /* Gain = 163.269531 Exposure Index = 415 */
         {  43051,   1940   },         /* Gain = 168.167969 Exposure Index = 416 */
         {  44343,   1940   },         /* Gain = 173.214844 Exposure Index = 417 */
         {  45674,   1940   },         /* Gain = 178.414062 Exposure Index = 418 */
         {  47045,   1940   },         /* Gain = 183.769531 Exposure Index = 419 */
         {  48457,   1940   },         /* Gain = 189.285156 Exposure Index = 420 */
         {  49911,   1940   },         /* Gain = 194.964844 Exposure Index = 421 */
         {  51409,   1940   },         /* Gain = 200.816406 Exposure Index = 422 */
         {  52952,   1940   },         /* Gain = 206.843750 Exposure Index = 423 */
         {  54541,   1940   },         /* Gain = 213.050781 Exposure Index = 424 */
         {  56178,   1940   },         /* Gain = 219.445312 Exposure Index = 425 */
         {  57864,   1940   },         /* Gain = 226.031250 Exposure Index = 426 */
         {  59600,   1940   },         /* Gain = 232.812500 Exposure Index = 427 */
         {  61388,   1940   },         /* Gain = 239.796875 Exposure Index = 428 */
         {  63230,   1940   },         /* Gain = 246.992188 Exposure Index = 429 */
         {  65127,   1940   },         /* Gain = 254.402344 Exposure Index = 430 */
         {  67081,   1940   },         /* Gain = 262.035156 Exposure Index = 431 */
         {  69094,   1940   },         /* Gain = 269.898438 Exposure Index = 432 */
         {  71167,   1940   },         /* Gain = 277.996094 Exposure Index = 433 */
         {  73303,   1940   },         /* Gain = 286.339844 Exposure Index = 434 */
         {  75503,   1940   },         /* Gain = 294.933594 Exposure Index = 435 */
         {  77769,   1940   },         /* Gain = 303.785156 Exposure Index = 436 */
         {  80103,   1940   },         /* Gain = 312.902344 Exposure Index = 437 */
         {  82507,   1940   },         /* Gain = 322.292969 Exposure Index = 438 */
         {  84983,   1940   },         /* Gain = 331.964844 Exposure Index = 439 */
         {  87533,   1940   },         /* Gain = 341.925781 Exposure Index = 440 */
         {  90159,   1940   },         /* Gain = 352.183594 Exposure Index = 441 */
         {  92864,   1940   },         /* Gain = 362.750000 Exposure Index = 442 */
         {  95650,   1940   },         /* Gain = 373.632812 Exposure Index = 443 */
         {  98520,   1940   },         /* Gain = 384.843750 Exposure Index = 444 */
         { 101476,   1940   },         /* Gain = 396.390625 Exposure Index = 445 */
         { 104521,   1940   },         /* Gain = 408.285156 Exposure Index = 446 */
         { 107657,   1940   },         /* Gain = 420.535156 Exposure Index = 447 */
         { 110887,   1940   },         /* Gain = 433.152344 Exposure Index = 448 */
         { 114214,   1940   },         /* Gain = 446.148438 Exposure Index = 449 */
         { 117641,   1940   },         /* Gain = 459.535156 Exposure Index = 450 */
         { 121171,   1940   },         /* Gain = 473.324219 Exposure Index = 451 */
         { 124807,   1940   },         /* Gain = 487.527344 Exposure Index = 452 */
         { 128552,   1940   },         /* Gain = 502.156250 Exposure Index = 453 */
         { 132409,   1940   },         /* Gain = 517.222656 Exposure Index = 454 */
         { 136382,   1940   },         /* Gain = 532.742188 Exposure Index = 455 */
         { 140474,   1940   },         /* Gain = 548.726562 Exposure Index = 456 */
         { 144689,   1940   },         /* Gain = 565.191406 Exposure Index = 457 */
         { 149030,   1940   },         /* Gain = 582.148438 Exposure Index = 458 */
         { 153501,   1940   },         /* Gain = 599.613281 Exposure Index = 459 */
         { 158107,   1940   },         /* Gain = 617.605469 Exposure Index = 460 */
         { 162851,   1940   },         /* Gain = 636.136719 Exposure Index = 461 */
         { 167737,   1940   },         /* Gain = 655.222656 Exposure Index = 462 */
         { 172770,   1940   },         /* Gain = 674.882812 Exposure Index = 463 */
         { 177954,   1940   },         /* Gain = 695.132812 Exposure Index = 464 */
         { 183293,   1940   },         /* Gain = 715.988281 Exposure Index = 465 */
         { 188792,   1940   },         /* Gain = 737.468750 Exposure Index = 466 */
         { 194456,   1940   },         /* Gain = 759.593750 Exposure Index = 467 */
         { 200290,   1940   },         /* Gain = 782.382812 Exposure Index = 468 */
         { 206299,   1940   },         /* Gain = 805.855469 Exposure Index = 469 */
         { 212488,   1940   },         /* Gain = 830.031250 Exposure Index = 470 */
         { 218863,   1940   },         /* Gain = 854.933594 Exposure Index = 471 */
         { 225429,   1940   },         /* Gain = 880.582031 Exposure Index = 472 */
         { 232192,   1940   },         /* Gain = 907.000000 Exposure Index = 473 */
         { 239158,   1940   },         /* Gain = 934.210938 Exposure Index = 474 */
         { 246333,   1940   },         /* Gain = 962.238281 Exposure Index = 475 */
         { 253723,   1940   },         /* Gain = 991.105469 Exposure Index = 476 */
      },
      /* enable_short_exp_table */
      0,
      /* short_exp_table */
      {
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
            {90,42},
            {150,42},
            {200,35},
            {280,30},
            {300,30},
            {360,25},
            {400,22},
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
         50,/* fast_conv.speed */
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
         126,
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
            {        52,           32,      1,      1,      1,       80,          4,           4,              4,                 5            },
            {        76,           32,      1,      1,      1,       78,          4,           4,              4,                 5            },
            {        86,           32,      1,      1,      1,       75,          4,           4,              4,                 5            },
            {        96,           32,      1,      1,      1,       73,          4,           4,              4,                 5            },
            {       110,           32,      1,      1,      1,       70,          6,           4,              4,                 5            },
            {       121,           32,      1,      1,      1,       55,          90,           4,              4,                 5            },
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
               2,//dark_th_low_w
               2,//dark_th_high_w
               1,//bright_th_low_w
               1,//bright_th_high_w
            },
            {
               160,//lux_index
               15,//dark_th_low
               30,//dark_th_high
               220,//bright_th_low
               240,//bright_th_high
               2,//dark_th_low_w
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
         1,//bright_en
         1,//dark_en
         1,//inter_en
         /* bright_dark_pra_t bright_dark_short_pra[MIDDLE_TRIGGER_COUNT] */
         {
            {
               0,//lux_index
               5,//dark_th_low
               10,//dark_th_high
               200,//bright_th_low
               220,//bright_th_high
               0.5,//dark_th_low_w
               0.5,//dark_th_high_w
               2,//bright_th_low_w
               4,//bright_th_high_w
            },
            {
               150,//lux_index
               5,//dark_th_low
               10,//dark_th_high
               200,//bright_th_low
               220,//bright_th_high
               0.5,//dark_th_low_w
               0.5,//dark_th_high_w
               2,//bright_th_low_w
               4,//bright_th_high_w
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
      { 0.4970, 0.5340 },   /* d50 *///{ 0.5223, 0.5420 },   /* d50 */
      { 0.5223, 0.5420 },   /* noon */
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
      { 1.0000, 1.0000 },   /* d50 */
      { 1.0000, 1.0000 },   /* noon */
      { 1.0000, 1.0000 },   /* cw */
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
         0,   /* enable */
         5,   /* count */
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
               3,
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
               1,
               2,
               5,
               1,
               1,
               1,
               1,
               1,
               3,
               4,
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
      1,//  AR_S32 enable_subsample;
      4,//AR_S32 w_ration;
      4,//AR_S32 h_ration;
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
            { 1.0000, 1.0000 },   /* d65 */
            { 1.0000, 1.0000 },   /* d50 */
            { 1.0000, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
            { 1.0000, 1.0000 },   /* tl84 */
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
            { 1.0300, 1.0300 },   /* d50 */
            { 1.0000, 1.0500 },   /* noon */
            { 1.0000, 1.0500 },   /* cw */
            { 1.0500, 1.0000 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 1.0000, 1.0000 },   /* custom1 */
            { 1.0000, 1.0000 },   /* custom2 */
         },
         270,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0000 },   /* d75 */
            { 1.0300, 1.0000 },   /* d65 */
            { 1.0000, 1.0300 },   /* d50 */
            { 1.0000, 1.0500 },   /* noon */
            { 1.0000, 1.0500 },   /* cw */
            { 1.0500, 1.0000 },   /* tl84 */
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
            { 1.0300, 1.0300 },   /* d50 */
            { 1.0000, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
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
            { 1.0300, 1.0300 },   /* d50 */
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
            { 1.0300, 1.0300 },   /* d50 */
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
            { 1.0300, 1.0600 },   /* d50 */
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
/*af_tuning_t  af_tuning*/
{
  0x00000001, /*version*/
  0,/*enable*/
  100,/*infinite*/
  512,/*near*/
  100,/*hyperfocal*/
  5,/*init_skip*/
  1, /*algo_type: 0-FOCUS_FULL_SWEEP, 1-FOCUS_SINGLE*/
  /*fullswep_algo_tuning_t*/
  {
    4,/*full_sweep_tuning_pra.code_per_step*/
    5,/*full_sweep_tuning_pra.skip_count_after_lens_move*/
  },
  4,/*af_stats_type 1-AF_STATS_SHARPNES, 2-AF_STATS_FIR, 4-AF_STATS_IIR*/
  /*roi_t*/
  {
    0.35,/*roi.x*/
    0.3,/*roi.y*/
    0.3,/*roi.width*/
    0.4,/*roi.height*/
  },
  /*stats_weight_table 16*9=144 */
  {
    //0   1    2    3    4    5    6    7    8   9    10   11   12   13   14   15
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, //0
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, //1
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 3.0, 2.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, //2
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 3.0, 3.0, 3.0, 2.0, 1.0, 1.0, 1.0, 1.0, 1.0, //3
    1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 3.0, 3.0, 3.0, 3.0, 3.0, 2.0, 1.0, 1.0, 1.0, 1.0, //4
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 3.0, 3.0, 3.0, 2.0, 1.0, 1.0, 1.0, 1.0, 1.0, //5
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 3.0, 2.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, //6
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, //7
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, //8
  },
  /*actuator_tunning_pra_t*/
  {
    1, //enable
    0,//damping_enable
    0,//init_dac
    0,//infinit_dac
    0,//hyperfocal_dac
    512,//near_dac
    0,//dmp_count
    /*move_to_far_damping*/
    {
      /*move_lens_steps; damp_value0; damp_value1;*/
      {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
      {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
    },
    /*move_to_near_damping*/
    {
      /*move_lens_steps; damp_value0; damp_value1;*/
      {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
      {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
    },
  },
  /*roi_type*/
  0, /*0-AF_ROI_TYPE_GENERAL*/
  /*run_mode*/
  2, /*1-AF_RUN_MODE_CAMERA, 2-AF_RUN_MODE_VIDEO*/
  /*af_monit_t af_monit*/
  {
    /*af_tuning_sad_t*/
    {
      1, /*af_par_sad.enable*/
      2.0, /*float af_par_sad.gain_min*/
      30.0, /*float af_par_sad.gain_max*/
      2.0, /*float af_par_sad.ref_gain_min*/
      30.0, /*float af_par_sad.ref_gain_max*/
      2, /*af_par_sad.threshold_min*/
      3, /*af_par_sad.threshold_max*/
      2, /*af_par_sad.ref_threshold_min*/
      4, /*af_par_sad.ref_threshold_max*/
      5, /*af_par_sad.frames_to_wait*/
    },
  },
  /*af_tuning_single_t*/
  {
    /*the index of array index[50]  single_optic_t optics;*/
    {
      5,/*the index of far_zone;*/
      6,/*the index of mid_zone;*/
      7,/*the index of near_zone;*/
      8,/*the index of init_pos;*/
      9,/*the index of far_start_pos;*/
      10,/*the index of near_start_pos;*/
      11,/*the index of TAF_far_end;*/
      12,/*the index of TAF_near_end;*/
      13,/*the index of CAF_far_end;*/
      14,/*the index of CAF_near_end;*/
      15,/*the index of srch_rgn_1;*/
      16,/*the index of srch_rgn_2;*/
      17,/*the index of srch_rgn_3;*/
      18,/*the index of fine_srch_rgn;*/
    },
    1,/*actuator_type: 0-ACT_TYPE_CLOSELOOP, 1-ACT_TYPE_OPENLOOP*/
    /*int index[50];*/
    {
      512, /*[0]*/ //single-near-limix-idx
      492, /*[1]*/ //single-10cm-idx
      412, /*[2]*/ //single-14cm-idx
      300, /*[3]*/ //single-20cm-idx
      100, /*[4]*/  //single-Inf-idx

      212, /*[5]*/ //far zone
      262, /*[6]*/ //mid zone
      300, /*[7]*/ //near zone

      100, /*[8]*/ //init pos,
      100, /*[9]*/ //far start pos
      512, /*[10]*/ //near start pos

      100, /*[11]*/ //TAF Far end
      512, /*[12]*/ //TAF near end

      100, /*[13]*/ //CAF Far end
      512, /*[14]*/ //CAF near end

      300, /*[15]*/ //srch rgn 1 for CAF/TAF
      200, /*[16]*/ //srch rgn 2 for CAF/TAF
      100, /*[17]*/ //srch rgn 3 for CAF/TAF
      50, /*[18]*/ //fine srch rgn
    },
    0, /*int  is_hys_comp_needed*/
    /*single_threshold_t hw;*/
    {
        40, /*uint32_t hist_dec_dec_thres; */
        0.60, /*float drop_thres; fratio threshold */
        /*dec_dec_3frame; */
        {/*(float thres[10])*/
          0.850, /*[0]*/
          0.850, /*[1]*/
          0.850, /*[2]*/
          0.850, /*[3]*/
          0.850, /*[4]*/
          0.850, /*[5]*/
          0.850, /*[6]*/
          0.850, /*[7]*/
          0.850, /*[8]*/
          0.850, /*[9]*/
        },
        /*inc_dec; */
        {/*(float thres[10])*/
          0.900, /*[0]*/
          0.900, /*[1]*/
          0.900, /*[2]*/
          0.900, /*[3]*/
          0.900, /*[4]*/
          0.900, /*[5]*/
          0.900, /*[6]*/
          0.900, /*[7]*/
          0.900, /*[8]*/
          0.900, /*[9]*/
        },
        /*inc_dec_3frame; */
        {/*(float thres[10])*/
          0.850, /*[0]*/
          0.850, /*[1]*/
          0.850, /*[2]*/
          0.850, /*[3]*/
          0.850, /*[4]*/
          0.850, /*[5]*/
          0.850, /*[6]*/
          0.850, /*[7]*/
          0.850, /*[8]*/
          0.850, /*[9]*/
        },
        /*dec_dec; */
        {/*(float thres[10])*/
        },
        /*dec_dec_noise; */
        {/*(float thres[10])*/
          0.930, /*[0]*/
          0.930, /*[1]*/
          0.930, /*[2]*/
          0.930, /*[3]*/
          0.930, /*[4]*/
          0.930, /*[5]*/
          0.930, /*[6]*/
          0.930, /*[7]*/
          0.930, /*[8]*/
          0.930, /*[9]*/
        },
        /*flat_threshold; */
        {/*(float thres[10])*/
          0.980, /*[0]*/
          0.980, /*[1]*/
          0.980, /*[2]*/
          0.980, /*[3]*/
          0.980, /*[4]*/
          0.980, /*[5]*/
          0.980, /*[6]*/
          0.980, /*[7]*/
          0.980, /*[8]*/
          0.980, /*[9]*/
        },
        40,/*uint32_t hist_inc_dec_thres; */
        0.95,/*float    flat_dec_thres; */
        1.05,/*float    flat_inc_thres; */
        1.3,/*float    macro_thres; */
    },
    /*BV_gain float, not used yet*/
    {
      1000,/*[0] 0 Lux*/
      1000,/*[1]*20 Lux*/
      1000,/*[2] 50 Lux*/
      1000,/*[3] 100 Lux*/
      1000,/*[4] 400Lux*/
      1000,/*[5] 700 Lux*/
      1000,/*[6] Over 700 Lux(outdoor lux)*/
      1000,/*[7] Sky Lux*/
      1000,/*[8] Max Lux*/
      1000,/*[9] */
    },
    1, /*step_index_per_um, the multiplicative coefficient of CAF/TAF*/
    /*step_size_table_t CAF_step_table*/
    {
      /*step_rgn_t   Prescan_low_light*/
      {
        18, /*int   rgn_0*/
        18, /*int   rgn_1*/
        18, /*int   rgn_2*/
        18, /*int   rgn_3*/
        18, /*int   rgn_4*/
      },
      /*step_rgn_t   Prescan_normal_light*/
      {
        17, /*int   rgn_0*/
        17, /*int   rgn_1*/
        17, /*int   rgn_2*/
        17, /*int   rgn_3*/
        17, /*int   rgn_4*/
      },
      /*step_rgn_t   Finescan_low_light*/
      {
        2, /*int   rgn_0*/
        2, /*int   rgn_1*/
        2, /*int   rgn_2*/
        2, /*int   rgn_3*/
        2, /*int   rgn_4*/
      },
      /*step_rgn_t   Finescan_normal_light*/
      {
        3, /*int   rgn_0*/
        3, /*int   rgn_1*/
        3, /*int   rgn_2*/
        3, /*int   rgn_3*/
        3, /*int   rgn_4*/
      },
    },
    /*step_size_table_t TAF_step_table, not used yet??*/
    {
      /*step_rgn_t   Prescan_low_light*/
      {
        16, /*int   rgn_0*/
        16, /*int   rgn_1*/
        16, /*int   rgn_2*/
        16, /*int   rgn_3*/
        16, /*int   rgn_4*/
      },
      /*step_rgn_t   Prescan_normal_light*/
      {
        15, /*int   rgn_0*/
        15, /*int   rgn_1*/
        15, /*int   rgn_2*/
        15, /*int   rgn_3*/
        15, /*int   rgn_4*/
      },
      /*step_rgn_t   Finescan_low_light*/
      {
        4, /*int   rgn_0*/
        4, /*int   rgn_1*/
        4, /*int   rgn_2*/
        4, /*int   rgn_3*/
        4, /*int   rgn_4*/
      },
      /*step_rgn_t   Finescan_normal_light*/
      {
        4, /*int   rgn_0*/
        4, /*int   rgn_1*/
        4, /*int   rgn_2*/
        4, /*int   rgn_3*/
        4, /*int   rgn_4*/
      },
    },
    /*skip_frame*/
    {
       3,// > large_range
       1,// > small_range
       1,// others
    },
  },
  1,//count_ae
  1,//trigger_mode 0-gain, 1-lux
  0,//interpolation_enable
  /*aec_trigger[MIN_TRIGGER_COUNT]*/
  {
     /* aec trigger 0 */
     {
        0.000000,
        0.000000,
     },
     /* aec trigger 1 */
     {
        0.000000,
        0.000000,
     },
     /* aec trigger 2 */
     {
        0.000000,
        0.000000,
     },
  },
  /* af_stats_tuning_pra[MIN_TRIGGER_COUNT] */
  {
     /* af_stats_tuning_pra 0 */
     {
        0.000000,//trigger_val not used
        8,//coef_y_b
        8,//coef_y_gb
        8,//coef_y_gr
        8,//coef_y_r
        2,//fv_mode 0-fv_max 1-fv_acc 2-sqr
        1,//ch_sel 0-R 1-GR 2-GB 3-B 4-Y
        0,//zoom_en
        0,//zoom_dwn
        0,//compander_en
        1,//lowfilter_en
        460,//high_luma_th
        0,//blc_b
        0,//blc_gb
        0,//blc_r
        0,//blc_gr
        0,//sharp_th
        0,//sharp_bitshift
        0,//sharp_sqr_bitshift
        /*sharp_filter_coff[39]*/
        {
           1,1,1,1,1,1023,1023,1023,1023,1023,1,1,1,
           1,1,1023,1023,1023,1023,1023,1,1,1,1,1,1023,
           1023,1023,1023,1023,1,1,1,1,1,1023,1023,1023,1023,
        },
        0,//fir_th
        0,//fir_bitshift
        0,//fir_sqr_bitshift
        /*fir_filter_coff[3]*/
        {
           1023,2,1023,
        },
        1,//iir1_en
        1,//iir0_en
        14,//iir_bitshift_2
        14,//iir_bitshift_1
        0,//iir_th
        /*iir_filter_coff[10]*/
        {
           10868,240409,10868,30536,247611,20111,221920,20111,28277,249899,
        },
        0,//iir_sqr_bitshift
        /*af_stats_table[256]*/
        {
           16384,0,0,0,32784,0,0,0,49184,0,0,0,65584,0,0,0,
           81984,0,0,0,98384,0,0,0,114784,0,0,0,131184,0,0,0,
           147584,0,0,0,163984,0,0,0,180384,0,0,0,196784,0,0,0,
           213184,0,0,0,229584,0,0,0,245984,0,0,0,262384,0,0,0,
           278784,0,0,0,295184,0,0,0,311584,0,0,0,327984,0,0,0,
           344384,0,0,0,360784,0,0,0,377184,0,0,0,393584,0,0,0,
           409984,0,0,0,426384,0,0,0,442784,0,0,0,459184,0,0,0,
           475584,0,0,0,491984,0,0,0,508384,0,0,0,524784,0,0,0,
           541184,0,0,0,557584,0,0,0,573984,0,0,0,590384,0,0,0,
           606784,0,0,0,623184,0,0,0,639584,0,0,0,655984,0,0,0,
           672384,0,0,0,688784,0,0,0,705184,0,0,0,721584,0,0,0,
           737984,0,0,0,754384,0,0,0,770784,0,0,0,787184,0,0,0,
           803584,0,0,0,819984,0,0,0,836384,0,0,0,852784,0,0,0,
           869184,0,0,0,885584,0,0,0,901984,0,0,0,918384,0,0,0,
           934784,0,0,0,951184,0,0,0,967584,0,0,0,983984,0,0,0,
           1000384,0,0,0,1016784,0,0,0,1033184,0,0,0,1048560,0,0,0,
        },
     },
     /* af_stats_tuning_pra 1 */
     {
        0.000000,//trigger_val not used
        8,//coef_y_b
        8,//coef_y_gb
        8,//coef_y_gr
        8,//coef_y_r
        2,//fv_mode 0-fv_max 1-fv_acc 2-sqr
        1,//ch_sel 0-R 1-GR 2-GB 3-B 4-Y
        0,//zoom_en
        0,//zoom_dwn
        0,//compander_en
        1,//lowfilter_en
        460,//high_luma_th
        0,//blc_b
        0,//blc_gb
        0,//blc_r
        0,//blc_gr
        0,//sharp_th
        0,//sharp_bitshift
        0,//sharp_sqr_bitshift
        /*sharp_filter_coff[39]*/
        {
           1,1,1,1,1,1023,1023,1023,1023,1023,1,1,1,
           1,1,1023,1023,1023,1023,1023,1,1,1,1,1,1023,
           1023,1023,1023,1023,1,1,1,1,1,1023,1023,1023,1023,
        },
        0,//fir_th
        0,//fir_bitshift
        0,//fir_sqr_bitshift
        /*fir_filter_coff[3]*/
        {
           1023,2,1023,
        },
        1,//iir1_en
        1,//iir0_en
        14,//iir_bitshift_2
        14,//iir_bitshift_1
        0,//iir_th
        /*iir_filter_coff[10]*/
        {
           10868,240409,10868,30536,247611,20111,221920,20111,28277,249899,
        },
        0,//iir_sqr_bitshift
        /*af_stats_table[256]*/
        {
           16384,0,0,0,32784,0,0,0,49184,0,0,0,65584,0,0,0,
           81984,0,0,0,98384,0,0,0,114784,0,0,0,131184,0,0,0,
           147584,0,0,0,163984,0,0,0,180384,0,0,0,196784,0,0,0,
           213184,0,0,0,229584,0,0,0,245984,0,0,0,262384,0,0,0,
           278784,0,0,0,295184,0,0,0,311584,0,0,0,327984,0,0,0,
           344384,0,0,0,360784,0,0,0,377184,0,0,0,393584,0,0,0,
           409984,0,0,0,426384,0,0,0,442784,0,0,0,459184,0,0,0,
           475584,0,0,0,491984,0,0,0,508384,0,0,0,524784,0,0,0,
           541184,0,0,0,557584,0,0,0,573984,0,0,0,590384,0,0,0,
           606784,0,0,0,623184,0,0,0,639584,0,0,0,655984,0,0,0,
           672384,0,0,0,688784,0,0,0,705184,0,0,0,721584,0,0,0,
           737984,0,0,0,754384,0,0,0,770784,0,0,0,787184,0,0,0,
           803584,0,0,0,819984,0,0,0,836384,0,0,0,852784,0,0,0,
           869184,0,0,0,885584,0,0,0,901984,0,0,0,918384,0,0,0,
           934784,0,0,0,951184,0,0,0,967584,0,0,0,983984,0,0,0,
           1000384,0,0,0,1016784,0,0,0,1033184,0,0,0,1048560,0,0,0,
        },
     },
     /* af_stats_tuning_pra 2 */
     {
        0.000000,//trigger_val not used
        8,//coef_y_b
        8,//coef_y_gb
        8,//coef_y_gr
        8,//coef_y_r
        2,//fv_mode 0-fv_max 1-fv_acc 2-sqr
        1,//ch_sel 0-R 1-GR 2-GB 3-B 4-Y
        0,//zoom_en
        0,//zoom_dwn
        0,//compander_en
        1,//lowfilter_en
        460,//high_luma_th
        0,//blc_b
        0,//blc_gb
        0,//blc_r
        0,//blc_gr
        0,//sharp_th
        0,//sharp_bitshift
        0,//sharp_sqr_bitshift
        /*sharp_filter_coff[39]*/
        {
           1,1,1,1,1,1023,1023,1023,1023,1023,1,1,1,
           1,1,1023,1023,1023,1023,1023,1,1,1,1,1,1023,
           1023,1023,1023,1023,1,1,1,1,1,1023,1023,1023,1023,
        },
        0,//fir_th
        0,//fir_bitshift
        0,//fir_sqr_bitshift
        /*fir_filter_coff[3]*/
        {
           1023,2,1023,
        },
        1,//iir1_en
        1,//iir0_en
        14,//iir_bitshift_2
        14,//iir_bitshift_1
        0,//iir_th
        /*iir_filter_coff[10]*/
        {
           10868,240409,10868,30536,247611,20111,221920,20111,28277,249899,
        },
        0,//iir_sqr_bitshift
        /*af_stats_table[256]*/
        {
           16384,0,0,0,32784,0,0,0,49184,0,0,0,65584,0,0,0,
           81984,0,0,0,98384,0,0,0,114784,0,0,0,131184,0,0,0,
           147584,0,0,0,163984,0,0,0,180384,0,0,0,196784,0,0,0,
           213184,0,0,0,229584,0,0,0,245984,0,0,0,262384,0,0,0,
           278784,0,0,0,295184,0,0,0,311584,0,0,0,327984,0,0,0,
           344384,0,0,0,360784,0,0,0,377184,0,0,0,393584,0,0,0,
           409984,0,0,0,426384,0,0,0,442784,0,0,0,459184,0,0,0,
           475584,0,0,0,491984,0,0,0,508384,0,0,0,524784,0,0,0,
           541184,0,0,0,557584,0,0,0,573984,0,0,0,590384,0,0,0,
           606784,0,0,0,623184,0,0,0,639584,0,0,0,655984,0,0,0,
           672384,0,0,0,688784,0,0,0,705184,0,0,0,721584,0,0,0,
           737984,0,0,0,754384,0,0,0,770784,0,0,0,787184,0,0,0,
           803584,0,0,0,819984,0,0,0,836384,0,0,0,852784,0,0,0,
           869184,0,0,0,885584,0,0,0,901984,0,0,0,918384,0,0,0,
           934784,0,0,0,951184,0,0,0,967584,0,0,0,983984,0,0,0,
           1000384,0,0,0,1016784,0,0,0,1033184,0,0,0,1048560,0,0,0,
        },
     },
  },
},

