//*CMZ :  2.00/00 06/03/98  17.28.43  by  Fons Rademakers
//*-- Author :    Fons Rademakers   15/01/98

//*KEEP,CopyRight,T=C.
/*************************************************************************
 * Copyright(c) 1995-1998, The ROOT System, All rights reserved.         *
 * Authors: Rene Brun, Nenad Buncic, Valery Fine, Fons Rademakers.       *
 *                                                                       *
 * Permission to use, copy, modify and distribute this software and its  *
 * documentation for non-commercial purposes is hereby granted without   *
 * fee, provided that the above copyright notice appears in all copies   *
 * and that both the copyright notice and this permission notice appear  *
 * in the supporting documentation. The authors make no claims about the *
 * suitability of this software for any purpose.                         *
 * It is provided "as is" without express or implied warranty.           *
 *************************************************************************/
//*KEND.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TRootCanvas                                                          //
//                                                                      //
// This class creates a main window with menubar, scrollbars and a      //
// drawing area. The widgets used are the new native ROOT GUI widgets.  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TRootApplication.h>
#include <TRootHelpDialog.h>
#include <TGClient.h>
#include <TGCanvas.h>
#include <TGMenu.h>
#include <TGWidget.h>
#include <TGFileDialog.h>
#include <TGStatusBar.h>
#include <TCanvas.h>
#include <TRootCanvas.h>
#include "AMSCanvas.h"

//*KEND.

#include <TROOT.h>
#include <TBrowser.h>
#include <TMarker.h>
#include <TStyle.h>
#include <TGXW.h>
#include <TApplication.h>
#include <TFile.h>
#include <HelpText.h>

// Canvas menu command ids
enum ERootCanvasCommands {
   kFileNewCanvas,
   kFileOpen,
   kFileSaveAs,
   kFileSaveAsRoot,
   kFileSaveAsC,
   kFileSaveAsPS,
   kFileSaveAsEPS,
   kFileSaveAsGIF,
   kFilePrint,
   kFileCloseCanvas,
   kFileQuit,

   kEditEditor,
   kEditUndo,
   kEditClearPad,
   kEditClearCanvas,

   kViewColors,
   kViewFonts,
   kViewMarkers,
   kViewIconify,
   kViewX3D,
   kViewOpenGL,

   kOptionEventStatus,
   kOptionAutoResize,
   kOptionResizeCanvas,
   kOptionRefresh,
   kOptionStatistics,
   kOptionHistTitle,
   kOptionFitParams,
   kOptionCanEdit,

   kInspectRoot,
   kInspectBrowser,

   kClassesFullTree,
   kClassesStartAt,
 
   
   kHelpAbout,
   kHelpOnCanvas,
   kHelpOnMenus,
   kHelpOnGraphicsEd,
   kHelpOnBrowser,
   kHelpOnObjects,
   kHelpOnPS,

   kSaveAsps,
   kSaveAsgif,
   kSaveAsprint
};

static const char *gOpenTypes[] = { "ROOT files",   "*.root",
                                    "All files",    "*",
                                    0,              0 };

static const char *gSaveAsTypes[] = { "PostScript",   "*.ps",
                                      "Encapsulated PostScript", "*.eps",
                                      "Gif files",    "*.gif",
                                      "ROOT files",   "*.root",
                                      "All files",    "*",
                                      0,              0 };


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TRootContainer                                                       //
//                                                                      //
// Utility class used by TRootCanvas. The TRootContainer is the frame   //
// embedded in the TGCanvas widget. The ROOT graphics goes into this    //
// frame. This class is used to enable input events on this graphics    //
// frame and forward the events to the TRootCanvas handlers.            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TRootContainer : public TGCompositeFrame {
private:
   TRootCanvas  *fCanvas;    // pointer back to canvas imp
