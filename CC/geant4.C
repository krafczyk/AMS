#include <geant4.h>
#include <job.h>
#include <event.h>
#include <daqevt.h>
#include <trrec.h>
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
 extern "C" void getfield_(geant& );

void g4ams::G4INIT(){
 
G4RunManager * pmgr = new G4RunManager();



// Initialize GEANT3

    TIMEL(GCTIME.TIMINT);

//   Detector

    pmgr->SetUserInitialization(new AMSG4DetectorInterface(AMSJob::gethead()->getgeom()->getg4pv())); 
   AMSG4Physics * pph = new AMSG4Physics();
//   AMSJob::gethead()->addup(pph);
    AMSJob::gethead()->getg4physics()=pph;
    pmgr->SetUserInitialization(pph);


     pmgr->Initialize();





     AMSG4GeneratorInterface* ppg=new AMSG4GeneratorInterface(CCFFKEY.npat);
//     AMSJob::gethead()->addup(ppg);
    AMSJob::gethead()->getg4generator()=ppg;
     pmgr->SetUserAction(ppg);
//    pmgr->SetUserAction(new AMSG4RunAction);
     pmgr->SetUserAction(new AMSG4EventAction);
//    pmgr->SetUserAction(new AMSG4SteppingAction);




}
void g4ams::G4RUN(){
 G4RunManager::GetRunManager()->BeamOn(GCFLAG.NEVENT);
}


void g4ams::G4LAST(){
delete  G4RunManager::GetRunManager();
}



void AMSG4MagneticField::GetFieldValue(const double x[3], double *B) const{
 int i;
 geant _v[3],_b[3];
 for(i=0;i<2;i++)_v[i]=x[i]/cm;
 GUFLD(_v,_b);
 for(i=0;i<2;i++)B[i]=_b[i]*kilogauss;


// Geant4 doesn't work well with non-uniform field at the moment
if(G4FFKEY.UniformMagField){
  static integer init=0;
  geant bb;
  if(!init++){
    getfield_(bb);
  }
  for(i=0;i<2;i++)B[i]=0;
  if(fabs(x[2])<40*cm){
   B[0]=bb*tesla;
  } 
}
//cout <<x[0]<<" "<<x[1]<<" "<<x[2]<<" "<<B[0]<<endl;
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
_particleGun[_cpart].SetParticleEnergy(0);
_particleGun[_cpart].SetParticleMomentum(mom*GeV);
_particleGun[_cpart].SetParticleMomentumDirection(G4ThreeVector(Dir[0],Dir[1],Dir[2]));
_particleGun[_cpart].SetParticlePosition(G4ThreeVector(Pos[0]*cm,Pos[1]*cm,Pos[2]*cm));
     _cpart++;
  
}

void AMSG4GeneratorInterface::GeneratePrimaries(G4Event* anEvent){

static integer event=0;

AMSJob::gethead()->getg4generator()->Reset();


// create new event & initialize it
  if(AMSJob::gethead()->isSimulation()){
    AMSgObj::BookTimer.start("GEANTTRACKING");
   if(IOPA.mode !=1 ){
    AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
    new AMSEvent(AMSID("Event",GCFLAG.IEVENT),CCFFKEY.run,0,0,0)));
    for(integer i=0;i<CCFFKEY.npat;i++){
    AMSmceventg* genp=new AMSmceventg(GCFLAG.NRNDM);
    if(genp){
     AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
     genp->runG4(GCKINE.ikine);
    }
    }
   }
   else {
    AMSIO io;
    if(io.read()){
     AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
     new AMSEvent(AMSID("Event",io.getevent()),io.getrun(),0,io.gettime(),io.getnsec(),
     io.getpolephi(),io.getstheta(),io.getsphi(),io.getveltheta(),
     io.getvelphi(),io.getrad(),io.getyaw(),io.getpitch(),io.getroll(),io.getangvel())));
     AMSmceventg* genp=new AMSmceventg(io);
     if(genp){
      AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
      genp->runG4();
      //genp->_printEl(cout);
     }
    }
    else{
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
     return;
    }   
   }



  for(int ipart=0;ipart<_cpart;ipart++){
//     cout <<_particleGun[ipart].GetParticleDefinition()->GetParticleName()<<endl;
     _particleGun[ipart].GeneratePrimaryVertex(anEvent);
  }

  }

}



