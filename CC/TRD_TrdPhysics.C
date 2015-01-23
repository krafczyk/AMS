#include "TRD_TrdPhysics.hh"
//#include "TRD_DetecorConstruction.hh"
//#include "G4RunManager.hh"
#include "TRD_GammaXTRadiator.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4RegionStore.hh"
#include <sstream>
#include "TRD_SimUtil.h"

TRD_TrdPhysics::TRD_TrdPhysics(const G4String& name ) : G4VPhysicsConstructor(name) {
  fAlphaFiber = 5.;//100.;
  fAlphaGas   = 0.66;//100.;
}

TRD_TrdPhysics::~TRD_TrdPhysics(){ }

void TRD_TrdPhysics::ConstructParticle()
{
  G4Gamma::Gamma();
  G4Electron::Electron();
  G4Positron::Positron();

  return;
}

void TRD_TrdPhysics::ConstructProcess()
{

  G4ProcessManager * pManager = 0;

  G4cout<<"In TRD_trdPhysics::ConstructProcess()"<<G4endl;
  std::stringstream name;
  name << "GammaXTRadiator" ;

  G4cout<<"In TRD_trdPhysics::ConstructProcess(), check A"<<G4endl;
  G4cout<<"alpha fiber "<<trdSimUtil.GetAlphaFiber()
	<<" alpha gas "<<trdSimUtil.GetAlphaGas()
	<<" fleece material "<<trdSimUtil.GetG4FleeceMaterial()->GetName()
	<<" gas material "<< trdSimUtil.GetG4FleeceGasMaterial()->GetName()
	<<" foil thickness "<< trdSimUtil.GetTrdFoilThickness()
	<<" gas thickness "<<trdSimUtil.GetTrdGasThickness()
	<< " nfoils "<<trdSimUtil.GetTrdFoilNumber()<<G4endl;
  
  G4cout<<"In TRD_trdPhysics::ConstructProcess(), check B"<<G4endl;

  TRD_VXTenergyLoss *processXTR = new TRD_GammaXTRadiator( "TrdArtRadGmat",
							   trdSimUtil.GetAlphaFiber(),
							   trdSimUtil.GetAlphaGas(),
							   trdSimUtil.GetG4FleeceMaterial(),
							   trdSimUtil.GetG4FleeceGasMaterial(),
							   
							   trdSimUtil.GetTrdFoilThickness(),
							   trdSimUtil.GetTrdGasThickness(),
							   trdSimUtil.GetTrdFoilNumber(),

							   //							   pDet->GetAMS02TrdFleeceMaterial(),
							   //							   pDet->GetAMS02TrdGasInRadiatorMaterial(),
							   //							   pDet->GetAMS02TrdFoilThickness(),
							   //							   pDet->GetAMS02TrdGasThickness(),
							   //							   pDet->GetAMS02TrdFoilNumber(),
							   //							   65,
							   //							   10,
							   //							   12.0*um,
							   //							   0.2,
							   //							   5,
							   name.str().c_str() );
    G4cout<<"In TRD_trdPhysics::ConstructProcess(), check F"<<G4endl;
  
  
  if( !processXTR ){
    printf("not xtr process\n");
    //      continue;
  }
  else{
    //    processXTR->SetVerboseLevel(2);
    //       processXTR->G4VProcess::SetVerboseLevel(2);
    
    pManager = G4PionPlus::PionPlus()->GetProcessManager();
    pManager->AddDiscreteProcess(processXTR);
    
    pManager = G4PionMinus::PionMinus()->GetProcessManager();
    pManager->AddDiscreteProcess(processXTR);
    
    pManager = G4Electron::Electron()->GetProcessManager();
    pManager->AddDiscreteProcess(processXTR);
    
    pManager = G4Positron::Positron()->GetProcessManager();
    pManager->AddDiscreteProcess(processXTR);
    
    pManager = G4Proton::Proton()->GetProcessManager();
    pManager->AddDiscreteProcess(processXTR);
  }


  return;
}

void TRD_TrdPhysics::SetPhysicsTableDirectories( const G4String dir ) {

  for( unsigned int proc = 0; proc < theXTRprocesses.size() ; ++proc ){

    TRD_VXTenergyLoss* processXTR = theXTRprocesses.at(proc);

    if( !processXTR ) continue;

    processXTR->SetPhysicsTableDirectory( dir );

  }
  return;
}
