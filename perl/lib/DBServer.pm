# $Id: DBServer.pm,v 1.5 2001/02/09 13:08:49 choutko Exp $

package DBServer;
use CORBA::ORBit idl => [ '../include/server.idl'];
use Error qw(:try);
use Carp;
use strict;
use lib::CID;
use lib::ActiveClient;
use lib::POADBServer;
use Carp;
use Fcntl ":flock";
use Fcntl;
use POSIX qw(tmpnam);
use MLDBM qw(DB_File Storable);


@DBServer::EXPORT= qw(new Update Init);
my %lock=(
 dblock=>0,
 uid=>0,
 time=>0,
 timeout=>60,
          );
my %fields=(
     cid=>undef,
     start=>undef,
     ac=>undef,
     orb=>undef,
     root_poa=>undef,
     mypoadbserver=>undef,
     myref=>undef,
     myior=>undef,
     arsref=>[],
     arpref=>[],
     nhl=>undef,
     ahls=>undef,
     ahlp=>undef,
     acl=>undef,
     aml=>undef,
     asl=>undef,
     adbsl=>undef,
     acl_maxc=>0,
     aml_maxc=>0,
     asl_maxc=>0,
     adbsl_maxc=>0,
     nsl=>undef,
     ncl=>undef,
     nkl=>undef,
     rtb=>undef,
     rtb_maxr=>undef,
     dsti=>undef,
     dsts=>undef,
     db=>undef,
     env=>undef,
     rn=>undef,
     dbfile=>undef,
     ok=>0,
             );

sub new{
    my $type=shift;
    my $self={
        %fields,
    };
$self->{start}=time();
$self->{cid}=new CID("DBServer");    
$self->{orb} = CORBA::ORB_init("orbit-local-orb");
$self->{root_poa} = $self->{orb}->resolve_initial_references("RootPOA");
$self->{root_poa}->_get_the_POAManager->activate;
$self->{mypoadbserver}=new POADBServer;
my $id = $self->{root_poa}->activate_object($self->{mypoadbserver});
$self->{myref}=$self->{root_poa}->id_to_reference ($id);
$self->{myior} = $self->{orb}->object_to_string ($self->{myref});
$self->{ac}= new ActiveClient($self->{myior},$self->{start},$self->{cid});

    my $mybless=bless $self,$type;
    if(ref($DBServer::Singleton)){
        croak "Only Single DBServer Allowed\n";
    }
$DBServer::Singleton=$mybless;
return $mybless;
}


