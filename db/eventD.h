/* C++ header file: Objectivity/DB DDL version 3.8.1 */

#ifndef EVENT_D_H
#define EVENT_D_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "eventD.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <typedefs.h>

#ifndef EVENT_D_REF_H
#include "eventD_ref.h"
#endif

class AMSTrClusterD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTrClusterD);
class ooItr(AMSTrClusterD);
class ooRef(AMSTrClusterD);
class ooShortRef(AMSTrClusterD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSTrRecHitD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTrRecHitD);
class ooItr(AMSTrRecHitD);
class ooRef(AMSTrRecHitD);
class ooShortRef(AMSTrRecHitD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSTOFClusterD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTOFClusterD);
class ooItr(AMSTOFClusterD);
class ooRef(AMSTOFClusterD);
class ooShortRef(AMSTOFClusterD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSCTCClusterD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSCTCClusterD);
class ooItr(AMSCTCClusterD);
class ooRef(AMSCTCClusterD);
class ooShortRef(AMSCTCClusterD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSTrMCClusterD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTrMCClusterD);
class ooItr(AMSTrMCClusterD);
class ooRef(AMSTrMCClusterD);
class ooShortRef(AMSTrMCClusterD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSTOFMCClusterD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTOFMCClusterD);
class ooItr(AMSTOFMCClusterD);
class ooRef(AMSTOFMCClusterD);
class ooShortRef(AMSTOFMCClusterD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSCTCMCClusterD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSCTCMCClusterD);
class ooItr(AMSCTCMCClusterD);
class ooRef(AMSCTCMCClusterD);
class ooShortRef(AMSCTCMCClusterD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSAntiMCClusterD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSAntiMCClusterD);
class ooItr(AMSAntiMCClusterD);
class ooRef(AMSAntiMCClusterD);
class ooShortRef(AMSAntiMCClusterD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSmceventgD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSmceventgD);
class ooItr(AMSmceventgD);
class ooRef(AMSmceventgD);
class ooShortRef(AMSmceventgD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSTrTrackD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTrTrackD);
class ooItr(AMSTrTrackD);
class ooRef(AMSTrTrackD);
class ooShortRef(AMSTrTrackD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSBetaD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSBetaD);
class ooItr(AMSBetaD);
class ooRef(AMSBetaD);
class ooShortRef(AMSBetaD);
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

#line 24 "eventD.ddl"
class AMSTrRecHitD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTrRecHitD <trrechit_ref.h>
#else
#include <trrechit_ref.h>
#endif
class opiDummyDeclaration;

class AMSTrClusterD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTrClusterD <tcluster_ref.h>
#else
#include <tcluster_ref.h>
#endif
class opiDummyDeclaration;

class AMSTrTrackD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTrTrackD <ttrack_ref.h>
#else
#include <ttrack_ref.h>
#endif
class opiDummyDeclaration;

class AMSTrMCClusterD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTrMCClusterD <tmccluster_ref.h>
#else
#include <tmccluster_ref.h>
#endif
class opiDummyDeclaration;

class AMSTOFClusterD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTOFClusterD <tofrecD_ref.h>
#else
#include <tofrecD_ref.h>
#endif
class opiDummyDeclaration;

class AMSTOFMCClusterD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTOFMCClusterD <mctofclusterD_ref.h>
#else
#include <mctofclusterD_ref.h>
#endif
class opiDummyDeclaration;

class AMSCTCClusterD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSCTCClusterD <ctcrecD_ref.h>
#else
#include <ctcrecD_ref.h>
#endif
class opiDummyDeclaration;

class AMSAntiMCClusterD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSAntiMCClusterD <mcanticlusterD_ref.h>
#else
#include <mcanticlusterD_ref.h>
#endif
class opiDummyDeclaration;

class AMSCTCMCClusterD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSCTCMCClusterD <mcctcclusterD_ref.h>
#else
#include <mcctcclusterD_ref.h>
#endif
class opiDummyDeclaration;

class AMSBetaD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSBetaD <tbeta_ref.h>
#else
#include <tbeta_ref.h>
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

class AMSmceventgD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSmceventgD <mceventgD_ref.h>
#else
#include <mceventgD_ref.h>
#endif
class opiDummyDeclaration;

