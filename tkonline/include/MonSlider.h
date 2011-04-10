#ifndef MonSlider_h
#define MonSlider_h

#include "TkLadder.h"
#include "TkDBc.h"
#include "TkSens.h"
#include "TrHistoMan.h"
#include "Predict.h"

#include "SliderI.h"

#include "TText.h"
#include "TLatex.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TFile.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TPaveLabel.h"
#include "TPolyLine.h"
#include "TGFrame.h"
#include "TGListBox.h"
#include "TROOT.h"

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
  /// Text
  TText*      text;

 public:
 
  /// Constructor
  MonSlider(char *name,char *title,int xsize=1000, int ysize=400);
  /// Destructor
  ~MonSlider();

  /// Show next ladder 
  void showNext();
  /// Show previous ladder
  void showPrev();
  /// Update canvas
  void Update();
  /// Set the rootfile
  void setRootFile(char* filename);
  /// Set reference file
  void setRefFile(char* filename) { reffile = TFile::Open(filename); }
  /// Select graph
  void selectGraph(Int_t w, Int_t id);
  /// Show ladder by TkId
  int  showTkId(int tkid) { if (rootfile==0) return 2; return try2Draw(TkDBc::Head->FindTkId(tkid)); }
  /// Show ladder by HwId
  int  showHwId(int hwid) { if (rootfile==0) return 2; return try2Draw(TkDBc::Head->FindHwId(hwid)); }
  /// Get the HwId of the current ladder
  int  GetHwId() { return ladder->GetHwId(); }
  /// Get the TkId of the current ladder
  int  GetTkId() { return ladder->GetTkId(); }
  /// Print this canvas to a file
  void PrintThis();
  /// Print this canvas to a file
  void PrintThis(char* name);
  /// Print all the 192 canvas
  void PrintAll();
  /// Set calibration from database (unused)
  void setCalfromDB(time_t run) {} 
 
  /// Build the data menu
  void BuildMenu();

  /// Drawing the ladder
  int  try2Draw(TkLadder *lad);  
  /// Draw
  void Draw();
  /// Draw raw entries
  void DrawRawEntries(int alternative = 0);
  /// Draw cluster entries on track
  void DrawTrackEntries(int alternative = 0);
  /// Draw the cluster signal summary (average signal and width)
  void DrawSignalSummary(int alternative = 0);
  /// Draw a summary of size plots (tracker, crates, ladders)
  void DrawSizeSummary(int alternative = 0);
  /// Low interevent time distance dependence plots
  void DrawDtSummary(int alternative = 0);
  /// Low interevent time distance dependence plots (crates)
  void DrawCrateVsDt(int alternative = 0);
  /// Draw plots of a single ladder (signal, width, occupancy, size)
  void DrawLadder(int alternative = 0);
  /// Reconstruction statistics
  void DrawReconStats(int alternative = 0);
  /// Hit occupancy on layers
  void DrawHitsOnTrack(int alternative = 0);
  /// Seed occupancy statistics
  void DrawSeedOccupancyGlobal(int alternative = 0);
  /// Track statistics (theta-phi, X-Y, rigidity plus-minus, chisquared-rigidity) 
  void DrawTrack(int alternative = 0);
  /// Draw rigidity
  void DrawRigidity(int alternative = 0);
  /// Occupancy ladder by ladder (beam test purpose)
  void DrawSeedOccupancyOnLayer(int alternative = 0);
  /// Draw Summary 
  void DrawClustersSummary(int alternative = 0);
  /// Draw reconstruction versus time
  void DrawReconVsTime(int alternative = 0);
  /// Draw some very rough estimation of orbits 
  void DrawOrbitFromTime(int alternative = 0);
  /// Draw a panel for data-goodness
  void DrawIsGood(int alternative = 0);
  /// Write monitoring file infos
  void DrawInfo(int alternative = 0);
  /// Test and draw a text
  void TestAndDrawText(int side, TH1D* histo, float xmin, float xmax);

  /// Get the histogram named "name", or if doesn't exist, the "oldname" one 
  TH1*  GetHisto(TFile* file, char* name, char* oldname);
  /// Get the histogram named "name"
  TH1*  GetHisto(TFile* file, char* name); 
  /// Get the histogram from the file
  TH1*  GetHistoFromFile(TFile* file, char* name);
  /// Get the histogram from memory
  TH1*  GetHistoFromMemory(char* name);
  /// Clear an histogram from the safe memory area
  void  ClearHistoFromMemory(char* name);
  /// Make the X profile in a safe memory area
  TH1D* GetProfileX(TH2* histo);
  /// Changing X index 
  void  ChangeToLayerIndex(TH1D* histo);
  // Set histograms styles
  void  SetHistSideStyle(int side, TH1* histo);

  ClassDef(MonSlider,0);
};

#endif

