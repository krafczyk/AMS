#include "GenericSlider.h"

ClassImp(GenericSlider);


GenericSlider::GenericSlider(TGWindow *p) : TGVerticalFrame(p) {
  slider.clear();
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


void GenericSlider::Clear() {
  for (map<int,SliderI*>::iterator it=slider.begin(); it!=slider.end(); it++) {
    delete (*it).second;
  }
  slider.clear();
}


void GenericSlider::showHwId(int hwid){
  if (getnSlider()==0) return;
  for (map<int,SliderI*>::iterator it=slider.begin(); it!=slider.end(); it++) {
    int ret = (*it).second->showHwId(hwid);
    switch(ret) {
    case 0:
      info->SetText("");
      Updated();
      break;
    case 1:
      info->SetText("HwId not found");
      break;
    case 2:
      info->SetText("Open a file before");
      break;
    }
  }
  Layout();
}


void GenericSlider::showTkId(int tkid){
  if (getnSlider()==0) return;
  for (map<int,SliderI*>::iterator it=slider.begin(); it!=slider.end(); it++) {
    int ret = (*it).second->showTkId(tkid);
    switch(ret) {
    case 0:
      info->SetText("");
      Updated();
      break;
    case 1:
      info->SetText("TkId not found");
      break;
    case 2:
      info->SetText("Open a file before");
      break;
    }
  }
  Layout();
}


void GenericSlider::PrintThis(){
  if (getnSlider()==0) return;
  info->SetText("This canvas has been printed");
  Layout();
  for (map<int,SliderI*>::iterator it=slider.begin(); it!=slider.end(); it++) {
    (*it).second->PrintThis();
  }
}


void GenericSlider::PrintAll(){
  if (getnSlider()==0) return;
  info->SetText("All canvas have been printed");
  Layout();
  for (map<int,SliderI*>::iterator it=slider.begin(); it!=slider.end(); it++) {
    (*it).second->PrintAll();
  }
}


void GenericSlider::showNext(){
  if (getnSlider()==0) return;
  for (map<int,SliderI*>::iterator it=slider.begin(); it!=slider.end(); it++) {
    (*it).second->showNext();
  }
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::showPrev(){
  if (getnSlider()==0) return;
  for (map<int,SliderI*>::iterator it=slider.begin(); it!=slider.end(); it++) {
    (*it).second->showPrev();
  }
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::addSlider(int id) {
  if (id==kCalSlider) { 
    if (slider.find(kCalSlider)!=slider.end()) delete slider[kCalSlider];
    slider[kCalSlider] = new CalSlider("CalSlider","Calibration Slider");
  }
  if (id==kMonSlider) {
    if (slider.find(kMonSlider)!=slider.end()) delete slider[kMonSlider];
    slider[kMonSlider] = new MonSlider("MonSlider","Data Slider");
  }
}


void GenericSlider::setRootFile(char *filename, int id) {
  if (slider[id]==0) addSlider(id); 
  slider[id]->canvas->ToggleEventStatus();
  slider[id]->setRootFile(filename);
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::Update(){
  if (getnSlider()==0) return;
  for (map<int,SliderI*>::iterator it=slider.begin(); it!=slider.end(); it++) {
    (*it).second->canvas->ToggleEventStatus();
    (*it).second->Update();
  }
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::setCalfromDB(time_t run){
  for (map<int,SliderI*>::iterator it=slider.begin(); it!=slider.end(); it++) {
    (*it).second->canvas->ToggleEventStatus();
    (*it).second->setCalfromDB(run);
  }
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::setRefFile(char *filename){
  if(getnSlider()==0){
    info->SetText("Select a Calibration or Monitoring file before.\n");
    Layout();
    return;
  }
  for (map<int,SliderI*>::iterator it=slider.begin(); it!=slider.end(); it++) {
    (*it).second->setRefFile(filename);
  }
  Layout();
  Updated();
}


void GenericSlider::Updated(){
  Emit("Updated()");
}


