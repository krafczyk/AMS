#include <job.h>
#include <event.h>
#include <trrec.h>
#include <mccluster.h>
#include <daqevt.h>
#include <mceventg.h>
#include <geant4.h>
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
 extern "C" void getfield_(geant& a);

void g4ams::G4INIT(){
 
G4RunManager * pmgr = new G4RunManager();




//   Detector

    pmgr->SetUserInitialization(new AMSG4DetectorInterface(AMSJob::gethead()->getgeom()->pg4v())); 
   AMSG4Physics * pph = new AMSG4Physics();
//   AMSJob::gethead()->addup(pph);
    AMSJob::gethead()->getg4physics()=pph;
    pmgr->SetUserInitialization(pph);


     pmgr->Initialize();





     AMSG4GeneratorInterface* ppg=new AMSG4GeneratorInterface(CCFFKEY.npat);
     AMSJob::gethead()->getg4generator()=ppg;
     pmgr->SetUserAction(ppg);
     pmgr->SetUserAction(new AMSG4EventAction);
     pmgr->SetUserAction(new AMSG4SteppingAction);
//    pmgr->SetUserAction(new AMSG4RunAction);




}
void g4ams::G4RUN(){
// Initialize GEANT3

    TIMEL(GCTIME.TIMINT);

 G4RunManager::GetRunManager()->BeamOn(GCFLAG.NEVENT);
}


void g4ams::G4LAST(){
delete  G4RunManager::GetRunManager();
}



