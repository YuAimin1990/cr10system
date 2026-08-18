#!/usr/bin/env python

import argparse
import sys
import os
import struct
import subprocess
import ConfigParser
import time
import fileinput

sdk_real_path = "../arsdk/"
platforms_real_path = "../arsdk/platforms/"

kernel_tar_file = "linux-linaro-stable-lsk-v4.9-17.07.tar.gz"
uboot_tar_file = "u-boot-2020.10.tar.bz2"
busybox_tar_file = "busybox-1.25.0.tar.bz2"

def run_os_command(command, run_dir=None, dry_run=False):

	if run_dir != None:
		command = "cd "+ run_dir + " && " + command

	command = command + "; pwd;"
	print("running :::", command)

	if dry_run == False:
		result = subprocess.check_call(command, shell=True)
		if 0 != result:
			sys.exit()
	return


def get_args():

	parser = argparse.ArgumentParser()
	parser.add_argument('--config', default="sdk.ini", help='sdk config file')
	parser.add_argument('--base_version', default="hosts/versions/sdk.version", help='sdk base config file')
	parser.add_argument('--output', default="release_output", help='sdk tarball output path')
	parser.add_argument('--boot_media', help='boot_media')
	parser.add_argument('--base_board', help='base_board')
	parser.add_argument('--build_type', help='build_type')
	parser.add_argument('--app', help='single app')
	parser.add_argument('--chip', help='chip name')
	parser.add_argument('--vendor', help='vendor name')
	parser.add_argument('--model', help='product name')
	parser.add_argument('--major_v', help='sdk major ver')
	parser.add_argument('--minor_v', help='sdk minor ver')
	parser.add_argument('--tag', help='image type')
	parser.add_argument('--config_bit', help='32bits or 64bits')

	return parser.parse_args()


def get_base_version(base_file):

	with open(base_file, "r") as fo:
		for line in fo.readlines():
			num  = line.find("gSDK_VERSION")
			if num >= 0:
				base_version = line.split("=")[1]
				return base_version.strip()


def set_base_version(base_file, new_ver):
	file = open(base_file, "a+")
	lines = file.readlines()
	file.truncate(0)

	for line in lines:
		if 'gSDK_VERSION' in line:
			old_ver = line.split("=")[1].strip('\n')
			line = line.replace(old_ver, new_ver)

		file.write(line)

	file.close()

def get_version(conf):

	version_file = conf.get("version", "version_ini").split()[0]
	projects = conf.get("projects", "project_path").split()[0]
	path = "configs/" + projects + "/" + version_file
	fo = open(path, "r")
	version = fo.readline().split("=")[1].strip().strip("\r").strip("\n")
	fo.close()

	return version


def get_sdk_name_prefix(conf):

	prefix = conf.get("sdk_name", "sdk_name_prefix").split()[0]
	return prefix


def copy_all_components(conf, output, boot_media, base_board, build_type, app, chip, config_bit):

	#### 1.make clobber ####
	cmd = "make clobber"
	run_os_command(cmd)

	#### 2.copy components before_build ####
	copy_sdk_components_before_build(conf, output, chip)

	#### 3. build ####
	cmd = "make BOOT_MEDIA=" + boot_media + " BASE_BOARD=" + base_board + " BUILD_TYPE=" + build_type + " APP=" + app + " CHIP=" + chip + " CONFIG_BIT=" + config_bit +" all"
	run_os_command(cmd)

	cmd = "make doxygen"
	run_os_command(cmd)

	#### 4. copy components after build  ####
	copy_sdk_components_after_build(conf, output, chip, config_bit)

	#### 5. copy projects ####
#	prepare_projects()
	copy_projects(conf, output, chip)

	#### 6. remove nonexist menu ####
#	remove_nonexist_menu_config_item(output+"base/arprj/config", output + "base")

	return

def prepare_projects():
	run_os_command("git submodule init; git submodule update")
	return

