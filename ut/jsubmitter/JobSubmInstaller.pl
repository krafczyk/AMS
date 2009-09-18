#!/usr/local/bin/perl
use Tk;
use Tk::Event;
use Time::gmtime;
use Time::localtime;
use IO::Handle;
use Tk::ProgressBar;
use Time::Local;
# use Term::ReadKey;
use File::Copy;
use lib "$ENV{MyCVSTopDir}/perl";
#
#-----> imp.glob.const, vars and arrays:
#
$WarnFlg=0;
$SessName="";
$workdir="";
$RootfDir=$ENV{MyRootfDir};#full path to /rootfiles;
$MCDaqfDir=$ENV{MyMCDaqfDir};#full path to /mcdaqfiles;
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
#
@WDSubDirNames=qw(amsjobwd daqflinks templinks rdlogfiles mclogfiles);
$WDSubDirs=scalar(@WDSubDirNames);
#
@RFSubDirNames=qw(RD MC);
$RFSubDirs=scalar(@RFSubDirNames);
#
@HistFileNames=qw(PrevSessTime SubmJobsList JobDaqfHist);
$HistFiles=scalar(@HistFileNames);
#
@SessTypeNames=qw(RDR MCS MCR);
$SessTypes=scalar(@SessTypeNames);
#
@RefCflListNames=qw(Tof Acc Emc);
$RefCflLists=scalar(@RefCflListNames);
#
$InstallSteps=$WDSubDirs+$RFSubDirs+$HistFiles*$SessTypes-1;#-1 means not existing JobDaqfHist.MCS
$InstStepsDone=0; 
#
$soundtext="Sound-ON";
$PrevSessUTC=0;
$PrevSessTime=0;
$SessTLabel="";
$jobtype="undefined";
$SubDetMsk=0;
$message="";
$CalfDir="";
#
#-----
$prog2run="";
$outlogf="out.log";
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
$mnwdszx=0.8;#X-size of main window (portion of display x-size)
$mnwdszy=0.4;#Y-size of main window (portion of display y-size)
$mwposx="+0";
$mwposy="+0";
my $szx=int($mnwdszx*$displx);#in pixels
my $szy=int($mnwdszy*$disply);
$mwndgeom=$szx."x".$szy.$mwposx.$mwposy;
print "Mwindow geom=",$mwndgeom,"\n";
$mwnd->geometry($mwndgeom);
$mwnd->title("Installer for easy AMS-job Submitter (author E.Choumilov)");
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
                                                      -relwidth=>$LogfXsize, -relheight=>0.9,
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
my ($nbutt,$bspac,$butw,$butww,$labw,$xpos,$ypos,$item,$loop,$bheig);
#
#
#-----> create job-types control panel:
#
@JobTypeNames=qw(Install Update CommitDC);
$JobTypes=scalar(@JobTypeNames);
$nbutt=$JobTypes+3;
$bspac=0.002;
$butw=(1-($nbutt-1)*$bspac)/$nbutt;
$xpos=0;
#
$ctpanel_fr=$mwnd->Frame(-relief=>'groove', -borderwidth=>2,
                                            -background=>red
                                                          )
					       ->place(
                                                      -relwidth=>$LogfXsize, -relheight=>0.1,
                                                      -relx=>(1-$LogfXsize), -rely=>0.9);
#---> label:
$ctpanel_fr->Label(-text=>"SelectJobType :",-font=>$font2,-relief=>'groove',
                                                   -background=>yellow,
                                                   -foreground=>red
						   )
                                            ->place(-relwidth=>$butw, -relheight=>1.,
                                                    -relx=>0, -rely=>0);
$xpos+=$butw+$bspac;
#---> create 1 radio-button for each job-type:
@JobTypButt=();
$JobType="";
$loop=0;
#---
  foreach $item (@JobTypeNames){
    $JobTypButt[$loop]=$ctpanel_fr->Radiobutton(-text => $item, -value => $item,
                                 -font=>$font2, -indicator=>0,
                                 -borderwidth=>3,-relief=>'raised',
			         -selectcolor=>orange,-activeforeground=>red,
			         -activebackground=>yellow, 
                                 -background=>green,-cursor=>hand2,
			         -variable => \$JobType)
			         ->place(
	                            -relwidth=>$butw, -relheight=>1.,
				    -relx=>$xpos, -rely=>0);

   $xpos+=$butw+$bspac;
   $loop+=1; 
  }
