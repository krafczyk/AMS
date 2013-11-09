{
  TString lib("$AMSWD/lib/$ARCH/ntuple_slc4.so");
  gSystem->ExpandPathName(lib);

  //  printf("\n");  
  printf("******* Loading lib %s\n", lib.Data());
  //  printf("\n");  
  gSystem->Load(lib.Data());
}
