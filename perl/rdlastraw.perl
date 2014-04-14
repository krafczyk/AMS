#!/usr/bin/perl -w
#  $Id$
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
                 my $phone="164733";
                 my $dl=10800;
                 my $dlr=3600;
my $nook=0;
my $okonly=0;
my $nowarning=0;
my $nowarning2=0;
my $sci="";
   my $run2p="";
   foreach my $chop  (@ARGV){
   if ($chop =~/^-dl/) {
        $dl=unpack("x3 A*",$chop);
    }
   if ($chop =~/^-ph/) {
        $phone=unpack("x3 A*",$chop);
    }
   if ($chop =~/^-dr/) {
        $dlr=unpack("x3 A*",$chop);
    }
   
   if ($chop =~/^-ok/) {
       $okonly=1;
   }
   if ($chop =~/^-sci/) {
       $sci="and type like 'SCI%'";
   }
   if ($chop =~/^-nook/) {
       $nook=1;
   }
   if ($chop =~/^-nowarning/) {
       $nowarning=1;
   }
   if ($chop =~/^-nowarning2/) {
       $nowarning=1;
       $nowarning2=1;
   }
}

    my $o=new DBSQLServer();
     my $ok=$o->ConnectRO();
my @good=();
my @goodplus=();
my @goodtime=();
my @gooddelay=(0,0,0);
if($ok && !$nook){
    my $timenow=time();
    $timenow-=3600*24;  
    my $sql = "SELECT path,timestamp,run fROM amsdes.datafiles  where  timestamp>$timenow and type not like 'MC%' $sci ORDER BY timestamp desc";
         my $ret=$o->Query($sql);
    my $delay=3600*24;
    $goodtime[0]=0;
    $goodtime[1]=0;
    $goodtime[2]=0;
         if (defined $ret->[0][0]) {
            $delay=$timenow+3600*24-$ret->[0][1];
            $goodtime[0]=$ret->[0][1];
            $gooddelay[0]=$delay;
         }
             if(!$okonly ){
         if (defined $ret->[0][0]) {
                print "$ret->[0][0] $ret->[0][1] $delay \n";
            }
         }
             else{
 
         $sql = "SELECT path,run,timestamp fROM amsdes.ntuples  where datamc=1 and version like 'v5.0%' and path not like '%hbk' and timestamp>$timenow-3600 and path like '%/std%'  $run2p  ORDER BY timestamp desc";
         my $rdlast=$o->Query($sql);
         $sql = "SELECT path,run,timestamp fROM amsdes.ntuples  where datamc=1 and version like 'v5.0%' and path not like '%hbk' and path like '%/std%' and timestamp>$timenow-3600  $run2p  ORDER BY run desc";
         my $rdlastr=$o->Query($sql);
                

         $good[0]=$delay<$dl?"OK":"NORAWFILES";
         $goodplus[0]=$delay<$dl+$dlr?"OK":"NORAWFILES";
         if(defined $rdlast->[0][0]){
           my $delayroot=$timenow+3600*24-$rdlast->[0][2];
                 $good[1]=$delayroot<$dl+$dlr*2?"OK":"NOROOTFILES";
                 $goodplus[1]=$delay<$dl+$dlr+$dlr*2?"OK":"NOROOTFILES";
                 $goodtime[1]=$rdlast->[0][2];
                 $gooddelay[1]=$delayroot;
         }
         else{
          $good[1]="NOROOTFILES";
          $goodplus[1]="NOROOTFILES";
         } 
         if(defined $rdlastr->[0][0]){
           my $delayrun=$timenow+3600*24-$rdlastr->[0][2];
                 $good[2]=$delayrun<$dl+$dlr*5?"OK":"NOFRESHRUNS";
                 $goodplus[2]=$delayrun<$dl+$dlr*5+$dlr?"OK":"NOFRESHRUNS";
                 $goodtime[2]=$rdlastr->[0][2];
                 $gooddelay[2]=$delayrun;
         }
         else{
          $good[2]="NOFRESHRUNS";
          $goodplus[2]="NOFRESHRUNS";
         } 
#                 print "$good[0] $good[1] $good[2] \n";
                 my $mess=",";
                 my $warn=0;
                 my $i=0;                 
                 foreach my $gd (@good){
                     if($gd ne 'OK' and not $nowarning){
                         $warn=1;
                     }
                     $mess=$mess." $gd";
                     if($nowarning2){
                       $mess=$mess."$gooddelay[$i++] ";
                     }
                     
                 }
                 my $warnplus=0;                 
                 foreach my $gd (@goodplus){
                     if($gd ne 'OK' and not $nowarning){
                         $warnplus=1;
                     }
                     
                 }
         if($warn){
#   send expert e-mail and sms if not already notified
#   form message
             $sql="select expert_notified from amsdes.filesprocessing";
             my $exp=$o->Query($sql);
            my $time=0; 
            if(defined $exp->[0][0]){
                $time=$exp->[0][0];
             }
             my $expert_notified=0;
             my $addmessage="Call Expert  $phone";
         if($time>$goodtime[0] or $time>$goodtime[1] or $time>$goodtime[2]){
             $addmessage="Expert Was Notified ";
             $expert_notified=1;
          } 
             if(not $expert_notified){
#                my @address=('vitali.choutko@cern.ch', '41764870923@mail2sms.cern.ch');
                 my @address=('Alexandre.Eline@cern.ch', '41764874733@mail2sms.cern.ch');
             foreach my $add (@address){
                 sendmailmessage($add,"rdlastraw-E-$mess",'To acknowledge  ssh ams@pcamsf4 ;cd /Offline/vdev/perl;./expert_notified.perl');
             }             
              if($warnplus){
                my @addressplus=('vitali.choutko@cern.ch', '41764870923@mail2sms.cern.ch');
                foreach my $add (@addressplus){
                  sendmailmessage($add,"rdlastraw-E-$mess",'To acknowledge  ssh ams@pcamsf4 ;cd /Offline/vdev/perl;./expert_notified.perl');
                }             
              }
             }
         
             $mess=$addmessage.$mess;               
        }
        else{
             $mess="SOC Status $mess";               
        }        
                 print "$mess \n";
     }
      
}
    else{
        my $addmessage="Call Expert  $phone";
        print "$addmessage,NODB \n";
    }





sub sendmailmessage{
    my $add=shift;
    my $sub=shift;
    my $mes=shift;
    open MAIL, "| /usr/lib/sendmail -t -i"
#        or die "could not open sendmail: $!";
        or return 1;
     print MAIL <<END_OF_MESSAGE2;
To:  $add
Subject: $sub

$mes
END_OF_MESSAGE2
    close MAIL or return 1;
    return 0;
}
