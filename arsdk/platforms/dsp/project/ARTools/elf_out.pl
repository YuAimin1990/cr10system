#!/usr/bin/perl -w
use strict;

my $ELFFILE;

$ELFFILE = <<"ELFFILE";
 ======================================
 file_name.elf
 ======================================
ELFFILE

if(@ARGV==0){
  print $ELFFILE;
  exit
}

if(@ARGV!=1){
  print "Error: wrong argument!\n";
  print $ELFFILE;
  exit;
}

my $file = shift(@ARGV);


system("cp $file test.elf");
system("./ceva-elf-objcopy -j .inttbl              -O ihex test.elf test_inttbl.hex");
system("./ceva-elf-objcopy -j .data.internal       -O ihex test.elf test_datain.hex");
system("./ceva-elf-objcopy -j .data.internal.clone -O ihex test.elf test_dataincl.hex");
system("./ceva-elf-objcopy -j .data.internal.ro    -O ihex test.elf test_datainro.hex");
system("./ceva-elf-objcopy -j .bss                 -O ihex test.elf test_bss.hex");
system("./ceva-elf-objcopy -j __STACK_SECT         -O ihex test.elf test_stack.hex");
system("./ceva-elf-objcopy -j .text                -O ihex test.elf test_text.hex");
system("./ceva-elf-objcopy -j .data.external       -O ihex test.elf test_dataex.hex");
system("./ceva-elf-objcopy -j .data.external.clone -O ihex test.elf test_dataexcl.hex");
system("./ceva-elf-objcopy -j .data.external.ro    -O ihex test.elf test_dataexro.hex");
system("./ceva-elf-objcopy -j __ARG_SECT           -O ihex test.elf test_arg.hex");
system("./ceva-elf-objcopy -j __MALLOC_SECT        -O ihex test.elf test_malloc.hex");
if(-e "test.elf")
{
system("rm -rf test.elf");
}

##=================================================================================
##                                 CODE SECTION
##=================================================================================
open(INTTBL,"<test_inttbl.hex") or die "Error: Unable to open the test_inttbl.hex!";

open(TEXT,"<test_text.hex")     or die "Error: Unable to open the test_text.hex";


open(CODE0,">test0C.out")       or die "Error: Unable to open the test0C.out!";

open(CODE1,">test1C.out")       or die "Error: Unable to open the test1C.out!";

my $read_inttbl;
my @inttbl_data;

my $addr_reg_0 = 0;
my $addr_reg_1 = 0;

while ($read_inttbl = <INTTBL>) 
{
      chomp $read_inttbl;
      #push @inttbl_data,$read_inttbl;
      my $inttbl_str    = substr($read_inttbl,3,4); 
      my $start_str     = substr($read_inttbl,1,2); 
      my $end_str       = substr($read_inttbl,7,2); 
      my $inttbl_len    = length($read_inttbl); 
      my $inttbl_bad    = hex($inttbl_str);
      my $a = 0;
	  #print $inttbl_len;
	    
      if(($start_str eq "02") && ($end_str eq "02"))
      {
            $addr_reg_0    = substr($read_inttbl ,9, 4);
            $addr_reg_1    = substr($read_inttbl ,3, 4);
            $addr_reg_0    = hex($addr_reg_0);
            $addr_reg_1    = hex($addr_reg_1);
            $addr_reg_0    = ($addr_reg_0<<4);
      }
      elsif(($start_str eq "02") && ($end_str eq "04"))
      {
            $addr_reg_0    = substr($read_inttbl ,9, 4);
            $addr_reg_1    = substr($read_inttbl ,3, 4);
            $addr_reg_0    = hex($addr_reg_0);
            $addr_reg_1    = hex($addr_reg_1);
            $addr_reg_0    = ($addr_reg_0<<16);
      }

      $inttbl_bad        = $inttbl_bad + $addr_reg_0;
      $inttbl_bad        = $inttbl_bad + $addr_reg_1;

      if(($start_str ne "00") && ($end_str eq "00")){
       
        my $inttbl_head = "C:";
        for($a = 9;$a < ($inttbl_len-3);$a=$a+2)
        {
			my $inttbl_code = substr($read_inttbl,$a,2);
			my $inttbl_hex  = sprintf("%08x",$inttbl_bad);

            print CODE0 $inttbl_head.$inttbl_hex." ".$inttbl_code."\n";   
	    
			$inttbl_bad     = $inttbl_bad + 1;
        }
      }
}


