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

#line 21 "tcluster.ddl"
declare(ooVArray,number)




class AMSTrClusterD : public ooObj { 



class AMSTrIdSoft _Id; 
number _Sum; 
number _Sigma; 
number _Mean; 
number _Rms; 
number _ErrorMean; 

integer _Status; 
integer _NelemL; 
integer _NelemR; 
integer _Position; 

ooVArray(number) _pValues; 


protected: void _printEl(); 
#line 55
public: AMSTrClusterD(); 
AMSTrClusterD(class AMSTrCluster *); 


inline integer getstatus() { return _Status; }
inline integer getnelem() { return - _NelemL + _NelemR; }
inline integer getnelemL() { return _NelemL; }
inline integer getnelemR() { return _NelemR; }
inline number getSigma() { return _Sigma; }
inline number getRms() { return _Rms; }
inline number getVal() { return _Sum; }
inline number getcofg() { return _Mean; }
inline number getecofg() { return _ErrorMean; }
inline integer getPosition() { return _Position; }
inline integer getSide() { return _Id . getside (); }

inline AMSTrIdSoft getidsoft() { return _Id; }

void copy(AMSTrCluster *); 
void getValues(number *); 


inline void setPosition(integer pos) { _Position = pos; }
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
#line 80 "tcluster.ddl"
}; 

#endif /* !defined(TCLUSTER_H) */
