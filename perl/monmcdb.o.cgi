#!/usr/bin/perl -w
#  $Id: monmcdb.o.cgi,v 1.3 2003/10/23 15:10:58 choutko Exp $
use Gtk;
use strict;


use lib::Monitor;
use lib::monitorHTML;

my $mc="-m ";
unshift @ARGV, $mc;
my $db="-d ";
unshift @ARGV, $db;
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/monmcdb.o.cgi";



# activate poa and get self ior

my $monitor=new Monitor();



#activate CGI

my $html=new monitorHTML;

 my $ok=$monitor->Connect();

#get ior from server and connect to



 if($ok){
   $html->Update();
 }
 else{
  $html->Warning();
 }