def copy_projects(conf, output, chip):
	projects = conf.get("projects", "project_path").split()
	print("projects:", projects)

	base_output = output + "/base"
	cmd = "mkdir -p " + base_output + "/arprj"
	run_os_command(cmd)

	files = "hosts/makery hosts/utility hosts/versions configs/common"
	for file in files.split():
		path = os.path.dirname(file)

		copy_path = base_output + "/arprj/" + path
		if os.path.exists(copy_path):
			print("~~~")
		else:
			cmd = "mkdir -p " + copy_path
			run_os_command(cmd)

		cmd = " cp -rf " + file + " " + copy_path
		run_os_command(cmd)

	for project in projects:
		copy_path = base_output + "/arprj/configs/" + chip
		if os.path.exists(copy_path):
			print("~~~")
		else:
			cmd = "mkdir -p " + copy_path
			run_os_command(cmd)
		cmd = "cp -rfL configs/" + chip + "/"+ project + " " + copy_path
		run_os_command(cmd)
		# rm sdk.ini
		cmd = "rm -rf " + base_output + "/arprj/configs/" + chip + "/" + project + "/sdk.ini"
		run_os_command(cmd)

# first copy the components before make any build.
def copy_sdk_components_before_build(conf, output, chip):
	if not conf.has_option("components", "base_components"):
		print("components has't base_components option")
	else:
		base_components = conf.get("components", "base_components")
		print("base_components:", base_components)
		base_output_path = output + "base/"
		cmd = "mkdir -p " + base_output_path
		run_os_command(cmd)
		copy_sdk_components(base_components, base_output_path)
		### other copy
		cmd = "cp ../arsdk/Makefile.param " + base_output_path + "/arsdk"
		run_os_command(cmd)
		cmd = "cp create_img.sh " + base_output_path + "/arprj"
		run_os_command(cmd)

	if not conf.has_option("components", "add_components"):
		print("components has't add_components option")
	else:
		add_components = conf.get("components", "add_components")
		print("add_components:", add_components)
		add_output_path = output + "add_on/"
		cmd = "mkdir -p " + add_output_path
		run_os_command(cmd)
		copy_sdk_components(add_components, add_output_path)
		### other copy
		cmd = "cp Makefile " + add_output_path + "/arprj"
		run_os_command(cmd)

	cmd = "mv Changelog.md " + base_output_path + "/arsdk"
	run_os_command(cmd)

	cmd = "cp ./configs/" + chip + "/common/feature.config " + base_output_path + "/arsdk"
	run_os_command(cmd)

