# $Id: Monitor.pm,v 1.52 2002/04/10 10:36:17 choutko Exp $

package Monitor;
use CORBA::ORBit idl => [ '../include/server.idl'];
use Error qw(:try);
use Carp;
use strict;
use lib::CID;
use lib::ActiveClient;
use lib::POAMonitor;
use lib::DBServer;
use lib::DBSQLServer;
@Monitor::EXPORT= qw(new Update Connect);

my %fields=(
     cid=>undef,
     start=>undef,
     ac=>undef,
     orb=>undef,
     root_poa=>undef,
     mypoamonitor=>undef,
     myref=>undef,
     myior=>undef,
     arsref=>[],
     arpref=>[],
     ardref=>[],
     nhl=>undef,
     ahls=>undef,
     ahlp=>undef,
     acl=>undef,
     aml=>undef,
     asl=>undef,
     adbsl=>undef,
     acl_maxc=>undef,
     aml_maxc=>-1,
     asl_maxc=>undef,
     adbsl_maxc=>undef,
     nsl=>undef,
     ncl=>undef,
     nkl=>undef,
     rtb=>undef,
     rtb_maxr=>undef,
     dsti=>undef,
     dsts=>undef,
     dbfile=>undef,
     db=>undef,
     env=>undef,
     rn=>undef,
     ok=>0,
     registered=>0,
     updatesviadb=>0,
     sqlserver=>undef,
     DataMC=>1,
        IOR=>undef,
            );

sub new{
    my $type=shift;
    my $self={
        %fields,
    };
$self->{start}=time();
$self->{cid}=new CID;    
$self->{orb} = CORBA::ORB_init("orbit-local-orb");
$self->{root_poa} = $self->{orb}->resolve_initial_references("RootPOA");
$self->{root_poa}->_get_the_POAManager->activate;
$self->{mypoamonitor}=new POAMonitor;
my $id = $self->{root_poa}->activate_object($self->{mypoamonitor});
$self->{myref}=$self->{root_poa}->id_to_reference ($id);
$self->{myior} = $self->{orb}->object_to_string ($self->{myref});
$self->{ac}= new ActiveClient($self->{myior},$self->{start},$self->{cid});

foreach my $chop  (@ARGV){
    if($chop =~/^-m/){
        $self->{DataMC}=0;
    }
    if($chop =~/^-d/){
        $self->{updatesviadb}=1;
#amsdatadir

        my $dir=$ENV{AMSDataDir};
        if (defined $dir){
            $self->{AMSDataDir}=$dir;
        }
        else{
            $self->{AMSDataDir}="/f0dat1/AMSDataDir";
            $ENV{AMSDataDir}=$self->{AMSDataDir};
        }
        $self->{sqlserver}=new DBSQLServer();
        $self->{sqlserver}->Connect();
#try to get ior
    my $sql="select dbfilename,lastupdate,IORS from Servers where status='Active'";
        my $ret=$self->{sqlserver}->Query($sql);
        my $updlast=0;
        foreach my $upd (@{$ret}){
            if($upd->[1]> $updlast){
                $updlast=$upd->[1];
                $self->{dbfile}=$upd->[0];
                $self->{IOR}=$upd->[2];
            }
        }
    }
}
    my $mc=shift;
    if(defined $mc){
    if($mc =~/^-m/){
        $self->{DataMC}=0;
    }
}




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
 if(defined $ref->{IOR}){
     return 1;
 }
 my $ior=>undef;
 if(not defined $ior){ 
  foreach my $chop  (@ARGV){
    if($chop =~/^-I/){
        $ior=unpack("x1 A*",$chop);
    }
    if($chop =~/^-ior/){
            my $iorfile = unpack("x4 A*",$chop);
            my $buf;
            open(FILE,"<".$iorfile) or die "Unable to open file [$iorfile] \n";
            read(FILE,$buf,1638400);
            close FILE;
            $ior=$buf;
#            print "Monitor::Connect -I- IOR read from $iorfile \n";
    }
 }
}
 if(not defined $ior) { 
  $ior=getior();
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

sub ResetFailedRuns{
 my $ref=shift;
      for my $j (0 ... $#{$ref->{rtb}}){
        my %rdst=%{${$ref->{rtb}}[$j]};
       if($rdst{Status} eq "Failed"){
         $rdst{Status}="ToBeRerun";
        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Update");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Update");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }


     }
}
}

