
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSCanvas                                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <unistd.h>		// for getpid()
#include <iostream.h>
#include <stdlib.h>
#include <TROOT.h>
#include <TControlBar.h>
#include <TContextMenu.h>


#include <TText.h>

#include "AMSDisplay.h"
#include "AMSCanvas.h"
#include "Debugger.h"



TRootCanvas * AMSCanvas::fTheCanvas = 0;


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
    fTheCanvas = (TRootCanvas *) fCanvasImp;
    printf("fTheCanvas = %lx in AMSCanvas::AMSCanvas()\n", fTheCanvas);
  }



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

void AMSCanvas::SubDet(int det, int set){
   gAMSDisplay->Dispatch(det,set);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}




//______________________________________________________________________
void AMSCanvas::SaveParticleCB()
{
   char fnam[255];
   sprintf(fnam, "%s.%s.ps",gAMSDisplay->getCurSubDet()->GetName(),gAMSDisplay->getGrSet());
   gAMSDisplay->GetCanvas()->SaveAs(fnam);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}
void AMSCanvas::SaveParticleGIF()
{
   char fnam[255];
   sprintf(fnam, "%s.%s.gif",gAMSDisplay->getCurSubDet()->GetName(),gAMSDisplay->getGrSet());
   gAMSDisplay->GetCanvas()->SaveAs(fnam);
   gAMSDisplay->GetCanvas()->Update();		// refresh the screen
}


void AMSCanvas::PrintCB()
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

