
/*---------------header of imx307 tuning-------------------------*/

/* version of the tuning header uint32_t header_version */
0x20220126,
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
               64,
           },
           /* aec trigger 1*/
           {
               124,
               128,
           },
           /* aec trigger 2*/
           {
               280,
               400,
           },
           /* aec trigger 3*/
           {
               580,
               600,
           },
           /* aec trigger 4*/
           {
               930,
               1000,
           },
       },
       /* isp_sub_module_blc_para para[MIDDLE_TRIGGER_COUNT] MIDDLE_TRIGGER_COUNT==5 */
       {
           /* para 0*/
           {
               800,//R
               806,//B
               810,//GR
               805,//GB
               270,//gian R
               270,//gain B
               265,//gain GR
               265,//gain GB
           },
           /* para 1*/
           {
               900,//R
               810,//B
               826,//GR
               814,//GB
               280,//gian R
               280,//gain B
               270,//gain GR
               270,//gain GB
           },
           /* para 2*/
           {
               1100,//R
               1040,//B
               877,//GR
               856,//GB
               340,//gian R
               340,//gain B
               270,//gain GR
               270,//gain GB
           },
           /* para 3*/
           {
               1618,//R
               1640,//B
               1089,//GR
               1057,//GB
               350,//gian R
               350,//gain B
               275,//gain GR
               274,//gain GB
           },
           /* para 4*/
           {
               2556,//R
               2432,//B
               1607,//GR
               1548,//GB
               500,//gian R
               500,//gain B
               270,//gain GR
               270,//gain GB
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
             800,
             1000.1,
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
               2,//denoise_layer
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
                       {25,25,25},
                       /* channel 2 */
                       {25,25,25},
                       /* channel 3 */
                       {25,25,25},
                       /* channel 4 */
                       {25,25,25},
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
                       {600,600,600},
                       /* channel 2 */
                       {600,600,600},
                       /* channel 3 */
                       {600,600,600},
                       /* channel 4 */
                       {600,600,600},
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
                       {600,600,600},
                       /* channel 2 */
                       {600,600,600},
                       /* channel 3 */
                       {600,600,600},
                       /* channel 4 */
                       {600,600,600},
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
             260,
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
                0.8,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.0553, 1.6458, 1.3735, 1.2596, 1.1651, 1.1961, 1.2795, 1.4219, 1.6984, 2.1803, 
                   1.9459, 1.5470, 1.3214, 1.1846, 1.1376, 1.1297, 1.1957, 1.3459, 1.6096, 2.0070, 
                   1.7922, 1.4191, 1.2170, 1.1005, 1.0673, 1.0691, 1.1044, 1.2463, 1.4739, 1.8703, 
                   1.7394, 1.4038, 1.1800, 1.1071, 1.0470, 1.0542, 1.1003, 1.2128, 1.4520, 1.8317, 
                   1.7214, 1.3628, 1.1659, 1.0853, 1.0163, 1.0145, 1.0721, 1.1757, 1.4141, 1.7568, 
                   1.6793, 1.3394, 1.1523, 1.0660, 1.0010, 1.0027, 1.0640, 1.1821, 1.3891, 1.7866, 
                   1.7339, 1.3998, 1.1828, 1.1015, 1.0457, 1.0448, 1.0868, 1.2006, 1.4556, 1.8136, 
                   1.7718, 1.3944, 1.2020, 1.0894, 1.0613, 1.0574, 1.0889, 1.2363, 1.4543, 1.8356, 
                   1.8806, 1.4971, 1.2808, 1.1454, 1.1106, 1.1102, 1.1501, 1.3313, 1.5473, 1.9956, 
                   2.0020, 1.6080, 1.3639, 1.2406, 1.1488, 1.1837, 1.2458, 1.4210, 1.6750, 2.0794, 
                   2.0867, 1.6483, 1.3962, 1.2709, 1.1755, 1.2064, 1.2499, 1.4447, 1.6378, 2.1860, 
                   1.9054, 1.5359, 1.3097, 1.1633, 1.1434, 1.1045, 1.2033, 1.3260, 1.5827, 1.9940, 
                   1.7926, 1.4242, 1.2114, 1.1114, 1.0596, 1.0838, 1.0882, 1.2512, 1.4392, 1.8565, 
                   1.7474, 1.4028, 1.1844, 1.1014, 1.0566, 1.0460, 1.0962, 1.2025, 1.4226, 1.8214, 
                   1.7051, 1.3461, 1.1648, 1.0809, 1.0117, 1.0214, 1.0659, 1.1799, 1.3847, 1.7680, 
                   1.7053, 1.3537, 1.1508, 1.0797, 0.9997, 0.9939, 1.0607, 1.1526, 1.3717, 1.7429, 
                   1.7251, 1.3763, 1.1787, 1.0932, 1.0429, 1.0511, 1.0837, 1.1935, 1.4245, 1.7700, 
                   1.7589, 1.4075, 1.1828, 1.0932, 1.0559, 1.0550, 1.0777, 1.2239, 1.4260, 1.8337, 
                   1.8999, 1.4904, 1.2898, 1.1466, 1.1077, 1.1135, 1.1548, 1.3013, 1.5432, 1.9221, 
                   1.9642, 1.6258, 1.3270, 1.2363, 1.1575, 1.1791, 1.2378, 1.3988, 1.6393, 2.0638, 
                   1.7787, 1.4947, 1.3174, 1.2241, 1.1688, 1.1920, 1.2420, 1.3473, 1.5793, 1.8569, 
                   1.7336, 1.4251, 1.2882, 1.1338, 1.1349, 1.1270, 1.1702, 1.2977, 1.5055, 1.7753, 
                   1.5716, 1.3423, 1.1574, 1.0911, 1.0594, 1.0690, 1.0913, 1.2033, 1.3937, 1.7081, 
                   1.6188, 1.3203, 1.1702, 1.0833, 1.0574, 1.0571, 1.0826, 1.1808, 1.3805, 1.6229, 
                   1.5143, 1.3104, 1.1346, 1.0672, 1.0247, 1.0211, 1.0765, 1.1556, 1.3585, 1.6430, 
                   1.5393, 1.2864, 1.1136, 1.0674, 1.0021, 0.9932, 1.0651, 1.1475, 1.3236, 1.6270, 
                   1.5443, 1.3470, 1.1516, 1.1063, 1.0415, 1.0690, 1.0660, 1.1892, 1.3697, 1.6796, 
                   1.5785, 1.3150, 1.1494, 1.0690, 1.0644, 1.0352, 1.0689, 1.1801, 1.3671, 1.6747, 
                   1.6688, 1.4112, 1.2296, 1.1366, 1.0859, 1.1062, 1.1410, 1.2836, 1.4505, 1.8322, 
                   1.7273, 1.4887, 1.2937, 1.2012, 1.1622, 1.1621, 1.2012, 1.3209, 1.5232, 1.8273, 
                },
             },
             /* awb para 1 */
             {
                0.8,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   1.9883, 1.5694, 1.3596, 1.2378, 1.1610, 1.1722, 1.2565, 1.3715, 1.6271, 2.0228, 
                   1.8168, 1.4686, 1.2796, 1.1481, 1.1129, 1.1045, 1.1520, 1.3054, 1.5011, 1.8868, 
                   1.7198, 1.3839, 1.2033, 1.1046, 1.0691, 1.0740, 1.1028, 1.2277, 1.4365, 1.7678, 
                   1.6761, 1.3504, 1.1680, 1.0878, 1.0408, 1.0343, 1.0804, 1.1916, 1.3788, 1.7444, 
                   1.5848, 1.3176, 1.1414, 1.0680, 1.0158, 1.0043, 1.0605, 1.1550, 1.3650, 1.6736, 
                   1.6358, 1.3185, 1.1485, 1.0751, 1.0033, 0.9969, 1.0649, 1.1595, 1.3503, 1.7060, 
                   1.6261, 1.3300, 1.1513, 1.0789, 1.0230, 1.0370, 1.0644, 1.1769, 1.3873, 1.6891, 
                   1.7038, 1.3683, 1.1880, 1.0906, 1.0655, 1.0565, 1.0936, 1.2226, 1.4061, 1.7916, 
                   1.7775, 1.4273, 1.2482, 1.1231, 1.0950, 1.0939, 1.1327, 1.2746, 1.4902, 1.8397, 
                   1.9342, 1.5114, 1.3397, 1.2054, 1.1447, 1.1450, 1.2133, 1.3708, 1.5779, 1.9968, 
                   2.0302, 1.6274, 1.3865, 1.2573, 1.1599, 1.2148, 1.2347, 1.4341, 1.6455, 2.1178, 
                   1.8763, 1.5122, 1.2826, 1.1536, 1.1234, 1.0987, 1.1760, 1.2913, 1.5511, 1.9276, 
                   1.7815, 1.4220, 1.2189, 1.1025, 1.0784, 1.0772, 1.1029, 1.2404, 1.4492, 1.8317, 
                   1.6898, 1.3877, 1.1564, 1.0910, 1.0380, 1.0426, 1.0807, 1.1849, 1.4074, 1.7830, 
                   1.6880, 1.3356, 1.1636, 1.0672, 1.0166, 1.0005, 1.0662, 1.1542, 1.3666, 1.7245, 
                   1.6619, 1.3527, 1.1387, 1.0723, 1.0031, 1.0014, 1.0642, 1.1515, 1.3746, 1.7064, 
                   1.7094, 1.3567, 1.1677, 1.0868, 1.0283, 1.0305, 1.0768, 1.1661, 1.3901, 1.7502, 
                   1.7599, 1.4080, 1.1909, 1.1009, 1.0652, 1.0651, 1.0976, 1.2095, 1.4402, 1.7924, 
                   1.8414, 1.4841, 1.2593, 1.1388, 1.1009, 1.1019, 1.1491, 1.2907, 1.5191, 1.9182, 
                   2.0068, 1.5729, 1.3531, 1.2205, 1.1474, 1.1587, 1.2390, 1.3647, 1.6254, 2.0639, 
                   1.8943, 1.5120, 1.3626, 1.2239, 1.1832, 1.2004, 1.2158, 1.3631, 1.5797, 1.8952, 
                   1.7307, 1.4190, 1.2508, 1.1335, 1.0924, 1.0961, 1.1549, 1.2878, 1.4682, 1.8714, 
                   1.6438, 1.3453, 1.1793, 1.0839, 1.0653, 1.0797, 1.0787, 1.2122, 1.4134, 1.7130, 
                   1.5857, 1.3215, 1.1433, 1.0839, 1.0477, 1.0369, 1.0843, 1.1854, 1.3526, 1.7040, 
                   1.5461, 1.2990, 1.1286, 1.0617, 0.9978, 1.0057, 1.0526, 1.1409, 1.3416, 1.6369, 
                   1.5655, 1.2783, 1.1235, 1.0610, 0.9992, 0.9973, 1.0623, 1.1422, 1.3350, 1.6407, 
                   1.5696, 1.3001, 1.1410, 1.0715, 1.0303, 1.0270, 1.0655, 1.1439, 1.3470, 1.6358, 
                   1.6524, 1.3147, 1.1692, 1.0706, 1.0521, 1.0498, 1.0770, 1.1928, 1.3908, 1.7047, 
                   1.6984, 1.3907, 1.2326, 1.1211, 1.0819, 1.0925, 1.1260, 1.2598, 1.4410, 1.8035, 
                   1.8673, 1.4784, 1.2888, 1.1947, 1.1301, 1.1513, 1.1738, 1.3155, 1.5399, 1.8400, 
                },
             },
             /* awb para 2 */
             {
                0.8,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   1.9689, 1.5499, 1.3399, 1.2116, 1.1441, 1.1424, 1.2486, 1.3787, 1.6414, 2.0457, 
                   1.9083, 1.5302, 1.3313, 1.1971, 1.1468, 1.1528, 1.2035, 1.3724, 1.5875, 2.0125, 
                   1.7261, 1.4147, 1.1969, 1.1154, 1.0695, 1.0820, 1.1109, 1.2484, 1.4719, 1.8167, 
                   1.6857, 1.3484, 1.1621, 1.0829, 1.0426, 1.0409, 1.0851, 1.1947, 1.4066, 1.7701, 
                   1.6751, 1.3903, 1.1790, 1.1122, 1.0457, 1.0374, 1.1015, 1.2100, 1.4308, 1.7770, 
                   1.5921, 1.2917, 1.1191, 1.0468, 0.9818, 0.9742, 1.0433, 1.1400, 1.3411, 1.6940, 
                   1.7270, 1.3964, 1.2001, 1.1082, 1.0729, 1.0626, 1.1200, 1.2309, 1.4567, 1.8396, 
                   1.6683, 1.3806, 1.1722, 1.0832, 1.0558, 1.0603, 1.0769, 1.2231, 1.4125, 1.7929, 
                   1.8214, 1.4250, 1.2633, 1.1098, 1.0967, 1.0805, 1.1471, 1.3093, 1.5015, 1.9529, 
                   1.9773, 1.6201, 1.3741, 1.2573, 1.1984, 1.2109, 1.2642, 1.4530, 1.6426, 2.1708, 
                   1.9577, 1.5904, 1.3237, 1.2313, 1.1326, 1.1567, 1.2249, 1.3605, 1.5893, 2.0429, 
                   1.9466, 1.5365, 1.3491, 1.1932, 1.1530, 1.1481, 1.2091, 1.3599, 1.5947, 2.0497, 
                   1.7682, 1.4370, 1.1998, 1.1119, 1.0688, 1.0723, 1.0987, 1.2317, 1.4477, 1.7955, 
                   1.7115, 1.3594, 1.1731, 1.0818, 1.0395, 1.0323, 1.0818, 1.1830, 1.3984, 1.7885, 
                   1.7162, 1.3996, 1.1815, 1.1111, 1.0391, 1.0440, 1.0933, 1.2041, 1.4215, 1.7778, 
                   1.6355, 1.3103, 1.1104, 1.0474, 0.9740, 0.9704, 1.0317, 1.1220, 1.3368, 1.6606, 
                   1.7456, 1.4131, 1.2017, 1.1206, 1.0675, 1.0642, 1.1152, 1.2210, 1.4452, 1.8191, 
                   1.7348, 1.3941, 1.1835, 1.0871, 1.0492, 1.0618, 1.0753, 1.2161, 1.4159, 1.7785, 
                   1.8568, 1.4645, 1.2522, 1.1332, 1.0935, 1.0844, 1.1365, 1.2801, 1.5034, 1.9094, 
                   2.0625, 1.6541, 1.4121, 1.2691, 1.1964, 1.2103, 1.2797, 1.4469, 1.6720, 2.1121, 
                   1.8666, 1.4764, 1.3121, 1.1594, 1.1370, 1.1134, 1.1960, 1.3106, 1.5314, 1.9638, 
                   1.8172, 1.5145, 1.3144, 1.1930, 1.1547, 1.1478, 1.1965, 1.3371, 1.5429, 1.8708, 
                   1.6636, 1.3607, 1.1807, 1.0936, 1.0620, 1.0713, 1.0919, 1.2198, 1.4069, 1.7706, 
                   1.6059, 1.3290, 1.1564, 1.0722, 1.0458, 1.0389, 1.0736, 1.1628, 1.3727, 1.6472, 
                   1.6325, 1.3683, 1.1688, 1.1073, 1.0487, 1.0458, 1.0916, 1.2045, 1.3834, 1.7360, 
                   1.5195, 1.2572, 1.0967, 1.0328, 0.9702, 0.9636, 1.0380, 1.1056, 1.3050, 1.5999, 
                   1.6623, 1.3722, 1.1946, 1.1047, 1.0767, 1.0727, 1.1153, 1.2107, 1.4208, 1.7398, 
                   1.6343, 1.3541, 1.1516, 1.0897, 1.0458, 1.0515, 1.0857, 1.1939, 1.3849, 1.7329, 
                   1.7055, 1.3909, 1.2377, 1.0919, 1.0845, 1.0785, 1.1217, 1.2575, 1.4518, 1.7902, 
                   1.9408, 1.5897, 1.3521, 1.2747, 1.1889, 1.1926, 1.3088, 1.3887, 1.6545, 2.0266, 
                },
             },
             /* awb para 3 */
             {
                0.8,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   1.9577, 1.5677, 1.3471, 1.2453, 1.1425, 1.2010, 1.2432, 1.3901, 1.6645, 1.9905, 
                   1.8688, 1.5163, 1.3112, 1.1803, 1.1491, 1.1269, 1.1992, 1.3548, 1.5563, 1.9804, 
                   1.6999, 1.3630, 1.2005, 1.0922, 1.0532, 1.0812, 1.0962, 1.2357, 1.4470, 1.8107, 
                   1.6827, 1.3831, 1.1840, 1.0987, 1.0619, 1.0544, 1.0942, 1.2187, 1.4128, 1.7651, 
                   1.6801, 1.3152, 1.1626, 1.0862, 1.0212, 1.0216, 1.0777, 1.1934, 1.3778, 1.7720, 
                   1.5869, 1.3138, 1.1425, 1.0565, 0.9993, 0.9945, 1.0602, 1.1536, 1.3601, 1.6856, 
                   1.7038, 1.3664, 1.1808, 1.1145, 1.0614, 1.0616, 1.1081, 1.2208, 1.4267, 1.7879, 
                   1.6498, 1.3611, 1.1693, 1.0688, 1.0577, 1.0421, 1.0827, 1.2092, 1.4138, 1.7602, 
                   1.8137, 1.4451, 1.2697, 1.1417, 1.1055, 1.1221, 1.1437, 1.3178, 1.5123, 1.9052, 
                   1.8297, 1.5688, 1.3392, 1.2216, 1.1690, 1.1617, 1.2581, 1.4079, 1.6185, 2.0927, 
                   2.0192, 1.5961, 1.3865, 1.2445, 1.1657, 1.1790, 1.2563, 1.4099, 1.6474, 2.1088, 
                   1.9120, 1.5502, 1.3117, 1.1839, 1.1505, 1.1254, 1.2054, 1.3258, 1.5818, 1.9606, 
                   1.7597, 1.4008, 1.2045, 1.0945, 1.0547, 1.0651, 1.0806, 1.2362, 1.4294, 1.8219, 
                   1.7266, 1.3991, 1.1885, 1.1021, 1.0646, 1.0525, 1.1045, 1.1935, 1.4364, 1.7745, 
                   1.6977, 1.3650, 1.1588, 1.0920, 1.0149, 1.0150, 1.0722, 1.1743, 1.3888, 1.7370, 
                   1.6445, 1.3431, 1.1376, 1.0644, 0.9951, 0.9930, 1.0566, 1.1455, 1.3652, 1.6950, 
                   1.7417, 1.4014, 1.1959, 1.1149, 1.0565, 1.0590, 1.1016, 1.2085, 1.4271, 1.8034, 
                   1.7408, 1.3845, 1.1818, 1.0811, 1.0486, 1.0455, 1.0785, 1.2009, 1.4138, 1.7643, 
                   1.8699, 1.5031, 1.2856, 1.1555, 1.1155, 1.1173, 1.1562, 1.3081, 1.5325, 1.9392, 
                   2.0691, 1.6050, 1.3847, 1.2352, 1.1786, 1.1632, 1.2495, 1.3964, 1.6314, 2.0858, 
                   1.8910, 1.5478, 1.3419, 1.2033, 1.1717, 1.1540, 1.2412, 1.3792, 1.5694, 2.0575, 
                   1.8544, 1.4891, 1.2963, 1.1865, 1.1207, 1.1446, 1.1738, 1.3267, 1.5411, 1.9060, 
                   1.6246, 1.3581, 1.1683, 1.0692, 1.0701, 1.0443, 1.0904, 1.2044, 1.3929, 1.7460, 
                   1.6759, 1.3589, 1.1772, 1.0971, 1.0538, 1.0653, 1.0827, 1.1990, 1.3984, 1.7439, 
                   1.6124, 1.3186, 1.1505, 1.0787, 1.0154, 1.0247, 1.0691, 1.1721, 1.3580, 1.7009, 
                   1.5666, 1.3055, 1.1194, 1.0520, 0.9951, 0.9833, 1.0506, 1.1278, 1.3280, 1.6496, 
                   1.6703, 1.3544, 1.1847, 1.1072, 1.0579, 1.0684, 1.0952, 1.2084, 1.3995, 1.7177, 
                   1.6294, 1.3361, 1.1590, 1.0653, 1.0406, 1.0497, 1.0659, 1.1846, 1.3691, 1.7220, 
                   1.7634, 1.4420, 1.2493, 1.1362, 1.1080, 1.0978, 1.1459, 1.2939, 1.4810, 1.8223, 
                   1.9390, 1.5012, 1.3644, 1.1919, 1.1676, 1.1640, 1.2439, 1.3555, 1.5864, 1.9619, 
                },
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                0.2,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   2.0553, 1.6458, 1.3735, 1.2596, 1.1651, 1.1961, 1.2795, 1.4219, 1.6984, 2.1803, 
                   1.9459, 1.5470, 1.3214, 1.1846, 1.1376, 1.1297, 1.1957, 1.3459, 1.6096, 2.0070, 
                   1.7922, 1.4191, 1.2170, 1.1005, 1.0673, 1.0691, 1.1044, 1.2463, 1.4739, 1.8703, 
                   1.7394, 1.4038, 1.1800, 1.1071, 1.0470, 1.0542, 1.1003, 1.2128, 1.4520, 1.8317, 
                   1.7214, 1.3628, 1.1659, 1.0853, 1.0163, 1.0145, 1.0721, 1.1757, 1.4141, 1.7568, 
                   1.6793, 1.3394, 1.1523, 1.0660, 1.0010, 1.0027, 1.0640, 1.1821, 1.3891, 1.7866, 
                   1.7339, 1.3998, 1.1828, 1.1015, 1.0457, 1.0448, 1.0868, 1.2006, 1.4556, 1.8136, 
                   1.7718, 1.3944, 1.2020, 1.0894, 1.0613, 1.0574, 1.0889, 1.2363, 1.4543, 1.8356, 
                   1.8806, 1.4971, 1.2808, 1.1454, 1.1106, 1.1102, 1.1501, 1.3313, 1.5473, 1.9956, 
                   2.0020, 1.6080, 1.3639, 1.2406, 1.1488, 1.1837, 1.2458, 1.4210, 1.6750, 2.0794, 
                   2.0867, 1.6483, 1.3962, 1.2709, 1.1755, 1.2064, 1.2499, 1.4447, 1.6378, 2.1860, 
                   1.9054, 1.5359, 1.3097, 1.1633, 1.1434, 1.1045, 1.2033, 1.3260, 1.5827, 1.9940, 
                   1.7926, 1.4242, 1.2114, 1.1114, 1.0596, 1.0838, 1.0882, 1.2512, 1.4392, 1.8565, 
                   1.7474, 1.4028, 1.1844, 1.1014, 1.0566, 1.0460, 1.0962, 1.2025, 1.4226, 1.8214, 
                   1.7051, 1.3461, 1.1648, 1.0809, 1.0117, 1.0214, 1.0659, 1.1799, 1.3847, 1.7680, 
                   1.7053, 1.3537, 1.1508, 1.0797, 0.9997, 0.9939, 1.0607, 1.1526, 1.3717, 1.7429, 
                   1.7251, 1.3763, 1.1787, 1.0932, 1.0429, 1.0511, 1.0837, 1.1935, 1.4245, 1.7700, 
                   1.7589, 1.4075, 1.1828, 1.0932, 1.0559, 1.0550, 1.0777, 1.2239, 1.4260, 1.8337, 
                   1.8999, 1.4904, 1.2898, 1.1466, 1.1077, 1.1135, 1.1548, 1.3013, 1.5432, 1.9221, 
                   1.9642, 1.6258, 1.3270, 1.2363, 1.1575, 1.1791, 1.2378, 1.3988, 1.6393, 2.0638, 
                   1.7787, 1.4947, 1.3174, 1.2241, 1.1688, 1.1920, 1.2420, 1.3473, 1.5793, 1.8569, 
                   1.7336, 1.4251, 1.2882, 1.1338, 1.1349, 1.1270, 1.1702, 1.2977, 1.5055, 1.7753, 
                   1.5716, 1.3423, 1.1574, 1.0911, 1.0594, 1.0690, 1.0913, 1.2033, 1.3937, 1.7081, 
                   1.6188, 1.3203, 1.1702, 1.0833, 1.0574, 1.0571, 1.0826, 1.1808, 1.3805, 1.6229, 
                   1.5143, 1.3104, 1.1346, 1.0672, 1.0247, 1.0211, 1.0765, 1.1556, 1.3585, 1.6430, 
                   1.5393, 1.2864, 1.1136, 1.0674, 1.0021, 0.9932, 1.0651, 1.1475, 1.3236, 1.6270, 
                   1.5443, 1.3470, 1.1516, 1.1063, 1.0415, 1.0690, 1.0660, 1.1892, 1.3697, 1.6796, 
                   1.5785, 1.3150, 1.1494, 1.0690, 1.0644, 1.0352, 1.0689, 1.1801, 1.3671, 1.6747, 
                   1.6688, 1.4112, 1.2296, 1.1366, 1.0859, 1.1062, 1.1410, 1.2836, 1.4505, 1.8322, 
                   1.7273, 1.4887, 1.2937, 1.2012, 1.1622, 1.1621, 1.2012, 1.3209, 1.5232, 1.8273, 
                },
             },
             /* awb para 1 */
             {
                0.2,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   1.9883, 1.5694, 1.3596, 1.2378, 1.1610, 1.1722, 1.2565, 1.3715, 1.6271, 2.0228, 
                   1.8168, 1.4686, 1.2796, 1.1481, 1.1129, 1.1045, 1.1520, 1.3054, 1.5011, 1.8868, 
                   1.7198, 1.3839, 1.2033, 1.1046, 1.0691, 1.0740, 1.1028, 1.2277, 1.4365, 1.7678, 
                   1.6761, 1.3504, 1.1680, 1.0878, 1.0408, 1.0343, 1.0804, 1.1916, 1.3788, 1.7444, 
                   1.5848, 1.3176, 1.1414, 1.0680, 1.0158, 1.0043, 1.0605, 1.1550, 1.3650, 1.6736, 
                   1.6358, 1.3185, 1.1485, 1.0751, 1.0033, 0.9969, 1.0649, 1.1595, 1.3503, 1.7060, 
                   1.6261, 1.3300, 1.1513, 1.0789, 1.0230, 1.0370, 1.0644, 1.1769, 1.3873, 1.6891, 
                   1.7038, 1.3683, 1.1880, 1.0906, 1.0655, 1.0565, 1.0936, 1.2226, 1.4061, 1.7916, 
                   1.7775, 1.4273, 1.2482, 1.1231, 1.0950, 1.0939, 1.1327, 1.2746, 1.4902, 1.8397, 
                   1.9342, 1.5114, 1.3397, 1.2054, 1.1447, 1.1450, 1.2133, 1.3708, 1.5779, 1.9968, 
                   2.0302, 1.6274, 1.3865, 1.2573, 1.1599, 1.2148, 1.2347, 1.4341, 1.6455, 2.1178, 
                   1.8763, 1.5122, 1.2826, 1.1536, 1.1234, 1.0987, 1.1760, 1.2913, 1.5511, 1.9276, 
                   1.7815, 1.4220, 1.2189, 1.1025, 1.0784, 1.0772, 1.1029, 1.2404, 1.4492, 1.8317, 
                   1.6898, 1.3877, 1.1564, 1.0910, 1.0380, 1.0426, 1.0807, 1.1849, 1.4074, 1.7830, 
                   1.6880, 1.3356, 1.1636, 1.0672, 1.0166, 1.0005, 1.0662, 1.1542, 1.3666, 1.7245, 
                   1.6619, 1.3527, 1.1387, 1.0723, 1.0031, 1.0014, 1.0642, 1.1515, 1.3746, 1.7064, 
                   1.7094, 1.3567, 1.1677, 1.0868, 1.0283, 1.0305, 1.0768, 1.1661, 1.3901, 1.7502, 
                   1.7599, 1.4080, 1.1909, 1.1009, 1.0652, 1.0651, 1.0976, 1.2095, 1.4402, 1.7924, 
                   1.8414, 1.4841, 1.2593, 1.1388, 1.1009, 1.1019, 1.1491, 1.2907, 1.5191, 1.9182, 
                   2.0068, 1.5729, 1.3531, 1.2205, 1.1474, 1.1587, 1.2390, 1.3647, 1.6254, 2.0639, 
                   1.8943, 1.5120, 1.3626, 1.2239, 1.1832, 1.2004, 1.2158, 1.3631, 1.5797, 1.8952, 
                   1.7307, 1.4190, 1.2508, 1.1335, 1.0924, 1.0961, 1.1549, 1.2878, 1.4682, 1.8714, 
                   1.6438, 1.3453, 1.1793, 1.0839, 1.0653, 1.0797, 1.0787, 1.2122, 1.4134, 1.7130, 
                   1.5857, 1.3215, 1.1433, 1.0839, 1.0477, 1.0369, 1.0843, 1.1854, 1.3526, 1.7040, 
                   1.5461, 1.2990, 1.1286, 1.0617, 0.9978, 1.0057, 1.0526, 1.1409, 1.3416, 1.6369, 
                   1.5655, 1.2783, 1.1235, 1.0610, 0.9992, 0.9973, 1.0623, 1.1422, 1.3350, 1.6407, 
                   1.5696, 1.3001, 1.1410, 1.0715, 1.0303, 1.0270, 1.0655, 1.1439, 1.3470, 1.6358, 
                   1.6524, 1.3147, 1.1692, 1.0706, 1.0521, 1.0498, 1.0770, 1.1928, 1.3908, 1.7047, 
                   1.6984, 1.3907, 1.2326, 1.1211, 1.0819, 1.0925, 1.1260, 1.2598, 1.4410, 1.8035, 
                   1.8673, 1.4784, 1.2888, 1.1947, 1.1301, 1.1513, 1.1738, 1.3155, 1.5399, 1.8400, 
                },
             },
             /* awb para 2 */
             {
                0.2,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   1.9689, 1.5499, 1.3399, 1.2116, 1.1441, 1.1424, 1.2486, 1.3787, 1.6414, 2.0457, 
                   1.9083, 1.5302, 1.3313, 1.1971, 1.1468, 1.1528, 1.2035, 1.3724, 1.5875, 2.0125, 
                   1.7261, 1.4147, 1.1969, 1.1154, 1.0695, 1.0820, 1.1109, 1.2484, 1.4719, 1.8167, 
                   1.6857, 1.3484, 1.1621, 1.0829, 1.0426, 1.0409, 1.0851, 1.1947, 1.4066, 1.7701, 
                   1.6751, 1.3903, 1.1790, 1.1122, 1.0457, 1.0374, 1.1015, 1.2100, 1.4308, 1.7770, 
                   1.5921, 1.2917, 1.1191, 1.0468, 0.9818, 0.9742, 1.0433, 1.1400, 1.3411, 1.6940, 
                   1.7270, 1.3964, 1.2001, 1.1082, 1.0729, 1.0626, 1.1200, 1.2309, 1.4567, 1.8396, 
                   1.6683, 1.3806, 1.1722, 1.0832, 1.0558, 1.0603, 1.0769, 1.2231, 1.4125, 1.7929, 
                   1.8214, 1.4250, 1.2633, 1.1098, 1.0967, 1.0805, 1.1471, 1.3093, 1.5015, 1.9529, 
                   1.9773, 1.6201, 1.3741, 1.2573, 1.1984, 1.2109, 1.2642, 1.4530, 1.6426, 2.1708, 
                   1.9577, 1.5904, 1.3237, 1.2313, 1.1326, 1.1567, 1.2249, 1.3605, 1.5893, 2.0429, 
                   1.9466, 1.5365, 1.3491, 1.1932, 1.1530, 1.1481, 1.2091, 1.3599, 1.5947, 2.0497, 
                   1.7682, 1.4370, 1.1998, 1.1119, 1.0688, 1.0723, 1.0987, 1.2317, 1.4477, 1.7955, 
                   1.7115, 1.3594, 1.1731, 1.0818, 1.0395, 1.0323, 1.0818, 1.1830, 1.3984, 1.7885, 
                   1.7162, 1.3996, 1.1815, 1.1111, 1.0391, 1.0440, 1.0933, 1.2041, 1.4215, 1.7778, 
                   1.6355, 1.3103, 1.1104, 1.0474, 0.9740, 0.9704, 1.0317, 1.1220, 1.3368, 1.6606, 
                   1.7456, 1.4131, 1.2017, 1.1206, 1.0675, 1.0642, 1.1152, 1.2210, 1.4452, 1.8191, 
                   1.7348, 1.3941, 1.1835, 1.0871, 1.0492, 1.0618, 1.0753, 1.2161, 1.4159, 1.7785, 
                   1.8568, 1.4645, 1.2522, 1.1332, 1.0935, 1.0844, 1.1365, 1.2801, 1.5034, 1.9094, 
                   2.0625, 1.6541, 1.4121, 1.2691, 1.1964, 1.2103, 1.2797, 1.4469, 1.6720, 2.1121, 
                   1.8666, 1.4764, 1.3121, 1.1594, 1.1370, 1.1134, 1.1960, 1.3106, 1.5314, 1.9638, 
                   1.8172, 1.5145, 1.3144, 1.1930, 1.1547, 1.1478, 1.1965, 1.3371, 1.5429, 1.8708, 
                   1.6636, 1.3607, 1.1807, 1.0936, 1.0620, 1.0713, 1.0919, 1.2198, 1.4069, 1.7706, 
                   1.6059, 1.3290, 1.1564, 1.0722, 1.0458, 1.0389, 1.0736, 1.1628, 1.3727, 1.6472, 
                   1.6325, 1.3683, 1.1688, 1.1073, 1.0487, 1.0458, 1.0916, 1.2045, 1.3834, 1.7360, 
                   1.5195, 1.2572, 1.0967, 1.0328, 0.9702, 0.9636, 1.0380, 1.1056, 1.3050, 1.5999, 
                   1.6623, 1.3722, 1.1946, 1.1047, 1.0767, 1.0727, 1.1153, 1.2107, 1.4208, 1.7398, 
                   1.6343, 1.3541, 1.1516, 1.0897, 1.0458, 1.0515, 1.0857, 1.1939, 1.3849, 1.7329, 
                   1.7055, 1.3909, 1.2377, 1.0919, 1.0845, 1.0785, 1.1217, 1.2575, 1.4518, 1.7902, 
                   1.9408, 1.5897, 1.3521, 1.2747, 1.1889, 1.1926, 1.3088, 1.3887, 1.6545, 2.0266, 
                },
             },
             /* awb para 3 */
             {
                0.2,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   1.9577, 1.5677, 1.3471, 1.2453, 1.1425, 1.2010, 1.2432, 1.3901, 1.6645, 1.9905, 
                   1.8688, 1.5163, 1.3112, 1.1803, 1.1491, 1.1269, 1.1992, 1.3548, 1.5563, 1.9804, 
                   1.6999, 1.3630, 1.2005, 1.0922, 1.0532, 1.0812, 1.0962, 1.2357, 1.4470, 1.8107, 
                   1.6827, 1.3831, 1.1840, 1.0987, 1.0619, 1.0544, 1.0942, 1.2187, 1.4128, 1.7651, 
                   1.6801, 1.3152, 1.1626, 1.0862, 1.0212, 1.0216, 1.0777, 1.1934, 1.3778, 1.7720, 
                   1.5869, 1.3138, 1.1425, 1.0565, 0.9993, 0.9945, 1.0602, 1.1536, 1.3601, 1.6856, 
                   1.7038, 1.3664, 1.1808, 1.1145, 1.0614, 1.0616, 1.1081, 1.2208, 1.4267, 1.7879, 
                   1.6498, 1.3611, 1.1693, 1.0688, 1.0577, 1.0421, 1.0827, 1.2092, 1.4138, 1.7602, 
                   1.8137, 1.4451, 1.2697, 1.1417, 1.1055, 1.1221, 1.1437, 1.3178, 1.5123, 1.9052, 
                   1.8297, 1.5688, 1.3392, 1.2216, 1.1690, 1.1617, 1.2581, 1.4079, 1.6185, 2.0927, 
                   2.0192, 1.5961, 1.3865, 1.2445, 1.1657, 1.1790, 1.2563, 1.4099, 1.6474, 2.1088, 
                   1.9120, 1.5502, 1.3117, 1.1839, 1.1505, 1.1254, 1.2054, 1.3258, 1.5818, 1.9606, 
                   1.7597, 1.4008, 1.2045, 1.0945, 1.0547, 1.0651, 1.0806, 1.2362, 1.4294, 1.8219, 
                   1.7266, 1.3991, 1.1885, 1.1021, 1.0646, 1.0525, 1.1045, 1.1935, 1.4364, 1.7745, 
                   1.6977, 1.3650, 1.1588, 1.0920, 1.0149, 1.0150, 1.0722, 1.1743, 1.3888, 1.7370, 
                   1.6445, 1.3431, 1.1376, 1.0644, 0.9951, 0.9930, 1.0566, 1.1455, 1.3652, 1.6950, 
                   1.7417, 1.4014, 1.1959, 1.1149, 1.0565, 1.0590, 1.1016, 1.2085, 1.4271, 1.8034, 
                   1.7408, 1.3845, 1.1818, 1.0811, 1.0486, 1.0455, 1.0785, 1.2009, 1.4138, 1.7643, 
                   1.8699, 1.5031, 1.2856, 1.1555, 1.1155, 1.1173, 1.1562, 1.3081, 1.5325, 1.9392, 
                   2.0691, 1.6050, 1.3847, 1.2352, 1.1786, 1.1632, 1.2495, 1.3964, 1.6314, 2.0858, 
                   1.8910, 1.5478, 1.3419, 1.2033, 1.1717, 1.1540, 1.2412, 1.3792, 1.5694, 2.0575, 
                   1.8544, 1.4891, 1.2963, 1.1865, 1.1207, 1.1446, 1.1738, 1.3267, 1.5411, 1.9060, 
                   1.6246, 1.3581, 1.1683, 1.0692, 1.0701, 1.0443, 1.0904, 1.2044, 1.3929, 1.7460, 
                   1.6759, 1.3589, 1.1772, 1.0971, 1.0538, 1.0653, 1.0827, 1.1990, 1.3984, 1.7439, 
                   1.6124, 1.3186, 1.1505, 1.0787, 1.0154, 1.0247, 1.0691, 1.1721, 1.3580, 1.7009, 
                   1.5666, 1.3055, 1.1194, 1.0520, 0.9951, 0.9833, 1.0506, 1.1278, 1.3280, 1.6496, 
                   1.6703, 1.3544, 1.1847, 1.1072, 1.0579, 1.0684, 1.0952, 1.2084, 1.3995, 1.7177, 
                   1.6294, 1.3361, 1.1590, 1.0653, 1.0406, 1.0497, 1.0659, 1.1846, 1.3691, 1.7220, 
                   1.7634, 1.4420, 1.2493, 1.1362, 1.1080, 1.0978, 1.1459, 1.2939, 1.4810, 1.8223, 
                   1.9390, 1.5012, 1.3644, 1.1919, 1.1676, 1.1640, 1.2439, 1.3555, 1.5864, 1.9619, 
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
             130,
          },
          /* aec trigger 1 */
          {
             150,
             250,
          },
          /* aec trigger 2 */
          {
             280,
             350,
          },
          /* aec trigger 3 */
          {
             359,
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
                   8105,8105,8043,7988,7937,7888,7842,7798,7755,7713,7673,7634,7595,7557,7520,7484,7448,7413,7379,7345,7312,7279,7246,7214,7183,
                   7152,7121,7090,7060,7031,7001,6972,6943,6915,6887,6859,6831,6804,6777,6750,6723,6697,6670,6645,6619,6593,6568,6543,6518,6493,
                   6469,6444,6420,6396,6373,6349,6326,6302,6279,6256,6234,6211,6189,6166,6144,6122,6100,6079,6057,6036,6014,5993,5972,5951,5931,
                   5910,5890,5869,5849,5829,5809,5789,5769,5749,5730,5710,5691,5672,5653,5634,5615,5596,5577,5559,5540,5522,5504,5486,5468,5450,
                   5432,5414,5396,5379,5361,5344,5326,5309,5292,5275,5258,5241,5224,5207,5191,5174,5158,5141,5125,5109,5093,5076,5060,5044,5029,
                   5013,4997,4981,4966,4950,4935,4920,4904,4889,4874,4859,4844,4829,4814,4799,4785,4770,4755,4741,4726,4712,4698,4683,4669,4655,
                   4641,4627,4613,4599,4585,4571,4558,4544,4530,4517,4503,4490,4477,4463,4450,4437,4424,4411,4397,4384,4372,4359,4346,4333,4320,
                   4308,4295,4282,4270,4257,4245,4233,4220,4208,4196,4184,4171,4159,4147,4135,4123,4112,4100,4088,4076,4064,4053,4041,4030,4018,
                   4007,3995,3984,3972,3961,3950,3939,3927,3916,3905,3894,3883,3872,3861,3850,3840,3829,3818,3807,3797,3786,3775,3765,3754,3744,
                   3733,3723,3713,3702,3692,3682,3671,3661,3651,3641,3631,3621,3611,3601,3591,3581,3571,3561,3552,3542,3532,3522,3513,3503,3494,
                   3484,3475,3465,3456,3446,3437,3427,
              },
              /* drc_high_LUT_point */
              {
                   7448,7448,6943,6518,6144,5808,5503,5224,4965,4726,4503,4294,4099,3916,3743,3581,3427,3282,3144,3014,2890,2773,2661,2555,2454,
                   2358,2266,2178,2095,2015,1939,1866,1796,1729,1666,1604,1546,1490,1436,1384,1335,1287,1242,1198,1156,1115,1077,1039,1003, 969,
                    936, 904, 873, 844, 815, 788, 761, 736, 711, 688, 665, 643, 622, 602, 582, 563, 545, 527, 510, 494, 478, 462, 448, 433, 420,
                    406, 393, 381, 369, 357, 346, 335, 325, 315, 305, 295, 286, 277, 269, 261, 253, 245, 237, 230, 223, 216, 210, 203, 197, 191,
                    185, 180, 174, 169, 164, 159, 154, 150, 145, 141, 137, 133, 129, 125, 121, 117, 114, 111, 107, 104, 101,  98,  95,  92,  90,
                     87,  84,  82,  80,  77,  75,  73,  71,  68,  66,  65,  63,  61,  59,  57,  56,  54,  52,  51,  49,  48,  46,  45,  44,  42,
                     41,  40,  39,  38,  37,  35,  34,  33,  32,  31,  30,  30,  29,  28,  27,  26,  25,  25,  24,  23,  22,  22,  21,  20,  20,
                     19,  19,  18,  17,  17,  16,  16,  15,  15,  14,  14,  13,  13,  13,  12,  12,  11,  11,  11,  10,  10,  10,   9,   9,   9,
                      8,   8,   8,   7,   7,   7,   7,   6,   6,   6,   6,   5,   5,   5,   5,   5,   4,   4,   4,   4,   4,   3,   3,   3,   3,
                      3,   3,   2,   2,   2,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,
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
              2,
              /* k2 */
              8,
              /* k3 */
              0.8,
           },
           /* para 1*/
           {
              /* drc_low_LUT_point */
              {
                   6484,6484,6434,6390,6349,6311,6274,6238,6204,6171,6138,6107,6076,6046,6016,5987,5959,5931,5903,5876,5849,5823,5797,5771,5746,
                   5721,5697,5672,5648,5624,5601,5578,5555,5532,5509,5487,5465,5443,5421,5400,5378,5357,5336,5316,5295,5275,5254,5234,5214,5195,
                   5175,5155,5136,5117,5098,5079,5060,5042,5023,5005,4987,4969,4951,4933,4915,4898,4880,4863,4846,4828,4811,4795,4778,4761,4744,
                   4728,4712,4695,4679,4663,4647,4631,4615,4599,4584,4568,4553,4537,4522,4507,4492,4477,4462,4447,4432,4418,4403,4388,4374,4360,
                   4345,4331,4317,4303,4289,4275,4261,4247,4233,4220,4206,4193,4179,4166,4152,4139,4126,4113,4100,4087,4074,4061,4048,4035,4023,
                   4010,3998,3985,3973,3960,3948,3936,3923,3911,3899,3887,3875,3863,3851,3839,3828,3816,3804,3793,3781,3770,3758,3747,3735,3724,
                   3713,3701,3690,3679,3668,3657,3646,3635,3624,3613,3603,3592,3581,3571,3560,3549,3539,3528,3518,3507,3497,3487,3477,3466,3456,
                   3446,3436,3426,3416,3406,3396,3386,3376,3366,3357,3347,3337,3327,3318,3308,3299,3289,3280,3270,3261,3251,3242,3233,3224,3214,
                   3205,3196,3187,3178,3169,3160,3151,3142,3133,3124,3115,3106,3098,3089,3080,3072,3063,3054,3046,3037,3029,3020,3012,3003,2995,
                   2987,2978,2970,2962,2953,2945,2937,2929,2921,2913,2905,2897,2889,2881,2873,2865,2857,2849,2841,2833,2826,2818,2810,2802,2795,
                   2787,2780,2772,2764,2757,2749,2742,
              },
              /* drc_high_LUT_point */
              {
                   5959,5959,5554,5214,4915,4647,4403,4179,3972,3781,3602,3435,3279,3133,2994,2864,2741,2625,2515,2411,2312,2218,2129,2044,1963,
                   1886,1813,1743,1676,1612,1551,1493,1437,1383,1332,1283,1237,1192,1149,1107,1068,1030, 993, 958, 925, 892, 861, 831, 803, 775,
                    749, 723, 698, 675, 652, 630, 609, 589, 569, 550, 532, 514, 497, 481, 465, 450, 436, 422, 408, 395, 382, 370, 358, 347, 336,
                    325, 315, 305, 295, 286, 277, 268, 260, 252, 244, 236, 229, 222, 215, 208, 202, 196, 190, 184, 178, 173, 168, 163, 158, 153,
                    148, 144, 139, 135, 131, 127, 123, 120, 116, 113, 109, 106, 103, 100,  97,  94,  91,  88,  86,  83,  81,  78,  76,  74,  72,
                     69,  67,  65,  64,  62,  60,  58,  56,  55,  53,  52,  50,  49,  47,  46,  44,  43,  42,  41,  39,  38,  37,  36,  35,  34,
                     33,  32,  31,  30,  29,  28,  27,  27,  26,  25,  24,  24,  23,  22,  21,  21,  20,  20,  19,  18,  18,  17,  17,  16,  16,
                     15,  15,  14,  14,  13,  13,  13,  12,  12,  11,  11,  11,  10,  10,  10,   9,   9,   9,   8,   8,   8,   8,   7,   7,   7,
                      7,   6,   6,   6,   6,   5,   5,   5,   5,   5,   4,   4,   4,   4,   4,   4,   3,   3,   3,   3,   3,   3,   3,   2,   2,
                      2,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,
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
              1.6,
              /* k2 */
              8,
              /* k3 */
              0.8,
           },
           /* para 2*/
           {
              /* drc_low_LUT_point */
              {
                   5268,5268,5228,5192,5159,5127,5097,5068,5041,5014,4987,4962,4937,4912,4888,4864,4841,4819,4796,4774,4752,4731,4710,4689,4669,
                   4648,4628,4609,4589,4570,4551,4532,4513,4494,4476,4458,4440,4422,4405,4387,4370,4353,4336,4319,4302,4285,4269,4253,4237,4220,
                   4205,4189,4173,4158,4142,4127,4112,4096,4081,4067,4052,4037,4022,4008,3994,3979,3965,3951,3937,3923,3909,3895,3882,3868,3855,
                   3841,3828,3815,3802,3789,3776,3763,3750,3737,3724,3712,3699,3687,3674,3662,3650,3637,3625,3613,3601,3589,3577,3565,3554,3542,
                   3530,3519,3507,3496,3485,3473,3462,3451,3440,3428,3417,3406,3395,3385,3374,3363,3352,3342,3331,3320,3310,3300,3289,3279,3268,
                   3258,3248,3238,3228,3218,3208,3198,3188,3178,3168,3158,3148,3139,3129,3119,3110,3100,3091,3081,3072,3063,3053,3044,3035,3026,
                   3016,3007,2998,2989,2980,2971,2962,2953,2945,2936,2927,2918,2910,2901,2892,2884,2875,2867,2858,2850,2841,2833,2825,2816,2808,
                   2800,2792,2783,2775,2767,2759,2751,2743,2735,2727,2719,2711,2703,2696,2688,2680,2672,2665,2657,2649,2642,2634,2627,2619,2612,
                   2604,2597,2589,2582,2575,2567,2560,2553,2545,2538,2531,2524,2517,2510,2503,2496,2489,2482,2475,2468,2461,2454,2447,2440,2433,
                   2426,2420,2413,2406,2400,2393,2386,2380,2373,2366,2360,2353,2347,2340,2334,2328,2321,2315,2308,2302,2296,2289,2283,2277,2271,
                   2264,2258,2252,2246,2240,2234,2228,
              },
              /* drc_high_LUT_point */
              {
                   4841,4841,4513,4236,3993,3775,3577,3395,3227,3072,2927,2791,2664,2545,2433,2327,2227,2133,2044,1959,1878,1802,1730,1661,1595,
                   1532,1473,1416,1361,1310,1260,1213,1167,1124,1082,1043,1005, 968, 933, 900, 867, 837, 807, 778, 751, 725, 700, 675, 652, 630,
                    608, 587, 567, 548, 530, 512, 495, 478, 462, 447, 432, 418, 404, 391, 378, 366, 354, 342, 331, 321, 310, 300, 291, 281, 273,
                    264, 256, 247, 240, 232, 225, 218, 211, 204, 198, 192, 186, 180, 175, 169, 164, 159, 154, 149, 145, 140, 136, 132, 128, 124,
                    120, 117, 113, 110, 106, 103, 100,  97,  94,  91,  89,  86,  83,  81,  79,  76,  74,  72,  70,  68,  66,  64,  62,  60,  58,
                     56,  55,  53,  52,  50,  49,  47,  46,  44,  43,  42,  41,  39,  38,  37,  36,  35,  34,  33,  32,  31,  30,  29,  28,  27,
                     27,  26,  25,  24,  24,  23,  22,  22,  21,  20,  20,  19,  18,  18,  17,  17,  16,  16,  15,  15,  14,  14,  14,  13,  13,
                     12,  12,  12,  11,  11,  10,  10,  10,   9,   9,   9,   9,   8,   8,   8,   7,   7,   7,   7,   6,   6,   6,   6,   6,   5,
                      5,   5,   5,   5,   4,   4,   4,   4,   4,   4,   3,   3,   3,   3,   3,   3,   3,   3,   2,   2,   2,   2,   2,   2,   2,
                      2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
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
              1.3,
              /* k2 */
              8,
              /* k3 */
              0.8,
           },
           /* para 3*/
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
              0.8,
           },
           /* para 4*/
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
                -0.9349,2.3071,-0.3722,
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
             180,
          },
          /* aec trigger 1 */
          {
             200,
             310,
          },
          /* aec trigger 2 */
          {
             320,
             350,
          },
          /* aec trigger 3 */
          {
             400,
             470,
          },
          /* aec trigger 4 */
          {
             500,
             550,
          },
       },
       4096,//gamma_lut_size
       /* isp_sub_module_gamma_lut_pra pra[MIDDLE_TRIGGER_COUNT] */
       {
          /* aec trigger 0 */
          {
                   0,   4,   9,  14,  19,  24,  29,  33,  38,  42,  47,  51,  56,  60,  64,  69,  73,  77,  81,  85,  89,  93,  97, 101, 105, 109, 113, 116, 120, 123, 126, 134,
                 142, 150, 157, 164, 171, 178, 185, 191, 198, 204, 210, 216, 222, 228, 233, 238, 244, 249, 254, 258, 263, 267, 272, 276, 280, 283, 287, 290, 293, 301, 308, 316,
                 323, 329, 336, 343, 349, 355, 361, 367, 373, 379, 384, 389, 395, 399, 404, 409, 413, 418, 422, 426, 430, 433, 437, 440, 443, 446, 449, 456, 463, 471, 478, 485,
                 491, 498, 504, 511, 517, 523, 529, 535, 540, 546, 551, 556, 561, 566, 571, 576, 580, 585, 589, 593, 597, 601, 604, 608, 610, 615, 620, 625, 629, 634, 638, 643,
                 647, 652, 656, 660, 665, 669, 673, 677, 681, 685, 689, 693, 697, 701, 705, 709, 713, 716, 720, 724, 727, 731, 734, 738, 741, 744, 748, 751, 754, 757, 760, 764,
                 766, 771, 775, 780, 784, 789, 793, 798, 802, 806, 810, 815, 819, 823, 827, 832, 836, 840, 844, 848, 852, 856, 860, 864, 868, 872, 876, 880, 884, 887, 891, 895,
                 899, 902, 906, 910, 914, 917, 921, 924, 928, 931, 935, 938, 942, 945, 949, 952, 956, 959, 962, 966, 969, 972, 975, 978, 982, 985, 988, 991, 994, 998,1002,1006,
                1011,1015,1019,1023,1027,1032,1036,1040,1044,1048,1052,1056,1060,1064,1068,1072,1076,1080,1084,1088,1092,1096,1100,1104,1107,1111,1115,1119,1123,1126,1130,1134,
                1138,1141,1145,1149,1152,1156,1159,1163,1166,1170,1174,1177,1181,1184,1187,1191,1194,1198,1201,1204,1208,1211,1214,1218,1221,1224,1227,1231,1234,1237,1240,1243,
                1246,1250,1253,1256,1259,1262,1265,1268,1271,1274,1277,1280,1283,1286,1290,1293,1297,1301,1304,1308,1312,1315,1319,1322,1326,1329,1333,1336,1340,1343,1347,1350,
                1354,1357,1361,1364,1368,1371,1374,1378,1381,1384,1388,1391,1394,1398,1401,1404,1407,1411,1414,1417,1420,1424,1427,1430,1433,1436,1439,1443,1446,1449,1452,1455,
                1458,1461,1464,1467,1470,1473,1476,1479,1482,1485,1488,1491,1494,1497,1500,1503,1506,1509,1511,1514,1517,1520,1523,1526,1528,1531,1534,1537,1539,1542,1545,1548,
                1550,1553,1556,1558,1561,1564,1566,1569,1571,1574,1577,1579,1582,1584,1587,1589,1591,1594,1597,1600,1603,1607,1610,1613,1616,1619,1622,1625,1629,1632,1635,1638,
                1641,1644,1647,1650,1653,1656,1659,1662,1665,1668,1671,1674,1677,1680,1683,1686,1689,1692,1695,1698,1700,1703,1706,1709,1712,1715,1718,1720,1723,1726,1729,1732,
                1734,1737,1740,1743,1745,1748,1751,1753,1756,1759,1761,1764,1767,1769,1772,1775,1777,1780,1782,1785,1788,1790,1793,1795,1798,1800,1803,1805,1808,1810,1813,1815,
                1818,1820,1823,1825,1828,1830,1832,1835,1837,1840,1842,1844,1847,1849,1851,1854,1856,1858,1860,1863,1865,1867,1870,1872,1874,1876,1878,1881,1883,1885,1887,1889,
                1891,1894,1896,1898,1900,1902,1904,1906,1907,1910,1912,1915,1917,1920,1922,1924,1927,1929,1931,1934,1936,1939,1941,1943,1946,1948,1950,1952,1955,1957,1959,1962,
                1964,1966,1968,1971,1973,1975,1977,1980,1982,1984,1986,1988,1991,1993,1995,1997,1999,2001,2003,2006,2008,2010,2012,2014,2016,2018,2020,2022,2025,2027,2029,2031,
                2033,2035,2037,2039,2041,2043,2045,2047,2049,2051,2053,2055,2057,2059,2061,2063,2065,2066,2068,2070,2072,2074,2076,2078,2080,2082,2083,2085,2087,2089,2091,2093,
                2095,2096,2098,2100,2102,2104,2105,2107,2109,2111,2112,2114,2116,2118,2119,2121,2123,2124,2126,2128,2129,2131,2133,2135,2136,2138,2139,2141,2143,2144,2146,2148,
                2149,2151,2153,2155,2158,2160,2162,2164,2166,2169,2171,2173,2175,2177,2180,2182,2184,2186,2188,2190,2193,2195,2197,2199,2201,2203,2205,2207,2209,2211,2213,2215,
                2217,2219,2221,2224,2226,2228,2230,2231,2233,2235,2237,2239,2241,2243,2245,2247,2249,2251,2253,2255,2257,2258,2260,2262,2264,2266,2268,2270,2271,2273,2275,2277,
                2279,2280,2282,2284,2286,2287,2289,2291,2293,2294,2296,2298,2299,2301,2303,2305,2306,2308,2310,2311,2313,2314,2316,2318,2319,2321,2323,2324,2326,2327,2329,2330,
                2332,2333,2335,2336,2338,2340,2341,2343,2344,2345,2347,2348,2350,2351,2353,2354,2356,2357,2358,2360,2361,2363,2364,2365,2367,2368,2369,2371,2372,2374,2375,2376,
                2378,2379,2380,2382,2383,2385,2386,2387,2389,2390,2391,2393,2394,2396,2397,2398,2400,2401,2402,2404,2405,2406,2408,2409,2410,2412,2413,2414,2416,2417,2418,2420,
                2421,2422,2424,2425,2426,2428,2429,2430,2432,2433,2434,2436,2437,2438,2439,2441,2442,2443,2445,2446,2447,2448,2450,2451,2452,2454,2455,2456,2457,2459,2460,2461,
                2462,2464,2465,2466,2467,2469,2470,2471,2472,2474,2475,2476,2477,2479,2480,2481,2482,2483,2485,2486,2487,2488,2490,2491,2492,2493,2494,2496,2497,2498,2499,2500,
                2502,2503,2504,2505,2506,2507,2509,2510,2511,2512,2513,2515,2516,2517,2518,2519,2520,2521,2523,2524,2525,2526,2527,2528,2529,2531,2532,2533,2534,2535,2536,2537,
                2539,2540,2541,2542,2542,2544,2545,2546,2547,2548,2549,2550,2551,2552,2553,2554,2555,2556,2558,2559,2560,2561,2562,2563,2564,2565,2566,2567,2568,2569,2570,2571,
                2572,2573,2574,2575,2576,2578,2579,2580,2581,2582,2583,2584,2585,2586,2587,2588,2589,2590,2591,2592,2593,2594,2595,2596,2597,2598,2599,2600,2601,2602,2603,2604,
                2605,2606,2607,2608,2609,2610,2611,2612,2613,2614,2615,2616,2617,2618,2619,2620,2621,2622,2623,2624,2625,2626,2627,2628,2629,2630,2631,2632,2633,2634,2635,2636,
                2636,2637,2638,2639,2640,2641,2642,2643,2644,2645,2646,2647,2648,2649,2650,2651,2652,2653,2653,2654,2655,2656,2657,2658,2659,2660,2661,2662,2663,2664,2665,2666,
                2666,2667,2668,2669,2670,2671,2672,2673,2674,2675,2676,2676,2677,2678,2679,2680,2681,2682,2683,2684,2684,2685,2686,2687,2688,2689,2690,2691,2691,2692,2693,2694,
                2695,2696,2697,2698,2698,2699,2700,2701,2702,2703,2704,2704,2705,2706,2707,2708,2709,2710,2710,2711,2712,2713,2714,2715,2715,2716,2717,2718,2719,2720,2720,2721,
                2722,2723,2724,2725,2725,2726,2727,2728,2729,2730,2730,2731,2732,2733,2734,2734,2735,2736,2737,2738,2738,2739,2740,2741,2742,2742,2743,2744,2745,2746,2746,2747,
                2748,2749,2749,2750,2751,2752,2753,2753,2754,2755,2756,2756,2757,2758,2759,2759,2760,2761,2762,2763,2763,2764,2765,2766,2766,2767,2768,2769,2769,2770,2771,2772,
                2772,2773,2774,2774,2775,2776,2777,2777,2778,2779,2780,2780,2781,2782,2782,2783,2784,2785,2785,2786,2787,2787,2788,2789,2790,2790,2791,2792,2792,2793,2794,2795,
                2795,2796,2797,2797,2798,2799,2799,2800,2801,2801,2802,2803,2803,2804,2805,2805,2805,2806,2807,2808,2809,2809,2810,2811,2812,2813,2813,2814,2815,2816,2817,2817,
                2818,2819,2820,2820,2821,2822,2823,2823,2824,2825,2826,2827,2827,2828,2829,2830,2830,2831,2832,2833,2833,2834,2835,2836,2836,2837,2838,2839,2839,2840,2841,2842,
                2842,2843,2844,2844,2845,2846,2847,2847,2848,2849,2850,2850,2851,2852,2852,2853,2854,2855,2855,2856,2857,2857,2858,2859,2859,2860,2861,2862,2862,2863,2864,2864,
                2865,2866,2866,2867,2868,2868,2869,2870,2870,2871,2872,2873,2873,2874,2875,2875,2876,2877,2877,2878,2879,2879,2880,2881,2881,2882,2882,2883,2884,2884,2885,2886,
                2886,2887,2888,2888,2889,2890,2890,2891,2892,2892,2893,2893,2894,2895,2895,2896,2897,2897,2898,2898,2899,2900,2900,2901,2902,2902,2903,2903,2904,2905,2905,2906,
                2906,2907,2908,2908,2909,2909,2910,2911,2911,2912,2912,2913,2914,2914,2915,2915,2916,2916,2917,2918,2918,2919,2919,2920,2920,2921,2922,2922,2923,2923,2924,2924,
                2925,2926,2926,2927,2927,2928,2928,2929,2929,2930,2931,2931,2932,2932,2933,2933,2934,2934,2935,2935,2936,2937,2937,2938,2938,2939,2939,2940,2940,2941,2941,2942,
                2942,2943,2943,2944,2944,2945,2945,2946,2946,2947,2947,2948,2948,2949,2949,2950,2950,2951,2951,2952,2952,2953,2953,2954,2954,2955,2955,2956,2956,2957,2957,2958,
                2958,2959,2959,2960,2960,2960,2961,2961,2962,2962,2963,2963,2964,2964,2965,2965,2966,2966,2966,2967,2967,2968,2968,2969,2969,2970,2970,2970,2971,2971,2972,2972,
                2973,2973,2973,2974,2974,2975,2975,2976,2976,2976,2977,2977,2978,2978,2979,2979,2979,2980,2980,2981,2981,2981,2982,2982,2983,2983,2983,2984,2984,2984,2984,2985,
                2985,2986,2986,2987,2987,2988,2988,2988,2989,2989,2990,2990,2991,2991,2992,2992,2992,2993,2993,2994,2994,2995,2995,2995,2996,2996,2997,2997,2998,2998,2999,2999,
                2999,3000,3000,3001,3001,3002,3002,3002,3003,3003,3004,3004,3004,3005,3005,3006,3006,3007,3007,3007,3008,3008,3009,3009,3010,3010,3010,3011,3011,3012,3012,3012,
                3013,3013,3014,3014,3014,3015,3015,3016,3016,3016,3017,3017,3018,3018,3018,3019,3019,3020,3020,3020,3021,3021,3022,3022,3022,3023,3023,3024,3024,3024,3025,3025,
                3026,3026,3026,3027,3027,3027,3028,3028,3029,3029,3029,3030,3030,3030,3031,3031,3032,3032,3032,3033,3033,3033,3034,3034,3035,3035,3035,3036,3036,3036,3037,3037,
                3038,3038,3038,3039,3039,3039,3040,3040,3040,3041,3041,3042,3042,3042,3043,3043,3043,3044,3044,3044,3045,3045,3045,3046,3046,3046,3047,3047,3048,3048,3048,3049,
                3049,3049,3050,3050,3050,3051,3051,3051,3052,3052,3052,3053,3053,3053,3054,3054,3054,3055,3055,3055,3056,3056,3056,3057,3057,3057,3058,3058,3058,3059,3059,3059,
                3060,3060,3060,3061,3061,3061,3061,3062,3062,3062,3063,3063,3063,3064,3064,3064,3065,3065,3065,3066,3066,3066,3066,3067,3067,3067,3068,3068,3068,3069,3069,3069,
                3070,3070,3070,3070,3071,3071,3071,3072,3072,3072,3073,3073,3073,3073,3074,3074,3074,3075,3075,3075,3075,3076,3076,3076,3077,3077,3077,3077,3078,3078,3078,3079,
                3079,3079,3079,3080,3080,3080,3081,3081,3081,3081,3082,3082,3082,3082,3083,3083,3083,3084,3084,3084,3084,3085,3085,3085,3085,3086,3086,3086,3086,3087,3087,3087,
                3087,3088,3088,3088,3088,3089,3089,3089,3089,3090,3090,3090,3091,3091,3091,3091,3092,3092,3092,3092,3093,3093,3093,3094,3094,3094,3094,3095,3095,3095,3095,3096,
                3096,3096,3096,3097,3097,3097,3098,3098,3098,3098,3099,3099,3099,3099,3100,3100,3100,3100,3101,3101,3101,3102,3102,3102,3102,3103,3103,3103,3103,3104,3104,3104,
                3104,3105,3105,3105,3105,3106,3106,3106,3106,3107,3107,3107,3107,3108,3108,3108,3109,3109,3109,3109,3110,3110,3110,3110,3111,3111,3111,3111,3112,3112,3112,3112,
                3113,3113,3113,3113,3114,3114,3114,3114,3115,3115,3115,3115,3116,3116,3116,3116,3117,3117,3117,3117,3118,3118,3118,3118,3119,3119,3119,3119,3120,3120,3120,3120,
                3121,3121,3121,3121,3122,3122,3122,3122,3123,3123,3123,3123,3123,3124,3124,3124,3124,3125,3125,3125,3125,3126,3126,3126,3126,3127,3127,3127,3127,3128,3128,3128,
                3128,3129,3129,3129,3129,3129,3130,3130,3130,3130,3131,3131,3131,3131,3132,3132,3132,3132,3133,3133,3133,3133,3133,3134,3134,3134,3134,3135,3135,3135,3135,3136,
                3136,3136,3136,3137,3137,3137,3137,3137,3138,3138,3138,3138,3139,3139,3139,3139,3140,3140,3140,3140,3140,3141,3141,3141,3141,3142,3142,3142,3142,3142,3143,3143,
                3143,3143,3144,3144,3144,3144,3144,3145,3145,3145,3145,3146,3146,3146,3146,3146,3147,3147,3147,3147,3148,3148,3148,3148,3148,3149,3149,3149,3149,3150,3150,3150,
                3150,3150,3151,3151,3151,3151,3152,3152,3152,3152,3152,3153,3153,3153,3153,3153,3154,3154,3154,3154,3155,3155,3155,3155,3155,3156,3156,3156,3156,3156,3157,3157,
                3157,3157,3157,3158,3158,3158,3158,3159,3159,3159,3159,3159,3160,3160,3160,3160,3160,3161,3161,3161,3161,3161,3162,3162,3162,3162,3162,3163,3163,3163,3163,3164,
                3164,3164,3164,3164,3165,3165,3165,3165,3165,3166,3166,3166,3166,3166,3167,3167,3167,3167,3167,3168,3168,3168,3168,3168,3169,3169,3169,3169,3169,3170,3170,3170,
                3170,3170,3171,3171,3171,3171,3171,3172,3172,3172,3172,3172,3172,3173,3173,3173,3173,3173,3174,3174,3174,3174,3174,3175,3175,3175,3175,3175,3176,3176,3176,3176,
                3176,3177,3177,3177,3177,3177,3177,3178,3178,3178,3178,3178,3179,3179,3179,3179,3179,3180,3180,3180,3180,3180,3180,3181,3181,3181,3181,3181,3182,3182,3182,3182,
                3182,3182,3183,3183,3183,3183,3183,3184,3184,3184,3184,3184,3184,3185,3185,3185,3185,3185,3186,3186,3186,3186,3186,3186,3187,3187,3187,3187,3187,3188,3188,3188,
                3188,3188,3188,3189,3189,3189,3189,3189,3189,3190,3190,3190,3190,3190,3190,3191,3191,3191,3191,3191,3192,3192,3192,3192,3192,3192,3193,3193,3193,3193,3193,3193,
                3194,3194,3194,3194,3194,3194,3195,3195,3195,3195,3195,3195,3196,3196,3196,3196,3196,3196,3197,3197,3197,3197,3197,3197,3198,3198,3198,3198,3198,3198,3199,3199,
                3199,3199,3199,3199,3200,3200,3200,3200,3200,3200,3200,3201,3201,3201,3201,3201,3201,3202,3202,3202,3202,3202,3202,3203,3203,3203,3203,3203,3203,3203,3204,3204,
                3204,3204,3204,3204,3205,3205,3205,3205,3205,3205,3205,3206,3206,3206,3206,3206,3206,3207,3207,3207,3207,3207,3207,3207,3208,3208,3208,3208,3208,3208,3209,3209,
                3209,3209,3209,3209,3209,3210,3210,3210,3210,3210,3210,3210,3211,3211,3211,3211,3211,3211,3211,3212,3212,3212,3212,3212,3212,3212,3213,3213,3213,3213,3213,3213,
                3213,3214,3214,3214,3214,3214,3214,3214,3215,3215,3215,3215,3215,3215,3215,3216,3216,3216,3216,3216,3216,3216,3217,3217,3217,3217,3217,3217,3217,3217,3218,3218,
                3218,3218,3218,3218,3218,3219,3219,3219,3219,3219,3219,3219,3219,3220,3220,3220,3220,3220,3220,3220,3221,3221,3221,3221,3221,3221,3221,3221,3222,3222,3222,3222,
                3222,3222,3222,3222,3223,3223,3223,3223,3223,3223,3223,3223,3224,3224,3224,3224,3224,3224,3224,3224,3225,3225,3225,3225,3225,3225,3225,3225,3226,3226,3226,3226,
                3226,3226,3226,3226,3227,3227,3227,3227,3227,3227,3227,3228,3228,3228,3228,3228,3229,3229,3229,3229,3229,3229,3230,3230,3230,3230,3230,3230,3231,3231,3231,3231,
                3231,3231,3232,3232,3232,3232,3232,3233,3233,3233,3233,3233,3233,3234,3234,3234,3234,3234,3235,3235,3235,3235,3235,3236,3236,3236,3236,3236,3236,3237,3237,3237,
                3237,3237,3238,3238,3238,3238,3238,3239,3239,3239,3239,3239,3240,3240,3240,3240,3240,3241,3241,3241,3241,3241,3242,3242,3242,3242,3242,3243,3243,3243,3243,3243,
                3244,3244,3244,3244,3245,3245,3245,3245,3245,3246,3246,3246,3246,3246,3247,3247,3247,3247,3248,3248,3248,3248,3248,3249,3249,3249,3249,3250,3250,3250,3250,3250,
                3251,3251,3251,3251,3252,3252,3252,3252,3252,3253,3253,3253,3253,3254,3254,3254,3254,3255,3255,3255,3255,3255,3256,3256,3256,3256,3257,3257,3257,3257,3258,3258,
                3258,3258,3259,3259,3259,3259,3260,3260,3260,3260,3261,3261,3261,3261,3262,3262,3262,3262,3263,3263,3263,3263,3264,3264,3264,3264,3265,3265,3265,3265,3266,3266,
                3266,3266,3267,3267,3267,3267,3268,3268,3268,3268,3269,3269,3269,3269,3270,3270,3270,3271,3271,3271,3271,3272,3272,3272,3272,3273,3273,3273,3274,3274,3274,3274,
                3275,3275,3275,3275,3276,3276,3276,3277,3277,3277,3277,3278,3278,3278,3278,3279,3279,3279,3280,3280,3280,3280,3281,3281,3281,3282,3282,3282,3282,3283,3283,3283,
                3284,3284,3284,3284,3285,3285,3285,3286,3286,3286,3287,3287,3287,3287,3288,3288,3288,3289,3289,3289,3289,3290,3290,3290,3291,3291,3291,3292,3292,3292,3292,3293,
                3293,3293,3294,3294,3294,3295,3295,3295,3296,3296,3296,3296,3297,3297,3297,3298,3298,3298,3299,3299,3299,3300,3300,3300,3301,3301,3301,3302,3302,3302,3302,3303,
                3303,3303,3304,3304,3304,3305,3305,3305,3306,3306,3306,3307,3307,3307,3308,3308,3308,3309,3309,3309,3310,3310,3310,3311,3311,3311,3312,3312,3312,3313,3313,3313,
                3314,3314,3314,3315,3315,3315,3316,3316,3316,3317,3317,3317,3318,3318,3318,3319,3319,3319,3320,3320,3320,3321,3321,3322,3322,3322,3323,3323,3323,3324,3324,3324,
                3325,3325,3325,3326,3326,3326,3327,3327,3328,3328,3328,3329,3329,3329,3330,3330,3330,3331,3331,3331,3332,3332,3333,3333,3333,3334,3334,3334,3335,3335,3336,3336,
                3336,3337,3337,3337,3338,3338,3338,3339,3339,3340,3340,3340,3341,3341,3341,3342,3342,3343,3343,3343,3344,3344,3344,3345,3345,3346,3346,3346,3347,3347,3348,3348,
                3348,3349,3349,3349,3350,3350,3351,3351,3351,3352,3352,3353,3353,3353,3354,3354,3355,3355,3355,3356,3356,3356,3357,3357,3358,3358,3358,3359,3359,3360,3360,3360,
                3361,3361,3362,3362,3362,3363,3363,3364,3364,3365,3365,3365,3366,3366,3367,3367,3367,3368,3368,3369,3369,3369,3370,3370,3371,3371,3371,3372,3372,3373,3373,3374,
                3374,3374,3375,3375,3376,3376,3376,3377,3377,3378,3378,3379,3379,3379,3380,3380,3381,3381,3382,3382,3382,3383,3383,3384,3384,3385,3385,3385,3386,3386,3387,3387,
                3388,3388,3388,3389,3389,3390,3390,3391,3391,3391,3392,3392,3393,3393,3394,3394,3395,3395,3395,3396,3396,3397,3397,3398,3398,3399,3399,3399,3400,3400,3401,3401,
                3402,3402,3403,3403,3403,3404,3404,3405,3405,3406,3406,3407,3407,3408,3408,3408,3409,3409,3410,3410,3411,3411,3412,3412,3413,3413,3413,3414,3414,3415,3415,3416,
                3416,3417,3417,3418,3418,3419,3419,3420,3420,3420,3421,3421,3422,3422,3423,3423,3424,3424,3425,3425,3426,3426,3427,3427,3428,3428,3428,3429,3429,3430,3430,3431,
                3431,3432,3432,3433,3433,3434,3434,3435,3435,3436,3436,3437,3437,3438,3438,3439,3439,3440,3440,3441,3441,3442,3442,3443,3443,3444,3444,3445,3445,3445,3446,3446,
                3447,3447,3448,3448,3449,3449,3450,3450,3451,3451,3452,3452,3453,3453,3454,3454,3455,3455,3456,3457,3457,3458,3458,3459,3459,3460,3460,3461,3461,3462,3462,3463,
                3463,3464,3464,3465,3465,3466,3466,3467,3467,3468,3468,3469,3469,3470,3470,3471,3471,3472,3472,3473,3473,3474,3475,3475,3476,3476,3477,3477,3478,3478,3479,3479,
                3480,3480,3481,3481,3482,3482,3483,3483,3484,3485,3485,3486,3486,3487,3487,3488,3488,3489,3489,3490,3490,3491,3491,3492,3493,3493,3494,3494,3495,3495,3496,3496,
                3497,3497,3498,3499,3499,3500,3500,3501,3501,3502,3502,3503,3503,3504,3505,3505,3506,3506,3507,3507,3508,3508,3509,3510,3510,3511,3511,3512,3512,3513,3513,3514,
                3515,3515,3516,3516,3517,3517,3518,3519,3519,3520,3520,3521,3521,3522,3522,3523,3524,3524,3525,3525,3526,3526,3527,3528,3528,3529,3529,3530,3530,3531,3532,3532,
                3533,3533,3534,3534,3535,3536,3536,3537,3537,3538,3539,3539,3540,3540,3541,3541,3542,3543,3543,3544,3544,3545,3546,3546,3547,3547,3548,3548,3549,3550,3550,3551,
                3551,3552,3553,3553,3554,3554,3555,3556,3556,3557,3557,3558,3559,3559,3560,3560,3561,3562,3562,3563,3563,3564,3565,3565,3566,3566,3567,3568,3568,3569,3569,3570,
                3571,3571,3572,3572,3573,3574,3574,3575,3576,3576,3577,3577,3578,3578,3579,3579,3580,3580,3581,3581,3582,3582,3583,3584,3584,3585,3585,3586,3586,3587,3587,3588,
                3588,3589,3589,3590,3591,3591,3592,3592,3593,3593,3594,3594,3595,3596,3596,3597,3597,3598,3598,3599,3599,3600,3601,3601,3602,3602,3603,3603,3604,3604,3605,3606,
                3606,3607,3607,3608,3608,3609,3610,3610,3611,3611,3612,3612,3613,3614,3614,3615,3615,3616,3616,3617,3618,3618,3619,3619,3620,3621,3621,3622,3622,3623,3623,3624,
                3625,3625,3626,3626,3627,3628,3628,3629,3629,3630,3630,3631,3632,3632,3633,3633,3634,3635,3635,3636,3636,3637,3638,3638,3639,3639,3640,3641,3641,3642,3642,3643,
                3644,3644,3645,3645,3646,3647,3647,3648,3649,3649,3650,3650,3651,3652,3652,3653,3653,3654,3655,3655,3656,3657,3657,3658,3658,3659,3660,3660,3661,3662,3662,3663,
                3663,3664,3665,3665,3666,3667,3667,3668,3668,3669,3670,3670,3671,3672,3672,3673,3674,3674,3675,3675,3676,3677,3677,3678,3679,3679,3680,3681,3681,3682,3682,3683,
                3684,3684,3685,3686,3686,3687,3688,3688,3689,3690,3690,3691,3692,3692,3693,3694,3694,3695,3696,3696,3697,3698,3698,3699,3700,3700,3701,3701,3702,3703,3703,3704,
                3705,3705,3706,3707,3708,3708,3709,3710,3710,3711,3712,3712,3713,3714,3714,3715,3716,3716,3717,3718,3718,3719,3720,3720,3721,3722,3722,3723,3724,3724,3725,3726,
                3727,3727,3728,3729,3729,3730,3731,3731,3732,3733,3733,3734,3735,3736,3736,3737,3738,3738,3739,3740,3740,3741,3742,3743,3743,3744,3745,3745,3746,3747,3747,3748,
                3749,3750,3750,3751,3752,3752,3753,3754,3755,3755,3756,3757,3757,3758,3759,3760,3760,3761,3762,3762,3763,3764,3765,3765,3766,3767,3768,3768,3769,3770,3770,3771,
                3772,3773,3773,3774,3775,3776,3776,3777,3778,3778,3779,3780,3781,3781,3782,3783,3784,3784,3785,3786,3787,3787,3788,3789,3790,3790,3791,3792,3793,3793,3794,3795,
                3796,3796,3797,3798,3799,3799,3800,3801,3802,3802,3803,3804,3805,3805,3806,3807,3808,3808,3809,3810,3811,3811,3812,3813,3814,3814,3815,3816,3817,3818,3818,3819,
                3820,3821,3821,3822,3823,3824,3824,3825,3826,3827,3828,3828,3829,3830,3831,3831,3832,3833,3834,3835,3835,3836,3837,3838,3839,3840,3840,3841,3841,3842,3843,3843,
                3844,3845,3845,3846,3846,3847,3848,3848,3849,3850,3850,3851,3852,3852,3853,3854,3854,3855,3856,3856,3857,3858,3858,3859,3860,3860,3861,3862,3862,3863,3864,3864,
                3865,3866,3866,3867,3868,3869,3869,3870,3871,3871,3872,3873,3873,3874,3875,3876,3876,3877,3878,3878,3879,3880,3881,3881,3882,3883,3883,3884,3885,3886,3886,3887,
                3888,3889,3889,3890,3891,3892,3892,3893,3894,3895,3895,3896,3897,3898,3898,3899,3900,3901,3901,3902,3903,3904,3904,3905,3906,3907,3908,3908,3909,3910,3911,3911,
                3912,3913,3914,3915,3915,3916,3917,3918,3919,3919,3920,3921,3922,3923,3923,3924,3925,3926,3927,3927,3928,3929,3930,3931,3932,3932,3933,3934,3935,3936,3937,3937,
                3938,3939,3940,3941,3942,3942,3943,3944,3945,3946,3947,3948,3948,3949,3950,3951,3952,3953,3954,3954,3955,3956,3957,3958,3959,3960,3961,3961,3962,3963,3964,3965,
                3966,3967,3968,3969,3969,3970,3971,3972,3973,3974,3975,3976,3977,3978,3978,3979,3980,3981,3982,3983,3984,3985,3986,3987,3988,3989,3989,3990,3991,3992,3993,3994,
                3995,3996,3997,3998,3999,4000,4001,4002,4003,4004,4005,4006,4006,4007,4008,4009,4010,4011,4012,4013,4014,4015,4016,4017,4018,4019,4020,4021,4022,4023,4024,4025,
                4026,4027,4028,4029,4030,4031,4032,4033,4034,4035,4036,4037,4038,4039,4040,4041,4042,4043,4044,4045,4046,4047,4048,4049,4050,4051,4052,4053,4054,4055,4056,4057,
                4058,4060,4061,4062,4063,4064,4065,4066,4067,4068,4069,4070,4071,4072,4073,4074,4075,4076,4077,4079,4080,4081,4082,4083,4084,4085,4086,4087,4088,4089,4090,4092,
          },
          /* aec trigger 1 */
          {
                   0,   2,   5,   7,  10,  13,  16,  19,  22,  25,  28,  31,  34,  37,  40,  43,  47,  50,  53,  57,  60,  64,  67,  71,  75,  79,  82,  86,  90,  94,  98, 102,
                 106, 110, 114, 118, 122, 127, 131, 135, 140, 144, 149, 153, 158, 162, 167, 172, 177, 181, 186, 191, 196, 201, 206, 211, 216, 222, 227, 232, 237, 243, 250, 257,
                 264, 270, 277, 284, 291, 298, 305, 311, 318, 325, 332, 339, 346, 353, 359, 366, 373, 380, 387, 394, 401, 408, 415, 422, 429, 435, 441, 448, 455, 462, 469, 475,
                 482, 489, 495, 502, 508, 515, 521, 528, 534, 540, 547, 553, 559, 565, 571, 577, 583, 590, 596, 602, 607, 613, 619, 625, 631, 637, 642, 648, 654, 659, 665, 670,
                 676, 681, 687, 692, 697, 703, 708, 713, 718, 724, 729, 734, 739, 744, 749, 754, 759, 764, 768, 773, 778, 783, 788, 792, 797, 801, 806, 810, 815, 819, 824, 828,
                 833, 837, 842, 847, 852, 857, 861, 866, 871, 876, 880, 885, 890, 894, 899, 904, 908, 913, 917, 922, 926, 931, 935, 940, 944, 948, 953, 957, 961, 966, 970, 974,
                 979, 983, 987, 991, 995, 999,1004,1008,1012,1016,1020,1024,1028,1032,1036,1040,1044,1047,1051,1055,1059,1063,1067,1070,1074,1078,1081,1085,1089,1092,1096,1099,
                1103,1107,1110,1114,1117,1121,1124,1128,1131,1135,1138,1142,1145,1149,1152,1156,1159,1163,1166,1169,1173,1176,1180,1183,1186,1190,1193,1196,1200,1203,1206,1210,
                1213,1216,1219,1223,1226,1229,1232,1235,1239,1242,1245,1248,1251,1254,1258,1261,1264,1267,1270,1273,1276,1279,1282,1285,1288,1291,1294,1297,1300,1303,1306,1309,
                1312,1315,1318,1321,1324,1326,1329,1332,1335,1338,1341,1344,1345,1348,1351,1354,1357,1360,1363,1366,1369,1372,1374,1377,1380,1383,1386,1389,1391,1394,1397,1400,
                1403,1405,1408,1411,1414,1416,1419,1422,1425,1427,1430,1433,1436,1438,1441,1444,1446,1449,1452,1454,1457,1460,1462,1465,1468,1470,1473,1475,1478,1481,1483,1486,
                1488,1491,1493,1496,1499,1501,1504,1506,1509,1511,1514,1516,1519,1521,1524,1526,1529,1531,1534,1536,1539,1541,1543,1546,1548,1551,1553,1555,1558,1560,1563,1565,
                1567,1570,1572,1574,1577,1579,1581,1584,1586,1588,1591,1593,1595,1598,1600,1602,1603,1606,1608,1611,1613,1616,1618,1620,1623,1625,1628,1630,1632,1635,1637,1639,
                1642,1644,1647,1649,1651,1654,1656,1658,1661,1663,1665,1668,1670,1672,1675,1677,1679,1682,1684,1686,1689,1691,1693,1696,1698,1700,1703,1705,1707,1709,1712,1714,
                1716,1719,1721,1723,1725,1728,1730,1732,1735,1737,1739,1741,1744,1746,1748,1750,1752,1755,1757,1759,1761,1764,1766,1768,1770,1772,1775,1777,1779,1781,1784,1786,
                1788,1790,1792,1794,1797,1799,1801,1803,1805,1807,1810,1812,1814,1816,1818,1820,1823,1825,1827,1829,1831,1833,1835,1838,1840,1842,1844,1846,1848,1850,1852,1854,
                1856,1859,1861,1863,1865,1867,1869,1871,1873,1874,1876,1878,1880,1882,1884,1886,1888,1890,1892,1894,1896,1898,1900,1902,1904,1906,1908,1909,1911,1913,1915,1917,
                1919,1921,1923,1924,1926,1928,1930,1932,1934,1936,1937,1939,1941,1943,1945,1946,1948,1950,1952,1954,1955,1957,1959,1961,1962,1964,1966,1968,1969,1971,1973,1975,
                1976,1978,1980,1981,1983,1985,1986,1988,1990,1992,1993,1995,1996,1998,2000,2001,2003,2005,2006,2008,2010,2011,2013,2014,2016,2018,2019,2021,2022,2024,2025,2027,
                2029,2030,2032,2033,2035,2036,2038,2039,2041,2042,2044,2045,2047,2048,2050,2051,2053,2054,2056,2057,2059,2060,2062,2063,2064,2066,2067,2069,2070,2072,2073,2074,
                2070,2072,2074,2076,2078,2080,2082,2084,2086,2088,2090,2092,2094,2096,2097,2099,2101,2103,2105,2107,2109,2111,2112,2114,2116,2118,2120,2122,2124,2125,2127,2129,
                2131,2133,2135,2136,2138,2140,2142,2144,2145,2147,2149,2151,2153,2154,2156,2158,2160,2161,2163,2165,2167,2168,2170,2172,2174,2175,2177,2179,2180,2182,2184,2186,
                2187,2189,2191,2192,2194,2196,2197,2199,2201,2202,2204,2206,2207,2209,2211,2212,2214,2216,2217,2219,2220,2222,2224,2225,2227,2228,2230,2232,2233,2235,2236,2238,
                2240,2241,2243,2244,2246,2247,2249,2250,2252,2254,2255,2257,2258,2260,2261,2263,2264,2266,2267,2269,2270,2272,2273,2275,2276,2278,2279,2281,2282,2283,2285,2286,
                2288,2289,2291,2292,2294,2295,2296,2298,2299,2301,2302,2303,2305,2306,2308,2309,2310,2312,2313,2314,2316,2317,2319,2320,2321,2323,2324,2325,2327,2328,2329,2331,
                2332,2333,2335,2336,2337,2338,2340,2341,2342,2344,2345,2346,2347,2349,2350,2351,2352,2354,2355,2356,2357,2359,2360,2361,2362,2364,2365,2366,2367,2368,2370,2371,
                2372,2373,2374,2375,2377,2378,2379,2380,2381,2382,2384,2385,2386,2387,2388,2389,2390,2392,2393,2394,2395,2396,2397,2398,2399,2400,2401,2403,2404,2405,2406,2407,
                2408,2409,2410,2411,2412,2413,2414,2415,2416,2417,2418,2419,2420,2421,2422,2423,2424,2425,2426,2427,2428,2429,2430,2431,2432,2433,2434,2435,2436,2437,2438,2439,
                2440,2441,2442,2443,2443,2445,2446,2447,2448,2449,2450,2451,2452,2453,2454,2455,2456,2457,2458,2459,2460,2461,2462,2464,2465,2466,2467,2468,2469,2470,2471,2472,
                2473,2474,2475,2476,2477,2478,2479,2480,2481,2482,2483,2484,2485,2486,2487,2488,2489,2490,2491,2492,2493,2494,2495,2497,2498,2499,2500,2501,2502,2503,2504,2505,
                2506,2507,2508,2509,2510,2511,2512,2513,2514,2515,2516,2517,2518,2519,2520,2521,2522,2523,2524,2525,2526,2527,2528,2529,2530,2531,2532,2532,2533,2534,2535,2536,
                2537,2538,2539,2540,2541,2542,2543,2544,2545,2546,2547,2548,2549,2550,2551,2552,2553,2554,2555,2556,2557,2558,2559,2560,2561,2562,2563,2563,2564,2565,2566,2567,
                2568,2569,2570,2571,2572,2573,2574,2575,2576,2577,2578,2579,2580,2580,2581,2582,2583,2584,2585,2586,2587,2588,2589,2590,2591,2592,2593,2593,2594,2595,2596,2597,
                2598,2599,2600,2601,2602,2603,2604,2605,2605,2606,2607,2608,2609,2610,2611,2612,2613,2614,2615,2615,2616,2617,2618,2619,2620,2621,2622,2623,2623,2624,2625,2626,
                2627,2628,2629,2630,2631,2631,2632,2633,2634,2635,2636,2637,2638,2639,2639,2640,2641,2642,2643,2644,2645,2646,2646,2647,2648,2649,2650,2651,2652,2652,2653,2654,
                2655,2656,2657,2658,2658,2659,2660,2661,2662,2663,2664,2664,2665,2666,2667,2668,2669,2670,2670,2671,2672,2673,2674,2675,2675,2676,2677,2678,2679,2680,2680,2681,
                2682,2683,2684,2685,2685,2686,2687,2688,2689,2690,2690,2691,2692,2693,2694,2694,2695,2696,2697,2698,2699,2699,2700,2701,2702,2703,2703,2704,2705,2706,2707,2707,
                2708,2709,2710,2711,2711,2712,2713,2714,2715,2715,2716,2717,2718,2719,2719,2720,2721,2721,2722,2723,2724,2724,2725,2726,2727,2727,2728,2729,2730,2731,2731,2732,
                2733,2734,2734,2735,2736,2737,2737,2738,2739,2740,2740,2741,2742,2743,2744,2744,2745,2746,2747,2747,2748,2749,2750,2750,2751,2752,2753,2753,2754,2755,2756,2756,
                2757,2758,2759,2759,2760,2761,2762,2762,2763,2764,2765,2765,2766,2767,2768,2768,2769,2770,2771,2771,2772,2773,2774,2774,2775,2776,2777,2777,2778,2779,2779,2780,
                2781,2782,2782,2783,2784,2785,2785,2786,2787,2788,2788,2789,2790,2791,2791,2792,2793,2793,2794,2795,2796,2796,2797,2798,2799,2799,2800,2801,2801,2802,2803,2804,
                2804,2805,2806,2806,2807,2808,2809,2809,2810,2811,2811,2812,2813,2814,2814,2815,2816,2816,2817,2818,2819,2819,2820,2821,2821,2822,2823,2824,2824,2825,2826,2826,
                2827,2828,2829,2829,2830,2831,2831,2832,2833,2833,2834,2835,2836,2836,2837,2838,2838,2839,2840,2840,2841,2842,2843,2843,2844,2845,2845,2846,2847,2847,2848,2849,
                2850,2850,2851,2852,2852,2853,2854,2854,2855,2856,2856,2857,2858,2858,2859,2860,2860,2861,2862,2863,2863,2864,2865,2865,2866,2867,2867,2868,2869,2869,2870,2871,
                2871,2872,2873,2873,2874,2875,2875,2876,2877,2877,2878,2879,2879,2880,2881,2881,2882,2883,2883,2884,2885,2885,2886,2887,2887,2888,2889,2889,2890,2891,2891,2892,
                2893,2893,2894,2895,2895,2896,2897,2897,2898,2899,2899,2900,2901,2901,2902,2903,2903,2904,2905,2905,2906,2907,2907,2908,2908,2909,2910,2910,2911,2912,2912,2913,
                2914,2914,2915,2916,2916,2917,2917,2918,2919,2919,2920,2921,2921,2922,2923,2923,2924,2925,2925,2926,2926,2927,2928,2928,2929,2930,2930,2931,2930,2931,2931,2932,
                2932,2933,2933,2934,2934,2934,2935,2935,2936,2936,2937,2937,2937,2938,2938,2939,2939,2940,2940,2940,2941,2941,2942,2942,2943,2943,2943,2944,2944,2945,2945,2946,
                2946,2947,2947,2947,2948,2948,2949,2949,2950,2950,2950,2951,2951,2952,2952,2953,2953,2953,2954,2954,2955,2955,2956,2956,2956,2957,2957,2958,2958,2959,2959,2959,
                2960,2960,2961,2961,2962,2962,2963,2963,2963,2964,2964,2965,2965,2966,2966,2966,2967,2967,2968,2968,2969,2969,2969,2970,2970,2971,2971,2972,2972,2972,2973,2973,
                2974,2974,2975,2975,2975,2976,2976,2977,2977,2978,2978,2979,2979,2979,2980,2980,2981,2981,2982,2982,2982,2983,2983,2984,2984,2985,2985,2985,2986,2986,2987,2987,
                2988,2988,2988,2989,2989,2990,2990,2991,2991,2991,2992,2992,2993,2993,2994,2994,2995,2995,2995,2996,2996,2997,2997,2998,2998,2998,2999,2999,3000,3000,3001,3001,
                3001,3002,3002,3003,3003,3004,3004,3004,3005,3005,3006,3006,3007,3007,3007,3008,3008,3009,3009,3010,3010,3010,3011,3011,3012,3012,3013,3013,3014,3014,3014,3015,
                3015,3016,3016,3017,3017,3017,3018,3018,3019,3019,3020,3020,3020,3021,3021,3022,3022,3023,3023,3023,3024,3024,3025,3025,3026,3026,3026,3027,3027,3028,3028,3029,
                3029,3029,3030,3030,3031,3031,3032,3032,3033,3033,3033,3034,3034,3035,3035,3036,3036,3036,3037,3037,3038,3038,3039,3039,3039,3040,3040,3041,3041,3042,3042,3042,
                3043,3043,3044,3044,3045,3045,3045,3046,3046,3047,3047,3048,3048,3049,3049,3049,3050,3050,3051,3051,3052,3052,3052,3053,3053,3054,3054,3055,3055,3055,3056,3056,
                3057,3057,3058,3058,3058,3059,3059,3060,3060,3061,3061,3061,3062,3062,3063,3063,3064,3064,3064,3065,3065,3066,3066,3067,3067,3067,3068,3068,3069,3069,3070,3070,
                3071,3071,3071,3072,3072,3073,3073,3074,3074,3074,3075,3075,3076,3076,3077,3077,3077,3078,3078,3079,3079,3080,3080,3080,3081,3081,3082,3082,3083,3083,3083,3084,
                3084,3085,3085,3086,3086,3086,3087,3087,3088,3088,3089,3089,3090,3090,3090,3091,3091,3092,3092,3093,3093,3093,3094,3094,3095,3095,3096,3096,3096,3097,3097,3098,
                3098,3099,3099,3099,3100,3100,3101,3101,3102,3102,3102,3103,3103,3104,3104,3105,3105,3105,3106,3106,3107,3107,3108,3108,3108,3109,3109,3110,3110,3111,3111,3112,
                3112,3112,3113,3113,3114,3114,3115,3115,3115,3116,3116,3117,3117,3118,3118,3118,3119,3119,3120,3120,3121,3121,3121,3122,3122,3123,3123,3124,3124,3124,3125,3125,
                3126,3126,3127,3127,3127,3128,3128,3129,3129,3130,3130,3130,3131,3131,3132,3132,3133,3133,3133,3134,3134,3135,3135,3136,3136,3137,3137,3137,3138,3138,3139,3139,
                3140,3140,3140,3141,3141,3142,3142,3143,3143,3143,3144,3144,3145,3145,3146,3146,3146,3147,3147,3148,3148,3149,3149,3149,3150,3150,3151,3151,3152,3152,3152,3153,
                3153,3154,3154,3155,3155,3155,3156,3156,3157,3157,3158,3158,3158,3159,3159,3160,3160,3161,3161,3162,3162,3162,3163,3163,3164,3164,3165,3165,3165,3166,3166,3167,
                3167,3168,3168,3168,3169,3169,3170,3170,3171,3171,3171,3172,3172,3173,3173,3174,3174,3174,3175,3175,3176,3176,3177,3177,3177,3178,3178,3179,3179,3180,3180,3180,
                3181,3181,3182,3182,3183,3183,3183,3184,3184,3185,3185,3186,3186,3186,3187,3187,3188,3188,3189,3189,3190,3190,3190,3190,3191,3191,3191,3192,3192,3192,3193,3193,
                3193,3194,3194,3194,3195,3195,3195,3195,3196,3196,3196,3197,3197,3197,3198,3198,3198,3199,3199,3199,3200,3200,3200,3201,3201,3201,3202,3202,3202,3202,3203,3203,
                3203,3204,3204,3204,3205,3205,3205,3206,3206,3206,3207,3207,3207,3208,3208,3208,3209,3209,3209,3209,3210,3210,3210,3211,3211,3211,3212,3212,3212,3213,3213,3213,
                3214,3214,3214,3215,3215,3215,3216,3216,3216,3217,3217,3217,3217,3218,3218,3218,3219,3219,3219,3220,3220,3220,3221,3221,3221,3222,3222,3222,3223,3223,3223,3224,
                3224,3224,3225,3225,3225,3226,3226,3226,3227,3227,3227,3228,3228,3228,3229,3229,3229,3229,3230,3230,3230,3231,3231,3231,3232,3232,3232,3233,3233,3233,3234,3234,
                3234,3235,3235,3235,3236,3236,3236,3237,3237,3237,3238,3238,3238,3239,3239,3239,3240,3240,3240,3241,3241,3241,3242,3242,3242,3243,3243,3243,3244,3244,3244,3245,
                3245,3245,3246,3246,3246,3247,3247,3247,3248,3248,3248,3249,3249,3249,3250,3250,3250,3251,3251,3251,3251,3252,3252,3252,3253,3253,3253,3254,3254,3254,3255,3255,
                3255,3256,3256,3256,3257,3257,3257,3258,3258,3258,3259,3259,3259,3260,3260,3260,3261,3261,3261,3262,3262,3262,3263,3263,3263,3264,3264,3265,3265,3265,3266,3266,
                3266,3267,3267,3267,3268,3268,3268,3269,3269,3269,3270,3270,3270,3271,3271,3271,3272,3272,3272,3273,3273,3273,3274,3274,3274,3275,3275,3275,3276,3276,3276,3277,
                3277,3277,3278,3278,3278,3279,3279,3279,3280,3280,3280,3281,3281,3281,3282,3282,3282,3283,3283,3283,3284,3284,3284,3285,3285,3285,3286,3286,3286,3287,3287,3288,
                3288,3288,3289,3289,3289,3290,3290,3290,3291,3291,3291,3292,3292,3292,3293,3293,3293,3294,3294,3294,3295,3295,3295,3296,3296,3296,3297,3297,3297,3298,3298,3299,
                3299,3299,3300,3300,3300,3301,3301,3301,3302,3302,3302,3303,3303,3303,3304,3304,3304,3305,3305,3305,3306,3306,3306,3307,3307,3307,3308,3308,3309,3309,3309,3310,
                3310,3310,3311,3311,3311,3312,3312,3312,3313,3313,3313,3314,3314,3314,3315,3315,3315,3316,3316,3317,3317,3317,3318,3318,3318,3319,3319,3319,3320,3320,3320,3321,
                3321,3321,3322,3322,3322,3323,3323,3324,3324,3325,3325,3325,3326,3326,3327,3327,3328,3328,3328,3329,3329,3330,3330,3330,3331,3331,3332,3332,3333,3333,3333,3334,
                3334,3335,3335,3336,3336,3336,3337,3337,3338,3338,3338,3339,3339,3340,3340,3341,3341,3341,3342,3342,3343,3343,3344,3344,3344,3345,3345,3346,3346,3347,3347,3347,
                3348,3348,3349,3349,3349,3350,3350,3351,3351,3352,3352,3352,3353,3353,3354,3354,3355,3355,3355,3356,3356,3357,3357,3358,3358,3358,3359,3359,3360,3360,3360,3361,
                3361,3362,3362,3363,3363,3363,3364,3364,3365,3365,3366,3366,3366,3367,3367,3368,3368,3369,3369,3369,3370,3370,3371,3371,3371,3372,3372,3373,3373,3374,3374,3374,
                3375,3375,3376,3376,3377,3377,3377,3378,3378,3379,3379,3380,3380,3380,3381,3381,3382,3382,3383,3383,3383,3384,3384,3385,3385,3386,3386,3386,3387,3387,3388,3388,
                3389,3389,3389,3390,3390,3391,3391,3391,3392,3392,3393,3393,3394,3394,3394,3395,3395,3396,3396,3397,3397,3397,3398,3398,3399,3399,3400,3400,3400,3401,3401,3402,
                3402,3403,3403,3403,3404,3404,3405,3405,3406,3406,3406,3407,3407,3408,3408,3409,3409,3409,3410,3410,3411,3411,3412,3412,3412,3413,3413,3414,3414,3415,3415,3415,
                3416,3416,3417,3417,3418,3418,3418,3419,3419,3420,3420,3421,3421,3421,3422,3422,3423,3423,3424,3424,3424,3425,3425,3426,3426,3427,3427,3427,3428,3428,3429,3429,
                3430,3430,3430,3431,3431,3432,3432,3433,3433,3433,3434,3434,3435,3435,3436,3436,3436,3437,3437,3438,3438,3439,3439,3440,3440,3440,3441,3441,3442,3442,3443,3443,
                3443,3444,3444,3445,3445,3446,3446,3446,3447,3447,3448,3448,3449,3449,3449,3450,3450,3451,3451,3452,3452,3452,3453,3453,3454,3454,3455,3455,3455,3456,3456,3457,
                3457,3458,3458,3459,3459,3459,3460,3460,3461,3461,3462,3462,3462,3463,3463,3464,3464,3465,3465,3465,3466,3466,3467,3467,3468,3468,3468,3469,3469,3470,3470,3471,
                3471,3472,3472,3472,3473,3473,3474,3474,3475,3475,3475,3476,3476,3477,3477,3478,3478,3478,3479,3479,3480,3480,3481,3481,3481,3482,3482,3483,3483,3484,3484,3485,
                3485,3485,3486,3486,3487,3487,3488,3488,3488,3489,3489,3490,3490,3491,3491,3492,3492,3492,3493,3493,3494,3494,3494,3495,3495,3495,3496,3496,3496,3497,3497,3498,
                3498,3498,3499,3499,3499,3500,3500,3501,3501,3501,3502,3502,3502,3503,3503,3504,3504,3504,3505,3505,3505,3506,3506,3507,3507,3507,3508,3508,3508,3509,3509,3510,
                3510,3510,3511,3511,3511,3512,3512,3513,3513,3513,3514,3514,3514,3515,3515,3516,3516,3516,3517,3517,3517,3518,3518,3519,3519,3519,3520,3520,3521,3521,3521,3522,
                3522,3522,3523,3523,3524,3524,3524,3525,3525,3525,3526,3526,3527,3527,3527,3528,3528,3529,3529,3529,3530,3530,3530,3531,3531,3532,3532,3532,3533,3533,3534,3534,
                3534,3535,3535,3536,3536,3536,3537,3537,3537,3538,3538,3539,3539,3539,3540,3540,3541,3541,3541,3542,3542,3543,3543,3543,3544,3544,3545,3545,3545,3546,3546,3546,
                3547,3547,3548,3548,3548,3549,3549,3550,3550,3550,3551,3551,3552,3552,3552,3553,3553,3554,3554,3554,3555,3555,3556,3556,3556,3557,3557,3558,3558,3558,3559,3559,
                3560,3560,3560,3561,3561,3562,3562,3562,3563,3563,3564,3564,3564,3565,3565,3566,3566,3566,3567,3567,3568,3568,3568,3569,3569,3570,3570,3570,3571,3571,3572,3572,
                3573,3573,3573,3574,3574,3575,3575,3575,3576,3576,3577,3577,3577,3578,3578,3579,3579,3579,3580,3580,3581,3581,3581,3582,3582,3583,3583,3584,3584,3584,3585,3585,
                3586,3586,3586,3587,3587,3588,3588,3589,3589,3589,3590,3590,3591,3591,3591,3592,3592,3593,3593,3593,3594,3594,3595,3595,3596,3596,3596,3597,3597,3598,3598,3598,
                3599,3599,3600,3600,3601,3601,3601,3602,3602,3603,3603,3604,3604,3604,3605,3605,3606,3606,3606,3607,3607,3608,3608,3609,3609,3609,3610,3610,3611,3611,3612,3612,
                3612,3613,3613,3614,3614,3615,3615,3615,3616,3616,3617,3617,3617,3618,3618,3619,3619,3620,3620,3620,3621,3621,3622,3622,3623,3623,3623,3624,3624,3625,3625,3626,
                3626,3626,3627,3627,3628,3628,3629,3629,3629,3630,3630,3631,3631,3632,3632,3633,3633,3633,3634,3634,3635,3635,3636,3636,3636,3637,3637,3638,3638,3639,3639,3639,
                3640,3640,3641,3641,3642,3642,3643,3643,3643,3644,3644,3645,3645,3646,3646,3646,3647,3647,3648,3648,3649,3649,3650,3650,3650,3651,3651,3652,3652,3653,3653,3654,
                3654,3654,3655,3655,3656,3656,3657,3657,3658,3658,3658,3659,3659,3660,3660,3661,3661,3662,3662,3662,3663,3663,3664,3664,3665,3665,3666,3666,3666,3667,3667,3668,
                3668,3669,3669,3670,3670,3670,3671,3671,3672,3672,3673,3673,3674,3674,3675,3675,3675,3676,3676,3677,3677,3678,3678,3679,3679,3679,3680,3680,3681,3681,3682,3682,
                3683,3683,3684,3684,3684,3685,3685,3686,3686,3687,3687,3688,3688,3689,3689,3689,3690,3690,3691,3691,3692,3692,3693,3693,3694,3694,3695,3695,3695,3696,3696,3697,
                3697,3698,3698,3699,3699,3700,3700,3700,3701,3701,3702,3702,3703,3703,3704,3704,3705,3705,3706,3706,3707,3707,3707,3708,3708,3709,3709,3710,3710,3711,3711,3712,
                3712,3713,3713,3713,3714,3714,3715,3715,3716,3716,3717,3717,3718,3718,3719,3719,3720,3720,3721,3721,3721,3722,3722,3723,3723,3724,3724,3725,3725,3726,3726,3727,
                3727,3728,3728,3729,3729,3729,3730,3730,3731,3731,3732,3732,3733,3733,3734,3734,3735,3735,3736,3736,3737,3737,3738,3738,3738,3739,3739,3740,3740,3741,3741,3742,
                3742,3743,3743,3744,3744,3745,3745,3746,3746,3747,3747,3748,3748,3749,3749,3749,3750,3750,3751,3751,3752,3752,3753,3753,3754,3754,3755,3755,3756,3756,3757,3757,
                3758,3758,3759,3759,3760,3760,3761,3761,3762,3762,3763,3763,3764,3764,3764,3765,3765,3766,3766,3767,3767,3768,3768,3769,3769,3770,3770,3771,3771,3772,3772,3773,
                3773,3774,3774,3775,3775,3776,3776,3777,3777,3778,3778,3779,3779,3780,3780,3781,3781,3782,3782,3783,3783,3784,3784,3785,3785,3786,3786,3787,3787,3788,3788,3789,
                3789,3790,3790,3791,3791,3792,3792,3793,3793,3794,3794,3795,3795,3796,3796,3797,3797,3797,3798,3798,3799,3799,3800,3800,3801,3801,3802,3802,3803,3804,3804,3805,
                3805,3806,3806,3807,3807,3808,3808,3809,3809,3810,3810,3811,3811,3812,3812,3813,3813,3814,3814,3815,3815,3816,3816,3817,3817,3818,3818,3819,3819,3820,3820,3821,
                3821,3822,3822,3823,3823,3824,3824,3825,3825,3826,3826,3827,3827,3828,3828,3829,3829,3830,3830,3831,3831,3832,3832,3833,3833,3834,3834,3835,3835,3836,3836,3837,
                3838,3838,3839,3839,3840,3840,3841,3841,3842,3842,3843,3843,3844,3844,3845,3845,3846,3846,3847,3847,3848,3848,3849,3849,3850,3850,3851,3851,3852,3852,3853,3854,
                3854,3855,3855,3856,3856,3857,3857,3858,3858,3859,3859,3860,3860,3861,3861,3862,3862,3863,3863,3864,3864,3865,3866,3866,3867,3867,3868,3868,3869,3869,3870,3870,
                3871,3871,3872,3872,3873,3873,3874,3874,3875,3876,3876,3877,3877,3878,3878,3879,3879,3880,3880,3881,3881,3882,3882,3883,3883,3884,3884,3885,3886,3886,3887,3887,
                3888,3888,3889,3889,3890,3890,3891,3891,3892,3892,3893,3894,3894,3895,3895,3896,3896,3897,3897,3898,3898,3899,3899,3900,3901,3901,3902,3902,3903,3903,3904,3905,
                3905,3906,3906,3907,3907,3908,3909,3909,3910,3910,3911,3911,3912,3913,3913,3914,3914,3915,3916,3916,3917,3917,3918,3918,3919,3920,3920,3921,3921,3922,3923,3923,
                3924,3924,3925,3925,3926,3927,3927,3928,3928,3929,3930,3930,3931,3931,3932,3933,3933,3934,3934,3935,3936,3936,3937,3937,3938,3939,3939,3940,3940,3941,3942,3942,
                3943,3943,3944,3945,3945,3946,3946,3947,3948,3948,3949,3950,3950,3951,3951,3952,3953,3953,3954,3954,3955,3956,3956,3957,3958,3958,3959,3959,3960,3961,3961,3962,
                3963,3963,3964,3964,3965,3966,3966,3967,3968,3968,3969,3969,3970,3971,3971,3972,3973,3973,3974,3974,3975,3976,3976,3977,3978,3978,3979,3980,3980,3981,3981,3982,
                3983,3983,3984,3985,3985,3986,3987,3987,3988,3989,3989,3990,3990,3991,3992,3992,3993,3994,3994,3995,3996,3996,3997,3998,3998,3999,4000,4000,4001,4002,4002,4003,
                4004,4004,4005,4006,4006,4007,4008,4008,4009,4010,4010,4011,4011,4012,4013,4013,4014,4015,4015,4016,4017,4017,4018,4019,4020,4020,4021,4022,4022,4023,4024,4024,
                4025,4026,4026,4027,4028,4028,4029,4030,4030,4031,4032,4032,4033,4034,4034,4035,4036,4036,4037,4038,4039,4039,4040,4041,4041,4042,4043,4043,4044,4045,4045,4046,
                4047,4047,4048,4049,4050,4050,4051,4052,4052,4053,4054,4054,4055,4056,4057,4057,4058,4059,4059,4060,4061,4061,4062,4063,4064,4064,4065,4066,4066,4067,4068,4069,
                4069,4070,4071,4071,4072,4073,4074,4074,4075,4076,4076,4077,4078,4079,4079,4080,4081,4081,4082,4083,4084,4084,4085,4086,4086,4087,4088,4089,4089,4090,4091,4092,
          },
          /* aec trigger 2 */
          {
                   0,   0,   1,   2,   4,   5,   6,   8,  10,  11,  13,  15,  17,  20,  22,  25,  27,  30,  33,  36,  39,  42,  45,  49,  52,  56,  59,  63,  67,  71,  72,  78,
                  83,  88,  94,  99, 104, 110, 115, 121, 126, 132, 138, 143, 149, 155, 161, 167, 173, 179, 185, 191, 197, 203, 209, 216, 222, 228, 234, 241, 247, 254, 260, 267,
                 274, 280, 287, 294, 301, 307, 314, 321, 328, 335, 342, 350, 357, 364, 371, 378, 386, 393, 401, 408, 416, 423, 431, 438, 446, 454, 462, 467, 472, 477, 482, 487,
                 492, 497, 502, 507, 512, 518, 523, 528, 533, 538, 543, 548, 553, 558, 562, 567, 572, 577, 582, 587, 592, 597, 602, 607, 611, 616, 621, 626, 631, 636, 640, 645,
                 650, 655, 659, 664, 669, 674, 678, 683, 688, 693, 697, 702, 706, 711, 716, 720, 725, 730, 734, 739, 743, 748, 752, 757, 762, 766, 771, 775, 780, 784, 789, 793,
                 797, 800, 803, 806, 809, 812, 816, 819, 822, 825, 828, 831, 835, 838, 841, 844, 847, 850, 853, 856, 859, 862, 865, 868, 871, 874, 877, 880, 883, 886, 889, 892,
                 895, 898, 901, 904, 907, 910, 913, 916, 919, 922, 925, 928, 930, 933, 936, 939, 942, 945, 948, 950, 953, 956, 959, 962, 964, 967, 970, 973, 975, 978, 981, 984,
                 986, 989, 992, 994, 997,1000,1003,1005,1008,1011,1013,1016,1018,1021,1024,1026,1029,1031,1034,1037,1039,1042,1044,1047,1049,1052,1054,1057,1059,1062,1064,1067,
                1069,1072,1074,1077,1079,1082,1084,1086,1089,1091,1094,1096,1098,1101,1103,1106,1108,1110,1113,1115,1117,1120,1122,1124,1126,1129,1131,1133,1135,1138,1140,1142,
                1144,1147,1149,1151,1153,1155,1158,1160,1162,1164,1166,1168,1171,1174,1176,1179,1181,1184,1186,1189,1191,1194,1196,1198,1201,1203,1206,1208,1211,1213,1215,1218,
                1220,1223,1225,1227,1230,1232,1234,1237,1239,1241,1244,1246,1248,1251,1253,1255,1258,1260,1262,1265,1267,1269,1272,1274,1276,1278,1281,1283,1285,1287,1290,1292,
                1294,1296,1299,1301,1303,1305,1307,1310,1312,1314,1316,1319,1321,1323,1325,1327,1329,1332,1334,1336,1338,1340,1342,1344,1347,1349,1351,1353,1355,1357,1359,1361,
                1364,1366,1368,1370,1372,1374,1376,1378,1380,1382,1384,1386,1388,1390,1392,1394,1395,1398,1400,1402,1404,1406,1408,1410,1412,1414,1417,1419,1421,1423,1425,1427,
                1429,1431,1433,1435,1437,1439,1442,1444,1446,1448,1450,1452,1454,1456,1458,1460,1462,1464,1466,1468,1470,1472,1474,1476,1478,1480,1482,1484,1486,1488,1490,1492,
                1494,1496,1498,1500,1502,1504,1506,1508,1510,1512,1514,1516,1518,1520,1522,1524,1526,1528,1530,1532,1534,1536,1538,1540,1542,1544,1546,1548,1550,1551,1553,1555,
                1557,1559,1561,1563,1565,1567,1569,1571,1573,1574,1576,1578,1580,1582,1584,1586,1588,1590,1592,1593,1595,1597,1599,1601,1603,1605,1606,1608,1610,1612,1614,1616,
                1618,1619,1621,1623,1625,1627,1629,1631,1631,1633,1635,1637,1639,1641,1643,1645,1647,1649,1651,1653,1654,1656,1658,1660,1662,1664,1666,1668,1670,1672,1674,1675,
                1677,1679,1681,1683,1685,1687,1689,1691,1692,1694,1696,1698,1700,1702,1704,1706,1708,1709,1711,1713,1715,1717,1719,1721,1723,1724,1726,1728,1730,1732,1734,1736,
                1738,1739,1741,1743,1745,1747,1749,1751,1752,1754,1756,1758,1760,1762,1764,1766,1767,1769,1771,1773,1775,1777,1778,1780,1782,1784,1786,1788,1790,1791,1793,1795,
                1797,1799,1801,1802,1804,1806,1808,1810,1812,1813,1815,1817,1819,1821,1823,1824,1826,1828,1830,1832,1834,1835,1837,1839,1841,1843,1845,1846,1848,1850,1852,1854,
                1855,1857,1859,1861,1863,1864,1866,1868,1870,1872,1873,1875,1877,1879,1881,1882,1884,1886,1888,1890,1891,1893,1895,1897,1899,1900,1902,1904,1906,1908,1909,1911,
                1913,1915,1917,1918,1920,1922,1924,1925,1927,1929,1931,1933,1934,1936,1938,1940,1941,1943,1945,1947,1948,1950,1952,1954,1955,1957,1959,1961,1963,1964,1966,1968,
                1970,1971,1973,1975,1977,1978,1980,1982,1984,1985,1987,1989,1991,1992,1994,1996,1998,1999,2001,2003,2005,2006,2008,2010,2011,2013,2015,2017,2018,2020,2022,2024,
                2025,2027,2029,2030,2032,2034,2036,2037,2039,2041,2043,2044,2046,2048,2049,2051,2053,2055,2056,2058,2060,2061,2063,2065,2067,2068,2070,2071,2073,2075,2076,2078,
                2080,2081,2083,2085,2086,2088,2089,2091,2093,2094,2096,2098,2099,2101,2102,2104,2106,2107,2109,2110,2112,2114,2115,2117,2119,2120,2122,2123,2125,2126,2128,2130,
                2131,2133,2134,2136,2138,2139,2141,2142,2144,2145,2147,2149,2150,2152,2153,2155,2156,2158,2160,2161,2163,2164,2166,2167,2169,2170,2172,2173,2175,2177,2178,2180,
                2181,2183,2184,2186,2187,2189,2190,2192,2193,2195,2196,2198,2199,2201,2202,2204,2205,2207,2208,2210,2211,2213,2214,2216,2217,2219,2220,2222,2223,2225,2226,2228,
                2229,2231,2232,2234,2235,2237,2238,2240,2241,2243,2244,2245,2247,2248,2250,2251,2253,2254,2256,2257,2259,2260,2261,2263,2264,2266,2267,2269,2270,2271,2273,2274,
                2276,2277,2279,2280,2280,2282,2284,2285,2287,2288,2290,2291,2293,2294,2296,2297,2299,2300,2302,2303,2305,2306,2308,2309,2311,2312,2314,2315,2317,2318,2320,2321,
                2323,2324,2326,2327,2329,2330,2332,2333,2335,2336,2338,2339,2341,2342,2344,2345,2347,2348,2350,2351,2353,2354,2356,2357,2359,2360,2361,2363,2364,2366,2367,2369,
                2370,2372,2373,2375,2376,2377,2379,2380,2382,2383,2385,2386,2388,2389,2390,2392,2393,2395,2396,2398,2399,2400,2402,2403,2405,2406,2408,2409,2410,2412,2413,2415,
                2416,2417,2419,2420,2422,2423,2424,2426,2427,2429,2430,2431,2433,2434,2436,2437,2438,2440,2441,2442,2444,2445,2447,2448,2449,2451,2452,2453,2455,2456,2458,2459,
                2460,2462,2463,2464,2466,2467,2468,2470,2471,2473,2474,2475,2477,2478,2479,2481,2482,2483,2485,2486,2487,2489,2490,2491,2493,2494,2495,2497,2498,2499,2501,2502,
                2503,2505,2506,2507,2508,2510,2511,2512,2514,2515,2516,2518,2519,2520,2522,2523,2524,2525,2527,2528,2529,2531,2532,2533,2534,2536,2537,2538,2540,2541,2542,2543,
                2545,2546,2547,2548,2550,2551,2552,2554,2555,2556,2557,2559,2560,2561,2562,2564,2565,2566,2567,2569,2570,2571,2572,2574,2575,2576,2577,2579,2580,2581,2582,2583,
                2585,2586,2587,2588,2590,2591,2592,2593,2594,2596,2597,2598,2599,2601,2602,2603,2604,2605,2607,2608,2609,2610,2611,2613,2614,2615,2616,2617,2618,2620,2621,2622,
                2623,2624,2626,2627,2628,2629,2630,2631,2633,2634,2635,2636,2637,2638,2640,2641,2642,2643,2644,2645,2647,2648,2649,2650,2651,2652,2654,2655,2656,2657,2658,2659,
                2660,2661,2663,2664,2665,2666,2667,2668,2669,2671,2672,2673,2674,2675,2676,2677,2678,2679,2680,2681,2682,2683,2685,2686,2687,2688,2689,2690,2692,2693,2694,2695,
                2696,2697,2699,2700,2701,2702,2703,2704,2705,2707,2708,2709,2710,2711,2712,2713,2715,2716,2717,2718,2719,2720,2721,2723,2724,2725,2726,2727,2728,2729,2730,2732,
                2733,2734,2735,2736,2737,2738,2739,2740,2742,2743,2744,2745,2746,2747,2748,2749,2750,2751,2753,2754,2755,2756,2757,2758,2759,2760,2761,2762,2764,2765,2766,2767,
                2768,2769,2770,2771,2772,2773,2774,2775,2776,2778,2779,2780,2781,2782,2783,2784,2785,2786,2787,2788,2789,2790,2791,2792,2793,2794,2796,2797,2798,2799,2800,2801,
                2802,2803,2804,2805,2806,2807,2808,2809,2810,2811,2812,2813,2814,2815,2816,2817,2818,2819,2820,2821,2822,2823,2824,2825,2826,2827,2828,2829,2830,2831,2832,2833,
                2834,2835,2836,2837,2838,2839,2840,2841,2842,2843,2844,2845,2846,2847,2848,2849,2850,2851,2852,2853,2854,2855,2856,2857,2858,2859,2860,2861,2862,2863,2864,2865,
                2866,2867,2868,2869,2870,2871,2872,2873,2874,2874,2875,2876,2877,2878,2879,2880,2881,2882,2883,2884,2885,2886,2887,2888,2889,2890,2890,2891,2892,2893,2894,2895,
                2896,2897,2898,2899,2900,2901,2902,2902,2903,2904,2905,2906,2907,2908,2909,2910,2911,2912,2912,2913,2914,2915,2916,2917,2918,2919,2920,2921,2921,2922,2923,2924,
                2925,2926,2927,2928,2928,2929,2930,2931,2932,2933,2934,2935,2935,2936,2937,2938,2939,2940,2941,2942,2942,2943,2944,2945,2946,2947,2948,2948,2949,2950,2951,2952,
                2953,2953,2954,2955,2956,2957,2958,2958,2959,2960,2961,2962,2963,2963,2964,2965,2966,2967,2968,2968,2969,2970,2971,2972,2973,2973,2974,2975,2975,2976,2977,2978,
                2979,2980,2981,2982,2983,2984,2985,2986,2987,2988,2989,2990,2991,2992,2992,2993,2994,2995,2996,2997,2998,2999,3000,3001,3002,3003,3004,3005,3005,3006,3007,3008,
                3009,3010,3011,3012,3013,3014,3015,3016,3016,3017,3018,3019,3020,3021,3022,3023,3024,3025,3026,3026,3027,3028,3029,3030,3031,3032,3033,3034,3034,3035,3036,3037,
                3038,3039,3040,3041,3042,3043,3043,3044,3045,3046,3047,3048,3049,3050,3050,3051,3052,3053,3054,3055,3056,3057,3057,3058,3059,3060,3061,3062,3063,3064,3064,3065,
                3066,3067,3068,3069,3070,3070,3071,3072,3073,3074,3075,3076,3076,3077,3078,3079,3080,3081,3082,3082,3083,3084,3085,3086,3087,3088,3088,3089,3090,3091,3092,3093,
                3093,3094,3095,3096,3097,3098,3098,3099,3100,3101,3102,3103,3103,3104,3105,3106,3107,3107,3108,3109,3110,3111,3112,3112,3113,3114,3115,3116,3116,3117,3118,3119,
                3120,3121,3121,3122,3123,3124,3125,3125,3126,3127,3128,3129,3129,3130,3131,3132,3133,3133,3134,3135,3136,3137,3137,3138,3139,3140,3141,3141,3142,3143,3144,3144,
                3145,3146,3147,3148,3148,3149,3150,3151,3151,3152,3153,3154,3155,3155,3156,3157,3158,3158,3159,3160,3161,3161,3162,3163,3164,3165,3165,3166,3167,3168,3168,3169,
                3170,3171,3171,3172,3173,3174,3174,3175,3176,3177,3177,3178,3179,3180,3180,3181,3182,3183,3183,3184,3185,3186,3186,3187,3188,3188,3189,3190,3191,3191,3192,3193,
                3194,3194,3195,3196,3197,3197,3198,3199,3199,3200,3201,3202,3202,3203,3204,3204,3205,3206,3207,3207,3208,3209,3209,3210,3211,3212,3212,3213,3214,3214,3215,3216,
                3217,3217,3218,3219,3219,3220,3221,3221,3222,3223,3223,3224,3225,3226,3226,3227,3228,3228,3229,3230,3230,3231,3232,3232,3233,3234,3234,3235,3236,3236,3237,3238,
                3239,3239,3240,3241,3241,3242,3243,3243,3244,3245,3245,3246,3247,3247,3248,3249,3249,3250,3251,3251,3252,3253,3253,3254,3254,3255,3256,3256,3257,3258,3258,3259,
                3260,3260,3261,3262,3262,3263,3264,3264,3265,3265,3266,3267,3267,3268,3269,3269,3270,3271,3271,3272,3272,3273,3274,3274,3275,3276,3276,3277,3277,3278,3279,3279,
                3280,3281,3281,3282,3282,3283,3284,3284,3285,3286,3286,3287,3287,3288,3289,3289,3290,3290,3291,3292,3292,3293,3293,3294,3295,3295,3296,3296,3297,3298,3298,3299,
                3299,3300,3301,3301,3302,3302,3303,3304,3304,3305,3305,3306,3306,3307,3308,3308,3309,3309,3310,3311,3311,3312,3312,3313,3313,3314,3315,3315,3316,3316,3317,3317,
                3318,3319,3319,3320,3320,3321,3321,3322,3322,3323,3324,3324,3325,3325,3326,3326,3327,3327,3328,3329,3329,3330,3330,3331,3331,3332,3332,3333,3333,3334,3335,3335,
                3336,3336,3337,3337,3338,3338,3339,3339,3340,3340,3341,3342,3342,3343,3343,3344,3344,3345,3345,3346,3346,3347,3347,3348,3348,3349,3349,3350,3350,3351,3351,3352,
                3352,3353,3353,3354,3355,3355,3356,3356,3357,3357,3358,3358,3359,3359,3360,3360,3361,3361,3362,3362,3363,3363,3364,3364,3365,3365,3365,3366,3366,3367,3367,3368,
                3368,3369,3369,3370,3370,3371,3371,3372,3372,3373,3373,3374,3374,3375,3375,3376,3376,3377,3377,3377,3378,3378,3379,3379,3380,3380,3381,3381,3382,3382,3383,3383,
                3384,3384,3384,3385,3385,3386,3386,3387,3387,3388,3388,3388,3389,3389,3390,3390,3391,3391,3392,3392,3392,3393,3393,3393,3394,3394,3394,3395,3395,3395,3396,3396,
                3396,3397,3397,3397,3398,3398,3398,3399,3399,3399,3400,3400,3400,3401,3401,3401,3402,3402,3402,3403,3403,3403,3404,3404,3404,3405,3405,3405,3406,3406,3406,3407,
                3407,3407,3408,3408,3408,3409,3409,3409,3410,3410,3410,3411,3411,3411,3412,3412,3412,3413,3413,3413,3414,3414,3415,3415,3415,3416,3416,3416,3417,3417,3417,3418,
                3418,3418,3419,3419,3419,3420,3420,3420,3421,3421,3421,3422,3422,3422,3423,3423,3423,3424,3424,3424,3425,3425,3425,3426,3426,3426,3427,3427,3427,3428,3428,3428,
                3429,3429,3429,3430,3430,3430,3431,3431,3431,3432,3432,3432,3433,3433,3433,3434,3434,3434,3435,3435,3435,3436,3436,3436,3437,3437,3437,3438,3438,3438,3439,3439,
                3439,3440,3440,3440,3441,3441,3442,3442,3442,3443,3443,3443,3444,3444,3444,3445,3445,3445,3446,3446,3446,3447,3447,3447,3448,3448,3448,3449,3449,3449,3450,3450,
                3450,3451,3451,3451,3452,3452,3452,3453,3453,3453,3454,3454,3454,3455,3455,3456,3456,3456,3457,3457,3457,3458,3458,3458,3459,3459,3459,3460,3460,3460,3461,3461,
                3461,3462,3462,3462,3463,3463,3463,3464,3464,3464,3465,3465,3465,3466,3466,3466,3467,3467,3468,3468,3468,3469,3469,3469,3470,3470,3470,3471,3471,3471,3472,3472,
                3472,3473,3473,3473,3474,3474,3474,3475,3475,3475,3476,3476,3477,3477,3477,3478,3478,3478,3479,3479,3479,3480,3480,3480,3481,3481,3481,3482,3482,3482,3483,3483,
                3483,3484,3484,3484,3485,3485,3486,3486,3486,3487,3487,3487,3488,3488,3488,3489,3489,3489,3490,3490,3490,3491,3491,3491,3492,3492,3492,3493,3493,3494,3494,3494,
                3495,3495,3495,3496,3496,3496,3497,3497,3497,3498,3498,3498,3499,3499,3499,3500,3500,3501,3501,3501,3502,3502,3502,3503,3503,3503,3504,3504,3504,3505,3505,3505,
                3506,3506,3506,3507,3507,3508,3508,3508,3509,3509,3509,3510,3510,3510,3511,3511,3511,3512,3512,3512,3513,3513,3514,3514,3514,3515,3515,3515,3516,3516,3516,3517,
                3517,3517,3518,3518,3518,3519,3519,3520,3520,3520,3521,3521,3521,3522,3522,3522,3523,3523,3523,3524,3524,3524,3525,3525,3526,3526,3526,3527,3527,3527,3528,3528,
                3528,3529,3529,3529,3529,3529,3529,3530,3530,3530,3531,3531,3531,3532,3532,3533,3533,3533,3534,3534,3534,3535,3535,3536,3536,3536,3537,3537,3537,3538,3538,3538,
                3539,3539,3540,3540,3540,3541,3541,3541,3542,3542,3543,3543,3543,3544,3544,3544,3545,3545,3545,3546,3546,3547,3547,3547,3548,3548,3548,3549,3549,3549,3550,3550,
                3551,3551,3551,3552,3552,3552,3553,3553,3553,3554,3554,3555,3555,3555,3556,3556,3556,3557,3557,3557,3558,3558,3559,3559,3559,3560,3560,3560,3561,3561,3561,3562,
                3562,3562,3563,3563,3564,3564,3564,3565,3565,3565,3566,3566,3566,3567,3567,3568,3568,3568,3569,3569,3569,3570,3570,3570,3571,3571,3571,3572,3572,3573,3573,3573,
                3574,3574,3574,3575,3575,3575,3576,3576,3576,3577,3577,3577,3578,3578,3579,3579,3579,3580,3580,3580,3581,3581,3581,3582,3582,3582,3583,3583,3583,3584,3584,3585,
                3585,3585,3586,3586,3586,3587,3587,3587,3588,3588,3588,3589,3589,3589,3590,3590,3590,3591,3591,3592,3592,3592,3593,3593,3593,3594,3594,3594,3595,3595,3595,3596,
                3596,3596,3597,3597,3597,3598,3598,3598,3599,3599,3599,3600,3600,3601,3601,3601,3602,3602,3602,3603,3603,3603,3604,3604,3604,3605,3605,3605,3606,3606,3606,3607,
                3607,3607,3608,3608,3608,3609,3609,3609,3610,3610,3610,3611,3611,3611,3612,3612,3612,3613,3613,3614,3614,3614,3615,3615,3615,3616,3616,3616,3617,3617,3617,3618,
                3618,3618,3619,3619,3619,3620,3620,3620,3621,3621,3621,3622,3622,3622,3623,3623,3623,3624,3624,3624,3625,3625,3625,3626,3626,3626,3627,3627,3627,3628,3628,3628,
                3629,3629,3629,3630,3630,3630,3631,3631,3631,3632,3632,3632,3633,3633,3633,3634,3634,3634,3635,3635,3635,3636,3636,3636,3637,3637,3637,3638,3638,3638,3639,3639,
                3639,3640,3640,3640,3641,3641,3641,3642,3642,3642,3643,3643,3643,3644,3644,3644,3645,3645,3645,3645,3646,3646,3646,3647,3647,3647,3648,3648,3648,3649,3649,3649,
                3650,3650,3650,3651,3651,3651,3652,3652,3652,3653,3653,3653,3654,3654,3654,3655,3655,3655,3656,3656,3656,3657,3657,3657,3657,3658,3658,3658,3659,3659,3659,3660,
                3660,3660,3661,3661,3661,3662,3662,3662,3663,3663,3663,3664,3664,3664,3665,3665,3665,3665,3666,3666,3666,3667,3667,3667,3668,3668,3668,3669,3669,3669,3670,3670,
                3670,3671,3671,3671,3671,3672,3672,3672,3673,3673,3673,3674,3674,3674,3675,3675,3675,3676,3676,3676,3677,3677,3677,3677,3678,3678,3678,3679,3679,3679,3680,3680,
                3680,3681,3681,3681,3682,3682,3682,3682,3683,3683,3683,3684,3684,3684,3685,3685,3685,3686,3686,3686,3686,3687,3687,3687,3688,3688,3688,3689,3689,3689,3690,3690,
                3690,3691,3691,3691,3691,3692,3692,3692,3693,3693,3693,3694,3694,3694,3694,3695,3695,3695,3696,3696,3696,3697,3697,3697,3698,3698,3698,3698,3699,3699,3699,3700,
                3700,3700,3701,3701,3701,3701,3702,3702,3702,3703,3703,3703,3704,3704,3704,3705,3705,3705,3705,3706,3706,3706,3707,3707,3707,3708,3708,3708,3708,3709,3709,3709,
                3710,3710,3710,3711,3711,3711,3711,3712,3712,3712,3713,3713,3713,3713,3714,3714,3714,3715,3715,3715,3716,3716,3716,3716,3717,3717,3717,3718,3718,3718,3719,3719,
                3719,3719,3720,3720,3720,3721,3721,3721,3721,3722,3722,3722,3723,3723,3723,3724,3724,3724,3724,3725,3725,3725,3726,3726,3726,3726,3727,3727,3727,3728,3728,3728,
                3728,3729,3729,3729,3730,3730,3730,3730,3731,3731,3731,3732,3732,3732,3733,3733,3733,3733,3734,3734,3734,3735,3735,3735,3735,3736,3736,3736,3737,3737,3737,3737,
                3738,3738,3738,3739,3739,3739,3739,3740,3740,3740,3741,3741,3741,3741,3742,3742,3742,3743,3743,3743,3743,3744,3744,3744,3745,3745,3745,3745,3746,3746,3746,3746,
                3747,3747,3747,3748,3748,3748,3748,3749,3749,3749,3750,3750,3750,3750,3751,3751,3751,3752,3752,3752,3752,3753,3753,3753,3753,3754,3754,3754,3755,3755,3755,3755,
                3756,3756,3756,3757,3757,3757,3757,3758,3758,3758,3758,3759,3759,3759,3760,3760,3760,3760,3761,3761,3761,3762,3762,3762,3762,3763,3763,3763,3763,3764,3764,3764,
                3765,3765,3765,3765,3766,3766,3766,3766,3767,3767,3767,3768,3768,3768,3768,3769,3769,3769,3769,3770,3770,3770,3771,3771,3771,3771,3772,3772,3772,3772,3773,3773,
                3773,3773,3774,3774,3774,3775,3775,3775,3775,3776,3776,3776,3776,3777,3777,3777,3777,3778,3778,3778,3779,3779,3779,3779,3780,3780,3780,3780,3781,3781,3781,3781,
                3782,3782,3782,3783,3783,3783,3783,3784,3784,3784,3784,3785,3785,3785,3785,3786,3786,3786,3786,3787,3787,3787,3788,3788,3788,3788,3789,3789,3789,3789,3790,3790,
                3790,3790,3791,3791,3791,3791,3792,3792,3792,3792,3793,3793,3793,3793,3794,3794,3794,3795,3795,3795,3795,3796,3796,3796,3796,3797,3797,3797,3797,3798,3798,3798,
                3798,3799,3799,3799,3799,3800,3800,3800,3800,3801,3801,3801,3801,3802,3802,3802,3802,3803,3803,3803,3803,3804,3804,3804,3804,3805,3805,3805,3805,3806,3806,3806,
                3806,3807,3807,3807,3807,3808,3808,3808,3809,3809,3810,3810,3810,3811,3811,3811,3811,3812,3812,3812,3813,3813,3813,3813,3814,3814,3814,3814,3815,3815,3815,3816,
                3816,3816,3817,3817,3817,3817,3818,3818,3818,3819,3819,3819,3819,3820,3820,3820,3821,3821,3821,3822,3822,3822,3822,3823,3823,3823,3824,3824,3824,3825,3825,3825,
                3825,3826,3826,3826,3827,3827,3827,3828,3828,3828,3828,3829,3829,3829,3830,3830,3830,3831,3831,3831,3832,3832,3832,3833,3833,3833,3833,3834,3834,3834,3835,3835,
                3835,3836,3836,3836,3837,3837,3837,3838,3838,3838,3839,3839,3839,3840,3840,3840,3841,3841,3841,3842,3842,3842,3843,3843,3843,3843,3844,3844,3844,3845,3845,3845,
                3846,3846,3846,3847,3847,3847,3848,3848,3849,3849,3849,3850,3850,3850,3851,3851,3851,3852,3852,3852,3853,3853,3853,3854,3854,3854,3855,3855,3855,3856,3856,3856,
                3857,3857,3857,3858,3858,3858,3859,3859,3860,3860,3860,3861,3861,3861,3862,3862,3862,3863,3863,3863,3864,3864,3865,3865,3865,3866,3866,3866,3867,3867,3867,3868,
                3868,3868,3869,3869,3870,3870,3870,3871,3871,3871,3872,3872,3873,3873,3873,3874,3874,3874,3875,3875,3875,3876,3876,3877,3877,3877,3878,3878,3878,3879,3879,3880,
                3880,3880,3881,3881,3882,3882,3882,3883,3883,3883,3884,3884,3885,3885,3885,3886,3886,3886,3887,3887,3888,3888,3888,3889,3889,3890,3890,3890,3891,3891,3892,3892,
                3892,3893,3893,3894,3894,3894,3895,3895,3896,3896,3896,3897,3897,3897,3898,3898,3899,3899,3899,3900,3900,3901,3901,3902,3902,3902,3903,3903,3904,3904,3904,3905,
                3905,3906,3906,3906,3907,3907,3908,3908,3908,3909,3909,3910,3910,3911,3911,3911,3912,3912,3913,3913,3913,3914,3914,3915,3915,3916,3916,3916,3917,3917,3918,3918,
                3918,3919,3919,3920,3920,3921,3921,3921,3922,3922,3923,3923,3924,3924,3924,3925,3925,3926,3926,3927,3927,3927,3928,3928,3929,3929,3930,3930,3930,3931,3931,3932,
                3932,3933,3933,3934,3934,3934,3935,3935,3936,3936,3937,3937,3938,3938,3938,3939,3939,3940,3940,3941,3941,3942,3942,3942,3943,3943,3944,3944,3945,3945,3946,3946,
                3946,3947,3947,3948,3948,3949,3949,3950,3950,3951,3951,3952,3952,3952,3953,3953,3954,3954,3955,3955,3956,3956,3957,3957,3958,3959,3959,3960,3960,3961,3961,3961,
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
                   0,   6,  12,  18,  24,  31,  37,  43,  49,  55,  62,  68,  74,  80,  86,  93,  99, 105, 111, 117, 123, 129, 136, 142, 148, 154, 160, 166, 172, 178, 184, 190,
                 196, 202, 208, 214, 221, 227, 233, 239, 245, 251, 257, 263, 269, 275, 281, 286, 292, 298, 304, 310, 316, 322, 328, 334, 340, 346, 352, 358, 364, 372, 379, 386,
                 392, 399, 406, 412, 419, 425, 431, 437, 443, 449, 455, 461, 466, 472, 477, 482, 487, 492, 497, 502, 507, 511, 516, 520, 524, 528, 533, 538, 543, 549, 554, 559,
                 564, 569, 574, 579, 584, 589, 593, 598, 602, 607, 611, 615, 620, 624, 628, 632, 636, 640, 644, 647, 651, 654, 658, 661, 665, 667, 670, 673, 675, 678, 681, 683,
                 686, 689, 691, 694, 697, 699, 702, 704, 707, 709, 712, 714, 717, 719, 722, 724, 727, 729, 732, 734, 737, 739, 741, 744, 746, 749, 751, 753, 756, 758, 760, 762,
                 764, 767, 770, 773, 776, 779, 783, 786, 789, 792, 795, 798, 801, 804, 807, 811, 814, 817, 820, 823, 826, 829, 831, 834, 837, 840, 843, 846, 849, 852, 855, 858,
                 860, 863, 866, 869, 872, 874, 877, 880, 883, 885, 888, 891, 893, 896, 899, 901, 904, 907, 909, 912, 914, 917, 920, 922, 925, 927, 930, 932, 935, 937, 940, 942,
                 945, 947, 949, 952, 954, 957, 959, 961, 964, 966, 968, 971, 973, 975, 977, 980, 982, 984, 986, 989, 991, 993, 995, 997, 999,1001,1004,1006,1008,1010,1012,1014,
                1016,1018,1020,1022,1024,1026,1028,1030,1032,1034,1036,1038,1040,1041,1043,1045,1047,1049,1051,1052,1054,1056,1058,1060,1061,1063,1065,1066,1068,1070,1071,1073,
                1075,1076,1078,1080,1081,1083,1084,1086,1087,1089,1091,1092,1092,1095,1098,1100,1103,1106,1108,1111,1113,1116,1119,1121,1124,1126,1129,1131,1134,1136,1139,1141,
                1144,1147,1149,1152,1154,1157,1159,1162,1164,1166,1169,1171,1174,1176,1179,1181,1184,1186,1189,1191,1193,1196,1198,1201,1203,1205,1208,1210,1212,1215,1217,1220,
                1222,1224,1227,1229,1231,1234,1236,1238,1240,1243,1245,1247,1250,1252,1254,1256,1259,1261,1263,1265,1268,1270,1272,1274,1277,1279,1281,1283,1285,1288,1290,1292,
                1294,1296,1298,1301,1303,1305,1307,1309,1311,1313,1316,1318,1320,1322,1324,1326,1328,1330,1332,1334,1336,1339,1341,1343,1345,1347,1349,1351,1353,1355,1357,1359,
                1361,1363,1365,1367,1369,1371,1373,1375,1377,1378,1380,1382,1384,1386,1388,1390,1392,1394,1396,1398,1400,1401,1403,1405,1407,1409,1411,1413,1414,1416,1418,1420,
                1422,1424,1425,1427,1429,1431,1433,1434,1436,1438,1440,1441,1443,1445,1447,1448,1450,1452,1454,1455,1457,1459,1460,1462,1464,1466,1467,1469,1471,1472,1474,1476,
                1477,1479,1480,1482,1484,1485,1487,1489,1490,1492,1493,1495,1496,1498,1500,1501,1503,1504,1506,1507,1509,1510,1512,1514,1515,1517,1518,1520,1521,1523,1524,1525,
                1527,1528,1530,1531,1533,1534,1536,1537,1537,1540,1542,1544,1547,1549,1551,1553,1556,1558,1560,1562,1565,1567,1569,1571,1573,1576,1578,1580,1582,1584,1587,1589,
                1591,1593,1595,1598,1600,1602,1604,1606,1608,1611,1613,1615,1617,1619,1621,1624,1626,1628,1630,1632,1634,1636,1638,1640,1643,1645,1647,1649,1651,1653,1655,1657,
                1659,1661,1663,1666,1668,1670,1672,1674,1676,1678,1680,1682,1684,1686,1688,1690,1692,1694,1696,1698,1700,1702,1704,1706,1708,1710,1712,1714,1716,1718,1720,1722,
                1724,1726,1728,1730,1732,1734,1736,1738,1740,1741,1743,1745,1747,1749,1751,1753,1755,1757,1759,1761,1762,1764,1766,1768,1770,1772,1774,1776,1778,1779,1781,1783,
                1785,1787,1789,1791,1793,1795,1798,1800,1802,1804,1806,1808,1810,1813,1815,1817,1819,1821,1823,1825,1828,1830,1832,1834,1836,1838,1840,1842,1844,1846,1849,1851,
                1853,1855,1857,1859,1861,1863,1865,1867,1869,1871,1874,1876,1878,1880,1882,1884,1886,1888,1890,1892,1894,1896,1898,1900,1902,1904,1906,1908,1910,1912,1914,1916,
                1918,1920,1922,1924,1926,1928,1930,1932,1934,1936,1938,1940,1942,1944,1946,1948,1950,1952,1954,1956,1958,1959,1961,1963,1965,1967,1969,1971,1973,1975,1977,1979,
                1981,1983,1984,1986,1988,1990,1992,1994,1996,1998,2000,2001,2003,2005,2007,2009,2011,2013,2014,2016,2018,2020,2022,2024,2026,2027,2029,2031,2033,2035,2037,2038,
                2040,2042,2044,2046,2047,2049,2051,2053,2055,2056,2058,2060,2062,2064,2065,2067,2069,2071,2072,2074,2076,2078,2080,2081,2083,2085,2087,2088,2090,2092,2093,2095,
                2097,2099,2100,2102,2104,2106,2107,2109,2111,2112,2114,2116,2117,2119,2121,2123,2124,2126,2128,2129,2131,2133,2134,2136,2138,2139,2141,2143,2144,2146,2147,2149,
                2151,2152,2154,2156,2157,2159,2161,2162,2164,2165,2167,2169,2170,2172,2173,2175,2177,2178,2180,2181,2183,2184,2186,2188,2189,2191,2192,2194,2195,2197,2199,2200,
                2202,2203,2205,2206,2208,2209,2211,2212,2214,2215,2217,2219,2220,2222,2223,2225,2226,2228,2229,2231,2232,2234,2235,2236,2238,2239,2241,2242,2244,2245,2247,2248,
                2250,2251,2253,2254,2255,2257,2259,2261,2262,2264,2266,2268,2269,2271,2273,2274,2276,2278,2280,2281,2283,2285,2286,2288,2290,2292,2293,2295,2297,2298,2300,2302,
                2303,2305,2307,2308,2310,2312,2313,2315,2317,2318,2320,2322,2323,2325,2327,2328,2330,2331,2333,2335,2336,2338,2340,2341,2343,2344,2346,2348,2349,2351,2353,2354,
                2356,2357,2359,2361,2362,2364,2365,2367,2369,2370,2372,2373,2375,2376,2378,2380,2381,2383,2384,2386,2387,2389,2391,2392,2394,2395,2397,2398,2400,2401,2403,2404,
                2406,2407,2409,2411,2412,2414,2415,2417,2418,2420,2421,2423,2424,2426,2427,2429,2430,2432,2433,2435,2436,2438,2439,2441,2442,2444,2445,2447,2448,2449,2451,2452,
                2454,2455,2457,2458,2460,2461,2463,2464,2465,2467,2468,2470,2471,2473,2474,2476,2477,2478,2480,2481,2483,2484,2485,2487,2488,2490,2491,2493,2494,2495,2497,2498,
                2499,2501,2502,2504,2505,2506,2508,2509,2511,2512,2513,2515,2516,2517,2519,2520,2521,2523,2524,2525,2527,2528,2530,2531,2532,2534,2535,2536,2538,2539,2540,2541,
                2543,2544,2545,2547,2548,2549,2551,2552,2553,2555,2556,2557,2558,2560,2561,2562,2564,2565,2566,2567,2569,2570,2571,2573,2574,2575,2576,2578,2579,2580,2581,2583,
                2584,2585,2586,2588,2589,2590,2591,2593,2594,2595,2596,2597,2599,2600,2601,2602,2604,2605,2606,2607,2608,2610,2611,2612,2613,2614,2616,2617,2618,2619,2620,2621,
                2623,2624,2625,2626,2627,2628,2630,2631,2632,2633,2634,2635,2637,2638,2639,2640,2641,2642,2643,2645,2646,2647,2648,2649,2650,2651,2652,2654,2655,2656,2657,2658,
                2659,2660,2661,2662,2663,2665,2666,2667,2668,2669,2670,2671,2672,2673,2674,2675,2676,2678,2679,2680,2681,2682,2683,2684,2685,2686,2687,2688,2689,2690,2691,2692,
                2693,2694,2695,2696,2697,2698,2699,2700,2701,2702,2703,2704,2706,2707,2708,2709,2710,2711,2712,2712,2713,2714,2715,2716,2717,2718,2719,2720,2721,2722,2723,2724,
                2725,2726,2727,2728,2729,2730,2731,2732,2733,2734,2735,2736,2737,2737,2738,2739,2740,2741,2742,2743,2744,2745,2746,2747,2748,2749,2749,2750,2751,2752,2753,2754,
                2755,2756,2757,2757,2758,2759,2760,2761,2762,2763,2764,2764,2765,2766,2767,2768,2769,2770,2770,2771,2772,2773,2774,2775,2775,2776,2777,2778,2779,2780,2780,2781,
                2782,2783,2784,2785,2785,2786,2787,2788,2789,2789,2790,2791,2792,2793,2793,2794,2795,2796,2796,2797,2798,2799,2800,2800,2801,2802,2803,2803,2804,2805,2806,2806,
                2807,2808,2809,2809,2810,2811,2812,2812,2813,2814,2815,2815,2816,2817,2817,2818,2819,2820,2820,2821,2822,2822,2823,2824,2824,2825,2826,2827,2827,2828,2829,2829,
                2830,2831,2831,2832,2833,2833,2834,2835,2835,2836,2837,2837,2838,2839,2839,2840,2840,2841,2842,2842,2843,2844,2844,2845,2845,2846,2847,2847,2848,2849,2849,2850,
                2850,2851,2852,2852,2853,2853,2854,2855,2855,2856,2856,2857,2857,2858,2859,2859,2860,2860,2861,2861,2862,2863,2863,2864,2864,2865,2865,2866,2866,2867,2867,2868,
                2869,2869,2870,2870,2871,2871,2872,2872,2873,2873,2874,2874,2875,2875,2876,2876,2877,2877,2878,2878,2879,2879,2880,2880,2881,2881,2882,2882,2883,2883,2884,2884,
                2884,2885,2885,2886,2886,2887,2887,2888,2888,2889,2889,2889,2890,2890,2891,2891,2892,2892,2892,2893,2893,2894,2894,2894,2895,2895,2896,2896,2896,2896,2897,2898,
                2899,2899,2900,2901,2902,2902,2903,2904,2905,2905,2906,2907,2908,2908,2909,2910,2911,2911,2912,2913,2914,2914,2915,2916,2917,2917,2918,2919,2920,2920,2921,2922,
                2923,2923,2924,2925,2925,2926,2927,2928,2928,2929,2930,2930,2931,2932,2933,2933,2934,2935,2935,2936,2937,2938,2938,2939,2940,2940,2941,2942,2942,2943,2944,2944,
                2945,2946,2947,2947,2948,2949,2949,2950,2951,2951,2952,2953,2953,2954,2955,2955,2956,2957,2957,2958,2959,2959,2960,2961,2961,2962,2963,2963,2964,2965,2965,2966,
                2967,2967,2968,2969,2969,2970,2970,2971,2972,2972,2973,2974,2974,2975,2976,2976,2977,2977,2978,2979,2979,2980,2981,2981,2982,2982,2983,2984,2984,2985,2985,2986,
                2987,2987,2988,2989,2989,2990,2990,2991,2992,2992,2993,2993,2994,2995,2995,2996,2996,2997,2997,2998,2999,2999,3000,3000,3001,3002,3002,3003,3003,3004,3004,3005,
                3006,3006,3007,3007,3008,3008,3009,3010,3010,3011,3011,3012,3012,3013,3013,3014,3015,3015,3016,3016,3017,3017,3018,3018,3019,3019,3020,3020,3021,3022,3022,3023,
                3023,3024,3024,3025,3025,3026,3026,3027,3027,3028,3028,3029,3029,3030,3030,3031,3031,3032,3033,3033,3034,3034,3035,3035,3036,3036,3037,3037,3038,3038,3039,3039,
                3039,3040,3040,3041,3041,3042,3042,3043,3043,3044,3044,3045,3045,3046,3046,3047,3047,3048,3048,3049,3049,3050,3050,3050,3051,3051,3052,3052,3053,3053,3054,3054,
                3055,3055,3055,3056,3056,3057,3057,3058,3058,3059,3059,3059,3060,3060,3061,3061,3062,3062,3062,3063,3063,3064,3064,3065,3065,3065,3066,3066,3067,3067,3068,3068,
                3068,3069,3069,3070,3070,3070,3071,3071,3072,3072,3073,3073,3074,3074,3075,3075,3076,3076,3077,3077,3078,3079,3079,3080,3080,3081,3081,3082,3082,3083,3083,3084,
                3084,3085,3085,3086,3087,3087,3088,3088,3089,3089,3090,3090,3091,3091,3092,3092,3093,3093,3094,3094,3095,3095,3096,3096,3097,3097,3098,3098,3099,3099,3100,3100,
                3101,3101,3102,3102,3103,3103,3104,3104,3105,3105,3106,3106,3107,3107,3108,3108,3109,3109,3110,3110,3111,3111,3112,3112,3112,3113,3113,3114,3114,3115,3115,3116,
                3116,3117,3117,3118,3118,3119,3119,3120,3120,3120,3121,3121,3122,3122,3123,3123,3124,3124,3125,3125,3125,3126,3126,3127,3127,3128,3128,3129,3129,3129,3130,3130,
                3131,3131,3132,3132,3133,3133,3133,3134,3134,3135,3135,3136,3136,3136,3137,3137,3138,3138,3139,3139,3139,3140,3140,3141,3141,3141,3142,3142,3143,3143,3144,3144,
                3144,3145,3145,3146,3146,3146,3147,3147,3148,3148,3148,3149,3149,3150,3150,3150,3151,3151,3152,3152,3152,3153,3153,3154,3154,3154,3155,3155,3155,3156,3156,3157,
                3157,3157,3158,3158,3159,3159,3159,3160,3160,3160,3161,3161,3161,3162,3162,3163,3163,3163,3164,3164,3164,3165,3165,3166,3166,3166,3167,3167,3167,3168,3168,3168,
                3169,3169,3169,3170,3170,3170,3171,3171,3171,3172,3172,3173,3173,3173,3174,3174,3174,3175,3175,3175,3176,3176,3176,3177,3177,3177,3178,3178,3178,3179,3179,3179,
                3179,3180,3180,3180,3181,3181,3181,3182,3182,3182,3183,3183,3183,3184,3184,3184,3185,3185,3185,3185,3186,3186,3186,3187,3187,3187,3188,3188,3188,3188,3189,3189,
                3189,3190,3190,3190,3190,3191,3191,3191,3192,3192,3192,3192,3193,3193,3193,3194,3194,3194,3194,3195,3195,3195,3195,3195,3195,3195,3195,3196,3196,3196,3196,3196,
                3196,3197,3197,3197,3197,3197,3197,3198,3198,3198,3198,3198,3198,3198,3199,3199,3199,3199,3199,3199,3200,3200,3200,3200,3200,3200,3201,3201,3201,3201,3201,3201,
                3202,3202,3202,3202,3202,3202,3203,3203,3203,3203,3203,3203,3204,3204,3204,3204,3204,3204,3205,3205,3205,3205,3205,3205,3206,3206,3206,3206,3206,3206,3207,3207,
                3207,3207,3207,3207,3208,3208,3208,3208,3208,3208,3209,3209,3209,3209,3209,3209,3210,3210,3210,3210,3210,3210,3211,3211,3211,3211,3211,3211,3212,3212,3212,3212,
                3212,3212,3213,3213,3213,3213,3213,3213,3214,3214,3214,3214,3214,3214,3215,3215,3215,3215,3215,3216,3216,3216,3216,3216,3216,3217,3217,3217,3217,3217,3217,3218,
                3218,3218,3218,3218,3218,3219,3219,3219,3219,3219,3220,3220,3220,3220,3220,3220,3221,3221,3221,3221,3221,3221,3222,3222,3222,3222,3222,3222,3223,3223,3223,3223,
                3223,3224,3224,3224,3224,3224,3224,3225,3225,3225,3225,3225,3226,3226,3226,3226,3226,3226,3227,3227,3227,3227,3227,3227,3228,3228,3228,3228,3228,3229,3229,3229,
                3229,3229,3229,3230,3230,3230,3230,3230,3231,3231,3231,3231,3231,3231,3232,3232,3232,3232,3232,3233,3233,3233,3233,3233,3233,3234,3234,3234,3234,3234,3235,3235,
                3235,3235,3235,3236,3236,3236,3236,3236,3236,3237,3237,3237,3237,3237,3238,3238,3238,3238,3238,3238,3239,3239,3239,3239,3239,3240,3240,3240,3240,3240,3241,3241,
                3241,3241,3241,3241,3242,3242,3242,3242,3242,3243,3243,3243,3243,3243,3244,3244,3244,3244,3244,3245,3245,3245,3245,3245,3245,3246,3246,3246,3246,3246,3247,3247,
                3247,3247,3247,3248,3248,3248,3248,3248,3249,3249,3249,3249,3249,3249,3250,3250,3250,3250,3250,3251,3251,3251,3251,3251,3252,3252,3252,3252,3252,3253,3253,3253,
                3253,3253,3254,3254,3254,3254,3254,3255,3255,3255,3255,3255,3256,3256,3256,3256,3256,3257,3257,3257,3257,3257,3257,3258,3258,3258,3258,3258,3259,3259,3259,3259,
                3259,3260,3260,3260,3260,3260,3261,3261,3261,3261,3261,3262,3262,3262,3262,3262,3263,3263,3263,3263,3263,3264,3264,3264,3264,3264,3265,3265,3265,3265,3265,3266,
                3266,3266,3266,3266,3267,3267,3267,3267,3268,3268,3268,3268,3268,3269,3269,3269,3269,3269,3270,3270,3270,3270,3270,3271,3271,3271,3271,3271,3272,3272,3272,3272,
                3272,3273,3273,3273,3273,3273,3274,3274,3274,3274,3274,3275,3275,3275,3275,3275,3276,3276,3276,3276,3277,3277,3277,3277,3277,3278,3278,3278,3278,3278,3279,3279,
                3279,3279,3279,3280,3280,3280,3280,3281,3281,3281,3281,3281,3282,3282,3282,3282,3282,3283,3283,3283,3283,3283,3284,3284,3284,3284,3285,3285,3285,3285,3285,3286,
                3286,3286,3286,3286,3287,3287,3287,3287,3287,3288,3288,3288,3288,3289,3289,3289,3289,3289,3290,3290,3290,3290,3290,3291,3291,3291,3291,3292,3292,3292,3292,3292,
                3293,3293,3293,3293,3294,3294,3294,3294,3294,3295,3295,3295,3295,3295,3296,3296,3296,3296,3297,3297,3297,3297,3297,3298,3298,3298,3298,3299,3299,3299,3299,3299,
                3300,3300,3300,3300,3300,3301,3301,3301,3301,3302,3302,3302,3302,3302,3303,3303,3303,3303,3304,3304,3304,3304,3304,3305,3305,3305,3305,3306,3306,3306,3306,3306,
                3307,3307,3307,3307,3308,3308,3308,3308,3308,3309,3309,3309,3309,3310,3310,3310,3310,3311,3311,3311,3311,3311,3312,3312,3312,3312,3313,3313,3313,3313,3313,3314,
                3314,3314,3314,3315,3315,3315,3315,3315,3316,3316,3316,3316,3317,3317,3317,3317,3318,3318,3318,3318,3318,3319,3319,3319,3319,3320,3320,3320,3320,3321,3321,3321,
                3321,3321,3322,3322,3322,3322,3323,3323,3323,3323,3324,3324,3324,3324,3324,3325,3325,3325,3325,3326,3326,3326,3326,3327,3327,3327,3327,3327,3328,3328,3328,3328,
                3329,3329,3329,3329,3330,3330,3330,3330,3331,3331,3331,3331,3331,3332,3332,3332,3332,3333,3333,3333,3333,3334,3334,3334,3334,3335,3335,3335,3335,3335,3336,3336,
                3336,3336,3337,3337,3337,3337,3338,3338,3338,3338,3339,3339,3339,3339,3340,3340,3340,3340,3340,3341,3341,3341,3341,3342,3342,3342,3342,3343,3343,3343,3343,3344,
                3344,3344,3344,3345,3345,3345,3345,3346,3346,3346,3346,3346,3347,3347,3347,3347,3348,3348,3348,3348,3349,3349,3349,3349,3350,3350,3350,3350,3351,3351,3351,3351,
                3352,3352,3352,3352,3353,3353,3353,3353,3354,3354,3354,3354,3355,3355,3355,3355,3356,3356,3356,3356,3357,3358,3358,3358,3359,3359,3360,3360,3360,3361,3361,3362,
                3362,3362,3363,3363,3364,3364,3364,3365,3365,3366,3366,3367,3367,3367,3368,3368,3369,3369,3369,3370,3370,3371,3371,3371,3372,3372,3373,3373,3374,3374,3374,3375,
                3375,3376,3376,3377,3377,3377,3378,3378,3379,3379,3379,3380,3380,3381,3381,3382,3382,3382,3383,3383,3384,3384,3385,3385,3385,3386,3386,3387,3387,3388,3388,3388,
                3389,3389,3390,3390,3391,3391,3392,3392,3392,3393,3393,3394,3394,3395,3395,3395,3396,3396,3397,3397,3398,3398,3399,3399,3399,3400,3400,3401,3401,3402,3402,3403,
                3403,3403,3404,3404,3405,3405,3406,3406,3407,3407,3407,3408,3408,3409,3409,3410,3410,3411,3411,3412,3412,3412,3413,3413,3414,3414,3415,3415,3416,3416,3417,3417,
                3418,3418,3418,3419,3419,3420,3420,3421,3421,3422,3422,3423,3423,3424,3424,3424,3425,3425,3426,3426,3427,3427,3428,3428,3429,3429,3430,3430,3431,3431,3432,3432,
                3433,3433,3433,3434,3434,3435,3435,3436,3436,3437,3437,3438,3438,3439,3439,3440,3440,3441,3441,3442,3442,3443,3443,3444,3444,3445,3445,3445,3446,3446,3447,3447,
                3448,3448,3449,3449,3450,3450,3451,3451,3452,3452,3453,3453,3454,3454,3455,3455,3456,3456,3457,3457,3458,3458,3459,3459,3460,3460,3461,3461,3462,3462,3463,3463,
                3464,3464,3465,3465,3466,3466,3467,3467,3468,3468,3469,3469,3470,3470,3471,3471,3472,3472,3473,3473,3474,3474,3475,3475,3476,3476,3477,3478,3478,3479,3479,3480,
                3480,3481,3481,3482,3482,3483,3483,3484,3484,3485,3485,3486,3486,3487,3487,3488,3488,3489,3489,3490,3491,3491,3492,3492,3493,3493,3494,3494,3495,3495,3496,3496,
                3497,3497,3498,3498,3499,3499,3500,3501,3501,3502,3502,3503,3503,3504,3504,3505,3505,3506,3506,3507,3508,3508,3509,3509,3510,3510,3511,3511,3512,3512,3513,3513,
                3514,3515,3515,3516,3516,3517,3517,3518,3518,3519,3519,3520,3521,3521,3522,3522,3523,3523,3524,3524,3525,3526,3526,3527,3527,3528,3528,3529,3529,3530,3531,3531,
                3532,3532,3533,3533,3534,3534,3535,3536,3536,3537,3537,3538,3538,3539,3539,3540,3541,3541,3542,3542,3543,3543,3544,3545,3545,3546,3546,3547,3547,3548,3549,3549,
                3550,3550,3551,3551,3552,3553,3553,3554,3554,3555,3555,3556,3557,3557,3558,3558,3559,3559,3560,3561,3561,3562,3562,3563,3564,3564,3565,3565,3566,3566,3567,3568,
                3568,3569,3569,3570,3571,3571,3572,3572,3573,3574,3574,3575,3575,3576,3576,3577,3578,3578,3579,3579,3580,3581,3581,3582,3582,3583,3584,3584,3585,3585,3586,3587,
                3587,3588,3588,3589,3590,3590,3591,3591,3592,3593,3593,3594,3594,3595,3596,3596,3597,3597,3598,3599,3599,3600,3600,3601,3602,3602,3603,3604,3604,3605,3605,3606,
                3607,3607,3608,3608,3609,3610,3610,3611,3612,3612,3613,3613,3614,3614,3614,3615,3615,3616,3616,3617,3617,3618,3618,3618,3619,3619,3620,3620,3621,3621,3621,3622,
                3622,3623,3623,3624,3624,3625,3625,3625,3626,3626,3627,3627,3628,3628,3629,3629,3630,3630,3630,3631,3631,3632,3632,3633,3633,3634,3634,3635,3635,3635,3636,3636,
                3637,3637,3638,3638,3639,3639,3640,3640,3641,3641,3642,3642,3642,3643,3643,3644,3644,3645,3645,3646,3646,3647,3647,3648,3648,3649,3649,3650,3650,3651,3651,3652,
                3652,3653,3653,3654,3654,3655,3655,3655,3656,3656,3657,3657,3658,3658,3659,3659,3660,3660,3661,3661,3662,3662,3663,3663,3664,3664,3665,3665,3666,3666,3667,3667,
                3668,3668,3669,3669,3670,3670,3671,3671,3672,3673,3673,3674,3674,3675,3675,3676,3676,3677,3677,3678,3678,3679,3679,3680,3680,3681,3681,3682,3682,3683,3683,3684,
                3684,3685,3686,3686,3687,3687,3688,3688,3689,3689,3690,3690,3691,3691,3692,3692,3693,3694,3694,3695,3695,3696,3696,3697,3697,3698,3698,3699,3700,3700,3701,3701,
                3702,3702,3703,3703,3704,3704,3705,3706,3706,3707,3707,3708,3708,3709,3709,3710,3711,3711,3712,3712,3713,3713,3714,3714,3715,3716,3716,3717,3717,3718,3718,3719,
                3720,3720,3721,3721,3722,3722,3723,3724,3724,3725,3725,3726,3726,3727,3728,3728,3729,3729,3730,3731,3731,3732,3732,3733,3733,3734,3735,3735,3736,3736,3737,3738,
                3738,3739,3739,3740,3741,3741,3742,3742,3743,3743,3744,3745,3745,3746,3746,3747,3748,3748,3749,3749,3750,3751,3751,3752,3752,3753,3754,3754,3755,3756,3756,3757,
                3757,3758,3759,3759,3760,3760,3761,3762,3762,3763,3764,3764,3765,3765,3766,3767,3767,3768,3768,3769,3770,3770,3771,3772,3772,3773,3773,3774,3775,3775,3776,3777,
                3777,3778,3779,3779,3780,3780,3781,3782,3782,3783,3784,3784,3785,3786,3786,3787,3787,3788,3789,3789,3790,3791,3791,3792,3793,3793,3794,3795,3795,3796,3797,3797,
                3798,3799,3799,3800,3800,3801,3802,3802,3803,3804,3804,3805,3806,3806,3807,3808,3808,3809,3810,3810,3811,3812,3812,3813,3814,3814,3815,3816,3816,3817,3818,3818,
                3819,3820,3820,3821,3822,3822,3823,3824,3825,3825,3826,3827,3827,3828,3829,3829,3830,3831,3831,3832,3833,3833,3834,3835,3836,3837,3838,3838,3839,3839,3840,3841,
                3841,3842,3843,3843,3844,3845,3845,3846,3847,3848,3848,3849,3850,3850,3851,3852,3852,3853,3854,3854,3855,3856,3856,3857,3858,3859,3859,3860,3861,3861,3862,3863,
                3864,3864,3865,3866,3866,3867,3868,3869,3869,3870,3871,3871,3872,3873,3874,3874,3875,3876,3877,3877,3878,3879,3880,3880,3881,3882,3883,3883,3884,3885,3886,3886,
                3887,3888,3889,3889,3890,3891,3892,3892,3893,3894,3895,3896,3896,3897,3898,3899,3899,3900,3901,3902,3903,3903,3904,3905,3906,3907,3907,3908,3909,3910,3911,3911,
                3912,3913,3914,3915,3915,3916,3917,3918,3919,3920,3920,3921,3922,3923,3924,3925,3925,3926,3927,3928,3929,3930,3930,3931,3932,3933,3934,3935,3935,3936,3937,3938,
                3939,3940,3941,3941,3942,3943,3944,3945,3946,3947,3948,3948,3949,3950,3951,3952,3953,3954,3955,3955,3956,3957,3958,3959,3960,3961,3962,3963,3964,3964,3965,3966,
                3967,3968,3969,3970,3971,3972,3973,3974,3974,3975,3976,3977,3978,3979,3980,3981,3982,3983,3984,3985,3986,3987,3987,3988,3989,3990,3991,3992,3993,3994,3995,3996,
                3997,3998,3999,4000,4001,4002,4003,4004,4005,4006,4007,4008,4009,4009,4010,4011,4012,4013,4014,4015,4016,4017,4018,4019,4020,4021,4022,4023,4024,4025,4026,4027,
                4028,4029,4030,4031,4032,4033,4034,4035,4036,4037,4038,4039,4040,4041,4042,4043,4045,4046,4047,4048,4049,4050,4051,4052,4053,4054,4055,4056,4057,4058,4059,4060,
                4061,4062,4063,4064,4065,4066,4068,4069,4070,4071,4072,4073,4074,4075,4076,4077,4078,4079,4080,4081,4083,4084,4085,4086,4087,4088,4089,4090,4091,4092,4093,4095,
          },
          /* aec trigger 4 */
          {
                   0,   0,   1,   1,   2,   2,   3,   3,   4,   4,   5,   6,   6,   7,   7,   8,   8,   9,  10,  10,  11,  12,  12,  13,  13,  14,  15,  15,  16,  17,  16,  16,
                  17,  17,  18,  19,  19,  20,  20,  21,  22,  22,  23,  23,  24,  24,  25,  26,  26,  27,  27,  28,  28,  29,  29,  30,  30,  31,  31,  32,  33,  33,  34,  34,
                  35,  35,  36,  36,  37,  37,  38,  38,  38,  39,  39,  40,  40,  41,  41,  42,  42,  43,  43,  44,  44,  44,  45,  45,  46,  46,  47,  48,  48,  49,  50,  50,
                  51,  51,  52,  53,  53,  54,  55,  56,  56,  57,  58,  59,  59,  60,  61,  62,  63,  63,  64,  65,  66,  67,  68,  69,  70,  71,  71,  72,  72,  73,  74,  75,
                  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  87,  88,  89,  90,  92,  93,  94,  96,  97,  99, 100, 102, 103, 105, 107, 108, 110, 112, 114, 116, 118,
                 120, 121, 122, 122, 123, 124, 125, 126, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 145, 146, 147, 148, 149, 151,
                 152, 153, 154, 156, 157, 158, 160, 161, 163, 164, 166, 167, 168, 170, 171, 173, 175, 176, 178, 179, 181, 183, 184, 186, 188, 189, 191, 193, 195, 196, 197, 198,
                 199, 200, 201, 202, 204, 205, 206, 207, 208, 210, 211, 212, 213, 215, 216, 217, 219, 220, 221, 223, 224, 226, 227, 228, 230, 231, 233, 234, 236, 237, 239, 240,
                 242, 243, 245, 246, 248, 250, 251, 253, 255, 256, 258, 260, 261, 263, 265, 266, 268, 270, 272, 274, 275, 277, 279, 281, 283, 285, 286, 288, 290, 292, 294, 296,
                 298, 300, 302, 304, 306, 308, 310, 312, 314, 316, 318, 320, 323, 324, 326, 328, 330, 332, 334, 335, 337, 339, 341, 343, 345, 347, 349, 351, 353, 355, 357, 359,
                 361, 363, 365, 367, 369, 371, 373, 376, 378, 380, 382, 384, 386, 389, 391, 393, 395, 398, 400, 402, 405, 407, 409, 412, 414, 416, 419, 421, 424, 426, 429, 431,
                 434, 436, 439, 441, 444, 446, 449, 451, 454, 457, 459, 462, 465, 467, 470, 473, 475, 478, 481, 484, 486, 489, 492, 495, 498, 500, 503, 506, 509, 512, 515, 518,
                 521, 524, 527, 530, 533, 536, 539, 542, 545, 548, 551, 554, 557, 560, 563, 566, 570, 573, 576, 580, 583, 586, 589, 593, 596, 599, 603, 606, 609, 613, 616, 619,
                 623, 626, 630, 633, 636, 640, 643, 647, 650, 654, 657, 661, 665, 668, 672, 675, 679, 683, 686, 690, 694, 697, 701, 705, 709, 712, 716, 720, 724, 728, 731, 735,
                 739, 743, 747, 751, 755, 759, 763, 767, 771, 775, 779, 783, 787, 791, 795, 799, 803, 807, 811, 816, 820, 824, 828, 832, 837, 841, 845, 849, 854, 858, 862, 866,
                 871, 875, 880, 884, 888, 893, 897, 902, 906, 911, 915, 920, 924, 929, 933, 938, 942, 947, 951, 956, 961, 965, 970, 975, 979, 984, 989, 994, 998,1003,1008,1013,
                1017,1022,1027,1032,1037,1042,1047,1052,1057,1060,1064,1067,1070,1074,1077,1080,1084,1087,1090,1094,1097,1100,1104,1107,1110,1114,1117,1120,1124,1127,1130,1134,
                1137,1141,1144,1147,1151,1154,1157,1161,1164,1168,1171,1174,1178,1181,1185,1188,1191,1195,1198,1202,1205,1209,1212,1216,1219,1222,1226,1229,1233,1236,1240,1243,
                1247,1250,1254,1257,1260,1264,1267,1271,1274,1278,1281,1285,1288,1292,1295,1299,1302,1306,1309,1313,1317,1320,1324,1327,1331,1334,1338,1341,1345,1348,1352,1356,
                1359,1363,1366,1370,1373,1377,1380,1384,1388,1391,1395,1398,1402,1406,1409,1413,1416,1420,1424,1427,1431,1434,1438,1442,1445,1449,1453,1456,1460,1464,1467,1471,
                1475,1479,1482,1486,1490,1493,1497,1501,1504,1508,1511,1515,1518,1522,1525,1529,1532,1535,1539,1542,1546,1549,1552,1556,1559,1562,1565,1569,1572,1575,1578,1582,
                1585,1588,1591,1594,1597,1600,1603,1606,1609,1612,1615,1618,1621,1624,1627,1630,1633,1636,1639,1642,1644,1647,1650,1653,1656,1658,1661,1664,1666,1669,1672,1674,
                1677,1680,1682,1685,1687,1690,1692,1695,1697,1700,1702,1705,1707,1709,1712,1714,1717,1719,1721,1724,1726,1728,1730,1733,1735,1737,1739,1741,1743,1746,1748,1750,
                1752,1754,1756,1758,1760,1762,1764,1766,1768,1770,1772,1774,1775,1777,1779,1781,1783,1784,1786,1788,1790,1791,1793,1795,1797,1798,1800,1802,1804,1806,1808,1810,
                1812,1814,1816,1818,1820,1822,1824,1825,1827,1829,1831,1833,1835,1837,1839,1841,1843,1844,1846,1848,1850,1852,1854,1856,1857,1859,1861,1863,1865,1867,1869,1870,
                1872,1874,1876,1878,1880,1881,1883,1885,1887,1889,1890,1892,1894,1896,1898,1899,1901,1903,1905,1907,1908,1910,1912,1914,1915,1917,1919,1921,1923,1924,1926,1928,
                1929,1931,1933,1935,1936,1938,1940,1942,1943,1945,1947,1948,1950,1952,1954,1955,1957,1959,1960,1962,1964,1965,1967,1969,1970,1972,1974,1975,1977,1979,1980,1982,
                1984,1985,1987,1989,1990,1992,1994,1995,1997,1998,2000,2002,2003,2005,2007,2008,2010,2011,2013,2015,2016,2018,2019,2021,2022,2024,2026,2027,2029,2030,2032,2033,
                2035,2037,2038,2040,2041,2043,2044,2046,2047,2049,2050,2052,2054,2055,2057,2058,2060,2061,2063,2064,2066,2067,2069,2070,2072,2073,2075,2076,2077,2079,2080,2082,
                2083,2085,2086,2088,2089,2091,2092,2094,2095,2096,2098,2099,2101,2102,2104,2105,2106,2108,2109,2111,2112,2113,2115,2116,2118,2119,2120,2122,2123,2125,2126,2127,
                2129,2130,2131,2133,2134,2135,2137,2138,2140,2141,2142,2144,2145,2146,2148,2149,2150,2152,2153,2154,2155,2157,2158,2159,2161,2162,2163,2165,2166,2167,2168,2170,
                2171,2172,2174,2175,2176,2177,2179,2180,2181,2182,2184,2185,2186,2187,2189,2190,2191,2192,2193,2195,2196,2197,2198,2200,2201,2202,2203,2204,2206,2207,2208,2209,
                2210,2212,2213,2214,2215,2216,2217,2219,2220,2221,2222,2223,2224,2225,2227,2228,2229,2230,2231,2232,2233,2235,2236,2237,2238,2239,2240,2241,2242,2243,2245,2246,
                2247,2248,2249,2250,2251,2252,2253,2254,2255,2256,2257,2259,2260,2261,2262,2263,2264,2265,2266,2267,2268,2269,2270,2271,2272,2273,2274,2275,2276,2277,2278,2279,
                2280,2281,2282,2283,2284,2285,2286,2287,2288,2289,2290,2291,2292,2293,2294,2295,2296,2297,2298,2299,2299,2300,2301,2302,2303,2304,2305,2306,2307,2308,2309,2310,
                2311,2311,2312,2313,2314,2315,2316,2317,2318,2319,2319,2320,2321,2322,2323,2324,2325,2325,2326,2327,2328,2329,2330,2331,2331,2332,2333,2334,2335,2336,2336,2337,
                2338,2339,2340,2340,2341,2342,2343,2344,2344,2345,2346,2347,2347,2348,2349,2350,2351,2351,2352,2353,2354,2354,2355,2356,2357,2357,2358,2359,2360,2360,2361,2362,
                2362,2363,2364,2365,2365,2366,2367,2367,2368,2369,2369,2370,2371,2371,2372,2373,2374,2374,2375,2376,2377,2378,2378,2379,2380,2381,2381,2382,2383,2384,2384,2385,
                2386,2387,2387,2388,2389,2390,2390,2391,2392,2393,2393,2394,2395,2396,2396,2397,2398,2399,2399,2400,2401,2402,2402,2403,2404,2405,2405,2406,2407,2408,2408,2409,
                2410,2411,2411,2412,2413,2414,2414,2415,2416,2417,2417,2418,2419,2420,2420,2421,2422,2423,2423,2424,2425,2426,2426,2427,2428,2429,2429,2430,2431,2431,2432,2433,
                2434,2434,2435,2436,2437,2437,2438,2439,2440,2440,2441,2442,2442,2443,2444,2445,2445,2446,2447,2448,2448,2449,2450,2451,2451,2452,2453,2453,2454,2455,2456,2456,
                2457,2458,2458,2459,2460,2461,2461,2462,2463,2464,2464,2465,2466,2466,2467,2468,2469,2469,2470,2471,2471,2472,2473,2474,2474,2475,2476,2476,2477,2478,2479,2479,
                2480,2481,2481,2482,2483,2484,2484,2485,2486,2486,2487,2488,2489,2489,2490,2491,2491,2492,2493,2493,2494,2495,2496,2496,2497,2498,2498,2499,2500,2500,2501,2502,
                2503,2503,2504,2505,2505,2506,2507,2507,2508,2509,2510,2510,2511,2512,2512,2513,2514,2514,2515,2516,2517,2517,2518,2519,2519,2520,2521,2521,2522,2523,2523,2524,
                2525,2525,2526,2527,2528,2528,2529,2530,2530,2531,2532,2532,2533,2534,2534,2535,2536,2536,2537,2538,2538,2539,2540,2541,2541,2542,2543,2543,2544,2545,2545,2546,
                2547,2547,2548,2549,2549,2550,2551,2551,2552,2553,2553,2554,2555,2555,2556,2557,2557,2558,2559,2559,2560,2561,2561,2562,2563,2563,2564,2565,2565,2566,2567,2567,
                2568,2569,2569,2570,2571,2571,2572,2573,2573,2574,2575,2575,2576,2577,2577,2578,2579,2579,2580,2581,2581,2582,2583,2583,2584,2585,2585,2586,2586,2587,2588,2588,
                2589,2589,2590,2590,2591,2591,2592,2593,2593,2594,2594,2595,2595,2596,2596,2597,2598,2598,2599,2599,2600,2600,2601,2601,2602,2603,2603,2604,2604,2605,2605,2606,
                2607,2607,2608,2608,2609,2609,2610,2610,2611,2612,2612,2613,2613,2614,2614,2615,2616,2616,2617,2617,2618,2618,2619,2619,2620,2621,2621,2622,2622,2623,2623,2624,
                2625,2625,2626,2626,2627,2627,2628,2629,2629,2630,2630,2631,2631,2632,2632,2633,2634,2634,2635,2635,2636,2636,2637,2638,2638,2639,2639,2640,2640,2641,2642,2642,
                2643,2643,2644,2644,2645,2646,2646,2647,2647,2648,2648,2649,2650,2650,2651,2651,2652,2652,2653,2654,2654,2655,2655,2656,2657,2657,2658,2658,2659,2659,2660,2661,
                2661,2662,2662,2663,2663,2664,2665,2665,2666,2666,2667,2668,2668,2669,2669,2670,2670,2671,2672,2672,2673,2673,2674,2674,2675,2676,2676,2677,2677,2678,2679,2679,
                2680,2680,2681,2681,2682,2683,2683,2684,2684,2685,2686,2686,2687,2687,2688,2689,2689,2690,2690,2691,2691,2692,2693,2693,2694,2694,2695,2696,2696,2697,2697,2698,
                2699,2699,2700,2700,2701,2701,2702,2703,2703,2704,2704,2705,2706,2706,2707,2707,2708,2709,2709,2710,2710,2711,2712,2712,2713,2713,2714,2715,2715,2716,2716,2717,
                2718,2718,2719,2719,2720,2720,2721,2722,2722,2723,2723,2724,2725,2725,2726,2726,2727,2728,2728,2729,2729,2730,2731,2731,2732,2732,2733,2734,2734,2735,2735,2736,
                2737,2737,2738,2739,2739,2740,2740,2741,2742,2742,2743,2743,2744,2745,2745,2746,2746,2747,2748,2748,2749,2749,2750,2751,2751,2752,2752,2753,2754,2754,2755,2755,
                2756,2757,2757,2758,2759,2759,2760,2760,2760,2760,2761,2761,2762,2762,2763,2763,2764,2765,2765,2766,2766,2767,2767,2768,2769,2769,2770,2770,2771,2771,2772,2773,
                2773,2774,2774,2775,2775,2776,2776,2777,2778,2778,2779,2779,2780,2780,2781,2782,2782,2783,2783,2784,2784,2785,2785,2786,2787,2787,2788,2788,2789,2789,2790,2790,
                2791,2792,2792,2793,2793,2794,2794,2795,2795,2796,2797,2797,2798,2798,2799,2799,2800,2800,2801,2802,2802,2803,2803,2804,2804,2805,2805,2806,2807,2807,2808,2808,
                2809,2809,2810,2810,2811,2811,2812,2813,2813,2814,2814,2815,2815,2816,2816,2817,2817,2818,2819,2819,2820,2820,2821,2821,2822,2822,2823,2823,2824,2825,2825,2826,
                2826,2827,2827,2828,2828,2829,2829,2830,2830,2831,2832,2832,2833,2833,2834,2834,2835,2835,2836,2836,2837,2837,2838,2839,2839,2840,2840,2841,2841,2842,2842,2843,
                2843,2844,2844,2845,2845,2846,2847,2847,2848,2848,2849,2849,2850,2850,2851,2851,2852,2852,2853,2853,2854,2855,2855,2856,2856,2857,2857,2858,2858,2859,2859,2860,
                2860,2861,2861,2862,2862,2863,2863,2864,2865,2865,2866,2866,2867,2867,2868,2868,2869,2869,2870,2870,2871,2871,2872,2872,2873,2873,2874,2874,2875,2875,2876,2876,
                2877,2878,2878,2879,2879,2880,2880,2881,2881,2882,2882,2883,2883,2884,2884,2885,2885,2886,2886,2887,2887,2888,2888,2889,2889,2890,2890,2891,2891,2892,2892,2893,
                2893,2894,2894,2895,2896,2896,2897,2897,2898,2898,2899,2899,2900,2900,2901,2901,2902,2902,2903,2903,2904,2904,2905,2905,2906,2906,2907,2907,2908,2908,2909,2909,
                2910,2910,2911,2911,2912,2912,2913,2913,2914,2914,2915,2915,2916,2916,2917,2917,2918,2918,2919,2919,2920,2920,2921,2921,2922,2922,2923,2923,2924,2924,2925,2925,
                2926,2926,2927,2927,2928,2928,2929,2929,2930,2930,2931,2931,2932,2932,2933,2933,2934,2934,2934,2935,2935,2936,2936,2937,2937,2938,2938,2939,2939,2940,2940,2941,
                2941,2942,2942,2943,2943,2944,2944,2945,2945,2946,2946,2947,2947,2948,2948,2949,2949,2950,2950,2951,2951,2952,2952,2952,2953,2953,2954,2954,2955,2955,2956,2956,
                2957,2957,2958,2958,2959,2959,2960,2960,2961,2961,2962,2962,2963,2963,2963,2964,2964,2965,2965,2966,2966,2967,2967,2968,2968,2969,2969,2970,2970,2971,2971,2972,
                2972,2972,2973,2973,2974,2974,2975,2975,2976,2976,2977,2977,2978,2978,2979,2979,2980,2980,2980,2981,2981,2982,2982,2983,2983,2984,2984,2985,2985,2986,2986,2987,
                2987,2987,2988,2988,2989,2989,2990,2990,2991,2991,2992,2992,2993,2993,2993,2994,2994,2995,2995,2996,2996,2997,2997,2998,2998,2999,2999,2999,3000,3000,3001,3001,
                3002,3002,3003,3003,3004,3004,3004,3005,3005,3006,3006,3007,3007,3008,3008,3009,3009,3009,3010,3010,3011,3011,3012,3012,3013,3013,3014,3014,3014,3015,3015,3016,
                3016,3017,3017,3018,3018,3018,3019,3019,3020,3020,3021,3021,3022,3022,3022,3023,3023,3024,3024,3025,3025,3026,3026,3026,3027,3027,3028,3028,3029,3029,3030,3030,
                3030,3031,3031,3032,3032,3033,3033,3034,3034,3034,3035,3035,3036,3036,3037,3037,3037,3038,3038,3039,3039,3040,3040,3041,3041,3041,3042,3042,3043,3043,3044,3044,
                3044,3045,3045,3046,3046,3047,3047,3047,3048,3048,3049,3049,3050,3050,3050,3051,3051,3052,3052,3053,3053,3053,3054,3054,3055,3055,3056,3056,3056,3057,3057,3058,
                3058,3059,3059,3059,3060,3060,3061,3061,3062,3062,3062,3063,3063,3064,3064,3065,3065,3065,3066,3066,3067,3067,3067,3068,3068,3069,3069,3070,3070,3070,3071,3071,
                3072,3072,3072,3073,3073,3074,3074,3075,3075,3075,3076,3076,3077,3077,3077,3078,3078,3079,3079,3080,3080,3080,3081,3081,3082,3082,3082,3083,3083,3084,3084,3084,
                3085,3085,3086,3086,3087,3087,3087,3088,3088,3089,3089,3089,3090,3090,3091,3091,3091,3092,3092,3093,3093,3093,3094,3094,3095,3095,3095,3096,3096,3097,3097,3097,
                3098,3098,3099,3099,3100,3100,3100,3101,3101,3101,3102,3102,3102,3103,3103,3103,3104,3104,3104,3105,3105,3105,3106,3106,3106,3107,3107,3107,3108,3108,3108,3109,
                3109,3109,3110,3110,3110,3111,3111,3111,3112,3112,3112,3113,3113,3113,3114,3114,3114,3115,3115,3115,3116,3116,3116,3117,3117,3117,3118,3118,3118,3119,3119,3120,
                3120,3120,3121,3121,3121,3122,3122,3122,3123,3123,3123,3124,3124,3124,3125,3125,3125,3126,3126,3126,3127,3127,3128,3128,3128,3129,3129,3129,3130,3130,3130,3131,
                3131,3131,3132,3132,3132,3133,3133,3133,3134,3134,3135,3135,3135,3136,3136,3136,3137,3137,3137,3138,3138,3138,3139,3139,3139,3140,3140,3141,3141,3141,3142,3142,
                3142,3143,3143,3143,3144,3144,3144,3145,3145,3146,3146,3146,3147,3147,3147,3148,3148,3148,3149,3149,3149,3150,3150,3151,3151,3151,3152,3152,3152,3153,3153,3153,
                3154,3154,3155,3155,3155,3156,3156,3156,3157,3157,3157,3158,3158,3159,3159,3159,3160,3160,3160,3161,3161,3161,3162,3162,3163,3163,3163,3164,3164,3164,3165,3165,
                3166,3166,3166,3167,3167,3167,3168,3168,3168,3169,3169,3170,3170,3170,3171,3171,3171,3172,3172,3173,3173,3173,3174,3174,3174,3175,3175,3176,3176,3176,3177,3177,
                3177,3178,3178,3179,3179,3179,3180,3180,3180,3181,3181,3182,3182,3182,3183,3183,3183,3184,3184,3185,3185,3185,3186,3186,3186,3187,3187,3188,3188,3188,3189,3189,
                3189,3190,3190,3191,3191,3191,3192,3192,3192,3193,3193,3194,3194,3194,3195,3195,3196,3196,3196,3197,3197,3197,3198,3198,3199,3199,3199,3200,3200,3201,3201,3201,
                3202,3202,3202,3203,3203,3204,3204,3204,3205,3205,3206,3206,3206,3207,3207,3207,3208,3208,3209,3209,3209,3210,3210,3211,3211,3211,3212,3212,3213,3213,3213,3214,
                3214,3215,3215,3215,3216,3216,3216,3217,3217,3218,3218,3218,3219,3219,3220,3220,3220,3221,3221,3222,3222,3222,3223,3223,3224,3224,3224,3225,3225,3226,3226,3226,
                3227,3227,3228,3228,3228,3229,3229,3230,3230,3230,3231,3231,3232,3232,3232,3233,3233,3234,3234,3234,3235,3235,3236,3236,3236,3237,3237,3238,3238,3238,3239,3239,
                3240,3240,3240,3241,3241,3242,3242,3242,3243,3243,3244,3244,3244,3245,3245,3246,3246,3246,3247,3247,3248,3248,3248,3249,3249,3250,3250,3250,3251,3251,3252,3252,
                3253,3253,3253,3254,3254,3255,3255,3255,3256,3256,3257,3257,3257,3258,3258,3259,3259,3259,3260,3260,3261,3261,3262,3262,3262,3263,3263,3264,3264,3264,3265,3265,
                3266,3266,3267,3267,3267,3268,3268,3269,3269,3269,3270,3270,3271,3271,3272,3272,3272,3273,3273,3274,3274,3274,3275,3275,3276,3276,3277,3277,3277,3278,3278,3279,
                3279,3279,3280,3280,3281,3281,3282,3282,3282,3283,3283,3284,3284,3285,3285,3285,3286,3286,3287,3287,3287,3288,3288,3289,3289,3290,3290,3290,3291,3291,3292,3292,
                3293,3293,3293,3294,3294,3295,3295,3296,3296,3296,3297,3297,3298,3298,3299,3299,3299,3300,3300,3301,3301,3302,3302,3302,3303,3303,3304,3304,3305,3305,3305,3306,
                3306,3307,3307,3308,3308,3308,3309,3309,3310,3310,3311,3311,3312,3312,3312,3313,3313,3314,3314,3315,3315,3315,3316,3316,3317,3317,3318,3318,3318,3319,3319,3320,
                3320,3321,3321,3322,3322,3322,3323,3323,3324,3324,3325,3325,3325,3326,3326,3327,3327,3328,3328,3329,3329,3329,3330,3330,3331,3331,3332,3332,3333,3333,3333,3334,
                3334,3335,3335,3336,3336,3337,3337,3337,3338,3338,3339,3339,3340,3340,3341,3341,3341,3342,3342,3343,3343,3344,3344,3345,3345,3345,3346,3346,3347,3347,3348,3348,
                3349,3349,3350,3350,3350,3351,3351,3352,3352,3353,3353,3354,3354,3354,3355,3355,3356,3356,3357,3357,3358,3358,3359,3359,3359,3360,3360,3361,3361,3362,3362,3363,
                3363,3364,3364,3364,3365,3365,3366,3366,3367,3367,3368,3368,3369,3369,3369,3370,3370,3371,3371,3372,3372,3373,3373,3374,3374,3375,3375,3375,3376,3376,3377,3377,
                3378,3378,3379,3379,3380,3380,3380,3381,3381,3382,3382,3383,3383,3384,3384,3385,3385,3386,3386,3387,3387,3387,3388,3388,3389,3389,3390,3390,3391,3391,3392,3392,
                3393,3393,3393,3394,3394,3395,3395,3396,3396,3397,3397,3398,3398,3399,3399,3400,3400,3401,3401,3401,3402,3402,3403,3403,3404,3404,3405,3405,3406,3406,3407,3407,
                3408,3408,3409,3409,3409,3410,3410,3411,3411,3412,3412,3413,3413,3414,3414,3415,3415,3416,3416,3417,3417,3418,3418,3418,3419,3419,3420,3420,3421,3421,3422,3422,
                3423,3423,3424,3424,3425,3425,3426,3426,3427,3427,3428,3428,3429,3429,3430,3430,3430,3431,3431,3432,3432,3433,3433,3434,3434,3435,3435,3436,3436,3437,3437,3438,
                3438,3439,3439,3440,3440,3441,3441,3442,3442,3443,3443,3444,3444,3444,3445,3445,3446,3446,3447,3447,3448,3448,3449,3449,3450,3450,3451,3451,3452,3452,3453,3453,
                3454,3454,3455,3455,3456,3456,3457,3457,3458,3458,3459,3459,3460,3460,3461,3461,3462,3462,3463,3463,3464,3464,3465,3465,3466,3466,3467,3467,3468,3468,3469,3469,
                3470,3470,3471,3471,3472,3472,3473,3473,3460,3461,3461,3462,3462,3462,3463,3463,3464,3464,3464,3465,3465,3466,3466,3466,3467,3467,3468,3468,3469,3469,3469,3470,
                3470,3471,3471,3471,3472,3472,3473,3473,3474,3474,3475,3475,3475,3476,3476,3477,3477,3478,3478,3479,3479,3479,3480,3480,3481,3481,3482,3482,3483,3483,3484,3484,
                3485,3485,3485,3486,3486,3487,3487,3488,3488,3489,3489,3490,3490,3491,3491,3492,3492,3493,3493,3494,3494,3495,3495,3496,3496,3497,3497,3498,3498,3499,3499,3500,
                3500,3501,3501,3502,3502,3503,3504,3504,3505,3505,3506,3506,3507,3507,3508,3508,3509,3509,3510,3511,3511,3512,3512,3513,3513,3514,3514,3515,3516,3516,3517,3517,
                3518,3518,3519,3519,3520,3521,3521,3522,3522,3523,3524,3524,3525,3525,3526,3526,3527,3528,3528,3529,3529,3530,3531,3531,3532,3532,3533,3534,3534,3535,3535,3536,
                3537,3537,3538,3539,3539,3540,3540,3541,3542,3542,3543,3544,3544,3545,3545,3546,3547,3547,3548,3549,3549,3550,3551,3551,3552,3553,3553,3554,3555,3555,3556,3557,
                3557,3558,3558,3559,3560,3561,3561,3562,3563,3563,3564,3565,3565,3566,3567,3567,3568,3569,3569,3570,3571,3571,3572,3573,3574,3574,3575,3576,3576,3577,3578,3578,
                3579,3580,3581,3581,3582,3583,3583,3584,3585,3586,3586,3587,3588,3589,3589,3590,3591,3591,3592,3593,3594,3594,3595,3596,3597,3597,3598,3599,3600,3600,3601,3602,
                3603,3603,3604,3605,3606,3607,3607,3608,3609,3610,3610,3611,3612,3613,3614,3614,3615,3616,3617,3617,3618,3619,3620,3621,3621,3622,3623,3624,3625,3625,3626,3627,
                3628,3629,3629,3630,3631,3632,3633,3633,3634,3635,3636,3637,3638,3638,3639,3640,3641,3642,3643,3643,3644,3645,3646,3647,3648,3648,3649,3650,3651,3652,3653,3654,
                3654,3655,3656,3657,3658,3659,3660,3660,3661,3662,3663,3664,3665,3666,3667,3667,3668,3669,3670,3671,3672,3673,3674,3675,3675,3676,3677,3678,3679,3680,3681,3682,
                3683,3683,3684,3685,3686,3687,3688,3689,3690,3691,3692,3693,3694,3694,3695,3696,3697,3698,3699,3700,3701,3702,3703,3704,3705,3706,3707,3708,3708,3709,3710,3711,
                3712,3713,3714,3715,3716,3717,3718,3719,3720,3721,3722,3723,3724,3725,3726,3727,3728,3729,3730,3731,3732,3733,3734,3735,3736,3737,3738,3739,3740,3741,3742,3743,
                3744,3745,3746,3747,3748,3749,3750,3751,3752,3753,3754,3755,3756,3757,3758,3759,3760,3761,3762,3763,3764,3765,3766,3767,3768,3769,3770,3771,3772,3773,3774,3775,
                3776,3777,3778,3779,3781,3782,3783,3784,3785,3786,3787,3788,3789,3790,3791,3792,3793,3794,3795,3796,3798,3799,3800,3801,3802,3803,3804,3805,3806,3807,3808,3810,
                3811,3812,3813,3814,3815,3816,3817,3818,3819,3821,3822,3823,3824,3825,3826,3827,3828,3829,3831,3832,3833,3834,3835,3836,3837,3839,3840,3841,3842,3843,3844,3845,
                3846,3848,3849,3850,3851,3852,3853,3855,3856,3857,3858,3859,3860,3861,3863,3864,3865,3866,3867,3868,3870,3871,3872,3873,3874,3876,3877,3878,3879,3880,3881,3883,
                3884,3885,3886,3887,3889,3890,3891,3892,3893,3895,3896,3897,3898,3899,3901,3902,3903,3904,3906,3907,3908,3909,3910,3912,3913,3914,3915,3917,3918,3919,3920,3922,
                3923,3924,3925,3927,3928,3929,3930,3932,3933,3934,3935,3937,3938,3939,3940,3942,3943,3944,3945,3947,3948,3949,3950,3952,3953,3954,3956,3957,3958,3959,3961,3962,
                3963,3965,3966,3967,3968,3970,3971,3972,3974,3975,3976,3978,3979,3980,3981,3983,3984,3985,3987,3988,3989,3991,3992,3993,3995,3996,3997,3999,4000,4001,4003,4004,
                4005,4007,4008,4009,4011,4012,4013,4015,4016,4017,4019,4020,4021,4023,4024,4025,4027,4028,4030,4031,4032,4034,4035,4036,4038,4039,4041,4042,4043,4045,4046,4047,
                4049,4050,4052,4053,4054,4056,4057,4059,4060,4061,4063,4064,4066,4067,4068,4070,4071,4073,4074,4075,4077,4078,4080,4081,4082,4084,4085,4087,4088,4090,4091,4093,
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
             350,
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
             0,//ltm_dark_fork
             0,//ltm_dark_spear
             0,//ltm_smooth_noise_erode_area
             0,//ltm_smooth_filter_area
             0,//soft_sample_rate
          },
          /* para 1 */
          {
             1,//enable_cdf_smooth
             0.005,//cut_ration
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
             0.004,//cut_ration
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
             0.004,//cut_ration
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
                1.1,//saturation
                0,//hue
             },
             /* awb para 1 */
             {
                1.1,//saturation
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
          /* aec trigger 1 */
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
          /* aec trigger 2 */
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
                0.65,//saturation
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
             7,
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
             800,
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
                -2,-6,-14,-18,//bpf(1,1:4)
                -6,-29,-70,-99,//bpf(2,1:4)
                -14,-70,-130,-28,//bpf(3,1:4)
                -18,-99,-28,1944,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             90,//strength_pos_edge
             90,//strength_neg_edge
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
             {0,2,4,6,8,10,12,14,16,18,20,24,28,30,34,40,48,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             120,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {16,32,48,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//luma_weight[64]
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
                -2,-7,-16,-20,//bpf(1,1:4)
                -7,-33,-78,-107,//bpf(2,1:4)
                -16,-78,-127,51,//bpf(3,1:4)
                -20,-107,51,1760,//bpf(4,1:4)
             },
             {
                -54,-186,-286,-286,//hpf(1,1:4)
                -114,-286,166,694,//hpf(2,1:4)
                -146,-286,694,1700,//hpf(3,1:4)
             },
             80,//strength_pos_edge
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
             {1,1,2,3,4,5,5,5,8,11,16,19,24,29,34,39,46,51,53,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             140,//adjust_strength
             15,//coef_moving_c00
             32,//coef_moving_c01
             32,//coef_moving_c10
             68,//coef_moving_c11
             {16,32,48,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {1,2,8,11,13,15,18,19,21,22,22,22,23,24,26,27,30,32,35,39,46,55,60,62,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             70,//strength_pos_edge
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
             {0,0,0,0,0,0,0,8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             180,//adjust_strength
             28,//coef_moving_c00
             29,//coef_moving_c01
             29,//coef_moving_c10
             28,//coef_moving_c11
             {16,28,40,52,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,10,12,15,18,21,22,26,30,33,35,37,42,46,50,57,61,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             60,//strength_pos_edge
             60,//strength_neg_edge
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
             {16,28,40,52,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,4,8,9,12,14,17,18,22,23,26,30,35,38,41,45,52,55,58,60,63,63,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             40,//strength_pos_edge
             45,//strength_neg_edge
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
             {0,0,0,0,0,0,0,6,7,9,11,14,15,19,21,22,23,26,29,31,36,43,49,57,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             200,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {16,28,40,52,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,5,8,9,13,14,16,19,20,22,24,26,30,35,39,43,46,50,54,58,62,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             60,//strength_pos_edge
             60,//strength_neg_edge
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
             220,//adjust_strength
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
             {0,0,0,0,0,0,0,4,12,17,19,22,22,22,23,23,24,26,29,29,33,34,32,32,34,33,30,31,31,32,32,31,32,35,41,50,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             250,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {30,38,46,54,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,6,9,10,14,18,21,23,23,26,26,27,26,26,28,28,28,28,28,28,30,30,34,40,44,50,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             20,//strength_pos_edge
             20,//strength_neg_edge
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
             {2,5,7,10,14,16,18,20,24,26,29,33,35,37,40,43,46,47,49,52,55,59,61,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             20,//strength_pos_edge
             20,//strength_neg_edge
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
             {2,6,8,10,13,14,16,19,23,26,29,31,34,37,40,42,45,47,48,51,53,58,60,61,63,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             10,//strength_pos_edge
             10,//strength_neg_edge
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
             {0,0,0,0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//edge_w[64]
             0,//limitation_for_move_strength
             250,//adjust_strength
             24,//coef_moving_c00
             30,//coef_moving_c01
             30,//coef_moving_c10
             40,//coef_moving_c11
             {8,16,24,32,40,48,56,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},//shrink_weight[64]
             {2,6,9,13,17,20,23,25,28,30,33,36,38,40,42,44,46,48,51,54,58,63,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,56,48,36,24,12},//luma_weight[64]
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
             800,
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
             200.1,
          },
          /* aec trigger 8 */
          {
             256,
             350.1,
          },
          /* aec trigger 9 */
          {
             512,
             512.1,
          },
          /* aec trigger 10 */
          {
             800,
             1000.1,
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
             13,//nr3d_mpy_th1
             26,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             13,//nr3d_mpc_th1
             26,//nr3d_mpc_th2
             3,//nr3d_lamda2d
             7,//nr3d_ite
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
             14,//nr3d_ite
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
             4,//dbk_gaus_y_c11
             17,//dbk_gaus_y_c12
             28,//dbk_gaus_y_c13
             6,//dbk_gaus_y_c21
             48,//dbk_gaus_y_c23
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
             0,//disable_1st_media_filter
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
             50,//nr3d_mpy_th1
             120,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             90,//nr3d_npc_th3
             180,//nr3d_npc_th4
             50,//nr3d_mpc_th1
             100,//nr3d_mpc_th2
             10,//nr3d_lamda2d
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
             0,//disable_1st_media_filter
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
             24,//nr3d_ite
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
             80,//nr3d_mpy_th1
             150,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             50,//nr3d_mpc_th1
             100,//nr3d_mpc_th2
             25,//nr3d_lamda2d
             24,//nr3d_ite
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
                0,0,0,1,2,3,4,8,8,8,8,9,10,11,12,12,13,14,15,16,16,16,17,17,17,18,18,18,19,19,19,20,20,21,21,21,22,22,22,24,24,24,26,26,28,28,28,30,30,30,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
             },
             /* noise profile cbcr*/
             {
                0,0,3,3,3,3,3,3,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
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
             50,//nr3d_mpy_th1
             170,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             80,//nr3d_npc_th3
             180,//nr3d_npc_th4
             60,//nr3d_mpc_th1
             160,//nr3d_mpc_th2
             30,//nr3d_lamda2d
             35,//nr3d_ite
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
                0,0,0,0,2,2,3,3,4,5,6,8,8,10,10,10,11,12,13,14,15,16,17,18,19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,4,4,4,4,4,6,8,11,13,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
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
             140,//nr3d_mpy_th1
             180,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             140,//nr3d_mpc_th1
             180,//nr3d_mpc_th2
             30,//nr3d_lamda2d
             38,//nr3d_ite
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
                0,0,0,0,0,0,0,0,0,1,1,1,1,2,3,4,5,8,9,9,10,10,10,11,12,13,14,15,16,16,16,16,16,17,18,18,18,18,19,19,19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
             },
             /* noise profile cbcr*/
             {
                0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
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
             30,//nr3d_lamda2d
             38,//nr3d_ite
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
             200,//nr3d_mpy_th1
             240,//nr3d_mpy_th2
             30,//nr3d_npc_th1
             60,//nr3d_npc_th2
             150,//nr3d_npc_th3
             180,//nr3d_npc_th4
             200,//nr3d_mpc_th1
             240,//nr3d_mpc_th2
             30,//nr3d_lamda2d
             38,//nr3d_ite
             0,//nr3d_msr
             248,//nr3d_decay
             30,//nr3d_satu
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
             61,//dbk_v_mv_diff
             90,//dbk_v_mv_str
             100,//dbk_h_yy_flat
             90,//dbk_h_yy_diff
             100,//dbk_v_yy_flat
             90,//dbk_v_yy_diff
             100,//dbk_satu
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
                0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
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
                0.9,//saturation
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
      225,
      /* ev0_count */
      225,
      /* ev2_count */
      225,
      /* fix_fps_index */
      25,
      /* enable_isp_digital_gain */
      0,
      /* max_sensor_gain */
      10,
      /* max_isp_gain1 */
      1,
      /* exp_table */
      {
      //1,102
      //10,102
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
         {    261,    102   },         /* Gain = 1.019531 Exposure Index = 149 */
         {    269,    102   },         /* Gain = 1.050781 Exposure Index = 150 */
         {    278,    102   },         /* Gain = 1.085938 Exposure Index = 151 */
         {    287,    102   },         /* Gain = 1.121094 Exposure Index = 152 */
         {    296,    102   },         /* Gain = 1.156250 Exposure Index = 153 */
         {    305,    102   },         /* Gain = 1.191406 Exposure Index = 154 */
         {    315,    102   },         /* Gain = 1.230469 Exposure Index = 155 */
         {    325,    102   },         /* Gain = 1.269531 Exposure Index = 156 */
         {    335,    102   },         /* Gain = 1.308594 Exposure Index = 157 */
         {    346,    102   },         /* Gain = 1.351562 Exposure Index = 158 */
         {    357,    102   },         /* Gain = 1.394531 Exposure Index = 159 */
         {    368,    102   },         /* Gain = 1.437500 Exposure Index = 160 */
         {    380,    102   },         /* Gain = 1.484375 Exposure Index = 161 */
         {    392,    102   },         /* Gain = 1.531250 Exposure Index = 162 */
         {    404,    102   },         /* Gain = 1.578125 Exposure Index = 163 */
         {    417,    102   },         /* Gain = 1.628906 Exposure Index = 164 */
         {    430,    102   },         /* Gain = 1.679688 Exposure Index = 165 */
         {    443,    102   },         /* Gain = 1.730469 Exposure Index = 166 */
         {    457,    102   },         /* Gain = 1.785156 Exposure Index = 167 */
         {    471,    102   },         /* Gain = 1.839844 Exposure Index = 168 */
         {    486,    102   },         /* Gain = 1.898438 Exposure Index = 169 */
         {    501,    102   },         /* Gain = 1.957031 Exposure Index = 170 */
         {    517,    102   },         /* Gain = 2.019531 Exposure Index = 171 */
         {    533,    102   },         /* Gain = 2.082031 Exposure Index = 172 */
         {    549,    102   },         /* Gain = 2.144531 Exposure Index = 173 */
         {    566,    102   },         /* Gain = 2.210938 Exposure Index = 174 */
         {    583,    102   },         /* Gain = 2.277344 Exposure Index = 175 */
         {    601,    102   },         /* Gain = 2.347656 Exposure Index = 176 */
         {    620,    102   },         /* Gain = 2.421875 Exposure Index = 177 */
         {    639,    102   },         /* Gain = 2.496094 Exposure Index = 178 */
         {    659,    102   },         /* Gain = 2.574219 Exposure Index = 179 */
         {    679,    102   },         /* Gain = 2.652344 Exposure Index = 180 */
         {    700,    102   },         /* Gain = 2.734375 Exposure Index = 181 */
         {    721,    102   },         /* Gain = 2.816406 Exposure Index = 182 */
         {    743,    102   },         /* Gain = 2.902344 Exposure Index = 183 */
         {    766,    102   },         /* Gain = 2.992188 Exposure Index = 184 */
         {    789,    102   },         /* Gain = 3.082031 Exposure Index = 185 */
         {    813,    102   },         /* Gain = 3.175781 Exposure Index = 186 */
         {    838,    102   },         /* Gain = 3.273438 Exposure Index = 187 */
         {    864,    102   },         /* Gain = 3.375000 Exposure Index = 188 */
         {    890,    102   },         /* Gain = 3.476562 Exposure Index = 189 */
         {    917,    102   },         /* Gain = 3.582031 Exposure Index = 190 */
         {    945,    102   },         /* Gain = 3.691406 Exposure Index = 191 */
         {    974,    102   },         /* Gain = 3.804688 Exposure Index = 192 */
         {   1004,    102   },         /* Gain = 3.921875 Exposure Index = 193 */
         {   1035,    102   },         /* Gain = 4.042969 Exposure Index = 194 */
         {   1067,    102   },         /* Gain = 4.167969 Exposure Index = 195 */
         {   1100,    102   },         /* Gain = 4.296875 Exposure Index = 196 */
         {   1133,    102   },         /* Gain = 4.425781 Exposure Index = 197 */
         {   1167,    102   },         /* Gain = 4.558594 Exposure Index = 198 */
         {   1203,    102   },         /* Gain = 4.699219 Exposure Index = 199 */
         {   1240,    102   },         /* Gain = 4.843750 Exposure Index = 200 */
         {   1278,    102   },         /* Gain = 4.992188 Exposure Index = 201 */
         {   1317,    102   },         /* Gain = 5.144531 Exposure Index = 202 */
         {   1357,    102   },         /* Gain = 5.300781 Exposure Index = 203 */
         {   1398,    102   },         /* Gain = 5.460938 Exposure Index = 204 */
         {   1440,    102   },         /* Gain = 5.625000 Exposure Index = 205 */
         {   1484,    102   },         /* Gain = 5.796875 Exposure Index = 206 */
         {   1529,    102   },         /* Gain = 5.972656 Exposure Index = 207 */
         {   1575,    102   },         /* Gain = 6.152344 Exposure Index = 208 */
         {   1623,    102   },         /* Gain = 6.339844 Exposure Index = 209 */
         {   1672,    102   },         /* Gain = 6.531250 Exposure Index = 210 */
         {   1723,    102   },         /* Gain = 6.730469 Exposure Index = 211 */
         {   1775,    102   },         /* Gain = 6.933594 Exposure Index = 212 */
         {   1829,    102   },         /* Gain = 7.144531 Exposure Index = 213 */
         {   1884,    102   },         /* Gain = 7.359375 Exposure Index = 214 */
         {   1941,    102   },         /* Gain = 7.582031 Exposure Index = 215 */
         {   2000,    102   },         /* Gain = 7.812500 Exposure Index = 216 */
         {   2060,    102   },         /* Gain = 8.046875 Exposure Index = 217 */
         {   2122,    102   },         /* Gain = 8.289062 Exposure Index = 218 */
         {   2186,    102   },         /* Gain = 8.539062 Exposure Index = 219 */
         {   2252,    102   },         /* Gain = 8.796875 Exposure Index = 220 */
         {   2320,    102   },         /* Gain = 9.062500 Exposure Index = 221 */
         {   2390,    102   },         /* Gain = 9.335938 Exposure Index = 222 */
         {   2462,    102   },         /* Gain = 9.617188 Exposure Index = 223 */
         {   2536,    102   },         /* Gain = 9.906250 Exposure Index = 224 */
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
         30,/* outdoor_luma_target_compensated */
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
            {180,30},
            {200,28},
            {230,26},
            {280,24},
            {300,22},
            {330,20},
            {360,20},
            {430,20},
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
         0,//bright_dark_en
         4,//bright_dark_count
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
               1.5,//dark_th_low_w
               1.5,//dark_th_high_w
               3,//bright_th_low_w
               3,//bright_th_high_w
            },
            {
               280,//lux_index
               15,//dark_th_low
               30,//dark_th_high
               220,//bright_th_low
               240,//bright_th_high
               1,//dark_th_low_w
               1,//dark_th_high_w
               1.5,//bright_th_low_w
               1.5,//bright_th_high_w
            },
            {
               300,//lux_index
               15,//dark_th_low
               30,//dark_th_high
               220,//bright_th_low
               230,//bright_th_high
               1,//dark_th_low_w
               1,//dark_th_high_w
               1.5,//bright_th_low_w
               1.5,//bright_th_high_w
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
      { 0.4020, 0.5901 },   /* d75 */
      { 0.4251, 0.5368 },   /* d65 */
      { 0.4967, 0.4776 },   /* d50 */
      { 0.4967, 0.4776 },   /* noon */
      { 0.5234, 0.3385 },   /* cw */
      { 0.6202, 0.3461 },   /* tl84 */
      { 0.6993, 0.2919 },   /* a */
      { 0.8242, 0.2535 },   /* h */
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
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
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
               4,
               4,
               4,
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
               1,
               1,
               1,
               1,
               1,
               1,
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
               4,
               4,
               4,
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
               1,
               1,
               1,
               1,
               1,
               1,
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
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
               1,
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
   0,//enable_init_point
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
      1,//misleading.enable
      1,//misleading.aec_count
      /* misleading region */
      {
         /* misleading region 0 */
         {
             200,//lux_index
             3,//misleading_region_count
             /* awb mesh reigon */
             {
                /* awb mesh region 0 */
                {
                    4,//point_num
                    0,//is_square
                    0.596,//min_x
                    0.275,//min_y
                    0.66,//max_x
                    0.32,//max_y
                    /* awb mesh point */
                    {
                       {0.596,0.32},
                       {0.596,0.31},
                       {0.654,0.275},
                       {0.66,0.292},
                    },
                },
                /* awb mesh region 1 */
                {
                    4,//point_num
                    0,//is_square
                    0.267,//min_x
                    0.281,//min_y
                    0.455,//max_x
                    0.448,//max_y
                    /* awb mesh point */
                    {
                       {0.267,0.311},
                       {0.267,0.281},
                       {0.421,0.448},
                       {0.455,0.403},
                    },
                },
                /* awb mesh region 2 */
                {
                    4,//point_num
                    0,//is_square
                    0.577,//min_x
                    0.293,//min_y
                    0.695,//max_x
                    0.418,//max_y
                    /* awb mesh point */
                    {
                       {0.59,0.418},
                       {0.577,0.375},
                       {0.673,0.293},
                       {0.695,0.325},
                    },
                },
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
      3,//count
      /* awb_gain_adjust_wb_t gain_adj[HIGH_TRIGGER_COUNT] */
      {
         150,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0200 },   /* d75 */
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
         220,//lux_index
         /* awb_gain_adj */
         {
            { 1.0300, 1.0000 },   /* d75 */
            { 1.0100, 1.0200 },   /* d65 */
            { 1.0100, 1.0000 },   /* d50 */
            { 1.0100, 1.0000 },   /* noon */
            { 1.0500, 1.0000 },   /* cw */
            { 1.0500, 1.0000 },   /* tl84 */
            { 1.0000, 1.0000 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 1.0000, 1.0000 },   /* custom1 */
            { 1.0000, 1.0000 },   /* custom2 */
         },
         380,//lux_index
         /* awb_gain_adj */
         {
            { 1.0000, 1.0000 },   /* d75 */
            { 1.0200, 1.0200 },   /* d65 */
            { 1.0200, 1.0200 },   /* d50 */
            { 1.0200, 1.0200 },   /* noon */
            { 1.0500, 1.0000 },   /* cw */
            { 1.0500, 1.0000 },   /* tl84 */
            { 1.0700, 1.0200 },   /* a */
            { 1.0000, 1.0000 },   /* h */
            { 1.0000, 1.0000 },   /* custom1 */
            { 1.0000, 1.0000 },   /* custom2 */
         },
      },
   },
},
{
   /* af version */
   0x3f800000,
   /* af enable */
   1065353216,
   /* infinite */
   1065353216,
   /* near */
   1065353216,
   /* hyperfocal */
   1065353216,
   /* init_skip */
   1065353216,
   /* algo_type: 0-FOCUS_FULL_SWEEP, 1-FOCUS_SINGLE */
   1065353216,
   /* fullswep_algo_tuning_t full_sweep_tuning_pra */
   {
      1065353216,//code_per_step
      1065353216,//skip_count_after_lens_move
   },
   /* af_stats_type 1-AF_STATS_SHARPNES, 2-AF_STATS_FIR, 4-AF_STATS_IIR */
   1065353216,
   /* roi_t roi */
   {
      1,//x
      1,//y
      1,//width
      1,//height
   },
   /* stats_weight_table[AF_BLOCK_ROW*AF_BLOCK_COL] 16x9 */
   {
      1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      2, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 
      3, 3, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 
      3, 3, 3, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 
      3, 3, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      2, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1.4013e-45, 0, 0, 0, 0, 7.17465e-43, 0, 
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   },
   /* actuator_tunning_pra_t atcuator */
   {
      0,//enable
      0,//damping_enable
      0,//init_dac
      0,//infinit_dac
      0,//hyperfocal_dac
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
   16,
   /* run_mode : 0-INIT 1-CAMERA 2-VIDEO 3-SNAPSHOT */
   17,
   /* af_monit_t af_monit */
   {
      /* af_tuning_sad_t af_par_sad */
      {
         18,//enable
         1.4013e-45,//gain_min
         7.17465e-43,//gain_max
         6.89439e-43,//ref_gain_min
         5.77335e-43,//ref_gain_max
         300,//threshold_min
         100,//threshold_max
         212,//ref_threshold_min
         262,//ref_threshold_max
         300,//frames_to_wait
      },
   },
   /* af_tuning_single_t  single_pra */
   {
      /* single_optic_t optics */
      {
         100,//far_zone
         100,//mid_zone
         512,//near_zone
         100,//init_pos
         512,//far_start_pos
         100,//near_start_pos
         512,//TAF_far_end
         300,//TAF_near_end
         200,//CAF_far_end
         100,//CAF_near_end
         50,//srch_rgn_1
         0,//srch_rgn_2
         0,//srch_rgn_3
         0,//fine_srch_rgn
      },
      /* acturator_type: 0-ACT_TYPE_CLOSELOOP, 1-ACE_TYPE_OPENLOOP */
      0,
      /* index[50] */
      {
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
         0, 0, 0, 0, 0, 0, 0, 0, 40, 1058642330, 
         1062836634, 1062836634, 1062836634, 1062836634, 1062836634, 1062836634, 1062836634, 1062836634, 1062836634, 1062836634, 
         1063675494, 1063675494, 1063675494, 1063675494, 1063675494, 1063675494, 1063675494, 1063675494, 1063675494, 1063675494, 
      },
      1062836634,//is_hys_comp_needed
      /* single_threshold_t hw */
      {
         1062836634,//hist_dec_dec_thres
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
            0,
            0,
            0,
         },
         /* inc_dec */
         {
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0.93,
            0.93,
            0.93,
         },
         /* inc_dec_3frame */
         {
            0.93,
            0.93,
            0.93,
            0.93,
            0.93,
            0.93,
            0.93,
            0.98,
            0.98,
            0.98,
         },
         /* dec_dec */
         {
            0.98,
            0.98,
            0.98,
            0.98,
            0.98,
            0.98,
            0.98,
            5.60519e-44,
            0.95,
            1.05,
         },
         /* dec_dec_noise */
         {
            1.3,
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
         /* flat_threshold */
         {
            1000,
            1.4013e-45,
            2.52234e-44,
            2.52234e-44,
            2.52234e-44,
            2.52234e-44,
            2.52234e-44,
            2.38221e-44,
            2.38221e-44,
            2.38221e-44,
         },
         17,//hist_inc_dec_thres
         2.38221e-44,//flat_dec_thres
         2.8026e-45,//flat_inc_thres
         2.8026e-45,//macro_thres
      },
      /* BV_gain[10] */
      {
         2.8026e-45,
         2.8026e-45,
         2.8026e-45,
         4.2039e-45,
         4.2039e-45,
         4.2039e-45,
         4.2039e-45,
         4.2039e-45,
         2.24208e-44,
         2.24208e-44,
      },
      16,//step_index_per_um
      /* step_size_table_t CAF_step_table */
      {
         /* step_rgn_t   Prescan_low_light */
         {
            16, 16, 15, 15, 15,
         },
         /* step_rgn_t   Prescan_normal_light */
         {
            15, 15, 4, 4, 4,
         },
         /* step_rgn_t   Finescan_low_light */
         {
            4, 4, 4, 4, 4,
         },
         /* step_rgn_t   Finescan_normal_light */
         {
            4, 4, 3, 1, 1,
         },
      },
      /* step_size_table_t TAF_step_table */
      {
         /* step_rgn_t   Prescan_low_light */
         {
            1, 1, 0, 0, 0,
         },
         /* step_rgn_t   Prescan_normal_light */
         {
            0, 0, 0, 0, 0,
         },
         /* step_rgn_t   Finescan_low_light */
         {
            8, 8, 8, 8, 2,
         },
         /* step_rgn_t   Finescan_normal_light */
         {
            1, 0, 0, 0, 1,
         },
      },
      /* skip_frame[3] */
      {
         460,//0-large
         0,//1-small
         0,//2-others
      },
   },
   0,//count_ae
   0,//trigger_mode 0-gain, 1-lux
   0,//interpolation_enable
   /* aec_trigger_t aec_trigger[MIN_TRIGGER_COUNT] */
   {
   },
   /* af_stats_tuning_t af_stats_tuning_pra[MIN_TRIGGER_COUNT] */
   {
   },
},
