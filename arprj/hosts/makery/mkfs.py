#!/usr/bin/env python
# -*- coding:utf8 -*-
import argparse
import os
import struct
import subprocess
import json
import ConfigParser

def get_args():
	parser = argparse.ArgumentParser()

	parser.add_argument('--fs_config_file', \
		help='the json file for fs config info')

	parser.add_argument('--command', \
		default="mk_fs_img", \
		help='command: get_fs_list get_component_list mk_fs_img')

	parser.add_argument('--flash_layout_file', \
		help='image init file')

	parser.add_argument('--output_dir', \
		help='output dir of fs img ')

	parser.add_argument('--board_config_dir', \
		help='board config dir used to replace string <BOARD_CONFIG_DIR> ')

	parser.add_argument('--keep_tmp_dir', \
		default="false", \
		help='whether keep the temporary dir')

	parser.add_argument('--mpp_service', \
		default="app", \
		help='mpp_service type')

	parser.add_argument('--uboot_env_file', \
		default="", \
		help='uboot env file path')

	parser.add_argument('--security_cfg_file', \
		default="", \
		help='configuration file for security')

	parser.add_argument('--build_type', \
		default="debug", \
		help='image with or without debug info')

	return parser.parse_args()

# load and format fs_img.json config
def load_fs_config(file_name):
	with open(file_name, 'r') as f:
		data = json.load(f)
	return data


# print out img
def output_fs_img_list(fs_dict):
	fs_list = list(fs_dict)
	for i in fs_list:
		print(i)

def extract(src, dst, strip_components=0):
	if not os.path.isdir(dst):
		print("There is no dst:", dst)
		return False
	if '.' in src:
		suffix = src.split('.')[-1]
		print("suffix:", suffix);
		if 'tar' == suffix:
			cmd = "tar -x -v -f " + src + " -C " + dst
		elif 'gz' == suffix:
			cmd = "tar -x -v -z -f " + src + " -C " + dst
		elif 'bz2' == suffix:
			cmd = "tar -x -v -j -f " + src + " -C " + dst
		elif '7z' == suffix or '7zip' == suffix:
			suffix1 = src.split('.')[-2]
			if 'tar' == suffix1:
				cmd = "7zr x -so " + src + " | " + "tar -x -v -f -C " + dst
			else:
				cmd = "7zr x " + src + " -o" + dst
		elif 'lzma' == suffix:
			cmd = "tar -x -v --lzma -f " + src + " -C " + dst
		elif 'lz' == suffix:
			cmd = "tar -x -v --lzip -f " + src + " -C " + dst
		elif 'tlz' == suffix:
			cmd = "tar -x -v --lzma -f " + src + " -C " + dst
		elif 'lzo' == suffix or 'tzo' == suffix:
			cmd = "tar -x -v --lzop -f " + src + " -C " + dst
		elif 'xz' == suffix:
			cmd = "tar -x -v --xz -f " + src + " -C " + dst
		elif 'Z' == suffix:
			cmd = "tar -x -v -Z -f " + src + " -C " + dst
		elif 'tgz' == suffix:
			cmd = "tar -x -v -z -f " + src + " -C " + dst
		elif 'tbz2' == suffix or 'tbz' == suffix:
			cmd = "tar -x -v -j -f " + src + " -C " + dst
		elif 'zip' == suffix:
			cmd = "unzip -o " + src + " -d " + dst
		else:
			return False
		# 仅对 tar 家族命令追加 --strip-components，剥离升级包中多余的挂载点前缀
		# (如从 / 打包的 usrdata/... 路径)，避免出现 /usrdata/usrdata/ 嵌套
		if strip_components > 0 and cmd.startswith("tar "):
			cmd = cmd + " --strip-components=" + str(strip_components)
		print("extract cmd:", cmd)
		result = subprocess.check_call(cmd, shell=True)
		if 0 != result:
			return False
	else:
		print("Wrong file format")
		return False

	return True

