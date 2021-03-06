# $Id$
# May 2006  V. Choutko 
package NetMonitor;
use Net::Ping;
use strict;
use Carp;
use DBI;
use POSIX  qw(strtod);
@NetMonitor::EXPORT=qw(new Run); 
$ENV{AMISERVER} = "http://pcamsj2.cern.ch:8081";
$ENV{ROOTSYS} = "/afs/cern.ch/exp/ams/Offline/root/Linux/527.icc64";
use lib::DirMonitor;

sub new{
    my $type=shift;

#
#  host targets
#
#  ssh ams@pcamss0
#  cd $AMSDataDir/DataManagement/prod
#  ./ServerRestart
#  ./ServerRestart_v5
#
my %fields=(
  sendmail=>[],
  hosts=>[],
  excluded=>['pcamsj0','scamsnd2','scamsnd3','amsvobox03','amsvobox04','amsvobox12','pcposc1','pcamsdt1','pcamsn0','amsvobox11','amsvobox01','amsvobox02','scamsnd5','amsvobox05','amsvobox06','amsvobox07','amsvobox10','amsvobox13','amsvobox14','pcamsf9','pcamsp1','pcamsf7','pcamsap','pcamsd1','pcamsvc','pcamsdt0','pcamst0','pcamsd3','lxplus'], 
  dbhosts=>['pcamss0','amsvobox02','scamsfs0'],
  sbhost=>'pcamss0',
  clusterhosts=>['pcamsr0','pcamsf2','pcamsf4'],
#  dbhoststargets=>['amsprodserver.exe','amsprodserverv5.exe','transfer.py','frame_decode','scdb_j2.perl'],
#  dbhoststargets=>['amsprodserverv5.exe','transfer.py','frame_decode','scdb_j2.perl'],
   dbhoststargets=>['amsprodserverv5.exe','transfer.py','frame_decode'],
  filesystems=>['f2users','r0fc00','fc03dat1'],
  afsvolumes=>['/afs/cern.ch/ams/local','/afs/cern.ch/ams/AMSDataBase','/afs/cern.ch/ams/Offline'],
  hostsstat=>[],
  bad=>[],
  badsave=>[],
  sleep=>120,
  hostfile=>"/afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/prod/NominalHost",
  ping=>undef,
  sqlserver=>undef,
  dir_monitor=>undef,
            );
my $self={
  %fields,
};

my $dmfeilds={
        warn_level=>30000,
        src_dirs=>['/afs/cern.ch/ams/local/logs','/afs/cern.ch/ams/local/bsubs','/afs/cern.ch/ams/Offline/AMSDataDirRW/prod.log/scripts','/afs/cern.ch/ams/local/prod.log/Producer','/afs/cern.ch/ams/local/prod.log/MCProducer'],
        dst_dir=>'/fc02dat0/scratch/MC/afsbackup',
        dst_dir2=>'/fc02dat0/scratch/MC/afsbackup',
#        dst_dir2=>'/f2users/scratch/MC/backupafs',
        max_nfiles=>25000,
        file_life=>604800,
        rm_mode=>1
        };
$self->{dir_monitor}=new DirMonitor($dmfeilds);

my %sfields=(
     start=>undef,
     dbhandler=>undef,
     dbdriver=>'Oracle:',
     dbfile=>'pdb_ams',
     dbinit=>0,
     lastupdate=>0,
     repet=>3600,
     keep=>3600*24,
             );
$self->{sqlserver}={%sfields,};
push @{$self->{sendmail}},{first=>1,repet=>21600,address=>'Alexandre.Eline@cern.ch 41764874733@mail2sms.cern.ch',sent=>0,timesent=>0};
#push @{$self->{sendmail}},{first=>0,repet=>21600,address=>'vitali.choutko@cern.ch  41764870923@mail2sms.cern.ch',sent=>0,timesent=>0};
#push @{$self->{sendmail}},{first=>1,repet=>21600,address=>'pavel.goglov@cern.ch  41764871287@mail2sms.cern.ch',sent=>0,timesent=>0};
push @{$self->{sendmail}},{first=>1,repet=>21600,address=>'aegorov2@cern.ch 41764876488@mail2sms.cern.ch',sent=>0,timesent=>0};
#push @{$self->{sendmail}},{first=>1,repet=>21600,address=>'dmitri.filippov@cern.ch 41764878747@mail2sms.cern.ch',sent=>0,timesent=>0};
push @{$self->{sendmail}},{first=>1,repet=>21600,address=>'baosong.shan@cern.ch 41764879642@mail2sms.cern.ch',sent=>0,timesent=>0};
   #  excluded hosts
    my $mybless=bless $self,$type;
    if(ref($NetMonitor::Singleton)){
        croak "Only Single NetMonitor Allowed\n";
    }
    $NetMonitor::Singleton=$mybless;
    return $mybless;
}