#### out_put = out_put + base or add
def copy_sdk_components(components, output):
	sdk_path = output + "arsdk/"
	cmd = "mkdir -p " + sdk_path
	run_os_command(cmd)

	prj_path = output + "arprj/"
	cmd = "mkdir -p " + prj_path
	run_os_command(cmd)

	for component in components.split():
		if file == "\\":
			continue

		if component.find("source") >= 0:
			copy_source_patch_code_before_build(component, sdk_path)

		elif component.find("makery") >= 0:
			makery_dir =  sdk_path + "platforms/" + component.split("-")[0]
			src = platforms_real_path + component.split("-")[0] + "/Makefile"
			dst = makery_dir
			cp_dir(src, dst)
			src_mods = platforms_real_path + component.split("-")[0] + "/mods"
			if os.path.exists(src_mods):
				cp_dir(src_mods, dst)

		elif component.find("uboot") >= 0 or component.find("kernel") >= 0 or component.find("rootfs") >= 0:
			# uboot/kernel/rootfs -all
			copy_source_patch_code_before_build(component, sdk_path)
			makery_dir =  sdk_path + "platforms/" + component.split("-")[0]
			src = platforms_real_path + component.split("-")[0] + "/Makefile"
			dst = makery_dir
			cp_dir(src, dst)

		elif component.find("kmodule") >= 0:
			if component.find("all") >= 0:
				src = platforms_real_path + "kmodule/"
				dst = sdk_path + "platforms/"
				cp_dir(src, dst)
				cmd = "rm -rf " + sdk_path + "platforms/kmodule/module_config.in " + sdk_path + "platforms/kmodule/module_config"
				run_os_command(cmd)
			else:
				src = platforms_real_path + component.replace("-", "/")
				dst = os.path.dirname(sdk_path + "platforms/" + component.replace("-", "/"))
				cp_dir(src, dst)

		elif component.find("dsp") >= 0:
			src = platforms_real_path + "dsp"
			dst = sdk_path + "platforms"
			cp_dir(src, dst)

		elif component.find("security") >= 0:
			if component.find("all") >= 0:
				src = platforms_real_path + "security/"
				dst = sdk_path + "platforms/"
				cp_dir(src, dst)
				run_os_command(cmd)
			else:
				src = platforms_real_path + component.replace("-", "/")
				dst = os.path.dirname(sdk_path + "platforms/" + component.replace("-", "/"))
				cp_dir(src, dst)

		elif component.find("apps") >= 0:
			if component.find("all") >= 0:
				src = "./apps"
				dst = prj_path
			else:
				src = component.replace("-", "/")
				dst = os.path.dirname(prj_path + component.replace("-", "/"))
			cp_dir(src, dst)
			if os.path.basename(os.path.normpath(output)) == "base":
				cmd = "cp -u " + "apps/Makefile apps/usr_lib_rules.mk " + prj_path + "/apps"
			else:
				cmd1 = "cp -rf " + "apps/libs " + prj_path + "/apps"
				run_os_command(cmd1)
				cmd = "cp -u " + "apps/Makefile apps/usr_lib_rules.mk " + prj_path + "/apps"
			run_os_command(cmd)

		elif component.find("middlewares") >= 0:
			cmd = "cp -u " + sdk_real_path + "middlewares/Makefile " + sdk_real_path + "middlewares/usr_lib_rules.mk " + sdk_path + "/middlewares"
			run_os_command(cmd)
			dst = sdk_path + "middlewares"
			if component.find("all") >= 0:
				 continue
			elif component.find("mpp") >= 0:
				src = sdk_real_path + "middlewares/mpp"
				cp_dir(src, dst)
				src = sdk_real_path + "middlewares/hal"
				cp_dir(src, dst)
			elif component.find("hal") >= 0:
				src = sdk_real_path + "middlewares/hal"
				cp_dir(src, dst)
			cmd = "cp " +  sdk_real_path + "middlewares/Makefile " +  sdk_real_path + "middlewares/usr_lib_rules.mk " + dst
			run_os_command(cmd)
			cmd = "rm -rf " + sdk_path + "middlewares/hal/hal_config.in " + sdk_path + "middlewares/hal/hal_config"
			run_os_command(cmd)
			cmd = "rm -rf " + sdk_path + "middlewares/hal/unitests/test_config.in " + sdk_path + "middlewares/hal/unitests/test_config"
			run_os_command(cmd)

def cp_dir(src, dst):
	if os.path.exists(dst):
		print("~~~")
	else:
		cmd = "mkdir -p " + dst
		run_os_command(cmd)

	cmd = "cp -rf " + src + " " + dst
	run_os_command(cmd)

def mv_file(src, dst):
	if os.path.exists(dst):
		print("~~~")
	else:
		cmd = "mkdir -p " + dst
		run_os_command(cmd)

	cmd = "mv " + src + " " + dst
	run_os_command(cmd)

def copy_source_patch_code_before_build(component, output):
	component_name = component.split("-")[0]
	fin_output = output + "/platforms/" + component_name
	if component.find("rootfs") >= 0:
		cmd = "make -C " + platforms_real_path + component.split("-")[0] +" busybox_prepare IS_KEEP_TAR=true"
		run_os_command(cmd)
		src = platforms_real_path + component.split("-")[0] + "/skel"
		dst = fin_output
		cp_dir(src, dst)
		src = platforms_real_path + component.split("-")[0] + "/sysroot"
		dst = fin_output
		cp_dir(src, dst)
		src = platforms_real_path + component.split("-")[0] + "/busybox/" + busybox_tar_file
		dst = fin_output + "/busybox"
		mv_file(src, dst)

	elif component.find("uboot") >= 0:
		cmd = "make -C " + platforms_real_path + component.split("-")[0] +" prepare IS_KEEP_TAR=true"
		run_os_command(cmd)
		src = platforms_real_path + component.split("-")[0] + "/" + uboot_tar_file
		dst = fin_output
		mv_file(src, dst)

	elif component.find("kernel") >= 0:
		cmd = "make -C " + platforms_real_path + component.split("-")[0] +" prepare IS_KEEP_TAR=true"
		run_os_command(cmd)
		src = platforms_real_path + component.split("-")[0] + "/" + kernel_tar_file
		dst = fin_output
		mv_file(src, dst)

	elif component.find("kmodule") >= 0:
		src = platforms_real_path + component.split("-")[0]
		dst = os.path.dirname(fin_output)
		cp_dir(src, dst)
		cmd = "rm -f " + fin_output + "/module_config*"
		run_os_command(cmd)
		cmd = "rm -f " + fin_output + "/Makefile"
		run_os_command(cmd)


