#ifndef GenericSlider_h
#define GenericSlider_h

#include <RQ_OBJECT.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include "CalSlider.h"
#include "MonSlider.h"
#include "SliderI.h"


class GenericSlider: public TGVerticalFrame {

  RQ_OBJECT("GenericSlider")

 public:

  enum Sliders { kCalSlider = 0, kMonSlider = 1}; 

  TGTextButton *next,*prev,*update;
  TGHorizontalFrame *frame;
  TGLabel *info;
  TGComboBox *graph;
  SliderI* slider;
  
  GenericSlider(TGWindow *p);
  void Updated(); //*SIGNAL*
  void Update();
  void setRootFile(char *filename);
  void setCalfromDB(time_t run);
  void setRefFile(char *filename);
  void showTkId(int tkid);
  void showHwId(int hwid);
  void showNext();
  void showPrev();
  void PrintThis();
  void PrintAll();

  void setSlider(int id); 
  SliderI* getSlider() { return slider; } 
 
  ClassDef(GenericSlider,0);
};

#endif

