//  $Id: AMSGenHist.h,v 1.1 2003/06/17 08:14:20 choutko Exp $
#ifndef __AMSGENHIST__
#define __AMSGENHIST__
#include "AMSHist.h"

#include "AMSHist.h"

class AMSGenHist : public AMSHist{
public:
AMSGenHist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};



#endif
