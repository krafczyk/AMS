/////////////////////// ams Virtual MonteCarlo Application,  Zhili.Weng  2009/8/17
#include "geantnamespace.h"
#include "amsvmc_MCApplication.h"
#include "commons.h"
#include <TROOT.h>
#include <TSystem.h>
#include <iostream>
#include <TString.h>
#include "TChain.h"
#include "TFile.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include <fenv.h>
void amsvmc::VMCINIT(amsvmc_MCApplication* _appl){
  _appl->SetPrintModulo(1);
  _appl->PreInit();
  //  gRandom->SetSeed(34985);
}





void amsvmc::VMCRUN(amsvmc_MCApplication* _appl){
  //  _appl->PreInit();
 _appl->InitMC();
 //  int eventno=10;
 //  cout<<"~~~~~~~~Please Input the desired event number:~~~~~~~~"<<endl;
 //  cin>>eventno;
 feclearexcept(FE_ALL_EXCEPT);
 fedisableexcept(FE_ALL_EXCEPT);
  _appl->RunMC(IOPA.eventno);
  //    delete _appl;
}

void amsvmc::VMCLAST(){
  cout<<"DEBUG: amsvmc::VMCLAST()"<<endl;
}

