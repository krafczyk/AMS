// Sep 05, 1996. ak. First try with Objectivity. Method file for AMSgvolume
// Nov 28, 1996. ak. New function CmpTrMedia
//                  
// Last Edit : Nov 28, 1996. ak.
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

ooStatus AMSgtmedD::CmpTrMedia(integer id, AMSgtmed* p)
{
  ooStatus rstatus = oocSuccess;

  cout <<"AMSgtmedD::CmpTrMedia -I- compare "<<_name<<endl;

  if (_id != id) {
   cout<<"AMSgtmedD::CmpTrMedia -E- id is different "<<_id<<", "<<id<<endl;
   return oocError;
  }
  if(_isvol != p -> _isvol) {
   cout<<"AMSgtmedD::CmpTrMedia -E- _isvol "<<_isvol<<", "<<p->_isvol<<endl;
   rstatus = oocError;
  }
  if(_itmed != p -> _itmed) {
   cout<<"AMSgtmedD::CmpTrMedia -E- _itmed "<<_itmed<<", "<<p->_itmed<<endl;
   rstatus = oocError;
  }

  if(_itmat != p -> _itmat) {
   cout<<"AMSgtmedD::CmpTrMedia -E- _itmat "<<_itmat<<", "<<p->_itmat<<endl;
   rstatus = oocError;
  }

  if(_ifield != p -> _ifield) {
   cout<<"AMSgtmedD::CmpTrMedia -E- _ifield "<<_ifield<<", "<<p->_ifield<<endl;
   rstatus = oocError;
  }
  if(_fieldm != p -> _fieldm) {
   cout<<"AMSgtmedD::CmpTrMedia -E- _fieldm "<<_fieldm<<", "<<p->_fieldm<<endl;
   rstatus = oocError;
  }

  if(_tmaxfd != p -> _tmaxfd) {
   cout<<"AMSgtmedD::CmpTrMedia -E- _tmaxfd "<<_tmaxfd<<", "<<p->_tmaxfd<<endl;
   rstatus = oocError;
  }

  if(_stemax != p -> _stemax) {
   cout<<"AMSgtmedD::CmpTrMedia -E- _stemax "<<_stemax<<", "<<p->_stemax<<endl;
   rstatus = oocError;
  }

  if(_deemax != p -> _deemax) {
   cout<<"AMSgtmedD::CmpTrMedia -E- _deemax "<<_deemax<<", "<<p->_deemax<<endl;
   rstatus = oocError;
  }

  if(_epsil != p -> _epsil) {
   cout<<"AMSgtmedD::CmpTrMedia -E- _epsil "<<_epsil<<", "<<p->_epsil<<endl;
   rstatus = oocError;
  }

  if(_stmin != p -> _stmin) {
   cout<<"AMSgtmedD::CmpTrMedia -E- _stmin "<<_stmin<<", "<<p->_stmin<<endl;
   rstatus = oocError;
  }

   for(integer i=0; i<3; i++) {
     if (_birks[i] != p -> _birks[i]) {
      cout<<"AMSgtmedD::CmpTrMedia -E- _birks[i] "<<_birks[i]
          <<", "<<p->_birks[i]<<endl;
      rstatus = oocError;
     }
   }

  return rstatus;
}
