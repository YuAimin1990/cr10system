#include <common.h>
#include <phy.h>
#include <errno.h>
#include <common.h>
#include <miiphy.h>
#include <malloc.h>
#include <linux/compiler.h>
#include <linux/err.h>
#include <asm/io.h>
#include <net.h>

#include "sirius_eth.h"

#if 0
#  define ETHERNET_DEBUG
#endif

/* Ethernet interrupt handler */
void eth_irq_handler(void)
{
}

static void _sirius_eth_wait_miim(unsigned int base_addr)
{
    unsigned int status_register ;
    unsigned long start;
    int timeout = 3000; //3s

    status_register = 0xffffffff ;
    start = get_timer(0);

    //wait data from PHY
    while (get_timer(start) < timeout)
    {
	status_register = readl((unsigned long)(base_addr + ETH_MIISTATUS));
	if( status_register & (ETH_MIISTATUS_BUSY | ETH_MIISTATUS_NVALID) )
		udelay(10);
	else
		return;
    }
    printf("Time out waiting mdio busy!\n");

}
static int _sirius_write_hwaddr(struct sirius_eth_dev *priv, u8 * enetaddr)
{
    char * eth_mac = (char *)enetaddr;
	
    writel(eth_mac[0] << 8 | eth_mac[1], (unsigned long)(priv->mac_base_addr + ETH_MAC_ADDR1));
    writel(eth_mac[2] << 24 | eth_mac[3] << 16 | eth_mac[4] << 8 | eth_mac[5], (unsigned long)(priv->mac_base_addr + ETH_MAC_ADDR0));

    return 0;
}

static void _sirius_eth_intr_enable(struct eth_device *dev)
{
    unsigned int val = ETH_INT_MASK_TXB | ETH_INT_MASK_TXE 
						| ETH_INT_MASK_RXF | ETH_INT_MASK_RXE
						| ETH_INT_MASK_BUSY | ETH_INT_MASK_TXC
						| ETH_INT_MASK_RXC;
						
    writel(val, dev->iobase + ETH_INT_MASK);
}

static int _sirius_adjust_link(phys_addr_t iobase, struct phy_device * phydev)
{
    //unsigned int mode = readl(iobase + ETH_SPEED_SEL_MODE);
    
    if(!phydev->link)
    {
	printf("%s: no link.\n", phydev->dev->name);
	return 0;
    }

    if(phydev->speed == SPEED_1000)
    {
        writel(0x1, iobase + ETH_SPEED_SEL_MODE);
        writel(0x4, iobase + ETH_IPGT); //inner gap 4 for 1G and 8 for 10/100M(default)
        writel(0x000f01ff, iobase + ETH_COLLCONF); //collision time for 10/100M is 3f(default) and 1ff for 1G
    }
    else if(phydev->speed == SPEED_100)
    {
	writel(0x0, iobase + ETH_SPEED_SEL_MODE);
    }
    else if(phydev->speed == SPEED_10)
    {
	writel(0x2, iobase + ETH_SPEED_SEL_MODE);
    }
    else
    {
	printf("Invalid speed!\n");
        return -1;
    }

    if(phydev->duplex)
    {
	writel(readl(iobase + ETH_MODER) | ETH_MODER_FULLD, iobase + ETH_MODER);
    }

    printf("Speed: %d, %s duplex.\n", phydev->speed, phydev->duplex ? "full":"half");
    return 0;

        /*Assert carrier sense on transmit*/
//        writel(0x1000 + phy_adr, dev->iobase + ETH_MIIADDRESS);
//        writel(ETH_MIICOMMAND_RSTAT, dev->iobase + ETH_MIICOMMAND);
 
}
static void _sirius_tx_descs_init(struct sirius_eth_dev *priv)
{
    u32 idx;
    struct sirius_eth_bd * desc_p = NULL;
    priv->tx_mac_descrtable = (struct sirius_eth_bd *)(priv->mac_base_addr + SIRIUS_ETH_TXBD_OFFSET);

    //start from 0 every time according to controller spec
    priv->tx_head = 0;
    priv->tx_tail = 0;
    priv->tx_full = 0;
    	
    for (idx = 0; idx < SIRIUS_ETH_TXBD_NUM; idx++) 
    {
	desc_p = &priv->tx_mac_descrtable[idx];
	desc_p->addr = net_tx_packet;
	desc_p->len_status = 0 << 16 | ETH_TX_BD_PAD | ETH_TX_BD_CRC | ETH_TX_BD_IRQ;	
        desc_p->len_status &= ~ETH_TX_BD_STATS;
        //The last Rx BD
        if(idx == SIRIUS_ETH_TXBD_NUM - 1)
            desc_p->len_status |= ETH_TX_BD_WRAP;
	
    }
   	
}

