#!/usr/bin/perl -w
#  $Id: deleteDST.cgi,v 1.1 2004/06/28 15:40:20 alexei Exp $
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
    $ok=$html->deleteDST();

 
#work
 
if (not $ok){
  $html->Warning();
}



