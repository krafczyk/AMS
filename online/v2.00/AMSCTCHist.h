//  $Id: AMSCTCHist.h,v 1.2 2001/01/22 17:32:52 choutko Exp $
#ifndef __AMSCTCHIST__
#define __AMSCTCHIST__
#include "AMSHist.h"

class AMSCTCHist : public AMSHist{
public:
AMSCTCHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active);
void ShowSet(Int_t cset);
void _Fill();
void _Fetch();
void Fill(AMSNtuple * ntuple);
ClassDef(AMSCTCHist,0)    // AMSCTCHist class
};
#endif
