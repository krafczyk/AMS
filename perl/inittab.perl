#!/usr/local/bin/perl -w
$FILE="</home/choutko/ladmap1.txt";
    open FILE or die "ups\n";
open (FILEO,">/home/choutko/inittab.txt");
while (<FILE>){
    if($_ =~/Octant/){
        next;
    }
    my @args=split ' ';
    my $layer=$args[0]-1;
    my $ladder=$args[1]-1;
    my $half=$args[2];
    my $sta=$args[3];
    my $crate=$args[4]-1;
    my $tdrs=$args[5];
    if($sta!=0){
     print FILEO  "       _GetHard[$layer][$ladder][$half][2]=$crate; //crate \n";
     print FILEO  "       _GetHard[$layer][$ladder][$half][0]=$tdrs; //side x \n";
     print FILEO  "       _GetHard[$layer][$ladder][$half][1]=$tdrs; //side y \n";
 }
   }

close FILEO;