sub InitOracle{
    my $self=shift;

   set_oracle_env();
    my $pwd="";
    my $user="amsdes";
    
    my $oracle="/afs/cern.ch/user/a/ams/.oracle/.oracle.oracle";
#  my $oracle="/var/www/cgi-bin/mon/lib/.oracle.oracle";
  aga2:
    if(not open(FILE,"<".$oracle)){
      $self->sendmailpolicy("NetMonitor-S-UnableToOpenFile $oracle \n",0,1);
      print localtime()." NetMonitor-S-UnableToOpenFile $oracle \n;";
      sleep $self->{sleep};
      goto aga2;
                        }
    while  (<FILE>){
        $pwd=$_;
    }
    close FILE;
      if(not $self->{sqlserver}->{dbhandler}=DBI->connect('DBI:'.$self->{sqlserver}->{dbdriver}.$self->{sqlserver}->{dbfile},$user,$pwd,{PrintError => 1, AutoCommit => 1})){
      $self->sendmailpolicy("NetMonitor-S-CannotConnectToOracle $DBI::errstr \n",0,1);
      print localtime()." NetMonitor-S-CannotConnectToOracle \n";
      sleep $self->{sleep};
      goto aga2;
     
  }


}

sub Run{
    my $self=shift;

    $self->InitOracle();
    my $force=1;
    my $sshTimeout=150;
#    local $SIG{ALRM} = sub { print localtime()." > command timeout\n"; };

    $self->sendmailpolicy("NetMonitor-I-Started \n",1);
    print localtime()." NetMonitor-I-Started \n";
#     $self->{ping} = Net::Ping->new();
    $self->{ping} = Net::Ping->new("tcp");
    $self->{ping}->{port_num} =20001;
    $self->getprodservers();
#    $self->{ping}->{port_num} = getservbyname("http", "tcp");







again:
if(not open(FILE,"<".$self->{hostfile})){
    $self->sendmailpolicy("NetMonitor-S-UnableToOpen File $self->{hostfile} \n",0);
    print localtime()." NetMonitor-S-UnableToOpen File $self->{hostfile} \n";
    sleep $self->{sleep};
    goto again;
}
       my $buf;
    $self->sendmailpolicy("NetMonitor-I-Opened File $self->{hostfile} \n",0);
    print localtime()." NetMonitor-I-Opened File $self->{hostfile} \n";
    if(not read(FILE,$buf,1638400)){
      close FILE;
      $self->sendmailpolicy("NetMonitor-S-ProblemsToReadFile $self->{hostfile} \n",0);
      print localtime()." NetMonitor-S-ProblemsToReadFile $self->{hostfile} \n";
      sleep $self->{sleep};
      goto again;
    }     
      $self->sendmailpolicy("NetMonitor-I-ReadFile $self->{hostfile} \n",0);
      print localtime()." NetMonitor-I-ReadFile $self->{hostfile} \n";
    close FILE;
    my @sbuf=split "\n",$buf;
    $#{$self->{hosts}}=-1;
    foreach my $line (@sbuf){
     if($line=~/^\#/){
        next;
     } 
     my $found =0;
     for my $excluded (@{$self->{excluded}}){
      if($line=~/$excluded/){
       $found=1;
       last;
     }
} 
     if($found){
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
    #    print "$host \n";
        if($self->{ping}->ping($host)){
        }
        else{
            push @{$self->{bad}}, $host." ".$mes;
            print localtime()." bad $host \n";
        }
    }

#
# afs volumes space check
#

    $mes="NetMonitor-W-AfsVolumeProblem";
    my $command="/afs/cern.ch/ams/local/bin/timeout $sshTimeout fs listquota ";
#    print "afs check\n";
    foreach my $afsvolume (@{$self->{afsvolumes}}) {
        unlink "/tmp/afsvol";
        my $i=system($command.$afsvolume." | grep WARNING | wc -l > /tmp/afsvol");
        if(1 or not $i){
                if(not open(FILE,"<"."/tmp/afsvol")){
                    push @{$self->{bad}}, "$afsvolume NetMonitor-W-AfsVolumeProblem";
                    print "\n".localtime()." > Afs warning1: $afsvolume\n";
                    next;
                }
                if(not read(FILE,$buf,16384)){
                    push @{$self->{bad}}, "$afsvolume NetMonitor-W-AfsVolumeProblem";
                    print "\n".localtime()." > Afs warning2: $afsvolume\n";
                    close FILE;
                    next;
               }
                close FILE;
                unlink "/tmp/afsvol";
                if($buf != 0){
                    push @{$self->{bad}}, "$afsvolume NetMonitor-W-AfsVolumeProblem";
                    print "\n".localtime()." > Afs warning: $afsvolume\n";
                }else{
#		    print "afs good: $afsvolume\n";
		}
		
            }

    }
{
#director monitor
        my $period = '';
    open (PERIOD, "/afs/cern.ch/user/a/ams/vc/perl/period");
    $period = <PERIOD>;
    close (PERIOD);
    if ($period%10==0) {
                                $self->{dir_monitor}->run;
                                if($self->{dir_monitor}->{error_counter}>0){
                                            push @{$self->{bad}}, $self->{dir_monitor}->{error_message};
                                }
    }
}
# AMI check
my $localtime = '';
my $delta = '';
my $period = '';
    open (PERIOD, "/afs/cern.ch/user/a/ams/vc/perl/period");
    $period = <PERIOD>;
    close (PERIOD);
    unlink "/tmp/strami";
    if ($period == 1) {
        if (my $answ = (system "/afs/cern.ch/ams/local/bin/timeout --signal 9 300 /afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/exe/linux/ami2root.exe 1900000000 2000000000 /tmp/t.root /tmp/t.root 6 1>/tmp/checkami 2>&1") != 0) {
            print "ami2root.exe: No answer\n";
            push @{$self->{bad}}, "ami2root.exe_does_not_reply NetMonitor-W-SlowDBProblem";
        }
        my $str = (system "less /tmp/checkami|grep 'last time' 1>/tmp/strami 2>&1");
#   print "$str\n"; 

        open (TIME, "/tmp/strami");
        my $time = <TIME>;
        my @in = split (/ /, $time);
    print ("ami time is $in[4]\n"); 
        $localtime = time();
        $delta = ($localtime - $in[4]);
     print ("delta time is $delta\n");
        if (($localtime - $in[4]) > 7200) {
            print "localtime is $localtime, old one: delta is $delta\n";
	    push @{$self->{bad}}, " slow_database_last_update_time_too_far_in_the_past:-$delta  NetMonitor-W-SlowDBProblem";
#	    $self->sendmailpolicy("slow_database_last_update_time_too_far_in_the_past:-$delta\n",0);
            }
        close TIME;
#        unlink "/tmp/checkami";
        } 
    if (($period++) == 30){
        $period = 1;
        }
    open (PERIOD, ">/afs/cern.ch/user/a/ams/vc/perl/period");
    print (PERIOD "$period");
    print ("period is $period\n");
    close (PERIOD);

#fs check

    $command="/afs/cern.ch/ams/local/bin/timeout $sshTimeout ssh -2 -x -o \'StrictHostKeyChecking no \' ";
    $mes="NetMonitor-W-NodeFileSystemProblem";
    foreach my $host (@{$self->{hosts}}){
#        print "Fs check: $host\n";
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
        my $i="";
        foreach my $fs (@{$self->{filesystems}}){
            print ".";
            unlink "/tmp/filesys";
#            alarm $sshTimeout;
            $i=system($command.$host." \'ls /".$fs."\' | grep -v \'/".$fs."\' | grep -v \'error\' | wc -l > /tmp/filesys");
#            alarm 0;
            if(1 or not $i){
                if(not open(FILE,"<"."/tmp/filesys")){
                    push @{$self->{bad}}, $host." NetMonitor-W-ssh1Failed";
                    print "\n".localtime()." > Fs error 1: $host $fs\n";
                    next;
                }
                if(not read(FILE,$buf,16384)){
                    push @{$self->{bad}}, $host." NetMonitor-W-ssh2Failed";
                    print "\n".localtime()." > Fs error 2: $host $fs\n";
                    close FILE;
                    next;
               }
                close FILE;
                unlink "/tmp/filesys";
                if($buf == 0){
                    push @{$self->{bad}}, $host."_$fs NetMonitor-W-NodeFileSystemProblem";
                    print "\n".localtime()." > $host $fs NetMonitor-W-NodeFileSystemProblem\n";
                }else{
#		    print "Fs good: $host $fs\n";
		}
		
            }
        }
    }
#print "@{$self->{bad}}\n";

#
# sanboxes
#

##temporary closed
#	$mes="NetMonitor-W-SanBoxesProblems";
#	$command="/afs/cern.ch/ams/local/bin/timeout $sshTimeout ssh -2 -x -o \'StrictHostKeyChecking no \' $self->{sbhost} /afs/cern.ch/ams/local/bin/check_sanbox.pl";
#        my $i1=system("$command -fsanbox01 >/tmp/sbhost.$$");
#        my $i2=system("$command -fsanbox02 >>/tmp/sbhost.$$");
#    if($i1 or $i2){
#                push @{$self->{bad}}, "$self->{sbhost} NetMonitor-W-ssh4Failed ";
#    }
#    else{
#            if(not open(FILE,"<"."/tmp/sbhost.$$")){
#                push @{$self->{bad}}, " /tmp/sbhost.$$ NetMonitor-W-ssh5Failed ";
#                print "\n".localtime()."   /tmp/sbhost.$$ NetMonitor-W-ssh5Failed \n";
#            }
#            else{
#            my @lines=<FILE>;
#            close FILE;
#    foreach my $bl (@lines){
#        $bl=~s/ /\_/g;
#            push @{$self->{bad}}, "$bl $mes ";
#    }
#
#        }
#        }
#            unlink "/tmp/sbhost.$$";

           
	#
	# dbhosts targets
	#
        my @foundp=();
        foreach my $target (@{$self->{dbhoststargets}}){
            push @foundp, 0;
        }
	$mes="NetMonitor-W-DBHostsTargetsProblems";
	$command="/afs/cern.ch/ams/local/bin/timeout $sshTimeout ssh -2 -x -o \'StrictHostKeyChecking no \' ";
	foreach my $host (@{$self->{dbhosts}}){
            print ".";
          unlink "/tmp/dbhosts.$$";
         #print "$command.$host. \n";
#        alarm $sshTimeout;
   my $try=0;
   my $i=0;
oncemore:
	my $qqq = "$command $host"."   ps -f -uams >/tmp/dbhosts.$$";
	$i=system($qqq);
#        $i=system($command $host."   ps -f -uams >/tmp/dbhosts.$$");
         if(!$i){
           $try++;
            sleep(2);
            if($try<10){
               goto oncemore;
            } 
          } 
#        alarm 0;
        if(1 or not $i){
            if(not open(FILE,"<"."/tmp/dbhosts.$$")){
                push @{$self->{bad}}, $host." NetMonitor-W-ssh3Failed /tmp/dbhosts";
                print "\n".localtime()." > $host NetMonitor-W-ssh3Failed /tmp/dbhosts\n";
                next;
            }
            my @words=<FILE>;
            close FILE;
            unlink "/tmp/dbhosts.$$";
             my $nt=-1;
            my $twp="";
            my $pn=-1;
             foreach my $target (@{$self->{dbhoststargets}}){
                 my $found=0;
#                print "checking $target on $host\n";
              foreach my $word (@words) {
                if($word=~/$target/){
                   $nt++;
                   $found=1;
                   last;
                 }
                }
                $pn++;
                if($found==0){
#                    $foundp[$pn]=0;
#                    $twp=$twp."\_$target\_";
                }else{
                    $foundp[$pn]=1;
                }
             }
#               print " joptat $nt \n;";
	}

}
    my $cnt=0;
    foreach my $target (@{$self->{dbhoststargets}}){
        if($foundp[$cnt]==0){
            push @{$self->{bad}}, "$target NetMonitor-W-DBHostsTargetsProblems ";
            print "\n".localtime()." > $target NetMonitor-W-DBHostsTargetsProblems\n";
        }
        $cnt++;
    }
#print "@{$self->{bad}}\n";

#
# df
#
    $mes="NetMonitor-W-SomeHostsHaveDiskSpaceProblems";
    $command="/afs/cern.ch/ams/local/bin/timeout $sshTimeout ssh -2 -x -o \'StrictHostKeyChecking no \' ";
    foreach my $host (@{$self->{hosts}}) {
        my $gonext=0;
        foreach my $bad (@{$self->{bad}}){
            my @sbad=split ' ',$bad;
            if($sbad[0] ){
                $gonext=1;
                last;
            }
        }
        if($gonext){
            next;
        }
        print ".";
        unlink "/tmp/xspace";
         #print "$command.$host. \n";
#        alarm $sshTimeout;
        my $i=system($command.$host." df -x nfs -x gfs > /tmp/xspace ");
#        alarm 0;
        if(1 or not $i){
            if(not open(FILE,"<"."/tmp/xspace")){
                push @{$self->{bad}}, $host." NetMonitor-W-ssh1Failed";
                print "\n".localtime()." > $host NetMonitor-W-ssh1Failed\n";
                next;
            }
            my @words=<FILE>;
            close FILE;
            unlink "/tmp/xspace";
              foreach my $word (@words) {
                if($word =~/\/$/){
                    my @sword= split ' ',$word;
                    if($#sword>1){
                        my $pc=$sword[$#sword-1];
                        my $ava=$sword[$#sword-2];
                        $pc=~ s/\%//;
                        if($ava < 100000 or $pc<2){
                            push @{$self->{bad}}, $host." NetMonitor-W-DiskSpaceProblems";
                            print "\n".localtime()." > $host NetMonitor-W-DiskSpaceProblems\n";
                       }
                        last;
                    }
                }
            }
        }
         else{
                push @{$self->{bad}}, $host." NetMonitor-W-sshFailed";
                print "\n".localtime()." > $host NetMonitor-W-sshFailed\n";
                next;
            }
        }
###
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
                 print localtime()."\n > $bada\n";
               }
    }
    else{
#      $self->sendmailpolicy("NetMonitor-I-AllHostsAreOK",0);
      $self->sendmailpolicy("NetMonitor-I-AllHostsAreOK",$force);
      $force=0;
      print "\n".localtime()." > NetMonitor-I-AllHostsAreOK\n";
    }
     
    sleep($self->{sleep});

}
}

