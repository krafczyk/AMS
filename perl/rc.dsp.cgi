#!/usr/bin/perl -w
#  $Id: rc.dsp.cgi,v 1.2 2002/03/13 18:07:31 alexei Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/rc.mon.cgi";


#activate CGI

my $html=new RemoteClient();


#get all info from the db

 my $ok=$html->ConnectDB();
    $ok=$html->listAll('all');

 
#work
 
if (not $ok){
  $html->Warning();
}



