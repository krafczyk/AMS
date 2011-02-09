#ifndef MonSlider_h
#define MonSlider_h

#include "TkLadder.h"
#include "TkDBc.h"
#include "TkSens.h"
#include "TrHistoMan.h"

#include "SliderI.h"

#include <TText.h>
#include <TLatex.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TFile.h>
#include <TColor.h>
#include <TCanvas.h>
#include <TPaveLabel.h>
#include <TPolyLine.h>
#include <TGFrame.h>
#include <TGListBox.h>
#include <TROOT.h>

char * GetOctName(int oct);

class MonSlider : public SliderI {

 private:

  /// Graph menu
  TGListBox*  graphmenu;
  /// Root file
  TFile*      rootfile;
  /// Reference file
  TFile*      reffile;
  /// Current graph
  int         graphtype;
  /// Current ladder
  TkLadder*   ladder;
  /// Labels
  TPaveLabel* label;

 public:
 
  /// Constructor
  MonSlider(char *name,char *title,int xsize=1000, int ysize=400);
  /// Destructor
  ~MonSlider();

  void showNext();
  void showPrev();
  void Update();
  void setRootFile(char* filename);
  void setRefFile(char* filename) { reffile = TFile::Open(filename); }
  void selectGraph(Int_t w, Int_t id);
  int  showTkId(int tkid) { if (rootfile==0) return 2; return try2Draw(TkDBc::Head->FindTkId(tkid)); }
  int  showHwId(int hwid) { if (rootfile==0) return 2; return try2Draw(TkDBc::Head->FindHwId(hwid)); }
  int  GetHwId() { return ladder->GetHwId(); }
  int  GetTkId() { return ladder->GetTkId(); }
  void PrintThis();
  void PrintThis(char* name);
  void PrintAll();
  void setCalfromDB(time_t run) {} 
 
  /* Specific */ 
  void BuildMenu();
  int  try2Draw(TkLadder *lad);  

  void Draw();
  void DrawSignalSummary();
  void DrawSizeSummary();
  void DrawDtSummary();
  void DrawCrateVsDt();
  void DrawLadder();
  void DrawReconStats();
  void DrawHitsOnTrack();
  void DrawSeedOccupancyGlobal();
  void DrawTrack();
  void DrawRigidity();
  void DrawSeedOccupancyOnLayer();
  void DrawHits();
  void DrawClustersSummary();
  // void DrawBeamTestVsEntries(int pp);
  // void DrawBeamTest3();

  // Check and store an histogram in a safe memory area 
  TH1* GetHisto(TFile* file, char* name, char* oldname);
  TH1* GetHisto(TFile* file, char* name); 
  TH1* GetHistoFromFile(TFile* file, char* name);
  TH1* GetHistoFromMemory(char* name);
  // Clear an histogram from the safe memory area
  void ClearHistoFromMemory(char* name);
  // Make the X profile in a safe memory area
  TProfile* GetProfileX(TH2D* histo);
  // Set histograms styles
  void SetHistSideStyle(int side, TH1* histo);

  ClassDef(MonSlider,0);
};

#endif

