#include <math.h>
#include "HistoMan.h"
#include "TFile.h"
#include "TROOT.h"
#include "TDirectoryFile.h"

HistoMan hman;


HistoMan::HistoMan():enabled(false), booked(false) { 
  //  fhist.SetOwner(kTRUE); 
  sprintf(fname,"histos.root");
}

HistoMan::~HistoMan(){}

void HistoMan::Save(){
  if(!enabled) return;
  if(!fname[0])return;
  printf("HistoMan::Save ----> Saving %s\n",fname);
  TFile* pp=TFile::Open(fname,"RECREATE");
  pp->cd();
  fhist.Write();
  pp->Write();
  pp->Close();
  printf(" ..... done\n");
  //  fdir->Purge();
  return;
}

void HistoMan::Save(TFile *file){
  if(!enabled) return;
  if(!file)return;
  printf("HistoMan::Save ----> Saving %s\n",file->GetName());

  TDirectory *dsave = gDirectory;
  file->cd();
  TDirectoryFile *dir = new TDirectoryFile("HistoMan", "HistoMan");
  dir->cd();
  fhist.Write();
  printf(" ..... done\n");
  if (dsave) dsave->cd();

  return;
}

void HistoMan::Fill(const char * name, double a,double  b,double w){
  if(!enabled) return;
  TH1*hist = (TH1*) fhist.FindObject(name);
  if(!hist){
    printf("HistoMan::Fill: Error Unknown histo %s\n",name);
    return;
  }
#pragma omp critical (hmanfill)
{
  if( hist->InheritsFrom("TH2"))
    ((TH2*)hist)->Fill(a,b,w);
  else if( hist->InheritsFrom("TH1"))
    ((TH1*)hist)->Fill(a,b);
} 
  return;
}



TH1F* TH1F_L(const char *name,const char * title,int nbin, double low, double up){
if(low<=0||up<=0){
printf("TH1F_L: Error low or up lower than 0 not allowed on log\n");
return 0;
}
  // Note that TH1F means the contents have float precision; the axis has still double precesion
  double lowL = log10(low);
  double upL  = log10(up);
  double step = (upL-lowL)/nbin;
  double *bins= new double[nbin+1];
  bins[nbin]=up;
  for (int ii=0;ii<nbin;ii++)
    bins[ii]=pow(10,lowL+ii*step);

  TH1F* hh= new TH1F(name,title,nbin, bins);
  delete bins;
  return hh;
}

TH2F* TH2F_L(const char *name,const char * title,int nbin, double low, double up, int nbiny, double lowy, double upy, bool logx, bool logy){
  if((logx && (low<=0||up<=0))|| (logy && (lowy<=0||upy<=0)) ){
    printf("TH2F_L: Error low or up lower than 0 not allowed on log\n");
    return 0;
  }
  // Note that TH2F means the contents have float precision; the axis has still double precesion
  double *bins = 0;
  if (logx) {
    double lowL = log10(low);
    double upL  = log10(up);
    double step = (upL-lowL)/nbin;
    bins= new double[nbin+1];
    bins[nbin]=up;
    for (int ii=0;ii<nbin;ii++)
      bins[ii]=pow(10,lowL+ii*step);
  }

  double *binsy = 0;
  if (logy) {
    double lowyL = log10(lowy);
    double upyL  = log10(upy);
    double stepy = (upyL-lowyL)/nbiny;
    binsy= new double[nbiny+1];
    binsy[nbiny]=upy;
    for (int ii=0;ii<nbiny;ii++)
      binsy[ii]=pow(10,lowyL+ii*stepy);
  }


  TH2F* hh;
  if (logx && logy)
    hh= new TH2F(name,title,nbin, bins, nbiny, binsy );
  else if (!logx && logy)
    hh= new TH2F(name,title,nbin, low, up, nbiny, binsy );
  else if (logx && !logy)
    hh= new TH2F(name,title,nbin, bins, nbiny, lowy, upy );
  else
    hh= new TH2F(name,title,nbin, low, up, nbiny, lowy, upy );
  delete bins;
  delete binsy;
  return hh;
}

TH1D* TH1D_L(const char *name,const char * title,int nbin, double low, double up){
if(low<=0||up<=0){
printf("TH1D_L: Error low or up lower than 0 not allowed on log\n");
return 0;
}
  double lowL = log10(low);
  double upL  = log10(up);
  double step = (upL-lowL)/nbin;
  double *bins= new double[nbin+1];
  bins[nbin]=up;
  for (int ii=0;ii<nbin;ii++)
    bins[ii]=pow(10,lowL+ii*step);

  TH1D* hh= new TH1D(name,title,nbin, bins);
  delete bins;
  return hh;
}

