//  $Id$
#include "cern.h"
#define ENABLE_BACKWARDS_COMPATIBILITY
#include "CLHEP/Random/Random.h"
#include "CLHEP/Random/RandGeneral.h"
#ifdef G4MULTITHREADED
#include "G4MTHepRandom.hh"
#include "G4MTRandGeneral.hh"
#endif

class AMSRandGeneral{
class service{
public:
#ifdef G4MULTITHREADED
G4MTRandGeneral * _pgenerator;
#else
CLHEP::RandGeneral * _pgenerator;
#endif
integer _id;
geant _xmi;
geant _range;
service * _next;
service(geant parray[],uinteger larray, geant xmi, geant xma, integer id):
_id(id),_xmi(xmi),_range(xma-xmi),_next(0){
 if(larray){
  number * _parray=new number[larray];
  for (unsigned int i=0;i<larray;i++)_parray[i]=parray[i];
#ifdef G4MULTITHREADED
  _pgenerator=new G4MTRandGeneral(_parray,larray);
#else
  _pgenerator=new CLHEP::RandGeneral(_parray,larray);
#endif
  delete[] _parray;
 } 
 else _pgenerator=0;
}
~service(){
 delete _next;
 _next=0;
  delete _pgenerator;
  _pgenerator=0;
}
};
service * _first;
static AMSRandGeneral * _pstatic;
#ifdef _OPENMP
//#pragma omp threadprivate (_pstatic)
#endif
public:
AMSRandGeneral():_first(0){}
~AMSRandGeneral(){delete _first;}
static void book(integer hid);
static void book(geant parray[], uinteger larray, geant xmi,geant xma, integer hid);
static number hrndm1(integer hid);
};


