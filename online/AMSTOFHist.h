//  $Id: AMSTOFHist.h,v 1.6 2003/06/17 07:39:54 choutko Exp $
#ifndef __AMSTOFHIST__
#define __AMSTOFHIST__
#include "AMSHist.h"

#include "AMSHist.h"

class AMSTOFHist : public AMSHist{
public:
AMSTOFHist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};



#endif
