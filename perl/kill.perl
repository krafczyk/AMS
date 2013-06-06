#!/usr/bin/perl -w
use HTTP::Date;
use Unix::PID;
use GTop();
use strict;
my $cpuload=10;
my $cpuloada=.5;
my $time=300;
my $delay=600;
my $go=1;
   my @killed;
   $#killed=-1;
while(1){
    my $gtop=GTop->new();
   my $pida = Unix::PID->new();
   my @pids=$pida->get_pidof(""); 
    my $i=-1;
    for my $kill (@killed){
        $i++;
        my $found=0;
     for my $pid (@pids){
        if($kill==$pid){
            $found=1;
            last;
        }      
     }
     if(not $found){
         splice(@killed,$i,1);
         $i--;
     }     
    }
for my $pid (@pids){
   if($pid==0){
      next;
   }
   if (not $pida->is_pid_running($pid)) {
       next;
   }
   my @pidinfo=$pida->pid_info($pid);
   if($#pidinfo>0){
       if($pidinfo[0]=~/root/ or $pidinfo[0]=~/ams/ or $pidinfo[0]=~/oracle/){
           #print "$pidinfo[0] \n";
           next;
       }
       my $proc_uid=$gtop->proc_uid($pid);
       my $ctime=time();
       if($pidinfo[2]>$cpuload){
           my @time=split ':',$pidinfo[9];
           my $cpt=$time[0]*60+$time[1];
           if($cpt>$time){ 
              my $proc_time=$gtop->proc_time($pid);
             my $stime=$proc_time->start_time;
              my $avload=$cpt/($ctime-$stime+1);
              if($avload>$cpuloada){
                  my $lsf=0;
                  my $ppid=-1;
                  while($ppid!=1 and $ppid!=0){
                   $ppid=$proc_uid->ppid;
                   my @ppidinfo=$pida->pid_info($ppid);
                   if($#ppidinfo>9 and ($ppidinfo[10]=~/pbs_mom/ or $ppidinfo[10]=~/sbatchd/) and $ppidinfo[0]=~/root/){
                       $lsf=1;
                       last;
                   }
                   $proc_uid=$gtop->proc_uid($ppid);
                   }      
                  if($lsf==0){
                   if($pidinfo[10]=~/root.exe/ and $cpt<1800 and $avload<0.9){
                      next;
}
 
                      my $cmd="kill $pid";
                      my $found=0;
                   for my $kill (@killed){
                       if($kill==$pid){
                        $cmd="kill -9 $pid";
                        $found=1;
##
## Hack by Pavel. Post wait for all dead children
##
$pida->non_blocking_wait();
##
                        last;
                       }
                   }
                   if(not $found){
                      unshift @killed,$pid;
                  }
                      my $strt=time2str(time());
                      if($go){
                       system($cmd);
                       my $add=$pidinfo[0].'@mail.cern.ch';
                       my $subj="Process Killed $strt $cmd @pidinfo";
                       my $message="All noninteractive jobs should be submitted only via lsf. \n Please use man bsub and refs wherein to control jobs submission."; 
                       sendmailmessage($add,$subj,$message);
                       } 
                   print " $strt killed $avload $cmd @pidinfo \n";
                  }
                 

              }
          }
       }
   }
 }
 sleep($#killed>=0?$delay/2:$delay);
}


sub sendmailmessage{
    my $add=shift;
    my $sub=shift;
    my $mes=shift;
    open MAIL, "| /usr/lib/sendmail -t -i"
        or die "could not open sendmail: $!";
     print MAIL <<END_OF_MESSAGE2;
To:  $add
Subject: $sub

$mes
END_OF_MESSAGE2
    close MAIL or die "Error closing sendmail: $!";
}

