//#define DEBUG

#include <config.h>
#include <common.h>
#include <errno.h>
#include <malloc.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/usb/composite.h>
#include <linux/compiler.h>
#include <version.h>
#include <g_dnl.h>
#include <env.h>
#include <linux/delay.h>
#include <watchdog.h>
#include <asm/arch/f_artousb.h>
#include <command.h>
#include <uboot_upgrade.h>

DECLARE_GLOBAL_DATA_PTR;

#define ARTOUSB_INTERFACE_CLASS         USB_CLASS_HID
#define ARTOUSB_INTERFACE_SUB_CLASS     0x00
#define ARTOUSB_INTERFACE_PROTOCOL      0x00

//#define EP_BUFFER_SIZE                  1024
/*
 * EP_BUFFER_SIZE must always be an integral multiple of maxpacket size
 * (64 or 512 or 1024), else we break on certain controllers like DWC3
 * that expect bulk OUT requests to be divisible by maxpacket size.
 */

//#define ARUSB_BUF_SIZE                  EP_BUFFER_SIZE * 2
#define ARBLOCK_BUF_SIZE                4096
#define AR_BUFFER_NUM                   2

/*
 * HID class requests
 */
#define HID_REQ_GET_REPORT              0x01
#define HID_REQ_GET_IDLE                0x02
#define HID_REQ_GET_PROTOCOL            0x03
#define HID_REQ_SET_REPORT              0x09
#define HID_REQ_SET_IDLE                0x0A
#define HID_REQ_SET_PROTOCOL            0x0B

/*
 * HID protocol status
 */
#define HID_REPORT_PROTOCOL             0x1
#define HID_BOOT_PROTOCOL               0x0

/*
 * HID class descriptor types
 */
#define HID_DT_HID                      (USB_TYPE_CLASS | 0x01)
#define HID_DT_REPORT                   (USB_TYPE_CLASS | 0x02)
#define HID_DT_PHYSICAL                 (USB_TYPE_CLASS | 0x03)


#define HID_USAGE_PAGE_LEN              76

/*
 * Artosyn HID upgrade
 */
#define     INDEX_SYS_UPGRADE           0
#define     INDEX_SYS_FILETRAN          1
#define     INDEX_SYS_FILEGET           2
#define     INDEX_SYS_USBCMD            3
#define     INDEX_UBOOT_FILETRAN        4
#define     INDEX_UBOOT_FILEGET         5
#define     INDEX_UBOOT_USBCMD          6

#define     RECV_CALLBACK_NUM           7
#define     HID_PROTOCOL                1

#define     upgrade_start_mark          "upgd"
#define     upgrade_prpa_mark           "prpaupgd"
#define     ufiletran_start_mark        "uftr"
#define     ufileget_start_mark         "ufgt"

#define     MAX_DDR_ADDRESS_LEN         (sizeof(unsigned char *))
#define     HID_TX_TIMEOUT              (1000)

#pragma pack(push)
#pragma pack(1)

typedef struct
{
    unsigned char   magic_num[8];
    unsigned int    header_len;
    unsigned int    total_len;
    unsigned int    payload_len;
    unsigned int    fragment_num;
    unsigned int    fragment_idx;
    unsigned char   sub_proto;
    unsigned char   reserved[3];
    unsigned int    checksum;
}STRU_USB_HEADER;

typedef struct
{
    unsigned short  port_index;
    unsigned short  header_len;
    unsigned int    data_len;
    unsigned char   status;
    unsigned char   reserved[3];
}STRU_HID_HEADER;

typedef struct
{
    unsigned int    cur_len;
    unsigned int    fragment_idx;
    unsigned short  port_index;
    unsigned short  header_len;
    unsigned int    data_len;
}STRU_USB_FRAGMENT_INFO;

typedef struct
{
    unsigned int    total_len;
    unsigned char * pimage;
    int             offset;
}image_info;

typedef struct
{

    unsigned int    total_len;
    unsigned char * pfile;
    int             offset;
}file_info;

#pragma pack(pop)

typedef int (*HID_INIT_CALLBACK_HANDLER)(void);
typedef int (*HID_RECV_CALLBACK_HANDLER)(void *, unsigned int);
typedef int (*ar_flash_cb)(int state, int percent);

typedef struct
{
    unsigned short port_index;
    HID_INIT_CALLBACK_HANDLER    init_callback;//optional
    HID_RECV_CALLBACK_HANDLER    recv_callback;
}STRU_HID_RECV_CALLBACK_HANDLER;

struct f_artousb {
    struct usb_function     usb_function;
    struct usb_ep           *in_ep, *out_ep;
    struct usb_request      *in_req, *out_req;
    char                    *dev_type;
    unsigned int            dev_index;
    unsigned int            tag;
    unsigned int            lba;
    unsigned int            dl_size;
    unsigned int            dl_bytes;
    unsigned int            ul_size;
    unsigned int            ul_bytes;
    int                     reboot_flag;
    void                    *buf;
    void                    *buf_head;
    unsigned int            buf_size;
    unsigned char           protocol;
    unsigned short          report_length;
    image_info              image;
    file_info               fileinfo;
    file_info               fgetinfo;
};

struct hid_report {
    u8 usage_page[HID_USAGE_PAGE_LEN];
} __packed;

static struct usb_interface_descriptor interface_desc = {
    .bLength                = USB_DT_INTERFACE_SIZE,
    .bDescriptorType        = USB_DT_INTERFACE,
    .bInterfaceNumber       = 0x00,
    .bAlternateSetting      = 0x00,
    .bNumEndpoints          = 0x02,
    .bInterfaceClass        = ARTOUSB_INTERFACE_CLASS,
    .bInterfaceSubClass     = ARTOUSB_INTERFACE_SUB_CLASS,
    .bInterfaceProtocol     = ARTOUSB_INTERFACE_PROTOCOL,
};

