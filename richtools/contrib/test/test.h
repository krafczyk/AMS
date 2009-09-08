#ifndef __TEST__
#define __TEST__

#include "TString.h"
#include "amschain.h"
#include "richrec.h"
#include "richid.h"
#include "richradid.h"

class Analysis{
 public:
  AMSChain *chain;

  Analysis(TString file);
  ~Analysis(){delete chain;}
    
  bool Select(AMSEventR *event);
  void Loop();
};

#endif
