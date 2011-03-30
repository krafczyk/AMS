sub PedCalibration
{
#---
my $dirfrheight=0.35;
my $setfrheight=0.52;
my $prgfrheight=0.08;
my $jclfrheight=0.05;

#---> imp.glob.const, vars and arrays:
$PedFName="";
$outlogf="out.log";
$binw_tev=120;#binwidth(sec/pix) for time-evolution graphs(2 min)
$last_pedscanned_file_num=0;
$plot1_actf=0;#plot1 "active" flag
$plot2_actf=0;#plot2 "active" flag
$plot3_actf=0;#plot3 "active" flag
$plot4_actf=0;#plot4 "active" flag
@plot_pntids=();# current plot points ids list
$plot_pntmem=0;# members in pntids
@plot_accids=();# ............ accessoiries ids (labels,text, grids,...) 
@plot_fileids=();#current plot point-assosiated files ids
$plot_accmem=0;# members in accids
$plot1_xcmax=0;#max x-coo when leaving plot1
$plot2_xcmax=0;#max x-coo when leaving plot2
$plot3_xcmax=0;#max x-coo when leaving plot3
$plot4_xcmax=0;#max x-coo when leaving plot4
$htextagi1=="";#last used help_text look_at tag_index_1
$htextagi2=="";#last used help_text look_at tag_index_2
#--------------
#frame_geom:
my $shf1=0.1;#down shift for dir-widgets
my $drh1=0.18;#height of dir-widgets
#------
$dir_fram=$mwnd->Frame(-label => "General Info/Settings :",-background => "Grey",
                                                      -relief=>'groove', -borderwidth=>5)
						      ->place(
                                                      -relwidth=>0.3, -relheight=>$dirfrheight,
                                                      -relx=>0, -rely=>0);
#---
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
$pedd_lab=$dir_fram->Label(-text=>"PedfD:",-font=>$font2,-relief=>'groove')
                                                    ->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+$drh1));
$pedd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$pedsdir)->place(
                                                                   -relwidth=>0.3, -relheight=>$drh1,  
                                                                   -relx=>0.2, -rely=>($shf1+$drh1));
#------
$amsg_lab=$dir_fram->Label(-text=>"AmsDD:",-font=>$font2)->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+2*$drh1));
$amsg_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$AMSDD)->place(
                                                                   -relwidth=>0.8, -relheight=>$drh1,  
                                                                   -relx=>0.2, -rely=>($shf1+2*$drh1));
#------
$file_lab=$dir_fram->Label(-text=>"DaqF:",-font=>$font2)->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+3*$drh1));
$files="All";#to process particular file (default->ALL) 
$amsg_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$files)->place(
                                                                   -relwidth=>0.8, -relheight=>$drh1,  
                                                                   -relx=>0.2, -rely=>($shf1+3*$drh1));
#---
$dir_fram->Button(-text=>"Browser", -font=>$font2, 
                                 -activebackground=>"yellow",
			         -activeforeground=>"red",
			         -foreground=>"red",
			         -background=>"green",
                                 -borderwidth=>3,-relief=>'raised',
			         -cursor=>hand2,
                                 -command => \&open_browser_window)
			         ->place(
                                         -relwidth=>0.333, -relheight=>$drh1,  
                                         -relx=>0, -rely=>($shf1+4*$drh1));
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
                                         -relwidth=>0.333, -relheight=>$drh1,  
                                         -relx=>0.333, -rely=>($shf1+4*$drh1));
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
                                                 -relwidth=>0.333, -relheight=>$drh1,
						 -relx=>0.666, -rely=>($shf1+4*$drh1));
#----------------
#set_frame:
my $shf2=0.07;#down shift for runcond-widgets
my $drh2=0.103;#height of runcond-widgets
#---
$set_fram=$mwnd->Frame(-label=>"Job Parameters :", 
                                                      -relief=>'groove', -borderwidth=>5)->place(
                                                      -relwidth=>0.3, -relheight=>$setfrheight,
                                                      -relx=>0, -rely=>$dirfrheight);
#-------
$set_fram->Label(-text=>"PedsSource:",-font=>$font2,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.3, -relheight=>$drh2,
                                               -relx=>0, -rely=>$shf2);
$pedcaltyp="OnBrd";
$indat_rbt=$set_fram->Radiobutton(-text=>"SciData(RawFmt)",-font=>$font2, -indicator=>0, 
                                                 -borderwidth=>5,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red, 
						 -activebackground=>yellow, 
                                                 -background=>green, 
			                         -cursor=>hand2,
                                                 -value=>"Data", -variable=>\$pedcaltyp)
                                               ->place(
						       -relwidth=>0.35, -relheight=>$drh2,
						      -relx=>0.3, -rely=>$shf2);
#---
$onbrd_rbt=$set_fram->Radiobutton(-text=>"BoardTable",-font=>$font2, -indicator=>0, 
                                                 -borderwidth=>5,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red, 
						 -activebackground=>yellow, 
                                                 -background=>green, 
			                         -cursor=>hand2,
                                                 -value=>"OnBrd", -variable=>\$pedcaltyp)
                                               ->place(
						       -relwidth=>0.35, -relheight=>$drh2,
						        -relx=>0.65, -rely=>$shf2);
#--------
$tofantsel="useTOF";
$tof_rbt=$set_fram->Radiobutton(-text=>"TofPeds",-font=>$font2, -indicator=>0,
                                                 -borderwidth=>5,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>"useTOF", -variable=>\$tofantsel)
                                              ->place(
						      -relwidth=>0.25, -relheight=>$drh2,
						      -relx=>0, -rely=>($shf2+$drh2));
#---
$ant_rbt=$set_fram->Radiobutton(-text=>"AccPeds",-font=>$font2, -indicator=>0, 
                                                 -borderwidth=>5,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red, 
						 -activebackground=>yellow, 
                                                 -background=>green, 
			                         -cursor=>hand2,
#						 -foreground=>red,
                                                 -value=>"useANT", -variable=>\$tofantsel)
                                               ->place(
						       -relwidth=>0.25, -relheight=>$drh2,
						        -relx=>0.25, -rely=>($shf2+$drh2));
#---
$setdef_bt=$set_fram->Button(-text=>"SetJobDefPars", -font=>$font2, 
                                      -activebackground=>"yellow",
			              -activeforeground=>"red",
			              -foreground=>"red",
			              -background=>"green",
                                      -borderwidth=>5,-relief=>'raised',
			              -cursor=>hand2,
                                      -command => \&setRunDefs)
			              ->place(
                                      -relwidth=>0.5, -relheight=>$drh2,
				      -relx=>0.5, -rely=>($shf2+$drh2));
#------
$dat1_lab=$set_fram->Label(-text=>"Date",-font=>$font2)->place(-relwidth=>0.12, -relheight=>$drh2,
                                                             -relx=>0, -rely=>($shf2+2*$drh2));
$fdat1="2008.01.01 00:00:01";#def. file-date-from 
$fdat1_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font3,
                                                                   -textvariable=>\$fdat1)->place(
                                                                   -relwidth=>0.43, -relheight=>$drh2,  
                                                                   -relx=>0.12, -rely=>($shf2+2*$drh2));
#
$dat2_lab=$set_fram->Label(-text=>"-",-font=>$font2)->place(-relwidth=>0.02, -relheight=>$drh2,
                                                             -relx=>0.55, -rely=>($shf2+2*$drh2));
$fdat2="2015.01.01 00:00:01";#def. file-date-till 
$fdat2_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font3,
                                                                   -textvariable=>\$fdat2)->place(
                                                                   -relwidth=>0.43, -relheight=>$drh2,  
                                                                   -relx=>0.57, -rely=>($shf2+2*$drh2));
#---
$num1_lab=$set_fram->Label(-text=>"RunN",-font=>$font2)->place(-relwidth=>0.16, -relheight=>$drh2,
                                                             -relx=>0, -rely=>($shf2+3*$drh2));
$fnum1="0";#def. file-number-from 
$fnum1_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font3,
                                                                   -textvariable=>\$fnum1)->place(
                                                                   -relwidth=>0.4, -relheight=>$drh2,  
                                                                   -relx=>0.16, -rely=>($shf2+3*$drh2));
#
$num2_lab=$set_fram->Label(-text=>"-",-font=>$font2)->place(-relwidth=>0.04, -relheight=>$drh2,
                                                             -relx=>0.56, -rely=>($shf2+3*$drh2));
$fnum2="1500000000";#def. file-num-till 
$fnum2_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font3,
                                                                   -textvariable=>\$fnum2)->place(
                                                                   -relwidth=>0.4, -relheight=>$drh2,  
                                                                   -relx=>0.6, -rely=>($shf2+3*$drh2));
$fnum1o=$fnum1;
$fnum2o=$fnum2;
$fdat1o=$fdat1;
$fdat2o=$fdat2;
#-------
$set_fram->Label(-text=>"Push if limit changed >>>",-font=>$font2,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.5, -relheight=>$drh2,
                                               -relx=>0, -rely=>($shf2+4*$drh2));
$convert_bt=$set_fram->Button(-text=>"RunNumber<=>Date", -font=>$font2, 
                                      -activebackground=>"yellow",
			              -activeforeground=>"red",
			              -foreground=>"red",
			              -background=>"green",
                                      -borderwidth=>5,-relief=>'raised',
			              -cursor=>hand2,
                                      -command => \&RunDateConv)
			              ->place(
                                      -relwidth=>0.5, -relheight=>$drh2,
				      -relx=>0.5, -rely=>($shf2+4*$drh2));
#------
$PedRmsMn=0.4;
$par1_lab=$set_fram->Label(-text=>"PedRmsMn:",-font=>$font2,-relief=>'groove')
                                                    ->place(-relwidth=>0.3, -relheight=>$drh2,
                                                             -relx=>0, -rely=>($shf2+5*$drh2));
$par1_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",
                                                                -background=>yellow,
                                                                -font=>$font3,
                                                                -textvariable=>\$PedRmsMn)->place(
                                                                -relwidth=>0.2, -relheight=>$drh2,  
                                                                -relx=>0.3, -rely=>($shf2+5*$drh2));
#---
$par2_e_state='disabled';
$par2_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",
                                                                -background=>yellow,
                                                                -font=>$font3,
								-state=>$par2_e_state,
                                                                -textvariable=>\$PedRmsMx)->place(
                                                                -relwidth=>0.2, -relheight=>$drh2,  
                                                                -relx=>0.8, -rely=>($shf2+5*$drh2));
#---
$par2_lab=$set_fram->Label(-text=>"PedRmsMx:",-font=>$font2,-relief=>'groove')
                                                    ->place(-relwidth=>0.3, -relheight=>$drh2,
                                                             -relx=>0.5, -rely=>($shf2+5*$drh2));
