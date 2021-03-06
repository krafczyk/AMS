#!/usr/bin/perl -w
#  $Id: checkfs.cgi,v 1.12 2011/03/30 12:48:35 dmitrif Exp 
use strict;
use Carp;

my $output="/afs/cern.ch/ams/local/SCDBGlobal/";
    if(defined $ENV{SCDBGlobal}){
        $output=$ENV{SCDBGlobal};
    }
my $min=1305810000;
my $xmin=0;
my $max=4000000000;
my $len=86400*2;
my $v=0;
my $HelpTxt=" -min -max -len ";
$ENV{AMISERVER}="http://pcamss0.cern.ch:8081";
my $scdbp="/afs/cern.ch/ams/local/bin/timeout --signal 9 14400 /afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/exe/linux/ami2root.exe ";
my $tmout=10800;
my $force=0;

   foreach my $chop  (@ARGV){
    if($chop =~/^-min/){
      $xmin=unpack("x4 A*",$chop);
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

if($xmin>0){
$min=$xmin;
}
else{
 $min=1305810000;
}
my $maxtime=2000000000;
        if (my $answ = (system "/afs/cern.ch/ams/local/bin/timeout --signal 9 300 /afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/exe/linux/ami2root.exe 1900000000 2000000000 /tmp/t1.root.$$ /tmp/t1.root.$$ 60 1>/tmp/checkami.$$ 2>&1") != 0) {
        warn "scdb.perl-E-CouldNotGetMaxTime  \n";
        die " UnableToRead DB \n";
        }
        else{
        my $str = (system "less /tmp/checkami.$$|grep 'last time' 1>/tmp/strami.$$ 2>&1");
#   print "$str\n"; 

        open (TIME, "/tmp/strami.$$");
        my $time = <TIME>;
        my @in = split (/([ \n])/, $time);
#    print ("ami time is $in[8]\n"); 
        $maxtime=$in[8];
        print "scdb.perl-I-MaxTimeInDB $maxtime \n";
    }
        unlink "/tmp/t1.root.$$";
        unlink "/tmp/checkami.$$";
        unlink "/tmp/strami.$$";
         if(time()-$maxtime>86400*2){
             my $ctime=localtime(time());
             warn "scdb.perl-E-DataBaseNotUpdating-T-$ctime $maxtime\n";
             sleep 600;
             $max=4000000000;
             goto begin;
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
            my @tmpa=();
            foreach my $file (@files){
                if($file=~/^SCDB/ and $file=~/.root$/){
                    my @junk=split '\.',$file;
                    my $t1=$junk[1];
                    my $t2=$junk[2];
                    my $tmp={};
                    $tmp->{b}=$t1;
                    $tmp->{e}=$t2;
                    $tmp->{file}="$output/$file";
                    if($t2<$t1){
                        print "  Begin>End file found $file , removing \n";
                      system("mv  $output/$file $output/$file.0");
                    }
                    push @tmpa,$tmp;
                    
#                    if($t1<$tmin){
#                        $tmin=$t1;
#                    }
#                    if($t2>$tmax){
#                        $tmax=$t2;
#                    }
                }
            }
            my @tmpb= sort prio @tmpa;
            my $end=2000000000;
            foreach my $tmp (@tmpb){
                if ($tmp->{b}>$end){
                    $tmax=$end;
                    last;
                }
                else{
                    $tmax=$tmp->{e};
                }
                $end=$tmp->{e};
            }            
            if($tmax>=$min){
                $min=$tmax;
                print "scdb.perl-I-MinChangedTo $min \n";
            }
        
  
#  remove repetitive files
my $i=1;
while ($i <$#tmpb){
    if($tmpb[$i-1]->{e}>$tmpb[$i+1]->{b}){
    my $cmd ="rm $tmpb[$i]->{file}";
    print "Removing $tmpb[$i-1]->{e} $tmpb[$i+1]->{b} $cmd \n";
    system($cmd);
    $i=$i+1;
}
    $i=$i+1;
}
        }



my $overlap=$len/24;
my $beg=$min-$overlap;
my $end=$beg+$len;
if($end>$max){
    $end=$max;
}
    my $t1=time();
        if($max-$min<$len/2){
            warn "scdb.perl-W-NotEnoughData $max $min  \n";
            if($max==$maxtime){
                my $ctime=localtime(time());
                warn "scdb.perl-I-willSleep $overlap $ctime \n";
               sleep $overlap;
                $force=0;
               $max=4000000000;
               goto begin;
            }
            elsif($max<$maxtime){
             warn "scdb.perl-W-MaxTimeRedefined $max $maxtime \n";
                            $max=4000000000;
               goto begin;
            }
            else{
                die "scdb.perl-F-Stop $max $maxtime \n";
            }
        }


while ($beg<$end and $end<=$max){
    system("kinit -R");
    my $upd="/afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/perl/klist.py";
    system($upd);
    my $cmd=$scdbp." $beg $end /tmp/t.root.$$ /tmp/t.root.$$ $tmout 1>/tmp/getior.$$ 2>&1";
    $t1=time();
     my $i=system($cmd);
    if($i){
        if(($i&255) or ($i>>8)!=4){
                  my $ctime=localtime(time());
            warn " scdb.perl-E-UnableToCreateRootFile-$ctime $i $t1 $cmd \n";
#                 here add mail message
             sendmailmessage('vitali.choutko@cern.ch',  "scdb.perl-E-UnableToCreateRootFile-$ctime $i"," ");
             system("mv /tmp/getior.$$ /tmp/getior.$$.$t1 ");
            unlink "/tmp/t.root.$$";
             die " scdb.perl-E-UnableToCreateRootFile-$ctime $i $t1 $cmd \n";
            $max=4000000000;
            goto begin;
           
        }
        next;
    }
    else{
    my $t2=time();
    my $t21=$t2-$t1;
        print "$output/SCDB.$beg.$end.root Created by $t21 sec\n";
    }
    my $t2=time();
    $cmd="mv /tmp/t.root.$$  $output/SCDB.$beg.$end.root";
    $i=system($cmd);
             if($i){
                 my $ctime=localtime(time());
                 warn "scdb.perl-E-UnableToMove-$ctime $i $t2 $cmd \n";
#                here add mail message
                sendmailmessage('vitali.choutko@cern.ch',  "scdb.perl-E-UnableToMove-$ctime $i $t2 "," ");
                system("mv /tmp/getior.$$ /tmp/getior.$$.$t2 ");
                 die "scdb.perl-E-UnableToMove-$ctime $i $t2 $cmd \n";
                 next;
             }
    $t1=time();
    $beg=$end-$overlap;
    if($end<$max){
     $end=$beg+$len;
     if($end>$max){
#         $end=$max;
     }
    }
    else{
     $end=$beg+$len;
    }
    goto begin;
}
unlink "/tmp/getior.$$";
        $max=4000000000;
        goto begin;


sub prio{
    $a->{b} <=> $b->{b};
}

sub sendmailmessage{
    my $self=shift;
    my $add=shift;
    my $sub=shift;
    my $mes=shift;
    open MAIL, "| /usr/lib/sendmail -t -i"
#        or die "could not open sendmail: $!";
        or return 1;
     print MAIL <<END_OF_MESSAGE2;
To:  $add
Subject: $sub

$mes
END_OF_MESSAGE2
    close MAIL or return 1;
    return 0;
}
