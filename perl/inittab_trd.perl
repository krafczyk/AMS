#!/usr/local/bin/perl -w
$FILE="</home/choutko/TRD_mapping.csv";
    open FILE or die "ups\n";
open (FILEO,">/home/choutko/inittab_trd.txt");
while (<FILE>){
    if($_ =~/Crate/){
        next;
    }
    my @ladders=(7,8,8,9,9);
    my @args=split ',';
    my $crate=$args[0];
    my $udr=$args[1];
    my $ufe=$args[2];
    my $lay=$args[3]-1;
    my $lad=$args[4];
    my $left=int($args[5]);
    if($lad<0){
       $lad+=$ladders[$lay];
    }
    elsif($lad==0){
      print "!error! \n"
    }
    else{
       $lad+=$ladders[$lay]-1;
   }
    my $up=1;
    if($up){
        print FILEO "   for(int k=0;k<trdid::nute;k++){ \n";
        print FILEO "    _GetGeo[$crate][$udr][$ufe][k][0]=$lay*trdid::nute+k; \n";
        print FILEO "    _GetGeo[$crate][$udr][$ufe][k][1]=$lad; \n";
        print FILEO "    _GetGeo[$crate][$udr][$ufe][k][2]=$left; \n";
        print FILEO "   } \n";
    }
    else{
        print FILEO "   for(int k=0;k<trdid::nute;k++){ \n";
        print FILEO "    _GetGeo[$crate][$udr][$ufe][k][1]=$lay*trdid::nute+trdid::nute-1-k; \n";
        print FILEO "    _GetGeo[$crate][$udr][$ufe][k][1]=$lad; \n";
        print FILEO "    _GetGeo[$crate][$udr][$ufe][k][2]=$left; \n";
        print FILEO "   } \n";
    }
      
   }

close FILEO;
