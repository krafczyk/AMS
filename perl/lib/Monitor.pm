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
     db=>undef,
     ok=>0,
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
     $ref->{ok}=0;
 my $ior=getior();
 if(not defined $ior){ 
  $ior= shift @ARGV;
  unshift @ARGV, $ior;
 }
 if( not defined $ior){
  return $ref->{ok};
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
 return $ref->{ok};

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

         my ($ok, $dhl)=$arsref->getDBSpace(\%cid);
         $ref->{db}=$dhl;
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
      my ($length,$pars)=$arsref->getARS(\%cid,"Any",0);
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
         ($length,$pars)=$arsref->getARS(\%cid,"Any",0);

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
 return 1;
}

sub Update{
    my  $ref=$Monitor::Singleton;
    my $ok=$ref->UpdateARS();
    if (not $ok){
        $ref->Connect() or return ($ref, 0);
        $ref->UpdateARS() or return ($ref, 0);
    }
    $ok=$ref->UpdateEverything();
    return ($ref,$ok);
}

sub Update2{
    warn "Update2Running";
    my $ref=shift;
    if(not ref($ref)){
     $ref=$Monitor::Singleton;
    }
}


sub getior{
    my $file ="/tmp/o";
    my $fileo ="/tmp/oo";
    my $i=system "bjobs -q linux_server >$file" ;
    if($i){
        return undef;
    }
    open(FILE,"<".$file) or return undef;
    while (<FILE>){
        if ($_ =~/^\d/){
            my @args=split ' ';
            $i=system "bpeek $args[0] >$fileo";
            if($i){
                next;
            }
            open(FILEO,"<".$fileo) or next;
            while (<FILEO>){
                if (/^IOR:/){
                    close(FILEO);
                    return $_;
                }
            }
            close(FILEO);
        }
    }

    close(FILE);
}

sub getdbok{
    my @output=();
    my @text=();
    push @text, $Monitor::Singleton->{db}->{fs};
    push @text, int $Monitor::Singleton->{db}->{dbtotal};
    push @text, int $Monitor::Singleton->{db}->{dbfree};
    push @text, int $Monitor::Singleton->{db}->{dbfree}/$Monitor::Singleton->{db}->{dbtotal}*100;
    if($text[1]<0 or $text[2]<0 or $text[2]<200){
      push @text ,1;
  }elsif($text[2]<100){
      push @text ,2;
  }else{
      push @text ,0;
  }
    push @output, [@text];
    my $i;
    for $i (0 ... $#{$Monitor::Singleton->{dsti}}){
     $#text=-1;
     my $hash=$Monitor::Singleton->{dsti}[$i];
     my $string=$hash->{HostName}.":".$hash->{OutputDirPath};
    push @text, $string;
    push @text, int $hash->{TotalSpace}/1024;           
    push @text, int $hash->{FreeSpace}/1024;           
    push @text, int $hash->{FreeSpace}/$hash->{TotalSpace}*100;           
    if($text[1]<0 or $text[2]<0 or $text[2]<2000){
      push @text ,1;
  }elsif($text[2]<1000){
      push @text ,2;
  }else{
      push @text ,0;
  }
    push @output, [@text];
 }

    return @output;
}

