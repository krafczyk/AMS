//  $Id: AMSTOFHist.h,v 1.5 2001/01/22 17:32:52 choutko Exp $
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
private:
  Int_t _norm;
};


#endif
