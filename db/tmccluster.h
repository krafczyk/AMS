/* C++ header file: Objectivity/DB DDL version 3.8.1 */

#ifndef TMCCLUSTER_H
#define TMCCLUSTER_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "tmccluster.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <typedefs.h>
#include <point.h>
#include <mccluster.h>

#ifndef TMCCLUSTER_REF_H
#include "tmccluster_ref.h"
#endif

#line 14 "tmccluster.ddl"
class AMSTrMCClusterD : public ooObj { 



protected: class AMSPoint _xca; 
AMSPoint _xcb; 
AMSPoint _xgl; 
number _sum; 

geant _ss[2][5]; 

integer _idsoft; 
integer _itra; 
integer _left[2]; 
integer _center[2]; 
integer _right[2]; 

integer _Position; 
#line 38
public: inline AMSTrMCClusterD() { }
AMSTrMCClusterD(class AMSTrMCCluster *); 


void copy(AMSTrMCCluster *); 
inline void setsum(number sum) { _sum = sum; }
inline number getsum() { return _sum; }
inline integer getPosition() { return _Position; }
inline void setPosition(integer pos) { _Position = pos; }
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSTrMCClusterD)& ooThis(ooHandle(AMSTrMCClusterD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSTrMCClusterD) ooThis(ooMode aMode = oocNoOpen) const;
    void* operator new(size_t, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const ooId& nearId);
    void* operator new(size_t, ooConstCPtr(ooObj) nearObj);
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSTrMCClusterD)& ooThis(ooRef(AMSTrMCClusterD)& objId, ooMode aMode = oocNoOpen) const;
    AMSTrMCClusterD(ooInternalObj iobj);
#line 47 "tmccluster.ddl"
}; 

#endif /* !defined(TMCCLUSTER_H) */
