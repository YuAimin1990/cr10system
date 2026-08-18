#!/bin/sh

# $1: time_ms $2: ddr_freq $3: noc_freq
if [ $# == 0 ]; then
	echo "no input params, exit"
        echo "[param1]:time_ms [param2]:ddr_freq [param3]:noc_freq"
        exit 0
fi


echo "run ddr monitor start "

echo "test_time(Ms):"$1
echo "ddr_freq(MHz):"$2
echo "noc_freq(MHz):"$3
echo 

reg=$(expr "$2" / "4") 
reg=$(expr "$reg" \* "$1")         
reg=$(expr "$reg" \* "1000")


#$1:reg  $2: val $3: start_bit $4:end_bit
function set_reg_bits() {
        let op_val=$(($4 - $3))
        let op_val=$(($op_val + 1))
        let op_val=$((32 - $op_val)) 
        let op_val=$((0xffffffff>>$op_val))	
        let op_val=$(($op_val<<$3))
        let op_val=$((~$op_val))        
        let op_val=$(($op_val&0xffffffff))
        
        let op_val_1=$(($2<<$3))
        
        let reg_val=`devmem $1 32`
        let reg_val=$(($reg_val&$op_val))
	let reg_val=$(($reg_val|$op_val_1))
        
        reg_val=$(printf "0x%08x" "$reg_val")
	#echo set_reg_bits value: $reg_val
        devmem $1 32 $reg_val 
}

function get_reg_bits() {
       let op_val=$(($3 - $2))
       let op_val=$(($op_val + 1))
       let op_val=$((32 - $op_val))
       let op_val=$((0xffffffff>>$op_val))
       let op_val=$(($op_val<<$2))
       let reg_val=`devmem $1 32`
       let reg_val=$(($reg_val&$op_val))
       let reg_val=$(($reg_val>>$2))
       #reg_val=$(printf "0x%08x" "$reg_val")
       echo  $reg_val
} 

# reset the monitor

set_reg_bits  0x06010058  0   2   5  
set_reg_bits  0x06010058  0  11  14 
usleep 1000

set_reg_bits  0x06010058  0xf   2   5           
set_reg_bits  0x06010058  0xf  11  14  
usleep 1000                              
  
  
set_reg_bits  0x06020004  $reg   0   31     
ret=$(get_reg_bits 0x06020004  0 31) 
echo ddr freq reg: $reg $ret
reg=$(expr "$3" \* "$1")
reg=$(expr "$reg" \* "1000")  
set_reg_bits  0x06030004  $reg   0   31
set_reg_bits  0x06040004  $reg   0   31
set_reg_bits  0x06050004  $reg   0   31
ret=$(get_reg_bits 0x06030004  0 31) 
echo noc freq reg: $reg $ret
echo 


#trigger on 
set_reg_bits  0x06020000  0x20   0  31
set_reg_bits  0x06030000  0         0  31
set_reg_bits  0x06040000  0         0  31
set_reg_bits  0x06050000  0         0  31

set_reg_bits  0x06020000  0x21   0  31
set_reg_bits  0x06030000  1         0  31
set_reg_bits  0x06040000  1         0  31
set_reg_bits  0x06050000  1         0  31


texp=$(expr "$1" \* "2000")    
usleep $texp


ret=$(get_reg_bits 0x06020200  0 31)
ret=$(get_reg_bits 0x06020200  0 31)
ret=$(get_reg_bits 0x06020200  0 31)
ret=$(get_reg_bits 0x06020200  0 31)
ret=$(get_reg_bits 0x06020200  0 31)
ret=$(get_reg_bits 0x06020200  0 31)
ret=$(get_reg_bits 0x06020200  0 31)

ret=$(get_reg_bits 0x06020200  0 31)
echo ddr_wr_bw:$ret 
ddr_wr_bw_MB=$(expr "$ret" \* "16")
ddr_wr_bw_MB=$(expr "$ddr_wr_bw_MB" / "$1")
ddr_wr_bw_MB=$(expr "$ddr_wr_bw_MB" / "1000")   


ret=$(get_reg_bits 0x06020200  0 31)
echo ddr_rd_bw:$ret 
ddr_rd_bw_MB=$(expr "$ret" \* "16") 
ddr_rd_bw_MB=$(expr "$ddr_rd_bw_MB" / "$1")  
ddr_rd_bw_MB=$(expr "$ddr_rd_bw_MB" / "1000") 

echo ddr_wr_bw_MB: $ddr_wr_bw_MB ddr_rd_bw_MB:$ddr_rd_bw_MB
echo 


#axi0
axi0_wr_cnt=$(get_reg_bits 0x06030040  0 31)
axi0_wrdata_cnt=$(get_reg_bits 0x06030044  0 31) 
axi0_wrdata_byte_cnt_l=$(get_reg_bits 0x06030048  0 31) 
axi0_wrdata_byte_cnt_h=$(get_reg_bits 0x0603004c  0 31) 
let axi0_wrdata_byte_cnt_h=$(($axi0_wrdata_byte_cnt_h<<32))
let axi0_wrdata_byte=$(($axi0_wrdata_byte_cnt_l|$axi0_wrdata_byte_cnt_h))
axi0_rd_cnt=$(get_reg_bits 0x06030050  0 31) 
axi0_rddata_cnt=$(get_reg_bits 0x06030054  0 31) 

echo axi0_wr_cnt: $axi0_wr_cnt 
echo axi0_wrdata_cnt:$axi0_wrdata_cnt 
echo axi0_wrdata_byte_cnt: $axi0_wrdata_byte 
echo axi0_rd_cnt:$axi0_rd_cnt 
echo axi0_rddata_cnt:$axi0_rddata_cnt

axi0_avg_wr_bust_len=$(expr "$axi0_wrdata_cnt" / "$axi0_wr_cnt")  
axi0_wrdata_cnt=$(expr "$axi0_wrdata_cnt" \* "16")  
axi0_wr_bw_MB=$(expr "$axi0_wrdata_cnt" / "$1")  
axi0_wr_bw_MB=$(expr "$axi0_wr_bw_MB" / "1000")

axi0_wr_valid_bw_MB=$(expr "$axi0_wrdata_byte" / "$1")
axi0_wr_valid_bw_MB=$(expr "$axi0_wr_valid_bw_MB" / "1000")
axi0_avg_rd_bust_len=$(expr "$axi0_rddata_cnt" / "$axi0_rd_cnt")  
axi0_rddata_cnt=$(expr "$axi0_rddata_cnt" \* "16")  
axi0_rd_bw_MB=$(expr "$axi0_rddata_cnt" / "$1")  
axi0_rd_bw_MB=$(expr "$axi0_rd_bw_MB" / "1000")

echo axi0_avg_wr_bust_len: $axi0_avg_wr_bust_len 
echo axi0_wr_bw_MB:$axi0_wr_bw_MB 
echo axi0_wr_valid_bw_MB: $axi0_wr_valid_bw_MB 
echo axi0_avg_rd_bust_len:$axi0_avg_rd_bust_len 
echo axi0_rd_bw_MB:$axi0_rd_bw_MB
echo


#axi1
axi1_wr_cnt=$(get_reg_bits 0x06040040  0 31)
axi1_wrdata_cnt=$(get_reg_bits 0x06040044  0 31) 
axi1_wrdata_byte_cnt_l=$(get_reg_bits 0x06040048  0 31) 
axi1_wrdata_byte_cnt_h=$(get_reg_bits 0x0604004c  0 31) 
let axi1_wrdata_byte_cnt_h=$(($axi1_wrdata_byte_cnt_h<<32))
let axi1_wrdata_byte=$(($axi1_wrdata_byte_cnt_l|$axi1_wrdata_byte_cnt_h))
axi1_rd_cnt=$(get_reg_bits 0x06040050  0 31) 
axi1_rddata_cnt=$(get_reg_bits 0x06040054  0 31) 

echo axi1_wr_cnt: $axi1_wr_cnt 
echo axi1_wrdata_cnt:$axi1_wrdata_cnt 
echo axi1_wrdata_byte_cnt: $axi1_wrdata_byte 
echo axi1_rd_cnt:$axi1_rd_cnt 
echo axi1_rddata_cnt:$axi1_rddata_cnt

axi1_avg_wr_bust_len=$(expr "$axi1_wrdata_cnt" / "$axi1_wr_cnt")  
axi1_wrdata_cnt=$(expr "$axi1_wrdata_cnt" \* "16")  
axi1_wr_bw_MB=$(expr "$axi1_wrdata_cnt" / "$1")  
axi1_wr_bw_MB=$(expr "$axi1_wr_bw_MB" / "1000") 

axi1_wr_valid_bw_MB=$(expr "$axi1_wrdata_byte" / "$1")
axi1_wr_valid_bw_MB=$(expr "$axi1_wr_valid_bw_MB" / "1000")
axi1_avg_rd_bust_len=$(expr "$axi1_rddata_cnt" / "$axi1_rd_cnt")  
axi1_rddata_cnt=$(expr "$axi1_rddata_cnt" \* "16")  
axi1_rd_bw_MB=$(expr "$axi1_rddata_cnt" / "$1")  
axi1_rd_bw_MB=$(expr "$axi1_rd_bw_MB" / "1000")  

echo axi1_avg_wr_bust_len: $axi1_avg_wr_bust_len 
echo axi1_wr_bw_MB:$axi1_wr_bw_MB 
echo axi1_wr_valid_bw_MB: $axi1_wr_valid_bw_MB 
echo axi1_avg_rd_bust_len:$axi1_avg_rd_bust_len 
echo axi1_rd_bw_MB:$axi1_rd_bw_MB
echo


#axi2
axi2_wr_cnt=$(get_reg_bits 0x06050040  0 31)
axi2_wrdata_cnt=$(get_reg_bits 0x06050044  0 31) 
axi2_wrdata_byte_cnt_l=$(get_reg_bits 0x06050048  0 31) 
axi2_wrdata_byte_cnt_h=$(get_reg_bits 0x0605004c  0 31) 
let axi2_wrdata_byte_cnt_h=$(($axi2_wrdata_byte_cnt_h<<32))
let axi2_wrdata_byte=$(($axi2_wrdata_byte_cnt_l|$axi2_wrdata_byte_cnt_h))
axi2_rd_cnt=$(get_reg_bits 0x06050050  0 31) 
axi2_rddata_cnt=$(get_reg_bits 0x06050054  0 31) 

echo axi2_wr_cnt: $axi2_wr_cnt 
echo axi2_wrdata_cnt:$axi2_wrdata_cnt 
echo axi2_wrdata_byte_cnt: $axi2_wrdata_byte 
echo axi2_rd_cnt:$axi2_rd_cnt 
echo axi2_rddata_cnt:$axi2_rddata_cnt


axi2_avg_wr_bust_len=$(expr "$axi2_wrdata_cnt" / "$axi2_wr_cnt")  
axi2_wrdata_cnt=$(expr "$axi2_wrdata_cnt" \* "16")  
axi2_wr_bw_MB=$(expr "$axi2_wrdata_cnt" / "$1")  
axi2_wr_bw_MB=$(expr "$axi2_wr_bw_MB" / "1000")  

axi2_wr_valid_bw_MB=$(expr "$axi2_wrdata_byte" / "$1")
axi2_wr_valid_bw_MB=$(expr "$axi2_wr_valid_bw_MB" / "1000")
axi2_avg_rd_bust_len=$(expr "$axi2_rddata_cnt" / "$axi2_rd_cnt")  
axi2_rddata_cnt=$(expr "$axi2_rddata_cnt" \* "16")  
axi2_rd_bw_MB=$(expr "$axi2_rddata_cnt" / "$1")  
axi2_rd_bw_MB=$(expr "$axi2_rd_bw_MB" / "1000")  

echo axi2_avg_wr_bust_len: $axi2_avg_wr_bust_len 
echo axi2_wr_bw_MB:$axi2_wr_bw_MB 
echo axi2_wr_valid_bw_MB: $axi2_wr_valid_bw_MB 
echo axi2_avg_rd_bust_len:$axi2_avg_rd_bust_len 
echo axi2_rd_bw_MB:$axi2_rd_bw_MB
echo

echo sum: .........................................................................
echo total:ddr_wr_bw_MB=$ddr_wr_bw_MB   ddr_rd_bw_MB= $ddr_rd_bw_MB
echo axi0:axi0_wr_bw_MB=$axi0_wr_bw_MB  axi0_rd_bw_MB=$axi0_rd_bw_MB
echo axi1:axi1_wr_bw_MB=$axi1_wr_bw_MB  axi1_rd_bw_MB=$axi1_rd_bw_MB
echo axi2:axi2_wr_bw_MB=$axi2_wr_bw_MB  axi2_rd_bw_MB=$axi2_rd_bw_MB

echo "run ddr monitor end "








