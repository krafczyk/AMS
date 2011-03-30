sub UniJob
{
#---
my $dirfrheight=0.3;
my $setfrheight=0.62;
my $jclfrheight=0.08;

#--------------
#dir_frame:
my $shf1=0.1;#down shift for dir-widgets
my $drh1=(1-$shf1)/5;#height of dir-widgets
my ($bwid,$bheig,$xpos,$ypos,$lbwid);
#---
$dir_fram=$mwnd->Frame(-label => "General Info/Settings/Actions :",-background => "Grey",
                                                      -relief=>'groove', -borderwidth=>5)
						      ->place(
                                                      -relwidth=>(1-$LogfXsize), -relheight=>$dirfrheight,
                                                      -relx=>0, -rely=>0);
#------
$wrd_lab=$dir_fram->Label(-text=>"WorkD:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>0.17, -relheight=>$drh1,
                                                -relx=>0, -rely=>$shf1);
$wrd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$workdir)
					      ->place(
                                              -relwidth=>0.83, -relheight=>$drh1,  
                                              -relx=>0.17, -rely=>$shf1);
#------
$rootfd_lab=$dir_fram->Label(-text=>"RootfD:",-font=>$LablWFont,-relief=>'groove')
                                                    ->place(
						    -relwidth=>0.17, -relheight=>$drh1,
                                                    -relx=>0, -rely=>($shf1+$drh1));
$rootfd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$EntrWFont,
                                               -textvariable=>\$RootfDir)
					       ->place(
                                               -relwidth=>0.43, -relheight=>$drh1,  
                                               -relx=>0.17, -rely=>($shf1+$drh1));
#---
$rootfsd_lab=$dir_fram->Label(-text=>"SubD:",-font=>$LablWFont,-relief=>'groove')
                                                    ->place(
						    -relwidth=>0.17, -relheight=>$drh1,
                                                    -relx=>0.6, -rely=>($shf1+$drh1));
$rootfsd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$EntrWFont,
                                               -textvariable=>\$RootfSubD)->place(
                                               -relwidth=>0.23, -relheight=>$drh1,  
                                               -relx=>0.77, -rely=>($shf1+$drh1));
#------
$mcdaqfd_lab=$dir_fram->Label(-text=>"MCDaqfD:",-font=>$LablWFont,-relief=>'groove')
                                                    ->place(
						    -relwidth=>0.17, -relheight=>$drh1,
                                                    -relx=>0, -rely=>($shf1+2*$drh1));
$mcdaqfd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$EntrWFont,
                                               -textvariable=>\$MCDaqfDir)
					       ->place(
                                               -relwidth=>0.43, -relheight=>$drh1,  
                                               -relx=>0.17, -rely=>($shf1+2*$drh1));
#---
$mcdaqfsd_lab=$dir_fram->Label(-text=>"SubD:",-font=>$LablWFont,-relief=>'groove')
                                                    ->place(
						    -relwidth=>0.17, -relheight=>$drh1,
                                                    -relx=>0.6, -rely=>($shf1+2*$drh1));
$mcdaqfsd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$EntrWFont,
                                               -textvariable=>\$MCDaqfSubD)->place(
                                               -relwidth=>0.23, -relheight=>$drh1,  
                                               -relx=>0.77, -rely=>($shf1+2*$drh1));
#--------------
$amsg_lab=$dir_fram->Label(-text=>"OfflD:",-font=>$LablWFont,-relief=>'groove')
                                                         ->place(
                                                         -relwidth=>0.17, -relheight=>$drh1,
                                                         -relx=>0, -rely=>($shf1+3*$drh1));
$amsg_ent=$dir_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$OFFLDIR)
					      ->place(
                                              -relwidth=>0.83, -relheight=>$drh1,  
                                              -relx=>0.17, -rely=>($shf1+3*$drh1));
#-------------
$bwid=0.15;
$xpos=0;
$soundtext="SoundON";
$dir_fram->Button(-text=>"SoundON", -font=>$ButtWFont, 
                                     -activebackground=>"yellow",
			             -activeforeground=>"red",
			             -foreground=>"red",
			             -background=>"yellow",
                                     -borderwidth=>3,-relief=>'raised',
			             -cursor=>hand2,
			             -textvariable=>\$soundtext,
                                     -command => sub{if($soundtext eq "SoundON"){$soundtext="SoundOFF";}
			                             else {$soundtext="SoundON";}})
			             ->place(
                                     -relwidth=>$bwid, -relheight=>$drh1,
				     -relx=>$xpos, -rely=>($shf1+4*$drh1));
$xpos+=$bwid;
#---
$bwid=0.17;
$lookar_bt=$dir_fram->Button(-text=>"LookAround", -font=>$ButtWFont, 
                                         -activebackground=>"yellow",
			                 -activeforeground=>"red",
			                 -foreground=>"red",
			                 -background=>"green",
                                         -borderwidth=>3,-relief=>'raised',
			                 -cursor=>hand2,
                                         -command => \&LookAround)
			                 ->place(
                                         -relwidth=>$bwid, -relheight=>$drh1,  
                                         -relx=>$xpos, -rely=>($shf1+4*$drh1));
$xpos+=$bwid;
#---
$clrhist_bt=$dir_fram->Button(-text=>"ResetHistory", -font=>$ButtWFont, 
                                         -activebackground=>"yellow",
			                 -activeforeground=>"red",
			                 -foreground=>"yellow",
			                 -background=>"red",
                                         -borderwidth=>3,-relief=>'raised',
			                 -cursor=>hand2,
                                         -command => \&ResetHistory)
			                 ->place(
                                         -relwidth=>$bwid, -relheight=>$drh1,  
                                         -relx=>$xpos, -rely=>($shf1+4*$drh1));
$xpos+=$bwid;
#---
$lbwid=0.15;
$dir_fram->Label(-text=>"DCDefFile:",-font=>$LablWFont,-relief=>'groove')
                                                    ->place(
						    -relwidth=>$lbwid, -relheight=>$drh1,
                                                    -relx=>$xpos, -rely=>($shf1+4*$drh1));
$xpos+=$lbwid;
#---
$bwid=0.11;
$lookar_bt=$dir_fram->Button(-text=>"Restore", -font=>$ButtWFont, 
                                         -activebackground=>"yellow",
			                 -activeforeground=>"red",
			                 -foreground=>"red",
			                 -background=>"green",
                                         -borderwidth=>3,-relief=>'raised',
			                 -cursor=>hand2,
                                         -command => \&RestoreDC)
			                 ->place(
                                         -relwidth=>$bwid, -relheight=>$drh1,  
                                         -relx=>$xpos, -rely=>($shf1+4*$drh1));
$xpos+=$bwid;
#---
$lookar_bt=$dir_fram->Button(-text=>"Commit", -font=>$ButtWFont, 
                                         -activebackground=>"yellow",
			                 -activeforeground=>"red",
			                 -foreground=>"red",
			                 -background=>"green",
                                         -borderwidth=>3,-relief=>'raised',
			                 -cursor=>hand2,
                                         -command => \&CommitDC)
			                 ->place(
                                         -relwidth=>$bwid, -relheight=>$drh1,  
                                         -relx=>$xpos, -rely=>($shf1+4*$drh1));
$xpos+=$bwid;
#---
$bwid=0.14;
$newdcf_bt=$dir_fram->Button(-text=>"MakeNew", -font=>$ButtWFont, 
                                         -activebackground=>"yellow",
			                 -activeforeground=>"red",
			                 -foreground=>"red",
			                 -background=>"orange",
                                         -borderwidth=>3,-relief=>'raised',
			                 -cursor=>hand2,
                                         -command => \&CreateNewDCFile)
			                 ->place(
                                         -relwidth=>$bwid, -relheight=>$drh1,  
                                         -relx=>$xpos, -rely=>($shf1+4*$drh1));
$xpos+=$bwid;
$newdcf_bt->bind("<ButtonRelease-3>", \&joutcontr_help);
#--------------------------------------------------------------------------
#run-conditions_set_frame:
my $shf2=0.06;#down shift for runcond-widgets
my $drh2=0.085;#height of butt/entr-widgets
my $xpos;
# my $drh2=(1-$shf2)/10;#height of runcond-widget
$sdframpos=$shf2+2*$drh2;
$sdframhig=4*$drh2;
my $shf3=$sdframpos+$sdframhig;
@P2DCGroupBut=();
@DCGroupEntry=();
@DCGrNames=qw(AMS TOF ACC EMC LVL1 TRK TRD RICH ZFit LVL3 IO SelEv MCgn);
#---
$set_fram=$mwnd->Frame(-label=>"Change Data Cards and Setup New Job :",-relief=>'groove', 
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>(1-$LogfXsize), -relheight=>$setfrheight,
                                                  -relx=>0, -rely=>$dirfrheight);
#------
if($SessName eq "RDR" || $SessName eq "MCR"){$RecoSimuP="RECO";}
else {$RecoSimuP="SIMU";}#just set default page(reco or simu params types) to start with...
$PageNumb="Page1";
#
$set_fram->Button(-text=>$RecoSimuP, -font=>$ButtWFont, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
#			      -foreground=>"red",
			      -background=>"yellow",
                              -borderwidth=>3,-relief=>'raised',
			      -cursor=>hand2,
			      -textvariable=>\$RecoSimuP,
			      -command =>\&ChangeRecSim)
#                              -command => sub{if($RecoSimuP eq "RECO" && $SessName eq "MCS"){$RecoSimuP="SIMU";}
#			                      else {$RecoSimuP="RECO";}})
			         ->place(
                                 -relwidth=>0.12, -relheight=>$drh2,
                                 -relx=>0, -rely=>$shf2);
#---
$set_fram->Button(-text=>$PageNumb, -font=>$ButtWFont, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
#			      -foreground=>"red",
			      -background=>"yellow",
                              -borderwidth=>3,-relief=>'raised',
			      -cursor=>hand2,
			      -textvariable=>\$PageNumb,
			      -command =>\&ChangePage3)
			         ->place(
                                 -relwidth=>0.1, -relheight=>$drh2,
                                 -relx=>0.12, -rely=>$shf2);
#---
$set_fram->Label(-text=>"for:",-font=>$LablWFont,-relief=>'groove')
                                                  ->place(
						  -relwidth=>0.08, -relheight=>$drh2,
                                                  -relx=>0.22, -rely=>$shf2);
#---
$P2DCGroupBut[0]=$set_fram->Radiobutton(-text=>$DCGrNames[0],-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -command=>\&SetAMSPars,
                                                 -value=>$DCGrNames[0], -variable=>\$DataGroup)
                                              ->place(
						      -relwidth=>0.14, -relheight=>$drh2,
						      -relx=>0.3, -rely=>$shf2);
#---
$P2DCGroupBut[1]=$set_fram->Radiobutton(-text=>$DCGrNames[1],-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -command=>\&SetTofPars,
                                                 -value=>$DCGrNames[1], -variable=>\$DataGroup)
                                              ->place(
						      -relwidth=>0.14, -relheight=>$drh2,
						      -relx=>0.44, -rely=>$shf2);
#---
$P2DCGroupBut[2]=$set_fram->Radiobutton(-text=>$DCGrNames[2],-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -command=>\&SetAccPars,
                                                 -value=>$DCGrNames[2], -variable=>\$DataGroup)
                                              ->place(
						      -relwidth=>0.14, -relheight=>$drh2,
						      -relx=>0.58, -rely=>$shf2);
#---
$P2DCGroupBut[3]=$set_fram->Radiobutton(-text=>$DCGrNames[3],-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -command=>\&SetEmcPars,
                                                 -value=>$DCGrNames[3], -variable=>\$DataGroup)
                                              ->place(
						      -relwidth=>0.14, -relheight=>$drh2,
						      -relx=>0.72, -rely=>$shf2);
#---
$P2DCGroupBut[4]=$set_fram->Radiobutton(-text=>$DCGrNames[4],-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -command=>\&SetLvl1Pars,
                                                 -value=>$DCGrNames[4], -variable=>\$DataGroup)
                                              ->place(
						      -relwidth=>0.14, -relheight=>$drh2,
						      -relx=>0.86, -rely=>$shf2);
#------
my $wid2=1/8;
$P2DCGroupBut[5]=$set_fram->Radiobutton(-text=>$DCGrNames[5],-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -command=>\&SetTrkPars,
                                                 -value=>$DCGrNames[5], -variable=>\$DataGroup)
                                              ->place(
						      -relwidth=>$wid2, -relheight=>$drh2,
						      -relx=>0, -rely=>($shf2+$drh2));
#---
$P2DCGroupBut[6]=$set_fram->Radiobutton(-text=>$DCGrNames[6],-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -command=>\&SetTrdPars,
                                                 -value=>$DCGrNames[6], -variable=>\$DataGroup)
                                              ->place(
						      -relwidth=>$wid2, -relheight=>$drh2,
						      -relx=>$wid2, -rely=>($shf2+$drh2));
#---
$P2DCGroupBut[7]=$set_fram->Radiobutton(-text=>$DCGrNames[7],-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -command=>\&SetRichPars,
                                                 -value=>$DCGrNames[7], -variable=>\$DataGroup)
                                              ->place(
						      -relwidth=>$wid2, -relheight=>$drh2,
						      -relx=>(2*$wid2), -rely=>($shf2+$drh2));
#---
$P2DCGroupBut[8]=$set_fram->Radiobutton(-text=>$DCGrNames[8],-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -command=>\&SetZFitPars,
                                                 -value=>$DCGrNames[8], -variable=>\$DataGroup)
                                              ->place(
						      -relwidth=>$wid2, -relheight=>$drh2,
						      -relx=>(3*$wid2), -rely=>($shf2+$drh2));
#---
$P2DCGroupBut[9]=$set_fram->Radiobutton(-text=>$DCGrNames[9],-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -command=>\&SetLvl3Pars,
                                                 -value=>$DCGrNames[9], -variable=>\$DataGroup)
                                              ->place(
						      -relwidth=>$wid2, -relheight=>$drh2,
						      -relx=>(4*$wid2), -rely=>($shf2+$drh2));
#---
$P2DCGroupBut[10]=$set_fram->Radiobutton(-text=>$DCGrNames[10],-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -command=>\&SetIOPars,
                                                 -value=>$DCGrNames[10], -variable=>\$DataGroup)
                                              ->place(
						      -relwidth=>$wid2, -relheight=>$drh2,
						      -relx=>(5*$wid2), -rely=>($shf2+$drh2));
#---
$P2DCGroupBut[11]=$set_fram->Radiobutton(-text=>$DCGrNames[11],-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -command=>\&SetEvSelPars,
                                                 -value=>$DCGrNames[11], -variable=>\$DataGroup)
                                              ->place(
						      -relwidth=>$wid2, -relheight=>$drh2,
						      -relx=>(6*$wid2), -rely=>($shf2+$drh2));
#---
$P2DCGroupBut[12]=$set_fram->Radiobutton(-text=>$DCGrNames[12],-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -command=>\&SetMCGenPars,
                                                 -value=>$DCGrNames[12], -variable=>\$DataGroup)
                                              ->place(
						      -relwidth=>$wid2, -relheight=>$drh2,
						      -relx=>(7*$wid2), -rely=>($shf2+$drh2));
#------------------------------
$set_fram->Button(-text=>"ConfirmPars", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ConfirmPars)
			                  ->place(
                                          -relwidth=>0.2, -relheight=>$drh2,  
                                          -relx=>0, -rely=>$shf3);
#---
$CrJScr_b=$set_fram->Button(-text=>"CreateJScript", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
			                  -state=>"disabled",
                                          -command => \&CreateJobScript)
			                  ->place(
                                          -relwidth=>0.2, -relheight=>$drh2,  
                                          -relx=>0.2, -rely=>$shf3);
#---
$set_fram->Button(-text=>"EditJScript", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&EditJobScript)
			                  ->place(
                                          -relwidth=>0.2, -relheight=>$drh2,  
                                          -relx=>0.4, -rely=>$shf3);
#---
$BatchFlg="BatchJob";
$Evs2Read="10000000";#number of events to read 
$batchj_bt=$set_fram->Button(-text=>"BatchJob", -font=>$ButtWFont, 
                                     -activebackground=>"yellow",
			             -activeforeground=>"red",
			             -background=>"yellow",
                                     -borderwidth=>3,-relief=>'raised',
			             -cursor=>hand2,
			             -textvariable=>\$BatchFlg,
                                     -command => sub{
				                     if($BatchFlg eq "BatchJob"){
						       $BatchFlg="InterJob";
						       $Evs2Read=100;
						     }
			                             else {
						       $BatchFlg="BatchJob";
						       $Evs2Read=10000000;
						       }
						     })
			             ->place(
                                     -relwidth=>0.2, -relheight=>$drh2,
				     -relx=>0.6, -rely=>$shf3);
#---
$MultiSubmFlg="SingleSubm";
$manauto_bt=$set_fram->Button(-text=>"SingleSubm", -font=>$ButtWFont, 
                                     -activebackground=>"yellow",
			             -activeforeground=>"red",
			             -background=>"yellow",
                                     -borderwidth=>3,-relief=>'raised',
			             -cursor=>hand2,
			             -textvariable=>\$MultiSubmFlg,
                                     -command => sub{if($MultiSubmFlg eq "SingleSubm"){$MultiSubmFlg="MultiSubm";}
			                             else {$MultiSubmFlg="SingleSubm";}})
			             ->place(
                                     -relwidth=>0.2, -relheight=>$drh2,
				     -relx=>0.8, -rely=>$shf3);
#------
$dat1_lab=$set_fram->Label(-text=>"Date",-font=>$LablWFont,-relief=>'groove')
                                         ->place(
					 -relwidth=>0.12, -relheight=>$drh2,
                                         -relx=>0, -rely=>($shf3+$drh2));
$fdat1="2008.01.01 00:00:01";#def. file-date-from 
$fdat1_ent=$set_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$EntrWFont,
                                               -textvariable=>\$fdat1)
					       ->place(
                                               -relwidth=>0.25, -relheight=>$drh2,  
                                               -relx=>0.12, -rely=>($shf3+$drh2));
#---
$dat2_lab=$set_fram->Label(-text=>"-",-font=>$LablWFont)
                                      ->place(
				      -relwidth=>0.02, -relheight=>$drh2,
                                      -relx=>0.37, -rely=>($shf3+$drh2));
$fdat2="2015.01.01 00:00:01";#def. file-date-till 
$fdat2_ent=$set_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$EntrWFont,
                                               -textvariable=>\$fdat2)
					       ->place(
                                               -relwidth=>0.25, -relheight=>$drh2,  
                                               -relx=>0.39, -rely=>($shf3+$drh2));
#---
$editrl_bt=$set_fram->Button(-text=>"EditRunList(AftScan)", -font=>$ButtWFont, 
                                              -activebackground=>"yellow",
			                      -activeforeground=>"red",
			                      -foreground=>"red",
			                      -background=>"green",
                                              -borderwidth=>3,-relief=>'raised',
					      -state=>'disabled',
			                      -cursor=>hand2,
                                              -command =>\&edit_rlist)
					      ->place(
                                              -relwidth=>0.36, -relheight=>$drh2,
				              -relx=>0.64, -rely=>($shf3+$drh2));
#---------
$num1_lab=$set_fram->Label(-text=>"RunN",-font=>$LablWFont,-relief=>'groove')
                                         ->place(
					 -relwidth=>0.12, -relheight=>$drh2,
                                         -relx=>0, -rely=>($shf3+2*$drh2));
$fnum1="1167606001";#def. file-number-from 
$fnum1_ent=$set_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$EntrWFont,
                                               -textvariable=>\$fnum1)->place(
                                               -relwidth=>0.2, -relheight=>$drh2,  
                                               -relx=>0.12, -rely=>($shf3+2*$drh2));
#---
$num2_lab=$set_fram->Label(-text=>"-",-font=>$LablWFont)
                                     ->place(
				     -relwidth=>0.02, -relheight=>$drh2,
				     -relx=>0.32, -rely=>($shf3+2*$drh2));
$fnum2="1500000000";#def. file-num-till 
$fnum2_ent=$set_fram->Entry(-relief=>'sunken', -background=>yellow,
                                               -font=>$EntrWFont,
                                               -textvariable=>\$fnum2)
					       ->place(
                                               -relwidth=>0.18, -relheight=>$drh2,  
                                               -relx=>0.34, -rely=>($shf3+2*$drh2));
#---
$convert_bt=$set_fram->Button(-text=>"Confirm Run/Date", -font=>$ButtWFont, 
                                      -activebackground=>"yellow",
			              -activeforeground=>"red",
			              -foreground=>"red",
			              -background=>"green",
                                      -borderwidth=>3,-relief=>'raised',
				      -state=>'disabled',
			              -cursor=>hand2,
                                      -command => \&RunDateConv)
			              ->place(
                                      -relwidth=>0.26, -relheight=>$drh2,
				      -relx=>0.52, -rely=>($shf3+2*$drh2));
#---
$dfscan_bt=$set_fram->Button(-text => "ScanDaqfDir", -font=>$ButtWFont, 
                                                 -activebackground=>"yellow",
			                         -activeforeground=>"red",
			                         -foreground=>"red",
			                         -background=>"green",
                                                 -borderwidth=>3,-relief=>'raised',
			                         -cursor=>hand2,
			                         -state=>'disabled',
                                                 -command => \&scanddir)
						 ->place(
                                                 -relwidth=>0.22,-relheight=>$drh2,
                                                 -relx=>0.78,-rely=>($shf3+2*$drh2));
#----------
$set_fram->Label(-text=>"Evs/job:",-font=>$LablWFont,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.17, -relheight=>$drh2,
                                               -relx=>0, -rely=>($shf3+3*$drh2));
