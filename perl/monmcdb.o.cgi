#!/usr/bin/perl -w
#  $Id: monmcdb.o.cgi,v 1.5 2011/03/30 12:48:34 dmitrif Exp $
use Gtk;
use strict;


use lib::Monitor;
use lib::monitorHTML;
if($ENV{MOD_PERL}){
  $#ARGV=-1;
}
my $mc="-m ";
unshift @ARGV, $mc;
my $db="-d ";
unshift @ARGV, $db;
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
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


