#include <geant4.h>
#include <job.h>
#include <g4physics.h>
#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4SimpleHeum.hh"
#include "G4ImplicitEuler.hh"
#include "G4ExplicitEuler.hh"
#include "G4ClassicalRK4.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"


void g4ams::G4INIT(){
 
G4RunManager * pmgr = new G4RunManager();

//Mag Field

    AMSG4MagneticField * pf = new AMSG4MagneticField();
    G4FieldManager* fieldMgr=
      G4TransportationManager::GetTransportationManager()->GetFieldManager();
    fieldMgr->SetDetectorField(pf);
    G4ChordFinder *pchord;
    G4Mag_EqRhs* fEquation = new G4Mag_UsualEqRhs(pf);
    G4double delta =G4FFKEY.Delta*cm;
    const G4double stepMinimum = 1.0e-2 * mm;
    if(G4FFKEY.BFOrder ==3){
       pchord=new G4ChordFinder(pf,stepMinimum,new G4SimpleHeum(fEquation));
    }
    else if(G4FFKEY.BFOrder ==2){
      pchord=new G4ChordFinder(pf,stepMinimum,new G4ImplicitEuler(fEquation));
    }
    else if(G4FFKEY.BFOrder ==1){
     pchord=new G4ChordFinder(pf,stepMinimum,new G4ExplicitEuler(fEquation));
    }
    else{
     pchord=new G4ChordFinder(pf,stepMinimum,new G4ClassicalRK4(fEquation));
    }
     pchord->SetDeltaChord(delta);
     fieldMgr->SetChordFinder(pchord);
     cout <<"g4ams::G4INIT-I-"<<G4FFKEY.BFOrder<<"Order Runge-Kutta Selected "<<fieldMgr->GetChordFinder()->GetDeltaChord()<<endl;





//   Detector

if(AMSJob::gethead()->getgeom()->getg4pv()){
    pmgr->SetUserInitialization(new AMSG4DetectorInterface(AMSJob::gethead()->getgeom()->getg4pv())); 
}
else{
cerr <<"g4ams::G4INIT()-W-DummyDetectorWillBeCreated "<<endl;

  G4LogicalVolume* dummyl
    = new G4LogicalVolume(new G4Box(" ",10.*m,10.*m,10.*m),
      new G4Material("Vacuuum",1,g/mole,0)," ",0,0,0);
  G4VPhysicalVolume* pgv=
  new G4PVPlacement(0,G4ThreeVector(),"Dummy",
                        dummyl,0,false,0);

    pmgr->SetUserInitialization(new AMSG4DetectorInterface(pgv)); 

}
   AMSG4Physics * pph = new AMSG4Physics();
   AMSJob::gethead()->addup(pph);
   pmgr->SetUserInitialization(pph);

  // set mandatory user action class
//  pmgr->SetUserAction(new ExN01PrimaryGeneratorAction);


 pmgr->Initialize();


// addup

}
void g4ams::G4RUN(){
 G4RunManager::GetRunManager()->BeamOn(GCFLAG.NEVENT);
}


void g4ams::G4LAST(){
delete  G4RunManager::GetRunManager();
}



void AMSG4MagneticField::GetFieldValue(const G4double x[3], G4double *B) const{
int i;
geant _v[3],_b[3];
for(i=0;i<2;i++)_v[i]=x[i]/cm;
GUFLD(_v,_b);
for(i=0;i<2;i++)B[i]=_b[i]*kilogauss;
}