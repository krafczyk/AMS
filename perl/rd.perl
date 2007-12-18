#!/usr/bin/perl -w
#  $Id: rd.perl,v 1.1 2007/12/18 10:13:41 choutko Exp $
use strict;

use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";



my $nocgi=1;
my $o=new RemoteClient(1);
my $ok=$o->ConnectOnlyDB();
if($ok){
    my $runmin=0;
    my $runmax=2000000000;
    my $sql = "SELECT run, path,fetime, nevents, tag, sizemb FROM amsdes.datafiles WHERE run>$runmin AND run<$runmax ORDER BY run";
         my $ret=$o->{sqlserver}->Query($sql);
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



