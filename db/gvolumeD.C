// Aug 08, 1996. ak. First try with Objectivity. Method file for AMSgvolume
// Oct 12, 1996. ak. coo copy is modified.
// Mar 25, 1997. ak. new class member _rel
// July 01,1997. ah. new method CmpGeometry added 
//                 
// Last Edit : Mar 25, 1996. ak.
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
  _rel    = p -> _rel;
  for (i=0; i<6; i++) { _par[i] = p -> _par[i];}
  for (i=0; i<3; i++) {
    for (j=0; j<3; j++) { 
     _nrm[i][j]   = p -> _nrm[i][j];
     _nrmA[i][j]  = p -> _nrmA[i][j];
     _inrmA[i][j] = p -> _inrmA[i][j];
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
  _coo    = p -> _coo;
  _cooA   = p -> _cooA;
}
 // Set/Get Methods
void AMSgvolumeD::getpar(geant* par) {UCOPY( &_par, par, sizeof(geant)*6/4);}
void AMSgvolumeD::getcoo(AMSPoint* coo) 
                                 {UCOPY( &_coo, coo, sizeof(AMSPoint)*1/4);}
void AMSgvolumeD::getcooA(AMSPoint* cooA) 
                                 {UCOPY( &_cooA, cooA, sizeof(AMSPoint)*1/4);}
void AMSgvolumeD::getshape(char shape[]) { if(_shape)strcpy(shape,_shape); }
void AMSgvolumeD::getgonly(char gonly[]) { if(_gonly)strcpy(gonly,_gonly); }
void AMSgvolumeD::getnrm(number* nbuff0) { 
                               UCOPY( &_nrm, nbuff0, sizeof(number)*3*3/4); }
void AMSgvolumeD::getnrmA(number* nbuff0) { 
                              UCOPY( &_nrmA, nbuff0, sizeof(number)*3*3/4); }
void AMSgvolumeD::getinrmA(number* nbuff0) { 
                               UCOPY( &_inrmA, nbuff0, sizeof(number)*3*3/4); }
ooStatus AMSgvolumeD::CmpGeometry(integer id, AMSgvolume* p)
{
  ooStatus rstatus = oocSuccess;
  if (dbg_prtout) cout <<"AMSgvolumeD::CmpGeometry -I- compare "<<_name<<endl;

  if (_id != id) {
   cout<<"AMSgvolumeD::CmpGeometry -E- id is different "<<_id<<", "<<id<<endl;
   return oocError;
  }
  if (_matter != p->_matter) {
   cout<<"AMSgvolumeD::CmpGeometry -E- _matter is different "<<_matter<<", "<<p->_matter<<endl;
   return oocError;
  }
  if (_rotmno != p->_rotmno) {
   cout<<"AMSgvolumeD::CmpGeometry -E- _rotmno is different "<<_rotmno<<", "<<p->_rotmno<<endl;
   return oocError;
  }
  if (_posp != p->_posp) {
   cout<<"AMSgvolumeD::CmpGeometry -E- _posp is different "<<_posp<<", "<<p->_posp<<endl;
   return oocError;
  }
  if (_gid != p->_gid) {
   cout<<"AMSgvolumeD::CmpGeometry -E- _gid is different "<<_gid<<", "<<p->_gid<<endl;
   return oocError;
  }
  if (_npar != p->_npar) {
   cout<<"AMSgvolumeD::CmpGeometry -E- _npar is different "<<_npar<<", "<<p->_npar<<endl;
   return oocError;
  }
  else {
   for (integer i=0; i<_npar; i++) {
  if (_par[i] != p->_par[i]) {
   cout<<"AMSgvolumeD::CmpGeometry -E- _par[i] is different "<<_par[i]<<", "<<p->_par[i]<<endl;
   return oocError;
   } //end of inner if loop
                                   } //end of for loop
  }//end of _npar if

  if (_rel != p->_rel) {
   cout<<"AMSgvolumeD::CmpGeometry -E- _rel is different "<<_rel<<", "<<p->_rel<<endl;
   return oocError;
  }
   if (strcmp(_gonly, p->_gonly) !=0) {
   cout<<"AMSgvolumeD::CmpGeometry -E- _gonly[i] is different "<<_gonly<<", "
                                                               <<p->_gonly<<endl;
   return oocError;
  } //end of inner if loop

   if (strcmp(_shape, p->_shape) !=0) {
   cout<<"AMSgvolumeD::CmpGeometry -E- _shape[i] is different "<<_shape<<", "
                                                               <<p->_shape<<endl;
   return oocError;
  } //end of inner if loop


  if (!(_cooA == p->_cooA)) {
   cout <<"AMSgvolumeD::CmpGeometry -E- _cooA is different "
        <<_cooA<<", "<<p->_cooA<<endl;
   return oocError;
 }
  if (!(_coo == p->_coo))  {
   cout<<"AMSgvolumeD::CmpGeometry -E- _coo is different "
       <<_cooA<<", "<<p->_cooA<<endl;
   return oocError;
 }                      
for (integer j=0; j<3; j++) {
 for (integer k=0; k<3; k++) {
  if (_nrm[j][k] != p->_nrm[j][k]) {
   cout<<"AMSgvolumeD::CmpGeometry -E- _nrm[i][j] is different " <<_nrm[j][k]<<","
                                                                 <<p->_nrm[j][k]<<endl;
   return oocError;
  } //end of inner if loop

  if (_nrmA[j][k] != p->_nrmA[j][k]) {
   cout<<"AMSgvolumeD::CmpGeometry -E- _nrmA[i][j] is different " <<_nrmA[j][k]<<", "
                                                                  <<p->_nrmA[j][k]<<endl;
   return oocError;
  } //end of inner if loop
                             } //end of k for loop
                            }  //end of j for loop

      if (_ContPos != p->_ContPos) {
          cout<<"AMSgvolumeD::CmpGeometry -E- _ContPos is different " <<_ContPos<<", "
                                                                      <<p->_ContPos<<endl;
   return oocError;
   } //end of inner if loop


 return rstatus;
}
