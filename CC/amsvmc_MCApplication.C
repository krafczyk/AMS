#include <TROOT.h>
#include <TInterpreter.h>
#include <TVirtualMC.h>
#include <TRandom.h>
#include <TPDGCode.h>
#include <TVector3.h>
#include <Riostream.h>
#include <TGeoManager.h>
#include <TVirtualGeoTrack.h>
#include <TParticle.h>
#include <TCanvas.h>
#include <TMath.h>
#include "TGeant3.h"
#include "TGeant3TGeo.h"
extern TGeant3* geant3;

#include <fenv.h>

#include <TMCProcess.h>
#include "amsvmc_MCApplication.h"
#include "amsvmc_MCStack.h"
#include "amsvmc_PrimaryGenerator.h"
#include "amsvmc_DetectorConstruction.h"
#include "amsvmc_MagField.h"
// //===============VMC_Geant4+++++++++
#include "TG4RunConfiguration.h"
#include "TGeant4.h"
#include "g4physics.h"
 extern TGeant4* geant4;
 class TGeant4;
 class TG4RunConfiguration;
///===================In order to simulate TRD
extern "C" void gentrd_(float &, float &, float&, float &,int &, float[],float[]);
extern "C" void gentrdi_(float &, float &, float &,int &, float[],float[]);
extern "C" void ephtrd_(float &, float &, float &);
extern "C" float sabs_(float &,float &,int &);
extern "C" float isotr_(float []);
extern "C" void diffscat_(int &, float&, int&, float&,int &, float &);
extern "C" float derand_(float &, float&, float&, int &,float[], float []);
extern "C"  void g3zinit_();
extern "C"  void g3part_();
//**************in order to set cut
#include "gmat.h"
#include "snode.h"
#include "amsgobj.h"
#include "job.h"
#include "richdbc.h"
//****************geant3.C include file
#include "typedefs.h"
#include "cern.h"
#include "mceventg.h"
#include "amsgobj.h"
#include "commons.h"
#include <math.h>
#ifdef _PGTRACK_
#include "TrMCCluster.h"
#include "TrRecon.h"
#else

#include "trmccluster.h"
#include "trid.h"
#endif
//#include <new.h>
#include <limits.h>
#include "extC.h"
//#include <trigger3.h>
#include "job.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include "gvolume.h"
#include "gmat.h"
#include "event.h"
#include "mccluster.h"
#include "cont.h"
#include "trrec.h"
#include "daqevt.h"
#include "richdbc.h"
#include "richid.h"
#include "producer.h"
#include "geantnamespace.h"         
#include "status.h"
#include "ntuple.h"
#ifdef _PGTRACK_
#include "MagField.h"
#endif
#ifdef __AMSDEBUG__
static integer globalbadthinghappened=0;

void DumpG3Commons(ostream & o){
  o<< " DumpG3Commons  "<<endl;
  o<< GCKINE.ipart<<endl;
  for(int i=0;i<7;i++)o<<GCTRAK.vect[i]<<" ";
  o<<endl;
  o<<GCTRAK.gekin<<" "<<GCKING.ngkine<<" "<<GCTMED.numed<<endl;
}
#endif


//===========To Set Cut=======

#include "gmat.h"
#include "id.h"
#include "snode.h"
#include "node.h"

//== TRD function for geant3(adjusted)=========
extern "C" void simde_(int&);
extern "C" void trphoton_(int&);
extern "C" void simtrd_(int& );
extern "C" void getscanfl_(int &scan);
extern "C" void abinelclear_();


#include "TCallf77.h"
extern "C" void g3fpart_(const int&, DEFCHARD, int&, float&, float&, float&, float*, int&);
extern "C" void g3birk_(float&);
#define G3BIRK g3birk_


/// \cond CLASSIMP
ClassImp(amsvmc_MCApplication)
/// \endcond

//_____________________________________________________________________________
amsvmc_MCApplication::amsvmc_MCApplication(const char *name, const char *title, int fileMode)
    : TVirtualMCApplication(name,title),
      fPrintModulo(1),
      fOptPhotonNo(0),
      fECALedep(0),
      fEventNo(0),
      fVerbose(0),
      fPrimaryGenerator(0),
      fDetConstruction(0),
      fStack(0),
      fRootManager("amstest",2),
      fMagField(0),
      CurrentLevel(0),
      CurrentMed(0),
      CurrentMat(0),
      vmc_isvol(0),
      vmc_nstep(0),
      vmc_istop(0),
      vmc_version(0),
      vmc_nevent(1),
      CurrentVol(0),
      copyNo(0),
      MotherVol(0),
      MotherVolName(0),
      mothercopyNo(0),
      GrandMotherVol(0),
      GrandMotherVolName(0)
{
  /// Standard constructor
  // Create a user stack
  fStack = new amsvmc_MCStack(80000);
  // Create detector construction
  fDetConstruction = new amsvmc_DetectorConstruction();
  // Create a primary generator
  fPrimaryGenerator = new amsvmc_PrimaryGenerator(fStack);
  // Constant magnetic field (in kiloGauss)
  fMagField = new amsvmc_MagField();
}

//_____________________________________________________________________________
amsvmc_MCApplication::amsvmc_MCApplication()
  : TVirtualMCApplication(),
    fPrintModulo(1),
    fOptPhotonNo(0),
    fECALedep(0),
    fEventNo(0),
    fStack(0),
    fDetConstruction(0),
    fPrimaryGenerator(0),
    fRootManager(), 
    fMagField(0),
    CurrentLevel(0),
    CurrentMed(),
    CurrentMat(),
    vmc_isvol(0),
    vmc_nstep(0),
    vmc_istop(0),
    vmc_version(0),
    vmc_nevent(1),
    CurrentVol(),
    copyNo(0),
    MotherVol(0),
    MotherVolName(0),
    mothercopyNo(0),
    GrandMotherVol(0),
    GrandMotherVolName(0)
{    
  /// Default constructor
}

//_____________________________________________________________________________
amsvmc_MCApplication::~amsvmc_MCApplication() 
{
  /// Destructor  
  delete fStack;
  delete fDetConstruction;
  delete fPrimaryGenerator;
  delete fMagField;
  delete gMC;
  gMC = 0;
}

//
// private methods
//

//_____________________________________________________________________________
void amsvmc_MCApplication::RegisterStack()
{
  cout<<"DEBUG: start RegisterStack()"<<endl;
  fRootManager.Register("stack", "amsvmc_MCStack", &fStack);   
}  

//
// public methods
//

//_____________________________________________________________________________

void amsvmc_MCApplication::InitMC()
{    



h_edep_absorption=new TH1F("h_edep_absorption","h_edep_absorption",2000,0,100);
h_edep_absorption2=new TH1F("h_edep_absorption2","h_edep_absorption",2000,0,100);
h_edep_ionization=new TH1F("h_edep_ionization","h_edep_ionization",2000,0,100);




  if(vmc_version==2){ //Geant4_VMC Initialization
    g3zinit_();
    g3part_();
    TG4RunConfiguration* runConfiguration = new TG4RunConfiguration("geomRoot", "emStandard+optical","stepLimiter+specialCuts");
    //       TG4RunConfiguration* runConfiguration = new TG4RunConfiguration("geomRoot", "emStandard+optical","stepLimiter+specialCuts");
    //          TG4RunConfiguration* runConfiguration = new TG4RunConfiguration("geomRoot", "QGSP+optical","stepLimiter+specialCuts");   //Note that specialControls automatically enabled in the geant4_vmc library
    TGeant4* geant4 = new TGeant4("TGeant4", "The Geant4 Monte Carlo", runConfiguration);
    AMSG4Physics * pph = new AMSG4Physics();
    pph->_init();
    AMSJob::gethead()->getg4physics()=pph;



    GCFLAG.IEVENT=1;
  }

  bool doinitgeomertry=1;
  if(doinitgeomertry){

    cout<<"About to set process"<<endl;
    gMC->SetProcess("MULS",1);
    gMC->SetProcess("LOSS",1); 
    gMC->SetProcess("RAYL",1);
    gMC->SetProcess("HADR",1);
    gMC->SetProcess("CKOV",1);
    gMC->SetProcess("PHOT",1); 
    gMC->SetProcess("COMP",1);
    gMC->SetProcess("BREM",1);
    gMC->SetProcess("DRAY",1);
    gMC->SetProcess("ANNI",1);
    gMC->SetProcess("LABS",1);
    gMC->SetProcess("PAIR",1);
    gMC->SetProcess("LABS",1);
    gMC->SetProcess("DCAY",1);
    gMC->SetProcess("MUNU",1);
    
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
    long seed[3]={0,0,0};
    seed[0]=GCFLAG.NRNDM[0];
    seed[1]=GCFLAG.NRNDM[1];
    CLHEP::HepRandom::setTheSeeds(seed);
  }

  fVerbose.InitMC();
  gMC->SetStack(fStack);
  gMC->SetMagField(fMagField);
  gMC->Init();
  cout<<"About to call g3pions"<<endl;
  g3pions_();
  RegisterStack();
  GCFLAG.ITEST=200;
}                                   

//_____________________________________________________________________________
void amsvmc_MCApplication::RunMC(Int_t nofEvents)
{    
  /// Run MC.
  /// \param nofEvents Number of events to be processed
  //***********Finish the initilization first
  gMC->BuildPhysics(); 
  fVerbose.RunMC(nofEvents);
  gMC->ProcessRun(nofEvents);
  FinishRun();
}

//_____________________________________________________________________________
void amsvmc_MCApplication::FinishRun()
{    
  /// Finish MC run.
  //  cout<<"DEBUG: in amsvmc_MCApplication::FinishRun()"<<endl;

  fRootManager.GetFile()->cd();
h_edep_absorption->Write();
h_edep_absorption2->Write();
h_edep_ionization->Write();

  fRootManager.WriteAll();
}

//_____________________________________________________________________________
void amsvmc_MCApplication::ReadEvent(Int_t i) 
{

}  

//_____________________________________________________________________________
void amsvmc_MCApplication::ConstructGeometry()
{    
  cout<<"DEBUG:start of Ex03MCApplication::ConstructGeometry"<<endl;
  gMC->SetRootGeometry();
}

