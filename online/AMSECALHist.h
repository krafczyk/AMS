//  $Id: AMSECALHist.h,v 1.2 2008/03/14 14:51:32 choumilo Exp $
#ifndef __AMSECALHIST__
#define __AMSECALHIST__
#include "AMSHist.h"

const Float_t emctrange[3]={120,120,120};//mins,hours,days

class AMSECALHist : public AMSHist{
public:
AMSECALHist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};


#endif

