/* C++ header file: Objectivity/DB DDL version 3.8.1 */

#ifndef MCTOFCLUSTER_D_H
#define MCTOFCLUSTER_D_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "mctofclusterD.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <typedefs.h>
#include <point.h>
#include <mccluster.h>

#ifndef MCTOFCLUSTER_D_REF_H
#include "mctofclusterD_ref.h"
#endif

#line 17 "mctofclusterD.ddl"
class AMSTOFMCClusterD : public ooObj { 


public: class AMSPoint _xcoo; 

integer _idsoft; 

number _tof; 
number _edep; 

integer _Position; 
#line 33
inline AMSTOFMCClusterD() { }
AMSTOFMCClusterD(class AMSTOFMCCluster *); 

void copy(AMSTOFMCCluster *); 

inline integer getPosition() { return _Position; }
inline void setPosition(integer pos) { _Position = pos; }
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSTOFMCClusterD)& ooThis(ooHandle(AMSTOFMCClusterD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSTOFMCClusterD) ooThis(ooMode aMode = oocNoOpen) const;
    void* operator new(size_t, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const ooId& nearId);
    void* operator new(size_t, ooConstCPtr(ooObj) nearObj);
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSTOFMCClusterD)& ooThis(ooRef(AMSTOFMCClusterD)& objId, ooMode aMode = oocNoOpen) const;
    AMSTOFMCClusterD(ooInternalObj iobj);
#line 41 "mctofclusterD.ddl"
}; 

#endif /* !defined(MCTOFCLUSTER_D_H) */
