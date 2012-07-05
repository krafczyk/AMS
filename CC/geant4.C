//  $Id: geant4.C,v 1.87 2012/07/05 14:06:26 pzuccon Exp $
#include "job.h"
#include "event.h"
#include "trrec.h"
#include "richdbc.h"
#include "richid.h"
#ifdef _PGTRACK_
#include "MagField.h"
#include "TrSim.h"
#else
#include "trmccluster.h"
#endif
#include "mccluster.h"
#include "daqevt.h"
#include "mceventg.h"
#include "geant4.h"
#include "astring.h"
#include "g4physics.h"
#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4SimpleHeum.hh"
#include "G4ImplicitEuler.hh"
#include "G4ExplicitEuler.hh"
#include "G4ClassicalRK4.hh"
#include "G4TransportationManager.hh"
#include "G4Material.hh"
#include "G4MaterialScanner.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Event.hh"
#include "G4PVPlacement.hh"
#include "G4StateManager.hh"
#include "G4ApplicationState.hh"
#include "G4VPhysicalVolume.hh"
#include "g4xray.h"
#include "producer.h"
#include "TofSimUtil.h"
#include "Tofsim02.h"
#include "g4tof.h"
#include "g4rich.h"
#ifdef G4VIS_USE
#include "g4visman.h"
#endif
 extern "C" void getfield_(geant& a);

AMSG4Physics * pph = new AMSG4Physics();

void g4ams::G4INIT(){

// Initialize Random Number Generator


HepRandom::setTheEngine(new RanecuEngine());
long seed[3]={0,0,0};
seed[0]=GCFLAG.NRNDM[0];
seed[1]=GCFLAG.NRNDM[1];
HepRandom::setTheSeeds(seed);

 
G4RunManager * pmgr = new G4RunManager();




//   Detector

    pmgr->SetUserInitialization(new AMSG4DetectorInterface(AMSJob::gethead()->getgeom()->pg4v())); 

    //   AMSG4Physics * pph = new AMSG4Physics();
    //   AMSJob::gethead()->addup(pph);
    //    pph->SetCuts();
    AMSJob::gethead()->getg4physics()=pph;
    pmgr->SetUserInitialization(pph);


     pmgr->Initialize();

     cout<<"~~~~~~Dump All Geant4 range cut: "<<endl;
     pph->DumpCutValuesTable();



     AMSG4GeneratorInterface* ppg=new AMSG4GeneratorInterface(CCFFKEY.npat);
     AMSJob::gethead()->getg4generator()=ppg;
     pmgr->SetUserAction(ppg);
     pmgr->SetUserAction(new AMSG4EventAction);
     pmgr->SetUserAction(new AMSG4RunAction);
     pmgr->SetUserAction(new AMSG4SteppingAction);
     pmgr->SetUserAction(new AMSG4StackingAction);
//    pmgr->SetUserAction(new AMSG4RunAction);


#ifdef G4VIS_USE
   AMSG4VisManager::create();
#endif

}
void g4ams::G4RUN(){
  if (MISCFFKEY.DoMatScan==1) {
    G4MaterialScanner *scanner=new G4MaterialScanner();
    scanner->SetNTheta(1);
    scanner->SetThetaMin(MISCFFKEY.StartScanTheta*deg);
    scanner->SetThetaSpan(0*deg);
    scanner->SetNPhi(1);
    scanner->SetPhiMin(MISCFFKEY.StartScanPhi*deg);
    scanner->SetPhiSpan(0*deg);
    scanner->SetRegionName("DefaultRegionForTheWorld");
    cout << scanner->GetRegionName() << endl;
    cout<<"~~~~~~~~~~~~~~~Start scanning the materials"<<endl;
    float X,Y,Z;
    X=  MISCFFKEY.StartScanXstart;
    while ( X<=MISCFFKEY.StartScanXstop){
      Y=  MISCFFKEY.StartScanYstart;
      while ( Y<=MISCFFKEY.StartScanYstop){
	Z=  MISCFFKEY.StartScanZstart;
	while ( Z<=MISCFFKEY.StartScanZstop){
	  scanner->SetEyePosition(G4ThreeVector(X*cm,Y*cm,Z*cm));
	  cout << " X= " << X << " Y= " << Y << " Z= " << Z << endl;
	  scanner->Scan();
	  Z+= MISCFFKEY.StartScanZstep;
	}
	Y+= MISCFFKEY.StartScanYstep;
      }
      X+= MISCFFKEY.StartScanXstep;
    }
    
    cout<<"~~~~~~~~~~~~~~~Finish scanning the materials _____________________________"<<endl;
  }

// Initialize GEANT3
    
    GCFLAG.IEVENT=1;
    TIMEL(GCTIME.TIMINT);
#ifndef G4VIS_USE
   G4RunManager::GetRunManager()->BeamOn(GCFLAG.NEVENT-GCFLAG.IEVENT+1);
#endif
}


