#!/usr/bin/perl -w
#  $Id: checkfs.cgi,v 1.17 2013/11/18 09:29:39 ams Exp $
#use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
unshift @ARGV, "-N/cgi-bin/mon/checkfs.cgi";


#activate CGI

my $html=new RemoteClient(1);


#get number of used hosts for each site

 my $ok=$html->ConnectOnlyDB();
    $ok=$html->CheckFS(1,1,1,'/Data');
    $ok=$html->CheckFS(1,1,1,'/MC');
 
#work
 
if (not $ok){
  $html->Warning();
}



