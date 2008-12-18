//  $Id: extC.h,v 1.22 2008/12/18 11:19:24 pzuccon Exp $
// Author V. Choutko 24-may-1996
//
// Oct 06, 1996. ak. add readDB 
// Oct 16, 1996. ak. add readGeomDB, writeGeomDB
// Oct 03, 1997. ak.
//
// Last Edit :  Oct 3, 1997. ak.
//
#include "typedefs.h"
//
extern "C" number rnormx();
extern "C" number sitkfints_(number &,number &);
extern "C" number sitkfint2_(number&,number&,number&,number&,number&);
extern "C" void uglast_();
extern "C" void uginit_();
extern "C" void gustep_();
extern "C" void guout_();
extern "C" void gukine_();


#ifndef _PGTRACK_
extern "C" void trafit_(integer & ifit, geant x[],  geant y[], geant wxy[],
                       geant z[],  geant ssz[], integer & npt,
                       geant resxy[], geant ressz[], integer & iflag,
                       geant spcor [], number work[], geant & chixy, 
                       geant &chiz,  geant & xmom, geant & exmom,
                        geant p0[], geant & dip, geant & phis);
#define TRAFIT trafit_
extern "C" void tkgetres_(geant res[][7], integer &nplan);
#define TKGETRES tkgetres_
extern "C" void tkfitg_(integer &npt,geant hits[][3], geant sigma[][3],
geant normal[][3], integer &ipart,  integer &ialgo, integer &ims, integer layer[], geant out[]);
#define TKFITG tkfitg_
extern "C" void tkfini_();
#define TKFINI tkfini_

extern "C" void tkfitpar_(geant init[7], geant & chrg, geant point[6],
                geant out[7], number m55[5][5], geant & step);
#define TKFITPAR tkfitpar_

extern "C" void tkfitparcyl_(geant init[7], geant & chrg, geant point[7],
                geant out[7], number m55[5][5], geant & step);
#define TKFITPARCYL tkfitparcyl_
#endif


extern "C" void fit_(geant arr[][8],integer fixpar[][8],geant &chi2m,integer &alg,integer &what, geant xf[],geant chi2[][2],geant &rigmin, integer &itermin);
#define FIT fit_
extern "C" void fite_(geant arr[][20],integer fixpar[][20],geant &chi2m,integer &alg,integer &what, geant xf[],geant chi2[][2],geant &rigmin, integer &itermin);
#define FITE fite_
extern "C" void dinv_(int &ndim, double matrix[], int &ndim2, 
		          int ridaux[], int &ifail);
#define DINV dinv_

extern "C" void invertmatrix_(double matrix[], int & dim0, int & dim1, int &ifail);
#define INVERTMATRIX invertmatrix_


extern "C" void initDB();
extern "C" void readDB();
extern "C" void readSetup();
extern "C" void writeSetup();
extern "C" void readmfield_();
#define READMFIELD readmfield_

extern "C" void cminit_();
extern "C" void cmgene_(geant & pmu, geant & themu, geant & phimu, 
                       geant & chargemu, geant & xmu, geant & ymu, geant &zmu);
extern "C" void cmgene2_(geant & pmu, geant & gamma, geant & chmu);
extern "C" void cmend_();
#define CMEND cmend_
#define CMGENE cmgene_
#define CMGENE2 cmgene2_
#define CMINIT cminit_

extern "C" void btempcor();
#define BTEMPCOR btempcor_
