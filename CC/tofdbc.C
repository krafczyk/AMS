// Author E.Choumilov 14.06.96.
#include <tofdbc.h>
//
integer TOFDBc::debug=1;
//
// Initialize TOF geometry parameters
//
//---> bar lengthes (cm) for each of SCBTPN types :
const number TOFDBc::_brlen[SCBTPN]={
  72.6,99.6,118.6,130.6,135.4
};
//---> bar types  in layers (4 layers top->bot):          
const integer TOFDBc::_brtype[SCLRS][SCMXBR]={
  {1,2,3,4,5,5,5,5,5,5,4,3,2,1},
  {1,2,3,4,5,5,5,5,5,5,4,3,2,1},
  {1,2,3,4,5,5,5,5,5,5,4,3,2,1},
  {1,2,3,4,5,5,5,5,5,5,4,3,2,1}
};
//---> name list for bar t-calibration files:
const char TOFDBc::_brfnam[SCBTPN][6]={
  {'1','0','7','2','6','\0'},
  {'1','0','9','9','6','\0'},
  {'1','1','1','8','6','\0'},
  {'1','1','3','0','6','\0'},
  {'1','1','3','5','4','\0'}
};
//---> plane rotation mask for 4 layers (1/0 -> rotated/not):
const integer TOFDBc::_plrotm[SCLRS]={
  0,1,1,0
};
//---> honeycomb supporting str. data:
const number TOFDBc::_supstr[10]={
   58.,    // i=1    Z-coo of TOP honeycomb (BOT-side)
  -58.,    //  =2    Z-coo    BOT           (TOP     )
   0.,0.,  //  =3,4  X/Y-shifts of TOP honeycomb centre in Mother r.s.
   0.,0.,  //  =5,6                BOT
   12.,78.,//  =7,8  DZ/Router of both honeycomb supports
   0.,0.   //  =9,10 spare
};
//---> sc. plane design parameters:
const number TOFDBc::_plnstr[15]={
  0.,0.,  // i=1,2  Z-gap between honeycomb and outer/inner scint. planes
    1.25,  // i=3    Z-shift of even/odd bars in plane (0 -> flat plane)
     0.6,  // i=4    X(Y)-overlaping(incl. sc_cover) of even/odd bars (0 for flat)
     11., // i=5    sc. bar width
      1., // i=6    sc. bar thickness
    0.05, // i=7    sc_cover thickness (carb.fiber)
 10.,10.,2.5, // i=8-10 width/length/thickness (x/y/z) of PMT shielding box.
  5.,         // i=11 dist. in x-y plane from sc. edge to PMT box.
  0.7,        // i=12 Z-shift of sc.bar/PMT-box centers (depends on par. i=10)
  0.,0.,0.    // i=13-15 spare
};
//---> tof MC const-parameters (float) :
const number TOFDBc::_mcfpar[10]={
  8000.,   // i=1  Mev to photons convertion factor in scintillator.
  0.19,    // i=2  PMT mean quantum efficiency
  0.14,    // i=3  PMT transition time spread (ns)
  0.,0.,0.,0.,0.,0.,0.
};
