//  $Id: mcctcclusterD.h,v 1.6 2001/01/22 17:32:30 choutko Exp $
/* C++ header file: Objectivity/DB DDL version 4.0.2 */

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
number _time; 




inline AMSCTCMCClusterD() { }
AMSCTCMCClusterD(class AMSCTCMCCluster *); 


void copy(AMSCTCMCCluster *); 
inline AMSPoint getxcoo() { return _xcoo; }
inline AMSPoint getxdir() { return _xdir; }
inline number getstep() { return _step; }
inline number getcharge() { return _charge; }
inline number getedep() { return _edep; }
inline number getbeta() { return _beta; }
inline integer getid() { return _idsoft; }
inline number gettime() { return _time; }
inline integer getlayno() const { return CTCDBc :: getgeom () < 2 ? (_idsoft / 1000) % 10 :
 _idsoft / 1000000; }
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
	return opiODMGDatabaseNew(size, ooTypeN(AMSCTCMCClusterD), odmg_db);
      }	
    void *operator new(size_t  size,
		       d_Database *odmg_db,
		       const char *)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSCTCMCClusterD), odmg_db);
      }	
#endif
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSCTCMCClusterD)& ooThis(ooRef(AMSCTCMCClusterD)& objId, ooMode aMode = oocNoOpen) const;
    AMSCTCMCClusterD(ooInternalObj iobj);
#line 44 "mcctcclusterD.ddl"
}; 

#endif /* !defined(MCCTCCLUSTER_D_H) */
