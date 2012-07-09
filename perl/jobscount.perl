#!/usr/bin/perl -w
#  $Id: checkfs.cgi,v 1.12 2011/03/30 12:48:35 dmitrif Exp 
use strict;
use Carp;
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
    my $o=new DBSQLServer();
     my $ok=$o->ConnectRO();
my $min=1305810000;
my $max=4000000000;
my $list="seu/joblist_mis";
my $v=0;
my $maxrun=2300000000;
my $HelpTxt=" -ref -force -v ";
my $force=0;
my $ref="";
my $rmc=undef;
   foreach my $chop  (@ARGV){
    if($chop =~/^-ref/){
      $ref=unpack("x4 A*",$chop);
    }
    if($chop =~/^-mxr/){
      $maxrun=unpack("x4 A*",$chop);
    }
    if($chop =~/^-rmc/){
      $rmc=unpack("x4 A*",$chop);
    }
    if($chop =~/^-force/){
        $force=1;
    }
      if ($chop =~/^-v/) {
        $v=1;
      }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
}
my $sql="select count(amsdes.jobs.jid),cites.name  from amsdes.jobs,amsdes.cites where jobs.cid=cites.cid and amsdes.jobs.did=233 and amsdes.jobs.jobname like '%pass2.%'  group by cites.name";
my $ret=$o->Query($sql);
$sql="select count(amsdes.jobs.jid),cites.name  from amsdes.jobs,amsdes.cites where amsdes.jobs.cid=cites.cid and amsdes.jobs.did=233 and amsdes.jobs.jobname like '%pass2.%' and amsdes.jobs.realtriggers>0 group by cites.name";
my $ret2=$o->Query($sql);
     $sql = "SELECT run,fetime,letime fROM amsdes.datafiles where  status  like '%OK%' and type like 'SCI%'   and run>=1305853512  $run2p  and run <=1337244535  ORDER BY run ";
my $ret3=$o->Query($sql);

print " Total of : $#{@{$ret3}}+1\n";

my $requested=0;
my $requestec=0;
foreach my $cite (@{$ret}){
print "Requested $cite->[1] $cite->[0] \n";
$requested+=$cite->[0];
}
foreach my $cite (@{$ret2}){
print "Completed $cite->[1] $cite->[0] \n";
$requestec+=$cite->[0];
}

if($requested!=$#{@{$ret3}}+1){
}

print " Requested $requested Completed $requestec \n";
if(1 or $requested>$#{@{$ret3}}) {

    my $double=0;

    $sql="select amsdes.jobs.jid,amsdes.dataruns.run  from amsdes.jobs,amsdes.dataruns where amsdes.jobs.jid=dataruns.jid and amsdes.jobs.did=233 and amsdes.jobs.jobname like '%pass2.%'  order by dataruns.run";
my $ret4=$o->Query($sql);
    my $runold=0;
    my $jidold=0;
foreach my $ntuple (@{$ret4}){
     if($runold == $ntuple->[1]){
     #   print "$ntuple->[0] $ntuple->[1] $runold $jidold\n";
        $sql="select path,jid from amsdes.ntuples where run=$ntuple->[1] and (jid=$jidold or jid=$ntuple->[0])";
      #print "$sql \n"; 
      my $ret5=$o->Query($sql);
        if(defined $ret5->[0][0]){
#            print "  exist in $ret5->[0][0] $ret5->[0][1] \n";
            my $rmv=$ntuple->[0];
            if($rmv==$ret5->[0][1]){
                $rmv=$jidold;
            }
            $sql="select jobname from amsdes.jobs where jid=$rmv  and realtriggers<0"; 
            my $ret6=$o->Query($sql);
            if(defined $ret6->[0][0]){
              print "$ret6->[0][0],"; 
          }
            $sql="select path from amsdes.ntuples where jid=$rmv"; 
             my $ret66=$o->Query($sql);
            if(not defined $ret66->[0][0]){
            $sql="select jobname from amsdes.jobs where jid=$rmv "; 
             $ret66=$o->Query($sql);
              print "$ret66->[0][0],"; 
          }
          
        }
        $double++;
}
      $runold=$ntuple->[1];
     $jidold=$ntuple->[0];
   }  
    if($double){
        print " \n Double $double \n";
    }

    $sql="select amsdes.jobs.jid,amsdes.dataruns.run  from amsdes.jobs,amsdes.dataruns where amsdes.jobs.jid=dataruns.jid and amsdes.jobs.did=233 and amsdes.jobs.jobname like '%pass2.%'  and jobs.realtriggers<0 and jobs.cid=1 order by dataruns.run";
my $ret7=$o->Query($sql);
foreach my $ntuple (@{$ret7}){
    print "$ntuple->[1],";
}
    print "\n";
}