static void _sirius_rx_descs_init(struct sirius_eth_dev *priv)
{    
	u32 idx;
	struct sirius_eth_bd * desc_p = NULL;
	
	priv->rx_mac_descrtable = (struct sirius_eth_bd *)(priv->mac_base_addr + SIRIUS_ETH_RXBD_OFFSET);

        priv->rx_tail = 0;
	
	/* Before passing buffers to GMAC we need to make sure zeros
	 * written there right after "priv" structure allocation were
	 * flushed into RAM.
	 * Otherwise there's a chance to get some of them flushed in RAM when
	 * GMAC is already pushing data to RAM via DMA. This way incoming from
	 * GMAC data will be corrupted. */
	//flush_dcache_range((unsigned int)rxbuffs, (unsigned int)rxbuffs + RX_TOTAL_BUFSIZE);

	for (idx = 0; idx < SIRIUS_ETH_RXBD_NUM; idx++) 
	{
		desc_p = &priv->rx_mac_descrtable[idx];
		desc_p->addr = net_rx_packets[idx];
		desc_p->len_status = 0 << 16 | ETH_RX_BD_EMPTY | ETH_RX_BD_IRQ;	
		desc_p->len_status &= ~ETH_RX_BD_STATS;
	        //The last Rx BD
	        if(idx == SIRIUS_ETH_RXBD_NUM - 1)
	            desc_p->len_status |= ETH_RX_BD_WRAP;
	
	}
		
	/* Flush all Rx buffer descriptors at once */
	//flush_dcache_range((unsigned int)priv->rx_mac_descrtable,(unsigned int)priv->rx_mac_descrtable + sizeof(priv->rx_mac_descrtable));
}

static void tx_push(struct sirius_eth_dev *priv)
{
	priv->tx_head++;
	if(priv->tx_head == SIRIUS_ETH_TXBD_NUM)
		priv->tx_head = 0;
	if(priv->tx_head == priv->tx_tail)
		priv->tx_full = 1;
}

static void tx_pop(struct sirius_eth_dev *priv)
{
	priv->tx_tail++;
	if(priv->tx_tail == SIRIUS_ETH_TXBD_NUM)
		priv->tx_tail = 0;

	priv->tx_full = 0;
}

static int tx_empty(struct sirius_eth_dev *priv)
{
	return priv->tx_tail == priv->tx_head && !priv->tx_full;
}

//tail is the latest desc ready to be transmit, the chip may be sent a pkt already, so we need check and update the tail value. 
static update_tail(struct sirius_eth_dev *priv)
{
	u32 st;
	struct sirius_eth_bd  * txbd = (struct sirius_eth_bd  *)priv->tx_mac_descrtable;
	for(;;tx_pop(priv))
	{
		st = txbd[priv->tx_tail].len_status;
		if((st & ETH_TX_BD_READY != ETH_TX_BD_READY) || tx_empty(priv))
			break;
	}
}
static void rx_pop(struct sirius_eth_dev *priv)
{
	priv->rx_tail++;
	if(priv->rx_tail == SIRIUS_ETH_RXBD_NUM)
		priv->rx_tail = 0;
}

static void sirius_test_print_packet(unsigned long add, int len)
{
  int i;
 
  printf("ipacket: add = %lx len = %d\n", add, len);
  for(i = 0; i < len; i++) {
      if(!(i % 16))
          printf("\n");
      printf(" %.2x", *(((unsigned char *)add) + i));
  }
  printf("\n");
}

