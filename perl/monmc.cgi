#!/usr/bin/perl -w
#  $Id: monmc.cgi,v 1.1 2002/02/08 15:36:49 choutko Exp $
use Gtk;
use strict;


use lib::Monitor;
use lib::monitorHTML;

my $mc="-m";
unshift @ARGV, $mc;

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


