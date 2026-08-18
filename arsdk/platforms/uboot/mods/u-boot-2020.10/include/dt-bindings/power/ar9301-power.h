/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Header providing constants for Artosyn pmu bindings.
 *
 * Copyright (c) 2021 Artosyn
 */

#ifndef __DT_BINDINGS_PMU_AR9301_H__
#define __DT_BINDINGS_PMU_AR9301_H__

#define AR9301_PWR_CTRL_SOC_VENC         0
#define AR9301_PWR_CTRL_SOC_ISP          1
#define AR9301_PWR_CTRL_SOC_DLA          2
#define AR9301_PWR_CTRL_A53_CORE1        3
#define AR9301_PWR_CTRL_A53_CORE2        4
#define AR9301_PWR_CTRL_A53_CORE3        5
#define AR9301_PWR_CTRL_CEVA_CORE0       6

#define AR9301_PWR_CTRL_MAX              7

#define AR9301_PWR_CTRL_GATING_SHIFT     16
#define AR9301_PWR_CTRL_OFF              (0 << AR9301_PWR_CTRL_GATING_SHIFT)

#endif