def copy_sdk_components_after_build(conf, output, chip, config_bit):
	if not conf.has_option("components", "base_components"):
		print("components has't base_components option")
	else:
		base_components = conf.get("components", "base_components")
		print("base_components:", base_components)

	if not conf.has_option("components", "base_components"):
		print("components has't base_components option")
	else:
		add_components = conf.get("components", "add_components")
		print("add_components:", add_components)

	if not conf.has_option("components", "spec_components"):
		spec_components = ''
		print("components has't spec_components option")
	else:
		spec_components = conf.get("components", "spec_components")
		print("spec_components:", spec_components)

	base_output_path = output + "base"
	add_output_path = output + "add_on"
	#1. cppy output
	cmd = "cp -rf release " + base_output_path + "/arprj"
	run_os_command(cmd)
	cmd = "rm -rf " + base_output_path + "/arprj/release/prebuilds/image"
	run_os_command(cmd)

	#2. check base_components source or output
	for component in base_components.split():
		if file == "\\":
			continue
		remove_out(component, base_output_path)
		# hal core process alone
#		if component.find("hal") >= 0:
#			procese_core(base_output_path)

	#3. check add_components source or output
	for component in add_components.split():
		if file == "\\":
			continue
		remove_out(component, base_output_path)
		# hal core process alone
#		if component.find("hal") >= 0:
#			procese_core(add_output_path)

#	procese_close_components(conf, base_output_path)
#	procese_close_components(conf, add_output_path)

	remove_file_content(add_output_path+"/arprj/Makefile", "#### for internal use start ####", "#### for internal use end ####")
	remove_file_content(add_output_path+"/arprj/Makefile", "clobber:clean", '')
	remove_file_content(add_output_path+"/arprj/Makefile", "##### internal clobber ####", "fi")

	for line in fileinput.input(add_output_path+"/arprj/Makefile", inplace=1):
		if "export CHIP" in line:
			default_chip = line.partition("?=")[2].strip(' ')
			line = line.replace(default_chip, chip)

		print(line.strip('\n'))

	for line in fileinput.input(base_output_path+"/arsdk/Makefile.param", inplace=1):
		if "export CONFIG_BIT" in line:
			default_config_bit = line.partition("?=")[2].strip(' ')
			line = line.replace(default_config_bit, config_bit)

		print(line.strip('\n'))

	#4. handle spec_components
	for component in spec_components.split():
		if component.find("kmodule") >= 0:
			src = platforms_real_path + component.replace("-", "/")
			dst = base_output_path + "/arsdk/platforms/" + component.split("-")[0]
			cp_dir(src, dst)
			cmd = "rm -rf *.c *.ko .*.cmd .tmp_versions Module.symvers modules.order"
			run_os_command(cmd, base_output_path + "/arsdk/platforms/" + component.replace("-", "/"))

	return

