from struct import *
import sys
import os

if len(sys.argv)!=2:
    print("Usage:python coff2binary.py filename");

#os.system("rm -rf *.bin")

filename = sys.argv[1]
print("filename:"+sys.argv[1])
filename0 =  filename+"0C.out"
filename1 =  filename+"0D.out"
filename2 =  filename+"1C.out"
filename3 =  filename+"1D.out"

filename0_srt =  filename+"0C.srt"
filename1_srt =  filename+"0D.srt"
filename2_srt =  filename+"1C.srt"
filename3_srt =  filename+"1D.srt"

filename0_bin =  "_0C.bin"
filename1_bin =  "_0D.bin"
filename2_bin =  "_1C.bin"
filename3_bin =  "_1D.bin"
filename32_bin =  "_1D2.bin"

os.system("./sorthex " + filename0 + " "+ filename0_srt)
os.system("./sorthex " + filename1 + " "+ filename1_srt)
os.system("./sorthex " + filename2 + " "+ filename2_srt)
os.system("./sorthex " + filename3 + " "+ filename3_srt)

file_in_list = {0:filename0_srt,1:filename1_srt,2:filename2_srt,3:filename3_srt}
file_out_list = {0:filename0_bin,1:filename1_bin,2:filename2_bin,3:filename3_bin,4:filename32_bin}
code_or_data = {0:'C:',1:'D:',2:'C:',3:'D:'}

#handle the file 0C, 1C, 0D;
for i in range(3):
    print "Handle:",file_in_list[i]
    if(os.path.exists(file_in_list[i])):
        with open(file_in_list[i]) as fp:
            lines = fp.readlines()
            filesize = int(lines[len(lines)-1].strip(code_or_data[i]).split(' ')[0], 16) - int(lines[0].strip(code_or_data[i]).split(' ')[0],16) + 1
	    print "filesize: ",filesize
            array = ['0']*filesize
            for index in range(len(array)):
                array[index] = chr(0)

	    print "Start retrieving..."
            for line in lines:
                line = line.strip(code_or_data[i])
                obj_struct = {}
                obj_struct['address'] = (int)(line.split(' ')[0],16) -int(lines[0].strip(code_or_data[i]).split(' ')[0],16)
                obj_struct['value'] =   line.split(' ')[1]
                array[obj_struct['address']] = chr(int(obj_struct['value'],16))
            array_str="".join(array)
            file = open(filename+'_0x'+lines[0].strip(code_or_data[i]).split(' ')[0] + file_out_list[i],"wb")
	    print "Start write binary file..."
            file.write(array_str)
            file.close()
    else:
        print("File " + file_in_list[i] + " not exist!")

#Handle the file 1D.srt, sometimes it's huge when using sram and ddr
if(os.path.exists(file_in_list[3])):
	print "Handle:",file_in_list[3] 
        with open(file_in_list[3]) as fp:
	    offset = 0
            lines = fp.readlines()
            filesize = int(lines[len(lines)-1].strip(code_or_data[3]).split(' ')[0], 16) - int(lines[0].strip(code_or_data[3]).split(' ')[0],16) + 1
	    print "filesize:",filesize
	    if(filesize > 0x10000000): #need to split
		array = ['0']*0x2000000
		for index in range(len(array)):
 	               array[index] = chr(0)

		file = open(filename+'_0x'+lines[0].strip(code_or_data[3]).split(' ')[0] + file_out_list[3],"wb")
		count = 0
		#find the big hole
		for line in lines:
			line = line.strip(code_or_data[3])
			obj_struct = {}
			obj_struct['address'] = (int)(line.split(' ')[0],16) -int(lines[0].strip(code_or_data[3]).split(' ')[0],16)
			obj_struct['value'] =   line.split(' ')[1]
			#print "line address:",obj_struct['address']
			if(obj_struct['address'] > 0x10000000 and offset == 0):
				offset = obj_struct['address']
				print "offset: ",offset
				print "Save to 1D.bin"
				array_str="".join(array[0:count])
				file.write(array_str)
				file.close()
				print "Open 1D2.bin"
				file = open(filename+'_0x'+line.split(' ')[0] + file_out_list[4],"wb")
				for index in range(len(array)):
 	               			array[index] = chr(0)
		
			#print "line 88: offset:",offset								
			count = obj_struct['address'] - offset
			array[count] = chr(int(obj_struct['value'],16))

		array_str="".join(array[0:count])
		file.write(array_str)
		file.close()
	    else: 
		array = ['0']*filesize
		for index in range(len(array)):
			array[index] = chr(0)

		print "Start retrieving binary..."
		for line in lines:
			line = line.strip(code_or_data[3])
			obj_struct = {}
			obj_struct['address'] = (int)(line.split(' ')[0],16) -int(lines[0].strip(code_or_data[3]).split(' ')[0],16)
			obj_struct['value'] =   line.split(' ')[1]
			array[obj_struct['address']] = chr(int(obj_struct['value'],16))
		array_str="".join(array)
		file = open(filename+'_0x'+lines[0].strip(code_or_data[3]).split(' ')[0] + file_out_list[3],"wb")
		file.write(array_str)
		file.close()
else:
    print("File " + file_in_list[3] + " not exist!")

print "Coff2binary done..."

#os.system("rm -rf *.out ")

