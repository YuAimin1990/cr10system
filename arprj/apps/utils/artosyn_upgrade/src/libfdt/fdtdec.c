/*
 * Copyright (c) 2011 The Chromium OS Authors.
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef USE_HOSTCC
#include <common.h> //for assert
#include <assert.h>
#include <errno.h>
#include <compiler.h>
#include <libfdt.h>
#include <fdtdec.h>
#include <stdlib.h>
#define debug(...)


/*
 * Here are the type we know about. One day we might allow drivers to
 * register. For now we just put them here. The COMPAT macro allows us to
 * turn this into a sparse list later, and keeps the ID with the name.
 *
 * NOTE: This list is basically a TODO list for things that need to be
 * converted to driver model. So don't add new things here unless there is a
 * good reason why driver-model conversion is infeasible. Examples include
 * things which are used before driver model is available.
 */
#define COMPAT(id, name) name
static const char * const compat_names[COMPAT_COUNT] = {
	COMPAT(UNKNOWN, "<none>"),
	COMPAT(NVIDIA_TEGRA20_EMC, "nvidia,tegra20-emc"),
	COMPAT(NVIDIA_TEGRA20_EMC_TABLE, "nvidia,tegra20-emc-table"),
	COMPAT(NVIDIA_TEGRA20_NAND, "nvidia,tegra20-nand"),
	COMPAT(NVIDIA_TEGRA124_PMC, "nvidia,tegra124-pmc"),
	COMPAT(NVIDIA_TEGRA186_SDMMC, "nvidia,tegra186-sdhci"),
	COMPAT(NVIDIA_TEGRA210_SDMMC, "nvidia,tegra210-sdhci"),
	COMPAT(NVIDIA_TEGRA124_SDMMC, "nvidia,tegra124-sdhci"),
	COMPAT(NVIDIA_TEGRA30_SDMMC, "nvidia,tegra30-sdhci"),
	COMPAT(NVIDIA_TEGRA20_SDMMC, "nvidia,tegra20-sdhci"),
	COMPAT(NVIDIA_TEGRA124_XUSB_PADCTL, "nvidia,tegra124-xusb-padctl"),
	COMPAT(NVIDIA_TEGRA210_XUSB_PADCTL, "nvidia,tegra210-xusb-padctl"),
	COMPAT(SMSC_LAN9215, "smsc,lan9215"),
	COMPAT(SAMSUNG_EXYNOS5_SROMC, "samsung,exynos-sromc"),
	COMPAT(SAMSUNG_S3C2440_I2C, "samsung,s3c2440-i2c"),
	COMPAT(SAMSUNG_EXYNOS5_SOUND, "samsung,exynos-sound"),
	COMPAT(WOLFSON_WM8994_CODEC, "wolfson,wm8994-codec"),
	COMPAT(SAMSUNG_EXYNOS_USB_PHY, "samsung,exynos-usb-phy"),
	COMPAT(SAMSUNG_EXYNOS5_USB3_PHY, "samsung,exynos5250-usb3-phy"),
	COMPAT(SAMSUNG_EXYNOS_TMU, "samsung,exynos-tmu"),
	COMPAT(SAMSUNG_EXYNOS_MIPI_DSI, "samsung,exynos-mipi-dsi"),
	COMPAT(SAMSUNG_EXYNOS_DWMMC, "samsung,exynos-dwmmc"),
	COMPAT(SAMSUNG_EXYNOS_MMC, "samsung,exynos-mmc"),
	COMPAT(MAXIM_MAX77686_PMIC, "maxim,max77686"),
	COMPAT(GENERIC_SPI_FLASH, "spi-flash"),
	COMPAT(MAXIM_98095_CODEC, "maxim,max98095-codec"),
	COMPAT(SAMSUNG_EXYNOS5_I2C, "samsung,exynos5-hsi2c"),
	COMPAT(SAMSUNG_EXYNOS_SYSMMU, "samsung,sysmmu-v3.3"),
	COMPAT(INTEL_MICROCODE, "intel,microcode"),
	COMPAT(AMS_AS3722, "ams,as3722"),
	COMPAT(INTEL_QRK_MRC, "intel,quark-mrc"),
	COMPAT(ALTERA_SOCFPGA_DWMAC, "altr,socfpga-stmmac"),
	COMPAT(ALTERA_SOCFPGA_DWMMC, "altr,socfpga-dw-mshc"),
	COMPAT(ALTERA_SOCFPGA_DWC2USB, "snps,dwc2"),
	COMPAT(INTEL_BAYTRAIL_FSP, "intel,baytrail-fsp"),
	COMPAT(INTEL_BAYTRAIL_FSP_MDP, "intel,baytrail-fsp-mdp"),
	COMPAT(INTEL_IVYBRIDGE_FSP, "intel,ivybridge-fsp"),
	COMPAT(COMPAT_SUNXI_NAND, "allwinner,sun4i-a10-nand"),
	COMPAT(ALTERA_SOCFPGA_CLK, "altr,clk-mgr"),
	COMPAT(ALTERA_SOCFPGA_PINCTRL_SINGLE, "pinctrl-single"),
	COMPAT(ALTERA_SOCFPGA_H2F_BRG, "altr,socfpga-hps2fpga-bridge"),
	COMPAT(ALTERA_SOCFPGA_LWH2F_BRG, "altr,socfpga-lwhps2fpga-bridge"),
	COMPAT(ALTERA_SOCFPGA_F2H_BRG, "altr,socfpga-fpga2hps-bridge"),
	COMPAT(ALTERA_SOCFPGA_F2SDR0, "altr,socfpga-fpga2sdram0-bridge"),
	COMPAT(ALTERA_SOCFPGA_F2SDR1, "altr,socfpga-fpga2sdram1-bridge"),
	COMPAT(ALTERA_SOCFPGA_F2SDR2, "altr,socfpga-fpga2sdram2-bridge"),
};