def remove_out(component, output):

		if component.find("makery") >= 0:
			return

		elif component.find("source") >= 0 or component.find("kernel-all") >= 0 or component.find("uboot-all") >= 0 :
			if component.find("rootfs") >= 0:
				cmd = "rm -rf " + output + "/arprj/release/prebuilds/rootfs_skel"
				run_os_command(cmd)
				cmd = "rm -rf " + output + "/arprj/release/prebuilds/sysroot"
				run_os_command(cmd)
				cmd = "rm -rf " + output + "/arprj/release/prebuilds/busybox"
				run_os_command(cmd)
				cmd = "rm -rf " + output + "/arprj/release/prebuilds/fs"
				run_os_command(cmd)
			elif component.find("kmodule") >= 0:
				cmd = "rm -rf " + output + "/arprj/release/prebuilds/ko"
				run_os_command(cmd)
				cmd = "rm -rf " + output + "/arprj/release/out/kmodule"
				run_os_command(cmd)
			else:
				cmd = "rm -rf " + output + "/arprj/release/prebuilds/" + component.split("-")[0]
				run_os_command(cmd)

		elif component.find("kmodule") >= 0:
			if component.find("all") >= 0:
				cmd = "rm -rf " + output + "/arprj/release/out/kmodule"
				run_os_command(cmd)
				cmd = "rm -rf " + output + "/arprj/release/prebuilds/ko"
				run_os_command(cmd)
			else:
				cmd = "rm -rf " + output + "/arprj/release/prebuilds/ko/" + component.split("-")[1] + ".ko"
				run_os_command(cmd)

		elif component.find("middlewares") >= 0:
			if component.find("all") >= 0:
				return
#				cmd = "rm -rf " + output + "/arprj/release/out/middlewares/inc"
#				run_os_command(cmd)
#				cmd = "rm -rf " + output + "/arprj/release/out/hal/inc"
#				run_os_command(cmd)
			elif component.find("mpp") >= 0:
				cmd = "rm -rf " + output + "/arprj/release/out/hal"
				run_os_command(cmd)
				cmd = "rm -rf " + output + "/arprj/release/out/mpp"
				run_os_command(cmd)
			elif component.find("hal") >= 0:
				cmd = "rm -rf " + output + "/arprj/release/out/hal"
				run_os_command(cmd)
				mpp_source_dir = output + "/arsdk/middlewares/mpp"
				mpp_out_dir = output + "/arprj/release/out/mpp"
				mpp_sample_dir = output + "/arprj/apps/verify/mpp_sample"
				app_out_dir = output + "/arprj/release/prebuilds/apps"
				print("mpp_source_dir:", mpp_source_dir)
				if os.path.exists(mpp_source_dir):
					print("mpp_source_dir:", mpp_source_dir)
					cmd = " rm -rf " + mpp_source_dir
					run_os_command(cmd)
				if os.path.exists(mpp_out_dir):
					cmd = " rm -rf " + mpp_out_dir
					run_os_command(cmd)
				if os.path.exists(mpp_sample_dir):
					cmd = " rm -rf " + mpp_sample_dir
					run_os_command(cmd)
					cmd = " rm -rf " + app_out_dir + "/bin/test_mpp_*"
					run_os_command(cmd)
					if not os.listdir(output + "/arprj/apps/verify"):
						cmd = " rm -rf " + output + "/arprj/apps/verify"
						run_os_command(cmd)
			src = "release/out/kmodule"
			dst = output + "/arprj/release/out"
			cp_dir(src, dst)

		elif component.find("apps") >= 0:
			if not component.find("all") >= 0:
				app_file = output + "/arprj/release/prebuilds/apps/bin/" + component.split("-")[1]
				if os.path.exists(app_file):
					cmd = "rm -f " + app_file
					run_os_command(cmd)
			cmd = "rm -rf " + output + "/arprj/release/prebuilds/apps/inc"
			run_os_command(cmd)
			cmd = "rm -rf " + output + "/arprj/release/prebuilds/apps/lib"
			run_os_command(cmd)

