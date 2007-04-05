#!/usr/bin/perl -w
#  $Id: validateRuns.o.cgi,v 1.6 2007/04/05 09:46:38 ams Exp $
use Gtk;
use strict;

#use lib qw(/var/www/cgi-bin/mon);
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






