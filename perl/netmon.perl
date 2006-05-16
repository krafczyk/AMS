#!/usr/local/bin/perl -w
#  $Id: netmon.perl,v 1.2 2006/05/16 08:40:09 choutko Exp $

use Carp;
use strict;
$SIG{INT}=sub{exit()};


package main;
use lib qw(/afs/cern.ch/user/c/choutko/AMS/perl);

#use Error qw(:try);
use lib::NetMonitor;
use Sys::Hostname;
    my $host = hostname;

# activate poa and get self ior


print " ip: $host \n";
my $monitor=new NetMonitor();
$monitor->Run();
