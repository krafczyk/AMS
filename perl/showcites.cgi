#!/usr/bin/perl -w
#  $Id: showcites.cgi,v 1.2 2011/03/30 12:48:35 dmitrif Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
unshift @ARGV, "-N/cgi-bin/mon/rc.mon.cgi";


#activate CGI

my $html=new RemoteClient();


#get all info from the db

 my $ok=$html->ConnectDB();
    $ok=$html->listAll('cites');

 
#work
 
if (not $ok){
  $html->Warning();
}