# combine components
def combine_fs_source(fs_dict_i, out_fs_dir, board_config_dir, mpp_service, build_type):
	fs_raw_list = []
	fs_component_list = []

	if "component_list" in fs_dict_i:
		fs_component_list = fs_dict_i["component_list"]

	if "debug_component" in fs_dict_i:
		if "debug" in build_type:
			fs_component_list = fs_component_list + fs_dict_i["debug_component"]

	if "raw_list" in fs_dict_i:
		fs_raw_list = fs_dict_i["raw_list"]

	#创建临时目录
	if not os.path.exists(out_fs_dir):
		os.makedirs(out_fs_dir)

	#拷贝源文件到临时目录的目标路径，目标路径不存在则创建
	if fs_component_list:
		for i in fs_component_list:
			if "<BOARD_CONFIG_DIR>" in i.split(':')[0]:
				src = board_config_dir + (i.split(':')[0]).strip("<BOARD_CONFIG_DIR>")
			else:
				src = './release/' + i.split(':')[0]
			dst = out_fs_dir + '/' + i.split(':')[1]
			if not os.path.exists(dst):
				subprocess.check_call("mkdir -p " + dst, shell=True)
			if "*" in src:
				result = subprocess.check_call("cp -rf " + src + " " + dst, shell=True)
			elif os.path.isdir(src):
				result = subprocess.check_call("cp -rf " + src + "/* " + dst, shell=True)
			elif os.path.isfile(src):
				if not extract(src, dst):
					result = subprocess.check_call("cp -rf " + src + " " + dst, shell=True)
			else:
					result = subprocess.check_call("cp -rf " + src + " " + dst, shell=True)

	if mpp_service != "":
		start_sh = out_fs_dir + "/etc/init.d/start.sh"
		start_sh_bk = start_sh + ".bk"
		if os.path.exists(start_sh):
			with open(start_sh, "r") as f, open(start_sh_bk, "w") as fw:
				for line in f:
					if "insmod /mod/ar_sys.ko" in line and "mpp_service" not in line:
						line = line.strip("\r\n") + " mpp_service=" + mpp_service + "\n"
					fw.write(line)
			subprocess.check_call("mv " + start_sh_bk + " " + start_sh, shell=True)
			subprocess.check_call("chmod +x " + start_sh, shell=True)
	return out_fs_dir

def get_part_size(fs_img_name, image_ini_file):
	cmd = "grep -rn " + fs_img_name + ". " +  image_ini_file + " | cut -d : -f1"
	tmp = subprocess.check_output(cmd, shell=True)
	if tmp == "":
		return 0
	line = int(tmp.split('\n')[0], base =10) + 1
	print("size line:", line)
	cmd = "sed -n " + str(line) + "p " + image_ini_file + " | grep 'size' | cut -d = -f2"
	partsize_str = subprocess.check_output(cmd, shell=True)
	if partsize_str == "":
		return 0
	partsize_str = partsize_str.split('\n')[0]
	partsize = int(partsize_str, base =16)
	print("partsize:", partsize)
	return partsize

def get_fs_format(fs_img_name, image_ini_file):
	cmd = "grep -rn " + fs_img_name + ". " +  image_ini_file + " | cut -d . -f2"
	tmp = subprocess.check_output(cmd, shell=True)
	if tmp == "":
		return tmp
	fs_format = tmp.split('\n')[0]
	print("fs_format:", fs_format)
	return fs_format

