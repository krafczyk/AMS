sub TAUL2DBWriter
{
#---
my $dirfrheight=0.35;
my $setfrheight=0.52;
my $prgfrheight=0.08;
my $jclfrheight=0.05;
#---> imp.glob.const, vars and arrays:
$outlogf="out.log";# temp.file
$binw_tev=120;#binwidth(sec) for time-evolution graphs(2 min)
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
my $drh1=0.15;#height of dir-widgets
#---
$dir_fram=$mwnd->Frame(-label => "General Info/Settings :",-background => "Grey",
                                                      -relief=>'groove', -borderwidth=>5)->place(
                                                      -relwidth=>(1-$LogfXsize), -relheight=>$dirfrheight,
                                                      -relx=>0, -rely=>0);
#---
$amsd_lab=$dir_fram->Label(-text=>"HeadD:",-font=>$font2,-relief=>'groove')->place(
                                                             -relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>$shf1);
$amsd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$workdir)->place(
                                                                   -relwidth=>0.8, -relheight=>$drh1,  
                                                                   -relx=>0.2, -rely=>$shf1);
#---------
$scd_lab=$dir_fram->Label(-text=>"CalfD:",-font=>$font2,-relief=>'groove')
                                                ->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+$drh1));
$scnd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$font1,
                                              -textvariable=>\$CalfDir)->place(
                                              -relwidth=>0.3, -relheight=>$drh1,  
                                              -relx=>0.2, -rely=>($shf1+$drh1));
#---
$arcd_lab=$dir_fram->Label(-text=>"AmsJD:",-font=>$font2,-relief=>'groove')
                                                    ->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0.5, -rely=>($shf1+$drh1));
$arcd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$amsjwd)->place(
                                                                   -relwidth=>0.3, -relheight=>$drh1,  
                                                                   -relx=>0.7, -rely=>($shf1+$drh1));
#---------
$amsg_lab=$dir_fram->Label(-text=>"AmsDD:",-font=>$font2,-relief=>'groove')->place(
                                                             -relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+2*$drh1));
$amsg_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$AMSDD)->place(
                                                                   -relwidth=>0.8, -relheight=>$drh1,  
                                                                   -relx=>0.2, -rely=>($shf1+2*$drh1));
#---------
$dbdir_lab=$dir_fram->Label(-text=>"DBdir:",-font=>$font2,-relief=>'groove')->place(
                                                             -relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+3*$drh1));
$DBDir="/DataBase";
$dbdir_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$DBDir)->place(
                                                                   -relwidth=>0.3, -relheight=>$drh1,  
                                                                   -relx=>0.2, -rely=>($shf1+3*$drh1));
#---
$dbrf_lab=$dir_fram->Label(-text=>"DBrawF:",-font=>$font2,-relief=>'groove')->place(
                                                             -relwidth=>0.25, -relheight=>$drh1,
                                                             -relx=>0.5, -rely=>($shf1+3*$drh1));
$DBrawf="/v4.00";
$dbrf_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$DBrawf)->place(
                                                                   -relwidth=>0.25, -relheight=>$drh1,  
                                                                   -relx=>0.75, -rely=>($shf1+3*$drh1));
#-------------
$tdvn_lab=$dir_fram->Label(-text=>"CalSetName:",-font=>$font2,-relief=>'groove')->place(
                                                             -relwidth=>0.3, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+4*$drh1));
#
$tdvn_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$TDVName)->place(
                                                                   -relwidth=>0.25, -relheight=>$drh1,  
                                                                   -relx=>0.3, -rely=>($shf1+4*$drh1));
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
						 -relx=>0.55, -rely=>($shf1+4*$drh1));
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
                                         -relwidth=>0.2, -relheight=>$drh1,  
                                         -relx=>0.8, -rely=>($shf1+4*$drh1));
#----------
$dir_fram->Label(-text=>"UserN:",-font=>$font2)->place(-relwidth=>0.18, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+5*$drh1));
#---
$tofusern="";# user name 
$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
#								   -show=>"*",
                                                                   -textvariable=>\$TofUserN)->place(
                                                                   -relwidth=>0.27, -relheight=>$drh1,  
                                                                   -relx=>0.18, -rely=>($shf1+5*$drh1));
#---
$dir_fram->Label(-text=>"PassW:",-font=>$font2)->place(-relwidth=>0.18, -relheight=>$drh1,
                                                             -relx=>0.45, -rely=>($shf1+5*$drh1));
#---
$passwd="";# pw 
$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
								   -show=>"*",
                                                                   -textvariable=>\$TofPassW)->place(
                                                                   -relwidth=>0.27, -relheight=>$drh1,  
                                                                   -relx=>0.63, -rely=>($shf1+5*$drh1));
#---
$dir_fram->Button(-text=>"OK", -font=>$font2, 
                                 -activebackground=>"yellow",
			         -activeforeground=>"red",
#			         -foreground=>"red",
			         -background=>"green",
                                 -borderwidth=>3,-relief=>'raised',
			         -cursor=>hand2,
                                 -command => \&pw_check)
			         ->place(
                                         -relwidth=>0.1, -relheight=>$drh1,  
                                         -relx=>0.9, -rely=>($shf1+5*$drh1));
#-------------
#frame_set:
my $shf2=0.07;#down shift for runcond-widgets
my $drh2=0.116;#height of runcond-widgets
#---
$set_fram=$mwnd->Frame(-label=>"Job Settings :", 
                                                      -relief=>'groove', -borderwidth=>5)->place(
                                                      -relwidth=>(1-$LogfXsize), -relheight=>$setfrheight,
                                                      -relx=>0, -rely=>$dirfrheight);
#-------
$calt_lab1=$set_fram->Label(-text=>"DB-update Request for CalType:",
                                                    -font=>$font2,-relief=>'groove',-foreground=>"red")
                                                    ->place(
						    -relwidth=>0.8, -relheight=>$drh2,
                                                    -relx=>0, -rely=>$shf2);
#---
my $cname;
if($CalType eq "TAU2DB"){$cname="TAUnif";}
if($CalType eq "TdcL2DB"){$cname="TDCLin";}
$calt_lab1=$set_fram->Label(-text=>$cname,-foreground=>"red",-background=>"yellow",
                                                    -font=>$font2c,-relief=>'sunken')
                                                    ->place(
						    -relwidth=>0.2, -relheight=>$drh2,
                                                    -relx=>0.8, -rely=>$shf2);
#-------
$set_fram->Label(-text=>"Push if limit changed >>>",-font=>$font2,-relief=>'groove')
                                               ->place(-relwidth=>0.5, -relheight=>$drh2,
                                                             -relx=>0, -rely=>($shf2+$drh2));
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
						 -relx=>0.5, -rely=>($shf2+$drh2));
$convert_bt->bind("<Button-3>", \&convert_help);
#------
#
$dat1_lab=$set_fram->Label(-text=>"Date",-font=>$font2)->place(-relwidth=>0.12, -relheight=>$drh2,
                                                             -relx=>0, -rely=>($shf2+2*$drh2));
$fdat1="2007.12.17 00:00:01";#def. file-date-from 
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
$fnum1=time2run($fdat1);#def. file-number-from
$fnum2=time2run($fdat2);#def. file-num-till
$num1_lab=$set_fram->Label(-text=>"RunN",-font=>$font2)->place(-relwidth=>0.16, -relheight=>$drh2,
                                                             -relx=>0, -rely=>($shf2+3*$drh2));
$fnum1_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font3,
                                                                   -textvariable=>\$fnum1)->place(
                                                                   -relwidth=>0.4, -relheight=>$drh2,  
                                                                   -relx=>0.16, -rely=>($shf2+3*$drh2));
#
$num2_lab=$set_fram->Label(-text=>"-",-font=>$font2)->place(-relwidth=>0.04, -relheight=>$drh2,
                                                             -relx=>0.56, -rely=>($shf2+3*$drh2));
$fnum2_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font3,
                                                                   -textvariable=>\$fnum2)->place(
                                                                   -relwidth=>0.4, -relheight=>$drh2,  
                                                                   -relx=>0.6, -rely=>($shf2+3*$drh2));
$fnum1o=$fnum1;
$fnum2o=$fnum2;
$fdat1o=$fdat1;
$fdat2o=$fdat2;
#------
$dbuse_lab=$set_fram->Label(-text=>"CalibInfo(LQDPU) storage(0/1->DB/LocFile):",
                                                    -font=>$font2,-relief=>'groove')
                                                    ->place(
						    -relwidth=>0.85, -relheight=>$drh2,
                                                    -relx=>0, -rely=>($shf2+4*$drh2));
$cfilesloc="10101";
$cfloc_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",
                                               -background=>yellow,
                                               -font=>$font3,
                                               -textvariable=>\$cfilesloc)
					       ->place(
                                               -relwidth=>0.15, -relheight=>$drh2,  
                                               -relx=>0.85, -rely=>($shf2+4*$drh2));
#---------
$set_fram->Button(-text=>"EditJobScript", -font=>$font2, 
                                          -activebackground=>"yellow",
			                  -activeforeground=>"red",
			                  -foreground=>"red",
			                  -background=>"green",
                                          -borderwidth=>5,-relief=>'raised',
			                  -cursor=>hand2,
                                          -command => \&EditJobScript)
			                  ->place(
                                          -relwidth=>0.333, -relheight=>$drh2,  
                                          -relx=>0, -rely=>($shf2+5*$drh2));
#---
$arcscval=0;#1/0->scan Archive/Normal_store
$arcscbt=$set_fram->Checkbutton(-text=>"SearchInArchive", -font=>$font2, -indicator=>0,
                                                 -borderwidth=>5,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
                                                 -variable=>\$arcscval)
					 ->place(
                                                 -relwidth=>0.333, -relheight=>$drh2,
						 -relx=>0.333, -rely=>($shf2+5*$drh2));
$arcscbt->bind("<Button-3>", \&arcscbt_help);
#---
$archval=0;#0/1->not/archive processed cal-files
$archbt=$set_fram->Checkbutton(-text=>"ArchiveOnExit",  -font=>$font2, -indicator=>0, 
                                          -borderwidth=>5,-relief=>'raised',
                                          -selectcolor=>orange,-activeforeground=>red,
				          -activebackground=>yellow, 
			                  -cursor=>hand2,
                                          -background=>green,
                                          -variable=>\$archval)
			          ->place(
				          -relwidth=>0.333,-relheight=>$drh2,      
                                          -relx=>0.666,-rely=>($shf2+5*$drh2));
$archbt->bind("<Button-3>", \&archbt_help);
#--------
$dbwrsel="InsDB";
$dbre_rbt=$set_fram->Radiobutton(-text=>"DBrewriteMode",-font=>$font2, -indicator=>0,
                                                      -borderwidth=>5,-relief=>'raised',
						      -selectcolor=>orange,-activeforeground=>red,
						      -activebackground=>yellow, 
			                              -cursor=>hand2,
                                                      -background=>green,
                                                      -disabledforeground=>blue,
                                                      -value=>"RewrDB", -variable=>\$dbwrsel)
                                              ->place(
						      -relwidth=>0.5, -relheight=>$drh2,
						      -relx=>0.5, -rely=>($shf2+6*$drh2));
#---
$dbin_rbt=$set_fram->Radiobutton(-text=>"DBinsertMode",-font=>$font2, -indicator=>0, 
                                                       -borderwidth=>5,-relief=>'raised',
						       -selectcolor=>orange,-activeforeground=>red, 
						       -activebackground=>yellow, 
			                               -cursor=>hand2,
                                                       -background=>green, 
                                                       -disabledforeground=>blue,
                                                       -value=>"InsDB", -variable=>\$dbwrsel)
                                               ->place(
						       -relwidth=>0.5, -relheight=>$drh2,
						        -relx=>0, -rely=>($shf2+6*$drh2));
#---------
$set_fram->Label(-text=>"RefCalibSetN:",-font=>$font2,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.3, -relheight=>$drh2,
                                               -relx=>0, -rely=>($shf2+7*$drh2));
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
                               -relx=>0.3, -rely=>($shf2+7*$drh2));
#---
$dryrunval=1;#0/1->real/dry DBupdate run
$dryruntext="DB ReadOnly"; 
$dryrbt=$set_fram->Checkbutton(-text=>$dryruntext,  -font=>$font2, -indicator=>0, 
                                          -borderwidth=>5,-relief=>'raised',
                                          -selectcolor=>orange,-activeforeground=>red,
				          -activebackground=>yellow, 
			                  -disabledforeground=>"blue",
			                  -state=>'disabled',
			                  -cursor=>hand2,
                                          -background=>green,
                                          -variable=>\$dryrunval)
			                  ->place(
				          -relwidth=>0.4,-relheight=>$drh2,      
                                          -relx=>0.6,-rely=>($shf2+7*$drh2));
$dryrbt->bind("<Button-3>", \&dryrbt_help);
#---
# $bellb=$set_fram->Button(-text => 'bell', -command =>[\&mybeep, 3, 3])
#			          ->place(
#				          -relwidth=>0.5,-relheight=>$drh2,      
#                                          -relx=>0.5,-rely=>($shf2+6*$drh2));
#--------------
#frame_progr:
$prg_fram=$mwnd->Frame(-label=>"DB-update progress :",-background => "red", -relief=>'groove',
                                                      -borderwidth=>5)->place(
                                                      -relwidth=>(1-$LogfXsize), -relheight=>$prgfrheight,
                                                      -relx=>0, -rely=>($dirfrheight+$setfrheight));
$perc_done=0.;
$prg_but=$prg_fram->ProgressBar( -width=>10, -from=>0, -to=>100, -blocks=>100,
                                 -colors=>[0,'green'], -gap=> 0,
                                -variable=>\$perc_done)->place(-relwidth=>1, -relheight=>0.5,
                                                             -relx=>0, -rely=>0.45);
#--------------
#job_control frame:
$jcl_fram=$mwnd->Frame(-label=>"CalPars2DB-Job control :",-background => "red",
                                                       -relief=>'groove', -borderwidth=>3)
						       ->place(
                                                       -relwidth=>(1-$LogfXsize), -relheight=>$jclfrheight,
                                                       -relx=>0, -rely=>($dirfrheight+$setfrheight+$prgfrheight));
#---
$scanbt=$jcl_fram->Button(-text => "Search", -font=>$font2, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
			      -background=>"green",
			      -cursor=>hand2,
                              -command => \&scand)->place(
                                         -relx=>0,-rely=>0,
                                         -relwidth=>0.25,-relheight=>1);
$scanbt->bind("<Button-3>", \&scanbt_help);
#---
$checkbt=$jcl_fram->Button(-text => "Check", -font=>$font2, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
			      -background=>"green",
			      -disabledforeground=>"blue",
			      -state=>'disabled',
			      -cursor=>hand2,
                              -command => \&checkq)->place(
                                         -relx=>0.25,-rely=>0,
                                         -relwidth=>0.25,-relheight=>1);
$checkbt->bind("<Button-3>", \&checkbt_help);
#---
$updbtext="UpdDB";
$upddbbt=$jcl_fram->Button(-text=>"UpdDB", -font=>$font2, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
			      -background=>"green",
			      -disabledforeground=>"blue",
			      -state=>'disabled',
			      -cursor=>hand2,
			      -textvariable=>\$updbtext,
                              -command => \&upd_db)
			         ->place(
                                     -relx=>0.5,-rely=>0,
                                     -relwidth=>0.25,-relheight=>1);
$upddbbt->bind("<Button-3>", \&upddbbt_help);
#---
$quitsbt=$jcl_fram->Button(-text => "QuitSess", -font=>$font2,
                               -activebackground=>"yellow",
			       -activeforeground=>"red",
			       -background=>"green",
			       -disabledforeground=>"blue",
			       -state=>'disabled',
			       -cursor=>hand2,
                               -command => \&quits_actions)->place(
                                         -relx=>0.75,-rely=>0,
                                         -relwidth=>0.25,-relheight=>1);
