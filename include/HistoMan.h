#ifndef HistoMan_h
#define HistoMan_h

#include "TObjArray.h"
#include "TFile.h"
#include "TH2F.h"
#include "TH2D.h"

// Note that TH1F/TH2F means the contents have float precision; the axis has still double precesion

/// Returns TH1F histogram with log bin
TH1F* TH1F_L(const char *name,const char * title,int nbin, double low, double up);
/// Returns TH2F histogram with log/log or log/lin bins depending on logx/logy
TH2F* TH2F_L(const char *name,const char * title,int nbin, double low, double up, int nbiny, double lowy, double upy, bool logx = true, bool logy = true);
/// Returns TH1D histogram with log bin
TH1D* TH1D_L(const char *name,const char * title,int nbin, double low, double up);
/// Returns TH2D histogram with log/log or log/lin bins depending on logx/logy
TH2D* TH2D_L(const char *name,const char * title,int nbin, double low, double up, int nbiny, double lowy, double upy, bool logx = true, bool logy = true);

class HistoMan : public TObject{

private:
  /// Enabled/disabled flag
  bool enabled;
  /// Flag to avoid double booking
  bool booked;
  /// Array to keep pointers to the histogram
  TObjArray fhist;
  /// Output file name
  char fname[256];
public:
  HistoMan();
  ~HistoMan();

  /// Add a new histogram
  void Add(TH1* hist ){fhist.Add(hist);}
  /// Get pointer to the histogram with name
  TH1* Get(const char * name){return (TH1*) fhist.FindObject(name);}
  /// Set output file name
  void Setname(const char *fnamein){sprintf(fname,"%s",fnamein);}
  /// Fill a histogram with name
  void Fill(const char * name, double a,double  b=1.,double w=1.);
  /// Save histograms to file
  void Save();
  /// Save histograms to a specified file
  void Save(TFile *file, const char* dirname = "HistoMan");
  /// Book default histograms (simmode 0: real data, 1: MC gen,  2: MC rec)
  void BookHistos(int simmode = 0);
  /// Is enabled or not
  bool IsEnabled() const { return enabled; }
  /// Enable this facility
  void Enable(){enabled=true;}
  /// Disable this facility
  void Disable(){enabled=false;}
  /// Clears all the histo from memory
  void Clear(Option_t* oo=""){TObject::Clear(oo);fhist.SetOwner(); fhist.Clear();}
  /// Get hman pointer (for ROOT CINT)
  static HistoMan *GetPtr();
  ClassDef(HistoMan,0)
};

extern HistoMan hman;
#endif