sub UpdateEverything{
 my $ref=shift;
 my $arsref;
 foreach $arsref (@{$ref->{arsref}}){
     my $length;
     my $ahl;
     try{
         my %cid=%{$ref->{cid}};
         $cid{Type}="Server";
       ($length,$ahl)=$arsref->getAHS(\%cid);
         if($length==0){
             $ref->{ahls}=undef;
         }
         else {
             $ref->{ahls}=$ahl;
         }
         my $maxc=0;
      ($length,$ahl,$maxc)=$arsref->getACS(\%cid);
         if($length==0){
             $ref->{asl}=undef;
              
         }
         else {
             $ref->{asl}=$ahl;
             $ref->{asl_maxc}=$maxc;
         }
      ($length,$ahl)=$arsref->getNHS(\%cid);
         if($length==0){
             $ref->{nhl}=undef;
         }
         else {
             $ref->{nhl}=$ahl;
         }
        ($length,$ahl)=$arsref->getNC(\%cid);
         if($length==0){
             $ref->{nsl}=undef;
         }
         else {
             $ref->{nsl}=$ahl;
         }
        ($length,$ahl)=$arsref->getNK(\%cid);
         if($length==0){
             $ref->{nkl}=undef;
         }
         else {
             $ref->{nkl}=$ahl;
         }
        ($length,$ahl)=$arsref->getEnv(\%cid);
         if($length==0){
             $ref->{env}=undef;
         }
         else {
             $ref->{env}=$ahl;
         }
         my $path="AMSDataDir";
         my $addpath="/DataBase/";
         my ($ok, $dhl)=$arsref->getDBSpace(\%cid,$path,$addpath);
         $ref->{db}=$dhl;
         $path="AMSProdOutputDir";
         $addpath="/";
          ($ok, $dhl)=$arsref->getDBSpace(\%cid,$path,$addpath);
         $ref->{rn}=$dhl;
         $cid{Type}="Producer";
      ($length,$ahl,$maxc)=$arsref->getACS(\%cid);
         if($length==0){
             $ref->{acl}=undef;
         }
         else {
             $ref->{acl}=$ahl;
             $ref->{acl_maxc}=$maxc;
         }
       ($length,$ahl)=$arsref->getAHS(\%cid);
         if($length==0){
             $ref->{ahlp}=undef;
         }
         else {
             $ref->{ahlp}=$ahl;
         }
        ($length,$ahl)=$arsref->getNC(\%cid);
         if($length==0){
             $ref->{ncl}=undef;
         }
         else {
             $ref->{ncl}=$ahl;
         }
         $cid{Type}="DBServer";
      ($length,$ahl,$maxc)=$arsref->getACS(\%cid);
         if($length==0){
             $ref->{adbsl}=undef;
         }
         else {
             $ref->{adbsl}=$ahl;
             $ref->{adbsl_maxc}=$maxc;
         }
         goto NEXT;
         $cid{Type}="Monitor";
      ($length,$ahl,$maxc)=$arsref->getACS(\%cid);
         if($length==0){
             $ref->{aml}=undef;
         }
         else {
             $ref->{aml}=$ahl;
             $ref->{aml_maxc}=$maxc;
         }
         goto NEXT;
     }
     catch CORBA::SystemException with{
     };
 }
       return 0;
NEXT:
 foreach $arsref (@{$ref->{arpref}}){
     my $length;
     my $ahl;
     try{
         my %cid=%{$ref->{cid}};
         $cid{Type}="Producer";

         my $maxr=0;
        ($length,$ahl)=$arsref->getRunEvInfoS(\%cid, \$maxr);
         if($length==0){
             $ref->{rtb}=undef;
         }
         else {
             $ref->{rtb}=$ahl;
             $ref->{rtb_maxr}=$maxr;
         }


        ($length,$ahl)=$arsref->getDSTInfoS(\%cid);
         if($length==0){
             $ref->{dsti}=undef;
         }
         else {
             $ref->{dsti}=$ahl;
         }
         
        ($length,$ahl)=$arsref->getDSTS(\%cid);
         if($length==0){
             $ref->{dsts}=undef;
         }
         else {
             $ref->{dsts}=$ahl;
         }

         goto LAST;
     }
     catch CORBA::SystemException with{
     };
 }

  return 0; 
LAST:
 return 1;
}
sub DESTROY{
    my $self=shift;
    warn "DESTROYING $self";
}

sub UpdateARS{
 my $ref=shift;
 my $arsref;
 if(not $ref->{ok}){
     return 0;
 }
 foreach $arsref (@{$ref->{arsref}}){
     try{
         my %cid=%{$ref->{cid}};
         $cid{Type}="Server";
      my ($length,$pars)=$arsref->getARS(\%cid,"Any",0,1);
         if($length==0 ){
            carp "updars returns zero \n";
            return 0;
        }
        for(;;){
             my $old=shift @{$ref->{arsref}};
          if(ref($old)){
            undef $old;
          }
          else{
            last;
          }
         }
         my $ior;
         foreach $ior (@$pars){
             try{
              my $newref=$ref->{orb}->string_to_object($ior->{IOR});
              if(rand >0.5){
                  unshift @{$ref->{arsref}}, $newref;
              }
              else{
                  push @{$ref->{arsref}}, $newref;
              }
             }
             catch CORBA::SystemException with{
               carp "getars 2 oops SystemException Error "."\n";
             };

         }   
         $cid{Type}="Producer";
         ($length,$pars)=$arsref->getARS(\%cid,"Any",0,1);

         if($length==0 ){
            carp "updars returns zero \n";
            return 0;
        }
        for(;;){
             my $old=shift @{$ref->{arpref}};
          if(ref($old)){
            undef $old;
          }
          else{
            last;
          }
         }
         foreach $ior (@$pars){
             try{
              my $newref=$ref->{orb}->string_to_object($ior->{IOR});
              if(rand >0.5){
#                  warn "unshift"; 
                  unshift @{$ref->{arpref}}, $newref;
              }
              else{
#                  warn "push"; 
                  push @{$ref->{arpref}}, $newref;
              }
             }
             catch CORBA::SystemException with{
               carp "getars 3 oops SystemException Error "."\n";
             };

         }   

         last;
     }
    catch CORBA::SystemException with{
     carp "getars oops SystemException Error "."\n";
 };
 }
 return 1;
}

