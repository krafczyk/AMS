//  $Id: tofrecD.h,v 1.6 2001/01/22 17:32:31 choutko Exp $
/* C++ header file: Objectivity/DB DDL version 4.0.2 */

#ifndef TOFREC_D_H
#define TOFREC_D_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "tofrecD.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <typedefs.h>
#include <point.h>
#include <tofrec.h>

#ifndef TOFREC_D_REF_H
#include "tofrecD_ref.h"
#endif

#line 16 "tofrecD.ddl"
class AMSTOFClusterD : public ooObj { 


protected: class AMSPoint _Coo; 
AMSPoint _ErrorCoo; 


number _edep; 
number _time; 
number _etime; 

integer _ntof; 
integer _plane; 

integer _status; 
integer _Position; 



public: static const integer BAD; 
static const integer USED; 

inline integer getntof() const { return _ntof; }
inline integer getplane() const { return _plane; }
inline number gettime() const { return _time; }
inline number getetime() const { return _etime; }
inline number getedep() const { return _edep; }
inline AMSPoint getcoo() const { return _Coo; }
inline AMSPoint getecoo() const { return _ErrorCoo; }

inline integer getstatus() { return _status; }

inline integer getPosition() { return _Position; }
inline void setPosition(integer pos) { _Position = pos; }
void copy(class AMSTOFCluster *); 


AMSTOFClusterD(); 
AMSTOFClusterD(AMSTOFCluster *); 
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSTOFClusterD)& ooThis(ooHandle(AMSTOFClusterD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSTOFClusterD) ooThis(ooMode aMode = oocNoOpen) const;
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
	return opiODMGDatabaseNew(size, ooTypeN(AMSTOFClusterD), odmg_db);
      }	
    void *operator new(size_t  size,
		       d_Database *odmg_db,
		       const char *)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSTOFClusterD), odmg_db);
      }	
#endif
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSTOFClusterD)& ooThis(ooRef(AMSTOFClusterD)& objId, ooMode aMode = oocNoOpen) const;
    AMSTOFClusterD(ooInternalObj iobj);
#line 55 "tofrecD.ddl"
}; 

#endif /* !defined(TOFREC_D_H) */