$set_fram->Entry(-relief=>'sunken', -background=>yellow,
                                    -font=>$EntrWFont,
                                    -textvariable=>\$Evs2Read)
				    ->place(
                                    -relwidth=>0.16, -relheight=>$drh2,  
                                    -relx=>0.17, -rely=>($shf3+3*$drh2));
#---
$set_fram->Label(-text=>"Queue",-font=>$LablWFont,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.13, -relheight=>$drh2,
                                               -relx=>0.33, -rely=>($shf3+3*$drh2));
#-
$Queue2run="1nd";#queue name the job sould be submitted 
$set_fram->Optionmenu(-textvariable => \$Queue2run, 
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [@AMSQueuesList],
	                       )
                               ->place(
                               -relwidth=>0.1, -relheight=>$drh2,  
                               -relx=>0.46, -rely=>($shf3+3*$drh2));
#---
$set_fram->Label(-text=>"Host",-font=>$LablWFont,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.11, -relheight=>$drh2,
                                               -relx=>0.56, -rely=>($shf3+3*$drh2));
#-
$Host2run="Any";
$set_fram->Optionmenu(-textvariable => \$Host2run, 
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [@AMSHostsList],
	                       )
                               ->place(
                               -relwidth=>0.15, -relheight=>$drh2,  
                               -relx=>0.67, -rely=>($shf3+3*$drh2));
#---
$set_fram->Label(-text=>"CPUs",-font=>$LablWFont,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.12, -relheight=>$drh2,
                                               -relx=>0.82, -rely=>($shf3+3*$drh2));
$Ncpus=1;# Ncpu 
$set_fram->Entry(-relief=>'sunken', -background=>yellow,
                                    -font=>$EntrWFont,
                                    -textvariable=>\$Ncpus)
				    ->place(
                                    -relwidth=>0.06, -relheight=>$drh2,  
                                    -relx=>0.94, -rely=>($shf3+3*$drh2));
#---------
$set_fram->Label(-text=>"SetupName:",-font=>$LablWFont,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.2, -relheight=>$drh2,
                                               -relx=>0, -rely=>($shf3+4*$drh2));
#-
$set_fram->Optionmenu(-textvariable => \$SetupName, 
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [@AMSSetupsList],
	                       )
                               ->place(
                               -relwidth=>0.3, -relheight=>$drh2,  
                               -relx=>0.2, -rely=>($shf3+4*$drh2));
#---
$setupj_state="disabled";
$setupj_bt=$set_fram->Button(-text=>"SetupJob", -font=>$ButtWFont, 
                                   -activebackground=>"yellow",
			           -activeforeground=>"red",
			           -foreground=>"red",
			           -background=>"orange",
                                   -borderwidth=>3,-relief=>'raised',
			           -cursor=>hand2,
			           -state=>$setupj_state,
                                   -command => \&SetupJob)
			           ->place(
                                   -relwidth=>0.25, -relheight=>$drh2,  
                                   -relx=>0.5, -rely=>($shf3+4*$drh2));
$setupj_bt->bind("<Button-3>", \&setupjob_help);
#---
$sjobbt_state="disabled";
$sjobbt=$set_fram->Button(-text => "SubmitJob", -font=>$ButtWFont, 
                                                -activebackground=>"yellow",
			                        -activeforeground=>"red",
			                        -background=>"green",
			                        -foreground=>"red",
                                                -borderwidth=>3,-relief=>'raised',
			                        -cursor=>hand2,
						-state=>$sjobbt_state,
                                                -command => \&SubmitJob)
						->place(
                                                -relwidth=>0.25,-relheight=>$drh2,
                                                -relx=>0.75,-rely=>($shf3+4*$drh2));
$sjobbt->bind("<Button-3>", \&submitjob_help);
#----------------
#job_control frame:
$jcl_fram=$mwnd->Frame(-label=>"Session control :",-background => "red",
                                                       -relief=>'groove', -borderwidth=>3)
						       ->place(
                                                       -relwidth=>(1-$LogfXsize), -relheight=>$jclfrheight,
                                                       -relx=>0, -rely=>($dirfrheight+$setfrheight));
#---
$jstatbt=$jcl_fram->Button(-text => "CheckJobsStat", -font=>$ButtWFont, 
                                                  -activebackground=>"yellow",
			                          -activeforeground=>"red",
			                          -background=>"green",
			                          -foreground=>"red",
                                                  -borderwidth=>3,-relief=>'raised',
			                          -cursor=>hand2,
                                                  -command => \&CheckJobsStat)
						  ->place(
                                                  -relx=>0,-rely=>0.35,
                                                  -relwidth=>0.333,-relheight=>0.65);
$jstatbt->bind("<Button-3>", \&jobstat_help);
#---
$joboc_bt=$jcl_fram->Button(-text => "JobOutpControl", -font=>$ButtWFont,
                                                 -activebackground=>"yellow",
			                         -activeforeground=>"red",
			                         -background=>"green",
			                         -foreground=>"red",
                                                 -borderwidth=>3,-relief=>'raised',
			                         -cursor=>hand2,
                                                 -command => \&JobOutpControl)
						 ->place(
                                                 -relx=>0.333,-rely=>0.35,
                                                 -relwidth=>0.333,-relheight=>0.65);
$joboc_bt->bind("<ButtonRelease-3>", \&joutcontr_help);
#---
$quitbt=$jcl_fram->Button(-text => "QuitSession", -font=>$ButtWFont,
                                                 -activebackground=>"yellow",
			                         -activeforeground=>"red",
			                         -background=>"green",
			                         -foreground=>"red",
                                                 -borderwidth=>3,-relief=>'raised',
			                         -cursor=>hand2,
                                                 -command => \&QuitSession)
						 ->place(
                                                 -relx=>0.666,-rely=>0.35,
                                                 -relwidth=>0.333,-relheight=>0.65);