#---
$PedValMn="10.";
$par3_lab=$set_fram->Label(-text=>"PedValMn:",-font=>$font2,-relief=>'groove')
                                                    ->place(-relwidth=>0.3, -relheight=>$drh2,
                                                             -relx=>0, -rely=>($shf2+6*$drh2));
$par3_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",
                                                                -background=>yellow,
                                                                -font=>$font3,
                                                                -textvariable=>\$PedValMn)->place(
                                                                -relwidth=>0.2, -relheight=>$drh2,  
                                                                -relx=>0.3, -rely=>($shf2+6*$drh2));
#---
$par4_lab=$set_fram->Label(-text=>"PedValMx:",-font=>$font2,-relief=>'groove')
                                                    ->place(-relwidth=>0.3, -relheight=>$drh2,
                                                             -relx=>0.5, -rely=>($shf2+6*$drh2));
$par4_e_state='disabled';
$par4_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",
                                                                -background=>yellow,
                                                                -font=>$font3,
								-state=>$par4_e_state,
                                                                -textvariable=>\$PedValMx)->place(
                                                                -relwidth=>0.2, -relheight=>$drh2,  
                                                                -relx=>0.8, -rely=>($shf2+6*$drh2));
#---
$par5_lab=$set_fram->Label(-text=>"TruncCut:",-font=>$font2,-relief=>'groove')
                                                    ->place(-relwidth=>0.3, -relheight=>$drh2,
                                                             -relx=>0, -rely=>($shf2+7*$drh2));
#---
$par5_e_state='disabled';
$par5_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",
                                                                -background=>yellow,
                                                                -font=>$font3,
								-state=>$par5_e_state,
                                                                -textvariable=>\$TruncCut)->place(
                                                                -relwidth=>0.2, -relheight=>$drh2,  
                                                                -relx=>0.3, -rely=>($shf2+7*$drh2));
#---
$MinEvsInFile=10000;#just to avoid rubbish
$Evs2Read=2000;
$par6_lab=$set_fram->Label(-text=>"Evs2Read:",-font=>$font2,-relief=>'groove')
                                                    ->place(-relwidth=>0.3, -relheight=>$drh2,
                                                             -relx=>0.5, -rely=>($shf2+7*$drh2));
$par6_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",
                                                                -background=>yellow,
                                                                -font=>$font3,
                                                                -textvariable=>\$Evs2Read)->place(
                                                                -relwidth=>0.2, -relheight=>$drh2,  
                                                                -relx=>0.8, -rely=>($shf2+7*$drh2));
#------
$scan_b_state='disabled';
$scanbt=$set_fram->Button(-text => "ScanDaqDir", -font=>$font2, 
			      -state=>$scan_b_state,
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
			      -foreground=>"red",
			      -background=>"green",
                              -borderwidth=>5,-relief=>'raised',
			      -cursor=>hand2,
                              -command => \&scanddir)->place(
                                         -relwidth=>0.333,-relheight=>$drh2,
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
                              -command =>\&edit_rlist)->place(
                                                 -relwidth=>0.333, -relheight=>$drh2,
						 -relx=>0.333, -rely=>($shf2+8*$drh2));
$editbt->bind("<Button-3>", \&editrlist_help);
#---
$set_fram->Button(-text=>"EditJobScript", -font=>$font2, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>3,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&EditJobScript)
			                  ->place(
                                          -relwidth=>0.333, -relheight=>$drh2,  
                                          -relx=>0.666, -rely=>($shf2+8*$drh2));
#--------------
#frame_progr:
$prg_fram=$mwnd->Frame(-label=>"PedCalc-job progress :", -relief=>'groove', -borderwidth=>5)->place(
                                                      -relwidth=>0.3, -relheight=>$prgfrheight,
                                                      -relx=>0, -rely=>($dirfrheight+$setfrheight));
$perc_done=0.;
$prg_but=$prg_fram->ProgressBar( -width=>10, -from=>0, -to=>100, -blocks=>100,
                                 -colors=>[0,'green'], -gap=> 0,
                                -variable=>\$perc_done)->place(-relwidth=>0.99, -relheight=>0.5,
                                                             -relx=>0, -rely=>0.45);
#--------------
#job_control frame:
$jcl_fram=$mwnd->Frame(-label=>"PedCalib-Job control :",-background => "red",
                                                       -relief=>'groove', -borderwidth=>3)
						       ->place(
                                                       -relwidth=>0.3, -relheight=>$jclfrheight,
                                                       -relx=>0, -rely=>($dirfrheight+$setfrheight+$prgfrheight));
#---
$stjob_b_state='disabled';
$sjobbt=$jcl_fram->Button(-text => "StartJob", -font=>$font2, 
			      -state=>$stjob_b_state,
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
			      -background=>"green",
                              -borderwidth=>3,-relief=>'raised',
			      -cursor=>hand2,
                              -command => \&StartJob)->place(
                                         -relx=>0,-rely=>0,
                                         -relwidth=>0.333,-relheight=>1);
$sjobbt->bind("<Button-3>", \&startjob_help);
#---
$checkbtxt="StopJob";
$checkbt=$jcl_fram->Button(-text => $checkbtxt, -font=>$font2, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
			      -background=>"orange",
                              -borderwidth=>3,-relief=>'raised',
			      -cursor=>hand2,
			      -textvariable=>\$checkbtxt,
                              -command => \&checkq)->place(
                                         -relx=>0.333,-rely=>0,
                                         -relwidth=>0.333,-relheight=>1);
$checkbt->bind("<Button-3>", \&checkbt_help);
#---
$quits_bt=$jcl_fram->Button(-text => "QuitSess", -font=>$font2,
                               -activebackground=>"yellow",
			       -activeforeground=>"red",
			       -background=>"green",
                               -borderwidth=>3,-relief=>'raised',
			       -cursor=>hand2,
                               -command => \&quitsess)->place(
                                         -relx=>0.666,-rely=>0,
                                         -relwidth=>0.333,-relheight=>1);
$quits_bt->bind("<ButtonRelease-3>", \&quits_help);
}
#------------------------------------------------------
sub open_help_window
{
  my $helpfile="TofAccPedCalcHelp.txt";
  my $hwdszx=0.4;#X-size of genhelp window (portion of display x-size)
  my $hwdszy=0.45;#Y-size of genhelp window (portion of display y-size)
  my $szx=int($hwdszx*$displx);#in pixels
  my $szy=int($hwdszy*$disply);
  my $hwdpx=int(0.01*$disply);# 2nd wind. pos-x in pixels(0.01->safety)
  my $hwdposx="-".$hwdpx;
  my $fn;
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
  $fn=$workdir."/".$helpfile;
  open(HFN,"< $fn") or die show_warn("   <-- Cannot open $helpfile for reading !");
  while(<HFN>){
    $helptext->insert('end',$_);
    $helptext->yview('end');
  }
  close(HFN);
  $helptext->see("1.0");
  $helptext->tagConfigure('lookat',-foreground=> red);
}
#-------------
sub scanddir_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
#    $helpwindow->deiconify();
    $helpwindow->after(500);
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
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
#    $helpwindow->deiconify();
    $helpwindow->after(500);
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
sub startjob_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
#    $helpwindow->deiconify();
    $helpwindow->after(500);
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "StartJob-Button:",'end');
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
sub checkbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
#    $helpwindow->deiconify();
    $helpwindow->after(500);
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "StopJob/Check-Button:",'end');
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
sub upddbbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
#    $helpwindow->deiconify();
    $helpwindow->after(500);
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "UpdDB-Button:",'end');
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
sub archbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
#    $helpwindow->deiconify();
    $helpwindow->after(500);
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "ArchiveOnExit-Button:",'end');
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
sub arcscbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
#    $helpwindow->deiconify();
    $helpwindow->after(500);
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "ScanArchive-Button:",'end');
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
sub quits_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->after(500);
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
#-------------
sub resplbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->after(500);
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "ResetPlot-Button:",'end');
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
sub dlenplbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->after(500);
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "NonEmpty/GoogStat-channels(%)-Button:",'end');
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
sub ptscplbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->after(500);
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "PedTimeScanPlot-Button:",'end');
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
sub stscplbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->after(500);
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "PedRmsTimeScanPlot-Button:",'end');
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
sub fpedplbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->after(500);
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "OneFilePedsPlot-Button:",'end');
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
#----------------------Welcome/LookAroundArea------------------------------
sub PEDC_Welcome{
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
  $line="       ---------- WELCOME to Tof/Acc Pedestals calibration !!! ----------\n\n";
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
}   
#------------------------------------------------------------------------------
sub setRunDefs
{
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time,$line,$fname);
#
  $line="----> Setting Run Default-parameters : \n\n";
  $logtext->insert('end',$line);
  $logtext->yview('end');
#
  $SubDetMsk=8;#bit-patt: (msb T8/U2/S4/R4/E2/L1 lsb)
  if($pedcaltyp eq "Data"){
     if($tofantsel eq "useTOF"){$TruncCut="0.15";}
     else{$TruncCut="0.05";}
     $par5_e_state='normal';
     $stjob_b_state='normal';
     $Evs2Read=4000;
     $MinEvsInFile=10000;# just to avoid rubbish runs
     $RunType="SCI";
     $FileSize=30;#mb
  }
  else{#OnBoardTable
     $TruncCut="0";
     $par5_e_state='disabled';
     $stjob_b_state='normal';
     $Evs2Read=500;
     $MinEvsInFile=1;
     $RunType="CAL";
     $FileSize=0.5;#mb
  }
#--
  if($tofantsel eq "useTOF"){
    $detname="tofp";
    $fname="TofDaqfHist";
    $PedRmsMx="10.";
    $par2_e_state='normal';
    $PedValMx="700.";
    $par4_e_state='normal';
  }
  else{
    $detname="antp";
    $fname="AccDaqfHist";
    $PedRmsMx="10.";
    $par2_e_state='normal';
    $PedValMx="700.";
    $par4_e_state='normal';
  }
  $PrevRunsFN=$fname."_".$pedcaltyp.".".$SessName;
# ---> Open previously processed DAQ-files list:
  @PrevRuns=();
  $NPrevRuns=0;
  open(INPFN,"< $PrevRunsFN") or show_warn("   <-- Cannot open $PrevRunsFN for reading $!");
  while(defined ($line = <INPFN>)){
    chomp $line;
    if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
    $line =~s/^\s+//s;# remove " "'s at the beg.of line
    push(@PrevRuns,$line);
  }
  close(INPFN) or show_warn("   <-- Cannot close $PrevRunsFN after reading $!");
  $NPrevRuns=scalar(@PrevRuns);
