#!/usr/bin/perl -w
#  $Id: validateRuns.o.cgi,v 1.1 2004/02/27 10:09:58 alexei Exp $
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
$html->ValidateRuns();






