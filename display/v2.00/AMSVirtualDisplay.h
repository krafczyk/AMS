#ifndef AMSVirtualDisplay_H
#define AMSVirtualDisplay_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSVirtualDisplay                                                    //
//                                                                      //
// Virtual base class for AMSRoot event display                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif

//class AMSTrigger;

enum EAMSView { kFrontView=0, kSideView=1, kTopView=2, kAllView=3, kTwoView=4 };

class AMSVirtualDisplay : public TObject {

public:
                     AMSVirtualDisplay();
   virtual          ~AMSVirtualDisplay();
   virtual void      Clear(Option_t *option="") = 0;
   virtual void      DisplayButtons() = 0;
//Ping   virtual void      DisplayTrigger(AMSTrigger *trig) = 0;
   virtual Int_t     DistancetoPrimitive(Int_t px, Int_t py) = 0;
   virtual void      Draw(Option_t *option="") = 0;
   virtual void      DrawAllViews()  = 0;
   virtual void      DrawFrontAndSideViews()  = 0;
//   virtual Bool_t    DrawClusters()  = 0;
   virtual Bool_t    DrawParticles() = 0;
   virtual void      DrawCaption(Option_t *option="") = 0;
   virtual void      DrawView(Float_t theta, Float_t phi, Int_t index) = 0;
   virtual void      DrawViewGL() = 0;
   virtual void      DrawViewX3D() = 0;
   virtual void      ExecuteEvent(Int_t event, Int_t px, Int_t py) = 0;
   virtual void      Paint(Option_t *option="") = 0;
//   virtual void      PaintFruit(TObject *obj, Float_t eta, Float_t phi, Float_t pt, Int_t type, Option_t *option="") = 0;
//   virtual void      PaintFruit(TObject *obj, Float_t * xyz, Float_t * erxyz, Int_t type, Option_t *option="") = 0;
   virtual void      PaintParticles(Option_t *option="") = 0;
   virtual Float_t   PTcut() = 0;
   virtual Float_t   PTcutEGMUNU() = 0;
   virtual void      SetView(Float_t theta, Float_t phi) = 0;
   virtual void      SetView(EAMSView newView=kFrontView) = 0;
   virtual void StartStop()=0;
   virtual void Select()=0;
   virtual void Print()=0;
   virtual void      ShowNextEvent(Int_t delta=1) = 0;
//   virtual void      SizeFruit() const;
   virtual void      SizeParticles() const;
//         void      SetGeo();
//         void      OkGeo();

   ClassDef(AMSVirtualDisplay, 0)   //Virtual base class for ATLFast event display
};

#endif
