#include <linux/acpi.h>
#include <linux/gpio/driver.h>
/* FIXME: for gpio_get_value(), replace this with direct register read */
#include <linux/gpio.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/spinlock.h>
#include <linux/platform_data/gpio-artosyn.h>
#include <linux/slab.h>

#include "gpiolib.h"

#define GPIO_DAT_PORTA		0xc4
#define GPIO_DAT_PORTB		0xd0
#define GPIO_DAT_PORTC		0xdc
#define GPIO_DAT_PORTD		0xe8
#define GPIO_DAT_PORTE		0xf4
#define GPIO_DAT_PORTF		0x100
#define GPIO_DAT_PORTG		0x10c

#define GPIO_SET_PORTA		0xbc
#define GPIO_SET_PORTB		0xc8

#define GPIO_DIR_PORTA		0xc0
#define GPIO_DIR_PORTB		0xcc

#define GPIO_INTR_MUX0		0x110
#define GPIO_INTR_MUX1		0x114
#define GPIO_INTR_ENABLE	0x118
#define GPIO_INTR_POLARITY	0x11c
#define GPIO_INTR_TYPE		0x120
#define GPIO_INTR_MASK		0x124
#define GPIO_INTR_EOI		0x12c
#define GPIO_INTR_RAWSTATUS	0x130

#define GPIO_DEBOUNCE		0x128

#define ARTOSYN_MAX_PORTS	7
#define GPIO_DAT_PORT_SIZE	(GPIO_DAT_PORTB - GPIO_DAT_PORTA)
#define GPIO_SET_PORT_SIZE	(GPIO_SET_PORTB - GPIO_SET_PORTA)
#define GPIO_DIR_PORT_SIZE	(GPIO_DIR_PORTB - GPIO_DIR_PORTA)

#define ARTOSYN_GPIO_CLOCK	(150 * 1000 * 1000)

struct artosyn_gpio;

#ifdef CONFIG_PM_SLEEP
/* Store GPIO context across system-wide suspend/resume transitions */
struct artosyn_context {
	u32 data;
	u32 dir;
};

struct artosyn_irq_context {
	u32 int_mux[ARTOSYN_MAX_INTERRUPTS / 4];
	u32 int_en;
	u32 int_mask;
	u32 int_type;
	u32 int_pol;
	u32 int_deb;
};
#endif

struct artosyn_gpio_port {
	struct gpio_chip	gc;
	bool			is_registered;
	struct artosyn_gpio	*gpio;
#ifdef CONFIG_PM_SLEEP
	struct artosyn_context	*ctx;
#endif
	unsigned int		idx;
};

struct artosyn_gpio {
	struct	device		*dev;
	void __iomem		*regs;
	struct artosyn_gpio_port	*ports;
	unsigned int		nr_ports;
	unsigned int		n_totalgpio;
	struct irq_domain	**domain;
	struct irq_chip		ic;
	int 				irq_used[ARTOSYN_MAX_INTERRUPTS];
	struct artosyn_platform_data *pdata;

	spinlock_t 			irq_lock;
	spinlock_t 			lvl_lock;
#ifdef CONFIG_PM_SLEEP
	struct artosyn_irq_context	*irq_ctx;
#endif
};

static struct lock_class_key gpio_lock_class;

static inline u32 artosyn_read(struct artosyn_gpio *gpio, unsigned int offset)
{
	struct gpio_chip *gc	= &gpio->ports[0].gc;
	void __iomem *reg_base	= gpio->regs;

	return gc->read_reg(reg_base + offset);
}

static inline void artosyn_write(struct artosyn_gpio *gpio, unsigned int offset,
				   u32 val)
{
	struct gpio_chip *gc	= &gpio->ports[0].gc;
	void __iomem *reg_base	= gpio->regs;

	gc->write_reg(reg_base + offset, val);
}

static int artosyn_gpio_to_irq(struct gpio_chip *gc, unsigned offset)
{
	struct artosyn_gpio_port *port = gpiochip_get_data(gc);
	struct artosyn_gpio *gpio = port->gpio;
	/*int i;

	for(i = 0; i < port->idx; ++i)
	{
	   offset += gpio->ports[i].gc.ngpio;
	}*/

	return irq_find_mapping(gpio->domain[port->idx], offset);
}

