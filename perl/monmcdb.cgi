#!/usr/bin/perl -w
#  $Id: monmcdb.cgi,v 1.2 2002/03/14 14:13:32 choutko Exp $
use Gtk;
use strict;


use lib::Monitor;
use lib::monitorHTML;

my $mc="-m ";
unshift @ARGV, $mc;
my $db="-d ";
unshift @ARGV, $db;

unshift @ARGV, "-N/cgi-bin/mon/monmcdb.cgi";


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