void g4ams::G4LAST(){
        if(MISCFFKEY.G3On)GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
        else {
          GCFLAG.NRNDM[0]=HepRandom::getTheSeeds()[0];
          GCFLAG.NRNDM[1]=HepRandom::getTheSeeds()[1];
        }
cout <<"           **** RANDOM NUMBER GENERATOR AFTER LAST COMPLETE EVENT "<<GCFLAG.NRNDM[0]<<" "<<GCFLAG.NRNDM[1]<<endl;
float TIMLFT;
 TIMEL(TIMLFT);
 geant xmean  = (GCTIME.TIMINT - TIMLFT)/GCFLAG.IEVENT;
  cout <<"           **** NUMBER OF EVENTS PROCESSED = "<<GCFLAG.IEVENT<<endl;
  cout <<"           **** TIME TO PROCESS ONE EVENT IS = "<<xmean<<" SECONDS "<<endl;
delete  G4RunManager::GetRunManager();
#ifdef G4VIS_USE
AMSG4VisManager::kill();
#endif
}



void AMSG4MagneticField::GetFieldValue(const double x[3], double *B) const{
 int i;
   geant _v[3],_b[3];
 for(i=0;i<3;i++)_v[i]=x[i]/cm;
 GUFLD(_v,_b);
 for(i=0;i<3;i++)*(B+i)=_b[i]*kilogauss;

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
   if(IOPA.mode%10 !=1 ){
    AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
    new AMSEvent(AMSID("Event",GCFLAG.IEVENT),CCFFKEY.run,0,0,0)));
    for(integer i=0;i<CCFFKEY.npat;i++){
        if(MISCFFKEY.G3On)GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
        else {
          GCFLAG.NRNDM[0]=HepRandom::getTheSeeds()[0];
          GCFLAG.NRNDM[1]=HepRandom::getTheSeeds()[1];
        }
        AMSmceventg* genp=new AMSmceventg(GCFLAG.NRNDM);
    if(genp){
     AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
     genp->runG4(GCKINE.ikine);
    }
    }
   }
   else {
    AMSIO io;
    if(IOPA.mode/10?io.readA():io.read()){
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



void  AMSG4RunAction::BeginOfRunAction(const G4Run* anRun){


  cout<<"~~~~~~~~~~~~~~~~Begin of Run Action, Construct G3G4 Tables here~~~~~~~~~~~~~~"<<endl;

  pph->_init();

}
void  AMSG4RunAction::EndOfRunAction(const G4Run* anRun){

  cout<<"~~~~~~~~~~~~~~~~End of Run Action~~~~~~~~~~~~~~"<<endl;



}




void  AMSG4EventAction::BeginOfEventAction(const G4Event* anEvent){


 DAQEvent * pdaq=0;
 if(!AMSJob::gethead()->isSimulation()){
    //
    // read daq    
    //
    DAQEvent * pdaq=0;
    DAQEvent::InitResult res=DAQEvent::init();
  for(;;){
     if(res==DAQEvent::OK){ 
       pdaq = new DAQEvent();
       if(pdaq->read()){
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
  else{
#ifdef __CORBA__
    try{
     AMSJob::gethead()->uhend();
     AMSProducer::gethead()->sendRunEnd(res);
     AMSProducer::gethead()->getRunEventInfo(AMSProducer::gethead()->IsSolo());
    }
    catch (AMSClientError a){
     cerr<<a.getMessage()<<endl;
     break;
    }
#else
     break;
#endif
    }
   }
   else{
#ifdef __CORBA__
    try{
     AMSJob::gethead()->uhend();
     AMSProducer::gethead()->sendRunEnd(res);
     AMSProducer::gethead()->getRunEventInfo(AMSProducer::gethead()->IsSolo());
    }
    catch (AMSClientError a){
     cerr<<a.getMessage()<<endl;
     break;
    }
#else
     break;
#endif
}
}  
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
     return; 
 }   



}



void  AMSG4EventAction::EndOfEventAction(const G4Event* anEvent){
//   cout <<" guout in"<<endl;
   if(AMSJob::gethead()->isSimulation())
   AMSgObj::BookTimer.stop("GEANTTRACKING");

      CCFFKEY.curtime=AMSEvent::gethead()->gettime();
   try{
          if(anEvent && AMSEvent::gethead()->HasNoErrors())AMSEvent::gethead()->event();
   }
   catch (AMSuPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->Recovery();
#ifdef __CORBA__
#pragma omp critical (g1)
    AMSProducer::gethead()->AddEvent();
#endif
      return;
   }
   catch (AMSaPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->Recovery();
#ifdef __CORBA__
#pragma omp critical (g1)
    AMSProducer::gethead()->AddEvent();
#endif
      return;
   }
   catch (AMSTrTrackError e){
     cerr << e.getmessage()<<endl;
     cerr <<"Event dump follows"<<endl;
     AMSEvent::gethead()->_printEl(cerr);
      AMSEvent::gethead()->seterror();
/*
     UPool.Release(0);
     AMSEvent::gethead()->remove();
     UPool.Release(1);
     AMSEvent::sethead(0);
      UPool.erase(0);
*/
   }
   catch (amsglobalerror e){
     cerr << e.getmessage()<<endl;
     cerr <<"Event dump follows"<<endl;
     AMSEvent::gethead()->_printEl(cerr);
      AMSEvent::gethead()->seterror(e.getlevel());
      if(e.getlevel()>2)throw e; 
/*
     UPool.Release(0);
     AMSEvent::gethead()->remove();
     UPool.Release(1);
     AMSEvent::sethead(0);
      UPool.erase(0);
*/
   }
#ifdef __CORBA__
#pragma omp critical (g1)
    AMSProducer::gethead()->AddEvent();
#endif
#pragma omp critical (g3)
      if(GCFLAG.IEVENT%abs(GCFLAG.ITEST)==0 ||     GCFLAG.IEORUN || GCFLAG.IEOTRI || 
         GCFLAG.IEVENT>=GCFLAG.NEVENT){
      AMSEvent::gethead()->printA(AMSEvent::debug);
        if(MISCFFKEY.G3On)GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
        else {
          GCFLAG.NRNDM[0]=HepRandom::getTheSeeds()[0];
          GCFLAG.NRNDM[1]=HepRandom::getTheSeeds()[1];
        }
       cout <<" RNDM "<<GCFLAG.NRNDM[0]<<" " <<GCFLAG.NRNDM[1]<<endl;
}
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
//   if(trig ){ 
//     AMSEvent::gethead()->copy();
//   }
     AMSEvent::gethead()->write(trig);

     UPool.Release(0);
   AMSEvent::gethead()->remove();
     UPool.Release(1);
   AMSEvent::sethead(0);
   UPool.erase(2000000);

   GCFLAG.IEVENT++;
   if(GCFLAG.IEVENT>=GCFLAG.NEVENT){
    GCFLAG.IEOTRI=1;
    GCFLAG.IEORUN=1;
   }
   if(GCFLAG.IEOTRI || GCFLAG.IEORUN)G4RunManager::GetRunManager()->AbortRun();
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
      const G4double stepMinimum = 1.e-4 * cm;
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
//---
  if(G4FFKEY.TFNewGeant4>0){ //TOF New Geant4 Geometry->down to mother
    cout << "AMSG4DetectorInterface::Construct-I-New TOFB Geometry "<<endl;
    TofSimUtil::Head->MakeTOFG4Volumes(AMSJob::gethead()->getgeom()->down());
   }
//--
// Attention as step volumes are linked to false_mother, not mother as other ones
 AString fnam(AMSDATADIR.amsdatadir);
 fnam+="amsstp_";
 fnam+=AMSJob::gethead()->getsetup();
 fnam+= AMSJob::gethead()->isRealData()?".1":".0";
// as usual doesn't work...
//  AMSJob::gethead()->getgeom()->ReadG4StepVolumes((char*)fnam);  
  cout << "AMSG4DetectorInterface::Construct-I-"<<AMSgvolume::getNpv()<<" Physical volumes, "<<AMSgvolume::getNlv()<<" logical volumes and "<<AMSgvolume::getNrm()<<" rotation matrixes have been created "<<endl;
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
//AMSG4RotationMatrix::AMSG4RotationMatrix(number nrm[3][3]):G4RotationMatrix(nrm[0][0],nrm[1][0],nrm[2][0],nrm[0][1],nrm[1][1],nrm[2][1],nrm[0][2],nrm[1][2],nrm[2][2]){
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
   AMSmceventg::SaveSeeds();   
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
   
   AMSmceventg::RestoreSeeds();   

}


AMSG4DummySD** AMSG4DummySD::_pSD=0;
integer AMSG4DummySDI::_Count=0;

AMSG4DummySDI::AMSG4DummySDI(){
if(!_Count++){
 AMSG4DummySD::_pSD=new AMSG4DummySD*[3];
 AMSG4DummySD::pSD()=new AMSG4DummySD();
 AMSG4DummySD::pSD(1)=new AMSG4DummySD("AMSG4TRDRadiator");
 AMSG4DummySD::pSD(2)=new AMSG4DummySD("AMSG4TRDGas");
}
}

AMSG4DummySDI::~AMSG4DummySDI(){
 if(--_Count==0){
  for(int i=0;i<3;i++)delete AMSG4DummySD::pSD(i);
  delete[] AMSG4DummySD::_pSD;
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
     
  GCTRAK.istop=0;

  //---TOF Geant4//in case of rich photon cut+photon track length cut
  G4ParticleDefinition* particleType = Step->GetTrack()->GetDefinition();
  if(particleType==G4OpticalPhoton::OpticalPhotonDefinition()){
    number phposz=Step->GetPostStepPoint()->GetPosition().z()/cm;
    if(phposz>-71.62||phposz<71.62){ //tof regin
      G4Track* theTrack = Step->GetTrack(); 
      if(theTrack->GetCreatorProcess()->GetProcessName()!="Scintillation"||
	 theTrack->GetTrackLength()/m>TFMCFFKEY.phtrlcut)
        {theTrack->SetTrackStatus(fStopAndKill);return;}
      G4String tofn=Step->GetPostStepPoint()->GetPhysicalVolume()->GetName();
      if(tofn(0)!='T'||tofn(1)!='O'||tofn(2)!='F'||tofn(3)!='P')return;
    }
  }
  //---

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

     //
     // TR Radiation Here
     // 

  */
  //       cout <<" stepping in"<<endl;
  //       AMSmceventg::PrintSeeds(cout);
  //       AMSmceventg::SaveSeeds();
  //       if(GCFLAG.NRNDM[0]==2130120 && GCFLAG.NRNDM[1]==1154959891){
  //        cout <<" jopa "<<endl;
  //       }
  //{
  //       G4Track * Track = Step->GetTrack();
  //       G4ParticleDefinition* particle =Track->GetDefinition();
  //       cout << "particle "<<particle->GetParticleName()<<endl;
  //}         
  //     geant d;
  // Checking if Volume is sensitive one 
  G4StepPoint * PostPoint = Step->GetPostStepPoint();
  G4VPhysicalVolume * PostPV = PostPoint->GetPhysicalVolume();
  G4StepPoint * PrePoint = Step->GetPreStepPoint();
  G4VPhysicalVolume * PrePV = PrePoint->GetPhysicalVolume();
  if(PostPV && PrePV){
   //  cout << "Stepping Pre  "<<" "<<PrePV->GetName()<<" "<<PrePV->GetCopyNo()<<" "<<PrePoint->GetPosition()<<endl;
//     cout << "Stepping  Post"<<" "<<PostPV->GetName()<<" "<<PostPV->GetCopyNo()<<" "<<PostPoint->GetPosition()<<" "<<PostPoint->GetKineticEnergy()/GeV<<" "<<Step->GetStepLength()/cm<<" " <<Step->GetTotalEnergyDeposit()/GeV<<endl;
//     cout << "Part ID " << Step->GetTrack()->GetDefinition()->GetParticleName()<<endl;
//   cout <<endl;
    GCTMED.isvol=PostPV->GetLogicalVolume()->GetSensitiveDetector()!=0 ||
      PrePV->GetLogicalVolume()->GetSensitiveDetector()!=0;
    GCTRAK.destep=Step->GetTotalEnergyDeposit()/GeV;
     
    //   if(PrePoint->GetProcessDefinedStep())cout<<" b "<<PrePoint->GetProcessDefinedStep()->GetProcessName()<<endl;
    //   if(PostPoint->GetProcessDefinedStep())cout<<"a "<<PostPoint->GetProcessDefinedStep()->GetProcessName()<<endl;
    G4Track * Track = Step->GetTrack();
    GCTRAK.nstep=Track->GetCurrentStepNumber()-1;
    GCKINE.itra=Track->GetParentID();
    //cout << " track id "<<GCKINE.itra<<" "<<GCTRAK.nstep<<endl;
    G4ParticleDefinition* particle =Track->GetDefinition();
    GCKINE.ipart=AMSJob::gethead()->getg4physics()->G4toG3(particle->GetParticleName());
    GCKINE.charge=particle->GetPDGCharge();
    //    if(GCKINE.ipart==51){
    //       cout <<" xray "<<PrePV->GetName()<<" "<<PostPV->GetName()<<" "<<PostPoint->GetPosition()<<endl;
    //    }
    /*
      if(GCKINE.ipart==Cerenkov_photon){
      //       cout <<" cerenkov "<<PrePV->GetName()<<" "<<PostPV->GetName()<<" "<<PostPoint->GetPosition()<<endl;
      if((PrePV->GetName())(0)=='R' && (PrePV->GetName())(1)=='I' &&
      (PrePV->GetName())(2)=='C' && (PrePV->GetName())(3)=='H'){
      if(PostPoint->GetProcessDefinedStep() && PostPoint->GetProcessDefinedStep()->GetProcessName() == "Boundary")RICHDB::numrefm++;
      }

      if(PrePV->GetName()(0)=='R' && PrePV->GetName()(1)=='A' &&
      PrePV->GetName()(2)=='D' && PrePV->GetName()(3)==' '){
      if(GCTRAK.nstep){
      if(PostPoint->GetProcessDefinedStep() && PostPoint->GetProcessDefinedStep()->GetProcessName() == "Rayleigh Scattering")RICHDB::numrayl++;
      else{
      RICHDB::numrayl=0;
      RICHDB::numrefm=0;
      //          RICHDB::nphgen++;  
      if(!RICHDB::detcer(GCTRAK.vect[6])) {
      }
      else RICHDB::nphgen++;
      }

      }
      }
      }
    */
        

    if(GCTMED.isvol){// <========== we are in sensitive volume !!!
      //
      //      cout << "Stepping  sensitive"<<" "<<PrePV->GetName()<<" "<<PrePV->GetCopyNo()<<" "<<PrePoint->GetPosition()<<endl;
      // gothering some info and put it into geant3 commons

      GCTRAK.inwvol= PostPV != PrePV;
      GCTRAK.step=Step->GetStepLength()/cm;
      GCTRAK.vect[0]=PostPoint->GetPosition().x()/cm; 
      GCTRAK.vect[1]=PostPoint->GetPosition().y()/cm; 
      GCTRAK.vect[2]=PostPoint->GetPosition().z()/cm; 
      GCTRAK.vect[3]=PostPoint->GetMomentumDirection().x(); 
      GCTRAK.vect[4]=PostPoint->GetMomentumDirection().y(); 
      GCTRAK.vect[5]=PostPoint->GetMomentumDirection().z(); 
      GCTRAK.getot=PostPoint->GetTotalEnergy()/GeV;
      GCTRAK.gekin=PostPoint->GetKineticEnergy()/GeV;
      GCTRAK.vect[6]=GCTRAK.getot*PostPoint->GetBeta();
      //     GCTRAK.tofg=PostPoint->GetGlobalTime()/second;
      GCTRAK.tofg=PrePoint->GetGlobalTime()/second;
      { int i;
	for(i=0;i<3;i++)GCKINE.vert[i]=Track->GetVertexPosition()[i]/cm; 
	for(i=0;i<3;i++)GCKINE.pvert[i]=Track->GetVertexMomentumDirection()[i]; 


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
	//      cout << "Stepping  sensitive"<<" "<<PostPV->GetName()<<" "<<PostPV->GetCopyNo()<<" "<<PostPoint->GetPosition()<<" "<<GCKINE.ipart<<" "<<GCTRAK.destep<<" "<<GCTRAK.step<<endl;

	try{
	  // Now one has decide based on the names of volumes (or their parents)
	  G4VPhysicalVolume * Mother=PrePV->GetMother();
	  G4VPhysicalVolume * GrandMother= Mother?Mother->GetMother():0;
	  //-------------------------------------------------------------
	  // TRD
	  if(GCTRAK.destep && PrePV->GetName()(0)=='T' && PrePV->GetName()(1)=='R' 
	     &&  PrePV->GetName()(2)=='D' && PrePV->GetName()(3)=='T'){
	    //cout <<" trd "<<GCKINE.itra<<" "<<GCKINE.ipart<<endl;
	    AMSTRDMCCluster::sitrdhits(PrePV->GetCopyNo(),GCTRAK.vect,
				       GCTRAK.destep,GCTRAK.gekin,GCTRAK.step,GCKINE.ipart,GCKINE.itra);   
	  }



	  //------------------------------------------------------------      
	  //  Tracker
	 //  cout <<"================="<<endl;
// 	  if(GrandMother)
// 	  cout <<GrandMother->GetName()<< endl;
// 	  if(Mother)
// 	  cout <<Mother->GetName()<< endl;
//  	  cout <<PrePV->GetName()<< endl;
// 	  cout <<GCTRAK.destep <<endl;
// 	  cout<<endl;
#ifdef _PGTRACK_
	  if(GCTRAK.destep && GrandMother && Mother &&
	     GrandMother->GetName()(0)=='S' &&  GrandMother->GetName()(1)=='T' && GrandMother->GetName()(2)=='K' &&
	     Mother->GetName()(0)=='L' &&  PrePV->GetName()(0)=='S'){
	    //         cout <<" tracker "<<endl;
	    TrSim::sitkhits(PrePV->GetCopyNo(),GCTRAK.vect,
			    GCTRAK.destep,GCTRAK.step,GCKINE.ipart);   
	  }
#else
	  if(GCTRAK.destep && GrandMother && GrandMother->GetName()(0)=='S' 
	     &&  GrandMother->GetName()(1)=='T' && GrandMother->GetName()(2)=='K'){
	    AMSTrMCCluster::sitkhits(PrePV->GetCopyNo(),GCTRAK.vect,
				     GCTRAK.destep,GCTRAK.step,GCKINE.ipart);   
	  }
#endif
	  //------------------------------------------------------------
	  //    TOF: (imply here that Pre or Post volume is sensitive as defined by above check !!!)
	  //
	  geant t,x,y,z;
	  char name[5]="dumm";
	  char media[21]="dummy_media         ";
	  geant de,dee,dtr2,div,tof,prtq,pstep;
	  geant tdedx;
	  geant trcut2(0.25);// Max. transv.shift (0.5cm)**2
	  geant stepmin(0.25);//(cm) min. step/cumul.step to store hit(0.5cm/2)
	  geant estepmin(1.e-5);//10kev
	  geant coo[3],dx,dy,dz,dt;
	  geant wvect[6],snext,safety;
	  int i,nd,numv,iprt,numl,numvp,tfprf(0);
	  static int numvo(-999),iprto(-999);
	  integer tbegtof(0);
	  integer tendtof(0);
	  integer intof(0);
	  if(PrePV->GetName()(0)== 'T' && PrePV->GetName()(1)=='F')tbegtof=1;
	  if(PostPV->GetName()(0)== 'T' && PostPV->GetName()(1)=='F')tendtof=1;
	  if(tbegtof==1 || tendtof==1)intof=1;
	  //
	  //------------------------------------------------------------------
	  //  TOF simple :
	  //
	  numv=PrePV->GetCopyNo();
	  dee=GCTRAK.destep;
	  tof=GCTRAK.tofg;
	  pstep=GCTRAK.step;
	  iprt=GCKINE.ipart;
	  x=GCTRAK.vect[0];
	  y=GCTRAK.vect[1];
	  z=GCTRAK.vect[2];
	  if(tendtof==1 && GCTRAK.inwvol==1){// just enter TFnn
	    //cout<<"---> Enter TOF: part="<<iprt<<" x/y/z="<<x<<" "<<y<<" "<<z<<" Edep="<<dee<<" numv="<<numv<<" pstep="<<pstep<<endl;  
	  }
	  if(tbegtof==1 && GCTRAK.destep>0.){
	    if(pstep!=0)tdedx=1000*dee/pstep;
	    else tdedx=0;
	    number rkb=0.0011;
	    number c=0.52;
	    number dedxcm=1000*dee/GCTRAK.step;
	    if(G4FFKEY.TFNewGeant4>0)dee=dee/(1.+TFMCFFKEY.birk*dedxcm*0.1);
	    else                     dee=dee/(1+c*atan(rkb/c*dedxcm));
	    //cout<<"   > continue TOF: part="<<iprt<<" x/y/z="<<x<<" "<<y<<"  "<<z<<" Edep="<<dee<<" numv="<<numv<<"  step="<<pstep<<" dedx="<<tdedx<<endl;
	    AMSTOFMCCluster::sitofhits(numv,GCTRAK.vect,dee,tof);
	    //----
	    if(G4FFKEY.TFNewGeant4>1){
	      number tofdt= Step->GetStepLength()/((PostPoint->GetVelocity()+PrePoint->GetVelocity())/2.)/nanosecond;
	      integer  parentid=Track->GetTrackID();
	      TOF2TovtN::covtoph(numv,GCTRAK.vect,dee,tof,tofdt,GCTRAK.step,parentid);
	    } 
	  }
     
	  //--Trace every photon in PMT //may be already absorb
	  if(G4FFKEY.TFNewGeant4==1){
	    if(PrePV->GetName()(0)== 'T' && PrePV->GetName()(1)=='O'&& PrePV->GetName()(2)=='F' && PrePV->GetName()(3)=='L'&&
	       PostPV->GetName()(0)== 'T' && PostPV->GetName()(1)=='O' && PostPV->GetName()(2)=='F'&&PostPV->GetName()(3)=='P'){
	      //check boundary           
	      G4OpBoundaryProcessStatus TOFPMBoundaryStatus=Undefined;
	      static RichG4OpBoundaryProcess* boundary=NULL;
	      if(!boundary){
		G4ProcessManager* pm= Step->GetTrack()->GetDefinition()->GetProcessManager();
		G4int nprocesses = pm->GetProcessListLength();
		G4ProcessVector* pv = pm->GetProcessList();
		G4int i;
		for( i=0;i<nprocesses;i++){
		  if((*pv)[i]->GetProcessName()=="OpBoundary"){
		    boundary = (RichG4OpBoundaryProcess* )(*pv)[i];
		    break;
		  }
		}
	      }
	      //----absorption
	      if(particle==G4OpticalPhoton::OpticalPhotonDefinition()&&PostPoint->GetStepStatus()==fGeomBoundary&&boundary
		 && Step->GetTrack()->GetCreatorProcess()->GetProcessName()=="Scintillation")
		{
		  TOFPMBoundaryStatus=boundary->GetStatus();
		  switch(TOFPMBoundaryStatus){
		  case Absorption:
		    //        G4cout<<"absorption"<<G4endl;
		    break;
		  case Detection://detected by pmt
		    {
		      integer parentid=Step->GetTrack()->GetParentID();
		      const G4VTouchable *touch = PostPoint->GetTouchable();
		      integer       pmtid =touch    ->GetReplicaNumber();
		      //                   cout<<"pmtid"<<pmtid<<endl;
		      geant         phtim= PostPoint->GetGlobalTime()/nanosecond;
		      geant         phtiml=PostPoint->GetLocalTime()/nanosecond;//from gene
		      geant         phene= PostPoint->GetTotalEnergy()/eV;
		      geant         phtral=Step     ->GetTrack()->GetTrackLength()/cm;
		      //                   G4ThreeVector phpos= PostPoint->GetPosition()/cm;
		      //                   G4ThreeVector phdir= PostPoint->GetMomentumDirection();
		      G4ThreeVector phpos=Step->GetTrack()->GetVertexPosition();//generatep pos
		      G4ThreeVector phdir=Step->GetTrack()->GetVertexMomentumDirection();//generatep pos  
		      G4ThreeVector localpos=touch  ->GetHistory()->GetTopTransform().TransformPoint(phpos);
		      G4ThreeVector localdir=touch  ->GetHistory()->GetTopTransform().TransformAxis(phdir);//grobal to local
		      //pmt effciency rely on angle and energy                   
		      //                   G4double tofsina=sqrt(1.-localdir.z()*localdir.z());
		      //                   G4double tofsinb=tofsina*TofSimUtil::LGRIND/TofSimUtil::VARIND;//vaccum angle
		      bool AnPass=1;
		      //                   if(tofsinb>=1.)AnPass=0;//total reflection in gap
		      //                   else  AnPass=1.;
		      if(AnPass==1){
			if(G4UniformRand()<TofSimUtil::PHEFFC[pmtid/1000%10][pmtid/100%10][pmtid/10%10][pmtid%10]){
			  geant tfpos[3],tfdir[3];
			  tfpos[0]=phpos.x();tfpos[1]=phpos.y();tfpos[2]=phpos.z();
			  tfdir[0]=phdir.x();tfdir[1]=phdir.y();tfdir[2]=phdir.z();
			  AMSTOFMCPmtHit::sitofpmthits(pmtid,parentid,phtim,phtiml,phtral,phene,tfpos,tfdir);
			}
		      }
		    }
		    break;
		  case FresnelReflection:
		    //G4cout<<"fresnel_reflection"<<G4endl;
		    break;
		  case FresnelRefraction:
                    // G4cout<<"postvol name="<<PostPV->GetName()<<G4endl;
		    break;
		  default:
		    break;
		  }
		}
	      //---end absorption
	    }//volume
	  }//TOF geant4

	  //------------------------------------------------------------------
	  //  ANTI :
	  //
	  integer isphys,islog;
	  if(PrePV->GetName()(0)== 'A' && PrePV->GetName()(1)=='N' &&
	     PrePV->GetName()(2)=='T' && PrePV->GetName()(3)=='S' && GCTRAK.destep>0.){
	    dee=GCTRAK.destep;
	    isphys=PrePV->GetCopyNo();
	    islog=floor(0.5*(isphys-1))+1;//not used now
	    number rkb=0.0011;
	    number c=0.52;
	    number dedxcm=1000*dee/GCTRAK.step;
	    dee=dee/(1+c*atan(rkb/c*dedxcm));
	    AMSAntiMCCluster::siantihits(isphys,GCTRAK.vect,
					 dee,GCTRAK.tofg);
	  }// <--- end of "in ANTS"
	  //------------------------------------------------------------------
	  //  ECAL :
	  //
	  if(PrePV->GetName()(0)== 'E' && PrePV->GetName()(1)=='C' && 
	     PrePV->GetName()(2)=='F' && PrePV->GetName()(3)=='C'){
	    if(GCTRAK.destep>0.){
	      dee=GCTRAK.destep;
	      //GBIRK(dee);
	      //  
	      //     birks law dirty way
	      //

	      number rkb=0.0011;
	      number c=0.52;
	      number dedxcm=1000*dee/GCTRAK.step;
	      //      dee=dee/(1+c*atan(rkb/c*dedxcm));
	      dee=dee/ECMCFFKEY.sbcgn;//correction for too high signal vrt g3
	      static unsigned int np=0; if(np++<0)cout<<"... in ECAL: numv="<<PrePV->GetCopyNo()<<" "<<dee<<" "<<PrePV->GetMother()->GetCopyNo()<<" "<<PrePV->GetName()<<" "<<GCTRAK.vect[0]<<" "<<GCTRAK.vect[1]<<" "<<GCTRAK.vect[2]<<" "<<PrePV->GetMother()->GetName()<<" "<<PrePV->GetMother()->GetLogicalVolume()<<" "<<GCTRAK.destep<<endl;
	      AMSEcalMCHit::siecalhits(PrePV->GetMother()->GetCopyNo(),GCTRAK.vect,dee,GCTRAK.tofg);
	    }
	  }
	  //------------------------------------------------------------------
	  // CJM : RICH (G4 synchronized version)
	  //
	  AMSPoint local_position;
	  int volume=PostPV->GetCopyNo()-1;
	  if(PostPV->GetName()(0)=='C' && PostPV->GetName()(1)=='A' &&
	     PostPV->GetName()(2)=='T' && PostPV->GetName()(3)=='O' && 
	     GCTRAK.inwvol==1){

	    if(GCKINE.ipart==Cerenkov_photon && GCTRAK.nstep==0){
	      GCTRAK.istop=1;

	      local_position=AMSPoint(GCTRAK.vect);
	      local_position=RichAlignment::AMSToRich(local_position);

	      geant xl=(RichPMTsManager::GetAMSPMTPos(volume,2)-RICHDB::cato_pos()-RICotherthk/2-local_position[2])/GCTRAK.vect[5];

	      geant vect[3];
	      vect[0]=GCTRAK.vect[0]+xl*GCTRAK.vect[3];
	      vect[1]=GCTRAK.vect[1]+xl*GCTRAK.vect[4];
	      vect[2]=GCTRAK.vect[2]+xl*GCTRAK.vect[5];
	
	      AMSRichMCHit::sirichhits(GCKINE.ipart,
				       volume,
				       vect,
				       GCKINE.vert,
				       GCKINE.pvert,
				       Status_Window-
				       (GCKINE.itra!=1?100:0));
	    }
      

	    if(GCKINE.ipart==Cerenkov_photon && GCTRAK.nstep!=0){
	      GCTRAK.istop=2; // Absorb it
	      local_position=AMSPoint(GCTRAK.vect);
	      local_position=RichAlignment::AMSToRich(local_position);
	      geant xl=(RichPMTsManager::GetAMSPMTPos(volume,2)-RICHDB::cato_pos()-RICotherthk/2-local_position[2])/GCTRAK.vect[5];

	      geant vect[3];
	      vect[0]=GCTRAK.vect[0]+xl*GCTRAK.vect[3];
	      vect[1]=GCTRAK.vect[1]+xl*GCTRAK.vect[4];
	      vect[2]=GCTRAK.vect[2]+xl*GCTRAK.vect[5];
	
	      if(GCKINE.vert[2]<RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::rich_height-
		 RICHDB::foil_height-RICradmirgap-RIClgdmirgap // in LG
		 || (GCKINE.vert[2]<RICHDB::RICradpos()-RICHDB::rad_height &&
		     GCKINE.vert[2]>RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::foil_height))
		AMSRichMCHit::sirichhits(GCKINE.ipart,
					 volume,
					 vect,
					 GCKINE.vert,
					 GCKINE.pvert,
					 Status_LG_origin);
	      else
		AMSRichMCHit::sirichhits(GCKINE.ipart,
					 volume,
					 vect,
					 GCKINE.vert,
					 GCKINE.pvert,
					 0);
	    }else if(GCTRAK.nstep!=0){	 
	      AMSRichMCHit::sirichhits(GCKINE.ipart,
				       volume,
				       GCTRAK.vect,
				       GCKINE.vert,
				       GCKINE.pvert,
				       Status_No_Cerenkov);
	    }
	  }
  

	  // end of RICH
	  //----------------------------------------------------------------

    
	} // <--- end of "try" ---
	//
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
      //   cout <<"leaving stepping "<<endl;
    }
    if(GCTRAK.istop){
      Track->SetTrackStatus(fStopAndKill);
    }

  }

}

G4ClassificationOfNewTrack AMSG4StackingAction::ClassifyNewTrack(const G4Track * aTrack)
{ 
  G4ParticleDefinition* particle =aTrack->GetDefinition();
  GCKINE.ipart=AMSJob::gethead()->getg4physics()->G4toG3(particle->GetParticleName());
  if(GCKINE.ipart==Cerenkov_photon){
    //--new TOF part
    if((G4FFKEY.TFNewGeant4==1)){
      G4ThreeVector phver=aTrack->GetPosition();
      number phposz=phver.z()/cm;
      bool IsTof=(phposz>-71.62&&phposz<71.62);//not RICH region
      if(IsTof){
	if(aTrack->GetCreatorProcess()->GetProcessName()!="Scintillation")return fKill;
	G4PhysicsTable* PMTEffTable=TofSimUtil::Head->TOFPM_Et;
	G4PhysicsOrderedFreeVector* PMTEnEff=(G4PhysicsOrderedFreeVector*)((*PMTEffTable)(0));
	G4double phene=aTrack->GetTotalEnergy()/eV;
	if(phene<2.4||phene>3.35)return fKill;
	G4double EnEff =PMTEnEff->Value(phene*eV);
	bool  EnPass=(G4UniformRand()<EnEff/TofSimUtil::Head->QEMAX);
	if(!EnPass)return fKill;
	//---verticle direction photon need more cpu time + more absorbtion=big angle cut
        const G4VTouchable *touch = aTrack->GetTouchable();
        G4ThreeVector phdir=aTrack->GetMomentumDirection();
        G4ThreeVector localdir=touch  ->GetHistory()->GetTopTransform().TransformAxis(phdir);//grobal to local
        G4bool  AnPass;
        AnPass=(fabs(localdir.y())>TFMCFFKEY.phancut);
        if(!AnPass){
	  return fKill;
	}
        return fWaiting;
      }
    }

    //--Rich part
    double e=aTrack->GetTotalEnergy()/GeV;
    if(!RICHDB::detcer(e)) return fKill; // Kill discarded Cerenkov photons
  }
  AMSmceventg::FillMCInfoG4( aTrack );
  return fWaiting;
}


