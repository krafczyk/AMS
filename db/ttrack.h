/* C++ header file: Objectivity/DB DDL version 4.0.2 */

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

class AMSTrRecHitD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTrRecHitD);
class ooItr(AMSTrRecHitD);
class ooRef(AMSTrRecHitD);
class ooShortRef(AMSTrRecHitD);
#endif // !defined(OO_BUGGY_TEMPLATES)

#line 21 "ttrack.ddl"
class AMSTrRecHitD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSTrRecHitD <trrechit_ref.h>
#else
#include <trrechit_ref.h>
#endif
class opiDummyDeclaration;
#line 31
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
#line 75 "ttrack.ddl"
public: 
#ifdef OO_DDL_TRANSLATION
#line 75 "ttrack.ddl"
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
#line 81 "ttrack.ddl"
AMSTrTrackD(); 
AMSTrTrackD(class AMSTrTrack *); 


inline integer getPosition() { return _Position; }
inline integer getPattern() { return _Pattern; }
inline integer getnhits() { return _NHits; }

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
	return opiODMGDatabaseNew(size, ooTypeN(AMSTrTrackD), odmg_db);
      }	
    void *operator new(size_t  size,
		       d_Database *odmg_db,
		       const char *)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSTrTrackD), odmg_db);
      }	
#endif
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSTrTrackD)& ooThis(ooRef(AMSTrTrackD)& objId, ooMode aMode = oocNoOpen) const;
    AMSTrTrackD(ooInternalObj iobj);
#line 93 "ttrack.ddl"
}; 

#endif /* !defined(TTRACK_H) */
