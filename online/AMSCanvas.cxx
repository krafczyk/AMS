
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

#include <TText.h>

#include "AMSDisplay.h"
#include "AMSCanvas.h"
#include "Debugger.h"


MenuDesc_t AMSCanvas::fgAMSFilePane[] = {
   { kAction, "Save As SubDetector.Set.ps", SaveParticleCB, NULL },
   { kAction, "Save As subDetector.Set.gif", SaveParticleGIF, NULL },
   { kAction, "Print",PrintCB, NULL },
   { kEnd },
};

MenuDesc_t AMSCanvas::fgAMSFileANTI[] = {
   { kAction, "Occupancies/Amplitudes Distibution", AntiSet0CB, NULL },
   { kAction, "UpVsDown 1-4", AntiSet1CB, NULL },
   { kAction, "UpVsDown 5-8", AntiSet2CB, NULL },
   { kAction, "UpVsDown 9-12", AntiSet3CB, NULL },
   { kAction, "UpVsDown 13-16", AntiSet4CB, NULL },
   { kEnd },
};

MenuDesc_t AMSCanvas::fgAMSFileTracker[] = {
   { kEnd },
};

MenuDesc_t AMSCanvas::fgAMSFileTOF[] = {
   { kAction, "Occupancies Distibutions ",  TOFSet0CB, NULL },
   { kAction, "Mean Amplitudes Distributions ",  TOFSet1CB, NULL },
   { kAction, "NTof/Amplitud Distributions ",  TOFSet2CB, NULL },
   { kEnd },
};

MenuDesc_t AMSCanvas::fgAMSFileLVL1[] = {
   { kAction, "Occupancies Distibutions", LVL1Set0CB, NULL },
   { kAction, "LVL1 vs TOF",  LVL1Set1CB, NULL },
   { kAction, "Trigger Pattern",  LVL1Set2CB, NULL },
   { kEnd },
};

MenuDesc_t AMSCanvas::fgAMSFileLVL3[] = {
   { kAction, "LVL3 Distributions", LVL3Set0CB, NULL },
   { kAction, "LVL3/AxAMS Comparision",  LVL3Set1CB, NULL },
   { kEnd },
};

MenuDesc_t AMSCanvas::fgAMSFileCTC[] = {
   { kAction, "Layer 1 Distributions", CTCSet0CB, NULL },
   { kAction, "Layer 2 Distributions",  CTCSet1CB, NULL },
   { kEnd },
};

MenuDesc_t AMSCanvas::fgAMSFileAxAMS[] = {
   { kAction, "AxAMS Set 0", AxAMSSet0CB, NULL },
   { kAction, "AxAMS Set 1",  AxAMSSet1CB, NULL },
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
  menu->AddSubmenu("Save As", fgAMSFilePane, NULL);
  menu->AddSubmenu("ANTI", fgAMSFileANTI, NULL);
  menu->AddSubmenu("Tracker", fgAMSFileTracker, NULL);
  menu->AddSubmenu("LVL1", fgAMSFileLVL1, NULL);
  menu->AddSubmenu("LVL3", fgAMSFileLVL3, NULL);
  menu->AddSubmenu("TOF", fgAMSFileTOF, NULL);
  menu->AddSubmenu("CTC", fgAMSFileCTC, NULL);
  menu->AddSubmenu("AxAMS", fgAMSFileAxAMS, NULL);
  TMenuItem * classesMenu = menu->RemoveItem("Classes");
  //  TMenuItem * inspectorMenu = menu->RemoveItem("Inspector");
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
//          Update canvas pad buffers


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

void AMSCanvas::DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected)
{
//*-*-*-*-*-*-*Report name and title of primitive below the cursor*-*-*-*-*-*
//*-*          ===================================================
//
//    This function is called when the option "Event Status"
//    in the canvas menu "Options" is selected.
//

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
void AMSCanvas::TOFSet0CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(4,0);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::TOFSet1CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(4,1);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::TOFSet2CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(4,2);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}

void AMSCanvas::CTCSet0CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(5,0);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::CTCSet1CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(5,1);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}

void AMSCanvas::AxAMSSet0CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(6,0);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::AxAMSSet1CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(6,1);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}

void AMSCanvas::LVL3Set0CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(3,0);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::LVL3Set1CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(3,1);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}

void AMSCanvas::LVL1Set0CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(2,0);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::LVL1Set1CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(2,1);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::LVL1Set2CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(2,2);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::AntiSet0CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(0,0);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::AntiSet1CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(0,1);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::AntiSet2CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(0,2);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::AntiSet3CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(0,3);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::AntiSet4CB(Widget wid, XtPointer cd, XtPointer pointer)
{
   gAMSDisplay->Dispatch(0,4);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}


//______________________________________________________________________
void AMSCanvas::SaveParticleCB(Widget wid, XtPointer cd, XtPointer pointer)
{
   char fnam[255];
   sprintf(fnam, "%s.%d.ps",gAMSDisplay->getCurSubDet()->GetName(),gAMSDisplay->getCurSubDet()->getCSet());
   gAMSDisplay->GetCanvas()->SaveAs(fnam);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::SaveParticleGIF(Widget wid, XtPointer cd, XtPointer pointer)
{
   char fnam[255];
   sprintf(fnam, "%s.%d.gif",gAMSDisplay->getCurSubDet()->GetName(),gAMSDisplay->getCurSubDet()->getCSet());
   gAMSDisplay->GetCanvas()->SaveAs(fnam);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}


//______________________________________________________________________
void AMSCanvas::OpenFileCB(Widget wid, XtPointer cd, XtPointer pointer)
{



}


//______________________________________________________________________
void AMSCanvas::PrintCB(Widget wid, XtPointer cd, XtPointer pointer)
{
 
   pid_t pid = getpid();
   char filename[80];
   sprintf(filename, "/tmp/AMSOnDisplay.%u.ps",pid);
   gAMSDisplay->GetCanvas()->SaveAs(filename);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
   char cmd[255];
   sprintf(cmd, "lpr /tmp/AMSOnDisplay.%u.ps",pid);
   system(cmd);
   sprintf(cmd, "rm /tmp/AMSOnDisplay.%u.ps",pid);
   system(cmd);
}


//______________________________________________________________________

