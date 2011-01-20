#include <TGFrame.h>
#include "GenericSlider.h"
#include "InteractionPanel.h"
#include "TrackerPanel.h"
#include "SearchPanel.h"
#include "TimerPanel.h"

class MonitorUI: public TGMainFrame {
public:
  TGMainFrame* fMain;
  GenericSlider *gs;
  TimerPanel *timer;
  InteractionPanel *intpan;
  SearchPanel *search;
  TrackerPanel *track;
  
  MonitorUI(const TGWindow *p,UInt_t w,UInt_t h,char *filename);
  ~MonitorUI() {}
  void HandleMenu(Int_t id);
  void HandleFile(char* filename, int ref = 0);
  void CloseWindow(); 
  ClassDef(MonitorUI,0);
};

