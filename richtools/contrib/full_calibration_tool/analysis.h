#ifndef __BETAHIT__
#define __BETAHIT__

#include "TString.h"
#include "amschain.h"
#include "richrec.h"
#include "richid.h"
#include "richradid.h"

const int max_cuts=100;
const int max_tiles=150;

class Analysis{//: public TObject{
 public:
  AMSChain *chain;                // Files to read 
  Analysis(TString file);         // Setup the chain
  ~Analysis(){delete chain;}      // destroy everything 
    
  void Init();
  bool Select(AMSEventR *event);  // The selection function
  void Loop();                    // The looping function

  // Flags
  bool isMC;


  // Histograms
  TH1F betahit_direct_vs_tile[max_tiles];

  // Mirror alignment
  TH2F betahit_reflected_vs_dx;
  TH2F betahit_reflected_vs_dy;
  TH2F betahit_reflected_vs_dz;

  // Rich alignment
  TH2F betahit_direct_vs_dx;
  TH2F betahit_direct_vs_dy;
  TH2F betahit_direct_vs_dz;

  // Arrays to compute cut efficiencies
  char cut_name[max_cuts][1000];          
  int selected[max_cuts];
  int last_cut;
  //  ClassDef(Analysis,1)
};

#endif
