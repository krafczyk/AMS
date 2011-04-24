#include "TGFrame.h"
#include "TGTextEntry.h"
#include "TList.h"
#include "TGWindow.h"
#include "TGLabel.h"
#include "TGTextBuffer.h"
#include "TGLayout.h"
#include "TGButton.h"
#include "TObject.h"
///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Input Dialog Widget                                                   //
//                                                                       //
///////////////////////////////////////////////////////////////////////////
#ifndef INPUTDIALOG_H_
#define INPUTDIALOG_H_
class InputDialog:public TObject {

private:
   TGTransientFrame *fDialog;  // transient frame, main dialog window
   TGTextEntry      *fTE;      // text entry widget containing
   TList            *fWidgets; // keep track of widgets to be deleted in dtor
   char             *fRetStr;  // address to store return string

public:
   InputDialog(const char *prompt, const char *defval, char *retstr);
   ~InputDialog();
   void ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   
	ClassDef(InputDialog,1);	
};
#endif

