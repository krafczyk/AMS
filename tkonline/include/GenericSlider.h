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

#include <vector>

class GenericSlider: public TGVerticalFrame {

  RQ_OBJECT("GenericSlider")

 public:

  /// Sliders types (... kShiSlider ...)
  enum Sliders { kCalSlider = 0, kMonSlider = 1}; 
  /// Slider collection (tmp, up to 2 sliders)
  vector<SliderI*> slider;

  ///////////////////
  // Grap. Elements
  ///////////////////

  /// Buttons
  TGTextButton *next,*prev,*update;
  /// Canvas
  TGHorizontalFrame *frame;
  /// Label
  TGLabel *info;
  /// Box
  TGComboBox *graph;

 public:

  /// Constructor
  GenericSlider(TGWindow *p);

  /// Signal update
  void Updated(); //*SIGNAL*
  /// Perform update
  void Update();
  /// Open root file
  void setRootFile(char *filename);
  /// Set calibration from TDV
  void setCalfromDB(time_t run);
  /// Set a reference file
  void setRefFile(char *filename);
  /// Show the choosen ladder (by TkId) 
  void showTkId(int tkid);
  /// Show the choosen ladder (by HwId)
  void showHwId(int hwid);
  /// Show next ladder
  void showNext();
  /// Show previous ladder
  void showPrev();
  /// Print current canvas
  void PrintThis();
  /// Print all the 192 ladder canvas
  void PrintAll();

  /// Add a slider
  void addSlider(int id);
  /// Get the number of active sliders
  int  getnSlider() { return (int) slider.size(); }
  /// Get a slider
  SliderI* getSlider(int id = 0) { return ((id>=0)&&id<getnSlider()) ? slider.at(id) : 0; } 
  /// Clear sliders
  void Clear() {
    for (int id=0; id<getnSlider(); id++) delete slider.at(id);	 
    slider.clear(); 
  }
 
  ClassDef(GenericSlider,0);
};

#endif