#ifndef CONFIG_DM_ETH
/* Waits for packet and pass it to the upper layers */
static int sirius_eth_recv(struct eth_device * dev)
{
    int i = 0;
    unsigned long len = 0;
    struct sirius_eth_dev * priv = (struct sirius_eth_dev *)dev->priv;
    struct sirius_eth_bd  * rxbd = (struct sirius_eth_bd  *)priv->rx_mac_descrtable;

    //printf("Start receive...mode %x\n", readl(SIRIUS_ETH_REG_BASE + ETH_MODER));

    //polling the rxbd, the IP should recv packets using rxbd 0,1,2...
    //we process all the pkts at each time, if current desc is empty, no need to go on, so return.
    for(i = 0; i < SIRIUS_ETH_RXBD_NUM; i++,rx_pop(priv))
    {
    
        int bad = 0;

	//printf("Int status 0: 0x%x\n", readl(SIRIUS_ETH_REG_BASE+ETH_INT));
        if(rxbd[priv->rx_tail].len_status & ETH_RX_BD_EMPTY) 
	{
	    //printf("Int status 1: 0x%x\n", readl(SIRIUS_ETH_REG_BASE+ETH_INT));
	    //printf("Empty desc %d!\n", priv->rx_tail);
	    return len;
        }
        
	//printf("Recv: %d\n", priv->rx_tail);
	writel(0x4, (unsigned long)(priv->mac_base_addr + ETH_INT));

        if(rxbd[priv->rx_tail].len_status & ETH_RX_BD_BUSERR) 
	{
            printf("eth rx: ETH_RX_BD_BUSERR\n");
            bad = 1;
        }
        if(rxbd[priv->rx_tail].len_status & ETH_RX_BD_OVERRUN) 
        {
            printf("eth rx: ETH_RX_BD_OVERRUN\n");
            bad = 1;
        }
        if(rxbd[priv->rx_tail].len_status & ETH_RX_BD_INVSIMB) 
	{
            printf("eth rx: ETH_RX_BD_INVSIMB\n");
            bad = 1;
        }
        if(rxbd[priv->rx_tail].len_status & ETH_RX_BD_DRIBBLE) 
	{
            printf("eth rx: ETH_RX_BD_DRIBBLE\n");
            bad = 1;
        }
        if(rxbd[priv->rx_tail].len_status & ETH_RX_BD_TOOLONG) 
	{
            printf("eth rx: ETH_RX_BD_TOOLONG\n");
            bad = 1;
        }
        if(rxbd[priv->rx_tail].len_status & ETH_RX_BD_SHORT) 
	{
            printf("eth rx: ETH_RX_BD_SHORT\n");
            bad = 1;
        }
        if(rxbd[priv->rx_tail].len_status & ETH_RX_BD_CRCERR) 
	{
            printf("eth rx: ETH_RX_BD_CRCERR\n");
            bad = 1;
        }
        if(rxbd[priv->rx_tail].len_status & ETH_RX_BD_LATECOL) 
	{
            printf("eth rx: ETH_RX_BD_LATECOL\n");
            bad = 1;
        }

	rxbd[priv->rx_tail].len_status &= ~ETH_RX_BD_STATS;
        rxbd[priv->rx_tail].len_status |= ETH_RX_BD_EMPTY;

        if(priv->rx_tail == SIRIUS_ETH_RXBD_NUM - 1)
                rxbd[priv->rx_tail].len_status |= ETH_RX_BD_WRAP;

        if(bad)
	{
	    printf("Receive error with rx desc %d: %x\n", priv->rx_tail, rxbd[priv->rx_tail].len_status);
	    //clear the error info so that this BD can be used next time
	    rxbd[priv->rx_tail].len_status &= ~ETH_RX_BD_STATS;
            rxbd[priv->rx_tail].len_status |= ETH_RX_BD_EMPTY;

            if(priv->rx_tail == SIRIUS_ETH_RXBD_NUM - 1)
                    rxbd[priv->rx_tail].len_status |= ETH_RX_BD_WRAP;
 
	    return -1;
	}
	else
	{
            len += rxbd[priv->rx_tail].len_status >> 16;
            /* Invalidate entire buffer descriptor */
            unsigned long flushAddr =  rxbd[priv->rx_tail].addr;
            flushAddr &= ~(ARCH_DMA_MINALIGN - 1);
            unsigned long flushSize = roundup(rxbd[priv->rx_tail].len_status >> 16, ARCH_DMA_MINALIGN);
            invalidate_dcache_range(flushAddr, flushAddr + flushSize);
            barrier();

            net_process_received_packet((void *)rxbd[priv->rx_tail].addr, rxbd[priv->rx_tail].len_status >> 16); 
            //printf("Good pkt at %d, len %d\n", priv->rx_tail, len);
            //sirius_test_print_packet(rxbd[priv->rx_tail].addr, len);
	}

	//Set this desc as empty
        rxbd[priv->rx_tail].len_status &= ~ETH_RX_BD_STATS;
        rxbd[priv->rx_tail].len_status |= ETH_RX_BD_EMPTY;

	if(priv->rx_tail == SIRIUS_ETH_RXBD_NUM - 1)
		rxbd[priv->rx_tail].len_status |= ETH_RX_BD_WRAP;

	//printf("rx bd: %x\n", readl(SIRIUS_ETH_REG_BASE + ETH_CURR_RXBD));
        //continue;
    }
	
    return len;
}
//#define ETH_PERF_TEST
static int sirius_eth_perf_test(struct eth_device *dev, void *packet, int length);