static int artosyn_irq_data_to_real_hwirq(struct artosyn_gpio *gpio, struct irq_data *d)
{
	int i = 0, real_offset = 0;

	for(i = 0; i < gpio->nr_ports; ++i)
	{
		if(d->domain == gpio->domain[i])
			break;
		real_offset += gpio->ports[i].gc.ngpio;
	}
	return real_offset + d->hwirq;
}

static int artosyn_gpio_set_debounce(struct gpio_chip *gc,
				  unsigned offset, unsigned debounce)
{
	struct artosyn_gpio_port *port = gpiochip_get_data(gc);
	struct artosyn_gpio *gpio = port->gpio;
	unsigned long flags, val_deb;
	//unsigned long mask = gc->pin2mask(gc, offset);

	spin_lock_irqsave(&gc->bgpio_lock, flags);

	debounce *= (ARTOSYN_GPIO_CLOCK / 1000);
	debounce = (debounce > 0x7) ? 0x7 : debounce;

	//set maxmium debounce val for gpio
	val_deb = artosyn_read(gpio, GPIO_DEBOUNCE);
	if(val_deb < debounce)
	{
		//artosyn gpio max debounce is 7 * 1 /150000000 s
		//unsigned debounce unit is ms, so we always set max.
		artosyn_write(gpio, GPIO_DEBOUNCE, debounce);
	}

	spin_unlock_irqrestore(&gc->bgpio_lock, flags);

	return 0;
}

static int artosyn_gpio_add_port(struct artosyn_gpio *gpio,
				   struct artosyn_port_property *pp,
				   unsigned int offs)
{
	struct artosyn_gpio_port *port;
	void __iomem *dat, *set, *dirin;
	int err;

	port = &gpio->ports[offs];
	port->gpio = gpio;
	port->idx = pp->idx;

#ifdef CONFIG_PM_SLEEP
	port->ctx = devm_kzalloc(gpio->dev, sizeof(*port->ctx), GFP_KERNEL);
	if (!port->ctx)
		return -ENOMEM;
#endif

	dat = gpio->regs + GPIO_DAT_PORTA + (pp->idx * GPIO_DAT_PORT_SIZE);
	set = gpio->regs + GPIO_SET_PORTA + (pp->idx * GPIO_SET_PORT_SIZE);
	dirin = gpio->regs + GPIO_DIR_PORTA +
		(pp->idx * GPIO_DIR_PORT_SIZE);

	err = bgpio_init(&port->gc, gpio->dev, 4, dat, set, NULL, NULL,
			 dirin, false);
	if (err) {
		dev_err(gpio->dev, "failed to init gpio chip for port%d\n",
			port->idx);
		return err;
	}

#ifdef CONFIG_OF_GPIO
	port->gc.of_node = to_of_node(pp->fwnode);
#endif
	port->gc.ngpio = pp->ngpio;
	port->gc.base = pp->gpio_base;

	port->gc.set_debounce = artosyn_gpio_set_debounce;

	err = gpiochip_add_data(&port->gc, port);
	if (err)
		dev_err(gpio->dev, "failed to register gpiochip for port%d\n",
			port->idx);
	else
		port->is_registered = true;

	gpio->n_totalgpio += pp->ngpio;

	

	//artosyn_configure_irqs(gpio, port, pp);

	/* Add GPIO-signaled ACPI event support */
	/*if (pp->irq)
		acpi_gpiochip_request_interrupts(&port->gc);*/

	return err;
}

static void artosyn_gpio_unregister(struct artosyn_gpio *gpio)
{
	unsigned int m;

	for (m = 0; m < gpio->nr_ports; ++m)
		if (gpio->ports[m].is_registered)
			gpiochip_remove(&gpio->ports[m].gc);
}

