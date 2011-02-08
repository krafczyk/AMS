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


void GenericSlider::showHwId(int hwid){
  if (getnSlider()==0) return;
  for (int id=0; id<getnSlider(); id++) {
    int ret = slider.at(id)->showHwId(hwid);
    printf("GenericSlider::showHwId calling slider->showHwId(hwid) returning %d\n",ret);
    switch(ret) {
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
  }
  Layout();
}


void GenericSlider::showTkId(int tkid){
  if (getnSlider()==0) return;
  for (int id=0; id<getnSlider(); id++) {
    int ret = slider.at(id)->showTkId(tkid);
    printf("GenericSlider::showHwId calling slider->showHwId(tkid) returning %d\n",ret);
    switch(ret) {
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
  }
  Layout();
}


void GenericSlider::PrintThis(){
  if (getnSlider()==0) return;
  info->SetText("This canvas has been printed.");
  Layout();
  for (int id=0; id<getnSlider(); id++) {
    slider.at(id)->PrintThis();
  }
}


void GenericSlider::PrintAll(){
  if (getnSlider()==0) return;
  info->SetText("All canvas have been printed.");
  Layout();
  for (int id=0; id<getnSlider(); id++) {
    slider.at(id)->PrintAll();
  }
}


void GenericSlider::showNext(){
  if (getnSlider()==0) return;
  for (int id=0; id<getnSlider(); id++) {
    slider.at(id)->showNext();
  }
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::showPrev(){
  if (getnSlider()==0) return;
  for (int id=0; id<getnSlider(); id++) {
    slider.at(id)->showPrev();
  }
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::addSlider(int id) {
  if (id==kCalSlider) { 
    slider.push_back(new CalSlider("CalSlider","Calibration Slider"));
  }
  if (id==kMonSlider) {
    slider.push_back(new MonSlider("MonSlider","Data Slider"));
  }
}


void GenericSlider::setRootFile(char *filename){
  for (int id=0; id<getnSlider(); id++) {
    slider.at(id)->canvas->ToggleEventStatus();
    slider.at(id)->setRootFile(filename);
  }
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::Update(){
  if (getnSlider()==0) return;
  for (int id=0; id<getnSlider(); id++) {
    slider.at(id)->canvas->ToggleEventStatus();
    slider.at(id)->Update();
  }
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::setCalfromDB(time_t run){
  for (int id=0; id<getnSlider(); id++) {
    slider.at(id)->canvas->ToggleEventStatus();
    slider.at(id)->setCalfromDB(run);
  }
  info->SetText("");
  Layout();
  Updated();
}


void GenericSlider::setRefFile(char *filename){
  if(getnSlider()==0){
    info->SetText("Select a Calibration or Monitoring file before.");
    Layout();
    return;
  }
  for (int id=0; id<getnSlider(); id++) {
    slider.at(id)->setRefFile(filename);
  }
  Layout();
  Updated();
}


void GenericSlider::Updated(){
  Emit("Updated()");
}


