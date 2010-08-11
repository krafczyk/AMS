#ifndef __TEST__
#define __TEST__

#include "TString.h"
#include "amschain.h"
#include "richrec.h"
#include "richid.h"
#include "richradid.h"
#include "TH3F.h"

class Analysis{
 public:
  AMSChain *chain;

  Analysis(TString file);
  ~Analysis(){delete chain;}
    
  TH2F betaVsWindow[20];
  TH1F means[20];
  TH1F sigmas[20];
  TH1F betahit;

  bool Select(AMSEventR *event);
  void Loop();
};

#endif
