//  $Id: AMSEverything.h,v 1.2 2010/10/21 12:34:54 choutko Exp $
#ifndef __AMSEEverything__
#define __AMSEverything__
#include "AMSHist.h"
class AMSEverything : public AMSHist{
public:
AMSEverything(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};


#endif
