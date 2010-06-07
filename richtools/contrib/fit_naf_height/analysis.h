#ifndef __TEST__
#define __TEST__

#include "TString.h"
#include "amschain.h"
#include "richrec.h"
#include "richid.h"
#include "richradid.h"

const int max_cuts=100;

class Analysis{
 public:
  AMSChain *chain;

  Analysis(TString file);
  ~Analysis(){delete chain;}


  TH2F beta_hit_direct;     // The beta hit direct vs height shift
  TH2F beta_hit_reflected;  // The beta hit direct vs height shift

  // Arrays to compute cut efficiencies
  char cut_name[max_cuts][1000];          
  int selected[max_cuts];
  int last_cut;

  bool Select(AMSEventR *event);
  void Loop();
};

#endif
