#!/usr/bin/perl -w
#  $Id: monmcdb.mysql.cgi,v 1.1 2002/08/08 17:18:09 alexei Exp $
use Gtk;
use strict;


use lib::Monitor;
use lib::monitorHTML;

my $mc="-m ";
unshift @ARGV, $mc;
my $db="-d ";
unshift @ARGV, $db;
unshift @ARGV, "-Dmysql";
unshift @ARGV, "-F:AMSMC02:pcamsf0";
unshift @ARGV, "-N/cgi-bin/mon/monmcdb.mysql.cgi";



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