sub sendmailpolicy{
   my $self=shift; 
   my $subj=shift;
   my $force=shift;
   my $nooracle=shift;
   my $curtim=time();
   if(not defined $nooracle){
     $nooracle=0;
   } 
   if(not defined $force){
       $force=0;
   }
   my $f2=$force;
   if($subj=~/-I-/){



       for my $i (0..$#{$self->{sendmail}}){
           my $hash=${$self->{sendmail}}[$i];
           my $ok=0;
           my @sadd=split ' ',$hash->{address};
           if($hash->{sent}>0  or $force){
               foreach my $add (@sadd){
                $ok+=$self->sendmailmessage($add,$subj," ");
                if(!$ok){
                  $f2=1;
                }
               }
           } 
           if($ok<$#sadd+1){
            $hash->{sent}=0;
            $hash->{timesent}=0;
           }
   }


#
#   Oracle
#

       if($f2 or $curtim-$self->{sqlserver}->{lastupdate}>$self->{sqlserver}->{repet}){
           if($nooracle or $self->updateoracle($subj," ")){
           }   
           else{
            $self->InitOracle();
            if(not $self->updateoracle($subj," ")){
             $self->sendmailpolicy("NetMonitor-E-UnableToConnectOracle",0,1);
           }
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
#
#  group bad hosts by subject
#
               my %badh=();
               my $mes="";
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

               foreach my $subj (keys %badh){
               foreach my $add (@sadd){
                $ok+=$self->sendmailmessage($add,$subj,$badh{$subj});
                if(!$ok){
                 $f2=1;
                }
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
#
#   Oracle
#
       if($f2 or $curtim-$self->{sqlserver}->{lastupdate}>$self->{sqlserver}->{repet}){
    if(scalar(keys(%badh)) ==0){
          if($nooracle or $self->updateoracle($subj,$badh{$subj})){
           }              
           else{
            $self->InitOracle();
            if(not $self->updateoracle($subj,$badh{$subj})){
             $self->sendmailpolicy("NetMonitor-E-UnableToConnectOracle",0,1);
           }            
        }
      }
         foreach my $subj (keys %badh){
           if($nooracle or $self->updateoracle($subj,$badh{$subj})){
           }              
           else{
            $self->InitOracle();
            if(not $self->updateoracle($subj,$badh{$subj})){
             $self->sendmailpolicy("NetMonitor-E-UnableToConnectOracle",0,1);
           }            
       }
        }    

}}

}
}


sub sendmailmessage{
    my $self=shift;
    my $add=shift;
    my $sub=shift;
    my $mes=shift;
    open MAIL, "| /usr/lib/sendmail -t -i"
#        or die "could not open sendmail: $!";
        or return 1;
     print MAIL <<END_OF_MESSAGE2;
To:  $add
Subject: $sub

$mes
END_OF_MESSAGE2
    close MAIL or return 1;
    return 0;
}


sub set_oracle_env {
    $ENV{"ORACLE_HOME"}='/afs/cern.ch/project/oracle/@sys/prod';
#    $ENV{"TNS_ADMIN"}='/afs/cern.ch/project/oracle/admin';
 $ENV{"TNS_ADMIN"}='/afs/cern.ch/exp/ams/Offline/oracle/admin';
#$ENV{"TNS_ADMIN"}='/opt/oracle/10g/network/admin';
    $ENV{"LD_LIBRARY_PATH"}=$ENV{"ORACLE_HOME"}."/lib";
    1;
}

sub updateoracle{
    my $self=shift;
    my $subj=shift;
    my $hosts=shift;
    my $curtim=time();
    my $category="U";
   
    if($subj=~/-I-/){
        $category="I";
    }
    elsif($subj=~/-W-/){
        $category="W";
    }
    elsif($subj=~/-E-/){
        $category="E";
    }
    elsif($subj=~/-S-/){
        $category="S";
    }
#
# delete all the messages less than retention period
#
    my $deltim=$curtim-$self->{sqlserver}->{keep};
    my $sql="delete from netmon where timestamp<$deltim";
    if(not $self->{sqlserver}->{dbhandler}->do($sql)){
        return 0;
    }
    $sql="insert into netmon values('$subj','$hosts',$curtim,'$category')";
    if(not $self->{sqlserver}->{dbhandler}->do($sql)){
        return 0;
    }
    $self->{sqlserver}->{lastupdate}=$curtim;
    return 1;
}

sub getprodservers{
    my $self=shift;
    my $sql="select unique hostname from servers where hostname is not null";

    my $sth=$self->{sqlserver}->{dbhandler}->prepare($sql);
    $sth->execute;
    my $ret=$sth->fetchall_arrayref();

    if( defined $ret->[0][0]){
        foreach my $host (@{$ret}){
            push @{$self->{dbhosts}}, $host->[0];
#            print $host->[0];
        }
    }else{
        push @{$self->{dbhosts}}, "pcamss0.cern.ch";
    }
}