/* Send a packet at address */
static int sirius_eth_send (struct eth_device *dev, void *packet, int length)
{
    //sirius_eth_perf_test(dev, packet, length);
    struct sirius_eth_dev * priv = (struct sirius_eth_dev *)dev->priv;
    struct sirius_eth_bd  * txbd = (struct sirius_eth_bd  *)priv->tx_mac_descrtable;
    static int test_num = 0;
    unsigned int len_status_val = 0;
//    sirius_test_print_packet(packet, 16);
    update_tail(priv);
    if(priv->tx_full)
    {
    	printf("TX descriptor is full, stop!\n");
    	return -1;
    }
    
    
    //flash data
    unsigned long flushAddr = (unsigned long)packet;
	flushAddr &= ~(ARCH_DMA_MINALIGN - 1);
	unsigned long flushSize = roundup(length, ARCH_DMA_MINALIGN);
	flush_dcache_range(flushAddr, flushAddr + flushSize);
	barrier();
    //printf("Send: head:%x Addr:%08x len:%d flush addr:%08x len:%d\n", priv->tx_head, (int)packet, length, flushAddr, flushSize);

    
    len_status_val = txbd[priv->tx_head].len_status;
    len_status_val &= (0x0000ffff & (~ETH_TX_BD_STATS));
    len_status_val |= (length << 16) | ETH_TX_BD_READY | ETH_TX_BD_PAD | ETH_TX_BD_CRC | ETH_TX_BD_IRQ;
    if(priv->tx_head == SIRIUS_ETH_TXBD_NUM - 1)
	len_status_val |= ETH_RX_BD_WRAP;

    txbd[priv->tx_head].addr = (unsigned long)packet;
    txbd[priv->tx_head].len_status = len_status_val;
    
    tx_push(priv); 
    //clear tx intr
    writel(0x1, (unsigned long)(priv->mac_base_addr + ETH_INT));
    //printf("tx bd: %x\n", readl(SIRIUS_ETH_REG_BASE + ETH_CURR_TXBD));         
    //printf("tx head %x, tail %x, full %d\n", priv->tx_head, priv->tx_tail, priv->tx_full);
}

static int sirius_eth_open(struct eth_device *dev)
{
    unsigned long tmp ;
    struct sirius_eth_dev * priv = (struct sirius_eth_dev *)dev->priv;
    int ret;
    static int has_initialized = 0;

//if(has_initialized == 0)
{
 
    /* hbbai: set little endian for TX/RX */
    //writel(0x3, dev->iobase + ETH_ENDIAN_CFG_REG); 
    //printf("Eth open, endian %x.\n", readl(dev->iobase + ETH_ENDIAN_CFG_REG));

    /* Exit Power save mode */
    writel(0xffffff0f & readl(GMAC_PWR_CTRL_REG), GMAC_PWR_CTRL_REG);

    /* Clear MODER */
    writel(0x00000000, dev->iobase + ETH_MODER);
    
    /* PAD, IFG, CRCEn */
    writel(readl(dev->iobase + ETH_MODER) | ETH_MODER_PAD | ETH_MODER_IFG | ETH_MODER_CRCEN | ETH_MODER_HUGEN /*| ETH_MODER_FIFO_FLAG */, dev->iobase + ETH_MODER);
    
    /* Setting TX BD number */
    writel(SIRIUS_ETH_TXBD_NUM, dev->iobase + ETH_TX_BD_NUM_REG);
    
    /* Set min/max packet length */
    //writel(0x00400600, dev->iobase + ETH_PACKETLEN);
    writel(0x0040fff0, dev->iobase + ETH_PACKETLEN);
    
    /* Initialize RX and TX buffer descriptors */
    _sirius_rx_descs_init(dev->priv);
    _sirius_tx_descs_init(dev->priv);
        
    /* Set local MAC address */
//    _sirius_write_hwaddr(dev->priv, bis->bi_enetaddr);
    
    /* Set IPGT register to recomended value */
    writel(0x00000008, dev->iobase + ETH_IPGT);
    
    /* Set COLLCONF register to recomended value */
    writel(0x000f003f, dev->iobase + ETH_COLLCONF);
    
    /* Clear all pending interrupts */
    writel(0xffffffff, dev->iobase + ETH_INT);
    
    /* Enable interrupt sources */
    writel(0xffffffff, dev->iobase + ETH_INT_MASK);
    
    writel(0, dev->iobase + ETH_CTRLMODER);

    //has_initialized = 1;

    /* Enable receiver and transmiter */
    writel(readl(dev->iobase + ETH_MODER) | ETH_MODER_RXEN | ETH_MODER_TXEN, dev->iobase + ETH_MODER);
   
    //printf("Gmac mode reg: %x.\n", readl(dev->iobase + ETH_MODER)); 

}/* has_initialized == 0 */

    /* PHY config */
    ret = phy_startup(priv->phydev);
    if(ret)
    {
        printf("Could not initialize PHY %s.\n", priv->phydev->drv->name);
	return ret;
    }

    ret = _sirius_adjust_link(dev->iobase, priv->phydev);
    if(ret)
    {
        printf("Could not adjust link.\n");
	return ret;
    }    

    return 0;
    /* We don't use interrupts in u-boot */
    //install_hdlr(ETH_IRQ, eth_int);
}

