//   \example trd_likelihood.C
///  To use TrdHCharge in analysis you need to call 2-3 commands in the EVENT LOOP:
///   - TrdHChargeR::gethead()->Initialize(AMSEventR* ev, char *databasedir="/afs/ams.cern.ch/Offline.cern/AMSDataDir")
///     mainly to initialize the interface to TDV at directory <databasedir> and read PDF definitions
///   - int charge=TrdHChargeR::gethead()->GetCharge(TrdHTrackR *tr,float rig=0.,int debug=0)
///     reconstruct the most probable charge of TRD event - in case tracker rigidity is passed a betagamma correction and according PDFs are applied
///   - optional: float loglik=TrdHChargeR::gethead()->GetLikelihood(int sig=0,int bkg=1,int debug=0);
///     return the likelihood ratios of signal (0:e 1:p 2:He) to background (bitwise controlled 1:p 2:He 3:p&He etc.)

#ifndef _PGTRACK_
#define _PGTRACK_
#endif
#include "TrTrackSelection.h"

#include "root.h"
#include "amschain.h"
#include "TFile.h"
#include "TH1.h"
#include <cstring>
#include <TVector3.h>
#include <TrdHRecon.h>
#include <TrdHCalib.h>
#include <TrdHCharge.h>
#include <TrdSCalib.h>
#include "HistoMan.h"
#include <map>
#include <bitset>

using namespace std;
class trd_likelihood : public AMSEventR {
public :
#ifdef __CINT__
#include <process_cint.h>
#endif

  trd_likelihood(){};
  ~trd_likelihood(){};

  void    UBegin();
  bool    UProcessCut();
  void    UProcessFill();
  void    UTerminate();

  /// get likelihood from trdscalib
  int     lik_trdscalib(int n, float &loglik,int Debug=0);
  
  AMSEventList* badrun_list;
  AMSEventList* select_list;
  unsigned int eventno;
  TFile *fout;
  TTree *tout;
  
  float tree_rigidity;
  int tree_run;
  float tree_trdhcalib_sa_loglik;
  float tree_trdhcalib_rig_loglik;

};

void trd_likelihood::UBegin(){
  fout = new TFile ("trd_likelihood.root", "RECREATE"); 

  select_list = new AMSEventList;
  badrun_list = new AMSEventList("my_badruns.list");
  eventno=0;
  
  tree_run=0;
  tree_rigidity=0.;
  tree_trdhcalib_sa_loglik=0.;
  tree_trdhcalib_rig_loglik=0.;

  tout=new TTree("tree","tree");
  tout->Branch("run",&tree_run,"run/I");
  tout->Branch("rigidity",&tree_rigidity,"rigidity/F");
  tout->Branch("lik_trdhcalib_sa",&tree_trdhcalib_sa_loglik,"lik_trdhcalib_sa/F");
  tout->Branch("lik_trdhcalib_rig",&tree_trdhcalib_rig_loglik,"lik_trdhcalib_rig/F");
  //  tout->Branch("lik_trdscalib",&tree_trdscalib_loglik,"lik_trdscalib/F");

}

// UProcessCut() is not mandatory
bool trd_likelihood::UProcessCut(){
  eventno++;
  if (!(eventno%2000)) {
    std::printf("Processed Events: %5i\r",(int)eventno);
    std::cout << std::flush;
  }

  TrdHCalibR::gethead()->Initialize(this);
  TrdHChargeR::gethead()->Initialize(this);//,"/afs/ams.cern.ch/user/trdcalib/newGain/space");
  
  // PUT ANY EVENT SELECTION BELOW
  if(nParticle()!=1||!pParticle(0)->pTrdHTrack()||!pParticle(0)->pTrTrack())return false;

  return true;
}

void trd_likelihood::UProcessFill() {
  float rig=pParticle(0)->pTrTrack()->GetRigidity();

  TrdHTrackR htmp=pParticle(0)->pTrdHTrack();
  TrdHTrackR* trdhtrack=&htmp;
  
  // rigidity dependent
  int charge_rig=TrdHChargeR::gethead()->GetCharge(trdhtrack,rig);
  float lik_rig=trdhtrack->GetLikelihood();
  
  // standalone 
  int charge_sa=TrdHChargeR::gethead()->GetCharge(trdhtrack);
  float lik_sa=trdhtrack->GetLikelihood();
 
  tree_run=Run();
  tree_rigidity=rig;
  tree_trdhcalib_sa_loglik=lik_sa;
  tree_trdhcalib_rig_loglik=lik_rig;

  tout->Fill();
};

void trd_likelihood::UTerminate()
{
  fout->Write();
  fout->Close();
}