public:
   TRootContainer(const TGWindow *p, TRootCanvas *c,
                  UInt_t w, UInt_t h, UInt_t options = 0,
                  ULong_t back = fgWhitePixel);

   Bool_t  HandleButton(Event_t *ev)
                { return fCanvas->HandleContainerButton(ev); }
   Bool_t  HandleConfigureNotify(Event_t *ev)
                { TGFrame::HandleConfigureNotify(ev);
                  return fCanvas->HandleContainerConfigure(ev); }
   Bool_t  HandleKey(Event_t *ev)
                { return fCanvas->HandleContainerKey(ev); }
   Bool_t  HandleMotion(Event_t *ev)
                { return fCanvas->HandleContainerMotion(ev); }
   Bool_t  HandleExpose(Event_t *ev)
                { TGWindow::HandleExpose(ev);
                  return fCanvas->HandleContainerExpose(ev); }
};

//______________________________________________________________________________
TRootContainer::TRootContainer(const TGWindow *p, TRootCanvas *c,
        UInt_t w, UInt_t h, UInt_t options, ULong_t back)
      : TGCompositeFrame(p, w, h, options, back)
{
   // Create a canvas container.

   fCanvas = c;

   gGXW->GrabButton(fId, kAnyButton, kAnyModifier,
                    kButtonPressMask | kButtonReleaseMask,
                    kNone, kNone);

   gGXW->SelectInput(fId, kKeyPressMask | kExposureMask | kPointerMotionMask |
                     kStructureNotifyMask);
}




ClassImp(TRootCanvas)

//______________________________________________________________________________
TRootCanvas::TRootCanvas(TCanvas *c, const char *name, UInt_t width, UInt_t height)
   : TGMainFrame(gClient->GetRoot(), width, height), TCanvasImp(c)
{
   // Create a basic ROOT canvas.

   CreateCanvas(name);

   Resize(width, height);
}

//______________________________________________________________________________
TRootCanvas::TRootCanvas(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height)
   : TGMainFrame(gClient->GetRoot(), width, height), TCanvasImp(c)
{
   // Create a basic ROOT canvas.

   CreateCanvas(name);

   MoveResize(x, y, width, height);
   SetWMPosition(x, y);
}

