#ifndef __AMSTOFHIST__
#define __AMSTOFHIST__
#include "AMSHist.h"

class AMSTOFHist : public AMSHist{
public:
AMSTOFHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active);
void ShowSet(Int_t cset);
void _Fill();
void _Fetch();
ClassDef(AMSTOFHist,0)    // AMSTOFHist class
};


#endif