const char *fdtdec_get_compatible(enum fdt_compat_id id)
{
	/* We allow reading of the 'unknown' ID for testing purposes */
	assert(id >= 0 && id < COMPAT_COUNT);
	return compat_names[id];
}

fdt_addr_t fdtdec_get_addr_size_fixed(const void *blob, int node,
		const char *prop_name, int index, int na, int ns,
		fdt_size_t *sizep, int translate)
{
	const fdt32_t *prop, *prop_end;
	const fdt32_t *prop_addr, *prop_size, *prop_after_size;
	int len;
	fdt_addr_t addr;

	debug("%s: %s: ", __func__, prop_name);

	if (na > (sizeof(fdt_addr_t) / sizeof(fdt32_t))) {
		debug("(na too large for fdt_addr_t type)\n");
		return FDT_ADDR_T_NONE;
	}

	if (ns > (sizeof(fdt_size_t) / sizeof(fdt32_t))) {
		debug("(ns too large for fdt_size_t type)\n");
		return FDT_ADDR_T_NONE;
	}

	prop = fdt_getprop(blob, node, prop_name, &len);
	if (!prop) {
		debug("(not found)\n");
		return FDT_ADDR_T_NONE;
	}
	prop_end = prop + (len / sizeof(*prop));

	prop_addr = prop + (index * (na + ns));
	prop_size = prop_addr + na;
	prop_after_size = prop_size + ns;
	if (prop_after_size > prop_end) {
		debug("(not enough data: expected >= %d cells, got %d cells)\n",
		      (unsigned int)(prop_after_size - prop), ((unsigned int)(prop_end - prop)));
		return FDT_ADDR_T_NONE;
	}

#if 0
#if CONFIG_IS_ENABLED(OF_TRANSLATE)
	if (translate)
		addr = fdt_translate_address(blob, node, prop_addr);
	else
#endif
#endif

		addr = fdtdec_get_number(prop_addr, na);

	if (sizep) {
		*sizep = fdtdec_get_number(prop_size, ns);
		debug("addr=%08llx, size=%llx\n", (unsigned long long)addr,
		      (unsigned long long)*sizep);
	} else {
		debug("addr=%08llx\n", (unsigned long long)addr);
	}

	return addr;
}

