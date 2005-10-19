#!/usr/bin/perl -w
#  $Id: monmc.cgi,v 1.3 2005/10/19 14:26:26 choutko Exp $
use Gtk;
use strict;


use lib::Monitor;
use lib::monitorHTML;
if($ENV{MOD_PERL}){
  $#ARGV=-1;
}
                                                                                

my $mc="-m";
unshift @ARGV, $mc;
unshift @ARGV, "-N/cgi-bin/mon/monmc.cgi";

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


