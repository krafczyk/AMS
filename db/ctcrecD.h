/* C++ header file: Objectivity/DB DDL version 4.0.2 */

#ifndef CTCREC_D_H
#define CTCREC_D_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "ctcrecD.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <typedefs.h>
#include <point.h>
#include <ctcrec.h>

#ifndef CTCREC_D_REF_H
#include "ctcrecD_ref.h"
#endif

#line 11 "ctcrecD.ddl"
class AMSCTCClusterD : public ooObj { 


protected: class AMSPoint _Coo; 
AMSPoint _ErrorCoo; 

number _Signal; 
number _CorrectedSignal; 
number _ErrorSignal; 


integer _Status; 
integer _Layer; 

integer _Position; 




public: inline integer checkstatus(integer checker) const { return _Status & checker; }
inline void setstatus(integer status) { _Status = _Status | status; }
inline void correct(number factor) {
               _CorrectedSignal = _CorrectedSignal * factor;
               _ErrorSignal = _ErrorSignal * factor; }

inline number getrawsignal() const { return _Signal; }
inline number getsignal() const { return _CorrectedSignal; }
inline number geterror() const { return _ErrorSignal; }
inline AMSPoint getcoo() const { return _Coo; }
inline AMSPoint getecoo() const { return _ErrorCoo; }
inline integer getPosition() { return _Position; }
inline integer getLayer() { return _Layer; }
inline integer getstatus() { return _Status; }

inline void setPosition(integer pos) { _Position = pos; }
void copy(class AMSCTCCluster *); 



AMSCTCClusterD(); 
AMSCTCClusterD(AMSCTCCluster *); 
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSCTCClusterD)& ooThis(ooHandle(AMSCTCClusterD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSCTCClusterD) ooThis(ooMode aMode = oocNoOpen) const;
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
	return opiODMGDatabaseNew(size, ooTypeN(AMSCTCClusterD), odmg_db);
      }	
    void *operator new(size_t  size,
		       d_Database *odmg_db,
		       const char *)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSCTCClusterD), odmg_db);
      }	
#endif
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSCTCClusterD)& ooThis(ooRef(AMSCTCClusterD)& objId, ooMode aMode = oocNoOpen) const;
    AMSCTCClusterD(ooInternalObj iobj);
#line 52 "ctcrecD.ddl"
}; 

#endif /* !defined(CTCREC_D_H) */
