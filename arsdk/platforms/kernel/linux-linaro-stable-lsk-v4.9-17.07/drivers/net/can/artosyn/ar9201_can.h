/*
 * Core driver for the ar9201 CAN controllers
 */
#ifndef AR9201_CAN_H
#define AR9201_CAN_H

#include <linux/can/dev.h>

#define AR9201_CAN_DEV_NAME     "ar9201_can"
#define AR9201_CAN_TX_FIFO_NUM  16
#define AR9201_CAN_RX_FIFO_NUM  16

#define AR9201_CAN_AMASK_ID10_0        (0x7FF)
#define AR9201_CAN_UNAMASK_ID10_0      (0x0)
#define AR9201_CAN_FRAME_LEN_AMASK     (0xF)

#define AR9201_CAN_AMASK_ID28_0        (0x1FFFFFFF)
#define AR9201_CAN_UNAMASK_ID28_0      (0x0)

//define register TCTRL
#define AR9201_CAN_TCTRL_FDISO         (1<<7)
#define AR9201_CAN_TCTRL_TSNEXT        (1<<6)
#define AR9201_CAN_TCTRL_TSMODE        (1<<5)
#define AR9201_CAN_TCTRL_TTTBM         (1<<4)
#define AR9201_CAN_TCTRL_TSSTAT_MASK   (0x03)
#define AR9201_CAN_TCTRL_TSSTAT_STB_EMPTY   (0x00)
#define AR9201_CAN_TCTRL_TSSTAT_STB_LEHF    (0x01)
#define AR9201_CAN_TCTRL_TSSTAT_STB_BGHF    (0x02)
#define AR9201_CAN_TCTRL_TSSTAT_STB_FULL    (0x03)

//define register RCTRL
#define AR9201_CAN_RCTRL_ROM            (1<<6)
#define AR9201_CAN_RCTRL_ROV            (1<<5)
#define AR9201_CAN_RCTRL_RREL           (1<<4)
#define AR9201_CAN_RCTRL_RSTAT_MASK     (0x03)
#define AR9201_CAN_RCTRL_RSTAT_STB_EMPTY    (0x00)
#define AR9201_CAN_RCTRL_RSTAT_STB_LEHF     (0x01)
#define AR9201_CAN_RCTRL_RSTAT_STB_BGHF     (0x02)
#define AR9201_CAN_RCTRL_RSTAT_STB_FULL     (0x03)

//define register TCMD
#define AR9201_CAN_TCMD_TBSEL  (1<<7)
#define AR9201_CAN_TCMD_LOM    (1<<6)
#define AR9201_CAN_TCMD_STBY   (1<<5)
#define AR9201_CAN_TCMD_TPE    (1<<4)
#define AR9201_CAN_TCMD_TPA    (1<<3)
#define AR9201_CAN_TCMD_TSONE  (1<<2)
#define AR9201_CAN_TCMD_TSALL  (1<<1)
#define AR9201_CAN_TCMD_TSA    (1<<0)

//define register RTIE
#define AR9201_CAN_RTIE_RIE    (1<<7)
#define AR9201_CAN_RTIE_ROIE   (1<<6)
#define AR9201_CAN_RTIE_RFIE   (1<<5)
#define AR9201_CAN_RTIE_RAFIE  (1<<4)
#define AR9201_CAN_RTIE_TPIE   (1<<3)
#define AR9201_CAN_RTIE_TSIE   (1<<2)
#define AR9201_CAN_RTIE_EIE    (1<<1)

#define AR9201_CAN_RTIE_TSFF   (1<<0)

#define AR9201_CAN_RTIE_ALL    (AR9201_CAN_RTIE_RIE | \
                                AR9201_CAN_RTIE_ROIE | \
                                AR9201_CAN_RTIE_RFIE | \
                                AR9201_CAN_RTIE_RAFIE | \
                                AR9201_CAN_RTIE_TPIE | \
                                AR9201_CAN_RTIE_TSIE | \
                                AR9201_CAN_RTIE_EIE)


