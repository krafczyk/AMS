
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSCanvas                                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <unistd.h>		// for getpid()
#include <iostream.h>
#include <stdlib.h>
#include <TROOT.h>
#ifndef ROOT_TMotifCanvas
#include <TMotifCanvas.h>
#endif
#ifndef ROOT_TControlBar
#include <TControlBar.h>
#endif
#ifndef ROOT_TMenuBar
#include <TMenuBar.h>
#endif
#ifndef ROOT_TMenuToggle
#include <TMenuToggle.h>
#endif
#ifndef ROOT_TContextMenu
#include <TContextMenu.h>
#endif
#ifndef ROOT_TMenu
#include <TMenu.h>
#endif
#ifndef ROOT_TMotifDialog
#include <TMotifDialog.h>
#endif
#ifndef ROOT_TFileSelectionDialog
#include <TFileSelectionDialog.h>
#endif
#ifndef ROOT_TErrorDialog
#include <TErrorDialog.h>
#endif
#ifndef ROOT_TInfoDialog
#include <TInfoDialog.h>
#endif
#ifndef ROOT_TBusyDialog
#include <TBusyDialog.h>
#endif
#ifndef ROOT_TPromptDialog
#include <TPromptDialog.h>
#endif
#ifndef ROOT_TQuestionDialog
#include <TQuestionDialog.h>
#endif
#ifndef ROOT_TWarningDialog
#include <TWarningDialog.h>
#endif
#ifndef ROOT_TSelectionDialog
#include <TSelectionDialog.h>
#endif

//#include "MySelectionDialog.h"


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
   { kAction, "Save As Run.Event.ps", SaveParticleCB, NULL },
   { kAction, "Save As Run.Event.gif", SaveParticleGIF, NULL },
   { kAction, "Print",PrintCB, NULL },
   { kAction, "Open data file", OpenFileCB, NULL },
   { kEnd },
};

TMotifCanvas * AMSCanvas::fTheCanvas = 0;


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
  if (fTheCanvas) {
    //
    // This means AMSCanvas is intialized more than once!
    //
    fprintf(stderr, "You can only have one AMSCanvas!\n");
    return;
  }
  else {
    fTheCanvas = (TMotifCanvas *) fCanvasImp;
    printf("fTheCanvas = %lx in AMSCanvas::AMSCanvas()\n", fTheCanvas);
  }

  TMotifCanvas * canvas = (TMotifCanvas *) fCanvasImp;
  TMenuBar * menu = canvas->Menu();
  menu->AddSubmenu("AMS", fgAMSFilePane, NULL);
  TMenuItem * classesMenu = menu->RemoveItem("Classes");
  TMenuItem * inspectorMenu = menu->RemoveItem("Inspector");
  TMenuItem * editMenu = menu->RemoveItem("Edit");
  TMenuItem * viewMenu = menu->RemoveItem("View");
  //menu->AddAction("my print", this->SaveCB, this);
  //menu->AddAction("my print", this->*myfunc, this);
  TMenuItem * m = menu->FindNamedItem("Event Status");
  if (m->MenuType() == kToggle) ((TMenuToggle*)m)->SetVisualState(GetShowEventStatus());


  //
  // testing codes
  //
/*
  TErrorDialog * erdi = new TErrorDialog("erdi");
  printf(" erdi is instantiated? %lx %d\n", erdi->BaseWidget(), erdi->Instantiated());
  Widget erdiWidget = erdi->CreateDialog(canvas->BaseWidget());
  printf(" erdi is instantiated? %lx %d\n", erdi->BaseWidget(), erdi->Instantiated());
  // try a non-modal dialog
  erdi->Post("A test message", "help string", canvas);
  // try a modal dialog
  //erdi->PostModal("A test message", "help string", canvas);
  //printf(" erdi is instantiated? %lx %d\n", erdi->BaseWidget(), erdi->Instantiated());
  printf(" erdi is instantiated? %lx %d\n", erdi->BaseWidget(), erdi->Instantiated());
  //erdi->Activate();
  //printf(" erdi is instantiated? %d\n", erdi->Instantiated());
  erdi->Show();
  printf(" erdi is instantiated? %lx %d\n", erdi->BaseWidget(), erdi->Instantiated());
*/

/*
  gErrorDialogManager->Post("My error message", canvas);
  gInfoDialogManager->Post("My info", canvas);
  gBusyDialogManager->Post("My busy dialog", canvas);

  gPromptDialogManager->Post("My prompt dialog", canvas);
  gQuestionDialogManager->Post("My question dialog", canvas);

  gWarningDialogManager->Post("My warning dialog", canvas);
*/


/*
  TSelectionDialog * mySelectionDialog = new TSelectionDialog("Select one");
  mySelectionDialog->AddItem("item 1");
  mySelectionDialog->AddItem("item 2");
  mySelectionDialog->AddItem("item 3");
  mySelectionDialog->PostBlocked("My selection dialog", canvas);
*/

/*  this is working now
  MySelectionDialog * selectionDialog = new MySelectionDialog("Select one");
  selectionDialog->AddItem("item 1");
  selectionDialog->AddItem("item 2");
  selectionDialog->AddItem("item 3");
  selectionDialog->PostBlocked("My selection dialog", canvas);
*/