sub getactivehosts{
    my @output=();
    my @text=();
    my @final_text=();
     my $total_cpu=0;
     my $total_ev=0;
    my $i;
    for $i (0 ... $#{$Monitor::Singleton->{ahlp}}){
     $#text=-1;
     my $hash=$Monitor::Singleton->{ahlp}[$i];
     my $string=$hash->{HostName};
    push @text, $string;
    push @text, int $hash->{ClientsRunning};           
    push @text, int $hash->{ClientsAllowed};           
    push @text, int $hash->{ClientsFailed};           
     my $ntp=0;
     my $evtag=0;
     my @dummy=split '\.', $string;
     my $host=$dummy[0];
     for my $j (0 ... $#{$Monitor::Singleton->{dsts}}){
     my $hdst=$Monitor::Singleton->{dsts}[$j];
      @dummy=split '\:', $hdst->{Name};
     my $chop=$dummy[0];
      if ( $chop eq $host){
          if( $hdst->{Type} eq "Ntuple"){
              $ntp++;
          }elsif( $hdst->{Type} eq "EventTag"){
              $evtag++;
          }
      }       
    }   
     push @text, $ntp;
     push @text, $evtag;
     my $evt=0;
     my $lastevt=0;
     my $err=0;
     my $cpu=0;
     my $total=0;
     for my $j (0 ... $#{$Monitor::Singleton->{rtb}}){
       my $rdst=$Monitor::Singleton->{rtb}[$j];
       $total+=$rdst->{LastEvent}+1-$rdst->{FirstEvent};
       my $rdstc=$rdst->{cinfo};
       if( $rdst->{Status} eq "Finished" or   $rdst->{Status} eq "Processing"){
           if( $rdstc->{HostName} eq $host){
               $evt+=$rdstc->{EventsProcessed};
               $lastevt+=$rdstc->{LastEventProcessed}+1-$rdst->{FirstEvent};
               $err+=$rdstc->{ErrorsFound};
               $cpu+=$rdstc->{CPUTimeSpent};
           }
       }
   }
   push @text, $evt; 
   push @text, int(1000*$lastevt/$total)/10.; 
   push @text, $err; 
     $total_cpu+=$cpu;
     $total_ev+=$evt;
   my $cpuper=int ($cpu*1000/($evt+1));
   push @text, $cpuper/1000.;
     push @text, $hash->{Status};
    if( $hash->{Status} eq "LastClientFailed"){
      push @text ,1;
  }elsif( $hash->{Status} eq "NoResponse"){
      push @text ,2;
  }else{
      push @text ,0;
  }
     if($i==0){
         @final_text=@text;
         $final_text[0]="Total";
         $final_text[10]="";
         $final_text[11]=0;
     }
     else{
        for my $i (1 ...8){ 
         $final_text[$i]+=$text[$i];
        }
     }
    push @output, [@text];
 }
    my $total_pr=$final_text[1]==0?1:$final_text[1];
   my $cpuper=int ($total_cpu*1000/($total_ev+1)/$total_pr);
   $final_text[9]= $cpuper/1000.;
    
    push @output, [@final_text];


    return @output;
}
sub getactiveclients{
    shift;
    my $producer=shift;
    my @output=();
    my @text=();
    my $hash;
    my $xmax;
    if($producer eq "Producer"){
        $xmax=$#{$Monitor::Singleton->{acl}};
 }else{
        $xmax=$#{$Monitor::Singleton->{asl}};
 }
    for my $i (0 ... $xmax){
        $#text=-1;
    if($producer eq "Producer"){
        $hash=$Monitor::Singleton->{acl}[$i];
 }else{
        $hash=$Monitor::Singleton->{asl}[$i];
 }
        push @text, $hash->{id}->{uid};
        push @text, $hash->{id}->{HostName};
        push @text, $hash->{id}->{pid};
        my $time=localtime($hash->{Start});
        push @text, $time;
        my $dt=time()-$hash->{LastUpdate};
        my $dts=$dt." sec ago";
#        $time=localtime($hash->{LastUpdate});
        push @text, $dts;
if ($producer eq "Producer"){
        my $run=-1;
     for my $j (0 ... $#{$Monitor::Singleton->{rtb}}){
       my $rdst=$Monitor::Singleton->{rtb}[$j];
       my $rdstc=$rdst->{cinfo};
       if( $rdst->{Status} eq "Processing"){
           if ($rdst->{cuid}==$hash->{id}->{uid}){
               $run=$rdst->{Run};
               last;
           }
       }
   }
        my $ntuple=" // ";
        my $lastev=0;
     for my $j (0 ... $#{$Monitor::Singleton->{dsts}}){
       my $rdst=$Monitor::Singleton->{dsts}[$j];
       if( $rdst->{Type} eq "Ntuple"){
           if ($rdst->{Run}==$run){
               if($rdst->{FirstEvent}>$lastev){
                   $lastev=$rdst->{FirstEvent};
                   $ntuple=$rdst->{Name};
               }
           }
       }
   }
        push @text, $run;
        my @dummy=split "//",$ntuple;
        push @text, $dummy[1];
    }

        push @text, $hash->{Status};
        if ($hash->{Status} eq "Registered" or $hash->{Status} eq "Active"){
         push @text, 0;
     }elsif( $hash->{Status} eq "TimeOut" or $hash->{Status} eq "Submitted"){
         push @text, 1;
     }else{
         push @text, 2;
     }
    push @output, [@text];
}
    return @output;

}


sub getruns{
    my @output=();
    my @text=();
    my @final_text=();
     my $total_cpu=0;
     my $total_ev=0;
    my @sort=( "Failed","Processing", "Finished","ToBeRerun");
    for my $j (0 ... $#sort){
    for my $i (0 ... $#{$Monitor::Singleton->{rtb}}){
     $#text=-1;
     my $hash=$Monitor::Singleton->{rtb}[$i];
     if ($hash->{Status} eq $sort[$j]){
     my $ctime=localtime($hash->{TFEvent});
     push @text, $hash->{Run},$ctime,$hash->{FirstEvent},$hash->{LastEvent},$hash->{Priority},$hash->{History},$hash->{Status};
     if ($hash->{Status} eq "Failed" and $hash->{History} eq "Failed"){
         push @text, 2;
     }elsif($hash->{History} eq "Failed"){
         push @text, 1;
     }else{
         push @text, 0;
     }
     push @output, [@text];
 }
 }
}
    return @output;

}

sub getntuples{
    my @output=();
    my @text=();
    my @final_text=();
     my $total_cpu=0;
     my $total_ev=0;
    my @sort=("Failure","InProgress","Success");
    for my $j (0 ... $#sort){
    for my $i (0 ... $#{$Monitor::Singleton->{dsts}}){
     $#text=-1;
     my $hash=$Monitor::Singleton->{dsts}[$i];
     if($hash->{Type} eq "Ntuple"){
         if($hash->{Status} eq $sort[$j]){
     my $ctime=localtime($hash->{Insert});
     push @text, $hash->{Run},$ctime,$hash->{FirstEvent},$hash->{LastEvent},$hash->{Name},$hash->{Status};
     push @output, [@text];
 }
 }
 }
}
    return @output;

}