//define register RTIF
#define AR9201_CAN_RTIF_RIF    (1<<7)
#define AR9201_CAN_RTIF_ROIF   (1<<6)
#define AR9201_CAN_RTIF_RFIF   (1<<5)
#define AR9201_CAN_RTIF_RAFIF  (1<<4)
#define AR9201_CAN_RTIF_TPIF   (1<<3)
#define AR9201_CAN_RTIF_TSIF   (1<<2)
#define AR9201_CAN_RTIF_EIF    (1<<1)
#define AR9201_CAN_RTIF_AIF    (1<<0)

#define AR9201_CAN_RTIF_ALL     (AR9201_CAN_RTIF_RIF | \
                                AR9201_CAN_RTIF_ROIF | \
                                AR9201_CAN_RTIF_RFIF | \
                                AR9201_CAN_RTIF_RAFIF | \
                                AR9201_CAN_RTIF_TPIF | \
                                AR9201_CAN_RTIF_TSIF | \
                                AR9201_CAN_RTIF_EIF | \
                                AR9201_CAN_RTIF_AIF)

//define register ERRINT
#define AR9201_CAN_ERRINT_EWARN (1<<7)
#define AR9201_CAN_ERRINT_EPASS (1<<6)
#define AR9201_CAN_ERRINT_EPIE  (1<<5)
#define AR9201_CAN_ERRINT_EPIF  (1<<4)
#define AR9201_CAN_ERRINT_ALIE  (1<<3)
#define AR9201_CAN_ERRINT_ALIF  (1<<2)
#define AR9201_CAN_ERRINT_BEIE  (1<<1)
#define AR9201_CAN_ERRINT_BEIF  (1<<0)
#define AR9201_CAN_ERRINT_F_ALL (AR9201_CAN_ERRINT_EWARN | \
                                AR9201_CAN_ERRINT_EPASS | \
                                AR9201_CAN_ERRINT_EPIF | \
                                AR9201_CAN_ERRINT_ALIF | \
                                AR9201_CAN_ERRINT_BEIF)

#define AR9201_CAN_REG4_ENABLE  (((AR9201_CAN_ERRINT_EPIE | AR9201_CAN_ERRINT_ALIE | AR9201_CAN_ERRINT_BEIE) << 16) | \
                                AR9201_CAN_RTIE_ALL)
#define AR9201_CAN_REG4_FLAG    ((AR9201_CAN_ERRINT_F_ALL << 16) | (AR9201_CAN_RTIF_ALL << 8) | AR9201_CAN_RTIE_TSFF)

//define register TCMD
/*#define AR9201_CAN_TCMD_TBSEL  (1<<7)
#define AR9201_CAN_TCMD_LOM    (1<<6)
#define AR9201_CAN_TCMD_STBY   (1<<5)
#define AR9201_CAN_TCMD_TPE    (1<<4)
#define AR9201_CAN_TCMD_TPA    (1<<3)
#define AR9201_CAN_TCMD_TSONE  (1<<2)
#define AR9201_CAN_TCMD_TSALL  (1<<1)
#define AR9201_CAN_TCMD_TSA    (1<<0)*/

//define register TBUF
#define AR9201_CAN_TBUF_IDE    (1<<7)  //std or ext
#define AR9201_CAN_TBUF_RTR    (1<<6)  //data or remote
#define AR9201_CAN_TBUF_EDL    (1<<5)
#define AR9201_CAN_TBUF_BRS    (1<<4)