static void sirius_eth_halt(struct eth_device * dev)
{
    struct sirius_eth_dev * priv = (struct sirius_eth_dev *)dev->priv;
#if 1
    /* disable receiver and transmiter */
    writel(readl(dev->iobase + ETH_MODER) & (~(ETH_MODER_RXEN | ETH_MODER_TXEN)), dev->iobase + ETH_MODER);
    /* clear all interrupts */
    writel(0xffffffff, dev->iobase + ETH_INT);

    /* drop all packets in desc ?*/

    /* shut down phy*/
    phy_shutdown(priv->phydev);
#endif

}
static int sirius_write_hwaddr(struct eth_device *dev)
{
    return _sirius_write_hwaddr(dev->priv, dev->enetaddr);
}

//ADDR is PHY addr, reg is register addr
static int sirius_mdio_read(struct mii_dev *bus, int addr, int devad, int reg)
{
    unsigned int mac_base_addr = (unsigned int)bus->priv;
    unsigned int tmp = 0;

    //printf("Sirius mdio read %d %d, ", addr, reg);
    writel((reg<<ETH_MII_REG_ADDR_SHIFT) + (addr<<ETH_MII_PHY_ADDR_SHIFT), (unsigned long)(mac_base_addr + ETH_MIIADDRESS));
    writel(ETH_MIICOMMAND_RSTAT, (unsigned long)(mac_base_addr + ETH_MIICOMMAND));
    _sirius_eth_wait_miim((unsigned long)mac_base_addr) ;
    tmp = readl((unsigned long)(mac_base_addr + ETH_MIIRX_DATA));
    //printf("data is %x\n", tmp);

    return tmp;
}

static int sirius_mdio_write(struct mii_dev *bus, int addr, int devad, int reg, u16 val)
{
    unsigned int mac_base_addr = (unsigned int)bus->priv;
    unsigned int tmp = 0;
	
    ///printf("Sirius mdio write %d %d, data is %x\n", addr, reg, val);
    writel((reg<<ETH_MII_REG_ADDR_SHIFT) + (addr<<ETH_MII_PHY_ADDR_SHIFT), (unsigned long)(mac_base_addr + ETH_MIIADDRESS));
    writel(val, (unsigned long)(mac_base_addr + ETH_MIITX_DATA));
    writel(ETH_MIICOMMAND_WCTRLDATA, (unsigned long)(mac_base_addr + ETH_MIICOMMAND));
    _sirius_eth_wait_miim((unsigned long)mac_base_addr);

    return 0;
}

static int sirius_mdio_init(const char * name, unsigned int mac_regs_p)
{
    struct mii_dev *bus = mdio_alloc();

    if (!bus) 
    {
    	printf("Failed to allocate MDIO bus\n");
    	return -ENOMEM;
    }
    
    bus->read = sirius_mdio_read;
    bus->write = sirius_mdio_write;
    snprintf(bus->name, sizeof(bus->name), name);
    
    bus->priv = (void *)(unsigned long)mac_regs_p;
    
    return mdio_register(bus);
}
static int sirius_phy_init(struct sirius_eth_dev *priv, void *dev)
{
    struct phy_device *phydev;
    int mask = 0xffffffff;
    unsigned int reg = 0;
    //Reset PHY
    reg = readl(GBE_RSTN_CTRL_REG);
    writel(reg & 0xfffffffe, GBE_RSTN_CTRL_REG);
    udelay(50000);
    writel(reg | 0x1, GBE_RSTN_CTRL_REG);
	udelay(50000);

#ifdef CONFIG_PHY_ADDR
    mask = 1 << CONFIG_PHY_ADDR;
#endif

    phydev = phy_find_by_mask(priv->bus, mask, priv->interface);
    if (!phydev)
    	return -ENODEV;
    
    phy_connect_dev(phydev, dev);
    
    phydev->supported &= PHY_GBIT_FEATURES;
    phydev->advertising = phydev->supported;
    
    priv->phydev = phydev;
    phy_config(phydev);
   
    printf("PHY init successed!\n"); 
    return 0;
}	
extern struct in_addr net_ip;