static struct artosyn_platform_data *
artosyn_gpio_get_pdata(struct device *dev)
{
	struct fwnode_handle *fwnode;
	struct artosyn_platform_data *pdata;
	struct artosyn_port_property *pp;
	int nports;
	int i;
	static int gpio_base = 0;

	nports = device_get_child_node_count(dev);
	if (nports == 0)
		return ERR_PTR(-ENODEV);

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return ERR_PTR(-ENOMEM);

	pdata->properties = devm_kcalloc(dev, nports, sizeof(*pp), GFP_KERNEL);
	if (!pdata->properties)
		return ERR_PTR(-ENOMEM);

	pdata->nports = nports;
	//pdata->fwnode = dev->fwnode;

	pdata->nr_irqs = 0;
	if (fwnode_property_read_bool(dev->fwnode,
					  "interrupt-controller")) {
	  	for(i = 0; i < ARTOSYN_MAX_INTERRUPTS; ++i) {
			pdata->irq[i] = irq_of_parse_and_map(dev->of_node, i);
			if (!pdata->irq)
				break;
			pdata->nr_irqs++;
		}
		dev_info(dev, "get %d irqs for all ports\n", i);
	}

	i = 0;
	device_for_each_child_node(dev, fwnode)  {
		pp = &pdata->properties[i++];
		pp->fwnode = fwnode;

		if (fwnode_property_read_u32(fwnode, "reg", &pp->idx) ||
			pp->idx >= ARTOSYN_MAX_PORTS) {
			dev_err(dev,
				"missing/invalid port index for port%d\n", i);
			fwnode_handle_put(fwnode);
			return ERR_PTR(-EINVAL);
		}

		if (fwnode_property_read_u32(fwnode, "arto,nr-gpios",
					 &pp->ngpio)) {
			dev_info(dev,
				 "failed to get number of gpios for port%d\n",
				 i);
			pp->ngpio = 32;
		}

		pp->gpio_base = gpio_base;
		gpio_base += pp->ngpio;
	}

	return pdata;
}

/*static int artosyn_hwirq_to_gc(struct artosyn_gpio *gpio, unsigned hwirq, struct gpio_chip **gc)
{
	int i = 0, real_offset = hwirq;

	for(i = 0; i < ARTOSYN_MAX_PORTS; ++i)
	{
		if(real_offset - gpio->ports[i].gc.ngpio < 0)
			break;
		real_offset -= gpio->ports[i].gc.ngpio;
	}
	*gc = &gpio->ports[i].gc;
	return real_offset;
}*/

static void artosyn_toggle_trigger(struct artosyn_gpio *gpio, struct artosyn_gpio_port	*port, unsigned int offs, unsigned channel)
{
	u32 v = artosyn_read(gpio, GPIO_INTR_POLARITY);
	int real_offset = offs;

	//printk(KERN_INFO "%s %d %d %d\n", __func__, __LINE__, offs, port->gc.get(&port->gc, real_offset));

	if (port->gc.get(&port->gc, real_offset))
		v &= ~BIT(channel);
	else
		v |= BIT(channel);

	artosyn_write(gpio, GPIO_INTR_POLARITY, v);
}

static irqreturn_t artosyn_irq_handler(int irq, void *dev_id)//(struct irq_desc *desc)
{
	struct artosyn_gpio *gpio = (struct artosyn_gpio *)dev_id;//irq_desc_get_handler_data(desc);
	struct artosyn_platform_data *pdata = gpio->pdata;//dev_get_platdata(gpio->dev);
	u32 irq_status = readl_relaxed(gpio->regs + GPIO_INTR_RAWSTATUS);
	//u32 irq_mask = readl_relaxed(gpio->regs + GPIO_INTR_MASK);
	u32 irq_channel = irq - pdata->irq[0];//irq_desc_get_irq(desc) - pdata->irq[0];
	//struct irq_chip *chip = irq_desc_get_chip(desc);
	u32 i = 0;

	//printk(KERN_INFO "%s %d irq status :0x%x %d\n", __func__, __LINE__, irq_status, irq_channel);
	if(irq_status & (1 << irq_channel)) {
		int hwirq = readl_relaxed(gpio->regs + GPIO_INTR_MUX0);
		int gpio_irq = 0;
		if(irq_channel >= 4) {
			hwirq = readl_relaxed(gpio->regs + GPIO_INTR_MUX1);
			hwirq = ((hwirq >> ((irq_channel - 4) * 8)) & 0xff);
		} else {
			hwirq = ((hwirq >> ((irq_channel - 0) * 8)) & 0xff);
		}

		for(i = 0; i < ARTOSYN_MAX_PORTS; ++i)
		{
			if(hwirq - gpio->ports[i].gc.ngpio < 0)
				break;
			hwirq -= gpio->ports[i].gc.ngpio;
		}

		gpio_irq = irq_find_mapping(gpio->domain[i], hwirq);

		generic_handle_irq(gpio_irq);

		if ((irq_get_trigger_type(gpio_irq) & IRQ_TYPE_SENSE_MASK)
			== IRQ_TYPE_EDGE_BOTH)
			artosyn_toggle_trigger(gpio, &gpio->ports[i], hwirq, irq_channel);

		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

static void artosyn_gpio_irq_ack(struct irq_data *d)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct artosyn_gpio *gpio = igc->private;
	int hwirq = 0;//d->hwirq;
	int i = 0;

	hwirq = artosyn_irq_data_to_real_hwirq(gpio, d);

	for(i = 0; i < ARTOSYN_MAX_INTERRUPTS; ++i)
	{
		if(hwirq == gpio->irq_used[i])
			break;
	}

	if(i >= ARTOSYN_MAX_INTERRUPTS)
	{
		return;
	}

	artosyn_write(gpio, GPIO_INTR_EOI, (1 << i));
	artosyn_write(gpio, GPIO_INTR_EOI, 0);
}

static void artosyn_gpio_irq_mask(struct irq_data *d)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct artosyn_gpio *gpio = igc->private;
	int hwirq = 0;//d->hwirq;
	int i = 0, mask = 0;
	unsigned long flags;

	hwirq = artosyn_irq_data_to_real_hwirq(gpio, d);

	//printk(KERN_INFO "%s %d %d\n", __func__, __LINE__, hwirq);

	spin_lock_irqsave(&gpio->irq_lock, flags);
	for(i = 0; i < ARTOSYN_MAX_INTERRUPTS; ++i)
	{
		if(hwirq == gpio->irq_used[i])
			break;
	}

	if(i >= ARTOSYN_MAX_INTERRUPTS)
	{
		spin_unlock_irqrestore(&gpio->irq_lock, flags);
		return;
	}

	mask = artosyn_read(gpio, GPIO_INTR_MASK);
	mask |= (1 << i);
	artosyn_write(gpio, GPIO_INTR_MASK, mask);

	spin_unlock_irqrestore(&gpio->irq_lock, flags);
}

