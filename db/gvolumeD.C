// Aug 08, 1996. ak. First try with Objectivity. Method file for AMSgvolume
// Oct 12, 1996. ak. coo copy is modified.
//                  
// Last Edit : Oct 12, 1996. ak.
// 

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <gvolumeD.h>

#include <point.h>

AMSgvolumeD::AMSgvolumeD() {}
AMSgvolumeD::AMSgvolumeD(integer id, AMSgvolume* p, char name[], integer pos)
{
  integer i, j;

  _id =  id;
  _matter = p -> _matter;
  _rotmno = p -> _rotmno;
  _posp   = p -> _posp;
  _gid    = p -> _gid;
  _npar   = p -> _npar;
  for (i=0; i<6; i++) { _par[i] = p -> _par[i];}
  for (i=0; i<3; i++) {
    for (j=0; j<3; j++) { 
     _nrm[i][j]  = p -> _nrm[i][j];
     _inrm[i][j] = p -> _inrm[i][j];
    }
  }

  if(name)  strcpy(_name, name);
  if(p -> _shape) strcpy(_shape, p-> _shape);
  if(p -> _gonly) strcpy(_gonly, p ->_gonly);

  _ContPos = pos;

  _posN = 0;
  _posP = 0;
  _posU = 0;
  _posD = 0;

// coo special trick
  _coo    = p -> _cooA;
}
 // Set/Get Methods
void AMSgvolumeD::getpar(geant* par) {UCOPY( &_par, par, sizeof(geant)*6/4);}
void AMSgvolumeD::getcoo(AMSPoint* coo) 
                                 {UCOPY( &_coo, coo, sizeof(AMSPoint)*1/4);}
void AMSgvolumeD::getshape(char shape[]) { if(_shape)strcpy(shape,_shape); }
void AMSgvolumeD::getgonly(char gonly[]) { if(_gonly)strcpy(gonly,_gonly); }
void AMSgvolumeD::getnrm(number* nbuff0)
{
  UCOPY( &_nrm, nbuff0, sizeof(number)*3*3/4);
  }

