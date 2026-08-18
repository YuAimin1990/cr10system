#!/usr/bin/perl -w
use strict;

my $HEXFILE;

$HEXFILE = <<"HEXFILE";
 ======================================
 file_name.hex
 ======================================
HEXFILE

if(@ARGV==0){
  print $HEXFILE;
  exit
}

if(@ARGV!=2){
  print "Error: wrong argument!\n";
  print $HEXFILE;
  exit;
}

my $file      = shift(@ARGV);
my $bin_addr  = shift(@ARGV);

system("cp $file test.hex");

open(BIN,"<test.hex")         or die "Error: Unable to open the test.hex!";
open(BIT,">test0B.out")       or die "Error: Unable to open the test0B.out!";

my $read_bin;
my @bin_data;
my $bin_base        = hex($bin_addr);

while ($read_bin = <BIN>)
{
      chomp $read_bin;
      #push @inttbl_data,$read_inttbl;
   
      my $bin_len        = length($read_bin);
      my $a = 0;

      #if(($start_str ne "00") && ($end_str eq "00")){
       
        my $bin_head = "B:";
        for($a = ($bin_len-2);$a >= 0;$a=$a-2)
        {
	    my $bin_code = substr($read_bin,$a,2);
	    my $bin_hex  = sprintf("%08x",$bin_base);

            print BIT $bin_head.$bin_hex." ".$bin_code."\n";   
	    
	    $bin_base   = $bin_base + 1;
        }
      #}

}

close (BIN);
close (BIT);

if(-e "test.hex")
{
system("rm -rf test.hex");
}

&init_file("test0B.out");

system("./ddr4_vel_for_bin.pl test0B_mem");

sub init_file{
  my $filename = shift(@_);
  my $type;
  my $other;
  
  my $name = $filename;
  $filename=~s/(.+)\.out/$1/;
  
  if(-e $name){
    print "\n$name\n";
    #system("./sorthex $name ${filename}_sort");
    
    open FP1,"<${filename}.out"  or die "Error! Fail to open ${filename}.out:$!\n";
    open FP2,">${filename}_mem"  or die "Error! Fail to open ${filename}_mem :$!\n";

    while(<FP1>){
      chomp;
      ($type,$other)=/([CDB]:)(.+)/;
      print FP2 "\@$other\n";
    }

    close FP1;
    close FP2;
  
  }else{
    system("touch ${filename}_mem");
  }
}

