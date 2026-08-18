/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Header providing constants for Artosyn power domain bindings.
 *
 * Copyright (c) 2021 Artosyn
 */

#include <common.h>
#include <dm.h>
#include <malloc.h>
#include <power-domain.h>
#include <power-domain-uclass.h>
#include <asm/io.h>
#include <command.h>

#include <dt-bindings/power/ar9301-power.h>

enum pd_action
{
    AR9301_PWR_CTRL_ACTION_SET_OFF,
    AR9301_PWR_CTRL_ACTION_SET_ON,
    AR9301_PWR_CTRL_ACTION_MAX
};

static char* artosyn_pd_name[AR9301_PWR_CTRL_MAX] = {
    [AR9301_PWR_CTRL_SOC_VENC] = "venc",
    [AR9301_PWR_CTRL_SOC_ISP] = "isp",
    [AR9301_PWR_CTRL_SOC_DLA] = "dla",
    [AR9301_PWR_CTRL_A53_CORE1] = "a53_core1",
    [AR9301_PWR_CTRL_A53_CORE2] = "a53_core2",
    [AR9301_PWR_CTRL_A53_CORE3] = "a53_core3",
    [AR9301_PWR_CTRL_CEVA_CORE0] = "ceva"
};

static char* artosyn_pd_action[AR9301_PWR_CTRL_ACTION_MAX] = {
    [AR9301_PWR_CTRL_ACTION_SET_OFF] = "off",
    [AR9301_PWR_CTRL_ACTION_SET_ON] = "on"
};

static int artosyn_pd_get_name_id(char* name)
{
    int pd_name_id = 0;

    for(pd_name_id=0; pd_name_id<AR9301_PWR_CTRL_MAX; pd_name_id++)
    {
        if(strcmp(artosyn_pd_name[pd_name_id], name) == 0)
        {
            return pd_name_id;
        }
    }
    printf("%s unsupported domain %s\n", __func__, name);
    return AR9301_PWR_CTRL_MAX;
}

static int artosyn_pd_get_action_id(char* action)
{
    int pd_action_id = 0;

    for(pd_action_id=0; pd_action_id<AR9301_PWR_CTRL_ACTION_MAX; pd_action_id++)
    {
        if(strcmp(artosyn_pd_action[pd_action_id], action) == 0)
        {
            return pd_action_id;
        }
    }
    printf("%s unsupported action %s\n", __func__, action);
    return AR9301_PWR_CTRL_ACTION_MAX;
}

static int get_pd_dev(const char *name, struct udevice **devp)
{
	int ret = 0;

	ret = uclass_get_device_by_name(UCLASS_POWER_DOMAIN, name, devp);
	if (ret) {
		printf("could not find device, ret=%d\n", ret);
		return ret;
	}

	return 0;
}

int artosyn_pd_test(int name_id, int action_id)
{
	int ret = 0;
	struct udevice *dev;
    struct power_domain pd;

    ret = get_pd_dev("pwr_ctrl@01075000", &dev);
    if(ret < 0)
    {
        return ret;
    }

	pd.dev = dev;
    pd.id = name_id;
    pd.priv = dev_get_priv(dev);

    switch(action_id)
    {
        default:
            break;

        case AR9301_PWR_CTRL_ACTION_SET_ON:
            ret = power_domain_on(&pd);
            break;

        case AR9301_PWR_CTRL_ACTION_SET_OFF:
            ret = power_domain_off(&pd);
            break;
    }
	printf("%s exit: %s\n", __func__, dev->name);
    return ret;
}

static int do_artosyn_pd_test(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    int pd_name_id = 0;
    int pd_action_id = 0;
    char *pd_name = NULL;
    char *pd_action = NULL;

    if (argc < 2)
    {
        printf("argc num: %d\n", argc);
        return CMD_RET_USAGE;
    }

    pd_name = argv[1];
    pd_name_id = artosyn_pd_get_name_id(pd_name);
    if(pd_name_id == AR9301_PWR_CTRL_MAX)
    {
        return -EINVAL;
    }

    if (argc == 3)
    {
        pd_action = argv[2];
        pd_action_id = artosyn_pd_get_action_id(pd_action);
        if(pd_action_id == AR9301_PWR_CTRL_ACTION_MAX)
        {
            return -EINVAL;
        }
    }

    artosyn_pd_test(pd_name_id, pd_action_id);
    return 0;
}

#ifndef CONFIG_SPL_BUILD
#ifdef CONFIG_SYS_LONGHELP
static char artosyn_pd_test_help_text[] =
	"<power_control_test> e.g. artosyn_pd_test <domains> <action>\n"
    "\tsupported domains:\tvenc isp dla a53_core1 a53_core2 a53_core3 ceva\n"
    "\tsupported actions:\ton off\n"
	"";
#endif

U_BOOT_CMD(artosyn_pd_test,    3, 1, do_artosyn_pd_test, "do artosyn_pd_test", artosyn_pd_test_help_text);
#endif