my $read_text;

my $addr_reg_c_0 = 0;
my $addr_reg_c_1 = 0;

while ($read_text = <TEXT>)
{
      chomp $read_text;
      #push @inttbl_data,$read_inttbl;
      my $text_str    = substr($read_text,3,4);
      my $start_tstr     = substr($read_text,1,2);
      my $end_tstr       = substr($read_text,7,2);
      my $text_len    = length($read_text);
      my $text_bad    = hex($text_str);
      my $b = 0;


      if(($start_tstr eq "02") && ($end_tstr eq "02"))
      {
            $addr_reg_c_0    = substr($read_text ,9, 4);
            $addr_reg_c_1    = substr($read_text ,3, 4);
            $addr_reg_c_0    = hex($addr_reg_c_0);
            $addr_reg_c_1    = hex($addr_reg_c_1);
            $addr_reg_c_0    = ($addr_reg_c_0<<4);
      }
      elsif(($start_tstr eq "02") && ($end_tstr eq "04"))
      {
            $addr_reg_c_0    = substr($read_text ,9, 4);
            $addr_reg_c_1    = substr($read_text ,3, 4);
            $addr_reg_c_0    = hex($addr_reg_c_0);
            $addr_reg_c_1    = hex($addr_reg_c_1);
            $addr_reg_c_0    = ($addr_reg_c_0<<16);
      }


      $text_bad        = $text_bad + $addr_reg_c_0;
      $text_bad        = $text_bad + $addr_reg_c_1;


      if(($start_tstr ne "00") && ($end_tstr eq "00")){
       
        my $text_head = "C:";
        for($b = 9;$b < ($text_len-3);$b=$b+2)
        {
			my $text_code = substr($read_text,$b,2);
			my $text_hex  = sprintf("%08x",$text_bad);
           
            print CODE1 $text_head.$text_hex." ".$text_code."\n";   
	    
			$text_bad     = $text_bad + 1;



        }
      }

}


close (INTTBL);
close (TEXT);
close (CODE0);
close (CODE1);

##=================================================================================
##                                 DATA SECTION
##=================================================================================
open(DATAIN,"<test_datain.hex")         or die "Error: Unable to open the test_datain.hex!";

open(DATAIO,"<test_datainro.hex")       or die "Error: Unable to open the test_datainro.hex";

open(DATAEX,"<test_dataex.hex")         or die "Error: Unable to open the test_dataex.hex!";

open(DATAEXIO,"<test_dataexro.hex")       or die "Error: Unable to open the test_dataexro.hex";

open(DATA0,">test0D.out")               or die "Error: Unable to open the test0D.out!";

open(DATA1,">test1D.out")               or die "Error: Unable to open the test1D.out";

my $read_datain;
my $addr_reg_d_0 = 0;
my $addr_reg_d_1 = 0;

