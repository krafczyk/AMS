// Objectivity class Oct 08, 1996 ak.
// ( origianal TOFDBcD class by E.Choumilov 13.06.96.)
//
// Last Edit : May 23, 1997. 
//
//
#include <string.h>
#include <typedefs.h>
#include <tofdbc.h>
//
//
class TOFDBcD : public ooObj{
//
//  Geometrical constants for TOF-system
//
private:
// geom. constants:
  integer _brtype[SCBLMX];   // map of bar types (diff. by length)
  integer _plrotm[SCLRS];    // TOF planes rotation mask
  geant   _brlen[SCBTPN];    // bar lengthes for SCBTPN types
  geant   _supstr[10];       // supp. structure parameters
  geant   _plnstr[15];       // sc. plane structure param.
// MC/RECO constants:
  geant _edep2ph;     // MC edep(Mev)-to-Photons convertion
  geant _fladctb;     // MC flash-ADC time binning (ns)
  geant _shaptb;      // MC shaper pulse time binning
  geant _shrtim;      // MC shaper pulse rise time (ns)
  geant _shftim;      // MC shaper pulse fall time (ns)
  geant _tdcbin[4];   // pipe-line TDC binning for ftdc/stdc/adca/adcd
  geant _trigtb;      // MC time-binning in logic(trig) pulse handling
  geant _strflu;      // Stretcher "end-mark" time fluctuations (ns)
  geant _daqpwd[15];  // MC DAQ-system pulse_widths/dead_times/...
  geant _di2anr;      // default(MC inp) dinode-to-anode signal ratio
  geant _strrat;      // default(MC inp) stretcher ratio (~10)
  geant _strjit1;
  geant _strjit2;
  geant _ftdelf;
  geant _ftdelm;
  geant _accdel;       // fixed(h/w) lev-1 "accept" delays(ns)(f/s_tdc)
  geant _fstdcd;
  geant _fatdcd;
  geant _pbonup;  

//
public:  
// constructor;
  TOFDBcD();
//
  ooStatus CmpConstants();
};

