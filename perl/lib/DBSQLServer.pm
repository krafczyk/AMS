# $Id: DBSQLServer.pm,v 1.14 2002/03/26 19:21:21 alexei Exp $

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
    my $oracle="/home/httpd/cgi-bin/mon/lib/.oracle.oracle";
    open(FILE,"<".$oracle) or die "Unable to open file $oracle \n";
    while  (<FILE>){
        $pwd=$_;
    }
    close FILE;
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


    my @tables=("filesystems", "Cites","Mails" ,"Jobs", "Servers", "Runs","Ntuples","DataSets", "Environment");
    my @createtables=("    CREATE TABLE filesystems
    (fid         CHAR(4) NOT NULL,
     path    CHAR(255),
     totalsize    INT,
     occupied     INT,
     free         INT,
     allowed      INT,
     modetype    CHAR(2))",
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
       timestamp INT)",
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
       timestamp INT)",
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
         path   VARCHAR(255))",
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
     if($self->{dbinit}<2){
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
     $dbh->do("insert into Environment values('AMSDataDir','/f2dat1/AMS01/AMSDataDir')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('CERN_ROOT','/cern/2001')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('UploadsDir','/home/httpd/cgi-bin/AMS02MCUploads')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('AMSSoftwareDir','DataManagement')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('DataSets','DataSets')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('gbatch','exe/linux/gbatch-orbit.exe')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('ntuplevalidator','exe/linux/fastntrd.exe')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('filedb','ams02mcdb.tar')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('dbversion','v3.00')") or die "cannot do: ".$dbh->errstr();     
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
  } else {
    warn "Table Cites has $cnt entries. Not initialized";
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
   $dbh->do("insert into Mails values(1,'$address','$alias','Vitali Choutko',1,1,1,'Active',0,$time)")or die "cannot do: ".$dbh->errstr();    
     $address='vitali@afl3u1.cern.ch';        
   $dbh->do("insert into Mails values(2,'$address',NULL,'Vitali Choutko',1,0,2,'Active',0,$time)")or die "cannot do: ".$dbh->errstr();    
     $address='a.klimentov@cern.ch';        
     $alias='alexei.klimentov@cern.ch';
   $dbh->do("insert into Mails values(3,'$address','$alias','Alexei Klimentov',1,1,1,'Active',0,$time)")or die "cannot do: ".$dbh->errstr();    
     $address='biland@particle.phys.ethz.ch';
     $alias='adrian.biland@cern.ch';      
  $dbh->do("insert into Mails values(4,'$address','$alias','Adrian Biland',1,0,4,'Active',0,$time)")or die "cannot do: ".$dbh->errstr();    
    $address='diego.casadei@bo.infn.it';
    $alias='diego.casadei@cern.ch'; 
  $dbh->do("insert into Mails values(5,'$address','$alias','Diego Casadei',1,0,3,'Active',0,$time)")or die "cannot do: ".$dbh->errstr();    
    $address='evgueni.choumilov@cern.ch';
    $alias='e.choumilov@cern.ch'; 
  $dbh->do("insert into Mails values(6,'$address','$alias','Eugeni Choumilov',0,0,2,'Active',0,$time)")or die "cannot do: ".$dbh->errstr();    
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
