/* C++ header file: Objectivity/DB DDL version 3.8.1 */

#ifndef TCLUSTER_H
#define TCLUSTER_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "tcluster.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <typedefs.h>
#include <trid.h>
#include <trrec.h>

#ifndef TCLUSTER_REF_H
#include "tcluster_ref.h"
#endif

class AMSTrRecHitD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSTrRecHitD);
class ooItr(AMSTrRecHitD);
class ooRef(AMSTrRecHitD);
class ooShortRef(AMSTrRecHitD);
#endif // !defined(OO_BUGGY_TEMPLATES)

#line 18 "tcluster.ddl"
declare(ooVArray,number)

class AMSTrRecHitD; 


class AMSTrClusterD : public ooObj { 



number _Sum; 
number _Sigma; 
number _Mean; 
number _Rms; 
number _ErrorMean; 

integer _Status; 
integer _NelemL; 
integer _NelemR; 
integer _Position; 
integer _Side; 

ooVArray(number) _pValues; 


protected: void _printEl(); 
#line 51 "tcluster.ddl"
public: 
#ifdef OO_DDL_TRANSLATION
#line 51 "tcluster.ddl"
    ooRef(AMSTrRecHitD) pTrRecHitX[] <-> pClusterX;
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pTrRecHitX[] */
    ooStatus pTrRecHitX(ooItr(AMSTrRecHitD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pTrRecHitX(ooItr(AMSTrRecHitD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSTrClusterD,pTrRecHitX),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pTrRecHitX(ooItr(AMSTrRecHitD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSTrClusterD,pTrRecHitX),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pTrRecHitX(const ooHandle(AMSTrRecHitD)& newObjH);
    ooStatus sub_pTrRecHitX(const ooHandle(AMSTrRecHitD)& objH);
    ooStatus del_pTrRecHitX()
      {
	return opiDelAssoc(ooDysoc(), ooAssocN(AMSTrClusterD,pTrRecHitX));
      }
    ooBoolean exist_pTrRecHitX(const ooHandle(AMSTrRecHitD)& objH) const;
    ooBoolean exist_pTrRecHitX() const
      {
        return this->exist_pTrRecHitX((ooHandle(AMSTrRecHitD)&) (oocNullHandle));
      }
    static ooAssocNumber pTrRecHitX_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 52 "tcluster.ddl"
    ooRef(AMSTrRecHitD) pTrRecHitY[] <-> pClusterY;
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pTrRecHitY[] */
    ooStatus pTrRecHitY(ooItr(AMSTrRecHitD)& itr, ooMode mode = oocNoOpen) const;
    ooStatus pTrRecHitY(ooItr(AMSTrRecHitD)& itr, const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSTrClusterD,pTrRecHitY),
				    predicate,
				    oocPublic,
				    oocNoOpen);
      }
    ooStatus pTrRecHitY(ooItr(AMSTrRecHitD)& itr,
		 ooMode mode,
		 const ooAccessMode access,
		 const char* predicate) const
      {
	return opiInitAssocVPredItr(((ooHandle(ooObj) &) (itr)),
				    ooDysoc(),
				    ooAssocN(AMSTrClusterD,pTrRecHitY),
				    predicate,
				    access,
				    mode);
      }
    ooStatus add_pTrRecHitY(const ooHandle(AMSTrRecHitD)& newObjH);
    ooStatus sub_pTrRecHitY(const ooHandle(AMSTrRecHitD)& objH);
    ooStatus del_pTrRecHitY()
      {
	return opiDelAssoc(ooDysoc(), ooAssocN(AMSTrClusterD,pTrRecHitY));
      }
    ooBoolean exist_pTrRecHitY(const ooHandle(AMSTrRecHitD)& objH) const;
    ooBoolean exist_pTrRecHitY() const
      {
        return this->exist_pTrRecHitY((ooHandle(AMSTrRecHitD)&) (oocNullHandle));
      }
    static ooAssocNumber pTrRecHitY_ooAssocN;
#endif /* !defined(OO_DDL_TRANSLATION) */
#line 56 "tcluster.ddl"
AMSTrClusterD(); 
AMSTrClusterD(class AMSTrCluster *); 


inline integer getstatus() { return _Status; }
inline integer getnelem() { return _NelemL + _NelemR; }
inline number getSigma() { return _Sigma; }
inline number getRms() { return _Rms; }
inline number getVal() { return _Sum; }
inline number getcofg() { return _Mean; }
inline number getecofg() { return _ErrorMean; }
inline integer getPosition() { return _Position; }
inline integer getSide() { return _Side; }
void copy(AMSTrCluster *); 
void getValues(number *); 


inline void setPosition(integer pos) { _Position = pos; }
inline void setSide(integer side) { _Side = side; }
void setValues(number *); 
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSTrClusterD)& ooThis(ooHandle(AMSTrClusterD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSTrClusterD) ooThis(ooMode aMode = oocNoOpen) const;
    void* operator new(size_t, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const ooId& nearId);
    void* operator new(size_t, ooConstCPtr(ooObj) nearObj);
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSTrClusterD)& ooThis(ooRef(AMSTrClusterD)& objId, ooMode aMode = oocNoOpen) const;
    AMSTrClusterD(ooInternalObj iobj);
#line 77 "tcluster.ddl"
}; 

#endif /* !defined(TCLUSTER_H) */
