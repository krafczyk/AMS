#!/usr/local/bin/perl -w
#  $Id: netmon.perl,v 1.1 2006/05/15 14:29:01 choutko Exp $

use Carp;
use strict;
$SIG{INT}=sub{exit()};


package main;
use lib qw(/afs/cern.ch/user/c/choutko/AMS/perl);

#use Error qw(:try);
use lib::NetMonitor;


# activate poa and get self ior



my $monitor=new NetMonitor();
$monitor->Run();
