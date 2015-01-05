#!/usr/bin/perl -w

use strict;
use lib::DBServer;
use lib::DBSQLServer;
$SIG{INT}=sub{exit()};
$SIG{QUIT}=sub{exit()};

# [ae-03.Nov.2014]  
# Being started from C++ monitorUI, together with 2 subroutines in 
# lib/DBServer.pm::InitExport & lib/DBServer.pm::InitDBFileExport 
# provide BDB export interface from C++ monitorUI to make a backpup 
# BDB copy

# activate poa and get self ior
my $dbserver=new DBServer();

#open db file and connect to server
my $ok=$dbserver->InitExport();
warn "  init $ok \n";
