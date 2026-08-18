import struct
import sys

BINFILE = sys.argv[1]#'boot.bin'
NEWFILE = sys.argv[2]#'code.hex'
OUT_WORD_LEN = int(sys.argv[3])#32768
OUT_SIZE_LEN = int(sys.argv[4])#128

# transfer bin to int
data = open(BINFILE, 'rb').read()
data_len = len(data)
unpack_str = ">%dB"%(data_len)
value_t = struct.unpack(unpack_str, data)

# pad zeros
pad_deficiency = OUT_WORD_LEN * 4 - data_len
zero_u = (0, )
zero_t = zero_u * pad_deficiency
final_t = value_t + zero_t

# transfer int to hex
final_hexstr = ['%02x' % i for i in final_t]

# format the data to lines as required
k = OUT_SIZE_LEN // 8
i = 0
newhexfile = []
for element in final_hexstr:
    if (i == 0): newhexline = element
    else: newhexline = element + newhexline

    i = i + 1
    if (i == k):
        newhexfile.append(newhexline)
        i = 0

# write out to file
newf = open(NEWFILE, 'w')
for line in newhexfile:
    newf.write(line+'\n')
newf.close()