//______________________________________________________________________________
void TRootCanvas::CreateCanvas(const char *name)
{
  char sSubDetMenu[8][]={ "&AntiCounters",
                          "T&racker",
                          "Level&1",
                          "Level&3",
                          "&TOF",
                          "&CTC",
                          "A&xAMS",
                          "General"};
   static TGPopupMenu         *fSubDetMenu[8];      //sub det
   static TGPopupMenu         *fSubDetCascadeMenu[8];
   static TGPopupMenu         *fSaveAsMenu;        //  saveas

   // Create the actual canvas.
   int i;
   for(i=0;i<8;i++)fSubDetMenu[i]=0;
   for( i=0;i<8;i++)fSubDetCascadeMenu[i]=0;
   fButton  = 0;
   fCwidth  = 0;
   fCheight = 0;
   fAutoFit = kTRUE;   // check also menu entry

   // Create menus
   fFileMenu = new TGPopupMenu(fClient->GetRoot());
   fFileMenu->AddEntry("&New Canvas",         kFileNewCanvas);
   fFileMenu->AddEntry("&Open...",            kFileOpen);
   //   fFileMenu->AddSeparator();
   //   fFileMenu->AddEntry("Save As...",          kFileSaveAs);
   //   fFileMenu->AddEntry("Save As canvas.root", kFileSaveAsRoot);
   //   fFileMenu->AddEntry("Save As canvas.C",    kFileSaveAsC);
   //   fFileMenu->AddEntry("Save As canvas.ps",   kFileSaveAsPS);
   //   fFileMenu->AddEntry("Save As canvas.eps",  kFileSaveAsEPS);
   //   fFileMenu->AddEntry("Save As canvas.gif",  kFileSaveAsGIF);
   //   fFileMenu->AddSeparator();
   //   fFileMenu->AddEntry("&Print...",           kFilePrint);
   //   fFileMenu->AddSeparator();
   //   fFileMenu->AddEntry("&Close Canvas",       kFileCloseCanvas);
   fFileMenu->AddSeparator();
   fFileMenu->AddEntry("&Quit ROOT",          kFileQuit);

   //fFileMenu->DefaultEntry(kFileNewCanvas);
   //fFileMenu->DisableEntry(kFileOpen);

   fEditMenu = new TGPopupMenu(fClient->GetRoot());
   fEditMenu->AddEntry("&Editor",             kEditEditor);
   fEditMenu->AddEntry("&Undo",               kEditUndo);
   fEditMenu->AddEntry("Clear &Pad",          kEditClearPad);
   fEditMenu->AddEntry("&Clear Canvas",       kEditClearCanvas);

   fEditMenu->DisableEntry(kEditUndo);

   fViewMenu = new TGPopupMenu(fClient->GetRoot());
   fViewMenu->AddEntry("&Colors",             kViewColors);
   fViewMenu->AddEntry("&Fonts",              kViewFonts);
   fViewMenu->AddEntry("&Markers",            kViewMarkers);
   fViewMenu->AddEntry("&Iconify",            kViewIconify);
   fViewMenu->AddEntry("&View with X3D",      kViewX3D);
   fViewMenu->AddEntry("View with &OpenGL",   kViewOpenGL);

   fOptionMenu = new TGPopupMenu(fClient->GetRoot());
   fOptionMenu->AddEntry("&Event Status",       kOptionEventStatus);
   fOptionMenu->AddEntry("&Auto Resize Canvas", kOptionAutoResize);
   fOptionMenu->AddEntry("&Resize Canvas",      kOptionResizeCanvas);
   fOptionMenu->AddEntry("R&efresh",            kOptionRefresh);
   fOptionMenu->AddEntry("&Statistics",         kOptionStatistics);
   fOptionMenu->AddEntry("&Histogram Title",    kOptionHistTitle);
   fOptionMenu->AddEntry("Show &Fit Params",    kOptionFitParams);
   //   fOptionMenu->AddEntry("Can Edit Histograms", kOptionCanEdit);

   fOptionMenu->CheckEntry(kOptionAutoResize);
   fOptionMenu->CheckEntry(kOptionStatistics);
   fOptionMenu->CheckEntry(kOptionHistTitle);

   fInspectMenu = new TGPopupMenu(fClient->GetRoot());
   //   fInspectMenu->AddEntry("&ROOT",              kInspectRoot);
   //   fInspectMenu->AddEntry("&Start Browser",     kInspectBrowser);

   fClassesMenu = new TGPopupMenu(fClient->GetRoot());
   //   fClassesMenu->AddEntry("&Full Tree",         kClassesFullTree);
   //   fClassesMenu->AddEntry("&Start at...",       kClassesStartAt);
   fSaveAsMenu = new TGPopupMenu(fClient->GetRoot());
   //   fSaveAsMenu->AddEntry("Save As SubDetector.Set.&ps",kSaveAsps);
   //   fSaveAsMenu->AddEntry("Save As SubDetector.Set.&gif",kSaveAsgif);
   //   fSaveAsMenu->AddSeparator();
   //   fSaveAsMenu->AddEntry("P&rint",kSaveAsprint);
   //
   for(i=0;i<7;i++)fSubDetCascadeMenu[i] = new TGPopupMenu(fClient->GetRoot());
   for(i=0;i<7;i++)fSubDetMenu[i] = new TGPopupMenu(fClient->GetRoot());
   //   fSubDetCascadeMenu[0]->AddEntry("1-4",101);
   //   fSubDetCascadeMenu[0]->AddEntry("5-8",102);
   //   fSubDetCascadeMenu[0]->AddEntry("9-12",103);
   //   fSubDetCascadeMenu[0]->AddEntry("13-16",104);
   //  
   //   fSubDetMenu[0]->AddEntry("Occupancies/Amplitudes Distibution",100);
   //   fSubDetMenu[0]->AddPopup( "UpVsDown",fSubDetCascadeMenu[0]);
   //
   //   fSubDetMenu[1]->AddEntry("PreClusters Distributions Set 1",110);
   //   fSubDetMenu[1]->AddEntry("PreClusters Distributions Set 2",111);
   //   fSubDetMenu[1]->AddEntry("Clusters Distribution",112);
   //   fSubDetMenu[1]->AddSeparator();
   //   fSubDetMenu[1]->AddEntry("Calibration Set 1",113);
   //   fSubDetMenu[1]->AddEntry("Calibration Set 2",114);
   //
   //   fSubDetMenu[2]->AddEntry("Occupancies Distibutions",120);
   //   fSubDetMenu[2]->AddEntry("LVL1 vs TOF",121);
   //   fSubDetMenu[2]->AddEntry("TOF Trigger Pattern",122);
   //   fSubDetMenu[2]->AddEntry("LVL1 vs Anti",123);
   //   fSubDetMenu[3]->AddEntry("LVL3 Distributions",130);
   //   fSubDetMenu[3]->AddEntry("LVL3/AxAMS Comparision",131);
   //
   //   fSubDetMenu[4]->AddEntry("JL1/TOF Side Occupancies",140);
   //   fSubDetMenu[4]->AddEntry("JL1/TOF Counter Occupancies",141);
   //   fSubDetMenu[4]->AddEntry("Occupancies Distributions",142);
   //   fSubDetMenu[4]->AddEntry("Tdiffs/(data size)/config",143);
   //   fSubDetMenu[4]->AddEntry("NTof/Eloss Distributions ",144);
   //   fSubDetMenu[4]->AddEntry("Mean Eloss per counter (MeV)",145);
   //   fSubDetMenu[4]->AddEntry("Anode Charge Spectra",146);
   //   fSubDetMenu[4]->AddEntry("Dynode Charge Spectra",147);
   //   fSubDetMenu[4]->AddEntry("Time Stretcher Ratios",148);
   //
   //
   //
   //   fSubDetMenu[5]->AddEntry("Layer 1 Distributions",150);
   //   fSubDetMenu[5]->AddEntry("Layer 2 Distributions",151);
   // 
   //   fSubDetMenu[6]->AddEntry("Set 0",160);
   //   fSubDetMenu[6]->AddEntry("Set 1",161);
   //
   //   fSubDetMenu[7]->AddEntry("Length",170);
   //   fSubDetMenu[7]->AddEntry("TOF Length",171);
   //   fSubDetMenu[7]->AddEntry("TRK (red) Length",172);
   //   fSubDetMenu[7]->AddEntry("TRK (raw,mix) Length",173);
   //

   fHelpMenu = new TGPopupMenu(fClient->GetRoot());
   fHelpMenu->AddEntry("&About ROOT...",        kHelpAbout);
   fHelpMenu->AddSeparator();
   fHelpMenu->AddEntry("Help On Canvas...",     kHelpOnCanvas);
   fHelpMenu->AddEntry("Help On Menus...",      kHelpOnMenus);
   fHelpMenu->AddEntry("Help On Graphics Editor...", kHelpOnGraphicsEd);
   fHelpMenu->AddEntry("Help On Browser...",    kHelpOnBrowser);
   fHelpMenu->AddEntry("Help On Objects...",    kHelpOnObjects);
   fHelpMenu->AddEntry("Help On PostScript...", kHelpOnPS);

   // This main frame will process the menu commands
   fFileMenu->Associate(this);
   fEditMenu->Associate(this);
   fViewMenu->Associate(this);
   fOptionMenu->Associate(this);
   fInspectMenu->Associate(this);
   fClassesMenu->Associate(this);
   fSaveAsMenu->Associate(this);
   for(i=0;i<7;i++)fSubDetMenu[i]->Associate(this);
   for(i=0;i<7;i++)fSubDetCascadeMenu[i]->Associate(this);
   fHelpMenu->Associate(this);
   // Create menubar layout hints
   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);

   // Create menubar
   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File",    fFileMenu,    fMenuBarItemLayout);
   //fMenuBar->AddPopup("&Edit",    fEditMenu,    fMenuBarItemLayout);
   //fMenuBar->AddPopup("&View",    fViewMenu,    fMenuBarItemLayout);
   fMenuBar->AddPopup("&Options", fOptionMenu,  fMenuBarItemLayout);
   //fMenuBar->AddPopup("&Inspect", fInspectMenu, fMenuBarItemLayout);
   //fMenuBar->AddPopup("&Classes", fClassesMenu, fMenuBarItemLayout);
   //fMenuBar->AddPopup("&SaveAs", fSaveAsMenu, fMenuBarItemLayout);
   //for(i=0;i<7;i++)fMenuBar->AddPopup(sSubDetMenu[i], fSubDetMenu[i], fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help",    fHelpMenu,    fMenuBarHelpLayout);

   AddFrame(fMenuBar, fMenuBarLayout);

   // Create canvas and canvas container that will host the ROOT graphics
   fCanvasWindow = new TGCanvas(this, GetWidth()+4, GetHeight()+4,
                                kSunkenFrame | kDoubleBorder);
   fCanvasContainer = new TRootContainer(fCanvasWindow->GetViewPort(), this, 10, 10);
   fCanvasWindow->SetContainer(fCanvasContainer);
   fCanvasLayout = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
   AddFrame(fCanvasWindow, fCanvasLayout);

   // Create status bar
   int parts[] = { 33, 10, 10, 47 };
   fStatusBar = new TGStatusBar(this, 10, 10);
   fStatusBar->SetParts(parts, 4);

   fStatusBarLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 2, 1, 1);

   AddFrame(fStatusBar, fStatusBarLayout);

   // Misc

   SetWindowName(name);
   SetIconName(name);
   SetClassHints("Canvas", "Canvas");

   SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);

   MapSubwindows();

   // by default status bar is hidden
   HideFrame(fStatusBar);

   // we need to use GetDefaultSize() to initialize the layout algorithm...
   Resize(GetDefaultSize());
}

