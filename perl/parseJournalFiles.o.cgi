#!/usr/bin/perl -w
#  $Id: parseJournalFiles.o.cgi,v 1.2 2003/05/23 08:32:42 alexei Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/validateRuns.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);

$html->set_root_env();
$html->parseJournalFiles();