int sirius_eth_initialize(ulong base_addr, unsigned int interface, unsigned char *enetaddr)
{
    int i = 0;
    int ret = 0;	
    struct eth_device *dev = NULL;
    struct sirius_eth_dev * priv = NULL;
    
    //temp test
    //asm volatile("cpsie if");
    printf("Init Sirius ethernet...\n");

    //enable bridge enhancement for better performance
    writel(0x1, GBE_TX_CTRL_REG);
    writel(0x1, GBE_RX_CTRL_REG);

    dev = (struct eth_device *) malloc(sizeof(struct eth_device));
    if (!dev)
    	return -ENOMEM;
    /*
     * Since the priv structure contains the descriptors which need a strict
     * buswidth alignment, memalign is used to allocate memory
     */
    priv = (struct sirius_eth_dev *) malloc(sizeof(struct sirius_eth_dev));
    //priv = (struct sirius_eth_dev *) memalign(ARCH_DMA_MINALIGN,sizeof(struct sirius_eth_dev));
    
    if (!priv) 
    {
    	free(dev);
    	return -ENOMEM;
    }
    
    memset(dev, 0, sizeof(struct eth_device));
    memset(priv, 0, sizeof(struct sirius_eth_dev));
    
    sprintf(dev->name, "%s", SIRIUS_ETH_DEV_NAME);
    dev->iobase = (int)base_addr;
    dev->priv = priv;
    for(i=0; i<6; i++)
    	dev->enetaddr[i] = enetaddr[i];
    	
    priv->dev = dev;
    priv->mac_base_addr = base_addr;
    priv->tx_head = 0;
    priv->tx_tail = 0;
    priv->tx_full = 0;
    priv->rx_tail = 0;    
    
    dev->init = sirius_eth_open;
    dev->send = sirius_eth_send;
    dev->recv = sirius_eth_recv;
    dev->halt = sirius_eth_halt;
    dev->write_hwaddr = sirius_write_hwaddr;
    
    eth_register(dev);
    
    priv->interface = interface;
    //hbbai: temp use
    //net_ip = string_to_ip(__stringify(CONFIG_IPADDR));

    //Register mdio bus so mdio_read/write can be used.
    sirius_mdio_init(dev->name, dev->iobase);
    priv->bus = miiphy_get_dev_by_name(dev->name);
    
    printf("Register mdio %s / eth %s successfully!\n", priv->bus->name, dev->name);	
    
    //detect phy id and init the phy
    ret =  sirius_phy_init(priv, dev);				
    if(ret)
    {
    	printf("Phy init failed! err %x.\n", ret);
    	return ret;
    }
    
    return 0;
}

#endif //CONFIG_DM_ETH

#define ETH_PERF_TEST
/* Send a packet at address */
static int sirius_eth_perf_test(struct eth_device *dev, void *packet, int length)
{
    struct sirius_eth_dev * priv = (struct sirius_eth_dev *)dev->priv;
    struct sirius_eth_bd  * txbd = (struct sirius_eth_bd  *)priv->tx_mac_descrtable;

    printf("Sirius eth start sending...\n");
#ifdef ETH_PERF_TEST
    int i = 0, pktlen = 64;
    int idx = 0;
    unsigned long long time2 = 0;
for(pktlen=1024; pktlen<1518; pktlen+=16)
{
    length = pktlen; 
    unsigned long long time1 = get_ticks();
    printf("Ticks before sending: 0x%llx\n", time1);
    //send SIRIUS_ETH_TXBD_NUM packets
    for(i = 0; i<SIRIUS_ETH_TXBD_NUM; i++) //64 x 1KB x 8bit = 512k bit
    {
#endif
	update_tail(priv);
	if(priv->tx_full)
	{
		printf("TX descriptor is full, stop!\n");
		return -1;
	}
        
        txbd[priv->tx_head].addr = (unsigned long)packet;
        txbd[priv->tx_head].len_status &= 0x0000ffff & ~ETH_TX_BD_STATS;
        txbd[priv->tx_head].len_status |= length << 16 | ETH_TX_BD_READY | ETH_TX_BD_PAD | ETH_TX_BD_CRC;
	if(priv->tx_head == SIRIUS_ETH_TXBD_NUM - 1)
		txbd[priv->tx_head].len_status |= ETH_RX_BD_WRAP;

	tx_push(priv);           
#ifdef ETH_PERF_TEST   
    }
    //check all the desc are transmitted.
    while(1)
    {
        for(i=0; i<SIRIUS_ETH_TXBD_NUM; i++)
	{
		if(txbd[i].len_status & ETH_TX_BD_READY)
			break;
	}
	if(i == SIRIUS_ETH_TXBD_NUM)
	{
		printf("Eth send 128 pkt finished!\n");
		break;
	}
    }
    time2 = get_ticks();
    printf("Ticks spend in sending %d size: %lld\n", length, time2-time1);
}
#endif	
}


