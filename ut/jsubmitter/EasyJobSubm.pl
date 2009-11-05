#!/usr/bin/perl
use Tk;
use Tk::Event;
use Time::gmtime;
use Time::localtime;
use IO::Handle;
use Tk::ProgressBar;
use Time::Local;
# use Term::ReadKey;
use File::Copy;
#
#use lib "/usr/lib/perl5/site_perl/5.6.1/i386-linux";
#use lib "/afs/cern.ch/user/c/choumilo/AMS/perl";
#
use lib "$ENV{MyCVSTopDir}/perl";
#
use lib::DBSQLServer;
#
#-----> imp.glob.const, vars and arrays:
#
$ConstPi=3.14159265358979;
$Deg2Rad=$ConstPi/180.;
$AMSCubeHSize=195.;
@DataTypes=qw(SCI CAL LAS);
$TrigPhysMembs=8;
@TrigPhysmNams=qw(TOF Pr+I Ion sIon Elec Gamm EC Ext);
$WarnFlg=0;
$JobTypes=3;
$SessName="";
$workdir="";
$amsjwd="/amsjobwd";
$daqflnk="/daqflinks";
$archdir="/archive";
#
$RootfDir=$ENV{MyRootfDir};#full path to /rootfiles;
$RootfSubD="";
$MCDaqfDir=$ENV{MyMCDaqfDir};#full path to /rootfiles;
$MCDaqfSubD="";
#
$logfdir="";
$AMSDD=$ENV{AMSDataDir};# current AMSDataDir path
$AMSCVS=$ENV{MyCVSTopDir};#full path to .../AMS;
#
$offldir=$ENV{Offline};# /Offline
$cmd="ls -all ".$offldir." |";
@array=();
@string=();
open(OFFLD,$cmd) or show_warn("   <-- Cannot find Offline ref.  $!");
while(<OFFLD>){push(@array,$_);}
close(FLIST);
@string=split(/\s+/,$array[scalar(@array)-1]);
$OFFLDIR=$string[scalar(@string)-1];
#
$DaqDataDir=$ENV{RunsDir};# current AMS SCI-data dir (/Offline/RunsDir)
$USERTD=$ENV{MyJobSubmDir};#full path to /jobsubmwd
$TofUserN="";
$TofPassW="";
$UNinAtt=0;  
$PWinAtt=0;  
$soundtext="SoundON";
$ResetHistFlg=0;
$PrevSessUTC=0;
$PrevSessTime=0;
$SessTLabel="";
$FirstRefCflistN=0;
$LastRefCflistN=0;
$JobDaqfHistFN="";
@DaqfHist=();
$DaqfHistLen=();
@SubmJobsList=();
@LigFileList=();
$AutoSubmFlg="";
$JobScriptN="";
$KilledJobs=0;
@KilledJobsPids=();
$SingleFileJob=0;
$CalRun1=0;#1st run in calibr.files set used in "submit job" procedure
$CalRun2=0;#last run in calibr.files set ............................
$jobtype="undefined";
$SubDetMsk=0;
$message="";
@DCdefpars=();
#
$TDVName="";
$CalfDir="";
$SetupName="";
$RefCFileData=();
#
#---> formats:
$SubmJobsFmt="A8 A9 A5 A12 A12 A11 A7 A3";#for pack/unpack subm.jobs.info line in hist.file
#
$LogfListFmt="A6 A11 A11 A9 A4";#for pack/unpack of log-files list(sjpid name 1strunn dd.hh:mm status)
$LogfLboxFmt="A6 A11 A11 A9 A4";#for log-file listbox(sjpid name 1strunn dd.hh:mm status)
#
$CalfListFmt="A11 A9 A9 A3";#for cal-files listbox-description(1strunn dd.hh:mm utcmin status)
$CalfLboxFmt="A11 A9 A3";#for cal-files listbox-description(1strunn dd.hh:mm status)
#
$BjobsFmt="A8 A8 A7 A10 A12 A12 A13 A14";#for packing message of "bjobs"
#
#-----
@AMSSetupsList=qw(AMS02PreAss AMS02Ass1 AMS02Space);
@AMSHostsList=qw(Any ams pcamsr0 pcamsn0 pcamsf3 pcamsj1);
@AMSQueuesList=qw(1nh 8nh 1nd 1nw 2nw);
#
@cfilenames=qw(TofCflistRD TofCStatRD TofTdelvRD TofTzslwRD TofAmplfRD TofElospRD TofTdcorRD);
#
@IllumTypes=qw(TopHSphera BotHSphera FxDir&Point AMSCubeTop); 
#-----
$prog2run="";
$outlogf="out.log";
$binw_tev=120;#binwidth(sec/pix) for time-evolution graphs(2 min)
$canscrx1=-40;# Pars time-evolution scroll-area (min/max) , all in pixels
$canscrx2=10000;#at $binw_tev=120(sec/pix) corresp.full time-scale almost 2weeks
$RunNum=0;
$RunType=" ";
$FileSize=0;
$nprocruns=0;
$stopjob=0;
$ServConnect=0;
$ServObject=0;
@barsppl=(8,8,10,8);
$antsec=8;
$TdcBin=24.4141;#ps
#----------- define/create main window :
$mwnd=MainWindow->new();
open(DISPLS,"xrandr -q |") or die "Couldn't xrandr $!\n";
@dsets=();
while(defined ($line=<DISPLS>)){
  chomp $line;
  if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
  $line =~s/^\s+//s;# remove " "'s at the beg.of line
  if($line =~/^\*/){last;}# break "*" at the beg.of line
}
@dsets=split(/\s+/,$line);#split by any number of continious " "'s
close(DISPLS);
#--
$displx=$dsets[1];
$disply=$dsets[3];
print "Display resolution is ",$displx,"x",$disply,"\n";
$LowResDisplay=0;
if($displx<1280 || $disply<800){$LowResDisplay=1;}
#--
$mnwdszx=0.94;#X-size of main window (portion of display x-size)
$mnwdszy=0.6;#Y-size of main window (portion of display y-size)
$mwposx="+0";
$mwposy="+0";
my $szx=int($mnwdszx*$displx);#in pixels
my $szy=int($mnwdszy*$disply);
$mwndgeom=$szx."x".$szy.$mwposx.$mwposy;
print "Mwindow geom=",$mwndgeom,"\n";
$mwnd->geometry($mwndgeom);
$mwnd->title("Universal AMS Offline-jobs Submitter (author E.Choumilov)");
$mwnd->bell;
#--------------
#---> fonts:
$font1="Helvetica 14 normal italic";
$font2="times 14 bold";
$font2a="times 12 bold";
$font2b="times 10 bold";
$font2c="times 16 bold";
$font3="Helvetica 12 bold italic";
$font3a="Helvetica 10 bold italic";
$font4="Helvetica 14 bold italic";# for Message and Attention
$font5="Helvetica 10 bold";#sess.logfile window
$font5b="Helvetica 12 bold";#sess.logfile window big text
$font5i="Helvetica 10 bold italic";#logfile window italic
$font6="Arial 12 bold";#help window
$font7="times 12 bold";
$font8="times 8 bold";
$font9="Helvetica 20 bold italic";# for BigAttention
$font10="Helvetica 18 bold italic";# for BigMessage
$font11="Helvetica 14 bold italic";# for NormMessage
#---
if($LowResDisplay==0){#high resol display
  $EntrWFont=$font3;
  $MenuWFont=$font3;
  $LablWFont=$font2;
  $ButtWFont=$font2;
  $ListbWFont=$font2;
}
else{
  $EntrWFont=$font3a;
  $MenuWFont=$font3a;
  $LablWFont=$font2a;
  $ButtWFont=$font2a;
  $ListbWFont=$font2a;
}
#--------------
#
#-----> create log-file screen:
#
$LogfXsize=0.55;
$log_fr=$mwnd->Frame(-label=>"LogFileFrame", -relief=>'groove', -borderwidth=>5)->place(
                                                      -relwidth=>$LogfXsize, -relheight=>0.95,
                                                      -relx=>(1-$LogfXsize), -rely=>0);
