#ifndef HistoMan_h
#define HistoMan_h

#include "TObjArray.h"
#include "TH2F.h"
#include "TH2D.h"

TH1F* TH1F_L(char *name,char* title,int nbin, float low, float up);
TH2F* TH2F_L(char *name,char* title,int nbin, float low, float up, int nbiny, float lowy, float upy);

TH1D* TH1D_L(char *name,char* title,int nbin, double low, double up);
TH2D* TH2D_L(char *name,char* title,int nbin, double low, double up, int nbiny, double lowy, double upy);

class HistoMan{

private:
  bool enabled;
  TObjArray fhist;
  char fname[128];
public:
  HistoMan():enabled(false) { fhist.SetOwner(kTRUE); sprintf(fname,"histos.root");}

  ~HistoMan(){ }

  void Add(TH1* hist ){fhist.Add(hist);}
  TH1* Get(char* name){return (TH1*) fhist.FindObject(name);}
  void Setname(char*fnamein){sprintf(fname,"%s",fnamein);}
  void Fill(char* name, double a,double  b=1.,double w=1.);
  void Save();
  void BookHistos();
  void Enable(){enabled=true;}
  void Disable(){enabled=false;}

};
#endif

