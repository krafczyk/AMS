#!/usr/bin/perl -w
#  $Id: calculateMips.o.cgi,v 1.7 2011/03/30 12:48:34 dmitrif Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
unshift @ARGV, "-N/cgi-bin/mon/calculateMips.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);
my $ok=$html->ConnectOnlyDB();
my $fast=0;
my $verbose=1;
   my @output=$html->calculateMipsVC($fast,$verbose);
    my $tcpu=0;
    my $rcpu=0;
   foreach my $arr (@output){
     foreach my $comp (@{$arr}){
       print " $comp ";
     }
     $tcpu+=$arr->[4];
     $rcpu+=$arr->[5];
     print "\n";
   }
   print " $tcpu $rcpu \n";