$logtext=$log_fr->Scrolled("Text",
#                                    -scrollbars=>'osoe',
                                     -width=>120, -height=>200,
                                     -font=>$font5,
                                     -foreground=>black,
                                     -background=>LightGreen)
                                     ->pack(-fill=>'both', -expand=>1);
#
$logtext->tagConfigure('Attention',-foreground=> red, -font=>$font4);
$logtext->tagConfigure('BigAttention',-foreground=> red, -font=>$font10);
$logtext->tagConfigure('BigMessage',-foreground=> blue, -font=>$font10);
$logtext->tagConfigure('Message',-foreground=> black, -font=>$font11);
$logtext->tagConfigure('ItalText',-font=>$font4);
$logtext->tagConfigure('Text',-font=>$font4);
$logtext->tagConfigure('Yellow',-foreground=> yellow, -font=>$font4);
$logtext->tagConfigure('Orange',-foreground=> orange, -font=>$font4);
$logtext->tagConfigure('Blue',-foreground=> blue, -font=>$font4);
$logtext->tagConfigure('Green',-foreground=> green, -font=>$font4);
#-------------
my $nbutt=$JobTypes+3;
my $bspac=0.002;
my $width=(1-($nbutt-1)*$bspac)/$nbutt;
my $bpos=0;
#
#-----> create job-types control panel:
#
$ctpanel_fr=$mwnd->Frame(-relief=>'groove', -borderwidth=>2,
                                            -background=>red
                                                          )
					       ->place(
                                                      -relwidth=>$LogfXsize, -relheight=>0.05,
                                                      -relx=>(1-$LogfXsize), -rely=>0.95);
