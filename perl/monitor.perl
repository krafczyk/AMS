#!/usr/bin/perl -w

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

my $monitor=new Monitor();


#get ior from server and connect to

$monitor->Connect();
$monitor->Update();

#Activate GTK

my $window=new monitorUI;
Gtk->timeout_add(60000,\&Monitor::Update);






#show window and run orbit

 $window->{window}->show_all;
 $monitor->{orb}->run();
# Gtk->main();