//define register CFG_STAT
#define AR9201_CFG_STAT_RESET     (1<<7) //
#define AR9201_CFG_STAT_LBME      (1<<6) //
#define AR9201_CFG_STAT_LBMI      (1<<5) //
#define AR9201_CFG_STAT_TPSS      (1<<4) //
#define AR9201_CFG_STAT_TSSS      (1<<3) //
#define AR9201_CFG_STAT_RACTIVE   (1<<2) // Reception ACTIVE (Receive Status bit)
#define AR9201_CFG_STAT_TACTIVE   (1<<1) // Transmission ACTIVE (Transmit Status bit)
#define AR9201_CFG_STAT_BUSOFF    (1<<0) //

#define AR9201_CAN_BTR_S_SEG_1(x)   ((x) & 0x3f)
#define AR9201_CAN_BTR_S_SEG_2(x)   (((x) & 0x1f) << 8)
#define AR9201_CAN_BTR_S_SJW(x)     (((x) & 0xf) << 16)

//define Acceptance Filter Control Register
#define AR9201_CAN_ACF_SELMASK(x)   (((x) & 0x1) << 5)
#define AR9201_CAN_ACF_ACFADR(x)    ((x) & 0xf)
#define AR9201_CAN_ACF_3_AIDE       (1 << 5)
#define AR9201_CAN_ACF_3_AIDEE      (1 << 6)

//define ACF_EN_0
#define AR9201_CAN_ACF_ENABLE(x)    (1 << ((x) & 0x07))

//define rxtxbuf control
#define AR9201_CAN_TXBUF_CTRL_IDE   (1 << 7)
#define AR9201_CAN_TXBUF_CTRL_RTR   (1 << 6)
#define AR9201_CAN_TXBUF_CTRL_EDL   (1 << 5)
#define AR9201_CAN_TXBUF_CTRL_BRS   (1 << 4)

//define TTCFG
#define AR9201_CAN_TTCFG_TTEN       (1 << 0)
#define AR9201_CAN_TTCFG_TTIE       (1 << 4)
#define AR9201_CAN_TTCFG_WTIE       (1 << 7)

//define id
#define AR9201_CAN_ID_ESI           (1 << 31)

//define EALCAP
#define AR9201_CAN_EALCAP_KOER_MASK (0xe0)
#define AR9201_CAN_EALCAP_KOER(x)   ((x & AR9201_CAN_EALCAP_KOER_MASK) >> 5)

#define AR9201_CAN_EALCAP_ALC_MASK  (0x1f)

enum ar9201_can_ealcap{
    KOER_NO_ERROR,
    KOER_BIT_ERROR,
    KOER_FORM_ERROR,
    KOER_STUFF_ERROR,
    KOER_ACK_ERROR,
    KOER_CRC_ERROR,
    KOER_OTHER_ERROR,
    KOER_NOT_USED,
};

/*******************can register define**************************/
#pragma pack(push)
#pragma pack(1)
struct ar9201_can_rxbuf{
    u32     id;
    u8      ctrl;
    u16     timestamp;
    u8      reserved;
    u8      data[64];
};

struct ar9201_can_txbuf{
    u32     id;
    u8      ctrl;
    u8      reserved[3];
    u8      data[64];
};

struct u8_ar9201_can_reg{
    volatile struct ar9201_can_rxbuf rxBuf;
    volatile struct ar9201_can_txbuf txBuf;

    volatile uint8_t    u8_CFG_STAT;
    volatile uint8_t    u8_TCMD;
    volatile uint8_t    u8_TCTRL;
    volatile uint8_t    u8_RCTRL;

    volatile uint8_t    u8_RTIE;
    volatile uint8_t    u8_RTIF;
    volatile uint8_t    u8_ERRINT;
    volatile uint8_t    u8_LIMIT;

    volatile uint8_t    u8_BITTIME0;
    volatile uint8_t    u8_BITTIME1;
    volatile uint8_t    u8_BITTIME2;
    volatile uint8_t    u8_BITTIME3;

    volatile uint8_t    u8_S_PRESC;
    volatile uint8_t    u8_F_PRESC;
    volatile uint8_t    u8_TDC;
    volatile uint8_t    u8_resvered0;

