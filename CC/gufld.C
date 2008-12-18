
#include "MagField.h"

// GEANT part
#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"






/// Get field value
extern "C" void gufld_(float *x, float *b) {
  MagField::GetPtr()->GuFld(x, b);
}

/// Get field  derivative
extern "C" void tkfld_(float *x, float *b) {
    float bb[3][3];
  
    MagField::GetPtr()->TkFld(x, bb);
    for (int ii=0;ii<3;ii++)
      for (int jj=0;jj<3;jj++){
	(*b)=bb[jj][ii];
	b++;
    }
}