AMSG4GeneratorInterface::~AMSG4GeneratorInterface(){
delete[] _particleGun;
}


void  AMSG4EventAction::BeginOfEventAction(const G4Event* anEvent){


 DAQEvent * pdaq=0;
 if(!AMSJob::gethead()->isSimulation()){
    //
    // read daq    
    //
  for(;;){
      pdaq = new DAQEvent();
      if(!(pdaq->read()))break;
      AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
      new AMSEvent(AMSID("Event",pdaq->eventno()),pdaq->runno(),
      pdaq->runtype(),pdaq->time(),pdaq->usec())));
//      pdaq->runtype(),tm,pdaq->usec()))); // tempor introduced to read PC-made files
//<------      
      AMSEvent::gethead()->addnext(AMSID("DAQEvent",pdaq->GetBlType()), pdaq);
   if(GCFLAG.IEORUN==2){
      // if production 
      // try to update the badrun list
         if(AMSJob::gethead()->isProduction() && AMSJob::gethead()->isRealData()){
           char fname[256];
           char * logdir = getenv("ProductionLogDirLocal");
           if(logdir){
            strcpy(fname,logdir);
           }
           else {
             cerr<<"gukine-E-NoProductionLogDirLocalFound"<<endl;
             strcpy(fname,"/Offline/local/logs");
           }
           strcat(fname,"/BadRunsList");
           ofstream ftxt;
           ftxt.open(fname,ios::app);
           if(ftxt){
            ftxt <<pdaq->runno()<<" "<<pdaq->eventno()<<endl;
            ftxt.close();
           }
           else{
            cerr<<"gukine-S-CouldNotOPenFile "<<fname<<endl;
            exit(1);
           }
           
         }
        pdaq->SetEOFIn();    
        GCFLAG.IEORUN=-2;
   }
   else if (GCFLAG.IEORUN==-2){
        GCFLAG.IEORUN=0;
      //  AMSJob::gethead()->uhend();
      //  AMSJob::gethead()->uhinit(pdaq->runno(),pdaq->eventno());
   }
      AMSG4EventAction::EndOfEventAction(anEvent);
      if(GCFLAG.IEOTRI || GCFLAG.IEVENT >= GCFLAG.NEVENT)break;
      GCFLAG.IEVENT++;
  }
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
     return; 
 }   



}



