#include "typedefs.h"

#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif

#include "cfortran.h"

#if defined(__ROOTSHAREDLIBRARY__) && defined(_PGTRACK_)
double ROOTRndm(int dummy);
#define RNDM(A) ROOTRndm(A)
extern "C" void ROOTPoissn(float &, int &, int&);
#define POISSN ROOTPoissn
#else
#ifdef __G4AMS__
#define ENABLE_BACKWARDS_COMPATIBILITY
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandPoissonQ.h"
#ifdef G4MULTITHREADED
#include "G4MTHepRandom.hh"
#endif
#include "commons.h"
extern "C" void poissn_(float &, int &, int&);
PROTOCCALLSFFUN1(FLOAT,RNDM,rndm,FLOAT)
#define RNDMG3(A) CCALLSFFUN1(RNDM,rndm,FLOAT,A)
#ifdef G4MULTITHREADED
#define RNDM(A) G4MTHepRandom::getTheEngine()->flat()
//fixme
#define POISSN(A,B,C) C=0;B=CLHEP::RandPoissonQ::shoot(A);
#else
#define RNDM(A) (MISCFFKEY.G4On?(CLHEP::RandFlat::shoot()):RNDMG3(A))
#define POISSN(A,B,C) if(MISCFFKEY.G4On){C=0;B=CLHEP::RandPoissonQ::shoot(A);} else poissn_(A,B,C)
#endif

#else

#ifdef __AMSVMC__
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandPoissonQ.h"
extern "C" void poissn_(float &, int &, int&);
PROTOCCALLSFFUN1(FLOAT,RNDM,rndm,FLOAT)
#define RNDMG3(A) CCALLSFFUN1(RNDM,rndm,FLOAT,A)
#define RNDM(A) (IOPA.VMCVersion==2?(CLHEP::RandFlat::shoot()):RNDMG3(A))
#define POISSN(A,B,C) if(IOPA.VMCVersion==2){C=0;B=CLHEP::RandPoissonQ::shoot(A);} else poissn_(A,B,C)

#else

extern "C" void poissn_(float &, int &, int&);
#define POISSN poissn_
PROTOCCALLSFFUN1(FLOAT,RNDM,rndm,FLOAT)
#define RNDM(A) CCALLSFFUN1(RNDM,rndm,FLOAT,A)

#endif
#endif
#endif
