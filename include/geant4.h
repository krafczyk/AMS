//  $Id$
#ifndef __GEANT4H__
#define __GEANT4H__

#include <set>
#include <algorithm>
#include "cern.h"
#include "geantnamespace.h"
#include "point.h"
#include "G4RunManager.hh"
#include "G4VUserDetectorConstruction.hh"

#include "G4MagneticField.hh"
#include "G4VUserPhysicsList.hh"

#include "node.h"

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

    void AddRegisteredTrack(int gtrkid);
    void AddRegisteredParentChild(int gtrkid, int gparentid, int processid);
    void FindClosestRegisteredTrack(int& gtrkid, int& processid);

 private:

    struct track_information {
      track_information()
        : parent(-1)
        , process(-1)
        , registered(false) {}

      track_information(int _parent, int _process, bool _registered)
        : parent(_parent)
        , process(_process)
        , registered(_registered) {}

      int parent;
      int process;
      bool registered;
    };

    map< int, track_information > fmap_det_tracks;
    int flast_trkid;
    int flast_resultid;
    int flast_processid;
};


#include "G4UserRunAction.hh"
class AMSG4RunAction: public G4UserRunAction{

 public:

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

  void DumpCrossSections(int verbose, G4int At = 12, G4int Zt = 6);

};





class AMSG4RotationMatrix: public G4RotationMatrix{

public:
AMSG4RotationMatrix(number _nrm[3][3]);
AMSG4RotationMatrix():G4RotationMatrix(){};
static  void Test();
};



#include "G4UserSteppingAction.hh"

class AMSG4SteppingAction : public G4UserSteppingAction
{
  public:
    virtual void UserSteppingAction(const G4Step*);
 private:
    void FillBackSplash(const G4Step* Step);
    void FillPrimaryInfo(const G4Step* Step);
    static const double facc_pl[21];
};



#include "G4VSensitiveDetector.hh"
class G4Step;

class AMSG4DummySD : public G4VSensitiveDetector
{
protected:
static AMSG4DummySD** _pSD;
public:
  AMSG4DummySD(char * name=0):G4VSensitiveDetector(name==0?"AMSG4dummySD":name){};
  ~AMSG4DummySD() {};

  void Initialize(G4HCofThisEvent*HCE) {};
  G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist) {return false;}
  void EndOfEvent(G4HCofThisEvent*HCE) {};
  static AMSG4DummySD* & pSD(uinteger i=0){return _pSD[i];}
friend class AMSG4DummySDI;
};

class AMSG4DummySDI{
public:
AMSG4DummySDI();
~AMSG4DummySDI();
private:
 static integer _Count;
};
static AMSG4DummySDI AMSG4DummySDI;



#include "G4UserStackingAction.hh"

class AMSG4StackingAction: public G4UserStackingAction{

public:
 AMSG4StackingAction(){};
 virtual ~AMSG4StackingAction(){};
   virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage(){};
    virtual void PrepareNewEvent(){};

};
#include "G4Version.hh"
#if G4VERSION_NUMBER  > 999 
#include "G4VUserActionInitialization.hh"
class AMSG4ActionInitialization: public G4VUserActionInitialization{

 public:
 int _npart;
 AMSG4ActionInitialization(int npart=1);
 virtual ~AMSG4ActionInitialization();
    virtual void BuildForMaster() const;
    virtual void Build() const;

};
#endif
#endif

