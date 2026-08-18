#include "vo/util.h"
#include <asm/gpio.h>

static int queue_pop(ar_queue_t * queue, void **item);
static int queue_insert(ar_queue_t * queue,void *item);
static int get_queue_size(ar_queue_t * queue);
static int look_up_head_item(ar_queue_t * queue, void **item);

extern int gpio_direction_output(unsigned gpio, int value);
extern int gpio_set_value(unsigned gpio, int value);

ar_queue_t *ar_creat_queue(uint32_t queue_size,char *name)
{
  ar_queue_t *queue=NULL;
  queue=(ar_queue_t*)ar_malloc(sizeof(ar_queue_t));
  if(queue&&queue_size>=2&&name)
  {
     void **data=NULL;
     data=(void **)ar_malloc(sizeof(void *)*queue_size);
     if(data)
     {
       queue->name=name;
       queue->data=data;
       queue->queue_size=queue_size;
       queue->header=0;
       queue->tail=queue->header;
       queue->valid_data_num=0;
       queue->queue_insert=queue_insert;
       queue->queue_pop=queue_pop;
       queue->get_queue_size=get_queue_size;
       queue->look_up_head=look_up_head_item;
       return queue;
     }else
     {
       if(queue)
           ar_free(queue);
     }
  }else
  {
    if(queue)
        ar_free(queue);
  }
  ar_err("creat queue failed");
  return NULL;
}
static int queue_pop(ar_queue_t * queue, void **item)
{
    if(!queue||!item)
    {
      return -1;
    }

    if(queue->valid_data_num==0)
    {
      //set to NULL init
      *item=NULL;
      return -1;
    }
    *item=queue->data[queue->header];
    queue->header=(queue->header+1)%queue->queue_size;
    queue->valid_data_num--;
    return 0;
}

static int get_queue_size(ar_queue_t * queue)
{
    int num=0;
    if(!queue)
    {
      return -1;
    }
    num=queue->valid_data_num;
    return num;
}

static int look_up_head_item(ar_queue_t * queue, void **item)
{
    int num=0;
    if(!queue)
    {
      return -1;
    }

    if(queue->valid_data_num==0)
    {
      //set to NULL init
      *item=NULL;
      return -1;
    }
    *item=queue->data[queue->header];
    return num;
}

static int queue_insert(ar_queue_t *queue,void *item)
{
   if(!queue||!item)
   {
      return -1;
   }
   if(queue->valid_data_num==queue->queue_size)
   {
      ar_err("queue %s size=%d is full",queue->name,queue->queue_size);
      return -1;
   }
   queue->data[queue->tail]=item;
   queue->tail=(queue->tail+1)%queue->queue_size;
   queue->valid_data_num++;
   return 0;
}

int ar_delete_queue(ar_queue_t **queue)
{
   ar_queue_t *p_queue=*queue;

   if(p_queue)
    {
       if(p_queue->data)
       {
         ar_free(p_queue->data);
         p_queue->data=NULL;
       }
      ar_free(p_queue);
      *queue=NULL;
    }
   return 0;
}

void write_reg32_Mask(uint32_t regAddr, uint32_t regData, uint32_t regDataMask)
{
    uint32_t u32_regDataTmp;
    volatile uint32_t* ptr_regAddr = (uint32_t*)regAddr;
    uint32_t u32_regDataWithMask = regData & regDataMask;

    u32_regDataTmp = *ptr_regAddr;
    u32_regDataTmp &= ~regDataMask;
    u32_regDataTmp |= u32_regDataWithMask;

    *ptr_regAddr = u32_regDataTmp;
}


void read_write_reg32_mask(uint32_t reg_addr, uint32_t reg_data, uint32_t mask)
{
    uint32_t old = (read_reg32(reg_addr) & (~mask));
    write_reg32(reg_addr, (old | (reg_data & mask)));
}

void ar_lock(ar_lock_t lock_id)
{
    return;
}

void ar_unlock(ar_lock_t lock_id)
{
    return;
}

ar_lock_t ar_creat_lock(void)
{
    return 0;
}