fdt_addr_t fdtdec_get_addr_size_auto_parent(const void *blob, int parent,
		int node, const char *prop_name, int index, fdt_size_t *sizep,
		int translate)
{
	int na, ns;

	debug("%s: ", __func__);

	na = fdt_address_cells(blob, parent);
	if (na < 1) {
		debug("(bad #address-cells)\n");
		return FDT_ADDR_T_NONE;
	}

	ns = fdt_size_cells(blob, parent);
	if (ns < 0) {
		debug("(bad #size-cells)\n");
		return FDT_ADDR_T_NONE;
	}

	debug("na=%d, ns=%d, ", na, ns);

	return fdtdec_get_addr_size_fixed(blob, node, prop_name, index, na,
					  ns, sizep, translate);
}

fdt_addr_t fdtdec_get_addr_size_auto_noparent(const void *blob, int node,
		const char *prop_name, int index, fdt_size_t *sizep,
		int translate)
{
	int parent;

	debug("%s: ", __func__);

	parent = fdt_parent_offset(blob, node);
	if (parent < 0) {
		debug("(no parent found)\n");
		return FDT_ADDR_T_NONE;
	}

	return fdtdec_get_addr_size_auto_parent(blob, parent, node, prop_name,
						index, sizep, translate);
}

fdt_addr_t fdtdec_get_addr_size(const void *blob, int node,
		const char *prop_name, fdt_size_t *sizep)
{
	int ns = sizep ? (sizeof(fdt_size_t) / sizeof(fdt32_t)) : 0;

	return fdtdec_get_addr_size_fixed(blob, node, prop_name, 0,
					  sizeof(fdt_addr_t) / sizeof(fdt32_t),
					  ns, sizep, 0);
}

fdt_addr_t fdtdec_get_addr(const void *blob, int node,
		const char *prop_name)
{
	return fdtdec_get_addr_size(blob, node, prop_name, NULL);
}


uint64_t fdtdec_get_uint64(const void *blob, int node, const char *prop_name,
		uint64_t default_val)
{
	const uint64_t *cell64;
	int length;

	cell64 = fdt_getprop(blob, node, prop_name, &length);
	if (!cell64 || length < sizeof(*cell64))
		return default_val;

	return fdt64_to_cpu(*cell64);
}

int fdtdec_get_is_enabled(const void *blob, int node)
{
	const char *cell;

	/*
	 * It should say "okay", so only allow that. Some fdts use "ok" but
	 * this is a bug. Please fix your device tree source file. See here
	 * for discussion:
	 *
	 * http://www.mail-archive.com/u-boot@lists.denx.de/msg71598.html
	 */
	cell = fdt_getprop(blob, node, "status", NULL);
	if (cell)
		return 0 == strcmp(cell, "okay");
	return 1;
}

enum fdt_compat_id fdtdec_lookup(const void *blob, int node)
{
	enum fdt_compat_id id;

	/* Search our drivers */
	for (id = COMPAT_UNKNOWN; id < COMPAT_COUNT; id++)
		if (0 == fdt_node_check_compatible(blob, node,
				compat_names[id]))
			return id;
	return COMPAT_UNKNOWN;
}

int fdtdec_next_compatible(const void *blob, int node,
		enum fdt_compat_id id)
{
	return fdt_node_offset_by_compatible(blob, node, compat_names[id]);
}

int fdtdec_next_compatible_subnode(const void *blob, int node,
		enum fdt_compat_id id, int *depthp)
{
	do {
		node = fdt_next_node(blob, node, depthp);
	} while (*depthp > 1);

	/* If this is a direct subnode, and compatible, return it */
	if (*depthp == 1 && 0 == fdt_node_check_compatible(
						blob, node, compat_names[id]))
		return node;

