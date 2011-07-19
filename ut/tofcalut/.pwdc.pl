sub pw_check{
  my $found=0;
  my @usrlist=qw(Bindi Choumilov Contin Guest Palmonari Quadrani);
  foreach $user(@usrlist) {
    if($user eq $TofUserN){$found=1;}
  }
  if($found==1){
    show_messg("   <--- User name is confirmed for user '$TofUserN' !");
  }
  else{
    $UNinAtt+=1;
    if($UNinAtt<=3){
      show_warn("   <--- Error: User Name is incorrect, try again !");
      return;
    }
    else{
      show_warn("   <-- Fatal: You can't work with this program !!!");
      $mwnd->after(6000);
      exit;
    }
  }
#--> now check PW:
  if($TofUserN eq "Guest"){
    show_messg("\n   <--- No need in PW for Guest but only dummy(DB ReadOnly)  mode is permitted !!!","Big");
    $LoginDone=1;
    return;
  }
#
  if($CurHost eq "scamsfs1"){
    show_messg("\n   <--- User $TofUserN do not need PW on this host, DB update is granted !!!","Big");
    $LoginDone=1;
    $dryrbt->configure(-state=>'normal');
    return;
  }
#
  my $stat=0;
  $pwfn="pwfile";
  open(PWF,"+> $pwfn") or die show_warn("   <-- Cannot open $pwfn !, $!");# clear file if exists
  print PWF $TofPassW,"\n";
  $stat=system("klog admin -cell ams.cern.ch < $pwfn");
  close(PWF);
  if($stat!=0){
    $PWinAtt+=1;
    if($PWinAtt<=3){
      show_warn("   <--- Error: Admin. pass-word is not correct, try again !");
      return;
    }
    else{
      show_warn("\n   <--- Alas ! You are not allowed to update DB, but you can still");  
      show_warn("        work in dummy mode for training (without real DB update) !!!");  
      $stat = system("rm -f $pwfn");
      if($stat != 0){show_warn("   <--- Can't delete $pwfn !");}
      $LoginDone=1;
    }
  }
  else{
    $stat = system("rm -f $pwfn");
    if($stat != 0){show_warn("   <--- Can't delete $pwfn !");}
    $dryrbt->configure(-state=>'normal');
    show_messg("\n   <--- Pass-word is OK, you are allowed to update DB !!!","Big");    
    $LoginDone=1;
  } 
}
1;
