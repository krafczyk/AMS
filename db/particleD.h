/* C++ header file: Objectivity/DB DDL version 3.8.1 */

#ifndef PARTICLE_D_H
#define PARTICLE_D_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "particleD.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <typedefs.h>
#include <point.h>
#include <ctc.h>
#include <particle.h>

#ifndef PARTICLE_D_REF_H
#include "particleD_ref.h"
#endif

class AMSEventD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSEventD);
class ooItr(AMSEventD);
class ooRef(AMSEventD);
class ooShortRef(AMSEventD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSBetaD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSBetaD);
class ooItr(AMSBetaD);
class ooRef(AMSBetaD);
class ooShortRef(AMSBetaD);
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

#line 17 "particleD.ddl"
class AMSEventD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSEventD <eventD_ref.h>
#else
#include <eventD_ref.h>
#endif
class opiDummyDeclaration;

class AMSTrTrackD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTrTrackD <ttrack_ref.h>
#else
#include <ttrack_ref.h>
#endif
class opiDummyDeclaration;

class AMSChargeD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSChargeD <chargeD_ref.h>
#else
#include <chargeD_ref.h>
#endif
class opiDummyDeclaration;

class AMSBetaD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSBetaD <tbeta_ref.h>
#else
#include <tbeta_ref.h>
#endif
class opiDummyDeclaration;

class AMSParticleD : public ooObj { 



protected: class AMSPoint _Coo; 

class CTC _Value; 

number _Mass; 
number _ErrMass; 
number _Momentum; 
number _ErrMomentum; 
number _Charge; 
number _Theta; 
number _Phi; 
number _SumAnti; 

integer _GPart; 
integer _Position; 




public: AMSParticleD(); 
AMSParticleD(class AMSParticle *); 
#ifdef OO_DDL_TRANSLATION
#line 56 "particleD.ddl"
    ooRef(AMSEventD) pEventP <-> pParticleE[];
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pEventP */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSEventD) pEventP(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSEventD)& pEventP(ooHandle(AMSEventD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pEventP(const ooHandle(AMSEventD)& objH);
    ooStatus del_pEventP()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSParticleD,pEventP));
      }
    ooBoolean exist_pEventP(const ooHandle(AMSEventD)& objH) const;
    ooBoolean exist_pEventP() const
      {
        return this->exist_pEventP((ooHandle(AMSEventD)&) (oocNullHandle));
      }
    static ooAssocNumber pEventP_ooAssocN;
    ooRef(AMSEventD)& pEventP(ooRef(AMSEventD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 57 "particleD.ddl"
    ooRef(AMSBetaD) pBetaP <-> pParticleB;
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pBetaP */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSBetaD) pBetaP(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSBetaD)& pBetaP(ooHandle(AMSBetaD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pBetaP(const ooHandle(AMSBetaD)& objH);
    ooStatus del_pBetaP()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSParticleD,pBetaP));
      }
    ooBoolean exist_pBetaP(const ooHandle(AMSBetaD)& objH) const;
    ooBoolean exist_pBetaP() const
      {
        return this->exist_pBetaP((ooHandle(AMSBetaD)&) (oocNullHandle));
      }
    static ooAssocNumber pBetaP_ooAssocN;
    ooRef(AMSBetaD)& pBetaP(ooRef(AMSBetaD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 58 "particleD.ddl"
    ooRef(AMSTrTrackD) pTrackP <-> pParticleT;
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pTrackP */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSTrTrackD) pTrackP(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSTrTrackD)& pTrackP(ooHandle(AMSTrTrackD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pTrackP(const ooHandle(AMSTrTrackD)& objH);
    ooStatus del_pTrackP()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSParticleD,pTrackP));
      }
    ooBoolean exist_pTrackP(const ooHandle(AMSTrTrackD)& objH) const;
    ooBoolean exist_pTrackP() const
      {
        return this->exist_pTrackP((ooHandle(AMSTrTrackD)&) (oocNullHandle));
      }
    static ooAssocNumber pTrackP_ooAssocN;
    ooRef(AMSTrTrackD)& pTrackP(ooRef(AMSTrTrackD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 59 "particleD.ddl"
    ooRef(AMSChargeD) pChargeP <-> pParticleC;
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pChargeP */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSChargeD) pChargeP(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSChargeD)& pChargeP(ooHandle(AMSChargeD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pChargeP(const ooHandle(AMSChargeD)& objH);
    ooStatus del_pChargeP()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSParticleD,pChargeP));
      }
    ooBoolean exist_pChargeP(const ooHandle(AMSChargeD)& objH) const;
    ooBoolean exist_pChargeP() const
      {
        return this->exist_pChargeP((ooHandle(AMSChargeD)&) (oocNullHandle));
      }
    static ooAssocNumber pChargeP_ooAssocN;
    ooRef(AMSChargeD)& pChargeP(ooRef(AMSChargeD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#line 61 "particleD.ddl"
void copy(AMSParticle *); 

inline integer getPosition() { return _Position; }
inline void setPosition(integer pos) { _Position = pos; }
inline integer getGPart() { return _GPart; }
inline void setGPart(integer gpart) { _GPart = gpart; }
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSParticleD)& ooThis(ooHandle(AMSParticleD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSParticleD) ooThis(ooMode aMode = oocNoOpen) const;
    void* operator new(size_t, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const ooId& nearId);
    void* operator new(size_t, ooConstCPtr(ooObj) nearObj);
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSParticleD)& ooThis(ooRef(AMSParticleD)& objId, ooMode aMode = oocNoOpen) const;
    AMSParticleD(ooInternalObj iobj);
#line 68 "particleD.ddl"
}; 

#endif /* !defined(PARTICLE_D_H) */
