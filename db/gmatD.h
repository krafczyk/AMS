/* C++ header file: Objectivity/DB DDL version 3.8.1 */

#ifndef GMAT_D_H
#define GMAT_D_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "gmatD.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <iostream.h>
#include <string.h>
#include <cern.h>
#include <gmat.h>
#include <typedefs.h>

#ifndef GMAT_D_REF_H
#include "gmatD_ref.h"
#endif

#line 16 "gmatD.ddl"
declare(ooVArray,geant)

class AMSgmatD : public ooObj { 


protected: integer _id; 
integer _imate; 

geant _rho; 
geant _radl; 
geant _absl; 
geant _ubuf[1]; 

ooVArray(geant) _a; 
ooVArray(geant) _z; 
ooVArray(geant) _w; 

integer _npar; 



public: char _name[80]; 



AMSgmatD(); 
AMSgmatD(integer, class AMSgmat *, geant *, geant *, geant *, char *); 



inline integer getid() { return _id; }
inline char *getname() { return _name; }
inline integer getnpar() { return _npar; }
void getNumbers(integer &, integer &, geant &, geant &, geant &, geant *, geant *, geant *); 
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSgmatD)& ooThis(ooHandle(AMSgmatD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSgmatD) ooThis(ooMode aMode = oocNoOpen) const;
    void* operator new(size_t, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const ooId& nearId);
    void* operator new(size_t, ooConstCPtr(ooObj) nearObj);
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSgmatD)& ooThis(ooRef(AMSgmatD)& objId, ooMode aMode = oocNoOpen) const;
    AMSgmatD(ooInternalObj iobj);
#line 52 "gmatD.ddl"
}; 

#endif /* !defined(GMAT_D_H) */
