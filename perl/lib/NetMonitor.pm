# $Id: NetMonitor.pm,v 1.6 2006/05/19 07:49:30 choutko Exp $
# May 2006  V. Choutko 
package NetMonitor;
use Net::Ping;
use strict;
use Carp;
use MIME::Lite;
use POSIX  qw(strtod);
@NetMonitor::EXPORT=qw(new Run); 

sub new{
    my $type=shift;

my %fields=(
  sendmail=>[],
  hosts=>[],
  hostsstat=>[],
  bad=>[],
  badsave=>[],
  sleep=>30,
  hostfile=>"/afs/ams.cern.ch/Offline/AMSDataDir/DataManagement/prod/NominalHost",
  ping=>undef,
            );
my $self={
  %fields,
};
push @{$self->{sendmail}},{first=>1,repet=>21600,address=>'Alexandre.Eline@cern.ch 41764874733@mail2sms.cern.ch',sent=>0,timesent=>0};
push @{$self->{sendmail}},{first=>0,repet=>21600,address=>'vitali.choutko@cern.ch  41764870923@mail2sms.cern.ch',sent=>0,timesent=>0};


    my $mybless=bless $self,$type;
    if(ref($NetMonitor::Singleton)){
        croak "Only Single NetMonitor Allowed\n";
    }
    $NetMonitor::Singleton=$mybless;
    return $mybless;
}


