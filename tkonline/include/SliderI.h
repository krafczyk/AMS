#ifndef SliderI_h
#define SliderI_h

#include <TROOT.h>
#include <TCanvas.h>
#include <TGFrame.h>

/* SliderI is the slider interface */

class SliderI : public TObject {

 public:

  /// Canvas in the frame
  TCanvas*     canvas;
  /// Frame
  TGMainFrame* frame;

 public:

  /// Get canvas
  TCanvas* GetCanvas() { return canvas; }
  /// Update
  virtual void Update() = 0;
  /// Set the root-file 
  virtual void setRootFile(char *filename) = 0;
  /// Set the comparison root-file 
  virtual void setRefFile(char *filename) = 0;
  virtual void selectGraph(Int_t w, Int_t id) = 0;
  /// Show next ladder
  virtual void showNext() = 0;
  /// Show previous ladder
  virtual void showPrev() = 0;
  /// Show ladder by TkId
  virtual int  showTkId(int tkid) = 0;
  /// Show ladder by HwId
  virtual int  showHwId(int hwid) = 0;
  /// Get HwId of current ladder
  virtual int  GetHwId() = 0;
  /// Get TkId of current ladder
  virtual int  GetTkId() = 0;
  /// Print this ladder
  virtual void PrintThis() = 0;
  /// Print all the 192 ladder
  virtual void PrintAll() = 0;
  /// Set calibration file from database
  virtual void setCalfromDB(time_t run) = 0; 

  /// Constructor
  SliderI(char *name, char *title, UInt_t w, UInt_t h);
  /// Destructor
  virtual ~SliderI();

  ClassDef(SliderI,0);
};

#endif

