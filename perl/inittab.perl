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
    my $octant=$args[4]-1;
    my $tdrs=$args[5];
    if($sta!=0){
       if($layer==0){
        $ladder=14-$ladder;
       }
       elsif($layer==2){
        $ladder=11-$ladder;
       }
       elsif($layer==4){
        $ladder=9-$ladder;
       }
       elsif($layer==6){
        $ladder=13-$ladder;
       }
        my $crate;
        if($octant==0){
            $crate=0;
        }
        elsif($octant==1){
            $crate=1;
        }
        elsif($octant==2){
            $crate=3;
        }
        elsif($octant==3){
            $crate=2;
        }
        elsif($octant==4){
            $crate=4;
        }
        elsif($octant==5){
            $crate=5;
        }
        elsif($octant==6){
            $crate=7;
        }
        elsif($octant==7){
            $crate=6;
        }
           
     print FILEO  "       _GetHard[$layer][$ladder][$half][2]=$crate; //crate \n";
     print FILEO  "       _GetHard[$layer][$ladder][$half][0]=$tdrs; //side x \n";
     print FILEO  "       _GetHard[$layer][$ladder][$half][1]=$tdrs; //side y \n";
 }
   }

close FILEO;
