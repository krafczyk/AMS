# $Id: RemoteClient.pm,v 1.163 2003/05/12 17:04:22 alexei Exp $
#
# Apr , 2003 . ak. Default DST file transfer is set to 'NO' for all modes
#
# to add - standalone/client type,
#          CPU, elapsed time, cite and host info into Job table
#
# Apr-May, 2003 : ValidateRuns, checkJobsTimeout, deleteTimeOutJobs,
#                 parseJournalFiles
#
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
@RemoteClient::EXPORT= qw(new  Connect Warning ConnectDB listAll listAllDisks listMin queryDB DownloadSA checkJobsTimeout deleteTimeOutJobs parseJournalFiles ValidateRuns updateAllRunCatalog printMC02GammaTest);

my     $bluebar      = 'http://ams.cern.ch/AMS/icons/bar_blue.gif';
my     $maroonbullet = 'http://ams.cern.ch/AMS/icons/bullet_maroon.gif';
my     $bluebullet   = 'http://ams.cern.ch/AMS/icons/bullet_blue.gif';
my     $silverbullet = 'http://ams.cern.ch/AMS/icons/bullet_silver.gif';
my     $purplebullet = 'http://ams.cern.ch/AMS/icons/bullet_purple.gif';

my     $srvtimeout = 30; # server timeout 30 seconds
my     @colors=("red","green","blue","magenta","orange","cyan","tomato");

my     $amscomp_page ='http://ams.cern.ch/AMS/Computing/computing.html';
my     $amshome_page ='http://ams.cern.ch/AMS/ams_homepage.html';
my     $dbqueryR   ='http://ams.cern.ch/AMS/Computing/dbqueryR.html';

my     $downloadcgi       ='http://pcamsf0.cern.ch/cgi-bin/mon/download.o.cgi';
my     $downloadcgiMySQL  ='http://pcamsf0.cern.ch/cgi-bin/mon/download.mysql.cgi';

my     $monmcdb      ='http://pcamsf0.cern.ch/cgi-bin/mon/monmcdb.o.cgi';
my     $monmcdbMySQL ='http://pcamsf0.cern.ch/cgi-bin/mon/monmcdb.mysql.cgi';

my     $rccgi      ='http://pcamsf0.cern.ch/cgi-bin/mon/rc.o.cgi?queryDB=Form';
my     $rccgiMySQL ='http://pcamsf0.cern.ch/cgi-bin/mon/rc.mysql.cgi?queryDB=Form';

my     $rchtml='http://pcamsf0.cern.ch/rc.html';
my     $rchtmlMySQL='http://ams.cern.ch/AMS/Computing/mcproduction/rc.mysql.html';

my     $validatecgi      ='http://pcamsf0.cern.ch/cgi-bin/mon/validate.o.cgi';
my     $validatecgiMySQL ='http://pcamsf0.cern.ch/cgi-bin/mon/validate.mysql.cgi';

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
        UploadsDir=>undef,
        UploadsHREF=>undef,
        FileDB=>undef,
        FileCRC=>undef,
        FileBBFTP=>undef,
        FileAttDB=>undef,
        FileCRCTimestamp=>undef,
        FileBBFTPTimestamp=>undef,
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
       arsref=>[],
       arpref=>[],
       ardref=>[],
       dbfile=>undef,
       scriptsOnly=>1,
       senddb=>0,
       sendaddon=>0,
       dwldaddon=>0
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
#    $self->{Name}="/cgi-bin/mon/validate.cgi";
    my $mybless=bless $self,$type;
    if(ref($RemoteClient::Singleton)){
        croak "Only Single RemoteClient Allowed\n";
    }
$RemoteClient::Singleton=$mybless;
return $mybless;
}

sub Init{
    my $self = shift;
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


#syntax (momenta,perticles, ...

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
     my $sql="select myvalue from Environment where mykey='CERN_ROOT'";
     my $ret=$self->{sqlserver}->Query($sql);
     if( defined $ret->[0][0]){
       $self->{CERN_ROOT}=$ret->[0][0];
     }
     else{
      $self->{CERN_ROOT}="/cern/2001";
  }
 }
    my $key='UploadsDir';
    my $sql="select myvalue from Environment where mykey='".$key."'";
    my $ret=$self->{sqlserver}->Query($sql);
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
    sub prio { $b->{TOTALEVENTS}*$b->{CPUPEREVENTPERGHZ}  <=> $a->{TOTALEVENTS}*$a->{CPUPEREVENTPERGHZ};}
    my @tmpb=sort prio @tmpa;
    foreach my $tmp (@tmpb){
     push @{$dataset->{jobs}},$tmp;     
    }
      }
    }
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


 my $ior=$self->getior();
if(not defined $ior){ 

  foreach my $chop  (@ARGV){
    if($chop =~/^-I/){
        $ior=unpack("x1 A*",$chop);
    }
  }
}
    if(defined $ior ){
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
                   my $sql="update Servers set status='Dead' where dbfilename='$ref->{dbfile}'";
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
                 $sql="insert into Servers values('$ref->{dbfile}','$ref->{IOR}','$ref->{IORP}',NULL,'Active',$ac->{Start},$createt)";
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
              my $i=system("chmod +x $full");
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
    if( not $self->Init()){
        die "ValidateRuns -F- Unable To Init";
        
    }
    if (not $self->ServerConnect()){
        die "ValidateRuns -F- Unable To Connect To Server";
    }
#
    $vdir=$self->getValidationDir();
    if (not defined $vdir) {
      die " ValidateRuns -F- cannot get path to ValidationDir \n";
     }
    $vlog = $vdir."/validateRuns.log.".$timenow;   

    open(FILE,">".$vlog) or die "Unable to open file $vlog\n";

    
    $self->htmlTop();

# get list of runs from DataBase
    $sql="SELECT run,submit FROM Runs";
    my $ret=$self->{sqlserver}->Query($sql);
# get list of runs from Server
    if( not defined $self->{dbserver}->{rtb}){
      DBServer::InitDBFile($self->{dbserver});
    }
    foreach my $run (@{$self->{dbserver}->{rtb}}){
     my @cpntuples   =();
     my @mvntuples   =();
     my $runupdate   = "UPDATE runs SET ";
     my $copyfailed  = 0;
# check if run is registered in database
     $sql   = "SELECT run, status FROM runs WHERE run=$run->{Run}";
     my $r0 = $self->{sqlserver}->Query($sql);
     if (not defined $r0->[0][0]) {
      htmlTable("Insert Runs");
      print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
      print "<td><b><font color=\"blue\">Run </font></b></td>";
      print "<td><b><font color=\"blue\" >FirstEvent</font></b></td>";
      print "<td><b><font color=\"blue\" >LastEvent</font></b></td>";
      print "<td><b><font color=\"blue\" >SubmitTime </font></b></td>";
      print "<td><b><font color=\"blue\" >Status</font></b></td>";
      print "</tr>\n";
      print FILE "INSERT $run->{Run}, 
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

       $sql   = "SELECT run, status FROM runs WHERE run=$run->{Run}";
       $r0 = $self->{sqlserver}->Query($sql);
       print "<td><b><font color=\"black\">$run->{Run} </font></b></td>";
       print "<td><b><font color=\"black\" >$run->{FirstEvent}</font></b></td>";
       print "<td><b><font color=\"black\" >$run->{LastEvent}</font></b></td>";
       print "<td><b><font color=\"black\" >$run->{SubmitTime} </font></b></td>";
       print "<td><b><font color=\"black\" >$run->{Status}</font></b></td>";
       print "</tr>\n";
      htmlTableEnd();
  }
     if(($run->{Status} eq "Finished" || $run->{Status} eq "Failed") && 
         (defined $r0->[0][1] && ($r0->[0][1] ne "Completed" && $r0->[0][1] ne "Unchecked" && $r0->[0][1] ne "TimeOut"))
        ){
        my $fevent =  1;
        my $levent =  0;
# check if corresponding job exist
         $sql   = "SELECT runs.status, jobs.content FROM runs,jobs 
                          WHERE jobs.jid=$run->{Run} AND runs.jid=jobs.jid";
         my $r1 = $self->{sqlserver}->Query($sql);
         if (not defined $r1->[0][0]) { 
          $sql = "UPDATE runs SET status='Failed' WHERE run=$run->{Run}"; 
          $self->{sqlserver}->Update($sql);
          $warn = "cannot find status, content in Jobs for JID=$run->{Run}. *TRY* Delete Run=$run->{Run} from server \n";
          htmlWarning("ValidateRuns",$warn);
          print FILE $warn;
# ---          DBServer::sendRunEvInfo($self->{dbserver},$run,"Delete"); 
         } else {
          my $jobstatus  = $r1->[0][0];
          my $jobcontent = $r1->[0][1];
          if ($jobcontent =~ m/-GR/) {
#
# remote job
#            update jobinfo first
           my $events  = $run->{cinfo}->{EventsProcessed};
           my $errors  = $run->{cinfo}->{CriticalErrorsFound};
           my $cputime = sprintf("%.2f",$run->{cinfo}->{CPUTimeSpent});
           my $elapsed = sprintf("%.2f",$run->{cinfo}->{TimeSpent});
           my $host    = $run->{cinfo}->{HostName};

           $sql = "UPDATE jobs SET 
                                     EVENTS=$events,
                                     ERRORS=$errors,
                                     CPUTIME=$cputime,
                                     ELAPSED=$elapsed,
                                     HOST='$host',
                                     TIMESTAMP=$timenow  
                            WHERE JID = $run->{Run}";
          $self->{sqlserver}->Update($sql);
          print FILE $sql;
# validate ntuples
# Find corresponding ntuples from server
              foreach my $ntuple (@{$self->{dbserver}->{dsts}}){
              if($ntuple->{Type} eq "Ntuple" and ($ntuple->{Status} eq "Success" or 
                $ntuple->{Status} eq "Validated") and $ntuple->{Run}== $run->{Run}){
                  $levent += $ntuple->{LastEvent}-$ntuple->{FirstEvent}+1;
                  $ntuple->{Name}=~s/\/\//\//;                  
                  my @fpatha=split ':', $ntuple->{Name};
                  my $fpath=$fpatha[$#fpatha];
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
                   close FILE;
                   my $validatecmd = "$self->{AMSSoftwareDir}/exe/linux/fastntrd.exe  $fpath $ntuple->{EventNumber}";
                   my $i=system($validatecmd);
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
                                                $ntuple->{Size},
                                                $status,
                                                $outputpath,
                                                $ntuple->{crc});
                            print FILE "insert $run->{Run}, $outputpath \n";
                            $copied++;
                            push @cpntuples, $fpath;
                            $runupdate = "UPDATE runs SET FEVENT = $fevent, LEVENT=$levent, ";
                         } else {
                             print FILE "failed to copy $fpath\n";
                             $copyfailed = 1;
                             last;
                         }
                       }  # ntuple status 'OK'
                      }
               }

             } # ntuple ->{Status} == "Validated"
         } #loop for ntuples
         my $status='Failed';
         if ($copyfailed == 0) {
          $warn = "Validation done : *TRY* send Delete to DBServer, Run =$run->{Run} \n";
          print FILE $warn;
          htmlText($warn," ");
#--          DBServer::sendRunEvInfo($self->{dbserver},$run,"Delete"); 
          foreach my $ntuple (@cpntuples) {
           my $cmd="rm -i $ntuple";
           system($cmd);
           $warn = "Validation done : $cmd \n";
           print FILE $warn;
           htmlText($warn," ");
         }
          if ($#cpntuples >= 0) { $status = 'Completed';}
      }
           else{
               $warn="Validation/copy failed : Run =$run->{Run} \n";
               print FILE $warn;
               htmlText($warn," ");
               $status='Unchecked';
               foreach my $ntuple (@mvntuples) {
                my $cmd = "rm -i $ntuple";
                $warn="Validation failed : $cmd \n";
                print FILE $warn;
                htmlText($warn," ");
                system($cmd);
                $failedcp++;
                $copied--;
               }
               $sql = "DELETE ntuples WHERE run=$run->{Run}";
               $self->{sqlserver}->Update($sql);
               $runupdate = "UPDATE runs SET ";
           }
          $sql = $runupdate." STATUS='$status' WHERE run=$run->{Run}";
          $self->{sqlserver}->Update($sql);
          $warn = "Update Runs : $sql\n";
          print FILE $warn;
          htmlText($warn," ");
          if ($status eq "Completed") {
           $self->updateRunCatalog($run->{Run});
           $warn = "Update RunCatalog table : $run->{Run}\n";
           print FILE $warn;
          }
     } # remote job
    }  # job found
   }   # run->{Status} == 'Finished' || 'Failed'
  }    # loop for runs from 'server'

  $self->htmlBottom();
  $warn = "$validated Ntuple(s) Successfully Validated.
                   \n $copied Ntuple(s) Copied.
                   \n $failedcp Ntuple(s) Not Copied.
                   \n. $bad Ntuple(s) Turned Bad.
                   \n $unchecked Ntuples(s) Could Not Be Validated.
                   \n $thrusted Ntuples Could Not be Validated But Assumed  OK.
                   \n";
 print FILE $warn;
 close FILE;
 $self->InfoPlus($warn);
 $self->{ok}=1;

}

