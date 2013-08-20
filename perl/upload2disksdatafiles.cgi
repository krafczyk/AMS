#!/usr/bin/perl -w
use strict;
use lib $ENV{'Offline'}.qw(/vdev/perl);
use lib::RemoteClient;

my $nocgi=1;
my $html=new RemoteClient($nocgi);
my $ok=$html->ConnectDB();
my $v=1;
my $upd=1;
my $run2p=0;
my $dir="/Data/AMS02/2011A/RawData";
 my $HelpTxt = '
#
#  Copy castor datafiles to disks only if corr disk is damaged
#  Updates catalogs
#
# input par: #  $dir:   path to castor files like MC/AMS02/2005A/dir
#                                     /dir are optional ones
#  $verbose   verbose if 1
#  $update    do sql/file rm  if 1
#  $run2p   only process run $run2p if not 0
#  output par:
#   0 if ok  1 otherwise
#
# 
#
';
   foreach my $chop  (@ARGV){
    if($chop =~/^-d/){
      $dir=unpack("x2 A*",$chop);
    }

    if ($chop =~/^-r/) {
        $run2p=unpack("x2 A*",$chop);
    }
      if ($chop =~/^-v/) {
        $v=1;
      }
      if ($chop =~/^-u/) {
        $upd=1;
      }

    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      exit();
    }
 }

$html->UploadToDisksDataFiles($dir,$v,$upd,$run2p);
