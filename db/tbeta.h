/* C++ header file: Objectivity/DB DDL version 4.0.2 */

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

class AMSTrTrackD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTrTrackD);
class ooItr(AMSTrTrackD);
class ooRef(AMSTrTrackD);
class ooShortRef(AMSTrTrackD);
#endif // !defined(OO_BUGGY_TEMPLATES)

#line 15 "tbeta.ddl"
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

class AMSBetaD : public ooObj { 



protected: number _Beta; 
number _InvErrBeta; 
number _Chi2; 

integer _Pattern; 

integer _Position; 
#line 37 "tbeta.ddl"
public: 
#ifdef OO_DDL_TRANSLATION
#line 37 "tbeta.ddl"
    ooRef(AMSTOFClusterD) pTOFBeta[] : copy(delete);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pTOFBeta[] */
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
    ooStatus sub_pTOFBeta(const ooHandle(AMSTOFClusterD)& objH, uint32 number = 1);
    ooStatus del_pTOFBeta()
      {
	return opiDelUssoc(ooDysoc(), ooAssocN(AMSBetaD,pTOFBeta));
      }
    ooBoolean exist_pTOFBeta(const ooHandle(AMSTOFClusterD)& objH) const;
    ooBoolean exist_pTOFBeta() const
      {
        return this->exist_pTOFBeta((ooHandle(AMSTOFClusterD)&) (oocNullHandle));
      }
    static ooAssocNumber pTOFBeta_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 38 "tbeta.ddl"
    ooRef(AMSTrTrackD) pTrackBeta : copy(delete);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: pTrackBeta */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSTrTrackD) pTrackBeta(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSTrTrackD)& pTrackBeta(ooHandle(AMSTrTrackD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pTrackBeta(const ooHandle(AMSTrTrackD)& objH);
    ooStatus del_pTrackBeta()
      {
	return opiDelUssoc_1(ooDysoc(), ooAssocN(AMSBetaD,pTrackBeta));
      }
    ooBoolean exist_pTrackBeta(const ooHandle(AMSTrTrackD)& objH) const;
    ooBoolean exist_pTrackBeta() const
      {
        return this->exist_pTrackBeta((ooHandle(AMSTrTrackD)&) (oocNullHandle));
      }
    static ooAssocNumber pTrackBeta_ooAssocN;
    ooRef(AMSTrTrackD)& pTrackBeta(ooRef(AMSTrTrackD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#line 42 "tbeta.ddl"
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
	return opiODMGDatabaseNew(size, ooTypeN(AMSBetaD), odmg_db);
      }	
    void *operator new(size_t  size,
		       d_Database *odmg_db,
		       const char *)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSBetaD), odmg_db);
      }	
#endif
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSBetaD)& ooThis(ooRef(AMSBetaD)& objId, ooMode aMode = oocNoOpen) const;
    AMSBetaD(ooInternalObj iobj);
#line 58 "tbeta.ddl"
}; 

#endif /* !defined(TBETA_H) */
