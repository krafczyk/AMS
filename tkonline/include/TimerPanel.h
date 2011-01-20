#ifndef TimerPanel_h
#define TimerPanel_h

#include <TGWindow.h>
#include <TGFrame.h>
#include <TGNumberEntry.h>
#include <TTimer.h>

class TimerPanel: public TGGroupFrame {

 public:

  TTimer* fTimer;
  TGNumberEntry *panel;
  TGCheckButton *button;

  TimerPanel(TGWindow *p, char* name);
  void Update(); //*SIGNAL*
  Bool_t HandleTimer(TTimer *);
  void SetTimer(Long_t val);
  void SetTimerStatus();

  ClassDef(TimerPanel,0)
};


#endif
