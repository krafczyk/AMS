//  $Id: AMSTrackerHist.h,v 1.2 2001/01/22 17:32:53 choutko Exp $
#ifndef __AMSTrackerHIST__
#define __AMSTrackerHIST__
#include "AMSHist.h"

class AMSTrackerHist : public AMSHist{
public:
AMSTrackerHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active);
void ShowSet(Int_t cset);
void _Fill();
void _Fetch();
void Fill (AMSNtuple * ntuple);
ClassDef(AMSTrackerHist,0)    // AMSTrackerHist class
};


#endif
