/* C++ header file: Objectivity/DB DDL version 3.8.1 */

#ifndef MCCTCCLUSTER_D_H
#define MCCTCCLUSTER_D_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "mcctcclusterD.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <typedefs.h>
#include <point.h>
#include <mccluster.h>

#ifndef MCCTCCLUSTER_D_REF_H
#include "mcctcclusterD_ref.h"
#endif

#line 13 "mcctcclusterD.ddl"
class AMSCTCMCClusterD : public ooObj { 


public: class AMSPoint _xcoo; 
class AMSDir _xdir; 
number _step; 
number _charge; 
number _edep; 
number _beta; 

integer _idsoft; 
integer _Position; 
#line 30
inline AMSCTCMCClusterD() { }
AMSCTCMCClusterD(class AMSCTCMCCluster *); 

void copy(AMSCTCMCCluster *); 

inline integer getPosition() { return _Position; }
inline void setPosition(integer pos) { _Position = pos; }
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSCTCMCClusterD)& ooThis(ooHandle(AMSCTCMCClusterD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSCTCMCClusterD) ooThis(ooMode aMode = oocNoOpen) const;
    void* operator new(size_t, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const ooId& nearId);
    void* operator new(size_t, ooConstCPtr(ooObj) nearObj);
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSCTCMCClusterD)& ooThis(ooRef(AMSCTCMCClusterD)& objId, ooMode aMode = oocNoOpen) const;
    AMSCTCMCClusterD(ooInternalObj iobj);
#line 38 "mcctcclusterD.ddl"
}; 

#endif /* !defined(MCCTCCLUSTER_D_H) */