sub ForceCheck{
 my $ref=shift;
 my $arsref;
 foreach $arsref (@{$ref->{arsref}}){
     my $length;
     my $ahl;
     try{
         $arsref->SystemCheck();
     }
     catch CORBA::SystemException with{
         print "SystemCheckFailed.\n";
     };
 }
}

sub UpdateServers{
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
             $ref->{asl_maxc}=$maxc;
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
 foreach $arsref (@{$ref->{ardref}}){
     try{
         my %cid=%{$ref->{cid}};
         $ref->{dbfile}=$arsref->getDBFilePath(\%cid);
         return 1;
     }
     catch CORBA::SystemException with{
     };
 }
 return 1;
}
sub DESTROY{
    my $self=shift;
    warn "DESTROYING $self";
}

sub UpdateARS{ my $ref=shift; my $arsref; if(not $ref->{ok}){ return
 0; } foreach $arsref (@{$ref->{arsref}}){ try{ my
 %cid=%{$ref->{cid}}; $cid{Type}="Server"; my
 ($length,$pars)=$arsref->getARS(\%cid,"Any",0,1); if($length==0 ){
 carp "updars returns zero \n"; return 0; } for(;;){ my $old=shift
 @{$ref->{arsref}}; if(ref($old)){ undef $old; } else{ last; } } my
 $ior; foreach $ior (@$pars){ try{ my
 $newref=$ref->{orb}->string_to_object($ior->{IOR}); if(rand >0.5){
 unshift @{$ref->{arsref}}, $newref; } else{ push @{$ref->{arsref}},
 $newref; } } catch CORBA::SystemException with{ carp "getars 2 oops
 SystemException Error "."\n"; };

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

         $cid{Type}="DBServer";
         ($length,$pars)=$arsref->getARS(\%cid,"Any",0,1);

         if($length==0 ){
#            carp "updars returns zero \n";
            return 1;
        }
        for(;;){
             my $old=shift @{$ref->{ardref}};
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
                  unshift @{$ref->{ardref}}, $newref;
              }
              else{
#                  warn "push"; 
                  push @{$ref->{ardref}}, $newref;
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
    if($ref->{updatesviadb}){
      $ok=DBServer::InitDBFile(undef,$ref);
      
    }
    else{
     if (not $ok){
        $ref->Connect() or return ($ref, 0);
        $ref->UpdateARS() or return ($ref, 0);
     }
     $ok=$ref->UpdateEverything();
    }
    return ($ref,$ok);
}

sub UpdateForce{
    my  $ref=$Monitor::Singleton;
    my $ok=$ref->UpdateARS();
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
    my $pid=$Monitor::Singleton->{cid}->{pid};
    my $file ="/tmp/o."."$pid";
    my $fileo ="/tmp/oo."."$pid";
    my $i=system "/usr/local/lsf/bin/bjobs -q linux_server -u all>$file" ;
    if($i){
        unlink $file;
        return getior2();
    }
    open(FILE,"<".$file) or return undef;
    my $ii=0;
    while (<FILE>){
        if (  $_ =~/^\d/){
            if($ii>0 || 1){
            my @args=split ' ';
            $i=system "/usr/local/lsf/bin/bpeek $args[0] >$fileo";
            if($i){
                next;
            }
            open(FILEO,"<".$fileo) or next;
            my $datamc=0;
            while (<FILEO>){
                if (/^DumpIOR/){
                    my $tag=substr($_,8,1);
                    if($tag eq $Monitor::Singleton->{DataMC}){
                        $datamc=1;
                    }
                }  
                if ($datamc and /^IOR:/){
                    close (FILEO);
                    unlink $file,$fileo;
                    return $_;
                }
            }
        }
        $ii++;
        }
    }
    close (FILEO);
    unlink $file,$fileo;
    return getior2();      
}
sub getior2{
    my $file ="/tmp/DumpIOR".".$Monitor::Singleton->{DataMC}";
    open(FILE,"<".$file) or return undef;
            while (<FILE>){
                if (/^IOR:/){
                    close(FILE);
                    return $_;
                }
            }
            close(FILE);
    return undef;
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
     $#text=-1;
    push @text, $Monitor::Singleton->{rn}->{fs};
    push @text, int $Monitor::Singleton->{rn}->{dbtotal};
    push @text, int $Monitor::Singleton->{rn}->{dbfree};
    push @text, int $Monitor::Singleton->{rn}->{dbfree}/$Monitor::Singleton->{rn}->{dbtotal}*100;
    if($text[1]<0 or $text[2]<0 or $text[2]<2000){
      push @text ,1;
  }elsif($text[2]<1000){
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
     my $total_time=0;
     my $total_cpu=0;
     my $total_ev=0;
     my $totcpu=0;
     my $totproc=0;
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
     my $cerr=0;
     my $cpu=0;
     my $cpup=0;
     my $time=0;
     my $timep=0;
     my $total=0;
     my $tevt=0;
     for my $j (0 ... $#{$Monitor::Singleton->{rtb}}){
       my $rdst=$Monitor::Singleton->{rtb}[$j];
       $total+=$rdst->{LastEvent}+1-$rdst->{FirstEvent};
       my $rdstc=$rdst->{cinfo};
       if( $rdst->{Status} eq "Finished" or   $rdst->{Status} eq "Processing"){
           $lastevt+=$rdstc->{LastEventProcessed}+1-$rdst->{FirstEvent};
           $tevt+=$rdstc->{EventsProcessed};
           if( $rdstc->{HostName} eq $host){
               $evt+=$rdstc->{EventsProcessed};
               $cerr+=$rdstc->{CriticalErrorsFound};
               $err+=$rdstc->{ErrorsFound};
               $cpu+=$rdstc->{CPUTimeSpent};
               $time+=$rdstc->{TimeSpent};
               if($rdst->{Status} eq "Processing"){
                 $cpup+=$rdstc->{CPUTimeSpent};
                 $timep+=$rdstc->{TimeSpent};
               }
           }
       }
   }
#     print "$total $host $lastevt \n";
   push @text, $evt; 
   push @text, int(1000*$lastevt/($total+1)*$evt/($tevt+1))/10.; 
   push @text, $err,$cerr; 
     $total_cpu+=$cpu;
     $total_time+=$time;
     $total_ev+=$evt;
     my $proc=$hash->{ClientsRunning}==0?1:$hash->{ClientsRunning};
   my $cpuper=int ($cpu*1000/($evt+1)/$proc);
   
     my $cpunumber=1;
    for my $nhlcycle (0 ...$#{$Monitor::Singleton->{nhl}}){
      my $hashnhl=$Monitor::Singleton->{nhl}[$nhlcycle];
      if($hash->{HostName} eq $hashnhl->{HostName}){
          $cpunumber=$hashnhl->{CPUNumber};
          last;
      }
  }
    my $effic=$time==0?0:int ($cpu*100*$proc/$cpunumber/($time)); 
     $totproc+=$proc;
     $totcpu+=$cpunumber;
  push @text, $cpuper/1000., $effic/100.;
     push @text, $hash->{Status};
    if( $hash->{Status} eq "LastClientFailed" or $hash->{Status} eq "InActive"){
      push @text ,1;
  }elsif( $hash->{Status} eq "NoResponse"){
      push @text ,2;
  }else{
      push @text ,0;
  }
     if($i==0){
         @final_text=@text;
         $final_text[0]="Total";
         $final_text[11]="  ";
         $final_text[12]=0;
     }
     else{
        for my $i (1 ...9){ 
         $final_text[$i]+=$text[$i];
        }
     }
    push @output, [@text];
 }
    my $total_pr=$final_text[1]==0?1:$final_text[1];
   my $cpuper=int ($total_cpu*1000/($total_ev+1)/$total_pr);
   $final_text[10]= $cpuper/1000.;
   $final_text[11]= int($total_cpu/$totcpu*$totproc/($total_time+0.001)*100)/100.;
    
    push @output, [@final_text];


    return @output;
}
sub getactiveclients{
    my $producer=shift;
    if(ref($producer)){
       $producer=shift;
    }
    my @output=();
    my @text=();
    my $hash;
    my $xmax;
    if($producer eq "Producer"){
        $xmax=$#{$Monitor::Singleton->{acl}};
 }elsif($producer eq "Server"){
        $xmax=$#{$Monitor::Singleton->{asl}};
 }elsif($producer eq "DBServer"){
        $xmax=$#{$Monitor::Singleton->{adbsl}};
 }else{
        $xmax=$#{$Monitor::Singleton->{aml}};
 }
    for my $i (0 ... $xmax){
        $#text=-1;
    if($producer eq "Producer"){
        $hash=$Monitor::Singleton->{acl}[$i];
 }elsif($producer eq "Server"){
        $hash=$Monitor::Singleton->{asl}[$i];
 }elsif($producer eq "DBServer"){
        $hash=$Monitor::Singleton->{adbsl}[$i];
 }else{
        $hash=$Monitor::Singleton->{aml}[$i];
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
        my $timeout=$hash->{TimeOut};
        push @text, $timeout;
if ($producer eq "Producer"){
        my $run=-1;
     for my $j (0 ... $#{$Monitor::Singleton->{rtb}}){
       my $rdst=$Monitor::Singleton->{rtb}[$j];
       my $rdstc=$rdst->{cinfo};
       if( $rdst->{Status} eq "Processing"){
           if ($rdst->{cuid}==$hash->{id}->{uid}){
               $run=$rdst->{Run};
#               warn "run ... $run \n";
               last;
           }
       }
   }
        my $ntuple=" // ";
        my $lastev=0;
     for my $j (0 ... $#{$Monitor::Singleton->{dsts}}){
       my $rdst=$Monitor::Singleton->{dsts}[$j];
       if( $rdst->{Type} eq "Ntuple"){
#           warn "wasrun $run $rdst->{Run} \n";
           if ($rdst->{Run}==$run){
#               warn "runfound $rdst->{FirstEvent} $lastev $rdst->{Name} \n";
               if($rdst->{FirstEvent}>$lastev){
                   $lastev=$rdst->{FirstEvent};
                   $ntuple=$rdst->{Name};
               }
           }
       }
   }
        push @text, $run;
        my @dummy=split "/",$ntuple;
        push @text, $dummy[$#dummy];
        push @text, $hash->{Status};
        if ($hash->{Status} eq "Registered" or $hash->{Status} eq "Active"){
            if($lastev>0){
             push @text, 0;
            }
            else{
             push @text, 1;
            }   
     }elsif( $hash->{Status} eq "TimeOut" or $hash->{Status} eq "Submitted"){
         push @text, 1;
     }else{
         push @text, 2;
     }


    }
    else{
        push @text, $hash->{Status};
        if ($hash->{Status} eq "Registered" or $hash->{Status} eq "Active"){
         push @text, 0;
     }elsif( $hash->{Status} eq "TimeOut" or $hash->{Status} eq "Submitted"){
         push @text, 1;
     }else{
         push @text, 2;
     }
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
    my @sort=( "Failed","Processing", "Finished","Allocated", "Foreign","ToBeRerun");
    for my $j (0 ... $#sort){
    for my $i (0 ... $#{$Monitor::Singleton->{rtb}}){
     $#text=-1;
     my $hash=$Monitor::Singleton->{rtb}[$i];
     my $order;
     if($hash->{History} eq "Failed"){
         $order=$hash->{History};
     }
     else{
      $order=$hash->{Status} ;
     }
     if ($order eq $sort[$j]){
     my $ctime=localtime($hash->{SubmitTime});
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
    my @sort=("Failure","InProgress","Success","Validated");
    for my $j (0 ... $#sort){
    for my $i (0 ... $#{$Monitor::Singleton->{dsts}}){
     $#text=-1;
     my $hash=$Monitor::Singleton->{dsts}[$i];
     if($hash->{Type} eq "Ntuple"){
         if($hash->{Status} eq $sort[$j]){
     my $ctime=localtime($hash->{Insert});
     push @text, $hash->{Run},$ctime,$hash->{FirstEvent},$hash->{LastEvent},$hash->{Name},$hash->{size},$hash->{Status};
         my $dt=time()-$hash->{Insert};
     if ($hash->{Status} eq "InProgress"){
         if($dt>3600*12 && $dt<3600*24){
             push @text,2;
         }elsif($dt>3600*6 && $dt<3600*24){
             push @text,1;
         }
         else{
             push @text,0;
         }
     }elsif($hash->{Status} eq "Failure" && $dt<3600*24){
         push @text, 1;
     }else{
         push @text, 0;
     }

     push @output, [@text];
 }
 }
 }
}
    return @output;

}


sub getcontrolthings{
    my $name=shift;
    my @output=();
    my @text=();
    if( $name eq "ServerClient"){    
     for my $i (0 ... $#{$Monitor::Singleton->{nsl}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{nsl}[$i];
         
         push @text,  $hash->{uid},$hash->{Type},$hash->{MaxClients}, 
int($hash->{CPUNeeded}*10)/10,
        $hash->{MemoryNeeded}, $hash->{WholeScriptPath}, $hash->{LogPath}, 
         $hash->{SubmitCommand}, $hash->{HostName}, $hash->{LogInTheEnd},$hash;
         push @output, [@text];   
     }
    }elsif( $name eq "ProducerClient"){           
     for my $i (0 ... $#{$Monitor::Singleton->{ncl}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{ncl}[$i];
         
         push @text,  $hash->{uid},$hash->{Type},$hash->{MaxClients}, 
int($hash->{CPUNeeded}*10)/10,
        $hash->{MemoryNeeded}, $hash->{WholeScriptPath}, $hash->{LogPath}, 
         $hash->{SubmitCommand}, $hash->{HostName}, $hash->{LogInTheEnd},$hash;
         push @output, [@text];   
     }
    }elsif( $name eq "ProducerActiveClient"){           
     for my $i (0 ... $#{$Monitor::Singleton->{acl}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{acl}[$i];
         push @text, $hash->{id}->{uid};
         push @text, $hash->{id}->{HostName};
         push @text, $hash->{id}->{pid};
         push @text, $hash->{Status};
         push @text, $hash->{StatusType};
         push @text, $hash->{TimeOut};
         push @output, [@text];   
     }
    }elsif( $name eq "ServerHost"){       
     for my $i (0 ... $#{$Monitor::Singleton->{nhl}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{nhl}[$i];
         
         push @text,  $hash->{HostName},$hash->{Interface},$hash->{OS}, 
         $hash->{CPUNumber}, $hash->{Memory}, $hash->{Clock};
#        find status;
                
     for my $j (0 ... $#{$Monitor::Singleton->{ahls}}){
         my $ahl=$Monitor::Singleton->{ahls}[$j];
         if( $ahl->{HostName} eq $hash->{HostName}){
            push @text,$ahl->{ClientsAllowed}; 
            push @text,$ahl->{Status}; 
            goto FOUND1;
         }
     }
         push @text,0;
         push @text,"NotFound";
     FOUND1:    
         push @output, [@text];   
     }
    }elsif( $name eq "ProducerHost"){        
     for my $i (0 ... $#{$Monitor::Singleton->{nhl}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{nhl}[$i];
         
         push @text,  $hash->{HostName},$hash->{Interface},$hash->{OS}, 
         $hash->{CPUNumber}, $hash->{Memory}, $hash->{Clock};
#        find status;
                
     for my $j (0 ... $#{$Monitor::Singleton->{ahlp}}){
         my $ahl=$Monitor::Singleton->{ahlp}[$j];
         if( $ahl->{HostName} eq $hash->{HostName}){
            push @text,$ahl->{ClientsAllowed}; 
            push @text,$ahl->{ClientsFailed}; 
            push @text,$ahl->{Status}; 
            goto FOUND;
         }
     }
         push @text,0;
         push @text,0;
         push @text,"NotFound";
     FOUND:    
         push @output, [@text];   
     }
    }elsif( $name eq "Ntuple"){        
     for my $i (0 ... $#{$Monitor::Singleton->{dsti}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{dsti}[$i];
         
         push @text, $hash->{uid}, $hash->{HostName},$hash->{OutputDirPath},$hash->{Mode},$hash->{UpdateFreq},$hash->{type};
         push @output, [@text];   
     }
    }elsif( $name eq "PNtuple"){        
     my @houtput=();
     for my $i (0 ... $#{$Monitor::Singleton->{dsts}}){
         my $hash=$Monitor::Singleton->{dsts}[$i];
         if($hash->{Type} eq "Ntuple"){
          push @houtput, $hash;
         }
     }
sub PNtupleSort{
    if( $a->{Status} ne $b->{Status}){
        return $a->{Status} cmp $b->{Status};
    }
    else{
        return $a->{Run}<=>$b->{Run};
    }
}

         my @sortedoutput=sort PNtupleSort @houtput;
         for my $i (0 ... $#sortedoutput){
          $#text=-1;
          my $hash=$sortedoutput[$i];
          push @text, $hash->{Run}, $hash->{Insert},$hash->{FirstEvent},$hash->{LastEvent},$hash->{Name},$hash->{size},$hash->{Status},$hash->{Type};
          push @output, [@text];   
      }
    }elsif( $name eq "Run"){        
     for my $i (0 ... $#{$Monitor::Singleton->{rtb}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{rtb}[$i];
         
         push @text,  $hash->{uid},$hash->{Run},$hash->{FirstEvent}, 
        $hash->{LastEvent}, $hash->{Priority}, $hash->{FilePath}, 
         $hash->{Status}, $hash->{History};
         push @output, [@text];   
     }
    }elsif( $name eq "Killer"){        
     for my $i (0 ... $#{$Monitor::Singleton->{nkl}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{nkl}[$i];
         
         push @text,  $hash->{uid},$hash->{Type},$hash->{MaxClients}, 
int($hash->{CPUNeeded}*10)/10,
        $hash->{MemoryNeeded}, $hash->{WholeScriptPath}, $hash->{LogPath}, 
         $hash->{SubmitCommand}, $hash->{HostName}, $hash->{LogInTheEnd},$hash;
         push @output, [@text];   
     }
 }elsif( $name eq "setEnv"){
     for my $i (0 ... $#{$Monitor::Singleton->{env}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{env}[$i];
         my @text = split '\=',$hash;
         push @output, [@text];   
     }
 }
    return @output;

}

sub sendback{
    my ($name, $action, $row,@data) =@_;
                my $safeact="Update";
                if($action eq "Delete"){
                  $safeact=$action;
              }
    if($name eq "ServerClient"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{nsl}}[$row]};
        $nc{uid}=shift @data;
        $nc{Type}=shift @data;
        $nc{MaxClients}=shift @data;
        $nc{CPUNeeded}=shift @data;
        $nc{MemoryNeeded}=shift @data;
        $nc{WholeScriptPath}=shift @data;
        $nc{LogPath}=shift @data;
        $nc{SubmitCommand}=shift @data;
        $nc{HostName}=shift @data;
        $nc{LogInTheEnd}=shift @data;
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}=$nc{Type};
                $arsref->sendNC(\%cid,\%nc,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}=$nc{Type};
                $arsref->sendNC(\%cid,\%nc,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
   }elsif($name eq "ProducerClient"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{ncl}}[$row]};
        $nc{uid}=shift @data;
        $nc{Type}=shift @data;
        $nc{MaxClients}=shift @data;
        $nc{CPUNeeded}=shift @data;
        $nc{MemoryNeeded}=shift @data;
        $nc{WholeScriptPath}=shift @data;
        $nc{LogPath}=shift @data;
        $nc{SubmitCommand}=shift @data;
        $nc{HostName}=shift @data;
        $nc{LogInTheEnd}=shift @data;
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}=$nc{Type};
                $arsref->sendNC(\%cid,\%nc,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}=$nc{Type};
                $arsref->sendNC(\%cid,\%nc,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
   }elsif($name eq "ProducerActiveClient"){
        my $ref=$Monitor::Singleton;
        my %ac=%{${$ref->{acl}}[$row]};
        shift @data;
        shift @data;
        shift @data;
        shift @data;
        $ac{StatusType}=shift @data;
        $ac{TimeOut}=shift @data;
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my $hash_ac=\%ac;
                my %cid=%{$ac{id}};
                $arsref->sendAC(\%cid,\$hash_ac,$safeact);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ac{id}};
                my $hash=\%cid;
                my $hash_ac=\%ac;
            try{
                $arsref->sendACPerl($hash,$hash_ac,$safeact);
#                $arsref->sendAC($hash,\$hash_ac,$safeact);
            }  catch DPS::DBProblem   with{
                my $e=shift;
                warn "DBProblem: $e->{message}\n";
            }
            catch CORBA::SystemException with{
                 $arsref->sendAC($hash,\$hash_ac,$safeact);
#                 $arsref->sendACPerl($hash,$hash_ac,$safeact);
            };
                last;
            }
            catch DPS::DBProblem   with{
                my $e=shift;
                warn "DBProblem: $e->{message}\n";
            }
            catch CORBA::SystemException with{
                warn "Exiting corba exc";
            };
}

    }elsif($name eq "Killer"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{nkl}}[$row]};
        $nc{uid}=shift @data;
        $nc{Type}=shift @data;
        $nc{MaxClients}=shift @data;
        $nc{CPUNeeded}=shift @data;
        $nc{MemoryNeeded}=shift @data;
        $nc{WholeScriptPath}=shift @data;
        $nc{LogPath}=shift @data;
        $nc{SubmitCommand}=shift @data;
        $nc{HostName}=shift @data;
        $nc{LogInTheEnd}=shift @data;
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}=$nc{Type};
                $arsref->sendNK(\%cid,\%nc,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}=$nc{Type};
                $arsref->sendNK(\%cid,\%nc,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
    }elsif($name eq "Ntuple"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{dsti}}[$row]};
        $nc{uid}=shift @data;
        $nc{HostName}=shift @data;
        $nc{OutputDirPath}=shift @data;
        $nc{Mode}=shift @data;
        $nc{UpdateFreq}=shift @data;
        $nc{type}=shift @data;
        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendDSTInfo(\%nc,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendDSTInfo(\%nc,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
    }elsif($name eq "PNtuple"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{dsts}}[$row]};
        $nc{Run}=shift @data;
        $nc{Insert}=shift @data;
        $nc{FirstEvent}=shift @data;
        $nc{LastEvent}=shift @data;
        $nc{Name}=shift @data;
        $nc{size}=shift @data;
        $nc{Status}=shift @data;
        $nc{Type}=shift  @data;
        my $arsref;
                my %cid=%{$ref->{cid}};
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendDSTEnd(\%cid,\%nc,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendDSTEnd(\%cid,\%nc,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
    }elsif($name eq "Run"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{rtb}}[$row]};
        $nc{uid}=shift @data;
        $nc{Run}=shift @data;
        $nc{FirstEvent}=shift @data;
        $nc{LastEvent}=shift @data;
        $nc{Priority}=shift @data;
        $nc{FilePath}=shift @data;
        $nc{Status}=shift @data;
        $nc{History}=shift @data;
        if($nc{History} eq "ToBeRerun"){
         my $rdstc=$nc{cinfo};
         $rdstc->{HostName}="      ";
        }
        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendRunEvInfo(\%nc,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
}
        foreach $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendRunEvInfo(\%nc,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
    }elsif($name eq "ServerHost"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{nhl}}[$row]};
        $nc{HostName}=shift @data;
        $nc{Interface}=shift @data;
        $nc{OS}=shift @data;
        $nc{CPUNumber}=shift @data;
        $nc{Memory}=shift @data;
        $nc{Clock}=shift @data;
#find ahl
my %ac;
      for my $j (0 ... $#{$Monitor::Singleton->{ahls}}){
         my $ahl=$Monitor::Singleton->{ahls}[$j];
         if( $ahl->{HostName} eq $nc{HostName}){
          %ac=%{${$ref->{ahls}}[$j]};
          $ac{ClientsAllowed}=shift @data;
          $ac{Status}=shift @data;
          $ac{Clock}=$nc{Clock};
          $ac{Interface}=$nc{Interface};
          goto FOUND2;
         }
      }
           %ac=%{${$ref->{ahls}}[0]};
          $ac{ClientsAllowed}=shift @data;
          $ac{Status}=shift @data;
          $ac{Hostname}=$nc{HostName};
          $ac{ClientsRunning}=0;
          $ac{ClientsProcessed}=0;
          $ac{ClientsFailed}=0;
          $ac{ClientsKilled}=0;
          $ac{Clock}=$nc{Clock};
          $ac{Interface}=$nc{Interface};
FOUND2:
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}="Server";
                $arsref->sendNH(\%cid,\%nc,$action);
                $arsref->sendAH(\%cid,\%ac,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}="Server";
                $arsref->sendNH(\%cid,\%nc,$action);
                $arsref->sendAH(\%cid,\%ac,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
    }elsif($name eq "ProducerHost"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{nhl}}[$row]};
        $nc{HostName}=shift @data;
        $nc{Interface}=shift @data;
        $nc{OS}=shift @data;
        $nc{CPUNumber}=shift @data;
        $nc{Memory}=shift @data;
        $nc{Clock}=shift @data;
