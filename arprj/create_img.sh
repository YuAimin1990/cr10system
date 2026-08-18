#!/bin/bash
if [[ $1 == "--help" ]] || [[ $1 == "-h" ]]; then
	echo "-f <flashtype> (e.g: nand/nor/emmc)"
	echo "-p <project>   (e.g: fpga_config_debug)"
	exit 0
fi

while getopts "f:p:" opt; do
  case $opt in
    f)
      flashtype=$OPTARG
      ;;
    p)
      project=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done

echo flashtype: ${flashtype}
echo project: ${project}

if [ "${flashtype}" = "" ] || [ "${project}" = "" ]; then
	echo Invalid para!
	exit 0
fi

# 1.select project config
if [ -f configs/${project}/board.config ] ; then
	if [ -f .product_profile ]; then \
		rm -f .product_profile; \
	fi
	ln -sf configs/${project}/board.config .product_profile;
	echo ${project}"written to .product_profile"
fi

# 2. create dtb
make -f hosts/makery/make_dtb_rules.mk

# 3.create fs img
# 打包 fs 前，使用 cr10_*_fs_upgrade.tar 覆盖更新对应分区内容
# 如需禁用单个，设对应环境变量为空: ROOTFS_UPGRADE_TAR= ./create_img.sh ...
SDK_ROOT="$(realpath "$(dirname "$0")/../..")"
setup_fs_upgrade() {
	local env_var="$1"
	local default_name="$2"
	local cur="${!env_var}"
	if [ -z "$cur" ]; then
		eval "export ${env_var}=\"$(realpath "${SDK_ROOT}/${default_name}" 2>/dev/null)\""
	fi
	if [ -f "${!env_var}" ]; then
		echo "${env_var}: ${!env_var}"
	else
		echo "${env_var} not found, skip upgrade: ${!env_var}"
		unset "$env_var"
	fi
}
setup_fs_upgrade ROOTFS_UPGRADE_TAR   cr10_root_fs_upgrade.tar
setup_fs_upgrade USERDATA_UPGRADE_TAR cr10_usrdata_fs_upgrade.tar
make -f hosts/makery/fs_img.mk BOOT_MEDIA=${flashtype}

# 4.create uboot env img
make -f hosts/makery/uboot_env.mk BOOT_MEDIA=${flashtype}

# 5.create program img and upgrate img
make -f hosts/makery/image.mk BOOT_MEDIA=${flashtype}