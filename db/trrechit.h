/* C++ header file: Objectivity/DB DDL version 3.8.1 */

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

class AMSTrTrackD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTrTrackD);
class ooItr(AMSTrTrackD);
class ooRef(AMSTrTrackD);
class ooShortRef(AMSTrTrackD);
#endif // !defined(OO_BUGGY_TEMPLATES)

#line 11 "trrechit.ddl"
class AMSTrTrackD; 
class AMSTrClusterD; 


class AMSTrRecHitD : public ooObj { 



class AMSPoint _Hit; 
AMSPoint _EHit; 

number _Sum; 
number _DifoSum; 

integer _Status; 
integer _Layer; 
integer _Position; 
#line 32 "trrechit.ddl"
public: 
#ifdef OO_DDL_TRANSLATION
#line 32 "trrechit.ddl"
    ooRef(AMSTrClusterD) pClusterX <-> pTrRecHitX[];
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pClusterX */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSTrClusterD) pClusterX(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSTrClusterD)& pClusterX(ooHandle(AMSTrClusterD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pClusterX(const ooHandle(AMSTrClusterD)& objH);
    ooStatus del_pClusterX()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSTrRecHitD,pClusterX));
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
#line 33 "trrechit.ddl"
    ooRef(AMSTrClusterD) pClusterY <-> pTrRecHitY[];
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pClusterY */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSTrClusterD) pClusterY(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSTrClusterD)& pClusterY(ooHandle(AMSTrClusterD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pClusterY(const ooHandle(AMSTrClusterD)& objH);
    ooStatus del_pClusterY()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSTrRecHitD,pClusterY));
      }
    ooBoolean exist_pClusterY(const ooHandle(AMSTrClusterD)& objH) const;
    ooBoolean exist_pClusterY() const
      {
        return this->exist_pClusterY((ooHandle(AMSTrClusterD)&) (oocNullHandle));
      }
    static ooAssocNumber pClusterY_ooAssocN;
    ooRef(AMSTrClusterD)& pClusterY(ooRef(AMSTrClusterD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 34 "trrechit.ddl"
    ooRef(AMSTrTrackD) pTrackH[] <-> pTrRecHitT[];
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pTrackH[] */
    ooStatus pTrackH(ooItr(AMSTrTrackD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pTrackH(ooItr(AMSTrTrackD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSTrRecHitD,pTrackH),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pTrackH(ooItr(AMSTrTrackD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSTrRecHitD,pTrackH),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pTrackH(const ooHandle(AMSTrTrackD)& newObjH);
    ooStatus sub_pTrackH(const ooHandle(AMSTrTrackD)& objH, uint32 number = 1);
    ooStatus del_pTrackH()
      {
	return opiDelAssoc(ooDysoc(), ooAssocN(AMSTrRecHitD,pTrackH));
      }
    ooBoolean exist_pTrackH(const ooHandle(AMSTrTrackD)& objH) const;
    ooBoolean exist_pTrackH() const
      {
        return this->exist_pTrackH((ooHandle(AMSTrTrackD)&) (oocNullHandle));
      }
    static ooAssocNumber pTrackH_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#line 37 "trrechit.ddl"
AMSTrRecHitD(); 
AMSTrRecHitD(class AMSTrRecHit *); 


inline integer getStatus() { return _Status; }
inline integer getLayer() { return _Layer; }
inline AMSPoint getHits() { return _Hit; }
inline AMSPoint getEHit() { return _EHit; }
inline number getSum() { return _Sum; }
inline number getDSum() { return _DifoSum; }
inline integer getPosition() { return _Position; }
void copy(AMSTrRecHit *); 


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
    void* operator new(size_t, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const ooId& nearId);
    void* operator new(size_t, ooConstCPtr(ooObj) nearObj);
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSTrRecHitD)& ooThis(ooRef(AMSTrRecHitD)& objId, ooMode aMode = oocNoOpen) const;
    AMSTrRecHitD(ooInternalObj iobj);
#line 55 "trrechit.ddl"
}; 

#endif /* !defined(TRRECHIT_H) */
