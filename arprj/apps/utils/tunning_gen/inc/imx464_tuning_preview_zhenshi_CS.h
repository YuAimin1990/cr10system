
/*---------------header of imx307 tuning-------------------------*/

/* version of the tuning header uint32_t header_version */
0x20220303,
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
               48,
           },
           /* aec trigger 1*/
           {
               64,
               64.1,
           },
           /* aec trigger 2*/
           {
               512,
               512.1,
           },
           /* aec trigger 3*/
           {
               1024,
               1024.1,
           },
           /* aec trigger 4*/
           {
               2000,
               2000.1,
           },
       },
       /* isp_sub_module_blc_para para[MIDDLE_TRIGGER_COUNT] MIDDLE_TRIGGER_COUNT==5 */
       {
           /* para 0*/
           {
               790,//R
               790,//B
               790,//GR
               790,//GB
               270,//gian R
               270,//gain B
               270,//gain GR
               270,//gain GB
           },
           /* para 1*/
           {
               820,//R
               790,//B
               790,//GR
               790,//GB
               271,//gian R
               271,//gain B
               271,//gain GR
               271,//gain GB
           },
           /* para 2*/
           {
               962,//R
               962,//B
               964,//GR
               957,//GB
               272,//gian R
               272,//gain B
               273,//gain GR
               272,//gain GB
           },
           /* para 3*/
           {
               1792,//R
               1787,//B
               1598,//GR
               1584,//GB
               282,//gian R
               282,//gain B
               282,//gain GR
               282,//gain GB
           },
           /* para 4*/
           {
               2391,//R
               2386,//B
               2330,//GR
               2326,//GB
               291,//gian R
               291,//gain B
               291,//gain GR
               291,//gain GB
           },
       },
    },
    /* isp_sub_module_hdr_mix_tuning_t isp_sub_module_hdr_mix_tuning */
    {
       /* enable */
       0,
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
       3,
       /* 0 for dynamic dpc, 1 for static dpc */
       0,
       /* tigger_mode : 0 gain ,1 lux */
       0,
       /* region enable */
       {
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
             40,
             50.1,
          },
          /* aec trigger 7 */
          {
             64,
             64.1,
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
          /* aec trigger 11 */
          {
             1500,
             1500.1,
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
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
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
               0,//profile_enable
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
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {70,70,70},
                       /* channel 2 */
                       {70,70,70},
                       /* channel 3 */
                       {70,70,70},
                       /* channel 4 */
                       {70,70,70},
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
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {95,95,95},
                       /* channel 2 */
                       {95,95,95},
                       /* channel 3 */
                       {95,95,95},
                       /* channel 4 */
                       {95,95,95},
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
               2,//denoise_layer
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
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {200,200,200},
                       /* channel 2 */
                       {200,200,200},
                       /* channel 3 */
                       {200,200,200},
                       /* channel 4 */
                       {200,200,200},
                   },
                   /* layer 2 */
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
           /* para 9*/
           {
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
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
           /* para 10*/
           {
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {180,180,180},
                       /* channel 2 */
                       {180,180,180},
                       /* channel 3 */
                       {180,180,180},
                       /* channel 4 */
                       {180,180,180},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {130,130,130},
                       /* channel 2 */
                       {130,130,130},
                       /* channel 3 */
                       {130,130,130},
                       /* channel 4 */
                       {130,130,130},
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
           /* para 11*/
           {
               1,//denoise_layer
               0,//profile_enable
               /* isp_sub_module_rnr_lay lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT] */
               {
                   /* layer 1 */
                   {
                       /* channel 1 */
                       {180,180,180},
                       /* channel 2 */
                       {180,180,180},
                       /* channel 3 */
                       {180,180,180},
                       /* channel 4 */
                       {180,180,180},
                   },
                   /* layer 2 */
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
             340,
          },
          /* aec trigger 1 */
          {
             250,
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
                0.5,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   1.4177, 1.2529, 1.1806, 1.1032, 1.0937, 1.0700, 1.1022, 1.1275, 1.2109, 1.3310, 
                   1.3405, 1.2138, 1.1378, 1.0734, 1.0445, 1.0491, 1.0563, 1.0942, 1.1623, 1.2624, 
                   1.3239, 1.1897, 1.1141, 1.0612, 1.0306, 1.0291, 1.0408, 1.0819, 1.1397, 1.2709, 
                   1.2880, 1.1745, 1.0903, 1.0312, 1.0130, 1.0057, 1.0276, 1.0665, 1.1184, 1.2262, 
                   1.2974, 1.1605, 1.0839, 1.0345, 0.9986, 0.9967, 1.0183, 1.0561, 1.1205, 1.2135, 
                   1.2639, 1.1571, 1.0823, 1.0263, 1.0095, 0.9893, 1.0239, 1.0607, 1.1167, 1.2308, 
                   1.3039, 1.1599, 1.0935, 1.0458, 1.0103, 1.0075, 1.0224, 1.0649, 1.1290, 1.2256, 
                   1.3027, 1.1876, 1.1071, 1.0717, 1.0316, 1.0261, 1.0467, 1.0837, 1.1484, 1.2420, 
                   1.3371, 1.2066, 1.1258, 1.0753, 1.0474, 1.0433, 1.0592, 1.0952, 1.1731, 1.2708, 
                   1.3871, 1.2517, 1.1643, 1.1027, 1.0972, 1.0726, 1.0967, 1.1426, 1.2134, 1.2959, 
                   1.3653, 1.2197, 1.1533, 1.0840, 1.0764, 1.0531, 1.0779, 1.0993, 1.1844, 1.2364, 
                   1.2834, 1.2000, 1.1084, 1.0798, 1.0392, 1.0433, 1.0581, 1.0881, 1.1441, 1.2476, 
                   1.2933, 1.1664, 1.1030, 1.0482, 1.0297, 1.0251, 1.0386, 1.0755, 1.1293, 1.2199, 
                   1.2514, 1.1562, 1.0765, 1.0355, 1.0121, 1.0036, 1.0275, 1.0613, 1.1106, 1.2055, 
                   1.2733, 1.1362, 1.0816, 1.0295, 1.0092, 0.9970, 1.0270, 1.0538, 1.1115, 1.1947, 
                   1.2501, 1.1365, 1.0805, 1.0203, 1.0144, 0.9963, 1.0154, 1.0620, 1.1088, 1.1989, 
                   1.2606, 1.1440, 1.0833, 1.0519, 1.0134, 1.0094, 1.0321, 1.0580, 1.1226, 1.2030, 
                   1.2633, 1.1661, 1.0984, 1.0564, 1.0289, 1.0260, 1.0364, 1.0760, 1.1329, 1.2252, 
                   1.3038, 1.1868, 1.1157, 1.0826, 1.0482, 1.0407, 1.0681, 1.0870, 1.1681, 1.2349, 
                   1.2862, 1.2287, 1.1254, 1.0917, 1.0728, 1.0770, 1.0768, 1.1201, 1.1778, 1.2900, 
                   1.3757, 1.2277, 1.1498, 1.0876, 1.0711, 1.0766, 1.0751, 1.1206, 1.1917, 1.2619, 
                   1.2796, 1.2135, 1.1157, 1.0878, 1.0423, 1.0497, 1.0621, 1.0929, 1.1527, 1.2599, 
                   1.2757, 1.1745, 1.1002, 1.0501, 1.0331, 1.0281, 1.0422, 1.0821, 1.1386, 1.2358, 
                   1.2683, 1.1705, 1.0907, 1.0386, 1.0197, 1.0087, 1.0347, 1.0606, 1.1232, 1.1944, 
                   1.2366, 1.1520, 1.0793, 1.0322, 1.0071, 0.9997, 1.0287, 1.0652, 1.1196, 1.2273, 
                   1.2700, 1.1508, 1.0760, 1.0371, 1.0072, 1.0069, 1.0234, 1.0624, 1.1113, 1.1940, 
                   1.2525, 1.1586, 1.0926, 1.0525, 1.0165, 1.0160, 1.0256, 1.0736, 1.1175, 1.2379, 
                   1.2557, 1.1807, 1.0893, 1.0717, 1.0234, 1.0362, 1.0469, 1.0809, 1.1394, 1.2307, 
                   1.3143, 1.1921, 1.1231, 1.0827, 1.0546, 1.0642, 1.0597, 1.1130, 1.1568, 1.2663, 
                   1.2867, 1.2339, 1.1296, 1.1109, 1.0755, 1.0699, 1.1074, 1.1118, 1.2114, 1.2622, 
                },
             },
             /* awb para 1 */
             {
                0.5,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   1.2780, 1.1869, 1.1250, 1.0783, 1.0645, 1.0879, 1.0975, 1.1780, 1.3046, 1.5195, 
                   1.2611, 1.1553, 1.0975, 1.0589, 1.0383, 1.0439, 1.0730, 1.1353, 1.2581, 1.4559, 
                   1.2116, 1.1394, 1.0699, 1.0384, 1.0200, 1.0337, 1.0556, 1.1187, 1.2260, 1.4171, 
                   1.2058, 1.1175, 1.0591, 1.0174, 1.0060, 1.0098, 1.0408, 1.0950, 1.1945, 1.3721, 
                   1.2009, 1.1101, 1.0468, 1.0139, 0.9941, 0.9969, 1.0339, 1.0859, 1.1814, 1.3379, 
                   1.2050, 1.1037, 1.0494, 1.0073, 1.0011, 0.9921, 1.0274, 1.0788, 1.1716, 1.3332, 
                   1.2035, 1.1024, 1.0523, 1.0328, 0.9889, 1.0128, 1.0325, 1.0850, 1.1747, 1.3228, 
                   1.2321, 1.1275, 1.0659, 1.0432, 1.0102, 1.0185, 1.0406, 1.0900, 1.1780, 1.3167, 
                   1.2279, 1.1339, 1.0777, 1.0468, 1.0225, 1.0338, 1.0566, 1.1035, 1.1839, 1.3459, 
                   1.2808, 1.1743, 1.1093, 1.0719, 1.0555, 1.0561, 1.0822, 1.1256, 1.2228, 1.3508, 
                   1.2731, 1.1849, 1.1100, 1.0668, 1.0674, 1.0780, 1.0953, 1.1792, 1.3279, 1.4862, 
                   1.2493, 1.1565, 1.0926, 1.0626, 1.0366, 1.0563, 1.0796, 1.1373, 1.2673, 1.4504, 
                   1.2294, 1.1327, 1.0757, 1.0324, 1.0258, 1.0299, 1.0642, 1.1287, 1.2376, 1.4286, 
                   1.2000, 1.1295, 1.0555, 1.0286, 1.0052, 1.0228, 1.0418, 1.1070, 1.2027, 1.3802, 
                   1.2156, 1.1057, 1.0588, 1.0086, 1.0047, 0.9972, 1.0385, 1.0872, 1.1879, 1.3463, 
                   1.1906, 1.1113, 1.0477, 1.0176, 1.0038, 0.9991, 1.0344, 1.0924, 1.1766, 1.3468, 
                   1.2086, 1.1076, 1.0554, 1.0316, 0.9943, 1.0151, 1.0282, 1.0893, 1.1795, 1.3280, 
                   1.2099, 1.1257, 1.0655, 1.0369, 1.0192, 1.0175, 1.0528, 1.0944, 1.1828, 1.3325, 
                   1.2378, 1.1481, 1.0812, 1.0622, 1.0228, 1.0459, 1.0542, 1.1081, 1.1998, 1.3343, 
                   1.2277, 1.1624, 1.0874, 1.0515, 1.0539, 1.0487, 1.0911, 1.1276, 1.2286, 1.3463, 
                   1.2969, 1.1801, 1.1217, 1.0712, 1.0857, 1.0561, 1.1048, 1.1663, 1.3209, 1.4974, 
                   1.2123, 1.1654, 1.0850, 1.0637, 1.0404, 1.0498, 1.0990, 1.1429, 1.2727, 1.4765, 
                   1.2134, 1.1370, 1.0689, 1.0386, 1.0277, 1.0257, 1.0602, 1.1189, 1.2327, 1.4140, 
                   1.2118, 1.1240, 1.0625, 1.0225, 1.0075, 1.0210, 1.0509, 1.1089, 1.1977, 1.3856, 
                   1.1997, 1.1059, 1.0538, 1.0108, 1.0052, 0.9910, 1.0424, 1.0936, 1.1870, 1.3621, 
                   1.2022, 1.1138, 1.0541, 1.0224, 0.9997, 1.0123, 1.0329, 1.0865, 1.1848, 1.3231, 
                   1.2011, 1.1062, 1.0605, 1.0292, 1.0030, 1.0063, 1.0353, 1.0907, 1.1690, 1.3475, 
                   1.1969, 1.1309, 1.0637, 1.0497, 1.0066, 1.0321, 1.0526, 1.1068, 1.1858, 1.3521, 
                   1.2233, 1.1480, 1.0751, 1.0551, 1.0316, 1.0346, 1.0726, 1.1078, 1.2039, 1.3438, 
                   1.2283, 1.1748, 1.1017, 1.0712, 1.0501, 1.0705, 1.1004, 1.1364, 1.2193, 1.3801, 
                },
             },
             /* awb para 2 */
             {
                0.5,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   1.4391, 1.2802, 1.1719, 1.1258, 1.0743, 1.0435, 1.0854, 1.0699, 1.1733, 1.2429, 
                   1.3701, 1.2260, 1.1467, 1.0711, 1.0481, 1.0352, 1.0445, 1.0658, 1.1242, 1.1952, 
                   1.3332, 1.2199, 1.1199, 1.0715, 1.0322, 1.0139, 1.0270, 1.0517, 1.0975, 1.2021, 
                   1.3356, 1.1830, 1.1056, 1.0409, 1.0137, 1.0028, 1.0197, 1.0423, 1.0878, 1.1736, 
                   1.2953, 1.1873, 1.0987, 1.0357, 1.0149, 0.9856, 1.0022, 1.0342, 1.0783, 1.1566, 
                   1.3126, 1.1759, 1.0985, 1.0434, 1.0083, 0.9926, 1.0072, 1.0419, 1.0812, 1.1733, 
                   1.3078, 1.1882, 1.1064, 1.0568, 1.0127, 1.0064, 1.0092, 1.0482, 1.0940, 1.1723, 
                   1.3286, 1.2126, 1.1192, 1.0857, 1.0286, 1.0254, 1.0303, 1.0580, 1.1051, 1.1979, 
                   1.3553, 1.2321, 1.1358, 1.0881, 1.0498, 1.0409, 1.0479, 1.0790, 1.1313, 1.2018, 
                   1.3747, 1.2904, 1.1711, 1.1278, 1.0855, 1.0922, 1.0865, 1.1266, 1.1850, 1.2797, 
                   1.3602, 1.2334, 1.1511, 1.0865, 1.0682, 1.0359, 1.0514, 1.0752, 1.1351, 1.1928, 
                   1.3205, 1.2126, 1.1207, 1.0768, 1.0348, 1.0362, 1.0374, 1.0594, 1.1033, 1.1861, 
                   1.2849, 1.1888, 1.1109, 1.0535, 1.0310, 1.0143, 1.0243, 1.0477, 1.0908, 1.1652, 
                   1.2886, 1.1749, 1.0907, 1.0450, 1.0089, 1.0019, 1.0095, 1.0421, 1.0713, 1.1516, 
                   1.2689, 1.1623, 1.0914, 1.0307, 1.0140, 0.9939, 1.0101, 1.0305, 1.0743, 1.1341, 
                   1.2799, 1.1522, 1.0909, 1.0414, 1.0075, 0.9891, 1.0054, 1.0306, 1.0752, 1.1330, 
                   1.2825, 1.1636, 1.1036, 1.0543, 1.0175, 1.0049, 1.0159, 1.0390, 1.0854, 1.1461, 
                   1.3019, 1.1854, 1.1064, 1.0707, 1.0252, 1.0159, 1.0249, 1.0493, 1.0977, 1.1577, 
                   1.3174, 1.2051, 1.1317, 1.0862, 1.0465, 1.0434, 1.0419, 1.0689, 1.1175, 1.1902, 
                   1.3298, 1.2438, 1.1308, 1.1105, 1.0701, 1.0569, 1.0725, 1.0920, 1.1454, 1.1967, 
                   1.3661, 1.2293, 1.1495, 1.0821, 1.0619, 1.0426, 1.0675, 1.0886, 1.1342, 1.2489, 
                   1.2982, 1.2031, 1.1224, 1.0816, 1.0375, 1.0368, 1.0411, 1.0530, 1.1027, 1.1651, 
                   1.2772, 1.1842, 1.1026, 1.0525, 1.0213, 1.0161, 1.0232, 1.0557, 1.0854, 1.1889, 
                   1.2751, 1.1685, 1.0932, 1.0368, 1.0140, 0.9974, 1.0158, 1.0366, 1.0732, 1.1386, 
                   1.2668, 1.1587, 1.0877, 1.0366, 1.0116, 0.9897, 1.0149, 1.0348, 1.0727, 1.1439, 
                   1.2675, 1.1530, 1.0893, 1.0364, 1.0050, 0.9948, 1.0047, 1.0352, 1.0735, 1.1301, 
                   1.2746, 1.1664, 1.0970, 1.0621, 1.0134, 1.0018, 1.0181, 1.0380, 1.0823, 1.1652, 
                   1.2675, 1.1831, 1.0979, 1.0761, 1.0184, 1.0244, 1.0332, 1.0550, 1.1007, 1.1678, 
                   1.3128, 1.2009, 1.1233, 1.0905, 1.0410, 1.0418, 1.0411, 1.0740, 1.1105, 1.1978, 
                   1.2914, 1.2429, 1.1416, 1.1131, 1.0589, 1.0584, 1.0838, 1.0862, 1.1582, 1.2100, 
                },
             },
             /* awb para 3 */
             {
                0.5,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   1.4110, 1.2929, 1.1770, 1.1334, 1.0816, 1.0675, 1.0964, 1.0831, 1.1751, 1.1841, 
                   1.3826, 1.2344, 1.1600, 1.0780, 1.0577, 1.0402, 1.0494, 1.0781, 1.1217, 1.2405, 
                   1.3353, 1.2339, 1.1323, 1.0765, 1.0429, 1.0237, 1.0404, 1.0575, 1.1065, 1.1765, 
                   1.3248, 1.1922, 1.1182, 1.0453, 1.0211, 1.0037, 1.0175, 1.0461, 1.0836, 1.1882, 
                   1.3089, 1.1926, 1.0972, 1.0407, 1.0107, 0.9928, 1.0046, 1.0365, 1.0753, 1.1450, 
                   1.3182, 1.1817, 1.1134, 1.0359, 1.0193, 0.9791, 1.0093, 1.0375, 1.0811, 1.1714, 
                   1.3136, 1.2018, 1.1107, 1.0710, 1.0159, 1.0156, 1.0213, 1.0509, 1.0979, 1.1764, 
                   1.3603, 1.2142, 1.1397, 1.0787, 1.0432, 1.0163, 1.0280, 1.0598, 1.0968, 1.1899, 
                   1.3456, 1.2415, 1.1377, 1.1019, 1.0472, 1.0457, 1.0625, 1.0794, 1.1415, 1.2269, 
                   1.3926, 1.2741, 1.1770, 1.1128, 1.0864, 1.0667, 1.0575, 1.1179, 1.1356, 1.2521, 
                   1.3333, 1.2369, 1.1406, 1.0881, 1.0580, 1.0381, 1.0526, 1.0718, 1.1297, 1.1945, 
                   1.3450, 1.2158, 1.1380, 1.0877, 1.0453, 1.0462, 1.0484, 1.0594, 1.1145, 1.1697, 
                   1.2955, 1.1951, 1.1137, 1.0572, 1.0328, 1.0143, 1.0286, 1.0503, 1.0932, 1.1696, 
                   1.3018, 1.1693, 1.0987, 1.0460, 1.0130, 1.0058, 1.0150, 1.0400, 1.0730, 1.1439, 
                   1.2662, 1.1665, 1.0883, 1.0263, 1.0101, 0.9850, 1.0041, 1.0208, 1.0734, 1.1145, 
                   1.2897, 1.1543, 1.0966, 1.0396, 1.0179, 0.9908, 1.0083, 1.0301, 1.0736, 1.1352, 
                   1.2731, 1.1769, 1.0959, 1.0607, 1.0147, 1.0115, 1.0113, 1.0445, 1.0816, 1.1474, 
                   1.3162, 1.1847, 1.1199, 1.0761, 1.0321, 1.0233, 1.0302, 1.0530, 1.0991, 1.1634, 
                   1.3039, 1.2140, 1.1288, 1.0871, 1.0542, 1.0449, 1.0453, 1.0737, 1.1166, 1.1923, 
                   1.3283, 1.2221, 1.1349, 1.0967, 1.0504, 1.0562, 1.0554, 1.0771, 1.1294, 1.2008, 
                   1.3423, 1.2304, 1.1424, 1.0875, 1.0655, 1.0426, 1.0686, 1.0718, 1.1446, 1.1968, 
                   1.3188, 1.2141, 1.1354, 1.1004, 1.0397, 1.0518, 1.0416, 1.0777, 1.1016, 1.1979, 
                   1.2858, 1.1944, 1.1107, 1.0602, 1.0313, 1.0203, 1.0339, 1.0500, 1.1003, 1.1644, 
                   1.2873, 1.1739, 1.0938, 1.0469, 1.0137, 1.0061, 1.0157, 1.0477, 1.0675, 1.1470, 
                   1.2552, 1.1660, 1.0864, 1.0375, 0.9998, 0.9935, 1.0044, 1.0290, 1.0731, 1.1353, 
                   1.2774, 1.1620, 1.0925, 1.0350, 1.0144, 0.9903, 1.0120, 1.0353, 1.0743, 1.1365, 
                   1.2733, 1.1726, 1.1046, 1.0636, 1.0204, 1.0116, 1.0239, 1.0421, 1.0868, 1.1505, 
                   1.2705, 1.1928, 1.1073, 1.0790, 1.0286, 1.0271, 1.0354, 1.0556, 1.1046, 1.1625, 
                   1.3181, 1.2059, 1.1343, 1.0879, 1.0494, 1.0480, 1.0486, 1.0760, 1.1166, 1.1880, 
                   1.2720, 1.2098, 1.1304, 1.0759, 1.0649, 1.0492, 1.0732, 1.0633, 1.1486, 1.1775, 
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
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
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
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
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
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
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
       1,//enable
       1,//interpolation_enable
       7,//ae count
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
             170,
          },
          /* aec trigger 2 */
          {
             230,
             290,
          },
          /* aec trigger 3 */
          {
             320,
             330,
          },
          /* aec trigger 4 */
          {
             340,
             370,
          },
          /* aec trigger 5 */
          {
             385,
             450,
          },
          /* aec trigger 6 */
          {
             460,
             500,
          },
       },
       /* isp_sub_module_drc_pra pra[MIDDLE_TRIGGER_COUNT] */
       {
           /* para 0*/
           {
              /* drc_low_LUT_point */
              {
                   4497,4497,4254,4058,3888,3737,3600,3475,3359,3250,3149,3054,2964,2879,2798,2721,2648,2578,2511,2447,2385,2326,2269,2214,2161,
                   2110,2061,2014,1968,1924,1881,1840,1799,1760,1723,1686,1650,1616,1582,1550,1518,1487,1458,1428,1400,1372,1346,1319,1294,1269,
                   1245,1221,1198,1176,1154,1132,1111,1091,1071,1051,1032,1014, 996, 978, 961, 944, 927, 911, 895, 879, 864, 849, 835, 821, 807,
                    793, 780, 767, 754, 741, 729, 717, 705, 693, 682, 671, 660, 649, 639, 629, 619, 609, 599, 589, 580, 571, 562, 553, 545, 536,
                    528, 520, 512, 504, 496, 488, 481, 474, 466, 459, 452, 446, 439, 432, 426, 420, 413, 407, 401, 395, 389, 384, 378, 373, 367,
                    362, 357, 351, 346, 341, 336, 332, 327, 322, 318, 313, 309, 304, 300, 296, 292, 288, 284, 280, 276, 272, 268, 264, 261, 257,
                    254, 250, 247, 243, 240, 237, 233, 230, 227, 224, 221, 218, 215, 212, 209, 207, 204, 201, 198, 196, 193, 191, 188, 186, 183,
                    181, 178, 176, 174, 171, 169, 167, 165, 163, 161, 159, 156, 154, 152, 151, 149, 147, 145, 143, 141, 139, 138, 136, 134, 132,
                    131, 129, 127, 126, 124, 123, 121, 120, 118, 117, 115, 114, 112, 111, 110, 108, 107, 106, 104, 103, 102, 100,  99,  98,  97,
                     96,  94,  93,  92,  91,  90,  89,  88,  87,  86,  85,  84,  83,  82,  81,  80,  79,  78,  77,  76,  75,  74,  73,  72,  71,
                     71,  70,  69,  68,  67,  67,  66,
              },
              /* drc_high_LUT_point */
              {
                   2648,2648,1799,1294, 960, 729, 562, 439, 346, 276, 221, 178, 145, 118,  97,  80,  66,  54,  45,  38,  31,  26,  22,  19,  16,
                     13,  11,   9,   8,   7,   6,   5,   4,   3,   3,   2,   2,   2,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,
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
              0.6,
              /* k2 */
              30,
              /* k3 */
              0.7,
           },
           /* para 1*/
           {
              /* drc_low_LUT_point */
              {
                   4885,4885,4855,4826,4796,4767,4738,4709,4681,4652,4624,4596,4568,4540,4512,4485,4457,4430,4403,4377,4350,4324,4297,4271,4245,
                   4219,4194,4168,4143,4118,4092,4068,4043,4018,3994,3969,3945,3921,3897,3874,3850,3827,3803,3780,3757,3734,3712,3689,3667,3644,
                   3622,3600,3578,3557,3535,3513,3492,3471,3450,3429,3408,3387,3366,3346,3326,3305,3285,3265,3245,3226,3206,3187,3167,3148,3129,
                   3110,3091,3072,3053,3035,3016,2998,2980,2961,2943,2926,2908,2890,2872,2855,2838,2820,2803,2786,2769,2752,2736,2719,2702,2686,
                   2670,2653,2637,2621,2605,2589,2574,2558,2542,2527,2512,2496,2481,2466,2451,2436,2421,2407,2392,2377,2363,2348,2334,2320,2306,
                   2292,2278,2264,2250,2237,2223,2209,2196,2183,2169,2156,2143,2130,2117,2104,2091,2079,2066,2053,2041,2028,2016,2004,1992,1980,
                   1968,1956,1944,1932,1920,1908,1897,1885,1874,1862,1851,1840,1829,1817,1806,1795,1784,1774,1763,1752,1741,1731,1720,1710,1699,
                   1689,1679,1669,1658,1648,1638,1628,1618,1609,1599,1589,1579,1570,1560,1551,1541,1532,1523,1513,1504,1495,1486,1477,1468,1459,
                   1450,1441,1432,1424,1415,1406,1398,1389,1381,1373,1364,1356,1348,1339,1331,1323,1315,1307,1299,1291,1283,1276,1268,1260,1252,
                   1245,1237,1230,1222,1215,1207,1200,1193,1185,1178,1171,1164,1157,1150,1143,1136,1129,1122,1115,1108,1102,1095,1088,1082,1075,
                   1069,1062,1056,1049,1043,1036,1030,
              },
              /* drc_high_LUT_point */
              {
                   4457,4457,4043,3666,3325,3016,2735,2481,2250,2040,1851,1678,1522,1380,1252,1135,1030, 934, 847, 768, 697, 632, 573, 520, 471,
                    427, 388, 351, 319, 289, 262, 238, 215, 195, 177, 161, 146, 132, 120, 109,  98,  89,  81,  73,  66,  60,  55,  49,  45,  41,
                     37,  33,  30,  27,  25,  22,  20,  18,  17,  15,  14,  12,  11,  10,   9,   8,   7,   7,   6,   5,   5,   4,   4,   3,   3,
                      3,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
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
              0.6,
              /* k2 */
              25,
              /* k3 */
              1,
           },
           /* para 2*/
           {
              /* drc_low_LUT_point */
              {
                   8910,8910,8824,8743,8666,8591,8518,8447,8377,8309,8243,8177,8113,8049,7987,7926,7865,7805,7746,7688,7631,7574,7518,7462,7408,
                   7354,7300,7247,7195,7143,7092,7041,6991,6941,6892,6844,6795,6748,6700,6654,6607,6561,6516,6471,6426,6382,6338,6295,6252,6209,
                   6167,6125,6083,6042,6001,5961,5921,5881,5841,5802,5764,5725,5687,5649,5612,5574,5537,5501,5464,5428,5393,5357,5322,5287,5253,
                   5218,5184,5150,5117,5084,5050,5018,4985,4953,4921,4889,4858,4826,4795,4764,4734,4704,4673,4643,4614,4584,4555,4526,4497,4469,
                   4440,4412,4384,4356,4329,4301,4274,4247,4221,4194,4168,4141,4115,4090,4064,4038,4013,3988,3963,3938,3914,3889,3865,3841,3817,
                   3794,3770,3747,3723,3700,3677,3655,3632,3610,3587,3565,3543,3522,3500,3478,3457,3436,3415,3394,3373,3352,3332,3312,3291,3271,
                   3251,3231,3212,3192,3173,3153,3134,3115,3096,3078,3059,3040,3022,3004,2986,2968,2950,2932,2914,2897,2879,2862,2845,2827,2810,
                   2794,2777,2760,2744,2727,2711,2695,2678,2662,2647,2631,2615,2599,2584,2569,2553,2538,2523,2508,2493,2478,2463,2449,2434,2420,
                   2406,2391,2377,2363,2349,2335,2321,2308,2294,2280,2267,2254,2240,2227,2214,2201,2188,2175,2162,2150,2137,2125,2112,2100,2087,
                   2075,2063,2051,2039,2027,2015,2003,1992,1980,1969,1957,1946,1934,1923,1912,1901,1890,1879,1868,1857,1846,1835,1825,1814,1804,
                   1793,1783,1772,1762,1752,1742,1732,
              },
              /* drc_high_LUT_point */
              {
                   7865,7865,6991,6251,5611,5050,4554,4115,3723,3372,3058,2776,2522,2293,2087,1900,1731,1578,1439,1314,1199,1096,1001, 915, 837,
                    766, 701, 642, 588, 538, 493, 452, 415, 380, 349, 320, 294, 270, 248, 227, 209, 192, 176, 162, 149, 137, 126, 116, 107,  98,
                     90,  83,  76,  70,  65,  60,  55,  50,  46,  43,  39,  36,  33,  31,  28,  26,  24,  22,  20,  19,  17,  16,  15,  14,  12,
                     11,  11,  10,   9,   8,   8,   7,   6,   6,   5,   5,   5,   4,   4,   3,   3,   3,   3,   2,   2,   2,   2,   2,   1,   1,
                      1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
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
              1.1,
              /* k2 */
              20,
              /* k3 */
              0.9,
           },
           /* para 3*/
           {
              /* drc_low_LUT_point */
              {
                   4860,4860,4813,4769,4726,4686,4646,4607,4569,4532,4496,4460,4425,4390,4356,4323,4290,4257,4225,4193,4162,4131,4100,4070,4040,
                   4011,3982,3953,3924,3896,3868,3840,3813,3786,3759,3733,3706,3680,3655,3629,3604,3579,3554,3529,3505,3481,3457,3433,3410,3387,
                   3363,3341,3318,3295,3273,3251,3229,3208,3186,3165,3144,3123,3102,3081,3061,3040,3020,3000,2980,2961,2941,2922,2903,2884,2865,
                   2846,2827,2809,2791,2773,2755,2737,2719,2701,2684,2667,2649,2632,2615,2599,2582,2565,2549,2533,2516,2500,2484,2469,2453,2437,
                   2422,2406,2391,2376,2361,2346,2331,2316,2302,2287,2273,2259,2245,2230,2216,2203,2189,2175,2161,2148,2135,2121,2108,2095,2082,
                   2069,2056,2043,2031,2018,2006,1993,1981,1969,1957,1945,1933,1921,1909,1897,1885,1874,1862,1851,1840,1828,1817,1806,1795,1784,
                   1773,1762,1752,1741,1730,1720,1709,1699,1689,1678,1668,1658,1648,1638,1628,1618,1609,1599,1589,1580,1570,1561,1551,1542,1533,
                   1524,1514,1505,1496,1487,1478,1470,1461,1452,1443,1435,1426,1418,1409,1401,1392,1384,1376,1368,1360,1351,1343,1335,1328,1320,
                   1312,1304,1296,1289,1281,1273,1266,1258,1251,1244,1236,1229,1222,1215,1207,1200,1193,1186,1179,1172,1165,1159,1152,1145,1138,
                   1132,1125,1118,1112,1105,1099,1093,1086,1080,1074,1067,1061,1055,1049,1043,1037,1031,1025,1019,1013,1007,1001, 995, 989, 984,
                    978, 972, 967, 961, 955, 950, 944,
              },
              /* drc_high_LUT_point */
              {
                   4290,4290,3813,3410,3060,2754,2484,2244,2030,1839,1668,1514,1376,1251,1138,1036, 944, 861, 785, 716, 654, 597, 546, 499, 456,
                    417, 382, 350, 320, 293, 269, 246, 226, 207, 190, 174, 160, 147, 135, 124, 114, 104,  96,  88,  81,  74,  68,  63,  58,  53,
                     49,  45,  41,  38,  35,  32,  30,  27,  25,  23,  21,  20,  18,  17,  15,  14,  13,  12,  11,  10,   9,   8,   8,   7,   7,
                      6,   6,   5,   5,   4,   4,   4,   3,   3,   3,   2,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
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
              0.6,
              /* k2 */
              20,
              /* k3 */
              0.9,
           },
           /* para 4*/
           {
              /* drc_low_LUT_point */
              {
                   4879,4879,4843,4808,4773,4738,4703,4669,4635,4601,4568,4534,4501,4468,4436,4403,4371,4339,4308,4276,4245,4214,4183,4153,4123,
                   4092,4063,4033,4004,3974,3945,3917,3888,3860,3831,3803,3776,3748,3721,3694,3667,3640,3613,3587,3561,3535,3509,3483,3458,3433,
                   3408,3383,3358,3334,3309,3285,3261,3237,3214,3190,3167,3144,3121,3098,3076,3053,3031,3009,2987,2965,2943,2922,2901,2880,2859,
                   2838,2817,2796,2776,2756,2736,2716,2696,2676,2657,2637,2618,2599,2580,2561,2542,2524,2505,2487,2469,2451,2433,2415,2398,2380,
                   2363,2346,2329,2312,2295,2278,2261,2245,2228,2212,2196,2180,2164,2148,2133,2117,2102,2086,2071,2056,2041,2026,2011,1997,1982,
                   1968,1953,1939,1925,1911,1897,1883,1869,1856,1842,1829,1815,1802,1789,1776,1763,1750,1737,1724,1712,1699,1687,1675,1662,1650,
                   1638,1626,1614,1603,1591,1579,1568,1556,1545,1534,1523,1511,1500,1489,1479,1468,1457,1446,1436,1425,1415,1405,1394,1384,1374,
                   1364,1354,1344,1335,1325,1315,1306,1296,1287,1277,1268,1259,1249,1240,1231,1222,1213,1204,1196,1187,1178,1170,1161,1153,1144,
                   1136,1128,1119,1111,1103,1095,1087,1079,1071,1063,1056,1048,1040,1033,1025,1018,1010,1003, 996, 988, 981, 974, 967, 960, 953,
                    946, 939, 932, 925, 918, 912, 905, 898, 892, 885, 879, 873, 866, 860, 854, 847, 841, 835, 829, 823, 817, 811, 805, 799, 793,
                    787, 782, 776, 770, 765, 759, 754,
              },
              /* drc_high_LUT_point */
              {
                   4371,4371,3888,3458,3075,2735,2433,2164,1924,1711,1522,1354,1204,1071, 952, 847, 753, 670, 596, 530, 471, 419, 373, 331, 295,
                    262, 233, 207, 184, 164, 146, 129, 115, 102,  91,  81,  72,  64,  57,  50,  45,  40,  35,  31,  28,  25,  22,  19,  17,  15,
                     14,  12,  11,   9,   8,   7,   6,   6,   5,   4,   4,   3,   3,   3,   2,   2,   2,   1,   1,   1,   1,   1,   1,   0,   0,
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
              0.6,
              /* k2 */
              30,
              /* k3 */
              1,
           },
           /* para 5*/
           {
              /* drc_low_LUT_point */
              {
                   9770,9770,9711,9652,9593,9535,9476,9419,9362,9305,9248,9192,9136,9080,9025,8970,8915,8861,8807,8754,8701,8648,8595,8543,8491,
                   8439,8388,8337,8286,8236,8185,8136,8086,8037,7988,7939,7891,7843,7795,7748,7701,7654,7607,7561,7515,7469,7424,7379,7334,7289,
                   7245,7201,7157,7114,7070,7027,6985,6942,6900,6858,6816,6775,6733,6692,6652,6611,6571,6531,6491,6452,6413,6374,6335,6296,6258,
                   6220,6182,6144,6107,6070,6033,5996,5960,5923,5887,5852,5816,5781,5745,5711,5676,5641,5607,5573,5539,5505,5472,5438,5405,5372,
                   5340,5307,5275,5243,5211,5179,5148,5116,5085,5054,5024,4993,4963,4933,4903,4873,4843,4814,4784,4755,4726,4697,4669,4641,4612,
                   4584,4556,4529,4501,4474,4446,4419,4392,4366,4339,4313,4287,4260,4235,4209,4183,4158,4132,4107,4082,4057,4033,4008,3984,3960,
                   3936,3912,3888,3864,3841,3817,3794,3771,3748,3725,3703,3680,3658,3635,3613,3591,3569,3548,3526,3505,3483,3462,3441,3420,3399,
                   3379,3358,3338,3317,3297,3277,3257,3237,3218,3198,3179,3159,3140,3121,3102,3083,3064,3046,3027,3009,2990,2972,2954,2936,2918,
                   2901,2883,2865,2848,2831,2813,2796,2779,2762,2746,2729,2712,2696,2679,2663,2647,2631,2615,2599,2583,2567,2552,2536,2521,2505,
                   2490,2475,2460,2445,2430,2415,2401,2386,2371,2357,2343,2328,2314,2300,2286,2272,2258,2245,2231,2217,2204,2191,2177,2164,2151,
                   2138,2125,2112,2099,2086,2073,2061,
              },
              /* drc_high_LUT_point */
              {
                   8915,8915,8086,7333,6651,6032,5471,4962,4500,4081,3702,3357,3045,2761,2505,2271,2060,1868,1694,1537,1394,1264,1146,1040, 943,
                    855, 776, 703, 638, 578, 525, 476, 431, 391, 355, 322, 292, 265, 240, 218, 197, 179, 162, 147, 133, 121, 110,  99,  90,  82,
                     74,  67,  61,  55,  50,  45,  41,  37,  34,  30,  28,  25,  23,  20,  18,  17,  15,  14,  12,  11,  10,   9,   8,   7,   7,
                      6,   5,   5,   4,   4,   3,   3,   3,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,
              },
              /* saturation */
              210,
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
              1.2,
              /* k2 */
              25,
              /* k3 */
              1,
           },
           /* para 6*/
           {
              /* drc_low_LUT_point */
              {
                   4879,4879,4843,4808,4773,4738,4703,4669,4635,4601,4568,4534,4501,4468,4436,4403,4371,4339,4308,4276,4245,4214,4183,4153,4123,
                   4092,4063,4033,4004,3974,3945,3917,3888,3860,3831,3803,3776,3748,3721,3694,3667,3640,3613,3587,3561,3535,3509,3483,3458,3433,
                   3408,3383,3358,3334,3309,3285,3261,3237,3214,3190,3167,3144,3121,3098,3076,3053,3031,3009,2987,2965,2943,2922,2901,2880,2859,
                   2838,2817,2796,2776,2756,2736,2716,2696,2676,2657,2637,2618,2599,2580,2561,2542,2524,2505,2487,2469,2451,2433,2415,2398,2380,
                   2363,2346,2329,2312,2295,2278,2261,2245,2228,2212,2196,2180,2164,2148,2133,2117,2102,2086,2071,2056,2041,2026,2011,1997,1982,
                   1968,1953,1939,1925,1911,1897,1883,1869,1856,1842,1829,1815,1802,1789,1776,1763,1750,1737,1724,1712,1699,1687,1675,1662,1650,
                   1638,1626,1614,1603,1591,1579,1568,1556,1545,1534,1523,1511,1500,1489,1479,1468,1457,1446,1436,1425,1415,1405,1394,1384,1374,
                   1364,1354,1344,1335,1325,1315,1306,1296,1287,1277,1268,1259,1249,1240,1231,1222,1213,1204,1196,1187,1178,1170,1161,1153,1144,
                   1136,1128,1119,1111,1103,1095,1087,1079,1071,1063,1056,1048,1040,1033,1025,1018,1010,1003, 996, 988, 981, 974, 967, 960, 953,
                    946, 939, 932, 925, 918, 912, 905, 898, 892, 885, 879, 873, 866, 860, 854, 847, 841, 835, 829, 823, 817, 811, 805, 799, 793,
                    787, 782, 776, 770, 765, 759, 754,
              },
              /* drc_high_LUT_point */
              {
                   4371,4371,3888,3458,3075,2735,2433,2164,1924,1711,1522,1354,1204,1071, 952, 847, 753, 670, 596, 530, 471, 419, 373, 331, 295,
                    262, 233, 207, 184, 164, 146, 129, 115, 102,  91,  81,  72,  64,  57,  50,  45,  40,  35,  31,  28,  25,  22,  19,  17,  15,
                     14,  12,  11,   9,   8,   7,   6,   6,   5,   4,   4,   3,   3,   3,   2,   2,   2,   1,   1,   1,   1,   1,   1,   0,   0,
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
              210,
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
              0.6,
              /* k2 */
              30,
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
             4,
          },
          /* aec trigger 1 */
          {
             5,
             8.1,
          },
          /* aec trigger 2 */
          {
             13,
             24.1,
          },
          /* aec trigger 3 */
          {
             32,
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
              0,//luma_detect_en
              100,//th1_very_light
              255,//th2_y_max
              1,//rgb_diff_detect_en
              60,//th1_rgb_diff
              90,//th2_rgb_diff
              1,//edge_detect_en
              1000,//th1_edge
              2000,//th2_edge
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
              0,//edge_offset0
              0,//edge_offset1
              0,//edge_offset2
              0,//edge_offset3
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
             370,
             511,
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
             5300,
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
                1.9712,-0.0703,-0.9009,
                -0.2252,1.2554,-0.0302,
                -0.0134,-1.0584,2.0718,
             },
             /* awb para 1 */
             {
                1.7712,-0.4703,-0.3009,
                -0.2252,1.2554,-0.0302,
                -0.2134,-0.7584,1.9718,
             },
             /* awb para 2 */
             {
                1.5973,-0.7797,0.1824,
                -0.2716,1.4052,-0.1336,
                0.0621,-1.1542,2.0921,
             },
             /* awb para 3 */
             {
                1.9363,-1.1002,0.1639,
                -0.4723,1.5455,-0.0732,
                0.0056,-0.9641,1.9587,
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                1.9755,-0.9512,-0.0243,
                -0.6205,1.6132,0.0073,
                -0.6537,-0.6836,2.3373,
             },
             /* awb para 1 */
             {
                1.6712,-0.9703,0.2991,
                -0.2752,1.4554,-0.1802,
                -0.2134,-0.7584,1.9718,
             },
             /* awb para 2 */
             {
                1.5973,-0.7797,0.1824,
                -0.2716,1.4052,-0.1336,
                0.0621,-1.1542,2.0921,
             },
             /* awb para 3 */
             {
                1.9363,-1.1002,0.1639,
                -0.4723,1.5455,-0.0732,
                0.0056,-0.9641,1.9587,
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
             330,
          },
          /* aec trigger 2 */
          {
             340,
             390,
          },
          /* aec trigger 3 */
          {
             410,
             450,
          },
          /* aec trigger 4 */
          {
             460,
             550,
          },
       },
       4096,//gamma_lut_size
       /* isp_sub_module_gamma_lut_pra pra[MIDDLE_TRIGGER_COUNT] */
       {
          /* aec trigger 0 */
          {
                   0,   8,  17,  26,  34,  42,  50,  58,  66,  74,  81,  89,  96, 103, 110, 117, 123, 130, 136, 142, 148, 154, 160, 166, 171, 177, 182, 187, 192, 197, 201, 205,
                 210, 215, 220, 224, 229, 234, 238, 243, 248, 252, 257, 261, 266, 271, 275, 280, 284, 288, 293, 297, 302, 306, 311, 315, 319, 324, 328, 332, 335, 340, 344, 348,
                 353, 357, 361, 366, 370, 374, 379, 383, 388, 392, 397, 401, 406, 410, 415, 419, 424, 428, 433, 437, 442, 447, 451, 456, 461, 465, 470, 475, 480, 484, 489, 494,
                 499, 503, 508, 513, 518, 523, 528, 533, 538, 543, 547, 552, 557, 562, 567, 572, 578, 583, 588, 593, 598, 603, 608, 613, 619, 622, 626, 630, 634, 638, 642, 645,
                 649, 653, 657, 661, 664, 668, 672, 676, 680, 683, 687, 691, 695, 698, 702, 706, 710, 713, 717, 721, 724, 728, 732, 735, 739, 743, 746, 750, 754, 757, 761, 765,
                 768, 772, 775, 779, 783, 786, 790, 793, 797, 801, 804, 808, 811, 815, 818, 822, 825, 829, 832, 836, 839, 843, 846, 850, 853, 857, 860, 864, 867, 871, 874, 878,
                 881, 884, 888, 891, 895, 898, 901, 905, 908, 912, 915, 918, 922, 925, 928, 932, 935, 938, 942, 945, 948, 952, 955, 958, 961, 965, 968, 971, 973, 976, 979, 983,
                 986, 990, 993, 996,1000,1003,1006,1010,1013,1016,1020,1023,1026,1030,1033,1036,1040,1043,1046,1049,1053,1056,1059,1062,1065,1069,1072,1075,1078,1081,1085,1088,
                1091,1094,1097,1100,1103,1106,1110,1113,1116,1119,1122,1125,1128,1131,1134,1137,1140,1143,1146,1149,1152,1155,1158,1161,1164,1167,1170,1172,1175,1178,1181,1184,
                1187,1190,1193,1195,1198,1201,1204,1207,1210,1212,1215,1218,1219,1223,1226,1229,1232,1235,1237,1240,1243,1246,1249,1252,1255,1258,1261,1264,1267,1270,1273,1276,
                1279,1282,1284,1287,1290,1293,1296,1299,1302,1304,1307,1310,1313,1316,1319,1321,1324,1327,1330,1332,1335,1338,1341,1344,1346,1349,1352,1354,1357,1360,1363,1365,
                1368,1371,1373,1376,1379,1381,1384,1387,1389,1392,1395,1397,1400,1403,1405,1408,1410,1413,1415,1418,1421,1423,1426,1428,1431,1433,1436,1438,1441,1444,1446,1449,
                1451,1454,1456,1458,1461,1463,1466,1468,1471,1473,1476,1478,1481,1483,1485,1488,1489,1492,1495,1498,1500,1503,1506,1509,1511,1514,1517,1519,1522,1525,1527,1530,
                1533,1535,1538,1540,1543,1546,1548,1551,1553,1556,1559,1561,1564,1566,1569,1571,1574,1576,1579,1582,1584,1587,1589,1592,1594,1597,1599,1601,1604,1606,1609,1611,
                1614,1616,1619,1621,1623,1626,1628,1631,1633,1635,1638,1640,1643,1645,1647,1650,1652,1654,1657,1659,1661,1663,1666,1668,1670,1673,1675,1677,1679,1682,1684,1686,
                1688,1691,1693,1695,1697,1699,1702,1704,1706,1708,1710,1713,1715,1717,1719,1721,1723,1725,1728,1730,1732,1734,1736,1738,1740,1742,1744,1746,1748,1750,1752,1754,
                1757,1759,1761,1763,1765,1767,1769,1771,1772,1774,1776,1778,1780,1782,1784,1786,1788,1789,1791,1793,1795,1797,1799,1801,1802,1804,1806,1808,1810,1812,1814,1815,
                1817,1819,1821,1823,1825,1826,1828,1830,1832,1834,1835,1837,1839,1841,1843,1845,1846,1848,1850,1852,1853,1855,1857,1859,1861,1862,1864,1866,1868,1869,1871,1873,
                1875,1877,1878,1880,1882,1884,1885,1887,1889,1891,1892,1894,1896,1897,1899,1901,1903,1904,1906,1908,1910,1911,1913,1915,1916,1918,1920,1921,1923,1925,1927,1928,
                1930,1932,1933,1935,1937,1938,1940,1942,1943,1945,1947,1948,1950,1952,1953,1955,1957,1958,1960,1961,1963,1965,1966,1968,1970,1971,1973,1975,1976,1978,1979,1981,
                1983,1984,1986,1987,1989,1991,1992,1994,1995,1997,1999,2000,2002,2003,2005,2006,2008,2010,2011,2013,2014,2016,2017,2019,2020,2022,2024,2025,2027,2028,2030,2031,
                2033,2034,2036,2037,2039,2040,2042,2043,2045,2047,2048,2050,2051,2053,2054,2056,2057,2059,2060,2062,2063,2064,2066,2067,2069,2070,2072,2073,2075,2076,2078,2079,
                2081,2082,2084,2085,2086,2088,2089,2091,2092,2094,2095,2097,2098,2099,2101,2102,2104,2105,2106,2108,2109,2111,2112,2114,2115,2116,2118,2119,2121,2122,2123,2125,
                2126,2127,2129,2130,2132,2133,2134,2136,2137,2138,2140,2141,2142,2144,2145,2146,2148,2149,2151,2152,2153,2155,2156,2157,2159,2160,2162,2163,2165,2167,2168,2170,
                2171,2173,2174,2176,2178,2179,2181,2182,2184,2186,2187,2189,2190,2192,2193,2195,2197,2198,2200,2201,2203,2204,2206,2207,2209,2211,2212,2214,2215,2217,2218,2220,
                2221,2223,2224,2226,2227,2229,2230,2232,2233,2235,2236,2238,2239,2241,2242,2244,2245,2247,2248,2250,2251,2253,2254,2256,2257,2259,2260,2262,2263,2265,2266,2268,
                2269,2271,2272,2274,2275,2277,2278,2279,2281,2282,2284,2285,2287,2288,2290,2291,2292,2294,2295,2297,2298,2300,2301,2302,2304,2305,2307,2308,2309,2311,2312,2314,
                2315,2316,2318,2319,2321,2322,2323,2325,2326,2328,2329,2330,2332,2333,2335,2336,2337,2339,2340,2341,2343,2344,2345,2347,2348,2349,2351,2352,2354,2355,2356,2358,
                2359,2360,2362,2363,2364,2366,2367,2368,2370,2371,2372,2374,2375,2376,2377,2379,2380,2381,2383,2384,2385,2387,2388,2389,2390,2392,2393,2394,2396,2397,2398,2399,
                2401,2402,2403,2405,2406,2407,2408,2410,2411,2412,2413,2415,2416,2417,2418,2420,2421,2422,2423,2425,2426,2427,2428,2429,2431,2432,2433,2434,2436,2437,2438,2439,
                2440,2442,2443,2444,2445,2446,2448,2449,2450,2451,2452,2454,2455,2456,2457,2458,2460,2461,2462,2463,2464,2465,2467,2468,2469,2470,2471,2472,2474,2475,2476,2477,
                2478,2479,2480,2482,2483,2484,2485,2486,2487,2488,2490,2491,2492,2493,2494,2495,2496,2497,2498,2500,2501,2502,2503,2504,2505,2506,2507,2508,2509,2511,2512,2513,
                2514,2515,2516,2517,2518,2519,2520,2521,2522,2524,2525,2526,2527,2528,2529,2530,2531,2532,2533,2534,2535,2536,2537,2538,2539,2540,2541,2542,2543,2544,2545,2546,
                2547,2548,2549,2551,2552,2553,2554,2555,2556,2557,2558,2559,2560,2561,2562,2563,2564,2564,2565,2566,2567,2568,2569,2570,2571,2572,2573,2574,2575,2576,2577,2578,
                2579,2580,2581,2582,2583,2584,2585,2586,2587,2588,2588,2589,2590,2591,2592,2593,2594,2595,2596,2597,2598,2599,2600,2600,2601,2602,2603,2604,2605,2606,2607,2608,
                2609,2609,2610,2611,2612,2613,2614,2615,2616,2617,2617,2618,2619,2620,2621,2622,2623,2623,2624,2625,2626,2627,2628,2629,2629,2630,2631,2632,2633,2634,2634,2635,
                2636,2637,2638,2639,2639,2640,2641,2642,2643,2643,2644,2645,2646,2647,2647,2648,2649,2650,2651,2651,2652,2653,2654,2655,2655,2656,2657,2658,2658,2659,2660,2661,
                2662,2662,2663,2664,2665,2665,2666,2667,2668,2668,2669,2670,2671,2671,2672,2673,2674,2674,2675,2676,2677,2678,2679,2680,2681,2682,2683,2684,2685,2685,2686,2687,
                2688,2689,2690,2691,2692,2693,2694,2695,2695,2696,2697,2698,2699,2700,2701,2702,2703,2704,2705,2706,2706,2707,2708,2709,2710,2711,2712,2713,2714,2715,2715,2716,
                2717,2718,2719,2720,2721,2722,2723,2724,2725,2725,2726,2727,2728,2729,2730,2731,2732,2733,2734,2734,2735,2736,2737,2738,2739,2740,2741,2742,2742,2743,2744,2745,
                2746,2747,2748,2749,2750,2751,2751,2752,2753,2754,2755,2756,2757,2758,2759,2759,2760,2761,2762,2763,2764,2765,2766,2767,2767,2768,2769,2770,2771,2772,2773,2774,
                2774,2775,2776,2777,2778,2779,2780,2781,2782,2782,2783,2784,2785,2786,2787,2788,2789,2789,2790,2791,2792,2793,2794,2795,2796,2796,2797,2798,2799,2800,2801,2802,
                2803,2803,2804,2805,2806,2807,2808,2809,2810,2810,2811,2812,2813,2814,2815,2816,2816,2817,2818,2819,2820,2821,2822,2823,2823,2824,2825,2826,2827,2828,2829,2829,
                2830,2831,2832,2833,2834,2835,2835,2836,2837,2838,2839,2840,2841,2841,2842,2843,2844,2845,2846,2847,2847,2848,2849,2850,2851,2852,2853,2853,2854,2855,2856,2857,
                2858,2859,2859,2860,2861,2862,2863,2864,2864,2865,2866,2867,2868,2869,2870,2870,2871,2872,2873,2874,2875,2875,2876,2877,2878,2879,2880,2881,2881,2882,2883,2884,
                2885,2886,2886,2887,2888,2889,2890,2891,2891,2892,2893,2894,2895,2896,2896,2897,2898,2899,2900,2901,2901,2902,2903,2904,2905,2906,2906,2907,2908,2909,2910,2911,
                2911,2912,2913,2914,2915,2916,2916,2917,2918,2919,2920,2921,2921,2922,2923,2924,2925,2925,2926,2927,2928,2929,2930,2930,2931,2932,2933,2934,2935,2935,2936,2937,
                2938,2939,2939,2940,2941,2942,2943,2944,2944,2945,2946,2947,2948,2949,2949,2950,2951,2952,2953,2953,2954,2955,2956,2957,2958,2958,2959,2960,2961,2962,2962,2963,
                2964,2965,2966,2967,2967,2968,2969,2970,2971,2971,2972,2973,2974,2975,2975,2976,2977,2978,2979,2979,2980,2981,2982,2983,2983,2984,2985,2986,2987,2987,2988,2989,
                2990,2991,2991,2992,2993,2994,2995,2995,2996,2997,2998,2998,2999,3000,3001,3002,3002,3003,3004,3005,3006,3006,3007,3008,3009,3009,3010,3011,3012,3013,3013,3014,
                3015,3016,3016,3017,3018,3019,3019,3020,3021,3022,3023,3023,3024,3025,3026,3026,3027,3028,3029,3029,3030,3031,3032,3032,3033,3034,3035,3036,3036,3037,3038,3039,
                3039,3040,3041,3042,3042,3043,3044,3045,3045,3046,3047,3048,3048,3049,3050,3051,3051,3052,3053,3054,3054,3055,3056,3057,3057,3058,3059,3059,3060,3061,3062,3062,
                3063,3064,3065,3065,3066,3067,3068,3068,3069,3070,3071,3071,3072,3073,3073,3074,3075,3076,3076,3077,3078,3079,3079,3080,3081,3081,3082,3083,3084,3084,3085,3086,
                3086,3087,3088,3089,3089,3090,3091,3091,3092,3093,3094,3094,3095,3096,3096,3097,3098,3099,3099,3100,3101,3101,3102,3103,3104,3104,3105,3106,3106,3107,3108,3108,
                3109,3110,3111,3111,3112,3113,3113,3114,3115,3115,3116,3117,3117,3118,3119,3120,3120,3121,3122,3122,3123,3124,3124,3125,3126,3126,3127,3128,3128,3129,3130,3131,
                3131,3132,3133,3133,3134,3135,3135,3136,3137,3137,3138,3139,3139,3140,3141,3141,3142,3143,3143,3144,3145,3145,3146,3147,3147,3148,3149,3149,3150,3151,3151,3152,
                3153,3153,3154,3155,3155,3156,3157,3157,3158,3158,3159,3159,3160,3161,3161,3162,3163,3163,3164,3165,3165,3166,3167,3167,3168,3169,3169,3170,3170,3171,3172,3172,
                3173,3174,3174,3175,3176,3176,3177,3177,3178,3179,3179,3180,3181,3181,3182,3183,3183,3184,3184,3185,3186,3186,3187,3188,3188,3189,3190,3190,3191,3191,3192,3193,
                3193,3194,3195,3195,3196,3196,3197,3198,3198,3199,3200,3200,3201,3201,3202,3203,3203,3204,3204,3205,3206,3206,3207,3208,3208,3209,3209,3210,3211,3211,3212,3212,
                3213,3214,3214,3215,3216,3216,3217,3217,3218,3219,3219,3220,3220,3221,3222,3222,3223,3223,3224,3225,3225,3226,3226,3227,3228,3228,3229,3229,3230,3231,3231,3232,
                3232,3233,3234,3234,3235,3235,3236,3237,3237,3238,3238,3239,3240,3240,3241,3241,3242,3243,3243,3244,3244,3245,3245,3246,3247,3247,3248,3248,3249,3250,3250,3251,
                3251,3252,3252,3253,3254,3254,3255,3255,3256,3257,3257,3258,3258,3259,3259,3260,3261,3261,3262,3262,3263,3263,3264,3265,3265,3266,3266,3267,3267,3268,3269,3269,
                3270,3270,3271,3271,3272,3273,3273,3274,3274,3275,3275,3276,3277,3277,3278,3278,3279,3279,3280,3280,3281,3282,3282,3283,3283,3284,3284,3285,3285,3286,3287,3287,
                3288,3288,3289,3289,3290,3290,3291,3292,3292,3293,3293,3294,3294,3295,3295,3296,3296,3297,3298,3298,3299,3299,3300,3300,3301,3301,3302,3302,3303,3304,3304,3305,
                3305,3306,3306,3307,3307,3308,3308,3309,3309,3310,3311,3311,3312,3312,3313,3313,3314,3314,3315,3315,3316,3316,3317,3317,3318,3319,3319,3320,3320,3321,3321,3322,
                3322,3323,3323,3324,3324,3325,3325,3326,3326,3327,3327,3328,3328,3329,3329,3330,3331,3331,3332,3332,3331,3332,3332,3333,3333,3334,3334,3335,3335,3336,3336,3337,
                3337,3338,3338,3339,3339,3340,3340,3340,3341,3341,3342,3342,3343,3343,3344,3344,3345,3345,3346,3346,3347,3347,3347,3348,3348,3349,3349,3350,3350,3351,3351,3352,
                3352,3353,3353,3353,3354,3354,3355,3355,3356,3356,3357,3357,3358,3358,3359,3359,3359,3360,3360,3361,3361,3362,3362,3363,3363,3364,3364,3364,3365,3365,3366,3366,
                3367,3367,3368,3368,3369,3369,3369,3370,3370,3371,3371,3372,3372,3373,3373,3373,3374,3374,3375,3375,3376,3376,3377,3377,3378,3378,3378,3379,3379,3380,3380,3381,
                3381,3382,3382,3382,3383,3383,3384,3384,3385,3385,3386,3386,3386,3387,3387,3388,3388,3389,3389,3389,3390,3390,3391,3391,3392,3392,3393,3393,3393,3394,3394,3395,
                3395,3396,3396,3396,3397,3397,3398,3398,3399,3399,3400,3400,3400,3401,3401,3402,3402,3403,3403,3403,3404,3404,3405,3405,3406,3406,3406,3407,3407,3408,3408,3409,
                3409,3409,3410,3410,3411,3411,3412,3412,3412,3413,3413,3414,3414,3414,3415,3415,3416,3416,3417,3417,3417,3418,3418,3419,3419,3420,3420,3420,3421,3421,3422,3422,
                3422,3423,3423,3424,3424,3425,3425,3425,3426,3426,3427,3427,3427,3428,3428,3429,3429,3430,3430,3430,3431,3431,3432,3432,3432,3433,3433,3434,3434,3434,3435,3435,
                3436,3436,3437,3437,3437,3438,3438,3439,3439,3439,3440,3440,3441,3441,3441,3442,3442,3443,3443,3443,3444,3444,3445,3445,3445,3446,3446,3447,3447,3447,3448,3448,
                3449,3449,3449,3450,3450,3451,3451,3451,3452,3452,3453,3453,3453,3454,3454,3455,3455,3455,3456,3456,3457,3457,3457,3458,3458,3459,3459,3459,3460,3460,3461,3461,
                3461,3462,3462,3462,3463,3463,3464,3464,3464,3465,3465,3466,3466,3466,3467,3467,3468,3468,3468,3469,3469,3469,3470,3470,3471,3471,3471,3472,3472,3473,3473,3473,
                3474,3474,3474,3475,3475,3476,3476,3476,3477,3477,3477,3478,3478,3479,3479,3479,3480,3480,3481,3481,3481,3482,3482,3482,3483,3483,3484,3484,3484,3485,3485,3485,
                3486,3486,3487,3487,3487,3488,3488,3488,3489,3489,3490,3490,3490,3491,3491,3491,3492,3492,3492,3493,3493,3494,3494,3494,3495,3495,3495,3496,3496,3497,3497,3497,
                3498,3498,3498,3499,3499,3499,3500,3500,3501,3501,3501,3502,3502,3502,3503,3503,3503,3504,3504,3504,3505,3505,3506,3506,3506,3507,3507,3507,3508,3508,3508,3509,
                3509,3509,3510,3510,3511,3511,3511,3512,3512,3512,3513,3513,3513,3514,3514,3514,3515,3515,3515,3516,3516,3517,3517,3517,3518,3518,3518,3519,3519,3519,3520,3520,
                3520,3521,3521,3521,3522,3522,3522,3523,3523,3523,3524,3524,3525,3525,3525,3526,3526,3526,3527,3527,3527,3528,3528,3528,3529,3529,3529,3530,3530,3530,3531,3531,
                3531,3532,3532,3532,3533,3533,3533,3534,3534,3534,3535,3535,3535,3536,3536,3536,3537,3537,3537,3538,3538,3538,3539,3539,3539,3540,3540,3540,3541,3541,3541,3542,
                3542,3542,3543,3543,3543,3544,3544,3544,3545,3545,3545,3546,3546,3546,3547,3547,3547,3548,3548,3548,3549,3549,3549,3550,3550,3550,3551,3551,3551,3551,3552,3552,
                3552,3553,3553,3553,3554,3554,3554,3555,3555,3555,3556,3556,3556,3557,3557,3557,3558,3558,3558,3558,3559,3559,3559,3560,3560,3560,3561,3561,3561,3562,3562,3562,
                3563,3563,3563,3564,3564,3564,3564,3565,3565,3565,3566,3566,3566,3567,3567,3567,3568,3568,3568,3568,3569,3569,3569,3570,3570,3570,3571,3571,3571,3571,3572,3572,
                3572,3573,3573,3573,3574,3574,3574,3575,3575,3575,3575,3576,3576,3576,3577,3577,3577,3578,3578,3578,3578,3579,3579,3579,3580,3580,3580,3580,3581,3581,3581,3582,
                3582,3582,3583,3583,3583,3583,3584,3584,3584,3585,3585,3585,3585,3586,3586,3586,3587,3587,3587,3588,3588,3588,3588,3589,3589,3589,3590,3590,3590,3590,3591,3591,
                3591,3592,3592,3592,3592,3593,3593,3593,3594,3594,3594,3594,3595,3595,3595,3595,3596,3596,3596,3597,3597,3597,3597,3598,3598,3598,3599,3599,3599,3599,3600,3600,
                3600,3601,3601,3601,3601,3602,3602,3602,3602,3603,3603,3603,3604,3604,3604,3604,3605,3605,3605,3605,3606,3606,3606,3607,3607,3607,3607,3608,3608,3608,3608,3609,
                3609,3609,3609,3610,3610,3610,3611,3611,3611,3611,3612,3612,3612,3612,3613,3613,3613,3613,3614,3614,3614,3615,3615,3615,3615,3616,3616,3616,3616,3617,3617,3617,
                3617,3618,3618,3618,3618,3619,3619,3619,3619,3620,3620,3620,3620,3621,3621,3621,3621,3622,3622,3622,3623,3623,3623,3623,3623,3624,3624,3624,3624,3625,3625,3625,
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
                   0,   1,   2,   3,   4,   5,   6,   7,   9,  10,  11,  12,  13,  14,  15,  16,  18,  19,  20,  21,  22,  23,  24,  25,  27,  28,  29,  30,  32,  33,  31,  32,
                  32,  33,  33,  34,  34,  35,  36,  37,  38,  38,  39,  41,  42,  43,  44,  46,  47,  48,  50,  52,  53,  55,  57,  59,  61,  63,  65,  67,  67,  69,  70,  72,
                  74,  75,  77,  79,  81,  83,  85,  87,  89,  92,  94,  96,  99, 101, 104, 106, 109, 112, 115, 118, 121, 124, 127, 130, 133, 136, 139, 143, 146, 149, 152, 155,
                 158, 162, 165, 168, 172, 175, 179, 182, 186, 189, 193, 196, 200, 204, 208, 211, 215, 219, 223, 227, 231, 235, 239, 243, 245, 250, 254, 258, 262, 266, 270, 275,
                 279, 283, 287, 291, 295, 299, 304, 308, 312, 316, 320, 324, 329, 333, 337, 341, 345, 349, 354, 358, 362, 366, 370, 374, 379, 383, 387, 391, 395, 400, 404, 408,
                 411, 414, 418, 421, 425, 429, 432, 436, 440, 443, 447, 451, 454, 458, 462, 465, 469, 473, 476, 480, 484, 487, 491, 495, 498, 502, 506, 509, 513, 517, 521, 524,
                 528, 532, 535, 539, 543, 547, 550, 554, 558, 561, 565, 569, 573, 576, 580, 584, 588, 592, 595, 599, 603, 607, 610, 614, 618, 622, 625, 629, 633, 637, 641, 644,
                 648, 652, 656, 660, 663, 667, 671, 675, 679, 683, 686, 690, 694, 698, 702, 706, 709, 713, 717, 721, 725, 729, 733, 737, 740, 744, 748, 752, 756, 760, 764, 768,
                 771, 775, 779, 783, 787, 791, 795, 799, 803, 807, 811, 814, 818, 822, 826, 830, 834, 838, 842, 846, 850, 854, 858, 862, 866, 870, 874, 878, 882, 886, 890, 894,
                 897, 901, 905, 909, 913, 917, 921, 925, 929, 933, 937, 941, 946, 949, 952, 956, 959, 963, 966, 969, 973, 976, 980, 983, 986, 990, 993, 996,1000,1003,1007,1010,
                1013,1017,1020,1024,1027,1030,1034,1037,1040,1044,1047,1050,1054,1057,1060,1064,1067,1070,1074,1077,1081,1084,1087,1090,1094,1097,1100,1104,1107,1110,1114,1117,
                1120,1124,1127,1130,1134,1137,1140,1144,1147,1150,1153,1157,1160,1163,1167,1170,1173,1176,1180,1183,1186,1190,1193,1196,1199,1203,1206,1209,1212,1216,1219,1222,
                1225,1229,1232,1235,1238,1242,1245,1248,1251,1255,1258,1261,1264,1267,1271,1274,1277,1280,1284,1287,1290,1293,1296,1300,1303,1306,1309,1312,1316,1319,1322,1325,
                1328,1332,1335,1338,1341,1344,1348,1351,1354,1357,1360,1363,1367,1370,1373,1376,1379,1382,1386,1389,1392,1395,1398,1401,1404,1408,1411,1414,1417,1420,1423,1426,
                1429,1433,1436,1439,1442,1445,1448,1451,1454,1458,1461,1464,1467,1470,1473,1476,1479,1482,1485,1489,1492,1495,1498,1501,1504,1507,1510,1513,1516,1519,1522,1525,
                1529,1532,1535,1538,1541,1544,1547,1550,1553,1556,1559,1562,1565,1568,1571,1574,1577,1580,1583,1586,1589,1592,1595,1598,1601,1604,1608,1611,1614,1617,1620,1623,
                1626,1629,1632,1635,1638,1641,1644,1647,1650,1653,1656,1658,1661,1664,1666,1669,1672,1675,1677,1680,1682,1685,1688,1690,1693,1696,1698,1701,1703,1706,1708,1711,
                1713,1716,1718,1721,1724,1726,1729,1731,1733,1736,1738,1741,1743,1746,1748,1751,1753,1755,1758,1760,1762,1765,1767,1770,1772,1774,1777,1779,1781,1783,1786,1788,
                1790,1793,1795,1797,1799,1802,1804,1806,1808,1810,1813,1815,1817,1819,1821,1823,1826,1828,1830,1832,1834,1836,1838,1840,1842,1844,1846,1849,1851,1853,1855,1857,
                1859,1861,1863,1865,1867,1869,1871,1872,1874,1876,1878,1880,1882,1884,1886,1888,1890,1892,1893,1895,1897,1899,1901,1903,1904,1906,1908,1910,1911,1913,1915,1917,
                1918,1920,1922,1924,1925,1927,1929,1931,1932,1934,1936,1937,1939,1941,1942,1944,1946,1947,1949,1951,1952,1954,1956,1957,1959,1961,1962,1964,1966,1967,1969,1970,
                1972,1974,1975,1977,1979,1980,1982,1983,1985,1987,1988,1990,1991,1993,1995,1996,1998,1999,2001,2002,2004,2006,2007,2009,2010,2012,2013,2015,2016,2018,2019,2021,
                2022,2024,2025,2027,2028,2030,2031,2033,2034,2036,2037,2039,2040,2042,2043,2045,2046,2048,2049,2051,2052,2054,2055,2057,2058,2059,2061,2062,2064,2065,2067,2068,
                2069,2071,2072,2074,2075,2076,2078,2079,2081,2082,2083,2085,2086,2088,2089,2090,2092,2093,2094,2096,2097,2098,2100,2101,2103,2104,2105,2106,2107,2108,2109,2111,
                2112,2113,2114,2115,2116,2118,2119,2120,2121,2122,2123,2124,2126,2127,2128,2129,2130,2131,2132,2134,2135,2136,2137,2138,2139,2141,2142,2143,2144,2145,2146,2147,
                2149,2150,2151,2152,2153,2154,2156,2157,2158,2159,2160,2161,2162,2164,2165,2166,2167,2168,2169,2170,2172,2173,2174,2175,2176,2177,2178,2180,2181,2182,2183,2184,
                2185,2186,2188,2189,2190,2191,2192,2193,2194,2196,2197,2198,2199,2200,2201,2202,2204,2205,2206,2207,2208,2209,2210,2212,2213,2214,2215,2216,2217,2218,2220,2221,
                2222,2223,2224,2225,2226,2228,2229,2230,2231,2232,2233,2234,2236,2237,2238,2239,2240,2241,2242,2244,2245,2246,2247,2248,2249,2250,2251,2253,2254,2255,2256,2257,
                2258,2259,2261,2262,2261,2263,2264,2265,2266,2267,2269,2270,2271,2272,2273,2275,2276,2277,2278,2279,2280,2282,2283,2284,2285,2286,2287,2289,2290,2291,2292,2293,
                2294,2296,2297,2298,2299,2300,2301,2303,2304,2305,2306,2307,2308,2310,2311,2312,2313,2314,2315,2316,2318,2319,2320,2321,2322,2323,2324,2326,2327,2328,2329,2330,
                2331,2332,2334,2335,2336,2337,2338,2339,2340,2342,2343,2344,2345,2346,2347,2348,2349,2351,2352,2353,2354,2355,2356,2357,2358,2360,2361,2362,2363,2364,2365,2366,
                2367,2369,2370,2371,2372,2373,2374,2375,2376,2377,2378,2380,2381,2382,2383,2384,2385,2386,2387,2388,2390,2391,2392,2393,2394,2395,2396,2397,2398,2399,2400,2402,
                2403,2404,2405,2406,2407,2408,2409,2410,2411,2412,2413,2415,2416,2417,2418,2419,2420,2421,2422,2423,2424,2425,2426,2427,2428,2430,2431,2432,2433,2434,2435,2436,
                2437,2438,2439,2440,2441,2442,2443,2444,2445,2447,2448,2449,2450,2451,2452,2453,2454,2455,2456,2457,2458,2459,2460,2461,2462,2463,2464,2465,2466,2467,2468,2470,
                2471,2472,2473,2474,2475,2476,2477,2478,2479,2480,2481,2482,2483,2484,2485,2486,2487,2488,2489,2490,2491,2492,2493,2494,2495,2496,2497,2498,2499,2500,2501,2502,
                2503,2504,2505,2506,2507,2508,2509,2510,2511,2512,2513,2514,2515,2516,2517,2518,2519,2520,2521,2522,2523,2524,2525,2526,2527,2528,2529,2530,2531,2532,2533,2534,
                2535,2536,2537,2538,2539,2540,2541,2542,2543,2544,2545,2546,2547,2548,2549,2550,2551,2552,2553,2554,2555,2556,2557,2558,2559,2560,2561,2561,2562,2563,2564,2565,
                2566,2567,2568,2569,2570,2571,2572,2573,2574,2575,2576,2577,2578,2579,2580,2581,2582,2582,2583,2584,2585,2586,2587,2588,2589,2590,2591,2592,2593,2594,2595,2596,
                2597,2597,2598,2599,2600,2601,2602,2603,2604,2605,2606,2607,2608,2609,2609,2610,2611,2612,2613,2614,2615,2616,2617,2618,2619,2620,2620,2621,2622,2623,2624,2625,
                2626,2627,2628,2629,2630,2630,2631,2632,2633,2634,2635,2636,2637,2638,2639,2639,2640,2641,2642,2643,2644,2645,2646,2647,2647,2648,2649,2650,2651,2652,2653,2654,
                2655,2655,2656,2657,2658,2659,2660,2661,2662,2662,2663,2664,2665,2666,2667,2668,2668,2669,2670,2671,2672,2673,2674,2675,2675,2676,2677,2678,2679,2680,2681,2681,
                2682,2683,2684,2685,2686,2687,2687,2688,2689,2690,2691,2692,2692,2693,2694,2695,2696,2697,2697,2698,2699,2700,2701,2702,2703,2703,2704,2705,2706,2707,2708,2708,
                2709,2710,2711,2712,2712,2713,2714,2715,2716,2717,2717,2718,2719,2720,2721,2722,2722,2723,2724,2725,2726,2726,2727,2728,2729,2730,2730,2731,2732,2733,2734,2734,
                2735,2736,2737,2738,2738,2739,2740,2741,2742,2742,2743,2744,2745,2746,2746,2747,2748,2749,2750,2750,2751,2752,2753,2753,2754,2755,2756,2757,2757,2758,2759,2760,
                2761,2761,2762,2763,2764,2764,2765,2766,2767,2767,2768,2769,2770,2771,2771,2772,2773,2774,2774,2775,2776,2777,2777,2778,2779,2780,2780,2781,2782,2783,2783,2784,
                2785,2786,2786,2787,2788,2789,2789,2790,2791,2792,2792,2793,2794,2795,2795,2796,2797,2798,2798,2799,2800,2801,2801,2802,2803,2803,2804,2805,2806,2806,2807,2808,
                2809,2809,2810,2811,2811,2812,2813,2814,2814,2815,2816,2816,2817,2818,2819,2819,2820,2821,2821,2822,2823,2824,2824,2825,2826,2826,2827,2828,2829,2830,2830,2831,
                2832,2832,2833,2834,2835,2835,2836,2837,2837,2838,2839,2840,2840,2841,2842,2842,2843,2844,2844,2845,2846,2847,2847,2848,2849,2849,2850,2851,2851,2852,2853,2854,
                2854,2855,2856,2856,2857,2858,2858,2859,2860,2861,2861,2862,2863,2863,2864,2865,2865,2866,2867,2868,2868,2869,2870,2870,2871,2872,2872,2873,2874,2874,2875,2876,
                2877,2877,2878,2879,2879,2880,2881,2881,2882,2883,2883,2884,2885,2885,2886,2887,2888,2888,2889,2890,2890,2891,2892,2892,2893,2894,2894,2895,2896,2896,2897,2898,
                2899,2899,2900,2901,2901,2902,2903,2903,2904,2905,2905,2906,2907,2907,2908,2909,2909,2910,2911,2911,2912,2913,2914,2914,2915,2916,2916,2917,2918,2918,2919,2920,
                2920,2921,2922,2922,2923,2924,2924,2925,2926,2926,2927,2928,2928,2929,2930,2930,2931,2932,2932,2933,2934,2934,2935,2936,2936,2937,2938,2938,2939,2940,2940,2941,
                2942,2942,2943,2944,2944,2945,2946,2946,2947,2948,2948,2949,2950,2950,2951,2952,2952,2953,2954,2954,2955,2956,2956,2957,2958,2958,2959,2960,2960,2961,2962,2962,
                2963,2964,2964,2965,2966,2966,2967,2968,2968,2969,2970,2970,2971,2972,2972,2973,2974,2974,2975,2976,2976,2977,2978,2978,2979,2980,2980,2981,2981,2982,2983,2983,
                2984,2985,2985,2986,2987,2987,2988,2989,2989,2990,2991,2991,2992,2993,2993,2994,2995,2995,2996,2996,2997,2998,2998,2999,3000,3000,3001,3002,3002,3003,3004,3004,
                3005,3006,3006,3007,3007,3008,3009,3009,3010,3011,3011,3012,3013,3013,3014,3015,3015,3016,3016,3017,3018,3018,3019,3020,3020,3021,3022,3022,3023,3023,3024,3025,
                3025,3026,3027,3027,3028,3029,3029,3030,3031,3031,3032,3032,3033,3034,3034,3035,3035,3036,3037,3037,3038,3038,3039,3040,3040,3041,3041,3042,3043,3043,3044,3044,
                3045,3046,3046,3047,3047,3048,3049,3049,3050,3050,3051,3052,3052,3053,3053,3054,3055,3055,3056,3056,3057,3058,3058,3059,3059,3060,3061,3061,3062,3063,3063,3064,
                3064,3065,3066,3066,3067,3067,3068,3069,3069,3070,3070,3071,3072,3072,3073,3073,3074,3075,3075,3076,3076,3077,3078,3078,3079,3079,3080,3081,3081,3082,3082,3083,
                3084,3084,3085,3085,3086,3087,3087,3088,3088,3089,3090,3090,3091,3091,3092,3093,3093,3094,3094,3095,3096,3096,3097,3097,3098,3099,3099,3100,3100,3101,3102,3102,
                3103,3103,3104,3104,3105,3106,3106,3107,3107,3108,3109,3109,3110,3110,3111,3112,3112,3113,3113,3114,3115,3115,3116,3116,3117,3118,3118,3119,3119,3120,3121,3121,
                3122,3122,3123,3124,3124,3125,3125,3126,3127,3127,3128,3128,3129,3130,3130,3131,3131,3132,3133,3133,3134,3134,3135,3135,3136,3137,3137,3138,3138,3139,3140,3140,
                3141,3141,3142,3143,3143,3144,3144,3145,3146,3146,3147,3147,3148,3149,3149,3150,3150,3151,3152,3152,3153,3153,3154,3154,3155,3156,3156,3157,3157,3158,3159,3159,
                3160,3160,3161,3162,3162,3163,3163,3164,3165,3165,3166,3166,3167,3167,3168,3169,3169,3170,3170,3171,3172,3172,3173,3173,3174,3175,3175,3176,3176,3177,3178,3178,
                3179,3179,3180,3180,3181,3182,3182,3183,3183,3184,3185,3185,3186,3186,3187,3188,3188,3189,3189,3190,3190,3191,3192,3192,3193,3193,3194,3195,3195,3196,3196,3197,
                3198,3198,3199,3199,3200,3200,3201,3202,3202,3203,3203,3204,3205,3205,3206,3206,3207,3207,3208,3209,3209,3209,3210,3210,3211,3211,3212,3212,3213,3213,3214,3214,
                3215,3215,3216,3216,3217,3217,3218,3218,3219,3219,3220,3220,3221,3221,3222,3222,3223,3223,3224,3224,3225,3226,3226,3227,3227,3228,3228,3229,3229,3230,3230,3231,
                3231,3232,3232,3233,3233,3234,3234,3235,3235,3236,3236,3237,3237,3238,3238,3239,3239,3240,3240,3241,3241,3242,3242,3243,3243,3244,3244,3245,3245,3246,3246,3247,
                3247,3248,3248,3249,3249,3250,3250,3251,3251,3252,3252,3253,3254,3254,3255,3255,3256,3256,3257,3257,3258,3258,3259,3259,3260,3260,3261,3261,3262,3262,3263,3263,
                3264,3264,3265,3265,3266,3266,3267,3267,3268,3268,3269,3269,3270,3270,3271,3271,3272,3272,3273,3273,3274,3274,3275,3275,3276,3276,3277,3277,3278,3278,3279,3279,
                3280,3280,3281,3281,3282,3282,3283,3283,3284,3284,3285,3285,3286,3286,3287,3287,3288,3288,3289,3289,3290,3290,3291,3291,3292,3292,3293,3293,3293,3294,3294,3295,
                3295,3296,3296,3297,3297,3298,3298,3299,3299,3300,3300,3301,3301,3302,3302,3303,3303,3304,3304,3305,3305,3306,3306,3307,3307,3308,3308,3309,3309,3310,3310,3311,
                3311,3312,3312,3313,3313,3314,3314,3315,3315,3316,3316,3317,3317,3318,3318,3319,3319,3320,3320,3320,3321,3321,3322,3322,3323,3323,3324,3324,3325,3325,3326,3326,
                3327,3327,3328,3328,3329,3329,3330,3330,3331,3331,3332,3332,3333,3333,3334,3334,3335,3335,3336,3336,3336,3337,3337,3338,3338,3339,3339,3340,3340,3341,3341,3342,
                3342,3343,3343,3344,3344,3345,3345,3346,3346,3347,3347,3348,3348,3348,3349,3349,3350,3350,3351,3351,3352,3352,3353,3353,3354,3354,3355,3355,3356,3356,3357,3357,
                3358,3358,3359,3359,3359,3360,3360,3361,3361,3362,3362,3363,3363,3364,3364,3365,3365,3366,3366,3367,3367,3368,3368,3368,3369,3369,3370,3370,3371,3371,3372,3372,
                3373,3373,3374,3374,3375,3375,3376,3376,3377,3377,3377,3378,3378,3379,3379,3380,3380,3381,3381,3382,3382,3383,3383,3384,3384,3384,3385,3385,3386,3386,3387,3387,
                3388,3388,3389,3389,3390,3390,3391,3391,3391,3392,3392,3393,3393,3394,3394,3395,3395,3396,3396,3397,3397,3398,3398,3398,3399,3399,3400,3400,3401,3401,3402,3402,
                3403,3403,3404,3404,3404,3405,3405,3406,3406,3407,3407,3408,3408,3409,3409,3410,3410,3411,3411,3411,3412,3412,3413,3413,3414,3414,3415,3415,3416,3416,3416,3417,
                3417,3418,3418,3419,3419,3420,3420,3421,3421,3422,3422,3422,3423,3423,3424,3424,3425,3425,3426,3426,3427,3427,3427,3428,3428,3429,3429,3430,3430,3431,3431,3432,
                3432,3432,3433,3433,3434,3434,3435,3435,3436,3436,3437,3437,3437,3438,3438,3439,3439,3440,3440,3441,3441,3442,3442,3442,3443,3443,3444,3444,3445,3445,3446,3446,
                3446,3447,3447,3448,3448,3449,3449,3450,3450,3451,3451,3451,3452,3452,3453,3453,3454,3454,3455,3455,3455,3456,3456,3457,3457,3458,3458,3459,3459,3459,3460,3460,
                3461,3461,3462,3462,3463,3463,3463,3464,3464,3465,3465,3466,3466,3467,3467,3467,3468,3468,3469,3469,3470,3470,3471,3471,3471,3472,3472,3473,3473,3474,3474,3475,
                3475,3475,3476,3476,3477,3477,3478,3478,3479,3479,3479,3480,3480,3481,3481,3482,3482,3482,3483,3483,3484,3484,3485,3485,3486,3486,3486,3487,3487,3488,3488,3489,
                3489,3489,3490,3490,3491,3491,3492,3492,3493,3493,3493,3494,3494,3495,3495,3496,3496,3496,3497,3497,3498,3498,3499,3499,3499,3500,3500,3501,3501,3502,3502,3502,
                3503,3503,3504,3504,3505,3505,3506,3506,3506,3507,3507,3508,3508,3509,3509,3509,3510,3510,3511,3511,3512,3512,3512,3513,3513,3514,3514,3515,3515,3515,3516,3516,
                3517,3517,3518,3518,3518,3519,3519,3520,3520,3521,3521,3521,3522,3522,3523,3523,3524,3524,3524,3525,3525,3526,3526,3526,3527,3527,3528,3528,3529,3529,3529,3530,
                3530,3531,3531,3532,3532,3532,3533,3533,3534,3534,3535,3535,3535,3536,3536,3537,3537,3537,3538,3538,3539,3539,3540,3540,3540,3541,3541,3542,3542,3543,3543,3543,
                3544,3544,3545,3545,3545,3546,3546,3547,3547,3548,3548,3548,3549,3549,3550,3550,3550,3551,3551,3552,3552,3553,3553,3553,3554,3554,3555,3555,3555,3556,3556,3557,
                3557,3558,3558,3558,3559,3559,3560,3560,3560,3561,3561,3562,3562,3563,3563,3563,3564,3564,3565,3565,3565,3566,3566,3567,3567,3567,3568,3568,3569,3569,3570,3570,
                3570,3571,3571,3572,3572,3572,3573,3573,3574,3574,3574,3575,3575,3576,3576,3576,3577,3577,3578,3578,3579,3579,3580,3580,3581,3581,3581,3582,3582,3582,3583,3583,
                3583,3584,3584,3584,3585,3585,3586,3586,3586,3587,3587,3587,3588,3588,3588,3589,3589,3589,3590,3590,3591,3591,3591,3592,3592,3592,3593,3593,3593,3594,3594,3595,
                3595,3595,3596,3596,3596,3597,3597,3597,3598,3598,3598,3599,3599,3600,3600,3600,3601,3601,3601,3602,3602,3602,3603,3603,3604,3604,3604,3605,3605,3605,3606,3606,
                3606,3607,3607,3607,3608,3608,3609,3609,3609,3610,3610,3610,3611,3611,3611,3612,3612,3613,3613,3613,3614,3614,3614,3615,3615,3615,3616,3616,3617,3617,3617,3618,
                3618,3618,3619,3619,3619,3620,3620,3621,3621,3621,3622,3622,3622,3623,3623,3623,3624,3624,3625,3625,3625,3626,3626,3626,3627,3627,3627,3628,3628,3629,3629,3629,
                3630,3630,3630,3631,3631,3631,3632,3632,3633,3633,3633,3634,3634,3634,3635,3635,3635,3636,3636,3637,3637,3637,3638,3638,3638,3639,3639,3639,3640,3640,3641,3641,
                3641,3642,3642,3642,3643,3643,3643,3644,3644,3645,3645,3645,3646,3646,3646,3647,3647,3647,3648,3648,3649,3649,3649,3650,3650,3650,3651,3651,3652,3652,3652,3653,
                3653,3653,3654,3654,3654,3655,3655,3656,3656,3656,3657,3657,3657,3658,3658,3658,3659,3659,3660,3660,3660,3661,3661,3661,3662,3662,3663,3663,3663,3664,3664,3664,
                3665,3665,3665,3666,3666,3667,3667,3667,3668,3668,3668,3669,3669,3670,3670,3670,3671,3671,3671,3672,3672,3672,3673,3673,3674,3674,3674,3675,3675,3675,3676,3676,
                3677,3677,3677,3678,3678,3678,3679,3679,3680,3680,3680,3681,3681,3681,3682,3682,3682,3683,3683,3684,3684,3684,3685,3685,3685,3686,3686,3687,3687,3687,3688,3688,
                3688,3689,3689,3690,3690,3690,3691,3691,3691,3692,3692,3692,3693,3693,3694,3694,3694,3695,3695,3695,3696,3696,3697,3697,3697,3698,3698,3698,3699,3699,3700,3700,
                3700,3701,3701,3701,3702,3702,3703,3703,3703,3704,3704,3704,3705,3705,3706,3706,3706,3707,3707,3707,3708,3708,3709,3709,3709,3710,3710,3710,3711,3711,3711,3712,
                3712,3713,3713,3713,3714,3714,3714,3715,3715,3716,3716,3716,3717,3717,3717,3718,3718,3719,3719,3719,3720,3720,3720,3721,3721,3722,3722,3722,3723,3723,3723,3724,
                3724,3725,3725,3725,3726,3726,3726,3727,3727,3728,3728,3728,3729,3729,3729,3730,3730,3731,3731,3731,3732,3732,3732,3733,3733,3734,3734,3734,3735,3735,3736,3736,
                3736,3737,3737,3737,3738,3738,3739,3739,3739,3740,3740,3740,3741,3741,3742,3742,3742,3743,3743,3743,3744,3744,3745,3745,3745,3746,3746,3746,3747,3747,3748,3748,
                3748,3749,3749,3749,3750,3750,3751,3751,3751,3752,3752,3753,3753,3753,3754,3754,3754,3755,3755,3756,3756,3756,3757,3757,3757,3758,3758,3759,3759,3759,3760,3760,
                3760,3761,3761,3762,3762,3762,3763,3763,3764,3764,3764,3765,3765,3765,3766,3766,3766,3767,3767,3767,3768,3768,3768,3769,3769,3769,3770,3770,3770,3771,3771,3771,
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
                   0,   3,   7,   9,  12,  15,  17,  20,  22,  24,  27,  29,  31,  33,  36,  38,  40,  42,  44,  46,  48,  50,  52,  54,  56,  58,  60,  62,  64,  66,  68,  69,
                  71,  73,  75,  77,  79,  81,  82,  84,  86,  88,  90,  91,  93,  95,  97,  98, 100, 102, 104, 105, 107, 109, 111, 112, 114, 116, 117, 119, 121, 122, 124, 126,
                 127, 129, 131, 132, 134, 136, 137, 139, 141, 142, 144, 146, 147, 149, 150, 152, 154, 155, 157, 158, 160, 162, 163, 165, 166, 168, 170, 171, 173, 174, 176, 177,
                 179, 181, 182, 184, 185, 187, 188, 190, 191, 193, 194, 196, 197, 199, 201, 202, 204, 205, 207, 208, 210, 211, 213, 214, 216, 217, 219, 220, 222, 223, 225, 227,
                 228, 230, 232, 233, 235, 237, 238, 240, 242, 243, 245, 247, 248, 250, 252, 253, 255, 257, 259, 260, 262, 264, 266, 267, 269, 271, 273, 275, 276, 278, 280, 282,
                 284, 286, 287, 289, 291, 293, 295, 297, 298, 300, 302, 304, 306, 308, 310, 312, 314, 316, 318, 320, 321, 323, 325, 327, 329, 331, 333, 335, 337, 339, 341, 343,
                 345, 347, 349, 351, 353, 356, 358, 360, 362, 364, 366, 368, 370, 372, 374, 376, 379, 381, 383, 385, 387, 389, 391, 394, 396, 398, 400, 402, 405, 408, 411, 414,
                 417, 420, 423, 426, 429, 432, 435, 438, 441, 444, 447, 450, 453, 456, 459, 462, 465, 468, 471, 474, 477, 480, 483, 486, 489, 492, 495, 498, 500, 503, 506, 509,
                 512, 515, 518, 521, 524, 527, 530, 532, 535, 538, 541, 544, 547, 550, 553, 555, 558, 561, 564, 567, 570, 572, 575, 578, 581, 584, 587, 589, 592, 595, 598, 601,
                 603, 606, 609, 612, 614, 617, 620, 623, 625, 628, 631, 634, 636, 639, 642, 645, 647, 650, 653, 656, 658, 661, 664, 666, 669, 672, 674, 677, 680, 682, 685, 688,
                 690, 693, 696, 698, 701, 704, 706, 709, 711, 714, 717, 719, 722, 725, 727, 730, 732, 735, 737, 740, 743, 745, 748, 750, 753, 755, 758, 761, 763, 766, 768, 771,
                 773, 776, 778, 781, 783, 786, 788, 791, 793, 796, 798, 801, 803, 806, 808, 811, 813, 815, 818, 820, 823, 825, 828, 830, 833, 835, 837, 840, 842, 845, 847, 849,
                 852, 854, 857, 859, 861, 864, 866, 868, 871, 873, 875, 878, 880, 883, 885, 887, 888, 892, 895, 898, 902, 905, 908, 912, 915, 918, 921, 925, 928, 931, 934, 938,
                 941, 944, 948, 951, 954, 957, 961, 964, 967, 970, 973, 977, 980, 983, 986, 990, 993, 996, 999,1003,1006,1009,1012,1015,1019,1022,1025,1028,1031,1035,1038,1041,
                1044,1047,1050,1054,1057,1060,1063,1066,1069,1073,1076,1079,1082,1085,1088,1092,1095,1098,1101,1104,1107,1110,1113,1117,1120,1123,1126,1129,1132,1135,1138,1141,
                1145,1148,1151,1154,1157,1160,1163,1166,1169,1172,1175,1179,1182,1185,1188,1191,1194,1197,1200,1203,1206,1209,1212,1215,1218,1221,1224,1227,1230,1233,1236,1239,
                1243,1246,1249,1252,1255,1258,1261,1264,1267,1270,1273,1276,1279,1282,1285,1288,1291,1293,1296,1299,1302,1305,1308,1311,1314,1317,1320,1323,1326,1329,1332,1335,
                1338,1341,1344,1347,1350,1353,1355,1358,1361,1364,1367,1370,1373,1376,1379,1382,1385,1387,1390,1393,1396,1399,1402,1405,1408,1411,1413,1416,1419,1422,1425,1428,
                1431,1434,1436,1439,1442,1445,1448,1451,1453,1456,1459,1462,1465,1468,1470,1473,1476,1479,1482,1485,1487,1490,1493,1496,1499,1501,1504,1507,1510,1513,1515,1518,
                1521,1524,1526,1529,1532,1535,1537,1540,1543,1546,1549,1551,1554,1557,1560,1562,1565,1568,1570,1573,1576,1579,1581,1584,1587,1590,1592,1595,1598,1600,1603,1606,
                1609,1611,1614,1616,1619,1621,1624,1626,1628,1631,1633,1635,1638,1640,1643,1645,1647,1649,1652,1654,1656,1659,1661,1663,1665,1668,1670,1672,1674,1677,1679,1681,
                1683,1685,1688,1690,1692,1694,1696,1698,1700,1703,1705,1707,1709,1711,1713,1715,1717,1719,1721,1723,1725,1727,1729,1731,1733,1735,1737,1739,1741,1743,1745,1747,
                1749,1751,1753,1755,1757,1759,1761,1762,1764,1766,1768,1770,1772,1774,1775,1777,1779,1781,1783,1784,1786,1788,1790,1791,1793,1795,1797,1798,1800,1802,1803,1805,
                1807,1808,1810,1812,1813,1815,1817,1818,1820,1821,1823,1825,1826,1828,1829,1831,1832,1834,1836,1837,1839,1840,1842,1843,1845,1847,1849,1850,1852,1854,1856,1857,
                1859,1860,1862,1864,1865,1867,1869,1870,1872,1874,1875,1877,1878,1880,1882,1883,1885,1886,1888,1889,1891,1892,1894,1895,1897,1899,1900,1902,1903,1905,1906,1908,
                1909,1910,1912,1913,1915,1916,1918,1919,1921,1922,1923,1925,1926,1928,1929,1930,1932,1933,1935,1936,1937,1939,1940,1941,1943,1944,1945,1947,1948,1949,1950,1952,
                1953,1954,1956,1957,1958,1959,1961,1962,1963,1964,1966,1967,1968,1969,1970,1972,1973,1974,1975,1976,1977,1979,1980,1981,1982,1983,1984,1985,1987,1988,1989,1990,
                1991,1992,1993,1994,1995,1996,1997,1999,2000,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015,2016,2016,2017,2018,2019,2020,2021,2022,
                2023,2024,2025,2026,2027,2027,2028,2029,2030,2031,2032,2033,2034,2035,2035,2036,2037,2038,2039,2040,2041,2042,2042,2043,2044,2045,2046,2047,2048,2049,2050,2050,
                2051,2052,2053,2054,2055,2056,2057,2057,2058,2059,2060,2061,2062,2063,2064,2064,2065,2066,2067,2068,2069,2070,2071,2071,2072,2073,2074,2075,2076,2077,2077,2078,
                2079,2080,2081,2082,2083,2083,2084,2085,2086,2087,2088,2089,2089,2090,2091,2092,2093,2094,2095,2095,2096,2097,2098,2099,2100,2101,2101,2102,2103,2104,2105,2106,
                2107,2107,2108,2109,2110,2111,2112,2112,2113,2114,2115,2116,2117,2118,2118,2119,2120,2121,2122,2123,2123,2124,2125,2126,2127,2128,2128,2129,2130,2131,2132,2133,
                2133,2134,2135,2136,2137,2138,2138,2139,2140,2141,2142,2143,2143,2144,2145,2146,2147,2148,2148,2149,2150,2151,2152,2153,2153,2154,2155,2156,2157,2157,2158,2159,
                2160,2161,2162,2162,2163,2164,2165,2166,2166,2167,2168,2169,2170,2171,2171,2172,2173,2174,2175,2175,2176,2177,2178,2179,2179,2180,2181,2182,2183,2183,2184,2185,
                2186,2187,2188,2188,2189,2190,2191,2192,2192,2193,2194,2195,2196,2196,2197,2198,2199,2200,2200,2201,2202,2203,2203,2204,2205,2206,2207,2207,2208,2209,2210,2211,
                2211,2212,2213,2214,2215,2215,2216,2217,2218,2218,2219,2220,2221,2222,2222,2223,2224,2225,2226,2226,2227,2228,2229,2229,2230,2231,2232,2233,2233,2234,2235,2236,
                2236,2237,2238,2239,2239,2240,2241,2242,2243,2243,2244,2245,2246,2246,2247,2248,2249,2249,2250,2251,2252,2253,2253,2254,2255,2256,2256,2257,2258,2259,2259,2260,
                2261,2262,2262,2263,2264,2265,2265,2266,2267,2268,2268,2269,2270,2271,2272,2272,2271,2272,2273,2274,2275,2276,2276,2277,2278,2279,2280,2280,2281,2282,2283,2284,
                2284,2285,2286,2287,2288,2288,2289,2290,2291,2292,2292,2293,2294,2295,2296,2296,2297,2298,2299,2300,2300,2301,2302,2303,2304,2304,2305,2306,2307,2308,2308,2309,
                2310,2311,2311,2312,2313,2314,2315,2315,2316,2317,2318,2319,2319,2320,2321,2322,2323,2323,2324,2325,2326,2326,2327,2328,2329,2330,2330,2331,2332,2333,2333,2334,
                2335,2336,2337,2337,2338,2339,2340,2340,2341,2342,2343,2344,2344,2345,2346,2347,2347,2348,2349,2350,2351,2351,2352,2353,2354,2354,2355,2356,2357,2357,2358,2359,
                2360,2361,2361,2362,2363,2364,2364,2365,2366,2367,2367,2368,2369,2370,2371,2371,2372,2373,2374,2374,2375,2376,2377,2377,2378,2379,2380,2380,2381,2382,2383,2383,
                2384,2385,2386,2386,2387,2388,2389,2389,2390,2391,2392,2392,2393,2394,2395,2395,2396,2397,2398,2398,2399,2400,2401,2401,2402,2403,2404,2404,2405,2406,2407,2407,
                2408,2409,2410,2410,2411,2412,2413,2413,2414,2415,2416,2416,2417,2418,2419,2419,2420,2421,2422,2422,2423,2424,2424,2425,2426,2427,2427,2428,2429,2430,2430,2431,
                2432,2433,2433,2434,2435,2435,2436,2437,2438,2438,2439,2440,2441,2441,2442,2443,2443,2444,2445,2446,2446,2447,2448,2449,2449,2450,2451,2451,2452,2453,2454,2454,
                2455,2456,2456,2457,2458,2459,2459,2460,2461,2462,2462,2463,2464,2464,2465,2466,2467,2467,2468,2469,2469,2470,2471,2472,2472,2473,2474,2474,2475,2476,2476,2477,
                2478,2479,2479,2480,2481,2481,2482,2483,2484,2484,2485,2486,2486,2487,2488,2488,2489,2490,2491,2491,2492,2493,2493,2494,2495,2496,2496,2497,2498,2498,2499,2500,
                2500,2501,2502,2502,2503,2504,2505,2505,2506,2507,2507,2508,2509,2509,2510,2511,2511,2512,2513,2514,2514,2515,2516,2516,2517,2518,2518,2519,2520,2520,2521,2522,
                2522,2523,2524,2525,2525,2526,2527,2527,2528,2529,2529,2530,2531,2531,2532,2533,2533,2534,2535,2535,2536,2537,2537,2538,2539,2540,2540,2541,2542,2542,2543,2544,
                2544,2545,2546,2546,2547,2548,2548,2549,2550,2550,2551,2552,2552,2553,2554,2554,2555,2556,2556,2557,2558,2558,2559,2560,2560,2561,2562,2562,2563,2564,2564,2565,
                2566,2566,2567,2568,2568,2569,2570,2570,2571,2572,2572,2573,2573,2574,2575,2575,2576,2577,2577,2578,2579,2579,2580,2581,2581,2582,2583,2583,2584,2585,2585,2586,
                2587,2587,2588,2589,2589,2590,2590,2591,2592,2592,2593,2594,2594,2595,2596,2596,2597,2598,2598,2599,2599,2600,2601,2601,2602,2603,2603,2604,2605,2605,2606,2607,
                2607,2608,2608,2609,2610,2610,2611,2612,2612,2613,2614,2614,2615,2615,2616,2617,2617,2618,2619,2619,2620,2620,2621,2622,2622,2623,2624,2624,2625,2626,2626,2627,
                2627,2628,2629,2629,2630,2631,2631,2632,2632,2633,2634,2634,2635,2635,2636,2637,2637,2638,2639,2639,2640,2640,2641,2642,2642,2643,2644,2644,2645,2645,2646,2647,
                2647,2648,2648,2649,2650,2650,2651,2652,2652,2653,2653,2654,2655,2655,2656,2656,2657,2658,2658,2659,2659,2660,2661,2661,2662,2662,2663,2664,2664,2665,2665,2666,
                2667,2667,2668,2669,2669,2670,2670,2671,2672,2672,2673,2673,2674,2674,2675,2676,2676,2677,2677,2678,2679,2679,2680,2680,2681,2682,2682,2683,2683,2684,2685,2685,
                2686,2686,2687,2688,2688,2689,2689,2690,2691,2692,2693,2694,2694,2695,2696,2696,2697,2698,2699,2699,2700,2701,2701,2702,2703,2704,2704,2705,2706,2706,2707,2708,
                2709,2709,2710,2711,2711,2712,2713,2713,2714,2715,2716,2716,2717,2718,2718,2719,2720,2720,2721,2722,2723,2723,2724,2725,2725,2726,2727,2727,2728,2729,2729,2730,
                2731,2732,2732,2733,2734,2734,2735,2736,2736,2737,2738,2738,2739,2740,2740,2741,2742,2743,2743,2744,2745,2745,2746,2747,2747,2748,2749,2749,2750,2751,2751,2752,
                2753,2753,2754,2755,2755,2756,2757,2758,2758,2759,2760,2760,2761,2762,2762,2763,2764,2764,2765,2766,2766,2767,2768,2768,2769,2770,2770,2771,2772,2772,2773,2774,
                2774,2775,2776,2776,2777,2778,2778,2779,2780,2780,2781,2782,2782,2783,2784,2784,2785,2786,2786,2787,2788,2788,2789,2789,2790,2791,2791,2792,2793,2793,2794,2795,
                2795,2796,2797,2797,2798,2799,2799,2800,2801,2801,2802,2803,2803,2804,2804,2805,2806,2806,2807,2808,2808,2809,2810,2810,2811,2812,2812,2813,2814,2814,2815,2815,
                2816,2817,2817,2818,2819,2819,2820,2821,2821,2822,2822,2823,2824,2824,2825,2826,2826,2827,2828,2828,2829,2829,2830,2831,2831,2832,2833,2833,2834,2834,2835,2836,
                2836,2837,2838,2838,2839,2840,2840,2841,2841,2842,2843,2843,2844,2844,2845,2846,2846,2847,2848,2848,2849,2849,2850,2851,2851,2852,2853,2853,2854,2854,2855,2856,
                2856,2857,2857,2858,2859,2859,2860,2861,2861,2862,2862,2863,2864,2864,2865,2865,2866,2867,2867,2868,2868,2869,2870,2870,2871,2872,2872,2873,2873,2874,2875,2875,
                2876,2876,2877,2878,2878,2879,2879,2880,2881,2881,2882,2882,2883,2884,2884,2885,2885,2886,2887,2887,2888,2888,2889,2889,2890,2891,2891,2892,2892,2893,2894,2894,
                2895,2895,2896,2897,2897,2898,2898,2899,2899,2900,2901,2901,2902,2902,2903,2904,2904,2905,2905,2906,2906,2907,2908,2908,2909,2909,2910,2911,2911,2912,2912,2913,
                2913,2914,2915,2915,2916,2916,2917,2917,2918,2919,2919,2920,2920,2921,2921,2922,2923,2923,2924,2924,2925,2925,2926,2927,2927,2928,2928,2929,2929,2930,2931,2931,
                2932,2932,2933,2933,2934,2935,2935,2936,2936,2937,2937,2938,2938,2939,2940,2940,2941,2941,2942,2942,2943,2943,2944,2945,2945,2946,2946,2947,2947,2948,2948,2949,
                2950,2950,2951,2951,2952,2952,2953,2953,2954,2954,2955,2956,2956,2957,2957,2958,2958,2959,2959,2960,2960,2961,2962,2962,2963,2963,2964,2964,2965,2965,2966,2966,
                2967,2968,2968,2969,2969,2970,2970,2971,2971,2972,2972,2973,2973,2974,2974,2975,2976,2976,2977,2977,2978,2978,2979,2979,2980,2980,2981,2981,2982,2982,2983,2983,
                2984,2985,2985,2986,2986,2987,2987,2988,2988,2989,2989,2990,2990,2991,2991,2992,2992,2993,2993,2994,2994,2995,2995,2996,2997,2997,2998,2998,2999,2999,3000,3000,
                3001,3001,3002,3002,3003,3003,3004,3004,3005,3005,3006,3006,3007,3007,3008,3008,3009,3009,3010,3010,3011,3011,3012,3012,3013,3013,3014,3014,3015,3015,3016,3016,
                3017,3017,3018,3018,3019,3019,3020,3020,3021,3021,3022,3022,3023,3023,3024,3024,3025,3025,3026,3026,3027,3027,3028,3028,3029,3029,3030,3030,3031,3031,3032,3032,
                3033,3033,3034,3034,3035,3035,3036,3036,3037,3037,3037,3038,3038,3039,3039,3040,3040,3041,3041,3042,3042,3043,3043,3044,3044,3045,3045,3046,3046,3047,3047,3048,
                3048,3048,3049,3049,3050,3050,3051,3051,3052,3052,3053,3053,3054,3054,3055,3055,3056,3056,3057,3057,3057,3058,3058,3059,3059,3060,3060,3061,3061,3062,3062,3063,
                3063,3063,3064,3064,3065,3065,3066,3066,3067,3067,3068,3068,3069,3069,3069,3070,3070,3071,3071,3072,3072,3073,3073,3074,3074,3074,3075,3075,3076,3076,3077,3077,
                3078,3078,3078,3079,3079,3080,3080,3081,3081,3082,3082,3083,3083,3083,3084,3084,3085,3085,3086,3086,3086,3087,3087,3088,3088,3089,3089,3090,3090,3090,3091,3091,
                3092,3092,3093,3093,3094,3095,3096,3096,3097,3098,3099,3099,3100,3101,3102,3102,3103,3104,3104,3105,3106,3107,3107,3108,3109,3109,3110,3111,3112,3112,3113,3114,
                3114,3115,3116,3117,3117,3118,3119,3119,3120,3121,3121,3122,3123,3124,3124,3125,3126,3126,3127,3128,3128,3129,3130,3131,3131,3132,3133,3133,3134,3135,3135,3136,
                3137,3138,3138,3139,3140,3140,3141,3142,3142,3143,3144,3144,3145,3146,3146,3147,3148,3149,3149,3150,3151,3151,3152,3153,3153,3154,3155,3155,3156,3157,3157,3158,
                3159,3159,3160,3161,3161,3162,3163,3163,3164,3165,3165,3166,3167,3167,3168,3169,3169,3170,3171,3171,3172,3173,3173,3174,3175,3175,3176,3177,3177,3178,3179,3179,
                3180,3181,3181,3182,3183,3183,3184,3185,3185,3186,3187,3187,3188,3189,3189,3190,3190,3191,3192,3192,3193,3194,3194,3195,3196,3196,3197,3198,3198,3199,3199,3200,
                3201,3201,3202,3203,3203,3204,3205,3205,3206,3206,3207,3208,3208,3209,3210,3210,3211,3212,3212,3213,3213,3214,3215,3215,3216,3217,3217,3218,3218,3219,3220,3220,
                3221,3222,3222,3223,3223,3224,3225,3225,3226,3227,3227,3228,3228,3229,3230,3230,3231,3231,3232,3233,3233,3234,3234,3235,3236,3236,3237,3238,3238,3239,3239,3240,
                3241,3241,3242,3242,3243,3244,3244,3245,3245,3246,3247,3247,3248,3248,3249,3250,3250,3251,3251,3252,3253,3253,3254,3254,3255,3256,3256,3257,3257,3258,3258,3259,
                3260,3260,3261,3261,3262,3263,3263,3264,3264,3265,3265,3266,3267,3267,3268,3268,3269,3270,3270,3271,3271,3272,3272,3273,3274,3274,3275,3275,3276,3276,3277,3278,
                3278,3279,3279,3280,3280,3281,3282,3282,3283,3283,3284,3284,3285,3285,3286,3287,3287,3288,3288,3289,3289,3290,3291,3291,3292,3292,3293,3293,3294,3294,3295,3296,
                3296,3297,3297,3298,3298,3299,3299,3300,3300,3301,3302,3302,3303,3303,3304,3304,3305,3305,3306,3306,3307,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,3313,
                3313,3314,3315,3315,3316,3316,3317,3317,3318,3318,3319,3319,3320,3320,3321,3321,3322,3322,3323,3324,3324,3325,3325,3326,3326,3327,3327,3328,3328,3329,3329,3330,
                3330,3331,3331,3332,3332,3333,3333,3334,3334,3335,3335,3336,3336,3337,3337,3338,3338,3339,3339,3340,3337,3338,3339,3339,3340,3340,3341,3341,3342,3342,3343,3343,
                3344,3344,3345,3345,3346,3347,3347,3348,3348,3349,3349,3350,3350,3351,3351,3352,3352,3353,3353,3354,3355,3355,3356,3356,3357,3357,3358,3358,3359,3359,3360,3360,
                3361,3361,3362,3362,3363,3364,3364,3365,3365,3366,3366,3367,3367,3368,3368,3369,3369,3370,3371,3371,3372,3372,3373,3373,3374,3374,3375,3375,3376,3376,3377,3377,
                3378,3379,3379,3380,3380,3381,3381,3382,3382,3383,3383,3384,3384,3385,3386,3386,3387,3387,3388,3388,3389,3389,3390,3390,3391,3391,3392,3393,3393,3394,3394,3395,
                3395,3396,3396,3397,3397,3398,3398,3399,3400,3400,3401,3401,3402,3402,3403,3403,3404,3404,3405,3405,3406,3407,3407,3408,3408,3409,3409,3410,3410,3411,3411,3412,
                3412,3413,3414,3414,3415,3415,3416,3416,3417,3417,3418,3418,3419,3420,3420,3421,3421,3422,3422,3423,3423,3424,3424,3425,3425,3426,3427,3427,3428,3428,3429,3429,
                3430,3430,3431,3431,3432,3433,3433,3434,3434,3435,3435,3436,3436,3437,3437,3438,3439,3439,3440,3440,3441,3441,3442,3442,3443,3443,3444,3445,3445,3446,3446,3447,
                3447,3448,3448,3449,3449,3450,3451,3451,3452,3452,3453,3453,3454,3454,3455,3455,3456,3457,3457,3458,3458,3459,3459,3460,3460,3461,3461,3462,3463,3463,3464,3464,
                3465,3465,3466,3466,3467,3468,3468,3469,3469,3470,3470,3471,3471,3472,3472,3473,3474,3474,3475,3475,3476,3476,3477,3477,3478,3479,3479,3480,3480,3481,3481,3482,
                3482,3483,3483,3484,3485,3485,3486,3486,3487,3487,3488,3488,3489,3490,3490,3491,3491,3492,3492,3493,3493,3494,3495,3495,3496,3496,3497,3497,3498,3498,3499,3500,
                3500,3501,3501,3502,3502,3503,3503,3504,3504,3505,3506,3506,3507,3507,3508,3508,3509,3509,3510,3511,3511,3512,3512,3513,3513,3514,3514,3515,3516,3516,3517,3517,
                3518,3518,3519,3519,3520,3521,3521,3522,3522,3523,3523,3524,3525,3525,3526,3526,3527,3527,3528,3528,3529,3530,3530,3531,3531,3532,3532,3533,3533,3534,3535,3535,
                3536,3536,3537,3537,3538,3538,3539,3540,3540,3541,3541,3542,3542,3543,3543,3544,3545,3545,3546,3546,3547,3547,3548,3549,3549,3550,3550,3551,3551,3552,3552,3553,
                3554,3554,3555,3555,3556,3556,3557,3558,3558,3559,3559,3560,3560,3561,3561,3562,3563,3563,3564,3564,3565,3565,3566,3567,3567,3568,3568,3569,3569,3570,3570,3571,
                3572,3572,3573,3573,3574,3574,3575,3576,3576,3577,3577,3578,3578,3579,3579,3580,3581,3581,3582,3582,3583,3583,3584,3585,3585,3586,3586,3587,3587,3588,3589,3589,
                3590,3590,3591,3591,3592,3593,3593,3594,3594,3595,3595,3596,3596,3597,3598,3598,3599,3599,3600,3600,3601,3602,3602,3603,3603,3604,3604,3605,3606,3606,3607,3607,
                3608,3608,3609,3610,3610,3611,3611,3612,3612,3613,3614,3614,3615,3615,3616,3616,3617,3618,3618,3619,3619,3620,3620,3621,3622,3622,3623,3623,3624,3624,3625,3626,
                3626,3627,3627,3628,3628,3629,3630,3630,3631,3631,3632,3632,3633,3634,3634,3635,3635,3636,3636,3637,3638,3638,3639,3639,3640,3640,3641,3642,3642,3643,3643,3644,
                3644,3645,3646,3646,3647,3647,3648,3648,3649,3650,3650,3651,3651,3652,3652,3653,3654,3654,3655,3655,3656,3656,3657,3658,3658,3659,3659,3660,3661,3661,3662,3662,
                3663,3663,3664,3665,3665,3666,3666,3667,3667,3668,3669,3669,3670,3670,3671,3671,3672,3673,3673,3674,3674,3675,3676,3676,3677,3677,3678,3678,3679,3680,3680,3681,
                3681,3682,3682,3683,3684,3684,3685,3685,3686,3687,3687,3688,3688,3689,3689,3690,3691,3691,3692,3692,3693,3693,3694,3695,3695,3696,3696,3697,3698,3698,3699,3699,
                3700,3700,3701,3702,3702,3703,3703,3704,3705,3705,3706,3706,3707,3707,3708,3709,3709,3710,3710,3711,3712,3712,3713,3713,3714,3714,3715,3716,3716,3717,3717,3718,
                3719,3719,3720,3720,3721,3721,3722,3723,3723,3724,3724,3725,3726,3726,3727,3727,3728,3728,3729,3730,3730,3731,3731,3732,3733,3733,3734,3734,3735,3735,3736,3737,
                3737,3738,3738,3739,3740,3740,3741,3741,3742,3743,3743,3744,3744,3745,3745,3746,3747,3747,3748,3748,3749,3750,3750,3751,3751,3752,3753,3753,3754,3754,3755,3755,
                3756,3757,3757,3758,3758,3759,3760,3760,3761,3761,3762,3763,3763,3764,3764,3765,3765,3766,3767,3767,3768,3768,3769,3770,3770,3771,3771,3772,3773,3773,3774,3774,
                3775,3776,3776,3777,3777,3778,3778,3779,3780,3780,3781,3781,3782,3783,3783,3784,3784,3785,3786,3786,3787,3787,3788,3789,3789,3790,3790,3791,3792,3792,3793,3793,
                3794,3795,3795,3796,3796,3797,3797,3798,3799,3799,3800,3800,3801,3802,3802,3803,3803,3804,3805,3805,3806,3806,3807,3808,3808,3809,3809,3810,3811,3811,3812,3812,
                3813,3814,3814,3815,3815,3816,3817,3817,3818,3818,3819,3820,3820,3821,3821,3822,3823,3823,3824,3824,3825,3826,3826,3827,3827,3828,3829,3829,3830,3830,3831,3831,
                3832,3833,3833,3834,3834,3835,3836,3836,3837,3837,3838,3839,3839,3840,3840,3841,3842,3842,3843,3843,3844,3845,3845,3846,3847,3848,3849,3850,3851,3851,3852,3853,
                3854,3855,3856,3856,3857,3858,3859,3860,3861,3861,3862,3863,3864,3865,3866,3867,3867,3868,3869,3870,3871,3872,3872,3873,3874,3875,3876,3877,3878,3878,3879,3880,
                3881,3882,3883,3883,3884,3885,3886,3887,3888,3888,3889,3890,3891,3892,3893,3894,3894,3895,3896,3897,3898,3899,3899,3900,3901,3902,3903,3904,3904,3905,3906,3907,
                3908,3909,3909,3910,3911,3912,3913,3914,3915,3915,3916,3917,3918,3919,3920,3920,3921,3922,3923,3924,3925,3925,3926,3927,3928,3929,3930,3931,3931,3932,3933,3934,
                3935,3936,3936,3937,3938,3939,3940,3941,3941,3942,3943,3944,3945,3946,3946,3947,3948,3949,3950,3951,3951,3952,3953,3954,3955,3956,3957,3957,3958,3959,3960,3961,
                3962,3962,3963,3964,3965,3966,3967,3967,3968,3969,3970,3971,3972,3972,3973,3974,3975,3976,3977,3977,3978,3979,3980,3981,3982,3983,3983,3984,3985,3986,3987,3988,
                3988,3989,3990,3991,3992,3993,3993,3994,3995,3996,3997,3998,3998,3999,4000,4001,4002,4003,4003,4004,4005,4006,4007,4008,4008,4009,4010,4011,4012,4013,4014,4014,
                4015,4016,4017,4018,4019,4019,4020,4021,4022,4023,4024,4024,4025,4026,4027,4028,4029,4029,4030,4031,4032,4033,4034,4034,4035,4036,4037,4038,4039,4039,4040,4041,
                4042,4043,4044,4044,4045,4046,4047,4048,4049,4049,4050,4051,4052,4053,4054,4054,4055,4056,4057,4058,4059,4059,4060,4061,4062,4063,4064,4064,4065,4066,4067,4068,
                4069,4069,4070,4071,4072,4073,4074,4074,4075,4076,4077,4078,4079,4079,4080,4081,4082,4083,4084,4084,4085,4086,4087,4088,4089,4089,4090,4091,4092,4093,4094,4095,
          },
          /* aec trigger 3 */
          {
                   0,   1,   3,   5,   7,   9,  11,  13,  15,  17,  19,  21,  23,  25,  27,  29,  31,  33,  35,  37,  39,  41,  43,  45,  46,  48,  50,  53,  55,  57,  59,  61,
                  63,  65,  67,  69,  71,  73,  75,  77,  79,  81,  83,  85,  87,  89,  91,  93,  95,  97, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 124, 126, 129,
                 132, 135, 138, 141, 143, 146, 149, 151, 154, 156, 159, 161, 163, 166, 168, 170, 172, 175, 177, 179, 181, 183, 185, 186, 188, 190, 192, 194, 197, 199, 202, 204,
                 207, 209, 212, 214, 216, 219, 221, 223, 226, 228, 230, 232, 234, 237, 239, 241, 243, 245, 247, 249, 251, 252, 254, 256, 257, 261, 265, 268, 272, 275, 279, 282,
                 285, 289, 292, 296, 299, 303, 306, 309, 313, 316, 320, 323, 326, 330, 333, 336, 339, 343, 346, 349, 352, 356, 359, 362, 365, 369, 372, 375, 378, 381, 384, 387,
                 391, 394, 397, 400, 403, 406, 409, 412, 415, 418, 421, 424, 427, 430, 433, 436, 439, 442, 445, 448, 451, 453, 456, 459, 462, 465, 468, 471, 473, 476, 479, 482,
                 484, 487, 490, 493, 495, 498, 501, 504, 506, 509, 512, 514, 517, 519, 522, 525, 527, 530, 532, 535, 537, 540, 543, 545, 548, 550, 553, 555, 558, 562, 565, 568,
                 571, 574, 577, 580, 583, 586, 589, 592, 595, 598, 601, 603, 606, 609, 612, 615, 618, 620, 623, 626, 629, 631, 634, 637, 640, 642, 645, 647, 650, 653, 655, 658,
                 660, 663, 665, 668, 670, 673, 675, 678, 680, 683, 685, 687, 690, 692, 694, 697, 699, 701, 704, 706, 708, 710, 713, 715, 717, 719, 721, 723, 726, 728, 730, 732,
                 734, 736, 738, 740, 742, 744, 746, 748, 750, 752, 753, 755, 755, 757, 760, 763, 765, 768, 771, 773, 776, 779, 781, 784, 786, 789, 792, 794, 797, 799, 802, 804,
                 807, 809, 812, 814, 817, 819, 822, 824, 827, 829, 832, 834, 837, 839, 841, 844, 846, 849, 851, 853, 856, 858, 861, 863, 865, 868, 870, 872, 874, 877, 879, 881,
                 884, 886, 888, 890, 892, 895, 897, 899, 901, 903, 906, 908, 910, 912, 914, 916, 918, 921, 923, 925, 927, 929, 931, 933, 935, 937, 939, 941, 943, 945, 947, 949,
                 951, 953, 955, 957, 959, 961, 963, 965, 967, 969, 971, 973, 974, 976, 978, 980, 982, 984, 986, 989, 991, 993, 996, 998,1000,1003,1005,1007,1010,1012,1014,1017,
                1019,1021,1024,1026,1028,1031,1033,1035,1037,1040,1042,1044,1046,1049,1051,1053,1055,1058,1060,1062,1064,1066,1069,1071,1073,1075,1077,1080,1082,1084,1086,1088,
                1090,1093,1095,1097,1099,1101,1103,1105,1108,1110,1112,1114,1116,1118,1120,1122,1124,1126,1128,1131,1133,1135,1137,1139,1141,1143,1145,1147,1149,1151,1153,1155,
                1157,1159,1161,1163,1165,1167,1169,1171,1173,1175,1177,1179,1181,1183,1184,1186,1188,1190,1192,1194,1196,1198,1200,1202,1204,1205,1207,1209,1211,1213,1215,1217,
                1218,1220,1222,1224,1226,1228,1229,1231,1233,1235,1237,1238,1240,1242,1244,1246,1247,1249,1251,1253,1254,1256,1258,1260,1261,1263,1265,1267,1268,1270,1272,1273,
                1275,1277,1278,1280,1282,1284,1285,1287,1289,1290,1292,1293,1295,1297,1298,1300,1302,1303,1305,1306,1308,1310,1311,1313,1314,1316,1318,1319,1321,1322,1324,1325,
                1327,1328,1330,1331,1333,1334,1336,1337,1339,1340,1342,1343,1345,1346,1348,1349,1351,1352,1354,1355,1357,1358,1360,1361,1362,1364,1365,1367,1368,1369,1371,1372,
                1374,1375,1376,1378,1379,1380,1382,1383,1385,1386,1387,1389,1390,1391,1393,1394,1395,1396,1398,1399,1400,1402,1403,1404,1405,1407,1408,1409,1411,1412,1413,1414,
                1416,1417,1419,1421,1422,1424,1426,1427,1429,1431,1432,1434,1435,1437,1439,1440,1442,1443,1445,1447,1448,1450,1451,1453,1455,1456,1458,1459,1461,1462,1464,1465,
                1467,1469,1470,1472,1473,1475,1476,1478,1479,1481,1482,1484,1485,1487,1488,1490,1491,1493,1494,1496,1497,1499,1500,1502,1503,1505,1506,1507,1509,1510,1512,1513,
                1515,1516,1517,1519,1520,1522,1523,1525,1526,1527,1529,1530,1532,1533,1534,1536,1537,1538,1540,1541,1543,1544,1545,1547,1548,1549,1551,1552,1553,1555,1556,1557,
                1559,1560,1561,1563,1564,1565,1567,1568,1569,1571,1572,1573,1574,1576,1577,1578,1579,1581,1582,1583,1585,1586,1587,1588,1590,1592,1593,1595,1596,1598,1600,1601,
                1603,1604,1606,1607,1609,1610,1612,1614,1615,1617,1618,1620,1621,1623,1624,1626,1627,1629,1630,1632,1633,1635,1636,1638,1639,1641,1642,1644,1645,1647,1648,1650,
                1651,1653,1654,1656,1657,1658,1660,1661,1663,1664,1666,1667,1669,1670,1671,1673,1674,1676,1677,1678,1680,1681,1683,1684,1685,1687,1688,1690,1691,1692,1694,1695,
                1697,1698,1699,1701,1702,1703,1705,1706,1707,1709,1710,1711,1713,1714,1715,1717,1718,1719,1721,1722,1723,1725,1726,1727,1728,1730,1731,1732,1734,1735,1736,1737,
                1739,1740,1741,1743,1744,1745,1746,1748,1749,1750,1751,1753,1754,1755,1756,1757,1759,1760,1761,1762,1764,1765,1766,1767,1768,1770,1771,1772,1773,1774,1775,1777,
                1778,1779,1780,1781,1782,1784,1785,1786,1787,1788,1790,1791,1792,1793,1794,1795,1797,1798,1799,1800,1801,1802,1804,1805,1806,1807,1808,1809,1811,1812,1813,1814,
                1815,1816,1818,1819,1820,1821,1822,1823,1824,1826,1827,1828,1829,1830,1831,1833,1834,1835,1836,1837,1838,1839,1841,1842,1843,1844,1845,1846,1847,1849,1850,1851,
                1852,1853,1854,1855,1856,1858,1859,1860,1861,1862,1863,1864,1866,1867,1868,1869,1870,1871,1872,1873,1874,1876,1877,1878,1879,1880,1881,1882,1883,1885,1886,1887,
                1888,1889,1890,1891,1892,1893,1895,1896,1897,1898,1899,1900,1901,1902,1903,1905,1906,1907,1908,1909,1910,1911,1912,1913,1914,1916,1917,1918,1919,1920,1921,1922,
                1923,1924,1925,1926,1928,1929,1930,1931,1932,1933,1934,1935,1936,1937,1938,1939,1941,1942,1943,1944,1945,1946,1947,1948,1949,1950,1951,1952,1953,1955,1956,1957,
                1958,1959,1960,1961,1962,1963,1964,1965,1966,1967,1968,1969,1970,1972,1973,1974,1975,1976,1977,1978,1979,1980,1981,1982,1983,1984,1985,1986,1987,1988,1989,1990,
                1992,1993,1994,1995,1996,1997,1998,1999,2000,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015,2016,2017,2018,2020,2021,2022,2023,2024,
                2025,2026,2027,2028,2029,2030,2031,2032,2033,2034,2035,2036,2037,2038,2039,2040,2041,2042,2043,2044,2045,2046,2047,2048,2049,2050,2051,2052,2053,2054,2055,2056,
                2057,2058,2059,2060,2061,2062,2063,2064,2065,2066,2067,2068,2069,2070,2071,2072,2073,2074,2075,2076,2077,2078,2079,2080,2081,2082,2083,2084,2085,2086,2087,2088,
                2089,2090,2091,2092,2093,2094,2095,2096,2096,2097,2098,2099,2100,2101,2102,2103,2103,2104,2105,2106,2107,2108,2109,2110,2111,2112,2113,2113,2114,2115,2116,2117,
                2118,2119,2120,2121,2122,2123,2123,2124,2125,2126,2127,2128,2129,2130,2131,2132,2132,2133,2134,2135,2136,2137,2138,2139,2140,2141,2141,2142,2143,2144,2145,2146,
                2147,2148,2149,2150,2150,2151,2152,2153,2154,2155,2156,2157,2158,2159,2159,2160,2161,2162,2163,2164,2165,2166,2167,2168,2168,2169,2170,2171,2172,2173,2174,2175,
                2176,2176,2177,2178,2179,2180,2181,2182,2183,2184,2185,2185,2186,2187,2188,2189,2190,2191,2192,2193,2193,2194,2195,2196,2197,2198,2199,2200,2201,2201,2202,2203,
                2204,2205,2206,2207,2208,2209,2209,2210,2211,2212,2213,2214,2215,2216,2217,2217,2218,2219,2220,2221,2222,2223,2224,2225,2225,2226,2227,2228,2229,2230,2231,2232,
                2232,2233,2234,2235,2236,2237,2238,2239,2240,2240,2241,2242,2243,2244,2245,2246,2247,2247,2248,2249,2250,2251,2252,2253,2254,2255,2255,2256,2257,2258,2259,2260,
                2261,2262,2262,2263,2264,2265,2266,2267,2268,2269,2269,2270,2271,2272,2273,2274,2275,2276,2276,2277,2278,2279,2280,2281,2282,2283,2283,2284,2285,2286,2287,2288,
                2289,2290,2290,2291,2292,2293,2294,2295,2296,2297,2297,2298,2299,2300,2301,2302,2303,2304,2304,2305,2306,2307,2308,2309,2310,2310,2311,2312,2313,2314,2315,2316,
                2317,2317,2318,2319,2320,2321,2322,2323,2324,2324,2325,2326,2327,2328,2329,2330,2330,2331,2332,2333,2334,2335,2336,2336,2337,2338,2339,2340,2341,2342,2343,2343,
                2344,2345,2346,2347,2348,2349,2349,2350,2351,2352,2353,2354,2355,2355,2356,2357,2358,2359,2360,2361,2361,2362,2363,2364,2365,2366,2367,2368,2368,2369,2370,2371,
                2372,2373,2374,2374,2375,2376,2377,2378,2379,2380,2380,2381,2382,2383,2384,2385,2385,2386,2387,2388,2389,2390,2391,2391,2392,2393,2394,2395,2396,2397,2397,2398,
                2399,2400,2401,2402,2403,2403,2404,2405,2406,2407,2408,2409,2409,2410,2411,2412,2413,2414,2414,2415,2416,2417,2418,2419,2420,2420,2421,2422,2423,2424,2425,2425,
                2426,2427,2428,2429,2430,2431,2431,2432,2433,2434,2435,2436,2437,2437,2438,2439,2440,2441,2442,2442,2443,2444,2445,2446,2447,2447,2448,2449,2450,2451,2452,2453,
                2453,2454,2455,2456,2457,2458,2458,2459,2460,2461,2462,2463,2463,2464,2465,2466,2467,2468,2469,2469,2470,2471,2472,2473,2474,2474,2475,2476,2477,2478,2479,2479,
                2480,2481,2482,2483,2484,2484,2485,2486,2487,2488,2489,2489,2490,2491,2492,2493,2494,2494,2495,2496,2497,2498,2499,2499,2500,2501,2502,2503,2504,2504,2505,2506,
                2507,2508,2509,2509,2510,2511,2512,2513,2514,2514,2515,2516,2517,2518,2519,2519,2520,2521,2522,2523,2524,2524,2525,2526,2527,2528,2528,2529,2530,2531,2532,2533,
                2533,2534,2535,2536,2537,2538,2538,2539,2540,2541,2542,2543,2543,2544,2545,2546,2547,2547,2548,2549,2550,2551,2552,2552,2553,2554,2555,2556,2557,2557,2558,2559,
                2560,2561,2561,2562,2563,2564,2565,2566,2566,2567,2568,2569,2570,2570,2571,2572,2573,2574,2575,2575,2576,2577,2578,2579,2579,2580,2581,2582,2583,2584,2584,2585,
                2586,2587,2588,2588,2589,2590,2591,2592,2593,2593,2594,2595,2596,2597,2597,2598,2599,2600,2601,2601,2602,2603,2604,2605,2606,2606,2607,2608,2609,2610,2610,2611,
                2612,2613,2614,2614,2615,2616,2617,2618,2619,2619,2620,2621,2622,2622,2623,2624,2625,2626,2626,2627,2628,2629,2630,2630,2631,2632,2633,2633,2634,2635,2636,2637,
                2637,2638,2639,2640,2640,2641,2642,2643,2644,2644,2645,2646,2647,2647,2648,2649,2650,2651,2651,2652,2653,2654,2654,2655,2656,2657,2658,2658,2659,2660,2661,2661,
                2662,2663,2664,2664,2665,2666,2667,2668,2668,2669,2670,2671,2671,2672,2673,2674,2674,2675,2676,2677,2678,2678,2679,2680,2681,2681,2682,2683,2684,2684,2685,2686,
                2687,2687,2688,2689,2690,2690,2691,2692,2693,2694,2694,2695,2696,2697,2697,2698,2699,2700,2700,2701,2702,2703,2703,2704,2705,2706,2706,2707,2708,2709,2709,2710,
                2711,2712,2712,2713,2714,2715,2715,2716,2717,2718,2718,2719,2720,2721,2721,2722,2723,2724,2724,2725,2726,2727,2727,2728,2729,2730,2730,2731,2732,2733,2733,2734,
                2735,2735,2736,2737,2738,2738,2739,2740,2741,2741,2742,2743,2744,2744,2745,2746,2747,2747,2748,2749,2750,2750,2751,2752,2752,2753,2754,2755,2755,2756,2757,2758,
                2758,2759,2760,2760,2761,2762,2763,2763,2764,2765,2766,2766,2767,2768,2768,2769,2770,2771,2771,2772,2773,2774,2774,2775,2776,2776,2777,2778,2779,2779,2780,2781,
                2782,2782,2783,2784,2784,2785,2786,2787,2787,2788,2789,2789,2790,2791,2792,2792,2793,2794,2794,2795,2796,2797,2797,2798,2799,2799,2800,2801,2802,2802,2803,2804,
                2804,2805,2806,2807,2807,2808,2809,2809,2810,2811,2812,2812,2813,2814,2814,2815,2816,2816,2817,2818,2819,2819,2820,2821,2821,2822,2823,2823,2824,2825,2826,2826,
                2827,2828,2828,2829,2830,2831,2831,2832,2833,2833,2834,2835,2835,2836,2837,2837,2838,2839,2840,2840,2841,2842,2842,2843,2844,2844,2845,2846,2847,2847,2848,2849,
                2849,2850,2851,2851,2852,2853,2853,2854,2855,2855,2856,2857,2858,2858,2859,2860,2860,2861,2862,2862,2863,2864,2864,2865,2866,2866,2867,2868,2868,2869,2870,2871,
                2871,2872,2873,2873,2874,2875,2875,2876,2877,2877,2878,2879,2879,2880,2881,2881,2882,2883,2883,2884,2885,2885,2886,2887,2887,2888,2889,2889,2890,2891,2891,2892,
                2893,2893,2894,2895,2895,2896,2897,2897,2898,2899,2900,2900,2901,2902,2902,2903,2904,2904,2905,2905,2906,2907,2907,2908,2909,2909,2910,2911,2911,2912,2913,2913,
                2914,2915,2915,2916,2917,2917,2918,2919,2919,2920,2921,2921,2922,2923,2923,2924,2925,2925,2926,2927,2927,2928,2929,2929,2930,2931,2931,2932,2932,2933,2934,2934,
                2935,2936,2936,2937,2938,2938,2939,2940,2940,2941,2942,2942,2943,2944,2944,2945,2945,2946,2947,2947,2948,2949,2949,2950,2951,2951,2952,2953,2953,2954,2954,2955,
                2956,2956,2957,2958,2958,2959,2960,2960,2961,2961,2962,2963,2963,2964,2965,2965,2966,2967,2967,2968,2968,2969,2970,2970,2971,2972,2972,2973,2974,2974,2975,2975,
                2976,2977,2977,2978,2979,2979,2980,2980,2981,2982,2982,2983,2984,2984,2985,2985,2986,2987,2987,2988,2989,2989,2990,2990,2991,2992,2992,2993,2994,2994,2995,2995,
                2996,2997,2997,2998,2999,2999,3000,3000,3001,3002,3002,3003,3003,3004,3005,3005,3006,3007,3007,3008,3008,3009,3010,3010,3011,3011,3012,3013,3013,3014,3015,3015,
                3016,3016,3017,3018,3018,3019,3019,3020,3021,3021,3022,3022,3023,3024,3024,3025,3025,3026,3027,3027,3028,3028,3029,3030,3030,3031,3031,3032,3033,3033,3034,3034,
                3035,3036,3036,3037,3037,3038,3039,3039,3040,3040,3041,3042,3042,3043,3043,3044,3045,3045,3046,3046,3047,3048,3048,3049,3049,3050,3051,3051,3052,3052,3053,3054,
                3054,3055,3055,3056,3056,3057,3058,3058,3059,3059,3060,3061,3061,3062,3062,3063,3064,3064,3065,3065,3066,3066,3067,3068,3068,3069,3069,3070,3071,3071,3072,3072,
                3073,3073,3074,3075,3075,3076,3076,3077,3077,3078,3079,3079,3080,3080,3081,3081,3082,3083,3083,3084,3084,3085,3086,3086,3087,3087,3088,3088,3089,3090,3090,3091,
                3091,3092,3092,3093,3094,3095,3096,3096,3097,3098,3099,3099,3100,3101,3102,3102,3103,3104,3104,3105,3106,3107,3107,3108,3109,3109,3110,3111,3112,3112,3113,3114,
                3114,3115,3116,3117,3117,3118,3119,3119,3120,3121,3121,3122,3123,3124,3124,3125,3126,3126,3127,3128,3128,3129,3130,3131,3131,3132,3133,3133,3134,3135,3135,3136,
                3137,3138,3138,3139,3140,3140,3141,3142,3142,3143,3144,3144,3145,3146,3146,3147,3148,3149,3149,3150,3151,3151,3152,3153,3153,3154,3155,3155,3156,3157,3157,3158,
                3159,3159,3160,3161,3161,3162,3163,3163,3164,3165,3165,3166,3167,3167,3168,3169,3169,3170,3171,3171,3172,3173,3173,3174,3175,3175,3176,3177,3177,3178,3179,3179,
                3180,3181,3181,3182,3183,3183,3184,3185,3185,3186,3187,3187,3188,3189,3189,3190,3190,3191,3192,3192,3193,3194,3194,3195,3196,3196,3197,3198,3198,3199,3199,3200,
                3201,3201,3202,3203,3203,3204,3205,3205,3206,3206,3207,3208,3208,3209,3210,3210,3211,3212,3212,3213,3213,3214,3215,3215,3216,3217,3217,3218,3218,3219,3220,3220,
                3221,3222,3222,3223,3223,3224,3225,3225,3226,3227,3227,3228,3228,3229,3230,3230,3231,3231,3232,3233,3233,3234,3234,3235,3236,3236,3237,3238,3238,3239,3239,3240,
                3241,3241,3242,3242,3243,3244,3244,3245,3245,3246,3247,3247,3248,3248,3249,3250,3250,3251,3251,3252,3253,3253,3254,3254,3255,3256,3256,3257,3257,3258,3258,3259,
                3260,3260,3261,3261,3262,3263,3263,3264,3264,3265,3265,3266,3267,3267,3268,3268,3269,3270,3270,3271,3271,3272,3272,3273,3274,3274,3275,3275,3276,3276,3277,3278,
                3278,3279,3279,3280,3280,3281,3282,3282,3283,3283,3284,3284,3285,3285,3286,3287,3287,3288,3288,3289,3289,3290,3291,3291,3292,3292,3293,3293,3294,3294,3295,3296,
                3296,3297,3297,3298,3298,3299,3299,3300,3300,3301,3302,3302,3303,3303,3304,3304,3305,3305,3306,3306,3307,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,3313,
                3313,3314,3315,3315,3316,3316,3317,3317,3318,3318,3319,3319,3320,3320,3321,3321,3322,3322,3323,3324,3324,3325,3325,3326,3326,3327,3327,3328,3328,3329,3329,3330,
                3330,3331,3331,3332,3332,3333,3333,3334,3334,3335,3335,3336,3336,3337,3337,3338,3338,3339,3339,3340,3337,3338,3339,3339,3340,3340,3341,3341,3342,3342,3343,3343,
                3344,3344,3345,3345,3346,3347,3347,3348,3348,3349,3349,3350,3350,3351,3351,3352,3352,3353,3353,3354,3355,3355,3356,3356,3357,3357,3358,3358,3359,3359,3360,3360,
                3361,3361,3362,3362,3363,3364,3364,3365,3365,3366,3366,3367,3367,3368,3368,3369,3369,3370,3371,3371,3372,3372,3373,3373,3374,3374,3375,3375,3376,3376,3377,3377,
                3378,3379,3379,3380,3380,3381,3381,3382,3382,3383,3383,3384,3384,3385,3386,3386,3387,3387,3388,3388,3389,3389,3390,3390,3391,3391,3392,3393,3393,3394,3394,3395,
                3395,3396,3396,3397,3397,3398,3398,3399,3400,3400,3401,3401,3402,3402,3403,3403,3404,3404,3405,3405,3406,3407,3407,3408,3408,3409,3409,3410,3410,3411,3411,3412,
                3412,3413,3414,3414,3415,3415,3416,3416,3417,3417,3418,3418,3419,3420,3420,3421,3421,3422,3422,3423,3423,3424,3424,3425,3425,3426,3427,3427,3428,3428,3429,3429,
                3430,3430,3431,3431,3432,3433,3433,3434,3434,3435,3435,3436,3436,3437,3437,3438,3439,3439,3440,3440,3441,3441,3442,3442,3443,3443,3444,3445,3445,3446,3446,3447,
                3447,3448,3448,3449,3449,3450,3451,3451,3452,3452,3453,3453,3454,3454,3455,3455,3456,3457,3457,3458,3458,3459,3459,3460,3460,3461,3461,3462,3463,3463,3464,3464,
                3465,3465,3466,3466,3467,3468,3468,3469,3469,3470,3470,3471,3471,3472,3472,3473,3474,3474,3475,3475,3476,3476,3477,3477,3478,3479,3479,3480,3480,3481,3481,3482,
                3482,3483,3483,3484,3485,3485,3486,3486,3487,3487,3488,3488,3489,3490,3490,3491,3491,3492,3492,3493,3493,3494,3495,3495,3496,3496,3497,3497,3498,3498,3499,3500,
                3500,3501,3501,3502,3502,3503,3503,3504,3504,3505,3506,3506,3507,3507,3508,3508,3509,3509,3510,3511,3511,3512,3512,3513,3513,3514,3514,3515,3516,3516,3517,3517,
                3518,3518,3519,3519,3520,3521,3521,3522,3522,3523,3523,3524,3525,3525,3526,3526,3527,3527,3528,3528,3529,3530,3530,3531,3531,3532,3532,3533,3533,3534,3535,3535,
                3536,3536,3537,3537,3538,3538,3539,3540,3540,3541,3541,3542,3542,3543,3543,3544,3545,3545,3546,3546,3547,3547,3548,3549,3549,3550,3550,3551,3551,3552,3552,3553,
                3554,3554,3555,3555,3556,3556,3557,3558,3558,3559,3559,3560,3560,3561,3561,3562,3563,3563,3564,3564,3565,3565,3566,3567,3567,3568,3568,3569,3569,3570,3570,3571,
                3572,3572,3573,3573,3574,3574,3575,3576,3576,3577,3577,3578,3578,3579,3579,3580,3581,3581,3582,3582,3583,3583,3584,3585,3585,3586,3586,3587,3587,3588,3589,3589,
                3590,3590,3591,3591,3592,3593,3593,3594,3594,3595,3595,3596,3596,3597,3598,3598,3599,3599,3600,3600,3601,3602,3602,3603,3603,3604,3604,3605,3606,3606,3607,3607,
                3608,3608,3609,3610,3610,3611,3611,3612,3612,3613,3614,3614,3615,3615,3616,3616,3617,3618,3618,3619,3619,3620,3620,3621,3622,3622,3623,3623,3624,3624,3625,3626,
                3626,3627,3627,3628,3628,3629,3630,3630,3631,3631,3632,3632,3633,3634,3634,3635,3635,3636,3636,3637,3638,3638,3639,3639,3640,3640,3641,3642,3642,3643,3643,3644,
                3644,3645,3646,3646,3647,3647,3648,3648,3649,3650,3650,3651,3651,3652,3652,3653,3654,3654,3655,3655,3656,3656,3657,3658,3658,3659,3659,3660,3661,3661,3662,3662,
                3663,3663,3664,3665,3665,3666,3666,3667,3667,3668,3669,3669,3670,3670,3671,3671,3672,3673,3673,3674,3674,3675,3676,3676,3677,3677,3678,3678,3679,3680,3680,3681,
                3681,3682,3682,3683,3684,3684,3685,3685,3686,3687,3687,3688,3688,3689,3689,3690,3691,3691,3692,3692,3693,3693,3694,3695,3695,3696,3696,3697,3698,3698,3699,3699,
                3700,3700,3701,3702,3702,3703,3703,3704,3705,3705,3706,3706,3707,3707,3708,3709,3709,3710,3710,3711,3712,3712,3713,3713,3714,3714,3715,3716,3716,3717,3717,3718,
                3719,3719,3720,3720,3721,3721,3722,3723,3723,3724,3724,3725,3726,3726,3727,3727,3728,3728,3729,3730,3730,3731,3731,3732,3733,3733,3734,3734,3735,3735,3736,3737,
                3737,3738,3738,3739,3740,3740,3741,3741,3742,3743,3743,3744,3744,3745,3745,3746,3747,3747,3748,3748,3749,3750,3750,3751,3751,3752,3753,3753,3754,3754,3755,3755,
                3756,3757,3757,3758,3758,3759,3760,3760,3761,3761,3762,3763,3763,3764,3764,3765,3765,3766,3767,3767,3768,3768,3769,3770,3770,3771,3771,3772,3773,3773,3774,3774,
                3775,3776,3776,3777,3777,3778,3778,3779,3780,3780,3781,3781,3782,3783,3783,3784,3784,3785,3786,3786,3787,3787,3788,3789,3789,3790,3790,3791,3792,3792,3793,3793,
                3794,3795,3795,3796,3796,3797,3797,3798,3799,3799,3800,3800,3801,3802,3802,3803,3803,3804,3805,3805,3806,3806,3807,3808,3808,3809,3809,3810,3811,3811,3812,3812,
                3813,3814,3814,3815,3815,3816,3817,3817,3818,3818,3819,3820,3820,3821,3821,3822,3823,3823,3824,3824,3825,3826,3826,3827,3827,3828,3829,3829,3830,3830,3831,3831,
                3832,3833,3833,3834,3834,3835,3836,3836,3837,3837,3838,3839,3839,3840,3840,3841,3842,3842,3843,3843,3844,3845,3845,3846,3847,3848,3849,3850,3851,3851,3852,3853,
                3854,3855,3856,3856,3857,3858,3859,3860,3861,3861,3862,3863,3864,3865,3866,3867,3867,3868,3869,3870,3871,3872,3872,3873,3874,3875,3876,3877,3878,3878,3879,3880,
                3881,3882,3883,3883,3884,3885,3886,3887,3888,3888,3889,3890,3891,3892,3893,3894,3894,3895,3896,3897,3898,3899,3899,3900,3901,3902,3903,3904,3904,3905,3906,3907,
                3908,3909,3909,3910,3911,3912,3913,3914,3915,3915,3916,3917,3918,3919,3920,3920,3921,3922,3923,3924,3925,3925,3926,3927,3928,3929,3930,3931,3931,3932,3933,3934,
                3935,3936,3936,3937,3938,3939,3940,3941,3941,3942,3943,3944,3945,3946,3946,3947,3948,3949,3950,3951,3951,3952,3953,3954,3955,3956,3957,3957,3958,3959,3960,3961,
                3962,3962,3963,3964,3965,3966,3967,3967,3968,3969,3970,3971,3972,3972,3973,3974,3975,3976,3977,3977,3978,3979,3980,3981,3982,3983,3983,3984,3985,3986,3987,3988,
                3988,3989,3990,3991,3992,3993,3993,3994,3995,3996,3997,3998,3998,3999,4000,4001,4002,4003,4003,4004,4005,4006,4007,4008,4008,4009,4010,4011,4012,4013,4014,4014,
                4015,4016,4017,4018,4019,4019,4020,4021,4022,4023,4024,4024,4025,4026,4027,4028,4029,4029,4030,4031,4032,4033,4034,4034,4035,4036,4037,4038,4039,4039,4040,4041,
                4042,4043,4044,4044,4045,4046,4047,4048,4049,4049,4050,4051,4052,4053,4054,4054,4055,4056,4057,4058,4059,4059,4060,4061,4062,4063,4064,4064,4065,4066,4067,4068,
                4069,4069,4070,4071,4072,4073,4074,4074,4075,4076,4077,4078,4079,4079,4080,4081,4082,4083,4084,4084,4085,4086,4087,4088,4089,4089,4090,4091,4092,4093,4094,4095,
          },
          /* aec trigger 4 */
          {
                   0,   3,   7,   9,  12,  15,  17,  20,  22,  24,  27,  29,  31,  33,  36,  38,  40,  42,  44,  46,  48,  50,  52,  54,  56,  58,  60,  62,  64,  66,  68,  69,
                  71,  73,  75,  77,  79,  81,  82,  84,  86,  88,  90,  91,  93,  95,  97,  98, 100, 102, 104, 105, 107, 109, 111, 112, 114, 116, 117, 119, 121, 122, 124, 126,
                 127, 129, 131, 132, 134, 136, 137, 139, 141, 142, 144, 146, 147, 149, 150, 152, 154, 155, 157, 158, 160, 162, 163, 165, 166, 168, 170, 171, 173, 174, 176, 177,
                 179, 181, 182, 184, 185, 187, 188, 190, 191, 193, 194, 196, 197, 199, 201, 202, 204, 205, 207, 208, 210, 211, 213, 214, 216, 217, 219, 220, 222, 223, 225, 226,
                 228, 229, 231, 232, 233, 235, 236, 238, 239, 241, 242, 244, 245, 247, 248, 250, 251, 253, 254, 255, 257, 258, 260, 261, 263, 264, 266, 267, 268, 270, 271, 273,
                 273, 275, 277, 279, 280, 282, 284, 285, 287, 289, 291, 292, 294, 296, 297, 299, 301, 302, 304, 306, 308, 309, 311, 313, 314, 316, 318, 319, 321, 323, 324, 326,
                 328, 330, 331, 333, 335, 336, 338, 340, 341, 343, 345, 346, 348, 350, 351, 353, 355, 356, 358, 360, 361, 363, 365, 367, 368, 370, 372, 373, 374, 376, 378, 380,
                 382, 384, 386, 388, 390, 392, 394, 396, 398, 400, 401, 403, 405, 407, 409, 411, 413, 415, 417, 419, 421, 423, 425, 427, 429, 431, 433, 434, 436, 438, 440, 442,
                 444, 446, 448, 450, 452, 454, 456, 458, 460, 462, 464, 466, 468, 470, 472, 474, 475, 477, 479, 481, 483, 485, 487, 489, 491, 493, 495, 497, 499, 501, 503, 505,
                 507, 509, 511, 513, 515, 517, 519, 521, 523, 525, 527, 529, 531, 533, 535, 537, 539, 541, 543, 545, 547, 549, 551, 553, 555, 556, 558, 560, 562, 564, 566, 568,
                 570, 572, 574, 576, 578, 580, 582, 584, 586, 588, 590, 592, 594, 596, 598, 600, 602, 604, 606, 609, 611, 613, 615, 617, 619, 621, 623, 625, 627, 629, 631, 633,
                 635, 637, 639, 641, 643, 645, 647, 649, 651, 653, 655, 657, 659, 661, 663, 665, 667, 669, 671, 673, 675, 677, 679, 681, 683, 685, 687, 690, 692, 694, 696, 698,
                 700, 702, 704, 706, 708, 710, 712, 714, 716, 718, 720, 722, 724, 726, 728, 730, 733, 736, 738, 740, 743, 745, 747, 750, 752, 754, 756, 759, 761, 763, 766, 768,
                 770, 772, 775, 777, 779, 781, 784, 786, 788, 790, 793, 795, 797, 799, 801, 804, 806, 808, 810, 812, 815, 817, 819, 821, 823, 826, 828, 830, 832, 834, 837, 839,
                 841, 843, 845, 847, 849, 852, 854, 856, 858, 860, 862, 864, 867, 869, 871, 873, 875, 877, 879, 881, 883, 886, 888, 890, 892, 894, 896, 898, 900, 902, 904, 906,
                 908, 911, 913, 915, 917, 919, 921, 923, 925, 927, 929, 931, 933, 935, 937, 939, 941, 943, 945, 947, 949, 951, 953, 955, 957, 959, 961, 963, 965, 967, 969, 971,
                 973, 975, 977, 979, 981, 983, 985, 987, 989, 991, 993, 995, 998,1000,1002,1005,1007,1009,1011,1014,1016,1018,1021,1023,1025,1027,1030,1032,1034,1036,1038,1041,
                1043,1045,1047,1050,1052,1054,1056,1058,1061,1063,1065,1067,1069,1071,1074,1076,1078,1080,1082,1084,1087,1089,1091,1093,1095,1097,1099,1101,1104,1106,1108,1110,
                1112,1114,1116,1118,1120,1122,1125,1127,1129,1131,1133,1135,1137,1139,1141,1143,1145,1147,1149,1151,1153,1155,1157,1159,1161,1163,1165,1167,1169,1171,1173,1175,
                1177,1179,1181,1183,1185,1187,1189,1191,1193,1195,1197,1199,1201,1203,1205,1206,1208,1210,1212,1214,1216,1218,1220,1222,1224,1225,1227,1229,1231,1233,1235,1237,
                1239,1240,1242,1244,1246,1248,1250,1252,1254,1256,1258,1260,1262,1264,1266,1268,1270,1272,1273,1275,1277,1279,1281,1283,1285,1287,1289,1291,1293,1295,1296,1298,
                1300,1302,1304,1306,1308,1310,1312,1314,1315,1317,1319,1321,1323,1325,1327,1329,1330,1332,1334,1336,1338,1340,1342,1344,1345,1347,1349,1351,1353,1355,1357,1358,
                1360,1362,1364,1366,1368,1369,1371,1373,1375,1377,1379,1380,1382,1384,1386,1388,1390,1391,1393,1395,1397,1399,1400,1402,1404,1406,1408,1409,1411,1413,1415,1417,
                1418,1420,1422,1424,1426,1427,1429,1431,1433,1435,1436,1438,1440,1442,1443,1445,1447,1449,1450,1452,1454,1456,1458,1459,1461,1463,1465,1466,1468,1470,1471,1473,
                1475,1477,1478,1480,1482,1484,1485,1487,1489,1491,1492,1494,1496,1497,1499,1501,1503,1504,1506,1508,1509,1511,1513,1514,1516,1518,1520,1521,1523,1525,1526,1528,
                1530,1531,1533,1535,1536,1538,1540,1541,1543,1545,1546,1548,1550,1551,1553,1555,1556,1558,1560,1561,1563,1565,1566,1568,1570,1571,1573,1575,1576,1578,1579,1581,
                1583,1584,1586,1588,1589,1591,1592,1594,1596,1597,1599,1601,1602,1604,1605,1607,1609,1610,1612,1613,1615,1617,1618,1620,1621,1623,1624,1626,1628,1629,1631,1632,
                1634,1636,1637,1639,1640,1642,1643,1645,1646,1648,1650,1651,1653,1654,1656,1657,1659,1660,1662,1664,1665,1667,1668,1670,1671,1673,1674,1676,1677,1679,1680,1682,
                1683,1685,1686,1688,1690,1691,1692,1694,1695,1696,1698,1699,1701,1702,1703,1705,1706,1707,1709,1710,1711,1713,1714,1716,1717,1718,1720,1721,1722,1724,1725,1726,
                1728,1729,1731,1732,1733,1735,1736,1737,1739,1740,1741,1743,1744,1745,1747,1748,1749,1751,1752,1753,1755,1756,1757,1759,1760,1761,1763,1764,1765,1767,1768,1769,
                1771,1772,1773,1775,1776,1777,1779,1780,1781,1783,1784,1785,1787,1788,1789,1791,1792,1793,1795,1796,1797,1799,1800,1801,1802,1804,1805,1806,1808,1809,1810,1812,
                1813,1814,1815,1817,1818,1819,1821,1822,1823,1825,1826,1827,1828,1830,1831,1832,1834,1835,1836,1837,1839,1840,1841,1843,1844,1845,1846,1848,1849,1850,1852,1853,
                1854,1855,1857,1858,1859,1860,1862,1863,1864,1866,1867,1868,1869,1871,1872,1873,1874,1876,1877,1878,1879,1881,1882,1883,1884,1886,1887,1888,1889,1891,1892,1893,
                1894,1896,1897,1898,1899,1901,1902,1903,1904,1906,1907,1908,1909,1911,1912,1913,1914,1916,1917,1918,1919,1920,1922,1923,1924,1925,1927,1928,1929,1930,1931,1933,
                1934,1935,1936,1938,1939,1940,1941,1942,1944,1945,1946,1947,1948,1950,1951,1952,1953,1955,1956,1957,1958,1959,1961,1962,1963,1964,1965,1967,1968,1969,1970,1971,
                1972,1974,1975,1976,1977,1978,1980,1981,1982,1983,1984,1986,1987,1988,1989,1990,1991,1993,1994,1995,1996,1997,1999,2000,2001,2002,2003,2004,2006,2007,2008,2009,
                2010,2011,2013,2014,2015,2016,2017,2018,2019,2021,2022,2023,2024,2025,2026,2028,2029,2030,2031,2032,2033,2034,2036,2037,2038,2039,2040,2041,2042,2044,2045,2046,
                2047,2048,2049,2050,2052,2053,2054,2055,2056,2057,2058,2059,2061,2062,2063,2064,2065,2066,2067,2068,2070,2071,2072,2073,2074,2075,2076,2077,2079,2080,2081,2082,
                2083,2084,2085,2086,2087,2088,2090,2091,2092,2093,2094,2095,2096,2097,2098,2100,2101,2102,2103,2104,2105,2106,2107,2108,2109,2110,2112,2113,2114,2115,2116,2117,
                2118,2119,2120,2121,2122,2123,2125,2126,2127,2128,2129,2130,2131,2132,2133,2134,2135,2136,2137,2138,2140,2141,2142,2143,2144,2145,2146,2147,2148,2149,2150,2151,
                2152,2153,2154,2155,2156,2157,2159,2160,2161,2162,2163,2164,2165,2166,2167,2168,2169,2170,2171,2172,2173,2174,2175,2176,2177,2178,2179,2180,2181,2182,2183,2185,
                2186,2187,2188,2189,2190,2191,2192,2193,2194,2195,2196,2197,2198,2199,2200,2201,2202,2203,2204,2205,2206,2207,2208,2209,2210,2211,2212,2213,2214,2215,2216,2217,
                2218,2219,2220,2221,2222,2223,2224,2225,2226,2227,2228,2229,2230,2231,2232,2233,2234,2235,2236,2237,2238,2239,2240,2241,2242,2243,2244,2245,2246,2247,2248,2249,
                2250,2250,2251,2252,2253,2254,2255,2256,2257,2258,2259,2260,2261,2262,2263,2264,2265,2266,2267,2268,2269,2270,2271,2272,2273,2274,2275,2275,2276,2277,2278,2279,
                2280,2281,2282,2283,2284,2285,2286,2287,2288,2289,2290,2291,2291,2292,2293,2294,2295,2296,2297,2298,2299,2300,2301,2302,2303,2304,2304,2305,2306,2307,2308,2309,
                2310,2311,2312,2313,2314,2315,2315,2316,2317,2318,2319,2320,2321,2322,2323,2324,2325,2325,2326,2327,2328,2329,2330,2331,2332,2333,2334,2334,2335,2336,2337,2338,
                2339,2340,2341,2342,2342,2343,2344,2345,2346,2347,2348,2349,2350,2350,2351,2352,2353,2354,2355,2356,2357,2357,2358,2359,2360,2361,2362,2363,2363,2364,2365,2366,
                2367,2368,2369,2370,2371,2372,2373,2374,2375,2376,2377,2377,2378,2379,2380,2381,2382,2383,2384,2385,2386,2387,2388,2389,2390,2390,2391,2392,2393,2394,2395,2396,
                2397,2398,2399,2400,2401,2401,2402,2403,2404,2405,2406,2407,2408,2409,2410,2411,2411,2412,2413,2414,2415,2416,2417,2418,2419,2420,2420,2421,2422,2423,2424,2425,
                2426,2427,2428,2429,2429,2430,2431,2432,2433,2434,2435,2436,2437,2438,2438,2439,2440,2441,2442,2443,2444,2445,2446,2446,2447,2448,2449,2450,2451,2452,2453,2453,
                2454,2455,2456,2457,2458,2459,2460,2460,2461,2462,2463,2464,2465,2466,2467,2467,2468,2469,2470,2471,2472,2473,2474,2474,2475,2476,2477,2478,2479,2480,2480,2481,
                2482,2483,2484,2485,2486,2486,2487,2488,2489,2490,2491,2492,2492,2493,2494,2495,2496,2497,2498,2498,2499,2500,2501,2502,2503,2504,2504,2505,2506,2507,2508,2509,
                2509,2510,2511,2512,2513,2514,2515,2515,2516,2517,2518,2519,2520,2520,2521,2522,2523,2524,2525,2525,2526,2527,2528,2529,2530,2530,2531,2532,2533,2534,2534,2535,
                2536,2537,2538,2539,2539,2540,2541,2542,2543,2544,2544,2545,2546,2547,2548,2548,2549,2550,2551,2552,2553,2553,2554,2555,2556,2557,2557,2558,2559,2560,2561,2561,
                2562,2563,2564,2565,2565,2566,2567,2568,2569,2569,2570,2571,2572,2573,2573,2574,2575,2576,2577,2577,2578,2579,2580,2581,2581,2582,2583,2584,2585,2585,2586,2587,
                2588,2589,2589,2590,2591,2592,2592,2593,2594,2595,2596,2596,2597,2598,2599,2599,2600,2601,2602,2603,2603,2604,2605,2606,2606,2607,2608,2609,2610,2610,2611,2612,
                2613,2613,2614,2615,2616,2617,2617,2618,2619,2619,2620,2621,2622,2622,2623,2624,2625,2626,2626,2627,2628,2629,2630,2630,2631,2632,2633,2633,2634,2635,2636,2637,
                2637,2638,2639,2640,2640,2641,2642,2643,2644,2644,2645,2646,2647,2647,2648,2649,2650,2651,2651,2652,2653,2654,2654,2655,2656,2657,2658,2658,2659,2660,2661,2661,
                2662,2663,2664,2664,2665,2666,2667,2668,2668,2669,2670,2671,2671,2672,2673,2674,2674,2675,2676,2677,2678,2678,2679,2680,2681,2681,2682,2683,2684,2684,2685,2686,
                2687,2687,2688,2689,2690,2690,2691,2692,2693,2694,2694,2695,2696,2697,2697,2698,2699,2700,2700,2701,2702,2703,2703,2704,2705,2706,2706,2707,2708,2709,2709,2710,
                2711,2712,2712,2713,2714,2715,2715,2716,2717,2718,2718,2719,2720,2721,2721,2722,2723,2724,2724,2725,2726,2727,2727,2728,2729,2730,2730,2731,2732,2733,2733,2734,
                2735,2735,2736,2737,2738,2738,2739,2740,2741,2741,2742,2743,2744,2744,2745,2746,2747,2747,2748,2749,2750,2750,2751,2752,2752,2753,2754,2755,2755,2756,2757,2758,
                2758,2759,2760,2760,2761,2762,2763,2763,2764,2765,2766,2766,2767,2768,2768,2769,2770,2771,2771,2772,2773,2774,2774,2775,2776,2776,2777,2778,2779,2779,2780,2781,
                2782,2782,2783,2784,2784,2785,2786,2787,2787,2788,2789,2789,2790,2791,2792,2792,2793,2794,2794,2795,2796,2797,2797,2798,2799,2799,2800,2801,2802,2802,2803,2804,
                2804,2805,2806,2807,2807,2808,2809,2809,2810,2811,2812,2812,2813,2814,2814,2815,2816,2816,2817,2818,2819,2819,2820,2821,2821,2822,2823,2823,2824,2825,2826,2826,
                2827,2828,2828,2829,2830,2831,2831,2832,2833,2833,2834,2835,2835,2836,2837,2837,2838,2839,2840,2840,2841,2842,2842,2843,2844,2844,2845,2846,2847,2847,2848,2849,
                2849,2850,2851,2851,2852,2853,2853,2854,2855,2855,2856,2857,2858,2858,2859,2860,2860,2861,2862,2862,2863,2864,2864,2865,2866,2866,2867,2868,2868,2869,2870,2871,
                2871,2872,2873,2873,2874,2875,2875,2876,2877,2877,2878,2879,2879,2880,2881,2881,2882,2883,2883,2884,2885,2885,2886,2887,2887,2888,2889,2889,2890,2891,2891,2892,
                2893,2893,2894,2895,2895,2896,2897,2897,2898,2899,2900,2900,2901,2902,2902,2903,2904,2904,2905,2905,2906,2907,2907,2908,2909,2909,2910,2911,2911,2912,2913,2913,
                2914,2915,2915,2916,2917,2917,2918,2919,2919,2920,2921,2921,2922,2923,2923,2924,2925,2925,2926,2927,2927,2928,2929,2929,2930,2931,2931,2932,2932,2933,2934,2934,
                2935,2936,2936,2937,2938,2938,2939,2940,2940,2941,2942,2942,2943,2944,2944,2945,2945,2946,2947,2947,2948,2949,2949,2950,2951,2951,2952,2953,2953,2954,2954,2955,
                2956,2956,2957,2958,2958,2959,2960,2960,2961,2961,2962,2963,2963,2964,2965,2965,2966,2967,2967,2968,2968,2969,2970,2970,2971,2972,2972,2973,2974,2974,2975,2975,
                2976,2977,2977,2978,2979,2979,2980,2980,2981,2982,2982,2983,2984,2984,2985,2985,2986,2987,2987,2988,2989,2989,2990,2990,2991,2992,2992,2993,2994,2994,2995,2995,
                2996,2997,2997,2998,2999,2999,3000,3000,3001,3002,3002,3003,3003,3004,3005,3005,3006,3007,3007,3008,3008,3009,3010,3010,3011,3011,3012,3013,3013,3014,3015,3015,
                3016,3016,3017,3018,3018,3019,3019,3020,3021,3021,3022,3022,3023,3024,3024,3025,3025,3026,3027,3027,3028,3028,3029,3030,3030,3031,3031,3032,3033,3033,3034,3034,
                3035,3036,3036,3037,3037,3038,3039,3039,3040,3040,3041,3042,3042,3043,3043,3044,3045,3045,3046,3046,3047,3048,3048,3049,3049,3050,3051,3051,3052,3052,3053,3054,
                3054,3055,3055,3056,3056,3057,3058,3058,3059,3059,3060,3061,3061,3062,3062,3063,3064,3064,3065,3065,3066,3066,3067,3068,3068,3069,3069,3070,3071,3071,3072,3072,
                3073,3073,3074,3075,3075,3076,3076,3077,3077,3078,3079,3079,3080,3080,3081,3081,3082,3083,3083,3084,3084,3085,3086,3086,3087,3087,3088,3088,3089,3090,3090,3091,
                3091,3092,3092,3093,3094,3095,3096,3096,3097,3098,3099,3099,3100,3101,3102,3102,3103,3104,3104,3105,3106,3107,3107,3108,3109,3109,3110,3111,3112,3112,3113,3114,
                3114,3115,3116,3117,3117,3118,3119,3119,3120,3121,3121,3122,3123,3124,3124,3125,3126,3126,3127,3128,3128,3129,3130,3131,3131,3132,3133,3133,3134,3135,3135,3136,
                3137,3138,3138,3139,3140,3140,3141,3142,3142,3143,3144,3144,3145,3146,3146,3147,3148,3149,3149,3150,3151,3151,3152,3153,3153,3154,3155,3155,3156,3157,3157,3158,
                3159,3159,3160,3161,3161,3162,3163,3163,3164,3165,3165,3166,3167,3167,3168,3169,3169,3170,3171,3171,3172,3173,3173,3174,3175,3175,3176,3177,3177,3178,3179,3179,
                3180,3181,3181,3182,3183,3183,3184,3185,3185,3186,3187,3187,3188,3189,3189,3190,3190,3191,3192,3192,3193,3194,3194,3195,3196,3196,3197,3198,3198,3199,3199,3200,
                3201,3201,3202,3203,3203,3204,3205,3205,3206,3206,3207,3208,3208,3209,3210,3210,3211,3212,3212,3213,3213,3214,3215,3215,3216,3217,3217,3218,3218,3219,3220,3220,
                3221,3222,3222,3223,3223,3224,3225,3225,3226,3227,3227,3228,3228,3229,3230,3230,3231,3231,3232,3233,3233,3234,3234,3235,3236,3236,3237,3238,3238,3239,3239,3240,
                3241,3241,3242,3242,3243,3244,3244,3245,3245,3246,3247,3247,3248,3248,3249,3250,3250,3251,3251,3252,3253,3253,3254,3254,3255,3256,3256,3257,3257,3258,3258,3259,
                3260,3260,3261,3261,3262,3263,3263,3264,3264,3265,3265,3266,3267,3267,3268,3268,3269,3270,3270,3271,3271,3272,3272,3273,3274,3274,3275,3275,3276,3276,3277,3278,
                3278,3279,3279,3280,3280,3281,3282,3282,3283,3283,3284,3284,3285,3285,3286,3287,3287,3288,3288,3289,3289,3290,3291,3291,3292,3292,3293,3293,3294,3294,3295,3296,
                3296,3297,3297,3298,3298,3299,3299,3300,3300,3301,3302,3302,3303,3303,3304,3304,3305,3305,3306,3306,3307,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,3313,
                3313,3314,3315,3315,3316,3316,3317,3317,3318,3318,3319,3319,3320,3320,3321,3321,3322,3322,3323,3324,3324,3325,3325,3326,3326,3327,3327,3328,3328,3329,3329,3330,
                3330,3331,3331,3332,3332,3333,3333,3334,3334,3335,3335,3336,3336,3337,3337,3338,3338,3339,3339,3340,3337,3338,3339,3339,3340,3340,3341,3341,3342,3342,3343,3343,
                3344,3344,3345,3345,3346,3347,3347,3348,3348,3349,3349,3350,3350,3351,3351,3352,3352,3353,3353,3354,3355,3355,3356,3356,3357,3357,3358,3358,3359,3359,3360,3360,
                3361,3361,3362,3362,3363,3364,3364,3365,3365,3366,3366,3367,3367,3368,3368,3369,3369,3370,3371,3371,3372,3372,3373,3373,3374,3374,3375,3375,3376,3376,3377,3377,
                3378,3379,3379,3380,3380,3381,3381,3382,3382,3383,3383,3384,3384,3385,3386,3386,3387,3387,3388,3388,3389,3389,3390,3390,3391,3391,3392,3393,3393,3394,3394,3395,
                3395,3396,3396,3397,3397,3398,3398,3399,3400,3400,3401,3401,3402,3402,3403,3403,3404,3404,3405,3405,3406,3407,3407,3408,3408,3409,3409,3410,3410,3411,3411,3412,
                3412,3413,3414,3414,3415,3415,3416,3416,3417,3417,3418,3418,3419,3420,3420,3421,3421,3422,3422,3423,3423,3424,3424,3425,3425,3426,3427,3427,3428,3428,3429,3429,
                3430,3430,3431,3431,3432,3433,3433,3434,3434,3435,3435,3436,3436,3437,3437,3438,3439,3439,3440,3440,3441,3441,3442,3442,3443,3443,3444,3445,3445,3446,3446,3447,
                3447,3448,3448,3449,3449,3450,3451,3451,3452,3452,3453,3453,3454,3454,3455,3455,3456,3457,3457,3458,3458,3459,3459,3460,3460,3461,3461,3462,3463,3463,3464,3464,
                3465,3465,3466,3466,3467,3468,3468,3469,3469,3470,3470,3471,3471,3472,3472,3473,3474,3474,3475,3475,3476,3476,3477,3477,3478,3479,3479,3480,3480,3481,3481,3482,
                3482,3483,3483,3484,3485,3485,3486,3486,3487,3487,3488,3488,3489,3490,3490,3491,3491,3492,3492,3493,3493,3494,3495,3495,3496,3496,3497,3497,3498,3498,3499,3500,
                3500,3501,3501,3502,3502,3503,3503,3504,3504,3505,3506,3506,3507,3507,3508,3508,3509,3509,3510,3511,3511,3512,3512,3513,3513,3514,3514,3515,3516,3516,3517,3517,
                3518,3518,3519,3519,3520,3521,3521,3522,3522,3523,3523,3524,3525,3525,3526,3526,3527,3527,3528,3528,3529,3530,3530,3531,3531,3532,3532,3533,3533,3534,3535,3535,
                3536,3536,3537,3537,3538,3538,3539,3540,3540,3541,3541,3542,3542,3543,3543,3544,3545,3545,3546,3546,3547,3547,3548,3549,3549,3550,3550,3551,3551,3552,3552,3553,
                3554,3554,3555,3555,3556,3556,3557,3558,3558,3559,3559,3560,3560,3561,3561,3562,3563,3563,3564,3564,3565,3565,3566,3567,3567,3568,3568,3569,3569,3570,3570,3571,
                3572,3572,3573,3573,3574,3574,3575,3576,3576,3577,3577,3578,3578,3579,3579,3580,3581,3581,3582,3582,3583,3583,3584,3585,3585,3586,3586,3587,3587,3588,3589,3589,
                3590,3590,3591,3591,3592,3593,3593,3594,3594,3595,3595,3596,3596,3597,3598,3598,3599,3599,3600,3600,3601,3602,3602,3603,3603,3604,3604,3605,3606,3606,3607,3607,
                3608,3608,3609,3610,3610,3611,3611,3612,3612,3613,3614,3614,3615,3615,3616,3616,3617,3618,3618,3619,3619,3620,3620,3621,3622,3622,3623,3623,3624,3624,3625,3626,
                3626,3627,3627,3628,3628,3629,3630,3630,3631,3631,3632,3632,3633,3634,3634,3635,3635,3636,3636,3637,3638,3638,3639,3639,3640,3640,3641,3642,3642,3643,3643,3644,
                3644,3645,3646,3646,3647,3647,3648,3648,3649,3650,3650,3651,3651,3652,3652,3653,3654,3654,3655,3655,3656,3656,3657,3658,3658,3659,3659,3660,3661,3661,3662,3662,
                3663,3663,3664,3665,3665,3666,3666,3667,3667,3668,3669,3669,3670,3670,3671,3671,3672,3673,3673,3674,3674,3675,3676,3676,3677,3677,3678,3678,3679,3680,3680,3681,
                3681,3682,3682,3683,3684,3684,3685,3685,3686,3687,3687,3688,3688,3689,3689,3690,3691,3691,3692,3692,3693,3693,3694,3695,3695,3696,3696,3697,3698,3698,3699,3699,
                3700,3700,3701,3702,3702,3703,3703,3704,3705,3705,3706,3706,3707,3707,3708,3709,3709,3710,3710,3711,3712,3712,3713,3713,3714,3714,3715,3716,3716,3717,3717,3718,
                3719,3719,3720,3720,3721,3721,3722,3723,3723,3724,3724,3725,3726,3726,3727,3727,3728,3728,3729,3730,3730,3731,3731,3732,3733,3733,3734,3734,3735,3735,3736,3737,
                3737,3738,3738,3739,3740,3740,3741,3741,3742,3743,3743,3744,3744,3745,3745,3746,3747,3747,3748,3748,3749,3750,3750,3751,3751,3752,3753,3753,3754,3754,3755,3755,
                3756,3757,3757,3758,3758,3759,3760,3760,3761,3761,3762,3763,3763,3764,3764,3765,3765,3766,3767,3767,3768,3768,3769,3770,3770,3771,3771,3772,3773,3773,3774,3774,
                3775,3776,3776,3777,3777,3778,3778,3779,3780,3780,3781,3781,3782,3783,3783,3784,3784,3785,3786,3786,3787,3787,3788,3789,3789,3790,3790,3791,3792,3792,3793,3793,
                3794,3795,3795,3796,3796,3797,3797,3798,3799,3799,3800,3800,3801,3802,3802,3803,3803,3804,3805,3805,3806,3806,3807,3808,3808,3809,3809,3810,3811,3811,3812,3812,
                3813,3814,3814,3815,3815,3816,3817,3817,3818,3818,3819,3820,3820,3821,3821,3822,3823,3823,3824,3824,3825,3826,3826,3827,3827,3828,3829,3829,3830,3830,3831,3831,
                3832,3833,3833,3834,3834,3835,3836,3836,3837,3837,3838,3839,3839,3840,3840,3841,3842,3842,3843,3843,3844,3845,3845,3846,3847,3848,3849,3850,3851,3851,3852,3853,
                3854,3855,3856,3856,3857,3858,3859,3860,3861,3861,3862,3863,3864,3865,3866,3867,3867,3868,3869,3870,3871,3872,3872,3873,3874,3875,3876,3877,3878,3878,3879,3880,
                3881,3882,3883,3883,3884,3885,3886,3887,3888,3888,3889,3890,3891,3892,3893,3894,3894,3895,3896,3897,3898,3899,3899,3900,3901,3902,3903,3904,3904,3905,3906,3907,
                3908,3909,3909,3910,3911,3912,3913,3914,3915,3915,3916,3917,3918,3919,3920,3920,3921,3922,3923,3924,3925,3925,3926,3927,3928,3929,3930,3931,3931,3932,3933,3934,
                3935,3936,3936,3937,3938,3939,3940,3941,3941,3942,3943,3944,3945,3946,3946,3947,3948,3949,3950,3951,3951,3952,3953,3954,3955,3956,3957,3957,3958,3959,3960,3961,
                3962,3962,3963,3964,3965,3966,3967,3967,3968,3969,3970,3971,3972,3972,3973,3974,3975,3976,3977,3977,3978,3979,3980,3981,3982,3983,3983,3984,3985,3986,3987,3988,
                3988,3989,3990,3991,3992,3993,3993,3994,3995,3996,3997,3998,3998,3999,4000,4001,4002,4003,4003,4004,4005,4006,4007,4008,4008,4009,4010,4011,4012,4013,4014,4014,
                4015,4016,4017,4018,4019,4019,4020,4021,4022,4023,4024,4024,4025,4026,4027,4028,4029,4029,4030,4031,4032,4033,4034,4034,4035,4036,4037,4038,4039,4039,4040,4041,
                4042,4043,4044,4044,4045,4046,4047,4048,4049,4049,4050,4051,4052,4053,4054,4054,4055,4056,4057,4058,4059,4059,4060,4061,4062,4063,4064,4064,4065,4066,4067,4068,
                4069,4069,4070,4071,4072,4073,4074,4074,4075,4076,4077,4078,4079,4079,4080,4081,4082,4083,4084,4084,4085,4086,4087,4088,4089,4089,4090,4091,4092,4093,4094,4095,
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
             160,
          },
          /* aec trigger 1 */
          {
             200,
             260,
          },
          /* aec trigger 2 */
          {
             300,
             330,
          },
          /* aec trigger 3 */
          {
             340,
             410,
          },
          /* aec trigger 4 */
          {
             420,
             500,
          },
       },
       /* isp_sub_module_gtm2_lut_pra pra[MAX_TRIGGER_COUNT] */
       {
          /* para 0 */
          {
             1,//enable_cdf_smooth
             0.0055,//cut_ration
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
             0,//ltm_dark_fork
             0,//ltm_dark_spear
             0,//ltm_smooth_noise_erode_area
             0,//ltm_smooth_filter_area
             0,//soft_sample_rate
          },
          /* para 1 */
          {
             1,//enable_cdf_smooth
             0.0025,//cut_ration
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
             0,//ltm_dark_fork
             0,//ltm_dark_spear
             0,//ltm_smooth_noise_erode_area
             0,//ltm_smooth_filter_area
             0,//soft_sample_rate
          },
          /* para 2 */
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
             0,//ltm_dark_fork
             0,//ltm_dark_spear
             0,//ltm_smooth_noise_erode_area
             0,//ltm_smooth_filter_area
             0,//soft_sample_rate
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
             0,//ltm_dark_fork
             0,//ltm_dark_spear
             0,//ltm_smooth_noise_erode_area
             0,//ltm_smooth_filter_area
             0,//soft_sample_rate
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
             0,//ltm_dark_fork
             0,//ltm_dark_spear
             0,//ltm_smooth_noise_erode_area
             0,//ltm_smooth_filter_area
             0,//soft_sample_rate
          },
       },
       1,//enable_tuning_mesh
       16,//mesh_w
       16,//mesh_h
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
             120,
          },
          /* aec trigger 1 */
          {
             160,
             300,
          },
          /* aec trigger 2 */
          {
             320,
             330,
          },
          /* aec trigger 3 */
          {
             340,
             400,
          },
          /* aec trigger 4 */
          {
             440,
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
             4400,
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
                1.1,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                1.2,//saturation
                0,//hue
             },
             /* awb para 4 */
             {
                1.2,//saturation
                0,//hue
             },
             /* awb para 5 */
             {
                1.2,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                1,//saturation
                0,//hue
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                0.8,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                0.9,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                1,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                1.2,//saturation
                0,//hue
             },
             /* awb para 4 */
             {
                1.1,//saturation
                0,//hue
             },
             /* awb para 5 */
             {
                1,//saturation
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
                0.9,//saturation
                0,//hue
             },
             /* awb para 2 */
             {
                1,//saturation
                0,//hue
             },
             /* awb para 3 */
             {
                1.2,//saturation
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
                0.9,//saturation
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
                0.7,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                0.7,//saturation
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
                0.7,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                0.7,//saturation
                0,//hue
             },
          },
       },
    },
    /* isp_sub_module_lee_tuning_t isp_sub_module_lee_tuning */
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
                -1,-3,-7,-8,//bpf(1,1:4)
                -3,-14,-40,-58,//bpf(2,1:4)
                -7,-40,-173,-312,//bpf(3,1:4)
                -8,-58,-312,2664,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             130,//strength_pos_edge
             150,//strength_neg_edge
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
             {0,4,8,16,24,32,32,32,32,36,36,36,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             60,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,6,10,18,26,34,42,50,58,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
                -1,-3,-7,-8,//bpf(1,1:4)
                -3,-14,-40,-58,//bpf(2,1:4)
                -7,-40,-173,-312,//bpf(3,1:4)
                -8,-58,-312,2664,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             125,//strength_pos_edge
             125,//strength_neg_edge
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
             {0,4,8,16,24,32,40,40,40,40,40,40,40,40,40,40,40,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             70,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {8,16,24,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,4,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
                -1,-3,-7,-8,//bpf(1,1:4)
                -3,-14,-40,-58,//bpf(2,1:4)
                -7,-40,-173,-312,//bpf(3,1:4)
                -8,-58,-312,2664,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             115,//strength_pos_edge
             115,//strength_neg_edge
             200,//overshoot_pos_edge
             -205,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,4,8,16,24,32,40,48,48,48,48,48,48,48,48,48,48,48,48,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             70,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {8,16,24,32,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,4,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
                -1,-3,-7,-8,//bpf(1,1:4)
                -3,-14,-40,-58,//bpf(2,1:4)
                -7,-40,-173,-312,//bpf(3,1:4)
                -8,-58,-312,2664,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             110,//strength_pos_edge
             110,//strength_neg_edge
             200,//overshoot_pos_edge
             -205,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,4,8,16,24,32,40,40,40,40,40,40,40,40,40,40,40,40,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             90,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {8,16,24,32,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,4,8,16,24,32,32,32,32,32,32,32,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
                -1,-3,-7,-8,//bpf(1,1:4)
                -3,-14,-40,-58,//bpf(2,1:4)
                -7,-40,-173,-312,//bpf(3,1:4)
                -8,-58,-312,2664,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             110,//strength_pos_edge
             115,//strength_neg_edge
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
             {0,0,4,8,16,24,32,40,40,40,40,40,40,40,44,48,48,48,48,48,48,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             100,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {8,16,24,32,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,4,8,16,24,32,32,32,32,32,32,32,32,32,32,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
                -1,-3,-7,-8,//bpf(1,1:4)
                -3,-14,-40,-58,//bpf(2,1:4)
                -7,-40,-173,-312,//bpf(3,1:4)
                -8,-58,-312,2664,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             90,//strength_pos_edge
             90,//strength_neg_edge
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
             {0,2,4,8,16,24,32,32,32,32,32,32,32,32,32,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             135,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {0,4,8,12,16,20,24,28,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,4,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
          },
          /* para 6 */
          {
             1,//enable_noise_level_weight_edge
             0,//enable_skin_detection
             0,//shrink enable
             1,//enable_luma_weight_function
             0,//enable_mono_mode
             0,//skin_weight
             {
                -1,-3,-7,-8,//bpf(1,1:4)
                -3,-14,-40,-58,//bpf(2,1:4)
                -7,-40,-173,-312,//bpf(3,1:4)
                -8,-58,-312,2664,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             130,//strength_pos_edge
             130,//strength_neg_edge
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
             {16,16,16,16,16,16,16,16,16,16,20,24,28,32,32,32,32,32,32,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             205,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,4,8,16,24,32,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
                -2,-8,-16,-19,//bpf(1,1:4)
                -8,-32,-66,-84,//bpf(2,1:4)
                -16,-66,-62,-84,//bpf(3,1:4)
                -19,-84,-84,1852,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             90,//strength_pos_edge
             90,//strength_neg_edge
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
             {0,0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,32,32,32,32,32,32,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             255,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {0,4,8,12,16,20,24,28,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,0,0,0,2,4,6,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             30,//strength_pos_edge
             30,//strength_neg_edge
             100,//overshoot_pos_edge
             -100,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,32,32,32,32,32,32,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             255,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {0,4,8,12,16,20,24,28,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,0,0,0,2,4,6,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             30,//strength_pos_edge
             30,//strength_neg_edge
             100,//overshoot_pos_edge
             -100,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,28,28,28,28,28,28,28,28,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             250,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,0,0,0,2,4,6,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             0,//strength_pos_edge
             0,//strength_neg_edge
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
             {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,4,6,8,10,12,16,20,24,28,28,28,28,28,28,28,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             255,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
          /* para 2 */
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
          /* para 3 */
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
          /* para 4 */
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
          /* para 7 */
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
          /* para 8 */
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
          /* para 9 */
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
          /* para 10 */
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
       },
    },
    /* isp_sub_module_3d_denoise_tuning_t isp_sub_module_3d_denoise_tuning */
    {
       1,//enable
       1,//interpolation_enable
       12,//count_ae
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
             62,
             62.1,
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
          /* aec trigger 11 */
          {
             1500,
             1500,
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
             10,//nr3d_mpy_th1
             15,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             10,//nr3d_mpc_th1
             15,//nr3d_mpc_th2
             3,//nr3d_lamda2d
             5,//nr3d_ite
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
             12,//nr3d_mpy_th1
             17,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             10,//nr3d_mpc_th1
             20,//nr3d_mpc_th2
             5,//nr3d_lamda2d
             5,//nr3d_ite
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
             0,//disable_1st_media_filter
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
             15,//nr3d_mpy_th1
             20,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             20,//nr3d_mpc_th1
             30,//nr3d_mpc_th2
             7,//nr3d_lamda2d
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
             0,//disable_1st_media_filter
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
             10,//nr3d_mpy_th1
             20,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             20,//nr3d_mpc_th1
             30,//nr3d_mpc_th2
             10,//nr3d_lamda2d
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
             20,//nr3d_mpy_th1
             30,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             90,//nr3d_npc_th3
             180,//nr3d_npc_th4
             20,//nr3d_mpc_th1
             30,//nr3d_mpc_th2
             20,//nr3d_lamda2d
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
             30,//dbk_h_mv_str
             20,//dbk_v_mv_diff
             30,//dbk_v_mv_str
             85,//dbk_h_yy_flat
             33,//dbk_h_yy_diff
             85,//dbk_v_yy_flat
             33,//dbk_v_yy_diff
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
             20,//nr3d_mpy_th1
             30,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             30,//nr3d_mpc_th1
             40,//nr3d_mpc_th2
             25,//nr3d_lamda2d
             13,//nr3d_ite
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
                1,1,2,2,3,4,5,6,7,8,8,8,8,9,10,10,10,12,12,12,14,14,14,16,16,16,16,18,19,19,19,20,20,21,21,21,22,22,22,23,23,23,24,24,25,25,25,26,26,26,27,27,28,28,28,29,29,29,30,30,30,31,31,31,
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
             20,//nr3d_mpy_th1
             30,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             30,//nr3d_mpc_th1
             40,//nr3d_mpc_th2
             50,//nr3d_lamda2d
             15,//nr3d_ite
             0,//nr3d_msr
             240,//nr3d_decay
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
             1,//disable_1st_media_filter
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
                2,2,4,4,4,6,6,8,8,8,8,8,8,8,8,8,10,10,12,12,14,14,14,14,16,16,18,18,20,20,20,22,22,24,24,24,26,26,26,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,29,29,29,30,30,30,31,31,31,
             },
             /* noise profile cbcr*/
             {
                0,1,1,2,2,2,2,2,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
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
             45,//nr3d_mpy_th1
             70,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             80,//nr3d_npc_th3
             180,//nr3d_npc_th4
             40,//nr3d_mpc_th1
             60,//nr3d_mpc_th2
             30,//nr3d_lamda2d
             10,//nr3d_ite
             0,//nr3d_msr
             240,//nr3d_decay
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
             1,//disable_1st_media_filter
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
             120,//dbk_satu
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
                0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,8,8,8,8,9,10,11,12,13,14,15,16,16,16,16,16,16,16,16,17,18,19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
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
             60,//nr3d_mpy_th1
             130,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             70,//nr3d_mpc_th1
             120,//nr3d_mpc_th2
             30,//nr3d_lamda2d
             30,//nr3d_ite
             0,//nr3d_msr
             220,//nr3d_decay
             50,//nr3d_satu
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
             60,//dbk_h_mv_diff
             90,//dbk_h_mv_str
             60,//dbk_v_mv_diff
             90,//dbk_v_mv_str
             100,//dbk_h_yy_flat
             80,//dbk_h_yy_diff
             100,//dbk_v_yy_flat
             80,//dbk_v_yy_diff
             120,//dbk_satu
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
                0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,4,4,4,5,6,7,8,8,8,8,9,10,11,12,12,12,13,14,15,16,16,16,17,18,19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,0,0,0,2,4,6,8,10,12,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
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
             60,//nr3d_mpy_th1
             150,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             70,//nr3d_mpc_th1
             150,//nr3d_mpc_th2
             40,//nr3d_lamda2d
             35,//nr3d_ite
             0,//nr3d_msr
             220,//nr3d_decay
             50,//nr3d_satu
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
             60,//dbk_h_mv_diff
             90,//dbk_h_mv_str
             60,//dbk_v_mv_diff
             90,//dbk_v_mv_str
             100,//dbk_h_yy_flat
             85,//dbk_h_yy_diff
             100,//dbk_v_yy_flat
             85,//dbk_v_yy_diff
             120,//dbk_satu
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
                0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,4,4,4,4,4,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16,16,16,16,17,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,0,0,1,4,6,8,8,8,8,10,12,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
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
             60,//nr3d_mpy_th1
             155,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             70,//nr3d_mpc_th1
             150,//nr3d_mpc_th2
             40,//nr3d_lamda2d
             40,//nr3d_ite
             0,//nr3d_msr
             220,//nr3d_decay
             50,//nr3d_satu
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
             60,//dbk_h_mv_diff
             90,//dbk_h_mv_str
             60,//dbk_v_mv_diff
             90,//dbk_v_mv_str
             100,//dbk_h_yy_flat
             90,//dbk_h_yy_diff
             100,//dbk_v_yy_flat
             90,//dbk_v_yy_diff
             120,//dbk_satu
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
                0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,3,3,3,3,3,4,4,4,4,5,6,7,8,9,10,11,12,13,14,15,15,15,15,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,1,2,4,4,4,4,4,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             1,//dbk_h_ed_en
             1,//dbk_v_ed_en
             0,//dbk_h_ed_sel
             0,//dbk_v_ed_sel
             28,//dbk_h_ed_thrd
             28,//dbk_v_ed_thrd
          },
          /* para 11 */
          {
             1,//enable_3d_noise
             1,//nr3d_pp_en
             1,//nr3d_md_en
             100,//nr3d_npy_th1
             180,//nr3d_npy_th2
             70,//nr3d_mpy_th1
             170,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             80,//nr3d_mpc_th1
             180,//nr3d_mpc_th2
             200,//nr3d_lamda2d
             50,//nr3d_ite
             0,//nr3d_msr
             230,//nr3d_decay
             50,//nr3d_satu
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
             60,//dbk_h_mv_diff
             90,//dbk_h_mv_str
             60,//dbk_v_mv_diff
             90,//dbk_v_mv_str
             100,//dbk_h_yy_flat
             95,//dbk_h_yy_diff
             100,//dbk_v_yy_flat
             95,//dbk_v_yy_diff
             120,//dbk_satu
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
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,2,2,2,3,4,4,4,4,5,6,6,6,7,8,8,8,8,9,10,11,12,13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
             },
             /* noise profile cbcr*/
             {
                0,1,2,4,4,4,4,4,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
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
    /* isp_sub_module_isp_vfe_tuning_t isp_sub_module_isp_vfe_tuning */
    {
       0,//enable
       1,//interpolation_enable
       1,//count_ae
       0,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             0,
          },
       },
       /* isp_sub_module_isp_vfe_tuning pra[MAX_TRIGGER_COUNT] */
       {
          /* para 0 */
          {
             /* isp_sub_module_hdr_mix_pra */
             {
                0,//blc_short_exp
                0,//blc_mid_exp
                0,//blc_long_exp
                0,//hdr_motion_en_sm
                0,//hdr_alpha_s_th_sm
                0,//hdr_exp_value_th1_sm
                0,//hdr_th2_th1_reverse_sm
                0,//hdr_noise_sp_sm
                0,//hdr_motion_mp_sm
                0,//hdr_motion_en_sl
                0,//hdr_alpha_s_th_sl
                0,//hdr_exp_value_th1_sl
                0,//hdr_th2_th1_reverse_sl
                0,//hdr_noise_sp_sl
                0,//hdr_motion_mp_sl
                0,//hdr_md_th4_sl
                0,//hdr_md_th9_sl
                0,//hdr_md_th16_sl
                0,//hdr_md_th25_sl
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
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
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
             440,
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
                1,//saturation
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
                1,//saturation
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
                0.9,//saturation
                0,//hue
                1,//y_lo_th1
                2,//y_lo_th2
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
                1,//y_lo_th1
                2,//y_lo_th2
                1000,//y_hi_th1
                1023,//y_hi_th2
             },
          },
       },
    },
    /* isp_sub_module_lsc_pra isp_sub_module_hdr_lsc_tuning */
    {
       0,//enable
       1,//interpolation_enable
       1,//count_ae
       1,//count_awb
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             500,
          },
       },
       /* awb trigger */
       {
          /* awb trigger 0 */
          {
             2700,
             2800,
          },
       },
       /* isp_sub_module_lsc_pra */
       {
          /* aec trigger 0 */
          {
             /* awb para 0 */
             {
                1,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   1.4644, 1.2524, 1.2012, 1.1411, 1.0928, 1.1147, 1.1221, 1.2031, 1.2856, 1.4863, 
                   1.3696, 1.2051, 1.1255, 1.0728, 1.0547, 1.0400, 1.0869, 1.1045, 1.2344, 1.3667, 
                   1.3027, 1.1553, 1.1006, 1.0513, 1.0151, 1.0322, 1.0352, 1.0923, 1.1572, 1.3481, 
                   1.2388, 1.1377, 1.0664, 1.0396, 1.0112, 1.0049, 1.0308, 1.0698, 1.1411, 1.2573, 
                   1.2427, 1.1035, 1.0903, 1.0132, 1.0137, 0.9937, 1.0181, 1.0527, 1.1226, 1.2729, 
                   1.2144, 1.1133, 1.0371, 1.0234, 1.0054, 1.0093, 1.0103, 1.0479, 1.1069, 1.2534, 
                   1.2017, 1.1289, 1.0547, 1.0386, 1.0078, 0.9922, 1.0303, 1.0366, 1.1406, 1.2334, 
                   1.2354, 1.1182, 1.0771, 1.0322, 1.0229, 1.0015, 1.0308, 1.0659, 1.1440, 1.3252, 
                   1.3042, 1.1714, 1.0908, 1.0801, 1.0474, 1.0396, 1.0576, 1.0864, 1.1738, 1.2886, 
                   1.3428, 1.1982, 1.1563, 1.0889, 1.0752, 1.0737, 1.0898, 1.1440, 1.2446, 1.4448, 
                   1.3716, 1.2295, 1.1821, 1.1050, 1.1123, 1.0786, 1.1299, 1.1772, 1.2520, 1.4707, 
                   1.3418, 1.1904, 1.1187, 1.0630, 1.0410, 1.0361, 1.0732, 1.1152, 1.2114, 1.3721, 
                   1.2754, 1.1396, 1.1001, 1.0361, 1.0371, 1.0269, 1.0347, 1.0894, 1.1333, 1.3325, 
                   1.2358, 1.1099, 1.0703, 1.0366, 1.0005, 1.0029, 1.0234, 1.0630, 1.1353, 1.2612, 
                   1.1938, 1.1226, 1.0752, 1.0205, 1.0103, 1.0005, 1.0171, 1.0532, 1.1030, 1.2466, 
                   1.1846, 1.0776, 1.0527, 1.0005, 1.0063, 0.9941, 1.0010, 1.0474, 1.0977, 1.2266, 
                   1.2065, 1.1162, 1.0532, 1.0381, 0.9985, 0.9902, 1.0225, 1.0342, 1.1133, 1.2227, 
                   1.2153, 1.1157, 1.0649, 1.0225, 1.0195, 0.9951, 1.0181, 1.0508, 1.1274, 1.2520, 
                   1.2549, 1.1431, 1.0898, 1.0508, 1.0513, 1.0210, 1.0508, 1.0786, 1.1597, 1.2803, 
                   1.2939, 1.2119, 1.1128, 1.0991, 1.0601, 1.0566, 1.0933, 1.1011, 1.2129, 1.3354, 
                   1.4399, 1.2388, 1.1729, 1.1143, 1.0928, 1.0757, 1.1035, 1.1426, 1.2539, 1.4194, 
                   1.3516, 1.1636, 1.1255, 1.0518, 1.0415, 1.0356, 1.0601, 1.1167, 1.1875, 1.3750, 
                   1.2866, 1.1323, 1.0938, 1.0439, 1.0293, 1.0254, 1.0298, 1.0586, 1.1445, 1.2930, 
                   1.2422, 1.1074, 1.0771, 1.0347, 1.0059, 0.9990, 1.0215, 1.0635, 1.1123, 1.2407, 
                   1.2227, 1.0952, 1.0874, 1.0220, 1.0093, 1.0093, 1.0044, 1.0449, 1.1011, 1.2168, 
                   1.1953, 1.0981, 1.0488, 1.0166, 1.0088, 0.9990, 1.0083, 1.0479, 1.0835, 1.2280, 
                   1.1865, 1.1143, 1.0620, 1.0210, 1.0073, 0.9712, 1.0200, 1.0239, 1.1123, 1.1973, 
                   1.2324, 1.1113, 1.0635, 1.0405, 1.0117, 1.0112, 1.0137, 1.0586, 1.1143, 1.2646, 
                   1.2607, 1.1548, 1.0986, 1.0415, 1.0513, 0.9976, 1.0542, 1.0625, 1.1538, 1.2988, 
                   1.2769, 1.2080, 1.1094, 1.1104, 1.0215, 1.0786, 1.0664, 1.0938, 1.2129, 1.3091, 
                },
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
      338,
      /* ev0_count */
      338,
      /* ev2_count */
      338,
      /* fix_fps_index */
      25,
      /* enable_isp_digital_gain */
      0,
      /* max_sensor_gain */
      64,
      /* max_isp_gain1 */
      1,
      /* exp_table */
      {
      //1,495
      //64,495
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
         {    261,    495   },         /* Gain = 1.019531 Exposure Index = 200 */
         {    269,    495   },         /* Gain = 1.050781 Exposure Index = 201 */
         {    278,    495   },         /* Gain = 1.085938 Exposure Index = 202 */
         {    287,    495   },         /* Gain = 1.121094 Exposure Index = 203 */
         {    296,    495   },         /* Gain = 1.156250 Exposure Index = 204 */
         {    305,    495   },         /* Gain = 1.191406 Exposure Index = 205 */
         {    315,    495   },         /* Gain = 1.230469 Exposure Index = 206 */
         {    325,    495   },         /* Gain = 1.269531 Exposure Index = 207 */
         {    335,    495   },         /* Gain = 1.308594 Exposure Index = 208 */
         {    346,    495   },         /* Gain = 1.351562 Exposure Index = 209 */
         {    357,    495   },         /* Gain = 1.394531 Exposure Index = 210 */
         {    368,    495   },         /* Gain = 1.437500 Exposure Index = 211 */
         {    380,    495   },         /* Gain = 1.484375 Exposure Index = 212 */
         {    392,    495   },         /* Gain = 1.531250 Exposure Index = 213 */
         {    404,    495   },         /* Gain = 1.578125 Exposure Index = 214 */
         {    417,    495   },         /* Gain = 1.628906 Exposure Index = 215 */
         {    430,    495   },         /* Gain = 1.679688 Exposure Index = 216 */
         {    443,    495   },         /* Gain = 1.730469 Exposure Index = 217 */
         {    457,    495   },         /* Gain = 1.785156 Exposure Index = 218 */
         {    471,    495   },         /* Gain = 1.839844 Exposure Index = 219 */
         {    486,    495   },         /* Gain = 1.898438 Exposure Index = 220 */
         {    501,    495   },         /* Gain = 1.957031 Exposure Index = 221 */
         {    517,    495   },         /* Gain = 2.019531 Exposure Index = 222 */
         {    533,    495   },         /* Gain = 2.082031 Exposure Index = 223 */
         {    549,    495   },         /* Gain = 2.144531 Exposure Index = 224 */
         {    566,    495   },         /* Gain = 2.210938 Exposure Index = 225 */
         {    583,    495   },         /* Gain = 2.277344 Exposure Index = 226 */
         {    601,    495   },         /* Gain = 2.347656 Exposure Index = 227 */
         {    620,    495   },         /* Gain = 2.421875 Exposure Index = 228 */
         {    639,    495   },         /* Gain = 2.496094 Exposure Index = 229 */
         {    659,    495   },         /* Gain = 2.574219 Exposure Index = 230 */
         {    679,    495   },         /* Gain = 2.652344 Exposure Index = 231 */
         {    700,    495   },         /* Gain = 2.734375 Exposure Index = 232 */
         {    721,    495   },         /* Gain = 2.816406 Exposure Index = 233 */
         {    743,    495   },         /* Gain = 2.902344 Exposure Index = 234 */
         {    766,    495   },         /* Gain = 2.992188 Exposure Index = 235 */
         {    789,    495   },         /* Gain = 3.082031 Exposure Index = 236 */
         {    813,    495   },         /* Gain = 3.175781 Exposure Index = 237 */
         {    838,    495   },         /* Gain = 3.273438 Exposure Index = 238 */
         {    864,    495   },         /* Gain = 3.375000 Exposure Index = 239 */
         {    890,    495   },         /* Gain = 3.476562 Exposure Index = 240 */
         {    917,    495   },         /* Gain = 3.582031 Exposure Index = 241 */
         {    945,    495   },         /* Gain = 3.691406 Exposure Index = 242 */
         {    974,    495   },         /* Gain = 3.804688 Exposure Index = 243 */
         {   1004,    495   },         /* Gain = 3.921875 Exposure Index = 244 */
         {   1035,    495   },         /* Gain = 4.042969 Exposure Index = 245 */
         {   1067,    495   },         /* Gain = 4.167969 Exposure Index = 246 */
         {   1100,    495   },         /* Gain = 4.296875 Exposure Index = 247 */
         {   1133,    495   },         /* Gain = 4.425781 Exposure Index = 248 */
         {   1167,    495   },         /* Gain = 4.558594 Exposure Index = 249 */
         {   1203,    495   },         /* Gain = 4.699219 Exposure Index = 250 */
         {   1240,    495   },         /* Gain = 4.843750 Exposure Index = 251 */
         {   1278,    495   },         /* Gain = 4.992188 Exposure Index = 252 */
         {   1317,    495   },         /* Gain = 5.144531 Exposure Index = 253 */
         {   1357,    495   },         /* Gain = 5.300781 Exposure Index = 254 */
         {   1398,    495   },         /* Gain = 5.460938 Exposure Index = 255 */
         {   1440,    495   },         /* Gain = 5.625000 Exposure Index = 256 */
         {   1484,    495   },         /* Gain = 5.796875 Exposure Index = 257 */
         {   1529,    495   },         /* Gain = 5.972656 Exposure Index = 258 */
         {   1575,    495   },         /* Gain = 6.152344 Exposure Index = 259 */
         {   1623,    495   },         /* Gain = 6.339844 Exposure Index = 260 */
         {   1672,    495   },         /* Gain = 6.531250 Exposure Index = 261 */
         {   1723,    495   },         /* Gain = 6.730469 Exposure Index = 262 */
         {   1775,    495   },         /* Gain = 6.933594 Exposure Index = 263 */
         {   1829,    495   },         /* Gain = 7.144531 Exposure Index = 264 */
         {   1884,    495   },         /* Gain = 7.359375 Exposure Index = 265 */
         {   1941,    495   },         /* Gain = 7.582031 Exposure Index = 266 */
         {   2000,    495   },         /* Gain = 7.812500 Exposure Index = 267 */
         {   2060,    495   },         /* Gain = 8.046875 Exposure Index = 268 */
         {   2122,    495   },         /* Gain = 8.289062 Exposure Index = 269 */
         {   2186,    495   },         /* Gain = 8.539062 Exposure Index = 270 */
         {   2252,    495   },         /* Gain = 8.796875 Exposure Index = 271 */
         {   2320,    495   },         /* Gain = 9.062500 Exposure Index = 272 */
         {   2390,    495   },         /* Gain = 9.335938 Exposure Index = 273 */
         {   2462,    495   },         /* Gain = 9.617188 Exposure Index = 274 */
         {   2536,    495   },         /* Gain = 9.906250 Exposure Index = 275 */
         {   2613,    495   },         /* Gain = 10.207031 Exposure Index = 276 */
         {   2692,    495   },         /* Gain = 10.515625 Exposure Index = 277 */
         {   2773,    495   },         /* Gain = 10.832031 Exposure Index = 278 */
         {   2857,    495   },         /* Gain = 11.160156 Exposure Index = 279 */
         {   2943,    495   },         /* Gain = 11.496094 Exposure Index = 280 */
         {   3032,    495   },         /* Gain = 11.843750 Exposure Index = 281 */
         {   3123,    495   },         /* Gain = 12.199219 Exposure Index = 282 */
         {   3217,    495   },         /* Gain = 12.566406 Exposure Index = 283 */
         {   3314,    495   },         /* Gain = 12.945312 Exposure Index = 284 */
         {   3414,    495   },         /* Gain = 13.335938 Exposure Index = 285 */
         {   3517,    495   },         /* Gain = 13.738281 Exposure Index = 286 */
         {   3623,    495   },         /* Gain = 14.152344 Exposure Index = 287 */
         {   3732,    495   },         /* Gain = 14.578125 Exposure Index = 288 */
         {   3844,    495   },         /* Gain = 15.015625 Exposure Index = 289 */
         {   3960,    495   },         /* Gain = 15.468750 Exposure Index = 290 */
         {   4079,    495   },         /* Gain = 15.933594 Exposure Index = 291 */
         {   4202,    495   },         /* Gain = 16.414062 Exposure Index = 292 */
         {   4329,    495   },         /* Gain = 16.910156 Exposure Index = 293 */
         {   4459,    495   },         /* Gain = 17.417969 Exposure Index = 294 */
         {   4593,    495   },         /* Gain = 17.941406 Exposure Index = 295 */
         {   4731,    495   },         /* Gain = 18.480469 Exposure Index = 296 */
         {   4873,    495   },         /* Gain = 19.035156 Exposure Index = 297 */
         {   5020,    495   },         /* Gain = 19.609375 Exposure Index = 298 */
         {   5171,    495   },         /* Gain = 20.199219 Exposure Index = 299 */
         {   5327,    495   },         /* Gain = 20.808594 Exposure Index = 300 */
         {   5487,    495   },         /* Gain = 21.433594 Exposure Index = 301 */
         {   5652,    495   },         /* Gain = 22.078125 Exposure Index = 302 */
         {   5822,    495   },         /* Gain = 22.742188 Exposure Index = 303 */
         {   5997,    495   },         /* Gain = 23.425781 Exposure Index = 304 */
         {   6177,    495   },         /* Gain = 24.128906 Exposure Index = 305 */
         {   6363,    495   },         /* Gain = 24.855469 Exposure Index = 306 */
         {   6554,    495   },         /* Gain = 25.601562 Exposure Index = 307 */
         {   6751,    495   },         /* Gain = 26.371094 Exposure Index = 308 */
         {   6954,    495   },         /* Gain = 27.164062 Exposure Index = 309 */
         {   7163,    495   },         /* Gain = 27.980469 Exposure Index = 310 */
         {   7378,    495   },         /* Gain = 28.820312 Exposure Index = 311 */
         {   7600,    495   },         /* Gain = 29.687500 Exposure Index = 312 */
         {   7828,    495   },         /* Gain = 30.578125 Exposure Index = 313 */
         {   8063,    495   },         /* Gain = 31.496094 Exposure Index = 314 */
         {   8305,    495   },         /* Gain = 32.441406 Exposure Index = 315 */
         {   8555,    495   },         /* Gain = 33.417969 Exposure Index = 316 */
         {   8812,    495   },         /* Gain = 34.421875 Exposure Index = 317 */
         {   9077,    495   },         /* Gain = 35.457031 Exposure Index = 318 */
         {   9350,    495   },         /* Gain = 36.523438 Exposure Index = 319 */
         {   9631,    495   },         /* Gain = 37.621094 Exposure Index = 320 */
         {   9920,    495   },         /* Gain = 38.750000 Exposure Index = 321 */
         {  10218,    495   },         /* Gain = 39.914062 Exposure Index = 322 */
         {  10525,    495   },         /* Gain = 41.113281 Exposure Index = 323 */
         {  10841,    495   },         /* Gain = 42.347656 Exposure Index = 324 */
         {  11167,    495   },         /* Gain = 43.621094 Exposure Index = 325 */
         {  11503,    495   },         /* Gain = 44.933594 Exposure Index = 326 */
         {  11849,    495   },         /* Gain = 46.285156 Exposure Index = 327 */
         {  12205,    495   },         /* Gain = 47.675781 Exposure Index = 328 */
         {  12572,    495   },         /* Gain = 49.109375 Exposure Index = 329 */
         {  12950,    495   },         /* Gain = 50.585938 Exposure Index = 330 */
         {  13339,    495   },         /* Gain = 52.105469 Exposure Index = 331 */
         {  13740,    495   },         /* Gain = 53.671875 Exposure Index = 332 */
         {  14153,    495   },         /* Gain = 55.285156 Exposure Index = 333 */
         {  14578,    495   },         /* Gain = 56.945312 Exposure Index = 334 */
         {  15016,    495   },         /* Gain = 58.656250 Exposure Index = 335 */
         {  15467,    495   },         /* Gain = 60.417969 Exposure Index = 336 */
         {  15932,    495   },         /* Gain = 62.234375 Exposure Index = 337 */
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
         6,/* count */
         /* luma target lut */
         {
            {90,40},
            {150,38},
            {230,35},
            {280,35},
            {300,30},
            {360,30},
            {450,25},
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
      1,//aec algo type,0:fast smooth  1:fast plus
      /* AEC_FAST_PLUS_PARAM_t aec_fast_plus_param */
      {
         3,//luma_tolerance
         2,//frame_skip
         2,//ddr_frame_skip_offset
         {5,10,20},//interval_threshold[3]
         {0.7,0.5,0.3},//speed_ratio[3]
         0.3,//speed_ratio_others
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
      { 0.4687, 0.7694 },   /* d75 */
      { 0.4842, 0.6931 },   /* d65 */
      { 0.5432, 0.6041 },   /* d50 */
      { 0.5432, 0.6041 },   /* noon */
      { 0.5823, 0.4466 },   /* cw */
      { 0.6731, 0.4969 },   /* tl84 */
      { 0.7836, 0.4110 },   /* a */
      { 0.9216, 0.3920 },   /* h */
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
            0.59,   /* rg d50 */
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
            0.6,   /* bg d50 */
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
         0x0000001e,//Ymin
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
      1,//enable_subsample
      4,//w_ration
      4,//h_ration
   },
   1,//enable_gain_adjust
   0,//enable_lowlight_lut
   1,//lowlight_lut_count
   0,//force_simple_grey_word
   1,//enable_init_point
   0,//use_init_point_count
   /* init point */
   {
      0.54,//rg
      0.66,//bg
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
            { 1.0100, 1.0000 },   /* d65 */
            { 1.0100, 0.9800 },   /* d50 */
            { 1.0100, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
            { 1.0000, 1.0000 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 1.0000, 1.0000 },   /* custom1 */
            { 1.0000, 1.0000 },   /* custom2 */
         },
         200,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0000 },   /* d75 */
            { 1.0100, 1.0000 },   /* d65 */
            { 1.0100, 0.9800 },   /* d50 */
            { 1.0100, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
            { 1.0000, 0.9800 },   /* tl84 */
            { 1.0100, 0.9800 },   /* a */
            { 1.0300, 0.9700 },   /* h */
            { 1.0000, 1.0000 },   /* custom1 */
            { 1.0000, 1.0000 },   /* custom2 */
         },
         280,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0000 },   /* d75 */
            { 1.0000, 1.0000 },   /* d65 */
            { 1.0500, 0.9500 },   /* d50 */
            { 1.0000, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
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
            { 1.0300, 1.0000 },   /* d50 */
            { 1.0300, 1.0000 },   /* noon */
            { 1.0300, 1.0000 },   /* cw */
            { 1.0300, 1.0000 },   /* tl84 */
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
            { 1.1000, 1.0000 },   /* d50 */
            { 1.0000, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
            { 1.0000, 0.9700 },   /* tl84 */
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
            { 1.0000, 1.1000 },   /* d50 */
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
            { 1.2000, 0.9000 },   /* d75 */
            { 1.0500, 0.9600 },   /* d65 */
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
