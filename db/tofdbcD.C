// Oct 08, 1996. ak. first try with Objectivity
//                   method source file of object TOFDBcD
//
// Last Edit : Jan 21, 1997. ak. 
//                     TOFDBcD doens't match to the latest E.Ch. version
//

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <tofdbc.h>
#include <tofdbcD.h>
//

TOFDBcD:: TOFDBcD() 
{
  integer i, j;

  for (i=0; i<SCLRS; i++) {
    for (j=0; j<SCMXBR; j++) {
      _brtypeD[i][j] = TOFDBc::_brtype[i][j];
    }
  }
 
  for (i=0; i<SCLRS; i++) {
   _plrotmD[i] = TOFDBc::_plrotm[i];
   SCBRS[i] = 14;
  }

  for (i=0; i<SCBTPN; i++) {
    if(TOFDBc::_brfnam[i]) strcpy(_brfnamD[i],TOFDBc::_brfnam[i]);
   _brlenD[i] = TOFDBc::_brlen[i];
  }

   for (i=0; i<10; i++) {
     _supstrD[i] = TOFDBc::_supstr[i];
     //
     //-------------------  _mcfparD[i] = TOFDBc::_mcfpar[i];
     //
   }

  for (i=0; i<15; i++) _plnstrD[i] = TOFDBc::_plnstr[i];
}