static void artosyn_gpio_irq_unmask(struct irq_data *d)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct artosyn_gpio *gpio = igc->private;
	int hwirq = 0;//d->hwirq;
	int i = 0, mask = 0;
	unsigned long flags;

	//printk(KERN_INFO "%s %d %d\n", __func__, __LINE__, hwirq);

	hwirq = artosyn_irq_data_to_real_hwirq(gpio, d);

	spin_lock_irqsave(&gpio->irq_lock, flags);
	for(i = 0; i < ARTOSYN_MAX_INTERRUPTS; ++i)
	{
		if(hwirq == gpio->irq_used[i])
			break;
	}

	if(i >= ARTOSYN_MAX_INTERRUPTS)
	{
		spin_unlock_irqrestore(&gpio->irq_lock, flags);
		return;
	}

	mask = artosyn_read(gpio, GPIO_INTR_MASK);
	mask &= (~(1 << i));
	artosyn_write(gpio, GPIO_INTR_MASK, mask);

	spin_unlock_irqrestore(&gpio->irq_lock, flags);
}

static int artosyn_gpio_irq_set_type(struct irq_data *d, unsigned int type)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct artosyn_gpio *gpio = igc->private;
	int bit = 0;
	unsigned long level, polarity, flags;
	int i = 0;

	if (type & ~(IRQ_TYPE_EDGE_RISING | IRQ_TYPE_EDGE_FALLING |
			 IRQ_TYPE_LEVEL_HIGH | IRQ_TYPE_LEVEL_LOW))
		return -EINVAL;

	spin_lock_irqsave(&gpio->irq_lock, flags);
	for(i = 0; i < ARTOSYN_MAX_INTERRUPTS; ++i)
	{
		if(-1 == gpio->irq_used[i])
			break;

		//printk(KERN_INFO "%s %d %d\n", __func__, __LINE__, gpio->irq_used[i]);
	}

	if(i >= ARTOSYN_MAX_INTERRUPTS)
	{
		spin_unlock_irqrestore(&gpio->irq_lock, flags);
		printk(KERN_ERR "irq request more than %d\n", ARTOSYN_MAX_INTERRUPTS);
		return -EINVAL;
	}
	bit = i;
	gpio->irq_used[i] = artosyn_irq_data_to_real_hwirq(gpio, d);
	spin_unlock_irqrestore(&gpio->irq_lock, flags);

	spin_lock_irqsave(&gpio->lvl_lock, flags);
	level = artosyn_read(gpio, GPIO_INTR_TYPE);
	polarity = artosyn_read(gpio, GPIO_INTR_POLARITY);

	switch (type) {
	case IRQ_TYPE_EDGE_BOTH:
		level |= BIT(bit);
		//artosyn_toggle_trigger(gpio, d->hwirq, bit);
		break;
	case IRQ_TYPE_EDGE_RISING:
		level |= BIT(bit);
		polarity |= BIT(bit);
		break;
	case IRQ_TYPE_EDGE_FALLING:
		level |= BIT(bit);
		polarity &= ~BIT(bit);
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		level &= ~BIT(bit);
		polarity |= BIT(bit);
		break;
	case IRQ_TYPE_LEVEL_LOW:
		level &= ~BIT(bit);
		polarity &= ~BIT(bit);
		break;
	}
	irq_setup_alt_chip(d, type);

	//printk(KERN_INFO "%s %d irq status :0x%x %d\n", __func__, __LINE__, artosyn_read(gpio, GPIO_INTR_RAWSTATUS), i);
	artosyn_write(gpio, GPIO_INTR_TYPE, level);
	//printk(KERN_INFO "%s %d irq status :0x%x %d\n", __func__, __LINE__, artosyn_read(gpio, GPIO_INTR_RAWSTATUS), i);
	if(type != IRQ_TYPE_EDGE_BOTH) {
		artosyn_write(gpio, GPIO_INTR_POLARITY, polarity);
		//printk(KERN_INFO "%s %d irq status :0x%x %d\n", __func__, __LINE__, artosyn_read(gpio, GPIO_INTR_RAWSTATUS), i);
	}
	else {
		for(i = 0; i < gpio->nr_ports; ++i)
		{
			if(d->domain == gpio->domain[i])
				break;
		}
		artosyn_toggle_trigger(gpio, &gpio->ports[i], d->hwirq, bit);
		//printk(KERN_INFO "%s %d irq status :0x%x %d\n", __func__, __LINE__, artosyn_read(gpio, GPIO_INTR_RAWSTATUS), i);
	}
	spin_unlock_irqrestore(&gpio->lvl_lock, flags);

	return 0;
}

