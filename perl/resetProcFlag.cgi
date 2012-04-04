#!/usr/bin/perl -w
#  $Id: resetProcFlag.cgi,v 1.2 2012/04/04 09:50:09 ams Exp $
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
unshift @ARGV, "-N/cgi-bin/mon/validateRuns.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);

my $ok = $html->ConnectOnlyDB();
   $ok = $html->resetFilesProcessingFlag(1);