#find ahl
my %ac;
      for my $j (0 ... $#{$Monitor::Singleton->{ahlp}}){
         my $ahl=$Monitor::Singleton->{ahlp}[$j];
         if( $ahl->{HostName} eq $nc{HostName}){
          %ac=%{${$ref->{ahlp}}[$j]};
          $ac{ClientsAllowed}=shift @data;
          $ac{ClientsFailed}=shift @data;
          $ac{Status}=shift @data;
          $ac{Clock}=$nc{Clock};
          $ac{Interface}=$nc{Interface};
          goto FOUND3;
         }
        }
           %ac=%{${$ref->{ahlp}}[0]};
          $ac{ClientsAllowed}=shift @data;
          $ac{ClientsFailed}=shift @data;
          $ac{Status}=shift @data;
          $ac{Hostname}=$nc{HostName};
          $ac{ClientsRunning}=0;
          $ac{ClientsProcessed}=0;
          $ac{ClientsFailed}=0;
          $ac{ClientsKilled}=0;
          $ac{Clock}=$nc{Clock};
          $ac{Interface}=$nc{Interface};
FOUND3:
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}="Server";
                $arsref->sendNH(\%cid,\%nc,$action);
                $cid{Type}="Producer";
                $arsref->sendAH(\%cid,\%ac,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}="Server";
                $arsref->sendNH(\%cid,\%nc,$action);
                $cid{Type}="Producer";
                $arsref->sendAH(\%cid,\%ac,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
      }elsif($name eq "setEnv"){
        my $ref=$Monitor::Singleton;
                my $env=shift @data;
                my $path=shift @data;
                foreach my $arsref (@{$ref->{arsref}}){
                try{
                 my %cid=%{$ref->{cid}};
                 $cid{Type}="Server";
                 $arsref->setEnv(\%cid,$env,$path);
                }
                catch CORBA::SystemException with{
                 warn "sendback corba exc";
                };
            }
      }
}


