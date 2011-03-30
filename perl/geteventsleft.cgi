#!/usr/bin/perl -w
#  $Id: geteventsleft.cgi,v 1.2 2011/03/30 12:48:35 dmitrif Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
unshift @ARGV, "-N/cgi-bin/mon/rc.mon.cgi";


#activate CGI

my $nocgi = 1;
my $html=new RemoteClient($nocgi);


$html->getEventsLeft();

 



