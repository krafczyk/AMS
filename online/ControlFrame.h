//  $Id: ControlFrame.h,v 1.1 2003/06/17 08:13:22 choutko Exp $
#ifndef __CONTROLFRAME__
#define __CONTROLFRAME__
#include <stdlib.h>
#include <iostream.h>
#include <TROOT.h>
#include <TApplication.h>
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
#include <vector>
using namespace std;

class AMSControlFrame : public TGTransientFrame {
protected:
  TGGroupFrame *_pbutfr, *_pcontrolfr, *_pcombofr, *_plogfr, *_pcyclefr;
  TGRadioButton *_plogx[2];
  TGRadioButton *_plogy[2];
  TGRadioButton *_plogz[2];
  vector<TGCheckButton *> _pcycle;
  vector<TGTextButton *> _pbutton;
  vector<TGTextButton *> _pcontrol;
  TGMenuBar           *fMenuBar;            // menubar
  TGPopupMenu         *fDetMenu;
  vector<TGPopupMenu*>         fSubDetMenu;      //sub det
  vector<TGPopupMenu*>   fSubDetCascadeMenu;
  TGPopupMenu         *fSaveAsMenu;        //  saveas
  
  
  TGLayoutHints       *fMenuBarLayout;      // menubar layout hints
  TGLayoutHints       *fMenuBarItemLayout;  // layout hints for menu in menubar
  TGLayoutHints       *fMenuBarHelpLayout;  // layout hint for help menu in menubar
  TGLayoutHints       *fL1;
  TGLayoutHints       *fL2;
  TGLayoutHints       *fL3;
  TGLayoutHints       *fL4;
  TGLayoutHints       *fL5;
public:
  AMSControlFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
		  UInt_t options = kMainFrame | kVerticalFrame);
  virtual ~AMSControlFrame();
  
  virtual void CloseWindow();
  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
ClassDef(AMSControlFrame,0)
};
#endif
