// Objectivity class June 06, 1997 ak.
// ( origianal TOFDBcD class by E.Choumilov 2.07.97.)
//
// Last Edit : Sep 15, 1997. 
//
//
#include <string.h>
#include <typedefs.h>
#include <antidbc.h>
//
//
class AntiDBcD : public ooObj{
//
private:
// geom. constants:
  geant   _scradi;   // internal radious of ANTI sc. cylinder
  geant   _scinth;   // thickness of scintillator
  geant   _scleng;   // scintillator paddle length (glob. Z-dim)
  geant   _wrapth;   // wrapper thickness
  geant   _groovr;   // groove radious (corresponds to bump on neigb. pad.
  geant   _pdlgap;   // inter paddle gap
  geant   _stradi;   // support tube radious (internal)
  geant   _stleng;   //              length
  geant   _stthic;   //              thickness
//MC/RECO const:
  geant   _tdcabw;    // bin width (ns) in TovT TDC.
  geant   _shprdt;    // shaper decay time (ns)
  geant   _ftpulw;    // FTrigger pulse width (ns) 

//
public:  
// constructor;
  AntiDBcD();
//
  ooStatus CmpConstants();
};

