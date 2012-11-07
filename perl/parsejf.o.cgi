#!/usr/bin/perl -w
#  $Id: parsejf.o.cgi,v 1.5 2012/11/07 10:01:17 ams Exp $
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






