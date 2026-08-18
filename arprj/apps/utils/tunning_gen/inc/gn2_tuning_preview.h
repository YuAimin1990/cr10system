
/*---------------header of imx307 tuning-------------------------*/

/* version of the tuning header uint32_t header_version */
0x20201105,
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
       1,
       /* interpolation_enable */
       1,
       /* count_ae */
       5,
       /* aec_trigger_mode_t trigger_mode : 0 gain trigger ,1 lux trigger */
       0,
       /* aec_trigger_t aec_trigger[MIDDLE_TRIGGER_COUNT] MIDDLE_TRIGGER_COUNT==5 */
       {
           /* aec trigger 0*/
           {
               1,
               32,
           },
           /* aec trigger 1*/
           {
               64,
               64,
           },
           /* aec trigger 2*/
           {
               256,
               256,
           },
           /* aec trigger 3*/
           {
               512,
               512,
           },
           /* aec trigger 4*/
           {
               1300,
               1500,
           },
       },
       /* isp_sub_module_blc_para para[MIDDLE_TRIGGER_COUNT] MIDDLE_TRIGGER_COUNT==5 */
       {
           /* para 0*/
           {
               801,//R
               800,//B
               801,//GR
               800,//GB
               270,//gian R
               270,//gain B
               270,//gain GR
               270,//gain GB
           },
           /* para 1*/
           {
               827,//R
               810,//B
               826,//GR
               814,//GB
               270,//gian R
               270,//gain B
               270,//gain GR
               270,//gain GB
           },
           /* para 2*/
           {
               1000,//R
               1000,//B
               948,//GR
               902,//GB
               272,//gian R
               271,//gain B
               272,//gain GR
               271,//gain GB
           },
           /* para 3*/
           {
               1400,//R
               1400,//B
               1183,//GR
               1115,//GB
               276,//gian R
               275,//gain B
               276,//gain GR
               275,//gain GB
           },
           /* para 4*/
           {
               4000,//R
               3000,//B
               1988,//GR
               1865,//GB
               294,//gian R
               290,//gain B
               292,//gain GR
               289,//gain GB
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
       0,
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             1,
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
              1,//hdr_motion_en_sm
              25,//hdr_alpha_s_th_sm
              867,//hdr_exp_value_th1_sm
              682,//hdr_th2_th1_reverse_sm
              16,//hdr_noise_sp_sm
              19,//hdr_motion_mp_sm
              1,//hdr_motion_en_sl
              25,//hdr_alpha_s_th_sl
              32,//hdr_exp_value_th1_sl
              68,//hdr_th2_th1_reverse_sl
              60,//hdr_noise_sp_sl
              12,//hdr_motion_mp_sl
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
              0,//enable_hdr_blc
              0,//mv_fix_th
              0,//mv_fix_value
              0,//mv_fix_en
              0,//use_long_exp_fix
              0,//use_mv_fix_value_fix
              0,//enable_sexp_denoise
              /* gaus_coef[4] */
              {
                 0,0,0,0,
              },
           },
       },
    },
    /* isp_sub_module_compander_tuning_t isp_sub_module_compander_tuning */
    {
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
       0,
       /* region enable */
       {
          1,
          1,
          1,
          1,
          1,
       },
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             1,
             8,
          },
          /* aec trigger 1 */
          {
             10,
             16,
          },
          /* aec trigger 2 */
          {
             20,
             60,
          },
          /* aec trigger 3 */
          {
             700,
             1500,
          },
          /* aec trigger 4 */
          {
             256,
             1500,
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
             0x00000040,0x00000000,0x000003e8,0x00000578,0x000000c8,0x000003e8,0x000000c8,0x00000320,0x000000c8,0x000000c8,
             0x000000c8,0x000003e8,0x000000c8,0x00000320,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
             0x00000000,0x00000020,0x00000060,0x000000a0,0x000000e0,
          },
          /* parameter 3 */
          {
             0x00000033,0x00000000,0x00000064,0x000001f4,0x000001f4,0x0000003c,0x00000190,0x00000320,0x00000001,0x000001f4,
             0x000004b0,0x0000003c,0x00000190,0x0000003c,0x00000099,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
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
       12,
       /* tigger_mode : 0 gain ,1 lux */
       0,
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             1,
             1.1,
          },
          /* aec trigger 1 */
          {
             2,
             3.5,
          },
          /* aec trigger 2 */
          {
             4,
             4.1,
          },
          /* aec trigger 3 */
          {
             8,
             8.1,
          },
          /* aec trigger 4 */
          {
             16,
             16.1,
          },
          /* aec trigger 5 */
          {
             32,
             32.1,
          },
          /* aec trigger 6 */
          {
             64,
             64.1,
          },
          /* aec trigger 7 */
          {
             128,
             128.1,
          },
          /* aec trigger 8 */
          {
             256,
             256.1,
          },
          /* aec trigger 9 */
          {
             512,
             512.1,
          },
          /* aec trigger 10 */
          {
             1500,
             1500.1,
          },
          /* aec trigger 11 */
          {
             2048,
             2048.1,
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
                       {30,30,30},
                       /* channel 2 */
                       {30,30,30},
                       /* channel 3 */
                       {30,30,30},
                       /* channel 4 */
                       {30,30,30},
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
                      {1,1,1,1,1,12,14,14,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {1,1,1,1,1,12,14,14,16},
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
                       {30,30,30},
                       /* channel 2 */
                       {30,30,30},
                       /* channel 3 */
                       {30,30,30},
                       /* channel 4 */
                       {30,30,30},
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
           /* para 2*/
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
                       {30,30,30},
                       /* channel 2 */
                       {30,30,30},
                       /* channel 3 */
                       {30,30,30},
                       /* channel 4 */
                       {30,30,30},
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
           /* para 3*/
           {
               1,//denoise_layer
               1,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
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
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {37,37,37},
                       /* channel 2 */
                       {37,37,37},
                       /* channel 3 */
                       {37,37,37},
                       /* channel 4 */
                       {37,37,37},
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
                      {6,10,10,10,10,12,14,14,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {6,10,10,10,10,12,14,14,16},
                   },
               },
           },
           /* para 4*/
           {
               2,//denoise_layer
               1,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
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
                       {3200,1598,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {3200,1598,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {3200,1598,600},
                       /* noise profile gain */
                       {16,13,10,6},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {3200,1598,600},
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
                      {12,12,12,10,10,12,14,16,16},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {12,12,12,10,10,12,14,16,16},
                   },
               },
           },
           /* para 5*/
           {
               2,//denoise_layer
               1,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
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
                      {16,16,14,12,10,8,6,4,2},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {13,26,40,60,90,120,160,200},
                      /* edge_Level */
                      {16,16,14,12,10,8,6,4,2},
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
                       {250,250,250},
                       /* channel 2 */
                       {250,250,250},
                       /* channel 3 */
                       {250,250,250},
                       /* channel 4 */
                       {250,250,250},
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
           /* para 7*/
           {
               2,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {100,100,100},
                       /* channel 2 */
                       {100,100,100},
                       /* channel 3 */
                       {100,100,100},
                       /* channel 4 */
                       {100,100,100},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {300,300,300},
                       /* channel 2 */
                       {300,300,300},
                       /* channel 3 */
                       {300,300,300},
                       /* channel 4 */
                       {300,300,300},
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
               2,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {150,150,150},
                       /* channel 2 */
                       {150,150,150},
                       /* channel 3 */
                       {150,150,150},
                       /* channel 4 */
                       {150,150,150},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {300,300,300},
                       /* channel 2 */
                       {300,300,300},
                       /* channel 3 */
                       {300,300,300},
                       /* channel 4 */
                       {300,300,300},
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
               2,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {150,150,150},
                       /* channel 2 */
                       {150,150,150},
                       /* channel 3 */
                       {150,150,150},
                       /* channel 4 */
                       {150,150,150},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {300,300,300},
                       /* channel 2 */
                       {300,300,300},
                       /* channel 3 */
                       {300,300,300},
                       /* channel 4 */
                       {300,300,300},
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
               2,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {300,300,300},
                       /* channel 2 */
                       {300,300,300},
                       /* channel 3 */
                       {300,300,300},
                       /* channel 4 */
                       {300,300,300},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {500,500,500},
                       /* channel 2 */
                       {500,500,500},
                       /* channel 3 */
                       {500,500,500},
                       /* channel 4 */
                       {500,500,500},
                   },
               },
               /* profile_t profile[RNR_CHANNEL_COUMT] */
               {
                   /* channel 1 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,16,16},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,16,16},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,16,16},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {3200,1600,600},
                       /* noise profile gain */
                       {16,13,16,16},
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
           /* para 11*/
           {
               2,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {1000,1000,1000},
                       /* channel 2 */
                       {1000,1000,1000},
                       /* channel 3 */
                       {1000,1000,1000},
                       /* channel 4 */
                       {1000,1000,1000},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {2000,2000,2000},
                       /* channel 2 */
                       {2000,2000,2000},
                       /* channel 3 */
                       {2000,2000,2000},
                       /* channel 4 */
                       {2000,2000,2000},
                   },
               },
               /* profile_t profile[RNR_CHANNEL_COUMT] */
               {
                   /* channel 1 */
                   {
                       /* noise profile */
                       {0,0,0},
                       /* noise profile gain */
                       {0,0,0,0},
                   },
                   /* channel 2 */
                   {
                       /* noise profile */
                       {0,0,0},
                       /* noise profile gain */
                       {0,0,0,0},
                   },
                   /* channel 3 */
                   {
                       /* noise profile */
                       {0,0,0},
                       /* noise profile gain */
                       {0,0,0,0},
                   },
                   /* channel 4 */
                   {
                       /* noise profile */
                       {0,0,0},
                       /* noise profile gain */
                       {0,0,0,0},
                   },
               },
               /* edge_t edge */
               {
                   /* channel 1 */
                   {
                      /* edge_V */
                      {0,0,0,0,0,0,0,0},
                      /* edge_Level */
                      {0,0,0,0,0,0,0,0,0},
                   },
                   /* channel 2 */
                   {
                      /* edge_V */
                      {0,0,0,0,0,0,0,0},
                      /* edge_Level */
                      {0,0,0,0,0,0,0,0,0},
                   },
               },
           },
       },
    },
    /* isp_sub_module_compander_tuning_t isp_sub_module_compander_tuning */
    {
    },
    /* isp_sub_module_lsc_tuning_t isp_sub_module_lsc_tuning */
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
                1,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                0,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.8660, 1.9515, 1.5694, 1.3496, 1.2478, 1.2341, 1.3557, 1.4759, 1.9502, 2.5039, 
                   2.5367, 1.8113, 1.4517, 1.2604, 1.1653, 1.1737, 1.2459, 1.4466, 1.7559, 2.4357, 
                   2.3511, 1.6856, 1.3603, 1.1893, 1.0902, 1.0777, 1.1852, 1.3449, 1.6656, 2.2402, 
                   2.2626, 1.6200, 1.3311, 1.1359, 1.0404, 1.0310, 1.1272, 1.3182, 1.6017, 2.1539, 
                   2.2388, 1.6196, 1.3150, 1.1433, 1.0029, 0.9929, 1.1331, 1.3115, 1.5822, 2.1573, 
                   2.2221, 1.5980, 1.3116, 1.1326, 1.0173, 1.0039, 1.1214, 1.3053, 1.5892, 2.1167, 
                   2.3773, 1.7063, 1.3904, 1.2125, 1.0975, 1.1026, 1.1856, 1.3802, 1.6604, 2.2565, 
                   2.4094, 1.7591, 1.4146, 1.2422, 1.1491, 1.1358, 1.2402, 1.4204, 1.7129, 2.3928, 
                   2.8310, 1.9795, 1.5738, 1.3845, 1.2700, 1.2791, 1.3543, 1.5630, 1.9086, 2.6342, 
                   3.0153, 2.1394, 1.6699, 1.4578, 1.3586, 1.3703, 1.4706, 1.6785, 2.0697, 2.9667, 
                   2.7382, 1.9591, 1.5349, 1.3479, 1.2657, 1.2220, 1.3582, 1.5108, 1.9317, 2.5955, 
                   2.4770, 1.7771, 1.4415, 1.2667, 1.1653, 1.1723, 1.2516, 1.4554, 1.7585, 2.4302, 
                   2.3042, 1.6824, 1.3556, 1.1960, 1.0889, 1.0841, 1.1796, 1.3510, 1.6678, 2.2411, 
                   2.1937, 1.6072, 1.3164, 1.1495, 1.0398, 1.0265, 1.1331, 1.3236, 1.6017, 2.1459, 
                   2.2369, 1.5985, 1.3283, 1.1436, 0.9956, 0.9964, 1.1233, 1.3226, 1.5776, 2.1289, 
                   2.1673, 1.5821, 1.3064, 1.1267, 1.0173, 1.0011, 1.1279, 1.3110, 1.5778, 2.1345, 
                   2.2998, 1.6987, 1.3705, 1.2063, 1.0893, 1.1057, 1.1752, 1.3832, 1.6648, 2.2487, 
                   2.4662, 1.7267, 1.4213, 1.2335, 1.1528, 1.1326, 1.2370, 1.4094, 1.7227, 2.3324, 
                   2.6522, 1.9578, 1.5574, 1.3788, 1.2663, 1.2796, 1.3530, 1.5527, 1.9153, 2.6259, 
                   3.0767, 2.1153, 1.6979, 1.4534, 1.3614, 1.3698, 1.4634, 1.6627, 2.1344, 2.8243, 
                   2.5157, 1.8627, 1.5370, 1.3032, 1.2737, 1.2609, 1.3902, 1.5500, 1.9401, 2.5519, 
                   2.3612, 1.7706, 1.4230, 1.3012, 1.1682, 1.1824, 1.2795, 1.4345, 1.7778, 2.4526, 
                   2.2220, 1.6208, 1.3678, 1.1633, 1.1050, 1.0920, 1.1835, 1.3657, 1.6761, 2.2110, 
                   2.1787, 1.5880, 1.3433, 1.1575, 1.0608, 1.0444, 1.1441, 1.3302, 1.5946, 2.1538, 
                   2.2342, 1.5785, 1.3206, 1.1507, 1.0035, 0.9834, 1.1583, 1.3190, 1.6138, 2.1685, 
                   2.1377, 1.5844, 1.3355, 1.1404, 1.0310, 1.0126, 1.1347, 1.3142, 1.5970, 2.0716, 
                   2.2415, 1.6905, 1.3513, 1.2280, 1.1016, 1.1051, 1.2122, 1.3956, 1.6976, 2.2992, 
                   2.2791, 1.7213, 1.4163, 1.2354, 1.1589, 1.1275, 1.2401, 1.4264, 1.7019, 2.3613, 
                   2.5930, 1.9438, 1.5315, 1.3850, 1.2723, 1.3074, 1.3514, 1.5646, 1.9209, 2.5600, 
                   2.8860, 2.0808, 1.6889, 1.4842, 1.3681, 1.3299, 1.4979, 1.6128, 2.1060, 2.8027, 
                },
             },
             /* awb para 1 */
             {
                1,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.8357, 1.9238, 1.5660, 1.3677, 1.2257, 1.2770, 1.3825, 1.5352, 1.9452, 2.6412, 
                   2.4240, 1.8107, 1.4121, 1.2655, 1.1810, 1.1706, 1.2591, 1.4327, 1.7774, 2.4290, 
                   2.3238, 1.6559, 1.3668, 1.1863, 1.0918, 1.0883, 1.1911, 1.3700, 1.6682, 2.2358, 
                   2.1708, 1.6241, 1.3055, 1.1510, 1.0496, 1.0374, 1.1462, 1.3261, 1.6112, 2.2185, 
                   2.2176, 1.5837, 1.3120, 1.1408, 1.0059, 0.9808, 1.1460, 1.3013, 1.6188, 2.0981, 
                   2.1407, 1.6106, 1.3234, 1.1358, 1.0382, 1.0160, 1.1381, 1.3306, 1.5938, 2.1551, 
                   2.3556, 1.6393, 1.3796, 1.1837, 1.0893, 1.0953, 1.1743, 1.3695, 1.6805, 2.2373, 
                   2.4142, 1.7602, 1.4333, 1.2800, 1.1602, 1.1693, 1.2567, 1.4409, 1.7491, 2.3578, 
                   2.6148, 1.9224, 1.5354, 1.3280, 1.2474, 1.2629, 1.3175, 1.5569, 1.8848, 2.5928, 
                   3.1956, 2.0745, 1.7179, 1.4953, 1.3486, 1.3949, 1.4673, 1.6861, 2.1108, 2.9036, 
                   2.7395, 1.9467, 1.5536, 1.3662, 1.2718, 1.2800, 1.3482, 1.5664, 1.9010, 2.6498, 
                   2.4521, 1.7817, 1.4357, 1.2704, 1.1651, 1.1757, 1.2566, 1.4427, 1.7746, 2.3949, 
                   2.2487, 1.6753, 1.3488, 1.1962, 1.1037, 1.0917, 1.1777, 1.3679, 1.6483, 2.2585, 
                   2.2556, 1.5928, 1.3309, 1.1502, 1.0410, 1.0311, 1.1440, 1.3226, 1.6103, 2.1478, 
                   2.1159, 1.5912, 1.2887, 1.1349, 0.9982, 0.9873, 1.1227, 1.3018, 1.5938, 2.1121, 
                   2.2485, 1.5868, 1.3370, 1.1467, 1.0295, 1.0148, 1.1403, 1.3173, 1.6068, 2.1360, 
                   2.2217, 1.6486, 1.3480, 1.1787, 1.0744, 1.0786, 1.1725, 1.3585, 1.6572, 2.2197, 
                   2.3751, 1.7394, 1.4094, 1.2456, 1.1560, 1.1543, 1.2402, 1.4350, 1.7378, 2.3651, 
                   2.6852, 1.9118, 1.5553, 1.3614, 1.2450, 1.2485, 1.3548, 1.5099, 1.9181, 2.5371, 
                   2.8978, 2.1214, 1.6556, 1.4714, 1.3755, 1.4024, 1.4763, 1.7197, 2.1305, 2.9328, 
                   2.4618, 1.9340, 1.4833, 1.3688, 1.2580, 1.2426, 1.3938, 1.5456, 1.9393, 2.6422, 
                   2.3351, 1.7267, 1.4175, 1.2607, 1.1607, 1.1812, 1.2324, 1.4248, 1.7319, 2.3201, 
                   2.1668, 1.6408, 1.3567, 1.1876, 1.1005, 1.0767, 1.1953, 1.3488, 1.6621, 2.2146, 
                   2.0910, 1.5569, 1.2991, 1.1371, 1.0363, 1.0417, 1.1532, 1.2888, 1.6214, 2.0634, 
                   2.1534, 1.5485, 1.3063, 1.1381, 0.9915, 0.9716, 1.1321, 1.3071, 1.5770, 2.1144, 
                   2.0954, 1.5876, 1.2976, 1.1437, 1.0149, 1.0182, 1.1430, 1.3247, 1.5911, 2.1611, 
                   2.1597, 1.6424, 1.3353, 1.1877, 1.0817, 1.0597, 1.1775, 1.3391, 1.6620, 2.1868, 
                   2.2608, 1.7256, 1.4071, 1.2399, 1.1681, 1.1489, 1.2387, 1.4708, 1.7153, 2.4189, 
                   2.5149, 1.8864, 1.5172, 1.3556, 1.2261, 1.2387, 1.3176, 1.4811, 1.8782, 2.4860, 
                   2.6695, 2.0872, 1.6780, 1.4247, 1.4273, 1.3642, 1.4830, 1.7779, 2.0959, 2.9535, 
                },
             },
             /* awb para 2 */
             {
                1,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.8569, 1.9578, 1.5664, 1.3440, 1.2622, 1.2641, 1.3683, 1.5664, 1.9344, 2.6618, 
                   2.4262, 1.7611, 1.4088, 1.2608, 1.1664, 1.1696, 1.2585, 1.4370, 1.7667, 2.4413, 
                   2.2784, 1.6742, 1.3570, 1.1845, 1.0960, 1.0912, 1.2026, 1.3672, 1.6941, 2.2713, 
                   2.2078, 1.5835, 1.3126, 1.1368, 1.0376, 1.0266, 1.1495, 1.3288, 1.6193, 2.2152, 
                   2.1856, 1.5854, 1.3193, 1.1434, 1.0098, 1.0047, 1.1630, 1.3337, 1.6208, 2.1847, 
                   2.1405, 1.6103, 1.2996, 1.1341, 1.0332, 1.0198, 1.1487, 1.3553, 1.6143, 2.2007, 
                   2.2693, 1.6585, 1.3750, 1.1954, 1.0956, 1.0995, 1.2072, 1.3934, 1.7085, 2.2670, 
                   2.4668, 1.7559, 1.4292, 1.2618, 1.1672, 1.1755, 1.2604, 1.4589, 1.7620, 2.4415, 
                   2.5353, 1.9381, 1.5404, 1.3539, 1.2729, 1.2690, 1.3743, 1.5752, 1.9537, 2.6335, 
                   3.1800, 2.1628, 1.6958, 1.5093, 1.3692, 1.4254, 1.5029, 1.6905, 2.1959, 2.9452, 
                   2.6306, 1.9228, 1.5366, 1.3725, 1.2536, 1.2643, 1.3739, 1.5332, 1.9598, 2.5728, 
                   2.3979, 1.7704, 1.4249, 1.2594, 1.1633, 1.1762, 1.2586, 1.4573, 1.7559, 2.4947, 
                   2.2725, 1.6493, 1.3569, 1.1886, 1.0912, 1.0900, 1.1993, 1.3770, 1.6854, 2.2897, 
                   2.1741, 1.5889, 1.3110, 1.1414, 1.0463, 1.0281, 1.1511, 1.3377, 1.6099, 2.2230, 
                   2.1240, 1.6019, 1.2935, 1.1414, 0.9987, 1.0015, 1.1529, 1.3327, 1.6230, 2.2095, 
                   2.1966, 1.5660, 1.3204, 1.1298, 1.0268, 1.0058, 1.1425, 1.3274, 1.6103, 2.1711, 
                   2.2568, 1.6666, 1.3599, 1.1898, 1.0984, 1.0961, 1.2136, 1.3860, 1.7070, 2.3051, 
                   2.3973, 1.7306, 1.4253, 1.2567, 1.1498, 1.1576, 1.2424, 1.4416, 1.7641, 2.3758, 
                   2.6448, 1.9164, 1.5601, 1.3597, 1.2719, 1.2773, 1.3846, 1.5597, 1.9520, 2.6741, 
                   2.9588, 2.1569, 1.6833, 1.4944, 1.3855, 1.4083, 1.4794, 1.7343, 2.1859, 2.9825, 
                   2.5245, 1.8773, 1.5046, 1.3537, 1.2503, 1.2734, 1.3562, 1.5281, 1.9560, 2.6062, 
                   2.2138, 1.7585, 1.3869, 1.2484, 1.1687, 1.1732, 1.2571, 1.4546, 1.7452, 2.4383, 
                   2.2098, 1.6030, 1.3525, 1.1906, 1.0899, 1.0906, 1.1978, 1.3600, 1.6859, 2.2184, 
                   2.1124, 1.5787, 1.3092, 1.1387, 1.0465, 1.0337, 1.1650, 1.3084, 1.6195, 2.1778, 
                   2.0612, 1.5727, 1.2876, 1.1390, 0.9946, 0.9834, 1.1547, 1.3276, 1.6108, 2.1228, 
                   2.1269, 1.5739, 1.3024, 1.1426, 1.0127, 1.0071, 1.1669, 1.2918, 1.6366, 2.0992, 
                   2.1921, 1.6423, 1.3477, 1.1864, 1.0870, 1.0970, 1.1814, 1.4041, 1.6728, 2.2986, 
                   2.2568, 1.7353, 1.4017, 1.2425, 1.1638, 1.1487, 1.2616, 1.4159, 1.7656, 2.3730, 
                   2.5219, 1.8633, 1.5429, 1.3503, 1.2526, 1.2759, 1.3522, 1.5578, 1.9159, 2.5647, 
                   2.8257, 2.1054, 1.6380, 1.4525, 1.4000, 1.3802, 1.4688, 1.6960, 2.1169, 3.0140, 
                },
             },
             /* awb para 3 */
             {
                1,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.6890, 1.9700, 1.4989, 1.3330, 1.2888, 1.2644, 1.3973, 1.5202, 1.9734, 2.4607, 
                   2.3615, 1.7584, 1.3933, 1.2593, 1.1353, 1.1500, 1.2257, 1.4353, 1.7291, 2.4274, 
                   2.2494, 1.6756, 1.3336, 1.1891, 1.1052, 1.0960, 1.2051, 1.3761, 1.6838, 2.2653, 
                   2.1733, 1.5653, 1.3016, 1.1249, 1.0321, 1.0323, 1.1243, 1.3325, 1.5963, 2.1669, 
                   2.1315, 1.5966, 1.2739, 1.1379, 0.9839, 0.9856, 1.1484, 1.3173, 1.5992, 2.1463, 
                   2.1734, 1.5897, 1.3160, 1.1345, 1.0212, 1.0303, 1.1362, 1.3452, 1.6119, 2.1818, 
                   2.3059, 1.6167, 1.3567, 1.1772, 1.0767, 1.0797, 1.1813, 1.3679, 1.6619, 2.2550, 
                   2.4065, 1.8067, 1.4055, 1.2645, 1.1761, 1.1570, 1.2706, 1.4574, 1.7731, 2.3977, 
                   2.6335, 1.8625, 1.5225, 1.3343, 1.2274, 1.2565, 1.3340, 1.5250, 1.8900, 2.6532, 
                   2.8692, 2.2424, 1.6403, 1.5023, 1.4168, 1.3811, 1.5184, 1.6980, 2.1850, 2.8477, 
                   2.7177, 1.9667, 1.5200, 1.3647, 1.2756, 1.2611, 1.3783, 1.5893, 1.9223, 2.6746, 
                   2.3132, 1.7384, 1.4056, 1.2288, 1.1528, 1.1593, 1.2348, 1.4242, 1.7522, 2.3570, 
                   2.2852, 1.6566, 1.3446, 1.1953, 1.0895, 1.0896, 1.1956, 1.3748, 1.6698, 2.2983, 
                   2.1159, 1.5788, 1.3043, 1.1110, 1.0447, 1.0217, 1.1306, 1.3186, 1.6099, 2.1623, 
                   2.1418, 1.5604, 1.2845, 1.1318, 0.9668, 0.9865, 1.1290, 1.3102, 1.5952, 2.1499, 
                   2.2153, 1.5865, 1.3233, 1.1349, 1.0280, 1.0200, 1.1322, 1.3360, 1.6119, 2.1981, 
                   2.1829, 1.6184, 1.3346, 1.1638, 1.0655, 1.0653, 1.1802, 1.3438, 1.6628, 2.2616, 
                   2.4270, 1.7492, 1.4323, 1.2622, 1.1689, 1.1692, 1.2616, 1.4537, 1.7760, 2.4296, 
                   2.5893, 1.8920, 1.5275, 1.3466, 1.2409, 1.2401, 1.3403, 1.5231, 1.8978, 2.6180, 
                   2.9860, 2.1548, 1.6898, 1.5156, 1.4050, 1.4007, 1.5333, 1.6824, 2.2452, 2.9503, 
                   2.5273, 1.9077, 1.5310, 1.3349, 1.2853, 1.2606, 1.4105, 1.5526, 1.9569, 2.6322, 
                   2.1928, 1.7010, 1.3843, 1.2284, 1.1434, 1.1592, 1.2281, 1.4088, 1.7145, 2.3823, 
                   2.2084, 1.6322, 1.3524, 1.1807, 1.0976, 1.0905, 1.1953, 1.3721, 1.6680, 2.2573, 
                   2.0369, 1.5586, 1.3020, 1.1227, 1.0280, 1.0303, 1.1392, 1.3046, 1.6059, 2.1510, 
                   2.0788, 1.5517, 1.2715, 1.1269, 0.9788, 0.9818, 1.1314, 1.3135, 1.5751, 2.1179, 
                   2.0563, 1.5774, 1.3051, 1.1469, 1.0173, 1.0229, 1.1441, 1.3386, 1.6016, 2.2085, 
                   2.1775, 1.6089, 1.3338, 1.1526, 1.0738, 1.0664, 1.1716, 1.3212, 1.6634, 2.1360, 
                   2.2648, 1.7545, 1.3913, 1.2744, 1.1600, 1.1562, 1.2634, 1.4498, 1.7716, 2.3744, 
                   2.5282, 1.8458, 1.5151, 1.3209, 1.2463, 1.2422, 1.3329, 1.4920, 1.8676, 2.4958, 
                   2.8307, 2.1451, 1.6749, 1.5135, 1.4101, 1.4101, 1.5137, 1.6768, 2.2173, 2.8455, 
                },
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                0.4,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.8660, 1.9515, 1.5694, 1.3496, 1.2478, 1.2341, 1.3557, 1.4759, 1.9502, 2.5039, 
                   2.5367, 1.8113, 1.4517, 1.2604, 1.1653, 1.1737, 1.2459, 1.4466, 1.7559, 2.4357, 
                   2.3511, 1.6856, 1.3603, 1.1893, 1.0902, 1.0777, 1.1852, 1.3449, 1.6656, 2.2402, 
                   2.2626, 1.6200, 1.3311, 1.1359, 1.0404, 1.0310, 1.1272, 1.3182, 1.6017, 2.1539, 
                   2.2388, 1.6196, 1.3150, 1.1433, 1.0029, 0.9929, 1.1331, 1.3115, 1.5822, 2.1573, 
                   2.2221, 1.5980, 1.3116, 1.1326, 1.0173, 1.0039, 1.1214, 1.3053, 1.5892, 2.1167, 
                   2.3773, 1.7063, 1.3904, 1.2125, 1.0975, 1.1026, 1.1856, 1.3802, 1.6604, 2.2565, 
                   2.4094, 1.7591, 1.4146, 1.2422, 1.1491, 1.1358, 1.2402, 1.4204, 1.7129, 2.3928, 
                   2.8310, 1.9795, 1.5738, 1.3845, 1.2700, 1.2791, 1.3543, 1.5630, 1.9086, 2.6342, 
                   3.0153, 2.1394, 1.6699, 1.4578, 1.3586, 1.3703, 1.4706, 1.6785, 2.0697, 2.9667, 
                   2.7382, 1.9591, 1.5349, 1.3479, 1.2657, 1.2220, 1.3582, 1.5108, 1.9317, 2.5955, 
                   2.4770, 1.7771, 1.4415, 1.2667, 1.1653, 1.1723, 1.2516, 1.4554, 1.7585, 2.4302, 
                   2.3042, 1.6824, 1.3556, 1.1960, 1.0889, 1.0841, 1.1796, 1.3510, 1.6678, 2.2411, 
                   2.1937, 1.6072, 1.3164, 1.1495, 1.0398, 1.0265, 1.1331, 1.3236, 1.6017, 2.1459, 
                   2.2369, 1.5985, 1.3283, 1.1436, 0.9956, 0.9964, 1.1233, 1.3226, 1.5776, 2.1289, 
                   2.1673, 1.5821, 1.3064, 1.1267, 1.0173, 1.0011, 1.1279, 1.3110, 1.5778, 2.1345, 
                   2.2998, 1.6987, 1.3705, 1.2063, 1.0893, 1.1057, 1.1752, 1.3832, 1.6648, 2.2487, 
                   2.4662, 1.7267, 1.4213, 1.2335, 1.1528, 1.1326, 1.2370, 1.4094, 1.7227, 2.3324, 
                   2.6522, 1.9578, 1.5574, 1.3788, 1.2663, 1.2796, 1.3530, 1.5527, 1.9153, 2.6259, 
                   3.0767, 2.1153, 1.6979, 1.4534, 1.3614, 1.3698, 1.4634, 1.6627, 2.1344, 2.8243, 
                   2.5157, 1.8627, 1.5370, 1.3032, 1.2737, 1.2609, 1.3902, 1.5500, 1.9401, 2.5519, 
                   2.3612, 1.7706, 1.4230, 1.3012, 1.1682, 1.1824, 1.2795, 1.4345, 1.7778, 2.4526, 
                   2.2220, 1.6208, 1.3678, 1.1633, 1.1050, 1.0920, 1.1835, 1.3657, 1.6761, 2.2110, 
                   2.1787, 1.5880, 1.3433, 1.1575, 1.0608, 1.0444, 1.1441, 1.3302, 1.5946, 2.1538, 
                   2.2342, 1.5785, 1.3206, 1.1507, 1.0035, 0.9834, 1.1583, 1.3190, 1.6138, 2.1685, 
                   2.1377, 1.5844, 1.3355, 1.1404, 1.0310, 1.0126, 1.1347, 1.3142, 1.5970, 2.0716, 
                   2.2415, 1.6905, 1.3513, 1.2280, 1.1016, 1.1051, 1.2122, 1.3956, 1.6976, 2.2992, 
                   2.2791, 1.7213, 1.4163, 1.2354, 1.1589, 1.1275, 1.2401, 1.4264, 1.7019, 2.3613, 
                   2.5930, 1.9438, 1.5315, 1.3850, 1.2723, 1.3074, 1.3514, 1.5646, 1.9209, 2.5600, 
                   2.8860, 2.0808, 1.6889, 1.4842, 1.3681, 1.3299, 1.4979, 1.6128, 2.1060, 2.8027, 
                },
             },
             /* awb para 1 */
             {
                0.4,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.8357, 1.9238, 1.5660, 1.3677, 1.2257, 1.2770, 1.3825, 1.5352, 1.9452, 2.6412, 
                   2.4240, 1.8107, 1.4121, 1.2655, 1.1810, 1.1706, 1.2591, 1.4327, 1.7774, 2.4290, 
                   2.3238, 1.6559, 1.3668, 1.1863, 1.0918, 1.0883, 1.1911, 1.3700, 1.6682, 2.2358, 
                   2.1708, 1.6241, 1.3055, 1.1510, 1.0496, 1.0374, 1.1462, 1.3261, 1.6112, 2.2185, 
                   2.2176, 1.5837, 1.3120, 1.1408, 1.0059, 0.9808, 1.1460, 1.3013, 1.6188, 2.0981, 
                   2.1407, 1.6106, 1.3234, 1.1358, 1.0382, 1.0160, 1.1381, 1.3306, 1.5938, 2.1551, 
                   2.3556, 1.6393, 1.3796, 1.1837, 1.0893, 1.0953, 1.1743, 1.3695, 1.6805, 2.2373, 
                   2.4142, 1.7602, 1.4333, 1.2800, 1.1602, 1.1693, 1.2567, 1.4409, 1.7491, 2.3578, 
                   2.6148, 1.9224, 1.5354, 1.3280, 1.2474, 1.2629, 1.3175, 1.5569, 1.8848, 2.5928, 
                   3.1956, 2.0745, 1.7179, 1.4953, 1.3486, 1.3949, 1.4673, 1.6861, 2.1108, 2.9036, 
                   2.7395, 1.9467, 1.5536, 1.3662, 1.2718, 1.2800, 1.3482, 1.5664, 1.9010, 2.6498, 
                   2.4521, 1.7817, 1.4357, 1.2704, 1.1651, 1.1757, 1.2566, 1.4427, 1.7746, 2.3949, 
                   2.2487, 1.6753, 1.3488, 1.1962, 1.1037, 1.0917, 1.1777, 1.3679, 1.6483, 2.2585, 
                   2.2556, 1.5928, 1.3309, 1.1502, 1.0410, 1.0311, 1.1440, 1.3226, 1.6103, 2.1478, 
                   2.1159, 1.5912, 1.2887, 1.1349, 0.9982, 0.9873, 1.1227, 1.3018, 1.5938, 2.1121, 
                   2.2485, 1.5868, 1.3370, 1.1467, 1.0295, 1.0148, 1.1403, 1.3173, 1.6068, 2.1360, 
                   2.2217, 1.6486, 1.3480, 1.1787, 1.0744, 1.0786, 1.1725, 1.3585, 1.6572, 2.2197, 
                   2.3751, 1.7394, 1.4094, 1.2456, 1.1560, 1.1543, 1.2402, 1.4350, 1.7378, 2.3651, 
                   2.6852, 1.9118, 1.5553, 1.3614, 1.2450, 1.2485, 1.3548, 1.5099, 1.9181, 2.5371, 
                   2.8978, 2.1214, 1.6556, 1.4714, 1.3755, 1.4024, 1.4763, 1.7197, 2.1305, 2.9328, 
                   2.4618, 1.9340, 1.4833, 1.3688, 1.2580, 1.2426, 1.3938, 1.5456, 1.9393, 2.6422, 
                   2.3351, 1.7267, 1.4175, 1.2607, 1.1607, 1.1812, 1.2324, 1.4248, 1.7319, 2.3201, 
                   2.1668, 1.6408, 1.3567, 1.1876, 1.1005, 1.0767, 1.1953, 1.3488, 1.6621, 2.2146, 
                   2.0910, 1.5569, 1.2991, 1.1371, 1.0363, 1.0417, 1.1532, 1.2888, 1.6214, 2.0634, 
                   2.1534, 1.5485, 1.3063, 1.1381, 0.9915, 0.9716, 1.1321, 1.3071, 1.5770, 2.1144, 
                   2.0954, 1.5876, 1.2976, 1.1437, 1.0149, 1.0182, 1.1430, 1.3247, 1.5911, 2.1611, 
                   2.1597, 1.6424, 1.3353, 1.1877, 1.0817, 1.0597, 1.1775, 1.3391, 1.6620, 2.1868, 
                   2.2608, 1.7256, 1.4071, 1.2399, 1.1681, 1.1489, 1.2387, 1.4708, 1.7153, 2.4189, 
                   2.5149, 1.8864, 1.5172, 1.3556, 1.2261, 1.2387, 1.3176, 1.4811, 1.8782, 2.4860, 
                   2.6695, 2.0872, 1.6780, 1.4247, 1.4273, 1.3642, 1.4830, 1.7779, 2.0959, 2.9535, 
                },
             },
             /* awb para 2 */
             {
                0.4,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.8569, 1.9578, 1.5664, 1.3440, 1.2622, 1.2641, 1.3683, 1.5664, 1.9344, 2.6618, 
                   2.4262, 1.7611, 1.4088, 1.2608, 1.1664, 1.1696, 1.2585, 1.4370, 1.7667, 2.4413, 
                   2.2784, 1.6742, 1.3570, 1.1845, 1.0960, 1.0912, 1.2026, 1.3672, 1.6941, 2.2713, 
                   2.2078, 1.5835, 1.3126, 1.1368, 1.0376, 1.0266, 1.1495, 1.3288, 1.6193, 2.2152, 
                   2.1856, 1.5854, 1.3193, 1.1434, 1.0098, 1.0047, 1.1630, 1.3337, 1.6208, 2.1847, 
                   2.1405, 1.6103, 1.2996, 1.1341, 1.0332, 1.0198, 1.1487, 1.3553, 1.6143, 2.2007, 
                   2.2693, 1.6585, 1.3750, 1.1954, 1.0956, 1.0995, 1.2072, 1.3934, 1.7085, 2.2670, 
                   2.4668, 1.7559, 1.4292, 1.2618, 1.1672, 1.1755, 1.2604, 1.4589, 1.7620, 2.4415, 
                   2.5353, 1.9381, 1.5404, 1.3539, 1.2729, 1.2690, 1.3743, 1.5752, 1.9537, 2.6335, 
                   3.1800, 2.1628, 1.6958, 1.5093, 1.3692, 1.4254, 1.5029, 1.6905, 2.1959, 2.9452, 
                   2.6306, 1.9228, 1.5366, 1.3725, 1.2536, 1.2643, 1.3739, 1.5332, 1.9598, 2.5728, 
                   2.3979, 1.7704, 1.4249, 1.2594, 1.1633, 1.1762, 1.2586, 1.4573, 1.7559, 2.4947, 
                   2.2725, 1.6493, 1.3569, 1.1886, 1.0912, 1.0900, 1.1993, 1.3770, 1.6854, 2.2897, 
                   2.1741, 1.5889, 1.3110, 1.1414, 1.0463, 1.0281, 1.1511, 1.3377, 1.6099, 2.2230, 
                   2.1240, 1.6019, 1.2935, 1.1414, 0.9987, 1.0015, 1.1529, 1.3327, 1.6230, 2.2095, 
                   2.1966, 1.5660, 1.3204, 1.1298, 1.0268, 1.0058, 1.1425, 1.3274, 1.6103, 2.1711, 
                   2.2568, 1.6666, 1.3599, 1.1898, 1.0984, 1.0961, 1.2136, 1.3860, 1.7070, 2.3051, 
                   2.3973, 1.7306, 1.4253, 1.2567, 1.1498, 1.1576, 1.2424, 1.4416, 1.7641, 2.3758, 
                   2.6448, 1.9164, 1.5601, 1.3597, 1.2719, 1.2773, 1.3846, 1.5597, 1.9520, 2.6741, 
                   2.9588, 2.1569, 1.6833, 1.4944, 1.3855, 1.4083, 1.4794, 1.7343, 2.1859, 2.9825, 
                   2.5245, 1.8773, 1.5046, 1.3537, 1.2503, 1.2734, 1.3562, 1.5281, 1.9560, 2.6062, 
                   2.2138, 1.7585, 1.3869, 1.2484, 1.1687, 1.1732, 1.2571, 1.4546, 1.7452, 2.4383, 
                   2.2098, 1.6030, 1.3525, 1.1906, 1.0899, 1.0906, 1.1978, 1.3600, 1.6859, 2.2184, 
                   2.1124, 1.5787, 1.3092, 1.1387, 1.0465, 1.0337, 1.1650, 1.3084, 1.6195, 2.1778, 
                   2.0612, 1.5727, 1.2876, 1.1390, 0.9946, 0.9834, 1.1547, 1.3276, 1.6108, 2.1228, 
                   2.1269, 1.5739, 1.3024, 1.1426, 1.0127, 1.0071, 1.1669, 1.2918, 1.6366, 2.0992, 
                   2.1921, 1.6423, 1.3477, 1.1864, 1.0870, 1.0970, 1.1814, 1.4041, 1.6728, 2.2986, 
                   2.2568, 1.7353, 1.4017, 1.2425, 1.1638, 1.1487, 1.2616, 1.4159, 1.7656, 2.3730, 
                   2.5219, 1.8633, 1.5429, 1.3503, 1.2526, 1.2759, 1.3522, 1.5578, 1.9159, 2.5647, 
                   2.8257, 2.1054, 1.6380, 1.4525, 1.4000, 1.3802, 1.4688, 1.6960, 2.1169, 3.0140, 
                },
             },
             /* awb para 3 */
             {
                0.4,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.6890, 1.9700, 1.4989, 1.3330, 1.2888, 1.2644, 1.3973, 1.5202, 1.9734, 2.4607, 
                   2.3615, 1.7584, 1.3933, 1.2593, 1.1353, 1.1500, 1.2257, 1.4353, 1.7291, 2.4274, 
                   2.2494, 1.6756, 1.3336, 1.1891, 1.1052, 1.0960, 1.2051, 1.3761, 1.6838, 2.2653, 
                   2.1733, 1.5653, 1.3016, 1.1249, 1.0321, 1.0323, 1.1243, 1.3325, 1.5963, 2.1669, 
                   2.1315, 1.5966, 1.2739, 1.1379, 0.9839, 0.9856, 1.1484, 1.3173, 1.5992, 2.1463, 
                   2.1734, 1.5897, 1.3160, 1.1345, 1.0212, 1.0303, 1.1362, 1.3452, 1.6119, 2.1818, 
                   2.3059, 1.6167, 1.3567, 1.1772, 1.0767, 1.0797, 1.1813, 1.3679, 1.6619, 2.2550, 
                   2.4065, 1.8067, 1.4055, 1.2645, 1.1761, 1.1570, 1.2706, 1.4574, 1.7731, 2.3977, 
                   2.6335, 1.8625, 1.5225, 1.3343, 1.2274, 1.2565, 1.3340, 1.5250, 1.8900, 2.6532, 
                   2.8692, 2.2424, 1.6403, 1.5023, 1.4168, 1.3811, 1.5184, 1.6980, 2.1850, 2.8477, 
                   2.7177, 1.9667, 1.5200, 1.3647, 1.2756, 1.2611, 1.3783, 1.5893, 1.9223, 2.6746, 
                   2.3132, 1.7384, 1.4056, 1.2288, 1.1528, 1.1593, 1.2348, 1.4242, 1.7522, 2.3570, 
                   2.2852, 1.6566, 1.3446, 1.1953, 1.0895, 1.0896, 1.1956, 1.3748, 1.6698, 2.2983, 
                   2.1159, 1.5788, 1.3043, 1.1110, 1.0447, 1.0217, 1.1306, 1.3186, 1.6099, 2.1623, 
                   2.1418, 1.5604, 1.2845, 1.1318, 0.9668, 0.9865, 1.1290, 1.3102, 1.5952, 2.1499, 
                   2.2153, 1.5865, 1.3233, 1.1349, 1.0280, 1.0200, 1.1322, 1.3360, 1.6119, 2.1981, 
                   2.1829, 1.6184, 1.3346, 1.1638, 1.0655, 1.0653, 1.1802, 1.3438, 1.6628, 2.2616, 
                   2.4270, 1.7492, 1.4323, 1.2622, 1.1689, 1.1692, 1.2616, 1.4537, 1.7760, 2.4296, 
                   2.5893, 1.8920, 1.5275, 1.3466, 1.2409, 1.2401, 1.3403, 1.5231, 1.8978, 2.6180, 
                   2.9860, 2.1548, 1.6898, 1.5156, 1.4050, 1.4007, 1.5333, 1.6824, 2.2452, 2.9503, 
                   2.5273, 1.9077, 1.5310, 1.3349, 1.2853, 1.2606, 1.4105, 1.5526, 1.9569, 2.6322, 
                   2.1928, 1.7010, 1.3843, 1.2284, 1.1434, 1.1592, 1.2281, 1.4088, 1.7145, 2.3823, 
                   2.2084, 1.6322, 1.3524, 1.1807, 1.0976, 1.0905, 1.1953, 1.3721, 1.6680, 2.2573, 
                   2.0369, 1.5586, 1.3020, 1.1227, 1.0280, 1.0303, 1.1392, 1.3046, 1.6059, 2.1510, 
                   2.0788, 1.5517, 1.2715, 1.1269, 0.9788, 0.9818, 1.1314, 1.3135, 1.5751, 2.1179, 
                   2.0563, 1.5774, 1.3051, 1.1469, 1.0173, 1.0229, 1.1441, 1.3386, 1.6016, 2.2085, 
                   2.1775, 1.6089, 1.3338, 1.1526, 1.0738, 1.0664, 1.1716, 1.3212, 1.6634, 2.1360, 
                   2.2648, 1.7545, 1.3913, 1.2744, 1.1600, 1.1562, 1.2634, 1.4498, 1.7716, 2.3744, 
                   2.5282, 1.8458, 1.5151, 1.3209, 1.2463, 1.2422, 1.3329, 1.4920, 1.8676, 2.4958, 
                   2.8307, 2.1451, 1.6749, 1.5135, 1.4101, 1.4101, 1.5137, 1.6768, 2.2173, 2.8455, 
                },
             },
          },
       },
    },
    /* isp_sub_module_drc_pra isp_sub_module_drc_tuning */
    {
       0,//enable
       1,//interpolation_enable
       6,//ae count
       1,//trigger mode : 0 gain trigger 1 lux trigger
       257,//drc_point_size
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
             200,
          },
          /* aec trigger 2 */
          {
             230,
             290,
          },
          /* aec trigger 3 */
          {
             320,
             370,
          },
          /* aec trigger 4 */
          {
             380,
             420,
          },
          /* aec trigger 5 */
          {
             430,
             500,
          },
       },
       /* isp_sub_module_drc_pra pra[MIDDLE_TRIGGER_COUNT] */
       {
           /* para 0*/
           {
              /* drc_low_LUT_point */
              {
                   1499,1499,1418,1352,1296,1245,1200,1158,1119,1083,1049,1018, 988, 959, 932, 907, 882, 859, 837, 815, 795, 775, 756, 738, 720,
                    703, 687, 671, 656, 641, 627, 613, 599, 586, 574, 562, 550, 538, 527, 516, 506, 495, 486, 476, 466, 457, 448, 439, 431, 423,
                    415, 407, 399, 392, 384, 377, 370, 363, 357, 350, 344, 338, 332, 326, 320, 314, 309, 303, 298, 293, 288, 283, 278, 273, 269,
                    264, 260, 255, 251, 247, 243, 239, 235, 231, 227, 223, 220, 216, 213, 209, 206, 203, 199, 196, 193, 190, 187, 184, 181, 178,
                    176, 173, 170, 168, 165, 162, 160, 158, 155, 153, 150, 148, 146, 144, 142, 140, 137, 135, 133, 131, 129, 128, 126, 124, 122,
                    120, 119, 117, 115, 113, 112, 110, 109, 107, 106, 104, 103, 101, 100,  98,  97,  96,  94,  93,  92,  90,  89,  88,  87,  85,
                     84,  83,  82,  81,  80,  79,  77,  76,  75,  74,  73,  72,  71,  70,  69,  69,  68,  67,  66,  65,  64,  63,  62,  62,  61,
                     60,  59,  58,  58,  57,  56,  55,  55,  54,  53,  53,  52,  51,  50,  50,  49,  49,  48,  47,  47,  46,  46,  45,  44,  44,
                     43,  43,  42,  42,  41,  41,  40,  40,  39,  39,  38,  38,  37,  37,  36,  36,  35,  35,  34,  34,  34,  33,  33,  32,  32,
                     32,  31,  31,  30,  30,  30,  29,  29,  29,  28,  28,  28,  27,  27,  27,  26,  26,  26,  25,  25,  25,  24,  24,  24,  23,
                     23,  23,  23,  22,  22,  22,  22,
              },
              /* drc_high_LUT_point */
              {
                    882, 882, 599, 431, 320, 243, 187, 146, 115,  92,  73,  59,  48,  39,  32,  26,  22,  18,  15,  12,  10,   8,   7,   6,   5,
                      4,   3,   3,   2,   2,   2,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,
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
              0.2,
              /* k2 */
              30,
              /* k3 */
              0.7,
           },
           /* para 1*/
           {
              /* drc_low_LUT_point */
              {
                    811, 811, 803, 795, 787, 780, 772, 765, 757, 750, 743, 735, 728, 721, 714, 707, 700, 693, 687, 680, 673, 667, 660, 654, 648,
                    641, 635, 629, 623, 617, 611, 605, 599, 593, 587, 582, 576, 570, 565, 559, 554, 548, 543, 538, 533, 527, 522, 517, 512, 507,
                    502, 497, 493, 488, 483, 478, 474, 469, 465, 460, 456, 451, 447, 442, 438, 434, 430, 425, 421, 417, 413, 409, 405, 401, 397,
                    393, 390, 386, 382, 378, 375, 371, 367, 364, 360, 357, 353, 350, 346, 343, 340, 336, 333, 330, 327, 324, 320, 317, 314, 311,
                    308, 305, 302, 299, 296, 293, 291, 288, 285, 282, 279, 277, 274, 271, 269, 266, 263, 261, 258, 256, 253, 251, 248, 246, 244,
                    241, 239, 237, 234, 232, 230, 227, 225, 223, 221, 219, 217, 215, 212, 210, 208, 206, 204, 202, 200, 198, 196, 195, 193, 191,
                    189, 187, 185, 183, 182, 180, 178, 176, 175, 173, 171, 170, 168, 166, 165, 163, 162, 160, 158, 157, 155, 154, 152, 151, 149,
                    148, 146, 145, 144, 142, 141, 139, 138, 137, 135, 134, 133, 131, 130, 129, 128, 126, 125, 124, 123, 122, 120, 119, 118, 117,
                    116, 115, 113, 112, 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100,  99,  98,  97,  96,  95,  94,  93,  92,  91,
                     91,  90,  89,  88,  87,  86,  85,  85,  84,  83,  82,  81,  80,  80,  79,  78,  77,  77,  76,  75,  74,  74,  73,  72,  72,
                     71,  70,  69,  69,  68,  67,  67,
              },
              /* drc_high_LUT_point */
              {
                    700, 700, 599, 512, 438, 375, 320, 274, 234, 200, 171, 146, 125, 107,  91,  78,  67,  57,  49,  42,  35,  30,  26,  22,  19,
                     16,  14,  12,  10,   8,   7,   6,   5,   4,   4,   3,   2,   2,   2,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,
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
              0.1,
              /* k2 */
              40,
              /* k3 */
              1,
           },
           /* para 2*/
           {
              /* drc_low_LUT_point */
              {
                   2443,2443,2431,2420,2409,2399,2389,2379,2369,2359,2350,2341,2331,2322,2313,2304,2295,2287,2278,2269,2261,2253,2244,2236,2228,
                   2220,2211,2203,2195,2188,2180,2172,2164,2156,2149,2141,2134,2126,2119,2111,2104,2097,2089,2082,2075,2068,2061,2054,2046,2039,
                   2033,2026,2019,2012,2005,1998,1992,1985,1978,1972,1965,1958,1952,1945,1939,1932,1926,1920,1913,1907,1901,1894,1888,1882,1876,
                   1870,1864,1857,1851,1845,1839,1833,1827,1821,1816,1810,1804,1798,1792,1786,1781,1775,1769,1764,1758,1752,1747,1741,1736,1730,
                   1725,1719,1714,1708,1703,1697,1692,1687,1681,1676,1671,1666,1660,1655,1650,1645,1640,1634,1629,1624,1619,1614,1609,1604,1599,
                   1594,1589,1584,1579,1574,1569,1565,1560,1555,1550,1545,1541,1536,1531,1526,1522,1517,1512,1508,1503,1498,1494,1489,1485,1480,
                   1476,1471,1467,1462,1458,1453,1449,1445,1440,1436,1431,1427,1423,1418,1414,1410,1406,1401,1397,1393,1389,1384,1380,1376,1372,
                   1368,1364,1360,1356,1352,1347,1343,1339,1335,1331,1327,1323,1319,1316,1312,1308,1304,1300,1296,1292,1288,1284,1281,1277,1273,
                   1269,1266,1262,1258,1254,1251,1247,1243,1240,1236,1232,1229,1225,1221,1218,1214,1211,1207,1203,1200,1196,1193,1189,1186,1182,
                   1179,1175,1172,1169,1165,1162,1158,1155,1152,1148,1145,1142,1138,1135,1132,1128,1125,1122,1118,1115,1112,1109,1105,1102,1099,
                   1096,1093,1089,1086,1083,1080,1077,
              },
              /* drc_high_LUT_point */
              {
                   2295,2295,2164,2046,1939,1839,1747,1660,1579,1503,1431,1364,1300,1239,1182,1128,1077,1028, 982, 938, 896, 857, 819, 783, 749,
                    716, 685, 655, 627, 600, 575, 550, 527, 505, 483, 463, 443, 425, 407, 390, 374, 358, 344, 329, 316, 303, 290, 279, 267, 256,
                    246, 236, 226, 217, 208, 200, 192, 184, 177, 170, 163, 156, 150, 144, 138, 133, 128, 123, 118, 113, 109, 104, 100,  96,  93,
                     89,  85,  82,  79,  76,  73,  70,  67,  65,  62,  60,  57,  55,  53,  51,  49,  47,  45,  44,  42,  40,  39,  37,  36,  34,
                     33,  32,  31,  29,  28,  27,  26,  25,  24,  23,  22,  21,  21,  20,  19,  18,  18,  17,  16,  16,  15,  14,  14,  13,  13,
                     12,  12,  11,  11,  11,  10,  10,   9,   9,   9,   8,   8,   8,   7,   7,   7,   6,   6,   6,   6,   6,   5,   5,   5,   5,
                      4,   4,   4,   4,   4,   4,   3,   3,   3,   3,   3,   3,   3,   3,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
                      1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,
              },
              /* saturation */
              170,
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
              0.3,
              /* k2 */
              10,
              /* k3 */
              0.9,
           },
           /* para 3*/
           {
              /* drc_low_LUT_point */
              {
                   1630,1630,1624,1618,1612,1606,1601,1596,1590,1585,1580,1575,1570,1565,1560,1555,1551,1546,1541,1537,1532,1527,1523,1518,1514,
                   1509,1505,1501,1496,1492,1488,1483,1479,1475,1471,1467,1462,1458,1454,1450,1446,1442,1438,1434,1430,1426,1422,1418,1414,1411,
                   1407,1403,1399,1395,1392,1388,1384,1380,1377,1373,1369,1366,1362,1358,1355,1351,1347,1344,1340,1337,1333,1330,1326,1323,1319,
                   1316,1312,1309,1305,1302,1299,1295,1292,1289,1285,1282,1279,1275,1272,1269,1265,1262,1259,1256,1253,1249,1246,1243,1240,1237,
                   1233,1230,1227,1224,1221,1218,1215,1212,1209,1206,1203,1200,1196,1193,1190,1187,1185,1182,1179,1176,1173,1170,1167,1164,1161,
                   1158,1155,1152,1150,1147,1144,1141,1138,1135,1133,1130,1127,1124,1121,1119,1116,1113,1110,1108,1105,1102,1099,1097,1094,1091,
                   1089,1086,1083,1081,1078,1076,1073,1070,1068,1065,1063,1060,1057,1055,1052,1050,1047,1045,1042,1040,1037,1035,1032,1030,1027,
                   1025,1022,1020,1017,1015,1012,1010,1007,1005,1003,1000, 998, 995, 993, 991, 988, 986, 984, 981, 979, 977, 974, 972, 970, 967,
                    965, 963, 960, 958, 956, 954, 951, 949, 947, 945, 942, 940, 938, 936, 934, 931, 929, 927, 925, 923, 920, 918, 916, 914, 912,
                    910, 908, 905, 903, 901, 899, 897, 895, 893, 891, 889, 886, 884, 882, 880, 878, 876, 874, 872, 870, 868, 866, 864, 862, 860,
                    858, 856, 854, 852, 850, 848, 846,
              },
              /* drc_high_LUT_point */
              {
                   1551,1551,1479,1414,1355,1299,1246,1196,1149,1105,1062,1022, 984, 947, 912, 878, 846, 815, 786, 757, 730, 704, 679, 655, 632,
                    610, 589, 569, 549, 530, 512, 494, 477, 461, 445, 430, 416, 402, 388, 375, 363, 351, 339, 328, 317, 306, 296, 287, 277, 268,
                    259, 251, 243, 235, 227, 220, 213, 206, 199, 193, 186, 180, 175, 169, 164, 158, 153, 148, 144, 139, 135, 130, 126, 122, 118,
                    115, 111, 108, 104, 101,  98,  95,  92,  89,  86,  83,  81,  78,  76,  73,  71,  69,  67,  65,  63,  61,  59,  57,  55,  54,
                     52,  50,  49,  47,  46,  44,  43,  42,  40,  39,  38,  37,  36,  34,  33,  32,  31,  30,  29,  29,  28,  27,  26,  25,  24,
                     24,  23,  22,  21,  21,  20,  20,  19,  18,  18,  17,  17,  16,  16,  15,  15,  14,  14,  13,  13,  12,  12,  12,  11,  11,
                     11,  10,  10,  10,   9,   9,   9,   8,   8,   8,   8,   7,   7,   7,   7,   6,   6,   6,   6,   6,   5,   5,   5,   5,   5,
                      5,   4,   4,   4,   4,   4,   4,   4,   3,   3,   3,   3,   3,   3,   3,   3,   2,   2,   2,   2,   2,   2,   2,   2,   2,
                      2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,
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
              0.2,
              /* k2 */
              8,
              /* k3 */
              0.9,
           },
           /* para 4*/
           {
              /* drc_low_LUT_point */
              {
                   1634,1634,1631,1628,1625,1621,1618,1615,1612,1609,1606,1603,1599,1596,1593,1590,1587,1584,1581,1578,1575,1572,1568,1565,1562,
                   1559,1556,1553,1550,1547,1544,1541,1538,1535,1532,1529,1526,1523,1520,1517,1514,1511,1508,1505,1502,1500,1497,1494,1491,1488,
                   1485,1482,1479,1476,1473,1471,1468,1465,1462,1459,1456,1453,1451,1448,1445,1442,1439,1436,1434,1431,1428,1425,1422,1420,1417,
                   1414,1411,1409,1406,1403,1400,1398,1395,1392,1389,1387,1384,1381,1379,1376,1373,1371,1368,1365,1363,1360,1357,1355,1352,1349,
                   1347,1344,1341,1339,1336,1334,1331,1328,1326,1323,1321,1318,1316,1313,1310,1308,1305,1303,1300,1298,1295,1293,1290,1288,1285,
                   1283,1280,1278,1275,1273,1270,1268,1265,1263,1260,1258,1255,1253,1250,1248,1245,1243,1241,1238,1236,1233,1231,1229,1226,1224,
                   1221,1219,1217,1214,1212,1209,1207,1205,1202,1200,1198,1195,1193,1191,1188,1186,1184,1181,1179,1177,1175,1172,1170,1168,1165,
                   1163,1161,1159,1156,1154,1152,1150,1147,1145,1143,1141,1138,1136,1134,1132,1130,1127,1125,1123,1121,1119,1116,1114,1112,1110,
                   1108,1105,1103,1101,1099,1097,1095,1093,1090,1088,1086,1084,1082,1080,1078,1076,1074,1071,1069,1067,1065,1063,1061,1059,1057,
                   1055,1053,1051,1049,1047,1045,1043,1040,1038,1036,1034,1032,1030,1028,1026,1024,1022,1020,1018,1016,1014,1012,1010,1008,1006,
                   1005,1003,1001, 999, 997, 995, 993,
              },
              /* drc_high_LUT_point */
              {
                   1587,1587,1538,1491,1445,1400,1357,1315,1275,1236,1198,1161,1125,1090,1057,1024, 993, 962, 932, 904, 876, 849, 823, 797, 773,
                    749, 726, 704, 682, 661, 641, 621, 602, 583, 565, 548, 531, 514, 499, 483, 468, 454, 440, 426, 413, 400, 388, 376, 365, 353,
                    342, 332, 322, 312, 302, 293, 284, 275, 266, 258, 250, 242, 235, 228, 221, 214, 207, 201, 195, 189, 183, 177, 172, 166, 161,
                    156, 151, 147, 142, 138, 133, 129, 125, 121, 118, 114, 110, 107, 104, 100,  97,  94,  91,  89,  86,  83,  81,  78,  76,  73,
                     71,  69,  67,  64,  62,  61,  59,  57,  55,  53,  52,  50,  48,  47,  45,  44,  43,  41,  40,  39,  37,  36,  35,  34,  33,
                     32,  31,  30,  29,  28,  27,  26,  25,  25,  24,  23,  22,  22,  21,  20,  20,  19,  18,  18,  17,  17,  16,  16,  15,  15,
                     14,  14,  13,  13,  12,  12,  11,  11,  11,  10,  10,  10,   9,   9,   9,   8,   8,   8,   8,   7,   7,   7,   7,   6,   6,
                      6,   6,   5,   5,   5,   5,   5,   5,   4,   4,   4,   4,   4,   4,   3,   3,   3,   3,   3,   3,   3,   3,   2,   2,   2,
                      2,   2,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,
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
              0.2,
              /* k2 */
              8,
              /* k3 */
              1,
           },
           /* para 5*/
           {
              /* drc_low_LUT_point */
              {
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,
              },
              /* drc_high_LUT_point */
              {
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,
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
              0,
              /* k2 */
              8,
              /* k3 */
              1,
           },
       },
       0,//enable_auto_gen
    },
    /* isp_sub_module_gic_tuning_t isp_sub_module_gic_tuning */
    {
       0,//enable
       1,//interpolation_enable
       5,//ae count
       0,//trigger mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             1,
             1.1,
          },
          /* aec trigger 1 */
          {
             4,
             4.1,
          },
          /* aec trigger 2 */
          {
             8,
             8.1,
          },
          /* aec trigger 3 */
          {
             32,
             32.1,
          },
          /* aec trigger 4 */
          {
             64,
             64.1,
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
              1,//enable
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
              1,//enable
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
              1,//enable
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
       5,//count_ae
       0,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             1,
             3,
          },
          /* aec trigger 1 */
          {
             4,
             8.1,
          },
          /* aec trigger 2 */
          {
             13,
             24.1,
          },
          /* aec trigger 3 */
          {
             48,
             48.1,
          },
          /* aec trigger 4 */
          {
             80,
             100.1,
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
              10000,//th1_ny
              2000000,//th2_ny
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
              100,//th1_very_light
              255,//th2_y_max
              1,//rgb_diff_detect_en
              30,//th1_rgb_diff
              50,//th2_rgb_diff
              1,//edge_detect_en
              1000,//th1_edge
              8000,//th2_edge
              10,//dpp_strength
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
              0,//dpp_correct
              0,//luma_detect_en
              0,//th1_very_light
              0,//th2_y_max
              0,//rgb_diff_detect_en
              0,//th1_rgb_diff
              0,//th2_rgb_diff
              0,//edge_detect_en
              0,//th1_edge
              0,//th2_edge
              0,//dpp_strength
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
              0,//dpp_correct
              0,//luma_detect_en
              0,//th1_very_light
              0,//th2_y_max
              0,//rgb_diff_detect_en
              0,//th1_rgb_diff
              0,//th2_rgb_diff
              0,//edge_detect_en
              0,//th1_edge
              0,//th2_edge
              0,//dpp_strength
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
              0,//dpp_correct
              0,//luma_detect_en
              0,//th1_very_light
              0,//th2_y_max
              0,//rgb_diff_detect_en
              0,//th1_rgb_diff
              0,//th2_rgb_diff
              0,//edge_detect_en
              0,//th1_edge
              0,//th2_edge
              0,//dpp_strength
          },
          /* para 4 */
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
              0,//dpp_correct
              0,//luma_detect_en
              0,//th1_very_light
              0,//th2_y_max
              0,//rgb_diff_detect_en
              0,//th1_rgb_diff
              0,//th2_rgb_diff
              0,//edge_detect_en
              0,//th1_edge
              0,//th2_edge
              0,//dpp_strength
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
       1,//count_ae
       4,//count_awb
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             511,
          },
       },
       /* awb trigger */
       {
          /* awb trigger 0 */
          {
             2500,
             3400,
          },
          /* awb trigger 1 */
          {
             4000,
             4500,
          },
          /* awb trigger 2 */
          {
             4800,
             6100,
          },
          /* awb trigger 3 */
          {
             6200,
             6700,
          },
       },
       /* isp_sub_module_ccm1_pra pra[5][7] */
       {
          /* aec trigger 0 */
          {
             /* awb para 0 */
             {
                2,-0.5,-0.5,
                -0.7349,2.1071,-0.3722,
                -0.1206,-0.7594,1.8799,
             },
             /* awb para 1 */
             {
                1.9984,-1.1156,0.1172,
                -0.3624,1.5619,-0.1994,
                0.2286,-1.3585,2.13,
             },
             /* awb para 2 */
             {
                2.1,-1.2,0.1,
                -0.6,2,-0.4,
                -0.05,-1.1,2.15,
             },
             /* awb para 3 */
             {
                2,-1,0,
                -0.5,2.05,-0.55,
                0,-1.1,2.1,
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
             160,
          },
          /* aec trigger 1 */
          {
             180,
             300,
          },
          /* aec trigger 2 */
          {
             310,
             380,
          },
          /* aec trigger 3 */
          {
             400,
             450,
          },
          /* aec trigger 4 */
          {
             470,
             550,
          },
       },
       4096,//gamma_lut_size
       /* isp_sub_module_gamma_lut_pra pra[MIDDLE_TRIGGER_COUNT] */
       {
          /* aec trigger 0 */
          {
                   0,   1,   3,   5,   7,   9,  12,  14,  16,  18,  21,  23,  25,  28,  30,  33,  36,  38,  41,  44,  46,  49,  52,  55,  58,  61,  64,  67,  70,  74,  77,  80,
                  83,  87,  90,  94,  97, 101, 104, 108, 112, 115, 119, 123, 127, 131, 135, 139, 143, 147, 151, 156, 160, 164, 168, 173, 177, 182, 186, 191, 195, 197, 199, 201,
                 203, 205, 208, 210, 212, 214, 217, 219, 221, 223, 226, 228, 230, 232, 235, 237, 239, 242, 244, 246, 248, 251, 253, 255, 258, 260, 262, 265, 267, 270, 272, 274,
                 277, 279, 281, 284, 286, 289, 291, 294, 296, 298, 301, 303, 306, 308, 311, 313, 316, 318, 320, 323, 325, 328, 330, 333, 336, 339, 342, 345, 349, 352, 355, 359,
                 362, 365, 368, 371, 375, 378, 381, 384, 387, 390, 394, 397, 400, 403, 406, 409, 412, 415, 418, 421, 424, 427, 430, 433, 436, 439, 442, 445, 448, 450, 453, 456,
                 459, 462, 465, 468, 472, 475, 478, 481, 485, 488, 491, 494, 497, 501, 504, 507, 510, 513, 516, 519, 523, 526, 529, 532, 535, 538, 541, 544, 547, 551, 554, 557,
                 560, 563, 566, 569, 572, 575, 578, 581, 584, 587, 590, 593, 596, 599, 602, 605, 607, 610, 613, 616, 619, 622, 625, 628, 631, 633, 636, 639, 641, 644, 647, 650,
                 653, 656, 659, 662, 666, 669, 672, 675, 678, 681, 684, 688, 691, 694, 697, 700, 703, 706, 709, 713, 716, 719, 722, 725, 728, 731, 734, 738, 741, 744, 747, 750,
                 753, 756, 759, 763, 766, 769, 772, 775, 778, 781, 784, 787, 791, 794, 797, 800, 803, 806, 809, 812, 815, 819, 822, 825, 828, 831, 834, 837, 840, 843, 846, 850,
                 853, 856, 859, 862, 865, 868, 871, 874, 877, 880, 883, 887, 890, 893, 896, 899, 902, 905, 908, 911, 914, 917, 920, 924, 927, 930, 933, 936, 939, 942, 945, 948,
                 951, 954, 957, 960, 964, 967, 970, 973, 976, 979, 982, 985, 988, 991, 994, 997,1000,1003,1006,1009,1012,1016,1019,1022,1025,1028,1031,1034,1037,1040,1043,1046,
                1049,1052,1055,1058,1061,1064,1067,1070,1073,1076,1080,1083,1086,1089,1092,1095,1098,1101,1104,1107,1110,1113,1116,1119,1122,1125,1128,1131,1134,1137,1140,1143,
                1146,1149,1152,1155,1158,1161,1164,1167,1170,1173,1176,1179,1182,1185,1188,1191,1195,1198,1201,1204,1207,1211,1214,1217,1220,1223,1226,1229,1233,1236,1239,1242,
                1245,1248,1251,1254,1257,1260,1263,1267,1270,1273,1276,1279,1282,1285,1288,1291,1294,1297,1300,1303,1306,1309,1312,1315,1318,1321,1324,1326,1329,1332,1335,1338,
                1341,1344,1347,1350,1353,1356,1358,1361,1364,1367,1370,1373,1376,1378,1381,1384,1387,1390,1393,1395,1398,1401,1404,1406,1409,1412,1415,1418,1420,1423,1426,1429,
                1431,1434,1437,1439,1442,1445,1447,1450,1453,1456,1458,1461,1463,1466,1469,1471,1474,1477,1479,1482,1485,1487,1490,1492,1495,1497,1500,1503,1505,1508,1510,1513,
                1515,1518,1520,1523,1525,1528,1531,1533,1536,1539,1542,1545,1548,1551,1554,1557,1560,1563,1566,1569,1572,1575,1578,1581,1584,1586,1589,1592,1595,1598,1601,1604,
                1607,1610,1613,1616,1619,1622,1624,1627,1630,1633,1636,1639,1642,1645,1648,1650,1653,1656,1659,1662,1665,1667,1670,1673,1676,1679,1681,1684,1687,1690,1693,1695,
                1698,1701,1704,1706,1709,1712,1715,1717,1720,1723,1726,1728,1731,1734,1736,1739,1742,1745,1747,1750,1753,1755,1758,1761,1763,1766,1769,1771,1774,1777,1779,1782,
                1784,1787,1790,1792,1795,1797,1800,1803,1805,1808,1810,1813,1815,1818,1821,1823,1826,1828,1831,1833,1836,1838,1841,1843,1846,1848,1851,1853,1856,1858,1861,1863,
                1866,1868,1871,1873,1876,1878,1880,1883,1885,1888,1890,1893,1895,1897,1900,1902,1905,1907,1909,1912,1914,1917,1919,1921,1924,1926,1928,1931,1933,1935,1938,1940,
                1942,1945,1947,1949,1951,1954,1956,1958,1961,1963,1965,1967,1970,1972,1974,1976,1979,1981,1983,1985,1988,1990,1992,1994,1996,1999,2001,2003,2005,2007,2010,2012,
                2014,2016,2018,2020,2023,2025,2027,2029,2031,2033,2035,2037,2039,2042,2044,2046,2048,2050,2052,2054,2056,2058,2060,2062,2064,2066,2068,2071,2073,2075,2077,2079,
                2081,2083,2085,2087,2089,2091,2093,2095,2097,2099,2101,2102,2104,2106,2108,2110,2112,2114,2116,2118,2120,2122,2124,2126,2128,2130,2132,2134,2136,2138,2139,2141,
                2143,2145,2147,2149,2150,2152,2154,2156,2157,2159,2161,2163,2165,2166,2168,2170,2172,2173,2175,2177,2178,2180,2182,2184,2185,2187,2189,2190,2192,2194,2195,2197,
                2199,2200,2202,2204,2205,2207,2209,2210,2212,2214,2215,2217,2218,2220,2222,2223,2225,2226,2228,2229,2231,2233,2234,2236,2237,2239,2240,2242,2243,2245,2246,2248,
                2249,2251,2252,2254,2255,2257,2258,2260,2261,2263,2264,2266,2267,2269,2270,2271,2273,2274,2276,2277,2279,2280,2281,2283,2284,2286,2287,2288,2290,2291,2292,2294,
                2295,2296,2298,2299,2300,2302,2303,2304,2306,2307,2308,2310,2311,2312,2313,2315,2316,2317,2319,2320,2321,2322,2324,2325,2326,2327,2329,2330,2331,2332,2333,2335,
                2336,2337,2338,2339,2340,2342,2343,2344,2345,2346,2347,2348,2349,2351,2352,2353,2354,2355,2356,2357,2358,2360,2361,2362,2363,2364,2365,2366,2367,2369,2370,2371,
                2372,2373,2374,2375,2376,2377,2379,2380,2381,2382,2383,2384,2385,2386,2387,2389,2390,2391,2392,2393,2394,2395,2396,2397,2398,2400,2401,2402,2403,2404,2405,2406,
                2407,2408,2409,2411,2412,2413,2414,2415,2416,2417,2418,2419,2420,2422,2423,2424,2425,2426,2427,2428,2429,2430,2431,2432,2434,2435,2436,2437,2438,2439,2440,2441,
                2442,2443,2444,2445,2447,2448,2449,2450,2451,2452,2453,2454,2455,2456,2457,2458,2460,2461,2462,2463,2464,2465,2466,2467,2468,2469,2470,2471,2472,2474,2475,2476,
                2477,2478,2479,2480,2481,2482,2483,2484,2485,2486,2487,2489,2490,2491,2492,2493,2494,2495,2496,2497,2498,2499,2500,2501,2502,2503,2505,2506,2507,2508,2509,2510,
                2511,2512,2513,2514,2515,2516,2517,2518,2519,2520,2521,2522,2524,2525,2526,2527,2528,2529,2530,2531,2532,2533,2534,2535,2536,2537,2538,2539,2540,2541,2542,2543,
                2545,2546,2547,2548,2549,2550,2551,2552,2553,2554,2555,2556,2557,2558,2559,2560,2561,2562,2563,2564,2565,2566,2567,2568,2569,2571,2572,2573,2574,2575,2576,2577,
                2578,2579,2580,2581,2582,2583,2584,2585,2586,2587,2588,2589,2590,2591,2592,2593,2594,2595,2596,2597,2598,2599,2600,2601,2602,2603,2604,2605,2606,2607,2608,2609,
                2611,2612,2613,2614,2615,2616,2617,2618,2619,2620,2621,2622,2623,2624,2625,2626,2627,2628,2629,2630,2631,2632,2633,2634,2635,2636,2637,2638,2639,2640,2641,2642,
                2643,2644,2645,2646,2647,2648,2649,2650,2651,2652,2653,2654,2655,2656,2657,2658,2659,2660,2661,2662,2663,2664,2665,2666,2667,2668,2669,2670,2671,2672,2673,2674,
                2675,2676,2677,2678,2679,2680,2681,2682,2683,2684,2685,2686,2687,2688,2689,2690,2691,2692,2692,2693,2694,2695,2696,2697,2698,2699,2700,2701,2702,2703,2704,2705,
                2706,2707,2708,2709,2710,2711,2712,2713,2714,2715,2716,2717,2718,2719,2720,2721,2722,2723,2724,2725,2726,2727,2728,2729,2729,2730,2731,2732,2733,2734,2735,2736,
                2737,2738,2739,2740,2741,2742,2743,2744,2745,2746,2747,2748,2749,2750,2751,2752,2753,2753,2754,2755,2756,2757,2758,2759,2760,2761,2762,2763,2764,2765,2766,2767,
                2768,2769,2770,2771,2772,2773,2773,2774,2775,2776,2777,2778,2779,2780,2781,2782,2783,2784,2785,2786,2787,2788,2789,2789,2790,2791,2792,2793,2794,2795,2796,2797,
                2798,2799,2800,2801,2802,2803,2804,2804,2805,2806,2807,2808,2809,2810,2811,2812,2813,2814,2815,2816,2817,2817,2818,2819,2820,2821,2822,2823,2824,2825,2826,2827,
                2828,2829,2829,2830,2831,2832,2833,2834,2835,2836,2837,2838,2839,2840,2840,2841,2842,2843,2844,2845,2846,2847,2848,2849,2850,2850,2851,2852,2853,2854,2855,2856,
                2857,2858,2859,2860,2860,2861,2862,2863,2864,2865,2866,2867,2868,2869,2870,2870,2871,2872,2873,2874,2875,2876,2877,2878,2879,2879,2880,2881,2882,2883,2884,2885,
                2886,2887,2887,2888,2889,2890,2891,2892,2893,2894,2895,2895,2896,2897,2898,2899,2900,2901,2902,2903,2903,2904,2905,2906,2907,2908,2909,2910,2911,2911,2912,2913,
                2914,2915,2916,2917,2918,2918,2919,2920,2921,2922,2923,2924,2925,2925,2926,2927,2928,2929,2930,2931,2932,2932,2933,2934,2935,2936,2937,2938,2938,2940,2941,2942,
                2943,2944,2945,2946,2947,2948,2949,2950,2951,2952,2953,2954,2956,2957,2958,2959,2960,2961,2962,2963,2964,2965,2966,2967,2968,2969,2970,2971,2972,2973,2974,2975,
                2976,2977,2978,2979,2980,2981,2982,2983,2984,2985,2986,2987,2988,2989,2990,2991,2992,2993,2994,2995,2996,2997,2998,2999,3000,3001,3001,3002,3003,3004,3005,3006,
                3007,3008,3009,3010,3011,3012,3013,3014,3015,3015,3016,3017,3018,3019,3020,3021,3022,3023,3024,3025,3025,3026,3027,3028,3029,3030,3031,3032,3033,3033,3034,3035,
                3036,3037,3038,3039,3039,3040,3041,3042,3043,3044,3045,3045,3046,3047,3048,3049,3050,3050,3051,3052,3053,3054,3055,3055,3056,3057,3058,3059,3059,3060,3061,3062,
                3063,3064,3064,3065,3066,3067,3067,3068,3069,3070,3071,3071,3072,3073,3074,3074,3075,3076,3077,3078,3078,3079,3080,3081,3081,3082,3083,3084,3084,3085,3086,3086,
                3087,3088,3089,3089,3090,3091,3092,3092,3093,3094,3094,3095,3096,3097,3097,3098,3099,3099,3100,3101,3101,3102,3103,3103,3104,3105,3106,3106,3107,3108,3108,3109,
                3110,3110,3111,3112,3112,3113,3113,3114,3115,3115,3116,3117,3117,3118,3119,3119,3120,3121,3121,3122,3122,3123,3124,3124,3125,3125,3126,3127,3127,3128,3128,3129,
                3130,3130,3131,3131,3132,3133,3133,3134,3134,3135,3136,3136,3137,3137,3138,3138,3139,3140,3140,3141,3141,3142,3142,3143,3143,3144,3144,3145,3146,3146,3147,3147,
                3148,3148,3149,3149,3150,3150,3151,3151,3152,3152,3153,3153,3154,3154,3155,3155,3156,3156,3157,3157,3158,3158,3159,3159,3160,3160,3161,3161,3162,3162,3163,3163,
                3163,3164,3164,3165,3165,3166,3166,3167,3166,3167,3168,3169,3169,3170,3171,3171,3172,3173,3173,3174,3175,3175,3176,3177,3177,3178,3179,3179,3180,3180,3181,3182,
                3182,3183,3184,3184,3185,3186,3186,3187,3188,3188,3189,3190,3190,3191,3192,3192,3193,3193,3194,3195,3195,3196,3197,3197,3198,3199,3199,3200,3200,3201,3202,3202,
                3203,3204,3204,3205,3205,3206,3207,3207,3208,3209,3209,3210,3210,3211,3212,3212,3213,3213,3214,3215,3215,3216,3217,3217,3218,3218,3219,3220,3220,3221,3221,3222,
                3223,3223,3224,3224,3225,3226,3226,3227,3227,3228,3228,3229,3230,3230,3231,3231,3232,3233,3233,3234,3234,3235,3236,3236,3237,3237,3238,3238,3239,3240,3240,3241,
                3241,3242,3242,3243,3244,3244,3245,3245,3246,3246,3247,3248,3248,3249,3249,3250,3250,3251,3251,3252,3253,3253,3254,3254,3255,3255,3256,3256,3257,3257,3258,3259,
                3259,3260,3260,3261,3261,3262,3262,3263,3263,3264,3265,3265,3266,3266,3267,3267,3268,3268,3269,3269,3270,3270,3271,3271,3272,3272,3273,3273,3274,3275,3275,3276,
                3276,3277,3277,3278,3278,3279,3279,3280,3280,3281,3281,3282,3282,3283,3283,3284,3284,3285,3285,3286,3286,3287,3287,3288,3288,3289,3289,3290,3290,3291,3291,3292,
                3292,3293,3293,3294,3294,3295,3295,3296,3296,3297,3297,3298,3298,3298,3299,3299,3300,3300,3301,3301,3302,3302,3303,3303,3304,3304,3305,3305,3306,3306,3306,3307,
                3307,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,3312,3313,3313,3314,3314,3315,3315,3316,3316,3316,3317,3317,3318,3318,3319,3319,3320,3320,3320,3321,3321,
                3322,3322,3323,3323,3323,3324,3324,3325,3325,3326,3326,3326,3327,3327,3328,3328,3329,3329,3329,3330,3329,3330,3331,3331,3332,3333,3333,3334,3334,3335,3336,3336,
                3337,3337,3338,3339,3339,3340,3341,3341,3342,3342,3343,3344,3344,3345,3345,3346,3346,3347,3348,3348,3349,3349,3350,3351,3351,3352,3352,3353,3354,3354,3355,3355,
                3356,3356,3357,3358,3358,3359,3359,3360,3360,3361,3362,3362,3363,3363,3364,3364,3365,3366,3366,3367,3367,3368,3368,3369,3370,3370,3371,3371,3372,3372,3373,3373,
                3374,3375,3375,3376,3376,3377,3377,3378,3378,3379,3379,3380,3381,3381,3382,3382,3383,3383,3384,3384,3385,3385,3386,3386,3387,3387,3388,3389,3389,3390,3390,3391,
                3391,3392,3392,3393,3393,3394,3394,3395,3395,3396,3396,3397,3397,3398,3398,3399,3399,3400,3400,3401,3401,3402,3402,3403,3403,3404,3404,3405,3405,3406,3406,3407,
                3407,3408,3408,3409,3409,3410,3410,3411,3411,3412,3412,3413,3413,3414,3414,3415,3415,3416,3416,3417,3417,3417,3418,3418,3419,3419,3420,3420,3421,3421,3422,3422,
                3423,3423,3424,3424,3424,3425,3425,3426,3426,3427,3427,3428,3428,3429,3429,3429,3430,3430,3431,3431,3432,3432,3433,3433,3433,3434,3434,3435,3435,3436,3436,3437,
                3437,3437,3438,3438,3439,3439,3440,3440,3440,3441,3441,3442,3442,3443,3443,3443,3444,3444,3445,3445,3445,3446,3446,3447,3447,3448,3448,3448,3449,3449,3450,3450,
                3450,3451,3451,3452,3452,3452,3453,3453,3454,3454,3454,3455,3455,3456,3456,3456,3457,3457,3458,3458,3458,3459,3459,3459,3460,3460,3461,3461,3461,3462,3462,3462,
                3463,3463,3464,3464,3464,3465,3465,3465,3466,3466,3467,3467,3467,3468,3468,3468,3469,3469,3469,3470,3470,3471,3471,3471,3472,3472,3472,3473,3473,3473,3474,3474,
                3474,3475,3475,3475,3476,3476,3476,3477,3477,3477,3478,3478,3478,3479,3479,3479,3480,3480,3480,3481,3481,3481,3482,3482,3482,3483,3483,3483,3484,3484,3484,3485,
                3485,3485,3486,3486,3486,3487,3487,3487,3488,3488,3488,3488,3489,3489,3489,3490,3490,3490,3491,3491,3491,3491,3492,3492,3492,3493,3493,3493,3494,3494,3494,3494,
                3495,3495,3495,3496,3496,3496,3496,3497,3497,3497,3498,3498,3498,3498,3499,3499,3499,3499,3500,3500,3500,3501,3501,3501,3501,3502,3502,3502,3502,3503,3503,3503,
                3503,3504,3504,3504,3503,3504,3504,3505,3505,3505,3506,3506,3506,3507,3507,3507,3508,3508,3508,3509,3509,3509,3510,3510,3510,3511,3511,3511,3511,3512,3512,3512,
                3513,3513,3513,3514,3514,3514,3515,3515,3515,3516,3516,3516,3517,3517,3517,3518,3518,3518,3519,3519,3519,3520,3520,3520,3521,3521,3521,3522,3522,3522,3523,3523,
                3523,3524,3524,3524,3525,3525,3525,3526,3526,3526,3527,3527,3527,3528,3528,3528,3529,3529,3529,3529,3530,3530,3530,3531,3531,3531,3532,3532,3532,3533,3533,3533,
                3534,3534,3534,3535,3535,3535,3536,3536,3536,3537,3537,3537,3538,3538,3538,3538,3539,3539,3539,3540,3540,3540,3541,3541,3541,3542,3542,3542,3543,3543,3543,3544,
                3544,3544,3544,3545,3545,3545,3546,3546,3546,3547,3547,3547,3548,3548,3548,3549,3549,3549,3550,3550,3550,3550,3551,3551,3551,3552,3552,3552,3553,3553,3553,3554,
                3554,3554,3555,3555,3555,3555,3556,3556,3556,3557,3557,3557,3558,3558,3558,3559,3559,3559,3559,3560,3560,3560,3561,3561,3561,3562,3562,3562,3563,3563,3563,3563,
                3564,3564,3564,3565,3565,3565,3566,3566,3566,3567,3567,3567,3567,3568,3568,3568,3569,3569,3569,3570,3570,3570,3570,3571,3571,3571,3572,3572,3572,3573,3573,3573,
                3573,3574,3574,3574,3575,3575,3575,3576,3576,3576,3576,3577,3577,3577,3578,3578,3578,3579,3579,3579,3579,3580,3580,3580,3581,3581,3581,3582,3582,3582,3582,3583,
                3583,3583,3584,3584,3584,3585,3585,3585,3585,3586,3586,3586,3587,3587,3587,3587,3588,3588,3588,3589,3589,3589,3590,3590,3590,3590,3591,3591,3591,3592,3592,3592,
                3592,3593,3593,3593,3594,3594,3594,3594,3595,3595,3595,3596,3596,3596,3596,3597,3597,3597,3598,3598,3598,3599,3599,3599,3599,3600,3600,3600,3601,3601,3601,3601,
                3602,3602,3602,3603,3603,3603,3603,3604,3604,3604,3605,3605,3605,3605,3606,3606,3606,3607,3607,3607,3607,3608,3608,3608,3609,3609,3609,3609,3610,3610,3610,3610,
                3611,3611,3611,3612,3612,3612,3612,3613,3613,3613,3614,3614,3614,3614,3615,3615,3615,3616,3616,3616,3616,3617,3617,3617,3617,3618,3618,3618,3619,3619,3619,3619,
                3620,3620,3620,3621,3621,3621,3621,3622,3622,3622,3622,3623,3623,3623,3624,3624,3624,3624,3625,3625,3623,3623,3623,3623,3623,3624,3624,3624,3624,3625,3625,3625,
                3625,3626,3626,3626,3626,3627,3627,3627,3627,3628,3628,3628,3628,3629,3629,3629,3629,3630,3630,3630,3630,3631,3631,3631,3631,3632,3632,3632,3632,3633,3633,3633,
                3633,3634,3634,3634,3634,3635,3635,3635,3635,3636,3636,3636,3636,3637,3637,3637,3637,3638,3638,3638,3638,3639,3639,3639,3639,3640,3640,3640,3640,3641,3641,3641,
                3642,3642,3642,3642,3643,3643,3643,3643,3644,3644,3644,3644,3645,3645,3645,3645,3646,3646,3646,3646,3647,3647,3647,3647,3648,3648,3648,3648,3649,3649,3649,3650,
                3650,3650,3650,3651,3651,3651,3651,3652,3652,3652,3652,3653,3653,3653,3653,3654,3654,3654,3655,3655,3655,3655,3656,3656,3656,3656,3657,3657,3657,3657,3658,3658,
                3658,3658,3659,3659,3659,3660,3660,3660,3660,3661,3661,3661,3661,3662,3662,3662,3662,3663,3663,3663,3664,3664,3664,3664,3665,3665,3665,3665,3666,3666,3666,3667,
                3667,3667,3667,3668,3668,3668,3668,3669,3669,3669,3670,3670,3670,3670,3671,3671,3671,3671,3672,3672,3672,3673,3673,3673,3673,3674,3674,3674,3674,3675,3675,3675,
                3676,3676,3676,3676,3677,3677,3677,3677,3678,3678,3678,3679,3679,3679,3679,3680,3680,3680,3681,3681,3681,3681,3682,3682,3682,3682,3683,3683,3683,3684,3684,3684,
                3684,3685,3685,3685,3686,3686,3686,3686,3687,3687,3687,3688,3688,3688,3688,3689,3689,3689,3690,3690,3690,3690,3691,3691,3691,3692,3692,3692,3692,3693,3693,3693,
                3694,3694,3694,3694,3695,3695,3695,3696,3696,3696,3696,3697,3697,3697,3698,3698,3698,3698,3699,3699,3699,3700,3700,3700,3700,3701,3701,3701,3702,3702,3702,3702,
                3703,3703,3703,3704,3704,3704,3704,3705,3705,3705,3706,3706,3706,3707,3707,3707,3707,3708,3708,3708,3709,3709,3709,3709,3710,3710,3710,3711,3711,3711,3711,3712,
                3712,3712,3713,3713,3713,3714,3714,3714,3714,3715,3715,3715,3716,3716,3716,3717,3717,3717,3717,3718,3718,3718,3719,3719,3719,3720,3720,3720,3720,3721,3721,3721,
                3722,3722,3722,3723,3723,3723,3723,3724,3724,3724,3725,3725,3725,3726,3726,3726,3726,3727,3727,3727,3728,3728,3728,3729,3729,3729,3729,3730,3730,3730,3731,3731,
                3731,3732,3732,3732,3733,3733,3733,3733,3734,3734,3734,3735,3735,3735,3736,3736,3736,3737,3737,3737,3737,3738,3738,3738,3739,3739,3739,3740,3740,3740,3741,3741,
                3741,3741,3742,3742,3742,3743,3743,3743,3744,3744,3744,3745,3745,3745,3745,3746,3746,3746,3747,3747,3747,3748,3748,3748,3749,3749,3749,3750,3750,3750,3751,3751,
                3751,3751,3752,3752,3752,3753,3753,3753,3754,3754,3754,3755,3755,3755,3756,3756,3756,3757,3757,3757,3757,3758,3758,3758,3759,3759,3759,3760,3760,3760,3761,3761,
                3761,3762,3762,3762,3763,3763,3763,3764,3764,3764,3764,3765,3765,3765,3766,3766,3766,3767,3767,3767,3768,3768,3768,3769,3769,3769,3770,3770,3770,3771,3771,3771,
                3772,3772,3772,3773,3773,3773,3774,3774,3774,3775,3775,3775,3776,3776,3776,3776,3777,3777,3777,3778,3778,3778,3779,3779,3779,3780,3780,3780,3781,3781,3781,3782,
                3782,3782,3783,3783,3783,3784,3784,3784,3785,3785,3785,3786,3786,3786,3787,3787,3787,3788,3788,3788,3789,3789,3789,3790,3790,3790,3791,3791,3791,3792,3792,3792,
                3793,3793,3793,3794,3794,3794,3795,3795,3795,3796,3796,3796,3797,3797,3797,3798,3798,3798,3799,3799,3799,3800,3800,3800,3801,3801,3801,3802,3802,3802,3803,3803,
                3803,3804,3804,3804,3805,3805,3806,3806,3806,3807,3807,3807,3808,3808,3808,3809,3809,3809,3810,3810,3810,3811,3811,3811,3812,3812,3812,3813,3813,3813,3814,3814,
                3814,3815,3815,3815,3816,3816,3816,3817,3817,3818,3818,3818,3819,3819,3819,3820,3820,3820,3821,3821,3821,3822,3822,3822,3823,3823,3823,3824,3824,3824,3825,3825,
                3826,3826,3826,3827,3827,3827,3828,3828,3828,3829,3829,3829,3830,3830,3830,3831,3831,3831,3832,3832,3833,3833,3833,3834,3834,3834,3835,3835,3835,3836,3836,3836,
                3837,3837,3837,3838,3838,3839,3839,3839,3840,3840,3840,3841,3841,3841,3842,3842,3842,3843,3843,3844,3844,3844,3845,3845,3845,3846,3846,3846,3847,3847,3847,3848,
                3848,3849,3849,3849,3850,3850,3850,3851,3851,3851,3852,3852,3852,3853,3853,3854,3854,3854,3855,3855,3855,3856,3856,3856,3857,3857,3858,3858,3858,3859,3859,3859,
                3860,3860,3860,3861,3861,3862,3862,3862,3863,3863,3863,3864,3864,3864,3865,3865,3866,3866,3866,3867,3867,3867,3868,3868,3869,3869,3869,3870,3870,3870,3871,3871,
                3871,3872,3872,3873,3873,3873,3874,3874,3874,3875,3875,3876,3876,3876,3877,3877,3877,3878,3878,3878,3879,3879,3880,3880,3880,3881,3881,3881,3882,3882,3883,3883,
                3883,3884,3884,3884,3885,3885,3886,3886,3886,3887,3887,3887,3888,3888,3889,3889,3889,3890,3890,3890,3891,3891,3892,3892,3892,3893,3893,3893,3894,3894,3895,3895,
                3895,3896,3896,3897,3897,3897,3898,3898,3898,3899,3899,3900,3900,3900,3901,3901,3901,3902,3902,3903,3903,3903,3904,3904,3905,3906,3906,3907,3907,3908,3908,3909,
                3909,3910,3910,3911,3911,3912,3912,3912,3913,3913,3914,3914,3915,3915,3916,3916,3917,3917,3918,3918,3919,3919,3920,3920,3921,3921,3922,3922,3923,3923,3924,3924,
                3925,3926,3926,3927,3927,3928,3928,3929,3929,3930,3930,3931,3931,3932,3932,3933,3933,3934,3934,3935,3936,3936,3937,3937,3938,3938,3939,3939,3940,3940,3941,3941,
                3942,3943,3943,3944,3944,3945,3945,3946,3946,3947,3948,3948,3949,3949,3950,3950,3951,3952,3952,3953,3953,3954,3954,3955,3956,3956,3957,3957,3958,3958,3959,3960,
                3960,3961,3961,3962,3963,3963,3964,3964,3965,3965,3966,3967,3967,3968,3968,3969,3970,3970,3971,3971,3972,3973,3973,3974,3974,3975,3976,3976,3977,3978,3978,3979,
                3979,3980,3981,3981,3982,3983,3983,3984,3984,3985,3986,3986,3987,3988,3988,3989,3989,3990,3991,3991,3992,3993,3993,3994,3995,3995,3996,3997,3997,3998,3998,3999,
                4000,4000,4001,4002,4002,4003,4004,4004,4005,4006,4006,4007,4008,4008,4009,4010,4010,4011,4012,4012,4013,4014,4014,4015,4016,4016,4017,4018,4018,4019,4020,4021,
                4021,4022,4023,4023,4024,4025,4025,4026,4027,4027,4028,4029,4030,4030,4031,4032,4032,4033,4034,4034,4035,4036,4037,4037,4038,4039,4039,4040,4041,4042,4042,4043,
                4044,4045,4045,4046,4047,4047,4048,4049,4050,4050,4051,4052,4053,4053,4054,4055,4056,4056,4057,4058,4058,4059,4060,4061,4061,4062,4063,4064,4064,4065,4066,4067,
                4067,4068,4069,4070,4071,4071,4072,4073,4074,4074,4075,4076,4077,4077,4078,4079,4080,4081,4081,4082,4083,4084,4084,4085,4086,4087,4088,4088,4089,4090,4091,4092,
          },
          /* aec trigger 1 */
          {
                   0,   1,   2,   3,   4,   5,   6,   7,   9,  10,  11,  12,  13,  14,  15,  16,  18,  19,  20,  21,  22,  23,  24,  25,  27,  28,  29,  30,  32,  33,  34,  35,
                  37,  38,  39,  41,  42,  43,  45,  46,  48,  50,  52,  54,  57,  59,  61,  63,  66,  68,  70,  72,  75,  77,  79,  81,  84,  86,  88,  90,  92,  94,  96,  98,
                 101, 103, 106, 108, 111, 114, 116, 119, 122, 125, 128, 131, 134, 137, 140, 143, 147, 150, 154, 157, 161, 164, 168, 171, 175, 178, 182, 186, 190, 194, 198, 202,
                 206, 210, 214, 218, 222, 226, 230, 234, 238, 242, 246, 250, 255, 259, 263, 267, 272, 276, 280, 284, 288, 292, 296, 300, 304, 308, 313, 318, 323, 328, 333, 338,
                 343, 347, 352, 356, 361, 366, 370, 375, 380, 384, 389, 394, 399, 404, 409, 414, 419, 423, 428, 433, 438, 442, 447, 452, 457, 461, 466, 471, 476, 481, 486, 491,
                 496, 501, 506, 511, 517, 522, 527, 532, 538, 543, 548, 553, 558, 563, 568, 573, 578, 582, 587, 591, 596, 600, 605, 609, 614, 619, 624, 629, 635, 640, 645, 650,
                 656, 660, 664, 669, 673, 677, 682, 686, 691, 695, 700, 705, 710, 715, 720, 725, 730, 734, 738, 743, 747, 751, 756, 760, 765, 769, 773, 778, 782, 786, 791, 795,
                 800, 804, 808, 813, 817, 821, 826, 830, 835, 839, 843, 847, 852, 856, 860, 864, 869, 873, 877, 881, 886, 890, 894, 898, 903, 906, 910, 913, 917, 920, 924, 927,
                 931, 935, 939, 943, 947, 951, 955, 959, 964, 967, 971, 974, 978, 981, 985, 988, 992, 995, 998,1002,1005,1008,1012,1015,1019,1023,1027,1031,1035,1039,1043,1047,
                1051,1054,1057,1061,1064,1067,1071,1074,1078,1081,1084,1087,1091,1094,1097,1100,1104,1107,1110,1113,1116,1119,1122,1125,1128,1131,1134,1137,1141,1144,1147,1150,
                1154,1157,1160,1164,1167,1170,1174,1177,1181,1183,1186,1189,1192,1194,1197,1200,1203,1206,1210,1213,1217,1220,1224,1227,1231,1234,1237,1240,1243,1246,1249,1252,
                1256,1258,1261,1263,1266,1269,1271,1274,1277,1279,1282,1284,1287,1290,1292,1295,1298,1300,1303,1305,1308,1310,1313,1315,1318,1321,1324,1327,1330,1333,1336,1339,
                1342,1344,1346,1349,1351,1353,1356,1358,1361,1363,1366,1368,1371,1374,1376,1379,1382,1384,1387,1390,1393,1396,1399,1402,1405,1407,1409,1412,1414,1416,1419,1421,
                1424,1426,1428,1431,1433,1435,1438,1440,1443,1446,1449,1452,1455,1458,1461,1464,1467,1469,1471,1473,1476,1478,1480,1482,1485,1487,1489,1492,1494,1496,1499,1501,
                1504,1506,1508,1510,1512,1514,1516,1518,1521,1523,1526,1529,1532,1534,1537,1540,1543,1545,1547,1549,1551,1553,1555,1557,1560,1562,1564,1566,1568,1570,1572,1574,
                1577,1579,1582,1584,1587,1589,1592,1594,1597,1599,1601,1603,1605,1607,1609,1611,1613,1615,1617,1619,1621,1623,1625,1627,1629,1630,1632,1634,1636,1638,1640,1642,
                1644,1646,1648,1651,1653,1655,1658,1660,1663,1664,1666,1668,1670,1672,1674,1676,1678,1679,1681,1683,1685,1686,1688,1690,1692,1694,1696,1698,1700,1702,1704,1706,
                1708,1709,1711,1713,1715,1717,1719,1721,1723,1724,1726,1728,1730,1731,1733,1735,1737,1739,1741,1743,1745,1747,1749,1751,1754,1755,1757,1759,1761,1762,1764,1766,
                1768,1769,1771,1773,1775,1777,1779,1781,1783,1784,1786,1788,1790,1791,1793,1795,1797,1798,1800,1801,1803,1805,1806,1808,1810,1811,1813,1814,1816,1817,1819,1820,
                1822,1823,1825,1826,1828,1829,1831,1832,1834,1835,1837,1839,1841,1842,1844,1846,1848,1849,1850,1852,1853,1854,1856,1857,1859,1860,1862,1863,1865,1866,1868,1869,
                1871,1873,1875,1878,1880,1883,1885,1887,1890,1892,1895,1897,1899,1902,1904,1907,1909,1911,1914,1916,1918,1921,1923,1925,1928,1930,1932,1935,1937,1939,1942,1944,
                1946,1948,1951,1953,1955,1958,1960,1962,1964,1967,1969,1971,1973,1975,1978,1980,1982,1984,1986,1989,1991,1993,1995,1997,2000,2002,2004,2006,2008,2010,2013,2015,
                2017,2019,2021,2023,2025,2027,2029,2032,2034,2036,2038,2040,2042,2044,2046,2048,2050,2052,2054,2056,2058,2060,2062,2064,2066,2068,2070,2073,2074,2076,2078,2080,
                2082,2084,2086,2088,2090,2092,2094,2096,2098,2100,2102,2104,2106,2108,2110,2111,2113,2115,2117,2119,2121,2123,2125,2127,2128,2130,2132,2134,2136,2138,2139,2141,
                2143,2145,2147,2149,2150,2152,2154,2156,2157,2159,2161,2163,2165,2166,2168,2170,2172,2173,2175,2177,2178,2180,2182,2184,2185,2187,2189,2190,2192,2194,2195,2197,
                2199,2200,2202,2204,2205,2207,2209,2210,2212,2214,2215,2217,2218,2220,2222,2223,2225,2226,2228,2229,2231,2233,2234,2236,2237,2239,2240,2242,2243,2245,2246,2248,
                2249,2251,2252,2254,2255,2257,2258,2260,2261,2263,2264,2266,2267,2269,2270,2271,2273,2274,2276,2277,2279,2280,2281,2283,2284,2286,2287,2288,2290,2291,2292,2294,
                2295,2296,2298,2299,2300,2302,2303,2304,2306,2307,2308,2310,2311,2312,2313,2315,2316,2317,2319,2320,2321,2322,2324,2325,2326,2327,2329,2330,2331,2332,2333,2335,
                2336,2337,2338,2339,2340,2342,2343,2344,2345,2346,2347,2348,2349,2351,2352,2353,2354,2355,2356,2357,2358,2360,2361,2362,2363,2364,2365,2366,2367,2369,2370,2371,
                2372,2373,2374,2375,2376,2377,2379,2380,2381,2382,2383,2384,2385,2386,2387,2389,2390,2391,2392,2393,2394,2395,2396,2397,2398,2400,2401,2402,2403,2404,2405,2406,
                2407,2408,2409,2411,2412,2413,2414,2415,2416,2417,2418,2419,2420,2422,2423,2424,2425,2426,2427,2428,2429,2430,2431,2432,2434,2435,2436,2437,2438,2439,2440,2441,
                2442,2443,2444,2445,2447,2448,2449,2450,2451,2452,2453,2454,2455,2456,2457,2458,2460,2461,2462,2463,2464,2465,2466,2467,2468,2469,2470,2471,2472,2474,2475,2476,
                2477,2478,2479,2480,2481,2482,2483,2484,2485,2486,2487,2489,2490,2491,2492,2493,2494,2495,2496,2497,2498,2499,2500,2501,2502,2503,2505,2506,2507,2508,2509,2510,
                2511,2512,2513,2514,2515,2516,2517,2518,2519,2520,2521,2522,2524,2525,2526,2527,2528,2529,2530,2531,2532,2533,2534,2535,2536,2537,2538,2539,2540,2541,2542,2543,
                2545,2546,2547,2548,2549,2550,2551,2552,2553,2554,2555,2556,2557,2558,2559,2560,2561,2562,2563,2564,2565,2566,2567,2568,2569,2571,2572,2573,2574,2575,2576,2577,
                2578,2579,2580,2581,2582,2583,2584,2585,2586,2587,2588,2589,2590,2591,2592,2593,2594,2595,2596,2597,2598,2599,2600,2601,2602,2603,2604,2605,2606,2607,2608,2609,
                2611,2612,2613,2614,2615,2616,2617,2618,2619,2620,2621,2622,2623,2624,2625,2626,2627,2628,2629,2630,2631,2632,2633,2634,2635,2636,2637,2638,2639,2640,2641,2642,
                2643,2644,2645,2646,2647,2648,2649,2650,2651,2652,2653,2654,2655,2656,2657,2658,2659,2660,2661,2662,2663,2664,2665,2666,2667,2668,2669,2670,2671,2672,2673,2674,
                2675,2676,2677,2678,2679,2680,2681,2682,2683,2684,2685,2686,2687,2688,2689,2690,2691,2692,2692,2693,2694,2695,2696,2697,2698,2699,2700,2701,2702,2703,2704,2705,
                2706,2707,2708,2709,2710,2711,2712,2713,2714,2715,2716,2717,2718,2719,2720,2721,2722,2723,2724,2725,2726,2727,2728,2729,2729,2730,2731,2732,2733,2734,2735,2736,
                2737,2738,2739,2740,2741,2742,2743,2744,2745,2746,2747,2748,2749,2750,2751,2752,2753,2753,2754,2755,2756,2757,2758,2759,2760,2761,2762,2763,2764,2765,2766,2767,
                2768,2769,2770,2771,2772,2773,2773,2774,2775,2776,2777,2778,2779,2780,2781,2782,2783,2784,2785,2786,2787,2788,2789,2789,2790,2791,2792,2793,2794,2795,2796,2797,
                2798,2799,2800,2801,2802,2803,2804,2804,2805,2806,2807,2808,2809,2810,2811,2812,2813,2814,2815,2816,2817,2817,2818,2819,2820,2821,2822,2823,2824,2825,2826,2827,
                2828,2829,2829,2830,2831,2832,2833,2834,2835,2836,2837,2838,2839,2840,2840,2841,2842,2843,2844,2845,2846,2847,2848,2849,2850,2850,2851,2852,2853,2854,2855,2856,
                2857,2858,2859,2860,2860,2861,2862,2863,2864,2865,2866,2867,2868,2869,2870,2870,2871,2872,2873,2874,2875,2876,2877,2878,2879,2879,2880,2881,2882,2883,2884,2885,
                2886,2887,2887,2888,2889,2890,2891,2892,2893,2894,2895,2895,2896,2897,2898,2899,2900,2901,2902,2903,2903,2904,2905,2906,2907,2908,2909,2910,2911,2911,2912,2913,
                2914,2915,2916,2917,2918,2918,2919,2920,2921,2922,2923,2924,2925,2925,2926,2927,2928,2929,2930,2931,2932,2932,2933,2934,2935,2936,2937,2938,2938,2940,2941,2942,
                2943,2944,2945,2946,2947,2948,2949,2950,2951,2952,2953,2954,2956,2957,2958,2959,2960,2961,2962,2963,2964,2965,2966,2967,2968,2969,2970,2971,2972,2973,2974,2975,
                2976,2977,2978,2979,2980,2981,2982,2983,2984,2985,2986,2987,2988,2989,2990,2991,2992,2993,2994,2995,2996,2997,2998,2999,3000,3001,3001,3002,3003,3004,3005,3006,
                3007,3008,3009,3010,3011,3012,3013,3014,3015,3015,3016,3017,3018,3019,3020,3021,3022,3023,3024,3025,3025,3026,3027,3028,3029,3030,3031,3032,3033,3033,3034,3035,
                3036,3037,3038,3039,3039,3040,3041,3042,3043,3044,3045,3045,3046,3047,3048,3049,3050,3050,3051,3052,3053,3054,3055,3055,3056,3057,3058,3059,3059,3060,3061,3062,
                3063,3064,3064,3065,3066,3067,3067,3068,3069,3070,3071,3071,3072,3073,3074,3074,3075,3076,3077,3078,3078,3079,3080,3081,3081,3082,3083,3084,3084,3085,3086,3086,
                3087,3088,3089,3089,3090,3091,3092,3092,3093,3094,3094,3095,3096,3097,3097,3098,3099,3099,3100,3101,3101,3102,3103,3103,3104,3105,3106,3106,3107,3108,3108,3109,
                3110,3110,3111,3112,3112,3113,3113,3114,3115,3115,3116,3117,3117,3118,3119,3119,3120,3121,3121,3122,3122,3123,3124,3124,3125,3125,3126,3127,3127,3128,3128,3129,
                3130,3130,3131,3131,3132,3133,3133,3134,3134,3135,3136,3136,3137,3137,3138,3138,3139,3140,3140,3141,3141,3142,3142,3143,3143,3144,3144,3145,3146,3146,3147,3147,
                3148,3148,3149,3149,3150,3150,3151,3151,3152,3152,3153,3153,3154,3154,3155,3155,3156,3156,3157,3157,3158,3158,3159,3159,3160,3160,3161,3161,3162,3162,3163,3163,
                3163,3164,3164,3165,3165,3166,3166,3167,3166,3167,3168,3169,3169,3170,3171,3171,3172,3173,3173,3174,3175,3175,3176,3177,3177,3178,3179,3179,3180,3180,3181,3182,
                3182,3183,3184,3184,3185,3186,3186,3187,3188,3188,3189,3190,3190,3191,3192,3192,3193,3193,3194,3195,3195,3196,3197,3197,3198,3199,3199,3200,3200,3201,3202,3202,
                3203,3204,3204,3205,3205,3206,3207,3207,3208,3209,3209,3210,3210,3211,3212,3212,3213,3213,3214,3215,3215,3216,3217,3217,3218,3218,3219,3220,3220,3221,3221,3222,
                3223,3223,3224,3224,3225,3226,3226,3227,3227,3228,3228,3229,3230,3230,3231,3231,3232,3233,3233,3234,3234,3235,3236,3236,3237,3237,3238,3238,3239,3240,3240,3241,
                3241,3242,3242,3243,3244,3244,3245,3245,3246,3246,3247,3248,3248,3249,3249,3250,3250,3251,3251,3252,3253,3253,3254,3254,3255,3255,3256,3256,3257,3257,3258,3259,
                3259,3260,3260,3261,3261,3262,3262,3263,3263,3264,3265,3265,3266,3266,3267,3267,3268,3268,3269,3269,3270,3270,3271,3271,3272,3272,3273,3273,3274,3275,3275,3276,
                3276,3277,3277,3278,3278,3279,3279,3280,3280,3281,3281,3282,3282,3283,3283,3284,3284,3285,3285,3286,3286,3287,3287,3288,3288,3289,3289,3290,3290,3291,3291,3292,
                3292,3293,3293,3294,3294,3295,3295,3296,3296,3297,3297,3298,3298,3298,3299,3299,3300,3300,3301,3301,3302,3302,3303,3303,3304,3304,3305,3305,3306,3306,3306,3307,
                3307,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,3312,3313,3313,3314,3314,3315,3315,3316,3316,3316,3317,3317,3318,3318,3319,3319,3320,3320,3320,3321,3321,
                3322,3322,3323,3323,3323,3324,3324,3325,3325,3326,3326,3326,3327,3327,3328,3328,3329,3329,3329,3330,3329,3330,3331,3331,3332,3333,3333,3334,3334,3335,3336,3336,
                3337,3337,3338,3339,3339,3340,3341,3341,3342,3342,3343,3344,3344,3345,3345,3346,3346,3347,3348,3348,3349,3349,3350,3351,3351,3352,3352,3353,3354,3354,3355,3355,
                3356,3356,3357,3358,3358,3359,3359,3360,3360,3361,3362,3362,3363,3363,3364,3364,3365,3366,3366,3367,3367,3368,3368,3369,3370,3370,3371,3371,3372,3372,3373,3373,
                3374,3375,3375,3376,3376,3377,3377,3378,3378,3379,3379,3380,3381,3381,3382,3382,3383,3383,3384,3384,3385,3385,3386,3386,3387,3387,3388,3389,3389,3390,3390,3391,
                3391,3392,3392,3393,3393,3394,3394,3395,3395,3396,3396,3397,3397,3398,3398,3399,3399,3400,3400,3401,3401,3402,3402,3403,3403,3404,3404,3405,3405,3406,3406,3407,
                3407,3408,3408,3409,3409,3410,3410,3411,3411,3412,3412,3413,3413,3414,3414,3415,3415,3416,3416,3417,3417,3417,3418,3418,3419,3419,3420,3420,3421,3421,3422,3422,
                3423,3423,3424,3424,3424,3425,3425,3426,3426,3427,3427,3428,3428,3429,3429,3429,3430,3430,3431,3431,3432,3432,3433,3433,3433,3434,3434,3435,3435,3436,3436,3437,
                3437,3437,3438,3438,3439,3439,3440,3440,3440,3441,3441,3442,3442,3443,3443,3443,3444,3444,3445,3445,3445,3446,3446,3447,3447,3448,3448,3448,3449,3449,3450,3450,
                3450,3451,3451,3452,3452,3452,3453,3453,3454,3454,3454,3455,3455,3456,3456,3456,3457,3457,3458,3458,3458,3459,3459,3459,3460,3460,3461,3461,3461,3462,3462,3462,
                3463,3463,3464,3464,3464,3465,3465,3465,3466,3466,3467,3467,3467,3468,3468,3468,3469,3469,3469,3470,3470,3471,3471,3471,3472,3472,3472,3473,3473,3473,3474,3474,
                3474,3475,3475,3475,3476,3476,3476,3477,3477,3477,3478,3478,3478,3479,3479,3479,3480,3480,3480,3481,3481,3481,3482,3482,3482,3483,3483,3483,3484,3484,3484,3485,
                3485,3485,3486,3486,3486,3487,3487,3487,3488,3488,3488,3488,3489,3489,3489,3490,3490,3490,3491,3491,3491,3491,3492,3492,3492,3493,3493,3493,3494,3494,3494,3494,
                3495,3495,3495,3496,3496,3496,3496,3497,3497,3497,3498,3498,3498,3498,3499,3499,3499,3499,3500,3500,3500,3501,3501,3501,3501,3502,3502,3502,3502,3503,3503,3503,
                3503,3504,3504,3504,3503,3504,3504,3505,3505,3505,3506,3506,3506,3507,3507,3507,3508,3508,3508,3509,3509,3509,3510,3510,3510,3511,3511,3511,3511,3512,3512,3512,
                3513,3513,3513,3514,3514,3514,3515,3515,3515,3516,3516,3516,3517,3517,3517,3518,3518,3518,3519,3519,3519,3520,3520,3520,3521,3521,3521,3522,3522,3522,3523,3523,
                3523,3524,3524,3524,3525,3525,3525,3526,3526,3526,3527,3527,3527,3528,3528,3528,3529,3529,3529,3529,3530,3530,3530,3531,3531,3531,3532,3532,3532,3533,3533,3533,
                3534,3534,3534,3535,3535,3535,3536,3536,3536,3537,3537,3537,3538,3538,3538,3538,3539,3539,3539,3540,3540,3540,3541,3541,3541,3542,3542,3542,3543,3543,3543,3544,
                3544,3544,3544,3545,3545,3545,3546,3546,3546,3547,3547,3547,3548,3548,3548,3549,3549,3549,3550,3550,3550,3550,3551,3551,3551,3552,3552,3552,3553,3553,3553,3554,
                3554,3554,3555,3555,3555,3555,3556,3556,3556,3557,3557,3557,3558,3558,3558,3559,3559,3559,3559,3560,3560,3560,3561,3561,3561,3562,3562,3562,3563,3563,3563,3563,
                3564,3564,3564,3565,3565,3565,3566,3566,3566,3567,3567,3567,3567,3568,3568,3568,3569,3569,3569,3570,3570,3570,3570,3571,3571,3571,3572,3572,3572,3573,3573,3573,
                3573,3574,3574,3574,3575,3575,3575,3576,3576,3576,3576,3577,3577,3577,3578,3578,3578,3579,3579,3579,3579,3580,3580,3580,3581,3581,3581,3582,3582,3582,3582,3583,
                3583,3583,3584,3584,3584,3585,3585,3585,3585,3586,3586,3586,3587,3587,3587,3587,3588,3588,3588,3589,3589,3589,3590,3590,3590,3590,3591,3591,3591,3592,3592,3592,
                3592,3593,3593,3593,3594,3594,3594,3594,3595,3595,3595,3596,3596,3596,3596,3597,3597,3597,3598,3598,3598,3599,3599,3599,3599,3600,3600,3600,3601,3601,3601,3601,
                3602,3602,3602,3603,3603,3603,3603,3604,3604,3604,3605,3605,3605,3605,3606,3606,3606,3607,3607,3607,3607,3608,3608,3608,3609,3609,3609,3609,3610,3610,3610,3610,
                3611,3611,3611,3612,3612,3612,3612,3613,3613,3613,3614,3614,3614,3614,3615,3615,3615,3616,3616,3616,3616,3617,3617,3617,3617,3618,3618,3618,3619,3619,3619,3619,
                3620,3620,3620,3621,3621,3621,3621,3622,3622,3622,3622,3623,3623,3623,3624,3624,3624,3624,3625,3625,3623,3623,3623,3623,3623,3624,3624,3624,3624,3625,3625,3625,
                3625,3626,3626,3626,3626,3627,3627,3627,3627,3628,3628,3628,3628,3629,3629,3629,3629,3630,3630,3630,3630,3631,3631,3631,3631,3632,3632,3632,3632,3633,3633,3633,
                3633,3634,3634,3634,3634,3635,3635,3635,3635,3636,3636,3636,3636,3637,3637,3637,3637,3638,3638,3638,3638,3639,3639,3639,3639,3640,3640,3640,3640,3641,3641,3641,
                3642,3642,3642,3642,3643,3643,3643,3643,3644,3644,3644,3644,3645,3645,3645,3645,3646,3646,3646,3646,3647,3647,3647,3647,3648,3648,3648,3648,3649,3649,3649,3650,
                3650,3650,3650,3651,3651,3651,3651,3652,3652,3652,3652,3653,3653,3653,3653,3654,3654,3654,3655,3655,3655,3655,3656,3656,3656,3656,3657,3657,3657,3657,3658,3658,
                3658,3658,3659,3659,3659,3660,3660,3660,3660,3661,3661,3661,3661,3662,3662,3662,3662,3663,3663,3663,3664,3664,3664,3664,3665,3665,3665,3665,3666,3666,3666,3667,
                3667,3667,3667,3668,3668,3668,3668,3669,3669,3669,3670,3670,3670,3670,3671,3671,3671,3671,3672,3672,3672,3673,3673,3673,3673,3674,3674,3674,3674,3675,3675,3675,
                3676,3676,3676,3676,3677,3677,3677,3677,3678,3678,3678,3679,3679,3679,3679,3680,3680,3680,3681,3681,3681,3681,3682,3682,3682,3682,3683,3683,3683,3684,3684,3684,
                3684,3685,3685,3685,3686,3686,3686,3686,3687,3687,3687,3688,3688,3688,3688,3689,3689,3689,3690,3690,3690,3690,3691,3691,3691,3692,3692,3692,3692,3693,3693,3693,
                3694,3694,3694,3694,3695,3695,3695,3696,3696,3696,3696,3697,3697,3697,3698,3698,3698,3698,3699,3699,3699,3700,3700,3700,3700,3701,3701,3701,3702,3702,3702,3702,
                3703,3703,3703,3704,3704,3704,3704,3705,3705,3705,3706,3706,3706,3707,3707,3707,3707,3708,3708,3708,3709,3709,3709,3709,3710,3710,3710,3711,3711,3711,3711,3712,
                3712,3712,3713,3713,3713,3714,3714,3714,3714,3715,3715,3715,3716,3716,3716,3717,3717,3717,3717,3718,3718,3718,3719,3719,3719,3720,3720,3720,3720,3721,3721,3721,
                3722,3722,3722,3723,3723,3723,3723,3724,3724,3724,3725,3725,3725,3726,3726,3726,3726,3727,3727,3727,3728,3728,3728,3729,3729,3729,3729,3730,3730,3730,3731,3731,
                3731,3732,3732,3732,3733,3733,3733,3733,3734,3734,3734,3735,3735,3735,3736,3736,3736,3737,3737,3737,3737,3738,3738,3738,3739,3739,3739,3740,3740,3740,3741,3741,
                3741,3741,3742,3742,3742,3743,3743,3743,3744,3744,3744,3745,3745,3745,3745,3746,3746,3746,3747,3747,3747,3748,3748,3748,3749,3749,3749,3750,3750,3750,3751,3751,
                3751,3751,3752,3752,3752,3753,3753,3753,3754,3754,3754,3755,3755,3755,3756,3756,3756,3757,3757,3757,3757,3758,3758,3758,3759,3759,3759,3760,3760,3760,3761,3761,
                3761,3762,3762,3762,3763,3763,3763,3764,3764,3764,3764,3765,3765,3765,3766,3766,3766,3767,3767,3767,3768,3768,3768,3769,3769,3769,3770,3770,3770,3771,3771,3771,
                3772,3772,3772,3773,3773,3773,3774,3774,3774,3775,3775,3775,3776,3776,3776,3776,3777,3777,3777,3778,3778,3778,3779,3779,3779,3780,3780,3780,3781,3781,3781,3782,
                3782,3782,3783,3783,3783,3784,3784,3784,3785,3785,3785,3786,3786,3786,3787,3787,3787,3788,3788,3788,3789,3789,3789,3790,3790,3790,3791,3791,3791,3792,3792,3792,
                3793,3793,3793,3794,3794,3794,3795,3795,3795,3796,3796,3796,3797,3797,3797,3798,3798,3798,3799,3799,3799,3800,3800,3800,3801,3801,3801,3802,3802,3802,3803,3803,
                3803,3804,3804,3804,3805,3805,3806,3806,3806,3807,3807,3807,3808,3808,3808,3809,3809,3809,3810,3810,3810,3811,3811,3811,3812,3812,3812,3813,3813,3813,3814,3814,
                3814,3815,3815,3815,3816,3816,3816,3817,3817,3818,3818,3818,3819,3819,3819,3820,3820,3820,3821,3821,3821,3822,3822,3822,3823,3823,3823,3824,3824,3824,3825,3825,
                3826,3826,3826,3827,3827,3827,3828,3828,3828,3829,3829,3829,3830,3830,3830,3831,3831,3831,3832,3832,3833,3833,3833,3834,3834,3834,3835,3835,3835,3836,3836,3836,
                3837,3837,3837,3838,3838,3839,3839,3839,3840,3840,3840,3841,3841,3841,3842,3842,3842,3843,3843,3844,3844,3844,3845,3845,3845,3846,3846,3846,3847,3847,3847,3848,
                3848,3849,3849,3849,3850,3850,3850,3851,3851,3851,3852,3852,3852,3853,3853,3854,3854,3854,3855,3855,3855,3856,3856,3856,3857,3857,3858,3858,3858,3859,3859,3859,
                3860,3860,3860,3861,3861,3862,3862,3862,3863,3863,3863,3864,3864,3864,3865,3865,3866,3866,3866,3867,3867,3867,3868,3868,3869,3869,3869,3870,3870,3870,3871,3871,
                3871,3872,3872,3873,3873,3873,3874,3874,3874,3875,3875,3876,3876,3876,3877,3877,3877,3878,3878,3878,3879,3879,3880,3880,3880,3881,3881,3881,3882,3882,3883,3883,
                3883,3884,3884,3884,3885,3885,3886,3886,3886,3887,3887,3887,3888,3888,3889,3889,3889,3890,3890,3890,3891,3891,3892,3892,3892,3893,3893,3893,3894,3894,3895,3895,
                3895,3896,3896,3897,3897,3897,3898,3898,3898,3899,3899,3900,3900,3900,3901,3901,3901,3902,3902,3903,3903,3903,3904,3904,3905,3906,3906,3907,3907,3908,3908,3909,
                3909,3910,3910,3911,3911,3912,3912,3912,3913,3913,3914,3914,3915,3915,3916,3916,3917,3917,3918,3918,3919,3919,3920,3920,3921,3921,3922,3922,3923,3923,3924,3924,
                3925,3926,3926,3927,3927,3928,3928,3929,3929,3930,3930,3931,3931,3932,3932,3933,3933,3934,3934,3935,3936,3936,3937,3937,3938,3938,3939,3939,3940,3940,3941,3941,
                3942,3943,3943,3944,3944,3945,3945,3946,3946,3947,3948,3948,3949,3949,3950,3950,3951,3952,3952,3953,3953,3954,3954,3955,3956,3956,3957,3957,3958,3958,3959,3960,
                3960,3961,3961,3962,3963,3963,3964,3964,3965,3965,3966,3967,3967,3968,3968,3969,3970,3970,3971,3971,3972,3973,3973,3974,3974,3975,3976,3976,3977,3978,3978,3979,
                3979,3980,3981,3981,3982,3983,3983,3984,3984,3985,3986,3986,3987,3988,3988,3989,3989,3990,3991,3991,3992,3993,3993,3994,3995,3995,3996,3997,3997,3998,3998,3999,
                4000,4000,4001,4002,4002,4003,4004,4004,4005,4006,4006,4007,4008,4008,4009,4010,4010,4011,4012,4012,4013,4014,4014,4015,4016,4016,4017,4018,4018,4019,4020,4021,
                4021,4022,4023,4023,4024,4025,4025,4026,4027,4027,4028,4029,4030,4030,4031,4032,4032,4033,4034,4034,4035,4036,4037,4037,4038,4039,4039,4040,4041,4042,4042,4043,
                4044,4045,4045,4046,4047,4047,4048,4049,4050,4050,4051,4052,4053,4053,4054,4055,4056,4056,4057,4058,4058,4059,4060,4061,4061,4062,4063,4064,4064,4065,4066,4067,
                4067,4068,4069,4070,4071,4071,4072,4073,4074,4074,4075,4076,4077,4077,4078,4079,4080,4081,4081,4082,4083,4084,4084,4085,4086,4087,4088,4088,4089,4090,4091,4092,
          },
          /* aec trigger 2 */
          {
                   0,   1,   2,   3,   4,   5,   6,   7,   9,  10,  11,  12,  13,  14,  15,  16,  18,  19,  20,  21,  22,  23,  24,  25,  27,  28,  29,  30,  32,  33,  33,  33,
                  33,  34,  34,  35,  36,  36,  37,  37,  38,  39,  39,  40,  41,  42,  42,  43,  44,  45,  46,  47,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,
                  59,  60,  61,  63,  64,  65,  66,  67,  69,  70,  71,  72,  73,  75,  76,  77,  78,  79,  81,  82,  83,  84,  86,  87,  88,  90,  91,  92,  93,  95,  96,  97,
                  99, 100, 101, 103, 104, 105, 107, 108, 109, 111, 112, 113, 115, 116, 117, 119, 120, 122, 123, 124, 126, 127, 129, 130, 132, 133, 135, 137, 139, 140, 143, 145,
                 147, 149, 152, 154, 157, 159, 162, 165, 167, 170, 173, 177, 180, 183, 186, 190, 193, 197, 201, 205, 208, 212, 216, 221, 225, 229, 233, 238, 242, 247, 252, 257,
                 260, 266, 271, 277, 282, 287, 293, 298, 304, 309, 314, 320, 325, 331, 336, 342, 347, 353, 358, 364, 369, 375, 380, 386, 391, 397, 403, 408, 414, 419, 425, 430,
                 436, 442, 447, 453, 459, 464, 470, 476, 481, 487, 493, 498, 504, 510, 516, 521, 527, 533, 539, 544, 550, 556, 562, 567, 573, 579, 585, 591, 597, 600, 603, 607,
                 610, 614, 617, 621, 624, 628, 631, 635, 638, 642, 646, 649, 653, 656, 660, 664, 667, 671, 675, 678, 682, 686, 689, 693, 697, 700, 704, 708, 712, 715, 719, 723,
                 727, 731, 734, 738, 742, 746, 750, 754, 758, 761, 765, 769, 773, 777, 781, 785, 789, 793, 797, 801, 805, 809, 813, 817, 821, 825, 829, 833, 837, 841, 846, 850,
                 854, 858, 862, 866, 870, 875, 879, 883, 887, 891, 896, 900, 904, 907, 911, 914, 918, 922, 925, 929, 932, 936, 940, 943, 947, 950, 954, 958, 961, 965, 968, 972,
                 976, 979, 983, 986, 990, 994, 997,1001,1004,1008,1011,1015,1019,1022,1026,1029,1033,1036,1040,1044,1047,1051,1054,1058,1061,1065,1069,1072,1076,1079,1083,1086,
                1090,1093,1097,1100,1104,1107,1111,1115,1118,1122,1125,1129,1132,1136,1139,1143,1146,1150,1153,1157,1160,1164,1167,1171,1174,1178,1181,1185,1188,1192,1195,1199,
                1202,1206,1209,1213,1216,1220,1223,1227,1230,1234,1237,1241,1244,1248,1251,1254,1257,1261,1264,1267,1270,1273,1276,1279,1282,1285,1288,1291,1294,1297,1300,1303,
                1306,1309,1312,1315,1318,1321,1324,1327,1330,1333,1336,1339,1342,1345,1348,1351,1354,1357,1360,1363,1365,1368,1371,1374,1377,1380,1383,1386,1389,1392,1394,1397,
                1400,1403,1406,1409,1412,1414,1417,1420,1423,1426,1429,1431,1434,1437,1440,1443,1446,1448,1451,1454,1457,1459,1462,1465,1468,1471,1473,1476,1479,1481,1484,1487,
                1490,1492,1495,1498,1501,1503,1506,1509,1511,1514,1517,1519,1522,1525,1527,1530,1533,1535,1538,1541,1543,1546,1549,1551,1554,1556,1559,1562,1564,1567,1570,1572,
                1575,1577,1580,1582,1585,1588,1590,1593,1595,1598,1600,1603,1605,1608,1610,1613,1615,1618,1621,1623,1626,1628,1630,1633,1635,1638,1640,1643,1645,1648,1650,1653,
                1655,1658,1660,1662,1665,1667,1670,1672,1675,1677,1679,1682,1684,1687,1689,1691,1694,1696,1698,1701,1703,1706,1708,1710,1713,1715,1717,1720,1722,1724,1726,1729,
                1731,1733,1736,1738,1740,1743,1745,1747,1749,1752,1754,1756,1758,1761,1763,1765,1767,1770,1772,1774,1776,1779,1781,1783,1785,1787,1790,1792,1794,1796,1798,1800,
                1803,1805,1807,1809,1811,1813,1815,1818,1820,1822,1824,1826,1828,1830,1832,1835,1837,1839,1841,1843,1845,1847,1849,1851,1853,1855,1857,1859,1861,1863,1865,1867,
                1870,1872,1874,1876,1878,1880,1882,1884,1886,1888,1890,1892,1894,1896,1898,1900,1902,1904,1906,1908,1910,1911,1913,1915,1917,1919,1921,1923,1925,1927,1929,1931,
                1933,1934,1936,1938,1940,1942,1944,1946,1948,1950,1952,1953,1955,1957,1959,1961,1963,1965,1967,1968,1970,1972,1974,1976,1978,1980,1982,1983,1985,1987,1989,1991,
                1993,1995,1996,1998,2000,2002,2004,2006,2007,2009,2011,2013,2015,2017,2018,2020,2022,2024,2026,2027,2029,2031,2033,2035,2037,2038,2040,2042,2044,2046,2047,2049,
                2051,2053,2054,2056,2058,2060,2062,2063,2065,2067,2069,2071,2072,2074,2076,2078,2079,2081,2083,2085,2086,2088,2090,2092,2093,2095,2097,2099,2100,2102,2104,2106,
                2107,2109,2111,2112,2114,2116,2118,2119,2121,2123,2124,2126,2128,2130,2131,2133,2135,2136,2138,2140,2141,2143,2145,2147,2148,2150,2152,2153,2155,2157,2158,2160,
                2162,2163,2165,2167,2168,2170,2172,2173,2175,2177,2178,2180,2182,2183,2185,2187,2188,2190,2191,2193,2195,2196,2198,2200,2201,2203,2205,2206,2208,2209,2211,2213,
                2214,2216,2217,2219,2221,2222,2224,2225,2227,2229,2230,2232,2233,2235,2237,2238,2240,2241,2243,2244,2246,2248,2249,2251,2252,2254,2255,2257,2259,2260,2262,2263,
                2265,2266,2268,2269,2271,2273,2274,2276,2277,2279,2280,2282,2283,2285,2286,2288,2289,2291,2292,2294,2295,2297,2299,2300,2302,2303,2305,2306,2308,2309,2311,2312,
                2314,2315,2317,2318,2320,2321,2322,2323,2324,2326,2327,2328,2329,2331,2332,2333,2334,2335,2337,2338,2339,2340,2342,2343,2344,2345,2346,2348,2349,2350,2351,2352,
                2354,2355,2356,2357,2358,2360,2361,2362,2363,2364,2366,2367,2368,2369,2370,2371,2373,2374,2375,2376,2377,2378,2380,2381,2382,2383,2384,2386,2387,2388,2389,2390,
                2391,2392,2394,2395,2396,2397,2398,2399,2401,2402,2403,2404,2405,2406,2407,2409,2410,2411,2412,2413,2414,2415,2417,2418,2419,2420,2421,2422,2423,2424,2426,2427,
                2428,2429,2430,2431,2432,2433,2434,2436,2437,2438,2439,2440,2441,2442,2443,2444,2446,2447,2448,2449,2450,2451,2452,2453,2454,2455,2456,2458,2459,2460,2461,2462,
                2463,2464,2465,2466,2467,2468,2469,2470,2472,2473,2474,2475,2476,2477,2478,2479,2480,2481,2482,2483,2484,2485,2486,2487,2488,2489,2491,2492,2493,2494,2495,2496,
                2497,2498,2499,2500,2501,2502,2503,2504,2505,2506,2507,2508,2509,2510,2511,2512,2513,2514,2515,2516,2517,2518,2519,2520,2521,2522,2523,2524,2525,2526,2527,2528,
                2529,2530,2531,2532,2533,2534,2535,2536,2537,2538,2539,2540,2541,2542,2543,2544,2545,2546,2547,2548,2549,2550,2551,2552,2553,2554,2555,2556,2557,2558,2559,2560,
                2561,2562,2563,2564,2564,2565,2566,2567,2568,2569,2570,2571,2572,2573,2574,2575,2576,2577,2578,2579,2580,2580,2581,2582,2583,2584,2585,2586,2587,2588,2589,2590,
                2591,2592,2592,2593,2594,2595,2596,2597,2598,2599,2600,2601,2602,2602,2603,2604,2605,2606,2607,2608,2609,2610,2611,2611,2612,2613,2614,2615,2616,2617,2618,2618,
                2619,2620,2621,2622,2623,2624,2625,2625,2626,2627,2628,2629,2630,2631,2631,2632,2632,2633,2634,2635,2636,2636,2637,2638,2639,2639,2640,2641,2642,2642,2643,2644,
                2645,2645,2646,2647,2648,2649,2649,2650,2651,2652,2652,2653,2654,2655,2655,2656,2657,2658,2658,2659,2660,2661,2661,2662,2663,2664,2664,2665,2666,2667,2667,2668,
                2669,2670,2670,2671,2672,2673,2673,2674,2675,2676,2677,2677,2678,2679,2680,2680,2681,2682,2683,2683,2684,2685,2686,2686,2687,2688,2689,2689,2690,2691,2692,2692,
                2693,2694,2695,2695,2696,2697,2697,2698,2699,2700,2700,2701,2702,2703,2703,2704,2705,2706,2706,2707,2708,2709,2709,2710,2711,2712,2712,2713,2714,2715,2715,2716,
                2717,2718,2718,2719,2720,2721,2721,2722,2723,2724,2724,2725,2726,2726,2727,2728,2729,2729,2730,2731,2732,2732,2733,2734,2735,2735,2736,2737,2738,2738,2739,2740,
                2740,2741,2742,2743,2743,2744,2745,2746,2746,2747,2748,2749,2749,2750,2751,2751,2752,2753,2754,2754,2755,2756,2757,2757,2758,2759,2760,2760,2761,2762,2762,2763,
                2764,2765,2765,2766,2767,2768,2768,2769,2770,2770,2771,2772,2773,2773,2774,2775,2776,2776,2777,2778,2778,2779,2780,2781,2781,2782,2783,2784,2784,2785,2786,2786,
                2787,2788,2789,2789,2790,2791,2791,2792,2793,2794,2794,2795,2796,2796,2797,2798,2799,2799,2800,2801,2802,2802,2803,2804,2804,2805,2806,2807,2807,2808,2809,2809,
                2810,2811,2812,2812,2813,2814,2814,2815,2816,2817,2817,2818,2819,2819,2820,2821,2822,2822,2823,2824,2824,2825,2826,2827,2827,2828,2829,2829,2830,2831,2831,2832,
                2833,2834,2834,2835,2836,2836,2837,2838,2839,2839,2840,2841,2841,2842,2843,2844,2844,2845,2846,2846,2847,2848,2848,2849,2850,2851,2851,2852,2853,2853,2854,2855,
                2856,2856,2857,2858,2858,2859,2860,2860,2861,2862,2863,2863,2864,2865,2865,2866,2867,2867,2868,2869,2870,2870,2871,2872,2872,2873,2874,2874,2875,2876,2876,2877,
                2878,2879,2879,2880,2881,2881,2882,2883,2883,2884,2885,2886,2886,2887,2888,2888,2889,2890,2890,2891,2892,2892,2893,2894,2895,2895,2896,2897,2897,2898,2899,2899,
                2900,2901,2901,2902,2903,2903,2904,2905,2906,2906,2907,2908,2908,2909,2910,2910,2911,2912,2912,2913,2914,2914,2915,2916,2917,2917,2918,2919,2919,2920,2921,2921,
                2922,2923,2923,2924,2925,2925,2926,2927,2927,2928,2929,2930,2930,2931,2932,2932,2933,2934,2934,2935,2936,2936,2937,2938,2938,2939,2940,2940,2941,2942,2942,2943,
                2944,2944,2945,2946,2946,2947,2948,2948,2949,2950,2951,2951,2952,2953,2953,2954,2955,2955,2956,2957,2957,2958,2959,2959,2960,2961,2961,2962,2963,2963,2964,2965,
                2965,2966,2967,2967,2968,2969,2969,2970,2971,2971,2972,2973,2973,2974,2975,2975,2976,2977,2977,2978,2979,2979,2980,2981,2981,2982,2983,2983,2984,2985,2985,2986,
                2987,2987,2988,2989,2989,2990,2991,2991,2992,2993,2993,2994,2995,2995,2996,2997,2997,2998,2999,2999,3000,3001,3001,3002,3002,3003,3004,3004,3005,3006,3006,3007,
                3008,3008,3009,3010,3010,3011,3012,3012,3013,3014,3014,3015,3016,3016,3017,3018,3018,3019,3020,3020,3021,3021,3022,3023,3023,3024,3025,3025,3026,3027,3027,3028,
                3029,3029,3030,3031,3031,3032,3033,3033,3034,3034,3035,3036,3036,3037,3038,3038,3039,3040,3040,3041,3042,3042,3043,3044,3044,3045,3045,3046,3047,3047,3048,3049,
                3049,3050,3051,3051,3052,3053,3053,3054,3054,3055,3056,3057,3057,3058,3059,3059,3060,3061,3061,3062,3063,3064,3064,3065,3066,3066,3067,3068,3068,3069,3070,3070,
                3071,3072,3073,3073,3074,3075,3075,3076,3077,3077,3078,3079,3079,3080,3081,3081,3082,3083,3084,3084,3085,3086,3086,3087,3088,3088,3089,3090,3090,3091,3092,3092,
                3093,3094,3094,3095,3096,3096,3097,3098,3098,3099,3100,3100,3101,3102,3102,3103,3104,3104,3105,3106,3106,3107,3108,3108,3109,3110,3110,3111,3112,3112,3113,3114,
                3114,3115,3116,3116,3117,3118,3118,3119,3120,3120,3121,3122,3122,3123,3124,3124,3125,3126,3126,3127,3128,3128,3129,3130,3130,3131,3132,3132,3133,3133,3134,3135,
                3135,3136,3137,3137,3138,3139,3139,3140,3141,3141,3142,3143,3143,3144,3144,3145,3146,3146,3147,3148,3148,3149,3150,3150,3151,3152,3152,3153,3153,3154,3155,3155,
                3156,3157,3157,3158,3159,3159,3160,3160,3161,3162,3162,3163,3164,3164,3165,3165,3166,3167,3167,3168,3169,3169,3170,3170,3171,3172,3172,3173,3174,3174,3175,3175,
                3176,3177,3177,3178,3179,3179,3180,3180,3181,3182,3182,3183,3184,3184,3185,3185,3186,3187,3187,3188,3188,3189,3190,3190,3191,3191,3192,3193,3193,3194,3195,3195,
                3196,3196,3197,3198,3198,3199,3199,3200,3201,3201,3202,3202,3203,3204,3204,3205,3205,3206,3207,3207,3208,3208,3209,3210,3210,3211,3211,3212,3213,3213,3214,3214,
                3215,3216,3216,3217,3217,3218,3219,3219,3220,3220,3221,3222,3222,3223,3223,3224,3224,3225,3226,3226,3227,3227,3228,3229,3229,3230,3230,3231,3231,3232,3233,3233,
                3234,3234,3235,3236,3236,3237,3237,3238,3238,3239,3240,3240,3241,3241,3242,3242,3243,3244,3244,3245,3245,3245,3246,3246,3247,3247,3248,3248,3249,3249,3250,3250,
                3251,3251,3252,3252,3253,3253,3254,3254,3255,3255,3256,3256,3257,3257,3258,3258,3259,3259,3260,3260,3261,3261,3262,3262,3263,3263,3264,3264,3265,3265,3266,3266,
                3267,3267,3268,3268,3269,3269,3270,3270,3270,3271,3271,3272,3272,3273,3273,3274,3274,3275,3275,3276,3276,3277,3277,3278,3278,3279,3279,3280,3280,3281,3281,3282,
                3282,3283,3283,3284,3284,3285,3285,3286,3286,3287,3287,3287,3288,3288,3289,3289,3290,3290,3291,3291,3292,3292,3293,3293,3294,3294,3295,3295,3296,3296,3297,3297,
                3297,3298,3298,3299,3299,3300,3300,3301,3301,3302,3302,3303,3303,3304,3304,3305,3305,3305,3306,3306,3307,3307,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,
                3312,3313,3313,3314,3314,3315,3315,3316,3316,3317,3317,3318,3318,3318,3319,3319,3320,3320,3321,3321,3322,3322,3323,3323,3323,3324,3324,3325,3325,3326,3326,3327,
                3327,3328,3328,3328,3329,3329,3330,3330,3331,3331,3332,3332,3333,3333,3333,3334,3334,3335,3335,3336,3336,3337,3337,3337,3338,3338,3339,3339,3340,3340,3341,3341,
                3341,3342,3342,3343,3343,3344,3344,3344,3345,3345,3346,3346,3347,3347,3348,3348,3348,3349,3349,3350,3350,3351,3351,3351,3352,3352,3353,3353,3354,3354,3354,3355,
                3355,3356,3356,3357,3357,3358,3358,3358,3359,3359,3360,3360,3361,3361,3361,3362,3362,3363,3363,3363,3364,3364,3365,3365,3366,3366,3366,3367,3367,3368,3368,3369,
                3369,3369,3370,3370,3371,3371,3372,3372,3372,3373,3373,3374,3374,3374,3375,3375,3376,3376,3377,3377,3377,3378,3378,3379,3379,3379,3380,3380,3381,3381,3381,3382,
                3382,3383,3383,3384,3384,3384,3385,3385,3386,3386,3386,3387,3387,3388,3388,3388,3389,3389,3390,3390,3390,3391,3391,3392,3392,3392,3393,3393,3394,3394,3394,3395,
                3395,3396,3396,3396,3397,3397,3398,3398,3398,3399,3399,3400,3400,3400,3401,3401,3402,3402,3402,3403,3403,3404,3404,3404,3405,3405,3406,3406,3406,3407,3407,3407,
                3408,3408,3409,3409,3409,3410,3410,3411,3411,3411,3412,3412,3413,3413,3413,3414,3414,3414,3415,3415,3416,3416,3416,3417,3417,3417,3418,3418,3419,3419,3419,3420,
                3420,3421,3421,3421,3421,3422,3422,3423,3423,3424,3424,3425,3425,3425,3426,3426,3427,3427,3428,3428,3428,3429,3429,3430,3430,3431,3431,3431,3432,3432,3433,3433,
                3434,3434,3434,3435,3435,3436,3436,3437,3437,3438,3438,3438,3439,3439,3440,3440,3441,3441,3441,3442,3442,3443,3443,3444,3444,3444,3445,3445,3446,3446,3447,3447,
                3447,3448,3448,3449,3449,3450,3450,3450,3451,3451,3452,3452,3452,3453,3453,3454,3454,3455,3455,3455,3456,3456,3457,3457,3458,3458,3458,3459,3459,3460,3460,3461,
                3461,3461,3462,3462,3463,3463,3464,3464,3464,3465,3465,3466,3466,3466,3467,3467,3468,3468,3469,3469,3469,3470,3470,3471,3471,3472,3472,3472,3473,3473,3474,3474,
                3474,3475,3475,3476,3476,3477,3477,3477,3478,3478,3479,3479,3479,3480,3480,3481,3481,3482,3482,3482,3483,3483,3484,3484,3484,3485,3485,3486,3486,3487,3487,3487,
                3488,3488,3489,3489,3489,3490,3490,3491,3491,3492,3492,3492,3493,3493,3494,3494,3494,3495,3495,3496,3496,3497,3497,3497,3498,3498,3499,3499,3499,3500,3500,3501,
                3501,3501,3502,3502,3503,3503,3504,3504,3504,3505,3505,3506,3506,3506,3507,3507,3508,3508,3508,3509,3509,3510,3510,3510,3511,3511,3512,3512,3513,3513,3513,3514,
                3514,3515,3515,3515,3516,3516,3517,3517,3517,3518,3518,3519,3519,3519,3520,3520,3521,3521,3521,3522,3522,3523,3523,3523,3524,3524,3525,3525,3525,3526,3526,3527,
                3527,3528,3528,3528,3529,3529,3530,3530,3530,3531,3531,3532,3532,3532,3533,3533,3534,3534,3534,3535,3535,3536,3536,3536,3537,3537,3538,3538,3538,3539,3539,3540,
                3540,3540,3541,3541,3542,3542,3542,3543,3543,3544,3544,3544,3545,3545,3546,3546,3546,3547,3547,3547,3548,3548,3549,3549,3549,3550,3550,3551,3551,3551,3552,3552,
                3553,3553,3553,3554,3554,3555,3555,3555,3556,3556,3557,3557,3557,3558,3558,3559,3559,3559,3560,3560,3561,3561,3561,3562,3562,3562,3563,3563,3564,3564,3564,3565,
                3565,3566,3566,3566,3567,3567,3568,3568,3568,3569,3569,3569,3570,3570,3571,3571,3571,3572,3572,3573,3573,3573,3574,3574,3575,3575,3575,3576,3576,3576,3577,3577,
                3578,3578,3578,3579,3579,3580,3580,3580,3581,3581,3582,3582,3582,3583,3583,3583,3584,3584,3585,3585,3584,3585,3585,3586,3586,3587,3587,3588,3588,3589,3589,3590,
                3590,3591,3591,3592,3592,3593,3593,3594,3594,3595,3595,3596,3596,3597,3597,3598,3598,3599,3599,3600,3600,3601,3601,3602,3602,3603,3603,3604,3604,3605,3605,3606,
                3606,3607,3607,3608,3608,3609,3609,3610,3610,3611,3611,3612,3612,3613,3613,3614,3614,3615,3615,3616,3616,3617,3617,3617,3618,3618,3619,3619,3620,3620,3621,3621,
                3622,3622,3623,3623,3624,3624,3625,3625,3626,3626,3627,3627,3628,3628,3629,3629,3630,3630,3631,3631,3631,3632,3632,3633,3633,3634,3634,3635,3635,3636,3636,3637,
                3637,3638,3638,3639,3639,3640,3640,3641,3641,3642,3642,3642,3643,3643,3644,3644,3645,3645,3646,3646,3647,3647,3648,3648,3649,3649,3650,3650,3650,3651,3651,3652,
                3652,3653,3653,3654,3654,3655,3655,3656,3656,3657,3657,3658,3658,3658,3659,3659,3660,3660,3661,3661,3662,3662,3663,3663,3664,3664,3665,3665,3665,3666,3666,3667,
                3667,3668,3668,3669,3669,3670,3670,3671,3671,3671,3672,3672,3673,3673,3674,3674,3675,3675,3676,3676,3677,3677,3677,3678,3678,3679,3679,3680,3680,3681,3681,3682,
                3682,3682,3683,3683,3684,3684,3685,3685,3686,3686,3687,3687,3687,3688,3688,3689,3689,3690,3690,3691,3691,3692,3692,3692,3693,3693,3694,3694,3695,3695,3696,3696,
                3697,3697,3697,3698,3698,3699,3699,3700,3700,3701,3701,3701,3702,3702,3703,3703,3704,3704,3705,3705,3705,3706,3706,3707,3707,3708,3708,3709,3709,3709,3710,3710,
                3711,3711,3712,3712,3713,3713,3713,3714,3714,3715,3715,3716,3716,3717,3717,3717,3718,3718,3719,3719,3720,3720,3720,3721,3721,3722,3722,3723,3723,3724,3724,3724,
                3725,3725,3726,3726,3727,3727,3727,3728,3728,3729,3729,3730,3730,3731,3731,3731,3732,3732,3733,3733,3734,3734,3734,3735,3735,3736,3736,3737,3737,3737,3738,3738,
                3739,3739,3740,3740,3740,3741,3741,3742,3742,3743,3743,3743,3744,3744,3745,3745,3746,3746,3746,3747,3747,3748,3748,3749,3749,3749,3750,3750,3751,3751,3752,3752,
                3752,3753,3753,3754,3754,3754,3755,3755,3756,3756,3757,3757,3757,3758,3758,3759,3759,3759,3760,3760,3761,3761,3762,3762,3762,3763,3763,3764,3764,3765,3765,3765,
                3766,3766,3767,3767,3767,3768,3768,3769,3769,3769,3770,3770,3771,3771,3772,3772,3772,3773,3773,3774,3774,3774,3775,3775,3776,3776,3777,3777,3777,3778,3778,3779,
                3779,3779,3780,3780,3781,3781,3781,3782,3782,3783,3783,3783,3784,3784,3785,3785,3785,3786,3786,3787,3787,3788,3788,3788,3789,3789,3790,3790,3790,3791,3791,3792,
                3792,3792,3793,3793,3794,3794,3794,3795,3795,3796,3796,3796,3797,3797,3798,3798,3798,3799,3799,3800,3800,3800,3801,3801,3802,3802,3802,3803,3803,3804,3804,3804,
                3805,3805,3806,3806,3806,3807,3807,3807,3807,3808,3808,3809,3809,3809,3810,3810,3810,3811,3811,3811,3812,3812,3812,3813,3813,3813,3814,3814,3814,3815,3815,3815,
                3816,3816,3816,3817,3817,3817,3818,3818,3818,3819,3819,3819,3820,3820,3820,3821,3821,3821,3822,3822,3822,3823,3823,3823,3824,3824,3825,3825,3825,3826,3826,3826,
                3827,3827,3827,3828,3828,3828,3829,3829,3829,3830,3830,3830,3831,3831,3831,3832,3832,3833,3833,3833,3834,3834,3834,3835,3835,3835,3836,3836,3836,3837,3837,3837,
                3838,3838,3839,3839,3839,3840,3840,3840,3841,3841,3841,3842,3842,3842,3843,3843,3843,3844,3844,3845,3845,3845,3846,3846,3846,3847,3847,3847,3848,3848,3848,3849,
                3849,3850,3850,3850,3851,3851,3851,3852,3852,3852,3853,3853,3854,3854,3854,3855,3855,3855,3856,3856,3856,3857,3857,3858,3858,3858,3859,3859,3859,3860,3860,3860,
                3861,3861,3862,3862,3862,3863,3863,3863,3864,3864,3865,3865,3865,3866,3866,3866,3867,3867,3868,3868,3868,3869,3869,3869,3870,3870,3870,3871,3871,3872,3872,3872,
                3873,3873,3873,3874,3874,3875,3875,3875,3876,3876,3876,3877,3877,3878,3878,3878,3879,3879,3879,3880,3880,3881,3881,3881,3882,3882,3883,3883,3883,3884,3884,3884,
                3885,3885,3886,3886,3886,3887,3887,3887,3888,3888,3889,3889,3889,3890,3890,3891,3891,3891,3892,3892,3892,3893,3893,3894,3894,3894,3895,3895,3896,3896,3896,3897,
                3897,3897,3898,3898,3899,3899,3899,3900,3900,3901,3901,3901,3902,3902,3903,3903,3903,3904,3904,3904,3905,3905,3906,3906,3906,3907,3907,3908,3908,3908,3909,3909,
                3910,3910,3910,3911,3911,3912,3912,3912,3913,3913,3914,3914,3914,3915,3915,3916,3916,3916,3917,3917,3918,3918,3918,3919,3919,3920,3920,3920,3921,3921,3922,3922,
                3922,3923,3923,3924,3924,3924,3925,3925,3926,3926,3926,3927,3927,3928,3928,3928,3929,3929,3930,3930,3930,3931,3931,3932,3932,3932,3933,3933,3934,3934,3934,3935,
                3935,3936,3936,3936,3937,3937,3938,3938,3939,3939,3939,3940,3940,3941,3941,3941,3942,3942,3943,3943,3943,3944,3944,3945,3945,3946,3946,3946,3947,3947,3948,3948,
                3948,3949,3949,3950,3950,3951,3951,3951,3952,3952,3953,3953,3953,3954,3954,3955,3955,3956,3956,3956,3957,3957,3958,3958,3958,3959,3959,3960,3960,3961,3961,3961,
                3962,3962,3963,3963,3963,3964,3964,3965,3965,3966,3966,3966,3967,3967,3968,3968,3969,3969,3969,3970,3970,3971,3971,3972,3972,3972,3973,3973,3974,3974,3975,3975,
                3975,3976,3976,3977,3977,3978,3978,3978,3979,3979,3980,3980,3981,3981,3981,3982,3982,3983,3983,3984,3984,3984,3985,3985,3986,3986,3987,3987,3987,3988,3988,3989,
                3989,3990,3990,3991,3991,3991,3992,3992,3993,3993,3994,3994,3994,3995,3995,3996,3996,3997,3997,3998,3998,3998,3999,3999,4000,4000,4001,4001,4001,4002,4002,4003,
                4003,4004,4004,4005,4005,4005,4006,4006,4007,4007,4008,4008,4009,4009,4009,4010,4010,4011,4011,4012,4012,4013,4013,4013,4014,4014,4015,4015,4016,4016,4017,4017,
                4017,4018,4018,4019,4019,4020,4020,4021,4021,4022,4022,4022,4023,4023,4024,4024,4025,4025,4026,4026,4027,4027,4027,4028,4028,4029,4029,4030,4030,4031,4031,4031,
                4032,4032,4033,4033,4034,4034,4035,4035,4036,4036,4037,4037,4037,4038,4038,4039,4039,4040,4040,4041,4041,4042,4042,4042,4043,4043,4044,4044,4045,4045,4046,4046,
                4047,4047,4048,4048,4048,4049,4049,4050,4050,4051,4051,4052,4052,4053,4053,4054,4054,4055,4055,4055,4056,4056,4057,4057,4058,4058,4059,4059,4060,4060,4061,4061,
                4062,4062,4062,4063,4063,4064,4064,4065,4065,4066,4066,4067,4067,4068,4068,4069,4069,4070,4070,4070,4071,4071,4072,4072,4073,4073,4074,4074,4075,4075,4076,4076,
                4077,4077,4078,4078,4079,4079,4080,4080,4080,4081,4081,4082,4082,4083,4083,4084,4084,4085,4085,4086,4086,4087,4087,4088,4088,4089,4089,4090,4090,4091,4091,4092,
          },
          /* aec trigger 3 */
          {
                   0,   0,   1,   1,   2,   2,   3,   4,   4,   5,   5,   6,   6,   7,   7,   8,   9,   9,  10,  10,  11,  11,  12,  12,  13,  13,  14,  14,  15,  15,  15,  16,
                  16,  17,  17,  18,  18,  19,  19,  20,  20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  24,  25,  25,  25,  26,  26,  26,  27,  27,  27,  28,  29,  29,
                  30,  31,  31,  32,  33,  34,  34,  35,  36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  42,  43,  44,  44,  45,  46,  46,  47,  48,  48,  49,  50,  51,  52,
                  53,  53,  54,  55,  56,  57,  57,  58,  59,  60,  61,  61,  62,  63,  64,  64,  65,  66,  66,  67,  68,  68,  69,  70,  70,  71,  72,  73,  73,  74,  75,  75,
                  76,  77,  77,  78,  79,  79,  80,  81,  81,  82,  83,  84,  84,  85,  86,  87,  87,  88,  89,  89,  90,  91,  92,  92,  93,  94,  95,  95,  96,  97,  98,  99,
                  98,  99,  99, 100, 100, 101, 101, 101, 102, 102, 103, 103, 104, 104, 104, 105, 105, 106, 106, 107, 107, 108, 108, 108, 109, 109, 110, 110, 111, 111, 112, 112,
                 113, 113, 114, 114, 115, 115, 116, 116, 117, 117, 118, 119, 119, 120, 120, 121, 121, 122, 122, 123, 124, 124, 125, 125, 126, 126, 127, 128, 128, 129, 129, 130,
                 131, 131, 132, 132, 133, 134, 134, 135, 135, 136, 137, 137, 138, 139, 139, 140, 141, 141, 142, 143, 143, 144, 145, 145, 146, 147, 147, 148, 149, 149, 150, 151,
                 151, 152, 153, 154, 154, 155, 156, 156, 157, 158, 159, 159, 160, 161, 162, 162, 163, 164, 165, 165, 166, 167, 168, 168, 169, 170, 171, 172, 172, 173, 174, 175,
                 176, 176, 177, 178, 179, 180, 180, 181, 182, 183, 184, 185, 185, 186, 187, 188, 189, 190, 191, 192, 193, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203,
                 204, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234,
                 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 264, 265, 266, 267, 268,
                 269, 270, 271, 272, 274, 275, 276, 277, 278, 279, 280, 282, 283, 284, 285, 286, 287, 288, 290, 291, 292, 293, 294, 295, 297, 298, 299, 300, 301, 303, 304, 305,
                 306, 307, 309, 310, 311, 312, 314, 315, 316, 317, 318, 320, 321, 322, 323, 325, 326, 327, 328, 330, 331, 332, 334, 335, 336, 337, 339, 340, 341, 342, 344, 345,
                 346, 348, 349, 350, 352, 353, 354, 356, 357, 358, 360, 361, 362, 364, 365, 366, 368, 369, 370, 372, 373, 374, 376, 377, 378, 380, 381, 383, 384, 385, 387, 388,
                 389, 391, 392, 394, 395, 396, 398, 399, 401, 402, 404, 405, 406, 408, 409, 411, 412, 414, 415, 416, 418, 419, 421, 422, 424, 425, 427, 428, 430, 431, 433, 434,
                 436, 437, 439, 440, 441, 443, 444, 446, 448, 450, 451, 453, 454, 456, 457, 459, 460, 461, 463, 464, 466, 467, 469, 470, 472, 473, 475, 476, 478, 479, 481, 482,
                 484, 485, 487, 488, 490, 491, 493, 494, 496, 497, 499, 501, 502, 504, 505, 507, 508, 510, 512, 513, 515, 516, 518, 520, 521, 523, 524, 526, 528, 529, 531, 533,
                 534, 536, 537, 539, 541, 542, 544, 546, 547, 549, 551, 552, 554, 556, 558, 559, 561, 563, 564, 566, 568, 570, 571, 573, 575, 577, 578, 580, 582, 584, 585, 587,
                 589, 591, 592, 594, 596, 598, 600, 601, 603, 605, 607, 609, 610, 612, 614, 616, 618, 620, 621, 623, 625, 627, 629, 631, 632, 634, 636, 638, 640, 642, 644, 646,
                 647, 649, 651, 653, 655, 656, 658, 660, 662, 663, 665, 667, 669, 670, 672, 674, 676, 677, 679, 681, 683, 685, 686, 688, 690, 692, 694, 695, 697, 699, 701, 702,
                 704, 706, 708, 710, 711, 713, 715, 717, 719, 720, 722, 724, 726, 728, 730, 731, 733, 735, 737, 739, 740, 742, 744, 746, 748, 750, 751, 753, 755, 757, 759, 761,
                 762, 764, 766, 768, 770, 772, 773, 775, 777, 779, 781, 783, 785, 786, 788, 790, 792, 794, 796, 798, 799, 801, 803, 805, 807, 809, 811, 812, 814, 816, 818, 820,
                 822, 824, 826, 828, 829, 831, 833, 835, 837, 839, 841, 843, 845, 846, 848, 850, 852, 854, 856, 858, 860, 862, 864, 866, 864, 866, 868, 870, 872, 874, 876, 878,
                 880, 882, 884, 886, 888, 890, 892, 894, 896, 898, 900, 902, 904, 906, 908, 910, 912, 914, 916, 918, 920, 922, 923, 925, 927, 929, 931, 933, 935, 937, 939, 941,
                 943, 945, 947, 949, 951, 953, 955, 957, 959, 961, 963, 965, 967, 969, 971, 973, 975, 977, 979, 981, 983, 985, 987, 989, 991, 993, 995, 997, 999,1001,1003,1005,
                1007,1009,1011,1013,1015,1017,1019,1021,1023,1025,1027,1028,1030,1032,1034,1036,1038,1040,1042,1044,1046,1048,1050,1052,1054,1056,1058,1060,1062,1064,1066,1068,
                1070,1072,1074,1076,1078,1080,1082,1084,1086,1088,1090,1092,1094,1096,1098,1100,1102,1104,1106,1108,1110,1112,1114,1116,1118,1120,1122,1124,1126,1129,1131,1133,
                1135,1137,1139,1141,1143,1145,1147,1149,1151,1153,1155,1157,1159,1161,1163,1165,1167,1169,1171,1173,1175,1177,1179,1181,1183,1185,1187,1189,1191,1193,1195,1197,
                1199,1201,1203,1205,1207,1209,1211,1213,1215,1217,1219,1221,1223,1225,1227,1229,1231,1233,1235,1238,1240,1242,1244,1246,1248,1250,1252,1254,1256,1258,1260,1262,
                1264,1266,1268,1270,1272,1274,1276,1278,1280,1282,1284,1286,1288,1290,1292,1294,1297,1299,1301,1303,1305,1307,1309,1311,1313,1315,1317,1319,1321,1323,1325,1327,
                1329,1331,1333,1335,1337,1339,1341,1344,1346,1348,1350,1352,1354,1356,1358,1360,1362,1364,1366,1368,1370,1372,1374,1376,1378,1380,1382,1385,1387,1389,1391,1393,
                1395,1397,1399,1401,1403,1405,1407,1409,1411,1413,1415,1417,1420,1422,1424,1426,1428,1430,1432,1434,1436,1438,1440,1442,1444,1446,1448,1451,1453,1455,1457,1459,
                1461,1463,1465,1467,1469,1471,1473,1475,1477,1479,1482,1484,1486,1488,1490,1492,1494,1496,1498,1500,1502,1504,1506,1509,1511,1513,1515,1517,1519,1521,1523,1525,
                1527,1529,1531,1533,1536,1538,1540,1542,1544,1546,1548,1550,1552,1554,1556,1558,1561,1563,1565,1567,1569,1571,1573,1575,1577,1579,1581,1584,1586,1588,1590,1592,
                1594,1596,1598,1600,1602,1604,1607,1609,1611,1613,1615,1617,1619,1621,1623,1625,1627,1630,1632,1634,1636,1638,1640,1642,1644,1646,1648,1651,1653,1655,1657,1659,
                1661,1663,1665,1667,1670,1672,1674,1676,1678,1680,1682,1684,1686,1688,1691,1693,1695,1697,1699,1701,1703,1705,1707,1710,1712,1714,1716,1718,1720,1722,1724,1726,
                1729,1731,1733,1735,1737,1739,1741,1743,1746,1748,1750,1752,1754,1756,1758,1760,1763,1766,1768,1770,1772,1774,1776,1778,1781,1783,1785,1787,1789,1791,1793,1795,
                1797,1800,1802,1804,1806,1808,1810,1812,1814,1816,1818,1821,1823,1825,1827,1829,1831,1833,1835,1837,1839,1841,1843,1846,1848,1850,1852,1854,1856,1858,1860,1862,
                1864,1866,1868,1870,1872,1874,1876,1879,1881,1883,1885,1887,1889,1891,1893,1895,1897,1899,1901,1903,1905,1907,1909,1911,1913,1915,1917,1919,1921,1923,1925,1927,
                1929,1931,1933,1935,1937,1939,1941,1943,1945,1947,1949,1951,1953,1955,1957,1959,1961,1963,1965,1967,1969,1971,1973,1975,1977,1979,1981,1983,1985,1987,1989,1991,
                1993,1995,1997,1999,2001,2003,2005,2007,2009,2011,2013,2015,2017,2019,2021,2023,2025,2026,2028,2030,2032,2034,2036,2038,2040,2042,2044,2046,2048,2050,2052,2054,
                2056,2057,2059,2061,2063,2065,2067,2069,2071,2073,2075,2077,2079,2080,2082,2084,2086,2088,2090,2092,2094,2096,2098,2099,2101,2103,2105,2107,2109,2111,2113,2115,
                2116,2118,2120,2122,2124,2126,2128,2130,2131,2133,2135,2137,2139,2141,2143,2144,2146,2148,2150,2152,2154,2156,2157,2159,2161,2163,2165,2167,2168,2170,2172,2174,
                2176,2178,2179,2181,2183,2185,2187,2189,2190,2192,2194,2196,2198,2200,2201,2203,2205,2207,2209,2210,2212,2214,2216,2218,2219,2221,2223,2225,2227,2228,2230,2232,
                2234,2236,2237,2239,2241,2243,2244,2246,2248,2250,2252,2253,2255,2257,2259,2260,2262,2264,2266,2267,2269,2271,2273,2274,2276,2278,2280,2281,2283,2285,2287,2288,
                2290,2292,2294,2295,2297,2299,2301,2302,2304,2306,2308,2309,2311,2313,2314,2316,2318,2320,2321,2323,2325,2326,2328,2330,2332,2333,2335,2337,2338,2340,2342,2344,
                2345,2347,2349,2350,2352,2354,2355,2357,2359,2360,2362,2364,2365,2367,2369,2370,2372,2374,2375,2377,2379,2380,2382,2384,2385,2387,2389,2390,2392,2394,2395,2397,
                2399,2400,2402,2404,2405,2407,2409,2410,2412,2414,2415,2417,2418,2420,2422,2423,2425,2427,2428,2430,2431,2433,2435,2436,2438,2440,2441,2443,2444,2446,2448,2449,
                2451,2452,2454,2456,2457,2459,2460,2462,2464,2465,2467,2468,2470,2472,2473,2475,2476,2478,2479,2481,2483,2484,2486,2487,2489,2490,2492,2494,2495,2497,2498,2500,
                2501,2503,2505,2506,2508,2509,2511,2512,2514,2515,2517,2518,2520,2522,2523,2525,2526,2528,2529,2531,2532,2534,2535,2537,2538,2540,2541,2543,2544,2546,2548,2549,
                2551,2552,2554,2555,2557,2558,2560,2561,2563,2564,2566,2567,2569,2570,2572,2573,2575,2576,2578,2579,2581,2582,2583,2585,2586,2588,2589,2591,2592,2594,2595,2597,
                2598,2600,2601,2603,2604,2606,2607,2608,2610,2611,2613,2614,2616,2617,2619,2620,2622,2623,2624,2626,2627,2629,2630,2632,2633,2634,2636,2637,2639,2640,2642,2643,
                2644,2646,2647,2649,2650,2652,2653,2654,2656,2657,2659,2660,2661,2663,2664,2666,2667,2668,2670,2671,2673,2674,2675,2677,2678,2680,2681,2682,2684,2685,2686,2688,
                2689,2691,2692,2693,2695,2696,2697,2699,2700,2702,2703,2704,2706,2707,2708,2710,2711,2712,2714,2715,2716,2718,2719,2720,2722,2723,2724,2726,2727,2728,2730,2731,
                2732,2734,2735,2736,2738,2739,2740,2742,2743,2744,2746,2747,2748,2750,2751,2752,2754,2755,2756,2757,2759,2760,2761,2763,2764,2765,2767,2768,2769,2770,2772,2773,
                2774,2776,2777,2778,2779,2781,2782,2783,2785,2786,2787,2788,2789,2791,2792,2793,2794,2795,2797,2798,2799,2800,2801,2803,2804,2805,2806,2807,2809,2810,2811,2812,
                2813,2815,2816,2817,2818,2819,2820,2822,2823,2824,2825,2826,2828,2829,2830,2831,2832,2834,2835,2836,2837,2838,2840,2841,2842,2843,2844,2845,2847,2848,2849,2850,
                2851,2853,2854,2855,2856,2857,2859,2860,2861,2862,2863,2864,2866,2867,2868,2869,2870,2872,2873,2874,2875,2876,2877,2879,2880,2881,2882,2883,2884,2886,2887,2888,
                2889,2890,2891,2893,2894,2895,2896,2897,2899,2900,2901,2902,2903,2904,2906,2907,2908,2909,2910,2911,2913,2914,2915,2916,2917,2918,2920,2921,2922,2923,2924,2925,
                2927,2928,2929,2930,2931,2932,2933,2935,2936,2937,2938,2939,2940,2942,2943,2944,2945,2946,2947,2949,2950,2951,2952,2953,2954,2955,2957,2958,2959,2960,2961,2962,
                2964,2965,2966,2967,2968,2969,2970,2972,2973,2974,2975,2976,2977,2978,2980,2981,2982,2983,2984,2985,2986,2988,2989,2990,2991,2992,2993,2994,2996,2997,2998,2999,
                3000,3001,3002,3004,3005,3006,3007,3008,3009,3010,3012,3013,3014,3015,3016,3017,3018,3019,3021,3022,3023,3024,3025,3026,3027,3029,3030,3031,3032,3033,3034,3035,
                3036,3038,3039,3040,3041,3042,3043,3044,3045,3047,3048,3049,3050,3051,3052,3053,3054,3056,3057,3058,3059,3060,3061,3062,3063,3065,3066,3067,3068,3069,3070,3071,
                3072,3073,3075,3076,3077,3078,3079,3080,3081,3082,3083,3085,3086,3087,3088,3089,3090,3091,3092,3093,3095,3096,3097,3098,3099,3100,3101,3102,3103,3105,3106,3107,
                3108,3109,3110,3111,3112,3113,3114,3116,3117,3118,3119,3120,3121,3122,3123,3124,3126,3127,3128,3129,3130,3130,3131,3132,3133,3134,3135,3136,3136,3137,3138,3139,
                3140,3141,3142,3143,3143,3144,3145,3146,3147,3148,3149,3149,3150,3151,3152,3153,3154,3155,3156,3156,3157,3158,3159,3160,3161,3162,3162,3163,3164,3165,3166,3167,
                3168,3168,3169,3170,3171,3172,3173,3173,3174,3175,3176,3177,3178,3179,3179,3180,3181,3182,3183,3184,3185,3185,3186,3187,3188,3189,3190,3190,3191,3192,3193,3194,
                3195,3196,3196,3197,3198,3199,3200,3201,3201,3202,3203,3204,3205,3206,3206,3207,3208,3209,3210,3211,3211,3212,3213,3214,3215,3216,3216,3217,3218,3219,3220,3221,
                3221,3222,3223,3224,3225,3226,3226,3227,3228,3229,3230,3230,3231,3232,3233,3234,3235,3235,3236,3237,3238,3239,3240,3240,3241,3242,3243,3244,3244,3245,3246,3247,
                3248,3249,3249,3250,3251,3252,3253,3253,3254,3255,3256,3257,3257,3258,3259,3260,3261,3262,3262,3263,3264,3265,3266,3266,3267,3268,3269,3270,3270,3271,3272,3273,
                3274,3274,3275,3276,3277,3278,3278,3279,3280,3281,3282,3282,3283,3284,3285,3286,3286,3287,3288,3289,3290,3290,3291,3292,3293,3294,3294,3295,3296,3297,3298,3298,
                3299,3300,3301,3302,3302,3303,3304,3305,3305,3306,3307,3308,3309,3309,3310,3311,3312,3313,3313,3314,3315,3316,3316,3317,3318,3319,3320,3320,3321,3322,3323,3323,
                3324,3325,3326,3327,3327,3328,3329,3330,3330,3331,3332,3333,3334,3334,3335,3336,3337,3337,3338,3339,3340,3340,3341,3342,3343,3344,3344,3345,3346,3347,3347,3348,
                3349,3350,3350,3351,3352,3353,3353,3354,3355,3356,3357,3357,3358,3359,3360,3360,3361,3362,3363,3363,3364,3365,3366,3366,3367,3368,3369,3369,3370,3371,3372,3372,
                3373,3374,3375,3375,3376,3377,3378,3378,3379,3380,3381,3381,3382,3383,3384,3384,3385,3386,3387,3387,3388,3389,3390,3390,3391,3392,3393,3393,3394,3395,3396,3396,
                3397,3398,3398,3399,3400,3401,3401,3402,3403,3404,3404,3405,3406,3407,3407,3408,3409,3409,3410,3411,3412,3412,3413,3414,3415,3415,3416,3417,3418,3418,3419,3420,
                3420,3421,3422,3423,3423,3424,3425,3425,3426,3427,3428,3428,3429,3430,3431,3431,3432,3433,3433,3434,3435,3436,3436,3437,3438,3438,3439,3440,3441,3441,3442,3443,
                3443,3444,3445,3446,3446,3447,3448,3448,3449,3450,3451,3451,3452,3453,3453,3454,3455,3456,3456,3457,3458,3458,3459,3460,3460,3461,3462,3463,3463,3464,3465,3465,
                3466,3467,3467,3468,3469,3470,3470,3471,3472,3472,3473,3474,3474,3475,3476,3477,3477,3478,3479,3479,3480,3481,3481,3482,3483,3483,3484,3485,3485,3486,3487,3488,
                3488,3489,3490,3490,3491,3492,3492,3493,3494,3494,3495,3496,3496,3497,3498,3499,3499,3500,3501,3501,3502,3503,3503,3504,3505,3505,3506,3507,3507,3508,3509,3509,
                3510,3511,3511,3512,3513,3513,3514,3515,3515,3516,3517,3517,3518,3519,3519,3520,3521,3521,3522,3523,3523,3524,3525,3525,3526,3527,3527,3528,3529,3529,3530,3531,
                3531,3532,3533,3533,3534,3535,3535,3536,3537,3537,3538,3539,3539,3540,3541,3541,3542,3543,3543,3544,3545,3545,3546,3546,3547,3548,3548,3549,3550,3550,3551,3552,
                3552,3553,3554,3554,3555,3556,3556,3557,3558,3558,3559,3559,3560,3561,3561,3562,3563,3563,3564,3565,3565,3566,3567,3567,3568,3568,3569,3570,3570,3571,3572,3572,
                3573,3574,3574,3575,3575,3576,3577,3577,3578,3579,3579,3580,3580,3581,3582,3582,3583,3584,3584,3585,3586,3586,3587,3587,3588,3589,3589,3590,3591,3591,3592,3592,
                3593,3594,3594,3595,3595,3596,3597,3597,3598,3599,3599,3600,3600,3601,3602,3602,3603,3604,3604,3605,3605,3606,3607,3607,3608,3608,3609,3610,3610,3611,3611,3612,
                3613,3613,3614,3615,3615,3616,3616,3617,3618,3618,3619,3619,3620,3621,3621,3622,3622,3623,3624,3624,3625,3625,3626,3627,3627,3628,3628,3629,3630,3630,3631,3631,
                3632,3633,3633,3634,3634,3635,3636,3636,3637,3637,3638,3639,3639,3640,3640,3641,3642,3642,3643,3643,3644,3644,3645,3646,3646,3647,3647,3648,3649,3649,3650,3650,
                3651,3651,3652,3653,3653,3654,3654,3655,3656,3656,3657,3657,3658,3658,3659,3660,3660,3661,3661,3662,3662,3663,3664,3664,3665,3665,3666,3667,3667,3668,3668,3669,
                3669,3670,3671,3671,3672,3672,3673,3673,3674,3674,3675,3676,3676,3677,3677,3678,3678,3679,3680,3680,3681,3681,3682,3682,3683,3684,3684,3685,3685,3686,3686,3687,
                3687,3688,3689,3689,3690,3690,3691,3691,3692,3692,3693,3694,3694,3695,3695,3696,3696,3697,3697,3698,3699,3700,3700,3700,3701,3701,3702,3702,3702,3703,3703,3703,
                3704,3704,3705,3705,3705,3706,3706,3706,3707,3707,3708,3708,3708,3709,3709,3709,3710,3710,3711,3711,3711,3712,3712,3712,3713,3713,3713,3714,3714,3715,3715,3715,
                3716,3716,3716,3717,3717,3718,3718,3718,3719,3719,3719,3720,3720,3721,3721,3721,3722,3722,3722,3723,3723,3724,3724,3724,3725,3725,3725,3726,3726,3726,3727,3727,
                3728,3728,3728,3729,3729,3729,3730,3730,3731,3731,3731,3732,3732,3732,3733,3733,3733,3734,3734,3735,3735,3735,3736,3736,3736,3737,3737,3737,3738,3738,3739,3739,
                3739,3740,3740,3740,3741,3741,3741,3742,3742,3743,3743,3743,3744,3744,3744,3745,3745,3746,3746,3746,3747,3747,3747,3748,3748,3748,3749,3749,3750,3750,3750,3751,
                3751,3751,3752,3752,3752,3753,3753,3754,3754,3754,3755,3755,3755,3756,3756,3756,3757,3757,3757,3758,3758,3759,3759,3759,3760,3760,3760,3761,3761,3761,3762,3762,
                3763,3763,3763,3764,3764,3764,3765,3765,3765,3766,3766,3766,3767,3767,3768,3768,3768,3769,3769,3769,3770,3770,3770,3771,3771,3772,3772,3772,3773,3773,3773,3774,
                3774,3774,3775,3775,3775,3776,3776,3777,3777,3777,3778,3778,3778,3779,3779,3779,3780,3780,3780,3781,3781,3782,3782,3782,3783,3783,3783,3784,3784,3784,3785,3785,
                3785,3786,3786,3786,3787,3787,3788,3788,3788,3789,3789,3789,3790,3790,3790,3791,3791,3791,3792,3792,3793,3793,3793,3794,3794,3794,3795,3795,3795,3796,3796,3796,
                3797,3797,3797,3798,3798,3799,3799,3799,3800,3800,3800,3801,3801,3801,3802,3802,3802,3803,3803,3803,3804,3804,3804,3805,3805,3806,3806,3806,3807,3807,3807,3808,
                3808,3808,3809,3809,3809,3810,3810,3810,3811,3811,3811,3812,3812,3813,3813,3813,3814,3814,3814,3815,3815,3815,3816,3816,3816,3817,3817,3817,3818,3818,3818,3819,
                3819,3819,3820,3820,3821,3821,3821,3822,3822,3822,3823,3823,3823,3824,3824,3824,3825,3825,3825,3826,3826,3826,3827,3827,3827,3828,3828,3829,3829,3829,3830,3830,
                3830,3831,3831,3831,3832,3832,3832,3833,3833,3833,3834,3834,3834,3835,3835,3835,3836,3836,3836,3837,3837,3837,3838,3838,3838,3839,3839,3840,3840,3840,3841,3841,
                3841,3842,3842,3842,3843,3843,3843,3844,3844,3844,3845,3845,3845,3846,3846,3846,3847,3847,3847,3848,3848,3848,3849,3849,3849,3850,3850,3850,3851,3851,3851,3852,
                3852,3853,3853,3853,3854,3854,3854,3855,3855,3855,3856,3856,3856,3857,3857,3857,3858,3858,3858,3859,3859,3859,3860,3860,3860,3861,3861,3861,3862,3862,3862,3863,
                3863,3863,3864,3864,3864,3865,3865,3865,3866,3866,3866,3867,3867,3867,3868,3868,3868,3869,3869,3869,3870,3870,3870,3871,3871,3871,3872,3872,3872,3873,3873,3873,
                3874,3874,3874,3875,3875,3876,3876,3876,3877,3877,3877,3878,3878,3878,3879,3879,3879,3880,3880,3880,3881,3881,3881,3882,3882,3882,3883,3883,3883,3884,3884,3884,
                3885,3885,3885,3886,3886,3886,3887,3887,3887,3888,3888,3888,3889,3889,3889,3890,3890,3890,3891,3891,3891,3892,3892,3892,3893,3893,3893,3894,3894,3894,3895,3895,
                3895,3896,3896,3896,3897,3897,3897,3897,3898,3898,3898,3899,3899,3899,3900,3900,3900,3901,3901,3901,3902,3902,3902,3903,3903,3903,3904,3904,3904,3905,3905,3905,
                3906,3906,3906,3907,3907,3907,3908,3908,3908,3909,3909,3909,3910,3910,3910,3911,3911,3911,3912,3912,3912,3913,3913,3913,3914,3914,3914,3915,3915,3915,3916,3916,
                3916,3917,3917,3917,3918,3918,3918,3919,3919,3919,3920,3920,3920,3921,3921,3921,3921,3922,3922,3922,3923,3923,3923,3924,3924,3924,3925,3925,3925,3926,3926,3926,
                3927,3927,3927,3928,3928,3928,3929,3929,3929,3930,3930,3930,3931,3931,3931,3932,3932,3932,3933,3933,3933,3934,3934,3934,3934,3935,3935,3935,3936,3936,3936,3937,
                3937,3937,3938,3938,3938,3939,3939,3939,3940,3940,3940,3941,3941,3941,3942,3942,3942,3943,3943,3943,3944,3944,3944,3944,3945,3945,3945,3946,3946,3946,3947,3947,
                3947,3948,3948,3948,3949,3949,3949,3950,3950,3950,3951,3951,3951,3952,3952,3952,3953,3953,3953,3953,3954,3954,3954,3955,3955,3955,3956,3956,3956,3957,3957,3957,
                3958,3958,3958,3959,3959,3959,3960,3960,3960,3960,3961,3961,3961,3962,3962,3962,3963,3963,3963,3964,3964,3964,3965,3965,3965,3966,3966,3966,3967,3967,3967,3967,
                3968,3968,3968,3969,3969,3969,3970,3970,3970,3971,3971,3971,3972,3972,3972,3973,3973,3973,3974,3974,3974,3974,3975,3975,3975,3976,3976,3976,3977,3977,3977,3978,
                3978,3978,3979,3979,3979,3979,3980,3980,3980,3981,3981,3981,3982,3982,3982,3983,3983,3983,3984,3984,3984,3985,3985,3985,3985,3986,3986,3986,3987,3987,3987,3988,
                3988,3988,3989,3989,3989,3990,3990,3990,3990,3991,3991,3991,3992,3992,3992,3993,3993,3993,3994,3994,3994,3995,3995,3995,3995,3996,3996,3996,3997,3997,3997,3998,
                3998,3998,3999,3999,3999,4000,4000,4000,4000,4001,4001,4001,4002,4002,4002,4003,4003,4003,4004,4004,4004,4005,4005,4005,4005,4006,4006,4006,4007,4007,4007,4008,
                4008,4008,4009,4009,4009,4009,4010,4010,4010,4011,4011,4011,4012,4012,4012,4013,4013,4013,4013,4014,4014,4014,4015,4015,4015,4016,4016,4016,4017,4017,4017,4017,
                4018,4018,4018,4019,4019,4019,4020,4020,4020,4021,4021,4021,4021,4022,4022,4022,4023,4023,4023,4024,4024,4024,4025,4025,4025,4025,4026,4026,4026,4027,4027,4027,
                4028,4028,4028,4029,4029,4029,4029,4030,4030,4030,4031,4031,4031,4032,4032,4032,4032,4033,4033,4033,4034,4034,4034,4035,4035,4035,4036,4036,4036,4036,4037,4037,
                4037,4038,4038,4038,4039,4039,4039,4040,4040,4040,4040,4041,4041,4041,4042,4042,4042,4043,4043,4043,4043,4044,4044,4044,4045,4045,4045,4046,4046,4046,4046,4047,
                4047,4047,4048,4048,4048,4049,4049,4049,4049,4050,4050,4050,4051,4051,4051,4052,4052,4052,4053,4053,4053,4053,4054,4054,4054,4055,4055,4055,4056,4056,4056,4056,
                4057,4057,4057,4058,4058,4058,4059,4059,4059,4059,4060,4060,4060,4061,4061,4061,4062,4062,4062,4062,4063,4063,4063,4064,4064,4064,4065,4065,4065,4065,4066,4066,
                4066,4067,4067,4067,4068,4068,4068,4068,4069,4069,4069,4070,4070,4070,4071,4071,4071,4071,4072,4072,4072,4073,4073,4073,4073,4074,4074,4074,4075,4075,4075,4076,
                4076,4076,4076,4077,4077,4077,4078,4078,4078,4079,4079,4079,4079,4080,4080,4080,4081,4081,4081,4082,4082,4082,4082,4083,4083,4083,4084,4084,4084,4084,4085,4085,
                4085,4086,4086,4086,4087,4087,4087,4087,4088,4088,4088,4089,4089,4089,4089,4090,4090,4090,4091,4091,4091,4092,4092,4092,4092,4093,4093,4093,4094,4094,4094,4095,
          },
          /* aec trigger 4 */
          {
                   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
                   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
                   1,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
                   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
                   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,
                   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,
                   6,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,
                   8,   8,   8,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,
                  11,  11,  11,  11,  11,  11,  11,  11,  11,  11,  11,  11,  11,  11,  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,  13,  13,  13,  13,  13,
                  13,  13,  13,  13,  13,  13,  13,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,  16,  16,
                  16,  16,  16,  16,  16,  16,  16,  16,  16,  17,  17,  17,  17,  17,  17,  17,  17,  17,  17,  17,  18,  18,  18,  18,  18,  18,  18,  18,  18,  18,  19,  19,
                  19,  19,  19,  19,  19,  19,  19,  19,  20,  20,  20,  20,  20,  20,  20,  20,  20,  21,  21,  21,  21,  21,  21,  21,  21,  21,  21,  22,  22,  22,  22,  22,
                  22,  22,  22,  22,  23,  23,  23,  23,  23,  23,  23,  23,  23,  24,  24,  24,  24,  24,  24,  24,  24,  25,  25,  25,  25,  25,  25,  25,  25,  25,  26,  26,
                  26,  26,  26,  26,  26,  26,  27,  27,  27,  27,  27,  27,  27,  27,  28,  28,  28,  28,  28,  28,  28,  28,  29,  29,  29,  29,  29,  29,  29,  29,  30,  30,
                  30,  30,  30,  30,  30,  31,  31,  31,  31,  31,  31,  31,  31,  32,  32,  32,  32,  32,  32,  32,  33,  33,  33,  33,  33,  33,  33,  34,  34,  34,  34,  34,
                  34,  34,  35,  35,  35,  35,  35,  35,  35,  36,  36,  36,  36,  36,  36,  36,  37,  37,  37,  37,  37,  37,  38,  38,  38,  38,  38,  38,  38,  39,  39,  39,
                  39,  39,  39,  40,  40,  40,  40,  40,  40,  40,  41,  41,  41,  41,  41,  41,  42,  42,  42,  42,  42,  42,  43,  43,  43,  43,  43,  43,  44,  44,  44,  44,
                  44,  44,  45,  45,  45,  45,  45,  45,  46,  46,  46,  46,  46,  46,  47,  47,  47,  47,  47,  47,  48,  48,  48,  48,  48,  48,  49,  49,  49,  49,  49,  50,
                  50,  50,  50,  50,  50,  51,  51,  51,  51,  51,  51,  52,  52,  52,  52,  52,  53,  53,  53,  53,  53,  54,  54,  54,  54,  54,  54,  55,  55,  55,  55,  55,
                  56,  56,  56,  56,  56,  57,  57,  57,  57,  57,  58,  58,  58,  58,  58,  58,  59,  59,  59,  59,  59,  60,  60,  60,  60,  60,  60,  60,  60,  61,  61,  61,
                  61,  61,  62,  62,  62,  62,  63,  63,  63,  63,  64,  64,  64,  64,  65,  65,  65,  66,  66,  66,  66,  67,  67,  67,  68,  68,  68,  68,  69,  69,  69,  70,
                  70,  70,  71,  71,  71,  72,  72,  72,  73,  73,  73,  74,  74,  74,  75,  75,  75,  76,  76,  77,  77,  77,  78,  78,  78,  79,  79,  80,  80,  80,  81,  81,
                  82,  82,  82,  83,  83,  84,  84,  84,  85,  85,  86,  86,  87,  87,  88,  88,  88,  89,  89,  90,  90,  91,  91,  92,  92,  93,  93,  94,  94,  94,  95,  95,
                  96,  96,  97,  97,  98,  98,  99,  99, 100, 101, 101, 102, 102, 103, 103, 104, 104, 105, 105, 106, 106, 107, 107, 108, 109, 109, 110, 110, 111, 111, 112, 113,
                 113, 114, 114, 115, 116, 116, 117, 117, 118, 119, 119, 120, 120, 121, 122, 122, 123, 123, 124, 125, 125, 126, 127, 127, 128, 129, 129, 130, 131, 131, 132, 133,
                 133, 134, 135, 135, 136, 137, 137, 138, 139, 139, 140, 141, 141, 142, 143, 144, 144, 145, 146, 146, 147, 148, 149, 149, 150, 151, 152, 152, 153, 154, 155, 155,
                 156, 157, 158, 158, 159, 160, 161, 161, 162, 163, 164, 165, 165, 166, 167, 168, 169, 169, 170, 171, 172, 173, 173, 174, 175, 176, 177, 178, 178, 179, 180, 181,
                 182, 183, 183, 184, 185, 186, 187, 188, 189, 190, 190, 191, 192, 193, 194, 195, 196, 197, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 207, 208, 209,
                 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241,
                 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 274, 275,
                 276, 277, 278, 279, 280, 281, 282, 284, 285, 286, 287, 288, 289, 290, 292, 293, 294, 295, 296, 297, 299, 300, 301, 302, 303, 304, 306, 307, 308, 309, 310, 312,
                 313, 314, 315, 316, 318, 319, 320, 321, 322, 324, 325, 326, 327, 329, 330, 331, 332, 334, 335, 336, 337, 339, 340, 341, 342, 344, 345, 346, 347, 349, 350, 351,
                 353, 354, 355, 356, 358, 359, 360, 362, 363, 364, 366, 367, 368, 369, 371, 372, 373, 375, 376, 377, 379, 380, 382, 383, 384, 386, 387, 388, 390, 391, 392, 394,
                 395, 397, 398, 399, 401, 402, 403, 405, 406, 408, 409, 410, 412, 413, 415, 416, 418, 420, 422, 424, 426, 427, 429, 431, 433, 434, 436, 438, 440, 442, 444, 445,
                 447, 449, 451, 453, 455, 456, 458, 460, 462, 464, 466, 468, 470, 472, 473, 475, 477, 479, 481, 483, 485, 487, 489, 491, 493, 495, 497, 499, 501, 503, 505, 507,
                 509, 511, 513, 515, 517, 519, 521, 523, 525, 527, 529, 531, 534, 536, 538, 540, 542, 544, 546, 548, 550, 553, 555, 557, 559, 561, 563, 566, 568, 570, 572, 574,
                 576, 579, 581, 583, 585, 588, 590, 592, 594, 596, 599, 601, 603, 606, 608, 610, 612, 615, 617, 619, 622, 624, 626, 629, 631, 633, 636, 638, 640, 643, 645, 647,
                 650, 652, 654, 657, 659, 662, 664, 667, 669, 671, 674, 676, 679, 681, 684, 686, 689, 691, 693, 696, 698, 701, 703, 706, 708, 711, 713, 716, 719, 721, 724, 726,
                 729, 731, 734, 736, 739, 742, 744, 747, 749, 752, 755, 757, 760, 763, 765, 768, 770, 773, 776, 778, 781, 784, 786, 789, 792, 795, 797, 800, 803, 805, 808, 811,
                 814, 816, 819, 822, 825, 827, 830, 833, 836, 838, 841, 844, 847, 850, 852, 855, 858, 861, 864, 867, 869, 872, 875, 878, 881, 884, 887, 890, 892, 895, 898, 901,
                 904, 907, 910, 913, 916, 919, 922, 925, 928, 931, 934, 937, 940, 943, 946, 949, 952, 955, 958, 961, 964, 967, 970, 973, 976, 979, 982, 985, 988, 991, 994, 997,
                1000,1004,1007,1010,1013,1016,1019,1022,1025,1029,1032,1035,1038,1041,1044,1048,1051,1054,1057,1060,1064,1067,1070,1073,1076,1080,1083,1086,1089,1093,1096,1099,
                1102,1106,1109,1112,1116,1119,1122,1126,1129,1132,1136,1139,1142,1146,1149,1152,1156,1159,1162,1166,1169,1173,1176,1179,1183,1186,1190,1193,1196,1199,1202,1205,
                1208,1211,1214,1217,1220,1223,1227,1230,1233,1236,1239,1242,1245,1248,1251,1254,1257,1260,1263,1266,1269,1272,1275,1278,1281,1284,1287,1290,1293,1296,1299,1302,
                1305,1308,1311,1314,1317,1320,1323,1326,1329,1332,1335,1338,1341,1344,1347,1350,1353,1356,1359,1362,1365,1367,1370,1373,1376,1379,1382,1385,1388,1391,1394,1396,
                1399,1402,1405,1408,1411,1414,1417,1419,1422,1425,1428,1431,1434,1436,1439,1442,1445,1448,1451,1453,1456,1459,1462,1465,1467,1470,1473,1476,1478,1481,1484,1487,
                1489,1492,1495,1498,1501,1503,1506,1509,1511,1514,1517,1520,1522,1525,1528,1530,1533,1536,1539,1541,1544,1547,1549,1552,1555,1557,1560,1563,1565,1568,1571,1573,
                1576,1579,1581,1584,1587,1589,1592,1594,1597,1600,1602,1605,1607,1610,1613,1615,1618,1620,1623,1626,1628,1631,1633,1636,1638,1641,1644,1646,1649,1651,1654,1656,
                1659,1661,1664,1666,1669,1671,1674,1676,1679,1681,1684,1686,1689,1691,1694,1696,1699,1701,1704,1706,1709,1711,1714,1716,1718,1721,1723,1726,1728,1731,1733,1735,
                1738,1740,1743,1745,1747,1750,1752,1755,1757,1759,1762,1764,1767,1769,1771,1774,1776,1778,1781,1783,1785,1788,1790,1792,1795,1797,1799,1802,1804,1806,1809,1811,
                1813,1816,1818,1820,1822,1825,1827,1829,1832,1834,1836,1838,1841,1843,1845,1847,1850,1852,1854,1856,1859,1861,1863,1865,1867,1870,1872,1874,1876,1878,1881,1883,
                1885,1887,1889,1892,1894,1896,1898,1900,1902,1905,1907,1909,1911,1913,1915,1917,1920,1922,1924,1926,1928,1930,1932,1934,1936,1938,1941,1943,1945,1947,1949,1951,
                1953,1955,1957,1959,1961,1963,1965,1967,1968,1970,1972,1974,1976,1978,1980,1982,1985,1987,1989,1991,1993,1995,1997,1999,2001,2004,2006,2008,2010,2012,2014,2016,
                2018,2020,2022,2024,2026,2029,2031,2033,2035,2037,2039,2041,2043,2045,2047,2049,2051,2053,2055,2057,2059,2061,2063,2065,2067,2069,2071,2073,2075,2077,2079,2081,
                2083,2085,2087,2089,2091,2093,2095,2097,2099,2101,2103,2105,2107,2109,2111,2113,2115,2117,2119,2121,2123,2125,2127,2129,2131,2133,2135,2137,2139,2140,2142,2144,
                2146,2148,2150,2152,2154,2156,2158,2160,2162,2163,2165,2167,2169,2171,2173,2175,2177,2179,2181,2182,2184,2186,2188,2190,2192,2194,2195,2197,2199,2201,2203,2205,
                2207,2208,2210,2212,2214,2216,2218,2219,2221,2223,2225,2227,2229,2230,2232,2234,2236,2238,2239,2241,2243,2245,2247,2248,2250,2252,2254,2256,2257,2259,2261,2263,
                2264,2266,2268,2270,2271,2273,2275,2277,2278,2280,2282,2284,2285,2287,2289,2291,2292,2294,2296,2298,2299,2301,2303,2304,2306,2308,2310,2311,2313,2315,2316,2318,
                2320,2321,2323,2325,2326,2328,2330,2332,2333,2335,2337,2338,2340,2342,2343,2345,2346,2348,2350,2351,2353,2355,2356,2358,2360,2361,2363,2364,2366,2368,2369,2371,
                2373,2374,2376,2377,2379,2381,2382,2384,2385,2387,2389,2390,2392,2393,2395,2396,2398,2400,2401,2403,2404,2406,2407,2409,2411,2412,2414,2415,2417,2418,2420,2421,
                2423,2424,2426,2427,2429,2431,2432,2434,2435,2437,2438,2440,2441,2443,2444,2446,2447,2449,2450,2452,2453,2455,2456,2458,2459,2460,2462,2463,2465,2466,2468,2469,
                2471,2472,2474,2475,2477,2478,2479,2481,2482,2484,2485,2487,2488,2489,2491,2492,2494,2495,2496,2498,2498,2500,2501,2502,2503,2504,2505,2506,2507,2508,2509,2510,
                2512,2513,2514,2515,2516,2517,2518,2519,2520,2521,2522,2523,2524,2526,2527,2528,2529,2530,2531,2532,2533,2534,2535,2536,2537,2538,2540,2541,2542,2543,2544,2545,
                2546,2547,2548,2549,2550,2551,2552,2553,2554,2556,2557,2558,2559,2560,2561,2562,2563,2564,2565,2566,2567,2568,2569,2570,2571,2573,2574,2575,2576,2577,2578,2579,
                2580,2581,2582,2583,2584,2585,2586,2587,2588,2589,2590,2592,2593,2594,2595,2596,2597,2598,2599,2600,2601,2602,2603,2604,2605,2606,2607,2608,2609,2610,2611,2612,
                2614,2615,2616,2617,2618,2619,2620,2621,2622,2623,2624,2625,2626,2627,2628,2629,2630,2631,2632,2633,2634,2635,2636,2637,2638,2639,2640,2642,2643,2644,2645,2646,
                2647,2648,2649,2650,2651,2652,2653,2654,2655,2656,2657,2658,2659,2660,2661,2662,2663,2664,2665,2666,2667,2668,2669,2670,2671,2672,2673,2674,2675,2676,2677,2678,
                2679,2680,2681,2682,2683,2684,2685,2686,2688,2689,2690,2691,2692,2693,2694,2695,2696,2697,2698,2699,2700,2701,2702,2703,2704,2705,2706,2707,2708,2709,2710,2711,
                2712,2713,2714,2715,2716,2717,2718,2719,2720,2721,2722,2723,2724,2725,2726,2727,2728,2729,2730,2731,2732,2733,2734,2735,2736,2737,2738,2739,2740,2741,2742,2743,
                2744,2745,2746,2747,2748,2749,2750,2751,2751,2752,2753,2754,2755,2756,2757,2758,2759,2760,2761,2762,2763,2764,2765,2766,2767,2768,2769,2770,2771,2772,2773,2774,
                2775,2776,2777,2778,2779,2780,2781,2782,2783,2784,2785,2786,2787,2788,2789,2790,2791,2792,2793,2794,2795,2795,2796,2797,2798,2799,2800,2801,2802,2803,2804,2805,
                2806,2807,2808,2809,2810,2811,2812,2813,2814,2815,2816,2817,2818,2819,2820,2821,2821,2822,2823,2824,2825,2826,2827,2828,2829,2830,2831,2832,2833,2834,2835,2836,
                2837,2838,2839,2840,2841,2841,2842,2843,2844,2845,2846,2847,2848,2849,2850,2851,2852,2853,2854,2855,2856,2857,2858,2858,2859,2860,2861,2862,2863,2864,2865,2866,
                2867,2868,2869,2870,2871,2872,2873,2873,2874,2875,2876,2877,2878,2879,2880,2881,2882,2883,2884,2885,2886,2887,2887,2888,2889,2890,2891,2892,2893,2894,2895,2896,
                2897,2898,2899,2899,2899,2900,2901,2902,2903,2903,2904,2905,2906,2906,2907,2908,2909,2909,2910,2911,2912,2912,2913,2914,2915,2916,2916,2917,2918,2919,2919,2920,
                2921,2922,2922,2923,2924,2925,2925,2926,2927,2928,2928,2929,2930,2931,2931,2932,2933,2934,2935,2935,2936,2937,2938,2938,2939,2940,2941,2941,2942,2943,2944,2944,
                2945,2946,2947,2947,2948,2949,2950,2950,2951,2952,2953,2953,2954,2955,2956,2956,2957,2958,2959,2959,2960,2961,2962,2963,2963,2964,2965,2966,2966,2967,2968,2969,
                2969,2970,2971,2972,2972,2973,2974,2975,2975,2976,2977,2978,2978,2979,2980,2981,2981,2982,2983,2984,2984,2985,2986,2987,2987,2988,2989,2990,2990,2991,2992,2993,
                2993,2994,2995,2996,2996,2997,2998,2999,2999,3000,3001,3002,3002,3003,3004,3005,3005,3006,3007,3008,3008,3009,3010,3011,3011,3012,3013,3013,3014,3015,3016,3016,
                3017,3018,3019,3019,3020,3021,3022,3022,3023,3024,3025,3025,3026,3027,3028,3028,3029,3030,3031,3031,3032,3033,3034,3034,3035,3036,3037,3037,3038,3039,3039,3040,
                3041,3042,3042,3043,3044,3045,3045,3046,3047,3048,3048,3049,3050,3051,3051,3052,3053,3054,3054,3055,3056,3056,3057,3058,3059,3059,3060,3061,3062,3062,3063,3064,
                3065,3065,3066,3067,3068,3068,3069,3070,3070,3071,3072,3073,3073,3074,3075,3076,3076,3077,3078,3079,3079,3080,3081,3081,3082,3083,3084,3084,3085,3086,3087,3087,
                3088,3089,3090,3090,3091,3092,3092,3093,3094,3095,3095,3096,3097,3098,3098,3099,3100,3101,3101,3102,3103,3103,3104,3105,3106,3106,3107,3108,3109,3109,3110,3111,
                3111,3112,3113,3114,3114,3115,3116,3117,3117,3118,3119,3119,3120,3121,3122,3122,3123,3124,3125,3125,3126,3127,3127,3128,3129,3130,3130,3131,3132,3133,3133,3134,
                3135,3135,3136,3137,3138,3138,3139,3140,3140,3141,3142,3143,3143,3144,3145,3146,3146,3147,3148,3148,3149,3150,3151,3151,3152,3153,3153,3154,3155,3156,3156,3157,
                3158,3159,3159,3160,3161,3161,3162,3163,3164,3164,3165,3166,3166,3167,3168,3169,3169,3170,3171,3171,3172,3173,3174,3174,3175,3176,3176,3177,3178,3179,3179,3180,
                3181,3181,3182,3183,3184,3184,3185,3186,3186,3187,3188,3189,3189,3190,3191,3191,3192,3193,3194,3194,3195,3196,3196,3197,3198,3199,3199,3200,3201,3201,3202,3203,
                3204,3204,3205,3206,3206,3207,3208,3209,3209,3210,3211,3211,3212,3213,3214,3214,3215,3216,3216,3217,3218,3218,3219,3220,3221,3221,3222,3223,3223,3224,3225,3226,
                3226,3227,3228,3228,3229,3230,3231,3231,3232,3233,3233,3234,3235,3235,3236,3237,3238,3238,3239,3240,3240,3241,3242,3243,3243,3244,3245,3245,3246,3247,3247,3248,
                3249,3250,3250,3251,3252,3252,3253,3254,3254,3255,3256,3257,3257,3258,3259,3259,3260,3261,3261,3262,3263,3264,3264,3265,3266,3266,3267,3268,3269,3269,3270,3271,
                3271,3272,3273,3273,3274,3275,3275,3276,3277,3278,3278,3279,3280,3280,3281,3282,3282,3283,3284,3285,3285,3286,3287,3287,3288,3289,3289,3290,3291,3292,3292,3293,
                3294,3294,3295,3296,3296,3297,3298,3298,3299,3300,3301,3301,3302,3303,3303,3304,3305,3305,3306,3307,3307,3308,3309,3310,3310,3311,3312,3312,3313,3314,3314,3315,
                3316,3316,3317,3318,3319,3319,3320,3321,3321,3322,3323,3323,3324,3325,3325,3326,3327,3328,3328,3329,3330,3330,3331,3332,3332,3333,3334,3334,3335,3336,3336,3337,
                3338,3339,3339,3340,3341,3341,3342,3343,3343,3344,3345,3345,3346,3347,3347,3348,3349,3349,3350,3351,3352,3352,3353,3354,3354,3355,3356,3356,3357,3358,3358,3359,
                3360,3360,3361,3362,3362,3363,3364,3365,3365,3366,3367,3367,3368,3369,3369,3370,3371,3371,3372,3373,3373,3374,3375,3375,3376,3377,3377,3378,3379,3379,3380,3381,
                3382,3382,3383,3384,3384,3385,3386,3386,3387,3388,3388,3389,3390,3390,3391,3392,3392,3393,3394,3394,3395,3396,3396,3397,3398,3398,3399,3400,3400,3401,3402,3403,
                3403,3404,3405,3405,3406,3407,3407,3408,3409,3409,3410,3411,3411,3412,3413,3413,3414,3415,3415,3416,3417,3417,3418,3419,3419,3420,3421,3421,3422,3423,3423,3424,
                3425,3425,3426,3427,3427,3428,3429,3429,3430,3431,3431,3432,3433,3433,3434,3435,3435,3436,3437,3437,3438,3439,3439,3440,3441,3441,3442,3443,3443,3444,3445,3445,
                3446,3447,3447,3448,3449,3449,3450,3451,3451,3452,3453,3453,3454,3455,3455,3456,3457,3457,3458,3459,3459,3460,3461,3461,3462,3463,3463,3464,3465,3465,3466,3467,
                3467,3468,3469,3469,3470,3471,3471,3472,3473,3473,3474,3475,3475,3476,3477,3477,3478,3479,3479,3480,3481,3481,3482,3483,3483,3484,3485,3485,3486,3486,3487,3488,
                3488,3489,3490,3490,3491,3492,3492,3493,3494,3494,3495,3496,3496,3497,3498,3498,3499,3500,3500,3501,3502,3502,3503,3504,3504,3505,3506,3506,3507,3507,3508,3509,
                3509,3510,3511,3511,3512,3513,3513,3514,3515,3515,3516,3517,3517,3518,3519,3519,3520,3521,3521,3522,3522,3523,3524,3524,3525,3526,3526,3527,3528,3528,3529,3530,
                3530,3531,3532,3532,3533,3534,3534,3535,3536,3537,3538,3538,3539,3540,3540,3541,3541,3542,3543,3543,3544,3544,3545,3546,3546,3547,3548,3548,3549,3549,3550,3551,
                3551,3552,3553,3553,3554,3554,3555,3556,3556,3557,3558,3558,3559,3560,3560,3561,3561,3562,3563,3563,3564,3565,3565,3566,3566,3567,3568,3568,3569,3570,3570,3571,
                3572,3572,3573,3574,3574,3575,3575,3576,3577,3577,3578,3579,3579,3580,3581,3581,3582,3583,3583,3584,3585,3585,3586,3587,3587,3588,3588,3589,3590,3590,3591,3592,
                3592,3593,3594,3594,3595,3596,3596,3597,3598,3598,3599,3600,3600,3601,3602,3602,3603,3604,3604,3605,3606,3606,3607,3608,3608,3609,3610,3610,3611,3612,3612,3613,
                3614,3615,3615,3616,3617,3617,3618,3619,3619,3620,3621,3621,3622,3623,3623,3624,3625,3625,3626,3627,3628,3628,3629,3630,3630,3631,3632,3632,3633,3634,3634,3635,
                3636,3637,3637,3638,3639,3639,3640,3641,3641,3642,3643,3643,3644,3645,3646,3646,3647,3648,3648,3649,3650,3651,3651,3652,3653,3653,3654,3655,3656,3656,3657,3658,
                3658,3659,3660,3660,3661,3662,3663,3663,3664,3665,3666,3666,3667,3668,3668,3669,3670,3671,3671,3672,3673,3673,3674,3675,3676,3676,3677,3678,3679,3679,3680,3681,
                3681,3682,3683,3684,3684,3685,3686,3687,3687,3688,3689,3690,3690,3691,3692,3692,3693,3694,3695,3695,3696,3697,3698,3698,3699,3700,3701,3701,3702,3703,3704,3704,
                3705,3706,3707,3707,3708,3709,3710,3710,3711,3712,3713,3713,3714,3715,3716,3716,3717,3718,3719,3719,3720,3721,3722,3722,3723,3724,3725,3725,3726,3727,3728,3729,
                3729,3730,3731,3732,3732,3733,3734,3735,3735,3736,3737,3738,3739,3739,3740,3741,3742,3742,3743,3744,3745,3746,3746,3747,3748,3749,3749,3750,3751,3752,3753,3753,
                3754,3755,3756,3756,3757,3758,3759,3760,3760,3761,3762,3763,3764,3764,3765,3766,3767,3768,3768,3769,3770,3771,3771,3772,3773,3774,3775,3775,3776,3777,3778,3779,
                3779,3780,3781,3782,3783,3783,3784,3785,3786,3787,3787,3788,3789,3790,3791,3792,3792,3793,3794,3795,3796,3796,3797,3798,3799,3800,3800,3801,3802,3803,3804,3805,
                3805,3806,3807,3808,3809,3809,3810,3811,3812,3813,3814,3814,3815,3816,3817,3818,3819,3819,3820,3821,3822,3823,3824,3824,3825,3826,3827,3828,3829,3829,3830,3831,
                3832,3833,3834,3834,3835,3836,3837,3838,3839,3839,3840,3841,3842,3843,3844,3844,3845,3846,3847,3848,3849,3850,3850,3851,3852,3853,3854,3855,3855,3856,3857,3858,
                3859,3860,3861,3861,3862,3863,3864,3865,3866,3867,3867,3868,3869,3870,3871,3872,3873,3873,3874,3875,3876,3877,3878,3879,3880,3880,3881,3882,3883,3884,3885,3886,
                3886,3887,3888,3889,3890,3891,3892,3893,3893,3894,3895,3896,3897,3898,3899,3900,3900,3901,3902,3903,3904,3905,3906,3907,3908,3908,3909,3910,3911,3912,3913,3914,
                3915,3916,3916,3917,3918,3919,3920,3921,3922,3923,3924,3924,3925,3926,3927,3928,3929,3930,3931,3932,3933,3933,3934,3935,3936,3937,3938,3939,3940,3941,3942,3942,
                3943,3944,3945,3946,3947,3948,3949,3950,3951,3952,3952,3953,3954,3955,3956,3957,3958,3959,3960,3961,3962,3963,3963,3964,3965,3966,3967,3968,3969,3970,3971,3972,
                3973,3974,3975,3975,3976,3977,3978,3979,3980,3981,3982,3983,3984,3985,3986,3987,3988,3988,3989,3990,3991,3992,3993,3994,3995,3996,3997,3998,3999,4000,4001,4002,
                4003,4003,4004,4005,4006,4007,4008,4009,4010,4011,4012,4013,4014,4015,4016,4017,4018,4019,4020,4021,4021,4022,4023,4024,4025,4026,4027,4028,4029,4030,4031,4032,
                4033,4034,4035,4036,4037,4038,4039,4040,4041,4042,4043,4044,4045,4045,4046,4047,4048,4049,4050,4051,4052,4053,4054,4055,4056,4057,4058,4059,4060,4061,4062,4063,
                4064,4065,4066,4067,4068,4069,4070,4071,4072,4073,4074,4075,4076,4077,4078,4079,4080,4081,4082,4083,4084,4085,4086,4087,4088,4089,4090,4091,4092,4093,4094,4095,
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
             180,
          },
          /* aec trigger 1 */
          {
             210,
             260,
          },
          /* aec trigger 2 */
          {
             300,
             340,
          },
          /* aec trigger 3 */
          {
             350,
             380,
          },
          /* aec trigger 4 */
          {
             400,
             500,
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
             0.2,//ltm_smooth_fork
             0.4,//ltm_smooth_spear
             1,//ltm_smooth_gamma
             0.6,//ltm_dehighlight_fork
             0.8,//ltm_dehighlight_spear
             2,//ltm_dehighlight_gamma
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
             0.2,//ltm_smooth_fork
             0.4,//ltm_smooth_spear
             1,//ltm_smooth_gamma
             0.6,//ltm_dehighlight_fork
             0.8,//ltm_dehighlight_spear
             2,//ltm_dehighlight_gamma
          },
          /* para 2 */
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
             1,//ltm_smooth
             1,//ltm_dehighlight
             0,//ltm_base
             0.2,//ltm_smooth_fork
             0.4,//ltm_smooth_spear
             1,//ltm_smooth_gamma
             0.6,//ltm_dehighlight_fork
             0.8,//ltm_dehighlight_spear
             2,//ltm_dehighlight_gamma
          },
          /* para 3 */
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
             1,//ltm_smooth
             1,//ltm_dehighlight
             0,//ltm_base
             0.2,//ltm_smooth_fork
             0.4,//ltm_smooth_spear
             1,//ltm_smooth_gamma
             0.6,//ltm_dehighlight_fork
             0.8,//ltm_dehighlight_spear
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
             1,//ltm_smooth
             1,//ltm_dehighlight
             0,//ltm_base
             0.2,//ltm_smooth_fork
             0.4,//ltm_smooth_spear
             1,//ltm_smooth_gamma
             0.6,//ltm_dehighlight_fork
             0.8,//ltm_dehighlight_spear
             2,//ltm_dehighlight_gamma
          },
       },
       1,//enable_tuning_mesh
       16,//mesh_w
       16,//mesh_h
    },
    /* isp_sub_module_3d_lut_tuning_t isp_sub_module_3d_lut_tuning */
   {
       /*int enable*/
       0,
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
             300,
          },
          /* aec trigger 2 */
          {
             320,
             360,
          },
          /* aec trigger 3 */
          {
             400,
             470,
          },
          /* aec trigger 4 */
          {
             490,
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
                0.9,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                0.9,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                0.9,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                0.9,//saturation
                0,//hue
             },
             /* awb para 4 */
             {
                0.9,//saturation
                0,//hue
             },
             /* awb para 5 */
             {
                0.9,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                0.9,//saturation
                0,//hue
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                1,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                0.8,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                0.6,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                0.8,//saturation
                0,//hue
             },
             /* awb para 4 */
             {
                0.8,//saturation
                0,//hue
             },
             /* awb para 5 */
             {
                0.9,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                0.9,//saturation
                0,//hue
             },
          },
          /* aec trigger 2 */
          {
             /* awb para 0 */
             {
                0.8,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                0.6,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                0.6,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                0.8,//saturation
                0,//hue
             },
             /* awb para 4 */
             {
                0.7,//saturation
                0,//hue
             },
             /* awb para 5 */
             {
                0.7,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                0.8,//saturation
                0,//hue
             },
          },
          /* aec trigger 3 */
          {
             /* awb para 0 */
             {
                0.8,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                0.8,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                0.8,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                0.8,//saturation
                0,//hue
             },
             /* awb para 4 */
             {
                0.8,//saturation
                0,//hue
             },
             /* awb para 5 */
             {
                0.8,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                0.8,//saturation
                0,//hue
             },
          },
          /* aec trigger 4 */
          {
             /* awb para 0 */
             {
                0.9,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                0.9,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                0.9,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                0.9,//saturation
                0,//hue
             },
             /* awb para 4 */
             {
                0.9,//saturation
                0,//hue
             },
             /* awb para 5 */
             {
                0.9,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                0.9,//saturation
                0,//hue
             },
          },
       },
    },
    /* isp_sub_module_lee_tuning_t isp_sub_module_lee_tuning */
    {
       1,//enable
       1,//interpolation_enable
       10,//count_ae
       0,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             1,
             1.1,
          },
          /* aec trigger 1 */
          {
             2,
             2.1,
          },
          /* aec trigger 2 */
          {
             4,
             5.1,
          },
          /* aec trigger 3 */
          {
             8,
             8.1,
          },
          /* aec trigger 4 */
          {
             16,
             16.1,
          },
          /* aec trigger 5 */
          {
             32,
             32.1,
          },
          /* aec trigger 6 */
          {
             64,
             64.1,
          },
          /* aec trigger 7 */
          {
             128,
             128.1,
          },
          /* aec trigger 8 */
          {
             512,
             512.1,
          },
          /* aec trigger 9 */
          {
             1500,
             1500.1,
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
                -1,-5,-11,-14,//bpf(1,1:4)
                -5,-22,-56,-82,//bpf(2,1:4)
                -11,-56,-136,-179,//bpf(3,1:4)
                -14,-82,-179,2312,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             85,//strength_pos_edge
             80,//strength_neg_edge
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
             {0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             250,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {16,32,48,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
                -1,-5,-11,-14,//bpf(1,1:4)
                -5,-22,-56,-82,//bpf(2,1:4)
                -11,-56,-136,-179,//bpf(3,1:4)
                -14,-82,-179,2312,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             60,//strength_pos_edge
             70,//strength_neg_edge
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
             250,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {16,32,48,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
                -1,-5,-11,-14,//bpf(1,1:4)
                -5,-22,-56,-82,//bpf(2,1:4)
                -11,-56,-136,-179,//bpf(3,1:4)
                -14,-82,-179,2312,//bpf(4,1:4)
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
             250,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {16,28,40,52,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
             40,//strength_pos_edge
             40,//strength_neg_edge
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
             250,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {16,28,40,52,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
             25,//strength_pos_edge
             35,//strength_neg_edge
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
             {0,0,0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             250,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {16,28,40,52,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
                0,-2,-10,-18,//bpf(1,1:4)
                -2,-26,-74,-102,//bpf(2,1:4)
                -10,-74,-222,96,//bpf(3,1:4)
                -18,-102,96,1776,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             20,//strength_pos_edge
             30,//strength_neg_edge
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
             {0,0,0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             250,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {21,37,51,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
                0,-2,-14,-24,//bpf(1,1:4)
                -2,-36,-104,-148,//bpf(2,1:4)
                -14,-104,-220,382,//bpf(3,1:4)
                -24,-148,382,1144,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             20,//strength_pos_edge
             25,//strength_neg_edge
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
             {0,0,0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             250,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {30,38,46,54,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
             4,//strength_pos_edge
             4,//strength_neg_edge
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
             {0,0,0,0,0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             250,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
             4,//strength_pos_edge
             4,//strength_neg_edge
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
             {0,0,0,0,0,0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             250,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
             20,//strength_pos_edge
             30,//strength_neg_edge
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
             {0,0,0,0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             250,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
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
       0,//trigger_mode : 0 gain trigger 1 lux trigger
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
             1,
             1.1,
          },
          /* aec trigger 1 */
          {
             2,
             2.1,
          },
          /* aec trigger 2 */
          {
             4,
             4.1,
          },
          /* aec trigger 3 */
          {
             8,
             8.1,
          },
          /* aec trigger 4 */
          {
             16,
             16.1,
          },
          /* aec trigger 5 */
          {
             32,
             32.1,
          },
          /* aec trigger 6 */
          {
             64,
             64.1,
          },
          /* aec trigger 7 */
          {
             128,
             128.1,
          },
          /* aec trigger 8 */
          {
             256,
             256.1,
          },
          /* aec trigger 9 */
          {
             512,
             512.1,
          },
          /* aec trigger 10 */
          {
             1024,
             1024.1,
          },
       },
       /* isp_sub_module_cnf_pra pra[MAX_TRIGGER_COUNT] */
       {
          /* para 0 */
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
          /* para 4 */
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
          /* para 5 */
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
       0,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             1,
             1.1,
          },
          /* aec trigger 1 */
          {
             2,
             2.1,
          },
          /* aec trigger 2 */
          {
             4,
             4.1,
          },
          /* aec trigger 3 */
          {
             8,
             8.1,
          },
          /* aec trigger 4 */
          {
             16,
             16.1,
          },
          /* aec trigger 5 */
          {
             32,
             32.1,
          },
          /* aec trigger 6 */
          {
             64,
             64.1,
          },
          /* aec trigger 7 */
          {
             128,
             128.1,
          },
          /* aec trigger 8 */
          {
             256,
             256.1,
          },
          /* aec trigger 9 */
          {
             512,
             512.1,
          },
          /* aec trigger 10 */
          {
             1500,
             1500.1,
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
             60,//nr3d_npy_th2
             18,//nr3d_mpy_th1
             30,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             13,//nr3d_mpc_th1
             27,//nr3d_mpc_th2
             3,//nr3d_lamda2d
             10,//nr3d_ite
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
             8,//dbk_h_mv_diff
             12,//dbk_h_mv_str
             8,//dbk_v_mv_diff
             12,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             20,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             20,//dbk_v_yy_diff
             10,//dbk_satu
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
                0,0,1,2,4,5,6,7,9,10,11,13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
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
          /* para 1 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             10,//nr3d_npy_th1
             80,//nr3d_npy_th2
             18,//nr3d_mpy_th1
             32,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             18,//nr3d_mpc_th1
             32,//nr3d_mpc_th2
             5,//nr3d_lamda2d
             13,//nr3d_ite
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
             8,//dbk_h_mv_diff
             14,//dbk_h_mv_str
             8,//dbk_v_mv_diff
             14,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             25,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             25,//dbk_v_yy_diff
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
                0,0,1,2,3,4,5,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16,16,16,16,17,17,17,17,17,17,18,18,18,18,18,19,19,19,19,19,19,20,20,20,20,20,21,21,21,21,21,22,22,22,22,22,22,23,23,23,23,23,23,
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
             20,//nr3d_mpy_th1
             40,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             20,//nr3d_mpc_th1
             40,//nr3d_mpc_th2
             7,//nr3d_lamda2d
             25,//nr3d_ite
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
             10,//dbk_h_mv_diff
             20,//dbk_h_mv_str
             10,//dbk_v_mv_diff
             20,//dbk_v_mv_str
             60,//dbk_h_yy_flat
             35,//dbk_h_yy_diff
             60,//dbk_v_yy_flat
             35,//dbk_v_yy_diff
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
                0,0,1,2,3,4,5,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16,17,17,17,18,18,18,19,19,19,20,20,21,21,21,22,22,22,23,23,23,24,24,25,25,25,26,26,26,27,27,28,28,28,29,29,29,30,30,30,31,31,31,
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
          /* para 3 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             10,//nr3d_npy_th1
             80,//nr3d_npy_th2
             50,//nr3d_mpy_th1
             80,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             45,//nr3d_mpc_th1
             90,//nr3d_mpc_th2
             10,//nr3d_lamda2d
             40,//nr3d_ite
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
             25,//dbk_h_mv_str
             12,//dbk_v_mv_diff
             25,//dbk_v_mv_str
             85,//dbk_h_yy_flat
             40,//dbk_h_yy_diff
             85,//dbk_v_yy_flat
             40,//dbk_v_yy_diff
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
                0,0,1,2,3,4,5,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16,17,17,17,18,18,18,19,19,19,20,20,21,21,21,22,22,22,23,23,23,24,24,25,25,25,26,26,26,27,27,28,28,28,29,29,29,30,30,30,31,31,31,
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
             10,//nr3d_npy_th1
             80,//nr3d_npy_th2
             80,//nr3d_mpy_th1
             80,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             90,//nr3d_npc_th3
             180,//nr3d_npc_th4
             50,//nr3d_mpc_th1
             100,//nr3d_mpc_th2
             10,//nr3d_lamda2d
             40,//nr3d_ite
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
             20,//dbk_h_mv_diff
             30,//dbk_h_mv_str
             20,//dbk_v_mv_diff
             30,//dbk_v_mv_str
             85,//dbk_h_yy_flat
             33,//dbk_h_yy_diff
             85,//dbk_v_yy_flat
             33,//dbk_v_yy_diff
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
                0,0,1,2,3,4,5,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16,17,17,17,18,18,18,19,19,19,20,20,21,21,21,22,22,22,23,23,23,24,24,25,25,25,26,26,26,27,27,28,28,28,29,29,29,30,30,30,31,31,31,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             20,//dbk_h_ed_thrd
             20,//dbk_v_ed_thrd
          },
          /* para 5 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             12,//nr3d_npy_th1
             80,//nr3d_npy_th2
             50,//nr3d_mpy_th1
             80,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             50,//nr3d_mpc_th1
             100,//nr3d_mpc_th2
             20,//nr3d_lamda2d
             40,//nr3d_ite
             0,//nr3d_msr
             248,//nr3d_decay
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
             45,//dbk_h_mv_str
             30,//dbk_v_mv_diff
             45,//dbk_v_mv_str
             85,//dbk_h_yy_flat
             36,//dbk_h_yy_diff
             85,//dbk_v_yy_flat
             36,//dbk_v_yy_diff
             10,//dbk_satu
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
                0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,16,16,16,17,17,17,18,18,18,19,19,19,20,20,21,21,21,22,22,22,23,23,23,24,24,25,25,25,26,26,26,27,27,28,28,28,29,29,29,30,30,30,31,31,31,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             20,//dbk_h_ed_thrd
             20,//dbk_v_ed_thrd
          },
          /* para 6 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             15,//nr3d_npy_th1
             80,//nr3d_npy_th2
             120,//nr3d_mpy_th1
             200,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             50,//nr3d_mpc_th1
             100,//nr3d_mpc_th2
             25,//nr3d_lamda2d
             38,//nr3d_ite
             0,//nr3d_msr
             248,//nr3d_decay
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
             50,//dbk_h_mv_str
             40,//dbk_v_mv_diff
             50,//dbk_v_mv_str
             100,//dbk_h_yy_flat
             40,//dbk_h_yy_diff
             100,//dbk_v_yy_flat
             40,//dbk_v_yy_diff
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
                0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,12,13,14,15,16,16,16,17,17,17,18,18,18,19,19,19,20,20,21,21,21,22,22,22,24,24,24,26,26,28,28,28,30,30,30,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             20,//dbk_h_ed_thrd
             20,//dbk_v_ed_thrd
          },
          /* para 7 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             15,//nr3d_npy_th1
             80,//nr3d_npy_th2
             120,//nr3d_mpy_th1
             200,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             80,//nr3d_npc_th3
             180,//nr3d_npc_th4
             60,//nr3d_mpc_th1
             130,//nr3d_mpc_th2
             30,//nr3d_lamda2d
             40,//nr3d_ite
             0,//nr3d_msr
             248,//nr3d_decay
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
             50,//dbk_h_mv_diff
             70,//dbk_h_mv_str
             50,//dbk_v_mv_diff
             70,//dbk_v_mv_str
             85,//dbk_h_yy_flat
             70,//dbk_h_yy_diff
             85,//dbk_v_yy_flat
             70,//dbk_v_yy_diff
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
                0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             24,//dbk_h_ed_thrd
             24,//dbk_v_ed_thrd
          },
          /* para 8 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             30,//nr3d_npy_th1
             100,//nr3d_npy_th2
             120,//nr3d_mpy_th1
             240,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             120,//nr3d_mpc_th1
             240,//nr3d_mpc_th2
             30,//nr3d_lamda2d
             45,//nr3d_ite
             0,//nr3d_msr
             248,//nr3d_decay
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
             60,//dbk_h_mv_diff
             90,//dbk_h_mv_str
             60,//dbk_v_mv_diff
             90,//dbk_v_mv_str
             100,//dbk_h_yy_flat
             80,//dbk_h_yy_diff
             100,//dbk_v_yy_flat
             80,//dbk_v_yy_diff
             50,//dbk_satu
             3,//dbk_gaus_y_c11
             16,//dbk_gaus_y_c12
             30,//dbk_gaus_y_c13
             5,//dbk_gaus_y_c21
             50,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,0,0,0,0,0,0,0,0,1,2,3,3,4,4,5,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16,16,17,18,18,18,18,19,19,19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             24,//dbk_h_ed_thrd
             24,//dbk_v_ed_thrd
          },
          /* para 9 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             50,//nr3d_npy_th1
             130,//nr3d_npy_th2
             120,//nr3d_mpy_th1
             240,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             120,//nr3d_mpc_th1
             240,//nr3d_mpc_th2
             40,//nr3d_lamda2d
             100,//nr3d_ite
             0,//nr3d_msr
             248,//nr3d_decay
             40,//nr3d_satu
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
             1,//disable_1st_media_filter
             1,//dbk_h_en
             1,//dbk_v_en
             0,//dbk_wr2ddr
             0,//dbk_bypass
             60,//dbk_h_mv_diff
             90,//dbk_h_mv_str
             60,//dbk_v_mv_diff
             90,//dbk_v_mv_str
             100,//dbk_h_yy_flat
             85,//dbk_h_yy_diff
             100,//dbk_v_yy_flat
             85,//dbk_v_yy_diff
             70,//dbk_satu
             7,//dbk_gaus_y_c11
             18,//dbk_gaus_y_c12
             25,//dbk_gaus_y_c13
             10,//dbk_gaus_y_c21
             36,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,14,15,16,16,16,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             28,//dbk_h_ed_thrd
             28,//dbk_v_ed_thrd
          },
          /* para 10 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             90,//nr3d_npy_th1
             170,//nr3d_npy_th2
             120,//nr3d_mpy_th1
             240,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             120,//nr3d_mpc_th1
             240,//nr3d_mpc_th2
             40,//nr3d_lamda2d
             100,//nr3d_ite
             0,//nr3d_msr
             248,//nr3d_decay
             60,//nr3d_satu
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
             60,//dbk_h_mv_diff
             90,//dbk_h_mv_str
             60,//dbk_v_mv_diff
             90,//dbk_v_mv_str
             100,//dbk_h_yy_flat
             90,//dbk_h_yy_diff
             100,//dbk_v_yy_flat
             90,//dbk_v_yy_diff
             100,//dbk_satu
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
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,3,4,5,6,6,7,8,9,10,10,11,12,13,14,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             28,//dbk_h_ed_thrd
             28,//dbk_v_ed_thrd
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
             170,
          },
          /* aec trigger 1 */
          {
             200,
             300,
          },
          /* aec trigger 2 */
          {
             340,
             370,
          },
          /* aec trigger 3 */
          {
             400,
             430,
          },
          /* aec trigger 4 */
          {
             450,
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
                1.1,//saturation
                0,//hue
                10,//y_lo_th1
                40,//y_lo_th2
                1022,//y_hi_th1
                1023,//y_hi_th2
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                1,//saturation
                0,//hue
                10,//y_lo_th1
                40,//y_lo_th2
                1022,//y_hi_th1
                1023,//y_hi_th2
             },
          },
          /* aec trigger 2 */
          {
             /* awb para 0 */
             {
                0.9,//saturation
                0,//hue
                10,//y_lo_th1
                40,//y_lo_th2
                1000,//y_hi_th1
                1023,//y_hi_th2
             },
          },
          /* aec trigger 3 */
          {
             /* awb para 0 */
             {
                0.7,//saturation
                0,//hue
                10,//y_lo_th1
                40,//y_lo_th2
                1000,//y_hi_th1
                1023,//y_hi_th2
             },
          },
          /* aec trigger 4 */
          {
             /* awb para 0 */
             {
                0.8,//saturation
                0,//hue
                10,//y_lo_th1
                40,//y_lo_th2
                1000,//y_hi_th1
                1023,//y_hi_th2
             },
          },
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
      472,
      /* ev0_count */
      472,
      /* ev2_count */
      472,
      /* fix_fps_index */
      25,
      /* enable_isp_digital_gain */
      0,
      /* max_sensor_gain */
      1500,
      /* max_isp_gain1 */
      1,
      /* exp_table */
      {
      //1,1125
      //1500,1125
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
         {    260,   1125   },         /* Gain = 1.015625 Exposure Index = 227 */
         {    268,   1125   },         /* Gain = 1.046875 Exposure Index = 228 */
         {    277,   1125   },         /* Gain = 1.082031 Exposure Index = 229 */
         {    286,   1125   },         /* Gain = 1.117188 Exposure Index = 230 */
         {    295,   1125   },         /* Gain = 1.152344 Exposure Index = 231 */
         {    304,   1125   },         /* Gain = 1.187500 Exposure Index = 232 */
         {    314,   1125   },         /* Gain = 1.226562 Exposure Index = 233 */
         {    324,   1125   },         /* Gain = 1.265625 Exposure Index = 234 */
         {    334,   1125   },         /* Gain = 1.304688 Exposure Index = 235 */
         {    345,   1125   },         /* Gain = 1.347656 Exposure Index = 236 */
         {    356,   1125   },         /* Gain = 1.390625 Exposure Index = 237 */
         {    367,   1125   },         /* Gain = 1.433594 Exposure Index = 238 */
         {    379,   1125   },         /* Gain = 1.480469 Exposure Index = 239 */
         {    391,   1125   },         /* Gain = 1.527344 Exposure Index = 240 */
         {    403,   1125   },         /* Gain = 1.574219 Exposure Index = 241 */
         {    416,   1125   },         /* Gain = 1.625000 Exposure Index = 242 */
         {    429,   1125   },         /* Gain = 1.675781 Exposure Index = 243 */
         {    442,   1125   },         /* Gain = 1.726562 Exposure Index = 244 */
         {    456,   1125   },         /* Gain = 1.781250 Exposure Index = 245 */
         {    470,   1125   },         /* Gain = 1.835938 Exposure Index = 246 */
         {    485,   1125   },         /* Gain = 1.894531 Exposure Index = 247 */
         {    500,   1125   },         /* Gain = 1.953125 Exposure Index = 248 */
         {    515,   1125   },         /* Gain = 2.011719 Exposure Index = 249 */
         {    531,   1125   },         /* Gain = 2.074219 Exposure Index = 250 */
         {    547,   1125   },         /* Gain = 2.136719 Exposure Index = 251 */
         {    564,   1125   },         /* Gain = 2.203125 Exposure Index = 252 */
         {    581,   1125   },         /* Gain = 2.269531 Exposure Index = 253 */
         {    599,   1125   },         /* Gain = 2.339844 Exposure Index = 254 */
         {    617,   1125   },         /* Gain = 2.410156 Exposure Index = 255 */
         {    636,   1125   },         /* Gain = 2.484375 Exposure Index = 256 */
         {    656,   1125   },         /* Gain = 2.562500 Exposure Index = 257 */
         {    676,   1125   },         /* Gain = 2.640625 Exposure Index = 258 */
         {    697,   1125   },         /* Gain = 2.722656 Exposure Index = 259 */
         {    718,   1125   },         /* Gain = 2.804688 Exposure Index = 260 */
         {    740,   1125   },         /* Gain = 2.890625 Exposure Index = 261 */
         {    763,   1125   },         /* Gain = 2.980469 Exposure Index = 262 */
         {    786,   1125   },         /* Gain = 3.070312 Exposure Index = 263 */
         {    810,   1125   },         /* Gain = 3.164062 Exposure Index = 264 */
         {    835,   1125   },         /* Gain = 3.261719 Exposure Index = 265 */
         {    861,   1125   },         /* Gain = 3.363281 Exposure Index = 266 */
         {    887,   1125   },         /* Gain = 3.464844 Exposure Index = 267 */
         {    914,   1125   },         /* Gain = 3.570312 Exposure Index = 268 */
         {    942,   1125   },         /* Gain = 3.679688 Exposure Index = 269 */
         {    971,   1125   },         /* Gain = 3.792969 Exposure Index = 270 */
         {   1001,   1125   },         /* Gain = 3.910156 Exposure Index = 271 */
         {   1032,   1125   },         /* Gain = 4.031250 Exposure Index = 272 */
         {   1063,   1125   },         /* Gain = 4.152344 Exposure Index = 273 */
         {   1095,   1125   },         /* Gain = 4.277344 Exposure Index = 274 */
         {   1128,   1125   },         /* Gain = 4.406250 Exposure Index = 275 */
         {   1162,   1125   },         /* Gain = 4.539062 Exposure Index = 276 */
         {   1197,   1125   },         /* Gain = 4.675781 Exposure Index = 277 */
         {   1233,   1125   },         /* Gain = 4.816406 Exposure Index = 278 */
         {   1270,   1125   },         /* Gain = 4.960938 Exposure Index = 279 */
         {   1309,   1125   },         /* Gain = 5.113281 Exposure Index = 280 */
         {   1349,   1125   },         /* Gain = 5.269531 Exposure Index = 281 */
         {   1390,   1125   },         /* Gain = 5.429688 Exposure Index = 282 */
         {   1432,   1125   },         /* Gain = 5.593750 Exposure Index = 283 */
         {   1475,   1125   },         /* Gain = 5.761719 Exposure Index = 284 */
         {   1520,   1125   },         /* Gain = 5.937500 Exposure Index = 285 */
         {   1566,   1125   },         /* Gain = 6.117188 Exposure Index = 286 */
         {   1613,   1125   },         /* Gain = 6.300781 Exposure Index = 287 */
         {   1662,   1125   },         /* Gain = 6.492188 Exposure Index = 288 */
         {   1712,   1125   },         /* Gain = 6.687500 Exposure Index = 289 */
         {   1764,   1125   },         /* Gain = 6.890625 Exposure Index = 290 */
         {   1817,   1125   },         /* Gain = 7.097656 Exposure Index = 291 */
         {   1872,   1125   },         /* Gain = 7.312500 Exposure Index = 292 */
         {   1929,   1125   },         /* Gain = 7.535156 Exposure Index = 293 */
         {   1987,   1125   },         /* Gain = 7.761719 Exposure Index = 294 */
         {   2047,   1125   },         /* Gain = 7.996094 Exposure Index = 295 */
         {   2109,   1125   },         /* Gain = 8.238281 Exposure Index = 296 */
         {   2173,   1125   },         /* Gain = 8.488281 Exposure Index = 297 */
         {   2239,   1125   },         /* Gain = 8.746094 Exposure Index = 298 */
         {   2307,   1125   },         /* Gain = 9.011719 Exposure Index = 299 */
         {   2377,   1125   },         /* Gain = 9.285156 Exposure Index = 300 */
         {   2449,   1125   },         /* Gain = 9.566406 Exposure Index = 301 */
         {   2523,   1125   },         /* Gain = 9.855469 Exposure Index = 302 */
         {   2599,   1125   },         /* Gain = 10.152344 Exposure Index = 303 */
         {   2677,   1125   },         /* Gain = 10.457031 Exposure Index = 304 */
         {   2758,   1125   },         /* Gain = 10.773438 Exposure Index = 305 */
         {   2841,   1125   },         /* Gain = 11.097656 Exposure Index = 306 */
         {   2927,   1125   },         /* Gain = 11.433594 Exposure Index = 307 */
         {   3015,   1125   },         /* Gain = 11.777344 Exposure Index = 308 */
         {   3106,   1125   },         /* Gain = 12.132812 Exposure Index = 309 */
         {   3200,   1125   },         /* Gain = 12.500000 Exposure Index = 310 */
         {   3296,   1125   },         /* Gain = 12.875000 Exposure Index = 311 */
         {   3395,   1125   },         /* Gain = 13.261719 Exposure Index = 312 */
         {   3497,   1125   },         /* Gain = 13.660156 Exposure Index = 313 */
         {   3602,   1125   },         /* Gain = 14.070312 Exposure Index = 314 */
         {   3711,   1125   },         /* Gain = 14.496094 Exposure Index = 315 */
         {   3823,   1125   },         /* Gain = 14.933594 Exposure Index = 316 */
         {   3938,   1125   },         /* Gain = 15.382812 Exposure Index = 317 */
         {   4057,   1125   },         /* Gain = 15.847656 Exposure Index = 318 */
         {   4179,   1125   },         /* Gain = 16.324219 Exposure Index = 319 */
         {   4305,   1125   },         /* Gain = 16.816406 Exposure Index = 320 */
         {   4435,   1125   },         /* Gain = 17.324219 Exposure Index = 321 */
         {   4569,   1125   },         /* Gain = 17.847656 Exposure Index = 322 */
         {   4707,   1125   },         /* Gain = 18.386719 Exposure Index = 323 */
         {   4849,   1125   },         /* Gain = 18.941406 Exposure Index = 324 */
         {   4995,   1125   },         /* Gain = 19.511719 Exposure Index = 325 */
         {   5145,   1125   },         /* Gain = 20.097656 Exposure Index = 326 */
         {   5300,   1125   },         /* Gain = 20.703125 Exposure Index = 327 */
         {   5459,   1125   },         /* Gain = 21.324219 Exposure Index = 328 */
         {   5623,   1125   },         /* Gain = 21.964844 Exposure Index = 329 */
         {   5792,   1125   },         /* Gain = 22.625000 Exposure Index = 330 */
         {   5966,   1125   },         /* Gain = 23.304688 Exposure Index = 331 */
         {   6145,   1125   },         /* Gain = 24.003906 Exposure Index = 332 */
         {   6330,   1125   },         /* Gain = 24.726562 Exposure Index = 333 */
         {   6520,   1125   },         /* Gain = 25.468750 Exposure Index = 334 */
         {   6716,   1125   },         /* Gain = 26.234375 Exposure Index = 335 */
         {   6918,   1125   },         /* Gain = 27.023438 Exposure Index = 336 */
         {   7126,   1125   },         /* Gain = 27.835938 Exposure Index = 337 */
         {   7340,   1125   },         /* Gain = 28.671875 Exposure Index = 338 */
         {   7561,   1125   },         /* Gain = 29.535156 Exposure Index = 339 */
         {   7788,   1125   },         /* Gain = 30.421875 Exposure Index = 340 */
         {   8022,   1125   },         /* Gain = 31.335938 Exposure Index = 341 */
         {   8263,   1125   },         /* Gain = 32.277344 Exposure Index = 342 */
         {   8511,   1125   },         /* Gain = 33.246094 Exposure Index = 343 */
         {   8767,   1125   },         /* Gain = 34.246094 Exposure Index = 344 */
         {   9031,   1125   },         /* Gain = 35.277344 Exposure Index = 345 */
         {   9302,   1125   },         /* Gain = 36.335938 Exposure Index = 346 */
         {   9582,   1125   },         /* Gain = 37.429688 Exposure Index = 347 */
         {   9870,   1125   },         /* Gain = 38.554688 Exposure Index = 348 */
         {  10167,   1125   },         /* Gain = 39.714844 Exposure Index = 349 */
         {  10473,   1125   },         /* Gain = 40.910156 Exposure Index = 350 */
         {  10788,   1125   },         /* Gain = 42.140625 Exposure Index = 351 */
         {  11112,   1125   },         /* Gain = 43.406250 Exposure Index = 352 */
         {  11446,   1125   },         /* Gain = 44.710938 Exposure Index = 353 */
         {  11790,   1125   },         /* Gain = 46.054688 Exposure Index = 354 */
         {  12144,   1125   },         /* Gain = 47.437500 Exposure Index = 355 */
         {  12509,   1125   },         /* Gain = 48.863281 Exposure Index = 356 */
         {  12885,   1125   },         /* Gain = 50.332031 Exposure Index = 357 */
         {  13272,   1125   },         /* Gain = 51.843750 Exposure Index = 358 */
         {  13671,   1125   },         /* Gain = 53.402344 Exposure Index = 359 */
         {  14082,   1125   },         /* Gain = 55.007812 Exposure Index = 360 */
         {  14505,   1125   },         /* Gain = 56.660156 Exposure Index = 361 */
         {  14941,   1125   },         /* Gain = 58.363281 Exposure Index = 362 */
         {  15390,   1125   },         /* Gain = 60.117188 Exposure Index = 363 */
         {  15852,   1125   },         /* Gain = 61.921875 Exposure Index = 364 */
         {  16328,   1125   },         /* Gain = 63.781250 Exposure Index = 365 */
         {  16818,   1125   },         /* Gain = 65.695312 Exposure Index = 366 */
         {  17323,   1125   },         /* Gain = 67.667969 Exposure Index = 367 */
         {  17843,   1125   },         /* Gain = 69.699219 Exposure Index = 368 */
         {  18379,   1125   },         /* Gain = 71.792969 Exposure Index = 369 */
         {  18931,   1125   },         /* Gain = 73.949219 Exposure Index = 370 */
         {  19499,   1125   },         /* Gain = 76.167969 Exposure Index = 371 */
         {  20084,   1125   },         /* Gain = 78.453125 Exposure Index = 372 */
         {  20687,   1125   },         /* Gain = 80.808594 Exposure Index = 373 */
         {  21308,   1125   },         /* Gain = 83.234375 Exposure Index = 374 */
         {  21948,   1125   },         /* Gain = 85.734375 Exposure Index = 375 */
         {  22607,   1125   },         /* Gain = 88.308594 Exposure Index = 376 */
         {  23286,   1125   },         /* Gain = 90.960938 Exposure Index = 377 */
         {  23985,   1125   },         /* Gain = 93.691406 Exposure Index = 378 */
         {  24705,   1125   },         /* Gain = 96.503906 Exposure Index = 379 */
         {  25447,   1125   },         /* Gain = 99.402344 Exposure Index = 380 */
         {  26211,   1125   },         /* Gain = 102.386719 Exposure Index = 381 */
         {  26998,   1125   },         /* Gain = 105.460938 Exposure Index = 382 */
         {  27808,   1125   },         /* Gain = 108.625000 Exposure Index = 383 */
         {  28643,   1125   },         /* Gain = 111.886719 Exposure Index = 384 */
         {  29503,   1125   },         /* Gain = 115.246094 Exposure Index = 385 */
         {  30389,   1125   },         /* Gain = 118.707031 Exposure Index = 386 */
         {  31301,   1125   },         /* Gain = 122.269531 Exposure Index = 387 */
         {  32241,   1125   },         /* Gain = 125.941406 Exposure Index = 388 */
         {  33209,   1125   },         /* Gain = 129.722656 Exposure Index = 389 */
         {  34206,   1125   },         /* Gain = 133.617188 Exposure Index = 390 */
         {  35233,   1125   },         /* Gain = 137.628906 Exposure Index = 391 */
         {  36290,   1125   },         /* Gain = 141.757812 Exposure Index = 392 */
         {  37379,   1125   },         /* Gain = 146.011719 Exposure Index = 393 */
         {  38501,   1125   },         /* Gain = 150.394531 Exposure Index = 394 */
         {  39657,   1125   },         /* Gain = 154.910156 Exposure Index = 395 */
         {  40847,   1125   },         /* Gain = 159.558594 Exposure Index = 396 */
         {  42073,   1125   },         /* Gain = 164.347656 Exposure Index = 397 */
         {  43336,   1125   },         /* Gain = 169.281250 Exposure Index = 398 */
         {  44637,   1125   },         /* Gain = 174.363281 Exposure Index = 399 */
         {  45977,   1125   },         /* Gain = 179.597656 Exposure Index = 400 */
         {  47357,   1125   },         /* Gain = 184.988281 Exposure Index = 401 */
         {  48778,   1125   },         /* Gain = 190.539062 Exposure Index = 402 */
         {  50242,   1125   },         /* Gain = 196.257812 Exposure Index = 403 */
         {  51750,   1125   },         /* Gain = 202.148438 Exposure Index = 404 */
         {  53303,   1125   },         /* Gain = 208.214844 Exposure Index = 405 */
         {  54903,   1125   },         /* Gain = 214.464844 Exposure Index = 406 */
         {  56551,   1125   },         /* Gain = 220.902344 Exposure Index = 407 */
         {  58248,   1125   },         /* Gain = 227.531250 Exposure Index = 408 */
         {  59996,   1125   },         /* Gain = 234.359375 Exposure Index = 409 */
         {  61796,   1125   },         /* Gain = 241.390625 Exposure Index = 410 */
         {  63650,   1125   },         /* Gain = 248.632812 Exposure Index = 411 */
         {  65560,   1125   },         /* Gain = 256.093750 Exposure Index = 412 */
         {  67527,   1125   },         /* Gain = 263.777344 Exposure Index = 413 */
         {  69553,   1125   },         /* Gain = 271.691406 Exposure Index = 414 */
         {  71640,   1125   },         /* Gain = 279.843750 Exposure Index = 415 */
         {  73790,   1125   },         /* Gain = 288.242188 Exposure Index = 416 */
         {  76004,   1125   },         /* Gain = 296.890625 Exposure Index = 417 */
         {  78285,   1125   },         /* Gain = 305.800781 Exposure Index = 418 */
         {  80634,   1125   },         /* Gain = 314.976562 Exposure Index = 419 */
         {  83054,   1125   },         /* Gain = 324.429688 Exposure Index = 420 */
         {  85546,   1125   },         /* Gain = 334.164062 Exposure Index = 421 */
         {  88113,   1125   },         /* Gain = 344.191406 Exposure Index = 422 */
         {  90757,   1125   },         /* Gain = 354.519531 Exposure Index = 423 */
         {  93480,   1125   },         /* Gain = 365.156250 Exposure Index = 424 */
         {  96285,   1125   },         /* Gain = 376.113281 Exposure Index = 425 */
         {  99174,   1125   },         /* Gain = 387.398438 Exposure Index = 426 */
         { 102150,   1125   },         /* Gain = 399.023438 Exposure Index = 427 */
         { 105215,   1125   },         /* Gain = 410.996094 Exposure Index = 428 */
         { 108372,   1125   },         /* Gain = 423.328125 Exposure Index = 429 */
         { 111624,   1125   },         /* Gain = 436.031250 Exposure Index = 430 */
         { 114973,   1125   },         /* Gain = 449.113281 Exposure Index = 431 */
         { 118423,   1125   },         /* Gain = 462.589844 Exposure Index = 432 */
         { 121976,   1125   },         /* Gain = 476.468750 Exposure Index = 433 */
         { 125636,   1125   },         /* Gain = 490.765625 Exposure Index = 434 */
         { 129406,   1125   },         /* Gain = 505.492188 Exposure Index = 435 */
         { 133289,   1125   },         /* Gain = 520.660156 Exposure Index = 436 */
         { 137288,   1125   },         /* Gain = 536.281250 Exposure Index = 437 */
         { 141407,   1125   },         /* Gain = 552.371094 Exposure Index = 438 */
         { 145650,   1125   },         /* Gain = 568.945312 Exposure Index = 439 */
         { 150020,   1125   },         /* Gain = 586.015625 Exposure Index = 440 */
         { 154521,   1125   },         /* Gain = 603.597656 Exposure Index = 441 */
         { 159157,   1125   },         /* Gain = 621.707031 Exposure Index = 442 */
         { 163932,   1125   },         /* Gain = 640.359375 Exposure Index = 443 */
         { 168850,   1125   },         /* Gain = 659.570312 Exposure Index = 444 */
         { 173916,   1125   },         /* Gain = 679.359375 Exposure Index = 445 */
         { 179134,   1125   },         /* Gain = 699.742188 Exposure Index = 446 */
         { 184509,   1125   },         /* Gain = 720.738281 Exposure Index = 447 */
         { 190045,   1125   },         /* Gain = 742.363281 Exposure Index = 448 */
         { 195747,   1125   },         /* Gain = 764.636719 Exposure Index = 449 */
         { 201620,   1125   },         /* Gain = 787.578125 Exposure Index = 450 */
         { 207669,   1125   },         /* Gain = 811.207031 Exposure Index = 451 */
         { 213900,   1125   },         /* Gain = 835.546875 Exposure Index = 452 */
         { 220317,   1125   },         /* Gain = 860.613281 Exposure Index = 453 */
         { 226927,   1125   },         /* Gain = 886.433594 Exposure Index = 454 */
         { 233735,   1125   },         /* Gain = 913.027344 Exposure Index = 455 */
         { 240748,   1125   },         /* Gain = 940.421875 Exposure Index = 456 */
         { 247971,   1125   },         /* Gain = 968.636719 Exposure Index = 457 */
         { 255411,   1125   },         /* Gain = 997.699219 Exposure Index = 458 */
         { 263074,   1125   },         /* Gain = 1027.632812 Exposure Index = 459 */
         { 270967,   1125   },         /* Gain = 1058.464844 Exposure Index = 460 */
         { 279097,   1125   },         /* Gain = 1090.222656 Exposure Index = 461 */
         { 287470,   1125   },         /* Gain = 1122.929688 Exposure Index = 462 */
         { 296095,   1125   },         /* Gain = 1156.621094 Exposure Index = 463 */
         { 304978,   1125   },         /* Gain = 1191.320312 Exposure Index = 464 */
         { 314128,   1125   },         /* Gain = 1227.062500 Exposure Index = 465 */
         { 323552,   1125   },         /* Gain = 1263.875000 Exposure Index = 466 */
         { 333259,   1125   },         /* Gain = 1301.792969 Exposure Index = 467 */
         { 343257,   1125   },         /* Gain = 1340.847656 Exposure Index = 468 */
         { 353555,   1125   },         /* Gain = 1381.074219 Exposure Index = 469 */
         { 364162,   1125   },         /* Gain = 1422.507812 Exposure Index = 470 */
         { 375087,   1125   },         /* Gain = 1465.183594 Exposure Index = 471 */
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
         8,/* count */
         /* luma target lut */
         {
            {180,49},
            {200,44},
            {230,40},
            {280,32},
            {300,40},
            {330,30},
            {360,30},
            {430,30},
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
         298,
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
         1,/* use_face_pra */
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
         1,/* 0:mesh 1:hw_face 2:software face */
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
         2,//bright_dark_count
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
               1,//dark_th_low_w
               1,//dark_th_high_w
               1,//bright_th_low_w
               1,//bright_th_high_w
            },
            {
               160,//lux_index
               15,//dark_th_low
               30,//dark_th_high
               220,//bright_th_low
               240,//bright_th_high
               1,//dark_th_low_w
               1,//dark_th_high_w
               2,//bright_th_low_w
               2,//bright_th_high_w
            },
         },
      },
      1,//aec algo type,0:fast smooth  1:fast plus
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
      { 0.4245, 0.5749 },   /* d75 */
      { 0.4803, 0.5107 },   /* d65 */
      { 0.5621, 0.4560 },   /* d50 */
      { 0.5607, 0.4609 },   /* noon */
      { 0.5241, 0.3324 },   /* cw */
      { 0.6081, 0.3449 },   /* tl84 */
      { 0.8204, 0.2956 },   /* a */
      { 0.8926, 0.2370 },   /* h */
      { 0.0000, 0.0000 },   /* custom1 */
      { 0.0000, 0.0000 },   /* custom2 */
   },
   /* awb_gain_adj */
   {
      { 1.0000, 1.0000 },   /* d75 */
      { 1.0300, 1.0100 },   /* d65 */
      { 1.0300, 1.0100 },   /* d50 */
      { 1.0300, 1.0100 },   /* noon */
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
      { 1, 1, 1, 2 },   /* d75 */
      { 5, 5, 3, 1 },   /* d65 */
      { 6, 6, 6, 0 },   /* d50 */
      { 2, 2, 2, 0 },   /* noon */
      { 1, 1, 1, 0 },   /* cw */
      { 2, 2, 2, 0 },   /* tl84 */
      { 1, 1, 1, 0 },   /* a */
      { 0, 0, 0, 0 },   /* h */
      { 0, 0, 0, 0 },   /* custom1 */
      { 0, 0, 0, 0 },   /* custom2 */
   },
   /* awb_lowlight_lut_t awb_lowlight_lut[AWB_LOWLIGHT_LUT_COUNT] */
   {
      /* awb_lowlight_lut 0 */
      {
         390,//lux index
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
      180,   /* outdoor_index */
      190,   /* inoutdoor_midpoint */
      350,   /* indoor_index */
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
            0.1,//weight
         },
      },
      1,   /* enable_gain_filter */
      1,   /* awb_gain_filter_count */
      /* stats_filter_t awb_gain_filter_weight[5] */
      {
         /* awb_gain_filter_count 0 */
         {
            500,//lux index
            0.1,//weight
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
               2,
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
               2,
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
               4,
               4,
               4,
               4,
               5,
               5,
               5,
               4,
               3,
               3,
               3,
               3,
               3,
               3,
               2,
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
               4,
               4,
               4,
               4,
               2,
               2,
               2,
               2,
               3,
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
            200,//exp_index
            /* hybrid_decision_w */
            {
               4,
               4,
               4,
               4,
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
               2,
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
               1,
               1,
               1,
               1,
               1,
               0.8,
               0.5,
               0.2,
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
      0.05,//keep_th_no_grey_to_grey
      0.05,//keep_th_grey_to_no_grey
      1,
      4,
      4,
   },
   1,//enable_gain_adjust
   0,//enable_lowlight_lut
   1,//lowlight_lut_count
   0,//force_simple_grey_word
   1,//enable_init_point
   25,//use_init_point_count
   /* init point */
   {
      0.56,//rg
      0.46,//bg
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
      0,//misleading.enable
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
             200,//lux_index
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
         /* misleading region 3 */
         {
             300,//lux_index
             6,//misleading_region_count
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
                /* awb mesh region 4 */
                {
                    4,//point_num
                    0,//is_square
                    0.562,//min_x
                    0.451,//min_y
                    0.76,//max_x
                    0.587,//max_y
                    /* awb mesh point */
                    {
                       {0.562,0.55},
                       {0.642,0.587},
                       {0.76,0.496},
                       {0.674,0.451},
                    },
                },
                /* awb mesh region 5 */
                {
                    4,//point_num
                    0,//is_square
                    0.525,//min_x
                    0.42,//min_y
                    0.584,//max_x
                    0.485,//max_y
                    /* awb mesh point */
                    {
                       {0.547,0.485},
                       {0.584,0.449},
                       {0.561,0.426},
                       {0.525,0.42},
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
      5,//count
      /* awb_gain_adjust_wb_t gain_adj[HIGH_TRIGGER_COUNT] */
      {
         150,//lux_index
         /* awb_gain_adj */
         {
            { 1.0500, 1.0200 },   /* d75 */
            { 1.0100, 1.0000 },   /* d65 */
            { 1.0100, 1.0000 },   /* d50 */
            { 1.0100, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
            { 1.0000, 1.0000 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 1.0000, 1.0000 },   /* custom1 */
            { 1.0000, 1.0000 },   /* custom2 */
         },
         180,//lux_index
         /* awb_gain_adj */
         {
            { 1.0300, 1.0000 },   /* d75 */
            { 1.0100, 1.0200 },   /* d65 */
            { 1.0100, 1.0000 },   /* d50 */
            { 1.0100, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
            { 1.0000, 1.0000 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 1.0000, 1.0000 },   /* custom1 */
            { 1.0000, 1.0000 },   /* custom2 */
         },
         380,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0000 },   /* d75 */
            { 1.0400, 1.0000 },   /* d65 */
            { 1.0400, 1.0000 },   /* d50 */
            { 1.0400, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
            { 1.0000, 1.0000 },   /* tl84 */
            { 1.0700, 1.0200 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 1.0000, 1.0000 },   /* custom1 */
            { 1.0000, 1.0000 },   /* custom2 */
         },
         430,//lux_index
         /* awb_gain_adj */
         {
            { 1.1000, 1.0000 },   /* d75 */
            { 1.1000, 1.0000 },   /* d65 */
            { 1.2000, 1.1000 },   /* d50 */
            { 1.1000, 1.0000 },   /* noon */
            { 1.2000, 1.0000 },   /* cw */
            { 1.2000, 1.0000 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 1.0000, 1.0000 },   /* custom1 */
            { 1.0000, 1.0000 },   /* custom2 */
         },
         450,//lux_index
         /* awb_gain_adj */
         {
            { 2.0000, 1.0000 },   /* d75 */
            { 2.0000, 1.0000 },   /* d65 */
            { 2.0000, 1.3000 },   /* d50 */
            { 1.0000, 1.0000 },   /* noon */
            { 2.0000, 1.0000 },   /* cw */
            { 2.0000, 1.0000 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 0.0000, 0.0000 },   /* custom1 */
            { 0.0000, 0.0000 },   /* custom2 */
         },
      },
   },
   /*STRU_AWB_SACAN_MODE_T awb_scan_mode;*/
   {
      /*int enable_awb_scan_mode;*/
      0,
      /*int frames_start_scan;*/
      300,
      /* int count;*/
      1,
      /*STRU_AWB_SACAN_PRA_T scan_pra[MIDDLE_TRIGGER_COUNT];*/
      {
        {
              /* int enable;*/
              1,
              /*int lux_index;*/
              500,
              /*int skip_frames*/
              250,
              /*int frames_run_every_scan;*/
              20,
        },
      }
   }
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
   },
   /* af_stats_type 1-AF_STATS_SHARPNES, 2-AF_STATS_FIR, 4-AF_STATS_IIR */
   4,
   /* roi_t roi */
   {
      0.35,//x
      0.3,//y
      0.3,//width
      0.4,//height
   },
   /* stats_weight_table[AF_BLOCK_ROW*AF_BLOCK_COL] 16x9 */
   {
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 2, 3, 2, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 2, 3, 3, 3, 2, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 2, 3, 3, 3, 3, 3, 2, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 2, 3, 3, 3, 2, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 2, 3, 2, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
   },
   /* actuator_tunning_pra_t atcuator */
   {
      1,//enable
      0,//damping_enable
      0,//init_dac
      0,//infinit_dac
      0,//hyperfocal_dac
      512,//near_dac
      0,//dmp_count
      /* damping_t move_to_far_damping[16] */
      {
      },
      /* damping_t move_to_near_damping[16] */
      {
      },
   },
   /* roi_type : 0-GENERAL 1-FACE 2-TOUCH */
   0,
   /* run_mode : 0-INIT 1-CAMERA 2-VIDEO 3-SNAPSHOT */
   2,
   /* af_monit_t af_monit */
   {
      /* af_tuning_sad_t af_par_sad */
      {
         1,//enable
         2,//gain_min
         30,//gain_max
         2,//ref_gain_min
         30,//ref_gain_max
         2,//threshold_min
         3,//threshold_max
         2,//ref_threshold_min
         4,//ref_threshold_max
         5,//frames_to_wait
      },
   },
   /* af_tuning_single_t  single_pra */
   {
      /* single_optic_t optics */
      {
         5,//far_zone
         6,//mid_zone
         7,//near_zone
         8,//init_pos
         9,//far_start_pos
         10,//near_start_pos
         11,//TAF_far_end
         12,//TAF_near_end
         13,//CAF_far_end
         14,//CAF_near_end
         15,//srch_rgn_1
         16,//srch_rgn_2
         17,//srch_rgn_3
         18,//fine_srch_rgn
      },
      /* acturator_type: 0-ACT_TYPE_CLOSELOOP, 1-ACE_TYPE_OPENLOOP */
      1,
      /* index[50] */
      {
         512, 492, 412, 300, 100, 212, 262, 300, 100, 100, 
         512, 100, 512, 100, 512, 300, 200, 100, 50, 0, 
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
      },
      0,//is_hys_comp_needed
      /* single_threshold_t hw */
      {
         40,//hist_dec_dec_thres
         0.6,//drop_thres
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
            0.85,
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
            0.9,
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
            0.85,
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
            0,
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
            0.93,
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
            0.98,
         },
         40,//hist_inc_dec_thres
         0.95,//flat_dec_thres
         1.05,//flat_inc_thres
         1.3,//macro_thres
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
         1000,
      },
      1,//step_index_per_um
      /* step_size_table_t CAF_step_table */
      {
         /* step_rgn_t   Prescan_low_light */
         {
            18, 18, 18, 18, 18,
         },
         /* step_rgn_t   Prescan_normal_light */
         {
            17, 17, 17, 17, 17,
         },
         /* step_rgn_t   Finescan_low_light */
         {
            2, 2, 2, 2, 2,
         },
         /* step_rgn_t   Finescan_normal_light */
         {
            3, 3, 3, 3, 3,
         },
      },
      /* step_size_table_t TAF_step_table */
      {
         /* step_rgn_t   Prescan_low_light */
         {
            16, 16, 16, 16, 16,
         },
         /* step_rgn_t   Prescan_normal_light */
         {
            15, 15, 15, 15, 15,
         },
         /* step_rgn_t   Finescan_low_light */
         {
            4, 4, 4, 4, 4,
         },
         /* step_rgn_t   Finescan_normal_light */
         {
            4, 4, 4, 4, 4,
         },
      },
      /* skip_frame[3] */
      {
         3,//0-large
         1,//1-small
         1,//2-others
      },
   },
   1,//count_ae
   1,//trigger_mode 0-gain, 1-lux
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
         0,//trigger_val not use
         8,//coef_y_b
         8,//coef_y_gb
         8,//coef_y_gr
         8,//coef_y_r
         2,//fv_mode 0-fv_max 1-fv_acc 2-sqr
         1,///ch_sel 0-R 1-GR 2-GB 3-B 4-Y
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
         /* sharp_filter_coff[39] */
         {
            1, 1, 1, 1, 1, 1023, 1023, 1023, 1023, 1023, 1, 1, 1, 1, 1, 1023, 1023, 1023, 1023, 
            1023, 1, 1, 1, 1, 1, 1023, 1023, 1023, 1023, 1023, 1, 1, 1, 1, 1, 1023, 1023, 1023, 
            1023, 
         },
         0,//fir_th
         0,//fir_bitshift
         0,//fir_sqr_bitshift
         /* fir_filter_coff[3] */
         {
            1023,
            2,
            1023,
         },
         1,//iir1_en
         1,//iir0_en
         14,//iir_bitshift_2
         14,//iir_bitshift_1
         0,//iir_th
         /* iir_filter_coff[10] */
         {
            10868,
            240409,
            10868,
            30536,
            247611,
            20111,
            221920,
            20111,
            28277,
            249899,
         },
         0,//iir_th
         /* gamma[AF_STATS_TABLE_SIZE] 256 */
         {
            16384, 0, 0, 0, 32784, 0, 0, 0, 49184, 0, 0, 0, 65584, 0, 0, 0, 
            81984, 0, 0, 0, 98384, 0, 0, 0, 114784, 0, 0, 0, 131184, 0, 0, 0, 
            147584, 0, 0, 0, 163984, 0, 0, 0, 180384, 0, 0, 0, 196784, 0, 0, 0, 
            213184, 0, 0, 0, 229584, 0, 0, 0, 245984, 0, 0, 0, 262384, 0, 0, 0, 
            278784, 0, 0, 0, 295184, 0, 0, 0, 311584, 0, 0, 0, 327984, 0, 0, 0, 
            344384, 0, 0, 0, 360784, 0, 0, 0, 377184, 0, 0, 0, 393584, 0, 0, 0, 
            409984, 0, 0, 0, 426384, 0, 0, 0, 442784, 0, 0, 0, 459184, 0, 0, 0, 
            475584, 0, 0, 0, 491984, 0, 0, 0, 508384, 0, 0, 0, 524784, 0, 0, 0, 
            541184, 0, 0, 0, 557584, 0, 0, 0, 573984, 0, 0, 0, 590384, 0, 0, 0, 
            606784, 0, 0, 0, 623184, 0, 0, 0, 639584, 0, 0, 0, 655984, 0, 0, 0, 
            672384, 0, 0, 0, 688784, 0, 0, 0, 705184, 0, 0, 0, 721584, 0, 0, 0, 
            737984, 0, 0, 0, 754384, 0, 0, 0, 770784, 0, 0, 0, 787184, 0, 0, 0, 
            803584, 0, 0, 0, 819984, 0, 0, 0, 836384, 0, 0, 0, 852784, 0, 0, 0, 
            869184, 0, 0, 0, 885584, 0, 0, 0, 901984, 0, 0, 0, 918384, 0, 0, 0, 
            934784, 0, 0, 0, 951184, 0, 0, 0, 967584, 0, 0, 0, 983984, 0, 0, 0, 
            1000384, 0, 0, 0, 1016784, 0, 0, 0, 1033184, 0, 0, 0, 1048560, 0, 0, 0, 
         },
      },
   },
},
