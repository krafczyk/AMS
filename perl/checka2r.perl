#!/usr/bin/perl -w
#  $Id: checkfs.cgi,v 1.12 2011/03/30 12:48:35 dmitrif Exp 
use strict;
use Carp;

my $output="/afs/cern.ch/ams/Offline/AMSDataDir/SlowControlDir/";
    if(defined $ENV{SCDBGlobal}){
        $output=$ENV{SCDBGlobal};
    }
my $min=1305810000;
my $max=4000000000;
my $ref="SCDB.1336410654.1336521957.root";
my $v=0;

my $HelpTxt=" -ref -force -v ";
$ENV{AMISERVER}="http://pcamss0.cern.ch:8081";
my $scdbp="/afs/cern.ch/ams/local/bin/timeout --signal 9 14400 /afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/exe/linux/checka2r.exe ";
my $force=0;

   foreach my $chop  (@ARGV){
    if($chop =~/^-ref/){
      $ref=unpack("x4 A*",$chop);
    }
    if($chop =~/^-force/){
        $force=1;
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

            opendir THISDIR,$output or die "checka2r.perl-F-UnableToOpen $output \n";
            my @files=readdir THISDIR;
            closedir THISDIR;
            my @tmpa=sort @files;
            my @jref=split '\.',$ref;
            my $begin=$jref[1];
            my $end=$jref[2];
            my $fileref=$output."/".$ref;
            foreach my $file (@tmpa){
                if($file=~/^SCDB/ and $file=~/.root$/){
                    my @junk=split '\.',$file;
                    my $t1=$junk[1];
                    my $t2=$junk[2];
                    my $cmd=$scdbp." $begin $end $fileref $t1 $t2 $output/$file 1>/tmp/checka2r.$$ 2>&1";                
                    my $i=system($cmd);
                    if(($i&255) or ($i>>8)){
                      my $j=($i>>8);
                      open (TIME, "/tmp/checka2r.$$");
                      while (my $line=<TIME>){
                         if ($line=~/^ERROR/){
                           print "$line";
                         }
                         if($v and $line=~/table size 0/){
                           print "$line";
                         }
                      }
                      close TIME;
                      print " Error $j  $output/$file \n"
                      if($force){
                          $cmd="mv $output/$file $output/$file.out";
                          system($cmd);
                      }                       
                      }
                      else{
                        print " OK $output/$file \n"
                      }
                      unlink "/tmp/checka2r.$$"; 
                }
            }


