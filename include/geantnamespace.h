//  $Id: geantnamespace.h,v 1.7 2001/01/22 17:32:43 choutko Exp $
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
  void UGINIT(int argc, char **argv );
  void UGLAST(const char *message=0);
}

class G4VPhysicalVolume;
typedef G4VPhysicalVolume amsg4pv;
class G4Material;
class G4LogicalVolume;
class HepRotation;
typedef HepRotation  amsg4rm;
#endif