//_____________________________________________________________________________

void amsvmc_MCApplication::PreInit()
{


//   char* str;
//   fstream file_op("Version.txt",ios::in);
//   file_op >> str;
//     cout << str;
//   file_op.close();

//  cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Version input:"<<str[0]<<endl; 
//  cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Version input2:"<<str<<endl;

//   if (str[0]=='1') vmc_version=1;  //Geant3
//   else vmc_version=2;  //Geant4


//    vmc_version=IOPA.version;



      //===========From geant.C====
      cout.sync_with_stdio();   
      GINIT();
      new AMSJob();
      AMSJob::gethead()->data();
      GFFGO();
      vmc_nevent=GCFLAG.NEVENT;
      vmc_version=IOPA.VMCVersion;

  if(vmc_version==2)  //For geant4_vmc, here only initilizing some data structure, the initialize of geant4_vmc in InitMC()
    {
      cout<<"~~~~~~~~~~~~~~~Geant4_VMC is selected"<<endl;
       
      //===========From geant.C====
      //      cout.sync_with_stdio();   
      //      GINIT();
      //      new AMSJob();
      //      AMSJob::gethead()->data();
      //      GFFGO();
      //      cout<<"in InitMC(), before TG4RunConfiguration"<<endl;

    }

  if( vmc_version ==1 ){ //For geant3_vmc, the initilizin of ginit, gffgo is embaded in the geant3_vmc library.
    cout<<"Check A"<<endl;
    TGeant3* geant3    = new TGeant3TGeo("C++ Interface to Geant3 with TGeo geometry",15000000); 
    cout<<"Check AA"<<endl;
  }
}

void amsvmc_MCApplication::ConstructOpGeometry()
{
  cout<<"In amsvmc_MCApplication::ConstructOpGeometry()"<<endl;
  const  char air[]="AIR";  //The First Medium must be set, can be change
  cout<<"Finding Medium:"<<air<<endl;
  AMSgtmed *_vmcmed =(AMSgtmed *)AMSgObj::GTrMedMap.getp(AMSID(air,0));
  for(  int lastmedno=_vmcmed->getmedia();lastmedno<_vmcmed->GetLastMedNo(); )
    {
      Int_t mediumId = gMC->MediumId(_vmcmed->getname());
      if ( mediumId && mediumId!=3 ) {  //The medium VACUUM can not be set as optical!!!
	if(_vmcmed->getyOptical()=='Y'){      
	  cout<<"~~~~~~~` Optical Properties need to be Set :"<<_vmcmed->getname()<<", number:"<<lastmedno<<endl;
	  number *_vmcindex=_vmcmed->VMCrindex();
	  if(_vmcindex[0]==0){
	    for(int j=0;j<_vmcmed->VMCnument();j++) _vmcindex[j]=0;
	  }
	  gMC->SetCerenkov(mediumId,_vmcmed->VMCnument(),_vmcmed->VMCpmom(),_vmcmed->VMCabsl(),_vmcmed->VMCeff(),_vmcindex);
	}
      }
      _vmcmed=_vmcmed->next();
      lastmedno=_vmcmed->getmedia();
    }      
}

void amsvmc_MCApplication::InitGeometry()
{    
  cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~In amsvmc_MCApplication::InitGeometry()~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
  
  const  char air[]="AIR";
  cout<<"Finding Medium:"<<air<<endl;
  AMSgtmed *_vmcmed =(AMSgtmed *)AMSgObj::GTrMedMap.getp(AMSID(air,0));
  for(  int lastmedno=_vmcmed->getmedia();lastmedno<_vmcmed->GetLastMedNo(); )
    {
      cout<<"~~~~~~~` Tracking medium :"<<_vmcmed->getname()<<", number:"<<lastmedno<<endl;
      Int_t mediumId = gMC->MediumId(_vmcmed->getname());
      if ( mediumId ) {
	gMC->Gstpar(mediumId, "CUTGAM", _vmcmed->CUTGAM());
	gMC->Gstpar(mediumId, "CUTELE", _vmcmed->CUTELE());
	gMC->Gstpar(mediumId, "CUTNEU", _vmcmed->CUTNEU());
	gMC->Gstpar(mediumId, "CUTHAD", _vmcmed->CUTHAD());
	gMC->Gstpar(mediumId, "CUTMUO",_vmcmed->CUTMUO());
	//       gMC->Gstpar(i,"BCUTE",0.0005);
	//       gMC->Gstpar(i,"BCUTM",0.0005);
	//       gMC->Gstpar(i,"DCUTE",0.0005);
	//       gMC->Gstpar(i,"DCUTM",0.0005);
	//       gMC->Gstpar(i,"PPCUTM",0.01);
	if(_vmcmed->getyb()=='Y'){      
	  cout<<"Birk's law is required, BIRKA:"<<_vmcmed->BIRKS1()<<", "<<_vmcmed->BIRKS2()<<","<<_vmcmed->BIRKS3()<<endl;
	  gMC->Gstpar(mediumId,"BIRK1",_vmcmed->BIRKS1());
	  gMC->Gstpar(mediumId,"BIRK2",_vmcmed->BIRKS2());
	  gMC->Gstpar(mediumId,"BIRK3",_vmcmed->BIRKS3());
	}
      }
      _vmcmed=_vmcmed->next();
      lastmedno=_vmcmed->getmedia();
    }
}

//_____________________________________________________________________________
void amsvmc_MCApplication::AddParticles()
{    

}

//_____________________________________________________________________________


void amsvmc_MCApplication::AddIons()
{    
  //===============this should be added in further, some ion already added from g3pion() in InitMC()

}

//_____________________________________________________________________________
void amsvmc_MCApplication::GeneratePrimaries()
{    
  /// Fill the user stack (derived from TVirtualMCStack) with primary particles.
  fPrimaryGenerator->GeneratePrimaries(vmc_version);
}

//_____________________________________________________________________________
void amsvmc_MCApplication::BeginEvent()
{    
  fEventNo++;
  cout << "\n---> Begin of event: " << fEventNo << endl;

  if(vmc_version==1)GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
  else {
    GCFLAG.NRNDM[0]=CLHEP::HepRandom::getTheSeeds()[0];
    GCFLAG.NRNDM[1]=CLHEP::HepRandom::getTheSeeds()[1];
  }
  cout <<"  **** RANDOM NUMBER GENERATOR Before starting event: "<<GCFLAG.NRNDM[0]<<" "<<GCFLAG.NRNDM[1]<<endl;


  fOptPhotonNo=0;
  fECALedep=0;
}

//_____________________________________________________________________________
void amsvmc_MCApplication::BeginPrimary()
{    
  //================User action when Primary particle begin=======
}

//_____________________________________________________________________________
void amsvmc_MCApplication::PreTrack()
{
 //================User action when new track begin, may not work for Fluka_vmc=======
}

//_____________________________________________________________________________


