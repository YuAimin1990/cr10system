//#define ENABALE_ALL_LOG

#include "ge2d_api.h"
#include "ge2d_hw.h"
#include "ar_mmap.h"
#include "ge2d_util.h"
#include "utils_debug.h"

#if defined LOG_TAG
#undef      LOG_TAG
#define     LOG_TAG  "ge2d"
#endif

static inline volatile ge2d_reg_t *get_ge2d_reg()
{
    volatile ge2d_reg_t *reg=(volatile ge2d_reg_t *)ar_dev_pa2va(GE2D_REG_BASE);
    return reg;
}
int ge2d_hw_init()
{
    volatile ge2d_reg_t *p_ge2d=get_ge2d_reg();
    volatile AR_PHYS_ADDR  *cgu_base = CGU_REG_BASE+0x4018;
    
    //first flush all the reginit list to reg space
    //memcpy((void *)p_ge2d,init_list,sizeof(ge2d_reg_t));
    //first on the ge2d clk
    //TODO ar_enter_critical();
    AR_SET_REG_BITS(__REG32__(cgu_base),1,12,12);    // enable display clock
    //AR_SET_REG_BITS(__REG32__(cgu_base),0,16,16);
    //AR_SET_REG_BITS(__REG32__(cgu_base),1,17,17);
    AR_SET_REG_BITS(__REG32__(cgu_base),0,18,18);
    AR_SET_REG_BITS(__REG32__(cgu_base),1,28,28);    // enable GE2D clock
    
    AR_SET_REG_BITS(__REG32__(cgu_base),0,25,25);    // cclk_sw_reset
    AR_SET_REG_BITS(__REG32__(cgu_base),0,26,26);    // pclk_sw_reset
    ar_delay(2);
    AR_SET_REG_BITS(__REG32__(cgu_base),1,25,25);    // cclk_sw_reset
    AR_SET_REG_BITS(__REG32__(cgu_base),1,26,26);    // pclk_sw_reset
    ar_delay(2);
    
    //TODO ar_exit_critical();
    
    //reset all the module
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_00,0x7f,0,6);
    ar_delay(2);
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_00,0x00,0,6);
    //reset the ge2d
    //off the draw timeout
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_11,0,0,31);
    //enable the axi wr requests responded and cfg to no wait dst write
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_12,1,0,0);
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_12,0,2,3);
    //disable range check
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_12,0,1,1);
    //set ddr range
    p_ge2d->GE2D_REG_25=0xffff0000;
    p_ge2d->GE2D_REG_26=0xffff0000;
    p_ge2d->GE2D_REG_27=0xffff0000;
    p_ge2d->GE2D_REG_28=0xffff0000;
    //set axcache to 0 ????
    p_ge2d->GE2D_REG_29=0;
    //AR_SET_REG_BITS(p_ge2d->GE2D_REG_29,0x1,24,24);
    //gate clk to default
    p_ge2d->GE2D_REG_30=0x20000000;  /* must disable low power mode otherwise ge2d would hang.*/
    p_ge2d->GE2D_REG_46=0;
#ifdef GE2D_INTER_FACE2
    //enable the ge2d int
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_35,0x00,0,15);
    //clear the int 	queue
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_37,0x1,0,0);
    //clear the cmd queue
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_39,0x1,0,0);
    //clear the ret queue
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_39,0x1,3,3);
#else
    //enable the ge2d int
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_02,0x00,0,15);
    //clear the int	   queue
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_04,0x1,0,0);
    //clear the cmd queue
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_06,0x1,0,0);
    //clear the ret queue
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_06,0x1,3,3);
#endif
    return 0;
}

int ge2d_hw_start_frame(frame_cmd_t *p_frame)
{
    volatile ge2d_reg_t *p_ge2d=get_ge2d_reg();
    
    //log_tag_info("frame_index=%d",p_frame->frame_index);
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_47,p_frame->th,0,7);
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_47,1,16,16);
    AR_SET_REG_BITS(p_ge2d->GE2D_REG_47,100,8,15);