#
$JobType=$JobTypeNames[0];# set def. state
#---
$StartButt=$ctpanel_fr->Button(-text=>"Start", -font=>$font2, 
                                   -activebackground=>"orange",
			           -activeforeground=>"red",
			           -foreground=>"red",
			           -background=>"green",
                                   -borderwidth=>3,-relief=>'raised',
			           -cursor=>hand2,
                                   -command => \&StartSess)
			           ->place(
                                   -relwidth=>$butw, -relheight=>0.95,  
                                   -relx=>$xpos, -rely=>0);
#
$xpos+=($butw+$bspac);
# 
$exsess_bt=$ctpanel_fr->Button(-text=>"Exit", -font=>$font2, 
                                   -activebackground=>"orange",
			           -activeforeground=>"red",
			           -foreground=>"red",
			           -background=>"green",
                                   -borderwidth=>3,-relief=>'raised',
			           -cursor=>hand2,
                                   -command => \&ExitSess)
			           ->place(
                                   -relwidth=>$butw, -relheight=>0.95,  
                                   -relx=>$xpos, -rely=>0);
#
#------> create input control pannel:
#
$inpanel_fr=$mwnd->Frame(-relief=>'groove', -borderwidth=>2,
#                                            -background=>red
                                                          )
					       ->place(
                                                      -relwidth=>(1-$LogfXsize), -relheight=>0.8,
                                                      -relx=>0., -rely=>0.);
#---> label:
$xpos=0;
$ypos=0;
$bheig=0.2;
$labw=0.5;
$inpanel_fr->Label(-text=>"Select SubDir of AMSDataDir  
to use as reference to official     :
 calib/config-files storage ",
                                                   -font=>$font2,
						   -relief=>'groove',
                                                   -background=>yellow,
#                                                   -foreground=>red
						   )
                                            ->place(-relwidth=>$labw, -relheight=>$bheig,
                                                    -relx=>$xpos, -rely=>$ypos);
$xpos+=$labw;
#
#---> SubDir buttons:
#
@SubDirNames=qw(v4.00 v5.00);
$SubDirTypes=scalar(@SubDirNames);
$nbutt=$SubDirTypes;
$bspac=0.002;
$butw=(1-$labw-($nbutt-1)*$bspac)/$nbutt;
$SubDirType="";
my $loop=0;
my $item;
#---> "v4.00",...buttons:
foreach $item (@SubDirNames){
  $inpanel_fr->Radiobutton(-text => $item, -value => $item,
                                 -font=>$font2, -indicator=>0,
                                 -borderwidth=>4,-relief=>'raised',
			         -selectcolor=>orange,-activeforeground=>red,
			         -activebackground=>yellow, 
                                 -background=>green,-cursor=>hand2,
			         -variable => \$SubDirType)
			         ->place(
	                            -relwidth=>$butw, -relheight=>$bheig,
				    -relx=>$xpos, -rely=>$ypos);

  $xpos+=($butw+$bspac);
  $loop+=1; 
}
$ypos+=$bheig;
#
#---> "CopyCalFiles" button:
#
$butw=1;
$xpos=0;
$inpanel_fr->Checkbutton(-text=>"Accept creation of private copy of calib/config-files
 for subsystems selected below", -font=>$font2, -indicator=>0,
                                                 -borderwidth=>4,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -command => \&SetSdState,
                                                 -variable=>\$CopyCalFlg)
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$bheig,
						 -relx=>$xpos, -rely=>$ypos);
