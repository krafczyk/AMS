# $Id: RemoteClient.pm,v 1.279 2004/09/23 11:31:10 alexei Exp $
#
# Apr , 2003 . ak. Default DST file transfer is set to 'NO' for all modes
#
# to add - standalone/client type,
#          CPU, elapsed time, cite and host info into Job table
#
# Apr-May, 2003 : ValidateRuns, checkJobsTimeout, deleteTimeOutJobs,
#                 parseJournalFiles
# Jun 2003      : script to set links
#
# Jul 2003      : add book-keeping tarball
#                 ValidateRuns : "local" runs
# Aug 2003      : remove explicit server name, e.g. pcamsf0.cern.ch
#                 from scripts names
# Sep 2003      : outputpath and $ntdir = /disk/dir   for local disks
#                                         /dir        for afs 
#                 add MIPS column into Jobs table
# Oct 2003      : updateHostInfo and calculateMips functions
#                 validateRuns is modified - add clock for local hosts
# Dec 4 2003    : increase number of cites to 32 (16/32, 27/26)
#
# Jan    2004    : queryDB changed drastically -> queryDB04
#                : parsejournalfile : closedst block, do nothing if DST file status ne "Validated"
#                : NFS, Web, CASTOR files access mode
#                  TriggerLVl1 - default trigger type
#                : sub checkDB -d   DirectoryPath
#                              -p   DBPath
#                              -crc do CRC check
#                              -u   update DB
#                              -o   print output to file
# Feb    2004    : $webmode, subs : printParserStat and amsprint 
#                  print statistics for each cite
#                  parser : verbose, cmd, web modes
#                           username and nFailedCopiesInRow check
#                  validateRuns : verbose, cmd, web modes
#                : stopParseJournalFiles
#                  decrease one sql query in updateRunCatalog
#                : checkDB add -i option
#
# Mar     2004   : if DST status is not 'Validated' in CloseDTS block, skip it
#                  if CRC=0 didn't check it either
#                : getHostsList sub, getDisks add MC[GB] - GB for MC's DSTs 
#                : listJobs, listNtuples, listRuns - modified to speed up output
#                : getHostsMips & updateHostsMips subs
#                  2 new tables cern_hosts and cpu_coeff
#                : argument list of fastntrd is modified, lastEvent is added
#                  implement timing (decrease number of logfiles, f.e. /tmp/cp.log
#                : always validate DSTs
#                : $rmpromt option
# Apr 15, 2004   : getRunInfo sub
# Apr 18, 2004   : validateRuns - check number of events and errors if both = 0
#                : mark run as 'Unchecked', even if server's status is 'Finished'
#
# May 11, 2004   : check CRC in validateRuns before file copying
#                : don't validate run if status != 'Finished' 
#                : journal files : CRC/crc
#                 
# June 2, 2004   : Jean Jacquemier & Alexei Klimentov : updateCopyStatus
#                  functions to keep and update information about DSTs 
#                  copied to remote cites.
#
# June 16, 2004  : pcamss0.cern.ch is a primary HTTP server
#
# July  2, 2004  : prepareCastorCopyScript, updateDSTPath subr
#
# Sep  22, 2004   : AMS01 production, add $ProductionStartTime in quaries
#                   no predefined $dbversion in Connect()
#
#
my $nTopDirFiles = 0;     # number of files in (input/output) dir 
my @inputFiles;           # list of file names in input dir

package RemoteClient;
use CORBA::ORBit idl => [ '../include/server.idl'];
use Error qw(:try);
use CGI qw(-unique_headers);
use Carp;
use strict;
use lib::Warning;
use MIME::Lite;
use lib::CID;
use lib::DBServer;
use Time::Local;
use lib::DBSQLServer;
use POSIX  qw(strtod);             
use File::Find;

@RemoteClient::EXPORT= qw(new  Connect Warning ConnectDB ConnectOnlyDB checkDB listAll listMin listShort queryDB04 DownloadSA  checkJobsTimeout deleteTimeOutJobs deleteDST  getEventsLeft getHostsList getHostsMips getOutputPath getRunInfo updateHostInfo parseJournalFiles prepareCastorCopyScript resetFilesProcessingFlag ValidateRuns updateAllRunCatalog printMC02GammaTest set_root_env updateCopyStatus updateHostsMips);


my     $webmode         = 1; # 1- cgi is executed from Web interface and 
                             # expects Web like output
                             # 0- cgi is executed from command line
my     $verbose         = 0; # verbose mode
my     $rmprompt        = 1; # prompt before files removal

#+ parser Statistics
 my $parserStartTime     = 0; # start journal files check
 my $validateStartTime   = 0; # start runs validation

 my $nCheckedCite = 0;  # cites dirs checked
 my $nActiveCites = 0;  # cites with new dsts 
 my @JouDirPath   = [];
 my @JouLastCheck = [];
 my @JournalFiles = [];  # number of journal files
 my @CheckedRuns  = [];  # runs processed
 my @FailedRuns   = [];  # failed runs
 my @GoodRuns     = [];  # marked as 'Completed'
 my @BadRuns      = [];  # marked as 'Unckecked' or 'Failed'
 my @CheckedDSTs  = [];  # dsts checked
 my @GoodDSTs     = [];  # copied to final destination
 my @BadDSTs      = [];  # marked as bad because of different reasons
                         # in particular :
 my @BadDSTCopy   = [];  #                 doCopy failed to copy DST
 my @BadCRCi      = [];  #                 dsts with crc error (before copying)
 my @BadCRCo      = [];  #                 dsts with crc error (after copying)
 my @gbDST        = [];

 my $doCopyTime   = 0;
 my $doCopyCalls  = 0;
 my $crcTime      = 0;
 my $crcCalls     = 0;
 my $fastntTime   = 0;
 my $fastntCalls  = 0;
 my $copyTime     = 0;
 my $copyCalls    = 0;
#-
 my $ActiveProductionSet   = undef;
 my $ProductionStartTime   = 0;
 my $defROOT    = "CHECKED";
 my $defNTUPLE  = " ";

  my $UNKNOWN               = 'unknown';

 my $nBadCopiesInRow = 0;   # counter of doCopy errors
 my $MAX_FAILED_COPIES = 5; # max number of allowed errors (in row)
#
my     $bluebar      = 'http://ams.cern.ch/AMS/icons/bar_blue.gif';
my     $maroonbullet = 'http://ams.cern.ch/AMS/icons/bullet_maroon.gif';
my     $bluebullet   = 'http://ams.cern.ch/AMS/icons/bullet_blue.gif';
my     $silverbullet = 'http://ams.cern.ch/AMS/icons/bullet_silver.gif';
my     $purplebullet = 'http://ams.cern.ch/AMS/icons/bullet_purple.gif';

my     $srvtimeout = 30; # server timeout 30 seconds
my     @colors=("red","green","blue","magenta","orange","cyan","tomato");

my     $amscomp_page ='http://ams.cern.ch/AMS/Computing/computing.html';
my     $amshome_page ='http://ams.cern.ch/AMS/ams_homepage.html';
my     $dbqueryR     ='http://ams.cern.ch/AMS/Computing/dbqueryR.html';


my     $downloadcgi       ='/cgi-bin/mon/download.o.cgi';
my     $downloadcgiMySQL  ='/cgi-bin/mon/download.mysql.cgi';

my     $monmcdb           ='/cgi-bin/mon/monmcdb.o.cgi';
my     $monmcdbMySQL      ='/cgi-bin/mon/monmcdb.mysql.cgi';

my     $rccgi             ='/cgi-bin/mon/rc.o.cgi?queryDB04=Form';
my     $rccgiMySQL        ='/cgi-bin/mon/rc.mysql.cgi?queryDB04=Form';

my     $rchtml            ='rc.html';

my     $rchtmlMySQL       ='rc.mysql.html';

my     $validatecgi       ='/cgi-bin/mon/validate.o.cgi';
my     $validatecgiMySQL  ='/cgi-bin/mon/validate.mysql.cgi';

my     $PrintMaxJobsPerCite = 25;

my     $MAX_CITES       = 32; # maximum number of allowed cites
my     $MAX_RUN_POWER   = 26; # 

#
my     $MIN_DISK_SPACE  = 10; # if available disk space <  MIN_DISK_SPACE 
                              # do not use disk to store DSTs
#
sub new{
    my $type=shift;
my %fields=(
        FinalMessage=>undef,
        Runs=>[],
        DataSetsT=>[],
        TempT=>[],
        MailT=>[],
        CiteT=>[],
        FilesystemT=>[],
        NickT=>[],
        AMSSoftwareDir=>undef,
        AMSDataDir=>undef,
        AMSProdDir=>undef,
        AMSDSTOutputDir=>undef,
        CERN_ROOT=>undef,
        ROOTSYS  =>'/afs/ams.cern.ch/Offline/root/Linux/pro/',
        HTTPserver=>'pcamss0.cern.ch',
        HTTPhtml  =>'http://pcamss0.cern.ch/',
        HTTPcgi   =>'http://pcamss0.cern.ch/cgi-bin/mon',
        UploadsDir=>undef,
        UploadsHREF=>undef,
        FileDB=>undef,
        FileCRC=>undef,
        FileBBFTP=>undef,
        FileBookKeeping=>undef,
        FileAttDB=>undef,
        FileCRCTimestamp=>undef,
        FileBBFTPTimestamp=>undef,
        FileBookKeepingTimestamp=>undef,
        FileDBTimestamp=>undef,
        FileAttDBTimestamp=>undef,
        FileDBLastLoad=>undef,
        FileAttDBLastLoad=>undef,
        LocalClientsDir=>"prod.log/scripts/",
        Name=>'/cgi-bin/mon/rc.cgi',
        DataMC=>0,
        IOR=>undef,
        IORP=>undef,
        ok=>0,
        q=>undef,
       read=>0,
       debug=>0,
       CEM=>undef,
       CECT=>undef,
       tsyntax=>undef,
       cputypes=>undef,
       triggertypes=>undef,
       arsref=>[],
       arpref=>[],
       ardref=>[],
       dbfile=>undef,
       scriptsOnly=>1,
       senddb=>0,
       sendaddon=>0,
       dwldaddon=>0,
       ProductionVersion=>undef,
       Version=>-1,
       Build=>-1,
       OS=>-1,
       ScriptFileDir=>'XYZ'
            );
    my $self={
      %fields,
    };


foreach my $chop  (@ARGV){
    if($chop =~/^-d/){
        $self->{debug}=1;
    }
    if($chop =~/^-N/){
       $self->{Name}= unpack("x2 A*",$chop);
    }


}
    $self->{q}=new CGI;
    my $mybless=bless $self,$type;
    if(ref($RemoteClient::Singleton)){
        croak "Only Single RemoteClient Allowed\n";
    }
$RemoteClient::Singleton=$mybless;
return $mybless;
}

sub Init{
    my $self = shift;
#
    my $sql  = undef;
    my $ret  = undef;

#just temporary skeleton to check basic princ
#should be replaced by real db servers

#cpu types

    my %cputypes=(
     'Pentium II'=>1.07,
     'Pentium III'=>1.0,
     'Pentium III Xeon'=>1.05,
     'Pentium IV Xeon'=>0.8,
     'Pentium IV'=>0.7,
     'AMD Duron'=>1.0,
     'AMD Athlon'=>1.15
                   );

    $self->{cputypes}=\%cputypes;

my %triggertypes=(
     'AMSParticle'=>0,
     'TriggerLVL1'=>1
                 );
    $self->{triggertypes}=\%triggertypes;

#syntax (momenta,particles, ...

    my $tsyntax={};
    $self->{tsyntax}=$tsyntax;
    my %particles=(
     'gamma'=>1,
     'positron'=>2,
     'electron'=>3,
     'muon+'=>5,
     'muon-'=>6,
      'pi0'=>7,
      'pi+'=>8,
      'pi-'=>9,
     'proton'=>14,
     'antiproton'=>15,
     'Deutron'=>45,
     'AntiDeutron'=>145,
     'Tritium'=>46,
     'He4'=>47,
     'AntiHe4'=>147,
     'Geantino'=>48,
     'He3'=>49,
     'Li6'=>61,
     'Li7'=>62,
     'Be7'=>63,
     'Be9'=>64,
     'B10'=>65,
     'B11'=>66,
     'C12'=>67,
     'N14'=>68,
     'O16'=>69,
     'Fe56'=>87,
                   );
    $tsyntax->{particles}=\%particles;
   


    my %spectra=(
      cosmic=>0,
undercutoff=>1,
sealevelmuons=>2,
lowenergyelectrons=>3,
uniform=>4,
uniformlog=>5,
                 );
    $tsyntax->{spectra}=\%spectra;
    my %focus=(
      NoFocusing=>0,
      ECALFocusing=>1,
      TRDHoneycombFocusing=>2,
                 );

    $tsyntax->{focus}=\%focus;
    my %planes=(
      AllSurfaces=>0,
      TopOnly=>1,
      BottomOnly=>2,
                 );

    $tsyntax->{planes}=\%planes;
    

my %default=(
     triggers=>10000000,     
     particle=>"proton",
     spectrum=>"cosmic",
     pmin=>0.1,
     pmax=>10000.,
     costhetamax=>1.,
     runnumber=>100,
     outputdirpath=>"",
     cpulimit=>100.,
              );
    $tsyntax->{default}=\%default;

my %mv=(
     triggers=>" ",     
     particle=>" ",
     spectrum=>" ",
     pmin=>"GeV/c",
     pmax=>"GeV/c",
     costhetamax=>" ",
     runnumber=>" ",
     outputdirpath=>" ",
     cpulimit=>"sec",
        );

    $tsyntax->{mv}=\%mv;


#amsdatadir

 my $dir=$ENV{AMSDataDir};
 if (defined $dir){
     $self->{AMSDataDir}=$dir;
 }
 else{
     $self->{AMSDataDir}="/afs/ams.cern.ch/AMSDataDir";
     $ENV{AMSDataDir}=$self->{AMSDataDir};
 }

#sqlserver
    $self->{sqlserver}=new DBSQLServer();
    $self->{sqlserver}->Connect();
#
   $sql = "SELECT myvalue from Environment where mykey='HTTPserver'";
   $ret=$self->{sqlserver}->Query($sql);
     if( defined $ret->[0][0]){
       $self->{HTTPserver}=$ret->[0][0];
     } 
   $self->set_URL();

# mySQL/Oracle 
    if($self->{sqlserver}->{dbdriver} =~ m/mysql/){
        $downloadcgi = $downloadcgiMySQL;
        $monmcdb     = $monmcdbMySQL;
        $rccgi       = $rccgiMySQL;
        $rchtml      = $rchtmlMySQL;
        $validatecgi = $validatecgiMySQL;
    } 
#
 $dir=$ENV{CERN_ROOT};
 if (defined $dir){
     $self->{CERN_ROOT}=$dir;
 }
 else{
     $sql="select myvalue from Environment where mykey='CERN_ROOT'";
     $ret=$self->{sqlserver}->Query($sql);
     if( defined $ret->[0][0]){
       $self->{CERN_ROOT}=$ret->[0][0];
     }
     else{
      $self->{CERN_ROOT}="/cern/2001";
  }
 }
    $dir =$ENV{ROOTSYS};
 if (defined $dir){
     $self->{ROOTSYS}=$dir;
 }
 else{
     my $sql="select myvalue from Environment where mykey='ROOTSYS'";
     my $ret=$self->{sqlserver}->Query($sql);
     if( defined $ret->[0][0]){
       $self->{ROOTSYS}=$ret->[0][0];
     }
     else{
      $self->{ROOTSYS}="/afs/ams.cern.ch/Offline/root/Linux/v3.05.05gcc322/";
  }
 }
#
    my $key='UploadsDir';
    $sql="select myvalue from Environment where mykey='".$key."'";
    $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     $self->{$key}=$ret->[0][0];
    }
    else{    
     $self->{$key}="/var/www/cgi-bin/AMS02MCUploads";
    }

    $key='UploadsHREF';
    $sql="select myvalue from Environment where mykey='".$key."'";
    $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     $self->{$key}=$ret->[0][0];
    }
    else{    
     $self->{$key}="AMS02MCUploads";
    }

    $key='FileDB';
    $sql="select myvalue from Environment where mykey='".$key."'";
    $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     $self->{$key}=$ret->[0][0];
    }
    else{    
     $self->{$key}="ams02mcdb.tar.gz";
    }

    $key='FileAttDB';
    $sql="select myvalue from Environment where mykey='".$key."'";
    $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     $self->{$key}=$ret->[0][0];
    }
    else{    
     $self->{$key}="ams02mcdb.addon.tar.gz";
    }


    $key='FileBBFTP';
    $sql="select myvalue from Environment where mykey='".$key."'";
    $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     $self->{$key}=$ret->[0][0];
    }
    else{    
     $self->{$key}="ams02bbftp.tar.gz";
    }


    $key='FileBookKeeping';
    $sql="select myvalue from Environment where mykey='".$key."'";
    $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     $self->{$key}=$ret->[0][0];
    }
    else{    
     $self->{$key}="ams02bookkeeping.tar.gz";
    }

    $key='FileCRC';
    $sql="select myvalue from Environment where mykey='".$key."'";
    $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     $self->{$key}=$ret->[0][0];
    }
    else{    
     $self->{$key}="ams02crc.tar.gz";
    }


     $key='AMSSoftwareDir';
     $sql="select myvalue from Environment where mykey='".$key."'";
     $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     $self->{$key}="$self->{AMSDataDir}/".$ret->[0][0];
 }
    else{    
     $self->{$key}="$self->{AMSDataDir}/DataManagement";
    }
     $key='AMSProdDir';
     $sql="select myvalue from Environment where mykey='".$key."'";
     $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     $self->{$key}="$self->{AMSSoftwareDir}/".$ret->[0][0];
 }
    else{    
     $self->{$key}="$self->{AMSSoftwareDir}/prod";
    }
    $ENV{$key}=$self->{$key};

#datasets
{
        my $totalcpu=0;
        my $restcpu=0;
     $dir="$self->{AMSSoftwareDir}/DataSets";
     opendir THISDIR ,$dir or die "unable to open $dir";
     my @allfiles= readdir THISDIR;
     closedir THISDIR;    
    foreach my $file (@allfiles){
       my $newfile="$dir/$file";
       if($file =~/^\.Trial/){

           open(FILE,"<".$newfile) or die "Unable to open dataset control file $newfile \n";
           my $buf;
           read(FILE,$buf,16384);
           close FILE;
           $self->{TrialRun}=$buf;          
           last;
       }
    }
    foreach my $file (@allfiles){
        my $newfile="$dir/$file";
        if(readlink $newfile or  $file =~/^\./){
         next;
        }
      my @sta = stat $newfile;
      if($sta[2]<32000){
          my $dataset={};
          $dataset->{name}=$file;
          $dataset->{jobs}=[];

          $dataset->{eventstodo} = 0;

          my @tmpa;
       opendir THISDIR, $newfile or die "unable to open $newfile";
       my @jobs=readdir THISDIR;
       closedir THISDIR;
       push @{$self->{DataSetsT}}, $dataset;
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
           $template->{filename}=$job;
           my @sbuf=split "\n",$buf;
           my @farray=("TOTALEVENTS","PART","PMIN","PMAX","TIMBEG","TIMEND","CPUPEREVENTPERGHZ");
           foreach my $ent (@farray){
            foreach my $line (@sbuf){
               if($line =~/$ent=/){
                   my @junk=split "$ent=",$line;                 
                   $template->{$ent}=$junk[$#junk];
                   $buf=~ s/$ent=/C $ent=/;
                   last;
               }
            }         
        }
        if(defined $self->{TrialRun}){
            $template->{TOTALEVENTS}*=$self->{TrialRun};
        }
        $totalcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};
           $template->{initok}=1;
           foreach my $ent (@farray){
             if(not defined $template->{$ent}){
               $template->{initok}=undef;
             }
           }
#
# get no of events
#
              my $sql="select did from DataSets where name='$dataset->{name}'";
              my $ret=$self->{sqlserver}->Query($sql);
              if( defined $ret->[0][0]){
                 $dataset->{did}=$ret->[0][0];    
                 $sql="select jid,time,triggers,timeout from Jobs where did=$ret->[0][0] and jobname like '%$template->{filename}'";
                 my $r2= $self->{sqlserver}->Query($sql);
                 if(defined $r2->[0][0]){
                     foreach my $job (@{$r2}){
                         if ($job->[1]-time()>$job->[3]){
                             $sql="select FEvent,LEvent from Runs where jid=$job->[0] and status='Finished'";
                             my $r3=$self->{sqlserver}->Query($sql);
                             if(defined $r3->[0][0]){
                              foreach my $run (@{$r3}){
                               $template->{TOTALEVENTS}-=$run->[0];
                              } 
                             }
                         }
                         else {
#
# subtract allocated events
                             $template->{TOTALEVENTS}-=$job->[2];

                         }
                     }
                 }
             }
           else{
               $sql="select did from DataSets";
               $ret=$self->{sqlserver}->Query($sql);
               my $did=0;
               if(defined $ret->[0][0]){
                   foreach my $ds (@{$ret}){
                       if($ds->[0]>$did){
                           $did=$ds->[0];
                       }
                   }
               }
               $did++;
               $dataset->{did}=$did;
               my $timestamp = time();
             $sql="insert into DataSets values($did,'$dataset->{name}',$timestamp)";
             $self->{sqlserver}->Update($sql); 
           }
        $restcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};
          
           if($sbuf[0] =~/^#/ && defined $template->{initok}){

            $buf=~ s/#/C /;
            $template->{filebody}=$buf;
            my $desc=$sbuf[0];
            substr($desc,0,1)=" ";
            $template->{filedesc}=$desc." Total Events Left $template->{TOTALEVENTS}";
            $dataset->{eventstodo} += $template->{TOTALEVENTS};
           if($template->{TOTALEVENTS}>100){
             push @tmpa, $template; 
         }
           }        
       }        
    }
    sub prio { $b->{TOTALEVENTS}*$b->{CPUPEREVENTPERGHZ}  <=> $a->{TOTALEVENTS}*$a->{CPUPEREVENTPERGHZ};}
    my @tmpb=sort prio @tmpa;
    foreach my $tmp (@tmpb){
     push @{$dataset->{jobs}},$tmp;     
    }
      }
    }
#die "  total/rest  $totalcpu  $restcpu \n";
}
#templates

     $dir="$self->{AMSSoftwareDir}/Templates";
    opendir THISDIR ,$dir or die "unable to open $dir";
    my @allfiles= readdir THISDIR;
    closedir THISDIR;    
foreach my $file (@allfiles){
    if($file =~ /\.job$/){
        my $temp={};
        my $full=$dir."/$file";
        open(FILE,"<".$full) or die "Unable to open file $full \n";
        my $buf;
        read(FILE,$buf,1638400) or next;
        close FILE;
        $temp->{filename}=$file;
        my @sbuf=split "\n",$buf;
        if($sbuf[0] =~/^#/){
           $buf=~ s/#/C /;
           $temp->{filebody}=$buf;
           my $desc=$sbuf[0];
           substr($desc,0,1)=" ";
           $temp->{filedesc}=$desc;
           push @{$self->{TempT}}, $temp; 
        }        
    }
    
        if($file =~/^\.Help/ ){
         my $full=$dir."/$file";
         open(FILE,"<".$full) or die "Unable to open file $full \n";
         my $buf;
         read(FILE,$buf,1638400) or die "Unable to read file $full \n";;
         $tsyntax->{headers}->{help}=$buf;
         close FILE;
        }
    if ($file =~/^\./){
        if($file =~/^\.Header/ ){
        my $full=$dir."/$file";
        open(FILE,"<".$full) or die "Unable to open file $full \n";
        my $buf;
        read(FILE,$buf,1638400) or die "Unable to read file $full \n";;
        my $str=substr($file,7);
        $tsyntax->{headers}->{$str}=$buf;
        close FILE;
        }
        else {
            next;
        }
}
} 
    if (not defined $self->{TempT}){
        die "No Basic Templates Available";
    }

#cites table
     $sql="select * from Cites";
    my ($values, $fields)=$self->{sqlserver}->QueryAll($sql);
    foreach my $row (@{$values})  {
     my $cite={};
     my $i=0;
     foreach my $field (@{$fields}){
      $cite->{lc($field)}=$row->[$i++];
     }
     push @{$self->{CiteT}}, $cite; 
    }


#mail table
    $sql="select * from Mails";
     ($values, $fields)=$self->{sqlserver}->QueryAll($sql);
    foreach my $row (@{$values})  {
     
     my $cite={};
     my $i=0;
     foreach my $field (@{$fields}){
#         warn " fileds $field $row->[$i] \n";
      $cite->{lc($field)}=$row->[$i++];
     }
     push @{$self->{MailT}}, $cite; 
 }
# filesystems table
     $sql="select * from Filesystems WHERE status='Active'";
     ($values, $fields)=$self->{sqlserver}->QueryAll($sql);
     foreach my $row (@{$values}) {
         my $fs={};
         my $i =0;
         foreach my $field (@{$fields}) {
             $fs->{lc($field)}=$row->[$i++];
         }
         push @{$self->{FilesystemT}},$fs;
     }

#try to get ior
    $sql="select dbfilename,lastupdate,IORS,IORP from Servers where status='Active'";
     $ret=$self->{sqlserver}->Query($sql);
    my $updlast=0;
    foreach my $upd (@{$ret}){
        if($upd->[1]> $updlast){
            $updlast=$upd->[1];
            $self->{dbfile}=$upd->[0];
            $self->{IOR}=$upd->[2];
            $self->{IORP}=$upd->[3];
        }        
    } 

 if ($webmode == 0 && $verbose == 1) {print "Init -I- get IOR from Server or DB \n";}
 my $ior=$self->getior();
 if(not defined $ior){ 
    if ($webmode == 0 && $verbose == 1) {print "Init -I- IOR not defined \n";}

  foreach my $chop  (@ARGV){
    if($chop =~/^-I/){
        $ior=unpack("x1 A*",$chop);
    }
  }
}
    $self->setActiveProductionSet();

    if(defined $ior ){
      if ($webmode ==0) {print "Init -I- IOR got it \n";}
      chomp $ior;
      if($self->{IOR} ne $ior){
        $self->{IOR}=$ior;
        $self->{IORP}=undef;
      }
  }
 if( not defined $self->{IOR}){
  return $self->{ok};
 } 
    return 1; 
}

sub ServerConnect{
    my $ref = shift;
$ref->{cid}=new CID;    
$ref->{orb} = CORBA::ORB_init("orbit-local-orb");
    my $tm={};
 try{
  $tm = $ref->{orb}->string_to_object($ref->{IOR});
  $ref->{arsref}=[$tm];
}
 catch CORBA::MARSHAL with{
     die "MARSHAL Error "."\n";
 }
 catch CORBA::COMM_FAILURE with{
     die "Commfailure Error "."\n";
 }
 catch CORBA::SystemException with{
     die "SystemException Error "."\n";
 };

#get db server id


     try{
         my %cid=%{$ref->{cid}};
         $cid{Type}="DBServer";
      my ($length,$pars)=${$ref->{arsref}}[0]->getARS(\%cid,"Any",0,1);
         if($length==0 ){
         }
         else{
         foreach my $ior (@$pars){
             try{
              my $newref=$ref->{orb}->string_to_object($ior->{IOR});
              push @{$ref->{ardref}}, $newref;
             }
             catch CORBA::SystemException with{
               die "stringtoobject  SystemException Error "."\n";
             };

         }
        }
      }
     catch CORBA::SystemException with{
         $ref->sendmailerror("Unable To connect to Server"," ");
#      try to restart server here
       my $failed=-1;
         my $file;
       $ref->{dbfile}=undef;
       if($ref->{CSR}){
         ($failed,$file)=$ref->RestartServer();
       }

       if($failed eq -1){
           $ref->ErrorPlus("Unable To Connect to Server");
       }
       elsif($failed ne 0){
           $ref->ErrorPlus("Attempt to Restart Server failed. Run $file manually");
       }
         else{
             $ref->ErrorPlus("Attempt to Restart Server Has Been Made.\n Please check bjobs -q linux_server -u all in few minutes $file");
         }
      };


    if($#{$ref->{ardref}} >=0){
        my $ard=${$ref->{ardref}}[0];
         my %cid=%{$ref->{cid}};
             try{
               my $dbfile=$ard->getDBFilePath(\%cid);
               
               if(not defined $ref->{dbfile} or $ref->{dbfile} ne $dbfile){
# put another server into dead mode 
                   my $sql="update Servers set status='Dead' where dbfilename !=  '$dbfile'";
                   $ref->{sqlserver}->Update($sql);
                   $ref->{dbfile}=$dbfile;
                   $ref->{IORP}=undef;
               }  
             }
             catch CORBA::SystemException with{
             };
        
}
if( defined $ref->{dbfile}){
     my $dbserver=blessdb();
     $dbserver->{dbfile}=$ref->{dbfile};
     $dbserver->{q}=$ref->{q};
     $ref->{dbserver}=$dbserver;
  if( not defined $ref->{IORP}){
   if(DBServer::InitDBFile($dbserver)){
    foreach my $ac (@{$dbserver->{asl}}){
          my $arsa=$ac->{ars};
          my $createt=$ac->{Start};
        foreach my $ars (@{$arsa}){
          if($ars->{Type} eq "Producer"){
                if($ars->{Interface} eq "default"){
                 $ref->{IORP}=$ars->{IOR};
#                 my $createt=time();
                 my $sql="delete from Servers where dbfilename='$ref->{dbfile}'";
                 $ref->{sqlserver}->Update($sql);
                 $sql="insert into Servers values('$ref->{dbfile}','$ref->{IOR}','$ref->{IORP}','xyz','Active',$ac->{Start},$createt)";
                 $ref->{sqlserver}->Update($sql);
                 last;
                }
            }
      }
        last;
      }
 }
}
}
if (not defined $ref->{IORP}){
    $ref->ErrorPlus("Unable to Connect to DBServer");
}
    return 1;
}


sub RestartServer{
    my $self=shift;
         my $sql="select myvalue from Environment where mykey='amsserver'";
         my $ret=$self->{sqlserver}->Query($sql);
         if(defined $ret->[0][0]){
           my $submit=$ret->[0][0];
           $sql="select dbfilename,lastupdate from Servers where status='Active'";
           $ret=$self->{sqlserver}->Query($sql);
           my $updlast=0;
           foreach my $upd (@{$ret}){
            $sql="update Servers set status='Dead' where dbfilename='$upd->[0]'";
            $self->{sqlserver}->Update($sql);
            if($upd->[1]> $updlast){
             $updlast=$upd->[1];
             $self->{dbfile}=$upd->[0];
           }        
          } 
          if(defined $self->{dbfile}){
              my $full="$self->{UploadsDir}/ServerRestart";
           open(FILE,">".$full) or die "Unable to open file $full \n";
              print FILE "export AMSDataDir=$self->{AMSDataDir} \n";
              print FILE "export AMSProdDir=$self->{AMSProdDir}/../ \n";
             if($self->{sqlserver}->{dbdriver} =~ m/Oracle/){
               print FILE "$submit -o -B$self->{dbfile} \n";
           }
             else{
               print FILE "$submit  -B$self->{dbfile} \n";
             }
           close FILE;
              my $i=system("chmod +x ./$full");
                 $i=system("./$full");
           return $i,$full;
          }
       }
         return 1;
     }

sub ValidateRuns {

   my $self = shift;
#ntuples
   my $validated=0;
   my $thrusted =0;
   my $copied   =0;
   my $failedcp =0;
   my $bad      =0; 
   my $unchecked=0; 
#
   my $warn= undef;
   my $sql = undef;
   my $ret = undef;
   my $vdir= undef;
   my $vlog= undef;
   my $timenow = time();
#

 my $HelpTxt = "
     validateRuns gets list of runs from production server 
                  validates DSTs and copies them to final destination
                  update Runs and NTuples DB tables
     -c    - output will be produced as ASCII page (default)
     -h    - print help
     -i    - prompt before files removal
     -v    - verbose mode
     -w    - output will be produced as HTML page
     ./validateRuns.o.cgi -c -v
";

  $rmprompt = 0;

   foreach my $chop  (@ARGV){
    if ($chop =~/^-c/) {
        $webmode = 0;
    }
    if ($chop =~/^-v/) {
        $verbose = 1;
    }
    if ($chop =~/^-i/) {
     $rmprompt = 1;
    }
    if ($chop =~/^-w/) {
     $webmode = 1;
    }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
   }



    if( not $self->Init()){
        die "ValidateRuns -F- Unable To Init";
        
    }
    if ($verbose && $webmode == 0) {print "ValidateRuns -I- Connect to Server \n";}
    if (not $self->ServerConnect()){
        die "ValidateRuns -F- Unable To Connect To Server";
    }
    if ($verbose && $webmode==0) {print "ValidateRuns -I- Connected \n";}
#
   $self->initFilesProcessingFlag();
#
    if ($verbose && $webmode==0) {print "ValidateRuns -I- Start Validation \n";}
    $vdir=$self->getValidationDir();
    if (not defined $vdir) {
      die " ValidateRuns -F- cannot get path to ValidationDir \n";
     }
    $vlog = $vdir."/validateRuns.log.".$timenow;   

    open(FILEV,">".$vlog) or die "Unable to open file $vlog\n";
    if ($webmode ==0  and $verbose ==1) { print "ValidateRuns -I- open $vlog \n";}
    if ($webmode == 1) { $self->htmlTop();}
#
   $validateStartTime = $timenow;

# get list of runs from DataBase
    if ($webmode ==0  and $verbose ==1) { print "ValidateRuns -I- select list of runs from DB \n";}
    $sql="SELECT run,submit FROM Runs";
    $ret=$self->{sqlserver}->Query($sql);
# get list of runs from Server
    if ($webmode ==0  and $verbose ==1) { print "ValidateRuns -I- get list of runs from server \n";}
    if( not defined $self->{dbserver}->{rtb}){
      DBServer::InitDBFile($self->{dbserver});
    }
    if ($webmode ==0  and $verbose ==1) { print "ValidateRuns -I- set active production set \n";}
    $self->setActiveProductionSet();
    if (not defined $ActiveProductionSet || $ActiveProductionSet eq $UNKNOWN) {
      $self->amsprint("parseJournalFiles -ERROR- cannot get active production set",0);
      die "bye";
   }

   $CheckedRuns[0] = 0;
   $FailedRuns[0]  = 0;  # failed runs
   $GoodRuns[0]    = 0;  # marked as 'Completed'
   $BadRuns[0]     = 0;  # marked as 'Unckecked' or 'Failed'
   $CheckedDSTs[0] = 0;  # dsts checked
   $GoodDSTs[0]    = 0;  # copied to final destination
   $BadDSTs[0]     = 0;  # marked as bad because of different reasons
                       # in particular :
   $BadDSTCopy[0]  = 0;  #                 doCopy failed to copy DST
   $BadCRCo[0]     = 0;  #                 dsts with crc error (after copying)
   $BadCRCi[0]     = 0;  #                 dsts with crc error (before copying)
   $gbDST[0]       = 0;


    foreach my $run (@{$self->{dbserver}->{rtb}}){
 # check flag
     my $rflag = $self->getFilesProcessingFlag();
     if ($rflag == 0) {
         $self->updateFilesProcessing();
         $self->printValidateStat();
         $self->amsprint("Processing flag = 0. Stop Runs Validation.",0);
         return 1;
     } elsif ($rflag ==  -1) {
         $self->printValidateStat();
         $self->amsprint("Processing flag = -1. Stop Runs Validation.",0);
         return 1;
     }
#
     $timenow = time();
     $CheckedRuns[0]++;
     my @cpntuples   =();
     my @mvntuples   =();
     my $runupdate   = "UPDATE runs SET ";
     my $copyfailed  = 0;
# check if run is registered in database
     $sql   = "SELECT run, status FROM runs WHERE run=$run->{Run}";
     my $r0 = $self->{sqlserver}->Query($sql);
     if (not defined $r0->[0][0]) {
      if ($webmode == 1) {
       htmlTable("Insert Runs");
       print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
       print "<td><b><font color=\"blue\">Run </font></b></td>";
       print "<td><b><font color=\"blue\" >FirstEvent</font></b></td>";
       print "<td><b><font color=\"blue\" >LastEvent</font></b></td>";
       print "<td><b><font color=\"blue\" >SubmitTime </font></b></td>";
       print "<td><b><font color=\"blue\" >Status</font></b></td>";
       print "</tr>\n";
      }
      print FILEV "INSERT $run->{Run}, 
                         $run->{Run},
                         $run->{Run},
                         $run->{FirstEvent},
                         $run->{LastEvent},
                         $run->{TFEvent},
                         $run->{TLEvent},
                         $run->{SubmitTime},
                         $run->{Status} \n";
      $self->insertRun(
                       $run->{Run},
                       $run->{Run},
                       $run->{FirstEvent},
                       $run->{LastEvent},
                       $run->{TFEvent},
                       $run->{TLEvent},
                       $run->{SubmitTime},
                       $run->{Status});

#       $sql   = "SELECT run, status FROM runs WHERE run=$run->{Run}";
#       $r0 = $self->{sqlserver}->Query($sql);
       if ($webmode == 1) {
        print "<td><b><font color=\"black\">$run->{Run} </font></b></td>";
        print "<td><b><font color=\"black\" >$run->{FirstEvent}</font></b></td>";
        print "<td><b><font color=\"black\" >$run->{LastEvent}</font></b></td>";
        print "<td><b><font color=\"black\" >$run->{SubmitTime} </font></b></td>";
        print "<td><b><font color=\"black\" >$run->{Status}</font></b></td>";
        print "</tr>\n";
        htmlTableEnd();
       } else {
        print "Run,FirstEvent,LastEvent,Submitted,Status...";
        print "$run->{Run},$run->{FirstEvent},$run->{LastEvent},$run->{SubmitTime},$run->{Status} \n";
       }
  }
#--     if(($run->{Status} eq "Finished" || $run->{Status} eq "Failed") && 
#--     (defined $r0->[0][1] && ($r0->[0][1] ne "Completed" && $r0->[0][1] ne "Unchecked" && $r0->[0][1] ne "TimeOut"))
     if(($run->{Status} eq "Finished") && 
      (defined $r0->[0][1] && ($r0->[0][1] ne "Completed" && $r0->[0][1] ne "TimeOut"))
        ){
# 
        print "Check Run : $run->{Run} Status : $run->{Status}, DB Status : $r0->[0][1] \n";
        print FILEV "Check Run : $run->{Run} Status : $run->{Status} DB Status -> $r0->[0][1] \n";
        my $fevent =  1;
        my $levent =  0;
# check if corresponding job exist
         $sql   = "SELECT runs.status, jobs.content, cites.status  
                    FROM runs,jobs,cites 
                     WHERE jobs.jid=$run->{Run} AND runs.jid=jobs.jid AND cites.cid=jobs.cid";
         my $r1 = $self->{sqlserver}->Query($sql);
         if (not defined $r1->[0][0]) { 
          $sql = "UPDATE runs SET status='Failed' WHERE run=$run->{Run}"; 
          $self->{sqlserver}->Update($sql);
          $warn = "cannot find status, content in Jobs for JID=$run->{Run}. *TRY* Delete Run=$run->{Run} from server \n";
          if ($verbose == 1) {$self->printWarn($warn);}
          print FILEV "$warn \n";
#          DBServer::sendRunEvInfo($self->{dbserver},$run,"Delete"); 
          print "--- done --- \n";
          print FILEV "--- done --- \n";
         } else {
          my $jobstatus  = $r1->[0][0];
          my $jobcontent = $r1->[0][1];
          my $citestatus = $r1->[0][2];
# Jul 25, 2003 ak.
# validate cite 'local' runs on the same way as 
# '-GR' runs
          if ($jobcontent =~ m/-GR/ || $citestatus eq "local") {
#
# remote job
#            update jobinfo first
           my $events  = $run->{cinfo}->{EventsProcessed};
           my $errors  = $run->{cinfo}->{CriticalErrorsFound};
           my $cputime = sprintf("%.2f",$run->{cinfo}->{CPUTimeSpent});
           my $elapsed = sprintf("%.2f",$run->{cinfo}->{TimeSpent});
           my $host    = $self->gethostname($run->{cinfo}->{HostName});
           
           if ($events == 0 && $errors == 0 && $run->{Status} eq 'Finished') {
               if ($webmode == 0 && $verbose == 1) {
                print "Run ... $run->{Run}, Status ... $run->{Status}, Events... $events, Errors... $errors \n";
                print "Set run=$run->{Run} status to Unchecked, Jobs tables not updated \n";
               }
               print FILEV "Run ... $run->{Run}, Status ... $run->{Status}, Events... $events, Errors... $errors \n";
               print FILEV "Set run=$run->{Run} status to Unchecked, Jobs tables not updated \n";
               $sql = "UPDATE runs SET STATUS='Unchecked' WHERE run=$run->{Run}";
               $self->{sqlserver}->Update($sql);
           } else {  # events && errors != 0
# get list of local hosts
            $sql = "UPDATE jobs SET 
                                     EVENTS=$events,
                                     ERRORS=$errors,
                                     CPUTIME=$cputime,
                                     ELAPSED=$elapsed,
                                     HOST='$host',
                                     MIPS=(SELECT mips FROM cern_hosts WHERE  cern_hosts.host LIKE '$host%'),
                                     TIMESTAMP=$timenow  
                            WHERE JID = $run->{Run}";
          $self->{sqlserver}->Update($sql);
          print FILEV "$sql \n";
# validate ntuples
# Find corresponding ntuples from server
             foreach my $ntuple (@{$self->{dbserver}->{dsts}}){
              if(($ntuple->{Status} eq "Success" or $ntuple->{Status} eq "Validated") and $
                  ntuple->{Run}== $run->{Run}){
                  $CheckedDSTs[0]++;
                  $levent += $ntuple->{LastEvent}-$ntuple->{FirstEvent}+1;
                  $ntuple->{Name}=~s/\/\//\//;                  
                  my @fpatha=split ':', $ntuple->{Name};
                  my $fpath=$fpatha[$#fpatha];
                  if ($webmode == 0 and $verbose==1) {print "$fpath \n";}
                  my $suc=open(FILE,"<".$fpath);
                  my $badevents=$ntuple->{ErrorNumber};
                  my $events=$ntuple->{EventNumber};
                  my $status="OK";                     
                  if(not $suc){
                      if($ntuple->{Status} ne "Validated"){
                         $status="Unchecked";                     
                         $events=$ntuple->{EventNumber};
                         $badevents="NULL";
                         $unchecked++;
                         $copyfailed = 1;
                         last;
                      }
                      else{
                        $thrusted++;
                      }
                  }  
                  else{

                   my  $retcrc = $self->calculateCRC($fpath,$ntuple->{crc});
                   print FILEV "calculateCRC($fpath, $ntuple->{crc}) : Status : $retcrc \n";
                   if ($retcrc != 1) {
                       $self->amsprint("********* validateRuns - ERROR- crc status :",666);
                       $self->amsprint($retcrc,0);
                       $BadCRCi[0]++;
                       $BadDSTs[0]++;
                       $bad++;
                   } else { # CRCi - correct
                   close FILE;
                   my ($ret,$i) = 
                       $self->validateDST($fpath,$ntuple->{EventNumber},$ntuple->{Type},$ntuple->{LastEvent});
                   if( ($i == 0xff00) or ($i & 0xff)){
                    if($ntuple->{Status} ne "Validated"){
                     $status="Unchecked";                     
                     $events=$ntuple->{EventNumber};
                     $badevents="NULL";
                     $unchecked++;
                     $copyfailed = 1;
                     last;
                    }
                     else{
                      $thrusted++;
                     }
                   }
                    else{
                         $i=($i>>8);
                         if(int($i/128)){
                          $events=0;
                          $badevents="NULL";
                          $status="Bad".($i-128);  
                          $bad++;                   
                          $levent -= $ntuple->{LastEvent}-$ntuple->{FirstEvent}+1;
                         }
                          else{
                           $status="OK";
                           $events=$ntuple->{EventNumber};
                           $badevents=int($i*$ntuple->{EventNumber}/100);
                           $validated++;
                           my $jobid = $run->{Run};
                           my ($outputpath,$rstatus) = $self->doCopy($jobid,$fpath,$ntuple->{crc});
                           if(defined $outputpath){
                              push @mvntuples, $outputpath; 
                          }
                          if ($rstatus == 1) {
                           $GoodDSTs[0]++;
                           $nBadCopiesInRow = 0;
                           $self->insertNtuple(
                                               $run->{Run},
                                               $ntuple->{Version},
                                               $ntuple->{Type},
                                               $run->{Run},
                                               $ntuple->{FirstEvent},
                                               $ntuple->{LastEvent},
                                               $events,
                                               $badevents,
                                               $ntuple->{Insert},
                                               $ntuple->{size},
                                               $status,
                                               $outputpath,
                                               $ntuple->{crc},
                                               $ntuple->{Insert},
                                               1,0);
                           print FILEV "insert : $run->{Run}, $outputpath, $status, $ntuple->{size} \n";
                           $copied++;
                           $gbDST[0] = $gbDST[0] + $ntuple->{size};
                           push @cpntuples, $fpath;
                           $runupdate = "UPDATE runs SET FEVENT = $fevent, LEVENT=$levent, ";
                         } else {
                           $BadDSTs[0]++;
                           $BadDSTCopy[0]++;
                           print FILEV "failed to copy or wrong CRC for : $fpath\n";
#--                           $copyfailed = 1;
                           $nBadCopiesInRow++;
                           if ($nBadCopiesInRow > $MAX_FAILED_COPIES) {
                            $self->amsprint("Too many doCopy failures : $nBadCopiesInRow. Quit",0);
                            $self->amsprint("Check $outputpath  free blocks and availability. Quit",0);
                            print FILEV "Too many doCopy failures : $nBadCopiesInRow. \n";
                            print FILEV "Check $outputpath. Quit\n";
                            close FILEV;
                            die "Bye";
                           }
                           $levent -= $ntuple->{LastEvent}-$ntuple->{FirstEvent}+1;
                           $bad++;
                           if ($outputpath) {
                               my $cmd = "rm $outputpath";
                               if ($rmprompt == 1) {
                                $cmd = "rm -i $outputpath";
                               }
                               my $rstat = system($cmd);
                               print FILEV "Remove bad file $cmd \n";
                           }
                         }
                       }  # ntuple status 'OK'
                     }
               } # passed CRCi
              } 
             } # ntuple ->{Status} == "Validated"
         } #loop for ntuples
         my $status='Failed';
         if ($copyfailed == 0) {
          $warn = "Validation done : *TRY* send Delete to DBServer, Run =$run->{Run} \n";
          print FILEV "$warn \n";
          $self->printWarn($warn);
#--          DBServer::sendRunEvInfo($self->{dbserver},$run,"Delete"); 
          foreach my $ntuple (@cpntuples) {
           my $cmd="rm $ntuple";
           if ($rmprompt == 1) {$cmd = "rm -i $ntuple";}
           system($cmd);
           $warn = "Validation done : $cmd \n";
           print FILEV $warn;
           $self->printWarn($warn);
         }
          if ($#cpntuples >= 0) { $status = "Completed";}
      }
           else{

               $warn="Validation/copy failed : Run =$run->{Run} \n";
               print FILEV $warn;
               $self->printWarn($warn);

               $status='Unchecked';
               foreach my $ntuple (@mvntuples) {
                my $cmd = "rm $ntuple";
                if ($rmprompt == 1) {$cmd = "rm -i $ntuple";}

                $warn="Validation failed : $cmd \n";
                print FILEV $warn;
                $self->printWarn($warn);

                system($cmd);
                $failedcp++;
                $copied--;
               }
               $sql = "DELETE ntuples WHERE run=$run->{Run}";
               $self->{sqlserver}->Update($sql);
               $runupdate = "UPDATE runs SET ";
           }

           if ($status eq "Completed") {
               $GoodRuns[0]++;
           } elsif ($status eq "Failed") {
               $BadRuns[0]++;
           }

          $sql = $runupdate." STATUS='$status' WHERE run=$run->{Run}";
          $self->{sqlserver}->Update($sql);
          $warn = "Update Runs : $sql\n";
          print FILEV $warn;
           if ($webmode == 1) {
              htmlWarning("validateRuns",$warn);
          } else {
           $self->printWarn($warn);
          }
          if ($status eq "Completed") {
           $self->updateRunCatalog($run->{Run});
           $warn = "Update RunCatalog table : $run->{Run}\n";
           print FILEV $warn;
          }
      }# events != 0 && errors != 0
     } # remote job             
    }  # job found
   }   # run->{Status} == 'Finished' || 'Failed'
  }    # loop for runs from 'server'
      
  if ($webmode == 1) {
   $self->htmlBottom();
  }
  $warn = "$validated Ntuple(s) Successfully Validated.
                   \n $copied Ntuple(s) Copied.
                   \n $failedcp Ntuple(s) Not Copied.
                   \n. $bad Ntuple(s) Turned Bad.
                   \n $unchecked Ntuples(s) Could Not Be Validated.
                   \n $thrusted Ntuples Could Not be Validated But Assumed  OK.
                   \n";
 print FILEV $warn;
 close FILEV;
 print "$warn \n";
 $self->printValidateStat();
 if ($webmode == 1) {$self->InfoPlus($warn);}
 $self->{ok}=1;

}

sub doCopy {
     my $self = shift;
     my $jid  = shift;
     my $inputfile = shift;
     my $crc       = shift;
# 
     my $sql   = undef;
     my $cmd   = undef;
     my $cmdstatus = undef;
#
     my $time0  = time();
     my $time00 = 0;      # begin CRC calc

     $doCopyCalls++;

#
#     if ($verbose == 1) {print "doCopy : $inputfile \n";}
#

     my @junk = split '/',$inputfile;
     my $file = $junk[$#junk];

     my $outputdisk => undef;
     my $outputpath => undef;

     my $filesize    = (stat($inputfile))[7];
     if ($filesize > 0) {
# get output disk
      my ($outputpath, $gb) = $self->getOutputPath();
         if ($outputpath =~ 'xyz' || $gb == 0) {
             if ($webmode == 0) {
              print "doCopy -E- Cannot find any disk to store DSTs. Exit";
              die;
             }
         } else {
# find job
          $sql = "SELECT cites.name,jobname  FROM jobs,cites 
                           WHERE jid =$jid AND cites.cid=jobs.cid";
          my $r1 = $self->{sqlserver}->Query($sql);
          if (defined $r1->[0][0]) {
            my $cite    = $r1->[0][0];
            my $jobname = $r1->[0][1];
            my $dataset = "unknown";
            $sql = "SELECT jobs.jid, datasets.name  FROM datasets, jobs   
                           WHERE jid =$jid AND jobs.did=datasets.did";
            my $r2 = $self->{sqlserver}->Query($sql);
            if (defined $r2->[0][0]) {
              $dataset = $r2->[0][1];
              $jobname =~ s/$cite.//;
              $jobname =~ s/$jid.//;
              $jobname =~ s/.job//;
              $outputpath = $outputpath."/".$dataset."/".$jobname;
              $cmd = "mkdir -p $outputpath";
              $cmdstatus = system($cmd);
              if ($cmdstatus == 0) {
               $outputpath = $outputpath."/".$file;
               $cmdstatus = $self->copyFile($inputfile,$outputpath);
               $time00 = time();
               $doCopyTime += $time00 - $time0;
               if ($cmdstatus == 0 ) {
                 my $rstatus = $self->calculateCRC($outputpath,$crc);
                 if ($rstatus == 1) {
                   return $outputpath,1;
                 } else {
                  $self->amsprint("********* docopy - ERROR- crc status :",666);
                  $self->amsprint($rstatus,0);
                  $BadCRCo[$nCheckedCite]++;
                  if ($webmode == 1) {
                   htmlWarning("doCopy","calculateCRC($outputpath,$crc)");
                   htmlWarning("doCopy","crc calculation failed for $outputpath");
                   htmlWarning("doCopy","crc calculation failed status $rstatus");
                  }
                  return $outputpath,0;
                 }
               }
               $BadDSTCopy[$nCheckedCite]++;
               $self->amsprint("********* docopy - cmd status : ",666);
               $self->amsprint($cmdstatus,0);
               return $outputpath,0;
              } else {
                  if ($webmode == 1) {htmlWarning("doCopy","failed $cmd");}
                  $BadDSTCopy[$nCheckedCite]++;
             }
            } else {
                if ($webmode == 1) {htmlWarning("doCopy","cannot find dataset for JID=$jid");}
            }
           } else {
              if ($webmode == 1) {htmlWarning("doCopy","cannot get info for JID=$jid");}
           }
        }
  } else {
      if ($webmode == 1) {htmlWarning("doCopy","cannot stat $inputfile");}
      $BadDSTs[$nCheckedCite]++;
  } 

  return undef,0;
}


sub Validate{
   my $self = shift;
   my $status = undef;

    if($self->{q}->param("Control")){
        my $pass=$self->{q}->param("password");
        my $crypt=crypt($pass,"ams");
        if($crypt ne "amGzkSRlnSMUU"){
                  Warning::error($self->{q},
                  "User authorization failed");
                   return;

                }
    }
       else{
         print $self->{q}->header( "text/html" ),
         $self->{q}->start_html( "Welcome");
         print $self->{q}->h1( "Welcome to the AMS02 Validation Form" );
         print $self->{q}->start_form(-method=>"GET", 
          -action=>$self->{Name});
   print qq`
Password: <INPUT TYPE="password" NAME="password" VALUE="" ><BR>
`;
        print $self->{q}->submit(-name=>"Control", -value=>"SubmitRequest");
        print $self->{q}->end_form();
   return;
     }
   my $validated=0;
   my $thrusted=0;
   my $bad=0; 
   my $unchecked=0; 
    if( not $self->Init()){
        die "Unable To Init";
        
    }
    if (not $self->ServerConnect()){
        die "Unable To Connect To Server";
    }
    my $sql="select run,submit from Runs";
    my $ret=$self->{sqlserver}->Query($sql);
   if( not defined $self->{dbserver}->{rtb}){
     DBServer::InitDBFile($self->{dbserver});
   }

   $self->htmlTop();

   my $vdir = undef;
   my $vlog = undef;
   $vdir=$self->getValidationDir();
   if (not defined $vdir) {
     die " Validate -F- cannot get path to ValidationDir \n";
   }
   my $timenow = time();
   $vlog = $vdir."/validateDST.log.".$timenow;   

   open(LOGFILE,">".$vlog) or die "Unable to open file $vlog\n";


    foreach my $run (@{$self->{dbserver}->{rtb}}){
     if($run->{Status} eq "Finished"){
# check if it is in the database
         my $found=0;
         foreach my $rdb (@{$ret}){
             if($rdb->[0] == $run->{Run} and $rdb->[1]==$run->{SubmitTime}){
                 $found=1;
                 last;
             }
         }
         $status = $run->{Status};
         if($found != 1){
             print LOGFILE "insert Run : $run-{Run} into Runs \n";
             print "insert Run : $run-{Run} into Runs \n";
             $self->insertRun(
                    $run->{Run},
                    $run->{Run},
                    $run->{FirstEvent},
                    $run->{LastEvent},
                    $run->{TFEvent},
                    $run->{TLEvent},
                    $run->{SubmitTime},
                    $status);
         }
# Find corresponding ntuples
          foreach my $ntuple (@{$self->{dbserver}->{dsts}}){
              if($ntuple->{Type} eq "Ntuple" and 
                ($ntuple->{Status} eq "Success" or $ntuple->{Status} eq "Validated") and 
                 $ntuple->{Run}== $run->{Run}){
# suppress double //
                  $ntuple->{Name}=~s/\/\//\//;                  
                  my @fpatha=split ':', $ntuple->{Name};
                  my $fpath=$fpatha[$#fpatha];
                  my $suc=open(FILE,"<".$fpath);
                  my $badevents=$ntuple->{ErrorNumber};
                  my $events=$ntuple->{EventNumber};
                  $status="OK";                     
                  if(not $suc){
                      if($ntuple->{Status} ne "Validated"){
                         $status="Unchecked";                     
                         $events=$ntuple->{EventNumber};
                         $badevents="NULL";
                         $unchecked++;
                      }
                      else{
                        $thrusted++;
                      }
                  }  
                  else{
                      close FILE;
                      my ($ret,$i) = $self->validateDST($fpath,$ntuple->{EventNumber},$ntuple->{Type},$ntuple->{LastEvent});
                      if( ($i == 0xff00) or ($i & 0xff)){
                      if($ntuple->{Status} ne "Validated"){
                       $status="Unchecked";                     
                       $events=$ntuple->{EventNumber};
                       $badevents="NULL";
                       $unchecked++;
                      }
                      else{
                        $thrusted++;
                      }
                  }
                      else{
                          $i=($i>>8);
                          if(int($i/128)){
                            $events=0;
                            $badevents="NULL";
                            $status="Bad".($i-128);  
                            $bad++;                   
                          }
                          else{
                           $status="OK";
                           $events=$ntuple->{EventNumber};
                           $badevents=int($i*$ntuple->{EventNumber}/100);
                           $validated++;
                          }
                      }
                  }

                  print "Validate : insert ntuple : $run->{Run}, $ntuple->{Name}\n";
                  $self->insertNtuple(
                               $run->{Run},
                               $ntuple->{Version},
                               $ntuple->{Type},
                               $run->{Run},
                               $ntuple->{FirstEvent},
                               $ntuple->{LastEvent},
                               $events,
                               $badevents,
                               $ntuple->{Insert},
                               $ntuple->{size},
                               $status,
                               $ntuple->{Name},
                               $ntuple->{crc});
              }
          }
        }
         else{
#        find all 'Unchecked' ntuples
             $sql="select path from Ntuples where status='Unchecked' and run=$run->{Run}";
             my $rts=$self->{sqlserver}->Query($sql);
             if(defined $rts->[0][0]){
                 foreach my $uc (@{$rts}){
                     my $path=$uc->[0];
                     foreach my $ntuple (@{$self->{dbserver}->{dsts}}){
                     if($ntuple->{Type} eq "Ntuple" and 
                        $ntuple->{Status} eq "Success" and $ntuple->{Run}== $run->{Run} ){
                     $ntuple->{Name}=~s/\/\//\//;                  
                     if($path eq $ntuple->{Name}){
                     my @fpatha=split ':', $ntuple->{Name};
                     my $fpath=$fpatha[$#fpatha];
                     my $suc=open(FILE,"<".$fpath);
                     my ($events,$badevents);
                     if(not $suc){
                         $unchecked++;
                     }  
                     else{
                      close FILE;
                      my ($ret,$i) = $self->validateDST($fpath,$ntuple->{EventNumber},$ntuple->{Type},$ntuple->{LastEvent});
                      if( ($i == 0xff00) or ($i & 0xff)){
                      }
                      else{
                          my $status;
                          $i=($i>>8);
                          if(int($i/128)){
                            $events=0;
                            $badevents="NULL";
                            $status="Bad".($i-128);  
                            $bad++;                   
                          }
                          else{
                           $status="OK";
                           $events=$ntuple->{EventNumber};
                           $badevents=int($i*$ntuple->{EventNumber}/100);
                           $validated++;
                          }
                          $sql="update Ntuples set status='$status', NEventsErr=$badevents where path='$path' and run=$ntuple->{Run}";
                          $self->{sqlserver}->Update($sql);
                      }
                     }



                     }                     
                     }
                     }
                 }
             }
         }
     }

   close LOGFILE;

   $self->InfoPlus("$validated Ntuple(s) Successfully Validated.\n $bad Ntuple(s) Turned Bad.\n $unchecked Ntuples(s) Could Not Be Validated.\n $thrusted Ntuples Could Not be Validated But Assumed  OK.");

  $self->htmlBottom();

   $self->{ok}=1;
}

sub ConnectOnlyDB{
    my $self = shift;
#sqlserver
    $self->{sqlserver}=new DBSQLServer();
    $self->{sqlserver}->Connect();
#
   my $dir=$ENV{AMSDataDir};
   if (defined $dir){
     $self->{AMSDataDir}=$dir;
   }
    else{
     $self->{AMSDataDir}="/afs/ams.cern.ch/AMSDataDir";
     $ENV{AMSDataDir}=$self->{AMSDataDir};
    }
     if (not defined $self->{AMSSoftwareDir}) {
         $dir =  $self->getDefByKey("AMSSoftwareDir");
         if (defined $dir) {
          $self->{AMSSoftwareDir}="$self->{AMSDataDir}/".$dir;
         } else {
             die "Cannot get def for key : AMSSoftwareDir \n";
         }
     }

    $self->setActiveProductionSet();
}

sub ConnectDB{

    my $self = shift;
    if( not $self->Init()){
        return 0;
    }
    else{
        $self->{ok}=1;
    }
    $self->setActiveProductionSet();
} 

sub Connect{

    my $self = shift;
    if( not $self->Init()){
        return 0;
    }
    else{
        $self->{ok}=1;
    }
 ; 

#
 my    $insertjobsql = undef;
 my $dbversion       = undef;
#
#understand parameters

        $self->{read}=0;
    my $q=$self->{q};
    my $sql=>undef;
    my $color;
    my $cite = 'Any';
# db query
    if ($self->{q}->param("getJobID")) {
     $self->{read}=1;
     if ($self->{q}->param("getJobID") eq "Submit") {
        htmlTop();
        my $title = "Jobs : ";
        my $jobid = 0;
        my $jobmin= 0;
        my $jobmax= 0;
        if (defined $q->param("JobID")) {
            if ($q->param("JobID") =~ /-/) {
                ($jobmin,$jobmax) = split '-',$q->param("JobID");
                $title = $title.$q->param("JobID");
                $sql = "SELECT jobname, triggers, host, events, errors, cputime, 
                               elapsed, cites.name, jobs.did, jobs.timestamp, jobs.jid   
                          FROM jobs, cites 
                          WHERE jobs.jid>$jobmin AND jobs.jid<$jobmax 
                                AND jobs.cid=cites.cid ";
                if ($q->param("QCite")) {
                 $cite = trimblanks($q->param("QCite"));
                 if ($cite ne 'Any') {
                  $sql = $sql." AND cites.name = '$cite' ";
                  $title=$title." for Cite : $cite";
                 }
                }
                $sql = $sql." ORDER BY jobs.jid DESC";
            } else {
             $jobid =  trimblanks($q->param("JobID"));
             if ($jobid > 0) {
              $title = $title.$jobid;
              $sql = "SELECT jobname, triggers , host, events, errors, cputime, 
                            elapsed, cites.name, content, jobs.timestamp, jobs.jid 
                          FROM jobs, cites 
                          WHERE jobs.jid=$jobid AND jobs.cid=cites.cid ORDER BY jobs.jid DESC";
             } else {
                 goto CheckCite;
             }
          }
        } else {
CheckCite:            if (defined $q->param("QCite")) {
             $cite = trimblanks($q->param("QCite"));
             if ($cite ne 'Any') {
              $sql = "SELECT jobname, triggers , host, events, errors, cputime, 
                            elapsed, cites.name, content, jobs.timestamp, jobs.jid 
                          FROM jobs, cites 
                          WHERE cites.name='$cite' AND jobs.cid=cites.cid 
                          ORDER BY jobs.jid DESC";
              $title=$title." for Cite : $cite";
             }
         }
        }
        my $content = " ";
        $self->htmlTemplateTable($title);
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<td align=left><b><font color=\"blue\">JID </font></b></td>";
               print "<td align=center><b><font color=\"blue\">Name </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Triggers</font></b></td>";
        if ($cite eq 'Any') {
               print "<td align=center><b><font color=\"blue\" >Cite </font></b></td>";
        }
               print "<td align=center><b><font color=\"blue\" >Host</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Events </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Errors </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >CPU  </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Elapsed </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Status </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Timestamp </font></b></td>";
              print "</tr>\n";
        
        my $ret=$self->{sqlserver}->Query($sql);
        if (defined $ret->[0][0]) {
         foreach my $r (@{$ret}){
             my $jobname =trimblanks($r->[0]);
             my $triggers=$r->[1];
             my $host    =trimblanks($r->[2]);
             
             my $events  =$r->[3];
             my $errors  =$r->[4];
             my $cputime =$r->[5];
             my $elapsed =$r->[6];
             my $cite    =trimblanks($r->[7]);
             $content    =$r->[8];
             my $timestamp=$r->[9];
             my $jid      =$r->[10];
             my $status = "Submitted";
             $sql = "SELECT status, submit FROM runs WHERE jid=$jid";
             $ret=$self->{sqlserver}->Query($sql);
             if (defined $ret->[0][0]) {
              foreach my $r (@{$ret}){
                  $status    = trimblanks($r->[0]);
                  $timestamp = $r->[1];
              }
             }
             $color="black";
             if ($status eq 'Finished' or $status eq 'Completed') {
                 $color = "green";
             } elsif ($status eq 'Failed' or $status eq 'Unchecked') {
                 $color = "red";
             } elsif ($status eq 'Processing') {
                 $color = "blue";
             }
              print "<td align=left><b><font color=$color> $jid </font></td></b>";
              print "<td align=center><b><font color=$color> $jobname </font></td></b>";
              print "<td align=center><b><font color=$color> $triggers </font></b></td>";
              if ($cite eq 'Any') {
               print "<td align=center><b><font color=$color> $cite </font></td></b>";
              }
              print "<td align=center><b><font color=$color> $host </font></td></b>";
              print "<td align=center><b><font color=$color> $events </font></b></td>";
              print "<td align=center><b><font color=$color> $errors </font></b></td>";
              print "<td align=center><b><font color=$color> $cputime </font></b></td>";
              print "<td align=center><b><font color=$color> $elapsed </font></b></td>";
              print "<td align=center><b><font color=$color> $status </font></b></td>";
              print "<td align=center><b><font color=$color> $timestamp </font></b></td>\n";
              print "</tr>\n";
         }
       } 
       htmlTableEnd();
        if ($jobid > 0) {
          print "<p></p>\n";
          print $q->textarea(-name=>"CCA",-default=>"$content",-rows=>30,-columns=>80);
# here run info
        $sql = "SELECT run, fevent, levent, submit FROM runs WHERE run=$jobid";
        $ret=$self->{sqlserver}->Query($sql);
        if (defined $ret->[0][0]) {
              my $run = $ret->[0][0];
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<td align=center><b><font color=\"blue\">Run </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >First Event</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Last Event </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Submit Time</font></b></td>";
               print "</tr>\n";
               print "<td align=center><b><font color=\"black\">$ret->[0][0] </font></b></td>";
               print "<td align=center><b><font color=\"black\" >$ret->[0][1]</font></b></td>";
               print "<td align=center><b><font color=\"black\" >$ret->[0][2] </font></b></td>";
               my $stime = EpochToDDMMYYHHMMSS($ret->[0][3]);
               print "<td align=center><b><font color=\"black\" >$stime</font></b></td>";
               print "</tr>\n";
              htmlTableEnd();
              $sql = "SELECT path, nevents, neventserr, sizemb, status FROM ntuples 
                       WHERE  run = $run";
              my $r0=$self->{sqlserver}->Query($sql);
              if (defined $r0->[0][0]) {
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
                print "<td align=center><b><font color=\"blue\">DST Path </font></b></td>";
                print "<td align=center><b><font color=\"blue\" >Events</font></b></td>";
                print "<td align=center><b><font color=\"blue\" >Errors </font></b></td>";
                print "<td align=center><b><font color=\"blue\" >MBytes</font></b></td>";
                print "<td align=center><b><font color=\"blue\" >Status</font></b></td>";
                print "</tr>\n";
               foreach my $nt (@{$r0}) {
                print "<td align=left><b><font color=\"black\">$nt->[0] </font></b></td>";
                print "<td align=center><b><font color=\"black\" >$nt->[1]</font></b></td>";
                print "<td align=center><b><font color=\"black\" >$nt->[2] </font></b></td>";
                print "<td align=center><b><font color=\"black\" >$nt->[3]</font></b></td>";
                print "<td align=center><b><font color=\"black\" >$nt->[4]</font></b></td>";
                print "</tr>\n";
               }
               htmlTableEnd();
              } else {
               htmlText("No ntuples found for Run=",$run);
              }
          } else {
             htmlText("No run found for Job=",$jobid);
           }
        htmlBottom();
      }
    }
   }
# getJobId ends here
#
    if ($self->{q}->param("getRunID")) {
     $self->{read}=1;
     if ($self->{q}->param("getRunID") eq "Submit") {
        htmlTop();
        my $title = "Runs List ";
        my $runid = 0;
        my $runmin= 0;
        my $runmax= 0;
        if (defined $q->param("RunID")) {
         $self->htmlTemplateTable($title);
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<td align=center><b><font color=\"blue\">Run </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Job</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >fEvent </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >lEvent</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Submitted </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Status </font></b></td>";
               print "</tr>\n";
            if ($q->param("RunID") =~ /-/) {
                ($runmin,$runmax) = split '-',$q->param("RunID");
                $title = $title.$q->param("RunID");
                $sql = "SELECT run, jid, fevent, levent, submit, status FROM Runs 
                          WHERE run>$runmin AND run<$runmax 
                          ORDER BY run";
            } else {
             $runid =  trimblanks($q->param("RunID"));
             $title = $title.$runid;
              $sql = "SELECT run, jid, fevent, levent, submit, status FROM Runs 
                          WHERE run=$runid"; 
            }
         my $ret=$self->{sqlserver}->Query($sql);
         if (defined $ret->[0][0]) {
          foreach my $r (@{$ret}){
             my $run    = $r->[0];
             my $jid    = $r->[1];
             my $fevent = $r->[2];
             my $levent = $r->[3];
             my $submit  =$r->[4];
             my $status  = trimblanks($r->[5]);
             $color="black";
             if ($status eq 'Finished' || $status eq 'Completed') {
                 $color = "green";
             } elsif ($status eq 'Failed' || $status eq 'Unchecked' || $status eq 'TimeOut')  {
                 $color = "red";
             }
           print "
                  <td align=left><b><font color=$color> $run </font></td></b>
                  <td align=left><b><font color=$color> $jid </font></td></b>
                  <td align=center><b><font color=$color> $fevent </font></b></td>
                  <td align=center><b><font color=$color> $levent </font></td></b>
                  <td align=center><b><font color=$color> $submit </font></td></b>
                  <td align=center><b><font color=$color> $status </font></b></td>\n";
              print "</tr>\n";

         }
      }
       htmlTableEnd();
      htmlTableEnd();
     }
    htmlBottom();
    }
 }
# getRunID end here
#
    if ($self->{q}->param("getDSTID")) {
     $self->{read}=1;
     if ($self->{q}->param("getDSTID") eq "Submit") {
        htmlTop();
        my $title = "DSTs List ";
        my $runid = 0;
        my $runmin= 0;
        my $runmax= 0;
        if (defined $q->param("DSTID")) {
         $self->htmlTemplateTable($title);
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<td align=center><b><font color=\"blue\">Job </font></b></td>";
               print "<td align=center><b><font color=\"blue\">Run </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Updated</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >FilePath </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Events</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Errors </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Status </font></b></td>";
               print "</tr>\n";
            if ($q->param("DSTID") =~ /-/) {
                ($runmin,$runmax) = split '-',$q->param("DSTID");
                $title = $title.$q->param("RunID");
                $sql = "SELECT jid, run, timestamp, nevents, neventserr, status 
                          FROM ntuples  
                          WHERE run>$runmin AND run<$runmax 
                          ORDER BY run";
            } else {
             $runid =  trimblanks($q->param("DSTID"));
             $title = $title.$runid;
             $sql = "SELECT jid, run, path, timestamp, nevents, neventserr, status 
                      FROM ntuples  WHERE run=$runid"; 
            }
         my $ret=$self->{sqlserver}->Query($sql);
         if (defined $ret->[0][0]) {
          foreach my $r (@{$ret}){
             my $jid       = $r->[0];
             my $run       = $r->[1];
             my $path      = trimblanks($r->[2]);
             my $starttime = EpochToDDMMYYHHMMSS($r->[3]); 
             my $nevents   = $r->[4];
             my $nerrors   = $r->[5];
             my $status    = trimblanks($r->[6]);
             my $color     = statusColor($status);
             print "<td><b> $jid </td></b>
                    <td><b> $run </td>
                    <td><b> $starttime </b></td>
                    <td><b> $path </b></td> 
                    <td align=middle><b> $nevents </b></td> 
                    <td align=middle><b> $nerrors </b></td> 
                    <td align=middle><b><font color=$color> $status </font></b></td> \n";
             print "</font></tr>\n";
         }
      }
       htmlTableEnd();
      htmlTableEnd();
     }
    htmlBottom();
    }
 }
# getDSTID end here
#
#queryDB04
   if ($self->{q}->param("queryDB04")) {
     $self->{read}=1;

     my @tempnam=();
     my $hash={};
     my @desc=();
     my $cite={};
     my @runs=();
     my @submits=();
     my @jobnames=();
     my @datasets=();

     my $qtemplate  = undef;
     my $qparticle  = undef;
     my $qmomentumI = undef;
     my $qmomentumA = undef;
     my $qtrigger   = undef;

    if ($self->{q}->param("queryDB04") eq "DoQuery") {

      my $sql=undef;
      my $dataset=undef;
      my $particle=undef;
#
# Template is defined explicitly
#
      if (defined $q->param("QTempDataset") and $q->param("QTempDataset") ne "Any") {
       $dataset = $q->param("QTempDataset");
       $dataset = trimblanks($dataset);
       $qtemplate = $dataset;
       $dataset =~ s/ /\% /g;
       $sql = "SELECT runs.run, jobs.jobname, runs.submit FROM runs, jobs, runcatalog  
                   WHERE runs.jid=jobs.jid AND 
                        (runcatalog.jobname LIKE '%$dataset%' AND runcatalog.run=runs.run) AND 
                        runs.status='Completed'";
#       htmlTop();
#       print "$sql \n";
#       htmlBottom();
# check TriggerType
       if (defined $q->param("QTrType") and $q->param("QTrType") ne "Any") {
           my $trtype = trimblanks($q->param("QTrType"));
           $qtrigger = $trtype;
           $sql=$sql." AND (runs.run = runcatalog.run AND runcatalog.trtype='$trtype') ";
       }
#
       $sql = $sql."ORDER BY Runs.Run";
       my $r1=$self->{sqlserver}->Query($sql);
        if (defined $r1->[0][0]) {
         foreach my $r (@{$r1}){
               push @runs,$r->[0];
               push @jobnames,$r->[1];
               push @submits,$r->[2];
         }
        }
#
# Template 'Any', particle (dataset) is defined
#
      } elsif (defined $q->param("QPart") and 
                   ($q->param("QPart") ne "Any" and 
                    $q->param("QPart") ne "ANY" and $q->param("QPart") ne "any"))  {
         $particle = $q->param("QPart");
         $qparticle = $particle;
         $sql = " SELECT DID FROM Datasets WHERE NAME LIKE '$particle%'";
         my $r0=$self->{sqlserver}->Query($sql);
         if (defined $r0->[0][0]) {
          foreach my $r (@{$r0}){
           my $did = $r->[0];
           $sql = "SELECT Runs.Run, Jobs.JOBNAME, Runs.SUBMIT 
                    FROM Runs, Jobs, runcatalog  
                     WHERE Jobs.DID=$did AND Jobs.JID=Runs.JID AND 
                            Runs.run=runcatalog.run AND Runs.Status='Completed'";
# check TriggerType
           if (defined $q->param("QTrType") and $q->param("QTrType") ne "Any") {
            my $trtype = trimblanks($q->param("QTrType"));
            $qtrigger = $trtype;
            $sql=$sql." AND (runs.run = runcatalog.run AND runcatalog.trtype='$trtype') ";
           }
# check Momentum
           if (defined $q->param("QMomI") and defined $q->param("QMomA")) {
               $qmomentumI = $q->param("QMomI");
               $qmomentumA = $q->param("QMomA");
               if ($q->param("QMomI") == 1 and $q->param("QMomA") == 10000) {
#              do nothing - defaults
               } else {
                 my $momentumMin = $q->param("QMomI");
                 my $momentumMax = $q->param("QMomA");
                 $sql = $sql." AND (runs.run = runcatalog.run AND PMIN >= $momentumMin AND PMAX <= $momentumMax) ";
               }
           }
#
            $sql = $sql." ORDER BY Runs.Run";
            my $r1=$self->{sqlserver}->Query($sql);
            if (defined $r1->[0][0]) {
             foreach my $r (@{$r1}){
               push @runs,$r->[0];
               push @jobnames,$r->[1];
               push @submits,$r->[2];
              }
             }
            }
      }
     } else {
        $sql = "SELECT Runs.RUN, Jobs.JOBNAME, Runs.SUBMIT 
                    FROM Runs, Jobs, runcatalog  
                     WHERE Runs.JID=Jobs.JID AND Runs.Status='Completed' and Runs.run = runcatalog.run ";
# check TriggerType
           if (defined $q->param("QTrType") and $q->param("QTrType") ne "Any") {
            my $trtype = trimblanks($q->param("QTrType"));
            $sql=$sql." AND (runs.run = runcatalog.run AND runcatalog.trtype='$trtype') ";
           }
# check Momentum
           if (defined $q->param("QMomI") and defined $q->param("QMomA")) {
               if ($q->param("QMomI") == 1 and $q->param("QMomA") == 10000) {
#              do nothing - defaults
               } else {
                 my $momentumMin = $q->param("QMomI");
                 my $momentumMax = $q->param("QMomA");
                 $sql = $sql." AND (runs.run = runcatalog.run AND PMIN >= $momentumMin AND PMAX <= $momentumMax) ";
               }
           }
#
            $sql = $sql." ORDER BY Runs.Run";
            my $r1=$self->{sqlserver}->Query($sql);
            if (defined $r1->[0][0]) {
             foreach my $r (@{$r1}){
               push @runs,$r->[0];
               push @jobnames,$r->[1];
               push @submits,$r->[2];
              }
             }
    }
# now check output
      my $accessmode = "xyz";
      my $remotecite = "xyz";
      if (defined $q->param("REMOTEACCESS")) {
          $accessmode="REMOTE";
          $remotecite = $q->param("REMOTEACCESS");
      }
      if (defined $q->param("NTOUT")) {
       htmlTop();
        $self->htmlTemplateTable("Selected Query Keys :");
        if (defined $qparticle) {
         print "<tr><td><b><font size=\"4\" color=\"tomato\"> Particle/dataset :";
         print "</font></b></td>";
         print " <td><b><font size=\"4\"> $qparticle </font></b></td></tr>";
       }
       if (defined $qtemplate) {
        my @description = split /Total/,$qtemplate;
        print "<tr><td><b><font size=\"4\" color=\"green\"> Template :";
        print "</font></b></td>";
        print "<td><b><font size=\"4\">$description[0] </font></b></td></tr>\n";
       } else {
        if (defined $qmomentumI and defined $qmomentumA) {
         print "<tr><td><b><font size=\"4\" color=\"green\">Momentum ";
         print "</font></b></td>";
         print "<td><b><font size=\"4\">$qmomentumI ... $qmomentumA [GeV/c]";
         print "</font></b></td></tr> \n";
        }
       }
       if (defined $qtrigger) {
        print "<tr><td><b><font size=\"4\" color=\"green\">Trigger Type ";
        print "</font></b></td>";
        print "<td><b><font size=\"4\">$qtrigger </font></b></td></tr>\n";
       }
       if ($accessmode eq "REMOTE" && $q->param("NTOUT") ne "SUMM") {
        print "<tr><td><b><font size=\"4\" color=\"blue\">Print ONLY Files copied to ";
        print "</font></b></td>";
        print "<td><b><font size=\"4\" color=\"red\">$remotecite </font></b></td></tr>\n";
       }
      htmlTableEnd();
      print "<BR><BR>\n";
       if (defined $#runs) {
           my $nruns = $#runs+1;
           print "<LI><font size=5 color=tomato><i> Total Runs Found in Database ... </font></i> $nruns </li>\n";
           print "<BR><BR>\n";
       }
# ....print 'ALL' information
       if ($q->param("NTOUT") eq "ALL") {
              my $i = 0;
              foreach my $run (@runs){
               my $printit = 1;
               if ($accessmode eq "REMOTE") {
                $sql = "SELECT run FROM MC_DST_COPY WHERE run=$run AND cite='$remotecite'";
                my $r0=$self->{sqlserver}->Query($sql);
                if (not defined $r0->[0]) {
                 $printit = 0;
                }
               }
               if ($printit == 1) {
                my $jobname = $jobnames[$i];
                my $submit  = $submits[$i];
                $i++;
                print "<tr><td><b><font size=\"3\" color=$color> Job : $jobname, Run : $run, Submitted : $submit";
                print "</font></b></td></tr>\n";
                print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
                print "<table border=1 width=\"100%\" cellpadding=0 cellspacing=0>\n";
                print "<tr><td width=10% align=left><b><font color=\"blue\" > NTuple </font></b></td>";
                print "<td width=10%><b><font color=\"blue\"> Events </font></b></td>";
                print "<td width=15%><b><font color=\"blue\" > Errors </font></b></td>";
                print "<td width=15%><b><font color=\"blue\" > Size[MB] </font></b></td>";
                print "<td td align=middle><b><font color=\"blue\" > Produced </font></b></td>";
                print "<td width=10%><b><font color=\"blue\" > Status </font></b></td>";
                print "</tr>\n";
                $sql="SELECT PATH, NEVENTS, NEVENTSERR, TIMESTAMP, STATUS, SIZEMB FROM Ntuples WHERE RUN=$run ";
                my $r1=$self->{sqlserver}->Query($sql);
               if (defined $r1->[0][0]) {
                foreach my $nt (@{$r1}){
                 my $path    = trimblanks($nt->[0]);
                 if ($accessmode eq "REMOTE") {
                  my $subpath = getPathNoDisk($path);
                  $sql = "SELECT prefix, path FROM MC_DST_COPY WHERE CITE='$remotecite' and path like '%$subpath%'";
                  my $r2=$self->{sqlserver}->Query($sql);
                  if (defined $r2->[0][0]) {
                     my $prefix = trimblanks($r2->[0][0]);
                     my $spath  = trimblanks($r2->[0][1]);
                     $path = $prefix."/".$spath;
                  } else {
                      $printit = 0;
                  }
                 }
                 if ($printit == 1) {
                  my $timel =localtime($nt->[3]);
                  my ($wday,$mon,$day,$time,$year) = split " ",$timel;
                  my $status=$nt->[4];
                  my $color=statusColor($status);
                  print "<td width=50%><b> $path    </td></b><td><b> $nt->[1] </td>
                        <td align=middle width=5%><b> $nt->[2] </b></td>
                        <td align=middle width=5%><b> $nt->[5] </b></td>
                        <td align=middle width=25%><b> $mon $day, $time, $year </b></td> 
                        <td align=middle width=10%><b><font color=$color> $status </font></b></td> \n";
                 print "</font></tr>\n";
              }
             }
            }
            htmlTableEnd();
            print "<BR><BR>\n";
            }
           }
   } elsif ($q->param("NTOUT") eq "RUNS") {  
# ... print Runs
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     print "<td><b><font color=\"blue\">Job </font></b></td>";
     print "<td><b><font color=\"blue\" >Run </font></b></td>";
     print "<td><b><font color=\"blue\" >Job Submit Time </font></b></td>\n";
     print "</tr>\n";
     my $color="black";
     my $i =0;
       foreach my $run (@runs){
         my $jobname = $jobnames[$i];
         my $submit  = $submits[$i];
         my $printit = 1;
         if ($accessmode eq "REMOTE") {
          $sql = "SELECT run FROM MC_DST_COPY WHERE run=$run AND cite='$remotecite'";
          my $r0=$self->{sqlserver}->Query($sql);
          if (not defined $r0->[0]) {
           $printit = 0;
           }
         }
         if ($printit == 1) {
          $i++;
          print "
            <td><b><font color=$color> $jobname </font></td></b>
            <td><b><font color=$color> $run </font></b></td>
            <td><b><font color=$color> $submit </font></b></td>\n";
            print "</font></tr>\n";
        }
     }
   htmlTableEnd();
  } elsif ($q->param("NTOUT") eq "FILES") {
# ... print DSTs
    print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
    print "<td><b><font color=\"blue\" >File Path </font></b></td>";
    print "</tr>\n";
     my $i =0;
       foreach my $run (@runs){
               if ($accessmode eq "REMOTE") {
                $sql = "SELECT prefix,path FROM MC_DST_COPY WHERE run=$run AND cite='$remotecite'";
                my $r0=$self->{sqlserver}->Query($sql);
                foreach my $path (@{$r0}) {
                   print "<td><b><font color=$color> $path->[0]/$path->[1] </font></td></b></font></tr>\n";
               }
            } else {
             my $jobname = $jobnames[$i];
             my $submit  = localtime($submits[$i]);
             $i++;
             $sql = "SELECT path From Ntuples WHERE Run=$run";
             my $r1=$self->{sqlserver}->Query($sql);
             foreach my $path (@{$r1}) {
             print "<td><b><font color=$color> $path->[0] </font></td></b></font></tr>\n";
           }
         }
      }
      htmlTableEnd();
   } elsif ($q->param("NTOUT") eq "SUMM") {
# ... print summary
      my $firstrun  = 0;
      my $lastrun   = 0;
      my $starttime = time();
      my $endtime   = 0;
      my $gigabytes = 0;
      my $nruns     = 0;
      my $ndsts     = 0;
      my $nevents   = 0;

      my $i =0;
       foreach my $run (@runs){
         my $jobname = $jobnames[$i];
         my $submit  = $submits[$i];
         $i++;
          if ($submit < $starttime) { $starttime = $submit; $firstrun = $run;}
          if ($submit > $endtime)   { $endtime   = $submit; $lastrun  = $run;}
          $nruns++;
          $sql = "SELECT COUNT(RUN), SUM(SIZEMB), SUM(NEVENTS) FROM Ntuples WHERE RUN=$run";
          my $r1=$self->{sqlserver}->Query($sql);
          foreach my $s (@{$r1}) {
              $ndsts = $ndsts + $s->[0];
              $gigabytes = $gigabytes + $s->[1];
              $nevents   = $nevents   + $s->[2];
          }
     }
    $gigabytes = $gigabytes/1000;
    my $from=localtime($starttime);
    my $to  =localtime($endtime);
    print "<tr><td><b><font size=\"4\" color=\"green\"><ul>  Jobs Completed : $nruns </b></font></td></tr>\n";
    print "<tr><td><b><font size=\"4\" color=\"green\"><ul>  $from Run : $firstrun  </b></font></td></tr>\n";
    print "<tr><td><b><font size=\"4\" color=\"green\"><ul>  $to   Run   $lastrun   </b></font></td></tr>\n";
    print "<tr><td><b><font size=\"4\" color=\"blue\"><ul>   DSTs : $ndsts ($gigabytes GB) </b></font></td></tr>\n";
    print "<tr><td><b><font size=\"4\" color=\"blue\"><ul>   Events : $nevents  </b></font></td></tr>\n";

  } elsif ($q->param("NTOUT") eq "ROOT") {
#... write RootAnalysisTemplate
      my $RootAnalysisTextNFS = 
         "// ROOT files accessed via NFS
         "; 
      my $RootAnalysisTextCastor = 
         "// it is assumed that CASTOR directory 
             structure is similar to one on AMS disks.
             /castor/cern.ch/MC/AMS02/ProductionPeriod/...
         ";
      my $RootAnalysisTextHTTP = 
         "// wildcards are not implemented yet in ROOT. 
             still  have to check what is the HTTPD protocol for
             getting a list of files.
         ";
     
      my $RootAnalysisTextRemote = 
         "// it is assumed that REMOTE directory structure and lib(s) path are similar to one on AMS disks.
         ";
      my $RootAnalysisTemplateTxt = 
         "gROOT->Reset(); 
          // for linux load
          gSystem->Load(\"/offline/vdev/lib/linux/icc/ntuple.so\");
          //
          //  for dunix aka ams.cern.ch load 
          //  gSystem->Load(\"/offline/vdev/lib/osf1/ntuple.so\");
          //
          TChain chain(\"AMSRoot\");
      ";
#...... check files access option
      $accessmode = "NFS";
      my $prefix     = "xyz";
       
      if (defined $q->param("ROOTACCESS")) {
          if ($q->param("ROOTACCESS") eq "HTTP") {
              $accessmode = "HTTP";
              $prefix     = "http://$self->{HTTPserver}";
          }
          if ($q->param("ROOTACCESS") eq "CASTOR") {
              $accessmode = "CASTOR";
              $prefix     = "rfio:/castor/cern.ch/ams/";
          }
      }
#..... remote cite (if any)
      if (defined $q->param("REMOTEACCESS")) {
          $accessmode="REMOTE";
          $remotecite = $q->param("REMOTEACCESS");
      }

    my $filename=$q->param("ROOT");
    my $buff=undef;
    my @dirs=[];
    print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
    print "<td><b><i>will be written into file : </i><font color=\"blue\" > $filename </font> </b></td>";
    print "</tr>\n";
      if ($accessmode eq "HTTP") {
         $buff = $RootAnalysisTextHTTP;
         print "<tr><td>$RootAnalysisTextHTTP</td></tr>\n";
     } elsif ($accessmode eq "CASTOR") {
         $buff = $RootAnalysisTextCastor;
         print "<tr><td>$RootAnalysisTextCastor</td></tr>\n";
     } elsif ($accessmode eq "REMOTE") {
         $buff = $RootAnalysisTextRemote;
         print "<tr><td>$RootAnalysisTextRemote</td></tr>\n";
     } else {
         $buff = $RootAnalysisTextNFS;
         print "<tr><td>$RootAnalysisTextNFS</td></tr>\n";
     }
         $buff=$buff."\n";
         $buff=$buff.$RootAnalysisTemplateTxt."\n";
         print "<tr><td>gROOT->Reset();</td></tr>\n";
         print "<tr><td>// for linux load</td></tr>\n";
         print "<tr><td>gSystem->Load(\"/offline/vdev/lib/linux/icc/ntuple.so\");</tr></td>\n";
         print "<tr><td>//</tr></td>\n";
         print "<tr><td>//  for dunix aka ams.cern.ch load </tr></td>\n";
         print "<tr><td>//  gSystem->Load(\"/offline/vdev/lib/osf1/ntuple.so\");</tr></td>\n";
         print "<tr><td>//</tr></td>\n";
         print "<tr><td>TChain chain(\"AMSRoot\");</tr></td>\n";
# 
      if ($accessmode eq "NFS") {
         foreach my $run (@runs){
          my $sql = "SELECT path From Ntuples WHERE Run=$run AND CRCFLAG=1";
          my $r1=$self->{sqlserver}->Query($sql);
          foreach my $path (@{$r1}) {
           $path=trimblanks($path->[0]);
           my @junk = split '/',$path;
           my $tdir ="";
           for (my $i=1; $i<$#junk; $i++) {
             $tdir = $tdir."/".$junk[$i];
         }
         $tdir = trimblanks($tdir);
         my $dirfound = 0;
         foreach my $dir (@dirs) {
          if ($dir eq $tdir) {
           $dirfound = 1;
           last;
          } 
         }
         if ($dirfound == 1) {
# skip it
         } else {
          $dirs[$#dirs] = $tdir;
          my $s = "chain.Add(\"".$tdir."/*.root\");";
          print "<tr><td> $s </tr></td>\n";
          $buff = $buff.$s."\n";
          push @dirs, $tdir;
        }
     }
      }
     } elsif ($accessmode eq "HTTP") {
         foreach my $run (@runs){
          my $sql = "SELECT path From Ntuples WHERE Run=$run AND CRCFLAG=1";
          my $r1=$self->{sqlserver}->Query($sql);
          foreach my $path (@{$r1}) {
           $path=trimblanks($path->[0]);
           my $httppath=$prefix.$path;
           my $s = "chain.Add(\"$httppath\");";
           print "<tr><td> $s </tr></td>\n";
           $buff = $buff.$s."\n";
          }
      }
     } elsif ($accessmode eq "REMOTE") {
         foreach my $run (@runs){
          my $sql = "SELECT prefix,path From MC_DST_COPY WHERE Run=$run AND Cite='$remotecite'";
          my $r1=$self->{sqlserver}->Query($sql);
          foreach my $p (@{$r1}) {
              my $prefix = trimblanks($p->[0]);
              my $local  = trimblanks($p->[1]);
              my $path=$prefix."/".$local;
              my $s = "chain.Add(\"$path\");";
              print "<tr><td> $s </tr></td>\n";
              $buff = $buff.$s."\n";
          }
      }
     } elsif ($accessmode eq "CASTOR") {
         foreach my $run (@runs){
          my $sql = "SELECT path From Ntuples WHERE Run=$run AND CRCFLAG=1 AND CASTORTIME != 0";
          my $r1=$self->{sqlserver}->Query($sql);
          foreach my $path (@{$r1}) {
           $path=trimblanks($path->[0]);
           my @junk = split '/',$path;
           my $tdir ="";
           for (my $i=2; $i<$#junk; $i++) {
             $tdir = $tdir."/".$junk[$i];
         }
         $tdir = trimblanks($tdir);
         my $dirfound = 0;
         foreach my $dir (@dirs) {
          if ($dir eq $tdir) {
           $dirfound = 1;
           last;
          } 
         }
         if ($dirfound == 1) {
# skip it
         } else {
          $dirs[$#dirs] = $tdir;
          my $s = "chain.Add(\"".$prefix.$tdir."/*.root\");";
          print "<tr><td> $s </tr></td>\n";
          $buff = $buff.$s."\n";
          push @dirs, $tdir;
        }
       }
      }
     }

     htmlTableEnd();
      if (defined $buff) {
       $self-> writetofile($filename,$buff);
      }
    }
     htmlReturnToQuery();
    htmlBottom();
   }
  } elsif ($self->{q}->param("queryDB04") eq "Continue") {
     my $query=$q->param("QPart");
     foreach my $dataset (@{$self->{DataSetsT}}){
      if($dataset->{name} eq $query){
                 foreach $cite (@{$dataset->{jobs}}){
                 if(not ($cite->{filename} =~/^\./)){
                  push @tempnam, $cite->{filename};
                  $hash->{$cite->{filename}}=$cite->{filedesc};
                  push @desc, $hash -> {$cite->{filename}}=$cite->{filedesc};
              }
           }
        }
      }

   htmlTop();
    $self->htmlAMSHeader("AMS-02 MC Database Query Form");
    print "<ul>\n";
    htmlMCWelcome();
    print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n"; 
     print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
      print "<tr><td><b><font color=\"green\">Datasets </font></b>\n";
      print "</td><td>\n";
      print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
       print "<tr valign=middle><td align=left><b><font size=3 color=green> $query</b></td> <td colspan=1>\n";
       print "<INPUT TYPE=\"hidden\" NAME=\"QPart\" VALUE=\"$query\">\n"; 
      htmlTableEnd();
     if ($query ne "Any" and $query ne "ANY" and $query ne "any") {
# Job Template
      print "<tr><td><b><font color=\"green\">Template</font></b>\n";
      print "</td><td>\n";
      print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
       print "<tr valign=middle><td align=left><b><font size=\"-1\"> </b></td> <td colspan=1>\n";
       print "<select name=\"QTempDataset\" >\n";
       my $i=0;
       print "<option value=\"Any\"> ANY </option>\n";
       foreach my $template (@tempnam) {
         my @description = split /Total/,$desc[$i];
         print "<option value=\"$description[0]\">$description[0] </option>\n";
         $i++;
        }
        print "</select>\n";
        print "</b></td></tr>\n";
      htmlTableEnd();
   }
# Trigger Type
     print "<tr><td><b><font color=\"green\">Job Parameters</font></b>\n";
     print "</td><td>\n";
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
      print "<tr valign=middle><td align=left><b><font size=\"-1\"> Trigger </b></td> <td colspan=1>\n";
      print "<select name=\"QTrType\" >\n";
      my %hash=%{$self->{triggertypes}};
      my @keysa=sort {$hash{$b} <=>$hash{$a}} keys %{$self->{triggertypes}};
      foreach my $trigger (@keysa) {
       print "<option value=\"$trigger\">$trigger </option>\n";
      }
       print "<option value=\"Any\">ANY </option>\n";
      print "</select>\n";
      print "</b></td></tr>\n";
# Momentum
      htmlTextField("Momentum min >=","number",7,1.,"QMomI","[GeV/c] :  valuable only if template ");  
      htmlTextField("Momentum max =<","number",7,10000.,"QMomA","[GeV/c] : type is ANY");  
     htmlTableEnd();
    htmlTableEnd();
# Output format
   print "<b><font color=green> Output :  </font><INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"ALL\" CHECKED> Full Listing\n";
   print "&nbsp;<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"RUNS\"> Only run numbers;\n";
   print "&nbsp;<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"FILES\"> Only file names;\n";
   print "<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"SUMM\"> Summary \n";
   print "<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"ROOT\"> ROOT Analysis Filename <INPUT TYPE=\"text\" name=\"ROOT\">";

   print "<br><TR></TR>";
   print "<br><TR></TR>";
   print "<b><font color=blue> ROOT Files \@CERN  </font>\n";
   print "<br><TR></TR>";
   print "<b><font color=blue> Access Mode  </font>\n";
   print "<INPUT TYPE=\"radio\" NAME=\"ROOTACCESS\" VALUE=\"NFS\">  NFS \n";
   print "<INPUT TYPE=\"radio\" NAME=\"ROOTACCESS\" VALUE=\"HTTP\"> via WebServer \n";
   print "<INPUT TYPE=\"radio\" NAME=\"ROOTACCESS\" VALUE=\"CASTOR\">  rfio CASTOR\n";
   print "<i><font color=green> (Note : files are copied to CASTOR weekly, access via HTTP is slow) </font><i>\n";
   print "<TR></TR>\n";

   print "<br><TR></TR>";
   print "<br><TR></TR>";
   print "<b><font color=blue> Files Available on Remote Cites (Note : no access from CERN, ask cite's Rep for details) :  </font>\n";
   print "<br><TR></TR>";
   print "<b><font color=blue> Cite :  </font>\n";
   print "<INPUT TYPE=\"radio\" NAME=\"REMOTEACCESS\" VALUE=\"lyon\">  Lyon \n";
   print "<TR></TR>\n";

     print "<p><br>\n";
     print "<INPUT TYPE=\"hidden\" NAME=\"SQLQUERY\" VALUE=\"$sql\">\n"; 
     print "<input type=\"submit\" name=\"queryDB04\" value=\"DoQuery\">        ";
     print "</form>\n";

     htmlReturnToQuery();
   htmlBottom();
  } else {
   htmlTop();
    $self->htmlAMSHeader("AMS-02 MC Database Query Form");
    print "<ul>\n";
    htmlMCWelcome();
    print "<p>\n";
    print "<TR><B><font color=green size= 4> Select <i>Dataset</i> and click <font color=red> Continue </font> or <i> Run/Job/DST  ID </i> and click <font color=red> Submit </font> </font>";
    print "<p>\n";
    print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";
    print "<TABLE BORDER=\"1\" WIDTH=\"50%\">";
     print "<tr><td valign=\"middle\"><b><font color=\"blue\" size=\"3\">Datasets (MC Production)</font></b></tr>\n";
     print "</td><td>\n";
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     print "<tr><td><font size=\"-1\"<b>\n";
           print "</b>
                   <INPUT TYPE=\"radio\" NAME=\"QPart\" VALUE=\"Any\" CHECKED>ANY<BR>\n";
           print "</b></font></td></tr>\n";
           foreach my $dataset (@{$self->{DataSetsT}}){
             print "</b>
                     <INPUT TYPE=\"radio\" NAME=\"QPart\" VALUE=$dataset->{name}>$dataset->{name}<BR>\n";
              print "</b></font></td></tr>\n";
           }
        htmlTableEnd();
      htmlTableEnd();
   print "<p><br>\n";
     print "<input type=\"submit\" name=\"queryDB04\" value=\"Continue\">        ";
     print "</form>\n";
      print "<tr></tr>\n";
      print "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\" width=\"100%\">\n";
      print "<tr><td valign=\"middle\" bgcolor=\"whitesmoke\"><font size=\"+2\"><B>\n";
      print "Find Job : (eg 805306383 or From-To) </B></font></td></tr></table> \n";
      print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";
      print "<b>JobID : </b> <input type =\"text\" name=\"JobID\">\n";

          print "<tr valign=middle><td align=left><b><font size=\"-1\"> Cite : </b></td> <td colspan=1>\n";
          print "<select name=\"QCite\" >\n";
          print "<option value=\"Any\">Any </option>\n";
          foreach my $cite (@{$self->{CiteT}}){
              print "<option value=\"$cite->{name}\">$cite->{name} </option>\n";
          }
          print "</select>\n";
          print "</b></td></tr>\n";

      print "<input type=\"submit\" name=\"getJobID\" value=\"Submit\"> \n";
      print "</form>\n";
      print "</table> \n";
       print "<tr></tr>\n";
       print "<br><p>\n";
       print "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\" width=\"100%\">\n";
       print "<tr><td valign=\"middle\" bgcolor=\"whitesmoke\"><font size=\"+2\"><B>\n";
       print "Find Run : (eg 1073741826  or From-To) </B></font></td></tr></table> \n";
       print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";
       print "<b>RunID : </b> <input type =\"text\" name=\"RunID\">\n";
       print "<input type=\"submit\" name=\"getRunID\" value=\"Submit\"> \n";
       print "</form>\n";
       print "</table> \n";
       print "<br><p>\n";
        print "<tr></tr>\n";
        print "<p></p>\n";
        print "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\" width=\"100%\">\n";
        print "<tr><td valign=\"middle\" bgcolor=\"whitesmoke\"><font size=\"+2\"><B>\n";
        print "Find DST(s) : (eg 1073741826  or From-To) </B></font></td></tr></table> \n";
        print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";
        print "<b>RunID : </b> <input type =\"text\" name=\"DSTID\">\n";
        print "<input type=\"submit\" name=\"getDSTID\" value=\"Submit\"> \n";
        print "</form>\n";
        print "</table> \n";


  htmlBottom();
  }
 }
#queryDB04 ends here



#Initial Request (just e-mail)
# UserRegistration 
    if ($self->{q}->param("UserRegistration")){
     $self->{read}=1;
      htmlTop();
        $self->htmlTemplateTable("AMS MC02 User Registration Form");
          print "<tr><td><b><font color=\"red\">User Info</font></b>\n";
          print "</td><td>\n";
          print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
           htmlTextField("First and Last Name","text",24,"' '","CNA","(Santa Klaus)");  
           htmlTextField("e-mail address","text",24,"' '","CEM","(name\@mail.domain)");  
           print "<tr valign=middle><td align=left><b><font size=\"-1\"> Cite : </b></td> <td colspan=1>\n";
          print "<select name=\"CCA\" >\n";
          my @cite=();
          foreach my $cite (@{$self->{CiteT}}){
             print "<option value=\"$cite->{name}\"> $cite->{name} </option>\n";
         }
          print "</select>\n";
          print "</b></td></tr>\n";
         htmlTableEnd();
        htmlTableEnd();
        print "<input type=\"submit\" name=\"MyRegister\" value=\"Submit\"></br><br>\n";
       htmlReturnToMain();
      htmlFormEnd();
     htmlBottom();
 }
# UserRegistration ends here
    if ($self->{read} != 1) {
     if ($self->{q}->param("RemoteClientEmail")){
      $self->{read}=1;
      my $cem=$self->{q}->param("CEM");
      if(validate_email_address($cem)){
        if ($self->findemail($cem)){
            my $sql="SELECT cid FROM Mails WHERE address='$cem'";
            my $ret=$self->{sqlserver}->Query($sql);
            my $cid=$ret->[0][0];
            if (not defined $cid) {
              $self->ErrorPlus("Unknown cite for user $cem. Check spelling.");
            }
            $sql="SELECT rsite FROM Mails WHERE address='$cem' AND cid=$cid";
            $ret=$self->{sqlserver}->Query($sql);
            my $resp = $ret->[0][0];
            $sql="SELECT name FROM Mails WHERE address='$cem' AND cid=$cid";
            $ret=$self->{sqlserver}->Query($sql);
            my $name = $ret->[0][0];
            $sql="SELECT name FROM Cites WHERE cid=$cid";
            $ret=$self->{sqlserver}->Query($sql);
            my $cite = $ret->[0][0];
            htmlTop();
            if ($resp == 1) {
             print "<TR><B><font color=green size= 5> Select Template or Production DataSet: </font>";
            } else { 
             print "<TR><B><font color=green size= 5> Select Template : </font>";
            }
            print "<p>\n";
            print "<FORM METHOD=\"POST\" action=\"$self->{Name}\">\n";
            print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
            print "<tr><td>\n";
            print "<b><font color=\"magenta\" size=\"3\">User's Info </font>
                   <font size=\"2\">(if in <i>italic</i> then cannot be changed)</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
            print "<i><b>e-mail : </td><td><input type=\"text\" size=24 value=$cem name=\"CEM\" onFocus=\"this.blur()\" >
                   </i>\n";
            print "</b></font></td></tr>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
            print "<i><b>cite : </td><td><input type=\"text\" size=18 value=$cite name=\"CCA\" onFocus=\"this.blur()\"></i>\n";
            print "</b></font></td></tr>\n";
            print "</TABLE>\n";
             print "<tr><td><b><font color=\"blue\" size=\"3\">Datasets </font><font color=\"tomato\" size=\"3\"> (MC PRODUCTION)</font></b>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             my $firstdataset = 1;
             my $checked="CHECKED";
              foreach my $dataset (@{$self->{DataSetsT}}){
              if ($firstdataset++ != 1) {
                  $checked="";
              }
              print "</b>\n";
                 if ($dataset->{eventstodo} == 0) {
                  print "<tr><td><b><font color=\"tomato\"><i> $dataset->{name} </i></font></b></td></tr>\n";
                 } elsif ($dataset->{eventstodo} < 5000) {                  
                   print "<tr><td><b><font color=\"tomato\"> $dataset->{name} </font><b></td></tr><BR>\n";
               } else {
                    print "<INPUT TYPE=\"radio\" NAME=\"CTT\" VALUE= $dataset->{name} $checked>$dataset->{name} <BR>\n";
                }
              print "</b></font></td></tr>\n";
             }
            print "</TABLE>\n";
            print "<tr><td>\n";
            print "<b><font color=\"red\" size=\"3\">Template</font></b>\n";
            print "</td><TD>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"</b>\n";
            print "<INPUT TYPE=\"radio\" NAME=\"CTT\" VALUE=\"Basic\" >Basic<BR>\n";
            print "<INPUT TYPE=\"radio\" NAME=\"CTT\" VALUE=\"Advanced\" >Advanced<BR>\n";
            print "</b></font></td></tr>\n";
            print "</TABLE>\n";
            print "</TABLE>\n";

            print "<TR><TD><font color=\"green\" size=\"3\"> Important : Basic and Advanced Templates are NOT PART OF MC PRODUCTION </font></TD></TR>\n";
            print "<br>\n";
            print "<TR><TD><font color=\"tomato\" size=\"3\"> Note : If dataset is not clickable, it means that all events already allocated for running jobs or processed</font></TD></TR>\n";
          
          print "<p>\n";
          print "<br>\n";
          print "<td><input type=\"submit\" name=\"FormType\" value=\"Continue\">       ";

          htmlReadMeFirst();
          htmlReturnToMain();
          htmlFormEnd();
         htmlBottom();
      } else {
            $self->ErrorPlus("Client e-mail $cem not exist. Check spelling or do registration.");
        }
     } else {
            $self->ErrorPlus("E-Mail $cem Seems Not to Be Valid (1).");
     }
 }
}
    if ($self->{q}->param("Registration")){
        $self->{read}=1;
        my $cem=lc($self->{q}->param("CEM"));
        if ($self->findemail($cem)){
            $self->ErrorPlus("Client $cem Already Exists.");
        }
        else{
#check email
        if(validate_email_address($cem)){
        if($self->{q}->param("Registration") eq "Submit"){
            my $name=$self->{q}->param("CNA");
            if(not defined $name or $name eq ""){
             $self->ErrorPlus("Please fill in Name and Surname Field");
            }
            my $responsible;
            my $cite=$self->{q}->param("CCA");
            if(not defined $cite or $cite eq ""){
                $cite=$self->{q}->param("CCAS");
              }
        my $sendsuc=undef;
        foreach my $chop (@{$self->{MailT}}) {
              if($chop->{rserver}==1){
                  my $address=$chop->{address};
                  my $subject="AMS02 MC User Registration Request ";
                  my $message=" E-Mail: $cem \n Name: $name \n Cite: $cite \n Responsible: $responsible"; 
                  $self->sendmailmessage($address,$subject,$message);
                  $sendsuc=$address;
                  last;
              }
        }          
        if(not defined $sendsuc){
            $self->ErrorPlus("Unable to find responsible for the server. Please try again later..");
        }
# check that e-mail address is unique
            $cem = trimblanks($cem);
            my $sql ="SELECT address from Mails where address='$cem'";
            my $ret=$self->{sqlserver}->Query($sql);
            if (defined $ret->[0][0]) {
                 my $error=" User with $cem address is already registered";
                 $self->ErrorPlus("$error");
                 return;
            }
# build up the corr entries in the database
            
# check if cite exist
            $sql="select cid from Cites where name='$cite'";
            $ret=$self->{sqlserver}->Query($sql);
            my $cid=$ret->[0][0];
            my $newcite=0;
            if(not defined $cid){
             $newcite=1;
             $sql="select cid from Cites";
             $ret=$self->{sqlserver}->Query($sql);
             $cid=$ret->[$#{$ret}][0]+1;
             if($cid>$MAX_CITES){
                 my $error=" Too many Cites. Your request will not be procedeed.";
                 $self->sendmailerror($error,"$cem");
                 $self->ErrorPlus("$error");
                 return;
             }
             my $run=(($cid-1)<<$MAX_RUN_POWER)+1;
             my $time=time();
             my $citedesc = "new cite";
             $sql="insert into Cites values($cid,'$cite',0,'remote',$run,0,'$citedesc',$time,0)";
             $self->{sqlserver}->Update($sql);

            }
            $sql="select mid from Mails";
            $ret=$self->{sqlserver}->Query($sql);
            my $mid=$ret->[$#{$ret}][0]+1;
            my $resp=$newcite;
            my $time=time();
            $cem = trimblanks($cem);
            $name= trimblanks($name);
            $sql="insert into Mails values($mid,'$cem',NULL,'$name',$resp,0,$cid,'Blocked',0,$time,0,0)";
            $self->{sqlserver}->Update($sql);
            if($newcite){
                $sql="update Cites set mid=$mid where cid=$cid";
                $self->{sqlserver}->Update($sql);
            }
         $self->{FinalMessage}=" Your request to register was succesfully sent to $sendsuc. Your account will be enabled soon.";     
        }
    }else{
            $self->ErrorPlus("E-Mail $cem Seems Not to Be Valid. (2)");
        }
       }  
    }

# cite registration
    if ($self->{q}->param("CiteRegistration")){
        $self->{read}=1;
        my $cem=lc($self->{q}->param("CEM"));
        if ($self->findemail($cem)){
            $self->ErrorPlus("User with e-mail $cem Already Exists.");
        }
        else{
#check email
        if(validate_email_address($cem)){
# build up the corr entries in the database
         my @cite=();
         foreach my $cite (@{$self->{CiteT}}){
             push @cite, $cite->{name};
         }
         my $name=$self->{q}->param("CNA");
         if(not defined $name or $name eq ""){
             $self->ErrorPlus("Please fill Name Field");
         }
         my $addcite=$self->{q}->param("CCA");
         if(not defined $addcite or $addcite eq ""){
             $self->ErrorPlus("Please fill Cite (University) Field");
         }
         my $newcitedesc=$self->{q}->param("CCF");
         if(not defined $newcitedesc or $newcitedesc eq ""){
             $self->ErrorPlus("Please fill Cite (University) Field");
         }
# send e-mail
        my $sendsuc=undef;
        foreach my $chop (@{$self->{MailT}}) {
             if($chop->{rserver}==1){
              my $address=$chop->{address};
              my $subject="AMS02 MC Cite Registration Request ";
              my $message=" E-Mail: $cem \n Name: $name \n Cite: $addcite ($newcitedesc)\n";
              $self->sendmailmessage($address,$subject,$message);
              $sendsuc=$address;
          }
        }        
        if(not defined $sendsuc){
            $self->ErrorPlus("Unable to find responsible for the server. Please try again later..");
        }
# check if cite exist
            my $sql="select cid from Cites where name='$addcite'";
            my $ret=$self->{sqlserver}->Query($sql);
            my $cid=$ret->[0][0];
            my $newcite=0;
            if(not defined $cid){
# add cite
             $newcite=1;
             my $time=time();
             $sql="SELECT MAX(cid) FROM Cites";
             $ret=$self->{sqlserver}->Query($sql);
             $cid=$ret->[0][0]+1;
             if($cid>$MAX_CITES){
                 my $error=" Too many Cites. Your request will not be procedeed.";
                 $self->sendmailerror($error,"$cem");
                 $self->ErrorPlus("$error");
             }
# 4.12.03 $MAX_CITES changed from 16 to 32
#            my $run=(($cid-1)<<27)+1;
             my $run=(($cid-1)<<$MAX_RUN_POWER)+1;
             $sql="INSERT INTO Cites VALUES($cid,'$addcite',0,'remote',$run,0,'$newcitedesc',$time,0)";
             $self->{sqlserver}->Update($sql);
# add responsible
             $sql="select MAX(mid) from Mails";
             $ret=$self->{sqlserver}->Query($sql);
             my $mid=$ret->[0][0]+1;
             my $resp=1;
             $sql="INSERT INTO Mails values($mid,'$cem',NULL,'$name',$resp,0,$cid,'Blocked',0,$time,0,0)";
            $self->{sqlserver}->Update($sql);
# add responsible info to Cites
             $sql="UPDATE Cites SET mid=$mid WHERE cid=$cid";
             $self->{sqlserver}->Update($sql);
         $self->{FinalMessage}=" Your request to register was succesfully sent to $sendsuc. Your account and cite registration will be done soon.";     
         } else {
            $self->ErrorPlus("Seems $addcite is already registered.");
        }
       }else{
            $self->ErrorPlus("E-Mail $cem Seems Not to Be Valid. (3)");
        }
       }  
    }
# CiteRegistration ends here

    if ($self->{q}->param("MyRegister")){
        $self->{read}=1;
        my $cem=lc($self->{q}->param("CEM"));
        if ($self->findemail($cem)){
            $self->ErrorPlus("Client $cem Already Exists.");
        }
        else{
#check email
        if(validate_email_address($cem)){
        if($self->{q}->param("MyRegister") eq "Register"){
         print $q->header( "text/html" ),
         $q->start_html( "Welcome");
         print $q->h1( "Welcome to the AMS02 RemoteClient  Registration Form" );
         print $q->start_form(-method=>"GET", 
          -action=>$self->{Name});
         print "E-Mail Address";
         print $q->textfield(-name=>"CEM",-default=>"$cem");
         print "<BR>";
         print "Name and SurName";
         print $q->textfield(-name=>"CNA",-default=>"");
         print "<BR>";
         print "Type Cite Name";
         print $q->textfield(-name=>"CCA",-default=>"");

         my @cite=();
         foreach my $cite (@{$self->{CiteT}}){
             push @cite, $cite->{name};
         }
         print "Or Choose It Here";
         print $q->popup_menu(
          -name=>"CCAS",
          -values=>\@cite,
          -default=>$cite[0]);
         print "<BR>";
         print "Type Yes if you want to be responsible for the cite";
         print "<BR>";
   print qq`
<INPUT TYPE="radio" NAME="CCR" VALUE="Yes">Yes<BR>
<INPUT TYPE="radio" NAME="CCR" VALUE="No" CHECKED>No<BR>
`;
         print $q->submit(-name=>"MyRegister", -value=>"Submit");
         print $q->reset(-name=>"Reset");
         print $q->end_form;
# real registration            
        }elsif($self->{q}->param("MyRegister") eq "Submit"){
            my $name=$self->{q}->param("CNA");
            if(not defined $name or $name eq ""){
             $self->ErrorPlus("Please fill in Name and Surname Field");
            }
            my $responsible=$self->{q}->param("CCR");
            my $cite=lc($self->{q}->param("CCA"));
            if(not defined $cite or $cite eq ""){
                $cite=$self->{q}->param("CCAS");
              }
        my $sendsuc=undef;
        foreach my $chop (@{$self->{MailT}}) {
              if($chop->{rserver}==1){
                  my $address=$chop->{address};
                  my $subject="AMS02 MC User Registration Request ";
                  my $message=" E-Mail: $cem \n Name: $name \n Cite: $cite \n Responsible: $responsible"; 
                  $self->sendmailmessage($address,$subject,$message);
                  $sendsuc=$address;
                  last;
              }
        }          
        if(not defined $sendsuc){
            $self->ErrorPlus("Unable to find responsible for the server. Please try again later..");
        }

# build up the corr entries in the database
                        
# check if cite exist
            my $sql="select cid from Cites where name='$cite'";
            my $ret=$self->{sqlserver}->Query($sql);
            my $cid=$ret->[0][0];
            my $newcite=0;
            if(not defined $cid){
             $self->ErrorPlus("Unknown Cite : '$cite', Please register it");
            }
            $sql="SELECT MAX(mid) FROM Mails";
            $ret=$self->{sqlserver}->Query($sql);
            my $mid=$ret->[0][0]+1;
            my $resp=0;
            my $time=time();
            $sql="insert into Mails values($mid,'$cem',NULL,'$name',$resp,0,$cid,'Blocked',0,$time,0,0)";
            $self->{sqlserver}->Update($sql);
         $self->{FinalMessage}=" Your request to register was succesfully sent to $sendsuc. Your account will be enabled soon.";     
        }
    }else{
            $self->ErrorPlus("E-Mail $cem Seems Not to Be Valid.(4)");
        }
       }  
    }
#MyRegister ends here

# Files Upload
    if ($self->{q}->param("Download")){
     $self->{read}=1;
     $self->{CEM}=$self->{q}->param("CEM");
     my $upl0=$self->{q}->param("UPL0");
     my $upl1=$self->{q}->param("UPL1");
     my $time = time();
     if ($upl1 > 0) {
      $sql=
      "update Mails set timeu1=$upl0, timeu2=$upl1, timestamp=$time WHERE address='$self->{CEM}'";
     } else {
      $sql=
      "update Mails set timeu1=$upl0, timestamp=$time WHERE address='$self->{CEM}'";
     } 
     $self->{sqlserver}->Update($sql);
     $self->AllDone();
   }
#Download ends here 

    if ($self->{q}->param("FormType")){
        $self->{read}=1;
# check ReadMeFirst
        if ($self->{q}->param("FormType") eq "ReadMeFirst") {
         htmlTop();
         $self->htmlTemplateTable("ReadMeFirst (click [Return] to Select Template or Production DataSet)");
         print "<p></p>\n";
         print $q->textarea(-name=>"RMF",-default=>"$self->{tsyntax}->{headers}->{help}",-rows=>30,-columns=>80);
         print "<BR><TR>";
#         print $q->submit(-name=>"FormType", -value=>"Return");
         print htmlBottom();
         return 1;   
        }
# check e-mail
        my $cem=lc($self->{q}->param("CEM"));
        if (not $self->findemail($cem)){
            $self->ErrorPlus("Client $cem Not Found. Please Register.");
        }
        elsif($self->{CEMA} ne "Active"){
            $self->ErrorPlus("Welcome $cem. Your account is not yet set up.
            Please try again later.");
        }
        elsif($self->{q}->param("CTT")){
# check form type
            if($self->{q}->param("CTT") eq "Basic"){
             htmlTop();
             $self->htmlTemplateTable("Select Basic Template File and Job Parameters");
# print templates
             my @tempnam=();
             my $hash={};
             foreach my $cite (@{$self->{TempT}}){
              if(not ($cite->{filename} =~/^\./)){
               push @tempnam, $cite->{filename};
               $hash->{$cite->{filename}}=$cite->{filedesc};
             }
          }
          print "<tr><td><b><font color=\"red\">Job Template </font></b>\n";
          print "</td><td>\n";
          print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
          print "<tr valign=middle><td align=left><b><font size=\"-1\"> File : </b></td> <td colspan=1>\n";
          print "<select name=\"QTemp\" >\n";
          foreach my $template (@tempnam) {
            print "<option value=\"$template\">$template </option>\n";
          }
          print "</select>\n";
          print "</b></td></tr>\n";
          htmlTextField("Nick Name","text",24,"MC02-basic","QNick"," ");  
          print "</TABLE>\n";
# Cite Parameters
              print "<tr><td><b><font color=\"blue\">Cite HW Parameters</font></b>\n";
              print "</td><td>\n";
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
              print "<tr valign=middle><td align=left><b><font  size=\"-1\"> CPU Type :</b>
                     </td><td colspan=1>\n";
              print "<select name=\"QCPUType\" >\n";
               my %hash=%{$self->{cputypes}};
               my @keysa=sort {$hash{$b} <=>$hash{$a}} keys %{$self->{cputypes}};
              foreach my $cputype (@keysa) {
                  print "<option value=\"$cputype\">$cputype </option>\n";
              }
              print "</select>\n";
              htmlTextField("CPU clock","number",10,1000,"QCPU"," [MHz]");  
              htmlTableEnd();
# Job Parameters
          print "<tr><td><b><font color=\"blue\">Job Parameters</font><font color=\"black\">
                 <i> (if in italic then cannot be changed)</font></b></i> \n";
          print "</td><td>\n";
          print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
          print "<tr valign=middle><td align=left><b><font size=\"-1\"> Particle : </b></td> <td colspan=1>\n";
          print "<select name=\"QPart\" >\n";
              my $ts=$self->{tsyntax};
              %hash=%{$ts->{particles}};
              @keysa=sort {$hash{$a} <=>$hash{$b}} keys %{$ts->{particles}};
              foreach my $particle (@keysa) {
                print "<option value=\"$particle\">$particle </option>\n";
              }
          print "</select>\n";
          print "</b></td></tr>\n";
            htmlTextField("Momentum min","number",7,1.,"QMomI","[GeV/c]");  
            htmlTextField("Momentum max","number",7,200.,"QMomA","[GeV/c]");  
            htmlTextField("Total Events","number",9,1000000.,"QEv"," ");  
            htmlTextField("Total Runs","number",7,3.,"QRun"," ");  

            my ($rid,$rndm1,$rndm2) = $self->getrndm();
            htmlHiddenTextField("rid","hidden",12,$rid,"QRNDMS"," ");  
            htmlHiddenTextField("rndm1","hidden",12,$rndm1,"QRNDM1"," ");  
            htmlHiddenTextField("rndm2","hidden",12,$rndm2,"QRNDM2"," ");  

            htmlText("<i>rndm sequence number </i>",abs($rid));
            htmlText("<i>rndm1 </i>",$rndm1);
            htmlText("<i>rndm2 </i>",$rndm2);

            htmlTextField("Begin Time","text",8,"01062005","QTimeB"," (ddmmyyyy)");  

            htmlTextField("End Time","text",8,"01062008","QTimeE"," (ddmmyyyy)");  
           htmlTableEnd();
            print "<tr><td><b><font color=\"green\">Automatic DST files transfer to Server</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
            if ($self->{CCT} eq "remote") {
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"L\" CHECKED><b> No </b><BR>\n";
            } 

else {
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"R\" ><b> Yes </b><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"L\" CHECKED><b> No </b><BR>\n";
            }
            print "</b></font></td></tr>\n";
           htmlTableEnd();
            if ($self->{CCT} eq "remote") {
             print "<tr><td>\n";
             print "<b><font color=\"green\">MC Production Mode</font></b><BR>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"Yes\" CHECKED><b> Standalone </b><BR>\n";
             print "</b></font></td></tr>\n";
           htmlTableEnd();
         }
# Script Custom/Generic
             print "<tr><td><b><font color=\"green\">Script  </font></b>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"JST\" VALUE=\"C\" ><b> Custom </b><i> (specific for for cite <font color=\"green\"> $self->{CCA} </font>)</i><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"JST\" VALUE=\"G\" CHECKED><b> Generic </b><i> (AMS generic)</i><BR>\n";
             print "</b></font></td></tr>\n";
            htmlTableEnd();


          htmlTableEnd();
             if ($self->{CCT} ne "remote") {
               my $ntdir    =undef; # directory path to store ntuples
               my $maxavail = 0;    # max available disk space
               foreach my $fs (@{$self->{FilesystemT}}){
                 if ($fs->{available} > $maxavail) {
                   $maxavail = $fs->{available};
                   $ntdir = $fs->{disk}.$fs->{path};
                   if ($fs->{disk} =~ /vice/) {
                    $ntdir = $fs->{path};
                   }
                 }
               }
DDTAB:         $self->htmlTemplateTable(" ");
               print "<tr><td>\n";
               print "<b><font color=\"green\">Ntuples Output Path</font></b><BR>\n";
               print "</tr></td><td>\n";
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<tr><td><font size=\"-1\"<b>\n";
               htmlTextField(" ","text",80,$ntdir,"NTDIR"," ");  
              print "</b></font></td></tr>\n";
            htmlTableEnd();
            htmlTableEnd();
           }


            print "<INPUT TYPE=\"hidden\" NAME=\"CEM\" VALUE=$cem>\n"; 
            print "<INPUT TYPE=\"hidden\" NAME=\"DID\" VALUE=-1>\n"; 
          print "<br>\n";
          print "<input type=\"submit\" name=\"BasicQuery\" value=\"Submit Request\"></br><br>        ";
          htmlReturnToMain();
          htmlFormEnd();
         htmlBottom();
         } elsif($self->{q}->param("CTT") eq "Advanced"){
             htmlTop();
             $self->htmlTemplateTable("Select Parameters for Advanced Request");
# Job Nick Name
          print "<tr><td><b><font color=\"red\">Job Nick Name</font></b>\n";
          print "</td><td>\n";
          print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
          htmlTextField("Nick Name","text",24,"MC02-advanced","QNick"," ");  
          print "</TABLE>\n";
# Cite Parameters
              print "<tr><td><b><font color=\"blue\">Cite HW Parameters</font></b>\n";
              print "</td><td>\n";
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
              print "<tr valign=middle><td align=left><b><font  size=\"-1\"> CPU Type :</b>
                     </td><td colspan=1>\n";
              print "<select name=\"QCPUType\" >\n";
               my %hash=%{$self->{cputypes}};
               my @keysa=sort {$hash{$b} <=>$hash{$a}} keys %{$self->{cputypes}};
              foreach my $cputype (@keysa) {
                  print "<option value=\"$cputype\">$cputype </option>\n";
              }
              print "</select>\n";
              htmlTextField("CPU clock","number",10,1000,"QCPU"," [MHz]");  
              htmlTableEnd();
# Job Parameters
              print "<tr><td><b><font color=\"blue\">Job Parameters</font></b>\n";
              print "</td><td>\n";
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Particle : </b></td> <td colspan=1>\n";
              print "<select name=\"QPart\" >\n";
              my $ts=$self->{tsyntax};
              %hash=%{$ts->{particles}};
              @keysa=sort {$hash{$a} <=>$hash{$b}} keys %{$ts->{particles}};
              foreach my $particle (@keysa) {
                print "<option value=\"$particle\">$particle </option>\n";
              }
              print "</select>\n";
              print "</b></td></tr>\n";
              htmlTextField("Momentum min","number",7,1.,"QMomI","[GeV/c]");  
              htmlTextField("Momentum max","number",7,200.,"QMomA","[GeV/c]");  
              htmlTextField("Total Events","number",12,1000000.,"QEv"," ");  
              htmlTextField("Total Runs","number",12,3.,"QRun"," ");  
              my ($rid,$rndm1,$rndm2) = $self->getrndm();

              htmlHiddenTextField("rid","hidden",12,$rid,"QRNDMS"," ");  
              htmlHiddenTextField("rndm1","hidden",12,$rndm1,"QRNDM1"," ");  
              htmlHiddenTextField("rndm2","hidden",12,$rndm2,"QRNDM2"," ");  

              htmlText("<i>rndm sequence number </i>",abs($rid));
              htmlText("<i>rndm1 </i>",$rndm1);
              htmlText("<i>rndm2 </i>",$rndm2);

              htmlTextField("Begin Time","text",11,"01062005","QTimeB"," (ddmmyyyy)");  
              htmlTextField("End Time","text",11,"01062008","QTimeE"," (ddmmyyyy)");  
              htmlTextField("Setup","text",20,"AMS02","QSetup"," ");
              htmlTextField("Trigger Type ","text",20,"AMSParticle","QTrType"," ");
           htmlTableEnd();

            $self->printJobParamFormatDST();

            print "<tr><td><b><font color=\"green\">DST file write mode</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
              print "<tr><td><font size=\"-1\"<b>\n";
              print "<INPUT TYPE=\"radio\" NAME=\"RNO\" VALUE=\"102\" CHECKED><b> Used Objects Only </b>\n";
              print "<INPUT TYPE=\"radio\" NAME=\"RNO\" VALUE=\"101\" ><b> Objects </b><BR>\n";
            print "</b></font></td></tr>\n";
           htmlTableEnd();
            if ($self->{CCT} eq "remote") {
             print "<tr><td>\n";
             print "<b><font color=\"green\">MC Production Mode</font></b><BR>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"Yes\" CHECKED><b> Standalone </b><BR>\n";
             print "</b></font></td></tr>\n";
           htmlTableEnd();
         }

            $self->printJobParamTransferDST();

# Script Custom/Generic
             print "<tr><td><b><font color=\"green\">Script  </font></b>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"JST\" VALUE=\"C\" ><b> Custom </b><i> (specific for for cite <font color=\"green\"> $self->{CCA} </font>)</i><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"JST\" VALUE=\"G\" CHECKED><b> Generic </b><i> (AMS generic)</i><BR>\n";
             print "</b></font></td></tr>\n";
            htmlTableEnd();
            print "<tr><td><b><font color=\"red\">Spectrum and Focusing</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            $ts=$self->{tsyntax};
            @keysa=sort keys %{$ts->{spectra}};
              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Input Spectrum : </b></td> <td colspan=1>\n";
              print "<select name=\"QSpec\" >\n";
              foreach my $spectrum (@keysa) {
                print "<option value=\"$spectrum\">$spectrum </option>\n";
              }
              print "</select>\n";
              print "</b></td></tr>\n";
           @keysa=sort keys %{$ts->{focus}};
              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Focusing : </b></td> <td colspan=1>\n";
              print "<select name=\"QFocus\" >\n";
              foreach my $focus (@keysa) {
                print "<option value=\"$focus\">$focus </option>\n";
              }
              print "</select>\n";
              print "</b></td></tr>\n";
           htmlTableEnd();
            print "<tr><td><b><font color=\"magenta\">GeoMagnetic Cutoff</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"GCF\" VALUE=\"1\" CHECKED><b> Yes </b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"GCF\" VALUE=\"0\" ><b> No </b><BR>\n";
             print "</b></font></td></tr>\n";
           htmlTableEnd();
            print "<tr><td><b><font color=\"blue\">Cube Coordinates</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td width=\"30%\"><font size=\"2\">\n";
            print "<b> Min : X : <input type=\"number\" size=5 value=-195 name=\"QXL\"></td>\n";  
            print "<td width=\"30%\"><font size=\"2\">\n";
            print "<b> Y : <input type=\"number\" size=5 value=-195 name=\"QYL\"></td>\n";  
            print "<td width=\"30%\"><font size=\"2\">\n";
            print "<b> Z : <input type=\"number\" size=5 value=-195 name=\"QZL\"> (cm)</td>\n";  
            print "</b></font></tr>\n";
            print "<tr><td width=\"30%\"><font size=\"2\">\n";
            print "<b> Max : X : <input type=\"number\" size=5 value=-195 name=\"QXU\"></td>\n";  
            print "<td width=\"30%\"><font size=\"2\">\n";
            print "<b> Y : <input type=\"number\" size=5 value=-195 name=\"QYU\"></td>\n";  
            print "<td width=\"30%\"><font size=\"2\">\n";
            print "<b> Z : <input type=\"number\" size=5 value=-195 name=\"QZU\"> (cm)</td>\n";  
            print "</b></font></tr>\n";
            htmlTextField("Cos Theta Max ","number",5,0.25,"QCos"," ");
            @keysa=sort keys %{$ts->{planes}};
              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Cube Surface Generation : </b></td> <td colspan=1>\n";
              print "<select name=\"QPlanes\" >\n";
              foreach my $surface (@keysa) {
                print "<option value=\"$surface\">$surface </option>\n";
              }
              print "</select>\n";
              print "</b></td></tr>\n";
           htmlTableEnd();
          htmlTableEnd();
             if ($self->{CCT} ne "remote") {
               my $ntdir="undef";
               my $maxavail=0;
               foreach my $fs (@{$self->{FilesystemT}}){
                 if ($fs->{available} > $maxavail) {
                   $maxavail = $fs->{available};
                   $ntdir = $fs->{disk}.$fs->{path};
                   if ($fs->{disk} =~ /vice/) {
                    $ntdir = $fs->{path};
                   }
                 }
              }
DDTAB:          $self->htmlTemplateTable(" ");
               print "<tr><td>\n";
               print "<b><font color=\"green\">Ntuples Output Path</font></b><BR>\n";
               print "</tr></td><td>\n";
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<tr><td><font size=\"-1\"<b>\n";
               htmlTextField(" ","text",80,$ntdir,"NTDIR"," ");  
              print "</b></font></td></tr>\n";
            htmlTableEnd();
           htmlTableEnd();
           }
            print "<INPUT TYPE=\"hidden\" NAME=\"CEM\" VALUE=$cem>\n"; 
            print "<INPUT TYPE=\"hidden\" NAME=\"DID\" VALUE=0>\n"; 
          print "<br>\n";
#          print "<input type=\"submit\" name=\"AdvancedForm\" value=\"Continue\"></br><br>        ";
          print "<input type=\"submit\" name=\"AdvancedQuery\" value=\"Submit Request\"></br><br>        ";
          htmlReturnToMain();
          htmlFormEnd();
         htmlBottom();
         } 
# Advanced Query ends here
          else{
           my $query=$self->{q}->param("CTT");
           my $found=0;
           my @tempnam=();
           my $hash={};
           my @desc=();
           my $cite={};
           foreach my $dataset (@{$self->{DataSetsT}}){
             if($dataset->{name} eq $query){
               $found=1;
               if(not $self->{CCR}){
                 $self->{FinalMessage}=" Sorry Only Cite Responsible Is allowed to Request Production DataSets";
               }
                 else{                   
                 foreach $cite (@{$dataset->{jobs}}){
                 if(not ($cite->{filename} =~/^\./)){
                  push @tempnam, $cite->{filename};
                  $hash->{$cite->{filename}}=$cite->{filedesc};
                  push @desc, $hash -> {$cite->{filename}}=$cite->{filedesc};
              }
             }
             htmlTop();
             $self->htmlTemplateTable("Select Parameters for Dataset Request");
# Job Template
             print "<tr><td><b><font color=\"red\">Job Template</font></b>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr valign=middle><td align=left><b><font size=\"-1\"> Dataset : </b></td> <td colspan=1>\n";
             print "<select name=\"QTemp\" >\n";
             my $i=0;
             foreach my $template (@tempnam) {
              print "<option value=\"$template\">$desc[$i] </option>\n";
              $i++;
            }
            print "</select>\n";
            print "</b></td></tr>\n";
            htmlTextField("Nick Name","text",24,"MC02-dataset","QNick"," ");  
            print "</TABLE>\n";
# Cite Parameters
              print "<tr><td><b><font color=\"blue\">Cite HW Parameters</font></b>\n";
              print "</td><td>\n";
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
              print "<tr valign=middle><td align=left><b><font  size=\"-1\"> CPU Type :</b>
                     </td><td colspan=1>\n";
              print "<select name=\"QCPUType\" >\n";
               my %hash=%{$self->{cputypes}};
               my @keysa=sort {$hash{$a} <=>$hash{$b}} keys %{$self->{cputypes}};
              foreach my $cputype (@keysa) {
                  print "<option value=\"$cputype\">$cputype </option>\n";
              }
              print "</select>\n";
              if ($self->{CCT} eq "remote") {
               htmlTextField("CPU clock","number",8,100,"QCPU"," [MHz]");  
              } else {
               htmlTextField("CPU clock","number",8,1000,"QCPU"," [MHz]");  
              } 
            htmlTableEnd();
# Job Parameters
              print "<tr><td><b><font color=\"blue\">Job Parameters</font></b>\n";
              print "</td><td>\n";
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
              $q->param("QEv",0);
              htmlTextField("CPU Time Limit Per Job","number",9,80000,"QCPUTime"," seconds (Native).");  
              htmlTextField("Total Jobs Requested","number",7,5.,"QRun"," ");  
                 if($self->{CCT} eq "local"){
   print qq`
<INPUT TYPE="checkbox" NAME="ForceCpuLimit" VALUE="FCL" >Force CPULimit<BR> 
`;
}
                 else{
   print qq`
<INPUT TYPE="checkbox" NAME="ForceCpuLimit" VALUE="FCL" CHECKED>Force CPULimit (Untick for NON-BATCH jobs)<BR> 
`;
                 }
              htmlTextField("Total  Real Time Required","number",3,15,"QTimeOut"," (days)");  
              my ($rid) = $self->getRID();
              htmlHiddenTextField("rid","hidden",12,$rid,"QRNDMS"," ");  
              htmlText("<i>rndm sequence number </i>",abs($rid));
            htmlTableEnd();
            if ($self->{CCT} eq "remote") {
             print "<tr><td>\n";
             print "<b><font color=\"green\">MC Production Mode</font></b><BR>\n";

             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"Yes\" ><b> Standalone </b><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"No\" CHECKED><b> Client </b><BR>\n";
             print "</b></font></td></tr>\n";
           htmlTableEnd();
         }

         $self->printJobParamFormatDST();
         $self->printJobParamTransferDST();

# Script Custom/Generic
             print "<tr><td><b><font color=\"green\">Script  </font></b>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"JST\" VALUE=\"C\" ><b> Custom </b><i> (specific for for cite <font color=\"green\"> $self->{CCA} </font>)</i><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"JST\" VALUE=\"G\" CHECKED><b> Generic </b><i> (AMS generic)</i><BR>\n";
             print "</b></font></td></tr>\n";
            htmlTableEnd();
           htmlTableEnd();
             if ($self->{CCT} ne "remote") {
               my $ntdir="Not Defined";
               my $maxavail=0;
               foreach my $fs (@{$self->{FilesystemT}}){
                if ($fs->{available} > $maxavail) {
                   $maxavail = $fs->{available};
                   $ntdir = $fs->{disk}.$fs->{path};
                   if ($fs->{disk} =~ /vice/) {
                    $ntdir = $fs->{path};
                   }
                 }
               }
DDTAB:         $self->htmlTemplateTable(" ");
               print "<tr><td>\n";
               print "<b><font color=\"green\">Ntuples Output Path</font></b><BR>\n";
               print "</tr></td><td>\n";
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<tr><td><font size=\"-1\"<b>\n";
               htmlTextField(" ","text",80,$ntdir,"NTDIR"," ");  
              print "</b></font></td></tr>\n";
            htmlTableEnd();
           htmlTableEnd();
           }
           print "<INPUT TYPE=\"hidden\" NAME=\"CEM\" VALUE=$cem>\n"; 
           print "<INPUT TYPE=\"hidden\" NAME=\"DID\" VALUE=$dataset->{did}>\n"; 
           print "<br>\n";
           print "<input type=\"submit\" name=\"ProductionQuery\" value=\"Submit Request\"></br><br>        ";
           print "<b><a href=load.cgi?$self->{UploadsHREF}/Help.txt target=\"_blank\">H E L P </b>\n";
           htmlReturnToMain();
           htmlBottom();
             }
            last;
           }
         }
         if(not $found){
          $self->{FinalMessage}=" Sorry  $query Query Not Yet Implemented";     
         }
       }
      }
    }
#MyQuery ends here    
    if ($self->{q}->param("MyQuery")){
        $self->{read}=1;
        my $cem=lc($self->{q}->param("CEM"));
        if (not $self->findemail($cem)){
            $self->ErrorPlus("Client $cem Not Found. Please Register.");
        }
        elsif($self->{CEMA} ne "Active"){
            $self->ErrorPlus("Welcome $cem. Your account is not yet set up.
            Please try again later.");
        }
        elsif($self->{q}->param("CTT")){
#...
            if($self->{q}->param("CTT") eq "Basic"){
                print $q->header( "text/html" ),
                $q->start_html( "Welcome");
                print $q->h1( "Welcome   $self->{CEM}. How are you today?");
                print $q->start_form(-method=>"GET", 
                -action=>$self->{Name});
print qq`
         <INPUT TYPE="hidden" NAME="CEM" VALUE=$cem> 
         <INPUT TYPE="hidden" NAME="DID" VALUE=0> 
`;
         print "<BR>Basic Templates";
         my @tempnam=();
             my $hash={};
         foreach my $cite (@{$self->{TempT}}){
             if(not ($cite->{filename} =~/^\./)){
              push @tempnam, $cite->{filename};
              $hash->{$cite->{filename}}=$cite->{filedesc};
          }
         }
         print $q->popup_menu(
          -name=>"QTemp",
          -values=>\@tempnam,
          -default=>$tempnam[0]);
#          -labels=>$hash);
         my $ts=$self->{tsyntax};
         my %hash=%{$ts->{particles}};
         my @keysa=sort {$hash{$a} <=>$hash{$b}} keys %{$ts->{particles}};
         print "Particle";
         print $q->popup_menu(
          -name=>"QPart",
          -values=>\@keysa,
          -default=>$keysa[0]);
          print "<BR>";
          print "Min Momentum (GeV/c)";
          print $q->textfield(-name=>"QMomI",-default=>0.2);
                print "Max Momentum (GeV/c)";
          print $q->textfield(-name=>"QMomA",-default=>200.);
          print "<BR>";
                print "Remote Computer Clock (MhZ)";
          print $q->textfield(-name=>"QCPU",-default=>1000);
          print "<BR>";
                print "Total Number of Events ";
          print $q->textfield(-name=>"QEv",-default=>1000000);
                print "Number of Runs (Max 100)";
          print $q->textfield(-name=>"QRun",-default=>3);
            my ($rid,$rndm1,$rndm2) = $self->getrndm();
          print "<BR>";
                print "RNDM1 ";
          print $q->textfield(-name=>"QRNDM1",-default=>$rndm1);
                print "RNDM2 ";
          print $q->textfield(-name=>"QRNDM2",-default=>$rndm2);
          print "<BR>";
                print "TimeBegin (ddmmyyyy) ";
          print $q->textfield(-name=>"QTimeB",-default=>"01062005");
                print "TimeEnd (ddmmyyyy) ";
          print $q->textfield(-name=>"QTimeE",-default=>"01062008");
          print "<BR>";
         print "Automatic NtupleFile Transfer to Server";
          print "<BR>";
   print qq`
<INPUT TYPE="radio" NAME="AFT" VALUE="R" CHECKED>Yes<BR>
<INPUT TYPE="radio" NAME="AFT" VALUE="L" >No<BR>
`;
          print "<BR>";
          print $q->submit(-name=>"BasicQuery", -value=>"Submit");
          print $q->reset(-name=>"Reset");
            }
            elsif($self->{q}->param("CTT") eq "Advanced"){
                print $q->header( "text/html" ),
                $q->start_html( "Welcome");
                print $q->h1( "Welcome   $self->{CEM}. How are you today?");
                print $q->start_form(-method=>"GET", 
                -action=>$self->{Name});
print qq`
         <INPUT TYPE="hidden" NAME="CEM" VALUE=$cem> 
         <INPUT TYPE="hidden" NAME="DID" VALUE=0> 
`;
         my $ts=$self->{tsyntax};
         my %hash=%{$ts->{particles}};
         my @keysa=sort {$hash{$a} <=>$hash{$b}} keys %{$ts->{particles}};
         print "Particle";
         print $q->popup_menu(
          -name=>"QPart",
          -values=>\@keysa,
          -default=>$keysa[0]);
          print "<BR>";
          print "Min Momentum (GeV/c)";
          print $q->textfield(-name=>"QMomI",-default=>1);
                print "Max Momentum (GeV/c)";
          print $q->textfield(-name=>"QMomA",-default=>200.);
          print "<BR>";
                print "Remote Computer Clock (MhZ)";
          print $q->textfield(-name=>"QCPU",-default=>1000);
          print "<BR>";
                print "Total Number of Events ";
          print $q->textfield(-name=>"QEv",-default=>1000000);
                print "Number of Jobs (Max 100)";
          print $q->textfield(-name=>"QRun",-default=>3);
            my ($rid,$rndm1,$rndm2) = $self->getrndm();
          print "<BR>";
                print "RNDM1 ";
          print $q->textfield(-name=>"QRNDM1",-default=>$rndm1);
                print "RNDM2 ";
          print $q->textfield(-name=>"QRNDM2",-default=>$rndm2);
          print "<BR>";
                print "TimeBegin (ddmmyyyy) ";
          print $q->textfield(-name=>"QTimeB",-default=>"01062005");
                print "TimeEnd (ddmmyyyy) ";
          print $q->textfield(-name=>"QTimeE",-default=>"01062008");
          print "<BR>";
                print "Setup ";
          print $q->textfield(-name=>"QSetup",-default=>"AMS02");
                print "Trigger Type ";
          print $q->textfield(-name=>"QTrType",-default=>"AMSParticle");
          print "<BR>";
                print "Output file Type ";
          print "<BR>";
   print qq`
<INPUT TYPE="radio" NAME="RootNtuple" VALUE="1=3 168=120000000 2=" $defNTUPLE>Ntuple
<INPUT TYPE="radio" NAME="RootNtuple" VALUE="1=0 168=500000000 127=2 128=" $defROOT>RootFile<BR>
`;
                print "Root/Ntuple Write Mode ";
          print "<BR>";
   print qq`
<INPUT TYPE="radio" NAME="RNO" VALUE="102" CHECKED>Used Objects Only
<INPUT TYPE="radio" NAME="RNO" VALUE="101" >All Objects<BR>
`;
                print "Automatic Ntuples/RootFiles Transfer to Server";
          print "<BR>";
   print qq`
<INPUT TYPE="radio" NAME="AFT" VALUE="R" CHECKED>Yes
<INPUT TYPE="radio" NAME="AFT" VALUE="L" >No<BR>
`;
          $ts=$self->{tsyntax};
          @keysa=sort keys %{$ts->{spectra}};
         print "Input Spectrum ";
         print $q->popup_menu(
          -name=>"QSpec",
          -values=>\@keysa,
          -default=>$keysa[0]);
          @keysa=sort keys %{$ts->{focus}};
         print "Focusing ";
         print $q->popup_menu(
          -name=>"QFocus",
          -values=>\@keysa,
          -default=>$keysa[0]);
          print "<BR>";
          print "Cos_Theta_Max ";
          print $q->textfield(-name=>"QCos",-default=>"0.25");
          @keysa=sort keys %{$ts->{planes}};
         print "Cube Surfaces Generation ";
         print $q->popup_menu(
          -name=>"QPlanes",
          -values=>\@keysa,
          -default=>$keysa[0]);
          print "<BR>";
         print "Geomagentic Cutoff";
          print "<BR>";
   print qq`
<INPUT TYPE="radio" NAME="GCF" VALUE="1" CHECKED>Yes
<INPUT TYPE="radio" NAME="GCF" VALUE="0" >No<BR>
`;
          print "Cube min X,Y,Z coo (cm)";
          print $q->textfield(-name=>"QXL",-default=>"-195");
          print $q->textfield(-name=>"QYL",-default=>"-195");
          print $q->textfield(-name=>"QZL",-default=>"-195");
          print "<BR>";
          print "Cube max X,Y,Z coo (cm)";
          print $q->textfield(-name=>"QXU",-default=>"195");
          print $q->textfield(-name=>"QYU",-default=>"195");
          print $q->textfield(-name=>"QZU",-default=>"195");

          print "<BR>";
          print $q->submit(-name=>"AdvancedQuery", -value=>"Submit");
          print $q->reset(-name=>"Reset");
# Advanced form ends here
            } 
          else{
            
              my $query=$self->{q}->param("CTT");
              my $found=0;
           
         foreach my $dataset (@{$self->{DataSetsT}}){
             if($dataset->{name} eq $query){
                 $found=1;
                 if(not $self->{CCR}){
                   $self->{FinalMessage}=" Sorry Only Cite Responsible Is allowed to Request Production DataSets";
                 }
                 else{                   
                print $q->header( "text/html" ),
                $q->start_html( "Welcome");
                print $q->h1( "Welcome   $self->{CEM}. How are you today?");
                print $q->start_form(-method=>"GET", 
                -action=>$self->{Name});
print qq`
         <INPUT TYPE="hidden" NAME="CEM" VALUE=$cem> 
         <INPUT TYPE="hidden" NAME="DID" VALUE=$dataset->{did}> 
`;
              print "<BR>Templates";
         my @tempnam=();
             my $hash={};
         foreach my $cite (@{$dataset->{jobs}}){
             if(not ($cite->{filename} =~/^\./)){
              push @tempnam, $cite->{filename};
              $hash->{$cite->{filename}}=$cite->{filedesc};
          }
         }
         print $q->popup_menu(
          -name=>"QTemp",
          -values=>\@tempnam,
          -default=>$tempnam[0],
          -labels=>$hash);
          print "<BR>";
                print "Remote Computer Clock (MhZ)";
          print $q->textfield(-name=>"QCPU",-default=>1000);
          print "<BR>";
                print "Total Number of Events ";
          print $q->textfield(-name=>"QEv",-default=>1000000);
                print "Number of Runs (Max 100)";
          print $q->textfield(-name=>"QRun",-default=>3);
          print "<BR>";
                print "Approximate Jobs Total Elapsed Time (days)";
          print $q->textfield(-name=>"QTimeOut",-default=>15);
          print "<BR>";
         print "Automatic NtupleFile Transfer to Server";
          print "<BR>";
   print qq`
<INPUT TYPE="radio" NAME="AFT" VALUE="R" CHECKED>Yes<BR>
<INPUT TYPE="radio" NAME="AFT" VALUE="L" >No<BR>
`;

          print "<BR>";
          print $q->submit(-name=>"ProductionQuery", -value=>"Submit");
          print $q->reset(-name=>"Reset");
            }
                 last;
             }
         }
              if(not $found){
            $self->{FinalMessage}=" Sorry  $query Query Not Yet Implemented";     
        }
          }
        }
    }
#MyQuery ends here
    if ($self->{q}->param("BasicQuery")      or 
        $self->{q}->param("AdvancedQuery")   or 
        $self->{q}->param("ProductionQuery") or
        $self->{q}->param("BasicForm")       or 
        $self->{q}->param("AdvancedForm")    or 
        $self->{q}->param("ProductionForm")){
        $self->{read}=1;
#  check par
        my $cem=lc($q->param("CEM"));
        if (not $self->findemail($cem)){
            $self->ErrorPlus("Client $cem Not Found. Please Register.");
        }
        elsif($self->{CEMA} ne "Active"){
            $self->ErrorPlus("Welcome $cem. Your account is not yet set up.
            Please try again later.");
        }
        if($self->{q}->param("BasicQuery") eq "Save"     or 
           $self->{q}->param("AdvancedQuery") eq "Save"  or 
           $self->{q}->param("ProductionQuery") eq "Save"){
            my $pass=$q->param("password");
            if($self->{CCT} ne "remote" or defined $pass){
             my $crypt=crypt($pass,"ams");
             if($crypt ne "amGzkSRlnSMUU"){
              $self->sendmailerror("User authorization failed","$self->{CEM}");
              $self->ErrorPlus
                    ("User Authorization Failed. All Your Activity is Logged.");

             }
         }
         my $filename=$q->param("FEM");
         $q=get_state($filename);          
         if(not defined $q){
          $self->ErrorPlus("Save State Expired. Please Start From the Very Beginning");
         }
         $self->{q}=$q;
         unlink $filename;
        }
         my $sonly="No";
         $sonly=$q->param("SONLY");
# Apr 2. 2003. a.k. uncommented/commented back
#         if ($sonly eq "Yes") {
#             $self->{scriptsOnly}=0;
#             $self->{senddb}=1;
#             $self->{sendaddon}=1;
#         }
#
# Second file for download only in StanAlone mode
        my $stalone = $q->param("STALONE");
        if ($stalone eq "Yes") {
          $self->{dwldaddon}=1;
        } else {
          $self->{dwldaddon}=0;
        }
#
# 'C'/'G' - custom, generic
#
        my $jst=$q->param("JST");
#
        my $aft=$q->param("AFT");
        my $templatebuffer=undef;
        my $template=undef;
        my $did=$q->param("DID");
        my $timeout=3600*24*35;
        my $dataset=undef;
        foreach my $ds (@{$self->{DataSetsT}}){
            if ($ds->{did}==$did){
                $dataset=$ds;
                last;
            }
        }
        if ($self->{q}->param("AdvancedQuery") ){
            $template='.Advanced.job';
        }
        else{
         $template=$q->param("QTemp");
        }
        if(defined $dataset){
         foreach my $tmp (@{$dataset->{jobs}}) {
            if($template eq $tmp->{filename}){
                $templatebuffer=$tmp->{filebody};
                my ($rid,$rndm1,$rndm2) = $self->getrndm($dataset);
                if(not defined $q->param("QRNDM1")){
                    $q->param("QRNDM1",$rndm1);
                }
                if(not defined $q->param("QRNDM2")){
                    $q->param("QRNDM2",$rndm2);
                }
                if(not defined $q->param("QTimeB")){
                    $q->param("QTimeB",$tmp->{TIMBEG});
                }
                if(not defined $q->param("QTimeE")){
                    $q->param("QTimeE",$tmp->{TIMEND});
                }
                if(not defined $q->param("QMomI")){
                    $q->param("QMomI",$tmp->{PMIN});
                }
                if(not defined $q->param("QMomA")){
                    $q->param("QMomA",$tmp->{PMAX});
                }
                if(not defined $q->param("QPart")){
                    $q->param("QPart",$tmp->{PART});
                }
                if(not defined $q->param("QPart")){
                    $q->param("QPart",$tmp->{PART});
                }
                if(not defined $q->param("QCPUPEREVENT")){
                    $q->param("QCPUPEREVENT",$tmp->{CPUPEREVENTPERGHZ});
                }
                my $evno=$q->param("QEv");
                if(not $evno =~/^\d+$/ or $evno <1 or $evno>$tmp->{TOTALEVENTS}){
                    my $corr=1;
                    if (defined $q->param("QCPUType")){
                        $corr=$self->{cputypes}->{$q->param("QCPUType")};
                    }
                    $evno=$q->param("QCPUTime")*$q->param("QCPU")/1000./$tmp->{CPUPEREVENTPERGHZ}*$corr;
                    if($evno<10000){
                       $evno=10000;
                    }
                    $evno=int($evno/1000)*1000*$q->param("QRun");     
                    if($evno>$tmp->{TOTALEVENTS}){
                        #  make runno correction 
                        my $evperrun=$evno/$q->param("QRun");     
                        $evno=$tmp->{TOTALEVENTS};
                        my $runno=int($evno/$evperrun+0.5);
                        if($runno le 0){
                            $runno=1;
                        }
                       $q->param("QRun",$runno);
                    }
                    $q->param("QEv",$evno);
                }
                last;
            }
        }
     }
        else{  
         foreach my $tmp (@{$self->{TempT}}){
            if($template eq $tmp->{filename}){
                $templatebuffer=$tmp->{filebody};
                last;
            }
         }
     }
        if(not defined $templatebuffer){
            $self->ErrorPlus("Could not find file for $template template.");
        }
        
        my $a=1;
        my $b=2147483647;
        my $rndm1=$q->param("QRNDM1");
        my $rndm2=$q->param("QRNDM2");
        if(not $rndm1 =~/^\d+$/ or $rndm1 <$a or $rndm1>$b){
             $self->ErrorPlus("RNDM1 $rndm1 is out of range ($a,$b)");
         
        }
        if(not $rndm2 =~/^\d+$/ or $rndm2 <$a or $rndm2>$b){
             $self->ErrorPlus("RNDM2 $rndm1 is out of range ($a,$b)");
         
        }
        if ($self->{CCT} ne "remote") {
         my $ntdir=$q->param("NTDIR");
         if (not defined $ntdir) {
             $self->ErrorPlus("The NTuples output directory NOT DEFINED");
         } else {
             if (not $ntdir =~ /\//  ) {
              $self->ErrorPlus("Invalid NTuples output directory : $ntdir");
            } else {
                $self->{AMSDSTOutputDir}=$ntdir;
            }
         }
        }
         my $pmin=$q->param("QMomI");
          my $pmax=$q->param("QMomA");
        if(not $pmin =~/^-?(?:\d+(?:\.\d*)?|\.\d+)$/){
             $self->ErrorPlus("pmin $pmin is not a float number ");
        }
        if(not $pmax =~/^-?(?:\d+(?:\.\d*)?|\.\d+)$/){
             $self->ErrorPlus("pmax $pmax is not a float number ");
        }
#randomize pmin
        if($pmin>$pmax){
             $self->ErrorPlus("pmin $pmin greater than pmax $pmax ");
        }
                
        my $cput=$q->param("QCPU");
        if(not $cput =~/^-?(?:\d+(?:\.\d*)?|\.\d+)$/){
             $self->ErrorPlus("Computer Clock $cput  is not a  number ");
        }
        if($cput < 300 ){
             $self->ErrorPlus("Computer Clock $cput  is too low for the AMS02 MC ");
        }
        my $clock=$cput;
        if($cput > 3000 ){
            $cput=3000;
        }
        my $corr=1;
            if (defined $q->param("QCPUType")){
              $corr=$self->{cputypes}->{$q->param("QCPUType")};
            }

        $cput=50+$pmax*1000/$cput/$corr;
        if($cput >7200){
         $cput=7200;
        } 
        
        my $evno=$q->param("QEv");
        my $runno=$q->param("QRun");
        if(not $evno =~/^\d+$/ or $evno <$a or $evno>$b){
             $self->ErrorPlus("Events no $evno is out of range ($a,$b)");
        }
        if(not $runno =~/^\d+$/ or $runno <$a or $runno>100){
             $self->ErrorPlus("Runs no $runno is out of range ($a,100)");
        }
         
        if($evno < $runno){
             $self->ErrorPlus("Runs no $runno greater than events no $evno");
         }
        my $evperrun=int ($evno/$runno);
        if($evperrun > (1<<31)-1){
            $self->ErrorPlus('EventsPerRun Exceeds 2^31-1 :'."$evperrun");
        } 
        my $lastrunev=$evno-$evperrun*($runno-1);
        my ($particleid,$timbeg,$timend,$timbegu,$timendu);
         $timbeg=$q->param("QTimeB");
         $timend=$q->param("QTimeE");
        if(not $timbeg =~/^\d+$/ ){
             $self->ErrorPlus("TimeBegin $timbeg is not an integer");
        }
        my $year=$timbeg%10000-1900;
        my $month=int($timbeg/10000)%100-1;
        my $date=int($timbeg/1000000)%100;
        if($year < 96 or $year > 110 or $month<0 or $month>11 or $date<1 or $date>31){
             $self->ErrorPlus("TimeBegin $timbeg is out of range $date $month $year");
        }
         $timbegu=timelocal(1,0,8,$date,$month,$year);
        if(not $timend =~/^\d+$/ ){
             $self->ErrorPlus("TimeEnd $timend is not an integer");
        }
        $year=$timend%10000-1900;
        $month=int($timend/10000)%100-1;
        $date=int($timend/1000000)%100;
        if($year < 96 or $year > 110 or $month<0 or $month>11 or $date<1 or $date>31){
             $self->ErrorPlus("TimeEnd $timend is out of range $date $month $year");
        }
         $timendu=timelocal(1,0,8,$date,$month,$year);
        if($self->{q}->param("ProductionQuery") or $self->{q}->param("ProductionForm")  ){
          $timeout=$q->param("QTimeOut");
          if(not $timeout =~/^-?(?:\d+(?:\.\d*)?|\.\d+)$/ or $timeout <1 or $timeout>40){
             $self->ErrorPlus("Time  $timeout is out of range (1,40) days. ");
          }
          $timeout=int($timeout*3600*24);
          $particleid= $q->param("QPart");
      }
        else{
#basic only
        my $particle=$q->param("QPart");
         $particleid=$self->{tsyntax}->{particles}->{$particle};
     }
# advanced only
        my ($setup,$trtype,$rootntuple,$rno,$spectrum,$focus,$cosmax,$geocutoff,$plane);
            my @cubes=();
        if($self->{q}->param("AdvancedQuery") or $self->{q}->param("AdvancedForm") ){
             $setup=$q->param("QSetup");
            if((not $setup =~ '^AMS02') and (not $setup=~'^AMSSHUTTLE')){
             $self->ErrorPlus
             ("Setup $setup does not exist. Only AMS02xxx or AMSSHUTTLEyyy  setup names allowed.");
            }
             $trtype=$q->param("QTrType");
             $rootntuple=$q->param("RootNtuple");
             $rno=$q->param("RNO");
             $spectrum=$self->{tsyntax}->{spectra}->{$q->param("QSpec")};
             $focus=$self->{tsyntax}->{focus}->{$q->param("QFocus")};
            my $ctmax=$q->param("QCos");
        if(not $ctmax =~/^-?(?:\d+(?:\.\d*)?|\.\d+)$/){
             $self->ErrorPlus("cos_theta_max $ctmax is not a float number ");
        }
            if($ctmax>1 or $ctmax<-1){
             $self->ErrorPlus("cos_theta_max $ctmax is out of range ");
        }
           $cosmax=sprintf("%.3f",$ctmax);
             $plane=$self->{tsyntax}->{planes}->{$q->param("QPlanes")};
             $geocutoff=$q->param("GCF");
            my @cubesnames=("QXL","QYL","QZL","QXU","QYU","QZU");
            foreach my $cube (@cubesnames){
                my $value=$q->param($cube);
                if(not $value =~/^-?(?:\d+(?:\.\d*)?|\.\d+)$/){
                 $self->ErrorPlus("cube $value is not a float number ");
                }
                push @cubes, $value;
            }
            for my $i (0...2) {
                if ($cubes[$i] >$cubes[$i+3]){
                  $self->ErrorPlus("cubemin  $cubes[$i] greater than cubemax $cubes[$i+3] ");
                }
            }
         }
            #get run
        my $run=undef;
        my $sql="select maxrun from Cites where name='$self->{CCA}' and state=0";
        my $res=$self->{sqlserver}->Query($sql);
        if( not defined $res->[0][0]){
            my $mes="Cite $self->{CCA} does not exist or locked";
              foreach my $chop (@{$self->{MailT}}) {
              if($chop->{rserver}==1){
                  my $address=$chop->{address};
                  my $message=" see subject";
                  $self->sendmailmessage($address,$mes,$message);
                  last;
              }
            }          
            $self->ErrorPlus($mes);
        }
        else{
            my $time = time();
            $sql="update Cites set state=1, timestamp=$time where name='$self->{CCA}'";
            $self->{sqlserver}->Update($sql);
            $run=$res->[0][0];
        }
# 4.12.03             my $switch=1<<27;
             my $switch=1<<$MAX_RUN_POWER;
             my $max=$switch-1;    
             if (($run%$switch)+$runno >$max){
              foreach my $chop (@{$self->{MailT}}) {
              if($chop->{rserver}==1){
               my $address=$chop->{address};
               my $subject="AMS02MC Request Form: run Capacity Exceeded for Cite $self->{CCA} $run";
               my $message=" see subject";
               $self->sendmailmessage($address,$subject,$message);
               last;
              }
             }          
             $self->ErrorPlus("Run Capacity Exceeds.");
             return;
          }
             
#       now everything is o.k except server check

        if (not $self->ServerConnect()){
        foreach my $chop (@{$self->{MailT}}) {
              if($chop->{rserver}==1){
                  my $address=$chop->{address};
                  $self->sendmessage($address,"unable to connect to servers by $self->{CEM}"," "); 
                  last;
              }
          }
            $self->ErrorPlus("Unable to Connect to Server.");
         }
#        prepare the tables
    
# check tar ball exists
        my $key='gbatch';
        $sql="select myvalue from Environment where mykey='".$key."'";
        my $ret=$self->{sqlserver}->Query($sql);
        if( not defined $ret->[0][0]){
            $self->ErrorPlus("unable to retreive gbatch name from db");
        }
        my $gbatch=$ret->[0][0];
        my @stag=stat "$self->{AMSSoftwareDir}/$gbatch";
        if($#stag<0){
              $self->ErrorPlus("Unable to find gbatch-orbit on the Server ");
        }
        $key='getior';
        $sql="select myvalue from Environment where mykey='".$key."'";
        $ret=$self->{sqlserver}->Query($sql);
        if( not defined $ret->[0][0]){
            $self->ErrorPlus("unable to retreive getior name from db");
        }
         my $gr=$ret->[0][0];
         my @stag2=stat "$self->{AMSSoftwareDir}/$gr";
        if($#stag2<0){
              $self->ErrorPlus("Unable to find $gr on the Server ");
        }
        $key='ntuplevalidator';
        $sql="select myvalue from Environment where mykey='".$key."'";
        $ret=$self->{sqlserver}->Query($sql);
        if( not defined $ret->[0][0]){
            $self->ErrorPlus("unable to retreive ntuplevalidator name from db");
        }
         my $nv=$ret->[0][0];
         my @stag1=stat "$self->{AMSSoftwareDir}/$nv";
        if($#stag1<0){
              $self->ErrorPlus("Unable to find $nv on the Server ");
        }
        my $file2tar;
        my $filedb;
        my $filedb_att;
        if($self->{CCT} eq "remote"){
        $filedb="$self->{UploadsDir}/$self->{FileDB}";
        my @sta = stat $filedb;
        if($#sta<0 or $sta[9]-time() >86400*7 or $stag[9] > $sta[9] or $stag1[9] > $sta[9] or $stag2[9] > $sta[9]){
           $self->{senddb}=2;
        my $filen="$self->{UploadsDir}/ams02mcdb.tar.$run";
        $key='dbversion';
        $sql="select myvalue from Environment where mykey='".$key."'";
        my $ret=$self->{sqlserver}->Query($sql);
        if( not defined $ret->[0][0]){
            $self->ErrorPlus("unable to retreive db version from db");
        }
#
        $dbversion=$ret->[0][0];
#-           $dbversion="v3.00";
#
        my $i=system "mkdir -p $self->{UploadsDir}/$dbversion";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/*.dat $self->{UploadsDir}/$dbversion";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/t* $self->{UploadsDir}/$dbversion";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/T* $self->{UploadsDir}/$dbversion";
       if($dbversion=~/v4/){
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/ri* $self->{UploadsDir}/$dbversion";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/*.flux $self->{UploadsDir}/$dbversion";
       }
        $i=system "tar -C$self->{UploadsDir} -h -cf $filen $dbversion";
        if($i){
              $self->ErrorPlus("Unable to tar $self->{UploadsDir} $dbversion to $filen");
         }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $gbatch") ;
          if($i){
              $self->ErrorPlus("Unable to tar gbatch-orbit to $filen ");
          }
         $i=system("tar -C$self->{CERN_ROOT} -uf $filen lib/flukaaf.dat") ;
          if($i){
              $self->ErrorPlus("Unable to tar flukaaf.dat to $filen ");
          }

         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $nv") ;
          if($i){
              $self->ErrorPlus("Unable to tar $nv to $filen ");
          }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $gr") ;
          if($i){
              $self->ErrorPlus("Unable to tar $gr to $filen ");
          }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen prod/tnsnames.ora") ;
          if($i){
              $self->ErrorPlus("Unable to tar prod/tnsnames.ora to $filen ");
          }

          $i=system("gzip -f $filen");
          if($i){
           $self->ErrorPlus("Unable to gzip  $filen");
          }
          $i=system("mv $filedb $filedb.o");
          $i=system("mv $filen.gz $filedb");
          unlink "$filedb.o";
       }
        elsif($sta[9]>$self->{TU1}){
            $self->{senddb}=2;
        }
        $filedb_att="$self->{UploadsDir}/$self->{FileAttDB}";
        @sta = stat $filedb_att;
          
         my @stag3=stat "$self->{AMSDataDir}/DataBase";
        if($#stag3<0){
              $self->ErrorPlus("Unable to find $self->{AMSDataDir}/DataBase on the Server ");
        }

        if(($#sta<0 or $sta[9]-time() >86400*7  or $stag3[9] > $sta[9] ) and $self->{dwldaddon}==1){
           $self->{sendaddon}=2;
        my $filen="$self->{UploadsDir}/ams02mcdb.addon.tar.$run";
        my $i=system "mkdir -p $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/MagneticFieldMap $self->{UploadsDir}/DataBase";
#-           my $dbversion="v3.00";
           if($dbversion =~/v4/){
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Tracker*.2* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Tracker*2 $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Anti* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Tof*2 $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Tofpeds* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Tof*MS $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/.*0 $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/.TrA*1 $self->{UploadsDir}/DataBase"; 
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Ecal* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TRD* $self->{UploadsDir}/DataBase";
        $i=system "tar -C$self->{UploadsDir} -h -cf $filen DataBase";
    }
           else{
        $i=system "mkdir $self->{UploadsDir}/DataBase/TrackerCmnNoise";
        $i=system "mkdir $self->{UploadsDir}/DataBase/TrackerSigmas.r";
        $i=system "mkdir $self->{UploadsDir}/DataBase/TrackerSigmas.l";
        $i=system "mkdir $self->{UploadsDir}/DataBase/TrackerPedestals.r";
        $i=system "mkdir $self->{UploadsDir}/DataBase/TrackerPedestals.l";
        $i=system "mkdir $self->{UploadsDir}/DataBase/TrackerStatus.r";
        $i=system "mkdir $self->{UploadsDir}/DataBase/TrackerStatus.l";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerCmnNoise/895964400 $self->{UploadsDir}/DataBase/TrackerCmnNoise";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerCmnNoise/896828400/ $self->{UploadsDir}/DataBase/TrackerCmnNoise";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerCmnNoise/896914800/ $self->{UploadsDir}/DataBase/TrackerCmnNoise";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerCmnNoise/897260400/ $self->{UploadsDir}/DataBase/TrackerCmnNoise";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerSigmas.r/895964400/ $self->{UploadsDir}/DataBase/TrackerSigmas.r";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerSigmas.r/896828400/ $self->{UploadsDir}/DataBase/TrackerSigmas.r";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerSigmas.r/896914800/ $self->{UploadsDir}/DataBase/TrackerSigmas.r";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerSigmas.r/897260400/ $self->{UploadsDir}/DataBase/TrackerSigmas.r";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerSigmas.l/895964400/ $self->{UploadsDir}/DataBase/TrackerSigmas.l";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerSigmas.l/896828400/ $self->{UploadsDir}/DataBase/TrackerSigmas.l";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerSigmas.l/896914800/ $self->{UploadsDir}/DataBase/TrackerSigmas.l";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerSigmas.l/897260400/ $self->{UploadsDir}/DataBase/TrackerSigmas.l";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerPedestals.r/895964400/ $self->{UploadsDir}/DataBase/TrackerPedestals.r";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerPedestals.r/896828400/ $self->{UploadsDir}/DataBase/TrackerPedestals.r";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerPedestals.r/896914800/ $self->{UploadsDir}/DataBase/TrackerPedestals.r";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerPedestals.r/897260400/ $self->{UploadsDir}/DataBase/TrackerPedestals.r";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerPedestals.l/895964400/ $self->{UploadsDir}/DataBase/TrackerPedestals.l";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerPedestals.l/896828400/ $self->{UploadsDir}/DataBase/TrackerPedestals.l";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerPedestals.l/896914800/ $self->{UploadsDir}/DataBase/TrackerPedestals.l";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerPedestals.l/897260400/ $self->{UploadsDir}/DataBase/TrackerPedestals.l";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerStatus.r/895964400/ $self->{UploadsDir}/DataBase/TrackerStatus.r";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerStatus.r/896828400/ $self->{UploadsDir}/DataBase/TrackerStatus.r";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerStatus.r/896914800/ $self->{UploadsDir}/DataBase/TrackerStatus.r";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerStatus.r/897260400/ $self->{UploadsDir}/DataBase/TrackerStatus.r";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerStatus.l/895964400/ $self->{UploadsDir}/DataBase/TrackerStatus.l";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerStatus.l/896828400/ $self->{UploadsDir}/DataBase/TrackerStatus.l";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerStatus.l/896914800/ $self->{UploadsDir}/DataBase/TrackerStatus.l";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerStatus.l/897260400/ $self->{UploadsDir}/DataBase/TrackerStatus.l";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TrackerG* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Anti* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Tof* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/C* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/.*0 $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/.TrA*1 $self->{UploadsDir}/DataBase"; 
        $i=system "tar -C$self->{UploadsDir} -h -cf $filen DataBase";
           }
        if($i){
              $self->ErrorPlus("Unable to tar $self->{UploadsDir} DataBase to $filen");
         }
          $i=system("gzip -f $filen");
                      if($i){
              $self->ErrorPlus("Unable to gzip  $filen");
          }
          $i=system("mv $filedb_att $filedb_att.o");
          $i=system("mv $filen.gz $filedb_att");
          unlink "$filedb_att.o";
       }
        elsif($sta[9]>$self->{TU2}){
            $self->{sendaddon}=2;
        }

    
# write readme file
        my $readme="$self->{UploadsDir}/README.$run";
        open(FILE,">".$readme) or die "Unable to open file $readme\n";
        if($self->{dwldaddon}==1){
         print FILE  $self->{tsyntax}->{headers}->{readmestandalone};
        }
        else{
         print FILE  $self->{tsyntax}->{headers}->{readmecorba};
        }
        my $sql = "SELECT dirpath FROM journals WHERE cid=$self->{CCID}";
        my $note='please contact alexei.klimentov@cern.ch for details'; 
        my $ret = $self->{sqlserver}->Query($sql);
  
        if (defined $ret->[0][0]) {
         $note ="DST directory path : $ret->[0][0]/nt 
                \n Journal files directory path : $ret->[0][0]/jou 
                \n Validation log files directory path : $ret->[0][0]/log
                \n";
         }         
        print FILE  $note;
        close FILE;
         $file2tar="$self->{UploadsDir}/ams02mcscript.$run.tar";
        my $i=system("tar -C$self->{UploadsDir} -cf  $file2tar README.$run"); 
          if($i){
              $self->ErrorPlus("Unable to tar readme to $file2tar ");
          }
        unlink $readme;
    }
        
        for my $i (1 ... $runno){
         #find buffer and patch it accordingly
         my $evts=$evperrun;
         if($i eq $runno){
             $evts=$lastrunev;
         }              
#read header
         my $buf=$self->{tsyntax}->{headers}->{$self->{CCT}};
         my $tmpb=$templatebuffer;
         if(not defined $buf){
             $self->ErrorPlus("Unable to find   header file. ");
         } 
         my $start=$timbegu+int(($timendu-$timbegu)/$runno)*($i-1);
             my $end=$start+int(($timendu-$timbegu)/$runno);
             my ($s,$m,$h,$date,$mon,$year)=localtime($start);
         my $timstart=($year+1900)+10000*($mon+1)+1000000*($date);
         ($s,$m,$h,$date,$mon,$year)=localtime($end);
         my $timfin=($year+1900)+10000*($mon+1)+1000000*($date);
         $buf=~ s/TIMBEG=/TIMBEG=$timstart/;         
         $buf=~ s/TIMEND=/TIMEND=$timfin/;         
         $buf=~ s/PART=/PART=$particleid/;         
         $buf=~ s/RUN=/RUN=$run/;         
         if($self->{q}->param("AdvancedQuery")){
           $buf=~ s/SPECTRUM=/SPECTRUM=$spectrum/;         
           $buf=~ s/GEOCUTOFF=/GEOCUTOFF=$geocutoff/;         
           $buf=~ s/COSMAX=/COSMAX=$cosmax/;         
           $buf=~ s/FOCUS=/FOCUS=$focus/;         
           $buf=~ s/PLANENO=/PLANENO=$plane/;         
#           $buf=~ s/ROOTNTUPLE=/ROOTNTUPLE=\'$rootntuple\'/;         
           $buf=~ s/TRIGGER=/TRIGGER=$trtype/;         
           $buf=~ s/SETUP=/SETUP=$setup/;         
           $buf=~ s/OUPUTMODE=/OUPUTMODE=$rno/;         
           for my $l (1...6){
               my $cn="COOCUB$l=";
               my $ccc=sprintf("%.3f",$cubes[$l-1]);
               $buf=~ s/$cn/$cn$ccc/;         
           }
       }
         if (defined $rootntuple && $i<2) {
          $buf=~ s/ROOTNTUPLE=/ROOTNTUPLE=\'$rootntuple\'/;         
         }
#       if ($self->{q}->param("ProductionQuery")) {
#          $buf=~ s/ROOTNTUPLE=/ROOTNTUPLE=\'$rootntuple\'/;         
#        }
         if($i > 1){
            my $rid=1; 
            ($rid,$rndm1,$rndm2) = $self->getrndm($dataset);
         }
          $buf=~ s/RNDM1=/RNDM1=$rndm1/;         
          $buf=~ s/RNDM2=/RNDM2=$rndm2/;         
          $buf=~ s/TRIG=/TRIG=$evts/;         
          my $r1=rand;
          my $r2=rand;
          my $pminr=$pmin*(1-0.01*$r1);
          my $pmaxr=$pmax*(1+0.01*$r2);
        if($pminr<0.01){
            $pminr=0.01;
        }
        if($pmaxr>100000){
            $pmaxr=100000;
        }
         my $cputf=sprintf("%.3f",$cput);
         my $pminf=sprintf("%.3f",$pminr);
         my $pmaxf=sprintf("%.3f",$pmaxr);
      
         $buf=~ s/CPULIM=/CPULIM=$cputf/;         
         $buf=~ s/PMIN=/PMIN=$pminf/;         
         $buf=~ s/PMAX=/PMAX=$pmaxf/;         
         my $cpus=$q->param("QCPUTime");         
         my $cpusf=sprintf("%.3f",$cpus);
         if(defined  $q->param("QCPUTime")){
          my $cpus=$q->param("QCPUTime");
          my $cpusf=sprintf("%.3f",$cpus);
          if($q->param("ForceCpuLimit")){
           $tmpb=~ s/TIME 3=/TIME 1=$cpusf TIME 3=/; 
          }
         }
         $buf=~ s/PART=/CPUTIME=$cpus \nPART=/; 
         $rootntuple=$q->param("RootNtuple");
         $buf=~ s/ROOTNTUPLE=/ROOTNTUPLE=\'$rootntuple\'/;         
         $tmpb=~ s/ROOTNTUPLE=/C ROOTNTUPLE/g;
         $tmpb=~ s/IOPA \$ROOTNTUPLE\'/IOPA \$ROOTNTUPLE\'\n/;
         $tmpb=~ s/TERM/TGL1 1=8 \nTERM/;
         my $cputype=$q->param("QCPUType");
         $buf=~ s/PART=/CPUTYPE=\"$cputype\" \nPART=/; 
         $buf=~ s/PART=/CLOCK=$clock \nPART=/;         
         my $ctime=time();
         $buf=~ s/PART=/SUBMITTIME=$ctime\nPART=/;    
         if($self->{CCT} eq "local"){
           $buf=~ s/\$AMSProducerExec/$self->{AMSSoftwareDir}\/$gbatch/;         
         }       
         else{
          $buf=~ s/gbatch-orbit.exe/gbatch-orbit.exe -$self->{IORP} -U$run -M -D1 -G$aft -S$stalone/;
      }
         my $script="$self->{CCA}.$run.$template";
         my $root=$self->{CCT} eq "remote"?"$self->{UploadsDir}/$script":
         "$self->{AMSDataDir}/$self->{LocalClientsDir}/$script";
         if($self->{q}->param("ProductionQuery") eq "Submit Request" and $self->{CCT} eq "local" ){
# add one more check here using save state
            my $time = time();
            $sql="update Cites set state=0, timestamp=$time  where name='$self->{CCA}'";
            $self->{sqlserver}->Update($sql);
             my $save="$self->{UploadsDir}/$self->{CCA}.$run.$self->{CEMID}save";
             my $param=$self->{q}->param("AdvancedQuery")?"AdvancedQuery":($self->{q}->param("ProductionQuery")?"ProductionQuery":"BasicQuery");
            $q->param($param,"Save");
            $q->param("FEM",$save);
             save_state($q,$save);
             htmlTop();
            $self->htmlTemplateTable("Job Submit Script (click [Save] to continue)");
          if($self->{CCT} eq "local"){
              print "<B><font color=red size= 3>
                     <i> the password is required </i></font></B>";
              print "<p></p>\n";
         }
          print "<input type=\"hidden\" name=\"CEM\" value=$cem>        ";
          print "<input type=\"hidden\" name=\"FEM\" value=$save>        ";
          print "<p></p>\n";
          print $q->textarea(-name=>"CCA",-default=>"$buf$tmpb",-rows=>30,-columns=>80);
          print "<BR><TR>";
         if($self->{CCT} eq "local"){
          print "Password :  <input type=\"password\" name=\"password\" value=\"\">  ";
          print "<TR><BR>";
         }
         print $q->submit(-name=>$param, -value=>"Save");
         print htmlBottom();
         return 1;   
        }
         open(FILE,">".$root) or die "Unable to open file $root\n";  
         if($self->{CCT} eq "local"){
          if(defined $self->{AMSDSTOutputDir} and $self->{AMSDSTOutputDir} ne ""){
 print FILE "export NtupleDestDir=$self->{AMSDSTOutputDir} \n";
 print FILE "export NtupleDir=/dat0/local/logs/nt \n";
}
        my $key='ntuplevalidator';
        my $sql="select myvalue from Environment where mykey='".$key."'";
        my $ret=$self->{sqlserver}->Query($sql);
        if( not defined $ret->[0][0]){
            $self->ErrorPlus("unable to retreive ntuplevalidator name from db");
        }
          my @tmpa=split '/', $ret->[0][0];
          print FILE "export NtupleValidatorExec=$tmpa[$#tmpa] \n";
        $key='getior';
        $sql="select myvalue from Environment where mykey='".$key."'";
        $ret=$self->{sqlserver}->Query($sql);
        if( not defined $ret->[0][0]){
            $self->ErrorPlus("unable to retreive getior name from db");
        }
           @tmpa=split '/', $ret->[0][0];
          print FILE "export GetIorExec=$tmpa[$#tmpa] \n";
          print FILE "export ExeDir=$self->{AMSSoftwareDir}/exe \n";
          print FILE "export AMSDataDir=$self->{AMSDataDir} \n";
      }
#
# check here custom/generic
#
         if(defined $q->param("JST") and  $q->param("JST") eq 'C'){
             my $sdir="$self->{AMSSoftwareDir}/scripts/";
             my $newfile=$sdir."$self->{CCA}"; 
             open(FILEI,"<".$newfile) or die " Unable to find script file $newfile.  Please make sure you did send your custom script requirements to ams production team. ";
             my $sbuf;
             read(FILEI,$sbuf,16384);
             close FILEI; 
             my @ssbuf = split ',;',$sbuf;
             if($#ssbuf ne 1){
              die " Could not parse script $newfile $#ssbuf";
             }
              
             $buf=~ s/export/$ssbuf[0]\nexport/;
             $tmpb =~ s/\!/\!\n$ssbuf[1]/;
         }

         print FILE $buf;
#  change tmpb to include pl1 dependence
         if($tmpb =~/15=0/){
             if($tmpb =~/34=1/){
                 $tmpb=~ s/pl1/ecal/;
             }
             else{ 
                 $tmpb=~ s/pl1/none/;
             }
         }
         elsif($tmpb =~/15=1/){
             if($tmpb =~/34=1/){
                 $tmpb=~ s/pl1/ecalpl1/;
             }
         }
#change file size
         $tmpb=~ s/168=500000000//;         
         $tmpb=~ s/168=120000000//;         
         $tmpb=~ s/126=50000/126=99999/;         
         print FILE $tmpb;
         if($self->{CCT} eq "local"){
             print FILE 'rm  /tmp/gbatch-orbit.exe.$RUN'."\n";
         }
         close FILE;
         my $j=system("chmod +x  $root"); 
         if($self->{CCT} eq "remote"){
         $j=system("tar -C$self->{UploadsDir} -uf  $file2tar $script"); 
          if($j){
              $self->ErrorPlus("Unable to tar $script to $file2tar ");
          }
          unlink $root;
          }
         $buf=~s/\$/\\\$/g;
         $buf=~s/\"/\\\"/g;
         $buf=~s/\(/\\\(/g;
         $buf=~s/\)/\\\)/g;
         $buf=~s/\'/\\'/g;
         if($self->{sqlserver}->{dbdriver} =~ m/Oracle/){
            $buf =~ s/'/''/g;
         }
         $tmpb=~s/\"/\\\"/g;
         $tmpb=~s/\(/\\\(/g;
         $tmpb=~s/\)/\\\)/g;
         $tmpb=~s/\$/\\\$/g;
         $tmpb=~s/\'/\\'/g;
    if($self->{sqlserver}->{dbdriver} =~ m/Oracle/){
         $tmpb =~ s/'/''/g;
    }
         $ctime=time();
         my $nickname = $q->param("QNick");
         my $stalone  = "STANDALONE";
         if ($q->param("STALONE") eq "No") {
          my $stalone  = "CLIENT";
         }
         $insertjobsql="INSERT INTO Jobs VALUES
                             ($run,
                              '$script',
                              $self->{CEMID},
                              $self->{CCID},
                              $did,
                              $ctime,
                              $evts,
                              $timeout,
                              '$buf$tmpb',
                              $ctime,
                              '$nickname',
                               'host',0,0,0,0,'$stalone',
                              -1)";
         $self->{sqlserver}->Update($insertjobsql);
#         $self->{sqlserver}->Update($sql);
#
#creat corresponding runevinfo
         my $ri={};
         $ri->{uid}=0;
         $ri->{Run}=$run;
         $ri->{FirstEvent}=1;
         $ri->{LastEvent}=$evts;
         $ri->{TFEvent}=$timbegu;
         $ri->{TLEvent}=$timendu;
         $ri->{Priority}=0;
         $ri->{FilePath}=$script;
       
            if ($self->{CCT} eq "remote"){
             $ri->{Status}="Foreign";
             $ri->{History}="Foreign";
             $ri->{CounterFail}=0;
             $ri->{cuid}=$ri->{Run};
            } 
            else{
             $ri->{Status}="ToBeRerun";
             $ri->{History}="ToBeRerun";
             $ri->{CounterFail}=0;
             $ri->{cuid}=0;
            }
         $ri->{SubmitTime}=time();
         $ri->{cinfo}={};
         $ri->{cinfo}->{Run}=$ri->{Run};
         $ri->{cinfo}->{EventsProcessed}=0;
         $ri->{cinfo}->{LastEventProcessed}=0;
         $ri->{cinfo}->{ErrorsFound}=0;
         $ri->{cinfo}->{CriticalErrorsFound}=0;
         $ri->{cinfo}->{CPUTimeSpent}=0;
         $ri->{cinfo}->{TimeSpent}=0;
         $ri->{cinfo}->{Status}=$ri->{Status};
         $ri->{cinfo}->{HostName}=" ";
         push @{$self->{Runs}}, $ri; 
         $run=$run+1;
        }

#
# Add files to server
#
        if(defined $self->{dbserver} ){
            if($self->{dwldaddon}==0){
            foreach my $ri (@{$self->{Runs}}){
              DBServer::sendRunEvInfo($self->{dbserver},$ri,"Create");
            }
            my $lu=time();
            my $sqll="update Servers set lastupdate=$lu where dbfilename='$self->{dbfile}'";
            $self->{sqlserver}->Update($sqll);
            }
        }
        else{
            $self->ErrorPlus("Unable To Communicate With Server");
        }
         if ($self->{CCT} eq "remote"){
          my $i=system("gzip -f $file2tar");
          if($i){
              $self->ErrorPlus("Unable to gzip  $file2tar");
          }
        }     
# insert into Jobs
#        if (defined $insertjobsql) {
#         $self->{sqlserver}->Update($insertjobsql);
#        }

        my $address=$self->{CEM};
        my $frun=$run-$runno;
        my $lrun=$run-1;
        my $subject="AMS02 MC Request Form Output Runs for $address $frun...$lrun Cite $self->{CCA}";
                 my $message;
                 if($self->{dwldaddon}==1){
                   $message=$self->{tsyntax}->{headers}->{readmestandalone};
                 }
                 else{
                   $message=$self->{tsyntax}->{headers}->{readmecorba};
                 }                
        $sql = "SELECT dirpath FROM journals WHERE cid=$self->{CCID}";
        my $note='please contact alexei.klimentov@cern.ch for details'; 
        $ret = $self->{sqlserver}->Query($sql);
  
        if (defined $ret->[0][0]) {
         $note ="DST directory path : $ret->[0][0]/nt \n Journal files directory path : $ret->[0][0]/jou \n";
         }         
         $message=$message.$note;
                  my $attach;
       if ($self->{CCT} eq "remote"){
          if($self->{senddb}==1){
          $self->{TU1}=time();
           $attach="$file2tar.gz,ams02mcscripts.tar.gz;$filedb,ams02mcdb.tar.gz";
          }
          elsif($self->{sendaddon}==1){
              $self->{TU2}=time();
              $self->{sendaddon}=0;
             $attach= "$file2tar.gz,ams02mcscripts.tar.gz;$filedb_att,ams02mcdb.addon.tar.gz";
          }
          else{
              $attach= "$file2tar.gz,ams02mcscripts.tar.gz";
          }
                  $self->sendmailmessage($address,$subject,$message,$attach);
                  my $i=unlink "$file2tar.gz";
                  if($self->{sendaddon}==1){
                   $self->{TU2}=time();
                   $attach="$filedb_att,ams02mcdb.addon.tar.gz";
                   $subject="Addon To AMS02 MC Request Form Output Runs for $address $frun...$lrun Cite $self->{CCA}";
                   $self->sendmailmessage($address,$subject,$message,$attach);
               }
              }
         my $totalreq=$self->{CEMR}+$runno;
         my $time=time();
         $sql="Update Mails set requests=$totalreq, timeu1=$self->{TU1}, timeu2=$self->{TU2}, timestamp=$time where mid=$self->{CEMID}";
         $self->{sqlserver}->Update($sql);              
         $subject="MC Request Form Output Runs for $address $frun...$lrun Cite $self->{CCA}";
         $self->sendmailerror($subject," ");
         $sql="SELECT mid FROM Cites WHERE cid=$self->{CCID}";
         $ret=$self->{sqlserver}->Query($sql);
         if(defined $ret->[0][0] && $ret->[0][0] != $self->{CEMID}){
           $sql="SELECT address FROM Mails WHERE mid=$ret->[0][0]";
           $ret=$self->{sqlserver}->Query($sql);
           if(defined $ret->[0][0]){
             $self->sendmailmessage($ret->[0][0],$subject," ");
           } else {
            $self->ErrorPlus("Unable to obtain mail address for MailId = $ret->[0][0] and Cite=$self->{CCID}");
           }           
       }
           $time=time();
           $sql="update Cites set state=0, maxrun=$run, timestamp=$time where name='$self->{CCA}'";
            $self->{sqlserver}->Update($sql);
            
        foreach my $cite (@{$self->{CiteT}}){
            if($self->{CCA} eq $cite->{name}){
              $cite->{maxrun}=$run;
              last;                          
          }
        }

                  my $dir=$self->{UploadsDir};
                  open DIR,$dir;
                  my @allfiles= readdir THISDIR;
                  closedir DIR;    
                  foreach my $file (@allfiles){
                     my $pat='/$\.'."$self->{CEMID}.save/";
                     if($file =~$pat){
                       unlink $file;
                     }
                  }                      
# check last download time
# but first check local/remote cite
      my $cite_status="remote";
      $sql="SELECT Cites.status FROM Cites, Mails WHERE Cites.cid=Mails.cid  AND ADDRESS='$self->{CEM}'"; 
      my $recites=$self->{sqlserver}->Query($sql);
      if(defined $ret->[0][0]){
        $cite_status= $recites->[0][0];
      }
# check last download time
     if ($cite_status eq "remote") {
          my $uplt0 = 0;  #last upload
          my $uplt1 = 0;  # for filedb and filedb.addon
          $sql="SELECT timeu1, timeu2 FROM Mails WHERE ADDRESS='$self->{CEM}'";
          my $retime=$self->{sqlserver}->Query($sql);
          if(defined $retime->[0][0]){
          foreach my $uplt (@{$retime}){
           $uplt0    = $uplt->[0];
           $uplt1    = $uplt->[1];
          }
        }

          $self->DownloadTime();

        $self->{FileDBLastLoad}=$uplt0;
        $self->{FileAttDBLastLoad}=$uplt1;

        if ($self->{dwldaddon} == 1) {
         if($uplt0 == 0 or $uplt0 < $self->{FileDBTimestamp} or $uplt1 == 0 or $uplt1 < $self->{FileAttDBTimestamp}) {
            $self->Download();
         } else {
          $self->{FinalMessage}=" Your request was successfully sent to $self->{CEM}";     
         } 
        } else {
         if($uplt0 == 0 or $uplt0 < $self->{FileDBTimestamp}) {
            $self->Download();
        } else {
         $self->{FinalMessage}=" Your request was successfully sent to $self->{CEM}";     
         } 
       }
            
     } else { 
      $self->{FinalMessage}=" Your request was successfully sent to $self->{CEM}";     
     }
}
#here the default action
 if($self->{read}==0){


    print $q->header( "text/html" ),
    $q->start_html(-title=>'Welcome');
#       -bgcolor=>'blue.jpg' );
#print qq`
#<TITLE> Welcome </TITLE>
#<body background="blue.jpg" >
#<hr>
#`;
    print $q->h1( "Welcome to the AMS02 RemoteClient MC Request Form" );
        print $q->start_form(-method=>"GET", 
          -action=>$self->{Name});
    print qq`
 Client E-Mail Address<INPUT TYPE="text" NAME="CEM" VALUE="" SIZE=32>
`;
       print $q->submit(-name=>"MyRegister", -value=>"Register");
    print "<BR>";
    print "Please Choose Basic or Advanced Request Form Below";
         print "<BR>";
   print qq`
<INPUT TYPE="radio" NAME="CTT" VALUE="Basic" CHECKED>Basic<BR>
<INPUT TYPE="radio" NAME="CTT" VALUE="Advanced" >Advanced<BR>
`;
    print "<BR>";
    print "Production DataSets (Cite's Responsible Only)";
    print "<BR>";
    foreach my $dataset (@{$self->{DataSetsT}}){
print qq`
<INPUT TYPE="radio" NAME="CTT" VALUE="$dataset->{name}" >$dataset->{name}<BR>
`;
    }
       print $q->submit(-name=>"MyQuery", -value=>"Click Here To Begin");
       print $q->reset(-name=>"Reset");
         print $q->end_form;


#    $self->listAll();
} # default action
    elsif($self->{read}==1){
        if(defined $self->{FinalMessage}){
           $self->InfoPlus($self->{FinalMessage});
       }
    }


#this is for final verification


# my $tm={};
# try{
#  $tm = $ref->{orb}->string_to_object($ref->{IOR});
#  $ref->{arsref}=[$tm];
#     $ref->{ok}=1;
#}
# catch CORBA::MARSHAL with{
#     carp "MARSHAL Error "."\n";
# }
# catch CORBA::COMM_FAILURE with{
#     carp "Commfailure Error "."\n";
# }
# catch CORBA::SystemException with{
#     carp "SystemException Error "."\n";
# };

    return 1;

}







sub getior{
    my $ref=shift;
    my $pid=$$;
    my $file ="/tmp/o."."$pid";
    my $fileo ="/tmp/oo."."$pid";
    my $i=system "/usr/local/lsf/bin/bjobs -q linux_server -u all>$file" ;
    if($i){
     if($ref->{debug}){
          $ref->WarningPlus("unable to bjobs");
      }
        unlink $file;
        return $ref->getior2();
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
                    if($tag eq $ref->{DataMC}){
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
     if($ref->{debug}){
      $ref->WarningPlus("unable to bpeek");
     }
    return $ref->getior2();      
}
sub getior2{
    my $ref=shift;
    my $file ="/tmp/DumpIOR".".$ref->{DataMC}";
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


sub Warning{
    my $ref=shift;
    Warning::error($ref->{q},"Unable To Connect The Servers");
  }
sub WarningPlus{
    my $ref=shift;
    Warning::warning($ref->{q},shift);
  }
sub InfoPlus{
    my $ref=shift;
    Warning::info($ref->{q},shift);
  }
sub ErrorPlus{
    my $ref=shift;
    if(defined $ref->{CCA} and defined $ref->{sqlserver}){
       my $time=time();
       my $sql="update Cites set state=0, timestamp=$time where name='$ref->{CCA}'";
       $ref->{sqlserver}->Update($sql);
    }
    Warning::error($ref->{q},shift);
  }

sub findemail(){
    my $self=shift;
        my $cem=shift;
        foreach my $chop (@{$self->{MailT}}) {
            if($chop->{address} eq $cem and $cem ne "" and $cem ne " " and defined $cem){
                $self->{CEM}=$cem;
                $self->{CEMA}=$chop->{status};
                $self->{CEMID}=$chop->{mid};
                $self->{CEMR}=$chop->{requests};
                $self->{CCR}=$chop->{rsite};
                $self->{CSR}=$chop->{rserver};
                $self->{TU1}=$chop->{timeu1};
                $self->{TU2}=$chop->{timeu2};
                foreach my $cite (@{$self->{CiteT}}){
                    if($chop->{cid} eq $cite->{cid}){
                        $self->{CCA}=$cite->{name};
                        $self->{CCT}=$cite->{status};
                        $self->{CCID}=$cite->{cid};
                        last;
                    }
                }  
                last;
            }
            elsif (defined $chop->{alias} and $chop->{status} eq "Active"){
             if($chop->{alias} eq $cem and $cem ne "" and $cem ne " " and defined $cem){
                $self->{CEM}=$cem;
                $self->{CEMA}=$chop->{status};
                $self->{CEMID}=$chop->{mid};
                $self->{CEMR}=$chop->{requests};
                $self->{CCR}=$chop->{rsite};
                $self->{CSR}=$chop->{rserver};
                $self->{TU1}=$chop->{timeu1};
                $self->{TU2}=$chop->{timeu2};
                foreach my $cite (@{$self->{CiteT}}){
                    if($chop->{cid} eq $cite->{cid}){
                        $self->{CCA}=$cite->{name};
                        $self->{CCT}=$cite->{status};
                        $self->{CCID}=$cite->{cid};
                        last;
                    }
                }  
                last;
            }
         }
        }
    if (defined $self->{CEM}){
        return 1;
    }
    else{
        return 0;
    }
}

sub validate_email_address {
    my $addr_to_check = shift;
    $addr_to_check =~ s/("(?:[^"\\]|\\.)*"|[^\t "]*)[ \t]*/$1/g;

    my $esc         = '\\\\';
    my $space       = '\040';
    my $ctrl        = '\000-\037';
    my $dot         = '\.';
    my $nonASCII    = '\x80-\xff';
    my $CRlist      = '\012\015';
    my $letter      = 'a-zA-Z';
    my $digit       = '\d';

    my $atom_char   = qq{ [^$space<>\@,;:".\\[\\]$esc$ctrl$nonASCII] };
    my $atom        = qq{ $atom_char+ };
    my $byte        = qq{ (?: 1?$digit?$digit |
                              2[0-4]$digit    |
                              25[0-5]         ) };

    my $qtext       = qq{ [^$esc$nonASCII$CRlist"] };
    my $quoted_pair = qq{ $esc [^$nonASCII] };
    my $quoted_str  = qq{ " (?: $qtext | $quoted_pair )* " };

    my $word        = qq{ (?: $atom | $quoted_str ) };
    my $ip_address  = qq{ \\[ $byte (?: $dot $byte ){3} \\] };
    my $sub_domain  = qq{ [$letter$digit]
                          [$letter$digit-]{0,61} [$letter$digit]};
    my $top_level   = qq{ (?: $atom_char ){2,4} };
    my $domain_name = qq{ (?: $sub_domain $dot )+ $top_level };
    my $domain      = qq{ (?: $domain_name | $ip_address ) };
    my $local_part  = qq{ $word (?: $dot $word )* };
    my $address     = qq{ $local_part \@ $domain };

    return $addr_to_check =~ /^$address$/ox ? $addr_to_check : "";
}
sub sendmailerror{
    my ($self,$subject,$message) = @_;
       foreach my $chop (@{$self->{MailT}}) {
              if($chop->{rserver}==1){
                  my $address=$chop->{address};
                  $self->sendmailmessage($address,$subject,$message);
                  last;
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
        or die "could not open sendmail: $!";
     print MAIL <<END_OF_MESSAGE2;
To:  $add
Subject: $sub

$mes
END_OF_MESSAGE2
    close MAIL or die "Error closing sendmail: $!";  
    }
 }

sub getRID() {
    my $self=shift;
    my $maxrun;
        my $sql="select rid from RNDM WHERE rid<0";
        my $res=$self->{sqlserver}->Query($sql);
        if(not defined $res->[0][0]){
           Warning::error($self->{q},"unable to find RID<0");
           $maxrun = -1;
       } else {
           $maxrun=$res->[0][0];
        }
        $sql="select rid,rndm1,rndm2 from RNDM where rid=$maxrun";
        $res=$self->{sqlserver}->Query($sql);
        if( not defined $res->[0][0] ){
          my $big=2147483647;
          my $rndm1=int (rand $big);
          my $rndm2=int (rand $big);
          my $rid  = 0;
          Warning::error($self->{q},"unable to read rndm table $maxrun");
          return $rid;
      }
      my $rid  =$res->[0][0];

      return $rid;

}  

sub getrndm(){
    my $self=shift;
     if(not defined shift){
          my $big=2147483647;
          my $rndm1=int (rand $big);
          my $rndm2=int (rand $big);
           my $rid=0;
          return ($rid,$rndm1,$rndm2);
     }
        my $sql="select rid from RNDM WHERE rid<0";
        my $res=$self->{sqlserver}->Query($sql);
        my $maxrun;
        if(not defined $res->[0][0]){
            $sql="UPDATE RNDM SET rid=-rid where rid=1";
             $self->{sqlserver}->Update($sql);
            $maxrun=-1;
        }
        else{
            $maxrun=$res->[0][0];
        }
        $sql="select rid,rndm1,rndm2 from RNDM where rid=$maxrun";
        $res=$self->{sqlserver}->Query($sql);
        if( not defined $res->[0][0] ){
          my $big=2147483647;
          my $rndm1=int (rand $big);
          my $rndm2=int (rand $big);
          my $rid  = 0;
           Warning::error($self->{q},"unable to read rndm table $maxrun");

          return ($rid,$rndm1,$rndm2);
         }
    my $rid  =$res->[0][0];
    my $rndm1=$res->[0][1];
    my $rndm2=$res->[0][2];
             $sql="UPDATE RNDM SET rid=-rid where rid=$maxrun";
             $self->{sqlserver}->Update($sql);
             $maxrun=-$maxrun+1;
             $sql="UPDATE RNDM SET rid=-rid where rid=$maxrun";
             $self->{sqlserver}->Update($sql);
             return ($rid,$rndm1,$rndm2);
 
    }


sub blessdb{
    my $objref={};
    bless $objref;
    return $objref;
}

sub Exiting{

    my $ref=shift;
    $ref->ErrorPlus(shift);
}

# Reads a saved CGI object from disk and return its params as a hash ref
sub get_state {
    my $cart = shift;
    
    local *FILE;
    
    -e $cart or return;
    open FILE, $cart or die "Cannot open $cart: $!";
    my $q_saved = new CGI( \*FILE ) or
        die "Unable to restore saved state.";
    close FILE;
    unlink $cart;
    return $q_saved;
}

# Saves the current CGI object to disk

sub save_state {
    my $q = shift;
    my $cart = shift;
    local( *FILE, *DIR );
    
    
    # Save the current CGI object to disk
    open FILE, "> $cart" or return die "Cannot write to $cart: $!";
    $q->save( \*FILE );
    close FILE;
}


sub htmlTop {
    print "Content-type: text/html\n\n";
    print "<HTML>\n";
    print "<HEAD><TITLE>Search </TITLE></HEAD>\n";
    print "<body bgcolor=cornsilk text=black link=#007746 vlink=navy alink=tomato>\n";
}

sub htmlBottom {
    my $version="1.01";    
    print "</TABLE>\n";
    print "<HR>\n<TABLE WIDTH=\"100%\"><TR>\n<TD WIDTH=\"33%\" ALIGN=\"Left\">";
    print "</TD>\n<TD ALIGN=\"center\"><FONT SIZE=2>jobt.pl V$version</FONT></TD>\n";
    print "</FONT></TD>\n</TR></TABLE>\n";
    print "</BODY></HTML>\n";
}

sub htmlAMSHeader {

   my ($self) = shift;
   my ($header) = shift;

   print "<basefont face=\"verdana,arial,helvetica,sans-serif\">\n";
   print "<table border=0   cellpadding=5 cellspacing=0 width=\"100%\">\n";
   print "<tr bgcolor=\"#ffdc99\">\n";
   print "<td align=left> <font size=\"-1\"><b>\n";
   print "<a href=$amshome_page>AMS</a>\n";
   print "&nbsp; <a href=$amscomp_page>Computing</a>\n";
   print "</b>\n";
   my $href=$self->{HTTPhtml}."/mm.html";
   print "&nbsp;<b><a href=$href>MC production</a>\n";
   print "</b></td>\n";
   print "<td align=right> <font size=\"-1\"> &nbsp;  <!-- top right corner  --> </font></td>\n";
   print "</tr>\n";
   print "<tr bgcolor=\"#ffdc9f\"><td align=center colspan=2><font size=\"+2\" color=\"#3366ff\"> <b>\n";
   print $header;
   print "</b></font></td></tr>\n";
   print "<tr bgcolor=\"#ffdc9f\">\n";
   print "<td align=left> <font size=\"-1\"><!-- lower left text --> &nbsp;</td>\n";
   print "<td align=right> <font size=\"-1\"> &nbsp; </font></td></tr>\n";
   print "<tr><td colspan=2 align=right> <font size=\"-1\">\n";
   print " </font></td></tr>\n";
   print "</table>\n";
   print "<p>\n";
}

sub htmlMCWelcome {
   print "<font size=\"3\"><TR><TD><b>\n";
   print " This is an interface to the AMS MC02 Remote/Client Database </TD></TR> \n";
   print "<TR><TD> \n";
   print "All comments (to <font color=\"green\"> alexei.klimentov\@cern.ch, vitali.choutko\@cern.ch </font>) appreciated (items in <font color=\"tomato\"> tomato </font> are not implemented yet). Basic query keys are 
in <font color=\"green\"> green </font>, advanced query keys are in <font color=\"blue\"> blue.</TD></TR>\n";
   print "</ul>\n";
   print "<font size=\"2\" color=\"black\">\n";
   print "<li> Catalogues are updated nightly.\n";
}
sub htmlTemplateTable {
   my ($self) = shift;
   my ($text) = shift;
   print "<TR><B><font color=green size= 5> $text </font>";
   print "<p>\n";
   print "<FORM METHOD=\"GET\" action=\"$self->{Name}\">\n";
   print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
}

sub htmlTable {
   my ($text) = shift;
   print "<TR><B><font color=green size= 5> $text </font>";
   print "<p>\n";
   print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
}

sub htmlTableEnd {
   print "</TABLE>\n";
}

sub htmlWarning {
    my ($subr,$text) = @_;
    print "<p><tr><td><font size=\"2\">\n";
    print "<b><i> $subr -W- </i> $text \n";
    print "</b></font></td></tr>\n";
}

sub htmlFormEnd {
   print "</form>\n";
}

sub htmlTextField {
    my ($text,$type,$size,$value,$name,$comment) = @_;
    print "<tr><td><font size=\"2\">\n";
    print "<b> $text </td><td><input type=$type size=$size value=$value name=$name>$comment\n";
    print "</b></font></td></tr>\n";
}

sub htmlHiddenTextField {
    my ($text,$type,$size,$value,$name,$comment) = @_;
    print "<tr><td><font size=\"2\">\n";
    print "<b>  </td><td><input type=$type size=$size value=$value name=$name>$comment\n";
    print "</b></font></td></tr>\n";
}

sub htmlText {
    my ($text,$value) = @_;
    print "<tr><td><font size=\"2\">\n";
    print "<b> $text </td><td>$value\n";
    print "</b></font></td></tr>\n";
}

sub htmlReturnToMain {
            print "<p><tr><td><i>\n";
            print "Return to <a href=$rchtml> MC02 Remote Client Request</a>\n";
            print "</i></td></tr>\n";
        }

sub htmlReadMeFirst {
            print "<p><tr><td><i>\n";
            print "<font color=\"green\"><b><a href=/cgi-bin/mon/load.cgi?AMS02MCUploads/Help.txt target=\"_blank\"> ReadMeFirst</a></b></font>\n";
            print "</i></td></tr>\n";
        }

sub htmlReturnToQuery {
            print "<p><tr><td><i>\n";
            print "Return to <a href=$rccgi> MC02 Query Form</a>\n";
            print "</i></td></tr>\n";
        }

sub getPathNoDisk {
    my $path = shift;
    my @junk = split '/',$path;
    my $tdir ="";
    for (my $i=2; $i<$#junk; $i++) {
      $tdir = $tdir.$junk[$i]."/";
    }
    $tdir = $tdir.$junk[$#junk];
    $tdir = trimblanks($tdir);

    return $tdir;
}

sub trimblanks {
    my @inp_string = @_;
    for (@inp_string) {
        s/^\s+//;        
        s/\s+$//;
    }
    return wantarray ? @inp_string : $inp_string[0];
}

sub checkJobsTimeout {
#
# give extra 24 hourse to timeout jobs
#
    my $self   = shift;
    my $lastupd= undef; 

    my $sql;
#    my $timenow = time() - 24*60*60;
    my $timenow = time();

 my $HelpTxt = "
     -c    - output will be produced as ASCII page (default)
     -h    - print help
     -v    - verbose mode
     -w    - output will be produced as HTML page
     ./checkjobtmout.cgi -c -v
";

  foreach my $chop  (@ARGV){
    if ($chop =~/^-c/) {
        $webmode = 0;
    }
    if ($chop =~/^-v/) {
        $verbose = 1;
    }
    if ($chop =~/^-w/) {
     $webmode = 1;
    }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
   }


   $self->setActiveProductionSet();

   if ($webmode == 1) {
    $self->htmlTop();
#
    htmlTable("Jobs below will be deleted from the database");
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     print "<td><b><font color=\"blue\">Cite </font></b></td>";
     print "<td><b><font color=\"blue\" >JobID </font></b></td>";
     print "<td><b><font color=\"blue\" >Submitted</font></b></td>";
     print "<td><b><font color=\"blue\" >Expired</font></b></td>";
     print "<td><b><font color=\"blue\" >Status</font></b></td>";
     print "<td><b><font color=\"blue\" >Owner</font></b></td>";
     print "</tr>\n";
     print_bar($bluebar,3);
   }
#
# 1st update runs.status from 'Finished'/'Failed' to 'Completed' if 
# at least one DST is copied to final destination 
#
# get production set path
    if (not defined $ActiveProductionSet || $ActiveProductionSet eq $UNKNOWN) {
    } else {
     $sql = "SELECT runs.run 
              FROM runs 
               WHERE runs.status != 'Completed' AND 
                     runs.status != 'TimeOut'   AND 
                     runs.status != 'Finished'  AND 
                     runs.status != 'Processing'";
      my $r1=$self->{sqlserver}->Query($sql);
      if( defined $r1->[0][0]){
       foreach my $r (@{$r1}){
        my $run= $r -> [0];
        $sql="SELECT run FROM ntuples WHERE path LIKE '%$ActiveProductionSet%' AND run=$run";
        my $r2=$self->{sqlserver}->Query($sql);
        if( defined $r2->[0][0]){
           $sql="UPDATE runs SET runs.status='Completed' WHERE run=$run";
           $self->{sqlserver}->Update($sql); 
       }
    }
   }
  } # Active Production Set
#
    $sql="SELECT jobs.jid, jobs.time, jobs.timeout, jobs.mid, jobs.cid, 
                 cites.name FROM jobs, cites 
          WHERE jobs.time+jobs.timeout <  $timenow AND 
                jobs.cid=cites.cid"; 
    my $r3=$self->{sqlserver}->Query($sql);
    if( defined $r3->[0][0]){
     foreach my $job (@{$r3}){
         my $tmoutflag    = 0;
         my $owner        ="xyz";
         my $jobstatus    ="unknown";
         my $jid          = $job->[0];
         my $address      = "alexei.klimentov\@cern.ch";

         my $timestamp    = $job->[1];
         my $submittime = localtime($timestamp);
         my $timeout      = $job->[2];
         my $tsubmit      = EpochToDDMMYYHHMMSS($timestamp);
         my $texpire      = EpochToDDMMYYHHMMSS($timestamp+$timeout);

         my $mid          = $job->[3];
         my $cid          = $job->[4];
         my $cite         = $job->[5];
         $sql = "SELECT mails.name, mails.address FROM Mails WHERE mails.mid=$mid";
         my $r4 = $self->{sqlserver}->Query($sql);
         if (defined $r4->[0][0]) {
            $owner   = $r4->[0][0];
            $address = $r4->[0][1].",".$address;
          }

         $sql="SELECT runs.run, runs.status 
                FROM runs 
                WHERE runs.jid = $jid ";
         $r4=$self->{sqlserver}->Query($sql); 
         if (defined $r4->[0][0]) {
             $jobstatus = $r4->[0][1];
             if ($jobstatus eq 'Failed'     ||
#-                 $jobstatus eq "Finished"   ||
#-                 $jobstatus eq "Foreign"    ||
#-                 $jobstatus eq "Processing" ||
                 $jobstatus eq "Unchecked")  {
               $sql= "UPDATE runs SET status='TimeOut' WHERE run=$jid";
               $self->{sqlserver}->Update($sql); 
               $tmoutflag = 1;
             }
         }
           else {
              $tmoutflag = 1;
          }
      if ($tmoutflag == 1) {
       my $timenow    = time();
       my $deletetime = localtime($timenow+60*60);

       my $exptime    = $timestamp+$timeout;

       $exptime       = localtime($exptime);
       my $sujet = "Job : $jid - expired";
       my $message    = "Job $jid, Submitted : $submittime, Expired : $exptime; 
                         \n Job will be removed from database (Not earlier than  : $deletetime).
                         \n MC Production Team.
                         \n ----------------------------------------------
                         \n This message was generated by program.
                         \n DO NOT reply using REPLY option of your mailer";
       $self->sendmailmessage($address,$sujet,$message);

        $self->amsprint($cite,666);
        $self->amsprint($jid,666);
        $self->amsprint($tsubmit,666);
        $self->amsprint($texpire,666);
        $self->amsprint($jobstatus,666);
        $self->amsprint($owner,0);
        if ($webmode == 1) {print "</tr>\n";}
      }
     }
 }

 if ($webmode == 1) {
  $self->htmlTableEnd();
  $self->htmlBottom();
 }
}

sub updateHostInfo {

# get nominal hosts list from CORBA server
# delete obsolete hosts list from database
# update status according to active hosts list

    my $self = shift;
    my $sql  = undef;

    my $hid    = 1;
    my $clock  = 0;
    my $cpus   = 0;
    my $face   = undef;
    my $mem    = 0;
    my $name   = undef;
    my $os     = undef;
    my $status = 'nominal';
    if( not $self->Init()){
        die "getHostInfo -F- Unable To Init";
    }
    if (not $self->ServerConnect()){
        die "getHostInfo -F- Unable To Connect To Server";
    }
#
    my $timestamp = time();
# get list of nominal hosts from Server
    if( not defined $self->{dbserver}->{nhl}){
      DBServer::InitDBFile($self->{dbserver});
    }
    $sql = "DELETE LocalHosts";
    $self->{sqlserver}->Update($sql);
    print "$sql \n";

    foreach my $h (@{$self->{dbserver}->{nhl}}){
        $name  = $h->{HostName};
        $clock = $h->{Clock};
        $cpus  = $h->{CPUNumber};
        $mem   = $h->{Memory};
        $face  = $h->{Interface};
        $os    = $h->{OS};
        
        $sql   = "INSERT INTO LocalHosts VALUES (
                   $hid,
                   '$name',
                   '$os',
                   $cpus,
                   $mem,
                   $clock,
                   '$face',
                   '$status',
                   $timestamp)";
        $self->{sqlserver}->Update($sql);
        print "$sql \n";
        $hid++;
    }
# get list of active hosts and update status
    foreach my $h (@{$self->{dbserver}->{ahlp}}){
        $name  = $h->{HostName};
        $sql   = "Update LocalHosts set status='active' WHERE name='$name'";
        $self->{sqlserver}->Update($sql);
        print "$sql \n";
    }

}


sub getHostsMips {

    my $self     = shift;
    my $hostname = shift;

    my $mips     = 0;
    my $mhz      = 0;
    my $model    = undef;
    my $name      = $self->gethostname($hostname);

    my $sql = "SELECT model, mhz FROM cern_hosts WHERE host='$name'";
    my $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
        $model = trimblanks($ret->[0][0]);
        $mhz = $ret->[0][1];
        $sql ="SELECT model, minclock, maxclock, coeff FROM cpu_coeff";
        my $r0 = $self->{sqlserver}->Query($sql);
        if (defined $r0->[0][0]) {
         foreach my $m (@{$r0}){
             my $mm = $m->[0];
             my $min= $m->[1];
             my $max= $m->[2];
             my $coeff = $m->[3];
             if ($model =~ m/$mm/) {
                 if ($mhz > $min && $mhz < $max) {
                     $mips = $mhz*$coeff;
                     last;
                 }
             }
         }
         if ($mips == 0) {print "Cannot find corresponding model for $hostname in cpu_coeff table \n";}
     }
    } else {
         print "Cannot find $hostname in cern_hosts table \n";
    }
    if ($verbose == 1) {print "$hostname $mips \n";}
    return $mips;
}

sub updateHostsMips {

    my $self     = shift;

  my $HelpTxt = "
     updateHostsMips updates MIPS column in cern_hosts DB table 
                     using host clock and coefficient from 
                     cpu_coeff table.


     -p    - print list of known hosts
     -h    - print help
     -v    - verbose mode
     -u    - update table 
     
     from pcamsf0 and pcamss0 only :

     ./updatemips.cgi -p -v -u
";

  my $printOut = 0;
  my $update   = 0;
  my $verbose  = 0;

  foreach my $chop  (@ARGV){
    if($chop =~/^-p/){
     $printOut = 1;
    } 
    if ($chop =~/^-v/) {
     $verbose = 1;
    }
    if ($chop =~/^-u/) {
     $update = 1;
    }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }

   }



    my $mips     = 0;
    my $mhz      = 0;
    my $model    = undef;
    my $hostname = undef;

    my $sql = "SELECT host, model, mhz FROM cern_hosts";
    my $ret = $self->{sqlserver}->Query($sql);
    if ($printOut == 1) {printf (" %10s %8s  %5s  %20s\n","Host", "ClockMHz", "Mips", "Model");}
    if (defined $ret->[0][0]) {
      foreach my $h (@{$ret}){
        $hostname = trimblanks($h->[0]);
        $model = trimblanks($h->[1]);
        $mhz = $h->[2];
        $sql ="SELECT model, minclock, maxclock, coeff FROM cpu_coeff";
        my $r0 = $self->{sqlserver}->Query($sql);
        if (defined $r0->[0][0]) {
         foreach my $m (@{$r0}){
             $mips = 0;
             my $mm = trimblanks($m->[0]);
             my $min= $m->[1];
             my $max= $m->[2];
             my $coeff = $m->[3];
#             print "$model ... $mm \n";

             my $tmm = trimblanks($m->[0]);
             $tmm =~ s/\(//g;
             $tmm =~ s/\)//g;

             my $tmodel = $model;
             $tmodel =~ s/\(//g;
             $tmodel =~ s/\)//g;

             if ($tmodel =~ /$tmm/) {
#                 print "found $model \n";
                 if ($mhz > $min && $mhz < $max) {
                     $mips = $mhz*$coeff;
                     if ($update == 1) {
                      $sql = "UPDATE cern_hosts SET MIPS=$mips where host='$hostname'";
                      $self->{sqlserver}->Update($sql);
                     }
                     last;
                 }
             }
         }
         if ($mips == 0) {print "Cannot find corresponding model for $hostname in cpu_coeff table \n";}
     }
        if ($printOut == 1) {printf (" %10s %6d  %6d  %20s\n",$hostname, $mhz, $mips, $model);}
    }

  }
      return 1;
}


sub getJobParticleFromNTPath {
    my $self = shift;
    my $jid  = shift;

    my $particle='xyz';
    my $sql = "SELECT path FROM ntuples WHERE jid = $jid";
    my $r1 = $self->{sqlserver}->Query($sql);
     if (defined $r1->[0][0]) {
       my $path = $r1->[0][0];
       my @junk = split '/',$path; # /disk/MC/AMS02/TrialPeriod/particle/jid/ntfile
       if ($#junk > 0) {
        $particle = trimblanks($junk[5]);
    }
   }
    return $particle;
}

sub getJobParticleFromDSTPath {
    my $self  = shift;
    my $path  = shift;

    $path = trimblanks($path);
    my $particle='xyz';
       my @junk = split '/',$path; # /disk/MC/AMS02/TrialPeriod/particle/jid/ntfile
       if ($#junk > 0) {
        $particle = trimblanks($junk[5]);
    }
    return $particle;
}

sub listAll {
    my $self = shift;
    my $show = shift;
    htmlTop();
    $self->ht_init();
    if ($show eq 'all') {
#     $self->ht_Menus();
     $self -> colorLegend();
    }
    
    $self->  setActiveProductionSet();
    $self -> listProductionSetPeriods();
    $self -> listStat();
     $self -> listCites();
      $self -> listMails();
       $self -> listServers();
        $self -> listJobs();
         $self -> listRuns();
          $self -> listNtuples();
            $self -> listDisks();
    htmlBottom();
}

sub listShort {
    my $self = shift;
    my $show = shift;
    htmlTop();
    $self->ht_init();
    if ($show eq 'all') {
     $self -> colorLegend();
    }
    
    $self->  setActiveProductionSet();
    $self -> listProductionSetPeriods();
    $self -> listStat();
     $self -> listCites();
      $self -> listMails();
       $self -> listServers();
        $self -> listDisks();
    htmlBottom();
}

sub listMin {
    my $self = shift;
    my $show = shift;
    htmlTop();
    $self->ht_init();
    if ($show eq 'all') {
     $self -> colorLegend();
    }
    
    $self -> listCites();
    $self -> listMails();

    htmlBottom();
}

sub queryDB {
    my $self = shift;
    my $q=$self->{q};
    if ($self->{q}->param("queryDB")) {
        htmlTop();
        if (defined $q->param("QTemp")) {
            print "File : $q->param(\"QTemp\")\n";
        }
         htmlReturnToQuery();
        htmlBottom();
  } else {
  htmlTop();
  print "<basefont face=\"verdana,arial,helvetica,sans-serif\">\n";
   print "<table border=0   cellpadding=5 cellspacing=0 width=\"100%\">\n";
    print "<tr bgcolor=\"#ffdc99\">\n";
  print "<td align=left> <font size=\"-1\"><b>\n";
  print "<a href=$amshome_page>AMS</a>\n";
  print "&nbsp; <a href=$amscomp_page>Computing</a>\n";
  print "</b>\n";
   my $href=$self->{HTTPhtml}."/mm.html";
   print "&nbsp;<b><a href=$href>MC production</a>\n";
   print "</b></td>\n";
  print "<td align=right> <font size=\"-1\"> &nbsp;  <!-- top right corner  --> </font></td>\n";
  print "</tr>\n";
  print "<tr bgcolor=\"#ffdc9f\"><td align=center colspan=2><font size=\"+2\" color=\"#3366ff\"> <b>\n";
  print "AMS02 MC  Database Query Form";
  print "</b></font></td></tr>\n";
  print "<tr bgcolor=\"#ffdc9f\">\n";
  print "<td align=left> <font size=\"-1\"><!-- lower left text --> &nbsp;</td>\n";
  print "<td align=right> <font size=\"-1\"> &nbsp; </font></td></tr>\n";
  print "<tr><td colspan=2 align=right> <font size=\"-1\">\n";
  print " </font></td></tr>\n";
  print "</table>\n";
  print "<p>\n";
  print "<ul>\n";
  print "<font size=\"3\"><TR><TD><b>\n";
  print " This is an interface to the AMS MC02 Remote/Client Database </TD></TR> \n";
  print "<TR><TD> \n";
  print "All comments (to <font color=\"green\"> alexei.klimentov\@cern.ch, vitali.choutko\@cern.ch </font>) appreciated. </TD></TR>\n";
  print "</ul>\n";
  print "<font size=\"2\">\n";
  print "<li> Catalogues are updated nightly.\n";
  print "<li> To browse AMS01 data and AMS02 NTuples produced before March 2002 click <a href=$dbqueryR> here </a>\n";
  print "<p>\n";
   print "<TR><B><font color=green size= 4> Select by key(s) (you can select multiple keys) </font>";
   print "<p>\n";
   print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
# print templates files and datasets
             my @tempnam=();
             my $hash={};
             foreach my $cite (@{$self->{TempT}}){
              if(not ($cite->{filename} =~/^\./)){
               push @tempnam, $cite->{filename};
               $hash->{$cite->{filename}}=$cite->{filedesc};
             }
          }
#          print "<tr><td><b><font color=\"red\" size=2>Job Template</font></b>\n";
#          print "</td><td>\n";
#          print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
#          print "<tr valign=middle><td align=left><b><font size=\"-1\"> File : </b></td> <td colspan=1>\n";
#          print "<select name=\"QTemp\" >\n";
#          print "<option value=\"Any\">  </option>\n";
#          foreach my $template (@tempnam) {
#            print "<option value=\"$template\">$template </option>\n";
#          }
#          print "</select>\n";
#          print "</b></td></tr>\n";
           my $found=0;
           @tempnam=();
           $hash={};
           my @desc=();
           my $cite={};
           foreach my $dataset (@{$self->{DataSetsT}}){
                 foreach $cite (@{$dataset->{jobs}}){
                 if(not ($cite->{filename} =~/^\./)){
                  push @tempnam, $cite->{filename};
                  $hash->{$cite->{filename}}=$cite->{filedesc};
                  push @desc, $hash -> {$cite->{filename}}=$cite->{filedesc};
                 }
             }
            }
            print "<tr valign=middle><td align=left><b><font size=\"-1\"> Dataset :</b></td><td colspan=1>\n";
            print "<select name=\"QTemp\" >\n";
            print "<option value=\"Any\">  </option>\n";
            my $i=0;
            foreach my $template (@tempnam) {
             print "<option value=\"$template\">$desc[$i] </option>\n";
             $i++;
            }
            print "</select>\n";
            print "</b></td></tr>\n";
        htmlTableEnd();
# Job Parameters
          print "<tr><td><b><font color=\"blue\" size=2>Job Parameters</font></b>\n";
          print "</td><td>\n";
          print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
          print "<tr valign=middle><td align=left><b><font size=\"-1\"> Particle : </b></td> <td colspan=1>\n";
          print "<select name=\"QPart\" >\n";
              my $ts=$self->{tsyntax};
              my %hash=%{$ts->{particles}};
              my @keysa=sort {$hash{$a} <=>$hash{$b}} keys %{$ts->{particles}};
              print "<option value=\"Any\">  </option>\n";
              foreach my $particle (@keysa) {
                print "<option value=\"$particle\">$particle </option>\n";
              }
          print "</select>\n";
          print "</b></td></tr>\n";
            htmlTextField("Momentum min","number",7,1.,"QMomI","[GeV/c]");  
            htmlTextField("Momentum max","number",7,200.,"QMomA","[GeV/c]");  
            htmlTextField("Setup","text",20,"AMS02","QSetup"," ");
            htmlTextField("Trigger Type ","text",20,"AMSParticle","QTrType"," ");
           htmlTableEnd();
      htmlTableEnd();
     print "<p><br>\n";
     print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";
     print "<input type=\"submit\" name=\"queryDB\" value=\"Submit\">        ";
     print "<input type=\"reset\" name=\"queryDB\" value=\"Reset\"></br><br>        ";
     print "</form>\n";
  htmlBottom();
 }
}

sub listProductionSetPeriods {
    my $self  = shift;

    my $pid   = 0;                 # period id
    my $name  = 0;                 # nick name
    my $begin = 0;                 # start time
    my $end   = 0;                 # end time
    my $status      = 'unknown';   # status
    my $version     = 'unknown';   # program version
    my $description = 'xyz';       # description

    my $sql = undef;
    my $ret = undef;
  
    $sql = "SELECT did, name, begin, end, status, version description 
             FROM productionset WHERE did > 0 ORDER by begin desc";
    $ret=$self->{sqlserver}->Query($sql);

     print "<b><h2><A Name = \"mc02sets\"> </a></h2></b> \n";
     htmlTable("MC02 Production Sets");
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<td align=center><b><font color=\"blue\">ID </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Name</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Begin </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >End </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Status</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Version </font></b></td>";
               print "</tr>\n";
     if (defined $ret->[0][0]) {
      foreach my $mc (@{$ret}){
          $pid      = $mc->[0];
          $name     = $mc->[1];
          $begin    = $mc->[2];
          $end      = $mc->[3];
          $status   = trimblanks($mc->[4]);
          $version  = $mc->[5];
          $description = $mc->[6];

          my $tbegin = EpochToDDMMYYHHMMSS($begin);
          my $tend   = EpochToDDMMYYHHMMSS($end);
          if ($end == 0) { 
              $tend = '->';
          }

          my $color=statusColor($status);

               print "<td align=center><b><font color=$color>$pid </font></b></td>";
               print "<td align=center><b><font color=$color >$name</font></b></td>";
               print "<td align=center><b><font color=$color >$tbegin </font></b></td>";
               print "<td align=center><b><font color=$color >$tend </font></b></td>";
               print "<td align=center><b><font color=$color >$status</font></b></td>";
               print "<td align=center><b><font color=$color >$version </font></b></td>";
               print "</tr>\n";


      }
  }
       htmlTableEnd();
     htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
}

sub listStat {
    my $self = shift;

    my $jobsreq    = 0;  # active jobs
    my $jobsdone   = 0;  # successfully finished
    my $jobsfailed = 0;  # failed
    my $jobstimeout= 0;  # timeout
    my $trigreq    = 0;  # requested events
    my $trigdone   = 0;  # processed levent-fevent+1
    my $timestart  = 0;  # 1st job request time
    my $lastupd    = 0;  # last job request time

    my $sql        = undef;
    my $ret        = undef;

     print "<b><h2><A Name = \"stat\"> </a></h2></b> \n";
     htmlTable("MC02 Jobs");

# first job timestamp
      $sql="SELECT MIN(Jobs.time), MAX(Jobs.timestamp) FROM Jobs, Cites 
                WHERE Jobs.cid=Cites.cid and Cites.name!='test' and Jobs.timestamp > $ProductionStartTime";
      $ret=$self->{sqlserver}->Query($sql);
      if (defined $ret->[0][0]) {
       $timestart = $ret->[0][0];
      }
      if (defined $ret->[0][1]) {
       $lastupd=localtime($ret->[0][1]);
      }

# running (active jobs)
      $sql = "SELECT COUNT(jobs.jid), SUM(triggers) FROM Jobs, Cites WHERE 
                     (Jobs.cid != Cites.cid AND
                      Cites.cid = (SELECT Cites.cid FROM Cites WHERE Cites.name = 'test')) AND 
                      Jobs.timestamp > $ProductionStartTime";
      $ret = $self->{sqlserver}->Query($sql);
      if (defined $ret->[0][0]) {
         $jobsreq = $ret->[0][0];
         $trigreq = $ret->[0][1];
      }
     $sql = "SELECT COUNT(runs.jid) FROM Jobs, Runs, Cites   
              WHERE  runs.jid = jobs.jid AND
                     (Jobs.cid != Cites.cid AND
                      Cites.cid = (SELECT Cites.cid FROM Cites WHERE Cites.name = 'test')) AND
                      Jobs.timestamp > $ProductionStartTime";
      $ret = $self->{sqlserver}->Query($sql);
      if (defined $ret->[0][0]) {
         $jobsreq = $jobsreq - $ret->[0][0];
      }
    $sql = "SELECT COUNT(runs.jid) FROM Jobs, Runs, Cites 
              WHERE  runs.jid = jobs.jid AND 
                     (runs.status='Foreign' OR runs.status='Processing') AND
                     (Jobs.cid != Cites.cid AND
                      Cites.cid = (SELECT Cites.cid FROM Cites WHERE Cites.name = 'test')) AND
                      Jobs.timestamp > $ProductionStartTime";
    $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
         $jobsreq += $ret->[0][0];
     }

# finished/completed jobs
    $sql = "SELECT COUNT(jid), sum(fevent), sum(levent) FROM Runs 
                WHERE status='Finished' OR status='Completed' AND 
                Runs.submit > $ProductionStartTime";

    $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
         $jobsdone = $ret->[0][0];
         $trigdone = $ret->[0][2] - $ret->[0][1] + $jobsdone
     }

# failed/unchecked jobs
    $sql = "SELECT COUNT(jid) FROM Runs 
                WHERE status='Failed' OR status='Unchecked' AND 
                Runs.submit > $ProductionStartTime";


    $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
         $jobsfailed = $ret->[0][0];
     }

# timeout jobs
    $sql = "SELECT COUNT(jid) FROM Runs WHERE status='TimeOut' AND Runs.submit > $ProductionStartTime";
    $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
         $jobstimeout = $ret->[0][0];
     }

# ntuples, runs
               $sql="SELECT COUNT(run), SUM(SIZEMB) from ntuples WHERE ntuples.timestamp> $ProductionStartTime";
               $ret=$self->{sqlserver}->Query($sql);
               my $nntuples=0;
               my $nsizegb =0;
               if(defined $ret->[0][0]){
                $nntuples= $ret->[0][0];
                $nsizegb = sprintf("%.1f",$ret->[0][1]/1000);
               }
# GB on CASTOR
               $sql="SELECT COUNT(run), SUM(SIZEMB) from ntuples 
                     where castortime !=0 AND ntuples.timestamp> $ProductionStartTime";
               $ret=$self->{sqlserver}->Query($sql);
               my $cntuples=0;
               my $csizegb =0;
               if(defined $ret->[0][0]){
                $cntuples= $ret->[0][0];
                $csizegb = sprintf("%.1f",$ret->[0][1]/1000);
               }
#
               my $timenow = time();
               my $timepassed = sprintf("%.1f",($timenow - $timestart)/60/60/24);
               
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<td align=center><b><font color=\"blue\">Jobs </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Jobs</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Jobs </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Jobs </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Events</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Events </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Total </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Size </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >CASTOR </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >CASTOR </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Last Update </font></b></td>";
              print "</tr>\n";
               print "<td align=center><b><font color=\"blue\">Started </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Finished </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Failed </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >TimeOut </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Requested </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Processed </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >NTuples</font></b></td>";
               print "<td align=center><b><font color=\"blue\" > [GB]     </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >NTuples</font></b></td>";
               print "<td align=center><b><font color=\"blue\" > [GB]     </font></b></td>";
               print "<td align=center><b><font color=\"green\" > $timepassed days</font></b></td>";
              print "</tr>\n";
           my $color="black";
           my $reqevents = $trigreq;
           if ($trigreq => 1000 && $trigreq <= 1000000) {
               $reqevents = sprintf("%.2fK",$trigreq/1000.);
	   } else {
               $reqevents = sprintf("%.2fM",$trigreq/1000000.);
	   }

           my $donevents = $trigdone;
           if ($trigdone=> 1000 && $trigdone<= 1000000) {
               $donevents = sprintf("%.2fK",$trigdone/1000.);
 	   } else {
               $donevents = sprintf("%.2fM",$trigdone/1000000.);
	   }

 
           print "
                  <td align=center><b><font color=$color> $jobsreq </font></td></b>
                  <td align=center><b><font color=$color> $jobsdone </font></b></td>
                  <td align=center><b><font color=$color> $jobsfailed </font></td></b>
                  <td align=center><b><font color=$color> $jobstimeout </font></td></b>
                  <td align=center><b><font color=$color> $reqevents </font></td></b>
                  <td align=center><b><font color=$color> $donevents </font></b></td>
                  <td align=center><b><font color=$color> $nntuples </font></b></td>
                  <td align=center><b><font color=$color> $nsizegb </font></b></td>
                  <td align=center><b><font color=$color> $cntuples </font></b></td>
                  <td align=center><b><font color=$color> $csizegb </font></b></td>
                  <td align=center><b><font color=$color> $lastupd </font></b></td>\n";

          print "</font></tr>\n";
 

       htmlTableEnd();
     htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";

     htmlTable("MC02 Datasets");
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     print "<td align=center><b><font color=\"blue\">Dataset </font></b></td>";
     print "<td align=center><b><font color=\"blue\" >Events </font></b></td>";
     print "<td align=center><b><font color=\"blue\" >Events </font></b></td>";
     print "</tr>\n";
     print "<td align=center><b><font color=\"blue\">           </font></b></td>";
     print "<td align=center><b><font color=\"blue\"> Requested </font></b></td>";
     print "<td align=center><b><font color=\"blue\" >Processed</font></b></td>";
     print "</tr>\n";


     $sql = "SELECT did, name FROM datasets where did > 111";
     my $r5=$self->{sqlserver}->Query($sql);
     print_bar($bluebar,3);
     if(defined $r5->[0][0]){
      foreach my $ds (@{$r5}){
          my $did       = $ds->[0];
          my $dataset   = trimblanks($ds->[1]);
          $sql = "SELECT SUM(levent), SUM(fevent), COUNT(fevent) FROM Jobs, Runs, Cites  
                  WHERE 
                    Jobs.did = $did AND
                    Jobs.jid = Runs.jid AND  
                    (Runs.status='Completed' OR Runs.status='Finished') AND 
                    (Jobs.cid != Cites.cid AND
                      Cites.cid = (SELECT Cites.cid FROM Cites WHERE Cites.name = 'test')) 
                      AND Jobs.timestamp> $ProductionStartTime";
           my $r6=$self->{sqlserver}->Query($sql);
           my $events = 0;
           if(defined $r6->[0][0]){
             $events = $r6->[0][0] - $r6->[0][1] + $r6->[0][2];
             if ($events > 1000 && $events <= 1000000) {
                 $events=sprintf("%.2fK",$events/1000);
             } elsif ($events > 1000000) {
                 $events=sprintf("%.2fM",$events/1000000);
             }
           }            
          $sql = "SELECT SUM(triggers) FROM Jobs, Cites   
                  WHERE Jobs.did = $did AND
                    (Jobs.cid != Cites.cid AND
                      Cites.cid = (SELECT Cites.cid FROM Cites WHERE Cites.name = 'test')) 
                      AND Jobs.timestamp> $ProductionStartTime";
          my $r7=$self->{sqlserver}->Query($sql);
          my $triggers = 0;
           if(defined $r7->[0][0]){
             $triggers = $r7->[0][0];
             if ($triggers > 1000 && $triggers <= 1000000) {
                 $triggers=sprintf("%.2fK",$triggers/1000);
             } elsif ($triggers > 1000000) {
                 $triggers=sprintf("%.2fM",$triggers/1000000);
             }
           }            
       print "<td align=left><b><font color=\"black\"> $dataset </font></b></td>";
       print "<td align=center><b><font color=\"black\"> $triggers </font></b></td>";
       print "<td align=center><b><font color=\"black\" >$events</font></b></td>";
       print "</tr>\n";
      }
      htmlTableEnd();
    }
                   
     htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
}
    
sub listCites {
    my $self = shift;
    print "<b><h2><A Name = \"cites\"> </a></h2></b> \n";
     htmlTable("MC02 Cites ");
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     my $sql="SELECT cid,descr, name, status, maxrun FROM Cites ORDER by name";
     my $r3=$self->{sqlserver}->Query($sql);
              print "<td><b><font color=\"blue\">Cite </font></b></td>";
              print "<td><b><font color=\"blue\" >Cite</font></b></td>";
              print "<td><b><font color=\"blue\" >Jobs</font></b></td>";
              print "<td><b><font color=\"blue\" >Jobs</font></b></td>";
              print "<td><b><font color=\"blue\" >Jobs</font></b></td>";
              print "<td><b><font color=\"blue\" >Last Job</font></b></td>";
              print "<td><b><font color=\"blue\" >Last Job</font></b></td>";
              print "<tr>\n";
              print "<td><b><font color=\"blue\">   </font></b></td>";
              print "<td><b><font color=\"blue\" >Type  </font></b></td>";
              print "<td><b><font color=\"blue\" >Act.</font></b></td>";
              print "<td><b><font color=\"blue\" >Ends</font></b></td>";
              print "<td><b><font color=\"blue\" >Failed</font></b></td>";
              print "<td><b><font color=\"blue\" >Start Time</font></b></td>";
              print "<td><b><font color=\"blue\" >End Time</font></b></td>";
              print "<tr>\n";
     print_bar($bluebar,3);
     if(defined $r3->[0][0]){
      foreach my $cite (@{$r3}){
          my $cid    = $cite->[0];
          my $descr  = $cite->[1];
          my $name   = $cite->[2];
          my $status = $cite->[3];
          my $maxrun = $cite->[4];
# 4.12.03          my $run=(($cid-1)<<27)+1;
          my $run=(($cid-1)<<$MAX_RUN_POWER)+1;
      
          my $r4 = undef;

          my $laststarttime = 0;
          my $starttime     = "---";
          $sql = "SELECT MAX(TIME) FROM Jobs WHERE cid=$cid AND Jobs.timestamp> $ProductionStartTime";
          $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {
            $laststarttime = $r4->[0][0];
            $starttime = EpochToDDMMYYHHMMSS($laststarttime);
          };
          my $lastendtime = 0;
          my $endtime       = "---";
          $sql = "SELECT MAX(jobs.time) FROM Jobs, Runs WHERE 
                     jobs.cid = $cid AND
                     jobs.jid = runs.jid AND           
                     (Runs.Status = 'Finished'  OR 
                      Runs.Status = 'Completed' OR
                      Runs.Status = 'Failed'    OR
                      Runs.Status = 'Unchecked') AND Jobs.timestamp> $ProductionStartTime";
          $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {
           $lastendtime = $r4->[0][0];
           $endtime = EpochToDDMMYYHHMMSS($lastendtime);
          };

          my $jobs = 0;   # total jobs
          $sql = "SELECT COUNT(jid) FROM Jobs WHERE cid=$cid AND Jobs.timestamp> $ProductionStartTime";
          $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {$jobs = $r4->[0][0]};

          my $jobsa = 0;  # processing jobs
          $sql = "SELECT COUNT(jobs.jid)  FROM Jobs, Runs 
                   where (jobs.jid=runs.jid and cid=$cid) AND Jobs.timestamp> $ProductionStartTime";
          $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {$jobsa = $jobs - $r4->[0][0]};
          $sql = "SELECT COUNT(Jobs.jid) FROM Jobs, Runs WHERE 
                     Jobs.jid=Runs.jid AND cid=$cid AND 
                     (Runs.Status = 'Foreign' OR 
                      Runs.Status = 'Processing') AND Jobs.timestamp> $ProductionStartTime";
          $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {$jobsa += $r4->[0][0]};

          my $jobsc = 0;
          $sql = "SELECT COUNT(Jobs.jid) FROM Jobs, Runs WHERE 
                     Jobs.jid=Runs.jid AND cid=$cid AND 
                     (Runs.Status = 'Finished' OR 
                      Runs.Status = 'Completed') AND Jobs.timestamp> $ProductionStartTime";
          $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {$jobsc = $r4->[0][0]};


          my $jobsf = 0;
          $sql = "SELECT COUNT(Jobs.jid) FROM Jobs, Runs WHERE 
                     Jobs.jid=Runs.jid AND cid=$cid AND 
                     (Runs.Status = 'Failed' OR 
                      Runs.Status = 'TimeOut' OR 
                      Runs.Status = 'Unchecked') AND Jobs.timestamp> $ProductionStartTime";
          $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {$jobsf = $r4->[0][0]};

          print "<tr><font size=\"2\">\n";
          print "<td><b> $descr ($name) </td>
                 <td><b> $status </td>
                 <td><b> $jobsa </td></b>
                 <td><b> $jobsc </b></td>
                 <td><b> $jobsf </b></td>
                 <td><b> $starttime </b></td>
                 <td><b> $endtime </b></td>
                 \n";
          print "</font></tr><p></p>\n";
      }
  }

       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
}


sub listDisks {
    my $self    = shift;
    my $lastupd = undef; 
    my $sql     = undef;

    my $dpath = undef;
    my $fs    = undef;
    my $usedGBMC = 0;
    my $totalGBMC= 0;

     $self->setActiveProductionSet();
    

    $sql="SELECT MAX(timestamp) FROM Filesystems";
    my $r0=$self->{sqlserver}->Query($sql);
    if( defined $r0->[0][0]){
      $lastupd=localtime($r0->[0][0]);
     }
     print "<b><h2><A Name = \"disks\"> </a></h2></b> \n";
     print "<TR><B><font color=green size= 5><b><font color=green> Disks and Filesystems </font></b>";
     if (defined $lastupd) {
      print "<font color=green size=3><b><i> (Checked : $lastupd) </i></b></font>";
     }
     print "<p>\n";
     print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
              print "<table border=1 width=\"100%\" cellpadding=0 cellspacing=0>\n";
              print "<td><b><font color=\"blue\" >Filesystem </font></b></td>";
              print "<td><b><font color=\"blue\" >GBytes </font></b></td>";
              print "<td><b><font color=\"blue\" >MC [GB] </font></b></td>";
              print "<td><b><font color=\"blue\" >Free [GB] </font></b></td>";
              print "<td><b><font color=\"blue\" >Status </font></b></td>";
     print_bar($bluebar,3);
     $sql="SELECT host, disk, path, totalsize, occupied, available, status, 
           timestamp FROM Filesystems ORDER BY available DESC";
     my $r3=$self->{sqlserver}->Query($sql);
     if(defined $r3->[0][0]){
      foreach my $dd (@{$r3}){
          $usedGBMC= 0;
          $dpath = $dd->[1].$dd->[2];
          if ($dd->[1] =~ /vice/) {
           $dpath  = $dd->[2];
          }
          $fs    = $dd->[0].":".$dpath;
          if (not defined $ActiveProductionSet || $ActiveProductionSet eq $UNKNOWN) {
          } else {
           $dpath = $dpath."/".$ActiveProductionSet;
          } 
          $sql = "SELECT SUM(sizemb) FROM ntuples WHERE PATH like '$dpath%'";
          my $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {
              $usedGBMC = sprintf("%6.1f",$r4->[0][0]/1000);
              $totalGBMC = $totalGBMC + $r4->[0][0];
          }


          my $size   = $dd->[3];
          my $used   = $dd->[4];
          my $avail  = $dd->[5];
          my $status   = trimblanks($dd->[6]);
          print "<tr><font size=\"2\">\n";
          my $color=statusColor($status);
          print "<td><b> $fs </b></td>
                 <td align=middle><b> $size </td>
                 <td align=middle><b> $usedGBMC </td>
                 <td align=middle><b> $avail </b></td>
                 <td><font color=$color><b> $status </font></b></td>\n";
          print "</font></tr>\n";
      }
  }
    $sql="SELECT SUM(totalsize), SUM(occupied), SUM(available) FROM Filesystems";
    my $r4=$self->{sqlserver}->Query($sql);
    if(defined $r4->[0][0]){
      foreach my $tt (@{$r4}){
          my $total = $tt->[0];
          my $occup = $tt->[1];
          my $free  = $tt->[2];
          my $color="green";
          my $status="ok";
          my $totalGB = sprintf("%6.1f",$totalGBMC/1000);
          if ($free < $total*0.1) {
            $color="magenta";
            $status=" no space";}
          print "<tr><font size=\"2\">\n";
          print "<td><font color=$color><b> Total </b></td>
                 <td align=middle><b> $total </td>
                 <td align=middle><b> $totalGB </td>
                 <td align=middle><b> $free </b></td>
                 <td><font color=$color><b> $status </font></b></td>\n";
          print "</font></tr>\n";
      }
     }

       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
}


sub listMails {
    my $self = shift;
     print "<b><h2><A Name = \"mails\"> </a></h2></b> \n";
     htmlTable("MC02 Authorized Users");
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     my $sql="SELECT address, Mails.name, rsite, requests, Mails.cid, Cites.cid, 
              Cites.name, Mails.status FROM  Mails, Cites WHERE Cites.cid=Mails.cid 
              ORDER BY Cites.name, Mails.name";

     my $r3=$self->{sqlserver}->Query($sql);
              print "<tr><td><b><font color=\"blue\">Cite </font></b></td>";
              print "<td><b><font color=\"blue\" >User </font></b></td>";
              print "<td><b><font color=\"blue\" >e-mail </font></b></td>";
              print "<td><b><font color=\"blue\" >Responsible </font></b></td>";
              print "<td><b><font color=\"blue\" >Requests </font></b></td>";
              print "<td><b><font color=\"blue\" >Status </font></b></td>";
     print_bar($bluebar,3);
     if(defined $r3->[0][0]){
      foreach my $mail (@{$r3}){
          my $name   = $mail->[1];
          my $email  = $mail->[0];
          my $req    = $mail->[3];
          my $cite   = $mail->[6];
          my $status   = $mail->[7];
          my $resp   = 'no';
          if ($mail->[2] == 1) { $resp = 'yes';}
          print "<tr><font size=\"2\">\n";
          print "<td><b> $cite </td><td><b> $name </b></td><td><b> [$email] </td><td><b> $resp </td><td><b> $req </b></td><td><b> $status </b></td>\n";
          print "</font></tr>\n";
      }
  }

       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
}


sub listServers {
    my $self = shift;
    print "<b><h2><A Name = \"servers\"> </a></h2></b> \n";
     print "<TR><B><font color=green size= 5><a href=$monmcdb><b><font color=blue> MC Servers </font></a><font size=3><i>(Click  servers to check current production status)</font></i></font>";
    print "<p>\n";
    print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
    print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
    my $sql="SELECT dbfilename, status, createtime, lastupdate FROM Servers ORDER BY lastupdate DESC";
    my $r3=$self->{sqlserver}->Query($sql);
    print "<tr><td><b><font color=\"blue\">Server </font></b></td>";
    print "<td><b><font color=\"blue\" >Started </font></b></td>";
    print "<td><b><font color=\"blue\" >LastUpdate </font></b></td>";
    print "<td><b><font color=\"blue\" >Status </font></b></td>";
    print_bar($bluebar,3);
    my $deadserver = 0;
    if(defined $r3->[0][0]){
        foreach my $srv (@{$r3}){
            my $name      = $srv->[0];
            my $status    = $srv->[1];
            my $starttime = EpochToDDMMYYHHMMSS($srv->[2]);
            my $lastupd   = $srv->[3];
            my $lasttime  = EpochToDDMMYYHHMMSS($lastupd);
            my $time      = time();
            if ($status eq 'Active' or $deadserver == 0) {
             if ($time - $lastupd < $srvtimeout) {
             print "<td><b> $name </td><td><b> $starttime </td><td><b> $lasttime </td><td><b> $status </td> </b>\n";
             } else {
             my $color = statusColor($status);
             print "<td><b> $name </td><td><b> $starttime </td><td><b><font color=$color> $lasttime </font></td><td><b><font color=$color> $status </font></td> </b>\n";
           }
             print "</font></tr>\n";
             if ($status eq 'Dead') { $deadserver++;}
         }
        }
    }
    htmlTableEnd();
    htmlTableEnd();
    print_bar($bluebar,3);
    print "<p></p>\n";
}




sub listJobs {
    my $self = shift;
    
    my $ret  = undef;
    my $sql  = undef;
    my $href = undef;    

    my $timenow = time();
    my $timelate= $timenow - 30*24*60*60 +1; # list jobs submitted 30 days ago or earlier

    my @runId     = ();
    my @runStatus = ();

    print "<b><h2><A Name = \"jobs\"> </a></h2></b> \n";
    htmlTable("MC02 Jobs (25 latest jobs per cite submitted earlier than 30 days ago)");
    print_bar($bluebar,3);


    $sql = "SELECT name FROM cites ORDER BY name";
    $ret=$self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
     print " <Table> \n";
     print " <TR><TH rowspan=2>MC Production Cites : <br> \n";
     foreach my $cite (@{$ret}) {
      my $rc = $cite->[0];
      print "</th> <th><small> \n";
      $href = $self->{HTTPcgi}."/rc.dsp.cgi\#jobs-".$rc;
      print "<a href= $href target=\"status\"> <b><font color=tomato>$rc</b></font></a>\n";
     }
     print "</TR></TABLE> \n";
    }

    $sql="SELECT 
                 Jobs.jid, Jobs.jobname,
                 Jobs.time, Jobs.timeout, 
                 Jobs.triggers,
                 Cites.cid, Cites.name, Cites.descr,
                 Mails.name  
           FROM   Jobs, Cites, Mails  
            WHERE  Jobs.cid=Cites.cid AND 
                     Jobs.mid=Mails.mid AND
                      Jobs.timestamp > $timelate AND
                       Jobs.timestamp > $ProductionStartTime 
             ORDER  BY Cites.name, Jobs.jid DESC";

     my $r3=$self->{sqlserver}->Query($sql);

     print_bar($bluebar,3);
     my $newline = " ";
     my $savcite = "unknown";
     my $njobs   = 0;    

     if(defined $r3->[0][0]){
      foreach my $job (@{$r3}){
          my $jid       = $job->[0];
          my $name      = $job->[1];
          my $starttime = EpochToDDMMYYHHMMSS($job->[2]);

          my $texpire = 0;
          if (defined $job->[2]) { $texpire = $job->[2]}; 
          if (defined $job->[3]) {$texpire  += $job->[3]};

          my $expiretime= EpochToDDMMYYHHMMSS($texpire); 
          my $trig      = $job->[4];
          my $cid       = $job->[5];
          my $cite      = $job->[6];
          my $citedescr = $job->[7];
          if ($cite ne $savcite) {
              $savcite = $cite;
              $njobs   = 0;
          }
          my $user      = $job->[8];

          my $status    = "Submitted";

          my $color     = "black";
          if (defined $job->[9]) {
              $status = $job->[9];
              $color  = statusColor($status);
          }

          if ($newline ne $cite) { 
               $newline = $cite;

               @runId =();
               @runStatus = ();
               $sql="SELECT Runs.jid, Runs.status from Runs, Jobs 
                     WHERE (Jobs.cid=$cid AND Runs.jid=Jobs.jid) AND 
                           Jobs.timestamp > $ProductionStartTime";
               $r3=$self->{sqlserver}->Query($sql);
               if (defined $r3->[0][0]) {
                foreach my $r (@{$r3}){
                 push @runId, $r->[0];
                 push @runStatus, $r->[1];
                }
               }



               print "</table></p> \n";
               print "<p>";
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<td><font color=tomato size=\"+1\"><b>$citedescr ($cite)</b></font></td>\n";
               print "</table>\n";
               my $citename="jobs-".$cite;
               print "<A Name = $citename> </a>\n";
                print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
                 print "<td><b><font color=\"blue\">JobId </font></b></td>";
                 print "<td><b><font color=\"blue\" >Owner </font></b></td>"; 
                 print "<td><b><font color=\"blue\" >JobName </font></b></td>";
                 print "<td><b><font color=\"blue\" >Submit Time </font></b></td>";
                 print "<td><b><font color=\"blue\" >Expire Time </font></b></td>";
                 print "<td><b><font color=\"blue\" >Triggers </font></b></td>";
                 print "<td><b><font color=\"blue\" >Status </font></b></td>";
                 print "</tr>\n";
          }
          if ($njobs < $PrintMaxJobsPerCite) {
           $status = "Submitted";
           $color  = "black";
           for (my $i=0; $i<$#runId+1; $i++) {
               if ($runId[$i] == $jid) {
                 $status = $runStatus[$i];
                 $color  = statusColor($status);
                 last;
               }
            }
                 print "
                  <td><b><font color=$color> $jid </font></td></b>
                  <td><b><font color=$color> $user </font></b></td>
                  <td><b><font color=$color> $name </font></td></b>
                  <td><b><font color=$color> $starttime </font></b></td>
                  <td><b><font color=$color> $expiretime </font></b></td>
                  <td><b><font color=$color> $trig </font></b></td>
                  <td><b><font color=$color> $status </font></b></td>\n";

                 print "</font></tr>\n";
             }
          $njobs++;
      }
    }  
     htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
}

sub listRuns {
    my $self = shift;
    my $rr   = 0;

     print "<b><h2><A Name = \"runs\"> </a></h2></b> \n";
     htmlTable("MC02 Runs");
     my $href=$self->{HTTPcgi}."/rc.o.cgi?queryDB04=Form";
     print "<tr><font color=blue><b><i> Only recent 50 runs are listed,  to get complete list 
            <a href=$href> click here</a>
            </b><i></font></tr>\n";

              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     my $sql="SELECT Runs.run, Runs.jid, Runs.submit, Runs.status 
              FROM   Runs WHERE Runs.submit > $ProductionStartTime 
              ORDER  BY Runs.submit DESC, Runs.jid";
     my $r3=$self->{sqlserver}->Query($sql);
              print "<tr><td><b><font color=\"blue\" >JobId </font></b></td>";
              print "<td><b><font color=\"blue\">Run </font></b></td>";
              print "<td><b><font color=\"blue\" >Started </font></b></td>";
              print "<td><b><font color=\"blue\" >Status </font></b></td>";
             print "</tr>\n";
     print_bar($bluebar,3);
     if(defined $r3->[0][0]){
      foreach my $run (@{$r3}){
          my $nn        = $run->[0];
          my $jid       = $run->[1];
          my $starttime = EpochToDDMMYYHHMMSS($run->[2]); 
          my $status    = $run->[3];
          my $color = statusColor($status);
           print "<td><b><font color=$color> $jid       </font></td></b><td><b> $nn </td>
                  <td><b><font color=$color> $starttime </font></b></td>
                  <td><b><font color=$color> $status    </font></b></td> \n";
          print "</font></tr>\n";
          $rr++;
          if ($rr > 50) { last;}
      }
  }
       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
}

sub listNtuples {
    my $self = shift;
    my $nn   = 0;

    my $time24h = time() - 24*60*60;

     print "<b><h2><A Name = \"ntuples\"> </a></h2></b> \n";
     print "<TR><B><font color=green size= 5><b>MC NTuples </font></b>";
     my $href = $self->{HTTPcgi}."/rc.o.cgi?queryDB04=Form";
     print "<tr><font color=blue><b><i> DSTs produced in last 24h, to get complete list 
            <a href=$href> click here</a>
            </b><i></font></tr>\n";
     print "<p>\n";


     print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
              print "<table border=1 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     my $sql="SELECT Ntuples.run, Ntuples.jid, Ntuples.nevents, Ntuples.neventserr, 
                     Ntuples.timestamp, Ntuples.status, Ntuples.path
              FROM   Ntuples
              WHERE  Ntuples.timestamp > $time24h  
              ORDER  BY Ntuples.timestamp DESC, Ntuples.jid";
     my $r3=$self->{sqlserver}->Query($sql);
              print "<tr><td width=10% align=left><b><font color=\"blue\" > JobId </font></b></td>";
              print "<td width=10%><b><font color=\"blue\"> Run </font></b></td>";
              print "<td width=15%><b><font color=\"blue\" > LastUpdate </font></b></td>";
              print "<td td align=middle><b><font color=\"blue\" > FilePath </font></b></td>";
              print "<td width=10%><b><font color=\"blue\" > Events </font></b></td>";
              print "<td width=10%><b><font color=\"blue\" > Errors </font></b></td>";
              print "<td width=10%><b><font color=\"blue\" > Status </font></b></td>";
             print "</tr>\n";
     print_bar($bluebar,3);
     if(defined $r3->[0][0]){
      foreach my $nt (@{$r3}){
          my $run       = $nt->[0];
          my $jid       = $nt->[1];
          my $nevents   = $nt->[2];
          my $nerrors   = $nt->[3];
          my $starttime = EpochToDDMMYYHHMMSS($nt->[4]); 
          my $status    = $nt->[5];
          my $color     = statusColor($status);
          my $path      = $nt->[6];
           print "<td><b> $jid </td></b><td><b> $run </td>
                  <td><b> $starttime </b></td>
                  <td><b> $path </b></td> 
                  <td align=middle><b> $nevents </b></td> 
                  <td align=middle><b> $nerrors </b></td> 
                  <td align=middle><b><font color=$color> $status </font></b></td> \n";
          print "</font></tr>\n";
          $nn++;
          if ($nn > 100) { last;}
      }
  }
       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
}

sub AllDone{
    htmlTop();
     my $self = shift;
     print "<font size=\"5\" color=\"green\">Download finished. Your request was successfully sent to $self->{CEM}";       htmlReturnToMain();
    htmlBottom();
}

sub DownloadTime {
        my $self = shift;

        my $filedb="$self->{UploadsDir}/$self->{FileDB}";
        $self->{FileDBTimestamp}=(stat($filedb))[9];

        $filedb="$self->{UploadsDir}/$self->{FileAttDB}";
        $self->{FileAttDBTimestamp}=(stat($filedb))[9];

        $filedb="$self->{UploadsDir}/$self->{FileBBFTP}";
        $self->{FileBBFTPTimestamp}=(stat($filedb))[9];

        $filedb="$self->{UploadsDir}/$self->{FileBookKeeping}";
        $self->{FileBookKeepingTimestamp}=(stat($filedb))[9];

        $filedb="$self->{UploadsDir}/$self->{FileCRC}";
        $self->{FileCRCTimestamp}=(stat($filedb))[9];

    }

sub Download {
    my $self = shift;
    print "Content-type: text/html\n\n";
    print "<HTML>\n";
    print "<body bgcolor=cornsilk text=black link=#007746 vlink=navy alink=tomato>\n";
    print "<head>\n";
    print "<TITLE>AMS Offline Software</TITLE></HEAD>\n";
    print "<FORM METHOD=\"GET\" action=\"$self->{Name}\">\n";
    print "<TABLE border=0 cellspacing=0 cellpadding=0>\n";
    my $mode = "Client";
    if ($self->{dwldaddon} == 1) {
     $mode = "Standalone";
    }
    print "<br><b><font color=\"blue\" size=\"4\"> $self->{CEM}, mode :  $mode </b></font>\n";
    my $dtime0=EpochToDDMMYYHHMMSS($self->{FileDBLastLoad});
    my $dtime1=EpochToDDMMYYHHMMSS($self->{FileAttDBLastLoad});
    print "<br><font color=\"green\" size=\"4\"><b><i> Last downloaded  : </i></font><b> $dtime0, $dtime1 </b>\n";
    my $dtime=EpochToDDMMYYHHMMSS($self->{FileDBTimestamp});
    print "<br><font color=\"green\" size=\"4\"><b><i>Files updated    : </i></font><b> $dtime </b>\n";
    print "<br><font color=\"red\" size=\"5\"><b><i> It is absolutely mandatory to download files</b></i></font>\n";
    $self->PrintDownloadTable();
    print "</TABLE>\n";
    print "<p>\n";
          my $time; 
          $time = time();
          print "<input type=\"hidden\" name=\"CEM\" value=$self->{CEM}>\n";
          print "<input type=\"hidden\" name=\"UPL0\" value=$time>\n";
          if ($self->{dwldaddon} == 1) {
           print "<input type=\"hidden\" name=\"UPL1\" value=$time>\n";
          } else {
           print "<input type=\"hidden\" name=\"UPL1\" value=0>\n";
          }
          print "<input type=\"submit\" name=\"Download\" value=\"Finish\">\n";
    print "</FORM>\n";
    print "</BODY>\n";
    print "</HTML>\n";
}


sub DownloadSA {
    my $self = shift;
    $self->{UploadsHREF}="AMS02MCUploads";
    $self->{UploadsDir} ="/var/www/cgi-bin/AMS02MCUploads";
    $self->{FileDB}     ="ams02mcdb.tar.gz";
    $self->{FileAttDB}  ="ams02mcdb.addon.tar.gz";
    $self->{FileBBFTP}  ="ams02bbftp.tar.gz";
    $self->{FileBookKeeping}  ="ams02bookkeeping.tar.gz";
    $self->{FileCRC}    ="ams02crc.tar.gz";
    $self->{dwldaddon}  = 1;
    $self->DownloadTime();
    print "Content-type: text/html\n\n";
    print "<HTML>\n";
    print "<body bgcolor=cornsilk text=black link=#007746 vlink=navy alink=tomato>\n";
    print "<head>\n";
    print "<TITLE>AMS Offline Software</TITLE></HEAD>\n";
    print "<TABLE border=0 cellspacing=0 cellpadding=0>\n";
    $self->PrintDownloadTable();
    print "</TABLE>\n";
    print "<p>\n";
    print "</BODY>\n";
    print "</HTML>\n";
}

sub PrintDownloadTable {
    my $self = shift;

    my $file = undef;
    my $dtime= undef;


    print "<tr><td width=700>\n";
    print "<table border=\"0\" cellspacing=\"5\" cellpadding=\"5\">\n";
    print "<P>\n";
    print "<p><tr><td bgcolor=\"#ffefd5\" width=700 valign=top colspan=2>\n";
    print "<font face=\"myriad,arial,geneva,helvetica\">\n";
    print "<TABLE BORDER=2 cellpadding=3 cellspacing=3 BGCOLOR=#eed8ae align=center width=100%>\n";
    print "<TR><br>\n";
    print "<TD><font color=#8b1a1a size=\"6\"><b>The following files are avaialable for download</b></font>:\n";
    print "<br><br>\n";
#
##ams02mcdb tar
    my $download = 1;
    if (defined $self->{FileDBLastLoad}) {
        if ($self->{FileDBLastLoad} > $self->{FileDBTimestamp}) {
            $download = 0;
        }
    }
     $file= $self->{FileDB};
    if ($download == 1) {
     print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$file>  ams02mcdb (tar.gz)</a>
           </font>";
     $dtime=EpochToDDMMYYHHMMSS($self->{FileDBTimestamp});
     print "<font size=\"3\" color=\"red\"><i><b>       ( Updated : $dtime)</b></i></font>\n";
     print "<br><br>";
    } else {
     print "<br><font size=\"4\">
           filedb files (tar.gz)</a>
           </font>";
     $dtime=EpochToDDMMYYHHMMSS($self->{FileDBTimestamp});
     print "<font size=\"3\" color=\"green\"><i><b>       ( Up to date : $dtime)</b></i></font>\n";
     print "<br><br>";
   }
   $download = 1;
#
## ams02mcdbaddon tar
    if (defined $self->{FileAttDBLastLoad}) {
        if ($self->{FileAttDBLastLoad} > $self->{FileAttDBTimestamp}) {
            $download = 0;
        }
    }
    $file= $self->{FileAttDB};
    
    if ($self->{dwldaddon} == 1) {
     if ($download == 1) {
      print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$file>   ams02mcdb.addon (tar.gz)</a>
           </font>\n";
      $dtime=EpochToDDMMYYHHMMSS($self->{FileAttDBTimestamp});
      print "<font size=\"3\" color=\"red\"><i><b>       ( Updated : $dtime)</b></i></font>\n";
      print "<br><br>\n";
     } else {
      print "<br><font size=\"4\">
           filedb att.files (tar.gz)</a>
           </font>\n";
      $dtime=EpochToDDMMYYHHMMSS($self->{FileAttDBTimestamp});
      print "<font size=\"3\" color=\"green\"><i><b>       ( Up to date : $dtime)</b></i></font>\n";
      print "<br><br>\n";
      }
#
## bbftp tar
     my $file= $self->{FileBBFTP};
     print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$file>  bbftp files (tar.gz) - <i> optional </i></a>
           </font>";
     my $dtime=EpochToDDMMYYHHMMSS($self->{FileBBFTPTimestamp});
     print "<font size=\"3\" color=\"green\"><i><b>       ( Updated : $dtime)</b></i></font>\n";
     print "<br><br>";   
#
## crc tar
     $file= $self->{FileCRC};
     print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$file>  CRC Linux exec (tar.gz) - <i> optional </i></a>
           </font>";
     $dtime=EpochToDDMMYYHHMMSS($self->{FileCRCTimestamp});
     print "<font size=\"3\" color=\"green\"><i><b>       ( Updated : $dtime)</b></i></font>\n";
     print "<br><br>";
#
## book-keeping tar
     $file= $self->{FileBookKeeping};
     print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$file>  AMS mysql book-keeping  execs and docs (tar.gz) - <i> optional </i></a>
           </font>";
     $dtime=EpochToDDMMYYHHMMSS($self->{FileBookKeepingTimestamp});
     print "<font size=\"3\" color=\"green\"><i><b>       ( Updated : $dtime)</b></i></font>\n";
     print "<br><br>";   
 }
    print "</TD></TR>\n";
    print "</TABLE>\n";
    print "</td></tr>\n";
    print "</table>\n";
    print "</td></tr>\n";
}
sub ht_init{
  my $self = shift;

  print "<table border=0   cellpadding=5 cellspacing=0 width=\"100%\">";
   print "<tr bgcolor=\"#ffdc99\">\n";
    print "<td align=left> <font size=\"-1\"><b>";
     print  "<a href=$amshome_page>AMS</a>\n";
     print "&nbsp; <a href=$amscomp_page>Computing</a>\n";
     print "</b></font>\n";
      my $href=$self->{HTTPhtml}."/mm.html";
      print "&nbsp;<b><a href=$href>MC production</a>\n";
    print "</b></td>\n";
    print "<td align=right> <font size=\"-1\">\n";
    print "&nbsp;</font></td></tr>\n";
    print "<tr bgcolor=\"#ffdc9f\"><td align=center colspan=2><font size=\"+2\" color=\"#3366ff\">";
    print "<b> AMS MC Production Status \n";
    print "</b></font></td></tr>\n";
    print "<tr bgcolor=\"#ffdc9f\"><td align=left> <font size=\"-1\">\n";
    print "&nbsp</font></td>\n";
    print "<td align=right> <font size=\"-1\">\n";
    print "&nbsp; </font></td></tr> \n";
    print "<tr><td colspan=2 align=right> <font size=\"-1\">\n";
   print "</font></td></tr>\n";
  print "</table>\n";
  print "<table border=0 width=\"100%\">";
   my $time; 
   $time = localtime;
   print "<tr><td><font size=\"-1\"><b>Page Update : $time</b></td>\n";
   my $dbtime = $self->lastDBUpdate();
   $time= EpochToDDMMYYHHMMSS($dbtime); 
   print "<td align=right><font size=\"-1\"><b>Last DB Update : $time </b></td></tr>\n";
   print "</TR></TABLE>\n";
   print "<p></p>\n";
  
#
#
}

sub print_bar {
    my ($bar,$height)    = @_;
    print "<TR><img width=100% height=$height src=\"$bar\">  </TR>\n";
}

sub EpochToDDMMYYHHMMSS {
    my $time = shift;
    my ($sec, $min, $hour, $mday, $month, $year, $wday, $yday, $isdst) = 
          localtime($time);

    $month++;
    $year = $year + 1900;
           
    if ($sec < 10) {$sec = "0".$sec;}
    if ($min < 10) {$min = "0".$min};
    if ($hour< 10) {$hour= "0".$hour};
    if ($mday< 10) {$mday= "0".$mday};
    if ($month<10) {$month="0".$month};
    my $ddmmyyhhmmss = $mday."/".$month."/".$year." ".$hour.":".$min.":".$sec;

    return $ddmmyyhhmmss;
}

sub DDMMYYHHMMSSToEpoch {
    my ($sec, $min, $hour, $mday, $month, $year) = (localtime)[0..5];
    my $time = timelocal($sec, $min, $hour, $mday, $month, $year);

    return $time;
}

sub colorLegend {
#    print "<td align=\"right\"><font size=4 color=\"#3399ff\"><b>Colors Legend</b></font></td>\n";
    print "<table width=100 cellpadding=2 cellspacing=2 border=2 align=right>\n";;
    print "<td width=40 height=20 align=middle valign=middle bgcolor=\"green\">
            <font size=3 color=\"ffffff\"><b>Normal</b></font></td>\n";
    print "<td width=40 height=20 align=middle valign=middle bgcolor=\"orange\">
            <font size=3 color=\"cc00cc\"><b>Warning</b></font></td>\n";
    print "<td width=40 height=20 align=middle valign=middle bgcolor=\"red\">
            <font size=3 color=\"990033\"><b>Error</b></font></td>\n";
    print "<td width=40 height=20 align=middle valign=middle bgcolor=\"#8cc099\">
            <font size=3 color=\"990099\"><b>NoUpdate</b></font></td>\n";
    print "<td width=40 height=20 align=middle valign=middle bgcolor=\"#990099\">
            <font size=3 color=\"ffff66\"><b>Obsolete</b></font></td>\n";
    print "</tr>\n";
    print "</table><p></p><br></br>\n";
}

sub ht_Menus {
 print "<dt><img src=\"$maroonbullet\">&#160;&#160;<a href=\"#cites\"><b><font color=green> MC02 Productiction Cites</font></a>\n";
 print "<dt><img src=\"$bluebullet\">&#160;&#160;<a href=\"#mails\"><b><font color=green>Authorized Users</b></font></a>\n";
 print "<dt><img src=\"$purplebullet\">&#160;&#160;<a href=\"#servers\"><b><font color=green>Servers</b></font></a>\n";
 print "<dt><img src=\"$silverbullet\">&#160;&#160;
        <a href=\"#jobs\"><b><font color=green>Jobs</b></font></a>\n";
 print "<dt><img src=\"$bluebullet\">&#160;&#160;
        <a href=\"#runs\"><b><font color=green>Runs</b></font></a>\n";
 print "<dt><img src=\"$purplebullet\">&#160;&#160;
        <a href=\"#ntuples\"><b><font color=green>Ntuples</b></font></a>\n";
 print "<dt><img src=\"$purplebullet\">&#160;&#160;
        <a href=\"#disks\"><b><font color=green>Disks and Filesystems \@CERN</b></font></a>\n";
 print "<dt><img src=\"$bluebullet\">&#160;&#160;
        <a href=$rchtml>
        <b><font color=green>Submit MC Job</b></font></a>\n";
 print "<dt><img src=\"$maroonbullet\">&#160;&#160;<a href=$rchtml>
        <b><font color=green>User and/or Cite Registration Form</b></font></a>\n";
 print "<dt><img src=\"$silverbullet\">&#160;&#160;
        <a href=$downloadcgi>
        <b><font color=green>Download MC data and exec files</b></font></a>\n";
}

sub lastDBUpdate {
     my $self = shift;
     my $lastupd =0;
     my $sql;
     my $ret;
      $sql="SELECT MAX(Cites.timestamp) FROM Cites"; 
      $ret=$self->{sqlserver}->Query($sql);
     if(defined $ret->[0][0]){
        if($ret->[0][0]>$lastupd){
          $lastupd=$ret->[0][0];
      }
    }
      $sql="SELECT MAX(Mails.timestamp) FROM Mails"; 
      $ret=$self->{sqlserver}->Query($sql);
     if(defined $ret->[0][0]){
        if($ret->[0][0]>$lastupd){
          $lastupd=$ret->[0][0];
      }
    }

      $sql="SELECT MAX(Servers.lastupdate) FROM Servers"; 
      $ret=$self->{sqlserver}->Query($sql);
     if(defined $ret->[0][0]){
        if($ret->[0][0]>$lastupd){
          $lastupd=$ret->[0][0];
      }
    }

      $sql="SELECT MAX(Jobs.time) FROM Jobs"; 
      $ret=$self->{sqlserver}->Query($sql);
     if(defined $ret->[0][0]){
        if($ret->[0][0]>$lastupd){
          $lastupd=$ret->[0][0];
      }
    }

      $sql="SELECT MAX(Runs.submit) FROM Runs"; 
      $ret=$self->{sqlserver}->Query($sql);
     if(defined $ret->[0][0]){
        if($ret->[0][0]>$lastupd){
          $lastupd=$ret->[0][0];
      }
    }

      $sql="SELECT MAX(Ntuples.timestamp) FROM Ntuples"; 
      $ret=$self->{sqlserver}->Query($sql);
     if(defined $ret->[0][0]){
        if($ret->[0][0]>$lastupd){
          $lastupd=$ret->[0][0];
      }
    }

  return $lastupd;
 }
sub statusColor {
    my $status = shift;
    my $color  = "red";

    if ($status eq "Finished" or $status eq "OK" or $status eq "Validated" or $status eq "Completed") {
               $color  = "green";
    }
    elsif ($status eq "TimeOut" or $status eq "Pending") {
         $color="magenta";
    }
    elsif ($status eq "Success") {
        $color  = "green";
    }
    elsif ($status eq "Foreign") {
               $color = "black";
    }
    elsif ($status eq "Processing") {
               $color = "blue";
    } 
    elsif ($status eq "Active") {
               $color = "green";
    } 
    elsif ($status eq "Dead" or $status eq "Unknown" or $status eq "ToBeRerun") {
               $color = "magenta";
    } 
    return $color;
}

sub Color {
    my $color  = shift;
    while ($color > $#colors) {
        $color = $color/$#colors+1;
    }
    return $colors[$color];
}


sub parseJournalFiles {

 my $self = shift;


 my $firstjobtime = 0;      # first job order time
 my $lastjobtime  = 0;      # last
 my $cid          = undef;
 my $sql          = undef;
 my $ret          = undef;

 my $HelpTxt = "
     parseJournalFiles check journal file directory for all cites 
     -c    - output will be produced as ASCII page (default)
     -h    - print help
     -i    - prompt before files removal 
     -v    - verbose mode
     -w    - output will be produced as HTML page
     ./parseJournalFiles.o.cgi -c
";

  $rmprompt = 0;
  foreach my $chop  (@ARGV){
    if ($chop =~/^-c/) {
        $webmode = 0;
    }
    if ($chop =~/^-v/) {
        $verbose = 1;
    }
    if ($chop =~/^-i/) {
        $rmprompt = 1;
    }
    if ($chop =~/^-w/) {
     $webmode = 1;
    }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
   }
 
    if( not $self->Init()){
      die "parseJournalFiles -F- Unable To Init";
    } else {
        if ($verbose == 1) {$self->amsprint("parseJournalFiles -I- Init done ",0);}
    }
    if ($verbose == 1) {$self ->amsprint ("parseJournalFiles -I- connect to server",0);}
    if (not $self->ServerConnect()){
        die "parseJournalFiles -F- Unable To Connect To Server";
    } else {
        if ($verbose == 1) {$self->amsprint("parseJournalFiles -I- connected to server. done",0);}
    }

    my $whoami = getlogin();
    if ($whoami =~ 'ams') {
    } else {
      $self->amsprint("parseJournalFiles -ERROR- script cannot be run from account : $whoami",0);
      die "bye";
    }
#
   $self->setActiveProductionSet();
   if (not defined $ActiveProductionSet || $ActiveProductionSet eq $UNKNOWN) {
      $self->amsprint("parseJournalFiles -ERROR- cannot get active production set",0);
      die "bye";
  }
# set flag
   my $timenow = time();
   $self->initFilesProcessingFlag();
#
    $self->set_root_env();

    $self->getProductionVersion();

 $sql = "SELECT begin FROM productionset WHERE STATUS='Active'";
 $ret = $self->{sqlserver}->Query($sql);
 if(not defined $ret->[0][0]){
     $self->amsprint("parseJournalFiles -ERROR- cannot find 'Active' production set",0);
     die "exit\n";
 } 

 $firstjobtime = $ret->[0][0] - 24*60*60;
 $lastjobtime  = time() + 24*60*60;

 if ($webmode == 1) {
  htmlTop();
  htmlTable("Parse Journal Files");
 }

 $sql = "SELECT MAX(CID) FROM Cites";
 $ret = $self->{sqlserver}->Query($sql);
 if (not defined $ret->[0][0] || $ret->[0][0]<1) {
     $self->amsprint("parseJournalFiles - ERROR - wrong MAX(CID)",0);
     die "exit\n";
 } 
 $nCheckedCite  = -1;
 $nActiveCites  = 0;  # cites with new dsts 
 for (my $i=0; $i<$ret->[0][0]; $i++) {
#+ parser statistics
 $JournalFiles[$i] = 0;  # number of journal files
 $JouLastCheck[$i] = 0;
 $JouDirPath[$i]   = 'xyz';
 $CheckedRuns[$i]  = 0;  # runs processed
 $FailedRuns[$i]   = 0;  # failed runs
 $GoodRuns[$i]     = 0;  # marked as 'Completed'
 $BadRuns[$i]      = 0;  # marked as 'Unckecked' or 'Failed'
 $CheckedDSTs[$i]  = 0;  # dsts checked
 $GoodDSTs[$i]     = 0;  # copied to final destination
 $BadDSTs[$i]      = 0;
 $BadDSTCopy[$i]   = 0;  # doCopy failed to copy DST
 $BadCRCi[$i]      = 0;  # dsts with crc error (before copying)
 $BadCRCo[$i]      = 0;  # dsts with crc error (after copying)
 $gbDST[$i]        = 0;  # GB of DSTs
#-
}

 $parserStartTime=time();# start journal files check
 
 

 $sql = "SELECT dirpath,journals.timelast,name,journals.cid  
              FROM journals,cites WHERE journals.cid=cites.cid";

 $ret = $self->{sqlserver}->Query($sql);

 if(defined $ret->[0][0]){
  foreach my $jou (@{$ret}){
   $nCheckedCite++;
   my $timenow    = time();
   my $dir        = trimblanks($jou->[0]);  # journal file's path
   my $timestamp  = trimblanks($jou->[1]);  # time of latest processed file
   my $cite       = trimblanks($jou->[2]);  # cite
      $cid        = $jou->[3];
   my $lastcheck  = EpochToDDMMYYHHMMSS($timestamp);
   $JouDirPath[$nCheckedCite]=$cite;
   $JouLastCheck[$nCheckedCite] = $lastcheck;
   my $title  = "Cite : ".$cite.", Directory : ".$dir." Last Check ".$lastcheck;
   if ($webmode == 1) {
    htmlTable($title);
   } else {
       if ($verbose == 1) {$self->amsprint($title,0);}
   }  
   my $newfile   = "./";
   my $lastfile  = "./";
   my $writelast = 0;
      my $logdir = $dir."/log";
      my $joudir = $dir."/jou";
      my $ntdir  = $dir."/nt";
      opendir THISDIR ,$joudir or die "unable to open $joudir";
      my @allfiles= readdir THISDIR;
      closedir THISDIR;
      if ($webmode == 1) {
       print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
       print "<td><b><font color=\"blue\">File </font></b></td>";
       print "<td><b><font color=\"blue\" >Write Time</font></b></td>";
       print "<td><b><font color=\"blue\" >Status</font></b></td>";
       print "</tr>\n";
     } 
      foreach my $file (@allfiles) {
       if ($file =~/^\./){
         next;
       }
       if ($file =~ m/journal/) {
       } else {
           next;
       }
#
# files *.journal.1 and *.journal.0 are already validated
#
       my @junk = split "journal.",$file;
       if (defined $junk[1]) {
        if ($junk[1] == 1 || $junk[1] == 0) {
           next;
        }
       }
       $newfile=$joudir."/".$file;
       my $writetime = 0;
       $writetime = (stat($newfile)) [9];
       if ($writetime > $writelast) {
           $writelast = $writetime;
           $lastfile = $newfile;
       }

# check flag
     $timenow = time();
     my $rflag = $self->getFilesProcessingFlag();
     if ($rflag == 0) {
         if ($webmode == 0) {$self -> printParserStat();}
         $self->updateFilesProcessing();
         $self->amsprint("Processing flag = 0. Stop parseJournalFiles.",0);
         return 1;
     } elsif ($rflag == -1) {
         $self->amsprint("Processing flag = -1. Stop parseJournalFiles.",0);
         return 1;
     }
#
       my $color   ="black";
       my $fstatus ="CheckInProgress";
       if ($writetime < $timestamp) { $color   = "magenta";
                                      $fstatus = "AlreadyChecked"}
       my $wtime = EpochToDDMMYYHHMMSS($writetime);
       if ($webmode == 0) {$color = 666;} # no CR
       $self->amsprint($file, $color);
       $self->amsprint($wtime, $color);
       $self->amsprint($fstatus,$color);
       $self->amsprint(" ",0);
       if ($webmode == 1) {print "</tr>\n";}
#
# $timestamp - latest time of journal file during previous validation
# all files produced $timestamp- 24h -> pass automatic validation
# it is possible that some files been transmitted with delay > 24h, so they 
# pass manual validation
#
       if ($writetime > $timestamp - 24*60*60) {
         $self->parseJournalFile($firstjobtime,
                                 $lastjobtime,
                                 $logdir,
                                 $newfile,
                                 $ntdir);
         $JournalFiles[$nCheckedCite]++;
       }
   }
   if ($webmode == 1) {
    htmlTableEnd();
    htmlTableEnd();
   }
   if (defined $cid) {
    if ($writelast > 0) {
     $sql = "UPDATE journals 
             SET timestamp=$timenow, 
                 timelast=$writelast, 
                 lastfile = '$lastfile' 
              WHERE cid=$cid";
      $nActiveCites++;
     } else {
      $sql = "UPDATE journals SET timestamp=$timenow WHERE cid=$cid";
     }
    $self->{sqlserver}->Update($sql); 
   }
  }
 } else {
     $self->amsprint("parseJournalFile - Warning - table Journals is empty",0);
 }
 if ($webmode == 1) {
   htmlTableEnd();
  htmlBottom();
 } else {
     $self -> printParserStat();
 }
 $self->updateFilesProcessing();
 return 1;
}


sub parseJournalFile {
#
# to do : check run with DBserver
#         copy to final destination
#

 my $self         = shift;
 my $firstjobtime = shift;
 my $lastjobtime  = shift;
 my $logdir       = shift;
 my $inputfile    = shift;
 my $dirpath      = shift;

 my $host         = "unknown";
 my $tevents      = 0;
 my $terrors      = 0;

 my $jobid        = -1;

 my $sql       = undef;

 my $fevent =  1;
 my $levent =  0;

 my $patternsmatched  = 0; 

my @startingjob   =();
my @startingrun   =();
my @opendst       =();
my @closedst      =();;
my @runfinished   =();
my @runincomplete =();

my $timestamp = time();    # unix time 
my $lastjobid = 0;

my $startingjobR = 0; # StartingJob record found and parsed
my $startingrunR = 0; # StartingRun "     "       "       "
my $opendstR     = 0; # OpenDST     "     "       "       "
my $closedstR    = 0; # CloseDST    "     "       "       "
my $runfinishedR = 0; # RunFinished "     "       "       "
my $status = undef;   # Run status
my $jobmips = -1;


   my $validated=0;
   my $thrusted =0;
   my $copied   =0;
   my $failedcp =0;
   my $bad      =0; 
   my $unchecked=0; 


    my $run  = 0;

    open(FILE,"<",$inputfile) or die "Unable to open $inputfile";
    my $buf;
    read(FILE,$buf,16384);
    close FILE;

    my @blocks =  split "-I-TimeStamp",$buf;


     my @cpntuples   =();
     my @mvntuples   =();
     my $runupdate   = "UPDATE runs SET ";
     my $copyfailed  = 0;

 my $timenow = time();
 
 my @jj      = split '/', $inputfile;

 my $joufile  = $jj[$#jj];

 my $copylog = $logdir."/copyValidateCRC.".$joufile.".log";

 open(FILE,">".$copylog) or die "Unable to open file $copylog\n";

foreach my $block (@blocks) {
    my @junk;
    @junk = split ",",$block;
    for (my $i=0; $i<$#junk+1; $i++) {
      $junk[$i] = trimblanks($junk[$i]);
    }
#    
    my ($utime,@jj) = split " ",$block;
    if (defined $utime) {
     if ($utime > $firstjobtime && $utime < $lastjobtime) {
#
# find one of the following : RunIncomplete StartingJob, StartingRun, 
#                             OpenDST, CloseDST, RunFinished
#
#-I-TimeStamp 1051668098 Wed Apr 30 04:01:38 2003
#,  RunIncomplete CInfo  , Host ccwall43 , EventsProcessed 162892 , LastEvent 319737 
#, Errors 4364 , CPU 100271 , Elapsed 126696 , CPU/Event 0.615564 , Status Failed

      if ($block =~/RunIncomplete/) {
          if ($startingrunR == 1) {
              print FILE "RunIncomplete : Ntuples Validated : $validated. Continue\n";
              print FILE "$block \n";

          $patternsmatched  = 0;
          my @RunIncompletePatterns = 
                ("RunIncomplete","Host","Run","EventsProcessed","LastEvent","Errors",
                              "CPU","Elapsed","CPU/Event","Status");
          for (my $i=0; $i<$#junk+1; $i++) {
           my @jj = split " ",$junk[$i];
           if ($#jj > 0) {
            my $found = 0;
            my $j     = 0;
            while ($j<$#RunIncompletePatterns+1 && $found == 0) { 
            if ($jj[0] eq $RunIncompletePatterns[$j]) {
             $runincomplete[$j] = trimblanks($jj[1]);
             $patternsmatched++;
             $found = 1;
            }
            $j++;
         }
        }
       }
   if ($patternsmatched == $#RunIncompletePatterns+1) { #RunIncomplete has a pair CInfo
    $runincomplete[0] = "RunIncomplete";
    $run              = $runincomplete[2];
    
    $runfinishedR   = 1;
    my $sql = "SELECT run FROM runs WHERE run = $run AND levent=$runincomplete[4]";
    my $ret = $self->{sqlserver}->Query($sql);
    if (not defined $ret->[0][0]) {
     my $cputime = sprintf("%.0f",$runincomplete[6]);
     my $elapsed = sprintf("%.0f",$runincomplete[7]);
     $host = $runincomplete[1];
     $sql = "UPDATE Jobs SET EVENTS=$runincomplete[3], ERRORS=$runincomplete[5], 
                                   CPUTIME=$cputime, ELAPSED=$elapsed,
                                   HOST='$host', TIMESTAMP = $timestamp 
                   WHERE JID = (SELECT Runs.jid FROM Runs WHERE Runs.jid = $run)";
     print FILE "$sql \n";
     $self->{sqlserver}->Update($sql);
     $sql = "UPDATE Runs SET LEVENT=$runincomplete[4] WHERE run=$run";
     $self->{sqlserver}->Update($sql);
     print FILE "$sql \n";
    }
   } else {
    print FILE "parseJournalFile -W- RunIncomplete - cannot find all patterns \n";
   }
   #
   # end RunIncomplete - normal
   # if no NTuples produced
   #
   } else {
# assume journal file : jobnumber.journal
       if ($webmode == 0 && $verbose == 1) {
        print "parseJournalFile -W- RunIncomplete CHECK File $joufile \n";
       }
       print FILE "parseJournalFile -W- RunIncomplete CHECK File $joufile \n";
       $runfinishedR   = 1;
          my @jj = split ".journal",$joufile;
          $jobid = $jj[0];
          my $mailto = "vitali.choutko\@cern.ch,alexei.klimentov\@cern.ch";
          my $subject = "RunIncomplet : ".$inputfile;
          my $text    = " Program found RunIncomplete status in ".
                         $inputfile." file. ".
                         "\n Please provide job log file to V.Choutko".
                         "\n MC production team";
          $sql = "SELECT Mails.address FROM Jobs, Mails 
                  WHERE Jobs.jid=$jobid AND Mails.mid = Jobs.mid";
          my $ret = $self->{sqlserver}->Query($sql);
          if(defined $ret->[0][0]){
              $mailto = $mailto.", $ret->[0][0]";
          }
#          $self->sendmailmessage($mailto,$subject,$text);
       last;
      }
     }
      elsif ($block =~/StartingJob/) {

#
# , JobStarted HostName gcie01 default , UID 805306385 , PID 3578 3573 , Type Producer , 
#   ExitStatus NOP , StatusType OneRunOnly
#              HostName pcamsvc  , UID 134217740 , PID 11894 11891 , Type Standalone , 
# ExitStatus NOP ,StatusType OneRunOnly   
#
      $patternsmatched = 0;
      my @StartingJobPatterns = ("StartingJob", "HostName","UID","PID","Type",
                                  "ExitStatus","StatusType");
      for (my $i=0; $i<$#junk+1; $i++) { 
        my @jj = split " ",$junk[$i];
        if ($#jj > 0) {
         my $found = 0;
         my $j     = 0;
         while ($j<$#StartingJobPatterns+1 && $found == 0) { 
          if ($jj[0]  eq $StartingJobPatterns[$j]) {
              $startingjob[$j] = trimblanks($jj[1]);
              $patternsmatched++;
              $found = 1;
          }
          $j++;
        }
       }
    }

   $jobid = $startingjob[2];
   if ($self->findJob($jobid) != $jobid) {
       print FILE "Fatal - cannot find JobInfo for $jobid \n";
       system("mv $inputfile $inputfile.0"); 
       return;
   }

   if ($self->getRunStatus($jobid,0) eq 'Completed') {
       print FILE "Job : $jobid Status : Completed. Return \n";
       return;
   }  

   if ($patternsmatched == $#StartingJobPatterns) { 
    $startingjob[0] = "StartingJob";
    $lastjobid = $startingjob[2];
    $startingjobR   = 1;
   } else  {
      print FILE "parseJournalFile -W- StartingJob - cannot find all patterns \n";
   }
    # end StartingJob cmd
    #
   } elsif (($block =~/JobStarted/)) {
# 19.09.03
#
#, JobStarted HostName pcamsf6 hrdl , UID 116 , PID 5751 5746 , Type 
# Producer , ExitStatus NOP , StatusType OneRunOnly , Mips 2445
#                                                     ^^^^
#
      $patternsmatched = 0;
      my @StartingJobPatterns = ("JobStarted", "HostName","UID","PID","Type",
                                  "ExitStatus","StatusType","Mips");
      for (my $i=0; $i<$#junk+1; $i++) { 
        my @jj = split " ",$junk[$i];
        if ($#jj > 0) {
         my $found = 0;
         my $j     = 0;
         while ($j<$#StartingJobPatterns+1 && $found == 0) { 
          if ($jj[0]  eq $StartingJobPatterns[$j]) {
              $startingjob[$j] = trimblanks($jj[1]);
              $patternsmatched++;
              if ($jj[0] eq "Mips") {
                $jobmips = trimblanks($jj[1]);
              }
              $found = 1;
          }
          $j++;
        }
       }
    }

   $jobid = $startingjob[2];
   if ($self->findJob($jobid) != $jobid) {
       print FILE "Fatal - cannot find JobInfo for $jobid \n";
       system("mv $inputfile $inputfile.0"); 
       $BadRuns[$nCheckedCite]++;
       return;
   }
   if ($patternsmatched == $#StartingJobPatterns || $patternsmatched == $#StartingJobPatterns-1) { 
    $startingjob[0] = "StartingJob";
    $startingjobR   = 1;
    $lastjobid = $startingjob[2];
   } else  {
    print FILE "parseJournalFiles -W- StartingJob - cannot find all patterns \n";
   }
    # end JobStarted cmd
    #
   } elsif (($block =~/StartingRun/)) {
#
# StartingRun REI, ID 0 , Run 134217740 , FirstEvent 1 , LastEvent 21000 , Prio 0 , Path  , 
# Status Allocated , History Foreign , ClientID 134217740 , SubmitTimeU 1049456649, 
# SubmitTime Fri Apr  4 13:44:09 2003, Host pcamsvc , Run 0, EventsProcessed 0 , 
# LastEvent 0 , Errors 0 , CPU 0 , Elapsed 0 , CPU/Event 0 , Status Processing
#
   

      $patternsmatched = 0;
      my @StartingRunPatterns = ("StartingRun","ID","Run","FirstEvent","LastEvent",
                                 "Prio","Path","Status","History","CounterFail", "ClientID",
                                 "SubmitTime","SubmitTimeU","Host","EventsProcessed","LastEvent",
                                 "Errors","CPU","Elapsed","CPU/Event","Status");
  
      my $j = 0;
      foreach my $pat (@StartingRunPatterns) {
        foreach my $xyz (@junk) {
         my @jj = split " ",$xyz;
         if ($#jj > 0) {
             if ($pat eq $jj[0]) {
              $patternsmatched++;
              $startingrun[$j] = trimblanks($jj[1]);
              if ($jj[0] eq 'Run') { last;}
             }
          }
       }
       $j++;
     }


   
   if ($patternsmatched == $#StartingRunPatterns+3 || $patternsmatched == $#StartingRunPatterns+4) { 
    $run = $startingrun[2];
    $startingrun[0] = "StartingRun";
    $startingrunR   = 1;
# insert run : run #, $jid, $fevent, $levent, $fetime, $letime, $submit, $status;
    $CheckedRuns[$nCheckedCite]++;
    $self->insertRun(
             $startingrun[2],
             $lastjobid,
             $startingrun[3],
             $startingrun[4],
             0,
             0,
             $startingrun[12],
             $startingrun[7]);

     if (defined $startingrun[13]) {
      $host=$startingrun[13];
     }
     $sql = "UPDATE Jobs SET 
                 host='$host',
                 events=$startingrun[14], errors=$startingrun[16],
                 cputime=$startingrun[17], elapsed=$startingrun[18],
                 timestamp=$timestamp, mips = $jobmips  
                where jid=$lastjobid";
     print FILE "$sql \n";
     $self->{sqlserver}->Update($sql);
   } else  {
       print FILE "StartingRun - cannot find all patterns $patternsmatched/$#StartingRunPatterns \n";
       print "StartingRun -W- cannot find all patterns $patternsmatched/$#StartingRunPatterns \n";
   }
   # end StartingRun 
   #
   } elsif ($block =~/OpenDST/) { 
#
# OpenDST  , Status InProgress , Type Ntuple , Name pcamsvc:/tmp/14/pl1/14/134217740.1.hbk , 
# Version v4.00/build56/os2 , Size 0 , CRC 0 , Insert 1049456665 , Begin 1177173455 , End 0 , 
# Run 134217740 , FirstEvent 1 , LastEvent 0 , EventNumber 0 , ErrorNumber 0
#
    $patternsmatched = 0;
    my @OpenDSTPatterns = ("OpenDST","Status","Type","Name","Version",
                           "Size","CRC","Insert","Begin","End",
                          "Run","FirstEvent","LastEvent","EventNumber","ErrorNumber");
     for (my $i=0; $i<$#junk+1; $i++) { 
      my @jj = split " ",$junk[$i];
      if ($#jj > 0) {
       my $found = 0;
       my $j     = 0;
        while ($j<$#OpenDSTPatterns+1 && $found == 0) { 
         my $pattern = lc($OpenDSTPatterns[$j]); 
         if ($jj[0] eq $OpenDSTPatterns[$j] || $jj[0] eq $pattern) {
            $opendst[$j] = trimblanks($jj[1]);
            $patternsmatched++;
            $found = 1;
         }
         $j++;
       }
     }
    }
   if ($patternsmatched == $#OpenDSTPatterns) { #OpenDST has no pair
    $opendst[0] = "OpenDST";
    $CheckedDSTs[$nCheckedCite]++;
   } else  {
     print FILE "OpenDST - cannot find all patterns \n";
   }
   # end OpenDST 
   # 
   } elsif ($block =~/CloseDST/) { 
#
# CloseDST  , Status Validated , Type Ntuple , Name pcamsvc:/tmp/14/pl1/14/134217740.1.hbk , 
# Version v4.00/build56/os2 , Size 3 , CRC 2453001786 , 
# Insert 1049457375 , Begin 1177173455 , End 1224534144 , 
# Run 134217740 , FirstEvent 1 , LastEvent 21000 , EventNumber 312 , ErrorNumber 0
#
    $patternsmatched  = 0;

    my $statusIndx       = 1; # STATUS in number 1 in the row, counting from '0'
    my $fileIndx         = 3; # NAME in number 1 in the row, counting from '0'
    my $crcIndx          = 6; # CRC in number 6 in the row, counting from '0'

    my @CloseDSTPatterns = ("CloseDST","Status","Type","Name","Version",
                            "Size","CRC","Insert","Begin","End",
                            "Run","FirstEvent","LastEvent","EventNumber","ErrorNumber");
     for (my $i=0; $i<$#junk+1; $i++) { 
      my @jj = split " ",$junk[$i];
      if ($#jj > 0) {
       my $found = 0;
       my $j     = 0;
       while ($j<$#CloseDSTPatterns+1 && $found == 0) { 
        my $pattern = lc($CloseDSTPatterns[$j]); 
        if ($jj[0] eq $CloseDSTPatterns[$j] || $jj[0] eq $pattern) {
          $closedst[$j] = trimblanks($jj[1]);
          $patternsmatched++;
          $found = 1;
         }
        $j++;
       }
     }
   }
   if ($patternsmatched == $#CloseDSTPatterns) { #CloseDST has no pair
#
# Check CRC
# 
   if ($closedst[$crcIndx] == 0) {
    print FILE "Status : $closedst[$statusIndx], CRC $closedst[$crcIndx]. Skip file : : $closedst[$fileIndx] \n";
    if ($verbose == 1 && $webmode == 0) {
     print "Status : $closedst[$statusIndx], CRC $closedst[$crcIndx]. Skip file : $closedst[$fileIndx] \n";
    }   
   }  else {
#
# find ntuple
#
    my @junk = split "/",$closedst[$fileIndx];
    my $dstfile = trimblanks($junk[$#junk]);
    my $filename= $dstfile;
    $dstfile=$dirpath."/".$dstfile;
    my $dstsize = -1;
    $dstsize = (stat($dstfile)) [7] or $dstsize = -1;
    if ($dstsize == -1) {
     print FILE "parseJournalFile -W- CloseDST block : cannot stat $dstfile \n";
     $runfinishedR=1;
     $dstsize = -1;
     $copyfailed = 1;
     last;
    } else {
     if ($closedst[1] ne "Validated" and $closedst[1] ne "Success" and $closedst[1] ne "OK") {
      print FILE "parseJournalFile -W- CloseDST block : $dstfile,  DST status  $closedst[1]. Check anyway.\n";
     } 
      $dstsize = sprintf("%.1f",$dstsize/1000/1000);
      $closedst[0] = "CloseDST";
       print FILE "$dstfile.... \n";
       my $ntstatus =$closedst[1];
       my $nttype   =$closedst[2];
       my $version  =$closedst[4];
       my $ntcrc    =$closedst[6];
       my $run      =$closedst[10];
       my $jobid    =$closedst[10];
       my $dstfevent=$closedst[11];
       my $dstlevent=$closedst[12];
       my $ntevents =$closedst[13];
       my $badevents=$closedst[14];
#
# print warning if DST's version < Declared version for the current production set
# Jan 19, 2004, set error flag, from now skip DST if version doesn't match
#
     if ($self->checkDSTVersion($version) != 1) {
      $self->amsprint ("------------ Check DST; Version : $version / Min production version :",666);
      $self->amsprint($self->{Version},0);
      $unchecked++;
      $copyfailed = 1;
      $BadDSTs[$nCheckedCite]++;
      last;
     }
      $levent += $dstlevent-$dstfevent+1;
      my $i = $self->calculateCRC($dstfile, $ntcrc);
      print FILE "calculateCRC($dstfile, $ntcrc) : Status : $i \n";
      if ($i != 1) {
          $unchecked++;
          $copyfailed = 1;
          $BadCRCi[$nCheckedCite]++;
          last;
      }
      my $ret = 0;
      ($ret,$i) = $self->validateDST($dstfile ,$ntevents, $nttype ,$dstlevent);
      print FILE "validateDST($dstfile ,$ntevents, $nttype ,$dstlevent) : Status : $i : Ret : $ret\n";
      if ($ret !=1) {
       $unchecked++;
       $copyfailed = 1;
        print FILE " validateDST return code != 1. Quit. \n";
       last;
      }

      if( ($i == 0xff00) or ($i & 0xff)){
       if($ntstatus ne "Validated"){
         $ntstatus="Unchecked";                     
         $badevents="NULL";
         $unchecked++;
         $copyfailed = 1;
         last;
        }
         else{
           $thrusted++;
                      }
      }
        else{
          $i=($i>>8);
          if(int($i/128)){
           $ntevents=0;
           $badevents="NULL";
           $ntstatus="Bad".($i-128);  
           $bad++;                   
           $levent -= $dstlevent-$dstfevent+1;
          }
           else{
            $badevents=int($i*$ntevents/100);
            $tevents += $ntevents;
            $terrors += $badevents;
            $validated++;
            $ntstatus = "Validated";
            my ($outputpath,$rstatus) = $self->doCopy($jobid,$dstfile,$ntcrc);
            if(defined $outputpath){
              push @mvntuples, $outputpath; 
            }
            print FILE "doCopy return status : $rstatus \n";
            if ($rstatus == 1) {
             $self->insertNtuple(
                               $run,
                               $version,
                               $nttype,
                               $jobid,
                               $dstfevent,
                               $dstlevent,
                               $ntevents,
                               $badevents,
                               $timestamp,
                               $dstsize,
                               $ntstatus,
                               $outputpath,
                               $ntcrc,
                               $timestamp, 1, 0); 

             print FILE "insert ntuple : $run, $outputpath, $closedst[1]\n";
             $gbDST[$nCheckedCite] = $gbDST[$nCheckedCite] + $dstsize;
             push @cpntuples, $dstfile;
           } else {
            print FILE "***** Error in doCopy for : $outputpath\n";
           }
         }
        }
     }
    } # CRC != 0
   } else  {
     print FILE "parseJournalFiles -W- CloseDST - cannot find all patterns \n";
   }
  
   # end CloseDST 
   # 

  } elsif ($block =~/RunFinished/) { 
#
# RunFinished CInfo  , Host pcamsvc , EventsProcessed 10796 , LastEvent 21000 , 
# Errors 3 , CPU 712.62 , Elapsed 725.923 , CPU/Event 0.0660017 , Status Finished
#
    $patternsmatched  = 0;
    my @RunFinishedPatterns = ("RunFinished","Host","Run","EventsProcessed","LastEvent","Errors",
                              "CPU","Elapsed","CPU/Event","Status");
     for (my $i=0; $i<$#junk+1; $i++) {
      my @jj = split " ",$junk[$i];
      if ($#jj > 0) {
        my $found = 0;
        my $j     = 0;
        while ($j<$#RunFinishedPatterns+1 && $found == 0) { 
         if ($jj[0] eq $RunFinishedPatterns[$j]) {
          $runfinished[$j] = trimblanks($jj[1]);
          $patternsmatched++;
          $found = 1;
         }
         $j++;
       }
     }
   }
   if ($patternsmatched == $#RunFinishedPatterns+1 ||
       $patternsmatched == $#RunFinishedPatterns) { #RunFinsihed has a pair CInfo
    $runfinished[0] = "RunFinished";
    $runfinishedR   = 1;
    $sql = "UPDATE Runs SET LEVENT=$runfinished[4] WHERE run=$run";
    $self->{sqlserver}->Update($sql);
    print FILE "$sql \n";

    my $cputime = sprintf("%.0f",$runfinished[6]);
    my $elapsed = sprintf("%.0f",$runfinished[7]);
    $host= $runfinished[1];
    $sql = "update jobs set events=$runfinished[3], errors=$runfinished[5], 
                                   cputime=$cputime, elapsed=$elapsed,
                                   host='$host',timestamp = $timestamp 
                               where jid = (select runs.jid from runs where runs.jid = $run)";
     print FILE "$sql \n";
     $self->{sqlserver}->Update($sql);
   } else {
     print FILE "parseJournalFile -W- RunFinished - cannot find all patterns $patternsmatched/$#RunFinishedPatterns\n";
   }
   #
   # end RunFinished
   # 
   }
  } else {
    $BadRuns[$nCheckedCite]++;
    print FILE "*********** wrong timestamp : $utime ($firstjobtime,$lastjobtime)\n";
    system("mv $inputfile $inputfile.0"); 
    return; 
   }
  } #if defined $utime 
}

 if ($startingrunR == 1 || $runfinishedR == 1) {
  $status="Failed";
  my $cmd = undef;
  my $inputfileLink = $inputfile.".0";
  if ($copyfailed == 0) {
    foreach my $ntuple (@cpntuples) {
      $cmd="rm  $ntuple";
      if ($rmprompt == 1) {$cmd = "rm -i $ntuple";}
      print FILE "$cmd\n";
      system($cmd);
      print FILE "Validation done : system command $cmd \n";
      $GoodDSTs[$nCheckedCite]++;
    }
   if ($#cpntuples >= 0) { 
    $status = 'Completed';
    $inputfileLink = $inputfile.".1";
    $GoodRuns[$nCheckedCite]++;
    if ($runfinishedR != 1) {
      print FILE "End of Run not found update Jobs \n";
      $sql = "UPDATE Jobs SET 
                 host='$host',
                 events=$tevents, errors=$terrors,
                 cputime=-1, elapsed=-1,
                 timestamp=$timestamp 
                where jid=$lastjobid";
      print FILE "$sql \n";
      $self->{sqlserver}->Update($sql);
     }
 } else {
     $BadRuns[$nCheckedCite]++;
 }
}
  else{
   print FILE "Validation/copy failed = $copyfailed for  Run =$run \n";
   $status='Unchecked';
   $BadRuns[$nCheckedCite]++;
   foreach my $ntuple (@mvntuples) {
     $cmd = "rm  $ntuple";
     if ($rmprompt == 1) {$cmd = "rm -i $ntuple";}
     print FILE "Validation failed : system command $cmd \n";
     print FILE "$cmd\n";
     system($cmd);
    }

    $sql = "DELETE ntuples WHERE run=$run";
    $self->{sqlserver}->Update($sql);
    print FILE "$sql \n";
    $runupdate = "UPDATE runs SET ";
}
 if ($run != 0) {
   $sql = $runupdate." STATUS='$status' WHERE run=$run";
   $self->{sqlserver}->Update($sql);
   print FILE "Update Runs : $sql \n";
   if ($status eq "Failed" || $status eq "Unchecked") {
    $sql = "SELECT dirpath FROM journals WHERE cid=-1";
    my $ret=$self->{sqlserver}->Query($sql);
     if( defined $ret->[0][0]){
      my $junkdir = $ret->[0][0];   
      $cmd = "mv $dirpath/*$run* $junkdir/";
      print FILE "$cmd\n";
      system($cmd);
      print FILE "Validation/copy failed : mv ntuples to $junkdir \n";
     }
   }
} 
  system("mv $inputfile $inputfileLink"); 
  print FILE "mv $inputfile $inputfileLink\n";
  if ($webmode == 0 && $verbose == 1) {print "mv $inputfile $inputfileLink \n";}
  if ($status eq "Completed") {
    $self->updateRunCatalog($run);
    if ($verbose == 1) {print "Update RunCatalog table : $run\n";}
  }
}
 close FILE;
}

sub updateAllRunCatalog {
    my $self= shift;
#
    if( not $self->Init()){
        die "updateAllRunCatalog -F- Unable To Init";
        
    }
    if (not $self->ServerConnect()){
        die "updateAllRunCatalog -F- Unable To Connect To Server";
    }
#

    
    my $sql = "SELECT Jobs.jid FROM Jobs, Runs 
               WHERE Jobs.jid=Runs.jid AND Runs.status='Completed'";
    my $r0 = $self->{sqlserver}->Query($sql);
    if(defined $r0->[0][0]){
      foreach my $job (@{$r0}) {
          $self->updateRunCatalog($job->[0]);
      }
  }
}

sub updateRunCatalog {
    my $self= shift;
    my $jid = shift;

    my $runstatus  = undef;
    my $jobcontent = undef;

    my $sql        = undef;
    my $sql0       = undef;
    my $sql1       = undef;

    my $timestamp = time();

    $sql = "SELECT runs.status, jobs.content FROM Runs, Jobs WHERE runs.jid=jobs.jid and jobs.jid=$jid";
    my $r0 = $self->{sqlserver}->Query($sql);
    if(defined $r0->[0][0] && defined $r0->[0][1]){
      $runstatus = $r0->[0][0];
      $jobcontent = $r0->[0][1];       
      if ($runstatus eq 'Completed') {
          $sql = "DELETE runcatalog WHERE run=$jid";
          if ($verbose == 1 && $webmode == 1) {print "$sql \n";}
          $self->{sqlserver}->Update($sql);
            my @sbuf=split "\n",$jobcontent;
            my @runparam;
            $sql0 = "INSERT INTO runcatalog (run";
            $sql1 = "                 VALUES($jid";
            my @farray=
                   ("PART","PMIN","PMAX","TRIGGER","SPECTRUM","SETUP","FOCUS","COSMAX",
                        "PLANENO","GEOCUTOFF",
                        "COOCUB1","COOCUB2","COOCUB3","COOCUB4","COOCUB5","COOCUB6");
            my @narray=(1,1,1,0,0,0,0,1,
                        0,0,
                        1,1,1,1,1,1);
            my $i=0;
            foreach my $ent (@farray){
             foreach my $line (@sbuf){
               if($line =~/$ent=/){
                my @junk=split "$ent=",$line;       
                if (defined $junk[$#junk]) {
                 if ($ent=~/TRIGGER/) {
                  $sql0=$sql0.", TRTYPE";
                 } else {
                  $sql0=$sql0.",".$ent;
                 }
                 if ($narray[$i] == 0) {
                  $sql1=$sql1."," ."'$junk[$#junk]'";         
                 } else {
                  $sql1=$sql1."," .$junk[$#junk];         
                 }
               } else { 
                if ($ent=~/SETUP/) {
                 $sql0=$sql0.", SETUP";
                 $sql1=$sql1.", 'AMS02'";
                }              
                if ($ent=~/TRIGGER/) {
                 $sql0=$sql0.", TRTYPE";
                 $sql1=$sql1.", 'TriggerLVL1'";
                }
              }              
              last;
            }
           }
           $i++;
         }
             foreach my $line (@sbuf){
              if ($line =~ m/generate/) {
                 $line =~ s/C //;
                 $line =~ s/\\//;
                 $line =~ s/\\//;
                 $line = trimblanks($line);
                       $sql0=$sql0.", jobname";
                       $sql1=$sql1.","."'$line'";
                       last;
               }
         
            }
            $sql0=$sql0.", TIMESTAMP) ";
            $sql1=$sql1.",".$timestamp.")";
            $sql=$sql0.$sql1;
            $self->{sqlserver}->Update($sql);
      }
    } else {
       $self->printWarn("updateRunCatalog -W- Cannot Find Run or/and Job content with JID = $jid");
    }
}

sub deleteTimeOutJobs {

    my $self = shift;

    my $vdir = undef;
    my $vlog = undef;
    my $timenow = time();

#
 my $HelpTxt = "
     -c    - output will be produced as ASCII page (default)
     -h    - print help
     -v    - verbose mode
     -w    - output will be produced as HTML page
     ./deleteTimeOutJobs.o.cgi -c -v
";

  foreach my $chop  (@ARGV){
    if ($chop =~/^-c/) {
        $webmode = 0;
    }
    if ($chop =~/^-v/) {
        $verbose = 1;
    }
    if ($chop =~/^-w/) {
     $webmode = 1;
    }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
   }
#


    if( not $self->Init()){
        die "deleteTimeoutJobs -F- Unable To Init";
    }
    if (not $self->ServerConnect()){
        die "deleteTimeOutJobs -F- Unable To Connect To Server";
    }
#
    $vdir=$self->getValidationDir();
    if (not defined $vdir) {
      die " deleteTimeOurJobs -F- cannot get path to ValidationDir \n";
     }
    $vlog = $vdir."/deleteTimeOutJobs.log.".$timenow;   

    open(FILE,">".$vlog) or die "Unable to open file $vlog\n";
#
    if ($webmode == 1) {$self->htmlTop();}

# get list of runs from Server
    if( not defined $self->{dbserver}->{rtb}){
      DBServer::InitDBFile($self->{dbserver});
    }
#
    if ($webmode == 1) {htmlTable("Jobs below will be deleted from the database");}

    my $sql  = undef;
    $sql="SELECT jobs.jid, jobs.timestamp, jobs.timeout, jobs.mid, jobs.cid, 
                 cites.name, mails.name FROM jobs, cites, mails, runs  
               WHERE Jobs.jid = Runs.jid AND  
                     Jobs.cid = Cites.cid AND 
                     Jobs.mid = Mails.mid AND 
                     Runs.status = 'TimeOut'";
    my $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
     if ($webmode == 1) {
      print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
      print "<td><b><font color=\"blue\">Cite </font></b></td>";
      print "<td><b><font color=\"blue\" >JobID </font></b></td>";
      print "<td><b><font color=\"blue\" >Submitted</font></b></td>";
      print "<td><b><font color=\"blue\" >Expired</font></b></td>";
      print "<td><b><font color=\"blue\" >Status</font></b></td>";
      print "<td><b><font color=\"blue\" >Owner</font></b></td>";
      print "</tr>\n";
      print_bar($bluebar,3);
     }
        foreach my $job (@{$ret}) {
            my $jid          = $job->[0];
            my $timestamp    = $job->[1];
            my $timeout      = $job->[2];
            my $tsubmit      = EpochToDDMMYYHHMMSS($timestamp);
            my $texpire      = EpochToDDMMYYHHMMSS($timestamp+$timeout);
            my $mid          = $job->[3];
            my $cid          = $job->[4];
            my $cite         = $job->[5];
            my $owner        = $job->[6];
           
            $sql = "DELETE Ntuples WHERE jid=$jid";
            $self->{sqlserver}->Update($sql);
            $sql = "DELETE Jobs WHERE jid=$jid";
            $self->{sqlserver}->Update($sql);
            print FILE "$sql \n";
            foreach my $runinfo (@{$self->{dbserver}->{rtb}}){
             if($runinfo->{Run}=$jid) {
#--              DBServer::sendRunEvInfo($self->{dbserver},$runinfo,"Delete"); 
              print FILE "*TRY* send Delete to DBServer, run=$runinfo->{Run} \n";
              last;
             }
            }
        $self->amsprint($cite,666);
        $self->amsprint($jid,666);
        $self->amsprint($tsubmit,666);
        $self->amsprint($texpire,666);
        $self->amsprint("TimeOut",666);
        $self->amsprint($owner,0);
        if ($webmode == 1) {print "</tr>\n";}
        }
      if ($webmode == 1) {htmlTableEnd();}
    }


#
# delete jobs without runs
#

    my $timedelete = time() - 60*60;
    $sql="SELECT jobs.jid, jobs.timestamp, jobs.timeout, jobs.mid, jobs.cid, 
                 cites.name FROM jobs, cites 
          WHERE jobs.timestamp+jobs.timeout < $timedelete  AND 
                jobs.cid=cites.cid"; 
    my $r3=$self->{sqlserver}->Query($sql);
    if( defined $r3->[0][0]){
     if ($webmode == 1) {
      print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
      print "<td><b><font color=\"blue\">Cite </font></b></td>";
      print "<td><b><font color=\"blue\" >JobID </font></b></td>";
      print "<td><b><font color=\"blue\" >Submitted</font></b></td>";
      print "<td><b><font color=\"blue\" >Expired</font></b></td>";
      print "<td><b><font color=\"blue\" >Owner</font></b></td>";
      print "</tr>\n";
     }
      foreach my $job (@{$r3}){
            my $jid          = $job->[0];
            my $timestamp    = $job->[1];
            my $timeout      = $job->[2];
            my $tsubmit      = EpochToDDMMYYHHMMSS($timestamp);
            my $texpire      = EpochToDDMMYYHHMMSS($timestamp+$timeout);
            my $mid          = $job->[3];
            my $cite         = $job->[5];

         my $owner = "xyz";
         $sql = "SELECT mails.name, mails.address FROM Mails 
                 WHERE mails.mid=$mid";
         my $r4 = $self->{sqlserver}->Query($sql);
         if (defined $r4->[0][0]) {
            $owner   = $r4->[0][0];
          }

         $sql="SELECT runs.run, runs.status 
                FROM runs 
                WHERE runs.jid = $jid";
         $r4=$self->{sqlserver}->Query($sql); 
         if (not defined $r4->[0][0]) {
           
            $sql = "DELETE Ntuples WHERE jid=$jid";
            $self->{sqlserver}->Update($sql);
            $sql = "DELETE Jobs WHERE jid=$jid";
            $self->{sqlserver}->Update($sql);
            print FILE "$sql \n";
            foreach my $runinfo (@{$self->{dbserver}->{rtb}}){
             if($runinfo->{Run}=$jid) {
              DBServer::sendRunEvInfo($self->{dbserver},$runinfo,"Delete"); 
              print FILE "send Delete to DBServer, run=$runinfo->{Run}\n";
              last;
             }
         }

        $self->amsprint($cite,666);
        $self->amsprint($jid,666);
        $self->amsprint($tsubmit,666);
        $self->amsprint($texpire,666);
        $self->amsprint("TimeOut",0);
        if ($webmode == 1) {print "</tr>\n";}
       }
     }
     if ($webmode == 1) {
       htmlTableEnd();
      htmlTableEnd();
     }
   }


   if ($webmode == 1) {
    htmlTableEnd();
    $self->htmlBottom();
   }
    close FILE;
}

sub insertRun {

    my $self   = shift;
    my $run    = shift;
    my $jid    = shift;
    my $fevent = shift;
    my $levent = shift;
    my $fetime = shift;
    my $letime = shift;
    my $submit = shift;
    my $status = shift;

    my $doinsert = 0;
    my $sql      = undef;

    $sql="SELECT run, jid, fevent, levent, status 
          FROM Runs WHERE run=$run";
    my $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
     my $dbrun = $ret->[0][0];
     my $dbjid = $ret->[0][1];
     my $dbfevent = $ret->[0][2];
     my $dblevent = $ret->[0][3];
     my $dbstatus = $ret->[0][4];
     if ($dbjid == $run &&
          $dbfevent == $fevent &&
          $dblevent == $levent &&
          $dbstatus eq $status) {
            print "InsertRun -W- Run $run already exists do nothing \n";
           } else {
            $sql="DELETE runs WHERE run=$run";
            $self->{sqlserver}->Update($sql);
            $doinsert = 1;
            }
      } else {
        $doinsert = 1;
      }
      if ($doinsert == 1) {
       $sql="INSERT INTO Runs VALUES(
                    $run,
                    $jid,
                    $fevent,
                    $levent,
                    $fetime,
                    $letime,
                    $submit,
                    '$status')";
        $self->{sqlserver}->Update($sql);
        if ($verbose == 1) {print "$sql \n";}
        if ($status eq "Completed") {
          $self->updateRunCatalog($run);
          if ($verbose == 1) {print "Update RunCatalog table : $run->{Run}\n";}
       }
   }
}

sub getValidationDir {
    my $self = shift;
    my $dir  = undef;

    my $sql="SELECT myvalue FROM Environment WHERE mykey='ValidationDirPath'";
    my $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
        $dir = $ret->[0][0];
    }
    return $dir;
}

sub findJob {

    my $self = shift;
    my $jid  = shift;

    my $rstatus = 0;

    my $sql = "SELECT jid FROM Jobs WHERE jid=$jid";
    my $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
        $rstatus = $ret->[0][0];
    }

    return $rstatus;
}

sub getRunStatus {

    my $self = shift;
    my $jid  = shift;
    my $run  = shift;

    my $sql    = undef;
    my $status = "Unknown";

    if ($jid > 0) {
        $sql = "SELECT status FROM Runs WHERE jid=$jid";
    } elsif ($run > 0) {
        $sql = "SELECT status FROM Runs WHERE run=$run";
    }    

    my $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
        $status = $ret->[0][0];
    }

    return $status;
}


sub insertNtuple {
  my $self     = shift;
#
  my $run      = shift;   # run id
  my $version  = shift;   # production version
  my $type     = shift;   # ROOT/NT
  my $jid      = shift;   # job id
  my $fevent   = shift;   # first event
  my $levent   = shift;   # last event
  my $events   = shift;   # total events
  my $errors   = shift;   # total errors
  my $timestamp= shift;   # insert time
  my $ntsize   = shift;   # size MB 
  my $status   = shift;   # status
  my $path     = shift;   # full file path
  my $crc      = shift;   # crc
  my $crctime  = shift;   # last CRC calc time
  my $crcflag  = shift;   # CRC flag
  my $castortime = shift; # last castor copy time
#
  my $sql      = undef;
  my $ret      = undef;
#
  my @junk    = split "/",$path;
  my $filename = trimblanks($junk[$#junk]);

  $sql = "SELECT run, path FROM ntuples 
          WHERE run=$run AND path like '%$filename%'";
  $ret = $self->{sqlserver}->Query($sql);
  if (defined $ret->[0][0]) {
    $sql = "DELETE ntuples WHERE run=$run AND path like '%$filename%'";
    print "------------- $sql \n";
    print "------------- $ret->[0][1] \n";
#   $self->{sqlserver}->Update($sql);
  }
  my $sizemb = $ntsize; # since May 7.03 size in bytes
  if ($ntsize > 2000) {
    $ntsize = $ntsize/1024/1024;
    $sizemb = sprintf("%.f",$ntsize);
  }
#              
  $sql = "INSERT INTO ntuples VALUES( $run,
                                         '$version',
                                         '$type',
                                          $jid,
                                          $fevent,
                                          $levent,
                                          $events,
                                          $errors,
                                          $timestamp,
                                          $sizemb,
                                          '$status',
                                          '$path',
                                           $crc, 
                                           $crctime,$crcflag,$castortime)"; 
  $self->{sqlserver}->Update($sql);

}

sub printMC02GammaTest {
    my $self = shift;

    my $buf;

    my $mc02gammafile = "/afs/ams.cern.ch/AMS02/MC/mc02-gamma-test.datasets";

    open(FILE,"<".$mc02gammafile) or die "Unable to open $mc02gammafile \n";
    read(FILE,$buf,1638400);
    close FILE;
  
    my @lines=split "\n",$buf;
  
    htmlTop();
      print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
      print "<td><b><font color=\"blue\">Dataset </font></b></td>";
      print "<td><b><font color=\"blue\" >Tape # </font></b></td>";
      print "</tr>\n";
  foreach my $line (@lines) {
        my @junk = split ';',$line;
        if ($#junk > 0) {
         my $dataset = $junk[0];
         my $tape    = $junk[1];
         if ($#junk > 1) { $tape = $tape.";".$junk[2]};
         print "<td><b><font color=\"black\">$dataset </font></b></td>";
         print "<td><b><font color=\"black\">$tape </font></b></td>";
         print "</tr>\n";
     } else {
         print "<td><b>--------------------------------</b><td></tr>\n";
     }
    }

    htmlBottom();
}

sub printJobParamFormatDST {

    my $self=shift;

# DST output format
            print "<tr><td><b><font color=\"green\">DST output format</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
            print "<INPUT TYPE=\"radio\" NAME=\"RootNtuple\" VALUE=\"1=0 168=500000000 127=2 128=\" $defROOT><b> RootFile </b><BR>\n";
            print "<INPUT TYPE=\"radio\" NAME=\"RootNtuple\" VALUE=\"1=3 168=120000000 2=\" $defNTUPLE><b> NTUPLE </b>\n";
            print "</b></font></td></tr>\n";
           htmlTableEnd();
}

sub printJobParamTransferDST {

    my $self=shift;

# DST transfer
             print "<tr><td><b><font color=\"green\">DST Transfer Mode </font></b>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
# Apr 1.2003 set default transfer to 'No' for both mode - remote/local
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"R\" ><b> Automatic </b><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"L\" CHECKED><b> Manual </b><BR>\n";
#            }
             print "</b></font></td></tr>\n";
            htmlTableEnd();
}

sub set_root_env {
#
# this function sets up the necessary environement variables
# to be able to use ROOT
#
    my $self = shift;
    my $ROOTSYS = $self->{ROOTSYS};
    if ($webmode ==0 and $verbose ==1) {print "------------ ROOTSYS : $ROOTSYS \n\n";}
    $ENV{"ROOTSYS"}=$ROOTSYS;
    $ENV{"PATH"}=$ENV{"PATH"}.":".$ENV{"ROOTSYS"}."/bin";
    $ENV{"LD_LIBRARY_PATH"}=$ENV{"LD_LIBRARY_PATH"}.":".$ENV{"ROOTSYS"}."/lib";
    1;
}

sub set_URL {

    my $self = shift;
    my $html= "http://".$self->{HTTPserver};
    my $cgi = "http://".$self->{HTTPserver}."/cgi-bin/mon";
    $downloadcgi       =$cgi."/download.o.cgi";
    $downloadcgiMySQL  =$cgi."/download.mysql.cgi";

    $monmcdb      =$cgi."/monmcdb.o.cgi";
    $monmcdbMySQL =$cgi."/monmcdb.mysql.cgi";

    $rccgi      =$cgi."/rc.o.cgi?queryDB04=Form";
    $rccgiMySQL =$cgi."/rc.mysql.cgi?queryDB04=Form";

    $rchtml=$html."/rc.html";

    $validatecgi      =$cgi."/validate.o.cgi";
    $validatecgiMySQL =$cgi."/validate.mysql.cgi";

}

sub getProductionVersion {
#
# production set version
# v4.00/build78/os2
#
    my $self = shift;
   
    my $sql     = undef;
    my $ret     = undef;
    my $version = 0;
    my $vbuild  = 0;
    my $os      = undef;

    $sql = "SELECT version FROM ProductionSet WHERE STATUS='Active'";
    my $r0 = $self->{sqlserver}->Query($sql);
    if (defined $r0->[0][0]) {
      my @junk = split '/',$r0->[0][0];
      $junk[0] =~ s/v//;
      $junk[1] =~ s/build//;      
      $junk[2] =~ s/os//;
      $self->{ProductionVersion}=$r0->[0][0];
      $self->{Version}=strtod($junk[0]);
      $self->{Build}  =strtod($junk[1]);
      $self->{OS}     =strtod($junk[2]);
 }
}

sub checkDSTVersion  {
#
# compare DST version with min allowed 
# version for the 'Active' productionset
# send '0' if DST version < MIN
#
    my $self = shift;
    my $dstv = shift;

    my $rstatus = 1; 

    if (defined $self->{ProductionVersion} && 
        $self->{Version} >=0 &&
        $self->{Build}  >=0  &&
        $self->{OS}     >=0) {

      my @junk = split '/',$dstv;
      $junk[0] =~ s/v//;
      $junk[1] =~ s/build//;      
      $junk[2] =~ s/os//;
      my $version=strtod($junk[0]);
      my $build  =strtod($junk[1]);
      my $os     =strtod($junk[2]);

      if ($version < $self->{Version} ||
          $build   < $self->{Build}) {
          $rstatus = 0;
      }      
  }
    return $rstatus;
}

sub writetofile {
    my $self         = shift;
    my $file         = shift;
    my $buff         = shift;
# open new file
           my $timenow = time();
           $timenow    = localtime($timenow);
           open(FILE,">".$file) or die "Unable to open file $file\n";
           print FILE "# File Created : $timenow \n";
           print FILE "# \n";
           print FILE "$buff";
           close FILE;
}

sub inputList {
    $inputFiles[$nTopDirFiles] = $File::Find::name;
    $nTopDirFiles++;
}


sub checkDB {
  my $self   = shift;
  my $sql    = undef;

# options
  my $topdir = undef;
  my $ntpath = undef;
  my $outputfile = undef;
  my $validate = 0; # run dsts validation program
  my $checkCRC = 0; # check CRC for files
  my $updateDB = 0; # set last crc check time

  my $HelpTxt = "
     checkDB compares DB and directories contents
     -d    - directory path (d:directory)
     -i    - internal content of db, that none appears twice
     -p    - DST query word for Database (p:query)
     -crc  - calculate CRC and compare them 
     -o    - write output to file
     -h    - print help
     -u    - update DB (valid only with -crc)
     -v    - verbose mode
     -Vldt - run DST validation program 

     ./checkdb.cgi -d:/f2dah1/MC/AMS02/2004A/protons -crc
     ./checkdb.cgi -p:/f2dat1/MC/AMS02/2004A -i 
     ./checkdb.cgi -d:/f2dat1/MC/AMS02/2004A/protons -Vldt 
";
#

  my $missed     = 0; # not found on disk
  my $zerolength = 0; # file size = 0
  my $content    = 0; # internal DB content
  my $correct    = 0; # found in DB and on disk
  my $vldterror  = 0; # validation error
  my $ferror     = 0; # file access error
  my $crcerror   = 0; # CRC doesn't match 
  my $notindb    = 0; # not found in db
  my $nfiles     = 0;
  my $nupdate    = 100; # print statistics when $nupdate files are checked
  $nTopDirFiles = 0;
  
  foreach my $chop  (@ARGV){
    if($chop =~/^-d:/){
       $topdir=unpack("x3 A*",$chop);
    } elsif ($chop =~/^-p:/) {
       $ntpath=unpack("x3 A*",$chop);
   }
    if ($chop =~/^-i/) {
        $content = 1;
    }

    if ($chop =~/^-crc/) {
        $checkCRC = 1;
        $nupdate  = 50;
    }

    if ($chop =~/^-Vldt/) {
        $validate = 1;
        $nupdate  = 50;
    }


    if ($chop =~/^-o:/) {
        $outputfile = unpack("x3 A*",$chop);
    }
    if ($chop =~/^-u/) {
     $updateDB = 1;
    }
    if ($chop =~/^-v/) {
     $verbose = 1;
    }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
   }
   
   if ((not defined $topdir) && (not defined $ntpath) && $content==0) {
    print "$HelpTxt \n";
    return 1;
   }
   my $timenow = time();
   print "***** checkDB is started : $timenow \n";
   if (defined $topdir) {
       print "  Files from directories $topdir will be checked \n";
       print "  with DB content \n";
   } elsif (defined $ntpath) {
       print "  All DSTs with PATH like %$ntpath% will be checked \n";
   }
   if ($checkCRC == 1) {
      print "  Files CRC will be calculated and to compare with ones stored in DB \n";
  } else {
      $updateDB = 0;
  }
  if ($updateDB == 1) {
      print "  CRCTime and CRCFlag of NTUPLES Table will be updated \n";
  }
  if (defined ($outputfile)) {
      print "The output will be stored into $outputfile \n";
  }
  if ($validate == 1) {
      print " DSTs will be revalidated \n";
  }
 if (defined $ntpath && $content == 0) {
    $sql = "SELECT COUNT(PATH) FROM ntuples WHERE PATH LIKE '%$ntpath%'";
    my $ret=$self->{sqlserver}->Query($sql);
    if(defined $ret->[0][0]){
      if ($ret->[0][0] > 0) {
        $nfiles = $ret->[0][0];
        print "$nfiles DSTs matched to path like '$ntpath'\n";
        print "start comparison \n";
#-
        $sql = "SELECT PATH, TYPE, LEVENT, NEVENTS, CRC FROM ntuples WHERE PATH LIKE '%$ntpath%' ORDER BY RUN";
        $ret=$self->{sqlserver}->Query($sql);
        if(defined $ret->[0][0]){
         foreach my $nt (@{$ret}){
             my $skip     = 0;
             my $filename = $nt->[0];
             my $dsttype  = $nt->[1];
             my $levent   = $nt->[2];
             my $events   = $nt->[3];
             my $crc      = $nt->[4];
             my $notfound = 0;
             my $filesize    = (stat($filename))[7] or $notfound=1;
             if ($notfound == 1) {
              print "*not found* $filename \n";
              $missed++;
            } else {
             if ($filesize > 0) {
              if ($validate == 1) {
                my ($ret,$i) = $self->validateDST($filename,$events,$dsttype,$levent);
                if ($ret == 1) {
                 $i=($i>>8);
                 if(int($i/128)){
                   print "validateDST -E- $filename : STATUS=Bad.($i-128) \n";  
                   $vldterror++;
                   $skip = 1;
                 }
                } else {
                    print "validateDST -W- check $filename \n";
                    $ferror++;
                    $skip = 1;
                }
              }
              if ($checkCRC == 1 && $skip==0) {
               my $rstatus = $self->calculateCRC($filename,$crc);
               if ($rstatus == 1) {
                $correct++;
               } else {
                 $rstatus = 0;
                 $crcerror++;
               }
               if ($updateDB == 1) {
                 my $timenow = time();
                 $sql = "UPDATE ntuples SET crctime = $timenow, crcflag=$rstatus  
                           WHERE path='$filename'";
                 if ($verbose == 1) {print "$sql \n";}
                 $self->{sqlserver}->Update($sql); 
               }
              }
             } else {
                 $zerolength++;
                 print "*file size = $filesize* $filename \n";
             }
         }
         }
     }
#-
   } else {
       print "NO  DSTs matched to path like '$ntpath'. Quit.\n";
   }
  } else {
       print "NO  DSTs matched to path like '$ntpath'. Quit.\n";
   }
  } elsif (defined $topdir) {
   find (\&inputList, $topdir);
    my $i= 0;
    my $nprint = 0;
    while ($i <= $#inputFiles) {
      if (-d $inputFiles[$i]) {
          if ($verbose == 1) {print "Directory : $inputFiles[$i] \n";}
      } else {
          $nfiles++;
          my $filename = trimblanks($inputFiles[$i]);
          $sql = "SELECT PATH, TYPE, LEVENT, NEVENTS, CRC FROM ntuples WHERE PATH LIKE '$filename'";
          my $ret=$self->{sqlserver}->Query($sql);
          if(not defined $ret->[0][0]){
              $notindb++;
              print "* not in DB* $filename\n";
          } else {
             my $skip     = 0;
             my $filename = $ret->[0][0];
             my $dsttype  = $ret->[0][1];
             my $levent   = $ret->[0][2];
             my $events   = $ret->[0][3];
             my $crc      = $ret->[0][4];

              if ($validate == 1) {
                my ($ret,$i) = $self->validateDST($filename,$events,$dsttype,$levent);
                if ($ret == 1) {
                 $i=($i>>8);
                 if(int($i/128)){
                   print "validateDST -E- $filename : STATUS=Bad.($i-128) \n";  
                   $vldterror++;
                   $skip = 1;
                 }
                } else {
                    print "validateDST -W- check $filename \n";
                    $ferror++;
                    $skip = 1;
                }
              }

            if ($checkCRC == 1 && $skip == 0) {
              my $rstatus = $self->calculateCRC($filename,$crc);
              if ($rstatus == 1) {
               $correct++;
              } else {
                $crcerror++;
              }
             if ($updateDB == 1) {
              my $timenow = time();
              $sql = "UPDATE ntuples SET crctime = $timenow, crcflag=$rstatus  
                           WHERE path='$filename'";
              if ($verbose == 1) {print "$sql \n";}
              $self->{sqlserver}->Update($sql); 
             }
          }
       $nprint++;
          if ($nprint == $nupdate) {
              my $t = time();
              my $l = localtime($t);
           print "$l - Files Checked : $nfiles not found in DB $notindb CRC error : $crcerror  Not Validated : $vldterror\n";
           $nprint = 0;
          }
       }
      }
      $i++;
  }
  print "Total Files Checked : $nfiles not found in DB $notindb ";
  if ($checkCRC == 1) {print " CRC error : $crcerror";}
  if ($validate == 1) {
    print " Validation errors : $vldterror";
    print " Fileaccess errors : $ferror";
  }
  print "\n";
} elsif ($content == 1) {
    my $N  = 0;
    my $Nd = 0;
    $sql = "SELECT path, timestamp FROM NTUPLES ORDER BY path, timestamp";
    if (defined $ntpath) {
     $sql = "SELECT path, timestamp FROM ntuples WHERE PATH LIKE '%$ntpath%' ORDER BY path,timestamp";
    }
    if ($verbose == 1) {
        print "$sql \n";
    }
    my $ret=$self->{sqlserver}->Query($sql);
    if(not defined $ret->[0][0]){
        $self->amsprint("checkDB - Table NTUPLES is empty",0);
    } else {
       foreach my $nt0 (@{$ret}){
        $N++;
        my $path0 = trimblanks($nt0->[0]);
        my $t0    = $nt0->[1];
        my $t1    = 0;
        my $nn = 0;
        foreach my $nt1 (@{$ret}) {
          my $path1 = trimblanks($nt1->[0]);
          $t1    = $nt1->[1];
          if ($path1 =~ $path0) { 
              $nn++;}
        }
        if ($nn == 0) {
         print "check your program - $path0 ?? $nn \n";        
        } elsif ($nn > 1) {
          print "$path0 found $nn times. Timestamps : $t0, $t1 \n";
          $Nd++;
        }
       }
    }
    print "Total Files $N, to be checked $Nd \n";
 }
  return 1;
}


sub calculateCRC {
  my $self      = shift;
  my $filename  = shift;
  my $crc       = shift;

  my $time0     = time();
  $crcCalls++;

  my $crccmd    = "$self->{AMSSoftwareDir}/exe/linux/crc $filename  $crc";
  my $rstatus   = system($crccmd);
  $rstatus=($rstatus>>8);
  if ($verbose == 1 && $webmode == 0) {print "$crccmd : $rstatus \n";}

  my $time1     = time();

  $crcTime = $crcTime + $time1 - $time0;

  return $rstatus;
}

sub amsprint {
   my $self      = shift;
   my $msg       = shift;
   my $txt       = $msg;
   my $color     = shift;

   if ($webmode==1) {
    $txt = "<td><b><font color=$color>".$txt."</font></b></td>";
    print $txt;
   } else {
       if ($color != 666) {$txt =$txt."\n";}
       print "$txt ";
   }
}

sub printParserStat {
   my $self      = shift;
   my $timenow = time();
   my $ltime   = localtime($timenow);

   my $totalRuns    = 0;
   my $totalDSTs    = 0;
   my $totalRunsBad = 0;
   my $totalDSTsBad = 0;
   my $totalGB      = 0;

    my $vdir=$self->getValidationDir();
    if (not defined $vdir) {
        $vdir = "/tmp";
     }
    my $vlog = $vdir."/parseRunsSummary.log.".$timenow;   
    open(FILEV,">".$vlog) or die "Unable to open file $vlog\n";
    if ($webmode ==0  and $verbose ==1) { print "printParserStat -I- open $vlog \n";}

   print "\n\n\n";
   my $firstline = "------------- parseJournalFiles ------------- \n";
   my $lastline  = "-------------     Thats It          ------------- \n";

   print $firstline;
   print FILEV $firstline;

   my $stime   = localtime($parserStartTime);
   my $hours   = ($timenow - $parserStartTime)/60/60;
   my $t0      = "Start Time : $stime \n";
   my $t1      = "End   Time : $ltime \n";
   print $t0,$t1;
   print   FILEV $t0,$t1;

   my $ctt = "Cites      : $nCheckedCite , active : $nActiveCites \n";
   print $ctt;
   print FILEV $ctt;

   for (my $i=0; $i<$nCheckedCite+1; $i++) {
     print "\n";
     my $cj = sprintf ("%-20.15s %-20.40s %-50.30s","Cite : $JouDirPath[$i]", "Latest Journal : $JouLastCheck[$i]", "New Files : $JournalFiles[$i] \n");
     print $cj;
     print FILEV "\n",$cj;
     if ($JournalFiles[$i] > 0) {
      my $l0 = "   Runs (Checked, Good, Bad, Failed) :  $CheckedRuns[$i], $GoodRuns[$i],  $BadRuns[$i], $FailedRuns[$i] \n";
      my $l1 = "   DSTs (Checked, Good, Bad, CRCi, CopyFail, CRCo) :  ";
      my $l2 = "$CheckedDSTs[$i],  $GoodDSTs[$i], $BadDSTs[$i], $BadCRCi[$i], $BadDSTCopy[$i], $BadCRCo[$i] \n";
      print "\n ",$l0,$l1,$l2;
      print FILEV "\n",$l0,$l1,$l2;

      $totalRuns    = $totalRuns    + $CheckedRuns[$i];
      $totalRunsBad = $totalRunsBad + $BadRuns[$i] + $FailedRuns[$i];
      $totalDSTs    = $totalDSTs    + $CheckedDSTs[$i];
      $totalDSTsBad = $totalDSTsBad + $BadDSTs[$i] + $BadCRCi[$i] + $BadDSTCopy[$i] + $BadCRCo[$i];
      $totalGB      = $totalGB      + $gbDST[$i];
     }
 }
 $totalGB = $totalGB/1000;
 my $chGB = sprintf("%3.1f",$totalGB);
 my $summ =  "Total : Runs $totalRuns ($totalRunsBad), DSTs $totalDSTs ($totalDSTsBad), GB $chGB \n";
 print $summ;
 my $ch0 = sprintf("Total Time %3.1f hours \n",$hours);
 my $ch1 = sprintf(" doCopy (calls, time) : %5d %3.1fh [cp file :%5d %3.1fh]; \n",$doCopyCalls, $doCopyTime/60/60, $copyCalls, $copyTime/60/60);
 my $ch2 = sprintf(" CRC (calls,time) : %5d, %3.1fh ; Validate (calls,time) : %5d, %3.3fh \n",
                   $crcCalls, $crcTime/60/60,$fastntCalls, $fastntTime/60/60 );
 print $ch0,$ch1,$ch2,$lastline;
 print FILEV $summ, $ch0, $ch1, $ch2, $lastline;
 close FILEV;
}

sub updateFilesProcessing {
    my $self = shift;
    my $timenow = time();
    
    my $nCites = $nCheckedCite+1;

    my $nJournalFiles = 0;
    my $nGoodRuns     = 0;
    my $nBadRuns      = 0;
    my $nFailedRuns   = 0;
    my $nGoodDSTs     = 0;
    my $nBadDSTs      = 0;

    for (my $i=0; $i<$nCites; $i++) {
        $nJournalFiles =  $nJournalFiles + $JournalFiles[$i];
        $nGoodRuns     =  $nGoodRuns     + $GoodRuns[$i];
        $nFailedRuns   =  $nFailedRuns   + $FailedRuns[$i] + $BadRuns[$i];
        $nGoodDSTs     =  $nGoodDSTs     +  $GoodDSTs[$i];
        $nBadDSTs      =  $nBadDSTs      +  $BadDSTs[$i] + $BadCRCi[$i] + $BadDSTCopy[$i] + $BadCRCo[$i];
    }
    my $sql = "UPDATE FilesProcessing SET Cites=$nCites, Active=$nActiveCites, JOU=$nJournalFiles,
                                          GOOD=$nGoodRuns, Failed=$nFailedRuns, GoodDSTS=$nGoodDSTs,
                                          BadDSTS = $nBadDSTs, Timestamp=$timenow";
    $self->{sqlserver}->Update($sql); 
}

sub printWarn {
    my $self = shift;
    my $warn = shift;

    if ($webmode == 1) {
     htmlText($warn," ");
    } else {
     $self->amsprint($warn,0);
    }
}

sub getFilesProcessingFlag {
# 
# set flag in DB showed that Parcing/Validation is in progress
#
# return  = -1   - error
#            0/1 - flag
#
    my $self = shift;

    my $return = -1;

    my $whoami = getlogin();
    if ($whoami =~ 'ams' || $whoami =~ 'alexei') {
    } else {
      $self->amsprint(" -ERROR- script cannot be run from account : $whoami",0);
      die "bye";
    }

   my $timenow = time();
   my $sql = "SELECT flag from FilesProcessing";
   my $ret = $self->{sqlserver}->Query($sql);
   if (defined $ret) { $return = $ret->[0][0];}
   return $return;
}


sub setFilesProcessingFlag {
# 
# set flag in DB showed that Parcing/Validation is in progress
#
    my $self = shift;


    my $whoami = getlogin();
    if ($whoami =~ 'ams' || $whoami =~ 'alexei') {
    } else {
      $self->amsprint(" -ERROR- script cannot be run from account : $whoami",0);
      die "bye";
    }

   my $timenow = time();
   my $sql = "update FilesProcessing set flag = 1, timestamp=$timenow";
   $self->{sqlserver}->Update($sql); 
}

sub resetFilesProcessingFlag {
#
# set flag in DB to stop journal 
# file processing or runs validation
#
    my $self = shift;


    my $whoami = getlogin();
    if ($whoami =~ 'ams' || $whoami =~ 'alexei') {
    } else {
      $self->amsprint("-ERROR- script cannot be run from account : $whoami",0);
      die "bye";
    }

   my $timenow = time();
   my $sql = "update FilesProcessing set flag = 0, timestamp=$timenow";
   $self->{sqlserver}->Update($sql); 

}

sub initFilesProcessingFlag {
#
# set flag in DB to stop journal 
# file processing or runs validation
#
    my $self = shift;
 
    my $timenow = time();

    my $sql = "delete FilesProcessing";
    $self->{sqlserver}->Update($sql); 
    $sql = "insert into FilesProcessing values(0,0,0,0,0,0,0,1,$timenow)";
    $self->{sqlserver}->Update($sql); 
}

sub getOutputPath {
#
# select disk to be used to store ntuples
#
    my $self       = shift;
    my $sql        = undef;
    my $ret        = undef;
    my $outputdisk = undef;
    my $outputpath = 'xyz';
    my $mtime      = 0;
    my $gb         = 0;
#

# get production set path
     $sql = "SELECT disk, path, available, allowed  FROM filesystems WHERE AVAILABLE > $MIN_DISK_SPACE
                  AND allowed > occupied+10 ORDER BY priority DESC, available DESC";
     $ret = $self->{sqlserver}->Query($sql);
     foreach my $disk (@{$ret}) {
      $outputdisk = trimblanks($disk->[0]);
      $outputpath = trimblanks($disk->[1]);
      if ($outputdisk =~ /vice/) {
       $outputpath = $outputpath."/".$ActiveProductionSet;
      } else {
       $outputpath = $outputdisk.$outputpath."/".$ActiveProductionSet;
      }
      $mtime = (stat $outputpath)[9];
#      print "$outputpath, mtime ... $mtime \n";
      if ($mtime != 0) { $gb = $disk->[2]; last;}
  }
     return $outputpath, $gb;
}

sub getHostsList {
    my $self = shift;

    my $sql       = undef;
    my $shortlist = 0;
    my $cl        = undef; # cites returned by sql
    my $hl        = undef; # hosts returned by sql

    my @hostlist   =(); # host;s liat per cite
    my @njobs      =(); # jobs per host
    my @nmips      =(); # mips per host

    my @chhosts    =(); # all hosts for all cites
    my @chjobs     =(); # jobs " "
    my @chmips     =(); # mips " "
    my @cnames     =(); # cites names
    my @chosts     =(); # total hosts per cite
    my @cjobs      =(); # total jobs per cite
    my @cmips      =(); # total mips per cite
    my @gbytes     =(); # gbytes per cite

    my $totaljobs  = 0;
    my $totalmips  = 0;
    my $totalgb    = 0;

    my $kCites   = 0; # known Cites
    my $nCites   = 0;
    my $nHosts   = 0;
    my $nJobs    = 0; # jobs per cite
    my $nMips    = 0; # mips per cite
    my $ndays    = 0; # days of running
    my $p3ghz    = 0; # PIII 1 GHz equivalent
    my $p3ghzday = 0; #                       per day
    my $ghz    = 0; # PIII 1 GHz equivalent
    my $ghzday = 0; #                       per day

    my $CiteName = undef;

  my $HelpTxt = "
     gethostslist get list of hosts where MC jobs run

     -c    - get list for particular cite (c:mycite)
     -h    - print help
     -s    - print short list (no hosts info per cite)
     -v    - verbose mode
     
     from pcamsf0 and pcamss0 only :

     ./gethostslist.cgi -c:ciemat -v
";


  foreach my $chop  (@ARGV){
    if($chop =~/^-c:/){
       $CiteName=unpack("x3 A*",$chop);
       $CiteName=trimblanks($CiteName);
    } 
    if ($chop =~/^-v/) {
     $verbose = 1;
    }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
    if ($chop =~/^-s/) {
        $shortlist = 1;
    }
   }

    my ($prodstart,$prodlastupd,$totaldays) = $self->getRunningDays();
    my $lt = localtime($prodstart);
    my $lu = localtime($prodlastupd);
    my $ld = sprintf("%3.1f",$totaldays);
    print "\n \n ************** Production Started : $lt (Last Updated $lu) \n";
    print "  ******************     Production time : $ld days \n";
    if (defined $CiteName) {
     $sql = "SELECT cid, descr FROM CITES WHERE name='$CiteName'";
    } else {
     $sql = "SELECT cid, descr FROM CITES ORDER BY cid";
    }
    $cl   =$self->{sqlserver}->Query($sql);
    
    if (defined $cl->[0][0]) {    
        foreach my $cite (@{$cl}) {
         $kCites++;
         my $cid = $cite->[0];
         $cnames[$cid] = $cite->[1];

         $sql = "SELECT SUM(sizemb)  from jobs, ntuples where ntuples.jid=jobs.jid and cid=$cid";
         my $r = $self->{sqlserver}->Query($sql);
         if (defined $r->[0][0]) {
             my $sizegb = $r->[0][0]/1000;
             $gbytes[$cid] = $sizegb;
             $totalgb += $sizegb;
         } else {
             $gbytes[$cid] = 0;
         }
         $totaljobs  = 0;
         $totalmips  = 0;

         $sql  = "SELECT host, mips, cputime FROM Jobs WHERE host != 'host' and cid=$cid ORDER BY host";
         $hl=$self->{sqlserver}->Query($sql);
         if (defined $hl->[0][0]) {
          foreach my $host (@{$hl}){
            my $hostname = trimblanks($host->[0]);
            my $mips    = 0;
            my $cputime = 0;
            if (defined $host->[1]) {$mips = $host->[1];}
            if (defined $host->[2]) {$cputime = $host->[2];}
            $totaljobs++;
            $totalmips += $mips*$cputime/1000.;

            $hostname = $self->gethostname($host->[0]);

            my $newcomp = 1;
            foreach my $comp (@hostlist) {
                if ($comp =~ $hostname) {
                  $newcomp = 0;
                  $njobs[$#hostlist]++;   
                  $nmips[$#hostlist] += $totalmips/60/60/24.;
                }
            }
            if ($newcomp == 1) {
             push @hostlist, $hostname;
             $njobs[$#hostlist] = 1;
             $nmips[$#hostlist] = $totalmips/60/60/24.;
            }
          }
      }
       my $j = $#hostlist+1;

       if ($totaljobs > 0) {
        $nCites++;
        $nJobs  += $totaljobs;
        $nMips  += $totalmips;
        $nHosts += $j;

        my $n = $cid*1000;

        for (my $i=0; $i<$j+1; $i++) {
            $chhosts[$n+$i] = $hostlist[$i];
            $chjobs[$n+$i]  = $njobs[$i];
            $chmips[$n+$i]  = $nmips[$i];
        }
        $cjobs[$cid]  = $totaljobs;
        $chosts[$cid] = $j;
        $cmips[$cid]  = $totalmips/$totaldays/24/60/60;

        @hostlist =();
        @njobs    =();
        @nmips    =();

      } else {
        $gbytes[$cid] = 0;
        $cjobs[$cid]  = 0;
        $chosts[$cid] = 0;
        $cmips[$cid]  = 0;
      }
     }
        for (my $c=1; $c<$kCites+1; $c++) {
         my $cite = -1;
         my $maxgb    =  0;
         for (my $m=1; $m<$kCites+1; $m++) {
           if ($gbytes[$m] > $maxgb) {
               $maxgb = $gbytes[$m];
               $cite = $m;
           }
         }
           if ($cite != -1) {
           print "\n \n ";
            my $sgb = sprintf(" %6.1f",$gbytes[$cite]);
            $gbytes[$cite] = -1;
            print "Cite : $cnames[$cite] \n";
            print " Hosts : $chosts[$cite], Jobs : $cjobs[$cite],  GB : $sgb \n";

            $p3ghzday    = sprintf("%3.1f",$cmips[$cite]);
            $p3ghz = sprintf("%3.1f",($cmips[$cite]*$totaldays));

           $ghz    += $p3ghz;
           $ghzday += $p3ghzday;

            print " PIII 1GHz equivalent : $p3ghz or per day $p3ghzday \n";
            if ($shortlist == 0) {
             my $i = 0;
             my $j = 0;
             my $n = $cite*1000;
             for (my $i=0; $i<$chosts[$cite]; $i++) {
              if ($j == 0) { print "\n";}
              printf (" %10s %5d %6d",$chhosts[$n+$i],$chjobs[$n+$i],$chmips[$n+$i]);
              $j++;
              if ($j == 3) { $j=0;}
             }
            }
        } else {
            last;
        }
     }
        print "\n";
        print "---------------- Summary --------------------- \n";
        my $sgb = sprintf(" %6.1f",$totalgb);
        print "Active Cites : $nCites, Total Jobs : $nJobs, Hosts : $nHosts \n";
        print "Total GB: $sgb, PIII 1GHz equiv : $ghz or per day $ghzday \n";
        print "----------------         --------------------- \n";

    } else {
        if (defined $CiteName) {
            print "Warning - Cannot find cite with name $CiteName \n";
        } else {
            print "Warning - Table cites is empty \n";
        }
   }         
    return 1;
}


sub setActiveProductionSet {
     my $self = shift;
     

     my $sql  = undef;
     my $ret  = undef;

      $sql = "SELECT NAME, BEGIN  FROM ProductionSet WHERE STATUS='Active'";
      $ret = $self->{sqlserver}->Query($sql);
      if (defined $ret->[0][0]) {
       $ActiveProductionSet=trimblanks($ret->[0][0]);
       $ProductionStartTime = $ret->[0][1];
# check AMS01 / AMS02 production and set default output format accordingly
       if ($ActiveProductionSet =~ /AMS01/) {
        $defNTUPLE = "CHECKED";
        $defROOT   = " ";
       }

      } else {
        $ActiveProductionSet = $UNKNOWN;
        $ProductionStartTime = 0;
      }
 }

sub validateDST {
     my $self    = shift;
     my $fname   = shift;  # file name
     my $nevents = shift;  # number of events
     my $ftype   = shift;  # file type : root / ntuple
     my $levent  = shift;  # last event number


     my $dtype       = undef;
     my $validatecmd = undef;
     my $ret         =  0;     # return code
     my $vcode       =  0;     # validate code

     my $time0 = time();
     $fastntCalls++;


     if($ftype eq "Ntuple") {
         $dtype = 0;
     } elsif ($ftype eq "RootFile") {
         $dtype = 1;
     }
     if (defined $dtype) {
      $validatecmd = "$self->{AMSSoftwareDir}/exe/linux/fastntrd.exe  $fname $nevents $dtype $levent";
      $vcode=system($validatecmd);
      if ($verbose == 1) {print "$validatecmd : $vcode \n";}
      $ret = 1;
     }

     my $time1 = time();
     $fastntTime += $time1 - $time0;
     return $ret,$vcode; 
 }

sub copyFile {
    my $self         = shift;
    my $inputfile    = shift;
    my $outputpath   = shift;

    my $time0 = time();
    $copyCalls++;
    my $cmd = "cp -pi -d -v $inputfile  $outputpath ";
    my $cmdstatus = system($cmd);
    if ($verbose == 1 && $webmode == 0)  {
      $self->amsprint("*** docopy - ",666);
      $self->amsprint($cmd,0);
    }
    my $time1 = time();

    $copyTime += $time1 - $time0;

    return $cmdstatus;
}

sub gethostname {
    my $self     = shift;
    my $hostname = shift;

    my $name     = $hostname;
 
    my @junk     = split '\.',$hostname;
    if ($#junk > 0) { $name = $junk[0];}

    return $name;
}

sub getRunningDays {

    my $self = shift;
    
    my $sql;
    my $ret;
    my $timestart = 0;
    my $lastupd   = 0;
    my $timepassed= 0;
    my $timenow   = time();
# first job timestamp
      $sql="SELECT MIN(Jobs.time), MAX(Jobs.timestamp) FROM Jobs, Cites 
                WHERE Jobs.cid=Cites.cid and Cites.name!='test'";
      $ret=$self->{sqlserver}->Query($sql);
      if (defined $ret->[0][0]) {
       $timestart = $ret->[0][0];
       $timepassed = ($timenow - $timestart)/60/60/24;
      }
      if (defined $ret->[0][1]) {
       $lastupd=$ret->[0][1];
      }


    return $timestart, $lastupd, $timepassed;
  }

sub printValidateStat {
   my $self      = shift;
   my $timenow = time();
   my $ltime   = localtime($timenow);


    my $vdir=$self->getValidationDir();
    if (not defined $vdir) {
        $vdir = "/tmp";
     }
    my $vlog = $vdir."/validateRunsSummary.log.".$timenow;   
    open(FILEV,">".$vlog) or die "Unable to open file $vlog\n";
    if ($webmode ==0  and $verbose ==1) { print "printParserStat -I- open $vlog \n";}

   print "\n\n\n";
   my $firstline = "------------- ValidateRuns ------------- \n";
   my $lastline  = "-------------     Thats It          ------------- \n";

   print $firstline;
   print FILEV $firstline;

   my $stime   = localtime($validateStartTime);
   my $hours   = ($timenow - $validateStartTime)/60/60;
   my $t0      = "Start Time : $stime \n";
   my $t1      = "End   Time : $ltime \n";
   print $t0,$t1;
   print   FILEV $t0,$t1;


      my $l0 = "   Runs (Listed, Completed, Bad, Failed) :  $CheckedRuns[0], $GoodRuns[0],  $BadRuns[0] \n";
      my $l1 = "   DSTs (Checked, Good, Bad, Crci, CopyFail/CRC ) :  ";
      my $l2 = "$CheckedDSTs[0],  $GoodDSTs[0], $BadDSTs[0], $BadCRCi[0], $BadDSTCopy[0] \n";
      print "\n ",$l0,$l1,$l2;
      print FILEV "\n",$l0,$l1,$l2;

 my $totalGB = $gbDST[0]/1000/1000/1000;
 my $chGB = sprintf("Total GB %3.1f \n",$totalGB);
 print $chGB;
 my $ch0 = sprintf("Total Time %3.1f hours \n",$hours);
 my $ch1 = sprintf(" doCopy (calls, time) : %5d %3.1fh [cp file :%5d %3.1fh]; \n",$doCopyCalls, $doCopyTime/60/60, $copyCalls, $copyTime/60/60);
 my $ch2 = sprintf(" CRC (calls,time) : %5d, %3.1fh ; Validate (calls,time) : %5d, %3.3fh \n",
                   $crcCalls, $crcTime/60/60,$fastntCalls, $fastntTime/60/60 );
 print $ch0,$ch1,$ch2,$lastline;
 print FILEV $chGB, $ch0, $ch1, $ch2, $lastline;
 close FILEV;
}

sub getDefByKey {
    my $self = shift;
    my $key  = shift;
    my $ret  = undef;

    my $sql="select myvalue from Environment where mykey='".$key."'";
    $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     $ret = $ret->[0][0];
    }

 return $ret;
}

sub getRunInfo {

 my $self = shift;


 my $HelpTxt = "
     validateRuns gets list of runs from production server 
                  validates DSTs and copies them to final destination
                  update Runs and NTuples DB tables
     -c    - output will be produced as ASCII page (default)
     -h    - print help
     -r    - run number
     -s    - get run info from server
     -v    - verbose mode
     ./getruninfo.cgi -c -v -r:102
";

 my $sql = undef;

 my $dbinfo  = 0;
 my $srvinfo = 1;
 my $Run     = -1;

   foreach my $chop  (@ARGV){
    if ($chop =~/^-c/) {
        $webmode = 0;
    }
    if ($chop =~/^-s/) {
        $srvinfo = 1;
    }
    if($chop =~/^-r:/){
       $Run=unpack("x3 A*",$chop);
    } 
    if ($chop =~/^-v/) {
        $verbose = 1;
    }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
   }

 if ($dbinfo == 0 && $srvinfo == 0) {
     print "getRunInfo -E- -d or -s flag should be defined \n";
     return 1;
 }

 if ($Run == -1) {
    print "getRunInfo -E- -r flag should be defined \n";
     return 1;
 }
 
    if( not $self->Init()){
        die "getRunInfo -F- Unable To Init";
        
    }
    if ($verbose) {print "getRunInfo -I- Connect to Server \n";}
    if (not $self->ServerConnect()){
        die "getRunInfo -F- Unable To Connect To Server";
    }
    if ($verbose) {print "getRunInfo -I- Connected \n";}
#
# get list of runs from Server
 if ($srvinfo == 1) {
    if( not defined $self->{dbserver}->{rtb}){
      DBServer::InitDBFile($self->{dbserver});
    }
  }
    print "Get Info Run ... $Run \n";
    foreach my $run (@{$self->{dbserver}->{rtb}}){
        if ($run->{Run} == $Run) {
            print "Run .....  $run->{Run} \n";
            print "Status...  $run->{Status} \n";
            print "Events...  $run->{cinfo}->{EventsProcessed} \n";
            print "Errors...  $run->{cinfo}->{CriticalErrorsFound} \n";
            print "CPUTime... $run->{cinfo}->{CPUTimeSpent} \n";
            print "Elapsed... $run->{cinfo}->{TimeSpent} \n";
            print "Host...    $run->{cinfo}->{HostName} \n";
            last; 
      }
    }
}


sub calculateMipsVC {

    my $self=shift;
{
        my $totalcpu=0;
        my $restcpu=0;
     my $dir="/afs/ams.cern.ch/AMSDataDir/DataManagement/DataSets";
     opendir THISDIR ,$dir or die "unable to open $dir";
     my @allfiles= readdir THISDIR;
     closedir THISDIR;    
    foreach my $file (@allfiles){
        my $newfile="$dir/$file";
       if($file =~/^\.Trial/){

           open(FILE,"<".$newfile) or die "Unable to open dataset control file $newfile \n";
           my $buf;
           read(FILE,$buf,16384);
           close FILE;
           $self->{TrialRun}=$buf;          
           last;
       }
    }
    foreach my $file (@allfiles){
        my $newfile="$dir/$file";
        if(readlink $newfile or  $file =~/^\./){
         next;
        }
       my @sta = stat $newfile;
      if($sta[2]<32000){
          my $dataset={};
          $dataset->{name}=$file;
          $dataset->{jobs}=[];
          my @tmpa;
       opendir THISDIR, $newfile or die "unable to open $newfile";
       my @jobs=readdir THISDIR;
       closedir THISDIR;
          push @{$self->{DataSetsT}}, $dataset;
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
           $template->{filename}=$job;
           my @sbuf=split "\n",$buf;
           my @farray=("TOTALEVENTS","PART","PMIN","PMAX","TIMBEG","TIMEND","CPUPEREVENTPERGHZ");
           foreach my $ent (@farray){
            foreach my $line (@sbuf){
               if($line =~/$ent=/){
                   my @junk=split "$ent=",$line;                 
                   $template->{$ent}=$junk[$#junk];
                   $buf=~ s/$ent=/C $ent=/;
                   last;
               }
            }         
        }
        if(defined $self->{TrialRun}){
            $template->{TOTALEVENTS}*=$self->{TrialRun};
        }
        $totalcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};
           $template->{initok}=1;
           foreach my $ent (@farray){
             if(not defined $template->{$ent}){
               $template->{initok}=undef;
             }
           }
#
# get no of events
#
              my $sql="select did from DataSets where name='$dataset->{name}'";
              my $ret=$self->{sqlserver}->Query($sql);
              if( defined $ret->[0][0]){
                 $dataset->{did}=$ret->[0][0];    
                 $sql="select jid,time,triggers,timeout from Jobs where did=$ret->[0][0] and jobname like '%$template->{filename}'";
                 my $r2= $self->{sqlserver}->Query($sql);
                 if(defined $r2->[0][0]){
                     foreach my $job (@{$r2}){
                         if ($job->[1]-time()>$job->[3]){
                             $sql="select FEvent,LEvent from Runs where jid=$job->[0] and status='Finished'";
                             my $r3=$self->{sqlserver}->Query($sql);
                             if(defined $r3->[0][0]){
                              foreach my $run (@{$r3}){
                               $template->{TOTALEVENTS}-=$run->[0];
                              } 
                             }
                         }
                         else {
#
# subtract allocated events
                             $template->{TOTALEVENTS}-=$job->[2];

                         }
                     }
                 }
             }
           else{
               $sql="select did from DataSets";
               $ret=$self->{sqlserver}->Query($sql);
               my $did=0;
               if(defined $ret->[0][0]){
                   foreach my $ds (@{$ret}){
                       if($ds->[0]>$did){
                           $did=$ds->[0];
                       }
                   }
               }
               $did++;
               $dataset->{did}=$did;
               my $timestamp = time();
             $sql="insert into DataSets values($did,'$dataset->{name}',$timestamp)";
             $self->{sqlserver}->Update($sql); 
           }
        $restcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};
          
           if($sbuf[0] =~/^#/ && defined $template->{initok}){

            $buf=~ s/#/C /;
            $template->{filebody}=$buf;
            my $desc=$sbuf[0];
            substr($desc,0,1)=" ";
            $template->{filedesc}=$desc." Total Events Left $template->{TOTALEVENTS}";
           if($template->{TOTALEVENTS}>100){
             push @tmpa, $template; 
         }
           }        
       }        
    }
      }
    }
warn "  total/rest  $totalcpu  $restcpu \n";
}
#datasets



    my @directories=(
     'aprotons',
     'deuterons',
     'electrons',
     'electrons.nomf',
     'gamma',
     'nuclei',
     'positrons',
     'protons',
     'C',
     'He');
     

    my $sql    = undef;
    my $jobname= undef;

    my @names;
    my @cpus;
    my @elapsed;
    my @events;
    my @mips;
    my @cpumean;
    my @elapsedmean;
    my @cpusigma;
    my @elapsedsigma;
    my @mipsmean;
    my @mipssigma;
    my @njobs;
    my @particle;

    my $n  = 0;
#
    my $timenow = time();
    my $ltime   = localtime($timenow);
#
    print "Start calculateMips : $ltime \n";
 
# get jobname and mips
#
    $sql = "SELECT 
              jobs.jobname, jobs.events, jobs.cputime, jobs.elapsed, jobs.mips, jobs.jid, 
              runs.fevent, runs.levent, ntuples.path     
              FROM Jobs, Runs, Ntuples  
              WHERE jobs.jid=runs.jid AND runs.status='Completed' AND jobs.mips>0 AND runs.run=ntuples.run  
              ORDER BY jid";
    my $r0 = $self->{sqlserver}->Query($sql);
    my $jidOld = 0;
    if (defined $r0->[0][0]) {
     foreach my $job (@{$r0}){
      $jobname=trimblanks($job->[0]);
      my $cite    = undef;
      my $jid     = undef;
      my $jobtype = undef;
      my $eventsj = undef;
      my $cpusj   = undef;
      my $elapsedj= undef;
      my $mipsj   = undef;
      my $fevent  = undef;
      my $levent  = undef;
      my $path    = undef;
      my $partj   = 'xyz';
      my $newjob  = 1;
      my @junk    = split '\.',$jobname;
      if ($#junk > 0) {
         $cite        = $junk[0];
         $jid         = $junk[1];
         if ($jid != $jidOld) { 
          $jobtype     = $junk[2].'.'.$junk[3].'.'.$junk[4];     
          $jobtype=trimblanks($jobtype);
          $eventsj     = $job->[1];
          $cpusj       = $job->[2];
          $elapsedj    = $job->[3];
          $mipsj       = $job->[4];
          $jid         = $job->[5];
          $fevent      = $job->[6];
          $levent      = $job->[7];
          $path        = $job->[8];

          $eventsj    = $levent - $fevent + 1;


          my $partj= $self->getJobParticleFromDSTPath($path);
          my $i        = 0;
          for ($i=0; $i<$#names+1; $i++) {
             if ($names[$i] eq $jobtype && $particle[$i] eq $partj) {
                 $events[$i] = $events[$i] + $eventsj;
                 $cpus[$i]   = $cpus[$i]   + $cpusj;
                 $elapsed[$i]= $elapsed[$i]+ $elapsedj;
                 $mips[$i]   = $mips[$i]   + $mipsj;
                 if ($eventsj > 0 && $mipsj > 0) {
                  $mipsmean[$i]     = $mipsmean[$i] + ($cpusj*1000/$eventsj)/$mipsj;
                 }
                 if ($njobs[$i] > 0) {$njobs[$i]++;}
                 $newjob  = 0;
                 last;
             }
          }
          if ($newjob == 1) {
                 $names[$n]     = $jobtype;
                 $events[$n]    = $eventsj;
                 $cpus[$n]      = $cpusj;
                 $elapsed[$n]   = $elapsedj;
                 $mips[$n]      = $mipsj;
                 $particle[$n]  = $partj;
                 $njobs[$n]  = 1;
                 if ($events[$n]>0 && $mipsj > 0) {
                  $mipsmean[$n]     = ($cpusj/($events[$n]))*1000/$mipsj;
                 }
                 $n++;
             }
      }
      $jidOld=$jid;
     }
  }
    my $totaljobs = 0;
    for (my $j=0; $j<$#names+1; $j++) {
     $mipsmean[$j]    = $mipsmean[$j]/$njobs[$j];
     $mipssigma[$j]   = 0;
 
     if ($events[$j] > 0) {
      if ($njobs[$j]>1) {
       $totaljobs = $totaljobs + $njobs[$j];
       foreach my $job (@{$r0}){
        $jobname=trimblanks($job->[0]);
        my @junk    = split '\.',$jobname;
        if ($#junk > 0) {
         my $jobtype     = $junk[2].'.'.$junk[3].'.'.$junk[4];     
         $jobtype=trimblanks($jobtype);
         my $path = $job->[8];
         my $partj= $self->getJobParticleFromDSTPath($path);
         if ($jobtype eq $names[$j] && $partj eq $particle[$j]) {
           my $eventsj     = $job->[1];
           my $cpusj       = $job->[2];
           my $elapsedj    = $job->[3];
           my $mipsj       = $job->[4];
           my $jid         = $job->[5];
           my $fevent      = $job->[6];
           my $levent      = $job->[7];
           $eventsj        = $levent - $fevent + 1;
           if ($eventsj > 0 && $mipsj >0) {
               $mipssigma[$j]    = ($mipsmean[$j] - ($cpusj*1000/$eventsj)/$mipsj)**2;
           }
       }
     }
    }
   }
  }
 }
 
    my $header =  sprintf("Jobs     Events     CPU[s]   Elapsed[s]   MIPS       <MIPS>       Particle        JobName\n");
    print "$header";
        my $totalcpu=0;
        my $restcpu=0;
    foreach my $p (@directories) {



{
     my $dir="/afs/ams.cern.ch/AMSDataDir/DataManagement/DataSets";
#     opendir THISDIR ,$dir or die "unable to open $dir";
#     my @allfiles= readdir THISDIR;
#     closedir THISDIR;    
    my $file=$p;
#    foreach my $file (@allfiles){
#        my $newfile="$dir/$file";
#       if($file =~/^\.Trial/){
#
#           open(FILE,"<".$newfile) or die "Unable to open dataset control file $newfile \n";
#           my $buf;
#           read(FILE,$buf,16384);
#           close FILE;
#           $self->{TrialRun}=$buf;          
#           last;
#       }
#}
#    foreach my $file (@allfiles){
     my $tcpu=0;
     my $rcpu=0;
        my $newfile="$dir/$file";
        if(readlink $newfile or  $file =~/^\./){
         next;
        }
       my @sta = stat $newfile;
      if($sta[2]<32000){
          my $dataset={};
          $dataset->{name}=$file;
          $dataset->{jobs}=[];
          my @tmpa;
       opendir THISDIR, $newfile or die "unable to open $newfile";
       my @jobs=readdir THISDIR;
       closedir THISDIR;
          push @{$self->{DataSetsT}}, $dataset;
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
        $template->{bufi}=$buf;
        $template->{full}=$full;
           close FILE;
           $template->{filename}=$job;
           my @sbuf=split "\n",$buf;
           my @farray=("TOTALEVENTS","PART","PMIN","PMAX","TIMBEG","TIMEND","CPUPEREVENTPERGHZ");
           foreach my $ent (@farray){
            foreach my $line (@sbuf){
               if($line =~/$ent=/){
                   my @junk=split "$ent=",$line;                 
                   $template->{$ent}=$junk[$#junk];
                   $buf=~ s/$ent=/C $ent=/;
                   last;
               }
            }         
        }
        if(defined $self->{TrialRun}){
            $template->{TOTALEVENTS}*=$self->{TrialRun};
        }
        my $fac=$template->{CPUPEREVENTPERGHZ};
     for (my $j=0; $j<$#names+1; $j++) {
         my $entry =$names[$j];
         if ($particle[$j] eq $p) { 
             $entry=$entry.".job";
            if($template->{filename} eq $entry and $mipsmean[$j]>0){
                if($fac/$mipsmean[$j]<0.8 or $fac/$mipsmean[$j]>1.25){
                    my $newfac=int($mipsmean[$j]*1000+0.5)/1000;
                 warn " ***change*** fac $fac $newfac $entry $p \n";
           open(FILE,">".$full) or die "Unable to open dataset file $full \n";
                 $template->{bufi}=~ s/CPUPEREVENTPERGHZ=$fac/CPUPEREVENTPERGHZ=$newfac/;
                 print FILE $template->{bufi};
           close(FILE)
                }                 
              $fac=$mipsmean[$j];
                $template->{CPUPEREVENTPERGHZ}=$fac;
                last;
            }
        }
     }
        $totalcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};
        $tcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};
           $template->{initok}=1;
           foreach my $ent (@farray){
             if(not defined $template->{$ent}){
               $template->{initok}=undef;
             }
           }
#
# get no of events
#
              my $sql="select did from DataSets where name='$dataset->{name}'";
              my $ret=$self->{sqlserver}->Query($sql);
              if( defined $ret->[0][0]){
                 $dataset->{did}=$ret->[0][0];    
                 $sql="select jid,time,triggers,timeout from Jobs where did=$ret->[0][0] and jobname like '%$template->{filename}'";
                 my $r2= $self->{sqlserver}->Query($sql);
                 if(defined $r2->[0][0]){
                     foreach my $job (@{$r2}){
                         if ($job->[1]-time()>$job->[3]){
                             $sql="select FEvent,LEvent from Runs where jid=$job->[0] and status='Finished'";
                             my $r3=$self->{sqlserver}->Query($sql);
                             if(defined $r3->[0][0]){
                              foreach my $run (@{$r3}){
                               $template->{TOTALEVENTS}-=$run->[0];
                              } 
                             }
                         }
                         else {
#
# subtract allocated events
                             $template->{TOTALEVENTS}-=$job->[2];

                         }
                     }
                 }
             }
           else{
               $sql="select did from DataSets";
               $ret=$self->{sqlserver}->Query($sql);
               my $did=0;
               if(defined $ret->[0][0]){
                   foreach my $ds (@{$ret}){
                       if($ds->[0]>$did){
                           $did=$ds->[0];
                       }
                   }
               }
               $did++;
               $dataset->{did}=$did;
               my $timestamp = time();
             $sql="insert into DataSets values($did,'$dataset->{name}',$timestamp)";
             $self->{sqlserver}->Update($sql); 
           }
        $restcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};
        $rcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};
          
           if($sbuf[0] =~/^#/ && defined $template->{initok}){

            $buf=~ s/#/C /;
            $template->{filebody}=$buf;
            my $desc=$sbuf[0];
            substr($desc,0,1)=" ";
            $template->{filedesc}=$desc." Total Events Left $template->{TOTALEVENTS}";
           if($template->{TOTALEVENTS}>100){
             push @tmpa, $template; 
         }
           }        
       }        
    }
      }
#    }
     my $res=int(($tcpu-$rcpu)/$tcpu*100);
      print "  total/rest  $totalcpu  $restcpu done:  $res  $p \n";
    }




     for (my $j=0; $j<$#names+1; $j++) {
      if ($particle[$j] eq $p) { 
       if ($events[$j] > 0 && $njobs[$j]>1) {
         $mipssigma[$j] = sqrt($mipssigma[$j])/($njobs[$j]-1);
       }
       my $line = sprintf("%5.f %11.f %9.f %9.f %9.f %6.2f +/- %2.4f %12s  %20s \n",
                        $njobs[$j],$events[$j],$cpus[$j],$elapsed[$j],$mips[$j],$mipsmean[$j],$mipssigma[$j],$particle[$j],$names[$j]);
       print "$line";
    }
  }
  }
     print "\n Total Jobs : $totaljobs \n";
 }
}

sub getEventsLeft {
 my $self = shift;


 my $HelpTxt = "
   getTotalEvents reads TOTALEVENTS from templates files in AMSSoftwareDir/DataSets
                  query DB for corresponding finished jobs
                  prints statistics
     -c    - output will be produced as ASCII page (default)
     -h    - print help
     -v    - verbose mode
     ./getruninfo.cgi -c -v -p:He
";

 my $sql = undef;

 my $totalprocessed = 0; # processed events | TRIGGERS
 my $totalrequested = 0; # requested events |
 my $totalevents    = 0; #
 my $TotalTotal     = 0; # TRIGGERS for all jobs
 my $TotalProcessed = 0; #                       processed
 my $TotalDataset   = 0; # TRIGGERS per dataset
 my $TotalDatasetP  = 0; #                        processed
 my $TotalDatasetC  = 0; # CPU per dataset
 my $TotalDatasetM  = 0; # will be needed CPU per dataset
 my $TotalMore      = 0; #                    for all datasets
 my $TotalCPU       = 0; # CPU spent for all datasets
 my $totalcpu       = 0; # CPU per job type
 my $totalmore      = 0; # CPU needed for job type
 my $cpuperevent    = 0; # CPU per event for particular job type

 my $dataset={};


   foreach my $chop  (@ARGV){
    if ($chop =~/^-c/) {
        $webmode = 0;
    }
    if ($chop =~/^-v/) {
        $verbose = 1;
    }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
   }


    if ( not $self->Init()) {die "getTotalEvents -F- Unable To Init";}
    if ($verbose)  {print "getTotalEvents -I- Connect to Server \n";}
    if (not $self->ServerConnect()){
        die "getTotalEvents -F- Unable To Connect To Server";}

    if ($verbose) {print "getTotalEvents -I- Connected \n";}

# get names from /afs/ams.cern.ch/AMSDataDir/DataManagement/
    my $dir="$self->{AMSSoftwareDir}/DataSets";
    opendir THISDIR ,$dir or die "unable to open $dir";
    my @allfiles= readdir THISDIR;
    closedir THISDIR;    
#     if ($verbose) {print "Directory... $dir \n";}
 
   foreach my $file (@allfiles){
       $totalevents    = 0;
       $totalrequested = 0;
       $totalprocessed = 0;
       $totalcpu       = 0;
       $totalmore      = 0;

       my $newfile="$dir/$file";
        if(readlink $newfile or  $file =~/^\./){
         next;
        }
       my @sta = stat $newfile;
      if($sta[2]<32000){
       my $line0 = sprintf("%15s %15s %10s %7s %8s %14s %16s %20s\n",
                           "Total","Processed","% Events","CPU","more CPU","CPU per Event","Job Name","DataSet");
       my $line1 = sprintf("%15s %15s %8s %10s %6s %14s %12s %20s\n",
                           "Events","Events","done","days","days","calc  estim"," "," ");
       print "$line0";
       print "$line1";
          $dataset={};
          $dataset->{name}=$file;
          $dataset->{jobs}=[];
          $TotalDataset   = 0;
          $TotalDatasetP  = 0;
          $TotalDatasetC  = 0;
          $TotalDatasetM  = 0;
#       if ($verbose) {print "Directory... $newfile \n";}
       opendir THISDIR, $newfile or die "unable to open $newfile";
       my @jobs=readdir THISDIR;
       closedir THISDIR;
       foreach my $job (@jobs){
       $totalcpu       = 0;
       $totalmore      = 0;
        if($job =~ /\.job$/){
        if($job =~ /^\./){
            next;
        }
           my $template={};
           my $full="$newfile/$job";
           my $buf;
#           print "$file \n";
           open(FILE,"<".$full) or die "Unable to open dataset file $full \n";
           read(FILE,$buf,1638400) or next;
           close FILE;
           my @sbuf=split "\n",$buf;
           my @ent = ("TOTALEVENTS","CPUPEREVENTPERGHZ");
           foreach my $e (@ent) {
            foreach my $line (@sbuf){
               if($line =~/$e=/){
                   my @junk=split "$e=",$line;                 
                   if ($e eq "TOTALEVENTS") {$totalevents=$junk[$#junk];}
                   if ($e eq "CPUPEREVENTPERGHZ") {$cpuperevent = $junk[$#junk]};
                   last;
               }
            }         
        }
#
# get no of events
#
              my $sql="SELECT did FROM DataSets WHERE name='$dataset->{name}'";
              my $ret=$self->{sqlserver}->Query($sql);
              if( defined $ret->[0][0]){
                 my $did=$ret->[0][0];    
                 my $secperevent = $self->getMips($did,$job);
                 if ($secperevent == 0) {$secperevent = $cpuperevent;}
                 $sql = "SELECT SUM(triggers)   FROM Jobs, Runs, Cites  
                  WHERE 
                    Jobs.did  = $did AND 
                    Jobs.jobname like '%$job' AND 
                    Jobs.jid = Runs.jid AND  
                    (Runs.status='Completed' OR Runs.status='Finished') AND 
                    (Jobs.cid != Cites.cid AND
                      Cites.cid = (SELECT Cites.cid FROM Cites WHERE Cites.name = 'test'))";
#                 print "$sql \n";
                 my $r2= $self->{sqlserver}->Query($sql);
                 $totalprocessed = 0;
                 if(defined $r2->[0][0]){
                     $totalprocessed = $r2->[0][0];
                 }
                 my $perc = 0;
                 $totalmore = 0;
                 if ($totalevents != 0) {
                  $perc = sprintf("%2.1f",($totalprocessed*100/$totalevents));
                 }
                 $totalcpu         = $totalprocessed*$secperevent/24/60/60;
                 $totalmore        = ($totalevents - $totalprocessed)*$secperevent/24/60/60;
                 $TotalDataset    += $totalevents;
                 $TotalDatasetP   += $totalprocessed;
                 $TotalTotal      += $totalevents;
                 $TotalProcessed  += $totalprocessed;
                 $TotalCPU        += $totalcpu;
                 $TotalMore       += $totalmore;
                 $TotalDatasetC   += $totalcpu;
                 $TotalDatasetM   += $totalmore;

                 my $line = sprintf("%15d %15d %8s %10.1f %6.1f %8.3f %4.3f %25s %12s\n",
                                    $totalevents,$totalprocessed,$perc, $totalcpu, $totalmore,
                                    $secperevent,$cpuperevent,$job,$dataset->{name});
                 print "$line";
             }
    }
          
    }           
   }
   my $perc = 0;
    if ($TotalDataset != 0) {
    $perc = sprintf("%2.1f",($TotalDatasetP*100/$TotalDataset));
    }
       my $cpud = sprintf("%6.1f",$TotalDatasetC);
       my $cpum = sprintf("%6.1f",$TotalDatasetM);
       my $perccpu = sprintf("%3.1f",($cpud*100/($cpum+$cpud)));
       print "$dataset->{name} : Events : $TotalDataset , Processed : $TotalDatasetP , % Events : $perc, CPU : $cpud , $cpum, % CPU : $perccpu \n\n";
   }
   
  my $perc = 0;
  if ($TotalTotal != 0) {
   $perc = sprintf("%2.1f",($TotalProcessed*100/$TotalTotal));
  }

  my $cpud = sprintf("%6.1f",$TotalCPU);
  my $cpum = sprintf("%6.1f",$TotalMore);
  my $perccpu = sprintf("%3.1f",($TotalCPU*100/($TotalMore+$TotalCPU)));
  print "Total Events : $TotalTotal, Processed : $TotalProcessed, % Events : $perc\n";
  print "CPU : $cpud days,  CPU more  : $cpum CPU days, % CPU : $perccpu \n";
  
}


sub calculateMips {

    my @directories=(
     'aprotons',
     'deuterons',
     'electrons',
     'electrons.nomf',
     'gamma',
     'nuclei',
     'positrons',
     'protons',
     'C',
     'He');
     
    my $self   = shift;

    my $sql    = undef;
    my $jobname= undef;

    my @names;
    my @cpus;
    my @elapsed;
    my @events;
    my @mips;
    my @cpumean;
    my @elapsedmean;
    my @cpusigma;
    my @elapsedsigma;
    my @mipsmean;
    my @mipssigma;
    my @njobs;
    my @particle;

    my $n  = 0;
#
    my $timenow = time();
    my $ltime   = localtime($timenow);
#
    print "Start calculateMips : $ltime \n";
 
# get jobname and mips
#
    $sql = "SELECT 
              jobs.jobname, jobs.events, jobs.cputime, jobs.elapsed, jobs.mips, jobs.jid, 
              runs.fevent, runs.levent, ntuples.path     
              FROM Jobs, Runs, Ntuples  
              WHERE jobs.jid=runs.jid AND runs.status='Completed' AND jobs.mips>0 AND runs.run=ntuples.run  
              ORDER BY jid";
    my $r0 = $self->{sqlserver}->Query($sql);
    my $jidOld = 0;
    if (defined $r0->[0][0]) {
     foreach my $job (@{$r0}){
      $jobname=trimblanks($job->[0]);
      my $cite    = undef;
      my $jid     = undef;
      my $jobtype = undef;
      my $eventsj = undef;
      my $cpusj   = undef;
      my $elapsedj= undef;
      my $mipsj   = undef;
      my $fevent  = undef;
      my $levent  = undef;
      my $path    = undef;
      my $partj   = 'xyz';
      my $newjob  = 1;
      my @junk    = split '\.',$jobname;
      if ($#junk > 0) {
         $cite        = $junk[0];
         $jid         = $junk[1];
         if ($jid != $jidOld) { 
          $jobtype     = $junk[2].'.'.$junk[3].'.'.$junk[4];     
          $jobtype=trimblanks($jobtype);
          $eventsj     = $job->[1];
          $cpusj       = $job->[2];
          $elapsedj    = $job->[3];
          $mipsj       = $job->[4];
          $jid         = $job->[5];
          $fevent      = $job->[6];
          $levent      = $job->[7];
          $path        = $job->[8];

          $eventsj    = $levent - $fevent + 1;


          my $partj= $self->getJobParticleFromDSTPath($path);
          my $i        = 0;
          for ($i=0; $i<$#names+1; $i++) {
             if ($names[$i] eq $jobtype && $particle[$i] eq $partj) {
                 $events[$i] = $events[$i] + $eventsj;
                 $cpus[$i]   = $cpus[$i]   + $cpusj;
                 $elapsed[$i]= $elapsed[$i]+ $elapsedj;
                 $mips[$i]   = $mips[$i]   + $mipsj;
                 if ($eventsj > 0 && $mipsj > 0) {
                  $mipsmean[$i]     = $mipsmean[$i] + ($cpusj*1000/$eventsj)/$mipsj;
                 }
                 if ($njobs[$i] > 0) {$njobs[$i]++;}
                 $newjob  = 0;
                 last;
             }
          }
          if ($newjob == 1) {
                 $names[$n]     = $jobtype;
                 $events[$n]    = $eventsj;
                 $cpus[$n]      = $cpusj;
                 $elapsed[$n]   = $elapsedj;
                 $mips[$n]      = $mipsj;
                 $particle[$n]  = $partj;
                 $njobs[$n]  = 1;
                 if ($events[$n]>0 && $mipsj > 0) {
                  $mipsmean[$n]     = ($cpusj/($events[$n]))*1000/$mipsj;
                 }
                 $n++;
             }
      }
      $jidOld=$jid;
     }
  }
    my $totaljobs = 0;
    for (my $j=0; $j<$#names+1; $j++) {
     $mipsmean[$j]    = $mipsmean[$j]/$njobs[$j];
     $mipssigma[$j]   = 0;
 
     if ($events[$j] > 0) {
      if ($njobs[$j]>1) {
       $totaljobs = $totaljobs + $njobs[$j];
       foreach my $job (@{$r0}){
        $jobname=trimblanks($job->[0]);
        my @junk    = split '\.',$jobname;
        if ($#junk > 0) {
         my $jobtype     = $junk[2].'.'.$junk[3].'.'.$junk[4];     
         $jobtype=trimblanks($jobtype);
         my $path = $job->[8];
         my $partj= $self->getJobParticleFromDSTPath($path);
         if ($jobtype eq $names[$j] && $partj eq $particle[$j]) {
           my $eventsj     = $job->[1];
           my $cpusj       = $job->[2];
           my $elapsedj    = $job->[3];
           my $mipsj       = $job->[4];
           my $jid         = $job->[5];
           my $fevent      = $job->[6];
           my $levent      = $job->[7];
           $eventsj        = $levent - $fevent + 1;
           if ($eventsj > 0 && $mipsj >0) {
               $mipssigma[$j]    = ($mipsmean[$j] - ($cpusj*1000/$eventsj)/$mipsj)**2;
           }
       }
     }
    }
   }
  }
 }
 
    my $header =  sprintf("Jobs     Events     CPU[s]   Elapsed[s]   MIPS       <MIPS>       Particle        JobName\n");
    print "$header";
    foreach my $p (@directories) {
     for (my $j=0; $j<$#names+1; $j++) {
      if ($particle[$j] eq $p) { 
       if ($events[$j] > 0 && $njobs[$j]>1) {
         $mipssigma[$j] = sqrt($mipssigma[$j])/($njobs[$j]-1);
       }
       my $line = sprintf("%5.f %11.f %9.f %9.f %9.f %6.2f +/- %2.4f %12s  %20s \n",
                        $njobs[$j],$events[$j],$cpus[$j],$elapsed[$j],$mips[$j],$mipsmean[$j],$mipssigma[$j],$particle[$j],$names[$j]);
       print "$line";
    }
  }
  }
     print "\n Total Jobs : $totaljobs \n";
 }
}

sub getMips {

    my $self    = shift;
    my $did     = shift;
    my $dataset = shift;

    my $events = 0;
    my $cpu    = 0;
    my $elapsed= 0;
    my $mips   = 0;
    my $mipsmean = 0;
    my $njobs    = 0;

 
    my $sql = "SELECT jobs.triggers, jobs.cputime, jobs.mips
              FROM Jobs, Runs
              WHERE jobs.did = $did AND jobs.jobname like '%$dataset' AND 
                    jobs.jid=runs.jid AND 
                    runs.status='Completed' AND jobs.mips>0 ";
    my $r0 = $self->{sqlserver}->Query($sql);
    if (defined $r0->[0][0]) {
     foreach my $job (@{$r0}){
         if ($job->[0] > 0 && $job->[1] > 0 && $job->[2] > 0) {
         $events = $job->[0];
         $cpu    = $job->[1];
         $mips   = $job->[2];
         $njobs++;
         $mipsmean += $cpu*$mips/1000/$events;
       }
     }
    }
    if ($njobs > 0) {$mipsmean = $mipsmean/$njobs;}

    return $mipsmean;
}

sub updateCopyStatus {

#
# v1.0 :MC DST copy update Data Base 
# Features: Update CERN data base with 
# copied DST to other AMS site (AMS02 MC simulation)
#
# May 2004.
# Authors  Jean Jacquemier & Alexei Klimentov
#
#


my $HelpTxt = "
     updateCopyStatus updates information about DST's copied to 
                      from CERN 
     -c    - cite nickname, like in  ~MC/scratch directory tree
     -d    - debug mode
     -f    - full path to ASCII file
     -h    - print help
     -r    - run number
     -t    - unixtimestamp (default:curent time)
     -u    - update DB (default noUpdate)
     -v    - verbose mode
     perl updateCopyStatus.pl -c:lyon -r:402658148 -t:1085672830
     perl  updateCopyStatus.pl -f:/f2users/scratch/MC/lyon/local/cp.last -c:lyon
";

   my $self       = shift;

   my $cite      = undef;
   my $run       = undef;
   my $filename  = undef;
   my $timestamp = undef;

   my $debug     = 0;
   my $update    = 0;

  foreach my $chop  (@ARGV){
    if ($chop =~/^-c:/) {
      $cite=unpack("x3 A*",$chop);
    }
    if ($chop =~/^-f:/) {
      $filename=unpack("x3 A*",$chop);
    }
    if ($chop =~/^-r:/) {
      $run=unpack("x3 A*",$chop);
    }
    if ($chop =~/^-t:/) {
      $timestamp=unpack("x3 A*",$chop);
    }
    if ($chop =~/^-u/) {
        $update = 1;
    }
    if ($chop =~/^-v/) {
        $verbose = 1;
    }

    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
}


if (not defined $cite) {
    print "updateCopyStatus -Error- Cite is not defined not defined. Exit \n";
    return 1;
} else {
    if ($verbose) {print "Cite ... $cite \n";}
}

if (not defined $filename && not defined $run) {
    print "updateCopyStatus -Error- File name or/and Run number are not defined. Exit \n";
    return 1;
} else {
    if ($filename) {
     if ($verbose) {print "File... $filename \n";}
     my $filesize    = (stat($filename))[7];
     if ($filesize < 1) {
         print "updateCopyStatus -Error- File is not accessible or have 0 length. Exit \n";
         return 1;
     }
    } else {
         if ($run && $verbose) {
          print "Run ... $run \n";
         }
     }
}

if (not defined $timestamp) {
    $timestamp = time();
 }
if ($verbose) { 
 print "Unix time ... $timestamp \n";

 if ($update) {
    print "DB will be updated \n";
 } else {
    print "----- NO DB UPDATE --------------\n";
 }
}   

if (defined $filename) {
 open(HISTORY,"$filename");
 while(<HISTORY>)
 {
     my $line = $_;
     my @junk=split(",",$line);
     if ($#junk > 2) {
      my $run_num     = trimblanks($junk[0]);
      my $site_prefix = trimblanks($junk[1]);
      my $file_path   = trimblanks($junk[2]);
      my $unixtime    = trimblanks($junk[3]);


        if ($verbose) {
	 print "Cite, Run, Time... $cite, $run_num, $unixtime \n";
         print "File... $site_prefix, $file_path   \n";
        }
        if ($update ==1) {
 	 $self->updateMCDSTCopy($run_num,$cite,$site_prefix,$file_path,$unixtime);
        }
  } else {
      print "**** -W- Cannot parse : $line \n";
  }
 }
 close HISTORY;
} else {
    if (defined $run) {
        $self->updateMCDSTCopyRun($run,$cite,$timestamp);
    }
}

 return 1;
}

sub updateMCDSTCopyRun {
    my $self = shift;
    my $run  = shift;
    my $cite = shift;
    my $unixtime  = shift;

    $cite     = trimblanks($cite);
    my $timestamp = time();

    my $sql = "UPDATE MC_DST_Copy SET copytime=$unixtime, timestamp=$timestamp 
               WHERE run=$run and cite='$cite'";
     $self->{sqlserver}->Update($sql);

    return 1;
}

sub updateMCDSTCopy {
    my $self = shift;
    my $run  = shift;
    my $cite = shift;
    my $cite_prefix = shift;
    my $filepath = shift;
    my $unixtime  = shift;

    $cite     = trimblanks($cite);
    $filepath = trimblanks($filepath);

    my $sql = "SELECT path FROM NTUPLES WHERE PATH like '%$filepath'";
    my $ret=$self->{sqlserver}->Query($sql);
 
    if( defined $ret->[0][0]){
     $sql = "DELETE MC_DST_Copy WHERE PATH='$filepath' AND CITE='$cite'";
     $self->{sqlserver}->Update($sql);

     my $timestamp = time();

     $sql = "INSERT INTO MC_DST_Copy VALUES($run,
                                            '$filepath',
                                            '$cite_prefix',
                                            $unixtime,
                                            '$cite',
                                            $timestamp)";
    $self->{sqlserver}->Update($sql); 
  } else {
     print "updateRemoteCopy -W- could not find DST with path : $filepath \n";
     print "updateRemoteCopy -W- NO DB UPDATE \n";
  }
 return 1;
}

sub deleteDST {
 my $HelpTxt = "
     deleteDST deletes files from online disks
               marks filepath as filepath
               updates timestamp in Ntuples table
     -h    - print help
     -d    - dirpath (-d:directory) - mandatory
     -v    - verbose mode
     -u    - update mode

     ./deleteDST.cgi -v -u -d:/s0dah1/MC/AMS02/2004A/C/c.pl1.12005366
";

  my $self         = shift;
  my $topdir       = undef;
  my $updateDB     = 0;
  my $sql          = undef;
  my $nupd         = 0;

  my $whoami = getlogin();
  if ($whoami =~ 'ams') {
  } else {
   print  "deleteDST -ERROR- script cannot be run from account : $whoami \n";
   return 1;
  }

  foreach my $chop  (@ARGV){
    if($chop =~/^-d:/){
       $topdir=unpack("x3 A*",$chop);
    } 

    if ($chop =~/^-u/) {
     $updateDB = 1;
    }
    if ($chop =~/^-v/) {
     $verbose = 1;
    }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
   }

  
   if (not defined $topdir) {
       print "deleteDST -E- -d option is mandatory. Quit.\n";
   }

 my $timenow = time();
 if ($updateDB) {
  my $vdir=$self->getValidationDir();
  if (not defined $vdir) {
   die " deleteDST -F- cannot get path to ValidationDir \n";
  }
  my $vlog = $vdir."/deleteDST.log.".$timenow;   
  open(FILE,">".$vlog) or die "Unable to open file $vlog\n";
}
#


   find (\&inputList, $topdir);
    my $i= 0;
    while ($i <= $#inputFiles) {
      if (-d $inputFiles[$i]) {
          if ($verbose == 1) {print "Directory : $inputFiles[$i] \n";}
      } else {
       my $n = 0; # files found
       my $filename = trimblanks($inputFiles[$i]);
       $sql = "SELECT COUNT(PATH) FROM ntuples WHERE PATH LIKE '$filename'";
       my $ret=$self->{sqlserver}->Query($sql);
       if (not defined $ret->[0]) {
           print "File : $filename \n";
           print "$sql \n";
           print "N O T     F O U N D   I N     D B \n";
           print "correct it and rerun. Bye \n";
           return 1;
       } else {
         $n = $ret->[0][0];
         if ($n > 1) {
           print "File : $filename \n";
           print "$sql \n";
           print "F O U N D   I N     D B, $n times \n";
           print "correct it and rerun. Bye \n";
           return 1;
         }
       }
       my $newpath = "# ".$filename;
       $sql = "UPDATE ntuples SET PATH='$newpath', timestamp=$timenow WHERE PATH LIKE '$filename'";
       if ($verbose) {
           print "$sql \n";
       }
       if ($updateDB) {
         $self->{sqlserver}->Update($sql);
         print FILE "$sql \n";         
         $nupd++;
       }         
   }
      $i++;
  }
 if ($updateDB == 1) {
  print "deleteDST - All DST paths like $topdir are marked (total files $nupd) \n";
  print "deleteDST - you can do :  rm -r $topdir \n";
  print FILE "deleteDST - you can do :  rm -r $topdir \n";
  close FILE;
 } else {
     print "deleteDST - you run in NO UPDATE mode \n";
     print "deleteDST - to update DB run with -u \n";
 }

}

sub prepareCastorCopyScript {
 my $HelpTxt = "
     restoreDST updates DB content for files restored from CASTOR

     -h    - print help
     -a    - overwrite existing files
     -d    - directory path. -d:dir
     -o    - output file. -o:/tmp/c.rfcp
     -p    - pattern to search in DB 
     -v    - verbose mode

     ./preparecastorscript.cgi -v -p:/s0dah1/MC/AMS02/2004A/C/c.pl1.12005366 -d:/f0dah1/MC/AMS02/2004 -o:/tmp/t.t
";

  my $self         = shift;
  my $sql          = undef;
  my $pattern      = undef;
  my $outputfile   = undef;
  my $topdir       = undef;
  my $overwrite    = 0;
  my @mkdirs       = ();

  my $whoami = getlogin();
  if ($whoami =~ 'casadmva' || $whoami =~ 'ams') {
  } else {
   print  "restoreDST -ERROR- script cannot be run from account : $whoami \n";
   return 1;
  }

  foreach my $chop  (@ARGV){

    if ($chop =~/^-a/) {
     $overwrite = 1;
    }

    if($chop =~/^-o:/){
       $outputfile=unpack("x3 A*",$chop);
    } 

    if($chop =~/^-d:/){
       $topdir=unpack("x3 A*",$chop);
    } 

    if($chop =~/^-p:/){
       $pattern=unpack("x3 A*",$chop);
    } 

    if ($chop =~/^-v/) {
     $verbose = 1;
    }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
   }

  
   if (not defined $pattern) {print "ERROR - -p option is mandatory. Quit.\n"; return 1;}
   if (not defined $outputfile) {print "ERROR - -o option is mandatory. Quit.\n"; return 1;}
   if (not defined $topdir) {print "ERROR - -d option is mandatory. Quit.\n"; return 1;}

   my $writetime = (stat($topdir)) [9];
   if (not defined $writetime) {
       print "ERROR - cannot access $topdir \n";
   }

   my $timenow = time();
   
   $pattern = "# ".$pattern;
   $sql = "SELECT path FROM ntuples WHERE path like '$pattern%'";
   my $ret=$self->{sqlserver}->Query($sql);
   if (defined $ret->[0][0]) {
    open(FILEV,">".$outputfile) or die "Unable to open file $outputfile\n";
    if ($verbose ==1) { print "INFO- open $outputfile \n";}
    foreach my $f (@{$ret}){
     my @junk = split '/',$f->[0];
     my $castorpath  = "/castor/cern.ch/ams";
     my $dirpath     = $topdir;
     my $filepath    = $topdir;
     my $rfcp        = "#... \n";
     for (my $i=2; $i<$#junk+1; $i++) {
      $castorpath = $castorpath."/".$junk[$i];
      $filepath   = $filepath."/".$junk[$i];
      if ($i < $#junk) {$dirpath    = $dirpath."/".$junk[$i];}
     }
     my $writetime = 0;
     $writetime = (stat($dirpath)) [9] or $writetime=0;
     if ($writetime == 0) {
      my $mkdir = "/bin/mkdir -p $dirpath \n";
      my $direxist = 0;
      for (my $j=0; $j<$#mkdirs+1; $j++) {
          if ($mkdirs[$j] =~ $mkdir) { $direxist=1; last;}
      }
      if ($direxist ==0) {print FILEV "#... \n"; print FILEV $mkdir; push @mkdirs, $mkdir;}
      }
      my $filetime = 0;
      $filetime = (stat($filepath)) [9] or $filetime = 0;
      if ($filetime == 0) {
        $rfcp = "/usr/local/bin/rfcp ".$castorpath." ". $dirpath." \n";
      } else {
       if ($overwrite) {
        $rfcp = "/usr/local/bin/rfcp ".$castorpath." ". $dirpath." \n";
       } else {
         $rfcp = "# File exist. Do nothing; $filepath.\n";
       }
      }
    print FILEV $rfcp;
   }
   close FILEV; 
  } else {
    print "$sql \n";
    print "INFO - cannot find any record \n";
    return 1;
   }
 return 1;
}



sub updateDSTPath {
 my $HelpTxt = "
     scan directory and update DST path in NTuples table

     -h    - print help
     -crc  - calculate CRC and update path only if CRC is the same
     -d    - directory path. -d:dir
     -v    - verbose mode
     -u    - update mode

     ./updateDSTPath.cgi -v -d:/d0dah1/MC/AMS02/2004 
";

  my $self         = shift;
  my $sql          = undef;
 
  my $checkCRC     = 0;
  my $topdir       = undef;
  my $update       = 0;

  my @ErrFileSize;
  my @ErrCRC;
  my @ErrNotFound; 
  my @ErrMultFound;

  my $whoami = getlogin();
  if ($whoami =~ 'ams') {
  } else {
   print  "restoreDST -ERROR- script cannot be run from account : $whoami \n";
   return 1;
  }

  foreach my $chop  (@ARGV){

    if ($chop =~/^-crc/) {$checkCRC = 1;}

    if($chop =~/^-d:/){$topdir=unpack("x3 A*",$chop);}

    if ($chop =~/^-v/) {$verbose = 1;}

    if ($chop =~/^-u/) {$update = 1;}

    if ($chop =~/^-h/) {print "$HelpTxt \n";return 1;}
   }

   if (not defined $topdir) {print "ERROR - -d option is mandatory. Quit.\n"; return 1;}

   my $timenow = time();

   my $ncrcerr = 0;  # CRC errors
   my $nsizemm = 0;  # file size mismatch
   my $notfound= 0;  # file not found in DB
   my $foundtwice=0; # more than 1 record found in DB

    my $i= 0;
    my $cdir = $topdir;

    find (\&inputList, $topdir);
    while ($i <= $#inputFiles) {
      my $skipfile = 0;
      if (-d $inputFiles[$i]) {
          if ($verbose == 1) {print "Directory : $inputFiles[$i] \n";}
          $cdir = $inputFiles[$i];
      } else {
       my $n = 0;
       my $filename = trimblanks($inputFiles[$i]);
       my $dbpath = undef;
       my @junk = split ("AMS02",$filename);
       if ($#junk > 0) {
           $dbpath = $junk[1];
       }

       if (defined $dbpath) {
        $sql = "SELECT COUNT(PATH) FROM ntuples WHERE PATH LIKE '%$dbpath'";
        my $ret=$self->{sqlserver}->Query($sql);
        if (not defined $ret->[0][0]) {
           print "File : $filename \n";
           print "$sql \n";
           print "N O T     F O U N D   I N     D B \n";
           push @ErrNotFound, $filename;
           $notfound++;
           $skipfile = 1;
       } else {
         $n = $ret->[0][0];
         if ($n > 1) {
           print "File : $filename \n";
           print "$sql \n";
           print "F O U N D   I N     D B, $n times \n";
           print "DO NOTHING \n";
           push @ErrMultFound, $filename;
           $foundtwice++;
           $skipfile = 1;
         }
       }
       if ($skipfile == 0) {
# check file size
        $sql = "SELECT sizemb, crc  FROM ntuples WHERE PATH LIKE '%$dbpath'";
        if ($verbose ==1) { print "$sql \n";}
        $ret=$self->{sqlserver}->Query($sql);
        my $sizemb = $ret->[0][0];
        my $crc    = $ret->[0][1];
        my $inputfile = $filename;
        my $filesize    = (stat($inputfile))[7];
        $filesize = $filesize/1000/1000;
        $filesize = sprintf("%.f",$filesize);
        if ($verbose == 1) {print "$inputfile $filesize/$sizemb [MB] \n";}
        if ($sizemb - $filesize < 2 ) { 
         $sql = "UPDATE ntuples set path='$filename', timestamp=$timenow ";
         if ($checkCRC == 1) {
          my $rstatus = $self->calculateCRC($filename,$crc);
          if ($rstatus != 1) {
            print "restoreDST -ERROR- CRC error for $filename \n";
            $ncrcerr++;
            $skipfile = 1;
            push @ErrCRC, $filename;
          } else {
            $sql = $sql.", crctime=$timenow ";
          }
      }
       $sql = $sql."WHERE PATH LIKE '%$dbpath'";
       if ($update == 1  && $skipfile==0) {
         $self->{sqlserver}->Update($sql);
         if ($verbose)  {print "$sql \n";}
        }
   } else {
       $nsizemm++;
       push @ErrFileSize, $filename;
       print "Error - $inputfile : $filesize MB, DB $dbpath : $sizemb MB \n";
   }
    }
  } 
  else {
   print "File : $filename \n";
   print "Error - cannot get DB Path, AMS02 pattern not found \n";
  } 
 }
 $i++;
  }

     print "Files checked : $#inputFiles \n";
     print "CRC error     : $ncrcerr \n";
     foreach my $f (@ErrCRC) { print "$f \n";}
     print "File size mismatch : $nsizemm \n";
     foreach my $f (@ErrFileSize) { print "$f \n";}
     print "File not found in DB : $notfound \n";
     foreach my $f (@ErrNotFound) { print "$f \n";}
     print "File found twice     : $foundtwice \n";
     foreach my $f (@ErrMultFound) { print "$f \n";}
}


