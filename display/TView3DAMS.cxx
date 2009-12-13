#include "TView3DAMS.h"
#include <math.h>

  ClassImp(TView3DAMS);

void TView3DAMS::SetRange(Double_t x0, Double_t y0, Double_t z0, Double_t x1, Double_t y1, Double_t z1, Int_t flag){
  TView3D::SetRange(x0,y0,z0,x1,y1,z1,flag);
}


//______________________________________________________________________________
void TView3DAMS::GetRange(Float_t *min, Float_t *max)
{
  //*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*Get Range function-*-*-*-*-*-*-*-*-*-*-*-*-*
  //*-*                            ==================
  //*-*
  double cc=sqrt(3.);
  if (IsPerspective() || !Hack )cc=1;
  for (Int_t i = 0; i < 3; max[i] = fRmax[i]*cc, min[i] = fRmin[i]*cc, i++);
}


//______________________________________________________________________________
void TView3DAMS::SetRange(const Double_t *min, const Double_t *max)
{
   //*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*Set Range function-*-*-*-*-*-*-*-*-*-*-*-*-*
   //*-*                            ==================
   //*-*
   Int_t irep;

   if (IsPerspective() || !Hack){
     for (Int_t i = 0; i < 3; fRmax[i] = max[i], fRmin[i] = min[i], i++);         }
   else{
     const double sc1=1/sqrt(3.);
     const double sc2=(1-sc1)/2.;
     for(int k=0;k<3;k++){           double shift=(min[k]+max[k])*sc2;
       fRmin[k]=shift+min[k]*sc1;
       fRmax[k]=shift+max[k]*sc1;
     }
   }
   
   if (IsPerspective()) SetDefaultWindow();
   ResetView(fLongitude, fLatitude, fPsi, irep);
   if(irep < 0)
     Error("SetRange", "problem setting view");
   if(fDefaultOutline) SetOutlineToCube();
}

