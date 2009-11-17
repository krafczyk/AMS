#!/usr/bin/perl -w
#  $Id: checkfs.cgi,v 1.8 2009/11/17 13:32:22 choutko Exp $
#use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/checkfs.cgi";


#activate CGI

my $html=new RemoteClient(1);


#get number of used hosts for each site

 my $ok=$html->ConnectOnlyDB();
    $ok=$html->CheckFS(1,1,1);
#    $ok=$html->CheckFS(1,1,1,'/MC');
 
#work
 
if (not $ok){
  $html->Warning();
}