$CopyCalFlg=0;#def.state
#
$SubDirType=$SubDirNames[0];# set def. state (v4.00)
$ypos+=$bheig;
#
#---> SubDetButt:
#                0   1   2   3   4    5    6    7
@SubDetNames=qw(TRD TOF TRK ACC RICH ECAL LVL1 LVL3);
@SubDetSteps = (0,   5,  0,  3,  0,   4,   1,   1);
$SubDets=scalar(@SubDetNames);
@SubDetFlg=(0,1,0,1,0,1,1,1);
@SubDetMsk=(0,1,0,1,0,1,1,1);#subd.currently in use
@SubDetButp=();
$labw=1.;
$bheig=0.2;
$xpos=0;
$inpanel_fr->Label(-text=>"Select SubSystems to copy files :",
                                                -background=>yellow,
#						-foreground=>red,
                                                -font=>$font2,-relief=>'groove'
						)
                                                ->place(
						-relwidth=>$labw, -relheight=>$bheig,
                                                -relx=>$xpos, -rely=>$ypos);
$ypos+=$bheig;
#--
$butw=2./$SubDets;
$bheig=0.2;
for($i=0;$i<$SubDets;$i++){
  if(($i%4)==0){
    $xpos=0;
    if($i>0){$ypos+=$bheig;}
  }
  if($CopyCalFlg==1 && $SubDetMsk[$i]==1){$state="normal";}
  else{$state="disabled";}
  $SubDetButp[$i]=$inpanel_fr->Checkbutton(-text=>$SubDetNames[$i], -font=>$font2, -indicator=>0,
                                                 -borderwidth=>4,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
						 -state=>"disabled",
                                                 -variable=>\$SubDetFlg[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$bheig,
						 -relx=>$xpos, -rely=>$ypos);
  
  $xpos+=$butw;
}
#--------------
#progress_frame:
$prg_fram=$mwnd->Frame(-relief=>'groove', -borderwidth=>5)->place(
                                                      -relwidth=>(1-$LogfXsize), -relheight=>0.2,
                                                      -relx=>0, -rely=>0.8);
$prg_fram->Label(-text=>"Installation progress(%):"
                                      ,-font=>$font2,-relief=>'groove',
                                                   -background=>yellow,
                                                   -foreground=>red
						   )
                                            ->place(-relwidth=>1., -relheight=>0.5,
                                                    -relx=>0, -rely=>0);
						      
$perc_done=0.;
$prg_bar=$prg_fram->ProgressBar( -width=>10, -from=>0, -to=>100, -blocks=>100,
                                 -colors=>[0,'green'], -gap=> 0,
                                -variable=>\$perc_done)->place(-relwidth=>1., -relheight=>0.5,
                                                             -relx=>0, -rely=>0.5);
#---------------------------------------------------------------------
MainLoop;
#---------------------------------------------------------------------
sub SetSdState{
  my ($state,$i);
  for($i=0;$i<$SubDets;$i++){
    if($CopyCalFlg==1){
      if($SubDetMsk[$i]==1){$state="normal";}
      else{$state="disabled";}
    }
    else{$state="disabled";}
    $SubDetButp[$i]->configure(-state=>$state);
  }
}
#---
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
#
#---> Create "screen" Log-file:
#
  my ($filen,$dir);
  my $logfname="InstallLog.";
  $workdir=$USERTD;
  if($InstStepsDone==0){exit;}
#
  chdir $workdir;
  $WarnFlg=0;
  $filen=$logfname.$SessTLabel;#extention is session time label 
  open(OFN, "> $filen") or show_warn("   <-- Cannot open $filen for writing !");
  print OFN $logtext->get("1.0","end");
  close(OFN) or show_warn("   <-- Cannot close $filen !");
  if($WarnFlg==0){ 
    show_messg("\n   <-- Session LogFile $filen is saved, session is over, bye-bye !");
  }
#------
  $mwnd->update;
  $mwnd->bell;
  $mwnd->after(5000);
  exit;
}
#----------------------------------------------------------------------
sub StartSess{
  my ($stat,$dir,$cmd,$i,$j,$sname,$fname,$frdir,$todir);
  my ($sec,$min,$hour,$day,$month,$year,$ptime,$syear);
  my ($line,$status,$fn,$nel,$fnn,$SessUTC);
  my ($format,$title);
  my @array=();
#
  $workdir=$USERTD;
#--> count total needed steps:
  if($CopyCalFlg==1){
    for($i=0;$i<$SubDets;$i++){
      if($SubDetFlg[$i]>0){$InstallSteps+=$SubDetSteps[$i];}# conditional cal/conf files
    }
  }
#
  for($i=0;$i<$RefCflLists;$i++){
    $InstallSteps+=1;# unconditional copy of RefCflistList-files (for inst/upd, RDR+MCS in one go)
  }
#
  $InstallSteps+=1;#unconditional copy of DCDefPars.txt file(for inst/upd)
  if($JobType eq "Install"){$InstallSteps+=1;}#unconditional copy of StartJobSubmitter file(for install only)
#---
  $ptime=localtime;#current local time(use gmtime for grinv)
  $year=$ptime->year+1900;
  $syear=($year%100);
  $month=$ptime->mon+1;
  $day=$ptime->mday;
  $hour=$ptime->hour;
  $min=$ptime->min;
  $sec=$ptime->sec;
  $SessUTC=timelocal($sec,$min,$hour,$day,$month-1,$year-1900);#or timegm for grinv
  $SessTLabel=$year."-".$month."-".$day."_".$hour.":".$min;
#---
  if($JobType eq "Install" || $JobType eq "Update"){
    if($JobType eq "Install"){
      chdir $workdir;
      for($i=0;$i<$WDSubDirs;$i++){
        $dir=$WDSubDirNames[$i];
        $cmd="mkdir -p ".$dir;#
        $stat=system($cmd);
        if($stat != 0){
          show_warn("\n   <-- Can't create SubDir $dir !!!");
        }
	else{
          show_messg("   <--- SubDir $dir is created !");
	  $InstStepsDone+=1;
          $perc_done=100*($InstStepsDone/$InstallSteps);
	}
        $mwnd->after(250);
        $mwnd->update;
      }
#-
      chdir $RootfDir;
      for($i=0;$i<$RFSubDirs;$i++){
        $dir=$RFSubDirNames[$i];
        $cmd="mkdir -p ".$dir;#
        $stat=system($cmd);
        if($stat != 0){
          show_warn("\n   <-- Can't create SubDir $dir !!!");
        }
	else{
          show_messg("   <--- SubDir $dir is created !");
	  $InstStepsDone+=1;
          $perc_done=100*($InstStepsDone/$InstallSteps);
	}
        $mwnd->after(250);
        $mwnd->update;
      }
#- make history-files:
      $dir=$workdir;
      chdir $dir;
      for($i=0;$i<$HistFiles;$i++){#<--hist-files loop
        $fname=$HistFileNames[$i];
        for($j=0;$j<$SessTypes;$j++){#<--sess-type loop
	  $sname=$SessTypeNames[$j];
	  if($fname eq "JobDaqfHist" && $sname eq "MCS"){next;}
	  $WarnFlg=0;
	  $fn=$fname.".".$sname;
	  if($fname eq "PrevSessTime"){
            open(HISTF,"> $fn") or show_warn("   <-- cannot open $fn $!");
            print HISTF $SessUTC,"\n";
            close(HISTF) or show_warn("   <-- Cannot close $fn after writing $!");
	  }
	  else{
	    $line="";
            open(HISTF,"> $fn") or show_warn("   <-- cannot open $fn $!");
            print HISTF $line;
            close(HISTF) or show_warn("   <-- Cannot close $fn after writing $!");
	  }
	  if($WarnFlg==0){
	    show_messg("   <--- History-file $fn was created !");
	    $InstStepsDone+=1;
            $perc_done=100*($InstStepsDone/$InstallSteps);
	  }
          $mwnd->after(250);
          $mwnd->update;
        }#-->endof sess-type loop
      }#-->endof hist-files loop
#
#--->copy StartJobSubmitter file(unconditional for install mode):
#
      $todir=$workdir;
      $frdir=$AMSCVS."/ut/jsubmitter";
      chdir $frdir;
      $fn="StartJobSubmitter";       
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("\n   <--- CVS-tree $fn file was copied to user dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
    }#-------> endof install-mode check
#
#--->copy RefCflistList files(unconditional for inst/upd mode):
#
    $todir=$workdir."/".$WDSubDirNames[0];# amsjobwd-dir
    $frdir=$AMSDD."/".$SubDirType;
    chdir $frdir;
    for($i=0;$i<$RefCflLists;$i++){
      $fn=$RefCflListNames[$i]."RefCflistList.*";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("\n   <--- Official $fn files copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
    }
#
#-->copy cal/conf-files:
#
    if($CopyCalFlg==1){#<--- copy subdet Cal/Conf files if requested
      $todir=$workdir."/".$WDSubDirNames[0];# amsjobwd-dir
      $frdir=$AMSDD."/".$SubDirType;
      chdir $frdir;
#1
      if($SubDetFlg[1]>0){
      $fn="Tof*RD.*";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("\n   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
#2
      $fn="Tof*MC.*";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
#3
      $fn="TofGeom*.dat";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
#4
      $fn="tof2c*.tsf";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
#5
      $fn="tofp_*.dat";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
      }
#6
      if($SubDetFlg[3]>0){
      $fn="Acc*RD.*";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
#7
      $fn="Acc*MC.*";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
#8
      $fn="antp_*.dat";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
      }
#9
      if($SubDetFlg[5]>0){
      $fn="Ecal*RD.*";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
#10
      $fn="Ecal*MC.*";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
#11
      $fn="EcalAlign*.dat";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
#12
      $fn="eclp_*.dat";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
      }
#13
      if($SubDetFlg[7]>0){
      $fn="TOFT0LVL3.*";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
      }
#14
      if($SubDetFlg[6]>0){
      $fn="Lvl1Conf*.*";
      $status = system("cp $fn $todir");
      if($status != 0){
        show_warn("\n   <-- Can't copy $fn from $frdir !");
      }
      else{
        show_messg("   <--- Official $fn files was copied to local dir $todir !");
        $InstStepsDone+=1;
        $perc_done=100*($InstStepsDone/$InstallSteps);
      }
      $mwnd->after(250);
      $mwnd->update;
      }
    }#-->endof 'Copy subdet Cal/Conf files' check
#
#---> copy DCDefPars file(unconditional for inst/upd):
#
    $todir=$workdir;
    $frdir=$AMSCVS."/ut/jsubmitter";
    chdir $frdir;
#
    $fn="DCDefPars.txt";       
    $status = system("cp $fn $todir");
    if($status != 0){
      show_warn("\n   <-- Can't copy $fn from $frdir !");
    }
    else{
      show_messg("\n   <--- CVS-tree $fn file was copied to user working dir $todir !");
      $InstStepsDone+=1;
      $perc_done=100*($InstStepsDone/$InstallSteps);
    }
    $mwnd->after(250);
    $mwnd->update;
#
    if($InstStepsDone==$InstallSteps){
      show_messg("\n   <--- Installation/Update is successfully done !","B");
    }
    else{
      show_warn("\n   <--- Installation/Update is not successfull: there are failed steps !!!");
      show_warn("        ($InstStepsDone steps were done from $InstallSteps needed)");
    }
  }#-->endof install- or update- mode
#---
  elsif($JobType eq "CommitDC"){# commit DCDefPars.txt file
    $todir=$AMSCVS."/ut/jsubmitter";
    $frdir=$workdir;
    chdir $frdir;
#
    $fn="DCDefPars.txt";       
    $status = system("cp $fn $todir");
    if($status != 0){
      show_warn("\n   <-- Can't copy $fn from $frdir !");
      return;
    }
    else{
      show_messg("\n   <--- Local $fn file was copied to user CVS-tree dir $todir !");
      show_messg("\n   <--- DCDefPars.txt file is successfully commited, you can do  !","B");
      $mwnd->update;
    }
  }
#-----------------------------
  else{
    show_warn("\n   <--- Job Type not defined, do selection first !!!");
    return;
  }
  print "steps=",$InstStepsDone," from ",$InstallSteps,"\n";
#
}
#------------------------------------------------------
