package Monitor;
use CORBA::ORBit idl => [ '../include/server.idl'];
use Error qw(:try);
use Carp;
use strict;
use lib::CID;
use lib::POAMonitor;
@Monitor::EXPORT= qw(new Update Connect);

my %fields=(
     cid=>undef,
     orb=>undef,
     root_poa=>undef,
     mypoamonitor=>undef,
     myref=>undef,
     myior=>undef,
     arsref=>[],
     arpref=>[],
     nhl=>undef,
     ahls=>undef,
     ahlp=>undef,
     acl=>undef,
     asl=>undef,
     nsl=>undef,
     ncl=>undef,
     nkl=>undef,
     rtb=>undef,
     dsti=>undef,
     dsts=>undef,
            );
sub new{
    my $type=shift;
    my $self={
        %fields,
    };
$self->{cid}=new CID;    
$self->{orb} = CORBA::ORB_init("orbit-local-orb");
$self->{root_poa} = $self->{orb}->resolve_initial_references("RootPOA");
$self->{root_poa}->_get_the_POAManager->activate;
$self->{mypoamonitor}=new POAMonitor;
my $id = $self->{root_poa}->activate_object($self->{mypoamonitor});
$self->{myref}=$self->{root_poa}->id_to_reference ($id);
$self->{myior} = $self->{orb}->object_to_string ($self->{myref});

    my $mybless=bless $self,$type;
    if(ref($Monitor::Singleton)){
        croak "Only Single Monitor Allowed\n";
    }
$Monitor::Singleton=$mybless;
return $mybless;
}

sub Connect{
 my $ref=shift;
 my $ior= shift @ARGV;
 unshift @ARGV, $ior;
 chomp $ior;
 my $tm={};
 try{
  $tm = $ref->{orb}->string_to_object($ior);
 }
 catch CORBA::MARSHAL with{
     croak "MARSHAL Error "."\n";
 }
 catch CORBA::COMM_FAILURE with{
     croak "Commfailure Error "."\n";
 }
 catch CORBA::SystemException with{
     croak "SystemException Error "."\n";
 };
 $ref->{arsref}=[$tm];
 

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
      ($length,$ahl)=$arsref->getACS(\%cid,\$maxc);
         if($length==0){
             $ref->{asl}=undef;
         }
         else {
             $ref->{asl}=$ahl;
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


         $cid{Type}="Producer";
       ($length,$ahl)=$arsref->getAHS(\%cid);
         if($length==0){
             $ref->{ahlp}=undef;
         }
         else {
             $ref->{ahlp}=$ahl;
         }
      ($length,$ahl)=$arsref->getACS(\%cid,\$maxc);
         if($length==0){
             $ref->{acl}=undef;
         }
         else {
             $ref->{acl}=$ahl;
         }
        ($length,$ahl)=$arsref->getNC(\%cid);
         if($length==0){
             $ref->{ncl}=undef;
         }
         else {
             $ref->{ncl}=$ahl;
         }
         last;
     }
     catch CORBA::SystemException with{
     };
 }

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
         last;
     }
     catch CORBA::SystemException with{
     };
 }


}

sub UpdateARS{
 my $ref=shift;
 my $arsref;
 foreach $arsref (@{$ref->{arsref}}){
     try{
         my %cid=%{$ref->{cid}};
         $cid{Type}="Server";
      my ($length,$pars)=$arsref->getARS(\%cid,"Any",0);
         if($length==0 ){
            croak "updars returns zero \n";
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
         ($length,$pars)=$arsref->getARS(\%cid,"Any",0);

         if($length==0 ){
            croak "updars returns zero \n";
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
                  unshift @{$ref->{arpref}}, $newref;
              }
              else{
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
         print "updars exited";
}

sub Update{
    my  $ref=$Monitor::Singleton;
    $ref->UpdateARS();
    $ref->UpdateEverything();
}

sub Update2{
    warn "Update2Running";
    my $ref=shift;
    if(not ref($ref)){
     $ref=$Monitor::Singleton;
    }
}