ar_signal_t ar_create_signal(void)
{
    return 0;
}

uint64_t ar_get_timestamp_us(void)
{
    return 0;
}

void ar_enter_critical(void)
{
}

void ar_exit_critical(void)
{
}

void ar_trace_line(void)
{
}
#ifdef CONFIG_ARTOSYN_AR9301
int gpio_set_direct(uint32_t group, uint32_t port, uint32_t inner_gpio, uint32_t dir)
{
    uint32_t  addr;
    uint32_t  value;

    if ((group >= MAX_GPIO_GROUP) || (port >= MAX_GPIO_PORT) ||
            (inner_gpio>=MAX_GPIO_PIN))
        return -1;

    addr = GPIO_DIR_BASE + group*GPIO_GROUP_STEP + port*GPIO_PORT_DIR_STEP;
    ar_debug("set GPIO_%c%d_%d(addr=0x%x) dir=%d", 'A'+port, group, inner_gpio, addr, dir);
    value = read_reg32(addr);
    if (dir == GPIO_DIR_OUTPUT)
        value |= (1<<inner_gpio);
    else
        value &= (~(1<<inner_gpio));

    write_reg32(addr, value);
    return 0;
}

int gpio_set_val(uint32_t group, uint32_t port, uint32_t inner_gpio, uint32_t val)
{
    uint32_t  addr;
    uint32_t  value;

    if ((group >= MAX_GPIO_GROUP) || (port >= MAX_GPIO_PORT) ||
            (inner_gpio>=MAX_GPIO_PIN))
        return -1;

    addr = GPIO_OUT_BASE + group*GPIO_GROUP_STEP + port*GPIO_PORT_DIR_STEP;
    ar_debug("set GPIO_%c%d_%d(addr=0x%x) val=%d", 'A'+port, group, inner_gpio, addr, val);
    value = read_reg32(addr);
    if (val == GPIO_DATA_HIGH)
        value |= (1<<inner_gpio);
    else
        value &= (~(1<<inner_gpio));

    ar_debug("addr:%02x value:%02x", addr, value);

    write_reg32(addr, value);
    return 0;
}
#endif

#ifdef CONFIG_ARTOSYN_AR9311
static unsigned char group_num[MAX_GPIO_PORT] = {23, 22, 26, 6, 6, 11, 16};

int gpio_set_direct(uint32_t group, uint32_t port, uint32_t inner_gpio, uint32_t dir)
{
    uint32_t  addr;
    uint32_t  value;

    if ((group >= MAX_GPIO_GROUP) || (port >= MAX_GPIO_PORT) ||
            (inner_gpio>=group_num[port]))
        return -1;

    addr = GPIO_DIR_BASE + port*GPIO_PORT_DIR_STEP;
    ar_debug("set GPIO_%c%d_%d(addr=0x%x) dir=%d", 'A'+port, group, inner_gpio, addr, dir);
    value = read_reg32(addr);
    if (dir == GPIO_DIR_INPUT)
        value |= (1<<inner_gpio);
    else
        value &= (~(1<<inner_gpio));

    ar_debug("addr:%02x value:%02x", addr, value);

    write_reg32(addr, value);

#if 0
    struct gpio_desc desc;
    char name[32]={0};
    char label[32]={0};
    int ret;

    memset(&desc, 0, sizeof(desc));

    snprintf(name, 32, "port%c0_%d", 'a'+port, inner_gpio);
    snprintf(label, 32, "%c0_%d", 'a'+port, inner_gpio);

    ret = dm_gpio_lookup_name(name, &desc);
    if (ret) {
        printf("Can't lookup request name  %s gpios\n", name);
        return ret;
    }

    ret = dm_gpio_request(&desc, label);
    if (ret) {
        printf("Can't lookup request label %s gpios\n", label);
        return ret;
    }

    dm_gpio_set_dir_flags(&desc, dir ? GPIOD_IS_OUT : GPIOD_IS_IN);
#endif
    return 0;
}

