#!/usr/bin/perl -w
use strict;
#use Gtk;
use lib::DBServer;
use lib::DBSQLServer;
$SIG{INT}=sub{exit()};
$SIG{QUIT}=sub{exit()};

# activate poa and get self ior

my $dbserver=new DBServer();

#open db file and connect to server

my $ok=$dbserver->Init();
warn "  init $ok \n";
if($ok){
#
#   update pdb
#
    my $oracle=0;
    my $datamc=0;
foreach my $chop  (@ARGV){
    if($chop =~/^-o/){
        $oracle=1;
        last;
    }
    if($chop=~/^-v5/){
        $datamc=1;
    }
}
    $#ARGV=-1;
    if($oracle){
     unshift @ARGV, "-DOracle:";
     unshift @ARGV, "-Fpdb_ams";
    
    
    my $sqls=new DBSQLServer();
    if($sqls->Connect()){
    foreach my $ac (@{$dbserver->{asl}}){
          my $arsa=$ac->{ars};
        foreach my $ars (@{$arsa}){
          if($ars->{Type} eq "Producer"){
                if($ars->{Interface} eq "default"){
                    $sqls->{IORP}=$ars->{IOR};
          }
          } 
          if($ars->{Type} eq "Server"){
                if($ars->{Interface} eq "default"){
                    $sqls->{IORS}=$ars->{IOR};
          }
          } 
          if($ars->{Type} eq "DBServer"){
                if($ars->{Interface} eq "default"){
                    $sqls->{IORD}=$ars->{IOR};
          }
          } 
         }
         if(not defined $sqls->{IORD}){
            $sqls->{IORD}=$dbserver->{myior};
         }
          warn " IORP $sqls->{IORP} \n";
          warn " IORS $sqls->{IORS} \n";
          warn "IORD $sqls->{IORD} \n";
          if(defined $sqls->{IORP} and defined $sqls->{IORS}){
            warn " updating sql \n"; 
            my $createt=time();
            my $sql="delete from Servers where dbfilename='$dbserver->{dbfile}'";
            $sqls->Update($sql);
            my $host=$sqls->ior2host($sqls->{IORS});
            $sql="insert into Servers values('$dbserver->{dbfile}','$sqls->{IORS}','$sqls->{IORP}','$sqls->{IORD}','Active',$ac->{Start},$createt,$datamc,$host)";
                 $sqls->Update($sql);
                 last;
        }
      }
     }        
 }
    $dbserver->{orb}->run();
}