	return -FDT_ERR_NOTFOUND;
}

int fdtdec_next_alias(const void *blob, const char *name,
		enum fdt_compat_id id, int *upto)
{
#define MAX_STR_LEN 20
	char str[MAX_STR_LEN + 20];
	int node, err;

	/* snprintf() is not available */
	assert(strlen(name) < MAX_STR_LEN);
	sprintf(str, "%.*s%d", MAX_STR_LEN, name, *upto);
	node = fdt_path_offset(blob, str);
	if (node < 0)
		return node;
	err = fdt_node_check_compatible(blob, node, compat_names[id]);
	if (err < 0)
		return err;
	if (err)
		return -FDT_ERR_NOTFOUND;
	(*upto)++;
	return node;
}

int fdtdec_find_aliases_for_id(const void *blob, const char *name,
			enum fdt_compat_id id, int *node_list, int maxcount)
{
	memset(node_list, '\0', sizeof(*node_list) * maxcount);

	return fdtdec_add_aliases_for_id(blob, name, id, node_list, maxcount);
}

/* TODO: Can we tighten this code up a little? */
int fdtdec_add_aliases_for_id(const void *blob, const char *name,
			enum fdt_compat_id id, int *node_list, int maxcount)
{
	int name_len = strlen(name);
	int nodes[maxcount];
	int num_found = 0;
	int offset, node;
	int alias_node;
	int count;
	int i, j;

	/* find the alias node if present */
	alias_node = fdt_path_offset(blob, "/aliases");

	/*
	 * start with nothing, and we can assume that the root node can't
	 * match
	 */
	memset(nodes, '\0', sizeof(nodes));

	/* First find all the compatible nodes */
	for (node = count = 0; node >= 0 && count < maxcount;) {
		node = fdtdec_next_compatible(blob, node, id);
		if (node >= 0)
			nodes[count++] = node;
	}
	if (node >= 0)
		debug("%s: warning: maxcount exceeded with alias '%s'\n",
		       __func__, name);

	/* Now find all the aliases */
	for (offset = fdt_first_property_offset(blob, alias_node);
			offset > 0;
			offset = fdt_next_property_offset(blob, offset)) {
		const struct fdt_property *prop;
		const char *path;
		int number;
		int found;

		node = 0;
		prop = fdt_get_property_by_offset(blob, offset, NULL);
		path = fdt_string(blob, fdt32_to_cpu(prop->nameoff));
		if (prop->len && 0 == strncmp(path, name, name_len))
			node = fdt_path_offset(blob, prop->data);
		if (node <= 0)
			continue;

		/* Get the alias number */
		number = strtoul(path + name_len, NULL, 10);
		if (number < 0 || number >= maxcount) {
			debug("%s: warning: alias '%s' is out of range\n",
			       __func__, path);
			continue;
		}

		/* Make sure the node we found is actually in our list! */
		found = -1;
		for (j = 0; j < count; j++)
			if (nodes[j] == node) {
				found = j;
				break;
			}

		if (found == -1) {
			debug("%s: warning: alias '%s' points to a node "
				"'%s' that is missing or is not compatible "
				" with '%s'\n", __func__, path,
				fdt_get_name(blob, node, NULL),
			       compat_names[id]);
			continue;
		}

		/*
		 * Add this node to our list in the right place, and mark
		 * it as done.
		 */
		if (fdtdec_get_is_enabled(blob, node)) {
			if (node_list[number]) {
				debug("%s: warning: alias '%s' requires that "
				      "a node be placed in the list in a "
				      "position which is already filled by "
				      "node '%s'\n", __func__, path,
				      fdt_get_name(blob, node, NULL));
				continue;
			}
			node_list[number] = node;
			if (number >= num_found)
				num_found = number + 1;
		}
		nodes[found] = 0;
	}