#ifdef GE2D_INTER_FACE2
    p_ge2d->GE2D_REG_32=(uint32_t)(p_frame->phy_orgin_addr + (p_frame->frame_start_addr - (uint32_t *)p_frame));
    p_ge2d->GE2D_REG_33=(uint32_t)(p_frame->phy_orgin_addr + (p_frame->frame_end_addr - (uint32_t *)p_frame));
    p_ge2d->GE2D_REG_34=(uint32_t)p_frame->frame_index;
    //AR_SET_REG_BITS(p_ge2d->GE2D_REG_39,0x1,1,1);
#else
    p_ge2d->GE2D_REG_07=(uint32_t)(p_frame->phy_orgin_addr + (p_frame->frame_start_addr - (uint32_t *)p_frame));
    p_ge2d->GE2D_REG_08=(uint32_t)(p_frame->phy_orgin_addr + (p_frame->frame_end_addr - (uint32_t *)p_frame));
    p_ge2d->GE2D_REG_09=(uint32_t)p_frame->frame_index;
    //AR_SET_REG_BITS(p_ge2d->GE2D_REG_06,0x1,1,1);
#endif
    return 0;
}
int clear_irq_force_for_debug()
{
    volatile ge2d_reg_t *p_ge2d=get_ge2d_reg();

#ifdef GE2D_INTER_FACE2
    uint32_t status=p_ge2d->GE2D_REG_36;
    p_ge2d->GE2D_REG_36=status;
#else
    uint32_t status=p_ge2d->GE2D_REG_03;
    p_ge2d->GE2D_REG_03=status;
#endif
    return 0;
}

int ge2d_hw_get_frame_index()
{
    volatile ge2d_reg_t *p_ge2d=get_ge2d_reg();
    int buf_index=0;

#ifdef GE2D_INTER_FACE2
    uint32_t status=p_ge2d->GE2D_REG_36;
    p_ge2d->GE2D_REG_36=status;
    buf_index=p_ge2d->GE2D_REG_31;
    //log_tag_info("interface2 status=0x%x buf_index=%d",status,buf_index);
    if(status&GE2D_INT_BUFFER_DONE_INT){
        if(buf_index&(1<<8))
        {
           log_tag_err("err index");
           return -1;
        }else{
           return buf_index&0xff;
        }
    }
    if(status&GE2D_ERR)
    {
        log_tag_err("0x%x",status);
    }
#else
    uint32_t status=p_ge2d->GE2D_REG_03;
    p_ge2d->GE2D_REG_03=status;
    buf_index=p_ge2d->GE2D_REG_24; 
    //log_tag_info("interface1 status=0x%x buf_index=%d",status,buf_index);
    if(status&GE2D_INT_BUFFER_DONE_INT){
    // 	uint32_t reg_val=0;
        if(buf_index&(1<<8))
        {
           log_tag_err("err index");
           return -1;
        }else{
           return buf_index&0xff;
        }
    }
    if(status&GE2D_ERR)
    {
      log_tag_err("0x%x",status);
    }
#endif
	return -1;
}

#define READ_DBG_INFO( A ) \
        do { \
            write_reg32(GE2D_REG_BASE + 0x50, A); \
          dbg_info = read_reg32(GE2D_REG_BASE + 0x54); \
        } while(0);

