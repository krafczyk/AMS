// Sep 05, 1996. ak. First try with Objectivity. Method file for AMSgvolume
//                   
// Last Edit : Oct 10, 1996. ak.
// 

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <gtmedD.h>


AMSgtmedD::AMSgtmedD() {}
AMSgtmedD::AMSgtmedD(integer id, AMSgtmed* p, char* name)
{
  _id = id;
  _isvol = p -> _isvol;
  _itmed = p -> _itmed;
  _itmat = p -> _itmat;
  _ifield = p -> _ifield;
  _fieldm = p -> _fieldm;
  _tmaxfd = p -> _tmaxfd;
  _stemax = p -> _stemax;
  _deemax = p -> _deemax;
  _epsil = p ->  _epsil;
  _stmin = p ->  _stmin;
  _ubuf[0] = p -> _ubuf[0];

   for(integer i=0; i<3; i++) {_birks[i] = p -> _birks[i];}  

   if ( p -> _yb) _yb = p -> _yb;

   if (name) strcpy (_name,name);
}

void AMSgtmedD::getNumbers(integer* ibuff, geant* gbuff)
{
   UCOPY(&_isvol, ibuff, sizeof(integer)*4/4);   
   UCOPY(&_fieldm, gbuff, sizeof(geant)*10/4);
}  
