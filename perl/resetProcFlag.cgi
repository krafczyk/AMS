#!/usr/bin/perl -w
#  $Id: resetProcFlag.cgi,v 1.3 2013/03/08 15:49:25 bshan Exp $
use strict;

use lib $ENV{Offline}.qw(/vdev/perl);
use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
unshift @ARGV, "-N/cgi-bin/mon/validateRuns.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);

my $ok = $html->ConnectOnlyDB();
   $ok = $html->resetFilesProcessingFlag(1);






