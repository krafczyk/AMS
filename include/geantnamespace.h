#ifndef __GEANTNAMESPACE__
#define __GEANTNAMESPACE__
#ifdef __G4AMS__

namespace g4ams{
  void G4INIT();
  void G4LAST();
  void G4RUN();
}

#endif
namespace gams{
  void UGINIT();
  void UGLAST();
}

class G4VPhysicalVolume;
typedef G4VPhysicalVolume amsg4pv;
class G4Material;
class G4LogicalVolume;
class HepRotation;
typedef HepRotation  amsg4rm;
#endif
