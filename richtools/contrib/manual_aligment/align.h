#ifndef __ALIGN__
#define __ALIGN__

#include "amschain.h"
#include "richid.h"
#include "richradid.h"
#include "richrec.h"

#include "TString.h"
#include "TH1F.h"
#include "TF1.h"


class Alignment{
public:
  static Alignment *THIS;

  AMSEventList *e_list;  // Event list to increase the speed 
  AMSChain *disk_chain;
  AMSChain *chain;
  TH1F mean_h[6];
  TH1F sigma_h[6];
  TH1F norma_h[6];

  Alignment(TString files);
  ~Alignment();
  
  void Loop();
  int Select(AMSEventR *event);
  int SelectRing(RichRing *ring);
  double Fit(TH1F &beta);
};

#endif
