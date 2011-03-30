#!/usr/local/bin/perl -w
#  $Id: rd.perl,v 1.5 2011/03/30 12:48:34 dmitrif Exp $
use strict;

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
    my $runmin=0;
    my $runmax=2000000000;
    my $sql = "SELECT run, path,fetime, nevents, tag, sizemb, type FROM amsdes.datafiles  WHERE run>$runmin AND run<$runmax ORDER BY run";
         my $ret=$o->Query($sql);
         if (defined $ret->[0][0]) {
          foreach my $r (@{$ret}){
             my $run       = $r->[0];
             my $path      = $r->[1];
             my $starttime = $r->[2];
             my $nevents   = $r->[3];
             my $tag   = $r->[4];
             my $sizemb   = $r->[5];
             my $type= $r->[6];
             print "$run,$path,$starttime,$nevents,$tag,$sizemb,$type \n";
         }
      }
}



