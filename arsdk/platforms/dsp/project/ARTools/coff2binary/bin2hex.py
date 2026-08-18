import struct
import sys

BINFILE = sys.argv[1]#'boot.bin'
NEWFILE = sys.argv[2]#'code.hex'
OUT_WORD_LEN = sys.argv[3]#32768 #Output total length in WORD(32bit)
OUT_SIZE_LEN = sys.argv[4]#128   #Output length of each line

# transfer bin to int
data = open(BINFILE, 'rb').read()
data_len = len(data) #length in byte
unpack_str = ">%dB"%(data_len)
value_t = struct.unpack(unpack_str, data)

OUT_WORD_LEN = int(OUT_WORD_LEN)
OUT_SIZE_LEN = int(OUT_SIZE_LEN)

#print(OUT_WORD_LEN, OUT_SIZE_LEN)

# pad zeros
pad_deficiency = OUT_WORD_LEN * 4 - data_len #length in byte

# if data_len is larger than denoted file size, would expand file size automatically.
if (pad_deficiency < 0):
    u = 4*(OUT_SIZE_LEN//32)
    new_OUT_WORD_LEN = ((OUT_WORD_LEN // u) + 1) * u
    print("Warning[bin2hex.py]: Out total len [{0}x4B] is shorter than bin file size [{1}B]. Would upgrade out len to [{2}B]".format(OUT_WORD_LEN, data_len, new_OUT_WORD_LEN))
    # renew 2 variables
    pad_deficiency = new_OUT_WORD_LEN * 4 - data_len
    OUT_WORD_LEN = new_OUT_WORD_LEN

# final data in decimal
zero_u = (0, )
zero_t = zero_u * pad_deficiency
final_t = value_t + zero_t

# final data transfer from decimal to hex
final_hexstr = ['%02x' % i for i in final_t]

# format the data to lines as required
k = OUT_SIZE_LEN // 8
i = 0
newhexfile = []
for one_hexstr in final_hexstr:
    if (i == 0): newhexline = one_hexstr
    else: newhexline = one_hexstr + newhexline

    i = i + 1
    if (i == k):
        newhexfile.append(newhexline)
        i = 0

# write out to file
newf = open(NEWFILE, 'w')
for line in newhexfile:
    newf.write(line+'\n')
newf.close()
