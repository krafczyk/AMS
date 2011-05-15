#include "GenericSlider.h"
#include "InteractionPanel.h"
#include "TrackerPanel.h"
#include "SearchPanel.h"
#include "TimerPanel.h"

#include "TStyle.h"
#include <TGFrame.h>

class MonitorUI: public TGMainFrame {

 public:

  TGMainFrame* fMain;
  GenericSlider *gs;
  TimerPanel *timer;
  InteractionPanel *intpan;
  SearchPanel *search;
  TrackerPanel *track;
  static TStyle *style; 
  
  MonitorUI(const TGWindow *p, UInt_t w, UInt_t h, char *filename1, char* filename2 = "");
  ~MonitorUI() {}
  void HandleMenu(Int_t id);
  void HandleFile(char* filename, int ref = 0);
  void CloseWindow(); 
  static void SetStyle();
  ClassDef(MonitorUI,0);
};