void AMSG4MagneticField::GetFieldValue(const double x[3], double *B) const{
 int i;
  static integer init=0;
  static geant bb=0;
if(G4FFKEY.UniformMagField){
  if(!init){
    init++;
    getfield_(bb);
  }
  for(i=0;i<2;i++)*(B+i)=0;
  if(fabs(x[2])<40*cm){
   *B=bb*tesla;
  } 
}
else{
   geant _v[3],_b[3];
 for(i=0;i<2;i++)_v[i]=x[i]/cm;
 GUFLD(_v,_b);
 for(i=0;i<2;i++)*(B+i)=_b[i]*kilogauss;

}
//if(B[0])cout <<x[0]<<" "<<x[1]<<" "<<x[2]<<" "<<B[0]<<endl;
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
     G4FieldManager*  fieldMgr =G4TransportationManager::GetTransportationManager()->GetFieldManager();
    if(!pf && G4FFKEY.UniformMagField!=-1){
     AMSG4MagneticField * pf = new AMSG4MagneticField();
     fieldMgr->SetDetectorField(pf);
     G4double delta =G4FFKEY.Delta*cm;
     if(G4FFKEY.BFOrder){
      G4ChordFinder *pchord;
      G4Mag_EqRhs* fEquation = new G4Mag_UsualEqRhs(pf);
      const G4double stepMinimum = 0.01e-2 * mm;
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
  cout << "AMSG4DetectorInterface::Construct-I-Building Geometry "<<endl;
  AMSJob::gethead()->getgeom()->MakeG4Volumes();
  cout << "AMSG4DetectorInterface::Construct-I-"<<AMSgvolume::getNpv()<<" Physical volumes, "<<AMSgvolume::getNlv()<<" logical volumes and "<<AMSgvolume::getNrm()<<" rotation matrixes have beem created "<<endl;
 _pv=AMSJob::gethead()->getgeom()->pg4v();
}
 
 if(!_pv){
   cerr <<"g4ams::G4INIT()-W-DummyDetectorWillBeCreated "<<endl;
   double kfac=1;
   G4LogicalVolume* dummyl=
      new G4LogicalVolume(new G4Box("Box",AMSDBc::ams_size[0]/2*cm,AMSDBc::ams_size[1]/2*cm,AMSDBc::ams_size[2]/2*cm),
      new G4Material("Vacuum",1,g/mole,kfac*universe_mean_density,kStateGas,0.1*kelvin,1.e-19*pascal*kfac),"Vacuum_log",0,0,0);
  _pv=new G4PVPlacement(0,G4ThreeVector(),"AMS",
                        dummyl,0,false,1);

 }
  _pv->GetLogicalVolume()->SetFieldManager(fieldMgr,true);
 return _pv;



}

AMSG4RotationMatrix::AMSG4RotationMatrix(number nrm[3][3]):G4RotationMatrix(nrm[0][0],nrm[0][1],nrm[0][2],nrm[1][0],nrm[1][1],nrm[1][2],nrm[2][0],nrm[2][1],nrm[2][2]){
#ifdef __AMSDEBUG__
 for (int i=0;i<3;i++){
  double norm=0;
  for(int j=0;j<3;j++){
   norm+=nrm[j][i]*nrm[j][i];
  }
  if(fabs(norm-1)>1.e-6){
   cerr <<"AMSG4RotationMatrix::AMSG4RotationMatrix-E-MatrixNotNormalized "<<i<<" "<<norm<<" "<<nrm[0][i]<<" "<<nrm[1][i]<<" "<<nrm[2][i]<<" "<<endl;
   exit(1);
  }
 }
#endif

}

void AMSG4RotationMatrix::Test(){
 
   AMSPoint xp,yp,zp;
   float d;
   number nrm[3][3];
// Test against possible CLHEP changes in the future
   for (int i=0;i<10;i++){
    int j,k;
    for(j=0;j<3;j++){
      xp[j]=RNDM(d);
      yp[j]=RNDM(d);
      zp[j]=RNDM(d);
    }
    AMSDir x(xp);
    AMSDir y(yp);
    AMSDir z(zp);
     for(j=0;j<3;j++){
        nrm[j][0]=x[j];
        nrm[j][1]=y[j];
        nrm[j][2]=z[j];
     }
     AMSG4RotationMatrix mtest(nrm);
     geant r[3],sph,cth,cph,sth,theta[3],phi[3];
     integer rt=0;
     for (j=0;j<3;j++){
      for (k=0;k<3;k++) r[k]=nrm[k][j];
       GFANG(r, cth,  sth, cph, sph,  rt);
       theta[j]=atan2((double)sth,(double)cth);
       phi[j]=atan2((double)sph,(double)cph);
      }
     geant t1[3],p1[3];
     p1[0]=mtest.phiX(); 
     p1[1]=mtest.phiY(); 
     p1[2]=mtest.phiZ(); 
     t1[0]=mtest.thetaX(); 
     t1[1]=mtest.thetaY(); 
     t1[2]=mtest.thetaZ(); 
     AMSPoint P1(p1); 
     AMSPoint T1(t1); 
     AMSPoint Phi(phi); 
     AMSPoint Theta(theta); 
     if(P1.dist(Phi)>1.e-6 ||  T1.dist(Theta)>1.e-6){
      cerr<<"AMSG4RotationMatrix::Test-F-TestFailed at "<<i<<" "<<
      P1<< " " <<Phi <<" "<<T1<< " "<<Theta<<" "<<P1.dist(Phi)<<" "<<T1.dist(Theta)<<endl;
      exit(1);
     }
   }
#ifdef __AMSDEBUG__
     cout <<"AMSG4RotationMatrix::Test-I-TestPassed "<<endl;
#endif
   

}


AMSG4DummySD* AMSG4DummySD::_pSD=0;
integer AMSG4DummySDI::_Count=0;

AMSG4DummySDI::AMSG4DummySDI(){
if(!_Count++){
 AMSG4DummySD::pSD()=new AMSG4DummySD();
}
}

AMSG4DummySDI::~AMSG4DummySDI(){
 if(--_Count==0){
  delete AMSG4DummySD::pSD();
 }
}
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

void AMSG4SteppingAction::UserSteppingAction(const G4Step * Step){


// just do as in example N04
// don't really understand the stuff

/* 
    Some stuff about step
G4StepPoint* GetPreStepPoint() const
void SetPreStepPoint(G4StepPoint* value)
G4StepPoint* GetPostStepPoint() const
void SetPostStepPoint(G4StepPoint* value)
G4double GetStepLength() const
void SetStepLength(G4double value)
G4Track* GetTrack() const
void SetTrack(G4Track* value)
G4ThreeVector GetDeltaPosition() const
G4double GetDeltaTime() const
G4ThreeVector GetDeltaMomentum() const
G4double GetDeltaEnergy() const
G4double GetTotalEnergyDeposit() const
void SetTotalEnergyDeposit(G4double value)
void AddTotalEnergyDeposit(G4double value)
void ResetTotalEnergyDeposit()
G4SteppingControl GetControlFlag() const
void SetControlFlag(G4SteppingControl StepControlFlag)

*/
 

     // Checking if Volume is sensitive one 
    G4StepPoint * PostPoint = Step->GetPostStepPoint();
    G4VPhysicalVolume * PostPV = PostPoint->GetPhysicalVolume();
    if(PostPV){
      cout << "Stepping  "<<" "<<PostPV->GetName()<<" "<<PostPV->GetCopyNo()<<" "<<PostPoint->GetPosition()<<endl;
    GCTMED.isvol=PostPV->GetLogicalVolume()->GetSensitiveDetector()!=0;
    GCTRAK.destep=Step->GetTotalEnergyDeposit()*GeV;
    if(GCTMED.isvol){
      cout << "Stepping  sensitive"<<" "<<PostPV->GetName()<<" "<<PostPV->GetCopyNo()<<" "<<PostPoint->GetPosition()<<endl;
     // gothering some info and put it into geant3 commons

     G4StepPoint * PrePoint = Step->GetPreStepPoint();
     G4VPhysicalVolume * PrePV = PrePoint->GetPhysicalVolume();
     GCTRAK.inwvol= PostPV != PrePV;
     GCTRAK.step=Step->GetStepLength()*cm;
     GCTRAK.vect[0]=PostPoint->GetPosition().x()*cm; 
     GCTRAK.vect[1]=PostPoint->GetPosition().y()*cm; 
     GCTRAK.vect[2]=PostPoint->GetPosition().z()*cm; 
     GCTRAK.vect[3]=PostPoint->GetMomentumDirection().x(); 
     GCTRAK.vect[4]=PostPoint->GetMomentumDirection().y(); 
     GCTRAK.vect[5]=PostPoint->GetMomentumDirection().z(); 
     GCTRAK.getot=PostPoint->GetTotalEnergy()*GeV;
     GCTRAK.gekin=PostPoint->GetKineticEnergy()*GeV;
     GCTRAK.vect[6]=GCTRAK.getot*PostPoint->GetBeta();
     GCTRAK.tofg=PostPoint->GetGlobalTime()*second;
     G4Track * Track = Step->GetTrack();

/* 
     Some stuff about track
 
 
 const G4ThreeVector& GetPosition() const
 void SetPosition(const G4ThreeVector& aValue)
 G4double GetGlobalTime() const
 void SetGlobalTime(const G4double aValue)
 // Time since the event in which the track belongs is created.
 G4double GetLocalTime() const
 void SetLocalTime(const G4double aValue)
 // Time since the current track is created.
 G4double GetProperTime() const
 void SetProperTime(const G4double aValue)
 // Proper time of the current track
 G4double GetTrackLength() const
 void AddTrackLength(const G4double aValue)
 // Accumulated the track length
 G4int GetParentID() const
 void SetParentID(const G4int aValue)
 G4int GetTrackID() const
 void SetTrackID(const G4int aValue)
 G4VPhysicalVolume* GetVolume() const
 G4VPhysicalVolume* GetNextVolume() const
 G4Material* GetMaterial() const
 G4Material* GetNextMaterial() const
 G4VTouchable* GetTouchable() const
 void SetTouchable(G4VTouchable* apValue)
 G4VTouchable* GetNextTouchable() const
 void SetNextTouchable(G4VTouchable* apValue)
 G4double GetKineticEnergy() const
 void SetKineticEnergy(const G4double aValue)
 G4double GetVelocity() const
 const G4ThreeVector& GetMomentumDirection() const
 void SetMomentumDirection(const G4ThreeVector& aValue)
 const G4ThreeVector& GetPolarization() const
 void SetPolarization(const G4ThreeVector& aValue)
 G4TrackStatus GetTrackStatus() const
 void SetTrackStatus(const G4TrackStatus aTrackStatus)
 G4bool IsBelowThreshold() const
 void SetBelowThresholdFlag(G4bool value = true)
 G4bool IsGoodForTracking() const
 void SetGoodForTrackingFlag(G4bool value = true)
 G4int GetCurrentStepNumber() const
 void IncrementCurrentStepNumber()
 G4double GetTotalEnergy() const
 G4ThreeVector GetMomentum() const
 G4DynamicParticle* GetDynamicParticle() const
 G4ParticleDefinition* GetDefinition() const
 G4double GetStepLength() const
 void SetStepLength(G4double value)
 G4Step* GetStep() const
 void SetStep(G4Step* aValue)
 const G4ThreeVector& GetVertexPosition() const
 void SetVertexPosition(const G4ThreeVector& aValue)
 const G4ThreeVector& GetVertexMomentumDirection() const
 void SetVertexMomentumDirection(const G4ThreeVector& aValue)
 G4double GetVertexKineticEnergy() const
 void SetVertexKineticEnergy(const G4double aValue)
 G4LogicalVolume* GetLogicalVolumeAtVertex() const
 void SetLogicalVolumeAtVertex(G4LogicalVolume* )
 const G4VProcess* GetCreatorProcess() const
 void SetCreatorProcess(G4VProcess* aValue)
 G4double GetWeight() const
 void SetWeight(G4double aValue)



*/
   G4ParticleDefinition* particle =Track->GetDefinition();
   GCKINE.ipart=AMSJob::gethead()->getg4physics()->G4toG3(particle->GetParticleName());
   GCKINE.charge=particle->GetPDGCharge();

  try{
  // Now one has decide based on the names of volumes (or their parents)
     G4VPhysicalVolume * Mother=PostPV->GetMother();
     G4VPhysicalVolume * GrandMother= Mother?Mother->GetMother():0;      
  // Tracker
     if(GCTRAK.destep && GrandMother && GrandMother->GetName()[0]=='S' 
     &&  GrandMother->GetName()[1]=='T' && GrandMother->GetName()[2]=='K'){
       cout <<" tracker "<<endl;
      AMSTrMCCluster::sitkhits(PostPV->GetCopyNo(),GCTRAK.vect,
      GCTRAK.destep,GCTRAK.step,GCKINE.ipart);   
     }
  }
   catch (AMSuPoolError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
     AMSEvent::gethead()->Recovery();
     return;
   }
   catch (AMSaPoolError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
    AMSEvent::gethead()->Recovery();
      return;
   }
   }
   cout <<"leaving stepping "<<endl;
  }
}
