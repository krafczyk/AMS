#!/usr/bin/perl -w
#  $Id: monmcdb.cgi,v 1.1 2002/02/20 17:59:57 choutko Exp $
use Gtk;
use strict;


use lib::Monitor;
use lib::monitorHTML;

my $mc="-m ";
unshift @ARGV, $mc;
my $db="-d ";
unshift @ARGV, $db;



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


