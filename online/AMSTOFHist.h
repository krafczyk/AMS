#ifndef __AMSTOFHIST__
#define __AMSTOFHIST__
#include "AMSHist.h"

class AMSTOFHist : public AMSHist{
public:
  ClassDef(AMSTOFHist,0)    // AMSTOFHist class
  AMSTOFHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active);
  void ShowSet(Int_t cset);
  void _Fill();
  void Fill(AMSNtuple *ntuple);
  void _Fetch();
//   Float_t[] GetAnodeCharge(){return _anode[]};
//   Float_t[] GetAnodeChaRMS(){return _anorms[]};
//   Float_t[] GetDynodeCharge(){return _dynode[]};
//   Float_t[] GetDynodeChaRMS(){return _dynrms[]};
//   Float_t[] GetStretch(){return _stretch[]};
//   Float_t[] GetStreRMS(){return _strrms[]};
private:
  Float_t _anode[112],_anorms[112];
  Float_t _dynode[112],_dynrms[112];
  Float_t _stretch[112],_strrms[112];
};


#endif
