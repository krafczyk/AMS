#!/usr/bin/perl -w
#  $Id: parsejf.o.cgi,v 1.3 2011/03/30 12:48:35 dmitrif Exp $
#use Gtk;
use strict;
use lib;
#use lib qw(/var/www/cgi-bin/mon);
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

my $ok = $html->parseJournalFiles();






