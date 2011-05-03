#ifndef LISTBOX_H
#define LISTBOX_H

#ifndef  ROOT_TGFrame
#include "TGFrame.h"
#endif


class TGListBox;
class TGTexButton;
class Tlist;

class AMSTimeID;

class CalList : public TGTransientFrame {
  
private:
  TGListBox           *fListBox;
  TGTextButton        *fOk;
  TGTextButton        *fCancel;
  TList               *fSelected;   
  AMSTimeID           *timeid;
  int                 nn;
  int                 num;
  CalList(const CalList&);
  CalList &operator= (const CalList&);

public:
  CalList(AMSTimeID * TT,
	  const TGWindow *p,const TGWindow* main, 
	  UInt_t w, UInt_t h, UInt_t options= kVerticalFrame);
  virtual ~CalList();
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  
   ClassDef(CalList, 0)
};

#endif