static struct usb_endpoint_descriptor fs_ep_in = {
    .bLength                = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType        = USB_DT_ENDPOINT,
    .bEndpointAddress       = USB_DIR_IN,
    .bmAttributes           = USB_ENDPOINT_XFER_INT,
    .wMaxPacketSize         = cpu_to_le16(64),
    .bInterval              = 1,
};

static struct usb_endpoint_descriptor fs_ep_out = {
    .bLength                = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType        = USB_DT_ENDPOINT,
    .bEndpointAddress       = USB_DIR_OUT,
    .bmAttributes           = USB_ENDPOINT_XFER_INT,
    .wMaxPacketSize         = cpu_to_le16(64),
    .bInterval              = 1,
};

static struct usb_endpoint_descriptor hs_ep_in = {
    .bLength                = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType        = USB_DT_ENDPOINT,
    .bEndpointAddress       = USB_DIR_IN,
    .bmAttributes           = USB_ENDPOINT_XFER_INT,
    .wMaxPacketSize         = cpu_to_le16(1024),
    .bInterval              = 1,
};

static struct usb_endpoint_descriptor hs_ep_out = {
    .bLength                = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType        = USB_DT_ENDPOINT,
    .bEndpointAddress       = USB_DIR_OUT,
    .bmAttributes           = USB_ENDPOINT_XFER_INT,
    .wMaxPacketSize         = cpu_to_le16(1024),
    .bInterval              = 1,
};

/*static struct usb_endpoint_descriptor ss_ep_in = {
    .bLength                = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType        = USB_DT_ENDPOINT,
    .bEndpointAddress       = USB_DIR_IN,
    .bmAttributes           = USB_ENDPOINT_XFER_INT,
    .wMaxPacketSize         = cpu_to_le16(1024),
    .bInterval              = 1,
};

static struct usb_ss_ep_comp_descriptor ss_in_comp_desc = {
    .bLength                = sizeof(ss_in_comp_desc),
    .bDescriptorType        = USB_DT_SS_ENDPOINT_COMP,
};

static struct usb_endpoint_descriptor ss_ep_out = {
    .bLength                = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType        = USB_DT_ENDPOINT,
    .bEndpointAddress       = USB_DIR_OUT,
    .bmAttributes           = USB_ENDPOINT_XFER_INT,
    .wMaxPacketSize         = cpu_to_le16(1024),
    .bInterval              = 1,
};

static struct usb_ss_ep_comp_descriptor ss_out_comp_desc = {
    .bLength                = sizeof(ss_out_comp_desc),
    .bDescriptorType        = USB_DT_SS_ENDPOINT_COMP,
};*/

static struct usb_class_hid_descriptor hidg_desc = {
    .bLength                = sizeof(hidg_desc),
    .bDescriptorType        = USB_DT_CS_DEVICE,
    .bcdCDC                 = __constant_cpu_to_le16(0x0110),
    .bCountryCode           = 0x00,
    .bNumDescriptors        = 0x1,
    .bDescriptorType0       = USB_DT_HID_REPORT,
    .wDescriptorLength0     = HID_USAGE_PAGE_LEN,
};

/* This is synchronized with what the SoC implementation reports */
static struct hid_report arto_hid_report = {
    .usage_page = {
        0x06, 0x00, 0xff, /* Usage Page */
        0x09, 0x01, /* Usage (Pointer?) */
        0xa1, 0x01, /* Collection */

        0x85, 0x01, /* Report ID */
        0x19, 0x01, /* Usage Minimum */
        0x29, 0x01, /* Usage Maximum */
        0x15, 0x00, /* Local Minimum */
        0x26, 0xFF, 0x00, /* Local Maximum? */
        0x75, 0x08, /* Report Size */
        0x95, 0x10, /* Report Count */
        0x91, 0x02, /* Output Data */

        0x85, 0x02, /* Report ID */
        0x19, 0x01, /* Usage Minimum */
        0x29, 0x01, /* Usage Maximum */
        0x15, 0x00, /* Local Minimum */
        0x26, 0xFF, 0x00, /* Local Maximum? */
        0x75, 0x80, /* Report Size 128 */
        0x95, 0x40, /* Report Count */
        0x91, 0x02, /* Output Data */

        0x85, 0x03, /* Report ID */
        0x19, 0x01, /* Usage Minimum */
        0x29, 0x01, /* Usage Maximum */
        0x15, 0x00, /* Local Minimum */
        0x26, 0xFF, 0x00, /* Local Maximum? */
        0x75, 0x08, /* Report Size 8 */
        0x95, 0x04, /* Report Count */
        0x81, 0x02, /* Input Data */

        0x85, 0x04, /* Report ID */
        0x19, 0x01, /* Usage Minimum */
        0x29, 0x01, /* Usage Maximum */
        0x15, 0x00, /* Local Minimum */
        0x26, 0xFF, 0x00, /* Local Maximum? */
        0x75, 0x08, /* Report Size 8 */
        0x95, 0x40, /* Report Count */
        0x81, 0x02, /* Input Data */
        0xc0
    },
};


static struct usb_descriptor_header *arusb_fs_function[] = {
    (struct usb_descriptor_header *)&interface_desc,
    (struct usb_descriptor_header *)&hidg_desc,
    (struct usb_descriptor_header *)&fs_ep_in,
    (struct usb_descriptor_header *)&fs_ep_out,
    NULL,
};

static struct usb_descriptor_header *arusb_hs_function[] = {
    (struct usb_descriptor_header *)&interface_desc,
    (struct usb_descriptor_header *)&hidg_desc,
    (struct usb_descriptor_header *)&hs_ep_in,
    (struct usb_descriptor_header *)&hs_ep_out,
    NULL,
};

/*static struct usb_descriptor_header *arusb_ss_function[] = {
    (struct usb_descriptor_header *)&interface_desc,
    (struct usb_descriptor_header *)&hidg_desc,
    (struct usb_descriptor_header *)&ss_ep_in,
    (struct usb_descriptor_header *)&ss_in_comp_desc,
    (struct usb_descriptor_header *)&ss_ep_out,
    (struct usb_descriptor_header *)&ss_out_comp_desc,
    NULL,
};*/

