#!/usr/bin/perl -w       
use Gtk;
use lib::monitorUI;
use lib::Monitor;

package main;

my $window=new monitorUI;

$window->{window}->show_all;
Gtk->main;
