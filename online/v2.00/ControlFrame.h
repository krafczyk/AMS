#ifndef __CONTROLFRAME__
#define __CONTROLFRAME__
#include <stdlib.h>
#include <iostream.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TGXW.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGFileDialog.h>

const nsubdets = 8;
const nbuttons = 8;

class AMSControlFrame : public TGTransientFrame {
protected:
  TGGroupFrame *_pbutfr, *_pcontrolfr, *_pcombofr, *_plogfr, *_pcyclefr;
  TGRadioButton *_plogx[2];
  TGRadioButton *_plogy[2];
  TGRadioButton *_plogz[2];
  TGCheckButton *_pcycle[nbuttons];
  TGTextButton *_pbutton[nbuttons];
  TGTextButton *_pcontrol[3];
  TGMenuBar           *fMenuBar;            // menubar
  TGPopupMenu         *fDetMenu;
  TGPopupMenu         *fSubDetMenu[nsubdets];      //sub det
  TGPopupMenu         *fSubDetCascadeMenu[nsubdets];
  TGPopupMenu         *fSaveAsMenu;        //  saveas
  
  TGPopupMenu         *fTOFSelectMenu;      //  TOF side selection 
  TGPopupMenu         *fTOFPlaneMenu[4];    //  TOF plane submenu
  TGPopupMenu         *fTOFCounterMenu[4][14]; //  TOF Counters
  TGPopupMenu         *fTOFSideMenu[4][14][2];     //  TOF Side
  
  TGLayoutHints       *fMenuBarLayout;      // menubar layout hints
  TGLayoutHints       *fMenuBarItemLayout;  // layout hints for menu in menubar
  TGLayoutHints       *fMenuBarHelpLayout;  // layout hint for help menu in menubar
  TGLayoutHints       *fL1;
  TGLayoutHints       *fL2;
  TGLayoutHints       *fL3;
  TGLayoutHints       *fL4;
public:
  AMSControlFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
		  UInt_t options = kMainFrame | kVerticalFrame);
  virtual ~AMSControlFrame();
  
  virtual void CloseWindow();
  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
ClassDef(AMSControlFrame,0)
};
#endif
