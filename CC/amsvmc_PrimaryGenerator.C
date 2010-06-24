// AMS02 Virtual MonteCarlo Application,  Zhili.Weng  2009/8/17

// Class for Primary particle generation, use mceventg
// Some include file needs clean up
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

#include <TVirtualMC.h>
#include <TVirtualMCStack.h>
#include <TVirtualMCApplication.h>
#include <TRandom.h>
#include <TPDGCode.h>
#include <TVector3.h>
#include <TParticlePDG.h>
#include <TDatabasePDG.h>
#include <iostream>

#include "amsvmc_PrimaryGenerator.h"

/// \cond CLASSIMP
ClassImp(amsvmc_PrimaryGenerator)
/// \endcond

//_____________________________________________________________________________
amsvmc_PrimaryGenerator::amsvmc_PrimaryGenerator(TVirtualMCStack* stack) 
  : TObject(),
    fStack(stack),
    fIsRandom(false),
    fUserParticles(false),
    fUserDecay(false),
    fNofPrimaries(1)
    
{
  cout<<"DEBUG: in amsvmc_PrimaryGenerator::amsvmc_PrimaryGenerator(TVirtualMCStack* stack), fNofPrimaries:"<<fNofPrimaries<<endl;
/// Standard constructor
/// \param stack  The VMC stack
}

//_____________________________________________________________________________
amsvmc_PrimaryGenerator::amsvmc_PrimaryGenerator()
  : TObject(),
    fStack(0),
    fIsRandom(false),
    fUserParticles(false),
    fUserDecay(false),
    fNofPrimaries(0)
{    

  cout<<"DEBUG: in amsvmc_PrimaryGenerator::amsvmc_PrimaryGenerator()"<<endl;

/// Default constructor
}

//_____________________________________________________________________________
amsvmc_PrimaryGenerator::~amsvmc_PrimaryGenerator() 
{
/// Destructor  
}

//
// private methods
//

//_____________________________________________________________________________
void amsvmc_PrimaryGenerator::GeneratePrimary1(int vmc_version)
{    

 if(AMSJob::gethead()->isSimulation()){

   //   cout<<"DEBUG:  AMSJob::gethead()->isSimulation()"<<endl;
   
   int a[2];


    AMSgObj::BookTimer.start("GEANTTRACKING");
	   if(IOPA.mode%10 !=1 ){
	    AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
    new AMSEvent(AMSID("Event",GCFLAG.IEVENT),CCFFKEY.run,0,0,0)));
    for(integer i=0;i<CCFFKEY.npat;i++){
      //       a[0]=10000000*gRandom->Rndm();
      //       a[1]=10000000*gRandom->Rndm();
      //       cout<<"In GeneratePrimary1, Random Seed, before:"<<a[0]<<","<<a[1]<<endl;
      //       cout<<"In GeneratePrimary1, Random Seed, before:"<<GCFLAG.NRNDM[0]<<","<<GCFLAG.NRNDM[1]<<endl;
      //       GRNDMQ(a[0],a[1],1,"S");
      //       cout<<"In GeneratePrimary1, Random Seed After :"<<a[0]<<","<<a[1]<<endl;
      //       cout<<"In GeneratePrimary1, Random Seed After:"<<GCFLAG.NRNDM[0]<<","<<GCFLAG.NRNDM[1]<<endl;

      //     AMSmceventg* genp=new AMSmceventg(a);
      

//       GCFLAG.NRNDM[0]=4324545;
//       GCFLAG.NRNDM[1]=8985;
//       cout<<"In GeneratePrimary1, Random Seed Before:"<<GCFLAG.NRNDM[0]<<","<<GCFLAG.NRNDM[1]<<endl;
//       GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"S");
//       GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],1,"S");
     
//       cout<<"In GeneratePrimary1, Random Seed After:"<<GCFLAG.NRNDM[0]<<","<<GCFLAG.NRNDM[1]<<endl;

//       GCFLAG.NRNDM[0]=76745;
//       GCFLAG.NRNDM[1]=55778;
       cout<<"In GeneratePrimary1, Random Seed After2:"<<GCFLAG.NRNDM[0]<<","<<GCFLAG.NRNDM[1]<<endl;

     AMSmceventg* genp=new AMSmceventg(GCFLAG.NRNDM);
    if(genp){
     AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
     //          cout<<"DEBUG:  about to call  genp->run(GCKINE.ikine);"<<endl;
     genp->runvmc(GCKINE.ikine,fStack,vmc_version);
     if (CCFFKEY.npat>1 && TRDMCFFKEY.multiples==1){
	 if (i>0){
	   cout<<"DEBUG:  about to call  genp->gendelay();"<<endl;
	     genp->gendelay();
	   }
       }
     //genp->_printEl(cout);

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
      genp->runvmc(fStack,vmc_version);
      //genp->_printEl(cout);
     }
    }
    else{
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
     AMSgObj::BookTimer.stop("GUKINE");
     return;
    }   
   }




 }

}

//_____________________________________________________________________________

//
// public methods
//

//_____________________________________________________________________________
void amsvmc_PrimaryGenerator::GeneratePrimaries(int vmc_version)
{    
/// Fill the user stack (derived from TVirtualMCStack) with primary particles.

//  std::cout<<"DEBUG: in GeneratePrimaries, fNofPrimaries= "<<fNofPrimaries<<std::endl;

  //  fNofPrimaries=1;
  

  for (Int_t i=0; i<fNofPrimaries; i++) GeneratePrimary1(vmc_version);
}

//_____________________________________________________________________________
void  amsvmc_PrimaryGenerator::SetUserParticles(Bool_t userParticles)
{ 
/// Switch on/off the user particles
/// \param userParticles  If true, the user defined particles are shooted

  fUserParticles = userParticles; 
  fUserDecay = ! userParticles;
}

//_____________________________________________________________________________
void  amsvmc_PrimaryGenerator::SetUserDecay(Bool_t userDecay)
{ 
/// Switch on/off the particle with a user decay
/// \param userDecay  If true, the particles with user defined decay are shooted

  fUserDecay = userDecay; 
  fUserParticles = ! userDecay; 
  
}