sub Exiting{
    my $ref=shift;
    if(defined $ref->{IOR}){
        $ref->ErrorPlus(shift);        
    }
    else{
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                $arsref->Exiting(\%cid,"Exiting ","CInExit");
                return;
            }
            catch CORBA::SystemException with{
                warn "Exiting corba exc";
            };
        }
    } 
}


sub SendId{

    use Error qw(:try);
    my $ref=$Monitor::Singleton;
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                my $hash=\%cid;
                my ($ok,$ok2)=$arsref->sendId(\$hash,600);
                $ref->{cid}=$hash;
                $ref->{ac}->{id}=$hash;
                my %ac=%{$ref->{ac}};
                my $hash_ac=\%ac;
                $arsref->sendAC($hash,\$hash_ac,"Update");
                $ref->{ac}=$hash_ac;
            }
            catch CORBA::SystemException with{
                warn "Exiting corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ref->{cid}};
                my $hash=\%cid;
                my %ac=%{$ref->{ac}};
                my $hash_ac=\%ac;
            try{
                $arsref->sendAC($hash,\$hash_ac,"Create");
            }            
            catch DPS::DBProblem   with{
                my $e=shift;
                warn "DBProblem: $e->{message}\n";
            }
            catch CORBA::SystemException with{
                 $arsref->sendACPerl($hash,$hash_ac,"Create");
            };
                $ref->{ac}=$hash_ac;
                last;
            }
            catch CORBA::SystemException with{
                warn "Exiting corba exc";
            };
        }
}


sub ErrorPlus{
   my $ref=shift;
   die shift;
}
