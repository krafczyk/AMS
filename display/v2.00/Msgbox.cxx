//  $Id: Msgbox.cxx,v 1.4 2001/01/22 17:32:41 choutko Exp $
#include "Msgbox.h"
#include "AMSRoot.h"

ClassImp(MsgBox)


MsgBox::MsgBox(const TGWindow *p, const TGWindow *main,
                       UInt_t w, UInt_t h, UInt_t options) :
     TGTransientFrame(p, main, w, h, options)
{

  //cout <<"ok"<<endl;
  fLtitle=0;
  fL1=0;
  fL2=0;
  f2=0;
  
     fMsg   = new TGTextEntry(this, fTbmsg = new TGTextBuffer(100),1);
   fMsg->Associate(this);
   //fTbmsg->AddText(0, "MsgBox");  
   fMsg->Resize(300, fMsg->GetDefaultHeight());
   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft,10, 2, 2, 2);
   fL2 =  new TGLayoutHints(kLHintsRight | kLHintsCenterY,
                           0, 2, 0, 0);
   AddFrame(fMsg,fL2);
   MapSubwindows();
   Resize(GetDefaultSize());

   // position relative to the parent's window
   Window_t wdum;
     int ax, ay;
     gGXW->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
     (((TGFrame *) main)->GetWidth()-fWidth ) >> 1,
     (((TGFrame *) main)->GetHeight() - fHeight) >> 1,
     //(((TGFrame *) main)->GetWidth() + fWidth) >> 1,
     //(((TGFrame *) main)->GetHeight() + fHeight) >> 1,
                          ax, ay, wdum);
   Move(ax, ay);

   SetWindowName("Select Event");

   MapWindow();
   fClient->WaitFor(this);
}

// Order is important when deleting frames. Delete children first,
// parents last.

MsgBox::~MsgBox()
{
   // Delete widgets created by dialog.
  delete fLtitle;
  delete fL1; 
  delete fL2; 
  delete fMsg;
  delete f2; 
}

void MsgBox::CloseWindow()
{
   // Close dialog in response to window manager close.

   delete this;
}

Bool_t MsgBox::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Process messages sent to this dialog.


   switch(GET_MSG(msg)) {
      case kC_TEXTENTRY:
         switch(GET_SUBMSG(msg)) {
            case kTE_TEXTCHANGED:
               switch(parm1) {
                  case 1:
                    if(parm2==10){
                     if(gAMSRoot->SetSelectedEvent(fTbmsg->GetString()))
                     CloseWindow();
                     else{
                       //                       cout <<"bad evt"<<endl;
                     }
                    }
                  break;
               }
         }
   }

   return kTRUE;
}