void amsvmc_MCApplication::Stepping()
{    



  float TRD_edep_ionization=0;
  float TRD_edep_absorption=0;
  float TRD_edep_absorption2=0;









  if(gMC->IsNewTrack())
    {
      vmc_nstep=0;
      vmc_istop=0;
    }
  else vmc_nstep++;   //Count for step number for every track, may not work for FlukaVMC

  bool isstepping=1;
  if(isstepping  && vmc_istop==0){
    int copyNo = 0;
    int CurrentVolId = gMC->CurrentVolID(copyNo);
    const char* CurrentVolName = gMC->CurrentVolName();
    geant vmc_destep = (geant) gMC->Edep();
    double vmc_double_destep = gMC->Edep();
    double vmc_double_vect[7];
    double vmc_totalenergy;
    double vmc_kine;
    geant vmc_vect[7];
    geant vmc_mass,vmc_gekin,vmc_step, vmc_gekin_test,vmc_tofg, vmc_charge;
    gMC->TrackPosition(vmc_double_vect[0],vmc_double_vect[1],vmc_double_vect[2]);
    
    //===============================================================Optimized

    if(gMC->IsTrackEntering() || gMC->IsNewTrack()  ) 
      { 
     	CurrentVol = gGeoManager->GetCurrentVolume();
      	CurrentMed = CurrentVol->GetMedium();
      	CurrentMat = CurrentVol->GetMaterial();
      	CurrentLevel= gGeoManager->GetLevel();
     	vmc_isvol = CurrentMed->GetParam(0);
    if(CurrentLevel>1)
      {
	MotherVol = gGeoManager->GetMother(1);
	MotherVolName = (char*) MotherVol->GetName();
	mothercopyNo = MotherVol->GetNumber();
      }
    else{
      MotherVol = 0;
      MotherVolName = 0;
      mothercopyNo = 0;
    }
    if(CurrentLevel>2){
      GrandMotherVol = gGeoManager->GetMother(CurrentLevel-2);        // Note that gGeoManger navigator already go to mothervolume, here grandmother stand for the volume in level 3 That is: AMSG->FMOT->XXX
      GrandMotherVolName =  (char*) GrandMotherVol->GetName();
    }
    else{
      GrandMotherVol =0;   
      GrandMotherVolName =0;
    }
      }
    
    gMC->TrackMomentum(vmc_double_vect[3],vmc_double_vect[4],vmc_double_vect[5],vmc_double_vect[6]);  
    vmc_totalenergy=gMC->Etot();     
     
    vmc_kine=vmc_totalenergy-vmc_mass;

    vmc_mass=(geant)gMC->TrackMass();
    vmc_charge = (geant) gMC->TrackCharge();
    vmc_tofg = (geant) gMC->TrackTime();
    vmc_step=(geant) gMC->TrackStep();
    vmc_double_vect[6] = sqrt(pow(vmc_double_vect[3],2)+pow(vmc_double_vect[4],2)+pow(vmc_double_vect[5],2));
    vmc_gekin = (geant) (vmc_totalenergy - vmc_mass);    //   KinematicEnergy = Totalenergy - mass 
    if(vmc_double_vect[6]!=vmc_double_vect[6])vmc_istop=2 ;       //To prevent nan problem

    vmc_double_vect[3]/=vmc_double_vect[6];
    vmc_double_vect[4]/=vmc_double_vect[6];
    vmc_double_vect[5]/=vmc_double_vect[6];

    for (int i=0;i<7;i++)
      {
	vmc_vect[i]=(geant)vmc_double_vect[i];
      }

    integer vmc_itra = 1+gMC->GetStack()->GetCurrentTrackNumber();   //Track number

    //================================ Convert PDG/G4 code to G3 code
    int vmc_ipart=-1;

    if(vmc_version == 1) //For geant3_vmc, we have function to convert between geant3 and PDG code
      {     
	vmc_ipart=gMC->IdFromPDG(gMC->GetStack()->GetCurrentTrack()->GetPdgCode()); 
	if(gMC->TrackPid() == 50000050) 
	  {
	    vmc_ipart = Cerenkov_photon;
	  } 
      }
     else{ //For geant4_vmc, we can only use the ams code conversion
       if(gMC->TrackPid()==50000050) vmc_ipart=Cerenkov_photon;
       else
 	{
 	  const char* currentparticlename = gMC->ParticleName(gMC->TrackPid());
 	  G4String G4currentparticle(currentparticlename); 
 	  vmc_ipart =  AMSJob::gethead()->getg4physics()->G4toG3(G4currentparticle);
 	}
     }

    bool isprimary=0;
    if(vmc_itra==1)isprimary=1;
    int vmc_ngkine = gMC->NSecondaries();

    bool vmc_istrackentering=gMC->IsTrackEntering();
    bool vmc_isnewtrack=gMC->IsNewTrack();  //It seems this do not work for cerenkov photon ?
    bool vmc_istrackexiting = gMC->IsTrackExiting();
    bool vmc_istrackout= gMC->IsTrackOut();
    bool vmc_istrackstop= gMC->IsTrackStop();
    bool vmc_istrackdisappeared = gMC->IsTrackDisappeared();
     
    int vmc_inwvol = 0;
    if(vmc_istrackexiting) vmc_inwvol=2;
    if(vmc_istrackout) vmc_inwvol=3;
    if(vmc_istrackentering || vmc_isnewtrack) vmc_inwvol=1;
  
    if( !AMSEvent::gethead()->HasNoCriticalErrors())return;
  
#ifdef __AMSDEBUG__
    if( globalbadthinghappened){
      cerr<<" a " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
      DumpG3Commons(cerr);
    }
#endif
    if(!AMSEvent::gethead()->HasNoCriticalErrors()){
      vmc_istop=1;
      return;
    }
    static integer freq=10;
    static integer trig=0;
    trig=(trig+1)%freq;
    //  if(trig==0 && freq>1)AMSgObj::BookTimer.start("GUSTEP");
    if(trig==0 && AMSgObj::BookTimer.check("GEANTTRACKING")>AMSFFKEY.CpuLimit){
      //    if(freq>1)AMSgObj::BookTimer.stop("GUSTEP");
      freq=1;
      vmc_istop =1;
      //    cout<<"trig==0 && AMSgObj::BookTimer.check(GEANTTRACKING)>AMSFFKEY.CpuLimit"<<endl;
      return;
    }
    else if(freq<10)freq=10;

    if (vmc_double_vect[6] <0) 
      { 
	vmc_istop=2 ;
      }

    TParticle *currentParticle=gMC->GetStack()->GetCurrentTrack();

    bool trdsimulation=1;
    if(trdsimulation   && vmc_version==1  ){  
      //
      //=================== TRD simulation here=============================================
      //
      //  if(trig==0 && freq>1)AMSgObj::BookTimer.start("TrdRadiationGen");
      if(vmc_version==2) cout<<"in trdsimulation, geant4"<<endl;
      float _param1= CurrentMed->GetParam(8);
      float _param2= CurrentMed->GetParam(9);
      float _param3= CurrentMed->GetParam(10);
      float _param4= CurrentMed->GetParam(11);
      float _param5= CurrentMed->GetParam(12);
      float _param6= CurrentMed->GetParam(13);

      int scan=0;
      getscanfl_(scan);

      if(!scan){
	if(TRDMCFFKEY.mode <3 && TRDMCFFKEY.mode >=0) {

	  if(vmc_charge!=0 && vmc_step >0){       //simtrd
	    bool trd_test=1; //in original ams code, the trd radiation happen in TRXX, which medium is vacuum, instead of in TRD9 which medium is TRD radiator, this is only to mimic this behavior

	    /*
	    if(trd_test) 
	      {
		const char* CurrentMedName = CurrentMed->GetName();
		if(CurrentVolName[0]=='T' &&  CurrentVolName[1]=='R' && CurrentVolName[2]!='D' && CurrentMedName[0]=='V' && CurrentMedName[1]=='A'){
		  _param2= 123654;
		  _param3= 0;
		  _param4= 1;
		  _param5= 0.68;
		  _param6=0;
		  _param1=5 ;
		}
	      }

	    */

	    /*
	    if(trd_test) 
	      {
		const char* CurrentMedName = CurrentMed->GetName();
		if(CurrentVolName[0]=='T' &&  CurrentVolName[1]=='R' && CurrentVolName[2]!='D' && CurrentVolName[3]!='1' && CurrentVolName[3]!='0' && CurrentMedName[0]=='V' && CurrentMedName[1]=='A'){
		  _param2= 123654;
		  _param3= 0;
		  _param4= 1;
		  _param5= 0.68;
		  _param6=0;
		  _param1=5 ;
		}
	      }
	    */

	    //	    if(_param1 >= 4 && _param2==TRDMCFFKEY.g3trd && _param3 >=0 && _param3 <=2 && !(CurrentVolName[0]=='T' &&  CurrentVolName[1]=='R' && CurrentVolName[2]=='D' && CurrentVolName[3]=='9'))
	    if(_param1 >= 4 && _param2==TRDMCFFKEY.g3trd && _param3 >=0 && _param3 <=2)
	      {
	    
		geant gamma=0;
		if(vmc_mass) gamma=gMC->Etot()/vmc_mass;
	    
	    
		if(gamma>100){
		  //       Run
		  integer ntr=0;
		  float etr[1000],str[1000];
		  float vect[7];
		  geant step= vmc_step;
		  geant charge= std::abs(gMC->TrackCharge());
		  if(_param4==3 && vmc_inwvol==1) gentrdi_(charge,gamma,step,ntr,etr,str);
		  else if(_param4==1)
		    {
		      gentrd_(charge,gamma,_param5,step,ntr,etr,str);
		    } 
		  if (ntr){                  
		    float eloss=0;
		    for(int i=0;i<ntr;i++)
		      {
			int toBeDone=1;	      
			int parent=gMC->GetStack()->GetCurrentParentTrackNumber();
			int pdg=22;
			if(_param3<2) pdg=0;  //PDG=0 IS ROOTINO, IN ANALOG TO GEANTINO
			double px=etr[i]*vmc_vect[3];
			double py=etr[i]*vmc_vect[4];
			double pz=etr[i]*vmc_vect[5];
			double e=etr[i];
			double vx=vmc_vect[0]-str[i]*vmc_vect[3];
			double vy=vmc_vect[1]-str[i]*vmc_vect[4];
			double vz=vmc_vect[2]-str[i]*vmc_vect[5];
			double tof = 0;
			double polx = 0;
			double poly = 0;
			double polz = 0;
			TMCProcess  mech=kPPhotoelectric;
			int ntr;
			double weight=1;
			int is =0;
			gMC->GetStack()->PushTrack(toBeDone,parent,pdg,px,py,pz,e,vx,vy,vz,tof,polx,poly,polz,mech,ntr,weight,is);
			eloss+=etr[i];
		      }	  
		    //=============Change the current track property======
		    double currenttote=vmc_totalenergy-eloss;
		    double currentkine = vmc_totalenergy-eloss-vmc_mass;
		    float vmc_momemtum=sqrt(currentkine*(currentkine+2*vmc_mass));
		    vmc_totalenergy-=eloss;
		    if(vmc_step>0)
		      {
			float  vmc_kine=vmc_totalenergy-vmc_mass;
			float  tem_px=vmc_momemtum*vmc_vect[3];
			float  tem_py=vmc_momemtum*vmc_vect[4];
			float  tem_pz=vmc_momemtum*vmc_vect[5];
			float  tem_tote=vmc_totalenergy;
			float  tem_pdg=gMC->TrackPid();			      
			float  tem_vx=vmc_vect[0];
			float  tem_vy=vmc_vect[1];
			float  tem_vz=vmc_vect[2];
			int parent=gMC->GetStack()->GetCurrentParentTrackNumber();
			double tof = 0;
			double polx = 0;
			double poly = 0;
			double polz = 0;
			TMCProcess  mech=kPPhotoelectric ;      // Just put one, need to be change if nessesary
			int ntr;
			double weight=1;
			int is =0;
			gMC->GetStack()->PushTrack(1,parent,tem_pdg,tem_px,tem_py,tem_pz,tem_tote,tem_vx,tem_vy,tem_vz,0,0,0,0,mech,ntr,weight,is);
			vmc_istop=1;
		      }
		  }
		}
	      }
	  }//===================simtrd End

	  //	  float _param1= CurrentMed->GetParam(8);
	  //	  float _param2= CurrentMed->GetParam(9);
	  //	  float _param3= CurrentMed->GetParam(10);
	  //	  float _param4= CurrentMed->GetParam(11);
	  //	  float _param5= CurrentMed->GetParam(12);
	  //	  float _param6= CurrentMed->GetParam(13);

	  if(TRDMCFFKEY.mode<2)
	    {    //trphoton_()
	      //==========================TR-photon absorption=========================
	      int nwbuf,jtm,im,g3trd;
	      float stea,sabs;
	      if(_param1 >= 4 && _param2==TRDMCFFKEY.g3trd && _param3 >=0 && _param3 <=1)
		{
		  //stop TR photon
		  int diff_flag=0;
		  float Epi=0;
		  int param4=(float) _param4;
		  float vmc_kine=vmc_totalenergy-vmc_mass;
		  diffscat_(param4,vmc_step,vmc_ipart,vmc_kine,diff_flag,Epi);  // Simulated photon->Pi+ Pi- ?
		  if(diff_flag==1)
		    {
		      cout<<"DIFFSCAT Effect is being called~~~~~~~~~~~"<<endl;
		      for(int i=0;i<2;i++)
			{
			  float pi_px=vmc_vect[3];    ///GKIN(1,NGKINE) = (VECT(4))   Seem to be not right
			  float pi_py=vmc_vect[4];
			  float pi_pz=vmc_vect[5]; 
			  float pi_e=Epi;
			  int pi_pdg;
			  if(i==0)  pi_pdg=211;
			  if(i==1)  pi_pdg=-211;
			  float  pi_vx=vmc_vect[0];
			  float  pi_vy=vmc_vect[1];
			  float  pi_vz=vmc_vect[2];
			  int parent=gMC->GetStack()->GetCurrentParentTrackNumber();
			  double tof = 0;
			  double polx = 0;
			  double poly = 0;
			  double polz = 0;
			  TMCProcess  mech=kPPhotoelectric ;      // Just put one, need to be change if nessesary
			  int ntr;
			  double weight=1;
			  int is =0;
			  gMC->GetStack()->PushTrack(1,parent,pi_pdg,pi_px,pi_py,pi_pz,pi_e,pi_vx,pi_vy,pi_vz,0,0,0,0,mech,ntr,weight,is);			      
			}
		      vmc_totalenergy-=2*Epi;
		      vmc_kine-=2*Epi;
		      float vmc_momemtum=sqrt(pow(vmc_totalenergy,2)-pow(vmc_mass,2));
		      if(vmc_step>0)
			{
			  float  vmc_kine=vmc_totalenergy-vmc_mass;
			  float  tem_px=vmc_momemtum*vmc_vect[3];
			  float  tem_py=vmc_momemtum*vmc_vect[4];
			  float  tem_pz=vmc_momemtum*vmc_vect[5];
			  float  tem_tote=vmc_totalenergy;
			  float  tem_pdg=gMC->TrackPid();
			  float  tem_vx=vmc_vect[0];
			  float  tem_vy=vmc_vect[1];
			  float  tem_vz=vmc_vect[2];
			  int parent=gMC->GetStack()->GetCurrentParentTrackNumber();
			  double tof = 0;
			  double polx = 0;
			  double poly = 0;
			  double polz = 0;
			  TMCProcess  mech=kPPhotoelectric ;      // Just put one, need to be change if nessesary
			  int ntr;
			  double weight=1;
			  int is =0;
			  gMC->GetStack()->PushTrack(1,parent,tem_pdg,tem_px,tem_py,tem_pz,tem_tote,tem_vx,tem_vy,tem_vz,0,0,0,0,mech,ntr,weight,is);			      
			  vmc_istop=1;
			}
		    }

		  if(gMC->TrackPid()==0 && vmc_istop==0 && vmc_step >0){      
		    if(_param4!=4)   //Absorbtion simulation 
		      {
			float kine=(vmc_totalenergy-vmc_mass)*1000000;//        ! E in keV
			int param4=(float) _param4;
			stea=sabs_(kine, vmc_step, param4);//do not need to re write;
			if(stea <=vmc_step)
			  {
			    vmc_vect[0]=vmc_vect[0]+(stea-vmc_step)*vmc_vect[3];
			    vmc_vect[1]=vmc_vect[1]+(stea-vmc_step)*vmc_vect[4];
			    vmc_vect[2]=vmc_vect[2]+(stea-vmc_step)*vmc_vect[5];
			    if(_param4==3)
			      {
				float e2=0;
				float flag=0;
				char cmech[4];
				ephtrd_(kine,flag,e2);     // 
				if(flag==1){    // implementation of ephtrd
				  float tem_p[3]={0,0,0};
				  isotr_(tem_p);
				  float  tem_px=tem_p[0]*e2;
				  float  tem_py=tem_p[1]*e2;
				  float  tem_pz=tem_p[2]*e2;
				  float  tem_tote=e2;
				  float tem_pdg=0;
				  float  tem_vx=vmc_vect[0];
				  float  tem_vy=vmc_vect[1];
				  float  tem_vz=vmc_vect[2];
				  int parent=gMC->GetStack()->GetCurrentParentTrackNumber();
				  double tof = 0;
				  double polx = 0;
				  double poly = 0;
				  double polz = 0;
				  TMCProcess  mech=kPPhotoelectric ;      // Just put one, need to be change if nessesary
				  int ntr;
				  double weight=1;
				  int is =0;
				  gMC->GetStack()->PushTrack(1,parent,tem_pdg,tem_px,tem_py,tem_pz,tem_tote,tem_vx,tem_vy,tem_vz,0,0,0,0,mech,ntr,weight,is);
				}
			      }
			    vmc_istop=2;
			    vmc_destep = kine/1000000;
			    float vmc_momemtum=0;

			    TRD_edep_absorption=vmc_destep*1000000;
			    vmc_totalenergy=0;
			  }
		      }
		    else if (_param4==4){
		      vmc_istop=2;
		      vmc_destep = vmc_kine;//vmc_totalenergy;
		      vmc_totalenergy=0;
			    TRD_edep_absorption2=vmc_destep*1000000;
		    }




		  }
		}
	      else {  //just convert geantino to photon
		if(gMC->TrackPid()==0 && vmc_istop==0 && vmc_step >0){
		  float vmc_kine=vmc_totalenergy-vmc_mass;
		  float  tem_px=vmc_kine*vmc_vect[3];
		  float  tem_py=vmc_kine*vmc_vect[4];
		  float  tem_pz=vmc_kine*vmc_vect[5];
		  float  tem_tote=vmc_totalenergy;
		  float  tem_pdg=22;
		  float  tem_vx=vmc_vect[0];
		  float  tem_vy=vmc_vect[1];
		  float  tem_vz=vmc_vect[2];
		  int parent=gMC->GetStack()->GetCurrentParentTrackNumber();
		  double tof = 0;
		  double polx = 0;
		  double poly = 0;
		  double polz = 0;
		  TMCProcess  mech=kPPhotoelectric ;      // Just put one, need to be change if nessesary
		  int ntr;
		  double weight=1;
		  int is =0;
		  gMC->GetStack()->PushTrack(1,parent,tem_pdg,tem_px,tem_py,tem_pz,tem_tote,tem_vx,tem_vy,tem_vz,0,0,0,0,mech,ntr,weight,is);			      
		  vmc_istop=1;
		  vmc_destep=0;
		  vmc_kine=0;
		  vmc_totalenergy=0;
		}
	      }
	    }	  //	===  trphoton_() END

	  if(TRDMCFFKEY.mode==0)     //simde_()
	    {
	      if(std::abs(vmc_charge)==1 && vmc_istop ==0 && vmc_step >=0)// keep GEANT3 DESTEP for IONS
		{
		  int mdr=10,ndr,i;
		  float edr[mdr],sdr[mdr];
		  float pmoddr,thtdr,phidr,rndm[1],dcute,cutele;
		  dcute=   35.01e-6;  //what is the energy unit ?  //can be asscess throuhg materail informatiln ?	      
		  cutele=  ECMCFFKEY.cutge ;
		  if(_param1 >= 4 && _param2==TRDMCFFKEY.g3trd && _param3 ==0 && _param4 ==3)
		    {
		      vmc_totalenergy+=vmc_destep;
		      float vmc_kine=vmc_totalenergy-vmc_mass;
		      float gamma=vmc_totalenergy/vmc_mass;
		      float de = derand_(gamma,dcute,vmc_step,ndr,edr,sdr);
		      //		      if(de==0)cout<<"TRDDEBUG Warning, de=0 , step:"<<vmc_step<<endl;

		      for(int i=0;i<min(ndr,mdr);i++)  /// put Delta-Rays on stack
			{
			  float temp_pmoddr=edr[i]*edr[i]+2*edr[i]*vmc_mass;
			  pmoddr=sqrt(temp_pmoddr);
			  thtdr=acos(sqrt(edr[i])/(edr[i]+2*vmc_mass));
			  float dummy=0;
			  float rndm[1];
			  rndm[0]=RNDM(dummy);
			  phidr = 6.28318530*rndm[0];
			  //			  cout<<"in stepping, rndm[0]="<<rndm[0]<<endl;
			  //			  cout<<"in stepping, phidr="<<phidr<<endl;
			  float temp_px=pmoddr*(vmc_vect[3]*sin(thtdr)*cos(phidr));
			  float temp_py=pmoddr*(vmc_vect[4]*sin(thtdr)*sin(phidr));
			  float temp_pz=pmoddr*(vmc_vect[5]*cos(thtdr));
			  float temp_e=edr[i];
			  float temp_pdg=kElectron;
			  float temp_vx=vmc_vect[0]-sdr[i]*vmc_vect[3];
			  float temp_vy=vmc_vect[1]-sdr[i]*vmc_vect[4];
			  float temp_vz=vmc_vect[2]-sdr[i]*vmc_vect[5];
			  int temp_parent=gMC->GetStack()->GetCurrentParentTrackNumber();
			  double tof = 0;
			  double polx = 0;
			  double poly = 0;
			  double polz = 0;
			  TMCProcess  mech=kPPhotoelectric ;      // Just put one, need to be change if nessesary
			  int ntr;
			  double weight=1;
			  int is =0;
			  gMC->GetStack()->PushTrack(1,temp_parent,temp_pdg,temp_px,temp_py,temp_pz,temp_e,temp_vx,temp_vy,temp_vz,0,0,0,0,mech,ntr,weight,is);
			  cout<<"Delta-ray in TRD, pushed in to track stack"<<endl;
			  vmc_kine-=edr[i];
			  vmc_totalenergy-=edr[i];
			}
			  
		      if(vmc_kine-de > cutele)
			{
			  vmc_destep=de;
			  vmc_kine = vmc_kine - de;
			  vmc_totalenergy = vmc_totalenergy-de;
			  float  vmc_momemtum=sqrt(vmc_kine*(vmc_kine+2*vmc_mass));

			  // 	      		h_edep_ionization->Fill(vmc_destep*1000000);
			  //cout<<"++++++~TRDDEBUG: In "<<CurrentVolName<<" destep:"<<vmc_destep*1000000<<", Particle: "<<vmc_ipart<<",  num: "<<vmc_itra<<", Material:"<<CurrentMat->GetName()<<endl;
			  if(vmc_step>0)
			    {
			      float  vmc_kine=vmc_totalenergy-vmc_mass;
			      float  tem_px=vmc_momemtum*vmc_vect[3];
			      float  tem_py=vmc_momemtum*vmc_vect[4];
			      float  tem_pz=vmc_momemtum*vmc_vect[5];
			      float  tem_tote=vmc_totalenergy;
			      float  tem_pdg=gMC->TrackPid();
			      float  tem_vx=vmc_vect[0];
			      float  tem_vy=vmc_vect[1];
			      float  tem_vz=vmc_vect[2];
			      int parent=gMC->GetStack()->GetCurrentParentTrackNumber();
			      double tof = 0;
			      double polx = 0;
			      double poly = 0;
			      double polz = 0;
			      TMCProcess  mech=kPPhotoelectric ;      // Just put one, need to be change if nessesary
			      int ntr;
			      double weight=1;
			      int is =0;
			      gMC->GetStack()->PushTrack(1,parent,tem_pdg,tem_px,tem_py,tem_pz,tem_tote,tem_vx,tem_vy,tem_vz,0,0,0,0,mech,ntr,weight,is);
			      vmc_istop=1;
			    }
			}
		      else{
			vmc_istop=2;
			vmc_destep=vmc_kine;
			//	      cout<<"~~~~~TRDDEBUG: In "<<CurrentVolName<<" destep:"<<vmc_destep*1000000<<", Particle: "<<vmc_ipart<<",  num: "<<vmc_itra<<", Material:"<<CurrentMat->GetName()<<endl;
			/// 	      		h_edep_ionization->Fill(vmc_destep*1000000);

		      }

			    TRD_edep_ionization=vmc_destep*1000000;

		    }
		}
	    }
	}
	else if(TRDMCFFKEY.mode ==3){

	}
      }
    }
    //===================================End of TRD simulation==============================

#ifdef __AMSDEBUG__
    if( globalbadthinghappened){
      cerr<<" b " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
    }
#endif


    if(CurrentLevel > 1){
      try{
	bool dotrd=1;
	if(dotrd)
	  {
	    // TRD
	    if(vmc_destep != 0 && vmc_isvol != 0 && CurrentVolName[0]=='T' && CurrentVolName[1]=='R'  && CurrentVolName[2]=='D' && CurrentVolName[3]=='T'){
	      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
	      // Secondary particles are tagged with - for ipart
	      AMSTRDMCCluster::sitrdhits(copyNo,vmc_vect,vmc_destep,vmc_gekin,vmc_step,isprimary?-vmc_ipart:vmc_ipart,vmc_itra);   
	      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");



	      //	    cout<<"Stepping Start"<<endl;
	    if(TRD_edep_absorption>0) {
h_edep_absorption->Fill(TRD_edep_absorption);
// cout<<"Absorption:"<<TRD_edep_absorption<<endl;  
}
	    if(TRD_edep_absorption2>0) {
h_edep_absorption2->Fill(TRD_edep_absorption2);
// cout<<"Absorption:"<<TRD_edep_absorption<<endl;  
}
	    if(TRD_edep_ionization>0){
 h_edep_ionization->Fill(TRD_edep_ionization);
 //      cout<<"DEBUG: In "<<CurrentVolName<<" destep:"<<vmc_destep*1000000<<", Particle: "<<vmc_ipart<<",  num: "<<vmc_itra<<", Material:"<<CurrentMat->GetName()<<endl;



 // cout<<"ionization:"<<TRD_edep_ionization<<endl;	    
}
	    //	    cout<<"Stepping Stop"<<endl;

	    //	      if(TRD_edep_absorption>0) h_edep_absorption->Fill(TRD_edep_absorption);
	    //	      if(TRD_edep_ionization>0) h_edep_ionization->Fill(TRD_edep_ionization);



	    } 
	  }

	bool dotracker=1;
	if(dotracker && GrandMotherVolName)
	  {
	    //Traker Here
		    if(CurrentLevel+1 >=3 && vmc_destep != 0 && vmc_isvol != 0 && GrandMotherVolName[0]== 'S' && GrandMotherVolName[1]=='T' && GrandMotherVolName[2]=='K'){


	      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
#ifdef _PGTRACK_
	      TrRecon::sitkhits(GCVOLU.number[lvl],GCTRAK.vect,
				GCTRAK.destep,GCTRAK.step,GCKINE.ipart);   
#else


	      //	      cout<<"Copy No:"<<copyNo<<", "<<vmc_destep<<", "<<vmc_step<<", "<<vmc_ipart<<endl;
	      AMSTrMCCluster::sitkhits(copyNo,vmc_vect,vmc_destep,vmc_step,vmc_ipart);   
#endif
	      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
#ifdef __AMSDEBUG__
	      if( globalbadthinghappened){
		cerr<<" d " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
	      }
#endif
	    }
	  }

	bool dotof=1;
	if(dotof)
	  { //TOF Here
	    if( CurrentLevel+1 ==3 && vmc_isvol != 0 && vmc_charge !=0 && CurrentVolName[0]=='T' && CurrentVolName[1]=='F'){// <=== charged part. in "TFnn"



	      int tflv;  
	      char name[4];
	      char media[20];
	      geant t,x,y,z;
	      geant de,dee,dtr2,div,tof,prtq,pstep;
	      static geant xpr(0.),ypr(0.),zpr(0.),tpr(0.);
	      static geant stepsum(0.),estepsum(0.);
	      static geant sscoo[3]={0.,0.,0.};
	      static geant sstime(0.);
	      static int nsmstps(0);
	      static geant ssbx(0),ssby(0),ssbz(0);
	      geant trcut2(0.25);// Max. transv.shift (0.5cm)**2
	      geant stepmin(0.25);//(cm) min. step/cumul.step to store hit(0.5cm/2)
	      geant estepmin(1.e-5);//"small steps" buffer is considered as empty when Eaccumulated below 10kev
	      geant coo[3],dx,dy,dz,dt,stepel;
	      geant wvect[6],snext,safety;
	      int i,nd,numv,iprt,numl,numvp,tfprf(0);
	      static int numvo(-999),iprto(-999);
    
	      tflv = CurrentLevel;
    
	      //  tflv=GCVOLU.nlevel-1;  
	      //---> print some GEANT standard values(for debugging):
	      //  if(GCFLAG.IEVENT==3118)tfprf=1;
	      //  if(GCFLAG.IEVENT==3118)TOF2DBc::debug=1;
    

	      numl=tflv+1;
	      numv=copyNo;
	      numvp=mothercopyNo;
	      //  numl=GCVOLU.nlevel;
	      //  numv=GCVOLU.number[numl-1];
	      //  numvp=GCVOLU.number[numl-2];
    

	      //  for(i=0;i<4;i++)name[i]=GCVOLU.names[numl-1][i];
	      //  UHTOC(GCTMED.natmed,4,media,20);
	      //


	      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");

	      iprt=vmc_ipart;
	      prtq=vmc_charge;
	      pstep=vmc_step;
	      numv=copyNo;
	      x=vmc_vect[0];
	      y=vmc_vect[1];
	      z=vmc_vect[2];
	      t=vmc_tofg;
	      de=vmc_destep;

	      //    cout<<"=====>In TOFsensvol: part="<<iprt<<" numv="<<numv<<" z="<<z<<" t/de="<<t<<" "<<de<<", vmc_inwvol:"<<vmc_inwvol<<endl;
    
	      //
	      if(vmc_inwvol==1){// <--- new volume or track : store param.
		iprto=iprt;
		numvo=numv;
		stepsum=0.;
		estepsum=0.;
		nsmstps=0;
		sstime=0;
		sscoo[0]=0;
		sscoo[1]=0;
		sscoo[2]=0;
		xpr=x;
		ypr=y;
		zpr=z;
		tpr=t;
		//      if(tfprf)cout<<"---> 1st entry in vol#:"<<numv<<" part="<<iprt<<" time="<<t<<" z="<<z<<endl;
	      }
	      //
	      else{// <--- inwvol!=1(still running through given volume or leaving it)
		//
		if(iprt==iprto && numv==numvo && de!=0.){// <-- same part. in the same volume, de!=0
		  //if(tfprf)cout<<"--->The same vol#/part:"<<numv<<" "<<iprt<<" time="<<t<<" z="<<z<<" de="<<de<<" step="<<
		  //                                                        pstep<<" stop="<<GCTRAK.istop<<endl;
		  dx=(x-xpr);
		  dy=(y-ypr);
		  dz=(z-zpr);
		  dt=(t-tpr);
		  dtr2=dx*dx+dy*dy;
		  //
		  if(pstep>=stepmin){ // <------ big step                                   //+++++++++ Is this stiill needed in new VMC ? 
		    //
		    if(dtr2>trcut2){//  big transv. shift: subdivide step
		      nd=integer(sqrt(dtr2/trcut2));
		      nd+=1;
		      dx=dx/geant(nd);
		      dy=dy/geant(nd);
		      dz=dz/geant(nd);
		      dt=dt/geant(nd);
            
		      vmc_destep=de/geant(nd);
		      vmc_step=pstep/geant(nd);
            
		      for(i=1;i<=nd;i++){//loop over subdivisions
			coo[0]=xpr+dx*(i-0.5);
			coo[1]=ypr+dy*(i-0.5);
			coo[2]=zpr+dz*(i-0.5);
			tof=tpr+dt*(i-0.5);

			dee=vmc_destep;

			if(tfprf)cout<<"-->WroBigTrsHit:numv="<<numv<<"x/y/z="<<coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" de="<<dee<<" tof="<<tof<<endl;

			if(TFMCFFKEY.birks && vmc_version==3 )G3BIRK(dee);     
			//	      cout<<"A, callAMSTOFMCCluster::sitofhits"<<endl;
			AMSTOFMCCluster::sitofhits(numv,coo,dee,tof);
		      }
		    }// ---> end of "big transv.shift"
		    //
		    else{// <---  small transv.shift(use middle of step params)
		      //
		      coo[0]=xpr+0.5*dx;
		      coo[1]=ypr+0.5*dy;
		      coo[2]=zpr+0.5*dz;
		      tof=tpr+0.5*dt;
		      dee=vmc_destep;
		      //  if(tfprf)cout<<"-->WroSmalTrsHit:numv="<<numv<<"x/y/z="<<coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" de="<<dee<<" tof="<<tof<<endl;
		      if(TFMCFFKEY.birks  && vmc_version==3 ){
			//	      cout<<"----->Bef.Birks:Edep="<<dee<<"  Q="<<GCKINE.charge<<" step="<<GCTRAK.step<<endl;
			G3BIRK(dee);
			//	      cout<<"----->Aft.Birks:Edep="<<dee<<endl;
		      }

		      //	      cout<<"B, callAMSTOFMCCluster::sitofhits"<<endl;
		      AMSTOFMCCluster::sitofhits(numv,coo,dee,tof);
		    }// ---> endof "small transv.shift"
		    //
		  }// ------> endof "big step"
		  //--------
		  else{//      <--- small step - accumulate in buffer
		    if(pstep==0 && vmc_istop>0){//abnorm.case: step=0(stop/decay/inel_inter)->just write hit 
		      //	    if(tfprf)cout<<"WroStep=0:numv="<<numv<<" de="<<de<<" z/t="<<z<<" "<<t<<" stop="<<GCTRAK.istop<<endl;
		      vmc_step=0.005;//fict.step
		      coo[0]=x;
		      coo[1]=y;
		      coo[2]=z;
		      tof=t;
		      dee=vmc_destep;
		      if(TFMCFFKEY.birks  && vmc_version==3 )G3BIRK(dee);

		      //	      cout<<"C, callAMSTOFMCCluster::sitofhits"<<endl;
		      AMSTOFMCCluster::sitofhits(numv,coo,dee,tof);
		    }
		    else{//normal case (step>0)
		      if(nsmstps>0){//buffer is not empty, check on broken sequence
			if(ssbx!=xpr || ssby!=ypr || ssbz!=zpr){//broken sequence: flush buffer and fill with current step
			  sscoo[0]/=geant(nsmstps);
			  sscoo[1]/=geant(nsmstps);
			  sscoo[2]/=geant(nsmstps);
			  sstime/=geant(nsmstps);
			  if(tfprf){
			    cout<<"-->WroSSBufBrokenSeq:numv="<<numv<<"SSB_last:x/y/z="<<ssbx<<" "<<ssby<<" "<<ssbz<<endl;
			    cout<<" CurrentStepBegin:x/y/z="<<xpr<<" "<<ypr<<" "<<zpr<<" SSB:x/y/z="<<sscoo[0]<<" "<<sscoo[1]<<" "<<sscoo[2]<<endl;
			    cout<<" Eacc/nst="<<estepsum<<" "<<nsmstps<<" tof:ssb/curr="<<sstime<<" "<<t<<endl;
			  }
			  vmc_destep=estepsum;
			  vmc_step=stepsum;
			  if(TFMCFFKEY.birks   && vmc_version==3 )G3BIRK(estepsum);
			  AMSTOFMCCluster::sitofhits(numv,sscoo,estepsum,sstime);
			  stepsum=pstep;//fill with current small step...
			  estepsum=de;
			  nsmstps=1;
			  sstime=t;
			  sscoo[0]=x;
			  sscoo[1]=y;
			  sscoo[2]=z;
			  ssbx=x;
			  ssby=y;
			  ssbz=z;
			}
			else{//continious sequence: add current small step
			  stepsum+=pstep;
			  estepsum+=vmc_destep;
			  nsmstps+=1;
			  sscoo[0]+=x;
			  sscoo[1]+=y;
			  sscoo[2]+=z;
			  sstime+=t;
			  ssbx=x;
			  ssby=y;
			  ssbz=z;
			  if(stepsum>=stepmin){//flush buffer if accumulated enough small steps 
			    sscoo[0]/=geant(nsmstps);
			    sscoo[1]/=geant(nsmstps);
			    sscoo[2]/=geant(nsmstps);
			    sstime/=geant(nsmstps);
			    //  if(tfprf)cout<<"-->WroSSBufContSeq:numv="<<numv<<"SSB:x/y/z="<<sscoo[0]<<" "<<sscoo[1]<<" "<<sscoo[2]
			    //                        <<" Eacc/nst="<<estepsum<<" "<<nsmstps<<" SSB:tof="<<sstime<<endl;
			    vmc_destep=estepsum;
			    vmc_step=stepsum;
			    if(TFMCFFKEY.birks   && vmc_version==3  )G3BIRK(estepsum);

			    //		  cout<<"D, callAMSTOFMCCluster::sitofhits"<<endl;
			    AMSTOFMCCluster::sitofhits(numv,sscoo,estepsum,sstime);
			    stepsum=0.;
			    estepsum=0.;
			    nsmstps=0;
			    sstime=0;
			    sscoo[0]=0;
			    sscoo[1]=0;
			    sscoo[2]=0;
			  }
			}//--->endof "cont.sequence"
		      }//--->endof "non-empty buffer"
		      else{//buffer is empty: fill with current smal step
			stepsum=pstep;//fill with current small step...
			estepsum=vmc_destep;
			nsmstps=1;
			sstime=t;
			sscoo[0]=x;
			sscoo[1]=y;
			sscoo[2]=z;
			ssbx=x;
			ssby=y;
			ssbz=z;
		      }//--->endof "buffer is empty"
		    }//---> endof "normal case step>0" 
		  }//---> endof "small step"
		  //--------
		  if((vmc_inwvol>=2 || vmc_istop>0 )  && estepsum>estepmin){// on leave/stop: write "small steps" buffer if not empty
		    sscoo[0]/=geant(nsmstps);
		    sscoo[1]/=geant(nsmstps);
		    sscoo[2]/=geant(nsmstps);
		    sstime/=geant(nsmstps);
		    //  if(tfprf){
		    //    cout<<"-->WroSSBufOnLeaveStop:numv="<<numv<<"SSB:x/y/z="<<sscoo[0]<<" "<<sscoo[1]<<" "<<sscoo[2]<<endl;
		    //    cout<<" STacc/Eacc/nst="<<stepsum<<" "<<estepsum<<" "<<nsmstps<<" SSB:tof="<<sstime<<" stop="<<GCTRAK.istop<<endl;
		    //  }
		    vmc_destep=estepsum;
		    vmc_step=stepsum;
		    if(TFMCFFKEY.birks  && vmc_version==3  )G3BIRK(estepsum);

		    //	      cout<<"E, callAMSTOFMCCluster::sitofhits"<<endl;
		    AMSTOFMCCluster::sitofhits(numv,sscoo,estepsum,sstime);
		  }//---> endof "on leave/stop" actions
		  //
		  xpr=x;
		  ypr=y;
		  zpr=z;
		  tpr=t;
		}// ===> end of "same part/vol, de>0"
		//
	      }// ===> endof "inwvol!=0"(still running throuhg given volume or leave it)
	      //
	      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
	      //
	    }// ===> endof "in TFnn"
	    //-------------------------------------


#ifdef __AMSDEBUG__
	    if( globalbadthinghappened){
	      cerr<<" e " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
	    }
#endif
	  }


	//

	bool doanti=1;
	if(doanti)
	  {  
	    geant dee;
	    integer manti(0),isphys,islog;
	    if(CurrentLevel==3 && CurrentVolName[0]== 'A' && CurrentVolName[1]=='N'&& CurrentVolName[2]=='T')manti=1;
	    if(vmc_destep != 0  && vmc_isvol != 0 && manti==1){
	      
	      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
	      dee=vmc_destep; 
	      if(TFMCFFKEY.birks   && vmc_version==3  )G3BIRK(dee);
	      isphys=copyNo;
	      islog=floor(0.5*(isphys-1))+1;//not used now
     
	      AMSAntiMCCluster::siantihits(isphys,vmc_vect,dee,vmc_tofg);
	      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
	    }
	    //
#ifdef __AMSDEBUG__
	    if( globalbadthinghappened){
	      cerr<<" f " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
	    }
#endif
	  }

	bool doecal=1;
	if(doecal)
	  {  //ECAL Here
	    geant dee;
	    	    if(CurrentLevel==6 && CurrentVolName[0]== 'E' && CurrentVolName[1]=='C'){
	    //	    if(CurrentVolName[0]== 'E' && CurrentVolName[1]=='C'){

	      if(vmc_destep != 0.){
	   
		if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
		dee=vmc_destep;
		if(vmc_destep<0)cout<<"----> destep<0 "<<vmc_destep<<endl;

		if(TFMCFFKEY.birks && vmc_version==1 )
		  {
		    G3BIRK(dee);
		  }

		if(dee<0)cout<<"---->Birks:dee<0 "<<dee<<" step="<<vmc_step<<" dee="<<dee
			     <<" ipart="<<vmc_ipart<<endl;


		//==============The following line implement the gbirk function for geant4, which is not implemented in vmc	   
		//   if ( mediumId ) {
		//         double birks[3]={0,0.0011,0.52};   
		//        gMC->Gstpar(mediumId,"BIRK1",birks[0]);
		//        gMC->Gstpar(mediumId,"BIRK2",birks[1]);
		//        gMC->Gstpar(mediumId,"BIRK3",birks[2]);
		//   }
	   
			   int mode=0;
	   
		 	   if(vmc_version==2 && std::abs(vmc_charge)>0.0000001 && mode<=2){
	          
		 	     float gbirk_rkb,gbirk_c;
		 	     double density=CurrentMat->GetDensity();
		 	     gbirk_rkb=0.0011/density;
		 	     gbirk_c=0.52/(density*density);
	        
		 	     if(mode==1 && std::abs(vmc_charge)>=2)gbirk_rkb=gbirk_rkb*7.2/12.6;
			     	     float dedxcm=1000*10;
		 	     dee=vmc_destep/(1.+gbirk_rkb*dedxcm+gbirk_c*dedxcm*dedxcm);
                
		 	     if(mode==1 && std::abs(vmc_charge)>=2)gbirk_c=0;
                
		 	     if(std::abs(vmc_charge)>=2){
		 	       float gamass=vmc_totalenergy+vmc_mass;
		 	       float bet2=vmc_kine*gamass/(vmc_totalenergy*vmc_totalenergy);
		 	       float bet=sqrt(bet2);
		 	       float w1=1.034-0.1777*exp(-0.08114*vmc_charge);
		 	       float w2=bet/pow(std::abs(vmc_charge),2/3);
		 	       float w3=121.4139*w2+0.0378*sin(190.7165*w2);
		 	       float charge1=vmc_charge*(1.-w1*exp(-w3));
		 	       if(charge1<0) vmc_charge=1;
		 	       float charge2=charge1*charge1;
		 	       dedxcm=dedxcm*charge2;
    
			     }
		 	     if(mode==0){
		 	       gbirk_c=gbirk_c*(density*density);
		 	       gbirk_rkb=gbirk_rkb*density;
		 	       dee=vmc_destep/(1+gbirk_c*atan(gbirk_rkb/gbirk_c*dedxcm));
			       //			       cout<<"mode=0, correction:"<<(1+gbirk_c*atan(gbirk_rkb/gbirk_c*dedxcm))<<endl;
			     }
		 	     else 
		 	       dee=vmc_destep/(1.+gbirk_rkb*dedxcm+gbirk_c*dedxcm*dedxcm);

		 }
		fECALedep+=vmc_destep;

		//		cout<<"G4BIRK: destep before, after:"<<vmc_destep<<",   "<<dee<<endl;



		AMSEcalMCHit::siecalhits(mothercopyNo,vmc_vect,dee,vmc_tofg);
		if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
	      }


	    }
	    //
       
#ifdef __AMSDEBUG__
	    if( globalbadthinghappened){
	      cerr<<" g " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
	    }
#endif
       	  }
     
     

	bool dorich=1;
	if(dorich)
	  {
	    // RICH simulation code
	    TArrayI  vmc_lmec;
	    Int_t  vmc_nmec=gMC->StepProcesses(vmc_lmec);
	    int Steppingparticle=vmc_ipart;
	    static int fscatcounter;
	    float vmc_vert[4]={(float)gMC->GetStack()->GetCurrentTrack()->Vx(),(float)gMC->GetStack()->GetCurrentTrack()->Vy(),(float)gMC->GetStack()->GetCurrentTrack()->Vz(),(float)gMC->GetStack()->GetCurrentTrack()->T()};
	    float vmc_pvert[4]={(float)gMC->GetStack()->GetCurrentTrack()->Px(),(float)gMC->GetStack()->GetCurrentTrack()->Py(),(float)gMC->GetStack()->GetCurrentTrack()->Pz(),(float)gMC->GetStack()->GetCurrentTrack()->P()};                             //This should be the momemtom when the track is in production  vertex, do not know whether this vmc function is giving such value.
	     
	    if(vmc_ipart==Cerenkov_photon){

	      if(gMC->IsNewTrack() && vmc_version ==2){
		float p = vmc_vect[6];
		bool  vmc_richpmtcut=RICHDB::detcer(p);
		if(vmc_richpmtcut) 
		  {
		    fOptPhotonNo++;
		  }
		else vmc_istop=1;
	      }
	      else if ( vmc_version ==1 && gMC->IsNewTrack()) 	    
		{
		  fOptPhotonNo++;
		}

	      if(vmc_nstep>6000) 
		{
		  vmc_istop=1;  
		  cout<<"Cerenkov_photon reach 6000 Steps, abandoned"<<endl;
		}

	      if( CurrentVolName[0]=='R' && CurrentVolName[1]=='I' && CurrentVolName[2]=='C' &&  CurrentVolName[3]=='H'){
		if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
		// 	 for(integer i=0;i<vmc_nmec;i++){            //Need to be activated in VMC ?
		// 	   if(TMCProcessName[vmc_lmec[i]]=="Cerenkov photon refraction") RICHDB::numrefm++;          //Need to knoe the corresponding process name/code for geant4, geant3,and vmc
		// 	   //		     if(GCTRAK.lmec[i]==2000) fscatcounter=1;
		// 	 }
		if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
	      }
       
	      // Added the counters for the NAF

	      if((CurrentVolName[0]=='R' && CurrentVolName[1]=='A' && CurrentVolName[2]=='D' && CurrentVolName[3]==' ')||
		 (CurrentVolName[0]=='N' && CurrentVolName[1]=='A' && CurrentVolName[2]=='F' && CurrentVolName[3]==' ')){
		if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");

		//        if(vmc_nstep!=0){
		//         for(integer i=0;i<vmc_nmec;i++)
		//            if(TMCProcessName[vmc_lmec[i]]== "Rayleigh scattering" ) RICHDB::numrayl++;
		//        }        
		//        else{
		//  	RICHDB::numrayl=0;
		//  	RICHDB::numrefm=0;
		// 	// 	fscatcounter=0;
		// 	RICHDB::nphgen++;
		//         }          

		if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
	      }

	    }

	    //     // THIS IS A TEST
	    //         if(GCTRAK.inwvol==2 && GCVOLU.names[lvl][0]=='F' &&
	    //            GCVOLU.names[lvl][1]=='O' && GCVOLU.names[lvl][2]=='I' &&
	    //            GCVOLU.names[lvl][3]=='L' && GCTRAK.nstep>1 && GCTRAK.vect[5]<0 && RICHDB::numrayl==0){
	    // 	  RICHDB::nphrad++;
	    // 	}
	    //         if(GCTRAK.inwvol==1 && GCVOLU.names[lvl][0]=='S' &&
	    //            GCVOLU.names[lvl][1]=='L' && GCVOLU.names[lvl][2]=='G' &&
	    //            GCVOLU.names[lvl][3]=='C' && GCTRAK.nstep>1 && GCTRAK.vect[5]<0 && RICHDB::numrayl==0){
	    // 	  RICHDB::nphbas++;
	    // 	}

	    if(CurrentVolName[0]=='C' && CurrentVolName[1]=='A' && CurrentVolName[2]=='T' && CurrentVolName[3]=='O' && vmc_inwvol==1){
	      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
	      if(Steppingparticle==Cerenkov_photon && vmc_isnewtrack){
		//	 cout<<"Cerenkov in CATO"<<endl;
		vmc_istop=1;
		geant xl=(RichPMTsManager::GetAMSPMTPos(mothercopyNo-1,2)-RICHDB::cato_pos()-RICotherthk/2-vmc_vect[2])/vmc_vect[5];
		geant vect[3];
		vect[0]=vmc_vect[0]+xl*vmc_vect[3];
		vect[1]=vmc_vect[1]+xl*vmc_vect[4];
		vect[2]=vmc_vect[2]+xl*vmc_vect[5];
		AMSRichMCHit::sirichhits(Steppingparticle,
					 mothercopyNo-1,
					 //                                   GCTRAK.vect,
					 vect,
					 vmc_vert,
					 vmc_pvert,
					 Status_Window-
					 (vmc_itra!=1?100:0));
	      }
	      geant vect[3];       
	      if(Steppingparticle==Cerenkov_photon && !vmc_isnewtrack){
		vmc_istop=2;
		geant xl=(RichPMTsManager::GetAMSPMTPos(mothercopyNo-1,2)-RICHDB::cato_pos()-RICotherthk/2-vmc_vect[2])/vmc_vect[5];	
		vect[0]=vmc_vect[0]+xl*vmc_vect[3];
		vect[1]=vmc_vect[1]+xl*vmc_vect[4];
		vect[2]=vmc_vect[2]+xl*vmc_vect[5];

		if(vmc_vert[2]<RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::rich_height- RICHDB::foil_height-RICradmirgap-RIClgdmirgap // in LG
		   || (vmc_vert[2]<RICHDB::RICradpos()-RICHDB::rad_height && vmc_vert[2]>RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::foil_height))
		  AMSRichMCHit::sirichhits(Steppingparticle,
					   mothercopyNo-1,
					   vect,
					   vmc_vert,
					   vmc_pvert,
					   Status_Window-
					   (vmc_itra!=1?100:0));
		else
		  AMSRichMCHit::sirichhits(Steppingparticle,
					   mothercopyNo-1,
					   vect,
					   vmc_vert,
					   vmc_pvert,
					   fscatcounter*1000+
					   (vmc_itra!=1?100:0)+
					   RICHDB::numrefm*10+
					   (RICHDB::numrayl>0?Status_Rayleigh:0));
	      }else if(!vmc_isnewtrack){	 
		AMSRichMCHit::sirichhits(Steppingparticle,
					 mothercopyNo-1,
					 vmc_vect,
					 vmc_vert,
					 vmc_pvert,
					 Status_No_Cerenkov-
					 (vmc_itra!=1?100:0));
	      }				   
	      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
	    }
     
	    // More RICH information added for MC
	    if(vmc_itra==1 && CurrentVolName[0]=='R' && 
	       CurrentVolName[1]=='A' &&CurrentVolName[2]=='D' && 
	       CurrentVolName[3]==' ' && vmc_ipart!=50 &&
	       (vmc_isnewtrack ||vmc_istrackentering ) && vmc_charge!=0 && vmc_vect[5]<0)
	      AMSRichMCHit::sirichhits(vmc_ipart,
				       0,
				       vmc_vect,
				       vmc_vect,
				       vmc_vect+3,
				       Status_primary_rad);
     
	    if(vmc_itra==1 && CurrentVolName[0]=='N' && 
	       CurrentVolName[1]=='A' &&CurrentVolName[2]=='F' && 
	       CurrentVolName[3]==' ' && vmc_ipart!=50 &&
	       (vmc_isnewtrack ||vmc_istrackentering ) && vmc_charge!=0 && vmc_vect[5]<0)
	      AMSRichMCHit::sirichhits(vmc_ipart,
				       0,
				       vmc_vect,
				       vmc_vect,
				       vmc_vect+3,
				       Status_primary_rad);

	    if(RICCONTROLFFKEY.iflgk_flag){  // This to be checked
	      if(vmc_itra==1 &&CurrentVolName[0]=='R' && 
		 CurrentVolName[1]=='A' &&CurrentVolName[2]=='D' && 
		 CurrentVolName[3]=='B' && vmc_ipart!=50 &&
		 (vmc_isnewtrack ||vmc_istrackentering )){
		AMSRichMCHit::sirichhits(vmc_ipart,
					 0,
					 vmc_vect,
					 vmc_vect,
					 vmc_vect+3,
					 Status_primary_radb);
	      }
    
	      if(vmc_itra==1 &&CurrentVolName[0]=='S' && 
		 CurrentVolName[1]=='T' &&CurrentVolName[2]=='K' 
		 && vmc_ipart!=50 && (vmc_isnewtrack ||vmc_istrackentering )) //Tracker
		AMSRichMCHit::sirichhits(vmc_ipart,
					 0,
					 vmc_vect,
					 vmc_vect,
					 vmc_vect+3,
					 Status_primary_tracker);

	      if(vmc_itra==1 &&CurrentVolName[0]=='T' && 
		 CurrentVolName[1]=='O' &&CurrentVolName[2]=='F' && 
		 CurrentVolName[2]=='H'&& vmc_ipart!=50 
		 && (vmc_isnewtrack ||vmc_istrackentering )) //TOF
		AMSRichMCHit::sirichhits(vmc_ipart,
					 0,
					 vmc_vect,
					 vmc_vect,
					 vmc_vect+3,
					 Status_primary_tof); 
	    }
	    // #ifdef __AMSDEBUG__
	    //   if( globalbadthinghappened){
	    //     cerr<<" h " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
	    //   }
	    // #endif

	  }//===========Rich simulationcode

	bool dofillmc=1;
	if(dofillmc)    AMSmceventg::FillMCInfoVMC(vmc_ipart,vmc_inwvol,CurrentLevel,CurrentVolName,vmc_vect);

      }//==========end of try
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
    }  //==============end of      if(CurrentLevel > 1)

    if(vmc_istop > 0) 	gMC->StopTrack();

  }
}