/*
  TFileSelectionDialog * myFileSelectionDialog = new TFileSelectionDialog("Select a file");
  myFileSelectionDialog->SetDirectory(".");
  //myFileSelectionDialog->SetFilterPattern("*.h");
  myFileSelectionDialog->PostBlocked("select one file", canvas);

  char mesg[100];
  sprintf(mesg, "You selected %s", myFileSelectionDialog->FileName());
  printf("%s\n", mesg);
  // gInfoDialogManager->Post(mesg);

  delete myFileSelectionDialog;
*/

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

#if ! defined(WIN32)
   gPad->SetDoubleBuffer(0);           // Turn off double buffer mode

   TVirtualPad * gPadSave = gPad;
   AMSDisplay * disp = (AMSDisplay *) gAMSRoot->Display();
   TPad * pad = disp->GetObjInfoPad();
   static TText * text=0;

/*
   char * name = selected->GetName();
   if ( strncmp(name, "AMS", 3) == 0 ) {
     atext[0]='\0';
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
     atext[0]='\0';
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





////////////////////////////////////////////////////////////////////////
//                                                                    //
//                      Menu     Functions                            //
//                                                                    //
////////////////////////////////////////////////////////////////////////

//______________________________________________________________________
void AMSCanvas::SaveParticleCB(Widget wid, XtPointer cd, XtPointer pointer)
{
   AMSDisplay * disp = (AMSDisplay *)gAMSRoot->Display();
   disp->AddParticleInfo();
   char fnam[255];
   sprintf(fnam, "%u.%u.ps",gAMSRoot->RunNum(),gAMSRoot->EventNum());
   disp->GetCanvas()->SaveAs(fnam);
   disp->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::SaveParticleGIF(Widget wid, XtPointer cd, XtPointer pointer)
{
   AMSDisplay * disp = (AMSDisplay *)gAMSRoot->Display();
   disp->AddParticleInfo();
   char fnam[255];
   sprintf(fnam, "%u.%u.gif",gAMSRoot->RunNum(),gAMSRoot->EventNum());
   disp->GetCanvas()->SaveAs(fnam);
   disp->GetCanvas()->Update();		// refresh the screen
}


//______________________________________________________________________
void AMSCanvas::OpenFileCB(Widget wid, XtPointer cd, XtPointer pointer)
{

  //printf("in AMSCanvas::OpenFile()... fTheCanvas = %lx\n", fTheCanvas);
  //char ch = fgetc(stdin);

  AMSDisplay * disp = (AMSDisplay *)gAMSRoot->Display();
  TMotifCanvas * canvas = (TMotifCanvas *) disp->GetCanvas()->GetCanvasImp();

  // TMotifCanvas * canvas = fTheCanvas; 

  TFileSelectionDialog * myFileSelectionDialog = new TFileSelectionDialog("Select a file");

  //printf("myFileSelectionDialog instantiated? %lx %d\n", myFileSelectionDialog->BaseWidget(), myFileSelectionDialog->Instantiated());
  //myFileSelectionDialog->SetFilterPattern("*.h");
  //myFileSelectionDialog->CreateDialog(canvas->BaseWidget());
  //printf("myFileSelectionDialog instantiated? %lx %d\n", myFileSelectionDialog->BaseWidget(), myFileSelectionDialog->Instantiated());

  myFileSelectionDialog->SetDirectory(".");
  myFileSelectionDialog->PostBlocked("select one file", canvas);

  //myFileSelectionDialog->PostModal("select one file", canvas);

  char mesg[100];
  sprintf(mesg, "You selected %s", myFileSelectionDialog->FileName());
  printf("%s\n", mesg);

  delete myFileSelectionDialog;

  return;

}


//______________________________________________________________________
void AMSCanvas::PrintCB(Widget wid, XtPointer cd, XtPointer pointer)
{
 
   AMSDisplay * disp = (AMSDisplay *)gAMSRoot->Display();
   disp->AddParticleInfo();
   pid_t pid = getpid();
   char filename[80];
   sprintf(filename, "/tmp/AMSDisplay.%u.ps",pid);
   disp->GetCanvas()->SaveAs(filename);
   disp->GetCanvas()->Update();		// refresh the screen
   char cmd[255];
   sprintf(cmd, "lpr /tmp/AMSDisplay.%u.ps",pid);
   system(cmd);
   sprintf(cmd, "rm /tmp/AMSDisplay.%u.ps",pid);
   system(cmd);
}


//______________________________________________________________________
void AMSCanvas::AddParticleInfo()
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
  }
  else {
     info = fruits->GetObjectInfo(0,0);
  }

  debugger.Print("get particle info: %s\n", info);
  char info1[80];
  cout <<" u "<<info<<endl;
  sprintf(info1, "Particle: %s", info);
  TText * text = new TText(0.01, 0.45, info1);	// should be 0.5 , but somehow 0.45 is better
  text->SetTextSize(0.60);	// 0.65 should be fine, but somehow 0.60 is better
//  text->SetTextColor(0);
  text->SetTextAlign(12);
  text->Draw();
  delete text;
   // then print it
   // TMotifCanvas *canv = (TMotifCanvas *) cd;
   //AMSCanvasImp *canv = (AMSCanvasImp *) cd;
   //debugger.Print("canv = %lx\n", canv);
   //if (canv) canv->Canvas()->SaveAs();
  gPadSave->cd();

}

