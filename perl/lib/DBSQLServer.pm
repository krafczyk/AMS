# $Id: DBSQLServer.pm,v 1.44 2003/04/24 17:09:53 alexei Exp $

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
# Apr  2, 2003 a.k. create indecies for RNDM table (it takes a while)
#                   insert CITES and MAILS info from ../doc/mc.cites.mails
#                          FILESYSTEMS          from ../doc/mc.filesystems
# Apr 21, 2003 a.k.        JournalFiles         from ../doc/mc.journals
# to add  - standalone/client type,
#          CPU, elapsed time, cite and host info into Job table
#
#  Apr 24, 2003. ak. Tables : Jobs, Runs, RNDM - add primary key
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
use POSIX qw(strtod);             
use MLDBM qw(DB_File Storable);
use DBI;
use Time::localtime;

my $MCCitesMailsFile = "../doc/mc.cites.mails"; # file with cites and mails definitions
my $MCFilesystems="../doc/mc.filesystems";      # file with filesystems definitions

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


    my @tables=("Filesystems", "Cites","Mails" ,"Jobs", "RNDM","Servers", "Runs","Ntuples","DataSets", "DataSetFiles", "Environment", "Journals","ProductionSet");
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

     "CREATE TABLE Journals
     (cid       INT NOT NULL ,
      dirpath   VARCHAR(255),
      lastfile  VARCHAR(255),
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
      (jid     INT NOT NULL primary key,
       jobname   VARCHAR(255),
       mid       INT,
       cid       INT,
       did       INT,
       time      INT,
       triggers  INT,
       timeout   INT,
       content   TEXT,
       timestamp INT,
       nickname VARCHAR(80),
        cite      VARCHAR(20),
        host      VARCHAR(20),
        events    INT,
        errors    int,
        cputime   int,
        elapsed   int,
       jobtype   VARCHAR(20))",

      "CREATE TABLE RNDM
      (rid     INT NOT NULL primary key,
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
        (run   INT NOT NULL primary key,
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
          name   VARCHAR(255),
          timestamp int)",
        "CREATE TABLE ProductionSet 
         (did        INT NOT NULL,
          name       VARCHAR(20),
          begin      int,
          end        int,
          status     char(10))",
        "CREATE TABLE DataSetFiles 
         (did        INT NOT NULL,
          name       VARCHAR(255),
          filename   VARCHAR(255),
          timestamp int)",
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

# my $RNDMTable="/afs/cern.ch/user/b/biland/public/AMS/rndm.seeds";
my $RNDMTable="/scratchA/groupA/biland/seed_1E12";

my $cntr = 0;
my $cnt  = 0;
my $sql;


   if($self->{dbdriver} =~ m/Oracle/){
    $sql="SELECT COUNT(rid) FROM RNDM";
    $cntr=$self->Query($sql);
     foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
     }
    }
    if ($cnt == 0) {
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
   }

# initialize 
     if($self->{dbdriver} =~ m/Oracle/){
    $sql="SELECT COUNT(mykey) FROM Environment";
    $cntr=$self->Query($sql);
    foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
    }
}
    if ($cnt == 0) {
     $dbh->do("insert into Environment values('AMSDataDir','/afs/ams.cern.ch/AMSDataDir')") or die "cannot do: ".$dbh->errstr();     
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
# ProductionSet Table
    $cnt=0;
   if($self->{dbdriver} =~ m/Oracle/){
      $sql = "SELECT COUNT(did) from ProductionSet";
      $cntr=$self->Query($sql);
      foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
      }
    } 
    if ($cnt == 0) {
     $dbh->do("insert into ProductionSet values(1,'2003Trial',1049891834,0,'Active')") 
           or die    "cannot do: ".$dbh->errstr();    
 }