//______________________________________________________________________________
TRootCanvas::~TRootCanvas()
{
   // Delete ROOT basic canvas. Order is significant. Delete in reverse
   // order of creation.

   delete fStatusBar;
   delete fStatusBarLayout;
   delete fCanvasContainer;
   delete fCanvasWindow;
   delete fFileMenu;
   delete fEditMenu;
   delete fViewMenu;
   delete fOptionMenu;
   delete fInspectMenu;
   delete fClassesMenu;
   //delete fSaveAsMenu;
   delete fHelpMenu;
   delete fMenuBar;
   delete fMenuBarLayout;
   delete fMenuBarItemLayout;
   delete fMenuBarHelpLayout;
   delete fCanvasLayout;
   //for(int i=0;i<7;i++)delete fSubDetMenu[i];
   //for(i=0;i<7;i++)delete fSubDetCascadeMenu[i];
}

//______________________________________________________________________________
void TRootCanvas::CloseWindow()
{
   // In case window is closed via WM we get here.
   // Forward message to central message handler as button event.

   SendMessage(this, MK_MSG(kC_COMMAND, kCM_BUTTON), kFileCloseCanvas, 0);
}

//______________________________________________________________________________
UInt_t TRootCanvas::GetCwidth() const
{
   // Return width of canvas container.

   return fCanvasContainer->GetWidth();
}