def creat_fs_image(fs_img_name, image_ini_file, src_dir, out_dir, uboot_env_file, security_cfg_file):
	print("fs_img_name:", fs_img_name)
	mkfs_format = get_fs_format(fs_img_name, image_ini_file)
	if mkfs_format == "":
		return
	out_file = out_dir + "/" + fs_img_name + "." + mkfs_format
	if mkfs_format == "ext2" or mkfs_format == "ext3" or mkfs_format == "ext4":
		# 文件系统格式为ext2/ext3/ext4,步骤如下：
		#1.获取文件系统镜像大小，并计算得到partsize
		partsize = get_part_size(fs_img_name, image_ini_file) / 0x100000
		if partsize == 0:
			return
		# 优先使用 make_ext4fs；旧版二进制对某些内容(如多文件、含升级包)会触发
		# "do_inode_allocate_indirect: Failed to allocate N blocks" bug，此时
		# 自动回退到系统 mke2fs -d 完成 ext4 镜像生成。
		made = False
		try:
			#2.使用make_ext4fs工具生成simg: $make_ext4fs -l partsizeM -s -a system out_dir/system.ext4img
			subprocess.check_call("./hosts/utility/bin/make_ext4fs" + " -l "+ str(partsize) + "M -s -a system " + out_dir + "/system.ext4img " + src_dir, shell=True)
			#3.使用simg2img工具转换为img: $simg2img out_dir/system.ext4img out_file
			subprocess.check_call("./hosts/utility/bin/simg2img " + out_dir + "/system.ext4img " + out_file, shell=True)
			#4.使用truncimg工具处理，用于截断img末尾为0的部分: $truncimg out_file
			subprocess.check_call("./hosts/utility/bin/truncimg " + out_file, shell=True)
			subprocess.check_call("rm -rf " + out_dir + "/system.ext4img ", shell=True)
			made = True
		except subprocess.CalledProcessError as e:
			print("[creat_fs_image] make_ext4fs pipeline failed (%s), fallback to mke2fs -d" % str(e))
			subprocess.check_call("rm -rf " + out_dir + "/system.ext4img " + out_file, shell=True)
		if not made:
			# 回退方案: 用系统 mke2fs -d 直接生成 raw ext4 镜像, 再用 truncimg 打孔
			# mke2fs 选项: -t ext4 指定类型, -b 4096 块大小, -L '' 清空标签,
			# -E lazy_itable_init=0 立即初始化inode表, -d 从目录填充
			mke2fs_cmd = "mke2fs -t ext4 -b 4096 -L '' -E lazy_itable_init=0 -d " + src_dir + " " + out_file + " " + str(partsize) + "M"
			subprocess.check_call(mke2fs_cmd, shell=True)
			subprocess.check_call("./hosts/utility/bin/truncimg " + out_file, shell=True)
	elif mkfs_format == "jffs2":
		# 文件系统格式为jffs2生成镜像命令：$mkfs.jffs2 --root=src_dir -o out_file & sync
		subprocess.check_call("mkfs.jffs2 " + " --root="+ src_dir + " -o " + out_file, shell=True)
		subprocess.check_call("sync", shell=True)
	elif mkfs_format == "cramfs":
		# no mkcramfs tool(暂时未用)
		subprocess.check_call("mkcramfs " + " "+ src_dir + " " + out_file, shell=True)
		subprocess.check_call("sync", shell=True)
	elif mkfs_format == "squashfs":
		# 文件系统格式为squashfs生成镜像命令：$mksquashfs src_dir out_file -noappend -no-progress -comp lzo & sync
		subprocess.check_call("mksquashfs "+ src_dir + " " + out_file + " -noappend -no-progress -comp lzo", shell=True)
                security_config = ConfigParser.ConfigParser(allow_no_value=True)
                security_config.read(security_cfg_file)
                if security_config.get('MAKEFILE', 'dm_verity') == 'y':
		    subprocess.check_call("./hosts/makery/dm_verity_fs.sh " + out_file + " " + uboot_env_file, shell=True)
		subprocess.check_call("sync", shell=True)
	elif mkfs_format == "ubifs":
		# 文件系统格式为ubifs,步骤如下：
		#1.获取block size
		cmd = "grep -rn block_size " + image_ini_file + " | cut -d = -f2"
		blocksize_str = subprocess.check_output(cmd, shell=True)
		print("blocksize_str:", blocksize_str)

		block_size = int(blocksize_str.split('\n')[0], base =16)
		print("block_size:", block_size)

		#2.获取page size
		cmd = "grep -rn page_size " + image_ini_file + " | cut -d = -f2"
		pagesize_str = subprocess.check_output(cmd, shell=True)
		page_size = int(pagesize_str.split('\n')[0], base =10)
		print("page_size:", page_size)

		#3.计算最大逻辑擦除块数量
		max_leb_cnt = get_part_size(fs_img_name, image_ini_file) / block_size
		print("ubifs maximum leb count= ", max_leb_cnt)
		if max_leb_cnt == 0:
			return

		#4.计算逻辑擦除块大小
		leb_size = block_size - 2*page_size
		print("leb_size:", leb_size)

		subprocess.check_call("mkfs.ubifs " + "-m " + str(page_size) + " -e " + str(leb_size) + " -F -c "+ str(max_leb_cnt) + " -r " + src_dir + " -o " + out_file, shell=True)
		subprocess.check_call("mkfs.ubifs " + "-m " + str(page_size) + " -e " + str(leb_size) + " -c "+ str(max_leb_cnt) + " -r " + src_dir + " -o " + out_file + ".nofixup", shell=True)
		subprocess.check_call("sync", shell=True)
	elif mkfs_format == "xfs":
		# 暂时未用
		tmp_tar_file = src_dir +"/../"+ fs_img_name + ".tar "
		subprocess.check_call("tar -cvf " + tmp_tar_file + " " + src_dir, shell=True)
		subprocess.check_call("mkdir -p " + out_dir +"/mnt", shell=True)
		print("Determine filesystem metadata overhead...")
		subprocess.check_call("dd if=/dev/zero of=" + out_file + " bs=1M count=0 seek=`du --block-size=1048576 " + tmp_tar_file +" | cut -f 1` > /dev/null", shell=True)
		subprocess.check_call("mkfs.xfs " + out_file + " > /dev/null", shell=True);
		metadata_overhead = subprocess.check_call("du --block-size=1048576 "+src_dir+" | cut -f 1", shell=True)
		print("metadata_overhead=", metadata_overhead)
		tarfile_size = subprocess.check_call("du --block-size=1048576 "+out_file+" | cut -f 1", shell=True)
		print("tarfile_size=", tarfile_size)
		fs_size = (((metadata_overhead + tarfile_size)*1.05)*1024)/1024
		print("fs_size=", fs_size)
		subprocess.check_call("dd if=/dev/zero of=" + out_file +" bs=1M count=" + fs_size +" oflag=sync", shell=True)
		subprocess.check_call("mkfs.xfs " + out_file +" > /dev/null", shell=True)
		print("Loopback mounting filesystem")
		subprocess.check_call("sudo mount -o loop " + out_file + " "+ out_dir +"/mnt", shell=True)
		print("Copying rootfs files into mounted XFS file")
		subprocess.check_call("sudo cp -rf " + src_dir + "/* " + out_dir  + "/mnt", shell=True)
		subprocess.check_call("sudo umount -d " + out_dir + "/mnt", shell=True)
		subprocess.check_call("sync", shell=True)


