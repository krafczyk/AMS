//  $Id: trrechit.h,v 1.5 2001/01/22 17:32:31 choutko Exp $
/* C++ header file: Objectivity/DB DDL version 4.0.2 */

#ifndef TRRECHIT_H
#define TRRECHIT_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "trrechit.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <typedefs.h>
#include <point.h>
#include <trrec.h>

#ifndef TRRECHIT_REF_H
#include "trrechit_ref.h"
#endif

class AMSTrClusterD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTrClusterD);
class ooItr(AMSTrClusterD);
class ooRef(AMSTrClusterD);
class ooShortRef(AMSTrClusterD);
#endif // !defined(OO_BUGGY_TEMPLATES)

#line 16 "trrechit.ddl"
class AMSTrClusterD; 

class AMSTrRecHitD : public ooObj { 




protected: class AMSPoint _Hit; 
AMSPoint _EHit; 

number _Sum; 
number _DifoSum; 

integer _Status; 
integer _Layer; 
integer _Position; 




integer _gid; 
char _name[80]; 
#line 42 "trrechit.ddl"
public: 
#ifdef OO_DDL_TRANSLATION
#line 42 "trrechit.ddl"
    ooRef(AMSTrClusterD) pClusterX : copy(delete);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pClusterX */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSTrClusterD) pClusterX(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSTrClusterD)& pClusterX(ooHandle(AMSTrClusterD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pClusterX(const ooHandle(AMSTrClusterD)& objH);
    ooStatus del_pClusterX()
      {
	return opiDelUssoc_1(ooDysoc(), ooAssocN(AMSTrRecHitD,pClusterX));
      }
    ooBoolean exist_pClusterX(const ooHandle(AMSTrClusterD)& objH) const;
    ooBoolean exist_pClusterX() const
      {
        return this->exist_pClusterX((ooHandle(AMSTrClusterD)&) (oocNullHandle));
      }
    static ooAssocNumber pClusterX_ooAssocN;
    ooRef(AMSTrClusterD)& pClusterX(ooRef(AMSTrClusterD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 43 "trrechit.ddl"
    ooRef(AMSTrClusterD) pClusterY : copy(delete);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pClusterY */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSTrClusterD) pClusterY(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSTrClusterD)& pClusterY(ooHandle(AMSTrClusterD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pClusterY(const ooHandle(AMSTrClusterD)& objH);
    ooStatus del_pClusterY()
      {
	return opiDelUssoc_1(ooDysoc(), ooAssocN(AMSTrRecHitD,pClusterY));
      }
    ooBoolean exist_pClusterY(const ooHandle(AMSTrClusterD)& objH) const;
    ooBoolean exist_pClusterY() const
      {
        return this->exist_pClusterY((ooHandle(AMSTrClusterD)&) (oocNullHandle));
      }
    static ooAssocNumber pClusterY_ooAssocN;
    ooRef(AMSTrClusterD)& pClusterY(ooRef(AMSTrClusterD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#line 46 "trrechit.ddl"
AMSTrRecHitD(); 
AMSTrRecHitD(class AMSTrRecHit *); 


inline integer getStatus() { return _Status; }
inline integer getLayer() { return _Layer; }
inline AMSPoint getHits() { return _Hit; }
inline AMSPoint getEHit() { return _EHit; }
inline number getSum() { return _Sum; }
inline number getDSum() { return _DifoSum; }
inline integer getPosition() { return _Position; }
inline integer getgid() { return _gid; }
inline char *getname() { return _name; }
#line 67
void copy(AMSTrRecHit *); 


inline void setgid(integer gid) { _gid = gid; }
inline void setname(char *name) { if (name) strcpy (_name, name); }
inline void setPosition(integer pos) { _Position = pos; }
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSTrRecHitD)& ooThis(ooHandle(AMSTrRecHitD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSTrRecHitD) ooThis(ooMode aMode = oocNoOpen) const;
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
	return opiODMGDatabaseNew(size, ooTypeN(AMSTrRecHitD), odmg_db);
      }	
    void *operator new(size_t  size,
		       d_Database *odmg_db,
		       const char *)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSTrRecHitD), odmg_db);
      }	
#endif
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSTrRecHitD)& ooThis(ooRef(AMSTrRecHitD)& objId, ooMode aMode = oocNoOpen) const;
    AMSTrRecHitD(ooInternalObj iobj);
#line 76 "trrechit.ddl"
}; 

#endif /* !defined(TRRECHIT_H) */
