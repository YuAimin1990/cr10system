#!/usr/bin/perl -w
use strict;

my $SYNTAX;

$SYNTAX = <<"SYNTAX";
 ======================================
 -i code_file
 ======================================
SYNTAX

if(@ARGV == 0){
	print $SYNTAX;
	exit;
}

if(@ARGV !=1){
	print $SYNTAX;
	print "Error: wrong argument!\n";
	exit;
}

my $data_file = shift(@ARGV);

open FP_DATA,"<$data_file"  or die "Error:Can't open vel data file:$!\n";

open FP0,">ddr4_init_vel_32bit_file0"   or die "Error:Can't open vel file0:$!\n";
open FP1,">ddr4_init_vel_32bit_file1"   or die "Error:Can't open vel file1:$!\n";
#open FP2,">ddr4_init_vel_file2"   or die "Error:Can't open vel file2:$!\n";
#open FP3,">ddr4_init_vel_file3"   or die "Error:Can't open vel file3:$!\n";

my $cnt0=0;#counter in  8
my $cnt1;  #counter out 8

my $first_flag=1;

my $addr;
my $byte;
my @mem;


##########################
######### DATA ###########
##########################
$first_flag = 1;

while(<FP_DATA>) {
	
  chomp;
  ($addr,$byte)=/@(\w+)\s+(\w+)/;
  $addr=hex($addr)-0x10000000;
  
  
  my $idx = $addr%4;
  
  if($first_flag==1){
    
    $cnt1 = int($addr/4);
    &init($addr);
    
    if($idx>0){
      foreach(0..$idx-1){
    	$mem[$_]="00";
      }
      $cnt0=$idx;
    }
    
    $mem[$idx] = $byte;
    if($cnt0==3){$cnt0=0;}else{$cnt0++;}

    $first_flag = 0;
  
  }else{
    
    if((int($addr/4))!=$cnt1){

      $cnt1 = int($addr/4);
      
      if($cnt0!=0){
        foreach($cnt0..3){
          $mem[$_]="00";
	}
	$cnt0=0;
	&ddr4_vel(@mem);
      }
      
      &init($addr);
      
      if($idx>0){
        foreach(0..$idx-1){
          $mem[$_]="00";
        }
        $cnt0 = $idx;
      }
      $mem[$idx] = $byte;
      if($cnt0==3){$cnt0=0;}else{$cnt0++;}
      
    }else{
      
      if($cnt0<$idx){
        foreach($cnt0..$idx-1){
    	  $mem[$_]="00";
	}
	$cnt0 = $idx;
      }
      $mem[$idx] = $byte;
      if($cnt0==3){$cnt0=0;}else{$cnt0++;}
    
    }
  }

  if($cnt0==0){&ddr4_vel(@mem);}
}


if($cnt0!=0){
  foreach($cnt0..3){
    $mem[$_]="00";
  }
  $cnt0=0;
  &ddr4_vel(@mem);
}

close FP_DATA;

close FP0;
close FP1;
#close FP2;
#close FP3;


sub init{
  my $iaddr=shift;
  my $iaddr_4byte;

  $iaddr_4byte  =(int($iaddr/4));
  
  printf FP0 "@%x ", $iaddr_4byte;
  printf FP1 "@%x ", $iaddr_4byte;
#  printf FP2 "@%x ", $iaddr_8byte;
#  printf FP3 "@%x ", $iaddr_8byte;
}




sub ddr4_vel{
  my @d=@_;
  my $data0;
  my $data1;
  my $data2;
  my $data3;

  $data0 = $d[1].$d[0];
  $data1 = $d[3].$d[2];
#  $data2 = $d[5].$d[4];
#  $data3 = $d[7].$d[6];

  print FP0 "$data0","\n";
  print FP1 "$data1","\n";
#  print FP2 "$data2","\n";
#  print FP3 "$data3","\n";
}

