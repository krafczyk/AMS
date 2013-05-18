#include "MVAtest.h"
#include "ConfigParser.h"
#include "MVAmanager.h"
#include "OutTreeManager.h"
#include "OutHistoManager.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <time.h>
#include <bitset>
#include <sys/resource.h>
#include <omp.h>
#include <dirent.h>


#include <root.h>
#include <amschain.h>
#include "TrdSCalib.h"
#include "TrdHCalib.h"
#include "TrTrackSelection.h"
#include "TofTrack.h"
#include "Tofrec02_ihep.h"
#include "TrdHRecon.h"
#include "TrdHCalib.h"
#include "TrdHCharge.h"
#include "TrExtAlignDB.h"
#include "TkDBc.h"
#include "DynAlignment.h"
#include "TrdKCluster.h"
#include "TrRecon.h"
#include "point.h"


#include "TrdQt.hh"
#include "TrkLH.h"

#include "TTree.h"
#include "TFrame.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TBenchmark.h"
#include "TRandom.h"
#include "TSystem.h"

using namespace std;

MVAtest::MVAtest(ConfigParser* aconf) {
fConf=aconf;
fMVA =0;
fTreeMan=0;
fHistoMan=0;
Debug=fConf->Value("general","Debug");
bmc=fConf->Value("general","bmc");
bool bmva=fConf->Value("general","bmva")>0;
bool bmvaana=fConf->Value("general","bmvaana")>0;
bool bmvatrain=fConf->Value("general","bmvatrain")>0;
bool bhisto=fConf->Value("general","bstoreouthisto")>0;
 if(bmva) {  
fMVA =new MVAmanager(aconf); //usemva
if(bmvaana) {
  fMVA ->setmethods();
  fMVA ->initreadvariables(); //also fill variables
   }
 } // bmva

 // create tree and histos
 if(!bmvatrain) fTreeMan=booktree();
 if(bhisto) fHistoMan=bookhisto();
 if(Debug) cout<<"= construct MVAtest"<<endl;
  return;
}
MVAtest::~MVAtest() {
  if(Debug) cout<<"= destruct MVAtest"<<endl;
  if(fTreeMan) delete fTreeMan;
  if(fMVA) delete fMVA; 
  if(fHistoMan) delete fHistoMan;
}
// book some help histogramms
OutHistoManager* MVAtest::bookhisto(){
OutHistoManager* histo=new OutHistoManager();
{
string name="hR_a";
TH1D* h=new TH1D(name.c_str(),name.c_str(),100,0.,1000.);
h->Sumw2();histo->AddH1(h);
}
bool bmva=fConf->Value("general","bmva")>0;
if(bmva) {
{
string name="hMVA_a";
TH1D* h=new TH1D(name.c_str(),name.c_str(),100,-1.,1);
h->Sumw2(); histo->AddH1(h);
}
{
string name="hMVA_s";
 TH1D* h=new TH1D(name.c_str(),name.c_str(),100,-1.,1);
h->Sumw2(); histo->AddH1(h);
}
{
string name="hMVA_b";
 TH1D* h=new TH1D(name.c_str(),name.c_str(),100,-1.,1);
h->Sumw2(); histo->AddH1(h);
}
}//bmva

return histo;
}

// book simple out  tree filled from AMSchain, doubles only
OutTreeManager* MVAtest::booktree() {
  if(Debug) cout<<">>booktree "<<endl;
 OutTreeManager* atree=new OutTreeManager();
 if(!atree) return 0;
 atree->Debug=Debug;

atree->AddPar("time");
atree->AddPar("trkQ");
atree->AddPar("tofQe");
atree->AddPar("trkQext");
atree->AddPar("tofQ");
atree->AddPar("tofQ1");
atree->AddPar("tofQ2");
atree->AddPar("tofQ3");
atree->AddPar("tofQ4");
atree->AddPar("nacccl");
atree->AddPar("ntofcl");
 if(bmc) {
atree->AddPar("mcfragm");
 }
//....

 bool binitouttree=fConf->Value("general","binitouttree")>0;
 bool bloadouttree=fConf->Value("general","bloadouttree")>0;

  // new tree
  if(binitouttree)  {
string fname=fConf->Value("general","outfile");
 if(!atree->InitTree(fname,"AMSflux")) {
cout<<"** failed to init outtree "<<endl;
 return 0; 
      }
  }
  // load existing
if(bloadouttree) {
string filelist=fConf->Value("general","filelist");
 if(Debug) cout<<"  load filelist "<<filelist<<endl;
 if(!atree->LoadTreeList(filelist,"AMSflux")) {
   cout<<"** failed to load outtree from list  "<<filelist<<endl;
 return 0; 
      }
   }
 return atree;
}

