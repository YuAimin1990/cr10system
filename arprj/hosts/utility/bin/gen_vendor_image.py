#!/usr/bin/env python
#
# Copyright (c) 2014, Artosyn
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
#
# vendor image layout:
# | header | 1d imem | 1d dmem | 2d imem | 2d dmem | diag imem | diag dmem |
# |         \
# |          \_________________________________________________________
# |
# | magic                 | version             | header checksum      |
# | ddr4 1d imem offset   | ddr4 1d imem size   | ddr4 1d imem sum     |
# | ddr4 1d dmem offset   | ddr4 1d dmem size   | ddr4 1d dmem sum     |
# | ddr4 2d imem offset   | ddr4 2d imem size   | ddr4 2d imem sum     |
# | ddr4 2d dmem offset   | ddr4 2d dmem size   | ddr4 2d dmem sum     |
# | ddr4 diag imem offset | ddr4 diag imem size | ddr4 diag imem sum   |
# | ddr4 diag dmem offset | ddr4 diag dmem size | ddr4 diag dmem sum   |
# | ddr3 1d imem offset   | ddr3 1d imem size   | ddr3 1d imem sum     |
# | ddr3 1d dmem offset   | ddr3 1d dmem size   | ddr3 1d dmem sum     |
# | reserve
#
#   total size 512 Byte

import argparse
import sys
import shutil
import os
import struct

IMAGE_HEADER_SIZE = 512
IMAGE_HEADER_SIZE_PART = 512 - 64
IMAGE_ALIGN_LEN = 512 #aligned with 512B

def write_header(input_file, version,
		 ddr4_imem_offset_1d, ddr4_imem_size_1d, ddr4_imem_sum_1d,
		 ddr4_dmem_offset_1d, ddr4_dmem_size_1d, ddr4_dmem_sum_1d,
		 ddr4_imem_offset_2d, ddr4_imem_size_2d, ddr4_imem_sum_2d,
		 ddr4_dmem_offset_2d, ddr4_dmem_size_2d, ddr4_dmem_sum_2d,
		 ddr4_imem_offset_diag, ddr4_imem_size_diag, ddr4_imem_sum_diag,
		 ddr4_dmem_offset_diag, ddr4_dmem_size_diag, ddr4_dmem_sum_diag,
		 ddr3_imem_offset_1d, ddr3_imem_size_1d, ddr3_imem_sum_1d,
		 ddr3_dmem_offset_1d, ddr3_dmem_size_1d, ddr3_dmem_sum_1d):
	magic = 0x41529301 # 'AR9301'
	header_checksum = 0x0

	input_file.seek(0x0);

	header = struct.Struct('27I')

	value = [ magic, \
		  version, \
		  header_checksum, \
		  ddr4_imem_offset_1d, \
		  ddr4_imem_size_1d, \
		  ddr4_imem_sum_1d, \
		  ddr4_dmem_offset_1d, \
		  ddr4_dmem_size_1d, \
		  ddr4_dmem_sum_1d, \
		  ddr4_imem_offset_2d, \
		  ddr4_imem_size_2d, \
		  ddr4_imem_sum_2d, \
		  ddr4_dmem_offset_2d, \
		  ddr4_dmem_size_2d, \
		  ddr4_dmem_sum_2d, \
		  ddr4_imem_offset_diag, \
		  ddr4_imem_size_diag, \
		  ddr4_imem_sum_diag, \
		  ddr4_dmem_offset_diag, \
		  ddr4_dmem_size_diag, \
		  ddr4_dmem_sum_diag, \
		  ddr3_imem_offset_1d, \
		  ddr3_imem_size_1d, \
		  ddr3_imem_sum_1d, \
		  ddr3_dmem_offset_1d, \
		  ddr3_dmem_size_1d, \
		  ddr3_dmem_sum_1d ]

	for i in value:
		header_checksum += i & 0xff
		header_checksum += i >> 8 & 0xff
		header_checksum += i >> 16 & 0xff
		header_checksum += i >> 24 & 0xff

	print "header checksum 0x%x" % header_checksum