#--
  $line="   <-- Found $NPrevRuns files in DAQ-processing history ($tofantsel/$pedcaltyp Mode) : \n\n";
  $logtext->insert('end',$line);
  $logtext->yview('end');
  for($i=0;$i<$NPrevRuns;$i++){#<--- loop over list to show content
    $ptime=localtime($PrevRuns[$i]);#local time (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
#
    $time="yyyy/mm/dd=".$year."/".$month."/".$day." hh:mm:ss=".$hour.":".$min.":".$sec;
    $line="Run: ".$PrevRuns[$i]."  Time: ".$time."\n";
    $logtext->insert('end',$line);
    $logtext->yview('end');
  }
  $mwnd->update;
#---
  if($files ne "All"){#single known daq-file run#
#
    $line="<---- Use unique requested daq-file as defStart/defEnd \n\n";
    $logtext->insert('end',$line);
    $logtext->yview('end');
#
    $fnum1=$files;
    $fnum2=$fnum1;
    $fdat1=run2time($fnum1);
    $fdat2=run2time($fnum2);
    $fnum1o=$fnum1;
    $fnum2o=$fnum2;
    $fdat1o=$fdat1;
    $fdat2o=$fdat2;
    print "$files $fnum1 $fnum2\n";
  }
  else{
    if($NPrevRuns > 0){
      $line="<---- Use last processed Run+1 as defStart(+2weeks-> defEnd) \n\n";
      $logtext->insert('end',$line);
      $logtext->yview('end');
      $fnum1=$PrevRuns[$NPrevRuns-1]+1;
      $fnum2=$fnum1+1209600;# +2 weeks
      $fdat1=run2time($fnum1);
      $fdat2=run2time($fnum2);
    }
    else{
      $line="<---- Use defStartDate/defEndDate as defStart/defEnd \n\n";
      $logtext->insert('end',$line);
      $logtext->yview('end');
      $fnum1=time2run($fdat1);
      $fnum2=time2run($fdat2);
    }
    $fnum1o=$fnum1;
    $fnum2o=$fnum2;
    $fdat1o=$fdat1;
    $fdat2o=$fdat2;
  }
#
#--> redefine some run-dependent params:
#
  if($fnum2<1230764399){#--->2008
    $SetupN="AMS02PreAss";
  }
  elsif($fnum1>1230764399 && $fnum2<1262300400){#--->2009
    $SetupN="AMS02Ass1";
  }
  elsif($fnum1>1262300400 && $fnum2>1262300400 && $fnum2<1284069601){#--->2010 < 10sept
    $SetupN="AMS02Ass1";
  } 
  elsif{$fnum2>1284069601 && $fnum2>1284069601){#--->2010 > 10sept
    $SetupN="AMS02Space";
  }
  else{
    show_warn("\n   <-- Wrong dates(runs) range definition, please correct it !!!");
    return;
  }
