#!/usr/bin/env python

import argparse
import os
import struct
import ConfigParser
import subprocess

def get_args():
	parser = argparse.ArgumentParser()

	parser.add_argument('--dl_type', \
		default="wget", \
		help='download method type, default wget')

	parser.add_argument('--filename', \
		help='download filename')

	parser.add_argument('--rt_dir', \
		help='the remote path of download file')

	parser.add_argument('--dl_dir', \
		help='the path to save download file')

	parser.add_argument('--extract_dir', \
		help='the path to extract file')

	parser.add_argument('--version', \
		help='the version of file')

	parser.add_argument('--keep_tar_file', \
		help='whether keep download tar file')

	return parser.parse_args()

def extract(src, dst):
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
			print("suffix is false")
			return False
		print("extract cmd:", cmd)
		result = subprocess.check_call(cmd, shell=True)
		if 0 != result:
			return False
	else:
		print("Wrong file format")
 		return False

	return True

def compress(tar_dir, src_dir, dst_name):
	if not os.path.isdir(tar_dir + src_dir):
		print("There is no dst:", src_dir)
		return False
	if '.' in dst_name:
		suffix = dst_name.split('.')[-1]
		print("suffix:", suffix);
		if 'tar' == suffix:
			cmd = "cd " + tar_dir + " && tar -c -v -f " + dst_name + " " + src_dir
		elif 'gz' == suffix:
			cmd = "cd " + tar_dir + " && tar -c -v -z -f " + dst_name + " " + src_dir
		elif 'bz2' == suffix:
			cmd = "cd " + tar_dir + " && tar -c -v -j -f " + dst_name + " " + src_dir
		elif 'lzma' == suffix:
			cmd = "cd " + tar_dir + " && tar -c -v --lzma -f " + dst_name + "" + src_dir
		elif 'lz' == suffix:
			cmd = "cd " + tar_dir + " && tar -c -v --lzip -f " + dst_name + " " + src_dir
		elif 'tlz' == suffix:
			cmd = "cd " + tar_dir + " && tar -c -v --lzma -f " + dst_name + " " + src_dir
		elif 'lzo' == suffix or 'tzo' == suffix:
			cmd = "cd " + tar_dir + " && tar -c -v --lzop -f " + dst_name + " " + src_dir
		elif '.xz' == suffix:
			cmd = "cd " + tar_dir + " && tar -c -v --xz -f " + dst_name + " " + src_dir
		elif 'Z' == suffix:
			cmd = "cd " + tar_dir + " && tar -c -v -Z -f " + dst_name + " " + src_dir
		elif 'tgz' == suffix:
			cmd = "cd " + tar_dir + " && tar -c -v -z -f " + dst_name + " " + src_dir
		elif 'tbz2' == suffix or 'tbz' == suffix:
			cmd = "cd " + tar_dir + " && tar -c -v -j -f " + dst_name + " " + src_dir
		elif 'zip' == suffix:
			cmd = "cd " + tar_dir + " && zip -r " + dst_name + " " + src_dir
		else:
			print("suffix is false")
			return False
		print("compress cmd:", cmd)

		result = subprocess.check_call(cmd, shell=True)
		if 0 != result:
			return False
	else:
		print("Wrong file format")
 		return False

	return True

def check(src, filename):
	cmd = "cd " + src + " && md5sum --quiet -c " + filename + ".md5";
	result = subprocess.check_call(cmd, shell=True)
	print("cmd:", cmd)
	if 0 == result:
		print("md5 check pass!", result)
	else:
		print("md5 check fail!", result)
		return False
	return True

def download(rt_dir, filename, dl_dir, download_type):
	if not os.path.exists(dl_dir):
		os.makedirs(dl_dir)
	if "wget" == download_type:
		cmd = "wget --no-verbose --no-check-certificate --user ftptest --password ftptest -q " \
              + rt_dir + "/" + filename \
              + " -O " + dl_dir + "/" + filename
		print("cmd:", cmd)
		result = subprocess.check_call(cmd, shell=True)
		print("result:", result)

	return


def main():
    # parrse args
	args = get_args()
	dl_type = args.dl_type
	filename = args.filename
	rt_dir = args.rt_dir
	dl_dir = args.dl_dir
	extract_dir = args.extract_dir
	keep_tar_file = args.keep_tar_file
	version = args.version
	print("keep_tar_file:", keep_tar_file)
#example:
#	rt_dir = "ftp://192.168.200.228/data/arsdk/resources/kernel"
#	filename = "linux-linaro-stable-lsk-v4.9-17.07.tar.gz"
#	dl_dir = "./tmp"
#	extract_dir = "./tmp"

	file_tar = dl_dir + "/" + filename;
	result = True

	file_dir = version
	extract_file_dir = extract_dir + file_dir
	mods_dir = extract_dir + "mods/" + file_dir
	print("file_dir:", file_dir)
	print("extract_file_dir:", extract_file_dir)
	print("mods_dir:", mods_dir)

	if not os.path.exists(extract_file_dir):
		#1. downlod and check md5
		if not os.path.isfile(file_tar):
			download(rt_dir, filename, dl_dir, dl_type)
			download(rt_dir, filename+".md5", dl_dir, dl_type)
			result = check(dl_dir, filename)
			#1.1 extract
			if result:
				extract(dl_dir+"/"+filename, extract_dir)
				#1.2 remove tar and md5
				os.remove(dl_dir + "/" + filename)
				os.remove(dl_dir + "/" + filename + ".md5")
				#1.3 mods_patch
				if os.path.exists(mods_dir):
					print("patch ...")
					cmd = "cp -ur " + mods_dir + "/* " + extract_file_dir
					result = subprocess.check_call(cmd, shell=True)
				#1.3 recompress
				if keep_tar_file == "true":
					print("compress new tar ...")
					compress(extract_dir, file_dir, filename)
			else:
				return
		else:
			extract(dl_dir+"/"+filename, extract_dir)
			if os.path.exists(mods_dir):
				print("patch ...")
				cmd = "cp -ur " + mods_dir + "/* " + extract_file_dir
				result = subprocess.check_call(cmd, shell=True)
	else:
		if os.path.exists(mods_dir):
			print("patch ...")
			cmd = "cp -ur " + mods_dir + "/* " + extract_file_dir
			result = subprocess.check_call(cmd, shell=True)

if __name__ == "__main__":
	main()
