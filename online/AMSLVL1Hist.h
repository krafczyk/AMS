#ifndef __AMSLVL1HIST__
#define __AMSLVL1HIST__
#include "AMSHist.h"

class AMSLVL1Hist : public AMSHist{
public:
AMSLVL1Hist(Text_t * name, Text_t * title, Int_t maxset, Int_t active);
void ShowSet(Int_t cset);
void _Fill();
void _Fetch();
ClassDef(AMSLVL1Hist,0)    // AMSLVL1Hist class
};


#endif
