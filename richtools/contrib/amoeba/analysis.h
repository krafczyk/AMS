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


const int max_cuts=100;

class Analysis{//: public TObject{
 public:
  AMSChain *chain;                // Files to read 
  Analysis(TString file);         // Setup the chain
  ~Analysis(){delete chain;}      // destroy everything 
    
  void Init();
  bool Select(AMSEventR *event);  // The selection function
  void Loop();                    // The looping function


  vector<double>   alignmentParameters[6];   // The generated alignment parameters (the first one is the loaded one)
  vector<TH1F>     betaHitHistograms;        // Betahit histograms;

  // Arrays to compute cut efficiencies
  char cut_name[max_cuts][1000];          
  int selected[max_cuts];
  int last_cut;
};

#endif
