#!/usr/local/bin/perl -w
#  $Id: netmon.perl,v 1.3 2006/05/17 12:43:38 ams Exp $

use Carp;
use strict;
$SIG{INT}=sub{exit()};


package main;
use lib qw(/afs/cern.ch/user/a/ams/vc/perl);

#use Error qw(:try);
use lib::NetMonitor;
use Sys::Hostname;
    my $host = hostname;

# activate poa and get self ior


print " ip: $host \n";
my $monitor=new NetMonitor();
$monitor->Run();
