//  $Id: AMSLVL1Hist.h,v 1.4 2003/06/17 07:39:53 choutko Exp $
#ifndef __AMSLVL1HIST__
#define __AMSLVL1HIST__
#include "AMSHist.h"


#include "AMSHist.h"

class AMSLVL1Hist : public AMSHist{
public:
AMSLVL1Hist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};




#endif
