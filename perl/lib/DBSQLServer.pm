# $Id: DBSQLServer.pm,v 1.36 2003/04/01 12:02:57 alexei Exp $

#
#
# Input parameters :
#                   -Iforce    - dbinit 
#                                    2 - recreate all tables
#                   -Ddbdriver - database driver 
#                                                oracle
#                                                mysql
#                   -Fdbfile   - database name or databasefile
#                                (can also include host name)
#                                for mysql : AMSMC02:pcamsf0
# example :
#          mySQL
#           ./dbsqlserver.perl -Iforce -Dmysql -F:AMSMC02:pcamsf0
#          Oracle (from pcamsf0)
#           ./dbsqlserver.perl -Iforce -DOracle -F:amsdb
#
# Aug  7, 2002 a.k. Tables names are case sensitive in MySQL
# Mar 30, 2003 a.k. '/vicepa,b,c' directory paths
# Apr  1, 2003 a.k. create indecies for RNDM table (it takes a while)
#
package DBSQLServer;
use Error qw(:try);
use lib::CID;
use Carp;
use strict;
use Carp;
use Fcntl ":flock";
use Fcntl;
use POSIX qw(tmpnam);
use MLDBM qw(DB_File Storable);
use DBI;
use Time::localtime;

@DBSQLServer::EXPORT= qw(new Connect QueryAll Query Update set_oracle_env);
my %fields=(
     start=>undef,
     cid=>undef,
     dbhandler=>undef,
     dbdriver=>undef,
     dbfile=>undef,
     dbdefaultfile=>"DataManagement/CSV",
     dbdefaultdriver=>"CSV:f_dir=",
     dbinit=>0,
             );

sub new{
    my $type=shift;
    my $self={
        %fields,
    };
$self->{start}=time();
$self->{cid}=new CID("DBSQLServer");    

foreach my $chop  (@ARGV){
    if($chop =~/^-F/){
     $self->{dbfile}=unpack("x2 A*",$chop);
    }
    if($chop =~/^-D/){
     $self->{dbdriver}=unpack("x2 A*",$chop);
    }
    if($chop =~/^-I/){
        $self->{dbinit}=unpack("x2 A*",$chop);
        if($self->{dbinit} eq "force"){
            $self->{dbinit}=2;
        }
        else{
            $self->{dbinit}=1;
        }
    }
}

    if(not defined $self->{dbdriver}){
        $self->{dbdriver}=$self->{dbdefaultdriver};
    }

    if(not defined $self->{dbfile}){
        $self->{dbfile}=$self->{dbdefaultfile};
    }
    my $dir=$ENV{AMSDataDir};
    if(defined $dir and $self->{dbdriver} =~ m/CSV/){
        $self->{dbfile}=$dir.'/'.$self->{dbfile};
    }        
    elsif($self->{dbdriver} =~ m/CSV/){
        die "DBSQL Server AMSDataDir Not Defined";
    }

    my $mybless=bless $self,$type;
    if(ref($DBSQLServer::Singleton)){
        croak "Only Single DBSQLServer Allowed\n";
    }
$DBSQLServer::Singleton=$mybless;
return $mybless;
}

sub Connect{
    my $self=shift;
    my $user   = "amsdes";
    my $pwd;
    if($self->{dbdriver} =~ m/Oracle/){
     set_oracle_env();
    }
    my $oracle="/var/www/cgi-bin/mon/lib/.oracle.oracle";
    open(FILE,"<".$oracle) or die "Unable to open file $oracle \n";
    while  (<FILE>){
        $pwd=$_;
    }
    close FILE;
#- interactive debug
#    print "dbinfo.......DBI:$self->{dbdriver}$self->{dbfile} \n";
#    print "user.........$user identified by pwd\n";
#    print "init level...$self->{dbinit}\n";
#-
    if($self->{dbinit}){
    $self->{dbhandler}=DBI->connect('DBI:'.$self->{dbdriver}.$self->{dbfile},$user,$pwd,{PrintError => 0, AutoCommit => 1}) or die "Cannot connect: ".$DBI::errstr;
      $self->Create();
    }
    else{
    $self->{dbhandler}=DBI->connect('DBI:'.$self->{dbdriver}.$self->{dbfile},$user,$pwd,{PrintError => 1, AutoCommit => 1}) or die "Cannot connect: ".$DBI::errstr;
        return 1;
    }
}

