sub TdcLinCalibration
{
#---
my $dirfrheight=0.35;
my $setfrheight=0.57;
my $jclfrheight=0.08;
my ($bwid,$bheig,$xpos,$ypos,$lbwid,$labw,$butw);

#--------------
#dir_frame:
my $shf1=0.1;#down shift for dir-widgets
my $drh1=(1-$shf1)/5;#height of dir-widgets
#---
$dir_fram=$mwnd->Frame(-label => "TdcLinearityCalibration - General Info/Settings:",-background => "Grey",
                                                      -relief=>'groove', -borderwidth=>5)
						      ->place(
                                                      -relwidth=>(1-$LogfXsize), -relheight=>$dirfrheight,
                                                      -relx=>0, -rely=>0);
#------
$wrd_lab=$dir_fram->Label(-text=>"HeadD:",-font=>$font2,-relief=>'groove')
                                                ->place(
						-relwidth=>0.2, -relheight=>$drh1,
                                                -relx=>0, -rely=>$shf1);
$wrd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$font1,
                                              -textvariable=>\$workdir)
					      ->place(
                                              -relwidth=>0.8, -relheight=>$drh1,  
                                              -relx=>0.2, -rely=>$shf1);
#------
$amsjd_lab=$dir_fram->Label(-text=>"AmsJD:",-font=>$font2,-relief=>'groove')
                                                    ->place(
						    -relwidth=>0.2, -relheight=>$drh1,
                                                    -relx=>0.5, -rely=>($shf1+$drh1));
$amsjd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$font1,
                                               -textvariable=>\$amsjwd)
					       ->place(
                                               -relwidth=>0.3, -relheight=>$drh1,  
                                               -relx=>0.7, -rely=>($shf1+$drh1));
#---
$taufd_lab=$dir_fram->Label(-text=>"TDCfD:",-font=>$font2,-relief=>'groove')
                                                    ->place(
						    -relwidth=>0.2, -relheight=>$drh1,
                                                    -relx=>0, -rely=>($shf1+$drh1));
$pedsd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$font1,
                                               -textvariable=>\$tdcsdir)->place(
                                               -relwidth=>0.3, -relheight=>$drh1,  
                                               -relx=>0.2, -rely=>($shf1+$drh1));
#--------------
$amsg_lab=$dir_fram->Label(-text=>"AmsDD:",-font=>$font2,-relief=>'groove')
                                                         ->place(
                                                         -relwidth=>0.2, -relheight=>$drh1,
                                                         -relx=>0, -rely=>($shf1+2*$drh1));
$amsg_ent=$dir_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$font1,
                                              -textvariable=>\$AMSDD)
					      ->place(
                                              -relwidth=>0.8, -relheight=>$drh1,  
                                              -relx=>0.2, -rely=>($shf1+2*$drh1));
#-------------
$lookar_bt=$dir_fram->Button(-text=>"LookAround", -font=>$font2, 
                                         -activebackground=>"yellow",
			                 -activeforeground=>"red",
			                 -foreground=>"red",
			                 -background=>"green",
                                         -borderwidth=>3,-relief=>'raised',
			                 -cursor=>hand2,
                                         -command => \&LookAround)
			                 ->place(
                                         -relwidth=>0.5, -relheight=>$drh1,  
                                         -relx=>0, -rely=>($shf1+3*$drh1));
$lookar_bt->bind("<Button-3>", \&lookaround_help);
#---
$clrhist_bt=$dir_fram->Button(-text=>"ResetHistory", -font=>$font2, 
                                         -activebackground=>"yellow",
			                 -activeforeground=>"red",
			                 -foreground=>"yellow",
			                 -background=>"red",
                                         -borderwidth=>3,-relief=>'raised',
			                 -cursor=>hand2,
                                         -command => \&ResetHistory)
			                 ->place(
                                         -relwidth=>0.5, -relheight=>$drh1,  
                                         -relx=>0.5, -rely=>($shf1+3*$drh1));
$clrhist_bt->bind("<Button-3>", \&resethist_help);
#---
$dir_fram->Button(-text=>"Help", -font=>$font2, 
                                         -activebackground=>"yellow",
			                 -activeforeground=>"red",
			                 -foreground=>"red",
			                 -background=>"green",
                                         -borderwidth=>3,-relief=>'raised',
			                 -cursor=>hand2,
                                         -command => \&open_help_window)
			                 ->place(
                                         -relwidth=>0.15, -relheight=>$drh1,  
                                         -relx=>0., -rely=>($shf1+4*$drh1));
#---
$soundtext="Sound-ON";
$dir_fram->Button(-text=>"Sound-ON", -font=>$font2, 
                                     -activebackground=>"yellow",
			             -activeforeground=>"red",
			             -foreground=>"red",
			             -background=>"green",
                                     -borderwidth=>3,-relief=>'raised',
			             -cursor=>hand2,
			             -textvariable=>\$soundtext,
                                     -command => sub{if($soundtext eq "Sound-ON"){$soundtext="Sound-OFF";}
			                             else {$soundtext="Sound-ON";}})
			             ->place(
                                     -relwidth=>0.25, -relheight=>$drh1,
				     -relx=>0.15, -rely=>($shf1+4*$drh1));
#---
$manauto_bt=$dir_fram->Button(-text=>"ManualSess", -font=>$font2, 
                                     -activebackground=>"yellow",
			             -activeforeground=>"red",
			             -foreground=>"darkgreen",
			             -background=>"orange",
                                     -borderwidth=>3,-relief=>'raised',
			             -cursor=>hand2,
			             -textvariable=>\$AutoSessFlg,
                                     -command => sub{if($AutoSessFlg eq "ManualSess"){$AutoSessFlg="AutoSess";}
			                             else {$AutoSessFlg="ManualSess";}})
			             ->place(
                                     -relwidth=>0.3, -relheight=>$drh1,
				     -relx=>0.4, -rely=>($shf1+4*$drh1));
$manauto_bt->bind("<Button-3>", \&manauto_help);
#---
$dbautoupd_bt=$dir_fram->Button(-text=>"DBAutoUpd_Off", -font=>$font2, 
                                     -activebackground=>"yellow",
			             -activeforeground=>"red",
			             -foreground=>"darkgreen",
			             -background=>"orange",
                                     -borderwidth=>3,-relief=>'raised',
			             -cursor=>hand2,
			             -textvariable=>\$DBAutoUpdFlg,
                                     -command => sub{if($DBAutoUpdFlg eq "DBAutoUpd_Off"){$DBAutoUpdFlg="DBAutoUpd_On";}
			                             else {$DBAutoUpdFlg="DBAutoUpd_Off";}})
			             ->place(
                                     -relwidth=>0.3, -relheight=>$drh1,
				     -relx=>0.7, -rely=>($shf1+4*$drh1));
#-----------------
sub ResetHistory
{
  my $fn,$dir,$cmd,$cmdstat;
  my $dflinksTD=$workdir.$daqflnk;
  my $dflinksSD="/JL*";
#
  if($ResetHistFlg==0){
    show_warn("\n   <--- Are you shure ? If yes - click again 'ResetHistory' !!!");
    $ResetHistFlg=1;#now reset is allowed
  }
  else{
    show_messg("\n   <========= ResetHistory request is confirmed ...","B");
    @SubmJobsList=();
    $fn=$workdir."/"."TofSubmJobsList".".".$SessName;
    truncate($fn,0) or show_warn("\n   <--- Can't reset history-file $fn !!!($!)");
    show_messg("\n   <--- JobSubmissionHistory Reset is successful in file $fn !!!");
    $UnFinishedJobs=0;
#
    @DaqfHist=();
    $fn=$workdir."/".$JobDaqfHistFN.".".$SessName;
    truncate($fn,0) or show_warn("\n   <--- Can't reset used DAQ-files history-file $fn !!!($!)");
    show_messg("\n   <--- UsedDaqFilesHistory Reset is successful in file $fn !!!");
#
    $dir=$dflinksTD.$dflinksSD;
    $cmd="rm -rf $dir";
    $cmdstat=system($cmd);
    if($cmdstat != 0){
      show_warn("\n   <--- Failed to remove all link-directories !!!");
    }
    else{show_messg("\n   <--- Command 'delete' all link-directories is successful !");}
#
    $ResetHistFlg=0;
  }
}
#--------------------------------------------------------------------------
#run-conditions_set_frame:
my $shf2=0.06;#down shift for runcond-widgets
#my $drh2=0.093;#height of runcond-widgets
my $drh2=(1-$shf2)/10;#height of runcond-widgets
#---
$set_fram=$mwnd->Frame(-label=>"Setup New Job :",-relief=>'groove', -borderwidth=>5,
                                                  -background => "blue")
						  ->place(
                                                  -relwidth=>(1-$LogfXsize), -relheight=>$setfrheight,
                                                  -relx=>0, -rely=>$dirfrheight);
#------
$set_fram->Label(-text=>"SelAmsState:",-font=>$font2,-relief=>'groove')
                                                  ->place(
						  -relwidth=>0.34, -relheight=>$drh2,
                                                  -relx=>0, -rely=>$shf2);
#---
$magstext="MagnetON";
$set_fram->Button(-text=>"MagnetON", -font=>$font2, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
#			      -foreground=>"red",
			      -background=>"green",
                              -borderwidth=>3,-relief=>'raised',
			      -cursor=>hand2,
			      -textvariable=>\$magstext,
                              -command => sub{if($magstext eq "MagnetON"){$magstext="MagnetOFF";}
			                      else {$magstext="MagnetON";}})
			         ->place(
                                 -relwidth=>0.33, -relheight=>$drh2,
                                 -relx=>0.34, -rely=>$shf2);
#---
$posstext="InSpace";
$set_fram->Button(-text=>"InSpace", -font=>$font2, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
#			      -foreground=>"red",
			      -background=>"green",
                              -borderwidth=>3,-relief=>'raised',
			      -cursor=>hand2,
			      -textvariable=>\$posstext,
                              -command => sub{if($posstext eq "InSpace"){$posstext="OnEarth";}
			                      else {$posstext="InSpace";}})
			         ->place(
                                 -relwidth=>0.33, -relheight=>$drh2,
			         -relx=>0.67, -rely=>$shf2);
#--------------
$ctypselflag=0;#1st selection flag
$set_fram->Label(-text=>"CalibMode:",-font=>$font2,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.25, -relheight=>$drh2,
                                               -relx=>0, -rely=>($shf2+$drh2));
#---
$CalibMode="Normal";
$mode1_bt=$set_fram->Radiobutton(-text=>"AllSFET",-font=>$font2, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>"Normal", -variable=>\$CalibMode)
                                              ->place(
						      -relwidth=>0.2, -relheight=>$drh2,
						      -relx=>0.25, -rely=>($shf2+$drh2));
#---
$mode2_bt=$set_fram->Radiobutton(-text=>"OneSFET",-font=>$font2, -indicator=>0, 
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red, 
						 -activebackground=>yellow, 
                                                 -background=>green, 
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>"Econom", -variable=>\$CalibMode)
                                               ->place(
						       -relwidth=>0.2, -relheight=>$drh2,
						        -relx=>0.45, -rely=>($shf2+$drh2));
#---
$set_fram->Label(-text=>"PassType:",-font=>$font2,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.25, -relheight=>$drh2,
                                               -relx=>0.65, -rely=>($shf2+$drh2));
#---
$PassType="1";#single pass job(2,3,4 for multipass,i.e with interm.file) 
$set_fram->Entry(-relief=>'sunken', -background=>yellow,
                                    -font=>$font3,
                                    -textvariable=>\$PassType)
				    ->place(
                                    -relwidth=>0.1, -relheight=>$drh2,  
                                    -relx=>0.9, -rely=>($shf2+$drh2));
#--------------
$setdpar_bt=$set_fram->Button(-text=>"SetDefPars", -font=>$font2, 
                                   -activebackground=>"yellow",
			           -activeforeground=>"red",
			           -foreground=>"red",
			           -background=>"green",
                                   -borderwidth=>3,-relief=>'raised',
			           -cursor=>hand2,
                                   -command => \&SetDefaultPars)
			           ->place(
                                   -relwidth=>0.3, -relheight=>$drh2,  
                                   -relx=>0, -rely=>($shf2+2*$drh2));
$setdpar_bt->bind("<Button-3>", \&setdefpar_help);
#---
$set_fram->Label(-text=>"Queue:",-font=>$font2,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.15, -relheight=>$drh2,
                                               -relx=>0.3, -rely=>($shf2+2*$drh2));
#
$Queue2run="1nd";#queue name the job sould be submitted 
$set_fram->Optionmenu(-textvariable => \$Queue2run, 
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$font3,
                               -options => [@AMSQueuesList],
	                       )
                               ->place(
                               -relwidth=>0.2, -relheight=>$drh2,  
                               -relx=>0.45, -rely=>($shf2+2*$drh2));
					       
#---
$set_fram->Label(-text=>"CPUs to use:",-font=>$font2,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.25, -relheight=>$drh2,
                                               -relx=>0.65, -rely=>($shf2+2*$drh2));
$ncpus="1";#number of CPUs to use 
$set_fram->Entry(-relief=>'sunken', -background=>yellow,
                                    -font=>$font3,
                                    -textvariable=>\$ncpus)
				    ->place(
                                    -relwidth=>0.1, -relheight=>$drh2,  
                                    -relx=>0.9, -rely=>($shf2+2*$drh2));
#-------------
$set_fram->Label(-text=>"RefCalibSetN:",-font=>$font2,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.3, -relheight=>$drh2,
                                               -relx=>0, -rely=>($shf2+3*$drh2));
#---
$refcalsetn="0";#ref.calib set number
$refcfl_om=$set_fram->Optionmenu(-textvariable => \$refcalsetn, 
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$font3,
                               -options => [$refcalsetn]
	                       )
                               ->place(
                               -relwidth=>0.3, -relheight=>$drh2,  
                               -relx=>0.3, -rely=>($shf2+3*$drh2));

 
#---
$set_fram->Label(-text=>"Events:",-font=>$font2,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.2, -relheight=>$drh2,
                                               -relx=>0.6, -rely=>($shf2+3*$drh2));
$Evs2Read="1000000";#number of events to read 
$set_fram->Entry(-relief=>'sunken', -background=>yellow,
                                    -font=>$font3,
                                    -textvariable=>\$Evs2Read)
				    ->place(
                                    -relwidth=>0.2, -relheight=>$drh2,  
                                    -relx=>0.8, -rely=>($shf2+3*$drh2));
#-------------
$refvel_lab=$set_fram->Label(-text=>"Evs/Bin(Min):",-font=>$font2,-relief=>'groove')
                                                    ->place(
						    -relwidth=>0.22, -relheight=>$drh2,
                                                    -relx=>0, -rely=>($shf2+4*$drh2));
$BinMinEvs="0";
$binmine_state="disabled";
$binmine_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",
                                              -background=>yellow,
                                              -font=>$font3,
					      -state=>$binmine_state,
                                              -textvariable=>\$BinMinEvs)->place(
                                              -relwidth=>0.08, -relheight=>$drh2,  
                                              -relx=>0.22, -rely=>($shf2+4*$drh2));
#---
$cfilesloc="10101";#(def) 10-base BitPattern Msb(tdcl elos dcrd ped cal)Lsb (=0 to use DB)
@TofDBUsePatt=();
$j=1;
for($i=0;$i<5;$i++){
  $TofDBUsePatt[$i]=1-(($cfilesloc/$j)%10);# 10-base BitPattern Msb(tdcl elos dcrd ped cal)Lsb (=1 to use BD)
  $j=10*$j;
}
my @tofdbdatatypes=qw(Cal Ped DCut Elos TdcL);
$labw=0.15;
$butw=0.11;
$xpos=0.3;
$cfloc_lab=$set_fram->Label(-text=>"UseDB4:",-font=>$font2,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh2,
                                                -relx=>$xpos, -rely=>($shf2+4*$drh2));
$xpos+=$labw;
for($i=0;$i<5;$i++){
  $set_fram->Checkbutton(-text=>$tofdbdatatypes[$i], -font=>$font2, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TofDBUsePatt[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh2,
						 -relx=>$xpos, -rely=>($shf2+4*$drh2));
  $xpos+=$butw;
}
#-------------
$dat1_lab=$set_fram->Label(-text=>"Date :",-font=>$font2,-relief=>'groove')
                                         ->place(
					 -relwidth=>0.12, -relheight=>$drh2,
                                         -relx=>0, -rely=>($shf2+5*$drh2));
$fdat1="2009.11.10 16:30:06";#def. file-date-from 
$fdat1_ent=$set_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$font3,
                                               -textvariable=>\$fdat1)
					       ->place(
                                               -relwidth=>0.43, -relheight=>$drh2,  
                                               -relx=>0.12, -rely=>($shf2+5*$drh2));
#---
$dat2_lab=$set_fram->Label(-text=>"-",-font=>$font2)
                                      ->place(
				      -relwidth=>0.02, -relheight=>$drh2,
                                      -relx=>0.55, -rely=>($shf2+5*$drh2));
$fdat2="2015.01.01 00:00:01";#def. file-date-till 
$fdat2_ent=$set_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$font3,
                                               -textvariable=>\$fdat2)
					       ->place(
                                               -relwidth=>0.43, -relheight=>$drh2,  
                                               -relx=>0.57, -rely=>($shf2+5*$drh2));
#---------
$num1_lab=$set_fram->Label(-text=>"RunN :",-font=>$font2,-relief=>'groove')
                                         ->place(
					 -relwidth=>0.16, -relheight=>$drh2,
                                         -relx=>0, -rely=>($shf2+6*$drh2));
$fnum1="1257867006";#def. file-number-from 
$fnum1_ent=$set_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$font3,
                                               -textvariable=>\$fnum1)->place(
                                               -relwidth=>0.4, -relheight=>$drh2,  
                                               -relx=>0.16, -rely=>($shf2+6*$drh2));
#---
$num2_lab=$set_fram->Label(-text=>"-",-font=>$font2)
                                     ->place(
				     -relwidth=>0.04, -relheight=>$drh2,
				     -relx=>0.56, -rely=>($shf2+6*$drh2));
$fnum2="1500000000";#def. file-num-till 
$fnum2_ent=$set_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$font3,
                                               -textvariable=>\$fnum2)
					       ->place(
                                               -relwidth=>0.4, -relheight=>$drh2,  
                                               -relx=>0.6, -rely=>($shf2+6*$drh2));
$fnum1o=$fnum1;
$fnum2o=$fnum2;
$fdat1o=$fdat1;
$fdat2o=$fdat2;
#-------------
$convert_bt=$set_fram->Button(-text=>"ConfirmRunsRange", -font=>$font2, 
                                      -activebackground=>"yellow",
			              -activeforeground=>"red",
			              -foreground=>"red",
			              -background=>"green",
                                      -borderwidth=>5,-relief=>'raised',
			              -cursor=>hand2,
                                      -command => \&RunDateConv)
			              ->place(
                                      -relwidth=>0.5, -relheight=>$drh2,
				      -relx=>0, -rely=>($shf2+7*$drh2));
$convert_bt->bind("<Button-3>", \&convert_help);
#---
$set_fram->Label(-text=>"                    ",-font=>$font2,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.5, -relheight=>$drh2,
                                               -relx=>0.5, -rely=>($shf2+7*$drh2));
