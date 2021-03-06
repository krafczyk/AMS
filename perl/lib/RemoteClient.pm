# $Id$
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
# Oct 15, 2004    : -dbonly mode for parseJournalFiles
#
#
# Nov 23, 2004    : updateDataSetsDescription
#
# Feb  4, 2005    : AMS02/2005A correct minor bugs
#
# Feb 17, 2005    : allow more than 1 "Active" Production Sets
#                   new subs, remove $ActiveProductionSet use @productionPeriods
#
# Mar  9, 2005    : Protection against invalid JID in parsejournalfiles
#
# Mar 16, 2005    : DBQuery04 performance is improved
# Mar 18, 2005.   : Table Jobs add pid - production period ID
#                   queries are modified accordingly
# Mar 22, 2005.   : use views instead of table (see listStat sub)
# Mar 28, 2005.   : checkJobsTimeOut performance is improved
#                   deleteTimeoutJobs  - ditto
#
# May  4, 2005    : listStat, listRuns, listJobs - queries are modified to
#                   improve performance
#
# May 30, 2005    : last_24h_html (O.Kounina) - production statistics for the
#                   last 24 hours
#
my $nTopDirFiles = 0;     # number of files in (input/output) dir
my @inputFiles;           # list of file names in input dir

package RemoteClient;
use Sys::Hostname;
use Storable;
 use CORBA::ORBit idl => [ '/usr/include/server.idl'];
use Error qw(:try);
use CGI qw(-unique_headers);
use Carp;
use strict;
use lib::Warning;
use MIME::Lite;
use lib::CID;
use lib::DBServer;
use lib::monitorHTML;
use Time::Local;
use lib::DBSQLServer;
use POSIX  qw(strtod);
use File::Find;
use Benchmark;
use Class::Struct;

@RemoteClient::EXPORT= qw(new  datasetlink Connect  Warning ConnectDB ConnectOnlyDB  checkDB listAll listMCStatus listMin listShort queryDB04 DownloadSA castorPath  checkJobsTimeout deleteTimeOutJobs deleteDST  getEventsLeft getHostsList getHostsMips getOutputPath getProductionPeriods getRunInfo updateHostInfo parseJournalFiles prepareCastorCopyScript resetFilesProcessingFlag ValidateRuns updateAllRunCatalog printMC02GammaTest readDataSets set_root_env updateCopyStatus updateHostsMips checkTiming list_24h_html test00 RemoveFromDisks RemoveFromDisksDF  UploadToDisks UploadToDisksDataFiles CheckCRC CheckCRCRaw CheckNTUnique MoveBetweenDisks CastorPrestage UploadToCastor GroupRuns TestPerl );

# debugging
my $benchmarking = 0;
my $t0Init = 0;
my $tsInit = 0;
my $teInit = 0;
my $tdInit = 0;
my @td;
$#td=10;
#

struct ProductionPeriod => {
                              id      => '$',
                              name    => '$',
                              begin   => '$',
                              end     => '$',
                              status  => '$',
                              vgbatch => '$',
                              vdb     => '$',
                              vos     => '$',
                              DST     => '$',
                          };

my @productionPeriods = ProductionPeriod->new();

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
 my @BadRunID     = [];  # files belongs to obsolete runs
 my @CheckedDSTs  = [];  # dsts checked
 my @GoodDSTs     = [];  # copied to final destination
 my @BadDSTs      = [];  # marked as bad because of different reasons
                         # in particular :
 my @BadDSTCopy   = [];  #                 doCopy failed to copy DST
 my @BadCRCi      = [];  #                 dsts with crc error (before copying)
 my @BadCRCo      = [];  #                 dsts with crc error (after copying)
 my @gbDST        = [];

 my $max_jobs = 0;
 my $capacity_jobs = 0;
 my $total_jobs = 0;
 my $completed_jobs = 0;
 my $doCopyTime   = 0;
 my $doCopyCalls  = 0;
 my $crcTime      = 0;
 my $crcCalls     = 0;
 my $fastntTime   = 0;
 my $fastntCalls  = 0;
 my $copyTime     = 0;
 my $copyCalls    = 0;
#-
 my $TestCiteId   = undef;
#

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
my     $monrddb=           '/cgi-bin/mon/monrddb.o.cgi';
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
    if($ENV{MOD_PERL} and ref($RemoteClient::Singleton)){
        return $RemoteClient::Singleton;
    }
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
        AMSSoftwareDir2=>undef,
        AMSDataDir=>undef,
        AMSDataDir2=>undef,
        AMSProdDir=>undef,
        AMSDSTOutputDir=>undef,
        CERN_ROOT=>undef,
        ROOTSYS  =>'/afs/cern.ch/ams/Offline/root/Linux/pro/',
        HTTPserver=>'pcamss0.cern.ch',
        HTTPhtml  =>'http://pcamss0.cern.ch/',
        HTTPcgi   =>'http://pcamss0.cern.ch/cgi-bin/mon',
        UploadsDir=>undef,
        UploadsHREF=>undef,

        ProductionSetName=>[],
        ProductionSetStatus=>[],
        ProductionSetBegin=>[],

        FileDB   =>[],
        FileAttDB=>[],
        FileCRC=>undef,
        FileBBFTP=>undef,
        FileBookKeeping=>undef,
        FileCRCTimestamp=>undef,
        FileBBFTPTimestamp=>undef,
        FileBookKeepingTimestamp=>undef,
        FileDBTimestamp   => [],
        FileAttDBTimestamp=> [],
        FileDBLastLoad    => [],
        FileAttDBLastLoad => [],
        LocalClientsDir=>"prod.log/scripts",
        Name=>'/cgi-bin/mon/rc.cgi',
        DataMC=>0,
        IOR=>undef,
        IORP=>undef,
        ok=>0,
        q=>undef,
        initdone=>0,
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
    my $mybless=bless $self,$type;
    if(ref($RemoteClient::Singleton)){
        croak "Only Single RemoteClient Allowed\n";
    }
$RemoteClient::Singleton=$mybless;
return $mybless;
}

sub Init{
    my @tdebug=();
     my $self = shift;
     my $forceret=shift;
#
    my $sql  = undef;
    my $ret  = undef;

    $self->{eosselect} = '/afs/cern.ch/project/eos/installation/ams/bin/eos.select';

    $t0Init = time();
#just temporary skeleton to check basic princ
#should be replaced by real db servers
      if(defined $self->{q}){
        delete $self->{q};
      }
      $self->{q}=new CGI;
#cpu types
      my $cachetime=3600;
    push @tdebug,time()-$t0Init;
     if(defined $self->{initdone} ){
      if(time()-$self->{initdone}<$cachetime and $self->{initdone} >$self->dblupdate()){
        return 1;
      }
     }
    my %cputypes=(
     'Pentium II'=>1.07,
     'Pentium III'=>1.0,
     'Pentium III Xeon'=>1.05,
     'Pentium IV Xeon'=>0.8,
     'Pentium IV'=>0.8,
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

 my $dir=$ENV{AMSDataDirRW};
 if (defined $dir){
     $self->{AMSDataDir}=$dir;
     $self->{AMSDataDir2}="AMSDataDir=$dir";
 }
 else{
     $self->{AMSDataDir}="/afs/cern.ch/ams/Offline/AMSDataDir";
     $self->{AMSDataDir2}="AMSDataDir=/afs/cern.ch/ams/Offline/AMSDataDir";
     $ENV{AMSDataDirRW}=$self->{AMSDataDir};
 }
#sqlserver
    $self->{sqlserver}=new DBSQLServer();
    $self->{sqlserver}->Connect();
    $tsInit = time();
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
      $self->{ROOTSYS}="/afs/cern.ch/ams/Offline/root/Linux/v3.05.05gcc322/";
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
#+
# Feb 15.02.2004 More than 1 Active Production set is allowed
# More than 1 FileDB, FileAttDB, etc can be available
    $key='FileDB';
    $#{$self->{FileDB}}=-1;
    $sql="select myvalue from Environment where mykey='".$key."'";
    $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     foreach my $file (@{$ret}){
      push @{$self->{FileDB}}, $file;
     }
    }
    else{
     push @{$self->{FileDB}}, "v3.00mcdb.tar.gz";
     push @{$self->{FileDB}}, "v4.00mcdb.tar.gz";
     push @{$self->{FileDB}}, "v4.00rddb.tar.gz";
     push @{$self->{FileDB}}, "v5.00mcdb.tar.gz";
     push @{$self->{FileDB}}, "v5.00rddb.tar.gz";
     push @{$self->{FileDB}}, "v5.01mcdb.tar.gz";
     push @{$self->{FileDB}}, "v5.01rddb.tar.gz";
    }
     $#{$self->{FileAttDB}}=-1;
    $key='FileAttDB';
    $sql="select myvalue from Environment where mykey='".$key."'";
    $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     foreach my $file (@{$ret}){
      push @{$self->{FileAttDB}}, $file;
     }
 }
    else{
      push @{$self->{FileAttDB}}, "v3.00mcdb.addon.tar.gz";
      push @{$self->{FileAttDB}}, "v4.00mcdb.addon.tar.gz";
      push @{$self->{FileAttDB}}, "v4.00rddb.addon.tar.gz";
      push @{$self->{FileAttDB}}, "v5.00mcdb.addon.tar.gz";
      push @{$self->{FileAttDB}}, "v5.00rddb.addon.tar.gz";
      push @{$self->{FileAttDB}}, "v5.01mcdb.addon.tar.gz";
      push @{$self->{FileAttDB}}, "v5.01rddb.addon.tar.gz";
    }

#-
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
     $key='afsroot';
     $sql="select myvalue from Environment where mykey='".$key."'";
     $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     $self->{$key}=$ret->[0][0];
 }
    else{
     $self->{$key}="/afs/cern.ch/ams";
    }

     $key='AMSSoftwareDir';
     my $key2='AMSSoftwareDir2';
     $sql="select myvalue from Environment where mykey='".$key."'";
     $ret=$self->{sqlserver}->Query($sql);
    if( defined $ret->[0][0]){
     $self->{$key}="$self->{AMSDataDir}/".$ret->[0][0];
     $self->{$key2}="\$AMSDataDir/".$ret->[0][0];
 }
    else{
     $self->{$key2}="\$AMSDataDir/DataManagement";
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
    $teInit = time();
#cites table
     $sql="select * from Cites";
      $#{$self->{CiteT}}=-1;
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
     $#{$self->{MailT}}=-1;
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
    if($self->{q}->param("queryDB04") or $self->{q}->param("getRunID") or
    $self->{q}->param("getJobID") or $self->{q}->param("getDSTID")){
       $forceret=1;
    }
#datasets
        if(defined $forceret and $forceret>0){
         return 1;
        }
        $#{$self->{DataSetsT}}=-1;
        my $cem=lc($self->{q}->param("CEM"));
        if(defined $cem){
         if(not $self->findemail($cem)){
#            $self->ErrorPlus("Client $cem Not Found.  Please Register...");
          }
          else{
           my $save="$self->{UploadsDir}/$self->{CEMID}.save2";
             my  @sta = stat $save;
#             my $lupdate=-1;
             my $lupdate=$self->dblupdate();
#              die "  $lupdate $sta[9] $cachetime \n";
             if($lupdate>0){
               $cachetime*=24;
             }
             # die "  $lupdate $sta[9] $cachetime \n";
            if($#sta>0 and time()-$sta[9] <$cachetime and $lupdate<$sta[9]){
             my $dsref=retrieve($save);
             if(defined $dsref){
              foreach my $dss (@{$dsref}){
               push @{$self->{DataSetsT}},$dss;
              }
             }
         }
    push @tdebug,time()-$t0Init;
        my $reputation=1;
        my $ret = $self->{sqlserver}->Query("select capacity, reputation from cites where cid=".$self->{CCID});
	if ( defined $ret->[0][0] ) {
		$capacity_jobs = $ret->[0][0];
                $reputation = $ret->[0][1];
	}

    push @tdebug,time()-$t0Init;
        $ret = $self->{sqlserver}->Query("select count(*) as total from
(select cid as jcid, pid, time as jtime from jobs union all select cid
as jcid, pid, time as jtime from jobs_deleted), productionset where
jcid=".$self->{CCID}." and pid=productionset.did and productionset.status='Active'");
	if ( defined $ret->[0][0] ) {
		$total_jobs = $ret->[0][0];
	}

    push @tdebug,time()-$t0Init;
        $ret = $self->{sqlserver}->Query("select count(*) as COMPLETED from
(select cid, pid as jpid, realtriggers from jobs_deleted), productionset where 
jpid=productionset.did  and cid=".$self->{CCID}." and productionset.status='Active'");
	if ( defined $ret->[0][0] ) {
		$completed_jobs = $ret->[0][0];
	}
    push @tdebug,time()-$t0Init;
       $ret = $self->{sqlserver}->Query("select count(*) as completed from
(select cid as jcid, pid, realtriggers , time as jtime from jobs ) 
, productionset where
jcid=".$self->{CCID}." and realtriggers>0 and pid=productionset.did and productionset.status='Active'");
        if ( defined $ret->[0][0] ) {
                $completed_jobs+= $ret->[0][0];
        }
 
    push @tdebug,time()-$t0Init;
         $ret = $self->{sqlserver}->Query("select count(*) as COMPLETED from
(select cid, pid as jpid, jid as jjid, realtriggers  from jobs), ntuples,productionset where 
jjid=ntuples.jid and ntuples.path like '%00000%' and jpid=productionset.did and realtriggers<=0 and cid=".$self->{CCID}." and productionset.status='Active' ");
	if ( defined $ret->[0][0] ) {
		$completed_jobs = $completed_jobs+$ret->[0][0];
	}
    push @tdebug,time()-$t0Init;
        $ret = $self->{sqlserver}->Query("select count(*) as COMPLETED from
(select cid, pid as jpid, jid as jjid, realtriggers  from jobs), ntuples,datafiles,productionset where
jjid=ntuples.jid and ntuples.path like '%00000%' and datafiles.type like '%MC%'  and datafiles.run=ntuples.jid and jpid=productionset.did and realtriggers<=0 and cid=".$self->{CCID}." and productionset.status='Active' ");
        if ( defined $ret->[0][0] ) {
                $completed_jobs = $completed_jobs-=$ret->[0][0];
        }
    push @tdebug,time()-$t0Init;
       $ret = $self->{sqlserver}->Query("select count(*) as COMPLETED from
(select cid, pid as jpid, jid as jjid, realtriggers  from jobs), datafiles,productionset where
jjid=datafiles.run and datafiles.type like '%MC%'  and jpid=productionset.did and realtriggers<=0 and cid=".$self->{CCID}." and productionset.status='Active' ");
        if ( defined $ret->[0][0] ) {
                $completed_jobs = $completed_jobs+=$ret->[0][0];
        }

        $max_jobs = int(2.2*$capacity_jobs*$reputation*($completed_jobs/($total_jobs+1))+1-($total_jobs-$completed_jobs));
        if($completed_jobs > 0){
	    if($max_jobs < $capacity_jobs*$reputation-$total_jobs+$completed_jobs){
		$max_jobs = int($capacity_jobs*$reputation-$total_jobs+$completed_jobs);
            }
        }
        if($max_jobs <= 0){
            $max_jobs = 0;
            if(int($self->{CCTP})>10){
             $max_jobs=$capacity_jobs;
            }
        }
          }
        }

    push @tdebug,time()-$t0Init;
if($#{$self->{DataSetsT}}==-1){
   my $ndatasets  =0; # total datasets scanned
   my $nfiles     =0; # total jobs scanned
   my $totalcpu   =0;
   my $restcpu    =0;
  my @st=();
    $st[0]=0;
    $st[1]=0;
    $st[2]=0;
    $st[3]=0;

   $dir="$self->{AMSSoftwareDir}/DataSets";
#
# get DB info :
#               All datasets
  my  $datasetsDB  = undef;
#  my  $jobsDB      = undef;
#  my ($period, $periodStartTime, $periodId) = $self->getActiveProductionPeriod();
# get list of datasets (names and dids)
#  my  $sql="select count(did) from DataSets";
#  my $ret=$self->{sqlserver}->Query($sql);
#  if (defined $ret->[0][0]) {$ndatasetsDB = $ret->[0][0];}
#  if ($ndatasetsDB > 0) {
      $sql = "SELECT  DID  FROM ProductionSet WHERE STATUS='Active' ORDER BY DID";
      $ret = $self->{sqlserver}->Query($sql);
   my $pps=undef;
   foreach my $pp  (@{$ret}){
       if(defined $pps){
           $pps=$pps." or jobs.pid =";
       }
       else{
           $pps=" where jobs.pid = ";
       }
       $pps=$pps." $pp->[0] ";
   }

   $sql="select did, name,version from DataSets";
   $datasetsDB =$self->{sqlserver}->Query($sql);
   $st[0]=$st[0]+1;
   if(defined $datasetsDB->[0][0]){
#   $sql="select count(jid) from Jobs where pid = $periodId";
#   $ret=$self->{sqlserver}->Query($sql);
#   if (defined $ret->[0][0]) {
#    $njobsDB = $ret->[0][0];
       $sql="select jid, time,triggers,timeout, did, jobname, realtriggers from Jobs" ;
#         where Jobs.pid = $periodId";
        $sql=$sql.$pps;
#    $jobsDB= $self->{sqlserver}->Query($sql);
#    $st[1]=$st[1]+1;
#   }
  }
# read list of datasets dirs from $dir
   opendir THISDIR ,$dir or die "unable to open $dir";
   my @allfiles= readdir THISDIR;
   closedir THISDIR;

    push @tdebug,time()-$t0Init;
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

          $sql = "SELECT  DID  FROM ProductionSet WHERE STATUS='Active' ORDER BY DID";
         my $ret = $self->{sqlserver}->Query($sql);
         $pps=undef;
         foreach my $pp  (@{$ret}){
          if(defined $pps){
            $pps=$pps." or jobs.pid =";
          }
          else{
           $pps=" and ( jobs.pid =";
          }
         $pps=$pps." $pp->[0] ";
        }
       if(defined $pps){
        $pps=$pps." ) ";
       }
       else{
        $pps="";
}

# scan all dataset dirs

  $#{$self->{DataSetsT}}=-1;
   foreach my $file (@allfiles){
    my $newfile="$dir/$file";
    if(readlink $newfile or  $file =~/^\./){
     next;
    }
    $ndatasets++;


    my @sta = stat $newfile;
    if($sta[2]<32000){
     my $dataset={};
     $dataset->{name}=$file;
     $dataset->{jobs}=[];
     $#{$dataset->{jobs}}=-1;
     $dataset->{eventstodo} = 0;
     $dataset->{eventstotal} = 0;
     $dataset->{version}="v4.00";
     $dataset->{datamc}=0;
     my @tmpa;
     $#tmpa=-1;
     opendir THISDIR, $newfile or die "unable to open $newfile";
     my @jobs=readdir THISDIR;
     closedir THISDIR;
#     push @{$self->{DataSetsT}}, $dataset;
     $dataset->{datamc}=0;
     $dataset->{g4}="";
     $dataset->{build}="";
     $dataset->{delta}=0;
     $dataset->{singlejob}=0;
     $dataset->{closed}=0;
     foreach my $job (@jobs){
         chomp $job;
         if($job=~/^data=true/){
             $dataset->{datamc}=1;
#             $self->{DataMC}=1;
         }
         if($job=~/\.runlist$/){
             if(open(FILE,"<"."$newfile/$job")){
                 $dataset->{$job}=<FILE>;
                 if($dataset->{$job}=~/\,$/){
                     chop $dataset->{$job};
                 }
                 close FILE;
             }
         }
         if($job=~/\.runalist$/){
             if(open(FILE,"<"."$newfile/$job")){
                 $dataset->{$job}=<FILE>;
                 if($dataset->{$job}=~/\,$/){
                     chop $dataset->{$job};
                 }
                 close FILE;
             }
         }
         if($job=~/^mc=true/){
             $dataset->{MC}=1;
#             $self->{DataMC}=1;
         }
         if($job=~/^g4=true/){
             $dataset->{g4}="g4";
         }
         if($job=~/^closed=true/){
             $dataset->{closed}=1;
         }
         if($job=~/^build=/){
            my @vrs= split '=',$job;
             $dataset->{build}=$vrs[1];
            my @j1=split '.B',$job;
            if($#j1>0){
                my $build=substr($j1[$#j1],0,4);
                $build =~ tr/0-9//cd;
                if(not defined $self->{Build} or $build>$self->{Build}){
#                    $self->{Build}=$build;
                    $dataset->{buildno}=$build;
                }
               
            }
         }
         if($job=~/^delta=/){
                  my @vrs= split '=',$job;
             $dataset->{delta}=$vrs[1];
           }
           if($job=~/^singlejob=true/){
             $dataset->{singlejob}=1;
         }
        if($job=~/^version=/){
             my @vrs= split '=',$job;
             $dataset->{version}=$vrs[1];
             if($dataset->{version}=~/v5/){
#              $self->{DataMC}=1;
             }
         }
 
     }
     if($dataset->{closed}){
         next;
     }
     if($dataset->{datamc}==0 ){
     foreach my $job (@jobs){
         if($job=~/^lastserverno=/){
             my @vrs= split '=',$job;
               $dataset->{serverno}=$vrs[1];
             unlink "$newfile/$job";
           last;           
         }
         if($job=~/^serverno=/){
             my @vrs= split '=',$job;
             my @add=split ',',$vrs[1];
     if(not defined $dataset->{serverno}){
             if($#add>0){
                 my $tme=time();
               $dataset->{serverno}=$add[$tme%($#add+1)];
                 my $sys="touch $newfile/lastserverno=$dataset->{serverno}";
                 my $i=system($sys);
             }          
             else{
               $dataset->{serverno}=$vrs[1];
              }
         }
          last;
         }
     }
     if(not defined $dataset->{serverno}){
         $dataset->{serverno}=$dataset->{version};
     }

    push @tdebug,time()-$t0Init;
      foreach my $job (@jobs){
      if($job =~ /\.job$/){
       if($job =~ /^\./){
            next;
        }
       $nfiles++;
       my $template={};
       my $full="$newfile/$job";
       my $buf;
       open(FILE,"<".$full) or die "Unable to open dataset file $full \n";
       read(FILE,$buf,1638400) or next;
       close FILE;
       $td[3] = time();
       $template->{filename}=$job;
       my @sbuf=split "\n",$buf;

       my @farray=("TOTALEVENTS","PART","PMIN","PMAX","TIMBEG","TIMEND","CPUPEREVENTPERGHZ","OPENCLOSE","THREADS");
       $template->{OPENCLOSE}=undef;
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
        
            if(defined $template->{OPENCLOSE}){
                #die " $template->{OPENCLOSE} $template->{TOTALEVENTS} $full " ;
           if($template->{OPENCLOSE}==0){
               $template->{TOTALEVENTS}=0;
           }
       elsif($template->{OPENCLOSE}==1 and $self->{CCT} eq 'remote' and $self->{CCID}!=2){
           $template->{TOTALEVENTS}=0;
         }
       elsif($template->{OPENCLOSE}==2 and $self->{CCT} eq 'local'){
           $template->{TOTALEVENTS}=0;
         }
       elsif($template->{OPENCLOSE}<0 and $self->{CCID} ne -$template->{OPENCLOSE}){
             $template->{TOTALEVENTS}=0;
         }
       elsif($max_jobs <= 0 and $self->{CCID}!=1){
            $template->{TOTALEVENTS}=0;
         }

       }
       if($dataset->{singlejob}){
           $template->{CPUPEREVENTPERGHZ}=1.e-9;
       }
        if(defined $self->{TrialRun}){
            $template->{TOTALEVENTS}*=$self->{TrialRun};
        }
        $template->{TOTALEVENTSC}=$template->{TOTALEVENTS};
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
              my $datasetfound = 0;
              foreach my $ds (@{$datasetsDB}){
                my $datasetsDidDB  = $ds->[0];
                my $datasetsNameDB = $ds->[1];
                my $datasetsVDB = $ds->[2];
               if (trimblanks($datasetsNameDB) eq trimblanks($dataset->{name}) and trimblanks($datasetsVDB) eq trimblanks($dataset->{version})  ) {
    
                 $dataset->{did}=$datasetsDidDB;
                  $sql="select sum(realtriggers) from jobs where did=$dataset->{did} and  jobname like '%$template->{filename}' and realtriggers>0".$pps;
                  my $rtn1=$self->{sqlserver}->Query($sql);
                   my $tm=time();
                  $sql="select sum(calcevents(time+timeout-$tm,Triggers)) from jobs where did=$dataset->{did} and  jobname like '%$template->{filename}' and realtriggers<0 and time+timeout>=$tm and timekill=0".$pps;
                  my $rtn2=$self->{sqlserver}->Query($sql);
               $datasetfound = 1;
                 my $rtrig=$rtn1->[0][0];
                 my $subm= $rtn2->[0][0];
                 if(not defined $rtrig){
                  $rtrig=0;
                 }              
                 if(not defined $subm){
                  $subm=0;
                 }              
                 $template->{TOTALEVENTS}-= $rtrig+$subm;
                 if($template->{TOTALEVENTS}<0){
                     $template->{TOTALEVENTS}=0;
                 }
#               die "  $template->{TOTALEVENTS} $rtn1->[0][0]+$rtn2->[0][0] $dataset->{name} $template->{filename} \n";
               last;
             } # $datasetsNameDB eq $dataset->{name}
            }

           if ($datasetfound == 0) {  # new dataset
               my $did = 1;
               $sql = "SELECT MAX(did) From DataSets";
               my $ret=$self->{sqlserver}->Query($sql);
               if (defined $ret->[0][0]) {
                   $did = $ret->[0][0]+1;
               }
               $dataset->{did}=$did;
               my $timestamp = time();
               my $dmc=$dataset->{datamc}+$dataset->{MC}*10;
               $sql="insert into DataSets values($did,'$dataset->{name}',$timestamp, '$dataset->{version}',$dmc)";
               $self->{sqlserver}->Update($sql);
               $sql="select did, name,version from DataSets";
               $datasetsDB =$self->{sqlserver}->Query($sql);
           }

           $restcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};

           if($sbuf[0] =~/^#/ && defined $template->{initok}){
            $buf=~ s/#/C /;
            $template->{filebody}=$buf;
            my $desc=$sbuf[0];
            substr($desc,0,1)=" ";
            $template->{filedesc}=$desc." Total Events Left $template->{TOTALEVENTS}";
            $dataset->{eventstodo} += $template->{TOTALEVENTS};
            $dataset->{eventstotal} += $template->{TOTALEVENTSC};
            if($template->{TOTALEVENTS}>1000){
             push @tmpa, $template;
         }
          }
   }
     } # end jobs of jobs
 }
     else{
# here data type datasets
      #datamc==1
         my $datafiles="datafiles";
#         if($dataset->{datamc}/10==1){
#             $datafiles="mcfiles";
#         }
     foreach my $job (@jobs){

      if($job =~ /\.job$/){
       if($job =~ /^\./){
            next;
        }
       $nfiles++;
       my $template={};
       my $full="$newfile/$job";
       my $buf;
       open(FILE,"<".$full) or die "Unable to open dataset file $full \n";
       read(FILE,$buf,16384000) or next;
       close FILE;
       $td[3] = time();
       $template->{filename}=$job;
       my @sbuf=split "\n",$buf;
       my @farray=("TOTALEVENTS","RUNMIN", "SERVERNO", "RUNMAX","OPENCLOSE","CPUPEREVENTPERGHZ","QTYPE","HOST","ROOTNTUPLE","RUNLIST","RUNALIST","PRIO","TAG","EVENTMIN","THREADS");
          $template->{THREADS}=1;   
          $template->{TAG}=-1;   
          $template->{RUNMIN}=0;   
          $template->{RUNMAX}=4000000000;   
          $template->{EVENTMIN}=1; 
           foreach my $ent (@farray){
            foreach my $line (@sbuf){
               if($line =~/$ent=/){
                   my @junk=split "$ent=",$line;
                   $template->{$ent}=$junk[$#junk];
                   $buf=~ s/$ent=/C $ent=/;
                   if($ent =~/LIST/){
                       $buf=~s/$line/C $ent=/;
                   }
                   last;
               }
            }
        }
#
#  check for cite.runlist files 
#

       if(defined $template->{SERVERNO}){
           $dataset->{serverno}=$template->{SERVERNO};
       }
       if(defined $dataset->{"$self->{CCA}.runlist"}){
          $template->{RUNLIST}=$dataset->{"$self->{CCA}.runlist"};   
       }
       if(defined $dataset->{"$self->{CCA}.runalist"}){
          $template->{RUNALIST}=$dataset->{"$self->{CCA}.runalist"};   
       }


           $template->{initok}=1;
           foreach my $ent (@farray){
             if(not defined $template->{$ent} and ($ent ne "HOST" and $ent ne "ROOTNTUPLE" and $ent ne "RUNLIST" and $ent ne "RUNALIST" and $ent ne "PRIO" and $ent ne "TAG" and $ent ne "SERVERNO" and $ent ne "THREADS")){
               $template->{initok}=undef;
             }
           }
          if($dataset->{delta}>0){
             $template->{RUNMAX}=time()-$dataset->{delta};
          }
             if(defined $template->{OPENCLOSE}){
           if($template->{OPENCLOSE}==0){
             $template->{RUNMAX}=1;
             $template->{RUNLIST}=undef;
           }
       elsif($template->{OPENCLOSE}==1 and $self->{CCT} eq 'remote' and $self->{CCID}!=2){
             $template->{RUNMAX}=1;
         }
       elsif($template->{OPENCLOSE}==2 and $self->{CCT} eq 'local'){
             $template->{RUNMAX}=1;
             $template->{RUNLIST}=undef;
         }
       elsif($template->{OPENCLOSE}<0 and $self->{CCID} ne -$template->{OPENCLOSE}){
             $template->{RUNMAX}=1;
             $template->{RUNLIST}=undef;
         }
       elsif($max_jobs <= 0 and $self->{CCID}!=1){
            $template->{RUNMAX}=1;
             $template->{RUNLIST}=undef;
         }
       }
       if(defined $template->{ROOTNTUPLE}  ){
           $dataset->{rootntuple}=$template->{ROOTNTUPLE};
       }

       if($dataset->{singlejob}){
           $template->{CPUPEREVENTPERGHZ}=1.e-9;
       }
#
# get no of events
#
# the idea is to first check openclose:  if 1:  open
# then goto to the datafiles; collect number of events 
# for the runs which have no corresponding ntuples in dataset with # sign... 
#  if such run exists put total events;
#   for the corresponding window put number of events fixed (10000000)
#   but correctly count number of events according to the run info....
#
         if($template->{initok}==1){       

              my $datasetfound = 0;
              foreach my $ds (@{$datasetsDB}){
                my $datasetsDidDB  = $ds->[0];
                my $datasetsNameDB = $ds->[1];
                my $datasetsVDB = $ds->[2];
               if ($datasetsNameDB eq $dataset->{name} and trimblanks($datasetsVDB) eq trimblanks($dataset->{version})  ) {
                 $dataset->{did}=$datasetsDidDB;
                 $datasetfound = 1;
                 last;
             } # $datasetsNameDB eq $dataset->{name}
            }
            if ($datasetfound == 0) {  # new dataset
               my $did = 1;
               $sql = "SELECT MAX(did) From DataSets";
               my $ret=$self->{sqlserver}->Query($sql);
               if (defined $ret->[0][0]) {
                   $did = $ret->[0][0]+1;
               }
               $dataset->{did}=$did;
               my $timestamp = time();
               my $dmc=$dataset->{datamc}+$dataset->{MC}*10;
               $sql="insert into DataSets values($did,'$dataset->{name}',$timestamp, '$dataset->{version}',$dmc)";
               $self->{sqlserver}->Update($sql);
               $sql="select did, name, version from DataSets";
               $datasetsDB =$self->{sqlserver}->Query($sql);
             }
           my $runlist="";
            if(defined $template->{RUNLIST}){
                my @junk=split   ",",$template->{RUNLIST};
                $runlist=" and (";
                foreach my $r (@junk){
                    $runlist=$runlist." run=$r or ";
                }
                $runlist=$runlist." run=-1)";
            }
           my $runalist="";
            if(defined $template->{RUNALIST}){
                my @junk=split   ",",$template->{RUNALIST};
                $runalist=" and (";
                foreach my $r (@junk){
                    $runalist=$runalist." run!=$r and ";
                }
                $runalist=$runalist." run!=-1)";
            }
           my $qtype="and  $datafiles.type not like '%CAL%' ";
           if($template->{QTYPE} ne ""){
               $qtype="and $datafiles.type like '$template->{QTYPE}%'";
            }
                   my $sql="select sum($datafiles.nevents),count($datafiles.run) from $datafiles where run>=$template->{RUNMIN} and $datafiles.nevents>$template->{EVENTMIN} and run<=$template->{RUNMAX}  and ($datafiles.status='OK' or $datafiles.status='Validated') $qtype $runlist  $runalist and $datafiles.nevents>0 and run not in  (select run from dataruns,jobs where  dataruns.jid=jobs.jid and jobs.did=$dataset->{did} and jobs.jobname like '%$template->{filename}') and run not in  (select distinct run from ntuples,jobs where  ntuples.jid=jobs.jid and jobs.did=$dataset->{did} and jobs.realtriggers>0 and jobs.jobname like '%$template->{filename}') ";
              if($template->{TAG}>=0){
                  $sql=$sql." and $datafiles.tag=$template->{TAG} ";
              }
                 my $rtn=$self->{sqlserver}->Query($sql);
                 if(defined $rtn){
                  $template->{TOTALEVENTS}=$rtn->[0][0];
                  $template->{TOTALRUNS}=$rtn->[0][1];
                 }
                 else{
                     $template->{TOTALEVENTS}=0;
                 }
               if($template->{filename} =~/el180/){
#die "  $template->{TOTALEVENTS} $template->{TOTALRUNS} $dataset->{name} $template->{filename} $sql \n";
          }
           if(not defined $template->{TOTALEVENTS}){
               $template->{TOTALEVENTS}=0;
            } 
          if(not defined $template->{TOTALEVENTSC}){
               $template->{TOTALEVENTSC}=0;
            }

           $restcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};

           if($sbuf[0] =~/^#/ && defined $template->{initok}){
            $buf=~ s/#/C /;
            $template->{filebody}=$buf;
            my $desc=$sbuf[0];
            substr($desc,0,1)=" ";
            $template->{filedesc}=$desc." Total Events/Runs Left $template->{TOTALEVENTS} $template->{TOTALRUNS}  ";
            $dataset->{eventstodo} += $template->{TOTALEVENTS};
            $dataset->{eventstotal} += $template->{TOTALEVENTSC};
            if($template->{TOTALEVENTS}>0){
             push @tmpa, $template;
         }
          }
   }
   }
     } # end jobs of jobs
     foreach my $job (@jobs){
         if($job=~/^lastserverno=/){
             my @vrs= split '=',$job;
               $dataset->{serverno}=$vrs[1];
             unlink "$newfile/$job";
            last;           
         }
         if($job=~/^serverno=/){
             my @vrs= split '=',$job;
             my @add=split ',',$vrs[1];
     if(not defined $dataset->{serverno}){

             if($#add>0){
                 my $tme=time();
               $dataset->{serverno}=$add[$tme%($#add+1)];
                 my $sys="touch $newfile/lastserverno=$dataset->{serverno}";
                 my $i=system($sys);
             }          
             else{
               $dataset->{serverno}=$vrs[1];
              }
         }
             last;
         }
     }
     if(not defined $dataset->{serverno}){
         $dataset->{serverno}=$dataset->{version};
     }


     }
    sub prio { $b->{TOTALEVENTS}*$b->{CPUPEREVENTPERGHZ}  <=> $a->{TOTALEVENTS}*$a->{CPUPEREVENTPERGHZ};}
    my @tmpb=sort prio @tmpa;
    foreach my $tmp (@tmpb){
     push @{$dataset->{jobs}},$tmp;
    }
    push @{$self->{DataSetsT}}, $dataset;
   }
  } # end files of allfiles
 #die "  query:  @st ";

}

#templates

     $dir="$self->{AMSSoftwareDir}/Templates";
    opendir THISDIR ,$dir or die "unable to open $dir";
    my @allfiles= readdir THISDIR;
    closedir THISDIR;
    $#{$self->{TempT}}=-1;
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
     $self->CheckFS(1,86400,0,'/');
# filesystems table
     $#{$self->{FilesystemT}}=-1;
     $sql="select * from Filesystems WHERE status='Active' and isonline=1 ";
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
    $sql="select dbfilename,lastupdate,IORS,IORP,hostname from Servers where status='Active' and datamc=$self->{DataMC}";
     $ret=$self->{sqlserver}->Query($sql);
    my $updlast=0;
    foreach my $upd (@{$ret}){
        if($upd->[1]> $updlast){
            $updlast=$upd->[1];
            $self->{dbfile}=$upd->[0];
            $self->{hostdbfile}=$upd->[4];
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

    if(defined $ior and $updlast==0){
      if ($webmode ==0) {print "Init -I- IOR got it \n";}
      chomp $ior;
      if($self->{IOR} ne $ior){
        $self->{IOR}=$ior;
        $self->{IORP}=undef;
      }
  }
#if($cem=~/afl3u1/){
#die " $max_jobs\n";
#}
    $self->{initdone}=time();
    push @tdebug,time()-$t0Init;
#die "timing @tdebug";
 if( not defined $self->{IOR}){
  return $self->{ok};
 }
    return 1;
}

sub ServerConnectDB{
    my $ref = shift;
    my $ver=shift;
    my $datamc=0;
    if(defined $ver){
      if($ver=~/v5/){
          $datamc=1;
      }
      elsif($ver=~/v4/){
          $datamc=0;
      }
      else{
          $datamc=$ver;
      }
       
  }
    my $ret="";
    my $sql="select dbfilename,lastupdate,IORS,IORP,hostname from Servers where status='Active' and datamc=$datamc order by lastupdate desc";
      $ret=$ref->{sqlserver}->Query($sql);
    if(defined $ret->[0][0]){
            $ret=$ret->[0][0];
        }
    return $ret;
}



sub ServerConnect{
    my $ref = shift;
    my $ver=shift;
    if($#{$ref->{arpref}}>=0){
#        warn "  already connected $#{$ref->{arpref} \n";
        return 1; 
        }
$ref->{cid}=new CID;
$ref->{orb} = CORBA::ORB_init("orbit-local-orb");
    my $tm={};
#try to get ior
    my $datamc=0;
    if(defined $ver){
      if($ver=~/v5/){
          $datamc=1;
      }
      elsif($ver=~/v4/){
          $datamc=0;
      }
      else{
          $datamc=$ver;
      }
    }
    if($datamc!=$ref->{DataMC}){   
    my $sql="select dbfilename,lastupdate,IORS,IORP,hostname from Servers where status='Active' and datamc=$datamc";
     my $ret=$ref->{sqlserver}->Query($sql);
   $ref->{DataMC}=$datamc; 
   my $updlast=0;
    foreach my $upd (@{$ret}){
        if($upd->[1]> $updlast){
            $updlast=$upd->[1];
            $ref->{dbfile}=$upd->[0];
            $ref->{hostdbfile}=$upd->[4];
            $ref->{IOR}=$upd->[2];
            $ref->{IORP}=$upd->[3];
        }
    }
}
  try{
           
#           die "$datamc $ver $ref->{DataMC} $ref->{IOR}";
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



#get prod server id

    if($#{$ref->{arpref}}<0){
     try{
         my %cid=%{$ref->{cid}};
         $cid{Type}="Producer";
      my ($length,$pars)=${$ref->{arsref}}[0]->getARS(\%cid,"Any",0,1);
         if($length==0 ){
         }
         else{
         foreach my $ior (@$pars){
             try{
              my $newref=$ref->{orb}->string_to_object($ior->{IOR});
              push @{$ref->{arpref}}, $newref;
             }
             catch CORBA::SystemException with{
               die "stringtoobject  SystemException Error "."\n";
             };

         }
     }
 }
     catch CORBA::SystemException with{
         $ref->sendmailerror("Unable To connect to Server"," ");
     };
}

#get db server id

     $#{$ref->{ardref}}=-1;
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
                 my $hostname=$ref->{sqlserver}->ior2host($ref->{IOR});
                   
                 $sql="insert into Servers values('$ref->{dbfile}','$ref->{IOR}','$ref->{IORP}','xyz','Active',$ac->{Start},$createt,$ref->{DataMC}),'$hostname'";
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
           $sql="select dbfilename,lastupdate from Servers where status='Active' and datamc=$self->{DataMC}";
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
   my $ver=shift;
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
   my $rflag        = 0;     # processing flag from FilesProcessing Table
   my $procstarttime= 0;     # files processing start time from  FilesProcessing Table

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
     -rRun    -  perfom op only for run  Run;
     -d     -  delete completed runs/ntuiples from server
     ./validateRuns.o.cgi -c -v
";
   my $Run=0;
   my $delete=0;
  $rmprompt = 0;
   foreach my $chop  (@ARGV){
    if ($chop =~/^-c/) {
        $webmode = 0;
    }
    if ($chop =~/^-r/) {
        $Run=unpack("x2 A*",$chop);
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
    if ($chop =~/^-d/) {
     $delete = 1;
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
    if (not $self->ServerConnect($ver)){
        die "ValidateRuns -F- Unable To Connect To Server";
    }
    if ($verbose && $webmode==0) {print "ValidateRuns -I- Connected \n";}
#
# check flag
     ($rflag, $procstarttime) = $self->getFilesProcessingFlag();
     if ($rflag == 1) {
         $self->amsprint("ValidateRuns -E- Processing flag = $rflag, $procstarttime. Stop validation.",0);
         return 1;
     }
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
    $sql = "SELECT min(begin) FROM productionset WHERE STATUS='Active' ORDER BY begin";
    $ret = $self->{sqlserver}->Query($sql);
    if(not defined $ret->[0][0]){
      $self->amsprint("validateRuns -ERROR- cannot find 'Active' production set",0);
      die "exit\n";
    }
    my $firstjobtime = $ret->[0][0] - 24*60*60;
    if ($webmode ==0  and $verbose ==1) { print "ValidateRuns -I- select list of runs from DB \n";}
    $sql="SELECT run,submit FROM Runs WHERE submit > $firstjobtime";
    $ret=$self->{sqlserver}->Query($sql);

# get list of runs from Server
    if ($webmode ==0  and $verbose ==1) {
       print "ValidateRuns -I- get list of runs from server \n";
    }
    if( not defined $self->{dbserver}->{rtb}){
     if ($webmode ==0  and $verbose ==1) {
       print "ValidateRuns -I- call DBServer:InitDBFile \n";
     }
      DBServer::InitDBFile($self->{dbserver});
    }
    if ($webmode ==0  and $verbose ==1) { print "ValidateRuns -I- set active production set \n";}
#    my ($period,$ptime,$periodId) = $self->getActiveProductionPeriod();
#    if (not defined $period || $period eq $UNKNOWN || $ptime==0) {
#      $self->amsprint("parseJournalFiles -ERROR- cannot get active production set",0);
#      die "bye";
#   }

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
   my @rmcmd=();
   my @rmbad=();
    if ($webmode ==0  and $verbose ==1) {
       print "ValidateRuns -I- got from server :  $#{$self->{dbserver}->{rtb}} runs \n";
    }

    foreach my $run (@{$self->{dbserver}->{rtb}}){
     my $outputpath=undef;

        if(($run->{Run} ne $Run and $Run>0) or $run->{DataMC}!=0){
            next;
        }

#        die " $Run $run->{Run} "; 
#
# check jobs processing flag if -1 stop processing
     ($rflag, $procstarttime) = $self->getFilesProcessingFlag();
     if ($rflag == -1) {
         $self->amsprint("Processing flag = $rflag, $procstarttime. Stop Validation.",0);
         return 1;
     }
#
     $timenow = time();
     $CheckedRuns[0]++;
     my @cpntuples   =();
      $#cpntuples=-1;
     my @mvntuples   =();
     $#mvntuples=-1;
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
       } else { #-1
        print "Run,FirstEvent,LastEvent,Submitted,Status...";
        print "$run->{Run},$run->{FirstEvent},$run->{LastEvent},$run->{SubmitTime},$run->{Status} \n";
   }
     $sql   = "SELECT run, status FROM runs WHERE run=$run->{Run}";
     $r0 = $self->{sqlserver}->Query($sql);
      
  }
     $sql   = "SELECT count(path)  FROM ntuples WHERE run=$run->{Run} and datamc=0";
     my $r1 = $self->{sqlserver}->Query($sql);
#     $sql   = "SELECT count(path)  FROM mcfiles WHERE run=$run->{Run}";
     $sql   = "SELECT count(path)  FROM datafiles WHERE run=$run->{Run} and type like 'MC%'";
     my $r2 = $self->{sqlserver}->Query($sql);
      my $status=$r0->[0][1];
      if(defined $status && $status eq 'Completed' && $r1->[0][0]==0 && $r2->[0][0]==0){
          $status='UnChecked';
      }
        if(defined $status and $status ne "Completed" and $status ne $run->{Status}){
         $sql="update runs set Status='$run->{Status}'  WHERE run=$run->{Run}";
         $self->{sqlserver}->Update($sql);
         }
#--     if(($run->{Status} eq "Finished" || $run->{Status} eq "Failed") &&
#--     (defined $r0->[0][1] && ($r0->[0][1] ne "Completed" && $r0->[0][1] ne "Unchecked" && $r0->[0][1] ne "TimeOut"))
     if(($run->{Status} eq "Finished") &&
      (defined $status && ($status ne "Completed" && $status ne "TimeOut"))
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
         if($self->findJob($run->{Run},undef,undef,1)!=$run->{Run}){
          $sql = "UPDATE runs SET status='Failed' WHERE run=$run->{Run}";
          $self->{sqlserver}->Update($sql);
          $warn = "cannot find status, content in Jobs for JID=$run->{Run}. *TRY* Delete Run=$run->{Run} from server \n";
          if ($verbose == 1) {$self->printWarn($warn);}
          print FILEV "$warn \n";
#          DBServer::sendRunEvInfo($self->{dbserver},$run,"Delete");
          print "--- done --- \n";
          print FILEV "--- done --- \n";
         }
         else { #0
          my $r1=$self->{sqlserver}->Query($sql);
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
           my $mips  = $run->{cinfo}->{Mips};
           my $events  = $run->{cinfo}->{EventsProcessed};
           my $errors  = $run->{cinfo}->{CriticalErrorsFound};
           my $cputime = sprintf("%.2f",$run->{cinfo}->{CPUMipsTimeSpent});
           my $elapsed = sprintf("%.2f",$run->{cinfo}->{TimeSpent});
           my $host    = $self->gethostname($run->{cinfo}->{HostName});
           if($mips<=0){
               print "  Mips Problem $mips $run->{cinfo}->{HostName} \n";
               $mips=1000;
           }
           $cputime=$cputime/$mips*1000;
           if ($events == 0 && $errors == 0 && $run->{Status} eq 'Finished'  && $run->{Run} eq 0) {
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
                                     MIPS=$mips,
                                     TIMESTAMP=$timenow
                            WHERE JID = $run->{Run}";
          $self->{sqlserver}->Update($sql);
          print FILEV "$sql \n";
# validate ntuples
# Find corresponding ntuples from server
               $sql = "DELETE ntuples WHERE jid=$run->{uid}";
               $self->{sqlserver}->Update($sql);
            my @ntuplelist=();
             foreach my $ntuple (@{$self->{dbserver}->{dsts}}){
              if(($ntuple->{Status} eq "Success" or $ntuple->{Status} eq "Validated") and $ntuple->{Run}== $run->{Run} ){
                  $ntuple->{OK}=1;
                  push @ntuplelist, $ntuple;
              }
          }
sub nprio{ $b->{Insert} <=> $a->{Insert};}
my @sntl = sort nprio @ntuplelist;
my $fevt=-1;
            my $dat0=0;
 foreach my $ntuple (@sntl){
     if($fevt>=0 and $ntuple->{LastEvent} > $fevt and ($ntuple->{Type} ne "RawFile")){
         $ntuple->{OK}=0;
         warn "  problems with $run->{Run} $ntuple->{Name} $fevt $ntuple->{LastEvent}\n";
     }
     else{
                  if($ntuple->{Type} ne "RawFile"){
                    $fevt=$ntuple->{FirstEvent};
                }
                  if($ntuple->{Name}=~/:\/dat0/){
                      $dat0=1;
                  }

     } 
 }
            foreach my $ntuple (@sntl){
                if($ntuple->{OK}==0){
                  $ntuple->{Name}=~s/\/\//\//;
                  my @fpatha=split ':', $ntuple->{Name};
                  my $fpath=$fpatha[$#fpatha];
                  push @rmbad,"rm $fpath"; 
                }
                else{
                    if($dat0){
                        $ntuple->{Status}="Success";
                    }
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
                  my $rcp=1;
                  if(not $suc){
#
# try to copy to to local directory
#
                     
                my @parser=split ':',$ntuple->{Name};
            if($#parser>0){
                my $host=$parser[0];
                my $hostok=0;
                if($host =~ /cern.ch/ ){
#                   ok
                    $hostok=1;
                }
                else{   #1
                 my @hparser=split '\.',$host;
                 if($#hparser>0 and $hparser[1] eq 'om'){
                     $host=$hparser[0].'.cern.ch';
                     $hostok=1;
                 }
                 else { #2 
#                   warn "Host Bizarre $host \n";
                 }
               }
                if($hostok ){
                    my @dir=split $run->{Run},$fpath;
                    my $cmd="mkdir -p $dir[0]";
                    my $i=system($cmd);
                    $cmd="scp -2  $host:$fpath $fpath ";
                    if($dir[0]=~/^\/dat0/){
                        $i=system($cmd);
                    }
                    else{
                     $i=system($cmd);
                    }
                    if($i){              
                       warn "$cmd failed $i \n";
                       $rcp=0;
                       
                   }
                    else{ #3
                             $cmd="ssh -2 -x $host rm $fpath";
                             push @rmcmd,$cmd;
                         }
                  } 
                } 
                   $suc=open(FILE,"<".$fpath);
                   $ntuple->{Status}="Success";
                     if(not $suc){
                         if($rcp){
                            die "  unabled to open file  $fpath \n";
                         }
                     }
            }
#
# Only do copy if not validated
# 
                   if($ntuple->{Status} ne "Validated" or 1){
                   my $retcrc=0;
                   if($rcp){
                    $retcrc = $self->calculateCRC($fpath,$ntuple->{crc},0);
                  }
                   print FILEV "calculateCRC($fpath, $ntuple->{crc}) : Status : $retcrc \n";
                   if ($retcrc != 1) {
                       $self->amsprint("********* validateRuns - ERROR- crc status :",666);
                       $self->amsprint($retcrc,0);
                       $BadCRCi[0]++;
                       $BadDSTs[0]++;
                       $bad++;
                    #   die "  bad crc \n";
                        push @cpntuples, $fpath;
                        
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
                    else{ #4
                         $i=($i>>8);
                         if(int($i/128)){
                          $events=0;
                          $badevents="NULL";
                          $status="Bad".($i-128);
                          $bad++;
                          $levent -= $ntuple->{LastEvent}-$ntuple->{FirstEvent}+1;
                         }
                          else{ #5
                           $status="OK";
                           $events=$ntuple->{EventNumber};
                           $badevents=int($i*$ntuple->{EventNumber}/100);
                           $validated++;
                           my $jobid = $run->{Run};
                           my($outputpatha,$rstatus) = $self->doCopy($jobid,$fpath,$ntuple->{crc},$ntuple->{Version},$outputpath,$run->{DataMC});
                           $outputpath=$outputpatha;   
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
                                               1,0,$run->{DataMC});
                           print FILEV "insert : $run->{Run}, $outputpath, $status, $ntuple->{size} \n";
                           $copied++;
                           $gbDST[0] = $gbDST[0] + $ntuple->{size};
                           push @cpntuples, $fpath;
#                           $runupdate = "UPDATE runs SET FEVENT = $fevent, LEVENT=$levent, ";
                         } else { #6
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
                      else{ #7

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
                                               $fpath,
                                               $ntuple->{crc},
                                               $ntuple->{Insert},
                                               1,0,$run->{DataMC});
                           print FILEV "insert : $run->{Run}, $fpath, $status, $ntuple->{size} \n";
                           $copied++;
                           $gbDST[0] = $gbDST[0] + $ntuple->{size};
#                           $runupdate = "UPDATE runs SET FEVENT = $fevent, LEVENT=$levent, ";


                       }
             } # ntuple ->{Status} == "Validated"
         } #loop for ntuples
         my $status='Failed';
         if ($copyfailed == 0) {
          $warn = "IIValidation done : *TRY* send Delete to DBServer, Run =$run->{Run} \n";
          print FILEV "$warn \n";
          $self->printWarn($warn);
#--          DBServer::sendRunEvInfo($self->{dbserver},$run,"Delete");
      
          if ($#cpntuples >= 0 && $#mvntuples>=0) { $status = "Completed";}
           elsif($#mvntuples>=0){

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
               $sql = "DELETE ntuples WHERE jid=$run->{uid}";
               $self->{sqlserver}->Update($sql);
               $runupdate = "UPDATE runs SET ";
           }
                 else{
                   $status = "Completed";
               }
           if ($status eq "Completed") {
               $GoodRuns[0]++;
           } elsif ($status eq "Failed") {
               $BadRuns[0]++;
           }

          $sql = "update runs set STATUS='$status' WHERE run=$run->{Run}";
            warn "executing $sql \n";
               $self->{sqlserver}->Update($sql);
#
# Check run lastevent
#

                         $sql="select sum(ntuples.levent-ntuples.fevent+1),min(ntuples.fevent)  from ntuples,runs where ntuples.run=runs.run and runs.run=$run->{Run}";
                          my $r4=$self->{sqlserver}->Query($sql);
                          my $ntevt=$r4->[0][0];
                          $fevt=$r4->[0][1];
                          if(not defined $ntevt){
                              $ntevt=0;
                          }
                          if( $ntevt ne $run->{LastEvent}-$run->{FirstEvent}+1){
                           warn "  ntuples/run mismatch $ntevt $run->{LastEvent}-$run->{FirstEvent}+1 $run->{Run} \n";
                       }
          if($ntevt>0){
                             $sql="UPDATE Runs SET fevent=$fevt, Levent=$ntevt-1+$fevt WHERE jid=$run->{Run}";

                             $self->{sqlserver}->Update($sql);
                      $sql="UPDATE Jobs SET realtriggers=$ntevt,timekill=0 WHERE jid=$run->{Run}";
                      $self->{sqlserver}->Update($sql);
                         }

          $warn = "Update Runs : $sql\n";

          foreach my $ntuple (@cpntuples) {
           my $cmd="rm  $ntuple";
           if ($rmprompt == 1) {$cmd = "rm -i $ntuple";}
            foreach my $mn (@mvntuples){
              if($ntuple eq $mn){
                $cmd=" ";
                 last;
              }
            }
           system($cmd);
           $warn = "Validation done : $cmd \n";
           print FILEV $warn;
           $self->printWarn($warn);
       }


          print FILEV $warn;
           if ($webmode == 1) {
              htmlWarning("validateRuns",$warn);
          } else { #8 
           $self->printWarn($warn);
          }
          if ($status eq "Completed" ) {
           $self->updateRunCatalog($run->{Run});
           $warn = "Update RunCatalog table : $run->{Run}\n";
           foreach my $cmd (@rmcmd){
               my $i=system($cmd);
               if($i){
                   warn " Remote command $cmd failed \n";
               }
           }
           $#rmcmd=-1;
           foreach my $cmd (@rmbad){
               my $i=system($cmd);
               if($i){
                   warn " Remove bad ntuples command $cmd failed \n";
               }
           }
           $#rmbad=-1;
           print FILEV $warn;
       }
          else{
              print "runcatalog not updated $status $run->{Run} $run->{Status} \n";
          }
      }
      }# events != 0 && errors != 0
     } # remote job
    }  # job found
    }
  }   # loop for runs from 'server'

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
          $self->resetFilesProcessingFlag();
 if ($webmode == 1) {$self->InfoPlus($warn);}
 $self->{ok}=1;


if($delete){
    foreach my $run (@{$self->{dbserver}->{rtb}}){
           if($run->{Status} eq 'Finished' or $run->{Status} eq 'Foreign' or $run->{Status} eq 'Canceled'){
               my $sql=" select ntuples.path from ntuples,runs where ntuples.run=runs.run and runs.status='Completed' and runs.jid=$run->{Run}";
               my $ret=$self->{sqlserver}->Query($sql);
               if(defined $ret->[0][0]){
                  print "  deleting  $run->{Run} \n";
    my $hostname=hostname();
    if($#{$self->{arpref}} <0 and (not ($self->{hostdbfile}=~/$hostname/))){
       $self->ServerConnect($ver);
    }
    if($#{$self->{arpref}} >=0){
        my $ard=${$self->{arpref}}[0];
          my %nc=%{$run};
try{
                $ard->sendRunEvInfo(\%nc,"Delete");
}
            catch CORBA::SystemException with{
                $self->ErrorPlus( "sendback corba exc while deleting run $run->{Run}");
            };
    }
     else{
                  
                  DBServer::sendRunEvInfo($self->{dbserver},$run,"Delete");
}
              }
              elsif($run->{Status} eq 'Finished'){
                 $run->{Status}='ToBeRerun';
                  print "  not deleting, updating  $run->{Run} \n";
#                 DBServer::sendRunEvInfo($self->{dbserver},$run,"Update");
              }
                   
}
}
}
}

sub doCopy {
     my $self = shift;
     my $jid  = shift;
     my $inputfile = shift;
     my $crc       = shift;
     my $version   = shift;
     my $outputpath=shift;
     my $datamc=shift;
     my $path='/MC';
     if(defined $datamc){
         if($datamc==1){
             $path='/Data'
             }
     }
#
     my $sql   = undef;
     my $cmd   = undef;
     my $cmdstatus = undef;
#
     my $time0  = time();
     my $time00 = 0;      # begin CRC calc

     my ($dbv, $gbv, $osv)       = $self->getDSTVersion($version);
     my ($period,$prodStartTime,$periodId) = $self->getActiveProductionPeriodByVersion($dbv);
     if (not defined $period || $period == $UNKNOWN || $prodStartTime == 0) {
         die "Cannnot find Active Production Period for DB version $dbv \n";
     }
     
     if(defined $outputpath and not (($outputpath=~$path) or ($outputpath=~$period) or ($outputpath=~/\.root$/ ))){
         $outputpath=$outputpath.$path."/$period/d1/d2/1.root";
     }

     $doCopyCalls++;

#
#     if ($verbose == 1) {print "doCopy : $inputfile \n";}
#

     my @junk = split '/',$inputfile;
     my $file = $junk[$#junk];

     my $outputdisk => undef;
     my $filesize    = (stat($inputfile))[7];
     if ($filesize > 0 or $inputfile=~/^\/castor/) {
# get output disk
         if(not defined $outputpath){
      my($outputpatha, $gb) = $self->getOutputPath($period,$path);
      $outputpath=$outputpatha;
         if ($outputpath =~ 'xyz' || $gb == 0) {
             if ($webmode == 0) {
              print "doCopy -E- Cannot find any disk to store DSTs. Exit";
              die;
             }
         }
  }
         else{
             my @junk=split '/',$outputpath;
             $outputpath="";
             for my $i (0...$#junk-3){
                 if($i>0){
                     $outputpath=$outputpath."/";
                 }
                 $outputpath=$outputpath."$junk[$i]";
             }
         }
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
              if($outputpath=~/^\/castor/){
                  $cmd="/usr/bin/nsmkdir -p $outputpath";
              }   
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
                               $ntuple->{crc},$ntuple->{Insert},1,0,$run->{DataMC});
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
    my $ronly=shift;
#sqlserver
    $self->{sqlserver}=new DBSQLServer();
    if(defined $ronly and $ronly==1){
     $self->{sqlserver}->ConnectRO();
   }
    else{
     $self->{sqlserver}->Connect();
    }
#
   my $dir=$ENV{AMSDataDirRW};
   if (defined $dir){
     $self->{AMSDataDir}=$dir;
   }
    else{
     $self->{AMSDataDir}="/afs/cern.ch/ams/Offline/AMSDataDir";
     $ENV{AMSDataDirRW}=$self->{AMSDataDir};
    }
     if (not defined $self->{AMSSoftwareDir}) {
         $dir =  $self->getDefByKey("AMSSoftwareDir");
         if (defined $dir) {
          $self->{AMSSoftwareDir}="$self->{AMSDataDir}/".$dir;
         } else {
             die "Cannot get def for key : AMSSoftwareDir \n";
         }
     }

    $TestCiteId = $self->getTestCiteId();
}

sub ConnectDB{

    my $self = shift;
    if( not $self->Init(1)){
        return 0;
    }
    else{
        $self->{ok}=1;
    }
    $TestCiteId = $self->getTestCiteId();
}

sub Connect{
    my $t0 = time();

    my $self = shift;
    if( not $self->Init()){
        return 0;
    }
    else{
        $self->{ok}=1;
    }
    # Flag to indicate if using /eosams/ as NTDIR
    $self->{eos} = 1;
         my $cem=lc($self->{q}->param("CEM"));
         if (defined $cem and not ($self->{q}->param("queryDB04"))){
          if(not $self->findemail($cem)){
 #          $self->ErrorPlus("Client $cem Not Found. Please Register...");
          }
          else{
             my $save="$self->{UploadsDir}/$self->{CEMID}.save2";
              store(\@{$self->{DataSetsT}},$save);
              $self->{FEM2}=$save;
          }
         }

 ;
    my $ti = time();
    $self->getProductionPeriods(0);
    my $ProductionPeriod = 'tmp';
#
 my    $insertjobsql = undef;
#
#understand parameters

    $self->{read}=0;
    my $q=$self->{q};
    my $sql="";
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
                            elapsed, cites.name, content, jobs.timestamp, jobs.jid, jobs.realtriggers
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
                            elapsed, cites.name, content, jobs.timestamp, jobs.jid, jobs.realtriggers
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
             my $datarun="runs";
        if (defined $ret->[0][0]) {
         foreach my $r (@{$ret}){
             my $jobname =trimblanks($r->[0]);
             my $triggers=$r->[1];
              if($r->[11]>0){
               $triggers=$r->[11];
                 
              }
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
             $sql = "SELECT status, submit FROM $datarun WHERE jid=$jid";
             $ret=$self->{sqlserver}->Query($sql);
             if (defined $ret->[0][0]) {
              foreach my $r (@{$ret}){
                  $status    = trimblanks($r->[0]);
                  $timestamp = $r->[1];
              }
          }
             else{
             $datarun="dataruns";
             $sql = "SELECT status, submit FROM $datarun WHERE jid=$jid";
             $ret=$self->{sqlserver}->Query($sql);
             if (defined $ret->[0][0]) {
              foreach my $r (@{$ret}){
                  $status    = trimblanks($r->[0]);
                  $timestamp = $r->[1];
              }
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
              if ($cite ne 'Any') {
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
        $sql = "SELECT run, fevent, levent, submit FROM $datarun WHERE jid=$jobid";
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
                       WHERE  run = $run and jid=$jobid";
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
        my $datamc="";
        if(defined $q->param("QTypeD") and $q->param("QTypeD") eq "Data"){
          $datamc="data";
        }
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
                $sql = "SELECT run, jid, fevent, levent, submit, status FROM $datamc"."Runs
                          WHERE run>=$runmin AND run<=$runmax
                          ORDER BY run";
            } else {
             $runid =  trimblanks($q->param("RunID"));
             $title = $title.$runid;
              $sql = "SELECT run, jid, fevent, levent, submit, status FROM $datamc"."Runs
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
       my $datamc=0;
        if(defined $q->param("QTypeD") and $q->param("QTypeD") eq "Data"){
          $datamc=1; 
        }
        if (defined $q->param("DSTID")) {
         $self->htmlTemplateTable($title);
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<td align=center><b><font color=\"blue\">Job </font></b></td>";
               print "<td align=center><b><font color=\"blue\">Run </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Updated</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >FilePath </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Size(MB) </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Events</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Errors </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Status </font></b></td>";
               print "</tr>\n";
            if ($q->param("DSTID") =~ /-/) {
                ($runmin,$runmax) = split '-',$q->param("DSTID");
                $title = $title.$q->param("RunID");
                $sql = "SELECT jid, run, path, timestamp, nevents, neventserr, status,sizemb
                          FROM ntuples
                          WHERE run>=$runmin AND run<=$runmax and datamc=$datamc 
                          ORDER BY run";
            } else {
             $runid =  trimblanks($q->param("DSTID"));
             $title = $title.$runid;
             if(not $runid =~/^\d+$/){
                $runmin=0;
                $runmax=2000000000;                
                $sql = "SELECT jid, run, path, timestamp, nevents, neventserr, status,sizemb
                          FROM ntuples
                          WHERE run>=$runmin AND run<=$runmax and datamc=$datamc 
                          ORDER BY run";
             }
             else{
             $sql = "SELECT jid, run, path, timestamp, nevents, neventserr, status,sizemb
                      FROM ntuples  WHERE run=$runid and datamc=$datamc";
            }
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
             my $crc=$r->[7];
             my $color     = statusColor($status);
             print "<td><b> $jid </td></b>
                    <td><b> $run </td>
                    <td><b> $starttime </b></td>
                    <td><b> $path </b></td>
                    <td><b> $crc </b></td>
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



    if ($self->{q}->param("getDataFileID")) {
     $self->{read}=1;
     if ($self->{q}->param("getDataFileID") eq "Submit") {
        htmlTop();
        my $title = "DataFiles List ";
        my $runid = 0;
        my $runmin= 0;
        my $runmax= 0;
        if (defined $q->param("DataFileID")) {
         $self->htmlTemplateTable($title);
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<td align=center><b><font color=\"blue\">Run </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >FilePath </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >StartTime </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Events</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Type </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >SizeMB </font></b></td>";
              print "<td align=center><b><font color=\"blue\" >OrigFiles </font></b></td>";

         my $pp=" ";
         my $pid=$q->param("QPPer");
         if($pid==0){
         }
         elsif($pid>0){
             my $sql1="select end,begin from productionset where did=$pid";
              my $ret = $self->{sqlserver}->Query($sql1);
              if(defined $ret->[0][0]){
               if($ret->[0][0]){    
               $pp=" and fetime<$ret->[0][0] and letime>=$ret->[0][1]  ";
               }
               else{
                $pp=" and letime>=$ret->[0][1]  ";
               }
              }             
         }
         else{
             my $sql1="select end,begin from productionset where status='Active'and name like '%AMS02%'";
              my $ret = $self->{sqlserver}->Query($sql1);
              my $beg=2000000000;
              my $end=0;
              if(defined $ret->[0][0]){
                foreach my $r (@{$ret}){
                   if($beg>$r->[1]){
                    $beg=$r->[1];
                   }
                }
                $pp=" and letime>=$beg  ";
             
            }
         }
         my $type="";
            if ($q->param("DataFileType") =~ /ANY/) {
             }                         
         else{
             my $dt=$q->param("DataFileType");
             $type=" and type like '%$dt%' ";
         }
               print "</tr>\n";
         my $showdst=0;
         if($q->param("ShowDST")){
            $showdst=1;
         }
         my $types="";
            if ($q->param("DataFileTypeS") =~ /ANY/) {
             }                         
         else{
             my $dts=$q->param("DataFileTypeS");
             my $dt="";
             if($dts=~/S/){
                 $type=$type." and type like '% 1%S%' ";
             }
             if($dts=~/T/){
                 $type=$type." and type like '% 1%T%' ";
             }
             if($dts=~/E/){
                 $type=$type." and type like '% 1%E%' ";
             }
             if($dts=~/U/){
                 $type=$type." and type like '% 1%U%' ";
             }
             if($dts=~/R/){
                 $type=$type." and type like '% 1%R%' ";
             }
             if($dts=~/L/){
                 $type=$type." and type like '% 1%L%' ";
             }
             if($dts=~/V/){
                 $type=$type." and type like '% V%3%' ";
             }
         }
               print "</tr>\n";
            my $sqld= "";
         if($q->param("DataFileID") =~ /now-/ or $q->param("DataFileID") =~ /now -/){
             my   ($now,$hrs) = split '-',$q->param("DataFileID");
             $runmax=time();
             $runmin=$runmax-$hrs*3600;
                $sql = "SELECT run, path,fetime, nevents, type, sizemb,pathb,paths,status,tag 
                          FROM datafiles
                          WHERE run>=$runmin AND run<=$runmax $type $pp
                          ORDER BY run";

         }
         else{
            if ($q->param("DataFileID") =~ /-/) {
                ($runmin,$runmax) = split '-',$q->param("DataFileID");
                $title = $title.$q->param("RunID");
                $sql = "SELECT run, path,fetime, nevents, type, sizemb,pathb,paths,status,tag 
                          FROM datafiles
                          WHERE run>=$runmin AND run<=$runmax $type $pp
                          ORDER BY run";
                    
            } else {
             $runid =  trimblanks($q->param("DataFileID"));
             if(not $runid =~/^\d+$/){
#               $self->ErrorPlus("Run no $runid is not digit");
                $runmin=0;
                $runmax=2000000000;                
                $sql = "SELECT run, path,fetime, nevents, type, sizemb,pathb,paths,status ,tag
                          FROM datafiles
                          WHERE run>=$runmin AND run<=$runmax $pp $type 
                          ORDER BY run";
             }
             else{
             $title = $title.$runid;
                $sql = "SELECT run, path,fetime, nevents,  type, sizemb,pathb,paths,status ,tag 
                          FROM datafiles
                          WHERE run=$runid $pp";
         }
         }
        }
         my $ret=$self->{sqlserver}->Query($sql);
         if (defined $ret->[0][0]) {
          foreach my $r (@{$ret}){
             my $run       = $r->[0];
             my $path      = trimblanks($r->[1]);
             my $starttime = EpochToDDMMYYHHMMSS($r->[2]);
             my $nevents   = $r->[3];
             my $type   = $r->[4];
             my $sizemb   = $r->[5];
             my $opath=$r->[6];
             my $paths=$r->[7];
             my $status=$r->[8];
             my $tag=$r->[9];
             my $runx=sprintf("%x",$run);
             my $tagx=sprintf("%04x",$tag);
             print "<td><b> $run / $runx / $tagx </td></b>
                    <td><b> $paths  </td>
                    <td><b> $starttime </b></td>
                    <td align=middle><b> $nevents </b></td>
                    <td align=middle><b> $type / $status </b></td>
                    <td><b> $sizemb </b></td>
                    <td><b> $opath </b></td>\n";
             print "</font></tr>\n";
             if($showdst){
               $sqld="select path from ntuples where run=$run and datamc=1";
               my $retld=$self->{sqlserver}->Query($sqld);
               foreach my $ntuple(@{$retld}){
                             print "<td><b> $ntuple->[0] </td></b>\n";
                              print "</font></tr>\n";

               } 
              } 
         }
      }
       htmlTableEnd();
      htmlTableEnd();
     }
    htmlBottom();
    }
 }
# getDataFileID end here
#



#queryDB04
   if ($self->{q}->param("queryDB04")) {
     $self->{read}=1;

     $self->updateAllRunCatalog();



     my @tempnam=();
     $#tempnam=-1;
     my $hash={};
     my @desc=();
     $#desc=-1;
     my $cite={};
     my @runs=();
     my @jids=();
     my @jidsnt=();
     my @submits=();
     my @jobnames=();
     my @datasets=();
     $#datasets=-1;
     $#jobnames=-1;
     $#submits=-1;
     $#runs=-1;
     my $qtemplate  = undef;
     my $qparticle  = undef;
     my $qmomentumI = undef;
     my $qmomentumA = undef;
     my $qtrigger   = undef;

    if ($self->{q}->param("queryDB04") eq "DoQuery") {
        my $datamc=0;
      my $sql     = undef;
      my $sqlNT   = undef; # will be used to get NT paths for ROOT file
      my $dataset = undef;
      my $particle= undef;
      my $sqlmom="";
      my $sqlamom="";
      my $sqlmom1="";
      my $sqlamom1="";
#
#   vc   -  remove monstrous loop in summary
#
      my $sqlsum=  "SELECT COUNT(ntuples.RUN), SUM(ntuples.SIZEMB), SUM(ntuples.NEVENTS), sum(ntuples.LEvent-ntuples.FEvent+1) FROM Ntuples, runs,jobs,runcatalog where ntuples.datamc=$datamc";
      my $rsum=undef;
#
# Template is defined explicitly
#
       my $qpp=$q->param("QPPer");
       my $pps="";
       if($qpp>0){
         $pps=$pps." and jobs.pid=$qpp ";
       }
       elsif($qpp==-1){
#         active
         my $sqlaa = "SELECT  DID  FROM ProductionSet WHERE STATUS='Active' ORDER BY DID";
         my $ret = $self->{sqlserver}->Query($sqlaa);
         $pps=undef;
         foreach my $pp  (@{$ret}){
          if(defined $pps){
            $pps=$pps." or jobs.pid =";
          }
          else{
           $pps=" and ( jobs.pid =";
          }
         $pps=$pps." $pp->[0] ";
        }
       if(defined $pps){
        $pps=$pps." ) ";
       }
       else{
        $pps="";
       }
  }



      if (defined $q->param("QTempDataset") and $q->param("QTempDataset") ne "Any") {

       $dataset = $q->param("QTempDataset");

       $dataset = trimblanks($dataset);
       $qtemplate = $dataset;
#- 20.06.05 a.k.       $dataset =~ s/ /\% /g;
       $sql = "SELECT runs.run, jobs.jobname, runs.submit FROM runs, jobs, runcatalog
                   WHERE runs.jid=jobs.jid  AND runs.status='Completed'";
                   $sqlmom=" AND
                        (runcatalog.jobname LIKE '%$dataset%' AND runcatalog.run=runs.run) ";
                   $sqlamom=" AND
                        (runcatalog.jobname  not LIKE '%$dataset%' AND runcatalog.run=runs.run)";


           if ($q->param("QBuildNum")){
               my @buildNumber= buildnum_string($q->param("QBuildNum"));
               my $buildnum_min = $buildNumber[0];
               my $buildnum_max = $buildNumber[1];
               $sqlmom1=$sqlmom1." AND (Ntuples.buildno>=$buildnum_min AND Ntuples.buildno<=$buildnum_max) ";
               $sqlamom1=$sqlamom1." And (Ntuples.buildno<$buildnum_min OR Ntuples.buildno>$buildnum_max) ";
           }


       $sqlNT = "SELECT Ntuples.path, Ntuples.run, Ntuples.nevents, Ntuples.neventserr,
                        Ntuples.timestamp, Ntuples.status, Ntuples.sizemb, Ntuples.castortime,ntuples.levent,ntuples.fevent,ntuples.jid
                 FROM runs, jobs, runcatalog, ntuples
                   WHERE runs.jid=jobs.jid and runs.run=ntuples.run  AND runs.status='Completed'";
       $sql=$sql.$sqlmom;
       $sqlNT=$sqlNT.$sqlmom;

# check TriggerType
       if (defined $q->param("QTrType") and $q->param("QTrType") ne "Any") {
           my $trtype = trimblanks($q->param("QTrType"));
           $qtrigger = $trtype;
           $sql=$sql." AND (runs.run = runcatalog.run AND runcatalog.trtype='$trtype') ";
           $sqlNT=$sqlNT." AND (runs.run = runcatalog.run AND runcatalog.trtype='$trtype') ";

       }
#

      my @garbage= split /WHERE/,$sql;
       my $rf="NFS";
         if (defined $q->param("ROOTACCESS")) {
             $rf = $q->param("ROOTACCESS");
         } 
      if($#garbage>0){
          my $addf="";
        if($rf=~/^NFS/ and $sqlsum=~/ntuples/){
            $addf=" and ntuples.path not like '%castor%'";
        }

        $sqlsum=$sqlsum." and ".$garbage[1].$pps." and ntuples.run=runs.run".$addf;
           $rsum=$self->{sqlserver}->Query($sqlsum);
        # die "$sqlsum $rsum->[0][0] $rsum->[0][1] $rsum->[0][2] ";

     }

       $sql = $sql.$pps."ORDER BY Runs.Run";
      $sqlmom=$sqlmom.$sqlmom1;
      $sqlamom=$sqlamom.$sqlamom1;
       $sqlNT = $sqlNT.$sqlmom1." ".$pps."ORDER BY Runs.Run";

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
         my $versioncond = '';
         if (defined $q->param("Version")) {
             $versioncond = " AND VERSION = '".$q->param("Version")."'";
         }
         $qparticle = $particle;
         $sql = " SELECT DID FROM Datasets WHERE NAME LIKE '$particle' $versioncond";
         my $r0=$self->{sqlserver}->Query($sql);
         if (defined $r0->[0][0]) {
          foreach my $r (@{$r0}){
           my $did = $r->[0];
           $sql  = "SELECT Runs.Run, Jobs.JOBNAME, Runs.SUBMIT
                    FROM Runs, Jobs, runcatalog
                     WHERE Jobs.DID=$did AND Jobs.JID=Runs.JID and
                            Runs.run=runcatalog.run AND Runs.Status='Completed'";
      $sqlNT = "SELECT Ntuples.path, Ntuples.run, Ntuples.nevents, Ntuples.neventserr,
                        Ntuples.timestamp, Ntuples.status, Ntuples.sizemb, Ntuples.castortime,ntuples.levent,ntuples.fevent
                    FROM Runs, Jobs, runcatalog, NTuples
                     WHERE Jobs.DID=$did AND Jobs.JID=Runs.JID AND
                            Runs.run=Ntuples.run AND
                            Runs.run=runcatalog.run AND Runs.Status='Completed'";
           if ($q->param("QBuildNum")){
               my @buildNumber= buildnum_string($q->param("QBuildNum"));
               my $buildnum_min = $buildNumber[0];
               my $buildnum_max = $buildNumber[1];
               $sqlmom1=$sqlmom1." AND (Ntuples.buildno>=$buildnum_min AND Ntuples.buildno<=$buildnum_max) ";
               $sqlamom1=$sqlamom1." AND (Ntuples.buildno<$buildnum_min OR Ntuples.buildno>$buildnum_max) ";
           }


# check TriggerType
           if (defined $q->param("QTrType") and $q->param("QTrType") ne "Any") {
            my $trtype = trimblanks($q->param("QTrType"));
            $qtrigger = $trtype;
            $sql=$sql." AND (runs.run = runcatalog.run AND runcatalog.trtype='$trtype') ";
            $sqlNT=$sqlNT." AND (runs.run = runcatalog.run AND runcatalog.trtype='$trtype') ";
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
                 $sqlmom = " and (runs.run = runcatalog.run AND runcatalog.PMIN >= $momentumMin AND runcatalog.PMAX <= $momentumMax) ";
                 $sqlamom=" and (runs.run = runcatalog.run AND (runcatalog.pmin< $momentumMin OR runcatalog.PMAX > $momentumMax))";
                 $sql=$sql.$sqlmom; 
                 $sqlNT = $sqlNT.$sqlmom;
               }
           }
#
       

       my $rf="NFS";
         if (defined $q->param("ROOTACCESS")) {
             $rf = $q->param("ROOTACCESS");
         } 

       my @garbage= split /WHERE/,$sql;
        if($#garbage>0){
          my $addf="";
        if($rf=~/^NFS/ and $sqlsum=~/ntuples/){
            $addf=" and ntuples.path not like '%castor%'";
        }
        $sqlsum=$sqlsum." and ".$garbage[1].$pps." and ntuples.run=runs.run".$addf;
#         $sqlsum=$sqlsum." where ".$garbage[1].$pps." and ntuples.run=runs.run";           $rsum=$self->{sqlserver}->Query($sqlsum);
         # die "$sqlsum $rsum->[0][0] $rsum->[0][1] $rsum->[0][2] ";
      }
            $sql = $sql.$pps." ORDER BY Runs.Run";
            $sqlNT = $sqlNT.$pps." ORDER BY Runs.Run";
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
        $sqlNT = "SELECT Ntuples.path, Ntuples.run, Ntuples.nevents, Ntuples.neventserr,
                         Ntuples.timestamp, Ntuples.status, Ntuples.sizemb, Ntuples.castortime,ntuples.levent,ntuples.fevent
                    FROM Runs, Jobs, runcatalog, Ntuples
                     WHERE
                        Runs.JID=Jobs.JID AND
                         Runs.Status='Completed' AND
                          Runs.run = runcatalog.run AND
                           Runs.run = Ntuples.run ";

           if ($q->param("QBuildNum")){
               my @buildNumber= buildnum_string($q->param("QBuildNum"));
               my $buildnum_min = $buildNumber[0];
               my $buildnum_max = $buildNumber[1];
               $sqlmom1=$sqlmom1." AND (Ntuples.buildno>=$buildnum_min AND Ntuples.buildno<=$buildnum_max) ";
               $sqlamom1=$sqlamom1." AND (Ntuples.buildno<$buildnum_min OR Ntuples.buildno>$buildnum_max) ";
           }


# check TriggerType
           if (defined $q->param("QTrType") and $q->param("QTrType") ne "Any") {
            my $trtype = trimblanks($q->param("QTrType"));
            $sql=$sql." AND (runs.run = runcatalog.run AND runcatalog.trtype='$trtype') ";
            $sqlNT=$sqlNT." AND (runs.run = runcatalog.run AND runcatalog.trtype='$trtype') ";
           }
# check Momentum
           if (defined $q->param("QMomI") and defined $q->param("QMomA")) {
               if ($q->param("QMomI") == 1 and $q->param("QMomA") == 10000) {
#              do nothing - defaults
               } else {
                 my $momentumMin = $q->param("QMomI");
                 my $momentumMax = $q->param("QMomA");
                 $sqlmom = " and (runs.run = runcatalog.run AND runcatalog.PMIN >= $momentumMin AND runcatalog.PMAX <= $momentumMax) ";
                 $sqlamom=" and (runs.run = runcatalog.run AND (runcatalog.pmin< $momentumMin OR runcatalog.PMAX > $momentumMax))";
                 $sql=$sql.$sqlmom;
                 $sqlNT = $sqlNT.$sqlmom;

               }
           }
#
            my @garbage= split /WHERE/,$sql;
       my $rf="NFS";
         if (defined $q->param("ROOTACCESS")) {
             $rf = $q->param("ROOTACCESS");
         } 
             if($#garbage>0){
          my $addf="";
        if($rf=~/^NFS/ and $sqlsum=~/ntuples/){
            $addf=" and ntuples.path not like '%castor%'";
        }
        $sqlsum=$sqlsum." and ".$garbage[1].$pps." and ntuples.run=runs.run".$addf;
#              $sqlsum=$sqlsum." where ".$garbage[1].$pps." and ntuples.run=runs.run";
              $rsum=$self->{sqlserver}->Query($sqlsum);
#             die "$sqlsum $rsum->[0][0] $rsum->[0][1] $rsum->[0][2] ";
          }
            $sql = $sql.$pps." ORDER BY Runs.Run";
            $sqlNT = $sqlNT.$pps." ORDER BY Runs.Run";
            my $r1=$self->{sqlserver}->Query($sql);
           #die " $sql $#{$r1} ";
            if (defined $r1->[0][0]) {
             foreach my $r (@{$r1}){
               push @runs,$r->[0];
               push @jobnames,$r->[1];
               push @submits,$r->[2];
              }
             }
    }
# now check output
      my $accessmode       = "xyz";
      my $remotecite       = "xyz";
      my $rootfile         = undef;
      my $rootfileaccess   = undef;

      if (defined $q->param("REMOTEACCESS")) {
          $accessmode="REMOTE";
          $remotecite = $q->param("REMOTEACCESS");
      }
      if (defined $q->param("NTOUT")) {
       $self->htmlTop();
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

       if ($q->param("NTOUT") eq "ROOT") {
        $rootfile = $q->param("ROOT");
        if (not $rootfile =~ /\//  ) {
            my $ctime = time();
            $rootfile = "/tmp/".$rootfile.$ctime;
        }
        print "<tr><td><b><font size=\"4\" color=\"blue\">ROOT script  ";
        print "</font></b></td>";
         if (defined $q->param("ROOTACCESS")) {
             $rootfileaccess = $q->param("ROOTACCESS");
         } else {
             $rootfileaccess = "NFS";
         }
         print "<td><b><font size=\"4\" color=\"black\">$rootfile </font></b><i> (access via $rootfileaccess) </i></td></tr>\n";
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
              my $i       = 0;
              my $runold  = 0;
              my $run     = 0;
              $sql = $sqlNT;
              my $r0=$self->{sqlserver}->Query($sql);
              if (defined $r0->[0][0]) {
               foreach my $nt (@{$r0}){
                my $printit = 1;
                $run = $nt->[1];
                if ($run != $runold) {
                 if ($accessmode eq "REMOTE") {
                  $sql = "SELECT run FROM MC_DST_COPY WHERE run=$run AND cite='$remotecite'";
                  my $r0=$self->{sqlserver}->Query($sql);
                  if (not defined $r0->[0]) {
                   $printit = 0;
                  }
                 }
                 if ($printit == 1) {
                  $i = 0;
                  foreach my $r (@runs) {
                   if ($r == $run) {
                    my $jobname = $jobnames[$i];
                    my $submit  = $submits[$i];
                    print "<tr><td><b><font size=\"3\" color=$color> Job : $jobname, Run : $run, Submitted : $submit";
                    print "</font></b></td></tr>\n";
                    if ($runold != 0) {            htmlTableEnd();}
                    print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
                    print "<table border=1 width=\"100%\" cellpadding=0 cellspacing=0>\n";
                    print "<tr><td width=10% align=left><b><font color=\"blue\" > NTuple </font></b></td>";
                    print "<td width=10%><b><font color=\"blue\"> Events </font></b></td>";
                    print "<td width=15%><b><font color=\"blue\" > Triggers </font></b></td>";
                    print "<td width=15%><b><font color=\"blue\" > Size[MB] </font></b></td>";
                    print "<td td align=middle><b><font color=\"blue\" > Produced </font></b></td>";
                    print "<td width=10%><b><font color=\"blue\" > Status </font></b></td>";
                    print "</tr>\n";
                   }
                   $i++;
                  }
                 }
                 $runold = $run;
                }
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
                     my $ntr=$nt->[8]-$nt->[9]+1;
                  my $timel =localtime($nt->[4]);
                  my ($wday,$mon,$day,$time,$year) = split " ",$timel;
                  my $status=$nt->[5];
                  my $color=statusColor($status);
                  print "<td width=50%><b> $path    </td></b><td><b> $nt->[2] </td>
                        <td align=middle width=5%><b> $ntr </b></td>
                        <td align=middle width=5%><b> $nt->[6] </b></td>
                        <td align=middle width=25%><b> $mon $day, $time, $year </b></td>
                        <td align=middle width=10%><b><font color=$color> $status </font></b></td> \n";
                 print "</font></tr>\n";
              }
            }
            htmlTableEnd();
            print "<BR><BR>\n";
           }
   } elsif ($q->param("NTOUT") eq "RUNS") {
# ... print Runs
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     print "<td><b><font color=\"blue\">Job </font></b></td>";
     print "<td><b><font color=\"blue\" >Run </font></b></td>";
     print "<td><b><font color=\"blue\" >Job Submit Time </font></b></td>";
     print "<td><b><font color=\"blue\" >Path </font></b></td>\n";
     print "</tr>\n";
     my $color="black";
     my $i =0;
     foreach my $run (@runs){
        my $jobname = $jobnames[$i];
        my $submit  = $submits[$i];
        my $path="";
        my $printit = 1;
        if ($accessmode eq "REMOTE") {
#        $sql = "SELECT run,prefix,path FROM MC_DST_COPY WHERE run=$run AND cite='$remotecite'";
        $sql = "SELECT run FROM MC_DST_COPY WHERE run=$run AND cite='$remotecite'";
        my $r0=$self->{sqlserver}->Query($sql);
        if (not defined $r0->[0]) {
            $printit = 0;
        }#else{
#                foreach my $pathPart (@{$r0}) {
#                    $path="$path $r0->[1]/$r0->[2];";
#                }
#            }
#        }else{
#            $sql = "SELECT path From Ntuples WHERE Run=$run";
#            my $r1=$self->{sqlserver}->Query($sql);
#            if (defined $r1->[0]) {
#                foreach my $pathPart (@{$r1}) {
#                    $path="$path $pathPart->[0];";
#                }
#            }
         }
         $sql = "SELECT path From Datafiles WHERE Run=$run";
         my $r2=$self->{sqlserver}->Query($sql);
         if (defined $r2->[0]) {
#             foreach my $pathPart (@{$r2}) {
           $path="$r2->[0][0]";
 #            }
         }
         if ($printit == 1) {
            $i++;
            print "
            <td><b><font color=$color> $jobname&nbsp;&nbsp;&nbsp; </font></td></b>
            <td><b><font color=$color> $run&nbsp;&nbsp;&nbsp; </font></b></td>
            <td><b><font color=$color> $submit&nbsp;&nbsp;&nbsp; </font></b></td>
            <td><b><font color=$color> $path </font></b></td>\n";
            print "</font></tr>\n";
        }
     }
   htmlTableEnd();
  } elsif ($q->param("NTOUT") eq "FILES") {
# ... print DSTs
    print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
    print "<td><b><font color=\"blue\" >Job </font></b></td>";
    print "<td><b><font color=\"blue\" >Run </font></b></td>";
    print "<td><b><font color=\"blue\" >Job Submit Time </font></b></td>\n";
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
            my $submit  = $submits[$i];
            $i++;
            $sql = "SELECT path From Ntuples WHERE Run=$run";
            my $r1=$self->{sqlserver}->Query($sql);
            print "
            <td><b><font color=$color> $jobname&nbsp;&nbsp;&nbsp; </font></td></b>
            <td><b><font color=$color> $run&nbsp;&nbsp;&nbsp; </font></b></td>
            <td><b><font color=$color> $submit&nbsp;&nbsp;&nbsp; </font></b></td>";
            print "<td><b><font color=$color>";
            foreach my $path (@{$r1}) {
                print " $path->[0]  ";
            }
            print "</font></td></b></font></tr>\n";
        }
    }
    htmlTableEnd();
   } elsif ($q->param("NTOUT") eq "SUMM") {
# ... print summary
#       die "nah";
        my @titles= (
        "Template",
        "Jobs",
        "DSTs",
        "DSTs GB",
        "Events",
        "Triggers",);
        my $query= $q->param('QPart');
        my $q2= $q->param("QTempDataset");
        if($q2 eq "Any"){
          $q2=undef;
        }
        my $buffer="Summary for $query  ";
        my @output=();
        my @temp=();
         my $sql;
        if($query eq "Any"){
          $titles[0]="Dataset";
          my $dmc=$q->param('DataMC');
          if(not defined $dmc){
              $dmc=0;
          }
           $sql = "SELECT datasetsdesc.dataset,datasetsdesc.did FROM datasets,DatasetsDesc where datasets.datamc=$dmc and datasets.did=datasetsdesc.did order by datasetsdesc.did ";
        }
        else{
         $sql="SELECT jobdesc FROM DatasetsDesc WHERE dataset='$query'"; 
        }
         my $rquery=$self->{sqlserver}->Query($sql);
         my $nruns=0;
        my $dsts=0;
        my $gb=0;
        my $evt=0;
        my $trg=0;
         my $prev="";
         foreach my $templat (@{$rquery}){
          $#temp=-1;
           my $like=trimblanks($templat->[0]);
           if($like eq $prev or  (defined $q2 and  $q2 ne $templat->[0])){
              next;
           }
           if($query eq "Any"){
             $sql=$sqlsum." and (jobs.did=$templat->[1]) and (jobs.jid=ntuples.jid)";
           }
           else{
            $sql=$sqlsum." and (runcatalog.jobname LIKE '%$like%' AND runcatalog.run=runs.run)";
           }
          
#             die "2 $query $templat->[0] $sql ";
          
#          my $sql22="select run from runs,jobs where jobs.jid=runs.jid and runs.status='Completed' order by jobs.timestamp desc";
#          my $r22=$self->{sqlserver}->Query($sql22);
#          my $i=0;
#          my $i0=19998;
#          for my $srun (@{$r22}){
#              if($i++>i0 and $i<$i0+17000){
#                 $self->updateRunCatalog($srun->[0]);
#           }
#          }          
          my $rsump=$self->{sqlserver}->Query($sql);
           my @sqla=split 'where',$sql;
#           $sqla[1]=~s/and ntuples.run=runs.run//;
#           $sqla[1]=~s/and ntuples.path not like \'\%castor\%\'//;
           $sql="select count(jobs.jid) from jobs,runcatalog,runs,ntuples where ".$sqla[1]." group by ntuples.run";
          my $rsuma=$self->{sqlserver}->Query($sql);
          my $i=0;
          foreach my $p (@{$rsuma}){
              $i++;
          }
          push @temp,$like;
          push @temp,$i;
          $nruns+=$i;
          $dsts+=$rsump->[0][0];
          $gb+=int($rsump->[0][1]/100)/10; 
          $evt+=$rsump->[0][2];
          $trg+=$rsump->[0][3];
          push @temp,$rsump->[0][0];
          push @temp,int($rsump->[0][1]/100)/10; 
          push @temp,$rsump->[0][2];
           push @temp,$rsump->[0][3];
            push @temp,"dum";
           push @output,[@temp];
           $prev=$like;
      }
          $#temp=-1;
           push @temp,"Total of";
          push @temp,$nruns;
        push @temp,$dsts;
        push @temp,$gb;
        push @temp,$evt;
        push @temp,$trg;
           push @temp,"dum";
           push @output,[@temp];

         monitorHTML::print_table($q,$buffer,$#titles,@titles,@output);
       

  } elsif ($q->param("NTOUT") eq "ROOT") {
#... write RootAnalysisTemplate
      my $RootAnalysisTextNFS =
         "// ROOT files accessed via NFS
         ";
      my $RootAnalysisTextCastor =
         "// it is assumed that CASTOR directory
          //   structure is similar to one on AMS disks.
          //   /castor/cern.ch/MC/AMS02/ProductionPeriod/...
         ";
      my $RootAnalysisTextHTTP =
         "// wildcards are not implemented yet in ROOT.
          //   still  have to check what is the HTTPD protocol for
          //   getting a list of files.
         ";

      my $RootAnalysisTextRemote =
         "// it is assumed that REMOTE directory structure and lib(s) path are similar to one on AMS disks.
         ";
      my $RootAnalysisTemplateTxt =
         "  gROOT->Reset();
          // for linuxicc load
          gSystem->Load(\"/Offline/vdev/lib/linuxicc/ntuple_slc4.so\");
          TChain chain(\"AMSRoot\");
      ";
        
#...... check files access option
      my $prefix     = "xyz";
          if ($rootfileaccess eq "HTTP") {
              $prefix     = "http://$self->{HTTPserver}";
          }
          if ($rootfileaccess eq "CASTOR") {
              $prefix     = "rfio:/castor/cern.ch/ams";
          }
#..... remote cite (if any)
      if (defined $q->param("REMOTEACCESS")) {
          $rootfileaccess="REMOTE";
          $remotecite = $q->param("REMOTEACCESS");
      }

    my $buff=undef;
    my @dirs=();
    $#{dirs} =-1;
    my @dirs_runs=();
    $#{dirs_runs} =-1;
    my @dirs_ntuples=();
    $#dirs_ntuples =-1;
    my @dirs_triggers=();
    $#{dirs_triggers} =-1;
    my @dirs_hr=();
     $#dirs_hr=-1;
      if ($rootfileaccess eq "HTTP") {
         $buff = $RootAnalysisTextHTTP;
         print "<tr><td>$RootAnalysisTextHTTP</td></tr><br>";
     } elsif ($rootfileaccess eq "CASTOR") {
         $buff = $RootAnalysisTextCastor;
         $RootAnalysisTextCastor=~s/\n/<br>/g;
         print "<tr><td>$RootAnalysisTextCastor</td></tr><br>";
     } elsif ($rootfileaccess eq "REMOTE") {
         $buff = $RootAnalysisTextRemote;
         print "<tr><td>$RootAnalysisTextRemote</td></tr><br>";
     } else {
         $buff = $RootAnalysisTextNFS;
         $RootAnalysisTextNFS=~s/\n/<br>/g;
         print "<tr><td>$RootAnalysisTextNFS</td></tr>\n";
     }
        
        $buff=$buff."\n";
         $buff=$buff.$RootAnalysisTemplateTxt."\n";
         $RootAnalysisTemplateTxt=~s/\n/<br>/g;
         print "<tr><td>$RootAnalysisTemplateTxt</td></tr>\n";
#
      my $sql =$sqlNT;
      my $r1=$self->{sqlserver}->Query($sql);
      if ($rootfileaccess=~/^NFS/ or $rootfileaccess eq "CASTOR") {
          my $oldrun=0;
          foreach my $nt (@{$r1}) {
           my $path=trimblanks($nt->[0]);
           if ($path =~ m/castor/ and $rootfileaccess=~/^NFS/) {
#           skip it, file has only archived copy 
           }
           elsif($rootfileaccess eq "NFSONLY" and $nt->[7]>0){
#           skip it, file has  archived copy 
           } elsif($rootfileaccess=~/^NFS/ or $nt->[7] > 0){
            my @junk = split '/',$path;
            my $tdir ="";
            my @jrun=split '\.',$junk[$#junk];
            my $run=$jrun[0]; 
            
            if($rootfileaccess eq "CASTOR"){
                $junk[1]=$prefix;
            }
            else{
             $junk[1]="/".$junk[1];
             }
               for (my $i=1; $i<$#junk; $i++) {
                my $sp="/";
                if($i==1){
                    $sp="";
                }
             $tdir = $tdir.$sp.$junk[$i];
            }
            $tdir = trimblanks($tdir);
            my $dirfound = -1;

            for my $i(0...$#dirs){
                
             if ($dirs[$i] eq $tdir) {
              $dirfound = $i;
              last;
          }
         }
         if ($dirfound >=0) {
# skip it
             $dirs_ntuples[$dirfound]++;
          $dirs_triggers[$dirfound]+=$nt->[8]-$nt->[9]+1;
             if($run != $oldrun){
                 $dirs_runs[$dirfound]++;
                 $oldrun=$run;
             }
                
         } 
         else {
          $dirs[$#dirs+1] = $tdir;
          $dirs_ntuples[$#dirs]=1;
          $dirs_runs[$#dirs]=1;
          $dirs_triggers[$#dirs]=$nt->[8]-$nt->[9]+1;
          $oldrun=$run;
           $dirs_hr[$#dirs]="root";
           if($path=~/\.hbk$/){
               $dirs_hr[$#dirs]="hbk";
             }

          }
        }
       }
          my $rs=0;
          my $ns=0;
          my $ntr=0;
          for my $ind (0...$#dirs){
              $rs+=$dirs_runs[$ind];
              $ns+=$dirs_ntuples[$ind];
              $ntr+=$dirs_triggers[$ind];
          }
          my $s="  // Total Of  runs: $rs  ntuples: $ns   triggers: $ntr";
          print "<tr><td> $s </tr></td><br>";
          $buff = $buff.$s."\n";
          for my $ind (0...$#dirs){
#//
#// to be sure everything is o.k.
#// try to open dir and check all the files are in place
#//
           my $ntd=0;
           if($rootfileaccess=~/^NFS/){
                #print "<tr><td> $dirs[$ind] </tr></td><br>";
               opendir THISDIR, $dirs[$ind];
               my @files=readdir THISDIR;
               closedir THISDIR;
               foreach my $file (@files){
                if($file =~ /\.root$/ or $file=~ /\.hbk/){
                    $ntd++;
               }
              }       
           my $offline=0; 
           if($ntd != $dirs_ntuples[$ind] and $sqlmom eq ""){
             $s=" // Database and Linux Disagree MC \n   // Database says $dirs[$ind] contains $dirs_ntuples[$ind]  ntuples \n  //  Linux says it has $ntd ntuples \n";
              my @junk=split '/',$dirs[$ind];
              $sql="select isonline from filesystems where disk='/$junk[1]'";
              #die "  $sql $dirs[$ind] ";
              my $rtn=$self->{sqlserver}->Query($sql);
              if(defined  $rtn and $rtn->[0][0]==0){
                $offline=1;
                $s=$s."//   Because /$junk[1]  is Offline \n"; 
           }
           else{
            $s=$s." //  please inform vitali.choutko\@cern.ch about discrepancy \n"; 
          }        
          $buff = $buff.$s."\n";
          $s=~s/\n/<br>/g;
          print "<tr><td><font color=red> $s </tr></td><font color=black>";
          if(not $offline){
          opendir THISDIR, $dirs[$ind];
          my @files = readdir THISDIR;
          closedir THISDIR;
          if($ntd>$dirs_ntuples[$ind]){
            foreach my $file (@files){
              if($file=~/\.root$/ or $file=~/\.hbk/){
               my $found=0;
               foreach my $nt (@{$r1}){
                 my $path=trimblanks($nt->[0]);
                 if($path=~/$dirs[$ind]/){
                   if($path=~/$file/){
                    $found=1;
                    last;
                  }
                 }
             }
               if($found==0 and $sqlmom eq ""){
                   my @junk=split '\.',$file;
                   if($#junk>=0){
                       my $r2c=$junk[0];
                       $self->updateRunCatalog($r2c);
                   }  
                print " <tr><td> rm dirs[$ind]/$file is not in database </td></tr><br>";        
                #print " <tr><td> $sqlNT </td></tr><br>";        
 
               }
               }
             }                  
          }
          else{
          my $ok=0;
          my $patho="";
          my $runo=0;
         
           sub nprio1{ $b->[0] cmp $a->[0];}
           my @ntsorted=sort nprio1 @{$r1}; 
#          foreach my $nt (@{$r1}) {
          foreach my $nt (@ntsorted) {
           my $path=trimblanks($nt->[0]);
            if(not $path=~/$dirs[$ind]/ ){
               next;
            }
            my $found=0;
            foreach my $file (@files){
               if($file =~ /\.root$/ or $file=~/\.hbk$/){ 
             if($path=~/$file/){
              $found=1;
              last;
             }
             }
            }
             if($path eq $patho){
              #die "$sqlNT"; 
           print "<tr><td><font color=red> //$ok $found $path is duplicated for runs $runo $nt->[1]</tr></td><font color=black><br>";
             }
              $patho=$path;
              $runo=$nt->[1];
	      if($found==0 && $ntd>0){
             print "<tr><td><font color=red> // $path is absent on disk</tr></td><font color=black><br>";
                  if($path=~'xyzf4bck0' ){
                  my $nd =$path;
                   $nd=~s/f4bck0/f8dah3/;
                   my $sq="update ntuples set path='$nd' where path='$path'";
#                   die "$sq";
                   $self->{sqlserver}->Update($sq);
}

            }
            else{
             $ok++;
             #print "<tr><td><font color=green> //$ok $path is ok</tr></td><font color=black><br>";

            }
          }
          }
         }
         }
         }
           $sqlmom=~s/runs\.run/ntuples\.run/;
           $sqlamom=~s/runs\.run/ntuples\.run/;
           if($sqlamom =~/buildno/ or $sqlamom =~/pmin/){
               $sqlamom=~s/not LIKE/LIKE/;
           }
           my $negative= "SELECT ntuples.run From Ntuples,runcatalog WHERE Path like '%$dirs[$ind]/%'   $sqlamom group by ntuples.run ";
            my $r4=undef;
            if($sqlmom ne ""){
              $r4=$self->{sqlserver}->Query($negative);
            }
#           die "- $sqlmom - $negative - $#{@{$r4}} \n";
           if(($sqlmom eq "" or $#{$r4} ==-1)  ){
#          if(($sqlmom eq "" or $#{$r4} ==-1) and $sqlmom1 eq "" ){
           if($dirs_hr[$ind] eq "root"){ 
            $s = "chain.Add(\"".$dirs[$ind]."/*.$dirs_hr[$ind]\");";
           }
           else{
            $s="nt/chain chain $dirs[$ind]/*.$dirs_hr[$ind]"
           } 
          print "<tr><td> $s </tr></td><br>";
          $buff = $buff.$s."\n";
       }
          else{
            my $positive= "SELECT ntuples.run From Ntuples,runcatalog WHERE Path like '%$dirs[$ind]/%'  $sqlmom group by ntuples.run ";
            my $r3=$self->{sqlserver}->Query($positive);
            foreach my $path (@{$r3}) {
                  my $pth =$path->[0];
                  if($dirs_hr[$ind] eq "root"){
                     $s = "chain.Add(\"".$dirs[$ind]."/$pth.*.$dirs_hr[$ind]\");";
                  }
                   else {
                       $s="nt/chain chain $dirs[$ind]/$pth.*.$dirs_hr[$ind]"; 
                   }
          print "<tr><td> $s </tr></td><br>";
          $buff = $buff.$s."\n";
	   }
          }
          }
            } elsif ($rootfileaccess eq "HTTP") {
          foreach my $nt (@{$r1}) {
          my $path=trimblanks($nt->[0]);
          if ($path =~ m/castor/) {
#          skip it, file has only archived copy only
          } else {
           my $httppath=$prefix.$path;
           my $s = "chain.Add(\"$httppath\");";
           print "<tr><td> $s </tr></td>\n";
           $buff = $buff.$s."\n";
       }
      }
     } elsif ($rootfileaccess eq "REMOTE") {
        my $rrun = 0; # run
        foreach my $nt (@{$r1}) {
         if ($rrun != $nt->[1]) {
           $rrun = $nt->[1]; # run
           my $sql = "SELECT prefix,path From MC_DST_COPY WHERE Run=$rrun AND Cite='$remotecite'";
           my $r1=$self->{sqlserver}->Query($sql);
           if (defined $r1->[0][0]) {
            foreach my $p (@{$r1}) {
              my $prefix = trimblanks($p->[0]);
              my $local  = trimblanks($p->[1]);
              my $path=$prefix."/".$local;
              my $s = "chain.Add(\"$path\");";
              print "<tr><td> $s </tr></td>\n";
              $buff = $buff.$s."\n";
           }
        }
       }
      }
     } elsif ($rootfileaccess eq "CASTOR obsolete") {
          foreach my $nt (@{$r1}) {
# check castortime
           if ($nt->[7] > 0) {
            my $path=trimblanks($nt->[0]);
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
           #$dirs[$#dirs] = $tdir;
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
       $self-> writetofile($rootfile,$buff);
      }
   }
     htmlReturnToQuery();
    htmlBottom();
   }
   
# queryDB04 / doQuery ends here
}    elsif ($self->{q}->param("queryDB04") eq "DoQueryD") {
        my $datamc=1;
      my $sql     = undef;
      my $sqlNT   = undef; # will be used to get NT paths for ROOT file
      my $dataset = undef;
      my $particle= undef;
      my $sqlmom="";
      my $sqlamom="";
      my $sqlmom1="";
      my $sqlamom1="";
#
#   vc   -  remove monstrous loop in summary
#
      my $sqlsum=  "SELECT COUNT(ntuples.jid), SUM(ntuples.SIZEMB), SUM(ntuples.NEVENTS), sum(ntuples.LEvent-ntuples.FEvent+1) FROM Ntuples, jobs,dataruns,datasetsdesc,datasets where (ntuples.datamc=1 or ntuples.datamc=0 or ntuples.datamc=2) ";
      my $rsum=undef;
#
# Template is defined explicitly
#
       my $qpp=$q->param("QPPer");
       my $pps="";
       if($qpp>0){
         $pps=$pps." and jobs.pid=$qpp ";
       }
       elsif($qpp==-1){
#         active
         my $sqlaa = "SELECT  DID  FROM ProductionSet WHERE STATUS='Active' ORDER BY DID";
         my $ret = $self->{sqlserver}->Query($sqlaa);
         $pps=undef;
         foreach my $pp  (@{$ret}){
          if(defined $pps){
            $pps=$pps." or jobs.pid =";
          }
          else{
           $pps=" and ( jobs.pid =";
          }
         $pps=$pps." $pp->[0] ";
        }
       if(defined $pps){
        $pps=$pps." ) ";
       }
       else{
        $pps="";
       }
  }



      if (defined $q->param("QTempDataset") and $q->param("QTempDataset") ne "Any") {

       $dataset = $q->param("QTempDataset");
       $dataset = trimblanks($dataset);
       $qtemplate = $dataset;
       my $qparticle="";
       my $qaparticle="";
       if (defined $q->param("QPartD") and
                   ($q->param("QPartD") ne "AnyData" and
                    $q->param("QPartD") ne "ANY" and $q->param("QPartD") ne "any"))  {
         $particle = $q->param("QPartD");
         $qparticle = " and datasets.did=jobs.did and datasets.name like '$particle' ";
         $qaparticle = " and datasets.did=jobs.did and datasets.name not like '$particle' ";
     }

#- 20.06.05 a.k.       $dataset =~ s/ /\% /g;
       $sql = "SELECT dataruns.run, jobs.jobname, dataruns.submit,dataruns.jid FROM dataruns, jobs,datasetsdesc,datasets
                   WHERE dataruns.jid=jobs.jid  AND dataruns.status='Completed'";
                   $sqlmom=" and datasetsdesc.did=jobs.did and datasetsdesc.jobname=split(jobs.jobname)  AND
                        (datasetsdesc.jobdesc LIKE '%$dataset%'  $qparticle) ";
                   $sqlamom="  and datasetsdesc.did=jobs.did and datasetsdesc.jobname=split(jobs.jobname) AND
           (datasetsdesc.jobdesc not LIKE '%$dataset%' $qaparticle)";





           if ($q->param("QBuildNum")){
               my @buildNumber= buildnum_string($q->param("QBuildNum"));
               my $buildnum_min = $buildNumber[0];
               my $buildnum_max = $buildNumber[1];
               $sqlmom1=$sqlmom1." AND (Ntuples.buildno>=$buildnum_min AND Ntuples.buildno<=$buildnum_max) ";
               $sqlamom1=$sqlamom1." And (Ntuples.buildno<$buildnum_min OR Ntuples.buildno>$buildnum_max) ";
           }


       $sqlNT = "SELECT Ntuples.path, Ntuples.run, Ntuples.nevents, Ntuples.neventserr,
                        Ntuples.timestamp, Ntuples.status, Ntuples.sizemb, Ntuples.castortime,ntuples.levent,ntuples.fevent,ntuples.jid
                 FROM dataruns, jobs, datasetsdesc, datasets, ntuples
                   WHERE dataruns.jid=jobs.jid and dataruns.run=ntuples.run  AND dataruns.status='Completed' and dataruns.jid=ntuples.jid";
       $sql=$sql.$sqlmom;
       $sqlNT=$sqlNT.$sqlmom;

#

      my @garbage= split /WHERE/,$sql;
       my $rf="NFS";
         if (defined $q->param("ROOTACCESS")) {
             $rf = $q->param("ROOTACCESS");
         } 
      if($#garbage>0){
          my $addf="";
        if($rf=~/^NFS/ and $sqlsum=~/ntuples/){
            $addf=" and ntuples.path not like '%castor%'";
        }
        $sqlsum=$sqlsum." and ntuples.jid=jobs.jid "." and ".$garbage[1].$pps." and ntuples.run=dataruns.run".$addf;
           $rsum=$self->{sqlserver}->Query($sqlsum);
        # die "$sqlsum $rsum->[0][0] $rsum->[0][1] $rsum->[0][2] ";

      }

       $sql = $sql.$pps."ORDER BY dataRuns.Run";
      $sqlmom=$sqlmom.$sqlmom1;
      $sqlamom=$sqlamom.$sqlamom1;
       $sqlNT = $sqlNT.$sqlmom1." ".$pps."ORDER BY dataRuns.Run";
       my $r1=$self->{sqlserver}->Query($sql);
       my $r2=$self->{sqlserver}->Query($sqlNT);
        if (defined $r2->[0][0]) {
         foreach my $r (@{$r2}){
             if($#jidsnt<0 or $jidsnt[$#jidsnt]!=$r->[10]){
               push @jidsnt,$r->[10];
           }
         }
     }
        if (defined $r1->[0][0]) {
         foreach my $r (@{$r1}){
               push @runs,$r->[0];
               push @jids,$r->[3];
               push @jobnames,$r->[1];
               push @submits,$r->[2];
         }
     }
#
# Template 'Any', particle (dataset) is defined
#
      } elsif (defined $q->param("QPartD") and
                   ($q->param("QPartD") ne "AnyData" and
                    $q->param("QPartD") ne "ANY" and $q->param("QPartD") ne "any"))  {
         $particle = $q->param("QPartD");
         my $versioncond = '';
         if (defined $q->param("Version")) {
             $versioncond = " AND VERSION = '".$q->param("Version")."'";
         }
         $qparticle = $particle;
         $sql = " SELECT DID FROM Datasets WHERE NAME LIKE '$particle' $versioncond";
         my $r0=$self->{sqlserver}->Query($sql);
         if (defined $r0->[0][0]) {
          foreach my $r (@{$r0}){
           my $did = $r->[0];
           $sql  = "SELECT dataRuns.Run, Jobs.JOBNAME, dataRuns.SUBMIT,dataruns.jid
                    FROM dataRuns, Jobs
                     WHERE Jobs.DID=$did AND Jobs.JID=dataRuns.JID 
                             AND dataRuns.Status='Completed'";
      $sqlNT = "SELECT Ntuples.path, Ntuples.run, Ntuples.nevents, Ntuples.neventserr,
                        Ntuples.timestamp, Ntuples.status, Ntuples.sizemb, Ntuples.castortime,ntuples.levent,ntuples.fevent,ntuples.jid
                    FROM dataRuns, Jobs, NTuples
                     WHERE Jobs.DID=$did AND Jobs.JID=dataRuns.JID AND
                            dataRuns.run=Ntuples.run AND
                            dataRuns.Status='Completed' and dataruns.jid=ntuples.jid";
           if ($q->param("QBuildNum")){
               my @buildNumber= buildnum_string($q->param("QBuildNum"));
               my $buildnum_min = $buildNumber[0];
               my $buildnum_max = $buildNumber[1];
               $sqlmom1=$sqlmom1." AND (Ntuples.buildno>=$buildnum_min AND Ntuples.buildno<=$buildnum_max) ";
               $sqlamom1=$sqlamom1." AND (Ntuples.buildno<$buildnum_min OR Ntuples.buildno>$buildnum_max) ";
           }


#
       

       my $rf="NFS";
         if (defined $q->param("ROOTACCESS")) {
             $rf = $q->param("ROOTACCESS");
         } 

       my @garbage= split /WHERE/,$sql;
        if($#garbage>0){
          my $addf="";
        if($rf=~/^NFS/ and $sqlsum=~/ntuples/){
            $addf=" and ntuples.path not like '%castor%'";
        }
        $sqlsum=$sqlsum." and  datasets.did=jobs.did and ntuples.jid=jobs.jid and ".$garbage[1].$pps." and ntuples.run=dataruns.run".$addf;
#         $sqlsum=$sqlsum." where ".$garbage[1].$pps." and ntuples.run=dataruns.run";           $rsum=$self->{sqlserver}->Query($sqlsum);
         # die "$sqlsum $rsum->[0][0] $rsum->[0][1] $rsum->[0][2] ";
      }
            $sql = $sql.$pps." ORDER BY dataRuns.Run";
            $sqlNT = $sqlNT.$pps." ORDER BY dataRuns.Run";
            my $r1=$self->{sqlserver}->Query($sql);
       my $r2=$self->{sqlserver}->Query($sqlNT);
        if (defined $r2->[0][0]) {
         foreach my $r (@{$r2}){
             if($#jidsnt<0 or $jidsnt[$#jidsnt]!=$r->[10]){
               push @jidsnt,$r->[10];
           }
         }
        }
            if (defined $r1->[0][0]) {
             foreach my $r (@{$r1}){
               push @runs,$r->[0];
               push @jids,$r->[3];
               push @jobnames,$r->[1];
               push @submits,$r->[2];
              }
             }
            }
      }
     } else {
        $sql = "SELECT dataRuns.RUN, Jobs.JOBNAME, dataRuns.SUBMIT,dataruns.jid 
                    FROM dataRuns, Jobs
                     WHERE dataRuns.JID=Jobs.JID AND dataRuns.Status='Completed'  ";
        $sqlNT = "SELECT Ntuples.path, Ntuples.run, Ntuples.nevents, Ntuples.neventserr,
                         Ntuples.timestamp, Ntuples.status, Ntuples.sizemb, Ntuples.castortime,ntuples.levent,ntuples.fevent,ntuples.jid
                    FROM dataRuns, Jobs, Ntuples
                     WHERE
                        dataRuns.JID=Jobs.JID AND
                         dataRuns.Status='Completed' AND
                           dataRuns.run = Ntuples.run ";

           if ($q->param("QBuildNum")){
               my @buildNumber= buildnum_string($q->param("QBuildNum"));
               my $buildnum_min = $buildNumber[0];
               my $buildnum_max = $buildNumber[1];
               $sqlmom1=$sqlmom1." AND (Ntuples.buildno>=$buildnum_min AND Ntuples.buildno<=$buildnum_max) ";
               $sqlamom1=$sqlamom1." AND (Ntuples.buildno<$buildnum_min OR Ntuples.buildno>$buildnum_max) ";
           }


#
            my @garbage= split /WHERE/,$sql;
       my $rf="NFS";
         if (defined $q->param("ROOTACCESS")) {
             $rf = $q->param("ROOTACCESS");
         } 
             if($#garbage>0){
          my $addf="";
        if($rf=~/^NFS/ and $sqlsum=~/ntuples/){
            $addf=" and ntuples.path not like '%castor%'";
        }
        $sqlsum=$sqlsum." and datasets.did=jobs.did and ntuples.jid=jobs.jid and ".$garbage[1].$pps." and ntuples.run=dataruns.run".$addf;
#              $sqlsum=$sqlsum." where ".$garbage[1].$pps." and ntuples.run=dataruns.run";
              $rsum=$self->{sqlserver}->Query($sqlsum);
#             die "$sqlsum $rsum->[0][0] $rsum->[0][1] $rsum->[0][2] ";
          }
            $sql = $sql.$pps." ORDER BY dataRuns.Run";
            $sqlNT = $sqlNT.$pps." ORDER BY dataRuns.Run";
            my $r1=$self->{sqlserver}->Query($sql);
       my $r2=$self->{sqlserver}->Query($sqlNT);
        if (defined $r2->[0][0]) {
         foreach my $r (@{$r2}){
             if($#jidsnt<0 or $jidsnt[$#jidsnt]!=$r->[10]){
               push @jidsnt,$r->[10];
           }
         }
        }
            if (defined $r1->[0][0]) {
             foreach my $r (@{$r1}){
               push @runs,$r->[0];
               push @jids,$r->[3];
               push @jobnames,$r->[1];
               push @submits,$r->[2];
              }
             }
    }
# now check output
      my $accessmode       = "xyz";
      my $remotecite       = "xyz";
      my $rootfile         = undef;
      my $rootfileaccess   = undef;

      if (defined $q->param("REMOTEACCESS")) {
          $accessmode="REMOTE";
          $remotecite = $q->param("REMOTEACCESS");
      }
      if (defined $q->param("NTOUT")) {
       $self->htmlTop();
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

       if ($q->param("NTOUT") eq "ROOT") {
        $rootfile = $q->param("ROOT");
        if (not $rootfile =~ /\//  ) {
            my $ctime = time();
            $rootfile = "/tmp/".$rootfile.$ctime;
        }
        print "<tr><td><b><font size=\"4\" color=\"blue\">ROOT script  ";
        print "</font></b></td>";
         if (defined $q->param("ROOTACCESS")) {
             $rootfileaccess = $q->param("ROOTACCESS");
         } else {
             $rootfileaccess = "NFS";
         }
         print "<td><b><font size=\"4\" color=\"black\">$rootfile </font></b><i> (access via $rootfileaccess) </i></td></tr>\n";
      }
      htmlTableEnd();
      print "<BR><BR>\n";
       if (defined $#runs) {
           my $nruns = $#runs+1;
           print "<LI><font size=5 color=tomato><i> Total Runs Found in Database ... </font></i> $nruns </li>\n";
           print "<BR><BR>\n";
       }
# ....print 'ALL' information
#        die " $#runs $sql $sqlNT blia\n";
       if ($q->param("NTOUT") eq "ALL") {
              my $i       = 0;
              my $runold  = 0;
              my $run     = 0;
              $sql = $sqlNT;
              my $r0=$self->{sqlserver}->Query($sql);
              if (defined $r0->[0][0]) {
               foreach my $nt (@{$r0}){
                my $printit = 1;
                $run = $nt->[1];
                if ($run != $runold) {
                 if ($accessmode eq "REMOTE") {
                  $sql = "SELECT run FROM MC_DST_COPY WHERE run=$run AND cite='$remotecite'";
                  my $r0=$self->{sqlserver}->Query($sql);
                  if (not defined $r0->[0]) {
                   $printit = 0;
                  }
                 }
                 if ($printit == 1) {
                  $i = 0;
                  foreach my $r (@runs) {
                   if ($r == $run) {
                    my $jobname = $jobnames[$i];
                    my $submit  = $submits[$i];
                    print "<tr><td><b><font size=\"3\" color=$color> Job : $jobname, Run : $run, Submitted : $submit";
                    print "</font></b></td></tr>\n";
                    if ($runold != 0) {            htmlTableEnd();}
                    print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
                    print "<table border=1 width=\"100%\" cellpadding=0 cellspacing=0>\n";
                    print "<tr><td width=10% align=left><b><font color=\"blue\" > NTuple </font></b></td>";
                    print "<td width=10%><b><font color=\"blue\"> Events </font></b></td>";
                    print "<td width=15%><b><font color=\"blue\" > Errors </font></b></td>";
                    print "<td width=15%><b><font color=\"blue\" > Size[MB] </font></b></td>";
                    print "<td td align=middle><b><font color=\"blue\" > Produced </font></b></td>";
                    print "<td width=10%><b><font color=\"blue\" > Status </font></b></td>";
                    print "</tr>\n";
                   }
                   $i++;
                  }
                 }
                 $runold = $run;
                }
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
                  my $timel =localtime($nt->[4]);
                  my ($wday,$mon,$day,$time,$year) = split " ",$timel;
                  my $status=$nt->[5];
                  my $color=statusColor($status);
                  print "<td width=50%><b> $path    </td></b><td><b> $nt->[2] </td>
                        <td align=middle width=5%><b> $nt->[3] </b></td>
                        <td align=middle width=5%><b> $nt->[6] </b></td>
                        <td align=middle width=25%><b> $mon $day, $time, $year </b></td>
                        <td align=middle width=10%><b><font color=$color> $status </font></b></td> \n";
                 print "</font></tr>\n";
              }
            }
            htmlTableEnd();
            print "<BR><BR>\n";
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
       #die "@runs"; 
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
    my $addf="";
         if (defined $q->param("ROOTACCESS")) {
             $rootfileaccess = $q->param("ROOTACCESS");
         } else {
             $rootfileaccess = "NFS";
         }
      if ($rootfileaccess=~/NFSONLY/ ) {
           $addf=" and castortime=0 ";
       }
      elsif ($rootfileaccess=~/NFS/ ) {
           $addf=" and path not like '/castor%'";
       }
      elsif ($rootfileaccess=~/CASTOR/ ) {
           $addf=" and castortime>0";
      }
#    die " $rootfileaccess  qwa  $addf ";
       
       foreach my $run (@jidsnt){
               if ($accessmode eq "REMOTE") {
                $sql = "SELECT prefix,path FROM MC_DST_COPY WHERE run=$run AND cite='$remotecite'";
                my $r0=$self->{sqlserver}->Query($sql);
                foreach my $path (@{$r0}) {
                   print "<td><b><font color=$color> $path->[0]/$path->[1] </font></td></b></font></tr>\n";
               }
            } else {
             my $jobname = $jobnames[$i];
             my $submit  = localtime($submits[$i]);
#             die "$sqlmom1";
             $sql = "SELECT path From Ntuples WHERE jid=$run".$addf;
             my $r1=$self->{sqlserver}->Query($sql);
             foreach my $path (@{$r1}) {
             $i++;
             my $file="";
             if((not $rootfileaccess=~/CASTOR/) || $path->[0]=~/^\/castor/ ){
                 $file=$path->[0];
             }
             else{
                 my @junk=split '\/',$path->[0];
                 $file="/castor/cern.ch/ams";
                 for my $i (2...$#junk){
                     $file=$file."/$junk[$i]";
                 }
             }
             print "<td><b><font color=$color> $file </font></td></b></font></tr>\n";
           }
         }
           }
             print "<td><b><font color=$color> Total of $i  </font></td></b></font></tr>\n";
      htmlTableEnd();
   } elsif ($q->param("NTOUT") eq "SUMM") {
# ... print summary
       my @titles= (
        "Template",
        "Jobs",
        "DSTs",
        "DSTs GB",
        "Events",
        "Triggers",);
        my $query= $q->param('QPartD');
        if($query eq "AnyData"){
            $query="Any";
        }
        my $q2= $q->param("QTempDataset");
        if($q2 eq "Any"){
          $q2=undef;
        }
        my $buffer="Summary for $query  ";
        my @output=();
        my @temp=();
         my $sql;
        if($query eq "Any"){
          $titles[0]="Dataset";
          my $dmc=$q->param('DataMC');
          if(not defined $dmc){
              $dmc=1;
          }
           $sql = "SELECT datasetsdesc.dataset,datasetsdesc.did FROM datasets,DatasetsDesc where datasets.datamc=$dmc and datasets.did=datasetsdesc.did order by datasetsdesc.did ";
        }
        else{
         $sql="SELECT jobdesc FROM DatasetsDesc WHERE dataset='$query'"; 
        }
#             die "2 $query $sql ";
         my $rquery=$self->{sqlserver}->Query($sql);
         my $nruns=0;
        my $dsts=0;
        my $gb=0;
        my $evt=0;
        my $trg=0;
         my $prev="";
         foreach my $templat (@{$rquery}){
          $#temp=-1;
           my $like=trimblanks($templat->[0]);
           if($like eq $prev or  (defined $q2 and  $q2 ne $templat->[0])){
              next;
           }
           if($query eq "Any"){
             $sql=$sqlsum." and (jobs.did=$templat->[1]) and (jobs.jid=ntuples.jid) and (dataruns.jid=ntuples.jid  ) and (datasetsdesc.did=jobs.did)";
           }
           else{
            $sql=$sqlsum." and datasetsdesc.did=jobs.did and datasetsdesc.jobname=split(jobs.jobname)  AND (datasetsdesc.jobdesc LIKE '$templat->[0]' ) ";
#             die "2 $query $templat->[0] $sql ";
           }
            my $rsump=$self->{sqlserver}->Query($sql);
#          die "$sql";
           my @sqla=split 'where',$sql;
#           $sqla[1]=~s/and ntuples.run=runs.run//;
#           $sqla[1]=~s/and ntuples.path not like \'\%castor\%\'//;
           $sql="select count(jobs.jid) from jobs,dataruns,ntuples,datasetsdesc,datasets where ".$sqla[1]." group by ntuples.run";
            my $rsuma=$self->{sqlserver}->Query($sql);
          my $i=0;
          foreach my $p (@{$rsuma}){
              $i++;
          }
          push @temp,$like;
          push @temp,$i;
          $nruns+=$i;
          $dsts+=$rsump->[0][0];
          $gb+=int($rsump->[0][1]/100)/10; 
          $evt+=$rsump->[0][2];
          $trg+=$rsump->[0][3];
          push @temp,$rsump->[0][0];
          push @temp,int($rsump->[0][1]/100)/10; 
          push @temp,$rsump->[0][2];
           push @temp,$rsump->[0][3];
            push @temp,"dum";
           push @output,[@temp];
           $prev=$like;
         }
          $#temp=-1;
           push @temp,"Total of";
          push @temp,$nruns;
        push @temp,$dsts;
        push @temp,$gb;
        push @temp,$evt;
        push @temp,$trg;
           push @temp,"dum";
           push @output,[@temp];

         monitorHTML::print_table($q,$buffer,$#titles,@titles,@output);
       

  } elsif ($q->param("NTOUT") eq "ROOT") {
#... write RootAnalysisTemplate
      my $RootAnalysisTextNFS =
         "// ROOT files accessed via NFS
         ";
      my $RootAnalysisTextCastor =
         "// it is assumed that CASTOR directory
          //   structure is similar to one on AMS disks.
          //   /castor/cern.ch/MC/AMS02/ProductionPeriod/...
         ";
      my $RootAnalysisTextHTTP =
         "// wildcards are not implemented yet in ROOT.
          //   still  have to check what is the HTTPD protocol for
          //   getting a list of files.
         ";

      my $RootAnalysisTextRemote =
         "// it is assumed that REMOTE directory structure and lib(s) path are similar to one on AMS disks.
         ";
      my $RootAnalysisTemplateTxt =
         "  gROOT->Reset();
          // for linuxicc load
          gSystem->Load(\"/Offline/vdev/lib/linuxicc/ntuple_slc4.so\");

          TChain chain(\"AMSRoot\");
      ";
        
#...... check files access option
      my $prefix     = "xyz";
          if ($rootfileaccess eq "HTTP") {
              $prefix     = "http://$self->{HTTPserver}";
          }
          if ($rootfileaccess eq "CASTOR") {
              $prefix     = "rfio:/castor/cern.ch/ams";
          }
#..... remote cite (if any)
      if (defined $q->param("REMOTEACCESS")) {
          $rootfileaccess="REMOTE";
          $remotecite = $q->param("REMOTEACCESS");
      }

    my $buff=undef;
    my @dirs=();
    $#{dirs} =-1;
    my @dirs_runs=();
    $#{dirs_runs} =-1;
    my @dirs_ntuples=();
    $#dirs_ntuples =-1;
    my @dirs_triggers=();
    $#{dirs_triggers} =-1;
    my @dirs_hr=();
     $#dirs_hr=-1;
      if ($rootfileaccess eq "HTTP") {
         $buff = $RootAnalysisTextHTTP;
         print "<tr><td>$RootAnalysisTextHTTP</td></tr><br>";
     } elsif ($rootfileaccess eq "CASTOR") {
         $buff = $RootAnalysisTextCastor;
         $RootAnalysisTextCastor=~s/\n/<br>/g;
         print "<tr><td>$RootAnalysisTextCastor</td></tr><br>";
     } elsif ($rootfileaccess eq "REMOTE") {
         $buff = $RootAnalysisTextRemote;
         print "<tr><td>$RootAnalysisTextRemote</td></tr><br>";
     } else {
         $buff = $RootAnalysisTextNFS;
         $RootAnalysisTextNFS=~s/\n/<br>/g;
         print "<tr><td>$RootAnalysisTextNFS</td></tr>\n";
     }
        
        $buff=$buff."\n";
         $buff=$buff.$RootAnalysisTemplateTxt."\n";
         $RootAnalysisTemplateTxt=~s/\n/<br>/g;
         print "<tr><td>$RootAnalysisTemplateTxt</td></tr>\n";
#
      my $sql =$sqlNT;
       my $dmc=$q->param('DataMC');
          if(not defined $dmc){
              $dmc=0;
          }
          else{
            my $qpartd=$q->param("QPartD");
            if(not defined $qpartd or $qpartd eq "AnyData"){
                $qpartd=" and ntuples.datamc=$dmc ";
             }
             else{
              $qpartd="and path like '%".$qpartd."%'"; 
             }
            $sql=~s/ORDER BY/ and ntuples.jid=jobs.jid  $qpartd ORDER BY/;

}

      my $r1=$self->{sqlserver}->Query($sql);
      if ($rootfileaccess=~/^NFS/ or $rootfileaccess eq "CASTOR") {
          my $oldrun=0;
          foreach my $nt (@{$r1}) {
           my $path=trimblanks($nt->[0]);
           if ($path =~ m/castor/ and $rootfileaccess=~/^NFS/) {
#           skip it, file has only archived copy 
           }
           elsif($rootfileaccess eq "NFSONLY" and $nt->[7]>0){
#           skip it, file has  archived copy 
           } elsif($rootfileaccess=~/^NFS/ or $nt->[7] > 0){
            my @junk = split '/',$path;
            my $tdir ="";
            my @jrun=split '\.',$junk[$#junk];
            my $run=$jrun[0]; 
            if($rootfileaccess eq "CASTOR"){
                $junk[1]=$prefix;
            }
                     else{
             $junk[1]="/".$junk[1];
             }
           for (my $i=1; $i<$#junk; $i++) {
                my $sp="/";
                if($i==1){
                    $sp="";
                }
             $tdir = $tdir.$sp.$junk[$i];
            }
            $tdir = trimblanks($tdir);
#            $tdir=$tdir."/";
            my $dirfound = -1;

            for my $i(0...$#dirs){
                
             if ($dirs[$i] eq $tdir) {
              $dirfound = $i;
              last;
          }
         }
         if ($dirfound >=0) {
# skip it
             $dirs_ntuples[$dirfound]++;
          $dirs_triggers[$dirfound]+=$nt->[8]-$nt->[9]+1;
             if($run != $oldrun){
                 $dirs_runs[$dirfound]++;
                 $oldrun=$run;
             }
                
         } 
         else {
          $dirs[$#dirs+1] = $tdir;
          $dirs_ntuples[$#dirs]=1;
          $dirs_runs[$#dirs]=1;
          $dirs_triggers[$#dirs]=$nt->[8]-$nt->[9]+1;
          $oldrun=$run;
           $dirs_hr[$#dirs]="root";
           if($path=~/\.hbk$/){
               $dirs_hr[$#dirs]="hbk";
             }

          }
        }
       }
          my $rs=0;
          my $ns=0;
          my $ntr=0;
          for my $ind (0...$#dirs){
              $rs+=$dirs_runs[$ind];
              $ns+=$dirs_ntuples[$ind];
              $ntr+=$dirs_triggers[$ind];
          }
          my $s="  // Total Of  Runs: $rs  ntuples: $ns   triggers: $ntr";
          print "<tr><td> $s </tr></td><br>";
          $buff = $buff.$s."\n";
          for my $ind (0...$#dirs){
#//
#// to be sure everything is o.k.
#// try to open dir and check all the files are in place
#//
           my $ntd=0;
           if($rootfileaccess=~/^NFS/){
                #print "<tr><td> $dirs[$ind] </tr></td><br>";
               opendir THISDIR, $dirs[$ind];
               my @files=readdir THISDIR;
               closedir THISDIR;
               foreach my $file (@files){
                if($file =~ /\.root$/ or $file=~ /\.hbk/){
                    $ntd++;
               }
              }       
           my $offline=0; 
           if($ntd != $dirs_ntuples[$ind] and $sqlmom eq ""){
             $s=" // Database and Linux Disagree \n   // Database says $dirs[$ind] contains $dirs_ntuples[$ind]  ntuples \n  //  Linux says it has $ntd ntuples \n";
              my @junk=split '/',$dirs[$ind];
              $sql="select isonline from filesystems where disk='/$junk[1]'";
              #die "  $sql $dirs[$ind] ";
              my $rtn=$self->{sqlserver}->Query($sql);
              if(defined  $rtn and $rtn->[0][0]==0){
                $offline=1;
                $s=$s."//   Because /$junk[1]  is Offline \n"; 
           }
           else{
            $s=$s." //  please inform vitali.choutko\@cern.ch about discrepancy \n"; 
          }        
          $buff = $buff.$s."\n";
          $s=~s/\n/<br>/g;
          print "<tr><td><font color=red> $s </tr></td><font color=black>";
          if(not $offline){
          opendir THISDIR, $dirs[$ind];
          my @files = readdir THISDIR;
          closedir THISDIR;
          if($ntd>$dirs_ntuples[$ind]){
            foreach my $file (@files){
              if($file=~/\.root$/ or $file=~/\.hbk/){
               my $found=0;
               foreach my $nt (@{$r1}){
                 my $path=trimblanks($nt->[0]);
                 if($path=~/$dirs[$ind]/){
                   if($path=~/$file/){
                    $found=1;
                    last;
                  }
                 }
               }
               if($found==0 and $sqlmom eq ""){
                print " <tr><td> rm $dirs[$ind]/$file is not in database </td></tr><br>";          
                #print " <tr><td> $sqlNT </td></tr><br>";        
             }
               }
             }                  
        }
          else{
          my $ok=0;
          my $patho="";
          my $runo=0;
         
#           sub nprio1{ $b->[0] cmp $a->[0];}
           my @ntsorted=sort nprio1 @{$r1}; 
#          foreach my $nt (@{$r1}) {
          foreach my $nt (@ntsorted) {
           my $path=trimblanks($nt->[0]);
            if(not $path=~/$dirs[$ind]/ ){
               next;
            }
            my $found=0;
            foreach my $file (@files){
               if($file =~ /\.root$/ or $file=~/\.hbk$/){ 
             if($path=~/$file/){
              $found=1;
              last;
             }
             }
            }
             if($path eq $patho and $dmc!=1){
              #die "$sqlNT"; 
           print "<tr><td><font color=red> //$ok $found $path is duplicated for runs $runo $nt->[1]</tr></td><font color=black><br>";
             }
              $patho=$path;
              $runo=$nt->[1];
	      if($found==0 && $ntd>0){
             print "<tr><td><font color=red> // $path is absent on disk</tr></td><font color=black><br>";

            }
            else{
             $ok++;
             #print "<tr><td><font color=green> //$ok $path is ok</tr></td><font color=black><br>";

            }
          }
          }
         }
         }
           }
           $sqlmom=~s/dataruns\.run/ntuples\.run/;
           $sqlamom=~s/dataruns\.run/ntuples\.run/;
           if($sqlamom =~/buildno/ or $sqlamom =~/pmin/){
               $sqlamom=~s/not LIKE/LIKE/;
                  $sqlamom=~s/not like/like/;
           }
       #    die "$sqlamom job";
my $negative= "SELECT ntuples.run From Ntuples,datasetsdesc,jobs,datasets WHERE Path like '%$dirs[$ind]/%' and  datasets.did=jobs.did and ntuples.jid=jobs.jid  $sqlamom group by ntuples.run ";
            my $r4=undef;
            if($sqlmom ne ""){
              $r4=$self->{sqlserver}->Query($negative);
            }
#           die "- $sqlmom - $negative - $#{@{$r4}} \n";
           if(($sqlmom eq "" or $#{$r4} ==-1)  ){
#          if(($sqlmom eq "" or $#{$r4} ==-1) and $sqlmom1 eq "" ){
           if($dirs_hr[$ind] eq "root"){ 
            $s = "chain.Add(\"".$dirs[$ind]."/*.$dirs_hr[$ind]\");";
           }
           else{
            $s="nt/chain chain $dirs[$ind]/*.$dirs_hr[$ind]"
           } 
          print "<tr><td> $s </tr></td><br>";
          $buff = $buff.$s."\n";
       }
          else{
            my $positive= "SELECT ntuples.run From Ntuples,datasetsdesc,jobs,datasets WHERE Path like '%$dirs[$ind]/%'  and datasets.did=jobs.did and ntuples.jid=jobs.jid $sqlmom group by ntuples.run ";
            my $r3=$self->{sqlserver}->Query($positive);
            foreach my $path (@{$r3}) {
                  my $pth =$path->[0];
                  if($dirs_hr[$ind] eq "root"){
                     $s = "chain.Add(\"".$dirs[$ind]."/$pth.*.$dirs_hr[$ind]\");";
                  }
                   else {
                       $s="nt/chain chain $dirs[$ind]/$pth.*.$dirs_hr[$ind]"; 
                   }
          print "<tr><td> $s </tr></td><br>";
          $buff = $buff.$s."\n";
	   }
        }
           if($ind ==21){
#             die " dirs $#dirs ";
         }
       }
            } elsif ($rootfileaccess eq "HTTP") {
          foreach my $nt (@{$r1}) {
          my $path=trimblanks($nt->[0]);
          if ($path =~ m/castor/) {
#          skip it, file has only archived copy only
          } else {
           my $httppath=$prefix.$path;
           my $s = "chain.Add(\"$httppath\");";
           print "<tr><td> $s </tr></td>\n";
           $buff = $buff.$s."\n";
       }
      }
     } elsif ($rootfileaccess eq "REMOTE") {
        my $rrun = 0; # run
        foreach my $nt (@{$r1}) {
         if ($rrun != $nt->[1]) {
           $rrun = $nt->[1]; # run
           my $sql = "SELECT prefix,path From MC_DST_COPY WHERE Run=$rrun AND Cite='$remotecite'";
           my $r1=$self->{sqlserver}->Query($sql);
           if (defined $r1->[0][0]) {
            foreach my $p (@{$r1}) {
              my $prefix = trimblanks($p->[0]);
              my $local  = trimblanks($p->[1]);
              my $path=$prefix."/".$local;
              my $s = "chain.Add(\"$path\");";
              print "<tr><td> $s </tr></td>\n";
              $buff = $buff.$s."\n";
           }
        }
       }
      }
     } elsif ($rootfileaccess eq "CASTOR obsolete") {
          foreach my $nt (@{$r1}) {
# check castortime
           if ($nt->[7] > 0) {
            my $path=trimblanks($nt->[0]);
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
           #$dirs[$#dirs] = $tdir;
           
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
       $self-> writetofile($rootfile,$buff);
      }
  }
     htmlReturnToQuery();
    htmlBottom();
   }
    

# queryDB04 / doQueryd ends here


  } elsif ($self->{q}->param("queryDB04") eq "Continue" ) {
     my $query=$q->param("QPart");
    my $qpp=$q->param("QPPer");
    my $dsname = $query;
    my $dsver = '';
    if ($query =~ /_v/) {
        my @junk = split /_v/, $query;
        if (scalar @junk >=2) {
            $dsname = $junk[0];
            $dsver = "v".$junk[1];
        }
    }

     foreach my $dataset (@{$self->{DataSetsT}}){
      if($dataset->{name} eq $dsname){
                 foreach $cite (@{$dataset->{jobs}}){
                 if(not ($cite->{filename} =~/^\./)){
                  push @tempnam, $cite->{filename};
                  $hash->{$cite->{filename}}=$cite->{filedesc};
                  push @desc, $hash ->{$cite->{filename}};
              }
           }
        }
      }
   htmlTop();
    $self->htmlAMSHeader("AMS-02 Production Database Query Form");
    print "<ul>\n";
    htmlMCWelcome();
    print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";
     print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
      print "<tr><td><b><font color=\"green\">Datasets </font></b>\n";
      print "</td><td>\n";
      print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
       print "<tr valign=middle><td align=left><b><font size=3 color=green> $query</b></td> <td colspan=1>\n";
       print "<INPUT TYPE=\"hidden\" NAME=\"QPart\" VALUE=\"$dsname\">\n";
       print "<INPUT TYPE=\"hidden\" NAME=\"Version\" VALUE=\"$dsver\">\n";
       print "<INPUT TYPE=\"hidden\" NAME=\"QPPer\" VALUE=\"$qpp\">\n";
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
      if ($query =~ "Any") {
         print "<option value=\"Any\">ANY </option>\n";
      } else {
           my $qpp=$q->param("QPPer");
           my $jobspid="";
           if(defined $qpp){
              if(  $qpp>0){
               $jobspid="jobs.pid=$qpp and ";
           }
              elsif ($qpp==-1){
                $jobspid=$self->getactiveppstring();
              }
          }
          my $sql="select count(jobs.jid),datasetsdesc.jobdesc from jobs,datasetsdesc,datasets where $jobspid jobs.did=datasetsdesc.did and datasetsdesc.dataset='$dsname' and datasets.version like '\%$dsver\%' and datasets.did=datasetsdesc.did and datasetsdesc.jobname=split(jobs.jobname) group by  datasetsdesc.jobdesc";
#       my $sql = "SELECT jobdesc FROM DatasetsDesc WHERE dataset='$query'";
       my $r5=$self->{sqlserver}->Query($sql);
       if(defined $r5->[0][0]){
        foreach my $template (@{$r5}){
         print "<option value=\"$template->[1]\">$template->[1] </option>\n";
        }
       } else {
         print "<option value=\"Any\">ANY </option>\n";
       }
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
     htmlTextField("Build Number ","number",14,0.,"QBuildNum");
     htmlTableEnd();
    htmlTableEnd();
# Build Number



# Output format
   print "<b><font color=green> Output :  </font><INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"ALL\" > Full Listing\n";
   print "&nbsp;<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"RUNS\"> Raw file names\n";
   print "&nbsp;<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"FILES\"> Root file names\n";
   print "<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"SUMM\" CHECKED> Summary \n";
   print "<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"ROOT\"> ROOT Analysis Filename <INPUT TYPE=\"text\" name=\"ROOT\">";

   print "<br><TR></TR>";
   print "<br><TR></TR>";
   print "<b><font color=blue> ROOT Files \@CERN  </font>\n";
   print "<br><TR></TR>";
   print "<b><font color=blue> Access Mode  </font>\n";
   print "<INPUT TYPE=\"radio\" NAME=\"ROOTACCESS\" VALUE=\"NFS\" CHECKED> NFS \n";
   print "<INPUT TYPE=\"radio\" NAME=\"ROOTACCESS\" VALUE=\"NFSONLY\" > NFSONLY \n";
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
#     print "<INPUT TYPE=\"hidden\" NAME=\"DataMC\" VALUE=\"0\">\n";
     print "<INPUT TYPE=\"hidden\" NAME=\"SQLQUERY\" VALUE=\"$sql\">\n";
     print "<input type=\"submit\" name=\"queryDB04\" value=\"DoQuery\">        ";
     print "</form>\n";

     htmlReturnToQuery();
   htmlBottom();
  } elsif ($self->{q}->param("queryDB04") eq "ContinueD" or $self->{q}->param("queryDB04") eq "ContinueDMC" ) {
      if($self->{q}->param("queryDB04") eq "ContinueD" ){
          $self->{q}->param("DataMC",1);
      }
      else{
          $self->{q}->param("DataMC",11);
      }
     my $query=$q->param("QPartD");
     if(not defined $query){
         $query="AnyData";
     }
     my $dsname = $query;
     my $dsver = '';
     if ($query =~ /_v/) {
         my @junk = split /_v/, $query;
         if (scalar @junk >=2) {
             $dsname = $junk[0];
             $dsver = "v".$junk[1];
         }
     }
    my $qpp=$q->param("QPPer");

     foreach my $dataset (@{$self->{DataSetsT}}){
      if($dataset->{name} eq $dsname){
                 foreach $cite (@{$dataset->{jobs}}){
                 if(not ($cite->{filename} =~/^\./)){
                  push @tempnam, $cite->{filename};
                  $hash->{$cite->{filename}}=$cite->{filedesc};
                  push @desc, $hash ->{$cite->{filename}};
              }
           }
        }
      }
   htmlTop();
    $self->htmlAMSHeader("AMS-02 Data Database Query Form");
    print "<ul>\n";
    htmlMCWelcome();
    print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";
     print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
      print "<tr><td><b><font color=\"green\">Datasets </font></b>\n";
      print "</td><td>\n";
      print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
       print "<tr valign=middle><td align=left><b><font size=3 color=green> $query</b></td> <td colspan=1>\n";
       print "<INPUT TYPE=\"hidden\" NAME=\"QPartD\" VALUE=\"$dsname\">\n";
       print "<INPUT TYPE=\"hidden\" NAME=\"Version\" VALUE=\"$dsver\">\n";
       print "<INPUT TYPE=\"hidden\" NAME=\"QPPer\" VALUE=\"$qpp\">\n";
      htmlTableEnd();
     if ($query ne "AnyData" and $query ne "ANYDATA" and $query ne "anydata") {
# Job Template
      print "<tr><td><b><font color=\"green\">Template</font></b>\n";
      print "</td><td>\n";
      print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
       print "<tr valign=middle><td align=left><b><font size=\"-1\"> </b></td> <td colspan=1>\n";
       print "<select name=\"QTempDataset\" >\n";
       my $i=0;
       print "<option value=\"Any\"> ANY </option>\n";
      if ($query =~ "Any") {
         print "<option value=\"Any\">ANY </option>\n";
      } else {
           my $qpp=$q->param("QPPer");
           my $jobspid="";
           if(defined $qpp){
              if(  $qpp>0){
               $jobspid="jobs.pid=$qpp and ";
           }
              elsif ($qpp==-1){
                  $jobspid=$self->getactiveppstring();
              }
          }
          my $sql="select count(jobs.jid),datasetsdesc.jobdesc from jobs,datasetsdesc,datasets where $jobspid jobs.did=datasets.did and datasets.did=datasetsdesc.did and datasetsdesc.dataset='$dsname' and datasets.version like '\%$dsver\%' and datasetsdesc.jobname=split(jobs.jobname) group by  datasetsdesc.jobdesc";
#       my $sql = "SELECT jobdesc FROM DatasetsDesc WHERE dataset='$query'";
       my $r5=$self->{sqlserver}->Query($sql);
       if(defined $r5->[0][0]){
        foreach my $template (@{$r5}){
         print "<option value=\"$template->[1]\">$template->[1] </option>\n";
        }
       } else {
         print "<option value=\"Any\">ANY </option>\n";
       }
      }
        print "</select>\n";
        print "</b></td></tr>\n";
      htmlTableEnd();
   }
     print "<tr><td><b><font color=\"green\">Job Parameters</font></b>\n";
     print "</td><td>\n";
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
      print "<tr valign=middle><td align=left><b><font size=\"-1\"> Trigger </b></td> <td colspan=1>\n";
     htmlTextField("Build Number ","number",14,0.,"QBuildNum");
     htmlTableEnd();
    htmlTableEnd();
# Build Number



# Output format
   print "<b><font color=green> Output :  </font><INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"ALL\" > Full Listing\n";
   print "&nbsp;<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"RUNS\"> Raw File Names\n";
   print "&nbsp;<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"FILES\"> Root File Names\n";
   print "<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"SUMM\" CHECKED> Summary \n";
   print "<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"ROOT\"> ROOT Analysis Filename <INPUT TYPE=\"text\" name=\"ROOT\">";
   print "<br><TR></TR>";
   print "<br><TR></TR>";
   print "<b><font color=blue> ROOT Files \@CERN  </font>\n";
   print "<br><TR></TR>";
   print "<b><font color=blue> Access Mode  </font>\n";
   print "<INPUT TYPE=\"radio\" NAME=\"ROOTACCESS\" VALUE=\"NFS\" CHECKED> NFS \n";
   print "<INPUT TYPE=\"radio\" NAME=\"ROOTACCESS\" VALUE=\"NFSONLY\" > NFSONLY \n";
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
     my $dm=$self->{q}->param("DataMC");
   #  die " $dm nah ";
     if(not defined $dm){
         $dm=1;
     }
     print "<INPUT TYPE=\"hidden\" NAME=\"DataMC\" VALUE=\"$dm\">\n";
     print "<INPUT TYPE=\"hidden\" NAME=\"SQLQUERY\" VALUE=\"$sql\">\n";
     print "<input type=\"submit\" name=\"queryDB04\" value=\"DoQueryD\">        ";
     print "</form>\n";

     htmlReturnToQuery();
   htmlBottom();
  } else {
   htmlTop();
    $self->htmlAMSHeader("AMS-02  Database Query Form");
    print "<ul>\n";
    htmlMCWelcome();
    print "<p>\n";
    print "<TR><B><font color=green size= 4> Select <i>Dataset</i> and click <font color=red> Continue </font> or <i> Run/Job/DST  ID </i> and click <font color=red> Submit </font> </font>";
    print "<p>\n";
#    print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";

      $sql     = "SELECT NAME, DID   FROM ProductionSet  ORDER BY DID";
      my $ret = $self->{sqlserver}->Query($sql);
       my @period=();
       my @periodid=();
       $period[1]="Any";
       $periodid[1]=0;
      $period[0]="Any Active";
       $periodid[0]=-1;
        foreach my $pp  (@{$ret}){
       push @period, trimblanks($pp->[0]);
       push @periodid, $pp->[1];
      }


    print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";
 print "<tr valign=middle><td align=left><b><font size=\"-1\"> Production Period : </b></td> <td colspan=1>\n";
            print "<select name=\"QPPer\" id=\"QPPerId\" onchange='check_prodset(this.form.QPPer);'>\n";
            my $ii=0;
            foreach my  $template (@periodid) {
                 print "<option value=\"$template\">$period[$ii] </option>\n";
                 $ii++;
            }
            print "</select>\n";
      print "</b></td></tr>\n";


    print "<TABLE BORDER=\"1\" WIDTH=\"50%\">";
     print "<tr><td valign=\"middle\"><b><font color=\"blue\" size=\"3\">Datasets (MC Raw Files and/or Root Files Production)</font></b></tr>\n";
     print "</td><td>\n";
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     print "<tr><td><font size=\"-1\"<b>\n";
           print "</b><div id=\"sets\">
                   <span id=\"pbany\"><INPUT TYPE=\"radio\" NAME=\"QPart\" VALUE=\"Any\" CHECKED>ANYMC<BR></span>\n";
           print "</b></font></td></tr>\n";
           my @datasets = ();
           my $sql = "SELECT datasetsdesc.dataset, datasets.version FROM DatasetsDesc,datasets  where datasets.did=datasetsdesc.did and datasets.datamc=0 group by dataset, version";
           my $r5=$self->{sqlserver}->Query($sql);
           if(defined $r5->[0][0]){
            foreach my $ds (@{$r5}){
                my $dsexist = 0;
                foreach my $d (@datasets) {
                    if ($d eq $ds->[0]) {
                        $dsexist = 1;
                        next;
                    }
                }
                if ($dsexist == 0) {push @datasets, $ds;}
            }
           }
            my $id=1;
           foreach my $dataset (@datasets) {
             print "<span id=\"pb$id\"><INPUT TYPE=\"radio\" NAME=\"QPart\" ID=$id VALUE=$dataset->[0]_$dataset->[1]><span id=b$id>$dataset->[0]</span><BR></span>\n";
             $id++;
#              print "</b></font></td></tr>\n";
           }
        print "</div><script LANGUAGE=\"javascript\">\nvar dsets=[];\n";
        foreach my  $template (@periodid) {
            if($template != 0){
                print "dsets.push([$template]);\n";
            }
        }
        my $dcnt=1;
        foreach my $dataset (@datasets) {
            my $sqlq = "select did, nvl(cnt,0) cnt from productionset left outer join (select jobs.pid as did, count(jobs.did) as cnt
                        FROM Runs, Jobs, datasets, productionset, datasetsdesc
                        WHERE Jobs.pid=productionset.did AND Jobs.JID=Runs.JID and datasets.did=jobs.did and datasetsdesc.dataset='$dataset->[0]' and datasetsdesc.did=datasets.did and datasets.version = '$dataset->[1]' and
                                Runs.Status='Completed' group by jobs.pid) using(did) union all select -1 as did, count(jobs.did) as cnt
                    FROM Runs, Jobs, datasets, productionset, datasetsdesc
                     WHERE Jobs.pid=productionset.did AND Jobs.JID=Runs.JID and datasets.did=jobs.did and datasetsdesc.dataset='$dataset->[0]' and productionset.status='Active' and datasetsdesc.did=datasets.did and productionset.vdb = datasets.version and datasets.version='$dataset->[1]' and
                            Runs.Status='Completed' order by did";
            my $res=$self->{sqlserver}->Query($sqlq);
            my $i=0;
            if(defined $res->[0][0]){
                foreach my $resq (@{$res}) {
                    print "dsets[$i][$dcnt]=$resq->[1];\n";
                    $i++;
                }
            }
            $dcnt++;
        }
        print "function check_prodset(dropdown){
        var index  = dropdown.selectedIndex
        var id = dropdown.options[index].value
        if(id==0){
            for(var j=1; j<dsets[0].length; j++){
                element=document.getElementById(j);
                element.style.display = 'inline';
                element.disabled=false;
                element=document.getElementById('b'+j);
                element.style.color = 'black';
                element.style.fontWeight = 'normal';
            }
            for(var j=1; j<dsetsdata[0].length; j++){
                element=document.getElementById(j+1000);
                element.style.display = 'inline';
                element.disabled=false;
                element=document.getElementById('b'+(j+1000));
                element.style.color = 'black';
                element.style.fontWeight = 'normal';
            }
            for(var j=1; j<dsetsdata2[0].length; j++){
                element=document.getElementById(j+2000);
                element.style.display = 'inline';
                element.disabled=false;
                element=document.getElementById('b'+(j+2000));
                element.style.color = 'black';
                element.style.fontWeight = 'normal';
            }
        }else{
            for(var i=0; i<dsets.length; i++){
                if(dsets[i][0]==id){
                    for(var j=1; j<dsets[i].length; j++){
                        element=document.getElementById(j);
                        if(dsets[i][j]>0){
                            element.disabled=false;
                            element.style.display = 'inline';
                            element=document.getElementById('b'+j);
                            element.style.color = 'black';
                            element.style.fontWeight = 'normal';
                        }else{
                            element.disabled=true;
                            element.style.display = 'none';
                            element=document.getElementById('b'+j);
                            element.style.color = \"tomato\";
                            element.style.fontWeight = 'bold';
                        }
                    }
                }
                if(dsetsdata[i][0]==id){
                    for(var j=1; j<dsetsdata[i].length; j++){
                        element=document.getElementById(j+1000);
                        if(dsetsdata[i][j]>0){
                            element.disabled=false;
                            element.style.display = 'inline';
                            element=document.getElementById('b'+(j+1000));
                            element.style.color = 'black';
                            element.style.fontWeight = 'normal';
                        }else{
                            element.disabled=true;
                            element.style.display = 'none';
                            element=document.getElementById('b'+(j+1000));
                            element.style.color = \"tomato\";
                            element.style.fontWeight = 'bold';
                        }
                    }
                }
                if(dsetsdata2[i][0]==id){
                    for(var j=1; j<dsetsdata2[i].length; j++){
                        element=document.getElementById(j+2000);
                        if(dsetsdata2[i][j]>0){
                            element.disabled=false;
                            element.style.display = 'inline';
                            element=document.getElementById('b'+(j+2000));
                            element.style.color = 'black';
                            element.style.fontWeight = 'normal';
                        }else{
                            element.disabled=true;
                            element.style.display = 'none';
                            element=document.getElementById('b'+(j+2000));
                            element.style.color = \"tomato\";
                            element.style.fontWeight = 'bold';
                        }
                    }
                }

            }
        }
        sort_prodsets();
    }
function sort_prodsets() {
    var disabled=[];
    var disableddata=[];
    var disableddata2=[];
    var d = document.getElementById('sets');
    for(var j=1; j<dsets[0].length; j++){
        element=document.getElementById('pb'+j);
        status=document.getElementById(j).disabled;
        if(status==true){
            disabled.push(element.id);
        }else{
            d.appendChild(element);
        }
    }
    element=document.getElementById('pbany');
    d.appendChild(element);
    for(var j=0; j<disabled.length; j++){
        d.appendChild(document.getElementById(disabled[j]));
    }
    var d = document.getElementById('setsdata');
    for(var j=1; j<dsetsdata[0].length; j++){
        element=document.getElementById('pb'+(j+1000));
        status=document.getElementById(j+1000).disabled;
        if(status==true){
            disableddata.push(element);
        }else{
            d.appendChild(element);
        }
    }
    element=document.getElementById('pbanydata');
    d.appendChild(element);
    for(var j=0; j<disableddata.length; j++){
        d.appendChild(disableddata[j]);
    }
    var d = document.getElementById('setsdata2');
    for(var j=1; j<dsetsdata2[0].length; j++){
        element=document.getElementById('pb'+(j+2000));
        status=document.getElementById(j+2000).disabled;
        if(status==true){
            disableddata2.push(element);
        }else{
            d.appendChild(element);
        }
    }
    element=document.getElementById('pbanydata2');
    d.appendChild(element);
    for(var j=0; j<disableddata2.length; j++){
        d.appendChild(disableddata2[j]);
    }
}\n";
        print "</script>\n";
    
        
#js end

        htmlTableEnd();
      htmlTableEnd();
   print "<p><br>\n";
#     print "<INPUT TYPE=\"hidden\" NAME=\"DataMC\" VALUE=\"0\">\n";
     print "<input type=\"submit\" name=\"queryDB04\" value=\"Continue\">        ";
     print "</form>\n";
      print "<tr></tr>\n";

    print "<TABLE BORDER=\"1\" WIDTH=\"50%\">";
     print "<tr><td valign=\"middle\"><b><font color=\"blue\" size=\"3\">Datasets (MC Root Files Only Production)</font></b></tr>\n";
     print "</td><td>\n";
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     print "<tr><td><font size=\"-1\"<b>\n";
           print "</b><div id=\"setsdata\">
                   <span id=\"pbanydata\"><INPUT TYPE=\"radio\" NAME=\"QPartD\" VALUE=\"AnyData\" CHECKED>ANYDATA<BR></span>\n";
           print "</b></font></td></tr>\n";
           $#datasets = -1;
           
           $sql = "SELECT name, version FROM datasets  where datamc=11";
#           $sql = "SELECT dataset FROM DatasetsDesc,datasets  where datasets.did=datasetsdesc.did and datasets.datamc=1 group by dataset";
           $r5=$self->{sqlserver}->Query($sql);
           if(defined $r5->[0][0]){
            foreach my $ds (@{$r5}){
                push @datasets, $ds->[0];
            }
           }
       $id=1001;
           foreach my $dataset (@datasets) {
             print "<span id=pb$id><INPUT TYPE=\"radio\" ID=$id NAME=\"QPartD\" VALUE=$dataset><span id=b$id>$dataset</span><BR></span>\n";
             $id++;
#              print "</b></font></td></tr>\n";
           }
        print "</div>";
#js start
        my $sqlq = "select did ";
        foreach my $dataset (@datasets) {
            $sqlq = $sqlq.",nvl(\"$dataset\",0) \"$dataset\" ";
        }
        $sqlq = $sqlq."from productionset ";
        foreach my $dataset (@datasets) {
            $sqlq = $sqlq."left outer join (
select jobs.pid as did, count(jobs.did) as \"$dataset\"
                    FROM datasets, dataruns, productionset, jobs
                     where productionset.did=jobs.pid and datasets.did=jobs.did and dataruns.jid=jobs.jid and datasets.name='$dataset' and
                            dataruns.Status='Completed' group by jobs.pid) using(did)";
        }
        $sqlq=$sqlq." union all (select * from (select -1 as did from dual) ";
        foreach my $dataset (@datasets) {
            $sqlq=$sqlq."left outer join (select -1 as did, count(jobs.did) as \"$dataset\"
                    FROM datasets, dataruns, productionset, jobs
                     where productionset.did=jobs.pid and datasets.did=jobs.did and productionset.status='Active' and dataruns.jid=jobs.jid  and datasets.name='$dataset' and
                            dataruns.Status='Completed') using(did)"
        }
#        print($sqlq);
        my $res=$self->{sqlserver}->Query($sqlq.") order by did");
        if(defined $res->[0][0]){
            print "<script LANGUAGE=\"javascript\">\nvar dsetsdata=[];\n";
            foreach my $resq (@{$res}) {
                my $jstr = "dsetsdata.push([";
                foreach my $subres (@{$resq}) {
                    $jstr = $jstr."$subres,";
                }
                $jstr = substr($jstr, 0, -1);
                print $jstr."]);\n";
            }
            print "</script>\n";
        }
        print "<script LANGUAGE=\"javascript\">";
        print "document.getElementById('QPPerId').value = '-1';";
        print "check_prodset(document.getElementById('QPPerId').form.QPPer);";
        print "</script>";
        
# end

        htmlTableEnd();
      htmlTableEnd();
   print "<p><br>\n";
#     print "<INPUT TYPE=\"hidden\" NAME=\"DataMC\" VALUE=\"11\">\n";
     print "<input type=\"submit\" name=\"queryDB04\" value=\"ContinueDMC\">        ";
     print "</form>\n";
      print "<tr></tr>\n";




    print "<TABLE BORDER=\"1\" WIDTH=\"50%\">";
     print "<tr><td valign=\"middle\"><b><font color=\"blue\" size=\"3\">Datasets (Data Root Files Production)</font></b></tr>\n";
     print "</td><td>\n";
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     print "<tr><td><font size=\"-1\"<b>\n";
           print "</b><div id=\"setsdata2\">
                   <span id=\"pbanydata2\"><INPUT TYPE=\"radio\" NAME=\"QPartD\" VALUE=\"AnyData\" CHECKED>ANYDATA<BR></span>\n";
           print "</b></font></td></tr>\n";
           $#datasets = -1;
           
           $sql = "SELECT name, version FROM datasets  where datamc=1";
#           $sql = "SELECT dataset FROM DatasetsDesc,datasets  where datasets.did=datasetsdesc.did and datasets.datamc=1 group by dataset";
           $r5=$self->{sqlserver}->Query($sql);
           if(defined $r5->[0][0]){
            foreach my $ds (@{$r5}){
                push @datasets, $ds;
            }
           }
       $id=2001;
           foreach my $dataset (@datasets) {
             print "<span id=pb$id><INPUT TYPE=\"radio\" ID=$id NAME=\"QPartD\" VALUE=$dataset->[0]_$dataset->[1]><span id=b$id>$dataset->[0]</span><BR></span>\n";
             $id++;
#              print "</b></font></td></tr>\n";
           }
        print "</div>";
#js start
         $sqlq = "select did ";
        foreach my $dataset (@datasets) {
            $sqlq = $sqlq.",nvl(\"$dataset->[0]_$dataset->[1]\",0) \"$dataset->[0]_$dataset->[1]\" ";
        }
        $sqlq = $sqlq."from productionset ";
        foreach my $dataset (@datasets) {
            $sqlq = $sqlq."left outer join (
select jobs.pid as did, count(jobs.did) as \"$dataset->[0]_$dataset->[1]\"
                    FROM datasets, dataruns, productionset, jobs
                     where productionset.did=jobs.pid and datasets.did=jobs.did and dataruns.jid=jobs.jid and datasets.name='$dataset->[0]' and datasets.version = '$dataset->[1]' and 
                            dataruns.Status='Completed' group by jobs.pid) using(did)";
        }
        $sqlq=$sqlq." union all (select * from (select -1 as did from dual) ";
        foreach my $dataset (@datasets) {
            $sqlq=$sqlq."left outer join (select -1 as did, count(jobs.did) as \"$dataset->[0]_$dataset->[1]\"
                    FROM datasets, dataruns, productionset, jobs
                     where productionset.did=jobs.pid and datasets.did=jobs.did and productionset.status='Active' and dataruns.jid=jobs.jid and datasets.name='$dataset->[0]' and datasets.version='$dataset->[1]' and
                            dataruns.Status='Completed') using(did)"
        }
#        print($sqlq);
         $res=$self->{sqlserver}->Query($sqlq.") order by did");
   #die "$res->[0][0]";
        if(defined $res->[0][0]){
            print "<script LANGUAGE=\"javascript\">\nvar dsetsdata2=[];\n";
            foreach my $resq (@{$res}) {
                my $jstr = "dsetsdata2.push([";
                foreach my $subres (@{$resq}) {
                    $jstr = $jstr."$subres,";
                }
                $jstr = substr($jstr, 0, -1);
                print $jstr."]);\n";
            }
            print "</script>\n";
        }
        print "<script LANGUAGE=\"javascript\">";
        print "document.getElementById('QPPerId').value = '-1';";
        print "check_prodset(document.getElementById('QPPerId').form.QPPer);";
        print "</script>";
# end

        htmlTableEnd();
      htmlTableEnd();
   print "<p><br>\n";
#     print "<INPUT TYPE=\"hidden\" NAME=\"DataMC\" VALUE=\"1\">\n";
     print "<input type=\"submit\" name=\"queryDB04\" value=\"ContinueD\">        ";
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
 print "<p><br>\n";
       print "</b> <INPUT TYPE=\"radio\" NAME=\"QTypeD\" VALUE=\"MC\" CHECKED>MC<BR>";
       print "<b> <INPUT TYPE=\"radio\" NAME=\"QTypeD\" VALUE=\"Data\" >Data<BR>\n";
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
 print "<p><br>\n";
       print "</b> <INPUT TYPE=\"radio\" NAME=\"QTypeD\" VALUE=\"MC\" CHECKED>MC<BR>";
       print "<b> <INPUT TYPE=\"radio\" NAME=\"QTypeD\" VALUE=\"Data\" >Data<BR>\n";

        print "</form>\n";
        print "</table> \n";


       print "<br><p>\n";
        print "<tr></tr>\n";
        print "<p></p>\n";
        print "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\" width=\"100%\">\n";
        print "<tr><td valign=\"middle\" bgcolor=\"whitesmoke\"><font size=\"+2\"><B>\n";
        print "Find DataFile(s) : (eg 1073741826  or From-To) </B></font></td></tr></table> \n";
        print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";


 print "<tr valign=middle><td align=left><b><font size=\"-1\"> Production Period : </b></td> <td colspan=1>\n";
             print "<select name=\"QPPer\" >\n";
             my $iii=0;
             foreach my  $template (@periodid) {
              print "<option value=\"$template\">$period[$iii] </option>\n";
              $iii++;
            }
            print "</select>\n";
      print "</b></td></tr>\n";
   print "<BR>";
        print "<b>RunID : </b> <input type =\"text\" name=\"DataFileID\" value=\"now-24\">\n";
        print "<input type=\"submit\" name=\"getDataFileID\" value=\"Submit\"> \n";
        print "<b>RunType ( SCI, CAL, LAS, ANY) : </b> <input type =\"text\" name=\"DataFileType\" value=\"ANY\">\n";
        print "<b>SubDetectors ( Combination of TURESL3) : </b> <input type =\"text\" name=\"DataFileTypeS\" value=\"ANY\">\n";
        print "<INPUT TYPE=\"checkbox\" NAME=\"ShowDST\" VALUE=\"FCL\" CHECKED>ShowDST ";

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
# User's and cite info
           $self->  getProductionPeriods(0);
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
            print "<input type=\"hidden\" name=\"QMaxJobs\" value=\"$max_jobs\">";
            print "</b></font></td></tr>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
            print "<i><b>cite : </td><td><input type=\"text\" size=18 value=$cite name=\"CCA\" onFocus=\"this.blur()\"></i>\n";
            print "</b></font></td></tr>\n";
            htmlText("<i>Capacity </i>",$capacity_jobs);
            htmlText("<i>Completed jobs</i>",$completed_jobs);
            htmlText("<i>Total jobs </i>",$total_jobs);
            if($max_jobs>0){
                htmlText("<i>Allowed jobs </i>",$max_jobs);
            }else{
                print "<tr><td><font color=\"tomato\" size=\"2\"><b> <i>Allowed jobs </i> </td><td><font color=\"red\">$max_jobs</b></font></td></tr>";
            }
            print "</TABLE>\n";
# Datasets
            foreach my $mc (@productionPeriods) {
             if ($mc->{status} =~ 'Active') {
              print "<tr><td><b><font color=\"blue\" size=\"3\">Dataset </font><font color=\"tomato\" size=\"3\"> ($mc->{name} $mc->{vdb})";
              print "</font></b></td><td>";
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>";
              print "<tr><td><font size=\"-1\"<b>";
              my $firstdataset = 1;
              my $checked="CHECKED";
              foreach my $dataset (@{$self->{DataSetsT}}){
               if ($dataset->{version} eq $mc->{vdb}) {
                if ($firstdataset++ != 1) {
                   $checked="";
                }
#                print "</b>";
                my $minratio=0.00002;
                my $minsize=1000;
                if($dataset->{datamc}==1){
                    $minratio=0;
                    $minsize=10;
                }
                if(not $self->{CCTP}=~/$dataset->{datamc}/){
                    $minsize=20000000000;
                }
                if ($dataset->{eventstodo}/($dataset->{eventstotal}+1) < $minratio or  $dataset->{eventstodo}<$minsize or $max_jobs <= 0){
                  print "<tr><td><b><font color=\"tomato\"> $dataset->{name} </font></b></td></tr>";
#                  print "</b></font></td></tr>";
                  $firstdataset--;
                 }
                  else {
                    print "<INPUT TYPE=\"radio\" NAME=\"CTT\" VALUE= $dataset->{name} $checked>$dataset->{name}<BR>";
                 }
#                print "</b></font></td></tr>";
             }
           }
            print "</TABLE>\n";
          }
         }
  #          print "<tr><td>\n";
  #          print "<b><font color=\"red\" size=\"3\">Template</font></b>\n";
  #          print "</td><TD>\n";
  #          print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
  #          print "<tr><td><font size=\"-1\"</b>\n";
  #          print "<INPUT TYPE=\"radio\" NAME=\"CTT\" VALUE=\"Basic\" >Basic<BR>\n";
   #         print "<INPUT TYPE=\"radio\" NAME=\"CTT\" VALUE=\"Advanced\" >Advanced<BR>\n";
   #         print "</b></font></td></tr>\n";
   #         print "</TABLE>\n";
            print "</TABLE>\n";

    #        print "<TR><TD><font color=\"green\" size=\"3\"> ";
    #        print "Important : Basic and Advanced Templates are NOT PART OF MC PRODUCTION </font></TD></TR>\n";
            print "<br>\n";
            print "<TR><TD><font color=\"tomato\" size=\"3\">";
            print "Note : If dataset is not clickable, it means that all events already allocated for running jobs or processed or you have reached maximum allowed job number";
            print " </font></TD></TR>\n";
             if ($benchmarking == 1) {
              print "<TR></TR>\n";

              my $elapsed = timediff($td[3],$td[0]);
              my $tstrelapsed = timestr($elapsed);
              print "<TR><TD><font color=green size= 3> Total time : $tstrelapsed</font></TD></TR>\n";
               $elapsed = timediff($td[1],$td[0]);
               $tstrelapsed = timestr($elapsed);
               print "<TR><TD><font color=green size= 2> read datasets dir: $tstrelapsed</font></TD></TR>\n";
                $elapsed = timediff($td[2],$td[1]);
                $tstrelapsed = timestr($elapsed);
                print "<TR><TD><font color=green size= 2> read jobs dir: $tstrelapsed</font></TD></TR>\n";
                 $elapsed = timediff($td[3],$td[2]);
                 $tstrelapsed = timestr($elapsed);
                 print "<TR><TD><font color=green size= 2> parse ALL jobs for ALL datasets: $tstrelapsed</font></TD></TR>\n";
                 print "<TR><TD><font color=green size= 2> parse ALL jobs for ALL datasets: $tstrelapsed</font></TD></TR>\n";
             }

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
             $sql="insert into Cites values($cid,'$cite',0,'remote',$run,0,'$citedesc',0,$time,'0',0,1)";
             $self->{sqlserver}->Update($sql);

            }
            $sql="select mid from Mails";
            $ret=$self->{sqlserver}->Query($sql);
            my $mid=$ret->[$#{$ret}][0]+1;
            my $resp=$newcite;
            my $time=time();
            $cem = trimblanks($cem);
            $name= trimblanks($name);
            $sql="insert into Mails values($mid,'$cem',NULL,'$name',$resp,0,$cid,'Blocked',0,$time,0,0,'v00')";
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
             $sql="INSERT INTO Cites VALUES($cid,'$addcite',0,'remote',$run,0,'$newcitedesc',0,$time,'0',0,1)";
             $self->{sqlserver}->Update($sql);
# add responsible
             $sql="select MAX(mid) from Mails";
             $ret=$self->{sqlserver}->Query($sql);
             my $mid=$ret->[0][0]+1;
             my $resp=1;
             $sql="INSERT INTO Mails values($mid,'$cem',NULL,'$name',$resp,0,$cid,'Blocked',0,$time,0,0,'v0.00')";
            $self->{sqlserver}->Update($sql);
# add responsible info to Cites
             $sql="UPDATE Cites SET mid=$mid WHERE cid=$cid";
             $self->{sqlserver}->Update($sql);
             $self->{FinalMessage}=
             " Your request to register was succesfully sent to $sendsuc. Your account and cite registration will be done soon.";
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
            $sql="insert into Mails values($mid,'$cem',NULL,'$name',$resp,0,$cid,'Blocked',0,$time,0,0,'v0.00')";
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
     my $vdb =$self->{q}->param("VDB");
     my $time = time();
     if ($upl1 > 0) {
      $sql=
      "update Mails set timeu1=$upl0, timeu2=$upl1, vdb='$vdb', timestamp=$time WHERE address='$self->{CEM}'";
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
              htmlTextField("CPU clock","number",10,2000,"QCPU"," [MHz]");
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
             print "<b><font color=\"green\"> Production Mode</font></b><BR>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"Yes\" CHECKED><b> Standalone </b><BR>\n";
             print "</b></font></td></tr>\n";
           htmlTableEnd();
         }
         else {
             print "<tr><td>\n";
             print "<b><font color=\"green\"> Production Mode</font></b><BR>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"Yes\" ><b> Standalone </b><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"No\" CHECKED><b> Client </b><BR>\n";
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
#---???                   $ntdir = $fs->{disk}.$fs->{path}."/".$ActiveProductionSet;
                   $ntdir = $fs->{disk}.$fs->{path}."/tmp/Basic";
                   if ($fs->{disk} =~ /vice/) {
#---???                    $ntdir = $fs->{path}."/".$ActiveProductionSet;
                    $ntdir = $fs->{path}."/tmp/Basic";;
                   }
                 }
              }
DDTAB:         $self->htmlTemplateTable(" ");
               print "    <tr><td>\n";
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
# end of basic
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
              htmlTextField("CPU clock","number",10,2000,"QCPU"," [MHz]");
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
              htmlTextField("Setup","text",20,"AMS02/v4.00","QSetup"," ");
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
             print "<b><font color=\"green\"> Production Mode</font></b><BR>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"Yes\" CHECKED><b> Standalone </b><BR>\n";
             print "</b></font></td></tr>\n";
           htmlTableEnd();
         }
         else {
             print "<tr><td>\n";
             print "<b><font color=\"green\"> Production Mode</font></b><BR>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"Yes\" ><b> Standalone </b><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"No\" CHECKED><b> Client </b><BR>\n";
             print "</b></font></td></tr>\n";
           htmlTableEnd();
         }
#            $self->printJobParamTransferDST();

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
            print "<b> Max : X : <input type=\"number\" size=5 value=195 name=\"QXU\"></td>\n";
            print "<td width=\"30%\"><font size=\"2\">\n";
            print "<b> Y : <input type=\"number\" size=5 value=195 name=\"QYU\"></td>\n";
            print "<td width=\"30%\"><font size=\"2\">\n";
            print "<b> Z : <input type=\"number\" size=5 value=195 name=\"QZU\"> (cm)</td>\n";
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
#---???                   $ntdir = $fs->{disk}.$fs->{path}."/".$ActiveProductionSet;
                   $ntdir = $fs->{disk}.$fs->{path}."/tmp/Advanced";
                   if ($fs->{disk} =~ /vice/) {
#---???                    $ntdir = $fs->{path}."/".$ActiveProductionSet;
                    $ntdir = $fs->{path}."/tmp/Advanced";
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
#        if (1 or $self->{CCT} eq "remote"){
        if ($self->{CCT} ne "remote" or $self->{CCID} == 2){
            push @tempnam,"Any";
#        }
#        if (1 or $self->{CCT} eq "remote"){
           push @desc," Any";
       }
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
                  push @desc, $hash ->{$cite->{filename}};

#
# check if new datasetdescription needed to be inserted into db
#
#
#  glue file name to filedesc
#
                  my $fs=  $cite->{filedesc};
                  my $full="$self->{AMSSoftwareDir}/DataSets/$dataset->{name}/$cite->{filename}";
      my @sta  =stat $full;
      if ($#sta>9 and $sta[9]>1304807618){
                   $fs=$dataset->{name}."/".$cite->{filename}." ".$cite->{filedesc};
               }
                    my @junk=split 'Total Events',$fs;
                  if($#junk>0){
                      my $smdesc="";
                      my @junk2=split   " ",$junk[0];
                      foreach my $mdesc (@junk2){
                       $smdesc=$smdesc." and ";
                       $mdesc=~s/ //g; 
                       $smdesc= $smdesc." jobdesc like '%$mdesc%' ";
                      }
                  $sql="select dirpath from datasetsdesc where dataset='$dataset->{name}' $smdesc";
                   
                     my $ret=$self->{sqlserver}->Query($sql);
#                      die "$sql $ret->[0][0]";
                     if(not defined $ret->[0][0]){
                         $sql = "SELECT did from Datasets WHERE name='$dataset->{name}' and version='$dataset->{version}'";
                         $ret=$self->{sqlserver}->Query($sql);
                         if(defined $ret->[0][0]){
                         my $timenow = time();
                         my $buf=$cite->{filebody};
                         $buf=~ s/#/C /;
                         $buf =~ s/'/''/g;
                         $sql = "INSERT INTO DatasetsDesc Values($ret->[0][0],'$dataset->{name}','$self->{AMSSoftwareDir}/DataSets','$cite->{filename}','$junk[0]','$buf',$timenow,$timenow)";
#                         die "$sql";
                         $self->{sqlserver}->Update($sql);
                     }
                     }
                }
              }
             }
             htmlTop();
             $self->htmlTemplateTable("ZSelect Parameters for Dataset Request");
# Job Template
             print "<tr><td><b><font color=\"red\">Job Template</font></b>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr valign=middle><td align=left><b><font size=\"-1\"> Dataset : </b></td> <td colspan=1>\n";
             print "<select name=\"QTemp\" >\n";
             my $index_selected = int(rand(scalar @tempnam));
             if ($tempnam[0] eq "Any"){
                 $index_selected = $index_selected + 1;
             }
             my $i=0;
             my $str_selected = "";
             foreach my $template (@tempnam) {
              if ($i == $index_selected) {
                  $str_selected = "selected";
              }
              else {
                  $str_selected = "";
              }
              print "<option value=\"$template\" $str_selected>$desc[$i] </option>\n";
              $i++;
            }
            print "</select>\n";
            print "</b></td></tr>\n";

             print "<tr valign=middle><td align=left><b><font size=\"-1\"> Nick Name : </b></td> <td colspan=1>\n";
             print "<select name=\"QNick\" >\n";
             foreach my $p (@productionPeriods) {
              if ($p->{status} =~ 'Active') {
                if ($p->{vdb} =~ $dataset->{version}) {
                 print "<option value=\"$p->{name}.$p->{vdb}\">$p->{name} </option>\n";
                 $ProductionPeriod = $p->{name};
                 if ($p->{vdb} =~ /v3.00/ or (defined $dataset->{rootntuple} and $dataset->{rootntuple}==0)) {
                    $defROOT = "";
                    $defNTUPLE = "CHECKED";
                } else {
                    $defROOT    = "CHECKED";
                    $defNTUPLE  = " ";
                }
               }
              }
            }
            print "</select>\n";
            print "</b></td></tr>\n";

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
               htmlTextField("CPU clock","number",8,200,"QCPU"," [MHz]");
              } else {
               htmlTextField("CPU clock","number",8,2000,"QCPU"," [MHz]");
              }
            htmlText("<i>Capacity </i>",$capacity_jobs);
            htmlText("<i>Completed jobs</i>",$completed_jobs);
            htmlText("<i>Total jobs </i>",$total_jobs);
            htmlText("<i>Allowed jobs </i>",$max_jobs);
            htmlTableEnd();
# Job Parameters
              print "<tr><td><b><font color=\"blue\">Job Parameters</font></b>\n";
              print "</td><td>\n";
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
              $q->param("QEv",0);
              htmlTextField("CPU Time Limit Per Job","number",9,300000,"QCPUTime"," seconds (Native).");
              my $jbs=99;
if($self->{CCT} eq "local"){
               if($dataset->{datamc} !=0 and not defined $dataset->{MC}){
                 $jbs=19;
                 if($dataset->{name}=~/laser/ or $dataset->{name}=~/ecal/ or $dataset->{name}=~/zg1/){
                 $jbs=199;
                } 
               }
              
my $dbserver=$self->{dbserver};
       if(defined $q->param("SERVERNO")){
           $dataset->{serverno}=$q->param("SERVERNO");
       }
       elsif(defined $dataset->{serverno}){
           $q->param("SERVERNO",$dataset->{serverno});
       }

if( not defined $dbserver->{dbfile}){
     $dbserver=blessdb();
     $dbserver->{dbfile}=$self->ServerConnectDB($dataset->{serverno});
}
               my $rn=-1;     
               my $rntbr=-1;
               my $rntfi=-1;
    if($#{$self->{arpref}} <0 ){
        $self->ServerConnect($dataset->{serverno});
     }
        foreach my $arsref (@{$self->{arpref}}){
            try{
                $rn=$arsref->getRunsTotal();
            }
            catch CORBA::SystemException with{
                $rn=DBServer::GetRunsNumber($dbserver);
            };
        }
               my $maxr=512;
               if($rn>=0){
                if($rn<$maxr){
                 $jbs=$maxr-$rn;
                 if($jbs<$maxr/2){
                   $jbs=$maxr/2;
                 }
                }
                else{
        foreach my $arsref (@{$self->{arpref}}){
            try{
                $rntbr=$arsref->getRunsNumber("ToBeRerun");
                $rntfi=$arsref->getRunsNumber("Finished");
            }
            catch CORBA::SystemException with{
                 $rntbr=DBServer::GetRunsNumber($dbserver,"ToBeRerun");
                 $rntfi=DBServer::GetRunsNumber($dbserver,"Finished");
            };
        }

                 if($rntbr>=0 and $rntfi>=0){
#                   my $tot=int(($rn-$rntbr-$rntfi)/2+$rntfi/4+$rntbr);
                   my $tot=int(($rn-$rntbr-$rntfi)/4+$rntfi/16+$rntbr);
                   $jbs=$maxr-$tot;
                   if($jbs<=0){
                    $jbs=49-$rntbr;
                    if($jbs<=0){
                    $jbs=9;
                    }
                   }
                 }
                }
               }
          }
              if($max_jobs<$jbs){
                $jbs=$max_jobs;
              }
       if(defined $q->param("SERVERNO")){
           $dataset->{serverno}=$q->param("SERVERNO");
       }
       elsif(defined $dataset->{serverno}){
           $q->param("SERVERNO",$dataset->{serverno});
       }
              my $ds="";
              if(defined $dataset->{serverno}){
                 $ds=" ServerNo  $dataset->{serverno}  ";
              }


              htmlTextField("Total Jobs Requested $ds","number",7,$jbs,"QRun"," ");
                 if($self->{CCT} eq "local" or $dataset->{datamc}==1){
   print qq`
<INPUT TYPE="checkbox" NAME="ForceCpuLimit" VALUE="FCL" >Force CPULimit<BR>
`;
}
                 else{
   print qq`
<INPUT TYPE="checkbox" NAME="ForceCpuLimit" VALUE="FCL" CHECKED>Force CPULimit (Untick for NON-BATCH jobs)<BR>
`;
                 }
              htmlTextField("Total  Real Time Required","number",3,21,"QTimeOut"," (days)");
              my ($rid) = $self->getRID();
              htmlHiddenTextField("rid","hidden",12,$rid,"QRNDMS"," ");
              htmlText("<i>rndm sequence number </i>",abs($rid));
            htmlTableEnd();
            if ($self->{CCT} eq "remote") {
             print "<tr><td>\n";
             print "<b><font color=\"green\"> Production Mode</font></b><BR>\n";

             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"Yes\" CHECKED><b> Standalone </b><BR>\n";
             if ($self->{CCID} == 2) {
                 print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"No\" ><b> Client </b><BR>\n";
             }
             else {
                 print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"No\" DISABLED=\"Yes\"><b> Client </b><BR>\n";
             }
             print "</b></font></td></tr>\n";
           htmlTableEnd();
         }
         else {
             print "<tr><td>\n";
             print "<b><font color=\"green\"> Production Mode</font></b><BR>\n";

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
             print "<INPUT TYPE=\"radio\" NAME=\"JST\" VALUE=\"C\" ><b> Custom </b><i> ";
             print "(specific for for cite <font color=\"green\"> $self->{CCA} </font>)</i><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"JST\" VALUE=\"G\" CHECKED><b> Generic </b><i> (AMS generic)</i><BR>\n";
             print "</b></font></td></tr>\n";
            htmlTableEnd();
           htmlTableEnd();
#           htmlFormEnd();
             if ($self->{CCT} ne "remote") {
               my $ntdir="Not Defined";
               my $path='/MC';
#          ugly take into account rawmc datasets
               if($dataset->{datamc} !=0 and not defined $dataset->{MC}){
                 $path='/Data';
               }
               if ($self->{eos} == 1) {
                   $ntdir = '/eosams'.$path."/".$ProductionPeriod."/$dataset->{name}";
               }
               else {
                   my $sqlfs="select disk,path,available from filesystems where status='Active' and isonline=1 and path='$path' ORDER BY priority DESC, available DESC";
                   my $fs = $self->{sqlserver}->Query($sqlfs);
                   if(not defined $fs->[0][0]){
                       $self->ErrorPlus("Unable to Obtain Active File System $sqlfs");
                   }
                   else{
                       $ntdir = $fs->[0][0].$fs->[0][1]."/".$ProductionPeriod."/$dataset->{name}";
                   }
               }

#               my $maxavail=0;
#               foreach my $fs (@{$self->{FilesystemT}}){
#                if ($fs->{available} > $maxavail) {
#                   $maxavail = $fs->{available};
#                   if ($fs->{disk} =~ /vice/) {
#                    $ntdir = $fs->{path}."/".$ProductionPeriod."/$dataset->{name}";
#                   }
#                 }
#               }
DDTAB:         htmlTable(" ");
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
           print "<INPUT TYPE=\"hidden\" NAME=\"QMaxJobs\" VALUE=$max_jobs>\n";
           print "<INPUT TYPE=\"hidden\" NAME=\"DID\" VALUE=$dataset->{did}>\n";
           print "<br>\n";
           print "<input type=\"submit\" name=\"ProductionQuery\" value=\"Submit Request\"> <b>(it will take a while to tar DB and execs)</b></br><br>";
           print "<b><a href=load.cgi?$self->{UploadsHREF}/Help.txt target=\"_blank\">H E L P </b>\n";
           htmlFormEnd();
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
          print $q->textfield(-name=>"QCPU",-default=>2000);
          print "<BR>";
                print "Total Number of Events ";
          print $q->textfield(-name=>"QEv",-default=>3000000);
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
          print $q->textfield(-name=>"QCPU",-default=>2000);
          print "<BR>";
                print "Total Number of Events ";
          print $q->textfield(-name=>"QEv",-default=>3000000);
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
          print $q->textfield(-name=>"QSetup",-default=>"AMS02/v4.00");
                print "Trigger Type ";
          print $q->textfield(-name=>"QTrType",-default=>"AMSParticle");
          print "<BR>";
                print "Output file Type ";
          print "<BR>";
#    $self->getProductionVersion();
   print qq`
<INPUT TYPE="radio" NAME="RootNtuple" VALUE="1=3 168=120000 170=$self->{Build} 2=" $defNTUPLE>Ntuple 
<INPUT TYPE="radio" NAME="RootNtuple" VALUE="1=0 168=18000000 170=$self->{Build} 127=-2 128=" $defROOT>RootFile<BR>
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
          print $q->textfield(-name=>"QCPU",-default=>2000);
          print "<BR>";
                print "Total Number of Events ";
          print $q->textfield(-name=>"QEv",-default=>3000000);
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
        if(defined($self->{q}->param("BasicQuery")) and $self->{q}->param("BasicQuery") eq "Save"     or
           defined($self->{q}->param("AdvancedQuery")) and $self->{q}->param("AdvancedQuery") eq "Save"  or
           defined($self->{q}->param("ProductionQuery")) and $self->{q}->param("ProductionQuery") eq "Save"){
            my $pass=$q->param("password");
            if($self->{CCT} ne "remote" or defined $pass){
             my $crypt=crypt($pass,"ams");
             my $vr=$self->{q}->param("FEM");
             if(!($crypt eq "amGzkSRlnSMUU" and ($vr=~/v4/ or $vr=~/AMS02MC/)) and !($crypt eq "am3SBM3U8rmqs" and $vr=~/v5/)) {
              $self->sendmailerror("User authorization failed", "$self->{CEM}");
              $self->ErrorPlus
                    ("User Authorization Failed.  All Your Activity is Logged. $vr");

             }
         }
         my $filename=$q->param("FEM");
         $q=get_state($filename);
         if(not defined $q){
          $self->ErrorPlus("Save State Expired. Please Start From the Very Beginning");
         }
         $self->{q}=$q;
         unlink $filename;
         my $version=$q->param("Version");
         #die "$version ";
         if( not $self->ServerConnect($version)){
            die " Unable to connect the server $version ";
         }          
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
        if (defined $stalone and $stalone eq "No") {
          $self->{dwldaddon}=0;
        } else {
          $self->{dwldaddon}=1;
        }
#
# 'C'/'G' - custom, generic
#
        my $jst=$q->param("JST");
        $self->{jst}=$jst;              
#
        my $aft=$q->param("AFT");
        my $templatebuffer=undef;
        my $templatehost=undef;
        my $templateprio=undef;
         my $tmps="";
        my $template=undef;
        my $did=$q->param("DID");
        my $timeout=3600*24*35;
        my $dataset=undef;
        foreach my $ds (@{$self->{DataSetsT}}){
            if (defined($ds->{did}) and $ds->{did}==$did){
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
       if(defined $q->param("SERVERNO") and defined $dataset){
           $dataset->{serverno}=$q->param("SERVERNO");
       }

        my $Any=-1;
        my $qrunno=$q->param("QRun");
        my $srunno=$q->param("QRun");
        if($qrunno<1){
           $qrunno=1;
        }elsif($qrunno>1000){
         $qrunno=1000;
        }
       if($srunno<1){
           $srunno=1;
        }elsif($srunno>1000){
         $srunno=1000;
        }
       if($max_jobs<$srunno){
            $srunno=$max_jobs;
        }
       if($max_jobs<$qrunno){
            $qrunno=$max_jobs;
        }
        my $runsave=undef;
        if($template eq "Any"){
          if ($self->{CCT} eq "remote" and $self->{CCID} != 2){
             my $index_selected = int(rand(scalar @{$dataset->{jobs}}));
             $template=${$dataset->{jobs}}[$index_selected]->{filename};
          }
          else {
          $Any=0;
          if(not defined $dataset or  $dataset->{datamc}!=1){
           $q->param("QRun",1);
           $q->param("QEv",0);
          }
          $template=${$dataset->{jobs}}[$Any]->{filename};
          }
        }
       if(defined $dataset and $dataset->{datamc}>0){
#  Data type dataset
#  03.12.2007 vc
         foreach my $tmp (@{$dataset->{jobs}}) {
            if($template eq $tmp->{filename} and $tmp->{TOTALEVENTS}>0){
                $q->param("QTag",$tmp->{TAG});
                $q->param("QRunList",$tmp->{RUNLIST});
                $q->param("QRunAList",$tmp->{RUNALIST});
                $templatebuffer=$tmp->{filebody};
                $templatehost=$tmp->{HOST};
                $templateprio=$tmp->{PRIO};
                if(defined $tmp->{THREADS}){
                $q->param("TTHREADS", $tmp->{THREADS});
                }
                $q->param("TFILENAME",$template);
                if(not defined $q->param("QCPUPEREVENT")){
                    $q->param("QCPUPEREVENT",$tmp->{CPUPEREVENTPERGHZ});
                }
                my $runno=$q->param("QRun");
                if(not $runno =~/^\d+$/ or $runno <1){
                    $runno=1;
                }
                if($runno>1000 ){
                    $runno=1000;
                }
                if($max_jobs<$runno){
                    $runno=$max_jobs;
                }
                $q->param("QRun",$runno);
                $q->param("QEventMi",$tmp->{EVENTMIN});
                $q->param("QRunMi",$tmp->{RUNMIN});
                $q->param("QRunMa",$tmp->{RUNMAX});
                if($tmp->{QTYPE} ne ""){
                  $q->param("QType",$tmp->{QTYPE});
                }
                last;
            }
        }
        if(not defined $templatebuffer){
            $self->ErrorPlus("Could not find file for $template template. $tmps");
        }
         my $ntdir=$q->param("NTDIR");
       if( $self->{CCT} eq "local"){
         if (  not defined $ntdir) {
             $self->ErrorPlus("The NTuples output directory NOT DEFINED");
         } else {
             if (not $ntdir =~ /\//  ) {
              $self->ErrorPlus("Invalid NTuples output directory : $ntdir");
            } else {
                $self->{AMSDSTOutputDir}=$ntdir;
            }
         }
       }
         my $cputime=200000;
        my $clock=3000;
        my $corr=1.00;
            if (defined $q->param("QCPUType")){
              $corr=$self->{cputypes}->{$q->param("QCPUType")};
            }

        my $cput=5;

        my $runno=$q->param("QRun");
        if($max_jobs<$runno){
            $runno=$max_jobs;
        }
        my $runmi=$q->param("QRunMi");
        my $runma=$q->param("QRunMa");
        if(not $runno =~/^\d+$/ or $runno <1 or $runno>1000){
             $self->ErrorPlus("Runs no $runno is out of range (1,1000)");
        }
#
#  get runs from database
#
           my $runlist="";
           my $tag="";
            if(defined $q->param("QTag") and $q->param("QTag")>=0){
               $tag=" and tag=$q->param('QTag')";
            }
            if(defined $q->param("QRunList")){
                my @junk=split   ",",$q->param("QRunList");
                $runlist=" and (";
                foreach my $r (@junk){
                    $runlist=$runlist." run=$r or ";
                }
                $runlist=$runlist." run=-1)";
            }
           my $runalist="";
            if(defined $q->param("QRunAList")){
                my @junk=split   ",",$q->param("QRunAList");
                $runalist=" and (";
                foreach my $r (@junk){
                    $runalist=$runalist." run!=$r and ";
                }
                $runalist=$runalist." run!=-1)";
            }
           my $qtype="and  datafiles.type not like '%CAL%' ";
           if(defined $q->param("QType")){
               my $qt=$q->param("QType");
               $qtype="and datafiles.type like '$qt%'";
            }
                 my $sql="select datafiles.run,datafiles.path,datafiles.paths , datafiles.fevent, datafiles.levent from datafiles where run>=$runmi and run<=$runma   and  datafiles.nevents>0 and (datafiles.status='OK' or datafiles.status='Validated')  $qtype $runlist  $runalist and run not in  (select run from dataruns,jobs where  dataruns.jid=jobs.jid and jobs.did=$dataset->{did} and jobs.jobname like '%$template') $tag order by datafiles.run desc";
          my $runsret=$self->{sqlserver}->Query($sql);
          $timeout=$q->param("QTimeOut");
          if(not $timeout =~/^-?(?:\d+(?:\.\d*)?|\.\d+)$/ or $timeout <1 or $timeout>40){
             $self->ErrorPlus("Time  $timeout is out of range (1,40) days. ");
          }
          $timeout=int($timeout*3600*24);
            #get job
          if(not defined $runsret){
            $self->ErrorPlus("No More Runs to do... ");
            return;
          }             
       my $job;
    if(not defined $job){
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
            $sql="select max(jid) from jobs where jobname like '$self->{CCA}.%'";
             my $resj=$self->{sqlserver}->Query($sql);
          if(defined $resj->[0][0] and $resj->[0][0]>= $res->[0][0]){
               my $mes="Cite $self->{CCA} maxrun/jid mismatch $resj->[0][0]  $res->[0][0] ";
               $sql="update Cites set maxrun=$resj->[0][0]+1 where name='$self->{CCA}'";
               $self->{sqlserver}->Update($sql);
               $self->ErrorPlus($mes);
          }
            my $time = time();
            $sql="update Cites set state=1, timestamp=$time where name='$self->{CCA}'";
            $self->{sqlserver}->Update($sql);
            $job=$res->[0][0];
        }
    }
# 4.12.03             my $switch=1<<27;
             my $switch=1<<$MAX_RUN_POWER;
             my $max=$switch-1;
             if (($job%$switch)+$runno >$max){
              foreach my $chop (@{$self->{MailT}}) {
              if($chop->{rserver}==1){
               my $address=$chop->{address};
               my $subject="AMS02MC Request Form: job Capacity Exceeded for Cite $self->{CCA} $job";
               my $message=" see subject";
               $self->sendmailmessage($address,$subject,$message);
               last;
              }
             }
             $self->ErrorPlus("Run Capacity Exceeds.");
             return;
          }

#       now everything is o.k except server check

        if (not $self->ServerConnect($dataset->{serverno})){
        foreach my $chop (@{$self->{MailT}}) {
              if($chop->{rserver}==1){
                  my $address=$chop->{address};
                  $self->sendmessage($address,"unable to connect to servers by $self->{CEM}"," ");
                  last;
              }
          }
            $self->ErrorPlus("Unable to Connect to Server.");
         }
        my $key='gbatch';
        $sql="select myvalue from Environment where mykey='".$key."'";
        my $ret=$self->{sqlserver}->Query($sql);
        if( not defined $ret->[0][0]){
            $self->ErrorPlus("unable to retreive gbatch name from db");
        }
        #$dataset->{version}='v4.00';

        my $gbatch="";
        my $gbatchcomp="";
        if($dataset->{g4}=~/g4/){
           $gbatchcomp=$ret->[0][0].".$dataset->{version}".".$dataset->{datamc}"."$dataset->{build}";
           $gbatch=$gbatchcomp.".$dataset->{g4}";
        }
        else{
           $gbatch=$ret->[0][0].".$dataset->{version}".".$dataset->{datamc}"."$dataset->{build}";
           $gbatchcomp=$gbatch.".g4";
        }
        
        my @stag=stat "$self->{AMSSoftwareDir}/$gbatch";
        if($#stag<0){
              $self->ErrorPlus("Unable to find $self->{AMSSoftwareDir}/$gbatch on the Server ");
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

        $key='ami2root';
        $sql="select myvalue from Environment where mykey='".$key."'";
        $ret=$self->{sqlserver}->Query($sql);
        if( not defined $ret->[0][0]){
            $self->ErrorPlus("unable to retreive $key name from db");
        }
         my $am=$ret->[0][0];
         @stag2=stat "$self->{AMSSoftwareDir}/$am";
        if($#stag2<0){
              $self->ErrorPlus("Unable to find $am on the Server ");
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
        my $vdb ='XYZ';
        my $stt ='UNK';

        if($self->{CCT} eq "remote"){
#+
         $self->  getProductionPeriods(0);
         foreach my $mc (@productionPeriods) {
          if ($mc->{status} =~ 'Active') {
           my $vdb = $mc->{vdb};
           if ($vdb =~ $dataset->{version}) {
            foreach my $file (@{$self->{FileDB}}) {
             if ( $file =~ m/$vdb/ and $file =~/rddb/) {
                $filedb = "$self->{UploadsDir}/$file";
                last;
             }
            }
           }
          }
         }
          if (not defined $filedb) {
#            die " pp @productionPeriods ; $dataset->{version} ; $vdb  ; @{$self->{FileDB}} ";
            $self->ErrorPlus("unable to find corrsponding database file ($dataset->{name},$dataset->{version}, $vdb, $stt)");
         }
#-
        my @sta = stat $filedb;
        if($#sta<0 or $sta[9]-time() >86400*7 or $stag[9] > $sta[9] or $stag1[9] > $sta[9] or $stag2[9] > $sta[9]){
           $self->{senddb}=2;
        my $filen="$self->{UploadsDir}/ams02mcdb.tar.$job";
#        $key='dbversion';
#        $sql="select myvalue from Environment where mykey='".$key."'";
#        my $ret=$self->{sqlserver}->Query($sql);
#        if( not defined $ret->[0][0]){
#            $self->ErrorPlus("unable to retreive db version from db");
#        }
#
#        my $dbversion=$ret->[0][0];
        my $dbversion=$dataset->{version};
        my $i=system "mkdir -p $self->{UploadsDir}/$dbversion";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/*.dat $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/\*.dat";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/*.bin $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/\*.bin";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/*.txt $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/\*.txt";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/L* $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/L\*";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/A* $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/A\*";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/t* $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/t\*";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/T* $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/T\*";
       if($dbversion=~/v4/ or $dbversion=~/v5/){
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/ri* $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/ri\*";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/*.flux $self->{UploadsDir}/$dbversion";
        unlink "$self->{AMSDataDir}/$dbversion/\*.flux";
       }
        $i=system "tar -C$self->{UploadsDir} -h -cf $filen $dbversion 1>/dev/null 2>&1";
        if($i){
              $self->ErrorPlus("Unable to tar $self->{UploadsDir} $dbversion to $filen  $i    tar -C$self->{UploadsDir} -h -cf $filen $dbversion 1>/dev/null 2>&1");
         }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $gbatch  1>/dev/null 2>&1") ;
          if($i){
              $self->ErrorPlus("Unable to tar $gbatch to $filen ");
          }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $gbatch.64  1>/dev/null 2>&1") ;
          if($i){
              $self->ErrorPlus("Unable to tar $gbatch.64 to $filen ");
          }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $gbatchcomp  1>/dev/null 2>&1") ;
          if($i){
              $self->ErrorPlus("Unable to tar $gbatchcomp to $filen");
          }
         $i=system("tar -C$self->{CERN_ROOT} -uf $filen lib/flukaaf.dat  1>/dev/null 2>&1") ;
          if($i){
              $self->ErrorPlus("Unable to tar flukaaf.dat to $filen ");
          }

         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $nv  1>/dev/null 2>&1") ;
          if($i){
              $self->ErrorPlus("Unable to tar $nv to $filen  ");
          }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $gr  1>/dev/null 2>&1") ;
          if($i){
              $self->ErrorPlus("Unable to tar $gr to $filen ");
          }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $am  1>/dev/null 2>&1") ;
          if($i){
              $self->ErrorPlus("Unable to tar $am to $filen ");
          }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen prod/tnsnames.ora  1>/dev/null 2>&1") ;
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
#
         foreach my $mc (@productionPeriods) {
          if ($mc->{status} =~ 'Active') {
           my $vdb = $mc->{vdb};
           if ($vdb =~ $dataset->{version}) {
            foreach my $file (@{$self->{FileAttDB}}) {
             if ( $file =~ m/$vdb/) {
                $filedb_att = "$self->{UploadsDir}/$file";
                last;
             }
            }
           }
          }
         }

#         die $filedb, $filedb_att;
#        $filedb_att=~s/ams02/$dataset->{version}/;
        @sta = stat $filedb_att;

#         my @stag3=stat "$self->{AMSDataDir}/DataBase";
#        if($#stag3<0){
#              $self->ErrorPlus("Unable to find $self->{AMSDataDir}/DataBase on the Server ");
#        }
#        if(($#sta<0 or time()-$sta[9] >86400*180  or $stag3[9] > $sta[9] ) and $self->{dwldaddon}==1){

         my $dte=stat_adv ("$self->{AMSDataDir}/DataBase",0,0,1);
#        For the moment Never Update beacuse of timeout ; user ~/AMS/perl/makeaddon instead
        if((($#sta<0 or time()-$sta[9] >86400*180  or $dte > $sta[9] ) and $self->{dwldaddon}==1) and 0){

           $self->{sendaddon}=2;
        my $filen="$self->{UploadsDir}/ams02mcdb.addon.tar.$job";
        my $i=system "mkdir -p $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/MagneticFieldMap* $self->{UploadsDir}/DataBase";
        $key='dbversion';
        $sql="select myvalue from Environment where mykey='".$key."'";
        my $ret=$self->{sqlserver}->Query($sql);
        if( not defined $ret->[0][0]){
            $self->ErrorPlus("unable to retreive db version from db");
        }
#
#        my $dbversion=$ret->[0][0];
         my $dbversion=$dataset->{version};
          my $rtn=0;
           if($dbversion =~/v4/ or $dbversion =~/v5/){

           closedir THISDIR;
           my $suc=opendir THISDIR,"$self->{AMSDataDir}/DataBase";
           if(!$suc){
              $self->ErrorPlus("Unable to opendir $self->{AMSDataDir}/DataBase");            }
     my @allfiles=readdir THISDIR;
     closedir THISDIR;
       foreach my $file (@allfiles){
         my $newfile="$self->{AMSDataDir}/DataBase/".$file;
         if(readlink $newfile){
#           next;
         }
         my @sta=stat $newfile;
         if($sta[2]>32000){
          if($newfile =~/\.1$/){
           $i=system "ln -s $newfile $self->{UploadsDir}/DataBase";
         }
         }
         else{
    if ($file =~/^\./){
       next;
    }
        $i=system " rm -rf $self->{UploadsDir}/DataBase/$file";
        if($file=~/Shuttle/ or $file=~/Scaler/ or $file=~/Temperature/ or $file =~/s\.r/ or $file=~/s\.l/ or $file eq "EventStatusTable"){
           next;
        }
        $i=system " mkdir -p $self->{UploadsDir}/DataBase/$file";
           $suc=opendir THISDIR,"$newfile";
           if(!$suc){
              $self->ErrorPlus("Unable to opendir $self->{AMSDataDir}/DataBase/$file");            }
         my @allnew=readdir THISDIR;
         closedir THISDIR;
         foreach my $new (@allnew){
    if ($new =~/^\./){
       next;
    }
          my $newfile1="$self->{AMSDataDir}/DataBase/$file/$new";
          if(readlink $newfile1){
            next;
          }
          my @sta=stat $newfile1;
          if($sta[2]<32000){
            $i=system " rm -rf $self->{UploadsDir}/DataBase/$file/$new";
           $i=system " mkdir -p $self->{UploadsDir}/DataBase/$file/$new";
           $suc=opendir THISDIR,"$self->{AMSDataDir}/DataBase/$file/$new";
           if(!$suc){
              $self->ErrorPlus("Unable to opendir $self->{AMSDataDir}/DataBase/$file/$new");            }
         my @allvar=readdir THISDIR;
         closedir THISDIR;
         foreach my $var (@allvar){
          if($var=~/\.1\./){
           $i=system "ln -s $self->{AMSDataDir}/DataBase/$file/$new/$var $self->{UploadsDir}/DataBase/$file/$new/$var";
          }
         }

         }
        }
         }
       }

        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerCmnNoise";
        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerSigmas.r";
        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerSigmas.l";
        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerPedestals.r";
        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerPedestals.l";
        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerStatus.r";
        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerStatus.l";

        $rtn=system "tar -C$self->{UploadsDir} -h -czf $filen.gz DataBase  1>/dev/null 2>&1";
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
        $rtn=system "tar -C$self->{UploadsDir} -h -czf $filen.gz DataBase  1>/dev/null 2>&1";
           }
        if($rtn){
        #      $self->ErrorPlus("Unable to tar $self->{UploadsDir} DataBase to $filen.gz $rtn");
        }
#          $i=system("gzip -f $filen");
#                      if($i){
#              $self->ErrorPlus("Unable to gzip  $filen");
#          }
          $i=system("mv $filedb_att $filedb_att.o");
          $i=system("mv $filen.gz $filedb_att");
          unlink "$filedb_att.o";
       }
        elsif($sta[9]>$self->{TU2}){
            $self->{sendaddon}=2;
        }


# write readme file
        my $readme="$self->{UploadsDir}/README.$job";
        open(FILE,">".$readme) or die "Unable to open file $readme (specify the exact path and/or check dir protection)\n";
         my $data="readmestandalone_data";
          if(defined $dataset->{MC} and $dataset->{MC}==1){
           $data=$data."mc";
          }
        if($self->{dwldaddon}==1){
          $self->{tsyntax}->{headers}->{$data}=~s/ams02/$dataset->{version}/g;
         print FILE  $self->{tsyntax}->{headers}->{$data};
        }
        else{
         $self->{tsyntax}->{headers}->{readmecorba}=~s/ams02/$dataset->{version}/g;
         print FILE  $self->{tsyntax}->{headers}->{readmecorba};
        }
        my $sql = "SELECT dirpath FROM journals WHERE cid=$self->{CCID}";
        my $note='please contact vitali.choutko@cern.ch for details';
        my $ret = $self->{sqlserver}->Query($sql);

        if (defined $ret->[0][0]) {
         $note ="DST directory path : $ret->[0][0]/nt
                \n Journal files directory path : $ret->[0][0]/jou
                \n Validation log files directory path : $ret->[0][0]/log
                \n";
         }
        print FILE  $note;
        close FILE;
         my $i;
         if($Any<0 or $srunno eq $qrunno){
         $file2tar="$self->{UploadsDir}/$dataset->{version}script.$job.tar";
           $i=system("tar -C$self->{UploadsDir} -cf  $file2tar README.$job  1>/dev/null 2>&1");
     }
         else{
           $i=system("tar -C$self->{UploadsDir} -uf  $file2tar README.$job  1>/dev/null 2>&1");
         }
          if($i){
              $self->ErrorPlus("Unable to tar readme to $file2tar ");
          }
        unlink $readme;
     }





        my $runmax=$#{@{$runsret}}+1;
        if($runno>$runmax){
            $runno=$runmax;
        }
        

        for my $i (1 ... $runno){
         my $run=$runsret->[$i-1][0];
         my $fevent=$runsret->[$i-1][3];
         my $levent=$runsret->[$i-1][4];
         my $path=$runsret->[$i-1][1];
         my $paths=$runsret->[$i-1][2];
         if($paths=~/$run/){
           if(index($paths,$self->{afsroot})>=0){
            $path=$paths;
           }
           else{
            $path=$self->{afsroot}.$paths;
           }
         }
         #find buffer and patch it accordingly
         my $evts=100000000;
#read header
         my $buf=$self->{tsyntax}->{headers}->{$self->{CCT}};
         my $tmpb=$templatebuffer;
         if(not defined $buf){
             $self->ErrorPlus("Unable to find   header file. ");
         }
         my $sql = "SELECT dirpath FROM journals WHERE cid=$self->{CCID}";
         my $ret=$self->{sqlserver}->Query($sql);
         if(defined $ret->[0][0] && $self->{CCT} ne 'remote'){
             my $subs=$ret->[0][0]."/jou";
           #  $subs=~s/\//\\\//g;
           $buf=~s/ProductionLogDir/ProductionLogDir=$subs/;
             #die " $subs $buf \n";
          }
          if($run<1000000000){
         $buf=~ s/RUN=/RUN=$run/;
          }
          else{
         $buf=~ s/RUN=/RUN=$run/;
         }
         $buf=~ s/JOB=/JOB=$job/;
         my $cputf=sprintf("%.3f",$cput);

         $buf=~ s/CPULIM=/CPULIM=$cputf/;
         my $cpus=$q->param("QCPUTime");
         my $cpusf=sprintf("%.3f",$cpus);
         if(defined  $q->param("QCPUTime")){
          my $cpus=$q->param("QCPUTime");
          my $cpusf=sprintf("%.3f",$cpus);
          if($q->param("ForceCpuLimit")){
           $tmpb=~ s/TIME 3=/TIME 1=$cpusf 2=100. 3=/;
          }
         }
         my $nickname=$q->param("QNick");
         $buf=~ s/JOB=/NICKNAME=$nickname \nJOB=/;
         $buf=~ s/JOB=/DATASETNAME=$dataset->{name} \nJOB=/;
         if(defined $q->param("TFILENAME")){
         my @junk=split '\.',$q->param("TFILENAME");
         $buf=~ s/JOB=/JOBNAME=$junk[0] \nJOB=/;
         }
         if(defined $q->param("TTHREADS")){
         my $threads=$q->param("TTHREADS");
         $buf=~ s/JOB=/THREADS=$threads \nJOB=/;
         }
          $buf=~ s/JOB=/CPUTIME=$cpus \nJOB=/;
         my $rootntuple=$q->param("RootNtuple");
if(defined $dataset->{buildno} ){
    if($self->{Build}<0){
    $self->getProductionVersion;
    }
    my $tago="170=$self->{Build}";
    my $tagn="170=$dataset->{buildno}";
    $rootntuple=~s/$tago/$tagn/;
}
         $buf=~ s/ROOTNTUPLE=\d/ROOTNTUPLE=/;
         $buf=~ s/ROOTNTUPLE=/ROOTNTUPLE=\'$rootntuple\'/;
         $tmpb=~ s/ROOTNTUPLE=/C ROOTNTUPLE/g;
         $tmpb=~ s/IOPA \$ROOTNTUPLE\'/IOPA \$ROOTNTUPLE\'\n/;
         my $cputype=$q->param("QCPUType");
         $buf=~ s/JOB=/CPUTYPE=\"$cputype\" \nJOB=/;
         $buf=~ s/JOB=/CLOCK=$clock \nJOB=/;
         my $ctime=time();
         $buf=~ s/JOB=/SUBMITTIME=$ctime\nJOB=/;
#       add common template here
{
        my $dir="$self->{AMSSoftwareDir}/Templates";
        my $full=$dir."/common.job";
        open(FILEI,"<".$full) or die "Unable to open file $full \n";
        my $bufb;
        read(FILEI,$bufb,1638400) or next;
        close FILEI;
#       $tmpb=~ s/END\n!/$bufb\nEND\n!/;
       $tmpb=~ s/LIST\n/LIST\n$bufb\n/;
}
        if($dataset->{g4}=~/g4/){
        my $dir="$self->{AMSSoftwareDir}/Templates";
        my $full=$dir."/commong4.job";
        open(FILEI,"<".$full) or die "Unable to open file $full \n";
        my $bufb;
        read(FILEI,$bufb,1638400) or next;
        close FILEI;
#      $tmpb=~ s/TERM\n!/$bufb\nTERM\n!/;
       $tmpb=~ s/LIST\n/LIST\n$bufb\n/;

    }
         if($self->{CCT} eq "local"){
          $tmpb=~ s/\$RUNDIR\/\$RUN/\$RUNDIR/;
          if(length($path)>76){
              my $len=length($path);
              my $add=$len%4;
              if($add){
                  $add=4-$add;
              }
              for my $i (0...$add-1){
                  $path=$path." ";
              }
              $len=length($path);
              my $i=$len-76;
              while($i>0){
                  substr($path,$i,0)="'\n'";
                  $i=$i-76;
              }         
              $tmpb=~s/\$RUNDIR/$path/;
          }
          else{
           $buf=~ s/RUNDIR=/RUNDIR=$path/;
          }
           $buf=~ s/\$AMSProducerExec/$self->{AMSSoftwareDir2}\/$gbatch/g;
      }
         else{
          $buf=~ s/RUNDIR=/CRUNDIR=/;
          if(defined $dataset->{MC} and $dataset->{MC}==1) {
              $tmpb=~ s/\$RUNDIR/\$RUNDIR\/\$RUN.raw/;
          }
          else {
              $tmpb=~ s/\$RUNDIR/\$RUNDIR\/\$RUN/;
          }
          $tmpb=~ s/END/SELECT 1=$run 2=$fevent 43=$run 44=$levent \n END/;
             my @gbc=split "\/", $gbatch;

          $buf=~ s/gbatch-orbit.exe/$gbc[$#gbc].64 -$self->{IORP} -U$job  -M -D1 -G$aft -S$stalone/;
}
         my $script="$self->{CCA}.$job.$template";
         my $root=$self->{CCT} eq "remote"?"$self->{UploadsDir}/$script":"$self->{AMSDataDir}/$self->{LocalClientsDir}/$script";
         if($self->{q}->param("ProductionQuery") eq "Submit Request" and $self->{CCT} eq "local"   ){
# add one more check here using save state
            my $time = time();
            $sql="update Cites set state=0, timestamp=$time  where name='$self->{CCA}'";
            $self->{sqlserver}->Update($sql);
             my $save="$self->{UploadsDir}/$self->{CCA}.$run.$dataset->{version}.$self->{CEMID}save";
             my $param=$self->{q}->param("AdvancedQuery")?"AdvancedQuery":($self->{q}->param("ProductionQuery")?"ProductionQuery":"BasicQuery");
            $q->param($param,"Save");
            $q->param("FEM",$save);
            $q->param("Version",$dataset->{serverno}); 
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
          print "Password :  <input type=\"password\" name=\"password\" value=\"\"/>  ";
          print "<TR><BR>";
         }
         print $q->submit(-name=>$param, -value=>"Save");
         htmlFormEnd();
	 print htmlBottom();
	
         return 1;
        }
         my $adddst=$template; 
         $adddst=~ s/\.job//;
         open(FILE,">".$root) or die "Unable to open file $root\n";
         if($self->{CCT} eq "local"){
               my $srv=$dataset->{serverno};
        if($dataset->{serverno}=~/^v/){
           $srv=0;
        }
   if($#{$self->{arpref}} <0 ){
       $self->ServerConnect($dataset->{serverno});
   }

    if($#{$self->{arpref}} >=0){
        my $ard=${$self->{arsref}}[0];
        my %cid=%{$self->{cid}};
        $cid{Type}="Server";

try{
                my ($length,$arr)=$ard->getEnv(\%cid);
                foreach my $ent (@{$arr}){
                    if($ent=~/AMSDataDir=/){
                      $self->{AMSDataDir2}=$ent;  
                 }
                }
            }
     catch CORBA::SystemException with{
     };

           }
              print FILE "export AMSDataDir2=$self->{AMSDataDir} \n";
              print FILE "export $self->{AMSDataDir2} \n";
              print FILE "if [ ! -d \$AMSDataDir/$dataset->{version} ]; then \n";
              print FILE "export AMSDataDir=\$AMSDataDir2 \n";
              print FILE "fi \n";


          if(defined $self->{AMSDSTOutputDir} and $self->{AMSDSTOutputDir} ne ""){
 print FILE "export NtupleDestDir=$self->{AMSDSTOutputDir}/$adddst/$run \n";
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
         if($dataset->{serverno}=~/^v/){
          }
          else{
              print FILE "export AMSServerNo=$dataset->{serverno} \n";
             }
           }
#
# check here custom/generic
#
         if(defined $q->param("JST")){
             $self->{jst}=$q->param("JST");         
             my $sdir="$self->{AMSSoftwareDir}/scripts/";
             my $newfile=$sdir."$self->{CCA}";
             if(not open(FILEI,"<".$newfile)){
              if(   $q->param("JST") eq 'C'){
                 die  " Unable to find script file $newfile.  Please make sure you did send your custom script requirements to ams production team. ";
             }
          }
              else{
             my $sbuf;
             read(FILEI,$sbuf,16384);
             close FILEI;
             my @ssbuf = split ',;',$sbuf;
             if($#ssbuf ne 1){
              die " Could not parse script $newfile $#ssbuf";
             }

             $buf=~ s/export/$ssbuf[0]\nexport/;
             if($buf=~/walltime=/){
                     my $thr=1;
                      if(defined $q->param("TTHREADS")){
                        $thr=$q->param("TTHREADS");
                        if($thr>8){
                          $thr=8;
                         }
                        if($thr<1){
                          $thr=1;
                         }
                    }
                     my $cpuperevent=0.18;
                     my $totaltime=($levent-$fevent)*$cpuperevent/$thr;
                     $totaltime=int($totaltime/3600.+0.5)+1;
                     if($totaltime<2){
                         $totaltime=2;
                     }
                     
                   if($totaltime>24){
                         $totaltime=24;
                     }
       $buf=~s/walltime=/walltime=$totaltime:0:0/;
                 }

             $tmpb =~ s/\!/\!\n$ssbuf[1]/;
         }
         }
         $path='./';
         if($self->{CCT} eq "local"){
             $path="$self->{AMSDataDir}/$self->{LocalClientsDir}/";
         }
        $buf=~s:export:export AMSFSCRIPT=$path$script \nexport:;
         print FILE $buf;
         print FILE $tmpb;
         if($self->{CCT} eq "local"){
             print FILE 'rm  $TMP/gbatch-orbit.exe.$RUN'."\n";
         }
         close FILE;
         my $j=system("chmod +x  $root");
         if($self->{CCT} eq "remote"){
         $j=system("tar -C$self->{UploadsDir} -uf  $file2tar $script  1>/dev/null 2>&1");
          if($j){
              $self->ErrorPlus("Unable to tar $script to $file2tar ");
          }
          unlink $root;
         }
else{
         my @jnk=split '=', $self->{AMSDataDir2};
if($#jnk>0 and not $jnk[1] =~ /\/cvmfs\//){
         my $i=system("cp $root $jnk[1]/$self->{LocalClientsDir}");
}

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
         $nickname = $q->param("QNick");
         my $stalone  = "STANDALONE";
         if (defined  $q->param("STALONE") and $q->param("STALONE") eq "No") {
          my $stalone  = "CLIENT";
         }
         my $pid = $self->getProductionSetIdByDatasetId($did);
#check run alrdy exist
 $sql="select FEvent from DataRuns where jid=$job";
 $ret=$self->{sqlserver}->Query($sql);
 while(defined $ret->[0][0]){
 $job++;
 $sql="select FEvent from DataRuns where jid=$job";
 $ret=$self->{sqlserver}->Query($sql);
}
my $buf2="$buf$tmpb";
if(length($buf2)>=4000){
    $buf2=~s/G4INSTALL/G4I/g;
}
if(length($buf2)>=4000){
    $buf2=~s/export/et/g;
}
if(length($buf2)>4000){
    my $len=length($buf2)-4000;
    my $mes="CUTTED:$len"; 
    $len=length($buf2)-4000+length($mes);
    $mes="CUTTED:$len"; 
    $buf2 = substr $buf2, 0, 4000-length($mes);
    $buf2="$buf2$mes";
}
            $insertjobsql="INSERT INTO Jobs VALUES
                             ($job,
                              '$script',
                              $self->{CEMID},
                              $self->{CCID},
                              $did,
                              $ctime,
                              $evts,
                              $timeout,
                              '$buf2',
                              $ctime,
                              '$nickname',
                               'host',0,0,0,0,'$stalone',
                              -1, $pid,-1,0)";
#creat corresponding runevinfo
         $sql ="select fevent,levent,fetime,letime,nevents,type from datafiles where run=$run";
         my $r1=$self->{sqlserver}->Query($sql);
         my $ri={};
         $ri->{uid}=$job;
         $ri->{Run}=$run;
         $ri->{FirstEvent}=$r1->[0][0];
         $ri->{LastEvent}=$r1->[0][1];
         $ri->{TFEvent}=$r1->[0][2];
         $ri->{TLEvent}=$r1->[0][3];
if($ri->{TLEvent}==0){
    $ri->{TLEvent}=$ri->{TFEvent}+30e7;
}
     
         $ri->{Priority}=1;
         $ri->{FilePath}=$script;
         $ri->{rndm1}=0;
         $ri->{rndm2}=0;
         $ri->{DataMC}=1;
           if($r1->[0][5]=~/^MC/){
               $ri->{DataMC}=0;
           }
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
          if(defined $templateprio and $templateprio>=0){
            $ri->{Priority}=$templateprio;
          }
         if(defined $templatehost and $templatehost=~/ams/){
          $ri->{Priority}=3;
          $ri->{cinfo}->{HostName}=$templatehost;
        }
        else{
         $ri->{cinfo}->{HostName}=" ";
        }
         $ri->{cinfo}->{Run}=$ri->{Run};
         $ri->{cinfo}->{EventsProcessed}=0;
         $ri->{cinfo}->{LastEventProcessed}=0;
         $ri->{cinfo}->{ErrorsFound}=0;
         $ri->{cinfo}->{CriticalErrorsFound}=0;
         $ri->{cinfo}->{CPUTimeSpent}=0;
         $ri->{cinfo}->{CPUMipsTimeSpent}=0;
         $ri->{cinfo}->{Mips}=-1;
         $ri->{cinfo}->{TimeSpent}=0;
         $ri->{cinfo}->{Status}=$ri->{Status};
         push @{$self->{Runs}}, $ri;
         if($pid>0){          
           $self->{sqlserver}->Update($insertjobsql);
         }
         $self->insertDataRun(
                       $ri->{Run},
                       $job,
                       $ri->{FirstEvent},
                       $ri->{LastEvent},
                       $ri->{TFEvent},
                       $ri->{TLEvent},
                       $ri->{SubmitTime},
                       $ri->{Status});
        if(defined $self->{dbserver} ){
            if($self->{dwldaddon}==0 ){
    my $hostname=hostname();
    if($#{$self->{arpref}} <0 and (not ($self->{hostdbfile}=~/$hostname/))){
       $self->ServerConnect($dataset->{serverno});
    }
    if($#{$self->{arpref}} >=0){
        my $ard=${$self->{arpref}}[0];
          my %nc=%{$ri};
try{
                $ard->sendRunEvInfo(\%nc,"Create");
}
            catch CORBA::SystemException with{
                $self->ErrorPlus( "sendback corba exc while creating run $ri->{Run}");
            };
}
else{
              DBServer::sendRunEvInfo($self->{dbserver},$ri,"Create");
}
        }
        }
        else{
            $self->ErrorPlus("Unable To Communicate With Server");
        }
         $job=$job+1;
           my $time=time();
           $sql="update Cites set state=0, maxrun=$job, timestamp=$time where name='$self->{CCA}'";
            $self->{sqlserver}->Update($sql);

        }
       


#
# Add files to server
#
        if(defined $self->{dbserver} ){
            if($self->{dwldaddon}==0 ){
            foreach my $ri (@{$self->{Runs}}){
              #DBServer::sendRunEvInfo($self->{dbserver},$ri,"Create");
           }
            my $lu=time();
            my $sqll="update Servers set lastupdate=$lu where dbfilename='$self->{dbfile}' and datamc=$self->{DataMC}";
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
        if($address eq 'vitali@afl3u1.cern.ch'){
          $address='vitali.choutko@cern.ch';
        }
        my $frun=$job-$runno;
        my $lrun=$job-1;
        my $subject="AMS02 Data Request Form Output Runs for $address $frun...$lrun Cite $self->{CCA}";
                 my $message;
        my $data="readmestandalone_data";
          if(defined $dataset->{MC} and $dataset->{MC}==1){
           $data=$data."mc";
          }

                 if($self->{dwldaddon}==1){
                   $message=$self->{tsyntax}->{headers}->{$data};
                 }
                 else{
                   $message=$self->{tsyntax}->{headers}->{readmecorba};
                 }
        $sql = "SELECT dirpath FROM journals WHERE cid=$self->{CCID}";
        my $note='please contact vitali.choutko@cern.ch for details';
        $ret = $self->{sqlserver}->Query($sql);

        if (defined $ret->[0][0]) {
         $note ="DST directory path : $ret->[0][0]/nt \n Journal files directory path : $ret->[0][0]/jou \n";
         }
         $message=$message.$note;
                  my $attach;
       if ($self->{CCT} eq "remote"){
          if($self->{senddb}==1){
          $self->{TU1}=time();
           $attach="$file2tar.gz,$dataset->{version}rdscripts.tar.gz;$filedb,ams02rddb.tar.gz";
          }
          elsif($self->{sendaddon}==1){
              $self->{TU2}=time();
              $self->{sendaddon}=0;
             $attach= "$file2tar.gz,$dataset->{version}rdscripts.tar.gz;$filedb_att,ams02rddb.addon.tar.gz";
          }
          else{
              $attach= "$file2tar.gz,$dataset->{version}rdscripts.tar.gz";
          }
           
                  $self->sendmailmessage($address,$subject,$message,$attach);
#                  my $i=unlink "$file2tar.gz";
          my $dire="";
    my $sqle="SELECT myvalue FROM Environment WHERE mykey='AMSSCRIPTS'";
    my $rete = $self->{sqlserver}->Query($sqle);
    if (defined $rete->[0][0]) {
        $dire = $rete->[0][0];
    }
    else{
                $self->ErrorPlus( "unable to find AMSSCRIPTS dir ");
    }                       
          system("mkdir -p $dire/$self->{CCA}");
          my $cmde="cp $file2tar.gz $dire/$self->{CCA}/";
          my $ie=system($cmde);
          if($ie){
                $self->ErrorPlus( "unable to $cmde ");
          }
    if($self->{jst} eq 'C' and $self->{CCA} eq 'taiwan'){
    }
          else{
                  my $i=unlink "$file2tar.gz";
              }




                  if($self->{sendaddon}==1){
                   $self->{TU2}=time();
                   $attach="$filedb_att,ams02rddb.addon.tar.gz";
                   $subject="Addon To AMS02 MC Request Form Output Runs for $address $frun...$lrun Cite $self->{CCA}";
                   $self->sendmailmessage($address,$subject,$message,$attach);
               }
              }
         my $totalreq=$self->{CEMR}+$runno;
         my $time=time();
         $sql="Update Mails set requests=$totalreq, timeu1=$self->{TU1}, timeu2=$self->{TU2}, timestamp=$time where mid=$self->{CEMID}";
         $self->{sqlserver}->Update($sql);
         $subject="Data Request Form Output Runs for $address $frun...$lrun Cite $self->{CCA}";
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
           $sql="update Cites set state=0, maxrun=$job, timestamp=$time where name='$self->{CCA}'";
            $self->{sqlserver}->Update($sql);

        foreach my $cite (@{$self->{CiteT}}){
            if($self->{CCA} eq $cite->{name}){
              $cite->{maxrun}=$job;
              last;
          }
        }

                  my $dir=$self->{UploadsDir};
                  open DIR,$dir;
                  my @allfiles= readdir THISDIR;
                  closedir THISDIR;
                  foreach my $file (@allfiles){
                     my $pat='/$\.'."$self->{CEMID}.save/";
                     if($file =~$pat){
                       unlink $file;
                     }
                  }


#
#         if custom and taiwan - print all the job scripts as html output
# 


    if($self->{jst} eq 'C' and $self->{CCA} eq 'taiwan'){
         my $dir2tar="/tmp/tar.$self->{CCA}.".$$;
        system("rm -rf $dir2tar");
        system("mkdir  -p $dir2tar");
       
         my $cmd ="tar -zxvf $file2tar.gz -C$dir2tar 1>/dev/null 2>&1";
         my $i=system($cmd);
               if($i){
                  system("rm -rf $dir2tar");
                  unlink "$file2tar.gz";
                  $self->ErrorPlus("Unable to untar: $cmd ");
               }
               else{
                 opendir THISDIR, $dir2tar or die "unable to open $dir2tar";
                 my @jobs=readdir THISDIR;
                 closedir THISDIR;
                 print $q->header("text/html ");
                 $q->start_html();
                 my $min=20000000000;
                 my $max=0;
                 foreach my $job (@jobs){
                     if($job =~ /\.job$/){
                       if($job =~ /^\./){
                           next;
                       }
                       my @junk=split '\.',$job;
                           if($#junk>1){
                               if($min>$junk[1]){
                                  $min=$junk[1];
                                }
                               if($max<$junk[1]){
                                  $max=$junk[1];
                                }
                            }
                      }
                     }
                     my $jobsetid="$min$max";
                       print $q->h1("JOBSETID=$jobsetid");
                 foreach my $job (@jobs){
                     if($job =~ /\.job$/){
                       if($job =~ /^\./){
                           next;
                       }
                       my $full="$dir2tar/$job";
                       open(FILE,"<".$full) or die "Unable to open job file $full \n";
                       my $buf; 
                       read(FILE,$buf,1638400) or next;
                       close FILE;
                       print $q->h3("JOB=$job ");
                       print $q->textarea($job,$buf,20,100);
                       print $q->h3(" ");
                   }
                 }
                 system("rm -rf $dir2tar");
                 #unlink "$file2tar.gz";
                 $q->end_html;
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
          my $uplt0 = 0;                   #last upload
          my $uplt1 = 0;                   # for filedb and filedb.addon
          my $vdb   = 'XYZ';               # lastdb version loaded
          my $vvv   = "$dataset->{version}rddb"; # dataset version
          $sql="SELECT timeu1, timeu2, vdb  FROM Mails WHERE ADDRESS='$self->{CEM}'";
          my $retime=$self->{sqlserver}->Query($sql);
          if(defined $retime->[0][0]){
           $vdb = trimblanks($retime->[0][2]);
           $uplt0    = $retime->[0][0];
           $uplt1    = $retime->[0][1];
          }

       $self->DownloadTime();

        my $timeFileDB     = 0; # file xyzmcdb.tar.gz time
        my $timeFileAttDB  = 0; #      xyzmcdbaddon.tar.gz time
        my $i              = 0;

        foreach my $filedb (@{$self->{FileDB}}) {
          $self->{FileDBLastLoad} ->[$i] = 0;
          if ($filedb =~ m/$vvv/ ) {
           $timeFileDB=(stat($filedb))[9];
           $self->{FileDBLastLoad} ->[$i] = $uplt0;
           last;
         }
         $i++;
        }

        $i = 0;
        foreach my $filedb (@{$self->{FileAttDB}}) {
          $self->{FileAttDBLastLoad} ->[$i] = 0;
          if ($filedb =~ m/$vvv/) {
           $timeFileAttDB=(stat($filedb))[9];
           $self->{FileAttDBLastLoad} ->[$i] = $uplt1;
           last;
         }
          $i++;
      }

       if ($vvv =~ $vdb ) {
        if ($uplt0 == 0 || $uplt1 == 0) {
            $self->Download($vvv, $vdb);
         } elsif ($timeFileDB == 0 || $timeFileAttDB == 0) {
            $self->Download($vvv, $vdb);
         } elsif ($uplt0 < $timeFileDB || $uplt1 < $timeFileAttDB) {
            $self->Download($vvv, $vdb);
         } else {
          $self->{FinalMessage}=" Your request was successfully sent to $self->{CEM}";

         }
       } else {
         $self->Download($vvv, $vdb);
        }
     } else {
      $self->{FinalMessage}=" Your request was successfully sent to $self->{CEM}";

     }






}
       else{
anyagain:
       if(defined $dataset){
         foreach my $tmp (@{$dataset->{jobs}}) {
            if($template eq $tmp->{filename} and $tmp->{TOTALEVENTS}>0){
                $templatebuffer=$tmp->{filebody};
                my ($rid,$rndm1,$rndm2) = $self->getrndm($dataset);
                if(defined $tmp->{THREADS}){
                    $q->param("TTHREADS", $tmp->{THREADS});
                }
                else {
                    $q->param("TTHREADS", 1);
                }
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
                $q->param("QSINGLEJOB",$dataset->{singlejob});
                if(not defined $q->param("QCPUPEREVENT")){
                    $q->param("QCPUPEREVENT",$tmp->{CPUPEREVENTPERGHZ});
                }
                my $evno=$q->param("QEv");
                if(not defined($evno) or not $evno =~/^\d+$/ or $evno <1 or $evno>$tmp->{TOTALEVENTS} ){
                    my $corr=1.00;
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
                      
                        if( $q->param("QSINGLEJOB")){
                            my $runno=$q->param("QRun");
                        }
                        else{
                        my $runno=int($evno/$evperrun+0.5);
                         
                        if($runno le 0){
                            $runno=1;
                        }
                       $q->param("QRun",$runno);
                    }
                    }
                    if($evno<10000){
                      $evno=10000;
                    }
                    $q->param("QEv",$evno);
                    $tmp->{TOTALEVENTS}-=$evno;
#                    warn "template $tmp->{TOTALEVENTS} runsave:$runsave  \n";                   
                }
                last;
            }
        }
     }
        else{
         foreach my $tmp (@{$self->{TempT}}){
              $tmps=$tmps." ".$tmp->{filename};
            if($template eq $tmp->{filename}){
                $templatebuffer=$tmp->{filebody};
                last;
            }
         }
     }
        if(not defined $templatebuffer){
            $self->ErrorPlus("Could not find file for $template template. $tmps");
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
        if($cput < 600 ){
             $self->ErrorPlus("Computer Clock $cput  is too low for the AMS02 MC (min 600)");
        }
         my $cputime=$q->param("QCPUTime");
        if(not defined $cputime){
           $cputime=300000
        }
        if($cputime < 86400  and $self->{CCA} ne 'test'){
             $self->ErrorPlus("CPU Time Limit Per Job $cputime is too low for the AMS02 MC (min 86400 sec) ");
        }

        my $clock=$cput;
        if($cput > 4000 ){
            $cput=4000;
        }

        my $corr=1.00;
            if (defined $q->param("QCPUType")){
              $corr=$self->{cputypes}->{$q->param("QCPUType")};
            }
        if($cputime*$cput*$corr < 100000000  and $self->{CCA} ne 'test' and $q->param("QSINGLEJOB")==0){
             $self->ErrorPlus("CPU Time Limit Per Job $cputime*$cput*$corr/1000 is too low for the AMS02 MC (min 100000 secGHz) ");
        }

        $cput=50+$pmax*1000/$cput/$corr;
        if($cput >7200){
         $cput=7200;
        }
        if ($dataset->{buildno} > 906) {
            $cput = 30;
        }

        my $evno=$q->param("QEv");
        my $runno=$q->param("QRun");
        if(not $evno =~/^\d+$/ or $evno <$a ){
             $self->ErrorPlus("Events no $evno is out of range ($a,$b)");
        }
        #die "$evno $runno $max_jobs";
        if($max_jobs<$runno){
            $runno=$max_jobs;
        }
        if(not $runno =~/^\d+$/ or $runno <$a or $runno>1000){
             $self->ErrorPlus("Runs no $runno is out of range ($a,1000)");
        }

        if($evno < $runno){
             $self->ErrorPlus("Runs no $runno greater than events no $evno");
         }
        my $evperrun=int ($evno/$runno);
        if($evperrun > (1<<31)-1){
            $self->ErrorPlus('EventsPerRun Exceeds 2^31-1 :'."$evperrun $runno");
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
        if($year < 96 or $year > 118 or $month<0 or $month>11 or $date<1 or $date>31){
             $self->ErrorPlus("TimeBegin $timbeg is out of range $date $month $year");
        }
         $timbegu=timelocal(1,0,0,$date,$month,$year);
        if(not $timend =~/^\d+$/ ){
             $self->ErrorPlus("TimeEnd $timend is not an integer");
        }
        $year=$timend%10000-1900;
        $month=int($timend/10000)%100-1;
        $date=int($timend/1000000)%100;
        if($year < 96 or $year > 128 or $month<0 or $month>11 or $date<1 or $date>31){
             $self->ErrorPlus("TimeEnd $timend is out of range $date $month $year");
        }
         $timendu=timelocal(1,0,0,$date,$month,$year);
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
if(defined $dataset->{buildno} ){
    if($self->{Build}<0){
    $self->getProductionVersion;
    }
    my $tago="170=$self->{Build}";
    my $tagn="170=$dataset->{buildno}";
    $rootntuple=~s/$tago/$tagn/;
}
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
       my $run=$runsave;
    if(not defined $run){
        my $sql="select maxrun from Cites where name='$self->{CCA}' and state=0";
        my $res=$self->{sqlserver}->Query($sql);
        if( not defined $res->[0][0]){
            my $mes="Cite $self->{CCA} does not exist or locked. Please retry from very beginning";
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
            $sql="select max(jid) from jobs where jobname like '$self->{CCA}.%'";
            my $resj=$self->{sqlserver}->Query($sql);
            if(defined $resj->[0][0] and $resj->[0][0]>= $res->[0][0]){
                my $mes="Cite $self->{CCA} maxrun/jid mismatch $resj->[0][0]  $res->[0][0] ";
                $sql="update Cites set maxrun=$resj->[0][0]+1 where name='$self->{CCA}'";
                $self->{sqlserver}->Update($sql);
                $self->ErrorPlus($mes);
            }
            my $time = time();
            $sql="update Cites set state=1, timestamp=$time where name='$self->{CCA}'";
            $self->{sqlserver}->Update($sql);
            $run=$res->[0][0];
        }
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

        if (not $self->ServerConnect($dataset->{serverno})){
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
    if(not defined $dataset){
        my $setup=$q->param("QSetup");
        if(defined $setup){
        if($setup =~/AMS02/){
           if($setup =~/v4.00/){
            $dataset->{version}='v4.00';
           }
           else{
            $dataset->{version}='v5.00';
           }
        }
        elsif($setup =~/AMSSHUTTLE/){
            $dataset->{version}='v3.00';
        }
    }
        else{
         my $setup=$q->param("QTemp");
         if($setup =~/v4.00/){
            $dataset->{version}='v4.00';
         }
         elsif($setup =~/v5.00/){
            $dataset->{version}='v5.00';
        }
        elsif($setup =~/mc01/){
            $dataset->{version}='v3.00';
        }
        }
    }

        my $gbatch="";
        my $gbatchcomp="";
        if($dataset->{g4}=~/g4/){
           $gbatchcomp=$ret->[0][0].".$dataset->{version}".".$dataset->{datamc}"."$dataset->{build}";
           $gbatch=$gbatchcomp.".$dataset->{g4}";
        }
        else{
           $gbatch=$ret->[0][0].".$dataset->{version}".".$dataset->{datamc}"."$dataset->{build}";
           $gbatchcomp=$gbatch.".g4";
        }
        my @stag=stat "$self->{AMSSoftwareDir}/$gbatch";
        if($#stag<0){
              $self->ErrorPlus("Unable to find $self->{AMSSoftwareDir}/$gbatch on the Server ");
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
        $key='ami2root';
        $sql="select myvalue from Environment where mykey='".$key."'";
        $ret=$self->{sqlserver}->Query($sql);
        if( not defined $ret->[0][0]){
            $self->ErrorPlus("unable to retreive $key name from db");
        }
         my $am=$ret->[0][0];
         @stag2=stat "$self->{AMSSoftwareDir}/$am";
        if($#stag2<0){
              $self->ErrorPlus("Unable to find $am on the Server ");
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
        my $vdb ='XYZ';
        my $stt ='UNK';

        if($self->{CCT} eq "remote"){
#+
         $self->  getProductionPeriods(0);
         foreach my $mc (@productionPeriods) {
          if ($mc->{status} =~ 'Active') {
           my $vdb = $mc->{vdb};
           if ($vdb =~ $dataset->{version}) {
            foreach my $file (@{$self->{FileDB}}) {
             if ( $file =~ m/$vdb/ and $file =~/mcdb/) {
                $filedb = "$self->{UploadsDir}/$file";
                last;
             }
            }
           }
          }
         }
          if (not defined $filedb) {
            $self->ErrorPlus("unable to find corrsponding database file ($dataset->{name},$dataset->{version}, $vdb, $stt)");
         }
#-
#        $filedb=~s/ams02/$dataset->{version}/;
        my @sta = stat $filedb;
        if($#sta<0 or $sta[9]-time() >86400*7 or $stag[9] > $sta[9] or $stag1[9] > $sta[9] or $stag2[9] > $sta[9]){
           $self->{senddb}=2;
        my $filen="$self->{UploadsDir}/ams02mcdb.tar.$run";
#        $key='dbversion';
#        $sql="select myvalue from Environment where mykey='".$key."'";
#        my $ret=$self->{sqlserver}->Query($sql);
#        if( not defined $ret->[0][0]){
#            $self->ErrorPlus("unable to retreive db version from db");
#        }
#
#        my $dbversion=$ret->[0][0];
        my $dbversion=$dataset->{version};
        my $i=system "mkdir -p $self->{UploadsDir}/$dbversion";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/*.dat $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/\*.dat";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/*.bin $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/\*.bin";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/*.txt $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/\*.txt";
      $i=system "ln -s $self->{AMSDataDir}/$dbversion/*.dat $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/\*.dat";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/t* $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/t\*";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/L* $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/L\*";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/A* $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/A\*";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/T* $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/T\*";
       if($dbversion=~/v4/ or $dbversion=~/v5/){
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/ri* $self->{UploadsDir}/$dbversion";
           unlink "$self->{AMSDataDir}/$dbversion/ri\*";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/*.flux $self->{UploadsDir}/$dbversion";
        unlink "$self->{AMSDataDir}/$dbversion/\*.flux";
       }
        $i=system "tar -C$self->{UploadsDir} -h -cf $filen --exclude='v5.01/RichDefaultPMTCalib/*' --exclude='v5.01/ACsoft/*' --exclude='v5.01/TRD/*' $dbversion 1>/dev/null 2>&1";
        if($i){
              $self->ErrorPlus("Unable to tar $self->{UploadsDir} $dbversion to $filen  $i    tar -C$self->{UploadsDir} -h -cf $filen $dbversion 1>/dev/null 2>&1");
         }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $gbatch  1>/dev/null 2>&1") ;
          if($i){
              $self->ErrorPlus("Unable to tar $gbatch to $filen ");
          }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $gbatchcomp  1>/dev/null 2>&1") ;
          if($i){
              $self->ErrorPlus("Unable to tar $gbatchcomp to $filen");
          }
         $i=system("tar -C$self->{CERN_ROOT} -uf $filen lib/flukaaf.dat  1>/dev/null 2>&1") ;
          if($i){
              $self->ErrorPlus("Unable to tar flukaaf.dat to $filen ");
          }

         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $nv  1>/dev/null 2>&1") ;
          if($i){
              $self->ErrorPlus("Unable to tar $nv to $filen  ");
          }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $gr  1>/dev/null 2>&1") ;
          if($i){
              $self->ErrorPlus("Unable to tar $gr to $filen ");
          }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $am  1>/dev/null 2>&1") ;
          if($i){
              $self->ErrorPlus("Unable to tar $am to $filen ");
          }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen prod/tnsnames.ora  1>/dev/null 2>&1") ;
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
#
         foreach my $mc (@productionPeriods) {
          if ($mc->{status} =~ 'Active') {
           my $vdb = $mc->{vdb};
           if ($vdb =~ $dataset->{version}) {
            foreach my $file (@{$self->{FileAttDB}}) {
             if ( $file =~ m/$vdb/) {
                $filedb_att = "$self->{UploadsDir}/$file";
                last;
             }
            }
           }
          }
         }

#         die $filedb, $filedb_att;
#        $filedb_att=~s/ams02/$dataset->{version}/;
        @sta = stat $filedb_att;

#         my @stag3=stat "$self->{AMSDataDir}/DataBase";
#        if($#stag3<0){
#              $self->ErrorPlus("Unable to find $self->{AMSDataDir}/DataBase on the Server ");
#        }
#        if(($#sta<0 or time()-$sta[9] >86400*180  or $stag3[9] > $sta[9] ) and $self->{dwldaddon}==1){

         my $dte=stat_adv_mc ("$self->{AMSDataDir}/DataBase",0,0,2,$sta[9]);
#         die "$sta[9] $dte $filedb_att \n";
        if(($#sta<0 or time()-$sta[9] >86400*180  or $dte > $sta[9] ) and $self->{dwldaddon}==1){

           $self->{sendaddon}=2;
        my $filen="$self->{UploadsDir}/ams02mcdb.addon.tar.$run";
        my $i=system "mkdir -p $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/MagneticFieldMap* $self->{UploadsDir}/DataBase";
        $key='dbversion';
        $sql="select myvalue from Environment where mykey='".$key."'";
        my $ret=$self->{sqlserver}->Query($sql);
        if( not defined $ret->[0][0]){
            $self->ErrorPlus("unable to retreive db version from db");
        }
#
#        my $dbversion=$ret->[0][0];
         my $dbversion=$dataset->{version};
          my $rtn=0;
           if($dbversion =~/v4/ or $dbversion=~/v5/){
           closedir THISDIR;
           my $suc=opendir THISDIR,"$self->{AMSDataDir}/DataBase";
           if(!$suc){
              $self->ErrorPlus("Unable to opendir $self->{AMSDataDir}/DataBase");            }
     my @allfiles=readdir THISDIR;
     closedir THISDIR;
       foreach my $file (@allfiles){
         my $newfile="$self->{AMSDataDir}/DataBase/".$file;
         if(readlink $newfile){
#           next;
         }
         my @sta=stat $newfile;
         if($sta[2]>32000){
          if($newfile =~/\.0$/){
           $i=system "ln -s $newfile $self->{UploadsDir}/DataBase";
         }
         }
         else{
    if ($file =~/^\./){
       next;
    }
        $i=system " rm -rf $self->{UploadsDir}/DataBase/$file";
        $i=system " mkdir -p $self->{UploadsDir}/DataBase/$file";
           $suc=opendir THISDIR,"$newfile";
           if(!$suc){
              $self->ErrorPlus("Unable to opendir $self->{AMSDataDir}/DataBase/$file");            }
         my @allnew=readdir THISDIR;
         closedir THISDIR;
         foreach my $new (@allnew){
    if ($new =~/^\./){
       next;
    }
          my $newfile1="$self->{AMSDataDir}/DataBase/$file/$new";
          if(readlink $newfile1){
            next;
          }
          my @sta=stat $newfile1;
          if($sta[2]<32000){
            $i=system " rm -rf $self->{UploadsDir}/DataBase/$file/$new";
           $i=system " mkdir -p $self->{UploadsDir}/DataBase/$file/$new";
           $suc=opendir THISDIR,"$self->{AMSDataDir}/DataBase/$file/$new";
           if(!$suc){
              $self->ErrorPlus("Unable to opendir $self->{AMSDataDir}/DataBase/$file/$new");            }
         my @allvar=readdir THISDIR;
         closedir THISDIR;
         foreach my $var (@allvar){
          if($var=~/\.0\./){
           $i=system "ln -s $self->{AMSDataDir}/DataBase/$file/$new/$var $self->{UploadsDir}/DataBase/$file/$new/$var";
          }
         }

         }
        }
         }
       }
        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerCmnNoise";
        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerSigmas.r";
        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerSigmas.l";
        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerPedestals.r";
        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerPedestals.l";
        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerStatus.r";
        $i=system "rm -rf $self->{UploadsDir}/DataBase/TrackerStatus.l";

        $rtn=system "tar -C$self->{UploadsDir} -h -czf $filen.gz DataBase  1>/dev/null 2>&1";
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
        $rtn=system "tar -C$self->{UploadsDir} -h -czf $filen.gz DataBase  1>/dev/null 2>&1";
           }
        if($rtn){
        #      $self->ErrorPlus("Unable to tar $self->{UploadsDir} DataBase to $filen.gz $rtn");
        }
#          $i=system("gzip -f $filen");
#                      if($i){
#              $self->ErrorPlus("Unable to gzip  $filen");
#          }
          $i=system("mv $filedb_att $filedb_att.o");
          $i=system("mv $filen.gz $filedb_att");
          unlink "$filedb_att.o";
       }
        elsif($sta[9]>$self->{TU2}){
            $self->{sendaddon}=2;
        }


# write readme file
        my $readme="$self->{UploadsDir}/README.$run";
        open(FILE,">".$readme) or die "Unable to open file $readme (specify the exact path and/or check dir protection)\n";
        if($self->{dwldaddon}==1){
         $self->{tsyntax}->{headers}->{readmestandalone}=~s/ams02/$dataset->{version}/g;
         print FILE  $self->{tsyntax}->{headers}->{readmestandalone};
        }
        else{
         $self->{tsyntax}->{headers}->{readmecorba}=~s/ams02/$dataset->{version}/g;
         print FILE  $self->{tsyntax}->{headers}->{readmecorba};
        }
        my $sql = "SELECT dirpath FROM journals WHERE cid=$self->{CCID}";
        my $note='please contact vitali.choutko@cern.ch for details';
        my $ret = $self->{sqlserver}->Query($sql);

        if (defined $ret->[0][0]) {
         $note ="DST directory path : $ret->[0][0]/nt
                \n Journal files directory path : $ret->[0][0]/jou
                \n Validation log files directory path : $ret->[0][0]/log
                \n";
         }
        print FILE  $note;
        close FILE;
         my $i;
         if($Any<0 or $srunno eq $qrunno){
         $file2tar="$self->{UploadsDir}/$dataset->{version}script.$run.tar";
         $self->{file2tar}=$file2tar;
           $i=system("tar -C$self->{UploadsDir} -cf  $file2tar README.$run  1>/dev/null 2>&1");
     }
         else{
           $i=system("tar -C$self->{UploadsDir} -uf  $file2tar README.$run  1>/dev/null 2>&1");
         }
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
         my $sql = "SELECT dirpath FROM journals WHERE cid=$self->{CCID}";
         my $ret=$self->{sqlserver}->Query($sql);
         if(defined $ret->[0][0] && $self->{CCT} ne 'remote'){
             my $subs=$ret->[0][0]."/jou";
           #  $subs=~s/\//\\\//g;
           $buf=~s/ProductionLogDir/ProductionLogDir=$subs/;
             #die " $subs $buf \n";
          }
           if($timendu-$timbegu>86400*10000){
               $timbegu=time();
               $timendu=$timbegu+3600*$runno;
           }
#         my $runnum=$srunno>$runno?$srunno:$runno;
           
         my $origjobspan = int(($timendu-$timbegu)/$runno);
         my $jobspan = $origjobspan;
#         if ($jobspan > 86400*30) {
#             $jobspan = 86400*30;
#         }

         my $start=$timbegu+$jobspan*($i-1);
             my $end=$start+$jobspan;
             my ($s,$m,$h,$date,$mon,$year)=localtime($start);
              my $hstart=$s+100*$m+10000*$h;
         my $timstart=($year+1900)+10000*($mon+1)+1000000*($date);
         ($s,$m,$h,$date,$mon,$year)=localtime($end);
              my $hfin=$s+100*$m+10000*$h;
         my $timfin=($year+1900)+10000*($mon+1)+1000000*($date);
         $buf=~ s/TIMBEG=/TIMBEG=$timstart\nHRSBEG=$hstart/;
         $buf=~ s/TIMEND=/TIMEND=$timfin\nHRSEND=$hfin/;
         $buf=~ s/PART=/PART=$particleid/;
         $buf=~ s/RUN=/RUN=$run/;
#         die "$timstart $runnon $timfin $start $end $end-$start \n $buf";
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
         if (defined $rootntuple && $i<2 && ($Any<0 or $srunno eq $qrunno) && not $self->{q}->param("AdvancedQuery")) {
if(defined $dataset->{buildno} ){
    if($self->{Build}<0){
    $self->getProductionVersion;
    }
    my $tago="170=$self->{Build}";
    my $tagn="170=$dataset->{buildno}";
    $rootntuple=~s/$tago/$tagn/;
}
          $buf=~ s/ROOTNTUPLE=/ROOTNTUPLE=\'$rootntuple\'/;
         }
         if($i > 1){
            my $rid=1;
            ($rid,$rndm1,$rndm2) = $self->getrndm($dataset);
         }
          $buf=~ s/RNDM1=/RNDM1=$rndm1/;
          $buf=~ s/RNDM2=/RNDM2=$rndm2/;
          $buf=~ s/TRIG=/TRIG=$evts/;
          my $r1=rand;
          my $r2=rand;
          my $pminr=$pmin*(1-0.005*$r1);
          my $pmaxr=$pmax*(1+0.005*$r2);
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
           $tmpb=~ s/TIME 3=/TIME 1=$cpusf 2=100. 3=/;
          }
         }
         my $nickname=$q->param("QNick");
         $buf=~ s/PART=/NICKNAME=$nickname \nPART=/;
         $buf=~ s/PART=/DATASETNAME=$dataset->{name} \nPART=/;
         if(defined $q->param("TFILENAME")){
         my @junk=split '\.',$q->param("TFILENAME");
         $buf=~ s/JOB=/JOBNAME=$junk[0] \nJOB=/;
         }
         if(defined $q->param("TTHREADS")){
            my $threads=$q->param("TTHREADS");
            $buf=~ s/JOB=/THREADS=$threads \nJOB=/;
         }
         $buf=~ s/PART=/CPUTIME=$cpus \nPART=/;
         $rootntuple=$q->param("RootNtuple");
         if($tmpb =~/DAQC 1=10/){
          $rootntuple=~ s/168=40/168=4000/;
         }
#         die  " $rootntuple $tmpb"
if(defined $dataset->{buildno} ){
    if($self->{Build}<0){
    $self->getProductionVersion;
    }
    my $tago="170=$self->{Build}";
    my $tagn="170=$dataset->{buildno}";
    $rootntuple=~s/$tago/$tagn/;
}
         $buf=~ s/ROOTNTUPLE=/ROOTNTUPLE=\'$rootntuple\'/;
         $tmpb=~ s/ROOTNTUPLE=/C ROOTNTUPLE/g;
         $tmpb=~ s/IOPA \$ROOTNTUPLE\'/IOPA \$ROOTNTUPLE\'\n/;
         #$tmpb=~ s/TERM/TGL1 1=8 \nTERM/;
         
         my $cputype=$q->param("QCPUType");
         $buf=~ s/PART=/CPUTYPE=\"$cputype\" \nPART=/;
         $buf=~ s/PART=/CLOCK=$clock \nPART=/;
         my $ctime=time();
         $buf=~ s/PART=/SUBMITTIME=$ctime\nPART=/;
         if($self->{CCT} eq "local"){
           $buf=~ s/\$AMSProducerExec/$self->{AMSSoftwareDir2}\/$gbatch/g;
         }
         else{
             my @gbc=split "\/", $gbatch;

          $buf=~ s/gbatch-orbit.exe/$gbc[$#gbc] -$self->{IORP} -U$run -M -D1 -G$aft -S$stalone/;



}
         my $script="$self->{CCA}.$run.$template";
         my $root=$self->{CCT} eq "remote"?"$self->{UploadsDir}/$script":
         "$self->{AMSDataDir}/$self->{LocalClientsDir}/$script";
         if($self->{q}->param("ProductionQuery") eq "Submit Request" and $self->{CCT} eq "local"   and ($Any<0 or $srunno eq $qrunno)){
# add one more check here using save state
            my $time = time();
            $sql="update Cites set state=0, timestamp=$time  where name='$self->{CCA}'";
            $self->{sqlserver}->Update($sql);
             my $save="$self->{UploadsDir}/$self->{CCA}.$run.$self->{CEMID}save";
             my $param=$self->{q}->param("AdvancedQuery")?"AdvancedQuery":($self->{q}->param("ProductionQuery")?"ProductionQuery":"BasicQuery");
            $q->param($param,"Save");
            $q->param("Version",$dataset->{serverno}); 
            $q->param("FEM",$save);
            if($Any>=0){
             $q->param("QRun",$srunno); 
            }
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
         my $adddst=$template; 
         $adddst=~ s/\.job//;
         open(FILE,">".$root) or die "Unable to open file $root\n";
         if($self->{CCT} eq "local"){
               my $srv=$dataset->{serverno};
        if($dataset->{serverno}=~/^v/){
           $srv=0;
        }
   if($#{$self->{arpref}} <0 ){
       $self->ServerConnect($dataset->{serverno});
    }

    if($#{$self->{arpref}} >=0){
        my $ard=${$self->{arsref}}[0];
        my %cid=%{$self->{cid}};
        $cid{Type}="Server";

try{
                my ($length,$arr)=$ard->getEnv(\%cid);
                foreach my $ent (@{$arr}){
                    if($ent=~/AMSDataDir/){
                      $self->{AMSDataDir2}=$ent;
                 }
                }
}
     catch CORBA::SystemException with{
     };
}   
   
              print FILE "export AMSDataDir2=$self->{AMSDataDir} \n";
              print FILE "export $self->{AMSDataDir2} \n";
              print FILE "if [ ! -d \$AMSDataDir/$dataset->{version} ]; then \n";
              print FILE "export AMSDataDir=\$AMSDataDir2 \n";
              print FILE "fi \n";


          if(defined $self->{AMSDSTOutputDir} and $self->{AMSDSTOutputDir} ne ""){
 print FILE "export NtupleDestDir=$self->{AMSDSTOutputDir}/$adddst/$run \n";
 print FILE "export NtupleDir=/dat0/local/logs/nt \n";
 print FILE "export NtupleDir2=/data/local/logs/nt \n";
 print FILE "export NtupleDir3=/afs/cern.ch/ams/Offline/local/logs/nt \n";
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
            if($dataset->{serverno}=~/^v/){
          }
          else{
              print FILE "export AMSServerNo=$dataset->{serverno} \n";
          }        
          #print FILE "export AMSDataDir=$self->{AMSDataDir} \n";
      }
#
# check here custom/generic
#
         if(defined $q->param("JST")){
             $self->{jst}=$q->param("JST");         
             my $sdir="$self->{AMSSoftwareDir}/scripts/";
             my $newfile=$sdir."$self->{CCA}";
             if(not open(FILEI,"<".$newfile)){
              if(   $q->param("JST") eq 'C'){
                 die  " Unable to find script file $newfile.  Please make sure you did send your custom script requirements to ams production team. ";
             }
          }
              else{
             my $sbuf;
             read(FILEI,$sbuf,16384);
             close FILEI;
             my @ssbuf = split ',;',$sbuf;
             if($#ssbuf ne 1){
              die " Could not parse script $newfile $#ssbuf";
             }

             $buf=~ s/export/$ssbuf[0]\nexport/;
             if($buf=~/walltime=/){
                     my $totaltime=int($cpus/3600)+1;
                     if($totaltime<2){
                         $totaltime=2;
                     }
                         $buf=~s/walltime=/walltime=$totaltime\:0\:0/;
}
             $tmpb =~ s/\!/\!\n$ssbuf[1]/;
         }
}

         my $path='./';
         if($self->{CCT} eq "local"){
             $path="$self->{AMSDataDir}/$self->{LocalClientsDir}/";
         }
         $buf=~s:export:export AMSFSCRIPT=$path$script \nexport:;
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
         $tmpb=~ s/126=50000/126=199999/;
#       add common template here
{
        my $dir="$self->{AMSSoftwareDir}/Templates";
        my $full=$dir."/common.job";
        open(FILE1,"<".$full) or die "Unable to open file $full \n";
        my $bufb;
        read(FILE1,$bufb,1638400) or next;
        close FILE1;
#       $tmpb=~ s/END\n!/$bufb\nEND\n!/;
#       Move content in common.job and commong4.job the beginning of
#       datacards, instead of the end, to avoid overwriting datacards
#       in job template.
       $tmpb=~ s/LIST\n/LIST\n$bufb\n/;
}
        if($dataset->{g4}=~/g4/){
        my $dir="$self->{AMSSoftwareDir}/Templates";
        my $full=$dir."/commong4.job";
        open(FILE1,"<".$full) or die "Unable to open file $full \n";
        my $bufb;
        read(FILE1,$bufb,1638400) or next;
        close FILE1;
#      $tmpb=~ s/END\n!/$bufb\nEND\n!/;
       $tmpb=~ s/LIST\n/LIST\n$bufb\n/;

    }
         print FILE $tmpb;
         if($self->{CCT} eq "local"){
             print FILE 'rm  $TMP/gbatch-orbit.exe.$RUN'."\n";
         }
         close FILE;
         my $j=system("chmod +x  $root");
         if($self->{CCT} eq "remote"){
         $j=system("tar -C$self->{UploadsDir} -uf  $file2tar $script  1>/dev/null 2>&1");
          if($j){
              $self->ErrorPlus("Unable to tar $script to $file2tar ");
          }
          unlink $root;
     }
else{
         my @jnk=split '=', $self->{AMSDataDir2};
if($#jnk>0 and not $jnk[1] =~ /\/cvmfs\//){
         system("cp $root $jnk[1]/$self->{LocalClientsDir}");
}

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
         $nickname = $q->param("QNick");
         my $stalone  = "STANDALONE";
         if ($q->param("STALONE") eq "No") {
          my $stalone  = "CLIENT";
         }
         my $pid = $self->getProductionSetIdByDatasetId($did);
         if($pid <=0){
           die " Unable to find production period  for  $did  $pid \n";
         }
#check run alrdy exist
 $sql="select FEvent from Runs where jid=$run";
 $ret=$self->{sqlserver}->Query($sql);
 while(defined $ret->[0][0]){
 $run++;
 $sql="select FEvent from Runs where jid=$run";
 $ret=$self->{sqlserver}->Query($sql);
}
my $buf2="$buf$tmpb";
if(length($buf2)>=4000){
    $buf2=~s/G4INSTALL/G4I/g;
}
if(length($buf2)>=4000){
    $buf2=~s/export/et/g;
}
if(length($buf2)>4000){
    my $len=length($buf2)-4000;
    my $mes="CUTTED:$len"; 
    $len=length($buf2)-4000+length($mes);
    $mes="CUTTED:$len"; 
    $buf2 = substr $buf2, 0, 4000-length($mes);
    $buf2="$buf2$mes";
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
                              '$buf2',
                              $ctime,
                              '$nickname',
                               'host',0,0,0,0,'$stalone',
                              -1, $pid,-1,0)";
         if($pid>0){          
           $self->{sqlserver}->Update($insertjobsql);
         }
#         $self->{sqlserver}->Update($sql);
#
#creat corresponding runevinfo
         my $ri={};
         $ri->{uid}=$run;
         $ri->{Run}=$run;
         $ri->{FirstEvent}=1;
         $ri->{LastEvent}=$evts;
         $ri->{TFEvent}=$start;
         $ri->{TLEvent}=$end+1;
         $ri->{Priority}=0;
         $ri->{FilePath}=$script;
         $ri->{rndm1}=0;
         $ri->{rndm2}=0;
         $ri->{DataMC}=0;
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
         $ri->{cinfo}->{CPUMipsTimeSpent}=0;
         $ri->{cinfo}->{Mips}=-1;
         $ri->{cinfo}->{TimeSpent}=0;
         $ri->{cinfo}->{Status}=$ri->{Status};
         $ri->{cinfo}->{HostName}=" ";
         push @{$self->{Runs}}, $ri;
         $self->insertRun(
                       $ri->{Run},
                       $run,
                       $ri->{FirstEvent},
                       $ri->{LastEvent},
                       $ri->{TFEvent},
                       $ri->{TLEvent},
                       $ri->{SubmitTime},
                       $ri->{Status});
         $run=$run+1;
    if($Any>=0 and defined $dataset){
#         last;
     }
        }
       
       if($Any>=0 and defined $dataset){
         my $evtl=0;
         foreach my $tmp (@{$dataset->{jobs}}) {
             if($tmp->{TOTALEVENTS}>0){
              $evtl+=$tmp->{TOTALEVENTS};
          }
         }
       if($evtl>0){
anynext:
         $Any+=1;
         if($#{$dataset->{jobs}}<=0){
           $Any=0;
         }
         else{
           $Any=$Any%($#{$dataset->{jobs}}+1);
         }
         if(${ $dataset->{jobs}}[$Any]->{TOTALEVENTS}<=0){
           goto anynext;
         }
         if($qrunno>1){
           $template= ${ $dataset->{jobs}}[$Any]->{filename};
           $qrunno--;
                    $q->delete("QRNDM1");
                    $q->delete("QRNDM2");
                    $q->delete("QTimeB");
                    $q->delete("QTimeE");
                    $q->delete("QMomI");
                    $q->delete("QMomA");
                    $q->delete("QPart");
                    $q->delete("QCPUPEREVENT");
                    $q->delete("QEv");
                    $runsave=$run;
            goto anyagain;
         }
       }
     }
        if($Any>=0){
         $runno=$srunno-$qrunno+1;
        }


#
# Add files to server
#
        if(defined $self->{dbserver} ){
            if($self->{dwldaddon}==0 ){
    my $hostname=hostname();  
    
    if($#{$self->{arpref}} <0 and (not ($self->{hostdbfile}=~/$hostname/))){
       $self->ServerConnect($dataset->{serverno});
    }

            foreach my $ri (@{$self->{Runs}}){
    if($#{$self->{arpref}} >=0){
        my $ard=${$self->{arpref}}[0];
          my %nc=%{$ri};
try{
                $ard->sendRunEvInfo(\%nc,"Create");
}
            catch CORBA::SystemException with{
                $self->ErrorPlus( "sendback corba exc while creating run $ri->{Run}");
            };
    }
else{
              DBServer::sendRunEvInfo($self->{dbserver},$ri,"Create");
}
           }
            my $lu=time();
            my $sqll="update Servers set lastupdate=$lu where dbfilename='$self->{dbfile}' and datamc=$self->{DataMC}";
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
        if($address eq 'vitali@afl3u1.cern.ch'){
          $address='vitali.choutko@cern.ch';
        }
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
        my $note='please contact vitali.choutko@cern.ch for details';
        $ret = $self->{sqlserver}->Query($sql);

        if (defined $ret->[0][0]) {
         $note ="DST directory path : $ret->[0][0]/nt \n Journal files directory path : $ret->[0][0]/jou \n";
         }
         $message=$message.$note;
                  my $attach;
       if ($self->{CCT} eq "remote"){
          if($self->{senddb}==1){
          $self->{TU1}=time();
           $attach="$file2tar.gz,$dataset->{version}mcscripts.tar.gz;$filedb,ams02mcdb.tar.gz";
          }
          elsif($self->{sendaddon}==1){
              $self->{TU2}=time();
              $self->{sendaddon}=0;
             $attach= "$file2tar.gz,$dataset->{version}mcscripts.tar.gz;$filedb_att,ams02mcdb.addon.tar.gz";
          }
          else{
              $attach= "$file2tar.gz,$dataset->{version}mcscripts.tar.gz";
          }

                  $self->sendmailmessage($address,$subject,$message,$attach);
          my $dire="";
    my $sqle="SELECT myvalue FROM Environment WHERE mykey='AMSSCRIPTS'";
    my $rete = $self->{sqlserver}->Query($sqle);
    if (defined $rete->[0][0]) {
        $dire = $rete->[0][0];
    }
    else{
                $self->ErrorPlus( "unable to find AMSSCRIPTS dir ");
    }                       
          
          system("mkdir -p $dire/$self->{CCA}");
          my $cmde="cp $file2tar.gz $dire/$self->{CCA}/";
          my $ie=system($cmde);
          if($ie){
                $self->ErrorPlus( "unable to $cmde ");
          }
    if($self->{jst} eq 'C' and $self->{CCA} eq 'taiwan'){
    }
          else{
                  my $i=unlink "$file2tar.gz";
              }
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




#
#         if custom and taiwan - print all the job scripts as html output
# 


    if($self->{jst} eq 'C' and $self->{CCA} eq 'taiwan'){
         my $dir2tar="/tmp/tar.$self->{CCA}.".$$;
        system("rm -rf $dir2tar");
        system("mkdir  -p $dir2tar");
       
         my $cmd ="tar -zxvf $file2tar.gz -C$dir2tar 1>/dev/null 2>&1";
         my $i=system($cmd);
               if($i){
                  system("rm -rf $dir2tar");
                  unlink "$file2tar.gz";
                  $self->ErrorPlus("Unable to untar: $cmd ");
               }
               else{
                 opendir THISDIR, $dir2tar or die "unable to open $dir2tar";
                 my @jobs=readdir THISDIR;
                 closedir THISDIR;
                 print $q->header("text/html ");
                 $q->start_html();
                 my $min=20000000000;
                 my $max=0;
                 foreach my $job (@jobs){
                     if($job =~ /\.job$/){
                       if($job =~ /^\./){
                           next;
                       }
                       my @junk=split '\.',$job;
                           if($#junk>1){
                               if($min>$junk[1]){
                                  $min=$junk[1];
                                }
                               if($max<$junk[1]){
                                  $max=$junk[1];
                                }
                            }
                      }
                     }
                     my $jobsetid="$min$max";
                       print $q->h1("JOBSETID=$jobsetid");
                 foreach my $job (@jobs){
                     if($job =~ /\.job$/){
                       if($job =~ /^\./){
                           next;
                       }
                       my $full="$dir2tar/$job";
                       open(FILE,"<".$full) or die "Unable to open job file $full \n";
                       my $buf; 
                       read(FILE,$buf,1638400) or next;
                       close FILE;
                       print $q->h3("JOB=$job ");
                       print $q->textarea($job,$buf,20,100);
                       print $q->h3(" ");
                   }
                 }
                 system("rm -rf $dir2tar");
                 #unlink "$file2tar.gz";
                 $q->end_html;
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
          my $uplt0 = 0;                   #last upload
          my $uplt1 = 0;                   # for filedb and filedb.addon
          my $vdb   = 'XYZ';               # lastdb version loaded
          my $vvv   = "$dataset->{version}mcdb"; # dataset version

          $sql="SELECT timeu1, timeu2, vdb  FROM Mails WHERE ADDRESS='$self->{CEM}'";
          my $retime=$self->{sqlserver}->Query($sql);
          if(defined $retime->[0][0]){
           $vdb = trimblanks($retime->[0][2]);
           $uplt0    = $retime->[0][0];
           $uplt1    = $retime->[0][1];
          }

       $self->DownloadTime();

        my $timeFileDB     = 0; # file xyzmcdb.tar.gz time
        my $timeFileAttDB  = 0; #      xyzmcdbaddon.tar.gz time
        my $i              = 0;

        foreach my $filedb (@{$self->{FileDB}}) {
          $self->{FileDBLastLoad} ->[$i] = 0;
          if ($filedb =~ m/$vvv/) {
           $timeFileDB=(stat($filedb))[9];
           $self->{FileDBLastLoad} ->[$i] = $uplt0;
           last;
         }
         $i++;
        }

        $i = 0;
        foreach my $filedb (@{$self->{FileAttDB}}) {
          $self->{FileAttDBLastLoad} ->[$i] = 0;
          if ($filedb =~ m/$vvv/) {
           $timeFileAttDB=(stat($filedb))[9];
           $self->{FileAttDBLastLoad} ->[$i] = $uplt1;
           last;
         }
          $i++;
      }
       
       if ($vvv =~ $vdb) {
        if ($uplt0 == 0 || $uplt1 == 0) {
            $self->Download($vvv, $vdb);
         } elsif ($timeFileDB == 0 || $timeFileAttDB == 0) {
            $self->Download($vvv, $vdb);
         } elsif ($uplt0 < $timeFileDB || $uplt1 < $timeFileAttDB) {
            $self->Download($vvv, $vdb);
         } else {
          $self->{FinalMessage}=" Your request was successfully sent to $self->{CEM}";

         }
       } else {
         $self->Download($vvv, $vdb);
        }



     } else {
      $self->{FinalMessage}=" Your request was successfully sent to $self->{CEM}";

  }
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
                if(defined $self->{FEM2}){
                 unlink $self->{FEM2};
                }
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
#    my $i=system "/usr/local/lsf/bin/bjobs -q linux_server -u all>$file" ;
     my $i=1;
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
       if ($webmode == 1 ) {
        $ref->WarningPlus("unable to bpeek");
       } else {
           print "getior -Warning - unable to bpeek \n";
       }
     }
    return $ref->getior2();
}
sub getior2{
    my $ref=shift;
    my $file=$ref->{AMSDataDir}."/prod.log/DumpIOR".".$ref->{DataMC}";
    open(FILE,"<".$file) or return undef;
            while (<FILE>){
                if (/^IOR:/){
                    close(FILE);
                    return $_;
                }
            }
            close(FILE);
    $file ="/tmp/DumpIOR".".$ref->{DataMC}";
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
                        $self->{CCTP}=$cite->{priv};
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
                        $self->{CCTP}=$cite->{priv};
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

    my $hostname=hostname();
    my $traceinfo = Carp::shortmess("Warning message");
    $mes .= "\n\nHostname: $hostname\n\nTraceback:\n$traceinfo";
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
        $sql="UPDATE RNDM SET rid=-rid where rid=1";
             $self->{sqlserver}->Update($sql);
             return -1;
       } else {
           return $res->[0][0];
        }

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
           if( not defined $res->[1][0]){
               my $mxr=-$maxrun+1;
             $sql="UPDATE RNDM SET rid=-rid where rid=$mxr";
             $self->{sqlserver}->Update($sql);
            }
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
   print "&nbsp;<b><a href=$href> production</a>\n";
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
   print " This is an interface to the AMS Production Remote/Client Database. </TD></TR> \n";
   print "<BR>";
   print "All comments (to <font color=\"green\">  vitali.choutko\@cern.ch </font>) appreciated. 
</TD></TR>\n";
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
   print "<TR><B><font color=green     size= 5> $text </font>";
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
            print "Return to <a href=$rchtml> AMS Production Remote Client Request</a>\n";
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
    my $update = 0;

    my $sql;
    my $timenow = time();

 my $HelpTxt = "
     -c    - output will be produced as ASCII page (default)
     -h    - print help
     -v    - verbose mode
     -u    - update mode (send e-mails and change job status)
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
    if ($chop =~/^-u/) {
     $update = 1;
    }
    if ($chop =~/^-w/) {
     $webmode = 1;
    }
    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
   }



   my  @periodStartTime = $self -> getActiveProductionPeriod(1);
   my $periodStartTime=2147483647;
   foreach my $periodst  (@periodStartTime){
     if($periodst < $periodStartTime){
       $periodStartTime=$periodst;
     }
    }
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
#    if (0) {
#    }
#    else {
#     $sql = "SELECT ntuples.run,
#              FROM runs, ntuples
#               WHERE
#                     (runs.run = ntuples.run) AND
#                     (runs.submit > $periodStartTime) AND
#                     (
#                      runs.status != 'Completed' AND
#                      runs.status != 'TimeOut'   AND
#
#                      runs.status != 'Finished'  AND
#                      runs.status != 'Foreign'  AND
#                      runs.status != 'Processing')";
#      my $r1=$self->{sqlserver}->Query($sql);
#      if( defined $r1->[0][0]){
#       foreach my $r (@{$r1}){
#         my $run= $r -> [0];
#          
#           $sql="UPDATE runs SET runs.status='Completed' WHERE run=$run";
#           $self->{sqlserver}->Update($sql);
#       }
#   }
#  } # Active Production Set
#

#
#    First Select Jobsto be kille\d
#
    my $jobkilled=0;
    my $jobsaved=0;
    my $jobpostponed=0;
    $sql="SELECT jobs.jid,jobs.realtriggers,jobs.time,jobs.cid from jobs where jobs.timekill<$timenow and jobs.timekill>0";
     my $rtokill=$self->{sqlserver}->Query($sql);


    if( defined $rtokill->[0][0]){
        foreach my $jtokill (@{$rtokill}){
            if($jtokill->[1]>0){
                $sql="update jobs set timekill=0 where jid=$jtokill->[0]";
            if ($update == 1){
                $self->{sqlserver}->Update($sql);
            }
                next;
                 $jobsaved++;              
            }
          my $sql="select sum(ntuples.levent-ntuples.fevent+1),min(ntuples.fevent),sum(nevents),sum(neventserr),sum(datamc) from ntuples where jid=$jtokill->[0]";
          my $sqlr="select sum(datafiles.levent-datafiles.fevent+1),min(datafiles.fevent),sum(nevents),sum(nevents),sum(nevents) from datafiles where run=$jtokill->[0]";
          my $q=$self->{sqlserver}->Query($sql);
          my $qr=$self->{sqlserver}->Query($sqlr);
          if(defined $q->[0][0] and $q->[0][0]>0){
              my $rname="dataruns";
                  if($q->[0][4]==0){
                      $rname="runs";
                  }
              $sql="select status from $rname where jid=$jtokill->[0]";
              my $q1=$self->{sqlserver}->Query($sql);
              if(defined $q1->[0][0] ){
                $sql="update $rname set fevent=$q->[0][1], levent=$q->[0][1]+$q->[0][0]-1,status='Completed' where jid=$jtokill->[0]";
              }
              elsif($q->[0][4]==0){
                  $sql="insert into $rname values($jtokill->[0],$jtokill->[0],$q->[0][1],$q->[0][1]+$q->[0][0]-1,$jtokill->[2],$jtokill->[2],$jtokill->[2],'Completed')";
              }
            if ($update == 1){
              $self->{sqlserver}->Update($sql);  
            }
                $sql="update jobs set timekill=0,events=$q->[0][2],errors=$q->[0][3],mips=0 where jid=$jtokill->[0]";
            if ($update == 1){
                $self->{sqlserver}->Update($sql);
            }
                next;
                $jobsaved++;              
                           
          }
          elsif(defined $qr->[0][0] and $qr->[0][0]>0){
              my $rname="runs";
              $sql="select status from $rname where jid=$jtokill->[0]";
              my $q1=$self->{sqlserver}->Query($sql);
              if(defined $q1->[0][0] ){
                $sql="update $rname set fevent=$qr->[0][1], levent=$qr->[0][1]+$qr->[0][0]-1,status='Completed' where jid=$jtokill->[0]";
              }
              elsif($qr->[0][4]==0){
                  $sql="insert into $rname values($jtokill->[0],$jtokill->[0],$qr->[0][1],$qr->[0][1]+$qr->[0][0]-1,$jtokill->[2],$jtokill->[2],$jtokill->[2],'Completed')";
              }
            if ($update == 1){
              $self->{sqlserver}->Update($sql);  
            }
                $sql="update jobs set timekill=0,events=$qr->[0][2],errors=$qr->[0][3],mips=0 where jid=$jtokill->[0]";
            if ($update == 1){
                $self->{sqlserver}->Update($sql);
            }
                next;
                $jobsaved++;              
                           
          }

          else{
#
# Any run processing?
#
              $sql="select status from runs where jid=$jtokill->[0]";
              my $q1=$self->{sqlserver}->Query($sql);
              if(defined $q1->[0][0] and $q1->[0][0]=~/'Processing'/){
                $sql="update jobs set timekill=0 where jid=$jtokill->[0]";
                if ($update == 1){
                 $self->{sqlserver}->Update($sql);
                }              
                 next;
                $jobsaved++;              
            }
              else{

              $sql="select status from dataruns where jid=$jtokill->[0]";
              my $q1=$self->{sqlserver}->Query($sql);
              if(defined $q1->[0][0] and $q1->[0][0]=~/'Processing'/){
                $sql="update jobs set timekill=0 where jid=$jtokill->[0]";
                if ($update == 1){
                 $self->{sqlserver}->Update($sql);
                }              
                 next;
                $jobsaved++;              
            }
          }

#
# Last Hope - journal file ready?
#
    $sql = "SELECT dirpath FROM journals WHERE cid=$jtokill->[3]";
               $q1=$self->{sqlserver}->Query($sql);
              my $dir=$q1->[0][0]."/jou";
              $sql="select name from cites where cid=$jtokill->[3]";
               my $q2=$self->{sqlserver}->Query($sql);
               if($q2->[0][0]=~/test/ or $q2->[0][0]=~/cern/){
                 $dir=$dir."/MCProducer";       
             }
              my @allfiles=();
              my @allf2=();
              $#allfiles=-1;
              $#allf2=-1;
              if(opendir MYDIR,  $dir){ 
               @allfiles=readdir MYDIR;
               closedir MYDIR;
              }
              else{
                print " unable to open dir $dir cid =$jtokill->[3] \n";
              }
               if($q2->[0][0]=~/test/ or $q2->[0][0]=~/cern/){
                 $dir=~ s/MCProducer/Producer/;
                if(opendir MYDIR, $dir){
                  @allf2=readdir MYDIR;
                 foreach my $file (@allf2){
                     push @allfiles,$file;
                 }
                 closedir MYDIR;
               }
               }
              my $jfound=0;
              foreach my $file (@allfiles){
                if($file =~ /\.journal$/){
                    if($file=~/$jtokill->[0]/){
                        if($verbose){
                        warn "  journal file $file found for $jtokill->[0]. cowardly refuses to kill the job \n";
                    }
                        $jfound=1;
                        last;
                    }
                }
            }
              if($jfound){
                  next;
                 $jobpostponed++;              
              }


          }


#
# Here actually kill the job
#                          
# in fact move it into jobs_deleted
            $sql="insert into jobs_deleted select * from jobs where jobs.jid=$jtokill->[0]";
            if ($verbose){
               print " $sql \n";
            }
            if ($update == 1){
                $self->{sqlserver}->Update($sql);
                $sql="update jobs_deleted set timekill=0 where jid=$jtokill->[0]";
                $self->{sqlserver}->Update($sql);
            }
            $sql="delete from jobs where jid=$jtokill->[0]";
            if ($verbose){
               print " $sql \n";
            }
            if ($update == 1){
                $self->{sqlserver}->Update($sql);
            }
            $sql="delete from runs where jid=$jtokill->[0]";
            if ($update == 1){
                $self->{sqlserver}->Update($sql);
            }
            $jobkilled++;              

        }
    }      

    if($verbose){
      print "  killed/saved/posponed jobs $jobkilled,$jobpostponed,$jobsaved \n";
  }
#



#
#  Here restore jobs mistkenly deleted ...
#
#

            $sql="select jobs_deleted.jid from jobs_deleted,dataruns where jobs_deleted.jid=dataruns.jid ";
                my $und=$self->{sqlserver}->Query($sql);
                if(defined $und->[0][0] ){
                    foreach my $jid (@{$und}){
                      $sql="select path from ntuples where jid=$jid->[0]"; 
                      my $nt=$self->{sqlserver}->Query($sql);
                      if(defined $nt->[0][0]){
                          $sql="insert into jobs select * from jobs_deleted where jobs_deleted.jid=$jid->[0]";
            if ($update == 2){
                $self->{sqlserver}->Update($sql);
                $sql="delete from jobs_deleted where jid=$jid->[0]";
                $self->{sqlserver}->Update($sql);
            }
                      }
                  }
                }


#
#  Change timeout for the failed runs
#

             $sql="select jid from runs where status='Failed'";
              my $q1=$self->{sqlserver}->Query($sql);
              foreach my $runf  (@{$q1}){
              $sql="update jobs set timeout=$timenow-jobs.time-1 where jid=$runf->[0]"; 
                $self->{sqlserver}->Update($sql);
              }
                       
#             $sql="select jid from dataruns where status='Failed'";
#              $q1=$self->{sqlserver}->Query($sql);
#              foreach my $runf  (@{$q1}){
#              $sql="update jobs set timeout=$timenow-jobs.time-1 where jid=$runf->[0]"; 
#                $self->{sqlserver}->Update($sql);
#              }
#                       

#  convert jobs with ntuples or runs with "Processing" status to good ones
#  refuse to kill jobs if corr journal file exists
#

    my $jobstimeout=0;
#
# Select timeout jobs, mark them to kill in 24h and mail owners about
#
    $sql="SELECT jobs.jid, jobs.time, jobs.timeout, jobs.mid, jobs.cid
            FROM jobs
             WHERE jobs.time+jobs.timeout <  $timenow and jobs.timekill=0 AND (jobs.mips <=0 OR jobs.events=0 or jobs.realtriggers<=0)";
    my $r3=$self->{sqlserver}->Query($sql);
    if( defined $r3->[0][0]){
     foreach my $job (@{$r3}){
       my $jid          = $job->[0];
       my $tmoutflag    = 0;
       my $jobstatus    ="unknown";
       my $owner        ="xyz";
       my $address      = "vitali.choutko\@cern.ch";
       $sql="SELECT runs.run, runs.status
              FROM runs
              WHERE (runs.status = 'Failed' OR runs.status = 'Unchecked' OR runs.status='Foreign' or runs.status='ToBeRerun')  AND runs.jid=$jid";
       my $r4=$self->{sqlserver}->Query($sql);
       if (defined $r4->[0][0]) {
        $sql= "UPDATE runs SET status='TimeOut' WHERE jid=$jid";
        if ($update == 1) {$self->{sqlserver}->Update($sql); }
        $tmoutflag = 1;
        $jobstatus = $r4->[0][1];
    }
        else{
       $sql="SELECT dataruns.run, dataruns.status
              FROM dataruns
              WHERE (dataruns.status = 'Failed' OR dataruns.status = 'Unchecked' OR dataruns.status='Foreign' or dataruns.status='ToBeRerun')  AND dataruns.jid=$jid";
       my $r4=$self->{sqlserver}->Query($sql);
       if (defined $r4->[0][0]) {
        $sql= "UPDATE dataruns SET status='TimeOut' WHERE jid=$jid";
        if ($update == 1) {$self->{sqlserver}->Update($sql); }
        $tmoutflag = 1;
        $jobstatus = $r4->[0][1];
    }
       else{ 
        $sql="SELECT runs.status,jobs.timekill from runs,jobs where runs.jid=jobs.jid and runs.jid=$jid";
        my $r5=$self->{sqlserver}->Query($sql);
        if(not defined $r5->[0][0] or ($r5->[0][0] eq 'TimeOut' and $r5->[0][1] <=0)){

        $sql="SELECT dataruns.status,jobs.timekill from dataruns,jobs where dataruns.jid=jobs.jid and dataruns.jid=$jid";
        $r5=$self->{sqlserver}->Query($sql);
        if(not defined $r5->[0][0] or ($r5->[0][0] eq 'TimeOut' and $r5->[0][1] <=0)){

        $tmoutflag = 1;
        $jobstatus = 'Timeout';
    }
    }
    }
   }
       if ($tmoutflag == 1) {
           $jobstimeout++;
         my $timestamp    = $job->[1];
         my $submittime = localtime($timestamp);
         my $timeout      = $job->[2];
         my $tsubmit      = EpochToDDMMYYHHMMSS($timestamp);
         my $texpire      = EpochToDDMMYYHHMMSS($timestamp+$timeout);
         my $mid          = $job->[3];
         my $cid          = $job->[4];
         my $cite         = "xyz";
         $sql = "SELECT mails.name, mails.address, cites.name
                    FROM Mails, Cites  WHERE mails.mid=$mid and mails.cid=cites.cid";
         my $r4 = $self->{sqlserver}->Query($sql);
         if (defined $r4->[0][0]) {
            $owner   = $r4->[0][0];
            $address = $r4->[0][1].",".$address;
            if($jobstimeout>1){
                $address = $r4->[0][1];
            }              
            $cite         = $r4->[0][2];
          }
         my $timenow    = time();
         my $deletetime = localtime($timenow+24*60*60*2+10);
         my $exptime    = $timestamp+$timeout;
         $exptime       = localtime($exptime);
         my $sujet = "Job : $jid - expired";
         my $message    = "Job $jid, Submitted : $submittime, Expired : $exptime;
                         \n Job will be removed from database (Not earlier than  : $deletetime). 
                         \n Please upload Journal/Ntuples ASAP
                         \n  Production Team.
                         \n ----------------------------------------------
                         \n This message was generated by program.
                         \n DO NOT reply using REPLY option of your mailer";
         if ($update == 1) {
          $sql=" update jobs set timekill=$timenow+3600*24+10, timeout=$timenow+3600*24*7-$job->[1]+1 where jid=$jid";
          $self->{sqlserver}->Update($sql); 
          $self->sendmailmessage($address,$sujet,$message);
         }
           else{
               print "$jid \n";
           }
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
    if($verbose){
     print " jobstimeout $jobstimeout \n";
    }
    if($jobstimeout>1){
          $self->sendmailmessage('vitali.choutko@cern.ch',"Jobs Expired: $jobstimeout"," ");
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

    $self->  getProductionPeriods(0);
    $self -> listProductionSetPeriods();
    $self -> listProductionSetPeriods();
#    $self -> listStat();
#     $self -> listCites();
#      $self -> listMails();
      $self -> listNetMonitor();
       $self -> listServers();
       $self -> listDataServers();
#        $self -> listJobs();
#         $self -> listRuns();
#          $self -> listNtuples();
   my $time = localtime;
   print "<tr><td><font size=\"-1\"><b>Page Update : $time</b></td>\n";
            $self -> listDisks();
            $self -> listDisks('/Data');
    htmlBottom();
}

sub listMCStatus {
    my $self = shift;
    my $show = shift;
    htmlTop();
    $self->ht_init();

    $self->  getProductionPeriods(0);
    $self -> listProductionSetPeriods();
    $self -> listStat();
    $self -> list_24h_html();

     htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";

    $self ->FullStatusLink();

    htmlBottom();
}

sub FullStatusLink {
    print "<dt> \n";
#    print "<a href=\"/cgi-bin/mon/rc.dsp.cgi\" target=\"all\">\n";
#    print "<b><font color=green> Production Main Page (Complete status)</font></a>\n";
    print "<b><font color=green> To get Complete list click Production Main Page (in menu)</font></a>\n";
}

sub listShort {
    my $self = shift;
    my $show = shift;
    htmlTop();
    $self->ht_init();
    if ($show eq 'all') {
     $self -> colorLegend();
    }

    $self->  getProductionPeriods(0);
    $self -> listProductionSetPeriods();
    $self -> listStat();
     $self -> listCites();
      $self -> listMails();
       $self -> listServers();
        $self -> listDisks();
            $self -> listDisks('/Data');
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
   print "&nbsp;<b><a href=$href> production</a>\n";
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
  print " This is an interface to the AMS Production Remote/Client Database </TD></TR> \n";
  print "<TR><TD> \n";
  print "All comments (to <font color=\"green\">  vitali.choutko\@cern.ch </font>) appreciated. </TD></TR>\n";
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
                  push @desc, $hash -> {$cite->{filename}};
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

#
    if ($webmode == 1) {
     print "<b><h2><A Name = \"mc02sets\"> </a></h2></b> \n";
     htmlTable("Production Sets");
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<td align=center><b><font color=\"blue\">ID </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Name</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Begin </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >End </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Status</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Versions (db, exec, os)</font></b></td>";
               print "</tr>\n";
      }
      foreach my $mc (@productionPeriods) {

          my $tbegin = EpochToDDMMYYHHMMSS($mc->{begin});
          my $tend   = '->';
          if ($mc->{end} != 0) {
              $tend = EpochToDDMMYYHHMMSS($mc->{end});
          }

          my $color='orange';
          if ($mc->{status} =~ /Active/) {$color='green';}
            if ($webmode == 1) {
               print "<td align=center><b><font color=$color>$mc->{id} </font></b></td>";
               print "<td align=center><b><font color=$color >$mc->{name}</font></b></td>";
               print "<td align=center><b><font color=$color >$tbegin </font></b></td>";
               print "<td align=center><b><font color=$color >$tend </font></b></td>";
               print "<td align=center><b><font color=$color >$mc->{status}</font></b></td>";
               print "<td align=center><b><font color=$color >$mc->{vdb}/$mc->{vgbatch}/$mc->{vos} </font></b></td>";
               print "</tr>\n";

           }
      }

    if ($webmode == 1) {
     htmlTableEnd();
     htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
 }
}

sub listStat {
    my $self = shift;

    my $jobsreq;     # active jobs
    my $jobsdone;    # successfully finished
    my $jobsfailed;  # failed
    my $jobstimeout; # timeout
    my $trigreq;     # requested events
    my $trigdone;    # processed levent-fevent+1
    my $timestart;   # 1st job request time
    my $lastupd;     # last job request time
    my $nntuples;
    my $nsizegb;
    my $cntuples;
    my $csizegb;

    my $sql        = undef;
    my $ret        = undef;

    if ($webmode == 1) {
     print "<b><h2><A Name = \"stat\"> </a></h2></b> \n";
     htmlTable("Jobs");
    }
    foreach my $ds (@productionPeriods) {
        if (not $ds->{status} =~ 'Active'){
            next;
        }
     if ($ds->{status} =~ 'Active') {

       $jobsreq    = 0;  # active jobs
       $jobsdone   = 0;  # successfully finished
       $jobsfailed = 0;  # failed
       $jobstimeout= 0;  # timeout
       $trigreq    = 0;  # requested events
       $trigdone   = 0;  # processed levent-fevent+1
       $timestart  = 0;  # 1st job request time
       $lastupd    = 0;  # last job request time
       $nntuples   = 0;  # ntuples
       $nsizegb    = 0;  # NT size in GB
       $cntuples   = 0;  # NT copied to CASTOR
       $csizegb    = 0;  #                     GB

       my $datasetStartTime = $ds->{begin};
       my $datasetId        = $ds->{id};
       my $datasetVDB       = $ds->{vdb};
       my $datasetName      = $ds->{name};
# first job timestamp and running (active jobs)
       my @druns=("Runs","Dataruns");
       foreach my $run (@druns){
       $td[0] = time();
       $sql="SELECT COUNT($run.run),SUM($run.fevent), SUM($run.levent)
                FROM Jobs, $run
                  WHERE $run.submit>= $datasetStartTime
                     AND ($run.status='Completed' OR $run.Status='Finished')
                     AND Jobs.pid=$datasetId AND Jobs.cid != $TestCiteId
                      AND $run.jid=Jobs.jid";
       $ret=$self->{sqlserver}->Query($sql);
       if (defined $ret->[0][0]) {
           $jobsdone+= $ret->[0][0];
           if ($jobsdone > 0) {
            #$trigdone    = $ret->[0][2] - $ret->[0][1] + $jobsdone;
           }


       $sql="SELECT COUNT($run.run),SUM($run.fevent), SUM($run.levent)
                FROM Jobs, $run
                  WHERE $run.submit>= $datasetStartTime
                     AND ($run.status='Failed' OR $run.Status='Unchecked')
                     AND Jobs.pid=$datasetId AND Jobs.cid != $TestCiteId
                      AND $run.jid=Jobs.jid";
       $ret=$self->{sqlserver}->Query($sql);
       if (defined $ret->[0][0]) {
           $jobsfailed+= $ret->[0][0];
       }
       }
   }
       if ($jobsdone > 0 || $jobsfailed >0 || 1) {
        if ($ds->{name} =~ /2005A/) {
          $sql="SELECT
                stat_2005A_Jobs.firstjobtime,stat_2005A_Jobs.lastjobtime,
                stat_2005A_Jobs.totaljobs,stat_2005A_Jobs.totaltriggers,
                stat_2005A_TimeoutRuns.TotalRuns,
                stat_2005A_NTuples.TotalFiles, stat_2005A_NTuples.SizeMB,
                stat_2005A_CastorNTuples.TotalFiles, stat_2005A_CastorNTuples.SizeMB
               FROM
                  stat_2005A_Jobs, stat_2005A_TimeoutRuns,
                  stat_2005A_NTuples,stat_2005A_CastorNTuples";
        }
        elsif ($ds->{name} =~ /2005B/) {
          $sql="SELECT
                stat_2005B_Jobs.firstjobtime,stat_2005B_Jobs.lastjobtime,
                stat_2005B_Jobs.totaljobs,stat_2005B_Jobs.totaltriggers,
                stat_2005B_TimeoutRuns.TotalRuns,
                stat_2005B_NTuples.TotalFiles, stat_2005B_NTuples.SizeMB,
                stat_2005B_CastorNTuples.TotalFiles, stat_2005B_CastorNTuples.SizeMB
               FROM
                  stat_2005B_Jobs, stat_2005B_TimeoutRuns,
                  stat_2005B_NTuples,stat_2005B_CastorNTuples";
      }
      elsif  ($ds->{name} =~ /2006A/) {
          my $target="2006A";
          $sql="SELECT
                stat_".$target."_Jobs_A.firstjobtime,stat_".$target."_Jobs_A.lastjobtime,
                stat_".$target."_Jobs_A.totaljobs,stat_".$target."_Jobs_A.totaltriggers,
                stat_".$target."_TimeoutRuns.TotalRuns,
                stat_".$target."_NTuples.TotalFiles, stat_".$target."_NTuples.SizeMB,
                stat_".$target."_CastorNTuples.TotalFiles, stat_".$target."_CastorNTuples.SizeMB, stat_".$target."_Jobs.totaltriggers
               FROM
                  stat_".$target."_Jobs_A,stat_".$target."_Jobs, stat_".$target."_TimeoutRuns,
                  stat_".$target."_NTuples,stat_".$target."_CastorNTuples";
      }
      elsif  ($ds->{name} =~ /2006B/) {
          $sql="SELECT
                stat_2006B_Jobs.firstjobtime,stat_2006B_Jobs.lastjobtime,
                stat_2006B_Jobs.totaljobs,stat_2006B_Jobs.totaltriggers,
                stat_2006B_TimeoutRuns.TotalRuns,
                stat_2006B_NTuples.TotalFiles, stat_2006B_NTuples.SizeMB,
                stat_2006B_CastorNTuples.TotalFiles, stat_2006B_CastorNTuples.SizeMB
               FROM
                  stat_2006B_Jobs, stat_2006B_TimeoutRuns,
                  stat_2006B_NTuples,stat_2006B_CastorNTuples";
      }

      elsif  ($ds->{name} =~ /\//) {
          my @starget=split '/',$ds->{name};
          
          $sql="SELECT
                stat_".$starget[1]."_Jobs.firstjobtime,stat_".$starget[1]."_Jobs.lastjobtime,
                stat_".$starget[1]."_Jobs.totaljobs,stat_".$starget[1]."_Jobs.totaltriggers,
                stat_".$starget[1]."_TimeoutRuns.TotalRuns,
                stat_".$starget[1]."_NTuples.TotalFiles, stat_".$starget[1]."_NTuples.SizeMB,
                stat_".$starget[1]."_CastorNTuples.TotalFiles, stat_".$starget[1]."_CastorNTuples.SizeMB, stat_".$starget[1]."_Jobs.totaltriggers
               FROM
                  stat_".$starget[1]."_Jobs, stat_".$starget[1]."_TimeoutRuns,
                  stat_".$starget[1]."_NTuples,stat_".$starget[1]."_CastorNTuples";
      }
        $ret=$self->{sqlserver}->Query($sql);
        if (defined $ret->[0][0]) {
         $timestart = $ret->[0][0];
         $lastupd=localtime($ret->[0][1]);
         $jobsreq     = $ret->[0][2];
         $trigreq     = $ret->[0][3];
         $jobstimeout = $ret->[0][4];
         $nntuples    = $ret->[0][5];
         $nsizegb     = sprintf("%.1f",$ret->[0][6]/1000);
         $cntuples    = $ret->[0][7];
         $csizegb     = sprintf("%.1f",$ret->[0][8]/1000);
         $jobsreq     = $jobsreq - $jobsdone - $jobsfailed;
         $trigdone    = $ret->[0][9];
      $sql="select count(jid) from jobs_deleted where  pid=$datasetId AND cid != $TestCiteId";
       my $jbs=$self->{sqlserver}->Query($sql);
       if(defined $jbs->[0][0]){
         $jobsfailed+=$jbs->[0][0];
        }
        $sql="select count(jid) from jobs where realtriggers<0 and pid=$datasetId and timekill=0";
        $jbs=$self->{sqlserver}->Query($sql);
      if(defined $jbs->[0][0]){
         $jobsreq=$jbs->[0][0];
        }

     }
    }
    
#
               my ($prodstart,$prodlastupd,$totaldays) = $self->getRunningDays($datasetStartTime);
    if ($webmode == 1) {
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
               print "<td align=center><b><font color=\"blue\" >Dataset </font></b></td>";
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
               print "<td align=center><b><font color=\"green\" > $totaldays days</font></b></td>";
              print "</tr>\n";
           }
           my $color="black";
           if ($trigreq <= 1000000) {
             $trigreq = sprintf("%.2fK",$trigreq/1000);
	   } else {
             $trigreq = sprintf("%.2fM",$trigreq/1000000);
	   }

           if ($trigdone<= 1000000) {
               $trigdone = sprintf("%.2fK",$trigdone/1000);
 	   } else {
               $trigdone = sprintf("%.2fM",$trigdone/1000000);
	   }


          if ($webmode == 1) {
           print "
                  <td align=center><b><font color=$color> $jobsreq </font></td></b>
                  <td align=center><b><font color=$color> $jobsdone </font></b></td>
                  <td align=center><b><font color=$color> $jobsfailed </font></td></b>
                  <td align=center><b><font color=$color> $jobstimeout </font></td></b>
                  <td align=center><b><font color=$color> $trigreq </font></td></b>
                  <td align=center><b><font color=$color> $trigdone </font></b></td>
                  <td align=center><b><font color=$color> $nntuples </font></b></td>
                  <td align=center><b><font color=$color> $nsizegb </font></b></td>
                  <td align=center><b><font color=$color> $cntuples </font></b></td>
                  <td align=center><b><font color=$color> $csizegb </font></b></td>
                  <td align=center><b><font color=$color> $ds->{name} </font></b></td>\n";

          print "</font></tr>\n";


       htmlTableEnd();
     htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
    }
   }
       $td[8] = time();
    }
    if ($webmode == 1) {
     htmlTable("Datasets");
    }
            my @output=();
            my $save="$self->{UploadsDir}/CalculatesMips";
             my $lupdate=$self->dblupdate();
              my $dte=stat_adv("$self->{AMSSoftwareDir}/DataSets",0,0,1);
             my  @sta = stat $save;
             my $dtime=3600;
             if($lupdate>0){
               $dtime=3600*24;
             }
             #die "  $part $dtime $lupdate $sta[9] $dte) \n";
            if(1 or ($#sta>0 and time()-$sta[9] <$dtime and $lupdate<$sta[9] and $dte<$sta[9])){
             my $dsref=retrieve($save);
             if(defined $dsref){
              foreach my $dss (@{$dsref}){
               push @output,$dss;
              }
              }
             }
             else{  
              @output=$self->calculateMipsVC(1,0);
              store(\@output,$save);
             }
     foreach my $prodperiod (@productionPeriods) {
      if ($prodperiod->{status} =~ 'Active') {
    if ($webmode == 1) {
# table per active production period
      print "<TR><B><font color=red size= 3> $prodperiod->{name} ($prodperiod->{vdb}) </font>";
      print "<p>\n";
      print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
#   datasets
      print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
      print "<td align=center><b><font color=\"blue\">Dataset </font></b></td>";
      print "<td align=center><b><font color=\"blue\" >Events </font></b></td>";
      print "<td align=center><b><font color=\"blue\" >Events </font></b></td>";
 print "<td align=center><b><font color=\"blue\" >Events </font></b></td>";
 print "<td align=center><b><font color=\"blue\" >CPU Days </font></b></td>";
 print "<td align=center><b><font color=\"blue\" >CPU Days </font></b></td>";

      print "</tr>\n";
      print "<td align=center><b><font color=\"blue\">           </font></b></td>";
      print "<td align=center><b><font color=\"blue\"> Total </font></b></td>";
      print "<td align=center><b><font color=\"blue\" >Processed</font></b></td>";
      print "<td align=center><b><font color=\"blue\" >Pending</font></b></td>";
print "<td align=center><b><font color=\"blue\" >Total</font></b></td>";
print "<td align=center><b><font color=\"blue\" >Required</font></b></td>";

      print "</tr>\n";
     }
         my $vdb               = $prodperiod->{vdb};
         my $periodStartTime   = $prodperiod->{begin};
         my $periodId          = $prodperiod->{id};
         $sql = "SELECT did, name FROM datasets where did > 111 and version='$vdb' ";
         my $r5=$self->{sqlserver}->Query($sql);
         if ($webmode == 1) {print_bar($bluebar,3);}
         if(defined $r5->[0][0]){
#         ${$r5}[$#{$r5}+1]=${$r5}[0];
#        ${$r5}[$#{$r5}]->[1]=$tmpa[0];
             for my $kk (1...5){
                 $output[$#output]->[$kk]=0;
             }
         for my $kk (0...$#{$r5}+1){
             my $ds={}; 
             if($kk<=$#{$r5}){
                 $ds=${$r5}[$kk];
             }
             else{
                 $ds=${$r5}[0];
                  $ds->[1]=$output[$#output]->[0];
             }
           my $did       = $ds->[0];
           my $dataset   = trimblanks($ds->[1]);
              my $ggot=-1;
             for my $got (0...$#output) {
               my $hash=$output[$got];
                if($dataset eq $hash->[0]){
                 $ggot=$got;
                 last;
                }
             }
             if($ggot<0){
                  next;
             }
              my $hash=$output[$ggot];
               if($ggot<$#output){
               for my $k (1...5) {
                 my $fac=1;
                 if($k==2 or $k==3){
                    $fac=$hash->[1];
                 }
                 $output[$#output]->[$k]+=$hash->[$k]*$fac;
               }
               
           }
    elsif ($output[$#output]->[1]>1){
        $output[$#output]->[2]/=$output[$#output]->[1];
        $output[$#output]->[3]/=$output[$#output]->[1];
        $output[$#output]->[2]=int(10* $output[$#output]->[2])/10;
        $output[$#output]->[3]=int(10* $output[$#output]->[3])/10;
    }
               my $events=$hash->[1]; 
             if ($events > 1000 && $events <= 1000000) {
                 $events=sprintf("%.2fK",$events/1000);
             } elsif ($events > 1000000) {
                 $events=sprintf("%.2fM",$events/1000000);
             }
#           }
#           $sql = "SELECT SUM(triggers) FROM Jobs
#                    WHERE
#                     Jobs.timestamp > $periodStartTime AND
#                     Jobs.did = $did AND
#                      Jobs.cid != $TestCiteId
#                       AND Jobs.pid = $periodId";
#           my $r7=$self->{sqlserver}->Query($sql);
#           my $triggers = 0;
#           if(defined $r7->[0][0]){
#             $triggers = $r7->[0][0];
#             if ($triggers > 1000 && $triggers <= 1000000) {
#                 $triggers=sprintf("%.2fK",$triggers/1000);
#             } elsif ($triggers > 1000000) {
#                 $triggers=sprintf("%.2fM",$triggers/1000000);
#             }
#           }
       if ($webmode == 1) {
        print "<td align=left><b><font color=\"black\"> $dataset </font></b></td>";
        print "<td align=center><b><font color=\"black\"> $events </font></b></td>";
        if($events==0){
            $hash->[3]=0;
        }
        print "<td align=center><b><font color=\"black\" >$hash->[3]\%</font></b></td>";
       print "<td align=center><b><font color=\"black\" >$hash->[2]\%</font></b></td>";
       print "<td align=center><b><font color=\"black\" >$hash->[4]</font></b></td>";
       print "<td align=center><b><font color=\"black\" >$hash->[5]</font></b></td>";



        print "</tr>\n";
        }
       }
       if ($webmode == 1) {htmlTableEnd();}
     }
         if ($webmode == 1) {
          htmlTableEnd();
          print_bar($bluebar,3);
          }
    } # Active production sets
  }  # all production sets
    if ($webmode == 1) {
     htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
    }
}

sub listCites {
    my $self = shift;

   my @periodSt = $self -> getActiveProductionPeriod(1);
   my $periodStartTime=2147483647;
   foreach my $periodst  (@periodSt){
     if($periodst < $periodStartTime){
       $periodStartTime=$periodst;
     }
    }
    if ($webmode == 1) {
     print "<b><h2><A Name = \"cites\"> </a></h2></b> \n";
     htmlTable("Cites ");
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
    }
     my $sql="SELECT cid,descr, name, status  FROM Cites ORDER by name";
     my $r3=$self->{sqlserver}->Query($sql);
        if ($webmode == 1) {
              print "<td><b><font color=\"blue\">Cite </font></b></td>";
              print "<td><b><font color=\"blue\" >CPU </font></b></td>";
              print "<td><b><font color=\"blue\" >Jobs </font></b></td>";
              print "<td><b><font color=\"blue\" >Jobs </font></b></td>";
              print "<td><b><font color=\"blue\" >Jobs </font></b></td>";
              print "<td><b><font color=\"blue\" >LastJob </font></b></td>";
              print "<td><b><font color=\"blue\" >LastJob </font></b></td>";
              print "<tr>\n";
              print "<td><b><font color=\"blue\">   </font></b></td>";
              print "<td><b><font color=\"blue\" >DaysSpent  </font></b></td>";
              print "<td><b><font color=\"blue\" >Active</font></b></td>";
              print "<td><b><font color=\"blue\" >Ends</font></b></td>";
              print "<td><b><font color=\"blue\" >Failed</font></b></td>";
              print "<td><b><font color=\"blue\" >StartTime</font></b></td>";
              print "<td><b><font color=\"blue\" >EndTime</font></b></td>";
              print "<tr>\n";
              print_bar($bluebar,3);
          }
     if(defined $r3->[0][0]){
      foreach my $cite (@{$r3}){
          my $cid    = $cite->[0];
          my $descr  = $cite->[1];
          my $name   = $cite->[2];
          my $status = $cite->[3];
          
          $sql="select sum(cputime*mips/1000/86400) from jobs,productionset where jobs.mips>0 and jobs.cid=$cid and productionset.status='Active' and jobs.pid=productionset.did";
           my $ans=$self->{sqlserver}->Query($sql); 
           $status=int($ans->[0][0]*10)/10;
          my $r4 = undef;

          my $laststarttime = 0;     # latest job start time
          my $starttime     = "---"; #                       ASCII
          my $lastendtime   = 0;
          my $endtime       = "---";
          my $jobs          = 0;     # total jobs
          my $jobsa         = 0;     #  active jobs
          my $jobsc         = 0;     #   completed jobs
          my $jobsf         = 0;     #     failed jobs
      $sql = "SELECT  DID  FROM ProductionSet WHERE STATUS='Active' ORDER BY DID";
      my $ret = $self->{sqlserver}->Query($sql);
      my $pps=undef;
   foreach my $pp  (@{$ret}){
       if(defined $pps){            $pps=$pps." or pid =";
       }
       else{
           $pps="  ( pid = ";
       }
       $pps=$pps." $pp->[0] ";
   }
   if(defined  $pps){
      $pps=$pps." ) ";
   }
          if(defined $pps){
          $sql = "SELECT MAX(TIME), COUNT(jid) FROM Jobs WHERE cid=$cid and ".$pps;
          $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {
            $laststarttime = $r4->[0][0];
            $starttime     = EpochToDDMMYYHHMMSS($laststarttime);
            $jobs          = $r4->[0][1];
          };
          if ($laststarttime != 0 ) {
           $sql = "SELECT COUNT(jid), max(timestamp) from jobs where cid=$cid and realtriggers>0 and ".$pps;
           $r4=$self->{sqlserver}->Query($sql);
           if (defined $r4->[0][0]) {
               $jobsc = $r4->[0][0];
               if ($jobsc > 0) {$lastendtime = $r4->[0][1];}
            };


           $sql = "SELECT COUNT(jid) from jobs where cid=$cid and ( realtriggers<=0 or realtriggers=null ) and timekill>0 and jid not in (select run from datafiles) and ".$pps; 
           $r4=$self->{sqlserver}->Query($sql);
           if (defined $r4->[0][0]) {
              $jobsf = $r4->[0][0];
            };
           $sql="select count(jid) from jobs_deleted where cid=$cid and jobs_deleted.jid not in (select run from datafiles) and ".$pps;
           $r4=$self->{sqlserver}->Query($sql);
          $endtime = EpochToDDMMYYHHMMSS($lastendtime);
          $jobsa = $jobs - $jobsf - $jobsc;
          $jobsf+=$r4->[0][0];
       }
          } 
          if ($webmode == 1) {
           if($starttime     ne "---"){
           print "<tr><font size=\"1\">\n";
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
      }
  }

    if ($webmode == 1) {
       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
   }
}


sub listDisks {
    my $self    = shift;
    my $path=shift;
    my $ct=1;
    if(not defined $path){
        $path='/MC';
        $ct=60;
    }
    my $lastupd = undef;
    my $sql     = undef;

    my $dpath = undef;
    my $fs    = undef;
    my $usedGBMC = 0;
    my $totalGBMC= 0;
    my $tall=0;
    my $disk=$self->CheckFS(1,$ct,0,$path);
    $self->getProductionPeriods(0);
    $sql="SELECT MAX(timestamp) FROM Filesystems";
    my $r0=$self->{sqlserver}->Query($sql);
    if( defined $r0->[0][0]){
      $lastupd=localtime($r0->[0][0]);
     }
     if ($webmode == 1) {
      print "<b><h2><A Name = \"disks\"> </a></h2></b> \n";
      print "<TR><B><font color=green size= 5><b><font color=green> Disks and Filesystems </font></b>";
      if (defined $lastupd) {
       print "<font color=green size=3><b><i> (Checked : $lastupd) </i></b></font>";
      }
      print "<p>\n";
      print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
              print "<table border=1 width=\"100%\" cellpadding=0 cellspacing=0>\n";
              print "<td><b><font color=\"blue\" >Filesystem </font></b></td>";
              print "<td><b><font color=\"blue\" >Size [GB] </font></b></td>";
              print "<td><b><font color=\"blue\" >Allowed [GB]</font></b></td>";
              print "<td><b><font color=\"blue\" >$path [GB] </font></b></td>";
              print "<td><b><font color=\"blue\" >Available [GB] </font></b></td>";
              print "<td><b><font color=\"blue\" >Status </font></b></td>";
      print_bar($bluebar,3);
     }
     $sql="SELECT host, disk, path, totalsize, occupied, available, status,
           timestamp, isonline,allowed FROM Filesystems where path like'%$path' ORDER BY available DESC  ";
     my $r3=$self->{sqlserver}->Query($sql);
     if(defined $r3->[0][0]){
      foreach my $dd (@{$r3}){
          $usedGBMC= 0;
          $dpath = $dd->[1].$dd->[2];
          if ($dd->[1] =~ /vice/) {
           $dpath  = $dd->[2];
          }
          $fs    = $dd->[0].":".$dpath;
          my $size   = 0;
          my $used   = 0;
          my $avail  = 0;
          my $status   = trimblanks($dd->[6]);
          foreach my $p (@productionPeriods) {
            if ( (not $p->{status} =~ 'Active') or 1) {
            my $ppath = $dpath."/".$p->{name};
            my $periodStartTime = $p->{begin};
            $sql = "SELECT SUM(sizemb) FROM ntuples WHERE  PATH like '$ppath%'";
            my $r4=$self->{sqlserver}->Query($sql);
            if (defined $r4->[0][0]) {
             $totalGBMC = $totalGBMC + $r4->[0][0];
             $used   = $used + $r4->[0][0];
            }
            $sql = "SELECT SUM(sizemb) FROM datafiles WHERE  PATH like '$ppath%'";
             $r4=$self->{sqlserver}->Query($sql);
            if (defined $r4->[0][0]) {
             $totalGBMC = $totalGBMC + $r4->[0][0];
             $used   = $used + $r4->[0][0];
            }

        }
        }
           $size   = $size + $dd->[3];
           $avail  = $avail+ $dd->[5];
           $status   = trimblanks($dd->[6]);
           $size/=1024;
           $avail/=1024;
           $tall+=$dd->[9]*$size;
           my $all=int($dd->[9]*$size/10)/10;
            $size=int($size*10)/10;
            $avail=int($avail*10)/10;
           $usedGBMC = sprintf("%6.1f",$used/1024);
          if($dd->[8]==0){
              $status="Offline";
          }
           my $color=statusColor($status);
            if ($webmode == 1) {
            print "<tr><font size=\"2\">\n";
             print "<td><b> $fs </b></td>
                 <td align=middle><b> $size </td>
                 <td align=middle><b> $all</td>
                 <td align=middle><b> $usedGBMC </td>
                 <td align=middle><b> $avail </b></td>
                 <td><font color=$color><b> $status </font></b></td>\n";
             print "</font></tr>\n";
           }
      }
    }
    $sql="SELECT SUM(totalsize), SUM(occupied), SUM(available) FROM Filesystems where path like '%$path'";
    my $r4=$self->{sqlserver}->Query($sql);
    if(defined $r4->[0][0]){
#      foreach my $tt (@{$r4}){
          my $total = int(10*$r4->[0][0]/1024)/10;
          my $occup = int(10*$r4->[0][1]/1024)/10;
          my $free  = int(10*$r4->[0][2]/1024)/10;
          my $status="OK";
          my $color="green";
          if(not defined $disk){
           $sql="select disk from filesystems where isonline=1";
           my $ret=$self->{sqlserver}->Query($sql);
           if(defined $ret->[0][0]){
            $status='Full';
           }
          else{ 
           $status='Collapse';
          } 
          }
          else{
           $sql="select disk from filesystems where isonline=0 and path like '%$path'";
           my $deg=$self->{sqlserver}->Query($sql);
           if(defined $deg->[0][0]){
            $status='Degraded';
           }
          }
          $color=statusColor($status);
          my $totalGB = sprintf("%6.1f",$totalGBMC/1024);
          $tall=int($tall/10)/10;
          if ($free < $total*0.01) {
            $color="blue";
            $status="Full";}
            if ($webmode == 1) {
             print "<tr><font size=\"2\">\n";
             print "<td><font color=$color><b> Total </b></td>
                    <td align=middle><b> $total </td>
                    <td align=middle><b> $tall </td>
                    <td align=middle><b> $totalGB </td>
                    <td align=middle><b> $free </b></td>
                    <td><font color=$color><b> $status </font></b></td>\n";
             print "</font></tr>\n";
         }
#      }
      }

    if ($webmode == 1) {
       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
   }
}


sub listMails {
    my $self = shift;
    if ($webmode == 1) {
     print "<b><h2><A Name = \"mails\"> </a></h2></b> \n";
     htmlTable("Authorized Users");
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
    }
     my $sql="SELECT address, Mails.name, rsite, requests, Mails.cid, Cites.cid,
              Cites.name, Mails.status FROM  Mails, Cites WHERE Cites.cid=Mails.cid
              ORDER BY Cites.name, Mails.name";

     my $r3=$self->{sqlserver}->Query($sql);
     if ($webmode == 1) {
              print "<tr><td><b><font color=\"blue\">Cite </font></b></td>";
              print "<td><b><font color=\"blue\" >User </font></b></td>";
              print "<td><b><font color=\"blue\" >e-mail </font></b></td>";
              print "<td><b><font color=\"blue\" >Responsible </font></b></td>";
              print "<td><b><font color=\"blue\" >Requests </font></b></td>";
              print "<td><b><font color=\"blue\" >Status </font></b></td>";
              print_bar($bluebar,3);
          }
    if(defined $r3->[0][0]){
      foreach my $mail (@{$r3}){
          my $name   = $mail->[1];
          my $email  = $mail->[0];
          $email=~s/\@/\_/;
          my $req    = $mail->[3];
          my $cite   = $mail->[6];
          my $status   = $mail->[7];
          my $resp   = 'no';
          if ($mail->[2] == 1) { $resp = 'yes';}
          if ($webmode == 1) {
           if($req>0 || $mail->[2] == 1){
           print "<tr><font size=\"2\">\n";
           print "<td><b> $cite </td><td><b> $name </b></td><td><b> [$email] </td><td><b> $resp </td><td><b> $req </b></td><td><b> $status </b></td>\n";
           print "</font></tr>\n";
           }
       }
      }
  }

    if ($webmode == 1) {
       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
   }
}


sub listServers {
    my $self = shift;
    if ($webmode == 1) {
     print "<b><h2><A Name = \"servers\"> </a></h2></b> \n";
     print "<TR><B><font color=green size= 5><a href=$monmcdb><b><font color=blue> V3V4 Servers </font></a><font size=3><i>(Click  servers to check current production status)</font></i></font>";
     print "<p>\n";
     print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
    }
    my $sql="SELECT dbfilename, status, createtime, lastupdate  FROM Servers where datamc=0 ORDER BY lastupdate DESC ";
    my $r3=$self->{sqlserver}->Query($sql);
    if ($webmode == 1) {
     print "<tr><td><b><font color=\"blue\">Server </font></b></td>";
     print "<td><b><font color=\"blue\" >Started </font></b></td>";
     print "<td><b><font color=\"blue\" >LastUpdate </font></b></td>";
     print "<td><b><font color=\"blue\" >Status </font></b></td>";
     print_bar($bluebar,3);
    }
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
             if ($webmode == 1) {
              if ($time - $lastupd < $srvtimeout) {
               print "<td><b> $name </td><td><b> $starttime </td><td><b> $lasttime </td><td><b> $status </td> </b>\n";
              } else {
               my $color = statusColor($status);
               print "<td><b> $name </td><td><b> $starttime </td><td><b><font color=$color> $lasttime </font></td><td><b><font color=$color> $status </font></td> </b>\n";
             }
              print "</font></tr>\n";
          }
            if ($status eq 'Dead') { $deadserver++;}
         }
        }
    }
    if ($webmode == 1) {
     htmlTableEnd();
     htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
    }
}


sub listDataServers {
    my $self = shift;
    if ($webmode == 1) {
     print "<b><h2><A Name = \"servers\"> </a></h2></b> \n";
     print "<TR><B><font color=green size= 5><a href=$monrddb><b><font color=blue> V5 Servers </font></a><font size=3><i>(Click  servers to check current production status)</font></i></font>";
     print "<p>\n";
     print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
    }
    my $sql="SELECT dbfilename, status, createtime, lastupdate  FROM Servers  where datamc=1 ORDER BY lastupdate DESC";
    my $r3=$self->{sqlserver}->Query($sql);
    if ($webmode == 1) {
     print "<tr><td><b><font color=\"blue\">Server </font></b></td>";
     print "<td><b><font color=\"blue\" >Started </font></b></td>";
     print "<td><b><font color=\"blue\" >LastUpdate </font></b></td>";
     print "<td><b><font color=\"blue\" >Status </font></b></td>";
     print_bar($bluebar,3);
    }
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
             if ($webmode == 1) {
              if ($time - $lastupd < $srvtimeout) {
               print "<td><b> $name </td><td><b> $starttime </td><td><b> $lasttime </td><td><b> $status </td> </b>\n";
              } else {
               my $color = statusColor($status);
               print "<td><b> $name </td><td><b> $starttime </td><td><b><font color=$color> $lasttime </font></td><td><b><font color=$color> $status </font></td> </b>\n";
             }
              print "</font></tr>\n";
          }
            if ($status eq 'Dead') { $deadserver++;}
         }
        }
    }
    if ($webmode == 1) {
     htmlTableEnd();
     htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
    }
}




sub listNetMonitor {
    my $self = shift;
    if ($webmode == 1) {
     print "<b><h2><A Name = \"netmon\"> </a></h2></b> \n";
     htmlTable("NetMonitor Messages");
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
    }
     my $sql="SELECT message, hosts, category,  timestamp FROM  netmon
              ORDER BY timestamp DESC";

     my $r3=$self->{sqlserver}->Query($sql);
     if ($webmode == 1) {
         print "<tr><td><b><font color=\"blue\">Category </font></b></td>";
              print "<td><b><font color=\"blue\" >Message </font></b></td>";
              print "<td><b><font color=\"blue\" >Hosts </font></b></td>";
              print "<td><b><font color=\"blue\" >Time </font></b></td>";
              print_bar($bluebar,3);
          }
    if(defined $r3->[0][0]){
        my $i=0;
        my $curtime=time();
      foreach my $msg (@{$r3}){
          my $mes   = $msg->[0];
          my $hosts   = $msg->[1];
          my $cat   = $msg->[2];
          my $stime = EpochToDDMMYYHHMMSS($msg->[3]);
          my  $color="black";
          if($i==0 and $curtime-$msg->[3]>3600*3){
            $self->sendmailmessage('41764874733@mail2sms.cern.ch',"-S-NetMonitorNotResponded"," ");
      $self->sendmailmessage('Alexandre.Eline@cern.ch',"-S-NetMonitorNotResponded"," ");
            $self->sendmailmessage('41764871287@mail2sms.cern.ch',"-S-NetMonitorNotResponded"," ");
           } 
          if($cat =~/I/){
              $color="green";
          }
          elsif($cat =~/W/){
              $color="magenta";
          }
          elsif($cat =~/E/){
              $color="tomato";
          }
          elsif($cat =~/S/){
              $color="red";
          }
          if($i++>1 and $cat =~/I/){
              next;
          }
           print "<tr><font size=\"2\">\n";
           print "<td><b><font color=$color> $cat </td><td><b><font color=$color> $mes </b></td><td><b><font color=$color> $hosts </td><td><b><font color=$color> $stime  </b></td>\n";
           print "</font></tr>\n";
      }
  }
    else{
      $self->sendmailmessage('41764874733@mail2sms.cern.ch',"-S-NetMonitorNotResponded"," ");
      $self->sendmailmessage('Alexandre.Eline@cern.ch',"-S-NetMonitorNotResponded"," ");
    }

    if ($webmode == 1) {
       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
   }
}




sub listJobs {
    my $self = shift;

    my $ret  = undef;
    my $sql  = undef;
    my $href = undef;

    my $timenow = time();
    my $timelate= $timenow - 30*24*60*60 +1; # list jobs submitted 30 days ago or earlier


#    my ($period, $periodStartTime, $periodId) = $self->getActiveProductionPeriod();
      $sql = "SELECT  DID  FROM ProductionSet WHERE STATUS='Active' ORDER BY DID";
      $ret = $self->{sqlserver}->Query($sql);
      my $pps=undef;
   foreach my $pp  (@{$ret}){
       if(defined $pps){            $pps=$pps." or jobs.pid =";
       }
       else{
           $pps="  ( jobs.pid = ";
       }
       $pps=$pps." $pp->[0] ";
   }
   if(defined  $pps){
      $pps=$pps." ) ";
   }

    if ($webmode == 1) {
     print "<b><h2><A Name = \"jobs\"> </a></h2></b> \n";
     htmlTable(" Jobs ( $PrintMaxJobsPerCite  jobs per cite submitted  earlier than 30 days ago or not completed)");
     print_bar($bluebar,3);
    }

    $sql = "SELECT name FROM cites ORDER BY name";
    $ret=$self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
     if ($webmode == 1) {
      print " <Table> \n";
      print " <TR><TH rowspan=2> Production Cites : <br> \n";
     }
     foreach my $cite (@{$ret}) {
      my $rc = $cite->[0];
      if ($webmode == 1) {
       print "</th> <th><small> \n";
       $href = $self->{HTTPcgi}."/rc.dsp.cgi\#jobs-".$rc;
       print "<a href= $href target=\"status\"> <b><font color=tomato>$rc</b></font></a>\n";
      }
     }
     if ($webmode == 1) {print "</TR></TABLE> \n";}
    }


    my @runId     = ();
    my @runStatus = ();

    $sql="SELECT Runs.jid, Runs.status from Runs
                  WHERE
                    Runs.submit >= $timelate-86400*30 ORDER BY Runs.jid";
#    my $r3=$self->{sqlserver}->Query($sql);
#    if (1) {
#      foreach my $r (@{$r3}){
#         push @runId, $r->[0];
#                 push @runStatus, $r->[1];
#       }
#     }



    $sql=defined $pps?"SELECT
                 Jobs.jid, Jobs.jobname,
                 Jobs.time, Jobs.timeout,
                 Jobs.triggers,
                 Cites.cid, Cites.name, Cites.descr,
                 Mails.name,jobs.timekill,runs.status,dataruns.status 
           FROM   Jobs left join dataruns on dataruns.jid=jobs.jid  left join runs on runs.jid=jobs.jid , Cites, Mails  
            WHERE
                 ( Jobs.timestamp <= $timelate or jobs.timeout>0) AND
                 $pps  AND  
                    Jobs.cid=Cites.cid AND
                     Jobs.mid=Mails.mid  and (dataruns.status !='Completed' or (dataruns.status is null and jobs.realtriggers<=0)) and ((runs.status!='Completed' and runs.status!='Finished') or runs.status is null)
             ORDER  BY Cites.name,  jobs.timekill desc, Jobs.timestamp+jobs.timeout ":"SELECT
                 Jobs.jid, Jobs.jobname,
                 Jobs.time, Jobs.timeout,
                 Jobs.triggers,
                 Cites.cid, Cites.name, Cites.descr,
                 Mails.name,jobs.timekill,runs.status,dataruns.status 
           FROM   Jobs left join dataruns on dataruns.jid=jobs.jid  left join runs on runs.jid=jobs.jid , Cites, Mails  
            WHERE
                  (Jobs.timestamp <= $timelate or jobs.timeout>0) 
                     AND
                    Jobs.cid=Cites.cid AND
                     Jobs.mid=Mails.mid and  (dataruns.status !='Completed' or dataruns.status is null) and ((runs.status!='Completed' and runs.status!='Finished') or runs.status is null)
             ORDER  BY Cites.name,  jobs.timekill desc, Jobs.timestamp+jobs.timeout ";

    my $r3=$self->{sqlserver}->Query($sql);
    #die $sql;

     if ($webmode == 1) {print_bar($bluebar,3);}
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
          if($job->[9]>0){
             $texpire=$job->[9];
          }
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



          if ($newline ne $cite) {
             $newline = $cite;
             if ($webmode == 1) {
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
          }
          if ($njobs < $PrintMaxJobsPerCite) {
           $status = "Submitted";
           $color  = "black";
#           for (my $i=0; $i<$#runId+1; $i++) {
#               if ($runId[$i] == $jid) {
#                 $status = $runStatus[$i];
#                 $color  = statusColor($status);
#                 last;
#               } elsif ($runId[$i] > $jid) {
#                   last;
#               }
#           }
#                if($status eq 'Completed'){
#                   next;
#                 }
#                elsif($status eq "Submitted" and $job->[9] >0 ) {
#                  $status='TimeOut';
#                  $color  = statusColor($status);
#                }
           if(($job->[10] eq "Submitted" or $job->[11] eq "Submitted") and  $job->[9] >0){
                  $status='TimeOut';
                  $color  = statusColor($status);
             }
                

            if ($webmode == 1) {
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
           }
          $njobs++;
      }
    }
    if ($webmode == 1) {
     htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
    }
}

sub listRuns {
    my $self = shift;
    my $rr   = 0;
    my $time24h = time() - 24*60*60;

    if ($webmode == 1) {
     print "<b><h2><A Name = \"runs\"> </a></h2></b> \n";
     htmlTable("Runs");
     my $href=$self->{HTTPcgi}."/rc.o.cgi?queryDB04=Form";
     print "<tr><font color=blue><b><i> Only 50 runs submitted/finished during last 24h are listed,  to get complete list
            <a href=$href> click here</a>
            </b><i></font></tr>\n";

              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     }
    my @rname=("runs","dataruns");
    foreach my $run (@rname){
     my $sql="SELECT run, jid, submit, status
               FROM   $run
                 WHERE
                  rownum<50 and submit> $time24h 
                    ORDER  BY submit DESC, jid";
     my $r3=$self->{sqlserver}->Query($sql);
     if ($webmode == 1) {
              print "<tr><td><b><font color=\"blue\" >JobId </font></b></td>";
              print "<td><b><font color=\"blue\">$run </font></b></td>";
              print "<td><b><font color=\"blue\" >Started </font></b></td>";
              print "<td><b><font color=\"blue\" >Status </font></b></td>";
              print "</tr>\n";
             print_bar($bluebar,3);
          }
     if(defined $r3->[0][0]){
      foreach my $run (@{$r3}){
          my $nn        = $run->[0];
          my $jid       = $run->[1];
          my $starttime = EpochToDDMMYYHHMMSS($run->[2]);
          my $status    = $run->[3];
          my $color = statusColor($status);
          if ($webmode == 1) {
           print "<td><b><font color=$color> $jid       </font></td></b><td><b> $nn </td>
                  <td><b><font color=$color> $starttime </font></b></td>
                  <td><b><font color=$color> $status    </font></b></td> \n";
           print "</font></tr>\n";
          }
          #$rr++;
          #if ($rr > 50) { last;}
      }
  }
}

    if ($webmode == 1) {
       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
   }
}

sub listNtuples {
    my $self = shift;
    my $nn   = 0;

    my $time24h = time() - 24*60*60;

    if ($webmode == 1) {
     print "<b><h2><A Name = \"ntuples\"> </a></h2></b> \n";
     print "<TR><B><font color=green size= 5><b>NTuples </font></b>";
     my $href = $self->{HTTPcgi}."/rc.o.cgi?queryDB04=Form";
     print "<tr><font color=blue><b><i> DSTs produced in last 24h, to get complete list
            <a href=$href> click here</a>
            </b><i></font></tr>\n";
     print "<p>\n";


     print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
              print "<table border=1 width=\"100%\" cellpadding=0 cellspacing=0>\n";
    }
     my $sql="SELECT Ntuples.run, Ntuples.jid, Ntuples.nevents, Ntuples.neventserr,
                     Ntuples.timestamp, Ntuples.status, Ntuples.path
              FROM   Ntuples
              WHERE  rownum<100 and Ntuples.timestamp > $time24h AND Ntuples.path NOT LIKE '/castor/cern.ch%'
              ORDER  BY Ntuples.timestamp DESC, Ntuples.jid";
     my $r3=$self->{sqlserver}->Query($sql);
     if ($webmode == 1) {
              print "<tr><td width=10% align=left><b><font color=\"blue\" > JobId </font></b></td>";
              print "<td width=10%><b><font color=\"blue\"> Run </font></b></td>";
              print "<td width=15%><b><font color=\"blue\" > LastUpdate </font></b></td>";
              print "<td td align=middle><b><font color=\"blue\" > FilePath </font></b></td>";
              print "<td width=10%><b><font color=\"blue\" > Events </font></b></td>";
              print "<td width=10%><b><font color=\"blue\" > Errors </font></b></td>";
              print "<td width=10%><b><font color=\"blue\" > Status </font></b></td>";
              print "</tr>\n";
              print_bar($bluebar,3);
          }
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
          if ($webmode == 1) {
           print "<td><b> $jid </td></b><td><b> $run </td>
                  <td><b> $starttime </b></td>
                  <td><b> $path </b></td>
                  <td align=middle><b> $nevents </b></td>
                  <td align=middle><b> $nerrors </b></td>
                  <td align=middle><b><font color=$color> $status </font></b></td> \n";
           print "</font></tr>\n";
          }
          #$nn++;
          #if ($nn > 100) { last;}
      }
  }
    if ($webmode == 1) {
       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
   }
}

sub AllDone{
    htmlTop();
     my $self = shift;
               if(defined $self->{FEM2}){
                 unlink $self->{FEM2};
                }

     print "<font size=\"5\" color=\"green\">Download finished. Your request was successfully sent to $self->{CEM}";       htmlReturnToMain();
    htmlBottom();
}

sub DownloadTime {
        my $self = shift;

        my $filedb = 'xyz';

        foreach $filedb (@{$self->{FileDB}}) {
          my $file ="$self->{UploadsDir}/$filedb";
          my $ftime=(stat($file))[9];
          push @{$self->{FileDBTimestamp}}, $ftime;
        }

        foreach $filedb (@{$self->{FileAttDB}}) {
         my $file="$self->{UploadsDir}/$filedb";
         my $ftime=(stat($file))[9];
         push @{$self->{FileAttDBTimestamp}}, $ftime;
        }

        $filedb="$self->{UploadsDir}/$self->{FileBBFTP}";
        $self->{FileBBFTPTimestamp}=(stat($filedb))[9];

        $filedb="$self->{UploadsDir}/$self->{FileBookKeeping}";
        $self->{FileBookKeepingTimestamp}=(stat($filedb))[9];

        $filedb="$self->{UploadsDir}/$self->{FileCRC}";
        $self->{FileCRCTimestamp}=(stat($filedb))[9];

    }

sub Download {
    my $self = shift;
    my $vvv  = shift; # dataset version
    my $vdb  = shift; # last downloaded version

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
    my $dtime0  = 0;
    my $dtime1  = 0;
    my $dtime00 = 0;
    my $dtime11 = 0;
    my $dtime  = 0;
    my $i      = 0;
    foreach my $filedb (@{$self->{FileAttDB}}) {
      if ($filedb =~ m/$vvv/) {
       $dtime0 = EpochToDDMMYYHHMMSS($self->{FileAttDBLastLoad} ->[$i]);
       $dtime00= EpochToDDMMYYHHMMSS($self->{FileAttDBTimestamp}->[$i]);
       last;
      }
      $i++;
    }
    $i = 0;
    foreach my $filedb (@{$self->{FileDB}}) {
      if ($filedb =~ m/$vvv/) {
       $dtime1 = EpochToDDMMYYHHMMSS($self->{FileDBLastLoad} ->[$i]);
       $dtime11= EpochToDDMMYYHHMMSS($self->{FileDBTimestamp}->[$i]);
       last;
      }
     $i++;
   }
    print "<br><font color=\"green\" size=\"4\"><b><i> Last downloaded (version $vdb) : </i></font><b> $dtime1, $dtime0 </b>\n";
    print "<br><font color=\"green\" size=\"4\"><b><i>Files updated    : </i></font><b> $dtime11, $dtime00 </b>\n";
    print "<br><font color=\"red\" size=\"5\"><b><i> It is absolutely mandatory to download files</b></i></font>\n";
    $self->PrintDownloadTable($vvv);
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
          print "<input type=\"hidden\" name=\"VDB\" value=$vvv>\n";
          print "<input type=\"submit\" name=\"Download\" value=\"Finish\">\n";

   print "</FORM>\n";
    print "</BODY>\n";
    print "</HTML>\n";
}


sub DownloadSA {
    my $self = shift;
    $self->{UploadsHREF}="AMS02MCUploads";
    $self->{UploadsDir} ="/var/www/cgi-bin/AMS02MCUploads";

    push @{$self->{FileDB}}, "v3.00mcdb.tar.gz";
    push @{$self->{FileDB}}, "v4.00mcdb.tar.gz";
    push @{$self->{FileDB}}, "v4.00rddb.tar.gz";
    push @{$self->{FileDB}}, "v5.00mcdb.tar.gz";
    push @{$self->{FileDB}}, "v5.00rddb.tar.gz";
    push @{$self->{FileDB}}, "v5.01mcdb.tar.gz";
    push @{$self->{FileDB}}, "v5.01rddb.tar.gz";
    push @{$self->{FileDB}}, "amsdatadir.addon.tar";

    push @{$self->{FileAttDB}}, "v3.00mcdb.addon.tar.gz";
    push @{$self->{FileAttDB}}, "v4.00mcdb.addon.tar.gz";
    push @{$self->{FileAttDB}}, "v4.00rddb.addon.tar.gz";
    push @{$self->{FileAttDB}}, "v5.00mcdb.addon.tar.gz";
    push @{$self->{FileAttDB}}, "v5.00rddb.addon.tar.gz";
    push @{$self->{FileAttDB}}, "v5.01mcdb.addon.tar.gz";
    push @{$self->{FileAttDB}}, "v5.01rddb.addon.tar.gz";

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
    $self->PrintDownloadTable('ALL');
    print "</TABLE>\n";
    print "<p>\n";
    print "</BODY>\n";
    print "</HTML>\n";
}

sub PrintDownloadTable {
    my $self = shift;
    my $vvv  = shift;

    my $file = undef;
    my $dtime= undef;


    print "<tr><td width=700>\n";
    print "<table border=\"0\" cellspacing=\"5\" cellpadding=\"5\">\n";
    print "<P>\n";
    print "<p><tr><td bgcolor=\"#ffefd5\" width=700 valign=top colspan=2>\n";
    print "<font face=\"myriad,arial,geneva,helvetica\">\n";
    print "<TABLE BORDER=2 cellpadding=3 cellspacing=3 BGCOLOR=#eed8ae align=center width=100%>\n";
    print "<TR><br>\n";
    if ($vvv =~ 'ALL') {
     print "<TD><font color=#8b1a1a size=\"6\"><b>The following files are avaialable for download</b></font>:\n";
    } else {
     print "<TD><font color=#8b1a1a size=\"6\"><b>The following files are avaialable for download ($vvv)</b></font>:\n";
    }
    print "<br><br>\n";
     print "<font color=#8b1a1a size=\"6\"><b> All executibles can also be found in /afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/exe/linux </b></font>\n";
    
    print "<br><br>\n";
     print "<font color=#8b1a1a size=\"5\"><b>To  download directly from outside CERN use </b></font>\n";
    print "<br><br>\n";
     print "<font color=#8b1a1a size=\"5\"><b>
ssh my_user_name_at_lxplus\@lxplus.cern.ch -L20000:pcamss0.cern.ch:80 -N  </b></font>\n";
    print "<br><br>\n";
     print "<font color=#8b1a1a size=\"5\"><b>
wget 'http://localhost:20000/file_name'  </b></font>\n";
    print "<br><br>\n";

#
##ams02mcdb tar
    my $download = 1;
    my $i        = 0;
    foreach my $filedb  (@{$self->{FileDB}}) {
     if ($vvv =~ 'ALL' || $filedb =~ m/$vvv/) {
      if (defined $self->{FileDBLastLoad}->[$i]) {
        if ($self->{FileDBLastLoad}->[$i] > $self->{FileDBTimestamp}->[$i]) {
            $download = 0;
        }
      }
      if ($download == 1) {
       print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$filedb> $filedb</a>
           </font>";
       $dtime=EpochToDDMMYYHHMMSS($self->{FileDBTimestamp}->[$i]);
       print "<font size=\"3\" color=\"red\"><i><b>       ( Updated : $dtime)</b></i></font>\n";
       print "<br><br>";
     } else {
      print "<br><font size=\"4\">
       <a href=load.cgi?$self->{UploadsHREF}/$filedb> $filedb</a>
           </font>";
     $dtime=EpochToDDMMYYHHMMSS($self->{FileDBTimestamp}->[$i]);
     print "<font size=\"3\" color=\"green\"><i><b>       ( Up to date : $dtime)</b></i></font>\n";
     print "<br><br>";
   }
  }
   $i++;
 }
   $download = 1;
#
## ams02mcdbaddon tar
    $i = 0;
    foreach my $filedb  (@{$self->{FileAttDB}}) {
     if ($vvv =~ 'ALL' || $filedb =~ m/$vvv/) {
      if (defined $self->{FileAttDBLastLoad}->[$i]) {
        if ($self->{FileAttDBLastLoad}->[$i] > $self->{FileAttDBTimestamp}->[$i]) {
            $download = 0;
        }
      }

      if ($self->{dwldaddon} == 1) {
       if ($download == 1) {
        print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$filedb>   $filedb </a>
           </font>\n";
       $dtime=EpochToDDMMYYHHMMSS($self->{FileAttDBTimestamp}->[$i]);
       print "<font size=\"3\" color=\"red\"><i><b>       ( Updated : $dtime)</b></i></font>\n";
       print "<br><br>\n";
      } else {
       print "<br><font size=\"4\">
         <a href=load.cgi?$self->{UploadsHREF}/$filedb>   $filedb </a>
           </font>\n";
       $dtime=EpochToDDMMYYHHMMSS($self->{FileAttDBTimestamp}->[$i]);
       print "<font size=\"3\" color=\"green\"><i><b>       ( Up to date : $dtime)</b></i></font>\n";
       print "<br><br>\n";
      }
  }
  }
     $i++;
 }
#
## bbftp tar
     $file= $self->{FileBBFTP};
     print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$file>  bbftp files (tar.gz) - <i> optional </i></a>
           </font>";
     $dtime=EpochToDDMMYYHHMMSS($self->{FileBBFTPTimestamp});
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
      print "&nbsp;<b><a href=$href> production</a>\n";
    print "</b></td>\n";
    print "<td align=right> <font size=\"-1\">\n";
    print "&nbsp;</font></td></tr>\n";
    print "<tr bgcolor=\"#ffdc9f\"><td align=center colspan=2><font size=\"+2\" color=\"#3366ff\">";
    print "<b> AMS  Production Status \n";
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
   my $oracle_version=$self->OracleVersion();
   $time= EpochToDDMMYYHHMMSS($dbtime);
   print "<td align=right><font size=\"-1\"><b> $oracle_version Last Update : $time </b></td></tr>\n";
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
        <b><font color=green>Submit  Job</b></font></a>\n";
 print "<dt><img src=\"$maroonbullet\">&#160;&#160;<a href=$rchtml>
        <b><font color=green>User and/or Cite Registration Form</b></font></a>\n";
 print "<dt><img src=\"$silverbullet\">&#160;&#160;
        <a href=$downloadcgi>
        <b><font color=green>Download MC data and exec files</b></font></a>\n";
}

sub OracleVersion {
     my $self = shift;
     my $sql='select banner from v$version'."  where banner like 'Oracle%' ";
     my  $ret=$self->{sqlserver}->Query($sql);
     if(defined $ret->[0][0]){
          
       return $ret->[0][0];
   }
     else{
       return  " ";
   }
 }

sub lastDBUpdate {
    my $timenow=time();
     my $self = shift;
     my $lastupd =0;
     my $sql;
     my $ret;
    my $clause=" where timestamp<$timenow";
      $sql="SELECT MAX(Cites.timestamp) FROM Cites".$clause;
      $ret=$self->{sqlserver}->Query($sql);
     if(defined $ret->[0][0]){
        if($ret->[0][0]>$lastupd){
          $lastupd=$ret->[0][0];
      }
    }
      $sql="SELECT MAX(Mails.timestamp) FROM Mails".$clause;
      $ret=$self->{sqlserver}->Query($sql);
     if(defined $ret->[0][0]){
        if($ret->[0][0]>$lastupd){
          $lastupd=$ret->[0][0];
      }
    }

      $clause=" where lastupdate<$timenow";
      $sql="SELECT MAX(Servers.lastupdate) FROM Servers ".$clause;
      $ret=$self->{sqlserver}->Query($sql);
     if(defined $ret->[0][0]){
        if($ret->[0][0]>$lastupd){
          $lastupd=$ret->[0][0];
      }
    }

     $clause=" where time<$timenow";
      $sql="SELECT MAX(Jobs.time) FROM Jobs".$clause;
      $ret=$self->{sqlserver}->Query($sql);
     if(defined $ret->[0][0]){
        if($ret->[0][0]>$lastupd){
          $lastupd=$ret->[0][0];
      }
    }

     $clause=" where submit<$timenow";
      $sql="SELECT MAX(Runs.submit) FROM Runs".$clause;
      $ret=$self->{sqlserver}->Query($sql);
     if(defined $ret->[0][0]){
        if($ret->[0][0]>$lastupd){
          $lastupd=$ret->[0][0];
      }
    }

     $clause=" where timestamp<$timenow";
      $sql="SELECT MAX(Ntuples.timestamp) FROM Ntuples".$clause;
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
    elsif ($status eq "TimeOut" or $status eq "Pending" or $status eq "Degraded") {
         $color="magenta";
    }
    elsif ($status eq "Success") {
        $color  = "green";
    }
    elsif ($status eq "Foreign" or $status eq "Reserved" or $status eq "ToBeRerun" ) {
               $color = "black";
    }
    elsif ($status eq "Processing" or $status eq "Full") {
               $color = "blue";
    }
    elsif ($status eq "Active") {
               $color = "green";
    }
    elsif ($status eq "Dead" or $status eq "Unknown"  ) {
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
 my $datamc=shift;
 my $castor=shift;
 if(not defined $datamc){
     $datamc=0;
 }
 my $firstjobtime = 0;      # first job order time
 my $lastjobtime  = 0;      # last
 my $cid          = -1;
 my $sql          = undef;
 my $ret          = undef;
 my $dbonly       = 0;
 my $mail=0;
 my $single=0;
 my $rflag        = 0;     # processing flag from FilesProcessing Table
 my $procstarttime= 0;     # files processing start time from  FilesProcessing Table

 my $HelpTxt = "
     parseJournalFiles check journal file directory for all cites
     -c      - output will be produced as ASCII page (default)
     -dbonly - do not connect to server
     -h      - print help
     -i      - prompt before files removal
     -v      - verbose mode
     -w      - output will be produced as HTML page
     -s      -  Only one journal file then quit
     -m      -  Mail owner if failed
     ./parseJournalFiles.o.cgi -c
";

  $rmprompt = 0;
  foreach my $chop  (@ARGV){
    if ($chop =~/^-c/) {
        $webmode = 0;
    }
    if ($chop =~/^-dbonly/) {
        $dbonly = 1;
    }
    if ($chop =~/^-v/) {
        $verbose = 1;
    }
    if ($chop =~/^-i/) {
        $rmprompt = 1;
    }
    if ($chop =~/^-s/) {
        $single = 1;
    }
    if ($chop =~/^-w/) {
     $webmode = 1;
    }
    if ($chop =~/^-m/) {
     $mail = 1;
      print "  Mail sending option on \n";
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
     if ($dbonly == 1) {
      if ($verbose == 1) {$self ->amsprint ("parseJournalFiles -I- DB ONLY mode, no connect to server",0);}
     } else {
     if ($verbose == 1) {$self ->amsprint ("parseJournalFiles -I- connect to server",0);}
     if (not $self->ServerConnect()){
        die "parseJournalFiles -F- Unable To Connect To Server";
     } else {
        if ($verbose == 1) {$self->amsprint("parseJournalFiles -I- connected to server. done",0);}
     }
    }
    my $whoami = getlogin();
    if ($whoami =~ 'ams' ) {
    } elsif(defined $whoami) {
      $self->amsprint("parseJournalFiles -ERROR- script cannot be run from account : $whoami",0);
      die "bye";
    }

# check flag
     ($rflag, $procstarttime) = $self->getFilesProcessingFlag();
     if ($rflag == 1) {
         my $timenow = time();
         if ($timenow - $procstarttime < 3600*6) {
          $self->amsprint("Processing flag = $rflag, $procstarttime. Stop parseJournalFiles.",0);
          return 1;
         } else {
          $self->amsprint("Processing flag = $rflag, $procstarttime (now $timenow). Reset flag and continue.",0);
          $self->resetFilesProcessingFlag();
         }
     }
     $self->initFilesProcessingFlag();
#
#   my ($period, $periodStartTime, $periodId) = $self->getActiveProductionPeriod();
   my  @period = $self -> getActiveProductionPeriod(0);
   my  @periodStartTime = $self -> getActiveProductionPeriod(1);
    my $periodStartTime=2147483647;    foreach my $periodst  (@periodStartTime){
     if($periodst < $periodStartTime){        $periodStartTime=$periodst;
     }
    }

   if ($#period<0 ) {
      $self->amsprint("parseJournalFiles -ERROR- cannot get active production set",0);
      die "bye";
  }
# set flag
   my $timenow = time();
#
    $self->set_root_env();

    $self->getProductionPeriods(0);
    $self->getProductionVersion();

 $sql = "SELECT begin FROM productionset WHERE STATUS='Active' ORDER BY begin";
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
 $BadRunID[$i]     = 0;  # Runs with oboslete ID
 $gbDST[$i]        = 0;  # GB of DSTs
#-
}

 $parserStartTime=time();# start journal files check
 my $minJobID    = 0;



 $sql = "SELECT dirpath,journals.timelast,name,journals.cid
              FROM journals,cites WHERE journals.cid=cites.cid order by cites.cid";
 $ret = $self->{sqlserver}->Query($sql);


 if(defined $ret->[0][0]){
  foreach my $jou (@{$ret}){
   $nCheckedCite++;
   my $timenow    = time();
   my $dir        = trimblanks($jou->[0]);  # journal file's path
   if($datamc==1){
       $dir=~s/MC/Data/;
   }
   my $cite       = trimblanks($jou->[2]);  # cite
   my $timestamp  = trimblanks($jou->[1]);  # time of latest processed file
   my $lastcheck  = EpochToDDMMYYHHMMSS($timestamp);
   if($jou->[3]==1){
        next;
   }   
   if ( $cid != $jou->[3]) {
    $cid        = $jou->[3];
#+ get min JID for cite and given production period
   $sql = "SELECT  min(jid) from jobs where cid=$cid and timestamp>=$periodStartTime";
   my $r0 = $self->{sqlserver}->Query($sql);
   if (not defined $r0->[0][0]) {
    $sql = "SELECT  MAX(jid) from jobs where cid=$cid and timestamp<$periodStartTime";
    my $r1 = $self->{sqlserver}->Query($sql);
    if (defined $r1->[0][0]) { $minJobID = $r1->[0][0];}
   } else {
    $minJobID = $r0->[0][0];
   }
#-
  }
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
               my $stf="/tmp/$cite";
               system("ls $dir 1>$stf 2>&1 &");
               my @stat =stat("$stf");
               unlink "$stf";
               if(not defined($stat[7]) or $stat[7]==0){
               system("ls $dir 1>$stf 2>&1 &");
               sleep (3); 
               my @stat =stat("$stf");
               unlink "$stf";
               if($stat[7]==0){
                   warn "$dir cannot be opened \n";
                   next;
               }
           }

      my $suc=opendir THISDIR ,$joudir; 
      if(!$suc){
       warn "unable to open $joudir\n";
       next;
      }
      my @allfiles= sort {$a cmp $b}  readdir THISDIR;
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
       if ($file =~ /journal$/) {
       } else {
           next;
       }
#
# files *.journal.1 and *.journal.0 are already validated
#
       my @junk = split "journal.",$file;
       if (defined $junk[1]) {
        if ($junk[1] == 1 || $junk[1] == 0 || $junk[1]==2) {
           next;
        }
       }

       my $fid  = $junk[0];
       $fid  =~ s/.journal//;
       if ($fid < $minJobID) {
        if ($webmode == 0) {
           $self->amsprint("ParseJournalFiles -W- skip $joudir/$file invalid JID $fid $minJobID \n",0);
        }
        $BadRunID[$nCheckedCite]++;
        next;
       }

        
        $newfile=$joudir."/".$file;
        my $writetime = 0;
        $writetime = (stat($newfile)) [9];
        if(not defined $writetime){
            print "$newfile is not stateble \n";
            $writetime=0; 
         }
         if ($writetime > $writelast) {
           $writelast = $writetime;
           $lastfile = $newfile;
       }

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
       if($verbose){
        print " parsejournal file  $writetime $timestamp $newfile \n";
       }
        if ($writetime > $timestamp - 24*60*60*30) {
          my ($suc,$logfile)=$self->parseJournalFile($firstjobtime,
                                 $lastjobtime,
                                 $logdir,
                                 $newfile,
                                 $ntdir,$cid,$datamc,$castor);
          if($suc>0 && $mail){
             my $sql = "SELECT jid,mid FROM Jobs WHERE jid=$suc";
             my $ret = $self->{sqlserver}->Query($sql);
             $sql = "SELECT mails.name, mails.address, cites.name
                    FROM Mails, Cites  WHERE mails.mid=$ret->[0][1] and mails.cid=cites.cid";
             my $r4 = $self->{sqlserver}->Query($sql);
             my $address="vitali.choutko\@cern.ch";
            if (defined $r4->[0][0]) {
             my   $owner   = $r4->[0][0];
             $address = $r4->[0][1];
             my $cite         = $r4->[0][2];
              my $sujet = "Validation Failed for Job:  $suc $r4->[0][1]";
             my $message="";
              if(defined $logfile){
                  if(open(FILE,"<",$logfile)){
                   read(FILE,$message,16384);
                   close FILE;
                  }
              }
                     
             $self->sendmailmessage($address,$sujet,$message);
           }

          }
          $JournalFiles[$nCheckedCite]++;
          if($single){
              last;
          }
        }
#
# check jobs processing flag if -1 stop processing
     ($rflag, $procstarttime) = $self->getFilesProcessingFlag();
     if ($rflag == -1) {
         $self->amsprint("Processing flag = $rflag, $procstarttime. Stop parseJournalFiles.",0);
         goto END_0;
     }
   }
 END_0:
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
  ($rflag, $procstarttime) = $self->getFilesProcessingFlag();
  if ($rflag == -1)  { goto END_PARSE;}
 }
END_PARSE :
    $self->amsprint("parseJournalFile -I- stop",0);
 } else {
     $self->amsprint("parseJournalFile - Warning - table Journals is empty",0);
 }
 if ($webmode == 1) {
   htmlTableEnd();
  htmlBottom();
 } else {
     $self -> printParserStat();
 }
 my ($good,$bad, $message) = $self->updateFilesProcessing();
if($mail && $good+$bad>0){
           $sql="select address from mails where rserver=1";
            my $rmail=$self->{sqlserver}->Query($sql);
             if(defined $rmail->[0][0]){
              my $address=$rmail->[0][0];
              my $sujet = "parseJournalFile: Good: $good Bad: $bad";
              $self->sendmailmessage($address,$sujet,$message);
              }
} 
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
 my $inputjfile    = shift;
 
 my $inputfile=$inputjfile.".work";
     my $cmd="cp ".$inputjfile." ".$inputfile;
     my $i=system($cmd);
    if($i){
        print " Unable to sys ",$cmd;
        return 0," ";
    }
      $cmd="rm -rf ".$inputjfile;
      $i=system($cmd);
    if($i){
        print " Unable to sys ",$cmd;
        return 0," ";
    }
 my $dirpath      = shift;
 my$cid=shift; 
#
# castor
#
 my $datamc=shift;
 if(not defined $datamc){
     $datamc=0;
 }
    my $outputpath=shift;
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

 my $leti=0;
 my $feti=2000000000;


    open(FILE,"<",$inputfile) or die "Unable to open $inputfile";
    my $buf;
    read(FILE,$buf,16384);
    close FILE;
#    check  journal filename run match
     my @sp1=split ', UID ',$buf;
     if($#sp1>0){  
      my @sp2=split ' , ', $sp1[1]; 
      if(not $inputfile=~/$sp2[0]/){
       print "Fatal - Run $sp2[0] does not match file  $inputfile\n";
         system("mv $inputfile $inputjfile.0");
        return 0;
      }
      else{
       my $i=opendir THISDIR, $dirpath;
       if($i){
         my @files=readdir THISDIR;
         close THISDIR;
         my $tnow=time(); 
         foreach my $file (@files){
         if($file=~/$sp2[0]/){
          my @bufsta=stat($dirpath."/$file");
          if($tnow-$bufsta[9]<300){
            print " Run $sp2[0] not yet completed. Postponed \n";
            return 0;
          }
         }
         }
        }
      }
     }  
    my @blocks =  split "-I-TimeStamp",$buf;


     my @cpntuples   =();
     my @mvntuples   =();
     my $copyfailed  = 0;

 my $timenow = time();

 my @jj      = split '/', $inputfile;

 my $joufile  = $jj[$#jj];

 my $copylog = $logdir."/copyValidateCRC.".$joufile.".log";

 open(FILE,">".$copylog) or die "Unable to open file $copylog\n";

#
#  check run incomplete block for data run
#
    my $run_incompleted=0;
    my $run_finished=0;
foreach my $block (@blocks) {
      if ($block =~/RunIncomplete/) {
          $run_incompleted=1;
      }
      if ($block =~/RunFinished/) {
          $run_finished=1;
      }
  }

foreach my $block (@blocks) {
      if ($block =~/RunValidated/) {
       my @junk = split ",",$block;
        for (my $i=0; $i<$#junk+1; $i++) {
        $junk[$i] = trimblanks($junk[$i]);
         if($junk[$i]=~/Path/){
             my @path=split ' ',  $junk[$i];
             $dirpath=trimblanks($path[1]);
             last;
         }
       }
   }
  }
foreach my $block (@blocks) {
    my @junk;
    @junk = split ",",$block;
    for (my $i=0; $i<$#junk+1; $i++) {
      $junk[$i] = trimblanks($junk[$i]);
    }
#
    my ($utime,@jj) = split " ",$block;
    if (defined $utime) {
        if ($utime < $firstjobtime or $utime > $lastjobtime) {
#            $BadRuns[$nCheckedCite]++;
            print FILE "*********** wrong timestamp : $utime ($firstjobtime,$lastjobtime)\n";
#            system("mv $inputfile $inputjfile.0");
#            close FILE;
#            return $jobid,$copylog;
        }
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
    my $run              = $runincomplete[2];

    $runfinishedR   = 1;
    if($#startingrun<20 or $startingrun[21]!=0){
#    data do not allow incomplete run
        warn "  Run $run incomplete while real data mode  do nothing \n";
         system("mv $inputfile $inputjfile.0");
        return 0;
    }
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
          my $mailto = "vitali.choutko\@cern.ch";
          my $subject = "RunIncomplet : ".$inputfile;
          my $text    = " Program found RunIncomplete status in ".
                         $inputfile." file. ".
                         "\n Please provide job log file to V.Choutko".
                         "\n  production team";
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

   if (not defined $startingjob[2]) {
       print FILE "Fatal - cannot find JobInfo in file $inputfile\n";
       system("mv $inputfile $inputjfile.0");
       return 0;
   }
   if ($self->getRunStatus($jobid,0) eq 'Completed') {
       print FILE "Job : $jobid Status : Completed. Return \n";
       return 0;
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

   if (defined $startingjob[2]) {
    $jobid = $startingjob[2];
    if ($self->findJob($jobid,$buf,$dirpath,$cid) != $jobid) {
       print FILE "Fatal - cannot find JobInfo for $jobid \n";
       system("mv $inputfile $inputjfile.0");
       $BadRuns[$nCheckedCite]++;
       return 0;
    }
   } else {
       print FILE "Fatal - cannot find JobInfo in file $inputfile\n";
       system("mv $inputfile $inputjfile.0");
       return 0;
   }
   if ($patternsmatched == $#StartingJobPatterns || $patternsmatched == $#StartingJobPatterns-1) {
    $startingjob[0] = "StartingJob";
    $startingjobR   = 1;
    $lastjobid = $startingjob[2];
     $sql = "UPDATE Jobs SET
                 mips = $jobmips,host='$startingjob[1]' 
                where jid=$lastjobid";
     print FILE "$sql \n";
     $self->{sqlserver}->Update($sql);

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
                                 "Errors","CPU","Elapsed","CPU/Event","Status","DataMC");

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
     
     if($lastjobid!=$startingrun[2] and $datamc!=1){
      print FILE " changung data mc $lastjobid $startingrun[2]\n";
      $startingrun[21]=2;
     }

   my $run = $startingrun[2];
    my $dataruns=$startingrun[21]!=0?"dataruns":"runs";
    $startingrun[0] = "StartingRun";
    my $sql=" select status from $dataruns where jid=$lastjobid";
        my $rq=$self->{sqlserver}->Query($sql);
    if(defined $rq->[0][0] and $rq->[0][0] =~/Completed/){
        warn "  Run $run already completed in database do nothing \n";
         system("mv $inputfile $inputjfile.1");
        return 0;
    }
   if ($patternsmatched == $#StartingRunPatterns+3 || $patternsmatched == $#StartingRunPatterns+4 ) {
    my $run = $startingrun[2];
    my $dataruns=$startingrun[21]!=0?"dataruns":"runs";
    $startingrun[0] = "StartingRun";
    my $sql=" select status from $dataruns where jid=$lastjobid";
        my $rq=$self->{sqlserver}->Query($sql);
    if(defined $rq->[0][0] and $rq->[0][0] =~/Completed/){
        warn "  Run $run already completed in database do nothing \n";
         system("mv $inputfile $inputjfile.1");
        return 0;
    }
    $startingrunR   = 1;
# insert run : run #, $jid, $fevent, $levent, $fetime, $letime, $submit, $status;
    $CheckedRuns[$nCheckedCite]++;
    if($startingrun[21]==0){
    $self->insertRun(
             $startingrun[2],
             $lastjobid,
             $startingrun[3],
             $startingrun[4],
             $feti,
             $leti,
             $startingrun[12],
             $startingrun[7]);

}
    else{
    $self->insertDataRun(
             $startingrun[2],
             $lastjobid,
             $startingrun[3],
             $startingrun[4],
             $feti,
             $leti,
             $startingrun[12],
             $startingrun[7]);

    }
     if (defined $startingrun[13]) {
      $host=$startingrun[13];
  }
    if( $startingrun[21]!=0 and ($run_incompleted==1 or $run_finished==0)){
#    data do not allow incomplete run
        warn "  Run $startingrun[2] incomplete or not finished while real data mode  do nothing \n";
         system("mv $inputfile $inputjfile.0");
        return 0;
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
    for my $icl  (0...$#CloseDSTPatterns){
        $closedst[$icl]=0;
    }
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
    if($feti>$closedst[8] && $closedst[8]>0){
        $feti=$closedst[8];
    }
    if($leti<$closedst[9]){
        $leti=$closedst[9];
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
    my $dstlink=$dstfile;
        my $inputfilel=readlink($dstfile);
    if(defined $inputfilel and $inputfilel=~/^\/castor/){
        $dstfile=$inputfilel    ;
        if(not defined $outputpath){
            $outputpath='/castor/cern.ch/ams';
        }
    }
    elsif(defined $inputfilel and $inputfilel=~/^\//){
        my @junk=split '\/',$inputfilel;
     my $path='/MC';
     if($startingrun[21]%2==1){
             $path='/Data'
             }
            my $disk="/$junk[1]";

      my $sql = "SELECT disk   FROM filesystems WHERE 
                   status='Active' and  isonline=1 and path='$path' and disk='$disk' ORDER BY priority DESC, available ";
        my $ret=$self->{sqlserver}->Query($sql);
        $dstfile=$inputfilel;
        if(defined $ret->[0][0] and $ret->[0][0] eq $disk){
            $outputpath=$disk;
        }
        else{
          $sql = "SELECT disk   FROM filesystems WHERE 
                   status='Full' and  Allowed=0  and path='$path' and disk='$disk' ORDER BY priority DESC, available ";
        $ret=$self->{sqlserver}->Query($sql);
        if(defined $ret->[0][0] and $ret->[0][0] eq $disk){
            $outputpath=$disk;
        }
        }
        
    }
    my $dstsize = -1;
    if($dstfile=~/^\/castor/){
        unlink "/tmp/castor.$$";
        system("stager_get -M $dstfile "); 
        my $cmd="/usr/bin/nsls -l $dstfile >/tmp/castor.$$";
        my $i=system($cmd);
        if(!$i){
            if(open(FILEL,"</tmp/castor.$$")){
                my $line=<FILEL>;
                close FILEL;
                my @junk=split ' ',$line;
                if($#junk>3){
                    $dstsize=$junk[4];
                }
#                foreach my $piece (@junk){
#                  if( $piece =~/^\d+$/ and $piece>1000000){
#                      $dstsize=$piece;
#                      last;
#                  }
#                }   
            }
            else{
             print "parsejournalfile-E-Unableto open file /tmp/castor.$$ \n";
            }
          
        }
        else {
            print "parsejournalfile-E-Unableto $cmd \n";
        }
        unlink "/tmp/castor.$$";
    }
    else{    
     $dstsize = (stat($dstfile)) [7] or $dstsize = -1;
 }
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
      $dstsize = sprintf("%.1f",$dstsize/1024/1024);
      $closedst[0] = "CloseDST";
       print FILE "$dstfile.... \n";

       my $ntstatus =$closedst[1];
       my $nttype   =$closedst[2];
       my $version  =$closedst[4];
       my $ntcrc    =$closedst[6];
       my $run      =$closedst[10];
       my $jobid    =$lastjobid;
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
      my $i = $self->calculateCRC($dstfile, $ntcrc,0);
      print FILE "calculateCRC($dstfile, $ntcrc) : Status : $i \n";
      if ($i != 1) {
          $unchecked++;
          $copyfailed = 1;
          $BadCRCi[$nCheckedCite]++;
          last;
      }
      my $ret = 0;
      ($ret,$i) = $self->validateDST($dstfile ,$ntevents, $nttype ,$dstlevent, $jobid);
      print FILE "validateDST($dstfile ,$ntevents, $nttype ,$dstlevent, $jobid) : Status : $i : Ret : $ret\n";
      if ($ret ==2) {
          $self->findJob($jobid,$buf,$dirpath,$cid);
      }
      elsif ($ret !=1) {
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
             my ($outputpatha,$rstatus) = $self->doCopy($jobid,$dstfile,$ntcrc,$version,$outputpath,$startingrun[21]%2);
            $outputpath=$outputpatha;
            if(defined $outputpath){
              push @mvntuples, $outputpath;
            }
            print FILE "doCopy return status : $rstatus \n";
            if ($rstatus == 1) {
                my $castortime=0;
                if($outputpath=~/^\/castor/){
                    $castortime=time();
                }
# add to castor immediately
                if($castortime==0){

#get dir name
                 my $castorPrefix = '/castor/cern.ch/ams';
                  my @junk=split '\/',$outputpath;
                  my $castordir=$castorPrefix;
                  for my $i  (2...$#junk-1){
                      $castordir=$castordir.'/'.$junk[$i];
                  }
                 my $sys="/usr/bin/nsmkdir -p $castordir";
                 my $i=system($sys);
                 my $rfcp="/usr/bin/rfcp $outputpath $castordir";
                 my $failure=system($rfcp);
                 if($failure){
                     if($verbose){
                         print " $rfcp failed \n";
                     }
                 }
                 else{
                     $castortime=time();
                 }
             }
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
                               $timestamp, 1, $castortime,$startingrun[21]%2,$feti,$leti);

             print FILE "insert ntuple : $run, $outputpath, $closedst[1]\n";
             $gbDST[$nCheckedCite] = $gbDST[$nCheckedCite] + $dstsize;
             push @cpntuples, $dstlink;
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
    my $dataruns=$startingrun[21]!=0?"dataruns":"runs";
    $sql = "UPDATE $dataruns SET LEVENT=$runfinished[4] WHERE jid=$lastjobid";
    if($startingrun[21]==0){
       $self->{sqlserver}->Update($sql);
   }
    if($startingrun[21]!=0){
    $sql = "UPDATE jobs SET realtriggers=$runfinished[3] WHERE jid=$lastjobid";   
    $self->{sqlserver}->Update($sql);
}
   print FILE "$sql \n";

    my $cputime = sprintf("%.0f",$runfinished[6]);
    my $elapsed = sprintf("%.0f",$runfinished[7]);
    $host= $runfinished[1];
    $sql = "update jobs set events=$runfinished[3], errors=$runfinished[5],
                                   cputime=$cputime, elapsed=$elapsed,
                                   host='$host',mips=$jobmips,timestamp = $timestamp
                               where jid =$lastjobid";
     print FILE "$sql \n";
     $self->{sqlserver}->Update($sql);
 
   } else {
     print FILE "parseJournalFile -W- RunFinished - cannot find all patterns $patternsmatched/$#RunFinishedPatterns\n";
 }
   #
   # end RunFinished
   #


}
  } #if defined $utime
}

 if ($startingrunR == 1 || $runfinishedR == 1) {
  $status="Failed";
  my $cmd = undef;
  my $inputfileLink = $inputjfile.".0";
  my $inputfileAdd=$inputjfile.".2";
  if ($copyfailed == 0) {
   if ($#cpntuples >= 0) {
    $status = 'Completed';
    $inputfileLink = $inputjfile.".1";
    open (FILEO,">".$inputfileAdd) or die "Unable to open $inputfileAdd \n";
    my $t=time();
    my $tl=localtime($t);
    print FILEO " \n";
    print FILEO "-I-TimeStamp $t $tl\n";      
    my $dst=$#cpntuples+1;
             my @junk=split '/',$outputpath;
             my $outp="";
             for my $i (0...$#junk-1){
                 if($i>0){
                     $outp=$outp."/";
                 }
                 $outp=$outp."$junk[$i]";
             }
    my $run=$startingrun[2];
    print FILEO ", RunValidated , Run $run , DST $dst , Path $outp  \n";
    close(FILEO);
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
    if($startingrun[21]==0){

                         $sql="select sum(ntuples.levent-ntuples.fevent+1),min(ntuples.fevent)  from ntuples,runs where ntuples.run=runs.run and runs.run=$run and ntuples.datamc=0";
                          my $r4=$self->{sqlserver}->Query($sql);
                          my $ntevt=$r4->[0][0];
                          my $fevt=$r4->[0][1];
                          if(not defined $ntevt){
                              $ntevt=0;
                          }
    if($ntevt>0){
                             $sql="UPDATE Runs SET fevent=$fevt, Levent=$ntevt-1+$fevt, fetime=$feti, letime=$leti WHERE jid=$run";
                             $self->{sqlserver}->Update($sql);
    $sql=" update jobs set realtriggers=$ntevt, timekill=0 where jid=$run";
                             $self->{sqlserver}->Update($sql);
                         }
                     }
    foreach my $ntuple (@cpntuples) {
      $cmd="rm  $ntuple";
      if ($rmprompt == 1) {$cmd = "rm -i $ntuple";}
      print FILE "$cmd\n";
      system($cmd);
      print FILE "Validation done : system command $cmd \n";
      $GoodDSTs[$nCheckedCite]++;
    }

 } else {
     $BadRuns[$nCheckedCite]++;
 }
}
  else{
    my $run=$startingrun[2];
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

    $sql = "DELETE ntuples WHERE jid=$lastjobid";
    $self->{sqlserver}->Update($sql);
    print FILE "$sql \n";
}
    my $dataruns=$startingrun[21]!=0?"dataruns":"runs";
    my $runupdate = "UPDATE $dataruns SET ";
 if ($startingrun[2] != 0) {
   $sql = $runupdate." STATUS='$status' WHERE jid=$lastjobid";
   $self->{sqlserver}->Update($sql);
   print FILE "Update Runs : $sql \n";
   if ($status eq "Failed" || $status eq "Unchecked") {
    $sql = "SELECT dirpath FROM journals WHERE cid=-1";
    my $ret=$self->{sqlserver}->Query($sql);
     if( defined $ret->[0][0]){
      my $junkdir = $ret->[0][0];
      if($verbose){
          print "run status $status  moving mv $dirpath/*$startingrun[2]* $junkdir \n";
      }
      #$cmd = "mkdir $dirpath/bad";
      #system($cmd);
      #$cmd = "mv -v $dirpath/*$run* $dirpath/bad";
      #print FILE "$cmd\n";
      #system($cmd);
       $sql="update jobs set realtriggers=-1 where jid=$jobid";
       print FILE "$sql \n";
       $self->{sqlserver}->Update($sql);  
      print FILE "Validation/copy failed : mv ntuples to $junkdir \n";
      close FILE;
      system("mv $inputfile $inputfileLink");
      return $jobid,$copylog; 
    }
   }
}

  system("mv $inputfile $inputfileLink");
  system("cat $inputfileAdd >> $inputfileLink");
  unlink $inputfileAdd;
  print FILE "mv $inputfile $inputfileLink\n";
  if ($webmode == 0 && $verbose == 1) {print "mv $inputfile $inputfileLink \n";}
  if ($status eq "Completed") {
      if($startingrun[21]==0){
    $self->updateRunCatalog($startingrun[2]);
     
    if ($verbose == 1) {print "Update RunCatalog table : $startingrun[2]\n";}
}
    return 0;
  }
}
 close FILE;
    return 0;
}

sub updateAllRunCatalog {
    my $self= shift;
#
#    if( not $self->Init()){
#        die "updateAllRunCatalog -F- Unable To Init";
#
#    }
#    if (not $self->ServerConnect()){
#        die "updateAllRunCatalog -F- Unable To Connect To Server";
#    }
#

    my $sql="select runs.run from runs left join runcatalog on (runs.run=runcatalog.run) where runcatalog.run is null and runs.status='Completed'";
#    my $sql = "SELECT Jobs.jid FROM Jobs, Runs
#               WHERE Jobs.jid=Runs.jid AND Runs.status='Completed'";
    my $r0 = $self->{sqlserver}->Query($sql);
    if(defined $r0->[0][0]){
      foreach my $job (@{$r0}) {
#       die "$job->[0]";
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
    $sql = "SELECT runs.status, jobs.content,datasetsdesc.jobdesc FROM Runs, Jobs,datasetsdesc WHERE runs.jid=jobs.jid and jobs.jid=$jid and datasetsdesc.did=jobs.did and   split(jobs.jobname) like datasetsdesc.jobname";
    my $r0 = $self->{sqlserver}->Query($sql);
    if(defined $r0->[0][0] && defined $r0->[0][1] && defined defined $r0->[0][2]){
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
#             foreach my $line (@sbuf){
#              if ($line =~ m/generate/) {
#                 $line =~ s/C //;
#                 $line =~ s/\\//;
#                 $line =~ s/\\//;
#                 $line = trimblanks($line);
#                       $sql0=$sql0.", jobname";
#                       $sql1=$sql1.","."'$line'";
#                       last;
#               }
#            }
                       $sql0=$sql0.", jobname";
                       $sql1=$sql1.","."'$r0->[0][2]'";

            $sql0=$sql0.", TIMESTAMP) ";
            $sql1=$sql1.",".$timestamp.")";
            $sql=$sql0.$sql1;
            $self->{sqlserver}->Update($sql);
      }
    } else {
#       $self->printWarn("updateRunCatalog -W- Cannot Find Run or/and Job content with JID = $jid");
    }
}

sub deleteTimeOutJobs {
   return; 
    my $self = shift;

    my $vdir = undef;
    my $vlog = undef;
    my $timenow = time();

    my $update  = 0;
#
 my $HelpTxt = "
     -c    - output will be produced as ASCII page (default)
     -h    - print help
     -v    - verbose mode
     -u    - update mode
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
    if ($chop =~/^-u/) {
        $update = 1;
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

    my $time0 = time();
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

    my $time1 = time();
    my $sql  = undef;
    $sql="SELECT jobs.jid, jobs.timestamp, jobs.timeout, jobs.mid, jobs.cid,
                 cites.name, mails.name FROM jobs, cites, mails, runs
               WHERE Jobs.jid = Runs.jid AND
                     Jobs.cid = Cites.cid AND
                     Jobs.mid = Mails.mid AND
                     Runs.status = 'TimeOut'";
    my $ret = $self->{sqlserver}->Query($sql);
    my $time2 = time();
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
           if ($update == 1) {
            $sql = "DELETE Ntuples WHERE jid=$jid";
            $self->{sqlserver}->Update($sql);
            $sql = "DELETE Jobs WHERE jid=$jid";
            $self->{sqlserver}->Update($sql);
            print FILE "$sql \n";
           }
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

    my $time3 = time();

#
# delete jobs without runs
#

    my $timedelete = time() - 4*60*60;
    $sql="SELECT jobs.jid, jobs.timestamp, jobs.timeout, jobs.mid, jobs.cid
            FROM jobs
             WHERE jobs.time+jobs.timeout <  $timedelete AND (jobs.mips <=0 OR jobs.events=0)";
#    $sql="SELECT jobs.jid, jobs.timestamp, jobs.timeout, jobs.mid, jobs.cid,
#                 cites.name FROM jobs, cites
#          WHERE jobs.timestamp+jobs.timeout < $timedelete  AND
#                jobs.cid=cites.cid";
    my $r3=$self->{sqlserver}->Query($sql);
    my $time4 = time();
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
            my $cid          = $job->[4];
            my $cite         = "XYZ";

        $sql="SELECT run  FROM runs WHERE jid = $jid";
        my $r4=$self->{sqlserver}->Query($sql);
         if ($update == 1) {
          if (not defined $r4->[0][0]) {
            $sql = "SELECT name FROM Cites where cid = $cid";
            my $r5=$self->{sqlserver}->Query($sql);
            $cite = $r5->[0][0];
            $sql = "DELETE Ntuples WHERE jid=$jid";
            $self->{sqlserver}->Update($sql);
            $sql = "DELETE Jobs WHERE jid=$jid";
            $self->{sqlserver}->Update($sql);
            print FILE "$sql \n";
            foreach my $runinfo (@{$self->{dbserver}->{rtb}}){
             if($runinfo->{Run}=$jid) {
    if($#{$self->{ardref}} >=0){
        my $ard=${$self->{ardref}}[0];
          my %nc=%{$runinfo};
try{
                $ard->sendRunEvInfo(\%nc,"Delete");
}
            catch CORBA::SystemException with{
                $self->ErrorPlus( "sendback corba exc while deleting run $runinfo->{Run}");
            };
    }
else{
              DBServer::sendRunEvInfo($self->{dbserver},$runinfo,"Delete");
            }
              print FILE "send Delete to DBServer, run=$runinfo->{Run}\n";
              last;
             }
         }

        $self->amsprint($cite,666);
        $self->amsprint($jid,666);
        $self->amsprint($tsubmit,666);
        $self->amsprint($texpire,666);
        $self->amsprint("TimeOut",666);
        $self->amsprint("Deleted",0);
        if ($webmode == 1) {print "</tr>\n";}
        }
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
    my $time5 = time();
# 1.04 1112375753 / 1112375763 / 1112375765 / 1112375770 / 1112375772 / 1112379161
# 4.04 1112608195 / 1112608203 / 1112608204 / 1112608206 / 1112608207 / 1112608225
#    print "$time0 / $time1 / $time2 / $time3 / $time4 / $time5 \n";

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
            print "InsertRun -W- Run $run already exists  \n";
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

sub insertDataRun {

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
          FROM dataRuns WHERE jid=$jid";
    my $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
     my $dbrun = $ret->[0][0];
     my $dbjid = $ret->[0][1];
     my $dbfevent = $ret->[0][2];
     my $dblevent = $ret->[0][3];
     my $dbstatus = $ret->[0][4];
            print "InsertRun -W- Run $run $jid already exists  \n";
      } else {
        $doinsert = 1;
      }
      if ($doinsert == 1) {
       $sql="INSERT INTO dataRuns VALUES(
                    $run,
                    $fevent,
                    $levent,
                    $fetime,
                    $letime,
                    '$status',
                    $jid,
                    $submit)";
        $self->{sqlserver}->Update($sql);
        if ($verbose == 1) {print "$sql \n";}
        if ($status eq "Completed") {
#          $self->updateRunCatalog($run);
#          if ($verbose == 1) {print "Update RunCatalog table : $run->{Run}\n";}
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
    my $buf=shift;
    my $dir=shift;
    my $cid=shift;
    my $rstatus = 0;
    my $sql = "SELECT jid,mid FROM Jobs WHERE jid=$jid";
    my $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
        $rstatus = $ret->[0][0];
    }
    else{
     $sql= "SELECT jid,mid FROM Jobs_deleted WHERE jid=$jid";
      $ret = $self->{sqlserver}->Query($sql);
        if (defined $ret->[0][0]) {
         $rstatus = $ret->[0][0]; 
         print "Job $jid   Restored from jobs_deleted \n"; 
         $sql="insert into jobs select * from jobs_deleted where jobs_deleted.jid=$jid";
         $self->{sqlserver}->Update($sql);
         $sql="delete from jobs_deleted where jid=$jid";
         $self->{sqlserver}->Update($sql);
         $self->{sqlserver}->Commit();
        }
        else{
#
#        got things from root file itself
#
my %CloseDSTPatterns = (
  CloseDST=>undef,
  Status=>undef,
  Type=>undef,
  Name=>undef,
  Version=>0,
  Size=>0,
  crc=>0,
  Insert=>0,
  Begin=>0,
  End=>0,
  Run=>0,
  FirstEvent=>0,
  LastEvent=>0,
  EventNumber=>0,
  ErrorNumber=>0
                );
    my $cpat={
      %CloseDSTPatterns,
    };
    if(not defined $buf){
     my $joudir="";
     my $sql="select dirpath from journals where cid=$cid";
     my $ret=$self->{sqlserver}->Query($sql);
     if(defined $ret->[0][0]){
       $joudir=$ret->[0][0];
     }
     else{
      return 0;
     }
#     $joudir=$joudir."/jou/MCProducer/";
     opendir THISDIR,$joudir or return 0;
     my @files=readdir THISDIR;
     closedir THISDIR;
    foreach my $file (@files){
        if($file=~/$jid/ and $file=~m/journal/){
         my $newfile=$joudir.$file;
         open(FILE,"<".$newfile) or return 0;
         read (FILE, $buf,65536);
         close FILE;
         last;
       } 
    }
    }
    if(not defined $buf){
      return 0;
    }
            my @blocks =  split "-I-TimeStamp ",$buf;
            my @junk=split ' ',$blocks[1];
            my $ctime=$junk[0];
            foreach my $line (@blocks){
             if($line=~/CloseDST/){
              my @pat=split ' \, ',$line;
              foreach my $entry (@pat){
               my @kv=split ' ',$entry;
               if($#kv>0){
                $cpat->{$kv[0]}=$kv[1];
               }
              } 
              last;
             } 
            }
        if(defined $cpat->{Type} and $cpat->{Type} eq 'RootFile'){
           if(defined $cpat->{Name}){
            my @jhost=split ':',$cpat->{Name};
            my $host=$jhost[0];
            my @junk=split '\/', $cpat->{Name};
            my $fnam=$jhost[1];
            if(defined $dir){
             $fnam=$dir.'/'.$junk[$#junk];
            }
            my $validatecmd = "$self->{AMSSoftwareDir}/exe/linux/fastntrd64.exe  $fnam 0 2 0 1";
            system($validatecmd);
             $fnam=$fnam.'.jou'; 
            system("sed -i \"s/\\.job.*/.job/g\" $fnam");
             my $bufj="";
             if(open(FILEJ,"<".$fnam)){
               read(FILEJ,$bufj,32768);
               close FILEJ;
             }
             unlink $fnam;
             my $job={};
             my @cont=split '\n',$bufj;
             my @pats=("DATASETNAME","ScriptName","TRIG","NICKNAME","SUBMITTIME");
             $job->{TRIG}=100000000;
               foreach my $line (@cont){
              foreach my $pat (@pats){
                if($line =~/$pat=/){
                  my @junk=split '=',$line;
                  if($pat eq "ScriptName"){
                   my @j2=split '\/',$junk[1];
                   $job->{$pat}=trimblanks($j2[$#j2]);
                  }
                  else{
                   if($#junk>0){
                     $job->{$pat}=trimblanks($junk[1]);
                   }
                  }
                }
              }
             }
             my $ok=1; 
             foreach my $pat (@pats){
              if(not defined $job->{$pat}){
                $ok=0;
                last;
              }
             }
             if(not $ctime =~/^\d+$/ ){
               $ctime=$job->{SUBMITTIME};
             }
             $sql="select did from datasets where name='$job->{DATASETNAME}'";
             $ret=$self->{sqlserver}->Query($sql);
             if($ok and defined $ret->[0][0]){
              my $did=$ret->[0][0];
              $sql="select mid from mails where cid=$cid";
              $ret=$self->{sqlserver}->Query($sql);
              my $pid = $self->getProductionSetIdByDatasetId($did);
              $bufj=~s/\$/\\\$/g;    
              $bufj=~s/\"/\\\"/g;
              $bufj=~s/\(/\\\(/g;
              $bufj=~s/\)/\\\)/g;
              $bufj=~s/\'/\\'/g;  
              if($self->{sqlserver}->{dbdriver} =~ m/Oracle/){
               $bufj =~ s/'/''/g;
              }

if(length($bufj)>=4000){
    $bufj=~s/G4INSTALL/G4I/g;
}
if(length($bufj)>=4000){
    $bufj=~s/export/et/g;
}
if(length($bufj)>4000){
    my $len=length($bufj)-4000;
    my $mes="CUTTED:$len"; 
    $len=length($bufj)-4000+length($mes);
    $mes="CUTTED:$len"; 
    $bufj = substr $bufj, 0, 4000-length($mes);
    $bufj="$bufj$mes";
}
              my $insertjobsql="INSERT INTO Jobs VALUES
                             ($jid,
                              '$job->{ScriptName}',
                              $ret->[0][0],
                              $cid,
                              $did,
                              $job->{SUBMITTIME},
                              $job->{TRIG},
                              864000,
                              '$bufj',
                              $ctime,
                              '$job->{NICKNAME}',
                               '$host',0,0,0,0,'STANDALONE',
                              -1, $pid,-1,0)";
               $self->{sqlserver}->Update($insertjobsql);
               $self->{sqlserver}->Commit();    
               $sql = "SELECT jid,mid FROM Jobs WHERE jid=$jid";
               $ret = $self->{sqlserver}->Query($sql);
               if (defined $ret->[0][0]) {
                $rstatus = $ret->[0][0];
                print "  Job $jid restored from ntuple\n";
               }                                               
              }
           }
            
        }

        }
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
  my $datamc=shift;
    my $fetime=shift;
    my $letime=shift;
#
  my $sql      = undef;
  my $ret      = undef;
#
  my @junk    = split "/",$path;
  my $filename = trimblanks($junk[$#junk]);
  my $dsn=trimblanks($junk[$#junk-1]);
              my @sp1=split 'build',$version;
              my @sp2=split '\/',$sp1[1];
              my $buildno=$sp2[0];

  my $sizemb = $ntsize; # since 01.08 size in mbytes
#  if ($ntsize > 4000) {
#    $ntsize = $ntsize;
    $sizemb = sprintf("%.f",$ntsize);
#  }
#
  if($ntsize > 20000){
#
# susp size
#
             print " Susp file size found $ntsize $path \n";
             my $notfound = 0;
             my $filesize    = (stat($path))[7] or $notfound=1;
             if($notfound){
                 print " $path not found \n";
             }
             else{
                 $ntsize=$filesize/1024/1024;
                 print "  file size corrected to $ntsize $path \n";

             }
         }

  if($type eq 'RawFile'){
      if($datamc==0){
#    my $paths='/Offline/RunsDir/MC/';
#    my $cmd="ln -s $path $paths";
#    system($cmd);
     my $paths=$self->datasetlink($path,'/Offline/RunsDir',1);
if(not defined $fetime or not defined $letime){ 
    my $ret=$self->{sqlserver}->Query(" select fetime,letime from runs where jid=$jid");
    if(defined($ret->[0][0])){
        $fetime=$ret->[0][0];
        $letime=$ret->[0][1];
    }
}
    else{
      $self->{sqlserver}->Update(" update runs set  fetime=$fetime  where jid=$jid");
      $self->{sqlserver}->Update(" update runs set  letime=$letime  where jid=$jid");
  }
   my $rj=$self->{sqlserver}->Query(" select content,jobname from jobs where jid=$jid");   
     my $part=-1;
    my $ds="";
    my $nick="";
    my $ver="";
    if(defined($rj->[0][0])){
      my $content=$rj->[0][0];
      my @junk=split "PART=",$content;
      if($#junk>0){
          my @junk1=split "\n",$junk[1];
          if($#junk1>=0){
             $part=$junk1[0];
          }
       }
  }
    if(defined($rj->[0][0])){
      my $content=$rj->[0][0];
      my @junk=split "DATASETNAME=",$content;
      if($#junk>0){
          my @junk1=split "\n",$junk[1];
          if($#junk1>=0){
             $ds=" ".$junk1[0]." ";
          }
       }
       
  }
    if(defined($rj->[0][0])){
      my $content=$rj->[0][0];
      my @junk=split "NICKNAME=",$content;
      if($#junk>0){
          my @junk1=split "\n",$junk[1];
          if($#junk1>=0){
             $nick=" ".$junk1[0]." ";
          }
       }
       
  }
    if(defined($rj->[0][0])){
      my $content=$rj->[0][0];
      if($content =~/v4\.00/){
          $ver="v4.00";
      }
      elsif($content =~/v5\.00/){
          $ver="v5.00";
      }
   }    
    if(defined($rj->[0][1])){
      my $content=$rj->[0][1];
      my @junk=split '\.',$content;
    
          for my $i (2...$#junk){
             $ds=$ds.$junk[$i];
             if($i<$#junk){
                 $ds=$ds.'.';
             }
         }
      if($#junk>=0){
          $ds=$ds." ".$nick." ".$junk[0];
      }
       
  }
    my $stype='MC'." ".$ver." ".$ds;

     $sql="select run,path from datafiles where run=$run";
     $ret= $self->{sqlserver}->Query($sql);
     foreach my $nt (@{$ret}){
    my $cmd="rm     $nt->[1]";
    if($path ne $nt->[1]){
       system($cmd);
    }
    else{
        print " cowardly refused to rm $path \n";
    }
       
    }
    $sql="delete from datafiles where run=$run";
     $self->{sqlserver}->Update($sql);
    $sql=" insert into datafiles (RUN,VERSION,TYPE,FEVENT,LEVENT,NEVENTS,NEVENTSERR,TIMESTAMP,SIZEMB,STATUS,PATH,PATHB,CRC,CRCTIME,CASTORTIME,BACKUPTIME,TAG,FETIME,LETIME,PATHS) values($run,'$version','$stype',$fevent,$levent,$events,$errors,$timestamp,$sizemb,'$status','$path',' ',$crc,$crctime,$castortime,0,$part,$fetime,$letime,'$paths')";
}
  }
  else{
#
# check duplicated ntuple  
#
  $sql = "SELECT run, path FROM ntuples
          WHERE run=$run AND path like '%$dsn/$filename'";
  $ret = $self->{sqlserver}->Query($sql);
  if (defined $ret->[0][0]) {
      print "Duplicated ntuple found $path $run \n";
  }
  
  $sql = "SELECT run, path FROM ntuples
          WHERE jid=$jid AND path like '%$filename'";
  $ret = $self->{sqlserver}->Query($sql);
  if (defined $ret->[0][0]) {
    $sql = "DELETE ntuples WHERE jid=$jid AND path like '%$filename'";
    print "------------- $sql \n";
    print "------------- $ret->[0][1] \n";
    my $cmd="rm     $ret->[0][1]";
    if($path ne $ret->[0][1]){
       system($cmd);
    }
    else{
        print " cowardly refused to rm $path \n";
    }
   $self->{sqlserver}->Update($sql);
    
  }
  $sql = "INSERT INTO ntuples (RUN,
                               VERSION,
                               TYPE,
                               JID,
                               FEVENT,
                               LEVENT,
                               NEVENTS,
                               NEVENTSERR,
                               TIMESTAMP,
                               SIZEMB,
                               STATUS,
                               PATH,
                               CRC,
                               CRCTIME,
                               CRCFLAG,
                               CASTORTIME,
                               BUILDNO,
                               DATAMC)
                                  VALUES( $run,
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
                                           $crctime,$crcflag,$castortime,$buildno,$datamc)";
}
  if($type ne 'RawFile'){
      $self->datasetlink($path,"/Offline/DataSetsDir",1);
  }
  $self->{sqlserver}->Update($sql);
}

sub printMC02GammaTest {
    my $self = shift;

    my $buf;

    my $mc02gammafile = "/afs/cern.ch/ams/AMS02/MC/mc02-gamma-test.datasets";

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
    $self->getProductionVersion();

# DST output format
            print "<tr><td><b><font color=\"green\">DST output format</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
            print "<INPUT TYPE=\"radio\" NAME=\"RootNtuple\" VALUE=\"1=0 168=18000000 170=$self->{Build} 127=-2 128=\" $defROOT><b> RootFile </b><BR>\n";
            if ($self->{CCT} ne "remote" or $self->{CCID} == 2) {
                print "<INPUT TYPE=\"radio\" NAME=\"RootNtuple\" VALUE=\"1=3 168=120000  170=$self->{Build} 2=\" $defNTUPLE><b> NTUPLE </b>\n";
            }
            else {
                print "<INPUT TYPE=\"radio\" NAME=\"RootNtuple\" VALUE=\"1=3 168=120000  170=$self->{Build} 2=\" $defNTUPLE DISABLED=\"Yes\"><b> NTUPLE </b>\n";
            }
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
             if ($self->{CCT} ne "remote" or $self->{CCID} == 2) {
                 print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"R\" ><b> Automatic </b><BR>\n";
             }
             else {
                 print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"R\" DISABLED=\"Yes\"><b> Automatic </b><BR>\n";
             }
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
    $monrddb=$cgi."/monrddb.o.cgi";
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

    $sql = "SELECT version, vgbatch FROM ProductionSet WHERE STATUS='Active' and name like '%AMS02%'";
    my $r0 = $self->{sqlserver}->Query($sql);
    if (defined $r0->[0][0]) {
      my @junk = split '\/',$r0->[0][0];
      $junk[0] =~ s/v//;
      $junk[1] =~ s/build//;
      $junk[2] =~ s/os//;
      $self->{ProductionVersion}=$r0->[0][0];
      $self->{Version}=strtod($junk[0]);
      if(not defined $self->{Build} or $self->{Build}  <strtod($junk[1]) ){
          $self->{Build}  =strtod($junk[1]);
      }
      if(trimblanks($r0->[0][1]) > $self->{Build}){
          die "ProductionSet Inconsitency (trimblanks($r0->[0][1])  $self->{Build} \n";
      }
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

    my @junk = split '/',$dstv;
    $junk[0]    =~ s/v//;
    $junk[1] =~ s/build//;
    my $version=strtod($junk[0]);
    my $build  =strtod($junk[1]);

    foreach my $p (@productionPeriods) {
      if ($p->{name} =~ 'Active') {
       my $vdb = $p->{vdb};
       $vdb    =~ s/v//;
       my $vvv  = strtod($vdb);
       if ($vvv == $version) {
           my $pbuild = $p->{vgbatch};
           $pbuild =~ s/build//;
           my $bbb =  strtod($pbuild);
           if ($build   < $self->{Build}) {
             $rstatus = 0;
             last;
         }
      }
   }
  }
    return $rstatus;
}


sub getDSTVersion  {
#
# split DST version to : db / build/ os versions
#
    my $self = shift;
    my $vvv = shift;


    my @junk = split '/',$vvv;

    my $version  =trimblanks($junk[0]);
    my $build    =trimblanks($junk[1]);
    my $os       =trimblanks($junk[2]);

    return $version, $build, $os;
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
  my $force=shift;
  if(not defined $force){
      $force=1;
  }
  my $time0     = time();
  $crcCalls++;
  if($filename=~/^\/castor/ or ( $force==0)){
      return 1;
  }
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
     print " \n";
     my $cj =
              sprintf ("%-20.15s %-20.40s %-50.30s",
                       "Cite : $JouDirPath[$i]", "Latest Journal : $JouLastCheck[$i]", "New Files : $JournalFiles[$i]");
     print $cj;
     print FILEV "\n",$cj;
     if ($JournalFiles[$i] > 0) {
      my $l0 = "   Runs (Checked, Good, Bad, Failed) :  $CheckedRuns[$i], $GoodRuns[$i],  $BadRuns[$i], $FailedRuns[$i] \n";
      my $l1 = "   DSTs (Checked, Good, Bad, CRCi, CopyFail, CRCo) :  ";
      my $l2 = "$CheckedDSTs[$i],  $GoodDSTs[$i], $BadDSTs[$i], $BadCRCi[$i], $BadDSTCopy[$i], $BadCRCo[$i], $BadRunID[$i] \n";
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
 print "\n",$summ;
 my $ch0   = sprintf("Total Time %3.1f hours \n",$hours);
 my $mbits = 0;
 if ($doCopyTime > 0) {
     $mbits = $chGB*8*1000/$doCopyTime;}
 my $ch1 = sprintf(" doCopy (calls, time, Mbit/s) : %5d %3.1fh %3.1f [cp file :%5d %3.1fh]; \n",$doCopyCalls, $doCopyTime/60/60, $mbits, $copyCalls, $copyTime/60/60);
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
    my $message=" ";
    for (my $i=0; $i<$nCites; $i++) {
        my $cid=$i+1;
        my $sql="select name from cites where cid=$cid";
        my $ret=$self->{sqlserver}->Query($sql);
        if($FailedRuns[$i] + $BadRuns[$i]+$GoodRuns[$i]>0){ 
        $message=$message." CiteNo: $ret->[0][0] JournalFiles:  $JournalFiles[$i] GoodRuns:  $GoodRuns[$i] BadRuns: $FailedRuns[$i] + $BadRuns[$i] \n";
        }
        $nJournalFiles =  $nJournalFiles + $JournalFiles[$i];
        $nGoodRuns     =  $nGoodRuns     + $GoodRuns[$i];
        $nFailedRuns   =  $nFailedRuns   + $FailedRuns[$i] + $BadRuns[$i];
        $nGoodDSTs     =  $nGoodDSTs     +  $GoodDSTs[$i];
        $nBadDSTs      =  $nBadDSTs      +  $BadDSTs[$i] + $BadCRCi[$i] + $BadDSTCopy[$i] + $BadCRCo[$i];
    }
    my $sql = "UPDATE FilesProcessing SET Cites=$nCites, Active=$nActiveCites, JOU=$nJournalFiles,
                                          GOOD=$nGoodRuns, Failed=$nFailedRuns, GoodDSTS=$nGoodDSTs,
                                          BadDSTS = $nBadDSTs, Flag = 0, Timestamp=$timenow";
    $self->{sqlserver}->Update($sql);
    return $nGoodRuns,$nFailedRuns,$message;
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

    my $flag      = -1;
    my $starttime = 0;
    my $whoami = getlogin();
    if ($whoami =~ 'ams' || $whoami =~ 'casadmva') {
    } elsif(defined $whoami){
      $self->amsprint(" -ERROR- script cannot be run from account : $whoami",0);
      die "bye";
    }

   my $timenow = time();
   my $sql = "SELECT flag, timestamp from FilesProcessing";
   my $ret = $self->{sqlserver}->Query($sql);
   if (defined $ret) {
      $flag      = $ret->[0][0];
      $starttime = $ret->[0][1];}

   return $flag, $starttime;
}


sub setFilesProcessingFlag {
#
# set flag in DB showed that Parcing/Validation is in progress
#
    my $self = shift;


    my $whoami = getlogin();
    if ($whoami =~ 'ams' || $whoami =~ 'casadmva') {
    } elsif(defined $whoami) {
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
    my $local=shift;

    my $whoami = getlogin();
    if ($whoami =~ 'ams' || $whoami =~ 'casadmva') {
    } elsif(defined $whoami) {
      $self->amsprint("-ERROR- script cannot be run from account : $whoami",0);
      die "-ERROR- script cannot be run from account : $whoami. bye";
    }

   my $timenow = time();
   my $sql = "update FilesProcessing set flag = -1, timestamp=$timenow";
   $self->{sqlserver}->Update($sql);
    if(defined $local){
     $sql = "update FilesProcessing set flaglocal = -1, timestamp=$timenow";
     $self->{sqlserver}->Update($sql);
 }
}

sub initFilesProcessingFlag {
#
# set flag in DB to stop journal
# file processing or runs validation
#
    my $self = shift;

    my $timenow = time();

    my $sql = "update  filesprocessing set flag=1 ";
    $self->{sqlserver}->Update($sql);
    $self->{sqlserver}->Update($sql);
}

sub getOutputPath {
#
# select disk to be used to store ntuples
#
    my $self       = shift;
    my $period     = shift;
    my $path=         shift;
    my $sql        = undef;
    my $ret        = undef;
    my $outputdisk = undef;
    my $outputpath = 'xyz';
    my $mtime      = 0;
    my $gb         = 0;
    if(not defined $path){
        $path='/MC';
    }
#
$self->CheckFS(1,120,1,$path);
print "CheckFS ok \n";
# get production set path
     my $tme=time();
     if($tme%2 ==0){ 
      $sql = "SELECT disk, path, available, allowed  FROM filesystems WHERE 
                   status='Active' and isonline=1 and path='$path' ORDER BY priority DESC, available ";
     }
     else{
     $sql = "SELECT disk, path, available, allowed  FROM filesystems WHERE                    status='Active' and isonline=1 and path='$path' ORDER BY priority DESC, available DESC";

     }
     $ret = $self->{sqlserver}->Query($sql);
    if(not defined $ret->[0][0]){
       $sql = "SELECT disk, path, totalsize-occupied, allowed  FROM filesystems WHERE                    status='Full' and isonline=1 and path='$path' ORDER BY totalsize-occupied DESC";
      $ret = $self->{sqlserver}->Query($sql);
     }       
     foreach my $disk (@{$ret}) {
      $outputdisk = trimblanks($disk->[0]);
      $outputpath = trimblanks($disk->[1]);
      if ($outputdisk =~ /vice/) {
       $outputpath = $outputpath."/".$period;
      } else {
       $outputpath = $outputdisk.$outputpath."/".$period;
      }
      system("mkdir -p $outputpath");
      $mtime = (stat $outputpath)[9];
 #     print "$outputpath, mtime ... $mtime \n";
      if (defined $mtime) {
       if ($mtime != 0) { $gb = $disk->[2]; last;}
      }
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

      my  @periodStartTime = $self -> getActiveProductionPeriod(1);
   my $periodStartTime=2147483647;
   foreach my $periodst  (@periodStartTime){      if($periodst < $periodStartTime){
       $periodStartTime=$periodst;
     }
    }
my ($prodstart,$prodlastupd,$totaldays) = $self->getRunningDays($periodStartTime);
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

         $sql = "SELECT SUM(sizemb)  FROM jobs, ntuples
                  WHERE ntuples.jid=jobs.jid AND cid=$cid AND jobs.timestamp > $periodStartTime";
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

         $sql  = "SELECT host, mips, cputime FROM Jobs
                   WHERE host != 'host' and cid=$cid and jobs.timestamp>$periodStartTime
                    ORDER BY host";
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


sub getActiveProductionPeriod {
     my $self = shift;
     my $var=shift;
     my @period=();
     my @begin=();
     my @pid=();

     my $sql  = undef;
     my $ret  = undef;
      my $d=0;

      $sql = "SELECT NAME, BEGIN, DID  FROM ProductionSet WHERE STATUS='Active' ORDER BY DID";
      $ret = $self->{sqlserver}->Query($sql);
        foreach my $pp  (@{$ret}){
       push @period, trimblanks($pp->[0]);
       push @begin ,$pp->[1];
       push @pid,    $pp->[2];
      }
     if($var==1){ 
        return @begin;
     }
     elsif($var==2){
       return @pid;
     }
      else{
         return @period;
      }
 }

sub getActiveProductionPeriodByVersion {
     my $self = shift;
     my $vvv  = shift;

     my $sql  = undef;
     my $ret  = undef;

     my $period = $UNKNOWN;
     my $begin  = 0;
     my $pid    = -1;

      $sql = "SELECT NAME, BEGIN, DID  FROM ProductionSet WHERE STATUS='Active' and vdb='$vvv'";
      $ret = $self->{sqlserver}->Query($sql);
      if (defined $ret->[0][0]) {
       $period=trimblanks($ret->[0][0]);
       $begin = $ret->[0][1];
       $pid   = $ret->[0][2];
      }
  return $period, $begin, $pid;
 }



sub getProductionPeriods {

     my $self = shift;
     my $flag = shift; # flag = 1 - debug printout

     my $sql  = undef;
     my $ret  = undef;
     my $i    = 0;

      $sql = "SELECT DID, NAME, BEGIN, END, STATUS, VGBATCH, VDB, VOS  FROM ProductionSet ORDER by begin desc";
      $ret = $self->{sqlserver}->Query($sql);
      if (defined $ret->[0][0]) {
       foreach my $p (@{$ret}){
           $productionPeriods[$i]->{id}       = $p->[0];
           $productionPeriods[$i]->{name}     = trimblanks($p->[1]);
           $productionPeriods[$i]->{begin}    = $p->[2];
           $productionPeriods[$i]->{end}      = $p->[3];
           $productionPeriods[$i]->{status}   = trimblanks($p->[4]);
           $productionPeriods[$i]->{vgbatch}  = trimblanks($p->[5]);
           $productionPeriods[$i]->{vdb}      = trimblanks($p->[6]);
           $productionPeriods[$i]->{vos}      = trimblanks($p->[7]);
           if ($flag == 1) {
             my $ch  = sprintf(" %5d %15s %10d %10d %12s %6s %6d %6s\n",
                       $productionPeriods[$i]->{id}, $productionPeriods[$i]->{name},$productionPeriods[$i]->{begin},
                       $productionPeriods[$i]->{end}, $productionPeriods[$i]->{status},
                       $productionPeriods[$i]->{vdb}, $productionPeriods[$i]->{vgbatch}, $productionPeriods[$i]->{vos});
             print $ch;
        }
           $i++;
       }
   }
 }


sub validateDST {
     my $self    = shift;
     my $fname   = shift;  # file name
     my $nevents = shift;  # number of events
     my $ftype   = shift;  # file type : root / ntuple
     my $levent  = shift;  # last event number
     my $jobid   = shift;  # job id

     my $dtype       = undef;
     my $validatecmd = undef;
     my $ret         =  1;     # return code
     my $vcode       =  0;     # validate code

     my $time0 = time();
     $fastntCalls++;


     if($ftype eq "Ntuple") {
         $dtype = 0;
     } elsif ($ftype eq "RootFile") {
         $dtype = 1;
     }
     my $prefix="";
     if($fname=~/^\/castor/){
         #return 1,0;
        $prefix="rfio:"; 
     }
     if (defined $dtype) {
      $validatecmd = "$self->{AMSSoftwareDir}/exe/linux/fastntrd64.exe  $prefix$fname $nevents $dtype $levent";
      $vcode=system($validatecmd);
     if($fname=~/^\/castor/ and $vcode/256==134){
      sleep(5);   
      $vcode=system($validatecmd);
       #return 1,0;
     }

     if (0) {#defined $jobid and -f "$prefix$fname.jou") {
         system("sed -i \"s/\\.job.*/.job/g\" $prefix$fname.jou");
         my $jobname;
         my $sql = "SELECT jobname FROM Jobs WHERE jid=$jobid";
         my $rs = $self->{sqlserver}->Query($sql);
         if (defined $rs->[0][0]) {
             $jobname = $rs->[0][0];
             my $realjobname = `grep "^ScriptName=" $prefix$fname.jou | awk -F/ '{print \$NF}' | sed "s/\\.job.*/.job/g"`;
             chomp $realjobname;
             if ($realjobname ne $jobname) {
                 print "validateDST: $jobname in DB, while should be $realjobname, removing DB entry of jid $jobid ...\n";
                 $sql="delete from jobs where jid=$jobid";
                 $self->{sqlserver}->Update($sql);
                 $self->{sqlserver}->Commit();
                 $ret = 2;      # Incositent job name between db and root journal
             }
         }
         else {
             $ret = 2;          # no records in db, should never happen here
         }
         system("rm -f $prefix$fname.jou");
    }

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
    if($inputfile eq $outputpath){
    if ($verbose == 1 && $webmode == 0)  {
      $self->amsprint("*** docopy - ",666);
    }
        return 0;
    }
# symbolic link
        my $inputfilel=readlink($inputfile);
    if(defined $inputfilel and $inputfilel=~/^\/castor/){
        $inputfile=$inputfilel    ;
        }
    my $time0 = time();
    $copyCalls++;
    my $cmd = "cp -p -d -v $inputfile  $outputpath ";
    if($outputpath=~/^\/castor/){
        $cmd="/usr/bin/rfcp $inputfile  $outputpath ";
        if($inputfile=~/^\/castor/){
          $cmd="/usr/bin/rfrename $inputfile  $outputpath ";
      }
    }
    elsif($inputfile=~/^\// and $outputpath=~/^\//){
        my @junk=split '\/',$inputfile;
        my @jo=split '\/',$outputpath;
        if($junk[1] eq $jo[1]){
            $cmd="mv   $inputfile  $outputpath ";
        }
    }
    
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
    my $periodStartTime = shift;

    my $sql;
    my $ret;
    my $timestart = 0;
    my $lastupd   = 0;
    my $timepassed= 0;
    my $timenow   = time();
# first job timestamp
      $sql="SELECT MIN(Jobs.time), MAX(Jobs.timestamp) FROM Jobs
                WHERE Jobs.cid != $TestCiteId and Jobs.time >= $periodStartTime";
      $ret=$self->{sqlserver}->Query($sql);
      if (defined $ret->[0][0]) {
       $timestart = $ret->[0][0];
       if ($timenow > $timestart) {
        $timepassed = sprintf("%.1f",($timenow - $timestart)/60/60/24);
       }
      if (defined $ret->[0][1]) {
       $lastupd=$ret->[0][1];
      }
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

 my $totalGB = $gbDST[0]/1024/1024/1024;
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
    my $schema="amsdes";
    my $sql="select myvalue from $schema.Environment where mykey='".$key."'";
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
    my $fast=shift;
    my $vrb=shift;
    if(not defined $vrb){
        $vrb=0;
    }



        my $totalcpu=0;
        my $restcpu=0;
     my $dir="/afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/DataSets";
      if(defined $self->{AMSSoftwareDir}){
        $dir="$self->{AMSSoftwareDir}/DataSets";
      }
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

      my $sql = "SELECT  DID  FROM ProductionSet WHERE STATUS='Active' ORDER BY DID";
      my $ret = $self->{sqlserver}->Query($sql);
    my $pps=undef;
         foreach my $pp  (@{$ret}){
          if(defined $pps){
            $pps=$pps." or jobs.pid =";
          }
          else{
           $pps=" and ( jobs.pid =";
          }
         $pps=$pps." $pp->[0] ";
        }
       if(defined $pps){
        $pps=$pps." ) ";
       }
       else{
        $pps="";
       }



    my @output=();
    foreach my $file (@allfiles){
        my $newfile="$dir/$file";
        my @tmpa=();
        if(readlink $newfile or  $file =~/^\./){
         next;
        }
       my @sta = stat $newfile;
      if($sta[2]<32000){
          my $dataset={};
          $dataset->{name}=$file;
#          if($dataset->{name}=~ /adeuterons/){
#          }
#          else{
#              next;
#          }
          $dataset->{jobs}=[];
          my $totevt=0;
          my $submitted=0;
           my $completed=0;
           my $totcpu=0;
           my $rcpu=0;
       opendir THISDIR, $newfile or die "unable to open $newfile";
       my @jobs=readdir THISDIR;
       closedir THISDIR;
      $dataset->{datamc}=0;
      foreach my $job (@jobs){
          if($job=~/^data=true/){
              $dataset->{datamc}=1;
          }
     }
          push @{$self->{DataSetsT}}, $dataset;

       if($dataset->{datamc}==0){
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
           my @farray=("TOTALEVENTS","PART","PMIN","PMAX","TIMBEG","TIMEND","CPUPEREVENTPERGHZ","THREADS");
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
        $totevt+=$template->{TOTALEVENTS};
        $totcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};
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
              if($vrb){
               print "  template $dataset->{name} $template->{filename} $template->{TOTALEVENTS} $template->{CPUPEREVENTPERGHZ} total \n";
               }
                 $dataset->{did}=$ret->[0][0];
                 my $sqlsum="select sum(cputime*mips/1000/realtriggers) from Jobs where did=$ret->[0][0]  and jobname like '%$template->{filename}' and realtriggers>0 and mips>0".$pps;
              my $r2= $self->{sqlserver}->Query($sqlsum);
              my $sum=$r2->[0][0];
                  $sqlsum="select count(cputime*mips/1000/realtriggers) from Jobs where did=$ret->[0][0]  and jobname like '%$template->{filename}' and realtriggers>0 and mips>0".$pps;
              $r2= $self->{sqlserver}->Query($sqlsum);
              my $count=$r2->[0][0];
               $sqlsum="select sum(cputime*mips/1000/realtriggers*cputime*mips/1000/realtriggers) from Jobs where did=$ret->[0][0]  and jobname like '%$template->{filename}' and realtriggers>0 and mips>0".$pps;
              $r2= $self->{sqlserver}->Query($sqlsum);
              my $sum2=$r2->[0][0];
#              print " $sum $count $sum2 \n";
              if(defined $count and $count>0){
                  $sum/=$count;
                  $sum2/=$count;
                  $sum2=sqrt(abs($sum2-$sum*$sum))/sqrt($count);
              if($vrb){
                  print " Sum/Sigma  $sum $sum2 \n";
              }
                  if($sum2/$sum<0.001){
                      $sum2=$sum*0.001;
                  }
                  $template->{SUM}=$sum;
                      $template->{SUM2}=$sum2;
              }
              if($fast){
                  $sql="select sum(realtriggers) from jobs where did=$ret->[0][0] and  jobname like '%$template->{filename}' and realtriggers>0".$pps;
                  my $rtn1=$self->{sqlserver}->Query($sql);
                   my $tm=time();
                  $sql="select sum(calcevents(time+timeout-$tm,Triggers)) from jobs where did=$ret->[0][0] and  jobname like '%$template->{filename}' and realtriggers<0 and time+timeout>=$tm and timekill=0".$pps;
                  my $rtn2=$self->{sqlserver}->Query($sql);
                  $completed+=$rtn1->[0][0];
                  $template->{TOTALEVENTS}-=$rtn1->[0][0];
                  $submitted+=$rtn2->[0][0];
                  $template->{TOTALEVENTS}-=$rtn2->[0][0];
              }
              else{
                 $sql="select jid,time,triggers,timeout,realtriggers from Jobs where did=$ret->[0][0]  and jobname like '%$template->{filename}'".$pps;
                  $r2= $self->{sqlserver}->Query($sql);
                 if(defined $r2->[0][0]){
                     foreach my $job (@{$r2}){
                       if (time()-$job->[1]>$job->[3] or $job->[4]>0){
                         my $rtrig=$job->[4];
                         if(not defined $rtrig ){
                             if($vrb){
                             warn " real trig not defined or 0 for $job->[0] \n";
                             }
                             $rtrig=0;
                             $sql="select sum(LEvent-FEvent+1) from Runs where jid=$job->[0] and  (status='Completed')";
                             my $r3=$self->{sqlserver}->Query($sql);
                             if(defined $r3->[0][0]){
                                 $rtrig=$r3->[0][0];
                                 $sql="UPDATE Jobs SET realtriggers=$rtrig, timekill=0 WHERE jid=$job->[0]";
                                 $self->{sqlserver}->Update($sql);
                             }

                         }
#
# take same from ntuples
#
                         $sql="select sum(ntuples.levent-ntuples.fevent+1) from ntuples,runs where ntuples.jid=runs.jid and runs.jid=$job->[0]";
                          my $r4=$self->{sqlserver}->Query($sql);
                         my $ntevt=$r4->[0][0];
                         if(not defined $ntevt){
                             $ntevt=0;
                         }
                          if( $ntevt ne $rtrig  and $rtrig>0){
                           warn "  ntuples/run mismatch $ntevt $rtrig $job->[0] \n";
                       }
                         if(defined $rtrig and $ntevt!=$rtrig and $ntevt>0){
                           $sql="select  LEvent from Runs where jid=$job->[0]";
                           my $qq=$self->{sqlserver}->Query($sql);
                           my $levent=$qq->[0][0]+$ntevt-$rtrig;
#                           warn "$levent $ntevt $rtrig $qq->[0][0] \n";
                             $sql="UPDATE Runs SET Levent=$levent WHERE jid=$job->[0]";
                              $self->{sqlserver}->Update($sql);
                              $sql="UPDATE Jobs SET realtriggers=$ntevt WHERE jid=$job->[0]";
                              $self->{sqlserver}->Update($sql);
#                           die "qq \n";
                       }
                          $rtrig=$ntevt;
                         $completed+=$rtrig;
                         $template->{TOTALEVENTS}-=$rtrig;

                     }

                         else {
#
# subtract allocated events
                             $template->{TOTALEVENTS}-=$job->[2];
                             $submitted+=$job->[2]; 
                         }
                   }
                 }
             }
          }
           else{
               if($vrb){
               warn "Dataset $dataset->{name}  not defined, skipped \n";
               }
           }
              if($vrb){
        print "  template $dataset->{name} $template->{filename} $template->{TOTALEVENTS} rest \n";
    }
        if ( $template->{TOTALEVENTS} <0){
            $template->{TOTALEVENTS}=0;
        }
       $restcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};
        if(!$fast){
        if(defined $template->{SUM} and abs( $template->{CPUPEREVENTPERGHZ} - $template->{SUM}) >3*$template->{SUM2}){
                 warn " ***change*** $full  $template->{CPUPEREVENTPERGHZ} $template->{SUM} \n";
           open(FILE,">".$full) or die "Unable to open dataset file $full \n";
                 my $fac=$template->{CPUPEREVENTPERGHZ};
                 my $newfac=int($template->{SUM}*100000)/100000.;
                 $template->{bufi}=~ s/CPUPEREVENTPERGHZ=$fac/CPUPEREVENTPERGHZ=$newfac/;
                 print FILE $template->{bufi};
           close(FILE)
           }
         }
     
        $rcpu+=$template->{CPUPEREVENTPERGHZ}*$template->{TOTALEVENTS};
         }
    }

   }
          else{
         my $datafiles="datafiles";
         if($dataset->{datamc}/10==1){
#             $datafiles="mcfiles";
         }
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
       my @farray=("TOTALEVENTS","RUNMIN",  "SERVERNO", "RUNMAX","OPENCLOSE","CPUPEREVENTPERGHZ","QTYPE","HOST","ROOTNTUPLE","RUNLIST","RUNALIST","PRIO","TAG","EVENTMIN","THREADS");
          $template->{THREADS}=1;   
          $template->{TAG}=-1;   
          $template->{RUNMIN}=0;   
          $template->{EVENTMIN}=1;   
          $template->{RUNMAX}=4000000000;   
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
#
#  check for cite.runlist files 
#
       if(defined $template->{SERVERNO}){
           $dataset->{serverno}=$template->{SERVERNO};
       }
       if(defined $dataset->{"$self->{CCA}.runlist"}){
          $template->{RUNLIST}=$dataset->{"$self->{CCA}.runlist"};   
       }
       if(defined $dataset->{"$self->{CCA}.runalist"}){
          $template->{RUNALIST}=$dataset->{"$self->{CCA}.runalist"};   
       }
           $template->{initok}=1;
           foreach my $ent (@farray){
             if(not defined $template->{$ent} and ($ent ne "HOST" and $ent ne "ROOTNTUPLE" and $ent ne "RUNLIST"  and $ent ne "RUNALIST" and $ent ne "PRIO" and $ent ne "TAG" and $ent ne "SERVERNO" and $ent ne "THREADS")){
               $template->{initok}=undef;
             }
           }
           if($template->{OPENCLOSE}==0){
             $template->{RUNMAX}=1;
             $template->{RUNLIST}=undef;
           }
       elsif($template->{OPENCLOSE}==1 and $self->{CCT} eq 'remote'){
             $template->{RUNMAX}=1;
             $template->{RUNLIST}=undef;
         }
       elsif($template->{OPENCLOSE}==2 and $self->{CCT} eq 'local'){
             $template->{RUNMAX}=1;
             $template->{RUNLIST}=undef;
         }

       if(defined $template->{ROOTNTUPLE}  ){
           $dataset->{rootntuple}=$template->{ROOTNTUPLE};
       }
#
# get no of events
#



        my $sql="select did from DataSets where name='$dataset->{name}'";
               my $ret=$self->{sqlserver}->Query($sql);
              if( defined $ret->[0][0]){
              if($vrb){
               print "  template $dataset->{name} $template->{filename} $template->{TOTALEVENTS} $template->{CPUPEREVENTPERGHZ} total \n";
               }
                 $dataset->{did}=$ret->[0][0];
           my $qtype="";
           if($template->{QTYPE} ne ""){
               $qtype="and $datafiles.type like '$template->{QTYPE}%'";
            }

           my $runlist="";
            if(defined $template->{RUNLIST}){
                my @junk=split   ",",$template->{RUNLIST};
                $runlist=" and (";
                foreach my $r (@junk){
                    $runlist=$runlist." run=$r or ";
                }
                $runlist=$runlist." run=-1)";
            }
           my $runalist="";
            if(defined $template->{RUNALIST}){
                my @junk=split   ",",$template->{RUNALIST};
                $runalist=" and (";
                foreach my $r (@junk){
                    $runalist=$runalist." run!=$r and ";
                }
                $runalist=$runalist." run!=-1)";
            }
                 $sql="select sum($datafiles.nevents),count($datafiles.run) from $datafiles where run>=$template->{RUNMIN} and run<=$template->{RUNMAX} and status='OK'  and $datafiles.nevents>$template->{EVENTMIN} $qtype $runlist $runalist";
#                 my $sql="select sum($datafiles.nevents),count($datafiles.run) from $datafiles where run>=$template->{RUNMIN} and run<=$template->{RUNMAX} and status='OK' $qtype $runlist $runalist and run not in  (select run from dataruns,jobs where  dataruns.jid=jobs.jid  and  jobs.did='dataset->{did} and jobs.jobname like '%$template->{filename}') ";
                 my $rtn=$self->{sqlserver}->Query($sql);
                 if(defined $rtn){
                  $template->{TOTALEVENTS}=$rtn->[0][0];
                  $template->{TOTALRUNS}=$rtn->[0][1];
                 }
                 else{
                     $template->{TOTALEVENTS}=0;
                 }
               #die "  $template->{TOTALEVENTS} $template->{TOTALRUNS} $dataset->{name} $template->{filename} \n";
        $totevt+=$template->{TOTALEVENTS};
        $totcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};
        $totalcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};
              if($vrb){
               print "  template $dataset->{name} $template->{filename} $template->{TOTALEVENTS} $template->{CPUPEREVENTPERGHZ} total \n";
               }



                 my $sqlsum="select sum(cputime*mips/1000/realtriggers) from Jobs where did=$ret->[0][0]  and jobname like '%$template->{filename}' and realtriggers>0 and mips>0".$pps;
              my $r2= $self->{sqlserver}->Query($sqlsum);
              my $sum=$r2->[0][0];
                  $sqlsum="select count(cputime*mips/1000/realtriggers) from Jobs where did=$ret->[0][0]  and jobname like '%$template->{filename}' and realtriggers>0 and mips>0".$pps;
              $r2= $self->{sqlserver}->Query($sqlsum);
              my $count=$r2->[0][0];
               $sqlsum="select sum(cputime*mips/1000/realtriggers*cputime*mips/1000/realtriggers) from Jobs where did=$ret->[0][0]  and jobname like '%$template->{filename}' and realtriggers>0 and mips>0".$pps;
              $r2= $self->{sqlserver}->Query($sqlsum);
              my $sum2=$r2->[0][0];
#              print " $sum $count $sum2 \n";
              if(defined $count and $count>0){
                  $sum/=$count;
                  $sum2/=$count;
                  $sum2=sqrt(abs($sum2-$sum*$sum))/sqrt($count);
              if($vrb){
                  print " Sum/Sigma  $sum $sum2 \n";
              }
                  if($sum2/$sum<0.001){
                      $sum2=$sum*0.001;
                  }
                  $template->{SUM}=$sum;
                      $template->{SUM2}=$sum2;
              }
#
# rectify jobs realtriggers
#

              if($fast){
                  $sql="select sum($datafiles.nevents/($datafiles.levent+1-$datafiles.fevent)*jobs.realtriggers)  from jobs,dataruns,$datafiles where jobs.jid=dataruns.jid and $datafiles.run=dataruns.run and jobs.did=$ret->[0][0] and  jobs.jobname like '%$template->{filename}' and jobs.realtriggers>0".$pps;
                  my $rtn1=$self->{sqlserver}->Query($sql);
                   my $tm=time();
                  $sql="select sum($datafiles.nevents)  from jobs,dataruns,$datafiles where jobs.jid=dataruns.jid and $datafiles.run=dataruns.run and jobs.did=$ret->[0][0] and  jobs.jobname like '%$template->{filename}' and jobs.realtriggers<0 and $datafiles.status='OK'".$pps;
                  my $rtn2=$self->{sqlserver}->Query($sql);
                  $completed+=$rtn1->[0][0];
                  $submitted+=$rtn2->[0][0];
                  $template->{TOTALEVENTS}-=$rtn1->[0][0];
#                  $submitted+=$rtn2->[0][0];
#                  $template->{TOTALEVENTS}-=$rtn2->[0][0];
              }
              else{
                 $sql="select jid,time,triggers,timeout,realtriggers from Jobs where did=$ret->[0][0]  and jobname like '%$template->{filename}'".$pps;
                  $r2= $self->{sqlserver}->Query($sql);
                 if(defined $r2->[0][0]){
                     foreach my $job (@{$r2}){
                       if (time()-$job->[1]>$job->[3] or $job->[4]>0){
                         my $rtrig=$job->[4];
                         if(not defined $rtrig ){
                             if($vrb){
                             warn " real trig not defined or 0 for $job->[0] \n";
                             }
                             $rtrig=0;
                             $sql="select sum(LEvent-FEvent+1) from dataRuns where jid=$job->[0] and  (status='Completed')";
                             my $r3=$self->{sqlserver}->Query($sql);
                             if(defined $r3->[0][0]){
                                 $rtrig=$r3->[0][0];
                                 $sql="UPDATE Jobs SET realtriggers=$rtrig, timekill=0 WHERE jid=$job->[0]";
                                 $self->{sqlserver}->Update($sql);
                             }

                         }
#
# take same from ntuples
#
                         $sql="select sum(ntuples.levent-ntuples.fevent+1) from ntuples,jobs where ntuples.jid=jobs.jid and jobs.jid=$job->[0]";
                          my $r4=$self->{sqlserver}->Query($sql);
                         my $ntevt=$r4->[0][0];
                         if(not defined $ntevt){
                             $ntevt=0;
                         }
                          if( $ntevt ne $rtrig  and $rtrig>0){
                           warn "  ntuples/run mismatch $ntevt $rtrig $job->[0] \n";
                       }
                         if(defined $rtrig and $ntevt!=$rtrig and $ntevt>0){
                           $sql="select  LEvent from dataRuns where jid=$job->[0]";
                           my $qq=$self->{sqlserver}->Query($sql);
                           my $levent=$qq->[0][0]+$ntevt-$rtrig;
#                           warn "$levent $ntevt $rtrig $qq->[0][0] \n";
                             $sql="UPDATE dataRuns SET Levent=$levent WHERE jid=$job->[0]";
                              $self->{sqlserver}->Update($sql);
                              $sql="UPDATE Jobs SET realtriggers=$ntevt WHERE jid=$job->[0]";
                              $self->{sqlserver}->Update($sql);
#                           die "qq \n";
                       }
                          $rtrig=$ntevt;
                          $sql="select $datafiles.nevents/($datafiles.levent-$datafiles.fevent+1) from $datafiles,dataruns,jobs where jobs.jid=dataruns.jid and $datafiles.run=dataruns.run and jobs.jid=$job->[0]";
                          my $qq=$self->{sqlserver}->Query($sql);
                          if(defined $qq->[0][0]){
                          $rtrig*=$qq->[0][0];
                         $completed+=$rtrig;
                         $template->{TOTALEVENTS}-=$rtrig;
                         }

                     }

                         else {
#
# subtract allocated events
#                             $template->{TOTALEVENTS}-=$job->[2];
#                              $submitted+=$job->[2]; 
                         }
                   }
                 }
             }
          }
           else{
               if($vrb){
               warn "Dataset $dataset->{name}  not defined, skipped \n";
               }
           }
              if($vrb){
        print "  template $dataset->{name} $template->{filename} $template->{TOTALEVENTS} rest \n";
    }
        if ( $template->{TOTALEVENTS} <0){
            $template->{TOTALEVENTS}=0;
        }
       $restcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};
        if(!$fast){
        if(defined $template->{SUM} and abs( $template->{CPUPEREVENTPERGHZ} - $template->{SUM}) >3*$template->{SUM2}){
                 warn " ***change*** $full  $template->{CPUPEREVENTPERGHZ} $template->{SUM} \n";
           open(FILE,">".$full) or die "Unable to open dataset file $full \n";
                 my $fac=$template->{CPUPEREVENTPERGHZ};
                 my $newfac=int($template->{SUM}*100000)/100000.;
                 $template->{bufi}=~ s/CPUPEREVENTPERGHZ=$fac/CPUPEREVENTPERGHZ=$newfac/;
                 print FILE $template->{bufi};
           close(FILE)
           }
         }
     
        $rcpu+=$template->{CPUPEREVENTPERGHZ}*$template->{TOTALEVENTS};
         }
    }

   }

       push @tmpa, $dataset->{name},$totevt,int(1000*$submitted/($totevt+1))/10.,int(1000*$completed/($totevt+1))/10,int(10*$totcpu/86400,)/10.,int(10*$rcpu/86400)/10.;
       push @output, [@tmpa];
        if($vrb){
            print "  $dataset->{name} $totevt $submitted $completed \n";
         }
    }
      }
              if($vrb){
                warn "  total/rest $totalcpu  $restcpu \n";
              }
         my @tmpa=();
         $tmpa[0]="Overall";
         $tmpa[1]=0;
         $tmpa[2]=0;
         $tmpa[3]=0;
         $tmpa[4]=0;
         $tmpa[5]=0;
    push @output,[@tmpa];
return @output;

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

# get names from /afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/
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
                     Jobs.cid != $TestCiteId ";
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
   return;
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
  $self->datasetlink($filename,"/Offline/DataSetsDir",0);
  $self->datasetlink($newpath,"/Offline/DataSetsDir",1);
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
  return;
  my $self         = shift;
  my $sql          = undef;
  my $pattern      = undef;
  my $outputfile   = undef;
  my $topdir       = undef;
  my $overwrite    = 0;
  my @mkdirs       = ();

  my $whoami = getlogin();
  if ($whoami =~ 'casadmva' || $whoami =~ 'ams') {
  } elsif(defined $whoami) {
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
        $rfcp = "/usr/bin/rfcp ".$castorpath." ". $dirpath." \n";
      } else {
       if ($overwrite) {
        $rfcp = "/usr/bin/rfcp ".$castorpath." ". $dirpath." \n";
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
  } elsif(defined $whoami) {
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
        $filesize = $filesize/1024/1024;
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
       $self->datasetlink($dbpath,"/Offline/DataSetsDir",0);
  $self->datasetlink($filename,"/Offline/DataSetsDir",1);
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

sub castorPath {
 my $HelpTxt = "
     set DST path in NTuples table to castor

     -h    - print help
     -d    - db path. -d:dir
     -v    - verbose mode
     -u    - update mode

     ./castorPath.cgi -v -d:/d0dah1/MC/AMS02/2004
";
  return;
  my $self         = shift;
  my $sql          = undef;
  my $nFiles          = 0;   # files matched DST path
  my $nUpdated        = 0;   # DST path updated
  my $nError          = 0;   # error in DST path
  my $notCopied       = 0;   # file not copied to CASTOR

  my $dbpath       = undef;
  my $update       = 0;
  my $castorPrefix = '/castor/cern.ch/ams';

  my $whoami = getlogin();
  if ($whoami =~ 'ams') {
  } else {
   print  "castorPath -ERROR- script cannot be run from account : $whoami \n";
   return 1;
  }

  foreach my $chop  (@ARGV){

    if($chop =~/^-d:/){$dbpath=unpack("x3 A*",$chop);}

    if ($chop =~/^-v/) {$verbose = 1;}

    if ($chop =~/^-u/) {$update = 1;}

    if ($chop =~/^-h/) {print "$HelpTxt \n";return 1;}
   }

   if (not defined $dbpath) {print "ERROR - -p option is mandatory. Quit.\n"; return 1;}

   my $timenow = time();

   $sql = "SELECT path, castortime FROM ntuples WHERE PATH LIKE '$dbpath%'";
   my $ret=$self->{sqlserver}->Query($sql);
   if (not defined $ret->[0][0]) {
     print "DB path : $dbpath \n";
     print "$sql \n";
     print "N O T     F O U N D   I N     D B \n";
     return 1;
    } else {
     foreach my $d (@{$ret}){
         $nFiles++;
         my $dst    = $d->[0];
         my $ctime  = $d->[1];
         if ($ctime > 0) {
          my @junk = split("MC",$dst);
          if (defined $junk[1]) {
           my $castorpath = $castorPrefix."/MC".$junk[1];
           if ($verbose) {print "$castorpath \n";}
           $nUpdated++;
           if ($update == 1) {
            $sql = "UPDATE ntuples SET PATH='$castorpath', timestamp=$timenow  WHERE path='$dst'";
  $self->datasetlink($dst,"/Offline/DataSetsDir",0);
            $self->{sqlserver}->Update($sql);
           }
          } else {
             $nError++;
             print "Error - unknown DST path $dst \n";
          }
       } else {
           $notCopied++;
           print "NO UPDATE (castorflag = 0) : $dst \n ";
       }
     }
    }
     print "DSTs matched $dbpath     : $nFiles \n";
     print "DSTs records updated     : $nUpdated \n";
     print "DST path error           : $nError \n";
     print "DST NOT UPDATED            : $notCopied \n";
}


#
# Subroutines to fill datasets and jobs description tables
# scan predefined directories
#
sub updateDataSetsDescription {

 my $HelpTxt = "
     scan dataset directory and update DatasetDesc table

     -h    - print help
     -d    - directory path. -d:dir
     -v    - verbose mode
     -u    - update mode

     ./updateDataSetDesc.cgi -v -d:/offline/AMSDataDir/DataManagement/DataSets
";

  my $self         = shift;
  my $sql          = undef;
  my $topdir       = undef;

  my $update       = 0;

  my $whoami = getlogin();
  if ($whoami =~ 'ams' || $whoami =~ 'alexei') {
  } else {
   print  "restoreDST -ERROR- script cannot be run from account : $whoami \n";
   return 1;
  }

  foreach my $chop  (@ARGV){


    if($chop =~/^-d:/){$topdir=unpack("x3 A*",$chop);}

    if ($chop =~/^-v/) {$verbose = 1;}

    if ($chop =~/^-u/) {$update = 1;}

    if ($chop =~/^-h/) {print "$HelpTxt \n";return 1;}
   }

   if (not defined $topdir) {print "ERROR - -d option is mandatory. Quit.\n"; return 1;}

   my $timenow = time();

   $topdir = trimblanks($topdir);
   opendir THISDIR, $topdir or die "unable to open $topdir";
   my @datasets=readdir THISDIR;
   closedir THISDIR;

   foreach my $dataset (@datasets) {
     if($dataset =~ /^\./){
       next;
     }
     $dataset = trimblanks($dataset);
     if ($verbose) {print "Dataset : $dataset \n";}
     my $jobdir = $topdir."/".$dataset;
     opendir THISDIR, $jobdir or die "unable to open $jobdir";
     my @jobs=readdir THISDIR;
     closedir THISDIR;
     my $vr="v4.00";     
     foreach my $job (@jobs){
        if($job=~/^version=/){
             my @vrs= split '=',$job;
             $vr=$vrs[1];
             last;
         }
    }
     foreach my $job (@jobs){
     if($job =~ /\.job$/){
      if($job =~ /^\./){
        next;
      }
      $job = trimblanks($job);
      if ($verbose) {print " Job name : $job \n";}
      my $full="$topdir/$dataset/$job";
      my $buf;
      open(FILE,"<".$full) or die "Unable to open dataset file $full \n";
      read(FILE,$buf,1638400) or next;
      close FILE;
      $buf=~ s/#/C /;
      $buf =~ s/'/''/g;
      my @sbuf=split "\n",$buf;
      my $desc=$sbuf[0];
      substr($desc,0,1)=" ";
      my @sta  =stat $full;
      if ($#sta>9 and $sta[9]>1304807618){
       $desc=$dataset."/".$job." ".$desc;
      }
      $sql = "SELECT dirpath, timeupdate FROM DatasetsDesc
                WHERE dataset = '$dataset' and jobdesc = '$desc'";
     my $ret = $self->{sqlserver}->Query($sql);
     if (defined $ret->[0][0]) {
         print "****Warning dataset : $job with Job : $job Exists \n";
         print "dir path : $ret->[0][0], Insert time : $ret->[0][1] \n";
         print "****Skip $topdir/$dataset/$job \n";
     } else {
      $sql = "SELECT did from Datasets WHERE name='$dataset' and version='$vr'";
      $ret = $self->{sqlserver}->Query($sql);
      if (not defined $ret->[0][0]) {
          print "****Warning dataset : $dataset NOT FOUND in Datasets Table. Skipped \n";
      } else {
        my $did = $ret->[0][0];
        
        $sql = "INSERT INTO DatasetsDesc Values($did,'$dataset','$topdir','$job','$desc','$buf',$timenow,$timenow)";
        if ($verbose) {print "$sql \n";}
        if ($update)  {
         $self->{sqlserver}->Update($sql);
        }
      }
     }
   }
  }
 }
}

sub readDataSets() {
#
# Read and parse datasets from the predefined
# directory
# default path : /afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/DataSets
#
    my $self = shift;

    my $totalcpu=0;
    my $restcpu =0;
    my $verbose =0;
    my $topdir     ='/afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/DataSets';

 my $HelpTxt = "
     scan dataset directory and estimate CPU/Elapsed time

     -h    - print help
     -d    - directory path. -d:dir
     -v    - verbose mode

     ./readDataSets.cgi -v -d:/offline/AMSDataDir/DataManagement/DataSets
";


  foreach my $chop  (@ARGV){


    if($chop =~/^-d:/){$topdir=unpack("x3 A*",$chop);}

    if ($chop =~/^-v/) {$verbose = 1;}

    if ($chop =~/^-h/) {print "$HelpTxt \n";return 1;}
   }
    if ($verbose == 1) {print "\n \n Datasets directory : $topdir \n";}

   $td[0] = time();

   my $ndatasets  =0; # total datasets scanned
   my $nfiles  =0; # total jobs scanned


#
# get DB info :
#               All datasets
  my  $ndatasetsDB = 0;   # datasets defined in DB
  my  $njobsDB     = 0;   # jobs in DB started since $ProductionStartTime
  my  $datasetsDB  = undef;
  my  $jobsDB      = undef;

    my ($period,$periodStartTime, $periodId) = $self->getActiveProductionPeriod();
# get list of datasets (names and dids)
  my  $sql="select count(did) from DataSets";
  my $ret=$self->{sqlserver}->Query($sql);
  if (defined $ret->[0][0]) {$ndatasetsDB = $ret->[0][0];}
  if ($ndatasetsDB > 0) {
   $sql="select did, name,version from DataSets";
   $datasetsDB =$self->{sqlserver}->Query($sql);

   $sql="select count(jid) from Jobs where timestamp > $periodStartTime";
   $ret=$self->{sqlserver}->Query($sql);
   if (defined $ret->[0][0]) {
    $njobsDB = $ret->[0][0];
    $sql="select jid,time,triggers,timeout, did, jobname from Jobs
         where timestamp > $periodStartTime";
    $jobsDB= $self->{sqlserver}->Query($sql);
   }
  }


# read list of datasets dirs from $topdir
   opendir THISDIR ,$topdir or die "unable to open $topdir";
   my @allfiles= readdir THISDIR;
   closedir THISDIR;

   foreach my $file (@allfiles){
    my $newfile="$topdir/$file";
     if($file =~/^\.Trial/){
      open(FILE,"<".$newfile) or die "Unable to open dataset control file $newfile \n";
      my $buf;
      read(FILE,$buf,16384);
      close FILE;
      $self->{TrialRun}=$buf;
      last;
     }
    }



# scan all dataset dirs

   foreach my $file (@allfiles){
    my $newfile="$topdir/$file";
    if(readlink $newfile or  $file =~/^\./){
     next;
    }
    $ndatasets++;

    if ($verbose == 1) { print "DataSet : $newfile \n";}

    my @sta = stat $newfile;
    if($sta[2]<32000){
     my $dataset={};
     $dataset->{name}=$file;
     $dataset->{jobs}=[];
     $dataset->{eventstodo} = 0;
     $dataset->{eventstotal} = 0;

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
       $nfiles++;
       my $template={};
       my $full="$newfile/$job";
       my $buf;
       open(FILE,"<".$full) or die "Unable to open dataset file $full \n";
       read(FILE,$buf,1638400) or next;
       close FILE;
       $td[3] = time();
       $template->{filename}=$job;
       my @sbuf=split "\n",$buf;
       my @farray=("TOTALEVENTS","PART","PMIN","PMAX","TIMBEG","TIMEND","CPUPEREVENTPERGHZ","THREADS");
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
              my $datasetfound = 0;
              for my $i (0..$ndatasetsDB-1) {
               my $datasetsDidDB  = $datasetsDB->[$i][0];
               my $datasetsNameDB = $datasetsDB->[$i][1];
               my $datasetsVDB = $datasetsDB->[$i][2];
               if ($datasetsNameDB eq $dataset->{name} and $datasetsVDB eq $dataset->{version}) {
                 $dataset->{did}=$datasetsDidDB;
                 for my $j (0...$njobsDB-1) {
                   my $jobsJidDB     = $jobsDB->[$j][0];
                   my $jobsTimeDB    = $jobsDB->[$j][1];
                   my $jobsTrigDB    = $jobsDB->[$j][2];
                   my $jobsTimeOutDB = $jobsDB->[$j][3];
                   my $jobsDidDB     = $jobsDB->[$j][4];
                   my $jobsNameDB    = $jobsDB->[$j][5];
                   if ($jobsDidDB == $dataset->{did}) {
                     my @junk     = split '\.',$jobsNameDB;
                     my $jobname = $junk[2];
                     for my $n (3..$#junk) {
                         $jobname = $jobname.".".$junk[$n];
                     }
                         if ($jobname eq $template->{filename}) {
                            if ($jobsTimeDB - time() > $jobsTimeOutDB) {
                              $sql="select FEvent,LEvent from Runs where jid=$jobsJidDB and status='Finished'";
                              $ret=$self->{sqlserver}->Query($sql);
                              if(defined $ret->[0][0]){
                               foreach my $run (@{$ret}){
                                $template->{TOTALEVENTS}-=$run->[0];
                               }
                              }
                             } else {
#
# subtract allocated events
                             $template->{TOTALEVENTS}-=$jobsTrigDB;
                         }
                        } # $jobname eq %$template->{filename}
                  } # $jobsDidDB == $dataset->{did)
                 } # loop for all jobs started after ProductionSetStartTime
               $datasetfound = 1;
               last;
             } # $datasetsNameDB eq $dataset->{name}
           }

           if ($datasetfound == 0) {  # new dataset
               my $did = 1;
               $sql = "SELECT MAX(did) From DataSets";
               my $ret=$self->{sqlserver}->Query($sql);
               if (defined $ret->[0][0]) {
                   $did = $ret->[0][0]+1;
               }
               $dataset->{did}=$did;
               my $timestamp = time();
               $sql="insert into DataSets values($did,'$dataset->{name}',$timestamp, '$dataset->{version}',$dataset->{datamc})";
               print "SKIP : $sql \n";
           }

           $restcpu+=$template->{TOTALEVENTS}*$template->{CPUPEREVENTPERGHZ};

           if($sbuf[0] =~/^#/ && defined $template->{initok}){
            $buf=~ s/#/C /;
            $template->{filebody}=$buf;
            my $desc=$sbuf[0];
            substr($desc,0,1)=" ";
            $template->{filedesc}=$desc." Total Events Left $template->{TOTALEVENTS}";
            $dataset->{eventstodo} += $template->{TOTALEVENTS};
            $dataset->{eventstotal} += $template->{TOTALEVENTSC};
            if($template->{TOTALEVENTS}>1000){
             push @tmpa, $template;
             if ($verbose == 1) {print "$template->{filedesc} \n"; }
           }

          }
      }
     } # end jobs of jobs

    my @tmpb=sort prio @tmpa;
    foreach my $tmp (@tmpb){
     push @{$dataset->{jobs}},$tmp;
    }
   }
  } # end files of allfiles


 $tdInit = time();
 my $elapsed = $tdInit - $td[0];
 print "Datasets scanned : $ndatasets , jobs : $nfiles \n";
}


sub getProductionSetIdByDatasetId() {

     my $self = shift;
     my $did  = shift;
     my $pid  = -1;

     my $sql="SELECT productionset.did FROM productionset
                 WHERE vdb=(SELECT datasets.version FROM datasets WHERE datasets.did=$did)
                              AND productionset.status='Active'";
     my $ret=$self->{sqlserver}->Query($sql);
     if (defined $ret->[0][0]) {
      $pid = $ret->[0][0];
     }

     return $pid;
 }

sub getTestCiteId() {

     my $self = shift;
     my $tid  = undef;
     my $schema="amsdes";
     my $sql = "SELECT Cites.cid FROM $schema.Cites WHERE Cites.name = 'test'";
     my $ret=$self->{sqlserver}->Query($sql);
     if (defined $ret->[0][0]) {
      $tid = $ret->[0][0];
  }  else {
      $tid = -1;
  }
     return $tid;
 }

sub checkTiming() {
    my $self= shift;
  $webmode = -1;
   my $TTT = 0;   # nchecks
   my @T = ("ProdPeriod","Stat","Cites","Mails","Servers","Jobs","Runs","NTuples","Disks");
   $td[$TTT] = time();
   $TTT++;
     $self->  getProductionPeriods(0);
     $self -> listProductionSetPeriods();
     $td[$TTT] = time();
     $TTT++;

      $self -> listStat();
      $td[$TTT] = time();
      $TTT++;

        $self -> listCites();
        $td[$TTT] = time();
        $TTT++;

         $self -> listMails();
         $td[$TTT] = time();
         $TTT++;

          $self -> listServers();
          $td[$TTT] = time();
          $TTT++;

           $self -> listJobs();
           $td[$TTT] = time();
           $TTT++;

            $self -> listRuns();
            $td[$TTT] = time();
            $TTT++;

             $self -> listNtuples();
             $td[$TTT] = time();
             $TTT++;

              $self -> listDisks();
              $td[$TTT] = time();

     my $elapsed = $td[$TTT] - $td[0];
     print "Total time [s].... $elapsed \n";
     $TTT++;
     for my $i (0..$TTT-2) {
      $elapsed = $td[$i+1] - $td[$i];
      my $sss = sprintf("%10s : %.0f [sec]\n",$T[$i],$elapsed);
      print $sss;
     }

}


sub list_24h_html {
    my $self = shift;
    my $time24h = time() - 24*60*60;
	   my  $jid       =0;
           my  $processed=0;
           my  $processtime =0;
           my  $starttime=0;
           my  $sizegb   =0;

    my ($period, $periodStartTime, $periodId) = $self->getActiveProductionPeriod();

    print "<b><h2><A Name = \"last_24h\"> </a></h2></b> \n";
    htmlTable(" Production Statistics (24 hours)");
#    my $href=$self->{HTTPcgi}."/rc.o.cgi?queryDB04=Form";
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";

               my $sql = "SELECT COUNT(jid), MAX(time)
                            FROM jobs
                             WHERE Jobs.timestamp>$time24h";
               my $r3=$self->{sqlserver}->Query($sql);
                  print "<td align=center><b><font color=\"blue\">Jobs</font></b></td>";
                  print "<td align=center><b><font color=\"blue\" >Last Job</font></b></td>";
                  print "<td align=center><b><font color=\"blue\" >Jobs</font></b></td>";
                 print "<td align=center><b><font color=\"blue\" >Last Job</font></b></td>";
                  print "<td align=center><b><font color=\"blue\" >Size</font></b></td>";
                  print "</tr>\n";
                  print "<td align=center><b><font color=\"blue\">Started</font></b></td>";
                  print "<td align=center><b><font color=\"blue\" >Start Time</font></b></td>";
                  print "<td align=center><b><font color=\"blue\" >Processed</font></b></td>";
                  print "<td align=center><b><font color=\"blue\" >Process Time</font></b></td>";
                  print "<td align=center><b><font color=\"blue\" >GBytes</font></b></td>";
                  print "</tr>\n";
                  print_bar($bluebar,3);
             if(defined $r3->[0][0]){
		 $jid = $r3->[0][0];
                 $starttime = EpochToDDMMYYHHMMSS($r3->[0][1]);
}

		 $sql = "SELECT COUNT(Jobs.timestamp), MAX(Jobs.timestamp)
                            FROM jobs, runs
                              WHERE
                                   Jobs.timestamp > $time24h AND
                                   Runs.submit > $periodStartTime AND
                                   Runs.jid=Jobs.jid AND
                                   (Runs.status != 'Processing' OR
				   Runs.status != 'Timeout')";
                $processed = 0;
                $processtime = EpochToDDMMYYHHMMSS($time24h);
                $processtime = " no jobs since ".$processtime;
                 my $r4=$self->{sqlserver}->Query($sql);
		 if (defined $r4->[0][0]) {
                   if ($r4->[0][0] > 0) {
		     $processed = $r4->[0][0];
                     $processtime = EpochToDDMMYYHHMMSS($r4->[0][1]);
                   }
		 }
                  $sql = "SELECT SUM(sizemb)
                            FROM ntuples
			    WHERE timestamp>$time24h AND path NOT LIKE '/castor/cern.ch%'";
                  my $r5=$self->{sqlserver}->Query($sql);
                  if (defined $r5->[0][0]) {
		   $sizegb = sprintf("%.1f",$r5->[0][0]/1000);
                  }

    my $color=" black";
     print "<td align=center><b><font color=$color> $jid </font></td></b><td align=center><b><font color=$color> $starttime </font></td>
                  <td align=center><b><font color=$color> $processed </font></b></td>
                  <td align=center><b><font color=$color> $processtime </font></b></td>
                  <td align=center><b><font color=$color> $sizegb </font></b></td> \n";
           print "</font></tr>\n";



      htmlTableEnd();
      htmlTableEnd();
#      print_bar($bluebar,3);
      print "<p></p>\n";
}

sub test00 {
    my $self = shift;
      my $firstrun  = 0;
      my $lastrun   = 0;
      my $starttime = time();
      my $endtime   = 0;
      my $gigabytes = 0;
      my $nruns     = 0;
      my $ndsts     = 0;
      my $nevents   = 0;
    my $t0 = time();
    my $sql  = "SELECT Runs.Run, Jobs.JOBNAME, Runs.SUBMIT
                  FROM Runs, Jobs, runcatalog
                   WHERE Runs.Status='Completed' AND
                     jobs.pid != 0 AND Jobs.DID=112 AND
                       Jobs.JID=Runs.JID AND
                         (runs.run = runcatalog.run AND runcatalog.trtype='TriggerLVL1') ORDER BY Runs.Run";
    my $r0=$self->{sqlserver}->Query($sql);
    my $t1 = time();

     my $runold = 0;
     my $nRunsDB = 0;
     my $sqltmp = "SELECT count(run) FROM Ntuples";
     my $r1=$self->{sqlserver}->Query($sqltmp);
     if (defined $r1->[0][0]) {
      $nRunsDB = $r1->[0][0];
     }
    my $iterAv = 0;
    my $N      = 0;
    if ($nRunsDB > 0) {
      $sqltmp = "SELECT run, sizemb, nevents FROM Ntuples ORDER BY run";
      $r1=$self->{sqlserver}->Query($sqltmp);
      foreach my $run (@{$r0}) {
        my $found = 0;
        my $n = $nRunsDB/2;
#        print "$run->[0] \n";
        my $iter = 0;
        while ($found == 0 && $iter <= $nRunsDB) {
          my $rr= $r1->[$n][0];
#          print "$iter, $rr, $run->[0], $n $found\n";
          if ($rr == $run->[0]) {
           $ndsts++;
           $gigabytes = $gigabytes + $r1->[$n][1];
           $nevents   = $nevents   + $r1->[$n][2];
           if ($runold != $rr) { $nruns++; $runold=$rr;}
           $found = 1;
          } elsif ($rr > $run->[0]) {
                  $n = $n/2;

          } elsif ($rr < $run->[0]) {
                  $n = $n + $n/2;
                  if ($n > $nRunsDB-1) {$n = $nRunsDB-1;}
          }
          $iter++;
      }
#        print "$N : $found : $run->[0]...$r1->[$n][0], $iter, $n \n";
        $iterAv = $iterAv+$iter;
        $N++;
    }
  }
    my $t2 = time();
    $iterAv = $iterAv/$N;
    print "$t0, $t1, $t2, $N, $iterAv \n";
}

sub stat_adv{
my $dir=shift;
my $dte=shift;
my $level=shift;
my $mxl=shift;
my @sta=stat $dir;
if($#sta<0){
  return time();
}
if($sta[9]>$dte){
 $dte=$sta[9];
}
opendir THISDIR ,$dir or return time();
my @allfiles= readdir THISDIR;
closedir THISDIR;
foreach my $file (@allfiles){
    if ($file =~/^\./){
       next;
    }
    my $newfile=$dir."/".$file;
    if(readlink $newfile){
        next;
    }
    my @sts = stat $newfile;
    if($#sts>0 && $sts[9]>$dte){
        $dte=$sts[9];
    }
    if($level<$mxl){
       if($sts[2]<32000){
        $dte=stat_adv($newfile,$dte,$level+1,$mxl);
       }
    }
}
return $dte;
}


sub stat_adv_mc{
my $dir=shift;
my $dte=shift;
my $level=shift;
my $mxl=shift;
my $lim=shift;
my @sta=stat $dir;
if($#sta<0){
#    die "youptat $dir ";
  return time();
}
opendir THISDIR ,$dir or return time();
#opendir THISDIR ,$dir or die "!!!! $dir";
my @allfiles= readdir THISDIR;
closedir THISDIR;
foreach my $file (@allfiles){
    if ($file =~/^\./){
       next;
    }
    my $newfile=$dir."/".$file;
    if(readlink $newfile){
        next;
    }
    if(not ($file=~/\.0\./ or $file=~/\.0$/) and $level==$mxl){
       next;
   }
    my @sts = stat $newfile;
    if (scalar @sts == 0) {
        die "Failed to stat $newfile";
    }
    if($level<$mxl){
       if($#sts>0 && $sts[2]<32000){
           if($sts[9]>$lim or $level==0){
           $dte=stat_adv_mc($newfile,$dte,$level+1,$mxl,$lim);
       }
    }
   }
    if(not ($file=~/\.0\./ or $file=~/\.0$/)){
       next;
   }
   if($#sts>0 && $sts[9]>$dte){
        $dte=$sts[9];
    }
 
}
return $dte;
}



sub ChangeFS{
    my ($ref,$oldfs, $newfs, $update,$run)=  @_;
    my $rc="";
    if($run!=0){
      $rc=" and run=$run"; 
    }
    my $sql ="select path from ntuples where path like'$oldfs%'".$rc;
    my $ret=$ref->{sqlserver}->Query($sql);
    my $upd=0;
          foreach my $entry (@{$ret}) {
              my $line=$entry->[0];
              $line=~s/$oldfs/$newfs/;
              $sql="update ntuples set path='$line'  where path='$entry->[0]'";
              if($update >= 1){
  $ref->datasetlink($entry->[0],"/Offline/DataSetsDir",0);
  $ref->datasetlink($line,"/Offline/DataSetsDir",1);
                 $ref->{sqlserver}->Update($sql);
                 $upd++;
              }
          } 
    print "Total of $upd   ntuples updated \n";
    if($update>=2){
     $ref->{sqlserver}->Commit(); 
 }
 }
sub adda{
    my $sql=" select version, run, buildno from ntuples";
    my $ref=shift;
    my $ret=$ref->{sqlserver}->Query($sql);
    my $upd=0;
          foreach my $entry (@{$ret}) {
              my @sp1=split 'build',$entry->[0];
              my @sp2=split '\/',$sp1[1];
              $sql="update ntuples set buildno=$sp2[0] where run=$entry->[1]";
              if(not defined $entry->[2]){
               $ref->{sqlserver}->Update($sql);
               $upd++;
              }
              if($upd%100 ==1 ){
                  warn"  Updated:  $upd $sql \n";
              }
          }
}
use Filesys::Df;
require "syscall.ph";
sub CheckFS{
#
#  check  filesystems, update the tables accordingly if $updatedb is set
#  status: Active  :  May be used 
#          Reserved :  Set manually, may not be reseting
#          Full     :  if < 3 GB left
#         
#  isonline: 1/0 
#  return disk with highest available and online=1 and path like requested
#
           my $self=shift; 
           my $updatedb=shift;  
           my $cachetime=shift;
           my $vrb=shift;
           if(not defined $cachetime){
             $cachetime=60;
           }
           my $path=shift;
           if(not defined $path){
             $path='/MC';
           }
           my $dss=shift;
           if(not defined $dss){
               $dss=1;
           }
           my $desc="";
           if($dss){
               $desc="desc";
           }
           my $sql="select disk from filesystems where isonline=1 and status='Active' and path like '$path%' order by available $desc";
           my $ret=$self->{sqlserver}->Query($sql);
           if(time()-$cachetime < $self->dbfsupdate() and defined $ret->[0][0] and $cachetime>0){
              return $ret->[0][0];
            }
            $sql="select disk,host,status,allowed  from filesystems where path like '$path%' and (allowed>0 or status='Active' or  isonline!=1)";
            $ret=$self->{sqlserver}->Query($sql);
           foreach my $fs (@{$ret}){
#
#          check to see if it is can be readed
#
               my $stf="$fs->[0].o";
               $stf=~s/\///g;
               $stf="/tmp/$stf";
               if(defined $vrb and $vrb eq 1){
                print " $stf \n";
               }
               system("ls $fs->[0] 1>$stf 2>&1 &");
               sleep (1);
               my @stat =stat("$stf");
               unlink "$stf";
               if($stat[7]==0  or not (-d $fs->[0])){
               system("ls $fs->[0] 1>$stf 2>&1 &");
               sleep (3); 
               my @stat =stat("$stf");
               unlink "$stf";
               if($stat[7]==0 or not (-d $fs->[0])){
                $sql="update filesystems set isonline=0 where disk='$fs->[0]'";
                if(defined $vrb and $vrb==1){
                 print " $fs->[1]:$fs->[0] is not online \n";
                }
                if(defined $updatedb and $updatedb!=0){
                $self->{sqlserver}->Update($sql);
                }
                }

                next;
            }
            my $buf = "\0"x64;
            my $res=syscall(&SYS_statfs, $fs->[0], $buf);
            my ($bsize, $blocks, $bfree, $bavail, $files, $ffree, $namelen) = unpack  "x4 L6 x8 L", $buf;
            my $isonline = ($res==0);
               if($res!=0){
                   my $ref=df($fs->[0]);
                   if(defined($ref)){
                       $isonline=1;
                       $blocks=$ref->{blocks};
                       $bavail=$ref->{bavail};
                       $bfree=$ref->{bfree};
                       $bsize=1024;
                   }
               }
              if($isonline){
                my $timestamp=time();
                my $status="Active";
                my $fac=$bsize/1024/1024;
                 my $tot=$blocks*$fac;
                 if($tot>1000000000){
                   $status="Unknown";
                    $isonline=0;
                    goto offline;
                 }
                 my $occ=($blocks-$bfree)*$fac;
                 $sql = "SELECT SUM(sizemb) FROM ntuples WHERE  PATH like '$fs->[0]%' and path like '%$path%'";
                 my $sizemb=$self->{sqlserver}->Query($sql);
                 my $rused=0;
                 if(defined $sizemb->[0][0]){
                  $rused=$sizemb->[0][0];
              }
                 $sql = "SELECT SUM(sizemb) FROM datafiles WHERE  PATH like '$fs->[0]%' and path like '%$path%'";
                 $sizemb=$self->{sqlserver}->Query($sql);
                 if(defined $sizemb->[0][0]){
                  $rused+=$sizemb->[0][0];
                 } 
                 my $ava=$bavail*$fac-20000;
                 if($rused<0){
                    $rused=0;
                 }
                 my $ava1=$tot*$fs->[3]/100-$rused;
                 if($fs->[2]=~'Reserved'){
                  $status='Reserved';
                 }
                 elsif( $ava1<=0 or $ava<30000){
                  $status='Full';
                 }
                 if($ava1<$ava){
                   $ava=$ava1;
                 }
                 if($ava<0){
                   $ava=0;
                 }
                if(defined $vrb and $vrb==1){
                 print " $path status $fs->[0] $status  $ava1 $bavail*$fac  =$tot*$fs->[3]/100-$rused\n";
             }
                  $sql="update filesystems set isonline=1, totalsize=$tot, status='$status',occupied=$occ,available=$ava,timestamp=$timestamp where disk='$fs->[0]' and path like '$path%' ";
            }
             else{
offline:
              $sql="update filesystems set isonline=0 where disk='$fs->[0]'";
              if(not defined $updatedb or $updatedb==0){
#                print " $fs->[1]:$fs->[0] is not online \n";
              }
             }
             if(defined $updatedb and $updatedb>0 ){
               $self->{sqlserver}->Update($sql);
             }
            }
             $self->{sqlserver}->Commit();

          $sql="select disk from filesystems where isonline=1 and status='Active'  and path like '$path%' order by available $desc";
           $ret=$self->{sqlserver}->Query($sql);
           if(not defined $ret->[0][0]){
    my $address='vitali.choutko@cern.ch';
        foreach my $chop (@{$self->{MailT}}) {
              if($chop->{rserver}==1){
                  $address=$chop->{address};
                  last;
               }
         }
         $self->sendmailmessage($address,"FileSystems are Full or Offline",$sql);                                                 
#        try to find online and not physicall phull system
          $sql="select disk from filesystems where isonline=1 and status='Full'  and path like '$path%' order by totalsize-occupied $desc";
           $ret=$self->{sqlserver}->Query($sql);
           if(not defined $ret->[0][0]){
           $self->sendmailmessage($address,"FileSystems are Offline",$sql);
       }
    else{
#        print "Got $ret->[0][0] \n";
    }
}
           return $ret->[0][0];

       }

sub dblupdate{
my $self=shift;
            my $part=0;
             my $sql="select systimestamp from dual";
             my $got=$self->{sqlserver}->Query($sql);
             if(defined $got->[0][0]){
              my @shift= split ' ',$got->[0][0];
              my @sh1=split ':',$shift[$#shift];
              if($#sh1>0){
                $part=($sh1[0]+$sh1[1]/60.)/24.
              }
             }
             $sql="select max(date_to_unixts(modtime-$part)) as u_modtime from mods where tabname != 'fs'";
             my $rdt=$self->{sqlserver}->Query($sql);
             my $lupdate=-1;
             if(defined $rdt->[0][0]){
                 $lupdate=$rdt->[0][0];
             }
             return $lupdate;
}
sub dbfsupdate{
my $self=shift;
            my $part=0;
             my $sql="select systimestamp from dual";
             my $got=$self->{sqlserver}->Query($sql);
             if(defined $got->[0][0]){
              my @shift= split ' ',$got->[0][0];
              my @sh1=split ':',$shift[$#shift];
              if($#sh1>0){
                $part=($sh1[0]+$sh1[1]/60.)/24.
              }
             }
             $sql="select max(date_to_unixts(modtime-$part)) as u_modtime from mods where tabname = 'fs'";
             my $rdt=$self->{sqlserver}->Query($sql);
             my $lupdate=-1;
             if(defined $rdt->[0][0]){
                 $lupdate=$rdt->[0][0];
             }
             return $lupdate;
}
sub DiskIsOnline{
my $self=shift;
my $disk=shift;
$self->CheckFS(1,3600,0,'/');
my $sql="select disk from filesystems where isonline=1 and disk like '$disk'";
my $ret=$self->{sqlserver}->Query($sql);
if(defined $ret->[0][0]){
   return 1;
}
return 0;
}

sub UploadToCastor{
#
#  uploads file to castor
#  input paramaters
#  $dir:   path to local files like /disk/MC/AMS02/2005A/dir
#                                   /disk /MC  /dir are optional ones
#  $verbose   verbose if 1 
#  $update    do sql/file rm  if 1
#  $cmp       compare castor sizes with local if 1 
#  $run2p   only process run $run2p if not 0
#  $mb      max upload in mbytes
#  output par:
#   1 if ok  0 otherwise
#
# datamc==2 datafiles, not ntuples
#
    my ($self,$dir,$verbose,$update,$cmp, $run2p,$mb,$maxer,$datamc,$force,$validate)= @_;
  if(not defined $datamc){
     $datamc=0;
  }  
 if(not defined $force){
     $force=0;
  }  
 if(not defined $validate){
     $validate=1;
  }  

  my $castorPrefix = '/castor/cern.ch/ams/MC';
    my $delim="MC";
    my $adelim="HZC";
    if($datamc ==1 or $datamc==3){
    $castorPrefix = '/castor/cern.ch/ams/Data';
    $delim="";
    $adelim="MC";
    }
    my $errors=0;
    if(not defined $maxer){
        $maxer=2;
    }
  my $rfcp="/usr/bin/rfcp ";

  my $whoami = getlogin();
  if ($whoami =~ 'ams'  ) {
  } elsif(defined $whoami) {
   print  "castorPath -ERROR- script cannot be run from account : $whoami \n";
   return 0;
  }
   my $runs=0;
   my $bad_runs=0;
   my $sql ="select name,did from productionset";
   my $ret =$self->{sqlserver}->Query($sql);
    my $did=-1;
    my $name="";
   foreach my $ds (@{$ret}){
    if($dir=~/$ds->[0]/){
        $did=$ds->[1];
        $name=$ds->[0];
        last;
    }
   }
  my $name_s=$name;
    $name_s=~s/\//\\\//g;
    if($did<0 ){
        if($verbose){
            print "No dasets found for $dir \n";
        }
        return 0;
    }
    my $castor="and castortime=0";
    if($run2p!=0 and $force!=0){
        $castor="";
    }
    my $path=undef;
#     $sql = "SELECT ntuples.run from jobs,ntuples where jobs.pid=$did and jobs.jid=ntuples.jid $castor and ntuples.path like '%$dir%' and ntuples.datamc=$datamc group by run";
     $sql = "SELECT ntuples.run,ntuples.path,ntuples.jid from ntuples where    ntuples.path like '%$dir%' $castor and ntuples.datamc=$datamc and ntuples.path not like '%.hbk' order by ntuples.jid";
    if($datamc>1){
     $sql = "SELECT run,path from datafiles where path like '%$dir%'  $castor and type like '$delim%' and type not like '$adelim%' order by run";
    }     
   $ret =$self->{sqlserver}->Query($sql);
   my $uplsize=0;
   foreach my $run (@{$ret}){
    my $timenow = time();
    if($run2p ne 0 and $run2p ne $run->[0]){
      next;
    }
    if($uplsize>$mb){
      last;
    }
        my $ok=$validate?$self->CheckCRC($verbose,0,$update,$run->[0],0,$run->[1],1,$datamc):1;
        if(!$ok){
        $errors++;
            if($errors>=$maxer){
                print " Too Many Errors, Exiting \n";
                return 0;
            }
        }
    if($datamc>1){
        $sql="select path,sizemb from datafiles where  run=$run->[0] and path like '%$dir%' $castor and path not like '/castor%' and type like '$delim%'";
    }
    else {
        $sql="select path,sizemb from ntuples where  jid=$run->[2]  and path like '%$dir%' $castor and path not like '/castor%' and  path not like '%.hbk' and datamc=$datamc";
    }
      my $ret_nt =$self->{sqlserver}->Query($sql);
      my $suc=1;
      if(not defined $ret_nt->[0][0]){
        next;
      }
      else{
           
               my @junk=split $name_s,$ret_nt->[0][0];
               my @junk2=split '\/',$junk[1];
               my $dir=$castorPrefix."/$name";
               for my $i (0...$#junk2-1) {
                $dir=$dir."/$junk2[$i]";
               }
               my $sys="/usr/bin/nsmkdir -p $dir";
               my $i=system($sys);
#               if($i){
#                if($verbose){
#                 print " Unable to $sys $i \n";
#                }
#                next;
#               }
          }
      $runs++;
      foreach my $ntuple (@{$ret_nt}){
         if($ntuple->[0]=~/^#/){
          next;
         }
#
#         check fs is active
#
         my @junk=split '\/',$ntuple->[0];
         if(not $junk[1] =~ /eosams/ and not $self->DiskIsOnline("/$junk[1]")){
          print " Disk $junk[1] is Offline, skipped \n";
          $suc=0;
          last;
         }  
         @junk=split $name_s,$ntuple->[0];
         my $castor=$castorPrefix."/$name$junk[1]";
         my @junk2=split '\/',$castor;
         my @junk3=split $junk2[$#junk2],$castor; 
         if($#junk3>=0){
         my $sys="/usr/bin/nsmkdir -p $junk3[0]";
        #  system($sys);
     }

#check if file already on place
          my $found=0;       
          my $ctmp="/tmp/castor.tmp";
          my $ltmp="/tmp/local.tmp";
          my $sys="/usr/bin/nsls -l $castor 1> $ctmp 2>\&1";
          system($sys);
          $sys="ls -l $ntuple->[0] 1> $ltmp 2>\&1";
          if ($ntuple->[0] =~ /^\/eosams/) {
              $sys = "$self->{eosselect} $sys";
              $sys =~ s#/eosams/#/eos/ams/#g;
          }
          my $res=system($sys);
          if($res){
              print "problem with $sys \n";
          }
                 open(FILE,"<$ctmp") or die "Unable to open $ctmp \n";
                 my $line_c = <FILE>;
                 close FILE;
               open(FILE,"<$ltmp") or die "Unable to open $ltmp \n";
                 my $line_l = <FILE>;
                 close FILE;
                 unlink $ctmp;
                 unlink $ltmp;
                 my @size_l= split ' ',$line_l;
                 my @size_c= split ' ',$line_c;
                 if(($#size_c<4 or not $size_c[4] =~/^\d+$/) or (not $size_l[4] =~/^\d+$/) or $size_l[4] != $size_c[4]){
                     system("rfrm $castor");
                 }
         else{
             $found=1;
         }
         $sys=$rfcp.$ntuple->[0]." $castor";
         if($found){
             $sys="echo $castor ";
         }
         my $i=system($sys);
         if ($i) { #rfcp failed, retrying with xrdcp
             if($verbose){
                 print " $sys failed (retcode=$i)\n";
                 print "Now trying to copy with xrdcp...\n";
             }
             if (not defined $ENV{'STAGE_SVCCLASS'} or $ENV{'STAGE_SVCCLASS'} eq '') {
                 $ENV{'STAGE_SVCCLASS'} = 'amscdr';
             }
             my $inputfile = $ntuple->[0];
             if ($inputfile =~ /^\/eosams/) {
                 $inputfile = $self->eosLink2Xrootd($inputfile);
             }
             $sys = "/afs/cern.ch/exp/ams/Offline/root/Linux/527.icc64/bin/xrdcp -f -np -v -ODsvcClass=" . $ENV{'STAGE_SVCCLASS'} . " ".$inputfile." \"root://castorpublic.cern.ch//".$castor."\"";
             $i = system($sys);
         }
         if($i){
          $suc=0;
          if($verbose){
            print " $sys failed  (retcode=$i)\n";
          }
          last;
         }
         else{
          if($verbose){
            print " $sys copied \n";
          }
         }
         $uplsize+=$ntuple->[1];
      }
      if(!$suc){
       if($verbose){
          print " Run $run->[0]  failed \n";
       }
       $bad_runs++;
      }
      else{
#
# run successfully copied
# 

              if($update){
              foreach my $ntuple (@{$ret_nt}){
                my @junk=split $name_s,$ntuple->[0];
                my $castor=$castorPrefix."/$name$junk[1]";
                my $tms=time(); 
                $sql="update ntuples set castortime=$timenow where path='$ntuple->[0]'";
                if($datamc>1){
                $sql="update datafiles set castortime=$timenow  where path='$ntuple->[0]'";
                }
                $self->{sqlserver}->Update($sql);
              }
               my $res=$self->{sqlserver}->Commit();
               if(!$res){
                 if($verbose){
                   print " Commit failed for run $run->[0] \n";
                 }
               }
               if($verbose){
                 print " Run $run->[0]  processed. Total of $uplsize mbytes uploaded\n";
               }
              }
        } 
    }
    print "Total Of $runs Selected.  Total Of $bad_runs  Failed \n";
#
# now optionally compare castorfiles with data
#
   if($cmp>0){
      $self->CheckFS(1,300,0,'/');
      $sql="select path,run from ntuples where   path like '%$dir%' and castortime>0 and path not like '/castor%' and datamc=$datamc";
      if($datamc>1){
      $sql="select path,run from datafiles where   path like '%$dir%' and castortime>0 and path not like '/castor%' and type like '$delim%' and type not like '$adelim%' ";
      } 
      my $ret_nt =$self->{sqlserver}->Query($sql);
       foreach my $ntuple (@{$ret_nt}){
         if($ntuple->[0]=~/^#/){
          next;
         }
         my @j1=split '/',$ntuple->[0];
         $sql="select isonline from filesystems where disk='/$j1[1]'";
         my $rtn=$self->{sqlserver}->Query($sql);
         if(defined  $rtn and $rtn->[0][0]==1){
          my @junk=split $name_s,$ntuple->[0];
          my $castor=$castorPrefix."/$name$junk[1]";
          my $ctmp="/tmp/castor.tmp";
          my $ltmp="/tmp/local.tmp";
          my $sys="/usr/bin/nsls -l $castor 1> $ctmp 2>\&1";
          system($sys);
          $sys="ls -l $ntuple->[0] 1> $ltmp 2>\&1";
          if ($ntuple->[0] =~ /^\/eosams/) {
              $sys = "$self->{eosselect} $sys";
          }

          my $try=0;
again:
          my $res=system($sys);
          if($res){
              print "problem with $sys \n";
              system("sleep 1");
              if($try==0){
                  $try=1;
                  goto again;
              }
              else{
               $try=0;
              }
          }
                 open(FILE,"<$ctmp") or die "Unable to open $ctmp \n";
                 my $line_c = <FILE>;
                 close FILE;
               open(FILE,"<$ltmp") or die "Unable to open $ltmp \n";
                 my $line_l = <FILE>;
                 close FILE;
                 unlink $ctmp;
                 unlink $ltmp;
                 my @size_l= split ' ',$line_l;
                 my @size_c= split ' ',$line_c;
                 if(($#size_c<4 or not $size_c[4] =~/^\d+$/) or (not $size_l[4] =~/^\d+$/) or $size_l[4] != $size_c[4]){
                  print "Problems with $ntuple->[0] castorsize: $size_c[4] localsize: $size_l[4] $castor $line_c $line_l \n";
                  if($cmp>1){
                   $self->UploadToCastor($ntuple->[0],$verbose,$update,0,$ntuple->[1],$mb*10,$maxer,$datamc,1);
                  }
                 }

         }
      }
   }

}

sub CheckNTUnique{
#check and optionally remove identical ntuples
    my ($self,$dir,$update,$verbose,$run2p)=@_;

# check duplicated entries
my $sql2="select max(jid)-min(jid),run from ntuples where path like '%$dir%' group by run";    
    if($run2p!=0){
        $sql2=$sql2." and run=$run2p";
    }
    my @duplicated=();
        my $ret_jid =$self->{sqlserver}->Query($sql2);
          foreach my $ntuple (@{$ret_jid}){
           if(  $ntuple->[0]!=0){
             push @duplicated,$ntuple->[1];
     
           }         
          }
    my $runs="";
    if($#duplicated>=0){
        print "$#duplicated runs find \n";
        foreach my $run (@duplicated){
           print " duplicated $run \n";
           my $sql="select path,jid,run,castortime,crctime from ntuples where path like '%$dir%' and run=$run";  
           my $ret_nt =$self->{sqlserver}->Query($sql);
           my $jidmax=-1;
           my $ctimemax=-1;
           my $valid=1;
           my $pathmax="";
          foreach my $ntuple (@{$ret_nt}){
              if($ntuple->[3]==0){
                  $valid=0;
                  last;
              }
              if($ntuple->[3]>$ctimemax  ){
                  $ctimemax=$ntuple->[3];
                  $jidmax=$ntuple->[1];
                  $pathmax=$ntuple->[0];
          }
          }
           if($valid){
          foreach my $ntuple (@{$ret_nt}){
              if($ntuple->[1]!=$jidmax and $pathmax eq $ntuple->[0]){
                  print "removeing $ntuple->[1] out of $jidmax \n";
                          if($update){
                            my $sqld="delete from ntuples where jid=$ntuple->[1]";
                            my $sqlj="delete from jobs where jid=$ntuple->[1]";
                                  $self->{sqlserver}->Update($sqld);
                                  $self->{sqlserver}->Update($sqlj);
                                   $self->{sqlserver}->Commit(1);
                          }               
              }
          }
      }
}   
 
    }
    return;
    my $sql="select path,jid,run,castortime from ntuples where path like '%$dir%'";    
    if($run2p!=0){
        $sql=$sql." and run=$run2p";
    } 
       my $ret_nt =$self->{sqlserver}->Query($sql);
          foreach my $ntuple (@{$ret_nt}){
              my $path=$ntuple->[0];
              my @junk=split $dir,$path;
              my $run=$ntuple->[2];
              my $jid=$ntuple->[1];
              my $ct=$ntuple->[3];
              foreach my $nt1 (@{$ret_nt}){
                  if(($nt1->[0] ne $path) and ($nt1->[0] =~/$junk[1]/)and ($jid=$nt1->[1])){
                      if(not $path=~/^\/castor/ and ($path lt $nt1->[0] or $nt1->[0]=~/^\/castor/)){
                      if($verbose){
                          print " duplicate ntuple found $run $path $nt1->[0]\n $ct";
                      }
                          if($update){
                              my $datamc=0;
                              if($path=~/\/Data/){
                                  $datamc=1;
                              }
                              my $ret=$self->CheckCRC($verbose,0,$update,$run,0,$path,0,$datamc);
                              if($ret==1){
                              if($ct==0 and $nt1->[0]=~/^\/castor/){
                                  $sql="update ntuples set castortime=$nt1->[3] where path=$path";
                                  $self->{sqlserver}->Update($sql);
                                  print " setting castortime $nt1->[3] for $path \n";
                               } 
                                  my $sql1="delete from ntuples where path='$nt1->[0]'";
                                  my $cmd="rm $nt1->[0]";
                                  my $i=system($cmd);
                                  if($i==0 or $nt1->[0]=~/^\/castor/ ){
                                     $self->{sqlserver}->Update($sql1);
                                     $self->{sqlserver}->Commit();
                                  }
                                  else{
                                   if($verbose){
                                      print "command failed $cmd \n";
                                    }                                  
                              }
                              }
                      }                            
                      }
                  }
              }
          }


}


sub CheckCRCRaw{
#check and copy from castor raw files
    my ($self,$dir)=@_;
    my $nbad=0;
    my $ntot=0;
    my $sql=" select path, run from datafiles where path like '$dir%' and castortime>0";
        my $ret_nt =$self->{sqlserver}->Query($sql);
         
          foreach my $ntuple (@{$ret_nt}){
              my $run=$ntuple->[1];
              my $path=$ntuple->[0];
              my $ret=$self->CheckCRC(1,0,1,$run,0,$path,0,2);
              if($ret==0){
                  $nbad++;
              }
              $ntot++;
          }
    print "CheckCRCRaw-I-Total $ntot Bad $nbad \n";
}


sub CheckCRC{
#
#  check crc of files on disks
#  copy from castor if bad crc found
#  remove from disk if castor copy is eq damaged
# only if true mc otherwise do nothing /run checkcrc.py for other cases/
#  change ntuple status to 'Bad' /see above/
#  Updates catalogs
#

# in case $datamc>1  check (and only check) crc from rawfiles

# input par: 
#                                     /dir are optional ones
#  $verbose   verbose if 1
#  $irm    
#  $update    do sql/file rm  if 1
#  $run2p   only process run $run2p if not 0
#  $dir:   path to  files like /s0dat1
#  $nocastoronly  only check crc for files without castor
#  output par:
#   1 if ok  0 otherwise
#
# 
    my ($self,$verbose,$irm,$update,$run2p,$force,$dir,$nocastoronly,$datamc)= @_;
    my $rm;                                                                                
    if($irm){
        $rm="rm -i ";
    }
    else{
        $rm="rm -f ";
    }
    my $address="";
        foreach my $chop (@{$self->{MailT}}) {
              if($chop->{rserver}==1){
                  $address=$chop->{address};
                  last;
               }
         }                                                                                
  my $castorPrefix = '/castor/cern.ch/ams';
  my $delimiter='MC';                                                                              
  my $rfcp="/usr/bin/rfcp ";
  my $whoami = getlogin();
  if (not defined $whoami or $whoami =~ 'ams' or $whoami =~'casadmva') {
  } else {
   print  "castorPath -ERROR- script cannot be run from account : $whoami \n";
   return 0;
  }
    if(not defined $datamc){
        $datamc=0;
    }
    if($datamc>1){
        my $delimiter='/Data';                                                                              
        if($run2p<=0){
            warn "CheckCRc withn datamc $datamc and run $run2p not supported \n";           
        return 0;
        }
        my $sql = "select path,crc,sizemb,castortime from datafiles where run=$run2p  and path  like '$dir' ";
    if(defined $nocastoronly and $nocastoronly == 1){
       $sql=$sql. " and castortime=0 ";
    }
        my $ret_nt =$self->{sqlserver}->Query($sql);
         
      if(defined $ret_nt->[0][0]){
          foreach my $ntuple (@{$ret_nt}){
              my $crccmd    = "$self->{AMSSoftwareDir}/exe/linux/crc $ntuple->[0]  $ntuple->[1]";
                        my $rstatus   = system($crccmd);
                        $rstatus=($rstatus>>8);
                        if($rstatus!=1 ){
                         if($verbose){
                             if($verbose>1){
                                   $self->sendmailmessage($address,"crc   failed $ntuple->[0]"," ");
                               }
                          print "$ntuple->[0] crc error:  $rstatus \n";
                         }
                      
                          if($ntuple->[2]>0){
#
# copy from castor
#          
                          my $suc=1;
                          my @junk=split $delimiter,$ntuple->[0];
                          if($#junk>0){
                          my $castornt=$castorPrefix."/$delimiter".$junk[1];
                          my $sys=$rfcp." $castornt $ntuple->[0].castor";
                          my $i=system($sys);
                          if($i){
                           $suc=0;
                           if($verbose){
                            print " $sys failed for $castornt \n";
                           }
                           system("rm $ntuple->[0].castor");              
                           return 0;
                          }
                          else{
                           $crccmd    = "$self->{AMSSoftwareDir}/exe/linux/crc $ntuple->[0].castor  $ntuple->[1]";
                            $rstatus   = system($crccmd);
                            $rstatus=($rstatus>>8);
                           }
                            if($rstatus!=1){
                                $suc=0;
                              if($verbose){
                                if($verbose>1){
                                $self->sendmailmessage($address,"crc castor  failed $castornt"," ");
                               }
                               print "$castornt crc error:  $rstatus \n";
                            }
                                return 0;

                            }
                             if($update){
                                    system("mv $ntuple->[0].castor $ntuple->[0]");    
                                 }
                                    else{
                                        system("rm $ntuple->[0].castor");
                                    }
                      }
                      }
                     }
                          return $rstatus==1;
          }
      }
        else{
            return 0;
        }
    }

    if($datamc!=0){
        $delimiter='Data';
    }
    my $sql="select ntuples.path,ntuples.crc,ntuples.castortime,ntuples.jid,ntuples.fevent,ntuples.levent,ntuples.sizemb,ntuples.run from ntuples where  ntuples.path not like  '$castorPrefix%' and datamc=$datamc "; 
    if(!$force){
       $sql=$sql."  and ( ntuples.status='OK' or ntuples.status='Validated') ";
    }
    if(defined $dir){
     $sql= $sql." and ntuples.path like '%$dir%' ";
    }
    if($run2p>0){
      $sql= $sql." and ntuples.run=$run2p ";
    } 
    if(defined $nocastoronly and $nocastoronly == 1){
       $sql=$sql. " and ntuples.castortime=0 ";
    }
    $sql=$sql." order by ntuples.jid ";
      my $run=0;
      my $jid=0;
     my $runs=0;
     my $ntp=0;
     my $ntpb=0;
     my $ntpf=0;
    my $ntna=0;
      my $ret_nt =$self->{sqlserver}->Query($sql);
    my @badfs=();
    my $totmb=0;
    my $cmb=0;
      
      if(defined $ret_nt->[0][0]){
          foreach my $ntuple (@{$ret_nt}){
            $totmb+=$ntuple->[6];
          } 
          my $times=time();
          $self->CheckFS(1,60,0,"/$delimiter");
          foreach my $ntuple (@{$ret_nt}){
              if($jid ne $ntuple->[3]){
                          my @junk=split $delimiter,$ntuple->[0];
                          if($#junk<=0){
                              print "fatal problem with $delimiter for $ntuple->[0]  do nothing \n";
                              $ntna++; 
                              next;
                          }
                          my $disk=$junk[0];  
                          chop $disk;
                  $sql="select disk from filesystems where disk='$disk' and isonline=1";
                 my $ret_fs =$self->{sqlserver}->Query($sql);
                 if(not defined $ret_fs->[0][0]){
                     my $found=0;
                     foreach my $bd (@badfs){
                         if($bd eq $disk){
                             $found=1;
                             last;
                         }
                     }
                     if(not $found){
                       push @badfs,$disk;
                       if($verbose){
                          print "$disk  is not online \n";
                       }
                      }
                     $ntna++;
                     next;
                 }
                  $run=$ntuple->[7];
                  $jid=$ntuple->[3];
                  my $timec=(time()-$times);
                  my $speed=$cmb/($timec+1);
                   my $timest=($totmb-$cmb)/($speed+0.001)/3600;
                  if($speed == 0){
                   $timest=0;
                  }
                  $timec/=60;
                  $timec=int($timec*10)/10;
                  $timest=int($timest*10)/10;
                  $speed=int($speed*10)/10;
                  if($verbose){
                      my $tn=$#{$ret_nt}+1;
                      print "New run $run. $ntp ntuples processed out of $tn for $timec min $speed mb/sec out of $timest hrs \n";
                  }
                  $runs++;
              }
              $ntp++;
              $cmb+=$ntuple->[6];
              my $crccmd    = "$self->{AMSSoftwareDir}/exe/linux/crc $ntuple->[0]  $ntuple->[1]";
                        my $rstatus   = system($crccmd);
                        $rstatus=($rstatus>>8);
              my $calibhbk=0;
              if($ntuple->[0]=~'calib' and $ntuple->[0]=~/\.hbk$/){
                  $calibhbk=1;
              } 
                        if($rstatus!=1 && $calibhbk!=1){
                         if($verbose){
                                $self->sendmailmessage($address,"crc   failed $ntuple->[0]"," ");
                          print "$ntuple->[0] crc error:  $rstatus \n";
                      }
                          $ntpb++;           
                          if($ntuple->[2]>0){
#
# copy from castor
#          
                          my $suc=1;
                          my @junk=split $delimiter,$ntuple->[0];
                          if($#junk>0){
                          my $castornt=$castorPrefix."/$delimiter".$junk[1];
                          my $sys=$rfcp." $castornt $ntuple->[0].castor";
                          my $i=system($sys);
                          if($i){
                           $suc=0;
                           if($verbose){
                            print " $sys failed for $castornt \n";
                           }
                           $ntpf++;
                           system("rm $ntuple->[0].castor");              
                           next;
                          }
                          else{
                           $crccmd    = "$self->{AMSSoftwareDir}/exe/linux/crc $ntuple->[0].castor  $ntuple->[1]";
                            $rstatus   = system($crccmd);
                            $rstatus=($rstatus>>8);
                          }
                            if($rstatus!=1){
                                $suc=0;
                              if($verbose){
                                $self->sendmailmessage($address,"crc castor  failed $castornt"," ");
                               print "$castornt crc error:  $rstatus \n";
                             }
                            }
                                if($suc){
                                    if($update){
                                        system("mv $ntuple->[0].castor $ntuple->[0]");    
                                 }
                                    else{
                                        system("rm $ntuple->[0].castor");
                                    }
                                }
                                else{
#
#  castor file bad
#
                               system("rm $ntuple->[0].castor");              
                            $ntpf++;
                              $sql="update ntuples set ntuples.status='BAD' where ntuples.path='$ntuple->[0]' ";
                              $self->{sqlserver}->Update($sql);
                              $sql="update ntuples set ntuples.crcflag=0 where ntuples.path='$ntuple->[0]' ";
                              $self->{sqlserver}->Update($sql);
                              $sql="update ntuples set ntuples.path='$castornt' where ntuples.path='$ntuple->[0]' ";
  $self->datasetlink($ntuple->[0],"/Offline/DataSetsDir",0);
                              $self->{sqlserver}->Update($sql);
                              
                              $sql=" update jobs set realtriggers=realtriggers-$ntuple->[5]+$ntuple->[4]-1 where jid=$ntuple->[3] ";
                               $self->{sqlserver}->Update($sql);
                            if($update){
                                  my $fname=$ntuple->[0];
                            my $res=$self->{sqlserver}->Commit();
                            if(!$res){
                            if($verbose){
                             print " Commit failed for run $ntuple->[0] \n";                                 
                            }
                            }
                              else{
                                my $sys="$rm $fname";
                                my $i=system($sys);
                                if($i){
                                $self->sendmailmessage($address,"unable to $sys"," ");
                                }
                              }
                            }
                            else{
                             $self->{sqlserver}->Commit(0);
                            }                                             
                          }  
                      }
                          else{
                              print "fatal problem with $delimiter for $ntuple->[0]  do nothing \n";
                              $ntna++; 
                          }
                         }      
                          else{
                              $ntpf++;
#
#  no castor file found and bad crc  remove ntuple;
#                           

#
#                               modify ntuple
#                       
                              if($datamc==0 and $jid==$run){
         
               $sql="insert into ntuples_deleted select * from ntuples where ntuples.path='$ntuple->[0]'";
               $self->{sqlserver}->Update($sql);
                              my $timenow=time();
               $sql="update ntuples_deleted set timestamp=$timenow  where path='$ntuple->[0]'";
               $self->{sqlserver}->Update($sql);
                              $sql="delete from ntuples where ntuples.path='$ntuple->[0]' ";
                              $self->{sqlserver}->Update($sql);
  $self->datasetlink($ntuple->[0],"/Offline/DataSetsDir",0);
                              
                              $sql=" update jobs set realtriggers=realtriggers-$ntuple->[5]+$ntuple->[4]-1 where jid=$ntuple->[3] ";
                              $self->{sqlserver}->Update($sql);
                              $sql="select path from ntuples where jid=$ntuple->[3]";
                              my $r2=$self->{sqlserver}->Query($sql);
                              $sql="select realtriggers from jobs where jid=$ntuple->[3]";
                              my $r3=$self->{sqlserver}->Query($sql);
                              if(not defined $r2->[0] and $r3->[0][0]<1){
#                                 $sql="delete from runs where run=$ntuple->[3]";
                             }
                              $self->{sqlserver}->Update($sql);
                              if($update){
                                  my $fname=$ntuple->[0];
                            my $res=$self->{sqlserver}->Commit();
                            if(!$res){
                            if($verbose){
                             print " Commit failed for file $fname \n";                                 
                            }
                        }
                              else{
                                my $sys="$rm $fname";
                                my $i=system($sys);
                                if($i){
                                $self->sendmailmessage($address,"unable to $sys"," ");
                                }
                              }
                         }
                            else{
                             $self->{sqlserver}->Commit(0);
                            }                                             
                        }     
                          }
                     }

          }
      }
    if($verbose and $ntp>0){
        print "Total of $runs  runs, $ntp ntuples  processed. \n ";
        if($ntpb){
           print " $ntpb bad ntuples found. \n";
        }
        if($ntpf){
        print " $ntpf  ntuples could not be repared\n ";
        }
        if($ntna){
           print "$ntna ntuples could not be verified\n";
       }
    }
    if($ntpf>0){
        return 0;
    }
    else{
        return 1;
    }
}

sub CastorPrestage{
#
#  prestage castor files 
#
# input par: #  $dir:   path to castor files like MC/AMS02/2005A/dir
#                                     /dir are optional ones
#  $verbose   verbose if 1
#  $run2p   only process run $run2p if not 0
#  output par:
#   1 if ok  0 otherwise
#
# 
    my ($self,$dir,$verbose,$run2p)= @_;
    my $castorPrefix = '/castor/cern.ch/ams';
                                                                                
    my $datamc=0;
                                                                                
  my $rfcp="/usr/bin/stager_get -M ";
  my $whoami = getlogin();
  if ($whoami =~ 'ams' or $whoami =~'casadmva') {
  } elsif(defined $whoami) {
   print  "castorPath -ERROR- script cannot be run from account : $whoami \n";
   return 0;
  }
   my $sql ="select name,did from productionset";
   my $ret =$self->{sqlserver}->Query($sql);
    my $did=-1;
    my $name="";
   foreach my $ds (@{$ret}){
    if($dir=~/$ds->[0]/){
        $did=$ds->[1];
        $name=$ds->[0];
        last;
    }
   }
    my $name_s=$name;
    $name_s=~s/\//\\\//g;
    if($did<=0){
        if($verbose){
            print "No datasets found for $dir \n";
        }
           return 0;
    }
    my @filetypes=('ntuples','datafiles');
    foreach my $ntuples (@filetypes){
    if($run2p eq 0){
    $sql = "SELECT path  from $ntuples where castortime>0 and path like '%$dir%'";
}
   else{
    my $srun=" and run=$run2p";
    if($run2p=~/-/){
     my @junk=split '-',$run2p;
     $srun=" and run>=$junk[0] and run<=$junk[1] ";        
 }
if($run2p=~/,/){
$srun=" and run in ($run2p)";
}
    $sql = "SELECT path  from $ntuples where castortime>0 and path like '%$dir%' $srun";

}
    if ($ntuples=~/datafiles/){
        $sql=$sql." and type like 'SCI%' order by run desc";
    }
   $ret =$self->{sqlserver}->Query($sql);
    foreach my $path (@{$ret}){
        if($path->[0]=~/^\/castor/){
            my $cmd=$rfcp.$path->[0] ;
            my $i=system($cmd);
            if($verbose){
                if($i){
                    print "$cmd failed $i \n";
                }
            }
        }
        else{
          my @junk=split '\/',$path->[0];
          my $file=$castorPrefix;
            for my $i (2...$#junk){
                $file=$file.'/'.$junk[$i];
            }
                   my $cmd=$rfcp.$file ;
            my $i=system($cmd);
            if($verbose){
                if($i){
                    print "$cmd failed $i \n";
                }
            }
      

      }
    }
}
}

sub UploadToDisks{
#
#  Copy castor files to disks
#  Updates catalogs
#
# input par: #  $dir:   path to castor files like MC/AMS02/2005A/dir
#                                     /dir are optional ones
#  $verbose   verbose if 1
#  $update    do sql/file rm  if 1
#  $run2p   only process run $run2p if not 0
#  output par:
#   1 if ok  0 otherwise
#
# 
    my ($self,$dir,$verbose,$update,$run2p,$filesystem,$force)= @_;
                                                                                
    my $datamc=0;
    my $runsn='runs';
  my $castorPrefix = '/castor/cern.ch/ams';
    if ($dir=~'/Data/'){
     $datamc=1;
     $runsn='dataruns';
    }
                                                                                
  my $rfcp="/usr/bin/rfcp ";
  my $whoami = getlogin();
  if ($whoami =~ 'ams' or $whoami =~'casadmva') {
  } elsif(defined $whoami) {
   print  "castorPath -ERROR- script cannot be run from account : $whoami \n";
   return 0;
  }
   my $runs=0;
   my $bad_runs=0;
   my $sql ="select name,did from productionset";
   my $ret =$self->{sqlserver}->Query($sql);
    my $did=-1;
    my $name="";
   foreach my $ds (@{$ret}){
    if($dir=~/$ds->[0]/){
        $did=$ds->[1];
        $name=$ds->[0];
        last;
    }
   }
    my $name_s=$name;
    $name_s=~s/\//\\\//g;
    if($did<0){
        if($verbose){
            print "No datasets found for $dir \n";
        }
           return 0;
    }
    if($did>0){
    if($run2p eq 0){
    $sql = "SELECT $runsn.run,$runsn.jid from $runsn,jobs,ntuples where $runsn.jid=jobs.jid and jobs.pid=$did and $runsn.jid=ntuples.jid and ntuples.datamc=$datamc and ntuples.path like '$castorPrefix%$dir%'";
}
   else{ 
    $sql = "SELECT $runsn.run,$runsn.jid from $runsn,jobs,ntuples where $runsn.jid=jobs.jid and jobs.pid=$did and $runsn.jid=ntuples.jid and ntuples.datamc=$datamc and ntuples.path like '$castorPrefix%$dir%' and ntuples.run=$run2p";

}
}
    else{
} 
   $ret =$self->{sqlserver}->Query($sql);

    foreach my $run (@{$ret}){
       my $timenow = time();
    if($run2p ne 0 and $run2p ne $run->[0]){
      next;
    }
        $sql="select path,crc from ntuples where  jid=$run->[1]  and castortime>0 and path not like '/castor%'";
      my $ret_nt =$self->{sqlserver}->Query($sql);
      my $suc=1;
      $runs++;
      if(defined $ret_nt->[0][0] && !$force){
       if($verbose){
        print "  Run $run->[0] $run->[1] has non-castor ntuples, ignored \n";
       }
       $suc=0;
       $bad_runs++;  
       next;
      }
#       $sql="select path,crc from ntuples where  jid=$run->[1] and path like '$castorPrefix%$dir%' and castortime>0 "; 
       $sql="select path,crc from ntuples where  jid=$run->[1] and path like '$castorPrefix%' and castortime>0 "; 
       $ret_nt =$self->{sqlserver}->Query($sql);
       my $disk=undef;
        my $dir=undef;
       if(defined $ret_nt->[0][0]){
           if(not defined $filesystem){
               my $path='/MC';
               if($datamc==1){
                   $path='/Data';
               }
            $disk=$self->CheckFS(1,300,0,$path);
           }
           else{
               $disk=$filesystem;
           }
        if(not defined $disk){
         if($verbose){
           print "  No FileSystem Available, Exiting \n ";
         }
         last;
        }
         my $cp=$castorPrefix;
        $cp=~s/\//\\\//g;
        my @junk=split "$cp",$ret_nt->[0][0];
        $dir=$disk;
        my @j2=split '\/',$junk[1];
        for my $i (1...$#j2-1){
         $dir=$dir."/$j2[$i]";
        }
        my $i=system("mkdir -p $dir");
        if($i){
          if($verbose){
           print "  FileSystem $dir is not writeable, Exiting \n ";
         }
         last;
        }
       }
       my @files=();
       $#files=-1;
       foreach my $ntuple (@{$ret_nt}){
         # Added by bshan to stager_get and stager_qry before copying
         my $ret = system("stager_get -M $ntuple->[0] 1>/dev/null 2>&1");
         my $staged = `stager_qry -M $ntuple->[0] | grep -c STAGED 2>/dev/null`;
         chomp $staged;
         if (not $staged) {
             for (my $i = 0; $i < 3; $i++) {
                 my $stagein = `stager_qry -M $ntuple->[0] | grep -c STAGE 2>/dev/null`;
                 chomp $stagein;
                 if (not $stagein) {
                     print "stager_get failed, retrying ...\n";
                     $ret = system("stager_get -M $ntuple->[0] 1>/dev/null 2>&1");
                     sleep 1;
                     $ret = system("stager_get -M $ntuple->[0] 1>/dev/null 2>&1");
                     sleep 2;
                     $ret = system("stager_get -M $ntuple->[0] 1>/dev/null 2>&1");
                 }
             }
             $staged = `stager_qry -M $ntuple->[0] | grep -c STAGED 2>/dev/null`; # check again if it is staged
             chomp $staged;
         }
         if (not $staged) {
             print "$ntuple->[0] is not staged yet, skipping ...\n";
             last;              # will not continue if not staged
         }
         print "$ntuple->[0] staged=$staged, copying ...\n";
         # End of modification by bshan

         my @junk=split '\/',$ntuple->[0];
         my $local=$dir."/$junk[$#junk]";
         my $sys=$rfcp.$ntuple->[0]." $local";
         my $i=system($sys);
         push @files,$local; 
         if($i){
          $suc=0;
          if($verbose){
            print " $sys failed \n";
          }
          last;
         }
         else{
          my $crccmd    = "$self->{AMSSoftwareDir}/exe/linux/crc $local  $ntuple->[1]";
          my $rstatus   = system($crccmd);
          $rstatus=($rstatus>>8);
          if($rstatus!=1){
           if($verbose){
              print "$local crc error:  $rstatus \n";
           }     
           $suc=0;
           last;
          }
         }
      }
      if(!$suc){
       if($verbose){
          print " Run $run->[0]  failed \n";
       }
       $bad_runs++;
       foreach my $file (@files){
        system("rm $file");
       }
      }
      else{
#
# run successfully copied
#
              if($update){
       foreach my $ntuple (@{$ret_nt}){
         my @junk=split '\/',$ntuple->[0];
         my $local=$dir."/$junk[$#junk]";
#                $sql="update ntuples set path='$local', timestamp=$timenow where path='$ntuple->[0]'";
                $sql="update ntuples set path='$local' where path='$ntuple->[0]'";
  $self->datasetlink($ntuple->[0],"/Offline/DataSetsDir",0);
  $self->datasetlink($local,"/Offline/DataSetsDir",1);
                $self->{sqlserver}->Update($sql);
               }
               my $res=$self->{sqlserver}->Commit();
               if(!$res){
                 if($verbose){
                   print " Commit failed for run $run->[0] \n";                                 }
                 $bad_runs++;
                foreach my $file (@files){
                 system("rm $file");
                }
               }
               $#files=-1;
              }
              else{
               foreach my $file (@files){ 
                 system("rm $file");
               }
              } 
              if($verbose){
                print " Run $run->[0]  processed \n";
              }
          }
       }
}




sub UploadToDisksDataFiles{
#
#  Copy castor datafiles to disks only if corr disk is damaged
#  Updates catalogs
#
# input par: #  $dir:   path to castor files like MC/AMS02/2005A/dir
#                                     /dir are optional ones
#  $verbose   verbose if 1
#  $update    do sql/file rm  if 1
#  $run2p   only process run $run2p if not 0
#  output par:
#   0 if ok  1 otherwise
#
# 
    my ($self,$dir,$verbose,$update,$run2p,$filesystem,$force)= @_;
                                                                                
    my $datamc=0;
    my $runsn='runs';
  my $castorPrefix = '/castor/cern.ch/ams';
    if ($dir=~'/Data/'){
     $datamc=1;
    }
                                                                                
  my $rfcp="/usr/bin/rfcp ";
  my $whoami = getlogin();
  if ($whoami =~ 'ams' or $whoami =~'casadmva') {
  } elsif(defined $whoami) {
   print  "castorPath -ERROR- script cannot be run from account : $whoami \n";
   return 1;
  }
   my $runs=0;
   my $bad_runs=0;
   my $sql ="select name,did from productionset";
   my $ret_did =$self->{sqlserver}->Query($sql);
    my $did=-1;
    my $name="";
   foreach my $ds (@{$ret_did}){
    if($dir=~/$ds->[0]/){
        $did=$ds->[1];
        $name=$ds->[0];
        last;
    }
   }
    my $name_s=$name;
    $name_s=~s/\//\\\//g;
    if($did<0){
        if($verbose){
            print "No datasets found for $dir \n";
        }
           return 1;
    }
    if($did>0){
    if($run2p eq 0){
    $sql = "SELECT run  from datafiles  where  path like '%$dir%'";
    if ($datamc) {
        $sql .= " and type like 'SCI%'";
    }
    if (!$force) {
        $sql .= " and path like '/castor%'";
    }
#    print " run not defined $run2p \n";
#    return 1;
}
   else{ 
    $sql = "SELECT run  from datafiles  where   path like '%$dir%' and run=$run2p";

}
}
    else{
} 
   my $ret_runs =$self->{sqlserver}->Query($sql);
   print "Totally " . scalar @{$ret_runs} . " runs selected.\n";
   my $ret;
   my $local=undef;
   my $clocal=undef;
    foreach my $run (@{$ret_runs}){
       my $timenow = time();
    if($run2p ne 0 and $run2p ne $run->[0]){
      next;
    }
        $sql="select path,crc from datafiles where  run=$run->[0]  and castortime>0 and path not like '/castor%'";
      my $ret_nt =$self->{sqlserver}->Query($sql);
      my $suc=1;
      $runs++;
      if(defined $ret_nt->[0][0] and !$force){
       if($verbose){
        print "  Run $run->[0] $run->[1] has non-castor ntuples, ignored \n";
       }
       $suc=0;
       $bad_runs++;  
       next;
      }
       $sql="select path,crc,paths from datafiles where  run=$run->[0] and path like '%$dir%' and castortime>0 "; 
       $ret_nt =$self->{sqlserver}->Query($sql);
       my $disk=undef;
        my $dir=undef;
       if(defined $ret_nt->[0][0]){
           if(not defined $filesystem){
               my $path='/MC';
               if($datamc==1){
                   $path='/Data';
               }
            $disk=$self->CheckFS(1,300,0,$path);
           }
           else{
               $disk=$filesystem;
           }
        if(not defined $disk){
         if($verbose){
           print "  No FileSystem Available, Exiting \n ";
         }
         last;
        }
         my $cp=$castorPrefix;
        $cp=~s/\//\\\//g;
        my @junk=split "$cp",$ret_nt->[0][0];
        $dir=$disk;
        my @j2=split '\/',$junk[1];
        for my $i (1...$#j2-1){
         $dir=$dir."/$j2[$i]";
        }
        my $i=system("mkdir -p $dir");
        if($i){
          if($verbose){
           print "  FileSystem $dir is not writeable, Exiting \n ";
         }
         last;
        }
       }
       my @files=();
       $#files=-1;
       $local=undef;
       $clocal=undef;
       foreach my $ntuple (@{$ret_nt}){
         # Added by bshan to stager_get and stager_qry before copying
         my $ret = system("stager_get -M $ntuple->[0] 1>/dev/null 2>&1");
         my $staged = `stager_qry -M $ntuple->[0] | grep -c STAGED 2>/dev/null`;
         chomp $staged;
         if (not $staged) {
             for (my $i = 0; $i < 3; $i++) {
                 my $stagein = `stager_qry -M $ntuple->[0] | grep -c STAGE 2>/dev/null`;
                 chomp $stagein;
                 if (not $stagein) {
                     print "stager_get failed, retrying ...\n";
                     $ret = system("stager_get -M $ntuple->[0] 1>/dev/null 2>&1");
                     sleep 1;
                     $ret = system("stager_get -M $ntuple->[0] 1>/dev/null 2>&1");
                     sleep 2;
                     $ret = system("stager_get -M $ntuple->[0] 1>/dev/null 2>&1");
                 }
             }
             $staged = `stager_qry -M $ntuple->[0] | grep -c STAGED 2>/dev/null`; # check again if it is staged
             chomp $staged;
         }
         if (not $staged) {
             print "$ntuple->[0] is not staged yet, skipping ...\n";
             $suc = 0;
             last;              # will not continue if not staged
         }
         print "$ntuple->[0] staged=$staged, copying ...\n";
         # End of modification by bshan


         my @junk=split '\/',$ntuple->[0];
         $clocal=$ntuple->[0];         
         $local=$dir."/$junk[$#junk]";
         my $sys=$rfcp.$ntuple->[0]." $local";
         my $i=system($sys);
         push @files,$local; 
         if($i){
          $suc=0;
          if($verbose){
            print " $sys failed \n";
          }
          last;
         }
         else{
          my $crccmd    = "$self->{AMSSoftwareDir}/exe/linux/crc $local  $ntuple->[1]";
          my $rstatus   = system($crccmd);
          $rstatus=($rstatus>>8);
          if($rstatus!=1){
           if($verbose){
              print "$local crc error:  $rstatus \n";
           }     
           $suc=0;
           last;
          }
         }
      }
      if(!$suc){
       if($verbose){
          print " Run $run->[0]  failed \n";
       }
       $bad_runs++;
       foreach my $file (@files){
        system("rm $file");
       }
   }
      else{
#
# run successfully copied
#
          if ($update) {
              foreach my $ntuple (@{$ret_nt}) {
                  my @junk = split '\/', $ntuple->[0];
                  $local = $dir."/$junk[$#junk]";
                  my $sql = "update datafiles set path='$local' where path='$ntuple->[0]'";
                  $self->datasetlink($local,"/Offline/RunsDir",1,$ntuple->[2]);
                  $self->{sqlserver}->Update($sql);
              }
              my $res = $self->{sqlserver}->Commit();
              if (!$res) {
                  if ($verbose) {
                      print " Commit failed for run $run->[0] \n"; 
                  }
                  $bad_runs++;
                  foreach my $file (@files){ 
                     system("rm $file");
                  } 
              }
              $#files = -1;
          }
          else {
              foreach my $file (@files){ 
                 system("rm $file");
              } 
          }
          if($verbose){
                print " Run $run->[0]  processed \n";
          }
       }
       $ret=0;
       if(!$suc){
           $ret=1;
       }
   }
   return $ret,$local,$clocal;
}



sub MoveBetweenDisks{
#
#  Move files between disks
#  Updates catalogs
#
# input par: #  $dir:   path to  files like /disk/MC/AMS02/2005A/dir
#                                     /dir are optional ones
#  $verbose   verbose if 1
#  $update    do sql/file rm  if 1
#  $run2p   only process run $run2p if not 0
#  $newdisk  (optional)   path to new disk
#  output par:
#   1 if ok  0 otherwise
#
# 
    my ($self,$dir,$verbose,$update,$irm,$tmp,$run2p,$newd)= @_;
    my $rm;                                                                                
    if($irm){
        $rm="rm -i ";
    }
    else{
        $rm="rm ";
    }

                                                                                
  my $whoami = getlogin();
  if ($whoami =~ 'ams' or $whoami =~'casadmva') {
  } elsif(defined $whoami) {
   print  "castorPath -ERROR- script cannot be run from account : $whoami \n";
   return 0;
  }
    my @filetypes=('ntuples','datafiles');
    foreach my $ntuples (@filetypes){
   my $runs=0;
   my $bad_runs=0;
    my $sql ="select run,sum(sizemb) from $ntuples where path like '$dir%' group by run";
   my $ret =$self->{sqlserver}->Query($sql);
   foreach my $ds (@{$ret}){
       my $run=$ds->[0];
       if($run2p ne 0 and $run2p ne $run){
           next;
       }
       if($verbose){
         print "processing run $run... $ds->[1] mb \n";
       }
       $sql=" select path,castortime from $ntuples where run=$ds->[0] and path like '$dir%'";
       my $r1=$self->{sqlserver}->Query($sql);
       my $path='/MC';
       if(defined $r1->[0][0]){
           if($r1->[0][0]=~/$path/){
               $path='/MC';
           }
           else{
               $path='/Data';
           }
       }
       my $tme=time();
       my $disk=$self->CheckFS(1,60,0,$path,$tme%2);
# get production set path
       my @pat=split '\/',$dir;
       my $oldd="";
       if($#pat<1){
           $oldd=$pat[0];
       }
       else{
           $oldd=$pat[1];
       }
     if($tme%2 ==0){ 
      $sql = "SELECT disk FROM filesystems WHERE 
                   status='Active' and isonline=1 and path='$path' and disk not like '%$oldd%' ORDER BY priority DESC, available ";
     }
     else{
     $sql = "SELECT disk, path, available, allowed  FROM filesystems WHERE                    status='Active' and isonline=1 and path='$path' and disk not like '%$oldd%'ORDER BY priority DESC, available DESC";

     }
        my $r3=$self->{sqlserver}->Query($sql);
       if(defined $r3->[0][0]){
           $disk=$r3->[0][0];
       }
       else{
        die "No FileSystems Available \n"
        }
       if(defined $newd){
        $sql="select available from filesystems where disk='$newd' and path='$path' and isonline=1";
        my $r2=$self->{sqlserver}->Query($sql);
        if(defined $r2->[0][0] and $r2->[0][0]>$ds->[1]){
            $disk=$newd;
        }
      }
        my $mkdir=1;
       foreach my $ds1 (@{$r1}){
           my $file=$ds1->[0];
             
           my @junk=split '\/',$file;
           my $newfile=$disk;
           for my $j (2...$#junk){
               $newfile=$newfile.'/'.$junk[$j];
           }
           if($mkdir eq 1){
           #  first mkdir    
              my @j2=split '\/',$newfile;   
           if($#j2>0){
              $#j2-=1;
             }
             my $mk="mkdir -p ";
            foreach my $jpart (@j2){
                $mk=$mk.'/'.$jpart;
           }
           my $ret=system($mk);
           #print "$mk returns $ret \n"; 
           #$mkdir=0;
          }
           my $dd="dd if=/dev/zero of=$newfile.dd bs=1M count=10" ;
           my $iok=system($dd);
           system("rm -rf $newfile.dd");
           if($iok){
             print "Problem with disk $disk  \n";
             return;
           } 
           my $cp="cp  --preserve=timestamps,ownership  $file $newfile";
           if($file eq $newfile){
             $cp="cp  $file /dev/null";
           }
           my $i=system($cp);
           if($i ){
             if($verbose){
                 print "Problem to $cp \n";
                
             }
            system("$rm  $newfile");
            
#
# get the file from castor
#   
# only for ntuples for the moment
#
             my @jnk=split '\/',$dir;
             my $dird="";

             for my $j (2...$#jnk){
              $dird=$dird.'/'.$jnk[$j];
             }
             if($ds1->[1]>0 and $ntuples eq 'ntuples'){
                 my $ok1=$self->RemoveFromDisks($dir,$verbose,$update,$irm,$tmp,$run,1,1);
                 my $ok2=$self->UploadToDisks($dird,$verbose,$update,$run,$disk,1);
                 last;
             }
             elsif($ds1->[1]>0 and $ntuples eq 'datafiles'){
                 my $ok1=$self->RemoveFromDisksDF($dir,$verbose,$update,$irm,$tmp,$run,1,1);
                 ($i,$newfile,$file)=$self->UploadToDisksDataFiles($dird,$verbose,$update,$run,$disk,1);
             }
         }
         if(!$i and defined $newfile and $file ne $newfile ){
#
# update catalogs and remove files from old location
#
              if($update){
               my $preset=undef;
               my $softlink='/Offline/DataSetsDir';
               if($ntuples eq 'datafiles'){
                   my $retpath=$self->{sqlserver}->Query("select paths from $ntuples where path='$file'");
                   if($path eq '/Data'){
                    $preset=$retpath->[0][0];
                   }
                   $softlink='/Offline/RunsDir';
               }
               my $timenow=time();
                $sql="update $ntuples set path='$newfile' where path='$file'";
  $self->datasetlink($file,$softlink,0);
  my $netlink=$self->datasetlink($newfile,$softlink,1,$preset);
               if($ntuples eq 'datafiles'){
                   $sql="update $ntuples set path='$newfile', paths='$netlink' where path='$file'";
               }

                $self->{sqlserver}->Update($sql);
               }
               my $res=$self->{sqlserver}->Commit();
               if(!$res){
                 if($verbose){
                     print " Commit failed for $file \n";                                      }
                 system("$rm  $newfile");
                 last;
             }
              else{
#
# do final check
#
               $sql="select path from $ntuples where path='$newfile'";
               my $fchk=$self->{sqlserver}->Query($sql);
               if(not defined $fchk->[0][0]){
                 if($verbose){
                     print " Failure after succ Commit $file $newfile\n";                                      }
                 last;

               }
               else{
                  system("$rm  $file");
              }
              }
          }
       }
   }
}
             
             
}


sub RemoveFromDisks{
#
# check castor files compare them with local ones
# update ntuple table
# remove files from disks disks
# 
# input par:
#  $dir:   path to local files like /disk/MC/AMS02/2005A/dir
#                                   /disk /MC  /dir are optional ones
#  $verbose   verbose if 1 
#  $update    do sql/file rm  if 1
#  $irm       rm -i      if 1
#  $tmp  path to temporarily storage castorfiles
#  $run2p   only process run $run2p if not 0
#  $notverify do not verify cstor file exists (only to be used to remove useless data!!!!)
#  output par:
#   1 if ok  0 otherwise
#
    my ($self,$dir,$verbose,$update,$irm, $tmp,$run2p,$notverify,$force,$eos)= @_;
    my $eosexe = `grep eos= /afs/cern.ch/project/eos/installation/ams/etc/setup.sh | awk -F= '{print \$2}'`;
    chomp $eosexe;

    if(system("mkdir -p $tmp;touch $tmp/qq")){
      if($verbose){
        print " Unable to write $tmp \n "
      }
      return 0;
    } 
    if($irm){
        $irm="rm -i ";
    }
    else{
        $irm="rm ";
    }
    my $datamc=0;
    my $runsn='runs';
  my $castorPrefix = '/castor/cern.ch/ams/MC';
    if ($dir=~'Data/'){
     $castorPrefix = '/castor/cern.ch/ams/Data';
     $datamc=1;
     $runsn='dataruns';
    }
  my $rfcp="/usr/bin/rfcp ";    

  my $whoami = getlogin();
  if ($whoami =~ 'ams' ) {
  } elsif(defined $whoami) {
   print  "castorPath -ERROR- script cannot be run from account : $whoami \n";
   return 0;
  }
   my $runs=0;
   my $bad_runs=0;
   my $sql ="select name,did from productionset";
   my $ret =$self->{sqlserver}->Query($sql);
    my $did=-1;
    my $name="";
   foreach my $ds (@{$ret}){
    if($dir=~/$ds->[0]/){
        $did=$ds->[1];
        $name=$ds->[0];
        last;
    }
   } 
    my $name_s=$name;
    $name_s=~s/\//\\\//g;
    if($did<0){
        if($verbose){
            print "No datasets found for $dir \n";
        }
        if($run2p eq 0){
           return 0;
       }
    }
 if($did>0){   
    $sql = "SELECT $runsn.run,$runsn.jid from $runsn,jobs,ntuples where $runsn.jid=jobs.jid and jobs.pid=$did and $runsn.jid=ntuples.jid and ntuples.datamc=$datamc and ntuples.path like '%$dir/%'";
}
    else{
        printf "Unable to find Dataset $dir \n";
        return 0;
    }
   $ret =$self->{sqlserver}->Query($sql);
    if($#$ret<0 and $runsn eq 'runs'){
       $runsn='dataruns';
    $sql = "SELECT $runsn.run,$runsn.jid from $runsn,jobs,ntuples where $runsn.jid=jobs.jid and jobs.pid=$did and $runsn.jid=ntuples.jid and ntuples.datamc=$datamc and ntuples.path like '%$dir%'";
   $ret =$self->{sqlserver}->Query($sql);
    if($#$ret<0 and $runsn eq 'dataruns'){
    $sql = "SELECT ntuples.run,ntuples.jid from jobs,ntuples where ntuples.jid=jobs.jid and jobs.pid=$did and   ntuples.datamc=$datamc and ntuples.path like '%$dir%'";
   $ret =$self->{sqlserver}->Query($sql);

    }
    }
    foreach my $run (@{$ret}){
       my $timenow = time();
    if($run2p > 0 and $run2p ne $run->[0]){
      next;
    }
    if($run2p < 0 and -$run2p < $run->[0]){
      next;
    }
      my $cond = "path not like '/castor%'";
      if ($eos) {
          $cond = 'eostime > 0';
      }

        $sql="select path,crc from ntuples where  jid=$run->[1] and path like '%$dir%' and castortime>0 and $cond and datamc=$datamc";
      my $ret_nt =$self->{sqlserver}->Query($sql);
      my $suc=1;
      if(not defined $ret_nt->[0][0]){
        next;
      }
      $runs++;
      my $sys="$irm $tmp/*";
      my $i=system($sys);
      foreach my $ntuple (@{$ret_nt}){
         if($ntuple->[0]=~/^#/ ){
          next;
         }
         my $castor;
         if ($ntuple->[0] =~ /\/castor\//) {
             $castor = $ntuple->[0];
         }
         else {
             my @junk=split $name_s,$ntuple->[0];
             $castor=$castorPrefix."/$name$junk[1]";
         }
         my @junk2=split /\//,$ntuple->[0];
         my $eospath = $castor;
         $eospath =~ s#/castor/cern.ch#/eos#g;
         $sys=$rfcp.$castor." $tmp";
         if($notverify){
            my $ctmp="/tmp/castor.tmp";
           $sys="/usr/bin/nsls -l $castor 1> $ctmp 2>\&1";
           system($sys);
           my $ltmp="/tmp/local.tmp";
           $sys="ls -l $ntuple->[0] 1> $ltmp 2>\&1";
           if ($eos) {
                $sys = "$eosexe ls -l $eospath 1> $ltmp 2>\&1";
           }
          $i=system($sys);
            if(not $i){
                 open(FILE,"<$ctmp") or die "Unable to open $ctmp \n";
                 my $line_c = <FILE>;
                 close FILE;
                 open(FILE,"<$ltmp") or die "Unable to open $ltmp \n";
                 my $line_l = <FILE>;
                 close FILE;
                 unlink $ctmp;
                 unlink $ltmp;
                 my @size_l= split ' ',$line_l;
                 my @size_c= split ' ',$line_c;
                 if(($#size_c<4 or not $size_c[4] =~/^\d+$/) or (not $size_l[4] =~/^\d+$/) or $size_l[4] != $size_c[4]){
                  print "Problems with $ntuple->[0] castorsize: $size_c[4] localsize: $size_l[4] $castor $line_c $line_l \n";
                 $i=2;       
              }
             }
          }
         else{
         # Added by bshan to stager_get and stager_qry before copying
         my $ret = system("stager_get -M $castor 1>/dev/null 2>&1");
         my $staged = `stager_qry -M $castor | grep -c STAGED 2>/dev/null`;
         chomp $staged;
         if (not $staged) {
             for (my $i = 0; $i < 3; $i++) {
                 my $stagein = `stager_qry -M $castor | grep -c STAGE 2>/dev/null`;
                 chomp $stagein;
                 if (not $stagein) {
                     print "stager_get failed, retrying ...\n";
                     $ret = system("stager_get -M $castor 1>/dev/null 2>&1");
                     sleep 1;
                     $ret = system("stager_get -M $castor 1>/dev/null 2>&1");
                     sleep 2;
                     $ret = system("stager_get -M $castor 1>/dev/null 2>&1");
                 }
             }
             $staged = `stager_qry -M $castor | grep -c STAGED 2>/dev/null`; # check again if it is staged
             chomp $staged;
         }
         if (not $staged) {
             print "$castor is not staged yet, skipping ...\n";
             last;              # will not continue if not staged
         }
         print "$castor staged=$staged, copying ...\n";
         # End of modification by bshan

          $i=system($sys);
         }
         if($i){
          $suc=0;
          if($verbose){
            print " $sys failed \n";
          }
          last;     
         }
         else{
          my $crccmd    = "$self->{AMSSoftwareDir}/exe/linux/crc $tmp/$junk2[$#junk2]  $ntuple->[1]";
          my $rstatus   = system($crccmd);
          $rstatus=($rstatus>>8);
          if($notverify){
           $rstatus=1;
          }
          if($rstatus!=1){
           if($verbose){
              print "$castor crc error:  $rstatus \n";
           }  
           $suc=0;
           last;
          }
         }
      }
      if(!$suc && !$force){
       if($verbose){
          print " Run $run->[0]  failed \n";
       }
       $bad_runs++;
      }
      else{
#
# run successfully copied
# 
              
              if($update){
               if (not $eos) {
               $sql="insert into ntuples_deleted select * from ntuples where ntuples.jid=$run->[1]";
               $self->{sqlserver}->Update($sql);
#               $sql="update ntuples_deleted set timestamp=$timenow where jid=$run->[1]";
#               $self->{sqlserver}->Update($sql);
               }
               foreach my $ntuple (@{$ret_nt}){
                   my $castor=$castorPrefix;
                   if ($ntuple->[0] =~ /\/castor\//) {
                       $castor = $ntuple->[0];
                   }
                   else {
                   if($did<0){
                       my @junk=split '\/',$ntuple->[0];
                       for my $j (3...$#junk){
                        $castor=$castor."/$junk[$j]";
                    }
                   }
                   else{
                    my @junk=split $name_s,$ntuple->[0];
                     $castor=$castor."/$name$junk[1]";
                   }
                   }
                   if ($eos) {
                       $sql="update ntuples set eostime=0 where path='$ntuple->[0]'";
                       $self->{sqlserver}->Update($sql);
                       $self->{sqlserver}->Commit();
                   }
                   else {
                $sql="update ntuples set path='$castor', timestamp=$timenow where path='$ntuple->[0]'";
                $self->{sqlserver}->Update($sql);
                $self->datasetlink($ntuple->[0],"/Offline/DataSetsDir",0);
                   }
                
                if ($eos) {
                    my $eospath = $castor;
                    $eospath =~ s#/castor/cern.ch#/eos#g;
                    $sys = "$eosexe $irm $eospath";
                }
                else {
                $sys=$irm." $ntuple->[0]";
                if($ntuple->[0]=~/^#/){
                 $sys="sleep 1";
                 my $newp=$ntuple->[0];
                 $newp=~s/^# //;
#                 $sql="update ntuples_deleted set path='$newp' where path='$ntuple->[0]'";
 
                }
                }
                $i=system($sys);
                if(!$i){
                  $self->{sqlserver}->Update($sql);
                  $self->{sqlserver}->Commit();
                }
                else{
                 if($verbose){
                  print " Unable to $sys \n";
                 }
                 $bad_runs++;
                 last;
                }

               }
               my $res=$self->{sqlserver}->Commit();
               if(!$res){
                 if($verbose){
                   print " Commit failed for run $run->[0] \n";
                 }
               }
              }
      if($verbose){
          print " Run $run->[0]  processed \n";
       }
     }
    }
    my $sys="$irm $tmp/*";
    system($sys);
    print "Total Of $runs Selected.  Total Of $bad_runs  Failed \n";

}




sub RemoveFromDisksDF{
#
# check castor files compare them with local ones
# update datafiles table
# remove files from disks disks
# 
# input par:
#  $dir:   path to local files like /disk/MC/AMS02/2005A/dir
#                                   /disk /MC  /dir are optional ones
#  $verbose   verbose if 1 
#  $update    do sql/file rm  if 1
#  $irm       rm -i      if 1
#  $tmp  path to temporarily storage castorfiles
#  $run2p   only process run $run2p if not 0
#  $notverify do not verify cstor file exists (only to be used to remove useless data!!!!)
#  output par:
#   1 if ok  0 otherwise
#
    my ($self,$dir,$verbose,$update,$irm, $tmp,$run2p,$notverify,$force)= @_;
    if(system("mkdir -p $tmp;touch $tmp/qq")){
      if($verbose){
        print " Unable to write $tmp \n "
      }
      return 0;
    } 
    if($irm){
        $irm="rm -i ";
    }
    else{
        $irm="rm ";
    }
    my $datamc=0;
    my $runsn='runs';
  my $castorPrefix = '/castor/cern.ch/ams/MC';
    if ($dir=~'/Data/'){
     $castorPrefix = '/castor/cern.ch/ams/Data';
     $datamc=1;
     $runsn='dataruns';
    }
  my $rfcp="/usr/bin/rfcp ";    

  my $whoami = getlogin();
  if ($whoami =~ 'ams' ) {
  } elsif(defined $whoami) {
   print  "castorPath -ERROR- script cannot be run from account : $whoami \n";
   return 0;
  }
   my $runs=0;
   my $bad_runs=0;
   my $sql ="select name,did from productionset";
   my $ret =$self->{sqlserver}->Query($sql);
    my $did=-1;
    my $name="";
   foreach my $ds (@{$ret}){
    if($dir=~/$ds->[0]/){
        $did=$ds->[1];
        $name=$ds->[0];
        last;
    }
   } 
    my $name_s=$name;
    $name_s=~s/\//\\\//g;
    if($did<0){
        if($verbose){
            print "No datasets found for $dir \n";
        }
        if($run2p eq 0){
           return 0;
       }
    }
 if($did>0){   
    $sql = "SELECT run from datafiles where  path like '%$dir%'";
if($run2p){
    $sql = "SELECT run from datafiles where  path like '%$dir%'and run=$run2p";
}

}
    else{
        printf "Unable to find Dataset $dir \n";
        return 0;
    }
   $ret =$self->{sqlserver}->Query($sql);
    foreach my $run (@{$ret}){
       my $timenow = time();
    if($run2p ne 0 and $run2p ne $run->[0]){
      next;
    }
        $sql="select path,crc,paths from datafiles where  run=$run->[0] and path like '%$dir%' and castortime>0 and path not like '/castor%' ";
      my $ret_nt =$self->{sqlserver}->Query($sql);
      my $suc=1;
      if(not defined $ret_nt->[0][0]){
        next;
      }
      $runs++;
      my $sys="$irm $tmp/*";
      my $i=system($sys);
      foreach my $ntuple (@{$ret_nt}){
         if($ntuple->[0]=~/^#/){
          next;
         }
         my @junk=split $name_s,$ntuple->[0];
         my $castor=$castorPrefix."/$name$junk[1]";
         my @junk2=split /\//,$ntuple->[0];
         $sys=$rfcp.$castor." $tmp";
         if($notverify){
            my $ctmp="/tmp/castor.tmp";
           $sys="/usr/bin/nsls -l $castor 1> $ctmp 2>\&1";
           system($sys);
           my $ltmp="/tmp/local.tmp";
           $sys="ls -l $ntuple->[0] 1> $ltmp 2>\&1";
          $i=system($sys);
            if(not $i){
                 open(FILE,"<$ctmp") or die "Unable to open $ctmp \n";
                 my $line_c = <FILE>;
                 close FILE;
                 open(FILE,"<$ltmp") or die "Unable to open $ltmp \n";
                 my $line_l = <FILE>;
                 close FILE;
                 unlink $ctmp;
                 unlink $ltmp;
                 my @size_l= split ' ',$line_l;
                 my @size_c= split ' ',$line_c;
                 if(($#size_c<4 or not $size_c[4] =~/^\d+$/) or (not $size_l[4] =~/^\d+$/) or $size_l[4] != $size_c[4]){
                  print "Problems with $ntuple->[0] castorsize: $size_c[4] localsize: $size_l[4] $castor $line_c $line_l \n";
                 $i=2;       
              }
             }
          }
         else{
          $i=system($sys);
         }
         if($i){
          $suc=0;
          if($verbose){
            print " $sys failed \n";
          }
          last;     
         }
         else{
          my $crccmd    = "$self->{AMSSoftwareDir}/exe/linux/crc $tmp/$junk2[$#junk2]  $ntuple->[1]";
          my $rstatus   = system($crccmd);
          $rstatus=($rstatus>>8);
          if($notverify){
           $rstatus=1;
          }
          if($rstatus!=1){
           if($verbose){
              print "$castor crc error:  $rstatus \n";
           }  
           $suc=0;
           last;
          }
         }
      }
      if(!$suc && !$force){
       if($verbose){
          print " Run $run->[0]  failed \n";
       }
       $bad_runs++;
      }
      else{
#
# run successfully copied
# 
              
              if($update){
               foreach my $ntuple (@{$ret_nt}){
                   my $castor=$castorPrefix;
                   if($did<0){
                       my @junk=split '\/',$ntuple->[0];
                       for my $j (3...$#junk){
                        $castor=$castor."/$junk[$j]";
                    }
                   }
                   else{
                    my @junk=split $name_s,$ntuple->[0];
                     $castor=$castor."/$name$junk[1]";
                   }
                $sql="update datafiles set path='$castor', timestamp=$timenow where path='$ntuple->[0]'";
                $self->{sqlserver}->Update($sql);
                $self->datasetlink($ntuple->[0],"/Offline/RunsDir",0,$ntuple->[2]);
                
                $sys=$irm." $ntuple->[0]";
                if($ntuple->[0]=~/^#/){
                 $sys="sleep 1";
                 my $newp=$ntuple->[0];
                 $newp=~s/^# //;
 
                }
                $i=system($sys);
                if(!$i){
                  $self->{sqlserver}->Update($sql);
                }
                else{
                 if($verbose){
                  print " Unable to $sys \n";
                 }
                 $bad_runs++;
                 last;
                }

               }
               my $res=$self->{sqlserver}->Commit();
               if(!$res){
                 if($verbose){
                   print " Commit failed for run $run->[0] \n";
                 }
               }
              }
      if($verbose){
          print " Run $run->[0]  processed \n";
       }
     }
    }
    my $sys="$irm $tmp/*";
    system($sys);
    print "Total Of $runs Selected.  Total Of $bad_runs  Failed \n";

}





sub GroupRuns{
#
# Ensures all ntuples from single run reside in the same directory
#
#  input parameters
#  $verbose  
#  $update  do sql/mv
#  $run2p   update only $run2p if not 0 
#
#
    my ($self,$verbose,$update,$run2p)= @_;
    my $sql= "select path,sizemb,run from ntuples where path not like '/castor%' and castortime>0 order by run";
    my $ret_nt=$self->{sqlserver}->Query($sql);
    my $runo=-1;
    my @path=();
     my @disk=();
      my $tots=0;
      print " $#{$ret_nt} ntuples to check \n";
    foreach my $ntuple (@{$ret_nt}){
     if($runo ne $ntuple->[2]){
#     new run 
      if($#path>0){
       if($verbose){
        print "found splitted run $runo \n";
       }
       my $ind=-1;
       my $maxa=$tots+1;
       for my $i (0...$#disk){
         $sql="select available from filesystems where disk='$disk[$i]' and isonline=1";
         my $ret_fs=$self->{sqlserver}->Query($sql);
         if(not defined $ret_fs->[0][0]){
          if($verbose){
           print "disk $disk[$i] is offline \n";
          }
           $ind=-1;
           last;
         }
     
         if(defined $ret_fs->[0][0] and $ret_fs->[0][0]>$maxa){
          $maxa=$ret_fs->[0][0];
          $ind=$i;
         }
       }
       if($ind>=0){
        my $pat=$path[$ind];
        $pat=~s/\//\\\//g;
        $sql="select path from ntuples where run=$runo";
        my $ret=$self->{sqlserver}->Query($sql);
        foreach my $file (@{$ret}){
         if($file->[0]=~/$pat/){
         }
         else{
          my @junk=split '\/',$file->[0];
          my $newfile=$path[$ind]."/$junk[$#junk]";
          my $sys="mv $file->[0] $newfile";
           if($update){
            my $i=system($sys);
            if(!$i){
             $sql="update ntuples set path='$newfile' where path='$file->[0]'";
  $self->datasetlink($file->[0],"/Offline/DataSetsDir",0);
  $self->datasetlink($newfile,"/Offline/DataSetsDir",1);
             $self->{sqlserver}->Update($sql);
            }
            elsif($verbose){
             print " Unable to $sys \n";
            }
           }

         }
        }
       }
       elsif($verbose){
         print " Unable to move run $runo because disk space is too low $maxa \n";
       }
      } 
      $#path=-1;
      $#disk=-1;
      $tots=0;
     }
     if($run2p ne 0 and $ntuple->[2] ne $run2p){
      next;
     }
     my @junk=split '\/',$ntuple->[0];
     my $dir="";
     for my $i (1...$#junk-1){
      $dir=$dir."/$junk[$i]";
     }
      my $found=0;
     foreach my $p (@path){
      if($dir eq $p){
        $found=1;
        last;
       }
     }
     if(!$found){
      push @path,$dir;
      push @disk,"/$junk[1]";
      $tots+=$ntuple->[1];
     }
     $runo=$ntuple->[2];
   } 
}

sub buildnum_string {
my @string = @_;
my $count = 0;
my $it = 0;
my $word= "";
my $tab = "";
my $buildnum_min = 100000000;
my $buildnum_max = 0;
my $string = "@string";
my @tab = split (//, $string);
my @result = ();

foreach my $char (@tab) {
    if ($char =~/[\d]/) {
        $word = $word.$char;
        $count++;
        if (($it == length($string)-1) and ($count != 0)) {
            push(@result, $word);
        }
    } else {
        if ($count != 0) {
            push(@result, $word);
            $word = "";
            $count = 0;
        } else {
# nothing
        }
}
    $it++;
    }

#print "numbers:\n";
foreach my $thing (@result) {
#    print "$thing\n";
    if ($thing > $buildnum_max) {
        $buildnum_max = $thing;
    }
    if ($thing < $buildnum_min) {
        $buildnum_min = $thing;
    }

}
return  $buildnum_min, $buildnum_max;
}
##############################################

sub TestPerl{
#test arb perl text

    my $self=shift;

    my $jid=shift;
        my     $sql="insert into datafiles select * from mcfiles where mcfiles.run=$jid";
     $self->{sqlserver}->Update($sql);
            $self->{sqlserver}->Commit();

   my $ret=$self->{sqlserver}->Query(" select run from mcfiles");
   foreach my $mcfile (@{$ret}){
    my $jid=$mcfile->[0];   
}
}
sub datasetlink{
    my $self=shift;
    my $path=shift;
    my $sdir=shift;
    my $crdel=shift;
    my $predefined=shift;
#split path
           my @junk=split '\/',$path;
           my $newfile=$sdir;
           my $file=$sdir;
           my $newdir=$sdir; 
           my $dir=$sdir; 
           for my $j (2...$#junk){
               if($junk[$j]=~/\.root$/){
                   my @rmrun=split '\.',$junk[$j];
                   my $isdir=int(int($rmrun[0])/1000000);
                   $isdir=$isdir*1000000;
                   $newdir=$newdir."/$isdir";
                   $newfile=$newfile."/$isdir";
               }
               $file=$file.'/'.$junk[$j];
               $newfile=$newfile.'/'.$junk[$j];
               if($j<$#junk){
                   $newdir=$newdir.'/'.$junk[$j];
                   $dir=$dir.'/'.$junk[$j];
               }
           }
    if(defined $predefined){
        $newfile=$predefined;
    }
    else{
     my $mkdir="mkdir -p $newdir";
     system($mkdir);
    }
    my $cp="";
    if($crdel==1){
        $cp=" ln -sf $path $newfile";
        if($path=~/^\/castor/){
            $path=~s/\/cern.ch\/ams//;
            $cp="ln -sf $path $newfile";
        }
    }
    else{
        $cp="rm $file";
        my $i=system($cp);
        $cp="rm $newfile";
    }
    my $i=system($cp);
    if($i){
        print "Problem with $cp \n";
    }
    return $newfile;
}

sub getactiveppstring{
    my $self=shift;
    my $sql = "SELECT  DID  FROM ProductionSet WHERE STATUS='Active' ORDER BY DID";
    my $ret = $self->{sqlserver}->Query($sql);
    my $jobspid=undef;
     foreach my $pp  (@{$ret}){
          if(defined $jobspid){
            $jobspid=$jobspid." or jobs.pid =";
          }
          else{
           $jobspid="  ( jobs.pid =";
          }
          $jobspid=$jobspid." $pp->[0] ";
      }
       if(defined $jobspid){
        $jobspid=$jobspid." ) and ";
       }
       else{
        $jobspid="";
    }
    return $jobspid;
}

sub eosLink2Xrootd {
    my $self = shift;
    my $path = shift;

    $path =~ s#^/eosams/#root://eosams.cern.ch//eos/ams/#g;
    return $path;
}