while ($read_datain = <DATAIN>)
{
      chomp $read_datain;
      #push @inttbl_data,$read_inttbl;
      my $datain_str     = substr($read_datain,3,4);
      my $start_dstr     = substr($read_datain,1,2);
      my $end_dstr       = substr($read_datain,7,2);
      my $datain_len     = length($read_datain);
      my $datain_bad     = hex($datain_str);
      my $c = 0;
      

      if(($start_dstr eq "02") && ($end_dstr eq "02"))
      {
            $addr_reg_d_0    = substr($read_datain ,9, 4);
            $addr_reg_d_1    = substr($read_datain ,3, 4);
            $addr_reg_d_0    = hex($addr_reg_d_0);
            $addr_reg_d_1    = hex($addr_reg_d_1);
            $addr_reg_d_0    = ($addr_reg_d_0<<4);
      }
      elsif(($start_dstr eq "02") && ($end_dstr eq "04"))
      {
            $addr_reg_d_0    = substr($read_datain ,9, 4);
            $addr_reg_d_1    = substr($read_datain ,3, 4);
            $addr_reg_d_0    = hex($addr_reg_d_0);
            $addr_reg_d_1    = hex($addr_reg_d_1);
            $addr_reg_d_0    = ($addr_reg_d_0<<16);
      }
      
       $datain_bad        = $datain_bad + $addr_reg_d_0;
       $datain_bad        = $datain_bad + $addr_reg_d_1;


      if(($start_dstr ne "00") && ($end_dstr eq "00")){
       
        my $datain_head = "D:";
        for($c = 9;$c < ($datain_len-3);$c=$c+2)
        {
			my $datain_code = substr($read_datain,$c,2);
			my $datain_hex  = sprintf("%08x",$datain_bad);

            print DATA0 $datain_head.$datain_hex." ".$datain_code."\n";   
	    
			$datain_bad     = $datain_bad + 1;


        }
      }

}

my $read_dataio;
my $addr_reg_dio_0 = 0;
my $addr_reg_dio_1 = 0;


while ($read_dataio = <DATAIO>)
{
      chomp $read_dataio;
      #push @inttbl_data,$read_inttbl;
      my $dataio_str     = substr($read_dataio,3,4);
      my $start_ostr     = substr($read_dataio,1,2);
      my $end_ostr       = substr($read_dataio,7,2);
      my $dataio_len     = length($read_dataio);
      my $dataio_bad     = hex($dataio_str);
      my $d = 0;


      if(($start_ostr eq "02") && ($end_ostr eq "02"))
      {
            $addr_reg_dio_0    = substr($read_dataio ,9, 4);
            $addr_reg_dio_1    = substr($read_dataio ,3, 4);
            $addr_reg_dio_0    = hex($addr_reg_dio_0);
            $addr_reg_dio_1    = hex($addr_reg_dio_1);
            $addr_reg_dio_0    = ($addr_reg_dio_0<<4);
      }
      elsif(($start_ostr eq "02") && ($end_ostr eq "04"))
      {
            $addr_reg_dio_0    = substr($read_dataio ,9, 4);
            $addr_reg_dio_1    = substr($read_dataio ,3, 4);
            $addr_reg_dio_0    = hex($addr_reg_dio_0);
            $addr_reg_dio_1    = hex($addr_reg_dio_1);
            $addr_reg_dio_0    = ($addr_reg_dio_0<<16);
      }

     $dataio_bad        = $dataio_bad + $addr_reg_dio_0;
     $dataio_bad        = $dataio_bad + $addr_reg_dio_1;


      if(($start_ostr ne "00") && ($end_ostr eq "00")){
       
        my $dataio_head = "D:";
        for($d = 9;$d < ($dataio_len-3);$d=$d+2)
        {
			my $dataio_code = substr($read_dataio,$d,2);
			my $dataio_hex  = sprintf("%08x",$dataio_bad);

            print DATA0 $dataio_head.$dataio_hex." ".$dataio_code."\n";   
	    
			$dataio_bad     = $dataio_bad + 1;



        }
      }

}



my $read_dataex;
my $addr_reg_dex_0 = 0;
my $addr_reg_dex_1 = 0;


