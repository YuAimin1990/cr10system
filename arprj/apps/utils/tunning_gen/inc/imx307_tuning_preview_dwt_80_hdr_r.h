
/*---------------header of imx307 tuing-------------------------*/

/* version of the tuning header uint32_t header_version */
0x20220106,
/* tuning_ctl_t tuning_ctl */
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
       6,
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
             220,
          },
          /* aec trigger 2 */
          {
             240,
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
              5,//hdr_motion_mp_sl
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
              64,//mv_fix_th
              10,//mv_fix_value
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
              64,//hdr_motion_mp_sm
              1,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              64,//hdr_exp_value_th1_sl
              512,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              5,//hdr_motion_mp_sl
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
              64,//mv_fix_th
              10,//mv_fix_value
              1,//mv_fix_en
              0,//use_long_exp_fix
              1,//use_mv_fix_value_fix
              1,//enable_sexp_denoise
              /* gaus_coef[4] */
              {
                 3,21,21,160,
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
              78,//hdr_motion_mp_sm
              1,//hdr_motion_en_sl
              0,//hdr_alpha_s_th_sl
              64,//hdr_exp_value_th1_sl
              512,//hdr_th2_th1_reverse_sl
              0,//hdr_noise_sp_sl
              5,//hdr_motion_mp_sl
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
              64,//mv_fix_th
              10,//mv_fix_value
              1,//mv_fix_en
              0,//use_long_exp_fix
              1,//use_mv_fix_value_fix
              1,//enable_sexp_denoise
              /* gaus_coef[4] */
              {
                 1,16,16,188,
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
              7,//hdr_motion_mp_sl
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
              10,//mv_fix_value
              1,//mv_fix_en
              0,//use_long_exp_fix
              1,//use_mv_fix_value_fix
              0,//enable_sexp_denoise
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
              7,//hdr_motion_mp_sl
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
              0,//enable_sexp_denoise
              /* gaus_coef[4] */
              {
                 0,10,10,216,
              },
           },
           /* para 5*/
           {
              275,//blc_short_exp
              275,//blc_mid_exp
              275,//blc_long_exp
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
              7,//hdr_motion_mp_sl
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
              0,//enable_sexp_denoise
              /* gaus_coef[4] */
              {
                 0,10,10,216,
              },
           },
       },
    },
    /* isp_sub_module_compander_tuning_t isp_sub_module_compander_tuning*/
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
             0x0000004c,0x00000000,0x000003e8,0x00000578,0x00000640,0x000003e8,0x00000578,0x00000640,0x00000c00,0x000000c8,
             0x00000190,0x00000bb8,0x000000c8,0x00000190,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
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
             0x00000026,0x00000000,0x000003e8,0x00000578,0x00000640,0x000003e8,0x000000c8,0x00000320,0x00000bb8,0x000000c8,
             0x00000fa0,0x000003e8,0x000000c8,0x00000320,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
             0x00000000,0x00000020,0x00000060,0x000000a0,0x000000e0,
          },
          /* parameter 3 */
          {
             0x00000000,0x00000000,0x000003e8,0x00000578,0x00000640,0x000003e8,0x000000c8,0x00000320,0x00000bb8,0x000000c8,
             0x00000190,0x000003e8,0x000000c8,0x00000320,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
             0x00000000,0x00000020,0x00000060,0x000000a0,0x000000e0,
          },
          /* parameter 4 */
          {
             0x00000000,0x00000000,0x000003e8,0x00000578,0x00000640,0x000003e8,0x000000c8,0x00000320,0x00000bb8,0x000000c8,
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
             180,
          },
          /* aec trigger 1 */
          {
             210,
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
                       {5,5,5},
                       /* channel 2 */
                       {5,5,5},
                       /* channel 3 */
                       {5,5,5},
                       /* channel 4 */
                       {5,5,5},
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
                      {0,2,4,6,8,10,12,14,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {0,2,4,6,8,10,12,14,16},
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
                       {10,10,10},
                       /* channel 2 */
                       {10,10,10},
                       /* channel 3 */
                       {10,10,10},
                       /* channel 4 */
                       {10,10,10},
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
                      {1,2,4,6,8,10,12,14,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {1,2,4,6,8,10,12,14,16},
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
                       {15,15,15},
                       /* channel 2 */
                       {15,15,15},
                       /* channel 3 */
                       {15,15,15},
                       /* channel 4 */
                       {15,15,15},
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
                      {1,2,4,6,8,10,12,14,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {1,2,4,6,8,10,12,14,16},
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
               0,//profile_enable
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
               0,//profile_enable
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
    /* isp_sub_module_decompander_tuning_t isp_sub_module_decompander_tuning*/
    {
	   /* enable */
       1,
    },
    /* isp_sub_module_lsc_tuning_t isp_sub_module_lsc_tuning*/
    {
        0,//enable
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
             120,
          },
          /* aec trigger 1 */
          {
             160,
             210,
          },
          /* aec trigger 2 */
          {
             240,
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
                   110930,110930,104393,99761,96079,92987,90304,87924,85781,83828,82031,80366,78814,77360,75991,74697,73471,72306,71195,70134,69118,68144,67209,66309,65441,
                   64605,63797,63015,62259,61526,60815,60125,59455,58804,58170,57553,56952,56367,55795,55238,54694,54163,53644,53136,52640,52154,51678,51213,50757,50310,
                   49872,49442,49021,48608,48202,47804,47413,47029,46652,46281,45917,45559,45207,44860,44519,44184,43854,43529,43209,42894,42584,42278,41977,41681,41388,
                   41100,40816,40536,40260,39987,39719,39454,39192,38934,38679,38428,38180,37935,37693,37454,37218,36985,36755,36528,36303,36081,35862,35645,35431,35219,
                   35009,34803,34598,34396,34195,33998,33802,33608,33417,33227,33040,32854,32671,32489,32310,32132,31956,31781,31609,31438,31269,31102,30936,30772,30609,
                   30448,30289,30131,29974,29819,29666,29514,29363,29214,29066,28920,28774,28630,28488,28346,28206,28067,27930,27793,27658,27524,27391,27259,27129,26999,
                   26870,26743,26617,26491,26367,26244,26122,26000,25880,25761,25642,25525,25408,25293,25178,25065,24952,24840,24729,24618,24509,24400,24293,24186,24080,
                   23974,23870,23766,23663,23561,23459,23358,23258,23159,23061,22963,22866,22769,22673,22578,22484,22390,22297,22204,22113,22021,21931,21841,21752,21663,
                   21575,21487,21400,21314,21228,21143,21059,20974,20891,20808,20726,20644,20562,20482,20401,20322,20242,20164,20085,20008,19930,19854,19777,19702,19626,
                   19551,19477,19403,19330,19257,19184,19112,19040,18969,18898,18828,18758,18688,18619,18550,18482,18414,18347,18280,18213,18147,18081,18015,17950,17885,
                   17821,17757,17693,17630,17567,17504,17442,
              },
              /* drc_high_LUT_point */
              {
                   73467,73467,59450,50752,44514,39713,35857,32666,29970,27653,25638,23865,22293,20887,19622,18478,17438,16489,15618,14818,14079,13396,12761,12172,11622,
                   11108,10627,10177,9753,9355,8980,8626,8292,7976,7677,7393,7124,6869,6626,6395,6174,5964,5764,5573,5390,5215,5048,4888,4735,4588,
                   4447,4312,4182,4058,3938,3823,3712,3605,3502,3403,3308,3216,3127,3041,2959,2879,2802,2727,2655,2586,2518,2453,2390,2329,2270,
                   2213,2157,2103,2051,2001,1952,1904,1858,1813,1769,1727,1686,1646,1607,1570,1533,1497,1463,1429,1396,1364,1333,1303,1273,1245,
                   1217,1189,1163,1137,1112,1087,1063,1040,1017, 995, 973, 952, 932, 911, 892, 873, 854, 836, 818, 800, 783, 766, 750, 734, 719,
                    704, 689, 674, 660, 646, 632, 619, 606, 593, 581, 569, 557, 545, 534, 523, 512, 501, 490, 480, 470, 460, 451, 441, 432, 423,
                    414, 405, 397, 388, 380, 372, 364, 356, 349, 341, 334, 327, 320, 313, 306, 299, 293, 287, 280, 274, 268, 262, 256, 251, 245,
                    240, 234, 229, 224, 218, 213, 209, 204, 199, 194, 190, 185, 181, 176, 172, 168, 164, 160, 156, 152, 148, 144, 140, 137, 133,
                    129, 126, 123, 119, 116, 113, 109, 106, 103, 100,  97,  94,  91,  88,  86,  83,  80,  77,  75,  72,  70,  67,  65,  62,  60,
                     57,  55,  53,  51,  48,  46,  44,  42,  40,  38,  36,  34,  32,  30,  28,  26,  24,  22,  21,  19,  17,  15,  14,  12,  10,
                      9,   7,   6,   4,   2,   1,   0,
              },
              /* saturation */
              135,
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
              7,
              /* k3 */
              0.45,
           },
           /* para 1*/
           {
              /* drc_low_LUT_point */
              {
                   124797,124797,117442,112231,108089,104610,101592,98915,96504,94306,92285,90412,88666,87030,85490,84034,82655,81344,80094,78901,77758,76662,75610,74597,73621,
                   72680,71771,70892,70041,69217,68417,67641,66887,66154,65441,64747,64071,63413,62770,62143,61531,60933,60349,59778,59220,58673,58138,57614,57101,56599,
                   56106,55623,55149,54684,54228,53780,53340,52908,52484,52067,51657,51254,50857,50468,50084,49707,49336,48970,48610,48256,47907,47563,47224,46891,46562,
                   46238,45918,45603,45292,44986,44683,44385,44091,43801,43514,43231,42952,42677,42404,42136,41870,41608,41349,41094,40841,40591,40344,40101,39859,39621,
                   39386,39153,38923,38695,38470,38247,38027,37809,37594,37381,37170,36961,36755,36550,36348,36148,35950,35754,35560,35368,35178,34989,34803,34618,34435,
                   34254,34075,33897,33721,33547,33374,33203,33034,32866,32699,32535,32371,32209,32049,31890,31732,31576,31421,31268,31115,30965,30815,30667,30520,30374,
                   30229,30086,29944,29803,29663,29524,29387,29250,29115,28981,28848,28716,28585,28455,28326,28198,28071,27945,27820,27696,27573,27450,27329,27209,27090,
                   26971,26853,26737,26621,26506,26392,26278,26166,26054,25943,25833,25724,25615,25507,25400,25294,25189,25084,24980,24877,24774,24672,24571,24471,24371,
                   24272,24173,24076,23978,23882,23786,23691,23596,23502,23409,23316,23224,23133,23042,22952,22862,22773,22684,22596,22509,22422,22335,22250,22164,22080,
                   21995,21912,21829,21746,21664,21582,21501,21420,21340,21261,21181,21103,21024,20947,20869,20793,20716,20640,20565,20490,20415,20341,20267,20194,20121,
                   20049,19976,19905,19834,19763,19692,19622,
              },
              /* drc_high_LUT_point */
              {
                   82650,82650,66881,57096,50078,44678,40339,36749,33716,31110,28843,26848,25079,23498,22075,20788,19618,18550,17571,16670,15839,15070,14357,13693,13074,
                   12497,11956,11449,10973,10525,10103,9705,9329,8973,8637,8318,8015,7727,7454,7194,6946,6710,6485,6269,6064,5867,5679,5499,5327,5162,
                   5003,4851,4705,4565,4430,4300,4176,4056,3940,3829,3721,3618,3518,3422,3329,3239,3152,3068,2987,2909,2833,2760,2689,2620,2554,
                   2489,2427,2366,2308,2251,2196,2142,2090,2040,1991,1943,1897,1852,1808,1766,1725,1684,1645,1608,1571,1535,1500,1466,1433,1400,
                   1369,1338,1308,1279,1251,1223,1196,1170,1144,1119,1095,1071,1048,1025,1003, 982, 961, 940, 920, 900, 881, 862, 844, 826, 809,
                    792, 775, 758, 742, 727, 712, 697, 682, 668, 654, 640, 627, 613, 601, 588, 576, 564, 552, 540, 529, 518, 507, 496, 486, 476,
                    466, 456, 446, 437, 427, 418, 410, 401, 392, 384, 376, 368, 360, 352, 344, 337, 330, 322, 315, 308, 302, 295, 288, 282, 276,
                    270, 263, 257, 252, 246, 240, 235, 229, 224, 219, 213, 208, 203, 198, 194, 189, 184, 180, 175, 171, 166, 162, 158, 154, 150,
                    146, 142, 138, 134, 130, 127, 123, 120, 116, 113, 109, 106, 103,  99,  96,  93,  90,  87,  84,  81,  78,  76,  73,  70,  67,
                     65,  62,  59,  57,  54,  52,  50,  47,  45,  42,  40,  38,  36,  34,  31,  29,  27,  25,  23,  21,  19,  17,  15,  14,  12,
                     10,   8,   6,   5,   3,   1,   0,
              },
              /* saturation */
              135,
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
              7,
              /* k3 */
              0.45,
           },
           /* para 2*/
           {
              /* drc_low_LUT_point */
              {
                   119018,119018,114473,111104,108340,105961,103856,101957,100220,98616,97123,95723,94404,93156,91971,90842,89763,88729,87737,86783,85863,84975,84117,83287,82482,
                   81702,80944,80208,79491,78793,78113,77451,76804,76172,75555,74952,74362,73785,73220,72667,72124,71593,71071,70560,70058,69565,69081,68606,68139,67679,
                   67228,66784,66347,65917,65494,65077,64667,64263,63865,63473,63086,62705,62330,61960,61594,61234,60878,60528,60182,59840,59503,59170,58841,58516,58195,
                   57878,57565,57256,56950,56648,56349,56054,55762,55473,55188,54905,54626,54350,54077,53806,53539,53274,53012,52753,52496,52242,51990,51741,51495,51251,
                   51009,50770,50532,50297,50065,49834,49606,49380,49156,48933,48713,48495,48279,48065,47852,47642,47433,47226,47021,46817,46616,46416,46217,46021,45826,
                   45632,45440,45250,45061,44874,44688,44504,44321,44140,43960,43781,43604,43428,43253,43080,42908,42738,42568,42400,42233,42068,41903,41740,41578,41417,
                   41257,41099,40941,40785,40630,40476,40323,40171,40020,39870,39721,39573,39426,39280,39135,38991,38848,38706,38565,38425,38286,38148,38010,37874,37738,
                   37603,37469,37336,37204,37072,36942,36812,36683,36555,36427,36301,36175,36050,35926,35802,35679,35557,35436,35315,35195,35076,34958,34840,34723,34607,
                   34491,34376,34261,34148,34035,33922,33810,33699,33589,33479,33369,33261,33153,33045,32938,32832,32726,32621,32517,32413,32309,32207,32104,32003,31901,
                   31801,31701,31601,31502,31403,31305,31208,31111,31014,30918,30823,30728,30633,30539,30446,30353,30260,30168,30077,29985,29895,29804,29715,29625,29536,
                   29448,29360,29272,29185,29098,29012,28926,
              },
              /* drc_high_LUT_point */
              {
                   89759,89759,76799,68133,61589,56343,51985,48273,45055,42227,39715,37463,35430,33583,31896,30347,28921,27601,26377,25238,24175,23181,22250,21375,20551,
                   19775,19042,18350,17694,17072,16481,15920,15387,14878,14394,13931,13489,13067,12663,12277,11906,11551,11211,10883,10569,10267,9977,9698,9429,9169,
                   8920,8679,8447,8223,8006,7797,7595,7400,7212,7029,6853,6682,6516,6356,6200,6050,5904,5762,5625,5491,5362,5237,5115,4996,4881,
                   4769,4660,4555,4452,4352,4254,4160,4067,3978,3890,3805,3722,3641,3563,3486,3411,3338,3267,3198,3130,3064,3000,2937,2876,2816,
                   2757,2700,2644,2590,2536,2484,2434,2384,2335,2288,2242,2196,2152,2108,2066,2024,1984,1944,1905,1867,1830,1793,1757,1722,1688,
                   1655,1622,1590,1558,1527,1497,1467,1438,1410,1382,1354,1328,1301,1275,1250,1225,1201,1177,1154,1131,1108,1086,1064,1043,1022,
                   1002, 982, 962, 942, 923, 905, 886, 868, 851, 833, 816, 799, 783, 767, 751, 735, 720, 705, 690, 675, 661, 647, 633, 620, 606,
                    593, 580, 567, 555, 543, 531, 519, 507, 496, 484, 473, 462, 452, 441, 431, 420, 410, 400, 391, 381, 372, 362, 353, 344, 335,
                    327, 318, 310, 301, 293, 285, 277, 269, 262, 254, 247, 239, 232, 225, 218, 211, 204, 198, 191, 184, 178, 172, 165, 159, 153,
                    147, 141, 136, 130, 124, 119, 113, 108, 103,  97,  92,  87,  82,  77,  72,  68,  63,  58,  54,  49,  45,  40,  36,  32,  27,
                     23,  19,  15,  11,   7,   3,   0,
              },
              /* saturation */
              135,
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
              6,
              /* k3 */
              0.5,
           },
           /* para 3*/
           {
              /* drc_low_LUT_point */
              {
                   119018,119018,114473,111104,108340,105961,103856,101957,100220,98616,97123,95723,94404,93156,91971,90842,89763,88729,87737,86783,85863,84975,84117,83287,82482,
                   81702,80944,80208,79491,78793,78113,77451,76804,76172,75555,74952,74362,73785,73220,72667,72124,71593,71071,70560,70058,69565,69081,68606,68139,67679,
                   67228,66784,66347,65917,65494,65077,64667,64263,63865,63473,63086,62705,62330,61960,61594,61234,60878,60528,60182,59840,59503,59170,58841,58516,58195,
                   57878,57565,57256,56950,56648,56349,56054,55762,55473,55188,54905,54626,54350,54077,53806,53539,53274,53012,52753,52496,52242,51990,51741,51495,51251,
                   51009,50770,50532,50297,50065,49834,49606,49380,49156,48933,48713,48495,48279,48065,47852,47642,47433,47226,47021,46817,46616,46416,46217,46021,45826,
                   45632,45440,45250,45061,44874,44688,44504,44321,44140,43960,43781,43604,43428,43253,43080,42908,42738,42568,42400,42233,42068,41903,41740,41578,41417,
                   41257,41099,40941,40785,40630,40476,40323,40171,40020,39870,39721,39573,39426,39280,39135,38991,38848,38706,38565,38425,38286,38148,38010,37874,37738,
                   37603,37469,37336,37204,37072,36942,36812,36683,36555,36427,36301,36175,36050,35926,35802,35679,35557,35436,35315,35195,35076,34958,34840,34723,34607,
                   34491,34376,34261,34148,34035,33922,33810,33699,33589,33479,33369,33261,33153,33045,32938,32832,32726,32621,32517,32413,32309,32207,32104,32003,31901,
                   31801,31701,31601,31502,31403,31305,31208,31111,31014,30918,30823,30728,30633,30539,30446,30353,30260,30168,30077,29985,29895,29804,29715,29625,29536,
                   29448,29360,29272,29185,29098,29012,28926,
              },
              /* drc_high_LUT_point */
              {
                   89759,89759,76799,68133,61589,56343,51985,48273,45055,42227,39715,37463,35430,33583,31896,30347,28921,27601,26377,25238,24175,23181,22250,21375,20551,
                   19775,19042,18350,17694,17072,16481,15920,15387,14878,14394,13931,13489,13067,12663,12277,11906,11551,11211,10883,10569,10267,9977,9698,9429,9169,
                   8920,8679,8447,8223,8006,7797,7595,7400,7212,7029,6853,6682,6516,6356,6200,6050,5904,5762,5625,5491,5362,5237,5115,4996,4881,
                   4769,4660,4555,4452,4352,4254,4160,4067,3978,3890,3805,3722,3641,3563,3486,3411,3338,3267,3198,3130,3064,3000,2937,2876,2816,
                   2757,2700,2644,2590,2536,2484,2434,2384,2335,2288,2242,2196,2152,2108,2066,2024,1984,1944,1905,1867,1830,1793,1757,1722,1688,
                   1655,1622,1590,1558,1527,1497,1467,1438,1410,1382,1354,1328,1301,1275,1250,1225,1201,1177,1154,1131,1108,1086,1064,1043,1022,
                   1002, 982, 962, 942, 923, 905, 886, 868, 851, 833, 816, 799, 783, 767, 751, 735, 720, 705, 690, 675, 661, 647, 633, 620, 606,
                    593, 580, 567, 555, 543, 531, 519, 507, 496, 484, 473, 462, 452, 441, 431, 420, 410, 400, 391, 381, 372, 362, 353, 344, 335,
                    327, 318, 310, 301, 293, 285, 277, 269, 262, 254, 247, 239, 232, 225, 218, 211, 204, 198, 191, 184, 178, 172, 165, 159, 153,
                    147, 141, 136, 130, 124, 119, 113, 108, 103,  97,  92,  87,  82,  77,  72,  68,  63,  58,  54,  49,  45,  40,  36,  32,  27,
                     23,  19,  15,  11,   7,   3,   0,
              },
              /* saturation */
              135,
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
              6,
              /* k3 */
              0.5,
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
              135,
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
                   128432,128432,127105,125982,124977,124053,123189,122373,121596,120853,120138,119449,118782,118135,117506,116894,116297,115714,115144,114586,114040,113504,112978,112462,111955,
                   111457,110966,110483,110008,109540,109078,108623,108174,107731,107294,106863,106437,106016,105600,105190,104783,104382,103985,103592,103203,102819,102438,102061,101688,101319,
                   100953,100591,100232,99877,99524,99175,98829,98486,98146,97809,97475,97144,96815,96489,96166,95845,95527,95211,94898,94587,94278,93972,93668,93366,93067,
                   92770,92474,92181,91890,91601,91314,91029,90746,90464,90185,89908,89632,89358,89086,88815,88547,88280,88015,87751,87489,87228,86970,86712,86457,86203,
                   85950,85699,85449,85201,84954,84709,84465,84222,83981,83741,83502,83265,83029,82795,82561,82329,82098,81869,81640,81413,81187,80962,80739,80516,80295,
                   80075,79856,79638,79421,79206,78991,78777,78565,78353,78143,77934,77725,77518,77312,77106,76902,76698,76496,76294,76094,75894,75696,75498,75301,75105,
                   74910,74716,74523,74330,74139,73948,73759,73570,73381,73194,73008,72822,72637,72453,72270,72088,71906,71725,71545,71366,71187,71010,70833,70656,70481,
                   70306,70132,69959,69786,69614,69443,69272,69103,68933,68765,68597,68430,68264,68098,67933,67769,67605,67442,67279,67117,66956,66796,66636,66476,66318,
                   66160,66002,65845,65689,65533,65378,65224,65070,64917,64764,64612,64460,64309,64159,64009,63860,63711,63563,63415,63268,63121,62975,62830,62685,62540,
                   62396,62253,62110,61967,61826,61684,61543,61403,61263,61124,60985,60846,60708,60571,60434,60298,60162,60026,59891,59756,59622,59489,59355,59223,59090,
                   58959,58827,58696,58566,58436,58306,58177,
              },
              /* drc_high_LUT_point */
              {
                   116295,116295,108171,101684,96161,91308,86964,83023,79415,76087,73001,70125,67434,64909,62532,60290,58169,56160,54253,52440,50714,49069,47499,45999,44564,
                   43191,41875,40613,39402,38240,37122,36048,35014,34018,33059,32135,31244,30384,29555,28753,27980,27232,26509,25810,25133,24479,23845,23231,22637,22061,
                   21503,20962,20437,19928,19434,18955,18489,18038,17599,17173,16759,16357,15966,15586,15216,14857,14507,14167,13836,13514,13201,12896,12599,12309,12028,
                   11753,11486,11225,10972,10724,10483,10248,10019,9795,9577,9365,9157,8955,8758,8565,8377,8194,8014,7840,7669,7502,7340,7181,7025,6874,
                   6726,6581,6440,6302,6167,6035,5906,5780,5656,5536,5418,5303,5190,5080,4972,4867,4764,4663,4565,4468,4374,4281,4191,4102,4016,
                   3931,3848,3767,3687,3610,3533,3459,3386,3314,3244,3175,3108,3042,2978,2915,2853,2792,2733,2675,2618,2562,2507,2453,2401,2349,
                   2298,2249,2200,2153,2106,2060,2015,1971,1928,1886,1845,1804,1764,1725,1686,1649,1612,1576,1540,1505,1471,1438,1405,1372,1341,
                   1310,1279,1249,1220,1191,1163,1135,1108,1081,1054,1029,1003, 979, 954, 930, 907, 884, 861, 839, 817, 795, 774, 753, 733, 713,
                    694, 674, 655, 637, 619, 601, 583, 566, 549, 532, 516, 499, 484, 468, 453, 438, 423, 408, 394, 380, 366, 353, 340, 327, 314,
                    301, 289, 276, 264, 253, 241, 230, 219, 208, 197, 186, 176, 165, 155, 145, 136, 126, 116, 107,  98,  89,  80,  72,  63,  55,
                     46,  38,  30,  22,  15,   7,   0,
              },
              /* saturation */
              135,
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
              0.7,
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
             300,
          },
          /* aec trigger 2 */
          {
             330,
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
              100000,//th1_ny
              3000000,//th2_ny
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
              200,//th2_y_max
              1,//rgb_diff_detect_en
              10,//th1_rgb_diff
              200,//th2_rgb_diff
              1,//edge_detect_en
              2000,//th1_edge
              5000,//th2_edge
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
              100000,//th1_ny
              3000000,//th2_ny
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
              200,//th2_y_max
              1,//rgb_diff_detect_en
              15,//th1_rgb_diff
              200,//th2_rgb_diff
              1,//edge_detect_en
              2000,//th1_edge
              5000,//th2_edge
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
             280,
          },
          /* aec trigger 2 */
          {
             300,
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
       0,//enable
       1,//interpolation_enable
       5,//count_ae
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             190,
          },
          /* aec trigger 1 */
          {
             210,
             300,
          },
          /* aec trigger 2 */
          {
             310,
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
             0.018,//cut_ration
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
             0.6,//ltm_dehighlight_fork
             0.8,//ltm_dehighlight_spear
             2,//ltm_dehighlight_gamma
          },
          /* para 1 */
          {
             1,//enable_cdf_smooth
             0.015,//cut_ration
             0.1,//weight
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
             0.8,//ltm_dehighlight_fork
             0.9,//ltm_dehighlight_spear
             2,//ltm_dehighlight_gamma
          },
          /* para 2 */
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
          /* para 3 */
          {
             1,//enable_cdf_smooth
             0.001,//cut_ration
             0.1,//weight
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
             0.1,//weight
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
             320,
          },
          /* aec trigger 2 */
          {
             360,
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
                1.4,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                1.4,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                1.4,//saturation
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
                1.4,//saturation
                0,//hue
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                1.3,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                1.3,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                1.3,//saturation
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
                1.3,//saturation
                0,//hue
             },
          },
          /* aec trigger 2 */
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
                1.1,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                1.1,//saturation
                0,//hue
             },
             /* awb para 4 */
             {
                1.1,//saturation
                0,//hue
             },
             /* awb para 5 */
             {
                1.1,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                1.1,//saturation
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
             180,
          },
          /* aec trigger 1 */
          {
             200,
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
             310,
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
             {4,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             50,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {4,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             {0,0,4,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             35,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
             {0,0,4,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             55,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
             {2,4,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             80,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
             {0,0,0,0,4,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             100,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
             {0,0,0,0,0,4,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             165,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
                -2,-8,-17,-23,//bpf(1,1:4)
                -8,-37,-85,-116,//bpf(2,1:4)
                -17,-85,-124,129,//bpf(3,1:4)
                -23,-116,129,1572,//bpf(4,1:4)
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
                0,-4,-18,-30,//bpf(1,1:4)
                -4,-44,-100,-120,//bpf(2,1:4)
                -18,-100,-200,488,//bpf(3,1:4)
                -30,-120,488,600,//bpf(4,1:4)
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
             220,
          },
          /* aec trigger 1 */
          {
             225,
             240,
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
             5,//nr3d_mpy_th1
             20,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             5,//nr3d_mpc_th1
             20,//nr3d_mpc_th2
             5,//nr3d_lamda2d
             10,//nr3d_ite
             0,//nr3d_msr
             240,//nr3d_decay
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
             20,//dbk_h_mv_diff
             20,//dbk_h_mv_str
             20,//dbk_v_mv_diff
             20,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             20,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             20,//dbk_v_yy_diff
             10,//dbk_satu
             0,//dbk_gaus_y_c11
             1,//dbk_gaus_y_c12
             16,//dbk_gaus_y_c13
             0,//dbk_gaus_y_c21
             188,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                1,2,3,4,5,6,8,10,12,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                1,2,4,6,8,10,12,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             8,//dbk_h_ed_thrd
             8,//dbk_v_ed_thrd
          },
          /* para 1 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             10,//nr3d_npy_th1
             80,//nr3d_npy_th2
             10,//nr3d_mpy_th1
             20,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             5,//nr3d_mpc_th1
             20,//nr3d_mpc_th2
             5,//nr3d_lamda2d
             15,//nr3d_ite
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
             12,//dbk_h_mv_diff
             12,//dbk_h_mv_str
             12,//dbk_v_mv_diff
             12,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             12,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             12,//dbk_v_yy_diff
             10,//dbk_satu
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
                1,2,3,4,5,6,8,10,12,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                1,2,4,6,8,10,12,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             8,//dbk_h_ed_thrd
             8,//dbk_v_ed_thrd
          },
          /* para 2 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             10,//nr3d_npy_th1
             80,//nr3d_npy_th2
             10,//nr3d_mpy_th1
             20,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             12,//nr3d_mpc_th1
             25,//nr3d_mpc_th2
             5,//nr3d_lamda2d
             20,//nr3d_ite
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
             15,//nr3d_mpc_th1
             30,//nr3d_mpc_th2
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
    /* isp_sub_module_dithering_tuning_t isp_sub_module_dithering_tuning */
    {
       0,//enable
    },
    /* isp_sub_module_ceva_vfe_tuning_t isp_sub_module_ceva_vfe_tuning */
    {

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
             170,
          },
          /* aec trigger 1 */
          {
             230,
             280,
          },
          /* aec trigger 2 */
          {
             310,
             340,
          },
          /* aec trigger 3 */
          {
             370,
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
                40,//y_lo_th2
                900,//y_hi_th1
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
                40,//y_lo_th2
                900,//y_hi_th1
                1023,//y_hi_th2
             },
          },
          /* aec trigger 2 */
          {
             /* awb para 0 */
             {
                1.75,//saturation
                0,//hue
                20,//y_lo_th1
                60,//y_lo_th2
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
                80,//y_lo_th2
                900,//y_hi_th1
                1023,//y_hi_th2
             },
          },
          /* aec trigger 4 */
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
       },
    },
    /* isp_sub_module_lsc_tuning_t isp_sub_module_lsc_tuning*/
    {
        0,//enable
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
   200,
   /* ae start_skip_count */
   1,
   /* ae torlerence */
   2,
   /* ae exp_table_para */
   {
      /* count */
      443,
      /* ev0_count */
      443,
      /* ev2_count */
      443,
      /* fix_fps_index */
      25,
      /* enable_isp_digital_gain */
      0,
      /* max_sensor_gain */
      500,
      /* max_isp_gain1 */
      1,
      /* exp_table */
      {
      //1,1400
      //500,1400
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
         {    258,   1400   },         /* Gain = 1.007812 Exposure Index = 234 */
         {    266,   1400   },         /* Gain = 1.039062 Exposure Index = 235 */
         {    274,   1400   },         /* Gain = 1.070312 Exposure Index = 236 */
         {    283,   1400   },         /* Gain = 1.105469 Exposure Index = 237 */
         {    292,   1400   },         /* Gain = 1.140625 Exposure Index = 238 */
         {    301,   1400   },         /* Gain = 1.175781 Exposure Index = 239 */
         {    311,   1400   },         /* Gain = 1.214844 Exposure Index = 240 */
         {    321,   1400   },         /* Gain = 1.253906 Exposure Index = 241 */
         {    331,   1400   },         /* Gain = 1.292969 Exposure Index = 242 */
         {    341,   1400   },         /* Gain = 1.332031 Exposure Index = 243 */
         {    352,   1400   },         /* Gain = 1.375000 Exposure Index = 244 */
         {    363,   1400   },         /* Gain = 1.417969 Exposure Index = 245 */
         {    374,   1400   },         /* Gain = 1.460938 Exposure Index = 246 */
         {    386,   1400   },         /* Gain = 1.507812 Exposure Index = 247 */
         {    398,   1400   },         /* Gain = 1.554688 Exposure Index = 248 */
         {    410,   1400   },         /* Gain = 1.601562 Exposure Index = 249 */
         {    423,   1400   },         /* Gain = 1.652344 Exposure Index = 250 */
         {    436,   1400   },         /* Gain = 1.703125 Exposure Index = 251 */
         {    450,   1400   },         /* Gain = 1.757812 Exposure Index = 252 */
         {    464,   1400   },         /* Gain = 1.812500 Exposure Index = 253 */
         {    478,   1400   },         /* Gain = 1.867188 Exposure Index = 254 */
         {    493,   1400   },         /* Gain = 1.925781 Exposure Index = 255 */
         {    508,   1400   },         /* Gain = 1.984375 Exposure Index = 256 */
         {    524,   1400   },         /* Gain = 2.046875 Exposure Index = 257 */
         {    540,   1400   },         /* Gain = 2.109375 Exposure Index = 258 */
         {    557,   1400   },         /* Gain = 2.175781 Exposure Index = 259 */
         {    574,   1400   },         /* Gain = 2.242188 Exposure Index = 260 */
         {    592,   1400   },         /* Gain = 2.312500 Exposure Index = 261 */
         {    610,   1400   },         /* Gain = 2.382812 Exposure Index = 262 */
         {    629,   1400   },         /* Gain = 2.457031 Exposure Index = 263 */
         {    648,   1400   },         /* Gain = 2.531250 Exposure Index = 264 */
         {    668,   1400   },         /* Gain = 2.609375 Exposure Index = 265 */
         {    689,   1400   },         /* Gain = 2.691406 Exposure Index = 266 */
         {    710,   1400   },         /* Gain = 2.773438 Exposure Index = 267 */
         {    732,   1400   },         /* Gain = 2.859375 Exposure Index = 268 */
         {    754,   1400   },         /* Gain = 2.945312 Exposure Index = 269 */
         {    777,   1400   },         /* Gain = 3.035156 Exposure Index = 270 */
         {    801,   1400   },         /* Gain = 3.128906 Exposure Index = 271 */
         {    826,   1400   },         /* Gain = 3.226562 Exposure Index = 272 */
         {    851,   1400   },         /* Gain = 3.324219 Exposure Index = 273 */
         {    877,   1400   },         /* Gain = 3.425781 Exposure Index = 274 */
         {    904,   1400   },         /* Gain = 3.531250 Exposure Index = 275 */
         {    932,   1400   },         /* Gain = 3.640625 Exposure Index = 276 */
         {    960,   1400   },         /* Gain = 3.750000 Exposure Index = 277 */
         {    989,   1400   },         /* Gain = 3.863281 Exposure Index = 278 */
         {   1019,   1400   },         /* Gain = 3.980469 Exposure Index = 279 */
         {   1050,   1400   },         /* Gain = 4.101562 Exposure Index = 280 */
         {   1082,   1400   },         /* Gain = 4.226562 Exposure Index = 281 */
         {   1115,   1400   },         /* Gain = 4.355469 Exposure Index = 282 */
         {   1149,   1400   },         /* Gain = 4.488281 Exposure Index = 283 */
         {   1184,   1400   },         /* Gain = 4.625000 Exposure Index = 284 */
         {   1220,   1400   },         /* Gain = 4.765625 Exposure Index = 285 */
         {   1257,   1400   },         /* Gain = 4.910156 Exposure Index = 286 */
         {   1295,   1400   },         /* Gain = 5.058594 Exposure Index = 287 */
         {   1334,   1400   },         /* Gain = 5.210938 Exposure Index = 288 */
         {   1375,   1400   },         /* Gain = 5.371094 Exposure Index = 289 */
         {   1417,   1400   },         /* Gain = 5.535156 Exposure Index = 290 */
         {   1460,   1400   },         /* Gain = 5.703125 Exposure Index = 291 */
         {   1504,   1400   },         /* Gain = 5.875000 Exposure Index = 292 */
         {   1550,   1400   },         /* Gain = 6.054688 Exposure Index = 293 */
         {   1597,   1400   },         /* Gain = 6.238281 Exposure Index = 294 */
         {   1645,   1400   },         /* Gain = 6.425781 Exposure Index = 295 */
         {   1695,   1400   },         /* Gain = 6.621094 Exposure Index = 296 */
         {   1746,   1400   },         /* Gain = 6.820312 Exposure Index = 297 */
         {   1799,   1400   },         /* Gain = 7.027344 Exposure Index = 298 */
         {   1853,   1400   },         /* Gain = 7.238281 Exposure Index = 299 */
         {   1909,   1400   },         /* Gain = 7.457031 Exposure Index = 300 */
         {   1967,   1400   },         /* Gain = 7.683594 Exposure Index = 301 */
         {   2027,   1400   },         /* Gain = 7.917969 Exposure Index = 302 */
         {   2088,   1400   },         /* Gain = 8.156250 Exposure Index = 303 */
         {   2151,   1400   },         /* Gain = 8.402344 Exposure Index = 304 */
         {   2216,   1400   },         /* Gain = 8.656250 Exposure Index = 305 */
         {   2283,   1400   },         /* Gain = 8.917969 Exposure Index = 306 */
         {   2352,   1400   },         /* Gain = 9.187500 Exposure Index = 307 */
         {   2423,   1400   },         /* Gain = 9.464844 Exposure Index = 308 */
         {   2496,   1400   },         /* Gain = 9.750000 Exposure Index = 309 */
         {   2571,   1400   },         /* Gain = 10.042969 Exposure Index = 310 */
         {   2649,   1400   },         /* Gain = 10.347656 Exposure Index = 311 */
         {   2729,   1400   },         /* Gain = 10.660156 Exposure Index = 312 */
         {   2811,   1400   },         /* Gain = 10.980469 Exposure Index = 313 */
         {   2896,   1400   },         /* Gain = 11.312500 Exposure Index = 314 */
         {   2983,   1400   },         /* Gain = 11.652344 Exposure Index = 315 */
         {   3073,   1400   },         /* Gain = 12.003906 Exposure Index = 316 */
         {   3166,   1400   },         /* Gain = 12.367188 Exposure Index = 317 */
         {   3261,   1400   },         /* Gain = 12.738281 Exposure Index = 318 */
         {   3359,   1400   },         /* Gain = 13.121094 Exposure Index = 319 */
         {   3460,   1400   },         /* Gain = 13.515625 Exposure Index = 320 */
         {   3564,   1400   },         /* Gain = 13.921875 Exposure Index = 321 */
         {   3671,   1400   },         /* Gain = 14.339844 Exposure Index = 322 */
         {   3782,   1400   },         /* Gain = 14.773438 Exposure Index = 323 */
         {   3896,   1400   },         /* Gain = 15.218750 Exposure Index = 324 */
         {   4013,   1400   },         /* Gain = 15.675781 Exposure Index = 325 */
         {   4134,   1400   },         /* Gain = 16.148438 Exposure Index = 326 */
         {   4259,   1400   },         /* Gain = 16.636719 Exposure Index = 327 */
         {   4387,   1400   },         /* Gain = 17.136719 Exposure Index = 328 */
         {   4519,   1400   },         /* Gain = 17.652344 Exposure Index = 329 */
         {   4655,   1400   },         /* Gain = 18.183594 Exposure Index = 330 */
         {   4795,   1400   },         /* Gain = 18.730469 Exposure Index = 331 */
         {   4939,   1400   },         /* Gain = 19.292969 Exposure Index = 332 */
         {   5088,   1400   },         /* Gain = 19.875000 Exposure Index = 333 */
         {   5241,   1400   },         /* Gain = 20.472656 Exposure Index = 334 */
         {   5399,   1400   },         /* Gain = 21.089844 Exposure Index = 335 */
         {   5561,   1400   },         /* Gain = 21.722656 Exposure Index = 336 */
         {   5728,   1400   },         /* Gain = 22.375000 Exposure Index = 337 */
         {   5900,   1400   },         /* Gain = 23.046875 Exposure Index = 338 */
         {   6077,   1400   },         /* Gain = 23.738281 Exposure Index = 339 */
         {   6260,   1400   },         /* Gain = 24.453125 Exposure Index = 340 */
         {   6448,   1400   },         /* Gain = 25.187500 Exposure Index = 341 */
         {   6642,   1400   },         /* Gain = 25.945312 Exposure Index = 342 */
         {   6842,   1400   },         /* Gain = 26.726562 Exposure Index = 343 */
         {   7048,   1400   },         /* Gain = 27.531250 Exposure Index = 344 */
         {   7260,   1400   },         /* Gain = 28.359375 Exposure Index = 345 */
         {   7478,   1400   },         /* Gain = 29.210938 Exposure Index = 346 */
         {   7703,   1400   },         /* Gain = 30.089844 Exposure Index = 347 */
         {   7935,   1400   },         /* Gain = 30.996094 Exposure Index = 348 */
         {   8174,   1400   },         /* Gain = 31.929688 Exposure Index = 349 */
         {   8420,   1400   },         /* Gain = 32.890625 Exposure Index = 350 */
         {   8673,   1400   },         /* Gain = 33.878906 Exposure Index = 351 */
         {   8934,   1400   },         /* Gain = 34.898438 Exposure Index = 352 */
         {   9203,   1400   },         /* Gain = 35.949219 Exposure Index = 353 */
         {   9480,   1400   },         /* Gain = 37.031250 Exposure Index = 354 */
         {   9765,   1400   },         /* Gain = 38.144531 Exposure Index = 355 */
         {  10058,   1400   },         /* Gain = 39.289062 Exposure Index = 356 */
         {  10360,   1400   },         /* Gain = 40.468750 Exposure Index = 357 */
         {  10671,   1400   },         /* Gain = 41.683594 Exposure Index = 358 */
         {  10992,   1400   },         /* Gain = 42.937500 Exposure Index = 359 */
         {  11322,   1400   },         /* Gain = 44.226562 Exposure Index = 360 */
         {  11662,   1400   },         /* Gain = 45.554688 Exposure Index = 361 */
         {  12012,   1400   },         /* Gain = 46.921875 Exposure Index = 362 */
         {  12373,   1400   },         /* Gain = 48.332031 Exposure Index = 363 */
         {  12745,   1400   },         /* Gain = 49.785156 Exposure Index = 364 */
         {  13128,   1400   },         /* Gain = 51.281250 Exposure Index = 365 */
         {  13522,   1400   },         /* Gain = 52.820312 Exposure Index = 366 */
         {  13928,   1400   },         /* Gain = 54.406250 Exposure Index = 367 */
         {  14346,   1400   },         /* Gain = 56.039062 Exposure Index = 368 */
         {  14777,   1400   },         /* Gain = 57.722656 Exposure Index = 369 */
         {  15221,   1400   },         /* Gain = 59.457031 Exposure Index = 370 */
         {  15678,   1400   },         /* Gain = 61.242188 Exposure Index = 371 */
         {  16149,   1400   },         /* Gain = 63.082031 Exposure Index = 372 */
         {  16634,   1400   },         /* Gain = 64.976562 Exposure Index = 373 */
         {  17134,   1400   },         /* Gain = 66.929688 Exposure Index = 374 */
         {  17649,   1400   },         /* Gain = 68.941406 Exposure Index = 375 */
         {  18179,   1400   },         /* Gain = 71.011719 Exposure Index = 376 */
         {  18725,   1400   },         /* Gain = 73.144531 Exposure Index = 377 */
         {  19287,   1400   },         /* Gain = 75.339844 Exposure Index = 378 */
         {  19866,   1400   },         /* Gain = 77.601562 Exposure Index = 379 */
         {  20462,   1400   },         /* Gain = 79.929688 Exposure Index = 380 */
         {  21076,   1400   },         /* Gain = 82.328125 Exposure Index = 381 */
         {  21709,   1400   },         /* Gain = 84.800781 Exposure Index = 382 */
         {  22361,   1400   },         /* Gain = 87.347656 Exposure Index = 383 */
         {  23032,   1400   },         /* Gain = 89.968750 Exposure Index = 384 */
         {  23723,   1400   },         /* Gain = 92.667969 Exposure Index = 385 */
         {  24435,   1400   },         /* Gain = 95.449219 Exposure Index = 386 */
         {  25169,   1400   },         /* Gain = 98.316406 Exposure Index = 387 */
         {  25925,   1400   },         /* Gain = 101.269531 Exposure Index = 388 */
         {  26703,   1400   },         /* Gain = 104.308594 Exposure Index = 389 */
         {  27505,   1400   },         /* Gain = 107.441406 Exposure Index = 390 */
         {  28331,   1400   },         /* Gain = 110.667969 Exposure Index = 391 */
         {  29181,   1400   },         /* Gain = 113.988281 Exposure Index = 392 */
         {  30057,   1400   },         /* Gain = 117.410156 Exposure Index = 393 */
         {  30959,   1400   },         /* Gain = 120.933594 Exposure Index = 394 */
         {  31888,   1400   },         /* Gain = 124.562500 Exposure Index = 395 */
         {  32845,   1400   },         /* Gain = 128.300781 Exposure Index = 396 */
         {  33831,   1400   },         /* Gain = 132.152344 Exposure Index = 397 */
         {  34846,   1400   },         /* Gain = 136.117188 Exposure Index = 398 */
         {  35892,   1400   },         /* Gain = 140.203125 Exposure Index = 399 */
         {  36969,   1400   },         /* Gain = 144.410156 Exposure Index = 400 */
         {  38079,   1400   },         /* Gain = 148.746094 Exposure Index = 401 */
         {  39222,   1400   },         /* Gain = 153.210938 Exposure Index = 402 */
         {  40399,   1400   },         /* Gain = 157.808594 Exposure Index = 403 */
         {  41611,   1400   },         /* Gain = 162.542969 Exposure Index = 404 */
         {  42860,   1400   },         /* Gain = 167.421875 Exposure Index = 405 */
         {  44146,   1400   },         /* Gain = 172.445312 Exposure Index = 406 */
         {  45471,   1400   },         /* Gain = 177.621094 Exposure Index = 407 */
         {  46836,   1400   },         /* Gain = 182.953125 Exposure Index = 408 */
         {  48242,   1400   },         /* Gain = 188.445312 Exposure Index = 409 */
         {  49690,   1400   },         /* Gain = 194.101562 Exposure Index = 410 */
         {  51181,   1400   },         /* Gain = 199.925781 Exposure Index = 411 */
         {  52717,   1400   },         /* Gain = 205.925781 Exposure Index = 412 */
         {  54299,   1400   },         /* Gain = 212.105469 Exposure Index = 413 */
         {  55928,   1400   },         /* Gain = 218.468750 Exposure Index = 414 */
         {  57606,   1400   },         /* Gain = 225.023438 Exposure Index = 415 */
         {  59335,   1400   },         /* Gain = 231.777344 Exposure Index = 416 */
         {  61116,   1400   },         /* Gain = 238.734375 Exposure Index = 417 */
         {  62950,   1400   },         /* Gain = 245.898438 Exposure Index = 418 */
         {  64839,   1400   },         /* Gain = 253.277344 Exposure Index = 419 */
         {  66785,   1400   },         /* Gain = 260.878906 Exposure Index = 420 */
         {  68789,   1400   },         /* Gain = 268.707031 Exposure Index = 421 */
         {  70853,   1400   },         /* Gain = 276.769531 Exposure Index = 422 */
         {  72979,   1400   },         /* Gain = 285.074219 Exposure Index = 423 */
         {  75169,   1400   },         /* Gain = 293.628906 Exposure Index = 424 */
         {  77425,   1400   },         /* Gain = 302.441406 Exposure Index = 425 */
         {  79748,   1400   },         /* Gain = 311.515625 Exposure Index = 426 */
         {  82141,   1400   },         /* Gain = 320.863281 Exposure Index = 427 */
         {  84606,   1400   },         /* Gain = 330.492188 Exposure Index = 428 */
         {  87145,   1400   },         /* Gain = 340.410156 Exposure Index = 429 */
         {  89760,   1400   },         /* Gain = 350.625000 Exposure Index = 430 */
         {  92453,   1400   },         /* Gain = 361.144531 Exposure Index = 431 */
         {  95227,   1400   },         /* Gain = 371.980469 Exposure Index = 432 */
         {  98084,   1400   },         /* Gain = 383.140625 Exposure Index = 433 */
         { 101027,   1400   },         /* Gain = 394.636719 Exposure Index = 434 */
         { 104058,   1400   },         /* Gain = 406.476562 Exposure Index = 435 */
         { 107180,   1400   },         /* Gain = 418.671875 Exposure Index = 436 */
         { 110396,   1400   },         /* Gain = 431.234375 Exposure Index = 437 */
         { 113708,   1400   },         /* Gain = 444.171875 Exposure Index = 438 */
         { 117120,   1400   },         /* Gain = 457.500000 Exposure Index = 439 */
         { 120634,   1400   },         /* Gain = 471.226562 Exposure Index = 440 */
         { 124254,   1400   },         /* Gain = 485.367188 Exposure Index = 441 */
         { 127982,   1400   },         /* Gain = 499.929688 Exposure Index = 442 */
      },
      /* enable_short_exp_table */
      1,
      /* short_exp_table */
      {
      //1,400
      //50,400
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
         {    261,    400   },         /* Gain = 1.019531 Exposure Index = 193 */
         {    269,    400   },         /* Gain = 1.050781 Exposure Index = 194 */
         {    278,    400   },         /* Gain = 1.085938 Exposure Index = 195 */
         {    287,    400   },         /* Gain = 1.121094 Exposure Index = 196 */
         {    296,    400   },         /* Gain = 1.156250 Exposure Index = 197 */
         {    305,    400   },         /* Gain = 1.191406 Exposure Index = 198 */
         {    315,    400   },         /* Gain = 1.230469 Exposure Index = 199 */
         {    325,    400   },         /* Gain = 1.269531 Exposure Index = 200 */
         {    335,    400   },         /* Gain = 1.308594 Exposure Index = 201 */
         {    346,    400   },         /* Gain = 1.351562 Exposure Index = 202 */
         {    357,    400   },         /* Gain = 1.394531 Exposure Index = 203 */
         {    368,    400   },         /* Gain = 1.437500 Exposure Index = 204 */
         {    380,    400   },         /* Gain = 1.484375 Exposure Index = 205 */
         {    392,    400   },         /* Gain = 1.531250 Exposure Index = 206 */
         {    404,    400   },         /* Gain = 1.578125 Exposure Index = 207 */
         {    417,    400   },         /* Gain = 1.628906 Exposure Index = 208 */
         {    430,    400   },         /* Gain = 1.679688 Exposure Index = 209 */
         {    443,    400   },         /* Gain = 1.730469 Exposure Index = 210 */
         {    457,    400   },         /* Gain = 1.785156 Exposure Index = 211 */
         {    471,    400   },         /* Gain = 1.839844 Exposure Index = 212 */
         {    486,    400   },         /* Gain = 1.898438 Exposure Index = 213 */
         {    501,    400   },         /* Gain = 1.957031 Exposure Index = 214 */
         {    517,    400   },         /* Gain = 2.019531 Exposure Index = 215 */
         {    533,    400   },         /* Gain = 2.082031 Exposure Index = 216 */
         {    549,    400   },         /* Gain = 2.144531 Exposure Index = 217 */
         {    566,    400   },         /* Gain = 2.210938 Exposure Index = 218 */
         {    583,    400   },         /* Gain = 2.277344 Exposure Index = 219 */
         {    601,    400   },         /* Gain = 2.347656 Exposure Index = 220 */
         {    620,    400   },         /* Gain = 2.421875 Exposure Index = 221 */
         {    639,    400   },         /* Gain = 2.496094 Exposure Index = 222 */
         {    659,    400   },         /* Gain = 2.574219 Exposure Index = 223 */
         {    679,    400   },         /* Gain = 2.652344 Exposure Index = 224 */
         {    700,    400   },         /* Gain = 2.734375 Exposure Index = 225 */
         {    721,    400   },         /* Gain = 2.816406 Exposure Index = 226 */
         {    743,    400   },         /* Gain = 2.902344 Exposure Index = 227 */
         {    766,    400   },         /* Gain = 2.992188 Exposure Index = 228 */
         {    789,    400   },         /* Gain = 3.082031 Exposure Index = 229 */
         {    813,    400   },         /* Gain = 3.175781 Exposure Index = 230 */
         {    838,    400   },         /* Gain = 3.273438 Exposure Index = 231 */
         {    864,    400   },         /* Gain = 3.375000 Exposure Index = 232 */
         {    890,    400   },         /* Gain = 3.476562 Exposure Index = 233 */
         {    917,    400   },         /* Gain = 3.582031 Exposure Index = 234 */
         {    945,    400   },         /* Gain = 3.691406 Exposure Index = 235 */
         {    974,    400   },         /* Gain = 3.804688 Exposure Index = 236 */
         {   1004,    400   },         /* Gain = 3.921875 Exposure Index = 237 */
         {   1035,    400   },         /* Gain = 4.042969 Exposure Index = 238 */
         {   1067,    400   },         /* Gain = 4.167969 Exposure Index = 239 */
         {   1100,    400   },         /* Gain = 4.296875 Exposure Index = 240 */
         {   1133,    400   },         /* Gain = 4.425781 Exposure Index = 241 */
         {   1167,    400   },         /* Gain = 4.558594 Exposure Index = 242 */
         {   1203,    400   },         /* Gain = 4.699219 Exposure Index = 243 */
         {   1240,    400   },         /* Gain = 4.843750 Exposure Index = 244 */
         {   1278,    400   },         /* Gain = 4.992188 Exposure Index = 245 */
         {   1317,    400   },         /* Gain = 5.144531 Exposure Index = 246 */
         {   1357,    400   },         /* Gain = 5.300781 Exposure Index = 247 */
         {   1398,    400   },         /* Gain = 5.460938 Exposure Index = 248 */
         {   1440,    400   },         /* Gain = 5.625000 Exposure Index = 249 */
         {   1484,    400   },         /* Gain = 5.796875 Exposure Index = 250 */
         {   1529,    400   },         /* Gain = 5.972656 Exposure Index = 251 */
         {   1575,    400   },         /* Gain = 6.152344 Exposure Index = 252 */
         {   1623,    400   },         /* Gain = 6.339844 Exposure Index = 253 */
         {   1672,    400   },         /* Gain = 6.531250 Exposure Index = 254 */
         {   1723,    400   },         /* Gain = 6.730469 Exposure Index = 255 */
         {   1775,    400   },         /* Gain = 6.933594 Exposure Index = 256 */
         {   1829,    400   },         /* Gain = 7.144531 Exposure Index = 257 */
         {   1884,    400   },         /* Gain = 7.359375 Exposure Index = 258 */
         {   1941,    400   },         /* Gain = 7.582031 Exposure Index = 259 */
         {   2000,    400   },         /* Gain = 7.812500 Exposure Index = 260 */
         {   2060,    400   },         /* Gain = 8.046875 Exposure Index = 261 */
         {   2122,    400   },         /* Gain = 8.289062 Exposure Index = 262 */
         {   2186,    400   },         /* Gain = 8.539062 Exposure Index = 263 */
         {   2252,    400   },         /* Gain = 8.796875 Exposure Index = 264 */
         {   2320,    400   },         /* Gain = 9.062500 Exposure Index = 265 */
         {   2390,    400   },         /* Gain = 9.335938 Exposure Index = 266 */
         {   2462,    400   },         /* Gain = 9.617188 Exposure Index = 267 */
         {   2536,    400   },         /* Gain = 9.906250 Exposure Index = 268 */
         {   2613,    400   },         /* Gain = 10.207031 Exposure Index = 269 */
         {   2692,    400   },         /* Gain = 10.515625 Exposure Index = 270 */
         {   2773,    400   },         /* Gain = 10.832031 Exposure Index = 271 */
         {   2857,    400   },         /* Gain = 11.160156 Exposure Index = 272 */
         {   2943,    400   },         /* Gain = 11.496094 Exposure Index = 273 */
         {   3032,    400   },         /* Gain = 11.843750 Exposure Index = 274 */
         {   3123,    400   },         /* Gain = 12.199219 Exposure Index = 275 */
         {   3217,    400   },         /* Gain = 12.566406 Exposure Index = 276 */
         {   3314,    400   },         /* Gain = 12.945312 Exposure Index = 277 */
         {   3414,    400   },         /* Gain = 13.335938 Exposure Index = 278 */
         {   3517,    400   },         /* Gain = 13.738281 Exposure Index = 279 */
         {   3623,    400   },         /* Gain = 14.152344 Exposure Index = 280 */
         {   3732,    400   },         /* Gain = 14.578125 Exposure Index = 281 */
         {   3844,    400   },         /* Gain = 15.015625 Exposure Index = 282 */
         {   3960,    400   },         /* Gain = 15.468750 Exposure Index = 283 */
         {   4079,    400   },         /* Gain = 15.933594 Exposure Index = 284 */
         {   4202,    400   },         /* Gain = 16.414062 Exposure Index = 285 */
         {   4329,    400   },         /* Gain = 16.910156 Exposure Index = 286 */
         {   4459,    400   },         /* Gain = 17.417969 Exposure Index = 287 */
         {   4593,    400   },         /* Gain = 17.941406 Exposure Index = 288 */
         {   4731,    400   },         /* Gain = 18.480469 Exposure Index = 289 */
         {   4873,    400   },         /* Gain = 19.035156 Exposure Index = 290 */
         {   5020,    400   },         /* Gain = 19.609375 Exposure Index = 291 */
         {   5171,    400   },         /* Gain = 20.199219 Exposure Index = 292 */
         {   5327,    400   },         /* Gain = 20.808594 Exposure Index = 293 */
         {   5487,    400   },         /* Gain = 21.433594 Exposure Index = 294 */
         {   5652,    400   },         /* Gain = 22.078125 Exposure Index = 295 */
         {   5822,    400   },         /* Gain = 22.742188 Exposure Index = 296 */
         {   5997,    400   },         /* Gain = 23.425781 Exposure Index = 297 */
         {   6177,    400   },         /* Gain = 24.128906 Exposure Index = 298 */
         {   6363,    400   },         /* Gain = 24.855469 Exposure Index = 299 */
         {   6554,    400   },         /* Gain = 25.601562 Exposure Index = 300 */
         {   6751,    400   },         /* Gain = 26.371094 Exposure Index = 301 */
         {   6954,    400   },         /* Gain = 27.164062 Exposure Index = 302 */
         {   7163,    400   },         /* Gain = 27.980469 Exposure Index = 303 */
         {   7378,    400   },         /* Gain = 28.820312 Exposure Index = 304 */
         {   7600,    400   },         /* Gain = 29.687500 Exposure Index = 305 */
         {   7828,    400   },         /* Gain = 30.578125 Exposure Index = 306 */
         {   8063,    400   },         /* Gain = 31.496094 Exposure Index = 307 */
         {   8305,    400   },         /* Gain = 32.441406 Exposure Index = 308 */
         {   8555,    400   },         /* Gain = 33.417969 Exposure Index = 309 */
         {   8812,    400   },         /* Gain = 34.421875 Exposure Index = 310 */
         {   9077,    400   },         /* Gain = 35.457031 Exposure Index = 311 */
         {   9350,    400   },         /* Gain = 36.523438 Exposure Index = 312 */
         {   9631,    400   },         /* Gain = 37.621094 Exposure Index = 313 */
         {   9920,    400   },         /* Gain = 38.750000 Exposure Index = 314 */
         {  10218,    400   },         /* Gain = 39.914062 Exposure Index = 315 */
         {  10525,    400   },         /* Gain = 41.113281 Exposure Index = 316 */
         {  10841,    400   },         /* Gain = 42.347656 Exposure Index = 317 */
         {  11167,    400   },         /* Gain = 43.621094 Exposure Index = 318 */
         {  11503,    400   },         /* Gain = 44.933594 Exposure Index = 319 */
         {  11849,    400   },         /* Gain = 46.285156 Exposure Index = 320 */
         {  12205,    400   },         /* Gain = 47.675781 Exposure Index = 321 */
      },
   },
   /* ae_tuning_par para */
   {
      1,/* aec_stats_type; 0:BAYER_AEC; 1:HYBRID_AEC */
      0,/* force_exp_forced */
      0.1,/* force_exp_value */
      1,/* preview_iso_enable */
      {
         75,/* outdoor_luma_target_compensated */
         75,/* default_luma_target_compensated */
         40,/* lowlight_luma_target */
         120,/* outdoor_index */
         240,/* indoor_index */
         350,/* lowlight_start_idx */
         420,/* lowlight_end_idx */
         3,/* luma_target_short */
         1,/* enable_luma _target_lut */
         8,/* count */
         /* luma target lut */
         {
            {50,70},
            {140,70},
            {190,70},
            {230,70},
            {270,62},
            {290,53},
            {330,44},
            {400,40},
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
         321,
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
            340,//use_lux_index
            400,//low_lux_index_hdr
            340,//high_lux_index_hdr
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
         1,
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
         0,/* face_luma_target */
         0,/* face_weight */
         0,/* face_skip_count */
         0,/* filter_weight */
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
         0,/* face_luma_high */
         0,/* face_luma_low */
         0,/* enable_w_dec */
         0,/* enable_region_percent */
         0,/* region_percent_low_limit */
         0,/* region_percent_low */
         0,/* region_percent_high */
         0,/* region_percent_high_limit */
         0,/* region_percent_low_limit_w */
         0,/* region_percent_low_w */
         0,/* region_percent_high_w */
         0,/* region_percent_high_limit_w */
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
         4,//bright_dark_count
         1,//bright_en
         1,//dark_en
         1,//inter_en
         /* bright_dark_pra_t bright_dark_pra[MIDDLE_TRIGGER_COUNT] */
         {
            {
               0,//lux_index
               5,//dark_th_low
               8,//dark_th_high
               230,//bright_th_low
               250,//bright_th_high
               2,//dark_th_low_w
               1.5,//dark_th_high_w
               1,//bright_th_low_w
               1,//bright_th_high_w
            },
            {
               100,//lux_index
               4,//dark_th_low
               8,//dark_th_high
               210,//bright_th_low
               250,//bright_th_high
               2,//dark_th_low_w
               1,//dark_th_high_w
               1,//bright_th_low_w
               1,//bright_th_high_w
            },
            {
               200,//lux_index
               4,//dark_th_low
               8,//dark_th_high
               210,//bright_th_low
               255,//bright_th_high
               2,//dark_th_low_w
               1,//dark_th_high_w
               1,//bright_th_low_w
               1,//bright_th_high_w
            },
            {
               300,//lux_index
               4,//dark_th_low
               8,//dark_th_high
               230,//bright_th_low
               245,//bright_th_high
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
         3,//frame_skip
         2,//ddr_frame_skip_offset
         {5,10,15},//interval_threshold[3]
         {0.65,0.5,0.4},//speed_ratio[3]
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
               200,//lux_index
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
      { 0.4299, 0.7439 },   /* d75 */
      { 0.4584, 0.6637 },   /* d65 */
      { 0.5000, 0.5600 },   /* d50 */
      { 0.5000, 0.5600 },   /* noon */
      { 0.5576, 0.4089 },   /* cw */
      { 0.6920, 0.4119 },   /* tl84 */
      { 0.7673, 0.3319 },   /* a */
      { 0.9240, 0.2230 },   /* h */
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
      { 0, 0, 0, 0 },   /* d75 */
      { 2, 2, 2, 1 },   /* d65 */
      { 4, 4, 2, 1 },   /* d50 */
      { 4, 4, 2, 1 },   /* noon */
      { 1, 1, 1, 1 },   /* cw */
      { 1, 1, 1, 1 },   /* tl84 */
      { 0, 0, 1, 1 },   /* a */
      { 0, 0, 1, 5 },   /* h */
      { 0, 0, 0, 0 },   /* custom1 */
      { 0, 0, 0, 0 },   /* custom2 */
   },
   /* awb_lowlight_lut_t awb_lowlight_lut[AWB_LOWLIGHT_LUT_COUNT] */
   {
      /* awb_lowlight_lut 0 */
      {
         282,//lux index
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
      120,   /* outdoor_index */
      190,   /* inoutdoor_midpoint */
      280,   /* indoor_index */
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
                  0.0036,0.0036,0.0048,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,
                  0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,0.0064,
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
      9,//luma_count
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
               0,
               0,
               0,
               0,
               1,
               2,
               3,
               4,
               5,
               4,
               3,
               2,
               2,
               2,
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
               0,
               0,
               0,
               0,
               1,
               2,
               3,
               4,
               5,
               4,
               3,
               2,
               2,
               2,
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
               0,
               0,
               0,
               0,
               2,
               2,
               3,
               4,
               5,
               4,
               3,
               2,
               2,
               2,
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
            150,//exp_index
            /* hybrid_decision_w */
            {
               0,
               0,
               0,
               0,
               2,
               2,
               2,
               2,
               5,
               4,
               3,
               3,
               3,
               3,
               2,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               0.5,
               0.5,
               0.5,
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
               1,
               1,
               1,
               1,
               2,
               2,
               2,
               2,
               5,
               3,
               3,
               3,
               3,
               3,
               2,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               0.5,
               0.5,
               0.5,
               0.5,
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
               1,
               1,
               1,
               1,
               2,
               2,
               2,
               2,
               2,
               3,
               3,
               3,
               3,
               3,
               2,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               0.5,
               0.5,
               0.5,
               0.5,
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
               2,
               2,
               2,
               2,
               2,
               2,
               2,
               2,
               2,
               2,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               0.5,
               0.5,
               0.5,
               0.5,
               0.5,
               0.5,
               0.5,
               0.5,
               0.5,
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
            400,//exp_index
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
               2,
               2,
               2,
               2,
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
      0.15,//keep_th_no_grey_to_grey
      0.05,//keep_th_grey_to_no_grey
      1,
      4,
      4,
   },
   0,//enable_gain_adjust
   0,//enable_lowlight_lut
   1,//lowlight_lut_count
   0,//force_simple_grey_word
   0,//enable_init_point
   0,//use_init_point_count
   /* init point */
   {
      0,//rg
      0,//bg
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
             4,//misleading_region_count
             /* awb mesh reigon */
             {
                /* awb mesh region 0 */
                {
                    4,//point_num
                    0,//is_square
                    0.742,//min_x
                    0.3,//min_y
                    0.952,//max_x
                    0.441,//max_y
                    /* awb mesh point */
                    {
                       {0.742,0.376},
                       {0.833,0.441},
                       {0.952,0.386},
                       {0.87,0.3},
                    },
                },
                /* awb mesh region 1 */
                {
                    4,//point_num
                    0,//is_square
                    0.6,//min_x
                    0.42,//min_y
                    0.75,//max_x
                    0.53,//max_y
                    /* awb mesh point */
                    {
                       {0.6,0.5},
                       {0.7,0.53},
                       {0.75,0.45},
                       {0.7,0.42},
                    },
                },
                /* awb mesh region 2 */
                {
                    4,//point_num
                    0,//is_square
                    0.743,//min_x
                    0.277,//min_y
                    1.0233,//max_x
                    0.413,//max_y
                    /* awb mesh point */
                    {
                       {0.743,0.368},
                       {0.839,0.413},
                       {1.0233,0.323},
                       {0.929,0.277},
                    },
                },
                /* awb mesh region 3 */
                {
                    4,//point_num
                    0,//is_square
                    0.719,//min_x
                    0.308,//min_y
                    0.889,//max_x
                    0.404,//max_y
                    /* awb mesh point */
                    {
                       {0.719,0.371},
                       {0.764,0.404},
                       {0.889,0.345},
                       {0.8435,0.308},
                    },
                },
             },
         },
         /* misleading region 1 */
         {
             140,//lux_index
             4,//misleading_region_count
             /* awb mesh reigon */
             {
                /* awb mesh region 0 */
                {
                    4,//point_num
                    0,//is_square
                    0.742,//min_x
                    0.3,//min_y
                    0.952,//max_x
                    0.441,//max_y
                    /* awb mesh point */
                    {
                       {0.742,0.376},
                       {0.833,0.441},
                       {0.952,0.386},
                       {0.87,0.3},
                    },
                },
                /* awb mesh region 1 */
                {
                    4,//point_num
                    0,//is_square
                    0.6,//min_x
                    0.42,//min_y
                    0.75,//max_x
                    0.53,//max_y
                    /* awb mesh point */
                    {
                       {0.6,0.5},
                       {0.7,0.53},
                       {0.75,0.45},
                       {0.7,0.42},
                    },
                },
                /* awb mesh region 2 */
                {
                    4,//point_num
                    0,//is_square
                    0.743,//min_x
                    0.277,//min_y
                    1.0233,//max_x
                    0.413,//max_y
                    /* awb mesh point */
                    {
                       {0.743,0.368},
                       {0.839,0.413},
                       {1.0233,0.323},
                       {0.929,0.277},
                    },
                },
                /* awb mesh region 3 */
                {
                    4,//point_num
                    0,//is_square
                    0.719,//min_x
                    0.308,//min_y
                    0.889,//max_x
                    0.404,//max_y
                    /* awb mesh point */
                    {
                       {0.719,0.371},
                       {0.764,0.404},
                       {0.889,0.345},
                       {0.8435,0.308},
                    },
                },
             },
         },
         /* misleading region 2 */
         {
             240,//lux_index
             3,//misleading_region_count
             /* awb mesh reigon */
             {
                /* awb mesh region 0 */
                {
                    4,//point_num
                    0,//is_square
                    0.742,//min_x
                    0.3,//min_y
                    0.952,//max_x
                    0.441,//max_y
                    /* awb mesh point */
                    {
                       {0.742,0.376},
                       {0.833,0.441},
                       {0.952,0.386},
                       {0.87,0.3},
                    },
                },
                /* awb mesh region 1 */
                {
                    4,//point_num
                    0,//is_square
                    0.6,//min_x
                    0.42,//min_y
                    0.75,//max_x
                    0.53,//max_y
                    /* awb mesh point */
                    {
                       {0.6,0.5},
                       {0.7,0.53},
                       {0.75,0.45},
                       {0.7,0.42},
                    },
                },
                /* awb mesh region 2 */
                {
                    4,//point_num
                    0,//is_square
                    0.719,//min_x
                    0.308,//min_y
                    0.889,//max_x
                    0.404,//max_y
                    /* awb mesh point */
                    {
                       {0.719,0.371},
                       {0.764,0.404},
                       {0.889,0.345},
                       {0.8435,0.308},
                    },
                },
             },
         },
         /* misleading region 3 */
         {
             350,//lux_index
             1,//misleading_region_count
             /* awb mesh reigon */
             {
                /* awb mesh region 0 */
                {
                    4,//point_num
                    0,//is_square
                    0.742,//min_x
                    0.3,//min_y
                    0.952,//max_x
                    0.441,//max_y
                    /* awb mesh point */
                    {
                       {0.742,0.376},
                       {0.833,0.441},
                       {0.952,0.386},
                       {0.87,0.3},
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
   0,//enable_gain_adj_lux_wb
   /* awb_gain_adjust_lux_wb_t */
   {
      0,//count
      /* awb_gain_adjust_wb_t gain_adj[HIGH_TRIGGER_COUNT] */
      {
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