#--------------
$scanbt_state="disabled";
$scanbt=$set_fram->Button(-text => "ScanDaqDir", -font=>$font2, 
                                                 -activebackground=>"yellow",
			                         -activeforeground=>"red",
			                         -foreground=>"red",
			                         -background=>"green",
                                                 -borderwidth=>5,-relief=>'raised',
			                         -cursor=>hand2,
			                         -state=>$scanbt_state,
                                                 -command => \&scanddir)
						 ->place(
                                                 -relwidth=>0.3,-relheight=>$drh2,
                                                 -relx=>0,-rely=>($shf2+8*$drh2));
$scanbt->bind("<Button-3>", \&scanddir_help);
#---
$edittext="EditRlist";
$editbt=$set_fram->Button(-text=>"EditRlist", -font=>$font2, 
                                              -activebackground=>"yellow",
			                      -activeforeground=>"red",
			                      -foreground=>"red",
			                      -background=>"green",
                                              -borderwidth=>5,-relief=>'raised',
			                      -cursor=>hand2,
			                      -textvariable=>\$edittext,
                                              -command =>\&edit_rlist)
					      ->place(
                                              -relwidth=>0.3, -relheight=>$drh2,
				              -relx=>0.3, -rely=>($shf2+8*$drh2));
$editbt->bind("<Button-3>", \&editrlist_help);
#---
$set_fram->Label(-text=>"UseHost:",-font=>$font2,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.2, -relheight=>$drh2,
                                               -relx=>0.6, -rely=>($shf2+8*$drh2));
$Host2run="Any";
$set_fram->Optionmenu(-textvariable => \$Host2run, 
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$font3,
                               -options => [@AMSHostsList],
	                       )
                               ->place(
                               -relwidth=>0.2, -relheight=>$drh2,  
                               -relx=>0.8, -rely=>($shf2+8*$drh2));
#--------------
$set_fram->Button(-text=>"EditJobScript", -font=>$font2, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&EditJobScript)
			                  ->place(
                                          -relwidth=>0.35, -relheight=>$drh2,  
                                          -relx=>0, -rely=>($shf2+9*$drh2));
#---
$setupj_state="disabled";
$setupj_bt=$set_fram->Button(-text=>"SetupJob", -font=>$font2, 
                                   -activebackground=>"yellow",
			           -activeforeground=>"red",
			           -foreground=>"red",
			           -background=>"orange",
                                   -borderwidth=>3,-relief=>'raised',
			           -cursor=>hand2,
			           -state=>$setupj_state,
                                   -command => \&SetupJob)
			           ->place(
                                   -relwidth=>0.325, -relheight=>$drh2,  
                                   -relx=>0.35, -rely=>($shf2+9*$drh2));
$setupj_bt->bind("<Button-3>", \&setupjob_help);
#---
$sjobbt_state="disabled";
$sjobbt=$set_fram->Button(-text => "SubmitJob", -font=>$font2, 
                                                -activebackground=>"yellow",
			                        -activeforeground=>"red",
			                        -background=>"green",
			                        -foreground=>"red",
                                                -borderwidth=>3,-relief=>'raised',
			                        -cursor=>hand2,
						-state=>$sjobbt_state,
                                                -command => \&SubmitJob)
						->place(
                                                -relwidth=>0.325,-relheight=>$drh2,
                                                -relx=>0.675,-rely=>($shf2+9*$drh2));
$sjobbt->bind("<Button-3>", \&submitjob_help);
#--------------
# $bellb=$set_fram->Button(-text => 'bell', -command =>[\&mybeep, 3, 3])
#			                    ->place(
#				            -relwidth=>0.5,-relheight=>$drh2,      
#                                           -relx=>0.5,-rely=>($shf2+6*$drh2));
#--------------
#job_progress frame:
#$prg_fram=$mwnd->Frame(-label=>"TAUcalib-Job progress :",-background => "green", 
#                                                        -relief=>'groove', -borderwidth=>3)
#						        ->place(
#                                                        -relwidth=>(1-$LogfXsize), -relheight=>0.055,
#                                                        -relx=>0, -rely=>0.87);
#$perc_done=0.;
#$prg_but=$prg_fram->ProgressBar( -width=>10, -from=>0, -to=>100, -blocks=>100,
#                                             -colors=>[0,'green'], -gap=> 0,
#                                             -variable=>\$perc_done)
#					     ->place(
#					     -relwidth=>0.999, -relheight=>0.3,
#                                             -relx=>0, -rely=>0.65);
#--------------
#job_control frame:
$jcl_fram=$mwnd->Frame(-label=>"TdcLinCalib-Job control :",-background => "red",
                                                       -relief=>'groove', -borderwidth=>3)
						       ->place(
                                                       -relwidth=>(1-$LogfXsize), -relheight=>$jclfrheight,
                                                       -relx=>0, -rely=>($dirfrheight+$setfrheight));
#---
$jstatbt=$jcl_fram->Button(-text => "CheckJobsStat", -font=>$font2, 
                                                  -activebackground=>"yellow",
			                          -activeforeground=>"red",
			                          -background=>"green",
			                          -foreground=>"red",
                                                  -borderwidth=>3,-relief=>'raised',
			                          -cursor=>hand2,
                                                  -command => \&CheckJobsStat)
						  ->place(
                                                  -relx=>0,-rely=>0.35,
                                                  -relwidth=>0.35,-relheight=>0.65);
$jstatbt->bind("<Button-3>", \&jobstat_help);
#---
$joboc_bt=$jcl_fram->Button(-text => "JobOutpControl", -font=>$font2,
                                                 -activebackground=>"yellow",
			                         -activeforeground=>"red",
			                         -background=>"green",
			                         -foreground=>"red",
                                                 -borderwidth=>3,-relief=>'raised',
			                         -cursor=>hand2,
                                                 -command => \&JobOutpControl)
						 ->place(
                                                 -relx=>0.35,-rely=>0.35,
                                                 -relwidth=>0.35,-relheight=>0.65);
$joboc_bt->bind("<ButtonRelease-3>", \&joutcontr_help);
#---
$quitbt=$jcl_fram->Button(-text => "QuitSession", -font=>$font2,
                                                 -activebackground=>"yellow",
			                         -activeforeground=>"red",
			                         -background=>"green",
			                         -foreground=>"red",
                                                 -borderwidth=>3,-relief=>'raised',
			                         -cursor=>hand2,
                                                 -command => \&QuitSession)
						 ->place(
                                                 -relx=>0.7,-rely=>0.35,
                                                 -relwidth=>0.3,-relheight=>0.65);
$quitbt->bind("<ButtonRelease-3>", \&quitbt_help);
#------------
}  
#------------------------HelpArea------------------------------
sub open_help_window
{
  my $helpfile="TofHelpTDCL.txt";
  my $hwdszx=0.4;#X-size of genhelp window (portion of display x-size)
  my $hwdszy=0.45;#Y-size of genhelp window (portion of display y-size)
  my $szx=int($hwdszx*$displx);#in pixels
  my $szy=int($hwdszy*$disply);
  my $hwdpx=int(0.01*$disply);# 2nd wind. pos-x in pixels(0.01->safety)
  my $hwdposx="-".$hwdpx;
#  my $hwdpy=int(0.11*$disply);# 2nd wind. pos-y in pixels(0.11->safety)
  my $hwdpy=int(0.48*$disply);# 2nd wind. pos-y in pixels(0.11->safety)
  my $hwdpozy="-".$hwdpy;
  my $hwdgeom=$szx."x".$szy.$hwdposx.$hwdpozy;
  print "HELP-window geom: ",$hwdgeom,"\n";
#
  if($soundtext eq "Sound-ON"){$mwnd->bell;}
  $helpwindow=$mwnd->Toplevel();
  $helpwindow->geometry($hwdgeom);
  $helpwindow->title("Help Window");
#  
  $helptext=$helpwindow->Scrolled("Text", -font=>$font6
#                          ,-width=>40
                           , -height=>200
			                 )->pack(-fill=>'both', -expand=>1);
  open(HFN,"< $helpfile") or die show_warn("   <-- Cannot open $helpfile for reading !");
  while(<HFN>){
    $helptext->insert('end',$_);
    $helptext->yview('end');
  }
  close(HFN);
  $helptext->see("1.0");
  $helptext->tagConfigure('lookat',-foreground=> red);
}
#-------------
sub lookaround_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "LookAround-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx",);
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub resethist_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "ResetHistory-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx",);
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub manauto_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "Manual/AutoSession-Switch:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx",);
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub setdefpar_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "SetDefPars-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx",);
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub convert_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "RunNumber<=>Date-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx",);
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub scanddir_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "ScanDaqDir-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx",);
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub editrlist_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "EditRlist-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub setupjob_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "SetupJob-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub submitjob_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "SubmitJob-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub jobstat_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "CheckJobsStat-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub joutcontr_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "JobOutpControl-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub quitbt_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "QuitSess-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#
#---------------------BrowserArea---------------------------------
sub open_browser_window{ # open mozilla browser
#
  my $stat=0;
  $stat = system("/usr/bin/mozilla");#<-- call mozilla 
  if($stat != 0) {die  show_warn("   <-- Mozilla-browser exited badly $!");}
}
#----------------------Welcome/LookAroundArea------------------------------
sub TDCL_Welcome{
  $ResetHistFlg=0;
  $PrevSessUTC=0;
  $PrevSessTime=0;
  my ($sec,$min,$hour,$day,$month,$year,$ptime,$syear);
  my ($line,$status,$fn,$nel);
  my ($format,$title);
  my @array=();
  $ptime=localtime;#current local time(use gmtime for grinv)
  $year=$ptime->year+1900;
  $syear=($year%100);
  $month=$ptime->mon+1;
  $day=$ptime->mday;
  $hour=$ptime->hour;
  $min=$ptime->min;
  $sec=$ptime->sec;
  $SessUTC=timelocal($sec,$min,$hour,$day,$month-1,$year-1900);#or timegm for grinv
  $SessTime="yyyy/mm/dd=".$year."/".$month."/".$day." hh:mm:ss=".$hour.":".$min.":".$sec;
  $delim1="-";
  $delim2="_";
  $delim3=":";
#  $SessTLabel=pack("a4 A1 a2 A1 a2 A1 a2 A1 a2",$year,$delim1,$month,$delim1,$day,$delim2,$hour,$delim3,$min);
  $SessTLabel=$year."-".$month."-".$day."_".$hour.":".$min;
  print "SessTLabel=",$SessTLabel,"\n";
#
  $line="       ---------- WELCOME to TDCs Linearity calibration !!! ----------\n\n";
  $logtext->insert('end',$line,'BigAttention');
  $logtext->yview('end');
  show_messg("   <--- Session starts at : ".$SessTime."\n");
#---
  $prevstfn="TofPrevSessTime".".".$SessName;
  $fn=$workdir."/".$prevstfn;
  open(PREVST,"< $fn") or show_warn("   <-- Cannot open $fn for reading $!");
  while(defined ($line = <PREVST>)){
    chomp $line;
    if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
    $line =~s/^\s+//s;# remove " "'s at the beg.of line
    $PrevSessUTC=$line;
  }
  close(PREVST) or show_warn("   <-- Cannot close $fn after reading $!");
  $PrevSessTime=run2time($PrevSessUTC);
  show_messg("   <--- Previous Session Time:".$PrevSessTime."\n");
  $mwnd->update;
#--- save curr.sess.utc in file:
  open(CURRST,"> $fn") or show_warn("   <-- Cannot save current session UTC in file $fn $!");
  print CURRST $SessUTC,"\n";
  close(CURRST) or show_warn("   <-- Cannot close $fn after writing $!");
#  $status=system("chmod 666 $fn");
#  if($status != 0){
#    show_warn("   <-- Cannot set write-priviledge for file $fn !");
#    print "Warning: problem with write-priv for file $fn, status=",$status,"\n";
#    exit;
#  }
#
  show_messg("   <--- DaqFiles Directory $DaqDataDir","Big");   
#
#--- Read list of all submitted jobs from file "TofSubmJobsList.TDCL"(upto current session):
#
  $fn="TofSubmJobsList".".".$SessName;
  open(SJLIST,"< $fn") or show_warn("\n   <--- Cannot open $fn for reading, $!");
  while(defined ($line = <SJLIST>)){
    chomp $line;
    if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
    $line =~s/^\s+//s;# remove " "'s at the beg.of line
    push(@SubmJobsList,$line);
  }
  close(SJLIST) or show_warn("\n   <--- Cannot close $fn after reading !");
  $nel=scalar(@SubmJobsList);
  my $max;
  if($nel>0){
    if($nel>10){$max=10;}
    else{$max=$nel;}
    show_messg("\n   <--- Last unfinished jobs submitted in prev.sessions:\n");  
    $title="JobId    Node    Que  CflRefNumb   FirstRunN    LastsRunN  CCode   SubmTime       JobStat \n";
    $logtext->insert('end',$title);
    $logtext->yview('end');
    for($j=0;$j<$nel;$j++){
      $line=$SubmJobsList[$nel-$j-1];
      @array=unpack($SubmJobsFmt,$line);
      $status=$array[9];
      if($status==1 || $status==5 || $status==10){# unfinished job(submitted in prev.session)
        $UnFinishedJobs+=1;
        $line=$line."\n";
        $logtext->insert('end',$line);
        $logtext->yview('end');
      }
    }
  }
  else{show_warn("\n   <--- Submitted jobs history file is empty !!!\n");}
  if($UnFinishedJobs==0){show_messg("\n   <--- There are no Unfinished jobs submitted in prev.sessions:\n");}
#
#--- Prepare the lists of RefCflist-files and their members (created upto now):
#
  $fn="TofRefCflistList".".TAUC";
  open(CFLIST,"< $fn") or show_warn("   <-- Cannot open $fn for reading $!");
  while(defined ($line = <CFLIST>)){
    chomp $line;
    if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
    $line =~s/^\s+//s;# remove " "'s at the beg.of line
    push(@RefCflistList,$line);
  }
  close(CFLIST) or show_warn("   <-- Cannot close $fn after writing $!");
  $nel=scalar(@RefCflistList);
  show_messg("\n   <--- Found $nel RefCflist-files available from previous sessions !!!");
  $LastRefCflistN=$RefCflistList[$nel-1];
#
  my $reffn;
  my @cflistmemb=();
  my @p2memlists=();
  my $nmemb=0;
  $p2memlists[0]=\@RefCStatList;
  $p2memlists[1]=\@RefTdelvList;
  $p2memlists[2]=\@RefTzslwList;
  $p2memlists[3]=\@RefAmplfList;
  $p2memlists[4]=\@RefElospList;
  $p2memlists[5]=\@RefTdcorList;
#--- read RefCflist-files:
  for($i=0;$i<$nel;$i++){#<-- loop over @RefCflistList
    @cflistmemb=();
    $reffn=$RefCflistList[$i];
    $fn=$workdir.$amsjwd."/".$cfilenames[0].".".$reffn;
    open(CFMEMB,"< $fn") or show_warn("   <-- Cannot open $fn for reading $!");
    while(defined ($line = <CFMEMB>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@cflistmemb,$line);
    }
    close(CFMEMB) or show_warn("   <-- Cannot close $fn after reading $!");
    $nmemb=$cflistmemb[0];# 6
    for($j=0;$j<$nmemb;$j++){#<-- loop over members of i'th RefCflist-file)
      push(@{$p2memlists[$j]},$cflistmemb[$j+1]);# fill the memb-lists
    }
  }
#
#---> create DaqfHist-array from DaqfHist-file:
#
  @DaqfHist=();
  $DaqfHistLen=();
  my @range=();
  my $t1,$t2;
  $fn=$JobDaqfHistFN.".".$SessName;
#
  open(INPFN,"< $fn") or show_warn("   <-- Cannot open $fn for reading $!");
  while(defined ($line = <INPFN>)){
    chomp $line;
    if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
    $line =~s/^\s+//s;# remove " "'s at the beg.of line    
    push(@DaqfHist,$line);
  }
  close(INPFN) or show_warn("   <-- Cannot close $fn after reading $!");
  $DaqfHistLen=scalar(@DaqfHist);
#--
  show_messg("\n   <--- Found $DaqfHistLen entries in DAQ-files processing history  : \n");
  for($i=0;$i<$DaqfHistLen;$i++){#<--- loop over list to show content
    @range=unpack("A10 x1 A10",$DaqfHist[$i]);
    $ptime=localtime($range[0]);#local time of beg-run(imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time="y/mm/dd=".$year."/".$month."/".$day." hh:mm:ss=".$hour.":".$min.":".$sec;
    $t1=$time;
#
    $ptime=localtime($range[1]);#local time of beg-run(imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time="y/mm/dd=".$year."/".$month."/".$day." hh:mm:ss=".$hour.":".$min.":".$sec;
    $t2=$time;
#
    $line="Run/Time-Beg: ".$range[0]."  ".$t1."  "."Run/Time-End: ".$range[1]."  ".$t2."\n";
    $logtext->insert('end',$line);
    $logtext->yview('end');
  }
#---
  if($DaqfHistLen > 0){
    show_messg("\n   <--- The last processed Run+1 will be used as default StartRun(+2weeks -> EndRun) \n");
    $line=$DaqfHist[$DaqfHistLen-1];
    @range=unpack("A10 x1 A10",$line);    
    $fnum1=$range[1]+1;
    $fnum2=$fnum1+1209600;# +2 weeks
    $fdat1=run2time($fnum1);
    $fdat2=run2time($fnum2);
  }
  else{
    $line="\n   <--- Use defStartDate/defEndDate as defStart/defEnd \n";
    show_messg("\n   <--- Use defStartDate/defEndDate as defStart/defEnd \n");
    $fnum1=time2run($fdat1);
    $fnum2=time2run($fdat2);
  }
#
  $fnum1o=$fnum1;
  $fnum2o=$fnum2;
  $fdat1o=$fdat1;
  $fdat2o=$fdat2;
#
  $mwnd->update;
}
#---------
sub LookAround
{
  $ResetHistFlg=0;
  my @array=();
  my @Cfrefs=();
  my @Cflstfn=();
  my @CStatfn=();
  my @Tdelvfn=();
  my @Tzslwfn=();
  my @Amplffn=();
  my @Elospfn=();
  my @Tdcorfn=();
#
  $Cfrefs[0]=\@Cflstfn;
  $Cfrefs[1]=\@CStatfn;
  $Cfrefs[2]=\@Tdelvfn;
  $Cfrefs[3]=\@Tzslwfn;
  $Cfrefs[4]=\@Amplffn;
  $Cfrefs[5]=\@Elospfn;
  $Cfrefs[6]=\@Tdcorfn;
#
  my @p2memlists=();
  my $nmemb=0;
  $p2memlists[0]=\@RefCStatList;
  $p2memlists[1]=\@RefTdelvList;
  $p2memlists[2]=\@RefTzslwList;
  $p2memlists[3]=\@RefAmplfList;
  $p2memlists[4]=\@RefElospList;
  $p2memlists[5]=\@RefTdcorList;
#
  my @ncfiles=();
#
  my $stat=0;
  my $pid=0;
  my $dir=$workdir.$amsjwd;
  my $cmd;
  my (@array,@string,@substrg);
  my ($title,$line,$dattim,$date,$time,$runn,$reffn,$nel,$tag);
  my $nelcft=0;
#
  show_messg("\n   <========= Enter 'LookAround' ...","B");
#
#---> see calib-files in amsjobwd:
#
  $nelcft=scalar(@cfilenames); 
  for($i=0;$i<$nelcft;$i++){#<--- all calf-types loop
    @array=();
    $cmd="ls -ogt --time-style=long-iso ".$dir."/".$cfilenames[$i].".*[0-9][^_] |";
    $pid=open(FLIST,$cmd) or show_warn("   <-- Cannot make cal-file list in amsjobwd  $!");
    while(<FLIST>){
      push(@array,$_);
    }
    close(FLIST);
    $ncfiles[$i]=scalar(@array);
    for($j=0;$j<$ncfiles[$i];$j++){#<--- calf-loop of given type
      @string=split(/\s+/,$array[$j]);
      $date=$string[3];
      $time=$string[4];
      $dattim=$date." ".$time;# yyyy-mm-dd hh:mm
      @substrg=split(/\./,$string[5]);
      $nel=scalar(@substrg);
      $runn=$substrg[$nel-1];#number after last "."
      if($runn==111){next;}#<-- skip generic files(used as input for db-update jobs)
      $line=$runn." ".$dattim;
      push(@{$Cfrefs[$i]},$line);
    }
  }
#---
  show_messg("\n   <--- Found the following calib-files in AmsJobWD (yellow -> RefFile)  : \n");
  for($i=0;$i<$nelcft;$i++){#<-- calf-types loop(7)
    $title="     ".$cfilenames[$i].":"."\n";
    $logtext->insert('end',$title,'ItalText');
    $logtext->yview('end');
    $nel=scalar(@RefCflistList);
    for($j=0;$j<$ncfiles[$i];$j++){#<-- i-th type memory cal-files loop
      $line=$Cfrefs[$i]->[$j];#<-- runn/date/time
      @string=split(/\s+/,$line);
      for($k=0;$k<$nel;$k++){#<-- Ref-files loop(prepared from refcflist-file)
        if($i==0){$reffn=$RefCflistList[$k];}
	else{$reffn=$p2memlists[$i-1]->[$k];}
        $tag='Text';
	if($reffn==$string[0]){
	  $tag='Yellow';
          $logtext->insert('end',$line."\n",$tag);
          $logtext->yview('end');
	  last;# skip others in p2mem-lists(some members can be repetetive)
	} 
      }
    }
    $line="\n";
    $logtext->insert('end',$line);
    $logtext->yview('end');
  }
#
#---> check the presence of log-files:
#
  $cmd="ls -ogt --time-style=long-iso ".$dir."/TofTDCL*.*[0-9]".".log.*[0-9] |";
  @array=();
  $pid=open(FJOBS,$cmd) or show_warn("\n   <--- Cannot make log-file list in amsjobwd  $!");
  while(<FJOBS>){
    push(@array,$_);
  }
  close(FJOBS);
  my $nelfjl=scalar(@array);
  if($nelfjl==0){show_messg("\n   <--- There are no any log-files in $amsjwd-directory !");}
  else{show_messg("\n   <--- Found $nelfjl log-files in $amsjwd-directory !");}
  my ($date,$time,$dattim,$substr,$runn,$lfpid);
  for($j=0;$j<$nelfjl;$j++){
    $line=$array[$j]."\n";
    $logtext->insert('end',$line);
    $logtext->yview('end');
  }
#
#---> check the presence of submitted jobs:
#
  my ($sjpid,$user,$stat,$queue,$frhost,$exhost,$jname,$sbtime);
  @array=();
  $cmd="bjobs -u all |";
  $sjpid=open(SJOBS,$cmd) or show_warn("   <--- Cannot run 'bjobs' to see running jobs  $!");
  while(<SJOBS>){
    push(@array,$_);
  }
  close(SJOBS);
#
  $nel=scalar(@array);
  if($nel>0){show_messg("\n   <--- Found the following jobs in the queues : \n");}
  else{show_messg("\n   <--- There are no any jobs in the queues !!! \n");}
#
  my $nell;
  for($j=0;$j<$nel;$j++){
    if($j==0){
      $line=" Jobid    User     Stat   Queue   From_host Exec_host  JobName     SubmDate  \n";
      $logtext->insert('end',$line);
      $logtext->yview('end');
    }
    if($j>0){# to skip title line
      @string=split(/\s+/,$array[$j]);
      $nell=scalar(@string);
      $rjpid=$string[0];
      $user=$string[1];
      $stat=$string[2];
      $queue=$string[3];
      $frhost=$string[4];
      if($nell>=10){
        $exhost=$string[5];
        $jname=$string[6];
        $sbtime=$string[7]."  ".$string[8]." ".$string[9];
      }
      else{
        $exhost="-------";
        $jname=$string[5];
        $sbtime=$string[6]."  ".$string[7]." ".$string[8];
      }
      $line=pack($BjobsFmt,$rjpid,$user,$stat,$queue,$frhost,$exhost,$jname,$sbtime);
      $logtext->insert('end',$line."\n");
      $logtext->yview('end');
    }
  }
#
}
#-------------------------------------------------------------------
sub SetDefaultPars
{
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time,$line,$fname);
  my ($fn,$w,$ww,$www);
  my $i,$j;