#
    $cnt = 0;
   if($self->{dbdriver} =~ m/Oracle/){
    $sql="SELECT COUNT(cid) FROM Cites";
    $cntr=$self->Query($sql);
    foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
    }
}
  if ($cnt == 0) {
   $dbh->do("delete journals") or die "cannot do: ".$dbh->errstr();    
   my $time=time();
   my $run=110;
   my $n  =  0;
   open(FILE,"<",$MCCitesMailsFile) or die "Unable to open $MCFilesystems";
   my @lines = <FILE>;
   close FILE;
   my $mails = 0;
   my $cites = 0;
   foreach my $line (@lines) {
   if ($line =~/^#/) {
     if ($line =~/MAILS/) {
         $mails = 1;
         $cites = 0;
     } 
     if ($line =~/CITES/) {
         $cites = 1;
         $mails = 0
     } 
   } else {
     if ($cites == 1) {
       my ($description,$name,$cid,$stat,$status,$journal,@junk) = split ":",$line;
       if (defined $description and defined $status) {
         $description = trimblanks($description);
         $name        = trimblanks($name);
         $cid         = strtod($cid);
         $stat        = strtod($stat);
         $status      = trimblanks($status);
         $journal     = trimblanks($journal);
         $dbh->do("insert into Cites values($cid,$name,0,$status,$run,$stat,$description,$time)")
         or die "cannot do: ".$dbh->errstr();
         my $dirpath=$journal."/".$name;    
         $dbh->do("insert into journals values($cid,$dirpath,' ',$time)")    
         or die "cannot do: ".$dbh->errstr();    
         $n++;
         $run=($n<<27)+1;
     }
    }
   }
  } # foreach my $line
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
   open(FILE,"<",$MCFilesystems) or die "Unable to open $MCFilesystems";
   my @lines = <FILE>;
   close FILE;

   my $fs = 0;
   my $fid= 0;
   foreach my $line (@lines) {
    if ($line =~/^#/) {
     if ($line =~/FILESYSTEMS/) {
         $fs = 1;
     } 
    } else {
     if ($fs == 1) {
         my ($host,$disk,$path,$total,$occup,$avail,$allow,$status,$prio,@junk) = split ":",$line;
         if (defined $host and defined $prio) {
          $fid++;
          $host    = trimblanks($host);
          $disk    = trimblanks($disk);
          $path    = trimblanks($path);
          $total   = strtod($total);
          $occup   = strtod($occup);
          $avail   = strtod($avail);
          $allow   = strtod($allow);
          $status  = trimblanks($status);
          $prio    = strtod($prio);
#          print "$host,$disk,$path,$total,$occup,$avail,$allow,$status\n";
          $dbh->do("INSERT INTO Filesystems VALUES
           ($fid,$host,$disk,$path,$total,$occup,$avail,$allow,$status,$prio,$time)")
           or die "cannot do: ".$dbh->errstr();    
      }
     }
   }
  }
  } else {
    warn "Table Filesystems has $cnt entries. Not initialized";
  }
 
#
# INSERT INTO datasets
#
    $cnt = 0;
    if($self->{dbdriver} =~ m/Oracle/){
     $sql="SELECT COUNT(did) FROM Datasets";
     $cntr=$self->Query($sql);
     foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
     }
    }
    if ($cnt == 0) {
     my $did  = 0;
     my $timestamp = time();    
     my $dir  => undef;
     my $key='AMSDataDir';
     $sql="select myvalue from Environment where mykey='".$key."'";
     my $ret = $self->Query($sql);
     if( defined $ret->[0][0]){
      $dir=$ret->[0][0];
     }
     $key='AMSSoftwareDir';
     $sql="select myvalue from Environment where mykey='".$key."'";
     $ret = $self->Query($sql);
     if( defined $ret->[0][0]){
      $dir=$dir."/".$ret->[0][0];
     }
     if (defined $dir) {
      $dir="$dir/DataSets";  # top level directory for datasets
      opendir THISDIR ,$dir or die "unable to open $dir";
      my @allfiles= readdir THISDIR;
      closedir THISDIR;    
       foreach my $file (@allfiles){
        my $newfile="$dir/$file";
        if(readlink $newfile or  $file =~/^\./){
         next;
        }
          my $dataset={};
          my @tmpa;
          opendir THISDIR, $newfile or die "unable to open $newfile";
          my @jobs=readdir THISDIR;
          closedir THISDIR;
          foreach my $job (@jobs){
           if($job =~ /\.job$/){
            if($job =~ /^\./){
             next;
            }
           my $template={};
           my $full="$newfile/$job";
           my $buf;
           open(FILE,"<".$full) or die "Unable to open dataset file $full \n";
           read(FILE,$buf,1638400) or next;
           close FILE;
           my @sbuf=split "\n",$buf;
           my $datasetdesc= "generate";
           foreach my $line (@sbuf){
            if($line =~/$datasetdesc/){
              my ($junk,$dataset) = split "#",$line;                 
              $dbh->do(
                       "INSERT INTO datasets values($did,'$dataset',$timestamp)")
              or die "cannot do: ".$dbh->errstr();    
              $did++;
              last;
          }
         }
        }
       }
    }
      } else {
       die "$key is not defined in Environment table";
    }
 }
