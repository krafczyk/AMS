#!/usr/bin/perl -w
#  $Id: checkfs.cgi,v 1.12 2011/03/30 12:48:35 dmitrif Exp 
use strict;
my $output="/afs/cern.ch/ams/local/SCDBGlobal/";
    if(defined $ENV{SCDBGlobal}){
        $output=$ENV{SCDBGlobal};
    }
my $min=1305810000;
my $max=4000000000;
my $len=86400*2;
my $v=0;
my $HelpTxt=" -min -max -len ";
$ENV{AMISERVER}="http://pcamss0.cern.ch:8081";
my $scdbp="/afs/cern.ch/ams/local/bin/timeout --signal 9 3600 /afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/exe/linux/ami2root.exe ";
my $tmout=1200;
my $force=0;

   foreach my $chop  (@ARGV){
    if($chop =~/^-min/){
      $min=unpack("x4 A*",$chop);
    }
    if($chop =~/^-force/){
        $force=1;
    }
    if($chop =~/^-len/){
      $len=unpack("x4 A*",$chop);
    }
      if ($chop =~/^-v/) {
        $v=1;
      }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
}
begin:

#check max time in db

my $maxtime=2000000000;
        if (my $answ = (system "/afs/cern.ch/ams/local/bin/timeout --signal 9 300 /afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/exe/linux/ami2root.exe 1900000000 2000000000 /tmp/t.root.$$ /tmp/t.root.$$ 60 1>/tmp/checkami.$$ 2>&1") != 0) {
        print "scdb.perl-E-CouldNotGetMaxTime  \n";
        die;
        }
        else{
        my $str = (system "less /tmp/checkami.$$|grep 'last time' 1>/tmp/strami.$$ 2>&1");
#   print "$str\n"; 

        open (TIME, "/tmp/strami.$$");
        my $time = <TIME>;
        my @in = split (/ /, $time);
#    print ("ami time is $in[4]\n"); 
        $maxtime=$in[4];
        print "scdb.perl-I-MaxTimeInDB $maxtime \n";
    }
        unlink "/tmp/t.root.$$";
        unlink "/tmp/checkami.$$";
        unlink "/tmp/strami.$$";
         if(time()-$maxtime>86400*2){
             die "scdb.perl-F-DataBaseNotUpdating, Exiting \n";
         }
        if($max>$maxtime){
            $max=$maxtime;
        }

# check mintime available of no $force

        if(!$force){
            my $tmin=2000000000;
            my $tmax=0;
            opendir THISDIR,$output or die "scdb.perl-F-UnableToOpen $output \n";
            my @files=readdir THISDIR;
            closedir THISDIR;
            foreach my $file (@files){
                if($file=~/^SCDB/ and $file=~/.root$/){
                    my @junk=split '\.',$file;
                    my $t1=$junk[1];
                    my $t2=$junk[2];
                    if($t1<$tmin){
                        $tmin=$t1;
                    }
                    if($t2>$tmax){
                        $tmax=$t2;
                    }
                }
            }
            if($tmin<=$min){
                $min=$tmax;
                print "scdb.perl-I-MinChangedTo $min \n";
            }
        }





my $overlap=$len/12;
my $beg=$min-$overlap;
my $end=$beg+$len;
if($end>$max){
    $end=$max;
}
    my $t1=time();
        if($max-$min<$len-$overlap){
            print "scdb.perl-I-NotEnoughData $max $min  \n";
            if($max==$maxtime){
                print "scdb.perl-I-willSleep $overlap \n";
               sleep $overlap;
                $force=0;
               goto begin;
            }
            else{
                die "scdb.perl-I-Stop \n";
            }
        }


while ($beg<$end and $end<=$max){
    my $cmd=$scdbp." $beg $end /tmp/t.root.$$ /tmp/t.root.$$ $tmout 1>/tmp/getior.$$ 2>&1";
    my $i=system($cmd);
    if($i){
        if(($i&255) or ($i>>8)!=4){
            print " scdb.perl-E-UnableTo $cmd \n";
#                 here add mail message
                  die "Exiting";
        }
        next;
    }
    else{
    my $t2=time();
    my $t21=$t2-$t1;
        print "$output/SCDB.$beg.$end.root Created by $t21 sec\n";
    }
    $cmd="mv /tmp/t.root.$$  $output/SCDB.$beg.$end.root";
    $i=system($cmd);
             if($i){
                 print "scdb.perl-E-UnableToMove $cmd \n";
#                here add mail message
                 next;
             }
    $t1=time();
    $beg=$end-$overlap;
    if($end<$max){
     $end=$beg+$len;
     if($end>$max){
         $end=$max;
     }
    }
    else{
     $end=$beg+$len;
    }
}
unlink "/tmp/getior.$$";
        goto begin;
