
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSCanvas                                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include <TROOT.h>
#ifndef ROOT_TMotifCanvas_H
#include <TMotifCanvas.h>
#endif
#ifndef ROOT_TControlBar_H
#include <TControlBar.h>
#endif
#ifndef ROOT_TMenuBar_H
#include <TMenuBar.h>
#endif
#ifndef ROOT_TMenuToggle_H
#include <TMenuToggle.h>
#endif
#ifndef ROOT_TContextMenu_H
#include <TContextMenu.h>
#endif
#ifndef ROOT_TMenu_H
#include <TMenu.h>
#endif

#ifndef AMSRoot_H
#include "AMSRoot.h"
#endif
#ifndef AMSDisplay_H
#include "AMSDisplay.h"
#endif
#include "AMSCanvas.h"
//#include "AMSCanvasImp.h"
#include "Debugger.h"


MenuDesc_t AMSCanvas::fgAMSFilePane[] = {
   { kAction, "Save As canvas.ps", SaveCB, NULL },
   { kSeparator },
   { kEnd },
};


ClassImp(AMSCanvas)


//______________________________________________________________________________
AMSCanvas::AMSCanvas(Text_t *name, Text_t *title, Int_t ww, Int_t wh)
   : TCanvas(name,title,ww,wh)
// : TCanvas(name,title,-1) 	// do not show the TCanvas because it 
				// will be deleted later
{
/*
  delete fCanvasImp;		// delete the default TCanvasImp
  fCanvasImp = new AMSCanvasImp(this, name, fCw, fCh);
  fCanvasImp->ShowMenuBar(kTRUE);	// do show menu bar
  fCanvasImp->Show();
*/

  //
  // Default to show event status
  //
  Int_t show = GetShowEventStatus();
  if (show == 0) ToggleEventStatus();	// make sure event status is shown

  //
  // Reflect the setting on menu --- Motif-specific!
  //
  TMotifCanvas * canvas = (TMotifCanvas *) fCanvasImp;
  TMenuBar * menu = canvas->Menu();
  menu->AddSubmenu("AMS", fgAMSFilePane, NULL);
  //menu->AddAction("my print", this->SaveCB, this);
  //menu->AddAction("my print", this->*myfunc, this);
  TMenuItem * m = menu->FindNamedItem("Event Status");
  if (m->MenuType() == kToggle) ((TMenuToggle*)m)->SetVisualState(GetShowEventStatus());

}


//______________________________________________________________________________
 void AMSCanvas::Update()
{
//*-*-*-*-*-*-*-*-*Update canvas pad buffers*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*              =========================


   if (!IsBatch()) FeedbackMode(kFALSE);      // Goto double buffer mode

   PaintModified();           // Repaint all modified pad's
   Flush();                   // Copy all pad pixmaps to the screen
}

 void AMSCanvas::EditorBar()
{
//*-*-*-*-*-*-*-*-*-*-*Create the Editor Controlbar*-*-*-*-*-*-*-*-*-*
//*-*                  ============================

   TControlBar *ed = new TControlBar("vertical");
   ed->AddButton("Arc",      "gROOT->SetEditorMode(\"Arc\")",       "Create an arc of circle");
   ed->AddButton("Arrow",    "gROOT->SetEditorMode(\"Arrow\")",     "Create an Arrow");
   ed->AddButton("Button",   "gROOT->SetEditorMode(\"Button\")",    "Create a user interface Button");
   ed->Show();
   fEditorBar = ed;
}