static const char arusb_name[] = "Artosyn Artousb Uboot HID";

static struct usb_string arusb_string_defs[] = {
    [0].s = arusb_name,
    {  }            /* end of list */
};

static struct usb_gadget_strings stringtab_arusb = {
    .language    = 0x0409,    /* en-us */
    .strings    = arusb_string_defs,
};

static struct usb_gadget_strings *arusb_strings[] = {
    &stringtab_arusb,
    NULL,
};

/*static struct dwc3_device dwc3_device_data[] =
{
    {
        .maximum_speed = USB_SPEED_SUPER,
        .base = 0x60500000,
        .dr_mode = USB_DR_MODE_PERIPHERAL,
        .index = 0,
    },
    {}
};*/

int artousb_sys_upgrade_recv(void * data, unsigned int len);
int artousb_uboot_filetran_recv(void * data, unsigned int len);
int artousb_uboot_fileget_recv(void * data, unsigned int len);

static void artousb_disable(struct usb_function *f);
static void artousb_rx_handler(struct usb_ep *ep, struct usb_request *req);
extern int do_upgrade(unsigned char *img, ar_flash_cb cb);
extern int set_single_upgrade(bool flag);
extern void artosyn_set_force_upgrade(bool force);

static struct f_artousb         *artousb_func;
static unsigned char            ar_image_dl_completed = 0;
static unsigned char            ar_usb_hid_tx_completed = 0;
static STRU_USB_FRAGMENT_INFO   fragment_info;

unsigned char magic_header[8] = {0xff, 0x55, 0xff, 0xaa, 0x41, 0x52, 0x54, 0x4f};
STRU_HID_RECV_CALLBACK_HANDLER  g_hid_recv_callback[RECV_CALLBACK_NUM] =
{
    {INDEX_SYS_UPGRADE, NULL, artousb_sys_upgrade_recv},
    {},
    {},
    {},
    {INDEX_UBOOT_FILETRAN, NULL, artousb_uboot_filetran_recv},
    {INDEX_UBOOT_FILEGET, NULL, artousb_uboot_fileget_recv},
    {INDEX_UBOOT_USBCMD, NULL, NULL},
};

static inline struct f_artousb *func_to_artousb(struct usb_function *f)
{
    return container_of(f, struct f_artousb, usb_function);
}

static struct usb_endpoint_descriptor *arusb_ep_desc(
                                    struct usb_gadget *g,
                                    struct usb_endpoint_descriptor *fs,
                                    struct usb_endpoint_descriptor *hs)
{
    if (gadget_is_dualspeed(g) && g->speed == USB_SPEED_HIGH)
        return hs;
    return fs;
}

static struct usb_request *artousb_start_ep(struct usb_ep *ep, enum usb_device_speed speed)
{
    struct usb_request *req;

    req = usb_ep_alloc_request(ep, 0);
    if (!req)
        return NULL;

    req->length = (USB_SPEED_FULL == speed) ?
        fs_ep_out.wMaxPacketSize : hs_ep_out.wMaxPacketSize;;//EP_BUFFER_SIZE;
    req->buf = memalign(CONFIG_SYS_CACHELINE_SIZE, req->length);//EP_BUFFER_SIZE);
    if (!req->buf) {
        usb_ep_free_request(ep, req);
        return NULL;
    }
    memset(req->buf, 0, req->length);

    return req;
}

struct f_artousb *get_arusb(void)
{
    struct f_artousb *f_arusb = artousb_func;
    phys_size_t size = 0;
    phys_size_t start = 0xFFFFFFFF;

    //debug("%s\n", __func__);

    if (!f_arusb) {
        f_arusb = memalign(CONFIG_SYS_CACHELINE_SIZE, sizeof(*f_arusb));
        if (!f_arusb) {
            printf("f_arusb is empty!\n");
            return NULL;
        }
        artousb_func = f_arusb;
        memset(f_arusb, 0 , sizeof(struct f_artousb));
    }

    if (!f_arusb->buf_head) {
        f_arusb->buf_head = (void  *)IMAGE_LOAD_ADDR;
        f_arusb->buf = f_arusb->buf_head;

//#ifdef CONFIG_SYS_SDRAM_BASE
//        start = CONFIG_SYS_SDRAM_BASE;
//#else
        start = DRAM_START_ADDR;
//#endif

#ifndef CONFIG_SPL_BUILD
        if(gd->ram_size)
        {
            size = gd->ram_size;
            //printf("gd->ram_size: 0x%x\n", gd->ram_size);
        }
        else
        {
            size = 0x40000000;
        }
#else
        size = 0x40000000;//(phys_size_t)(*((unsigned int *)DRAM_DENSITY_STORE_ADDRESS));
#endif

        f_arusb->buf_size = (size - (IMAGE_LOAD_ADDR - start));

        printf("%s size: 0x%x 0x%llx@0x%llx\n", __func__, f_arusb->buf_size, size, start);
    }

    return f_arusb;
}

