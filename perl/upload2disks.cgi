#!/usr/bin/perl -w
#  $Id: upload2disks.cgi,v 1.7 2013/03/08 15:47:09 bshan Exp $
use strict;
use lib $ENV{'Offline'}.qw(/vdev/perl);
use lib::RemoteClient;
my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
unshift @ARGV, "-N/cgi-bin/mon/validateRuns.o.cgi";
unshift @ARGV,"-zero";


my $nocgi=1;
my $html=new RemoteClient($nocgi);
my $ok=$html->ConnectDB();
my $v=1;
my $upd=1;
my $run2p=0;
my $dir="/Data/AMS02/2011B";
 my $HelpTxt = "
     RemoteClient::UploadToCastor
#
#
#  uploads file to disk
#  input paramaters
#  -d :   path  to castor files like /MC/AMS02/2005A/dir
#                                   /dir are optional ones
#  -v   verbose if 1 
#  -u     do sql  if 1
#  -r   only process run $run2p if not 0
#  output par:
#   1 if ok  0 otherwise
#
# 
#     example
     ./upload2disks.cgi -v -u -d/MC/AMS02/2004A 
";
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

$html->UploadToDisks($dir,$v,$upd,$run2p);