    volatile uint8_t    u8_EALCAP;
    volatile uint8_t    u8_resvered1;
    volatile uint8_t    u8_RECNT;
    volatile uint8_t    u8_TECNT;

    volatile uint8_t    u8_ACFCTRL;
    volatile uint8_t    u8_resvered2;
    volatile uint8_t    u8_ACF_EN0;
    volatile uint8_t    u8_ACF_EN1;

    volatile uint8_t    u8_ACF0;
    volatile uint8_t    u8_ACF1;
    volatile uint8_t    u8_ACF2;
    volatile uint8_t    u8_ACF3;

    volatile uint8_t    u8_VER0;
    volatile uint8_t    u8_VER1;
    volatile uint8_t    u8_TBSLOT;
    volatile uint8_t    u8_TTCFG;

    volatile uint8_t    u8_REF_MSG0;
    volatile uint8_t    u8_REF_MSG1;
    volatile uint8_t    u8_REF_MSG2;
    volatile uint8_t    u8_REF_MSG3;

    volatile uint8_t    u8_TRIG_CFG0;
    volatile uint8_t    u8_TRIG_CFG1;
    volatile uint8_t    u8_TT_TRIG0;
    volatile uint8_t    u8_TT_TRIG1;

    volatile uint8_t    u8_TT_WTRIG0;
    volatile uint8_t    u8_TT_WTRIG1;
    volatile uint8_t    u8_resvered3[2];
};

struct u32_ar9201_can_reg{
    volatile uint32_t    u32_rxBuf[18];          // 0x00-0x47
    volatile uint32_t    u32_txBuf[18];          // 0x48-0x8f
    volatile uint32_t    u32_reg3;               // 0x90 -->  RCTRL    TCTRL    TCMD      CFG_STAT
    volatile uint32_t    u32_reg4;               // 0x94 -->  LIMIT    ERRINT   RTIF      RTIE
    volatile uint32_t    u32_reg5;               // 0x98 -->  BITTIME3 BITTIME2 BITTIME1  BITTIME0
    volatile uint32_t    u32_reg6;               // 0x9c -->  --       TDC      F_PRESC   S_PRESC
    volatile uint32_t    u32_reg7;               // 0xa0 -->  TECNT    RECNT    --        EALCAP
    volatile uint32_t    u32_reg8;               // 0xa4 -->  ACF_EN1  ACF_EN0  --        ACFCTRL
    volatile uint32_t    u32_reg9;               // 0xa8 -->  ACF3     ACF2     ACF1      ACF0
    volatile uint32_t    u32_reg10;              // 0xac -->  TTCFG    TBSLOT   VER1      VER0
    volatile uint32_t    u32_reg11;              // 0xb0 -->  REF_MSG3 REF_MSG2 REF_MSG1  REF_MSG0
    volatile uint32_t    u32_reg12;              // 0xb4 -->  TT_TRIG1 TT_TRIG0 TRIG_CFG1 TRIG_CFG0
    volatile uint32_t    u32_reg13;              // 0xb8 -->  --       --       TT_WTRIG1 TT_WTRIG0
};

union ar9201_can_reg{
    struct u8_ar9201_can_reg   u8_if;
    struct u32_ar9201_can_reg  u32_if;
};
#pragma pack(pop)
struct ar9201_can_priv{
    struct can_priv                 can;    /* MUST be first member/field */
    struct napi_struct              napi;
    struct net_device               *ndev;
    union ar9201_can_reg __iomem    *regs;
    struct clk                      *clk;
    struct clk                      *can_clk;
    u8                              tx_dlc[AR9201_CAN_TX_FIFO_NUM];
    u32                             tx_head;
    u32                             tx_tail;
    struct work_struct              tx_queue_work;
    u32                             tx_queue_num;
    u32                             tx_full_num;
    spinlock_t                      tx_lock;
    spinlock_t                      rx_lock;
};

#endif