	/* Add any nodes not mentioned by an alias */
	for (i = j = 0; i < maxcount; i++) {
		if (!node_list[i]) {
			for (; j < maxcount; j++)
				if (nodes[j] &&
					fdtdec_get_is_enabled(blob, nodes[j]))
					break;

			/* Have we run out of nodes to add? */
			if (j == maxcount)
				break;

			assert(!node_list[i]);
			node_list[i] = nodes[j++];
			if (i >= num_found)
				num_found = i + 1;
		}
	}

	return num_found;
}
#if 0
int fdtdec_get_alias_seq(const void *blob, const char *base, int offset,
			 int *seqp)
{
	int base_len = strlen(base);
	const char *find_name;
	int find_namelen;
	int prop_offset;
	int aliases;

	find_name = fdt_get_name(blob, offset, &find_namelen);
	debug("Looking for '%s' at %d, name %s\n", base, offset, find_name);

	aliases = fdt_path_offset(blob, "/aliases");
	for (prop_offset = fdt_first_property_offset(blob, aliases);
	     prop_offset > 0;
	     prop_offset = fdt_next_property_offset(blob, prop_offset)) {
		const char *prop;
		const char *name;
		const char *slash;
		int len, val;

		prop = fdt_getprop_by_offset(blob, prop_offset, &name, &len);
		debug("   - %s, %s\n", name, prop);
		if (len < find_namelen || *prop != '/' || prop[len - 1] ||
		    strncmp(name, base, base_len))
			continue;

		slash = strrchr(prop, '/');
		if (strcmp(slash + 1, find_name))
			continue;
		val = trailing_strtol(name);
		if (val != -1) {
			*seqp = val;
			debug("Found seq %d\n", *seqp);
			return 0;
		}
	}

	debug("Not found\n");
	return -ENOENT;
}
#endif
const char *fdtdec_get_chosen_prop(const void *blob, const char *name)
{
	int chosen_node;

	if (!blob)
		return NULL;
	chosen_node = fdt_path_offset(blob, "/chosen");
	return fdt_getprop(blob, chosen_node, name, NULL);
}

int fdtdec_get_chosen_node(const void *blob, const char *name)
{
	const char *prop;

	prop = fdtdec_get_chosen_prop(blob, name);
	if (!prop)
		return -FDT_ERR_NOTFOUND;
	return fdt_path_offset(blob, prop);
}


int fdtdec_lookup_phandle(const void *blob, int node, const char *prop_name)
{
	const unsigned int *phandle;
	int lookup;

	debug("%s: %s\n", __func__, prop_name);
	phandle = fdt_getprop(blob, node, prop_name, NULL);
	if (!phandle)
		return -FDT_ERR_NOTFOUND;

	lookup = fdt_node_offset_by_phandle(blob, fdt32_to_cpu(*phandle));
	return lookup;
}

/**
 * Look up a property in a node and check that it has a minimum length.
 *
 * @param blob		FDT blob
 * @param node		node to examine
 * @param prop_name	name of property to find
 * @param min_len	minimum property length in bytes
 * @param err		0 if ok, or -FDT_ERR_NOTFOUND if the property is not
			found, or -FDT_ERR_BADLAYOUT if not enough data
 * @return pointer to cell, which is only valid if err == 0
 */
static const void *get_prop_check_min_len(const void *blob, int node,
		const char *prop_name, int min_len, int *err)
{
	const void *cell;
	int len;

	debug("%s: %s\n", __func__, prop_name);
	cell = fdt_getprop(blob, node, prop_name, &len);
	if (!cell)
		*err = -FDT_ERR_NOTFOUND;
	else if (len < min_len)
		*err = -FDT_ERR_BADLAYOUT;
	else
		*err = 0;
	return cell;
}

int fdtdec_get_int_array(const void *blob, int node, const char *prop_name,
		unsigned int *array, int count)
{
	const unsigned int *cell;
	int i, err = 0;

	debug("%s: %s\n", __func__, prop_name);
	cell = get_prop_check_min_len(blob, node, prop_name,
				      sizeof(unsigned int) * count, &err);
	if (!err) {
		for (i = 0; i < count; i++)
			array[i] = fdt32_to_cpu(cell[i]);
	}
	return err;
}

