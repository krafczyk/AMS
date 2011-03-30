#!/usr/bin/perl -w
#  $Id: updateHostClock.o.cgi,v 1.2 2011/03/30 12:48:34 dmitrif Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
unshift @ARGV, "-N/cgi-bin/mon/updateHostClock.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);

$html->updateHostInfo();






