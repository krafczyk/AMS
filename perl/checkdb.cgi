#!/usr/bin/perl -w
#  $Id: checkdb.cgi,v 1.2 2004/03/05 19:04:19 alexei Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/checkdb.cgi";


#activate CGI

my $html=new RemoteClient();


#get number of used hosts for each site

 my $ok=$html->ConnectOnlyDB();
    $ok=$html->checkDB();

 
#work
 
if (not $ok){
  $html->Warning();
}



