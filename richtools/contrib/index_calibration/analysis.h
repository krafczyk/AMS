#ifndef __BETAHIT__
#define __BETAHIT__

#include "TString.h"
#include "amschain.h"
#include "richrec.h"
#include "richid.h"
#include "richradid.h"
#include "TH3F.h"
#include "TRandom.h"
#include <vector>                

const int max_tiles=121;
const int max_cuts=100;
const long int maxReconstructions=2300000;
const int trials=3;
//const double indexes[3]={1.048,1.05,1.052};
double indexes[max_tiles][3];


class Analysis{//: public TObject{
 public:
  AMSChain *chain;                // Files to read 
  Analysis(TString file);         // Setup the chain
  ~Analysis(){delete chain;}      // destroy everything 
    
  void Init();
  bool Select(AMSEventR *event);  // The selection function
  void Loop();                    // The looping function

  double alignmentParameters[6];
  TH1F betaHitHistograms[max_tiles][3];
  TH1F summary;

  long int entries[max_tiles];
  long int total_entries;

  // Arrays to compute cut efficiencies
  char cut_name[max_cuts][1000];          
  int selected[max_cuts];
  int last_cut;
};

#endif
