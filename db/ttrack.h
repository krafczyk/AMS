/* C++ header file: Objectivity/DB DDL version 3.8.1 */

#ifndef TTRACK_H
#define TTRACK_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "ttrack.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <typedefs.h>
#include <point.h>
#include <trrec.h>

#ifndef TTRACK_REF_H
#include "ttrack_ref.h"
#endif

class AMSEventD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSEventD);
class ooItr(AMSEventD);
class ooRef(AMSEventD);
class ooShortRef(AMSEventD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSTrRecHitD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTrRecHitD);
class ooItr(AMSTrRecHitD);
class ooRef(AMSTrRecHitD);
class ooShortRef(AMSTrRecHitD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSBetaD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSBetaD);
class ooItr(AMSBetaD);
class ooRef(AMSBetaD);
class ooShortRef(AMSBetaD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSParticleD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSParticleD);
class ooItr(AMSParticleD);
class ooRef(AMSParticleD);
class ooShortRef(AMSParticleD);
#endif // !defined(OO_BUGGY_TEMPLATES)

#line 16 "ttrack.ddl"
class AMSEventD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSEventD <eventD_ref.h>
#else
#include <eventD_ref.h>
#endif
class opiDummyDeclaration;

class AMSTrRecHitD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTrRecHitD <trrechit_ref.h>
#else
#include <trrechit_ref.h>
#endif
class opiDummyDeclaration;

class AMSBetaD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSBetaD <tbeta_ref.h>
#else
#include <tbeta_ref.h>
#endif
class opiDummyDeclaration;

class AMSParticleD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSParticleD <particleD_ref.h>
#else
#include <particleD_ref.h>
#endif
class opiDummyDeclaration;


class AMSTrTrackD : public ooObj { 



protected: class AMSPoint _P0; 
AMSPoint _GP0; 
AMSPoint _HP0[2]; 

number _Chi2StrLine; 
number _Chi2Circle; 
number _CircleRidgidity; 
number _Chi2FastFit; 
number _Ridgidity; 
number _ErrRidgidity; 
number _Theta; 
number _Phi; 
number _GChi2; 
number _GRidgidity; 
number _GErrRidgidity; 
number _GTheta; 
number _GPhi; 
number _HChi2[2]; 
number _HRidgidity[2]; 
number _HErrRidgidity[2]; 
number _HTheta[2]; 
number _HPhi[2]; 
number _Chi2MS; 
number _GChi2MS; 
number _RidgidityMS; 
number _GRidgidityMS; 

integer _Status; 
integer _Pattern; 
integer _NHits; 
integer _FastFitDone; 
integer _GeaneFitDone; 
integer _AdvancedFitDone; 

integer _Position; 
#line 72 "ttrack.ddl"
public: 
#ifdef OO_DDL_TRANSLATION
#line 72 "ttrack.ddl"
    ooRef(AMSEventD) pEventT <-> pTrack[];
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pEventT */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSEventD) pEventT(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSEventD)& pEventT(ooHandle(AMSEventD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pEventT(const ooHandle(AMSEventD)& objH);
    ooStatus del_pEventT()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSTrTrackD,pEventT));
      }
    ooBoolean exist_pEventT(const ooHandle(AMSEventD)& objH) const;
    ooBoolean exist_pEventT() const
      {
        return this->exist_pEventT((ooHandle(AMSEventD)&) (oocNullHandle));
      }
    static ooAssocNumber pEventT_ooAssocN;
    ooRef(AMSEventD)& pEventT(ooRef(AMSEventD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 74 "ttrack.ddl"
    ooRef(AMSTrRecHitD) pTrRecHitT[] : copy(delete);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pTrRecHitT[] */
    ooStatus pTrRecHitT(ooItr(AMSTrRecHitD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pTrRecHitT(ooItr(AMSTrRecHitD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSTrTrackD,pTrRecHitT),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pTrRecHitT(ooItr(AMSTrRecHitD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSTrTrackD,pTrRecHitT),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pTrRecHitT(const ooHandle(AMSTrRecHitD)& newObjH);
    ooStatus sub_pTrRecHitT(const ooHandle(AMSTrRecHitD)& objH, uint32 number = 1);
    ooStatus del_pTrRecHitT()
      {
	return opiDelUssoc(ooDysoc(), ooAssocN(AMSTrTrackD,pTrRecHitT));
      }
    ooBoolean exist_pTrRecHitT(const ooHandle(AMSTrRecHitD)& objH) const;
    ooBoolean exist_pTrRecHitT() const
      {
        return this->exist_pTrRecHitT((ooHandle(AMSTrRecHitD)&) (oocNullHandle));
      }
    static ooAssocNumber pTrRecHitT_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 75 "ttrack.ddl"
    ooRef(AMSBetaD) pBetaT <-> pTrackBeta;
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pBetaT */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSBetaD) pBetaT(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSBetaD)& pBetaT(ooHandle(AMSBetaD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pBetaT(const ooHandle(AMSBetaD)& objH);
    ooStatus del_pBetaT()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSTrTrackD,pBetaT));
      }
    ooBoolean exist_pBetaT(const ooHandle(AMSBetaD)& objH) const;
    ooBoolean exist_pBetaT() const
      {
        return this->exist_pBetaT((ooHandle(AMSBetaD)&) (oocNullHandle));
      }
    static ooAssocNumber pBetaT_ooAssocN;
    ooRef(AMSBetaD)& pBetaT(ooRef(AMSBetaD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 76 "ttrack.ddl"
    ooRef(AMSParticleD) pParticleT <-> pTrackP;
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pParticleT */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSParticleD) pParticleT(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSParticleD)& pParticleT(ooHandle(AMSParticleD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pParticleT(const ooHandle(AMSParticleD)& objH);
    ooStatus del_pParticleT()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSTrTrackD,pParticleT));
      }
    ooBoolean exist_pParticleT(const ooHandle(AMSParticleD)& objH) const;
    ooBoolean exist_pParticleT() const
      {
        return this->exist_pParticleT((ooHandle(AMSParticleD)&) (oocNullHandle));
      }
    static ooAssocNumber pParticleT_ooAssocN;
    ooRef(AMSParticleD)& pParticleT(ooRef(AMSParticleD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#line 80 "ttrack.ddl"
AMSTrTrackD(); 
AMSTrTrackD(class AMSTrTrack *); 


inline integer getPosition() { return _Position; }
inline integer getPattern() { return _Pattern; }
void copy(AMSTrTrack *); 


inline void setPosition(integer pos) { _Position = pos; }
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSTrTrackD)& ooThis(ooHandle(AMSTrTrackD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSTrTrackD) ooThis(ooMode aMode = oocNoOpen) const;
    void* operator new(size_t, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const ooId& nearId);
    void* operator new(size_t, ooConstCPtr(ooObj) nearObj);
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSTrTrackD)& ooThis(ooRef(AMSTrTrackD)& objId, ooMode aMode = oocNoOpen) const;
    AMSTrTrackD(ooInternalObj iobj);
#line 90 "ttrack.ddl"
}; 

#endif /* !defined(TTRACK_H) */
