#!/usr/local/bin/perl -w
#  $Id: rdlast.perl,v 1.3 2011/04/05 07:29:29 choutko Exp $
use strict;
use lib qw(../perl);
use lib::DBSQLServer;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";

# DESCRIBE DATAFILES
# RUN                                       NOT NULL NUMBER(38)
# VERSION                                            VARCHAR2(64)
# TYPE                                               VARCHAR2(64)
# FEVENT                                             NUMBER(38)
# LEVENT                                             NUMBER(38)
# NEVENTS                                            NUMBER(38)
# NEVENTSERR                                         NUMBER(38)
# TIMESTAMP                                          NUMBER(38)
# SIZEMB                                             NUMBER(38)
# STATUS                                             VARCHAR2(64)
# PATH                                               VARCHAR2(255)
# PATHB                                              VARCHAR2(255)
# CRC                                                NUMBER(38)
# CRCTIME                                            NUMBER(38)
# CASTORTIME                                         NUMBER(38)
# BACKUPTIME                                         NUMBER(38)
# TAG                                                NUMBER(38)
# FETIME                                             NUMBER(38)
# LETIME                                             NUMBER(38)
# PATHS                                              VARCHAR2(255)




    my $o=new DBSQLServer();
     my $ok=$o->ConnectRO();
if($ok){
    my $sql = "SELECT path fROM amsdes.ntuples  where datamc=1 and version like 'v5.00%' ORDER BY timestamp desc";
         my $ret=$o->Query($sql);
         if (defined $ret->[0][0]) {
             print "$ret->[0][0] \n";
         }
}