#
  $ResetHistFlg=0;
  if($CalibMode eq "Econom"){
    show_messg("\n   <--- Economical mode: all SFETs are identical - require lower statistics !");
  }
  else{
    show_messg("\n   <--- Normal mode: all SFETs are uniqe - require higher statistics !!!");
  }
#
  show_messg("\n\n   <--- Setting Job's default-parameters ... \n");
#
  $cfilesloc="10101";#def value
  $ncpus="4";#def number of CPUs to use 
  $CalCode=900;# i just add 9 in front of true calcode for better logf identification later
  $jobctypes=0;#calib-types in job
  $jpar1=0;#(TFCA #39=MN)
  $Evs2Read=0;  
  $RunType="SCI";# required Daq Files type
  $SubDetMsk=9;#bit-patt: (msb T8/U2/S4/R4/E2/L1 lsb)
  $FileSize=50;# mb
  $MinEvsInFile=100000;
  $BinMinEvs=10;
  $OutFileExt="";
#
#---
  if($CalibMode eq "Econom"){
    $Evs2Read=1000000;
    $jpar1+=10;
    $CalCode+=10;
    $Queue2run="1nd";
    $PassType=1;
    $OutFileExt="_ecom";
  }
  else{
    $Evs2Read=10000000;
    $Queue2run="1nw";
    $PassType=1;
    $OutFileExt="_norm";
  }
  $CalCode+=($PassType-1);
  $jpar1+=($PassType-1);#convention for TFCA 39 datacard
#
  if($magstext eq "MagnetON"){
    $jpar6=1;
  }
  else{
    $jpar6=0;
  }
#
  if($posstext eq "InSpace"){
    $jpar4=0;
  }
  else{
    $jpar4=1;
  }
#-------------
  $refcfl_om->configure(-options => [@RefCflistList]);
  $refcalsetn=$RefCflistList[0];# use 1st in the list as default
#
  $scanbt_state="normal";
  $scanbt->configure(-state=>$scanbt_state);
