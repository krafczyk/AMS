#!/usr/bin/perl -w
#  $Id: parsejf.data.cgi,v 1.1 2013/03/08 15:22:56 bshan Exp $
#use Gtk;
use strict;
use lib;
use lib qw(/afs/cern.ch/ams/Offline/vdev/perl);
use lib::RemoteClient;

$SIG{INT}=sub{exit()};
$SIG{QUIT}=sub{exit()}; 
my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV,"-m ";
unshift @ARGV, "-Fpdb_ams";
unshift @ARGV, "-N/cgi-bin/mon/parseJournalRuns.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);
my $ok = $html->parseJournalFiles(1);