$quitsbt->bind("<ButtonRelease-3>", \&quitsbt_help);
}
#------------------------------------------------------
sub open_help_window
{
  my $helpfile="TofHelpTAL2DB.txt";
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
sub convert_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise($mwnd);
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
sub scanbt_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise($mwnd);
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "Scan-Button:",'end');
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
sub dryrbt_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise($mwnd);
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "DB ReadOnly-Button:",'end');
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
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise($mwnd);
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "Check-Button:",'end');
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
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise($mwnd);
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
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise($mwnd);
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
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise($mwnd);
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
sub quitsbt_help{
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise($mwnd);
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
#  $mwnd->after(500);
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->deiconify();
    $helpwindow->raise($mwnd);
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
#------------------------------------------------------
sub TAUL2DB_Welcome
{
  $ResetHistFlg=0;
  $PrevSessUTC=0;
  $PrevSessTime=0;
  my ($sec,$min,$hour,$day,$month,$year,$ptime,$syear);
  my ($status,$fn);
  my $dir=$workdir.$amsjwd;
  my $cmd,$pid;
  my (@array,@string,@substrg);
  my ($format,$title,$line,$dattim,$date,$time,$runn,$reffn,$nel,$tag);
#
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
  $nel=length($SessName);
  my $subn=substr($SessName,0,$nel-1);
  $line="       ----- WELCOME for updating of TOF $subn-parameters in DB !!! -----\n\n";
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
#-----
  my $dbd=$AMSDD.$DBDir;
  show_messg("   <--- Current DB-directory is $dbd","Big");
#-----
  $fn="TofRefCflistList.TAUC";
  open(CFLIST,"< $fn") or show_warn("   <-- Cannot open $fn for reading $!");
  while(defined ($line = <CFLIST>)){
    chomp $line;
    if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
    $line =~s/^\s+//s;# remove " "'s at the beg.of line
    push(@RefCflistList,$line);
  }
  close(CFLIST) or show_warn("   <-- Cannot close $fn after writing $!");
  $nel=scalar(@RefCflistList);
  show_messg("\n   <--- Found $nel RefCflist-files  !!!");
#
  $refcfl_om->configure(-options => [@RefCflistList]);
  $refcalsetn=$RefCflistList[0];
#
#---> show list of processed(DB-write attempts) calf-sets from prev.session :
#
  my $histfn="TofDBupdHist.".$SessName;
  my @DBupdfHist=();
  open(HISTF,"< $histfn") or show_warn("   <-- Cannot open $histfn for reading $!");
  while(defined ($line = <HISTF>)){
    chomp $line;
    if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
    $line =~s/^\s+//s;# remove " "'s at the beg.of line
    push(@DBupdfHist,$line);
  }
  close(HISTF) or show_warn("   <-- Cannot close $histfn after writing $!");
  $nel=scalar(@DBupdfHist);
  if($CalType eq "TAU2DB"){show_messg("\n   <--- Found $nel DBupd-attempts for TAUC-calib in prev.Session ");}
  if($CalType eq "TdcL2DB"){show_messg("\n   <--- Found $nel DBups-attempts for TDCL-calib in prev.Session ");}
  if($nel>0){
    $title="          CalSetN     UpdStat(1/2=not/updated)\n";
    $logtext->insert('end',$title,'ItalText');
    $logtext->yview('end');
    for($i=1;$i<=$nel;$i++){
      $line=$DBupdfHist[$i-1];
      $tag='Blue';
      $logtext->insert('end',"        ".$line."\n",$tag);
      $logtext->yview('end');
    }
  }
#
#   
#---> scan calf-store directory for related files
#
  $updbtext="UpdDB";
#
  $pathcalfs=$workdir.$CalfDir;#complete path to tof cal-files store directory
  $patharch=$workdir.$CalfDir.$archdir;#complete path to tof cal-files archive-directory
#---
  my $cftype;
  if($CalType eq "TAU2DB"){$cftype=0;}
  if($CalType eq "TdcL2DB"){$cftype=6;}
  $cmd="ls -ogt --time-style=long-iso ".$pathcalfs."/".$cfilenames[$cftype].".*[0-9][^_] |";
#
  my $cfinstore=0;
  my @cflist=();
  @array=();
  $pid=open(FLIST,$cmd) or show_warn("   <-- Cannot make cal-file list in store,  $!");
  while(<FLIST>){
    push(@array,$_);
  }
  close(FLIST);
#
  $cfinstore=scalar(@array);
  if($cfinstore==0){
    show_warn("\n   <--- Don't see any cal-files in store, create them first !!!");
    return;
  }
  my $rmin=2999999999;
  my $rmax=0;
  for($j=0;$j<$cfinstore;$j++){#<--- calf-loop of given type
    @string=split(/\s+/,$array[$j]);
    $date=$string[3];
    $time=$string[4];
    $dattim=$date." ".$time;# yyyy-mm-dd hh:mm
    @substrg=split(/\./,$string[5]);
    $nel=scalar(@substrg);
    $runn=$substrg[$nel-1];#number after last "."
    if($runn<$rmin){$rmin=$runn;}
    if($runn>$rmax){$rmax=$runn;}
    $line=$runn." ".$dattim;
    push(@cflist,$line);
  }
#
  show_messg("\n   <--- Found $cfinstore CalFiles(Sets) ($cfilenames[$cftype]-files) ready for DB-update  :");
  $title="         RunNum      CreationTime\n";
  $logtext->insert('end',$title,'Blue');
  $logtext->yview('end');
  for($j=0;$j<$cfinstore;$j++){#<-- 
    $line=$cflist[$j];#<-- runn/date/time
    @string=split(/\s+/,$line);
    $logtext->insert('end',"        ".$line."\n");
    $logtext->yview('end');
  }
  $line="\n";
  $logtext->insert('end',$line);
  $logtext->yview('end');
#
#--> set default range for later scan:
#
  $fnum1=$rmin;
  $fnum2=$rmax;
  if(($fnum1o ne $fnum1) || ($fnum2o ne $fnum2)){
    $fnum1o=$fnum1;
    $fnum2o=$fnum2;
    $fdat1=run2time($fnum1);
    $fdat2=run2time($fnum2);
    $fdat1o=$fdat1;
    $fdat2o=$fdat2;
  }
#---
  show_messg("\n   <--- Please login otherwise only scan-function is allowed !!!","Big");
}
#------------------------------------------------------
sub EditJobScript
{
#
#--- View/Edit AMS-job script file:
#
  if($JobScriptN ne ""){
    my $stat=0;
    my $fname=$workdir."/".$JobScriptN;
    if($LoginDone==1){$stat=system("nedit $fname");}
    else{$stat=system("nedit -read $fname");}
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
sub scand{
# scan cal-files-directory for needed files in required date-window
#(imply: date<=>runn conversion is done by prior clicking of "RunNumber/RunDate" button !!!)
  my $state="normal";
  $RunNum+=1;
  $curline="\n =====> New Scan $RunNum started...\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
#---
  if($LoginDone==1){
    $upddbbt->configure(-state=>$state);
    $updbtext="UpdDB";
    $checkbt->configure(-state=>$state);
    $quitsbt->configure(-state=>$state);
  }
#---
  $pathcalfs=$workdir.$CalfDir;#complete path to tof cal-files store directory
  $patharch=$workdir.$CalfDir.$archdir;#complete path to tof cal-files archive-directory
  if($arcscval==1){#want to scan archive instead of normal store $pedsdir
    $pathcalfs=$patharch;
    $curline="   <-- Scaning Archive !!!\n\n";
    $logtext->insert('end',$curline);
    $logtext->yview('end');
  }
#
  my $patt2find="";
  my $cftype;
  if($CalType eq "TAU2DB"){$cftype=0;}
  if($CalType eq "TdcL2DB"){$cftype=6;}
  $patt2find="\^$cfilenames[$cftype]"."."."\\d+\$";
#--- scan directory to create cal-files list :
  @filelist=();
  opendir(DIR,$pathcalfs);
  @filelist=grep{/$patt2find/} readdir(DIR);
  closedir(DIR);
#
  $nelem=scalar(@filelist);
  if($nelem == 0){
    show_warn("\n   <--- CalFilesStore directory doesn't contain files of type '$cfilenames[$cftype]' ?");  
    show_warn("   <--- Check selections and repeat scan !!!");  
    return;
  }
#--- sort list in the increasing order of run-number (it is unix-time of calibration):
  @processed = ();#clear list-pattern of processed files
  @sfilelist= ();#clear sorted list
  @runs = ();
#
  my $fname,$namelen;
  $fname=$cfilenames[$cftype];
  $namelen=length($fname)+1;# +1 stay for "."
  my $unpackfmt="x".$namelen." A*";
  @sfilelist = sort{unpack($unpackfmt,$a) <=> unpack($unpackfmt,$b)} @filelist;# sort by runN in incr.order
#
#--- make necessary selection of files in the ordered list(make pattern of good ones):
#
  my ($yy1,$mm1,$dd1,$hh1,$mn1,$ss1);
  my ($yy2,$mm2,$dd2,$hh2,$mn2,$ss2);
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time);
  $ss1=0;
  ($yy1,$mm1,$dd1,$hh1,$mn1,$ss1)=unpack("A4 x1 A2 x1 A2 x1 A2 x1 A2 x1 A2",$fdat1);#unpack dates low limit
  $mm1=$mm1;
  $TIMEL=timelocal($ss1,$mn1,$hh1,$dd1,$mm1-1,$yy1-1900);# UTC(GMT) in seconds (imply local time as input)
#   
  $ss2=0;
  ($yy2,$mm2,$dd2,$hh2,$mn2,$ss2)=unpack("A4 x1 A2 x1 A2 x1 A2 x1 A2 x1 A2",$fdat2);#unpack dates high limit
  $mm2=$mm2;
  $TIMEH=timelocal($ss2,$mn2,$hh2,$dd2,$mm2-1,$yy2-1900);
#
  my @substr=();
#--
  $nselem=0;
  for($i=0;$i<$nelem;$i++){#<--- loop over list to select files in dates window
    @substr=split(/\./,$sfilelist[$i]);
    $nel=scalar(@substr);
    $runs[$i]=$substr[$nel-1];#number after last "."
    $ptime=localtime($runs[$i]);#local time (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
#
    $time=$year."/".$month."/".$day." ".$hour.":".$min.":".$sec;
#
    if($processed[$i] == 0){
      if(($runs[$i]>=$TIMEL && $runs[$i]<=$TIMEH) || ($TIMEL > $TIMEH)){
        $nselem+=1;
        $processed[$i]=1;# set status to 'found'
	if($nselem==1){
	  show_messg("\n   <--- List of calib-files in search window:","B");
	}
        $curline="      file $sfilelist[$i] is selected, date: ".$time."\n";
        $logtext->insert('end',$curline);
        $logtext->yview('end');
      }
#
    }
  }#---> endof files loop
#---
  if($nselem == 0){
    show_warn("\n   <--- None of files were selected - change date window and repeat scan !!!");
    return;
  }
  else{
    show_messg("\n   <--- Found $nselem cal-files in dates window:");
  }
#--- find min/max run# in the dates window:
  $runmx=0;
  for($i=0;$i<$nelem;$i++){
    if($processed[$i] == 1){
      if($runs[$i] > $runmx) {$runmx=$runs[$i];}
    }
  }
  $runmn=$runmx;
  for($i=0;$i<$nelem;$i++){
    if($processed[$i] == 1){
      if($runs[$i] <= $runmn) {$runmn=$runs[$i];}
    }
  }
#
  if(($runmx-$runmn)/$binw_tev >= $canscrx2){
    show_warn("\n   <--- Dates window is too big (>= 2 weeks), change limits  !!!");
    return;
  }
#
#--> redefine some run-dependent params(here SetupNameame) and set other job params:
#
  if($runmx<1230764399){#--->2008
    $SetupName="AMS02PreAss";
    $MagStat=-1;# warm(=off)
    $AmsPos=1;#on earth
  }
  elsif($runmn>1230764399 && $runmx<1262300400){#--->2009
    $SetupName="AMS02Ass1";
    $MagStat=-1;# warm(=off)
    $AmsPos=1;#on earth
  }
  elsif($runmn>1262300400 && $runmx<1284069601){#--->2010 < 10sept
    $SetupName="AMS02Ass1";
    $MagStat=1;# cold(=on)
    $AmsPos=1;# on earth
  } 
  elsif($runmn>1284069601 && $runmx>1284069601){#--->2010 > 10sept
    $SetupName="AMS02Space";
    $MagStat=1;# cold(=on)
    $AmsPos=0;# in space
  }
  else{
    show_warn("\n   <-- Wrong dates(runs) range definition, please correct it !!!");
    return;
  }
#---
  $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
  $year=$ptime->year+1900;
  $month=$ptime->mon+1;
  $day=$ptime->mday;
  $hour=$ptime->hour;
  $min=$ptime->min;
  $sec=$ptime->sec;
  $time=$year."/".$month."/".$day." ".$hour.":".$min.":".$sec;
  $curline="       Date(local) of the earliest run in the dir : ".$time."\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
#
  $ptime=localtime($runmx);#local time of the latest run in the dir 
  $year=$ptime->year+1900;
  $month=$ptime->mon+1;
  $day=$ptime->mday;
  $hour=$ptime->hour;
  $min=$ptime->min;
  $sec=$ptime->sec;
  $time=$year."/".$month."/".$day." ".$hour.":".$min.":".$sec;
  $curline="       Date(local) of the   latest run in the dir : ".$time."\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
#
  $curline="  **** If you need to modify dates window - do that now and repeate scan !!! ****\n\n";
  $logtext->insert('end',$curline,'Attention');
  $logtext->yview('end');
  $mwnd->update;
#---
  if($LoginDone!=1){
    return;
  }
#
#--- create standard list-file with selected file-names:
#
#  $sfname="FilesList.P2DB";
#  my $fn=$workdir."/".$sfname;
#  open(OUTFN,"> $fn") or die show_warn("   <-- Cannot open $sfname $!");
#  for($i=0;$i<$nelem;$i++){#<--- selected files --> list-file
#    if($processed[$i] == 1){
#     print OUTFN $sfilelist[$i],"\n";#write to file 
#    }
#  }
#
#  $curline="   <-- File-list is saved in $sfname\n\n";
#  $logtext->insert('end',$curline);
#  $logtext->yview('end');
#
}
#----------------------------------------------------------------
sub checkq{# create TopLevel with canvas and control panel
#
  if($CalType eq "TAU2DB"){checkq1();}
  if($CalType eq "TdcL2DB"){checkq2();}
}
#----------------------------------------------------------------
sub checkq1{# create TopLevel with canvas and control panel
#
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
  $topl1->title("Calibration parameters quality control");
#
  my $cnvrsz=0.7;# width of display area
  my $nclines=7;
  my $bspac=0.005;
  my $offset=0.07;
  my $bheight=(1-$offset-$nclines*$bspac)/$nclines;
  my $crely;
#------
  $cp_fram=$topl1->Frame(-label => "Control panel",-background => "Cyan",  
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>(1-$cnvrsz), -relheight=>1,
                                                    -relx=>0, -rely=>0);
  $crely=$offset;
#-------  
  $plot1_bt=$cp_fram->Button(-text=>"General Calib-Status",        
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&make_plot1)->place(
                                                                 -relwidth=>0.5, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $plot1_bt->bind("<Button-3>", \&plot1bt_help);
#---
  $plot2_bt=$cp_fram->Button(-text=>"SidesTimeOffs, ID=LBB(SP)",        
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&make_plot2)->place(
                                                                 -relwidth=>0.5, -relheight=>$bheight,
                                                                           -relx=>0.5, -rely=>$crely);
#
  $plot2_bt->bind("<Button-3>", \&plot2bt_help);
  $crely+=($bheight+$bspac);
#-------  
  $plot3_bt=$cp_fram->Button(-text=>"Paddle T0, ID=LBB(SP)",        
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&make_plot3)->place(
                                                                 -relwidth=>0.5, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $plot3_bt->bind("<Button-3>", \&plot3bt_help);
#---
  $plot4_bt=$cp_fram->Button(-text=>"Anode RelAmplF, ID=LBBS(P)",        
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&make_plot4)->place(
                                                                 -relwidth=>0.5, -relheight=>$bheight,
                                                                           -relx=>0.5, -rely=>$crely);
  $plot4_bt->bind("<Button-3>", \&plot4bt_help);
#
  $crely+=($bheight+$bspac);
#-------  
  $plot5_bt=$cp_fram->Button(-text=>"Anode2Dyn AmplF, ID=LBBS(P)",        
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&make_plot5)->place(
                                                                 -relwidth=>0.5, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $plot5_bt->bind("<Button-3>", \&plot5bt_help);
#---
  $plot6_bt=$cp_fram->Button(-text=>"RelDynodeAmplF, ID=LBBSP",        
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&make_plot6)->place(
                                                                 -relwidth=>0.5, -relheight=>$bheight,
                                                                           -relx=>0.5, -rely=>$crely);
  $plot6_bt->bind("<Button-3>", \&plot6bt_help);
#
  $crely+=($bheight+$bspac);
#--------
  $chidvar="10410(LBBSP)";
  $cp_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$font3,
                                              -textvariable=>\$chidvar)->place(
                                              -relwidth=>0.4, -relheight=>$bheight,  
                                              -relx=>0, -rely=>$crely);
#---------
  $plot7_bt=$cp_fram->Button(-text=>"AbsNorm for PaddleType :",        
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&make_plot7)->place(
                                                                 -relwidth=>0.5, -relheight=>$bheight,
                                                                           -relx=>0.4, -rely=>$crely);
  $plot7_bt->bind("<Button-3>", \&plot7bt_help);
#---
  $chidvar2="1";
  $cp_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$font3,
                                              -textvariable=>\$chidvar2)->place(
                                              -relwidth=>0.1, -relheight=>$bheight,  
                                              -relx=>0.9, -rely=>$crely);
#
  $crely+=($bheight+$bspac);
#--------
$fileidvar="";
$fentrw=$cp_fram->Entry(-relief=>'sunken', -background=>yellow,
#                                             -foreground=>"blue",
                                              -font=>$font3,
                                              -textvariable=>\$fileidvar)->place(
                                              -relwidth=>0.6, -relheight=>$bheight,  
                                              -relx=>0, -rely=>$crely);
#---
$fileidvar1="";
$fentrw1=$cp_fram->Entry(-relief=>'sunken', -background=>yellow,
#                                             -foreground=>"blue",
                                              -font=>$font3,
                                              -textvariable=>\$fileidvar1)->place(
                                              -relwidth=>0.4, -relheight=>$bheight,  
                                              -relx=>0.6, -rely=>$crely);
#
  $crely+=($bheight+$bspac);
#-------
  $logf_text="View JobLogFile";
  $cp_fram->Button(-textvariable=>\$logf_text,  -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&view_JobLog)->place(
                                                        -relwidth=>0.333, -relheight=>$bheight,
                                                        -relx=>0, -rely=>$crely);
#---
  $calf_text="View CalibFile";
  $cp_fram->Button(-textvariable=>\$calf_text,  -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&view_CalFile)->place(
                                                        -relwidth=>0.333, -relheight=>$bheight,
                                                        -relx=>0.333, -rely=>$crely);
#---
  $remove_text="Del/Restore CalSet";
  $rmfset_bt=$cp_fram->Button(-textvariable=>\$remove_text,  -borderwidth=>3,
                                                        -activebackground=>"yellow",
#			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -foreground=>"red",
			                                -cursor=>hand2,
                                                        -command=>\&mark_file)->place(
                                                        -relwidth=>0.333, -relheight=>$bheight,
                                                        -relx=>0.666, -rely=>$crely);
#
  $crely+=($bheight+$bspac);
#--------
  
  $resplbt=$cp_fram->Button(-text=>"ResetPlot",               -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&points_clear)->place(
                                                        -relwidth=>0.333, -relheight=>$bheight,
                                                        -relx=>0, -rely=>$crely);
  $resplbt->bind("<Button-3>", \&resplbt_help);
#----
  $cp_fram->Button(-text=>"Save plot",               -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&plot_save)->place(
                                                        -relwidth=>0.333, -relheight=>$bheight,
                                                        -relx=>0.333, -rely=>$crely);
#----
  $cp_fram->Button(-text=>"Close",                      -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>sub{$topl1->destroy();})->place(
                                                        -relwidth=>0.333, -relheight=>$bheight,
                                                        -relx=>0.666, -rely=>$crely);
#
  $crely+=($bheight+$bspac);
#--------
  $cv_fram=$topl1->Frame(-label => "Params time-evolution window", 
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>$cnvrsz, -relheight=>1,
                                                    -relx=>(1-$cnvrsz), -rely=>0);
  $canscrx1=-40;# scroll-area (min/max), all in pixels
  $canscry1=-500;
  $canscrx2=10000;#at $binw_tev=120(sec/pix) corresp.full time-scale almost 2weeks
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
  $plot5_actf=0;#plot5 "active" flag
  $plot6_actf=0;#plot6 "active" flag
  $plot7_actf=0;#plot7 "active" flag
#
  &access_clear;
  &points_clear;
  @plot_fileids=();# clear current plot point-assosiated files ids
#
  $topl1->bell;
#  print "\aQUQU\n";
}
#----------------------------------------------------------------
sub checkq2{# create TopLevel with canvas and control panel
#
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
  $topl1->title("Calibration parameters quality control");
#
  my $cnvrsz=0.7;# width of display area
  my $nclines=6;
  my $bspac=0.005;
  my $offset=0.07;
  my $bheight=(1-$offset-$nclines*$bspac)/$nclines;
  my $crely;
#------
  $cp_fram=$topl1->Frame(-label => "Control panel",-background => "Cyan",  
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>(1-$cnvrsz), -relheight=>1,
                                                    -relx=>0, -rely=>0);
  $crely=$offset;
#-------  
  $plot10_bt=$cp_fram->Button(-text=>"Max(+)/Min(-) NonLinearity(ps), ChID=C(rate)S(lot)I(nput)",        
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&make_plot10)->place(
                                                                 -relwidth=>1, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $plot10_bt->bind("<Button-3>", \&plot10bt_help);
  $crely+=($bheight+$bspac);
#-------  
  $plot11_bt=$cp_fram->Button(-text=>"Max/Min NonLin BinNumber, ID=CSI",        
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&make_plot11)->place(
                                                                 -relwidth=>1, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $plot11_bt->bind("<Button-3>", \&plot11bt_help);
#
  $crely+=($bheight+$bspac);
#-------  
  $plot12_bt=$cp_fram->Button(-text=>"Integral NonLinearity, ID=CSI",        
                                                        -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&make_plot12)->place(
                                                                 -relwidth=>0.5, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $plot12_bt->bind("<Button-3>", \&plot12bt_help);
#---
  $chidvar="111(CSI)  ";
  $cp_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$font3,
                                              -textvariable=>\$chidvar)->place(
                                              -relwidth=>0.5, -relheight=>$bheight,  
                                              -relx=>0.5, -rely=>$crely);
#
  $crely+=($bheight+$bspac);
#-------
$fileidvar="";
$fentrw=$cp_fram->Entry(-relief=>'sunken', -background=>yellow,
#                                             -foreground=>"blue",
                                              -font=>$font3,
                                              -textvariable=>\$fileidvar)->place(
                                              -relwidth=>0.6, -relheight=>$bheight,  
                                              -relx=>0, -rely=>$crely);
#---
$fileidvar1="";
$fentrw1=$cp_fram->Entry(-relief=>'sunken', -background=>yellow,
#                                             -foreground=>"blue",
                                              -font=>$font3,
                                              -textvariable=>\$fileidvar1)->place(
                                              -relwidth=>0.4, -relheight=>$bheight,  
                                              -relx=>0.6, -rely=>$crely);
#
  $crely+=($bheight+$bspac);
#-------
  $logf_text="View JobLogFile";
  $cp_fram->Button(-textvariable=>\$logf_text,  -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&view_JobLog)->place(
                                                        -relwidth=>0.333, -relheight=>$bheight,
                                                        -relx=>0, -rely=>$crely);
#---
  $calf_text="View CalibFile";
  $cp_fram->Button(-textvariable=>\$calf_text,  -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&view_CalFile)->place(
                                                        -relwidth=>0.333, -relheight=>$bheight,
                                                        -relx=>0.333, -rely=>$crely);
#---
  $remove_text="Del/Restore CalSet";
  $rmfset_bt=$cp_fram->Button(-textvariable=>\$remove_text,  -borderwidth=>3,
                                                        -activebackground=>"yellow",
#			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -foreground=>"red",
			                                -cursor=>hand2,
                                                        -command=>\&mark_file)->place(
                                                        -relwidth=>0.333, -relheight=>$bheight,
                                                        -relx=>0.666, -rely=>$crely);
#
  $crely+=($bheight+$bspac);
#--------
  
  $resplbt=$cp_fram->Button(-text=>"Reset plot",               -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&points_clear)->place(
                                                        -relwidth=>0.333, -relheight=>$bheight,
                                                        -relx=>0, -rely=>$crely);
  $resplbt->bind("<Button-3>", \&resplbt_help);
#----
  $cp_fram->Button(-text=>"Save plot",               -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&plot_save)->place(
                                                        -relwidth=>0.333, -relheight=>$bheight,
                                                        -relx=>0.333, -rely=>$crely);
#----
  $cp_fram->Button(-text=>"Close",                      -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>sub{$topl1->destroy();})->place(
                                                        -relwidth=>0.333, -relheight=>$bheight,
                                                        -relx=>0.666, -rely=>$crely);
#
  $crely+=($bheight+$bspac);
#--------
  $cv_fram=$topl1->Frame(-label => "Params time-evolution window", 
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>$cnvrsz, -relheight=>1,
                                                    -relx=>(1-$cnvrsz), -rely=>0);
  $canscry1=-500;
  $canscry2=20;
  $cnv=$cv_fram->Scrolled("Canvas",-background => "black",
                                        -scrollregion=>[$canscrx1,$canscry1,
					                $canscrx2,$canscry2])
                                                              ->pack(-fill => 'both', -expand => 1);
  $canvas=$cnv->Subwidget("canvas");
    
  $xaxe=$canvas->createLine($canscrx1,0,$canscrx2,0, -fill=>"yellow", -arrow=>"last", -width=>2);
  $yaxe=$canvas->createLine(0,$canscry2,0,$canscry1, -fill=>"yellow", -arrow=>"last", -width=>2);
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
  $plot5_actf=0;#plot5 "active" flag
  $plot6_actf=0;#plot6 "active" flag
  $plot7_actf=0;#plot7 "active" flag
#
  $plot10_actf=0;#plot10 "active" flag
  $plot11_actf=0;#plot11 "active" flag
  $plot12_actf=0;#plot12 "active" flag
  $plot13_actf=0;#plot13 "active" flag
#
  &access_clear;
  &points_clear;
  @plot_fileids=();# clear current plot point-assosiated files ids
#
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
  elsif($plot5_actf>0){
    $fname="plot_5_v".$plot5_actf.".ps";
    $width=($plot5_xcmax-$canscrx1)+50;
    $xcmax=$plot5_xcmax;
  }
  elsif($plot6_actf>0){
    $fname="plot_6_v".$plot6_actf.".ps";
    $width=($plot6_xcmax-$canscrx1)+50;
    $xcmax=$plot6_xcmax;
  }
  elsif($plot7_actf>0){
    $fname="plot_7_v".$plot7_actf.".ps";
    $width=($plot7_xcmax-$canscrx1)+50;
    $xcmax=$plot7_xcmax;
  }
#
  elsif($plot10_actf>0){
    $fname="plot_10_v".$plot10_actf.".ps";
    $width=($plot10_xcmax-$canscrx1)+10;
    $width=($canscrx2-$canscrx1);
    $xcmax=$plot10_xcmax;
  }
#
  elsif($plot11_actf>0){
    $fname="plot_11_v".$plot11_actf.".ps";
    $width=($plot11_xcmax-$canscrx1)+10;
    $width=($canscrx2-$canscrx1);
    $xcmax=$plot11_xcmax;
  }
#
  elsif($plot12_actf>0){
    $fname="plot_12_v".$plot12_actf.".ps";
    $width=($plot12_xcmax-$canscrx1)+10;
    $width=($canscrx2-$canscrx1);
    $xcmax=$plot12_xcmax;
  }
#
  elsif($plot13_actf>0){
    $fname="plot_13_v".$plot13_actf.".ps";
    $width=($plot13_xcmax-$canscrx1)+10;
    $width=($canscrx2-$canscrx1);
    $xcmax=$plot13_xcmax;
  }
#
  $canvas->postscript(-file=>$fname,-width=>$width,-height=>$height,-x=>$canscrx1,-y=>$canscry1);
  $curline="   <-- Plot is saved in $fname\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
}
#-----------------------------------------------------------------
sub make_plot1
{
#
#---> time-evolution of CStat-file's statuses(%)
#
  my ($xcoarsegrd,$xfinegrd)=(86400,3600);# day/hour in sec
  my ($ycoarsegrd,$yfinegrd)=(50,10);#(%)
  my $ymaxv=125;#(%)
#
  show_warn("");
  my $layx,$barx,$sidx,$pmtx,$rest;
  my $runn;
  my $maxb;
  my $locvar;
  my @gcolors=qw(green darkred blue orange black );
  $curline="   <-- GoodStatus-Channels(%) Time-scan \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
#
  my ($ped,$sig,$sta)=(0,0,0);
  my ($totcha,$totchd,$nempcha,$nempchd,$gstacha,$gstachd)=(0,0,0,0,0,0);
  my ($nemppora,$nemppord,$gstapora,$gstapord);
  my ($laymx,$barmx,$pmmx);
#
  my ($sta_aamp,$sta_atim,$sta_ahis);
  my ($gtimch,$gampch,$ghisch,$gdynch,$gsdyn);
  my @sta_dyn=();
#
  $laymx=4;
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
#------ create Grid vertical lines + X-axes subscripts :
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
    $ptime=localtime($runmn);#loc.time of earliest run in runs-window (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time=$year."/".$month."/".$day."  ".$hour.":".$min;
    $titl="Good Status Channels(%) vs time,  '0'-time is $time";
    $vcoo=int(120*$canscry1/$ymaxv);#place at val=120%
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font2c,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl=" (blue-TimeCh, green-AmplCh(a), orange-HistCh, darkred-DynCh)";
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
    $plot5_actf=0;
    $plot6_actf=0;
  }
#---
  $plot1_actf+=1;
#---
  for($i=0;$i<$nelem;$i++){#
    $runn=$runs[$i];
    $sfname=$pathcalfs."/".$cfilenames[1].".".$runn;
    $nzlines=0;
    $nwords=0;
    @globpeds=();#store all data of the file
    if($processed[$i] == 1){
#
      open(INPFN,"< $sfname") or die show_warn("   <-- Cannot open $sfname for reading $!");
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
#--
      $seqnum=0;
#--->first - anodes:
      $totcha=0;
      $nempcha=0;
      $gtimch=0;
      $gampch=0;
      $ghisch=0;
      $gdynch=0;
      $gstacha=0;
      $nemppora=0;
      $gstapora=0;
      for($il=0;$il<$laymx;$il++){
        $barmx=$barsppl[$il];
        for($ib=0;$ib<$barmx;$ib++){
	  $pmmx=2;
	  if(($il==0 || $il==($laymx-1)) && ($ib==0 || $ib==($barmx-1))){$pmmx=3;}
          for($is=0;$is<2;$is++){
	    $totcha+=1;
	    $sta=$globpeds[$seqnum+$is];
	    $sta_aamp=0;
	    $sta_atim=0;
	    $sta_ahis=0;
	    @sta_dyn=();
	    if(($sta/100000)>0){$sta_ahis=1;}#bad
	    if((($sta%100000)/10000)>0){$sta_atim=1;}#bad
	    if((($sta%10000)/1000)>0){$sta_aamp=1;}#bad
	    if((($sta%1000)/100)>0){$sta_dyn[2]=1;}#bad 3rd dyn
	    if((($sta%1000)/10)>0){$sta_dyn[1]=1;}#bad 2nd dyn
	    if(($sta%10)>0){$sta_dyn[0]=1;}#bad 1st dyn
	    if(($sta_atim==0) || ($sta_aamp==0)){$nempcha+=1;}
	    if($sta_atim==0){$gtimch+=1;}
	    if($sta_aamp==0){$gampch+=1;}
	    if($sta_ahis==0){$ghisch+=1;}
	    $gsdyn=0;
            for($pm=0;$pm<pmmx;$pm++){
	      if($sta_dyn[$pm]==0){$gsdyn+=1;}
	    }
	    if($gsdyn==pmmx){$gdynch+=1;}
	  }#-->endof side-loop
	  $seqnum+=2;
#
        }#-->endof bar-loop
#
      }#-->endof lay-loop
      if($totcha>0){
        $gtimchp=100.*$gtimch/$totcha;
        $gampchp=100.*$gampch/$totcha;
        $ghischp=100.*$ghisch/$totcha;
        $gdynchp=100.*$gdynch/$totcha;
#	print " gt/ga/gh/gd=",$gtimchp," ",$gampchp," ",$ghischp," ",$gdynchp,"\n";
      }
#--
      $xc=($runs[$i]-$runmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
#
#anodes(timch,ampch):
      $vcoo=int(-$gtimchp*$canscry1/$ymaxv+0.5);# pos.in pix
      $curcol=$gcolors[2];#blue
      $lid=$canvas->createOval($xc-3,-($vcoo-4),$xc+3,-($vcoo+4), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
      $vcoo=int(-$gampchp*$canscry1/$ymaxv+0.5);
      $curcol=$gcolors[0];#green
#      if($gstapora>90){$curcol=$gcolors[0];}#green
#      else{$curcol=$gcolors[1];}#red
      $lid=$canvas->createOval($xc-4,-($vcoo-3),$xc+4,-($vcoo+3), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
#anode(histch),dynodes:
      $vcoo=int(-$ghischp*$canscry1/$ymaxv+0.5);
      $curcol=$gcolors[3];#orange
      $lid=$canvas->createRectangle($xc-3,-($vcoo-4),$xc+3,-($vcoo+4), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
      $vcoo=int(-$gdynchp*$canscry1/$ymaxv+0.5);
      $curcol=$gcolors[1];#darkred
#      if($gstapord>90){$curcol=$gcolors[0];}#green
#      else{$curcol=$gcolors[1];}#red
      $lid=$canvas->createRectangle($xc-4,-($vcoo-3),$xc+4,-($vcoo+3), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
#
      if($xc>$plot1_xcmax){$plot1_xcmax=$xc;}
    }
    $topl1->update;
  }
  $curline="   <-- GoodStatus-channels(%) scan was completed ! \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
}
#------------------------------------------------------------------
sub make_plot2
{
#---> time-evolution of SidesTimeOffset for channel LBB 
#
  my ($xcoarsegrd,$xfinegrd)=(86400,3600);# day/hour in sec
  my ($ycoarsegrd,$yfinegrd)=(100,50);#(ps)
  my $ymaxv=1000;#(ps)
  my $voffs=500;#scale vertical offset(ps)
#------
  show_warn("");
  my $layx,$barx,$sidx,$pmtx,$rest;
  my $maxb;
  my $locvar;
  my @goodcolorlist=qw(green darkred blue orange black );
  $locvar=$chidvar;
#--> check ID :
  if($locvar =~/(LBBSP)/){substr($locvar,-7,7)="";}# remove "(LBBSP)" if present
  ($layx,$barx,$sidx,$pmtx,$rest)=unpack("A1 A2 A1 A1 A*",$locvar);
  if($layx>4 || $layx==0 || $barx==0 || $barx>$barsppl[$layx-1] || $sidx==0 || $sidx>2 || $pmtx>3){
    show_warn("   <-- wrong TOF channel-ID setting: $locvar ?!");
    return;
  }
  show_messg("\n   <--- 2SidesTimesOffset Time-scan for id=$locvar, l/bb=$layx/$barx");
#--
  my $seqnum;
  my ($xc,$yc)=(0,0);
  my $ypbin=int(-$canscry1/$ymaxv+0.5);#(pixels/bin)
  my $ycoarsegrdp=int(-$canscry1*$ycoarsegrd/$ymaxv+0.5);#coarse-grid in pix
  my $yfinegrdp=int(-$canscry1*$yfinegrd/$ymaxv+0.5);#fine-grid in pix
  my $voffspix=int(-$canscry1*$voffs/$ymaxv+0.5);# vert,offset in pix
  my $goodc="darkgreen";
  my $badc="red";
  my $nzlines=0;
  my $curcol;
  my $colindx;
  my ($width,$dig,$ltxt,$colr,$colrl,$dasht,$titl);
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time);
  my $vcoo;
#--
  my ($vval,$runn,$fn,$fnn,$line,$lvelos,$lvelosr,$stoffs,$stoffsr);
#
  @globpeds=();
#
#---> create scale/grid:
#
  if($plot2_actf==0){# 1st entry for plot2
    &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
#----- create Grid vertical lines + X-axes subscripts :
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
       if($i>0 && (($i % $yfinegrdp)==0 || ($i % $ycoarsegrdp)==0 || ($i % $voffspix)==0)){
         $yc=$i;
	 if(($i % $voffspix)==0){# "0"-offset line
	   $width=2;
	   $dig=$i*$voffs/$voffspix-$voffs;
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='';
	   $colrl="darkblue";
	 }
	 elsif(($i % $ycoarsegrdp)==0){# coarse scale line
	   $width=1;
	   $dig=$i*$ycoarsegrd/$ycoarsegrdp-$voffs;
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	   $colrl="black";
	 }
	 else{# fine scale line
	   $width=1;
	   $dig=$i*$yfinegrd/$yfinegrdp-$voffs;
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	   $colrl="black";
	 }
         $lid=$canvas->createLine(0,-$yc,$canscrx2,-$yc, -fill=>$colrl, -width=>$width,-dash=>$dasht);
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
    $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time=$year."/".$month."/".$day."  ".$hour.":".$min;
    $titl="Paddle's 2SidesTimeDiff Td-Tdref [picosec] Time-evolution,  '0'-time is $time";
    $vcoo=int(950*$canscry1/$ymaxv);#place at val=950 ps
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font2c,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl="Time-scale: red digits ->days, blue ->hours";
    $vcoo=int(20*$canscry1/$ymaxv);#place at val=10 ps
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font2c,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#
#---> Read related RefCal-file data:
#
    @RefCFileData=();
    my @CflFileMemb=();
    $fn=$cfilenames[0].".".$refcalsetn;
    $ffn=$workdir.$amsjwd."/".$fn;
    open(RFMEMB,"< $ffn") or show_warn("   <-- Cannot open $fn for reading, $!");
    while(defined ($line = <RFMEMB>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@CflFileMemb,$line);
    }
    close(RFMEMB) or show_warn("   <-- Cannot close $fn after reading, $!");
    $nel=scalar(@CflFileMemb);
    if($nel==0){
      show_warn("\n   <--- Plot-2: Ref.calibset $fn is not found, something wrong !!!");
      return;
    }
#
    $fn=$cfilenames[2].".".$CflFileMemb[2];# Tdelv-file 
    $ffn=$workdir.$amsjwd."/".$fn;
    open(RFMEMB,"< $ffn") or show_warn("   <-- Cannot open $fn for reading, $!");
    while(defined ($line = <RFMEMB>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      if($line =~/==========/){last;}# break on my internal EOF
      if($line =~/12345/){last;}# break on my internal EOF
      $nzlines+=1;
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      @words=split(/\s+/,$line);#split by any number of continious " "'s
      $nwords=scalar(@words);
      push(@RefCFileData,@words);
    }
    close(RFMEMB) or die show_warn("   <-- Cannot open $fn after reading $!");
#
    $plot1_actf=0;
    $plot3_actf=0;
    $plot4_actf=0;
    $plot5_actf=0;
    $plot6_actf=0;
  }
#---
  $plot2_actf+=1;
  $colindx=($plot2_actf % 5);
  $goodc=$goodcolorlist[$colindx];#change color according fill-entry number
#
  my $npadds=0;
  for($il=0;$il<4;$il++){$npadds+=$barsppl[$il];}# tot.number of paddles
#
  for($i=0;$i<$nelem;$i++){#<--- selected files loop
    $runn=$runs[$i];
    $sfname=$pathcalfs."/".$cfilenames[2].".".$runn;
    $nzlines=0;
    $nwords=0;
    @globpeds=();#store all data of the file
    if($processed[$i] == 1){
#
      open(INPFN,"< $sfname") or die show_warn("   <-- Cannot open $sfname for reading $!");
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
      for($il=0;$il<$layx-1;$il++){$seqnum+=$barsppl[$il];}# count in layers befor layx
      for($ib=0;$ib<$barx-1;$ib++){$seqnum+=1;}# count in paddles befor barx
      $lvelos=$globpeds[$seqnum];
      $lvelosr=$RefCFileData[$seqnum];
      $stoffs=1000*$globpeds[$npadds+$seqnum];#ps
      $stoffsr=1000*$RefCFileData[$npadds+$seqnum];
#
      $xc=($runs[$i]-$runmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
#
      if($sta==0){$curcol=$goodc;}
      else {$curcol=$badc;}
      $vval=int(-($stoffs-$stoffsr+$voffs)*$canscry1/$ymaxv+0.5);# in pixels
      $lid=$canvas->createOval($xc-3,-($vval-3),$xc+3,-($vval+3), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
      if($xc>$plot2_xcmax){$plot2_xcmax=$xc;}
    }#--->endof file-loop
    $topl1->update;
  }
}
#------------------------------------------------------------------
sub make_plot3
{
#---> time-evolution of Paddle's T0 for channel LBB 
#
  my ($xcoarsegrd,$xfinegrd)=(86400,3600);# day/hour in sec
  my ($ycoarsegrd,$yfinegrd)=(100,50);#(ps)
  my $ymaxv=1000;#(ps)
  my $voffs=500;#scale vertical offset(ps)
#------
  show_warn("");
  my $layx,$barx,$sidx,$pmtx,$rest;
  my $maxb;
  my $locvar;
  my @goodcolorlist=qw(green darkred blue orange black );
  $locvar=$chidvar;
#--> check ID :
  if($locvar =~/(LBBSP)/){substr($locvar,-7,7)="";}# remove "(LBBSP)" if present
  ($layx,$barx,$sidx,$pmtx,$rest)=unpack("A1 A2 A1 A1 A*",$locvar);
  if($layx>4 || $layx==0 || $barx==0 || $barx>$barsppl[$layx-1] || $sidx==0 || $sidx>2 || $pmtx>3){
    show_warn("   <-- wrong TOF channel-ID setting: $locvar ?!");
    return;
  }
  show_messg("\n   <--- Paddle T0 Time-scan for id=$locvar, l/bb=$layx/$barx");
#--
  my $seqnum;
  my ($xc,$yc)=(0,0);
  my $ypbin=int(-$canscry1/$ymaxv+0.5);#(pixels/bin)
  my $ycoarsegrdp=int(-$canscry1*$ycoarsegrd/$ymaxv+0.5);#coarse-grid in pix
  my $yfinegrdp=int(-$canscry1*$yfinegrd/$ymaxv+0.5);#fine-grid in pix
  my $voffspix=int(-$canscry1*$voffs/$ymaxv+0.5);# vert,offset in pix
  my $goodc="darkgreen";
  my $badc="red";
  my $nzlines=0;
  my $curcol;
  my $colindx;
  my ($width,$dig,$ltxt,$colr,$colrl,$dasht,$titl);
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time);
  my $vcoo;
#--
  my ($vval,$runn,$fn,$fnn,$line,$slew,$slewr,$tzero,$tzeror);
#
  @globpeds=();
#
#---> create scale/grid:
#
  if($plot3_actf==0){# 1st entry for plot3
    &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
#----- create Grid vertical lines + X-axes subscripts :
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
       if($i>0 && (($i % $yfinegrdp)==0 || ($i % $ycoarsegrdp)==0 || ($i % $voffspix)==0)){
         $yc=$i;
	 if(($i % $voffspix)==0){# "0"-offset line
	   $width=2;
	   $dig=$i*$voffs/$voffspix-$voffs;
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='';
	   $colrl="darkblue";
	 }
	 elsif(($i % $ycoarsegrdp)==0){# coarse scale line
	   $width=1;
	   $dig=$i*$ycoarsegrd/$ycoarsegrdp-$voffs;
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	   $colrl="black";
	 }
	 else{# fine scale line
	   $width=1;
	   $dig=$i*$yfinegrd/$yfinegrdp-$voffs;
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	   $colrl="black";
	 }
         $lid=$canvas->createLine(0,-$yc,$canscrx2,-$yc, -fill=>$colrl, -width=>$width,-dash=>$dasht);
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
    $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time=$year."/".$month."/".$day."  ".$hour.":".$min;
    $titl="Paddle's T0-T0ref [picosec] Time-evolution,  '0'-time is $time";
    $vcoo=int(950*$canscry1/$ymaxv);#place at val=950 ps
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font10,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl="Time-scale: red digits ->days, blue ->hours";
    $vcoo=int(20*$canscry1/$ymaxv);#place at val=10 ps
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font10,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#
#---> Read related RefCal-file data:
#
    @RefCFileData=();
    my @CflFileMemb=();
    $fn=$cfilenames[0].".".$refcalsetn;
    $ffn=$workdir.$amsjwd."/".$fn;
    open(RFMEMB,"< $ffn") or show_warn("   <-- Cannot open $fn for reading, $!");
    while(defined ($line = <RFMEMB>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@CflFileMemb,$line);
    }
    close(RFMEMB) or show_warn("   <-- Cannot close $fn after reading, $!");
    $nel=scalar(@CflFileMemb);
    if($nel==0){
      show_warn("\n   <--- Plot-2: Ref.calibset $fn is not found, something wrong !!!");
      return;
    }
#---> read ref Tzslw-file:
    $fn=$cfilenames[3].".".$CflFileMemb[3];# Tzslw-file 
    $ffn=$workdir.$amsjwd."/".$fn;
    open(RFMEMB,"< $ffn") or show_warn("   <-- Cannot open $fn for reading, $!");
    while(defined ($line = <RFMEMB>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      if($line =~/==========/){last;}# break on my internal EOF
      if($line =~/12345/){last;}# break on my internal EOF
      $nzlines+=1;
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      @words=split(/\s+/,$line);#split by any number of continious " "'s
      $nwords=scalar(@words);
      push(@RefCFileData,@words);
    }
    close(RFMEMB) or die show_warn("   <-- Cannot open $fn after reading $!");
#
    $plot1_actf=0;
    $plot2_actf=0;
    $plot4_actf=0;
    $plot5_actf=0;
    $plot6_actf=0;
  }
#---
  $plot3_actf+=1;
  $colindx=($plot3_actf % 5);
  $goodc=$goodcolorlist[$colindx];#change color according fill-entry number
#
  my $npadds=0;
  for($il=0;$il<4;$il++){$npadds+=$barsppl[$il];}# tot.number of paddles
#
  for($i=0;$i<$nelem;$i++){#<--- selected files loop
    $runn=$runs[$i];
    $sfname=$pathcalfs."/".$cfilenames[3].".".$runn;
    $nzlines=0;
    $nwords=0;
    @globpeds=();#store all data of the file
    if($processed[$i] == 1){
#
      open(INPFN,"< $sfname") or die show_warn("   <-- Cannot open $sfname for reading $!");
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
      $seqnum=1;# to skip common slewing parameter at the beg.of file
      for($il=0;$il<$layx-1;$il++){$seqnum+=$barsppl[$il];}# count in layers befor layx
      for($ib=0;$ib<$barx-1;$ib++){$seqnum+=1;}# count in paddles befor barx
      $tzero=1000*$globpeds[$seqnum];# ps
      $tzeror=1000*$RefCFileData[$seqnum];# ps
      $slew=$globpeds[0];
      $slewr=$RefCFileData[0];
#
      $xc=($runs[$i]-$runmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
#
      if($sta==0){$curcol=$goodc;}
      else {$curcol=$badc;}
      $vval=int(-($tzero-$tzeror+$voffs)*$canscry1/$ymaxv+0.5);# in pixels
      $lid=$canvas->createOval($xc-3,-($vval-3),$xc+3,-($vval+3), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
      if($xc>$plot3_xcmax){$plot3_xcmax=$xc;}
    }#--->endof file-loop
    $topl1->update;
  }
}
#------------------------------------------------------------------
sub make_plot4
{
#---> time-evolution of PaddleSide's RelAmplf for channel LBB 
#
  my ($xcoarsegrd,$xfinegrd)=(86400,3600);# day/hour in sec
  my ($ycoarsegrd,$yfinegrd)=(0.5,0.1);#
  my $ymaxv=2;#(ps)
#  my $voffs=500;#scale vertical offset(ps)
#------
  show_warn("");
  my $layx,$barx,$sidx,$pmtx,$rest;
  my $maxb;
  my $locvar;
  my @goodcolorlist=qw(green darkred blue orange black );
  $locvar=$chidvar;
#--> check ID :
  if($locvar =~/(LBBSP)/){substr($locvar,-7,7)="";}# remove "(LBBSP)" if present
  ($layx,$barx,$sidx,$pmtx,$rest)=unpack("A1 A2 A1 A1 A*",$locvar);
  if($layx>4 || $layx==0 || $barx==0 || $barx>$barsppl[$layx-1] || $sidx==0 || $sidx>2 || $pmtx>3){
    show_warn("   <-- wrong TOF channel-ID setting: $locvar ?!");
    return;
  }
  show_messg("\n   <--- PaddleSide relat.gain Time-scan for id=$locvar, l/bb/side=$layx/$barx/sidx");
#--
  my $seqnum;
  my ($xc,$yc)=(0,0);
  my $ypbin=int(-$canscry1/$ymaxv+0.5);#(pixels/bin)
  my $ycoarsegrdp=int(-$canscry1*$ycoarsegrd/$ymaxv+0.5);#coarse-grid in pix
  my $yfinegrdp=int(-$canscry1*$yfinegrd/$ymaxv+0.5);#fine-grid in pix
#  my $voffspix=int(-$canscry1*$voffs/$ymaxv+0.5);# vert,offset in pix
  my $goodc="darkgreen";
  my $badc="red";
  my $nzlines=0;
  my $curcol;
  my $colindx;
  my ($width,$dig,$ltxt,$colr,$colrl,$dasht,$titl);
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time);
  my $vcoo;
#--
  my ($vval,$runn,$fn,$fnn,$line,$slew,$slewr,$tzero,$tzeror,$gain,$gainr);
  my $val;
#
  @globpeds=();
#
#---> create scale/grid:
#
  if($plot4_actf==0){# 1st entry for plot3
    &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
#----- create Grid vertical lines + X-axes subscripts :
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
	   $dig=$i*$ycoarsegrd/$ycoarsegrdp-$voffs;
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	   $colrl="black";
	 }
	 else{# fine scale line
	   $width=1;
	   $dig=$i*$yfinegrd/$yfinegrdp-$voffs;
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	   $colrl="black";
	 }
         $lid=$canvas->createLine(0,-$yc,$canscrx2,-$yc, -fill=>$colrl, -width=>$width,-dash=>$dasht);
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
    $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time=$year."/".$month."/".$day."  ".$hour.":".$min;
    $titl="Paddle/Side's RelGain/RefFileRelGain Time-evolution,  '0'-time is $time";
    $vcoo=int(1.9*$canscry1/$ymaxv);#place at val=1.9
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font10,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl="Time-scale: red digits ->days, blue ->hours";
    $vcoo=int(0.05*$canscry1/$ymaxv);#place at val=0.05
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font10,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#
#---> Read related RefCal-file data:
#
    @RefCFileData=();
    my @CflFileMemb=();
    $fn=$cfilenames[0].".".$refcalsetn;
    $ffn=$workdir.$amsjwd."/".$fn;
    open(RFMEMB,"< $ffn") or show_warn("   <-- Cannot open $fn for reading, $!");
    while(defined ($line = <RFMEMB>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@CflFileMemb,$line);
    }
    close(RFMEMB) or show_warn("   <-- Cannot close $fn after reading, $!");
    $nel=scalar(@CflFileMemb);
    if($nel==0){
      show_warn("\n   <--- Plot-2: Ref.calibset $fn is not found, something wrong !!!");
      return;
    }
#---> read ref Tzslw-file:
    $fn=$cfilenames[4].".".$CflFileMemb[4];# Amplf-file 
    $ffn=$workdir.$amsjwd."/".$fn;
    open(RFMEMB,"< $ffn") or show_warn("   <-- Cannot open $fn for reading, $!");
    while(defined ($line = <RFMEMB>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      if($line =~/==========/){last;}# break on my internal EOF
      if($line =~/12345/){last;}# break on my internal EOF
      $nzlines+=1;
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      @words=split(/\s+/,$line);#split by any number of continious " "'s
      $nwords=scalar(@words);
      push(@RefCFileData,@words);
    }
    close(RFMEMB) or die show_warn("   <-- Cannot open $fn after reading $!");
#
    $plot1_actf=0;
    $plot2_actf=0;
    $plot3_actf=0;
    $plot5_actf=0;
    $plot6_actf=0;
  }
#---
  $plot4_actf+=1;
  $colindx=($plot4_actf % 5);
  $goodc=$goodcolorlist[$colindx];#change color according fill-entry number
#
  my $npadds=0;
  for($il=0;$il<4;$il++){$npadds+=$barsppl[$il];}# tot.number of paddles
#
  for($i=0;$i<$nelem;$i++){#<--- selected files loop
    $runn=$runs[$i];
    $sfname=$pathcalfs."/".$cfilenames[4].".".$runn;
    $nzlines=0;
    $nwords=0;
    @globpeds=();#store all data of the file
    if($processed[$i] == 1){
#
      open(INPFN,"< $sfname") or die show_warn("   <-- Cannot open $sfname for reading $!");
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
      for($il=0;$il<$layx-1;$il++){$seqnum+=2*$barsppl[$il];}# count in layers befor layx
      if($sidx>1){$seqnum+=$barsppl[$layx-1];}# count in sides befor sidx
      for($ib=0;$ib<$barx-1;$ib++){$seqnum+=1;}# count in paddles befor barx
      $gain=$globpeds[$seqnum];
      $gainr=$RefCFileData[$seqnum];
      $val=$gain/$gainr;
#   print "L/b/s=",$layx,"/",$barx,"/",$sidx,"  gain/gainr=",$gain,"/",$gainr,"\n";
#
      $xc=($runs[$i]-$runmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
#
      if($sta==0){$curcol=$goodc;}
      else {$curcol=$badc;}
      $vval=-int($val*$canscry1/$ymaxv+0.5);# in pixels
      $lid=$canvas->createOval($xc-3,-($vval-3),$xc+3,-($vval+3), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
      if($xc>$plot4_xcmax){$plot4_xcmax=$xc;}
    }#--->endof file-loop
    $topl1->update;
  }
}
#------------------------------------------------------------------
sub make_plot5
{
}
#------------------------------------------------------------------
sub make_plot6
{
}
#------------------------------------------------------------------
sub make_plot7
{
#---> time-evolution of Paddle's Edep abs.normalization for channel LBB 
#
  my ($xcoarsegrd,$xfinegrd)=(86400,3600);# day/hour in sec
  my ($ycoarsegrd,$yfinegrd)=(50,10);# pC/Mev
  my $ymaxv=125;# pC/Mev
#------
  show_warn("");
  my $layx,$barx,$sidx,$pmtx,$rest;
  my $maxb;
  my $locvar;
  my $bartype;
  my @goodcolorlist=qw(green darkred blue orange black );
  $bartype=$chidvar2;
#--> check ID :
  if($bartype<=0 || $bartype>11){
    show_warn("   <-- wrong TOF BarType-ID setting: $bartype ?!");
    return;
  }
  show_messg("\n   <--- PaddleType ADC->Edep conversion factor for BarType=$bartype !");
#--
  my $seqnum;
  my ($xc,$yc)=(0,0);
  my $ypbin=int(-$canscry1/$ymaxv+0.5);#(pixels/bin)
  my $ycoarsegrdp=int(-$canscry1*$ycoarsegrd/$ymaxv+0.5);#coarse-grid in pix
  my $yfinegrdp=int(-$canscry1*$yfinegrd/$ymaxv+0.5);#fine-grid in pix
#  my $voffspix=int(-$canscry1*$voffs/$ymaxv+0.5);# vert,offset in pix
  my $goodc="darkgreen";
  my $badc="red";
  my $nzlines=0;
  my $curcol;
  my $colindx;
  my ($width,$dig,$ltxt,$colr,$colrl,$dasht,$titl);
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time);
  my $vcoo;
#--
  my ($vval,$runn,$fn,$fnn,$line,$slew,$slewr,$tzero,$tzeror,$gain,$gainr);
  my $val;
#
  @globpeds=();
#
#---> create scale/grid:
#
  if($plot7_actf==0){# 1st entry for plot3
    &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
#----- create Grid vertical lines + X-axes subscripts :
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
	   $colrl="black";
	 }
	 else{# fine scale line
	   $width=1;
	   $dig=$i*$yfinegrd/$yfinegrdp;
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	   $colrl="black";
	 }
         $lid=$canvas->createLine(0,-$yc,$canscrx2,-$yc, -fill=>$colrl, -width=>$width,-dash=>$dasht);
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
    $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time=$year."/".$month."/".$day."  ".$hour.":".$min;
    $titl="PaddleType's ADC(pC)==>Edep(Mev) Conversion Factor [pC/Mev] Time-evolution,  '0'-time is $time";
    $vcoo=int(120*$canscry1/$ymaxv);#place at val=120
    $lid=$canvas->createText(600,$vcoo,-text=>$titl,-font=>$font10,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl="Time-scale: red digits ->days, blue ->hours";
    $vcoo=int(5*$canscry1/$ymaxv);#place at val=5
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font10,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#
#---> Read related RefCal-file data:
#
    @RefCFileData=();
    my @CflFileMemb=();
    $fn=$cfilenames[0].".".$refcalsetn;
    $ffn=$workdir.$amsjwd."/".$fn;
    open(RFMEMB,"< $ffn") or show_warn("   <-- Cannot open $fn for reading, $!");
    while(defined ($line = <RFMEMB>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@CflFileMemb,$line);
    }
    close(RFMEMB) or show_warn("   <-- Cannot close $fn after reading, $!");
    $nel=scalar(@CflFileMemb);
    if($nel==0){
      show_warn("\n   <--- Plot-2: Ref.calibset $fn is not found, something wrong !!!");
      return;
    }
#---> read ref Tzslw-file:
    $fn=$cfilenames[4].".".$CflFileMemb[4];# Amplf-file 
    $ffn=$workdir.$amsjwd."/".$fn;
    open(RFMEMB,"< $ffn") or show_warn("   <-- Cannot open $fn for reading, $!");
    while(defined ($line = <RFMEMB>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      if($line =~/==========/){last;}# break on my internal EOF
      if($line =~/12345/){last;}# break on my internal EOF
      $nzlines+=1;
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      @words=split(/\s+/,$line);#split by any number of continious " "'s
      $nwords=scalar(@words);
      push(@RefCFileData,@words);
    }
    close(RFMEMB) or die show_warn("   <-- Cannot open $fn after reading $!");
#
    $plot1_actf=0;
    $plot2_actf=0;
    $plot3_actf=0;
    $plot5_actf=0;
    $plot6_actf=0;
  }
#---
  $plot7_actf+=1;
  $colindx=($plot7_actf % 5);
  $goodc=$goodcolorlist[$colindx];#change color according fill-entry number
#
  my $npadds=0;
  for($il=0;$il<4;$il++){$npadds+=$barsppl[$il];}# tot.number of paddles
#
  for($i=0;$i<$nelem;$i++){#<--- selected files loop
    $runn=$runs[$i];
    $sfname=$pathcalfs."/".$cfilenames[4].".".$runn;
    $nzlines=0;
    $nwords=0;
    @globpeds=();#store all data of the file
    if($processed[$i] == 1){
#
      open(INPFN,"< $sfname") or die show_warn("   <-- Cannot open $sfname for reading $!");
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
      $seqnum=2*$npadds+2*$npadds+2*3*$npadds;# to skip AnodeRelGains, An2Dyn and RelDynGain tables
      $gain=$globpeds[$seqnum+$bartype-1];
      $gainr=$RefCFileData[$seqnum+$bartype-1];
      $val=$gain;
#   print "BType==",$bartype,"  norm/normr=",$gain,"/",$gainr,"\n";
#
      $xc=($runs[$i]-$runmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
#
      if($sta==0){$curcol=$goodc;}
      else {$curcol=$badc;}
      $vval=-int($val*$canscry1/$ymaxv+0.5);# in pixels
      $lid=$canvas->createOval($xc-3,-($vval-3),$xc+3,-($vval+3), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
      if($xc>$plot7_xcmax){$plot7_xcmax=$xc;}
    }#--->endof file-loop
    $topl1->update;
  }
}
#------------------------------------------------------------------
sub view_JobLog
{
#
#--- View displayed(after its peds scan) file:
#
  if($fileidvar ne ""){
    my $stat=0;
    my $fname=$pathcalfs."/".$fileidvar;
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
sub view_CalFile
{
#
#--- View displayed(after its val scan) file:
#
  if($fileidvar1 ne ""){
    my $stat=0;
    my $fname=$pathcalfs."/".$fileidvar1;
    $stat=system("nedit -read $fname");
    if($stat != 0){
      show_warn("   <-- View_CalFile Error: fail to view file !");
      return;
    }
  }
  else{
    show_warn("   <-- View_CalFile Error: missing input file name !");
    return;
  }
}
#------------------------------------------------------------------
sub item_mark
{
  my ($canv,$id) = @_;
  my $fnam;
  my $color=$canv->itemcget($id, -fill);
  if($color eq "green"){
    print "set_bad branch\n";
    $canv->itemconfigure($id,-fill=>"red");
    $elem_num=-1;
    for($i=0;$i<$nelem;$i++){# scan IDlist to find item#
      if($plot_fileids[$i]==$id){
        $elem_num=$i;
        last;
      }
    }
    if($elem_num>=0 && $elem_num<$nelem){
      $fnam=$sfilelist[$elem_num];
      $processed[$i] = 0;# bad
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
    for($i=0;$i<$nelem;$i++){# scan IDlist to find item#
      if($idlist[$i]==$id){
        $elem_num=$i;
        last;
      }
    }
    if($elem_num>=0 && $elem_num<$nelem){
      $fnam=$sfilelist[$elem_num];
      $processed[$i] = 1;# good
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
  my @arr=();
  my @string=();
  my $runn,$fn,$nel,$f2find,$logfn,$cmd,$dir;
  $dir=$pathcalfs;
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
    $fnam=$sfilelist[$findx];
    @arr=split(/\./,$fnam);
    $nel=scalar(@arr);
    if($nel==2){
      $runn=$arr[$nel-1];#number after last "."
      $fn=$arr[0];
      if($fn eq $cfilenames[0]){#Cflist
        $cmd="ls -ogt --time-style=long-iso ".$dir."/TofTAUC*.".$runn.".log.*[0-9] |";
      }
      elsif($fn eq $cfilenames[6]){#Tdcor
        $cmd="ls -ogt --time-style=long-iso ".$dir."/TofTDCL*.".$runn.".log.*[0-9] |";
      }
      else{
        show_messg("\n   <--- Bind1Error2: wrong file name in scan-list !");
        $topl1->update;
	return;
      }
#
      @arr=();
      open(FJOBS,$cmd) or show_warn("\n   <--- Cannot make log-file list in amsjobwd  $!");
      while(<FJOBS>){
        push(@arr,$_);
      }
      close(FJOBS);
      $nel=scalar(@arr);
      if($nel==0){
        show_messg("\n   <--- Bind1Error1: there is no related log-file in $dir !");
        $topl1->update;
	return;
      }
      elsif($nel>1){show_messg("\n   <--- Multiple log-files, 1st will be used !");}
      @string=split(/\s+/,$arr[0]);
      $logfn=$string[5];#logf-name(complete path !!!)
      show_messg("\n   <--- Bind1: found $logfn !");
      @arr=split(/\//,$logfn);
      $nel=scalar(@arr);
      $fnam=$arr[$nel-1];
#
      $fileidvar=$fnam;# to show logf-name in the window
#---> calf:
      if($plot1_actf>0){$fileidvar1=$cfilenames[1].".".$runn;}
      elsif($plot2_actf>0){$fileidvar1=$cfilenames[2].".".$runn;}
      elsif($plot3_actf>0){$fileidvar1=$cfilenames[3].".".$runn;}
      elsif($plot4_actf>0 || $plot5_actf>0 || $plot6_actf>0 || $plot7_actf>0){$fileidvar1=$cfilenames[4].".".$runn;}
      elsif($plot10_actf>0){$fileidvar1=$cfilenames[6].".".$runn;}
      elsif($plot11_actf>0){$fileidvar1=$cfilenames[6].".".$runn;}
#
      $fentrw->configure(-foreground=>"blue");
      $fentrw1->configure(-foreground=>"blue");
    }
    else{
      show_messg("\n   <--- Bind1Error1: wrong file name in scan-list !");
    }
  }
  else{
    show_messg("\n   <--- Bind1Error: LineID=$id not found !");
  }
#
  $topl1->update;
}
#----------------------------------------------------------------------------
sub mark_file
{
#
#--- Mark displayed(after its scan) file as bad/good:
#
  my $fn2find;
  my @substr=();
  if($fileidvar1 ne ""){
    my $flag=0;
    @substr=split(/\./,$fileidvar1);
    if($CalType eq "TAU2DB"){
      $fn2find=$cfilenames[0].".".$substr[1];
    }
    if($CalType eq "TdcL2DB"){
      $fn2find=$fileidvar1;
    }
    for($i=0;$i<$nelem;$i++){#<--- selected files loop
      if($fn2find eq $sfilelist[$i]){
        if($processed[$i]==1){
	  $fentrw->configure(-foreground=>"red");
	  $fentrw1->configure(-foreground=>"red");
	  $processed[$i]=0;
	  show_warn("\n   <--- File(set) $fn2find marked as bad !");	
	  $flag=1;
	  last;
	}
	else{
	  $fentrw->configure(-foreground=>"blue");
	  $fentrw1->configure(-foreground=>"blue");
	  $processed[$i]=1;	
	  show_messg("\n   <--- File(set) $fn2find restored as good !","Big");	
	  $flag=2;
	  last;
	}
      }
#      $topl1->update;
    }
    if($flag==0){
      show_warn("\n   <--- Mark_file Error: file name not in the list !");
      $mwnd->bell;
    }
  }
  else{
    show_warn("\n   <--- Mark_file Error: missing file name !");
    $mwnd->bell;
    return;
  }
}
#----------------------------- for TDC Linearity ------------------------------------
sub make_plot10
{
#
#---> time-evolution of Min/Max values of Integral NonLinearity
#
  my ($xcoarsegrd,$xfinegrd)=(86400,3600);# day/hour in sec
  my ($ycoarsegrd,$yfinegrd)=(100,20);#(ps)
  my $ymaxv=500;#(ps)
#
  show_warn("");
  my $rcrat,$rslot,$rinch,$rest;
  my $crat,$slot,$inch,$cmap;
  my $icr,$isl,$ich,$ibn;
  my $binmn,$binmx,$nlmxneg,$nlmxpos;
  my @TdcLinArr=();
  my $runn;
  my $maxb;
  my $locvar;
  my @gcolors=qw(green red blue magenta lightblue black );
#
  $crtmx=4;
  $sltmx=4;
  $ichmx=6;
#
  $locvar=$chidvar;
#--> check ID :
#  if($locvar =~/(CSI)/){substr($locvar,3,5)="";}# remove "(CSI)" if present
  ($rcrat,$rslot,$rinch,$rest)=unpack("A1 A1 A1 A*",$locvar);
  if($rcrat>$crtmx || $rcrat==0 || $rslot>$sltmx || $rslot==0 || $rinch>$ichmx || $rinch==0){
    show_warn("   <-- wrong TOF Time channel ID setting: $locvar ?!");
    return;
  }
  show_messg("\n   <--- TDC max(+)/min(-) NonLin  Time-scan for crate/slot/chan=$rcrat/$rslot/$rinch");
#--
  my $seqnum;
  my ($top,$bot,$mid);
#
  my $goodc="darkgreen";
  my $badc="red";
  my $nzlines=0;
  my $curcol;
  my $colindx;
#
  @globvars=();
#-----
  my ($xc,$yc)=(0,0);
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
  if($plot10_actf==0){# 1st entry for plot2
    $cnv->configure(-background=>"yellow");
    $canvas->itemconfigure($xaxe,-fill=>"black");
    $canvas->itemconfigure($yaxe,-fill=>"black");
    &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
#------ create Grid vertical lines + X-axes subscripts :
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
    $lid=$canvas->createText(-10,10,-text=>"0",-font=>$font7,-fill=>'red');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#-- add title :
    $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time=$year."/".$month."/".$day."  ".$hour.":".$min;
    $titl="TDC Max(+)/Min(-) values of Integral NonLinearity in PicoSec vs time ('+'=>vert.oval,'-'=>hor.oval),  '0'-time is $time";
    $vcoo=int(380*$canscry1/$ymaxv);#place at val=380ps
    $lid=$canvas->createText(800,$vcoo,-text=>$titl,-font=>$font10,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl="Time-scale: red digits ->days, blue ->hours";
    $vcoo=int(10*$canscry1/$ymaxv);#place at val=10ps
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font10,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#
    $plot11_actf=0;
    $plot12_actf=0;
    $plot13_actf=0;
  }
#---
  $colindx=($plot10_actf % 6);
  $curcol=$gcolors[$colindx];#change color according fill-entry number
  $plot10_actf+=1;
#---
  for($i=0;$i<$nelem;$i++){#
    $runn=$runs[$i];
    $sfname=$pathcalfs."/".$cfilenames[6].".".$runn;
    $nzlines=0;
    $nwords=0;
    @globpeds=();#store all data of the file
    if($processed[$i] == 1){
#
      open(INPFN,"< $sfname") or die show_warn("   <-- Cannot open $sfname for reading $!");
      while(defined ($line = <INPFN>)){
        chomp $line;
        if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
        if($line =~/==========/){last;}# break on my internal EOF
        if($line =~/12345/){last;}# break on my internal EOF
	$nzlines+=1;
	$line =~s/^\s+//s;# remove " "'s at the beg.of line
	@words=split(/\s+/,$line);#split by any number of continious " "'s
	$nwords=scalar(@words);
	push(@globvars,@words);
      }
      close(INPFN) or die show_warn("   <-- Cannot open $sfname after reading $!");
#--
      $seqnum=0;
      @TdcLinArr=();
      for($icr=0;$icr<$rcrat-1;$icr++){#<-- skip all crates befor requested $rcrat
        $crat=$globvars[$seqnum];
	$seqnum+=1;
	for($isl=0;$isl<$sltmx;$isl++){
	  $slot=$globvars[$seqnum];
	  $seqnum+=1;
	  $cmap=$globvars[$seqnum];
	  $seqnum+=1;
	  for($ich=0;$ich<$ichmx;$ich++){
	    if(($cmap & (1<<$ich))==0){next;}#skip empty chan
	    $seqnum+=1024;
	  }
	}
      }
# now in ref-crate:
      $crat=$globvars[$seqnum];
      $seqnum+=1;
      for($isl=0;$isl<$rslot-1;$isl++){#<--- skip slots befor requested $rslot
        $slot=$globvars[$seqnum];
        $seqnum+=1;
	$cmap=$globvars[$seqnum];
	$seqnum+=1;
	for($ich=0;$ich<$ichmx;$ich++){
	  if(($cmap & (1<<$ich))==0){next;}#skip empty chan
	  $seqnum+=1024;
	}
      }
# now in ref slot
      $slot=$globvars[$seqnum];
      $seqnum+=1;
      $cmap=$globvars[$seqnum];
      $seqnum+=1;
      $inch=0;
      for($ich=0;$ich<$rinch-1;$ich++){#<--- skip channels befor requested $rinch
	$inch=$ich+1;
	if(($cmap & (1<<$ich))==0){next;}#skip empty chan
	$seqnum+=1024;
      }
#
      $inch+=1;# +1 to get next(requested) channel
#      print "rcrat/crat=",$rcrat,"/",$crat," rslot/slot=",$rslot,"/",$slot," rch/ch=",$rinch,"/",$inch,"\n";
      if(($crat != $rcrat) || ($slot != $rslot) || ($cmap & (1<<($rinch-1)))==0){
        show_warn("\n   <--- Requested TDC channel is empty, cr/sl/ch=$crat/$slot/$inch !");
	return;
      }      
      for($ibn=0;$ibn<1024;$ibn++){push(@TdcLinArr,$globvars[$seqnum+$ibn]);}#<-- fill requested channel bins
#--
      $binmn=0;
      $binmx=1023;
      $nlmxpos=-9999;
      $nlmxneg=9999;
      for($ibn=0;$ibn<1024;$ibn++){
        if($TdcLinArr[$ibn]>$nlmxpos){
	  $nlmxpos=$TdcLinArr[$ibn];
	  $binmx=$ibn;
	}
        if($TdcLinArr[$ibn]<$nlmxneg){
	  $nlmxneg=$TdcLinArr[$ibn];
	  $binmn=$ibn;
	}
      }
      $nlmxpos*=$TdcBin;
      $nlmxneg*=$TdcBin;
#--
      $xc=($runs[$i]-$runmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
#
#max/min.nonlin:
      $vcoo=int(-$nlmxpos*$canscry1/$ymaxv+0.5);# pos.in pix
      $lid=$canvas->createOval($xc-2,-($vcoo-4),$xc+2,-($vcoo+4), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
      $vcoo=int($nlmxneg*$canscry1/$ymaxv+0.5);
      $lid=$canvas->createOval($xc-4,-($vcoo-2),$xc+4,-($vcoo+2), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
#
      if($xc>$plot10_xcmax){$plot10_xcmax=$xc;}
    }
    $topl1->update;
  }
  $curline="   <-- Max(+)/Min(-) non linearity  scan was completed ! \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
}
#-----------------------------------------------------------------
sub make_plot11
{
#
#---> time-evolution of chan.numbers at Min/Max values of Integral NonLinearity
#
  my ($xcoarsegrd,$xfinegrd)=(86400,3600);# day/hour in sec
  my ($ycoarsegrd,$yfinegrd)=(200,50);#(bins)
  my $ymaxv=1000;#(bin)
#
  show_warn("");
  my $rcrat,$rslot,$rinch,$rest;
  my $crat,$slot,$inch,$cmap;
  my $icr,$isl,$ich,$ibn;
  my $binmn,$binmx,$nlmxneg,$nlmxpos;
  my @TdcLinArr=();
  my $runn;
  my $maxb;
  my $locvar;
  my @gcolors=qw(green red blue magenta lightblue black );
#
  $crtmx=4;
  $sltmx=4;
  $ichmx=6;
#
  $locvar=$chidvar;
#--> check ID :
#  if($locvar =~/(CSI)/){substr($locvar,3,5)="";}# remove "(CSI)" if present
  ($rcrat,$rslot,$rinch,$rest)=unpack("A1 A1 A1 A*",$locvar);
  if($rcrat>$crtmx || $rcrat==0 || $rslot>$sltmx || $rslot==0 || $rinch>$ichmx || $rinch==0){
    show_warn("   <-- wrong TOF Time channel ID setting: $locvar ?!");
    return;
  }
  show_messg("\n   <--- BinNumber at TDC NonLin max/min values Time-scan for crate/slot/chan=$rcrat/$rslot/$rinch");
#--
  my $seqnum;
  my ($top,$bot,$mid);
#
  my $goodc="darkgreen";
  my $badc="red";
  my $nzlines=0;
  my $curcol;
  my $colindx;
#
  @globvars=();
#-----
  my ($xc,$yc)=(0,0);
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
  if($plot11_actf==0){# 1st entry for plot2
    $cnv->configure(-background=>"yellow");
    $canvas->itemconfigure($xaxe,-fill=>"black");
    $canvas->itemconfigure($yaxe,-fill=>"black");
    &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
#------ create Grid vertical lines + X-axes subscripts :
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
    $lid=$canvas->createText(-10,10,-text=>"0",-font=>$font7,-fill=>'red');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#-- add title :
    $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time=$year."/".$month."/".$day."  ".$hour.":".$min;
    $titl="BinNumber at TDC Max(+)/Min(-) values of Integral NonLinearity vs time ('+'=>vert.oval,'-'=>hor.oval),  '0'-time is $time";
    $vcoo=int(950*$canscry1/$ymaxv);#place at val=950bins
    $lid=$canvas->createText(800,$vcoo,-text=>$titl,-font=>$font10,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl="Time-scale: red digits ->days, blue ->hours";
    $vcoo=int(20*$canscry1/$ymaxv);#place at val=20bins
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font10,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#
    $plot10_actf=0;
    $plot12_actf=0;
    $plot13_actf=0;
  }
#---
  $colindx=($plot11_actf % 6);
  $curcol=$gcolors[$colindx];#change color according fill-entry number
  $plot11_actf+=1;
#---
  for($i=0;$i<$nelem;$i++){#
    $runn=$runs[$i];
    $sfname=$pathcalfs."/".$cfilenames[6].".".$runn;
    $nzlines=0;
    $nwords=0;
    @globpeds=();#store all data of the file
    if($processed[$i] == 1){
#
      open(INPFN,"< $sfname") or die show_warn("   <-- Cannot open $sfname for reading $!");
      while(defined ($line = <INPFN>)){
        chomp $line;
        if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
        if($line =~/==========/){last;}# break on my internal EOF
        if($line =~/12345/){last;}# break on my internal EOF
	$nzlines+=1;
	$line =~s/^\s+//s;# remove " "'s at the beg.of line
	@words=split(/\s+/,$line);#split by any number of continious " "'s
	$nwords=scalar(@words);
	push(@globvars,@words);
      }
      close(INPFN) or die show_warn("   <-- Cannot open $sfname after reading $!");
#--
      $seqnum=0;
      @TdcLinArr=();
      for($icr=0;$icr<$rcrat-1;$icr++){#<-- skip all crates befor requested $rcrat
        $crat=$globvars[$seqnum];
	$seqnum+=1;
	for($isl=0;$isl<$sltmx;$isl++){
	  $slot=$globvars[$seqnum];
	  $seqnum+=1;
	  $cmap=$globvars[$seqnum];
	  $seqnum+=1;
	  for($ich=0;$ich<$ichmx;$ich++){
	    if(($cmap & (1<<$ich))==0){next;}#skip empty chan
	    $seqnum+=1024;
	  }
	}
      }
# now in ref-crate:
      $crat=$globvars[$seqnum];
      $seqnum+=1;
      for($isl=0;$isl<$rslot-1;$isl++){#<--- skip slots befor requested $rslot
        $slot=$globvars[$seqnum];
        $seqnum+=1;
	$cmap=$globvars[$seqnum];
	$seqnum+=1;
	for($ich=0;$ich<$ichmx;$ich++){
	  if(($cmap & (1<<$ich))==0){next;}#skip empty chan
	  $seqnum+=1024;
	}
      }
# now in ref slot
      $slot=$globvars[$seqnum];
      $seqnum+=1;
      $cmap=$globvars[$seqnum];
      $seqnum+=1;
      $inch=0;
      for($ich=0;$ich<$rinch-1;$ich++){#<--- skip channels befor requested $rinch
	$inch=$ich+1;
	if(($cmap & (1<<$ich))==0){next;}#skip empty chan
	$seqnum+=1024;
      }
#
      $inch+=1;# +1 to get next(requested) channel
#      print "rcrat/crat=",$rcrat,"/",$crat," rslot/slot=",$rslot,"/",$slot," rch/ch=",$rinch,"/",$inch,"\n";
      if(($crat != $rcrat) || ($slot != $rslot) || ($cmap & (1<<($rinch-1)))==0){
        show_warn("\n   <--- Requested TDC channel is empty, cr/sl/ch=$crat/$slot/$inch !");
	return;
      }      
      for($ibn=0;$ibn<1024;$ibn++){push(@TdcLinArr,$globvars[$seqnum+$ibn]);}#<-- fill requested channel bins
#--
      $binmn=0;
      $binmx=1023;
      $nlmxpos=-9999;
      $nlmxneg=9999;
      for($ibn=0;$ibn<1024;$ibn++){
        if($TdcLinArr[$ibn]>$nlmxpos){
	  $nlmxpos=$TdcLinArr[$ibn];
	  $binmx=$ibn;
	}
        if($TdcLinArr[$ibn]<$nlmxneg){
	  $nlmxneg=$TdcLinArr[$ibn];
	  $binmn=$ibn;
	}
      }
      $nlmxpos*=$TdcBin;
      $nlmxneg*=$TdcBin;
#--
      $xc=($runs[$i]-$runmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
#
#bin at max/min.nonlin:
      $vcoo=-int($binmx*$canscry1/$ymaxv+0.5);# pos.in pix
      $lid=$canvas->createOval($xc-2,-($vcoo-4),$xc+2,-($vcoo+4), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
      $vcoo=-int($binmn*$canscry1/$ymaxv+0.5);
      $lid=$canvas->createOval($xc-4,-($vcoo-2),$xc+4,-($vcoo+2), -fill=>$curcol, -width=>1);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
#
      if($xc>$plot11_xcmax){$plot11_xcmax=$xc;}
    }
    $topl1->update;
  }
  $curline="   <-- Max(+)/Min(-) non linearity  scan was completed ! \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
}
#-----------------------------------------------------------------
sub make_plot12
{
#
#---> single channel Integral NonLinearity
#
  my ($xcoarsegrd,$xfinegrd)=(128,16);# in bins
  my ($ycoarsegrd,$yfinegrd)=(100,20);# in ps
  my $xscale=1;# bins/pix
  my $ymaxv=500;#(ps)
  my $voffs=200;#scale vertical offset(ps)
#
  show_warn("");
  my $rcrat,$rslot,$rinch,$rest;
  my $crat,$slot,$inch,$cmap;
  my $icr,$isl,$ich,$ibn;
  my $binmn,$binmx,$nlmxneg,$nlmxpos;
  my @TdcLinArr=();
  my $runn;
  my $maxb;
  my $locvar;
  my @gcolors=qw(green red blue magenta lightblue yellow );
#
#--> read marked file:
#
  my $stat,$fname;
  my $nzlines=0;
  @globvars=();
  if($fileidvar1 ne ""){
    $stat=0;
    $fname=$pathcalfs."/".$fileidvar1;
    open(INPFN,"< $fname") or die show_warn("   <-- Cannot open $fname for reading $!");
    while(defined ($line = <INPFN>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      if($line =~/==========/){last;}# break on my internal EOF
      if($line =~/12345/){last;}# break on my internal EOF
      $nzlines+=1;
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      @words=split(/\s+/,$line);#split by any number of continious " "'s
      $nwords=scalar(@words);
      push(@globvars,@words);
    }
    close(INPFN) or die show_warn("   <-- Cannot open $sfname after reading $!");
  }
  else{
    show_warn("   <-- View_CalFile Error: missing TofTdcorRD-file name !");
    return;
  }
#
#--> read/check channel ID :
#
  $locvar=$chidvar;
#  if($locvar =~/(CSI)/){substr($locvar,3,5)="";}# remove "(CSI)" if present
  ($rcrat,$rslot,$rinch,$rest)=unpack("A1 A1 A1 A*",$locvar);
  if($rcrat>$crtmx || $rcrat==0 || $rslot>$sltmx || $rslot==0 || $rinch>$ichmx || $rinch==0){
    show_warn("   <-- wrong TOF Time channel ID setting: $locvar ?!");
    return;
  }
  show_messg("\n   <--- Integral NonLinearity plot for crate/slot/chan=$rcrat/$rslot/$rinch");
#--
  my $seqnum;
  my ($top,$bot,$mid);
#
  my $goodc="darkgreen";
  my $badc="red";
  my $nzlines=0;
  my $curcol;
  my $colindx;
#
#-----
  my ($xc,$yc)=(0,0);
  my $ypbin=int(-$canscry1/$ymaxv+0.5);#(pixels/bin)
  my $ycoarsegrdp=int(-$canscry1*$ycoarsegrd/$ymaxv+0.5);#coarse-grid in pix
  my $yfinegrdp=int(-$canscry1*$yfinegrd/$ymaxv+0.5);#fine-grid in pix
  my $voffspix=int(-$canscry1*$voffs/$ymaxv+0.5);# vert,offset in pix
#
  my $vcoo;
  my ($width,$ltxt,$dig,$colr,$vpos,$dasht,$titl);
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time);
#
#---> create scale/grid:
#
  if($plot12_actf==0){# 1st entry for plot2
    $cnv->configure(-background=>"black");
    $canvas->itemconfigure($xaxe,-fill=>"yellow");
    $canvas->itemconfigure($yaxe,-fill=>"yellow");
   &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
#------ create Grid vertical lines + X-axes subscripts :
    for($i=0;$i<$canscrx2;$i++){
       if($i>0 && ($i*$xscale-int($i*$xscale))==0 && ((int($i*$xscale) % $xfinegrd)==0 || (int($i*$xscale) % $xcoarsegrd)==0)){
         $xc=$i;
	 if(($i*$xscale % $xcoarsegrd)>0){# 128bins line
	   $width=1;
	   $dig=(($i*$xscale % $xcoarsegrd)/$xfinegrd);
	   $ltxt="$dig";
	   $colr="green";
	   $dasht='.';
	 }
	 else{# 16bins-line
	   $width=1;
	   $dig=($i*$xscale/$xcoarsegrd);
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	 }
         $lid=$canvas->createLine($xc,0,$xc,$canscry1, -fill=>'yellow', -width=>$width,-dash=>$dasht);
         $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
         $plot_accmem+=1;
         $lid=$canvas->createText($xc,+10,-text=>$ltxt,-font=>$font7,-fill=>$colr);  
         $plot_accids[$plot_accmem]=$lid;#store in glob.list
         $plot_accmem+=1;
       }
    }
#-- create Grid horizontal lines + Y-axes subscripts :
    for($i=0;$i<-$canscry1;$i++){
       if($i>0 && (($i % $yfinegrdp)==0 || ($i % $ycoarsegrdp)==0 || ($i==$voffspix))){
         $yc=$i;
	 if($i == $voffspix){# "0"-offset line
	   $width=2;
	   $dig=$i*$voffs/$voffspix-$voffs;
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='';
	   $colrl="yellow";
	 }
	 elsif(($i % $ycoarsegrdp)==0){# coarse scale line
	   $width=1;
	   $dig=$i*$ycoarsegrd/$ycoarsegrdp-$voffs;
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	   $colrl="yellow";
	 }
	 else{# fine scale line
	   $width=1;
	   $dig=$i*$yfinegrd/$yfinegrdp-$voffs;
	   $ltxt="$dig";
	   $colr="green";
	   $dasht='.';
	   $colrl="yellow";
	 }
         $lid=$canvas->createLine(0,-$yc,$canscrx2,-$yc, -fill=>$colrl, -width=>$width,-dash=>$dasht);
         $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
         $plot_accmem+=1;
         $lid=$canvas->createText(-20,-$yc,-text=>$ltxt,-font=>$font2,-fill=>$colr);  
         $plot_accids[$plot_accmem]=$lid;#store in glob.list
         $plot_accmem+=1;
       }
    }
#-- add 0 :
    $lid=$canvas->createText(-10,10,-text=>"0",-font=>$font7,-fill=>'green');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#-- add title :
    $titl="TDC Integral NonLinearity in pSec (structure period is 25ns)";
    $vcoo=int((280+$voffs)*$canscry1/$ymaxv);#place at val=280ps
    $lid=$canvas->createText(600,$vcoo,-text=>$titl,-font=>$font10,-fill=>'red');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl="Full scale is 25 ns ( 16 x 8 x 8 = 1024 bins)";
    $vcoo=int((-180+$voffs)*$canscry1/$ymaxv);#place at val=-180ps
    $lid=$canvas->createText(400,$vcoo,-text=>$titl,-font=>$font10,-fill=>'red');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#
    $plot10_actf=0;
    $plot11_actf=0;
    $plot13_actf=0;
  }
#---
  $colindx=($plot12_actf % 6);
  $curcol=$gcolors[$colindx];#change color according fill-entry number
  $plot12_actf+=1;
#---
  $seqnum=0;
  @TdcLinArr=();
  for($icr=0;$icr<$rcrat-1;$icr++){#<-- skip all crates befor requested $rcrat
    $crat=$globvars[$seqnum];
    $seqnum+=1;
    for($isl=0;$isl<$sltmx;$isl++){
      $slot=$globvars[$seqnum];
      $seqnum+=1;
      $cmap=$globvars[$seqnum];
      $seqnum+=1;
      for($ich=0;$ich<$ichmx;$ich++){
	if(($cmap & (1<<$ich))==0){next;}#skip empty chan
	$seqnum+=1024;
      }
    }
  }
# now in ref-crate:
  $crat=$globvars[$seqnum];
  $seqnum+=1;
  for($isl=0;$isl<$rslot-1;$isl++){#<--- skip slots befor requested $rslot
    $slot=$globvars[$seqnum];
    $seqnum+=1;
    $cmap=$globvars[$seqnum];
    $seqnum+=1;
    for($ich=0;$ich<$ichmx;$ich++){
      if(($cmap & (1<<$ich))==0){next;}#skip empty chan
      $seqnum+=1024;
    }
  }
# now in ref slot
  $slot=$globvars[$seqnum];
  $seqnum+=1;
  $cmap=$globvars[$seqnum];
  $seqnum+=1;
  $inch=0;
  for($ich=0;$ich<$rinch-1;$ich++){#<--- skip channels befor requested $rinch
    $inch=$ich+1;
    if(($cmap & (1<<$ich))==0){next;}#skip empty chan
    $seqnum+=1024;
  }
#
  $inch+=1;# +1 to get next(requested) channel
#  print "rcrat/crat=",$rcrat,"/",$crat," rslot/slot=",$rslot,"/",$slot," rch/ch=",$rinch,"/",$inch,"\n";
  if(($crat != $rcrat) || ($slot != $rslot) || ($cmap & (1<<($rinch-1)))==0){
    show_warn("\n   <--- Requested TDC channel is empty, cr/sl/ch=$crat/$slot/$inch !");
    return;
  }      
  for($ibn=0;$ibn<1024;$ibn++){push(@TdcLinArr,$globvars[$seqnum+$ibn]);}#<-- fill requested channel bins
#--
  my $var;
  my $xcp=0;
  my $vcoop=-int($voffs*$canscry1/$ymaxv+0.5);#pix
  for($ibn=0;$ibn<1024;$ibn++){
    $xc=($ibn+1)/$xscale;# bin coo in pix
    $var=$TdcLinArr[$ibn]*$TdcBin;
    $vcoo=-int(($var+$voffs)*$canscry1/$ymaxv+0.5);# pos.in pix
    $lid=$canvas->createLine($xcp,-$vcoop,$xc,-$vcoo, -fill=>$curcol, -width=>1);
    $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
    $plot_pntmem+=1;
#    $lid=$canvas->createOval($xc-1,-($vcoo-1),$xc+1,-($vcoo+1), -fill=>$curcol, -width=>1);
#    $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
#    $plot_pntmem+=1;
    $xcp=$xc;
    $vcoop=$vcoo;
  }
#--
  $plot12_xcmax=$xc;
  $topl1->update;
  $curline="   <-- Single channel NonLinearity curve is created ! \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
}
#------------------------------------------------------------------------------
sub upd_db
{
#
#--- call DB-writer after calib-files are selected[and checked] :
#
#
#---> check if "DBrewr"-mode:
#
  if($dryrunval==0){# real DB-update
    if($dbwrsel eq "RewrDB" && $updbtext eq "UpdDB"){
      show_warn("   ===> Your choise is to clean DB befor update, please 'Confirm' that! <===");
      $updbtext="Confirm";
      $mwnd->after(500);
      return;
    }
    elsif ($updbtext eq "Confirm"){
      $updbtext="UpdDB";
    }
  }
#
#---> check presence of selected("good"="processed") cal-files :
#
  my $ngelem=0;
  for($i=0;$i<$nelem;$i++){if($processed[$i] == 1){$ngelem+=1;}}
  if($ngelem==0){
    show_warn("   <-- Missing good ped-files !?");
    return;
  }
#
  my $jpar1,$jpar2,$jpar3,$jpar4,$jpar5,$jpar6,$jpar7,$jpar8,$jpar9;
#
#---> read map-file and "day"-subdirectories names:
#
  @daysdlist=();#list of "day"-subdirectories
  @mapfcont=();# current map-file content
  my ($sizemap,$nrecmap);
  my ($begtm,$endtm,$instm,$instmm,$updflg);
  my ($pedfn,$pedfile,$mapdir,$mapfile);
  $mapdir=$AMSDD.$DBDir;

  $mapfile=$SessName."map.dat";# to store copy of current real one
  my ($mapfn,$daysdir,$ndaysd);
  $daysdir=$AMSDD.$DBDir.$TDVName;
  $mapfn=$AMSDD.$DBDir."/.".$TDVName.".1.map";
  $len=length($TDVName);#length of TDVName
#
#--- call of DBWriter-script just to update official map-file :
#
  $dbwlfname="dbwlogf.log";#tempor DBwriter log-file (also copied into $logtext window !)
  my $dblfn=$workdir.$amsjwd."/".$dbwlfname;
  $updflg=0;#dummy(toy) run
  $begtm=1000000000;
  $endtm=1581120000;
#
# ===> !!! due to following action (dummy run) map-file will be updated(recreated if was deleted befor)
#     according to existing contents of day-subdirs(containing real calib DB-objects) <=== !!!
#
  $curline="=====> Start DBwriter-script to inspect current map-file (dummy run) \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
  $mwnd->update;
#
#---> use default RefCalSet(defined in 'Welcome' from TofRefCflistList.TAUC file) for that dummy job:
#
  show_messg("\n   <--- Use default RefCflist-file $refcalsetn in dummy run !!!");
  $FirstRefCflistN=$RefCflistList[0];
  $jpar1=$TDVName;
  $jpar2=$begtm;
  $jpar3=$endtm;
  $jpar4=$updflg;
  $jpar5=$SetupName;
  $jpar6=$cfilesloc;
  $jpar7=$refcalsetn;
  $jpar8=$MagStat;
  $jpar9=$AmsPos;
  print "scr=",$JobScriptN," jp:",$jpar1,",",$jpar2,",",$jpar3,",",$jpar4,",",$jpar5,",",$jpar6,",",$jpar7,",",$jpar8,",",$jpar9,"\n";
#
  open(DBWLOGF,"+> $dblfn") or die show_warn("   <-- Cannot open $dbwlfname (dummy run) $!");# clear if exists
  $status = system("$JobScriptN $jpar1 $jpar2 $jpar3 $jpar4 $jpar5 $jpar6 $jpar7 $jpar8 $jpar9  >> $dblfn");#<-- call DBWriter 
  if($status != 0) {die  show_warn("   <-- $JobScriptN exited badly(dummy run) !");}
  while(<DBWLOGF>){
    $curline=$_;
    $logtext->insert('end',$curline);
    $logtext->yview('end');
  }
  close(DBWLOGF) or die show_warn("   <-- Cannot close DBWLOGF (dummy run) !");
#
  show_messg("\n   <--- DBWriter finished its inspect(dummy) run !");
  $mwnd->update;
#
#--- check the presence of map-file:
#
  my $mapfound=0;
  my $patt;
  my @mapflist=();
  opendir(DIR,$mapdir);#<--- get map-dir list
  $patt=".".$TDVName.".1.map";
  @mapflist=grep{/$patt/} readdir(DIR);
  $mapfound=scalar(@mapflist);
  closedir(DIR);
  if($mapfound==1){show_messg("   <--- Map-file found !");}
  elsif($mapfound==0){show_messg("   <--- Map-file was not found - DB is empty !");}
  else{
    show_warn("   <--- Error: multiple Map-files !");
    return;
  }
  $sizemap=0;
  $nrecmap=0;
#
#---> copy current map-file (if present) to local "SessName_map.dat"-file:
#
  if($mapfound==1){
    copy($mapfn,$mapfile) or die show_warn("   <--- copy failed for $mapfn !!!");#copy  map-file
    open(MAPF,"< $mapfile") or die show_warn("   <--- Cannot open $mapfile for reading !");
    while(defined ($line = <MAPF>)){
      chomp $line;
      push(@mapfcont,$line);
    }
    close(MAPF) or die show_warn("   <-- Cannot close $mapfile after reading $!");
    $sizemap=scalar(@mapfcont);
    $nrecmap=$mapfcont[0];# records in the I(B,Bs) sections of map-file
    show_messg("   <--- Map-file $mapfn has $nrecmap records !");
    if($nrecmap>0){
    show_messg("\n   <--- BeginTime-ordered Begin/End/Insert-times list is shown below:","B");
      for($i=0;$i<$nrecmap;$i++){
        $begtm=$mapfcont[1+4*$nrecmap+$i];# Beg-sorted
	for($j=0;$j<$nrecmap;$j++){# find corresponding to beg-time end/ins-times
	  if($begtm==$mapfcont[1+2*$nrecmap+$j]){
	    $endtm=$mapfcont[1+3*$nrecmap+$j];
	    $instm=$mapfcont[1+$j];
	  }
	}
        show_messg("          Beg/End/Ins :  $begtm  $endtm  $instm");
      }
      show_messg("          ---------------------------------------------\n\n");
      $mwnd->update;
    }
  }
#---
  if($dryrunval==1){ # = Dummy Mode was selected
    $updflg=0;
#    return; 
  }
  else{
    $updflg=2;
  }
#--- 
  if($nrecmap==0 || $updflg==0){goto UPDATE;}# empty map-file or ReadOnly Job: don't cleanup DB(days dirs)
#  return;
#
#---> get list of "day"-directories:
#
  opendir(DIR,$daysdir);#<--- get day-dirs list
  @daysdlist=grep{/^\d+$/} readdir(DIR);# read only "number"-dirs
  $ndaysd=scalar(@daysdlist);
  closedir(DIR);
#
#---> delete TDV DB-records in requested dates range and remove original map-file
#
  my ($sdir,$fmt,$rest);
  my @dbrlist=();
  if($dbwrsel eq "RewrDB"){#<--- only for "clean up" mode
    show_messg("\n   <------ Preparing to DB clean-up procedure ...");
    $patt=$TDVName.".1.";
#
    for($i=0;$i<$ndaysd;$i++){#<-- loop over day-directories(subdirs of TDVName-dir)
      if((($TIMEL-$daysdlist[$i])<=86400 && ($daysdlist[$i]-$TIMEH)<=86400)
                                         || ($TIMEL > $TIMEH)){;#need day-subdirs only in range(+-1day)
        $sdir=$daysdir."/".$daysdlist[$i];#full day-subdir name
        opendir(DDIR,$sdir);#open day-subdir of days-dir
	@dbrnlist=grep{/$patt/} readdir(DDIR);# fill array of real_data ped-files names
#
	foreach $dbrn(@dbrnlist) {#<--- records(real Tofpeds.1.instm files) loop
	  $fmt="A".$len;
	  ($rest,$instm)=unpack("$fmt x3 A*",$dbrn);#get insert-time of real db-record (from his name)
#
	  for($im=0;$im<$nrecmap;$im++){#<--- map-file content loop
	    $instmm=$mapfcont[1+$im];#instime from map-file
	    if($instmm>0 && $instmm==$instm){# found map-file record, corresponding to real Tofpeds.1.instm file
	      $mapfcont[1+$im]=0;#clear map-file instm-entry (in memory)
              $begtm=$mapfcont[1+2*$nrecmap+$im];
	      if(($begtm>=$TIMEL && $begtm<=$TIMEH) || ($TIMEL > $TIMEH)){#delete real ped-file(it in time-range)
		unlink($sdir."/".$dbrn) or die show_warn("   <-- Cannot delete TDV-record $dbrn $!");
		last;#skip the rest of map-file
	      }
	    }#--->endof "found map-file record"
	  }#--->endof map-file content loop
#
	}#--->endof records(real-data ped-files) loop
	close(DDIR);#close given day-subdir
#
      }#--->endof "day-subdir in the range" check
    }#--->endof day-dirs loop
#
    unlink($mapfn) or die show_warn("   <-- Cannot delete map-file $mapfn: $!");#delete original map-file
    show_messg("   <------ DB clean up done, old map-file $mapfn deleted !!!"); 
  }#--->endof DB clean up request
#---------
# 
UPDATE:
#
  $percent=0;
  $perc_done=0;
#
  $curline="===========> Start Run $RunNum for real DB-update...\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
  $mwnd->update;
#
#---> write selected cal-files into DB:
# 
  my $cstat,$indx;
  open(DBWLOGF,"+> $dblfn") or die show_warn("   <-- Cannot open $dbwlfname  !");# clear file if exists
  for($i=0;$i<$nelem;$i++){#<--- selected files loop
    if($processed[$i] == 1){#<-- in time-selection window
      system("date >> $dblfn");
      $cstat=0;
      $indx=$i;
      $cstat=CpyCalf2jwd($indx);#<-- create generic cal-file(set) in amsjobwd
      if($cstat!=0){
        show_warn("\n   <--- Problem to find cal-file(set) in store, skip it !!!");
	next;
      }
      $begtm=$runs[$i];
      $endtm=0;
      if(($dbwrsel eq "RewrDB") || ($nrecmap == 0) ){#<--- "DB clean up" mode
        $endtm=1581120000;# till year 2020
	show_messg("   <------ RewriteDB: cal-file(set) $sfilelist[$i],  begtm=$begtm, endtm=$endtm");
      }
      else{#<-- "insert"-mode
        if($begtm < $mapfcont[1+4*$nrecmap]){
          $endtm = $mapfcont[1+4*$nrecmap];
        }
        elsif($begtm >= $mapfcont[5*$nrecmap]){
          $endtm=1581120000;# till year 2020
        }
        else {
          for($im=0;$im<$nrecmap;$im++){# loop over 5th group(ordered "begins")
            if(($begtm >= $mapfcont[1+4*$nrecmap+$im]) && ($begtm < $mapfcont[1+4*$nrecmap+$im+1])){
	      $endtm=$mapfcont[1+4*$nrecmap+$im+1]-1;
	      last;
	    }
	  }
	  show_messg("   <------ InsertDB: cal-file(set) $sfilelist[$i],  begtm=$begtm, endtm=$endtm");
        }
      }#-->endof "insert"-mode
#
      $jpar1=$TDVName;
      $jpar2=$begtm;
      $jpar3=$endtm;
      $jpar4=$updflg;
      $jpar5=$SetupName;
      $jpar6=$cfilesloc;
#      $jpar7=$cfilenames[0].".111";
      $jpar7="111";
      $jpar8=$MagStat;
      $jpar9=$AmsPos;
      $status=0;
# print "scr=",$JobScriptN," jp:",$jpar1,",",$jpar2,",",$jpar3,",",$jpar4,",",$jpar5,",",$jpar6,",",$jpar7,",",$jpar8,",",$jpar9,"\n";
      $status = system("$JobScriptN $jpar1 $jpar2 $jpar3 $jpar4 $jpar5 $jpar6 $jpar7 $jpar8 $jpar9 >> $dblfn");#<-- call DBWriter
      while(<DBWLOGF>){#<-- put on screen job-log
        $curline=$_;
        $logtext->insert('end',$curline);
        $logtext->yview('end');
      }
      if($status != 0){
        show_warn("\n   <--- DB-update problems for file(set) $sfilelist[$i] !!!");
	print "Run failed :",$sfilelist[$i],"\n";
      }
      else{
        show_messg("\n   <--- DB-update is successful for file(set) $sfilelist[$i] !!!");
        $processed[$i]=2;#<-- written to DB
	print "Run completed :",$sfilelist[$i],"\n";
      }
#
    }#--->endof "selected-file" check
    $percent+=1;
    $perc_done=100*($percent/$nelem);
#
    $mwnd->update;
  }#--->endof ped-files loop
  close(DBWLOGF) || die show_warn("   <-- Cannot close DBWLOGF !");
}
#------------------------------------------------------------------------------
sub CpyCalf2jwd#<-- copy needed cal-file(s) from store to amsjobwd
{
  my $indx=$_[0];
  my $sta=1;#bad
  my $im;
#------
  if(($indx < 0) || $indx >= $nelem){
    show_warn("\n   <--- Calling CpyCalf2jwd with wrong index $indx !!!");
    return $sta;
  }
#------
  my @CflFileMemb=();
  my $line,$fn,$ffn,$nel,$run,$mrun,$mname,$mfn,$fnfr,$fnto,$rwsta,
  $run=$runs[$indx];# cflist/Tdcor runn
#------
  if($CalType eq "TAU2DB"){
#
#--> open relevant Cflist-file:
#
    $fn=$sfilelist[$indx];
    $ffn=$pathcalfs."/".$fn;
    open(CFMEMB,"< $ffn") or show_warn("   <-- Cannot open $fn for reading, $!");
    while(defined ($line = <CFMEMB>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@CflFileMemb,$line);
    }
    close(CFMEMB) or show_warn("   <-- Cannot close $fn after reading, $!");
    $nel=scalar(@CflFileMemb);
    if($nel==0){
      show_warn("\n   <--- Calib-file $fn is not found in store, something wrong !!!");
      return $sta;
    }
#
    for($im=1;$im<$nel-2;$im++){#cal-types loop(starting from TofCStatRD-file)
      $mrun=$CflFileMemb[$im];
      $mname=$cfilenames[$im];
      if($mrun == $run){#<-- means this file is in store (real cal-file, not default one)
        $CflFileMemb[$im]=111;#<-- indication for Jscript to read temp.copy  
        $mfn=$mname.".".$mrun;
        $fnfr=$pathcalfs."/".$mfn;
        $fnto=$workdir.$amsjwd."/".$mname.".111";
        $rwsta = system("cp -f $fnfr $fnto");#<--- make temp.copy in amsjobwd
        if($rwsta != 0){
	  show_warn("\n   <-- Can't copy $mfn from store to amsjobwd !");
	  return $sta;
	}
	else{show_messg("\n   <--- Cal-file $mfn was copied to amsjobwd as generic file !");}
      }
    }
  }
#-------
  elsif($CalType eq "TdcL2DB"){
#
#--> open ref(default) Cflist-file:
#
    @CflFileMemb=();
    $fn=$cfilenames[0].".".$refcalsetn;
    $ffn=$workdir.$amsjwd."/".$fn;
    open(RFMEMB,"< $ffn") or show_warn("   <-- Cannot open $fn for reading, $!");
    while(defined ($line = <RFMEMB>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      push(@CflFileMemb,$line);
    }
    close(RFMEMB) or show_warn("   <-- Cannot close $fn after reading, $!");
    $nel=scalar(@CflFileMemb);
    if($nel==0){
      show_warn("\n   <--- Ref.calibset $fn is not found, something wrong !!!");
      return $sta;
    }
#
    $CflFileMemb[6]=111;#<-- indication for Jscript to read temp.copy  
    $mfn=$sfilelist[$indx];
    $mname=$cfilenames[6];# Tdcor
    $fnfr=$pathcalfs."/".$mfn;
    $fnto=$workdir.$amsjwd."/".$mname.".111";
    $rwsta = system("cp -f $fnfr $fnto");
    if($rwsta != 0){
      show_warn("\n   <-- Can't copy $mfn from store to amsjobwd !");
      return $sta;
    }
    else{show_messg("   <--- Cal-file $mfn was copied to amsjobwd as temporary file !");}
  }
#
#---> now create tempor Cflist-file physically in amsjobwd-dir:
#
  $WarnFlg=0;
  $fn=$cfilenames[0].".111";
  $ffn=$workdir.$amsjwd."/".$fn;
  $nel=scalar(@CflFileMemb);
  open(CFLIST,"+> $ffn") or show_warn("   <-- Cannot open tempor.file $fn for writing, $!");
  for($im=0;$im<$nel;$im++){
    $line=$CflFileMemb[$im];
    print CFLIST $line."\n";
  }
  close(CFLIST) or show_warn("   <-- Cannot close tempor.file $fn after writing, $!");
  if($WarnFlg==0){
    show_messg("   <--- Temporary cal-files set $fn was successfully created !!!");
    $sta=0;
    return $sta;
  }
  else{return $sta;}
#------ 
}
#------------------------------------------------------------------------------
sub quits_actions
{
  my ($curfn,$newfn,$curfnl,$newfnl,$run,$shsn,$fpath);
  my $status,$line,$rwsta;
#
#---> save processed Tdcor/CalSets info into "last session" hist-file:
#
#if($dryrunval==0){
  my $histfn="TofDBupdHist.".$SessName;
  $fpath=$workdir."/".$histfn;
  $WarnFlg=0;
  open(HISTF,"> $histfn") or show_warn("\n   <--- Cannot open $fhistn for writing, $!");
  if($WarnFlg==0){
    for($i=0;$i<$nelem;$i++){
      $run=$runs[$i];
      $status=$processed[$i];
      if($status>=1){
        $line=$run."    ".$status;
        print HISTF $line."\n";
      }
    }
    close(HISTF) or show_warn("   <--- Cannot close $histfn after writing, $!");
    show_messg("\n   <---  History file $histfn is updated !");
#    $status=system("chmod 666 $histfn");
#    if($status != 0){
#      show_warn("\n   <--- Cannot change write privilege for $histfn, $!");
#    }
  }
  else{
    show_warn("\n   <--- Cannot create history file $histfn !!!, $!");
  }
#}
#
#---> archive(from storage) processed TofTdcor/TofCflist-files and log-files (if requested):
#
  $shsn="";
  $shsn=substr($SessName,0,4);# extract TAUC(TDCL) from full sess.name TAUCU(TDCLU)
  if($archval==1 && $arcscval==0 && $dryrunval==0){# moving(if requested) only for Real mode of DB-update
    show_messg("\n   <--- Archiving of processed cal-files and related logs...");
    for($i=0;$i<$nelem;$i++){#<--- processed files loop 
      if($processed[$i] >= 1){
        $run=$runs[$i];
        $curfn=$pathcalfs."/".$sfilelist[$i];# really imply TofTdcor-files or TofCflist-files only
        $newfn=$patharch."/".$sfilelist[$i];
        $curfnl=$pathcalfs."/Tof".$shsn."*.".$run.".log*.*";
        $newfnl=$patharch;
        move($curfn,$newfn) or show_warn("   <-- Archiving failed for cal-file $curfn, $!");# move calf to archive
#        $rwsta = system("mv $curfnl $newfnl");
#        if($rwsta != 0){show_warn("   <-- Archiving failed for log-file $curfnl, $!");}
#        else{show_messg("\n   <--- CalibLogFile $curfnl was moved to archive !");}
#        move($curfnl,$newfnl) or show_warn("   <-- Archiving failed for log-file $curfnl, $!");#...logs ...
      }
    }
    show_messg("\n   <--- Archiving is completed !");
  }
#
#---> save log-screen to log-file and move it to archive:
#
  my $logfname="DBwr".$SessName."sessLog.";
  $filen=$logfname.$SessTLabel;#extention is session time label 
  $newfn=$patharch."/".$filen;
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
  $mwnd->after(10000);
  $dir_fram->destroy() if Exists($dir_fram);
  $set_fram->destroy() if Exists($set_fram);
  $jcl_fram->destroy() if Exists($jcl_fram);
  $prg_fram->destroy() if Exists($prg_fram);  
  $topl1->destroy() if Exists($topl1);
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
#--------
1;