#if 0
void ge2d_hw_info_show(void)
{
    unsigned int dbg_info;

    READ_DBG_INFO( 0x30 );
    ar_always(" DebugInfoSel: 0x30 ispr_debug_info                               \n");
    ar_always("   [31:0] : ISPR State (Useless) (Always 0s)                    %d\n",  dbg_info   );

    READ_DBG_INFO( 0x00 );
    ar_always(" DebugInfoSel: 0x00 dline_debug_info                              \n");
    ar_always("   [2:0] :  dline state.                                        %d\n",  ((dbg_info>>0)&0x7) );
    ar_always("            0: IDLE (Default)                                     \n");
    ar_always("            1: CYC0                                               \n");
    ar_always("            2: REQI: request block32x16                           \n");
    ar_always("            3: WAIT: run one block32x16                           \n");
    ar_always("            4: RELI: release block32x16                           \n");

    READ_DBG_INFO( 0x13 );
    ar_always(" DebugInfoSel: 0x13 dbmp_map_mgr_top_debug_info                   \n");
    ar_always("   [0:0]: state                                                 %d\n",  ((dbg_info>>0)&0x1) );
    ar_always("           0: IDLE                                                \n");
    ar_always("           1: RUN                                                 \n");

    READ_DBG_INFO( 0x10 );
    ar_always(" DebugInfoSel: 0x10 dbmp_map_buf_ldr_debug_info                   \n");
    ar_always("   [0:0] : dbmp_map_buf_ldr_state                               %d\n",  ((dbg_info>>0)&0x1) );
    ar_always("           0: IDLE                                                \n");
    ar_always("           1: RUN : draw bmp is running                           \n");
    ar_always("   [5:4] : dbmp_map_buf_ldr_req_state                           %d\n",  ((dbg_info>>4)&0x3) );
    ar_always("           0: REQ_IDLE                                            \n");
    ar_always("           1: REQ_TOP : request top map                           \n");
    ar_always("           2: REQ_BOT : request bot map                           \n");
    ar_always("           3: REQ_WAIT: wait request done                         \n");
    ar_always("   [7:6] : dbmp_map_buf_ldr_ret_state                           %d\n",  ((dbg_info>>6)&0x3) );
    ar_always("           0: RET_IDLE :                                          \n");
    ar_always("           1: RET_TOP  : wait axi return top map data             \n");
    ar_always("           2: RET_BOT  : wait axi return bot map data             \n");
    ar_always("   [15:8] : dbmp_map_buf_ldr_ver_16_cnt (0)                     %d\n",  ((dbg_info>>8)&0xFF) );
    ar_always("   [19:16]: dbmp_map_buf_ldr_hor_bl_cnt (0)                     %d\n",  ((dbg_info>>16)&0xF) );

    READ_DBG_INFO( 0x11 );
    ar_always(" DebugInfoSel: 0x11 dbmp_map_buf_ldr_debug_info                   \n");
    ar_always("   [3:0] : map_buf_rdptr                                        %d\n",  ((dbg_info>>0)&0xF) );
    ar_always("   [7:4] : map_buf_wrptr                                        %d\n",  ((dbg_info>>4)&0xF) );

    READ_DBG_INFO( 0x12 );
    ar_always(" DebugInfoSel: 0x12: dbmp_blk_map_gen_debug_info                  \n");
    ar_always("   [8:0] : ver_blk_cnt( rotation: blk16x16, blk32x8 )           %d\n",  ((dbg_info>>0)&0x1FF) );
    ar_always("   [17:9]: hor_blk_cnt( rotation: blk16x16, blk32x8 )           %d\n",  ((dbg_info>>9)&0x1FF) );
    ar_always("   [21:20]: state                                               %d\n",  ((dbg_info>>20)&0x3) );
    ar_always("           0: IDLE                                                \n");
    ar_always("           1: WAIT_RDY : blk64x64 ready & blk map request         \n");
    ar_always("           2: PROC_REQ0 : generate left                           \n");
    ar_always("           3: PROC_REQ1 : generate right                          \n");

    READ_DBG_INFO( 0x17 );
    ar_always(" DebugInfoSel: 0x17 dbmp_ldsrc_get_blk_map_debug_info             \n");
    ar_always("   [1:0]: state                                                 %d\n",  ((dbg_info>>0)&0x3) );
    ar_always("           0: IDLE                                                \n");
    ar_always("           1: WAIT_POS_REQ: wait post req blk map                 \n");
    ar_always("           2: WAIT_ONE_BLK: wait one blk map ready                \n");
    ar_always("   [5:4]: blk_map_buf_wrptr                                     %d\n",  ((dbg_info>>4)&0x3) );
    ar_always("   [7:6]: blk_map_buf_rdptr                                     %d\n",  ((dbg_info>>6)&0x3) );
    ar_always("   [15:8]:hor_blk_cnt                                           %d\n",  ((dbg_info>>8)&0xFF) );
    ar_always("   [24:16]:ver_blk_cnt                                          %d\n",  ((dbg_info>>16)&0xFF) );

    READ_DBG_INFO( 0x14 );
    ar_always(" DebugInfoSel: 0x14 dbmp_ldsrc_fifo_debug_info                    \n");
    ar_always("   [0:0] : axi_req_bp_fifo_full                                 %d\n",  ((dbg_info>>0)&0x1) );
    ar_always("   [1:1] : axi_req_bp_fifo_empty                                %d\n",  ((dbg_info>>1)&0x1) );
    ar_always("   [2:2] : axi_ret_fifo_full                                    %d\n",  ((dbg_info>>2)&0x1) );
    ar_always("   [3:3] : axi_ret_fifo_empty                                   %d\n",  ((dbg_info>>3)&0x1) );
    ar_always("   [4:4] : axi_req_fifo_full                                    %d\n",  ((dbg_info>>4)&0x1) );
    ar_always("   [5:5] : axi_req_fifo_emtpy                                   %d\n",  ((dbg_info>>5)&0x1) );


    READ_DBG_INFO( 0x15 );
    ar_always(" DebugInfoSel: 0x15 dbmp_ldsrc_axi_req_debug_info                 \n");
    ar_always("   [1:0] : axi_req_generater_state                              %d\n",  ((dbg_info>>0)&0x3) );
    ar_always("          0: WAIT_PRE_VLD : wait block map valid                  \n");
    ar_always("          1: BILINEAR_MAP : bilinar blk map                       \n");
    ar_always("          2: WAIT_POS_REQ : wait fmt convert ready                \n");
    ar_always("          3: GEN_ONE_BLK  : generate axi req for one block        \n");
    ar_always("   [5:4] : axi_req_sender state                                 %d\n",  ((dbg_info>>4)&0x3) );
    ar_always("          0: WAIT_AXI_REQ : wait line request                     \n");
    ar_always("          1: WAIT_AXI_IDX : wait axi buf idx                      \n");
    ar_always("          2: WAIT_AXI_REQ : send one line request                 \n");
    ar_always("   [7:6] : axi_req_sender send state                            %d\n",  ((dbg_info>>6)&0x3) );
    ar_always("          0: SEND_Y0 req                                          \n");
    ar_always("          1: Send_Y1 req                                          \n");
    ar_always("          2: send_Cb_req                                          \n");
    ar_always("          3: Send_Cr_req                                          \n");
    ar_always("   [10:8] : axi_ret_receiver state                              %d\n",  ((dbg_info>>8)&0x7) );
    ar_always("          0: IDLE                                                 \n");
    ar_always("          1: REC_Y0                                               \n");
    ar_always("          2: REC_Y1                                               \n");
    ar_always("          3: REC_CB                                               \n");
    ar_always("          4: REC_CR                                               \n");


    READ_DBG_INFO( 0x16 );
    ar_always(" DebugInfoSel: 0x16 dbmp_ldsrc_ret_dat_debug_info                 \n");
    ar_always("    [3:0] : ret_dat_fetcher state                               %d\n",  ((dbg_info>>0)&0xF) );
    ar_always("          0: IDLE                                                 \n");
    ar_always("          1: WAIT_POS_REQ: wait draw_core request one block       \n");
    ar_always("          2: WAIT_PRE_RDY: wait block ready                       \n");
    ar_always("          3: WAIT_BUF_EPT: wait buf empty for format convert      \n");
    ar_always("          4: WAIT_RET_VLD: wait return data ready                 \n");
    ar_always("          5: MOVE: move ret data to convert buffer                \n");
    ar_always("          6: WAIT_REL_DONE: release axi buffer                    \n");
    ar_always("          7: WAIT_BLK_DONE: wait release axi buffer done and wait \n");
    ar_always("                            one blk convert done                  \n");
    ar_always("    [5:4] : ret_data_fetcher ft_state                           %d\n",  ((dbg_info>>4)&0x3) );
    ar_always("          0: FETCH_Y0                                             \n");
    ar_always("          1: FETCH_Y1                                             \n");
    ar_always("          2: FETCH_CB                                             \n");
    ar_always("          3: FETCH_CR                                             \n");
    ar_always("    [6:6] : ret_data_fetcher wait_blk_write done state          %d\n",  ((dbg_info>>6)&0x1) );
    ar_always("          0: IDLE                                                 \n");
    ar_always("          1£ºWAIT_DONE                                            \n");
    ar_always("    [7:7] : ret_data_fetcher abi_rel_st                         %d\n",  ((dbg_info>>7)&0x1) );
    ar_always("          0: IDLE                                                 \n");
    ar_always("          1: WAIT_DONE                                            \n");
    ar_always("    [10:8] : ret_buf_rdptr                                      %d\n",  ((dbg_info>>8)&0x3) );
    ar_always("    [14:12]: ret_buf_wrptr                                      %d\n",  ((dbg_info>>12)&0x3) );

    READ_DBG_INFO( 0x20 );
    ar_always(" DebugInfoSel: 0x20: abld_top_debug_info                          \n");
    ar_always("    [2:0] : abld_top state                                      %d\n",  ((dbg_info>>0)&0x7) );
    ar_always("          0£ºAB_IDLE                                              \n");
    ar_always("          1: AB_RUN                                               \n");
    ar_always("          2: AB_WAIT_DST_DONE                                     \n");
    ar_always("          3: AB_WAIT_SRC_DONE                                     \n");
    ar_always("          4: AB_WAIT_ABLD_DONE                                    \n");
    ar_always("                                                                  \n");

    READ_DBG_INFO( 0x21 );
    ar_always(" DebugInfoSel: 0x21: abld_dst_debug_info                          \n");
    ar_always("    [0:0] : state                                                 %d\n",  ((dbg_info>>0)&0x1) );
    ar_always("           0: IDLE                                                  \n");
    ar_always("           1: RUN                                                   \n");
    ar_always("    [3:1] : blk32x16_line_chk state                               %d\n",  ((dbg_info>>1)&0x7) );
    ar_always("           0: IDLE                                                  \n");
    ar_always("           1: RD_MSK                                                \n");
    ar_always("           2: WAIT                                                  \n");
    ar_always("    [5:4] : axi_req_send state                                    %d\n",  ((dbg_info>>4)&0x3) );
    ar_always("          0: WAIT_AXI_REQ : wait line request                     \n");
    ar_always("          1: WAIT_AXI_IDX : wait axi buf idx                      \n");
    ar_always("          2: WAIT_AXI_REQ : send one line request                 \n");
    ar_always("   [7:6] : axi_req_sender send state                            %d\n",  ((dbg_info>>6)&0x3) );
    ar_always("          0: SEND_Y0 req                                          \n");
    ar_always("          1: Send_Y1 req                                          \n");
    ar_always("          2: send_Cb_req                                          \n");
    ar_always("          3: Send_Cr_req                                          \n");
    ar_always("   [10:8] : axi_ret_receiver state                              %d\n",  ((dbg_info>>8)&0x7) );
    ar_always("          0: IDLE                                                 \n");
    ar_always("          1: REC_Y0                                               \n");
    ar_always("          2: REC_Y1                                               \n");
    ar_always("          3: REC_CB                                               \n");
    ar_always("          4: REC_CR                                               \n");
    ar_always("   [15:12] : ret_dat_fetcher state                             %d\n",  ((dbg_info>>12)&0x7) );
    ar_always("          0: IDLE                                                 \n");
    ar_always("          1: WAIT_POS_REQ: wait draw_core request one block       \n");
    ar_always("          2: WAIT_PRE_RDY: wait block ready                       \n");
    ar_always("          3: WAIT_BUF_EPT: wait buf empty for format convert      \n");
    ar_always("          4: WAIT_RET_VLD: wait return data ready                 \n");
    ar_always("          5: MOVE: move ret data to convert buffer                \n");
    ar_always("          6: WAIT_REL_DONE: release axi buffer                    \n");
    ar_always("          7: WAIT_BLK_DONE: wait release axi buffer done and wait \n");
    ar_always("                            one blk convert done                  \n");
    ar_always("    [17:16] : ret_data_fetcher ft_state                         %d\n",  ((dbg_info>>16)&0x3) );
    ar_always("          0: FETCH_Y0                                             \n");
    ar_always("          1: FETCH_Y1                                             \n");
    ar_always("          2: FETCH_CB                                             \n");
    ar_always("          3: FETCH_CR                                             \n");
    ar_always("    [18:18] : ret_data_fetcher wait_blk_write done state        %d\n",  ((dbg_info>>18)&0x1) );
    ar_always("          0: IDLE                                                 \n");
    ar_always("          1£ºWAIT_DONE                                            \n");
    ar_always("    [19:19] : ret_data_fetcher abi_rel_st                       %d\n",  ((dbg_info>>19)&0x1) );
    ar_always("          0: IDLE                                                 \n");
    ar_always("          1: WAIT_DONE                                            \n");
    ar_always("                                                                  \n");

    READ_DBG_INFO( 0x22 );
    ar_always(" DebugInfoSel: 0x22: abld_src_debug_info                          \n");
    ar_always("     [2:0] : state                                                %d\n",  ((dbg_info>>0)&0x7) );
    ar_always("           0: IDLE                                                  \n");
    ar_always("           1: RUN   : analyze one block                             \n");
    ar_always("           2: WAIT  : wait src_req_fifo empty                       \n");
    ar_always("     [5:4] : blk32x16_line_chk state                              %d\n",  ((dbg_info>>4)&0x3) );
    ar_always("           0: IDLE                                                  \n");
    ar_always("           1: RD_MSK                                                \n");
    ar_always("           2: WAIT                                                  \n");
    ar_always("     [9:8] : src_fetch_line_rd state                              %d\n",  ((dbg_info>>8)&0x3) );
    ar_always("           0: WAIT_POS_RDY                                          \n");
    ar_always("           1: WAIT_PRE_VLD                                          \n");
    ar_always("           2: FT_SRC_MEM                                            \n");
    ar_always("           3: PU_SRC_FFO                                            \n");

    READ_DBG_INFO( 0x23 );
    ar_always(" DebugInfoSel: 0x23: abld_rst_debug_info                          \n");
    ar_always("    [1:0] : wrst_fmt_convert                                    %d\n",  ((dbg_info>>0)&0x3) );
    ar_always("          0: WT_PRE_VLD                                           \n");
    ar_always("          1: WT_POS_RDY                                           \n");
    ar_always("          2: PROC_LINE                                            \n");
    ar_always("    [3:2] : wrst_fmt_convert write state                        %d\n",  ((dbg_info>>2)&0x3) );
    ar_always("          0: WR_IDLE                                              \n");
    ar_always("          1: WR_P0                                                \n");
    ar_always("          2: WR_P1                                                \n");
    ar_always("          3: WR_p2                                                \n");
    ar_always("    [6:4] : wrst_wr_req_gen state                               %d\n",  ((dbg_info>>4)&0x7) );
    ar_always("          0: IDLE                                                 \n");
    ar_always("          1: WR_P0_REQ                                            \n");
    ar_always("          2: WR_P0_DAT                                            \n");
    ar_always("          3: WR_P1_REQ                                            \n");
    ar_always("          4: WR_P1_DAT                                            \n");
    ar_always("          5: WR_P2_REQ                                            \n");
    ar_always("          6: WR_P2_DAT                                            \n");
    ar_always("          7: WT_AXI_REL                                           \n");
    ar_always("    [9:8] : wrst_wr_req_gen read axi buffer for write state     %d\n",  ((dbg_info>>8)&0x3) );
    ar_always("          0: RD_IDLE                                              \n");
    ar_always("          2: RD_READ                                              \n");
    ar_always("    [10]  : wreq_fifo_empty                                     %d\n",  ((dbg_info>>10)&0x1) );

    READ_DBG_INFO( 0x24 );
    ar_always(" DebugInfoSel: 0x24: abld_dst_read_line_cnt_debug_info            \n");
    ar_always("    [31:0] : line cnt £º totol read line cnt                    %d\n",  ((dbg_info>>0)) );
    READ_DBG_INFO( 0x25 );
    ar_always(" DebugInfoSel: 0x25: abld_dst_push_pcs_cnt_debug_info             \n");
    ar_always("    [31:0] : piece_cnt:  totol piece cnt                        %d\n",  ((dbg_info>>0)) );
    READ_DBG_INFO( 0x26 );
    ar_always(" DebugInfoSel: 0x26: abld_src_read_line_cnt_debug_info            \n");
    ar_always("    [15:0] : need_dst_line_cnt                                  %d\n",  ((dbg_info>>0)&0xFFFF) );
    ar_always("    [31:16] : no_need_dst_line_cnt                              %d\n",  ((dbg_info>>16)&0xFFFF) );

    READ_DBG_INFO( 0x27 );
    ar_always(" DebugInfoSel: 0x27: sum write                                    \n");
    ar_always("    [1:0] : sum write state                                     %d\n",  ((dbg_info>>0)&0x3) );
    ar_always("           0: IDLE                                                \n");
    ar_always("           1: WRIT                                                \n");
    ar_always("           2£ºWREQ                                                \n");
    ar_always("           3: WDAT                                                \n");
    ar_always("    [2:2] : bresp_fifo_full_w                                   %d\n",  ((dbg_info>>2)&0x1) );
    ar_always("    [3:3] : bresp_fifo_empty_w                                  %d\n",  ((dbg_info>>3)&0x1) );

    READ_DBG_INFO( 0x40 );
    ar_always(" DebugInfoSel: 0x40: axi_buf_mgr_debug_info                       \n");
    ar_always("    [7:0] : ipsr_abi_req_cnt                                    %d\n",  ((dbg_info>>0)&0xFF) );
    ar_always("    [15:8]: dbmp_rsrc_abi_req_cnt                               %d\n",  ((dbg_info>>8)&0xFF) );
    ar_always("    [23:16]:abld_rdst_abi_req_cnt                               %d\n",  ((dbg_info>>16)&0xFF) );
    ar_always("    [31:24]:abld_wdst_abi_req_cnt                               %d\n",  ((dbg_info>>24)&0xFF) );


    dbg_info = read_reg32( GE2D_REG_BASE + 0x0C);
    ar_always(" RTOS Interrupt Info:%08x\n", dbg_info);
    dbg_info = read_reg32( GE2D_REG_BASE + 0x14);
    ar_always(" RTOS CmdQueue Fullness:%08x\n", dbg_info);
    dbg_info = read_reg32( GE2D_REG_BASE + 0x28);
    ar_always(" RTOS RetQuee Fullness:%04x\n", dbg_info);


    dbg_info = read_reg32( GE2D_REG_BASE + 0x90);
    ar_always(" Linux Interrupt Info:%08x\n", dbg_info);
    dbg_info = read_reg32( GE2D_REG_BASE + 0x98);
    ar_always(" Linux CmdQueue Fullness:%08x\n", dbg_info);
    dbg_info = read_reg32( GE2D_REG_BASE + 0xAC);
    ar_always(" Linux RetQuee Fullness:%04x\n", dbg_info);
}
#endif
