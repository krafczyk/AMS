#ifndef __AMSANTIHIST__
#define __AMSANTIHIST__
#include "AMSHist.h"

class AMSAntiHist : public AMSHist{
public:
AMSAntiHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active);
void ShowSet(Int_t cset);
void _Fill();
void _Fetch();
ClassDef(AMSAntiHist,0)    // AMSAntiHist class
};


#endif
