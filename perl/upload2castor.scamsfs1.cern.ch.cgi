#!/usr/bin/perl -w
#  $Id: upload2castor.scamsfs1.cern.ch.cgi,v 1.3 2013/11/24 22:53:58 bshan Exp $
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
my $cmp=1;
my $validate=0;
my $dir="AMS01/2006B";
my $mb=2000000;
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
#  -mxxx process up to xxx   mbytes
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
       if($chop =~/^-m/){
      $mb=unpack("x2 A*",$chop);
    }
    if($chop =~/^-c/){
      $cmp=0;
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

$dir="AMS02/2014";
$html->UploadToCastor($dir,$v,$upd,$cmp,$run2p,$mb,20,1,0,$validate);
print "$dir Done \n";




$dir="AMS02/2011A";
$html->UploadToCastor($dir,$v,$upd,$cmp,$run2p,$mb,2,3,0,$validate);

print "$dir Done \n";

$html->UploadToCastor($dir,$v,$upd,$cmp,$run2p,$mb,20,1,0,$validate);
print "$dir Done \n";


$dir="AMS02/2011B";
$html->UploadToCastor($dir,$v,$upd,$cmp,$run2p,$mb,20,1,0,$validate);
print "$dir Done \n";
$html->UploadToCastor($dir,$v,$upd,$cmp,$run2p,$mb,2,3,0,$validate);
print "$dir Done \n";








