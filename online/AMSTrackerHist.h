#ifndef __AMSTrackerHIST__
#define __AMSTrackerHIST__
#include "AMSHist.h"

class AMSTrackerHist : public AMSHist{
public:
AMSTrackerHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active);
void ShowSet(Int_t cset);
void _Fill();
void _Fetch();
ClassDef(AMSTrackerHist,0)    // AMSTrackerHist class
};


#endif