//______________________________________________________________________________
void AMSCanvas::DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected)
{
//*-*-*-*-*-*-*Report name and title of primitive below the cursor*-*-*-*-*-*
//*-*          ===================================================
//
//    This function is called when the option "Event Status"
//    in the canvas menu "Options" is selected.
//

   const Int_t kTMAX=256;
   static char atext[kTMAX];
   static Int_t pxt, pyt;

   // debugger.Print("+++ AMSCanvas::DrawEventStatus() selected = %lx\n", selected);
   if (!selected) return;

#ifndef WIN32
   gPad->SetDoubleBuffer(0);           // Turn off double buffer mode

   TVirtualPad * gPadSave = gPad;
   AMSDisplay * disp = (AMSDisplay *) gAMSRoot->Display();
   TPad * pad = disp->GetObjInfoPad();
   static TText * text=0;

/*
   char * name = selected->GetName();
   if ( strncmp(name, "AMS", 3) == 0 ) {
     sprintf(atext,"%s: %s ", selected->GetName()
                           , selected->GetObjectInfo(px,py));
   }
   for (Int_t i=strlen(atext);i<kTMAX-1;i++) atext[i] = ' ';
   atext[kTMAX-1] = 0;

   if (! text) {
     text = new TText(0.01, 0.5, atext);
     text->SetTextColor(1);
     text->SetTextAlign(12);
     text->SetTextSize(0.65);
   }
   else {
     text->SetText(0.01, 0.5, atext);
   }
   pad->cd();
   text->Draw();
   gPadSave->cd();
*/

   Int_t x1, x2, y1, y2;
   x1 = pad->XtoAbsPixel(0.0);
   x2 = pad->XtoAbsPixel(1.0);
   y1 = pad->YtoAbsPixel(0.0);
   y2 = pad->YtoAbsPixel(1.0);
   pxt = 0.99 * x1 + 0.01 * x2;
   pyt = 0.45 * y1 + 0.55 * y2;

   //sprintf(atext,"%s / %s ", selected->GetName()
   //                        , selected->GetObjectInfo(px,py));
   const char * name = selected->GetName();
   if ( strncmp(name, "AMS", 3) == 0 ) {
     sprintf(atext,"%s: %s ", selected->GetName()
                           , selected->GetObjectInfo(px,py));
   }
   for (Int_t i=strlen(atext);i<kTMAX-1;i++) atext[i] = ' ';
   atext[kTMAX-1] = 0;
   gGXW->SetTextColor(1);
   gGXW->SetTextAlign(12);
   gGXW->SetTextSize(0.65*(y2-y1));

   gGXW->DrawText(pxt, pyt,    0,             1, atext, TGXW::kOpaque);
	//        x,   y,  angle, magnification, text, option

#else
   fCanvasImp->SetStatusText((Text_t *)(selected->GetTitle()),0);
   fCanvasImp->SetStatusText((Text_t *)(selected->GetName()),1);
   sprintf(atext,"%d,%d",px,py);
   fCanvasImp->SetStatusText(atext,2);
   fCanvasImp->SetStatusText((Text_t *)(selected->GetObjectInfo(px,py)),3);
#endif
}

