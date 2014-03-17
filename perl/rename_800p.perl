#!/usr/local/bin/perl -w
use strict;
use lib qw(../perl);
use lib::DBSQLServer;
use File::Basename;

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

my $eos=0;
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
   if ($chop =~/^-eos/) {
       $eos=1;
    }
   if ($chop =~/^-copyeos/) {
       $eos=2;
    }
   if ($chop =~/^-deleos/) {
       $eos=4;
    }
   if ($chop =~/^-copychangeeos/) {
       $eos=3;
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
my $ds="pass5";
if($ok){
    my $time=time();
    my $runmin=1385485339;
    if($run2p eq ""){
        $run2p="and run>=$runmin and run<$runmax ";
    }
    my $sql = "select count(run) from amsdes.ntuples where  path like '%ISS.B800P/$ds%' and path not like '/castor%' $run2p";
         my $ret=$o->Query($sql);
    if($ret->[0][0]>0){
        print "Not castor file exists $ret->[0][0] \n";
 $sql = "select count(run) from amsdes.ntuples where  path like '%ISS.B800P/$ds%' and path not like '/castor%' and castortime=0 $run2p";
       $ret=$o->Query($sql);
  if($ret->[0][0]>0){
        print "Not backuped to castor file exists $ret->[0][0] \n";

    }
       exit;
    }

    if($eos){
 $sql = "select count(run) from amsdes.ntuples where  path like '%ISS.B800P/$ds%' and eostime=0 $run2p";
       $ret=$o->Query($sql);

  if($ret->[0][0]>0){
        print "Not backuped to eos file exists $ret->[0][0] \n";

        if($eos>1){
            my $eosprefix="root://eosams.cern.ch///eos";
            my $castorprefix="root://castorpublic.cern.ch//";
            my $castorsuffix="?svcClass=amscdr";
             $sql = "select path from amsdes.ntuples where  path like '%ISS.B800P/$ds%' and eostime=0 $run2p";
       $ret=$o->Query($sql);
    foreach my $file (@{$ret}){
            my $stage="stager_get -M $file->[0]";
            system($stage);
            $stage="stager_qry -M $file->[0]";
            system($stage);
            my $fileinput =$file->[0];
            my $fileoutput=$fileinput;
            $fileinput=$castorprefix.$fileinput.$castorsuffix;
           $fileoutput=~s?/castor/cern.ch?$eosprefix?;
            my $xrdcp="xrdcp  ".'"'.$fileinput.'"'." $fileoutput";
            my $i_xrdcp=system($xrdcp);
            if($i_xrdcp){
             print " xrdcp error $xrdcp \n";
             if($eos>2){
    my $eos_prefix="/tmp/eosams";
    my $eosmount="/afs/cern.ch/project/eos/installation/0.3.1-22/bin/eos.select -b fuse mount $eos_prefix";
    my $i_eosmount=system($eosmount);
    if($i_eosmount){
        print "eos mount failed $eosmount \n";
#        exit;
    }
    my $nsls="nsls -l $file->[0]";
    my $castoro=`$nsls`; 
    my $eosfile=$file->[0];
    $eosfile=~s:/castor/cern.ch::;
    my $eosls="ls -l $eos_prefix$eosfile ";
    my $eoso=`$eosls`; 

    my @jc=split ' ',$castoro;
    my @je=split ' ',$eoso;

    if($#jc>4 and $#je>4 and ($jc[4] eq $je[4])){
        goto seteos;
    }
}
         }
            else{
seteos:
                my $eostime=time();
                $sql="update ntuples set eostime=$eostime where path like '$file->[0]'";
                $o->Update($sql);
                $o->Commit();
            }
        
        }
        }
       exit;
    }
}
 $sql = "select count(path) from amsdes.ntuples where  path like '%ISS.B800/$ds%' and run in (select run from   ntuples   where  path like '%ISS.B800P/$ds%' and path  like '/castor%') $run2p";
       $ret=$o->Query($sql);
  if($ret->[0][0]>0){
        print "duplicated runs  exist $ret->[0][0] \n";
       exit;

    }

    print "#processing...\n";
    my $eos_prefix="/tmp/eosams";
    my $eosmount="/afs/cern.ch/project/eos/installation/0.3.1-22/bin/eos.select -b fuse mount $eos_prefix";
    my $i_eosmount=system($eosmount);
    if($i_eosmount){
        print "eos mount failed $eosmount \n";
#        exit;
    }
    my $castor_prefix="/castor/cern.ch";
    $sql="select path,jid,eostime from ntuples   where  path like '%ISS.B800P/$ds%' and path  like '/castor%' $run2p";
    $ret=$o->Query($sql);
    my $eosdir = dirname($ret->[0]->[0]);
    $eosdir =~ s:$castor_prefix:$eos_prefix:;
    my $neweosdir = $eosdir;
    $neweosdir =~ s/ISS.B800P/ISS.B800/;
    system("chmod u+w $eosdir; chmod u+w $eosdir/..; chmod u+w  $neweosdir/.. 2>/dev/null; chmod u+w  $neweosdir 2>/dev/null; mkdir -p $neweosdir");
    foreach my $file (@{$ret}){
        my $line=$file->[0];
        my $eos_filein=$line;
        my $eos_fileout=$line;
        if($file->[2]>0){
          $eos_filein=~s:$castor_prefix:$eos_prefix:;
          $eos_fileout=~s:$castor_prefix:$eos_prefix:;
          $eos_fileout=~s/ISS.B800P/ISS.B800/;
          my $eoscopy="mv $eos_filein $eos_fileout ";
          my $i_eoscopy=system($eoscopy);
        if($i_eoscopy){
            print "unable to eoscopy $eoscopy $i_eoscopy \n";

            if($eos>3){

    my $nsls="nsls -l $file->[0]";
    my $castoro=`$nsls`; 
    my $eosfile=$file->[0];
    $eosfile=~s:/castor/cern.ch::;
    my $eosls="ls -l $eos_prefix$eosfile ";
    my $eoso=`$eosls`; 

    my @jc=split ' ',$castoro;
    my @je=split ' ',$eoso;

    if($#jc>4 and $#je>4 and ($jc[4] eq $je[4])){
    }
    else{
        my $rm="rm $eos_prefix$eosfile";
        if(system($rm) and $#je>4){
            print "rm failed $rm $eoso \n";
        }
        else{
                $sql="update ntuples set eostime=0 where path like '$file->[0]'";
          $o->Update($sql);
          $o->Commit(1); 
      
        }
    }
            }

            next;
        }
      }
        $line=~s/ISS.B800P/ISS.B800/;
        my $cmd ="rfrename $file->[0] $line";
        my $i=system($cmd);
        if($i==0){
          $sql="update jobs set did=310 where jid=$file->[1]";      
          $o->Update($sql);
          $sql="update ntuples set path='$line'   where path='$file->[0]'";      
          $o->Update($sql);
          $o->Commit(1); 
      }
        else{
          if($file->[2]>0){
          my $eoscopy="mv $eos_fileout $eos_filein ";
          my $i_eoscopy=system($eoscopy);
             if($i_eoscopy){
            print "fatal-unable to eoscopy back $eoscopy $i_eoscopy \n";
            exit;
        }

      }
      }
    }
}



