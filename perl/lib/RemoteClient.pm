# $Id: RemoteClient.pm,v 1.1 2002/02/20 18:00:25 choutko Exp $
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
@RemoteClient::EXPORT= qw(new  Connect Warning);

sub new{
    my $type=shift;
my %fields=(
        FinalMessage=>undef,
        Runs=>[],
        TempT=>[],
        MailT=>[],
        CiteT=>[],
        AMSSoftwareDir=>undef,
        AMSDataDir=>undef,
        CERN_ROOT=>undef,
        UploadsDir=>undef,
        RemoteClientsDir=>"prod.log/scripts/",
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
            );
    my $self={
      %fields,
    };

foreach my $chop  (@ARGV){
    if($chop =~/^-D/){
        $self->{debug}=1;
    }
}
     $self->{q}=new CGI;
    $self->{Name}="/cgi-bin/mon/validate.cgi";
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
     'photon'=>1,
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
     $self->{AMSDataDir}="/f2dat1/AMS01/AMSDataDir";
     $ENV{AMSDataDir}=$self->{AMSDataDir};
 }
  $dir=$ENV{CERN_ROOT};
 if (defined $dir){
     $self->{CERN_ROOT}=$dir;
 }
 else{
     $self->{CERN_ROOT}="/cern/2001";
 }
    $self->{UploadsDir}="/home/httpd/cgi-bin/AMS02MCUploads";
    $self->{AMSSoftwareDir}="$self->{AMSDataDir}/DataManagement";
#templates

     $dir="$self->{AMSSoftwareDir}/Templates";
    opendir THISDIR ,$dir or die "unable to open $dir";
    my @allfiles= readdir THISDIR;
    closedir THISDIR;    
