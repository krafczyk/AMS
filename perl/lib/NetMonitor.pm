# $Id: NetMonitor.pm,v 1.1 2006/05/15 14:29:05 choutko Exp $
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
push @{$self->{sendmail}},{first=>1,repet=>3600*6,address=>'Alexandre.Eline@cern.ch 41764874733@mail2sms.cern.ch',sent=>0,timesent=>0};
push @{$self->{sendmail}},{first=>0,repet=>3600*6,address=>'vitali.choutko@cern.ch  41764870923@mail2sms.cern.ch',sent=>0,timesent=>0};


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
    $self->sendmailpolicy("NetMonitor-S-UnableToOpen File $self->{hostfile} \n");
    sleep $self->{sleep};
    goto again;
}
       my $buf;
    if(not read(FILE,$buf,1638400)){
      close FILE;
      $self->sendmailpolicy("NetMonitor-S-ProblemsToReadFile $self->{hostfile} \n");
      sleep $self->{sleep};
      goto again;
    }     
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
         push @{$self->{hosts}},"pcamsec.cern.ch";

    while(1){
    $#{$self->{bad}}=-1; 
    foreach my $host (@{$self->{hosts}}) {
        if($self->{ping}->ping($host)){
        }
        else{
            push @{$self->{bad}}, $host;
        }
    }
#    sleep(6);
#    while (my ($host,$rtt,$ip) = $self->{ping}->ack) {
#      print "HOST: $host [$ip] ACKed in $rtt seconds.\n";
#    }
#    sleep(6);
#@{$self->{bad}}=keys %{$self->{ping}->{bad}};
    if($#{$self->{bad}}>-1){
        foreach my $bad (@{$self->{bad}}){
            my $found=0;
          foreach my $badsave (@{$self->{badsave}}){
              if($badsave eq $bad){
                  $found=1;
                  last;
              }
          }
            if( $found ==0){
              $self->sendmailpolicy("NetMonitor-W-SomeHostsAreDown");
              $#{$self->{badsave}}=-1;
               foreach my $bada (@{$self->{bad}}){
                 push @{$self->{badsave}}, $bada;
               }                
              last;
          }
        }
    }
    else{
#reset mailcounts
      $self->sendmailpolicy("NetMonitor-I-AllHostsAreOK");
    }
     
    sleep($self->{sleep});

}
}

sub sendmailpolicy{
   my $self=shift; 
   my $subj=shift;
   if($subj=~/-I-/){
       for my $i (0..$#{$self->{sendmail}}){
           my $hash=${$self->{sendmail}}[$i];
           if($hash->{sent}>0){
               my @sadd=split ' ',$hash->{address};
               foreach my $add (@sadd){
                $self->sendmailmessage($add,$subj," ");
               }
           } 
           $hash->{sent}=0;
           $hash->{timesent}=0;
       }
   } 
   else{
#  error
       my $firstsent=0;
       for my $i (0..$#{$self->{sendmail}}){
           my $hash=${$self->{sendmail}}[$i];
           if($hash->{first} ==1 and $hash->{sent}>0){
            $firstsent=1;
            last;
           }
       }
       for my $i (0..$#{$self->{sendmail}}){
           my $hash=${$self->{sendmail}}[$i];
           my $curtim=time();           
           if(($hash->{sent}==0 or $curtim-$hash->{timesent}>$hash->{repet}) and ($hash->{first}==1 or $firstsent)){
               my @sadd=split ' ',$hash->{address};
               my $mes="";
               foreach my $bad (@{$self->{bad}}){
                   $mes=$mes."$bad \n";
               }
               foreach my $add (@sadd){
                $self->sendmailmessage($add,$subj,$mes);
               }
              $hash->{sent}++;
              $hash->{timesent}=$curtim;
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
    close MAIL or return 2;
    }
    return 0;
}

