#!/usr/bin/perl -w
use Gtk;
use strict;


use lib::Monitor;
use lib::monitorHTML;



# activate poa and get self ior

my $monitor=new Monitor();





#get ior from server and connect to

my $ok=$monitor->Connect();

#activate CGI

my $html=new monitorHTML;


 if($ok){
   $html->Update();
   exit;
 }
if(not $ok){
  $html->Warning();
}