$quitbt->bind("<ButtonRelease-3>", \&quitbt_help);
#
}  
#--------------------------------------------------------------
sub ChangePage2
{
  my ($i,$state);
  if($PageNumb eq "Page1"){$PageNumb="Page2";}
  else{$PageNumb="Page1";}
  for($i=0;$i<13;$i++){
    if($DataGroup eq $DCGrNames[$i]){
      $P2DCGroupBut[$i]->invoke();
      last;
    }
  }
}
#---
sub ChangePage3
{
  my ($i,$state);
  if($PageNumb eq "Page1"){$PageNumb="Page2";}
  elsif($PageNumb eq "Page2"){$PageNumb="Page3";}
  else{$PageNumb="Page1";}
  for($i=0;$i<13;$i++){
    if($DataGroup eq $DCGrNames[$i]){
      $P2DCGroupBut[$i]->invoke();
      last;
    }
  }
}
#------
sub ChangeRecSim{
  my ($i,$state);
  if($RecoSimuP eq "RECO" && $SessName eq "MCS"){$RecoSimuP="SIMU";}
  else{$RecoSimuP="RECO";}
  for($i=0;$i<13;$i++){
    if($DataGroup eq $DCGrNames[$i]){
      $P2DCGroupBut[$i]->invoke();
      last;
    }
  }
}
#---------------------
sub RestoreDC
{
  my ($rwsta,$fn,$fr,$to);
  $fn="DCDefPars.txt";
  $to=$workdir."/".$fn;
  $fr=$AMSCVS."/ut/jsubmitter/".$fn;
  $rwsta = system("cp $fr $to");
  if($rwsta != 0){show_warn("\n   <-- Can't copy DCDefPars-file from local CVS to workdir !");}
  else{show_warn("\n   <--- Workdir DCDefPars-file was restored(copied from local CVS-store !");}
#
#--- Read again DataCards def.settings file for local workdir:
#
  ReadDCDefFile();
}
#---
sub CommitDC
{
  my ($rwsta,$fn,$fr,$to);
  $fn="DCDefPars.txt";
  $fr=$workdir."/".$fn;
  $to=$AMSCVS."/ut/jsubmitter/".$fn;
  $rwsta = system("cp $fr $to");
  if($rwsta != 0){show_warn("\n   <-- Can't copy DCDefPars-file from workdir to local CVS !");}
  else{show_warn("\n   <--- DCDefPars-file was commited(copied to local CVS-store !");}
}
#-------
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
    $fn=$workdir."/"."SubmJobsList".".".$SessName;
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
#
#----------------------Welcome/LookAroundArea------------------------------
sub Welcome{
  my $i,$j;
  $ResetHistFlg=0;
  $PrevSessUTC=0;
  $PrevSessTime=0;
  my ($sec,$min,$hour,$day,$month,$year,$ptime,$syear);
  my ($line,$status,$fn,$nel,$fnn);
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
  $line="       ---------- WELCOME to AMS-jobs Submitter !!! ----------\n\n";
  $logtext->insert('end',$line,'BigAttention');
  $logtext->yview('end');
  show_messg("   <--- Session starts at : ".$SessTime."\n");
#---
  $prevstfn="PrevSessTime".".".$SessName;
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
#
  show_messg("   <--- DaqFiles Directory $DaqDataDir","Big");   
#
#--- Read DataCards def.settings file:
#
  ReadDCDefFile();
#---
#
#--- Prepare the lists of RefCflist-files (RD&MC):
#
    $fnn="TofRefCflistList".".RDR";
    $fn=$workdir.$amsjwd."/".$fnn;
    open(CFLIST,"< $fn") or show_warn("   <-- Cannot open $fnn for reading $!");
    while(defined ($line = <CFLIST>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@TofRefCflistListRD,$line);
    }
    close(CFLIST) or show_warn("   <-- Cannot close $fnn after reading $!");
    $nel=scalar(@TofRefCflistListRD);
    show_messg("\n   <--- Found $nel TofRefCflistRD-files available !!!");
#
    $fnn="AccRefCflistList".".RDR";
    $fn=$workdir.$amsjwd."/".$fnn;
    open(CFLIST,"< $fn") or show_warn("   <-- Cannot open $fnn for reading $!");
    while(defined ($line = <CFLIST>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@AccRefCflistListRD,$line);
    }
    close(CFLIST) or show_warn("   <-- Cannot close $fnn after reading $!");
    $nel=scalar(@AccRefCflistListRD);
    show_messg("   <--- Found $nel AccRefCflistRD-files available !!!");
#
    $fnn="EmcRefCflistList".".RDR";
    $fn=$workdir.$amsjwd."/".$fnn;
    open(CFLIST,"< $fn") or show_warn("   <-- Cannot open $fnn for reading $!");
    while(defined ($line = <CFLIST>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@EmcRefCflistListRD,$line);
    }
    close(CFLIST) or show_warn("   <-- Cannot close $fnn after reading $!");
    $nel=scalar(@EmcRefCflistListRD);
    show_messg("   <--- Found $nel EmcRefCflistRD-files available !!!");
#
    $fnn="TofRefCflistList".".MCS";
    $fn=$workdir.$amsjwd."/".$fnn;
    open(CFLIST,"< $fn") or show_warn("   <-- Cannot open $fnn for reading $!");
    while(defined ($line = <CFLIST>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@TofRefCflistListMC,$line);
    }
    close(CFLIST) or show_warn("   <-- Cannot close $fnn after reading $!");
    $nel=scalar(@TofRefCflistListMC);
    show_messg("   <--- Found $nel TofRefCflistMC-files available !!!");
#
    $fnn="AccRefCflistList".".MCS";
    $fn=$workdir.$amsjwd."/".$fnn;
    open(CFLIST,"< $fn") or show_warn("   <-- Cannot open $fnn for reading $!");
    while(defined ($line = <CFLIST>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@AccRefCflistListMC,$line);
    }
    close(CFLIST) or show_warn("   <-- Cannot close $fnn after reading $!");
    $nel=scalar(@AccRefCflistListMC);
    show_messg("   <--- Found $nel AccRefCflistMC-files available !!!");
#
    $fnn="EmcRefCflistList".".MCS";
    $fn=$workdir.$amsjwd."/".$fnn;
    open(CFLIST,"< $fn") or show_warn("   <-- Cannot open $fnn for reading $!");
    while(defined ($line = <CFLIST>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@EmcRefCflistListMC,$line);
    }
    close(CFLIST) or show_warn("   <-- Cannot close $fnn after reading $!");
    $nel=scalar(@EmcRefCflistListMC);
    show_messg("   <--- Found $nel EmcRefCflistMC-files available !!!");
#
#---------------> set some defaults (which, normally, should be redefined by DCset and "ConfirmPars" procedures):
#
  $TFREcmval[12]=$TofRefCflistListRD[0];
  $TFMCcmval[4]=$TofRefCflistListMC[0];
  $ATREcmval[5]=$AccRefCflistListRD[0];
  $ATMCcmval[3]=$AccRefCflistListMC[0];
  $ECREcmval[5]=$EmcRefCflistListRD[0];
  $ECMCcmval[7]=$EmcRefCflistListMC[0];
#---
  @NPartNames=qw(Any 1 2 3 >0 >1);
  $NPartName=$NPartNames[0];
#---
  @DetInDaq=(1,1,1,1,1,1);#def: full set of subdets, pattern:(Lvl Emc Ric T+A Trd Trk)
#---
  @TofDBUsePatt=();
  $j=1;
  for($i=0;$i<5;$i++){
    $TofDBUsePatt[$i]=1-(($TFREcmval[11]/$j)%10);# 10-base BitPattern Msb(tdcl elos dcrd ped cal)Lsb (=1 to use)
    $j=10*$j;
  }
#
  @MCTofDBUsePatt=();
  $j=1;
  for($i=0;$i<4;$i++){
    $MCTofDBUsePatt[$i]=1-(($TFMCcmval[3]/$j)%10);# 10-base BitPattern Msb(TlSd Peds Tmap CalSd)Lsb (=1 to use)
    $j=10*$j;
  }
#-
  @AccDBUsePatt=();
  $j=1;
  for($i=0;$i<3;$i++){
    $AccDBUsePatt[$i]=1-(($ATREcmval[4]/$j)%10);# 10-base BitPattern Msb(Peds VarP StabP)Lsb (=1 to use)
    $j=10*$j;
  }
#
  @MCAccDBUsePatt=();
  $j=1;
  for($i=0;$i<2;$i++){
    $MCAccDBUsePatt[$i]=1-(($ATMCcmval[2]/$j)%10);# 10-base BitPattern Msb(SDped MCped)Lsb (=1 to use)
    $j=10*$j;
  }
#-
  @EcDBUsePatt=();
  $j=1;
  for($i=0;$i<3;$i++){
    $EcDBUsePatt[$i]=1-(($ECREcmval[4]/$j)%10);# 10-base BitPattern Msb(DCut Calp Peds)Lsb (=1 to use)
    $j=10*$j;
  }
#
  @MCEcDBUsePatt=();
  $j=1;
  for($i=0;$i<2;$i++){
    $MCEcDBUsePatt[$i]=1-(($ECMCcmval[6]/$j)%10);# 10-base BitPattern Msb(SDcal MCped)Lsb (=1 to use)
    $j=10*$j;
  }
#-
  @Lv1DBUsePatt=();
  $j=1;
  for($i=0;$i<1;$i++){
    $Lv1DBUsePatt[$i]=1-(($TGL1cmval[17]/$j)%10);# 10-base BitPattern Msb(TrConf)Lsb (=1 to use)
    $j=10*$j;
  }
#-
  $Lv1OffPrivFlg=int($TGL1cmval[17]/10);# 0/1=>offic/priv
#--- redef. def-setting for DAQ-r/w param:
  if($SessName eq "RDR" || $SessName eq "MCR"){
    $DAQCcmdf[0]=1;#read daq-files by default
  }
  else{$DAQCcmdf[0]=0;}#do not write MC daq-file by default
  $DAQCcmval[0]=$DAQCcmdf[0];
  $ReadDaqFlg=($DAQCcmval[0]%10);
  $WriteDaqFlg=(int($DAQCcmval[0]/10)%10);
  $ComprDaqFlg=int($DAQCcmval[0]/100);
#
#---
  @TrigPhysmVals=();
  $TrigPhysmPatt=0;# trig-membs bit-pattern: Msb(Ext,unbEC,Phot,Elect,slowIon,Ion,Prot,unbTOF)Lsb
  if($TGL1cmval[0]>0){$TrigPhysmPatt=$TGL1cmval[0];}# setting will be defined by user in this program
#                     (otherwise setting is taken at run time from DB(or TrigConfigFile))
#---
  $Tgl1PrtFlg=($TGL1cmval[18]%10);#lsb
  $Tgl1DebFlg=int($TGL1cmval[18]/10);#msb
  @Tgl1PrtNames=qw(NoPrt Summ+Hist +SetupInfo);
  @Tgl1DebNames=qw(NoDeb ErrMess +TrigPatt +BigDebug +EvBitDump);
  $Tgl1PrtName=$Tgl1PrtNames[$Tgl1PrtFlg];
  $Tgl1DebName=$Tgl1DebNames[$Tgl1DebFlg];
#
  @Tgl1SetupDestNames=qw(Memory +File +DB);
  $Tgl1SetupDestName=$Tgl1SetupDestNames[$TGL1cmval[19]];
#
  @Tgl1MCConfNames=qw(Set1(noPrs) Set2(Presc) Set3);
  $Tgl1MCConfName=$Tgl1MCConfNames[$TGL1cmval[12]-1]; 
#
  @AccRDPrtNames=qw(NoPrint Summary +Histogr +MoreHist);
  $AccRDPrtName=$AccRDPrtNames[$ATREcmval[0]];
  @AccMCPrtNames=qw(NoPrint Histogr +PulseArr +PmPulses);
  $AccMCPrtName=$AccMCPrtNames[$ATMCcmval[0]];
#
  @EmcPrtNames=qw(NoPrint Summary +Histogr);
  @EmcDebNames=qw(NoDebug ErrMess +FullDeb);
  $EmcDebName=$EmcPrtNames[$ECREcmval[1]];
  $EmcPrtName=$EmcPrtNames[$ECREcmval[0]];
#
  @ObjWriteModes=qw(OnlyUsed All +TofMatchedTrk);
  $ObjWriteVal=($IOPAcmval[0]%10);
  $ObjWriteMode=$ObjWriteModes[$ObjWriteVal];
  $Wr2RootfCond1=int($IOPAcmval[0]/10);# if > 0 fill header block even if trig.cond not ok 
  $Wr2RootfCond2=int($IOPAcmval[0]/100);#write event with error if > 0
#
  @EnSpectra=qw(Cosmic undCutof SeaLevMu MevElect UnifMom UnifLogM);
  $EnSpectr=$EnSpectra[$MCGENcmval[17]];
#---
  @TrkPrjNames=qw(y x+y,y x+y,y,x);
  $TrkPrjName=$TrkPrjNames[$CHARcmval[0]];
#
  @TrkFitMethods=qw(Yale Geane);
  $TrkFitMethod=$TrkFitMethods[$TRFIcmval[18]];
#
  @TrkPattRecMethods=qw(JAlcaraz VChoutko);
  $TrkPattRecMethod=$TrkPattRecMethods[$TRFIcmval[19]];
#---
  @FastTrkModes=qw(PresEvsOnly PresEvs+Ion AllEvs-Lvl3=3 AllEvs+Lvl3=3 +HiQualTrk);
  $FastTrkMode=$FastTrkModes[4-$TRFIcmval[6]];
#---
  $FileSize=50;# mb (for daq-files search)
  $MinEvsInFile=50000;
#
  $RunType=$DataTypes[0];# SCI
#
  $RLogName=$AMSJOBcmval[1];#jobname
#----------------------------------------
#--- Read list of all submitted jobs from file "SubmJobsList.*"(upto current session):
#
  $fn="SubmJobsList".".".$SessName;
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
    show_messg("\n   <--- List of Unfinished jobs submitted in prev.sessions:\n");  
    $title="JobId    Node    Que    FirstRunN    LastRunN    SubmDate   SubmTime       JobStat \n";
    $logtext->insert('end',$title);
    $logtext->yview('end');
    for($j=0;$j<$nel;$j++){
      $line=$SubmJobsList[$nel-$j-1];
      @array=unpack($SubmJobsFmt,$line);
      $status=$array[7];
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
#-------
  if($SessName eq "MCS" || $SessName eq "MCR"){return;}# MC do not need RD DAQ-files 
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
  show_messg("\n   <--- Found $DaqfHistLen entries in DAQ-files processing history  :");
  for($i=0;$i<$DaqfHistLen;$i++){#<--- loop over list to show content
    @range=unpack("A10 x1 A10",$DaqfHist[$i]);
    $ptime=localtime($range[0]);#local time of beg-run(imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time="y/m/d=".$year."/".$month."/".$day." h:m:s=".$hour.":".$min.":".$sec;
    $t1=$time;
#
    $ptime=localtime($range[1]);#local time of beg-run(imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time="y/m/d=".$year."/".$month."/".$day." h:m:s=".$hour.":".$min.":".$sec;
    $t2=$time;
#
    $line="RunBeg: ".$range[0]."  Time: ".$t1."  "."RunEnd: ".$range[1]."  Time: ".$t2."\n";
    $logtext->insert('end',$line);
    $logtext->yview('end');
  }
#---
  if($DaqfHistLen > 0){
    show_messg("\n   <--- The last processed Run+1 will be used as default StartRun(+2weeks -> EndRun) !!!");
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
  $fnum1o=$fnum1;# save for later use in "RunDateConv"
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
#---> check the presence of log-files:
#
  $cmd="ls -ogt --time-style=long-iso ".$dir."/*.*[0-9]".".log.*[0-9] |";
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
sub SetupJob
{
  $ResetHistFlg=0;
  $SubmJobCount=0;
  if($fnum1==$fnum2){
    $SingleFileJob=1;
    $MultiSubmFlg="SingleSubm";
  }
#
  show_messg("\n   <--- Setting up job ...","B");
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
#---> set some ext.params for the job:
#
  $jpar1=$Evs2Read;
  $jpar2=$Ncpus;
#---
  my $rsdir,$rssdir;
  if($WrRootFlg==1){
    if($SessName eq "RDR"){$rsdir="/RD";}
    else{$rsdir="/MC";}
    if($RootfSubD eq ""){$rssdir=$RootfSubD;}
    else{$rssdir="/".$RootfSubD;}
    $dir=$RootfDir.$rsdir.$rssdir;
    $cmd="mkdir -p ".$dir;#
    $stat=system($cmd);
    if($stat != 0){
      show_warn("\n   <-- Can't create subdir $rssdir for root-files, check 'rootfiles'-dir !!!");
      return;
    }
    show_messg("\n   <--- RootFilesSubDir $rssdir is opened !","B");
  }
#
  if($SessName eq "MCS" && $WriteDaqFlg>0){
    if($MCDaqfSubD eq ""){$rssdir=$MCDaqfSubD;}
    else{$rssdir="/".$MCDaqfSubD;}
    $dir=$MCDaqfDir.$rsdir.$rssdir;
    $cmd="mkdir -p ".$dir;#
    $stat=system($cmd);
    if($stat != 0){
      show_warn("\n   <-- Can't create subdir $rssdir for MCDaq-files, check 'mcdaqfiles'-dir !!!");
      return;
    }
    show_messg("\n   <--- MCDaqFilesSubDir $rssdir is opened !","B");
  }
#---
  if($SessName eq "MCS"){
    $jpar3=$SetupName;
    $jpar4="/dummy";
    $CalRun1=$MCGENcmval[16];
    $sjobbt_state="normal";
    $sjobbt->configure(-state=>$sjobbt_state);
    return;
  }
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
      $fpath=$DaqDataDir."/".$RunType."/".$daqfrunn[$i];#daq-file path
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
      if($totevs>$Evs2Read || $SingleFileJob==1){
        $DaqfSets+=1;
        last;
      }
    }#--->endof "selected-file" check
  }#--->endof "selected daq-files loop"
#----
  if($totevs<$Evs2Read && $SingleFileJob==0){#<-- low statistics and not 'single file job'
    if($MultiSubmFlg eq "MultiSubm"){
      if($DaqfSets>0){#<-- last Daqf sub-range with low-stat
        show_messg("\n   <--- AutoSubmit: Full DaqFiles-range is completed after $SubmJobCount submissions !");
#---> put here job status/output-manager program  :
        WatchDog();
        show_messg("\n   <--- AutoSession: WatchDog on duty !!!","Big");
	$setupj_state="disabled";
	$setupj_bt->configure(-state=>$setupj_state);   
        $sjobbt_state="disabled";
        $sjobbt->configure(-state=>$sjobbt_state);
      }
      else{
        show_warn("\n   <--- AutoSubmit: only $totevs events found in 1st DaqFiles sub-range, change DaqFiles-range");
        show_warn("          and repeat 'ScanDaqDir'=>'SetupJob', or change 'Events' and repeat 'SetupJob'=> !!!  ");
      }
    }
    else{
      show_warn("\n   <--- ManualSubmit: only $totevs events found, change DaqFiles-range and repeat");
      show_warn("          'ScanDaqDir'=>'SetupJob', or change 'Evs/job' and repeat 'SetupJob'=> !!!  ");
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
    if($SingleFileJob==1 && $totevs<$Evs2Read){
      show_warn("\n   <--- SingleFileJob mode: single file has only $totevs events !?..");
      show_warn("        If it is OK - continue job by clicking 'SubmitJob'...");
    } 
#--> check SetupName<->DaqfRange matching :
    if($CalRun2<1230764399){#--->2008
    print "setupn=",$SetupName,"\n";
      if($SetupName eq "AMS02PreAss"){$jpar3=$SetupName;}
      else{
        show_warn("\n   <--- Mismatch between SetupName and daq-files range, check settings !!!");
	return;
      }
    }
    elsif($CalRun1>1230764399 && $CalRun2<1262300399){#--->2009
      if($SetupName eq "AMS02Ass1"){$jpar3=$SetupName;}
      else{
        show_warn("\n   <--- Mismatch between SetupName and daq-files range, check settings !!!");
	return;
      }
    }
    else{
      if($SetupName eq "AMS02Space"){$jpar3=$SetupName;}
      else{
        show_warn("\n   <--- Mismatch between SetupName and daq-files range, check settings !!!");
	return;
      }
    }
#--> create dir to keep job's daqf-links:
    $dflinksSD="/JL".$CalRun1;
    if($BatchFlg eq "InterJob"){$dflinksSD="/IJL".$CalRun1;};
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
    $jpar4=$lpath."/";
    $sjobbt_state="normal";
    $sjobbt->configure(-state=>$sjobbt_state);
    if($MultiSubmFlg eq "SingleSubm"){#<-- SubmJob will be done manually
      show_messg("\n   <--- $nlinks DaqFiles was selected for SingleSubmit in range $CalRun1 - $CalRun2 ...");
      return;
    }
    else{#<-- Auto JobSubmit
      show_messg("\n   <--- Starting MultiSubmit with $nlinks DaqFiles in range $CalRun1 - $CalRun2 ...","Y");
      $sjobbt->invoke();#<--call SubmitJob
      if($MultiSubmFlg eq "SingleSubm"){return;}#<-- check of "stop auto-submissions"
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
#
  $dfscan_bt->configure(-state=>'normal');
}
#------------------------------------------------------
sub scanddir{ # scan DAQ-directories to search for needed files in required date(runN)-window
#---
  if($fnum1o ne $fnum1 || $fnum2o ne $fnum2 || $fdat1o ne $fdat1 || $fdat2o ne $fdat2){
    show_warn("\n   <--- DAQ-files range was changed but NOT confirmed, click 'Confirm R/D' !!!","Big");
    return;
  }
#---
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
    unshift @ARGV, "-Fpdb_ams";
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
	if(substr($type,0)=~/R2/){$detin[2]=1;}
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
    $editrl_bt->configure(-state=>'normal');
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
                                                        -font=>$ButtWFont,
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
                                                        -font=>$ButtWFont,
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
                                                        -font=>$ButtWFont,
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
							    -font => $ListbWFont,
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
#--- View/Edit created AMS-job script file:
#
    my $stat=0;
    my $fname=$workdir."/UnivJScript";
    $stat=system("nedit $fname");
    if($stat != 0){
      show_warn("   <-- View/Edit UnivJScript-file Error: fail to view file !");
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
  my $pid_alive=0;
  my ($line,$format,$string);
  my $dir=$workdir.$amsjwd;
  my $logf=$dir."/".$RLogName.".".$CalRun1.".log.%J";
  my $logef=$dir."/".$RLogName.".".$CalRun1.".loge.%J";
  my $comm2run;
  my $fn;
  my $ext=substr($CalRun1,-5,5);
  my $jobname=$SessName.$ext;
  my $curline;
#---
  if($BatchFlg eq "InterJob"){
    $fn=$RLogName;
    $fn =~ s/'//g;
    $logf=$dir."/".$fn.".".$CalRun1.".log.00000";#tempor log-file
    show_messg("\n   <--- Be patient, program will be locked for 1-2 mins ...","B");
    $mwnd->update;
    open(INTJLOGF,"+> $logf") or die show_warn("   <-- Cannot open $logf  !");# clear file if exists
    $status=0;
    $status = system("$JobScriptN $jpar1 $jpar2 $jpar3 $jpar4  > $logf");#<--
    if($status != 0){
      show_warn("\n   <--- Interactive job problems !!!");
    }
    else{
      show_messg("   <--- Job is done, enter 'JobOutControl' and inspect logfile !","B");
    }
    close(INTJLOGF) || die show_warn("   <-- Cannot close INTJLOGF !");
    $mwnd->update;
    return;
  }
#---
  if($Host2run eq "Any"){$Host2run="";}
  if($Host2run eq ""){#<-- means any host
#    $comm2run="bsub -q $Queue2run -n $jpar2 -o $logf -e $logef  -J $jobname $JobScriptN";
    $comm2run="bsub -q $Queue2run -o $logf -e $logef  -J $jobname $JobScriptN";
  }
  else{
#    $comm2run="bsub -q $Queue2run -n $jpar2 -m $Host2run -o $logf -e $logef  -J $jobname $JobScriptN";
    $comm2run="bsub -q $Queue2run -m $Host2run -o $logf -e $logef  -J $jobname $JobScriptN";
  }
#
TryAgain:
  $bsubpid=0;
#  $fn="mess.log";
#  open(MESS,"+>$fn") or show_warn("\n   <--- Cannot open mess-file $fn, $!");
#  $status=system("$comm2run $jpar1 $jpar2 $jpar3 $jpar4 >$fn");
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
  $pid=open(BSUBM,"$comm2run $jpar1 $jpar2 $jpar3 $jpar4 |") 
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
    if($attempts<5){
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
  $title="JobId    Node    Que   FirstRunN    LastsRunN    SubmDate    SubmTime       JobStat \n";
  $string=pack($SubmJobsFmt,$bsubpid,$h2run,$Queue2run,$CalRun1,$CalRun2,$submdate,$submtime,$jobsta);
  $line=$string."\n";
  push(@SubmJobsList,$line);
#---> save to SubmJobsList-file:
  my $fn="SubmJobsList".".".$SessName;
  my $nel=scalar(@SubmJobsList);
  open(SJLIST,"> $fn") or die show_warn("\n   <--- Cannot open $fn for writing on 'submit job', $!");
  for($i=0;$i<$nel;$i++){
    print SJLIST $SubmJobsList[$i]."\n";
  }
  close(SJLIST) or die show_warn("\n   <--- Cannot close $fn after writing on 'submit job', $!");
  show_messg("\n   <---  file $fn is updated !");
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
  $swdszx=0.6*$mnwdszx;#X-size of 2nd window (portion of display x-size)
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
                                                        -font=>$ButtWFont,
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
                                                        -font=>$ButtWFont,
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
  my $label="JobId        Node      Que    FirstRunN      LastsRunN         SubmTime      JStat(1/5/10/20/100->sub/pend/run/fin/ok)";
  my $jsw_lb=$jsw_fram->Label(-text=>$label, -anchor=>'w', -font => $LablWFont)
                                            ->place(
                                                    -relwidth=>1., -relheight=>0.07,
                                                    -relx=>0, -rely=>0);
#---
  $jsw_lbox=$jsw_fram->Scrolled("Listbox", -scrollbars => "e", -selectmode => "multiple",
                                                            -relief => "sunken",
							    -font => $ListbWFont,
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
#---> extract job-pids, job-stats from current @SubmJobsList(to use locally):
#
  $nelsjl=scalar(@SubmJobsList);
#  @strarr format => JobId  Node  Que   FirstRunN  LastsRunN  SubmDate  SubmTime  JobStat
#  (if split is used)
  @strarr=();
  for($j=0;$j<$nelsjl;$j++){
    @strarr=split(/\s+/,$SubmJobsList[$j]);
    push(@SubmJPid,$strarr[0]);
    push(@SubmJStat,$strarr[7]);
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
          $rarray[7]=$SubmJStat[$i];#<-- update stat in SubmJobsList
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
  $cmd="ls -ogt --time-style=long-iso ".$dir."/*.*[0-9]".".log.*[0-9] |";
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
    $name=$substr[0];# ="TofTAUCnnn"
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
  $title="JobId      Node     Que  FirstRunN    LastsRunN   SubmDate  Time  JobStat \n";
  $logtext->insert('end',$title);
  $logtext->yview('end');
  for($j=0;$j<$nelsjl;$j++){
    $line=$SubmJobsList[$j];
    @array=unpack($SubmJobsFmt,$line);
    $array[7]=$SubmJStat[$j];#<--update status
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
    $stat=$array[7];
    $rhost=$array[1];
    $cmd="bkill ".$pid2kill." |";
    $pid=open(KJOBS,$cmd) or show_warn("\n   <--- Cannot do 'bkill' to kill job $pid2kill, $!");
    while(<KJOBS>){
      $line=$_;
      @warr=split(/\s+/,$line);#warr => "Job <11230> is being terminated"
      print "<--- Kill.mess=",$line,"\n";
    }
    close(KJOBS);
    $killedpid=$warr[1];
    $killedpid =~ tr/0-9\<\>:/0-9/d;#remove <> in output <killedpid>
    $pid_alive=(kill 0 => $pid);#check existance of process $pid
    if(($killedpid==$pid2kill) && ($warr[4] eq "terminated") && $pid_alive==0){
      show_messg("\n   <--- Job $pid2kill is successfully killed !!!");
      $KilledJobs+=1;#<- count really killed jobs
      $array[7]=-1;#<--update status to -1 (killed)
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
        $array[7]=-1;#<--update status to -1 (killed)
        $line=pack($SubmJobsFmt,@array);
        $SubmJobsList[$index]=$line;#<--update list in memory
        push(@KilledJobsPids,$pid2kill);
      }
      else{
        show_warn("         possibly the 'kill' command failed, so you can try once again...");
      } 
    }
  }
#
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
  $cmd="ls -ogt --time-style=long-iso ".$dir."/*.*[0-9]".".log.*[0-9] |";
  @array=();
  $pid=open(FJOBS,$cmd) or show_warn("\n   <--- Cannot make log-file list in amsjobwd  $!");
  while(<FJOBS>){
    push(@array,$_);
  }
  close(FJOBS);
  $nellfl=scalar(@array);
  if($nellfl==0){show_messg("\n   <--- There are no any log-files in $amsjwd-directory !");}
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
    $name=$substr[0];# ="JobName"
    $stat=1;#<--- logf status=found (2->jobconfirmed,3->logf(job)goodquality)
    $line=pack($LogfListFmt,$lfpid,$strun,$dtime,$fcrmin,$stat)."\n";
    push(@LogFileList,$line);#<-- add to LofFileList new member(line) with latest status
  }
#
#---> delete killed job's log-files:
#
  $nellfl=scalar(@LogFileList);
  for($lf=0;$lf<$nellfl;$lf++){#<---logf-loop (tempor.created above)
    $line=$LogFileList[$lf];
    ($lfpid,$strun,$time,$utcmin,$stat)=unpack($LogfListFmt,$line);
    for($kind=0;$kind<$KilledJobs;$kind++){#<-- look into killed jobs list
      $kjpid=$KilledJobsPids[$kind];
      if($kjpid==$lfpid){#<-- delete related log(e)-files
        $fn="*.".$strun.".log.".$lfpid;
        $fnfr=$workdir.$amsjwd."/".$fn;
        $rwsta = system("rm -f $fnfr");
        if($rwsta != 0){show_warn("\n   <-- Can't delete $fn in work-dir !");}
        else{show_messg("\n   <-- file $fn is deleted in work-dir !");}
#      
        $fne="*.".$strun.".loge.".$lfpid;
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
#---> delete link-dirs of deleted jobs:
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
    $strun=$array[3];
    $stat=$array[7];
    if($stat==-1){#<-- job was deleted, delete related daqf-links
      $sjpid=$array[0];
      $strun=$array[3];
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
  $fn="SubmJobsList".".".$SessName;
  my @newlist=();
  open(SJLIST,"> $fn") or die show_warn("\n   <--- Can't open $fn for removing of killed job, $!");
  for($j=0;$j<$nelsjl;$j++){
    $line=$SubmJobsList[$j];
    @array=unpack($SubmJobsFmt,$line);
    if($array[7]==-1){
      next;#<-- skip deleted job line
    }
    print SJLIST $line."\n";
    push(@newlist,$line);
  }
  close(SJLIST) or show_warn("\n   <--- Cannot close $fn after writing, $!");
  @SubmJobsList=();
  push(@SubmJobsList,@newlist);
  show_messg("\n   <---  SubmJobsList/file are updated after job killing !");
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
  $swdszx=0.6*$mnwdszx;#X-size of 2nd window (portion of display x-size)
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
  my $nbuttons=4;
  my $lbfwid=0.74;#width(portion of topl4-window) of listboxes frame
  my $bvrspac=0.05;#button rel V-spacing in units of button-height
  my $vbias=0.08;#buttons v-bias to place frame-label
  my $bheight=(1-$vbias)/(1+$bvrspac)/$nbuttons;
  my $bspac=$bheight*$bvrspac;#abs.v-spacing for buttons
  my $crely,$relwid;
  $jocp_fram=$topl4->Frame(-label => "JobOutp Control panel",-background => "Cyan",  
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>(1-$lbfwid), -relheight=>1,
                                                    -relx=>0, -rely=>0);
  $crely=$vbias;
#---
  if($BatchFlg eq "BatchJob"){$relwid=0.5;}
  else{$relwid=1.;} 
  $showlf_bt=$jocp_fram->Button(-text=>"ShowLog",
                                                        -font=>$ButtWFont,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ocontr_shlog)->place(
                                                                 -relwidth=>$relwid, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  if($BatchFlg eq "BatchJob"){
    $showelf_bt=$jocp_fram->Button(-text=>"ShowELog",
                                                        -font=>$ButtWFont,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ocontr_shelog)->place(
                                                                 -relwidth=>0.5, -relheight=>$bheight,
                                                                           -relx=>0.5, -rely=>$crely);
  }
  $crely+=($bheight+$bspac);
#---
  $delilf_bt=$jocp_fram->Button(-text=>"MarkLogAsBad",
                                                        -font=>$ButtWFont,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ocontr_delintjlog)->place(
                                                                 -relwidth=>1., -relheight=>$bheight,
                                                                           -relx=>0., -rely=>$crely);
#   
  $crely+=($bheight+$bspac);
#--- 
  $cleanup_bt=$jocp_fram->Button(-text=>"Cleanup",
                                                        -font=>$ButtWFont,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ocontr_cleanup)->place(
                                                                 -relwidth=>1., -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  if($BatchFlg eq "BatchJob"){$cleanup_bt->configure(-state => "normal");}
  else{$cleanup_bt->configure(-state => "disabled");}
#
  $crely+=($bheight+$bspac);
#--
  $finish_bt=$jocp_fram->Button(-text=>"Finish", 
                                                        -font=>$ButtWFont,
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ocontr_finish)->place(
                                                                 -relwidth=>1., -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
#-------- frame for logf-listbox:
  my $nlistbs=1;
  my $vbias=0.08;#listb v-bias to place frame-label
  my $lbwid=1;
  my $crelx;
  @JClboxPointer=();
#
  $listb_fram=$topl4->Frame(-background => "Cyan", 
                          -relief=>'groove', 
			  -borderwidth=>5)->
			   place(-relwidth=>$lbfwid, -relheight=>1,
                                 -relx=>(1-$lbfwid), -rely=>0);
#---
  my $label="   List of found LogFiles(status=1/2/3->found/JobConfirmed/good)";
  my $logf_lab=$listb_fram->Label(-text=>$label, -anchor=>'w', -font => $LablWFont)
                                            ->place(
                                                    -relwidth=>1, -relheight=>$vbias,
                                                    -relx=>0, -rely=>0);
  $crelx=0;
#--- logfile lbx
  $logf_lbx=$listb_fram->Scrolled("Listbox", -scrollbars => "e", -selectmode => "multiple",
                                                            -relief => "sunken",
							    -font => $ListbWFont,
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
    $strun=$sarray[3];
    $ext=substr($strun,-5,5);
    $jobname=$SessName.$ext;
    $stat=$sarray[7];# -1/0/1/5/10/20/30/40/100/->killed/???/submitted/pend/running/ended/haslogf/logfok/goodjob
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
	  if(scalar(@string)<9){next;}#<-- crazy line (LSF bug) - skip it
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
      $sarray[7]=$stat;#update job-status
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
  if($FiniJobCount==0 && $BatchFlg eq "BatchJob"){
    show_messg("\n   <--- JobOutputControl: there are no New finished batch-jobs !");
#    return;
  }
  else{show_messg("\n   <--- JobOutputControl: Found $FiniJobCount finished batch-jobs !");}
#
#------> create the list of A-L-L log-files(=> finished jobs) on the moment of 'JobOutpContr' clicking :
#                                     (also match with jobs)
  @LogJobMatchIndex=();
  $dir=$workdir.$amsjwd;
  $cmd="ls -ogt --time-style=long-iso ".$dir."/*.*[0-9]".".log.*[0-9] |";
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
  my $logfok,$nnel;;
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
    $name=$substr[0];# ="JobName"
    $stat=1;#<--- logf status=found (2->jobconfirmed,3->logf(job)goodquality)
    for($ij=0;$ij<$nelsjl;$ij++){#<-- match logs with jobs (by sjpid)
      $sjpid=$SubmJPid[$ij];
      $sjsta=$SubmJStat[$ij];
      if($lfpid==$sjpid && ($sjsta==10 || $sjsta==5 || $sjsta==20)){#<-- found related job(running/pend job could be finished just befor)
        $stat=2;#<--logf-status = "job-matched"
	$JobConfLogFiles+=1;
        $SubmJStat[$ij]=30;#<-- job-status = "logf-matched"
	$jmindx=$ij;
	$LogJobMatchIndex[$j]=$ij;# attach matched job-index(as in SubmJobList) to LogFileList index
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
      $logfok=0;
      @lfresult=();
      for($il=0;$il<$nel;$il++){# <-- loop over log-file content
	if($rarray[$il] =~ /NUMBER OF EVENTS PROCESSED/){
	  @lfresult=split(/\s+/,$rarray[$il]);
	  $nnel=scalar(@lfresult);
	  if($nnel==8 && $lfresult[1] eq "****" && $lfresult[6] eq "=" && $lfresult[7] > 1){
	    $logfok=1;
	    last;
	  }
	}
      }# --> endof log-file content loop
      if($logfok==1){
        $GoodLogFiles+=1;
        $stat=3;#<--log with good results
        $SubmJStat[$jmindx]=40;#<-- job has logf with good results
      }
      else{
	show_warn("\n   <--- Finished/logmatched job $lfpid has bad result according to Log-file !!!");
      }
#--> update job-status in SubmJobsList:
      $line=$SubmJobsList[$jmindx];
      @rarray=unpack($SubmJobsFmt,$line);
      $rarray[7]=$SubmJStat[$jmindx];
      $SubmJobsList[$jmindx]=pack($SubmJobsFmt,@rarray);
#
    }#--->endof 'logf is job-confirmed'
#<--- endof job(logf) quality check
    $line=pack($LogfListFmt,$lfpid,$name,$strun,$dtime,$stat)."\n";
    push(@LogFileList,$line);#<-- add to LofFileList new member(line) with latest status
  }#--->endof logf-loop
#
  if($GoodLogFiles==0 && $BatchFlg eq "BatchJob"){  
    show_messg("\n   <--- JobOutputControl: there are no any good log-files in $amsjwd-directory !");
  }
#
#--- refill LogFiles Lbox:
#
  @loglistbox=();
  for($i=0;$i<$nellfl;$i++){#prepare list to view
    $line=$LogFileList[$i];
    @string=unpack($LogfListFmt,$LogFileList[$i]);
    $line=pack($LogfLboxFmt,$string[0],$string[1],$string[2],$string[3],$string[4]);
    push(@loglistbox,$line);
  }
  $logf_lbx->insert("end",@loglistbox);
#--------
  $topl4->update;
  $JOCReady=1;
}
#---------------
sub ocontr_shlog
{
  my $lfpid,$name,$strun,$time,$utcmin,$stat;
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
    ($lfpid,$name,$strun,$time,$stat)=unpack($LogfListFmt,$line);
    $fn=$name.".".$strun.".log.".$lfpid;
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
  my $lfpid,$name,$strun,$time,$utcmin,$stat;
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
    ($lfpid,$name,$strun,$time,$stat)=unpack($LogfListFmt,$line);
    $fn=$name.".".$strun.".loge.".$lfpid;
    $fname=$workdir.$amsjwd."/".$fn;
    $stat=system("nedit -read $fname");
    if($stat != 0){
      show_warn("   <-- ShowErrLog error: fail to view file $fn !");
      return;
    }
  }
}
#---------------
sub ocontr_delintjlog{
  my $lfpid,$name,$strun,$time,$utcmin,$stat;
  my $index,$nel,$fn,$fname,$dir,$cmd,$cmdstat;
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
    ($lfpid,$name,$strun,$time,$stat)=unpack($LogfListFmt,$line);
    $stat=0;# mark log-file for killing
    $line=pack($LogfListFmt,$lfpid,$name,$strun,$time,$stat);# <- update stat in LogfList
    $LogFileList[$index]=$line;
  }
#--- update listbox:
  @loglistbox=();
  $nel=scalar(@LogFileList);
  for($i=0;$i<$nel;$i++){#prepare list to view
    $line=$LogFileList[$i];
    @string=unpack($LogfListFmt,$LogFileList[$i]);
    $line=pack($LogfLboxFmt,$string[0],$string[1],$string[2],$string[3],$string[4]);
    push(@loglistbox,$line);
  }
  $logf_lbx->delete(0,"end");
  $logf_lbx->insert("end",@loglistbox);
#
#--> remove related(interactive) link-dirs:
  my $dflinksTD=$workdir.$daqflnk;
  my $dflinksSD;
#
  $dflinksSD="/IJL".$strun;
  $dir=$dflinksTD.$dflinksSD;
  $cmd="rm -rf $dir";
  $cmdstat=system($cmd);
  if($cmdstat != 0){
    show_warn("\n   <--- Failed to remove link-dir of $strun interactive jobs !!!");
  }
  else{show_messg("\n   <--- Link-dir of $strun interactive jobs is removed  !");}
#---
  $topl4->update;
#
}
#---------------
sub ocontr_cleanup
{
#
#---> cleanup log-files list:
#
  my $nellfl=scalar(@LogFileList);
  my ($lfpid,$name,$strun,$time,$utcmin,$stat);
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
    ($lfpid,$name,$strun,$time,$stat)=unpack($LogfListFmt,$line);
    if($stat<3 && $stat>0){#<-- delete because not-matched
      $stat=0;# mark log-file for killing
      $line=pack($LogfListFmt,$lfpid,$name,$strun,$time,$stat);# <- update stat in LogfList
      $LogFileList[$lf]=$line;
      show_messg("\n      <--- Job's $lfpid log-file is marked as Bad(cfiles will not be kept) !!!","Y");
      @rarray=();
      $line=$SubmJobsList[$LogJobMatchIndex[$lf]];# <- update stat in SubmJobList
      @rarray=unpack($SubmJobsFmt,$line);
      $rarray[7]=0;# mark job for killing in SubmJobList as having bad logfile
      $SubmJobsList[$LogJobMatchIndex[$lf]]=pack($SubmJobsFmt,@rarray);
#      $logf_lbx->selectionSet($lf-$ditm);
#      $logf_lbx->delete($lf-$ditm);
      $ditm+=1;
    }
  }#--->endof logf-loop
#
#--- update Log/CalFiles Lboxes to see new statuses:
#
  my @loglistbox=();
  my @string=();
  for($i=0;$i<$nellfl;$i++){#prepare list to view
    $line=$LogFileList[$i];
    @string=unpack($LogfListFmt,$LogFileList[$i]);
    $line=pack($LogfLboxFmt,$string[0],$string[1],$string[2],$string[3],$string[4]);
    push(@loglistbox,$line);
  }
  $logf_lbx->delete(0,"end");
  $logf_lbx->insert("end",@loglistbox);
#
  $topl4->update;
#
}
#---------------
sub ocontr_finish
{
#---
  my $nellfl=scalar(@LogFileList);
  my ($lfpid,$name,$strun,$time,$utcmin,$stat);
  my @rarray=();
  my $line,$nel,$dir;
  my ($strunc,$timec,$utcminc,$statc);
  my @rlistbox=();
  my @string=();
  my $fn,$fne,$fnfr,$fnto,$rwsta;
  my $bext;
#
#---> move/delete log-files:
#
  for($lf=0;$lf<$nellfl;$lf++){#<---logf-loop
    $line=$LogFileList[$lf];
    ($lfpid,$name,$strun,$time,$stat)=unpack($LogfListFmt,$line);
    $fn=$name.".".$strun.".log.".$lfpid;
    $fne=$name.".".$strun.".loge.".$lfpid;
    $fnfr=$workdir.$amsjwd."/".$fn;
    $fnto=$workdir.$logfdir."/".$fn;
    if($stat==0){#bad logf - delete it
      $rwsta = system("rm -f $fnfr");
      if($rwsta != 0){show_warn("\n   <-- Can't delete bad logfile $fn !");}
      else{show_messg("\n   <--- Bad Log-file $fn was deleted !");}
    }
    else{
      $rwsta = system("mv $fnfr $fnto");
      if($rwsta != 0){show_warn("\n   <-- Can't move $fn to store !");}
      else{show_messg("\n   <--- Log-file $fn was moved to store !");}
    }
#
    if($BatchFlg ne "BatchJob"){next;}
#  
    $fnfr=$workdir.$amsjwd."/".$fne;
    $fnto=$workdir.$logfdir."/".$fne;
    if($stat==0){#bad elogf - delete it
      $rwsta = system("rm -f $fnfr");
      if($rwsta != 0){show_warn("\n   <-- Can't delete bad errlogfile $fne !");}
      else{show_messg("\n   <--- Bad ErrLog-file $fne was deleted !");}
    }
    else{
      $rwsta = system("mv $fnfr $fnto");
      if($rwsta != 0){show_warn("\n   <-- Can't move $fne to store !");}
      else{show_messg("\n   <--- ErrLog-file $fne was moved to store !");}
    }
  }#--->endof logf-loop
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
    $strun=$array[3];
    $stat=$array[7];# 0/1/5/10/20/30/40/100/->killed/submitted/pend/running/ended/haslogf/logfok/goodjob
    if($stat==0 || $stat>=20){next;}
    push(@narray,$line."\n");
  }
#-- save:
  $fn="SubmJobsList".".".$SessName;
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
  $swdszx=0.6*$mnwdszx;#X-size of 2nd window (portion of display x-size)
  $swdszy=1-$mnwdszy-0.11;#Y-size of 2nd window (portion of display y-size)(0.1->bot.syst.panel)
#
#---> define size and create WatchDog status-window:
#
  $topl5=$mwnd->Toplevel();
  $wdwdszx=0.15;#X-size of window (portion of display x-size)
  $wdwdszy=1-$mnwdszy-0.11;#Y-size of window (portion of display y-size, 0.1->bot.system pannel )
  my $szx=int($wdwdszx*$displx);#in pixels
  my $szy=int($wdwdszy*$disply);
  my $wdwdpy=int(($mnwdszy+0.03)*$disply);# 0.03 for safety
  my $wdwdposy="+".$wdwdpy;
  my $wdwdpx=int(($mnwdszx-$wdwdszx)*$displx);# wind. pos-x in pixels
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
                          -font=>$LablWFont,-relief=>'groove')
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
                          -font=>$LablWFont,-relief=>'groove')
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
    $cleanup_bt->invoke();
    $topl4->after(5000);# tempor for debug (to see result)
    $finish_bt->invoke();
    $topl4->after(2000);# tempor for debug
  }
  if($MultiSubmFlg eq "MultiSubm" && $UnFinishedJobs>0){
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
#---> save list of  jobs(unfinished only !!!) in file:
#
  my $nelsjl=scalar(@SubmJobsList);
#
  for($j=0;$j<$nelsjl;$j++){#<--- SubmJobList loop
    $line=$SubmJobsList[$j];
    @array=unpack($SubmJobsFmt,$line);
    $sjpid=$array[0];
    $strun=$array[3];
    $stat=$array[7];# 0/1/5/10/20/30/40/100/->killed/submitted/running/pend/ended/haslogf/logfok/goodjob
    if(($stat > 0) && ($stat < 20)){push(@narray,$line);}#save only unfinished jobs
  }
#-- save:
  $fn="SubmJobsList".".".$SessName;
  open(SJLIST,"> $fn") or show_warn("\n   <--- Cannot open $fn for update, $!");
  for($i=0;$i<$nelsjl;$i++){print SJLIST $narray[$i]."\n";}
  close(SJLIST) or show_warn("\n   <--- Cannot close $fn after updating, $!");
#
#---> Create/Move "screen" Log-file:
#
  my $logfname=$SessName."sessLog.";
  $WarnFlg=0;
  $filen=$logfname.$SessTLabel;#extention is session time label 
  $newfn=$workdir.$logfdir."/".$filen;
  open(OFN, "> $filen") or show_warn("   <-- Cannot open $filen for writing !");
  print OFN $logtext->get("1.0","end");
  close(OFN) or show_warn("   <-- Cannot close $filen !");
  if($WarnFlg==0){ 
    show_messg("\n   <-- LogFile $filen is saved !");
    $rwsta = system("mv $filen $newfn");
    if($rwsta != 0){show_warn("\n   <-- Can't move $filen to store !");}
    else{show_messg("\n   <--- SessionLog $filen was moved to store !");}
  }
#------
  $mwnd->update;
  if($soundtext eq "SoundON"){$mwnd->bell;}
  $mwnd->after(6000);
  $dir_fram->destroy() if Exists($dir_fram);
  $set_fram->destroy() if Exists($set_fram);
  $jcl_fram->destroy() if Exists($jcl_fram);
  $topl3->destroy() if Exists($topl3);
  $topl4->destroy() if Exists($topl4);
  $logtext->delete("1.0",'end');
#--> enable JobType/Start-buttons:
  my $state="normal";
  for($i=0;$i<$JobTypes;$i++){
    $JobTypButt[$i]->configure(-state=>$state);
  }
  $StartButt->configure(-state=>$state);
#
  $mwnd->update;
}
#---------------------------------------------------------------
sub SetTofPars{
  my $i,$j;
  my $shf,$nl,$nlmc,$drh,$labw,$entw,$rwd,$xpos;
#------
$sdset_fram->destroy() if Tk::Exists($sdset_fram);
#------
if($RecoSimuP eq "RECO"){
#
#--------> RD page-1 :
  if($PageNumb eq "Page1"){
#
  $sdset_fram=$set_fram->Frame(-label=>"Modify Page-1 RECO-Params for selected above group:",-relief=>'groove',
                                                  -borderwidth=>5,-foreground => "red",
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
  my @prttypes=qw(Sum Hist TdcH Debg More);
  @TofPrtBut=();
  $labw=0.08;
  $butw=0.08;
  $xpos=0;
  $sdset_fram->Label(-text=>"Prt:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  for($i=0;$i<5;$i++){
    $sdset_fram->Checkbutton(-text=>$prttypes[$i], -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TFREcmval[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
    $xpos+=$butw;
  }
#---
  my @tofdbdatatypes=qw(Cal Ped DCut Elos TdcL);
  $labw=0.12;
  $butw=0.08;
  $sdset_fram->Label(-text=>"UseDB4:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  for($i=0;$i<5;$i++){
    $sdset_fram->Checkbutton(-text=>$tofdbdatatypes[$i], -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TofDBUsePatt[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
    $xpos+=$butw;
  }
#-----
  $xpos=0;
  $labw=0.13;
  $sdset_fram->Label(-text=>"CalFiles:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
#
  $butw=0.1;
  $sdset_fram->Radiobutton(-text=>"Offic",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>0, -variable=>\$TFCAcmval[1])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$butw;
#
  $butw=0.1;
  $sdset_fram->Radiobutton(-text=>"Priv",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>1, -variable=>\$TFCAcmval[1])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$butw;
#---
  $labw=0.22;
  $entw=0.22;
  $sdset_fram->Label(-text=>"RefCalSetN:",-font=>$LablWFont,-relief=>'groove')
                                               ->place(
					       -relwidth=>$labw, -relheight=>$drh,
                                               -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Optionmenu(-textvariable => \$TofRefCalSetRD, 
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [@TofRefCflistListRD],
			       -command => sub{$TFREcmval[12]=$TofRefCalSetRD;}
	                       )
                               ->place(
                               -relwidth=>$entw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$entw;
#---
  $butw=0.23;
  $sdset_fram->Checkbutton(-text=>"HistHit-Match", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TFREcmval[5])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$butw;
#-----
  $labw=0.4;
  $entw=0.1;
  $xpos=0;
  $sdset_fram->Label(-text=>"DAQthr(sig) An/Dyn:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TFREcmval[9])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TFREcmval[10])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#---
  $labw=0.25;
  $entw=0.15;
  $sdset_fram->Label(-text=>"Adc2pC conv.f:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TFCAcmval[0])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#---
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+3*$drh));
  }#<-------- endof RD page-1 :
  elsif($PageNumb eq "Page2"){#-------->  RD page-2 :
#
  $sdset_fram=$set_fram->Frame(-label=>"Modify Page-2 RECO-Params for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,-foreground => "red",
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
  my @TofLayPatt=qw(1+3 1+4 2+3 2+4 1+2 3+4);
  @TofLayPattBut=();
  $labw=0.34;
  $butw=0.11;
  $xpos=0;
  $sdset_fram->Label(-text=>"AcceptVelocityLaysPatt:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  for($i=0;$i<6;$i++){
    $sdset_fram->Checkbutton(-text=>$TofLayPatt[$i], -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$BETAFITcmval[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
    $xpos+=$butw;
  }
#---
  $labw=0.3;
  $entw=0.1;
  $xpos=0;
  $sdset_fram->Label(-text=>"MaxChi2(time/space-fit):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$BETAFITcmval[6])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$entw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$BETAFITcmval[7])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$entw;
#---
  $labw=0.4;
  $entw=0.05;
  $sdset_fram->Label(-text=>"SearchRegAroundTrk(Sigm,x/y):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$BETAFITcmval[8])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$entw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$BETAFITcmval[9])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$entw;
#---
  $butw=0.3;
  $xpos=0;
  $sdset_fram->Checkbutton(-text=>"AllowTofClustSharing", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$BETAFITcmval[10])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$butw;
#---
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+3*$drh));
  }#<-------- endof RD page-2 :
}
#------------ MC:
elsif($RecoSimuP eq "SIMU"){
  if($PageNumb eq "Page1"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify Page-1 SIMU-params for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nlmc=3;
  $drh=(1.-$shf)/$nlmc;
#
  my @mcprttypes=qw(PMpulse Histogr Debug);
  $labw=0.08;
  $butw=0.14;
  $xpos=0;
  $sdset_fram->Label(-text=>"Prt:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  for($i=0;$i<3;$i++){
    $sdset_fram->Checkbutton(-text=>$mcprttypes[$i], -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TFMCcmval[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
    $xpos+=$butw;
  }
#---
  my @mctofdbdatatypes=qw(CalSd Tmap Peds TlSd);
  $labw=0.12;
  $butw=0.095;
  $sdset_fram->Label(-text=>"UseDB4:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  for($i=0;$i<4;$i++){
    $sdset_fram->Checkbutton(-text=>$mctofdbdatatypes[$i], -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$MCTofDBUsePatt[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
    $xpos+=$butw;
  }
#-----
  $labw=0.22;
  $entw=0.2;
  $xpos=0;
  $sdset_fram->Label(-text=>"RefCalSetN:",-font=>$LablWFont,-relief=>'groove')
                                               ->place(
					       -relwidth=>$labw, -relheight=>$drh,
                                               -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#---
  $sdset_fram->Optionmenu(-textvariable => \$TofRefCalSetMC, 
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [@TofRefCflistListMC],
			       -command => sub{$TFMCcmval[4]=$TofRefCalSetMC;}
	                       )
                               ->place(
                               -relwidth=>$entw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw+$entw;
#------ 
  $labw=0.4;
  $entw=0.1;
  $xpos=0;
  $sdset_fram->Label(-text=>"Thresh(mv) LT/HR/SHT:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  for($i=0;$i<3;$i++){
    $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TFREcmval[$i+6])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
    $xpos+=$entw;
  }
#---
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+2*$drh));
  }#-->endof page-1
}
}
#---------------------------------------------------
sub SetAccPars{
  my $i,$j;
  my $shf,$nl,$nlmc,$drh,$labw,$entw,$rwd,$xpos;
#------
$sdset_fram->destroy() if Tk::Exists($sdset_fram);
#------
if($RecoSimuP eq "RECO"){
  if($PageNumb eq "Page1"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify Page-1 RECO-Params for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
  $labw=0.17;
  $xpos=0;
  $sdset_fram->Label(-text=>"PrintMode:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>$shf);
  $xpos+=$labw;
#-
  $menw=0.2;
  $optmenu1=$sdset_fram->Optionmenu(-textvariable => \$AccRDPrtName, -variable => \$ATREcmval[0],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$AccRDPrtNames[0],0],[$AccRDPrtNames[1],1],[$AccRDPrtNames[2],2],
			                    [$AccRDPrtNames[3],3]],
#			       -command => sub{print "text/var=",$AccRDPrtName," ",$ATREcmval[0],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#---
  $butw=0.17;
  $sdset_fram->Checkbutton(-text=>"DebugPrt", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$ATREcmval[1])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#---
  my @accdbdatatypes=qw(StabP VarP Peds);
  $labw=0.13;
  $butw=0.11;
  $sdset_fram->Label(-text=>"UseDB4:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  for($i=0;$i<3;$i++){
    $sdset_fram->Checkbutton(-text=>$accdbdatatypes[$i], -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$AccDBUsePatt[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
    $xpos+=$butw;
  }
#-----
  $xpos=0;
  $labw=0.13;
  $sdset_fram->Label(-text=>"CalFiles:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
#
  $butw=0.1;
  $sdset_fram->Radiobutton(-text=>"Offic",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>0, -variable=>\$ATCAcmval[0])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$butw;
#
  $butw=0.1;
  $sdset_fram->Radiobutton(-text=>"Priv",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>1, -variable=>\$ATCAcmval[0])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$butw;
#---
  $labw=0.22;
  $entw=0.22;
  $sdset_fram->Label(-text=>"RefCalSetN:",-font=>$LablWFont,-relief=>'groove')
                                               ->place(
					       -relwidth=>$labw, -relheight=>$drh,
                                               -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Optionmenu(-textvariable => \$AccRefCalSetRD, 
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [@AccRefCflistListRD],
			       -command => sub{$ATREcmval[5]=$AccRefCalSetRD;}
	                       )
                               ->place(
                               -relwidth=>$entw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$entw;
#---
  $butw=0.23;
  $sdset_fram->Checkbutton(-text=>"AmLinCorrON", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$ATREcmval[3])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$butw;
#-----
  $labw=0.32;
  $entw=0.1;
  $xpos=0;
  $sdset_fram->Label(-text=>"Thresh/sector(mev):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$ATREcmval[2])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
#---
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+3*$drh));
  }#-->endof page-1
}
#------ MC:
elsif($RecoSimuP eq "SIMU"){
  if($PageNumb eq "Page1"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify Page-1 SIMU-params for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nlmc=3;
  $drh=(1.-$shf)/$nlmc;
#---
  $labw=0.15;
  $xpos=0;
  $sdset_fram->Label(-text=>"PrintMode:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>$shf);
  $xpos+=$labw;
#-
  $menw=0.2;
  $optmenu1=$sdset_fram->Optionmenu(-textvariable => \$AccMCPrtName, -variable => \$ATMCcmval[0],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$AccMCPrtNames[0],0],[$AccMCPrtNames[1],1],[$AccMCPrtNames[2],2],
			                    [$AccMCPrtNames[3],3]],
#			       -command => sub{print "text/var=",$AccMCPrtName," ",$ATMCcmval[0],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#---
  my @mcaccdbdatatypes=qw(MCped SDped);
  $labw=0.14;
  $butw=0.12;
  $sdset_fram->Label(-text=>"UseDB4:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  for($i=0;$i<2;$i++){
    $sdset_fram->Checkbutton(-text=>$mcaccdbdatatypes[$i], -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$MCAccDBUsePatt[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
    $xpos+=$butw;
  }
#------
  $labw=0.22;
  $entw=0.22;
  $xpos=0;
  $sdset_fram->Label(-text=>"RefCalSetN:",-font=>$LablWFont,-relief=>'groove')
                                               ->place(
					       -relwidth=>$labw, -relheight=>$drh,
                                               -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Optionmenu(-textvariable => \$AccRefCalSetMC, 
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [@AccRefCflistListMC],
			       -command => sub{$ATMCcmval[3]=$AccRefCalSetMC;}
	                       )
                               ->place(
                               -relwidth=>$entw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$entw;
#--- 
  $labw=0.4;
  $entw=0.1;
  $sdset_fram->Label(-text=>"EffLightVel[cm/ns]:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$ATMCcmval[1])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+$drh));
    $xpos+=$entw;
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+2*$drh));
  }#-->endof page-1
}
}
#---------------------------------------------------
sub SetEmcPars{
  my $i,$j;
  my $shf,$nl,$nlmc,$drh,$labw,$entw,$rwd,$xpos;
#------
  $sdset_fram->destroy() if Tk::Exists($sdset_fram);
#------
if($RecoSimuP eq "RECO"){
  if($PageNumb eq "Page1"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify Page-1 RECO-Params for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
  $labw=0.15;
  $xpos=0;
  $sdset_fram->Label(-text=>"PrintMode:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>$shf);
  $xpos+=$labw;
#-
  $menw=0.15;
  $optmenu1=$sdset_fram->Optionmenu(-textvariable => \$EmcPrtName, -variable => \$ECREcmval[0],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$EmcPrtNames[0],0],[$EmcPrtNames[1],1],[$EmcPrtNames[2],2]],
#			       -command => sub{print "text/var=",$EmcPrtName," ",$ECREcmval[0],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#---
  $labw=0.16;
  $sdset_fram->Label(-text=>"DebugMode:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>$shf);
  $xpos+=$labw;
#-
  $menw=0.15;
  $optmenu1=$sdset_fram->Optionmenu(-textvariable => \$EmcDebName, -variable => \$ECREcmval[1],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$EmcDebNames[0],0],[$EmcDebNames[1],1],[$EmcDebNames[2],2]],
#			       -command => sub{print "text/var=",$EmcDebName," ",$ECREcmval[1],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#---
  my @ecdbdatatypes=qw(Peds Calp DCut);
  $labw=0.12;
  $butw=0.09;
  $sdset_fram->Label(-text=>"UseDB4:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  for($i=0;$i<3;$i++){
    $sdset_fram->Checkbutton(-text=>$ecdbdatatypes[$i], -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$EcDBUsePatt[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
    $xpos+=$butw;
  }
#-----
  $xpos=0;
  $labw=0.13;
  $sdset_fram->Label(-text=>"CalFiles:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
#
  $butw=0.1;
  $sdset_fram->Radiobutton(-text=>"Offic",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>0, -variable=>\$ECCAcmval[0])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$butw;
#
  $butw=0.1;
  $sdset_fram->Radiobutton(-text=>"Priv",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>1, -variable=>\$ECCAcmval[0])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$butw;
#---
  $labw=0.22;
  $entw=0.22;
  $sdset_fram->Label(-text=>"RefCalSetN:",-font=>$LablWFont,-relief=>'groove')
                                               ->place(
					       -relwidth=>$labw, -relheight=>$drh,
                                               -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Optionmenu(-textvariable => \$EmcRefCalSetRD, 
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [@EmcRefCflistListRD],
			       -command => sub{$ECREcmval[5]=$EmcRefCalSetRD;}
	                       )
                               ->place(
                               -relwidth=>$entw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$entw;
#-----
  $labw=0.32;
  $entw=0.08;
  $xpos=0;
  $sdset_fram->Label(-text=>"DaqThr(sig)(A|D):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$ECREcmval[2])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$ECREcmval[3])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#---
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+3*$drh));
  }#-->endof page-1
}
#------ MC:
elsif($RecoSimuP eq "SIMU"){
  if($PageNumb eq "Page1"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify Page-1 SIMU-params for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nlmc=4;
  $drh=(1.-$shf)/$nlmc;
#------
  $butw=0.2;
  $xpos=0;
  $sdset_fram->Checkbutton(-text=>"PrintHistg", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$ECMCcmval[0])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#---
  $labw=0.17;
  $sdset_fram->Label(-text=>"UseAsSeed:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#
  $butw=0.12;
  $sdset_fram->Radiobutton(-text=>"MCcal",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>0, -variable=>\$ECMCcmval[2])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#
  $butw=0.12;
  $sdset_fram->Radiobutton(-text=>"RDcal",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>1, -variable=>\$ECMCcmval[2])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#---
  my @mcecdbdatatypes=qw(MCped SDcal);
  $labw=0.13;
  $butw=0.12;
  $sdset_fram->Label(-text=>"UseDB4:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  for($i=0;$i<2;$i++){
    $sdset_fram->Checkbutton(-text=>$mcecdbdatatypes[$i], -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$MCEcDBUsePatt[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
    $xpos+=$butw;
  }
#------
  $xpos=0;
  $labw=0.16;
  $entw=0.11;
  $sdset_fram->Label(-text=>"RefCalSet:",-font=>$LablWFont,-relief=>'groove')
                                               ->place(
					       -relwidth=>$labw, -relheight=>$drh,
                                               -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Optionmenu(-textvariable => \$EmcRefCalSetMC, 
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [@EmcRefCflistListMC],
			       -command => sub{$ECMCcmval[7]=$EmcRefCalSetMC;}
	                       )
                               ->place(
                               -relwidth=>$entw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$entw;
#------ 
  $labw=0.22;
  $entw=0.12;
  $sdset_fram->Label(-text=>"egCut(gev):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$ECMCcmval[1])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$entw;
#---
  $labw=0.22;
  $entw=0.11;
  $sdset_fram->Label(-text=>"Mev2Mev:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$ECMCcmval[3])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$entw;
#------
  $xpos=0;
  $labw=0.21;
  $entw=0.12;
  $sdset_fram->Label(-text=>"Mev2Adc:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$ECMCcmval[4])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#---
  $labw=0.21;
  $entw=0.12;
  $sdset_fram->Label(-text=>"Adc2pC:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$ECMCcmval[5])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+3*$drh));
  }#-->endof page-1
}
}
#-----------------------------------------------
sub SetIOPars{
  my $i,$j;
  my $shf,$nl,$nlmc,$drh,$labw,$entw,$rwd,$xpos;
#------
$sdset_fram->destroy() if Tk::Exists($sdset_fram);
#------
if($RecoSimuP eq "RECO"){
  if($PageNumb eq "Page1"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify Page-1 RECO-Params for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
  $labw=0.28;
  $entw=0.04;
  $xpos=0;
  $sdset_fram->Label(-text=>"WrtRootf(ComprLev):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>$shf);
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$IOPAcmval[3])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>$shf);
  $xpos+=$entw;
#---
  $labw=0.13;
  $sdset_fram->Label(-text=>"Objects:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>$shf);
  $xpos+=$labw;
#-
  $menw=0.23;
  $optmenu1=$sdset_fram->Optionmenu(-textvariable => \$ObjWriteMode, -variable => \$ObjWriteVal,
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$ObjWriteModes[0],0],[$ObjWriteModes[1],1],[$ObjWriteModes[2],2]],
#			       -command => sub{print "text/var=",$ObjWriteMode," ",$IOPAcmval[0],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#---
  $butw=0.17;
  $sdset_fram->Checkbutton(-text=>"+Header", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$Wr2RootfCond1)
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
  $butw=0.15;
  $sdset_fram->Checkbutton(-text=>"+ErrorEvs", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$Wr2RootfCond2)
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#------
  $labw=0.25;
  $entw=0.75;
  $xpos=0;
  $sdset_fram->Label(-text=>"EventWrCondition:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$IOPAcmval[1])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$entw;
#------
  $labw=0.18;
  $entw=0.15;
  $xpos=0;
  $sdset_fram->Label(-text=>"RootFName:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$IOPAcmval[4])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#---
  $labw=0.18;
  $entw=0.14;
  $sdset_fram->Label(-text=>"MaxEvs/File:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$IOPAcmval[2])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#---
  $labw=0.22;
  $entw=0.13;
  $sdset_fram->Label(-text=>"MaxFileSize(kb):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$IOPAcmval[5])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#------
  $butw=0.2;
  $xpos=0;
#
  if($SessName eq "RDR" || $SessName eq "MCR"){
    $sdset_fram->Checkbutton(-text=>"ReadDaqF", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$ReadDaqFlg)
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+3*$drh));
    $xpos+=$butw;
  }
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+3*$drh));
  }#-->endof page-1
}
#------ MC:
elsif($RecoSimuP eq "SIMU"){
  if($PageNumb eq "Page1"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify Page-1 SIMU-params for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nlmc=3;
  $drh=(1.-$shf)/$nlmc;
#---
  $butw=0.21;
  $xpos=0;
#
  $sdset_fram->Radiobutton(-text=>"NotWriteDaqF",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>0, -variable=>\$WriteDaqFlg)
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#-
  $sdset_fram->Radiobutton(-text=>"WriteF(norepl)",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>1, -variable=>\$WriteDaqFlg)
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#-
  $sdset_fram->Radiobutton(-text=>"WriteF(append)",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>2, -variable=>\$WriteDaqFlg)
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#---
  $labw=0.19;
  $entw=0.18;
  $sdset_fram->Label(-text=>"DAQfileName:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$DAQCcmval[1])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+2*$drh));
  }#-->endof page-1
}
}
#-----------------------------------------------
sub SetAMSPars{
  my $i,$j;
  my $shf,$nl,$nlmc,$drh,$labw,$entw,$rwd,$xpos;
  my $butw;
#------
$sdset_fram->destroy() if Tk::Exists($sdset_fram);
#------
if($RecoSimuP eq "RECO"){
  if($PageNumb eq "Page1"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify Page-1 RECO-Params for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
  $xpos=0;
  $labw=0.22;
  $sdset_fram->Label(-text=>"IrradiationMode:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#
  $butw=0.1;
  $sdset_fram->Radiobutton(-text=>"Space",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>0, -variable=>\$MISCcmval[0])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#
  $butw=0.1;
  $sdset_fram->Radiobutton(-text=>"Earth",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>1, -variable=>\$MISCcmval[0])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#
  $butw=0.1;
  $sdset_fram->Radiobutton(-text=>"TBeam",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>2, -variable=>\$MISCcmval[0])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#---
  $labw=0.15;
  $sdset_fram->Label(-text=>"MagnState:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#
  $butw=0.11;
  $sdset_fram->Radiobutton(-text=>"Warm",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>-1, -variable=>\$MAGScmval[0])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#
  $butw=0.1;
  $sdset_fram->Radiobutton(-text=>"Cold",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>0, -variable=>\$MAGScmval[0])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#
  $butw=0.12;
  $sdset_fram->Radiobutton(-text=>"Charged",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>1, -variable=>\$MAGScmval[0])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#-----
  my @detdaqnames=qw(Levl Emc Rich Tof+Acc Trd Trk);
#  @DetInDaq=();#def.setting is done in "Welcome"-routine
  @P2DetInDaqBut=();
  $labw=0.2;
  $butw=(1-$labw)/6;;
  $xpos=0;
  $sdset_fram->Label(-text=>"DetInDaq:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
  for($i=0;$i<6;$i++){#Lev1 come first
    $P2DetInDaqBut[$i]=$sdset_fram->Checkbutton(-text=>$detdaqnames[$i], -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$DetInDaq[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+$drh));
    $xpos+=$butw;
  }
#-----
  $labw=0.25;
  $entw=0.15;
  $xpos=0;
  $sdset_fram->Label(-text=>"InputDataType:",-font=>$LablWFont,-relief=>'groove')
                                               ->place(
					       -relwidth=>$labw, -relheight=>$drh,
                                               -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Optionmenu(-textvariable => \$RunTypeText, 
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [@DataTypes],
			       -command => sub{$RunType=$RunTypeText;}
	                       )
                               ->place(
                               -relwidth=>$entw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#---
  $labw=0.2;
  $entw=0.04;
  $sdset_fram->Label(-text=>"DebugMode:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$AMSJOBcmval[0])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#------
  $labw=0.2;
  $entw=0.2;
  $xpos=0;
  $sdset_fram->Label(-text=>"JobName:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$AMSJOBcmval[1])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$entw;
#---
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+3*$drh));
  }#-->endof page-1
}
#------ MC:
elsif($RecoSimuP eq "SIMU"){
  if($PageNumb eq "Page1"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify Page-1 SIMU-params for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nlmc=3;
  $drh=(1.-$shf)/$nlmc;
#---
  $labw=0.19;
  $entw=0.05;
  $xpos=0;
  $sdset_fram->Label(-text=>"G3-mode:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$MISCcmval[1])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#---
  $labw=0.19;
  $entw=0.05;
  $sdset_fram->Label(-text=>"G4-mode:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$MISCcmval[2])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#---
  $labw=0.4;
  $entw=0.12;
  $sdset_fram->Label(-text=>"MagMed e(ph)-thr(gev):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$MAGScmval[1])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+2*$drh));
  }#-->endof page-1
}
}
#-----------------------------------------------
sub SetLvl1Pars{
  my $i,$j;
  my $shf,$nl,$nlmc,$drh,$labw,$entw,$rwd,$xpos,$butw,$menw;
  my $ar;
  my $optmenu1;
#------
$sdset_fram->destroy() if Tk::Exists($sdset_fram);
#------
if($RecoSimuP eq "RECO"){
  if($PageNumb eq "Page1"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify Page-1 RECO-Params for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
  $labw=0.2;
  $xpos=0;
  $sdset_fram->Label(-text=>"PrintMode:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>$shf);
  $xpos+=$labw;
#-
  $menw=0.25;
  $optmenu1=$sdset_fram->Optionmenu(-textvariable => \$Tgl1PrtName, -variable => \$Tgl1PrtFlg,
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$Tgl1PrtNames[0],0],[$Tgl1PrtNames[1],1],[$Tgl1PrtNames[2],2]],
#			       -command => sub{print "text/var=",$Tgl1PrtName," ",$Tgl1PrtFlg,"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#---
  $labw=0.2;
  $sdset_fram->Label(-text=>"DebugMode:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>$shf);
  $xpos+=$labw;
#-
  $menw=0.25;
  $optmenu1=$sdset_fram->Optionmenu(-textvariable => \$Tgl1DebName, -variable => \$Tgl1DebFlg,
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$Tgl1DebNames[0],0],[$Tgl1DebNames[1],1],[$Tgl1DebNames[2],2],
			                    [$Tgl1DebNames[3],3],[$Tgl1DebNames[4],4]],
#			       -command => sub{print "text/var=",$Tgl1DebName," ",$Tgl1DebFlg,"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#------
  $labw=0.3;
  $xpos=0;
  $sdset_fram->Label(-text=>"WriteSetupUpdatesTo:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
#-
  $menw=0.2;
  $optmenu=$sdset_fram->Optionmenu(-textvariable => \$Tgl1SetupDestName, -variable => \$TGL1cmval[19],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$Tgl1SetupDestNames[0],0],[$Tgl1SetupDestNames[1],1],[$Tgl1SetupDestNames[2],2]],
#			       -command => sub{print "text/var=",$Tgl1SetupDestName," ",$TGL1cmval[19],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$menw;
#---
  my @lv1dbdatatypes=qw(RD_TrigConf);
  $labw=0.14;
  $butw=0.2;
  $sdset_fram->Label(-text=>"UseDB4:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
  for($i=0;$i<1;$i++){
    $sdset_fram->Checkbutton(-text=>$lv1dbdatatypes[$i], -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$Lv1DBUsePatt[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+1*$drh));
    $xpos+=$butw;
  }
#-------
  $xpos=0;
  $labw=0.2;
  $sdset_fram->Label(-text=>"TrigConfFile:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
#
  $butw=0.1;
  $sdset_fram->Radiobutton(-text=>"Offic",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>0, -variable=>\$Lv1OffPrivFlg)
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$butw;
#
  $butw=0.1;
  $sdset_fram->Radiobutton(-text=>"Priv",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>1, -variable=>\$Lv1OffPrivFlg)
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$butw;
#---
  $labw=0.28;
  $entw=0.25;
  $sdset_fram->Label(-text=>"DefLvl1Config(file):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TGL1cmval[16])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#--------
  $labw=0.27;
  $entw=0.09;
  $xpos=0;
  $sdset_fram->Label(-text=>"MinLiveTime:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TGL1cmval[15])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$entw;
#---
  $labw=0.27;
  $entw=0.12;
  $sdset_fram->Label(-text=>"MaxScalRate:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TGL1cmval[14])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$entw;
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+3*$drh));
  }#-->endof page-1
}
#------ MC:
elsif($RecoSimuP eq "SIMU"){
  if($PageNumb eq "Page1"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify Page-1 SIMU-params for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nlmc=4;
  $drh=(1.-$shf)/$nlmc;
#------
  $labw=0.1;
  $butw=(1.-$labw)/$TrigPhysMembs;
  $xpos=0;
  $sdset_fram->Label(-text=>"Trig:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#
  for($i=0;$i<$TrigPhysMembs;$i++){
    $TrigPhysmVals[$i]=0;
    if(($TrigPhysmPatt & (1<<$i))>0){$TrigPhysmVals[$i]=1;}
    $sdset_fram->Checkbutton(-text=>$TrigPhysmNams[$i], -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TrigPhysmVals[$i])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
    $xpos+=$butw;
  }
#------
  $labw=0.18;
  $xpos=0;
  $sdset_fram->Label(-text=>"TofLays:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
  $menw=0.14;
  $optmenu1=$sdset_fram->Optionmenu(-textvariable => \$TofLayConfDispl, -variable => \$TGL1cmval[1],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["def",-1],["3o4",0],["4o4",1],["T&B",2],["2o4",3]],
#			       -command => sub{print "text/var=",$TofLayConfDispl," ",$TGL1cmval[1],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$menw;
#---
  $labw=0.18;
  $menw=0.14;
  $sdset_fram->Label(-text=>"TofLaysI:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
  $optmenu1=$sdset_fram->Optionmenu(-textvariable => \$TofLayConfIDispl, -variable => \$TGL1cmval[4],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["def",-1],["3o4",0],["4o4",1],["T&B",2]],
#			       -command => sub{print "text/var=",$TofLayConfIDispl," ",$TGL1cmval[4],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$menw;
#---
  $labw=0.183;
  $menw=0.177;
  $sdset_fram->Label(-text=>"TofLaysSI:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$labw;
  $optmenu1=$sdset_fram->Optionmenu(-textvariable => \$TofLayConfSIDispl, -variable => \$TGL1cmval[3],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["def",-1],["aT&Ba",0],["aT&Bo",1],["oT&Ba",2],["oT&Bo",2]],
#			       -command => sub{print "text/var=",$TofLayConfSIDispl," ",$TGL1cmval[3],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+$drh));
  $xpos+=$menw;
#------
  $labw=0.25;
  $xpos=0;
  $sdset_fram->Label(-text=>"DefLvl1Config(file):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $menw=0.25;
  $optmenu1=$sdset_fram->Optionmenu(-textvariable => \$Tgl1MCConfName, -variable => \$TGL1cmval[12],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$Tgl1MCConfNames[0],1],[$Tgl1MCConfNames[1],2],[$Tgl1MCConfNames[2],3]],
#			       -command => sub{print "text/var=",$Tgl1MCConfName," ",$TGL1cmval[12],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$menw;
#---
  $butw=0.2;
  $sdset_fram->Checkbutton(-text=>"PrescalingOFF", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TGL1cmval[20])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$butw;
#------
  $labw=0.3;
  $entw=0.06;
  $xpos=0;
  $sdset_fram->Label(-text=>"MaxAccS(eq/pol):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TGL1cmval[6])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$entw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TGL1cmval[7])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+3*$drh));

#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+($nlmc-1)*$drh));
  }#-->endof page-1
}
}
#----------------------------------------------
sub SetEvSelPars{
  my $i,$j;
  my $shf,$nl,$nlmc,$drh,$labw,$entw,$rwd,$xpos,$butw,$menw;
  my $ar;
  my $optmenu1;
#------
$sdset_fram->destroy() if Tk::Exists($sdset_fram);
#------
if($RecoSimuP eq "RECO"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify RECO-Params on $PageNumb for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
  if($PageNumb eq "Page1"){
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
  $labw=0.15;
  $xpos=0;
  $sdset_fram->Label(-text=>"Nparticles:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#--
  $menw=0.1;
  $sdset_fram->Optionmenu(-textvariable => \$NPartName, -variable => \$ESTAcmval[0],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$NPartNames[0],0],[$NPartNames[1],10],[$NPartNames[2],100],
			               [$NPartNames[3],1000],[$NPartNames[4],1110],[$NPartNames[5],1100]],
			       -command => sub{print "text/var=",$NPartName," ",$ESTAcmval[0],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#---
  my @SelObjNames=qw(TrdTr TofCl TrkTr RichR EcShw Vertx);
  $SelObjMembs=scalar(@SelObjNames);
  $labw=0.14;
  $butw=(1.-$labw-$xpos)/$SelObjMembs;
  $sdset_fram->Label(-text=>"PartMemb:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#
  for($i=0;$i<$SelObjMembs;$i++){
    $sdset_fram->Checkbutton(-text=>$SelObjNames[$i], -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
						 -onvalue=>10,
                                                 -variable=>\$ESTAcmval[1+$i],
						 )
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
    $xpos+=$butw;
  }
#------
  $labw=0.15;
  $menw=0.1;
  $xpos=0;
  $sdset_fram->Label(-text=>"NTrdTr:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#--
  $sdset_fram->Optionmenu(-textvariable => \$NTrdTrName, -variable => \$ESTAcmval[7],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["Any",0],[">0",1110],["1",10],[">1",1100]]
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$menw;
#---
  $sdset_fram->Label(-text=>"NTofCl:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#--
  $sdset_fram->Optionmenu(-textvariable => \$NTofClName, -variable => \$ESTAcmval[8],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["Any",0],[">0",11111110],[">1",11111100],[">2",11111000]]
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$menw;
#---
  $sdset_fram->Label(-text=>"NTrkTr:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#--
  $sdset_fram->Optionmenu(-textvariable => \$NTrkTrName, -variable => \$ESTAcmval[9],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["Any",0],[">0",1110],["1",10],[">1",1100]]
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$menw;
#---
  $sdset_fram->Label(-text=>"NRichR:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#--
  $sdset_fram->Optionmenu(-textvariable => \$NRichRName, -variable => \$ESTAcmval[10],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["Any",0],[">0",1110],["1",10],[">1",1100]]
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$menw;
#------
  $xpos=0;
  $sdset_fram->Label(-text=>"NEcSh:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
#--
  $sdset_fram->Optionmenu(-textvariable => \$NEcShwName, -variable => \$ESTAcmval[11],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["Any",0],[">0",1110],["1",10],[">1",1100]]
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$menw;
#---
  $sdset_fram->Label(-text=>"NVertx:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
#--
  $sdset_fram->Optionmenu(-textvariable => \$NVertxName, -variable => \$ESTAcmval[12],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["Any",0],[">0",1110],["1",10],[">1",1100]]
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$menw;
#---
  $sdset_fram->Label(-text=>"NAccS:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
#--
  $sdset_fram->Optionmenu(-textvariable => \$NAccSrName, -variable => \$ESTAcmval[13],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["Any",0],[">0",1110],[">1",1100],[">2",1000]]
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$menw;
#---
  $sdset_fram->Label(-text=>"Charge:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
#--
  $sdset_fram->Optionmenu(-textvariable => \$NChargeName, -variable => \$ESTAcmval[14],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["Any",0],[">0",11111110],["1",10],[">1",11111100],["2",100],[">2",11111000],
			                    ["3",1000],["4",10000],["5",100000],["6",1000000],[">=7",10000000]]
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$menw;
#------
  $labw=0.4;
  $menw=0.1;
  $xpos=0;
#--
  $sdset_fram->Label(-text=>"Lev1-TofLayConfig -> Flag'z>=1':",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$labw;
#--
  $sdset_fram->Optionmenu(-textvariable => \$Conf1Name, -variable => \$ESTAcmval[15],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["Any",0],["4of4",1000],[">=3",1100],[">=2",1110],["2",10]]
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$menw;
#---
  $labw=0.15;
  $menw=0.1;
#--
  $sdset_fram->Label(-text=>"Flag'z>=2':",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$labw;
#--
  $sdset_fram->Optionmenu(-textvariable => \$Conf2Name, -variable => \$ESTAcmval[16],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["Any",0],["4of4",1000],[">=3",1100],[">=2",1110],["2",10]]
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$menw;
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+($nl-1)*$drh));
  }#-->endof page-1
#------
  elsif($PageNumb eq "Page2"){#<--- Page-2
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
  $labw=0.28;
  $entw=0.22;
  $xpos=0;
  $sdset_fram->Label(-text=>"RunNumberToFind:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$SELEcmval[0])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#---
  $labw=0.28;
  $entw=0.22;
  $sdset_fram->Label(-text=>"EventNumberToFind:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$SELEcmval[1])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#------
  $labw=0.4;
  $entw=0.4;
  $xpos=0;
  $sdset_fram->Label(-text=>"OR Use EventsList from FileName:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$SELEcmval[2])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$entw;
#------
  $labw=0.2;
  $entw=0.2;
  $xpos=0;
  $sdset_fram->Label(-text=>"LastRunNumb:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$SELEcmval[3])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#---
  $labw=0.2;
  $entw=0.2;
  $sdset_fram->Label(-text=>"LastEventNumb:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$SELEcmval[4])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+($nl-1)*$drh));
  }#-->endof page-2
}
}
#----------------------------------------------
sub SetTrkPars{
  my $i,$j;
  my $shf,$nl,$nlmc,$drh,$labw,$entw,$rwd,$xpos,$butw,$menw;
  my $ar;
  my $optmenu1;
#------
  $sdset_fram->destroy() if Tk::Exists($sdset_fram);
#------
if($RecoSimuP eq "RECO"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify RECO-Params on $PageNumb for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
  if($PageNumb eq "Page1"){#<--- page-1
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
  $labw=0.18;
  $xpos=0;
  $sdset_fram->Label(-text=>"TofCls2Match:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#-
  $entw=0.03;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TRFIcmval[0])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#---
  $labw=0.18;
  $sdset_fram->Label(-text=>"MxChi2FastF:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#-
  $entw=0.08;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TRFIcmval[1])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#-
  $labw=0.2;
  $sdset_fram->Label(-text=>"MxChi2StLineF:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#-
  $entw=0.07;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TRFIcmval[2])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#---
  $labw=0.18;
  $sdset_fram->Label(-text=>"MxChi2noMS:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#-
  $entw=0.08;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TRFIcmval[3])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#------
  $labw=0.18;
  $xpos=0;
  $sdset_fram->Label(-text=>"MinRigid(Gv):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#-
  $entw=0.07;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TRFIcmval[4])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$entw;
#---
  $butw=0.16;
  $sdset_fram->Checkbutton(-text=>"UseUsedCls", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TRFIcmval[5])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$butw;
#---
  $labw=0.23;
  $sdset_fram->Label(-text=>"FastTrackingMode:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#--
  $menw=0.19;
  $sdset_fram->Optionmenu(-textvariable => \$FastTrkMode, -variable => \$TRFIcmval[6],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$FastTrkModes[0],4],[$FastTrkModes[1],3],
			               [$FastTrkModes[2],2],[$FastTrkModes[3],1],[$FastTrkModes[4],0]],
			       -command => sub{print "text/var=",$FastTrkMode," ",$TRFIcmval[6],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$menw;
#---
  $butw=0.17;
  $sdset_fram->Checkbutton(-text=>"UseWeekCls", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TRFIcmval[7])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$butw;
#------
  $xpos=0;
  $butw=0.16;
  $sdset_fram->Checkbutton(-text=>"accept 4y+3x", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TRFIcmval[8])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$butw;
#---
  $labw=0.24;
  $sdset_fram->Label(-text=>"3PntsCase_MxChi2:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
#-
  $entw=0.06;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TRFIcmval[9])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#---
  $butw=0.18;
  $sdset_fram->Checkbutton(-text=>"AddFalseTofX", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TRFIcmval[10])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$butw;
#---
  $butw=0.17;
  $sdset_fram->Checkbutton(-text=>"Use 4y+TofX", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TRFIcmval[11])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$butw;
#---
  $butw=0.19;
  $sdset_fram->Checkbutton(-text=>"NoTofInconHit", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TRFIcmval[12])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$butw;
#-------
  $butw=0.15;
  $xpos=0;
  $sdset_fram->Checkbutton(-text=>"UseAdvFit", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TRFIcmval[13])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$butw;
#---
  $labw=0.2;
  $sdset_fram->Label(-text=>"Max Hits/Layer:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$labw;
#-
  $entw=0.06;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TRFIcmval[14])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$entw;
#---
  $butw=0.12;
  $sdset_fram->Checkbutton(-text=>"UseTRD", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TRFIcmval[15])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$butw;
#---
  $butw=0.27;
  $sdset_fram->Checkbutton(-text=>"Accept >=5plns Tracks", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TRFIcmval[16])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$butw;
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.2, -relheight=>$drh,  
                                          -relx=>0.8, -rely=>($shf+($nl-1)*$drh));
 }
#----------
  elsif($PageNumb eq "Page2"){#<--- page-2
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
  $butw=0.22;
  $xpos=0;
  $sdset_fram->Checkbutton(-text=>"2TrksVertexOnly", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$TRFIcmval[17])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$butw;
#---
  $labw=0.2;
  $sdset_fram->Label(-text=>"TrkFitMethod:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#-
  $menw=0.13;
  $sdset_fram->Optionmenu(-textvariable => \$TrkFitMethod, -variable => \$TRFIcmval[18],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$TrkFitMethods[0],0],[$TrkFitMethods[1],1]],
#			       -command => sub{print "text/var=",$TrkFitMethod," ",$TRFIcmval[18],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#---
  $labw=0.28;
  $sdset_fram->Label(-text=>"TrkPattRecognMethod:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#-
  $menw=0.17;
  $sdset_fram->Optionmenu(-textvariable => \$TrkPattRecMethod, -variable => \$TRFIcmval[19],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$TrkPattRecMethods[0],0],[$TrkPattRecMethods[1],1]],
#			       -command => sub{print "text/var=",$TrkPattRecMethod," ",$TRFIcmval[19],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#------
  $labw=0.15;
  $xpos=0;
  $sdset_fram->Label(-text=>"MainAlg:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#-
  $entw=0.08;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$TRFIcmval[20])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$entw;
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.2, -relheight=>$drh,  
                                          -relx=>0.8, -rely=>($shf+($nl-1)*$drh));
  }#-->endof page-2
}
#------ MC:
elsif($RecoSimuP eq "SIMU"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify SIMU-params on $PageNumb for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
  if($PageNumb eq "Page1"){#<--- page-1
#---
  $shf=0.17;
  $nlmc=3;
  $drh=(1.-$shf)/$nlmc;
#---
  }#-->endof page-1
}
#
}
#----------------------------------------------
sub SetTrdPars{
  my $i,$j;
  my $shf,$nl,$nlmc,$drh,$labw,$entw,$rwd,$xpos,$butw,$menw;
  my $ar;
  my $optmenu1;
#------
  $sdset_fram->destroy() if Tk::Exists($sdset_fram);
if($RecoSimuP eq "RECO"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify RECO-Params on $PageNumb for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
}
#------ MC:
elsif($RecoSimuP eq "SIMU"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify SIMU-params on $PageNumb for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nlmc=3;
  $drh=(1.-$shf)/$nlmc;
#---
}
#
}
#----------------------------------------------
sub SetRichPars{
  my $i,$j;
  my $shf,$nl,$nlmc,$drh,$labw,$entw,$rwd,$xpos,$butw,$menw;
  my $ar;
  my $optmenu1;
#------
  $sdset_fram->destroy() if Tk::Exists($sdset_fram);
if($RecoSimuP eq "RECO"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify RECO-Params on $PageNumb for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
}
#------ MC:
elsif($RecoSimuP eq "SIMU"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify SIMU-params on $PageNumb for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nlmc=3;
  $drh=(1.-$shf)/$nlmc;
#---
}
#
}
#----------------------------------------------
sub SetZFitPars{
  my $i,$j;
  my $shf,$nl,$nlmc,$drh,$labw,$entw,$rwd,$xpos,$butw,$menw;
  my $ar;
  my $optmenu1;
#------
$sdset_fram->destroy() if Tk::Exists($sdset_fram);
#------
if($RecoSimuP eq "RECO"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify RECO-Params on $PageNumb for Particle Z-fit :",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
 if($PageNumb eq "Page1"){#<--- page-1
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
  $labw=0.2;
  $menw=0.13;
  $xpos=0;
#-
  $sdset_fram->Label(-text=>"TrkProjUse:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Optionmenu(-textvariable => \$TrkPrjName, -variable => \$CHARcmval[0],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$TrkPrjNames[0],0],[$TrkPrjNames[1],1],[$TrkPrjNames[2],2]]
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#---
  $labw=0.25;
  $entw=0.08;
#-
  $sdset_fram->Label(-text=>"ClustEtaMin(x/y):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[1])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[2])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#---
  $labw=0.1;
  $sdset_fram->Label(-text=>"Max:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[3])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[4])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#---
  $labw=0.15;
  $entw=0.06;
  $xpos=0;
#-
  $sdset_fram->Label(-text=>"RefitProb <",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[5])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$entw;
#---
  $labw=0.19;
  $entw=0.08;
#-
  $sdset_fram->Label(-text=>"TrunMeanAlg:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#-
  $butw=0.15;
  $sdset_fram->Radiobutton(-text=>"MaxClExcl",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>0, -variable=>\$CHARcmval[10])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$butw;
#-
  $sdset_fram->Radiobutton(-text=>"IncClExcl",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>1, -variable=>\$CHARcmval[10])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$butw;
#---
  $labw=0.2;
  $entw=0.05;
#-
  $sdset_fram->Label(-text=>"ExclThrTof/Trk:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[6])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$entw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[7])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$entw;
#---
  $labw=0.25;
  $entw=0.06;
  $xpos=0;
#-
  $sdset_fram->Label(-text=>"EclRelSpread mn/mx:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[8])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[9])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#---
  $labw=0.15;
  $entw=0.08;
#-
  $sdset_fram->Label(-text=>"OkZProb >",-font=>$LablWFont,-relief=>'groove',-foreground=>'red')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[11])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#---
  $labw=0.17;
#-
  $sdset_fram->Label(-text=>"TrkPdfFrom:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
#-
  $butw=0.06;
  $sdset_fram->Radiobutton(-text=>"DB",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>0, -variable=>\$CHARcmval[18])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$butw;
#-
  $butw=0.1;
  $sdset_fram->Radiobutton(-text=>"File#:",-font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>1, -variable=>\$CHARcmval[18])
                                                 ->place(
						      -relwidth=>$butw, -relheight=>$drh,
						      -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$butw;
#-
  $entw=0.07;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[17])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#------
  $labw=0.2;
  $entw=0.045;
  $xpos=0;
#-
  $sdset_fram->Label(-text=>"UseTrkIfTofZ >",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[12])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$entw;
#---
  $labw=0.2;
  $entw=0.045;
#-
  $sdset_fram->Label(-text=>"UseTrkProbIfZ >",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[15])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$entw;
#---
  $butw=0.16;
  $sdset_fram->Checkbutton(-text=>"(x+y)TrkFit", -font=>$ButtWFont, -indicator=>0,
                                                 -borderwidth=>3,-relief=>'raised',
						 -selectcolor=>orange,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$CHARcmval[13])
					         ->place(
                                                 -relwidth=>$butw, -relheight=>$drh,
						 -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$butw;
#-
  $labw=0.07;
  $entw=0.07;
#-
  $sdset_fram->Label(-text=>"Ex/y:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[14])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$entw;
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.2, -relheight=>$drh,  
                                          -relx=>0.8, -rely=>($shf+($nl-1)*$drh));
 }#--->endof page-1
 elsif($PageNumb eq "Page2"){#<--- page-2
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
 }#-->endof page-2
}
#------ MC:
elsif($RecoSimuP eq "SIMU"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify SIMU-params on $PageNumb for Particle Z-fit:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
  if($PageNumb eq "Page1"){#<--- page-1
#---
  $shf=0.17;
  $nlmc=3;
  $drh=(1.-$shf)/$nlmc;
#---
  $labw=0.3;
  $entw=0.07;
  $xpos=0;
#-
  $sdset_fram->Label(-text=>"IfNoDB->TrkDefPdfFile#:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
#-
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CHARcmval[16])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.2, -relheight=>$drh,  
                                          -relx=>0.8, -rely=>($shf+($nlmc-1)*$drh));
  }#-->endof page-1
}
#
}
#----------------------------------------------
sub SetLvl3Pars{
  my $i,$j;
  my $shf,$nl,$nlmc,$drh,$labw,$entw,$rwd,$xpos,$butw,$menw;
  my $ar;
  my $optmenu1;
#------
$sdset_fram->destroy() if Tk::Exists($sdset_fram);
#------
if($RecoSimuP eq "RECO"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify RECO-Params on $PageNumb for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nl=4;
  $drh=(1.-$shf)/$nl;
#---
}
#------ MC:
elsif($RecoSimuP eq "SIMU"){
  $sdset_fram->destroy() if Tk::Exists($sdset_fram);
  $sdset_fram=$set_fram->Frame(-label=>"Modify SIMU-params on $PageNumb for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  $shf=0.17;
  $nlmc=3;
  $drh=(1.-$shf)/$nlmc;
#---
}
#
}
#----------------------------------------------
sub SetMCGenPars{
  my $i,$j;
  my $shf,$nl,$nlmc,$drh,$labw,$entw,$rwd,$xpos,$butw,$menw;
  my $optmenu1;
#------
$sdset_fram->destroy() if Tk::Exists($sdset_fram);
#------
if($RecoSimuP eq "SIMU"){
  $sdset_fram=$set_fram->Frame(-label=>"Modify SIMU-Pars on $PageNumb for selected group:",-relief=>'groove',
                                                  -borderwidth=>5,
                                                  -background => "gray")
						  ->place(
                                                  -relwidth=>1, -relheight=>$sdframhig,
                                                  -relx=>0, -rely=>$sdframpos);
#---
  if($PageNumb eq "Page1"){
#---
  $shf=0.17;
  $nlmc1=4;
  $drh=(1.-$shf)/$nlmc1;
#------
  $labw=0.12;
  $xpos=0;
  $sdset_fram->Label(-text=>"Part:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  $menw=0.14;
  $optmenu=$sdset_fram->Optionmenu(-textvariable => \$PartName, -variable => \$KINEcmval[0],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["Phot",1],["e+",2],["e-",3],["mu+",5],["mu-",6],["pi0",7],["pi+",8],
			                    ["pi-",9],["Neut",13],["Prot",14],["aPr",15],["aNeu",25],["Deut",45],["Trit",46],
					    ["He4",47],["He3",49],["Li6",61],["Be9",64],["Be10",65],["C12",67],["N14",68],
					    ["O16",69],["F19",70],["Fe56",68]],
#			       -command => sub{print "PartInd=",$KINEcmval[0],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#---
  $optmenu1=$sdset_fram->Optionmenu(-textvariable => \$EmcDebName, -variable => \$ECREcmval[1],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$EmcDebNames[0],0],[$EmcDebNames[1],1],[$EmcDebNames[2],2]],
#			       -command => sub{print "text/var=",$EmcDebName," ",$ECREcmval[1],"\n";}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
#--------
  $labw=0.18;
  $sdset_fram->Label(-text=>"Spectrum:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  $menw=0.2;
  $optmenu=$sdset_fram->Optionmenu(-textvariable => \$EnSpectr, -variable => \$MCGENcmval[17],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [[$EnSpectra[0],0],[$EnSpectra[1],1],[$EnSpectra[2],2],[$EnSpectra[3],3],
			                    [$EnSpectra[4],4],[$EnSpectra[5],5]],
#			       -command => sub{}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#---
  $labw=0.15;
  $sdset_fram->Label(-text=>"MomR:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  $entw=0.105;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$MCGENcmval[12])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
  $entw=0.105;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$MCGENcmval[13])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#------
  $labw=0.15;
  $xpos=0;
  $sdset_fram->Label(-text=>"Illumin:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
  $menw=0.25;
  $optmenu=$sdset_fram->Optionmenu(-textvariable => \$IllumType,
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [@IllumTypes],
			       -command => \&SetIllLimits
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$menw;
#---
  $labw=0.15;
  $sdset_fram->Label(-text=>"ImpThe:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#
  $entw=0.08;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$ImpAngThe)
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$entw;
#
  $labw=0.15;
  $sdset_fram->Label(-text=>"ImpPhi:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
#
  $entw=0.08;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$ImpAngPhi)
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$entw;
#------
  $IPointX="0.";
  $IPointY="0.";
  $IPointZ="195.";
  $labw=0.2;
  $xpos=0;
  $sdset_fram->Label(-text=>"ImpactXYZ:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
#
  $entw=0.1;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$IPointX)
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$IPointY)
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$IPointZ)
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#---
  $labw=0.2;
  $sdset_fram->Label(-text=>"ImpArea(+-):",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$labw;
#
  $entw=0.1;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$HAreaX)
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$HAreaY)
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$HAreaZ)
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+2*$drh));
  $xpos+=$entw;
#------
  $labw=0.18;
  $xpos=0;
  $sdset_fram->Label(-text=>"FocusOn:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$labw;
  $menw=0.22;
  $optmenu=$sdset_fram->Optionmenu(-textvariable => \$FocusOn, -variable => \$MCGENcmval[21],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["NoFocus",0],["Ecal",1],["TrdTopHC",2],["SolarPanel",3]],
#			       -command => sub{}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$menw;
#---
  $labw=0.15;
  $sdset_fram->Label(-text=>"RunNum:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$labw;
  $entw=0.1;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$MCGENcmval[16])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+3*$drh));
  $xpos+=$entw;
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+($nlmc1-1)*$drh));
  }#-->endof page-1
#------
  elsif($PageNumb eq "Page2"){
#--
  $shf=0.17;
  $nlmc2=4;
  $drh=(1.-$shf)/$nlmc2;
#---
  $labw=0.2;
  $xpos=0;
  $sdset_fram->Label(-text=>"GamSource:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  $menw=0.2;
  $optmenu=$sdset_fram->Optionmenu(-textvariable => \$GamSource, -variable => \$GMSRCcmval[0],
                               -background=>yellow,
                               -activebackground=>yellow,
			       -relief=>'sunken',
			       -borderwidth=>2,
                               -font=>$MenuWFont,
                               -options => [["None",0],["User's",1],["Crab",2],["GalCentr",3],["Vela",4]],
#			       -command => sub{}
	                       )
                               ->place(
                               -relwidth=>$menw, -relheight=>$drh,  
                               -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$menw;
#---
  $labw=0.15;
  $sdset_fram->Label(-text=>"RNDM:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$labw;
  $entw=0.225;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$RNDMcmval[0])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
  $entw=0.225;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$RNDMcmval[1])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+0*$drh));
  $xpos+=$entw;
#------
  $labw=0.4;
  $xpos=0;
  $sdset_fram->Label(-text=>"Cuts(g,e,nh,ch,mu)[gev]:",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+1*$drh));
  $xpos+=$labw;
  $entw=0.12;
  for($i=0;$i<5;$i++){
    $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$CUTScmval[$i])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+1*$drh));
    $xpos+=$entw;
  }
#------
  $labw=0.25;
  $xpos=0;
  $sdset_fram->Label(-text=>"DebugPrt(each",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+($nlmc2-1)*$drh));
  $xpos+=$labw;
  $entw=0.12;
  $sdset_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$EntrWFont,
                                              -textvariable=>\$DEBUGcmval[2])
					      ->place(
                                              -relwidth=>$entw, -relheight=>$drh,  
                                              -relx=>$xpos, -rely=>($shf+($nlmc2-1)*$drh));
  $xpos+=$entw;
  $labw=0.13;
  $sdset_fram->Label(-text=>"events)",-font=>$LablWFont,-relief=>'groove')
                                                ->place(
						-relwidth=>$labw, -relheight=>$drh,
                                                -relx=>$xpos, -rely=>($shf+($nlmc2-1)*$drh));
  $xpos+=$labw;
#------
  $sdset_fram->Button(-text=>"Reset2DefPar", -font=>$ButtWFont, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&ResetDC2Defs)
			                  ->place(
                                          -relwidth=>0.25, -relheight=>$drh,  
                                          -relx=>0.75, -rely=>($shf+($nlmc2-1)*$drh));
  }#-->endof page-2
#
}
#
  $mwnd->update;
}
#---------------------
sub SetIllLimits{
  if($IllumType eq $IllumTypes[0]){#top hemisphera
    $MCGENcmval[11]=0.;
    $MCGENcmval[14]=0;#plane=0 to be shure
  }
  elsif($IllumType eq $IllumTypes[1]){#bot hemisphera
    $MCGENcmval[8]=0.;
    $MCGENcmval[14]=0;#plane=0 to be shure
  }
  elsif($IllumType eq $IllumTypes[2]){# Fixed Dir&Point
    $HAreaX="0.";
    $HAreaY="0.";
    $HAreaZ="0.";
    $ImpAngThe="180.";
    $ImpAngPhi="0.";
    $MCGENcmval[14]=0;#plane=0 to be shure
  }
  elsif($IllumType eq $IllumTypes[3]){#AMSCubeTop
    $MCGENcmval[11]=-0.7;
    $MCGENcmval[14]=1;#AMS-cube top plane
  }
#
  $mwnd->update;
}
#---------------------
sub ResetDC2Defs{
  my $i,$j;
#--->gr-2
  if($DataGroup eq "TOF"){
    if($RecoSimuP eq "RECO"){
      if($PageNumb eq "Page1"){
        for($i=0;$i<$TFREparsN;$i++){$TFREcmval[$i]=$TFREcmdf[$i];}
        for($i=0;$i<$TFCAparsN;$i++){$TFCAcmval[$i]=$TFCAcmdf[$i];}
        $TofRefCalSetRD=$TFREcmdf[12];
#
        $j=1;
        for($i=0;$i<5;$i++){
          $TofDBUsePatt[$i]=1-(($TFREcmval[11]/$j)%10);# 10-base BitPattern Msb(tdcl elos dcrd ped cal)Lsb (=1 to use)
          $j=10*$j;
        }
      }#--->endof page-1
      elsif($PageNumb eq "Page2"){
        for($i=0;$i<$BETAFITparsN;$i++){$BETAFITcmval[$i]=$BETAFITcmdf[$i];}
      }#--->endof page-2
    }
#--
    else{
      if($PageNumb eq "Page1"){
        for($i=0;$i<$TFMCparsN;$i++){$TFMCcmval[$i]=$TFMCcmdf[$i];}
        $TofRefCalSetMC=$TFMCcmdf[4];
#
        $j=1;
        for($i=0;$i<4;$i++){
          $MCTofDBUsePatt[$i]=1-(($TFMCcmval[3]/$j)%10);# 10-base BitPattern Msb(TlSd Peds Tmap CalSd)Lsb (=1 to use)
          $j=10*$j;
        }
      }#--->endof page-1
    }
  }
#--->gr-3
  elsif($DataGroup eq "ACC"){
    if($RecoSimuP eq "RECO"){
      for($i=0;$i<$ATREparsN;$i++){$ATREcmval[$i]=$ATREcmdf[$i];}
      for($i=0;$i<$ATCAparsN;$i++){$ATCAcmval[$i]=$ATCAcmdf[$i];}
      $AccRefCalSetRD=$ATREcmdf[5];
      $AccRDPrtName=$AccRDPrtNames[$ATREcmval[0]];
#
      $j=1;
      for($i=0;$i<3;$i++){
        $AccDBUsePatt[$i]=1-(($ATREcmval[4]/$j)%10);# 10-base BitPattern Msb(Peds VarP StabP)Lsb (=1 to use)
        $j=10*$j;
      }
    }
    else{
      for($i=0;$i<$ATMCparsN;$i++){$ATMCcmval[$i]=$ATMCcmdf[$i];}
      $AccRefCalSetMC=$ATMCcmdf[3];
      $AccMCPrtName=$AccMCPrtNames[$ATMCcmval[0]];
#
      $j=1;
      for($i=0;$i<2;$i++){
        $MCAccDBUsePatt[$i]=1-(($ATMCcmval[2]/$j)%10);# 10-base BitPattern Msb(SDped MCped)Lsb (=1 to use)
        $j=10*$j;
      }
    }
  }
#--->gr-4
  elsif($DataGroup eq "EMC"){
    if($RecoSimuP eq "RECO"){
      for($i=0;$i<$ECREparsN;$i++){$ECREcmval[$i]=$ECREcmdf[$i];}
      for($i=0;$i<$ECCAparsN;$i++){$ECCAcmval[$i]=$ECCAcmdf[$i];}
      $EmcRefCalSetRD=$ECREcmdf[5];
      $EmcPrtName=$EmcPrtNames[$ECREcmval[0]];
      $EmcDebName=$EmcPrtNames[$ECREcmval[1]];
#
      $j=1;
      for($i=0;$i<3;$i++){
        $EcDBUsePatt[$i]=1-(($ECREcmval[4]/$j)%10);# 10-base BitPattern Msb(Peds Calp DCut)Lsb (=1 to use)
        $j=10*$j;
      }
    }
    else{
      for($i=0;$i<$ECMCparsN;$i++){$ECMCcmval[$i]=$ECMCcmdf[$i];}
      $EmcRefCalSetMC=$ECMCcmdf[7];
#
      $j=1;
      for($i=0;$i<2;$i++){
        $MCEcDBUsePatt[$i]=1-(($ECMCcmval[6]/$j)%10);# 10-base BitPattern Msb(SDcal MCped)Lsb (=1 to use)
        $j=10*$j;
      }
    }
  }
#--->gr-5
  elsif($DataGroup eq "AMS"){
    for($i=0;$i<$AMSJOBparsN;$i++){$AMSJOBcmval[$i]=$AMSJOBcmdf[$i];}
    for($i=0;$i<$MAGSparsN;$i++){$MAGScmval[$i]=$MAGScmdf[$i];}
    for($i=0;$i<$MISCparsN;$i++){$MISCcmval[$i]=$MISCcmdf[$i];}
#
    if($RecoSimuP eq "RECO"){
      for($i=0;$i<6;$i++){
        $P2DetInDaqBut[$i]->select();#on=def
      }
      $RunTypeText=$DataTypes[0];
    }
  }
#------>gr-6
  elsif($DataGroup eq "IO"){
    if($RecoSimuP eq "RECO"){
      for($i=0;$i<$IOPAparsN;$i++){$IOPAcmval[$i]=$IOPAcmdf[$i];}
      for($i=0;$i<$DAQCparsN;$i++){$DAQCcmval[$i]=$DAQCcmdf[$i];}
      $ObjWriteVal=($IOPAcmval[0]%10);
      $ObjWriteMode=$ObjWriteModes[$ObjWriteVal];
      $Wr2RootfCond1=int($IOPAcmval[0]/10);# if > 0 fill header block even if trig.cond not ok 
      $Wr2RootfCond2=int($IOPAcmval[0]/100);#write event with error if > 0
    }
    else{
      for($i=0;$i<$DAQCparsN;$i++){$DAQCcmval[$i]=$DAQCcmdf[$i];}
    }
    $ReadDaqFlg=($DAQCcmval[0]%10);
    $WriteDaqFlg=(int($DAQCcmval[0]/10)%10);
    $ComprDaqFlg=int($DAQCcmval[0]/100)
  }
#------>gr-7
  elsif($DataGroup eq "LVL1"){
    for($i=0;$i<$TGL1parsN;$i++){$TGL1cmval[$i]=$TGL1cmdf[$i];}
    if($RecoSimuP eq "SIMU"){
      for($i=0;$i<14;$i++){$TGL1cmval[$i]=$TGL1cmdf[$i];}# reset SIMU-page params
      $TofLayConfDispl="def";
      $TofLayConfIDispl="def";
      $TofLayConfSIDispl="def";
      $Tgl1MCConfName=$Tgl1MCConfNames[$TGL1cmval[12]-1];
#
      if($TGL1cmval[0]>0){$TrigPhysmPatt=$TGL1cmval[0];}
      else{$TrigPhysmPatt=0;}
      for($i=0;$i<$TrigPhysMembs;$i++){
        if(($TrigPhysmPatt & (1<<$i))>0){$TrigPhysmVals[$i]=1;}
	else{$TrigPhysmVals[$i]=0;}
      }
#
    }
#--
    else{
      for($i=14;$i<$TGL1parsN;$i++){$TGL1cmval[$i]=$TGL1cmdf[$i];}# reset RECO-page params
      $Tgl1PrtFlg=($TGL1cmval[18]%10);#lsb
      $Tgl1DebFlg=int($TGL1cmval[18]/10);#msb
      $Tgl1PrtName=$Tgl1PrtNames[$Tgl1PrtFlg];
      $Tgl1DebName=$Tgl1DebNames[$Tgl1DebFlg];
      $Tgl1SetupDestName=$Tgl1SetupDestNames[$TGL1cmval[19]];
#
      $j=1;
      for($i=0;$i<1;$i++){
        $Lv1DBUsePatt[$i]=1-(($TGL1cmval[17]/$j)%10);# 10-base BitPattern Msb(TrConf)Lsb (=1 to use)
        $j=10*$j;
      }
#
      $Lv1OffPrivFlg=int($TGL1cmval[17]/10);# 0/1=>offic/priv
    }
  }
#--->gr-1
  elsif($DataGroup eq "MCgn"){
    if($RecoSimuP eq "SIMU"){
      for($i=0;$i<$KINEparsN;$i++){$KINEcmval[$i]=$KINEcmdf[$i];}
      for($i=0;$i<$RNDMparsN;$i++){$RNDMcmval[$i]=$RNDMcmdf[$i];}
      for($i=0;$i<$DEBUGparsN;$i++){$DEBUGcmval[$i]=$DEBUGcmdf[$i];}
      for($i=0;$i<$CUTSparsN;$i++){$CUTScmval[$i]=$CUTScmdf[$i];}
      for($i=0;$i<$MCGENparsN;$i++){$MCGENcmval[$i]=$MCGENcmdf[$i];}
      for($i=0;$i<$GMSRCparsN;$i++){$GMSRCcmval[$i]=$GMSRCcmdf[$i];}
      $PartName="Prot";
      $EnSpectr=$EnSpectra[$MCGENcmval[17]];
      $ImpAngThe="180.";
      $ImpAngPhi="0.";
      $GamSource="None";
      $FocusOn="NoFocus";
    }
  }
#--->gr-8
  elsif($DataGroup eq "SelEv"){
    if($PageNumb eq "Page1"){#<--- for "ESTA" card
      for($i=0;$i<$ESTAparsN;$i++){$ESTAcmval[$i]=$ESTAcmdf[$i];}
      $NPartName=$NPartNames[0];
      $NTrdTrName="Any";
      $NTofClName="Any";
      $NTrkTrName="Any";
      $NRichRName="Any";
      $NEcShwName="Any";
      $NVertxName="Any";
      $NAccSrName="Any";
      $NChargeName="Any";
      $Conf1Name="Any";
      $Conf2Name="Any";
    }
    elsif($PageNumb eq "Page2"){#<--- for "SELECT" card
      for($i=0;$i<$SELEparsN;$i++){$SELEcmval[$i]=$SELEcmdf[$i];}
    }#-->endof page-2
  }
#--->gr-10
  elsif($DataGroup eq "TRK"){
    if($RecoSimuP eq "RECO"){
      if($PageNumb eq "Page1"){#<--- for part of "TRFI" card
        for($i=0;$i<17;$i++){$TRFIcmval[$i]=$TRFIcmdf[$i];}
        $FastTrkMode=$FastTrkModes[4-$TRFIcmval[6]];
      }
      elsif($PageNumb eq "Page2"){#page-2
        for($i=17;$i<$TRFIparsN;$i++){$TRFIcmval[$i]=$TRFIcmdf[$i];}
        $TrkFitMethod=$TrkFitMethods[$TRFIcmval[18]];
        $TrkPattRecMethod=$TrkPattRecMethods[$TRFIcmval[19]];
      }
    }#-->endof reco-page
  }
#--->gr-9
  elsif($DataGroup eq "ZFit"){
    if($RecoSimuP eq "RECO"){
      for($i=0;$i<$CHARparsN;$i++){
        if($i==16){next;}#bypass mc-parameter
        $CHARcmval[$i]=$CHARcmdf[$i];
      }
      if($PageNumb eq "Page1"){#<--- for "CHARGEFIT" card
        $TrkPrjName=$TrkPrjNames[$CHARcmdf[0]];
      }
    }
    else{
      $CHARcmval[16]=$CHARcmdf[16];
    }
  }
#
  $mwnd->update;
#  
}
#---------------------
sub ConfirmPars{
  my $i,$j;
  my $errn=0;
  my ($nr,$r)=(0,0);
  my $theta,$phi;
  my $fvar;
#--->"TOF"
# set db-use DCards:
  $TFREcmval[11]=0;
  $j=1;
  for($i=0;$i<5;$i++){
    if($TofDBUsePatt[$i]==0){$TFREcmval[11]+=$j;}#means use RawFile instead DB
    $j=10*$j;
  }
#
  $TFMCcmval[3]=0;
  $j=1;
  for($i=0;$i<4;$i++){
    if($MCTofDBUsePatt[$i]==0){$TFMCcmval[3]+=$j;}#means use RawFile instead DB
    $j=10*$j;
  }
#--->"ACC"
# set db-use DCards:
  $ATREcmval[4]=0;
  $j=1;
  for($i=0;$i<3;$i++){
    if($AccDBUsePatt[$i]==0){$ATREcmval[4]+=$j;}#means use RawFile instead DB
    $j=10*$j;
  }
#
  $ATMCcmval[2]=0;
  $j=1;
  for($i=0;$i<2;$i++){
    if($MCAccDBUsePatt[$i]==0){$ATMCcmval[2]+=$j;}#means use RawFile instead DB
    $j=10*$j;
  }
#--->"EMC"
# set db-use DCards:
  $ECREcmval[4]=0;
  $j=1;
  for($i=0;$i<3;$i++){
    if($EcDBUsePatt[$i]==0){$ECREcmval[4]+=$j;}#means use RawFile instead DB
    $j=10*$j;
  }
#
  $ECMCcmval[6]=0;
  $j=1;
  for($i=0;$i<2;$i++){
    if($MCEcDBUsePatt[$i]==0){$ECMCcmval[6]+=$j;}#means use RawFile instead DB
    $j=10*$j;
  }
#--->"IO"
  $DAQCcmval[0]=$ReadDaqFlg+10*$WriteDaqFlg+100*$ComprDaqFlg;# create  DAQ-read/write param 
#
  if($IOPAcmval[3]>0){$WrRootFlg=1;}
  else{$WrRootFlg=0;}
#
  $IOPAcmval[0]=$ObjWriteVal+10*$Wr2RootfCond1+100*$Wr2RootfCond2;# set obj to write
#--->"AMS"
  $nr=0;
  $r=0;
  if($SessName eq "RDR"){
    for($i=0;$i<6;$i++){
      if($DetInDaq[$i]==1){
	$r+=(1<<$i);
	$nr+=1;
      }
    }
    if($nr>0){
      $SubDetMsk=$r;#needed subdets bit-patt: (msb T8/U2/S4/R2/E2/L1 lsb)
    }
    else{
      show_warn("\n   <--- Needed SubDetectors pattern is not defined, do selection !!!");
      $errn+=1;
    }
  }
#--->"LVL1"
  $nr=0;
  $r=0;
  if($SessName eq "MCS"){
    $TrigPhysmPatt=0;
    for($i=0;$i<$TrigPhysMembs;$i++){#<-create trig-patt according to buttons state
      if($TrigPhysmVals[$i]==1){$TrigPhysmPatt+=(1<<$i);}
    }
    if($TrigPhysmPatt>0){$TGL1cmval[0]=$TrigPhysmPatt;}
    else{
      $TGL1cmval[0]=-1;
      show_messg("\n   <--- PhysTrigMembPattern is not defined, so it will be taken at run-time","B");
      show_messg("        from DB or default MC TrigConfigFile if you don't define it NOW !!!","B");
    }
  }
#
  $TGL1cmval[18]=$Tgl1PrtFlg+10*$Tgl1DebFlg;#prt/deb flag
#
  $TGL1cmval[17]=(1-$Lv1DBUsePatt[0])+10*$Lv1OffPrivFlg;# off/priv|db/file flag
#
  $fvar=sprintf("%.1f",$TGL1cmval[14]);#max.scal.rate: prot.against missing "." in stupid FFREAD
  $TGL1cmval[14]=$fvar;
  $fvar=sprintf("%.2f",$TGL1cmval[15]);#min.live time: prot.against missing "." in stupid FFREAD
  $TGL1cmval[15]=$fvar;
#
#--->"MCgn"
  if($SessName eq "MCS"){
    if($IllumType eq $IllumTypes[2]){# Fixed Dir&(Point||Area)
      $MCGENcmval[0]=sprintf("%.2f",($IPointX-$HAreaX));
      $MCGENcmval[1]=sprintf("%.2f",($IPointY-$HAreaY));
      $MCGENcmval[2]=sprintf("%.2f",($IPointZ-$HAreaZ));
      $MCGENcmval[3]=sprintf("%.2f",($IPointX+$HAreaX));
      $MCGENcmval[4]=sprintf("%.2f",($IPointY+$HAreaY));
      $MCGENcmval[5]=sprintf("%.2f",($IPointZ+$HAreaZ));
      $theta=$ImpAngThe*$Deg2Rad;
      $phi=$ImpAngPhi*$Deg2Rad;
      $MCGENcmval[6]=sprintf("%.3f",(sin($theta)*cos($phi)));
      $MCGENcmval[7]=sprintf("%.3f",(sin($theta)*sin($phi)));
      $MCGENcmval[8]=sprintf("%.3f",(cos($theta)));
      $MCGENcmval[9]=$MCGENcmval[6];#to have fixed dir
      $MCGENcmval[10]=$MCGENcmval[7];#to have fixed dir
      $MCGENcmval[11]=$MCGENcmval[8];#to have fixed dir
    }
  }
#  
#    
#---> Logf-name=jobname:
  $RLogName=$AMSJOBcmval[1];#jobname
#-----
  if($errn>0){show_warn("\n   <--- Confirmation is failed, correct errors first !!!","Big");}
  else{
    show_messg("\n   <--- Parameters are successfully confirmed !!!");
    $CrJScr_b->configure(-state=>"normal");#release CreateJScript-button
  }
#  
  $mwnd->update;
}
#--------------------------------------------
sub CreateJobScript{
  my $gr,$gm,$gmm,$cm,$cmm,$pos,$cnam,$cmid,$val,$def,$linem,$slinem;
  my $i,$j;
  $line,$sline;
  my @arr=();
  my @jobscript=();
  my $wroot=0;
  my $rsdir,$rssdir,$path,$ppath;
#
  if($SessName eq "RDR"){$rsdir="/RD";}
  else{$rsdir="/MC";}
#
#--- add standard header:
#
  $line='if [ "$4" = "" ] ; then';
  push(@jobscript,$line);
  $line=' echo " Missing  parameter !!!"';
  push(@jobscript,$line);
  $line=" exit";
  push(@jobscript,$line);
  $line="else";
  push(@jobscript,$line);
  $line=' echo "Evs2Use  : $1"';
  push(@jobscript,$line);   
  $line=' echo "CPUs2Use : $2"';
  push(@jobscript,$line);   
  $line=' echo "SetupName: $3"';
  push(@jobscript,$line);   
  $line=' echo "LinksDir : $4"';
  push(@jobscript,$line);   
  $line=" NEVENTS=\$1\n NCPUS=\$2\n SETUPN=\$3\n LINKDIR=\$4";
  push(@jobscript,$line);   
  $line=' echo " Start program ..."';
  $line="fi\n\n";
  push(@jobscript,$line);   
#---
  $line="cd ".$workdir.$amsjwd."\n";
  push(@jobscript,$line);
  $line=$AMSCVS."/exe/linuxicc/gbatch.exe <<!\n";
  push(@jobscript,$line);
  $line="LIST";
  push(@jobscript,$line);
#
  $line="";
  if($SessName eq "MCS"){#<--- put KINE-card here for conveniens
    $line="KINE ";
    $cmm=$KINEparsN;
    for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
      $linem=length($line);
      $cmid=$KINEcmid[$cm];
      $def=$KINEcmdf[$cm];
      $val=$KINEcmval[$cm];
      $sline=" ".$cmid."=".$val;
      $slinem=length($sline);
      if($slinem < (72-$linem)){$line=$line.$sline;}
      else{
        push(@jobscript,$line);
        $line=$sline;
      }
    }#<-- card-mems loop
    $linem=length($line);
    if($linem > 0){push(@jobscript,$line);}#uncompleted line
  }
#
  $line="TRIG \$NEVENTS";#Nevents
  push(@jobscript,$line);
#---
  $pos=0;
  for($gr=0;$gr<$Ngroups;$gr++){#<-- groups loop
    $gmm=$GroupMemsN[$gr];
#    print "---> group:",$gr," members:",$gmm,"\n";
    for($gm=0;$gm<$gmm;$gm++){#<-- Gr-member(dc-cards types) loop
      $cnam=$MemsNames[$pos];
# print "cname=",$cnam,"\n";
      $line="";
#---
      if($cnam eq "TFRE"){
        $line="TFRE";
	$cmm=$TFREparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  if($SessName eq "RDR" && ($cm>=6 && $cm<=8)){next;}#skip mc-pars in rd-card
	  $linem=length($line);
	  $cmid=$TFREcmid[$cm];
	  $def=$TFREcmdf[$cm];
	  $val=$TFREcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
      if($cnam eq "TFCA"){
        $line="TFCA";
	$cmm=$TFCAparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$TFCAcmid[$cm];
	  $def=$TFCAcmdf[$cm];
	  $val=$TFCAcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
      if($cnam eq "TFMC" && $SessName ne "RDR"){
        $line="TFMC";
	$cmm=$TFMCparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$TFMCcmid[$cm];
	  $def=$TFMCcmdf[$cm];
	  $val=$TFMCcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
      if($cnam eq "BETAFIT"){
        $line="BETAFIT";
	$cmm=$BETAFITparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$BETAFITcmid[$cm];
	  $def=$BETAFITcmdf[$cm];
	  $val=$BETAFITcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
#(ACC)
      if($cnam eq "ATRE"){
        $line="ATRE";
	$cmm=$ATREparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$ATREcmid[$cm];
	  $def=$ATREcmdf[$cm];
	  $val=$ATREcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
      if($cnam eq "ATCA"){
        $line="ATCA";
	$cmm=$ATCAparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$ATCAcmid[$cm];
	  $def=$ATCAcmdf[$cm];
	  $val=$ATCAcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
      if($cnam eq "ATMC" && $SessName ne "RDR"){
        $line="ATMC";
	$cmm=$ATMCparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$ATMCcmid[$cm];
	  $def=$ATMCcmdf[$cm];
	  $val=$ATMCcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
#(EMC)
      if($cnam eq "ECRE"){
        $line="ECRE";
	$cmm=$ECREparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$ECREcmid[$cm];
	  $def=$ECREcmdf[$cm];
	  $val=$ECREcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
      if($cnam eq "ECCA"){
        $line="ECCA";
	$cmm=$ECCAparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$ECCAcmid[$cm];
	  $def=$ECCAcmdf[$cm];
	  $val=$ECCAcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
      if($cnam eq "ECMC" && $SessName ne "RDR"){
        $line="ECMC";
	$cmm=$ECMCparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$ECMCcmid[$cm];
	  $def=$ECMCcmdf[$cm];
	  $val=$ECMCcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
#(AMS)
      if($cnam eq "MAGS"){
        $line="MAGS";
	$cmm=$MAGSparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  if($SessName eq "RDR" && $cm==1){next;}#skip mc-pars in rd-card
	  $linem=length($line);
	  $cmid=$MAGScmid[$cm];
	  $def=$MAGScmdf[$cm];
	  $val=$MAGScmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
      if($cnam eq "MISC"){
        $line="MISC";
	$cmm=$MISCparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  if($SessName eq "RDR" && ($cm>=1 && $cm<=2)){next;}#skip mc-pars in rd-card
	  $linem=length($line);
	  $cmid=$MISCcmid[$cm];
	  $def=$MISCcmdf[$cm];
	  $val=$MISCcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	$sline=" 8=\$NCPUS";
	$slinem=length($sline);
	if($slinem < (72-$linem)){$line=$line.$sline;}
	else{
          push(@jobscript,$line);
	  $line=$sline;
	}
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
      if($cnam eq "AMSJOB"){
        $val=0;
        if($SessName eq "RDR"){$val=11;}
	elsif($SessName eq "MCS"){$val=0;}
	else{$val=1;}
        $line="AMSJOB 1=".$val;# add rd|reco-flag line
        push(@jobscript,$line);
	$cmm=$AMSJOBparsN;
	$line="";
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$AMSJOBcmid[$cm];
	  $def=$AMSJOBcmdf[$cm];
	  $val=$AMSJOBcmval[$cm];
	  if($cm==1){$val="'".$val."'";}# add "''" to job-name
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	$sline=" 46='\$SETUPN'";# add setup-name passed as job-param
	$slinem=length($sline);
	if($slinem < (72-$linem)){$line=$line.$sline;}
	else{
          push(@jobscript,$line);
	  $line=$sline;
	}
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
#(IO)
      if($cnam eq "IOPA"){
        $line="IOPA";
	$cmm=$IOPAparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$IOPAcmid[$cm];
	  $def=$IOPAcmdf[$cm];
	  $val=$IOPAcmval[$cm];
	  if($cm==4){#<---make complete root-file path
	    if($RootfSubD eq ""){$rssdir=$RootfSubD."/";}
	    else{$rssdir="/".$RootfSubD."/";}
	    $path="'".$RootfDir.$rsdir.$rssdir.$val.".root"."'";
	    $val=$path;
	  }
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
      if($cnam eq "DAQC"){
        $line="DAQC";
	$cmm=$DAQCparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$DAQCcmid[$cm];
	  $def=$DAQCcmdf[$cm];
	  $val=$DAQCcmval[$cm];
	  if($cm==1){# MCDaqfName to write
	    if($SessName eq "RDR"){next;}#no DAQ-writing
	    if($SessName eq "MCS" && $WriteDaqFlg>0){# 
	      if($MCDaqfSubD eq ""){$rssdir=$MCDaqfSubD."/";}
	      else{$rssdir="/".$MCDaqfSubD."/";}
	      $path="'".$MCDaqfDir.$rsdir.$rssdir.$val."'";
	      $val=$path;
	    }
	    else{next;}#no DAQ-writing
	  }
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	$sline=" 2='\$LINKDIR'";
	$slinem=length($sline);
	if($slinem < (72-$linem)){$line=$line.$sline;}
	else{
          push(@jobscript,$line);
	  $line=$sline;
	}
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
#(LVL1)
      if($cnam eq "TGL1"){
        $line="TGL1";
	$cmm=$TGL1parsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  if($SessName ne "MCS" && ($cm>=0 && $cm<=13)){next;}#skip mc-pars when reco
	  $linem=length($line);
	  $cmid=$TGL1cmid[$cm];
	  $def=$TGL1cmdf[$cm];
	  $val=$TGL1cmval[$cm];
	  if($SessName eq "MCS" && ($cm>=0 && $cm<=11) && $val==-1){next;}#skip not needed(=defs in conf.file) mc-pars
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
#(MCgn)
      if($cnam eq "RNDM" && $SessName eq "MCS" && $RNDMcmval[0]>0){
        $line="RNDM";
	$cmm=$RNDMparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$RNDMcmid[$cm];
	  $def=$RNDMcmdf[$cm];
	  $val=$RNDMcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
      if($cnam eq "DEBUG"){
        $line="DEBUG";
	$cmm=$DEBUGparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$DEBUGcmid[$cm];
	  $def=$DEBUGcmdf[$cm];
	  $val=$DEBUGcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
	$line="\n";
	push(@jobscript,$line);
      }
      if($cnam eq "CUTS"){
        $line="CUTS";
	$cmm=$CUTSparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$CUTScmid[$cm];
	  $def=$CUTScmdf[$cm];
	  $val=$CUTScmval[$cm];
	  if($val==$def){next;}
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0 && $line ne "CUTS"){push(@jobscript,$line);}#uncompleted line
      }
      if($cnam eq "MCGEN" && $SessName eq "MCS"){
        $line="MCGEN";
	$cmm=$MCGENparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$MCGENcmid[$cm];
	  $def=$MCGENcmdf[$cm];
	  $val=$MCGENcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
      if($cnam eq "GMSRC" && $SessName eq "MCS"){
        $line="GMSRC";
	$cmm=$GMSRCparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$GMSRCcmid[$cm];
	  $def=$GMSRCcmdf[$cm];
	  $val=$GMSRCcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0){push(@jobscript,$line);}#uncompleted line
      }
#(SelEv)
      if($cnam eq "ESTA"){
        $line="ESTA";
	$cmm=$ESTAparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$ESTAcmid[$cm];
	  $def=$ESTAcmdf[$cm];
	  $val=$ESTAcmval[$cm];
	  if($val==0){next;}#<-- skip def.settings(they are definitely equal to ones in job.C)
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0 && $line ne "ESTA"){push(@jobscript,$line);}#uncompleted line non-empty
      }
      if($cnam eq "SELE"){
        $line="SELE";
	$cmm=$SELEparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$SELEcmid[$cm];
	  $def=$SELEcmdf[$cm];
	  $val=$SELEcmval[$cm];
	  if($cm!=2 && $val==0){next;}#<-- skip def.settings(they are definitely equal to ones in job.C)
	  if($cm==2){
	    if($val eq "None"){next;}#<-- skip def.settings(they are definitely equal to ones in job.C)
	    else{$val="'".$workdir.$amsjwd."/".$SELEcmval[2]."'";}
	  }
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0 && $line ne "SELE"){push(@jobscript,$line);}#uncompleted line non-empty
      }
#(Charge)
      if($cnam eq "CHAR"){
        $line="CHAR";
	$cmm=$CHARparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$CHARcmid[$cm];
	  $def=$CHARcmdf[$cm];
	  $val=$CHARcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0 && $line ne "CHAR"){push(@jobscript,$line);}#uncompleted line
      }
#(TRFIT)
      if($cnam eq "TRFI"){
        $line="TRFI";
	$cmm=$TRFIparsN;
	for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $linem=length($line);
	  $cmid=$TRFIcmid[$cm];
	  $def=$TRFIcmdf[$cm];
	  $val=$TRFIcmval[$cm];
	  $sline=" ".$cmid."=".$val;
	  $slinem=length($sline);
	  if($slinem < (72-$linem)){$line=$line.$sline;}
	  else{
            push(@jobscript,$line);
	    $line=$sline;
	  }
        }#<-- card-mems loop
	$linem=length($line);
	if($linem > 0 && $line ne "TRFI"){push(@jobscript,$line);}#uncompleted line
      }
#---
      $pos+=1;
    }#-->endof Gr-members(dc-cards types) loop
    $line="\n";
    push(@jobscript,$line);
  }#-->endof groups loop
#
#--- add standard trailer:
#
  $line="\n";
  push(@jobscript,$line);
  $line="TIME 3=10000000";
  push(@jobscript,$line);
  $line="TERM 1=1234567890";
  push(@jobscript,$line);
  $line="END\n";
  push(@jobscript,$line);
  $line="\n!\n";
  push(@jobscript,$line);
#
#------ save script in file:
#
  my $fn=$workdir."/UnivJScript";
  my $status=0;
  my $nel=scalar(@jobscript);
  $WarnFlg=0;
  open(JSCRF,"+> $fn") or show_warn("   <-- Cannot open $fn for writing, $!");
  for($i=0;$i<$nel;$i++){print JSCRF $jobscript[$i]."\n";}
  close(JSCRF) or show_warn("   <-- Cannot close $fn after writing, $!");
  $status=system("chmod u+x $fn");
  if($status != 0){
    show_warn("   <-- Warning: problem with write-priv for JScript, status=$status");
  }
  if($WarnFlg > 0){
    return;
    show_warn("   <-- Error: problems to create JScript !!!","Big");
  };
#
  show_messg("\n   <---  JobScript file $fn is created !","B");
  if($SessName eq "MCS" || $SessName eq "MCR"){
    $setupj_bt->configure(-state=>"normal");#release setupjob-button(nothing else to set for MC-session)
  }
  if($SessName eq "RDR"){
    $convert_bt->configure(-state=>'normal');
  }
#
}
#---------------------------------------------
sub CreateNewDCFile{
  my ($gr,$gm,$gmm,$cm,$cmm,$pos,$cnam,$cmid,$val);
  my @DCbuffer=();
  $pos=0;
  show_messg("   <--- Found $Ngroups datacards groups...");
  $line=$Ngroups."\n";
  push(@DCbuffer,$line);
  for($gr=0;$gr<$Ngroups;$gr++){#<-- groups loop
    $val=$gr+1;#grid
    $line=$val."\n";# group number
    push(@DCbuffer,$line);
    $gmm=$GroupMemsN[$gr];
    $line=" ".$gmm."\n";;# gr-members
    push(@DCbuffer,$line);
    for($gm=0;$gm<$gmm;$gm++){#<-- Gr-member(dc-cards types) loop
      $cnam=$MemsNames[$pos];
      if($cnam eq "KINE"){
        $cmm=$KINEparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$KINEcmid[$cm];
	  if($KINEcmdf[$cm] eq $KINEcmval[$cm]){$val=$KINEcmdf[$cm];}
	  else{$val=$KINEcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "RNDM"){
        $cmm=$RNDMparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$RNDMcmid[$cm];
	  if($RNDMcmdf[$cm] eq $RNDMcmval[$cm]){$val=$RNDMcmdf[$cm];}
	  else{$val=$RNDMcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "DEBUG"){
        $cmm=$DEBUGparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$DEBUGcmid[$cm];
	  if($DEBUGcmdf[$cm] eq $DEBUGcmval[$cm]){$val=$DEBUGcmdf[$cm];}
	  else{$val=$DEBUGcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "CUTS"){
        $cmm=$CUTSparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$CUTScmid[$cm];
	  if($CUTScmdf[$cm] eq $CUTScmval[$cm]){$val=$CUTScmdf[$cm];}
	  else{$val=$CUTScmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "MCGEN"){
        $cmm=$MCGENparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$MCGENcmid[$cm];
	  if($MCGENcmdf[$cm] eq $MCGENcmval[$cm]){$val=$MCGENcmdf[$cm];}
	  else{$val=$MCGENcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "GMSRC"){
        $cmm=$GMSRCparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$GMSRCcmid[$cm];
	  if($GMSRCcmdf[$cm] eq $GMSRCcmval[$cm]){$val=$GMSRCcmdf[$cm];}
	  else{$val=$GMSRCcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      if($cnam eq "TFRE"){
        $cmm=$TFREparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$TFREcmid[$cm];
	  if($TFREcmdf[$cm] eq $TFREcmval[$cm]){$val=$TFREcmdf[$cm];}
	  else{$val=$TFREcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "TFCA"){
        $cmm=$TFCAparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$TFCAcmid[$cm];
	  if($TFCAcmdf[$cm] eq $TFCAcmval[$cm]){$val=$TFCAcmdf[$cm];}
	  else{$val=$TFCAcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "TFMC"){
        $cmm=$TFMCparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$TFMCcmid[$cm];
	  if($TFMCcmdf[$cm] eq $TFMCcmval[$cm]){$val=$TFMCcmdf[$cm];}
	  else{$val=$TFMCcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      if($cnam eq "ATRE"){
        $cmm=$ATREparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$ATREcmid[$cm];
	  if($ATREcmdf[$cm] eq $ATREcmval[$cm]){$val=$ATREcmdf[$cm];}
	  else{$val=$ATREcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "ATCA"){
        $cmm=$ATCAparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$ATCAcmid[$cm];
	  if($ATCAcmdf[$cm] eq $ATCAcmval[$cm]){$val=$ATCAcmdf[$cm];}
	  else{$val=$ATCAcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "ATMC"){
        $cmm=$ATMCparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$ATMCcmid[$cm];
	  if($ATMCcmdf[$cm] eq $ATMCcmval[$cm]){$val=$ATMCcmdf[$cm];}
	  else{$val=$ATMCcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      if($cnam eq "ECRE"){
        $cmm=$ECREparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$ECREcmid[$cm];
	  if($ECREcmdf[$cm] eq $ECREcmval[$cm]){$val=$ECREcmdf[$cm];}
	  else{$val=$ECREcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "ECCA"){
        $cmm=$ECCAparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$ECCAcmid[$cm];
	  if($ECCAcmdf[$cm] eq $ECCAcmval[$cm]){$val=$ECCAcmdf[$cm];}
	  else{$val=$ECCAcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "ECMC"){
        $cmm=$ECMCparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$ECMCcmid[$cm];
	  if($ECMCcmdf[$cm] eq $ECMCcmval[$cm]){$val=$ECMCcmdf[$cm];}
	  else{$val=$ECMCcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      if($cnam eq "MAGS"){
        $cmm=$MAGSparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$MAGScmid[$cm];
	  if($MAGScmdf[$cm] eq $MAGScmval[$cm]){$val=$MAGScmdf[$cm];}
	  else{$val=$MAGScmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "MISC"){
        $cmm=$MISCparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$MISCcmid[$cm];
	  if($MISCcmdf[$cm] eq $MISCcmval[$cm]){$val=$MISCcmdf[$cm];}
	  else{$val=$MISCcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "AMSJOB"){
        $cmm=$AMSJOBparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$AMSJOBcmid[$cm];
	  if($AMSJOBcmdf[$cm] eq $AMSJOBcmval[$cm]){$val=$AMSJOBcmdf[$cm];}
	  else{$val=$AMSJOBcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      if($cnam eq "IOPA"){
        $cmm=$IOPAparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$IOPAcmid[$cm];
	  if($IOPAcmdf[$cm] eq $IOPAcmval[$cm]){$val=$IOPAcmdf[$cm];}
	  else{$val=$IOPAcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "DAQC"){
        $cmm=$DAQCparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$DAQCcmid[$cm];
	  if($DAQCcmdf[$cm] eq $DAQCcmval[$cm]){$val=$DAQCcmdf[$cm];}
	  else{$val=$DAQCcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      if($cnam eq "TGL1"){
        $cmm=$TGL1parsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$TGL1cmid[$cm];
	  if($TGL1cmdf[$cm] eq $TGL1cmval[$cm]){$val=$TGL1cmdf[$cm];}
	  else{$val=$TGL1cmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
#
      if($cnam eq "ESTA"){
        $cmm=$ESTAparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$ESTAcmid[$cm];
	  if($ESTAcmdf[$cm] eq $ESTAcmval[$cm]){$val=$ESTAcmdf[$cm];}
	  else{$val=$ESTAcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
      elsif($cnam eq "SELE"){
        $cmm=$SELEparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$SELEcmid[$cm];
	  if($SELEcmdf[$cm] eq $SELEcmval[$cm]){$val=$SELEcmdf[$cm];}
	  else{$val=$SELEcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
#
      if($cnam eq "CHAR"){
        $cmm=$CHARparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$CHARcmid[$cm];
	  if($CHARcmdf[$cm] eq $CHARcmval[$cm]){$val=$CHARcmdf[$cm];}
	  else{$val=$CHARcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
#
      if($cnam eq "TRFI"){
        $cmm=$TRFIparsN;
	$line="  ".$cmm."  ".$cnam."\n";
        push(@DCbuffer,$line);
        for($cm=0;$cm<$cmm;$cm++){#<-- card-mems loop
	  $cmid=$TRFIcmid[$cm];
	  if($TRFIcmdf[$cm] eq $TRFIcmval[$cm]){$val=$TRFIcmdf[$cm];}
	  else{$val=$TRFIcmval[$cm];}
	  $line="      ".$cmid." ".$val."\n";
          push(@DCbuffer,$line);
	}
      }
#
      $pos+=1;
    }#-->endof Gr-members(dc-cards types) loop
  }#-->endof groups loop
#---
  my $fn="DCDefPars.txt";
  $WarnFlg=0;
  open(DCDEF,"> $fn") or show_warn("\n   <--- Cannot open $fn for reading, $!");
  print DCDEF @DCbuffer;
  close(DCDEF) or show_warn("\n   <--- Cannot close $fn after reading !");
  if($WarnFlg==1){
    show_warn("\n   <--- Writing problem with new DC-defaults file !!!");
    return;
  }
  show_messg("\n   <--- New Data Cards def.values file is created !");
}
#------------------------------------------------------------------------------
1;



