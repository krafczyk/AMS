sub Ped2DBWriter
{
#---
my $dirfrheight=0.35;
my $setfrheight=0.52;
my $prgfrheight=0.08;
my $jclfrheight=0.05;
#---> imp.glob.const, vars and arrays:
$outlogf="out.log";
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
                                                      -relwidth=>0.3, -relheight=>$dirfrheight,
                                                      -relx=>0, -rely=>0);
#---
$amsd_lab=$dir_fram->Label(-text=>"HeadD:",-font=>$font2)->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>$shf1);
$amsd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$workdir)->place(
                                                                   -relwidth=>0.8, -relheight=>$drh1,  
                                                                   -relx=>0.2, -rely=>$shf1);
#---
$scd_lab=$dir_fram->Label(-text=>"PedfD:",-font=>$font2,-relief=>'groove')
                                                ->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+$drh1));
$scnd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$font1,
                                              -textvariable=>\$pedsdir)->place(
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
#---
$amsg_lab=$dir_fram->Label(-text=>"AmsDD:",-font=>$font2)->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+2*$drh1));
$amsg_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$AMSDD)->place(
                                                                   -relwidth=>0.8, -relheight=>$drh1,  
                                                                   -relx=>0.2, -rely=>($shf1+2*$drh1));
#---
$file_lab=$dir_fram->Label(-text=>"DaqF:",-font=>$font2)->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+3*$drh1));
$files="All";#to process particular file (default->ALL) 
$amsg_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$files)->place(
                                                                   -relwidth=>0.8, -relheight=>$drh1,  
                                                                   -relx=>0.2, -rely=>($shf1+3*$drh1));
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
                                         -relwidth=>1, -relheight=>$drh1,  
                                         -relx=>0, -rely=>($shf1+4*$drh1));
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
$set_fram=$mwnd->Frame(-label=>"Job Conditions :", 
                                                      -relief=>'groove', -borderwidth=>5)->place(
                                                      -relwidth=>0.3, -relheight=>$setfrheight,
                                                      -relx=>0, -rely=>$dirfrheight);
#---
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
						      -relwidth=>0.5, -relheight=>$drh2,
						      -relx=>0, -rely=>$shf2);
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
						       -relwidth=>0.5, -relheight=>$drh2,
						        -relx=>0.5, -rely=>$shf2);
#------
$set_fram->Label(-text=>"Push if limit changed >>>",-font=>$font2,-relief=>'groove')
                                               ->place(
					       -relwidth=>0.5, -relheight=>$drh2,
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
$boardpeds=1;#on
$brd_cbt=$set_fram->Radiobutton(-text=>"OnBoardPeds", -font=>$font2, -indicator=>0,
                                                 -borderwidth=>5,-relief=>'raised',
						 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green,
						 -value=>1,
                                                 -variable=>\$boardpeds)
					 ->place(
                                                 -relwidth=>0.5, -relheight=>$drh2,
						 -relx=>0, -rely=>($shf2+4*$drh2));
#---
$ofl_cbt=$set_fram->Radiobutton(-text=>"Data(RawFMT)Peds", -font=>$font2, -indicator=>0,
                                                 -borderwidth=>5,-relief=>'raised',
                                                 -selectcolor=>orange,-activeforeground=>red,
						 -activebackground=>yellow, 
			                         -cursor=>hand2,
                                                 -background=>green, 
						 -value=>0,
                                                 -variable=>\$boardpeds)
					 ->place(
                                                 -relwidth=>0.5, -relheight=>$drh2,
						 -relx=>0.5, -rely=>($shf2+4*$drh2));
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
                                                 -relwidth=>0.5, -relheight=>$drh2,
						 -relx=>0, -rely=>($shf2+5*$drh2));
$arcscbt->bind("<Button-3>", \&arcscbt_help);
#----
$archval=0;#0/1->not/archive processed ped-files
$archbt=$set_fram->Checkbutton(-text=>"ArchiveOnQuit",  -font=>$font2, -indicator=>0, 
                                          -borderwidth=>5,-relief=>'raised',
                                          -selectcolor=>orange,-activeforeground=>red,
				          -activebackground=>yellow, 
			                  -cursor=>hand2,
                                          -background=>green,
                                          -variable=>\$archval)
			          ->place(
				          -relwidth=>0.5,-relheight=>$drh2,      
                                          -relx=>0.5,-rely=>($shf2+5*$drh2));
