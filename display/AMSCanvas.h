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
#ifndef ROOT_TCanvas
#include <TCanvas.h>
#endif

typedef struct _WidgetRec *Widget;
typedef void*           XtPointer;
struct MenuDesc_t;


class AMSCanvas : public TCanvas {

private:
            void      DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected);
   static MenuDesc_t  fgAMSFilePane[];

public:
                      AMSCanvas() : TCanvas()  {}
                      AMSCanvas(Text_t *name, Text_t *title="The AMS Display with Root", Int_t ww=700, Int_t wh=550);
   virtual           ~AMSCanvas() {}

            void      Update();
            void      EditorBar();
	    void      HandleInput(Int_t event, Int_t px, Int_t py);

   static   void      SaveCB(Widget, XtPointer, XtPointer);


   ClassDef(AMSCanvas, 0)   //AMS canvas control class
};

#endif
