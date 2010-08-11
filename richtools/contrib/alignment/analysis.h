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

const int max_tiles=1;
const int max_cuts=100;

const int nSamples=500/10;
//const int nSamples=200;
const long int maxReconstructions=230000/10/2;


class Analysis{//: public TObject{
 public:
  AMSChain *chain;                // Files to read 
  Analysis(TString file);         // Setup the chain
  ~Analysis(){delete chain;}      // destroy everything 
    
  void Init();
  bool Select(AMSEventR *event);  // The selection function
  void Loop();                    // The looping function

  vector<double> alignmentParameters[6];
  //  vector<TH1F> betaHitHistograms[max_tiles];
  //  TH1F betaHitHistograms[max_tiles][nSamples];
  TH1F *betaHitHistograms[max_tiles];

  long int entries[max_tiles];
  long int total_entries;

  vector<TH1F> betaHitSummary;
  TH1F testigo;

  // Arrays to compute cut efficiencies
  char cut_name[max_cuts][1000];          
  int selected[max_cuts];
  int last_cut;
};

#endif
