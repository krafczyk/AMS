void IdleHandle(Int_t option)
{
  static TTree *t = 0;

  //
  // CUT comes here
  //
//  static char *cut   
//   = "ntrtr==1 && Ridgidity<0 && abs(pmom)/pcharge<2 && nctccl<2";
  static char *cut = "Chi2strline<2 && Chi2circle<2 && Chi2fastfit<10 && beta>-1.";

  static TTreeFormula *f_cut=0;
  static Int_t totEvt = 0;

  if (option <= 0) {
    if (t != gAMSR_Root->GetInput()) {
      t = gAMSR_Root->GetInput();
      totEvt = t->GetEntries();
      printf("new data: TTree %lx has %d events\n",t,totEvt);
      totEvt = totEvt-1;

      if (f_cut != 0) {delete f_cut; f_cut=0;}
      f_cut=new TTreeFormula("cut",cut,t);
    }
  }

  if (option == 0) {
    t->SetBranchStatus("*", 0);         //disable all branches
    t->SetBranchStatus("Chi2strline", 1);
    t->SetBranchStatus("Chi2circle", 1);
    t->SetBranchStatus("Chi2fastfit",1);
    t->SetBranchStatus("beta",1);      //end of enabling branches
    Int_t current = gAMSR_Root->Event();
    for (Int_t next=current+1; next<=totEvt; next++) {
      t->GetEvent(next);
      if (f_cut->EvalInstance(0) != 0) {
        t->SetBranchStatus("*", 1);    //recover to enable all branches
        gAMSR_Root->Clear();
        gAMSR_Root->GetEvent(next);
        gAMSR_Display->Pad()->cd();
        gAMSR_Display->Draw();
        break;
      }
    }
    if ( next >= totEvt ) {
//      t->SetBranchStatus("*", 1);       //recover to enable all branches
      printf("reach to the end of events, next %d !!\n",next);
      gSystem->ExitLoop();  
      return;
    }
    printf("event changed to %d\n",next);

  } else if (option > 0) {
    if (f_cut) delete f_cut;

  }

  return;
}