int gpio_set_val(uint32_t group, uint32_t port, uint32_t inner_gpio, uint32_t val)
{
    uint32_t  addr;
    uint32_t  value;

    if ((group >= MAX_GPIO_GROUP) || (port >= MAX_GPIO_PORT) ||
            (inner_gpio>=group_num[port]))
        return -1;

    addr = GPIO_OUT_BASE + port*GPIO_PORT_DIR_STEP;
    ar_debug("set GPIO_%c%d_%d(addr=0x%x) val=%d", 'A'+port, group, inner_gpio, addr, val);
    value = read_reg32(addr);
    if (val == GPIO_DATA_HIGH)
        value |= (1<<inner_gpio);
    else
        value &= (~(1<<inner_gpio));

    write_reg32(addr, value);

#if 0
    struct gpio_desc desc;
    char name[32]={0};
    char label[32]={0};
    int ret;

    memset(&desc, 0, sizeof(desc));

    snprintf(name, 32, "port%c0_%d", 'a'+port, inner_gpio);
    snprintf(label, 32, "%c0_%d", 'a'+port, inner_gpio);;

    ret = dm_gpio_lookup_name(name, &desc);
    if (ret) {
        printf("Can't lookup request name %s gpios\n", name);
        return ret;
    }

    ret = dm_gpio_request(&desc, label);
    if (ret) {
        printf("Can't lookup request label %s gpios\n", label);
        return ret;
    }

    dm_gpio_set_value(&desc, val);
#endif
    return 0;
}
#endif

static void ar9301_abb_set_pixel(u32 freq)
{
	u32 loop_div, post_div;
    u32 abb_core_reg38;

    ar_debug("freq: %u", freq);

	if (freq > 600000000 || freq < 6250000) {
		ar_err("Unsupport abb pix freq %d\n", freq);
		return;
	}

    abb_core_reg38 = read_reg32(0x1072098);
	abb_core_reg38 &= ~0x3f;
    write_reg32(0x1072098, abb_core_reg38);

	if (freq > 400000000) {
		loop_div = 5;
		post_div = 1;
	} else if (freq > 200000000) {
		loop_div = 5;
		post_div = 2;
	} else if (freq > 100000000) {
		loop_div = 5;
		post_div = 3;
	} else if (freq > 50000000) {
		loop_div = 5;
		post_div = 4;
	} else if (freq > 25000000) {
		loop_div = 5;
		post_div = 5;
	} else if (freq > 12500000) {
		loop_div = 5;
		post_div = 6;
	} else if (freq >= 6250000) {
		loop_div = 4;
		post_div = 7;
	}

    write_reg32(0x1072098, (loop_div | (post_div << 3)));
    write_reg32(0x1072a10, (u32)(((u64)400000000) *
			       ((u64)(1 << (24 + loop_div - post_div))) /
			       (u64)freq));
}

#ifdef CONFIG_ARTOSYN_AR9311

#define SYS_PMU_CTRL          0x0a106200
#define SYS_IP_PMU_CTRL       0x0a106204
#define SYS_RSZ_CTRL          0x0a106010
#define RGU_RESET0_LEVEL      0x0a102004
#define RGU_RESET1_LEVEL      0x0a102014
#define CGU_CLK_EN_00         0x0a104100
#define CGU_CLK_EN_03         0x0a10410c
#define CGU_CLK_DE            0x0a104014

#define CGU_CLK_DVP_SUB_1_2X  0x0a10404c

#define MIPI_PLL_LOOP_DIV     20
#define MIPI_DSI_BASE_ADDR    0x8850000

