# $Id: RemoteClient.pm,v 1.85 2003/04/07 10:50:23 choutko Exp $
#
# Apr , 2003 . ak. Default DST file transfer is set to 'NO' for all modes
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
@RemoteClient::EXPORT= qw(new  Connect Warning ConnectDB listAll listAllDisks listMin queryDB DownloadSA);

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

my     $rchtml='http://ams.cern.ch/AMS/Computing/mcproduction/rc.html';
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
        FileAttDB=>undef,
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
             $sql="insert into DataSets values($did,'$dataset->{name}')";
             $self->{sqlserver}->Update($sql); 
           }
          
           if($sbuf[0] =~/^#/ && defined $template->{initok}){

            $buf=~ s/#/C /;
            $template->{filebody}=$buf;
            my $desc=$sbuf[0];
            substr($desc,0,1)=" ";
            $template->{filedesc}=$desc." Total Events Left $template->{TOTALEVENTS}";
           push @{$dataset->{jobs}}, $template; 
           }        
       }        
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
       my $failed=1;
       $ref->{dbfile}=undef;
       if($ref->{CSR}){
        $failed=$ref->RestartServer();
       }

       if($failed ne 0){
           $ref->ErrorPlus("Unable To Connect to Server");
       }
         else{
             $ref->ErrorPlus("Attempt to Restart Server Has Been Made.\n Please check bjobs -q linux_server -u all in few minutes");
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
#           my $i=system("$submit -B$self->{dbfile}" );
              my $i=system("chmod +x $full");
           return $i;
          }
         } 
         return 1;
     }

