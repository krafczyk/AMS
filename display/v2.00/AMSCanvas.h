#ifndef AMSCanvas_H
#define AMSCanvas_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSCanvas                                                            //
//                                                                      //
// Main class to control the canvas.                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TCanvas.h>
#include <TRootCanvas.h>
#include <TGCanvas.h>



class AMSCanvas : public TCanvas {

private:
            void      DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected);

protected:

public:
   static TRootCanvas * fTheCanvas;
   static   void      AddParticleInfo();
                      AMSCanvas() : TCanvas()  {}
                      AMSCanvas(Text_t *name, Text_t *title="The AMS Display with Root", Int_t ww=700, Int_t wh=550);
   virtual           ~AMSCanvas() {}

            void      Update();
            void      EditorBar();
	    void      HandleInput(Int_t event, Int_t px, Int_t py);

   //
   // Call Back functions
   //
   static   void      OpenFileCB();
   static   void      SaveParticleCB();
   static   void      SaveParticleGIF();
   static   void      PrintCB();


   ClassDef(AMSCanvas, 0)   //AMS canvas control class
};

#endif
