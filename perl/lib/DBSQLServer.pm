
# $Id: DBSQLServer.pm,v 1.1 2002/02/20 18:00:24 choutko Exp $

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

@DBSQLServer::EXPORT= qw(new Connect QueryAll Query Update);
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
    if(defined $dir){
        $self->{dbfile}=$dir.'/'.$self->{dbfile};
    }        
    else{
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
    $self->{dbhandler}=DBI->connect('DBI:'.$self->{dbdriver}.$self->{dbfile},,{PrintError => 1, AutoCommit => 1}) or die "Cannot connect: ".$DBI::errstr;
    if($self->{dbinit}){
      $self->Create();
    }
    else{
        return 1;
    }
}


sub Create{
# creates a database
    my $self=shift;
    my $dbh=$self->{dbhandler};


    my @tables=("filesystems", "Cites","Mails" ,"Jobs", "Servers", "Runs","Ntuples");

    my @createtables=("    CREATE TABLE filesystems
    (fid         CHAR(4) NOT NULL,
     path    CHAR(255),
     totalsize    INT,
     occupied     INT,
     free         INT,
     allowed      INT,
     mode    CHAR(2))",
     "CREATE TABLE Cites
     (cid      INT NOT NULL,
      name     VARCHAR(64),
      mid      INT,
      status   VARCHAR(255),
      maxrun   INT,
      state    INT)",
      "CREATE TABLE Mails
      (mid INT NOT NULL,
       address VARCHAR(255),
       alias   VARCHAR(255),
       name    VARCHAR(64),
       rSite   INT,
       rServer INT,
       cid     INT,
       status  VARCHAR(16),
       requests INT)",
      "CREATE TABLE Jobs 
      (jid     INT NOT NULL,
       jobname VARCHAR(255),
       mid     INT,
       cid     INT,
       time    INT,
       content TEXT )",
      "CREATE TABLE Servers
       (dbfilename VARCHAR(255) NOT NULL,
        IORS   TEXT,
        IORP   TEXT,
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
         date   INT,
         sizemb   INT,
         status   VARCHAR(64),
         path   VARCHAR(255))"
         );
# status ntuples:  OK, Unchecked, Errors
#      
#   check if tables exist
    my $i=0;
    foreach my $table (@tables){
     if($self->{dbinit}<2){
       my $ok =$dbh->do("select * from $table");
      if(defined $ok){
        die "Table $table already exist";
      }
     }
     if($self->{dbdriver} =~ m/CSV/){
         system "mkdir -p $self->{dbfile}";
         system "rm -f $self->{dbfile}"."/"."$table";
     }
        
    my $sth=$dbh->prepare($createtables[$i]) or die "cannot prepare: ".$dbh->errstr();
    $sth->execute() or die "cannot execute: ".$dbh->errstr();
    $sth->finish();    
     if($self->{dbdriver} =~ m/CSV/){
         system "chmod o+w $self->{dbfile}"."/"."$table";
     }
     $i=$i+1;
 }

# initialize 
    my $run=100;
  $dbh->do("insert into Cites values(1,'cern',0,
            'local',$run,0)")or die "cannot do: ".$dbh->errstr();    
     $run=(1<<27)+1;
  $dbh->do("insert into Cites values(2,'test',0,
            'remote',$run,0)")or die "cannot do: ".$dbh->errstr();    
     $run=(2<<27)+1;
  $dbh->do("insert into Cites values(3,'bolo',0,
            'remote',$run,0)")or die "cannot do: ".$dbh->errstr();    
     $run=(3<<27)+1;
  $dbh->do("insert into Cites values(4,'ethz',0,
            'remote',$run,0)")or die "cannot do: ".$dbh->errstr();    
    my $address='v.choutko@cern.ch';           
    my $alias='vitali.choutko@cern.ch';        
  $dbh->do("insert into Mails values(1,'$address','$alias','Vitali Choutko',1,1,1,'Active',0)")or die "cannot do: ".$dbh->errstr();    
    $address='vitali@afl3u1.cern.ch';        
  $dbh->do("insert into Mails values(2,'$address',NULL,'Vitali Choutko',1,0,2,'Active',0)")or die "cannot do: ".$dbh->errstr();    
    $address='a.klimentov@cern.ch';        
    $alias='alexei.klimentov@cern.ch';
  $dbh->do("insert into Mails values(3,'$address','$alias','Alexei Klimentov',1,1,1,'Active',0)")or die "cannot do: ".$dbh->errstr();    
    $address='biland@particle.phys.ethz.ch';
    $alias='adrian.biland@cern.ch';      
  $dbh->do("insert into Mails values(4,'$address','$alias','Adrian Biland',1,0,4,'Active',0)")or die "cannot do: ".$dbh->errstr();    
    $address='diego.casadei@bo.infn.it';
    $alias='diego.casadei@cern.ch'; 
  $dbh->do("insert into Mails values(5,'$address','$alias','Diego Casadei',1,0,3,'Active',0)")or die "cannot do: ".$dbh->errstr();    
    $address='evgueni.choumilov@cern.ch';
    $alias='e.choumilov@cern.ch'; 
  $dbh->do("insert into Mails values(6,'$address','$alias','Eugeni Choumilov',0,0,2,'Active',0)")or die "cannot do: ".$dbh->errstr();    
  my $time=time();
    warn $time;
#find responsible
    my $sql="select cid from Cites";
    my $ret=$self->Query($sql);
    $sql="select cid,mid from Mails where rSite=1";
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
#    open FILE,"<./home/httpd/cgi-bin/AMS02MCUploads/test.134217729.ideal.job";
#    my $buf;
#    read(FILE,$buf,1638400);
#         $buf=~s/\$/\\\$/g;
#         $buf=~s/\"/\\\"/g;
#         $buf=~s/\(/\\\(/g;
#         $buf=~s/\)/\\\)/g;
#         $buf=~s/\'/\\\\\'/g;
#   $sql="insert into Jobs values(43342535,'4retg.wefwe.job',1,3,'$buf')";
#
#   $self->Update($sql);


}


sub Query{
    my $self=shift;
    my $query=shift;
    my $dbh=$self->{dbhandler};
    my $sth=$dbh->prepare($query) or die "Cannot prepare ".$dbh->errstr();
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
    $dbh->do($query) or die "Cannot do ".$dbh->errstr();
}

sub DESTROY{

    my $self=shift;
    if(defined $self->{dbhandler}){
        $self->{dbhandler}->disconnect();
    }
    warn "DESTROYING $self";
}