static int artousb_setup(struct usb_function *f, const struct usb_ctrlrequest *ctrl)
{

    struct usb_gadget   *gadget     = f->config->cdev->gadget;
    struct usb_request  *req        = f->config->cdev->req;
    struct f_artousb    *f_arusb    = get_arusb();
    int                 status      = 0;
    __u16               value, length;

    value   = __le16_to_cpu(ctrl->wValue);
    length  = __le16_to_cpu(ctrl->wLength);

    debug("%s crtl_request : bRequestType:0x%x bRequest:0x%x Value:0x%x\n",
         __func__, ctrl->bRequestType, ctrl->bRequest, value);

    switch ((ctrl->bRequestType << 8) | ctrl->bRequest) {
    case ((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8
          | HID_REQ_GET_REPORT):
        debug("get_report\n");
        /* send an empty report */
        length = min_t(unsigned, length, f_arusb->report_length);
        memset(req->buf, 0x0, length);
        goto respond;
        break;

    case ((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8
          | HID_REQ_GET_PROTOCOL):
        debug("get_protocol\n");
        length = min_t(unsigned int, length, 1);
        ((u8 *) req->buf)[0] = f_arusb->protocol;
        goto respond;
        break;

    case ((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8
          | HID_REQ_SET_REPORT):
        debug("set_report | wLength=%d\n", ctrl->wLength);
        goto stall;
        break;

    case ((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8
          | HID_REQ_SET_PROTOCOL):
        debug("set_protocol\n");
        goto stall;
        break;

    case ((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8
          | HID_REQ_SET_IDLE):
        debug("set_idle\n");

        /* send an empty report */
        length = min_t(unsigned, length, f_arusb->report_length);
        memset(req->buf, 0x0, length);

        goto respond;
        break;

    case ((USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE) << 8
          | USB_REQ_GET_DESCRIPTOR):
    case ((USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_INTERFACE) << 8
          | USB_REQ_GET_DESCRIPTOR):
        switch (value >> 8) {
        case HID_DT_HID:
        {
            struct usb_class_hid_descriptor hidg_desc_copy = hidg_desc;
            debug("USB_REQ_GET_DESCRIPTOR: HID\n");
            length = min_t(unsigned short, length,
                           hidg_desc_copy.bLength);
            memcpy(req->buf, &hidg_desc_copy, length);
            goto respond;
            break;
        }

        case HID_DT_REPORT:
            debug("USB_REQ_GET_DESCRIPTOR: REPORT\n");
            length = min_t(unsigned short, length, (u16) sizeof(arto_hid_report));
            memcpy(req->buf, &arto_hid_report, length);
            goto respond;
            break;

        default:
            debug("Unknown descriptor request 0x%x\n",
                 value >> 8);
            goto stall;
            break;
        }
        break;

    default:
        debug("Unknown request 0x%x\n",
             ctrl->bRequest);
        goto stall;
        break;
    }

stall:
    return -EOPNOTSUPP;

respond:
    req->zero = 0;
    req->length = length;
    status = usb_ep_queue(gadget->ep0, req, GFP_ATOMIC);
    if (status < 0)
        printf("usb_ep_queue error on ep0 %d\n", status);
    return status;
}


/* config the artousb device*/
static int artousb_bind(struct usb_configuration *c, struct usb_function *f)
{
    int id;
    struct usb_gadget *gadget = c->cdev->gadget;
    struct f_artousb *f_arusb = func_to_artousb(f);
    //const char *s;

    id = usb_interface_id(c, f);
    if (id < 0)
        return id;
    interface_desc.bInterfaceNumber = id;

    id = usb_string_id(c->cdev);
    if (id < 0)
        return id;

    arusb_string_defs[0].id = id;
    interface_desc.iInterface = id;

    f_arusb->in_ep = usb_ep_autoconfig(gadget, &fs_ep_in);
    if (!f_arusb->in_ep)
        return -ENODEV;
    f_arusb->in_ep->driver_data = c->cdev;

    f_arusb->out_ep = usb_ep_autoconfig(gadget, &fs_ep_out);
    if (!f_arusb->out_ep)
        return -ENODEV;
    f_arusb->out_ep->driver_data = c->cdev;

    f_arusb->report_length = fs_ep_out.wMaxPacketSize;//EP_BUFFER_SIZE;
    f_arusb->protocol = HID_REPORT_PROTOCOL;

    f->descriptors = arusb_fs_function;

    if (gadget_is_dualspeed(gadget)) {
        hs_ep_in.bEndpointAddress = fs_ep_in.bEndpointAddress;
        hs_ep_out.bEndpointAddress = fs_ep_out.bEndpointAddress;
        f->hs_descriptors = arusb_hs_function;
        f_arusb->report_length = hs_ep_out.wMaxPacketSize;
    }

    memset(&fragment_info, 0, sizeof(STRU_USB_FRAGMENT_INFO));

    return 0;
}

static void artousb_unbind(struct usb_configuration *c, struct usb_function *f)
{
    /* clear the configuration*/
    if(NULL != artousb_func)
        memset(artousb_func, 0, sizeof(*artousb_func));
}

static void artousb_complete(struct usb_ep *ep, struct usb_request *req)
{
    int status = req->status;

    if (!status)
    {
        ar_usb_hid_tx_completed = 1;
        return;
    }
    debug("status: %d ep '%s' trans: %d\n", status, ep->name, req->actual);
}

static int artousb_set_alt(struct usb_function *f, unsigned int interface,
               unsigned int alt)
{
    int ret;
    struct usb_composite_dev *cdev = f->config->cdev;
    struct usb_gadget *gadget = cdev->gadget;
    struct f_artousb *f_arusb = func_to_artousb(f);
    const struct usb_endpoint_descriptor *d;

    debug("%s: func: %s intf: %d alt: %d\n",
          __func__, f->name, interface, alt);

    d = arusb_ep_desc(gadget, &fs_ep_out, &hs_ep_out);
    ret = usb_ep_enable(f_arusb->out_ep, d);
    if (ret) {
        printf("failed to enable out ep\n");
        return ret;
    }

    f_arusb->out_req = artousb_start_ep(f_arusb->out_ep, gadget->speed);
    if (!f_arusb->out_req) {
        printf("failed to alloc out req\n");
        ret = -EINVAL;
        goto err;
    }
    f_arusb->out_req->complete = artousb_rx_handler;

    d = arusb_ep_desc(gadget, &fs_ep_in, &hs_ep_in);
    ret = usb_ep_enable(f_arusb->in_ep, d);
    if (ret) {
        printf("failed to enable in ep\n");
        goto err;
    }

    f_arusb->in_req = artousb_start_ep(f_arusb->in_ep, gadget->speed);
    if (!f_arusb->in_req) {
        printf("failed alloc req in\n");
        ret = -EINVAL;
        goto err;
    }
    f_arusb->in_req->complete = artousb_complete;

    f_arusb->report_length = (USB_SPEED_FULL == gadget->speed) ?
        fs_ep_out.wMaxPacketSize : hs_ep_out.wMaxPacketSize;

    ret = usb_ep_queue(f_arusb->out_ep, f_arusb->out_req, 0);
    if (ret)
        goto err;

    return 0;
err:
    artousb_disable(f);
    return ret;
}

static void artousb_disable(struct usb_function *f)
{
    struct f_artousb *f_arusb = func_to_artousb(f);

    usb_ep_disable(f_arusb->out_ep);
    usb_ep_disable(f_arusb->in_ep);

    if (f_arusb->out_req) {
        usb_ep_free_request(f_arusb->out_ep, f_arusb->out_req);
        f_arusb->out_req = NULL;
    }
    if (f_arusb->in_req) {
        usb_ep_free_request(f_arusb->in_ep, f_arusb->in_req);
        f_arusb->in_req = NULL;
    }
    if (f_arusb->buf_head) {
        f_arusb->buf_head = NULL;
        f_arusb->buf = NULL;
    }
}

static int artousb_add(struct usb_configuration *c)
{
    struct f_artousb *f_arusb = get_arusb();
    int status;

    debug("%s: cdev: 0x%p\n", __func__, c->cdev);

    f_arusb->usb_function.name = "f_artousb";
    f_arusb->usb_function.bind = artousb_bind;
    f_arusb->usb_function.unbind = artousb_unbind;
    f_arusb->usb_function.set_alt = artousb_set_alt;
    f_arusb->usb_function.disable = artousb_disable;
    f_arusb->usb_function.strings = arusb_strings;
    f_arusb->usb_function.setup = artousb_setup;

    status = usb_add_function(c, &f_arusb->usb_function);
    if (status) {
        free(f_arusb);
        artousb_func = NULL;
    }
    return status;
}
DECLARE_GADGET_BIND_CALLBACK(usb_dnl_artousb, artousb_add);

/*static int board_usb_init(int index)
{
    sirius_usb3_phy_init();
    return dwc3_uboot_init(&dwc3_device_data[index]);
}*/

static void artousb_dev_init(char *dev_type, int dev_index)
{
    struct f_artousb *f_arusb = get_arusb();

    if(f_arusb)
    {
        f_arusb->dev_type = dev_type;
        f_arusb->dev_index = dev_index;
    }
}

int artosyn_hid_upgrade(int index)
{
    int ret;

    printf("usb_booting enter!!!\n");

    set_single_upgrade(true);

    ar_image_dl_completed = 0;

    ret = usb_gadget_initialize(index);
    if (ret) {
        printf("USB init failed: %d\n", ret);
        return CMD_RET_FAILURE;
    }
    //board_usb_init(index);

    artousb_dev_init(NULL, 0);
    g_dnl_clear_detach();

    ret = g_dnl_register("usb_dnl_artousb");
    if(0 == ret)
    {
        while(1)//(!ar_image_dl_completed)
        {
            if (ctrlc())
                break;

            WATCHDOG_RESET();

            usb_gadget_handle_interrupts(0);
        }
    }
    else
    {
        printf("do_artosyn_hid_upgrade init failed!\n");
    }

    g_dnl_unregister();
    g_dnl_clear_detach();
    usb_gadget_release(index);
    //dwc3_uboot_exit(0);
    return 0;
}

static int do_artosyn_hid_upgrade(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])////, unsigned int partition_addr, unsigned int partition_size)
{
    int index = 0;//, i = 0;
    char *usb_controller;

    if (argc < 2)
    {
        printf("argc num: %d\n", argc);
        return CMD_RET_USAGE;
    }


    usb_controller = argv[1];
    index = simple_strtoul(usb_controller, NULL, 0);

    return artosyn_hid_upgrade(index);
}

#ifndef CONFIG_SPL_BUILD
#ifdef CONFIG_SYS_LONGHELP
static char artosyn_upgrade_help_text[] =
	"<USB_controller> e.g. artosyn_hid_upgrade 0\n"
	"";
#endif

U_BOOT_CMD(
	artosyn_hid_upgrade,     2,      1,      do_artosyn_hid_upgrade,
	"do artosyn_hid_upgrade", artosyn_upgrade_help_text
);

#endif

static unsigned int get_checksum(unsigned char * data, unsigned int len)
{
    unsigned int checksum = 0;

    while(len)
    {
        checksum += *data++;
        --len;
    }

    return checksum;
}

static unsigned char * get_hid_send_buffer(unsigned int buf_len, unsigned int port)
{
    unsigned char * buff = NULL;
    STRU_HID_HEADER * phead;

    buff = (unsigned char *)malloc(buf_len + sizeof(STRU_HID_HEADER) + sizeof(STRU_USB_HEADER));
    if(NULL == buff)
    {
        printf("malloc buffer failed!\n");
        return NULL;
    }
    memset(buff, 0, buf_len + sizeof(STRU_HID_HEADER) + sizeof(STRU_USB_HEADER));

    phead = (STRU_HID_HEADER *)(buff + sizeof(STRU_USB_HEADER));
    phead->data_len     = buf_len;
    phead->header_len   = sizeof(STRU_HID_HEADER);
    phead->port_index   = port;
    phead->status       = 0;
    phead->reserved[0]  = 0;
    phead->reserved[1]  = 0;
    phead->reserved[2]  = 0;

    return buff + sizeof(STRU_HID_HEADER) + sizeof(STRU_USB_HEADER);
}


static int hid_write(int fd, unsigned char * data, unsigned int buf_len, unsigned long long timeout)
{
    struct f_artousb    *f_arusb    = get_arusb();
    struct usb_request *in_req = f_arusb->in_req;
    int ret;

    memcpy(in_req->buf, data, buf_len);
    in_req->length = buf_len;
    debug("Transferring 0x%x bytes\n", buf_len);
    usb_ep_dequeue(f_arusb->in_ep, in_req);
    ret = usb_ep_queue(f_arusb->in_ep, in_req, 0);
    if (ret)
        printf("Error %d on queue\n", ret);
    else
    {
        ar_usb_hid_tx_completed = 0;
        do
        {
            usb_gadget_handle_interrupts(0);
            --timeout;
        }while(timeout && 0 == ar_usb_hid_tx_completed);
        if(0 == timeout)
            printf("Hid send timeout!\n");
    }

    return ret;
}


static int hid_send_data(void * data, unsigned int buf_len, unsigned long long timeout)
{
    struct f_artousb    *f_arusb    = get_arusb();
    int ret, index;
    if(NULL == data)
    {
        printf("data is NULL\n");
        return -1;
    }

    STRU_HID_HEADER  * phid = (STRU_HID_HEADER *)((char *)data - sizeof(STRU_HID_HEADER));
    STRU_USB_HEADER  * pusb = (STRU_USB_HEADER *)((char *)phid - sizeof(STRU_USB_HEADER));

    int offset = 0;
    int step = f_arusb->report_length - sizeof(STRU_USB_HEADER);//EP_BUFFER_SIZE - sizeof(STRU_USB_HEADER);
    int fragment_num = ((phid->header_len + phid->data_len) % step) ?
                        ((phid->header_len + phid->data_len) / step) + 1 :
                        ((phid->header_len + phid->data_len) / step);

    if(buf_len == phid->data_len)
    {
        if(1)
        {
            for(offset = 0, index = 0; (offset + step) < (phid->header_len + phid->data_len); offset += step, pusb = (STRU_USB_HEADER  *)((unsigned char *)pusb + step), ++index)
            {
                memcpy(pusb->magic_num, magic_header, 8);
                pusb->header_len = 36;
                pusb->total_len = phid->header_len + phid->data_len;
                pusb->payload_len = step;
                pusb->fragment_idx = index;
                pusb->fragment_num = fragment_num;
                pusb->sub_proto = HID_PROTOCOL;
                pusb->reserved[0] = 0;
                pusb->reserved[1] = 0;
                pusb->reserved[2] = 0;
                pusb->checksum = get_checksum(((unsigned char *)pusb + sizeof(STRU_USB_HEADER)),
                                                                pusb->payload_len);

                ret = hid_write(0, (unsigned char *)pusb, f_arusb->report_length, timeout);//EP_BUFFER_SIZE, timeout);
                if(ret < 0)
                {
                    printf("write hid failed!\n");
                    free(data - sizeof(STRU_HID_HEADER) - sizeof(STRU_USB_HEADER));
                    return -1;
                }
                //printf("i:%d offset:%d total:%d\n", pusb->fragment_idx, offset, pusb->total_len);
            }

            memcpy(pusb->magic_num, magic_header, 8);
            pusb->header_len = 36;
            pusb->total_len = phid->header_len + phid->data_len;
            pusb->payload_len = pusb->total_len - offset;
            pusb->fragment_idx = index;
            pusb->fragment_num = fragment_num;
            pusb->sub_proto = HID_PROTOCOL;
            pusb->reserved[0] = 0;
            pusb->reserved[1] = 0;
            pusb->reserved[2] = 0;
            pusb->checksum = get_checksum(((unsigned char *)pusb + sizeof(STRU_USB_HEADER)),
                                                            pusb->payload_len);

            ret = hid_write(0, (unsigned char *)pusb, pusb->payload_len + pusb->header_len, timeout);
            if(ret < 0)
            {
                printf("write hid failed!\n");
                free(data - sizeof(STRU_HID_HEADER) - sizeof(STRU_USB_HEADER));
                return -1;
            }

            //printf("i:%d offset:%d total:%d\n", pusb->fragment_idx, offset, pusb->total_len);
        }
        else
        {
            ret = -1;
        }

        free(data - sizeof(STRU_HID_HEADER) - sizeof(STRU_USB_HEADER));
    }
    else
    {
        printf("memory buffer len no match(%d %d), can not free memory!\n", buf_len, phid->data_len);
        ret = -1;
    }

    return ret;
}

static int artousb_check_start_mark(char * data, char * mark, unsigned int len)
{
    int i;
    for(i = 0; i < strlen(mark); ++i)
    {
        if(data[i] != mark[i])
            return -1;
    }
    return 0;
}

static int artousb_show_upgrade(int state, int percent)
{
    printf("Upgrade status: %d, percent:%d\n", state, percent);
    unsigned char * buffer = NULL;
    unsigned int buf_len = 64;

    buffer = get_hid_send_buffer(buf_len, INDEX_SYS_UPGRADE);
    if(NULL != buffer)
    {
        sprintf((char *)buffer, "Upgrade status: %d, percent:%d\n", state, percent);
        hid_send_data(buffer, buf_len, HID_TX_TIMEOUT);
        buffer = NULL;
    }
    WATCHDOG_RESET();

    return 0;
}

static int artousb_upgrade_from_ram(unsigned char * upgrade_img, unsigned int len)
{
    int ret = -1;
    char c;

    ret = 0;
#if 1
try_do_upgrade:
    ret = do_upgrade((unsigned char *)upgrade_img, artousb_show_upgrade);
    if (ret == -7) {
        /* Check conole inuput, force upgrade only if input is 'f' */
        printf("DDR change!!! reset or press 'f' to force upgrade......\n");
        while (tstc())
            (void)getc();
        do {
            if (tstc()) {
                c = getc();
                if (c == 'f' || c == 'F') {
                    printf("Force upgrade......\n");
                    break;
                }
            }
            mdelay(500);
        } while(true);
        artosyn_set_force_upgrade(true);
        goto try_do_upgrade;
    }
    artosyn_set_force_upgrade(false);
#endif
    return ret;
}

int artousb_sys_upgrade_recv(void * data, unsigned int len)
{
    struct f_artousb    *f_arusb    = get_arusb();
    char                mark[]  = upgrade_start_mark;
    int                 ret = -1;
    char                *buffer = NULL;

    if(strlen(mark) + sizeof(int) == len)
    {
        char * pcmd = (char *)data;
        printf("upgrade start cmd check! cmd:%c%c%c%c\n", pcmd[0], pcmd[1], pcmd[2], pcmd[3]);

        if(0 == artousb_check_start_mark((char *)data, mark, strlen(mark)))
        {
            unsigned int * data_len = (unsigned int *)((char *)data + strlen(upgrade_start_mark));

            if(NULL != f_arusb->image.pimage)
            {
                f_arusb->image.pimage = NULL;
            }

            printf("upgrade receive start! len:%d\n", *data_len);

            f_arusb->image.pimage = f_arusb->buf_head;//set buffer

            if(f_arusb->image.pimage == NULL)
            {
                printf("spl buffer is null\n");
                buffer = (char *)get_hid_send_buffer(16, INDEX_SYS_UPGRADE);
                if(buffer != NULL)
                {
                    sprintf(buffer, "spl buffer is null\n");
                    hid_send_data(buffer, 16, HID_TX_TIMEOUT);
                    buffer = NULL;
                }
            }
            f_arusb->image.offset = 0;
            f_arusb->image.total_len = *data_len;
            return 0;
        }
        else
        {
            printf("upgrade start cmd check failed!\n");
        }
    }

    if(0 != f_arusb->image.total_len && f_arusb->image.offset < f_arusb->image.total_len)
    {
        if(f_arusb->image.pimage == NULL)
        {
            printf("buffer allocate for image is null\n");
            return -1;
        }

        memcpy(f_arusb->image.pimage + f_arusb->image.offset, data, len);
        f_arusb->image.offset += len;

        if(f_arusb->image.offset >= f_arusb->image.total_len)
        {
            int upgrade_ret;

            buffer = (char *)get_hid_send_buffer(64, INDEX_SYS_UPGRADE);
            printf("upgrade receive complete(offset: %d total_len: %d)!\n", f_arusb->image.offset, f_arusb->image.total_len);

            //set completed flag
            ar_image_dl_completed = 1;

            if((upgrade_ret = artousb_upgrade_from_ram(f_arusb->image.pimage, f_arusb->image.total_len)) < 0)
            {
                if(NULL != buffer)
                {
                    sprintf(buffer, "upgrade failed!(ret = %d)\n", upgrade_ret);
                }
                printf("upgrade failed!(ret = %d)\n", upgrade_ret);
            }
            else
            {
                if(NULL != buffer)
                {
                    sprintf(buffer, "upgrade complete\n");
                }
                printf("upgrade complete!\n");
                printf("artousb upgrade finish, please reset the board...\n");
            }

            //free(image.pimage);
            f_arusb->image.offset = 0;
            f_arusb->image.total_len = 0;
            f_arusb->image.pimage = NULL;

            //todo notify host
            if(NULL != buffer)
            {
                hid_send_data(buffer, 64, HID_TX_TIMEOUT);
                buffer = NULL;
            }
        }
        ret = 0;
    }
    else
    {
        printf("sys_upgrade_revc no operation!\n");
    }

    return ret;
}

int artousb_uboot_filetran_recv(void * data, unsigned int len)
{
    struct f_artousb    *f_arusb    = get_arusb();
    int ret = -1;

    //printf("len:%d\n", len);

    if(strlen(ufiletran_start_mark) + sizeof(int) + MAX_DDR_ADDRESS_LEN == len)
    {
        char * pcmd = (char *)data;
        printf("fileRecv start cmd check! cmd:%c%c%c%c\n", pcmd[0], pcmd[1], pcmd[2], pcmd[3]);

        if(0 == artousb_check_start_mark((char *)data, ufiletran_start_mark, strlen(ufiletran_start_mark)))
        {
            unsigned int * data_len = (unsigned int *)((char *)data + strlen(ufiletran_start_mark));
            unsigned long * addr = (unsigned long *)((char *)data + strlen(ufiletran_start_mark) + sizeof(int));

            if(NULL != f_arusb->fileinfo.pfile)
            {
                f_arusb->fileinfo.pfile = NULL;
            }

            printf("file transfer receive start! len:%d addr:0x%lx\n", *data_len, *addr);

            f_arusb->fileinfo.pfile = (unsigned char *)(*addr);
            f_arusb->fileinfo.offset = 0;
            f_arusb->fileinfo.total_len = *data_len;
            return 0;
        }
        else
        {
            printf("file transfer start cmd check failed!\n");
        }
    }

    if(0 != f_arusb->fileinfo.total_len && f_arusb->fileinfo.offset < f_arusb->fileinfo.total_len)
    {
        if(4 != f_arusb->fileinfo.total_len)
        {
            memcpy(f_arusb->fileinfo.pfile + f_arusb->fileinfo.offset, (char *)data, len);
        }
        else
        {
            unsigned int * p = (unsigned int *)f_arusb->fileinfo.pfile;
            *p = *((unsigned int *)data);
        }
        f_arusb->fileinfo.offset += len;

        if(f_arusb->fileinfo.offset >= f_arusb->fileinfo.total_len)
        {
            printf("file transfer receive complete!\n");
            f_arusb->fileinfo.offset = 0;
            f_arusb->fileinfo.total_len = 0;
            f_arusb->fileinfo.pfile = NULL;
        }
        ret = 0;
    }
    else
    {
        printf("sys_filetran_revc no operation!\n");
    }

    return ret;
}

int artousb_uboot_fileget_recv(void * data, unsigned int len)
{
    struct f_artousb    *f_arusb    = get_arusb();
    //int ret;

    if(strlen(ufileget_start_mark) + sizeof(int) + MAX_DDR_ADDRESS_LEN == len)
    {
        char * pcmd = (char *)data;
        printf("start cmd check! cmd:%c%c%c%c\n", pcmd[0], pcmd[1], pcmd[2], pcmd[3]);

        if(0 == artousb_check_start_mark((char *)data, ufileget_start_mark, strlen(ufileget_start_mark)))
        {
            unsigned int * data_len = (unsigned int *)((char *)data + strlen(ufiletran_start_mark));
            unsigned long * addr = (unsigned long *)((char *)data + strlen(ufiletran_start_mark) + sizeof(int));
            //char   path[512] = {""};

            if(NULL != f_arusb->fgetinfo.pfile)
            {
                f_arusb->fgetinfo.pfile = NULL;
            }

            f_arusb->fgetinfo.pfile = (unsigned char *)(*addr);
            if(NULL == f_arusb->fgetinfo.pfile)
            {
                unsigned char * buffer = NULL;
                printf("target addr can not be zero!\n");
                buffer = get_hid_send_buffer(64, INDEX_SYS_FILEGET);
                if(NULL != buffer)
                {
                    sprintf((char *)buffer, "target addr can not be zero!\n");
                    hid_send_data(buffer, 64, 0);
                    buffer = NULL;
                }
                return 0;
            }
            else
            {
                unsigned char * buffer = NULL;

                f_arusb->fgetinfo.offset = 0;
                f_arusb->fgetinfo.total_len = *data_len;

                printf("file get start! len:%d addr:0x%lx\n", *data_len, *addr);
                //send file data
                buffer = get_hid_send_buffer(*data_len, INDEX_SYS_FILEGET);
                if(NULL != buffer)
                {
                    if(4 != *data_len)
                    {
                        memcpy((char *)buffer, f_arusb->fgetinfo.pfile , *data_len);
                    }
                    else
                    {
                        *((unsigned int *)buffer) = *((unsigned int *)f_arusb->fgetinfo.pfile);
                    }

                    hid_send_data(buffer, *data_len, 0);
                    buffer = NULL;
                    printf("fget commplete!\n");
                }

                f_arusb->fgetinfo.pfile = NULL;
            }
        }
    }
    return 0;
}

static int artousb_magic_num_check(unsigned char * data)
{
    unsigned int i;
    for(i = 0; i < sizeof(magic_header); ++i)
    {
        if(data[i] != magic_header[i])
            return -1;
    }
    return 0;
}

static int hid_protocol_analysis(unsigned char * data, unsigned len, unsigned int frame_idx, unsigned int left_frame)
{
    STRU_HID_HEADER *   phid_header;
    int ret;

    if(0 == frame_idx)//first fragment
    {
        phid_header = (STRU_HID_HEADER *)data;

        if(phid_header->port_index >= RECV_CALLBACK_NUM)
        {
            printf("port_index too big(%d)!\n", phid_header->port_index);
            return 1;
        }

        if(phid_header->header_len < sizeof(STRU_HID_HEADER))
        {
            printf("header_len too small(%d)!\n", phid_header->header_len);
            return 1;
        }

        if(NULL != g_hid_recv_callback[phid_header->port_index].recv_callback)//call back check
        {
            if(left_frame > 0)//more fragment
            {
                fragment_info.port_index = phid_header->port_index;//record port index
                fragment_info.data_len = phid_header->data_len;
            }

            ret = g_hid_recv_callback[phid_header->port_index].recv_callback(data + phid_header->header_len,
                                                                             len - phid_header->header_len);

            return ret;
        }
        else//no call back
        {
            printf("unregistered port(%d)!\n", phid_header->port_index);
            return 1;
        }
    }
    else//other fragment(no hid header)
    {
        if(0 != fragment_info.data_len)
        {
            ret = g_hid_recv_callback[fragment_info.port_index].recv_callback(data, len);
        }
        else
        {
            printf("no header!\n");
            ret = 1;
        }
        return ret;
    }
}

static void artousb_rx_handler(struct usb_ep *ep, struct usb_request *req)
{
    STRU_USB_HEADER *   pusb_header = NULL;
    int                 read_len = 0;
    int                 ret = 0;
    struct f_artousb    *f_arusb    = get_arusb();

    if (req->status || req->length == 0)
        return;

    read_len = req->actual;
    pusb_header = (STRU_USB_HEADER *)req->buf;
    if(pusb_header)
    {
        //printf("0x%02x%02x%02x%02x len:%d\n", req->buf[0], req->buf[1], req->buf[2], req->buf[3], read_len);

        if(0 == artousb_magic_num_check(pusb_header->magic_num))//header check
        {
            if(HID_PROTOCOL == pusb_header->sub_proto)//usb protocol check
            {
                if(pusb_header->payload_len <= f_arusb->report_length)//EP_BUFFER_SIZE)//mtu check
                {
                    if(pusb_header->fragment_num > 1)//more fragment
                    {
                        if(pusb_header->fragment_idx == 0)//first fragment
                        {
                            fragment_info.fragment_idx = 0;
                        }
                        else if(fragment_info.fragment_idx != pusb_header->fragment_idx)//check fragment index
                        {
                            printf("index no match!\n");
                            if(fragment_info.fragment_idx > pusb_header->fragment_idx)
                            {
                                printf("repeat data!!!\n");
                                return;
                            }
                        }

                        ret = hid_protocol_analysis(req->buf + sizeof(STRU_USB_HEADER),
                                                    read_len - sizeof(STRU_USB_HEADER),
                                                    pusb_header->fragment_idx,
                                                    pusb_header->fragment_num - 1 - pusb_header->fragment_idx);

                        ++fragment_info.fragment_idx;//next fragment index

                        if(pusb_header->fragment_idx == pusb_header->fragment_num - 1)
                        {
                            memset(&fragment_info, 0, sizeof(STRU_USB_FRAGMENT_INFO));
                        }
                    }
                    else//one fragment
                    {
                        hid_protocol_analysis(req->buf + sizeof(STRU_USB_HEADER),
                                              read_len - sizeof(STRU_USB_HEADER),
                                              0, 0);
                    }
                }
                else
                {
                    printf("size out of mtu %d(%d)!\n", f_arusb->report_length, pusb_header->payload_len);//EP_BUFFER_SIZE, pusb_header->payload_len);
                }
            }
            else
            {
                printf("sub_proto no match hid(0x%x)! (0x%x)\n", HID_PROTOCOL, pusb_header->sub_proto);
            }
        }
        else
        {
            printf("magic num error!\n");
        }
    }

    req->actual = 0;
    usb_ep_queue(ep, req, 0);
}


