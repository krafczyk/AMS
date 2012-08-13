#!/usr/bin/perl -w
#  $Id: prestage.cgi,v 1.1 2012/08/13 10:23:43 ams Exp $
#use Gtk;
use strict;


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
my $p=0;
my $run2p=0;
my $tmp="/tmp/castor";
my $irm=0;
my $dir="/f5dah1";
my $new=undef;
 my $HelpTxt = "
     RemoteClient::MoveBetweenDisks
#
# prestage files
# 
# 
# 
# input par:
#  -dPath:   path to local files like /disk/MC/AMS02/2005A/dir
#                                    /MC  /dir are optional ones
#  -nDisk:   new disk  /disk (optional) 
#  -p   prestage
#  -v   verbose mode 
#  -u    do sql/file rm  
#  -i        rm -i  
#  -tTmp  path to temporarily storage castorfiles
#  -rRun   only process run Run if not 0
#  -rRunMin-Runmax 
#     example
     ./movefiles.cgi -v -d/f0dah1/MC/AMS02/2004A -n/s0dah1 -u -i
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

    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
 }

$html->CastorPrestage($dir,$v,$run2p);