int fdtdec_get_int_array_count(const void *blob, int node,
			       const char *prop_name, unsigned int *array, int count)
{
	const unsigned int *cell;
	int len, elems;
	int i;

	debug("%s: %s\n", __func__, prop_name);
	cell = fdt_getprop(blob, node, prop_name, &len);
	if (!cell)
		return -FDT_ERR_NOTFOUND;
	elems = len / sizeof(unsigned int);
	if (count > elems)
		count = elems;
	for (i = 0; i < count; i++)
		array[i] = fdt32_to_cpu(cell[i]);

	return count;
}

const unsigned int *fdtdec_locate_array(const void *blob, int node,
			       const char *prop_name, int count)
{
	const unsigned int *cell;
	int err;

	cell = get_prop_check_min_len(blob, node, prop_name,
				      sizeof(unsigned int) * count, &err);
	return err ? NULL : cell;
}

int fdtdec_get_bool(const void *blob, int node, const char *prop_name)
{
	const int *cell;
	int len;

	debug("%s: %s\n", __func__, prop_name);
	cell = fdt_getprop(blob, node, prop_name, &len);
	return cell != NULL;
}

int fdtdec_parse_phandle_with_args(const void *blob, int src_node,
				   const char *list_name,
				   const char *cells_name,
				   int cell_count, int index,
				   struct fdtdec_phandle_args *out_args)
{
	const __be32 *list, *list_end;
	int rc = 0, size, cur_index = 0;
	uint32_t count = 0;
	int node = -1;
	int phandle;

	/* Retrieve the phandle list property */
	list = fdt_getprop(blob, src_node, list_name, &size);
	if (!list)
		return -ENOENT;
	list_end = list + size / sizeof(*list);

	/* Loop over the phandles until all the requested entry is found */
	while (list < list_end) {
		rc = -EINVAL;
		count = 0;

		/*
		 * If phandle is 0, then it is an empty entry with no
		 * arguments.  Skip forward to the next entry.
		 */
		phandle = be32_to_cpup(list++);
		if (phandle) {
			/*
			 * Find the provider node and parse the #*-cells
			 * property to determine the argument length.
			 *
			 * This is not needed if the cell count is hard-coded
			 * (i.e. cells_name not set, but cell_count is set),
			 * except when we're going to return the found node
			 * below.
			 */
			if (cells_name || cur_index == index) {
				node = fdt_node_offset_by_phandle(blob,
								  phandle);
				if (!node) {
					debug("%s: could not find phandle\n",
					      fdt_get_name(blob, src_node,
							   NULL));
					goto err;
				}
			}

			if (cells_name) {
				count = fdtdec_get_int(blob, node, cells_name,
						       -1);
				if (count == -1) {
					debug("%s: could not get %s for %s\n",
					      fdt_get_name(blob, src_node,
							   NULL),
					      cells_name,
					      fdt_get_name(blob, node,
							   NULL));
					goto err;
				}
			} else {
				count = cell_count;
			}

			/*
			 * Make sure that the arguments actually fit in the
			 * remaining property data length
			 */
			if (list + count > list_end) {
				debug("%s: arguments longer than property\n",
				      fdt_get_name(blob, src_node, NULL));
				goto err;
			}
		}

		/*
		 * All of the error cases above bail out of the loop, so at
		 * this point, the parsing is successful. If the requested
		 * index matches, then fill the out_args structure and return,
		 * or return -ENOENT for an empty entry.
		 */
		rc = -ENOENT;
		if (cur_index == index) {
			if (!phandle)
				goto err;

			if (out_args) {
				int i;

				if (count > MAX_PHANDLE_ARGS) {
					debug("%s: too many arguments %d\n",
					      fdt_get_name(blob, src_node,
							   NULL), count);
					count = MAX_PHANDLE_ARGS;
				}
				out_args->node = node;
				out_args->args_count = count;
				for (i = 0; i < count; i++) {
					out_args->args[i] =
							be32_to_cpup(list++);
				}
			}

			/* Found it! return success */
			return 0;
		}

		node = -1;
		list += count;
		cur_index++;
	}

