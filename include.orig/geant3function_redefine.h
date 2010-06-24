#ifndef __G3REDEC__
#define __G3REDEC__

#include "TCallf77.h"
extern "C" void g3fpart_(const int&, DEFCHARD, int&, float&, float&, float&, float*, int&);
extern "C" void g3birk_(float&);
extern "C" void g3pions_();           //Need to be expicitly call inorder to change the particle code table
extern "C" void g3physi_();
extern "C" void gufldvmc_(float [], float []);
//extern "C" void g3zinit_();

#define G3PHYSI g3physi_ 
#define GFPART g3fpart_
#define G3BIRK g3birk_
#define GUFLD gufldvmc_
//#define GZINIT g3zinit_

#endif