sub Validate{
   my $self = shift;

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
         if(not $found){
             my $status=$run->{Status};
             $sql="insert into Runs values($run->{Run},$run->{Run},$run->{FirstEvent},$run->{LastEvent},$run->{TFEvent},$run->{TLEvent},$run->{SubmitTime},'$status')";
             $self->{sqlserver}->Update($sql);
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
                  $sql="insert into Ntuples values($run->{Run},'$ntuple->{Version}','$ntuple->{Type}',$run->{Run},$ntuple->{FirstEvent},$ntuple->{LastEvent},$events,$badevents,$ntuple->{Insert},$ntuple->{size},'$status','$ntuple->{Name}',$ntuple->{crc})";
                  $self->{sqlserver}->Update($sql);
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
# db query
    if ($self->{q}->param("queryDB")) {
     $self->{read}=1;
     if ($self->{q}->param("queryDB") eq "Submit") {
        $sql="SELECT run, particle FROM runscat, Jobs WHERE ";
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
        $self->htmlTemplateTable("Selected Query Keys :");
        my $qtempany = 0;
        if (not defined $q->param("QTemp") or $q->param("QTemp") eq "Any") {
         $qtempany = 1;
        }
        if (defined $q->param("QTempDataset") and $q->param("QTempDataset") ne "Any") { $qtempany = 0;}

        if (defined $q->param("QTemp") and $q->param("QTemp") ne "Any") {
         $file = $q->param("QTemp");
         $qtempany = 0;
         $sql = "SELECT run, particle FROM runscat, Jobs WHERE jid=runscat.run AND jobname LIKE '%.$file' AND ";
         print "<tr><td><font size=\"3\" color=\"red\"><b>Job Template File :</b></td><td><b> $file</b></td></tr>\n";
        }
        if (not defined $file) {
         if (defined $q->param("QTempDataset") and $q->param("QTempDataset") ne "Any") {
          $dataset = $q->param("QTempDataset");
          $color="blue";
          print "<tr><td><font size=\"3\" color=$color><b>Job Template Dataset :</b></td><td><b> $dataset</b></td></tr>\n";
         }
        }
        if (defined $q->param("QNick") and $q->param("QNick") ne "Any") {
         $nickname = $q->param("QNick");
         print "<tr><td><font size=\"3\" color=$color><b>Job Nickname </b></td><td><b>  $nickname</b></td></tr>\n";
         $sql = $sql." Jobs.NICKNAME = '$nickname' AND ";
        }
        if (defined $q->param("QPart") and $q->param("QPart") ne "Any") {
         $particle = $q->param("QPart");
         print "<tr><td><font size=\"3\" color=$color><b>Particle : </b></td><td><b> $particle</b></td></tr>\n";
         my $particleid=$self->{tsyntax}->{particles}->{$particle};
         $sql=$sql."PARTICLE=".$particleid." AND ";
      }
        if (defined $q->param("QMomI")) {
         $qmomi = $q->param("QMomI");
         print "<tr><td><font size=\"3\" color=$color><b>Momentum min [Gev/c] </b></td><td><b> >= $qmomi</b></td></tr>\n";
         $sql=$sql." ENERGYMIN >= $qmomi AND ";
      }
        if (defined $q->param("QMomA")) {
         my $qmoma = $q->param("QMomA");
         print "<tr><td><font size=\"3\" color=$color><b>Momentum max [Gev/c] </b></td><td><b> =< $qmoma</b></td></tr>\n";
         $sql=$sql." ENERGYMAX <= $qmoma AND ";
     }
      if (defined $dataset or $qtempany == 1) {
# advanced query options
        if (defined $q->param("QSetup")) {
         $setup = $q->param("QSetup");
         print "<tr><td><font size=\"3\" color=$color><b>Setup : </b></td><td><b> $setup</b></td></tr>\n";
         $sql=$sql." SETUP = '$setup'  AND ";
     }
        if (defined $q->param("QTrType")) {
         $trtype = $q->param("QTrType");
         print "<tr><td><font size=\"3\" color=$color><b>Trigger Type : </b></td><td><b> $trtype</b></td></tr>\n";
         $sql=$sql." TRTYPE= '$trtype' AND ";
      }
        if (defined $q->param("QSpec") and $q->param("QSpec") ne "Any" ) {
         $spectrum = $q->param("QSpec");
         print "<tr><td><font size=\"3\" color=$color><b>Spectrum : </b></td><td><b> $spectrum</b></td></tr>\n";
         $sql=$sql." SPECTRUM='$spectrum' AND ";
      }
        if (defined $q->param("QFocus") and $q->param("QFocus") ne "Any") {
         $focus = $q->param("QFocus");
         print "<tr><td><font size=\"3\" color=$color><b>Focusing : </b></td><td><b> $focus</b></td></tr>\n";
      }
        if (defined $q->param("GCF")) {
         my $cut = "No";
         $cutoff = $q->param("GCF");
         if ($cutoff == 1) {
           $cut = "Yes"; 
           $sql = $sql." CUTOFF=1 AND ";
        } else {
           $sql = $sql." CUTOFF=0 AND ";
        }
         print "<tr><td><font size=\"3\" color=$color><b>Geomagnetic cutoff : </b></td><td><b> $cut</b></td></tr>\n";
     }
       if (defined $q->param("QTempDataset") and $q->param("QTempDataset") ne "Any") {
        if (defined $q->param("QXL") and defined $q->param("QYL") and defined $q->param("QZL")) {
         if (defined $q->param("QXU") and defined $q->param("QYU") and defined $q->param("QZU")) {
             $qxl=$q->param("QXL");
             $qyl=$q->param("QYL");
             $qzl=$q->param("QZL");
             $qxu=$q->param("QXU");
             $qyu=$q->param("QYU");
             $qzu=$q->param("QZU");
             $sql=$sql." XL=$qxl AND YL=$qyl AND ZL=$qzl AND XU=$qxu AND YU=$qyu AND ZU=$qzu AND ";
            print "<tr><td><b><font color=$color size=3>Cube Coordinates</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td width=\"30%\"><font size=\"2\">\n";
             print "<b> Min : X =$qxl </td><td width=\"30%\"><font size=\"2\"><b> Y =$qyl</td>\n";  
             print "<td width=\"30%\"><font size=\"2\"><b> Z =$qzl (cm)</td>\n";  
             print "</b></font></tr>\n";
             print "<tr><td width=\"30%\"><font size=\"2\">\n";
             print "<b> Max : X =$qxu </td><td width=\"30%\"><font size=\"2\"><b> Y =$qyu</td>\n";  
             print "<td width=\"30%\"><font size=\"2\"><b> Z =$qzu (cm)</td>\n";  
             print "</b></font></tr>\n";
            htmlTableEnd();
        }
     }
        if (defined $q->param("QCos")) {
         $qcos = $q->param("QCos");
         print "<tr><td><font size=\"3\" color=$color><b>Cos Theta Max : </b></td><td><b> $qcos</b></td></tr>\n";
         $sql=$sql."COSTHETA=$qcos AND ";
      }
     }
        if (defined $q->param("QPlanes") and $q->param("QPlanes") ne "Any") {
         $qplanes = $q->param("QPlanes");
         print "<tr><td><font size=\"3\" color=$color><b>Cube Surface Generation : </b></td><td><b> $qplanes</b></td></tr>\n";
         $sql=$sql."SURFACE=$qplanes AND ";
     }

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
        $sql=$sql." runscat.TIMESTAMP != 0 ORDER BY particle, run ";
      htmlTableEnd();
#                  RUN          NUMBER       NOT NULL
#  $particle       PARTICLE     NUMBER(10)
#  $qmomi          energymin    NUMBER(24)
#  $qmoma          energymax    NUMBER(24)
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
       my $ret=$self->{sqlserver}->Query($sql);
       if (defined $ret->[0][0]) {
         my $newchain = 0;
         foreach my $r (@{$ret}){
             $totaljobs++;
             my $run=$r->[0];
             my $particleid=$r->[1];
             my $part=>undef;
             $sql="SELECT jobname, runs.submit FROM Jobs, Runs WHERE Jobs.jid=Runs.jid AND Runs.RUN=$run";
             my $r3=$self->{sqlserver}->Query($sql);
             if (defined $r3->[0][0]) {
              my $ts=$self->{tsyntax};
              my %hash=%{$ts->{particles}};
              my @keysa=sort {$hash{$a} <=>$hash{$b}} keys %{$ts->{particles}};
              foreach my $particle (@keysa){
                if ($particleid eq $self->{tsyntax}->{particles}->{$particle}) {
                    $part=$particle;
                    goto PART;
              }
            }
PART:       
              my $color = Color($particleid);
              my $jobname=>undef;
              my $submittime=>undef;
             
              foreach my $job (@{$r3}) {
                  $jobname   = $job->[0];
                  my $submit = $job->[1];
                  $submittime= localtime($submit);
              }
              if ($ntout eq "ALL" or $ntout eq "RUNS") {
               print "<tr><td><b><font size=\"4\" color=$color> Job : <i>$jobname </i>, Run : <i>$run </i>, Particle : <i> $part </i>,  Submitted : <i> $submittime </i></font></b></td></tr>\n";
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
      }   
         print "<BR><BR>\n";
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
    print "<tr><td><b><font size=\"4\" color=\"green\"><ul>  Jobs Submitted : $totaljobs </b></font></td></tr>\n";
    print "<tr><td><b><font size=\"4\" color=\"green\"><ul>  $from / $to </b></font></td></tr>\n";
    print "<tr><td><b><font size=\"4\" color=\"blue\"><ul>   NTuples : $totalnt  </b></font></td></tr>\n";
    print "<tr><td><b><font size=\"4\" color=\"blue\"><ul>   Events : $totalev  </b></font></td></tr>\n";
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
   print "<li> To browse AMS01 data and AMS02 NTuples produced before March 2002 click <a href=$dbqueryR> here </a>\n";
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
          print "<tr valign=middle><td align=left><b><font size=\"-1\"> File : </b></td> <td colspan=1>\n";
          print "<select name=\"QTemp\" >\n";
          print "<option value=\"Any\">  </option>\n";
          foreach my $template (@tempnam) {
            print "<option value=\"$template\">$template </option>\n";
          }
          print "</select>\n";
          print "</b></td></tr>\n";
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
            print "<tr valign=middle><td align=left><b><font color=\"tomato\" size=\"-1\"> Dataset :</b></td><td colspan=1>\n";
            print "<select name=\"QTempDataset\" >\n";
            print "<option value=\"Any\">  </option>\n";
            my $i=0;
            foreach my $template (@tempnam) {
             print "<option value=\"$template\">$desc[$i] </option>\n";
             $i++;
            }
            print "</select>\n";
            htmlTextField("Job nickname","text",80,'Any',"QNick"," ");  
            print "</b></td></tr>\n";
        htmlTableEnd();
# Run Parameters
          print "<tr><td><b><font color=\"green\" size=2>Run Parameters</font></b>\n";
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
            htmlTextField("Momentum max =<","number",7,200.,"QMomA","[GeV/c]");  
            htmlTextField("Setup","text",20,"AMS02","QSetup"," ");
            htmlTextField("Trigger Type ","text",20,"AMSParticle","QTrType"," ");
           htmlTableEnd();
# spectrum and focusing
            print "<tr><td><b><font color=\"blue\" size=2>Spectrum and Focusing</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            $ts=$self->{tsyntax};
            @keysa=sort keys %{$ts->{spectra}};
              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Input Spectrum : </b></td> <td colspan=1>\n";
              print "<select name=\"QSpec\" >\n";
              print "<option value=\"Any\">  </option>\n";
              foreach my $spectrum (@keysa) {
                print "<option value=\"$spectrum\">$spectrum </option>\n";
              }
              print "</select>\n";
              print "</b></td></tr>\n";
           @keysa=sort keys %{$ts->{focus}};
              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Focusing : </b></td> <td colspan=1>\n";
              print "<select name=\"QFocus\" >\n";
              print "<option value=\"Any\">  </option>\n";
              foreach my $focus (@keysa) {
                print "<option value=\"$focus\">$focus </option>\n";
              }
              print "</select>\n";
              print "</b></td></tr>\n";
           htmlTableEnd();
# geo magnetic cutoff
            print "<tr><td><b><font color=\"blue\" size=2>GeoMagnetic Cutoff</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"GCF\" VALUE=\"1\" CHECKED><b> Yes </b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"GCF\" VALUE=\"0\" ><b> No </b><BR>\n";
             print "</b></font></td></tr>\n";
           htmlTableEnd();
# cube coordinates
            print "<tr><td><b><font color=\"blue\" size=2>Cube Coordinates</font></b>\n";
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
              print "<option value=\"Any\">  </option>\n";
              foreach my $surface (@keysa) {
                print "<option value=\"$surface\">$surface </option>\n";
              }
              print "</select>\n";
              print "</b></td></tr>\n";
           htmlTableEnd();
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
            print "<b><font color=\"magenta\" size=\"3\">User's Info </font><font size=\"2\"> (if in <i>italic</i> then cannot be changed)</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
            print "<i><b>e-mail : </td><td><input type=\"text\" size=24 value=$cem name=\"CEM\" onFocus=\"this.blur()\" ></i>\n";
            print "</b></font></td></tr>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
            print "<i><b>cite : </td><td><input type=\"text\" size=18 value=$cite name=\"CCA\" onFocus=\"this.blur()\"></i>\n";
            print "</b></font></td></tr>\n";
            print "</TABLE>\n";
            print "<tr><td>\n";
            print "<b><font color=\"red\" size=\"3\">Template</font></b>\n";
            print "</td><TD>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
            print "<INPUT TYPE=\"radio\" NAME=\"CTT\" VALUE=\"Basic\" CHECKED>Basic<BR>\n";
            print "<INPUT TYPE=\"radio\" NAME=\"CTT\" VALUE=\"Advanced\" >Advanced<BR>\n";
            print "</b></font></td></tr>\n";
            print "</TABLE>\n";
             print "<tr><td><b><font color=\"blue\" size=\"3\">Datasets</font></b>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             foreach my $dataset (@{$self->{DataSetsT}}){
              print "<INPUT TYPE=\"radio\" NAME=\"CTT\" VALUE=$dataset->{name} >$dataset->{name}<BR>\n";
             print "</b></font></td></tr>\n";
          }
           print "</TABLE>\n";
          print "</TABLE>\n";
          print "<p>\n";
          print "<br>\n";
          print "<input type=\"submit\" name=\"FormType\" value=\"Continue\">        ";
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
             $self->htmlTemplateTable("Select Basic Template File and Run Parameters");
# print templates
             my @tempnam=();
             my $hash={};
             foreach my $cite (@{$self->{TempT}}){
              if(not ($cite->{filename} =~/^\./)){
               push @tempnam, $cite->{filename};
               $hash->{$cite->{filename}}=$cite->{filedesc};
             }
          }
          print "<tr><td><b><font color=\"red\">Job Template</font></b>\n";
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
# Run Parameters
          print "<tr><td><b><font color=\"blue\">Run Parameters</font></b>\n";
          print "</td><td>\n";
          print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
          print "<tr valign=middle><td align=left><b><font size=\"-1\"> Particle : </b></td> <td colspan=1>\n";
          print "<select name=\"QPart\" >\n";
              my $ts=$self->{tsyntax};
              my %hash=%{$ts->{particles}};
              my @keysa=sort {$hash{$a} <=>$hash{$b}} keys %{$ts->{particles}};
              foreach my $particle (@keysa) {
                print "<option value=\"$particle\">$particle </option>\n";
              }
          print "</select>\n";
          print "</b></td></tr>\n";
            htmlTextField("Momentum min","number",7,1.,"QMomI","[GeV/c]");  
            htmlTextField("Momentum max","number",7,200.,"QMomA","[GeV/c]");  
            htmlTextField("Total Events","number",9,1000000.,"QEv"," ");  
            htmlTextField("Total Runs","number",7,3.,"QRun"," ");  
            my ($rndm1,$rndm2) = $self->getrndm();
            htmlTextField("rndm1","text",12,$rndm1,"QRNDM1"," ");  
            htmlTextField("rndm2","text",12,$rndm2,"QRNDM2"," ");  
            htmlTextField("Begin Time","text",8,"01062005","QTimeB"," (ddmmyyyy)");  
            htmlTextField("End Time","text",8,"01062008","QTimeE"," (ddmmyyyy)");  
            htmlTextField("CPU clock","number",8,1000,"QCPU"," [MHz]");  
           htmlTableEnd();
            print "<tr><td><b><font color=\"green\">Automatic DST files transfer to Server</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
# Apr 1.2003 set default transfer to 'No' for both mode - remote/local
#            if ($self->{CCT} eq "remote") {
#             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"R\" CHECKED><b> Yes </b><BR>\n";
#             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"L\" ><b> No </b><BR>\n";
#            } 
#
#else {
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"R\" ><b> Yes </b><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"L\" CHECKED><b> No </b><BR>\n";
#            }
            print "</b></font></td></tr>\n";
           htmlTableEnd();
            if ($self->{CCT} eq "remote") {
             print "<tr><td>\n";
             print "<b><font color=\"green\">Standalone MC production</font></b><BR>\n";
             print "<font color=\"green\" size=2>
                  <i>(<b> STANDALONE</b>  mode if <b> Yes </b>, <BR>\n";
             print "  communicate via <b> SERVER </b> if <b> NO</b>)
                      </i></font></b>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"Yes\" ><b> Yes </b><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"No\" CHECKED><b> No </b><BR>\n";
#             print "<input type=\"checkbox\" name=\"STALONE\" value=\"No\"><b> Yes </b>";
             print "</b></font></td></tr>\n";
           htmlTableEnd();
          }
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
          print "<input type=\"submit\" name=\"BasicQuery\" value=\"Submit\"></br><br>        ";
          htmlReturnToMain();
          htmlFormEnd();
         htmlBottom();
         } elsif($self->{q}->param("CTT") eq "Advanced"){
             htmlTop();
             $self->htmlTemplateTable("Select  Parameters for Advanced Request");
# Job Nick Name
          print "<tr><td><b><font color=\"red\">Job Nick Name</font></b>\n";
          print "</td><td>\n";
          print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
          htmlTextField("Nick Name","text",24,"MC02-advanced","QNick"," ");  
          print "</TABLE>\n";
# Run Parameters
              print "<tr><td><b><font color=\"blue\">Run Parameters</font></b>\n";
              print "</td><td>\n";
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Particle : </b></td> <td colspan=1>\n";
              print "<select name=\"QPart\" >\n";
              my $ts=$self->{tsyntax};
              my %hash=%{$ts->{particles}};
              my @keysa=sort {$hash{$a} <=>$hash{$b}} keys %{$ts->{particles}};
              foreach my $particle (@keysa) {
                print "<option value=\"$particle\">$particle </option>\n";
              }
              print "</select>\n";
              print "</b></td></tr>\n";
              htmlTextField("Momentum min","number",7,1.,"QMomI","[GeV/c]");  
              htmlTextField("Momentum max","number",7,200.,"QMomA","[GeV/c]");  
              htmlTextField("Total Events","number",12,1000000.,"QEv"," ");  
              htmlTextField("Total Runs","number",12,3.,"QRun"," ");  
              my ($rndm1,$rndm2) = $self->getrndm();
              htmlTextField("rndm1","text",12,$rndm1,"QRNDM1"," ");  
              htmlTextField("rndm2","text",12,$rndm2,"QRNDM2"," ");  
              htmlTextField("Begin Time","text",11,"01062005","QTimeB"," (ddmmyyyy)");  
              htmlTextField("End Time","text",11,"01062008","QTimeE"," (ddmmyyyy)");  
              htmlTextField("CPU clock","number",10,1000,"QCPU"," [MHz]");  
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
            print "<tr><td><b><font color=\"green\">Automatic DST files transfer to Server</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
# Apr 1.2003 set default transfer to 'No' for both mode - remote/local
#            if ($self->{CCT} eq "remote") {
#             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"R\" CHECKED><b> Yes </b><BR>\n";
#             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"L\" ><b> No </b><BR>\n";
#            } else {
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"R\" ><b> Yes </b><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"L\" CHECKED><b> No </b><BR>\n";
#            }
            print "</b></font></td></tr>\n";
           htmlTableEnd();
            if ($self->{CCT} eq "remote") {
             print "<tr><td>\n";
             print "<b><font color=\"green\">Standalone MC production</font></b><BR>\n";
             print "<font color=\"green\" size=2>
                  <i>(<b> STANDALONE</b>  mode if <b> Yes </b>, <BR>\n";
             print "  communicate via <b> SERVER </b> if <b> NO</b>)
                      </i></font></b>\n";

             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"Yes\" ><b> Yes </b><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"No\" CHECKED><b> No </b><BR>\n";
#             print "<input type=\"checkbox\" name=\"STALONE\" value=\"No\"><b> Yes </b>";
             print "</b></font></td></tr>\n";
           htmlTableEnd();
          }
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
          print "<input type=\"submit\" name=\"AdvancedQuery\" value=\"Submit\"></br><br>        ";
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
# Run Parameters
              print "<tr><td><b><font color=\"blue\">Run Parameters</font></b>\n";
              print "</td><td>\n";
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
              $q->param("QEv",0);
              htmlTextField("CPU Time Limit Per Job","number",9,80000,"QCPUTime"," sec");  
              htmlTextField("CPU clock","number",8,1000,"QCPU"," [MHz]");  
              htmlTextField("Total Jobs Requested","number",7,5.,"QRun"," ");  
              htmlTextField("Total  Real Time Required","number",3,10,"QTimeOut"," (days)");  
            htmlTableEnd();
# DST transfer
             print "<tr><td><b><font color=\"green\">Automatic DST files transfer to Server</font></b>\n";
             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
# Apr 1.2003 set default transfer to 'No' for both mode - remote/local
#            if ($self->{CCT} eq "remote") {
#             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"R\" CHECKED><b> Yes </b><BR>\n";
#             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"L\" ><b> No </b><BR>\n";
#            } else {
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"R\" ><b> Yes </b><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"AFT\" VALUE=\"L\" CHECKED><b> No </b><BR>\n";
#            }
             print "</b></font></td></tr>\n";
            htmlTableEnd();
            if ($self->{CCT} eq "remote") {
             print "<tr><td>\n";
             print "<b><font color=\"green\">Standalone MC production</font></b><BR>\n";

             print "<font color=\"green\" size=2>
                  <i>(<b> STANDALONE</b>  mode if <b> Yes </b>, <BR>\n";
             print "  communicate via <b> SERVER </b> if <b> NO</b>)
                      </i></font></b>\n";

             print "</td><td>\n";
             print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
             print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"Yes\" ><b> Yes </b><BR>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"STALONE\" VALUE=\"No\" CHECKED><b> No </b><BR>\n";
#             print "<input type=\"checkbox\" name=\"STALONE\" value=\"No\"><b> Yes </b>";
             print "</b></font></td></tr>\n";
           htmlTableEnd();
          }
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
           print "<input type=\"submit\" name=\"ProductionQuery\" value=\"Submit\"></br><br>        ";
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
                my ($rndm1,$rndm2) = $self->getrndm();
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
                my ($rndm1,$rndm2) = $self->getrndm();
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
                my ($rndm1,$rndm2) = $self->getrndm();
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
                    $evno=$q->param("QCPUTime")*$q->param("QCPU")/1000./$tmp->{CPUPEREVENTPERGHZ};
                    $evno=int($evno/1000)*1000*$q->param("QRun");                                     if($evno>$tmp->{TOTALEVENTS}){
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
        if($cput > 3000 ){
            $cput=3000;
        }
        $cput=50+$pmax*1000/$cput;
        
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
             $self->ErrorPlus("Time  $evno is out of range (1,31) days. ");
          }
          $timeout=int($timeout*1.41*3600*24);
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

         my $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen $nv") ;
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
        $i=system "ln -s $self->{AMSDataDir}/DataBase/MagneticFieldMap/9* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Tracker*.2* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Tracker*2 $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/*of*/*/*.0.* $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/.*0 $self->{UploadsDir}/DataBase";
        $i=system "ln -s $self->{AMSDataDir}/DataBase/.TrA*1 $self->{UploadsDir}/DataBase"; 
        $i=system "ln -s $self->{AMSDataDir}/DataBase/Ecal* $self->{UploadsDir}/DataBase";
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
             ($rndm1,$rndm2)=$self->getrndm();
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
         my $ctime=time();
         my $nickname = $q->param("QNick");
         my $sql="insert into Jobs values($run,'$script',$self->{CEMID},$self->{CCID},$did,$ctime,$evts,$timeout,'$buf$tmpb',$ctime,'$nickname')";
         $self->{sqlserver}->Update($sql);
#
#         $sql="insert into JobsNick values($run,'$nickname')";
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
         if ($self->{CCT} eq "remote"){
          my $i=system("gzip -f $file2tar");
          if($i){
              $self->ErrorPlus("Unable to gzip  $file2tar");
          }
        }     
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
#
# Add files to server
#                    
        if(defined $self->{dbserver}){
            foreach my $ri (@{$self->{Runs}}){
              DBServer::sendRunEvInfo($self->{dbserver},$ri,"Create");
            }
            my $lu=time();
            $sql="update Servers set lastupdate=$lu where dbfilename='$self->{dbfile}'";
            $self->{sqlserver}->Update($sql);
        }
        else{
            $self->ErrorPlus("Unable To Communicate With Server");
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
      my $cite_status=>"remote";
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
        $filedb="$self->{UploadsDir}/$self->{FileDB}";
        my @sta0 = stat $filedb;
        $filedb="$self->{UploadsDir}/$self->{FileAttDB}";
        my @sta1 = stat $filedb;
        $self->{FileDBTimestamp}=$sta0[9];
        $self->{FileAttDBTimestamp}=$sta1[9];
        $self->{FileDBLastLoad}=$uplt0;
        $self->{FileAttDBLastLoad}=$uplt1;
        if ($self->{dwldaddon} == 1) {
         if($uplt0 == 0 or $uplt0 < $sta0[9] or $uplt1 == 0 or $uplt1 < $sta1[9]) {
            $self->Download();
         } else {
          $self->{FinalMessage}=" Your request was successfully sent to $self->{CEM}";     
         } 
        } else {
         if($uplt0 == 0 or $uplt0 < $sta0[9]) {
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

sub getrndm(){
    my $self=shift;
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
        $sql="select rndm1,rndm2 from RNDM where rid=$maxrun";
        my $res=$self->{sqlserver}->Query($sql);
        if( not defined $res->[0][0] ){
          my $big=2147483647;
          my $rndm1=int (rand $big);
          my $rndm2=int (rand $big);
           Warning::error($self->{q},"unable to read rndm table $maxrun");

          return ($rndm1,$rndm2);
         }
    my $rndm1=$res->[0][0];
    my $rndm2=$res->[0][1];
             $sql="UPDATE RNDM SET rid=-rid where rid=$maxrun";
             $self->{sqlserver}->Update($sql);
             $maxrun=-$maxrun+1;
             $sql="UPDATE RNDM SET rid=-rid where rid=$maxrun";
             $self->{sqlserver}->Update($sql);
             return ($rndm1,$rndm2);
 
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

sub htmlFormEnd {
   print "</form>\n";
}

sub htmlTextField {
    my ($text,$type,$size,$value,$name,$comment) = @_;
    print "<tr><td><font size=\"2\">\n";
    print "<b> $text </td><td><input type=$type size=$size value=$value name=$name>$comment\n";
    print "</b></font></td></tr>\n";
}

sub htmlReturnToMain {
            print "<p><tr><td><i>\n";
            print "Return to <a href=$rchtml> MC02 Remote Client Request</a>\n";
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

sub listAll {
    my $self = shift;
    my $show = shift;
    htmlTop();
    $self->ht_init();
    if ($show eq 'all') {
#     $self->ht_Menus();
     $self -> colorLegend();
    }
    
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
          print "<tr><td><b><font color=\"red\" size=2>Job Template</font></b>\n";
          print "</td><td>\n";
          print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
          print "<tr valign=middle><td align=left><b><font size=\"-1\"> File : </b></td> <td colspan=1>\n";
          print "<select name=\"QTemp\" >\n";
          print "<option value=\"Any\">  </option>\n";
          foreach my $template (@tempnam) {
            print "<option value=\"$template\">$template </option>\n";
          }
          print "</select>\n";
          print "</b></td></tr>\n";
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
# Run Parameters
          print "<tr><td><b><font color=\"blue\" size=2>Run Parameters</font></b>\n";
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
            print "<tr><td><b><font color=\"green\" size=2>Spectrum and Focusing</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            $ts=$self->{tsyntax};
            @keysa=sort keys %{$ts->{spectra}};
              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Input Spectrum : </b></td> <td colspan=1>\n";
              print "<select name=\"QSpec\" >\n";
              print "<option value=\"Any\">  </option>\n";
              foreach my $spectrum (@keysa) {
                print "<option value=\"$spectrum\">$spectrum </option>\n";
              }
              print "</select>\n";
              print "</b></td></tr>\n";
           @keysa=sort keys %{$ts->{focus}};
              print "<tr valign=middle><td align=left><b><font size=\"-1\"> Focusing : </b></td> <td colspan=1>\n";
              print "<select name=\"QFocus\" >\n";
              print "<option value=\"Any\">  </option>\n";
              foreach my $focus (@keysa) {
                print "<option value=\"$focus\">$focus </option>\n";
              }
              print "</select>\n";
              print "</b></td></tr>\n";
           htmlTableEnd();
# geo magnetic cutoff
            print "<tr><td><b><font color=\"magenta\" size=2>GeoMagnetic Cutoff</font></b>\n";
            print "</td><td>\n";
            print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
            print "<tr><td><font size=\"-1\"<b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"GCF\" VALUE=\"1\" CHECKED><b> Yes </b>\n";
             print "<INPUT TYPE=\"radio\" NAME=\"GCF\" VALUE=\"0\" ><b> No </b><BR>\n";
             print "</b></font></td></tr>\n";
           htmlTableEnd();
# cube coordinates
            print "<tr><td><b><font color=\"blue\" size=2>Cube Coordinates</font></b>\n";
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
              print "<select name=\"QFocus\" >\n";
              print "<option value=\"Any\">  </option>\n";
              foreach my $surface (@keysa) {
                print "<option value=\"$surface\">$surface </option>\n";
              }
              print "</select>\n";
              print "</b></td></tr>\n";
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
    
sub listCites {
    my $self = shift;
    print "<b><h2><A Name = \"cites\"> </a></h2></b> \n";
     htmlTable("MC02 Cites");
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     my $sql="SELECT cid, descr, name, status, maxrun FROM Cites ORDER by cid";
     my $r3=$self->{sqlserver}->Query($sql);
              print "<tr><td><b><font color=\"blue\">Cite </font></b></td>";
              print "<td><b><font color=\"blue\" >ID </font></b></td>";
              print "<td><b><font color=\"blue\" >Type </font></b></td>";
              print "<td><b><font color=\"blue\" >Jobs Ends</font></b></td>";
              print "<td><b><font color=\"blue\" >Jobs Reqs</font></b></td>";
     print_bar($bluebar,3);
     if(defined $r3->[0][0]){
      foreach my $cite (@{$r3}){
          my $cid    = $cite->[0];
          my $descr  = $cite->[1];
          my $name   = $cite->[2];
          my $status = $cite->[3];
          my $maxrun = $cite->[4];
          my $run=(($cid-1)<<27)+1;
          $sql="SELECT Jobs.jid, Runs.jid, Jobs.cid  
                FROM Runs, Jobs  
                WHERE Jobs.jid=Runs.jid AND cid=$cid 
                      AND (Runs.status='Finished' OR Runs.status='Failed')";
          my $r4=$self->{sqlserver}->Query($sql);
          my $jobs = 0;
          foreach my $cnt (@{$r4}){
              $jobs++;
          }
          $sql="SELECT SUM(requests) FROM Mails WHERE cid=$cid";
          $r4=$self->{sqlserver}->Query($sql);
          my $reqs = 0;
          foreach my $cnt (@{$r4}){
              $reqs = $cnt->[0];
          }
          print "<tr><font size=\"2\">\n";
          print "<td><b> $descr ($name) </td><td><b> $cid </td><td><b> $status </td>
                 <td><b> $jobs </td></b><td><b> $reqs </b></td>\n";
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
    my $r4=$self->{sqlserver}->Query($sql);
    if( defined $r4->[0][0]){
      $lastupd=localtime($r4->[0][0]);
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
    my $r4=$self->{sqlserver}->Query($sql);
    if( defined $r4->[0][0]){
      $lastupd=localtime($r4->[0][0]);
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
     print "<b><h2><A Name = \"jobs\"> </a></h2></b> \n";
     htmlTable("MC02 Jobs");
     my $sql="SELECT Jobs.jid, Jobs.jobname, Jobs.cid, Jobs.mid, Jobs.time, Jobs.triggers,
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
          my $trig      = $job->[5];
          my $cite      = $job->[7];
          my $user      = $job->[9];
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
               print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
#               print "<tr><td><b><font color=\"blue\" >Cite </font></b></td>";
               print "<td><b><font color=\"blue\">JobId </font></b></td>";
               print "<td><b><font color=\"blue\" >Owner </font></b></td>";
               print "<td><b><font color=\"blue\" >JobName </font></b></td>";
               print "<td><b><font color=\"blue\" >Submit Time </font></b></td>";
               print "<td><b><font color=\"blue\" >Triggers </font></b></td>";
               print "<td><b><font color=\"blue\" >Status </font></b></td>";
              print "</tr>\n";
           }
          }
#          print "<td><b><font color=$color> $cite </font></td></b>
           print "
                  <td><b><font color=$color> $jid </font></td></b>
                  <td><b><font color=$color> $user </font></b></td>
                  <td><b><font color=$color> $name </font></td></b>
                  <td><b><font color=$color> $starttime </font></b></td>
                  <td><b><font color=$color> $trig </font></b></td>
                  <td><b><font color=$color> $status </font></b></td>\n";

          print "</font></tr>\n";
      }
  }
       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
}

sub listRuns {
    my $self = shift;
     print "<b><h2><A Name = \"runs\"> </a></h2></b> \n";
     htmlTable("MC02 Runs");
              print "<table border=0 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     my $sql="SELECT Runs.run, Runs.jid, Jobs.jobname, Runs.submit, Runs.status, Jobs.jid 
              FROM   Runs, Jobs
              WHERE  Jobs.jid=Runs.jid 
              ORDER  BY Jobs.jid";
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
          my $starttime = EpochToDDMMYYHHMMSS($run->[3]); 
          my $status    = $run->[4];
          my $color = statusColor($status);
           print "<td><b><font color=$color> $jid       </font></td></b><td><b> $nn </td>
                  <td><b><font color=$color> $starttime </font></b></td>
                  <td><b><font color=$color> $status    </font></b></td> \n";
          print "</font></tr>\n";
      }
  }
       htmlTableEnd();
      htmlTableEnd();
     print_bar($bluebar,3);
     print "<p></p>\n";
}

sub listNtuples {
    my $self = shift;
     print "<b><h2><A Name = \"ntuples\"> </a></h2></b> \n";
     print "<TR><B><font color=green size= 5><a href=$validatecgi><b><font color=green> MC NTuples </font></a><font size=3><i> (Click NTuples to validate)</font></i></font>";
     print "<p>\n";
     print "<TABLE BORDER=\"1\" WIDTH=\"100%\">";
              print "<table border=1 width=\"100%\" cellpadding=0 cellspacing=0>\n";
     my $sql="SELECT Ntuples.run, Ntuples.jid, Ntuples.nevents, Ntuples.neventserr, 
                     Ntuples.timestamp, Ntuples.status, Ntuples.path
              FROM   Ntuples
              ORDER  BY Ntuples.timestamp";
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
        my @sta0 = stat $filedb;
        $filedb="$self->{UploadsDir}/$self->{FileAttDB}";
        my @sta1 = stat $filedb;
        $self->{FileDBTimestamp}=$sta0[9];
        $self->{FileAttDBTimestamp}=$sta1[9];
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
    my $mode = "Server";
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
    $self->{UploadsDir}="/var/www/cgi-bin/AMS02MCUploads";
    $self->{FileDB}="ams02mcdb.tar.gz";
    $self->{FileAttDB}="ams02mcdb.addon.tar.gz";
    $self->{dwldaddon} = 1;
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
    $self->DownloadTime();
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
    my $file= $self->{FileDB};
#    $file=~ s/gz/Z /;
    print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$file>  filedb files (tar.gz)</a>
           </font>";
    my $dtime=EpochToDDMMYYHHMMSS($self->{FileDBTimestamp});
    print "<font size=\"3\" color=\"red\"><i><b>       ( Updated : $dtime)</b></i></font>\n";
    print "<br><br>";
    $file= $self->{FileAttDB};
#    $file=~ s/gz/Z /;
#           <a href=$self->{UploadsHREF}/$self->{FileAttDB}>   filedb att.files (tar.gz)</a>
    if ($self->{dwldaddon} == 1) {
     print "<br><font size=\"4\">
           <a href=load.cgi?$self->{UploadsHREF}/$file>   filedb att.files (tar.gz)</a>
           </font>\n";
     $dtime=EpochToDDMMYYHHMMSS($self->{FileAttDBTimestamp});
     print "<font size=\"3\" color=\"red\"><i><b>       ( Updated : $dtime)</b></i></font>\n";
     print "<br><br>\n";
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
    print "<table width=100 cellpadding=2 cellspacing=2 border=2 align=right>\n";;
    print "<caption><font size=4 color=\"#3399ff\"><b>Colors Legend</b></font><br>\n";
    print "<tr>\n";
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
    print "</table><p></p><br>\n";
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

    if ($status eq "Finished" or $status eq "OK" or $status eq "Validated") {
               $color  = "green";
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
