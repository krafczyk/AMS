// Sep 05, 1996. ak. First try with Objectivity. Method file for AMSgvolume
//                   
// Last Edit : Sep 09, 1996. ak.
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
