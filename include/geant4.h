#ifndef __GEANT4H__
#define __GEANT4H__
#include <cern.h>
#include <geantnamespace.h>
#include <point.h>
#include "G4RunManager.hh"
#include "G4VUserDetectorConstruction.hh"

#include "globals.hh"
#include "G4MagneticField.hh"
#include "G4VUserPhysicsList.hh"

#include <node.h>

class G4VPhysicalVolume;
class AMSG4DetectorInterface: public G4VUserDetectorConstruction{
protected:
G4VPhysicalVolume * _pv;
public:
 AMSG4DetectorInterface(G4VPhysicalVolume * pv=0):_pv(pv){}
 G4VPhysicalVolume* Construct();
};


class AMSG4MagneticField : public G4MagneticField{
  public:

    void GetFieldValue( const  G4double Point[3],
                               G4double *Bfield ) const;
};




#include "G4VUserPrimaryGeneratorAction.hh"
class G4ParticleGun;
class G4Event;

class AMSG4GeneratorInterface: public AMSNode, public G4VUserPrimaryGeneratorAction{

public:
    AMSG4GeneratorInterface(G4int npart=1);
    virtual ~AMSG4GeneratorInterface();
    void GeneratePrimaries(G4Event* anEvent);
    void SetParticleGun(int part, number mom, AMSPoint  Pos,  AMSPoint Dir);
    void Reset(){_cpart=0;}
private:
    G4int        _npart;
    G4int        _cpart;
    G4ParticleGun* _particleGun;
    void _init(){};

};


#include "G4UserEventAction.hh"
class AMSG4EventAction: public G4UserEventAction{

public:
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

};


#endif
