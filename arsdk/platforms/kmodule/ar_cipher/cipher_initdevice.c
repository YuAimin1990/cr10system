#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include "hal_type.h"
//#include "drv_osal_lib.h"

extern int  CIPHER_DRV_ModInit(void);
extern void CIPHER_DRV_ModExit(void);

static int __init cipher_mod_init(void)
{
    return CIPHER_DRV_ModInit();
}

static void __exit cipher_mod_exit(void)
{
    CIPHER_DRV_ModExit();
}

module_init(cipher_mod_init);
module_exit(cipher_mod_exit);

MODULE_AUTHOR("Artosyn");
MODULE_DESCRIPTION("Artosyn cipher driver");
MODULE_LICENSE("GPL");

