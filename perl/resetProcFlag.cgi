#!/usr/bin/perl -w
#  $Id: resetProcFlag.cgi,v 1.1 2012/03/26 14:57:47 choutko Exp $
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
unshift @ARGV, "-N/cgi-bin/mon/validateRuns.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);

my $ok = $html->ConnectOnlyDB();
   $ok = $html->resetFilesProcessingFlag();






