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
use lib qw(/Offline/vdev/perl);
use lib::DBSQLServer;
# use lib::RemoteClient;
#use Audio::Beep;
#use Audio::Data;
#use Audio::Play;
require '.pwdc.pl';
#
#-----> imp.glob.const, vars and arrays:
#
$WarnFlg=0;
$SessName="";
$workdir="";
$amsjwd="/amsjobwd";
$daqflnk="/daqflinks";
$archdir="/archive";
$pedsdir="/pedfiles";
$tausdir="/taucfiles";
$tdcsdir="/tdclfiles";
$AMSDD=$ENV{AMSDataDir};# current AMSDataDir path
$DaqDataDir=$ENV{RunsDir};# current AMS SCI-data dir (/Offline/RunsDir)
$USERTD=$ENV{TofUserTopDir};#full path to /tofassuser
$TofUserN="";
$TofPassW="";
$UNinAtt=0;  
$PWinAtt=0;  
$soundtext="Sound-ON";
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
$AutoSessFlg="";
$DBAutoUpdFlg="";
$JobScriptN="";
$KilledJobs=0;
@KilledJobsPids=();
$SingleFileJob=0;
$CalRun1=0;#1st run in calibr.files set used in "submit job" procedure
$CalRun2=0;#last run in calibr.files set ............................
$jobtype="undefined";
$SubDetMsk=0;
$message="";
#
$TDVName="";
$CalfDir="";
$SetupName="";
$RefCFileData=();
#
#---> formats:
$SubmJobsFmt="A8 A9 A5 A12 A12 A12 A7 A11 A7 A3";#for pack/unpack subm.jobs.info line in hist.file
#
$LogfListFmt="A6 A4 A11 A9 A9 A4";#for pack/unpack of log-files list(sjpid ccode 1strunn dd.hh:mm utcmin status)
$LogfLboxFmt="A6 A4 A11 A9 A4";#for log-file listbox(sjpid ccode 1strunn dd.hh:mm status)
#
$CalfListFmt="A11 A9 A9 A3";#for cal-files listbox-description(1strunn dd.hh:mm utcmin status)
$CalfLboxFmt="A11 A9 A3";#for cal-files listbox-description(1strunn dd.hh:mm status)
#
$BjobsFmt="A8 A8 A7 A10 A12 A12 A13 A14";#for packing message of "bjobs"
#
@AMSSetupsList=qw(AMS02PreAss AMS02Ass1 AMS02Space);
@AMSHostsList=qw(Any scamsfs1 pcamsr0 pcamsr1 pcamsn0 pcamsf3 pcamss0 );
@AMSQueuesList=qw(1nh 8nh 1nd 1nw 2nw);
#
@cfilenames=qw(TofCflistRD TofCStatRD TofTdelvRD TofTzslwRD TofAmplfRD TofElospRD TofTdcorRD); 
#
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
#
$displx=$dsets[1];
$disply=$dsets[3];
$mnwdszx=0.9;#X-size of main window (portion of display x-size)
$mnwdszy=0.6;#Y-size of main window (portion of display y-size)
$mwposx="+0";
$mwposy="+0";
my $szx=int($mnwdszx*$displx);#in pixels
my $szy=int($mnwdszy*$disply);
$mwndgeom=$szx."x".$szy.$mwposx.$mwposy;
print "Mwindow geom=",$mwndgeom,"\n";
$mwnd->geometry($mwndgeom);
$mwnd->title("TOF Calibration Commander");
$mwnd->bell;
#--------------
#---> fonts:
$font1="Helvetica 14 normal italic";#font for entry-widgets
$font2="times 14 bold";
$font2a="times 12 bold";
$font2b="times 10 bold";
$font2c="times 16 bold";
$font3="Helvetica 12 bold italic";#smaller font for entry-widgets
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
#--------------
#
#-----> create log-file screen:
#
$LogfXsize=0.6;
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
$logtext->tagConfigure('BigAttention',-foreground=> red, -font=>$font9);
$logtext->tagConfigure('BigMessage',-foreground=> blue, -font=>$font10);
$logtext->tagConfigure('Message',-foreground=> black, -font=>$font11);
$logtext->tagConfigure('ItalText',-font=>$font4);
$logtext->tagConfigure('Text',-font=>$font4);
$logtext->tagConfigure('Yellow',-foreground=> yellow, -font=>$font4);
$logtext->tagConfigure('Orange',-foreground=> orange, -font=>$font4);
$logtext->tagConfigure('Blue',-foreground=> blue, -font=>$font4);
$logtext->tagConfigure('Green',-foreground=> green, -font=>$font4);
#-------------
my $nbutt=9;
my $bspac=0.002;
my $width=(1-($nbutt-1)*$bspac)/$nbutt;
my $bpos=0;
#
#-----> create calib-types control panel:
#
$ctpanel_fr=$mwnd->Frame(-relief=>'groove', -borderwidth=>2,
                                            -background=>red
                                                          )
					       ->place(
                                                      -relwidth=>$LogfXsize, -relheight=>0.05,
                                                      -relx=>(1-$LogfXsize), -rely=>0.95);