//______________________________________________________________________________
UInt_t TRootCanvas::GetCheight() const
{
   // Return height of canvas container.

   return fCanvasContainer->GetHeight();
}

//______________________________________________________________________________
void TRootCanvas::SetStatusText(Text_t *txt, Int_t partidx)
{
   // Set text in status bar.

   fStatusBar->SetText(txt, partidx);
}

//______________________________________________________________________________
Bool_t TRootCanvas::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle menu and other command generated by the user.

   TRootHelpDialog *hd;

   switch (GET_MSG(msg)) {

      case kC_COMMAND:

         switch (GET_SUBMSG(msg)) {

            case kCM_BUTTON:
            case kCM_MENU:
              if(parm1/100 ==1){
               int sdet=(parm1/10)%10;
               int set=parm1%10;
               AMSCanvas::SubDet(sdet,set);
               return kTRUE;
              }
               switch (parm1) {
                  // Handle File menu items...
                  case kFileNewCanvas:
                     gROOT->GetMakeDefCanvas()();
                     break;
                  case kFileOpen:
                     {
                        TGFileInfo fi;
                        fi.fFileTypes = (char **) gOpenTypes;
                        new TGFileDialog(fClient->GetRoot(), this, kFDOpen,&fi);
                        if (!fi.fFilename) return kTRUE;
                        new TFile(fi.fFilename, "update");
                        delete [] fi.fFilename;
                     }
                     break;
                  case kFileSaveAs:
                     {
                        TGFileInfo fi;
                        fi.fFileTypes = (char **) gSaveAsTypes;
                        new TGFileDialog(fClient->GetRoot(), this, kFDSave,&fi);
                        if (!fi.fFilename) return kTRUE;
                        if (strstr(fi.fFilename, ".root"))
                           fCanvas->SaveAs(".root");
                        else if (strstr(fi.fFilename, ".C"))
                           fCanvas->SaveSource();
                        else if (strstr(fi.fFilename, ".ps"))
                           fCanvas->SaveAs();
                        else if (strstr(fi.fFilename, ".eps"))
                           fCanvas->SaveAs(".eps");
                        else if (strstr(fi.fFilename, ".gif"))
                           fCanvas->SaveAs(".gif");
                        else
                           Warning("ProcessMessage", "file cannot be save with this extension (%s)", fi.fFilename);
                        delete [] fi.fFilename;
                     }
                     break;
                  case kFileSaveAsRoot:
                     fCanvas->SaveAs(".root");
                     break;
                  case kFileSaveAsC:
                     fCanvas->SaveSource();
                     break;
                  case kFileSaveAsPS:
                     fCanvas->SaveAs();
                     break;
                  case kFileSaveAsEPS:
                     fCanvas->SaveAs(".eps");
                     break;
                  case kFileSaveAsGIF:
                     fCanvas->SaveAs(".gif");
                     break;
                  case kFilePrint:
                     fCanvas->Print();
                     break;
                  case kFileCloseCanvas:
                     delete fCanvas;  // this in turn will delete this object
                     break;
                  case kFileQuit:
                     gApplication->Terminate(0);
                     break;

                  // Handle Edit menu items...
                  case kEditEditor:
                     fCanvas->EditorBar();
                     break;
                  case kEditUndo:
                     // noop
                     break;
                  case kEditClearPad:
                     gPad->Clear();
                     gPad->Modified();
                     gPad->Update();
                     break;
                  case kEditClearCanvas:
                     fCanvas->Clear();
                     fCanvas->Modified();
                     fCanvas->Update();
                     break;

                  // Handle View menu items...
                  case kViewColors:
                     {
                        TVirtualPad *padsav = gPad->GetCanvas();
                        TCanvas *m = new TCanvas("colors","Color Table");
                        gROOT->DisplayColorTable();
                        m->Update();
                        padsav->cd();
                     }
                     break;
                  case kViewFonts:
                     // noop
                     break;
                  case kViewMarkers:
                     {
                        TVirtualPad *padsav = gPad->GetCanvas();
                        TCanvas *m = new TCanvas("markers","Marker Types",600,200);
                        TMarker::DisplayMarkerTypes();
                        m->Update();
                        padsav->cd();
                     }
                     break;
                  case kViewIconify:
                     Iconify();
                     break;
                  case kViewX3D:
                     gPad->x3d();
                     break;
                  case kViewOpenGL:
                     gPad->x3d("OPENGL");
                     break;

                  // Handle Option menu items...
                  case kOptionEventStatus:
                     fCanvas->ToggleEventStatus();
                     if (fCanvas->GetShowEventStatus()) {
                        ShowFrame(fStatusBar);
                        fOptionMenu->CheckEntry(kOptionEventStatus);
                     } else {
                        HideFrame(fStatusBar);
                        fOptionMenu->UnCheckEntry(kOptionEventStatus);
                     }
                     break;
                  case kOptionAutoResize:
                     {
                        fAutoFit = fAutoFit ? kFALSE : kTRUE;
                        int opt = fCanvasContainer->GetOptions();
                        if (fAutoFit) {
                           opt &= ~kFixedSize;
                           fOptionMenu->CheckEntry(kOptionAutoResize);
                        } else {
                           opt |= kFixedSize;
                           fOptionMenu->UnCheckEntry(kOptionAutoResize);
                        }
                        fCanvasContainer->ChangeOptions(opt);
                        // in case of autofit this will generate a configure
                        // event for the container and this will force the
                        // update of the TCanvas
                        Layout();
                     }
                     break;
                  case kOptionResizeCanvas:
                     FitCanvas();
                     break;
                  case kOptionRefresh:
                     fCanvas->Paint();
                     fCanvas->Update();
                     break;
                  case kOptionStatistics:
                     if (gStyle->GetOptStat()) {
                        gStyle->SetOptStat(0);
                        fOptionMenu->UnCheckEntry(kOptionStatistics);
                     } else {
                        gStyle->SetOptStat(1);
                        fOptionMenu->CheckEntry(kOptionStatistics);
                     }
                     gPad->Modified();
                     fCanvas->Update();
                     break;
                  case kOptionHistTitle:
                     if (gStyle->GetOptTitle()) {
                        gStyle->SetOptTitle(0);
                        fOptionMenu->UnCheckEntry(kOptionHistTitle);
                     } else {
                        gStyle->SetOptTitle(1);
                        fOptionMenu->CheckEntry(kOptionHistTitle);
                     }
                     gPad->Modified();
                     fCanvas->Update();
                     break;
                  case kOptionFitParams:
                     if (gStyle->GetOptFit()) {
                        gStyle->SetOptFit(0);
                        fOptionMenu->UnCheckEntry(kOptionFitParams);
                     } else {
                        gStyle->SetOptFit(1);
                        fOptionMenu->CheckEntry(kOptionFitParams);
                     }
                     gPad->Modified();
                     fCanvas->Update();
                     break;
                  case kOptionCanEdit:
                     if (gROOT->GetEditHistograms()) {
                        gROOT->SetEditHistograms(kFALSE);
                        fOptionMenu->UnCheckEntry(kOptionCanEdit);
                     } else {
                        gROOT->SetEditHistograms(kTRUE);
                        fOptionMenu->CheckEntry(kOptionCanEdit);
                     }
                     break;

                  // Handle Inspect menu items...
                  case kInspectRoot:
                     fCanvas->cd();
                     gROOT->Inspect();
                     fCanvas->Update();
                     break;
                  case kInspectBrowser:
                     new TBrowser("browser");
                     break;

                  // Handle Inspect menu items...
                  case kClassesFullTree:
                     // noop
                     break;
                  case kClassesStartAt:
                     // noop
                     break;

                  // Handle Help menu items...
                  case kHelpAbout:
                     // coming soon
                     {
                        char str[32];
                        sprintf(str, "About ROOT %s...", gROOT->GetVersion());
                        hd = new TRootHelpDialog(this, str, 600, 400);
                        hd->SetText(gHelpAbout);
                        hd->Popup();
                     }
                     break;
                  case kHelpOnCanvas:
                     hd = new TRootHelpDialog(this, "Help on Canvas...", 600, 400);
                     hd->SetText(gHelpCanvas);
                     hd->Popup();
                     break;
                  case kHelpOnMenus:
                     hd = new TRootHelpDialog(this, "Help on Menus...", 600, 400);
                     hd->SetText(gHelpPullDownMenus);
                     hd->Popup();
                     break;
                  case kHelpOnGraphicsEd:
                     hd = new TRootHelpDialog(this, "Help on Graphics Editor...", 600, 400);
                     hd->SetText(gHelpGraphicsEditor);
                     hd->Popup();
                     break;
                  case kHelpOnBrowser:
                     hd = new TRootHelpDialog(this, "Help on Browser...", 600, 400);
                     hd->SetText(gHelpBrowser);
                     hd->Popup();
                     break;
                  case kHelpOnObjects:
                     hd = new TRootHelpDialog(this, "Help on Objects...", 600, 400);
                     hd->SetText(gHelpObjects);
                     hd->Popup();
                     break;
                  case kHelpOnPS:
                     hd = new TRootHelpDialog(this, "Help on PostScript...", 600, 400);
                     hd->SetText(gHelpPostscript);
                     hd->Popup();
                     break;
               case kSaveAsps:
                 AMSCanvas::SaveParticleCB();
                 break;
               case kSaveAsgif:
                 AMSCanvas::SaveParticleGIF();
                 break;
               case kSaveAsprint:
                 AMSCanvas::PrintCB();
                 break;
               }
            default:
               break;
         }
      default:
         break;
   }
   return kTRUE;
}

