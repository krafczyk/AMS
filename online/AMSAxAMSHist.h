#ifndef __AMSAxAMSHIST__
#define __AMSAxAMSHIST__
#include "AMSHist.h"

class AMSAxAMSHist : public AMSHist{
public:
AMSAxAMSHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active);
void ShowSet(Int_t cset);
void _Fill();
void _Fetch();
ClassDef(AMSAxAMSHist,0)    // AMSAxAMSHist class
};


#endif
