#!/usr/bin/perl -w
#  $Id: checkdb.cgi,v 1.1 2004/02/18 10:38:28 alexei Exp $
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

 my $ok=$html->ConnectDB();
    $ok=$html->checkDB();

 
#work
 
if (not $ok){
  $html->Warning();
}