#---> label:
$ctpanel_fr->Label(-text=>"SelectSessType :",-font=>$LablWFont,-relief=>'groove',
                                                   -background=>yellow,
                                                   -foreground=>red
						   )
                                            ->place(-relwidth=>$width, -relheight=>0.95,
                                                    -relx=>0, -rely=>0);
$bpos+=$width+$bspac;
#---> create 1 radio-button for each job-type:
@JobTypButt=();
$JobType="";
my $loop=0;
foreach (qw(RealDataReco McDataSimu McDataReco)){
  $JobTypButt[$loop]=$ctpanel_fr->Radiobutton(-text => $_, -value => $_,
                                 -font=>$ButtWFont, -indicator=>0,
                                 -borderwidth=>3,-relief=>'raised',
			         -selectcolor=>orange,-activeforeground=>red,
			         -activebackground=>yellow, 
                                 -background=>green,-cursor=>hand2,
			         -variable => \$JobType)
			         ->place(
	                            -relwidth=>$width, -relheight=>0.95,
				    -relx=>$bpos, -rely=>0);

 $bpos+=$width+$bspac;
 $loop+=1; 
}
#---> StartSess-butt:
$StartButt=$ctpanel_fr->Button(-text=>"Start", -font=>$ButtWFont, 
                                   -activebackground=>"orange",
			           -activeforeground=>"red",
			           -foreground=>"red",
			           -background=>"green",
                                   -borderwidth=>3,-relief=>'raised',
			           -cursor=>hand2,
                                   -command => \&StartSess)
			           ->place(
                                   -relwidth=>$width, -relheight=>0.95,  
                                   -relx=>$bpos, -rely=>0);
#
$bpos+=$width+$bspac;
# 
$exsess_bt=$ctpanel_fr->Button(-text=>"Exit", -font=>$ButtWFont, 
                                   -activebackground=>"orange",
			           -activeforeground=>"red",
			           -foreground=>"red",
			           -background=>"green",
                                   -borderwidth=>3,-relief=>'raised',
			           -cursor=>hand2,
                                   -command => \&ExitSess)
			           ->place(
                                   -relwidth=>$width, -relheight=>0.95,  
                                   -relx=>$bpos, -rely=>0);