//______________________________________________________________________________
Int_t TRootCanvas::InitWindow()
{
   return gGXW->InitWindow((ULong_t) fCanvasContainer->GetId());
}

//______________________________________________________________________________
void TRootCanvas::SetCanvas(UInt_t w, UInt_t h)
{
   // Set size of canvas container. Unix in pixels.

   // turn off autofit, we want to stay at the given size
   fAutoFit = kFALSE;
   fOptionMenu->UnCheckEntry(kOptionAutoResize);
   int opt = fCanvasContainer->GetOptions();
   opt |= kFixedSize;    // turn on fixed size mode
   fCanvasContainer->ChangeOptions(opt);
   fCanvasContainer->SetWidth(w);
   fCanvasContainer->SetHeight(h);
   Layout();  // force layout (will update container to given size)
   fCanvas->Resize();
   fCanvas->Update();
}

//______________________________________________________________________________
void TRootCanvas::SetCanvasTitle(const char *title)
{
   // Change title on canvas.

   SetWindowName(title);
   SetIconName(title);
}

//______________________________________________________________________________
void TRootCanvas::FitCanvas()
{
   // Fit canvas container to current window size.

   if (!fAutoFit) {
      int opt = fCanvasContainer->GetOptions();
      int oopt = opt;
      opt &= ~kFixedSize;   // turn off fixed size mode
      fCanvasContainer->ChangeOptions(opt);
      Layout();  // force layout
      fCanvas->Resize();
      fCanvas->Update();
      fCanvasContainer->ChangeOptions(oopt);
   }
}