TH2D* TH2D_L(const char *name,const char * title,int nbin, double low, double up, int nbiny, double lowy, double upy, bool logx, bool logy){
  if((logx && (low<=0||up<=0))|| (logy && (lowy<=0||upy<=0)) ){
    printf("TH2D_L: Error low or up lower than 0 not allowed on log\n");
    return 0;
  }
  double *bins= 0;
  if (logx) {
    double lowL = log10(low);
    double upL  = log10(up);
    double step = (upL-lowL)/nbin;
    bins= new double[nbin+1];
    bins[nbin]=up;
    for (int ii=0;ii<nbin;ii++)
      bins[ii]=pow(10,lowL+ii*step);
  }

  double *binsy= 0;
  if (logy) {
    double lowyL = log10(lowy);
    double upyL  = log10(upy);
    double stepy = (upyL-lowyL)/nbiny;
    binsy= new double[nbiny+1];
    binsy[nbiny]=upy;
    for (int ii=0;ii<nbiny;ii++)
      binsy[ii]=pow(10,lowyL+ii*stepy);
  }

  TH2D* hh;
  if (logx && logy)
    hh= new TH2D(name,title,nbin, bins, nbiny, binsy );
  else if (!logx && logy)
    hh= new TH2D(name,title,nbin, low, up, nbiny, binsy );
  else if (logx && !logy)
    hh= new TH2D(name,title,nbin, bins, nbiny, lowy, upy );
  else
    hh= new TH2D(name,title,nbin, low, up, nbiny, lowy, upy );
  delete bins;
  delete binsy;
  return hh;
}
//==============================================================
//==============================================================
//==============================================================

#include "TrRecon.h"

void HistoMan::BookHistos(){
  if (!enabled || booked) return;

  TDirectory *dsave = gDirectory;

  // Histograms are booked under memory-based directory 
  // and written in file later when HistoMan::Save is called
  gROOT->cd();
  TDirectory *dir = new TDirectory("HistoMan", "HistoMan");
  dir->cd();
  printf("HistoMan::BookHistos: Directroy HistoMan created in memory\n");

  // Tracker low level
  Add(new TH2D("TrSizeDt", "#DeltaT VS TrSize", 500, 0, 1000, 300, 0, 24000));
  Add(new TH2D("TrNrawLt", "nRaw VS Evt LoDt", 1000, 0,  1e6, 500, 0,  1000));
  Add(new TH2D("TrNclsLt", "nCls VS Evt LoDt", 1000, 0,  1e6, 500, 0,  1000));
  Add(new TH2D("TrNhitLt", "nHit VS Evt LoDt", 1000, 0,  1e6, 500, 0,  1000));
  Add(new TH2D("TrNrawHt", "nRaw VS Evt HiDt", 1000, 0,  1e6, 500, 0,  1000));
  Add(new TH2D("TrNclsHt", "nCls VS Evt HiDt", 1000, 0,  1e6, 500, 0,  1000));
  Add(new TH2D("TrNhitHt", "nHit VS Evt HiDt", 1000, 0,  1e6, 500, 0,  1000));

  // TrCluster
  Add(new TH2D("TrClsSigP", "Amplitude(P) VS OnTrack", 2, 0, 2, 500, 0, 500));
  Add(new TH2D("TrClsSigN", "Amplitude(N) VS OnTrack", 2, 0, 2, 500, 0, 500));

  // TrRecHit
  Add(new TH2D("TrLadTrk", "Ladder on track",   33, -16.5, 16.5, 8, 0.5, 8.5));
  Add(new TH2D("TrLadYh",  "Ladder with hitY",  33, -16.5, 16.5, 8, 0.5, 8.5));
  Add(new TH2D("TrLadXYh", "Ladder with hitXY", 33, -16.5, 16.5, 8, 0.5, 8.5));

  // TrTrack
  Add(TH2D_L("TrTime","Tcpu VS nTrHit", 500,   0, 1e3,  140, 1e-3, 1e4, 0, 1));
  Add(TH2D_L("TrNhit", "NhitXY VS NhitY", 9,   0,   9,    9,    0,   9, 0, 0));
  Add(TH2D_L("TrCsqX",  "ChisqX VS RGT", 40, 0.1, 1e3,  120, 1e-2, 1e4, 1, 1));
  Add(TH2D_L("TrCsqY",  "ChisqY VS RGT", 40, 0.1, 1e3,  120, 1e-2, 1e4, 1, 1));
  Add(TH2D_L("TrResX", "ResX/um VS RGT", 40, 0.1, 1e3, 1000, -5e3, 5e3, 1, 0));
  Add(TH2D_L("TrResY", "ResY/um VS RGT", 40, 0.1, 1e3, 1000, -5e3, 5e3, 1, 0));
  Add(TH2D_L("TrChgP", "ChargeP VS RGT", 40, 0.1, 1e3,  500,    0, 500, 1, 0));
  Add(TH2D_L("TrChgN", "ChargeN VS RGT", 40, 0.1, 1e3,  500,    0, 500, 1, 0));

  // TrSim
  if (TrSim::SkipRawSim) {
    Add(new TH2F("TrSimRx", "Sim Xreso VS angX", 50, 0, 50, 100, -100, 100));
    Add(new TH2F("TrSimRy", "Sim Yreso VS angY", 50, 0, 50, 100, -100, 100));
  }

  if (dsave) dsave->cd();
  booked = true;
}
