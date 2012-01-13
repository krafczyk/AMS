#!/usr/local/bin/perl -w
#  $Id: rdlastr.perl,v 1.1 2012/01/13 16:59:00 ams Exp $
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

   my $run2p="";
   foreach my $chop  (@ARGV){
   if ($chop =~/^-r/) {
        $run2p=unpack("x2 A*",$chop);
        $run2p=" and run=".$run2p;
    }
   }

    my $o=new DBSQLServer();
     my $ok=$o->ConnectRO();
if($ok){
    my $sql = "SELECT path fROM amsdes.ntuples  where datamc=1 and version like 'v5.00%' and path not like '%hbk' and path like '%/std%'  $run2p  ORDER BY run desc";
         my $ret=$o->Query($sql);
         if (defined $ret->[0][0]) {
             print "$ret->[0][0] \n";
         }
}