sub Run{
    my $self=shift;
     $self->{ping} = Net::Ping->new();
#    $self->{ping} = Net::Ping->new("syn");
#    $self->{ping}->{port_num} = getservbyname("http", "tcp");

again:
if(not open(FILE,"<".$self->{hostfile})){
    $self->sendmailpolicy("NetMonitor-S-UnableToOpen File $self->{hostfile} \n",0);
    sleep $self->{sleep};
    goto again;
}
       my $buf;
    $self->sendmailpolicy("NetMonitor-I-Opened File $self->{hostfile} \n",0);
    if(not read(FILE,$buf,1638400)){
      close FILE;
      $self->sendmailpolicy("NetMonitor-S-ProblemsToReadFile $self->{hostfile} \n",0);
      sleep $self->{sleep};
      goto again;
    }     
      $self->sendmailpolicy("NetMonitor-I-ReadFile $self->{hostfile} \n",0);
    close FILE;
    my @sbuf=split "\n",$buf;
    $#{$self->{hosts}}=-1;
    foreach my $line (@sbuf){
     if($line=~/^\#/){
        next;
     }
     my @sline=split ' ',$line;
     if($#sline>2){
         my $host=$sline[0].".".$sline[1];
         push @{$self->{hosts}},$host;
            }
  }
#         push @{$self->{hosts}},"pcamsf6.cern.ch";
#         push @{$self->{hosts}},"pcamsec.cern.ch";

    while(1){
    $#{$self->{bad}}=-1; 
    my $mes="NetMonitor-W-SomeHostsAreDown";
    foreach my $host (@{$self->{hosts}}) {
        if($self->{ping}->ping($host)){
        }
        else{
            push @{$self->{bad}}, $host." ".$mes;
        }
    }
#    sleep(6);
#    while (my ($host,$rtt,$ip) = $self->{ping}->ack) {
#      print "HOST: $host [$ip] ACKed in $rtt seconds.\n";
#    }
#    sleep(6);
#@{$self->{bad}}=keys %{$self->{ping}->{bad}};

#
# Now timing
#

    $mes="NetMonitor-W-SomeHostsHaveWrongTime";
    my $command="ssh -x -o \'StrictHostKeyChecking no \' ";
    my ($sec,$min,$hr,$mday,$mon,$y,$w,$yd,$isdst)=localtime(time());
    foreach my $host (@{$self->{hosts}}) {
        my $gonext=0;
        foreach my $bad (@{$self->{bad}}){
            my @sbad=split ' ',$bad;
            if($sbad[0] eq $host){
                $gonext=1;
                last;
            }
        }
        if($gonext){
            next;
        }
        unlink "/tmp/xtime";
        my $i=system($command.$host." date > /tmp/xtime ");
        if(not $i){
          if(not open(FILE,"<"."/tmp/xtime")){
                 push @{$self->{bad}}, $host." NetMonitor-W-ssh1Failed";
            next;
          }           
             my $buf;
             if(not read(FILE,$buf,16384)){
                 push @{$self->{bad}}, $host." NetMonitor-W-ssh2Failed";
                 close FILE;
               next;
             }
             close FILE;
             my @sbuf= split ' ',$buf;
             if($#sbuf>3){
               my $xtl=($mday-1)*24*3600+$hr*3600+$min*60+$sec;
               my @ssbuf=split ':',$sbuf[3];
               my $xt=($sbuf[2]-1)*3600*24+$ssbuf[0]*3600+$ssbuf[1]*60+$ssbuf[2];
               if(abs($xt-$xtl)>360){
                  push @{$self->{bad}}, $host." NetMonitor-W-ClockProblems";
               }
               next;
             }  
                push @{$self->{bad}}, $host." NetMonitor-W-ClockReadProblems";
         }
        else{
            push @{$self->{bad}}, $host." NetMonitor-W-sshFailed";
            next;
        }
    }




    if($#{$self->{bad}}>-1){
        my $found=0;
        foreach my $bad (@{$self->{bad}}){
          foreach my $badsave (@{$self->{badsave}}){
              if($badsave eq $bad){
                  $found++;
                  last;
              }
          }
        }
              $self->sendmailpolicy("NetMonitor-W-SomeHostsAreDown",$#{$self->{bad}}+1-$found);
              $#{$self->{badsave}}=-1;
               foreach my $bada (@{$self->{bad}}){
                 push @{$self->{badsave}}, $bada;
               }                
    }
    else{
#reset mailcounts
      $self->sendmailpolicy("NetMonitor-I-AllHostsAreOK",0);
    }
     
    sleep($self->{sleep});

}
}

sub sendmailpolicy{
   my $self=shift; 
   my $subj=shift;
   my $force=shift;
   if(not defined $force){
       $force=0;
   }
   if($subj=~/-I-/){
       for my $i (0..$#{$self->{sendmail}}){
           my $hash=${$self->{sendmail}}[$i];
           my $ok=0;
           my @sadd=split ' ',$hash->{address};
           if($hash->{sent}>0){
               foreach my $add (@sadd){
                $ok+=$self->sendmailmessage($add,$subj," ");
               }
           } 
           if($ok<$#sadd+1){
            $hash->{sent}=0;
            $hash->{timesent}=0;
           }
       }
   } 
   else{
#  error
       my $firstsent=0;
       for my $i (0..$#{$self->{sendmail}}){
           my $hash=${$self->{sendmail}}[$i];
           if($hash->{first} ==1 and $hash->{sent}>1){
            $firstsent=1;
            last;
           }
       }
       for my $i (0..$#{$self->{sendmail}}){
           my $hash=${$self->{sendmail}}[$i];
           my $curtim=time();           
           if(($hash->{sent}==0 or $curtim-$hash->{timesent}>$hash->{repet} or $force!=0)
                and ($hash->{first}==1 or $firstsent)){
               my @sadd=split ' ',$hash->{address};
               my $mes="";
               my $ok=0;
               if($#{$self->{bad}} eq -1){
                foreach my $add(@sadd){
                  $ok+=$self->sendmailmessage($add,$subj," ");
                }
               }
#
#  group bad hosts by subject
#
               my %badh=();
               foreach my $bad (@{$self->{bad}}){
                   my @smes = split ' ',$bad;
                   $subj=$smes[1];
                   $mes="$smes[0] \n";
                   $badh{$subj}="";
               }             
               foreach my $bad (@{$self->{bad}}){
                   my @smes = split ' ',$bad;
                   $subj=$smes[1];
                   $mes="$smes[0] \n";
                   $badh{$subj}=$badh{$subj}.$mes;
               }             

               foreach my $subj (keys %badh){
               foreach my $add (@sadd){
                $ok+=$self->sendmailmessage($add,$subj,$badh{$subj});
               }
               }
               my $min=$#sadd+1;
               if($min<($#sadd+1)*scalar(keys(%badh))){
                  $min=($#sadd+1)*scalar(keys(%badh));
               }
              if($ok<$min){
               $hash->{sent}++;
               $hash->{timesent}=$curtim;
              }
           } 
       }
   }
}

sub sendmailmessage{
    my $self=shift;
    my $add=shift;
    my $sub=shift;
    my $mes=shift;
    my $att=shift;
    if(defined $att){
        my $msg = MIME::Lite->new(
                     To      =>$add,
                     Subject =>$sub,
                     Type    =>'multipart/mixed'
                               );

        $msg->attach(Type     =>'TEXT',
                     Data     =>$mes,
                     );
        my @files = split ';', $att;
        foreach my $file (@files){
            my @f=split ',', $file;
         $msg->attach(Type     =>'octet/stream',
                     Path     =>$f[0],
                     Filename =>$f[1],
                     Disposition => 'attachment'
                     );
        }
      #  $msg->as_string;

      #  $msg->print(\*SENDMAIL);
        $msg->send();
    }
    else{
    open MAIL, "| /usr/lib/sendmail -t -i"
#        or die "could not open sendmail: $!";
        or return 1;
     print MAIL <<END_OF_MESSAGE2;
To:  $add
Subject: $sub

$mes
END_OF_MESSAGE2
    close MAIL or return 1;
    }
    return 0;
}