def create_crypt_tool(image_ini_file, security_cfg_file, rootfs_dir):
	subprocess.check_call("./hosts/makery/dm_crypt_cfg.sh " + security_cfg_file + " " + image_ini_file + " " + rootfs_dir, shell=True)


# 分区升级包配置表: (fs_name 关键字, 环境变量名, 默认相对 SDK 根的升级包文件名, 分区挂载点前缀)
# fs_name 中包含关键字即视为匹配；环境变量为空或文件不存在则跳过该分区的升级
# mount_prefix 用于检测升级包是否从根目录打包(路径含 <mount_prefix>/...)，若是则剥离该前缀
#   - rootfs 挂载在 /，无前缀，无需剥离
#   - usr_data 挂载在 /usrdata，若 tar 内路径以 usrdata/ 开头则剥离一层
FS_UPGRADE_SPECS = [
	('rootfs',   'ROOTFS_UPGRADE_TAR',   'cr10_root_fs_upgrade.tar',   ''),
	('usr_data', 'USERDATA_UPGRADE_TAR', 'cr10_usrdata_fs_upgrade.tar', 'usrdata'),
]

# 读取 tar 包第一个有效成员的路径(去除开头的 './' 与 '/')，无法读取时返回 None
def _tar_first_member(tar_path):
	try:
		out = subprocess.check_output("tar -tf " + tar_path + " 2>/dev/null", shell=True)
		for line in out.splitlines():
			name = line.strip()
			if not name:
				continue
			if name.startswith('./'):
				name = name[2:]
			if not name or name == '.':
				continue
			return name
		return None
	except subprocess.CalledProcessError:
		return None
	except Exception:
		return None

