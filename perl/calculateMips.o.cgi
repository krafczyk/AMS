#!/usr/bin/perl -w
#  $Id: calculateMips.o.cgi,v 1.1 2003/10/02 10:10:23 alexei Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/calculateMips.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);
my $ok=$html->ConnectDB();
   $ok=$html->calculateMips();






