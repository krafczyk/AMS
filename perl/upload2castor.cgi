#!/usr/bin/perl -w
#  $Id: upload2castor.cgi,v 1.1 2005/11/15 17:29:37 ams Exp $
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/validateRuns.o.cgi";
unshift @ARGV,"-zero";


my $nocgi=1;
my $html=new RemoteClient($nocgi);
my $ok=$html->ConnectDB();
my $v=1;
my $upd=1;
my $run2p=1387;
my $cmp=1;
my $dir="AMS01/2004B";
 my $HelpTxt = "
     RemoteClient::UploadToCastor
#
#
#  uploads file to castor
#  input paramaters
#  -d :   path to local files like /disk/MC/AMS02/2005A/dir
#                                   /disk /MC  /dir are optional ones
#  -v   verbose if 1 
#  -u     do sql  if 1
#  -c       compare castor sizes with local if 1 
#  -r   only process run $run2p if not 0
#
#  output par:
#   1 if ok  0 otherwise
#
# 
#     example
     ./upload2castor.cgi -v -d/f0dah1/MC/AMS02/2004A 
";
   foreach my $chop  (@ARGV){
    if($chop =~/^-d/){
      $dir=unpack("x2 A*",$chop);
    }
    if($chop =~/^-c/){
      $cmp=unpack("x2 A*",$chop);
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
      return 1;
    }
 }

$html->UploadToCastor($dir,$v,$upd,$cmp,$run2p);