sub doCopy {
     my $self = shift;
     my $jid  = shift;
     my $inputfile = shift;
     my $crc       = shift;
# 
     my @junk = split '/',$inputfile;
     my $file = $junk[$#junk];

     my $time = time();
     my $logfile = "/tmp/cp.$time.log";
     my $cmd = "touch $logfile";
     my $cmdstatus = system($cmd); 

     my $outputdisk => undef;
     my $outputpath => undef;

     my $filesize = 0;
     $filesize    = (stat($inputfile))[7];
     if ($filesize > 0) {
      $filesize = 1+$filesize/1000000/1000;
      my $mtime = 0;
      my $pset  => undef;
      my $sql   => undef;
# get production set path
       $sql = "SELECT NAME FROM ProductionSet WHERE STATUS='Active'";
       my $r0 = $self->{sqlserver}->Query($sql);
       if (defined $r0->[0][0]) {
         $pset=trimblanks($r0->[0][0]);
# find disk
         $sql = "SELECT disk, path FROM filesystems WHERE AVAILABLE > $filesize 
                   ORDER BY priority DESC, available DESC";
         my $ret = $self->{sqlserver}->Query($sql);
         foreach my $disk (@{$ret}) {
           $outputdisk = trimblanks($disk->[0]);
           $outputpath = trimblanks($disk->[1]);
           $outputpath = $outputpath."/".$pset;
           $mtime = (stat $outputpath)[9];
           if ($mtime) { last;}
          }   
         if ($mtime) {
# find job
          $sql = "SELECT cites.name,jobname,jobs.jid FROM jobs,cites 
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
               $cmd = "cp -pi -d -v $inputfile  $outputpath >> $logfile";
               $cmdstatus = system($cmd);
               if ($cmdstatus == 0 ) {
                 my $rstatus = system("$self->{AMSSoftwareDir}/exe/linux/crc $outputpath $crc");
                 $rstatus=($rstatus>>8);
                 if ($rstatus == 1) {
                   return $outputpath,1;
                 } else {
                  htmlWarning("doCopy","crc calculation failed for $outputpath");
                  htmlWarning("doCopy","crc calculation failed status $rstatus");
                  return $outputpath,0;
                 }
               }
               return $outputpath,0;
              } else {
               htmlWarning("doCopy","failed $cmd");
             }
            } else {
             htmlWarning("doCopy","cannot find dataset for JID=$jid");
            }
           } else {
            htmlWarning("doCopy","cannot get info for JID=$jid");
           }
        } else {
         htmlWarning("doCopy","cannot stat disk from Filesystems");
        } 
      } else {
       htmlWarning("doCopy","cannot get ProductionSet status=Active");
     }
  } else {
    htmlWarning("doCopy","cannot stat $inputfile");
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
         if(not $found){
       
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
              if($ntuple->{Type} eq "Ntuple" and ($ntuple->{Status} eq "Success" or $ntuple->{Status} eq "Validated") and $ntuple->{Run}== $run->{Run}){
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
                      my $i=system("$self->{AMSSoftwareDir}/exe/linux/fastntrd.exe  $fpath $ntuple->{EventNumber}");
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
#        find all the unchecked ntuples
             $sql="select path from Ntuples where status='Unchecked' and run=$run->{Run}";
             my $rts=$self->{sqlserver}->Query($sql);
             if(defined $rts->[0][0]){
                 foreach my $uc (@{$rts}){
                     my $path=$uc->[0];
                     foreach my $ntuple (@{$self->{dbserver}->{dsts}}){
                     if($ntuple->{Type} eq "Ntuple" and $ntuple->{Status} eq "Success" and $ntuple->{Run}== $run->{Run} ){
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
                      my $i=system("$self->{AMSSoftwareDir}/exe/linux/fastntrd.exe  $fpath $ntuple->{EventNumber}");
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
   $self->InfoPlus("$validated Ntuple(s) Successfully Validated.\n $bad Ntuple(s) Turned Bad.\n $unchecked Ntuples(s) Could Not Be Validated.\n $thrusted Ntuples Could Not be Validated But Assumed  OK.");
   $self->{ok}=1;
}

sub ConnectDB{

    my $self = shift;
    if( not $self->Init()){
        return 0;
    }
    else{
        $self->{ok}=1;
    }
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

#understand parameters

        $self->{read}=0;
    my $q=$self->{q};
    my $sql=>undef;
    my $color;
# db query
    if ($self->{q}->param("getJobID")) {
     $self->{read}=1;
     if ($self->{q}->param("getJobID") eq "Submit") {
        htmlTop();
        my $title = "Job : ";
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
                                AND jobs.cid=cites.cid ORDER BY jobs.jid";
            } else {
             $jobid =  trimblanks($q->param("JobID"));
             $title = $title.$jobid;
             $sql = "SELECT jobname, triggers , host, events, errors, cputime, 
                            elapsed, cites.name, content, jobs.timestamp, jobs.jid 
                          FROM jobs, cites 
                          WHERE jobs.jid=$jobid AND jobs.cid=cites.cid";
            }
        }
        my $content = " ";
        $self->htmlTemplateTable($title);
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
        if ($jobmax > 0) {
               print "<td align=center><b><font color=\"blue\">JID </font></b></td>";
        }
               print "<td align=center><b><font color=\"blue\">Name </font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Triggers</font></b></td>";
               print "<td align=center><b><font color=\"blue\" >Cite </font></b></td>";
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
             if ($status eq 'Finished') {
                 $color = "green";
             } elsif ($status eq 'Failed') {
                 $color = "red";
             }
             if ($jobmax > 0) {
              print "
                  <td align=left><b><font color=$color> $jid </font></td></b>";
          }
           print "
                  <td align=left><b><font color=$color> $jobname </font></td></b>
                  <td align=center><b><font color=$color> $triggers </font></b></td>
                  <td align=center><b><font color=$color> $cite </font></td></b>
                  <td align=center><b><font color=$color> $host </font></td></b>
                  <td align=center><b><font color=$color> $events </font></b></td>
                  <td align=center><b><font color=$color> $errors </font></b></td>
                  <td align=center><b><font color=$color> $cputime </font></b></td>
                  <td align=center><b><font color=$color> $elapsed </font></b></td>
                  <td align=center><b><font color=$color> $status </font></b></td>
                  <td align=center><b><font color=$color> $timestamp </font></b></td>\n";
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

    if ($self->{q}->param("queryDB")) {
     $self->{read}=1;
     if ($self->{q}->param("queryDB") eq "Submit") {
        $sql="SELECT RUN, PART FROM runcatalog WHERE ";
        htmlTop();
        my $nickname=>undef;
        my $cutoff=>undef;
        my $dataset=>undef;
        my $file=>undef;
        my $focus=>undef;
        my $qcos=>undef;
        my $qmomi=>undef;
        my $qmoma=>undef;
        my $qplanes=>undef;
        my $qxl=>undef;
        my $qyl=>undef;
        my $qzl=>undef;
        my $qxu=>undef;
        my $qyu=>undef;
        my $qzu=>undef;
        my $particle=>undef;
        my $setup=>undef;
        my $spectrum=>undef;
        my $trtype=>undef;
        my $color="green";


        my @tempnam=();
        my $hash={};
        my @desc=();
        my $cite={};

        foreach my $subdataset (@{$self->{DataSetsT}}){
           foreach my $cite (@{$subdataset->{jobs}}){
            if(not ($cite->{filename} =~/^\./)){
               push @tempnam, $cite->{filename};
                $hash->{$cite->{filename}}=$cite->{filedesc};
                my @description = split /Total/,$hash -> {$cite->{filename}}=$cite->{filedesc};
                push @desc, $description[0];
                 }
             }
       }

        $self->htmlTemplateTable("Selected Query Keys :");


        if (defined $q->param("QTempDataset") and $q->param("QTempDataset") ne "Any") {
         $dataset = $desc[$q->param("QTempDataset")];
         $dataset = trimblanks($dataset);
         my $sdataset = $dataset;
         $sdataset =~ s/ /\%/g;
         $sql = $sql." jobname like '%$sdataset%' AND ";
         print "<tr><td><font size=\"3\" color=\"red\"><b>Job Dataset :</b></td><td><b> $dataset</b></td></tr>\n";
        }
        if (defined $q->param("QPart") and $q->param("QPart") ne "Any") {
         $particle = $q->param("QPart");
         print "<tr><td><font size=\"3\" color=$color><b>Particle : </b></td><td><b> $particle</b></td></tr>\n";
         my $particleid=$self->{tsyntax}->{particles}->{$particle};
         $sql=$sql."PART=".$particleid." AND ";
      }
        if (defined $q->param("QMomI")) {
         $qmomi = $q->param("QMomI");
         print "<tr><td><font size=\"3\" color=$color><b>Momentum min [Gev/c] </b></td><td><b> >= $qmomi</b></td></tr>\n";
         $sql=$sql." PMIN >= $qmomi AND ";
      }
        if (defined $q->param("QMomA")) {
         my $qmoma = $q->param("QMomA");
         print "<tr><td><font size=\"3\" color=$color><b>Momentum max [Gev/c] </b></td><td><b> =< $qmoma</b></td></tr>\n";
         $sql=$sql." PMAX <= $qmoma AND ";
     }
        if (defined $q->param("QTrType")) {
         $trtype = $q->param("QTrType");
         print "<tr><td><font size=\"3\" color=$color><b>Trigger Type : </b></td><td><b> $trtype</b></td></tr>\n";
         $sql=$sql." TRTYPE= '$trtype' AND ";
      }
        if (defined $q->param("QSetup")) {
         $setup = $q->param("QSetup");
         print "<tr><td><font size=\"3\" color=$color><b>Setup : </b></td><td><b> $setup</b></td></tr>\n";
         $sql=$sql." SETUP= '$setup' AND ";
      }
        if (defined $q->param("NTOUT")) {
         my $outform=>undef;
         my $ntchain="NTchain";
         my $ntout=$q->param("NTOUT");
         if ($q->param("NTOUT") eq "ALL") {
          $outform = "List ALL NTuples matching to query";
         } elsif ($q->param("NTOUT") eq "RUNS") { 
          $outform = "List Runs matching to query";
         } elsif ($q->param("NTOUT") eq "NTCHAIN") {
          if ( defined $q->param("NTCHAIN")) {
           $ntchain=$q->param("NTCHAIN");
          }
          $outform = "List  NTuples names for chain '$ntchain'";
         } else {
             $outform = "Print summary for NTuples matching query";
         }
         print "<tr><td><font size=\"3\" color=\"green\"><b>Output Format : </b></td><td><b> $outform</b></td></tr>\n";
      print "<INPUT TYPE=\"hidden\" NAME=\"NTOUT\" VALUE=\"$ntout\">\n"; 
      print "<INPUT TYPE=\"hidden\" NAME=\"NTCHAIN\" VALUE=\"$ntchain\">\n"; 

     }
        $sql=$sql." runcatalog.TIMESTAMP != 0 ORDER BY PART, RUN ";
      htmlTableEnd();
#                  RUN          NUMBER       NOT NULL
#  $particle       PART         NUMBER(10)
#  $qmomi          PMIN         NUMBER(24)
#  $qmoma          PMAX         NUMBER(24)
#  $trtype         TRTYPE       VARCHAR(255)
#  $spectrum       SPECTRUM     VARCHAR(255)
#  $setup          SETUP        VARCHAR(255)
#  $cutoff         CUTOFF       NUMBER(3)
#  $focus          FOCUS        VARCHAR(255)
#  $qplanes        SURFACE      VARCHAR(255)
#  $qcos           COSTHETA     NUMBER(10)
#  $qxl            XL           NUMBER(24)
#  $qyl            YL           NUMBER(24)
#  $qzl            ZL           NUMBER(24)
#  $qxu            XU           NUMBER(24)
#  $qyu            YU           NUMBER(24)
#  $qzu            XU           NUMBER(24)
#                  timestamp    NUMBER(24)
#
#        print "$sql \n";
     print "<INPUT TYPE=\"hidden\" NAME=\"SQLQUERY\" VALUE=\"$sql\">\n"; 
     print "<input type=\"submit\" name=\"queryDB\" value=\"DoQuery\">        ";
     htmlReturnToQuery();
    htmlBottom();
  } elsif ($self->{q}->param("queryDB") eq "DoQuery") {
      htmlTop();
      my $sql=>undef;
      my $ntout="ALL";
      my $totalev=0;
      my $totalnt=0;
      my $mintm  = time();
      my $maxtm  = 0;
      my $totaljobs= 0;
      my $plist=" ";
      my $pold=>undef;
      if (defined $q->param("SQLQUERY")) {
        $sql = $q->param("SQLQUERY");
        if (defined $q->param("NTOUT")) {
            $ntout=$q->param("NTOUT");
        }
             if ($ntout eq "RUNS") {
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<td><b><font color=\"blue\" >Particle </font></b></td>";
               print "<td><b><font color=\"blue\">Job </font></b></td>";
               print "<td><b><font color=\"blue\" >Run </font></b></td>";
               print "<td><b><font color=\"blue\" >Job Submit Time </font></b></td>\n";
              print "</tr>\n";
           }

       my $ret=$self->{sqlserver}->Query($sql);
       if (defined $ret->[0][0]) {
         my $newchain = 0;
         foreach my $r (@{$ret}){
             $totaljobs++;
             my $run=$r->[0];
             my $particleid=$r->[1];
             my $part=>undef;
             $sql="SELECT jobname, runs.submit FROM Jobs, Runs 
                     WHERE Jobs.jid=Runs.jid AND Runs.RUN=$run ORDER BY Runs.run";
             my $r3=$self->{sqlserver}->Query($sql);
             if (defined $r3->[0][0]) {
              my $ts=$self->{tsyntax};
              my %hash=%{$ts->{particles}};
              my @keysa=sort {$hash{$a} <=>$hash{$b}} keys %{$ts->{particles}};
              foreach my $particle (@keysa){
                if ($particleid eq $self->{tsyntax}->{particles}->{$particle}) {
                    $part=$particle;
                    last;
              }
            }
              my $color = Color($particleid);
              my $jobname=>undef;
              my $submittime=>undef;
             
              foreach my $job (@{$r3}) {
                  $jobname   = $job->[0];
                  my $submit = $job->[1];
                  $submittime= localtime($submit);
              }
              if ($ntout eq "ALL") {
               print "<tr><td><b><font size=\"4\" color=$color> Job : <i>$jobname </i>, Run : <i>$run </i>, Particle : <i> $part </i>,  Submitted : <i> $submittime </i></font></b></td></tr>\n";
           }
            if ($ntout eq "RUNS") {
               my $color="black";
               print "
                  <td><b><font color=$color> $part </font></td></b>
                  <td><b><font color=$color> $jobname </font></td></b>
                  <td><b><font color=$color> $run </font></b></td>
                  <td><b><font color=$color> $submittime </font></b></td>\n";
               print "</font></tr>\n";
           }

# oracle specific SUM, COUNT, MIN, MAX
       if ($ntout eq "SUMM") {
           if ($pold ne $part) {
            $plist=$plist." ".$part;
            $pold=$part;
           }
           $sql="SELECT COUNT(NEVENTS) From Ntuples WHERE RUN=$run";
           my $rsum=$self->{sqlserver}->Query($sql);
           if (defined $rsum->[0][0]) {
            $totalnt=$totalnt+$rsum->[0][0];
            $sql="SELECT SUM(NEVENTS) FROM Ntuples WHERE RUN=$run";
            my $rev=$self->{sqlserver}->Query($sql);
            if (defined $rsum->[0][0]) {
             $totalev=$totalev+$rev->[0][0];
            }
            $sql="SELECT MIN(timestamp) FROM Ntuples WHERE RUN=$run";
            my $rtm=$self->{sqlserver}->Query($sql);
            if (defined $rtm->[0][0]) {
                if ($mintm > $rtm->[0][0]) {
                    $mintm=$rtm->[0][0];
                }
            }
            $sql="SELECT MAX(timestamp) FROM Ntuples WHERE RUN=$run";
            $rtm=$self->{sqlserver}->Query($sql);
            if (defined $rtm->[0][0]) {
                if ($maxtm < $rtm->[0][0]) {
                    $maxtm=$rtm->[0][0];
                }
            }
        }
       }
      if ($ntout eq "NTCHAIN") {
        my $ntchainname=$q->param("NTCHAIN");
        print "<TABLE BORDER=\"0\" WIDTH=\"100%\">";
        if ($newchain == 0) {
         print("<TR><TD width=\"10%\" align=\"Left\"><B> Chain   </B> </TD>
                   <TD width=\"20%\" align=\"Center\"><B> Name </B></TD>
                   <TD width=\"40%\" align=\"Center\"><B> FilePath </B></TD></TR>\n");
         $newchain = 1;
        }
        $sql="SELECT PATH FROM Ntuples WHERE RUN=$run ";
        my $r4=$self->{sqlserver}->Query($sql);
        if (defined $r4->[0][0]) {
            foreach my $nt (@{$r4}) {
             my $filepath=$nt->[0];
             my ($disk,$file) = split(/:/,$filepath);
             if (not defined $file) { $file=$filepath;}
             print("<TR><TD width = \"10\%\" align=\"Left\"> nt/chain </TD>
                       <TD width = \"20\%\" align=\"Center\"> $ntchainname </TD>
                       <TD width = \"40\%\" align=\"Left\">   $file    </TD></TR>");
          }
        }
      htmlTableEnd();
    }
       if ($ntout eq "ALL") {
        print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
              print "<table border=1 width=\"100%\" cellpadding=0 cellspacing=0>\n";
              print "<tr><td width=10% align=left><b><font color=\"blue\" > NTuple </font></b></td>";
              print "<td width=10%><b><font color=\"blue\"> Events </font></b></td>";
              print "<td width=15%><b><font color=\"blue\" > Errors </font></b></td>";
              print "<td width=15%><b><font color=\"blue\" > Size[MB] </font></b></td>";
              print "<td td align=middle><b><font color=\"blue\" > Produced </font></b></td>";
              print "<td width=10%><b><font color=\"blue\" > Status </font></b></td>";
             print "</tr>\n";
   
               my $jobname=$r3->[0][0];
               $sql="SELECT PATH, NEVENTS, NEVENTSERR, TIMESTAMP, STATUS, SIZEMB FROM Ntuples WHERE RUN=$run ";
               my $r4=$self->{sqlserver}->Query($sql);
               if (defined $r4->[0][0]) {
                 foreach my $nt (@{$r4}){
                  my $timel =localtime($nt->[3]);
                  my ($wday,$mon,$day,$time,$year) = split " ",$timel;
                  my $status=$nt->[4];
                  my $color=statusColor($status);
                  print "<td><b> $nt->[0] </td></b><td><b> $nt->[1] </td>
                         <td><b> $nt->[2] </b></td>
                         <td><b> $nt->[5] </b></td>
                         <td><b> $mon $day, $time, $year </b></td> 
                         <td align=middle><b><font color=$color> $status </font></b></td> \n";
                  print "</font></tr>\n";
              }
             }
            htmlTableEnd();
         print "<BR><BR>\n";
      }   
     }
    }
   } else {
         print "<TR><TD><font color=\"magenta\"><B> Nothing Found for SQL request : </B></font></TD></TR>\n";
         print "<TR><TD><i>$sql</i></td></tr>\n";
       }
   } else {
           $self->ErrorPlus("Unrecognized SQL request.");
          }

   if ($ntout eq "SUMM") {
       my $from=localtime($mintm);
       my $to  =localtime($maxtm);
    print "<tr><td><b><font size=\"4\" color=\"red\"><ul>  For Particles : <i> $plist </i></b></font></td></tr>\n";
    print "<tr><td><b><font size=\"4\" color=\"green\"><ul>  Jobs Completed : $totaljobs </b></font></td></tr>\n";
    print "<tr><td><b><font size=\"4\" color=\"green\"><ul>  $from / $to </b></font></td></tr>\n";
    print "<tr><td><b><font size=\"4\" color=\"blue\"><ul>   NTuples : $totalnt  </b></font></td></tr>\n";
    
    print "<tr><td><b><font size=\"4\" color=\"blue\"><ul>   Events : $totalev  </b></font></td></tr>\n";
   }      
   if ($ntout eq "RUNS") {
       htmlTableEnd();
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
   print "&nbsp;<b><a href=\"http://pcamsf0.cern.ch/mm.html\">MC production</a>\n";
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
   print "All comments (to <font color=\"green\"> alexei.klimentov\@cern.ch, vitali.choutko\@cern.ch </font>) appreciated (items in <font color=\"tomato\"> tomato </font> are not implemented yet). Basic query keys are 
in <font color=\"green\"> green </font>, advanced query keys are in <font color=\"blue\"> blue.</TD></TR>\n";
   print "</ul>\n";
   print "<font size=\"2\" color=\"black\">\n";
   print "<li> Catalogues are updated nightly.\n";
   print "<li> To browse AMS01 data and AMS02 NTuples produced before March 2002 click <a href=\"http://pcamsf0.cern.ch/cgi-bin/mon/print.gamma.test.cgi\"> here </a>\n";
   print "<p>\n";
    print "<TR><B><font color=green size= 4> Select by key(s) (you can select multiple keys) </font>";
    print "<p>\n";
    print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";
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
          print "<tr><td><b><font color=\"green\" size=2>Job Template</font></b>\n";
          print "</td><td>\n";
          print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
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
                  my @description = split /Total/,$hash -> {$cite->{filename}}=$cite->{filedesc};
                  push @desc, $description[0];
                 }
             }
            }
            print "<tr valign=middle><td align=left><b><font color=\"tomato\" size=\"-1\"> Dataset :</b></td><td colspan=1>\n";
            print "<select name=\"QTempDataset\" >\n";
            print "<option value=\"Any\">  </option>\n";
            my $i=0;
            foreach my $template (@tempnam) {
             my $subdataset = $i;
             print "<option value=\"$subdataset\">$desc[$i] </option>\n";
             $i++;
            }
#            print "</select>\n";
#            htmlTextField("Job nickname","text",80,'Any',"QNick"," ");  
#            print "</b></td></tr>\n";
        htmlTableEnd();
# Job Parameters
          print "<tr><td><b><font color=\"green\" size=2>Job Parameters</font></b>\n";
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
            htmlTextField("Momentum min >=","number",7,1.,"QMomI","[GeV/c]");  
            htmlTextField("Momentum max =<","number",7,2000.,"QMomA","[GeV/c]");  
            htmlTextField("Setup","text",20,"AMS02","QSetup"," ");
            htmlTextField("Trigger Type ","text",20,"AMSParticle","QTrType"," ");
           htmlTableEnd();
# spectrum and focusing
#            print "<tr><td><b><font color=\"blue\" size=2>Spectrum and Focusing</font></b>\n";
#            print "</td><td>\n";
#            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
#            $ts=$self->{tsyntax};
#            @keysa=sort keys %{$ts->{spectra}};
#              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Input Spectrum : </b></td> <td colspan=1>\n";
#              print "<select name=\"QSpec\" >\n";
#              print "<option value=\"Any\">  </option>\n";
#              foreach my $spectrum (@keysa) {
#                print "<option value=\"$spectrum\">$spectrum </option>\n";
#              }
#              print "</select>\n";
#              print "</b></td></tr>\n";
#           @keysa=sort keys %{$ts->{focus}};
#              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Focusing : </b></td> <td colspan=1>\n";
#              print "<select name=\"QFocus\" >\n";
#              print "<option value=\"Any\">  </option>\n";
#              foreach my $focus (@keysa) {
#                print "<option value=\"$focus\">$focus </option>\n";
#              }
#              print "</select>\n";
#              print "</b></td></tr>\n";
#           htmlTableEnd();
## geo magnetic cutoff
#            print "<tr><td><b><font color=\"blue\" size=2>GeoMagnetic Cutoff</font></b>\n";
#            print "</td><td>\n";
#            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
#            print "<tr><td><font size=\"-1\"<b>\n";
#             print "<INPUT TYPE=\"radio\" NAME=\"GCF\" VALUE=\"1\" CHECKED><b> Yes </b>\n";
#             print "<INPUT TYPE=\"radio\" NAME=\"GCF\" VALUE=\"0\" ><b> No </b><BR>\n";
#             print "</b></font></td></tr>\n";
#           htmlTableEnd();
## cube coordinates
#            print "<tr><td><b><font color=\"blue\" size=2>Cube Coordinates</font></b>\n";
#            print "</td><td>\n";
#            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
#            print "<tr><td width=\"30%\"><font size=\"2\">\n";
#            print "<b> Min : X : <input type=\"number\" size=5 value=-195 name=\"QXL\"></td>\n";  
#            print "<td width=\"30%\"><font size=\"2\">\n";
#            print "<b> Y : <input type=\"number\" size=5 value=-195 name=\"QYL\"></td>\n";  
#            print "<td width=\"30%\"><font size=\"2\">\n";
#            print "<b> Z : <input type=\"number\" size=5 value=-195 name=\"QZL\"> (cm)</td>\n";  
#            print "</b></font></tr>\n";
#            print "<tr><td width=\"30%\"><font size=\"2\">\n";
#            print "<b> Max : X : <input type=\"number\" size=5 value=-195 name=\"QXU\"></td>\n";  
#            print "<td width=\"30%\"><font size=\"2\">\n";
#            print "<b> Y : <input type=\"number\" size=5 value=-195 name=\"QYU\"></td>\n";  
#            print "<td width=\"30%\"><font size=\"2\">\n";
#            print "<b> Z : <input type=\"number\" size=5 value=-195 name=\"QZU\"> (cm)</td>\n";  
#            print "</b></font></tr>\n";
#            htmlTextField("Cos Theta Max ","number",5,0.25,"QCos"," ");
#            @keysa=sort keys %{$ts->{planes}};
#              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Cube Surface Generation : </b></td> <td colspan=1>\n";
#              print "<select name=\"QPlanes\" >\n";
#              print "<option value=\"Any\">  </option>\n";
#              foreach my $surface (@keysa) {
#                print "<option value=\"$surface\">$surface </option>\n";
#              }
#              print "</select>\n";
#              print "</b></td></tr>\n";
#           htmlTableEnd();
      htmlTableEnd();
   print "<p><br>\n";
   print "<b> Print <INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"ALL\" CHECKED> Full Listing\n";
   print "&nbsp;<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"RUNS\"> Only run numbers (or file names);\n";
   print "<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"SUMM\"> Summary \n";
   print "<INPUT TYPE=\"radio\" NAME=\"NTOUT\" VALUE=\"NTCHAIN\"> NTchain <INPUT TYPE=\"text\" name=\"NTCHAIN\">";
   print "</b><p><br>\n";
     print "<p><br>\n";
     print "<input type=\"submit\" name=\"queryDB\" value=\"Submit\">        ";
     print "<input type=\"reset\" name=\"queryDB\" value=\"Reset\"></br><br>        ";
     print "</form>\n";
      print "<tr></tr>\n";
      print "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\" width=\"100%\">\n";
      print "<tr><td valign=\"middle\" bgcolor=\"whitesmoke\"><font size=\"+2\"><B>\n";
      print "Find Job : (eg 805306383 or From-To) </B></font></td></tr></table> \n";
      print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";
      print "<b>JobID : </b> <input type =\"text\" name=\"JobID\">\n";
      print "<input type=\"submit\" name=\"getJobID\" value=\"Submit\"> \n";
      print "</form>\n";
       print "<tr></tr>\n";
       print "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\" width=\"100%\">\n";
       print "<tr><td valign=\"middle\" bgcolor=\"whitesmoke\"><font size=\"+2\"><B>\n";
       print "Find Run : (eg 1073741826  or From-To) </B></font></td></tr></table> \n";
       print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";
       print "<b>RunID : </b> <input type =\"text\" name=\"RunID\">\n";
       print "<input type=\"submit\" name=\"getRunID\" value=\"Submit\"> \n";
       print "</form>\n";
        print "<tr></tr>\n";
        print "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\" width=\"100%\">\n";
        print "<tr><td valign=\"middle\" bgcolor=\"whitesmoke\"><font size=\"+2\"><B>\n";
        print "Find Ntuple(s) : (eg 1073741826  or From-To) </B></font></td></tr></table> \n";
        print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";
        print "<b>RunID : </b> <input type =\"text\" name=\"DSTID\">\n";
        print "<input type=\"submit\" name=\"getDSTID\" value=\"Submit\"> \n";
        print "</form>\n";
  htmlBottom();
  }
 }



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
               print "</b>
                     <INPUT TYPE=\"radio\" NAME=\"CTT\" VALUE=$dataset->{name} $checked>$dataset->{name}<BR>\n";
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

# build up the corr entries in the database
            
# check if cite exist
            my $sql="select cid from Cites where name='$cite'";
            my $ret=$self->{sqlserver}->Query($sql);
            my $cid=$ret->[0][0];
            my $newcite=0;
            if(not defined $cid){
             $newcite=1;
             $sql="select cid from Cites";
             $ret=$self->{sqlserver}->Query($sql);
             $cid=$ret->[$#{$ret}][0]+1;
             if($cid>16){
                 my $error=" Too many Cites. Your request will not be procedeed.";
                 $self->sendmailerror($error,"$cem");
                 $self->ErrorPlus("$error");
             }
             my $run=(($cid-1)<<27)+1;
             my $time=time();
             my $citedesc = "new cite";
             $sql="insert into Cites values($cid,'$cite',0,'remote',$run,0,'$citedesc',$time)";
             $self->{sqlserver}->Update($sql);

            }
            $sql="select mid from Mails";
            $ret=$self->{sqlserver}->Query($sql);
            my $mid=$ret->[$#{$ret}][0]+1;
            my $resp=$newcite;
            my $time=time();
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
             if($cid>16){
                 my $error=" Too many Cites. Your request will not be procedeed.";
                 $self->sendmailerror($error,"$cem");
                 $self->ErrorPlus("$error");
             }
             my $run=(($cid-1)<<27)+1;
             $sql="INSERT INTO Cites VALUES($cid,'$addcite',0,'remote',$run,0,'$newcitedesc',$time)";
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
               my $ntdir="Not Defined";
               my $avail=0;
               my $minspace = 30; # at least 30GB
               foreach my $fs (@{$self->{FilesystemT}}){
                 if ($fs->{available} > $avail and $fs->{available}>$minspace) {
                   $avail = $fs->{available};
                   $ntdir = $fs->{disk}.$fs->{path};
                 }
                } 
               if ($avail > $minspace) {
                $minspace = $avail/2;
                foreach my $fs (@{$self->{FilesystemT}}){
                 if ($fs->{available} > $minspace and $fs->{available} != $avail) {
                   $avail = $fs->{available};
                   $ntdir = $fs->{disk}.$fs->{path};
                   goto DDTAB;
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
            print "<tr><td><b><font color=\"green\">DST file type</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
              print "<tr><td><font size=\"-1\"<b>\n";
              print "<INPUT TYPE=\"radio\" NAME=\"RootNtuple\" VALUE=\"1=3 2=\" CHECKED><b> NTUPLE </b>\n";
              print "<INPUT TYPE=\"radio\" NAME=\"RootNtuple\" VALUE=\"1=0 127=1 128=\" ><b> ROOT </b><BR>\n";
            print "</b></font></td></tr>\n";
           htmlTableEnd();
            print "<tr><td><b><font color=\"green\">DST file wrtie mode</font></b>\n";
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
            print "<tr><td><b><font color=\"green\">Automatic DST files transfer to Server</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
# Apr 1.2003 set default transfer to 'No' for both mode - remote/local
            if ($self->{CCT} eq "remote") {
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"L\" CHECKED><b> No </b><BR>\n";
            } else {
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"R\" ><b> Yes </b><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"L\" CHECKED><b> No </b><BR>\n";
            }
            print "</b></font></td></tr>\n";
           htmlTableEnd();
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
               my $ntdir="Not Defined";
               my $avail=0;
               my $minspace = 30; # at least 30GB
               foreach my $fs (@{$self->{FilesystemT}}){
                 if ($fs->{available} > $avail and $fs->{available}>$minspace) {
                   $avail = $fs->{available};
                   $ntdir = $fs->{disk}.$fs->{path};
                 }
                } 
               if ($avail > $minspace) {
                $minspace = $avail/2;
                foreach my $fs (@{$self->{FilesystemT}}){
                 if ($fs->{available} > $minspace and $fs->{available} != $avail) {
                   $avail = $fs->{available};
                   $ntdir = $fs->{disk}.$fs->{path};
                   goto DDTAB;
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
             $self->htmlTemplateTable("Select  Parameters for Dataset Request");
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
              htmlTextField("CPU clock","number",8,100,"QCPU"," [MHz]");  
            htmlTableEnd();
# Job Parameters
              print "<tr><td><b><font color=\"blue\">Job Parameters</font></b>\n";
              print "</td><td>\n";
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
              $q->param("QEv",0);
              htmlTextField("CPU Time Limit Per Job","number",9,80000,"QCPUTime"," seconds (Native)");  
              htmlTextField("Total Jobs Requested","number",7,5.,"QRun"," ");  
              htmlTextField("Total  Real Time Required","number",3,10,"QTimeOut"," (days)");  
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
               my $avail=0;
               my $minspace = 30; # at least 30GB
               foreach my $fs (@{$self->{FilesystemT}}){
                 if ($fs->{available} > $avail and $fs->{available}>$minspace) {
                   $avail = $fs->{available};
                   $ntdir = $fs->{disk}.$fs->{path};
                 }
                } 
               if ($avail > $minspace) {
                $minspace = $avail/2;
                foreach my $fs (@{$self->{FilesystemT}}){
                 if ($fs->{available} > $minspace and $fs->{available} != $avail) {
                   $avail = $fs->{available};
                   $ntdir = $fs->{disk}.$fs->{path};
                   goto DDTAB;
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
<INPUT TYPE="radio" NAME="RootNtuple" VALUE="1=3 2=" CHECKED>Ntuple
<INPUT TYPE="radio" NAME="RootNtuple" VALUE="1=0 127=1 128=" >RootFile<BR>
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
          print $q->textfield(-name=>"QTimeOut",-default=>7);
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
              return;

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
                        $evno=$tmp->{TOTALEVENTS};
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
        my $b=2147483647000;
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
          if(not $timeout =~/^-?(?:\d+(?:\.\d*)?|\.\d+)$/ or $timeout <1 or $timeout>31){
             $self->ErrorPlus("Time  $timeout is out of range (1,31) days. ");
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
             my $switch=1<<27;
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
        if($#sta<0 or $sta[9]-time() >86400*7 or $stag[9] > $sta[9] ){
           $self->{senddb}=2;
        my $filen="$self->{UploadsDir}/ams02mcdb.tar.$run";
        $key='dbversion';
        $sql="select myvalue from Environment where mykey='".$key."'";
        my $ret=$self->{sqlserver}->Query($sql);
        if( not defined $ret->[0][0]){
            $self->ErrorPlus("unable to retreive db version from db");
        }
        my $dbversion=$ret->[0][0];
        my $i=system "mkdir -p $self->{UploadsDir}/$dbversion";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/*.dat $self->{UploadsDir}/$dbversion";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/t* $self->{UploadsDir}/$dbversion";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/T* $self->{UploadsDir}/$dbversion";
        $i=system "ln -s $self->{AMSDataDir}/$dbversion/ri* $self->{UploadsDir}/$dbversion";
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
          

        if(($#sta<0 or $sta[9]-time() >86400*7  or $stag1[9] > $sta[9] or $stag2[9] > $sta[9]) and $self->{dwldaddon}==1){
           $self->{sendaddon}=2;
        my $filen="$self->{UploadsDir}/ams02mcdb.addon.tar.$run";
        my $i=system "mkdir -p $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/MagneticFieldMap $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Tracker*.2* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Tracker*2 $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Tof* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/.*0 $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/.TrA*1 $self->{UploadsDir}/DataBase"; 
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Ecal* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/TRD* $self->{UploadsDir}/DataBase";
        $i=system "tar -C$self->{UploadsDir} -h -cf $filen DataBase";
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
           $buf=~ s/ROOTNTUPLE=/ROOTNTUPLE=\'$rootntuple\'/;         
           $buf=~ s/TRIGGER=/TRIGGER=$trtype/;         
           $buf=~ s/SETUP=/SETUP=$setup/;         
           $buf=~ s/OUPUTMODE=/OUPUTMODE=$rno/;         
           for my $l (1...6){
               my $cn="COOCUB$l=";
               my $ccc=sprintf("%.3f",$cubes[$l-1]);
               $buf=~ s/$cn/$cn$ccc/;         
           }
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
          my $pminr=$pmin*(1-0.01*$r1);
          my $pmaxr=$pmax*(1+0.01*$r2);
        if($pminr<0.01){
            $pminr=0.01;
        }
        if($pmaxr>21000){
            $pmaxr=21000;
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
          $tmpb=~ s/TIME 3=/TIME 1=$cpusf TIME 3=/; 
         }
         $buf=~ s/PART=/CPUTIME=$cpus \nPART=/; 
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
         if($self->{q}->param("AdvancedQuery") eq "Submit" or $self->{q}->param("BasicQuery") eq "Submit" or $self->{q}->param("ProductionQuery") eq "Submit"){
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
 print FILE "export NtupleDir=$self->{AMSDSTOutputDir} \n";
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
         print FILE $buf;
         print FILE $tmpb;
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
         my $sql="INSERT INTO Jobs VALUES
         ($run,'$script',$self->{CEMID},$self->{CCID},$did,$ctime,$evts,$timeout,'$buf$tmpb',
                   $ctime,'$nickname',' ',' ',0,0,0,0,'$stalone')";
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
             $ri->{cuid}=$ri->{Run};
            } 
            else{
             $ri->{Status}="ToBeRerun";
             $ri->{History}="ToBeRerun";
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
        $self->{sqlserver}->Update($sql);

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
            print "<font color=\"green\"><b><a href=http://pcamsf0.cern.ch/cgi-bin/mon/load.cgi?AMS02MCUploads/Help.txt target=\"_blank\"> ReadMeFirst</a></b></font>\n";
            print "</i></td></tr>\n";
        }

sub htmlReturnToQuery {
            print "<p><tr><td><i>\n";
            print "Return to <a href=$rccgi> MC02 Query Form</a>\n";
            print "</i></td></tr>\n";
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
    my $self = shift;
    my $lastupd=>undef; 
    my $pset=>undef;

    my $sql;
    my $timenow = time();
    $self->htmlTop();
#
# 1st update runs.status from 'Finished'/'Failed' to 'Completed' if 
# at least one DST is copied to final destination 
#
# get production set path
    $sql = "SELECT NAME FROM ProductionSet WHERE STATUS='Active'";
    my $r0 = $self->{sqlserver}->Query($sql);
    if (defined $r0->[0][0]) {
     $pset=trimblanks($r0->[0][0]);
     $sql = "SELECT runs.run FROM runs WHERE runs.status != 'Completed'";
     my $r1=$self->{sqlserver}->Query($sql);
     if( defined $r1->[0][0]){
      foreach my $r (@{$r1}){
       my $run= $r -> [0];
       $sql="SELECT run FROM ntuples WHERE path LIKE '%$pset%' AND run=$run";
       my $r2=$self->{sqlserver}->Query($sql);
       if( defined $r2->[0][0]){
           $sql="UPDATE runs SET runs.status='Completed' WHERE run=$run";
           $self->{sqlserver}->Update($sql); 
       }
      }
     }
    }
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
                WHERE runs.jid = $jid";
         $r4=$self->{sqlserver}->Query($sql); 
         if (defined $r4->[0][0]) {
             $jobstatus = $r4->[0][1];
             if ($jobstatus eq 'Failed'     ||
                 $jobstatus eq "Finished"    ||
                 $jobstatus eq "Foreign"    ||
                 $jobstatus eq "Processing" ||
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
       my $sujet = "Job : $jid - expired";
       my $message    = "Job $jid, Submitted : $submittime, Timeout : $timeout sec. 
                         \n Job will be removed from database (Not earlier than  : $deletetime).
                         \n MC Production Team.";
        $self->sendmailmessage($address,$sujet,$message);

        print "<td><b><font color=\"black\">$cite </font></b></td>";
        print "<td><b><font color=\"black\">$jid </font></b></td>";
        print "<td><b><font color=\"black\">$tsubmit </font></b></td>";
        print "<td><b><font color=\"black\">$texpire </font></b></td>";
        print "<td><b><font color=\"black\">$jobstatus </font></b></td>";
        print "<td><b><font color=\"black\">$owner </font></b></td>";
        print "</tr>\n";
      }
     }
 }

  $self->htmlTableEnd();
 $self->htmlBottom();
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

sub listAllDisks {
    my $self = shift;
    my $show = shift;
    htmlTop();
    $self->ht_init();
    
    $self -> listDisksAMS();

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
   print "&nbsp;<b><a href=\"http://pcamsf0.cern.ch/mm.html\">MC production</a>\n";
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
# spectrum and focusing
#            print "<tr><td><b><font color=\"green\" size=2>Spectrum and Focusing</font></b>\n";
#            print "</td><td>\n";
#            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
#            $ts=$self->{tsyntax};
#            @keysa=sort keys %{$ts->{spectra}};
#              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Input Spectrum : </b></td> <td colspan=1>\n";
#              print "<select name=\"QSpec\" >\n";
#              print "<option value=\"Any\">  </option>\n";
#              foreach my $spectrum (@keysa) {
#                print "<option value=\"$spectrum\">$spectrum </option>\n";
#              }
#              print "</select>\n";
#              print "</b></td></tr>\n";
#           @keysa=sort keys %{$ts->{focus}};
#              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Focusing : </b></td> <td colspan=1>\n";
#              print "<select name=\"QFocus\" >\n";
#              print "<option value=\"Any\">  </option>\n";
#              foreach my $focus (@keysa) {
#                print "<option value=\"$focus\">$focus </option>\n";
#              }
#              print "</select>\n";
#              print "</b></td></tr>\n";
#           htmlTableEnd();
## geo magnetic cutoff
#            print "<tr><td><b><font color=\"magenta\" size=2>GeoMagnetic Cutoff</font></b>\n";
#            print "</td><td>\n";
#            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
#            print "<tr><td><font size=\"-1\"<b>\n";
#             print "<INPUT TYPE=\"radio\" NAME=\"GCF\" VALUE=\"1\" CHECKED><b> Yes </b>\n";
#             print "<INPUT TYPE=\"radio\" NAME=\"GCF\" VALUE=\"0\" ><b> No </b><BR>\n";
#             print "</b></font></td></tr>\n";
#           htmlTableEnd();
## cube coordinates
#            print "<tr><td><b><font color=\"blue\" size=2>Cube Coordinates</font></b>\n";
#            print "</td><td>\n";
#            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
#            print "<tr><td width=\"30%\"><font size=\"2\">\n";
#            print "<b> Min : X : <input type=\"number\" size=5 value=-195 name=\"QXL\"></td>\n";  
#            print "<td width=\"30%\"><font size=\"2\">\n";
#            print "<b> Y : <input type=\"number\" size=5 value=-195 name=\"QYL\"></td>\n";  
#            print "<td width=\"30%\"><font size=\"2\">\n";
#            print "<b> Z : <input type=\"number\" size=5 value=-195 name=\"QZL\"> (cm)</td>\n";  
#            print "</b></font></tr>\n";
#            print "<tr><td width=\"30%\"><font size=\"2\">\n";
#            print "<b> Max : X : <input type=\"number\" size=5 value=195 name=\"QXU\"></td>\n";  
#            print "<td width=\"30%\"><font size=\"2\">\n";
#            print "<b> Y : <input type=\"number\" size=5 value=195 name=\"QYU\"></td>\n";  
#            print "<td width=\"30%\"><font size=\"2\">\n";
#            print "<b> Z : <input type=\"number\" size=5 value=195 name=\"QZU\"> (cm)</td>\n";  
#            print "</b></font></tr>\n";
#            htmlTextField("Cos Theta Max ","number",5,0.25,"QCos"," ");
#            @keysa=sort keys %{$ts->{planes}};
#         print "<tr valign=middle><td align=left><b><font size=\"-1\"> Cube Surface Generation : </b></td> <td colspan=1>\n";
#              print "<select name=\"QFocus\" >\n";
#              print "<option value=\"Any\">  </option>\n";
#              foreach my $surface (@keysa) {
#                print "<option value=\"$surface\">$surface </option>\n";
#              }
#              print "</select>\n";
#              print "</b></td></tr>\n";
#           htmlTableEnd();
#    
      htmlTableEnd();
     print "<p><br>\n";
     print "<FORM METHOD=\"GET\" action=\"/cgi-bin/mon/rc.o.cgi\">\n";
     print "<input type=\"submit\" name=\"queryDB\" value=\"Submit\">        ";
     print "<input type=\"reset\" name=\"queryDB\" value=\"Reset\"></br><br>        ";
     print "</form>\n";
  htmlBottom();
 }
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
      $sql="SELECT MIN(Jobs.timestamp) FROM Jobs, Cites 
                WHERE Jobs.cid=Cites.cid and Cites.name!='test'";
      $ret=$self->{sqlserver}->Query($sql);
      if (defined $ret->[0][0]) {
       $timestart = $ret->[0][0];
      }
# last job timestamp
      $sql="SELECT MAX(Jobs.timestamp) FROM Jobs, Cites 
                 WHERE Jobs.cid=Cites.cid and Cites.name!='test'";
      $ret=$self->{sqlserver}->Query($sql);
      if (defined $ret->[0][0]) {
       $lastupd=localtime($ret->[0][0]);
      }

# running (active jobs)
      $sql = "SELECT COUNT(jobs.jid), SUM(triggers) FROM Jobs, Cites WHERE 
                     (Jobs.cid != Cites.cid AND
                      Cites.cid = (SELECT Cites.cid FROM Cites WHERE Cites.name = 'test'))";
      $ret = $self->{sqlserver}->Query($sql);
      if (defined $ret->[0][0]) {
         $jobsreq = $ret->[0][0];
         $trigreq = $ret->[0][1];
      }
     $sql = "SELECT COUNT(runs.jid) FROM Jobs, Runs, Cites 
              WHERE  runs.jid = jobs.jid AND 
                     (Jobs.cid != Cites.cid AND
                      Cites.cid = (SELECT Cites.cid FROM Cites WHERE Cites.name = 'test'))";
      $ret = $self->{sqlserver}->Query($sql);
      if (defined $ret->[0][0]) {
         $jobsreq = $jobsreq - $ret->[0][0];
      }
    $sql = "SELECT COUNT(runs.jid) FROM Jobs, Runs, Cites 
              WHERE  runs.jid = jobs.jid AND 
                     (runs.status='Foreign' OR runs.status='Processing') AND
                     (Jobs.cid != Cites.cid AND
                      Cites.cid = (SELECT Cites.cid FROM Cites WHERE Cites.name = 'test'))";
    $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
         $jobsreq += $ret->[0][0];
     }

# finished/completed jobs
    $sql = "SELECT COUNT(jid), sum(fevent), sum(levent) FROM Runs 
                WHERE status='Finished' OR status='Completed'";
    $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
         $jobsdone = $ret->[0][0];
         $trigdone = $ret->[0][2] - $ret->[0][1] + $jobsdone
     }

# failed/unchecked jobs
    $sql = "SELECT COUNT(jid) FROM Runs 
                WHERE status='Failed' OR status='Unchecked'";
    $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
         $jobsfailed = $ret->[0][0];
     }

# timeout jobs
    $sql = "SELECT COUNT(jid) FROM Runs WHERE status='TimeOut'";
    $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
         $jobstimeout = $ret->[0][0];
     }

# ntuples, runs
               $sql="SELECT COUNT(run), SUM(SIZEMB) from ntuples";
               $ret=$self->{sqlserver}->Query($sql);
               my $nntuples=0;
               my $nsizegb =0;
               if(defined $ret->[0][0]){
                $nntuples= $ret->[0][0];
                $nsizegb = sprintf("%.1f",$ret->[0][1]/1000);
               }
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
               print "<td align=center><b><font color=\"green\" > $timepassed days</font></b></td>";
              print "</tr>\n";
           my $color="black";
           my $reqevents;
           if ($trigreq < 1000) {
	       $reqevents = $trigreq;
           } elsif ($trigreq => 1000 && $trigreq <= 1000000) {
               $reqevents = sprintf("%.2fK",$trigreq/1000.);
	   } else {
               $reqevents = sprintf("%.2fM",$trigreq/1000000.);
	   }

           my $donevents;
           if ($trigdone< 1000) {
	       $donevents = $trigdone;
           } elsif ($trigdone=> 1000 && $trigdone<= 1000000) {
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


     $sql = "SELECT did, name FROM datasets where did>132";
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
                      Cites.cid = (SELECT Cites.cid FROM Cites WHERE Cites.name = 'test'))";
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
                      Cites.cid = (SELECT Cites.cid FROM Cites WHERE Cites.name = 'test'))";
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
     htmlTable("MC02 Cites");
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     my $sql="SELECT cid,descr, name, status, maxrun FROM Cites ORDER by name";
     my $r3=$self->{sqlserver}->Query($sql);
              print "<td><b><font color=\"blue\">Cite </font></b></td>";
              print "<td><b><font color=\"blue\" >Type </font></b></td>";
              print "<td><b><font color=\"blue\" >Jobs</font></b></td>";
              print "<td><b><font color=\"blue\" >Jobs</font></b></td>";
              print "<td><b><font color=\"blue\" >Jobs</font></b></td>";
              print "<tr>\n";
              print "<td><b><font color=\"blue\">   </font></b></td>";
              print "<td><b><font color=\"blue\" >  </font></b></td>";
              print "<td><b><font color=\"blue\" >Act.</font></b></td>";
              print "<td><b><font color=\"blue\" >Ends</font></b></td>";
              print "<td><b><font color=\"blue\" >Failed</font></b></td>";
              print "<tr>\n";
     print_bar($bluebar,3);
     if(defined $r3->[0][0]){
      foreach my $cite (@{$r3}){
          my $cid    = $cite->[0];
          my $descr  = $cite->[1];
          my $name   = $cite->[2];
          my $status = $cite->[3];
          my $maxrun = $cite->[4];
          my $run=(($cid-1)<<27)+1;

          my $jobs = 0;   # total jobs
          $sql = "SELECT COUNT(jid) FROM Jobs WHERE cid=$cid";
          my $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {$jobs = $r4->[0][0]};

          my $jobsa = 0;  # processing jobs
          $sql = "SELECT COUNT(jobs.jid)  FROM Jobs, Runs where jobs.jid=runs.jid and cid=$cid";
          $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {$jobsa = $jobs - $r4->[0][0]};
          $sql = "SELECT COUNT(Jobs.jid) FROM Jobs, Runs WHERE 
                     Jobs.jid=Runs.jid AND cid=$cid AND 
                     (Runs.Status = 'Foreign' OR 
                      Runs.Status = 'Processing')";
          $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {$jobsa += $r4->[0][0]};

          my $jobsc = 0;
          $sql = "SELECT COUNT(Jobs.jid) FROM Jobs, Runs WHERE 
                     Jobs.jid=Runs.jid AND cid=$cid AND 
                     (Runs.Status = 'Finished' OR 
                      Runs.Status = 'Completed')";
          $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {$jobsc = $r4->[0][0]};


          my $jobsf = 0;
          $sql = "SELECT COUNT(Jobs.jid) FROM Jobs, Runs WHERE 
                     Jobs.jid=Runs.jid AND cid=$cid AND 
                     (Runs.Status = 'Failed' OR 
                      Runs.Status = 'TimeOut' OR 
                      Runs.Status = 'Unchecked')";
          $r4=$self->{sqlserver}->Query($sql);
          if (defined $r4->[0][0]) {$jobsf = $r4->[0][0]};

          print "<tr><font size=\"2\">\n";
          print "<td><b> $descr ($name) </td>
                 <td><b> $status </td>
                 <td><b> $jobsa </td></b>
                 <td><b> $jobsc </b></td>
                 <td><b> $jobsf </b></td>\n";
          print "</font></tr><p></p>\n";
      }
  }

       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
}

sub listDisksAMS {
    my $self = shift;
    my $lastupd=>undef; 
    my $sql;
    $sql="SELECT MAX(timestamp) FROM m_nominal_disks";
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
              print "<td><b><font color=\"blue\" >Used [GB] </font></b></td>";
              print "<td><b><font color=\"blue\" >Free [GB] </font></b></td>";
              print "<td><b><font color=\"blue\" >Status </font></b></td>";
     print_bar($bluebar,3);
     $sql="SELECT host, mntpoint, capacity, occupied, available, status, timestamp 
              FROM m_nominal_disks WHERE checkflag=1 ORDER BY host ";
     my $r3=$self->{sqlserver}->Query($sql);
     if(defined $r3->[0][0]){
      foreach my $dd (@{$r3}){
          my $fs     = $dd->[0].":".$dd->[1];
          my $size   = $dd->[2];
          my $used   = $dd->[3];
          my $avail  = $dd->[4];
          my $status   = $dd->[5];
          print "<tr><font size=\"2\">\n";
          my $color=statusColor($status);
          print "<td><b> $fs </b></td><td align=middle><b> $size </td><td align=middle><b> $used </td><td align=middle><b> $avail </b></td>
                 <td><font color=$color><b> $status </font></b></td>\n";
          print "</font></tr>\n";
      }
  }
    $sql="SELECT SUM(capacity), SUM(occupied), SUM(available) 
          FROM m_nominal_disks WHERE checkflag=1";
    my $r4=$self->{sqlserver}->Query($sql);
    if(defined $r4->[0][0]){
      foreach my $tt (@{$r4}){
          my $total = $tt->[0];
          my $occup = $tt->[1];
          my $free  = $tt->[2];
          my $color="green";
          my $status="ok";
          if ($free < $total*0.1) {
            $color="magenta";
            $status=" no space";}
          print "<tr><font size=\"2\">\n";
          print "<td><font color=$color><b> Total </b></td><td align=middle><b> $total </td><td align=middle><b> $occup </td><td align=middle><b> $free </b></td>
                 <td><font color=$color><b> $status </font></b></td>\n";
          print "</font></tr>\n";
      }
     }
       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     
    print "<TR><TD><font size=\"6\" color=\"red\"><b> Daily Graphs (2 hours average) </b></font></TD></TR>\n";
    print "<P>\n";
     if(defined $r3->[0][0]){
      my $j;
      my $host=>undef;
      my $disk=>undef;
      foreach my $dd (@{$r3}){
          my $fs     = $dd->[0].":".$dd->[1];
          my $size   = $dd->[2];
          my $used   = $dd->[3];
          my $avail  = $dd->[4];
          my $status   = $dd->[5];

          if ($host ne $dd->[0]) {
           if (defined $host) {
            if ($j == 1) {
             print "</TR>\n";
             print "</TABLE>\n";
             print "<TABLE CELLPADDING=0 CELLSPACING=0>\n";
#             print "<TD width=\"500\" ALIGN=left><B><SMALL><FONT COLOR=\"black\">$disk</FONT></SMALL></TD>\n";
             print "</TR>\n";
             print "</TABLE>\n";
            }
           }
           print "<p><B>$dd->[0].cern.ch </B><BR>\n";
           $host=$dd->[0];
           $j=0;
          }

          my $src="http://ams.cern.ch/AMS/daily-graphs".$dd->[1]."-day.gif";
          if ($j==0) {
           print "<TABLE CELLPADDING=0 CELLSPACING=0>\n";
           print "<TR>\n";
           print "<IMG VSPACE=10 WIDTH=500 HEIGHT=135 ALIGN=TOP SRC=$src>\n";
           $j=1;
          } else {
           print "<IMG VSPACE=10 WIDTH=500 HEIGHT=135 ALIGN=TOP ";
           print " SRC=$src\n";
           print "<TD WIDTH=5></TD>\n";
           print "</TR>\n";
           print "</TABLE>\n";
           print "<TABLE CELLPADDING=0 CELLSPACING=0>\n";
           print "<TR>\n";
#           print "<TD width=\"500\" ALIGN=left><B><SMALL><FONT COLOR=\"black\">$disk</FONT></SMALL></TD>\n";
#           print "<TD width=\"500\" ALIGN=left><B><SMALL><FONT COLOR=\"black\">$dd->[1]</FONT></SMALL></TD>\n";
           print "<TD WIDTH=5></TD>\n";
           print "</TR>\n";
           print "</TABLE>\n";
           $j=0;
          }
        $disk = $dd->[1];
      }
            if ($j == 1) {
             print "</TR>\n";
             print "</TABLE>\n";
             print "<TABLE CELLPADDING=0 CELLSPACING=0>\n";
#             print "<TD width=\"500\" ALIGN=left><B><SMALL><FONT COLOR=\"black\">$disk</FONT></SMALL></TD>\n";
             print "</TR>\n";
             print "</TABLE>\n";
            }
     }          
     print "<p></p>\n";
}

sub listDisks {
    my $self = shift;
    my $lastupd=>undef; 
    my $sql;
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
              print "<td><b><font color=\"blue\" >Used [GB] </font></b></td>";
              print "<td><b><font color=\"blue\" >Free [GB] </font></b></td>";
              print "<td><b><font color=\"blue\" >Status </font></b></td>";
     print_bar($bluebar,3);
     $sql="SELECT host, disk, path, totalsize, occupied, available, status, 
           timestamp FROM Filesystems ORDER BY available DESC";
     my $r3=$self->{sqlserver}->Query($sql);
     if(defined $r3->[0][0]){
      foreach my $dd (@{$r3}){
          my $fs     = $dd->[0].":".$dd->[1].$dd->[2];
          my $size   = $dd->[3];
          my $used   = $dd->[4];
          my $avail  = $dd->[5];
          my $status   = $dd->[6];
          print "<tr><font size=\"2\">\n";
          my $color=statusColor($status);
          print "<td><b> $fs </b></td><td align=middle><b> $size </td><td align=middle><b> $used </td><td align=middle><b> $avail </b></td>
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
          if ($free < $total*0.1) {
            $color="magenta";
            $status=" no space";}
          print "<tr><font size=\"2\">\n";
          print "<td><font color=$color><b> Total </b></td><td align=middle><b> $total </td><td align=middle><b> $occup </td><td align=middle><b> $free </b></td>
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
    my $sql="SELECT dbfilename, status, createtime, lastupdate FROM Servers";
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
             if ($time - $lasttime < $srvtimeout) {
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

    print "<b><h2><A Name = \"jobs\"> </a></h2></b> \n";
    htmlTable("MC02 Jobs");
    print_bar($bluebar,3);

    $sql = "SELECT name FROM cites ORDER BY name";
    $ret=$self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
     print " <Table> \n";
     print " <TR><TH rowspan=2>MC Production Cites :<br> \n";
     foreach my $cite (@{$ret}) {
      my $rc = $cite->[0];
      print "</th> <th><small> \n";
      $href = "http://pcamsf0.cern.ch/cgi-bin/mon/rc.dsp.cgi\#jobs-".$rc;
      print "<a href= $href target=\"status\"> <b><font color=blue>$rc</b></font></a>\n";
     }
     print "</TR></TABLE> \n";
    }

    $sql="SELECT Jobs.jid, Jobs.jobname, Jobs.cid, Jobs.mid, 
                     Jobs.time, Jobs.timeout, Jobs.triggers,
                     Cites.cid, Cites.name,
                     Mails.mid, Mails.name
              FROM   Jobs, Cites, Mails
              WHERE  Jobs.cid=Cites.cid AND Jobs.mid=Mails.mid
              ORDER  BY Cites.name, Jobs.jid";
     my $r3=$self->{sqlserver}->Query($sql);
     print_bar($bluebar,3);
     my $newline = " ";
     if(defined $r3->[0][0]){
      foreach my $job (@{$r3}){
          my $jid       = $job->[0];
          my $name      = $job->[1];
          my $starttime = EpochToDDMMYYHHMMSS($job->[4]); 
          my $texpire   = $job->[4] + $job->[5];
          my $expiretime= EpochToDDMMYYHHMMSS($texpire); 
          my $trig      = $job->[6];
          my $cite      = $job->[8];
          my $user      = $job->[10];
          $sql="SELECT status from Runs WHERE jid=$jid";
          $r3=$self->{sqlserver}->Query($sql);
          my $status    = "Submitted";
          my $color     = "black";
          if (defined $r3->[0][0]) {
              $status = $r3->[0][0];
              $color  = statusColor($status);
          }
          if ($newline ne $cite) { 
              $sql="SELECT descr from Cites WHERE name='$cite'";
              $r3=$self->{sqlserver}->Query($sql);
              my $citedescr = "Test Test";
              if (defined $r3->[0][0]) {
               $citedescr = $r3->[0][0];
               $newline = $cite;
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
               print "<td><font color=green size=\"+1\"><b>$citedescr ($cite)</b></font></td>\n";
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
      htmlTableEnd();
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
     print "<tr><font color=blue><b><i> Only recent 100 runs are listed, to get complete list 
            <a href=http://pcamsf0.cern.ch/cgi-bin/mon/rc.o.cgi?queryDB=Form> click here</a>
            </b><i></font></tr>\n";

              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     my $sql="SELECT Runs.run, Runs.jid, Runs.submit, Runs.status 
              FROM   Runs 
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
          if ($rr > 100) { last;}
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
     print "<b><h2><A Name = \"ntuples\"> </a></h2></b> \n";
     print "<TR><B><font color=green size= 5><a href=$validatecgi><b><font color=green> MC NTuples </font></a><font size=3><i> (Click NTuples to validate)</font></i></font>";
     print "<tr><font color=blue><b><i> Only recent 100 files are listed, to get complete list 
            <a href=http://pcamsf0.cern.ch/cgi-bin/mon/rc.o.cgi?queryDB=Form> click here</a>
            </b><i></font></tr>\n";
     print "<p>\n";


     print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
              print "<table border=1 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     my $sql="SELECT Ntuples.run, Ntuples.jid, Ntuples.nevents, Ntuples.neventserr, 
                     Ntuples.timestamp, Ntuples.status, Ntuples.path
              FROM   Ntuples
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
    print "<tr><td width=600>\n";
    print "<table border=\"0\" cellspacing=\"5\" cellpadding=\"5\">\n";
    print "<P>\n";
    print "<p><tr><td bgcolor=\"#ffefd5\" width=600 valign=top colspan=2>\n";
    print "<font face=\"myriad,arial,geneva,helvetica\">\n";
    print "<TABLE BORDER=2 cellpadding=3 cellspacing=3 BGCOLOR=#eed8ae align=center width=100%>\n";
    print "<TR><br>\n";
    print "<TD><font color=#8b1a1a size=\"6\"><b>The following files are avaialable for download</b></font>:\n";
    print "<br><br>\n";
#   print "<br><font size=\"4\"><a href=$self->{UploadsHREF}/$self->{FileDB}>  filedb files (tar.gz)</a></font>";
#ams02mcdb tar
    my $download = 1;
    if (defined $self->{FileDBLastLoad}) {
        if ($self->{FileDBLastLoad} > $self->{FileDBTimestamp}) {
            $download = 0;
        }
    }
     my $file= $self->{FileDB};
    my $dtime = undef;
    if ($download == 1) {
     print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$file>  filedb files (tar.gz)</a>
           </font>";
     $dtime=EpochToDDMMYYHHMMSS($self->{FileDBTimestamp});
     print "<font size=\"3\" color=\"red\"><i><b>       ( Updated : $dtime)</b></i></font>\n";
     print "<br><br>";
    } else {
     print "<br><font size=\"4\">
           filedb files (tar.gz)</a>
           </font>";
     my $dtime=EpochToDDMMYYHHMMSS($self->{FileDBTimestamp});
     print "<font size=\"3\" color=\"green\"><i><b>       ( Up to date : $dtime)</b></i></font>\n";
     print "<br><br>";
   }
   $download = 1;
#ams02mcdbaddon tar
    if (defined $self->{FileAttDBLastLoad}) {
        if ($self->{FileAttDBLastLoad} > $self->{FileAttDBTimestamp}) {
            $download = 0;
        }
    }
    $file= $self->{FileAttDB};
    
    if ($self->{dwldaddon} == 1) {
     if ($download == 1) {
      print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$file>   filedb att.files (tar.gz)</a>
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
#bbftp tar
     my $file= $self->{FileBBFTP};
     print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$file>  bbftp files (tar.gz) - <i> optional </i></a>
           </font>";
     my $dtime=EpochToDDMMYYHHMMSS($self->{FileBBFTPTimestamp});
     print "<font size=\"3\" color=\"green\"><i><b>       ( Updated : $dtime)</b></i></font>\n";
     print "<br><br>";   
#crc tar
     my $file= $self->{FileCRC};
     print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$file>  CRC Linux exec (tar.gz) - <i> optional </i></a>
           </font>";
     my $dtime=EpochToDDMMYYHHMMSS($self->{FileCRCTimestamp});
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
      print "&nbsp;<b><a href=\"http://pcamsf0.cern.ch/mm.html\">MC production</a>\n";
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
    my $color  = "magenta";

    if ($status eq "Finished" or $status eq "OK" or $status eq "Validated" or $status eq "Completed") {
               $color  = "green";
    }
    elsif ($status eq "TimeOut") {
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
    else {
               $color = "red";
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


    if( not $self->Init()){
        die "parseJournalFiles -F- Unable To Init";
        
    }
    if (not $self->ServerConnect()){
        die "parseJournalFiles -F- Unable To Connect To Server";
    }


 htmlTop();
 htmlTable("Parse Journal Files");

 my $sql = "SELECT dirpath,journals.timestamp,name,journals.cid  
              FROM journals,cites WHERE journals.cid=cites.cid";

 my $cid = undef;
 my $ret = $self->{sqlserver}->Query($sql);

 if(defined $ret->[0][0]){
  foreach my $jou (@{$ret}){
   my $timenow    = time();
   my $dir        = trimblanks($jou->[0]);
   my $timestamp  = trimblanks($jou->[1]);
   my $cite       = trimblanks($jou->[2]);
      $cid        = $jou->[3];
   my $lastcheck  = EpochToDDMMYYHHMMSS($timestamp);
   my $title  = "Cite : ".$cite.", Directory : ".$dir." Last Check ".$lastcheck;
   htmlTable($title);
   my $newfile   = "./";
   my $lastfile  = "./";
   my $writelast = 0;
      my $logdir = $dir."/log";
      my $joudir = $dir."/jou";
      my $ntdir  = $dir."/nt";
      opendir THISDIR ,$joudir or die "unable to open $joudir";
      my @allfiles= readdir THISDIR;
      closedir THISDIR;
      print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
      print "<td><b><font color=\"blue\">File </font></b></td>";
      print "<td><b><font color=\"blue\" >Write Time</font></b></td>";
      print "<td><b><font color=\"blue\" >Status</font></b></td>";
      print "</tr>\n";
      foreach my $file (@allfiles) {
       if ($file =~/^\./){
         next;
       }
       $newfile=$joudir."/".$file;
       my $writetime = (stat($newfile)) [9];
       if ($writetime > $writelast) {
           $writelast = $writetime;
           $lastfile = $newfile;
       }
       my $color   ="black";
       my $fstatus ="CheckInProgress";
       if ($writetime < $timestamp) { $color   = "magenta";
                                      $fstatus = "AlreadyChecked"}
       my $wtime = EpochToDDMMYYHHMMSS($writetime);
       print "<td><b><font color=$color>$file </font></b></td>";
       print "<td><b><font color=$color >$wtime</font></b></td>";
       print "<td><b><font color=$color >$fstatus</font></b></td>";
       print "</tr>\n";
       if ($writetime > $timestamp) {
         $self->parseJournalFile($logdir,$newfile,$ntdir);
       }
   }
   htmlTableEnd();
   htmlTableEnd();
   if (defined $cid) {
    $sql = "UPDATE journals SET timestamp=$timenow, lastfile = '$lastfile' 
            WHERE cid=$cid";
    $self->{sqlserver}->Update($sql); 
   }
  }
 } else {
     print "Warning - table Journals is empty \n";
 }

   htmlTableEnd();
 htmlBottom();
}


sub parseJournalFile {
#
# to do : check run with DBserver
#         copy to final destination
#

 my $self      = shift;
 my $logdir    = shift;
 my $inputfile = shift;
 my $dirpath   = shift;


 my $jobnotfound  = 0;       # check that job exists in database
                             # quit if not
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

my $timestamp = 0;    # unix time 
my $lastjobid = 0;

my $startingjobR = 0; # StartingJob record found and parsed
my $startingrunR = 0; # StartingRun "     "       "       "
my $opendstR     = 0; # OpenDST     "     "       "       "
my $closedstR    = 0; # CloseDST    "     "       "       "
my $runfinishedR = 0; # RunFinished "     "       "       "
my $status = undef;   # Run status


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

use POSIX  qw(strtod);             

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
     $sql = "UPDATE Jobs SET EVENTS=$runincomplete[3], ERRORS=$runincomplete[5], 
                                   CPUTIME=$cputime, ELAPSED=$elapsed,
                                   HOST='$runincomplete[1]', TIMESTAMP = $timestamp 
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
# , JobStarted HostName gcie01 default , UID 805306385 , PID 3578 3573 , Type Producer , ExitStatus NOP , 
#   StatusType OneRunOnly
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
       $jobnotfound = 1;
       print FILE "Fatal - cannot find JobInfo for $jobid \n";
       system("mv $inputfile $inputfile.0"); 
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
      $patternsmatched = 0;
      my @StartingJobPatterns = ("JobStarted", "HostName","UID","PID","Type",
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
       $jobnotfound = 1;
       print FILE "Fatal - cannot find JobInfo for $jobid \n";
       system("mv $inputfile $inputfile.0"); 
       return;
   }
   if ($patternsmatched == $#StartingJobPatterns) { 
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
                                 "Prio","Path","Status","History","ClientID",
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


   
   if ($patternsmatched == $#StartingRunPatterns+4) { # Ignore one 'Run' 
    $run = $startingrun[2];
    $startingrun[0] = "StartingRun";
    $startingrunR   = 1;
    $timestamp = time();
    $self->insertRun(
             $startingrun[2],
             $lastjobid,
             $startingrun[3],
             $startingrun[4],
             0,
             0,
             $startingrun[11],
             $startingrun[7]);

     $sql = "UPDATE Jobs SET 
                 host='$startingrun[12]',
                 events=$startingrun[13], errors=$startingrun[15],
                 cputime=$startingrun[16], elapsed=$startingrun[17],
                 timestamp=$timestamp 
                where jid=$lastjobid";
     print FILE "$sql \n";
     $self->{sqlserver}->Update($sql);
   } else  {
       print FILE "StartingRun - cannot find all patterns $patternsmatched/$#StartingRunPatterns \n";
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
         if ($jj[0] eq $OpenDSTPatterns[$j]) {
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
    my @CloseDSTPatterns = ("CloseDST","Status","Type","Name","Version",
                            "Size","CRC","Insert","Begin","End",
                            "Run","FirstEvent","LastEvent","EventNumber","ErrorNumber");
     for (my $i=0; $i<$#junk+1; $i++) { 
      my @jj = split " ",$junk[$i];
      if ($#jj > 0) {
       my $found = 0;
       my $j     = 0;
       while ($j<$#CloseDSTPatterns+1 && $found == 0) { 
        if ($jj[0] eq $CloseDSTPatterns[$j]) {
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
# find ntuple
#
    my @junk = split "/",$closedst[3];
    my $dstfile = trimblanks($junk[$#junk]);
    my $filename= $dstfile;
    $dstfile=$dirpath."/".$dstfile;
    my $dstsize = -1;
    $dstsize = (stat($dstfile)) [7] or $dstsize = -1;
    if ($dstsize == -1) {
     print FILE "parseJournalFile -W- CloseDST block : cannot stat $dstfile \n";
     $dstsize = -1;
     $copyfailed = 1;
     last;
    } else {
     $dstsize = sprintf("%.1f",$dstsize/1000/1000);
     $closedst[0] = "CloseDST";
     $timestamp = time();
#- mv to insertNtuple 
#     my $sql = "SELECT run, path FROM ntuples 
#                   WHERE run=$closedst[10] AND path like '%$filename%'";
#     my $ret = $self->{sqlserver}->Query($sql);
#     if(not defined $ret->[0][0]){
#-
      print FILE "$dstfile.... \n";
      my $badevents=$closedst[14];
      my $ntevents =$closedst[13];
      my $ntstatus ="OK";                     
      my $run      =$closedst[10];
      my $jobid    =$closedst[10];
      my $ntcrc    =$closedst[6];
      my $nttype   =$closedst[2];

      $levent += $closedst[12]-$closedst[11]+1;
      my $i = 0;
      my $crccmd = "$self->{AMSSoftwareDir}/exe/linux/crc $dstfile $ntcrc";
      $i=system($crccmd);
      $i=($i>>8);
      print FILE "$crccmd : Status : $i \n";
      if ($i != 1) {
          $unchecked++;
          $copyfailed = 1;
          last;
      }
      $i=0;
      if ($nttype eq 'Ntuple') {
        my $validatecmd = 
           "$self->{AMSSoftwareDir}/exe/linux/fastntrd.exe  $dstfile $closedst[13]";
        $i=system($validatecmd);
        print FILE "$validatecmd : Status : $i \n";
    }
       if( ($i == 0xff00) or ($i & 0xff)){
       if($closedst[1] ne "Validated"){
         $ntstatus="Unchecked";                     
         $ntevents=$closedst[13];
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
           $levent -= $closedst[12]-$closedst[11]+1;
       }
         else{
          $ntstatus="OK";
          $ntevents=$closedst[13];
          $badevents=int($i*$closedst[13]/100);
          $validated++;
          my ($outputpath,$rstatus) = $self->doCopy($jobid,$dstfile,$ntcrc);
          if(defined $outputpath){
             push @mvntuples, $outputpath; 
           }
          if ($rstatus == 1) {
           $self->insertNtuple(
                               $run,
                               $closedst[4],
                               $nttype,
                               $jobid,
                               $closedst[11],
                               $closedst[12],
                               $ntevents,
                               $badevents,
                               $timestamp,
                               $dstsize,
                               $closedst[1],
                               $outputpath,
                               $ntcrc); 

           print FILE "insert ntuple : $run, $outputpath \n";
          push @cpntuples, $dstfile;
        }
       }
      }
#     } else {
#        print FILE "parseJournalFile -W- CloseDST: ntuple exist - $closedst[3]\n";
#     }
     }
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
    $sql = "update jobs set events=$runfinished[3], errors=$runfinished[5], 
                                   cputime=$cputime, elapsed=$elapsed,
                                   host='$runfinished[1]',timestamp = $timestamp 
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
  }
 }

 if ($startingrunR == 1 || $runfinishedR == 1) {
  $status="Failed";
  my $inputfileLink = $inputfile.".0";
  if ($copyfailed == 0) {
    foreach my $ntuple (@cpntuples) {
      my $cmd="rm  $ntuple";
      print FILE "$cmd\n";
      system($cmd);
      print FILE "Validation done : system command rm -i $ntuple \n";
  }
  if ($#cpntuples >= 0) { 
    $status = 'Completed';
    $inputfileLink = $inputfile.".1";
   }
 }
  else{
   print FILE "Validation/copy failed : Run =$run \n";
   $status='Unchecked';
   foreach my $ntuple (@mvntuples) {
     my $cmd = "rm  $ntuple";
     print FILE "Validation failed : system command rm -i $ntuple \n";
     print FILE "$cmd\n";
     system($cmd);
    }
    $sql = "DELETE ntuples WHERE run=$run";
    $self->{sqlserver}->Update($sql);
    print FILE "$sql \n";
    $runupdate = "UPDATE runs SET ";
    $sql = "SELECT dirpath FROM journals WHERE cid=-1";
    my $ret=$self->{sqlserver}->Query($sql);
     if( defined $ret->[0][0]){
      my $junkdir = $ret->[0][0];   
      foreach my $ntuple (@cpntuples) {
       my $cmd="mv  $ntuple $junkdir/";
       print FILE "$cmd\n";
       system($cmd);
      }
      print FILE "Validation/copy failed : mv ntuples to $junkdir \n";
    }
}
  $sql = $runupdate." STATUS='$status' WHERE run=$run";
  $self->{sqlserver}->Update($sql);
  print FILE "Update Runs : $sql \n";
  system("mv $inputfile $inputfileLink"); 
  print FILE "mv $inputfile $inputfileLink\n";
  if ($status eq "Completed") {
    $self->updateRunCatalog($run);
    print "Update RunCatalog table : $run\n";
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

    $sql = "SELECT runs.status FROM Runs WHERE runs.jid=$jid";
    my $r0 = $self->{sqlserver}->Query($sql);
    if(defined $r0->[0][0]){
      $runstatus = $r0->[0][0];
      if ($runstatus eq 'Completed') {
          $sql = "SELECT run FROM runcatalog WHERE run=$jid";
          my $r1 = $self->{sqlserver}->Query($sql);
          if(not defined $r1->[0][0]){
           $sql = "SELECT content FROM Jobs WHERE jid=$jid";
           my $r2 = $self->{sqlserver}->Query($sql);
           if(defined $r2->[0][0]){
            $jobcontent = $r2->[0][0];       
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
                 $sql1=$sql1.", 'AMSParticle'";
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
           } else {
            htmlWarning("updateRunCatalog","Cannot find content for Job=$jid");
           }
          } else {
#           htmlWarning("updateRunCatalog","Run = $jid exists in RunCatalog. No Update");
          }
      } else {
        htmlWarning("updateRunCatalog","Run = $jid, Status = $runstatus. No Update");
      }     
    } else {
       htmlWarning("updateRunCatalog","Cannot Find Run with JID = $jid");
    }
}

sub deleteTimeOutJobs {

    my $self = shift;

    my $vdir = undef;
    my $vlog = undef;
    my $timenow = time();

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
    $self->htmlTop();

# get list of runs from Server
    if( not defined $self->{dbserver}->{rtb}){
      DBServer::InitDBFile($self->{dbserver});
    }
#
    htmlTable("Jobs below will be deleted from the database");

    my $sql  = undef;
    $sql="SELECT jobs.jid, jobs.timestamp, jobs.timeout, jobs.mid, jobs.cid, 
                 cites.name, mails.name FROM jobs, cites, mails, runs  
               WHERE Jobs.jid = Runs.jid AND  
                     Jobs.cid = Cites.cid AND 
                     Jobs.mid = Mails.mid AND 
                     Runs.status = 'TimeOut'";
    my $ret = $self->{sqlserver}->Query($sql);
    if (defined $ret->[0][0]) {
     print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     print "<td><b><font color=\"blue\">Cite </font></b></td>";
     print "<td><b><font color=\"blue\" >JobID </font></b></td>";
     print "<td><b><font color=\"blue\" >Submitted</font></b></td>";
     print "<td><b><font color=\"blue\" >Expired</font></b></td>";
     print "<td><b><font color=\"blue\" >Status</font></b></td>";
     print "<td><b><font color=\"blue\" >Owner</font></b></td>";
     print "</tr>\n";
     print_bar($bluebar,3);
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
              print FILE "*TRY* send Delete to DBServer, run=$runinfo->{Run}\n";
              last;
             }
            }
        print "<td><b><font color=\"black\">$cite </font></b></td>";
        print "<td><b><font color=\"black\">$jid </font></b></td>";
        print "<td><b><font color=\"black\">$tsubmit </font></b></td>";
        print "<td><b><font color=\"black\">$texpire </font></b></td>";
        print "<td><b><font color=\"red\">TimeOut </font></b></td>";
        print "<td><b><font color=\"black\">$owner </font></b></td>";
        print "</tr>\n";
 
        }
      htmlTableEnd();
    }


#
# delete jobs without runs
#

    my $timedelete = time() - 2*60*60;
    $sql="SELECT jobs.jid, jobs.timestamp, jobs.timeout, jobs.mid, jobs.cid, 
                 cites.name FROM jobs, cites 
          WHERE jobs.timestamp+jobs.timeout < $timedelete  AND 
                jobs.cid=cites.cid"; 
    my $r3=$self->{sqlserver}->Query($sql);
    if( defined $r3->[0][0]){
      print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
      print "<td><b><font color=\"blue\">Cite </font></b></td>";
      print "<td><b><font color=\"blue\" >JobID </font></b></td>";
      print "<td><b><font color=\"blue\" >Submitted</font></b></td>";
      print "<td><b><font color=\"blue\" >Expired</font></b></td>";
      print "<td><b><font color=\"blue\" >Owner</font></b></td>";
      print "</tr>\n";
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


        print "<td><b><font color=\"black\">$cite </font></b></td>";
        print "<td><b><font color=\"black\">$jid </font></b></td>";
        print "<td><b><font color=\"black\">$tsubmit </font></b></td>";
        print "<td><b><font color=\"black\">$texpire </font></b></td>";
        print "<td><b><font color=\"red\">TimeOut </font></b></td>";
        print "</tr>\n";
       }
     }
      htmlTableEnd();
    htmlTableEnd();
   }


    htmlTableEnd();
    $self->htmlBottom();
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
        print "$sql \n";
        if ($status eq "Completed") {
          $self->updateRunCatalog($run);
          print "Update RunCatalog table : $run->{Run}\n";
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

sub insertNtuple {
  my $self     = shift;
#
  my $run      = shift;
  my $version  = shift;
  my $type     = shift;
  my $jid      = shift;
  my $fevent   = shift;
  my $levent   = shift;
  my $events   = shift;
  my $errors   = shift;
  my $timestamp= shift;
  my $ntsize   = shift;
  my $status   = shift;
  my $path     = shift;
  my $crc      = shift;
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
                                           $crc)"; 
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
