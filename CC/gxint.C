// Author V. Choutko 24-may-1996
 
#include <iostream.h>
#include <cern.h>
#include <gvolume.h>
const int NWPAW=1000000;
struct PAWC_DEF{
float q[NWPAW];
};
const int NWGEAN=3000000;
struct GCBANK_DEF{
float q[NWGEAN];
};
//
#define GCBANK COMMON_BLOCK(GCBANK,gcbank)
COMMON_BLOCK_DEF(GCBANK_DEF,GCBANK);
GCBANK_DEF GCBANK;

#define PAWC COMMON_BLOCK(PAWC,pawc)
COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
PAWC_DEF PAWC;
//
main(){
   AMSgvolume::debug=0;
  GPAW(NWGEAN,NWPAW);
return 0;
}
