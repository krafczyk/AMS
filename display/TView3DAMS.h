#ifndef TView3DAMS_H
#define TView3DAMS_H

#include "TView3D.h"

class TView3DAMS: public TView3D{

 public:
  Bool_t Hack;
  
  TView3DAMS()
    :TView3D(),Hack(kFALSE){}

  TView3DAMS(Int_t system, const Double_t *rmin, const Double_t *rmax)
    :TView3D(system,rmin,rmax),Hack(kFALSE){}

  TView3DAMS(const TView3DAMS& tv)
    :TView3D(tv),Hack(tv.Hack){}
  
  
  virtual ~TView3DAMS(){}

  void SetRange(const Double_t *min, const Double_t *max);
  void SetRange(Double_t x0, Double_t y0, Double_t z0, Double_t x1, Double_t y1, Double_t z1, Int_t flag=0);
  void GetRange(Float_t *min, Float_t *max);
  void GetRange(Double_t *min, Double_t *max);


  ClassDef(TView3DAMS,1);
};

#endif
