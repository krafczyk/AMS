//  $Id: AMSGenHist.h,v 1.2 2001/01/22 17:32:52 choutko Exp $
#ifndef __AMSGENHIST__
#define __AMSGENHIST__
#include "AMSHist.h"

class AMSGenHist : public AMSHist{
public:
AMSGenHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active);
void ShowSet(Int_t cset);
void _Fill();
void _Fetch();
void Fill(AMSNtuple * ntuple);
ClassDef(AMSGenHist,0)    
};


#endif