#---> label:
$ctpanel_fr->Label(-text=>"SessType:",-font=>$font2,-relief=>'groove',
                                                   -background=>yellow,
                                                   -foreground=>red
						   )
                                            ->place(-relwidth=>$width, -relheight=>0.95,
                                                    -relx=>0, -rely=>0);
$bpos+=$width+$bspac;
#---> create 6 radio-buttons for each cal-type:
@CalTypeButt=();
$CalType="";
my $loop=0;
foreach (qw(PedCal Ped2DB TdcLin TdcL2DB TAUCal TAU2DB)){
  $CalTypButt[$loop]=$ctpanel_fr->Radiobutton(-text => $_, -value => $_,
                                 -font=>$font2, -indicator=>0,
                                 -borderwidth=>3,-relief=>'raised',
			         -selectcolor=>orange,-activeforeground=>red,
			         -activebackground=>yellow, 
                                 -background=>green,-cursor=>hand2,
			         -variable => \$CalType)
			         ->place(
	                            -relwidth=>$width, -relheight=>0.95,
				    -relx=>$bpos, -rely=>0);

 $bpos+=$width+$bspac;
 $loop+=1; 
}
#---> StartSess-butt:
$StartButt=$ctpanel_fr->Button(-text=>"Start", -font=>$font2, 
                                   -activebackground=>"orange",
			           -activeforeground=>"red",
			           -foreground=>"black",
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
$exsess_bt=$ctpanel_fr->Button(-text=>"Exit", -font=>$font2, 
                                   -activebackground=>"orange",
			           -activeforeground=>"red",
			           -foreground=>"black",
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
  $message=$text;
  $logtext->insert('end',$text."\n",'Attention');
  $logtext->yview('end');
  $count=2;
  if($soundtext eq "Sound-ON"){
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
  if($soundtext eq "Sound-ON"){
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
  if($soundtext eq "Sound-ON"){$mwnd->bell;}
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
  if($soundtext eq "Sound-ON"){ 
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
  $JobDaqfHistFN="TofDaqfHist";#generic, each sess.add uniqe extention
  $SubmJobCount=0;# reset on session start, incr. on each job-submitting
  $FiniJobCount=0;# reset on each "JobOutpControl"-click, incr. when finished jobs found(st=20 after 'bjobs')
  $FinishedJobs=0;# reset on session start, incr. in each "JobOutpControl"-click when finished jobs found 
  $JobConfLogFiles=0;#reset on "JOC"-click, incr. when log-file found and job-confirmed(in "JobOutpControl") 
  $UnFinishedJobs=0;# set on each "Welcome"-call by analizing the SubmJobsList-file
  if($CalType eq "PedCal"){
    $SessName="PEDC";
    $workdir=$USERTD."/headquarters";
    $JobScriptN="JScriptTofPEDC";
    require 'TofAccPedCalculator.pl';
    PedCalibration();
#
    PEDC_Welcome();
  }
#------------------------------
  elsif($CalType eq "Ped2DB"){
    $SessName="PEDCU";
    $workdir=$USERTD."/headquarters";
    $JobScriptN="JScriptTofP2DB";
    require 'TofAccPedDBWriter.pl';
    Ped2DBWriter();
    $LoginDone=0;
#
    PEDC2DB_Welcome();
  }
#------------------------------
  elsif($CalType eq "TdcLin"){
    $SessName="TDCL";
    $workdir=$USERTD."/headquarters";
    $JobScriptN="JScriptTofTDCL";
    $ResultsBlockLen=6;#<-- TDCL(TAUC) log-file results block length (incl.header/trailer)
#
    @RefCflistList=();
    @RefCStatList=();
    @RefTdelvList=();
    @RefTzslwList=();
    @RefAmplfList=();
    @RefElospList=();
    @RefTdcorList=();
#
    @CflistList=();
    @CStatList=();
    @TdelvList=();
    @TzslwList=();
    @AmplfList=();
    @ElospList=();
    @TdcorList=();
#
    $MatchCFSets=0;
    @MatCflistInd=();
    @MatCStatInd=();
    @MatTdelvInd=();
    @MatTzslwInd=();
    @MatAmplfInd=();
    @MatElospInd=();
    @MatTdcorInd=(); 
# 
    @CflistLbox=();
    @CStatLbox=();
    @TdelvLbox=();
    @TzslwLbox=();
    @AmplfLbox=();
    @ElospLbox=();
    @TdcorLbox=();
#
    @LogJobMatchIndex=();
#
    require 'TofTdcLinCalib.pl';
    $AutoSessFlg="ManualSess";
    $DBAutoUpdFlg="DBAutoUpd_Off";
    TdcLinCalibration();
    $JOCReady=0;
#
    TDCL_Welcome();
  }
#-----------------------------
  elsif($CalType eq "TdcL2DB"){
    $SessName="TDCLU";
    $TDVName="TofTdcCor";
    $workdir=$USERTD."/headquarters";
    $JobScriptN="JScriptTofTAL2DB";
    $CalfDir=$tdcsdir;
#
    @RefCflistList=();
    @RefCStatList=();
    @RefTdelvList=();
    @RefTzslwList=();
    @RefAmplfList=();
    @RefElospList=();
    @RefTdcorList=();
#
    @CflistList=();
    @CStatList=();
    @TdelvList=();
    @TzslwList=();
    @AmplfList=();
    @ElospList=();
    @TdcorList=();
#
    @CflistLbox=();
    @CStatLbox=();
    @TdelvLbox=();
    @TzslwLbox=();
    @AmplfLbox=();
    @ElospLbox=();
    @TdcorLbox=();
#
    require 'TofTAULDBWriter.pl';
#
    $AutoSessFlg="ManualSess";
    TAUL2DBWriter();
    $LoginDone=0;
#
    TAUL2DB_Welcome();
  }
#------------------------------
  elsif($CalType eq "TAUCal"){
    $SessName="TAUC";
    $workdir=$USERTD."/headquarters";
    $JobScriptN="JScriptTofTAUC";
    $ResultsBlockLen=6;#<-- TAUC(TDCL) log-file results block length (incl.header/trailer)
#
    @RefCflistList=();
    @RefCStatList=();
    @RefTdelvList=();
    @RefTzslwList=();
    @RefAmplfList=();
    @RefElospList=();
    @RefTdcorList=();
#
    @CflistList=();
    @CStatList=();
    @TdelvList=();
    @TzslwList=();
    @AmplfList=();
    @ElospList=();
    @TdcorList=();
#
    $MatchCFSets=0;
    @MatCflistInd=();
    @MatCStatInd=();
    @MatTdelvInd=();
    @MatTzslwInd=();
    @MatAmplfInd=();
    @MatElospInd=();
    @MatTdcorInd=(); 
# 
    @CflistLbox=();
    @CStatLbox=();
    @TdelvLbox=();
    @TzslwLbox=();
    @AmplfLbox=();
    @ElospLbox=();
    @TdcorLbox=();
#
    @LogJobMatchIndex=();
#
    require 'TofTimeAmplCalib.pl';
#
    $AutoSessFlg="ManualSess";
    $DBAutoUpdFlg="DBAutoUpd_Off";
    TmAmCalibration();
    $JOCReady=0;
#
    TAUC_Welcome();
  }
#-----------------------------
  elsif($CalType eq "TAU2DB"){
    $SessName="TAUCU";
    $TDVName="Tofbarcal2";
    $workdir=$USERTD."/headquarters";
    $JobScriptN="JScriptTofTAL2DB";
    $CalfDir=$tausdir;
#
    @RefCflistList=();
    @RefCStatList=();
    @RefTdelvList=();
    @RefTzslwList=();
    @RefAmplfList=();
    @RefElospList=();
    @RefTdcorList=();
#
    @CflistList=();
    @CStatList=();
    @TdelvList=();
    @TzslwList=();
    @AmplfList=();
    @ElospList=();
    @TdcorList=();
#
    @CflistLbox=();
    @CStatLbox=();
    @TdelvLbox=();
    @TzslwLbox=();
    @AmplfLbox=();
    @ElospLbox=();
    @TdcorLbox=();
#
    require 'TofTAULDBWriter.pl';
#
    $AutoSessFlg="ManualSess";
    TAUL2DBWriter();
    $LoginDone=0;
#
    TAUL2DB_Welcome();
  }
#-----------------------------
  else{
    show_warn("\n   <--- Calibration Type not defined, do selection first !!!");
  }
#--> disable CalType/Start-buttons:
  my $state="disabled";
  for($i=0;$i<6;$i++){
    $CalTypButt[$i]->configure(-state=>$state);
  }
  $StartButt->configure(-state=>$state);
}
#----------------------------------------------------------------------
