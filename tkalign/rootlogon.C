// $Id: rootlogon.C,v 1.1 2009/06/10 10:26:34 shaino Exp $

// Example of rootlogon.C
// You should edit the library path if needed

void rootlogon(void)
{
  // Load shared library
  gSystem->Load("../lib/linux/ntuple_slc4.so");

  // Set include path for ACLiC and compile flag
  gSystem->SetIncludePath("-I$ROOTSYS/include -I../include -D_PGTRACK_");
}
