//  $Id: TCanvasImp.h,v 1.2 2001/01/22 17:32:53 choutko Exp $

// Copyright (C) 1995 CodeCERN. All rights reserved.

#ifndef ROOT_TCanvasImp
#define ROOT_TCanvasImp

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCanvasImp                                                           //
//                                                                      //
// ABC describing GUI independent main window (with menubar, scrollbars //
// and a drawing area).                                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_Rtypes
//*KEEP,Rtypes.
#include "Rtypes.h"
//*KEND.
#endif

class TCanvas;


class TCanvasImp {

protected:
   TCanvas  *fCanvas;   //TCanvas associated with this implementation

public:
   TCanvasImp(TCanvas *c=0) : fCanvas(c) { }
   TCanvasImp(TCanvas *c, const char *name, UInt_t width, UInt_t height) { }
   TCanvasImp(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height) { }
   virtual ~TCanvasImp() { }

   TCanvas       *Canvas() const { return fCanvas; }
   virtual void   SetStatusText(Text_t *text = 0, Int_t partidx = 0) { }
   virtual void   ForceUpdate() { }
   virtual void   Iconify() { }
   virtual Int_t  InitWindow() { return 0; }
   virtual void   SetCanvas(UInt_t w, UInt_t h) { }
   virtual void   SetCanvasTitle(const Text_t *newTitle) { }
   virtual void   ShowMenuBar(Bool_t show = kTRUE) { }
   virtual void   Show() { }

   ClassDef(TCanvasImp,0)  //ABC describing main window protocol
};

#endif
