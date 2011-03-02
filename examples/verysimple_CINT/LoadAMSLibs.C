{
  gROOT->Reset();
  
  // Set include path
  gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSSRC/include"));
  char libname[255];
  sprintf(libname,"$AMSWD/lib/%s/ntuple_slc4_PG.so",gSystem->GetBuildArch()); 
  // for loading AMSRoot libraries
  gSystem->Load(libname);
  gROOT->ProcessLine(".L analyze.C");
}
