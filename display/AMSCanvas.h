//  $Id: AMSCanvas.h,v 1.5 2001/01/22 17:32:31 choutko Exp $
#ifndef AMSCanvas_H
#define AMSCanvas_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSCanvas                                                            //
//                                                                      //
// Main class to control the canvas.                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

/*
#ifndef ROOT_TMotifCanvas
#include <TMotifCanvas.h>
#endif
*/
class TMotifCanvas;
#ifndef ROOT_TCanvas
#include <TCanvas.h>
#endif

//
// #include <TMotifCanvas.h>  makes rootcint crazy!
// Use the following forward declarations to get around this problem
// 
typedef struct _WidgetRec *Widget;
typedef void*           XtPointer;
struct MenuDesc_t;


class AMSCanvas : public TCanvas {

private:
            void      DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected);

protected:
   static MenuDesc_t  fgAMSFilePane[];
   static TMotifCanvas * fTheCanvas;

public:
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
   static   void      OpenFileCB(Widget, XtPointer, XtPointer);
   static   void      SaveParticleCB(Widget, XtPointer, XtPointer);
   static   void      SaveParticleGIF(Widget, XtPointer, XtPointer);
   static   void      PrintCB(Widget, XtPointer, XtPointer);


   ClassDef(AMSCanvas, 0)   //AMS canvas control class
};

#endif
