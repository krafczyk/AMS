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
   static MenuDesc_t  fgAMSFileANTI[];
   static MenuDesc_t  fgAMSFileTracker[];
   static MenuDesc_t  fgAMSFileLVL1[];
   static MenuDesc_t  fgAMSFileLVL3[];
   static MenuDesc_t  fgAMSFileTOF[];
   static MenuDesc_t  fgAMSFileCTC[];
   static MenuDesc_t  fgAMSFileAxAMS[];
   static TMotifCanvas * fTheCanvas;

public:
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

   // Anti
   static void        AntiSet0CB(Widget, XtPointer, XtPointer);
   static void        AntiSet1CB(Widget, XtPointer, XtPointer);
   static void        AntiSet2CB(Widget, XtPointer, XtPointer);
   static void        AntiSet3CB(Widget, XtPointer, XtPointer);
   static void        AntiSet4CB(Widget, XtPointer, XtPointer);
   // Tracker
   // Level1
   static void        LVL1Set0CB(Widget, XtPointer, XtPointer);
   static void        LVL1Set1CB(Widget, XtPointer, XtPointer);
   // Level3
   static void        LVL3Set0CB(Widget, XtPointer, XtPointer);
   static void        LVL3Set1CB(Widget, XtPointer, XtPointer);
   // TOF
   // CTC
   static void        CTCSet0CB(Widget, XtPointer, XtPointer);
   static void        CTCSet1CB(Widget, XtPointer, XtPointer);
   // AxAMS
   static void        AxAMSSet0CB(Widget, XtPointer, XtPointer);
   static void        AxAMSSet1CB(Widget, XtPointer, XtPointer);

   ClassDef(AMSCanvas, 0)   //AMS canvas control class
};

#endif
