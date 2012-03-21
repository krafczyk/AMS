#!/usr/bin/perl -w
#  $Id: chfs.cgi,v 1.5 2012/03/21 15:43:15 choutko Exp $
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
    $ok=$html->CheckFS(1,1,1,'/MC');
    print "Data \n";
    sleep(10);
    $ok=$html->CheckFS(1,1,1,'/Data');
 
#work
 
if (not $ok){
  $html->Warning("not ok");
}



