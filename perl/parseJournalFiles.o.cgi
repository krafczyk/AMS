#!/usr/bin/perl -w
#  $Id: parseJournalFiles.o.cgi,v 1.1 2003/04/28 11:58:49 alexei Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/validateRuns.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);


$html->parseJournalFiles();