while ($read_dataex = <DATAEX>)
{
      chomp $read_dataex;
      #push @inttbl_data,$read_inttbl;
      my $dataex_str     = substr($read_dataex,3,4);
      my $startex_dstr     = substr($read_dataex,1,2);
      my $endex_dstr       = substr($read_dataex,7,2);
      my $dataex_len     = length($read_dataex);
      my $dataex_bad     = hex($dataex_str);
      my $cex = 0;
      
      if(($startex_dstr eq "02") && ($endex_dstr eq "02"))
      {
            $addr_reg_dex_0    = substr($read_dataex ,9, 4);
            $addr_reg_dex_1    = substr($read_dataex ,3, 4);
            $addr_reg_dex_0    = hex($addr_reg_dex_0);
            $addr_reg_dex_1    = hex($addr_reg_dex_1);
            $addr_reg_dex_0    = ($addr_reg_dex_0<<4);
      }
      elsif(($startex_dstr eq "02") && ($endex_dstr eq "04"))
      {
            $addr_reg_dex_0    = substr($read_dataex ,9, 4);
            $addr_reg_dex_1    = substr($read_dataex ,3, 4);
            $addr_reg_dex_0    = hex($addr_reg_dex_0);
            $addr_reg_dex_1    = hex($addr_reg_dex_1);
            $addr_reg_dex_0    = ($addr_reg_dex_0<<16);
      }

       $dataex_bad        = $dataex_bad + $addr_reg_dex_0;
       $dataex_bad        = $dataex_bad + $addr_reg_dex_1;


      if(($startex_dstr ne "00") && ($endex_dstr eq "00")){
       
        my $dataex_head = "D:";
        for($cex = 9;$cex < ($dataex_len-3);$cex=$cex+2)
        {
			my $dataex_code = substr($read_dataex,$cex,2);
			my $dataex_hex  = sprintf("%08x",$dataex_bad);

            print DATA1 $dataex_head.$dataex_hex." ".$dataex_code."\n";   
	    
			$dataex_bad     = $dataex_bad + 1;
           

        }
      }

}

my $read_dataexio;
my $addr_reg_dexio_0 = 0;
my $addr_reg_dexio_1 = 0;


while ($read_dataexio = <DATAEXIO>)
{
      chomp $read_dataexio;
      #push @inttbl_data,$read_inttbl;
      my $dataexio_str     = substr($read_dataexio,3,4);
      my $startex_ostr     = substr($read_dataexio,1,2);
      my $endex_ostr       = substr($read_dataexio,7,2);
      my $dataexio_len     = length($read_dataexio);
      my $dataexio_bad     = hex($dataexio_str);
      my $dex = 0;

      if(($startex_ostr eq "02") && ($endex_ostr eq "02"))
      {
            $addr_reg_dexio_0    = substr($read_dataexio ,9, 4);
			$addr_reg_dexio_1    = substr($read_dataexio ,3, 4);
            $addr_reg_dexio_0    = hex($addr_reg_dexio_0);
			$addr_reg_dexio_1    = hex($addr_reg_dexio_1);
			$addr_reg_dexio_0    = ($addr_reg_dexio_0<<4);
      }
      elsif(($startex_ostr eq "02") && ($endex_ostr eq "04"))
      {
            $addr_reg_dexio_0    = substr($read_dataexio ,9, 4);
            $addr_reg_dexio_1    = substr($read_dataexio ,3, 4);
            $addr_reg_dexio_0    = hex($addr_reg_dexio_0);
            $addr_reg_dexio_1    = hex($addr_reg_dexio_1);
            $addr_reg_dexio_0    = ($addr_reg_dexio_0<<16);
      }
            
	  $dataexio_bad        = $dataexio_bad + $addr_reg_dexio_0;
      $dataexio_bad        = $dataexio_bad + $addr_reg_dexio_1;

      

      if(($startex_ostr ne "00") && ($endex_ostr eq "00")){
       
        my $dataexio_head = "D:";
        for($dex = 9;$dex < ($dataexio_len-3);$dex=$dex+2)
        {
			my $dataexio_code = substr($read_dataexio,$dex,2);
			my $dataexio_hex  = sprintf("%08x",$dataexio_bad);

            print DATA1 $dataexio_head.$dataexio_hex." ".$dataexio_code."\n";   
	    
			$dataexio_bad     = $dataexio_bad + 1;
	   

        }
      }

}

close (DATAIN);
close (DATAIO);
close (DATAEX);
close (DATAEXIO);
close (DATA0);
close (DATA1);

system("mv test0C.out ./coff2binary/");
system("mv test0D.out ./coff2binary/");
system("mv test1C.out ./coff2binary/");
system("mv test1D.out ./coff2binary/");

system("rm -rf *.hex");

system("./file_splice.sh test");

