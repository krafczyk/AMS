#ifndef __AMSLVL3HIST__
#define __AMSLVL3HIST__
#include "AMSHist.h"

class AMSLVL3Hist : public AMSHist{
public:
AMSLVL3Hist(Text_t * name, Text_t * title, Int_t maxset, Int_t active);
void ShowSet(Int_t cset);
void _Fill();
void _Fetch();
ClassDef(AMSLVL3Hist,0)    // AMSLVL3Hist class
};


#endif
