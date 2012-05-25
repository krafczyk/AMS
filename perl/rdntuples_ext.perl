#!/usr/bin/perl -w
#  $Id: rdntuples_ext.perl,v 1.3 2012/05/25 10:25:13 ams Exp $
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
    my $time=time();
    my $sql = "SELECT ntuples.run,ntuples.path,cites.name,ntuples.jid fROM amsdes.ntuples,amsdes.jobs,amsdes.cites  where ntuples.datamc=1 and ntuples.version like 'v5.00%' and ntuples.path like '%ISS.B584/pass2%root' and ntuples.jid=jobs.jid and jobs.cid!=1  and jobs.cid=cites.cid $run2p  ORDER BY run ";
         my $ret=$o->Query($sql);
    my $okk=0;
    foreach my $file (@{$ret}){
         if((not $okk) or $time-$file->[0]<864000000){ 
             print "$file->[1] Cite $file->[2] $file->[3]\n";
             $okk=1;
         }
     }
}