sub Create{
# creates a database
    my $self=shift;
    my $dbh=$self->{dbhandler};


    my @tables=("Filesystems", "Cites","Mails" ,"Jobs", "RNDM","Servers", "Runs","Ntuples","DataSets", "Environment");
    my @createtables=("    CREATE TABLE Filesystems
     (fid         CHAR(4) NOT NULL,   
     host    VARCHAR(40),            
     disk    VARCHAR(20),            
     path    VARCHAR(255),           
     totalsize    INT,               
     occupied     INT,               
     available    INT,               
     allowed      INT,               
     status   CHAR(40),              
     priority     INT,               
     timestamp    INT)",             
     "CREATE TABLE Cites
     (cid      INT NOT NULL ,
      name     VARCHAR(64),
      mid      INT,
      status   VARCHAR(255),
      maxrun   INT,
      state    INT,
      descr    VARCHAR(255),
      timestamp INT)",
      "CREATE TABLE Mails
      (mid INT NOT NULL ,
       address VARCHAR(255),
       alias   VARCHAR(255),
       name    VARCHAR(64),
       rsite   INT,
       rserver INT,
       cid     INT,
       status  VARCHAR(16),
       requests INT,
       timestamp INT,
       timeu1    INT,
       timeu2    INT)",
      "CREATE TABLE Jobs 
      (jid     INT NOT NULL,
       jobname VARCHAR(255),
       mid     INT,
       cid     INT,
       did     INT,
       time    INT,
       triggers INT,
       timeout  INT,
       content TEXT,
       timestamp INT,
       nickname VARCHAR(80))",
      "CREATE TABLE RNDM
      (rid     INT NOT NULL,
       rndm1   INT,
       rndm2   INT)",
      "CREATE TABLE Servers
       (dbfilename VARCHAR(255) NOT NULL,
        IORS   TEXT NOT NULL,
        IORP   TEXT NOT NULL,
        IORD   TEXT,
        status VARCHAR(64),
        createtime INT,
        lastupdate INT)",
       "CREATE TABLE Runs
        (run   INT NOT NULL,
         jid   INT,
         FEvent INT,
         LEvent INT,
         FETime INT,
         LETime INT,
         submit INT, 
         status VARCHAR(64))",
        "CREATE TABLE Ntuples
        (run   INT NOT NULL,
         version VARCHAR(64),
         type  VARCHAR(64),
         jid    INT,
         FEvent INT,
         LEvent INT,
         NEvents INT,
         NEventsErr INT,
         timestamp   INT,
         sizemb   INT,
         status   VARCHAR(64),
         path   VARCHAR(255),
         crc INT)",
        "CREATE TABLE DataSets
         (did    INT NOT NULL,
          name   VARCHAR(255))",
        "CREATE TABLE Environment
         (mykey    VARCHAR(255),
          myvalue   VARCHAR(255))"
         );
# status ntuples:  OK, Unchecked, Errors
#      
#   check if tables exist
    my $i=0;
    foreach my $table (@tables){
#    if($i==0){
#     $i++;
#     next;
#    } 
    if($self->{dbinit}<2){
       print "Create -I- check table $table \n";
       my $ok =$dbh->do("select * from $table");
      if(defined $ok){
        warn "Table $table already exist";
        $i=$i+1;
        next;
      }
    }
    if($self->{dbdriver} =~ m/CSV/){
         system "mkdir -p $self->{dbfile}";
         system "rm -f $self->{dbfile}"."/"."$table";
     }
     elsif($self->{dbdriver}=~ m/Oracle/){
         $createtables[$i] =~ s/TEXT/VARCHAR2(4000)/g;
     }        
    $dbh->do("drop table ".$tables[$i]);
    my $sth=$dbh->prepare($createtables[$i]) or die "cannot prepare: ".$dbh->errstr();
#     print " $createtables[$i] \n";
    $sth->execute() or die "cannot execute: ".$dbh->errstr();
    $sth->finish();    
     if ($tables[$i] eq "Servers" and $self->{dbdriver} =~ m/Oracle/) {
         $sth=$dbh->prepare("grant select on servers to amsro") or die "cannot prepare".$dbh->errstr();
         $sth->execute() or die "cannot execute: ".$dbh->errstr();
         $sth->finish();    
     }
    print "$tables[$i]\n";
     if($self->{dbdriver} =~ m/CSV/){
         system "chmod o+w $self->{dbfile}"."/"."$table";
     }
     $i=$i+1;
 }

#

# initialize rndm table;

my $RNDMTable="/afs/cern.ch/user/b/biland/public/AMS/rndm.seeds";
open(FILEI,"<".$RNDMTable) or die "Unable to open file $RNDMTable\n";

    my $line;
while ( $line = <FILEI>){
    my @pat=split / /,$line;
    my @arr;
    foreach my $chop (@pat){
        if($chop  =~/^\d/){
            $arr[$#arr+1]=$chop;
        }
    }
     $dbh->do("insert into RNDM values($arr[0],$arr[1],$arr[2])") or die "cannot do: ".$dbh->errstr();     
    
}
    close(FILEI);
# create indecies, it takes a while.
    $dbh->do("create index rid_ind on rndm (rid)") or die "cannot do: ".$dbh->errstr(); 

    my $cntr = 0;
    my $cnt  = 0;
    my $sql;


# initialize 
     if($self->{dbdriver} =~ m/Oracle/){
    $sql="SELECT COUNT(mykey) FROM Environment";
    $cntr=$self->Query($sql);
    foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
    }
}
    if ($cnt == 0) {
     $dbh->do("insert into Environment values('AMSDataDir','/f0dat1/AMSDataDir')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('CERN_ROOT','/cern/2001')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('UploadsDir','/var/www/cgi-bin/AMS02MCUploads')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('UploadsHREF','AMS02MCUploads')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('AMSSoftwareDir','DataManagement')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('DataSets','DataSets')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('gbatch','exe/linux/gbatch-orbit.exe')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('ntuplevalidator','exe/linux/fastntrd.exe')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('getior','exe/linux/getior.exe')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('filedb','ams02mcdb.tar.gz')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('filedb.att','ams02mcdb.att.tar.gz')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('dbversion','v4.00')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('AMSProdDir','prod')") or die "cannot do: ".$dbh->errstr();     
     my $apd='$AMSProdDir/prod/starttagmtb_db_mc';
     $dbh->do("insert into Environment values('amsserver','$apd')") or die "cannot do: ".$dbh->errstr();     
 } else {
    warn "Table Environment has $cnt entries. Not initialized";
 }
    $cnt = 0;
   if($self->{dbdriver} =~ m/Oracle/){
    $sql="SELECT COUNT(cid) FROM Cites";
    $cntr=$self->Query($sql);
    foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
    }
}
  if ($cnt == 0) {
   my $time=time();
   my $run=110;
   $dbh->do("insert into Cites values(1,'cern',0,
            'local',$run,0,'CERN',$time)")or die "cannot do: ".$dbh->errstr();    
     $run=(1<<27)+1;
   $dbh->do("insert into Cites values(2,'test',0,
            'remote',$run,0,'Test',$time)")or die "cannot do: ".$dbh->errstr();    
     $run=(2<<27)+1;
   $dbh->do("insert into Cites values(3,'bolo',0,
            'remote',$run,0,'INFN Bologna',$time)")or die "cannot do: ".$dbh->errstr();    
     $run=(3<<27)+1;
   $dbh->do("insert into Cites values(4,'ethz',0,
            'remote',$run,0,'ETH Zurich',$time)")or die "cannot do: ".$dbh->errstr();    
     $run=(4<<27)+1;
   $dbh->do("insert into Cites values(5,'mila',0,
            'remote',$run,0,'INFN Milano',$time)")or die "cannot do: ".$dbh->errstr();    
     $run=(5<<27)+1;
   $dbh->do("insert into Cites values(6,'knu',0,
            'remote',$run,0,'Kyungpook Nat. Univ.',$time)")or die "cannot do: ".$dbh->errstr();    
     $run=(6<<27)+1;
  } else {
    warn "Table Cites has $cnt entries. Not initialized";
 }

    $cnt = 0;
   if($self->{dbdriver} =~ m/Oracle/){
    $sql="SELECT COUNT(fid) FROM Filesystems";
    $cntr=$self->Query($sql);
    foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
    }
}
  if ($cnt == 0) {
   my $time=time();
   $dbh->do("INSERT INTO Filesystems VALUES
    (1,'pcamsa0','/a0dah1','/AMS02/MC/ntuples',87,87,0,30,'Full',0,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (2,'pcamsa0','/a0dat1','/AMS02/MC/ntuples',53,43,10,20,'Active',0,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (3,'pcamsa0','/a0dat2','/AMS02/MC/ntuples',53,48,5,20,'Active',0,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (4,'pcamsp0','/p0dah1','/AMS02/MC/ntuples',97,87,5,20,'Active',0,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (5,'pcamsp0','/p0dat1','/AMS02/MC/ntuples',105,86,13,20,'Active',0,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (6,'pcamsp1','/p1dah1','/AMS02/MC/ntuples',65,60,1,20,'Full',0,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (7,'pcamsp1','/p1dat1','/AMS02/MC/ntuples',98,84,9,20,'Active',0,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (9,'pcamsf0','/vicepb','/afs/ams.cern.ch/data/data1/AMS02/MC',246,195,39,240,'Active',0,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (11,'pcamsf0','/f0dah1','/AMS02/MC/ntuples',220,158,51,60,'Active',0,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (12,'pcamsf2','/f2dat1','/AMS02/MC/ntuples',1040,889,140,20,'Active',1,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (13,'pcamsf2','/f2dah1','/AMS02/MC/ntuples',807,635,163,20,'Active',1,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (14,'pcamsf3','/f3dah1','/AMS02/MC/ntuples',221,201,9,120,'Active',1,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (15,'pcamsf4','/vicepa','/afs/ams.cern.ch/data/data2/AMS02/MC',346,275,53,246,'Active',1,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (16,'pcamsf4','/vicepc','/afs/ams.cern.ch/data/data3',523,52,466,480,'Active',1,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
    (17,'pcamsf5','/f5dah1','/AMS02/MC/ntuples',228,190,95,40,'Active',1,$time)")
    or die "cannot do: ".$dbh->errstr();    
   $dbh->do("INSERT INTO Filesystems VALUES
     (20,'pcamst0','/t0dah1','/AMS02/MC/ntuples',221,178,32,20,'Full',0,$time)")
    or die "cannot do: ".$dbh->errstr();    
  } else {
    warn "Table Filesystems has $cnt entries. Not initialized";
  }
 
    $cnt = 0;
     if($self->{dbdriver} =~ m/Oracle/){
    $sql="SELECT COUNT(mid) FROM Mails";
    $cntr=$self->Query($sql);
    foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
    }
}
   if ($cnt == 0) {
    my $address='v.choutko@cern.ch';           
    my $alias='vitali.choutko@cern.ch';
    my $time = time();        
   $dbh->do("insert into Mails values(1,'$address','$alias','Vitali Choutko',1,1,1,'Active',0,$time,0,0)")or die "cannot do: ".$dbh->errstr();    
     $address='vitali@afl3u1.cern.ch';        
   $dbh->do("insert into Mails values(2,'$address',NULL,'Vitali Choutko',1,0,2,'Active',0,$time,0,0)")or die "cannot do: ".$dbh->errstr();    
     $address='a.klimentov@cern.ch';        
     $alias='alexei.klimentov@cern.ch';
   $dbh->do("insert into Mails values(3,'$address','$alias','Alexei Klimentov',1,1,1,'Active',0,$time,0,0)")or die "cannot do: ".$dbh->errstr();    
     $address='biland@particle.phys.ethz.ch';
     $alias='adrian.biland@cern.ch';      
  $dbh->do("insert into Mails values(4,'$address','$alias','Adrian Biland',1,0,4,'Active',0,$time,0,0)")or die "cannot do: ".$dbh->errstr();    
    $address='diego.casadei@bo.infn.it';
    $alias='diego.casadei@cern.ch'; 
  $dbh->do("insert into Mails values(5,'$address','$alias','Diego Casadei',1,0,3,'Active',0,$time,0,0)")or die "cannot do: ".$dbh->errstr();    
    $address='davide.grandi@mib.infn.it';
    $alias='davide.grandi@cern.ch'; 
  $dbh->do("insert into Mails values(6,'$address','$alias','Davide Grandi',1,0,5,'Active',0,$time,0,0)")or die "cannot do: ".$dbh->errstr();    
    $address='evgueni.choumilov@cern.ch';
    $alias='e.choumilov@cern.ch'; 
  $dbh->do("insert into Mails values(7,'$address','$alias','Eugeni Choumilov',0,0,2,'Active',0,$time,0,0)")or die "cannot do: ".$dbh->errstr();    
    $address='gnkim@knu.ac.kr';
    $alias='guinyun.kim@cern.ch'; 
  $dbh->do("insert into Mails values(8,'$address','$alias','Guinyun Kim',1,0,6,'Active',0,$time,0,0)")or die "cannot do: ".$dbh->errstr();    
    $time=time();
    warn $time;
#find responsible
    $sql="select cid from Cites";
    my $ret=$self->Query($sql);
    $sql="select cid,mid from Mails where rsite=1";
    my $rem=$self->Query($sql);
    foreach my $cid (@{$ret}){ 
        foreach my $cmid (@{$rem}){
            if($cid->[0]==$cmid->[0]){
                my $mid=$cmid->[1];
                $sql="update Cites set mid=$mid where cid=".$cid->[0];
                $self->Update($sql);
#                last;
            }
        }
    }
  } else {
    warn "Table Mails has $cnt entries. Not initialized";
 }


}


sub Query{
    my $self=shift;
    my $query=shift;
    my $dbh=$self->{dbhandler};
    my $sth=$dbh->prepare($query) or die "Cannot prepare query : $query ".$dbh->errstr();
    $sth->execute or die "Cannot execute ".$dbh->errstr();
    my $ret=$sth->fetchall_arrayref();     
    $sth->finish();
    return $ret;
    
    
} 
sub QueryAll{
    my $self=shift;
    my $query=shift;
    my $dbh=$self->{dbhandler};
    my $sth=$dbh->prepare($query) or die "Cannot prepare ".$dbh->errstr();
    $sth->execute or die "Cannot execute ".$dbh->errstr();
    my @fields;
    my $row={};
    my @ret;
     
    while ($row=$sth->fetchrow_hashref()){
        @fields=keys %{$row};
        my @res=values %{$row};
        push @ret, \@res;
    }
    return (\@ret,\@fields);
    
    
} 
sub Update{
    my $self=shift;
    my $query=shift;
    my $dbh=$self->{dbhandler};
    $dbh->do($query) or die "Cannot do query : $query ".$dbh->errstr();
}


# this function sets up the necessary environement variables
# to be able to connect to Oracle
sub set_oracle_env {
    $ENV{"ORACLE_HOME"}='/afs/cern.ch/project/oracle/@sys/prod';
    $ENV{"TNS_ADMIN"}='/afs/cern.ch/project/oracle/admin';
    $ENV{"LD_LIBRARY_PATH"}=$ENV{"ORACLE_HOME"}."/lib";
    1;
}

sub DESTROY{

    my $self=shift;
    if(defined $self->{dbhandler}){
        $self->{dbhandler}->disconnect();
    }
    warn "DESTROYING $self";
}
