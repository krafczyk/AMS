{
  gROOT->Reset();
  
  // Set include path
  gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSSRC/include"));
  
  // for loading AMSRoot libraries
  gSystem->Load("$AMSWD/lib/linux/ntuple_slc4_PG.so");
 //gROOT->ProcessLine(".L BObjArray.C+"); 
 
 //gROOT->ProcessLine(".L BHist.C+"); 
 
 //gROOT->ProcessLine(".L DHF.C+"); 
}
