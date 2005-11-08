#!/usr/bin/perl -w
#  $Id: parseJournalFiles.o.cgi,v 1.7 2005/11/08 13:31:57 ams Exp $
#use Gtk;
use strict;


use lib::RemoteClient;

$SIG{INT}=sub{exit()};
$SIG{QUIT}=sub{exit()}; 
my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV,"-m ";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/parseJournalRuns.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);

my $ok = $html->parseJournalFiles();