//#define SIRIUS_ETHERNET_DEBUG
/* ---[ debuging helpers ]----------------------------------------------- */
#ifdef SIRIUS_ETHERNET_DEBUG
void sirius_test_show_tx_bd(int start, int max)
{
    int i;
    
    for(i = start; i <= max; i++) 
    {
      /* Read Tx BD */
        printf ("LEN:%04lx", readl(SIRIUS_ETH_BD_BASE + (i << 3)) >> 16);
        printf (" RD:%04lx", (readl(SIRIUS_ETH_BD_BASE + (i << 3)) >> 15) & 0x1);
        printf (" IRQ:%04lx", (readl(SIRIUS_ETH_BD_BASE + (i << 3)) >> 14) & 0x1);
        printf (" WR:%04lx", (readl(SIRIUS_ETH_BD_BASE + (i << 3)) >> 13) & 0x1);
        printf (" PAD:%04lx", (readl(SIRIUS_ETH_BD_BASE + (i << 3)) >> 12) & 0x1);
        printf (" CRC:%04lx", (readl(SIRIUS_ETH_BD_BASE + (i << 3)) >> 11) & 0x1);
        printf (" UR:%04lx", (readl(SIRIUS_ETH_BD_BASE + (i << 3)) >> 8) & 0x1);
        printf (" RTRY:%04lx", (readl(SIRIUS_ETH_BD_BASE + (i << 3)) >> 4) & 0xf);
        printf (" RL:%04lx", (readl(SIRIUS_ETH_BD_BASE + (i << 3)) >> 3) & 0x1);
        printf (" LC:%04lx", (readl(SIRIUS_ETH_BD_BASE + (i << 3)) >> 2) & 0x1);
        printf (" DF:%04lx", (readl(SIRIUS_ETH_BD_BASE + (i << 3)) >> 1) & 0x1);
        printf (" CS:%04lx", (readl(SIRIUS_ETH_BD_BASE + (i << 3)) >> 0) & 0x1);
        printf ("\nTx Buffer Pointer: %08lx\n", readl(SIRIUS_ETH_BD_BASE + (i << 3) + 4));
    }
}

void sirius_test_show_rx_bd (int start, int max)
{
  int i;
  unsigned long rx_bd_base, rx_bd_num;

  rx_bd_num =  readl(dev->iobase + SIRIUS_ETH_RXBD_NUM);
  rx_bd_base = SIRIUS_ETH_BD_BASE + (rx_bd_num << 2);

  for(i = start; i <= max; i++){
    /* Read Rx BD */
    printf ("LEN:%04lx", readl(rx_bd_base + (i << 3)) >> 16);
    printf (" E:%04lx", (readl(rx_bd_base + (i << 3)) >> 15) & 0x1);
    printf (" IRQ:%04lx", (readl(rx_bd_base + (i << 3)) >> 14) & 0x1);
    printf (" WR:%04lx", (readl(rx_bd_base + (i << 3)) >> 13) & 0x1);
    printf (" M:%04lx", (readl(rx_bd_base + (i << 3)) >> 7) & 0x1);
    printf (" OR:%04lx", (readl(rx_bd_base + (i << 3)) >> 6) & 0x1);
    printf (" IS:%04lx", (readl(rx_bd_base + (i << 3)) >> 5) & 0x1);
    printf (" DN:%04lx", (readl(rx_bd_base + (i << 3)) >> 4) & 0x1);
    printf (" TL:%04lx", (readl(rx_bd_base + (i << 3)) >> 3) & 0x1);
    printf (" SF:%04lx", (readl(rx_bd_base + (i << 3)) >> 2) & 0x1);
    printf (" CRC:%04lx", (readl(rx_bd_base + (i << 3)) >> 1) & 0x1);
    printf (" LC:%04lx", (readl(rx_bd_base + (i << 3)) >> 0) & 0x1);
    printf ("\nRx Buffer Pointer: %08lx\n", readl(rx_bd_base + (i << 3) + 4));
  }
}

void sirius_test_print_packet(unsigned long add, int len)
{
  int i;
 
  printf("ipacket: add = %lx len = %d\n", add, len);
  for(i = 0; i < len; i++) {
      if(!(i % 16))
          printf("\n");
      printf(" %.2x", *(((unsigned char *)add) + i));
  }
  printf("\n");
}

