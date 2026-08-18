
/*---------------header of imx307 tuning-------------------------*/

/* version of the tuning header uint32_t header_version */
0x20221013,
/* tuning_ctl_t tuning_ctl */
{
  //float aec_trigger_tolerence;
  3,
  //float aec_trigger_tolerence_gain;
  0.3,
  //float awb_trigger_tolerence;
  50,
  //tuning_pra_update_ration
  10,
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
               16.1,
           },
           /* aec trigger 1*/
           {
               32,
               32.1,
           },
           /* aec trigger 2*/
           {
               64,
               64.1,
           },
           /* aec trigger 3*/
           {
               128,
               128.1,
           },
           /* aec trigger 4*/
           {
               252,
               256.1,
           },
       },
       /* isp_sub_module_blc_para para[MIDDLE_TRIGGER_COUNT] MIDDLE_TRIGGER_COUNT==5 */
       {
           /* para 0*/
           {
               1025,//R
               1024,//B
               1024,//GR
               1024,//GB
               274,//gian R
               274,//gain B
               274,//gain GR
               274,//gain GB
           },
           /* para 1*/
           {
               1027,//R
               1027,//B
               1027,//GR
               1029,//GB
               274,//gian R
               274,//gain B
               274,//gain GR
               274,//gain GB
           },
           /* para 2*/
           {
               1033,//R
               1038,//B
               1052,//GR
               1054,//GB
               274,//gian R
               274,//gain B
               274,//gain GR
               274,//gain GB
           },
           /* para 3*/
           {
               1042,//R
               1052,//B
               1041,//GR
               1045,//GB
               274,//gian R
               274,//gain B
               274,//gain GR
               274,//gain GB
           },
           /* para 4*/
           {
               986,//R
               905,//B
               884,//GR
               892,//GB
               275,//gian R
               275,//gain B
               275,//gain GR
               275,//gain GB
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
       1,
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
       3,
       /* 0 for dynamic dpc, 1 for static dpc */
       2,
       /* tigger_mode : 0 gain ,1 lux */
       0,
       /* region enable */
       {
          0,
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
             18,
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
             0x0000004c,0x00000002,0x000003e8,0x00000578,0x00000640,0x000003e8,0x00000578,0x00000640,0x00000c00,0x00000028,
             0x0000003c,0x00000bb8,0x000000c8,0x00000190,0x00000033,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
             0x00000000,0x00000020,0x00000060,0x000000a0,0x000000e0,
          },
          /* parameter 1 */
          {
             0x0000000c,0x00000002,0x00000190,0x00000bb8,0x00000fa0,0x000007d0,0x00000fa0,0x00000bb8,0x00000d48,0x00000bb8,
             0x00000ed8,0x000007d0,0x00000fa0,0x00000bb8,0x00000100,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,
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
             251,
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
                       {8,8,8},
                       /* channel 2 */
                       {8,8,8},
                       /* channel 3 */
                       {8,8,8},
                       /* channel 4 */
                       {8,8,8},
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
                       {12,12,12},
                       /* channel 2 */
                       {12,12,12},
                       /* channel 3 */
                       {12,12,12},
                       /* channel 4 */
                       {12,12,12},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {12,12,12},
                       /* channel 2 */
                       {12,12,12},
                       /* channel 3 */
                       {12,12,12},
                       /* channel 4 */
                       {12,12,12},
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
                       {26,26,26},
                       /* channel 2 */
                       {26,26,26},
                       /* channel 3 */
                       {26,26,26},
                       /* channel 4 */
                       {26,26,26},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {16,16,16},
                       /* channel 2 */
                       {16,16,16},
                       /* channel 3 */
                       {16,16,16},
                       /* channel 4 */
                       {16,16,16},
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
                       {26,26,26},
                       /* channel 2 */
                       {26,26,26},
                       /* channel 3 */
                       {26,26,26},
                       /* channel 4 */
                       {26,26,26},
                   },
                   /* layer 2 */
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
                       {66,66,66},
                       /* channel 2 */
                       {66,66,66},
                       /* channel 3 */
                       {66,66,66},
                       /* channel 4 */
                       {66,66,66},
                   },
                   /* layer 2 */
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
                       {111,111,111},
                       /* channel 2 */
                       {111,111,111},
                       /* channel 3 */
                       {111,111,111},
                       /* channel 4 */
                       {111,111,111},
                   },
                   /* layer 2 */
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
                       {185,185,185},
                       /* channel 2 */
                       {185,185,185},
                       /* channel 3 */
                       {185,185,185},
                       /* channel 4 */
                       {185,185,185},
                   },
                   /* layer 2 */
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
                       {220,220,220},
                       /* channel 2 */
                       {220,220,220},
                       /* channel 3 */
                       {220,220,220},
                       /* channel 4 */
                       {220,220,220},
                   },
                   /* layer 2 */
                   {
                       /* channel 1 */
                       {75,75,75},
                       /* channel 2 */
                       {75,75,75},
                       /* channel 3 */
                       {75,75,75},
                       /* channel 4 */
                       {75,75,75},
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
                       {245,245,245},
                       /* channel 2 */
                       {245,245,245},
                       /* channel 3 */
                       {245,245,245},
                       /* channel 4 */
                       {245,245,245},
                   },
                   /* layer 2 */
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
                       {33,33,33},
                       /* channel 2 */
                       {33,33,33},
                       /* channel 3 */
                       {33,33,33},
                       /* channel 4 */
                       {33,33,33},
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
                       {75,75,75},
                       /* channel 2 */
                       {75,75,75},
                       /* channel 3 */
                       {75,75,75},
                       /* channel 4 */
                       {75,75,75},
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
             340,
          },
          /* aec trigger 1 */
          {
             350,
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
                   2.6423, 2.0980, 1.7652, 1.5725, 1.5013, 1.5118, 1.5693, 1.7590, 2.0965, 2.6276, 
                   2.3858, 1.8820, 1.6099, 1.4294, 1.3479, 1.3460, 1.4162, 1.5875, 1.8570, 2.3426, 
                   2.1383, 1.6625, 1.4316, 1.2422, 1.1684, 1.1631, 1.2368, 1.4018, 1.6436, 2.0692, 
                   1.9973, 1.5819, 1.3390, 1.1809, 1.0772, 1.0845, 1.1625, 1.3466, 1.5611, 1.9756, 
                   2.0058, 1.6111, 1.3568, 1.1828, 1.0383, 1.0254, 1.1683, 1.3426, 1.6010, 1.9696, 
                   1.9501, 1.5490, 1.2985, 1.1455, 0.9918, 1.0031, 1.1173, 1.2953, 1.5289, 1.9014, 
                   1.9814, 1.5917, 1.3362, 1.1774, 1.0811, 1.0787, 1.1665, 1.3302, 1.5804, 1.9425, 
                   2.2494, 1.7704, 1.4956, 1.3209, 1.2315, 1.2382, 1.3140, 1.4992, 1.7378, 2.2376, 
                   2.3519, 1.8698, 1.5765, 1.4113, 1.3310, 1.3208, 1.4061, 1.5525, 1.8511, 2.2889, 
                   2.6587, 2.0686, 1.7529, 1.5554, 1.4425, 1.5053, 1.5568, 1.7745, 2.0381, 2.6936, 
                   2.5197, 2.0347, 1.7087, 1.5533, 1.4633, 1.4612, 1.5502, 1.6972, 2.0303, 2.5034, 
                   2.2884, 1.8414, 1.5728, 1.4047, 1.3343, 1.3274, 1.4024, 1.5546, 1.8087, 2.2678, 
                   2.0544, 1.6189, 1.3909, 1.2421, 1.1456, 1.1538, 1.2273, 1.3779, 1.5946, 1.9878, 
                   1.9034, 1.5527, 1.3198, 1.1674, 1.0701, 1.0697, 1.1501, 1.3177, 1.5285, 1.9262, 
                   1.9480, 1.5773, 1.3462, 1.1805, 1.0408, 1.0252, 1.1687, 1.3331, 1.5629, 1.9193, 
                   1.8686, 1.5160, 1.2881, 1.1354, 0.9943, 0.9968, 1.1105, 1.2807, 1.4949, 1.8436, 
                   1.9264, 1.5583, 1.3193, 1.1732, 1.0774, 1.0768, 1.1585, 1.3185, 1.5367, 1.9162, 
                   2.1801, 1.7285, 1.4876, 1.3123, 1.2356, 1.2260, 1.3117, 1.4633, 1.7242, 2.1200, 
                   2.2721, 1.8228, 1.5485, 1.3888, 1.3125, 1.3190, 1.3844, 1.5347, 1.7834, 2.2239, 
                   2.5598, 1.9738, 1.7189, 1.5037, 1.4532, 1.4434, 1.5500, 1.6732, 2.0054, 2.5010, 
                   2.4267, 2.0285, 1.7280, 1.5416, 1.4725, 1.4766, 1.5613, 1.7030, 2.0787, 2.4415, 
                   2.3233, 1.8552, 1.5745, 1.4159, 1.3449, 1.3196, 1.4210, 1.5320, 1.8433, 2.2169, 
                   2.0000, 1.5901, 1.3961, 1.2289, 1.1443, 1.1634, 1.2117, 1.3751, 1.5977, 1.9160, 
                   1.9389, 1.5483, 1.3126, 1.1540, 1.0872, 1.0573, 1.1730, 1.2811, 1.5598, 1.8014, 
                   1.9544, 1.5717, 1.3402, 1.1796, 1.0272, 1.0295, 1.1564, 1.3201, 1.5727, 1.8435, 
                   1.8758, 1.5119, 1.2944, 1.1197, 1.0139, 0.9884, 1.1301, 1.2642, 1.5122, 1.8035, 
                   1.9282, 1.5287, 1.3060, 1.1595, 1.0576, 1.0686, 1.1481, 1.2914, 1.5246, 1.8240, 
                   2.1484, 1.7252, 1.4550, 1.3193, 1.2171, 1.2227, 1.2941, 1.4440, 1.7042, 2.0575, 
                   2.2924, 1.8137, 1.5721, 1.3769, 1.3226, 1.3132, 1.3915, 1.5377, 1.7967, 2.1585, 
                   2.4416, 1.9620, 1.6171, 1.5015, 1.3973, 1.4079, 1.4811, 1.5852, 1.9238, 2.3329, 
                },
             },
             /* awb para 1 */
             {
                0.5,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.6195, 2.0717, 1.7333, 1.5942, 1.4835, 1.5152, 1.5604, 1.7582, 2.0554, 2.5636, 
                   2.2979, 1.8119, 1.5680, 1.3852, 1.3196, 1.3149, 1.3838, 1.5259, 1.8037, 2.2202, 
                   2.1486, 1.6780, 1.4519, 1.2811, 1.1987, 1.1969, 1.2717, 1.4364, 1.6605, 2.0973, 
                   1.9910, 1.5803, 1.3554, 1.1943, 1.0974, 1.1010, 1.1749, 1.3549, 1.5618, 1.9529, 
                   1.9483, 1.5490, 1.3259, 1.1562, 1.0109, 1.0058, 1.1382, 1.3127, 1.5407, 1.9003, 
                   1.9045, 1.5677, 1.3094, 1.1623, 1.0211, 1.0249, 1.1312, 1.3210, 1.5293, 1.9135, 
                   2.0090, 1.5959, 1.3668, 1.1999, 1.1045, 1.1010, 1.2021, 1.3663, 1.5822, 2.0057, 
                   2.1445, 1.6934, 1.4538, 1.2774, 1.2041, 1.2115, 1.2779, 1.4485, 1.6829, 2.0900, 
                   2.2998, 1.8829, 1.5730, 1.4167, 1.3393, 1.3310, 1.4150, 1.5583, 1.8371, 2.3080, 
                   2.7131, 2.0337, 1.7974, 1.5647, 1.5023, 1.5234, 1.6140, 1.7766, 2.0756, 2.6047, 
                   2.5093, 2.0442, 1.7078, 1.5476, 1.4760, 1.4748, 1.5270, 1.7027, 2.0024, 2.4533, 
                   2.2548, 1.7903, 1.5404, 1.3825, 1.3072, 1.3035, 1.3712, 1.5167, 1.7571, 2.2019, 
                   2.0805, 1.6729, 1.4291, 1.2721, 1.1864, 1.1854, 1.2494, 1.4161, 1.6176, 2.0235, 
                   1.9497, 1.5647, 1.3484, 1.1887, 1.0969, 1.0934, 1.1745, 1.3415, 1.5371, 1.9330, 
                   1.8740, 1.5424, 1.3120, 1.1554, 1.0115, 0.9992, 1.1379, 1.3012, 1.5115, 1.8501, 
                   1.8864, 1.5481, 1.3109, 1.1577, 1.0272, 1.0158, 1.1378, 1.2984, 1.5139, 1.8516, 
                   1.9718, 1.5877, 1.3615, 1.1924, 1.1026, 1.0960, 1.1864, 1.3474, 1.5541, 1.9397, 
                   2.0862, 1.6953, 1.4407, 1.2913, 1.2087, 1.2006, 1.2800, 1.4230, 1.6639, 2.0410, 
                   2.3128, 1.8357, 1.5771, 1.3941, 1.3367, 1.3206, 1.3981, 1.5389, 1.7935, 2.2219, 
                   2.5813, 2.0566, 1.7620, 1.5746, 1.4917, 1.4922, 1.5999, 1.6883, 2.0425, 2.4926, 
                   2.3938, 2.0279, 1.6899, 1.5465, 1.4852, 1.4569, 1.5504, 1.6736, 2.0544, 2.4187, 
                   2.2326, 1.7921, 1.5390, 1.3796, 1.3028, 1.3016, 1.3604, 1.5067, 1.7469, 2.1259, 
                   2.0721, 1.6182, 1.4304, 1.2548, 1.1917, 1.1866, 1.2565, 1.4055, 1.6208, 1.9936, 
                   1.9418, 1.5680, 1.3322, 1.1927, 1.0862, 1.0848, 1.1688, 1.3061, 1.5520, 1.8121, 
                   1.8808, 1.5046, 1.3123, 1.1283, 1.0136, 1.0024, 1.1355, 1.3045, 1.4997, 1.8433, 
                   1.8880, 1.5508, 1.3027, 1.1541, 1.0256, 1.0096, 1.1344, 1.2737, 1.5211, 1.7797, 
                   1.9660, 1.5451, 1.3566, 1.1694, 1.0951, 1.0967, 1.1716, 1.3471, 1.5382, 1.8916, 
                   2.0839, 1.6721, 1.4118, 1.2775, 1.1805, 1.1860, 1.2558, 1.4021, 1.6365, 1.9967, 
                   2.2358, 1.8250, 1.5637, 1.3905, 1.3300, 1.3257, 1.3893, 1.5472, 1.7842, 2.1745, 
                   2.5714, 1.9648, 1.7152, 1.5135, 1.4489, 1.4626, 1.5167, 1.6510, 1.9742, 2.4120, 
                },
             },
             /* awb para 2 */
             {
                0.5,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.5430, 2.0922, 1.7364, 1.5770, 1.5055, 1.4990, 1.5565, 1.7463, 2.0341, 2.5568, 
                   2.3197, 1.8088, 1.5470, 1.3806, 1.2929, 1.2872, 1.3569, 1.5050, 1.7877, 2.2441, 
                   2.2293, 1.7493, 1.5098, 1.3189, 1.2411, 1.2317, 1.3206, 1.4884, 1.7292, 2.1742, 
                   2.0304, 1.6170, 1.3650, 1.2042, 1.1000, 1.1058, 1.1812, 1.3592, 1.5960, 1.9810, 
                   1.9113, 1.5303, 1.2800, 1.1253, 0.9866, 0.9663, 1.1102, 1.2785, 1.5103, 1.8731, 
                   2.0202, 1.6282, 1.3637, 1.1902, 1.0447, 1.0509, 1.1729, 1.3652, 1.5940, 2.0002, 
                   2.0520, 1.6288, 1.3758, 1.2098, 1.1105, 1.1139, 1.2149, 1.3773, 1.6248, 2.0299, 
                   2.1206, 1.6805, 1.4212, 1.2617, 1.1614, 1.1800, 1.2399, 1.4207, 1.6344, 2.1173, 
                   2.3873, 1.9202, 1.6216, 1.4336, 1.3740, 1.3571, 1.4594, 1.6071, 1.9089, 2.3748, 
                   2.8190, 2.1527, 1.8478, 1.6569, 1.5208, 1.5832, 1.6457, 1.8387, 2.1595, 2.7836, 
                   2.5253, 2.0201, 1.7137, 1.5341, 1.4633, 1.4575, 1.5166, 1.6724, 2.0001, 2.4323, 
                   2.1773, 1.7782, 1.5019, 1.3660, 1.2755, 1.2811, 1.3488, 1.4890, 1.7310, 2.1591, 
                   2.1846, 1.7184, 1.5003, 1.3129, 1.2368, 1.2359, 1.2990, 1.4765, 1.6846, 2.1085, 
                   1.9469, 1.5984, 1.3537, 1.2011, 1.1043, 1.0961, 1.1933, 1.3472, 1.5679, 1.9616, 
                   1.8383, 1.4917, 1.2779, 1.1180, 0.9837, 0.9786, 1.0984, 1.2725, 1.4761, 1.7906, 
                   2.0040, 1.5932, 1.3688, 1.1939, 1.0603, 1.0400, 1.1910, 1.3391, 1.5741, 1.9603, 
                   1.9781, 1.6202, 1.3718, 1.2121, 1.1222, 1.1207, 1.2048, 1.3715, 1.5985, 1.9542, 
                   2.0616, 1.6330, 1.4031, 1.2412, 1.1639, 1.1592, 1.2407, 1.3791, 1.6182, 2.0103, 
                   2.3400, 1.8883, 1.6078, 1.4365, 1.3714, 1.3519, 1.4408, 1.5824, 1.8461, 2.2916, 
                   2.6814, 2.1225, 1.8194, 1.6179, 1.5318, 1.5708, 1.6338, 1.7778, 2.1313, 2.6280, 
                   2.3904, 1.9612, 1.6674, 1.5221, 1.4509, 1.4491, 1.5343, 1.6602, 2.0119, 2.3962, 
                   2.2005, 1.7225, 1.5225, 1.3323, 1.2709, 1.2690, 1.3312, 1.4632, 1.7233, 2.0712, 
                   2.1528, 1.7050, 1.4794, 1.3208, 1.2386, 1.2341, 1.3149, 1.4591, 1.7018, 2.0569, 
                   1.9581, 1.5702, 1.3522, 1.1904, 1.1025, 1.0965, 1.1766, 1.3257, 1.5616, 1.8397, 
                   1.8408, 1.4671, 1.2682, 1.1107, 0.9782, 0.9631, 1.1108, 1.2375, 1.4734, 1.7347, 
                   1.9295, 1.6185, 1.3477, 1.1971, 1.0689, 1.0538, 1.1813, 1.3399, 1.5738, 1.9098, 
                   1.9897, 1.5871, 1.3685, 1.2095, 1.1073, 1.1098, 1.2066, 1.3437, 1.5876, 1.8957, 
                   1.9665, 1.6163, 1.3704, 1.2359, 1.1492, 1.1556, 1.2063, 1.3693, 1.5593, 1.9159, 
                   2.3487, 1.8539, 1.6073, 1.4185, 1.3608, 1.3468, 1.4381, 1.5634, 1.8575, 2.2292, 
                   2.5897, 2.0597, 1.7583, 1.6051, 1.4838, 1.5278, 1.5655, 1.7421, 2.0151, 2.4787, 
                },
             },
             /* awb para 3 */
             {
                0.5,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.6731, 2.1366, 1.8119, 1.6220, 1.5435, 1.5666, 1.5964, 1.8430, 2.1061, 2.7328, 
                   2.3384, 1.8839, 1.6000, 1.4300, 1.3426, 1.3387, 1.4304, 1.5666, 1.8768, 2.3148, 
                   2.1301, 1.6629, 1.4281, 1.2522, 1.1706, 1.1674, 1.2401, 1.4116, 1.6279, 2.0703, 
                   2.0385, 1.6100, 1.3784, 1.2072, 1.1035, 1.1170, 1.1910, 1.3625, 1.6041, 1.9921, 
                   1.9702, 1.6247, 1.3450, 1.1927, 1.0425, 1.0269, 1.1758, 1.3397, 1.5927, 1.9441, 
                   1.9132, 1.5418, 1.2987, 1.1278, 0.9963, 0.9927, 1.1099, 1.2947, 1.5082, 1.8902, 
                   2.0192, 1.6368, 1.3738, 1.2051, 1.1129, 1.1003, 1.2068, 1.3526, 1.6170, 1.9740, 
                   2.2490, 1.7576, 1.5140, 1.3229, 1.2422, 1.2384, 1.3228, 1.4951, 1.7494, 2.2074, 
                   2.3353, 1.8641, 1.5682, 1.3950, 1.3177, 1.3122, 1.3840, 1.5473, 1.8147, 2.2715, 
                   2.7070, 2.0917, 1.8013, 1.5797, 1.5022, 1.5211, 1.6185, 1.7411, 2.1511, 2.6027, 
                   2.5830, 2.0639, 1.7685, 1.5895, 1.5078, 1.5074, 1.5869, 1.7611, 2.0609, 2.5456, 
                   2.3077, 1.8399, 1.5726, 1.4143, 1.3235, 1.3320, 1.3973, 1.5379, 1.7982, 2.2412, 
                   2.0356, 1.6118, 1.4013, 1.2397, 1.1517, 1.1529, 1.2323, 1.3793, 1.5846, 1.9819, 
                   1.9728, 1.5931, 1.3644, 1.2055, 1.1108, 1.1018, 1.1912, 1.3452, 1.5643, 1.9583, 
                   1.9488, 1.5822, 1.3539, 1.1858, 1.0381, 1.0310, 1.1721, 1.3392, 1.5568, 1.9225, 
                   1.8747, 1.5068, 1.2843, 1.1304, 0.9978, 0.9873, 1.1156, 1.2654, 1.4827, 1.8309, 
                   1.9520, 1.6031, 1.3567, 1.2025, 1.1080, 1.1077, 1.1922, 1.3503, 1.5729, 1.9460, 
                   2.2024, 1.7365, 1.4996, 1.3301, 1.2395, 1.2357, 1.3197, 1.4765, 1.7076, 2.1489, 
                   2.2349, 1.8059, 1.5335, 1.3757, 1.3105, 1.3003, 1.3690, 1.5118, 1.7629, 2.1922, 
                   2.6082, 2.0255, 1.7539, 1.5727, 1.4815, 1.4842, 1.5958, 1.6996, 2.0335, 2.5338, 
                   2.4765, 2.1055, 1.7361, 1.6024, 1.5015, 1.5166, 1.5672, 1.7408, 2.0930, 2.4993, 
                   2.2826, 1.8141, 1.5860, 1.4052, 1.3398, 1.3195, 1.4097, 1.5418, 1.7927, 2.1927, 
                   1.9833, 1.5985, 1.3766, 1.2265, 1.1493, 1.1559, 1.2116, 1.3746, 1.5809, 1.9210, 
                   1.9442, 1.5797, 1.3525, 1.2045, 1.1066, 1.1042, 1.1886, 1.3470, 1.5638, 1.8992, 
                   1.9512, 1.5700, 1.3525, 1.1790, 1.0501, 1.0323, 1.1753, 1.3254, 1.5658, 1.8602, 
                   1.7923, 1.4944, 1.2646, 1.1181, 0.9929, 0.9921, 1.1058, 1.2765, 1.4560, 1.8039, 
                   1.9448, 1.5780, 1.3453, 1.2046, 1.0927, 1.1132, 1.1820, 1.3258, 1.5673, 1.8561, 
                   2.1407, 1.7051, 1.4726, 1.3106, 1.2269, 1.2330, 1.3084, 1.4677, 1.6850, 2.0859, 
                   2.2035, 1.7895, 1.5239, 1.3751, 1.2918, 1.3033, 1.3575, 1.4974, 1.7462, 2.1058, 
                   2.5346, 1.9517, 1.6928, 1.5167, 1.4151, 1.4811, 1.5085, 1.6508, 1.9651, 2.4007, 
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
                   2.6426, 2.0985, 1.7652, 1.5725, 1.5013, 1.5117, 1.5696, 1.7590, 2.0974, 2.6277, 
                   2.3855, 1.8823, 1.6098, 1.4296, 1.3479, 1.3460, 1.4163, 1.5876, 1.8571, 2.3425, 
                   2.1387, 1.6624, 1.4318, 1.2421, 1.1684, 1.1631, 1.2357, 1.4022, 1.6428, 2.0694, 
                   1.9977, 1.5819, 1.3391, 1.1809, 1.0772, 1.0844, 1.1630, 1.3466, 1.5614, 1.9761, 
                   2.0020, 1.6119, 1.3563, 1.1829, 1.0383, 1.0254, 1.1676, 1.3429, 1.6006, 1.9698, 
                   1.9516, 1.5490, 1.2987, 1.1454, 0.9918, 1.0030, 1.1174, 1.2952, 1.5290, 1.9018, 
                   1.9810, 1.5917, 1.3365, 1.1775, 1.0811, 1.0787, 1.1665, 1.3303, 1.5803, 1.9428, 
                   2.2492, 1.7706, 1.4953, 1.3209, 1.2315, 1.2383, 1.3140, 1.4991, 1.7378, 2.2379, 
                   2.3527, 1.8696, 1.5769, 1.4114, 1.3310, 1.3207, 1.4062, 1.5526, 1.8511, 2.2891, 
                   2.6574, 2.0695, 1.7520, 1.5556, 1.4423, 1.5055, 1.5567, 1.7745, 2.0385, 2.6939, 
                   2.5207, 2.0354, 1.7088, 1.5536, 1.4635, 1.4613, 1.5506, 1.6975, 2.0307, 2.5052, 
                   2.2888, 1.8415, 1.5730, 1.4048, 1.3343, 1.3274, 1.4024, 1.5546, 1.8091, 2.2676, 
                   2.0547, 1.6192, 1.3909, 1.2422, 1.1457, 1.1539, 1.2273, 1.3781, 1.5948, 1.9885, 
                   1.9038, 1.5528, 1.3200, 1.1674, 1.0701, 1.0697, 1.1501, 1.3177, 1.5286, 1.9266, 
                   1.9485, 1.5775, 1.3464, 1.1806, 1.0408, 1.0251, 1.1688, 1.3332, 1.5631, 1.9195, 
                   1.8687, 1.5160, 1.2881, 1.1355, 0.9943, 0.9968, 1.1106, 1.2807, 1.4950, 1.8437, 
                   1.9269, 1.5585, 1.3196, 1.1732, 1.0775, 1.0767, 1.1585, 1.3186, 1.5370, 1.9165, 
                   2.1805, 1.7288, 1.4875, 1.3124, 1.2357, 1.2260, 1.3118, 1.4633, 1.7245, 2.1204, 
                   2.2725, 1.8230, 1.5488, 1.3889, 1.3126, 1.3191, 1.3846, 1.5349, 1.7836, 2.2242, 
                   2.5606, 1.9746, 1.7189, 1.5041, 1.4535, 1.4433, 1.5504, 1.6730, 2.0061, 2.5014, 
                   2.4181, 2.0345, 1.7266, 1.5428, 1.4748, 1.4781, 1.5638, 1.7001, 2.0866, 2.4295, 
                   2.3253, 1.8566, 1.5744, 1.4169, 1.3447, 1.3194, 1.4216, 1.5316, 1.8444, 2.2172, 
                   2.0000, 1.5910, 1.3964, 1.2289, 1.1447, 1.1640, 1.2120, 1.3749, 1.5996, 1.9149, 
                   1.9414, 1.5486, 1.3128, 1.1544, 1.0874, 1.0573, 1.1737, 1.2805, 1.5610, 1.8014, 
                   1.9563, 1.5723, 1.3403, 1.1800, 1.0272, 1.0296, 1.1564, 1.3203, 1.5733, 1.8448, 
                   1.8763, 1.5118, 1.2951, 1.1195, 1.0142, 0.9886, 1.1301, 1.2650, 1.5120, 1.8050, 
                   1.9305, 1.5282, 1.3066, 1.1593, 1.0576, 1.0688, 1.1478, 1.2922, 1.5241, 1.8262, 
                   2.1487, 1.7264, 1.4553, 1.3196, 1.2176, 1.2235, 1.2946, 1.4441, 1.7054, 2.0581, 
                   2.2972, 1.8134, 1.5741, 1.3765, 1.3232, 1.3136, 1.3914, 1.5390, 1.7971, 2.1610, 
                   2.4391, 1.9658, 1.6153, 1.5036, 1.3973, 1.4087, 1.4826, 1.5842, 1.9262, 2.3323, 
                },
             },
             /* awb para 1 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.6178, 2.0725, 1.7330, 1.5938, 1.4838, 1.5152, 1.5608, 1.7582, 2.0561, 2.5649, 
                   2.2982, 1.8117, 1.5680, 1.3853, 1.3196, 1.3151, 1.3837, 1.5262, 1.8037, 2.2206, 
                   2.1475, 1.6782, 1.4515, 1.2811, 1.1988, 1.1969, 1.2719, 1.4363, 1.6605, 2.0973, 
                   1.9914, 1.5803, 1.3553, 1.1943, 1.0974, 1.1010, 1.1747, 1.3552, 1.5616, 1.9538, 
                   1.9488, 1.5490, 1.3262, 1.1561, 1.0111, 1.0058, 1.1384, 1.3129, 1.5407, 1.9008, 
                   1.9052, 1.5678, 1.3094, 1.1622, 1.0213, 1.0247, 1.1311, 1.3210, 1.5291, 1.9140, 
                   2.0094, 1.5960, 1.3668, 1.2001, 1.1044, 1.1011, 1.2022, 1.3664, 1.5823, 2.0061, 
                   2.1441, 1.6935, 1.4536, 1.2775, 1.2041, 1.2115, 1.2780, 1.4483, 1.6830, 2.0896, 
                   2.3002, 1.8834, 1.5729, 1.4171, 1.3392, 1.3310, 1.4150, 1.5582, 1.8371, 2.3081, 
                   2.7108, 2.0337, 1.7975, 1.5645, 1.5021, 1.5240, 1.6136, 1.7765, 2.0765, 2.6029, 
                   2.5100, 2.0448, 1.7080, 1.5479, 1.4762, 1.4749, 1.5274, 1.7030, 2.0029, 2.4544, 
                   2.2549, 1.7906, 1.5406, 1.3826, 1.3073, 1.3034, 1.3713, 1.5166, 1.7573, 2.2020, 
                   2.0809, 1.6732, 1.4292, 1.2722, 1.1865, 1.1855, 1.2495, 1.4163, 1.6178, 2.0242, 
                   1.9503, 1.5650, 1.3485, 1.1888, 1.0968, 1.0933, 1.1746, 1.3415, 1.5373, 1.9332, 
                   1.8741, 1.5426, 1.3121, 1.1555, 1.0116, 0.9992, 1.1379, 1.3013, 1.5116, 1.8501, 
                   1.8865, 1.5483, 1.3110, 1.1578, 1.0272, 1.0159, 1.1378, 1.2985, 1.5140, 1.8517, 
                   1.9723, 1.5879, 1.3616, 1.1924, 1.1026, 1.0960, 1.1866, 1.3475, 1.5544, 1.9399, 
                   2.0862, 1.6956, 1.4407, 1.2913, 1.2088, 1.2007, 1.2799, 1.4231, 1.6639, 2.0416, 
                   2.3129, 1.8360, 1.5773, 1.3943, 1.3368, 1.3206, 1.3983, 1.5390, 1.7938, 2.2220, 
                   2.5823, 2.0573, 1.7622, 1.5747, 1.4921, 1.4923, 1.6001, 1.6886, 2.0429, 2.4935, 
                   2.3970, 2.0290, 1.6910, 1.5464, 1.4863, 1.4578, 1.5507, 1.6746, 2.0554, 2.4230, 
                   2.2343, 1.7921, 1.5394, 1.3799, 1.3030, 1.3019, 1.3611, 1.5068, 1.7476, 2.1282, 
                   2.0729, 1.6183, 1.4310, 1.2546, 1.1919, 1.1868, 1.2563, 1.4061, 1.6209, 1.9939, 
                   1.9418, 1.5682, 1.3325, 1.1927, 1.0863, 1.0851, 1.1688, 1.3066, 1.5521, 1.8132, 
                   1.8817, 1.5051, 1.3125, 1.1283, 1.0137, 1.0026, 1.1355, 1.3048, 1.5002, 1.8438, 
                   1.8883, 1.5511, 1.3029, 1.1541, 1.0258, 1.0097, 1.1343, 1.2737, 1.5215, 1.7797, 
                   1.9664, 1.5454, 1.3572, 1.1692, 1.0954, 1.0970, 1.1717, 1.3477, 1.5385, 1.8922, 
                   2.0844, 1.6719, 1.4125, 1.2773, 1.1805, 1.1859, 1.2555, 1.4026, 1.6365, 1.9964, 
                   2.2383, 1.8251, 1.5646, 1.3905, 1.3308, 1.3261, 1.3898, 1.5477, 1.7848, 2.1757, 
                   2.5708, 1.9637, 1.7170, 1.5121, 1.4494, 1.4628, 1.5161, 1.6527, 1.9740, 2.4152, 
                },
             },
             /* awb para 2 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.5437, 2.0930, 1.7362, 1.5774, 1.5056, 1.4988, 1.5569, 1.7462, 2.0350, 2.5572, 
                   2.3193, 1.8091, 1.5469, 1.3807, 1.2930, 1.2873, 1.3571, 1.5050, 1.7879, 2.2441, 
                   2.2289, 1.7497, 1.5095, 1.3192, 1.2409, 1.2316, 1.3207, 1.4880, 1.7295, 2.1740, 
                   2.0309, 1.6170, 1.3649, 1.2042, 1.1000, 1.1057, 1.1814, 1.3604, 1.5956, 1.9832, 
                   1.9121, 1.5302, 1.2803, 1.1253, 0.9866, 0.9663, 1.1100, 1.2781, 1.5104, 1.8724, 
                   2.0210, 1.6281, 1.3638, 1.1902, 1.0448, 1.0509, 1.1730, 1.3661, 1.5938, 2.0018, 
                   2.0518, 1.6289, 1.3759, 1.2097, 1.1106, 1.1139, 1.2148, 1.3776, 1.6247, 2.0302, 
                   2.1217, 1.6803, 1.4213, 1.2617, 1.1615, 1.1800, 1.2400, 1.4206, 1.6344, 2.1174, 
                   2.3864, 1.9209, 1.6216, 1.4338, 1.3741, 1.3572, 1.4594, 1.6073, 1.9093, 2.3748, 
                   2.8206, 2.1517, 1.8483, 1.6565, 1.5208, 1.5832, 1.6456, 1.8385, 2.1593, 2.7837, 
                   2.5244, 2.0206, 1.7135, 1.5344, 1.4634, 1.4577, 1.5168, 1.6723, 2.0008, 2.4321, 
                   2.1780, 1.7783, 1.5026, 1.3658, 1.2759, 1.2812, 1.3489, 1.4892, 1.7311, 2.1602, 
                   2.1851, 1.7187, 1.5003, 1.3131, 1.2368, 1.2359, 1.2992, 1.4767, 1.6849, 2.1088, 
                   1.9469, 1.5986, 1.3540, 1.2011, 1.1045, 1.0962, 1.1933, 1.3473, 1.5681, 1.9617, 
                   1.8384, 1.4919, 1.2779, 1.1180, 0.9837, 0.9785, 1.0985, 1.2727, 1.4763, 1.7910, 
                   2.0043, 1.5932, 1.3689, 1.1939, 1.0603, 1.0400, 1.1911, 1.3390, 1.5743, 1.9602, 
                   1.9779, 1.6204, 1.3718, 1.2121, 1.1223, 1.1208, 1.2048, 1.3716, 1.5986, 1.9545, 
                   2.0623, 1.6333, 1.4033, 1.2413, 1.1640, 1.1592, 1.2408, 1.3793, 1.6183, 2.0108, 
                   2.3403, 1.8885, 1.6078, 1.4366, 1.3715, 1.3520, 1.4410, 1.5826, 1.8464, 2.2924, 
                   2.6826, 2.1230, 1.8198, 1.6182, 1.5317, 1.5707, 1.6340, 1.7782, 2.1316, 2.6286, 
                   2.3909, 1.9621, 1.6680, 1.5222, 1.4512, 1.4495, 1.5344, 1.6604, 2.0129, 2.3959, 
                   2.2009, 1.7229, 1.5225, 1.3324, 1.2709, 1.2690, 1.3313, 1.4633, 1.7236, 2.0718, 
                   2.1538, 1.7054, 1.4795, 1.3210, 1.2387, 1.2342, 1.3151, 1.4591, 1.7024, 2.0572, 
                   1.9591, 1.5701, 1.3525, 1.1904, 1.1024, 1.0966, 1.1765, 1.3262, 1.5615, 1.8408, 
                   1.8408, 1.4672, 1.2681, 1.1105, 0.9782, 0.9630, 1.1108, 1.2375, 1.4737, 1.7345, 
                   1.9299, 1.6186, 1.3477, 1.1971, 1.0689, 1.0537, 1.1814, 1.3399, 1.5740, 1.9103, 
                   1.9909, 1.5873, 1.3688, 1.2096, 1.1075, 1.1101, 1.2067, 1.3439, 1.5878, 1.8959, 
                   1.9670, 1.6163, 1.3705, 1.2359, 1.1492, 1.1556, 1.2064, 1.3695, 1.5595, 1.9167, 
                   2.3489, 1.8547, 1.6072, 1.4189, 1.3610, 1.3470, 1.4384, 1.5632, 1.8580, 2.2290, 
                   2.5916, 2.0605, 1.7586, 1.6049, 1.4839, 1.5281, 1.5655, 1.7431, 2.0162, 2.4816, 
                },
             },
             /* awb para 3 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.6744, 2.1369, 1.8116, 1.6223, 1.5441, 1.5669, 1.5956, 1.8435, 2.1069, 2.7364, 
                   2.3391, 1.8840, 1.6004, 1.4302, 1.3427, 1.3388, 1.4305, 1.5670, 1.8763, 2.3156, 
                   2.1298, 1.6630, 1.4283, 1.2521, 1.1708, 1.1674, 1.2403, 1.4115, 1.6288, 2.0711, 
                   2.0378, 1.6109, 1.3783, 1.2074, 1.1036, 1.1172, 1.1910, 1.3626, 1.6046, 1.9927, 
                   1.9713, 1.6248, 1.3453, 1.1929, 1.0426, 1.0269, 1.1760, 1.3399, 1.5927, 1.9449, 
                   1.9136, 1.5418, 1.2990, 1.1278, 0.9963, 0.9927, 1.1099, 1.2950, 1.5083, 1.8904, 
                   2.0193, 1.6371, 1.3739, 1.2052, 1.1130, 1.1004, 1.2069, 1.3524, 1.6173, 1.9735, 
                   2.2484, 1.7580, 1.5142, 1.3228, 1.2423, 1.2387, 1.3226, 1.4954, 1.7497, 2.2080, 
                   2.3356, 1.8641, 1.5687, 1.3949, 1.3180, 1.3122, 1.3843, 1.5477, 1.8149, 2.2709, 
                   2.7065, 2.0927, 1.8006, 1.5803, 1.5021, 1.5215, 1.6186, 1.7415, 2.1513, 2.6065, 
                   2.5827, 2.0645, 1.7688, 1.5897, 1.5079, 1.5075, 1.5871, 1.7614, 2.0612, 2.5457, 
                   2.3087, 1.8402, 1.5728, 1.4144, 1.3236, 1.3321, 1.3974, 1.5382, 1.7985, 2.2423, 
                   2.0358, 1.6120, 1.4015, 1.2398, 1.1518, 1.1529, 1.2324, 1.3794, 1.5849, 1.9821, 
                   1.9729, 1.5933, 1.3645, 1.2055, 1.1108, 1.1018, 1.1912, 1.3453, 1.5645, 1.9586, 
                   1.9490, 1.5824, 1.3540, 1.1858, 1.0381, 1.0310, 1.1721, 1.3392, 1.5570, 1.9227, 
                   1.8750, 1.5071, 1.2843, 1.1304, 0.9978, 0.9874, 1.1156, 1.2655, 1.4828, 1.8313, 
                   1.9522, 1.6032, 1.3568, 1.2024, 1.1080, 1.1078, 1.1922, 1.3503, 1.5730, 1.9463, 
                   2.2032, 1.7368, 1.4998, 1.3303, 1.2395, 1.2358, 1.3199, 1.4767, 1.7079, 2.1493, 
                   2.2353, 1.8062, 1.5337, 1.3758, 1.3106, 1.3004, 1.3690, 1.5120, 1.7630, 2.1931, 
                   2.6088, 2.0258, 1.7539, 1.5732, 1.4815, 1.4845, 1.5961, 1.6995, 2.0340, 2.5338, 
                   2.4747, 2.1069, 1.7354, 1.6005, 1.5023, 1.5167, 1.5674, 1.7404, 2.0939, 2.4996, 
                   2.2840, 1.8141, 1.5864, 1.4056, 1.3398, 1.3194, 1.4097, 1.5421, 1.7928, 2.1928, 
                   1.9830, 1.5989, 1.3758, 1.2264, 1.1490, 1.1564, 1.2116, 1.3746, 1.5812, 1.9211, 
                   1.9447, 1.5798, 1.3529, 1.2046, 1.1066, 1.1041, 1.1886, 1.3472, 1.5641, 1.8992, 
                   1.9514, 1.5704, 1.3523, 1.1792, 1.0502, 1.0323, 1.1754, 1.3254, 1.5662, 1.8602, 
                   1.7922, 1.4944, 1.2646, 1.1180, 0.9928, 0.9921, 1.1057, 1.2766, 1.4559, 1.8045, 
                   1.9446, 1.5784, 1.3453, 1.2046, 1.0926, 1.1132, 1.1819, 1.3259, 1.5675, 1.8564, 
                   2.1408, 1.7052, 1.4727, 1.3106, 1.2270, 1.2331, 1.3084, 1.4678, 1.6853, 2.0863, 
                   2.2042, 1.7901, 1.5240, 1.3754, 1.2919, 1.3035, 1.3575, 1.4978, 1.7463, 2.1068, 
                   2.5348, 1.9518, 1.6926, 1.5168, 1.4150, 1.4809, 1.5085, 1.6510, 1.9653, 2.4028, 
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
             200,
             250,
          },
          /* aec trigger 3 */
          {
             260,
             370,
          },
          /* aec trigger 4 */
          {
             380,
             430,
          },
          /* aec trigger 5 */
          {
             440,
             460,
          },
          /* aec trigger 6 */
          {
             490,
             500,
          },
       },
       /* isp_sub_module_drc_pra pra[MIDDLE_TRIGGER_COUNT] */
       {
           /* para 0*/
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
              155,
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
                   8148,8148,8104,8061,8017,7974,7932,7889,7847,7805,7763,7722,7680,7639,7598,7558,7517,7477,7437,7397,7357,7318,7279,7240,7201,
                   7162,7124,7086,7048,7010,6973,6935,6898,6861,6824,6788,6752,6715,6679,6644,6608,6573,6537,6502,6468,6433,6399,6364,6330,6296,
                   6263,6229,6196,6162,6129,6097,6064,6032,5999,5967,5935,5903,5872,5840,5809,5778,5747,5716,5686,5655,5625,5595,5565,5535,5505,
                   5476,5446,5417,5388,5359,5331,5302,5274,5246,5217,5189,5162,5134,5107,5079,5052,5025,4998,4971,4945,4918,4892,4866,4840,4814,
                   4788,4762,4737,4711,4686,4661,4636,4611,4586,4562,4537,4513,4489,4465,4441,4417,4394,4370,4347,4323,4300,4277,4254,4231,4209,
                   4186,4164,4142,4119,4097,4075,4054,4032,4010,3989,3967,3946,3925,3904,3883,3862,3842,3821,3801,3780,3760,3740,3720,3700,3680,
                   3660,3641,3621,3602,3583,3563,3544,3525,3506,3488,3469,3450,3432,3413,3395,3377,3359,3341,3323,3305,3288,3270,3252,3235,3218,
                   3200,3183,3166,3149,3132,3116,3099,3082,3066,3049,3033,3017,3001,2985,2969,2953,2937,2921,2906,2890,2874,2859,2844,2829,2813,
                   2798,2783,2768,2754,2739,2724,2710,2695,2681,2666,2652,2638,2624,2610,2596,2582,2568,2554,2540,2527,2513,2500,2486,2473,2460,
                   2447,2434,2421,2408,2395,2382,2369,2356,2344,2331,2319,2306,2294,2282,2269,2257,2245,2233,2221,2209,2198,2186,2174,2162,2151,
                   2139,2128,2116,2105,2094,2083,2071,
              },
              /* drc_high_LUT_point */
              {
                   7517,7517,6898,6330,5808,5330,4891,4488,4119,3779,3468,3183,2920,2680,2459,2257,2071,1900,1744,1600,1468,1347,1236,1134,1041,
                    955, 877, 804, 738, 677, 621, 570, 523, 480, 440, 404, 371, 340, 312, 286, 263, 241, 221, 203, 186, 171, 157, 144, 132, 121,
                    111, 102,  93,  86,  79,  72,  66,  61,  56,  51,  47,  43,  39,  36,  33,  30,  28,  25,  23,  21,  19,  18,  16,  15,  14,
                     13,  11,  10,  10,   9,   8,   7,   7,   6,   6,   5,   5,   4,   4,   3,   3,   3,   3,   2,   2,   2,   2,   1,   1,   1,
                      1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,
              },
              /* saturation */
              155,
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
              1,
              /* k2 */
              22,
              /* k3 */
              1,
           },
           /* para 3*/
           {
              /* drc_low_LUT_point */
              {
                   8142,8142,8092,8043,7994,7945,7897,7849,7801,7754,7707,7660,7613,7567,7521,7475,7429,7384,7339,7295,7250,7206,7162,7119,7076,
                   7032,6990,6947,6905,6863,6821,6780,6738,6697,6657,6616,6576,6536,6496,6457,6417,6378,6339,6301,6263,6224,6187,6149,6112,6074,
                   6037,6001,5964,5928,5892,5856,5820,5785,5750,5715,5680,5645,5611,5577,5543,5509,5476,5442,5409,5376,5344,5311,5279,5247,5215,
                   5183,5152,5120,5089,5058,5027,4997,4966,4936,4906,4876,4847,4817,4788,4759,4730,4701,4672,4644,4616,4588,4560,4532,4504,4477,
                   4450,4423,4396,4369,4342,4316,4290,4264,4238,4212,4186,4161,4136,4110,4085,4060,4036,4011,3987,3963,3938,3914,3891,3867,3843,
                   3820,3797,3774,3751,3728,3705,3683,3660,3638,3616,3594,3572,3550,3529,3507,3486,3465,3444,3423,3402,3381,3361,3340,3320,3300,
                   3280,3260,3240,3220,3200,3181,3162,3142,3123,3104,3085,3067,3048,3029,3011,2993,2974,2956,2938,2920,2903,2885,2867,2850,2833,
                   2815,2798,2781,2764,2748,2731,2714,2698,2681,2665,2649,2633,2617,2601,2585,2569,2554,2538,2523,2507,2492,2477,2462,2447,2432,
                   2417,2402,2388,2373,2359,2344,2330,2316,2302,2288,2274,2260,2246,2233,2219,2206,2192,2179,2166,2152,2139,2126,2113,2100,2088,
                   2075,2062,2050,2037,2025,2013,2000,1988,1976,1964,1952,1940,1928,1917,1905,1893,1882,1870,1859,1848,1837,1825,1814,1803,1792,
                   1781,1771,1760,1749,1738,1728,1717,
              },
              /* drc_high_LUT_point */
              {
                   7429,7429,6738,6111,5542,5027,4559,4135,3750,3401,3085,2798,2537,2301,2087,1893,1717,1557,1412,1281,1161,1053, 955, 866, 786,
                    713, 646, 586, 531, 482, 437, 396, 359, 326, 296, 268, 243, 220, 200, 181, 164, 149, 135, 122, 111, 101,  91,  83,  75,  68,
                     62,  56,  51,  46,  41,  38,  34,  31,  28,  25,  23,  21,  19,  17,  15,  14,  13,  11,  10,   9,   8,   7,   7,   6,   5,
                      5,   4,   4,   4,   3,   3,   3,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,
              },
              /* saturation */
              155,
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
              1,
              /* k2 */
              25,
              /* k3 */
              1,
           },
           /* para 4*/
           {
              /* drc_low_LUT_point */
              {
                   8142,8142,8092,8043,7994,7945,7897,7849,7801,7754,7707,7660,7613,7567,7521,7475,7429,7384,7339,7295,7250,7206,7162,7119,7076,
                   7032,6990,6947,6905,6863,6821,6780,6738,6697,6657,6616,6576,6536,6496,6457,6417,6378,6339,6301,6263,6224,6187,6149,6112,6074,
                   6037,6001,5964,5928,5892,5856,5820,5785,5750,5715,5680,5645,5611,5577,5543,5509,5476,5442,5409,5376,5344,5311,5279,5247,5215,
                   5183,5152,5120,5089,5058,5027,4997,4966,4936,4906,4876,4847,4817,4788,4759,4730,4701,4672,4644,4616,4588,4560,4532,4504,4477,
                   4450,4423,4396,4369,4342,4316,4290,4264,4238,4212,4186,4161,4136,4110,4085,4060,4036,4011,3987,3963,3938,3914,3891,3867,3843,
                   3820,3797,3774,3751,3728,3705,3683,3660,3638,3616,3594,3572,3550,3529,3507,3486,3465,3444,3423,3402,3381,3361,3340,3320,3300,
                   3280,3260,3240,3220,3200,3181,3162,3142,3123,3104,3085,3067,3048,3029,3011,2993,2974,2956,2938,2920,2903,2885,2867,2850,2833,
                   2815,2798,2781,2764,2748,2731,2714,2698,2681,2665,2649,2633,2617,2601,2585,2569,2554,2538,2523,2507,2492,2477,2462,2447,2432,
                   2417,2402,2388,2373,2359,2344,2330,2316,2302,2288,2274,2260,2246,2233,2219,2206,2192,2179,2166,2152,2139,2126,2113,2100,2088,
                   2075,2062,2050,2037,2025,2013,2000,1988,1976,1964,1952,1940,1928,1917,1905,1893,1882,1870,1859,1848,1837,1825,1814,1803,1792,
                   1781,1771,1760,1749,1738,1728,1717,
              },
              /* drc_high_LUT_point */
              {
                   7429,7429,6738,6111,5542,5027,4559,4135,3750,3401,3085,2798,2537,2301,2087,1893,1717,1557,1412,1281,1161,1053, 955, 866, 786,
                    713, 646, 586, 531, 482, 437, 396, 359, 326, 296, 268, 243, 220, 200, 181, 164, 149, 135, 122, 111, 101,  91,  83,  75,  68,
                     62,  56,  51,  46,  41,  38,  34,  31,  28,  25,  23,  21,  19,  17,  15,  14,  13,  11,  10,   9,   8,   7,   7,   6,   5,
                      5,   4,   4,   4,   3,   3,   3,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,
              },
              /* saturation */
              155,
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
              1,
              /* k2 */
              25,
              /* k3 */
              1,
           },
           /* para 5*/
           {
              /* drc_low_LUT_point */
              {
                   7607,7607,7264,6983,6738,6519,6320,6136,5965,5804,5653,5510,5375,5246,5123,5005,4893,4784,4681,4581,4484,4391,4302,4215,4131,
                   4050,3971,3895,3821,3749,3680,3612,3546,3482,3420,3359,3300,3242,3186,3131,3078,3026,2975,2925,2877,2830,2783,2738,2694,2651,
                   2609,2567,2527,2487,2448,2410,2373,2337,2301,2266,2232,2199,2166,2134,2102,2071,2041,2011,1982,1953,1925,1897,1870,1844,1817,
                   1792,1767,1742,1717,1694,1670,1647,1625,1602,1580,1559,1538,1517,1497,1477,1457,1437,1418,1400,1381,1363,1345,1327,1310,1293,
                   1276,1260,1244,1228,1212,1197,1181,1166,1152,1137,1123,1109,1095,1081,1068,1054,1041,1028,1016,1003, 991, 979, 967, 955, 944,
                    932, 921, 910, 899, 888, 877, 867, 857, 846, 836, 826, 817, 807, 798, 788, 779, 770, 761, 752, 743, 735, 726, 718, 709, 701,
                    693, 685, 678, 670, 662, 655, 647, 640, 633, 626, 618, 612, 605, 598, 591, 585, 578, 572, 565, 559, 553, 547, 541, 535, 529,
                    523, 517, 512, 506, 501, 495, 490, 485, 479, 474, 469, 464, 459, 454, 449, 444, 440, 435, 430, 426, 421, 417, 412, 408, 404,
                    399, 395, 391, 387, 383, 379, 375, 371, 367, 363, 360, 356, 352, 349, 345, 341, 338, 334, 331, 328, 324, 321, 318, 314, 311,
                    308, 305, 302, 299, 296, 293, 290, 287, 284, 281, 278, 276, 273, 270, 267, 265, 262, 259, 257, 254, 252, 249, 247, 244, 242,
                    240, 237, 235, 233, 230, 228, 226,
              },
              /* drc_high_LUT_point */
              {
                   4892,4892,3546,2693,2102,1670,1345,1094, 898, 743, 618, 517, 435, 367, 311, 265, 226, 193, 166, 142, 123, 106,  92,  80,  69,
                     60,  52,  46,  40,  35,  31,  27,  24,  21,  18,  16,  14,  12,  11,  10,   8,   7,   7,   6,   5,   4,   4,   3,   3,   3,
                      2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
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
              155,
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
              1,
              /* k2 */
              25,
              /* k3 */
              0.7,
           },
           /* para 6*/
           {
              /* drc_low_LUT_point */
              {
                   16320,16320,16256,16193,16130,16067,16004,15942,15880,15818,15756,15695,15633,15572,15512,15451,15391,15331,15271,15212,15153,15093,15035,14976,14918,
                   14859,14802,14744,14686,14629,14572,14515,14459,14402,14346,14290,14235,14179,14124,14069,14014,13959,13905,13851,13797,13743,13689,13636,13583,13530,
                   13477,13425,13372,13320,13268,13217,13165,13114,13063,13012,12961,12911,12860,12810,12760,12710,12661,12612,12562,12513,12465,12416,12368,12319,12271,
                   12224,12176,12128,12081,12034,11987,11940,11894,11848,11801,11755,11710,11664,11618,11573,11528,11483,11438,11394,11349,11305,11261,11217,11173,11130,
                   11087,11043,11000,10957,10915,10872,10830,10788,10745,10704,10662,10620,10579,10538,10497,10456,10415,10374,10334,10294,10254,10214,10174,10134,10095,
                   10055,10016,9977,9938,9899,9861,9822,9784,9746,9708,9670,9632,9595,9557,9520,9483,9446,9409,9373,9336,9300,9263,9227,9191,9156,
                   9120,9084,9049,9014,8979,8944,8909,8874,8839,8805,8771,8736,8702,8668,8635,8601,8567,8534,8501,8468,8435,8402,8369,8336,8304,
                   8272,8239,8207,8175,8143,8112,8080,8048,8017,7986,7955,7924,7893,7862,7831,7801,7770,7740,7710,7680,7650,7620,7590,7561,7531,
                   7502,7473,7444,7415,7386,7357,7328,7300,7271,7243,7215,7187,7159,7131,7103,7075,7048,7020,6993,6966,6938,6911,6884,6858,6831,
                   6804,6778,6751,6725,6699,6673,6647,6621,6595,6569,6544,6518,6493,6467,6442,6417,6392,6367,6342,6318,6293,6268,6244,6220,6195,
                   6171,6147,6123,6099,6076,6052,6028,
              },
              /* drc_high_LUT_point */
              {
                   15391,15391,14458,13582,12759,11986,11260,10578,9937,9335,8769,8238,7739,7270,6829,6416,6027,5662,5319,4996,4694,4409,4142,3891,3655,
                   3434,3226,3030,2847,2674,2512,2360,2217,2082,1956,1838,1726,1622,1523,1431,1344,1263,1186,1114,1047, 983, 924, 868, 815, 766,
                    719, 676, 635, 596, 560, 526, 494, 464, 436, 410, 385, 361, 340, 319, 300, 281, 264, 248, 233, 219, 206, 193, 182, 170, 160,
                    150, 141, 133, 125, 117, 110, 103,  97,  91,  85,  80,  75,  71,  66,  62,  59,  55,  52,  48,  46,  43,  40,  38,  35,  33,
                     31,  29,  27,  26,  24,  23,  21,  20,  19,  18,  16,  15,  14,  14,  13,  12,  11,  10,  10,   9,   9,   8,   7,   7,   7,
                      6,   6,   5,   5,   5,   4,   4,   4,   4,   3,   3,   3,   3,   2,   2,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,
                      1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                      0,   0,   0,   0,   0,   0,   0,
              },
              /* saturation */
              155,
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
              2,
              /* k2 */
              16,
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
              1,//luma_detect_en
              100,//th1_very_light
              255,//th2_y_max
              1,//rgb_diff_detect_en
              90,//th1_rgb_diff
              110,//th2_rgb_diff
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
              100,//th1_very_light
              255,//th2_y_max
              1,//rgb_diff_detect_en
              60,//th1_rgb_diff
              90,//th2_rgb_diff
              1,//edge_detect_en
              1500,//th1_edge
              2000,//th2_edge
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
              0,//dpp_correct
              1,//luma_detect_en
              0,//th1_very_light
              0,//th2_y_max
              1,//rgb_diff_detect_en
              0,//th1_rgb_diff
              0,//th2_rgb_diff
              1,//edge_detect_en
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
              0,//dpp_correct
              1,//luma_detect_en
              0,//th1_very_light
              0,//th2_y_max
              1,//rgb_diff_detect_en
              0,//th1_rgb_diff
              0,//th2_rgb_diff
              1,//edge_detect_en
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
             310,
          },
          /* aec trigger 1 */
          {
             320,
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
                1.046,0.354,-0.4,
                -0.543,1.812,-0.269,
                -0.254,-0.901001,2.155,
             },
             /* awb para 1 */
             {
                1.496,-0.338,-0.158,
                -0.424,1.681,-0.257,
                -0.063,-0.72,1.783,
             },
             /* awb para 2 */
             {
                1.4849,-0.1289,-0.356,
                -0.268,1.6979,-0.4299,
                -0.025,-0.709,1.734,
             },
             /* awb para 3 */
             {
                1.5257,-0.3927,-0.133,
                -0.3115,1.6782,-0.3667,
                -0.0177,-0.6402,1.6579,
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
                1.4849,-0.4789,-0.006,
                -0.168,1.1979,-0.0299,
                -0.025,-0.709,1.734,
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
             160,
          },
          /* aec trigger 1 */
          {
             180,
             285,
          },
          /* aec trigger 2 */
          {
             305,
             390,
          },
          /* aec trigger 3 */
          {
             405,
             460,
          },
          /* aec trigger 4 */
          {
             480,
             550,
          },
       },
       4096,//gamma_lut_size
       /* isp_sub_module_gamma_lut_pra pra[MIDDLE_TRIGGER_COUNT] */
       {
          /* aec trigger 0 */
          {
                   0,   2,   4,   6,   9,  11,  13,  16,  18,  20,  22,  24,  27,  29,  31,  33,  35,  38,  40,  42,  44,  46,  48,  51,  53,  55,  57,  59,  61,  63,  62,  66,
                  70,  74,  78,  82,  86,  90,  94,  98, 102, 106, 110, 114, 118, 122, 126, 130, 134, 138, 142, 146, 150, 154, 158, 162, 165, 169, 173, 177, 181, 185, 189, 193,
                 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 237, 240, 244, 248, 252, 256, 260, 264, 268, 272, 276, 280, 284, 288, 292, 296, 300, 304, 309, 313, 318, 322,
                 326, 331, 335, 339, 344, 348, 353, 357, 361, 366, 370, 374, 379, 383, 387, 392, 396, 400, 405, 409, 413, 418, 422, 426, 431, 436, 440, 445, 449, 453, 458, 462,
                 466, 470, 475, 479, 483, 487, 491, 495, 499, 503, 507, 510, 514, 518, 522, 525, 529, 533, 536, 540, 544, 547, 551, 554, 557, 561, 564, 567, 571, 574, 577, 580,
                 582, 585, 589, 593, 597, 601, 604, 608, 612, 616, 619, 623, 627, 631, 634, 638, 642, 645, 649, 653, 656, 660, 664, 667, 671, 675, 678, 682, 685, 689, 693, 696,
                 700, 703, 707, 710, 714, 717, 721, 724, 728, 731, 735, 738, 741, 745, 748, 752, 755, 758, 762, 765, 769, 772, 775, 779, 782, 785, 789, 792, 795, 798, 801, 804,
                 807, 810, 813, 816, 819, 822, 826, 829, 832, 835, 838, 841, 844, 847, 850, 853, 856, 860, 863, 866, 869, 872, 875, 878, 881, 884, 887, 890, 893, 896, 899, 903,
                 906, 909, 912, 915, 918, 921, 924, 927, 930, 933, 936, 939, 942, 945, 948, 951, 954, 957, 960, 963, 966, 969, 972, 975, 978, 981, 984, 987, 990, 993, 996, 999,
                1002,1006,1009,1011,1014,1017,1020,1023,1026,1029,1032,1035,1038,1041,1044,1047,1050,1053,1056,1059,1062,1065,1068,1071,1074,1077,1080,1083,1086,1089,1092,1095,
                1098,1101,1104,1107,1110,1112,1115,1118,1121,1124,1127,1130,1133,1136,1139,1142,1145,1148,1151,1154,1156,1159,1162,1165,1168,1171,1174,1177,1180,1183,1186,1188,
                1191,1194,1197,1200,1203,1206,1209,1212,1214,1217,1220,1223,1226,1229,1232,1235,1237,1240,1243,1246,1249,1252,1255,1258,1260,1263,1266,1269,1272,1275,1277,1280,
                1283,1286,1289,1292,1295,1297,1300,1303,1306,1309,1312,1314,1317,1320,1323,1326,1329,1332,1335,1338,1341,1344,1348,1351,1354,1357,1360,1363,1366,1370,1373,1376,
                1379,1382,1385,1388,1391,1394,1397,1401,1404,1407,1410,1413,1416,1419,1422,1425,1428,1431,1434,1437,1440,1443,1446,1449,1452,1455,1457,1460,1463,1466,1469,1472,
                1475,1478,1481,1484,1487,1489,1492,1495,1498,1501,1504,1507,1509,1512,1515,1518,1521,1523,1526,1529,1532,1535,1537,1540,1543,1546,1548,1551,1554,1557,1559,1562,
                1565,1567,1570,1573,1576,1578,1581,1584,1586,1589,1591,1594,1597,1599,1602,1605,1607,1610,1612,1615,1618,1620,1623,1625,1628,1630,1633,1635,1638,1641,1643,1646,
                1648,1651,1653,1656,1658,1660,1663,1665,1668,1670,1673,1675,1678,1680,1683,1685,1687,1690,1692,1695,1697,1699,1702,1704,1706,1709,1711,1713,1716,1718,1720,1723,
                1725,1727,1730,1732,1734,1736,1739,1741,1743,1745,1748,1750,1752,1754,1757,1759,1761,1763,1765,1768,1770,1772,1774,1776,1779,1781,1783,1785,1787,1789,1791,1793,
                1796,1798,1800,1802,1804,1806,1808,1810,1812,1814,1816,1818,1820,1822,1824,1826,1828,1830,1832,1834,1836,1838,1840,1842,1844,1846,1848,1850,1852,1854,1856,1858,
                1860,1862,1864,1865,1867,1869,1871,1873,1875,1877,1878,1880,1882,1884,1886,1888,1889,1891,1893,1895,1897,1898,1900,1902,1904,1905,1907,1909,1911,1912,1914,1916,
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
          /* aec trigger 1 */
          {
                   0,   2,   4,   6,   9,  11,  13,  16,  18,  20,  22,  24,  27,  29,  31,  33,  35,  38,  40,  42,  44,  46,  48,  51,  53,  55,  57,  59,  61,  63,  62,  65,
                  68,  71,  74,  77,  80,  83,  86,  88,  91,  94,  97, 100, 103, 105, 108, 111, 113, 116, 119, 121, 124, 127, 129, 132, 134, 137, 139, 141, 143, 145, 148, 150,
                 153, 155, 158, 161, 164, 166, 169, 172, 175, 177, 180, 183, 186, 189, 192, 195, 198, 201, 204, 207, 210, 213, 216, 220, 223, 226, 229, 232, 236, 239, 242, 246,
                 249, 253, 256, 259, 263, 266, 270, 274, 277, 281, 284, 288, 292, 295, 299, 303, 307, 310, 314, 318, 322, 326, 330, 334, 336, 340, 344, 347, 351, 354, 358, 362,
                 365, 369, 373, 376, 380, 384, 387, 391, 395, 398, 402, 406, 410, 414, 417, 421, 425, 429, 433, 436, 440, 444, 448, 452, 456, 460, 464, 468, 471, 475, 479, 483,
                 483, 487, 490, 494, 497, 501, 504, 508, 511, 514, 518, 521, 525, 528, 531, 535, 538, 542, 545, 548, 552, 555, 558, 562, 565, 568, 571, 575, 578, 581, 585, 588,
                 591, 594, 598, 601, 604, 607, 611, 614, 617, 620, 624, 627, 630, 633, 636, 640, 643, 646, 649, 652, 655, 658, 662, 665, 668, 671, 674, 677, 680, 683, 687, 690,
                 693, 696, 699, 702, 705, 708, 711, 714, 717, 720, 723, 726, 729, 732, 735, 738, 741, 744, 747, 750, 753, 756, 759, 762, 765, 768, 770, 773, 776, 779, 782, 785,
                 788, 791, 794, 796, 799, 802, 805, 808, 811, 813, 816, 819, 822, 825, 828, 830, 833, 836, 839, 841, 844, 847, 850, 852, 855, 858, 861, 863, 866, 869, 871, 874,
                 877, 879, 882, 885, 887, 890, 893, 895, 898, 901, 903, 906, 909, 912, 914, 917, 920, 922, 925, 927, 930, 933, 935, 938, 941, 943, 946, 949, 951, 954, 957, 959,
                 962, 965, 967, 970, 973, 975, 978, 981, 983, 986, 989, 991, 994, 997, 999,1002,1004,1007,1010,1012,1015,1018,1020,1023,1026,1028,1031,1034,1036,1039,1041,1044,
                1047,1049,1052,1055,1057,1060,1063,1065,1068,1070,1073,1076,1078,1081,1084,1086,1089,1091,1094,1097,1099,1102,1105,1107,1110,1112,1115,1118,1120,1123,1125,1128,
                1131,1133,1136,1139,1141,1144,1146,1149,1152,1154,1157,1159,1162,1165,1167,1170,1173,1175,1177,1179,1181,1183,1185,1187,1189,1192,1194,1196,1198,1200,1202,1204,
                1206,1208,1210,1212,1214,1216,1219,1221,1223,1225,1227,1229,1231,1233,1235,1237,1239,1241,1243,1245,1247,1249,1251,1253,1255,1257,1259,1261,1263,1265,1267,1269,
                1271,1273,1275,1277,1279,1281,1283,1285,1287,1289,1290,1292,1294,1296,1298,1300,1302,1304,1306,1308,1310,1312,1313,1315,1317,1319,1321,1323,1325,1327,1329,1330,
                1332,1334,1336,1338,1340,1342,1343,1345,1347,1349,1351,1353,1354,1356,1358,1360,1362,1364,1365,1367,1369,1371,1373,1374,1376,1378,1380,1381,1383,1385,1387,1389,
                1390,1392,1394,1396,1397,1399,1401,1403,1403,1405,1407,1409,1411,1412,1414,1416,1417,1419,1421,1423,1424,1426,1428,1430,1431,1433,1435,1436,1438,1440,1441,1443,
                1445,1446,1448,1450,1451,1453,1455,1456,1458,1460,1461,1463,1465,1466,1468,1469,1471,1473,1474,1476,1478,1479,1481,1482,1484,1486,1487,1489,1490,1492,1493,1495,
                1497,1498,1500,1501,1503,1504,1506,1507,1509,1510,1512,1514,1515,1517,1518,1520,1521,1523,1524,1526,1527,1529,1530,1532,1533,1534,1536,1537,1539,1540,1542,1543,
                1545,1546,1548,1549,1550,1552,1553,1555,1556,1558,1559,1560,1562,1563,1565,1566,1567,1569,1570,1572,1573,1574,1576,1577,1578,1580,1581,1583,1584,1585,1587,1588,
                1587,1589,1590,1591,1592,1594,1595,1596,1597,1598,1600,1601,1602,1603,1605,1606,1607,1608,1609,1611,1612,1613,1614,1616,1617,1618,1619,1620,1622,1623,1624,1625,
                1626,1628,1629,1630,1631,1632,1634,1635,1636,1637,1638,1640,1641,1642,1643,1644,1646,1647,1648,1649,1650,1652,1653,1654,1655,1656,1658,1659,1660,1661,1662,1663,
                1665,1666,1667,1668,1669,1671,1672,1673,1674,1675,1676,1678,1679,1680,1681,1682,1684,1685,1686,1687,1688,1689,1691,1692,1693,1694,1695,1696,1698,1699,1700,1701,
                1702,1703,1705,1706,1707,1708,1709,1710,1712,1713,1714,1715,1716,1717,1719,1720,1721,1722,1723,1724,1726,1727,1728,1729,1730,1731,1732,1734,1735,1736,1737,1738,
                1739,1740,1742,1743,1744,1745,1746,1747,1748,1750,1751,1752,1753,1754,1755,1756,1758,1759,1760,1761,1762,1763,1764,1766,1767,1768,1769,1770,1771,1772,1773,1775,
                1776,1777,1778,1779,1780,1781,1782,1784,1785,1786,1787,1788,1789,1790,1791,1793,1794,1795,1796,1797,1798,1799,1800,1802,1803,1804,1805,1806,1807,1808,1809,1810,
                1812,1813,1814,1815,1816,1817,1818,1819,1820,1821,1823,1824,1825,1826,1827,1828,1829,1830,1831,1832,1834,1835,1836,1837,1838,1839,1840,1841,1842,1843,1844,1846,
                1847,1848,1849,1850,1851,1852,1853,1854,1855,1856,1858,1859,1860,1861,1862,1863,1864,1865,1866,1867,1868,1869,1870,1872,1873,1874,1875,1876,1877,1878,1879,1880,
                1881,1882,1883,1884,1886,1887,1888,1889,1890,1891,1892,1893,1894,1895,1896,1897,1898,1899,1900,1901,1902,1903,1904,1905,1906,1907,1908,1909,1910,1911,1911,1912,
                1913,1914,1915,1916,1917,1918,1919,1920,1921,1922,1923,1924,1925,1926,1927,1928,1929,1930,1931,1932,1933,1934,1934,1935,1936,1937,1938,1939,1940,1941,1942,1943,
                1944,1945,1946,1947,1948,1949,1950,1951,1952,1953,1954,1955,1956,1957,1957,1958,1959,1960,1961,1962,1963,1964,1965,1966,1967,1968,1969,1970,1971,1972,1973,1974,
                1975,1976,1977,1978,1979,1979,1980,1981,1982,1983,1984,1985,1986,1987,1988,1989,1990,1991,1992,1993,1994,1995,1996,1997,1998,1999,2000,2001,2001,2002,2003,2004,
                2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015,2016,2017,2018,2019,2020,2021,2022,2023,2023,2024,2025,2026,2027,2028,2029,2030,2031,2032,2033,2034,2035,
                2036,2037,2038,2039,2040,2041,2042,2043,2044,2044,2045,2046,2047,2048,2049,2050,2051,2052,2053,2054,2055,2056,2057,2058,2059,2060,2061,2062,2063,2064,2064,2065,
                2066,2067,2068,2069,2070,2071,2072,2073,2074,2075,2076,2077,2078,2079,2080,2081,2082,2083,2084,2084,2085,2086,2087,2088,2089,2090,2091,2092,2093,2094,2095,2096,
                2097,2098,2099,2100,2101,2102,2103,2104,2104,2105,2106,2107,2108,2109,2110,2111,2112,2113,2114,2115,2116,2117,2118,2119,2120,2121,2122,2123,2123,2124,2125,2126,
                2127,2128,2129,2130,2131,2132,2133,2134,2135,2136,2137,2138,2139,2140,2141,2142,2142,2143,2144,2145,2146,2147,2148,2149,2150,2151,2152,2153,2154,2155,2156,2157,
                2158,2159,2160,2161,2161,2162,2163,2164,2165,2166,2167,2168,2169,2170,2171,2172,2168,2169,2170,2171,2172,2173,2174,2174,2175,2176,2177,2178,2179,2180,2180,2181,
                2182,2183,2184,2185,2186,2186,2187,2188,2189,2190,2191,2191,2192,2193,2194,2195,2196,2197,2197,2198,2199,2200,2201,2202,2202,2203,2204,2205,2206,2207,2208,2208,
                2209,2210,2211,2212,2213,2213,2214,2215,2216,2217,2218,2219,2219,2220,2221,2222,2223,2224,2224,2225,2226,2227,2228,2229,2230,2230,2231,2232,2233,2234,2235,2235,
                2236,2237,2238,2239,2240,2240,2241,2242,2243,2244,2245,2245,2246,2247,2248,2249,2250,2251,2251,2252,2253,2254,2255,2256,2256,2257,2258,2259,2260,2261,2261,2262,
                2263,2264,2265,2266,2266,2267,2268,2269,2270,2271,2271,2272,2273,2274,2275,2276,2276,2277,2278,2279,2280,2281,2281,2282,2283,2284,2285,2286,2286,2287,2288,2289,
                2290,2291,2291,2292,2293,2294,2295,2296,2296,2297,2298,2299,2300,2301,2301,2302,2303,2304,2305,2306,2306,2307,2308,2309,2310,2311,2311,2312,2313,2314,2315,2316,
                2316,2317,2318,2319,2320,2320,2321,2322,2323,2324,2325,2325,2326,2327,2328,2329,2330,2330,2331,2332,2333,2334,2335,2335,2336,2337,2338,2339,2339,2340,2341,2342,
                2343,2344,2344,2345,2346,2347,2348,2349,2349,2350,2351,2352,2353,2353,2354,2355,2356,2357,2358,2358,2359,2360,2361,2362,2362,2363,2364,2365,2366,2367,2367,2368,
                2369,2370,2371,2371,2372,2373,2374,2375,2376,2376,2377,2378,2379,2380,2380,2381,2382,2383,2384,2385,2385,2386,2387,2388,2389,2389,2390,2391,2392,2393,2394,2394,
                2395,2396,2397,2398,2398,2399,2400,2401,2402,2402,2403,2404,2405,2406,2407,2407,2408,2409,2410,2411,2411,2412,2413,2414,2415,2415,2416,2417,2418,2419,2420,2420,
                2421,2422,2423,2424,2424,2425,2426,2427,2428,2428,2429,2430,2431,2432,2432,2433,2434,2435,2436,2436,2437,2438,2439,2440,2441,2441,2442,2443,2444,2445,2445,2446,
                2447,2448,2449,2449,2450,2451,2452,2453,2453,2454,2455,2456,2457,2457,2458,2459,2460,2461,2461,2462,2463,2464,2465,2465,2466,2467,2468,2469,2469,2470,2471,2472,
                2473,2473,2474,2475,2476,2477,2477,2478,2479,2480,2481,2481,2482,2483,2484,2485,2485,2486,2487,2488,2489,2489,2490,2491,2492,2493,2493,2494,2495,2496,2497,2497,
                2498,2499,2500,2501,2501,2502,2503,2504,2505,2505,2506,2507,2508,2509,2509,2510,2511,2512,2512,2513,2514,2515,2516,2516,2517,2518,2519,2520,2520,2521,2522,2523,
                2524,2524,2525,2526,2527,2528,2528,2529,2530,2531,2531,2532,2533,2534,2535,2535,2536,2537,2538,2539,2539,2540,2541,2542,2543,2543,2544,2545,2546,2546,2547,2548,
                2549,2550,2550,2551,2552,2553,2554,2554,2555,2556,2557,2557,2558,2559,2560,2561,2561,2562,2563,2564,2565,2565,2566,2567,2568,2568,2569,2570,2571,2572,2572,2573,
                2574,2575,2575,2576,2577,2578,2579,2579,2580,2581,2582,2583,2583,2584,2585,2586,2586,2587,2588,2589,2590,2590,2591,2592,2593,2593,2594,2595,2596,2597,2597,2598,
                2599,2600,2600,2601,2602,2603,2604,2604,2605,2606,2607,2607,2608,2609,2610,2611,2611,2612,2613,2614,2614,2615,2616,2617,2617,2618,2619,2620,2621,2621,2622,2623,
                2624,2624,2625,2626,2627,2628,2628,2629,2630,2631,2631,2632,2633,2634,2634,2635,2636,2637,2638,2638,2639,2640,2641,2641,2642,2643,2644,2645,2645,2646,2647,2648,
                2648,2649,2650,2651,2651,2652,2653,2654,2655,2656,2657,2658,2658,2659,2660,2660,2661,2662,2662,2663,2664,2664,2665,2666,2666,2667,2668,2668,2669,2670,2670,2671,
                2672,2672,2673,2674,2674,2675,2676,2676,2677,2677,2678,2679,2679,2680,2681,2681,2682,2683,2683,2684,2685,2685,2686,2687,2687,2688,2689,2689,2690,2691,2691,2692,
                2693,2693,2694,2695,2695,2696,2697,2697,2698,2699,2699,2700,2701,2701,2702,2703,2703,2704,2705,2705,2706,2706,2707,2708,2708,2709,2710,2710,2711,2712,2712,2713,
                2714,2714,2715,2716,2716,2717,2718,2718,2719,2719,2720,2721,2721,2722,2723,2723,2724,2725,2725,2726,2727,2727,2728,2729,2729,2730,2731,2731,2732,2732,2733,2734,
                2734,2735,2736,2736,2737,2738,2738,2739,2740,2740,2741,2741,2742,2743,2743,2744,2745,2745,2746,2747,2747,2748,2749,2749,2750,2750,2751,2752,2752,2753,2754,2754,
                2755,2756,2756,2757,2757,2758,2759,2759,2760,2761,2761,2762,2763,2763,2764,2764,2765,2766,2766,2767,2768,2768,2769,2770,2770,2771,2771,2772,2773,2773,2774,2775,
                2775,2776,2776,2777,2778,2778,2779,2780,2780,2781,2781,2782,2783,2783,2784,2785,2785,2786,2787,2787,2788,2788,2789,2790,2790,2791,2792,2792,2793,2793,2794,2795,
                2795,2796,2797,2797,2798,2798,2799,2800,2800,2801,2802,2802,2803,2803,2804,2805,2805,2806,2806,2807,2808,2808,2809,2810,2810,2811,2811,2812,2813,2813,2814,2815,
                2815,2816,2816,2817,2818,2818,2819,2819,2820,2821,2821,2822,2823,2823,2824,2824,2825,2826,2826,2827,2827,2828,2829,2829,2830,2831,2831,2832,2832,2833,2834,2834,
                2835,2835,2836,2837,2837,2838,2838,2839,2840,2840,2841,2842,2842,2843,2843,2844,2845,2845,2846,2846,2847,2848,2848,2849,2849,2850,2851,2851,2852,2852,2853,2854,
                2854,2855,2855,2856,2857,2857,2858,2858,2859,2860,2860,2861,2861,2862,2863,2863,2864,2864,2865,2866,2866,2867,2867,2868,2869,2869,2870,2870,2871,2872,2872,2873,
                2873,2874,2875,2875,2876,2876,2877,2878,2878,2879,2879,2880,2881,2881,2882,2882,2883,2884,2884,2885,2885,2886,2887,2887,2888,2888,2889,2889,2890,2891,2891,2892,
                2892,2893,2894,2894,2895,2895,2896,2897,2897,2898,2898,2899,2900,2900,2901,2901,2902,2902,2903,2904,2904,2905,2905,2906,2907,2907,2908,2908,2909,2909,2910,2911,
                2911,2912,2912,2913,2914,2914,2915,2915,2916,2916,2917,2918,2918,2919,2919,2920,2921,2921,2922,2922,2923,2923,2924,2925,2925,2926,2926,2927,2927,2928,2929,2929,
                2930,2930,2931,2931,2932,2933,2933,2934,2934,2935,2936,2936,2937,2937,2938,2938,2939,2940,2940,2941,2941,2942,2942,2943,2944,2944,2945,2945,2946,2946,2947,2948,
                2948,2949,2949,2950,2950,2951,2952,2952,2953,2953,2954,2954,2955,2956,2956,2957,2957,2958,2958,2959,2959,2960,2961,2961,2962,2962,2963,2963,2964,2965,2965,2966,
                2966,2967,2967,2968,2968,2969,2970,2970,2971,2971,2972,2972,2973,2974,2974,2975,2975,2976,2976,2977,2977,2978,2979,2979,2980,2980,2981,2981,2982,2982,2983,2984,
                2984,2985,2985,2986,2986,2987,2987,2988,2989,2989,2990,2990,2991,2991,2992,2992,2993,2994,2994,2995,2995,2996,2996,2997,2997,2998,2999,2999,3000,3000,3001,3001,
                3002,3002,3003,3003,3004,3005,3005,3006,3006,3007,3007,3008,3008,3009,3010,3010,3011,3011,3012,3012,3013,3013,3014,3014,3015,3016,3016,3017,3017,3018,3018,3019,
                3019,3020,3020,3021,3021,3022,3023,3023,3024,3024,3025,3025,3026,3026,3027,3027,3028,3028,3029,3030,3030,3031,3031,3032,3032,3033,3033,3034,3034,3035,3035,3036,
                3037,3037,3038,3038,3039,3039,3040,3040,3041,3041,3042,3042,3043,3044,3044,3045,3045,3046,3046,3047,3047,3048,3048,3049,3049,3050,3050,3051,3051,3052,3053,3053,
                3054,3054,3055,3055,3056,3056,3057,3057,3058,3058,3059,3059,3060,3060,3061,3062,3062,3063,3063,3064,3064,3065,3065,3066,3066,3067,3067,3068,3068,3069,3069,3070,
                3070,3071,3071,3072,3073,3074,3075,3075,3076,3076,3077,3077,3078,3078,3079,3080,3080,3081,3081,3082,3082,3083,3083,3084,3085,3085,3086,3086,3087,3087,3088,3088,
                3089,3090,3090,3091,3091,3092,3092,3093,3093,3094,3095,3095,3096,3096,3097,3097,3098,3099,3099,3100,3100,3101,3101,3102,3102,3103,3104,3104,3105,3105,3106,3106,
                3107,3108,3108,3109,3109,3110,3110,3111,3111,3112,3113,3113,3114,3114,3115,3115,3116,3117,3117,3118,3118,3119,3119,3120,3120,3121,3122,3122,3123,3123,3124,3124,
                3125,3126,3126,3127,3127,3128,3128,3129,3130,3130,3131,3131,3132,3132,3133,3133,3134,3135,3135,3136,3136,3137,3137,3138,3139,3139,3140,3140,3141,3141,3142,3143,
                3143,3144,3144,3145,3145,3146,3147,3147,3148,3148,3149,3149,3150,3151,3151,3152,3152,3153,3153,3154,3155,3155,3156,3156,3157,3157,3158,3159,3159,3160,3160,3161,
                3161,3162,3163,3163,3164,3164,3165,3165,3166,3167,3167,3168,3168,3169,3170,3170,3171,3171,3172,3172,3173,3174,3174,3175,3175,3176,3176,3177,3178,3178,3179,3179,
                3180,3180,3181,3182,3182,3183,3183,3184,3185,3185,3186,3186,3187,3187,3188,3189,3189,3190,3190,3191,3192,3192,3193,3193,3194,3194,3195,3196,3196,3197,3197,3198,
                3198,3199,3200,3200,3201,3201,3202,3203,3203,3204,3204,3205,3205,3206,3207,3207,3208,3208,3209,3210,3210,3211,3211,3212,3213,3213,3214,3214,3215,3215,3216,3217,
                3217,3218,3218,3219,3220,3220,3221,3221,3222,3222,3223,3224,3224,3225,3225,3226,3227,3227,3228,3228,3229,3230,3230,3231,3231,3232,3233,3233,3234,3234,3235,3235,
                3236,3237,3237,3238,3238,3239,3240,3240,3241,3241,3242,3243,3243,3244,3244,3245,3246,3246,3247,3247,3248,3248,3249,3250,3250,3251,3251,3252,3253,3253,3254,3254,
                3255,3256,3256,3257,3257,3258,3259,3259,3260,3260,3261,3262,3262,3263,3263,3264,3265,3265,3266,3266,3267,3268,3268,3269,3269,3270,3271,3271,3272,3272,3273,3274,
                3274,3275,3275,3276,3277,3277,3278,3278,3279,3280,3280,3281,3281,3282,3283,3283,3284,3284,3285,3286,3286,3287,3287,3288,3289,3289,3290,3290,3291,3292,3292,3293,
                3293,3294,3295,3295,3296,3296,3297,3298,3298,3299,3299,3300,3301,3301,3302,3302,3303,3304,3304,3305,3306,3307,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,
                3313,3314,3314,3315,3315,3316,3316,3317,3317,3318,3318,3319,3320,3320,3321,3321,3322,3322,3323,3323,3324,3324,3325,3325,3326,3327,3327,3328,3328,3329,3329,3330,
                3330,3331,3331,3332,3332,3333,3334,3334,3335,3335,3336,3336,3337,3337,3338,3338,3339,3340,3340,3341,3341,3342,3342,3343,3343,3344,3344,3345,3345,3346,3347,3347,
                3348,3348,3349,3349,3350,3350,3351,3351,3352,3352,3353,3354,3354,3355,3355,3356,3356,3357,3357,3358,3358,3359,3359,3360,3361,3361,3362,3362,3363,3363,3364,3364,
                3365,3365,3366,3367,3367,3368,3368,3369,3369,3370,3370,3371,3371,3372,3372,3373,3373,3374,3375,3375,3376,3376,3377,3377,3378,3378,3379,3379,3380,3380,3381,3382,
                3382,3383,3383,3384,3384,3385,3385,3386,3386,3387,3387,3388,3389,3389,3390,3390,3391,3391,3392,3392,3393,3393,3394,3394,3395,3396,3396,3397,3397,3398,3398,3399,
                3399,3400,3400,3401,3401,3402,3403,3403,3404,3404,3405,3405,3406,3406,3407,3407,3408,3408,3409,3409,3410,3411,3411,3412,3412,3413,3413,3414,3414,3415,3415,3416,
                3416,3417,3418,3418,3419,3419,3420,3420,3421,3421,3422,3422,3423,3423,3424,3424,3425,3426,3426,3427,3427,3428,3428,3429,3429,3430,3430,3431,3431,3432,3432,3433,
                3434,3434,3435,3435,3436,3436,3437,3437,3438,3438,3439,3439,3440,3440,3441,3442,3442,3443,3443,3444,3444,3445,3445,3446,3446,3447,3447,3448,3448,3449,3450,3450,
                3451,3451,3452,3452,3453,3453,3454,3454,3455,3455,3456,3456,3457,3458,3458,3459,3459,3460,3460,3461,3461,3462,3462,3463,3463,3464,3464,3465,3466,3466,3467,3467,
                3468,3468,3469,3469,3470,3470,3471,3471,3472,3472,3473,3474,3474,3475,3475,3476,3476,3477,3477,3478,3478,3479,3479,3480,3480,3481,3481,3482,3483,3483,3484,3484,
                3485,3485,3486,3486,3487,3487,3488,3488,3489,3489,3490,3491,3491,3492,3492,3493,3493,3494,3494,3495,3495,3496,3496,3497,3497,3498,3498,3499,3500,3500,3501,3501,
                3502,3502,3503,3503,3504,3504,3505,3505,3506,3506,3507,3507,3508,3509,3509,3510,3510,3511,3511,3512,3512,3513,3513,3514,3514,3515,3515,3516,3516,3517,3518,3518,
                3519,3519,3520,3520,3521,3521,3522,3522,3523,3523,3524,3524,3525,3525,3526,3526,3527,3528,3528,3529,3529,3530,3530,3531,3531,3532,3532,3533,3533,3534,3534,3535,
                3535,3536,3537,3537,3538,3538,3539,3539,3540,3540,3541,3541,3542,3542,3543,3543,3544,3544,3545,3545,3546,3547,3547,3548,3548,3549,3549,3550,3550,3551,3551,3552,
                3552,3553,3553,3554,3554,3555,3555,3556,3557,3557,3558,3558,3559,3559,3560,3560,3561,3561,3562,3562,3563,3563,3564,3564,3565,3565,3566,3567,3567,3568,3568,3569,
                3569,3570,3570,3571,3571,3572,3572,3573,3573,3574,3574,3575,3575,3576,3577,3577,3578,3578,3579,3579,3580,3580,3581,3581,3582,3582,3583,3583,3584,3584,3585,3585,
                3586,3586,3587,3588,3588,3589,3589,3590,3590,3591,3591,3592,3592,3593,3593,3594,3594,3595,3595,3596,3596,3597,3597,3598,3599,3599,3600,3600,3601,3601,3602,3602,
                3603,3603,3604,3604,3605,3605,3606,3606,3607,3607,3608,3608,3609,3609,3610,3611,3611,3612,3612,3613,3613,3614,3614,3615,3615,3616,3616,3617,3617,3618,3618,3619,
                3619,3620,3620,3621,3621,3622,3623,3623,3624,3624,3625,3625,3626,3626,3627,3627,3628,3628,3629,3629,3630,3630,3631,3631,3632,3632,3633,3633,3634,3635,3635,3636,
                3636,3637,3637,3638,3638,3639,3639,3640,3640,3641,3641,3642,3642,3643,3643,3644,3644,3645,3645,3646,3646,3647,3648,3648,3649,3649,3650,3650,3651,3651,3652,3652,
                3653,3653,3654,3654,3655,3655,3656,3656,3657,3657,3658,3658,3659,3659,3660,3661,3661,3662,3662,3663,3663,3664,3664,3665,3665,3666,3666,3667,3667,3668,3668,3669,
                3669,3670,3670,3671,3671,3672,3672,3673,3673,3674,3675,3675,3676,3676,3677,3677,3678,3678,3679,3679,3680,3680,3681,3681,3682,3682,3683,3683,3684,3684,3685,3685,
                3686,3686,3687,3687,3688,3688,3689,3690,3690,3691,3691,3692,3692,3693,3693,3694,3694,3695,3695,3696,3696,3697,3697,3698,3698,3699,3699,3700,3700,3701,3701,3702,
                3702,3703,3703,3704,3705,3705,3706,3706,3707,3707,3708,3708,3709,3709,3710,3710,3711,3711,3712,3712,3713,3713,3714,3714,3715,3715,3716,3716,3717,3717,3718,3718,
                3719,3719,3720,3720,3721,3722,3722,3723,3723,3724,3724,3725,3725,3726,3726,3727,3727,3728,3728,3729,3729,3730,3730,3731,3731,3732,3732,3733,3733,3734,3734,3735,
                3735,3736,3736,3737,3737,3738,3738,3739,3740,3740,3741,3741,3742,3742,3743,3743,3744,3744,3745,3745,3746,3746,3747,3747,3748,3748,3749,3749,3750,3750,3751,3751,
                3752,3752,3753,3753,3754,3754,3755,3755,3756,3756,3757,3757,3758,3759,3759,3760,3760,3761,3761,3762,3762,3763,3763,3764,3764,3765,3765,3766,3766,3767,3767,3768,
                3768,3769,3769,3770,3770,3771,3771,3772,3772,3773,3773,3774,3774,3775,3775,3776,3776,3777,3777,3778,3778,3779,3779,3780,3781,3781,3782,3783,3784,3785,3786,3787,
                3787,3788,3789,3790,3791,3792,3792,3793,3794,3795,3796,3797,3798,3798,3799,3800,3801,3802,3803,3804,3805,3805,3806,3807,3808,3809,3810,3811,3812,3813,3813,3814,
                3815,3816,3817,3818,3819,3820,3821,3821,3822,3823,3824,3825,3826,3827,3828,3829,3830,3831,3832,3832,3833,3834,3835,3836,3837,3838,3839,3840,3841,3842,3843,3844,
                3845,3845,3846,3847,3848,3849,3850,3851,3852,3853,3854,3855,3856,3857,3858,3859,3860,3861,3862,3863,3864,3865,3866,3867,3868,3869,3870,3870,3871,3872,3873,3874,
                3875,3876,3877,3878,3879,3880,3881,3882,3883,3884,3885,3886,3887,3888,3889,3890,3891,3892,3893,3894,3895,3897,3898,3899,3900,3901,3902,3903,3904,3905,3906,3907,
                3908,3909,3910,3911,3912,3913,3914,3915,3916,3917,3918,3919,3920,3921,3922,3924,3925,3926,3927,3928,3929,3930,3931,3932,3933,3934,3935,3936,3937,3939,3940,3941,
                3942,3943,3944,3945,3946,3947,3948,3949,3950,3952,3953,3954,3955,3956,3957,3958,3959,3960,3962,3963,3964,3965,3966,3967,3968,3969,3970,3972,3973,3974,3975,3976,
                3977,3978,3979,3981,3982,3983,3984,3985,3986,3987,3989,3990,3991,3992,3993,3994,3996,3997,3998,3999,4000,4001,4002,4004,4005,4006,4007,4008,4010,4011,4012,4013,
                4014,4015,4017,4018,4019,4020,4021,4023,4024,4025,4026,4027,4028,4030,4031,4032,4033,4034,4036,4037,4038,4039,4041,4042,4043,4044,4045,4047,4048,4049,4050,4052,
                4053,4054,4055,4056,4058,4059,4060,4061,4063,4064,4065,4066,4068,4069,4070,4071,4073,4074,4075,4076,4078,4079,4080,4081,4083,4084,4085,4086,4088,4089,4090,4092,
          },
          /* aec trigger 2 */
          {
                   0,   2,   4,   6,   9,  11,  13,  16,  18,  20,  22,  24,  27,  29,  31,  33,  35,  38,  40,  42,  44,  46,  48,  51,  53,  55,  57,  59,  61,  63,  62,  65,
                  68,  71,  74,  77,  80,  83,  86,  88,  91,  94,  97, 100, 103, 105, 108, 111, 113, 116, 119, 121, 124, 127, 129, 132, 134, 137, 139, 141, 143, 145, 148, 150,
                 153, 155, 158, 161, 164, 166, 169, 172, 175, 177, 180, 183, 186, 189, 192, 195, 198, 201, 204, 207, 210, 213, 216, 220, 223, 226, 229, 232, 236, 239, 242, 246,
                 249, 253, 256, 259, 263, 266, 270, 274, 277, 281, 284, 288, 292, 295, 299, 303, 307, 310, 314, 318, 322, 326, 330, 334, 336, 340, 344, 347, 351, 354, 358, 362,
                 365, 369, 373, 376, 380, 384, 387, 391, 395, 398, 402, 406, 410, 414, 417, 421, 425, 429, 433, 436, 440, 444, 448, 452, 456, 460, 464, 468, 471, 475, 479, 483,
                 483, 487, 490, 494, 497, 501, 504, 508, 511, 514, 518, 521, 525, 528, 531, 535, 538, 542, 545, 548, 552, 555, 558, 562, 565, 568, 571, 575, 578, 581, 585, 588,
                 591, 594, 598, 601, 604, 607, 611, 614, 617, 620, 624, 627, 630, 633, 636, 640, 643, 646, 649, 652, 655, 658, 662, 665, 668, 671, 674, 677, 680, 683, 687, 690,
                 693, 696, 699, 702, 705, 708, 711, 714, 717, 720, 723, 726, 729, 732, 735, 738, 741, 744, 747, 750, 753, 756, 759, 762, 765, 768, 770, 773, 776, 779, 782, 785,
                 788, 791, 794, 796, 799, 802, 805, 808, 811, 813, 816, 819, 822, 825, 828, 830, 833, 836, 839, 841, 844, 847, 850, 852, 855, 858, 861, 863, 866, 869, 871, 874,
                 877, 879, 882, 885, 887, 890, 893, 895, 898, 901, 903, 906, 909, 912, 914, 917, 920, 922, 925, 927, 930, 933, 935, 938, 941, 943, 946, 949, 951, 954, 957, 959,
                 962, 965, 967, 970, 973, 975, 978, 981, 983, 986, 989, 991, 994, 997, 999,1002,1004,1007,1010,1012,1015,1018,1020,1023,1026,1028,1031,1034,1036,1039,1041,1044,
                1047,1049,1052,1055,1057,1060,1063,1065,1068,1070,1073,1076,1078,1081,1084,1086,1089,1091,1094,1097,1099,1102,1105,1107,1110,1112,1115,1118,1120,1123,1125,1128,
                1131,1133,1136,1139,1141,1144,1146,1149,1152,1154,1157,1159,1162,1165,1167,1170,1173,1175,1177,1179,1181,1183,1185,1187,1189,1192,1194,1196,1198,1200,1202,1204,
                1206,1208,1210,1212,1214,1216,1219,1221,1223,1225,1227,1229,1231,1233,1235,1237,1239,1241,1243,1245,1247,1249,1251,1253,1255,1257,1259,1261,1263,1265,1267,1269,
                1271,1273,1275,1277,1279,1281,1283,1285,1287,1289,1290,1292,1294,1296,1298,1300,1302,1304,1306,1308,1310,1312,1313,1315,1317,1319,1321,1323,1325,1327,1329,1330,
                1332,1334,1336,1338,1340,1342,1343,1345,1347,1349,1351,1353,1354,1356,1358,1360,1362,1364,1365,1367,1369,1371,1373,1374,1376,1378,1380,1381,1383,1385,1387,1389,
                1390,1392,1394,1396,1397,1399,1401,1403,1403,1405,1407,1409,1411,1412,1414,1416,1417,1419,1421,1423,1424,1426,1428,1430,1431,1433,1435,1436,1438,1440,1441,1443,
                1445,1446,1448,1450,1451,1453,1455,1456,1458,1460,1461,1463,1465,1466,1468,1469,1471,1473,1474,1476,1478,1479,1481,1482,1484,1486,1487,1489,1490,1492,1493,1495,
                1497,1498,1500,1501,1503,1504,1506,1507,1509,1510,1512,1514,1515,1517,1518,1520,1521,1523,1524,1526,1527,1529,1530,1532,1533,1534,1536,1537,1539,1540,1542,1543,
                1545,1546,1548,1549,1550,1552,1553,1555,1556,1558,1559,1560,1562,1563,1565,1566,1567,1569,1570,1572,1573,1574,1576,1577,1578,1580,1581,1583,1584,1585,1587,1588,
                1587,1589,1590,1591,1592,1594,1595,1596,1597,1598,1600,1601,1602,1603,1605,1606,1607,1608,1609,1611,1612,1613,1614,1616,1617,1618,1619,1620,1622,1623,1624,1625,
                1626,1628,1629,1630,1631,1632,1634,1635,1636,1637,1638,1640,1641,1642,1643,1644,1646,1647,1648,1649,1650,1652,1653,1654,1655,1656,1658,1659,1660,1661,1662,1663,
                1665,1666,1667,1668,1669,1671,1672,1673,1674,1675,1676,1678,1679,1680,1681,1682,1684,1685,1686,1687,1688,1689,1691,1692,1693,1694,1695,1696,1698,1699,1700,1701,
                1702,1703,1705,1706,1707,1708,1709,1710,1712,1713,1714,1715,1716,1717,1719,1720,1721,1722,1723,1724,1726,1727,1728,1729,1730,1731,1732,1734,1735,1736,1737,1738,
                1739,1740,1742,1743,1744,1745,1746,1747,1748,1750,1751,1752,1753,1754,1755,1756,1758,1759,1760,1761,1762,1763,1764,1766,1767,1768,1769,1770,1771,1772,1773,1775,
                1776,1777,1778,1779,1780,1781,1782,1784,1785,1786,1787,1788,1789,1790,1791,1793,1794,1795,1796,1797,1798,1799,1800,1802,1803,1804,1805,1806,1807,1808,1809,1810,
                1812,1813,1814,1815,1816,1817,1818,1819,1820,1821,1823,1824,1825,1826,1827,1828,1829,1830,1831,1832,1834,1835,1836,1837,1838,1839,1840,1841,1842,1843,1844,1846,
                1847,1848,1849,1850,1851,1852,1853,1854,1855,1856,1858,1859,1860,1861,1862,1863,1864,1865,1866,1867,1868,1869,1870,1872,1873,1874,1875,1876,1877,1878,1879,1880,
                1881,1882,1883,1884,1886,1887,1888,1889,1890,1891,1892,1893,1894,1895,1896,1897,1898,1899,1900,1901,1902,1903,1904,1905,1906,1907,1908,1909,1910,1911,1911,1912,
                1913,1914,1915,1916,1917,1918,1919,1920,1921,1922,1923,1924,1925,1926,1927,1928,1929,1930,1931,1932,1933,1934,1934,1935,1936,1937,1938,1939,1940,1941,1942,1943,
                1944,1945,1946,1947,1948,1949,1950,1951,1952,1953,1954,1955,1956,1957,1957,1958,1959,1960,1961,1962,1963,1964,1965,1966,1967,1968,1969,1970,1971,1972,1973,1974,
                1975,1976,1977,1978,1979,1979,1980,1981,1982,1983,1984,1985,1986,1987,1988,1989,1990,1991,1992,1993,1994,1995,1996,1997,1998,1999,2000,2001,2001,2002,2003,2004,
                2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015,2016,2017,2018,2019,2020,2021,2022,2023,2023,2024,2025,2026,2027,2028,2029,2030,2031,2032,2033,2034,2035,
                2036,2037,2038,2039,2040,2041,2042,2043,2044,2044,2045,2046,2047,2048,2049,2050,2051,2052,2053,2054,2055,2056,2057,2058,2059,2060,2061,2062,2063,2064,2064,2065,
                2066,2067,2068,2069,2070,2071,2072,2073,2074,2075,2076,2077,2078,2079,2080,2081,2082,2083,2084,2084,2085,2086,2087,2088,2089,2090,2091,2092,2093,2094,2095,2096,
                2097,2098,2099,2100,2101,2102,2103,2104,2104,2105,2106,2107,2108,2109,2110,2111,2112,2113,2114,2115,2116,2117,2118,2119,2120,2121,2122,2123,2123,2124,2125,2126,
                2127,2128,2129,2130,2131,2132,2133,2134,2135,2136,2137,2138,2139,2140,2141,2142,2142,2143,2144,2145,2146,2147,2148,2149,2150,2151,2152,2153,2154,2155,2156,2157,
                2158,2159,2160,2161,2161,2162,2163,2164,2165,2166,2167,2168,2169,2170,2171,2172,2168,2169,2170,2171,2172,2173,2174,2174,2175,2176,2177,2178,2179,2180,2180,2181,
                2182,2183,2184,2185,2186,2186,2187,2188,2189,2190,2191,2191,2192,2193,2194,2195,2196,2197,2197,2198,2199,2200,2201,2202,2202,2203,2204,2205,2206,2207,2208,2208,
                2209,2210,2211,2212,2213,2213,2214,2215,2216,2217,2218,2219,2219,2220,2221,2222,2223,2224,2224,2225,2226,2227,2228,2229,2230,2230,2231,2232,2233,2234,2235,2235,
                2236,2237,2238,2239,2240,2240,2241,2242,2243,2244,2245,2245,2246,2247,2248,2249,2250,2251,2251,2252,2253,2254,2255,2256,2256,2257,2258,2259,2260,2261,2261,2262,
                2263,2264,2265,2266,2266,2267,2268,2269,2270,2271,2271,2272,2273,2274,2275,2276,2276,2277,2278,2279,2280,2281,2281,2282,2283,2284,2285,2286,2286,2287,2288,2289,
                2290,2291,2291,2292,2293,2294,2295,2296,2296,2297,2298,2299,2300,2301,2301,2302,2303,2304,2305,2306,2306,2307,2308,2309,2310,2311,2311,2312,2313,2314,2315,2316,
                2316,2317,2318,2319,2320,2320,2321,2322,2323,2324,2325,2325,2326,2327,2328,2329,2330,2330,2331,2332,2333,2334,2335,2335,2336,2337,2338,2339,2339,2340,2341,2342,
                2343,2344,2344,2345,2346,2347,2348,2349,2349,2350,2351,2352,2353,2353,2354,2355,2356,2357,2358,2358,2359,2360,2361,2362,2362,2363,2364,2365,2366,2367,2367,2368,
                2369,2370,2371,2371,2372,2373,2374,2375,2376,2376,2377,2378,2379,2380,2380,2381,2382,2383,2384,2385,2385,2386,2387,2388,2389,2389,2390,2391,2392,2393,2394,2394,
                2395,2396,2397,2398,2398,2399,2400,2401,2402,2402,2403,2404,2405,2406,2407,2407,2408,2409,2410,2411,2411,2412,2413,2414,2415,2415,2416,2417,2418,2419,2420,2420,
                2421,2422,2423,2424,2424,2425,2426,2427,2428,2428,2429,2430,2431,2432,2432,2433,2434,2435,2436,2436,2437,2438,2439,2440,2441,2441,2442,2443,2444,2445,2445,2446,
                2447,2448,2449,2449,2450,2451,2452,2453,2453,2454,2455,2456,2457,2457,2458,2459,2460,2461,2461,2462,2463,2464,2465,2465,2466,2467,2468,2469,2469,2470,2471,2472,
                2473,2473,2474,2475,2476,2477,2477,2478,2479,2480,2481,2481,2482,2483,2484,2485,2485,2486,2487,2488,2489,2489,2490,2491,2492,2493,2493,2494,2495,2496,2497,2497,
                2498,2499,2500,2501,2501,2502,2503,2504,2505,2505,2506,2507,2508,2509,2509,2510,2511,2512,2512,2513,2514,2515,2516,2516,2517,2518,2519,2520,2520,2521,2522,2523,
                2524,2524,2525,2526,2527,2528,2528,2529,2530,2531,2531,2532,2533,2534,2535,2535,2536,2537,2538,2539,2539,2540,2541,2542,2543,2543,2544,2545,2546,2546,2547,2548,
                2549,2550,2550,2551,2552,2553,2554,2554,2555,2556,2557,2557,2558,2559,2560,2561,2561,2562,2563,2564,2565,2565,2566,2567,2568,2568,2569,2570,2571,2572,2572,2573,
                2574,2575,2575,2576,2577,2578,2579,2579,2580,2581,2582,2583,2583,2584,2585,2586,2586,2587,2588,2589,2590,2590,2591,2592,2593,2593,2594,2595,2596,2597,2597,2598,
                2599,2600,2600,2601,2602,2603,2604,2604,2605,2606,2607,2607,2608,2609,2610,2611,2611,2612,2613,2614,2614,2615,2616,2617,2617,2618,2619,2620,2621,2621,2622,2623,
                2624,2624,2625,2626,2627,2628,2628,2629,2630,2631,2631,2632,2633,2634,2634,2635,2636,2637,2638,2638,2639,2640,2641,2641,2642,2643,2644,2645,2645,2646,2647,2648,
                2648,2649,2650,2651,2651,2652,2653,2654,2655,2656,2657,2658,2658,2659,2660,2660,2661,2662,2662,2663,2664,2664,2665,2666,2666,2667,2668,2668,2669,2670,2670,2671,
                2672,2672,2673,2674,2674,2675,2676,2676,2677,2677,2678,2679,2679,2680,2681,2681,2682,2683,2683,2684,2685,2685,2686,2687,2687,2688,2689,2689,2690,2691,2691,2692,
                2693,2693,2694,2695,2695,2696,2697,2697,2698,2699,2699,2700,2701,2701,2702,2703,2703,2704,2705,2705,2706,2706,2707,2708,2708,2709,2710,2710,2711,2712,2712,2713,
                2714,2714,2715,2716,2716,2717,2718,2718,2719,2719,2720,2721,2721,2722,2723,2723,2724,2725,2725,2726,2727,2727,2728,2729,2729,2730,2731,2731,2732,2732,2733,2734,
                2734,2735,2736,2736,2737,2738,2738,2739,2740,2740,2741,2741,2742,2743,2743,2744,2745,2745,2746,2747,2747,2748,2749,2749,2750,2750,2751,2752,2752,2753,2754,2754,
                2755,2756,2756,2757,2757,2758,2759,2759,2760,2761,2761,2762,2763,2763,2764,2764,2765,2766,2766,2767,2768,2768,2769,2770,2770,2771,2771,2772,2773,2773,2774,2775,
                2775,2776,2776,2777,2778,2778,2779,2780,2780,2781,2781,2782,2783,2783,2784,2785,2785,2786,2787,2787,2788,2788,2789,2790,2790,2791,2792,2792,2793,2793,2794,2795,
                2795,2796,2797,2797,2798,2798,2799,2800,2800,2801,2802,2802,2803,2803,2804,2805,2805,2806,2806,2807,2808,2808,2809,2810,2810,2811,2811,2812,2813,2813,2814,2815,
                2815,2816,2816,2817,2818,2818,2819,2819,2820,2821,2821,2822,2823,2823,2824,2824,2825,2826,2826,2827,2827,2828,2829,2829,2830,2831,2831,2832,2832,2833,2834,2834,
                2835,2835,2836,2837,2837,2838,2838,2839,2840,2840,2841,2842,2842,2843,2843,2844,2845,2845,2846,2846,2847,2848,2848,2849,2849,2850,2851,2851,2852,2852,2853,2854,
                2854,2855,2855,2856,2857,2857,2858,2858,2859,2860,2860,2861,2861,2862,2863,2863,2864,2864,2865,2866,2866,2867,2867,2868,2869,2869,2870,2870,2871,2872,2872,2873,
                2873,2874,2875,2875,2876,2876,2877,2878,2878,2879,2879,2880,2881,2881,2882,2882,2883,2884,2884,2885,2885,2886,2887,2887,2888,2888,2889,2889,2890,2891,2891,2892,
                2892,2893,2894,2894,2895,2895,2896,2897,2897,2898,2898,2899,2900,2900,2901,2901,2902,2902,2903,2904,2904,2905,2905,2906,2907,2907,2908,2908,2909,2909,2910,2911,
                2911,2912,2912,2913,2914,2914,2915,2915,2916,2916,2917,2918,2918,2919,2919,2920,2921,2921,2922,2922,2923,2923,2924,2925,2925,2926,2926,2927,2927,2928,2929,2929,
                2930,2930,2931,2931,2932,2933,2933,2934,2934,2935,2936,2936,2937,2937,2938,2938,2939,2940,2940,2941,2941,2942,2942,2943,2944,2944,2945,2945,2946,2946,2947,2948,
                2948,2949,2949,2950,2950,2951,2952,2952,2953,2953,2954,2954,2955,2956,2956,2957,2957,2958,2958,2959,2959,2960,2961,2961,2962,2962,2963,2963,2964,2965,2965,2966,
                2966,2967,2967,2968,2968,2969,2970,2970,2971,2971,2972,2972,2973,2974,2974,2975,2975,2976,2976,2977,2977,2978,2979,2979,2980,2980,2981,2981,2982,2982,2983,2984,
                2984,2985,2985,2986,2986,2987,2987,2988,2989,2989,2990,2990,2991,2991,2992,2992,2993,2994,2994,2995,2995,2996,2996,2997,2997,2998,2999,2999,3000,3000,3001,3001,
                3002,3002,3003,3003,3004,3005,3005,3006,3006,3007,3007,3008,3008,3009,3010,3010,3011,3011,3012,3012,3013,3013,3014,3014,3015,3016,3016,3017,3017,3018,3018,3019,
                3019,3020,3020,3021,3021,3022,3023,3023,3024,3024,3025,3025,3026,3026,3027,3027,3028,3028,3029,3030,3030,3031,3031,3032,3032,3033,3033,3034,3034,3035,3035,3036,
                3037,3037,3038,3038,3039,3039,3040,3040,3041,3041,3042,3042,3043,3044,3044,3045,3045,3046,3046,3047,3047,3048,3048,3049,3049,3050,3050,3051,3051,3052,3053,3053,
                3054,3054,3055,3055,3056,3056,3057,3057,3058,3058,3059,3059,3060,3060,3061,3062,3062,3063,3063,3064,3064,3065,3065,3066,3066,3067,3067,3068,3068,3069,3069,3070,
                3070,3071,3071,3072,3073,3074,3075,3075,3076,3076,3077,3077,3078,3078,3079,3080,3080,3081,3081,3082,3082,3083,3083,3084,3085,3085,3086,3086,3087,3087,3088,3088,
                3089,3090,3090,3091,3091,3092,3092,3093,3093,3094,3095,3095,3096,3096,3097,3097,3098,3099,3099,3100,3100,3101,3101,3102,3102,3103,3104,3104,3105,3105,3106,3106,
                3107,3108,3108,3109,3109,3110,3110,3111,3111,3112,3113,3113,3114,3114,3115,3115,3116,3117,3117,3118,3118,3119,3119,3120,3120,3121,3122,3122,3123,3123,3124,3124,
                3125,3126,3126,3127,3127,3128,3128,3129,3130,3130,3131,3131,3132,3132,3133,3133,3134,3135,3135,3136,3136,3137,3137,3138,3139,3139,3140,3140,3141,3141,3142,3143,
                3143,3144,3144,3145,3145,3146,3147,3147,3148,3148,3149,3149,3150,3151,3151,3152,3152,3153,3153,3154,3155,3155,3156,3156,3157,3157,3158,3159,3159,3160,3160,3161,
                3161,3162,3163,3163,3164,3164,3165,3165,3166,3167,3167,3168,3168,3169,3170,3170,3171,3171,3172,3172,3173,3174,3174,3175,3175,3176,3176,3177,3178,3178,3179,3179,
                3180,3180,3181,3182,3182,3183,3183,3184,3185,3185,3186,3186,3187,3187,3188,3189,3189,3190,3190,3191,3192,3192,3193,3193,3194,3194,3195,3196,3196,3197,3197,3198,
                3198,3199,3200,3200,3201,3201,3202,3203,3203,3204,3204,3205,3205,3206,3207,3207,3208,3208,3209,3210,3210,3211,3211,3212,3213,3213,3214,3214,3215,3215,3216,3217,
                3217,3218,3218,3219,3220,3220,3221,3221,3222,3222,3223,3224,3224,3225,3225,3226,3227,3227,3228,3228,3229,3230,3230,3231,3231,3232,3233,3233,3234,3234,3235,3235,
                3236,3237,3237,3238,3238,3239,3240,3240,3241,3241,3242,3243,3243,3244,3244,3245,3246,3246,3247,3247,3248,3248,3249,3250,3250,3251,3251,3252,3253,3253,3254,3254,
                3255,3256,3256,3257,3257,3258,3259,3259,3260,3260,3261,3262,3262,3263,3263,3264,3265,3265,3266,3266,3267,3268,3268,3269,3269,3270,3271,3271,3272,3272,3273,3274,
                3274,3275,3275,3276,3277,3277,3278,3278,3279,3280,3280,3281,3281,3282,3283,3283,3284,3284,3285,3286,3286,3287,3287,3288,3289,3289,3290,3290,3291,3292,3292,3293,
                3293,3294,3295,3295,3296,3296,3297,3298,3298,3299,3299,3300,3301,3301,3302,3302,3303,3304,3304,3305,3306,3307,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,
                3313,3314,3314,3315,3315,3316,3316,3317,3317,3318,3318,3319,3320,3320,3321,3321,3322,3322,3323,3323,3324,3324,3325,3325,3326,3327,3327,3328,3328,3329,3329,3330,
                3330,3331,3331,3332,3332,3333,3334,3334,3335,3335,3336,3336,3337,3337,3338,3338,3339,3340,3340,3341,3341,3342,3342,3343,3343,3344,3344,3345,3345,3346,3347,3347,
                3348,3348,3349,3349,3350,3350,3351,3351,3352,3352,3353,3354,3354,3355,3355,3356,3356,3357,3357,3358,3358,3359,3359,3360,3361,3361,3362,3362,3363,3363,3364,3364,
                3365,3365,3366,3367,3367,3368,3368,3369,3369,3370,3370,3371,3371,3372,3372,3373,3373,3374,3375,3375,3376,3376,3377,3377,3378,3378,3379,3379,3380,3380,3381,3382,
                3382,3383,3383,3384,3384,3385,3385,3386,3386,3387,3387,3388,3389,3389,3390,3390,3391,3391,3392,3392,3393,3393,3394,3394,3395,3396,3396,3397,3397,3398,3398,3399,
                3399,3400,3400,3401,3401,3402,3403,3403,3404,3404,3405,3405,3406,3406,3407,3407,3408,3408,3409,3409,3410,3411,3411,3412,3412,3413,3413,3414,3414,3415,3415,3416,
                3416,3417,3418,3418,3419,3419,3420,3420,3421,3421,3422,3422,3423,3423,3424,3424,3425,3426,3426,3427,3427,3428,3428,3429,3429,3430,3430,3431,3431,3432,3432,3433,
                3434,3434,3435,3435,3436,3436,3437,3437,3438,3438,3439,3439,3440,3440,3441,3442,3442,3443,3443,3444,3444,3445,3445,3446,3446,3447,3447,3448,3448,3449,3450,3450,
                3451,3451,3452,3452,3453,3453,3454,3454,3455,3455,3456,3456,3457,3458,3458,3459,3459,3460,3460,3461,3461,3462,3462,3463,3463,3464,3464,3465,3466,3466,3467,3467,
                3468,3468,3469,3469,3470,3470,3471,3471,3472,3472,3473,3474,3474,3475,3475,3476,3476,3477,3477,3478,3478,3479,3479,3480,3480,3481,3481,3482,3483,3483,3484,3484,
                3485,3485,3486,3486,3487,3487,3488,3488,3489,3489,3490,3491,3491,3492,3492,3493,3493,3494,3494,3495,3495,3496,3496,3497,3497,3498,3498,3499,3500,3500,3501,3501,
                3502,3502,3503,3503,3504,3504,3505,3505,3506,3506,3507,3507,3508,3509,3509,3510,3510,3511,3511,3512,3512,3513,3513,3514,3514,3515,3515,3516,3516,3517,3518,3518,
                3519,3519,3520,3520,3521,3521,3522,3522,3523,3523,3524,3524,3525,3525,3526,3526,3527,3528,3528,3529,3529,3530,3530,3531,3531,3532,3532,3533,3533,3534,3534,3535,
                3535,3536,3537,3537,3538,3538,3539,3539,3540,3540,3541,3541,3542,3542,3543,3543,3544,3544,3545,3545,3546,3547,3547,3548,3548,3549,3549,3550,3550,3551,3551,3552,
                3552,3553,3553,3554,3554,3555,3555,3556,3557,3557,3558,3558,3559,3559,3560,3560,3561,3561,3562,3562,3563,3563,3564,3564,3565,3565,3566,3567,3567,3568,3568,3569,
                3569,3570,3570,3571,3571,3572,3572,3573,3573,3574,3574,3575,3575,3576,3577,3577,3578,3578,3579,3579,3580,3580,3581,3581,3582,3582,3583,3583,3584,3584,3585,3585,
                3586,3586,3587,3588,3588,3589,3589,3590,3590,3591,3591,3592,3592,3593,3593,3594,3594,3595,3595,3596,3596,3597,3597,3598,3599,3599,3600,3600,3601,3601,3602,3602,
                3603,3603,3604,3604,3605,3605,3606,3606,3607,3607,3608,3608,3609,3609,3610,3611,3611,3612,3612,3613,3613,3614,3614,3615,3615,3616,3616,3617,3617,3618,3618,3619,
                3619,3620,3620,3621,3621,3622,3623,3623,3624,3624,3625,3625,3626,3626,3627,3627,3628,3628,3629,3629,3630,3630,3631,3631,3632,3632,3633,3633,3634,3635,3635,3636,
                3636,3637,3637,3638,3638,3639,3639,3640,3640,3641,3641,3642,3642,3643,3643,3644,3644,3645,3645,3646,3646,3647,3648,3648,3649,3649,3650,3650,3651,3651,3652,3652,
                3653,3653,3654,3654,3655,3655,3656,3656,3657,3657,3658,3658,3659,3659,3660,3661,3661,3662,3662,3663,3663,3664,3664,3665,3665,3666,3666,3667,3667,3668,3668,3669,
                3669,3670,3670,3671,3671,3672,3672,3673,3673,3674,3675,3675,3676,3676,3677,3677,3678,3678,3679,3679,3680,3680,3681,3681,3682,3682,3683,3683,3684,3684,3685,3685,
                3686,3686,3687,3687,3688,3688,3689,3690,3690,3691,3691,3692,3692,3693,3693,3694,3694,3695,3695,3696,3696,3697,3697,3698,3698,3699,3699,3700,3700,3701,3701,3702,
                3702,3703,3703,3704,3705,3705,3706,3706,3707,3707,3708,3708,3709,3709,3710,3710,3711,3711,3712,3712,3713,3713,3714,3714,3715,3715,3716,3716,3717,3717,3718,3718,
                3719,3719,3720,3720,3721,3722,3722,3723,3723,3724,3724,3725,3725,3726,3726,3727,3727,3728,3728,3729,3729,3730,3730,3731,3731,3732,3732,3733,3733,3734,3734,3735,
                3735,3736,3736,3737,3737,3738,3738,3739,3740,3740,3741,3741,3742,3742,3743,3743,3744,3744,3745,3745,3746,3746,3747,3747,3748,3748,3749,3749,3750,3750,3751,3751,
                3752,3752,3753,3753,3754,3754,3755,3755,3756,3756,3757,3757,3758,3759,3759,3760,3760,3761,3761,3762,3762,3763,3763,3764,3764,3765,3765,3766,3766,3767,3767,3768,
                3768,3769,3769,3770,3770,3771,3771,3772,3772,3773,3773,3774,3774,3775,3775,3776,3776,3777,3777,3778,3778,3779,3779,3780,3781,3781,3782,3783,3784,3785,3786,3787,
                3787,3788,3789,3790,3791,3792,3792,3793,3794,3795,3796,3797,3798,3798,3799,3800,3801,3802,3803,3804,3805,3805,3806,3807,3808,3809,3810,3811,3812,3813,3813,3814,
                3815,3816,3817,3818,3819,3820,3821,3821,3822,3823,3824,3825,3826,3827,3828,3829,3830,3831,3832,3832,3833,3834,3835,3836,3837,3838,3839,3840,3841,3842,3843,3844,
                3845,3845,3846,3847,3848,3849,3850,3851,3852,3853,3854,3855,3856,3857,3858,3859,3860,3861,3862,3863,3864,3865,3866,3867,3868,3869,3870,3870,3871,3872,3873,3874,
                3875,3876,3877,3878,3879,3880,3881,3882,3883,3884,3885,3886,3887,3888,3889,3890,3891,3892,3893,3894,3895,3897,3898,3899,3900,3901,3902,3903,3904,3905,3906,3907,
                3908,3909,3910,3911,3912,3913,3914,3915,3916,3917,3918,3919,3920,3921,3922,3924,3925,3926,3927,3928,3929,3930,3931,3932,3933,3934,3935,3936,3937,3939,3940,3941,
                3942,3943,3944,3945,3946,3947,3948,3949,3950,3952,3953,3954,3955,3956,3957,3958,3959,3960,3962,3963,3964,3965,3966,3967,3968,3969,3970,3972,3973,3974,3975,3976,
                3977,3978,3979,3981,3982,3983,3984,3985,3986,3987,3989,3990,3991,3992,3993,3994,3996,3997,3998,3999,4000,4001,4002,4004,4005,4006,4007,4008,4010,4011,4012,4013,
                4014,4015,4017,4018,4019,4020,4021,4023,4024,4025,4026,4027,4028,4030,4031,4032,4033,4034,4036,4037,4038,4039,4041,4042,4043,4044,4045,4047,4048,4049,4050,4052,
                4053,4054,4055,4056,4058,4059,4060,4061,4063,4064,4065,4066,4068,4069,4070,4071,4073,4074,4075,4076,4078,4079,4080,4081,4083,4084,4085,4086,4088,4089,4090,4092,
          },
          /* aec trigger 3 */
          {
                   0,   2,   4,   6,   9,  11,  13,  16,  18,  20,  22,  24,  27,  29,  31,  33,  35,  38,  40,  42,  44,  46,  48,  51,  53,  55,  57,  59,  61,  63,  61,  62,
                  64,  65,  67,  69,  70,  72,  74,  76,  77,  79,  81,  83,  85,  87,  88,  90,  92,  94,  96,  98, 101, 103, 105, 107, 109, 111, 113, 116, 118, 120, 122, 125,
                 127, 130, 132, 134, 137, 139, 142, 144, 147, 149, 152, 155, 157, 160, 163, 165, 168, 171, 173, 176, 179, 182, 185, 188, 191, 194, 197, 199, 202, 204, 206, 209,
                 211, 214, 217, 219, 222, 224, 227, 230, 233, 235, 238, 241, 244, 247, 250, 253, 256, 259, 262, 265, 268, 272, 275, 278, 281, 283, 285, 287, 290, 292, 295, 297,
                 299, 302, 304, 306, 309, 311, 314, 316, 319, 321, 324, 326, 329, 331, 334, 336, 339, 341, 344, 346, 349, 351, 354, 357, 359, 362, 364, 367, 370, 372, 375, 378,
                 380, 383, 386, 388, 391, 394, 397, 399, 402, 405, 408, 411, 413, 416, 419, 422, 425, 427, 430, 433, 436, 439, 442, 445, 448, 450, 453, 456, 459, 462, 465, 468,
                 471, 474, 477, 480, 483, 486, 489, 492, 495, 498, 502, 505, 508, 511, 514, 517, 520, 523, 526, 530, 533, 536, 539, 542, 546, 549, 552, 555, 558, 561, 563, 565,
                 568, 570, 573, 575, 577, 580, 582, 585, 587, 590, 592, 594, 597, 599, 602, 605, 607, 610, 612, 615, 617, 620, 623, 625, 628, 631, 633, 636, 639, 641, 644, 647,
                 649, 652, 655, 658, 661, 663, 666, 669, 672, 675, 678, 680, 683, 686, 689, 692, 695, 698, 701, 704, 707, 710, 713, 716, 719, 722, 725, 728, 731, 734, 737, 741,
                 744, 747, 750, 753, 756, 760, 763, 766, 769, 772, 776, 779, 778, 782, 785, 788, 791, 794, 797, 800, 803, 806, 809, 812, 815, 818, 821, 824, 827, 830, 833, 836,
                 839, 842, 845, 848, 851, 854, 857, 860, 863, 866, 869, 872, 875, 878, 881, 884, 887, 890, 893, 896, 899, 902, 905, 908, 911, 914, 917, 920, 923, 926, 929, 932,
                 935, 938, 941, 944, 947, 950, 953, 956, 958, 961, 964, 967, 970, 973, 976, 979, 982, 985, 988, 991, 994, 996, 999,1002,1005,1008,1011,1014,1017,1020,1023,1026,
                1028,1031,1034,1037,1040,1043,1046,1049,1051,1054,1057,1060,1063,1066,1069,1072,1073,1075,1078,1081,1083,1086,1089,1091,1094,1096,1099,1102,1104,1107,1110,1112,
                1115,1117,1120,1123,1125,1128,1130,1133,1136,1138,1141,1143,1146,1148,1151,1154,1156,1159,1161,1164,1166,1169,1171,1174,1176,1179,1181,1184,1186,1189,1191,1194,
                1196,1199,1201,1204,1206,1208,1211,1213,1216,1218,1221,1223,1225,1228,1230,1233,1235,1237,1240,1242,1245,1247,1249,1252,1254,1256,1259,1261,1263,1266,1268,1270,
                1273,1275,1277,1280,1282,1284,1287,1289,1291,1294,1296,1298,1300,1303,1305,1307,1309,1312,1314,1316,1318,1321,1323,1325,1327,1329,1332,1334,1336,1338,1340,1343,
                1345,1347,1349,1351,1353,1356,1358,1360,1362,1364,1366,1368,1371,1373,1375,1377,1379,1381,1383,1385,1387,1390,1392,1394,1396,1398,1400,1402,1404,1406,1408,1410,
                1412,1414,1416,1418,1420,1422,1424,1426,1428,1430,1432,1434,1436,1438,1440,1442,1444,1446,1448,1450,1452,1454,1456,1458,1460,1462,1464,1465,1467,1469,1471,1473,
                1475,1477,1479,1481,1482,1484,1486,1488,1490,1492,1494,1495,1497,1499,1501,1503,1505,1506,1508,1510,1512,1514,1515,1517,1519,1521,1522,1524,1526,1528,1530,1531,
                1533,1535,1537,1538,1540,1542,1543,1545,1547,1549,1550,1552,1554,1555,1557,1559,1560,1562,1564,1565,1567,1569,1570,1572,1574,1575,1577,1578,1580,1582,1583,1585,
                1587,1589,1590,1591,1592,1594,1595,1596,1597,1598,1600,1601,1602,1603,1605,1606,1607,1608,1609,1611,1612,1613,1614,1616,1617,1618,1619,1620,1622,1623,1624,1625,
                1626,1628,1629,1630,1631,1632,1634,1635,1636,1637,1638,1640,1641,1642,1643,1644,1646,1647,1648,1649,1650,1652,1653,1654,1655,1656,1658,1659,1660,1661,1662,1663,
                1665,1666,1667,1668,1669,1671,1672,1673,1674,1675,1676,1678,1679,1680,1681,1682,1684,1685,1686,1687,1688,1689,1691,1692,1693,1694,1695,1696,1698,1699,1700,1701,
                1702,1703,1705,1706,1707,1708,1709,1710,1712,1713,1714,1715,1716,1717,1719,1720,1721,1722,1723,1724,1726,1727,1728,1729,1730,1731,1732,1734,1735,1736,1737,1738,
                1739,1740,1742,1743,1744,1745,1746,1747,1748,1750,1751,1752,1753,1754,1755,1756,1758,1759,1760,1761,1762,1763,1764,1766,1767,1768,1769,1770,1771,1772,1773,1775,
                1776,1777,1778,1779,1780,1781,1782,1784,1785,1786,1787,1788,1789,1790,1791,1793,1794,1795,1796,1797,1798,1799,1800,1802,1803,1804,1805,1806,1807,1808,1809,1810,
                1812,1813,1814,1815,1816,1817,1818,1819,1820,1821,1823,1824,1825,1826,1827,1828,1829,1830,1831,1832,1834,1835,1836,1837,1838,1839,1840,1841,1842,1843,1844,1846,
                1847,1848,1849,1850,1851,1852,1853,1854,1855,1856,1858,1859,1860,1861,1862,1863,1864,1865,1866,1867,1868,1869,1870,1872,1873,1874,1875,1876,1877,1878,1879,1880,
                1881,1882,1883,1884,1886,1887,1888,1889,1890,1891,1892,1893,1894,1895,1896,1897,1898,1899,1900,1901,1902,1903,1904,1905,1906,1907,1908,1909,1910,1911,1911,1912,
                1913,1914,1915,1916,1917,1918,1919,1920,1921,1922,1923,1924,1925,1926,1927,1928,1929,1930,1931,1932,1933,1934,1934,1935,1936,1937,1938,1939,1940,1941,1942,1943,
                1944,1945,1946,1947,1948,1949,1950,1951,1952,1953,1954,1955,1956,1957,1957,1958,1959,1960,1961,1962,1963,1964,1965,1966,1967,1968,1969,1970,1971,1972,1973,1974,
                1975,1976,1977,1978,1979,1979,1980,1981,1982,1983,1984,1985,1986,1987,1988,1989,1990,1991,1992,1993,1994,1995,1996,1997,1998,1999,2000,2001,2001,2002,2003,2004,
                2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015,2016,2017,2018,2019,2020,2021,2022,2023,2023,2024,2025,2026,2027,2028,2029,2030,2031,2032,2033,2034,2035,
                2036,2037,2038,2039,2040,2041,2042,2043,2044,2044,2045,2046,2047,2048,2049,2050,2051,2052,2053,2054,2055,2056,2057,2058,2059,2060,2061,2062,2063,2064,2064,2065,
                2066,2067,2068,2069,2070,2071,2072,2073,2074,2075,2076,2077,2078,2079,2080,2081,2082,2083,2084,2084,2085,2086,2087,2088,2089,2090,2091,2092,2093,2094,2095,2096,
                2097,2098,2099,2100,2101,2102,2103,2104,2104,2105,2106,2107,2108,2109,2110,2111,2112,2113,2114,2115,2116,2117,2118,2119,2120,2121,2122,2123,2123,2124,2125,2126,
                2127,2128,2129,2130,2131,2132,2133,2134,2135,2136,2137,2138,2139,2140,2141,2142,2142,2143,2144,2145,2146,2147,2148,2149,2150,2151,2152,2153,2154,2155,2156,2157,
                2158,2159,2160,2161,2161,2162,2163,2164,2165,2166,2167,2168,2169,2170,2171,2172,2168,2169,2170,2171,2172,2173,2174,2174,2175,2176,2177,2178,2179,2180,2180,2181,
                2182,2183,2184,2185,2186,2186,2187,2188,2189,2190,2191,2191,2192,2193,2194,2195,2196,2197,2197,2198,2199,2200,2201,2202,2202,2203,2204,2205,2206,2207,2208,2208,
                2209,2210,2211,2212,2213,2213,2214,2215,2216,2217,2218,2219,2219,2220,2221,2222,2223,2224,2224,2225,2226,2227,2228,2229,2230,2230,2231,2232,2233,2234,2235,2235,
                2236,2237,2238,2239,2240,2240,2241,2242,2243,2244,2245,2245,2246,2247,2248,2249,2250,2251,2251,2252,2253,2254,2255,2256,2256,2257,2258,2259,2260,2261,2261,2262,
                2263,2264,2265,2266,2266,2267,2268,2269,2270,2271,2271,2272,2273,2274,2275,2276,2276,2277,2278,2279,2280,2281,2281,2282,2283,2284,2285,2286,2286,2287,2288,2289,
                2290,2291,2291,2292,2293,2294,2295,2296,2296,2297,2298,2299,2300,2301,2301,2302,2303,2304,2305,2306,2306,2307,2308,2309,2310,2311,2311,2312,2313,2314,2315,2316,
                2316,2317,2318,2319,2320,2320,2321,2322,2323,2324,2325,2325,2326,2327,2328,2329,2330,2330,2331,2332,2333,2334,2335,2335,2336,2337,2338,2339,2339,2340,2341,2342,
                2343,2344,2344,2345,2346,2347,2348,2349,2349,2350,2351,2352,2353,2353,2354,2355,2356,2357,2358,2358,2359,2360,2361,2362,2362,2363,2364,2365,2366,2367,2367,2368,
                2369,2370,2371,2371,2372,2373,2374,2375,2376,2376,2377,2378,2379,2380,2380,2381,2382,2383,2384,2385,2385,2386,2387,2388,2389,2389,2390,2391,2392,2393,2394,2394,
                2395,2396,2397,2398,2398,2399,2400,2401,2402,2402,2403,2404,2405,2406,2407,2407,2408,2409,2410,2411,2411,2412,2413,2414,2415,2415,2416,2417,2418,2419,2420,2420,
                2421,2422,2423,2424,2424,2425,2426,2427,2428,2428,2429,2430,2431,2432,2432,2433,2434,2435,2436,2436,2437,2438,2439,2440,2441,2441,2442,2443,2444,2445,2445,2446,
                2447,2448,2449,2449,2450,2451,2452,2453,2453,2454,2455,2456,2457,2457,2458,2459,2460,2461,2461,2462,2463,2464,2465,2465,2466,2467,2468,2469,2469,2470,2471,2472,
                2473,2473,2474,2475,2476,2477,2477,2478,2479,2480,2481,2481,2482,2483,2484,2485,2485,2486,2487,2488,2489,2489,2490,2491,2492,2493,2493,2494,2495,2496,2497,2497,
                2498,2499,2500,2501,2501,2502,2503,2504,2505,2505,2506,2507,2508,2509,2509,2510,2511,2512,2512,2513,2514,2515,2516,2516,2517,2518,2519,2520,2520,2521,2522,2523,
                2524,2524,2525,2526,2527,2528,2528,2529,2530,2531,2531,2532,2533,2534,2535,2535,2536,2537,2538,2539,2539,2540,2541,2542,2543,2543,2544,2545,2546,2546,2547,2548,
                2549,2550,2550,2551,2552,2553,2554,2554,2555,2556,2557,2557,2558,2559,2560,2561,2561,2562,2563,2564,2565,2565,2566,2567,2568,2568,2569,2570,2571,2572,2572,2573,
                2574,2575,2575,2576,2577,2578,2579,2579,2580,2581,2582,2583,2583,2584,2585,2586,2586,2587,2588,2589,2590,2590,2591,2592,2593,2593,2594,2595,2596,2597,2597,2598,
                2599,2600,2600,2601,2602,2603,2604,2604,2605,2606,2607,2607,2608,2609,2610,2611,2611,2612,2613,2614,2614,2615,2616,2617,2617,2618,2619,2620,2621,2621,2622,2623,
                2624,2624,2625,2626,2627,2628,2628,2629,2630,2631,2631,2632,2633,2634,2634,2635,2636,2637,2638,2638,2639,2640,2641,2641,2642,2643,2644,2645,2645,2646,2647,2648,
                2648,2649,2650,2651,2651,2652,2653,2654,2655,2656,2657,2658,2658,2659,2660,2660,2661,2662,2662,2663,2664,2664,2665,2666,2666,2667,2668,2668,2669,2670,2670,2671,
                2672,2672,2673,2674,2674,2675,2676,2676,2677,2677,2678,2679,2679,2680,2681,2681,2682,2683,2683,2684,2685,2685,2686,2687,2687,2688,2689,2689,2690,2691,2691,2692,
                2693,2693,2694,2695,2695,2696,2697,2697,2698,2699,2699,2700,2701,2701,2702,2703,2703,2704,2705,2705,2706,2706,2707,2708,2708,2709,2710,2710,2711,2712,2712,2713,
                2714,2714,2715,2716,2716,2717,2718,2718,2719,2719,2720,2721,2721,2722,2723,2723,2724,2725,2725,2726,2727,2727,2728,2729,2729,2730,2731,2731,2732,2732,2733,2734,
                2734,2735,2736,2736,2737,2738,2738,2739,2740,2740,2741,2741,2742,2743,2743,2744,2745,2745,2746,2747,2747,2748,2749,2749,2750,2750,2751,2752,2752,2753,2754,2754,
                2755,2756,2756,2757,2757,2758,2759,2759,2760,2761,2761,2762,2763,2763,2764,2764,2765,2766,2766,2767,2768,2768,2769,2770,2770,2771,2771,2772,2773,2773,2774,2775,
                2775,2776,2776,2777,2778,2778,2779,2780,2780,2781,2781,2782,2783,2783,2784,2785,2785,2786,2787,2787,2788,2788,2789,2790,2790,2791,2792,2792,2793,2793,2794,2795,
                2795,2796,2797,2797,2798,2798,2799,2800,2800,2801,2802,2802,2803,2803,2804,2805,2805,2806,2806,2807,2808,2808,2809,2810,2810,2811,2811,2812,2813,2813,2814,2815,
                2815,2816,2816,2817,2818,2818,2819,2819,2820,2821,2821,2822,2823,2823,2824,2824,2825,2826,2826,2827,2827,2828,2829,2829,2830,2831,2831,2832,2832,2833,2834,2834,
                2835,2835,2836,2837,2837,2838,2838,2839,2840,2840,2841,2842,2842,2843,2843,2844,2845,2845,2846,2846,2847,2848,2848,2849,2849,2850,2851,2851,2852,2852,2853,2854,
                2854,2855,2855,2856,2857,2857,2858,2858,2859,2860,2860,2861,2861,2862,2863,2863,2864,2864,2865,2866,2866,2867,2867,2868,2869,2869,2870,2870,2871,2872,2872,2873,
                2873,2874,2875,2875,2876,2876,2877,2878,2878,2879,2879,2880,2881,2881,2882,2882,2883,2884,2884,2885,2885,2886,2887,2887,2888,2888,2889,2889,2890,2891,2891,2892,
                2892,2893,2894,2894,2895,2895,2896,2897,2897,2898,2898,2899,2900,2900,2901,2901,2902,2902,2903,2904,2904,2905,2905,2906,2907,2907,2908,2908,2909,2909,2910,2911,
                2911,2912,2912,2913,2914,2914,2915,2915,2916,2916,2917,2918,2918,2919,2919,2920,2921,2921,2922,2922,2923,2923,2924,2925,2925,2926,2926,2927,2927,2928,2929,2929,
                2930,2930,2931,2931,2932,2933,2933,2934,2934,2935,2936,2936,2937,2937,2938,2938,2939,2940,2940,2941,2941,2942,2942,2943,2944,2944,2945,2945,2946,2946,2947,2948,
                2948,2949,2949,2950,2950,2951,2952,2952,2953,2953,2954,2954,2955,2956,2956,2957,2957,2958,2958,2959,2959,2960,2961,2961,2962,2962,2963,2963,2964,2965,2965,2966,
                2966,2967,2967,2968,2968,2969,2970,2970,2971,2971,2972,2972,2973,2974,2974,2975,2975,2976,2976,2977,2977,2978,2979,2979,2980,2980,2981,2981,2982,2982,2983,2984,
                2984,2985,2985,2986,2986,2987,2987,2988,2989,2989,2990,2990,2991,2991,2992,2992,2993,2994,2994,2995,2995,2996,2996,2997,2997,2998,2999,2999,3000,3000,3001,3001,
                3002,3002,3003,3003,3004,3005,3005,3006,3006,3007,3007,3008,3008,3009,3010,3010,3011,3011,3012,3012,3013,3013,3014,3014,3015,3016,3016,3017,3017,3018,3018,3019,
                3019,3020,3020,3021,3021,3022,3023,3023,3024,3024,3025,3025,3026,3026,3027,3027,3028,3028,3029,3030,3030,3031,3031,3032,3032,3033,3033,3034,3034,3035,3035,3036,
                3037,3037,3038,3038,3039,3039,3040,3040,3041,3041,3042,3042,3043,3044,3044,3045,3045,3046,3046,3047,3047,3048,3048,3049,3049,3050,3050,3051,3051,3052,3053,3053,
                3054,3054,3055,3055,3056,3056,3057,3057,3058,3058,3059,3059,3060,3060,3061,3062,3062,3063,3063,3064,3064,3065,3065,3066,3066,3067,3067,3068,3068,3069,3069,3070,
                3070,3071,3071,3072,3073,3074,3075,3075,3076,3076,3077,3077,3078,3078,3079,3080,3080,3081,3081,3082,3082,3083,3083,3084,3085,3085,3086,3086,3087,3087,3088,3088,
                3089,3090,3090,3091,3091,3092,3092,3093,3093,3094,3095,3095,3096,3096,3097,3097,3098,3099,3099,3100,3100,3101,3101,3102,3102,3103,3104,3104,3105,3105,3106,3106,
                3107,3108,3108,3109,3109,3110,3110,3111,3111,3112,3113,3113,3114,3114,3115,3115,3116,3117,3117,3118,3118,3119,3119,3120,3120,3121,3122,3122,3123,3123,3124,3124,
                3125,3126,3126,3127,3127,3128,3128,3129,3130,3130,3131,3131,3132,3132,3133,3133,3134,3135,3135,3136,3136,3137,3137,3138,3139,3139,3140,3140,3141,3141,3142,3143,
                3143,3144,3144,3145,3145,3146,3147,3147,3148,3148,3149,3149,3150,3151,3151,3152,3152,3153,3153,3154,3155,3155,3156,3156,3157,3157,3158,3159,3159,3160,3160,3161,
                3161,3162,3163,3163,3164,3164,3165,3165,3166,3167,3167,3168,3168,3169,3170,3170,3171,3171,3172,3172,3173,3174,3174,3175,3175,3176,3176,3177,3178,3178,3179,3179,
                3180,3180,3181,3182,3182,3183,3183,3184,3185,3185,3186,3186,3187,3187,3188,3189,3189,3190,3190,3191,3192,3192,3193,3193,3194,3194,3195,3196,3196,3197,3197,3198,
                3198,3199,3200,3200,3201,3201,3202,3203,3203,3204,3204,3205,3205,3206,3207,3207,3208,3208,3209,3210,3210,3211,3211,3212,3213,3213,3214,3214,3215,3215,3216,3217,
                3217,3218,3218,3219,3220,3220,3221,3221,3222,3222,3223,3224,3224,3225,3225,3226,3227,3227,3228,3228,3229,3230,3230,3231,3231,3232,3233,3233,3234,3234,3235,3235,
                3236,3237,3237,3238,3238,3239,3240,3240,3241,3241,3242,3243,3243,3244,3244,3245,3246,3246,3247,3247,3248,3248,3249,3250,3250,3251,3251,3252,3253,3253,3254,3254,
                3255,3256,3256,3257,3257,3258,3259,3259,3260,3260,3261,3262,3262,3263,3263,3264,3265,3265,3266,3266,3267,3268,3268,3269,3269,3270,3271,3271,3272,3272,3273,3274,
                3274,3275,3275,3276,3277,3277,3278,3278,3279,3280,3280,3281,3281,3282,3283,3283,3284,3284,3285,3286,3286,3287,3287,3288,3289,3289,3290,3290,3291,3292,3292,3293,
                3293,3294,3295,3295,3296,3296,3297,3298,3298,3299,3299,3300,3301,3301,3302,3302,3303,3304,3304,3305,3306,3307,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,
                3313,3314,3314,3315,3315,3316,3316,3317,3317,3318,3318,3319,3320,3320,3321,3321,3322,3322,3323,3323,3324,3324,3325,3325,3326,3327,3327,3328,3328,3329,3329,3330,
                3330,3331,3331,3332,3332,3333,3334,3334,3335,3335,3336,3336,3337,3337,3338,3338,3339,3340,3340,3341,3341,3342,3342,3343,3343,3344,3344,3345,3345,3346,3347,3347,
                3348,3348,3349,3349,3350,3350,3351,3351,3352,3352,3353,3354,3354,3355,3355,3356,3356,3357,3357,3358,3358,3359,3359,3360,3361,3361,3362,3362,3363,3363,3364,3364,
                3365,3365,3366,3367,3367,3368,3368,3369,3369,3370,3370,3371,3371,3372,3372,3373,3373,3374,3375,3375,3376,3376,3377,3377,3378,3378,3379,3379,3380,3380,3381,3382,
                3382,3383,3383,3384,3384,3385,3385,3386,3386,3387,3387,3388,3389,3389,3390,3390,3391,3391,3392,3392,3393,3393,3394,3394,3395,3396,3396,3397,3397,3398,3398,3399,
                3399,3400,3400,3401,3401,3402,3403,3403,3404,3404,3405,3405,3406,3406,3407,3407,3408,3408,3409,3409,3410,3411,3411,3412,3412,3413,3413,3414,3414,3415,3415,3416,
                3416,3417,3418,3418,3419,3419,3420,3420,3421,3421,3422,3422,3423,3423,3424,3424,3425,3426,3426,3427,3427,3428,3428,3429,3429,3430,3430,3431,3431,3432,3432,3433,
                3434,3434,3435,3435,3436,3436,3437,3437,3438,3438,3439,3439,3440,3440,3441,3442,3442,3443,3443,3444,3444,3445,3445,3446,3446,3447,3447,3448,3448,3449,3450,3450,
                3451,3451,3452,3452,3453,3453,3454,3454,3455,3455,3456,3456,3457,3458,3458,3459,3459,3460,3460,3461,3461,3462,3462,3463,3463,3464,3464,3465,3466,3466,3467,3467,
                3468,3468,3469,3469,3470,3470,3471,3471,3472,3472,3473,3474,3474,3475,3475,3476,3476,3477,3477,3478,3478,3479,3479,3480,3480,3481,3481,3482,3483,3483,3484,3484,
                3485,3485,3486,3486,3487,3487,3488,3488,3489,3489,3490,3491,3491,3492,3492,3493,3493,3494,3494,3495,3495,3496,3496,3497,3497,3498,3498,3499,3500,3500,3501,3501,
                3502,3502,3503,3503,3504,3504,3505,3505,3506,3506,3507,3507,3508,3509,3509,3510,3510,3511,3511,3512,3512,3513,3513,3514,3514,3515,3515,3516,3516,3517,3518,3518,
                3519,3519,3520,3520,3521,3521,3522,3522,3523,3523,3524,3524,3525,3525,3526,3526,3527,3528,3528,3529,3529,3530,3530,3531,3531,3532,3532,3533,3533,3534,3534,3535,
                3535,3536,3537,3537,3538,3538,3539,3539,3540,3540,3541,3541,3542,3542,3543,3543,3544,3544,3545,3545,3546,3547,3547,3548,3548,3549,3549,3550,3550,3551,3551,3552,
                3552,3553,3553,3554,3554,3555,3555,3556,3557,3557,3558,3558,3559,3559,3560,3560,3561,3561,3562,3562,3563,3563,3564,3564,3565,3565,3566,3567,3567,3568,3568,3569,
                3569,3570,3570,3571,3571,3572,3572,3573,3573,3574,3574,3575,3575,3576,3577,3577,3578,3578,3579,3579,3580,3580,3581,3581,3582,3582,3583,3583,3584,3584,3585,3585,
                3586,3586,3587,3588,3588,3589,3589,3590,3590,3591,3591,3592,3592,3593,3593,3594,3594,3595,3595,3596,3596,3597,3597,3598,3599,3599,3600,3600,3601,3601,3602,3602,
                3603,3603,3604,3604,3605,3605,3606,3606,3607,3607,3608,3608,3609,3609,3610,3611,3611,3612,3612,3613,3613,3614,3614,3615,3615,3616,3616,3617,3617,3618,3618,3619,
                3619,3620,3620,3621,3621,3622,3623,3623,3624,3624,3625,3625,3626,3626,3627,3627,3628,3628,3629,3629,3630,3630,3631,3631,3632,3632,3633,3633,3634,3635,3635,3636,
                3636,3637,3637,3638,3638,3639,3639,3640,3640,3641,3641,3642,3642,3643,3643,3644,3644,3645,3645,3646,3646,3647,3648,3648,3649,3649,3650,3650,3651,3651,3652,3652,
                3653,3653,3654,3654,3655,3655,3656,3656,3657,3657,3658,3658,3659,3659,3660,3661,3661,3662,3662,3663,3663,3664,3664,3665,3665,3666,3666,3667,3667,3668,3668,3669,
                3669,3670,3670,3671,3671,3672,3672,3673,3673,3674,3675,3675,3676,3676,3677,3677,3678,3678,3679,3679,3680,3680,3681,3681,3682,3682,3683,3683,3684,3684,3685,3685,
                3686,3686,3687,3687,3688,3688,3689,3690,3690,3691,3691,3692,3692,3693,3693,3694,3694,3695,3695,3696,3696,3697,3697,3698,3698,3699,3699,3700,3700,3701,3701,3702,
                3702,3703,3703,3704,3705,3705,3706,3706,3707,3707,3708,3708,3709,3709,3710,3710,3711,3711,3712,3712,3713,3713,3714,3714,3715,3715,3716,3716,3717,3717,3718,3718,
                3719,3719,3720,3720,3721,3722,3722,3723,3723,3724,3724,3725,3725,3726,3726,3727,3727,3728,3728,3729,3729,3730,3730,3731,3731,3732,3732,3733,3733,3734,3734,3735,
                3735,3736,3736,3737,3737,3738,3738,3739,3740,3740,3741,3741,3742,3742,3743,3743,3744,3744,3745,3745,3746,3746,3747,3747,3748,3748,3749,3749,3750,3750,3751,3751,
                3752,3752,3753,3753,3754,3754,3755,3755,3756,3756,3757,3757,3758,3759,3759,3760,3760,3761,3761,3762,3762,3763,3763,3764,3764,3765,3765,3766,3766,3767,3767,3768,
                3768,3769,3769,3770,3770,3771,3771,3772,3772,3773,3773,3774,3774,3775,3775,3776,3776,3777,3777,3778,3778,3779,3779,3780,3781,3781,3782,3783,3784,3785,3786,3787,
                3787,3788,3789,3790,3791,3792,3792,3793,3794,3795,3796,3797,3798,3798,3799,3800,3801,3802,3803,3804,3805,3805,3806,3807,3808,3809,3810,3811,3812,3813,3813,3814,
                3815,3816,3817,3818,3819,3820,3821,3821,3822,3823,3824,3825,3826,3827,3828,3829,3830,3831,3832,3832,3833,3834,3835,3836,3837,3838,3839,3840,3841,3842,3843,3844,
                3845,3845,3846,3847,3848,3849,3850,3851,3852,3853,3854,3855,3856,3857,3858,3859,3860,3861,3862,3863,3864,3865,3866,3867,3868,3869,3870,3870,3871,3872,3873,3874,
                3875,3876,3877,3878,3879,3880,3881,3882,3883,3884,3885,3886,3887,3888,3889,3890,3891,3892,3893,3894,3895,3897,3898,3899,3900,3901,3902,3903,3904,3905,3906,3907,
                3908,3909,3910,3911,3912,3913,3914,3915,3916,3917,3918,3919,3920,3921,3922,3924,3925,3926,3927,3928,3929,3930,3931,3932,3933,3934,3935,3936,3937,3939,3940,3941,
                3942,3943,3944,3945,3946,3947,3948,3949,3950,3952,3953,3954,3955,3956,3957,3958,3959,3960,3962,3963,3964,3965,3966,3967,3968,3969,3970,3972,3973,3974,3975,3976,
                3977,3978,3979,3981,3982,3983,3984,3985,3986,3987,3989,3990,3991,3992,3993,3994,3996,3997,3998,3999,4000,4001,4002,4004,4005,4006,4007,4008,4010,4011,4012,4013,
                4014,4015,4017,4018,4019,4020,4021,4023,4024,4025,4026,4027,4028,4030,4031,4032,4033,4034,4036,4037,4038,4039,4041,4042,4043,4044,4045,4047,4048,4049,4050,4052,
                4053,4054,4055,4056,4058,4059,4060,4061,4063,4064,4065,4066,4068,4069,4070,4071,4073,4074,4075,4076,4078,4079,4080,4081,4083,4084,4085,4086,4088,4089,4090,4092,
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
             250,
          },
          /* aec trigger 2 */
          {
             260,
             320,
          },
          /* aec trigger 3 */
          {
             330,
             480,
          },
          /* aec trigger 4 */
          {
             480,
             500,
          },
       },
       /* isp_sub_module_gtm2_lut_pra pra[MAX_TRIGGER_COUNT] */
       {
          /* para 0 */
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
             4,//soft_sample_rate
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
             4,//soft_sample_rate
          },
          /* para 2 */
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
             4,//soft_sample_rate
          },
          /* para 3 */
          {
             1,//enable_cdf_smooth
             0.0015,//cut_ration
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
             4,//soft_sample_rate
          },
          /* para 4 */
          {
             1,//enable_cdf_smooth
             0.012,//cut_ration
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
             120,
          },
          /* aec trigger 1 */
          {
             160,
             250,
          },
          /* aec trigger 2 */
          {
             260,
             360,
          },
          /* aec trigger 3 */
          {
             380,
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
                0.9,//saturation
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
                1,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                0.9,//saturation
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
                0.9,//saturation
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
                1.2,//saturation
                0,//hue
             },
             /* awb para 6 */
             {
                1.1,//saturation
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
                0.9,//saturation
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
             251,
             256.1,
          },
          /* aec trigger 9 */
          {
             512,
             512.1,
          },
          /* aec trigger 10 */
          {
             1000,
             1000.1,
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
                -2,-6,-13,-16,//bpf(1,1:4)
                -6,-27,-80,-126,//bpf(2,1:4)
                -13,-80,-142,-24,//bpf(3,1:4)
                -16,-126,-24,2140,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             130,//strength_pos_edge
             140,//strength_neg_edge
             140,//overshoot_pos_edge
             -160,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,4,8,12,16,20,24,28,32,36,40,44,48,48,48,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             100,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
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
                -2,-6,-13,-17,//bpf(1,1:4)
                -6,-31,-98,-157,//bpf(2,1:4)
                -13,-98,-145,81,//bpf(3,1:4)
                -17,-157,81,2020,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             125,//strength_pos_edge
             135,//strength_neg_edge
             140,//overshoot_pos_edge
             -160,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,60,56,52,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,32,32,32,32,32,32},//edge_w[64]
             0,//limitation_for_move_strength
             120,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
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
                -1,-4,-13,-17,//bpf(1,1:4)
                -4,-30,-92,-147,//bpf(2,1:4)
                -13,-92,-175,106,//bpf(3,1:4)
                -17,-147,106,1928,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             95,//strength_pos_edge
             105,//strength_neg_edge
             130,//overshoot_pos_edge
             -145,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,4,8,12,16,16,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,60,56,52,48,44,40,36,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32},//edge_w[64]
             0,//limitation_for_move_strength
             160,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
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
                -1,-4,-13,-17,//bpf(1,1:4)
                -4,-30,-92,-147,//bpf(2,1:4)
                -13,-92,-175,106,//bpf(3,1:4)
                -17,-147,106,1928,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             105,//strength_pos_edge
             115,//strength_neg_edge
             130,//overshoot_pos_edge
             -145,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,2,4,8,12,16,20,24,28,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             160,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {16,16,16,16,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
                -1,-4,-13,-17,//bpf(1,1:4)
                -4,-30,-92,-147,//bpf(2,1:4)
                -13,-92,-175,106,//bpf(3,1:4)
                -17,-147,106,1928,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             95,//strength_pos_edge
             105,//strength_neg_edge
             130,//overshoot_pos_edge
             -140,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,4,8,12,16,20,24,28,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             160,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {16,16,16,16,24,32,40,48,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,4,8,16,24,32,32,32,32,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
                -2,-6,-13,-17,//bpf(1,1:4)
                -6,-31,-98,-157,//bpf(2,1:4)
                -13,-98,-145,81,//bpf(3,1:4)
                -17,-157,81,2020,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             90,//strength_pos_edge
             90,//strength_neg_edge
             130,//overshoot_pos_edge
             -140,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             160,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {32,32,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,4,8,12,16,20,24,28,32,32,32,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
                -3,-8,-16,-20,//bpf(1,1:4)
                -8,-36,-99,-152,//bpf(2,1:4)
                -16,-99,-109,110,//bpf(3,1:4)
                -20,-152,110,1824,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             90,//strength_pos_edge
             90,//strength_neg_edge
             120,//overshoot_pos_edge
             -130,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,32,36,40,44,48,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             255,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {0,0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,0,0,2,4,6,8,12,16,20,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             110,//overshoot_pos_edge
             -120,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,0,0,0,0,8,16,16,16,16,16,16,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             155,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {0,4,8,12,16,20,24,28,32,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,0,0,2,4,6,8,10,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
                -4,-14,-25,-29,//bpf(1,1:4)
                -14,-49,-90,-109,//bpf(2,1:4)
                -25,-90,43,132,//bpf(3,1:4)
                -29,-109,132,1096,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             80,//strength_pos_edge
             80,//strength_neg_edge
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
             {0,0,0,0,0,0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             0,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {0,0,0,0,0,0,0,28,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
                -6,-17,-30,-35,//bpf(1,1:4)
                -17,-59,-106,-128,//bpf(2,1:4)
                -30,-106,50,259,//bpf(3,1:4)
                -35,-128,259,900,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             70,//strength_pos_edge
             70,//strength_neg_edge
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
             {0,0,0,0,0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,28,28,28,28,28,28,28,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             250,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {0,0,0,4,8,12,16,20,24,28,32,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
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
             60,//strength_pos_edge
             60,//strength_neg_edge
             80,//overshoot_pos_edge
             -80,//overshoot_neg_edge
             {77,92,112,127},//colour_u_p[4]
             {0,-422,128,797,0},//colour_u_pdes[5]
             {0,5,0,-6,0},//colour_u_slop[5]
             {133,143,163,173},//colour_v_p[4]
             {0,-1154,128,1588,0},//colour_v_pdes[5]
             {0,8,0,-9,0},//colour_v_slop[5]
             114,//conv_3x3_ration_constant
             146,//conv_1x7_ration_constant
             {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,28,28,28,28,28,28,28,28,28,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             255,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {0,0,0,2,4,8,12,16,20,24,28,32,32,32,32,32,32,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {0,0,0,2,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             251,
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
             12,//nr3d_mpy_th1
             18,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             12,//nr3d_mpc_th1
             18,//nr3d_mpc_th2
             15,//nr3d_lamda2d
             15,//nr3d_ite
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
             16,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             25,//nr3d_mpc_th1
             31,//nr3d_mpc_th2
             15,//nr3d_lamda2d
             15,//nr3d_ite
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
             16,//nr3d_mpy_th1
             20,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             16,//nr3d_mpc_th1
             25,//nr3d_mpc_th2
             15,//nr3d_lamda2d
             18,//nr3d_ite
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
             15,//nr3d_mpy_th1
             28,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             16,//nr3d_mpc_th1
             20,//nr3d_mpc_th2
             15,//nr3d_lamda2d
             18,//nr3d_ite
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
             30,//dbk_satu
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
             25,//nr3d_mpy_th1
             45,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             90,//nr3d_npc_th3
             180,//nr3d_npc_th4
             20,//nr3d_mpc_th1
             25,//nr3d_mpc_th2
             20,//nr3d_lamda2d
             25,//nr3d_ite
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
             50,//dbk_satu
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
             40,//nr3d_mpy_th1
             60,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             20,//nr3d_mpc_th1
             35,//nr3d_mpc_th2
             40,//nr3d_lamda2d
             33,//nr3d_ite
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
             80,//dbk_satu
             1,//dbk_gaus_y_c11
             13,//dbk_gaus_y_c12
             31,//dbk_gaus_y_c13
             2,//dbk_gaus_y_c21
             72,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,16,17,17,17,18,18,18,19,19,19,20,20,21,21,21,22,22,22,23,23,23,24,24,25,25,25,26,26,26,27,27,28,28,28,29,29,29,30,30,30,31,31,31,
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
             50,//nr3d_mpy_th1
             70,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             30,//nr3d_mpc_th1
             50,//nr3d_mpc_th2
             40,//nr3d_lamda2d
             33,//nr3d_ite
             0,//nr3d_msr
             240,//nr3d_decay
             30,//nr3d_satu
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
             80,//dbk_satu
             1,//dbk_gaus_y_c11
             13,//dbk_gaus_y_c12
             31,//dbk_gaus_y_c13
             2,//dbk_gaus_y_c21
             72,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,0,0,0,1,2,3,4,5,6,7,8,8,8,8,9,10,11,12,13,14,15,16,17,18,18,18,19,19,19,20,20,21,21,21,22,22,22,24,24,24,26,26,28,28,28,30,30,30,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
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
             70,//nr3d_mpy_th1
             100,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             80,//nr3d_npc_th3
             180,//nr3d_npc_th4
             40,//nr3d_mpc_th1
             55,//nr3d_mpc_th2
             80,//nr3d_lamda2d
             30,//nr3d_ite
             0,//nr3d_msr
             240,//nr3d_decay
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
             50,//dbk_h_mv_diff
             70,//dbk_h_mv_str
             50,//dbk_v_mv_diff
             70,//dbk_v_mv_str
             85,//dbk_h_yy_flat
             70,//dbk_h_yy_diff
             85,//dbk_v_yy_flat
             70,//dbk_v_yy_diff
             120,//dbk_satu
             1,//dbk_gaus_y_c11
             13,//dbk_gaus_y_c12
             31,//dbk_gaus_y_c13
             2,//dbk_gaus_y_c21
             72,//dbk_gaus_y_c23
             0,//bypass_dbk_saturation_adjust
             1,//md_yuv_sel
             1,//md_1st_media_filter_disable
             0,//md_noise_profile_selection
             0,//md_divs_selection
             0,//md_expand_ration
             0,//md_sort_sel
             /* noise profile y */
             {
                0,0,0,0,0,1,2,3,4,5,6,7,8,8,8,8,8,8,8,8,9,10,11,12,13,14,15,16,16,16,16,16,16,16,16,17,18,19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
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
             110,//nr3d_mpy_th1
             190,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             44,//nr3d_mpc_th1
             66,//nr3d_mpc_th2
             160,//nr3d_lamda2d
             40,//nr3d_ite
             0,//nr3d_msr
             240,//nr3d_decay
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
                0,0,0,0,0,1,2,2,2,3,4,5,6,7,8,8,8,8,8,8,8,8,8,8,8,8,9,10,11,12,12,12,13,14,15,16,16,16,17,18,19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
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
             65,//nr3d_mpy_th1
             75,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             50,//nr3d_mpc_th1
             60,//nr3d_mpc_th2
             40,//nr3d_lamda2d
             30,//nr3d_ite
             0,//nr3d_msr
             240,//nr3d_decay
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
                0,0,0,0,0,0,0,1,2,2,2,3,4,4,4,5,6,7,8,8,8,8,8,8,8,8,9,10,11,12,13,14,15,16,16,16,16,16,16,16,17,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
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
             70,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             60,//nr3d_mpc_th1
             70,//nr3d_mpc_th2
             40,//nr3d_lamda2d
             30,//nr3d_ite
             0,//nr3d_msr
             240,//nr3d_decay
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
                0,0,0,0,0,0,0,0,0,1,2,2,2,3,4,4,4,4,5,6,7,8,8,8,8,8,8,8,8,8,9,10,11,12,13,14,15,15,15,15,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
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
             1,
             4,
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
                {"dark_iso100_400_param.npubin"},//net_name
                0.35,//scale
                {
                   0.0040943,//k_coeff[0]
                   0.0141648,//k_coeff[1]
                },
                {
                   3.89845e-06,//b_coeff[0]
                   0.0688134,//b_coeff[1]
                   0,//b_coeff[2]
                },
                /* blc */
                {961,961,961,961},
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
             310,
          },
          /* aec trigger 2 */
          {
             320,
             370,
          },
          /* aec trigger 3 */
          {
             380,
             430,
          },
          /* aec trigger 4 */
          {
             490,
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
                1.1,//saturation
                0,//hue
                10,//y_lo_th1
                40,//y_lo_th2
                1000,//y_hi_th1
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
                0.6,//saturation
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
                0.6,//saturation
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
       /* isp_sub_module_lsc_pra */
       {
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
      445,
      /* ev0_count */
      445,
      /* ev2_count */
      445,
      /* fix_fps_index */
      25,
      /* enable_isp_digital_gain */
      0,
      /* max_sensor_gain */
      255,
      /* max_isp_gain1 */
      1,
      /* exp_table */
      {
      //1,2994
      //255,2994
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
         {    256,   1957   },         /* Gain = 1.000000 Exposure Index = 245 */
         {    256,   2016   },         /* Gain = 1.000000 Exposure Index = 246 */
         {    256,   2077   },         /* Gain = 1.000000 Exposure Index = 247 */
         {    256,   2140   },         /* Gain = 1.000000 Exposure Index = 248 */
         {    256,   2205   },         /* Gain = 1.000000 Exposure Index = 249 */
         {    256,   2272   },         /* Gain = 1.000000 Exposure Index = 250 */
         {    256,   2341   },         /* Gain = 1.000000 Exposure Index = 251 */
         {    256,   2412   },         /* Gain = 1.000000 Exposure Index = 252 */
         {    256,   2485   },         /* Gain = 1.000000 Exposure Index = 253 */
         {    256,   2560   },         /* Gain = 1.000000 Exposure Index = 254 */
         {    256,   2637   },         /* Gain = 1.000000 Exposure Index = 255 */
         {    256,   2717   },         /* Gain = 1.000000 Exposure Index = 256 */
         {    256,   2799   },         /* Gain = 1.000000 Exposure Index = 257 */
         {    256,   2883   },         /* Gain = 1.000000 Exposure Index = 258 */
         {    256,   2970   },         /* Gain = 1.000000 Exposure Index = 259 */
         {    262,   2994   },         /* Gain = 1.023438 Exposure Index = 260 */
         {    270,   2994   },         /* Gain = 1.054688 Exposure Index = 261 */
         {    279,   2994   },         /* Gain = 1.089844 Exposure Index = 262 */
         {    288,   2994   },         /* Gain = 1.125000 Exposure Index = 263 */
         {    297,   2994   },         /* Gain = 1.160156 Exposure Index = 264 */
         {    306,   2994   },         /* Gain = 1.195312 Exposure Index = 265 */
         {    316,   2994   },         /* Gain = 1.234375 Exposure Index = 266 */
         {    326,   2994   },         /* Gain = 1.273438 Exposure Index = 267 */
         {    336,   2994   },         /* Gain = 1.312500 Exposure Index = 268 */
         {    347,   2994   },         /* Gain = 1.355469 Exposure Index = 269 */
         {    358,   2994   },         /* Gain = 1.398438 Exposure Index = 270 */
         {    369,   2994   },         /* Gain = 1.441406 Exposure Index = 271 */
         {    381,   2994   },         /* Gain = 1.488281 Exposure Index = 272 */
         {    393,   2994   },         /* Gain = 1.535156 Exposure Index = 273 */
         {    405,   2994   },         /* Gain = 1.582031 Exposure Index = 274 */
         {    418,   2994   },         /* Gain = 1.632812 Exposure Index = 275 */
         {    431,   2994   },         /* Gain = 1.683594 Exposure Index = 276 */
         {    444,   2994   },         /* Gain = 1.734375 Exposure Index = 277 */
         {    458,   2994   },         /* Gain = 1.789062 Exposure Index = 278 */
         {    472,   2994   },         /* Gain = 1.843750 Exposure Index = 279 */
         {    487,   2994   },         /* Gain = 1.902344 Exposure Index = 280 */
         {    502,   2994   },         /* Gain = 1.960938 Exposure Index = 281 */
         {    518,   2994   },         /* Gain = 2.023438 Exposure Index = 282 */
         {    534,   2994   },         /* Gain = 2.085938 Exposure Index = 283 */
         {    551,   2994   },         /* Gain = 2.152344 Exposure Index = 284 */
         {    568,   2994   },         /* Gain = 2.218750 Exposure Index = 285 */
         {    586,   2994   },         /* Gain = 2.289062 Exposure Index = 286 */
         {    604,   2994   },         /* Gain = 2.359375 Exposure Index = 287 */
         {    623,   2994   },         /* Gain = 2.433594 Exposure Index = 288 */
         {    642,   2994   },         /* Gain = 2.507812 Exposure Index = 289 */
         {    662,   2994   },         /* Gain = 2.585938 Exposure Index = 290 */
         {    682,   2994   },         /* Gain = 2.664062 Exposure Index = 291 */
         {    703,   2994   },         /* Gain = 2.746094 Exposure Index = 292 */
         {    725,   2994   },         /* Gain = 2.832031 Exposure Index = 293 */
         {    747,   2994   },         /* Gain = 2.917969 Exposure Index = 294 */
         {    770,   2994   },         /* Gain = 3.007812 Exposure Index = 295 */
         {    794,   2994   },         /* Gain = 3.101562 Exposure Index = 296 */
         {    818,   2994   },         /* Gain = 3.195312 Exposure Index = 297 */
         {    843,   2994   },         /* Gain = 3.292969 Exposure Index = 298 */
         {    869,   2994   },         /* Gain = 3.394531 Exposure Index = 299 */
         {    896,   2994   },         /* Gain = 3.500000 Exposure Index = 300 */
         {    923,   2994   },         /* Gain = 3.605469 Exposure Index = 301 */
         {    951,   2994   },         /* Gain = 3.714844 Exposure Index = 302 */
         {    980,   2994   },         /* Gain = 3.828125 Exposure Index = 303 */
         {   1010,   2994   },         /* Gain = 3.945312 Exposure Index = 304 */
         {   1041,   2994   },         /* Gain = 4.066406 Exposure Index = 305 */
         {   1073,   2994   },         /* Gain = 4.191406 Exposure Index = 306 */
         {   1106,   2994   },         /* Gain = 4.320312 Exposure Index = 307 */
         {   1140,   2994   },         /* Gain = 4.453125 Exposure Index = 308 */
         {   1175,   2994   },         /* Gain = 4.589844 Exposure Index = 309 */
         {   1211,   2994   },         /* Gain = 4.730469 Exposure Index = 310 */
         {   1248,   2994   },         /* Gain = 4.875000 Exposure Index = 311 */
         {   1286,   2994   },         /* Gain = 5.023438 Exposure Index = 312 */
         {   1325,   2994   },         /* Gain = 5.175781 Exposure Index = 313 */
         {   1365,   2994   },         /* Gain = 5.332031 Exposure Index = 314 */
         {   1406,   2994   },         /* Gain = 5.492188 Exposure Index = 315 */
         {   1449,   2994   },         /* Gain = 5.660156 Exposure Index = 316 */
         {   1493,   2994   },         /* Gain = 5.832031 Exposure Index = 317 */
         {   1538,   2994   },         /* Gain = 6.007812 Exposure Index = 318 */
         {   1585,   2994   },         /* Gain = 6.191406 Exposure Index = 319 */
         {   1633,   2994   },         /* Gain = 6.378906 Exposure Index = 320 */
         {   1682,   2994   },         /* Gain = 6.570312 Exposure Index = 321 */
         {   1733,   2994   },         /* Gain = 6.769531 Exposure Index = 322 */
         {   1785,   2994   },         /* Gain = 6.972656 Exposure Index = 323 */
         {   1839,   2994   },         /* Gain = 7.183594 Exposure Index = 324 */
         {   1895,   2994   },         /* Gain = 7.402344 Exposure Index = 325 */
         {   1952,   2994   },         /* Gain = 7.625000 Exposure Index = 326 */
         {   2011,   2994   },         /* Gain = 7.855469 Exposure Index = 327 */
         {   2072,   2994   },         /* Gain = 8.093750 Exposure Index = 328 */
         {   2135,   2994   },         /* Gain = 8.339844 Exposure Index = 329 */
         {   2200,   2994   },         /* Gain = 8.593750 Exposure Index = 330 */
         {   2266,   2994   },         /* Gain = 8.851562 Exposure Index = 331 */
         {   2334,   2994   },         /* Gain = 9.117188 Exposure Index = 332 */
         {   2405,   2994   },         /* Gain = 9.394531 Exposure Index = 333 */
         {   2478,   2994   },         /* Gain = 9.679688 Exposure Index = 334 */
         {   2553,   2994   },         /* Gain = 9.972656 Exposure Index = 335 */
         {   2630,   2994   },         /* Gain = 10.273438 Exposure Index = 336 */
         {   2709,   2994   },         /* Gain = 10.582031 Exposure Index = 337 */
         {   2791,   2994   },         /* Gain = 10.902344 Exposure Index = 338 */
         {   2875,   2994   },         /* Gain = 11.230469 Exposure Index = 339 */
         {   2962,   2994   },         /* Gain = 11.570312 Exposure Index = 340 */
         {   3051,   2994   },         /* Gain = 11.917969 Exposure Index = 341 */
         {   3143,   2994   },         /* Gain = 12.277344 Exposure Index = 342 */
         {   3238,   2994   },         /* Gain = 12.648438 Exposure Index = 343 */
         {   3336,   2994   },         /* Gain = 13.031250 Exposure Index = 344 */
         {   3437,   2994   },         /* Gain = 13.425781 Exposure Index = 345 */
         {   3541,   2994   },         /* Gain = 13.832031 Exposure Index = 346 */
         {   3648,   2994   },         /* Gain = 14.250000 Exposure Index = 347 */
         {   3758,   2994   },         /* Gain = 14.679688 Exposure Index = 348 */
         {   3871,   2994   },         /* Gain = 15.121094 Exposure Index = 349 */
         {   3988,   2994   },         /* Gain = 15.578125 Exposure Index = 350 */
         {   4108,   2994   },         /* Gain = 16.046875 Exposure Index = 351 */
         {   4232,   2994   },         /* Gain = 16.531250 Exposure Index = 352 */
         {   4359,   2994   },         /* Gain = 17.027344 Exposure Index = 353 */
         {   4490,   2994   },         /* Gain = 17.539062 Exposure Index = 354 */
         {   4625,   2994   },         /* Gain = 18.066406 Exposure Index = 355 */
         {   4764,   2994   },         /* Gain = 18.609375 Exposure Index = 356 */
         {   4907,   2994   },         /* Gain = 19.167969 Exposure Index = 357 */
         {   5055,   2994   },         /* Gain = 19.746094 Exposure Index = 358 */
         {   5207,   2994   },         /* Gain = 20.339844 Exposure Index = 359 */
         {   5364,   2994   },         /* Gain = 20.953125 Exposure Index = 360 */
         {   5525,   2994   },         /* Gain = 21.582031 Exposure Index = 361 */
         {   5691,   2994   },         /* Gain = 22.230469 Exposure Index = 362 */
         {   5862,   2994   },         /* Gain = 22.898438 Exposure Index = 363 */
         {   6038,   2994   },         /* Gain = 23.585938 Exposure Index = 364 */
         {   6220,   2994   },         /* Gain = 24.296875 Exposure Index = 365 */
         {   6407,   2994   },         /* Gain = 25.027344 Exposure Index = 366 */
         {   6600,   2994   },         /* Gain = 25.781250 Exposure Index = 367 */
         {   6798,   2994   },         /* Gain = 26.554688 Exposure Index = 368 */
         {   7002,   2994   },         /* Gain = 27.351562 Exposure Index = 369 */
         {   7213,   2994   },         /* Gain = 28.175781 Exposure Index = 370 */
         {   7430,   2994   },         /* Gain = 29.023438 Exposure Index = 371 */
         {   7653,   2994   },         /* Gain = 29.894531 Exposure Index = 372 */
         {   7883,   2994   },         /* Gain = 30.792969 Exposure Index = 373 */
         {   8120,   2994   },         /* Gain = 31.718750 Exposure Index = 374 */
         {   8364,   2994   },         /* Gain = 32.671875 Exposure Index = 375 */
         {   8615,   2994   },         /* Gain = 33.652344 Exposure Index = 376 */
         {   8874,   2994   },         /* Gain = 34.664062 Exposure Index = 377 */
         {   9141,   2994   },         /* Gain = 35.707031 Exposure Index = 378 */
         {   9416,   2994   },         /* Gain = 36.781250 Exposure Index = 379 */
         {   9699,   2994   },         /* Gain = 37.886719 Exposure Index = 380 */
         {   9990,   2994   },         /* Gain = 39.023438 Exposure Index = 381 */
         {  10290,   2994   },         /* Gain = 40.195312 Exposure Index = 382 */
         {  10599,   2994   },         /* Gain = 41.402344 Exposure Index = 383 */
         {  10917,   2994   },         /* Gain = 42.644531 Exposure Index = 384 */
         {  11245,   2994   },         /* Gain = 43.925781 Exposure Index = 385 */
         {  11583,   2994   },         /* Gain = 45.246094 Exposure Index = 386 */
         {  11931,   2994   },         /* Gain = 46.605469 Exposure Index = 387 */
         {  12289,   2994   },         /* Gain = 48.003906 Exposure Index = 388 */
         {  12658,   2994   },         /* Gain = 49.445312 Exposure Index = 389 */
         {  13038,   2994   },         /* Gain = 50.929688 Exposure Index = 390 */
         {  13430,   2994   },         /* Gain = 52.460938 Exposure Index = 391 */
         {  13833,   2994   },         /* Gain = 54.035156 Exposure Index = 392 */
         {  14248,   2994   },         /* Gain = 55.656250 Exposure Index = 393 */
         {  14676,   2994   },         /* Gain = 57.328125 Exposure Index = 394 */
         {  15117,   2994   },         /* Gain = 59.050781 Exposure Index = 395 */
         {  15571,   2994   },         /* Gain = 60.824219 Exposure Index = 396 */
         {  16039,   2994   },         /* Gain = 62.652344 Exposure Index = 397 */
         {  16521,   2994   },         /* Gain = 64.535156 Exposure Index = 398 */
         {  17017,   2994   },         /* Gain = 66.472656 Exposure Index = 399 */
         {  17528,   2994   },         /* Gain = 68.468750 Exposure Index = 400 */
         {  18054,   2994   },         /* Gain = 70.523438 Exposure Index = 401 */
         {  18596,   2994   },         /* Gain = 72.640625 Exposure Index = 402 */
         {  19154,   2994   },         /* Gain = 74.820312 Exposure Index = 403 */
         {  19729,   2994   },         /* Gain = 77.066406 Exposure Index = 404 */
         {  20321,   2994   },         /* Gain = 79.378906 Exposure Index = 405 */
         {  20931,   2994   },         /* Gain = 81.761719 Exposure Index = 406 */
         {  21559,   2994   },         /* Gain = 84.214844 Exposure Index = 407 */
         {  22206,   2994   },         /* Gain = 86.742188 Exposure Index = 408 */
         {  22873,   2994   },         /* Gain = 89.347656 Exposure Index = 409 */
         {  23560,   2994   },         /* Gain = 92.031250 Exposure Index = 410 */
         {  24267,   2994   },         /* Gain = 94.792969 Exposure Index = 411 */
         {  24996,   2994   },         /* Gain = 97.640625 Exposure Index = 412 */
         {  25746,   2994   },         /* Gain = 100.570312 Exposure Index = 413 */
         {  26519,   2994   },         /* Gain = 103.589844 Exposure Index = 414 */
         {  27315,   2994   },         /* Gain = 106.699219 Exposure Index = 415 */
         {  28135,   2994   },         /* Gain = 109.902344 Exposure Index = 416 */
         {  28980,   2994   },         /* Gain = 113.203125 Exposure Index = 417 */
         {  29850,   2994   },         /* Gain = 116.601562 Exposure Index = 418 */
         {  30746,   2994   },         /* Gain = 120.101562 Exposure Index = 419 */
         {  31669,   2994   },         /* Gain = 123.707031 Exposure Index = 420 */
         {  32620,   2994   },         /* Gain = 127.421875 Exposure Index = 421 */
         {  33599,   2994   },         /* Gain = 131.246094 Exposure Index = 422 */
         {  34607,   2994   },         /* Gain = 135.183594 Exposure Index = 423 */
         {  35646,   2994   },         /* Gain = 139.242188 Exposure Index = 424 */
         {  36716,   2994   },         /* Gain = 143.421875 Exposure Index = 425 */
         {  37818,   2994   },         /* Gain = 147.726562 Exposure Index = 426 */
         {  38953,   2994   },         /* Gain = 152.160156 Exposure Index = 427 */
         {  40122,   2994   },         /* Gain = 156.726562 Exposure Index = 428 */
         {  41326,   2994   },         /* Gain = 161.429688 Exposure Index = 429 */
         {  42566,   2994   },         /* Gain = 166.273438 Exposure Index = 430 */
         {  43843,   2994   },         /* Gain = 171.261719 Exposure Index = 431 */
         {  45159,   2994   },         /* Gain = 176.402344 Exposure Index = 432 */
         {  46514,   2994   },         /* Gain = 181.695312 Exposure Index = 433 */
         {  47910,   2994   },         /* Gain = 187.148438 Exposure Index = 434 */
         {  49348,   2994   },         /* Gain = 192.765625 Exposure Index = 435 */
         {  50829,   2994   },         /* Gain = 198.550781 Exposure Index = 436 */
         {  52354,   2994   },         /* Gain = 204.507812 Exposure Index = 437 */
         {  53925,   2994   },         /* Gain = 210.644531 Exposure Index = 438 */
         {  55543,   2994   },         /* Gain = 216.964844 Exposure Index = 439 */
         {  57210,   2994   },         /* Gain = 223.476562 Exposure Index = 440 */
         {  58927,   2994   },         /* Gain = 230.183594 Exposure Index = 441 */
         {  60695,   2994   },         /* Gain = 237.089844 Exposure Index = 442 */
         {  62516,   2994   },         /* Gain = 244.203125 Exposure Index = 443 */
         {  64392,   2994   },         /* Gain = 251.531250 Exposure Index = 444 */
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
            {90,47},
            {150,47},
            {200,42},
            {280,42},
            {300,32},
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
      { 0.4031, 0.7004 },   /* d75 */
      { 0.4313, 0.6440 },   /* d65 */
      { 0.5162, 0.5763 },   /* d50 */
      { 0.5162, 0.5763 },   /* noon */
      { 0.4830, 0.4250 },   /* cw */
      { 0.5626, 0.4277 },   /* tl84 */
      { 0.7721, 0.3760 },   /* a */
      { 0.9472, 0.3212 },   /* h */
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
         /* awb_mesh_w[16] */
         {
            0,   /* lux_index */
            /* awb_mesh_w[36][64] */
            {
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            },
            0,   /* lux_index */
            /* awb_mesh_w[36][64] */
            {
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            },
            0,   /* lux_index */
            /* awb_mesh_w[36][64] */
            {
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            },
            0,   /* lux_index */
            /* awb_mesh_w[36][64] */
            {
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            },
            0,   /* lux_index */
            /* awb_mesh_w[36][64] */
            {
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
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
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
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
            { 1.1000, 0.9700 },   /* d50 */
            { 1.0000, 1.0000 },   /* noon */
            { 1.0000, 1.0000 },   /* cw */
            { 1.0000, 1.0000 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 0.0000, 0.0000 },   /* custom1 */
            { 0.0000, 0.0000 },   /* custom2 */
         },
         400,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0000 },   /* d75 */
            { 1.0000, 1.0000 },   /* d65 */
            { 1.1000, 0.9700 },   /* d50 */
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
      1,//disable_mv_to_max_pos
   },
   /* af_stats_type 1-AF_STATS_SHARPNES, 2-AF_STATS_FIR, 4-AF_STATS_IIR */
   4,
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
         -1,//enable
         30,//gain_min
         2,//gain_max
         30,//ref_gain_min
         0,//ref_gain_max
         2,//threshold_min
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
      -1,
      /* index[50] */
      {
         492, 412, 300, 100, 212, 262, 300, 100, 100, 512, 
         100, 512, 100, 512, 300, 200, 100, 50, 0, 0, 
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
      },
      -1,//is_hys_comp_needed
      /* single_threshold_t hw */
      {
         99,//hist_dec_dec_thres
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
         99,//hist_inc_dec_thres
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
         0,//trigger_val not use
         8,//coef_y_b
         8,//coef_y_gb
         8,//coef_y_gr
         2,//coef_y_r
         1,//fv_mode 0-fv_max 1-fv_acc 2-sqr
         0,///ch_sel 0-R 1-GR 2-GB 3-B 4-Y
         0,//zoom_en
         0,//zoom_dwn
         1,//compander_en
         99,//lowfilter_en
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
         99,//fir_sqr_bitshift
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
         99,//iir_th
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
         99,//iir_th
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