def procese_core(output):
	mpp_source_path = "../arsdk/middlewares/hal/modules/core/"
	copy_path = output + "/arsdk/middlewares/hal/modules/core/"

	cmd = " rm -rf " + copy_path
	run_os_command(cmd)

	if os.path.exists(copy_path):
		print("~~~")
	else:
		cmd = "mkdir -p " + copy_path
		run_os_command(cmd)

	cmd = "cp -rf " + mpp_source_path + "Makefile " + copy_path
	run_os_command(cmd)
	cmd = "cp -rf " + mpp_source_path + "apps " + copy_path + "apps"
	run_os_command(cmd)
	cmd = "cp -rf " + mpp_source_path + "build " + copy_path + "build"
	run_os_command(cmd)
	cmd = "cp -rf " + mpp_source_path + "out " + copy_path + "out"
	run_os_command(cmd)
	cmd = "mkdir -p " + copy_path + "common/"
	run_os_command(cmd)
	cmd = "cp -rf " + mpp_source_path + "common/sub_include_all.mk " + copy_path + "common/"
	run_os_command(cmd)
	cmd = "cp -rf " + copy_path + "out/staging/* " + copy_path
	run_os_command(cmd)
	cmd = "rm  -rf " + copy_path + "out/staging "
	run_os_command(cmd)
	cmd = "find " + copy_path + " -name *.o | xargs rm -rf"
	run_os_command(cmd)
	cmd = "find " + copy_path + " -name *.d | xargs rm -rf"
	run_os_command(cmd)

def procese_close_components(conf, output):
	if not conf.has_option("components", "close_components"):
		print("components has't close_components option")
		return

	close_components = conf.get("components", "close_components")
	print("close_components:", close_components)

	if close_components.find("middlewares-mpp") >= 0:
		mpp_source_dir = output + "/arsdk/middlewares/mpp"
		mpp_out_dir = output + "/arprj/release/out/mpp"
		mpp_sample_dir = output + "/arprj/apps/verify/mpp_sample"
		app_out_dir = output + "/arprj/release/prebuilds/apps"
		print("mpp_source_dir:", mpp_source_dir)
		if os.path.exists(mpp_source_dir):
			print("mpp_source_dir:", mpp_source_dir)
			cmd = " rm -rf " + mpp_source_dir
			run_os_command(cmd)
		if os.path.exists(mpp_out_dir):
			cmd = " rm -rf " + mpp_out_dir
			run_os_command(cmd)
		if os.path.exists(mpp_sample_dir):
			cmd = " rm -rf " + mpp_sample_dir
			run_os_command(cmd)
			cmd = " rm -rf " + app_out_dir + "/bin/test_mpp_*"
			run_os_command(cmd)

# remove non-exist config
def remove_nonexist_menu_config_item(file, output):
	# read line and check the file exist
	print("file:", file)
	with open(file, "r") as old:
		new = open(file + ".tmp", "w+")
		print ("new file:: ", file + ".tmp")

		for lines in old.readlines():
			source = output + "arprj/" + lines.split()[1].strip()
			print("source:", source)
			if os.path.isfile(source):
				new.writelines(lines)
			else:
				print(source, "non-exist, remove")

	new.close()

	# use new config files
	cmd = "mv " + file + ".tmp " + file
	run_os_command(cmd)

	#goto trigger the menuconfig, remove build config in base/menu.config
#	cmd = "make config"
#	run_os_command(cmd, output + "arsdk")

	return

def remove_file_content(file, start, end):
	print("file:", file)
	with open(file, "r") as old:
		new = open(file + ".tmp", "w+")
		print ("new file:: ", file + ".tmp")

		del_flag = False
		for line in old.readlines():
			if line.find(start) >= 0:
				del_flag = True

			if not del_flag:
				new.writelines(line)

			if end == '':
				del_flag = False
			elif line.find(end) >= 0:
				del_flag = False

	new.close()

	# use new files
	cmd = "mv " + file + ".tmp " + file
	run_os_command(cmd)

def replace_file_content(file, dict):
	print("file:", file)
	with open(file, "r") as old:
		new = open(file + ".tmp", "w+")
		print ("new file:: ", file + ".tmp")

		for line in old.readlines():
			for key,value in dict.items():
				if line.find(key) >= 0:
					line = value
					break
			new.writelines(line)

	new.close()

	# use new files
	cmd = "mv " + file + ".tmp " + file
	run_os_command(cmd)

