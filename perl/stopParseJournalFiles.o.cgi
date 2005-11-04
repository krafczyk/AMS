#!/usr/bin/perl -w
#  $Id: stopParseJournalFiles.o.cgi,v 1.4 2005/11/04 20:10:03 ams Exp $
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/validateRuns.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);

my $ok = $html->ConnectOnlyDB();
   $ok = $html->resetFilesProcessingFlag();






