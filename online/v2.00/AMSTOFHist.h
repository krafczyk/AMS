#ifndef __AMSTOFHIST__
#define __AMSTOFHIST__
#include "AMSHist.h"

class AMSTOFHist : public AMSHist{
public:
  ClassDef(AMSTOFHist,0)    // AMSTOFHist class
  AMSTOFHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active);
  void ShowSet(Int_t cset);
  void _Fill();
  void Fill(AMSNtuple *ntuple);
  void _Fetch();
  static void ShowChan(Int_t ilay,Int_t ibar,Int_t isid); // D.Casadei May 9 1998
private:
  Int_t _norm;
};


#endif
