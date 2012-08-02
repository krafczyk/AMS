//  $Id: geantnamespace.h,v 1.10 2012/08/02 09:21:57 choutko Exp $
#ifndef __GEANTNAMESPACE__
#define __GEANTNAMESPACE__

#ifdef __AMSVMC__
#include "amsvmc_MCApplication.h"
namespace amsvmc{
  void VMCINIT(amsvmc_MCApplication * _appl);
  void VMCLAST();
  void VMCRUN(amsvmc_MCApplication * _appl);
}
#endif



#ifdef __G4AMS__

namespace g4ams{
  void G4INIT();
  void G4LAST();
  void G4RUN();
  
}
#include <CLHEP/Vector/Rotation.h>
typedef CLHEP::HepRotation  amsg4rm;
#endif
namespace gams{
  void UGINIT(int argc, char **argv );
  void UGLAST(const char *message=0);
  int mem_not_enough(int Thr=102400);
}
class G4VPhysicalVolume;
typedef G4VPhysicalVolume amsg4pv;
class G4Material;
class G4LogicalVolume;
#endif
