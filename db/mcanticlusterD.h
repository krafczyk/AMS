/* C++ header file: Objectivity/DB DDL version 4.0.2 */

#ifndef MCANTICLUSTER_D_H
#define MCANTICLUSTER_D_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "mcanticlusterD.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <typedefs.h>
#include <point.h>
#include <mccluster.h>

#ifndef MCANTICLUSTER_D_REF_H
#include "mcanticlusterD_ref.h"
#endif

#line 11 "mcanticlusterD.ddl"
class AMSAntiMCClusterD : public ooObj { 


public: class AMSPoint _xcoo; 

integer _idsoft; 

number _tof; 
number _edep; 



inline AMSAntiMCClusterD() { }
AMSAntiMCClusterD(class AMSAntiMCCluster *); 


void copy(AMSAntiMCCluster *); 
inline integer getid() { return _idsoft; }
inline AMSPoint getcoo() { return _xcoo; }
inline number gettof() { return _tof; }
inline number getedep() { return _edep; }
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSAntiMCClusterD)& ooThis(ooHandle(AMSAntiMCClusterD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSAntiMCClusterD) ooThis(ooMode aMode = oocNoOpen) const;
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
	return opiODMGDatabaseNew(size, ooTypeN(AMSAntiMCClusterD), odmg_db);
      }	
    void *operator new(size_t  size,
		       d_Database *odmg_db,
		       const char *)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSAntiMCClusterD), odmg_db);
      }	
#endif
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSAntiMCClusterD)& ooThis(ooRef(AMSAntiMCClusterD)& objId, ooMode aMode = oocNoOpen) const;
    AMSAntiMCClusterD(ooInternalObj iobj);
#line 33 "mcanticlusterD.ddl"
}; 

#endif /* !defined(MCANTICLUSTER_D_H) */
