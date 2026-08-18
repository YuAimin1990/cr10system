

#ifndef __ARTOSYN_QSPI_H
#define __ARTOSYN_QSPI_H
#include <linux/mtd/spi-nor.h>


#define NOR_DMA (1)

struct art_nor_host {
	struct device		*dev;
	/* Dma regs */
	void __iomem	*nor_dma_base;
	void __iomem	*nor_dma_base_low;
	dma_addr_t		dma_buffer;
	u_char			*temp_buf;
	size_t			temp_buf_len;
	struct ahb_dma_llp 	*dma_llp;
};



struct artosyn_qspi {
	/* Qspi register registers */
	void *regs;
	/* Flash memory map */
	void *mem;
	phys_addr_t map_phys;
	struct spi_nor nor;
	struct art_nor_host *host;
	u32 clk_rate;
	u32 read_dummy_cycle;
	uint8_t id[8];
#ifdef CONFIG_ARTOSYN_QSPI_ENHANCE_PATCH
	int pin_no;
	int pin_func;
	int gpio;
#endif
	int flag_mutex;
};


#endif /* __MXC_NAND_H */


