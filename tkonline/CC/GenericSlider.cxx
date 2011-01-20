#include "GenericSlider.h"

ClassImp(GenericSlider);


GenericSlider::GenericSlider(TGWindow *p) : TGVerticalFrame(p) {
  slider = 0;
  // Graphics stuff (Next and Prev, and Update)
  frame = new TGHorizontalFrame(this);
  prev = new TGTextButton(frame,"&Prev");
  prev->Connect("Clicked()","GenericSlider",this,"showPrev()");
  frame->AddFrame(prev,new TGLayoutHints(kLHintsNormal,2,2,2,2));
  next = new TGTextButton(frame,"&Next");
  next->Connect("Clicked()","GenericSlider",this,"showNext()");
  frame->AddFrame(next,new TGLayoutHints(kLHintsNormal,2,2,2,2));
  update = new TGTextButton(frame,"&Update");
  update->Connect("Clicked()","GenericSlider",this,"Update()");
  frame->AddFrame(update,new TGLayoutHints(kLHintsNormal,2,2,2,2));  
  AddFrame(frame,new TGLayoutHints(kLHintsTop | kLHintsExpandX));
  info = new TGLabel(this);
  AddFrame(info,new TGLayoutHints(kLHintsTop | kLHintsExpandX));
}


void GenericSlider::showHwId(int hwid){
  if (!slider) return;
  int ret = slider->showHwId(hwid);
  printf("GenericSlider::showHwId calling slider->showHwId(hwid) returning %d\n",ret); 
  switch(ret){
  case 0:
    info->SetText("");
    Updated();
    break;
  case 1:
    info->SetText("HwId NOT FOUND.");
    break;
  case 2:
    info->SetText("Open a file before.");
    break;
  }
  Layout();
}


void GenericSlider::showTkId(int tkid){
  if (!slider) return;
  int ret = slider->showTkId(tkid);
  switch(ret){
  case 0:
    info->SetText("");
    Updated();
    break;
  case 1:
    info->SetText("TkId NOT FOUND.");
    break;
  case 2:
    info->SetText("Open a file before.");
    break;
  }
  Layout();
}


void GenericSlider::PrintThis(){
  if (!slider) return;
  info->SetText("This canvas has been printed.");
  Layout();
  slider->PrintThis();
}


void GenericSlider::PrintAll(){
  if (!slider) return;
  info->SetText("All canvas have been printed.");
  Layout();
  slider->PrintAll();
}


void GenericSlider::showNext(){
  if (!slider) return;
  slider->showNext();
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::showPrev(){
  if (!slider) return;
  slider->showPrev();
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::setSlider(int id) {
  if (slider!=0) { 
    delete slider;
    slider = 0; 
  }
  if (id==kCalSlider) { 
    slider = new CalSlider("CalSlider","Calibration Slider");
  }
  if (id==kMonSlider) {
    slider = new MonSlider("MonSlider","Data Slider");
  }
}


void GenericSlider::setRootFile(char *filename){
  slider->canvas->ToggleEventStatus();
  slider->setRootFile(filename);
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::Update(){
  if (!slider) return;
  slider->canvas->ToggleEventStatus();
  slider->Update();
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::setCalfromDB(time_t run){
  slider->canvas->ToggleEventStatus();
  slider->setCalfromDB(run);
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::setRefFile(char *filename){
  if(!slider){
    info->SetText("Select a Calibration or Monitoring file before.");
    Layout();
    return;
  }
  slider->setRefFile(filename);
  Layout();
  Updated();
}


void GenericSlider::Updated(){
  Emit("Updated()");
}


