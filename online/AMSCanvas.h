//  $Id: AMSCanvas.h,v 1.9 2003/06/17 07:39:53 choutko Exp $
#ifndef AMSCanvas_H
#define AMSCanvas_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSCanvas                                                            //
//                                                                      //
// Main class to control the canvas.                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TRootCanvas.h>
#include <TCanvas.h>

class AMSCanvas : public TCanvas {

private:
            void      DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected);

protected:

public:
   static TRootCanvas * fTheCanvas;
                      AMSCanvas() : TCanvas()  {}
                      AMSCanvas(Text_t *name, Text_t *title="The AMS Display with Root", Int_t ww=700, Int_t wh=550);
   virtual           ~AMSCanvas() {}

            void      Update();
            void      EditorBar();
	    void      HandleInput(Int_t event, Int_t px, Int_t py);

   //
   // Call Back functions
   //

   static   void      SaveParticleCB();
   static   void      SaveParticleGIF();
   static   void      PrintCB();

   static void        SubDet(int det, int set);

   ClassDef(AMSCanvas, 0)   //AMS canvas control class
};

#endif
