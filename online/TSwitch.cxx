//  $Id: TSwitch.cxx,v 1.3 2003/06/17 07:39:55 choutko Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Display Switch class                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TCanvas.h>
#include <TMath.h>

#include "TSwitch.h"

ClassImp(TSwitch)


//_____________________________________________________________________________
TSwitch::TSwitch(const char * title, Bool_t *state, char * method,
			     Coord_t x1, Coord_t y1, Coord_t x2, Coord_t y2,
				char * offMethod)
  : TPad(title, "", x1, y1, x2, y2)	// method is defined in ExecuteEvent
{

   TVirtualPad * gPadSave = gPad;
   this->Draw();
   this->cd();

// Coord_t ycenter = (y1+y2)/2,
//  butsize = (y2-y1)/3,
//  xcenter = 0.95*x1+0.05*x2;
   Coord_t buty1 = 0.2,
	   buty2 = 0.8,
	   butx1 = 0.1,
	   butx2;

   Int_t ylow  = this->YtoAbsPixel(buty1);
   Int_t yhi   = this->YtoAbsPixel(buty2);
   Int_t xlow  = this->XtoAbsPixel(butx1);
   Int_t bsize = TMath::Abs(yhi-ylow);
   Int_t xhi   = xlow + bsize;
         butx2 = this->AbsPixeltoX(xhi);

 
   fButton = new TRadioButton(state, method, butx1, buty1, butx2, buty2, offMethod);
   fPrimitives->Add(fButton);
// fButton->Draw();

   fText   = new TText(butx2+0.5*(butx2-butx1), 0.5, title);
// fText->SetTextSize((y2-y1)*0.8);
   fText->SetTextAlign(12);
   fText->SetTextAngle(0);
   fText->SetTextColor(1);
   fText->SetTextFont(61);
   fText->SetTextSize(0.65);
   fPrimitives->Add(fText);
   Short_t align = fText->GetTextAlign();
   Float_t angle = fText->GetTextAngle();
   Color_t color = fText->GetTextColor();
   Font_t  font  = fText->GetTextFont();
   Float_t size  = fText->GetTextSize();

   SetBorderMode(1);
   this->Modified();

   gPadSave->cd();
}



//_____________________________________________________________________________
void TSwitch::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
  
  //
  // one can move, resize, ... etc  to this TSwitch if and only if
  // the canvas is editable.
  //
  if (fMother->GetCanvas()->IsEditable()) {
     TPad::ExecuteEvent(event,px,py);
     return;
  }

  // do nothing to inhibit resizing, moving, ... etc.
  return;
}
