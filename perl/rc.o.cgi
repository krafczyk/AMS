#!/usr/bin/perl -w
#  $Id: rc.o.cgi,v 1.1 2002/03/13 08:42:23 alexei Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/rc.o.cgi";


#activate CGI

my $html=new RemoteClient();


#get all info from the db

 my $ok=$html->Connect();

 
#work
 
if (not $ok){
  $html->Warning();
}



