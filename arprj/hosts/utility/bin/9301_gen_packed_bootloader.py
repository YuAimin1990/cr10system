#!/usr/bin/env python
#
# Copyright (c) 2014, Linaro Limited
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
# bootloader file layout:
# |---header---|---fw---|---signature---|---spl---|
#

import argparse
import sys
import shutil
import os
import struct

IMAGE_ALIGN_LEN = 512 #aligned with 512B
IMAGE_TYPE_COMPRESS = 0x1
IMAGE_TYPE_COMPRESS_SHIFT = 0x10
IMAGE_TYPE_COMPRESS_MASK = 0xf

def write_header(outf, flag, spl_addr, spl_len, troot_fw_addr, troot_fw_len, sig_addr, sig_len, spl_dtb_offset, checksuml, checksumh):
	magic = 0x41529301 # 'AR**'
	version = 1
	header_checksum = 0x0 

	outf.seek(0x0);

	header=struct.Struct('16I')

	value=[magic, \
		0x00400001, \
		header_checksum, \
		version, \
		flag, \
		0x0,  \
		spl_addr, \
		spl_len, \
		troot_fw_addr, \
		troot_fw_len, \
		sig_addr, \
		sig_len, \
		spl_dtb_offset, \
		0x0, \
		checksuml, \
		checksumh ]

    	for i in value:
		header_checksum += i
	
	print "header checksum 0x%x" % header_checksum
	#update checksum
	value[2] = header_checksum&0xFFFFFFFF
	pack_data =header.pack(*value)
	outf.write(pack_data);


def append_to(outf, in_file):
	inf = open(in_file, 'rb');

	while True :
		nbytes = 16 * 1024
		if nbytes == 0 :
			break
		buf = inf.read(nbytes)
		if not buf :
			break
		outf.write(buf)
	inf.close()

def file_checksum(inf, start_offs):
	checksum=0
	inf.seek(start_offs)
	while True :
		nbytes = 16 * 1024
		if nbytes == 0 :
			break
		buf = inf.read(nbytes)
		if not buf :
			break

		for i in range(len(buf)) :
			checksum+=ord(buf[i])

	return checksum


def file_checksum64(inf, start_offs):
        checksum=0L
        inf.seek(start_offs)
        while True :
                nbytes = 16 * 1024
                if nbytes == 0 :
                        break
                buf = inf.read(nbytes)
                if not buf :
                        break

                for i in range(len(buf)/8) :
                        data = struct.unpack_from("L",buf,i*8)[0]
			checksum += data

        return checksum



def int_parse(str):
	return int(str, 0)

def get_args():
	parser = argparse.ArgumentParser()

	parser.add_argument('--out_file', \
		required=True, type=argparse.FileType('w+b'), \
		help='out put file path')

	parser.add_argument('--spl_addr', \
		required=True, type=int_parse, \
		help='spl entry point')

	parser.add_argument('--spl_file', \
		required=True, \
		help='spl file path')

	parser.add_argument('--fw_addr', \
		type=int_parse, default=0, \
		help='troot firmware run address')

	parser.add_argument('--fw_file', \
		default="", \
		help='troot firmware file path')

	parser.add_argument('--sig_addr', \
		type=int_parse, default=0, \
		help='signature address')

	parser.add_argument('--sig_file', \
		default="", \
		help='signature file path')

	parser.add_argument('--compress', \
		default="n", \
		help='if image is compressed')

	parser.add_argument('--spl_nodtb_file', \
		default="", \
		help='spl nodtb file path')

	parser.add_argument('--flag', \
		type=int_parse, default=0, \
		help='spl image flags')	

	return parser.parse_args();

def main():
	args = get_args()
	out_file = args.out_file
	spl_addr = args.spl_addr
	spl_file = args.spl_file
	spl_nodtb_file = args.spl_nodtb_file
	fw_addr = args.fw_addr
	fw_file = args.fw_file
	sig_addr = args.sig_addr
	sig_file = args.sig_file
	compress = args.compress
	flag = args.flag

	spl_len = os.path.getsize(spl_file)

	if spl_nodtb_file == "":
		spl_dtb_offset=0
	else:
		spl_dtb_offset = os.path.getsize(spl_nodtb_file)
	if fw_file == "":
		fw_len=0
	else:
		fw_len = os.path.getsize(fw_file)
	if sig_file == "":
		sig_len=0
	else:
		sig_len = os.path.getsize(sig_file)

	print "spl addr 0x%x len %d" % (spl_addr, spl_len)
	print "fw addr  0x%x len %d" % (fw_addr, fw_len)
	print "sig addr 0x%x len %d" % (sig_addr, sig_len)

	if fw_file == "" and fw_addr != 0:
		print "Error 0"
		sys.exit(0)
	if fw_file != "" and fw_addr == 0:
		print "Error 1"
		sys.exit(0)
	if sig_file == "" and sig_addr != 0:
		print "Error 2"
		sys.exit(0)
	if sig_file != "" and sig_addr == 0:
		print "Error 3"
		sys.exit(0)
	if fw_file == "" and sig_file != "":
		print "Error 4"
		sys.exit(0)
	if fw_file != "" and sig_file == "":
		print "Error 5"
		sys.exit(0)

	#fill header 
	out_file.write(b'\x00'*IMAGE_ALIGN_LEN)

	if fw_file == "":
		print "No input firmware file"
	else:
		append_to(out_file, fw_file)
		fill_len = fw_len%IMAGE_ALIGN_LEN
		if fill_len != 0:
			out_file.write(b'\x00'*(IMAGE_ALIGN_LEN-fill_len)) #align image               

	if sig_file == "":
		print "No input signature file"
	else:
		append_to(out_file, sig_file)
		fill_len = sign_len%IMAGE_ALIGN_LEN
		if fill_len != 0:
			out_file.write(b'\x00'*(IMAGE_ALIGN_LEN-fill_len)) #align image               

	append_to(out_file, spl_file)
	fill_len = spl_len%IMAGE_ALIGN_LEN
	out_file.write(b'\x00'*(IMAGE_ALIGN_LEN-fill_len)) #align image               

	checksum = file_checksum64(out_file, IMAGE_ALIGN_LEN) #skip header

	checksumL = int(checksum & 0xFFFFFFFF)
	checksumH = int((checksum >>32) & 0xFFFFFFFF)

	print "checkL = %x" % checksumL	
	print "checkH = %x" % checksumH	

	#align spl_len, fw_len, sig_len with 8 bytes
	spl_len = (spl_len+7)&0xFFFFFFF8
	fw_len  = (fw_len+7)&0xFFFFFFF8
	sig_len = (sig_len+7)&0xFFFFFFF8

	write_header(out_file, \
			flag,  \
			spl_addr, \
			spl_len, \
			fw_addr, \
			fw_len, \
			sig_addr, \
			sig_len, \
			spl_dtb_offset, \
			checksumL, \
			checksumH)


	out_file.close()

	print "generate spl done..."

if __name__ == "__main__":
	main()