//_____________________________________________________________________________
void amsvmc_MCApplication::PostTrack()
{    
  /// User actions after finishing of each track
}





//_____________________________________________________________________________
void amsvmc_MCApplication::FinishPrimary()
{    
  /// User actions after finishing of a primary track

}


void amsvmc_MCApplication::FinishEvent()
{    
  //  cout<<"~~~~~~~~~~~~~~~~~~~~~FinishEvent()~~~~~~~~~~~~~~~~~~~"<<endl;
  fVerbose.FinishEvent();
  AMSgObj::BookTimer.start("GUOUT");
  if(    AMSEvent::gethead()->HasNoCriticalErrors()){
    RICHDB::Nph()=0;
    try{
      CCFFKEY.curtime=AMSEvent::gethead()->gettime();
      if(AMSJob::gethead()->isSimulation()){
	if(GCFLAG.IEOTRI)AMSJob::gethead()->uhend(AMSEvent::gethead()->getrun(),AMSEvent::gethead()->getid(),AMSEvent::gethead()->gettime());
	number tt=AMSgObj::BookTimer.stop("GEANTTRACKING");
#ifdef _PGTRACK_
	AMSTrTrack::SetTimeLimit(AMSFFKEY.CpuLimit-tt);
#else
	AMSTrTrack::TimeLimit()=AMSFFKEY.CpuLimit-tt;
#endif
	//        cout <<  "  tt   " <<tt<<endl;
#ifdef __AMSDEBUG__
	globalbadthinghappened=0;
#endif

	if(tt > AMSFFKEY.CpuLimit){
	  int nsec=(AMSEvent::gethead()->getC("AMSmceventg",0))->getnelem();
	  cerr<<" GEANTTRACKING Time Spent"<<tt<<" "<<nsec<<" Secondaries Generated"<<endl;
	  if(nsec==1 && tt>AMSFFKEY.CpuLimit*1.2 ){
	    // bad thing
	    (AMSEvent::gethead()->getC("AMSmceventg",0))->printC(cerr);
#ifdef __AMSDEBUG__
	    globalbadthinghappened=1;
#endif
	  }
	  throw AMSTrTrackError("SimCPULimit exceeded");
	}
      }
      // special trick to simulate/reconstruct with different mag field
      // special trick to simulate/reconstruct with different mag field
      if(TKFIELD.iniok==2)TKFIELD.iniok=3;
      if(AMSEvent::gethead()->HasNoErrors())AMSEvent::gethead()->event();
      if(TKFIELD.iniok==3)TKFIELD.iniok=2;
    }
    catch (AMSuPoolError e){
      AMSgObj::BookTimer.stop("GUOUT");
      cerr << e.getmessage()<<endl;
      AMSEvent::gethead()->seterror(2);
#ifdef __CORBA__
#pragma omp critical (g1)
      AMSProducer::gethead()->AddEvent();
#endif
      AMSEvent::gethead()->Recovery();
      return;
    }
    catch (AMSaPoolError e){
      AMSgObj::BookTimer.stop("GUOUT");
      cerr << "Event "<<AMSEvent::gethead()->getid()<<" Thread "<<AMSEvent::get_thread_num()<<" "<<e.getmessage()<<endl;
      AMSEvent::gethead()->seterror(2);
#ifdef __CORBA__
      //#pragma omp critical (g1)
      //      AMSProducer::gethead()->AddEvent();
#endif
#pragma omp critical (g1)
      AMSEvent::gethead()->Recovery();
      if(MISCFFKEY.NumThreads>2 || MISCFFKEY.NumThreads<0)MISCFFKEY.NumThreads=2;
      else MISCFFKEY.NumThreads=1;
      cerr<<"  AMSaPool-W-MemoryRecovered "<<AMSEvent::TotalSize()<<" Thread "<<AMSEvent::get_thread_num()<<" will be idled and number of thread will be reduced to "<<MISCFFKEY.NumThreads<<endl;
      AMSEvent::ThreadWait()=1;
      AMSEvent::ThreadSize()=UPool.size();
      return;
    }
    catch (AMSTrTrackError e){
      cerr << e.getmessage()<<endl;
      AMSEvent::gethead()->_printEl(cerr);
      AMSEvent::gethead()->seterror(2);
      /*
	#ifdef __CORBA__
	#pragma omp critical (g1)
	AMSProducer::gethead()->AddEvent();
	#endif
	UPool.Release(0);
	#pragma omp critical (g2)
	AMSEvent::gethead()->remove();
	UPool.Release(7);
	delete AMSEvent::gethead();
	AMSEvent::sethead(0);
	UPool.erase(512000);
	return;
      */
    }
    catch (amsglobalerror e){
      AMSgObj::BookTimer.stop("GUOUT");
      cerr << e.getmessage()<<endl;
      cerr <<"Event dump follows"<<endl;
      AMSEvent::gethead()->_printEl(cerr);
      AMSEvent::gethead()->seterror(e.getlevel());
      if(e.getlevel()>2)throw e; 
     
      /*
	#ifdef __CORBA__
	#pragma omp critical (g1)
	AMSProducer::gethead()->AddEvent();
	#endif
	UPool.Release(0);
	#pragma omp critical (g2)
	AMSEvent::gethead()->remove();
	UPool.Release(1);
	delete AMSEvent::gethead();
	AMSEvent::sethead(0);
	UPool.erase(512000);
	return;
      */
    }

#ifdef __CORBA__
#pragma omp critical (g1)
    AMSProducer::gethead()->AddEvent();
#endif
#pragma omp critical (g3)
    if(GCFLAG.IEVENT%abs(GCFLAG.ITEST)==0)
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
    //   if(trig ){ 
    //     AMSEvent::gethead()->copy();
    //   }
    AMSEvent::gethead()->write(trig);
  }
#pragma omp critical (g2)
  AMSEvent::ThreadSize()=UPool.size();
  UPool.Release(0);
  AMSEvent::gethead()->remove();
  delete AMSEvent::gethead();
  UPool.Release(1);
  AMSEvent::sethead(0);
  UPool.erase(2000000);

  if( AMSEvent::Waitable() && AMSEvent::TotalSize()>AMSEvent::MaxMem()){
    cerr<<"  AMSaPool-W-MemoryTooBig "<<AMSEvent::TotalSize()<<" Thread "<<AMSEvent::get_thread_num()<<" will be idled "<<UPool.size()<<endl;
    AMSEvent::ThreadWait()=1;
    AMSEvent::ThreadSize()=UPool.size();
  }
  AMSgObj::BookTimer.stop("GUOUT");

  // allow termination via time via datacard
  {  
    float xx,yy;
    TIMEX(xx);   
    TIMEL(yy);   
    if(GCTIME.TIMEND < xx || (yy>0 && yy<AMSFFKEY.CpuLimit) ){
      GCTIME.ITIME=1;
    }
  }




  if(vmc_version==1)GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
  else {
    GCFLAG.NRNDM[0]=CLHEP::HepRandom::getTheSeeds()[0];
    GCFLAG.NRNDM[1]=CLHEP::HepRandom::getTheSeeds()[1];
  }
  cout <<"           **** RANDOM NUMBER GENERATOR AFTER LAST COMPLETE EVENT "<<GCFLAG.NRNDM[0]<<" "<<GCFLAG.NRNDM[1]<<endl;

  if(vmc_version==2)GCFLAG.IEVENT++;


  //    GCFLAG.IEOTRI=0;
  //    GCFLAG.IEORUN=0;
  if(GCFLAG.IEVENT>=vmc_nevent){

   gMC->StopRun();
  //    GCFLAG.IEOTRI=1;
  //    GCFLAG.IEORUN=1;
  }
  //  if(GCFLAG.IEOTRI || GCFLAG.IEORUN) this->FinishRun();

  //cout<<"DEBUG: in FinishEvent(), check T"<<endl;
  //  cout <<"Track number in stacks:"<<gMC->GetStack()->GetNtrack()<<endl;
  //  cout <<"Optical photon in this event:"<<fOptPhotonNo<<endl;
  //  cout <<"Energy depostion in TRDT in this event:"<<fECALedep<<" GeV"<<endl;
  fStack->Reset();
  //	 cout<<"DEBUG: in FinishEvent(), check U"<<endl;
} 


