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
#include "G4Event.hh"
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
//   AMSJob::gethead()->addup(pph);
    AMSJob::gethead()->getg4physics()=pph;
    pmgr->SetUserInitialization(pph);
     AMSG4GeneratorInterface* ppg=new AMSG4GeneratorInterface(CCFFKEY.npat);
//     AMSJob::gethead()->addup(ppg);
    AMSJob::gethead()->getg4generator()=ppg;
     pmgr->SetUserAction(ppg);
//    pmgr->SetUserAction(new AMSG4RunAction);
     pmgr->SetUserAction(new AMSG4EventAction);
//    pmgr->SetUserAction(new AMSG4SteppingAction);

 pmgr->Initialize();



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



#include "G4ParticleGun.hh"
AMSG4GeneratorInterface::AMSG4GeneratorInterface(G4int npart):_npart(npart),_cpart(0),AMSNode(AMSID("AMSG4GeneratorInterface",0)),G4VUserPrimaryGeneratorAction(){
  _particleGun = new G4ParticleGun[_npart];

}



void AMSG4GeneratorInterface::SetParticleGun(int ipart, number mom, AMSPoint  Pos, AMSPoint   Dir){

#ifdef __AMSDEBUG__
assert(_cpart < _npart);
#endif
 G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  const G4String pname(AMSJob::gethead()->getg4physics()->G3toG4(ipart));
_particleGun[_cpart].SetParticleDefinition(particleTable->FindParticle(pname));
_particleGun[_cpart].SetParticleMomentum(mom*GeV);
_particleGun[_cpart].SetParticleMomentumDirection(G4ThreeVector(Dir[0],Dir[1],Dir[2]));
_particleGun[_cpart].SetParticlePosition(G4ThreeVector(Pos[0]*cm,Pos[1]*cm,Pos[2]*cm));
     _cpart++;

}

void AMSG4GeneratorInterface::GeneratePrimaries(G4Event* anEvent)
{
  for(G4int ipart=0;ipart<_cpart;ipart++)_particleGun[ipart].GeneratePrimaryVertex(anEvent);
}


AMSG4GeneratorInterface::~AMSG4GeneratorInterface(){
delete[] _particleGun;
}


void  AMSG4EventAction::BeginOfEventAction(const G4Event*){

AMSJob::gethead()->getg4generator()->Reset();
//gukine_();

}
void  AMSG4EventAction::EndOfEventAction(const G4Event*){

GCFLAG.IEVENT++;
}