#
$bpos+=$width+$bspac;
#---------------------------------------------------------------------
MainLoop;
#---------------------------------------------------------------------
sub run2time{
  my ($time,$ptime,$year,$month,$day,$hour,$min,$sec);
  my $run=$_[0];
  $ptime=localtime($run);#local time (imply run# to be UTC-seconds as input)
  $year=$ptime->year+1900;
  $month=$ptime->mon+1;
  $day=$ptime->mday;
  $hour=$ptime->hour;
  $min=$ptime->min;
  $sec=$ptime->sec;
  $time=sprintf("%4d.%02d.%02d %02d:%02d:%02d",$year,$month,$day,$hour,$min,$sec);
  return $time;
}
#---
sub time2run{# imply "yyyy.mm.dd hh:mm:ss" as input
  my ($run,$yy,$mm,$dd,$hh,$mn,$ss);
  my $time=$_[0];
  ($yy,$mm,$dd,$hh,$mn,$ss)=unpack("A4 x1 A2 x1 A2 x1 A2 x1 A2 x1 A2",$time);#unpack time
  $run=timelocal($ss,$mn,$hh,$dd,$mm-1,$yy-1900);# UTC(GMT) in seconds (imply local time as input)
  return $run;
}
#---------------------------------------------------------------------
sub show_warn {# 1-line message + 3*beep
  my $text=$_[0];
  my $color=$_[1];
  my $tag;
  if($color eq "Big"){$tag='BigAttention';}
  else{$tag='Attention';}
  $message=$text;
  $logtext->insert('end',$text."\n",$tag);
  $logtext->yview('end');
  $count=2;
  if($soundtext eq "SoundON"){
    while($count-- >0){
      $mwnd->bell;
      $mwnd->after(100);
    }
  }
  $WarnFlg=1;
}
#--------------
sub show_warn_setfstat {# 1-line message + 3*beep + set file-status to 211(bad)
  my $text=$_[0];
  my $indx=$_[1];
  $message=$text;
  $logtext->insert('end',$text."\n",'Attention');
  $logtext->yview('end');
  $message="   <-- run $daqfrunn[$indx] status is set to bad !";
  $logtext->insert('end',$message."\n",'Attention');
  $logtext->yview('end');
  $daqfstat[$indx]=211;
  $count=2;
  if($soundtext eq "SoundON"){
    while($count-- >0){
      $mwnd->bell;
      $mwnd->after(150);
    }
  }
}
#--------------
sub show_messg {# 1-line message + 1*beep
  my $text=$_[0];
  my $color=$_[1];
  my $tag;
  if($color eq "Y"){$tag='Yellow';}
  elsif($color eq "O"){$tag='Orange';}
  elsif($color eq "B"){$tag='Blue';}
  elsif($color eq "G"){$tag='Green';}
  elsif($color eq "Big"){$tag='BigMessage';}
  else{$tag='Message';}
  $message=$text;
  if($tag eq ""){
    $logtext->insert('end',$text."\n");
    $logtext->yview('end');
  }
  else{
    $logtext->insert('end',$text."\n",$tag);
    $logtext->yview('end');
  }
  if($soundtext eq "SoundON"){$mwnd->bell;}
}
#--------------
sub play_swav {
  my $file;
  if ($_[0]){ $file = $_[0]; }
  else{ $file = "explose.wav"; }
  die "$file is not a file\n" if (! -f $file);

  open(BEEP,"/dev/beep") || die "Can't open /dev/beep\n";

  fork && exit;
  while (1)
  {
	if (read(BEEP,$a,1))
	{
		fork &&	exec ("/usr/bin/esdplay", "$file");
	}
  }

  close BEEP;
}
#-------------
sub mybeep { 
  my ($button,$count,$var) = @_;
  print $button,$count,$var,"\n";
  if($soundtext eq "SoundON"){ 
    while ($count-- > 0) { 	
      $mwnd->bell;                # ring the bell
      $mwnd->after(150);
    }
  }
  $curline="   <-- Beep is clicked !! \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
}
#----------------------------------------------------------------------
sub ExitSess{
  exit;
}
#----------------------------------------------------------------------
sub StartSess{
  $JobDaqfHistFN="JobDaqfHist";#generic, each sess.add uniqe extention
  $SubmJobCount=0;# reset on session start, incr. on each job-submitting
  $FiniJobCount=0;# reset on each "JobOutpControl"-click, incr. when finished jobs found(st=20 after 'bjobs')
  $FinishedJobs=0;# reset on session start, incr. in each "JobOutpControl"-click when finished jobs found 
  $JobConfLogFiles=0;#reset on "JOC"-click, incr. when log-file found and job-confirmed(in "JobOutpControl") 
  $UnFinishedJobs=0;# set on each "Welcome"-call by analizing the SubmJobsList-file
  @DetInDaq=();
  $WrRootFlg=0;
# trig:
  @P2PhysMem=();
  $JobScriptN="UnivJScript";
  $workdir=$USERTD;
#---
  if($JobType eq "McDataSimu"){
    $SessName="MCS";
    $logfdir="/mclogfiles";
#
    $IllumType=$IllumTypes[0];#TopHemiSphera(default,correspnds to what is in DCdef-file)
    $ImpAngThe="180.";
    $ImpAngPhi="0.";
    $IPointX="0.";
    $IPointY="0.";
    $IPointZ="0.";
    $HAreaX="0.";#half-size of the area
    $HAreaY="0.";
    $HAreaZ="0.";
#
    $PartName="Prot";#corresponds to DCdef-file
    $EnSpectr="Cosmic";
    $GamSource="None";
    $FocusOn="NoFocus";
    $AccRDPrintDispl="Summary";
  }
#---
  elsif($JobType eq "RealDataReco"){
    $SessName="RDR";
    $logfdir="/rdlogfiles";
  }
  elsif($JobType eq "McDataReco"){
    $SessName="MCR";
    $logfdir="/mclogfiles";
  }
#-----------------------------
  else{
    show_warn("\n   <--- Job Type not defined, do selection first !!!");
    return;
  }
#
  @TofRefCflistListRD=();
  @AccRefCflistListRD=();
  @EmcRefCflistListRD=();
  @TofRefCflistListMC=();
  @AccRefCflistListMC=();
  @EmcRefCflistListMC=();
#
  require "$ENV{MyCVSTopDir}/ut/jsubmitter/UniJob.pl";
#
  $AutoSubmFlg="ManualSubm";
#
  UniJob();
  $JOCReady=0;
#
  Welcome();
#
#--> disable JobType/Start-buttons:
  my $state="disabled";
  for($i=0;$i<$JobTypes;$i++){
    $JobTypButt[$i]->configure(-state=>$state);
  }
  $StartButt->configure(-state=>$state);
}
#----------------------------------------------------------------------
sub ReadDCDefFile{
#
#--- read/process DC def.params file :
#
  my $fn="DCDefPars.txt";# in local workdir
#  my $fnf=$AMSCVS."/ut/jsubmitter/".$fn;
  my $fnf=$fn;
  my $line,$nel;
  @DCdefpars=();
  @MemsNames=();
  $WarnFlg=0;
  open(DCDEF,"< $fnf") or show_warn("\n   <--- Cannot open $fn for reading, $!");
  while(defined ($line = <DCDEF>)){
    chomp $line;
    if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
    $line =~s/^\s+//s;# remove " "'s at the beg.of line
    push(@DCdefpars,$line);
  }
  close(DCDEF) or show_warn("\n   <--- Cannot close $fn after reading !");
  $nel=scalar(@DCdefpars);
  if($WarnFlg==1 || $nel<4){
    show_warn("\n   <--- Reading problem with DC-defaults file, correct it first !!!");
    return;
  }
  show_messg("\n   <--- Data Cards def.values file is read !");
#---
  my $gr,$gm,$gmm,$cm,$cmm,$pos,$cnam,$cmid,$val;
  my @arr=();
  $pos=0;
  $Ngroups=$DCdefpars[0];
  show_messg("   <--- Found $Ngroups datacards groups...");
  $pos+=1;
  for($gr=0;$gr<$Ngroups;$gr++){#<-- groups loop
    $grid=$DCdefpars[$pos];
    $pos+=1;
    $gmm=$DCdefpars[$pos];
    $pos+=1;
    $GroupMemsN[$grid-1]=$gmm;
#    print "---> groupID:",$grid," members:",$gmm,"\n";
    for($gm=0;$gm<$gmm;$gm++){#<-- Gr-member(dc-cards types) loop
      $line=$DCdefpars[$pos];
      $pos+=1;
      @arr=split(/\s+/,$line);
      $cmm=$arr[0];
      $cnam=$arr[1];
      push(@MemsNames,$cnam);
#      print "    cardnam=",$cnam," cardmems=",$cmm,"\n"; 
      for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
        $line=$DCdefpars[$pos];
	@arr=split(/\s+/,$line);
	$cmid=$arr[0];
	$val=$arr[1];
#1(MCgn)
	if($cnam eq "KINE"){
	  $KINEcmid[$cm]=$cmid;
	  $KINEcmdf[$cm]=$val;
	  if($cm==0){$KINEparsN=$cmm;}
	  $KINEcmval[$cm]=$val;
	}
	elsif($cnam eq "RNDM"){
	  $RNDMcmid[$cm]=$cmid;
	  $RNDMcmdf[$cm]=$val;
	  if($cm==0){$RNDMparsN=$cmm;}
	  $RNDMcmval[$cm]=$val;
	}
	elsif($cnam eq "DEBUG"){
	  $DEBUGcmid[$cm]=$cmid;
	  $DEBUGcmdf[$cm]=$val;
	  if($cm==0){$DEBUGparsN=$cmm;}
	  $DEBUGcmval[$cm]=$val;
	}
	elsif($cnam eq "CUTS"){
	  $CUTScmid[$cm]=$cmid;
	  $CUTScmdf[$cm]=$val;
	  if($cm==0){$CUTSparsN=$cmm;}
	  $CUTScmval[$cm]=$val;
	}
	elsif($cnam eq "MCGEN"){
	  $MCGENcmid[$cm]=$cmid;
	  $MCGENcmdf[$cm]=$val;
	  if($cm==0){$MCGENparsN=$cmm;}
	  $MCGENcmval[$cm]=$val;
	}
	elsif($cnam eq "GMSRC"){
	  $GMSRCcmid[$cm]=$cmid;
	  $GMSRCcmdf[$cm]=$val;
	  if($cm==0){$GMSRCparsN=$cmm;}
	  $GMSRCcmval[$cm]=$val;
	}
#2(TOF)
	if($cnam eq "TFRE"){
	  $TFREcmid[$cm]=$cmid;
	  $TFREcmdf[$cm]=$val;
	  if($cm==0){$TFREparsN=$cmm;}
	  $TFREcmval[$cm]=$val;
#	  print $TFREparsN," cmem/id/val=",$cm,",",$TFREcmid[$cm],",",$TFREcmval[$cm],"\n"; 
	}
	elsif($cnam eq "TFCA"){
	  $TFCAcmid[$cm]=$cmid;
	  $TFCAcmdf[$cm]=$val;
	  if($cm==0){$TFCAparsN=$cmm;}
	  $TFCAcmval[$cm]=$val;
	}
	elsif($cnam eq "TFMC"){
	  $TFMCcmid[$cm]=$cmid;
	  $TFMCcmdf[$cm]=$val;
	  if($cm==0){$TFMCparsN=$cmm;}
	  $TFMCcmval[$cm]=$val;
	}
	elsif($cnam eq "BETAFIT"){
	  $BETAFITcmid[$cm]=$cmid;
	  $BETAFITcmdf[$cm]=$val;
	  if($cm==0){$BETAFITparsN=$cmm;}
	  $BETAFITcmval[$cm]=$val;
	}
#3(ACC)
	if($cnam eq "ATRE"){
	  $ATREcmid[$cm]=$cmid;
	  $ATREcmdf[$cm]=$val;
	  if($cm==0){$ATREparsN=$cmm;}
	  $ATREcmval[$cm]=$val;
	}
	elsif($cnam eq "ATCA"){
	  $ATCAcmid[$cm]=$cmid;
	  $ATCAcmdf[$cm]=$val;
	  if($cm==0){$ATCAparsN=$cmm;}
	  $ATCAcmval[$cm]=$val;
	}
	elsif($cnam eq "ATMC"){
	  $ATMCcmid[$cm]=$cmid;
	  $ATMCcmdf[$cm]=$val;
	  if($cm==0){$ATMCparsN=$cmm;}
	  $ATMCcmval[$cm]=$val;
	}
#4(EMC)
	if($cnam eq "ECRE"){
	  $ECREcmid[$cm]=$cmid;
	  $ECREcmdf[$cm]=$val;
	  if($cm==0){$ECREparsN=$cmm;}
	  $ECREcmval[$cm]=$val;
	}
	elsif($cnam eq "ECCA"){
	  $ECCAcmid[$cm]=$cmid;
	  $ECCAcmdf[$cm]=$val;
	  if($cm==0){$ECCAparsN=$cmm;}
	  $ECCAcmval[$cm]=$val;
	}
	elsif($cnam eq "ECMC"){
	  $ECMCcmid[$cm]=$cmid;
	  $ECMCcmdf[$cm]=$val;
	  if($cm==0){$ECMCparsN=$cmm;}
	  $ECMCcmval[$cm]=$val;
	}
#5(AMS)
	if($cnam eq "MAGS"){
	  $MAGScmid[$cm]=$cmid;
	  $MAGScmdf[$cm]=$val;
	  if($cm==0){$MAGSparsN=$cmm;}
	  $MAGScmval[$cm]=$val;
	}
	elsif($cnam eq "MISC"){
	  $MISCcmid[$cm]=$cmid;
	  $MISCcmdf[$cm]=$val;
	  if($cm==0){$MISCparsN=$cmm;}
	  $MISCcmval[$cm]=$val;
	}
	elsif($cnam eq "AMSJOB"){
	  $AMSJOBcmid[$cm]=$cmid;
	  $AMSJOBcmdf[$cm]=$val;
	  if($cm==0){$AMSJOBparsN=$cmm;}
	  $AMSJOBcmval[$cm]=$val;
	}
#6(IO)
	if($cnam eq "IOPA"){
	  $IOPAcmid[$cm]=$cmid;
	  $IOPAcmdf[$cm]=$val;
	  if($cm==0){$IOPAparsN=$cmm;}
	  $IOPAcmval[$cm]=$val;
	}
	elsif($cnam eq "DAQC"){
	  $DAQCcmid[$cm]=$cmid;
	  $DAQCcmdf[$cm]=$val;
	  if($cm==0){$DAQCparsN=$cmm;}
	  $DAQCcmval[$cm]=$val;
	}
#7(LVL1)
	if($cnam eq "TGL1"){
	  $TGL1cmid[$cm]=$cmid;
	  $TGL1cmdf[$cm]=$val;
	  if($cm==0){$TGL1parsN=$cmm;}
	  $TGL1cmval[$cm]=$val;
	}
#
#8(EvSel)
	if($cnam eq "ESTA"){
	  $ESTAcmid[$cm]=$cmid;
	  $ESTAcmdf[$cm]=$val;
	  if($cm==0){$ESTAparsN=$cmm;}
	  $ESTAcmval[$cm]=$val;
	}
	elsif($cnam eq "SELE"){
	  $SELEcmid[$cm]=$cmid;
	  $SELEcmdf[$cm]=$val;
	  if($cm==0){$SELEparsN=$cmm;}
	  $SELEcmval[$cm]=$val;
	}
#9(ZFit)
	if($cnam eq "CHAR"){
	  $CHARcmid[$cm]=$cmid;
	  $CHARcmdf[$cm]=$val;
	  if($cm==0){$CHARparsN=$cmm;}
	  $CHARcmval[$cm]=$val;
	}
#10(TRK)
	if($cnam eq "TRFI"){
	  $TRFIcmid[$cm]=$cmid;
	  $TRFIcmdf[$cm]=$val;
	  if($cm==0){$TRFIparsN=$cmm;}
	  $TRFIcmval[$cm]=$val;
	}
#
        $pos+=1;
      }#--> card-mems loop
    }#-->endof Gr-members(dc-cards types) loop
  }#-->endof groups loop
#---
}
#------------------------------------------------------