foreach my $file (@allfiles){
    if($file =~ /\.job$/){
        my $temp={};
        $temp->{filename}=$file;
        my $full=$dir."/$file";
        open(FILE,"<".$full) or die "Unable to open file $full \n";
        my $buf;
        read(FILE,$buf,1638400) or next;
        close FILE;
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

#sqlserver
    $self->{sqlserver}=new DBSQLServer();
    $self->{sqlserver}->Connect();
#cites table
    my $sql="select * from Cites";
    my ($values, $fields)=$self->{sqlserver}->QueryAll($sql);
    foreach my $row (@{$values})  {
     my $cite={};
     my $i=0;
     foreach my $field (@{$fields}){
      $cite->{$field}=$row->[$i++];
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
      $cite->{$field}=$row->[$i++];
     }
     push @{$self->{MailT}}, $cite; 
    }


#try to get ior
    $sql="select dbfilename,lastupdate,IORS,IORP from Servers where status='Active'";
    my $ret=$self->{sqlserver}->Query($sql);
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
      $self->{IOR}=$ior;
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
               die "DBServer corba  SystemException Error "."\n";
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
        foreach my $ars (@{$arsa}){
          if($ars->{Type} eq "Producer"){
                if($ars->{Interface} eq "default"){
                 $ref->{IORP}=$ars->{IOR};
                 my $createt=time();
                 my $sql="insert into Servers values('$ref->{dbfile}','$ref->{IOR}','$ref->{IORP}',NULL,'Active',$createt,$createt)";
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
              if($ntuple->{Type} eq "Ntuple" and $ntuple->{Status} eq "Success"and $ntuple->{Run}== $run->{Run}){
# suppress double //
                  $ntuple->{Name}=~s/\/\//\//;                  
                  my @fpatha=split ':', $ntuple->{Name};
                  my $fpath=$fpatha[$#fpatha];
                  my $suc=open(FILE,"<".$fpath);
                  my ($events,$badevents);
                  if(not $suc){
                    $status="Unchecked";                     
                    $events=$ntuple->{EventNumber};
                    $badevents="NULL";
                    $unchecked++;
                  }  
                  else{
                      close FILE;
                      my $i=system("$self->{AMSSoftwareDir}/exe/linux/fastntrd.exe  $fpath $ntuple->{EventNumber}");
                      if( ($i == 0xff00) or ($i & 0xff)){
                       $status="Unchecked";                     
                       $events=$ntuple->{EventNumber};
                       $badevents="NULL";
                       $unchecked++;
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
                  $sql="insert into Ntuples values($run->{Run},'$ntuple->{Version}','$ntuple->{Type}',$run->{Run},$ntuple->{FirstEvent},$ntuple->{LastEvent},$events,$badevents,$ntuple->{Insert},$ntuple->{size},'$status','$ntuple->{Name}')";
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
   $self->InfoPlus("$validated Ntuple(s) Successfully Validated.\n $bad Ntuple(s) Turned Bad.\n $unchecked Ntuples(s) Could Not Be Validated.");
        $self->{ok}=1;
    
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
    $self->{Name}="/cgi-bin/mon/rc.cgi";

#understand parameters

        $self->{read}=0;
    my $q=$self->{q};

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
            
        }elsif($self->{q}->param("MyRegister") eq "Submit"){
            my $name=$self->{q}->param("CNA");
            if(not defined $name or $name eq ""){
             $self->ErrorPlus("Please fill in Name and Surname Field");
            }
            my $responsible=$self->{q}->param("CCR");
            my $cite=$self->{q}->param("CCA");
            if(not defined $cite or $cite eq ""){
                $cite=$self->{q}->param("CCAS");
              }
        my $sendsuc=undef;
        foreach my $chop (@{$self->{MailT}}) {
              if($chop->{rServer}==1){
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
             $sql="insert into Cites values($cid,'$cite',0,'remote',$run,0)";
             $self->{sqlserver}->Update($sql);

            }
            $sql="select mid from Mails";
            $ret=$self->{sqlserver}->Query($sql);
            my $mid=$ret->[$#{$ret}][0]+1;
            my $resp=($responsible eq "Yes" or $newcite)?1:0;
            $sql="insert into Mails values($mid,'$cem',NULL,'$name',$resp,0,$cid,'Blocked',0)";
            $self->{sqlserver}->Update($sql);
            if($newcite){
                $sql="update Cites set mid=$mid where cid=$cid";
                $self->{sqlserver}->Update($sql);
            }
         $self->{FinalMessage}=" Your request to register was succesfully sent to $sendsuc. Your account will be enabled soon.";     
        }
    }else{
            $self->ErrorPlus("E-Mail $cem Seems Not to Be Valid.");
        }
       }  
    }

    
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
         my @keysa=sort keys %{$ts->{particles}};
         print "Particle";
         print $q->popup_menu(
          -name=>"QPart",
          -values=>\@keysa,
          -default=>$keysa[0]);
          print "<BR>";
          print "Min Momentum (GeV/c)";
          print $q->textfield(-name=>"QMomI",-default=>0.2);
                print "Max Momentum (GeV/c)";
          print $q->textfield(-name=>"QMomA",-default=>2000.);
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
`;
#         print "<BR>Basic Templates";
#         my @tempnam=();
#             my $hash={};
#         foreach my $cite (@{$self->{TempT}}){
#             push @tempnam, $cite->{filename};
#             $hash->{$cite->{filename}}=$cite->{filedesc};
#         }
#         print $q->popup_menu(
#          -name=>"QTemp",
#          -values=>\@tempnam,
#          -default=>$tempnam[0]);
##          -labels=>$hash);
         my $ts=$self->{tsyntax};
         my @keysa=sort keys %{$ts->{particles}};
         print "Particle";
         print $q->popup_menu(
          -name=>"QPart",
          -values=>\@keysa,
          -default=>$keysa[0]);
          print "<BR>";
          print "Min Momentum (GeV/c)";
          print $q->textfield(-name=>"QMomI",-default=>1);
                print "Max Momentum (GeV/c)";
          print $q->textfield(-name=>"QMomA",-default=>2000.);
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
          }
          else{
              my $query=$self->{q}->param("CTT");
            $self->{FinalMessage}=" Sorry  $query Query Not Yet Implemented";     
          }
         #$self->{FinalMessage}=" Your request was succsesfully sent to $self->{CEM}";     

        }
}


    if ($self->{q}->param("BasicQuery") or $self->{q}->param("AdvancedQuery")){
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

#        $self->ErrorPlus(" $self->{CCA} $self->{CCT} $self->{CCID} $self->{CEM} $self->{CEMA} $self->{CEMID}");
        if($self->{q}->param("BasicQuery") eq "Save" or $self->{q}->param("AdvancedQuery") eq "Save"){
         my $filename=$q->param("FEM");
         $q=get_state($filename);          
         if(not defined $q){
             $self->ErrorPlus("Save State Expired. Please Start From the Very Beginning");
         }
         $self->{q}=$q;
         unlink $filename;
        }
        my $aft=$q->param("AFT");
        my $timbeg=$q->param("QTimeB");
        my $timend=$q->param("QTimeE");
        my $templatebuffer=undef;
        my $template=undef;
        if ($self->{q}->param("BasicQuery")){
         $template=$q->param("QTemp");
        }
        else{
            $template='.Advanced.job';
        }
         foreach my $tmp (@{$self->{TempT}}){
            if($template eq $tmp->{filename}){
                $templatebuffer=$tmp->{filebody};
                last;
            }
         }
        if(not defined $templatebuffer){
            $self->ErrorPlus("Could not find file for $template template.");
        }
        my $particle=$q->param("QPart");
        my $particleid=$self->{tsyntax}->{particles}->{$particle};
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
        my $pmin=$q->param("QMomI");
        my $pmax=$q->param("QMomA");
        if(not $pmin =~/^-?(?:\d+(?:\.\d*)?|\.\d+)$/){
             $self->ErrorPlus("pmin $pmin is not a float number ");
        }
        if(not $pmax =~/^-?(?:\d+(?:\.\d*)?|\.\d+)$/){
             $self->ErrorPlus("pmax $pmin is not a float number ");
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
        
        if(not $timbeg =~/^\d+$/ ){
             $self->ErrorPlus("TimeBegin $timbeg is not an integer");
        }
        my $year=$timbeg%10000-1900;
        my $month=int($timbeg/10000)%100-1;
        my $date=int($timbeg/1000000)%100;
        if($year < 96 or $year > 110 or $month<0 or $month>11 or $date<1 or $date>31){
             $self->ErrorPlus("TimeBegin $timbeg is out of range $date $month $year");
        }
        my $timbegu=timelocal(1,0,8,$date,$month,$year);
        if(not $timend =~/^\d+$/ ){
             $self->ErrorPlus("TimeEnd $timend is not an integer");
        }
        $year=$timend%10000-1900;
        $month=int($timend/10000)%100-1;
        $date=int($timend/1000000)%100;
        if($year < 96 or $year > 110 or $month<0 or $month>11 or $date<1 or $date>31){
             $self->ErrorPlus("TimeEnd $timend is out of range $date $month $year");
        }
        my $timendu=timelocal(1,0,8,$date,$month,$year);
#check if remote
       
            if ($self->{CCT} ne "remote"){
                $self->ErrorPlus("$self->{CCT} requests are not yet supported.");
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
        my $lastrunev=$evno-$evperrun*($runno-1);

# advanced only
        my ($setup,$trtype,$rootntuple,$rno,$spectrum,$focus,$cosmax,$geocutoff,$plane);
            my @cubes=();
        if($self->{q}->param("AdvancedQuery")){
             $setup=$q->param("QSetup");
            if((not $setup =~ '^AMS02') and (not $setup=~'^AMSSHUTTLE')){
                $self->ErrorPlus("Setup $setup does not exist. Only AMS02xxx or AMSSHUTTLEyyy  setup names allowed.");
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
              if($chop->{rServer}==1){
                  my $address=$chop->{address};
                  my $message=" see subject";
                  $self->sendmailmessage($address,$mes,$message);
                  last;
              }
            }          
            $self->ErrorPlus($mes);
        }
        else{
            $sql="update Cites set state=1 where name='$self->{CCA}'";
            $self->{sqlserver}->Update($sql);
            $run=$res->[0][0];
        }
             my $switch=1<<27;
             my $max=$switch-1;    
             if (($run%$switch)+$runno >$max){
              foreach my $chop (@{$self->{MailT}}) {
              if($chop->{rServer}==1){
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
              if($chop->{rServer}==1){
                  my $address=$chop->{address};
                  $self->sendmessage($address,"unable to connect to servers by $self->{CEM}"," "); 
                  last;
              }
          }
            $self->ErrorPlus("Unable to Connect to Server.");
         }


#        prepare the tables
    
# check tar ball exists
 
        my $filedb="$self->{UploadsDir}/ams02mcdb.tar.gz";
        my @sta = stat $filedb;
        if($#sta<0 or $sta[9]-time() >86400*7){
        my $filen="$self->{UploadsDir}/ams02mcdb.tar.$run";
        my $i=system "mkdir -p $self->{UploadsDir}/v3.00";
        $i=system "ln -s $self->{AMSDataDir}/v3.00/*.dat $self->{UploadsDir}/v3.00";
        $i=system "ln -s $self->{AMSDataDir}/v3.00/t* $self->{UploadsDir}/v3.00";
        $i=system "ln -s $self->{AMSDataDir}/v3.00/T* $self->{UploadsDir}/v3.00";
        $i=system "tar -C$self->{UploadsDir} -h -cf $filen v3.00";
          if($i){
              $self->ErrorPlus("Unable to tar v3.00 to $filen");
          }
         $i=system("tar -C$self->{AMSSoftwareDir} -uf $filen exe/linux/gbatch-orbit.exe") ;
          if($i){
              $self->ErrorPlus("Unable to tar gbatch-orbit to $filen ");
          }
         $i=system("tar -C$self->{CERN_ROOT} -uf $filen lib/flukaaf.dat") ;
          if($i){
              $self->ErrorPlus("Unable to tar flukaaf.dat to $filen ");
          }
          $i=system("gzip -f $filen");
                      if($i){
              $self->ErrorPlus("Unable to gzip  $filen");
          }
          $i=system("mv $filedb $filedb.o");
          $i=system("mv $filen.gz $filedb");
          unlink "$filedb.o";
    }
    
# write readme file
        my $readme="$self->{UploadsDir}/README.$run";
        open(FILE,">".$readme) or die "Unable to open file $readme\n";
        print FILE  $self->{tsyntax}->{headers}->{readme};
        close FILE;
        my $file2tar="$self->{UploadsDir}/ams02mcscript.$run.tar";
        my $i=system("tar -C$self->{UploadsDir} -cf  $file2tar README.$run"); 
          if($i){
              $self->ErrorPlus("Unable to tar readme to $file2tar ");
          }
        unlink $readme;
        for $i (1 ... $runno){
         #find buffer and patch it accordingly
         my $evts=$evperrun;
         if($i eq $runno){
             $evts=$lastrunev;
         }              
#read header
         my $buf=$self->{tsyntax}->{headers}->{remote};
         my $tmpb=$templatebuffer;
         if(not defined $buf){
             $self->ErrorPlus("Unable to find  remote header file. ");
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
          $buf=~ s/gbatch-orbit.exe/gbatch-orbit.exe -$self->{IORP} -U$run -M -D1 -G$aft/;
         my $script="$self->{CCA}.$run.$template";
         my $root="$self->{UploadsDir}/$script";
         if($self->{q}->param("AdvancedQuery") eq "Submit" or $self->{q}->param("BasicQuery") eq "Submit"){
# add one more check here using save state
            $sql="update Cites set state=0 where name='$self->{CCA}'";
            $self->{sqlserver}->Update($sql);
             my $save="$self->{UploadsDir}/$self->{CCA}.$run.$self->{CEMID}save";
             my $param=$self->{q}->param("AdvancedQuery")?"AdvancedQuery":"BasicQuery";
            $q->param($param,"Save");
            $q->param("FEM",$save);
             save_state($q,$save);
                print $q->header( "text/html" ),
                $q->start_html( "Welcome");
                print $q->h1( "Below is the Script Example. Click Save if you wish to continue.");
                print $q->start_form(-method=>"GET", 
                -action=>$self->{Name});
print qq`
           <INPUT TYPE="hidden" NAME="CEM" VALUE=$cem> 
           <INPUT TYPE="hidden" NAME="FEM" VALUE=$save> 
`;
         print $q->textarea(-name=>"CCA",-default=>"$buf$tmpb",-rows=>30,-columns=>80);
             print "<BR>";
         print $q->submit(-name=>$param, -value=>"Save");
         return 1;   
         }
         open(FILE,">".$root) or die "Unable to open file $root\n";  
         print FILE $buf;
         print FILE $tmpb;
         close FILE;
         my $j=system("chmod +x  $root"); 
         $j=system("tar -C$self->{UploadsDir} -uf  $file2tar $script"); 
          if($j){
              $self->ErrorPlus("Unable to tar $script to $file2tar ");
          }
         $buf=~s/\$/\\\$/g;
         $buf=~s/\"/\\\"/g;
         $buf=~s/\(/\\\(/g;
         $buf=~s/\)/\\\)/g;
         $buf=~s/\'/\\'/g;
         $tmpb=~s/\"/\\\"/g;
         $tmpb=~s/\(/\\\(/g;
         $tmpb=~s/\)/\\\)/g;
         $tmpb=~s/\$/\\\$/g;
         $tmpb=~s/\'/\\'/g;
         my $sql="insert into Jobs values($run,'$script',$self->{CEMID},$self->{CCID},'$buf$tmpb')";
         $self->{sqlserver}->Update($sql);
         unlink $root;
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
         $ri->{Status}="Foreign";
         $ri->{History}="Foreign";
         $ri->{SubmitTime}=time();
         $ri->{cuid}=$ri->{Run};
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
          $i=system("gzip -f $file2tar");
          if($i){
              $self->ErrorPlus("Unable to gzip  $file2tar");
          }
         
                  my $address=$self->{CEM};
        my $frun=$run-$runno;
        my $lrun=$run-1;
        my $subject="AMS02 MC Request Form Output Runs for $address $frun...$lrun Cite $self->{CCA}";
                  my $message=$self->{tsyntax}->{headers}->{readme};
                  my $attach="$file2tar.gz,ams02mcscripts.tar.gz;$filedb,ams02mcdb.tar.gz";
#                  my $attach="$file2tar.gz,ams02mcscripts.tar.gz";
                  $self->sendmailmessage($address,$subject,$message,$attach);
                  $i=unlink "$file2tar.gz";
         my $totalreq=$self->{CEMR}+$runno;
         $sql="Update Mails set requests=$totalreq where mid=$self->{CEMID}";
         $self->{sqlserver}->Update($sql);              
         $self->sendmailerror($subject," ");
         $sql="select mid from Cites where cid=$self->{CCID}";
         my $ret=$self->{sqlserver}->Query($sql);
         if(defined $ret->[0][0] && $ret->[0][0] != $self->{CEMID}){
           $sql="select address from Mails where mid=$ret->[0][0]";
           $ret=$self->{sqlserver}->Query($sql);
           if(defined $ret->[0][0]){
             $self->sendmailmessage($ret->[0][0],$subject," ");
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
             $sql="update Cites set state=0, maxrun=$run where name='$self->{CCA}'";
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
                  $self->{FinalMessage}=" Your request was succsesfully sent to $self->{CEM}";     
            
        
    }


#here the default action
    if($self->{read}==0){
    print $q->header( "text/html" ),
    $q->start_html( "Welcome");
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
       print $q->submit(-name=>"MyQuery", -value=>"Submit");
       print $q->reset(-name=>"Reset");
         print $q->end_form;
}
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
       my $sql="update Cites set state=0 where name='$ref->{CCA}'";
       $ref->{sqlserver}->Update($sql);
    }
    Warning::error($ref->{q},shift);
  }


sub findemail(){
    my $self=shift;
        my $cem=shift;
        foreach my $chop (@{$self->{MailT}}) {
            if($chop->{address} eq $cem ){
                $self->{CEM}=$cem;
                $self->{CEMA}=$chop->{status};
                $self->{CEMID}=$chop->{mid};
                $self->{CEMR}=$chop->{requests};
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
             if($chop->{alias} eq $cem){
                $self->{CEM}=$cem;
                $self->{CEMA}=$chop->{status};
                $self->{CEMID}=$chop->{mid};
                $self->{CEMR}=$chop->{requests};
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
              if($chop->{rServer}==1){
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
    my $big=2147483647;
    my $rndm1=int (rand $big);
    my $rndm2=int (rand $big);
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




