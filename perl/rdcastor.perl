#!/usr/local/bin/perl -w
#  $Id: rdcastor.perl,v 1.1 2012/01/05 13:48:08 choutko Exp $
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
    my $sql = "SELECT path,castortime fROM amsdes.ntuples  where datamc=1 and version like 'v5.00%' and path not like '%hbk' and path like '%ISS.B550/pass2%'  $run2p  ORDER BY run ";
         my $ret=$o->Query($sql);
     foreach my $file (@{$ret}){
             print "$file->[0] $file->[1]\n";
         }
}