// populate OutTree branches from AMSEvent 
int MVAtest::filloutevent(AMSEventR* pev, int ntotev) {
  if(Debug) cout<<"==filloutevent "<<ntotev<<" "<<pev<<" "<<fTreeMan<<endl;
  if(!fTreeMan) return 1; 
  if(!pev) return 2;
  int ipart=0;
  fTreeMan->FillTreePar("time",pev->UTime());// always there for possible lookup table
ParticleR*  ppart=0;
 if(pev->NParticle()!=0)
 ppart  = pev->pParticle(ipart);
 if(!ppart) return 3;
TrTrackR* ptrk = ppart->pTrTrack();
if(!ptrk) return 4;
if(!pev->nBetaH()) TofRecH::ReBuild();
BetaHR* ptofbetah = ppart->pBetaH();
if(!ptofbetah) return 5;
double betac = ptofbetah->GetBetaC();
 if(bmc)
   if(pev->NMCEventg()<1) return 6; // no mc record
 //


//
 if(Debug) cout<<"- select "<<ntotev<<endl;
 fTreeMan->FillTreePar("nacccl",pev->nAntiCluster ());
 fTreeMan->FillTreePar("ntofcl",pev->nTofClusterH ());
 double tofq[4]={-1.};
 for (unsigned int ilay=0; ilay<4; ilay++){
      if(  ptofbetah->TestExistHL(ilay) ) {
	tofq[ilay]=ptofbetah->GetQL(ilay);
      }
    }
 int nl;
 float qrms;
 // tofQ by layers
 double tofaq=ptofbetah->GetQ(nl,qrms);
fTreeMan->FillTreePar("tofQ",tofaq); 
fTreeMan->FillTreePar("tofQe",qrms); 
fTreeMan->FillTreePar("tofQ1",tofq[0]);
fTreeMan->FillTreePar("tofQ2",tofq[1]);
fTreeMan->FillTreePar("tofQ3",tofq[2]);
fTreeMan->FillTreePar("tofQ4",tofq[3]);
fTreeMan->FillTreePar("trkQ",ptrk->GetInnerQ(betac));
fTreeMan->FillTreePar("trkQext", ptrk->GetQ(betac));
 if(bmc){
 int mcfragm=0;
for(int imc=1; imc<pev->NMCEventg(); imc++) {
 MCEventgR* pMCEventg = pev->pMCEventg(imc);
 if (!pMCEventg) continue;
 if ((pMCEventg->Particle!=pev->pMCEventg(0)->Particle)&&(pMCEventg->Momentum>1.0)) mcfragm++;
}
fTreeMan->FillTreePar("mcfragm",mcfragm);
 }
//....
  return 0;
}

//train mva, all related parameters are defined in config
int MVAtest::trainoutree() {
  if(Debug) cout<<"==MVA training start "<<fMVA <<endl;
  if(!fMVA ) return 1;
   fMVA ->fillvariables();
   fMVA ->setmethods();
   fMVA ->initfactory();
   fMVA ->train();
 if(Debug) cout<<"==MVA training done "<<endl;
   return 0;
}

double MVAtest::getmva(AMSEventR* pev, int ntot) {
if(Debug) cout<<"==getmva from OutTreeManager "<<fMVA<<endl;
 if(!filloutevent(pev,ntot)) return -99; // store intermidiately to OutTree
return getmva();
}

// the parameters are extracted from OutTreeBranches
// here all parameters which can be potentially used in MVA, 
// should be defined by names used in config
double MVAtest::getmva() {
if(Debug) cout<<"==getmva from OutTreeManager "<<fMVA <<endl;
 if(!fMVA ) return -99;
       //fill parameters
     vector<AParam*> vvar=fMVA ->getparams();
   for(vector<AParam*>::iterator ip = vvar.begin(); ip != vvar.end(); ++ip) {
     if(Debug>2) cout<<" fillvar "<<(*ip)->name<<endl;
  
    if((*ip)->name=="nacccl")  (*ip)->val=fTreeMan->GetTreePar("nacccl");
    if((*ip)->name=="ntofcl")  (*ip)->val=fTreeMan->GetTreePar("ntofcl");

  if((*ip)->name=="tofQ/trkQ") 
    {if(fTreeMan->GetTreePar("trkQ")>0)  (*ip)->val=fTreeMan->GetTreePar("tofQ")/fTreeMan->GetTreePar("trkQ");}
 if((*ip)->name=="trkQext/trkQ") 
    {if(fTreeMan->GetTreePar("trkQ")>0)  (*ip)->val=fTreeMan->GetTreePar("trkQext")/fTreeMan->GetTreePar("trkQ");}
 if((*ip)->name=="tofQ1/tofQ") 
    {if(fTreeMan->GetTreePar("tofQ")>0)  (*ip)->val=fTreeMan->GetTreePar("tofQ1")/fTreeMan->GetTreePar("tofQ");}
 if((*ip)->name=="tofQ2/tofQ") 
    {if(fTreeMan->GetTreePar("tofQ")>0)  (*ip)->val=fTreeMan->GetTreePar("tofQ2")/fTreeMan->GetTreePar("tofQ");}
 if((*ip)->name=="tofQ3/tofQ") 
    {if(fTreeMan->GetTreePar("tofQ")>0)  (*ip)->val=fTreeMan->GetTreePar("tofQ3")/fTreeMan->GetTreePar("tofQ");}
 if((*ip)->name=="tofQ4/tofQ") 
    {if(fTreeMan->GetTreePar("tofQ")>0)  (*ip)->val=fTreeMan->GetTreePar("tofQ4")/fTreeMan->GetTreePar("tofQ");}

//// add more dfinitions
//.....

} //ip

double mvaout=fMVA->getclassifier("BDTG"); // here specify your classfier, can use many...
 if(Debug) cout<<" mvaout="<<mvaout<<endl;
 return mvaout;
}






