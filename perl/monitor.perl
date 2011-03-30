#!/usr/local/bin/perl -w
#  $Id: monitor.perl,v 1.13 2011/03/30 12:48:35 dmitrif Exp $

use Error qw(:try);
use Gtk;
use Carp;
use strict;
$SIG{INT}=sub{exit()};


package main;
use Error qw(:try);
use lib::Monitor;
use lib::monitorUI;


# activate poa and get self ior


#my $mc="-m";
#unshift @ARGV, $mc;
#   $#ARGV=-1;
     unshift @ARGV, "-DOracle:";
     unshift @ARGV, "-Fpdb_ams";
                                                                                            

my $monitor=new Monitor();

#get ior from server and connect to

my $ok=$monitor->Connect();

#Activate GTK

my $window=new monitorUI;



#show window and run orbit
 if($ok){
  $ok=$window->Update();
 }
 $window->{window}->show_all;
if(not $ok){
  monitorUI::Warning();
}
else{
 $monitor->{orb}->run();
}
# Gtk->main();
