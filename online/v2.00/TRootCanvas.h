//  $Id: TRootCanvas.h,v 1.2 2001/01/22 17:32:53 choutko Exp $

// Copyright (C) 1998 CodeCERN. All rights reserved.

#ifndef ROOT_TRootCanvas
#define ROOT_TRootCanvas

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TRootCanvas                                                          //
//                                                                      //
// This class creates a main window with menubar, scrollbars and a      //
// drawing area. The widgets used are the new native ROOT GUI widgets.  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TCanvas.h>
#include <TGFrame.h>

class TCanvasImp;
class TGCanvas;
class TGMenuBar;
class TGPopupMenu;
class TGLayoutHints;
class TGStatusBar;


class TRootCanvas : public TGMainFrame, public TCanvasImp {

friend class TRootContainer;

private:
   TGCanvas            *fCanvasWindow;       // canvas widget
   TRootContainer      *fCanvasContainer;    // container in canvas widget
   TGMenuBar           *fMenuBar;            // menubar
   TGPopupMenu         *fFileMenu;           // file menu
   TGPopupMenu         *fEditMenu;           // edit menu
   TGPopupMenu         *fViewMenu;           // view menu
   TGPopupMenu         *fOptionMenu;         // option menu
   TGPopupMenu         *fInspectMenu;        // inspect menu
   TGPopupMenu         *fClassesMenu;        // classes menu
   TGPopupMenu         *fHelpMenu;           // help menu
   TGPopupMenu         *fSubDetMenu[7];      //sub det
   TGPopupMenu         *fSubDetCascadeMenu[7];
   TGPopupMenu         *fSaveAsMenu;        //  saveas
   TGLayoutHints       *fMenuBarLayout;      // menubar layout hints
   TGLayoutHints       *fMenuBarItemLayout;  // layout hints for menu in menubar
   TGLayoutHints       *fMenuBarHelpLayout;  // layout hint for help menu in menubar
   TGLayoutHints       *fCanvasLayout;       // layout for canvas widget
   TGStatusBar         *fStatusBar;          // statusbar widget
   TGLayoutHints       *fStatusBarLayout;    // layout hints for statusbar
   //TGToolBar           *fToolBar;            // icon button toolbar

   Bool_t               fAutoFit;    // when true canvas container keeps same size as canvas
   UInt_t               fCwidth;     // width of canvas container
   UInt_t               fCheight;    // height of canvas
   Int_t                fButton;     // currently pressed button

   void     CreateCanvas(const char *name);

   Bool_t   HandleContainerButton(Event_t *ev);
   Bool_t   HandleContainerConfigure(Event_t *ev);
   Bool_t   HandleContainerKey(Event_t *ev);
   Bool_t   HandleContainerMotion(Event_t *ev);
   Bool_t   HandleContainerExpose(Event_t *ev);

public:
   TRootCanvas(TCanvas *c, const char *name, UInt_t width, UInt_t height);
   TRootCanvas(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height);
   virtual ~TRootCanvas();

   void     ForceUpdate() { Layout(); }
   void     FitCanvas();
   UInt_t   GetCwidth() const;
   UInt_t   GetCheight() const;
   void     Iconify() { }
   Int_t    InitWindow();
   void     SetCanvas(UInt_t w, UInt_t h);
   void     SetCanvasTitle(const Text_t *newTitle);
   void     SetStatusText(Text_t *txt = 0, Int_t partidx = 0);
   void     ShowMenuBar(Bool_t show = kTRUE);
   void     ShowStatusBar(Bool_t show = kTRUE);
   void     Show() { MapRaised(); }

   // overridden from TGMainFrame
   void     CloseWindow();
   Bool_t   ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

   ClassDef(TRootCanvas,0)  //ROOT native GUI version of main window with menubar and drawing area
};

#endif
