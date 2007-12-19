#!/usr/local/bin/perl -w
#  $Id: rd.perl,v 1.2 2007/12/19 17:27:16 choutko Exp $
use strict;

use lib::DBSQLServer;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";



    my $o=new DBSQLServer();
     my $ok=$o->ConnectRO();
if($ok){
    my $runmin=0;
    my $runmax=2000000000;
    my $sql = "SELECT run, path,fetime, nevents, tag, sizemb FROM amsdes.datafiles WHERE run>$runmin AND run<$runmax ORDER BY run";
         my $ret=$o->Query($sql);
         if (defined $ret->[0][0]) {
          foreach my $r (@{$ret}){
             my $run       = $r->[0];
             my $path      = $r->[1];
             my $starttime = $r->[2];
             my $nevents   = $r->[3];
             my $tag   = $r->[4];
             my $sizemb   = $r->[5];
             print "$run,$path,$starttime,$nevents,$tag,$sizemb \n";
         }
      }
}