void ar9311_pwrctrl_vision_on(void)
{
    u32 value = 0xacce55;
    write_reg32(SYS_PMU_CTRL, value);

    value = read_reg32(SYS_IP_PMU_CTRL);
    // powen up
    value |= (1 << 11);
    write_reg32(SYS_IP_PMU_CTRL, value);
    value |= (1 << 3);
    write_reg32(SYS_IP_PMU_CTRL, value);
    udelay(20);
    value &= ~(1 << 19);
    write_reg32(SYS_IP_PMU_CTRL, value);

    value = read_reg32(RGU_RESET0_LEVEL);
    value |= (1 << 13 | 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17 | 1 << 18 | 1 << 19 | 1 << 30);
    write_reg32(RGU_RESET0_LEVEL, value);
    value |= (1 << 4 | 1 << 6 | 1 << 17 | 1 << 18 | 1 << 20 |
              1 << 21 | 1 << 22 | 1 << 23 | 1 << 24 | 1 << 30 | 1 << 31);
    write_reg32(RGU_RESET1_LEVEL, value);

    value = read_reg32(SYS_RSZ_CTRL);
    value |= 0xf00;
    write_reg32(SYS_RSZ_CTRL, value);

    value = read_reg32(SYS_RSZ_CTRL);

    //enable clk
    value = read_reg32(CGU_CLK_DE);
    value |= 1 << 28;
    write_reg32(CGU_CLK_DE, value);
    value = read_reg32(CGU_CLK_DVP_SUB_1_2X);
    value |= 1 << 12;
    write_reg32(CGU_CLK_DVP_SUB_1_2X, value);

    value = read_reg32(CGU_CLK_EN_00);
    value |= (1 << 1);
    write_reg32(CGU_CLK_EN_00, value);

    value = read_reg32(CGU_CLK_EN_03);
    value |= (1 << 1 | 1 << 5);
    write_reg32(CGU_CLK_EN_03, value);

    value = read_reg32(RGU_RESET1_LEVEL);
    value |= (1 << 14 | 1 << 19);
    write_reg32(RGU_RESET1_LEVEL, value);

}

int get_mipi_pll_post_div_num(unsigned int freq)
{
    unsigned int post_div_num = 0;

    ar_debug("get_mipi_pll_post_div_num freq:%d\n", freq);

    if(freq > 1250000000)
        return -1;
    else if(freq > 500000000)
        post_div_num = 1;
    else if(freq > 250000000)
        post_div_num = 2;
    else if(freq > 125000000)
        post_div_num = 3;
    else if(freq > 62500000)
        post_div_num = 4;
    else
        return -1;

    return post_div_num;
}

int ar9311_abb_set_mipi_pll(unsigned int freq)
{
    unsigned int post_div_num = 1;//max 4aaaaaa
    unsigned int post_div = 0;
    float fcw = 0;
    unsigned int vco_freq = 0;
    u32 value;
    double v_2c0_t = 0;

    ar_debug("0 ar9311_abb_set_mipi_pll freq:%d\n", freq);

    post_div_num = get_mipi_pll_post_div_num(freq);

    if(-1 == post_div_num)
        return -1;

    post_div = 1 << post_div_num;

    vco_freq = freq * post_div;
    if(vco_freq<1000000000 || vco_freq>2500000000)
        return -1;

    fcw = 400000000.0 * MIPI_PLL_LOOP_DIV / (freq * post_div);
    if(fcw < 3.2 || fcw > 8)
        return -1;

    v_2c0_t = (1 << 24) * 400000000.0 * 20.0 / vco_freq;

    ar_debug("post_div:%d vco_freq:%u fcw:%d\n", post_div, vco_freq, (int)fcw*1000000);
    /* MIPITX_PHY_RESET */
    value = read_reg32(MIPI_DSI_BASE_ADDR + 0xa0);
    //value |= 1 << 1;
    value = 0x0f;
    write_reg32(MIPI_DSI_BASE_ADDR + 0xa0, value);
    ar_debug("mipi addr:0x%02x value:0x%02x\n", MIPI_DSI_BASE_ADDR + 0xa0, value);

    /* MIPIPLL_DIG_REG0 */
    value = read_reg32(MIPI_DSI_BASE_ADDR + 0x2c0);
    value = (int)v_2c0_t;
    write_reg32(MIPI_DSI_BASE_ADDR + 0x2c0, value);
    ar_debug("mipi addr:0x%02x value:0x%02x\n", MIPI_DSI_BASE_ADDR + 0x2c0, value);

    /* MIPIPLL_DIG_REG1 */
    value = read_reg32(MIPI_DSI_BASE_ADDR + 0x2c4);
    value = 0x0010B00E | 1<<4;
    write_reg32(MIPI_DSI_BASE_ADDR + 0x2c4, value);

    /* MIPIPLL_ANA_REG2_0 */
    value = read_reg32(MIPI_DSI_BASE_ADDR + 0x38c);
    value |= (MIPI_PLL_LOOP_DIV << 8);
    write_reg32(MIPI_DSI_BASE_ADDR + 0x38c, value);
    value |= (post_div_num << 16);
    write_reg32(MIPI_DSI_BASE_ADDR + 0x38c, value);
    ar_debug("mipi addr:%02x value:%02x\n", MIPI_DSI_BASE_ADDR + 0x38c, value);

    return 0;
}