$archbt->bind("<Button-3>", \&archbt_help);
#---
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
#---
$soundtext="Sound-ON";
$set_fram->Button(-text=>"Sound-ON", -font=>$font2, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
			      -foreground=>"red",
			      -background=>"green",
                              -borderwidth=>5,-relief=>'raised',
			      -cursor=>hand2,
			      -textvariable=>\$soundtext,
                              -command => sub{if($soundtext eq "Sound-ON"){$soundtext="Sound-OFF";}
			                      else {$soundtext="Sound-ON";}})
			         ->place(
                                                 -relwidth=>0.5, -relheight=>$drh2,
						 -relx=>0, -rely=>($shf2+7*$drh2));
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
				          -relwidth=>0.5,-relheight=>$drh2,      
                                          -relx=>0.5,-rely=>($shf2+7*$drh2));
$dryrbt->bind("<Button-3>", \&dryrbt_help);
#---
# $bellb=$set_fram->Button(-text => 'bell', -command =>[\&mybeep, 3, 3])
#			          ->place(
#				          -relwidth=>0.5,-relheight=>$drh2,      
#                                          -relx=>0.5,-rely=>($shf2+6*$drh2));
#--------------
#frame_progr:
$prg_fram=$mwnd->Frame(-label=>"DB-update progress :", -relief=>'groove', -borderwidth=>5)->place(
                                                      -relwidth=>0.3, -relheight=>$prgfrheight,
                                                      -relx=>0, -rely=>($dirfrheight+$setfrheight));
$perc_done=0.;
$prg_but=$prg_fram->ProgressBar( -width=>10, -from=>0, -to=>100, -blocks=>100,
                                 -colors=>[0,'green'], -gap=> 0,
                                -variable=>\$perc_done)->place(-relwidth=>0.99, -relheight=>0.5,
                                                             -relx=>0, -rely=>0.45);
