#ifndef CalSlider_h
#define CalSlider_h

#include <TText.h>
#include <TH1F.h>
#include <TFile.h>
#include <TColor.h>
#include <TCanvas.h>
#include <TPaveLabel.h>

#include <TGFrame.h>
#include <TGListBox.h>

#include <TkDBc.h>
#include <TrCalDB.h>
#include <TrLadCal.h>
#include "SliderI.h"


class AMSTimeID;
char * GetOctName(int oct);

class CalSlider: public SliderI {

 private:

  TGListBox* graphmenu;

  static char* filename;

  TH1F *Ped,*SigR,*Sig,*Cmn,*Occ;
  TH1F *ST[16];
  TH1F *ped_n,*ped_p,*sig_p,*sig_n,*sigR_p,*sigR_n,*sta_n,*sta_p;
  TH1F *cn_p,*cn_n,*cn2_p,*cn2_n,*cn3_p,*cn3_n; 
  TH1F *sigR_p_vs_lad,*sig_p_vs_lad,*sigR_n_vs_lad,*sig_n_vs_lad,*bad_p_vs_lad,*bad_n_vs_lad;  
  TH1F *deltaped_vs_lad,*deltasig_vs_lad,*deltasigr_vs_lad;

  TText* text;
  
  TrLadCal* cal;
  TkLadder* lad;
  void SetHistStyle1(TH1* hh);
  void SetHistStyle2(TH1* hh);
  void SetHistStyle3(TH1* hh);
  
 public:
  AMSTimeID *TDV;

  TPaveLabel *label;
  TFile *rootfile,*reffile;
  int fromdb;
  TrCalDB *caldb;
  trcalIT calit_begin,calit_end,calit_cur;
  int graphtype;
  
  CalSlider(char *name,char *title,int xsize=1000, int ysize=400);
  ~CalSlider();
  int OpenTDV();
  int try2Draw(TrLadCal *cc);
  
  /* To implement interface */
  void showNext();
  void showPrev();
  void setRootFile(char* filename);
  void Update();
  void setCalfromDB(time_t run);
  int  ReadCalFromDB(time_t run);
  void setRefFile(char* filename);
  void selectGraph(Int_t w, Int_t id);
  int  showTkId(int tkid);
  int  showHwId(int hwid);
  int  GetHwId();
  int  GetTkId();
  void PrintThis();
  void PrintAll();
  void BuildMenu();

  /* CalPainter methods */
  void FillLadderHistos();
  void Draw1(const Option_t *aa,int flag);
  void Draw2(const Option_t *aa,int flag);
  void Draw3(const Option_t *aa,int flag);
  void Draw4(const Option_t *aa,int flag);
  void Draw5(const Option_t *aa,int flag);
  void Draw6(const Option_t *aa,int flag);
  void Draw7(const Option_t *aa,int flag);
  void Draw8(const Option_t *aa,int flag);
  void Draw9(const Option_t *aa,int flag);

  void Draw(const Option_t* aa,int flag);
  void Draw(const Option_t* aa=""){Draw(aa,0);}
  void Draw(int flag){Draw("",flag);}
  void ClearHistograms();
  void OpenFile();
  static void CloseFile();
  static void SetFileName(char* name){filename=name; }
  
  ClassDef(CalSlider,0);
};

#endif

