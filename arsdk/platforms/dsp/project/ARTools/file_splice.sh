#!/bin/bash
prj_path=.
prj_name=$1
ceva_bin_name=${prj_name}
ceva_split_bin_name=${prj_name}
ceva_split_bin_pre_name=${prj_name}_0x
ceva_split_bin_pre_name_len=${#ceva_split_bin_pre_name}
echo "bin_name   "${ceva_split_bin_name}
echo "ceva_split_bin_pre_name   "${ceva_split_bin_pre_name}
echo "ceva_split_bin_pre_name len  "${ceva_split_bin_pre_name_len}

rm -rf *.bin 

cd coff2binary
echo "call convert.sh..."
./convert.sh ${ceva_bin_name}
cp ${ceva_split_bin_name}_*.bin ../ -rf 
rm ${ceva_split_bin_name}_*.bin  -rf 
cd -

C0_bin_name="_0C.bin"
D0_bin_name="_0D.bin"
C1_bin_name="_1C.bin"
D1_bin_name="_1D.bin"
D12_bin_name="_1D2.bin"

C0_bin_file_name="C0.bin"
D0_bin_file_name="D0.bin"
C1_bin_file_name="C1.bin"
D1_bin_file_name="D1.bin"
D12_bin_file_name="D12.bin"

C0_source_bin_file_name=$(find ./ -name "*${C0_bin_name}")
D0_source_bin_file_name=$(find ./ -name "*${D0_bin_name}")
C1_source_bin_file_name=$(find ./ -name "*${C1_bin_name}")
D1_source_bin_file_name=$(find ./ -name "*${D1_bin_name}")
D12_source_bin_file_name=$(find ./ -name "*${D12_bin_name}")
echo "C0_source_bin_file_name   "${C0_source_bin_file_name}
echo "D0_source_bin_file_name   "${D0_source_bin_file_name}
echo "C1_source_bin_file_name   "${C1_source_bin_file_name}
echo "D1_source_bin_file_name   "${D1_source_bin_file_name}
echo "D12_source_bin_file_name  "${D12_source_bin_file_name}


addr_len=8
C0_source_bin_file_name=${C0_source_bin_file_name:2}
D0_source_bin_file_name=${D0_source_bin_file_name:2}
C1_source_bin_file_name=${C1_source_bin_file_name:2}
D1_source_bin_file_name=${D1_source_bin_file_name:2}
D12_source_bin_file_name=${D12_source_bin_file_name:2}
echo ${C0_source_bin_file_name}
echo ${D0_source_bin_file_name}
echo ${C1_source_bin_file_name}
echo ${D1_source_bin_file_name}
echo ${D12_source_bin_file_name}

C0_bin_len=${C0_source_bin_file_name:${ceva_split_bin_pre_name_len}:${addr_len}}
D0_bin_len=${D0_source_bin_file_name:${ceva_split_bin_pre_name_len}:${addr_len}}
C1_bin_len=${C1_source_bin_file_name:${ceva_split_bin_pre_name_len}:${addr_len}}
D1_bin_len=${D1_source_bin_file_name:${ceva_split_bin_pre_name_len}:${addr_len}}
D12_bin_len=${D12_source_bin_file_name:${ceva_split_bin_pre_name_len}:${addr_len}}

echo "C0_bin_len   "${C0_bin_len}
echo "D0_bin_len   "${D0_bin_len}
echo "C1_bin_len   "${C1_bin_len}
echo "D1_bin_len   "${D1_bin_len}
echo "D12_bin_len   "${D12_bin_len}

cp ${C0_source_bin_file_name} ${C0_bin_file_name} -rf 
cp ${D0_source_bin_file_name} ${D0_bin_file_name} -rf 
cp ${C1_source_bin_file_name} ${C1_bin_file_name} -rf
cp ${D1_source_bin_file_name} ${D1_bin_file_name} -rf
cp ${D12_source_bin_file_name} ${D12_bin_file_name} -rf


./a.out ${C0_bin_len} ${D0_bin_len} ${C1_bin_len} ${D1_bin_len} ${D12_bin_len}



