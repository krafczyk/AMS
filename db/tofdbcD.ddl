// Objectivity class Oct 08, 1996 ak.
// ( origianal TOFDBcD class by E.Choumilov 13.06.96.)
//
// Last Edit : Mar 21, 1997. 
//
// TOFDBcD should be modified according to the last E.C. version
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
  integer _brtypeD[SCBLMX];  // map of bar types (diff. by length)
  integer _plrotmD[SCLRS];   // TOF planes rotation mask
  geant   _brlenD[SCBTPN];   // bar lengthes for SCBTPN types
  geant   _supstrD[10];      // supp. structure parameters
  geant   _plnstrD[15];      // sc. plane structure param.
// MC/RECO constants:
  geant _edep2phD;     // MC edep(Mev)-to-Photons convertion
  geant _fladctbD;     // MC flash-ADC time binning (ns)
  geant _shaptbD;      // MC shaper pulse time binning
  geant _shrtimD;      // MC shaper pulse rise time (ns)
  geant _shftimD;      // MC shaper pulse fall time (ns)
  geant _tdcbinD[4];   // pipe-line TDC binning for ftdc/stdc/adca/adcd
  geant _trigtbD;      // MC time-binning in logic(trig) pulse handling
  geant _strfluD;      // Stretcher "end-mark" time fluctuations (ns)
  geant _accdelD[2];   // fixed(h/w) lev-1 "accept" delays(ns)(f/s_tdc)
  geant _accdelmxD[2]; // max. lev-1 "accept" delays (incl. above fixed)
  geant _daqpwdD[15];  // MC DAQ-system pulse_widths/dead_times/...
  geant _di2anrD;      // default(MC inp) dinode-to-anode signal ratio
  geant _strratD;      // default(MC inp) stretcher ratio (~10)
//
public:  
// constructor;
  TOFDBcD();
//
  ooStatus CmpConstants();
};