void sirius_test_send_packet(unsigned long len, unsigned long start_data, int num_of_packets)
{
  unsigned long i, TxBD;
  unsigned char *eth_mac = config_get_eth_mac();

  while (num_of_packets--) {
    unsigned long *data = (unsigned long *)eth_get_tx_buf ();

    /* Set dest & src address */
    *data++ = dest_mac_addr[0] << 24 |
              dest_mac_addr[1] << 16 |
              dest_mac_addr[2] << 8  |
              dest_mac_addr[3] << 0;

    *data++ = dest_mac_addr[4] << 24 |
              dest_mac_addr[5] << 16 |
              eth_mac[0]       << 8  |
              eth_mac[1]       << 0;

    *data++ = eth_mac[2]       << 24 |
              eth_mac[3]       << 16 |
              eth_mac[4]       << 8  |
              eth_mac[5]       << 0;

    /* Write data to buffer */
    for(i = 12; i < len; i += 4)
      *data++ = (i + start_data - 12) << 24 | (i + start_data + 1 - 12) << 16 |
      	        (i + start_data + 2 - 12) << 8 | (i + start_data + 3 - 12);

    eth_send (data, len);
    printf (".");
  }
}


int sirius_test_show_mac_regs(struct eth_device *dev)
{ 
    printf ("\n %08x", dev->iobase + ETH_MODER);
    printf (" MODER: %08lx",readl(dev->iobase + ETH_MODER));
    
    printf ("\n %08x", dev->iobase + ETH_INT);
    printf (" INT: %08lx", readl(dev->iobase + ETH_INT));
    
    printf ("\n %08x", dev->iobase + ETH_INT_MASK);
    printf (" INT_MASK: %08lx", readl(dev->iobase + ETH_INT_MASK));
    
    printf ("\n %08x", dev->iobase + ETH_IPGT);
    printf (" IPGT: %08lx", readl(dev->iobase + ETH_IPGT));
    
    printf ("\n %08x", dev->iobase + ETH_IPGR1);
    printf (" IPGR1: %08lx", readl(dev->iobase + ETH_IPGR1));
    
    printf ("\n %08x", dev->iobase + ETH_IPGR2);
    printf (" IPGR2: %08lx", readl(dev->iobase + ETH_IPGR2));
    
    printf ("\n %08x", dev->iobase + ETH_PACKETLEN);
    printf (" PACKETLEN: %08lx", readl(dev->iobase + ETH_PACKETLEN));
    
    printf ("\n %08x", dev->iobase + ETH_COLLCONF);
    printf (" COLLCONF: %08lx", readl(dev->iobase + ETH_COLLCONF));
    
    printf ("\n %08x", dev->iobase + SIRIUS_ETH_RXBD_NUM);
    printf (" RX_BD_NUM: %08lx", readl(dev->iobase + SIRIUS_ETH_RXBD_NUM));
    
    printf ("\n %08x", dev->iobase + ETH_CTRLMODER);
    printf (" CTRLMODER: %08lx", readl(dev->iobase + ETH_CTRLMODER));
    
    printf ("\n %08x", dev->iobase + ETH_MIIMODER);
    printf (" MIIMODER: %08lx", readl(dev->iobase + ETH_MIIMODER));
    
    printf ("\n %08x", dev->iobase + ETH_MIICOMMAND);
    printf (" MIICOMMAND: %08lx", readl(dev->iobase + ETH_MIICOMMAND));
    
    printf ("\n %08x", dev->iobase + ETH_MIIADDRESS);
    printf (" MIIADDRESS: %08lx", readl(dev->iobase + ETH_MIIADDRESS));
    
    printf ("\n %08x", dev->iobase + ETH_MIITX_DATA);
    printf (" MIITX_DATA: %08lx", readl(dev->iobase + ETH_MIITX_DATA));
    
    printf ("\n %08x", dev->iobase + ETH_MIIRX_DATA);
    printf (" MIIRX_DATA: %08lx", readl(dev->iobase + ETH_MIIRX_DATA));
    
    printf ("\n %08x", dev->iobase + ETH_MIISTATUS);
    printf (" MIISTATUS: %08lx", readl(dev->iobase + ETH_MIISTATUS));
    
    printf ("\n %08x", dev->iobase + ETH_MAC_ADDR0);
    printf (" MAC_ADDR0: %08lx", readl(dev->iobase + ETH_MAC_ADDR0));
    
    printf ("\n %08x", dev->iobase + ETH_MAC_ADDR1);
    printf (" MAC_ADDR1: %08lx", readl(dev->iobase + ETH_MAC_ADDR1));
    
    printf ("\n %08x", dev->iobase + ETH_HASH_ADDR0);
    printf (" ETH_HASH_ADDR0: %08lx", readl(dev->iobase + ETH_HASH_ADDR0));
    
    printf ("\n %08x", dev->iobase + ETH_HASH_ADDR1);
    printf (" ETH_HASH_ADDR1: %08lx", readl(dev->iobase + ETH_HASH_ADDR1));
    
    printf ("\n");
    return 0;
}
#endif
