//  $Id: AMSAntiHist.h,v 1.3 2001/01/22 17:32:51 choutko Exp $
#ifndef __AMSANTIHIST__
#define __AMSANTIHIST__
#include "AMSHist.h"

class AMSAntiHist : public AMSHist{
public:
AMSAntiHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active);
void ShowSet(Int_t cset);
void _Fill();
void _Fetch();
void Fill(AMSNtuple * ntuple);
ClassDef(AMSAntiHist,0)    // AMSAntiHist class
};


#endif
