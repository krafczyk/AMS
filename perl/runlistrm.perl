#!/usr/bin/perl -w
#  $Id: checkfs.cgi,v 1.12 2011/03/30 12:48:35 dmitrif Exp 
use strict;
use Carp;
use lib qw(../perl);
use lib::DBSQLServer;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";

# DESCRIBE DATAFILES
# RUN                                       NOT NULL NUMBER(38)
# VERSION                                            VARCHAR2(64)
# TYPE                                               VARCHAR2(64)
# FEVENT                                             NUMBER(38)
# LEVENT                                             NUMBER(38)
# NEVENTS                                            NUMBER(38)
# NEVENTSERR                                         NUMBER(38)
# TIMESTAMP                                          NUMBER(38)
# SIZEMB                                             NUMBER(38)
# STATUS                                             VARCHAR2(64)
# PATH                                               VARCHAR2(255)
# PATHB                                              VARCHAR2(255)
# CRC                                                NUMBER(38)
# CRCTIME                                            NUMBER(38)
# CASTORTIME                                         NUMBER(38)
# BACKUPTIME                                         NUMBER(38)
# TAG                                                NUMBER(38)
# FETIME                                             NUMBER(38)
# LETIME                                             NUMBER(38)
# PATHS                                              VARCHAR2(255)


my $run2p="";
    my $o=new DBSQLServer();
     my $ok=$o->ConnectRO();
    my @rungood=();
    my @runbeg=();
    my @runend=();
    my @runlist=();
    my @runalist=();
if($ok){
    my $time=time();
    my $sql = "SELECT run,fetime,letime fROM amsdes.datafiles where  status  like '%OK%' and type like 'SCI%'   and run>1305800000  $run2p  and run <=1337244535 ORDER BY run ";
         my $ret=$o->Query($sql);
    my $okk=0;
    foreach my $file (@{$ret}){
              push @rungood,$file->[0];
              push @runbeg,$file->[1];
              push @runend,$file->[2];
          }
}
else{
    return 1;
}



my $output="/afs/cern.ch/ams/Offline/AMSDataDir/SlowControlDir/";
    if(defined $ENV{SCDBGlobal}){
        $output=$ENV{SCDBGlobal};
    }
my $min=1305810000;
my $max=4000000000;
my $list="cern.runlist";
my $alist="cern.runalist";
my $v=0;

my $HelpTxt=" -ref -force -v ";
my $force=0;
my $ref="";
my $rmc=undef;
   foreach my $chop  (@ARGV){
    if($chop =~/^-ref/){
      $ref=unpack("x4 A*",$chop);
    }
    if($chop =~/^-rmc/){
      $rmc=unpack("x4 A*",$chop);
    }
    if($chop =~/^-force/){
        $force=1;
    }
      if ($chop =~/^-v/) {
        $v=1;
      }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
}
my $runlistr="";
if(defined $rmc){
$list="$rmc.runlist";
$alist="$rmc.runalist";
if(open(FILE,"</f2users/scratch/MC/"."$rmc/$rmc".".runlist")){
$runlistr=<FILE>;
if($runlistr=~/\,$/){
chop $runlistr;
}
$runlistr=~s/\~//g;
close FILE;
}
   else{
 die" unable to open $rmc runlist \n";
}
}
#check max time in db
my @runlistrm=split '\,',$runlistr;
            opendir THISDIR,$output or die "checka2r.perl-F-UnableToOpen $output \n";
            my @files=readdir THISDIR;
            closedir THISDIR;
            my @tmpa=sort @files;
            foreach my $file (@tmpa){
                if($file=~/^SCDB/ and $file=~/.root$/){
                    my @junk=split '\.',$file;
                    my $t1=$junk[1];
                    my $t2=$junk[2];
                    for my $i (0...$#rungood){
                        if(defined $rungood[$i] and $runbeg[$i]>$t1 and $runend[$i]<$t2){
                           push @runlist,$rungood[$i];
                           delete $rungood[$i];
                           delete $runbeg[$i];
                           delete $runend[$i];
                       }
                    }
                }
            }
my $lst=0;
my $alst=0;
if(defined $rmc){
    @rungood=@runlist;
                    for my $i (0...$#rungood){
                        if(defined $rungood[$i]){
                        if($rungood[$i]=~/1308224335/){
                        }
                        foreach my $run (@runlistrm){
                            if( $rungood[$i]==$run){
                                delete $rungood[$i];
                                last;
                            }
                        }
                    }
                   }
                    for my $i (0...$#runlist){
                        my $ok=0;
                        foreach my $run (@runlistrm){
                            if(defined $runlist[$i] and $runlist[$i]==$run){
                                $ok=1;
                                last;
                            }
                            if(defined $runlist[$i] and $runlist[$i]<$run){
                                last;
                            }
                        }
                        if($ok==0){
                            delete $runlist[$i];
                        }
                    }
                }
                if(open(FILE,">".$list)){
                    foreach my $run (@runlist){
                       if(defined $run){
                        print FILE "$run,";
                        $lst++;
                    }
                    }
                    close FILE;
                }
                if(open(FILE,">".$alist)){
                    foreach my $run (@rungood){
                       if(defined $run){
                           print FILE "$run,";
                           $alst++;
                       }
                    }
                    close FILE;
                }

print " Total of : $#rungood Good $lst NotGood $alst \n";            
