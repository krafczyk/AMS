//  $Id$
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
#include "G4GeometryTolerance.hh"
#include "G4PropagatorInField.hh"
#include "G4PhysicalVolumeStore.hh"
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
#include "G4VProcess.hh"
#include "g4xray.h"
#include "producer.h"
#include "TofSimUtil.h"
#include "Tofsim02.h"
#include "g4tof.h"
#include "g4rich.h"
#ifdef G4VIS_USE
#include "g4visman.h"
#endif

#ifndef __DARWIN__
#include <malloc.h>
#else
#include <mach/task.h>
#include <mach/mach_init.h>
#endif

 extern "C" void getfield_(geant& a);

size_t get_memory_usage() {

#ifndef __DARWIN__
  struct mallinfo m = mallinfo();
  return m.uordblks + m.arena;
#else
  struct task_basic_info t_info;
  mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
  task_info(current_task(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count);
  return t_info.resident_size;
#endif
}

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
  cout<<"  geant4i-I-visman created"<<endl; 
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
  static unsigned int iq=0;
  if(iq++==0){ 
    cout<<"~~~~~~~~~~~~~~~~Begin of Run Action, Construct G3G4 Tables here~~~~~~~~~~~~~~"<<endl;
    pph->_init();
  }
}


#include "G4ProcessTable.hh"
#include "G4ParticleTable.hh"
#include "G4DynamicParticle.hh"
#include "G4ThreeVector.hh"
#include "G4Element.hh"
#include "G4NistManager.hh"
// #include "G4CrossSectionDataStore.hh"
#include "G4HadronicInteraction.hh"
#include "G4HadFinalState.hh"
#include "G4HadProjectile.hh"
#include "G4Nucleus.hh"
#include "G4Version.hh"

void  AMSG4RunAction::DumpCrossSections(int verbose, G4int At, G4int Zt) {
  if (verbose<=0) return; 
  cout << "~~~~~~~~~~~~~~~~ DumpCrossSections ~~~~~~~~~~~~~~" << endl;
  // loop on three kinds of particles (p, He, C)
  G4ParticleTable* table = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle[3] = {G4Proton::Proton(),G4Alpha::Alpha(),table->GetIon(6,12,0)};
  for (int iparticle=0; iparticle<3; iparticle++) {
    if (!particle[iparticle]) continue; 
    particle[iparticle]->DumpTable();
    G4ProcessManager* manager = particle[iparticle]->GetProcessManager();
    if (manager) manager->DumpInfo();
    G4ProcessTable* theProcessTable = G4ProcessTable::GetProcessTable();
    // get the hadronic inelastic model whatever it is 
    G4HadronInelasticProcess* process = (G4HadronInelasticProcess*) theProcessTable->FindProcess("ionInelastic",particle[iparticle]);
    if (!process) process = (G4HadronInelasticProcess*) theProcessTable->FindProcess("ProtonInelastic",particle[iparticle]);
    if (!process) process = (G4HadronInelasticProcess*) theProcessTable->FindProcess("alphaInelastic",particle[iparticle]);
    if (!process) process = (G4HadronInelasticProcess*) theProcessTable->FindProcess("AlphaInelastic",particle[iparticle]);
    if (!process) process = (G4HadronInelasticProcess*) theProcessTable->FindProcess("IonInelastic",particle[iparticle]);
    if (!process) {
      printf("No inelastic process found.\n");
      continue;
    }  
    // dump tables
    process->DumpPhysicsTable(*particle[iparticle]);
    G4int Ap = particle[iparticle]->GetBaryonNumber();
    G4int Zp = particle[iparticle]->GetPDGCharge()/eplus;
    // search/build target
    G4Element* target = 0;
    G4ElementTable::iterator iter;
    G4ElementTable *elementTable = const_cast<G4ElementTable*> (G4Element::GetElementTable());
    for (iter = elementTable->begin(); iter != elementTable->end(); ++iter) {
      G4int AA = (*iter)->GetN();
      G4int ZZ = (*iter)->GetZ();
      if ( (AA==At)&&(ZZ=Zt) ) target = *iter;
    }
    if(!target){
      G4cout << "No element found for A=" << At << " Z= " << Zt << G4endl;
      return;
    }
    G4Material* material = new G4Material("material_target",2.26,1);
    material->AddElement(target,1.);
    G4HadronicInteraction* model = process->GetManagerPointer()->GetHadronicInteraction(10*GeV,material,target); 
    cout << "Hadronic model @ Kn = 100 GeV/n is " << model->GetModelName() << endl;
    for (int irig=0; irig<=50; irig++) {
      // inealastic cross section  
      G4double rigidity = 0.1*pow(10,irig*(log10(10000)-log10(0.1))/50); 
      G4double momentum = Zp*rigidity*GeV;
      G4ThreeVector momentum_vector(momentum,0,0); 
      G4DynamicParticle projectile(particle[iparticle],momentum_vector);
      G4double k = projectile.GetKineticEnergy();  
      G4double kn = k/Ap;
#if G4VERSION_NUMBER  > 945 
      G4Material mat_target("mat_target",1,1);
      mat_target.AddElement(target,1);
      G4double XS = process->GetElementCrossSection(&projectile,target,&mat_target); // v9.6.p02
#else
      G4double XS = process->GetMicroscopicCrossSection(&projectile,target,295*kelvin); // v9.4.p04
#endif
      if ( (verbose<=1)||(Zp!=6)||(rigidity>3000.) ) { 
        printf("(%2d,%2d)->(%2d,%2d) @ %10.3f GeV/c (%10.3f GeV/n) = %10.3f mbarn\n",Ap,Zp,At,Zt,momentum/GeV,kn/GeV,XS/millibarn);
        continue; 
      }
      // fake MC to get partial cross-section
      G4int ngood = 0; 
      G4int nmc = 10000;
      for (int imc=0; imc<nmc; imc++) { 
        model = process->GetManagerPointer()->GetHadronicInteraction(k,material,target);
        if (!model) continue;
        G4HadProjectile a_projectile(projectile);
        G4Nucleus a_target(At,Zt);
        G4int Zmax = 0;    
        G4HadFinalState* final_state = model->ApplyYourself(a_projectile,a_target); 
        for (int isec=0; isec<final_state->GetNumberOfSecondaries(); isec++) {
          G4DynamicParticle* secondary = final_state->GetSecondary(isec)->GetParticle();
          G4int As = secondary->GetParticleDefinition()->GetBaryonNumber();
          G4int Zs = secondary->GetParticleDefinition()->GetPDGCharge()/eplus;
          if ( (As<1)||(Zs<1) ) continue;
          G4double ks = secondary->GetKineticEnergy();
          G4double kns = ks/As;
          if (kns<0.1*kn) continue; 
          if (Zs>Zmax) Zmax = Zs;  
        }
        if (Zmax==6) ngood++;
      }
      printf("(%2d,%2d)->(%2d,%2d) @ %10.3f GeV/c (%10.3f GeV/n) = %10.3f mbarn   P(C+C->X) = %4.3f\n",Ap,Zp,At,Zt,momentum/GeV,kn/GeV,XS/millibarn,1-1.*ngood/nmc);
    }
  }
  cout << "~~~~~~~~~~~~~~~~ DumpCrossSections ~~~~~~~~~~~~~~" << endl;
}


