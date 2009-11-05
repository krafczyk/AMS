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
  TH1F h_beta;


  Alignment(TString files);
  ~Alignment();
  
  void Loop(double &_x,double &_y,double &_z,
	    double &_alpha,double &_beta,double &_gamma);
  int Select(AMSEventR *event);
  int SelectRing(RichRing *ring);
  double Fit(TH1F &beta);


};

#endif
