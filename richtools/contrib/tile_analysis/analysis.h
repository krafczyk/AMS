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
  bool isMC;

  Analysis(TString file);
  ~Analysis(){delete chain;}


  TH1F beta_tile[121]; 
  TH1F beta_used[121]; 
  TH1F final_beta[121];
  TH2F final_beta_vs_used[121];
  TH2F beta_vs_beta_used[121];

  // Arrays to compute cut efficiencies
  char cut_name[max_cuts][1000];          
  int selected[max_cuts];
  int last_cut;

  bool Select(AMSEventR *event);
  void Loop();
};

#endif
