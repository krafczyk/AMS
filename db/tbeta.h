/* C++ header file: Objectivity/DB DDL version 3.8.1 */

#ifndef TBETA_H
#define TBETA_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "tbeta.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <cern.h>
#include <typedefs.h>
#include <beta.h>

#ifndef TBETA_REF_H
#include "tbeta_ref.h"
#endif

class AMSTOFClusterD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTOFClusterD);
class ooItr(AMSTOFClusterD);
class ooRef(AMSTOFClusterD);
class ooShortRef(AMSTOFClusterD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSEventD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSEventD);
class ooItr(AMSEventD);
class ooRef(AMSEventD);
class ooShortRef(AMSEventD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSTrTrackD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTrTrackD);
class ooItr(AMSTrTrackD);
class ooRef(AMSTrTrackD);
class ooShortRef(AMSTrTrackD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSChargeD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSChargeD);
class ooItr(AMSChargeD);
class ooRef(AMSChargeD);
class ooShortRef(AMSChargeD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSParticleD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSParticleD);
class ooItr(AMSParticleD);
class ooRef(AMSParticleD);
class ooShortRef(AMSParticleD);
#endif // !defined(OO_BUGGY_TEMPLATES)

#line 14 "tbeta.ddl"
class AMSTrTrackD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTrTrackD <ttrack_ref.h>
#else
#include <ttrack_ref.h>
#endif
class opiDummyDeclaration;

class AMSEventD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSEventD <eventD_ref.h>
#else
#include <eventD_ref.h>
#endif
class opiDummyDeclaration;

class AMSTOFClusterD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTOFClusterD <tofrecD_ref.h>
#else
#include <tofrecD_ref.h>
#endif
class opiDummyDeclaration;

class AMSChargeD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSChargeD <chargeD_ref.h>
#else
#include <chargeD_ref.h>
#endif
class opiDummyDeclaration;

class AMSParticleD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSParticleD <particleD_ref.h>
#else
#include <particleD_ref.h>
#endif
class opiDummyDeclaration;

class AMSBetaD : public ooObj { 



protected: number _Beta; 
number _InvErrBeta; 
number _Chi2; 

integer _Pattern; 

integer _Position; 
#line 45 "tbeta.ddl"
public: 
#ifdef OO_DDL_TRANSLATION
#line 45 "tbeta.ddl"
    ooRef(AMSTOFClusterD) pTOFBeta[] <-> pBetaTOF;
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pTOFBeta[] */
    ooStatus pTOFBeta(ooItr(AMSTOFClusterD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pTOFBeta(ooItr(AMSTOFClusterD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSBetaD,pTOFBeta),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pTOFBeta(ooItr(AMSTOFClusterD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSBetaD,pTOFBeta),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pTOFBeta(const ooHandle(AMSTOFClusterD)& newObjH);
    ooStatus sub_pTOFBeta(const ooHandle(AMSTOFClusterD)& objH);
    ooStatus del_pTOFBeta()
      {
	return opiDelAssoc(ooDysoc(), ooAssocN(AMSBetaD,pTOFBeta));
      }
    ooBoolean exist_pTOFBeta(const ooHandle(AMSTOFClusterD)& objH) const;
    ooBoolean exist_pTOFBeta() const
      {
        return this->exist_pTOFBeta((ooHandle(AMSTOFClusterD)&) (oocNullHandle));
      }
    static ooAssocNumber pTOFBeta_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 46 "tbeta.ddl"
    ooRef(AMSEventD) pEventB <-> pBeta[];
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pEventB */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSEventD) pEventB(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSEventD)& pEventB(ooHandle(AMSEventD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pEventB(const ooHandle(AMSEventD)& objH);
    ooStatus del_pEventB()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSBetaD,pEventB));
      }
    ooBoolean exist_pEventB(const ooHandle(AMSEventD)& objH) const;
    ooBoolean exist_pEventB() const
      {
        return this->exist_pEventB((ooHandle(AMSEventD)&) (oocNullHandle));
      }
    static ooAssocNumber pEventB_ooAssocN;
    ooRef(AMSEventD)& pEventB(ooRef(AMSEventD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 47 "tbeta.ddl"
    ooRef(AMSTrTrackD) pTrackBeta <-> pBetaT;
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pTrackBeta */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSTrTrackD) pTrackBeta(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSTrTrackD)& pTrackBeta(ooHandle(AMSTrTrackD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pTrackBeta(const ooHandle(AMSTrTrackD)& objH);
    ooStatus del_pTrackBeta()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSBetaD,pTrackBeta));
      }
    ooBoolean exist_pTrackBeta(const ooHandle(AMSTrTrackD)& objH) const;
    ooBoolean exist_pTrackBeta() const
      {
        return this->exist_pTrackBeta((ooHandle(AMSTrTrackD)&) (oocNullHandle));
      }
    static ooAssocNumber pTrackBeta_ooAssocN;
    ooRef(AMSTrTrackD)& pTrackBeta(ooRef(AMSTrTrackD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 48 "tbeta.ddl"
    ooRef(AMSChargeD) pChargeB <-> pBetaCh;
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pChargeB */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSChargeD) pChargeB(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSChargeD)& pChargeB(ooHandle(AMSChargeD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pChargeB(const ooHandle(AMSChargeD)& objH);
    ooStatus del_pChargeB()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSBetaD,pChargeB));
      }
    ooBoolean exist_pChargeB(const ooHandle(AMSChargeD)& objH) const;
    ooBoolean exist_pChargeB() const
      {
        return this->exist_pChargeB((ooHandle(AMSChargeD)&) (oocNullHandle));
      }
    static ooAssocNumber pChargeB_ooAssocN;
    ooRef(AMSChargeD)& pChargeB(ooRef(AMSChargeD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 49 "tbeta.ddl"
    ooRef(AMSParticleD) pParticleB <-> pBetaP;
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pParticleB */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSParticleD) pParticleB(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSParticleD)& pParticleB(ooHandle(AMSParticleD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pParticleB(const ooHandle(AMSParticleD)& objH);
    ooStatus del_pParticleB()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSBetaD,pParticleB));
      }
    ooBoolean exist_pParticleB(const ooHandle(AMSParticleD)& objH) const;
    ooBoolean exist_pParticleB() const
      {
        return this->exist_pParticleB((ooHandle(AMSParticleD)&) (oocNullHandle));
      }
    static ooAssocNumber pParticleB_ooAssocN;
    ooRef(AMSParticleD)& pParticleB(ooRef(AMSParticleD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#line 53 "tbeta.ddl"
AMSBetaD(); 
AMSBetaD(class AMSBeta *); 


inline number getchi2() const { return _Chi2; }
inline number getbeta() const { return _Beta; }
inline number getebeta() const { return _InvErrBeta; }
inline integer getpattern() const { return _Pattern; }

void copy(AMSBeta *); 

inline integer getPosition() { return _Position; }


inline void setPosition(integer position) { _Position = position; }
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSBetaD)& ooThis(ooHandle(AMSBetaD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSBetaD) ooThis(ooMode aMode = oocNoOpen) const;
    void* operator new(size_t, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const ooId& nearId);
    void* operator new(size_t, ooConstCPtr(ooObj) nearObj);
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSBetaD)& ooThis(ooRef(AMSBetaD)& objId, ooMode aMode = oocNoOpen) const;
    AMSBetaD(ooInternalObj iobj);
#line 69 "tbeta.ddl"
}; 

#endif /* !defined(TBETA_H) */