//______________________________________________________________________________
void AMSCanvas::HandleInput(Int_t event, Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*Handle Input Events*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                ===================
//  Handle input events, like button up/down in current canvas.
//

   TPad         *pad;
   TObjLink     *pickobj;

   fPadSave = (TPad*)gPad;
   cd();        // make sure this canvas is the current canvas

   fEvent = event;

   switch (event) {

   case kMouseMotion:
      // highlight object tracked over
      fSelected    = 0;
      fSelectedOpt = "";
      fSelectedPad = 0;
      pickobj      = 0;
      pad = Pick(px, py, pickobj);
      if (!pad) return;

      if (!pickobj) {
         fSelected    = pad;
         fSelectedOpt = "";
      } else {
         if (!fSelected) {
            fSelected    = pickobj->GetObject();
            fSelectedOpt = pickobj->GetOption();
         }
      }
      fSelectedPad = pad;

      gPad = pad;   // don't use cd() we will use the current
                    // canvas via the GetCanvas member and not via
                    // gPad->GetCanvas

      fSelected->ExecuteEvent(event, px, py);

      if (fShowEventStatus) DrawEventStatus(event, px, py, fSelected);

      break;

   case kButton1Down:

      // find pad in which input occured
      fSelected    = 0;
      fSelectedOpt = "";
      fSelectedPad = 0;
      pickobj      = 0;
      pad = Pick(px, py, pickobj);
      if (!pad) return;

      if (!pickobj) {
         fSelected    = pad;
         fSelectedOpt = "";
      } else {
         if (!fSelected) {
            fSelected    = pickobj->GetObject();
            fSelectedOpt = pickobj->GetOption();
         }
      }
      fSelectedPad = pad;

      gPad = pad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system

      FeedbackMode(kTRUE);   // to draw in rubberband mode

      fSelected->ExecuteEvent(event, px, py);

      if (fShowEventStatus) DrawEventStatus(event, px, py, fSelected);

      break;

   case kButton1Motion:

      if (fSelected) {
         gPad = fSelectedPad;

         fSelected->ExecuteEvent(event, px, py);

         if (fSelected->InheritsFrom(TBox::Class()))
            if ((!((TBox*)fSelected)->IsBeingResized() && fMoveOpaque) ||
                (((TBox*)fSelected)->IsBeingResized() && fResizeOpaque)) {
               gPad = fPadSave;
               Update();
               FeedbackMode(kTRUE);
            }
         if (fShowEventStatus) DrawEventStatus(event, px, py, fSelected);
      }

      break;

   case kButton1Up:

      if (fSelected) {
         gPad = fSelectedPad;

         fSelected->ExecuteEvent(event, px, py);

         if (fShowEventStatus) DrawEventStatus(event, px, py, fSelected);

         if (fPadSave->TestBit(kNotDeleted))
            gPad = fPadSave;
         else {
            gPad     = this;
            fPadSave = this;
         }

         Update();    // before calling update make sure gPad is reset
      }

      break;

//*-*----------------------------------------------------------------------

   case kButton2Down:
      // find pad in which input occured
      fSelected    = 0;
      fSelectedOpt = "";
      fSelectedPad = 0;
      pickobj      = 0;
      pad = Pick(px, py, pickobj);
      if (!pad) return;

      if (!pickobj) {
         fSelected    = pad;
         fSelectedOpt = "";
      } else {
         if (!fSelected) {
            fSelected    = pickobj->GetObject();
            fSelectedOpt = pickobj->GetOption();
         }
      }
      fSelectedPad = pad;

      gPad = pad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system

      FeedbackMode(kTRUE);

      fSelected->Pop();           // pop object to foreground
      pad->cd();                  // and make its pad the current pad
      if (gDebug)
         printf("Current Pad: %s / %s\n", pad->GetName(), pad->GetTitle());

      // loop over all canvases to make sure that only one pad is highlighted
      {
         TIter next(gROOT->GetListOfCanvases());
         TCanvas *tc;
         while (tc = (TCanvas *)next())
            tc->Update();
      }

      return;   // don't want fPadSave->cd() to be executed at the end

      break;

   case kButton2Motion:
      break;

   case kButton2Up:
      break;

//*-*----------------------------------------------------------------------

   case kButton3Down:
   {
      pad = Pick( px, py, pickobj );
      if ( !pad ) return;

      if (!pickobj) {
         fSelected    = pad;
         fSelectedOpt = "";
      } else {
         if (!fSelected) {
            fSelected    = pickobj->GetObject();
            fSelectedOpt = pickobj->GetOption();
         }
      }
      fSelectedPad = pad;

      if( fContextMenu )
          fContextMenu->Popup( px, py, fSelected, this, pad );

      break;
   }
   case kButton3Motion:
      break;

   case kButton3Up:
      break;
   }

   fPadSave->cd();
}


//______________________________________________________________________
void AMSCanvas::SaveCB(Widget wid, XtPointer cd, XtPointer pointer)
{

   // first append the particle info in event status pad
   AMSDisplay * disp = (AMSDisplay *)gAMSRoot->Display();
   TPad * objInfo     = disp->GetObjInfoPad();
   TVirtualPad * gPadSave = gPad;
   objInfo->cd();

   AMSMaker * p = (AMSMaker *) gAMSRoot->ParticleMaker();
   
  TObject *fruits = p->Fruits();
  TObject *obj;
  char * info=0;

// If m_Fruits is a ClonesArray, insert all the objects in the list
// of objects to be painted

  if (fruits->InheritsFrom("TClonesArray")) {
     TClonesArray *clones = (TClonesArray*)fruits;
     Int_t nobjects = clones->GetEntries();
     for (Int_t i=0;i<nobjects;i++) {
        obj = clones->At(i);
        if (obj && i==0){ 
           debugger.Print("obj class = %s\n", obj->ClassName());}
        if (obj && p->Enabled(obj)) 
           info = obj->GetObjectInfo(0,0);
     }
// m_Fruits points to an object in general. Insert this object in the pad
  } else {
     info = fruits->GetObjectInfo(0,0);
  }

  debugger.Print("get particle info: %s\n", info);
  char info1[80];
  sprintf(info1, "Particle: %s", info);
  TText * text = new TText(0.01, 0.45, info1);	// should be 0.5 , but somehow 0.45 is better
  text->SetTextSize(0.60);	// 0.65 should be fine, but somehow 0.60 is better
  text->SetTextAlign(12);
  text->Draw();

   // then print it
   // TMotifCanvas *canv = (TMotifCanvas *) cd;
   //AMSCanvasImp *canv = (AMSCanvasImp *) cd;
   //debugger.Print("canv = %lx\n", canv);
   //if (canv) canv->Canvas()->SaveAs();
  gPadSave->cd();

   disp->GetCanvas()->SaveAs();

//  delete text;
}

