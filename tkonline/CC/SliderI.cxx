#include "SliderI.h"


ClassImp(SliderI);


SliderI::SliderI(char *name, char *title, UInt_t w, UInt_t h) : TObject() {
  canvas = new TCanvas(name,title, w,h);
  frame  = new TGMainFrame(gClient->GetRoot(),200,200);
}


SliderI::~SliderI() { 
  printf("IL DISTRUTTORE %d %d\n",canvas,frame);
  if (canvas!=0) delete canvas;
  canvas = 0;
  if (frame!=0) delete frame; 
  frame = 0;
  printf("IL DISTRUTTORE %d %d\n",canvas,frame); 
}

