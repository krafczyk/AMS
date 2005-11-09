#!/usr/bin/perl -w
#  $Id: validateRuns.o.cgi,v 1.4 2005/11/09 12:38:08 ams Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/validateRuns.o.cgi";
unshift @ARGV, "-r0";



my $nocgi=1;
my $html=new RemoteClient($nocgi);

$html->set_root_env();
$html->ValidateRuns();






