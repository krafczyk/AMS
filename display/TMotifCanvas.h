//  $Id: TMotifCanvas.h,v 1.2 2001/01/22 17:32:33 choutko Exp $

// Copyright (C) 1995 CodeCERN. All rights reserved.

#ifndef ROOT_TMotifCanvas
#define ROOT_TMotifCanvas

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TMotifCanvas                                                         //
//                                                                      //
// This class creates a main window with menubar, scrollbars and a      //
// drawing area.                                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TCanvasImp
//*KEEP,TCanvasImp.
#include "TCanvasImp.h"
//*KEND.
#endif
#ifndef ROOT_TMenuWindow
//*KEEP,TMenuWindow.
#include "TMenuWindow.h"
//*KEND.
#endif


class TObjLink;
class TContextMenu;

class TMotifCanvas : public TCanvasImp, public TMenuWindow {

protected:

   Widget     fScrolledWindow;         // Scrolled window containing canvas
   Widget     fDrawingArea;            // Canvas widget
   UInt_t     fCwidth;                 // Width of canvas in fUnits units
   UInt_t     fCheight;                // Height of canvas in fUnits units
   EUnits     fUnits;                  // Canvas dimension units
   Int_t      fButton;                 // Currently pressed button
   Bool_t     fAutoFit;                // if TRUE automatically fit canvas

protected:

   TMotifCanvas();  // used by Dictionary
   virtual void AfterRealizeHook();
   virtual void CreateWindow();

   // window action routine
   virtual void HandleWMDeleteMessage();
   virtual void HandleWMQuitMessage();
   virtual void ResizeWindow(XConfigureEvent *ev);
   static void ResizeWindowCB(Widget w, XConfigureEvent *ev, char **, int *);

   // drawing area callbacks
   virtual void ExposeOrResize(XtPointer);
   virtual void HandleInput(XtPointer);
   virtual void HandleMove(XEvent *);
   virtual void HandleEnter();
   static void ExposeOrResizeCB(Widget, XtPointer, XtPointer);
   static void HandleInputCB(Widget, XtPointer, XtPointer);
   static void HandleMoveCB(Widget, XtPointer, XEvent *);
   static void HandleEnterCB(Widget, XtPointer, XEvent *);

   // menubar callbacks
   virtual void NewCanvas();
   // void Open();
   // void Save();
   // void SaveAs();
   // void Print();

   // void Editor();
   // void Cut();
   // void Paste();

   // void Manage();
   // void Iconify();

   // void Refresh();

   virtual void RootExec(const char *cmd);

   static void NewCB(Widget, XtPointer, XtPointer);
   static void OpenCB(Widget, XtPointer, XtPointer);
   static void SaveROOTCB(Widget, XtPointer, XtPointer);
   static void SaveSourceCB(Widget, XtPointer, XtPointer);
   static void SaveCB(Widget, XtPointer, XtPointer);
   static void SaveAsCB(Widget, XtPointer, XtPointer);
   static void SaveGIFCB(Widget, XtPointer, XtPointer);
   static void PrintCB(Widget, XtPointer, XtPointer);
   static void CloseCB(Widget, XtPointer, XtPointer);
   static void QuitCB(Widget, XtPointer, XtPointer);

   static void EditorCB(Widget, XtPointer, XtPointer);
   static void UndoCB(Widget, XtPointer, XtPointer);
   static void ClearCanvasCB(Widget, XtPointer, XtPointer);
   static void ClearPadCB(Widget, XtPointer, XtPointer);

   static void ColorsCB(Widget, XtPointer, XtPointer);
   static void FontsCB(Widget, XtPointer, XtPointer);
   static void MarkersCB(Widget, XtPointer, XtPointer);
   static void IconifyCB(Widget, XtPointer, XtPointer);
   static void X3DViewCB(Widget, XtPointer, XtPointer);
   static void OpenGLViewCB(Widget, XtPointer, XtPointer);

   static void EventStatusCB(Widget, XtPointer, XtPointer);
   static void AutoFitCanvasCB(Widget, XtPointer, XtPointer);
   static void FitCanvasCB(Widget, XtPointer, XtPointer);
   static void RefreshCB(Widget, XtPointer, XtPointer);
   static void OptStatCB(Widget, XtPointer, XtPointer);
   static void OptTitleCB(Widget, XtPointer, XtPointer);
   static void OptFitCB(Widget, XtPointer, XtPointer);
   static void CanEditHistogramsCB(Widget, XtPointer, XtPointer);

   static void ROOTInspectCB(Widget, XtPointer, XtPointer);
   static void BrowserCB(Widget, XtPointer, XtPointer);

   static void FullTreeCB(Widget, XtPointer, XtPointer);
   static void PartialTreeCB(Widget, XtPointer, XtPointer);

   static void HelpAboutCB(Widget, XtPointer, XtPointer);
   static void HelpCanvasCB(Widget, XtPointer, XtPointer);
   static void HelpPullDownMenusCB(Widget, XtPointer, XtPointer);
   static void HelpGraphicsEditorCB(Widget, XtPointer, XtPointer);
   static void HelpBrowserCB(Widget, XtPointer, XtPointer);
   static void HelpObjectsCB(Widget, XtPointer, XtPointer);
   static void HelpPostscriptCB(Widget, XtPointer, XtPointer);

   // the menubar stuff
   static MenuDesc_t fgMenuPane[];
   static MenuDesc_t fgFilePane[];
   static MenuDesc_t fgEditPane[];
   static MenuDesc_t fgViewPane[];
   static MenuDesc_t fgOptionPane[];
   static MenuDesc_t fgInspectorPane[];
   static MenuDesc_t fgClassesPane[];
   static MenuDesc_t fgHelpPane[];

   static String fgDefaultResources[];

public:

   TMotifCanvas(TCanvas *c, const char *name, UInt_t width, UInt_t height);
   TMotifCanvas(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height);
   ~TMotifCanvas();

   const char         *ClassName() const { return "TMotifCanvas"; }
   void                ForceUpdate() { TMenuWindow::Update(); }
   void                FitCanvas();
   UInt_t              GetCwidth() const;               // Units in pixels
   UInt_t              GetCheight() const;              // Units in pixels
   void                Iconify() { TMenuWindow::Iconify(); }
   Int_t               InitWindow();
   void                ResizeCanvas();
   void                SetCanvas(UInt_t w, UInt_t h);
   void                SetCanvasTitle(const Text_t *newTitle);
   void                ShowMenuBar(Bool_t show = kTRUE);
   void                Show() { TMenuWindow::Show(); }

#ifdef MotifDict
   ClassDef(TMotifCanvas,0)  //Motif version of main window with menubar and drawing area
#endif
};

inline void TMotifCanvas::SetCanvasTitle(const Text_t *newTitle)
{ TBaseWindow::SetTitle(newTitle); TBaseWindow::SetIconName(newTitle); }

#endif