class AMSEventD : public ooObj { 



class ooVString fID; 

time_t _Timestamp; 
integer _run; 
short _runtype; 
short fErrorCode; 
uinteger fEventNumber; 
uinteger fTriggerMask; 

short _nTrHits; 
short _nTrClusters; 
short _nTOFClusters; 
short _nCTCClusters; 
short _nTracks; 
short _nBetas; 
short _nCharges; 
short _nParticles; 


integer _Position; 
#line 94 "eventD.ddl"
public: 
#ifdef OO_DDL_TRANSLATION
#line 94 "eventD.ddl"
    ooRef(AMSTrClusterD) pCluster[] : copy(delete);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pCluster[] */
    ooStatus pCluster(ooItr(AMSTrClusterD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pCluster(ooItr(AMSTrClusterD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pCluster),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pCluster(ooItr(AMSTrClusterD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pCluster),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pCluster(const ooHandle(AMSTrClusterD)& newObjH);
    ooStatus sub_pCluster(const ooHandle(AMSTrClusterD)& objH, uint32 number = 1);
    ooStatus del_pCluster()
      {
	return opiDelUssoc(ooDysoc(), ooAssocN(AMSEventD,pCluster));
      }
    ooBoolean exist_pCluster(const ooHandle(AMSTrClusterD)& objH) const;
    ooBoolean exist_pCluster() const
      {
        return this->exist_pCluster((ooHandle(AMSTrClusterD)&) (oocNullHandle));
      }
    static ooAssocNumber pCluster_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 96 "eventD.ddl"
    ooRef(AMSTrRecHitD) pTrRecHitS[] : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pTrRecHitS[] */
    ooStatus pTrRecHitS(ooItr(AMSTrRecHitD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pTrRecHitS(ooItr(AMSTrRecHitD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pTrRecHitS),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pTrRecHitS(ooItr(AMSTrRecHitD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pTrRecHitS),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pTrRecHitS(const ooHandle(AMSTrRecHitD)& newObjH);
    ooStatus sub_pTrRecHitS(const ooHandle(AMSTrRecHitD)& objH, uint32 number = 1);
    ooStatus del_pTrRecHitS()
      {
	return opiDelUssoc(ooDysoc(), ooAssocN(AMSEventD,pTrRecHitS));
      }
    ooBoolean exist_pTrRecHitS(const ooHandle(AMSTrRecHitD)& objH) const;
    ooBoolean exist_pTrRecHitS() const
      {
        return this->exist_pTrRecHitS((ooHandle(AMSTrRecHitD)&) (oocNullHandle));
      }
    static ooAssocNumber pTrRecHitS_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 97 "eventD.ddl"
    ooRef(AMSTOFClusterD) pTOFCluster[] : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pTOFCluster[] */
    ooStatus pTOFCluster(ooItr(AMSTOFClusterD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pTOFCluster(ooItr(AMSTOFClusterD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pTOFCluster),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pTOFCluster(ooItr(AMSTOFClusterD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pTOFCluster),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pTOFCluster(const ooHandle(AMSTOFClusterD)& newObjH);
    ooStatus sub_pTOFCluster(const ooHandle(AMSTOFClusterD)& objH, uint32 number = 1);
    ooStatus del_pTOFCluster()
      {
	return opiDelUssoc(ooDysoc(), ooAssocN(AMSEventD,pTOFCluster));
      }
    ooBoolean exist_pTOFCluster(const ooHandle(AMSTOFClusterD)& objH) const;
    ooBoolean exist_pTOFCluster() const
      {
        return this->exist_pTOFCluster((ooHandle(AMSTOFClusterD)&) (oocNullHandle));
      }
    static ooAssocNumber pTOFCluster_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 98 "eventD.ddl"
    ooRef(AMSCTCClusterD) pCTCCluster[] : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pCTCCluster[] */
    ooStatus pCTCCluster(ooItr(AMSCTCClusterD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pCTCCluster(ooItr(AMSCTCClusterD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pCTCCluster),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pCTCCluster(ooItr(AMSCTCClusterD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pCTCCluster),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pCTCCluster(const ooHandle(AMSCTCClusterD)& newObjH);
    ooStatus sub_pCTCCluster(const ooHandle(AMSCTCClusterD)& objH, uint32 number = 1);
    ooStatus del_pCTCCluster()
      {
	return opiDelUssoc(ooDysoc(), ooAssocN(AMSEventD,pCTCCluster));
      }
    ooBoolean exist_pCTCCluster(const ooHandle(AMSCTCClusterD)& objH) const;
    ooBoolean exist_pCTCCluster() const
      {
        return this->exist_pCTCCluster((ooHandle(AMSCTCClusterD)&) (oocNullHandle));
      }
    static ooAssocNumber pCTCCluster_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 99 "eventD.ddl"
    ooRef(AMSTrMCClusterD) pMCCluster[] : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pMCCluster[] */
    ooStatus pMCCluster(ooItr(AMSTrMCClusterD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pMCCluster(ooItr(AMSTrMCClusterD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pMCCluster),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pMCCluster(ooItr(AMSTrMCClusterD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pMCCluster),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pMCCluster(const ooHandle(AMSTrMCClusterD)& newObjH);
    ooStatus sub_pMCCluster(const ooHandle(AMSTrMCClusterD)& objH, uint32 number = 1);
    ooStatus del_pMCCluster()
      {
	return opiDelUssoc(ooDysoc(), ooAssocN(AMSEventD,pMCCluster));
      }
    ooBoolean exist_pMCCluster(const ooHandle(AMSTrMCClusterD)& objH) const;
    ooBoolean exist_pMCCluster() const
      {
        return this->exist_pMCCluster((ooHandle(AMSTrMCClusterD)&) (oocNullHandle));
      }
    static ooAssocNumber pMCCluster_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 100 "eventD.ddl"
    ooRef(AMSTOFMCClusterD) pTOFMCCluster[] : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pTOFMCCluster[] */
    ooStatus pTOFMCCluster(ooItr(AMSTOFMCClusterD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pTOFMCCluster(ooItr(AMSTOFMCClusterD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pTOFMCCluster),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pTOFMCCluster(ooItr(AMSTOFMCClusterD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pTOFMCCluster),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pTOFMCCluster(const ooHandle(AMSTOFMCClusterD)& newObjH);
    ooStatus sub_pTOFMCCluster(const ooHandle(AMSTOFMCClusterD)& objH, uint32 number = 1);
    ooStatus del_pTOFMCCluster()
      {
	return opiDelUssoc(ooDysoc(), ooAssocN(AMSEventD,pTOFMCCluster));
      }
    ooBoolean exist_pTOFMCCluster(const ooHandle(AMSTOFMCClusterD)& objH) const;
    ooBoolean exist_pTOFMCCluster() const
      {
        return this->exist_pTOFMCCluster((ooHandle(AMSTOFMCClusterD)&) (oocNullHandle));
      }
    static ooAssocNumber pTOFMCCluster_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 101 "eventD.ddl"
    ooRef(AMSCTCMCClusterD) pCTCMCCluster[] : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pCTCMCCluster[] */
    ooStatus pCTCMCCluster(ooItr(AMSCTCMCClusterD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pCTCMCCluster(ooItr(AMSCTCMCClusterD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pCTCMCCluster),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pCTCMCCluster(ooItr(AMSCTCMCClusterD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pCTCMCCluster),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pCTCMCCluster(const ooHandle(AMSCTCMCClusterD)& newObjH);
    ooStatus sub_pCTCMCCluster(const ooHandle(AMSCTCMCClusterD)& objH, uint32 number = 1);
    ooStatus del_pCTCMCCluster()
      {
	return opiDelUssoc(ooDysoc(), ooAssocN(AMSEventD,pCTCMCCluster));
      }
    ooBoolean exist_pCTCMCCluster(const ooHandle(AMSCTCMCClusterD)& objH) const;
    ooBoolean exist_pCTCMCCluster() const
      {
        return this->exist_pCTCMCCluster((ooHandle(AMSCTCMCClusterD)&) (oocNullHandle));
      }
    static ooAssocNumber pCTCMCCluster_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 102 "eventD.ddl"
    ooRef(AMSAntiMCClusterD) pAntiMCCluster[] : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pAntiMCCluster[] */
    ooStatus pAntiMCCluster(ooItr(AMSAntiMCClusterD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pAntiMCCluster(ooItr(AMSAntiMCClusterD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pAntiMCCluster),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pAntiMCCluster(ooItr(AMSAntiMCClusterD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pAntiMCCluster),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pAntiMCCluster(const ooHandle(AMSAntiMCClusterD)& newObjH);
    ooStatus sub_pAntiMCCluster(const ooHandle(AMSAntiMCClusterD)& objH, uint32 number = 1);
    ooStatus del_pAntiMCCluster()
      {
	return opiDelUssoc(ooDysoc(), ooAssocN(AMSEventD,pAntiMCCluster));
      }
    ooBoolean exist_pAntiMCCluster(const ooHandle(AMSAntiMCClusterD)& objH) const;
    ooBoolean exist_pAntiMCCluster() const
      {
        return this->exist_pAntiMCCluster((ooHandle(AMSAntiMCClusterD)&) (oocNullHandle));
      }
    static ooAssocNumber pAntiMCCluster_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 103 "eventD.ddl"
    ooRef(AMSmceventgD) pmcEventg[] : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pmcEventg[] */
    ooStatus pmcEventg(ooItr(AMSmceventgD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pmcEventg(ooItr(AMSmceventgD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pmcEventg),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pmcEventg(ooItr(AMSmceventgD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pmcEventg),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pmcEventg(const ooHandle(AMSmceventgD)& newObjH);
    ooStatus sub_pmcEventg(const ooHandle(AMSmceventgD)& objH, uint32 number = 1);
    ooStatus del_pmcEventg()
      {
	return opiDelUssoc(ooDysoc(), ooAssocN(AMSEventD,pmcEventg));
      }
    ooBoolean exist_pmcEventg(const ooHandle(AMSmceventgD)& objH) const;
    ooBoolean exist_pmcEventg() const
      {
        return this->exist_pmcEventg((ooHandle(AMSmceventgD)&) (oocNullHandle));
      }
    static ooAssocNumber pmcEventg_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 104 "eventD.ddl"
    ooRef(AMSTrTrackD) pTrack[] <-> pEventT : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pTrack[] */
    ooStatus pTrack(ooItr(AMSTrTrackD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pTrack(ooItr(AMSTrTrackD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pTrack),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pTrack(ooItr(AMSTrTrackD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pTrack),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pTrack(const ooHandle(AMSTrTrackD)& newObjH);
    ooStatus sub_pTrack(const ooHandle(AMSTrTrackD)& objH);
    ooStatus del_pTrack()
      {
	return opiDelAssoc(ooDysoc(), ooAssocN(AMSEventD,pTrack));
      }
    ooBoolean exist_pTrack(const ooHandle(AMSTrTrackD)& objH) const;
    ooBoolean exist_pTrack() const
      {
        return this->exist_pTrack((ooHandle(AMSTrTrackD)&) (oocNullHandle));
      }
    static ooAssocNumber pTrack_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 105 "eventD.ddl"
    ooRef(AMSBetaD) pBeta[] <-> pEventB : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pBeta[] */
    ooStatus pBeta(ooItr(AMSBetaD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pBeta(ooItr(AMSBetaD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pBeta),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pBeta(ooItr(AMSBetaD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pBeta),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pBeta(const ooHandle(AMSBetaD)& newObjH);
    ooStatus sub_pBeta(const ooHandle(AMSBetaD)& objH);
    ooStatus del_pBeta()
      {
	return opiDelAssoc(ooDysoc(), ooAssocN(AMSEventD,pBeta));
      }
    ooBoolean exist_pBeta(const ooHandle(AMSBetaD)& objH) const;
    ooBoolean exist_pBeta() const
      {
        return this->exist_pBeta((ooHandle(AMSBetaD)&) (oocNullHandle));
      }
    static ooAssocNumber pBeta_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 106 "eventD.ddl"
    ooRef(AMSChargeD) pChargeE[] <-> pEventCh : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pChargeE[] */
    ooStatus pChargeE(ooItr(AMSChargeD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pChargeE(ooItr(AMSChargeD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pChargeE),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pChargeE(ooItr(AMSChargeD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pChargeE),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pChargeE(const ooHandle(AMSChargeD)& newObjH);
    ooStatus sub_pChargeE(const ooHandle(AMSChargeD)& objH);
    ooStatus del_pChargeE()
      {
	return opiDelAssoc(ooDysoc(), ooAssocN(AMSEventD,pChargeE));
      }
    ooBoolean exist_pChargeE(const ooHandle(AMSChargeD)& objH) const;
    ooBoolean exist_pChargeE() const
      {
        return this->exist_pChargeE((ooHandle(AMSChargeD)&) (oocNullHandle));
      }
    static ooAssocNumber pChargeE_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 107 "eventD.ddl"
    ooRef(AMSParticleD) pParticleE[] <-> pEventP : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pParticleE[] */
    ooStatus pParticleE(ooItr(AMSParticleD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pParticleE(ooItr(AMSParticleD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pParticleE),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pParticleE(ooItr(AMSParticleD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSEventD,pParticleE),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pParticleE(const ooHandle(AMSParticleD)& newObjH);
    ooStatus sub_pParticleE(const ooHandle(AMSParticleD)& objH);
    ooStatus del_pParticleE()
      {
	return opiDelAssoc(ooDysoc(), ooAssocN(AMSEventD,pParticleE));
      }
    ooBoolean exist_pParticleE(const ooHandle(AMSParticleD)& objH) const;
    ooBoolean exist_pParticleE() const
      {
        return this->exist_pParticleE((ooHandle(AMSParticleD)&) (oocNullHandle));
      }
    static ooAssocNumber pParticleE_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#line 111 "eventD.ddl"
AMSEventD(); 
AMSEventD(uinteger, integer, integer, time_t, char *); 




inline integer &RunNumber() { return _run; }
inline short &RunType() { return _runtype; }
inline uinteger &EventNumber() { return fEventNumber; }
inline time_t &TriggerTime() { return _Timestamp; }
inline uinteger &Trigmask() { return fTriggerMask; }
inline short &ErrorCode() { return fErrorCode; }
inline const char *GetID() { return fID; }
inline integer getPosition() { return _Position; }
void getNumbers(integer &, integer &, uinteger &, time_t &); 


inline void incTrHits() { _nTrHits ++; }
inline void decTrHits() { _nTrHits --; }
inline short &TrHits() { return _nTrHits; }

inline void incTrClusters() { _nTrClusters ++; }
inline void decTrClusters() { _nTrClusters --; }
inline short &TrClusters() { return _nTrClusters; }

inline void incTOFClusters() { _nTOFClusters ++; }
inline void decTOFClusters() { _nTOFClusters --; }
inline short &TOFClusters() { return _nTOFClusters; }

inline void incCTCClusters() { _nCTCClusters ++; }
inline void decCTCClusters() { _nCTCClusters --; }
inline short &CTCClusters() { return _nCTCClusters; }

inline void incTracks() { _nTracks ++; }
inline void decTracks() { _nTracks --; }
inline short &Tracks() { return _nTracks; }

inline void incBetas() { _nBetas ++; }
inline void decBetas() { _nBetas --; }
inline short &Betas() { return _nBetas; }

inline void incCharges() { _nCharges ++; }
inline void decCharges() { _nCharges --; }
inline short &Charges() { return _nCharges; }

inline void incParticles() { _nParticles ++; }
inline void decParticles() { _nParticles --; }
inline short &Particles() { return _nParticles; }



inline void setPosition(integer position) { _Position = position; }


void Print(); 
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSEventD)& ooThis(ooHandle(AMSEventD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSEventD) ooThis(ooMode aMode = oocNoOpen) const;
    void* operator new(size_t, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const ooId& nearId);
    void* operator new(size_t, ooConstCPtr(ooObj) nearObj);
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSEventD)& ooThis(ooRef(AMSEventD)& objId, ooMode aMode = oocNoOpen) const;
    AMSEventD(ooInternalObj iobj);
#line 167 "eventD.ddl"
}; 

#endif /* !defined(EVENT_D_H) */
