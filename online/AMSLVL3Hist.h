//  $Id: AMSLVL3Hist.h,v 1.4 2003/06/17 07:39:54 choutko Exp $
#ifndef __AMSLVL3HIST__
#define __AMSLVL3HIST__
#include "AMSHist.h"

class AMSLVL3Hist : public AMSHist{
public:
AMSLVL3Hist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};



#endif