#--------------
#job_control frame:
$jcl_fram=$mwnd->Frame(-label=>"Ped2DB-Job control :",-background => "red",
                                                       -relief=>'groove', -borderwidth=>3)
						       ->place(
                                                       -relwidth=>0.3, -relheight=>$jclfrheight,
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
$checkbt=$jcl_fram->Button(-text => "CheckQ", -font=>$font2, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
			      -background=>"green",
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
#			      -state=>'disabled',
			      -cursor=>hand2,
			      -textvariable=>\$updbtext,
                              -command => \&upd_db)
			         ->place(
                                     -relx=>0.5,-rely=>0,
                                     -relwidth=>0.25,-relheight=>1);
$upddbbt->bind("<Button-3>", \&upddbbt_help);
#---
$exitbt=$jcl_fram->Button(-text => "QuitSess", -font=>$font2,
                               -activebackground=>"yellow",
			       -activeforeground=>"red",
			       -background=>"green",
			       -cursor=>hand2,
                               -command => \&quits_actions)->place(
                                         -relx=>0.75,-rely=>0,
                                         -relwidth=>0.25,-relheight=>1);
$exitbt->bind("<ButtonRelease-3>", \&exitbt_help);
}
#------------------------------------------------------
sub open_help_window
{
  my $helpfile="TofAccPedDBWriterHelp.txt";
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
sub scanbt_help{
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
  my $indx=$helptext->search(-backwards, "UpdDB_DryRun-Button:",'end');
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
sub exitbt_help{
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
  my $indx=$helptext->search(-backwards, "Exit-Button:",'end');
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
  my $indx=$helptext->search(-backwards, "DataLengthTimeScanPlot-Button:",'end');
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
#------------------------------------------------------
sub PEDC2DB_Welcome
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
  $line="       ----- WELCOME for updating of TOF/ACC Pedestals in DB !!! -----\n\n";
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
  show_messg("\n   <--- Please login otherwise only 'DB ReadOnly' mode is allowed !!!","Big");
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
sub scand{ # scan ped-directory for needed ped-files in required date-window
#(imply: date<=>runn conversion is done by prior clicking of "RunNumber/RunDate" button !!!)
  $RunNum+=1;
  $curline="\n =====> New Scan $RunNum of ped-files storage is started...\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
  $message="";
#
  $updbtext="UpdDB";
#
  $pathpeds=$workdir.$pedsdir;#complete path to tof/anti peds_files_store directory
  $patharch=$workdir.$pedsdir.$archdir;#complete path to tof/anti peds_files_archive-directory
  if($arcscval==1){#want to scan archive instead of normal store $pedsdir
    $pathpeds=$patharch;
    $curline="   <-- Scaning Archive !!!\n\n";
    $logtext->insert('end',$curline);
    $logtext->yview('end');
  }
#
  if($tofantsel eq "useTOF"){
    $detpat="tofp_";
  }
  else{
    $detpat="antp_";
  }
#
  $patt_tb="\^$detpat"."tb_rl."."\\d+\$";
  $patt_ds="\^$detpat"."ds_rl."."\\d+\$";
  my $fspatt;
#--- scan directory to create ped-file's list :
  @filelist=();
  opendir(DIR,$pathpeds);
  if($boardpeds==1){
    $fspatt=$detpat."tb_";
    @filelist=grep{/$patt_tb/} readdir(DIR);
  }
  elsif($boardpeds==0){
    $fspatt=$detpat."ds_";
    @filelist=grep{/$patt_ds/} readdir(DIR);
  }
  else{
    $message="Undefined ped-type (onboard|offline) !";
    return;
  }
  closedir(DIR);
#
  $nelem=scalar(@filelist);
  if($nelem == 0){
    show_warn("\n   <--- PedFilesStore directory doesn't contain files of type '$fspatt' ?");  
    show_warn("   <--- Check selections and repeat scan !!!");  
    return;
  }
#--- sort list in the increasing order of run-number (it is unix-time of calibration):
  @processed = ();#clear list-pattern of processed files
  @runs = ();
  @detnam = ();
  @pedtyp = ();
  @sfilelist= ();#clear sorted list
#
  @sfilelist = sort{unpack("x11 A*",$a) <=> unpack("x11 A*",$b)} @filelist;# sort by run>
#
#--- make necessary selection of files in the ordered list(make pattern of good ones):
#
  my ($yy1,$mm1,$dd1,$hh1,$mn1);
  my ($yy2,$mm2,$dd2,$hh2,$mn2);
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time);
  $ss1=0;
  ($yy1,$mm1,$dd1,$hh1,$mn1)=unpack("A4 x1 A2 x1 A2 x1 A2 x1 A2",$fdat1);#unpack dates low limit
  $mm1=$mm1;
  $TIMEL=timelocal($ss1,$mn1,$hh1,$dd1,$mm1-1,$yy1-1900);# UTC(GMT) in seconds (imply local time as input)
#   
  $ss2=0;
  ($yy2,$mm2,$dd2,$hh2,$mn2)=unpack("A4 x1 A2 x1 A2 x1 A2 x1 A2",$fdat2);#unpack dates high limit
  $mm2=$mm2;
  $TIMEH=timelocal($ss2,$mn2,$hh2,$dd2,$mm2-1,$yy2-1900);
#   
  $nselem=0;
  $curline="   <--- Found the following files in dates(runs) window :\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
#
  for($i=0;$i<$nelem;$i++){#<--- loop over list to select files in dates window
    ($detnam[$i],$pedtyp[$i],$runs[$i])=unpack("A4 x1 A2 x4 A*",$sfilelist[$i]);
#    print $detnam[$i]," ",$pedtyp[$i]," ",$runs[$i]," ",$processed[$i],"\n";
    $ptime=localtime($runs[$i]);#local time (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
#
    $time=$year."/".$month."/".$day." ".$hour.":".$min.":".$sec;
#    $time="yyyy/mm/dd=".$year."/".$month."/".$day." hh:mm:ss=".$hour.":".$min.":".$sec;
    if($debug==1){
#      printf("Date: %02d:%02d:%02d-%04d/%02d/%02d\n",$hour,$min,$sec,$year,$month,$day);
      $curline=$detnam[$i]." ".$pedtyp[$i]." ".$runs[$i]." ".$processed[$i]."  PedFile time(local): ".$time."\n\n";
      $logtext->insert('end',$curline);
      $logtext->yview('end');
      $mwnd->update;
    }
#
    if($processed[$i] == 0){
      if($files != "All"){ # select single file
        if($files == $sfilelist[$i]){
          $nselem+=1;
          $processed[$i]=1;
          $curline="      file $sfilelist[$i] is selected, date: ".$time."\n";
          $logtext->insert('end',$curline);
          $logtext->yview('end');
	}
      }
      else{ #select all files in required dates range
        if(($runs[$i]>=$TIMEL && $runs[$i]<$TIMEH) || ($TIMEL > $TIMEH)){
          $nselem+=1;
          $processed[$i]=1;
	  if($nselem==1){
	    show_messg("\n   <--- List of ped-files in search window:","B");
	  }
          $curline="      file $sfilelist[$i] is selected, date: ".$time."\n";
          $logtext->insert('end',$curline);
          $logtext->yview('end');
        }
      }
#
    }
  }#---> endof files loop
  if($nselem == 0){
    show_warn("\n   <--- None of files were selected - change date window and repeat scan !!!");
    return;
  }
  else{
    show_messg("\n   <--- Found $nselem cal-files in dates window:");
  }
#--- find min/max run# in the dates window :
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
#--> redefine some run-dependent params(here SetupName):
#
  if($runmx<1230764399){#--->2008
    $SetupN="AMS02PreAss";
  }
  elsif($runmx>1230764399){#--->2009
    $SetupN="AMS02Ass1";
  }
  elsif($runmn>1262300400 && $runmx<1284069601){#--->2010 < 10sept
    $SetupN="AMS02Ass1";
  } 
  elsif{$runmx>1284069601){#--->2010 > 10sept
    $SetupN="AMS02Space";
  }
  else{
    show_warn("\n   <-- Wrong dates(runs) range definition, please correct it !!!");
    return;
  }
#---
#
#--- create standard list-file with selected file-names:
#
#  $sfname="FilesList.P2DB";
#  my $fn=$workdir."/".$sfname;
#  open(OUTFN,"> $fn") or die show_warn("   <-- Cannot open $sfname $!");
#  for($i=0;$i<$nelem;$i++){#<--- selected files --> list-file
#    if($processed[$i] == 1){
#      print OUTFN $sfilelist[$i],"\n";#write to file 
#    }
#  }
#  close(OUTFN);
#
#  $curline="   <-- File-list is saved in $sfname\n\n";
#  $logtext->insert('end',$curline);
#  $logtext->yview('end');
#
  $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
  $year=$ptime->year+1900;
  $month=$ptime->mon+1;
  $day=$ptime->mday;
  $hour=$ptime->hour;
  $min=$ptime->min;
  $sec=$ptime->sec;
  $time=$year."/".$month."/".$day." ".$hour.":".$min.":".$sec;
  $curline="        Date(local) of the earliest run in the window : ".$time."\n";
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
  $curline="        Date(local) of the   latest run in the window : ".$time."\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
#
  $curline="  **** If you need to modify dates window - do that now and repeate scan !!! ****\n\n";
  $logtext->insert('end',$curline,'Attention');
  $logtext->yview('end');
  $mwnd->update;
}
#----------------------------------------------------------------
sub checkq{# create TopLevel with canvas and control panel
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
                                                        -command=>sub{$topl1->destroy();})->place(
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
  $canscry1=-500;
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
    $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
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
  for($i=0;$i<$nelem;$i++){#
    $sfname=$pathpeds."/".$sfilelist[$i];
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
      $xc=($runs[$i]-$runmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
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
    my $fname=$pathpeds."/".$fileidvar.".log";
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
    $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
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
  for($i=0;$i<$nelem;$i++){#<--- selected files loop
    $sfname=$pathpeds."/".$sfilelist[$i];
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
        show_warn("   <-- Channel $locvar is empty for run $runs[$i] !");
      }
      $xc=($runs[$i]-$runmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
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
    for($i=0;$i<$nelem;$i++){#<--- selected files loop
      if($fileidvar eq $sfilelist[$i]){
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
  $sfname=$pathpeds."/".$fileidvar;
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
    $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
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
  for($i=0;$i<$nelem;$i++){#<--- selected files loop
    $sfname=$pathpeds."/".$sfilelist[$i];
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
        show_warn("   <-- Channel $locvar is empty for run $runs[$i] !");
      }
      $xc=($runs[$i]-$runmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
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
#--- Mark displayed(after its peds scan) file as bad/good:
#
  if($fileidvar ne ""){
    my $flag=0;
    for($i=0;$i<$nelem;$i++){#<--- selected files loop
      if($fileidvar eq $sfilelist[$i]){
        if($processed[$i]==1){
          $file_remove_btext="^^^ Mark above file as good ^^^";
	  $fentrw->configure(-foreground=>"red");
	  $processed[$i]=0;	
          $curline="    <--File $fileidvar marked as bad !\n\n";
          $logtext->insert('end',$curline);
          $logtext->yview('end');
	  $flag=1;
	  last;
	}
	else{
          $file_remove_btext="^^^ Mark above file as bad ^^^";
	  $fentrw->configure(-foreground=>"blue");
	  $processed[$i]=1;	
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
sub upd_db
{
#
#--- call DB-writer after ped_table files are selected[and checked] :
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
#---> check presence of selected("good"="processed") ped-files :
#
  my $ngelem=0;
  for($i=0;$i<$nelem;$i++){if($processed[$i] == 1){$ngelem+=1;}}
  if($ngelem==0){
    show_warn("   <-- Missing good ped-files !?");
    return;
  }
#
#---> read map-file and "day"-subdirectories names:
#
  @daysdlist=();#list of "day"-subdirectories
  @mapfcont=();# current map-file content
  my ($sizemap,$nrecmap);
  my ($begtm,$endtm,$tdvname,$instm,$instmm,$updflg);
  my ($pedfn,$pedfile,$mapdir,$mapfile);
  $mapdir=$AMSDD."/DataBase";

  $mapfile=$SessName."map.dat";# to store copy of current real one
  my ($mapfn,$daysdir,$ndaysd);
  if($tofantsel eq "useTOF"){
    $daysdir=$AMSDD."/DataBase/Tofpeds";
    $mapfn=$AMSDD."/DataBase/.Tofpeds.1.map";
    $tdvname="Tofpeds";
    $len=7;#length of tdvname
  }
  else {
    $daysdir=$AMSDD."/DataBase/Antipeds";
    $mapfn=$AMSDD."/DataBase/.Antipeds.1.map";
    $tdvname="Antipeds";
    $len=8;
  }
#
#--- call of DBWriter-script just to update official map-file :
#
  $dbwlfname="dbwlogf.log";#tempor DBwriter log-file (also copied into $logtext window !)
  my $dblfn=$workdir.$amsjwd."/".$dbwlfname;
  $updflg=0;#dummy(dry) run
  $begtm=1000000000;
  $endtm=1581120000;
#
# ===> !!! due to following action (dummy run) map-file will be updated(recreated if was deleted befor)
#     according to existing contents of day-subdirs(containing real ped-files DB-objects) <=== !!!
#
  $curline="=====> Start DBwriter-script to inspect current map-file (dummy run) \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
  $mwnd->update;
# make def.ped-file from the 1st existing one (i need it now pure formally for offline-program)
  $pedfn=$pathpeds."/".$sfilelist[0];
  $pedfile=$workdir.$amsjwd."/".$detnam[0]."_df_rl.dat";#default input ped-file name required by offline-program
  copy($pedfn,$pedfile) or die show_warn("   <-- Pedf-copy to amsjobwd failed for $pedfn (dummy run)!");#copy 
#
  open(DBWLOGF,"+> $dblfn") or die show_warn("   <-- Cannot open $dbwlfname (dummy run) $!");# clear if exists
  $status = system("$JobScriptN $tdvname $begtm $endtm $updflg $SetupN >> $dblfn");#<-- call DBWriter 
  if($status != 0) {die  show_warn("   <-- $JobScriptN exited badly(dummy run) !");}
  while(<DBWLOGF>){
    $curline=$_;
    $logtext->insert('end',$curline);
    $logtext->yview('end');
  }
  close(DBWLOGF) or die show_warn("   <-- Cannot close DBWLOGF (dummy run) !");
#
  $curline="<===== Finished DBWriter inspect(dummy) run !\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
  $mwnd->update;
#
#--- check the presence of map-file:
#
  my $mapfound=0;
  my $patt;
  my @mapflist=();
  opendir(DIR,$mapdir);#<--- get map-dir list
  $patt=".".$tdvname.".1.map";
  @mapflist=grep{/$patt/} readdir(DIR);
  $mapfound=scalar(@mapflist);
  closedir(DIR);
  if($mapfound==1){show_messg("   <-- Map-file found !");}
  elsif($mapfound==0){show_messg("   <-- Map-file was not found - DB is empty !");}
  else{
    show_warn("   <-- Error: multiple Map-files !");
    return;
  }
  $sizemap=0;
  $nrecmap=0;
#
#---> copy current official map-file (if present) to local one:
#
  if($mapfound==1){
    copy($mapfn,$mapfile) or die show_warn("   <-- copy failed for $mapfn !!!");#copy  ped map-file
    open(MAPF,"< $mapfile") or die show_warn("   <-- Cannot open $mapfile for reading !");
    while(defined ($line = <MAPF>)){
      chomp $line;
      push(@mapfcont,$line);
    }
    close(MAPF) or die show_warn("   <-- Cannot close $mapfile after reading $!");
    $sizemap=scalar(@mapfcont);
    $nrecmap=$mapfcont[0];# records in the I(B,Bs) sections of map-file
    show_messg("   <-- Map-file $mapfn has $nrecmap records !");
    if($nrecmap>0){
    show_messg("       BeginTime-ordered Begin/End/Insert-times list is followed:\n");
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
      show_messg("---------------------------------------------\n\n");
      $mwnd->update;
    }
  }
  if($dryrunval==1){return;} # = was dry run
  if($nrecmap==0){goto UPDATE;}# empty(def) map-file is equivalent to "DB clean up" mode
#
#---> get list of "day"-directories:
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
    show_messg("   <------ Preparing to DB clean-up procedure ...");
    $patt=$tdvname.".1.";
#
    for($i=0;$i<$ndaysd;$i++){#<-- loop over day-directories(subdirs of TDVname-dir)
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
#---> write selected ped-files into DB:
# (all run-logs will be temporary written in common log-file "dbwlogf.log" in /amsjobwd)
# 
  open(DBWLOGF,"+> $dblfn") or die show_warn("   <-- Cannot open $dbwlfname  !");# clear file if exists
  for($i=0;$i<$nelem;$i++){#<--- selected files loop 
    if($processed[$i] == 1){
      system("date >> $dblfn");
      $pedfn=$pathpeds."/".$sfilelist[$i];
      $pedfile=$workdir.$amsjwd."/".$detnam[$i]."_df_rl.dat";#default input ped-file name required by offline-program
      copy($pedfn,$pedfile) or die show_warn("   <-- Copy failed for $pedfn $!");#copy peds into "***p_df_rl.dat"
      $begtm=$runs[$i];
      $endtm=0;
      if(($dbwrsel eq "RewrDB") || ($nrecmap == 0) ){#<--- "DB clean up" mode
        $endtm=1581120000;# till year 2020
	show_messg("   <------ RewriteDB: ped-file $sfilelist[$i],  begtm=$begtm, endtm=$endtm");
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
	  show_messg("   <------ InsertDB: ped-file $sfilelist[$i],  begtm=$begtm, endtm=$endtm");
        }
      }#-->endof "insert"-mode
      $updflg=2; 
      $status = system("$JobScriptN $tdvname $begtm $endtm $updflg $SetupN >> $dblfn");#<-- call DBWriter 
      if($status != 0) {die  show_warn("   <-- $JobScriptN exited badly !");}
      while(<DBWLOGF>){
        $curline=$_;
        $logtext->insert('end',$curline);
        $logtext->yview('end');
      }
#
      $curline="<======================== File $sfilelist[$i] is written to DB !\n\n";
      $logtext->insert('end',$curline);
      $logtext->yview('end');
    }#--->endof "selected-file" check
    $percent+=1;
    $perc_done=100*($percent/$nelem);
#
    $mwnd->update;
  }#--->endof ped-files loop
  close(DBWLOGF) || die show_warn("   <-- Cannot close DBWLOGF !");
}
#------------------------------------------------------------------------------
sub quits_actions
{
  my ($curfn,$newfn,$curfnl,$newfnl,$run,$shsn,$fpath);
  my $status,$line,$detname,$histfn,$pedtyp;
#
#---> save processed ped-files info into "last session" hist-file:
#
  if($tofantsel eq "useTOF"){$detname="Tof";}
  else{$detname="Acc";}
#
# if($dryrunval==0){
    $histfn="TofDBupdHist.".$SessName;
    $fpath=$workdir."/".$histfn;
    $WarnFlg=0;
    open(HISTF,"> $histfn") or show_warn("\n   <--- Cannot open $fhistn for writing, $!");
    if($WarnFlg==0){
      for($i=0;$i<$nelem;$i++){
        $run=$sfilelist[$i];# full name like 'tofp_ds_rl.1211974030'
        $status=$processed[$i];
        if($status>=1){
          $line=$run."    ".$status;
          print HISTF $line."\n";
        }
      }
      close(HISTF) or show_warn("   <--- Cannot close $histfn after writing, $!");
      show_messg("\n   <---  History file $histfn is updated !");
#      $status=system("chmod 666 $histfn");
#      if($status != 0){
#        show_warn("\n   <--- Cannot change write privilege for $histfn, $!");
#      }
    }
    else{
      show_warn("\n   <--- Cannot create history file $histfn !!!, $!");
    }
# }
#
#---> archive processed files if requested:
#
  $status=0;
  if($archval==1 && $arcscval==0){
    show_messg("   <-- Archiving of processed ped-files...");
    for($i=0;$i<$nelem;$i++){#<--- processed files loop 
      if($processed[$i] == 1){
        $curfn=$pathpeds."/".$sfilelist[$i];
        $newfn=$patharch."/".$sfilelist[$i];
        $curfnl=$pathpeds."/".$sfilelist[$i].".log";
        $newfnl=$patharch."/".$sfilelist[$i].".log";
        move($curfn,$newfn) or show_warn("   <-- Archiving failed for ped-file $curfn $!");# move peds to archive
        move($curfnl,$newfnl) or show_warn("   <-- Archiving failed for log-file $curfnl $!");#...logs ...
      }
    }
    show_messg("   <-- Archiving is completed !");
  }
#
#---> save log-file (use run# of the 1st processed file as time-stamp of log-file):
#
  my $firstr=0;
  my $logfname="DBwrLogFile";
  my $fn;
  my $filen;
  for($i=0;$i<$nelem;$i++){#<--- selected files loop 
    if($processed[$i] == 1){
      $firstr=$runs[$i];
      last;
    }
  }
  if($firstr>0){
    $filen=$detname.$logfname.".".$firstr;
    $fn=$workdir.$pedsdir."/".$filen; 
    $newfn=$patharch."/".$filen;
    open(OFN, "> $fn") or die show_warn("   <-- Cannot open $filen for writing !");
    print OFN $logtext->get("1.0","end");
    show_warn("   <-- LogFile $filen is saved !");
    $status=system("chmod 666 $fn");
    if($status != 0){
      print "Warning: problem with write-priv for Logfile, status=",$status,"\n";
      exit;
    }
    move($fn,$newfn) or show_warn("   <-- Archiving failed for logfile $filen $!");# move to archive
    show_warn("   <-- LogFile $filen is archived, Bye-Bye !");
  }
  $mwnd->update;
  if($soundtext eq "Sound-ON"){$mwnd->bell;}
  $mwnd->after(5000);
  exit;
}
#--------
1;