void  AMSG4RunAction::EndOfRunAction(const G4Run* anRun){


  if (G4FFKEY.DumpCrossSections>0) DumpCrossSections(G4FFKEY.DumpCrossSections,
						     G4FFKEY.DumpCrossSectionsAt,
						     G4FFKEY.DumpCrossSectionsZt);
    AMSG4Physics::SaveXS(GCKINE.ikine);
}


void  AMSG4EventAction::BeginOfEventAction(const G4Event* anEvent){

 fset_reg_tracks.clear();
 fmap_det_tracks.clear();
 fset_reg_tracks.insert(1); //primary track
 fmap_det_tracks.insert( std::pair<int, std::pair<int,int> >(1, std::pair<int,int>(0,0)) );
 flast_trkid=flast_resultid=flast_processid=-1;


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
   if(AMSJob::gethead()->isSimulation()){

     if (AMSEvent::gethead()->EventSkipped()) {
       G4ThreeVector primaryMomentumVector = anEvent->GetPrimaryVertex(0)->GetPrimary(0)->GetMomentum();
       G4double primaryMomentum = sqrt(primaryMomentumVector.x() / GeV * primaryMomentumVector.x() / GeV +
                                       primaryMomentumVector.y() / GeV * primaryMomentumVector.y() / GeV +
                                       primaryMomentumVector.z() / GeV * primaryMomentumVector.z() / GeV);
       hman.Fill("Pskipped", primaryMomentum);
       AMSEvent::gethead()->SetEventSkipped(false);
     }

   AMSgObj::BookTimer.stop("GEANTTRACKING");
 {
    float xx,yy;
    TIMEX(xx);
    TIMEL(yy);
    if(GCTIME.TIMEND < xx || (yy>0 && yy<AMSFFKEY.CpuLimit) ){
      GCFLAG.IEORUN=1;
      GCFLAG.IEOTRI=1;
      GCTIME.ITIME=1;
    }
  }

   static unsigned int minit=0;
    unsigned int mall=get_memory_usage();
    if(minit==0){
      minit=mall;
      cout<<"  AMSG4EventAction::EndOfEventAction-I-InitialMemoryAllocation "<<mall<<" "<<minit<<" "<<G4FFKEY.MemoryLimit<<endl;
      
    }
    if(gams::mem_not_enough(2*102400)){
      GCFLAG.IEORUN=1;
      GCFLAG.IEOTRI=1;
    }
    if(G4FFKEY.MemoryLimit>0 && mall-minit>G4FFKEY.MemoryLimit){
      GCFLAG.IEORUN=1;
      GCFLAG.IEOTRI=1;
      cout<<"  AMSG4EventAction::EndOfEventAction-I-Memory Allocation "<<mall<<endl;
#ifdef __G4MODIFIED__
      G4EventManager::GetEventManager()->trackContainer->clear();
      G4EventManager::GetEventManager()->trackContainer->ClearPostponeStack();
      G4StackedTrack::ResetMemory();
      mall = get_memory_usage();
      cout<<"  AMSG4EventAction::EndOfEventAction-I-Memory Allocation "<<mall<<endl;
#endif
      cerr<<"  AMSG4EventAction::EndOfEventAction-W-TerminatingRun "<<mall<<" "<<G4FFKEY.MemoryLimit<<endl;

      
    } 
}
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



void AMSG4EventAction::AddRegisteredTrack(int gtrkid)
{
  fset_reg_tracks.insert(gtrkid);
}


void AMSG4EventAction::AddRegisteredParentChild(int gtrkid, int gparentid, int processid)
{
  fmap_det_tracks.insert( std::pair<int, std::pair<int,int> >( gtrkid, std::pair<int,int>(gparentid,processid) ) );
}

bool AMSG4EventAction::IsRegistered(int gtrkid)
{
  set<int>::iterator it = find(fset_reg_tracks.begin(), fset_reg_tracks.end(), gtrkid);
  if( it==fset_reg_tracks.end() ) return false;
  return true;
}

void AMSG4EventAction::FindClosestRegisteredTrack( int& gtrkid, int& processid ){


  if( gtrkid == flast_trkid ) {
    gtrkid = flast_resultid; //to speedup
    processid = flast_processid;
    return;
  }

  if( IsRegistered( gtrkid ) ) {
    flast_trkid = gtrkid;
    flast_resultid = gtrkid;
    flast_processid = processid;
    return;
  }

  bool found = false;
  bool err = false;
  int par_id = gtrkid;
  int process_id = 0;

  /*  cout<<"Finding clsest track to: "<< gtrkid<<endl;
  cout<<"Content of set: ";
  for( set<int>::iterator it = fset_reg_tracks.begin(); it!=fset_reg_tracks.end(); ++it ) cout<<" "<<*it;
  cout<<endl;

  cout<<"Content of map: ";
  for( map<int,int>::iterator itm = fmap_det_tracks.begin(); itm!=fmap_det_tracks.end(); ++itm ) cout<<" ("<<itm->first<<" "<<itm->second<<")";
  cout<<endl;*/

  while( (!found) && (!err)  ){
    
    std::map<int, pair<int,int> >::iterator it = fmap_det_tracks.find( par_id );
    if( it==fmap_det_tracks.end() ){  
      err = true;

      //   cout<<"!!!Error, chain is broken on track: "<<par_id<<endl;
    }
    par_id = (it->second).first;
    process_id = (it->second).second;

    if( IsRegistered( par_id ) ) found = true;

    //  cout<<"("<<par_id<<","<<it->second<<")="<<found<<endl;
  }

  // flip sign of parent ID to signify that this parent is indirectly associated
  par_id = -par_id;

  // store results for this input gtrkid for future retrieval
  flast_trkid = gtrkid;
  flast_resultid = par_id;
  flast_processid = process_id;

  // store results in input references
  gtrkid = flast_resultid;
  process_id = flast_processid;
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
     cout << "AMSG4DetectorInterface::Construct()-I-chord was "<<fieldMgr->GetChordFinder()->GetDeltaChord()<<endl;
     fieldMgr->GetChordFinder()->SetDeltaChord(delta);
     cout << "AMSG4DetectorInterface::Construct()-I-chord set "<<fieldMgr->GetChordFinder()->GetDeltaChord()<<endl;



double mxs=G4TransportationManager::GetTransportationManager()->GetPropagatorInField()->GetLargestAcceptableStep();
G4TransportationManager::GetTransportationManager()->GetPropagatorInField()->SetLargestAcceptableStep(50);

     cout << "AMSG4DetectorInterface::Construct()-I-MaxStep was/ set "<<mxs<<" "<<G4TransportationManager::GetTransportationManager()->GetPropagatorInField()->GetLargestAcceptableStep()<<endl;

 } 

if(!_pv){
  cout << "AMSG4DetectorInterface::Construct-I-Building Geometry "<<endl;
  //G4GeometryTolerance::GetInstance()->SetSurfaceTolerance(10000);
  cout << "AMSG4DetectorInterface::Construct-I-SurficeToleranceSetTo "<<G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()<<" mm" <<endl;;

  AMSJob::gethead()->getgeom()->MakeG4Volumes();
//---
  if(G4FFKEY.TFNewGeant4>0){ //TOF New Geant4 Geometry->down to mother
    cout << "AMSG4DetectorInterface::Construct-I-New TOFB Geometry "<<endl;
    TofSimUtil::Head->MakeTOFG4Volumes(AMSJob::gethead()->getgeom()->down());
   }


  G4PhysicalVolumeStore* phystore = G4PhysicalVolumeStore::GetInstance();
if(G4FFKEY.OverlapTol &&phystore){
  cout <<" AMSgvolume::MakeG4Volumes-I-Total of "<<phystore->size()<<" volumes found"<<endl;
  for(int i=0;i<phystore->size();i++){
    G4VPhysicalVolume*p=(*phystore)[i];
     if(p && p->CheckOverlaps(1000,G4FFKEY.OverlapTol*cm,false)){
       cerr<<"  AMSgvolume::MakeG4Volumes-E-OverlapFoundFor "<<p->GetName()<<endl;
}
}
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
if(!Step)return;
  // just do as in example N04
  // don't really understand the stuff
     
  GCTRAK.istop=0;


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
  AMSEvent::gethead()->SetEventSkipped(false);
  static integer freq=10;
  static integer trig=0;
  trig=(trig+1)%freq;
  static bool report=true; 
  if(trig==0 && AMSgObj::BookTimer.check("GEANTTRACKING")>AMSFFKEY.CpuLimit){
    freq=1;
    GCTRAK.istop =1;
    G4Track * Track = Step->GetTrack();
    Track->SetTrackStatus(fStopAndKill);
    AMSEvent::gethead()->seterror();
    if(report)cerr<<"AMSG4EventAction::EndOfEventAction-E-CpuLimitExceeded Run Event "<<" "<<AMSEvent::gethead()->getrun()<<" "<<AMSEvent::gethead()->getid()<<" "<<AMSgObj::BookTimer.check("GEANTTRACKING")<<" "<<AMSFFKEY.CpuLimit<<endl;
    report=false;
    AMSEvent::gethead()->SetEventSkipped(true);
   return;
  }
  else if(freq<10){
   freq=10;
   report=true;
  }

  /// <SH> Scan of element abundance along the track
  if (MISCFFKEY.ScanElemAbundance && MISCFFKEY.G4On == 1
                                  && MISCFFKEY.G3On == 0) {
    static double Z1 =   53.1*cm;  // Before L2
    static double Z2 =  -29.3*cm;  // After  L8
    static double Z3 = -135.7*cm;  // Before L9

    static double par1[4] = { 63.14, 48.4,  158.9, 67.14 };
    static double par9[4] = { 46.62, 34.5, -135.7, 67.14 };
    static AMSPoint crp1;
    static AMSPoint crp9;

    if (crp1.norm() == 0) crp1.setp(par1[0], par1[1], par1[2]);
    if (crp9.norm() == 0) crp9.setp(par9[0], par9[1], par9[2]);

    G4StepPoint *PrePoint  = Step->GetPreStepPoint ();
    G4StepPoint *PostPoint = Step->GetPostStepPoint();
    G4Track *trk = Step->GetTrack();

    if (trk && trk->GetParentID() == 0 && PrePoint && PostPoint &&
	PrePoint->GetPosition().z() > Z3) {

      enum { NZ = 14, NE = 6 };
                         // 1   2  3  4  5  6  7  8   9 10 11 12 13 14
      static int iZ[NZ] = { 0, -1,-1,-1,-1, 1, 2, 3, -1,-1,-1,-1, 4, 5 };
      static int evno =  -1;
      static int nevt =   0;
      static int nxsc =   0;
      static int fpl1 =   0;
      static int intv = 100;
      static double wsum[NE*3];
      static double wavg[NE*3];
      static double xsec[NE];
      if (evno < 0) {
	G4cout << "AMSG4SteppingAction::UserSteppingAction-I-Initialize "
	       << "element abundance vector" << G4endl;
	for (int i = 0; i < NE*3; i++) wavg[i] = 0;
	for (int i = 0; i < NE;   i++) xsec[i] = 0;
	nevt = 0;
      }
      if (evno != AMSEvent::gethead()->getEvent()) {
	  evno  = AMSEvent::gethead()->getEvent();
	for (int i = 0; i < NE*3; i++) wsum[i] = 0;
	fpl1 = 0;
      }

      G4double x1 =  PrePoint->GetPosition().x();
      G4double y1 =  PrePoint->GetPosition().y();
      G4double z1 =  PrePoint->GetPosition().z();
      G4double z2 = PostPoint->GetPosition().z();

      // Layer1 check
      if (z1/cm > crp1.z() && z2/cm < crp1.z() &&
	  fabs(x1)/cm < crp1.x() && fabs(y1)/cm < crp1.y() && 
	  (x1*x1+y1*y1)/cm/cm < par1[3]*par1[3]) fpl1 = 1;

      // Layer9 check
      if (z1 > Z3 && z2 < Z3 && 
	  fabs(x1)/cm < crp9.x() && fabs(y1)/cm < crp9.y() && 
	  (x1*x1+y1*y1)/cm/cm < par9[3]*par9[3] && fpl1) {

	for (int i = 0; i < NE*3; i++) wavg[i] += wsum[i];
	nevt++;

	if (nevt%intv == 0) {
	  if (intv < 100000) intv *= 10;
	  G4cout<< std::fixed << std::setprecision(1);
	  G4cout << "## Element abundance (rho*L*w/A) in cm^2 "
		 << "averaged with Nevent= " << nevt << G4endl;
	  G4cout << "## Element abundance (1) Z>" << Z1/cm
		 << " (2) Z> " << Z3/cm
		 << " (3) " << Z2/cm << " >Z> " << Z3/cm << G4endl;
	  G4cout << "##" << G4endl;
	  G4cout << "## Element abundance    : "
		 << "  1(H)   6(C)   7(N)   8(O) 13(Al) 14(Si)" << G4endl;
	  G4cout<< std::fixed << std::setprecision(4);
	  for (int i = 0; i < 3; i++) {
	    G4cout << "## Element abundance ("<<i+1<<"): ";
	    for (int j = 0; j < NE; j++) G4cout << wavg[i*NE+j]/nevt << " ";
	    G4cout << G4endl;
	  }
	  G4cout << "##" << G4endl;
	  if (xsec[0] > 0) {
	    G4cout << "## Element cross section: "
		   << "  1(H)   6(C)   7(N)   8(O) 13(Al) 14(Si)" << G4endl;
	    G4cout << "## Cross section (mb)   : ";
	    G4cout << std::fixed << std::setprecision(3)
		   << xsec[0]/millibarn << " ";
	    G4cout << std::setprecision(2);
	    for (int j = 1; j < NE; j++) G4cout << xsec[j]/millibarn << " ";
	    G4cout << G4endl;
	    G4cout<< std::fixed << std::setprecision(4);
	    for (int i = 0; i < 3; i++) {
	      G4double sum = 0;
	      for (int j = 0; j < NE; j++) sum += xsec[j]*wavg[i*NE+j]/nevt;
	      if (sum > 0) {
		G4cout << "## Element rel.xsec. ("<<i+1<<"): ";
		for (int j = 0; j < NE; j++)
		  G4cout << xsec[j]*wavg[i*NE+j]/nevt/sum << " ";
		G4cout << G4endl;
	      }
	    }
	    G4cout << "##" << G4endl;
	  }
	}
      }

      const  G4DynamicParticle *prj = 0;
      G4HadronInelasticProcess *prc = 0;
      if (nxsc < NE) {
	prj = trk->GetDynamicParticle();
	const G4ParticleDefinition *pt = trk->GetParticleDefinition();
	G4ProcessTable *ptbl = G4ProcessTable::GetProcessTable();

	G4String pname = pt->GetParticleName();

	if (pname == "proton") {
	  prc = (G4HadronInelasticProcess *)ptbl
	                                 ->FindProcess("ProtonInelastic", pt);
	  if (!prc) prc = (G4HadronInelasticProcess *)ptbl
		                         ->FindProcess("protonInelastic", pt);
	}
	else if (pname == "alpha") {
	  prc = (G4HadronInelasticProcess *)ptbl
	                                 ->FindProcess("AlphaInelastic", pt);
	  if (!prc) prc = (G4HadronInelasticProcess *)ptbl
		                         ->FindProcess("alphaInelastic", pt);
	}
	else if (pname == "GenericIon")
	  prc = (G4HadronInelasticProcess *)ptbl
	                                 ->FindProcess("IonInelastic", pt);

	if (!prc) nxsc = NE;
      }

      G4Material *material = PrePoint->GetMaterial();
      if (material) {
	G4double slen = Step->GetStepLength();
	G4double dens = material->GetDensity();
	for (G4int i = 0; i < material->GetNumberOfElements(); i++) {
	  const G4Element *elm = material->GetElement(i);
	  G4int Z = elm->GetZ();
	  if (Z < 1 || NZ < Z) continue;

	  G4int iz = iZ[Z-1];
	  if (iz < 0 || NE <= iz) continue;

	  if (xsec[iz] == 0 && prc && prj) {
	    xsec[iz] = prc->GetMicroscopicCrossSection(prj, elm, 0);
	    nxsc++;
	  }

	  G4double W = material->GetFractionVector()[i];
	  G4double A = elm->GetA();
	  G4double S = W*dens/A*slen*cm2;
	  G4double z = PrePoint->GetPosition().z();
	  if (z > Z1) wsum[iz]      += S;
	  if (z > Z3) wsum[iz+NE]   += S;
	  if (z < Z2 &&
	      z > Z3) wsum[iz+NE*2] += S;
	}
      }
    }
  }
  /// <SH> Measurement of element abundance on the track

   /// <CC> BEGIN material scanning information
   // only for Geant4
   if (MISCFFKEY.SaveMCTrack == 1 && MISCFFKEY.G4On == 1 && MISCFFKEY.G3On == 0)
   {
      static double ECAL_Z = ECALDBc::ZOfEcalTopHoneycombSurface()*cm;
      static double ene_threshold = MISCFFKEY.MCTrackMinEne*MeV;
      static bool   save_sec = MISCFFKEY.SaveMCTrackSecondary == 1;

      G4Track *step_trk = Step->GetTrack();

      if (step_trk != NULL)
      {
         G4int pid = step_trk->GetParentID();
         G4int tid = step_trk->GetTrackID();

         G4ParticleDefinition *pdef = step_trk->GetDynamicParticle()->GetDefinition();
         G4String part_name = pdef->GetParticleName();
         int part_info;
         int g3code = AMSJob::gethead()->getg4physics()->G4toG3(part_name, part_info);
         bool isDummy = g3code == AMSG4Physics::_G3DummyParticle;
         bool isNeutrino = pdef->GetPDGCharge() == 0 && pdef->GetLeptonNumber() != 0;

         // save only secondary particles (but not neutrinos or dummy particles) if datacard is set
         if (pid == 0 || (save_sec && !isNeutrino && !isDummy))
         {
            G4StepPoint *preStepPoint = Step->GetPreStepPoint();
            G4StepPoint *postStepPoint = Step->GetPostStepPoint();

            if (preStepPoint != NULL)
            {
               G4ThreeVector pre_pos = preStepPoint->GetPosition();
               G4ThreeVector post_pos = postStepPoint->GetPosition();

               bool isBacksplash = pre_pos.z() < ECAL_Z && post_pos.z() > ECAL_Z;

               G4double ekin = step_trk->GetKineticEnergy();

               // save only primary particle and secondary particles above ECAL (or backsplash) and with kinetic energy greater than the threshold
               if (pid == 0 || ((pre_pos.z() > ECAL_Z || isBacksplash) && ekin >= ene_threshold))
               {
                  G4Material *material = preStepPoint->GetMaterial();

                  if (material != NULL)
                  {
                     G4double stlen = Step->GetStepLength();
                     G4double x0 = stlen/(material->GetRadlen());
                     G4double lambda = stlen/(material->GetNuclearInterLength());
                     G4double enetot = Step->GetTotalEnergyDeposit()/GeV*1000;
                     G4double eneion = enetot - Step->GetNonIonizingEnergyDeposit()/GeV*1000;

                     const char *tmp_name = preStepPoint->GetPhysicalVolume()->GetName().data();
                     char vol_name[5] = "";
                     for (int i = 0; i < 4; ++i)
                     {
                        vol_name[i] = tmp_name[i];
                     }

                     float pos[3];
                     for (int i = 0; i < 3; ++i)
                     {
                        pos[i] = pre_pos[i]/cm;
                     }
                     map <int,float>felmap;
                      for (int i=0; i<material->GetNumberOfElements(); ++i) {
                      int Zi = (*material->GetElementVector())[i]->GetZ();
                      float Ni=material->GetVecNbOfAtomsPerVolume()[i];
                      felmap.insert(make_pair(Zi,Ni));
                      }
               
                     AMSmctrack *genp = new AMSmctrack(x0, lambda, pos, vol_name, stlen, enetot, eneion, tid, felmap);
                     AMSEvent::gethead()->addnext(AMSID("AMSmctrack", 0), genp);
                  }
               }
            }
         }
      }
   }
   /// <CC> END material scanning information

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
    int gtrkid = Track->GetTrackID();

    FillPrimaryInfo(Step);
    FillBackSplash(Step);

    //cout << " track id "<<GCKINE.itra<<" "<<GCTRAK.nstep<<endl;
    G4ParticleDefinition* particle =Track->GetDefinition();
    int parinfo;
    GCKINE.ipart=AMSJob::gethead()->getg4physics()->G4toG3(particle->GetParticleName(),parinfo);
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

        //looging if given track is already registered in MCEventG. If not, find closest parent and change sign of gtrkid
        AMSG4EventAction* evt_act = (AMSG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction();
        const G4VProcess* process = Track->GetCreatorProcess();
        int process_id = process ? ( (process->GetProcessType() << 24) | (process->GetProcessSubType() & 0xFFFFFF) ) : 0;
        evt_act->FindClosestRegisteredTrack( gtrkid, process_id );

	G4ParticleDefinition* particle =Track->GetDefinition();
        int parinfo;
	GCKINE.ipart=AMSJob::gethead()->getg4physics()->G4toG3(particle->GetParticleName(),parinfo);
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
				       GCTRAK.destep,GCTRAK.gekin,GCTRAK.step,GCKINE.ipart,GCKINE.itra, gtrkid, process_id);   
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
	     // cout <<" tracker "<<endl;

            int part_code = GCKINE.ipart;

            int A = int((particle->GetPDGMass()/GeV)/0.931494061+0.5);
            int Z = abs(int(GCKINE.charge));
            int S = (GCKINE.charge>0) ? 1 : -1;

            // encode charge in in first 5 bits of the status
            int status = Z;
            if (Z>31) status = 31;

            // try to encode in a short int the new G4 particle ID for ions
            if (abs(part_code)>1000000000) {
              int a = (A>99) ? 99 : A;
              int z = (Z>99) ? 99 : Z;
              int s = (S==1) ? 0 : 1;  
              part_code = 10000 + s*10000 + z*100 + a;
            }

            int sign = -1;
            if (Track->GetTrackID()==1) sign = 1; // TrackID is 1 for primaries  
            TrSim::sitkhits(
              PrePV->GetCopyNo(),
              GCTRAK.vect,
              GCTRAK.destep,
              GCTRAK.step,
              sign*abs(part_code), // to be sure of sign
              gtrkid,
              status 
            );

	  }
#else
	  if(GCTRAK.destep && GrandMother && GrandMother->GetName()(0)=='S' 
	     &&  GrandMother->GetName()(1)=='T' && GrandMother->GetName()(2)=='K'){
	    AMSTrMCCluster::sitkhits(PrePV->GetCopyNo(),GCTRAK.vect,
				     GCTRAK.destep,GCTRAK.step,/*GCKINE.ipart,*/gtrkid);   
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
//----Birk Retune
            number cb1=1.1;
            geant deer=dee;
            geant beta=(PostPoint->GetVelocity()+PrePoint->GetVelocity())/2./(29.9792*cm/nanosecond);//covert to beta
	    if(G4FFKEY.TFNewGeant4>0){
               dee=dee/(1.+cb1*atan(TFMCFFKEY.birk*dedxcm*0.1/cb1));
//             dee=dee/(1.+TFMCFFKEY.birk*dedxcm*0.1);
            }
	    else                     dee=dee/(1+c*atan(rkb/c*dedxcm));
	    //cout<<"   > continue TOF: part="<<iprt<<" x/y/z="<<x<<" "<<y<<"  "<<z<<" Edep="<<dee<<" numv="<<numv<<"  step="<<pstep<<" dedx="<<tdedx<<endl;
	    AMSTOFMCCluster::sitofhits(numv,GCTRAK.vect,dee,tof,beta,deer,GCTRAK.step,GCKINE.itra,GCKINE.ipart,gtrkid);
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
					 dee,GCTRAK.tofg, gtrkid);
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
	      static unsigned int np=0; if(np==0)cout<<"... in ECAL: numv="<<PrePV->GetCopyNo()<<" "<<dee<<" "<<PrePV->GetMother()->GetCopyNo()<<" "<<PrePV->GetName()<<" "<<GCTRAK.vect[0]<<" "<<GCTRAK.vect[1]<<" "<<GCTRAK.vect[2]<<" "<<PrePV->GetMother()->GetName()<<" "<<PrePV->GetMother()->GetLogicalVolume()<<" "<<GCTRAK.destep<<endl;
		  ++np;
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
				       (GCKINE.itra!=1?100:0),
                                       gtrkid,
                                       GCKINE.itra);
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
					 Status_LG_origin,
                                         gtrkid,
                                         GCKINE.itra);
	      else
		AMSRichMCHit::sirichhits(GCKINE.ipart,
					 volume,
					 vect,
					 GCKINE.vert,
					 GCKINE.pvert,
					 0,
                                         gtrkid,
                                         GCKINE.itra);
	    }else if(GCTRAK.nstep!=0){	 
	      AMSRichMCHit::sirichhits(GCKINE.ipart,
				       volume,
				       GCTRAK.vect,
				       GCKINE.vert,
				       GCKINE.pvert,
				       Status_No_Cerenkov,
                                       gtrkid,
                                       GCKINE.itra);
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
    int parinfo;
  GCKINE.ipart=AMSJob::gethead()->getg4physics()->G4toG3(particle->GetParticleName(),parinfo);
  if(GCKINE.ipart==Cerenkov_photon){
    //--new TOF part
    if(G4FFKEY.TFNewGeant4==1){
      G4ThreeVector phver=aTrack->GetPosition();
      G4String volnam=aTrack->GetVolume()->GetName();
      bool IsTof=(volnam(0)=='T'&&(volnam(1)=='O'||volnam(1)=='F'));//not RICH region

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
  if( !aTrack->GetCurrentStepNumber() ){ //don't fill twice for the same track
    AMSmceventg::FillMCInfoG4( aTrack );
    AMSG4EventAction* evt_act =(AMSG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction();
    const G4VProcess* process = aTrack->GetCreatorProcess();
    int process_id = process ? ( (process->GetProcessType() << 24) | (process->GetProcessSubType() & 0xFFFFFF) ) : 0;
    evt_act->AddRegisteredParentChild( aTrack->GetTrackID(), aTrack->GetParentID(), process_id );
  }

  return fWaiting;
}


const double AMSG4SteppingAction::facc_pl[21] = {
  158.92,  141.2, 86.1, 
  64.425,  65.975, 61.325, 62.875,
  53.06,   29.228, 25.212, 1.698, -2.318, -25.212, -29.228,
  -62.875, -61.325, -64.425, -69.975, 
  -135.882,
  -142.792,
  -158.457
};

void AMSG4SteppingAction::FillPrimaryInfo( const G4Step *Step){

  G4Track * aTrack = Step->GetTrack();
  if (aTrack->GetParentID()!=0) return;
  
  double z_post = Step->GetPostStepPoint()->GetPosition().z();
  double z_pre = Step->GetPreStepPoint()->GetPosition().z();


  int pl = 0;
  int ipl = -1;


  //if partcle is died, or go out of the mother volume, record it
  if( aTrack->GetTrackStatus()!=fAlive ) ipl = 100;

  while( (pl<21) && (ipl<0) ){
    if(  ( (z_post < facc_pl[pl]*cm) && (z_pre > facc_pl[pl]*cm ) ) || //top-bottom
         ( (z_pre < facc_pl[pl]*cm) && (z_post > facc_pl[pl]*cm ) ) ){ //bot-top

      ipl = pl;
    }
    pl++;
  }

  if (ipl < 0 ) return;
 
  G4double ekin = aTrack->GetKineticEnergy();
  G4String name = aTrack -> GetDynamicParticle() -> GetDefinition() -> GetParticleName();
  G4ThreeVector mom = aTrack->GetMomentumDirection();
    int partinfo;
    int g3code = AMSJob::gethead()->getg4physics()->G4toG3( name,partinfo );
    partinfo++;
  if(g3code==AMSG4Physics::_G3DummyParticle)return;

  G4ThreeVector pos = aTrack->GetPosition();
  AMSPoint point( pos.x(), pos.y(), pos.z() );

  AMSDir dir( mom.x(), mom.y(), mom.z() );
  int nskip = -1000; //indicates that this is primary acceptance particle
  nskip -= ipl;
   G4ParticleDefinition * pdef = aTrack->GetDynamicParticle()->GetDefinition();
   float charge=pdef->GetPDGCharge();
   float mass=pdef->GetPDGMass()/GeV;

  AMSEvent::gethead()->addnext(
                               AMSID("AMSmceventg",0),
                               new AMSmceventg( g3code,  aTrack->GetTrackID(), aTrack->GetParentID(), ekin/GeV, point/cm, dir,partinfo,mass,charge,nskip) // negetive code for secondary
                               );

}


void AMSG4SteppingAction::FillBackSplash( const G4Step *Step){
  static double ECAL_Z = ECALDBc::ZOfEcalTopHoneycombSurface();
  double z_post = Step->GetPostStepPoint()->GetPosition().z();
  double z_pre = Step->GetPreStepPoint()->GetPosition().z();

  if( (z_pre < ECAL_Z*cm) && (z_post >  ECAL_Z*cm) ){
    G4Track * aTrack = Step->GetTrack();
    G4ParticleDefinition * pdef = aTrack->GetDynamicParticle()->GetDefinition();
    G4int pdgid = aTrack->GetDynamicParticle()->GetDefinition()->GetPDGEncoding();
    bool isNeutrino = pdef->GetPDGCharge()==0 and pdef->GetLeptonNumber()!=0;
    G4double ekin = aTrack->GetKineticEnergy();
    if( ekin < 1*MeV or isNeutrino ) return;
    G4String name = aTrack -> GetDynamicParticle() -> GetDefinition() -> GetParticleName();
    G4ThreeVector mom = aTrack->GetMomentumDirection();
    // 
    // ams form of the track/particle information
    //
    int partinfo;
    int g3code = AMSJob::gethead()->getg4physics()->G4toG3( name,partinfo );
    partinfo++;
    partinfo=-partinfo;
    if(g3code==AMSG4Physics::_G3DummyParticle)return;
    G4ThreeVector pos = aTrack->GetPosition();
    AMSPoint point( pos.x(), pos.y(), pos.z() );
    float parr[3] = { float(pos.x()), float(pos.y()), float(pos.z()) };
    AMSDir dir( mom.x(), mom.y(), mom.z() );
    int nskip = -2; //indicates that this is step of backsplashed particle
   float charge=pdef->GetPDGCharge();
   float mass=pdef->GetPDGMass()/GeV;
    AMSEvent::gethead()->addnext(
                                 AMSID("AMSmceventg",0),
                                 new AMSmceventg( g3code,  aTrack->GetTrackID(), aTrack->GetParentID(), ekin/GeV, point/cm, dir,partinfo,mass,charge,nskip) // negetive code for secondary
                                 );
    AMSG4EventAction* evt_act = (AMSG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction();
    evt_act->AddRegisteredTrack( aTrack->GetTrackID() );
  }
}
