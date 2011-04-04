// $Id: rootlogon.C,v 1.3 2011/04/04 20:24:22 haino Exp $

// Example of rootlogon.C
// You should edit the library path if needed

void rootlogon(void)
{
  // Load shared library
  gSystem->Load("$AMSWD/lib/linuxx8664gcc/ntuple_slc4_PG.so");

  // Set include path for ACLiC and compile flag
  gSystem->SetIncludePath("-I$ROOTSYS/include -I../include -D_PGTRACK_");
}