static void artosyn_gpio_irq_enable(struct irq_data *d)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct artosyn_gpio *gpio = igc->private;
	int hwirq = 0;//d->hwirq;
	//struct gpio_chip *gc = NULL;
	unsigned long flags;
	int i = 0, val = 0;

	hwirq = artosyn_irq_data_to_real_hwirq(gpio, d);

	spin_lock_irqsave(&gpio->irq_lock, flags);

	for(i = 0; i < ARTOSYN_MAX_INTERRUPTS; ++i)
	{
		if(hwirq == gpio->irq_used[i])
			break;

		//printk(KERN_INFO "%s %d %d\n", __func__, __LINE__, gpio->irq_used[i]);
	}

	//printk(KERN_INFO "%s %d %d %d\n", __func__, __LINE__, i, hwirq);

	if(i >= ARTOSYN_MAX_INTERRUPTS)
	{
		spin_unlock_irqrestore(&gpio->irq_lock, flags);
		return;
	}

	//printk(KERN_INFO "%s %d %d\n", __func__, __LINE__, gpio->irq_used[i]);
	//printk(KERN_INFO "%s %d irq status :0x%x %d\n", __func__, __LINE__, artosyn_read(gpio, GPIO_INTR_RAWSTATUS), i);

	if(i >= 4)
	{
		val = artosyn_read(gpio, GPIO_INTR_MUX1);
		val &= (~(0xff << ((i - 4) * 8)));
		artosyn_write(gpio, GPIO_INTR_MUX1, val | ((gpio->irq_used[i] & 0x7f) << ((i - 4) * 8)));
	}
	else
	{
		val = artosyn_read(gpio, GPIO_INTR_MUX0);
		val &= (~(0xff << (i * 8)));
		artosyn_write(gpio, GPIO_INTR_MUX0, val | ((gpio->irq_used[i] & 0x7f) << (i * 8)));
	}

	//printk(KERN_INFO "%s %d irq status :0x%x %d\n", __func__, __LINE__, artosyn_read(gpio, GPIO_INTR_RAWSTATUS), i);
	artosyn_write(gpio, GPIO_INTR_EOI, (1 << i));// clean pending irq
	artosyn_write(gpio, GPIO_INTR_EOI, 0);// clean pending irq
	val = artosyn_read(gpio, GPIO_INTR_ENABLE);
	artosyn_write(gpio, GPIO_INTR_ENABLE, val | (1 << i));
	//printk(KERN_INFO "%s %d irq status :0x%x %d\n", __func__, __LINE__, artosyn_read(gpio, GPIO_INTR_RAWSTATUS), i);

	spin_unlock_irqrestore(&gpio->irq_lock, flags);
}

