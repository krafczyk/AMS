//  $Id: gmatD.C,v 1.5 2001/01/22 17:32:29 choutko Exp $
// Sep 05, 1996. ak. First try with Objectivity. Method file for AMSgvolume
// Nov 27, 1996. ak. new function CmpMaterials
//                  
// Last Edit : Nov 27, 1996. ak.
// 

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <gmatD.h>


AMSgmatD::AMSgmatD()  {}

AMSgmatD::AMSgmatD
           (integer id, AMSgmat* p, geant* a, geant* z, geant* w, char* name)
{
  _id      = id;
  _imate   = p -> _imate;
  _npar    = p -> _npar;
  _rho     = p -> _rho;
  _radl    = p -> _radl;
  _absl    = p -> _absl;
  _ubuf[0] = p -> _ubuf[0];  

   _a.resize(_npar);
   _z.resize(_npar);
   _w.resize(_npar);

  for (integer i =0; i< _npar; i++)
    {
      _a.set(i,a[i]);
      _z.set(i,z[i]);
      _w.set(i,w[i]);
    }
  
  if(name)  strcpy(_name, name);
}

void   AMSgmatD::getNumbers(integer& id, integer& imate,  
                            geant& rho, geant& radl, geant& absl,
                            geant* a, geant* z, geant* w) 
{
  id   = _id;
  imate= _imate;
  rho  = _rho;
  radl = _radl;
  absl = _absl;

  for (integer i=0; i<_npar; i++) 
  {
   a[i] = _a[i];
   z[i] = _z[i];
   w[i] = _w[i];
  }
 }

ooStatus AMSgmatD::CmpMaterials(integer id, AMSgmat* p)
{
  integer rstatus = oocSuccess;

  cout <<"AMSgmatD::CmpMaterials -I- "<<_name<<endl;
  if (_id != id) {
    cout<<"AMSgmatD::CmpMaterials -E- id "<<_id<<", "<<id<<endl;
    return oocError;
  }
  if (_imate   != p -> _imate) {
   cout<<"AMSgmatD::CmpMaterials -E- imate "<<_imate<<", "<<p ->_imate<<endl;
   rstatus = oocError; 
  }
  if (_npar   != p -> _npar) {
   cout<<"AMSgmatD::CmpMaterials -E- npar "<<_npar<<", "<<p ->_npar<<endl;
   rstatus = oocError; 
  }
  if (_rho   != p -> _rho) {
   cout<<"AMSgmatD::CmpMaterials -E- rho "<<_rho<<", "<<p->_rho<<endl;
   rstatus = oocError; 
  }
  if (_radl   != p -> _radl) {
   cout<<"AMSgmatD::CmpMaterials -E- radl "<<_radl<<", "<<p->_radl<<endl;
   rstatus = oocError; 
  }
  if (_absl   != p -> _absl) {
   cout<<"AMSgmatD::CmpMaterials -E- absl "<<_absl<<", "<<p->_absl<<endl;
   rstatus = oocError; 
  }

  if (_npar == p -> _npar) {
   geant a[10], z[10], w[10];
   p -> getNumbers(a, z, w);
   for (integer i =0; i< _npar; i++)
     {
      if (_a[i] != a[i] ||
          _z[i] != z[i] ||
          _w[i] != w[i] ) {
        cout<<"AMSgmatD::CmpMaterials -E- a,w,z are different"<<endl;
        cout<<"AMSgmatD::db - a,w,z "<<_a[i]<<", "<<_w[i]<<", "<<_z[i]<<endl;
        cout<<"AMSgmatD::mem- a,w,z "<<a[i]<<", "<<w[i]<<", "<<z[i]<<endl;
        return oocError;
      }
     }
  } else {
   cout<<"AMSgmatD::CmpMaterials -E- npar "<<_npar<<", "
       <<p -> _npar<<endl;
   rstatus = oocError; 
  }  
 return rstatus;
}
