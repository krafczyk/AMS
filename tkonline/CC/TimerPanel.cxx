#include <TGLabel.h>
#include "TimerPanel.h"

ClassImp(TimerPanel);

TimerPanel::TimerPanel(TGWindow *p,char* name) : TGGroupFrame(p,name){
  int defaulttime = 60; // sec
  fTimer = new TTimer(this,defaulttime*1000,0);
  fTimer->TurnOff();
  TGHorizontalFrame *frame = new TGHorizontalFrame(this);
  // on/off
  button = new TGCheckButton(frame," ",1);
  button->SetState(kButtonUp);
  frame->AddFrame(button);
  // comment 1
  TGLabel *label1 = new TGLabel(frame,"Update every ");
  frame->AddFrame(label1);
  // numbers
  panel = new TGNumberEntry(frame,defaulttime,defaulttime,-1,
    TGNumberFormat::kNESInteger,TGNumberFormat::kNEAPositive,
    TGNumberFormat::kNELLimitMinMax,1,100000);
  panel->Resize(50,18);
  frame->AddFrame(panel);
  // comment 2
  TGLabel *label2 = new TGLabel(frame," (sec)");
  frame->AddFrame(label2);

  // connections 
  button->Connect("Clicked()","TimerPanel",this,"SetTimerStatus()");
  panel->Connect("ValueChanged(Long_t)","TimerPanel",this,"SetTimer(Long_t)");
  panel->Connect("ValueSet(Long_t)","TimerPanel",this,"SetTimer(Long_t)");

  AddFrame(frame,new TGLayoutHints(kLHintsRight));
}

void TimerPanel::SetTimerStatus() {
  if (button->IsOn()) { 
    fTimer->Reset(); 
    fTimer->TurnOn(); 
  }
  else
    fTimer->TurnOff(); 
}


Bool_t TimerPanel::HandleTimer(TTimer *) {
  if (!button->IsOn()) return kFALSE; // deactivated
  Update();
  fTimer->Reset();
  fTimer->TurnOn();
  return kTRUE;
}

void TimerPanel::SetTimer(Long_t val) {
  fTimer->SetTime(panel->GetIntNumber()*1000);
  if (!button->IsOn()) return; // deactivated 
  fTimer->Reset();
  fTimer->TurnOn();
}

void TimerPanel::Update() { 
  Emit("Update()");
}