static void artosyn_gpio_irq_disable(struct irq_data *d)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct artosyn_gpio *gpio = igc->private;
	int hwirq = 0;//d->hwirq;
	unsigned long flags;
	int i = 0, val = 0;

	hwirq = artosyn_irq_data_to_real_hwirq(gpio, d);

	spin_lock_irqsave(&gpio->irq_lock, flags);

	for(i = 0; i < ARTOSYN_MAX_INTERRUPTS; ++i)
	{
		if(hwirq == gpio->irq_used[i])
			break;
	}

	if(i >= ARTOSYN_MAX_INTERRUPTS)
	{
		spin_unlock_irqrestore(&gpio->irq_lock, flags);
		return;
	}

	gpio->irq_used[i] = -1;

	val = artosyn_read(gpio, GPIO_INTR_ENABLE);
	artosyn_write(gpio, GPIO_INTR_ENABLE, val & (~(1 << i)));

	spin_unlock_irqrestore(&gpio->irq_lock, flags);
}


static void artosyn_configure_irqs(struct artosyn_gpio *gpio,
				 struct artosyn_platform_data *pdata)
{
	struct irq_chip_generic	*irq_gc = NULL;
	unsigned int hwirq;
	struct irq_chip_type *ct;
	int err, i, j;

	for(i = 0; i < pdata->nr_irqs; ++i)
	{
		gpio->irq_used[i] = -1;
		////printk(KERN_INFO "%s %d %d\n", __func__, __LINE__, gpio->irq_used[i]);
	}

	spin_lock_init(&gpio->irq_lock);
	spin_lock_init(&gpio->lvl_lock);

	gpio->domain = devm_kcalloc(gpio->dev, gpio->nr_ports, sizeof(struct irq_domain *),
				 GFP_KERNEL);
	if (!gpio->domain)
		return;

	for(i = 0; i < gpio->nr_ports; ++i)
	{
		struct artosyn_port_property *pp = &pdata->properties[i];
		struct artosyn_gpio_port *port = &gpio->ports[i];
		struct fwnode_handle  *fwnode = pp->fwnode;
		struct gpio_chip *gc = &port->gc;
		unsigned int ngpio = gc->ngpio;
		gpio->domain[i] = irq_domain_create_linear(fwnode, ngpio,
							 &irq_generic_chip_ops, gpio);
		if (!gpio->domain[i])
			goto err;

		err = irq_alloc_domain_generic_chips(gpio->domain[i], ngpio, 2,
							 "gpio-artosyn", handle_level_irq,
							 IRQ_NOREQUEST, 0,
							 IRQ_GC_INIT_NESTED_LOCK);
		if (err) {
			dev_info(gpio->dev, "irq_alloc_domain_generic_chips failed\n");
			irq_domain_remove(gpio->domain[i]);
			gpio->domain[i] = NULL;
			goto err;
		}

		irq_gc = irq_get_domain_generic_chip(gpio->domain[i], 0);
		if (!irq_gc) {
			irq_domain_remove(gpio->domain[i]);
			gpio->domain[i] = NULL;
			goto err;
		}

		irq_gc->reg_base = gpio->regs;
		irq_gc->private = gpio;
		port->gc.to_irq = artosyn_gpio_to_irq;

		for (j = 0; j < 2; j++) {
			ct = &irq_gc->chip_types[j];
			ct->chip.irq_ack = artosyn_gpio_irq_ack;
			ct->chip.irq_mask = artosyn_gpio_irq_mask;
			ct->chip.irq_unmask = artosyn_gpio_irq_unmask;
			ct->chip.irq_set_type = artosyn_gpio_irq_set_type;
			ct->chip.irq_enable = artosyn_gpio_irq_enable;
			ct->chip.irq_disable = artosyn_gpio_irq_disable;
			//ct->regs.ack = GPIO_PORTA_EOI;
			//ct->regs.mask = GPIO_INTMASK;
			ct->type = IRQ_TYPE_LEVEL_MASK;
		}

		irq_gc->chip_types[0].type = IRQ_TYPE_LEVEL_MASK;
		irq_gc->chip_types[1].type = IRQ_TYPE_EDGE_BOTH;
		irq_gc->chip_types[1].handler = handle_edge_irq;

		for (hwirq = 0; hwirq < ngpio; hwirq++) {
			irq_create_mapping(gpio->domain[i], hwirq);
		}
	}

