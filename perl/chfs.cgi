#!/usr/bin/perl -w
#  $Id: chfs.cgi,v 1.4 2011/03/30 12:48:34 dmitrif Exp $
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
    $ok=$html->CheckFS(1,1,1,'/');
 
#work
 
if (not $ok){
  $html->Warning("not ok");
}