#---
  $sjobbt->configure(-state=>$stjob_b_state);
  $scan_b_state='normal';
  $scanbt->configure(-state=>$scan_b_state);
  $par2_ent->configure(-state=>$par2_e_state);
  $par4_ent->configure(-state=>$par4_e_state);
  $par5_ent->configure(-state=>$par5_e_state);
  $mwnd->update;
}# ---> endof sub:setRunDefs
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
#	print "type=",$type," Dtyp=",$rtype," Conf=",$conf," SubDetPat=",$subdetpat," Rtype=",$rtype," fsize=",$sizemb,"\n";
	if($rtype eq $RunType){#<-- use only requested runtype(LAS,SCI,CAL or UNK)
          $curline="  Run/Tag/RType:".$run." ".$tag." ".$type."  StTime=".$fdat." Evs:".$nevents."  Path:".$path."\n";
          $logtext->insert('end',$curline);
          $logtext->yview('end');
          $daqfstat[$ndaqfound]=1;#status=found
	  if(($sizemb>$FileSize)
	              && ($nevents>=$MinEvsInFile)
#tempor		      && (($subdetpat & $SubDetMsk) == $SubDetMsk)
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
    $sjobbt_state="normal";
    $sjobbt->configure(-state=>"normal");
  }
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
sub open_browser_window{ # open mozilla browser
#
  my $stat=0;
  $stat = system("/usr/bin/mozilla");#<-- call mozilla 
  if($stat != 0) {die  show_warn("   <-- Mozilla-browser exited badly $!");}
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
sub StartJob
{
#
  if($ndaqgood==0){
    show_warn("   <-- Missing good daq-files !?");
    return;
  }
#
  my @daqfstatr=();
  my ($logfn,$detname,$pedtyp,$logfn,$pedrlogf,$daqfn,$daqfpath);
  if($tofantsel eq "useTOF"){$detname="tofp";}
  else{$detname="antp";}
  if($pedcaltyp eq "Rand"){$pedtyp="cl";}
  elsif($pedcaltyp eq "Data"){$pedtyp="ds";}
  else{$pedtyp="tb";}
  $logfn=$detname."_".$pedtyp."_rl.";
  $PedFName=$logfn;# is common part of all ped-files names (i.e. tofp[antp]_ds[tb,cl]_rl.)
#
  $percent=0;
  $perc_done=0;
  $nsuccruns=0;
  $nprocruns=0;
  $stopjob=0;
#
#---> clean amsjobwd-dir from old ped/ped.log-files:
  my $stat=0;
  my $dir=$workdir.$amsjwd;
  my $fn2clear=$dir."/".$PedFName."*";
  $stat = system("rm -f $fn2clear");
  if($stat != 0){show_warn("   <-- There are no any old ped-files to clear in amsjobwd-dir !");}
#
  $RunNum+=1;
  $curline="===========> Start PedCalib-Job $RunNum to extract ped-info...\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
  $mwnd->update;
#
  $daqfrunmn=4294000000;#put max to find min
  push(@daqfstatr,@daqfstat);#save to tempor.array for possible re-StartJob without DAQ-dir rescan.
#
#---> Run gbatch for each selected daq-file:
# (each run will create his own log-file like tofp_tb_rl.nnnn.log in amsjobwd)
#
  for($i=0;$i<$ndaqfound;$i++){#<--- selected daq-files loop
    if($daqfstat[$i] == 11){#selected
      $nprocruns+=1;
      $pedrlogf=$dir."/".$logfn.$daqfrunn[$i].".log";#logf-path
      open(RUNLOGF,"+> $pedrlogf") or die show_warn("   <-- Cannot open $pedrlogf  !");# clear file if exists
      system("date >> $pedrlogf");
#      $daqfn=$daqfpath[$i].$daqfrunn[$i];#exact path to daq-fn, for better reliability use generic one.
      $daqfn=$daqfrunn[$i];#daq-fn
      $daqfpath=$DaqDataDir."/".$RunType."/".$daqfn;# generic path=/Offline/RunsDir/SCI(CAL)/runnumber
      $status = system("$JobScriptN $daqfpath $detname $pedcaltyp $Evs2Read $TruncCut $PedValMn $PedValMx $PedRmsMn $PedRmsMx $SetupN>> $pedrlogf"); 
      if($status != 0){
        show_warn("   <-- $JobScriptN exited badly for file $daqfrunn[$i]!");
	$daqfstat[$i]=211;
	print RUNLOGF "<=== This Ped-run $daqfrunn[$i] is unsuccessful !!!\n"; 
      }
      else{
        $daqfstat[$i]=111;
	$nsuccruns+=1;
	if($daqfrunn[$i] <= $daqfrunmn){$daqfrunmn=$daqfrunn[$i];}# find min. good run#
      }
      $curline="   <== Daq-file $daqfn is processed !\n\n";
      $logtext->insert('end',$curline);
      $logtext->yview('end');
      close(RUNLOGF) or show_warn("   <-- Cannot close RUNLOGF !");
      $percent+=1.;
    }#--->endof "selected-file" check
#
    $perc_done=100*($percent/$ndaqgood);
    $mwnd->update;
    if($stopjob!=0){
      show_warn("   <-- PedCalib-Job $RunNum was interrupted by user !!!");
      last;
    }
  }#--->endof daq-files loop
#
  if($stopjob==0){
    $curline="<=========== PedCalib-Job finished with $nsuccruns successful runs from $nprocruns processed !\n\n";
    $logtext->insert('end',$curline);
    $logtext->yview('end');
    if($nsuccruns==0){
      show_warn("   <-- No good runs ! Check conditions and rescan DAQ-dirs or edit run-list accordingly !");
    }
    else{
      $checkbtxt="CheckJob";
      $checkbt->configure(-background => "green");
      show_warn(" ");
    }
  }
  else{
    $stopjob=0;
    @daqfstat=();
    push(@daqfstat,@daqfstatr);#restore statuses to original when job was interrupted by user
#    for($i=0;$i<$ndaqfound;$i++){print "$daqfstat[$i]\n";}
    show_warn("   <-- Change conditions and rescan DAQ-dirs or just re-StartJob if run-list ok !!!");
  }
  $mwnd->update;
}
#----------------------------------------------------------------
sub checkq{# create TopLevel with canvas and control panel
#
if($checkbtxt eq "StopJob"){
  $stopjob=1;
#  $checkbt->configure(-background => "orange");
  return;
}
if(! Exists($topl1)){# <-- define size and create 2nd window:
  $topl1=$mwnd->Toplevel();
  $swdszx=$mnwdszx;#X-size of 2nd window (portion of display x-size)
  $swdszy=1-$mnwdszy-0.11;#Y-size of 2nd window (portion of display y-size)(0.1->bot.syst.panel)
  my $szx=int($swdszx*$displx);#in pixels
  my $szy=int($swdszy*$disply);
  my $swdposx="+0";
  my $swdpy=int(($mnwdszy+0.03)*$disply);# 2nd wind. pos-y in pixels(0.03->safety)
  my $swdpozy="+".$swdpy;
  my $swdgeom=$szx."x".$szy.$swdposx.$swdpozy;
  print "CANVAS-window geom-",$swdgeom,"\n";
  $topl1->geometry($swdgeom);
  $topl1->title("Peds Analysis");
#
  my $cnvrsz=0.7;
  my $bheight=0.1;
  my $bspac=0.02;
  my $crely;
  $cp_fram=$topl1->Frame(-label => "Control panel",-background => "Cyan",  
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>(1-$cnvrsz), -relheight=>1,
                                                    -relx=>0, -rely=>0);
  $crely=0.1;
  $resplbt=$cp_fram->Button(-text=>"Reset plot",               -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&points_clear)->place(
                                                                 -relwidth=>0.25, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>0.9);
  $resplbt->bind("<Button-3>", \&resplbt_help);
#----
  $cp_fram->Button(-text=>"Save plot",               -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&plot_save)->place(
                                                                 -relwidth=>0.25, -relheight=>$bheight,
                                                                           -relx=>0.25, -rely=>0.9);
#----
  $cp_fram->Button(-text=>"Close",                      -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>sub{$topl1->withdraw();})->place(
                                                                 -relwidth=>0.25, -relheight=>$bheight,
                                                                           -relx=>0.75, -rely=>0.9);
#----
  $dlenplbt=$cp_fram->Button(-text=>"NonEmpty/GoodStat-channels(%) time-scan",        
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&dlen_tscan)->place(
                                                                 -relwidth=>1, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $crely+=($bheight+$bspac);
  $dlenplbt->bind("<Button-3>", \&dlenplbt_help);
#----
  $stscplbt=$cp_fram->Button(-text=>"PedRms time-scan for Ch_ID(LBBSP) :",
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&sig_tscan)->place(
                                                                 -relwidth=>1, -relheight=>$bheight,
                                                                    -relx=>0, -rely=>$crely);
  $crely+=($bheight);
  $stscplbt->bind("<Button-3>", \&stscplbt_help);
#----
  $ptscplbt=$cp_fram->Button(-text=>"Ped+-Rms time-scan for Ch_ID(LBBSP) :",-borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ped_tscan)->place(
                                                                 -relwidth=>1, -relheight=>$bheight,
                                                                    -relx=>0, -rely=>$crely);
  $crely+=($bheight);
  $ptscplbt->bind("<Button-3>", \&ptscplbt_help);
#----
if($tofantsel eq "useTOF"){
  $chidvar="10410(LBBSP)";
}
else{
  $chidvar="410(BSP)";
}
$cp_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$font3,
                                              -textvariable=>\$chidvar)->place(
                                              -relwidth=>1, -relheight=>$bheight,  
                                              -relx=>0, -rely=>$crely);
  $crely+=($bheight+$bspac);
#----
  $fpedplbt=$cp_fram->Button(-text=>"Plot All Channels Peds for file name :",  -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&file_scan)->place(
                                                                 -relwidth=>1, -relheight=>$bheight,
                                                                 -relx=>0, -rely=>$crely);
  $crely+=($bheight);
  $fpedplbt->bind("<Button-3>", \&fpedplbt_help);
#----
$fileidvar="";
$fentrw=$cp_fram->Entry(-relief=>'sunken', -background=>yellow,
#                                              -foreground=>"blue",
                                              -font=>$font3,
                                              -textvariable=>\$fileidvar)->place(
                                              -relwidth=>1, -relheight=>$bheight,  
                                              -relx=>0, -rely=>$crely);
  $crely+=($bheight);
#----
  $file_remove_btext="Mark above file as bad";
  $cp_fram->Button(-textvariable=>\$file_remove_btext,  -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&mark_file)->place(
                                                                 -relwidth=>0.5, -relheight=>$bheight,
                                                                           -relx=>0.5, -rely=>$crely);
#--
  $file_view_btext="View above file JobLog";
  $cp_fram->Button(-textvariable=>\$file_view_btext,  -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&view_JobLog)->place(
                                                                 -relwidth=>0.5, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $crely+=($bheight+$bspac);
#---
  $cv_fram=$topl1->Frame(-label => "Params time-evolution window", 
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>$cnvrsz, -relheight=>1,
                                                    -relx=>(1-$cnvrsz), -rely=>0);
  $canscrx1=-40;# scroll-area (min/max), all in pixels
  $canscry1=-500;
  $canscrx2=10000;#at $binw_tev=120sec corresp.full time-scale almost 2weeks
  $canscry2=20;
  $cnv=$cv_fram->Scrolled("Canvas",-background => "yellow",
                                        -scrollregion=>[$canscrx1,$canscry1,
					                $canscrx2,$canscry2])
                                                              ->pack(-fill => 'both', -expand => 1);
  $canvas=$cnv->Subwidget("canvas");
    
  $xaxe=$canvas->createLine($canscrx1,0,$canscrx2,0, -fill=>"black", -arrow=>"last", -width=>2);
  $yaxe=$canvas->createLine(0,$canscry2,0,$canscry1, -fill=>"black", -arrow=>"last", -width=>2);
#---
  }
  else{
    $topl1->deiconify();
    $topl1->raise();
  }
  $plot1_actf=0;#plot1 "active" flag
  $plot2_actf=0;#plot2 "active" flag
  $plot3_actf=0;#plot3 "active" flag
  $plot4_actf=0;#plot4 "active" flag
  $topl1->bell;
#  print "\aQUQU\n";
}
#-------------
sub points_clear #<--- delete canvas-items in plot's point-ids list 
{
  if($plot_pntmem > 0){
    for($i=0;$i<$plot_pntmem;$i++){$canvas->delete($plot_pntids[$i]);}
  }
  $plot_pntmem = 0;
}
#-------------
sub access_clear #<--- delete canvas-items in plot's accessories-ids list 
{
  if($plot_accmem > 0){
    for($i=0;$i<$plot_accmem;$i++){$canvas->delete($plot_accids[$i]);}
  }
  $plot_accmem = 0;
}
#-------------
sub plot_save #<--- current plot in ps-file 
{
  my ($fname,$width,$height,$xcmax);
  $height=$canscry2-$canscry1;
#
  if($plot1_actf>0){
    $fname="plot_1_v".$plot1_actf.".ps";
    $width=($plot1_xcmax-$canscrx1)+10;
    if($width>($canscrx2-$canscrx1)){$width=$canscrx2-$canscrx1;}
    $width=($canscrx2-$canscrx1);
    $xcmax=$plot1_xcmax;
  }
  elsif($plot2_actf>0){
    $fname="plot_2_v".$plot2_actf.".ps";
    $width=($plot2_xcmax-$canscrx1)+10;
    $width=($canscrx2-$canscrx1);
    $xcmax=$plot2_xcmax;
  }
  elsif($plot3_actf>0){
    $fname="plot_3_v".$plot3_actf.".ps";
    $width=($plot3_xcmax-$canscrx1)+10;
    $width=($canscrx2-$canscrx1);
    $xcmax=$plot3_xcmax;
  }
  elsif($plot4_actf>0){
    $fname="plot_4_v".$plot4_actf.".ps";
    $width=($plot4_xcmax-$canscrx1)+50;
    $xcmax=$plot4_xcmax;
  }
  $canvas->postscript(-file=>$fname,-width=>$width,-height=>$height,-x=>$canscrx1,-y=>$canscry1);
  $curline="   <-- Plot is saved in $fname\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
}
#-----------------------------------------------------------------
sub dlen_tscan
{
#---> this is plot-1
#
#---> time-evolution of file's NonEmpCh/TotChannels(%) and StatOkCh/NonEmpChannels(%)
#
  show_warn("");
  my $layx,$barx,$sidx,$pmtx,$rest;
  my $maxb;
  my $locvar;
  my @gcolors=qw(green darkred blue orange black );
  $curline="   <-- NonEmpy/GoodStat-Channels(%) Time-scan \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
#
  my ($ped,$sig,$sta)=(0,0,0);
  my ($totcha,$totchd,$nempcha,$nempchd,$gstacha,$gstachd)=(0,0,0,0,0,0);
  my ($nemppora,$nemppord,$gstapora,$gstapord);
  my($laymx,$barmx,$pmmx);
  if($tofantsel eq "useTOF"){$laymx=4;}
  if($tofantsel eq "useANT"){$laymx=1;}
  my $seqnum;
  my ($top,$bot,$mid);
#
  my $goodc="darkgreen";
  my $badc="red";
  my $nzlines=0;
  my $curcol;
  my $colindx;
#
  @globpeds=();
#-----
  my ($xc,$yc)=(0,0);
  my ($xcoarsegrd,$xfinegrd)=(86400,3600);# day/hour in sec
  my ($ycoarsegrd,$yfinegrd)=(50,10);#(%)
  my $ymaxv=125;#(125%)
  my $ypbin=int(-$canscry1/$ymaxv+0.5);#(pixels/bin)
  my $ycoarsegrdp=int(-$canscry1*$ycoarsegrd/$ymaxv+0.5);#coarse-grid in pix
  my $yfinegrdp=int(-$canscry1*$yfinegrd/$ymaxv+0.5);#fine-grid in pix
#
  my $vcoo;
  my ($width,$ltxt,$dig,$colr,$vpos,$dasht,$titl);
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time);
#
#---> create scale/grid:
#
  if($plot1_actf==0){# 1st entry for plot2
    &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
#-- create Grid vertical lines + X-axes subscripts :
    for($i=0;$i<$canscrx2;$i++){
       if($i>0 && ($i*$binw_tev-int($i*$binw_tev))==0 && ((int($i*$binw_tev) % $xfinegrd)==0 || (int($i*$binw_tev) % $xcoarsegrd)==0)){
         $xc=$i;
	 if(($i*$binw_tev % $xcoarsegrd)>0){# hour-line
	   $width=1;
	   $dig=(($i*$binw_tev % $xcoarsegrd)/$xfinegrd);
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	 }
	 else{# day-line
	   $width=1;
	   $dig=($i*$binw_tev/$xcoarsegrd);
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	 }
         $lid=$canvas->createLine($xc,0,$xc,$canscry1, -fill=>'black', -width=>$width,-dash=>$dasht);
         $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
         $plot_accmem+=1;
         $lid=$canvas->createText($xc,+10,-text=>$ltxt,-font=>$font7,-fill=>$colr);  
         $plot_accids[$plot_accmem]=$lid;#store in glob.list
         $plot_accmem+=1;
       }
    }
#-- create Grid horizontal lines + Y-axes subscripts :
    for($i=0;$i<-$canscry1;$i++){
       if($i>0 && (($i % $yfinegrdp)==0 || ($i % $ycoarsegrdp)==0)){
         $yc=$i;
	 if(($i % $ycoarsegrdp)==0){# coarse scale line
	   $width=1;
	   $dig=$i*$ycoarsegrd/$ycoarsegrdp;
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	 }
	 else{# fine scale line
	   $width=1;
	   $dig=$i*$yfinegrd/$yfinegrdp;
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	 }
         $lid=$canvas->createLine(0,-$yc,$canscrx2,-$yc, -fill=>'black', -width=>$width,-dash=>$dasht);
         $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
         $plot_accmem+=1;
         $lid=$canvas->createText(-20,-$yc,-text=>$ltxt,-font=>$font7,-fill=>$colr);  
         $plot_accids[$plot_accmem]=$lid;#store in glob.list
         $plot_accmem+=1;
       }
    }
#-- add 0 :
    $lid=$canvas->createText(-10,10,-text=>"0",-font=>$font7,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#-- add title :
    $ptime=localtime($daqfrunmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time=$year."/".$month."/".$day."  ".$hour.":".$min;
    $titl="NonEmpty(blue)/GoodStat(green)-Channels(%) vs time,  '0'-time is $time";
    $vcoo=int(120*$canscry1/$ymaxv);#place at val=120%
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl="         (oval->Anodes, rectangle->Dynodes)";
    $vcoo=int(110*$canscry1/$ymaxv);#place at val=110%
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl="Time-scale: red digits ->days, blue ->hours";
    $vcoo=int(5*$canscry1/$ymaxv);#place at val=5%
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#
    $plot2_actf=0;
    $plot3_actf=0;
    $plot4_actf=0;
  }
  $plot1_actf+=1;
#
#
  for($i=0;$i<$ndaqfound;$i++){#
    $sfname=$workdir.$amsjwd."/".$PedFName.$daqfrunn[$i];
    $nzlines=0;
    $nwords=0;
    @globpeds=();#store all data of the file
    if($daqfstat[$i] == 111){
      open(INPFN,"< $sfname") or show_warn_setfstat("   <-- Cannot open $sfname for reading $!",$i);
      if($daqfstat[$i] == 211){
        show_warn("   <-- Missing file $sfname !");
#	next;
      }
      else {
        while(defined ($line = <INPFN>)){
          chomp $line;
          if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
          if($line =~/==========/){last;}# break on my internal EOF
          if($line =~/12345/){last;}# break on my internal EOF
	  $nzlines+=1;
	  $line =~s/^\s+//s;# remove " "'s at the beg.of line
	  @words=split(/\s+/,$line);#split by any number of continious " "'s
	  $nwords=scalar(@words);
	  push(@globpeds,@words);
        }
        close(INPFN) or show_warn("   <-- Cannot open $sfname after reading $!");
      }
#--
      $seqnum=0;
#--->first - anodes:
      $totcha=0;
      $nempcha=0;
      $gstacha=0;
      $nemppora=0;
      $gstapora=0;
      for($il=0;$il<$laymx;$il++){
        if($tofantsel eq "useTOF"){$barmx=$barsppl[$il];}
        else {$barmx=8;}
        for($ib=0;$ib<$barmx;$ib++){
          for($is=0;$is<2;$is++){
	    $totcha+=1;
	    if($tofantsel eq "useTOF"){
	      $sta=$globpeds[$seqnum+3*$is];
	      $ped=$globpeds[$seqnum+3*$is+1];
	      $sig=$globpeds[$seqnum+3*$is+2];
	    }
	    else{
	      $sta=$globpeds[$seqnum+$is];
	      $ped=$globpeds[$seqnum+$is+2];
	      $sig=$globpeds[$seqnum+$is+4];
	    }
	    if($ped>0 && $sig>0){
	      $nempcha+=1;
	      if($sta==0){$gstacha+=1;}
	    }
	  }#-->endof side-loop
	  $seqnum+=6;
#
        }#-->endof bar-loop
#
      }#-->endof lay-loop
      if($totcha>0){$nemppora=100.*$nempcha/$totcha;}
      if($nempcha>0){$gstapora=100.*$gstacha/$nempcha;}
#--->next - dynodes:
      $totchd=0;
      $nempchd=0;
      $gstachd=0;
      $nemppord=0;
      $gstapord=0;
      if($tofantsel eq "useTOF"){#only for TOF
        for($il=0;$il<$laymx;$il++){
          $barmx=$barsppl[$il];
          for($ib=0;$ib<$barmx;$ib++){
	    if(($il==0 || $il==($laymx-1)) && ($ib==0 || $ib==($barmx-1))){$pmmx=3;}
	    else{$pmmx=2;}
            for($is=0;$is<2;$is++){
              for($pm=0;$pm<3;$pm++){
	        if($pm<$pmmx){
	          $totchd+=1;
	          $sta=$globpeds[$seqnum];
	          $ped=$globpeds[$seqnum+1];
	          $sig=$globpeds[$seqnum+2];
#
	          if($ped>0 && $sig>0){
	            $nempchd+=1;
	            if($sta==0){$gstachd+=1;}
	          }
	        }
	        $seqnum+=3;
	      }#-->endof pm-loop
	    }#-->endof side-loop
#
          }#-->endof bar-loop
#
        }#-->endof lay-loop
        if($totchd>0){$nemppord=100.*$nempchd/$totchd};
        if($nempchd>0){$gstapord=100.*$gstachd/$nempchd;}
      }#--->endof "tof"-check
#--
      $xc=($daqfrunn[$i]-$daqfrunmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
#
#anodes:
      $vcoo=int(-$nemppora*$canscry1/$ymaxv+0.5);# pos.in pix
      $curcol=$gcolors[2];#blue
      $lid=$canvas->createOval($xc-3,-($vcoo-4),$xc+3,-($vcoo+4), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
      $vcoo=int(-$gstapora*$canscry1/$ymaxv+0.5);
      if($gstapora>90){$curcol=$gcolors[0];}#green
      else{$curcol=$gcolors[1];}#red
      $lid=$canvas->createOval($xc-3,-($vcoo-4),$xc+3,-($vcoo+4), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
#dynodes:
      if($tofantsel eq "useTOF"){
        $vcoo=int(-$nemppord*$canscry1/$ymaxv+0.5);
        $curcol=$gcolors[2];#blue
        $lid=$canvas->createRectangle($xc-3,-($vcoo-2),$xc+3,-($vcoo+2), -fill=>$curcol, -width=>1);
        $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
        $plot_fileids[$plot_pntmem]=$i;#store file-index
        $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
        $plot_pntmem+=1;
        $vcoo=int(-$gstapord*$canscry1/$ymaxv+0.5);
        if($gstapord>90){$curcol=$gcolors[0];}#green
        else{$curcol=$gcolors[1];}#red
        $lid=$canvas->createRectangle($xc-3,-($vcoo-2),$xc+3,-($vcoo+2), -fill=>$curcol, -width=>1);
        $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
        $plot_fileids[$plot_pntmem]=$i;#store file-index
        $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
        $plot_pntmem+=1;
      }
#
      if($xc>$plot1_xcmax){$plot1_xcmax=$xc;}
    }
    $topl1->update;
  }
  $curline="   <-- NonEmpt/GoodStat-channels(%) scan was completed ! \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
}
#------------------------------------------------------------------
sub view_JobLog
{
#
#--- View displayed(after its peds scan) file:
#
  if($fileidvar ne ""){
    my $stat=0;
    my $fname=$workdir.$amsjwd."/".$fileidvar.".log";
    $stat=system("nedit -read $fname");
    if($stat != 0){
      show_warn("   <-- View_JobLog Error: fail to view file !");
      return;
    }
  }
  else{
    show_warn("   <-- View_JobLog Error: missing input file name !");
    return;
  }
}
#------------------------------------------------------------------
sub ped_tscan
{
#---> time-evolution of particular channel ped+-sig, this is plot-3
#
  show_warn("");
  my $layx,$barx,$sidx,$pmtx,$rest;
  my $maxb;
  my $locvar;
  my @goodcolorlist=qw(green darkred blue orange black );
  $locvar=$chidvar;
#--> check ID :
  if($tofantsel eq "useTOF"){
    if($locvar =~/(LBBSP)/){substr($locvar,-7,7)="";}# remove "(LBBSP)" if present
    ($layx,$barx,$sidx,$pmtx,$rest)=unpack("A1 A2 A1 A1 A*",$locvar);
    if($layx>4 || $layx==0 || $barx==0 || $barx>$barsppl[$layx-1] || $sidx==0 || $sidx>2 || $pmtx>3){
      show_warn("   <-- Wrong TOF channel-ID setting: $locvar ?!");
      return;
    }
  }
  if($tofantsel eq "useANT"){
    if($locvar =~/(BSP)/){substr($locvar,-5,5)="";}# remove "(BSP)" if present
    ($barx,$sidx,$pmtx,$rest)=unpack("A1 A1 A1 A*",$locvar);
    $layx=0;
    if($barx==0 || $barx>8 || $sidx==0 || $sidx>2 || $pmtx>0){
      show_warn("   <-- Wrong ACC channel-ID setting: $locvar ?!");
      return;
    }
  }
  $curline="   <-- Single channel Ped+-Rms Time-scan for id=$locvar, l/bb/s/p=$layx/$barx/$sidx/$pmtx \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
#
  my ($ped,$sig,$sta,$vped,$vsig);
  my $seqnum;
  my ($xc,$yc)=(0,0);
  my ($xcoarsegrd,$xfinegrd)=(86400,3600);# day/hour in sec
  my ($ycoarsegrd,$yfinegrd)=(100,20);#(adc-ch)
  my $ymaxv=400;#(adc-ch)
  my $ypbin=int(-$canscry1/$ymaxv+0.5);#(pixels/bin)
  my $ycoarsegrdp=int(-$canscry1*$ycoarsegrd/$ymaxv+0.5);#coarse-grid in pix
  my $yfinegrdp=int(-$canscry1*$yfinegrd/$ymaxv+0.5);#fine-grid in pix
  my $goodc="darkgreen";
  my $badc="red";
  my $nzlines=0;
  my $curcol;
  my $colindx;
  my ($width,$dig,$ltxt,$colr,$dasht,$titl);
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time);
  my $vcoo;
#
  @globpeds=();
#
#---> create scale/grid:
#
  if($plot3_actf==0){# 1st entry for plot3
    &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
#-- create Grid vertical lines + X-axes subscripts :
    for($i=0;$i<$canscrx2;$i++){
       if($i>0 && (($i*$binw_tev % $xfinegrd)==0 || ($i*$binw_tev % $xcoarsegrd)==0)){
         $xc=$i;
	 if(($i*$binw_tev % $xcoarsegrd)>0){# hour-line
	   $width=1;
	   $dig=(($i*$binw_tev % $xcoarsegrd)/$xfinegrd);
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	 }
	 else{# day-line
	   $width=1;
	   $dig=($i*$binw_tev/$xcoarsegrd);
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	 }
         $lid=$canvas->createLine($xc,0,$xc,$canscry1, -fill=>'black', -width=>$width,-dash=>$dasht);
         $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
         $plot_accmem+=1;
         $lid=$canvas->createText($xc,+10,-text=>$ltxt,-font=>$font2,-fill=>$colr);  
         $plot_accids[$plot_accmem]=$lid;#store in glob.list
         $plot_accmem+=1;
       }
    }
#-- create Grid horizontal lines + Y-axes subscripts :
    for($i=0;$i<-$canscry1;$i++){
       if($i>0 && (($i % $yfinegrdp)==0 || ($i % $ycoarsegrdp)==0)){
         $yc=$i;
	 if(($i % $ycoarsegrdp)==0){# coarse scale line
	   $width=1;
	   $dig=$i*$ycoarsegrd/$ycoarsegrdp;
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	 }
	 else{# fine scale line
	   $width=1;
	   $dig=$i*$yfinegrd/$yfinegrdp;
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	 }
         $lid=$canvas->createLine(0,-$yc,$canscrx2,-$yc, -fill=>'black', -width=>$width,-dash=>$dasht);
         $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
         $plot_accmem+=1;
         $lid=$canvas->createText(-20,-$yc,-text=>$ltxt,-font=>$font2,-fill=>$colr);  
         $plot_accids[$plot_accmem]=$lid;#store in glob.list
         $plot_accmem+=1;
       }
    }
#-- add 0 :
    $lid=$canvas->createText(-10,10,-text=>"0",-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#-- add title :
    $ptime=localtime($daqfrunmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time=$year."/".$month."/".$day."  ".$hour.":".$min;
    $titl="Ped+-Rms Time-evolution,  '0'-time is $time";
    $vcoo=int(340*$canscry1/$ymaxv);#place at val=340 adc-ch
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl="Time-scale: red digits ->days, blue ->hours";
    $vcoo=int(10*$canscry1/$ymaxv);#place at val=10 adc-ch
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#
    $plot1_actf=0;
    $plot2_actf=0;
    $plot4_actf=0;
  }
  $colindx=($plot3_actf % 5);
  $goodc=$goodcolorlist[$colindx];#change color according fill-entry number
  $plot3_actf+=1;
#
  for($i=0;$i<$ndaqfound;$i++){#<--- selected files loop
    $sfname=$workdir.$amsjwd."/".$PedFName.$daqfrunn[$i];
    $nzlines=0;
    $nwords=0;
    @globpeds=();#store all data of the file
    if($daqfstat[$i] == 111){
      open(INPFN,"< $sfname") or show_warn_setfstat("   <-- Cannot open $sfname for reading $!",$i);
      if($daqfstat[$i] == 211){
        show_warn("   <-- Missing file $sfname !");
	next;
      }
      while(defined ($line = <INPFN>)){
        chomp $line;
        if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
        if($line =~/==========/){last;}# break on my internal EOF
        if($line =~/12345/){last;}# break on my internal EOF
	$nzlines+=1;
	$line =~s/^\s+//s;# remove " "'s at the beg.of line
	@words=split(/\s+/,$line);#split by any number of continious " "'s
	$nwords=scalar(@words);
	push(@globpeds,@words);
      }
      close(INPFN) or die show_warn("   <-- Cannot open $sfname after reading $!");
#
      $seqnum=0;
      if($pmtx==0){# anode (TOF/ANTI)
        if($layx>0){# tof
          for($il=0;$il<$layx-1;$il++){$seqnum+=$barsppl[$il]*6;}# count in layers befor layx
          for($ib=0;$ib<$barx-1;$ib++){$seqnum+=6;}# count in paddles befor barx
          for($is=0;$is<$sidx-1;$is++){$seqnum+=3;}# count in sides befor sidx
	  $sta=$globpeds[$seqnum];
	  $ped=$globpeds[$seqnum+1];
	  $sig=$globpeds[$seqnum+2];
	}
	else{# anti
          for($ib=0;$ib<$barx-1;$ib++){$seqnum+=6;}# count in paddles befor barx
          for($is=0;$is<$sidx-1;$is++){$seqnum+=1;}# count in sides befor sidx
	  $sta=$globpeds[$seqnum];
	  $ped=$globpeds[$seqnum+2];
	  $sig=$globpeds[$seqnum+4];
	}
      }
      else{# dynodes (only TOF)
        $seqnum=6*($barsppl[0]+$barsppl[1]+$barsppl[2]+$barsppl[3]);# total anode-data
        for($il=0;$il<$layx-1;$il++){$seqnum+=$barsppl[$il]*18;}# count in layers befor layx
        for($ib=0;$ib<$barx-1;$ib++){$seqnum+=18;}# count in paddles befor barx
        for($is=0;$is<$sidx-1;$is++){$seqnum+=9;}# count in sides befor sidx
        for($ip=0;$ip<$pmtx-1;$ip++){$seqnum+=3;}# count in pmts befor pmtx
	$sta=$globpeds[$seqnum];
	$ped=$globpeds[$seqnum+1];
	$sig=$globpeds[$seqnum+2];
      }
#
      if($ped==0 && $sig==0){
        show_warn("   <-- Channel $locvar is empty for run $daqfrunn[$i] !");
      }
      $xc=($daqfrunn[$i]-$daqfrunmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
#
      if($sta==0){$curcol=$goodc;}
      else {$curcol=$badc;}
      $vped=int(-$ped*$canscry1/$ymaxv+0.5);#in pixels
      $vsig=int(-$sig*$canscry1/$ymaxv+0.5);
      $lid=$canvas->createLine($xc,-($vped-$vsig),$xc,-($vped+$vsig), -fill=>$curcol, -width=>4);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
      if($xc>$plot3_xcmax){$plot3_xcmax=$xc;}
    }
    $topl1->update;
  }#--->endof file-loop
}
#-------------
sub item_mark
{
  my ($canv,$id) = @_;
  my $fnam;
  my $color=$canv->itemcget($id, -fill);
  if($color eq "green"){
    print "set_bad branch\n";
    $canv->itemconfigure($id,-fill=>"red");
    $elem_num=-1;
    for($i=0;$i<$ndaqfound;$i++){# scan IDlist to find item#
      if($plot_fileids[$i]==$id){
        $elem_num=$i;
        last;
      }
    }
    if($elem_num>=0 && $elem_num<$ndaqfound){
      $fnam=$PedFName.$daqfrunn[$i];
      $daqfstat[$i] = 0;# bad
      $message="Select $fnam as bad ($elem_num-th in the FileList) !";
      $mwnd->bell;
    }
    else{
      $message="BindError: LineID=$id not found  !";
      $mwnd->bell;
    }
  }
#
  else{
    print "set_good branch\n";
    $canv->itemconfigure($id,-fill=>"green");
    $elem_num=-1;
    for($i=0;$i<$ndaqfound;$i++){# scan IDlist to find item#
      if($idlist[$i]==$id){
        $elem_num=$i;
        last;
      }
    }
    if($elem_num>=0 && $elem_num<$ndaqfound){
      $fnam=$PedFName.$daqfrunn[$i];
      $daqfstat[$i] = 1;# good
      $message="Restore $fnam as good ($elem_num-th in the FileList) !";
    }
    else{
      $message="BindError: LineID=$id not found  !";
      $mwnd->bell;
    }
  }
  $topl1->update;
}
#-------------
sub item_mark1
{
  my ($canv,$id) = @_;
  my ($findx,$fnam,$elem_num);
  $elem_num=-1;
  for($i=0;$i<$plot_pntmem;$i++){
    if($id == $plot_pntids[$i]){
      $elem_num=$i;
      last;
    }
  }
#
  if($elem_num>=0 && $elem_num<$plot_pntmem){
    $findx=$plot_fileids[$elem_num];
    $fnam=$PedFName.$daqfrunn[$findx];
    $fileidvar=$fnam;# show name
  }
  else{
    $message="Bind1Error: LineID=$id not found  !";
    $mwnd->bell;
  }
#
  $topl1->update;
}
#---------------------------------------------------------------------------
sub file_scan
{
#--->all-channels  ped/sig-info for one particular ped-file, this is plot-4
#
  show_warn("");
  $last_pedscanned_file_num=0;
  if($fileidvar ne ""){
    for($i=0;$i<$ndaqfound;$i++){#<--- selected files loop
      if($fileidvar eq ($PedFName.$daqfrunn[$i])){
        $last_pedscanned_file_num=1+$i;
	last;
      }
    }
    if($last_pedscanned_file_num==0){
      $message="FileScan Error: file name not in the list!";
      $mwnd->bell;
      return
    }
  }
  else{
    $message="FileScan Error: missing file name !";
    $mwnd->bell;
    return;
  }
#
  my($laymx,$xc,$barmx);
  if($tofantsel eq "useTOF"){$laymx=4;}
  if($tofantsel eq "useANT"){$laymx=1;}
  my($bot,$top,$xc1,$xc2);
#
  $sfname=$workdir.$amsjwd."/".$fileidvar;
  @globpeds=();
  my $pntwid=2;#point width in pixels
  my $barspac=2;#pixels spacing between 2 neigbour paddles
  my $sidspac=4;#pixels spacing between 2 neigbour sides
  my $layspac=8;#pixels spacing between 2 neigbour layers
  my $dynspac=16;#pixels spacing between Anode and Dynode plots
#
  my ($ycoarsegrd,$yfinegrd)=(100,20);#(adc-ch)
  my $ymaxv=500;#(adc-ch)
  my $ypbin=int(-$canscry1/$ymaxv+0.5);#(pixels/bin)
  my $ycoarsegrdp=int(-$canscry1*$ycoarsegrd/$ymaxv+0.5);#coarse-grid in pix
  my $yfinegrdp=int(-$canscry1*$yfinegrd/$ymaxv+0.5);#fine-grid in pix
#
  my $layshift=2*10*($pntwid+$barspac)+$sidspac+$layspac;
  my $dynshift=4*$layshift+$dynspac;
  my $sidshift=10*($pntwid+$barspac)+$sidspac;
  my $dynvsh=50;#pix. vert.shift to plot dynodes for the same side/paddle
  my ($width,$ltxt,$stxt,$lmax,$dapl,$vstp);
  my ($text,$dig);
#
#---> create scales/greeds:
#
  if($plot4_actf==0){# 1st entry for plot4
    &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
    $dapl=2;#need dynode/anode plot
    $lmax=$laymx;
    if($laymx==1){$dapl=1;}
#
    for($dyn=0;$dyn<$dapl;$dyn++){
    for($il=0;$il<$lmax;$il++){#layer-loop
      $ltxt="L".($il+1);
      for($is=0;$is<2;$is++){#side-loop
        $stxt="S".($is+1);
        $xc1=$il*$layshift+$is*$sidshift+$dyn*$dynshift;
	$xc2=$xc1+($barspac+$pntwid)*9;
	$width=1;
#	if($is==0){$width=2;}
        $lid=$canvas->createLine($xc1,0,$xc1,$canscry1, -fill=>'black', -width=>$width,-dash=>'-');
        $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
        $plot_accmem+=1;
	$width=1;
#	if($is==1){$width=2;}
        $lid=$canvas->createLine($xc2,0,$xc2,$canscry1, -fill=>'black', -width=>$width,-dash=>'-');
        $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
        $plot_accmem+=1;
	$xc=$xc1+5*($barspac+$pntwid);
        $lid=$canvas->createText($xc,-8,-text=>$stxt,-font=>$font3,-fill=>'blue');  
        $plot_accids[$plot_accmem]=$lid;#store in glob.list
        $plot_accmem+=1;
        if($xc2>$plot4_xcmax){$plot4_xcmax=$xc2;}
      }
      $xc=$xc1-0.5*$sidspac;
      $lid=$canvas->createText($xc,-16,-text=>$ltxt,-font=>$font2,-fill=>'red');  
      $plot_accids[$plot_accmem]=$lid;#store in glob.list
      $plot_accmem+=1;
    }#--->endof Layer-loop
#
    if($dyn==0){$stxt="Anode-channels";}
    else {$stxt="Dynode-channels";}
    $xc=2*$layshift+$dyn*$dynshift;
    $lid=$canvas->createText($xc,9,-text=>$stxt,-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;#store in glob.list
    $plot_accmem+=1;
    }#--->endof An/Dyn loop
#
#-- create Grid horizontal lines + Y-axes subscripts :
    for($i=0;$i<-$canscry1;$i++){
       if($i>0 && (($i % $yfinegrdp)==0 || ($i % $ycoarsegrdp)==0)){
         $yc=$i;
	 if(($i % $ycoarsegrdp)==0){# coarse scale line
	   $width=1;
	   $dig=$i*$ycoarsegrd/$ycoarsegrdp;
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	 }
	 else{# fine scale line
	   $width=1;
	   $dig=$i*$yfinegrd/$yfinegrdp;
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	 }
         $lid=$canvas->createLine(0,-$yc,$canscrx2,-$yc, -fill=>'black', -width=>$width,-dash=>$dasht);
         $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
         $plot_accmem+=1;
         $lid=$canvas->createText(-20,-$yc,-text=>$ltxt,-font=>$font2,-fill=>$colr);  
         $plot_accids[$plot_accmem]=$lid;#store in glob.list
         $plot_accmem+=1;
       }
    }
    $plot1_actf=0;
    $plot2_actf=0;
    $plot3_actf=0;
  }
  $plot4_actf+=1;
  $text="Channels Ped+-5*Rms";
  $lid=$canvas->createText(120,$canscry1+15,-text=>$text,-font=>$font2,-fill=>'blue');  
  $plot_accids[$plot_accmem]=$lid;#store in glob.list
  $plot_accmem+=1;
  $text="for file $fileidvar";
  $lid=$canvas->createText(130,$canscry1+30,-text=>$text,-font=>$font2,-fill=>'blue');  
  $plot_accids[$plot_accmem]=$lid;#store in glob.list
  $plot_accmem+=1;
#---------
    open(INPFN,"< $sfname") or die show_warn("   <-- Cannot open $sfname for reading $!");
    while(defined ($line = <INPFN>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      if($line =~/==========/){last;}# break on my internal EOF
      if($line =~/12345/){last;}# break on my internal EOF
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      @words=split(/\s+/,$line);#split by any number of continious " "'s
      $nwords=scalar(@words);
      push(@globpeds,@words);
    }
    close(INPFN) or show_warn("   <-- Cannot open $sfname after reading $!");
#
    $seqnum=0;
#--->first anodes:
    $xc=0;
    for($il=0;$il<$laymx;$il++){
      if($tofantsel eq "useTOF"){$barmx=$barsppl[$il];}
      else {$barmx=8;}
      for($ib=0;$ib<$barmx;$ib++){
        for($is=0;$is<2;$is++){
	  if($tofantsel eq "useTOF"){
	    $sta=$globpeds[$seqnum+3*$is];
	    $ped=$globpeds[$seqnum+3*$is+1];
	    $sig=$globpeds[$seqnum+3*$is+2];
	  }
	  else{
	    $sta=$globpeds[$seqnum+$is];
	    $ped=$globpeds[$seqnum+$is+2];
	    $sig=$globpeds[$seqnum+$is+4];
	  }
          $xc=$il*$layshift+$is*$sidshift+($barspac+$pntwid)*$ib+$pntwid/2;
#
	  if(sta==0){$curcol="green";}
	  else {$curcol="red";}
          $vped=int(-$ped*$canscry1/$ymaxv+0.5);#in pixels
          $vsig=int(-$sig*$canscry1/$ymaxv+0.5);
          $lid=$canvas->createLine($xc,-($vped-5*$vsig),$xc,-($vped+5*$vsig), -fill=>$curcol, -width=>$pntwid);
          $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
          $plot_pntmem+=1;
          $lid=$canvas->createOval($xc-2,-($vped-2),$xc+2,-($vped+2), -fill=>$curcol, -width=>1);
          $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
          $plot_pntmem+=1;
          if($xc>$plot4_xcmax){$plot4_xcmax=$xc;}
	}#-->endof side-loop
	$seqnum+=6;
#
      }#-->endof bar-loop
#
    }#-->endof lay-loop
#--->next - dynodes:
    if($tofantsel eq "useTOF"){#only for TOF
    for($il=0;$il<$laymx;$il++){
      $barmx=$barsppl[$il];
      for($ib=0;$ib<$barmx;$ib++){
        for($is=0;$is<2;$is++){
          for($pm=0;$pm<3;$pm++){
	    $sta=$globpeds[$seqnum];
	    $ped=$globpeds[$seqnum+1];
	    $sig=$globpeds[$seqnum+2];
            $xc=$dynshift+$il*$layshift+$is*$sidshift+($barspac+$pntwid)*$ib+$pntwid/2;
#
            if($ped!=0 && $sig!=0){#dynode exists
	      if(sta==0){$curcol="green";}
	      else {$curcol="red";}
              $vped=int(-$ped*$canscry1/$ymaxv+0.5);#in pixels
              $vsig=int(-$sig*$canscry1/$ymaxv+0.5);
              $lid=$canvas->createLine($xc,-($vped-5*$vsig),$xc,-($vped+5*$vsig), -fill=>$curcol, -width=>$pntwid);
              $plot_pntids[$plot_pntmem]=$lid;#store points in glob.list
              $plot_pntmem+=1;
	      
	      if($pm==0){$dig="1";}
	      elsif($pm==1){$dig="2";}
	      else {$dig="3";}
              $lid=$canvas->createText($xc,-$vped,-text=>$dig,-font=>$font8,-fill=>'black');  
#              $lid=$canvas->createOval($xc-3,-($vped-3),$xc+3,-($vped+3), -fill=>$curcol, -width=>1);
              $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
              $plot_pntmem+=1;
              if($xc>$plot4_xcmax){$plot4_xcmax=$xc;}
	    }
	    $seqnum+=3;
	  }#-->endof pm-loop
	}#-->endof side-loop
#
      }#-->endof bar-loop
#
    }#-->endof lay-loop
    }
#
}
#------------------------------------------------------------------
sub sig_tscan
{
#---> time-evolution of file's particular channel ped's Rms, this is plot-2
#
  show_warn("");
  my $layx,$barx,$sidx,$pmtx,$rest;
  my $maxb;
  my $locvar;
  my @goodcolorlist=qw(green darkred blue orange black );
  $locvar=$chidvar;
#--> check ID :
  if($tofantsel eq "useTOF"){
    if($locvar =~/(LBBSP)/){substr($locvar,-7,7)="";}# remove "(LBBSP)" if present
    ($layx,$barx,$sidx,$pmtx,$rest)=unpack("A1 A2 A1 A1 A*",$locvar);
    if($layx>4 || $layx==0 || $barx==0 || $barx>$barsppl[$layx-1] || $sidx==0 || $sidx>2 || $pmtx>3){
      show_warn("   <-- wrong TOF channel-ID setting: $locvar ?!");
      return;
    }
  }
  if($tofantsel eq "useANT"){
    if($locvar =~/(BSP)/){substr($locvar,-5,5)="";}# remove "(BSP)" if present
    ($barx,$sidx,$pmtx,$rest)=unpack("A1 A1 A1 A*",$locvar);
    $layx=0;
    if($barx==0 || $barx>8 || $sidx==0 || $sidx>2 || $pmtx>0){
      show_warn("   <-- wrong ACC channel-ID setting: $locvar ?!");
      return;
    }
  }
  $curline="   <-- Single channel PedRms Time-scan for id=$locvar, l/bb/s/p=$layx/$barx/$sidx/$pmtx \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
#
  my ($ped,$sig,$sta,$vped,$vsig);
  my $seqnum;
  my ($xc,$yc)=(0,0);
  my ($xcoarsegrd,$xfinegrd)=(86400,3600);# day/hour in sec
  my ($ycoarsegrd,$yfinegrd)=(10,2);#(adc-ch)
  my $ymaxv=20;#(adc-ch)
  my $ypbin=int(-$canscry1/$ymaxv+0.5);#(pixels/bin)
  my $ycoarsegrdp=int(-$canscry1*$ycoarsegrd/$ymaxv+0.5);#coarse-grid in pix
  my $yfinegrdp=int(-$canscry1*$yfinegrd/$ymaxv+0.5);#fine-grid in pix
  my $goodc="darkgreen";
  my $badc="red";
  my $nzlines=0;
  my $curcol;
  my $colindx;
  my ($width,$dig,$ltxt,$colr,$dasht,$titl);
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time);
  my $vcoo;
#
  @globpeds=();
#
#---> create scale/grid:
#
  if($plot2_actf==0){# 1st entry for plot2
    &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
#-- create Grid vertical lines + X-axes subscripts :
    for($i=0;$i<$canscrx2;$i++){
       if($i>0 && (($i*$binw_tev % $xfinegrd)==0 || ($i*$binw_tev % $xcoarsegrd)==0)){
         $xc=$i;
	 if(($i*$binw_tev % $xcoarsegrd)>0){# hour-line
	   $width=1;
	   $dig=(($i*$binw_tev % $xcoarsegrd)/$xfinegrd);
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	 }
	 else{# day-line
	   $width=1;
	   $dig=($i*$binw_tev/$xcoarsegrd);
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	 }
         $lid=$canvas->createLine($xc,0,$xc,$canscry1, -fill=>'black', -width=>$width,-dash=>$dasht);
         $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
         $plot_accmem+=1;
         $lid=$canvas->createText($xc,+10,-text=>$ltxt,-font=>$font2,-fill=>$colr);  
         $plot_accids[$plot_accmem]=$lid;#store in glob.list
         $plot_accmem+=1;
       }
    }
#-- create Grid horizontal lines + Y-axes subscripts :
    for($i=0;$i<-$canscry1;$i++){
       if($i>0 && (($i % $yfinegrdp)==0 || ($i % $ycoarsegrdp)==0)){
         $yc=$i;
	 if(($i % $ycoarsegrdp)==0){# coarse scale line
	   $width=1;
	   $dig=$i*$ycoarsegrd/$ycoarsegrdp;
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	 }
	 else{# fine scale line
	   $width=1;
	   $dig=$i*$yfinegrd/$yfinegrdp;
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	 }
         $lid=$canvas->createLine(0,-$yc,$canscrx2,-$yc, -fill=>'black', -width=>$width,-dash=>$dasht);
         $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
         $plot_accmem+=1;
         $lid=$canvas->createText(-20,-$yc,-text=>$ltxt,-font=>$font2,-fill=>$colr);  
         $plot_accids[$plot_accmem]=$lid;#store in glob.list
         $plot_accmem+=1;
       }
    }
#-- add 0 :
    $lid=$canvas->createText(-10,10,-text=>"0",-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#-- add title :
    $ptime=localtime($daqfrunmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time=$year."/".$month."/".$day."  ".$hour.":".$min;
    $titl="PedRms Time-evolution,  '0'-time is $time";
    $vcoo=int(11*$canscry1/$ymaxv);#place at val=11 adc-ch
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl="Time-scale: red digits ->days, blue ->hours";
    $vcoo=int(0.5*$canscry1/$ymaxv);#place at val=0.5 adc-ch
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#
    $plot1_actf=0;
    $plot3_actf=0;
    $plot4_actf=0;
  }
  $colindx=($plot2_actf % 5);
  $goodc=$goodcolorlist[$colindx];#change color according fill-entry number
  $plot2_actf+=1;
#
  for($i=0;$i<$ndaqfound;$i++){#<--- selected files loop
    $sfname=$workdir.$amsjwd."/".$PedFName.$daqfrunn[$i];
    $nzlines=0;
    $nwords=0;
    @globpeds=();#store all data of the file
    if($daqfstat[$i] == 111){
      open(INPFN,"< $sfname") or show_warn_setfstat("   <-- Cannot open $sfname for reading $!",$i);
      if($daqfstat[$i] == 211){
        show_warn("   <-- Missing file $sfname !");
	next;
      }
      while(defined ($line = <INPFN>)){
        chomp $line;
        if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
        if($line =~/==========/){last;}# break on my internal EOF
        if($line =~/12345/){last;}# break on my internal EOF
	$nzlines+=1;
	$line =~s/^\s+//s;# remove " "'s at the beg.of line
	@words=split(/\s+/,$line);#split by any number of continious " "'s
	$nwords=scalar(@words);
	push(@globpeds,@words);
      }
      close(INPFN) or die show_warn("   <-- Cannot open $sfname after reading $!");
#
      $seqnum=0;
      if($pmtx==0){# anode (TOF/ANTI)
        if($layx>0){# tof
          for($il=0;$il<$layx-1;$il++){$seqnum+=$barsppl[$il]*6;}# count in layers befor layx
          for($ib=0;$ib<$barx-1;$ib++){$seqnum+=6;}# count in paddles befor barx
          for($is=0;$is<$sidx-1;$is++){$seqnum+=3;}# count in sides befor sidx
	  $sta=$globpeds[$seqnum];
	  $ped=$globpeds[$seqnum+1];
	  $sig=$globpeds[$seqnum+2];
	}
	else{# anti
          for($ib=0;$ib<$barx-1;$ib++){$seqnum+=6;}# count in paddles befor barx
          for($is=0;$is<$sidx-1;$is++){$seqnum+=1;}# count in sides befor sidx
	  $sta=$globpeds[$seqnum];
	  $ped=$globpeds[$seqnum+2];
	  $sig=$globpeds[$seqnum+4];
	}
      }
      else{# dynodes (only TOF)
        $seqnum=6*($barsppl[0]+$barsppl[1]+$barsppl[2]+$barsppl[3]);# total anode-data
        for($il=0;$il<$layx-1;$il++){$seqnum+=$barsppl[$il]*18;}# count in layers befor layx
        for($ib=0;$ib<$barx-1;$ib++){$seqnum+=18;}# count in paddles befor barx
        for($is=0;$is<$sidx-1;$is++){$seqnum+=9;}# count in sides befor sidx
        for($ip=0;$ip<$pmtx-1;$ip++){$seqnum+=3;}# count in pmts befor pmtx
	$sta=$globpeds[$seqnum];
	$ped=$globpeds[$seqnum+1];
	$sig=$globpeds[$seqnum+2];
      }
#
      if($ped==0 && $sig==0){
        show_warn("   <-- Channel $locvar is empty for run $daqfrunn[$i] !");
      }
      $xc=($daqfrunn[$i]-$daqfrunmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
#
      if($sta==0){$curcol=$goodc;}
      else {$curcol=$badc;}
      $vsig=int(-$sig*$canscry1/$ymaxv+0.5);
      $lid=$canvas->createOval($xc-3,-($vsig-3),$xc+3,-($vsig+3), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
      if($xc>$plot2_xcmax){$plot2_xcmax=$xc;}
    }#--->endof file-loop
    $topl1->update;
  }
}
#----------------------------------------------------------------------------
sub mark_file
{
#
#--- Mark displayed(after file-selection in Time_scan window) file as bad/good:
#
  if($fileidvar ne ""){
    my $flag=0;
    for($i=0;$i<$ndaqfound;$i++){#<--- selected files loop
      if($fileidvar eq ($PedFName.$daqfrunn[$i])){
        if($daqfstat[$i]==111){
          $file_remove_btext="^^^ Mark above file as good ^^^";
	  $fentrw->configure(-foreground=>"red");
	  $daqfstat[$i]=211;	
          $curline="    <--File $fileidvar marked as bad !\n\n";
          $logtext->insert('end',$curline);
          $logtext->yview('end');
	  $flag=1;
	  last;
	}
	else{
          $file_remove_btext="^^^ Mark above file as bad ^^^";
	  $fentrw->configure(-foreground=>"blue");
	  $daqfstat[$i]=111;	
          $curline="    <--File $fileidvar marked as good !\n\n";
          $logtext->insert('end',$curline);
          $logtext->yview('end');
	  $flag=2;
	  last;
	}
      }
#      $topl1->update;
    }
    if($flag==0){
      $message="Mark_file Error: file name not in the list !";
      $mwnd->bell;
    }
  }
  else{
    $message="Mark_file Error: missing file name !";
    $mwnd->bell;
    return;
  }
}
#------------------------------------------------------------------------------
sub quitsess
{
#
#---> Move good ped-files(+ their Logs) to storage $pedsdir:
#
  my ($curfn,$fnfr,$fnto);
  my $status=0;
  if($nprocruns > 0){
    show_messg("\n   <-- Moving good ped/log-files to $pedsdir storage...");
    for($i=0;$i<$ndaqfound;$i++){#<--- processed daq-files loop
      if($daqfstat[$i] == 111){
        $curfn=$PedFName.$daqfrunn[$i];
	$fnfr=$workdir.$amsjwd."/".$curfn;
	$fnto=$workdir.$pedsdir."/".$curfn;
        move($fnfr,$fnto) or show_warn("   <-- PedsMove to $pedsdir failed for $curfn, $!");# move files to peddir
        $curfn=$PedFName.$daqfrunn[$i].".log";
	$fnfr=$workdir.$amsjwd."/".$curfn;
	$fnto=$workdir.$pedsdir."/".$curfn;
        move($fnfr,$fnto) or show_warn("   <-- LogsMove to $pedsdir failed for $curfn, $!");# move files to peddir
      }
      elsif($daqfstat[$i] == 211){#remove bad ped/log-files
        $curfn=$PedFName.$daqfrunn[$i];
	$fnfr=$workdir.$amsjwd."/".$curfn;
        system("rm -f $fnfr");
        $curfn=$PedFName.$daqfrunn[$i].".log";
	$fnfr=$workdir.$amsjwd."/".$curfn;
        system("rm -f $fnfr");
      }
    }
    show_messg("   <-- Peds/Logs-files moving to $pedsdir is completed !");
#
#---> Remove_Old/Create_new PrevRuns-file :
#
    $curfn=$workdir."/".$PrevRunsFN;
    $stat = system("rm -f $curfn");
    if($stat != 0){show_warn("   <-- No $PrevRunsFN-files to remove in pedcalib-dir !");}
    show_messg("   <-- Creating new $PrevRunsFN file... !");
    open(OUTFN,"> $PrevRunsFN") or die show_warn("   <-- Cannot open new $PrevRunsFN for writing $!");
    for($i=0;$i<$ndaqfound;$i++){#<--- all "tried to process" daq-files loop
      print OUTFN $daqfrunn[$i],"\n"; 
    }
    close(OUTFN) or die show_warn("   <-- Cannot close $PrevRunsFN after writing $!");
    $status=system("chmod 666 $curfn");
    if($status != 0){
      print "Warning: problem with write-priv for $PrevRunsFN, status=",$status,"\n";
#      exit;
    }
    show_messg("   <-- new $PrevRunsFN file created !");
  }#--->endof nprocruns check
#
#---> Create/Move "screen" SessLog-file:
#
  my $firstr=0;
  my $logfname="PedCalcLogFile";
  my $detname;
  if($tofantsel eq "useTOF"){$detname="Tof";}
  else{$detname="Acc";}
  my $filen;
  for($i=0;$i<$ndaqfound;$i++){#<--- found daq-files loop 
    if($daqfstat[$i] == 111){
      $firstr=$daqfrunn[$i];
      last;
    }
  }
  if($firstr>0){
    $curfn=$detname.$logfname.".".$firstr;#extention is 1st good run#
    $fnfr=$workdir.$pedsdir."/".$curfn; 
    $fnto=$workdir.$pedsdir.$archdir."/".$curfn;
    open(OFN, "> $fnfr") or die show_warn("   <-- Cannot open session-log $curfn for writing !");
    print OFN $logtext->get("1.0","end");
    close(OFN) or die show_warn("   <-- Cannot close session-log $curfn after writing $!");
    show_messg("\n   <-- SessLogFile $curfn is saved !");
    $status=system("chmod 666 $fnfr");
    if($status != 0){
      print "Warning: problem with write-priv for SessLogFile, status=",$status,"\n";
      exit;
    }
    move($fnfr,$fnto) or show_warn("   <-- Moving failed for SessLogFile $curfn, $!");# move to pedfiles-dir
    show_messg("   <-- SessLogFile $curfn is moved to archive, Bye-Bye !");
  }
  else{
    show_warn("   <-- Leaving with no good ped-files created, Bye-Bye !");
  }
#
  $mwnd->update;
  if($soundtext eq "Sound-ON"){$mwnd->bell;}
  $mwnd->after(8000);
  exit;
}
#--------
1;