#	#update checksum
	value[2] = header_checksum & 0xFFFFFFFF
	pack_data = header.pack(*value)
	input_file.write(pack_data);

def append_to(target_file, source_file):
	while True :
		nbytes = 16 * 1024
		if nbytes == 0 :
			break
		buf = source_file.read(nbytes)
		if not buf :
			break
		target_file.write(buf)

def file_checksum(input_file, start_offs):
	checksum = 0
	input_file.seek(start_offs)
	while True :
		nbytes = 16 * 1024
		if nbytes == 0 :
			break
		buf = input_file.read(nbytes)
		if not buf :
			break
		for i in range(len(buf)) :
			checksum += ord(buf[i])
	input_file.seek(0)

	return checksum

def int_parse(str):
	return int(str, 0)

def get_args():
	parser = argparse.ArgumentParser()

	parser.add_argument('--target', default = "", \
			    help='out put file path')

	parser.add_argument("--version", default = 0, \
			    required=True, type=int_parse, \
			    help="ddr firmware version 4 char")

	parser.add_argument('--ddr4_1d_imem', default="", \
			    help='ddr4 1d imem firmware file path')

	parser.add_argument('--ddr4_1d_dmem', default="", \
			    help='ddr4 1d dmem firmware file path')

	parser.add_argument('--ddr4_2d_imem', default="", \
			    help='ddr4 2d imem firmware file path')

	parser.add_argument('--ddr4_2d_dmem', default="", \
			    help='ddr4 2d dmem firmware file path')

	parser.add_argument('--ddr4_diag_imem', default="", \
			    help='ddr4 diag imem firmware file path')

	parser.add_argument('--ddr4_diag_dmem', default="", \
			    help='ddr4 diag dmem firmware file path')

	parser.add_argument('--ddr3_1d_imem', default="", \
			    help='ddr3 1d imem firmware file path')

	parser.add_argument('--ddr3_1d_dmem', default="", \
			    help='ddr3 1d dmem firmware file path')

	return parser.parse_args();

def image_align(input_file, file_size, align_size):
	pad_size = (align_size - (file_size % align_size)) % align_size
	if pad_size != 0:
		input_file.write(b'\x00' * pad_size) #align image
	print "pad size %d" % pad_size

	return file_size + pad_size