#
$j=1;
for($i=0;$i<5;$i++){
  $TofDBUsePatt[$i]=1-(($cfilesloc/$j)%10);# 10-base BitPattern Msb(tdcl elos dcrd ped cal)Lsb (=1 to use BD)
  $j=10*$j;
}
#
  $mwnd->update;
}# ---> endof sub:setRunDefs
#------------------------------------------------------
sub SetupJob
{
  $ResetHistFlg=0;
  $SubmJobCount=0;
#
  show_messg("\n   <========= Enter 'SetupJob ...","B");
#
  my $stat=0;
  my $nlinks,$firstr,$totevs;
  my $cmd,$fpath,$lname,$dir,$fn;
  my $dflinksTD=$workdir.$daqflnk;
  my $dflinksSD;
  $DaqfIndex1=0;
  $DaqfIndex2=0;
  $DaqfSets=0;
#
  my ($sec,$min,$hour,$day,$month,$year,$ptime,$curutc);
  $ptime=localtime;#current local time(use gmtime for grinv)
  $year=$ptime->year+1900;
  $month=$ptime->mon+1;
  $day=$ptime->mday;
  $hour=$ptime->hour;
  $min=$ptime->min;
  $sec=$ptime->sec;
  $curutc=timelocal($sec,$min,$hour,$day,$month-1,$year-1900);# or timegm for grinv
#
#---> set values for job's stable params:
#
  $jpar2=0;#DBusage
  $j=1;
  for($i=0;$i<5;$i++){
    if($TofDBUsePatt[$i]==0){$jpar2+=$j;}#means use RawFile instead DB
    $j=10*$j;
  }
  show_messg("\n   <--- DB-usage parameter: $jpar2 (LQDPC) !\n");
#
  $jpar3=$refcalsetn;# ref TofClfile number
  if($posstext eq "InSpace"){$jpar4=0;}# space/earth calib
  else{$jpar4=1;}
  $jpar5=$BinMinEvs;# min.evs/bin to concider bin as bad diring quality evaluation
  if($magstext eq "MagnetON"){$jpar6=1;}
  else{$jpar6=-1;}
  $jpar7=$ncpus;
  $jpar8=$Evs2Read;
#------
NextJob:
#------>
#  
#---> clean templinks-dir from previous job temporary links:
#
  $fn=$workdir."/templinks/*";
  $stat = system("rm -f $fn");
  if($stat != 0){show_messg("\n   <--- There are no temporary links to clear in templinks-dir !\n");}
#
#---> loop over DB-selected daq-files and create links to them :
#
  $totevs=0;
  $firstr=0;
  $nlinks=0;
#  
  for($i=$DaqfIndex1;$i<$ndaqfound;$i++){#<--- selected daq-files loop
    if($daqfstat[$i] == 11){#was selected by DB-search as good
      if($firstr==0){#<-- 1st run in sub-range
        $firstr=1;
	$CalRun1=$daqfrunn[$i];
      }
      $DaqfIndex2=$i;
      $CalRun2=$daqfrunn[$i];
#--> create symbolic link for given daq-file:
      $fpath=$DaqDataDir."/SCI/".$daqfrunn[$i];#daq-file path
      $lname=$daqfrunn[$i];
      $cmd="ln -s $fpath $lname";
      $stat = system("$cmd");
      if($stat != 0){
        show_warn("\n   <-- link $fpath was not created, skip to next !");
	next;
      }
#--> and move it from current dir(workdir) to templinks-dir:
      $dir=$workdir."/templinks";
      $stat=system("mv $lname $dir");
      if($stat!=0){
        show_warn("\n   <-- Moving of new link $lname to templinks-dir failed, skip to next run !!!");
        next;
      }
      $nlinks+=1;
      $totevs+=$daqfevts[$i];
      if($totevs>$Evs2Read){
        $DaqfSets+=1;
        last;
      }
    }#--->endof "selected-file" check
  }#--->endof "selected daq-files loop"
#----
  if($totevs<$Evs2Read){#<-- low statistics
    if($AutoSessFlg eq "AutoSess"){
      if($DaqfSets>0){#<-- last Daqf sub-range with low-stat
        show_messg("\n   <--- AutoSession: Full DaqFiles-range is completed after $SubmJobCount submissions !");
#---> put here job status/output-manager program  :
        WatchDog();
        show_messg("\n   <--- AutoSession: WatchDog on duty !!!","Big");
	$setupj_state="disabled";
	$setupj_bt->configure(-state=>$setupj_state);   
        $sjobbt_state="disabled";
        $sjobbt->configure(-state=>$sjobbt_state);
      }
      else{
        show_warn("\n   <--- AutoSession: only $totevs events found in 1st DaqFiles sub-range, change DaqFiles-range");
        show_warn("          and repeat 'ScanDaqDir'=>'SetupJob', or change 'Events' and repeat 'SetupJob'=> !!!  ");
      }
    }
    else{
      show_warn("\n   <--- ManualSession: only $totevs events found, change DaqFiles-range and repeat");
      show_warn("          'ScanDaqDir'=>'SetupJob', or change 'Events' and repeat 'SetupJob'=> !!!  ");
    }
#---> clean templinks-dir from previous job temporary links and return:
    $fn=$workdir."/templinks/*";
    $stat = system("rm -f $fn");
    if($stat != 0){show_messg("   <--- There are no temporary links to clear in templinks-dir !\n");}
#
    return;
  }
#----
  else{#<-- statistics ok: 
#--> redefine some run-dependent params:
    if($CalRun2<1230764399){#--->2008
      $jpar10="AMS02PreAss";
    }
    elsif($CalRun1>1230764399 && $CalRun2<1262300400){#--->2009
      $jpar10="AMS02Ass1";
    }
    elsif($CalRun1>1262300400 && $CalRun2>1262300400){#--->2010
      if($posstext eq "OnEarth"){#--->still on earth
        $jpar10="AMS02";# ="AMS02Ass1" as default
      }
      else{
        $jpar10="AMS02Space";
      }
    } 
    else{
      show_warn("\n   <-- Wrong dates(runs) range definition, please correct it !!!");
      return;
    }
#--> create dir to keep job's daqf-links:
    $dflinksSD="/JL".$CalRun1;
    $cmd="mkdir -p -m a=rwx ".$dflinksTD.$dflinksSD;# ($dflinksTD=$workdir."/daqflinks")
    $stat=system($cmd);
    if($stat != 0){
      show_warn("\n   <-- Can't create links-dir $dflinksSD, skip to next job !!!");
      goto NextJob;#<-- means next sub-range job
    }
#--> move job's links from templinks-dir to job's links-dir:
    $lname=$workdir."/templinks/*";#all link-names (=run#) created temporary in templinks-dir
    $lpath=$dflinksTD.$dflinksSD;
    $stat=system("mv $lname $lpath");
    if($stat!=0){
      show_warn("\n   <-- Moving of links to $dflinksSD failed, skip to next job !!!");# move links to job's link-dir
      goto NextJob;#<-- means next sub-range job
    }
#---
    $jpar9=$lpath."/";
    $sjobbt_state="normal";
    $sjobbt->configure(-state=>$sjobbt_state);
    if($AutoSessFlg eq "ManualSess"){#<-- SubmJob will be done manually
      show_messg("\n   <--- $nlinks DaqFiles was selected for ManualSubmit in range $CalRun1 - $CalRun2 ...");
      return;
    }
    else{#<-- Auto JobSubmit
      show_messg("\n   <--- Starting AutoSubmit with $nlinks DaqFiles in range $CalRun1 - $CalRun2 ...","Y");
      $sjobbt->invoke();#<--call SubmitJob
      if($AutoSessFlg eq "ManualSess"){return;}#<-- check of "stop auto-submissions"
      $DaqfIndex1=$DaqfIndex2+1;
      goto NextJob;#<-- means next sub-range job
    }
  }
#--
} 
#------------------------------------------------------
sub RunDateConv {
  if($fnum1o ne $fnum1){
    $fnum1o=$fnum1;
    $fdat1=run2time($fnum1);
    $fdat1o=$fdat1;
  }
#  
  if($fnum2o ne $fnum2){
    $fnum2o=$fnum2;
    $fdat2=run2time($fnum2);
    $fdat2o=$fdat2;
  }
#  
  if($fdat1o ne $fdat1){
    $fdat1o=$fdat1;
    $fnum1=time2run($fdat1);
    $fnum1o=$fnum1;
  }
#  
  if($fdat2o ne $fdat2){
    $fdat2o=$fdat2;
    $fnum2=time2run($fdat2);
    $fnum2o=$fnum2;
  }
}
#------------------------------------------------------
sub scanddir{ # scan DAQ-directories to search for needed files in required date(runN)-window
#
  $nprocruns=0;
  @daqfrunn=();
  @daqftags=();
  @daqftyps=();
  @daqfconfs=();
  @daqfpath=();
  @daqfevts=();
  @daqfleng=();
  @daqfstat=();
  $ndaqgood=0;
  $ndaqfound=0;
  @rlistbox=();
  my $lbitem=0;
  my $fdat=0;
  my ($rtype,$n1,$n2,$n3,$conf)=(0,0,0,0,0);
  my $subdetpat=0;
  my @detin=();#L1(0)/EC(1)/RIC(2)/TOF(3)/TRD(4)/TRK(5)
#
  my $debug="-d";
  if($ServConnect==0){
    unshift @ARGV, "-DOracle:";
    unshift @ARGV, "-Famsdb";
    $ServObject=new DBSQLServer();
    $ServConnect=$ServObject->ConnectRO();
  }
  if($ServConnect){
    my $runmin=$fnum1;
    my $runmax=$fnum2;
    my $sql = "SELECT run,type,path,fetime, nevents, tag, sizemb FROM amsdes.datafiles WHERE run>=$runmin AND run<=$runmax ORDER BY run";
    my $ret=$ServObject->Query($sql);
    if(defined $ret->[0][0]) {
      foreach my $r (@{$ret}){
        @detin=();
        my $run       = $r->[0];
	my $type      = $r->[1];
        my $path      = $r->[2];
        my $starttime = $r->[3];
        my $nevents   = $r->[4];
        my $tag   = $r->[5];
        my $sizemb   = $r->[6];
        ($rtype,$n1,$n2,$n3,$conf)=unpack("A3 A2 A2 A3 A*",$type);#unpack type
	if(substr($type,0)=~/L1/){$detin[0]=1;}
	if(substr($type,0)=~/E2/){$detin[1]=1;}
	if(substr($type,0)=~/R4/){$detin[2]=1;}
	if(substr($type,0)=~/S4/){$detin[3]=1;}
	if(substr($type,0)=~/U2/){$detin[4]=1;}
	if(substr($type,0)=~/T8/){$detin[5]=1;}
	$subdetpat=32*$detin[5]+16*$detin[4]+8*$detin[3]+4*$detin[2]+2*$detin[1]+$detin[0];#bit patt
#	$srtyp = substr($type,0,3);#LAS,SCI,CAL,UNK
	$path =~ s/\w+(?!\/)$//s;
#	$path =~ s/$\d+(?=\/)//s;
        $fdat=run2time($starttime);
	$fdat=substr($fdat,0,16);
#	print "type=",$type," Dtyp=",$rtype," Conf=",$conf," SubDetPat=",$subdetpat," Rtype=",$rtype,"\n";
	if($rtype eq $RunType){#<-- use only requested runtype(LAS,SCI,CAL or UNK)
          $curline="  Run/Tag/RType:".$run." ".$tag." ".$type."  StTime=".$fdat." Evs:".$nevents."  Path:".$path."\n";
          $logtext->insert('end',$curline);
          $logtext->yview('end');
          $daqfstat[$ndaqfound]=1;#status=found
	  if(($sizemb>$FileSize)
	              && ($nevents>=$MinEvsInFile)
		      && (($subdetpat & $SubDetMsk) == $SubDetMsk)
		                                                  ){
            $daqfstat[$ndaqfound]+=10;#status=selected(good)
            $ndaqgood+=1;
	  }
          $ndaqfound+=1;
          push(@daqfrunn,$run);
          push(@daqftags,$tag);
          push(@daqftyps,$rtype);
          push(@daqfconfs,$conf);
          push(@daqfpath,$path);
          push(@daqfevts,$nevents);
          push(@daqfleng,$sizemb);
          $fdat=run2time($run);
	  $lbitem="Run/Tag:".$run."  ".$tag."  Date:".$fdat."  Type:".$rtype." ".$conf."  Nev=".$nevents."  Stat=".$daqfstat[$ndaqfound-1]."\n";
	  push(@rlistbox,$lbitem);
	}
      }
    }
    else{
      show_warn("\n   <-- Query failed !!!");
    }
  }
#
  if($ndaqfound==0){
    show_warn("\n   <-- Found 0 DAQ-files !!! Change search window and repeate scan !!!");
  }
  elsif($ndaqgood==0){
    show_warn("\n   <-- Selected 0 DAQ-files from $ndaqfound found !!! Change selection and repeate scan !!!");
  }
  else{
    show_messg("\n   <-- Selected $ndaqgood DAQ-files from $ndaqfound found !!!");
  }
#
  if($ndaqgood>0){
    $setupj_state="normal";
    $setupj_bt->configure(-state=>$setupj_state);
  }
#
  $mwnd->update;
}
#------------------------------------------------------------------------------
sub edit_rlist{ #edit run-list prepared by sub-scandir
#
# <-- define size and create Editor-window:
  $topl2=$mwnd->Toplevel();
  $swdszx=$mnwdszx;#X-size of 2nd window (portion of display x-size)
  $swdszy=1-$mnwdszy-0.11;#Y-size of 2nd window (portion of display y-size)(0.1->bot.syst.panel)
  my $szx=int($swdszx*$displx);#in pixels
  my $szy=int($swdszy*$disply);
  my $swdposx="+0";
  my $swdpy=int(($mnwdszy+0.03)*$disply);# 2nd wind. pos-y in pixels(0.03->safety)
  my $swdpozy="+".$swdpy;
  my $swdgeom=$szx."x".$szy.$swdposx.$swdpozy;
  print "EDITOR-window geom-",$swdgeom,"\n";
  $topl2->geometry($swdgeom);
  $topl2->title("                  Edit DAQ-files list (select to redefind as good/bad)");
#------ frame for edit-control:
  my $cnvrsz=0.8;
  my $bheight=0.15;
  my $bspac=0.01;
  my $crely;
  $ec_fram=$topl2->Frame(-label => "Control panel",-background => "Cyan",  
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>(1-$cnvrsz), -relheight=>1,
                                                    -relx=>0, -rely=>0);
#--- 
  $crely=0.08;
  my $addit_bt=$ec_fram->Button(-text=>"Select for processing",
                                                        -font=>$font2,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&edit_add)->place(
                                                                 -relwidth=>1., -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
#--
  $crely+=($bheight+$bspac);
  my $remit_bt=$ec_fram->Button(-text=>"Exclude from processing",
                                                        -font=>$font2,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&edit_rem)->place(
                                                                 -relwidth=>1., -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
#--
  $crely+=($bheight+$bspac);
  my $remi_bt=$ec_fram->Button(-text=>"Exit Editor", 
                                                        -font=>$font2,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&edit_exit)->place(
                                                                 -relwidth=>1., -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
#-------- frame for edit-window:
  $ew_fram=$topl2->Frame(-label => "List of found/selected(1/11) DAQ-files:",-background => "Cyan", 
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>$cnvrsz, -relheight=>1,
                                                    -relx=>(1-$cnvrsz), -rely=>0);
#---
  $edit_lb=$ew_fram->Scrolled("Listbox", -scrollbars => "e", -selectmode => "multiple",
                                                            -relief => "sunken",
							    -font => $font2,
                                                            -selectbackground => "yellow",
							    -selectforeground => "red"
							    )->pack(-fill=>'both', -expand=>1);
  @rlistbox=();
  my ($lbitem,$fdat,$run,$tag,$rtype,$conf,$nevs,$stat);
  show_messg("   <-- Editor: Enter with $ndaqgood good files !");
  $ndaqgood=0;
  for($i=0;$i<$ndaqfound;$i++){#prepare list
    $run=$daqfrunn[$i];
    $tag=$daqftags[$i];
    $rtype=$daqftyps[$i];
    $conf=$daqfconfs[$i];
    $nevs=$daqfevts[$i];
    $stat=$daqfstat[$i];
    $fdat=run2time($run);
#    $lbitem=sprintf("Run/Tag: %10d %11u   Date: %s Type: %10d Nevs: %8d Stat: %4d",$run,$tag,$fdat,$rtype,$nevs,$stat);
    $lbitem="Run/Tag:".$run." ".$tag."  Date:".$fdat."   Type:".$rtype."  ".$conf."    Nev=".$nevs."    Stat=".$stat;
    push(@rlistbox,$lbitem);
    if($stat==11){$ndaqgood+=1;}
  }
  $edit_lb->insert("end",@rlistbox);
  show_messg("   <-- Editor: really found $ndaqgood good files !");
#
}
#-------
  sub edit_add
  {
    my @selected=$edit_lb->curselection();
    foreach (@selected) {
      $edit_lb->selectionClear($_);
      if($daqfstat[$_]==1){
        $daqfstat[$_]=11;#set "found+selected"
        $ndaqgood+=1;
      }
    }
  }
#---
  sub edit_rem
  {
    my @selected=$edit_lb->curselection();
    foreach (@selected) {
      $edit_lb->selectionClear($_);
      if($daqfstat[$_]==11){
        $daqfstat[$_]=1;#set "found"(not selected)
        $ndaqgood-=1;
      }
    }
  }
#---
  sub edit_exit
  {
    show_warn("<=========== Left editor with $ndaqgood selected DAQ-files  !");
    $topl2->destroy();
  }
#------------------------------------------------------------------------------
sub ShowCalFiles
{
}
#------------------------------------------------------------------------------
sub EditJobScript
{
#
#--- View/Edit AMS-job script file:
#
  if($JobScriptN ne ""){
    my $stat=0;
    my $fname=$workdir."/".$JobScriptN;
    $stat=system("nedit $fname");
    if($stat != 0){
      show_warn("   <-- View/Edit JobScript-file Error: fail to view file !");
      return;
    }
  }
  else{
    show_warn("   <-- View/Edit JobScript-file Error: JobScript file name not defined !");
    return;
  }
}
#------------------------------------------------------------------------------
sub SubmitJob
{
#---
  $ResetHistFlg=0;
  my $status;
  my @words=();
  my $nwords=0;
  my $output;
  my $pid=0;
  my $bsubpid=0;
  my $attempts=0;
  my $par="111";
  my $pid_alive=0;
  my ($line,$format,$string);
  my $dir=$workdir.$amsjwd;
  my $logf=$dir."/TofTDCL".$CalCode.".".$CalRun1.".log.%J";
  my $logef=$dir."/TofTDCL".$CalCode.".".$CalRun1.".loge.%J";
  my $comm2run;
  my $fn;
  my $ext=substr($CalRun1,-5,5);
  my $jobname=$SessName.$ext;
  if($Host2run eq "Any"){$Host2run="";}
  if($Host2run eq ""){#<-- means any host
    $comm2run="bsub -q $Queue2run -o $logf -e $logef  -J $jobname $JobScriptN";
  }
  else{
    $comm2run="bsub -q $Queue2run -m $Host2run -o $logf -e $logef  -J $jobname $JobScriptN";
#    $comm2run="bsub -q $Queue2run -n $jpar7 -m $Host2run -o $logf -e $logef  -J $jobname $JobScriptN";
  }
#
TryAgain:
  $bsubpid=0;
#  $fn="mess.log";
#  open(MESS,"+>$fn") or show_warn("\n   <--- Cannot open mess-file $fn, $!");
#  $status=system("$comm2run $jpar1 $jpar2 $jpar3 $jpar4 $jpar5 $jpar6 $jpar7 $jpar8 $jpar9 $jpar10 $jpar11 >$fn");
#  $mwnd->after(1000);#
#  if($status==0){
#    while(<MESS>){
#      $output=$_;
#      chomp $output;
#      if($output =~/^\s*$/){next;}# skip empty or all " "'s lines
#      $output =~s/^\s+//s;# remove " "'s at the beg.of line
#      @words=split(/\s+/,$output);#split by any number of continious " "'s
#      $nwords=scalar(@words);
#      print " bsub-mess output:",$output," file=",$CalRun1,"\n";
#    }
#  }
#  else{
#    show_warn("\n   <--- System(bsub) status is bad : $status !!!");
#    close(MESS) or show_warn("        Cannot close-1 mess-file $fn, $!");
#    return;
#  }
#  close(MESS) or show_warn("        Cannot close-2 mess-file $fn, $!");
#  $bsubpid=$words[1];
#  $bsubpid =~ tr/0-9\<\>/0-9/d;#remove <> in output <bsubpid>
#  if($bsubpid>0 && ($words[3] eq "submitted")){
#    $SubmJobCount+=1;
#    show_messg("\n   <========= Job $bsubpid is successfully submitted !!!","B");
#  }
#  else{
#    show_warn("\n   <--- Leave SubmitJob after $attempts unsuccessful attempts !!!");
#    return;
#  }
#---  
  $pid=open(BSUBM,"$comm2run $jpar1 $jpar2 $jpar3 $jpar4 $jpar5 $jpar6 $jpar7 $jpar8 $jpar9 $jpar10 |") 
                                              or show_warn("   <-- Job-submit problem (at open), $! !!!");
  while(<BSUBM>){
    $output=$_;
    chomp $output;
    if($output =~/^\s*$/){next;}# skip empty or all " "'s lines
    $output =~s/^\s+//s;# remove " "'s at the beg.of line
    @words=split(/\s+/,$output);#split by any number of continious " "'s
    $nwords=scalar(@words);
  }
  close(BSUBM) or show_warn("   <-- Job-submit problem (at close), $! !!!");
#
  $mwnd->after(2000);#
  $attempts+=1;
  $pid_alive=(kill 0 => $pid);#check existance of process $pid
  $bsubpid=$words[1];
  $bsubpid =~ tr/0-9\<\>/0-9/d;#remove <> in output <bsubpid>
  if($bsubpid>0 && ($words[3] eq "submitted") && $pid_alive==0){
    $SubmJobCount+=1;
    $UnFinishedJobs+=1;
    show_messg("\n   <--- Job $bsubpid is successfully submitted !!!","B");
  }
  else{
    kill 9 => $pid;
    show_warn("\n   <--- JobSubmit problem, submit-process is killed, trying to repeat submission...");
    if($attempts<4){
      goto TryAgain;
    }
    else{
      show_warn("\n   <--- Leave SubmitJob after $attempts unsuccessful attempts !!!");
      return;
    }
  }
#
#---> see the true host2run when submitted with $Host2run="" :
#
  my @array=();
  my $cmd="bjobs ".$bsubpid." |";
  my $nelrjl=0;
  my @string=();
  my $nell=0;
  my $h2run=$Host2run;
  my $jstat="";
  $attempts=0;
#
  if($Host2run eq ""){
    $h2run="--------";
TryAgainBjobs:
    @array=();
    $cmd="bjobs ".$bsubpid." |";
    open(RJOBS,$cmd) or show_warn("\n   <--- SubmitJob: 'bjobs' open failed to see status of just submitted job $bsubpid, $!");
    while(<RJOBS>){
      $output=$_;
      chomp $output;
      if($output =~/^\s*$/){next;}# skip empty or all " "'s lines
      $output =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@array,$output);
    }
    close(RJOBS) or show_warn("\n   <--- SubmitJob: 'bjobs' close failed to see status of just submitted job $bsubpid, $!");
#
    $attempts+=1;
    $nelrjl=scalar(@array);
    if($nelrjl>1){#<-- means there is title+list
      @string=();
      @string=split(/\s+/,$array[1]);
      $nell=scalar(@string);
      $jstat=$string[2];
      if($string[2] eq "RUN"){#<-- stat='RUN' -> host2run is known
        $h2run=$string[5];
        show_messg("\n   <--- SubmitJob: job $bsubpid is running on host $h2run !");
      }
      elsif($string[2] eq "PEND"){#<-- 
        show_messg("\n   <--- SubmitJob: job $bsubpid is PEND, host2run is still unknown... ");
      }
      elsif($string[2] eq "EXIT"){#<-- 
        $h2run=$string[5];
        show_messg("\n   <--- SubmitJob: job $bsubpid is just EXITed host $h2run ?!");
      }
      elsif($string[2] eq "DONE"){#<-- 
        $h2run=$string[5];
        show_messg("\n   <--- SubmitJob: job $bsubpid is DONE on host $h2run ?!");
      }
      else{
        show_warn("\n   <--- SubmitJob: job $bsubpid status is unknown ($string[2]) ???");
      }
    }
    else{#<-- means no answer or status not found
      if($attempts<5){
        $mwnd->after(1000);#
        goto TryAgainBjobs;
      }
      show_warn("\n   <--- SubmitJob: 'bjobs'-> just submitted job $bsubpid status is not found ?!: ");
      show_warn("   <--- $array[0]");
    }
#
  }
#
#---> add submitted job-params in the list and save to file:
#
  my ($sec,$min,$hour,$day,$month,$year,$ptime,$submdate,$submtime,$submutc,$jobsta,$title);
  $ptime=localtime;#current local time(use gmtime for grinv)
  $year=$ptime->year+1900;
  $month=$ptime->mon+1;
  $day=$ptime->mday;
  $hour=$ptime->hour;
  $min=$ptime->min;
  $sec=$ptime->sec;
  $submutc=timelocal($sec,$min,$hour,$day,$month-1,$year-1900);#or timegm for grinv
  $submdate=$year."/".$month."/".$day;
  $submtime=$hour.":".$min;
  $jobsta=1;
  $title="JobId    Node    Que  CflRefNumb   FirstRunN    LastsRunN  CCode   SubmTime       JobStat \n";
  $string=pack($SubmJobsFmt,$bsubpid,$h2run,$Queue2run,$refcalsetn,$CalRun1,$CalRun2,$CalCode,$submdate,$submtime,$jobsta);
  $line=$string."\n";
  push(@SubmJobsList,$line);
#---> save to SubmJobsList-file:
  my $fn="TofSubmJobsList".".".$SessName;
  my $nel=scalar(@SubmJobsList);
  open(SJLIST,"> $fn") or die show_warn("\n   <--- Cannot open $fn for writing on 'submit job', $!");
  for($i=0;$i<$nel;$i++){
    print SJLIST $SubmJobsList[$i]."\n";
  }
  close(SJLIST) or die show_warn("\n   <--- Cannot close $fn after writing on 'submit job', $!");
  show_messg("\n   <---  file $fn is updated !");
#  $status=system("chmod 666 $fn");
#  if($status != 0){
#    print "Warning: problem with write-priv for SubmJobsList-file, status=",$status,"\n";
#      exit;
#  }
#
#---> update DaqfHist-array add save to file:
#
  $fn=$JobDaqfHistFN.".".$SessName;
#
  my $spc=" ";
  $line=pack("A10 A1 A10",$CalRun1,$spc,$CalRun2);
  push(@DaqfHist,$line);
  $nel=scalar(@DaqfHist);
  open(INPFN,"> $fn") or show_warn("   <-- Cannot open $fn for writing, $!");
  for($i=0;$i<$nel;$i++){print INPFN $DaqfHist[$i]."\n";}
  close(INPFN) or show_warn("   <-- Cannot close $fn after writing, $!");
  show_messg("\n   <---  file $fn is updated !");
#  $status=system("chmod 666 $fn");
#  if($status != 0){
#    show_warn("\n   <---  problem with write-priv for $fn, stat=$status !");
#    print "Warning: problem with write-priv for DaqfHist-file, status=",$status,"\n";
#  }
#  else{
#    show_messg("\n   <---  write-priv for $fn is ok !");
#  }
#
  $mwnd->update;
}
#------------------------------- CheckJobStatus Area ------------------------------------
sub CheckJobsStat
{
#
# <-- define size and create JobStatControl window:
#
  $topl3=$mwnd->Toplevel();
  $swdszx=$mnwdszx;#X-size of 2nd window (portion of display x-size)
  $swdszy=1-$mnwdszy-0.11;#Y-size of 2nd window (portion of display y-size)(0.1->bot.syst.panel)
  my $szx=int($swdszx*$displx);#in pixels
  my $szy=int($swdszy*$disply);
  my $swdposx="+0";
  my $swdpy=int(($mnwdszy+0.03)*$disply);# 2nd wind. pos-y in pixels(0.03->safety)
  my $swdpozy="+".$swdpy;
  my $swdgeom=$szx."x".$szy.$swdposx.$swdpozy;
  print "EDITOR-window geom-",$swdgeom,"\n";
  $topl3->geometry($swdgeom);
  $topl3->title("                  Submitted Jobs View/Control window");
#------ frame for job-control pannel:
  my $cnvrsz=0.8;
  my $bheight=0.15;
  my $bspac=0.01;
  my $crely;
  $jcp_fram=$topl3->Frame(-label => "Job Control panel",-background => "Cyan",  
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>(1-$cnvrsz), -relheight=>1,
                                                    -relx=>0, -rely=>0);
#--- 
  $crely=0.08;
  my $killj_bt=$jcp_fram->Button(-text=>"Kill job",
                                                        -font=>$font2,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&jcontr_kill)->place(
                                                                 -relwidth=>1., -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
#---
  $crely+=($bheight+$bspac);
  my $exit_bt=$jcp_fram->Button(-text=>"Quit JobControl", 
                                                        -font=>$font2,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&jcontr_quit)->place(
                                                                 -relwidth=>1., -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
#-------- frame for show-window:
  $jsw_fram=$topl3->Frame(
                                                    -background => "Cyan", 
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>$cnvrsz, -relheight=>1,
                                                    -relx=>(1-$cnvrsz), -rely=>0);
#---
  my $label="JobId     Node      Que   CflRefNumb    FirstRunN      LastsRunN   CCode       SubmTime       JobStat(1/10/20/100->subm/run/fin/ok)";
  my $jsw_lb=$jsw_fram->Label(-text=>$label, -anchor=>'w', -font => $font2a)
                                            ->place(
                                                    -relwidth=>1., -relheight=>0.07,
                                                    -relx=>0, -rely=>0);
#---
  $jsw_lbox=$jsw_fram->Scrolled("Listbox", -scrollbars => "e", -selectmode => "multiple",
                                                            -relief => "sunken",
							    -font => $font2,
							    -background=>LightBlue,
                                                            -selectbackground => "yellow",
							    -selectforeground => "red"
							    )->place(
                                                            -relwidth=>1., -relheight=>0.93,
                                                            -relx=>0, -rely=>0.07);
#----------
  $ResetHistFlg=0;
  my ($pid,$sjpid,$rjpid,$fjpid,$user,$stat,$queue,$frhost,$exhost,$jname,$sbtime);
  my ($nel,$line,$title,$string,$format);
  my @array=();
  my @strarr=();
  my @substrg=();
  my @string=();
  my @substr=();
  my @ssstr=();
  my @SubmJStat=();
  my @SubmJPid=();
  my @rarray=();
  my ($dir,$cmd);
  my ($nelsjl,$nelfjl,$nelrjl);
  $KilledJobs=0;
  @KilledJobsPids=();
  my $jstat=0;
#
  show_messg("\n   <========= Enter 'CheckJobStatus' ...","B");
#
#---> extract job-pids, job-stats from @SubmJobsList(to use locally):
#
  $nelsjl=scalar(@SubmJobsList);
#  @strarr format => JobId  Node  Que  CflRefNumb  FirstRunN  LastsRunN  CCode  SubmDate  SubmTime  JobStat
#  (if split is used)
  @strarr=();
  for($j=0;$j<$nelsjl;$j++){
    @strarr=split(/\s+/,$SubmJobsList[$j]);
    push(@SubmJPid,$strarr[0]);
    push(@SubmJStat,$strarr[9]);
  }
#
#---> check the presence of all running/pend jobs:
#
  @array=();
  my $cmd="bjobs |";
  $pid=open(RJOBS,$cmd) or show_warn("\n   <--- Cannot run 'bjobs' to see running jobs  $!");
  while(<RJOBS>){
    push(@array,$_);
  }
  close(RJOBS);
#
  $nelrjl=scalar(@array);
  if($nelrjl>1){show_messg("\n   <--- JobStatus: found the following running/pend jobs : \n");}
  else{
    show_messg("\n   <--- JobStatus: no any running/pend jobs, @array !!! \n");
    goto NextStage1;
  }
#
  my $nell;
  for($j=0;$j<$nelrjl;$j++){
    if($j==0){
      $line=" Jobid    User     Stat   Queue   From_host Exec_host  JobName     SubmDate  \n";
      $logtext->insert('end',$line);
      $logtext->yview('end');
    }
    if($j>0){# to skip title line
      @string=split(/\s+/,$array[$j]);
      $nell=scalar(@string);
      if($nell<9){next;}#<-- crazy line (LSF bug) - skip it
      $rjpid=$string[0];
      $user=$string[1];
      $stat=$string[2];
      $queue=$string[3];
      $frhost=$string[4];
      if($stat eq "RUN"){
        $exhost=$string[5];
        $jname=$string[6];
        $sbtime=$string[7]."  ".$string[8]." ".$string[9];
	$jstat=10;#running
      }
      elsif($stat eq "PEND"){
        $exhost="---------";
        $jname=$string[5];
        $sbtime=$string[6]."  ".$string[7]." ".$string[8];
	$jstat=5;#pend
      }
      else{
        show_warn("\n   <--- JobStatus: unknown job-stat, $stat !");
        $exhost=$string[5];
        $jname=$string[6];
        $sbtime=$string[7]."  ".$string[8]." ".$string[9];
	$jstat=0;#???
      }
      $line=pack($BjobsFmt,$rjpid,$user,$stat,$queue,$frhost,$exhost,$jname,$sbtime);
      $logtext->insert('end',$line."\n");
      $logtext->yview('end');
      for($i=0;$i<$nelsjl;$i++){#find rjpid in sjpid-list
        $sjpid=$SubmJPid[$i];
        if($sjpid==$rjpid){#<-- 'in queue'-job found in SubmJobsList
	  $SubmJStat[$i]=$jstat;#set stat=run/pend	
          $line=$SubmJobsList[$i];
          @rarray=unpack($SubmJobsFmt,$line);
          $rarray[9]=$SubmJStat[$i];#<-- update stat in SubmJobsList
	  if($rarray[1] eq "---------"){$rarray[1]=$exhost;}#<-- update host2run when was unknown
          $SubmJobsList[$i]=pack($SubmJobsFmt,@rarray);
	}
      }
    }
  }
#-------->
NextStage1:
#
#---> create LogFileList(to use locally!) as marker of finished jobs:
#
  my $date,$time,$lfname,$lfpid,$ccode,$strun,$dtime,$dattim,$fcrmin,$crutc,$times,$name;
  @LogFileList=();
  $dir=$workdir.$amsjwd;
  $cmd="ls -ogt --time-style=long-iso ".$dir."/TofTDCL*.*[0-9]".".log.*[0-9] |";
  @array=();
  $pid=open(FJOBS,$cmd) or show_warn("\n   <--- Cannot make log-file list in amsjobwd  $!");
  while(<FJOBS>){
    push(@array,$_);
  }
  close(FJOBS);
  $nelfjl=scalar(@array);
  if($nelfjl==0){show_messg("\n   <--- There are no any log-files in $amsjwd-directory !");}
  for($j=0;$j<$nelfjl;$j++){
    @string=split(/\s+/,$array[$j]);
    $date=$string[3];# yyyy-mm-dd
    @ssstr=split(/-/,$date);
    $time=$string[4];# hh:mm
    $lfname=$string[5];#logf-name(complete path !!!)
    $dtime=$ssstr[2].".".$time;#dd.hh:mm
    $times=$time.":00";
    $dattim=$date." ".$times;# yyyy-mm-dd hh:mm:00
    $crutc=time2run($dattim);#<-- logf creation utc-time (sec)
    $fcrmin=$crutc/60;# logf creation loc.time(since 1900) in minutes (8digs)
    @ssstr=split(/\//,$lfname);# split log-file name by "/"
    $nel=scalar(@ssstr);
    @substr=split(/\./,$ssstr[$nel-1]);# split last variable(generic log-file name) by "."
    $nel=scalar(@substr);
    $lfpid=$substr[$nel-1];#number after last "."(embedded jobid)
    $strun=$substr[$nel-3];#number after first "."(1st daq-file number used, 10digs)
    $name=$substr[0];# ="TofTDCLnnn"
    $ccode=substr($name,-3,3);
    $stat=1;#<--- logf status=found (2->jobconfirmed,3->logf(job)goodquality,4->cfilesconf)
    for($i=0;$i<$nelsjl;$i++){#find lfpid in sjpid-list
      $sjpid=$SubmJPid[$i];
      if($sjpid==$lfpid){$SubmJStat[$i]=20;}#set job-stat=finished
    }
    $line=pack($LogfListFmt,$lfpid,$ccode,$strun,$dtime,$fcrmin,$stat)."\n";
    push(@LogFileList,$line);#<-- add to LofFileList new member(line) with latest status
  }
#
#---> show all submitted jobs list with updated status:
#
  @array=();
  show_messg("\n   <--- JobStatus: current list of submitted upto now jobs:\n");  
  $title="JobId    Node    Que  CflRefNumb   FirstRunN    LastsRunN  CCode   SubmTime       JobStat \n";
  $logtext->insert('end',$title);
  $logtext->yview('end');
  for($j=0;$j<$nelsjl;$j++){
    $line=$SubmJobsList[$j];
    @array=unpack($SubmJobsFmt,$line);
    $array[9]=$SubmJStat[$j];#<--update status
    $line=pack($SubmJobsFmt,@array);
    $SubmJobsList[$j]=$line;
    $logtext->insert('end',$line."\n");
    $logtext->yview('end');
  }
#---
  @rlistbox=();
  for($i=0;$i<$nelsjl;$i++){#prepare list to view
    $line=$SubmJobsList[$i];
    push(@rlistbox,$line);
  }
  $jsw_lbox->insert("end",@rlistbox);
  show_messg("\n\n   <--- JobStatus: submitted Jobs List is updated, you can view/delete/... !");
}
#--------------------
sub jcontr_kill
{
  my (@line,$string,$cmd,$pid2kill,$killedpid,$pid,$pid_alive,$rhost,$index,);
  my @warr=();
  my $array=();
  my @selected=$jsw_lbox->curselection();
  $KilledJobs=0;
  @KilledJobsPids=();
#
  foreach (@selected) {
    $index=$_;
    $jsw_lbox->selectionClear($index);
    $line=$SubmJobsList[$index];
    @array=unpack($SubmJobsFmt,$line);
    $pid2kill=$array[0];
    $stat=$array[9];
    $rhost=$array[1];
    $cmd="bkill ".$pid2kill." |";
    $pid=open(KJOBS,$cmd) or show_warn("\n   <--- Cannot do 'bkill' to kill job $pid2kill, $!");
    while(<KJOBS>){
      $line=$_;
      @warr=split(/\s+/,$line);#warr => "Job <11230> is being terminated"
      print "mess=",$line,"\n";
    }
    close(KJOBS);
    $killedpid=$warr[1];
    $killedpid =~ tr/0-9\<\>:/0-9/d;#remove <> in output <killedpid>
    $pid_alive=(kill 0 => $pid);#check existance of process $pid
    if(($killedpid==$pid2kill) && ($warr[4] eq "terminated") && $pid_alive==0){
      show_messg("\n   <--- Job $pid2kill is successfully killed !!!");
      $KilledJobs+=1;#<- count really killed jobs
      $array[9]=-1;#<--update status to -1 (killed)
      $line=pack($SubmJobsFmt,@array);
      $SubmJobsList[$index]=$line;#<--update list in memory
      push(@KilledJobsPids,$pid2kill);
    }
    else{
      kill 9 => $pid;
      show_warn("\n   <--- JobKill: selected job $pid2kill is not recognized as running, pend or finished !!!");
      if($stat==1){
        show_warn("         this is a lost job - it will be removed from SubmJobs-list anyway !!!");
        $KilledJobs+=1;#<- count really killed jobs
        $array[9]=-1;#<--update status to -1 (killed)
        $line=pack($SubmJobsFmt,@array);
        $SubmJobsList[$index]=$line;#<--update list in memory
        push(@KilledJobsPids,$pid2kill);
      }
      else{
        show_warn("         possibly the 'kill' command failed, so you can try once again...");
      } 
    }
  }
}
#---------------------
sub jcontr_susp
{
}
#---------------------
sub jcontr_rel
{
}
#---------------------
sub jcontr_quit
{
  my ($dir,@line,$cmd,$killedpid,$pid,$pid_alive,$status,$fn);
  my @array=();
  my @string=();
  my @substr=();
  my @ssstr=();
  my $lfpid,$ccode,$strun,$time,$utcmin,$stat;
  my $fn,$fne,$rwsta,$fnfr,$kind,$kjpid;
  my $nellfl=0;
  my $dellogs=0;
  my $attempts=0;
  my $date,$time,$lfname,$lfpid,$ccode,$strun,$dtime,$dattim,$fcrmin,$crutc,$times,$name;
#
if($KilledJobs > 0){
#
TryAgain_1:
  $mwnd->after(8000);#<--- wait to be shure that killed jobs log-files are arrived
  $attempts+=1;
#
#---> recreate log-files list:
#
  @LogFileList=();
  $dir=$workdir.$amsjwd;
  $cmd="ls -ogt --time-style=long-iso ".$dir."/TofTDCL*.*[0-9]".".log.*[0-9] |";
  @array=();
  $pid=open(FJOBS,$cmd) or show_warn("\n   <--- Cannot make log-file list in amsjobwd  $!");
  while(<FJOBS>){
    push(@array,$_);
  }
  close(FJOBS);
  $nellfl=scalar(@array);
  if($nellfl==0){show_messg("\n   <--- There are no any TDCL log-files in $amsjwd-directory !");}
  for($j=0;$j<$nellfl;$j++){
    @string=split(/\s+/,$array[$j]);
    $date=$string[3];# yyyy-mm-dd
    @ssstr=split(/-/,$date);
    $time=$string[4];# hh:mm
    $lfname=$string[5];#logf-name(complete path !!!)
    $dtime=$ssstr[2].".".$time;#dd.hh:mm
    $times=$time.":00";
    $dattim=$date." ".$times;# yyyy-mm-dd hh:mm:00
    $crutc=time2run($dattim);#<-- logf creation utc-time (sec)
    $fcrmin=$crutc/60;# logf creation loc.time(since 1900) in minutes (8digs)
    @ssstr=split(/\//,$lfname);# split log-file name by "/"
    $nel=scalar(@ssstr);
    @substr=split(/\./,$ssstr[$nel-1]);# split last variable(generic log-file name) by "."
    $nel=scalar(@substr);
    $lfpid=$substr[$nel-1];#number after last "."(embedded jobid)
    $strun=$substr[$nel-3];#number after first "."(1st daq-file number used, 10digs)
    $name=$substr[0];# ="TofTDCLnnn"
    $ccode=substr($name,-3,3);
    $stat=1;#<--- logf status=found (2->jobconfirmed,3->logf(job)goodquality,4->cfilesconf)
    $line=pack($LogfListFmt,$lfpid,$ccode,$strun,$dtime,$fcrmin,$stat)."\n";
    push(@LogFileList,$line);#<-- add to LogFileList new member(line) with latest status
  }
#
#---> delete killed job's log-files:
#
  $nellfl=scalar(@LogFileList);
  for($lf=0;$lf<$nellfl;$lf++){#<---logf-loop (tempor.created above)
    $line=$LogFileList[$lf];
    ($lfpid,$ccode,$strun,$time,$utcmin,$stat)=unpack($LogfListFmt,$line);
    for($kind=0;$kind<$KilledJobs;$kind++){#<-- look into killed jobs list
      $kjpid=$KilledJobsPids[$kind];
      if($kjpid==$lfpid){#<-- delete related log(e)-files
        $fn="TofTDCL".$ccode.".".$strun.".log.".$lfpid;
        $fnfr=$workdir.$amsjwd."/".$fn;
        $rwsta = system("rm -f $fnfr");
        if($rwsta != 0){show_warn("\n   <-- Can't delete $fn in work-dir !");}
        else{show_messg("\n   <-- file $fn is deleted in work-dir !");}
#      
        $fne="TofTDCL".$ccode.".".$strun.".loge.".$lfpid;
        $fnfr=$workdir.$amsjwd."/".$fne;
        $rwsta = system("rm -f $fnfr");
        if($rwsta != 0){show_warn("\n   <-- Can't delete $fne in work-dir !");}
        else{show_messg("\n   <-- file $fne is deleted in work-dir !");}
	$dellogs+=1;
      }
    }#--->endof killed jobs loop
  }#-->endof logf loop
#
  if($dellogs<$KilledJobs && $attempts<5){
    show_messg("     <-- Deleting logs of killed jobs, attempt $attempts... !","Y");
    goto TryAgain_1;
  }
  if($dellogs<$KilledJobs){
    show_warn("\n   <-- Failed to delete all log-files of killed jobs(dlogs=$dellogs,djobs=$KilledJobs)");
    show_warn("\n       after $attempts attempts !");
  }
#
#---> delete cal-files(if any) of deleted jobs and related link-dirs  :
#
  @array=();
  my $nelsjl=scalar(@SubmJobsList);
  my $crefn,$fext,$sjpid;
  my $dflinksTD=$workdir.$daqflnk;
  my $dflinksSD,$cmdstat;
#
  for($j=0;$j<$nelsjl;$j++){
    $line=$SubmJobsList[$j];
    @array=unpack($SubmJobsFmt,$line);
    $sjpid=$array[0];
    $crefn=$array[3];
    $strun=$array[4];
    $ccode=$array[6];
    $stat=$array[9];
    if($stat==-1){#<-- job was deleted, delete related cal-files
      $sjpid=$array[0];
      $crefn=$array[3];
      $strun=$array[4];
      $ccode=$array[6];
      if((($ccode%100)/10)>0){$fext="_ecom";}
      else{$fext="_norm";}
      $fn=$cfilenames[6].".".$fext;# <-- Tdcor
      $fnfr=$workdir.$amsjwd."/".$fn;
      $rwsta = system("rm -f $fnfr");
      if($rwsta == 0){show_messg("\n   <-- file $fn is deleted as related with killed job $sjpid !");}
      else{show_warn("\n   <-- can't delete $fn of killed job $sjpid !");}
#
#--> remove related link-dirs:
      $dflinksSD="/JL".$strun;
      $dir=$dflinksTD.$dflinksSD;
      $cmd="rm -rf $dir";
      $cmdstat=system($cmd);
      if($cmdstat != 0){
        show_warn("\n   <--- Failed to remove links-dir of deleted job $sjpid !!!");
      }
      else{show_messg("\n   <--- Command 'delete' links-dir of deleted job $sjpid is done  !");}
    }#--->endof 'killed job' check
  }
#
#---> remove killed job from SubmJobsList and file:
#
  @array=();
  $fn="TofSubmJobsList".".".$SessName;
  my @newlist=();
  open(SJLIST,"> $fn") or die show_warn("\n   <--- Can't open $fn for removing of killed job, $!");
  for($j=0;$j<$nelsjl;$j++){
    $line=$SubmJobsList[$j];
    @array=unpack($SubmJobsFmt,$line);
    if($array[9]==-1){
      next;#<-- skip deleted job line
    }
    print SJLIST $line."\n";
    push(@newlist,$line);
  }
  close(SJLIST) or show_warn("\n   <--- Cannot close $fn after writing, $!");
  @SubmJobsList=();
  push(@SubmJobsList,@newlist);
  show_messg("\n   <---  SubmJobsList/file are updated after job killing !");
#  $status=system("chmod 666 $fn");
#  if($status != 0){
#    show_warn("\n   <---  problem with write-priv for $fn, stat=$status !");
#    print "Warning: problem with write-priv for SubmJobsList-file, status=",$status,"\n";
#  }
#  else{
#    show_messg("\n   <---  write-priv for $fn is ok !");
#  }
#---
  $UnFinishedJobs-=$KilledJobs;
}
  show_messg("\n   <--- Left CheckJobsStatus with $KilledJobs killed jobs  !");
  $topl3->destroy();
  $mwnd->update;
}
#-------------------------- JobOutpControl Area --------------------------------------
sub JobOutpControl
{
  $ResetHistFlg=0;
#
#---> define size and create JobOutpControl window:
#
  $topl4=$mwnd->Toplevel();
  $swdszx=$mnwdszx;#X-size of 2nd window (portion of display x-size)
  $swdszy=1-$mnwdszy-0.11;#Y-size of 2nd window (portion of display y-size)(0.1->bot.syst.panel)
  my $szx=int($swdszx*$displx);#in pixels
  my $szy=int($swdszy*$disply);
  my $swdposx="+0";
  my $swdpy=int(($mnwdszy+0.03)*$disply);# 2nd wind. pos-y in pixels(0.03->safety)
  my $swdpozy="+".$swdpy;
  my $swdgeom=$szx."x".$szy.$swdposx.$swdpozy;
  print "JobOutpControl-window geom-",$swdgeom,"\n";
  $topl4->geometry($swdgeom);
  $topl4->title("                  Finished Jobs Output Control window");
#---> frame for outp-control pannel:
  my $nbuttons=6;
  my $lbfwid=0.84;#width(portion of topl4-window) of listboxes frame
  my $bvrspac=0.05;#button rel V-spacing in units of button-height
  my $vbias=0.08;#buttons v-bias to place frame-label
  my $bheight=(1-$vbias)/(1+$bvrspac)/$nbuttons;
  my $bspac=$bheight*$bvrspac;#abs.v-spacing for buttons
  my $crely;
  $jocp_fram=$topl4->Frame(-label => "JobOutp Control panel",-background => "Cyan",  
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>(1-$lbfwid), -relheight=>1,
                                                    -relx=>0, -rely=>0);
  $crely=$vbias;
#--- 
  $showlf_bt=$jocp_fram->Button(-text=>"ShowLog",
                                                        -font=>$font2,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ocontr_shlog)->place(
                                                                 -relwidth=>0.5, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $showelf_bt=$jocp_fram->Button(-text=>"ShowELog",
                                                        -font=>$font2,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ocontr_shelog)->place(
                                                                 -relwidth=>0.5, -relheight=>$bheight,
                                                                           -relx=>0.5, -rely=>$crely);
  $crely+=($bheight+$bspac);
#--- 
  $matchf_bt=$jocp_fram->Button(-text=>"MatchFiles",
                                                        -font=>$font2,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ocontr_match)->place(
                                                                 -relwidth=>1., -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $crely+=($bheight+$bspac);
#--
  my $deletf_bt=$jocp_fram->Button(-text=>"AddNewRefSet",
                                                        -font=>$font2,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ocontr_addrefset)->place(
                                                                 -relwidth=>1., -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $crely+=($bheight+$bspac);
#--
  $cleanup_bt=$jocp_fram->Button(-text=>"Cleanup",
                                                        -font=>$font2,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ocontr_cleanup)->place(
                                                                 -relwidth=>1., -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $crely+=($bheight+$bspac);
#--
  my $edcfl_bt=$jocp_fram->Button(-text=>"EditCflistF", 
                                                        -font=>$font2,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ocontr_editcl)->place(
                                                                 -relwidth=>1., -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $crely+=($bheight+$bspac);
#--
  $finish_bt=$jocp_fram->Button(-text=>"Finish", 
                                                        -font=>$font2,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ocontr_finish)->place(
                                                                 -relwidth=>1., -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
#-------- frame for logf/calf-listboxes:
  my $nlistbs=2;
  my $bhrspac=0.002;#listb rel H-spacing in units of listb-width
  my $vbias=0.08;#listb v-bias to place frame-label
  my $lbwid=1/($nlistbs*(1+$bhrspac)-$bhrspac);
  my $lbspac=$lbwid*$bhrspac;#abs.h-spacing for listboxes
  my $crelx;
  @JClboxPointer=();
#
  $listb_fram=$topl4->Frame(-background => "Cyan", 
                          -relief=>'groove', 
			  -borderwidth=>5)->
			   place(-relwidth=>$lbfwid, -relheight=>1,
                                 -relx=>(1-$lbfwid), -rely=>0);
#---
  my $label1="  LogFiles(jobid, CalCode, RunN, CrTime, stat=2/3/4->jobconf/good/Cfmatched)";
  my $label2="                   TdcorFiles(RunN, CrTime, stat=0/1/2->MarkedAsBad/Found/LogfMatch, type";
  my $label=$label1.$label2;
  my $logf_lab=$listb_fram->Label(-text=>$label, -anchor=>'w', -font => $font2)
                                            ->place(
                                                    -relwidth=>1, -relheight=>$vbias,
                                                    -relx=>0, -rely=>0);
  $crelx=0;
#--- logfile lbx
  $logf_lbx=$listb_fram->Scrolled("Listbox", -scrollbars => "e", -selectmode => "multiple",
                                                            -relief => "sunken",
							    -font => $font2c,
							    -background=>LightBlue,
                                                            -selectbackground => "yellow",
							    -selectforeground => "red"
							    )->place(
                                                            -relwidth=>$lbwid, -relheight=>(1-$vbias),
                                                            -relx=>$crelx, -rely=>$vbias);
#--- TdcorList lbx
  $crelx+=($lbwid+$lbspac);
  $JClboxPointer[6]=$listb_fram->Scrolled("Listbox", -scrollbars => "e", -selectmode => "multiple",
                                                            -relief => "sunken",
							    -font => $font2c,
							    -background=>LightBlue,
                                                            -selectbackground => "yellow",
							    -selectforeground => "red"
							    )->place(
                                                            -relwidth=>$lbwid, -relheight=>(1-$vbias),
                                                            -relx=>$crelx, -rely=>$vbias);
#----------
  my @sarray=();
  my @rarray=();
  my @array=();
  my @string=();
  my @substr=();
  my @ssstr=();
  my @lfresult=();
  my ($fname,$lfname,$dir,$line,$nel,$var,$nelsjl,$nelcft);
  my ($sjpid,$sjsta,$crefn,$ccode,$strun,$date,$time,$dtime,$stat,$cmd);
  my ($rjpid,$user,$rstat,$queue,$jname,$sbtag,$month,$day,$time,$hour,$mins);
  my ($nellfl,$ccode,$lfccode,$name);
  my $dflinksTD=$workdir.$daqflnk;
  my $dflinksSD,$cmdstat;
  my $nnel;
#
  $JobConfLogFiles=0;#incr. when log-file found and job-confirmed(in "JobOutpControl") 
  $GoodLogFiles=0;# + "good"
  $FiniJobCount=0;
  $JOCReady=0;
#  
  my @SubmJStat=();
  my @SubmJPid=();
  my @calqual=();
  my $jobname,$ext;
#
#---> check/update (in SubmJobList) status of all submitted(upto now) jobs:
#
  @sarray=();
  $nelsjl=scalar(@SubmJobsList);
  for($j=0;$j<$nelsjl;$j++){#<--- SubmJobList loop
    $line=$SubmJobsList[$j];
    @sarray=unpack($SubmJobsFmt,$line);
    $sjpid=$sarray[0];
    $crefn=$sarray[3];
    $strun=$sarray[4];
    $ext=substr($strun,-5,5);
    $jobname=$SessName.$ext;
    $ccode=$sarray[6];
    $stat=$sarray[9];# -1/0/1/5/10/20/30/40/100/->killed/???/submitted/pend/running/ended/haslogf/logfok/goodjob
    if($stat>0 && $stat<20){#<--send 'bjobs' to see running $sjpid job (if not found -> ended)
#---> call 'bjobs':
      @rarray=();
      $cmd="bjobs -u all -J $jobname $sjpid |";
      open(SJOBS,$cmd) or show_warn("   <--- Cannot run 'bjobs' to see running jobs,  $!");
      while(<SJOBS>){
        push(@rarray,$_);
      }
      close(SJOBS);
#
      $nel=scalar(@rarray);
      if($nel>1){
        for($i=1;$i<$nel;$i++){# '1' to skip title
          @string=split(/\s+/,$rarray[$i]);
	  $nnel=scalar(@string);
	  if($nnel<9){next;}#<-- crazy line (LSF bug) - skip it
          $rjpid=$string[0];
          $user=$string[1];
          $rstat=$string[2];
          $jname=$string[6];
	  $month=$string[7];
	  $day=$string[8];
	  $time=$string[9];
          $sbtag=$string[7]."  ".$string[8]." ".$string[9];
          if($rstat eq "RUN"){$stat=10;}
          elsif($rstat eq "PEND"){$stat=5;}
	  else{$stat=20;}#<-- EXIT,DONE,?
        }
      }
      else{
        $stat=20;#<-- given sjpid is not running(pend) -> i.e. job ended
      }
#--> update job-status in @SubmJobsList:
      $sarray[9]=$stat;#update job-status
      $line=pack($SubmJobsFmt,@sarray);
      $SubmJobsList[$j]=$line;
    }#<---endof job-stat check in the list
#
    if($stat==20){#<-- remove links-dir of finished job
      $FiniJobCount+=1;
      $dflinksSD="/JL".$strun;
      $dir=$dflinksTD.$dflinksSD;
      $cmd="rm -rf $dir";
      $cmdstat=system($cmd);
      if($cmdstat != 0){
        show_warn("\n   <--- Failed to remove links-dir of finished job $sjpid !!!");
      }
      else{show_messg("\n   <--- Command 'delete' Links-dir of finished job $sjpid is done  !");}
    }
#
#-> save job pid/stat(updated) in separate lists:
    push(@SubmJPid,$sjpid);
    push(@SubmJStat,$stat);
  }#--->endof SubmJobList loop
#
  $FinishedJobs+=$FiniJobCount;
  if($FiniJobCount==0){
    show_messg("\n   <--- JobOutputControl: there are no New finished jobs !");
#    return;
  }
#
#------> create the list of A-L-L log-files(=> finished jobs) on the moment of 'JobOutpContr' clicking :
#                                     (also match with jobs)
  @LogJobMatchIndex=();
# print "create list of log-files...","\n";
  $dir=$workdir.$amsjwd;
  $cmd="ls -ogt --time-style=long-iso ".$dir."/TofTDCL*.*[0-9]".".log.*[0-9] |";
  @array=();
  open(FJOBS,$cmd) or show_warn("\n   <--- JobOutputControl: Cannot make log-file list in amsjobwd  $!");
  while(<FJOBS>){
    chomp;
    push(@array,$_);
  }
  close(FJOBS);
  $nellfl=scalar(@array);
  if($nellfl==0){
    show_messg("\n   <--- JobOutputControl: there are no any log-files in $amsjwd-directory !");
    $JOCReady=1;
    return;
  }
#---
  my ($dattim,$lfpid,$times,$crutc,$fcrmin);
  my ($ccodeok,$jmindx,$stline);
  @LogFileList=();
#
  for($j=0;$j<$nellfl;$j++){#<--- logf-loop
    @string=split(/\s+/,$array[$j]);
    $date=$string[3];# yyyy-mm-dd
    @ssstr=split(/-/,$date);
    $time=$string[4];# hh:mm
    $lfname=$string[5];#logf-name(complete path !!!)
    $dtime=$ssstr[2].".".$time;#dd.hh:mm
    $times=$time.":00";
    $dattim=$date." ".$times;# yyyy-mm-dd hh:mm:00
    $crutc=time2run($dattim);#<-- logf creation utc-time (sec)
    $fcrmin=$crutc/60;# logf creation loc.time(since 1900) in minutes (8digs)
    @ssstr=split(/\//,$lfname);# split log-file name by "/"
    $nel=scalar(@ssstr);
    @substr=split(/\./,$ssstr[$nel-1]);# split last variable(generic log-file name) by "."
    $nel=scalar(@substr);
    $lfpid=$substr[$nel-1];#number after last "."(embedded jobid)
    $strun=$substr[$nel-3];#number after first "."(1st daq-file number used, 10digs)
    $name=$substr[0];# ="TofTDCLnnn"
    $ccode=substr($name,-3,3);
    $stat=1;#<--- logf status=found (2->jobconfirmed,3->logf(job)goodquality,4->cfilesconf)
    for($ij=0;$ij<$nelsjl;$ij++){#<-- match logs with jobs (by sjpid)
      $sjpid=$SubmJPid[$ij];
      $sjsta=$SubmJStat[$ij];
      if($lfpid==$sjpid && ($sjsta==5 || $sjsta==10 || $sjsta==20)){#<-- found related job(running/pend job could be finished just befor)
        $stat=2;#<--logf-status = "job-matched"
	$JobConfLogFiles+=1;
        $SubmJStat[$ij]=30;#<-- job-status = "logf-matched"
	$jmindx=$ij;
	$LogJobMatchIndex[$j]=$ij;# attach matched job-index(as in SubmJobList) to LogFileList index
# print "Matched Log ",$lfpid," with job ",$sjpid," Lf ccode=",$ccode," name=",$name,"\n";  
        last;
      }
    }#--->endof jobs loop 
#---> check job(log)-quality(special info(block) near the end of log-file):
    if($stat==2){#<-- logf is job-confirmed, check quality 
      @rarray=();
      $fname=$lfname;
      open(FLOGS,"< $fname") or show_warn("\n   <--- JobOutputControl: Cannot open $lfname,  $!");
      while(defined ($line = <FLOGS>)){
        chomp $line;
        if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
        $line =~s/^\s+//s;# remove " "'s at the beg. of line
        $line =~s/\s+$//s;# remove " "'s at the end  of line
        push(@rarray,$line);
      }
      close(FLOGS);
#-> search/read "results"-block:
      $nel=scalar(@rarray);
      $stline=1000;
      @lfresult=();
      for($il=$stline;$il<$nel;$il++){
	if(($rarray[$il] eq "<==========Tof CALIB Results:") && ($rarray[$il+$ResultsBlockLen-1] eq "<==========End of Results")){
	  for($k=1;$k<$ResultsBlockLen-1;$k++){push(@lfresult,$rarray[$il+$k]);}#read results section into buffer
	  last;
	}
      }
      $nel=scalar(@lfresult);
      $ccodeok=0;
      if($nel==$ResultsBlockLen-2){#<-- decoding/checking results block(header/trailler removed)
	if(($ccode==900) || ($ccode==910)){#<--TDCL-results check(accept MN=00/10, i.e. 1-pass normal/economic modes)
	  @calqual=split(/\s+/,$lfresult[3]);
	  if($calqual[0]>1000 && $calqual[1]>100 && $calqual[2]<20 && $calqual[3]<5.5){#<-- TDCL pars ok:
#         aver.evs/bin | aver.min.evs/bin | chan% with low evs/bin in any bin | chan% dead(connected, but aver.evs/bin too low)                 
	    $ccodeok+=1;
	  }
	  else{show_messg("\n   <--- Bad Tdcor-results: $lfresult[3]\n");}
	}
      }
      else{
	show_warn("\n   <--- Finished/logmatched job $lfpid has no result block in Log-file !!!");
      }
      if($ccodeok>0){
        $GoodLogFiles+=1;
        $stat=3;#<--log with good results
        $SubmJStat[$jmindx]=40;#<-- job has logf with good results
      }
      elsif($nel==$ResultsBlockLen-2){
	show_warn("\n   <--- Finished/logmatched job $lfpid has bad result according to Log-file !!!");
	show_warn("      results: $calqual[0], $calqual[1], $calqual[2], $calqual[3] ");
      }
#--> update job-status in SubmJobsList:
      $line=$SubmJobsList[$jmindx];
      @rarray=unpack($SubmJobsFmt,$line);
      $rarray[9]=$SubmJStat[$jmindx];
      $SubmJobsList[$jmindx]=pack($SubmJobsFmt,@rarray);
#
    }#--->endof 'logf is job-confirmed'
#<--- endof job(logf) quality check
    $line=pack($LogfListFmt,$lfpid,$ccode,$strun,$dtime,$fcrmin,$stat)."\n";
    push(@LogFileList,$line);#<-- add to LofFileList new member(line) with latest status
  }#--->endof logf-loop
#
  if($GoodLogFiles==0){  
    show_messg("\n   <--- JobOutputControl: there are no any good log-files in $amsjwd-directory !");
#    return;
  }
#
#------> create the list of A-L-L ordinary(incl.'_norm') Tdcor-files on the moment of 'JobOutpControl' clicking :
#                 (RefCalib-files are not included in this ordinary cal-files lists !!!)
#
  my @Cfrefs=();
  $Cfrefs[0]=\@CflistList;
  $Cfrefs[1]=\@CStatList;
  $Cfrefs[2]=\@TdelvList;
  $Cfrefs[3]=\@TzslwList;
  $Cfrefs[4]=\@AmplfList;
  $Cfrefs[5]=\@ElospList;
  $Cfrefs[6]=\@TdcorList;
  @TdcorfExt=();#to keep found outfiles extentions(_ecom/_norm)
#-- lists of ref.cal-files:
  my @refCfrefs=();
  $refCfrefs[0]=\@RefCflistList;# all below are already existing(created in 'Welcome')
  $refCfrefs[1]=\@RefCStatList;
  $refCfrefs[2]=\@RefTdelvList;
  $refCfrefs[3]=\@RefTzslwList;
  $refCfrefs[4]=\@RefAmplfList;
  $refCfrefs[5]=\@RefElospList;
  $refCfrefs[6]=\@RefTdcorList;
#-- reset Index-lists of matched cal-files(created in 'Match'):
  $MatchCFSets=0;
  @MatCflistInd=();
  @MatCStatInd=();
  @MatTdelvInd=();
  @MatTzslwInd=();
  @MatAmplfInd=();
  @MatElospInd=(); 
  @MatTdcorInd=(); 
#----
  $nelcft=scalar(@cfilenames); 
#---> see calib-files in amsjobwd: 
  my @ncfiles=();
  my ($refstrun,$nrefcfl,$refcffl);
  my $length,$ofext;
#
  for($i=0;$i<$nelcft;$i++){# <--- cal-file types loop
    @{$Cfrefs[$i]}=();# clear refs
    if($i!=6){next;}#<-- need only Tdcor-type(6) files
    @array=();
    $cmd="ls -ogt --time-style=long-iso ".$dir."/".$cfilenames[$i].".*[0-9_cemnor] |";
    open(FLIST,$cmd) or show_warn("   <-- JobOutputControl: Cannot make cal-file list in amsjobwd  $!");
    while(<FLIST>){
      push(@array,$_);
    }
    close(FLIST);
    $ncfiles[$i]=scalar(@array);
# print "cftype=",$i," nfiles=",$ncfiles[$i],"  arr1=",$array[0]," ",$array[1],"\n";
    for($j=0;$j<$ncfiles[$i];$j++){#<--- cfiles-loop for type $i
      @string=split(/\s+/,$array[$j]);
      $date=$string[3];# yyyy-mm-dd
      @ssstr=split(/-/,$date);
      $time=$string[4];# hh:mm
      $dtime=$ssstr[2].".".$time;#dd.hh:mm
      $times=$time.":00";
      $dattim=$date." ".$times;# yyyy-mm-dd hh:mm:00
      $crutc=time2run($dattim);#<-- calf creation utc-time (sec)
      $fcrmin=$crutc/60;# calf creation loc.time(since 1900) in minutes (8digs)
      @substrg=split(/\./,$string[5]);
      $nel=scalar(@substrg);
      $ext=$substrg[$nel-1];#extention="number_norm(ecom)" after last "."
      $length=length($ext);
      if($length>10){# output Tdcor-file (with '_norm(ecom)' ext) 
        $strun=substr($ext,0,($length-5));# "-5" to remove "_ecom(_norm)"
	$ofext=substr($ext,-5,5);# '_ecom/_norm'
      }
      else{# standard Tdcor-file
        $strun=$ext;
	$ofext="";
      }
      if($strun==111){next;}#<-- skip generic files(used as input for db-update jobs)
#  print "cfile:",$j," ext=",$ext,"  length=",$length," strun=",$strun,"\n";
#
      $nrefcfl=scalar(@{$refCfrefs[$i]});# ref.cal-files of type $i (here Tdcor-type)
      $refcffl=0;
      for($k=0;$k<$nrefcfl;$k++){#<-- ref.cal-files loop
        $refstrun=$refCfrefs[$i]->[$k];
	if($strun==$refstrun){
	  $refcffl=1;
	  last;
	}
      }#--->endof ref.cal-files loop
#
      $stat=1;# found 
      if($refcffl==1){
        $stat=10;# this file is one of the ref-files
	next;#go to next Tdcor file in the list(to skip current $j-file as ref.one)
      }
      $line=pack($CalfListFmt,$strun,$dtime,$fcrmin,$stat)."\n";
      push(@{$Cfrefs[$i]},$line);
      push(@TdcorfExt,$ofext);
    }#<---endof cfiles-loop for type $i
  }#---> endof cal-file types loop
#
#--- fill LogFiles Lbox:
#
  @loglistbox=();
  for($i=0;$i<$nellfl;$i++){#prepare list to view
    $line=$LogFileList[$i];
    @string=unpack($LogfListFmt,$LogFileList[$i]);
    $line=pack($LogfLboxFmt,$string[0],$string[1],$string[2],$string[3],$string[5]);# utcmin excluded in listbox
    push(@loglistbox,$line);
  }
  $logf_lbx->insert("end",@loglistbox);
#
#--- fill CalibFiles Lboxes:
#
  my @Cflbref=();
  $Cflbref[0]=\@CflistLbox;
  $Cflbref[1]=\@CStatLbox;
  $Cflbref[2]=\@TdelvLbox;
  $Cflbref[3]=\@TzslwLbox;
  $Cflbref[4]=\@AmplfLbox;
  $Cflbref[5]=\@ElospLbox;
  $Cflbref[6]=\@TdcorLbox;
#
  for($i=0;$i<$nelcft;$i++){#<-- cal-files types loop
    if($i!=6){next;}# need only Tdcor-type
    $nel=$ncfiles[$i];
    @{$Cflbref[$i]}=();
    if($nel>0){
      for($j=0;$j<$nel;$j++){#<--file-loop inside given type
        $line=$Cfrefs[$i]->[$j];
        @string=unpack($CalfListFmt,$line);
	if($i==6){$line=pack($CalfLboxFmt." A5",$string[0],$string[1],$string[3],$TdcorfExt[$j]);}# for Tdcor only
        else{$line=pack($CalfLboxFmt,$string[0],$string[1],$string[3]);}# utcmin excluded in listbox
        push(@{$Cflbref[$i]},$line);
      }
    }
    $JClboxPointer[$i]->insert("end",@{$Cflbref[$i]});
  }
#--------
  $topl4->update;
  $JOCReady=1;
}
#---------------
sub ocontr_shlog
{
  my $lfpid,$ccode,$strun,$time,$utcmin,$stat;
  my $index,$nel,$fn,$fname;
  my @selected=$logf_lbx->curselection();
  $nel=scalar(@selected);
  if($nel==0){
    show_messg("\n   <--- There is no selected file, do single selection !!!","Y");
    return;
  }
  elsif($nel>1){
    show_messg("\n   <--- Only single selection can be accepted !!!","Y");
    $logf_lbx->selectionClear(0,'end');
    return;
  }
#
  foreach (@selected) {
    $index=$_;
#    $logf_lbx->selectionClear($index);
    $line=$LogFileList[$index];
    ($lfpid,$ccode,$strun,$time,$utcmin,$stat)=unpack($LogfListFmt,$line);
    $fn="TofTDCL".$ccode.".".$strun.".log.".$lfpid;
    $fname=$workdir.$amsjwd."/".$fn;
    $stat=system("nedit -read $fname");
    if($stat != 0){
      show_warn("   <-- ShowLog error: fail to view file $fn !");
      return;
    }
  }
}
#---------------
sub ocontr_shelog
{
  my $lfpid,$ccode,$strun,$time,$utcmin,$stat;
  my $index,$nel,$fn,$fname;
  my @selected=$logf_lbx->curselection();
  $nel=scalar(@selected);
  if($nel==0){
    show_messg("\n   <--- There is no selected file, do single selection !!!","Y");
    return;
  }
  elsif($nel>1){
    show_messg("\n   <--- Only single selection can be accepted !!!","Y");
    $logf_lbx->selectionClear(0,'end');
    return;
  }
#
  foreach (@selected) {
    $index=$_;
#    $logf_lbx->selectionClear($index);
    $line=$LogFileList[$index];
    ($lfpid,$ccode,$strun,$time,$utcmin,$stat)=unpack($LogfListFmt,$line);
    $fn="TofTDCL".$ccode.".".$strun.".loge.".$lfpid;
    $fname=$workdir.$amsjwd."/".$fn;
    $stat=system("nedit -read $fname");
    if($stat != 0){
      show_warn("   <-- ShowErrLog error: fail to view file $fn !");
      return;
    }
  }
}
#---------------
sub ocontr_match
{
  my $ccodeok=0;
  my $tdcormok=0;
  my $statfmok=0;
  my $cflfmok=0;
  my @array=();
  my @rarray=();
  my @string=();
  my @rlistbox=();
  my $nlfmatch=0;
  
  my ($lfpid,$ccode,$strun,$time,$utcmin,$stat,$strunc,$timec,$utcminc,$statc);
  
  my ($bsubpid,$Host2run,$Queue2run,$refcalsetn,$CalRun1,$CalRun2,$CalCode,$submdate,$submtime,$jobsta);
  
  my ($dir,$line,$substr,$nel,$var,$nelsjl);
  my ($sjpid,$crefn,$ccode,$strun,$date,$time,$stat,$cmd);
  my ($rjpid,$user,$rstat,$queue,$jname,$sbtag,$month,$day,$time,$hour,$mins);
  my ($nellfl,$nelcft,$ccode,$lfccode,$name);
  
#-- make pointers to cal-files lists(filled at 'JobOutpControl'-click):
  my @Cfrefs=();
  $Cfrefs[0]=\@CflistList;
  $Cfrefs[1]=\@CStatList;
  $Cfrefs[2]=\@TdelvList;
  $Cfrefs[3]=\@TzslwList;
  $Cfrefs[4]=\@AmplfList;
  $Cfrefs[5]=\@ElospList;
  $Cfrefs[6]=\@TdcorList;
#-- make pointer to Index-lists of matched cal-files(reseted at 'JobOutpControl'-click):
  my @matCfrefs=();
  $matCfrefs[0]=\@MatCflistInd; 
  $matCfrefs[1]=\@MatCStatInd; 
  $matCfrefs[2]=\@MatTdelvInd; 
  $matCfrefs[3]=\@MatTzslwInd; 
  $matCfrefs[4]=\@MatAmplfInd; 
  $matCfrefs[5]=\@MatElospInd; 
  $matCfrefs[6]=\@MatTdcorInd; 
 
  my @cfmatchindx=();# current logfile  matched cal-files indexes  of each cal-type (1->last)

#-- make pointer to listboxes:
  my @Cflbref=();
  $Cflbref[0]=\@CflistLbox;
  $Cflbref[1]=\@CStatLbox;
  $Cflbref[2]=\@TdelvLbox;
  $Cflbref[3]=\@TzslwLbox;
  $Cflbref[4]=\@AmplfLbox;
  $Cflbref[5]=\@ElospLbox;
  $Cflbref[6]=\@TdcorLbox;
#
  $nelcft=scalar(@cfilenames); 
  $nellfl=scalar(@LogFileList);
  show_messg("\n   <========= Enter Match with $nellfl log-files in the list ...","B");
#
#---> match logfs with cal-files:
#
  for($lf=0;$lf<$nellfl;$lf++){#<---logf-loop
    $line=$LogFileList[$lf];
    ($lfpid,$ccode,$strun,$time,$utcmin,$stat)=unpack($LogfListFmt,$line);
    $ccodeok=0;
    $statfmok=0;
    $cflfmok=0;
    @cfmatchindx=();# match-indexes of cal-files for given log-file $lf 
    if($stat==3){#<-- logf is jobmatched+good
      for($i=0;$i<$nelcft;$i++){#<--cal.types loop
        if($i!=6){next;}# need only Tdcor-type
        $nel=scalar(@{$Cfrefs[$i]});
# print "--> Ctyp=",$cfilenames[$i]," Nfiles=",$nel,"\n";
        for($j=0;$j<$nel;$j++){#<--- cal-files loop
          $line=$Cfrefs[$i]->[$j];
          ($strunc,$timec,$utcminc,$statc)=unpack($CalfListFmt,$line);
# print "-> cf=",$j," strun/utc=",$strunc," ",$utcminc," stst=",$statc,"\n";
	  if($statc==10){next;}#<-- skip ref-files(if was accepted to be present in the list of all cal-files)
	  if((($strun==$strunc) || ($strunc==($strun+1)))
	                        && (abs($utcmin-$utcminc)<=5)
				&& ($statc==1)){#<--currently unmatched file satisfy matching conditions
	    $statc=2;
	    $cfmatchindx[$i]=$j+1;#<- +1 because "0" means 'no matching' for $i-th type of cal-file
	    $logf_lbx->selectionSet($lf);
	    $JClboxPointer[$i]->selectionSet($j);
	    $line=pack($CalfListFmt,$strunc,$timec,$utcminc,$statc);# update stat in cal-files list
	    $Cfrefs[$i]->[$j]=$line;
	    if($i==6){$tdcormok=1;}
	  }#--->endof matched
	}#--->endof cal-files loop
      }#--->endof cal-types loop
    }#<---endof 'logf is job_matched/good ?'
#
    if($tdcormok==1){#<-- overall matching ok
      $MatchCFSets+=1;
      $k=6;# tdcor-type
      push(@{$matCfrefs[$k]},$cfmatchindx[$k]);#<--store cal-files match-indexes
      push(@MatLogfInd,$lf+1);#<-- store log-file match-index
      $nlfmatch+=1;
      $stat=4;#-> set logf-stat to 'all matched'=ok
      $line=pack($LogfListFmt,$lfpid,$ccode,$strun,$time,$utcmin,$stat);# <- update stat in LogfList
      $LogFileList[$lf]=$line;
      @rarray=();
      $line=$SubmJobsList[$LogJobMatchIndex[$lf]];# <- update stat in SubmJobList
      @rarray=unpack($SubmJobsFmt,$line);
      $rarray[9]=100;# job processed and ok
      $SubmJobsList[$LogJobMatchIndex[$lf]]=pack($SubmJobsFmt,@rarray);
#
      $topl4->update;
#
      show_messg("\n   <--- Job(pid=$lfpid,ccode=$ccode) outputs are completely matched and OK !");
      $topl4->after(1000);#<-- delay 1sec on each matched line log-cfiles
#
    }#--->endof overall matching ok
#
  }#---> endof logf-loop
#------------------------
#
#--- update Log/CalFiles Lboxes to see new statuses:
#
  @loglistbox=();
  for($i=0;$i<$nellfl;$i++){#prepare list to view
    $line=$LogFileList[$i];
    @string=unpack($LogfListFmt,$LogFileList[$i]);
    $line=pack($LogfLboxFmt,$string[0],$string[1],$string[2],$string[3],$string[5]);# utcmin excluded in listbox
    push(@loglistbox,$line);
  }
  $logf_lbx->delete(0,"end");
  $logf_lbx->insert("end",@loglistbox);
#
  for($i=0;$i<$nelcft;$i++){#<-- cal-file types loop
    $nel=scalar(@{$Cfrefs[$i]});
    @{$Cflbref[$i]}=();
    if($i!=6){next;}# need only Tdcor-type
    if($nel>0){
      for($j=0;$j<$nel;$j++){#<--file-loop inside given type
        $line=$Cfrefs[$i]->[$j];
        @string=unpack($CalfListFmt,$line);
	if($i==6){$line=pack($CalfLboxFmt." A5",$string[0],$string[1],$string[3],$TdcorfExt[$j]);}# for Tdcor only
        else{$line=pack($CalfLboxFmt,$string[0],$string[1],$string[3]);}# utcmin excluded in listbox
        push(@{$Cflbref[$i]},$line);
      }
    }
    $JClboxPointer[$i]->delete(0,"end");
    $JClboxPointer[$i]->insert("end",@{$Cflbref[$i]});
  }
#
  $topl4->update;
#
  show_messg("\n   <--- Leaving 'Match' with $MatchCFSets Log/Cal_files matched sets !");
}
#---------------
sub ocontr_addrefset
{
#
  my @Cfrefs=();
  $Cfrefs[0]=\@CflistList;
  $Cfrefs[1]=\@CStatList;
  $Cfrefs[2]=\@TdelvList;
  $Cfrefs[3]=\@TzslwList;
  $Cfrefs[4]=\@AmplfList;
  $Cfrefs[5]=\@ElospList;
  $Cfrefs[6]=\@TdcorList;
#
  my @matCfrefs=();
  $matCfrefs[0]=\@MatCflistInd; 
  $matCfrefs[1]=\@MatCStatInd; 
  $matCfrefs[2]=\@MatTdelvInd; 
  $matCfrefs[3]=\@MatTzslwInd; 
  $matCfrefs[4]=\@MatAmplfInd; 
  $matCfrefs[5]=\@MatElospInd;
  $matCfrefs[6]=\@MatTdcorInd; 
#
  my @p2memlists=();
  $p2memlists[0]=\@RefCStatList;
  $p2memlists[1]=\@RefTdelvList;
  $p2memlists[2]=\@RefTzslwList;
  $p2memlists[3]=\@RefAmplfList;
  $p2memlists[4]=\@RefElospList;
  $p2memlists[5]=\@RefTdcorList;
#
  my @list=();
  my $nel=0;
  my ($index,$cstatind,$tdelvind,$tzslwind,$amplfind)=(0,0,0,0,0);
  my ($matlineind,$logind,$jobind)=(-1,-1,-1);
  my ($lfpid,$ccode,$strun,$time,$utcmin,$stat,$status);
  my $nel;
  my ($bsubpid,$Host2run,$Queue2run,$refcalsetn,$CalRun1,$CalRun2,$CalCode,$submdate,$submtime,$jobsta);
  my $refsetused=0;
  my @CflistFile=();
  my @RefCflistFile=();
  my $badsel=0;
#
  show_messg("\n   <------ Create new RefCalSet using selected Tdcor-file and default Cflist-file ...");
  if($MatchCFSets==0){
    show_warn("\n   <--- There is no any matched Tdcor-file  -  New RefSet can't be created !!!");
    goto BadRet;
  }
#
  @list=$JClboxPointer[6]->curselection();
#
  $nel=scalar(@list);
  if($nel==0){
    show_warn("\n   <--- There is no any selected item in Tdcor-files list, do selection and repeate !!!");
    goto BadRet;
  }
  elsif($nel>1){
    show_warn("\n   <--- More than one selected item in Tdcor-files list, de-select unnecessary and repeate !!!");
    goto BadRet;
  }
  else{#<-- selection ok in 1st approximation
    $index=$list[0];#<-- single=first, range:0-max
    for($i=0;$i<$MatchCFSets;$i++){#<-- find this $index in the matched Tdcor-files list
      if($MatTdcorInd[$i]==($index+1)){
        $matlineind=$i;#<-- index in matched lines list (total matched lines = $MatchCFSets) 
        last;
      }
    }
    if($matlineind<0){
      show_warn("\n   <--- selected Tdcor-file candidate was not matched, select correct one !!!");
      goto BadRet;
    }
#
#--> now selection ok, continue:
#
    my @MCalFileNums=();#<-- matched cal-files numbers as found in amsjobwd-dir(i.e.in *List-files) (0 if missing due ccode)
    my $nelcft=scalar(@cfilenames);
    my @string=();
    $badsel=0;
# 
    for($i=0;$i<$nelcft;$i++){#<--- calf-types loop
      if($i!=6){next;}# need only Tdcor-file
      $ind=$matCfrefs[$i]->[$matlineind];#<-- index+1 of matched cal-file in corresponding ***List
      if($ind>0){
        $line=$Cfrefs[$i]->[$ind-1];
        @string=unpack($CalfListFmt,$line);
        $MCalFileNums[$i]=$string[0];#<-- strun
      }
    }
#
    $logind=$MatLogInd[$matlineind];#<-- now it is index in @LogFileList
    $line=$LogFileList[$logind]; 
    ($lfpid,$ccode,$strun,$time,$utcmin,$stat)=unpack($LogfListFmt,$line);
    $jobind=$LogJobMatchIndex[$logind];#<-- matched job-index in SubmJobsList
    $line=$SubmJobsList[$jobind];
    ($bsubpid,$Host2run,$Queue2run,$refcalsetn,$CalRun1,$CalRun2,$CalCode,$submdate,$submtime,$jobsta)=unpack($SubmJobsFmt,$line);
#
#--> open default Cflist-file and create its members list:
#
    my @CflFileMemb=();#<-- cal-files numbers in default Cflist-file(1st element is total num_of_cfiles=6 !!)
    my $fn,$ffn;
    $fn=$cfilenames[0].".".$refcalsetn;
    $ffn=$workdir.$amsjwd."/".$fn;
    open(CFMEMB,"< $ffn") or show_warn("   <-- Cannot open $fn for reading, $!");
    while(defined ($line = <CFMEMB>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@CflFileMemb,$line);
    }
    close(CFMEMB) or show_warn("   <-- Cannot close $fn after reading, $!");
    $nel=scalar(@CflFileMemb);
    if($nel!=7){
      show_warn("\n   <--- default Cflist-file physically missing or something wrong inside !!!");
      goto BadRet;
    }
#
#---> update Tdcor-member in the members list and create new ref.Cflist-file:
#
    my $newsetn=$strun-1;# tempor use strun-1
    $CflFileMemb[6]=$strun;
    $fn=$cfilenames[0].".".$newsetn; 
    $ffn=$workdir.$amsjwd."/".$fn;
    open(NCFLF,"> $ffn") or show_warn("\n   <--- Cannot open new ref.Cflist-file $fn, $!");
    for($i=0;$i<7;$i++){print NCFLF $CflFileMemb[$i]."\n";}
    close(NCFLF) or show_warn("\n   <--- Cannot close new ref.Cflist-file $fn after creating, $!");
    $status=system("chmod 666 $ffn");
    if($status != 0){
      print "Warning: problem with write-priv for new ref.Cflist-file $fn, status=",$status,"\n";
    }
#
#--> update status of selected Tdcor-file in Tdcor-files list:
#
    for($i=0;$i<$nelcft;$i++){#<--- calf-types loop
      if($i!=6){next;}
      $ind=$matCfrefs[$i]->[$matlineind];#<-- index+1 of matched cal-file in corresponding ***List
      if($ind>0){#<-- means presence of cfile of type $i
        $line=$Cfrefs[$i]->[$ind-1];
        @string=unpack($CalfListFmt,$line);
	$string[3]=10;#<-- update status(member of new RefCalSet) in calf-lists
	$line=pack($CalfListFmt,@string);
	$Cfrefs[$i]->[$ind-1]=$line;
      }
    }
#
#--> add new Cflist and members to existing Ref-files lists:
#
    my $nmemb=$CflFileMemb[0];   
    for($j=0;$j<$nmemb;$j++){#<-- loop over members of new RefCflist-file
      push(@{$p2memlists[$j]},$CflFileMemb[$j+1]);# add to ref.memb-lists
    }
    push(@RefCflistList,$newsetn);#<-- add Cflist itself
#
#-- and update TofRefCflistList.TAUC(!!!) file:
#
    $WarnFlg=0;
    $fn="TofRefCflistList"."."."TAUC";
    $nel=scalar(@RefCflistList);
    open(CFLIST,"> $fn") or show_warn("   <-- Cannot open $fn for writing, $!");
    for($i=0;$i<$nel;$i++){
      $line=$RefCflistList[$i];
      print CFLIST $line."\n";
    }
    close(CFLIST) or show_warn("   <-- Cannot close $fn after writing $!");
    if($WarnFlg==0){
      show_messg("\n   <--- New RefCalFiles set was successfully created !!!");
      $LastRefCflistN=$RefCflistList[$nel-1];
    }
#
#---> rename selected Tdcor-file(remove _norm/ecom extention):
#
    $ind=$matCfrefs[$i]->[$matlineind];#<-- index+1 of matched cal-file in corresponding ***List
    $fn=$cfilenames[6].".".$strun;
    my $fnfr=$workdir.$amsjwd."/".$fn.$TdcorfExt[$ind-1];
    my $fnto=$workdir.$amsjwd."/".$fn;
    my $rwsta=0;
    $rwsta = system("mv $fnfr $fnto");
    if($rwsta != 0){show_warn("\n   <-- Can't rename $fn !");}
    else{show_messg("\n   <--- Cal-file $fn was renamed as standard file !");}
  }#<---endof "selection ok"
#
#---> update cal-files list-boxes:  
#
  for($i=0;$i<$nelcft;$i++){#<-- cal-file types loop
    if($i!=6){next;}
    $nel=scalar(@{$Cfrefs[$i]});
    @{$Cflbref[$i]}=();
    if($nel>0){
      for($j=0;$j<$nel;$j++){#<--file-loop inside given type
        $line=$Cfrefs[$i]->[$j];
        @string=unpack($CalfListFmt,$line);
        $line=pack($CalfLboxFmt,$string[0],$string[1],$string[3]);# utcmin excluded in listbox
        push(@{$Cflbref[$i]},$line);
      }
    }
    $JClboxPointer[$i]->delete(0,"end");
    $JClboxPointer[$i]->insert("end",@{$Cflbref[$i]});
  }
#
  $topl4->update;
  return;
#---
BadRet:
  show_messg("\n   <--- New RefCalFiles set was not created due to above problems !!!");
}
#---------------
sub ocontr_cleanup
{
#
#---> cleanup log-files list:
#
  my $nellfl=scalar(@LogFileList);
  my ($lfpid,$ccode,$strun,$time,$utcmin,$stat);
  my @rarray=();
  my $line,$nel;
  my ($strunc,$timec,$utcminc,$statc);
  my @rlistbox=();
  my @string=();
  my $ditm=0;
#
  show_messg("\n   <========= Enter Cleanup with $nellfl log-files in the list ...","B");
  $logf_lbx->selectionClear(0,'end');
  for($lf=0;$lf<$nellfl;$lf++){#<---logf-loop
    $line=$LogFileList[$lf];
    ($lfpid,$ccode,$strun,$time,$utcmin,$stat)=unpack($LogfListFmt,$line);
    if($stat<4 && $stat>0){#<-- delete because not-matched
      $stat=0;# mark log-file for killing
      $line=pack($LogfListFmt,$lfpid,$ccode,$strun,$time,$utcmin,$stat);# <- update stat in LogfList
      $LogFileList[$lf]=$line;
      show_messg("\n      <--- Job's $lfpid log-file will be deleted !!!","Y");
      @rarray=();
      $line=$SubmJobsList[$LogJobMatchIndex[$lf]];# <- update stat in SubmJobList
      @rarray=unpack($SubmJobsFmt,$line);
      $rarray[9]=0;# mark job for killing in SubmJobList as having bad logfile
      $SubmJobsList[$LogJobMatchIndex[$lf]]=pack($SubmJobsFmt,@rarray);
#      $logf_lbx->selectionSet($lf-$ditm);
#      $logf_lbx->delete($lf-$ditm);
      $ditm+=1;
    }
  }#--->endof logf-loop
#
#---> cleanup cal-files lists:
# 
  my @Cfrefs=();
  $Cfrefs[0]=\@CflistList;
  $Cfrefs[1]=\@CStatList;
  $Cfrefs[2]=\@TdelvList;
  $Cfrefs[3]=\@TzslwList;
  $Cfrefs[4]=\@AmplfList;
  $Cfrefs[5]=\@ElospList;
  $Cfrefs[6]=\@TdcorList;
#
  my @matCfrefs=();
  $matCfrefs[0]=\@MatCflistInd; 
  $matCfrefs[1]=\@MatCStatInd; 
  $matCfrefs[2]=\@MatTdelvInd; 
  $matCfrefs[3]=\@MatTzslwInd; 
  $matCfrefs[4]=\@MatAmplfInd; 
  $matCfrefs[5]=\@MatElospInd;
  $matCfrefs[6]=\@MatTdcorInd; 
#--
  my $nelcft=scalar(@cfilenames);
#
  $ditm=0; 
  for($i=0;$i<$nelcft;$i++){#<--cal.types loop(exept Tdcor/Elosp which are irrelevant of TAUC)
    if($i!=6){next;}
    $JClboxPointer[$i]->selectionClear(0,'end');
    $nel=scalar(@{$Cfrefs[$i]});
    for($j=0;$j<$nel;$j++){#<--- cal-files loop
      $line=$Cfrefs[$i]->[$j];
      ($strunc,$timec,$utcminc,$statc)=unpack($CalfListFmt,$line);
      if($statc<2){
        $statc=0;#<-- mark for killing
        $line=pack($CalfListFmt,$strunc,$timec,$utcminc,$statc);# update stat in cal-files list
        $Cfrefs[$i]->[$j]=$line;
      }
    }
  }
#
#--- update Log/CalFiles Lboxes to see new statuses:
#
  my @loglistbox=();
  my @string=();
  for($i=0;$i<$nellfl;$i++){#prepare list to view
    $line=$LogFileList[$i];
    @string=unpack($LogfListFmt,$LogFileList[$i]);
    $line=pack($LogfLboxFmt,$string[0],$string[1],$string[2],$string[3],$string[5]);# utcmin excluded in listbox
    push(@loglistbox,$line);
  }
  $logf_lbx->delete(0,"end");
  $logf_lbx->insert("end",@loglistbox);
#
  for($i=0;$i<$nelcft;$i++){#<-- cal-file types loop
    if($i!=6){next;}
    $nel=scalar(@{$Cfrefs[$i]});
    @{$Cflbref[$i]}=();
    if($nel>0){
      for($j=0;$j<$nel;$j++){#<--file-loop inside given type
        $line=$Cfrefs[$i]->[$j];
        @string=unpack($CalfListFmt,$line);
	if($i==6 && $string[3]!=10){$line=pack($CalfLboxFmt." A5",$string[0],$string[1],$string[3],$TdcorfExt[$j]);}# for Tdcor only
        else{$line=pack($CalfLboxFmt,$string[0],$string[1],$string[3]);}# utcmin excluded in listbox
        push(@{$Cflbref[$i]},$line);
      }
    }
    $JClboxPointer[$i]->delete(0,"end");
    $JClboxPointer[$i]->insert("end",@{$Cflbref[$i]});
  }
#
  $topl4->update;
#
}
#---------------
sub ocontr_editcl
{
}
#---------------
sub ocontr_finish
{
#---
  my @Cfrefs=();
  $Cfrefs[0]=\@CflistList;
  $Cfrefs[1]=\@CStatList;
  $Cfrefs[2]=\@TdelvList;
  $Cfrefs[3]=\@TzslwList;
  $Cfrefs[4]=\@AmplfList;
  $Cfrefs[5]=\@ElospList;
  $Cfrefs[6]=\@TdcorList;
#
  my @matCfrefs=();
  $matCfrefs[0]=\@MatCflistInd; 
  $matCfrefs[1]=\@MatCStatInd; 
  $matCfrefs[2]=\@MatTdelvInd; 
  $matCfrefs[3]=\@MatTzslwInd; 
  $matCfrefs[4]=\@MatAmplfInd; 
  $matCfrefs[5]=\@MatElospInd;
  $matCfrefs[6]=\@MatTdcorInd; 
#
  my $nellfl=scalar(@LogFileList);
  my ($lfpid,$ccode,$strun,$time,$utcmin,$stat);
  my @rarray=();
  my $line,$nel,$dir;
  my ($strunc,$timec,$utcminc,$statc);
  my @rlistbox=();
  my @string=();
  my $fn,$fne,$fnfr,$fnto,$rwsta;
  my $bext;
#
#---> move log-files(incl.bad) to store(/tdclfiles):
#
  for($lf=0;$lf<$nellfl;$lf++){#<---logf-loop
    $line=$LogFileList[$lf];
    ($lfpid,$ccode,$strun,$time,$utcmin,$stat)=unpack($LogfListFmt,$line);
    $fn="TofTDCL".$ccode.".".$strun.".log.".$lfpid;
    $fne="TofTDCL".$ccode.".".$strun.".loge.".$lfpid;
    $bext="";
    if($stat==0){#<-- move bad-job log-files renaming them with "bad" mark
      $bext=".bad";
    }
    $fnfr=$workdir.$amsjwd."/".$fn;
    $fnto=$workdir.$tdcsdir."/".$fn.$bext;
    $rwsta = system("mv $fnfr $fnto");
    if($rwsta != 0){show_warn("\n   <-- Can't move $fn to store !");}
    else{
      show_messg("\n   <--- Log-file $fn.$bext was moved to store !");
    }
      
    $fnfr=$workdir.$amsjwd."/".$fne;
    $fnto=$workdir.$tdcsdir."/".$fne.$bext;
    $rwsta = system("mv $fnfr $fnto");
    if($rwsta != 0){show_warn("   <-- Can't move $fne to store !");}
    else{
      show_messg("\n   <--- Loge-file $fne.$bext was moved to store !");
    }
  }#--->endof logf-loop
#
#---> move/delete cal-files:
# 
#--
  my $nelcft=scalar(@cfilenames);
# 
  for($i=0;$i<$nelcft;$i++){#<--cal.types loop(here only Tdcor-type)
    if($i!=6){next;}
    $nel=scalar(@{$Cfrefs[$i]});
    for($j=0;$j<$nel;$j++){#<--- cal-files loop
      $line=$Cfrefs[$i]->[$j];
      ($strunc,$timec,$utcminc,$statc)=unpack($CalfListFmt,$line);
      $fn=$cfilenames[$i].".".$strunc;
      if($statc==0){#<-- delete cal-file
        $fnfr=$workdir.$amsjwd."/".$fn.$TdcorfExt[$j];
        $rwsta = system("rm -f $fnfr");
        if($rwsta != 0){show_warn("\n   <-- Can't delete $fn in work-dir !");}
	else{show_messg("\n   <--- Cal-file $fn is deleted in work-dir !");}
      }
      elsif($statc!=10){#<-- move cal-file to store(exept ref-file)
        $fnfr=$workdir.$amsjwd."/".$fn.$TdcorfExt[$j];
        $fnto=$workdir.$tdcsdir."/".$fn;
#        move($fnfr,$fnto) or show_warn("\n   <-- Moving failed for $fn, $!");
        $rwsta = system("mv $fnfr $fnto");
        if($rwsta != 0){show_warn("\n   <-- Can't move $fn to store !");}
	else{show_messg("\n   <--- Cal-file $fn was moved to store !");}
      }
      elsif($statc==10){#<-- copy newly created ref.cal-file to store
        $fnfr=$workdir.$amsjwd."/".$fn;
        $fnto=$workdir.$tdcsdir."/".$fn;
        $rwsta = system("cp -f $fnfr $fnto");
        if($rwsta != 0){show_warn("\n   <-- Can't copy $fn to store !");}
	else{show_messg("\n   <--- New ref.cal-file $fn was copied to store !");}
      }
    }
  }
#
#---> save list of  jobs in file:
#
  my @array=();
  my @narray=();
  my ($sjpid,$crefn,$date);
  my $nelsjl=scalar(@SubmJobsList);
#
  for($j=0;$j<$nelsjl;$j++){#<--- SubmJobList loop
    $line=$SubmJobsList[$j];
    @array=unpack($SubmJobsFmt,$line);
    $sjpid=$array[0];
    $crefn=$array[3];
    $strun=$array[4];
    $ccode=$array[6];
    $stat=$array[9];# 0/1/5/10/20/30/40/100/->killed/submitted/pend/running/ended/haslogf/logfok/goodjob
      push(@narray,$line."\n");
  }
#-- save:
  $fn="TofSubmJobsList".".".$SessName;
  open(SJLIST,"> $fn") or show_warn("\n   <--- Cannot open $fn for update, $!");
  print SJLIST @narray;
  close(SJLIST) or show_warn("\n   <--- Cannot close $fn after updating !");
#---
  show_messg("\n   <--- Left JobOutputControl ...  !");
  $topl4->destroy() if Exists($topl4);
  $mwnd->update;
}
#------------------------------------------------------------------------------
sub WatchDog
{
#
#---> define size and create WatchDog status-window:
#
  $topl5=$mwnd->Toplevel();
  $wdwdszx=1-$mnwdszx-0.01;#X-size of window (portion of display x-size)
  $wdwdszy=$mnwdszy;#Y-size of window (portion of display y-size)
  my $szx=int($wdwdszx*$displx);#in pixels
  my $szy=int($wdwdszy*$disply);
  my $wdwdposy="+0";
  my $wdwdpx=int(($mnwdszx+0.005)*$displx);# wind. pos-x in pixels(0.03->safety)
  my $wdwdposx="+".$wdwdpx;
  my $wdwdgeom=$szx."x".$szy.$wdwdposx.$wdwdposy;
  print "WatchDog status-window geom-",$wdwdgeom,"\n";
  $topl5->geometry($wdwdgeom);
  $topl5->title("WDW");
#
  my $till=$UnFinishedJobs;
  my $blocks=$UnFinishedJobs;
#---
  $wdpr_fram1=$topl5->Frame(-background => "Cyan",  
                           -relief=>'groove', -borderwidth=>3)->place(
                           -relwidth=>0.5, -relheight=>1,
                           -relx=>0, -rely=>0);
#---
  $wdpr_lab1=$wdpr_fram1->Label(-text=>"UnfJobs",
                          -font=>$font2,-relief=>'groove')
                          ->place(
		                  -relwidth=>1, -relheight=>0.05,
                                  -relx=>0, -rely=>0);
#---
  $UnFinJs=$UnFinishedJobs;
  $wdpr_pb1=$wdpr_fram1->ProgressBar( -from=>0, -to=>$till, -blocks=>$blocks,
                                             -anchor=>'n',
                                             -colors=>[0,'red'], -gap=> 1,
                                             -variable=>\$UnFinJs)
					     ->place(
					     -relwidth=>1, -relheight=>0.95,
                                             -relx=>0, -rely=>0.05);
#-----
  $wdpr_fram2=$topl5->Frame(-background => "Cyan",  
                           -relief=>'groove', -borderwidth=>3)->place(
                           -relwidth=>0.5, -relheight=>1,
                           -relx=>0.5, -rely=>0);
#---
  $wdpr_lab2=$wdpr_fram2->Label(-text=>"FinJobs",
                          -font=>$font2,-relief=>'groove')
                          ->place(
		                  -relwidth=>1, -relheight=>0.05,
                                  -relx=>0, -rely=>0);
#---
  $FinJs=$FinishedJobs;
  $wdpr_pb2=$wdpr_fram2->ProgressBar( -from=>0, -to=>$till, -blocks=>$blocks,
                                             -anchor=>'n',
                                             -colors=>[0,'green'], -gap=> 1,
                                             -variable=>\$FinJs)
					     ->place(
					     -relwidth=>1, -relheight=>0.95,
                                             -relx=>0, -rely=>0.05);
#------
  my $period=600000;# 60 sec
  my $id;
  $topl5->repeat($period,\&Analyze);  
#
}
#---
sub Analyze
{
  JobOutpControl();#(incide incr.$FinishedJobs, return new $FiniJobCount,$JobConfLogFiles)
#  if($JOCReady == 0){$topl4->waitVariable(\$JOCReady);}#<-- just protection (may be stupid)
  if($FiniJobCount>0){$UnFinishedJobs-=$FiniJobCount;}
  $UnFinJs=$UnFinishedJobs;
  $FinJs=$FinishedJobs;
#  $wdpr_pb1->configure(-value => $UnFinJs);
#  $wdpr_pb2->configure(-value => $FinJs);
  $topl5->update();
  if($JobConfLogFiles>0){
    $matchf_bt->invoke();
    $topl4->after(2000);# tempor for debug (to see result)
    $cleanup_bt->invoke();
    $topl4->after(5000);# tempor for debug (to see result)
    $finish_bt->invoke();
    $topl4->after(2000);# tempor for debug
  }
  if($AutoSessFlg eq "AutoSess" && $UnFinishedJobs>0){
    show_messg("   <--- On duty: Unf/Fin=$UnFinishedJobs / $FinishedJobs","B");
    $topl4->after(1000);# tempor for debug
    $topl4->destroy() if Tk::Exists($topl4);
  }
  else{#<--- current session is completed
    show_messg("\n   <--- WatchDog: Job done, gone away... Unf/Fin=$UnFinishedJobs / $FinishedJobs","Big");
    JobOutpControl();#just to leave on screen latest status for few seconds  
    $topl5->after(2000);# tempor for debug (to see result)
    $topl4->destroy() if Tk::Exists($topl4);
    $topl5->destroy() if Tk::Exists($topl5);
    $setupj_state="normal";
    $setupj_bt->configure(-state=>$setupj_state);   
    $sjobbt_state="normal";
    $sjobbt->configure(-state=>$sjobbt_state);
#    QuitSession();#<-- tempor commented for debug
  }
#
  $mwnd->update;
}
#------------------------------------------------------------------------------
sub QuitSession
{
  my @array=();
  my @narray=();
  my $nelsjl=scalar(@SubmJobsList);
  my ($sjpid,$crefn,$ccode,$strun,$date,$time,$dtime,$stat,$cmd);
  my $fn,$status,$rwsta;
#
#---> save list of  jobs in file:
#
  my $nelsjl=scalar(@SubmJobsList);
#
  for($j=0;$j<$nelsjl;$j++){#<--- SubmJobList loop
    $line=$SubmJobsList[$j];
    @array=unpack($SubmJobsFmt,$line);
    $sjpid=$array[0];
    $crefn=$array[3];
    $strun=$array[4];
    $ccode=$array[6];
    $stat=$array[9];# 0/1/5/10/20/30/40/100/->killed/submitted/running/pend/ended/haslogf/logfok/goodjob
    if(($stat > 0) && ($stat < 20)){push(@narray,$line);}#save only unfinished jobs
  }
#-- save:
  $fn="TofSubmJobsList".".".$SessName;
  open(SJLIST,"> $fn") or show_warn("\n   <--- Cannot open $fn for update, $!");
    for($i=0;$i<$nelsjl;$i++){print SJLIST $narray[$i]."\n";}
  close(SJLIST) or show_warn("\n   <--- Cannot close $fn after updating, $!");
#
#---> Create/Move "screen" Log-file:
#
  my $logfname="Tof".$SessName."sessLog.";
    $filen=$logfname.$SessTLabel;#extention is session time label 
    $newfn=$workdir.$tdcsdir."/".$filen;
    open(OFN, "> $filen") or die show_warn("   <-- Cannot open $filen for writing !");
    print OFN $logtext->get("1.0","end");
    show_messg("\n   <-- LogFile $filen is saved !");
    $status=system("chmod 666 $filen");
    if($status != 0){
      print "Warning: problem with write-priv for logfile, status=",$status,"\n";
    }
    else{
      $rwsta = system("mv $filen $newfn");
      if($rwsta != 0){show_warn("\n   <-- Can't move $filen to store !");}
      else{show_messg("\n   <--- SessionLog $filen was moved to store !");}
    }
#------
  $mwnd->update;
  if($soundtext eq "Sound-ON"){$mwnd->bell;}
  $mwnd->after(6000);
  $dir_fram->destroy() if Exists($dir_fram);
  $set_fram->destroy() if Exists($set_fram);
  $jcl_fram->destroy() if Exists($jcl_fram);
  $topl3->destroy() if Exists($topl3);
  $topl4->destroy() if Exists($topl4);
  $logtext->delete("1.0",'end');
#--> enable CalType/Start-buttons:
  my $state="normal";
  for($i=0;$i<6;$i++){
    $CalTypButt[$i]->configure(-state=>$state);
  }
  $StartButt->configure(-state=>$state);
#
  $mwnd->update;
}
#------------------------------------------------------------------------------
1;



