//  $Id: gtmedD.h,v 1.4 2001/01/22 17:32:29 choutko Exp $
/* C++ header file: Objectivity/DB DDL version 4.0.2 */

#ifndef GTMED_D_H
#define GTMED_D_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "gtmedD.ddl"
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

#ifndef GTMED_D_REF_H
#include "gtmedD_ref.h"
#endif

#line 16 "gtmedD.ddl"
class AMSgtmedD : public ooObj { 


protected: geant _fieldm; 
geant _tmaxfd; 
geant _stemax; 
geant _deemax; 
geant _epsil; 
geant _stmin; 
geant _ubuf[1]; 
geant _birks[3]; 

integer _id; 

integer _isvol; 
integer _itmed; 
integer _itmat; 
integer _ifield; 

char _yb; 



public: char _name[80]; 


AMSgtmedD(); 
AMSgtmedD(integer, class AMSgtmed *, char *); 

inline integer getid() { return _id; }
inline char *getname() { return _name; }
inline char getyb() { return _yb; }
void getNumbers(integer *, geant *); 
ooStatus CmpTrMedia(integer, AMSgtmed *); 
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSgtmedD)& ooThis(ooHandle(AMSgtmedD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSgtmedD) ooThis(ooMode aMode = oocNoOpen) const;
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
	return opiODMGDatabaseNew(size, ooTypeN(AMSgtmedD), odmg_db);
      }	
    void *operator new(size_t  size,
		       d_Database *odmg_db,
		       const char *)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSgtmedD), odmg_db);
      }	
#endif
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSgtmedD)& ooThis(ooRef(AMSgtmedD)& objId, ooMode aMode = oocNoOpen) const;
    AMSgtmedD(ooInternalObj iobj);
#line 50 "gtmedD.ddl"
}; 

#endif /* !defined(GTMED_D_H) */
