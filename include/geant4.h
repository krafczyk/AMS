#ifndef __GEANT4H__
#define __GEANT4H__
#include <cern.h>
#include <geantnamespace.h>
#include "G4RunManager.hh"
#include "G4VUserDetectorConstruction.hh"

#include "globals.hh"
#include "G4MagneticField.hh"
#include "G4VUserPhysicsList.hh"


class G4VPhysicalVolume;
class AMSG4DetectorInterface: public G4VUserDetectorConstruction{
protected:
G4VPhysicalVolume * _pv;
public:
 AMSG4DetectorInterface(G4VPhysicalVolume * pv=0):_pv(pv){}
 G4VPhysicalVolume* Construct(){return _pv;}
};


class AMSG4MagneticField : public G4MagneticField{
  public:

    void GetFieldValue( const  G4double Point[3],
                               G4double *Bfield ) const;
};




#endif
