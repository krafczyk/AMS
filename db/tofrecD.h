/* C++ header file: Objectivity/DB DDL version 3.8.1 */

#ifndef TOFREC_D_H
#define TOFREC_D_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "tofrecD.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <typedefs.h>
#include <point.h>
#include <tofrec.h>

#ifndef TOFREC_D_REF_H
#include "tofrecD_ref.h"
#endif

class AMSBetaD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSBetaD);
class ooItr(AMSBetaD);
class ooRef(AMSBetaD);
class ooShortRef(AMSBetaD);
#endif // !defined(OO_BUGGY_TEMPLATES)

#line 14 "tofrecD.ddl"
class AMSBetaD; 

class AMSTOFClusterD : public ooObj { 


protected: class AMSPoint _Coo; 
AMSPoint _ErrorCoo; 


number _edep; 
number _time; 
number _etime; 

integer _status; 
integer _ntof; 
integer _plane; 

integer _Position; 
#line 35 "tofrecD.ddl"
public: 
#ifdef OO_DDL_TRANSLATION
#line 35 "tofrecD.ddl"
    ooRef(AMSBetaD) pBetaTOF <-> pTOFBeta[];
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for bidirectional association link: pBetaTOF */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSBetaD) pBetaTOF(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSBetaD)& pBetaTOF(ooHandle(AMSBetaD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_pBetaTOF(const ooHandle(AMSBetaD)& objH);
    ooStatus del_pBetaTOF()
      {
	return opiDelAssoc_1(ooDysoc(), ooAssocN(AMSTOFClusterD,pBetaTOF));
      }
    ooBoolean exist_pBetaTOF(const ooHandle(AMSBetaD)& objH) const;
    ooBoolean exist_pBetaTOF() const
      {
        return this->exist_pBetaTOF((ooHandle(AMSBetaD)&) (oocNullHandle));
      }
    static ooAssocNumber pBetaTOF_ooAssocN;
    ooRef(AMSBetaD)& pBetaTOF(ooRef(AMSBetaD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#line 37 "tofrecD.ddl"
static const integer BAD; 
static const integer USED; 

inline integer checkstatus(integer checker) const { return _status & checker; }
inline void setstatus(integer status) { _status = _status | status; }
inline integer getstatus() const { return _status; }
inline integer getntof() const { return _ntof; }
inline integer getplane() const { return _plane; }
inline number gettime() const { return _time; }
inline number getetime() const { return _etime; }
inline number getedep() const { return _edep; }
inline AMSPoint getcoo() const { return _Coo; }
inline AMSPoint getecoo() const { return _ErrorCoo; }

inline integer getPosition() { return _Position; }
inline void setPosition(integer pos) { _Position = pos; }
void copy(class AMSTOFCluster *); 

AMSTOFClusterD(); 
AMSTOFClusterD(AMSTOFCluster *); 
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSTOFClusterD)& ooThis(ooHandle(AMSTOFClusterD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSTOFClusterD) ooThis(ooMode aMode = oocNoOpen) const;
    void* operator new(size_t, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const ooId& nearId);
    void* operator new(size_t, ooConstCPtr(ooObj) nearObj);
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSTOFClusterD)& ooThis(ooRef(AMSTOFClusterD)& objId, ooMode aMode = oocNoOpen) const;
    AMSTOFClusterD(ooInternalObj iobj);
#line 57 "tofrecD.ddl"
}; 

#endif /* !defined(TOFREC_D_H) */
