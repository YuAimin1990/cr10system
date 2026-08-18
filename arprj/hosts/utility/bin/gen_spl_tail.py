#!/usr/bin/env python

import argparse
import sys
import shutil
import os
import struct

def write_tail(outf, ddr_type, ddr_freq, ddr_width, spl_len):

	outf.seek(spl_len)
	tail=struct.Struct('3I')
	value=(ddr_type, \
		ddr_freq, \
		ddr_width)
	pack_data =tail.pack(*value)
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

def int_parse(str):
	return int(str, 0)

def get_args():
	parser = argparse.ArgumentParser()

	parser.add_argument('--out_file', \
		required=True, type=argparse.FileType('w+b'), \
		help='out put file path')

	parser.add_argument('--spl_file', \
		required=True, \
		help='spl file path')
		
	parser.add_argument('--ddr_type', \
		type=int_parse, default=0, \
		help='ddr type')

	parser.add_argument('--ddr_freq', \
		type=int_parse, default=0, \
		help='ddr freq')

	parser.add_argument('--ddr_width', \
		type=int_parse, default=0, \
		help='ddr width')


	return parser.parse_args();

def main():
	args = get_args()
	out_file = args.out_file
	spl_file = args.spl_file
	ddr_type = args.ddr_type
	ddr_freq = args.ddr_freq
	ddr_width = args.ddr_width
	
	spl_len = os.path.getsize(spl_file)
	print "spl len %d" % spl_len

	print "ddr type %d" % ddr_type
	print "ddr freq %d" % ddr_freq
	print "ddr width %d" % ddr_width
	out_file.seek(0x0)
	append_to(out_file, spl_file)
	write_tail(out_file, \
		ddr_type, \
		ddr_freq, \
		ddr_width, \
		spl_len)
	
	out_file.close()

	print "spl add tail done..."

if __name__ == "__main__":
	main()