# 用升级 tar 覆盖更新已合并的 fs 临时目录
# 通过对应环境变量(见 FS_UPGRADE_SPECS)指定升级包路径
def apply_fs_upgrade(fs_name, out_combine_tmp_dir):
	for keyword, env_var, default_name, mount_prefix in FS_UPGRADE_SPECS:
		if keyword not in fs_name:
			continue
		upgrade_tar = os.environ.get(env_var, '').strip()
		if upgrade_tar == '':
			continue
		if not os.path.isfile(upgrade_tar):
			print("[apply_fs_upgrade] WARNING: %s not found: %s" % (env_var, upgrade_tar))
			continue
		print("[apply_fs_upgrade] applying '%s' -> '%s'" % (upgrade_tar, out_combine_tmp_dir))
		# 检测升级包是否从根目录打包(第一个成员路径以 <挂载点>/ 开头)，
		# 若是则剥离该前缀，避免出现 /usrdata/usrdata/ 这种嵌套路径
		strip_components = 0
		if mount_prefix:
			first = _tar_first_member(upgrade_tar)
			if first:
				first_norm = first.lstrip('/')
				prefix_norm = mount_prefix.lstrip('/') + '/'
				if first_norm.startswith(prefix_norm):
					strip_components = 1
					print("[apply_fs_upgrade] detected mount prefix '%s' in tar, strip 1 component" % mount_prefix)
		# 复用脚本内已有的 extract() 逻辑支持 tar/tar.gz/tgz 等多种格式
		if not extract(upgrade_tar, out_combine_tmp_dir, strip_components):
			raise RuntimeError("[apply_fs_upgrade] extract failed: " + upgrade_tar)


# 循环取出fs配置文件中描述文件系统的信息，并生成flash_layout配置文件中描述格式和大小的镜像文件
def mk_fs_images(fs_dict, sdk_out_dir, image_ini_file, is_keep_tmp_dir, board_config_dir, mpp_service, uboot_env_file, security_cfg_file, build_type):
	for i in list(fs_dict):
	        out_combine_tmp_dir = sdk_out_dir + '/tmp/' + i
		# 0.生成分区加密所需的脚本和key
                create_crypt_tool(image_ini_file, security_cfg_file, out_combine_tmp_dir)
		# 1.拷贝component_list raw_list中描述的源文件或目录到目标文件或目录，并返回合并后的临时目录
		combine_fs_source(fs_dict[i], out_combine_tmp_dir, board_config_dir, mpp_service, build_type)
		# 1.5(可选)用升级 tar 覆盖更新 fs 内容，再制作映像
		apply_fs_upgrade(i, out_combine_tmp_dir)
		fsimg_out_dir = sdk_out_dir
		# 2.将合并后的临时目录生成镜像文件，文件系统镜像格式和大小由flash_layout配置文件提供
		creat_fs_image(i, image_ini_file, out_combine_tmp_dir, fsimg_out_dir, uboot_env_file, security_cfg_file)
		# 3.判断是否保留合并后的临时目录
		if is_keep_tmp_dir == "false":
			if os.path.exists(fsimg_out_dir + "/tmp"):
				subprocess.check_call("rm -rf " + fsimg_out_dir + "/tmp", shell=True)

def main():
	# parse args
	args = get_args()
	# fs打包配置文件(主要描述文件系统名及对应的需打包内容)
	config_file = args.fs_config_file
	# flash_layout配置文件(主要描述文件系统类型及大小)
	image_ini_file = args.flash_layout_file
	# 文件系统镜像输出路径
	output_dir = args.output_dir
	# 是否保留生成镜像前的合并文件夹参数，默认不保留
	keep_tmp_dir = args.keep_tmp_dir
	command = args.command
	board_config_dir = args.board_config_dir
	fs_dict = load_fs_config(config_file)
	mpp_service = args.mpp_service
	uboot_env_file = args.uboot_env_file
	security_cfg_file = args.security_cfg_file
	build_type = args.build_type

	if not os.path.exists(output_dir):
		cmd = "mkdir -p " + output_dir
		subprocess.check_call(cmd, shell=True)

	if command == "get_fs_list":
		#列出fs配置文件中描述的文件系统镜像列表
		output_fs_img_list(fs_dict)
	elif command == "mk_fs_img":
		#生成文件系统镜像
		mk_fs_images(fs_dict, output_dir, image_ini_file, keep_tmp_dir, board_config_dir, mpp_service, uboot_env_file, security_cfg_file, build_type)

if __name__ == "__main__":
	main()
