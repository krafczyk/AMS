/* C++ header file: Objectivity/DB DDL version 3.8.1 */

#ifndef GVOLUME_D_H
#define GVOLUME_D_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "gvolumeD.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <iostream.h>
#include <string.h>
#include <cern.h>
#include <typedefs.h>
#include <point.h>
#include <gvolume.h>

#ifndef GVOLUME_D_REF_H
#include "gvolumeD_ref.h"
#endif

#line 18 "gvolumeD.ddl"
class AMSgvolumeD : public ooObj { 



protected: integer _id; 
integer _matter; 
integer _rotmno; 
integer _posp; 
integer _gid; 
integer _npar; 
integer _rel; 
char _gonly[5]; 
char _shape[5]; 
geant _par[6]; 
class AMSPoint _coo; 
number _nrm[3][3]; 
number _inrm[3][3]; 

integer _ContPos; 
integer _posN; 
integer _posP; 
integer _posU; 
integer _posD; 



public: ooRef(AMSgvolumeD) _Next; 
ooRef(AMSgvolumeD) _Prev; 
ooRef(AMSgvolumeD) _Up; 
ooRef(AMSgvolumeD) _Down; 

char _name[80]; 


AMSgvolumeD(); 
AMSgvolumeD(integer, class AMSgvolume *, char *, integer); 


void getnrm(number *); 
void getinrm(number *); 
inline integer getid() { return _id; }
inline char *getname() { return _name; }
inline integer getmatter() { return _matter; }
inline integer getrotmno() { return _rotmno; }
inline integer getgid() { return _gid; }
inline integer getposp() { return _posp; }
inline integer getnpar() { return _npar; }
void getshape(char *); 
void getgonly(char *); 
void getpar(geant *); 
void getcoo(AMSPoint *); 

inline integer getNext() { return _posN; }
inline integer getPrev() { return _posP; }
inline integer getUp() { return _posU; }
inline integer getDown() { return _posD; }
inline void setNext(integer pos) { _posN = pos; }
inline void setPrev(integer pos) { _posP = pos; }
inline void setUp(integer pos) { _posU = pos; }
inline void setDown(integer pos) { _posD = pos; }
inline integer getContPos() { return _ContPos; }
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSgvolumeD)& ooThis(ooHandle(AMSgvolumeD)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSgvolumeD) ooThis(ooMode aMode = oocNoOpen) const;
    void* operator new(size_t, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const ooId& nearId);
    void* operator new(size_t, ooConstCPtr(ooObj) nearObj);
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSgvolumeD)& ooThis(ooRef(AMSgvolumeD)& objId, ooMode aMode = oocNoOpen) const;
    AMSgvolumeD(ooInternalObj iobj);
#line 80 "gvolumeD.ddl"
}; 

#endif /* !defined(GVOLUME_D_H) */
