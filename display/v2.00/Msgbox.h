//  $Id: Msgbox.h,v 1.2 2001/01/22 17:32:41 choutko Exp $
#ifndef __MSGBOX__
#define __MSGBOX__
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

class MsgBox : public TGTransientFrame {

private:
   TGTextEntry          *fMsg;
   TGTextBuffer         *fTbmsg;
   TGLabel              *fLtitle;
   TGLayoutHints        *fL1;
   TGLayoutHints        *fL2;
   TGCompositeFrame     *f2;
public:
   MsgBox(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
              UInt_t options = kMainFrame | kVerticalFrame);
   virtual ~MsgBox();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

   ClassDef(MsgBox,0)
};

#endif