//______________________________________________________________________________
void TRootCanvas::ShowMenuBar(Bool_t show)
{
   // Show or hide menubar.

   if (show) {
      ShowFrame(fMenuBar);
   } else {
      HideFrame(fMenuBar);
   }
}

//______________________________________________________________________________
void TRootCanvas::ShowStatusBar(Bool_t show)
{
   // Show or hide statusbar.

   if (show) {
      ShowFrame(fStatusBar);
      fOptionMenu->CheckEntry(kOptionEventStatus);
   } else {
      HideFrame(fStatusBar);
      fOptionMenu->UnCheckEntry(kOptionEventStatus);
   }
}

//______________________________________________________________________________
Bool_t TRootCanvas::HandleContainerButton(Event_t *event)
{
   // Handle mouse button events in the canvas container.

   Int_t button = event->fCode;
   Int_t x = event->fX;
   Int_t y = event->fY;

   if (event->fType == kButtonPress) {
      fButton = button;
      if (button == kButton1)
         fCanvas->HandleInput(kButton1Down, x, y);
      if (button == kButton2)
         fCanvas->HandleInput(kButton2Down, x, y);
      if (button == kButton3) {
         fCanvas->HandleInput(kButton3Down, x, y);
         fButton = 0;  // button up is consumed by TContextMenu
      }

   } else if (event->fType == kButtonRelease) {
      if (button == kButton1)
         fCanvas->HandleInput(kButton1Up, x, y);
      if (button == kButton2)
         fCanvas->HandleInput(kButton2Up, x, y);
      if (button == kButton3)
         fCanvas->HandleInput(kButton3Up, x, y);

      fButton = 0;
   }

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TRootCanvas::HandleContainerConfigure(Event_t *)
{
   // Handle configure (i.e. resize) event.

   if (fAutoFit) {
      fCanvas->Resize();
      fCanvas->Update();
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TRootCanvas::HandleContainerKey(Event_t *event)
{
   // Handle keyboard events in the canvas container.

   printf("key event %d\n", event->fCode);
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TRootCanvas::HandleContainerMotion(Event_t *event)
{
   // Handle mouse motion event in the canvas container.

   Int_t x = event->fX;
   Int_t y = event->fY;

   if (fButton == 0)
      fCanvas->HandleInput(kMouseMotion, x, y);
   if (fButton == kButton1)
      fCanvas->HandleInput(kButton1Motion, x, y);

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TRootCanvas::HandleContainerExpose(Event_t *event)
{
   // Handle expose events.

   if (event->fCount == 0)
      fCanvas->Flush();

   return kTRUE;
}