/* attention!!!: mipitx_pll_div2 must more than 500MHz. */
int ar9311_abb_set_mipi_div2(unsigned int freq)
{
    unsigned int post_div_num = 0; //1~4
    unsigned int mipi_pll_freq = 0;

    for(post_div_num=1 ; post_div_num<5; ++post_div_num)
    {
        mipi_pll_freq = freq * 2 / (1 << post_div_num);
        if(post_div_num == get_mipi_pll_post_div_num(mipi_pll_freq))
        {
            if(0 == ar9311_abb_set_mipi_pll(mipi_pll_freq))
            {
                return 0;
            }
        }
    }

    return -1;
}

unsigned int ar9311_abb_get_mipi_pll(void)
{
    u32 value_38c;
    u32 value_2c0;
    unsigned int freq = 0;
    unsigned int post_div_num = 0;
    unsigned int post_div = 0;

    value_38c = read_reg32(MIPI_DSI_BASE_ADDR + 0x38c);
    post_div_num = (value_38c >> 16) & 0xff;

    post_div = 1 << post_div_num;

    value_2c0 = read_reg32(MIPI_DSI_BASE_ADDR + 0x2c0);
    freq = 400000000.0 * 20 / value_2c0 / post_div * (1 << 24);

    ar_debug("ar9311_abb_get_mipi_pll freq:%d\n", freq);

    return freq;
}

unsigned int ar9311_abb_get_mipi_div2(void)
{
    u32 value_38c;
    unsigned int freq;
    unsigned int mipi_pll_freq;
    unsigned int post_div;

    mipi_pll_freq = ar9311_abb_get_mipi_pll();
    value_38c = read_reg32(MIPI_DSI_BASE_ADDR + 0x38c);
    post_div = 1 << (value_38c >> 16 & 0xff);

    freq = mipi_pll_freq  * post_div / 2;
    ar_debug("ar9311_abb_get_mipi_div2 freq:%d\n", freq);
    return freq;
}

void ar9311_abb_set_pixel(u32 freq)
{
    u32 value;
    unsigned int source_freq, div, remainder;
    source_freq = ar9311_abb_get_mipi_div2();
    value = read_reg32(CGU_CLK_DVP_SUB_1_2X);

    ar_debug("ar9311_abb_set_pixel freq:%d\n", freq);

    //bind 7
    value |= 7 << 8;

    div = source_freq / freq - 1;
    remainder = source_freq % freq;

    if(remainder > freq*0.8)
        div++;

    ar_debug("ar9311_abb_set_pixel div:%d\n", div);
    value &= ~(0x3f);
    value |= (0x7f & div);
    //enable
    value |= 1 << 12;

    write_reg32(CGU_CLK_DVP_SUB_1_2X, value);
    ar_debug("ar9311_abb_set_pixel addr:%02x value:%02x\n", CGU_CLK_DVP_SUB_1_2X, value);
}
#endif

int ar_hal_set_pix_clk(float clk)
{
#ifdef CONFIG_ARTOSYN_AR9301
    ar9301_abb_set_pixel(clk*1000000);
#endif
#ifdef CONFIG_ARTOSYN_AR9311
    ar9311_abb_set_pixel(clk*1000000);
#endif
    return 0;
}

