/* C++ header file: Objectivity/DB DDL version 4.0.2 */

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

class AMSTrMCClusterV;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTrMCClusterV);
class ooItr(AMSTrMCClusterV);
class ooRef(AMSTrMCClusterV);
class ooShortRef(AMSTrMCClusterV);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSTOFMCClusterV;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTOFMCClusterV);
class ooItr(AMSTOFMCClusterV);
class ooRef(AMSTOFMCClusterV);
class ooShortRef(AMSTOFMCClusterV);
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

#line 29 "eventD.ddl"
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

class AMSTOFClusterD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTOFClusterD <tofrecD_ref.h>
#else
#include <tofrecD_ref.h>
#endif
class opiDummyDeclaration;

class AMSTrMCClusterV; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTrMCClusterV <tmcclusterV_ref.h>
#else
#include <tmcclusterV_ref.h>
#endif
class opiDummyDeclaration;

class AMSTOFMCClusterV; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTOFMCClusterV <mctofclusterV_ref.h>
#else
#include <mctofclusterV_ref.h>
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



time_t _time; 
integer _runNumber; 
short _runType; 
uinteger _eventNumber; 
uinteger _triggerMask; 

short _nTrHits; 
short _nTrClusters; 
short _nTOFClusters; 
short _nCTCClusters; 
short _nTracks; 
short _nBetas; 
short _nCharges; 
short _nParticles; 


integer _Position; 
#line 96 "eventD.ddl"
public: 
#ifdef OO_DDL_TRANSLATION
#line 96 "eventD.ddl"
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
#line 98 "eventD.ddl"
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
#line 99 "eventD.ddl"
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
#line 100 "eventD.ddl"
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
#line 101 "eventD.ddl"
    ooRef(AMSTrMCClusterV) pMCCluster : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pMCCluster */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSTrMCClusterV) pMCCluster(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSTrMCClusterV)& pMCCluster(ooHandle(AMSTrMCClusterV)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pMCCluster(const ooHandle(AMSTrMCClusterV)& objH);
    ooStatus del_pMCCluster()
      {
	return opiDelUssoc_1(ooDysoc(), ooAssocN(AMSEventD,pMCCluster));
      }
    ooBoolean exist_pMCCluster(const ooHandle(AMSTrMCClusterV)& objH) const;
    ooBoolean exist_pMCCluster() const
      {
        return this->exist_pMCCluster((ooHandle(AMSTrMCClusterV)&) (oocNullHandle));
      }
    static ooAssocNumber pMCCluster_ooAssocN;
    ooRef(AMSTrMCClusterV)& pMCCluster(ooRef(AMSTrMCClusterV)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 102 "eventD.ddl"
    ooRef(AMSTOFMCClusterV) pTOFMCCluster : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pTOFMCCluster */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSTOFMCClusterV) pTOFMCCluster(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSTOFMCClusterV)& pTOFMCCluster(ooHandle(AMSTOFMCClusterV)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pTOFMCCluster(const ooHandle(AMSTOFMCClusterV)& objH);
    ooStatus del_pTOFMCCluster()
      {
	return opiDelUssoc_1(ooDysoc(), ooAssocN(AMSEventD,pTOFMCCluster));
      }
    ooBoolean exist_pTOFMCCluster(const ooHandle(AMSTOFMCClusterV)& objH) const;
    ooBoolean exist_pTOFMCCluster() const
      {
        return this->exist_pTOFMCCluster((ooHandle(AMSTOFMCClusterV)&) (oocNullHandle));
      }
    static ooAssocNumber pTOFMCCluster_ooAssocN;
    ooRef(AMSTOFMCClusterV)& pTOFMCCluster(ooRef(AMSTOFMCClusterV)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 103 "eventD.ddl"
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
#line 104 "eventD.ddl"
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
#line 105 "eventD.ddl"
    ooRef(AMSmceventgD) pmcEventg : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pmcEventg */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSmceventgD) pmcEventg(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSmceventgD)& pmcEventg(ooHandle(AMSmceventgD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pmcEventg(const ooHandle(AMSmceventgD)& objH);
    ooStatus del_pmcEventg()
      {
	return opiDelUssoc_1(ooDysoc(), ooAssocN(AMSEventD,pmcEventg));
      }
    ooBoolean exist_pmcEventg(const ooHandle(AMSmceventgD)& objH) const;
    ooBoolean exist_pmcEventg() const
      {
        return this->exist_pmcEventg((ooHandle(AMSmceventgD)&) (oocNullHandle));
      }
    static ooAssocNumber pmcEventg_ooAssocN;
    ooRef(AMSmceventgD)& pmcEventg(ooRef(AMSmceventgD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 106 "eventD.ddl"
    ooRef(AMSTrTrackD) pTrack[] : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pTrack[] */
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
    ooStatus sub_pTrack(const ooHandle(AMSTrTrackD)& objH, uint32 number = 1);
    ooStatus del_pTrack()
      {
	return opiDelUssoc(ooDysoc(), ooAssocN(AMSEventD,pTrack));
      }
    ooBoolean exist_pTrack(const ooHandle(AMSTrTrackD)& objH) const;
    ooBoolean exist_pTrack() const
      {
        return this->exist_pTrack((ooHandle(AMSTrTrackD)&) (oocNullHandle));
      }
    static ooAssocNumber pTrack_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 107 "eventD.ddl"
    ooRef(AMSBetaD) pBeta[] : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pBeta[] */
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
    ooStatus sub_pBeta(const ooHandle(AMSBetaD)& objH, uint32 number = 1);
    ooStatus del_pBeta()
      {
	return opiDelUssoc(ooDysoc(), ooAssocN(AMSEventD,pBeta));
      }
    ooBoolean exist_pBeta(const ooHandle(AMSBetaD)& objH) const;
    ooBoolean exist_pBeta() const
      {
        return this->exist_pBeta((ooHandle(AMSBetaD)&) (oocNullHandle));
      }
    static ooAssocNumber pBeta_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 108 "eventD.ddl"
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
#line 109 "eventD.ddl"
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
#line 113 "eventD.ddl"
AMSEventD(); 
AMSEventD(integer, uinteger, integer, time_t); 




inline integer &getrun() { return _runNumber; }
inline short &RunType() { return _runType; }
inline uinteger &getevent() { return _eventNumber; }
inline time_t &TriggerTime() { return _time; }
inline uinteger &Trigmask() { return _triggerMask; }
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
	return opiODMGDatabaseNew(size, ooTypeN(AMSEventD), odmg_db);
      }	
    void *operator new(size_t  size,
		       d_Database *odmg_db,
		       const char *)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSEventD), odmg_db);
      }	
#endif
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSEventD)& ooThis(ooRef(AMSEventD)& objId, ooMode aMode = oocNoOpen) const;
    AMSEventD(ooInternalObj iobj);
#line 167 "eventD.ddl"
}; 

#endif /* !defined(EVENT_D_H) */
