#!/usr/bin/perl -w
#  $Id: netmon.perl,v 1.6 2009/07/21 21:06:31 ams Exp $

use Carp;
use strict;
$SIG{INT}=sub{exit()};
#$SIG{'TTIN'}='IGNORE';

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