	for(i = 0; i < pdata->nr_irqs; ++i) {
		/*
		 * Request a shared IRQ since where MFD would have devices
		 * using the same irq pin
		 */
		err = devm_request_irq(gpio->dev, pdata->irq[i],
					   artosyn_irq_handler,
					   IRQF_SHARED, "gpio-artosyn", gpio);
		if (err) {
			dev_err(gpio->dev, "error requesting IRQ\n");
			goto err;
		}
	}

	/* Unmask interrupts */
	artosyn_write(gpio, GPIO_INTR_MASK, 0x0);
	return;

err:
	for(i = 0; i < gpio->nr_ports; ++i) {
		if(NULL != gpio->domain[i])
			continue;

		for (hwirq = 0 ; hwirq < gpio->ports[i].gc.ngpio ; hwirq++)
			irq_dispose_mapping(irq_find_mapping(gpio->domain[i], hwirq));

		irq_domain_remove(gpio->domain[i]);
		gpio->domain[i] = NULL;
	}

	devm_kfree(gpio->dev, gpio->domain);
}

static void artosyn_irq_teardown(struct artosyn_gpio *gpio)
{
	//unsigned int n_totalgpio = gpio->n_totalgpio;
	irq_hw_number_t hwirq;
	int i = 0;

	if (!gpio->domain)
		return;

	for(i = 0; i < gpio->nr_ports; ++i) {
		if(NULL != gpio->domain[i])
			continue;

		for (hwirq = 0 ; hwirq < gpio->ports[i].gc.ngpio ; hwirq++)
			irq_dispose_mapping(irq_find_mapping(gpio->domain[i], hwirq));

		irq_domain_remove(gpio->domain[i]);
		gpio->domain[i] = NULL;
	}

	/* Mask out interrupts */
	artosyn_write(gpio, GPIO_INTR_MASK, 0xffffffff);

	devm_kfree(gpio->dev, gpio->domain);
}

static int artosyn_gpio_probe(struct platform_device *pdev)
{
	unsigned int i;
	struct resource *res;
	struct artosyn_gpio *gpio;
	int err;
	struct device *dev = &pdev->dev;
	struct artosyn_platform_data *pdata = dev_get_platdata(dev);

	if (!pdata) {
		pdata = artosyn_gpio_get_pdata(dev);
		if (IS_ERR(pdata))
			return PTR_ERR(pdata);
	}

	if (!pdata->nports)
		return -ENODEV;

	gpio = devm_kzalloc(&pdev->dev, sizeof(*gpio), GFP_KERNEL);
	if (!gpio)
		return -ENOMEM;

	gpio->dev = &pdev->dev;
	gpio->nr_ports = pdata->nports;
	gpio->pdata = pdata;

	gpio->ports = devm_kcalloc(&pdev->dev, gpio->nr_ports,
				   sizeof(*gpio->ports), GFP_KERNEL);
	if (!gpio->ports)
		return -ENOMEM;

#ifdef CONFIG_PM_SLEEP
	gpio->irq_ctx = devm_kzalloc(gpio->dev, sizeof(*gpio->irq_ctx), GFP_KERNEL);
	if (!gpio->irq_ctx)
		return -ENOMEM;
#endif

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	gpio->regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(gpio->regs))
		return PTR_ERR(gpio->regs);

	for (i = 0; i < gpio->nr_ports; i++) {
		err = artosyn_gpio_add_port(gpio, &pdata->properties[i], i);
		if (err)
			goto out_unregister;
	}

	artosyn_configure_irqs(gpio, pdata);

	platform_set_drvdata(pdev, gpio);

	return 0;

out_unregister:
	artosyn_gpio_unregister(gpio);
	artosyn_irq_teardown(gpio);

	return err;
}

