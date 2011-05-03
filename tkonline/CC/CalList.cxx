
#include "CalList.h"

#include "TGButton.h"
#include "TGLabel.h"
#include "TGTextEntry.h"
#include "TGListBox.h"
#include "timeid.h"
#include "TrCalDB.h"
#include "amsdbc.h"

ClassImp(CalList);


CalList::CalList(AMSTimeID * TT,
		 const TGWindow *p,const TGWindow* main, 
		 UInt_t w, UInt_t h,UInt_t options):
  TGTransientFrame(p, main, 10, 10, options)
{
  if(!TT ||(!p&&!main)){
    MakeZombie();
    return;
  }
  SetCleanup(kDeepCleanup);
  timeid=TT;
  nn=0;
  
  // Create list  frame
  num=timeid->GetDataBaseSize();
  int first=0,last=num;
  char tmp[50];
  fListBox= new TGListBox(this,num);
  for (int ii=0; ii< num;ii++){
    uinteger times[3];
    time_t tim=timeid->GetDBRecordTimes(num-(ii+1),times);
    sprintf(tmp,"%d %s",tim,ctime(&tim));
    fListBox->AddEntry(tmp,ii);
  }
  fSelected = new TList;
  fListBox->Resize(100,150);
  AddFrame(fListBox, new TGLayoutHints(kLHintsTop | kLHintsLeft |
				       kLHintsExpandX | kLHintsExpandY, 
				       5, 5, 5, 5));

   // create frame and layout hints for Ok and Cancel buttons
   TGHorizontalFrame *hf = new TGHorizontalFrame(this, 60, 20, kFixedWidth);
   hf->SetCleanup(kDeepCleanup);

   // create OK and Cancel buttons in their own frame (hf)
   UInt_t  width = 0, height = 0;

   fOk = new TGTextButton(hf, "&Ok", 1);
   fOk->Associate(this);
   hf->AddFrame(fOk, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 5, 5, 0, 0));
   height = fOk->GetDefaultHeight();
   width  = TMath::Max(width, fOk->GetDefaultWidth());

   //   fCancel = new TGTextButton(hf, "&Cancel", 2);
   //   fCancel->Associate(this);
   //   hf->AddFrame(fCancel, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 5, 5, 0, 0));
   //   height = fCancel->GetDefaultHeight();
   //   width  = TMath::Max(width, fCancel->GetDefaultWidth());

   // place button frame (hf) at the bottom
   AddFrame(hf, new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 0, 0, 5, 5));

   // keep buttons centered and with the same width
   hf->Resize((width + 20) * 2, height);


   // Set a name to the main frame   
   SetWindowName("Tracker Calibration DB");
    MapSubwindows();

   width  = GetDefaultWidth();
   height = GetDefaultHeight();

   Resize(300, 400);

   // position relative to the parent's window
   CenterOnParent();

   // Map main frame
   MapWindow();
   fListBox->Select(1);
   gClient->WaitFor(this);
}

CalList::~CalList()
{
   // Clean up main frame...
   timeid=0;
   Cleanup();
   if (fSelected) {
      fSelected->Delete();
      delete fSelected;
   }
}

//______________________________________________________________________________
Bool_t CalList::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle button and text enter events
  uinteger times[3];
  time_t tim=0;
  
   switch (GET_MSG(msg)) {
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {
            case kCM_BUTTON:
               switch (parm1) {
                  case 1:
                     // here copy the string from text buffer to return variable
		    //                     strcpy(fRetStr, fTE->GetBuffer()->GetString());
		     nn= fListBox->GetSelected();
		     tim=timeid->GetDBRecordTimes(num-(nn+1),times);
		     timeid->validate(tim);
                     delete this;
                     break;
                  case 2:
		    nn= 1;
		     tim=timeid->GetDBRecordTimes(num-(nn+1),times);
		     timeid->validate(tim);
                     delete this;
                     break;
               }
               default:
                  break;
         }
         break;

      case kC_TEXTENTRY:
         switch (GET_SUBMSG(msg)) {
            case kTE_ENTER:
               // here copy the string from text buffer to return variable
	      //               strcpy(fRetStr, fTE->GetBuffer()->GetString());
               delete this;
               break;
            default:
               break;
         }
         break;

      default:
         break;
   }
   return kTRUE;
}


