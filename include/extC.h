// Author V. Choutko 24-may-1996
//
// Oct 06, 1996. ak. add readDB 
// Oct 16, 1996. ak. add readGeomDB, writeGeomDB
//
// Last Edit :  Oct 16, 1996. ak.
#include <typedefs.h>
extern "C" number rnormx();
extern "C" number sitkfint_s_(number &,number &);
extern "C" number sitkfint2_(number&,number&,number&,number&,number&);
extern "C" void uglast_();
extern "C" void uginit_();
extern "C" void gustep_();
extern "C" void guout_();
extern "C" void gukine_();
extern "C" void trafit_(integer & ifit, geant x[],  geant y[], geant wxy[],
                       geant z[],  geant ssz[], integer & npt,
                       geant resxy[], geant ressz[], integer & iflag,
                       geant spcor [], number work[], geant & chixy, 
                       geant &chiz,  geant & xmom, geant & dip, geant & phis);
#define TRAFIT trafit_
extern "C" void tkfitg_(integer &npt,geant hits[][3], geant sigma[][3],
geant normal[][3], integer &ipart,  integer &ialgo, integer &ims, geant out[]);
#define TKFITG tkfitg_

extern "C" void tkfini_();
#define TKFINI tkfini_

extern "C" void tkfitpar_(geant init[7], geant & chrg, geant point[6],
                geant out[7], number m55[5][5], geant & step);
#define TKFITPAR tkfitpar_

extern "C" void readDB();
extern "C" void readGeomDB();
extern "C" void writeGeomDB();