static int artosyn_gpio_remove(struct platform_device *pdev)
{
	struct artosyn_gpio *gpio = platform_get_drvdata(pdev);

	artosyn_gpio_unregister(gpio);
	artosyn_irq_teardown(gpio);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int artosyn_gpio_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct artosyn_gpio *gpio = platform_get_drvdata(pdev);
	struct gpio_chip *gc	= &gpio->ports[0].gc;
	unsigned long flags;
	int i;
	struct artosyn_irq_context *irq_ctx = NULL;

	spin_lock_irqsave(&gc->bgpio_lock, flags);
	for (i = 0; i < gpio->nr_ports; i++) {
		unsigned int offset;
		unsigned int idx = gpio->ports[i].idx;
		struct artosyn_context *ctx = gpio->ports[i].ctx;

		BUG_ON(!ctx);

		offset = GPIO_DIR_PORTA + idx * GPIO_DIR_PORT_SIZE;
		ctx->dir = artosyn_read(gpio, offset);

		offset = GPIO_SET_PORTA + idx * GPIO_SET_PORT_SIZE;
		ctx->data = artosyn_read(gpio, offset);
	}

	irq_ctx = gpio->irq_ctx;

	BUG_ON(!irq_ctx);

	irq_ctx->int_mux[0] = artosyn_read(gpio, GPIO_INTR_MUX0);
	irq_ctx->int_mux[1] = artosyn_read(gpio, GPIO_INTR_MUX1);
	irq_ctx->int_mask	= artosyn_read(gpio, GPIO_INTR_MASK);
	irq_ctx->int_en	= artosyn_read(gpio, GPIO_INTR_ENABLE);
	irq_ctx->int_pol	= artosyn_read(gpio, GPIO_INTR_POLARITY);
	irq_ctx->int_type	= artosyn_read(gpio, GPIO_INTR_TYPE);
	irq_ctx->int_deb	= artosyn_read(gpio, GPIO_DEBOUNCE);

	artosyn_write(gpio, GPIO_INTR_MASK, 0xffffffff);

	spin_unlock_irqrestore(&gc->bgpio_lock, flags);

	return 0;
}

static int artosyn_gpio_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct artosyn_gpio *gpio = platform_get_drvdata(pdev);
	struct gpio_chip *gc	= &gpio->ports[0].gc;
	unsigned long flags;
	int i;
	struct artosyn_irq_context *irq_ctx = NULL;

	spin_lock_irqsave(&gc->bgpio_lock, flags);
	for (i = 0; i < gpio->nr_ports; i++) {
		unsigned int offset;
		unsigned int idx = gpio->ports[i].idx;
		struct artosyn_context *ctx = gpio->ports[i].ctx;

		BUG_ON(!ctx);

		offset = GPIO_SET_PORTA + idx * GPIO_SET_PORT_SIZE;
		artosyn_write(gpio, offset, ctx->data);

		offset = GPIO_DIR_PORTA + idx * GPIO_DIR_PORT_SIZE;
		artosyn_write(gpio, offset, ctx->dir);
	}

	irq_ctx = gpio->irq_ctx;

	BUG_ON(!irq_ctx);

	artosyn_write(gpio, GPIO_INTR_MUX0, irq_ctx->int_mux[0]);
	artosyn_write(gpio, GPIO_INTR_MUX1, irq_ctx->int_mux[1]);
	artosyn_write(gpio, GPIO_INTR_MASK, irq_ctx->int_mask);
	artosyn_write(gpio, GPIO_INTR_ENABLE, irq_ctx->int_en);
	artosyn_write(gpio, GPIO_INTR_POLARITY, irq_ctx->int_pol);
	artosyn_write(gpio, GPIO_INTR_TYPE, irq_ctx->int_type);
	artosyn_write(gpio, GPIO_DEBOUNCE, irq_ctx->int_deb);

	artosyn_write(gpio, GPIO_INTR_EOI, 0xffffffff);
	artosyn_write(gpio, GPIO_INTR_EOI, 0);

	spin_unlock_irqrestore(&gc->bgpio_lock, flags);

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(artosyn_gpio_pm_ops, artosyn_gpio_suspend,
			 artosyn_gpio_resume);

static const struct of_device_id artosyn_of_match[] = {
	{ .compatible = "artosyn,gpio" },
	{ /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, artosyn_of_match);

static struct platform_driver artosyn_gpio_driver = {
	.driver		= {
		.name	= "gpio-artosyn",
		.pm	= &artosyn_gpio_pm_ops,
		.of_match_table = of_match_ptr(artosyn_of_match),
	},
	.probe		= artosyn_gpio_probe,
	.remove		= artosyn_gpio_remove,
};

module_platform_driver(artosyn_gpio_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kwang");
MODULE_DESCRIPTION("Artosyn GPIO driver");