#
# end INSERT INTO datasets
#
#
# INSERT INTO DataSetFiles
#
    $cnt = 0;
    if($self->{dbdriver} =~ m/Oracle/){
     $sql="SELECT COUNT(did) FROM datasetfiles";
     $cntr=$self->Query($sql);
     foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
     }
    }
    if ($cnt == 0) {
     my $did  = 0;
     my $timestamp = time();    
     my $dir  => undef;
     my $key='AMSDataDir';
     $sql="select myvalue from environment where mykey='".$key."'";
     my $ret = $self->Query($sql);
     if( defined $ret->[0][0]){
      $dir=$ret->[0][0];
     }
     $key='AMSSoftwareDir';
     $sql="select myvalue from environment where mykey='".$key."'";
     $ret = $self->Query($sql);
     if( defined $ret->[0][0]){
      $dir=$dir."/".$ret->[0][0];
     }
     if (defined $dir) {
      $dir="$dir/DataSets";  # top level directory for datasets
      opendir THISDIR ,$dir or die "unable to open $dir";
      my @allfiles= readdir THISDIR;
      closedir THISDIR;    
       foreach my $file (@allfiles){
        my $newfile="$dir/$file";
        if(readlink $newfile or  $file =~/^\./){
         next;
        }
          my $dataset={};
          my @tmpa;
          opendir THISDIR, $newfile or die "unable to open $newfile";
          my @jobs=readdir THISDIR;
          closedir THISDIR;
          foreach my $job (@jobs){
           if($job =~ /\.job$/){
            if($job =~ /^\./){
             next;
            }
           my $template={};
           my $full="$newfile/$job";
           my $buf;
           open(FILE,"<".$full) or die "Unable to open dataset file $full \n";
           read(FILE,$buf,1638400) or next;
           close FILE;
           my @sbuf=split "\n",$buf;
           my $datasetdesc= "generate";
           foreach my $line (@sbuf){
            if($line =~/$datasetdesc/){
              my ($junk,$dataset) = split "#",$line;                 
              $dbh->do(
                       "INSERT INTO datasetfiles values($did,'$dataset','$job',$timestamp)")
              or die "cannot do: ".$dbh->errstr();    
              $did++;
              last;
          }
         }
        }
       }
    }
      } else {
       die "$key is not defined in Environment table";
    }
 }
#
# end INSERT INTO datasets
#
    $cnt = 0;
     if($self->{dbdriver} =~ m/Oracle/){
    $sql="SELECT COUNT(mid) FROM Mails";
    $cntr=$self->Query($sql);
    foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
    }
}
   if ($cnt == 0) {
    my $mid  = 1;
    my $time = time();    
    open(FILE,"<",$MCCitesMailsFile) or die "Unable to open $MCCitesMailsFile";
    my @lines = <FILE>;
    close FILE;
    my $mails = 0;
    my $cites = 0;             
    foreach my $line (@lines) {
    if ($line =~/^#/) {
     if ($line =~/MAILS/) {
         $mails = 1;
         $cites = 0;
     } 

     if ($line =~/CITES/) {
         $cites = 1;
         $mails = 0
     } 
    } else {
     if ($mails == 1) {
         my ($address,$name,$r,$cid,$rs,$alias,@junk) = split ":",$line;
         if (defined $address and defined $alias) {
          $address = trimblanks($address);
          $alias   = trimblanks($alias);
          $name    = trimblanks($name);
          $r       = strtod($r);
          $rs      = strtod($rs);
          $cid     = strtod($cid);
          $dbh->do
           ("insert into Mails values($mid,'$address','$alias',$name,$r,$rs,$cid,'Active',0,$time,0,0)")
           or die "cannot do: ".$dbh->errstr();    
          $mid++;
      }
     }
    }
   } # foreach my $line 
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

sub trimblanks {
    my @inp_string = @_;
    for (@inp_string) {
        s/^\s+//;        
        s/\s+$//;
    }
    return wantarray ? @inp_string : $inp_string[0];
}
