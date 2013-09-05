#!/usr/local/bin/perl -w
#  $Id: rename_620p.perl,v 1.1 2013/09/05 12:30:52 bshan Exp $
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
   my $path=0;
   my $timei=0;
my $runmax=2337449847;
   foreach my $chop  (@ARGV){
   if ($chop =~/^-r/) {
        $run2p=unpack("x2 A*",$chop);
        $run2p=" and run in (".$run2p.")";
    }
   if ($chop =~/^-path/) {
       $path=1;
    }
   if ($chop =~/^-time/) {
       $timei=1;
    }
   if ($chop =~/^-RMAX/) {
        my $rmax=unpack("x5 A*",$chop);
       $runmax=int($rmax);
    }
}

    my $o=new DBSQLServer();
     my $ok=$o->Connect();
my $ds="pass4";
if($ok){
    my $time=time();
my $runmin=1305853512;
    my $sql = "select count(run) from amsdes.ntuples where  path like '%ISS.B620P/$ds%' and path not like '/castor%' $run2p";
         my $ret=$o->Query($sql);
    if($ret->[0][0]>0){
        print "Not castor file exists $ret->[0][0] \n";
 $sql = "select count(run) from amsdes.ntuples where  path like '%ISS.B620P/$ds%' and path not like '/castor%' and castortime=0 $run2p";
       $ret=$o->Query($sql);
  if($ret->[0][0]>0){
        print "Not backuped castor file exists $ret->[0][0] \n";

    }
       exit;
    }
 $sql = "select count(path) from amsdes.ntuples where  path like '%ISS.B620/$ds%' and run in (select run from   ntuples   where  path like '%ISS.B620P/$ds%' and path  like '/castor%') $run2p";
       $ret=$o->Query($sql);
  if($ret->[0][0]>0){
        print "duplicated runs  exist $ret->[0][0] \n";
       exit;

    }

    print "#processing...\n";

    $sql="select path,jid from ntuples   where  path like '%ISS.B620P/$ds%' and path  like '/castor%' $run2p";
         $ret=$o->Query($sql);
    foreach my $file (@{$ret}){
        my $line=$file->[0];
        $line=~s/ISS.B620P/ISS.B620/;
        my $cmd ="rfrename $file->[0] $line";
        my $i=system($cmd);
        if($i==0){
          $sql="update jobs set did=266 where jid=$file->[1]";      
          $o->Update($sql);
          $sql="update ntuples set path='$line'   where path='$file->[0]'";      
          $o->Update($sql);
          $o->Commit(1); 
      }
    }
}


