//  $Id: AMSAntiHist.h,v 1.4 2003/06/17 07:39:53 choutko Exp $
#ifndef __AMSANTIHIST__
#define __AMSANTIHIST__
#include "AMSHist.h"

#include "AMSHist.h"

class AMSAntiHist : public AMSHist{
public:
AMSAntiHist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};


#endif
