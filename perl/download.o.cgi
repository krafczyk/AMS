#!/usr/bin/perl -w
#  $Id: download.o.cgi,v 1.1 2002/08/07 08:08:07 alexei Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/download.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);


$html->Download();






