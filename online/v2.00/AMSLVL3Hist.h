//  $Id: AMSLVL3Hist.h,v 1.2 2001/01/22 17:32:52 choutko Exp $
#ifndef __AMSLVL3HIST__
#define __AMSLVL3HIST__
#include "AMSHist.h"

class AMSLVL3Hist : public AMSHist{
public:
AMSLVL3Hist(Text_t * name, Text_t * title, Int_t maxset, Int_t active);
void ShowSet(Int_t cset);
void _Fill();
void _Fetch();
void Fill(AMSNtuple *ntuple);
ClassDef(AMSLVL3Hist,0)    // AMSLVL3Hist class
};


#endif
