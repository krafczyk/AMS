#ifndef __TEST__
#define __TEST__

#include "TString.h"
#include "amschain.h"
#include "richrec.h"
#include "richid.h"
#include "richradid.h"
#include "TH3F.h"

const int max_cuts=100;

class Analysis{
 public:
  AMSChain *chain;

  Analysis(TString file);
  ~Analysis(){delete chain;}

  TH1F beta_tile[121]; 
  TH1F beta_tile_ref[121]; 
  TH1F final_beta[121];
  TH1F final_wbeta[121];
  TH1F summary;
  TH1F summary_used;
  TH1F summary_back;
  TH1F summary_ref;
  TH1F summary_ref_used;
  TH1F summary_ref_back;
    
  TH2F beta;
  TH2F wbeta;

  TH1F beta_win_ref[3][2];
  TH1F beta_win_ref_used[3][2];


  TH2F HotSpotShift;
  TH2F HotSpotLG;
  TH2F ShiftVsTheta;
  TH3F Shift;

  // Arrays to compute cut efficiencies
  char cut_name[max_cuts][1000];          
  int selected[max_cuts];
  int last_cut;

  bool Select(AMSEventR *event);
  void Loop();
  bool HotSpotPos(AMSEventR *event,double &x,double &y,double &lx,double &ly);


};

#endif