def create_sdk_version_file(chip, version):
	if os.path.exists("sdk.version"):
		os.remove("sdk.version")

	f = open("sdk.version", 'w')
	f.write(version + '\n')
	f.close()
	cmd = "mv sdk.version ../arsdk/platforms/rootfs/skel/nodes/etc/"
	run_os_command(cmd)
	cmd = "cp Changelog.md ../arsdk/platforms/rootfs/skel/nodes/etc/"
	run_os_command(cmd)

	cmd = "cp ./configs/" + chip + "/common/feature.config ../arsdk/platforms/rootfs/skel/nodes/etc/"
	run_os_command(cmd)

def validate_empty_args(args):
	if args.vendor  == '':
		return False, 'VENDOR_NAME'
	if args.model   == '':
		return False, 'MODEL'
	if args.major_v == '':
		return False, 'MAJOR_V'
	if args.minor_v == '':
		return False, 'MINOR_V'
	if args.tag	== '':
		return False, 'TAG'
	return True, ''

def main():

	# get current path
	cur_path = os.path.abspath("./") + "/"

	# parrse args
	args = get_args()
	result, keyword = validate_empty_args(args)
	if result == False:
		sys.stderr.write("\n ERROR - Missing input parameter for %s\n" % keyword)
		sys.exit(1)

	config_bit = args.config_bit
	output = args.output + "_" + config_bit + "/"
	run_os_command("mkdir -p " + output)

	boot_media = args.boot_media
	base_board = args.base_board
	build_type = args.build_type
	app = args.app
	chip = args.chip
	vendor = args.vendor
	model = args.model
	tag = args.tag
	print("====boot_media",boot_media)
	print("====app",app)
	print("====chip",chip)
	print("====vendor",vendor)
	print("====model",model)
	print("====tag",tag)
	print("====config_bit ",config_bit)

	# parrse ini file
	conf = ConfigParser.ConfigParser()
	conf.readfp(open(args.config))

	version = args.major_v + "-" + args.minor_v
	set_base_version(args.base_version, version)   # update sdk version to input version

	prefix  = chip + "-" + vendor + "-" + model + "-"
	create_sdk_version_file(chip, prefix + version)
	suffix  = "-" + tag + "-" + time.strftime("%Y%m%d")

#	tar_dir =  os.path.abspath(output) + "/tmp/sdk_release" + str(os.getpid()) + "/"
	tar_dir =  os.path.abspath(output) + "/tmp/sdk_release" + "/"
	tmppath = tar_dir + prefix + version + "_" + config_bit + suffix + "/"

	print("tmppath:", tmppath, tar_dir)
	run_os_command("mkdir -p " + tmppath)

	# make and mv bases to tmppath
	copy_all_components(conf, tmppath, boot_media, base_board, build_type, app, chip, config_bit)

	# cd to tar_dir and tar
	soure_tar_dir = tar_dir + prefix + version + "_" + config_bit + suffix

	setup_file = soure_tar_dir + "/set_up.sh"
	if not os.path.exists(setup_file):
		f = open(setup_file, 'w')
		f.write('#! /bin/bash\n')
		f.write('cp -ur add_on/* base\n')
		f.close()
		cmd = "chmod 774 " + setup_file
		run_os_command(cmd)

	if os.path.exists(soure_tar_dir + "/base"):
		base_sdkname ="base.tar.gz"
		cmd = "cd " + soure_tar_dir + " && " + "tar -czvf " + base_sdkname + " base/"
		run_os_command(cmd)
		cmd = "rm -rf " + soure_tar_dir + "/base"
		run_os_command(cmd)

	if os.path.exists(soure_tar_dir + "/add_on"):
		add_sdkname ="add_on.tar.gz"
		cmd = "cd " + soure_tar_dir + " && " + "tar -czvf " + add_sdkname + " add_on/"
		run_os_command(cmd)
		cmd = "rm -rf " + soure_tar_dir + "/add_on"
		run_os_command(cmd)

	sdkname = prefix + version + "_" + config_bit + suffix
	cmd = "cd " + tar_dir + " && " + "tar -czvf " + cur_path + output + sdkname + ".tar.gz" + " " + sdkname + "/"
	run_os_command(cmd)

	run_os_command("rm -rf " + cur_path + output + "tmp")

if __name__ == "__main__":
	main()