	/*
	 * Result will be one of:
	 * -ENOENT : index is for empty phandle
	 * -EINVAL : parsing error on data
	 * [1..n]  : Number of phandle (count mode; when index = -1)
	 */
	rc = index < 0 ? cur_index : -ENOENT;
 err:
	return rc;
}

int fdtdec_get_child_count(const void *blob, int node)
{
	int subnode;
	int num = 0;

	fdt_for_each_subnode(subnode, blob, node)
		num++;

	return num;
}

int fdtdec_get_byte_array(const void *blob, int node, const char *prop_name,
		u8 *array, int count)
{
	const u8 *cell;
	int err;

	cell = get_prop_check_min_len(blob, node, prop_name, count, &err);
	if (!err)
		memcpy(array, cell, count);
	return err;
}

const u8 *fdtdec_locate_byte_array(const void *blob, int node,
			     const char *prop_name, int count)
{
	const u8 *cell;
	int err;

	cell = get_prop_check_min_len(blob, node, prop_name, count, &err);
	if (err)
		return NULL;
	return cell;
}

int fdtdec_get_config_int(const void *blob, const char *prop_name,
		int default_val)
{
	int config_node;

	debug("%s: %s\n", __func__, prop_name);
	config_node = fdt_path_offset(blob, "/config");
	if (config_node < 0)
		return default_val;
	return fdtdec_get_int(blob, config_node, prop_name, default_val);
}

int fdtdec_get_config_bool(const void *blob, const char *prop_name)
{
	int config_node;
	const void *prop;

	debug("%s: %s\n", __func__, prop_name);
	config_node = fdt_path_offset(blob, "/config");
	if (config_node < 0)
		return 0;
	prop = fdt_get_property(blob, config_node, prop_name, NULL);

	return prop != NULL;
}

char *fdtdec_get_config_string(const void *blob, const char *prop_name)
{
	const char *nodep;
	int nodeoffset;
	int len;

	debug("%s: %s\n", __func__, prop_name);
	nodeoffset = fdt_path_offset(blob, "/config");
	if (nodeoffset < 0)
		return NULL;

	nodep = fdt_getprop(blob, nodeoffset, prop_name, &len);
	if (!nodep)
		return NULL;

	return (char *)nodep;
}

int fdtdec_decode_region(const void *blob, int node, const char *prop_name,
			 fdt_addr_t *basep, fdt_size_t *sizep)
{
	const fdt_addr_t *cell;
	int len;

	debug("%s: %s: %s\n", __func__, fdt_get_name(blob, node, NULL),
	      prop_name);
	cell = fdt_getprop(blob, node, prop_name, &len);
	if (!cell || (len < sizeof(fdt_addr_t) * 2)) {
		debug("cell=%p, len=%d\n", cell, len);
		return -1;
	}

	*basep = fdt_addr_to_cpu(*cell);
	*sizep = fdt_size_to_cpu(cell[1]);
	debug("%s: base=%08lx, size=%lx\n", __func__, (ulong)*basep,
	      (ulong)*sizep);

	return 0;
}

u64 fdtdec_get_number(const fdt32_t *ptr, unsigned int cells)
{
	u64 number = 0;

	while (cells--)
		number = (number << 32) | fdt32_to_cpu(*ptr++);

	return number;
}
#if 0
void * __attribute__((weak)) board_fdt_blob_setup(void)
{
	debug("%s", "board_fdt_blob_setup == NULL\n");

	return (void *)UBOOT_LOAD_CFG_DTB_ADDR;

}
#endif
#endif /* !USE_HOSTCC */