void  AMSG4EventAction::EndOfEventAction(const G4Event* anEvent){

   if(AMSJob::gethead()->isSimulation())
   AMSgObj::BookTimer.stop("GEANTTRACKING");

   try{
          if(AMSEvent::gethead()->HasNoErrors())AMSEvent::gethead()->event();
   }
   catch (AMSuPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSaPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSTrTrackError e){
     cerr << e.getmessage()<<endl;
     cerr <<"Event dump follows"<<endl;
     AMSEvent::gethead()->_printEl(cerr);
     UPool.Release(0);
     AMSEvent::gethead()->remove();
     UPool.Release(1);
     AMSEvent::sethead(0);
      UPool.erase(0);
      return;
   }
      if(GCFLAG.IEVENT%abs(GCFLAG.ITEST)==0 ||     GCFLAG.IEORUN || GCFLAG.IEOTRI || 
         GCFLAG.IEVENT==GCFLAG.NEVENT)
      AMSEvent::gethead()->printA(AMSEvent::debug);
     integer trig;
     if(AMSJob::gethead()->gettriggerOr()){
      trig=0;
     integer ntrig=AMSJob::gethead()->gettriggerN();
       for(int n=0;n<ntrig;n++){
        for(int i=0; ;i++){
         AMSContainer *p=AMSEvent::gethead()->
         getC(AMSJob::gethead()->gettriggerC(n),i);
         if(p)trig+=p->getnelem();
         else break;
        }
       }
     }
     else{
      trig=1;
     integer ntrig=AMSJob::gethead()->gettriggerN();
       for(int n=0;n<ntrig;n++){
        integer trigl=0;
        for(int i=0; ;i++){
         AMSContainer *p=AMSEvent::gethead()->
         getC(AMSJob::gethead()->gettriggerC(n),i);
         if(p)trigl+=p->getnelem();
         else break;
        }
        if(trigl==0)trig=0;
       }
     }
// try to manipulate the conditions for writing....
   if(trig ){ 
     AMSEvent::gethead()->copy();
   }
     AMSEvent::gethead()->write(trig);

     UPool.Release(0);
   AMSEvent::gethead()->remove();
     UPool.Release(1);
   AMSEvent::sethead(0);
   UPool.erase(2000000);

   GCFLAG.IEVENT++;
   if(GCFLAG.IEVENT==GCFLAG.NEVENT){
    GCFLAG.IEOTRI=1;
    GCFLAG.IEORUN=1;
   }
   if(GCFLAG.IEOTRI)G4RunManager::GetRunManager()->AbortRun();
}




 G4VPhysicalVolume* AMSG4DetectorInterface::Construct(){

//Mag Field
    static AMSG4MagneticField * pf=0;
    if(!pf){
     AMSG4MagneticField * pf = new AMSG4MagneticField();
     G4FieldManager* fieldMgr=
       G4TransportationManager::GetTransportationManager()->GetFieldManager();
     fieldMgr->SetDetectorField(pf);
     G4double delta =G4FFKEY.Delta*cm;
     if(G4FFKEY.BFOrder){
      G4ChordFinder *pchord;
      G4Mag_EqRhs* fEquation = new G4Mag_UsualEqRhs(pf);
      const G4double stepMinimum = 0.1e-2 * mm;
      if(G4FFKEY.BFOrder ==3){
        pchord=new G4ChordFinder(pf,stepMinimum,new G4SimpleHeum(fEquation));
      }
      else if(G4FFKEY.BFOrder ==2){
        pchord=new G4ChordFinder(pf,stepMinimum,new G4ImplicitEuler(fEquation));
      }
      else if(G4FFKEY.BFOrder ==1){
       pchord=new G4ChordFinder(pf,stepMinimum,new G4ExplicitEuler(fEquation));
      }
      else if(G4FFKEY.BFOrder==4){
       pchord=new G4ChordFinder(pf,stepMinimum,new G4ClassicalRK4(fEquation));
      }
      else {
       cerr<<"gams::G4INIT-F-No"<<G4FFKEY.BFOrder<<"OrderRunge-KuttaFound"<<endl;
       exit(1);
      }
      fieldMgr->SetChordFinder(pchord);
      cout <<"g4ams::G4INIT-I-"<<G4FFKEY.BFOrder<<"Order Runge-Kutta Selected "<<endl;
     }
     else{
      cout <<"g4ams::G4INIT-I-DefaultTrackingSelected "<<endl;
      fieldMgr->CreateChordFinder(pf);
     }
     fieldMgr->GetChordFinder()->SetDeltaChord(delta);
     cout << "chord "<<fieldMgr->GetChordFinder()->GetDeltaChord()<<endl;
 } 
 if(!_pv){
   cerr <<"g4ams::G4INIT()-W-DummyDetectorWillBeCreated "<<endl;
   double kfac=1;
   G4LogicalVolume* dummyl=
      new G4LogicalVolume(new G4Box("Box",AMSDBc::ams_size[0]/2*cm,AMSDBc::ams_size[1]/2*cm,AMSDBc::ams_size[2]/2*cm),
      new G4Material("Vacuum",1,g/mole,kfac*universe_mean_density,kStateGas,0.1*kelvin,1.e-19*pascal*kfac),"Vacuum_log",0,0,0);
  _pv=new G4PVPlacement(0,G4ThreeVector(),"AMS",
                        dummyl,0,false,0);

 }
 return _pv;



}