def main():
	args = get_args()

	path_target = args.target
	version = args.version
	path_ddr4_1d_imem = args.ddr4_1d_imem
	path_ddr4_1d_dmem = args.ddr4_1d_dmem
	path_ddr4_2d_imem = args.ddr4_2d_imem
	path_ddr4_2d_dmem = args.ddr4_2d_dmem
	path_ddr4_diag_imem = args.ddr4_diag_imem
	path_ddr4_diag_dmem = args.ddr4_diag_dmem
	path_ddr3_1d_imem = args.ddr3_1d_imem
	path_ddr3_1d_dmem = args.ddr3_1d_dmem

	if path_target == "":
		print "No target file"
	elif path_ddr4_1d_imem == "":
		print "No DDR4 1D IMEM firmware file"
	elif path_ddr4_1d_dmem == "":
		print "No DDR4 1D DMEM firmware file"
	elif path_ddr4_2d_imem == "":
		print "No DDR4 2D IMEM firmware file"
	elif path_ddr4_2d_dmem == "":
		print "No DDR4 2D DMEM firmware file"
	elif path_ddr4_diag_imem == "":
		print "No DDR4 DIAG IMEM firmware file"
	elif path_ddr4_diag_dmem == "":
		print "No DDR4 DIAG DMEM firmware file"
	elif path_ddr3_1d_imem == "":
		print "No DDR3 1D IMEM firmware file"
	elif path_ddr3_1d_dmem == "":
		print "No DDR3 1D DMEM firmware file"
	else:
		print "Firmware path OK"

	file_target = open(path_target, 'wb+')
	file_ddr4_1d_imem = open(path_ddr4_1d_imem)
	file_ddr4_1d_dmem = open(path_ddr4_1d_dmem)
	file_ddr4_2d_imem = open(path_ddr4_2d_imem)
	file_ddr4_2d_dmem = open(path_ddr4_2d_dmem)
	file_ddr4_diag_imem = open(path_ddr4_diag_imem)
	file_ddr4_diag_dmem = open(path_ddr4_diag_dmem)
	file_ddr3_1d_imem = open(path_ddr3_1d_imem)
	file_ddr3_1d_dmem = open(path_ddr3_1d_dmem)

	size_ddr4_1d_imem = os.path.getsize(path_ddr4_1d_imem)
	size_ddr4_1d_dmem = os.path.getsize(path_ddr4_1d_dmem)
	size_ddr4_2d_imem = os.path.getsize(path_ddr4_2d_imem)
	size_ddr4_2d_dmem = os.path.getsize(path_ddr4_2d_dmem)
	size_ddr4_diag_imem = os.path.getsize(path_ddr4_diag_imem)
	size_ddr4_diag_dmem = os.path.getsize(path_ddr4_diag_dmem)
	size_ddr3_1d_imem = os.path.getsize(path_ddr3_1d_imem)
	size_ddr3_1d_dmem = os.path.getsize(path_ddr3_1d_dmem)

	print "DDR4 1D IMEM size %d" % size_ddr4_1d_imem
	print "DDR4 1D DMEM size %d" % size_ddr4_1d_dmem
	print "DDR4 2D IMEM size %d" % size_ddr4_2d_imem
	print "DDR4 2D DMEM size %d" % size_ddr4_2d_dmem
	print "DDR4 Diag IMEM size %d" % size_ddr4_diag_imem
	print "DDR4 Diag DMEM size %d" % size_ddr4_diag_dmem
	print "DDR3 1D IMEM size %d" % size_ddr3_1d_imem
	print "DDR3 1D DMEM size %d" % size_ddr3_1d_dmem

	sum_ddr4_1d_imem = file_checksum(file_ddr4_1d_imem, 0)
	sum_ddr4_1d_dmem = file_checksum(file_ddr4_1d_dmem, 0)
	sum_ddr4_2d_imem = file_checksum(file_ddr4_2d_imem, 0)
	sum_ddr4_2d_dmem = file_checksum(file_ddr4_2d_dmem, 0)
	sum_ddr4_diag_imem = file_checksum(file_ddr4_diag_imem, 0)
	sum_ddr4_diag_dmem = file_checksum(file_ddr4_diag_dmem, 0)
	sum_ddr3_1d_imem = file_checksum(file_ddr3_1d_imem, 0)
	sum_ddr3_1d_dmem = file_checksum(file_ddr3_1d_dmem, 0)

	print "DDR4 1D IMEM sum %d" % sum_ddr4_1d_imem
	print "DDR4 1D DMEM sum %d" % sum_ddr4_1d_dmem
	print "DDR4 2D IMEM sum %d" % sum_ddr4_2d_imem
	print "DDR4 2D DMEM sum %d" % sum_ddr4_2d_dmem
	print "DDR4 Diag IMEM sum %d" % sum_ddr4_diag_imem
	print "DDR4 Diag DMEM sum %d" % sum_ddr4_diag_dmem
	print "DDR3 1D IMEM sum %d" % sum_ddr3_1d_imem
	print "DDR3 1D DMEM sum %d" % sum_ddr3_1d_dmem

	# pad for header
	file_target.write(b'\x00' * IMAGE_HEADER_SIZE_PART)

	offset_ddr4_1d_imem = IMAGE_HEADER_SIZE
	append_to(file_target, file_ddr4_1d_imem)

	offset_ddr4_1d_dmem = image_align(file_target, IMAGE_HEADER_SIZE + size_ddr4_1d_imem, IMAGE_ALIGN_LEN)
	append_to(file_target, file_ddr4_1d_dmem)

	offset_ddr4_2d_imem = image_align(file_target, offset_ddr4_1d_dmem + size_ddr4_1d_dmem, IMAGE_ALIGN_LEN)
	append_to(file_target, file_ddr4_2d_imem)

	offset_ddr4_2d_dmem = image_align(file_target, offset_ddr4_2d_imem + size_ddr4_2d_imem, IMAGE_ALIGN_LEN)
	append_to(file_target, file_ddr4_2d_dmem)

	offset_ddr4_diag_imem = image_align(file_target, offset_ddr4_2d_dmem + size_ddr4_2d_dmem, IMAGE_ALIGN_LEN)
	append_to(file_target, file_ddr4_diag_imem)

	offset_ddr4_diag_dmem = image_align(file_target, offset_ddr4_diag_imem + size_ddr4_diag_imem, IMAGE_ALIGN_LEN)
	append_to(file_target, file_ddr4_diag_dmem)

	offset_ddr3_1d_imem = image_align(file_target, offset_ddr4_diag_dmem + size_ddr4_diag_dmem, IMAGE_ALIGN_LEN)
	append_to(file_target, file_ddr3_1d_imem)

	offset_ddr3_1d_dmem = image_align(file_target, offset_ddr3_1d_imem + size_ddr3_1d_imem, IMAGE_ALIGN_LEN)
	append_to(file_target, file_ddr3_1d_dmem)

	end_offset = image_align(file_target, offset_ddr3_1d_dmem + size_ddr3_1d_dmem, IMAGE_ALIGN_LEN)

	print "offset_ddr4_1d_imem %d" % offset_ddr4_1d_imem
	print "offset_ddr4_1d_dmem %d" % offset_ddr4_1d_dmem
	print "offset_ddr4_2d_imem %d" % offset_ddr4_2d_imem
	print "offset_ddr4_2d_dmem %d" % offset_ddr4_2d_dmem
	print "offset_ddr4_diag_imem %d" % offset_ddr4_diag_imem
	print "offset_ddr4_diag_dmem %d" % offset_ddr4_diag_dmem
	print "offset_ddr3_1d_imem %d" % offset_ddr3_1d_imem
	print "offset_ddr3_1d_dmem %d" % offset_ddr3_1d_dmem

	write_header(file_target, version, \
		     offset_ddr4_1d_imem, size_ddr4_1d_imem, sum_ddr4_1d_imem, \
		     offset_ddr4_1d_dmem, size_ddr4_1d_dmem, sum_ddr4_1d_dmem, \
		     offset_ddr4_2d_imem, size_ddr4_2d_imem, sum_ddr4_2d_imem, \
		     offset_ddr4_2d_dmem, size_ddr4_2d_dmem, sum_ddr4_2d_dmem, \
		     offset_ddr4_diag_imem, size_ddr4_diag_imem, sum_ddr4_diag_imem, \
		     offset_ddr4_diag_dmem, size_ddr4_diag_dmem, sum_ddr4_diag_dmem,
		     offset_ddr3_1d_imem, size_ddr3_1d_imem, sum_ddr3_1d_imem, \
		     offset_ddr3_1d_dmem, size_ddr3_1d_dmem, sum_ddr3_1d_dmem)

	file_target = file_target.close()
	file_ddr4_1d_imem = file_ddr4_1d_imem.close()
	file_ddr4_1d_dmem = file_ddr4_1d_dmem.close()
	file_ddr4_2d_imem = file_ddr4_2d_imem.close()
	file_ddr4_2d_dmem = file_ddr4_2d_dmem.close()
	file_ddr4_diag_imem = file_ddr4_diag_imem.close()
	file_ddr4_diag_dmem = file_ddr4_diag_dmem.close()
	file_ddr3_1d_imem = file_ddr3_1d_imem.close()
	file_ddr3_1d_dmem = file_ddr3_1d_dmem.close()

	print "generate spl done..."

if __name__ == "__main__":
	main()

