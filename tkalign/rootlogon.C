// $Id: rootlogon.C,v 1.2 2009/08/22 18:54:01 pzuccon Exp $

// Example of rootlogon.C
// You should edit the library path if needed

void rootlogon(void)
{
  // Load shared library
  gSystem->Load("$AMSWD/lib/linux/ntuple_slc4_PG.so");

  // Set include path for ACLiC and compile flag
  gSystem->SetIncludePath("-I$ROOTSYS/include -I../include -D_PGTRACK_");
}
