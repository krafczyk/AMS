#!/usr/bin/perl -w
#  $Id: calculateMips.o.cgi,v 1.2 2004/03/01 16:44:00 alexei Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/calculateMips.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);
my $ok=$html->ConnectOnlyDB();
   $ok=$html->calculateMips();






