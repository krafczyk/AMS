//  $Id: gmatD.h,v 1.5 2001/01/22 17:32:29 choutko Exp $
/* C++ header file: Objectivity/DB DDL version 4.0.2 */

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

#line 17 "gmatD.ddl"
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


ooStatus CmpMaterials(integer, AMSgmat *); 
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
    void *operator new(size_t, const ooHandle(ooObj) &nearH = oovTopDB);
    void *operator new(size_t, const ooId& nearId);
    void *operator new(size_t, ooConstCPtr(ooObj) nearObj);
    void *operator new(size_t,
		       const ooHandle(ooObj) &nearH,
		       const char *typeName);
#ifdef OO_ODMG
    void *operator new(size_t  size,
		       d_Database *odmg_db)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSgmatD), odmg_db);
      }	
    void *operator new(size_t  size,
		       d_Database *odmg_db,
		       const char *)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSgmatD), odmg_db);
      }	
#endif
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSgmatD)& ooThis(ooRef(AMSgmatD)& objId, ooMode aMode = oocNoOpen) const;
    AMSgmatD(ooInternalObj iobj);
#line 54 "gmatD.ddl"
}; 

#endif /* !defined(GMAT_D_H) */