sub Update{
}




sub Exiting{
    my $ref=shift;
    my $message=shift;
    my $status=shift;
        my $arsref;
                $ref->{cid}->{Status}=$status;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                 
                $arsref->Exiting(\%cid,$message,$status);
            }
            catch CORBA::SystemException with{
                warn "Exiting corba exc";
            };
        }
}


sub SendId{
    my $ref=shift;
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                my $hash=\%cid;
                my ($ok,$ok2)=$arsref->sendId(\$hash,30);
                if (not $ok){
                    sleep 10;
                    ($ok,$ok2)=$arsref->sendId(\$hash,30);
                if (not $ok){
                  $ref->Exiting(" Server Requested To Exit ","SInExit");
                   return 0;
                }
                }
                $ref->{cid}=$hash;
                my %ac=%{$ref->{ac}};
                my $hash_ac=\%ac;
                $arsref->sendAC($hash,\$hash_ac,"Update");
                $ref->{ac}=$hash_ac;
            }
            catch CORBA::SystemException with{
                warn "Exiting corba exc";
            };
        }
    $ref->{myref}->sendACPerl($ref->{cid},$ref->{ac},"Update");
    return 1;
}






sub Init{
my $ior;
my $amsprodlogdir;
my $amsprodlogfile;
my $uid;
foreach my $chop  (@ARGV){
    if($chop =~/^-U/){
        $uid=unpack("x2 A*",$chop);
    }
    elsif($chop =~/^-i/){
     $ior=unpack("x2 A*",$chop);
    }
    elsif($chop=~/^-A/){
     $amsprodlogdir=unpack("x2 A*",$chop);
    }
    elsif($chop=~/^-F/){
     $amsprodlogfile=unpack("x2 A*",$chop);
    }
}


 my $ref=shift;
 $ref->{cid}->{uid}=$uid;
 $ref->{ok}=0;
 if(not defined $ior){ 
     return 0;
 }
 chomp $ior;
 my $tm={};
 try{
  $tm = $ref->{orb}->string_to_object($ior);
  $ref->{arsref}=[$tm];
     $ref->{ok}=1;
 }
 catch CORBA::MARSHAL with{
     carp "MARSHAL Error "."\n";
 }
 catch CORBA::COMM_FAILURE with{
     carp "Commfailure Error "."\n";
 }
 catch CORBA::SystemException with{
     carp "SystemException Error "."\n";
 };
 if(not $ref->{ok}){
   return 0;
 }
 if(not $ref->UpdateARS()){
     $ref->Exiting(" Unable to get ARS From Server ","CInAbort");
     return 0;
 }
   $ref->{dbfile}=$amsprodlogfile;
  if(not $ref->InitDBFile($amsprodlogdir)){
      my $dbfile=$ref->{dbfile};
     $ref->Exiting(" Unable to open  DB File $dbfile","CInAbort");
     return 0;
  }
  return $ref->SendId();

}
sub InitDBFile{
    my %hash;
    local *DBM;
    my $db;
    my $init=0;
    my $ref=shift;
    my $amsprodlogdir;
    if(ref($ref)){
     $amsprodlogdir=shift;
    }
     else{
         $ref=shift;
      }
    if (defined $ref->{dbfile}){
      $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
    }
    else{
        my $tmpname=tmpnam();
        $ref->{dbfile}=$amsprodlogdir."/".unpack("x5 A*",$tmpname);
        $db=tie %hash, "MLDBM",$ref->{dbfile},O_CREAT | O_RDWR, 0644 ;
        $init=1;
    }
     if(not $db){
      $ref->Exiting("Unable to tie db_file $ref->{dbfile}","CInAbort");
      return 0;
      }
      my $fd=$db->fd;
      my $ok=open DBM, "<&=$fd";
      if( not $ok){
       $ref->Exiting("Unable to dup DBM for lock: $!","CInAbort");
       return 0;
      }
     my $ntry=0;
     until (flock DBM, LOCK_EX|LOCK_NB){
         sleep 1;
         $ntry=$ntry+1;
         if($ntry>10){
           $ref->Exiting("Unable to get lock for $ref->{dbfile}","CInAbort");
          return 0;
         }
     }
      undef $db;
    if($init){
        if(not $ref->UpdateEverything()){
          $ref->Exiting("Unable to get Tables From Server","CInAbort");
          return 0;
        }       
        $hash{nhl}=$ref->{nhl};
        $hash{ahls}=$ref->{ahls};
        $hash{ahlp}=$ref->{ahlp};
        $hash{acl}=$ref->{acl};
        $hash{aml}=$ref->{aml};
        $hash{asl}=$ref->{asl};
        $hash{asl_maxc}=$ref->{asl_maxc};
        $hash{adbsl}=$ref->{adbsl};
        $hash{adbsl_maxc}=$ref->{adbsl_maxc};
        $hash{acl_maxc}=$ref->{acl_maxc};
        $hash{aml_maxc}=$ref->{aml_maxc};
        $hash{nsl}=$ref->{nsl};
        $hash{nkl}=$ref->{nkl};
        $hash{ncl}=$ref->{ncl};
        $hash{dsti}=$ref->{dsti};
        $hash{dsts}=$ref->{dsts};
        $hash{rtb}=$ref->{rtb};
        $hash{env}=$ref->{env};
        $hash{db}=$ref->{db};
        $hash{rn}=$ref->{rn};
        $hash{rtb_maxr}=$ref->{rtb_maxr};
    }
    else{
        $ref->{nhl}=$hash{nhl};
        $ref->{ahls}=$hash{ahls};
        $ref->{ahlp}=$hash{ahlp};
        $ref->{acl}=$hash{acl};
        $ref->{aml}=$hash{aml};
        $ref->{asl}=$hash{asl};
        $ref->{asl_maxc}=$hash{asl_maxc};
        $ref->{adbsl}=$hash{adbsl};
        $ref->{adbsl_maxc}=$hash{adbsl_maxc};
        $ref->{acl_maxc}=$hash{acl_maxc};
        $ref->{aml_maxc}=$hash{aml_maxc};
        $ref->{nsl}=$hash{nsl};
        $ref->{nkl}=$hash{nkl};
        $ref->{ncl}=$hash{ncl};
        $ref->{dsts}=$hash{dsts};
        $ref->{dsti}=$hash{dsti};
        $ref->{env}=$hash{env};
        $ref->{db}=$hash{db};
        $ref->{rn}=$hash{rn};
        $ref->{rtb}=$hash{rtb};
        $ref->{rtb_maxr}=$hash{rtb_maxr};
    }
    untie %hash;
    return 1;

}


sub OpenDBFile{
    my %hash;
    local *DBM;
    my $db;
    my ($ref,$amsprodlogdir)=@_;
    if (defined $ref->{dbfile}){
      $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
    }
    else{
        return 0;
    }
     if(not $db){
      return 0;
      }
      my $fd=$db->fd;
      my $ok=open DBM, "<&=$fd";
      if( not $ok){
       untie %hash;
       return 0;
      }
     my $ntry=0;
     until (flock DBM, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
          untie %hash;
          return 0;
         }
     }
#      undef $db;
    return 1,$db,%hash;

}

