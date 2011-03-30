#!/usr/bin/perl -w
#  $Id: stopParseJournalFiles.o.cgi,v 1.5 2011/03/30 12:48:34 dmitrif Exp $
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






