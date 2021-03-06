/// $Id$ 
#include <math.h>
#include "HistoMan.h"
#include "TFile.h"
#include "TH3.h"
#include "TProfile.h"
#include "TROOT.h"
#include "TDirectoryFile.h"
#include "tkdcards.h"
#ifdef __G4AMS__
#ifndef __ROOTSHAREDLIBRARY__
#include "geant321g.h"
#endif
#endif
ClassImp(HistoMan)
HistoMan hman;

HistoMan::HistoMan():enabled(false), booked(false) { 
  //  fhist.SetOwner(kTRUE); 
  sprintf(fname,"histos.root");
}

HistoMan::~HistoMan(){}

HistoMan *HistoMan::GetPtr()
{
  return &hman;
}

void HistoMan::Save(){
  if(!enabled) return;
  if(!fname[0])return;
  printf("HistoMan::Save ----> Saving %s\n",fname);
  TDirectory *dsave = gDirectory;
  TFile* pp=TFile::Open(fname,"RECREATE");
  pp->cd();
  for (int i = 0; i < fhist.GetEntries(); i++) {
    TH1 *obj = (TH1 *)fhist.At(i);
    if (obj && obj->GetEntries() > 0) obj->Write();
  }
  pp->Write();
  pp->Close();
  printf(" ..... done\n");
  if (dsave) dsave->cd();
  //  fdir->Purge();
  return;
}

void HistoMan::Save(TFile *file, const char* dirname){
  if(!enabled) return;
  if(!file)return;
  printf("HistoMan::Save ----> Saving %s\n",file->GetName());

  TDirectory *dsave = gDirectory;
  file->cd();

  if (dirname) {
    TDirectoryFile *dir = new TDirectoryFile(dirname, dirname);
    dir->cd();
  }
  for (int i = 0; i < fhist.GetEntries(); i++) {
    TH1 *obj = (TH1 *)fhist.At(i);
    if (obj && obj->GetEntries() > 0) obj->Write();
  }
  printf(" ..... done\n");
  if (dsave) dsave->cd();

  return;
}

void HistoMan::Fill(const char * name, double a,double  b,double w){
  if(!enabled) return;
  TH1*hist = (TH1*) fhist.FindObject(name);
  if(!hist){
    static int nerr = 0;
    if (nerr++ < 20)
      printf("HistoMan::Fill: Error Unknown histo %s\n",name);
    return;
  }

  int ndim = hist->GetDimension();
  if      (ndim == 3 && dynamic_cast<TH3*>(hist)) ((TH3*)hist)->Fill(a,b,w);
  else if (ndim == 2 && dynamic_cast<TH2*>(hist)) ((TH2*)hist)->Fill(a,b,w);
  else if (ndim == 1) {
    if (hist->ClassName() == TString("TProfile") &&
	dynamic_cast<TProfile*>(hist)) ((TProfile*)hist)->Fill(a,b,w);
    else if (dynamic_cast<TH1*>(hist)) ((TH1*)     hist)->Fill(a,b);
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
  delete []bins;
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
  delete []bins;
  delete []binsy;
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
  delete []bins;
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
  delete []bins;
  delete []binsy;
  return hh;
}
//==============================================================
//==============================================================
//==============================================================

#include "TrRecon.h"
#include "TrSim.h"

void HistoMan::BookHistos(int simmode){
  if (!enabled || booked) return;

  TDirectory *dsave = gDirectory;

  // Histograms are booked under memory-based directory 
  // and written in file later when HistoMan::Save is called
  gROOT->cd();
  TDirectory *dir = new TDirectory("HistoMan", "HistoMan");
  dir->cd();
  printf("HistoMan::BookHistos: Directory HistoMan created in memory %d\n",simmode);

  Add(new TH2D("1N_XY","1N_XY",200,-10,10,200,-10,10));
  Add(new TH2D("1N_Y","1N_Y",200,-10,10,200,-10,10));

  Add(new TH2D("P6_XY","1N_XY",200,-2,2,200,-2,2));
  Add(new TH2D("P6_Y","1N_Y",200,-2,2,200,-2,2));

  // Tracker low level
  Add(new TH2D("TrSizeDt", "#DeltaT VS TrSize", 500, 0, 1000, 300, 0, 24000));
  Add(new TH2D("TrNrawLt", "nRaw VS Evt LoDt", 1000, 0,  1e6, 500, 0,  2000));
  Add(new TH2D("TrNclsLt", "nCls VS Evt LoDt", 1000, 0,  1e6, 500, 0,  2000));
  Add(new TH2D("TrNhitLt", "nHit VS Evt LoDt", 1000, 0,  1e6, 500, 0,  2000));
  Add(new TH2D("TrNrawHt", "nRaw VS Evt HiDt", 1000, 0,  1e6, 500, 0,  2000));
  Add(new TH2D("TrNclsHt", "nCls VS Evt HiDt", 1000, 0,  1e6, 500, 0,  2000));
  Add(new TH2D("TrNhitHt", "nHit VS Evt HiDt", 1000, 0,  1e6, 500, 0,  2000));
  Add(new TH1D("TrRecon",  "TrRecon status", 32, 0, 32));
  
  // TrRawCluster
  Add(new TH2D("TrOccRaw","; Ladder Index; Address",192,-0.5,191.5,1024,-0.5,1023.5));

  // TrCluster
  Add(new TH2D("TrClsSigP", "TotSig(P) VS OnTrack", 2, 0, 2, 500, 0, 500));
  Add(new TH2D("TrClsSigN", "TotSig(N) VS OnTrack", 2, 0, 2, 500, 0, 500));
  Add(new TH3D("TrClsStrP", "Rsig(P)", 10, 0, 10, 100, 0, 0.5, 100, 0, 1));
  Add(new TH3D("TrClsStrN", "Rsig(P)", 10, 0, 10, 100, 0, 0.5, 100, 0, 1));
  Add(new TH2D("TrOccSeed","; Ladder Index; Address",192,-0.5,191.5,1024,-0.5,1023.5));
  Add(new TH2D("TrOccStri","; Ladder Index; Address",192,-0.5,191.5,1024,-0.5,1023.5));

  // TrRecHit
  Add(new TH2D("TrLadTrk", "Ladder on track",   33, -16.5, 16.5, 9, 0.5, 9.5));
  Add(new TH2D("TrLadYh",  "Ladder with hitY",  33, -16.5, 16.5, 9, 0.5, 9.5));
  Add(new TH2D("TrLadXYh", "Ladder with hitXY", 33, -16.5, 16.5, 9, 0.5, 9.5));

  double bintm[71];
  bintm[0] = 5e-4;
  for (int i = 0; i <   9; i++) bintm[i+1]  = 1e-3*(i+1);
  for (int i = 0; i <= 60; i++) bintm[i+10] = TMath::Power(10, i*0.05-2);

  // TrTrack
  Add(new TH2D("TrTimH","Tcpu VS nTrHit", 500, 0, 500, 70, bintm));
  Add(new TH2D("TrTimT","Tcpu VS nTrack", 5,   0,   5, 70, bintm));
  Add(TH2D_L("TrNhit","Nhit(Xi/Yi/X/Y)",  5,   0,   5,   10,    0,  10, 0, 0));
  Add(TH2D_L("TrCsqX",  "ChisqX VS RGT", 40, 0.1, 1e3,  120, 1e-2, 1e4, 1, 1));
  Add(TH2D_L("TrCsqY",  "ChisqY VS RGT", 40, 0.1, 1e3,  120, 1e-2, 1e4, 1, 1));
  Add(TH2D_L("TrCsqXh", "ChisqX VS RGT", 40, 0.1, 1e3,  120, 1e-2, 1e4, 1, 1));
  Add(TH2D_L("TrCsqYh", "ChisqY VS RGT", 40, 0.1, 1e3,  120, 1e-2, 1e4, 1, 1));
  Add(TH2D_L("TrCsqXi", "ChisqX VS RGT", 40, 0.1, 1e3,  120, 1e-2, 1e4, 1, 1));
  Add(TH2D_L("TrCsqYi", "ChisqY VS RGT", 40, 0.1, 1e3,  120, 1e-2, 1e4, 1, 1));
  Add(TH2D_L("TrCsqXf", "ChisqX VS RGT", 40, 0.1, 1e3,  120, 1e-2, 1e4, 1, 1));
  Add(TH2D_L("TrCsqYf", "ChisqY VS RGT", 40, 0.1, 1e3,  120, 1e-2, 1e4, 1, 1));
  Add(TH2D_L("TrEriC", "ErRinvC VS RGT", 40, 0.1, 1e3,   80, 1e-2, 1e2, 1, 1));
  Add(TH2D_L("TrResX", "ResX/um VS RGT", 40, 0.1, 1e3, 1000, -5e3, 5e3, 1, 0));
  Add(TH2D_L("TrResY", "ResY/um VS RGT", 40, 0.1, 1e3, 1000, -5e3, 5e3, 1, 0));
  Add(TH2D_L("TrChgP", "ChargeP VS RGT", 40, 0.1, 1e3,  200,    0,  10, 1, 0));
  Add(TH2D_L("TrChgN", "ChargeN VS RGT", 40, 0.1, 1e3,  200,    0,  10, 1, 0));

  Add(new TH2D("TrPtkL1", "tkY VS tkX", 140, -70, 70, 140, -70, 70));
  Add(new TH2D("TrPtkL4", "tkY VS tkX", 140, -70, 70, 140, -70, 70));
  Add(new TH2D("TrPtkL7", "tkY VS tkX", 140, -70, 70, 140, -70, 70));
  Add(new TH2D("TrPtkL8", "tkY VS tkX", 140, -70, 70, 140, -70, 70));
  Add(new TH2D("TrPftL8", "tkY VS tkX", 140, -70, 70, 140, -70, 70));
  Add(new TH2D("TrPtkL9", "tkY VS tkX", 140, -70, 70, 140, -70, 70));
  Add(new TH2D("TrPftL9", "tkY VS tkX", 140, -70, 70, 140, -70, 70));

  Add(new TH2D("TkTrdD0", "TkTRD dy VS dx bef.",   300, 0, 60, 100, 0,  2));
  Add(new TH2D("TkTrdDD", "TkTRD dx aft. VS bef.", 300, 0, 60, 300, 0, 60));
  Add(TH2D_L  ("TkMoveC", "CsqX 2 VS 1", 120, 1e-5, 1e7, 120, 1e-5, 1e7));

  Add(TH2D_L("TrEdx1", "dX VS R L1N", 80, 0.1, 1e3, 200, -10, 10, 1, 0));
  Add(TH2D_L("TrEdy1", "dY VS R L1N", 80, 0.1, 1e3, 200, -10, 10, 1, 0));
  Add(TH2D_L("TrEdx9", "dX VS R L9",  80, 0.1, 1e3, 200, -10, 10, 1, 0));
  Add(TH2D_L("TrEdy9", "dY VS R L9",  80, 0.1, 1e3, 200, -10, 10, 1, 0));

  Add(TH2D_L("TrChfit", "Choutko Fit", 20, 0, 20, 100, 1e-9, 1e1, 0, 1));

  Add(new TH2D("trdmatch","trdmatch",1000,0,100,5,0,5));
  Add(new TH2D("Time","Time",1000,0,50.,500,0,500));

  Add(new TH2D("Time2","Time2",1000,-4,2.,500,0,500));

  // 1/Rigidity
  Add(new TH2D("TrRiIBi", "binv VS 1/R(I)", 400,  -1,  1, 300,  -3,  3));
  Add(new TH2D("TrRiFBi", "binv VS 1/R(F)", 400, -.1, .1, 300,  -3,  3));
  Add(new TH2D("TrRiICs", "sChg VS 1/R(I)", 400,  -1,  1, 800, -20, 20));
  Add(new TH2D("TrRiFCs", "sChg VS 1/R(F)", 400, -.1, .1, 800, -20, 20));
  Add(new TH2D("TrTofDD", "Track-TOF diff", 100,  -5,  5, 100,  -5,  5));

  Add(TH2D_L("TrNclsL", "Ncls on trackL", 100, 1e-2, 1e3, 100, 0, 100, 1, 0));
  Add(TH2D_L("TrNmaxL", "Nmax on trackL", 100, 1e-2, 1e3, 100, 0, 100, 1, 0));
  Add(new TH2D("TrNmxcl","Nmax VS Ncls", 100, 0, 100, 100, 0, 100));

  // Trackfinding
  Add(TH2D_L("TfPsY0", "Pre-sel Y",     100, 1e-4,  10, 140, 1e-6,  10));
  Add(TH2D_L("TfPsY2", "Pre-sel Y",     100, 1e-4,  10, 140, 1e-6,  10));
  Add(TH2D_L("TfPsY1", "Pre-sel Y",     100, 1e-4,  10, 140, 1e-6,  10));
  Add(TH2D_L("TfCsq0", "CsqY1 VS Rgt1", 100, 1e-2, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfCsq1", "CsqY1 VS Rgt1", 100, 1e-2, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfCsq2", "CsqY2 VS Rgt2", 100, 1e-2, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfCsq3", "CsqY1 (No L1)", 100, 1e-2, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfCsq4", "CsqY2 (No L1)", 100, 1e-2, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfCsq5", "CsqY3 VS Rgt3", 100, 1e-2, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfMrg1", "Rmrg VS CsqY1", 100, 1e-7, 1e3, 120, 1e-5, 1e1));
  Add(TH2D_L("TfDty1", "TRD match-Y",   120, 1e-2, 1e4, 100, 1e-7, 1e3));
  Add(TH2D_L("TfPsX1", "Pre-sel X(c)",  120, 1e-2, 1e4, 100, 1e-7, 1e3));
  Add(TH2D_L("TfPsX2", "Pre-sel X(m)",  120, 1e-2, 1e4, 100, 1e-7, 1e3));
  Add(TH2D_L("TfMrg2", "Rmrg VS CsqY2", 100, 1e-7, 1e3, 120, 1e-5, 1e1));
  Add(TH2D_L("TfMrg3", "Rmrg VS CsqX3", 100, 1e-7, 1e3, 120, 1e-5, 1e1));
  Add(TH2D_L("TfMrg4", "Rmrg VS CsqY3", 100, 1e-7, 1e3, 120, 1e-5, 1e1));
  Add(TH2D_L("TfCsq6", "CsqX 4 VS 3",   100, 1e-7, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfCsq7", "CsqY 4 VS 3",   100, 1e-7, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfRgt1", "Rgt1 VS Rgtf",  100, 1e-2, 1e3, 120, 1e-2, 1e4));
  Add(TH2D_L("TfRgt2", "Rgt2 VS Rgtf",  100, 1e-2, 1e3, 120, 1e-2, 1e4));
  Add(TH2D_L("TfCsqf", "CsqY VS CsqY2", 100, 1e-7, 1e3, 140, 1e-3, 1e4));
  Add(TH2D_L("TfCsn0", "ClsSN psel0",   120,   10, 1e4, 120, 1e-2, 1e2));
  Add(TH2D_L("TfCsn1", "ClsSN psel1",   120,   10, 1e4, 120, 1e-2, 1e2));
  Add(TH2D_L("TfCsn2", "ClsSN mrgY",    120,   10, 1e4, 120, 1e-2, 1e2));
  Add(TH2D_L("TfCsn3", "ClsSN mrgX",    120,   10, 1e4, 120, 1e-2, 1e2));
  Add(TH2D_L("TfCsn4", "ClsSN mrgX",    120,   10, 1e4, 120, 1e-2, 1e2));
  Add(TH2D_L("TfCsn5", "ClsSN mrgY",    120,   10, 1e4, 120, 1e-2, 1e2));
  Add(TH2D_L("TfCsn6", "ClsSN ntcl",    120,   10, 1e4, 120, 1e-2, 1e2));
  Add(TH2D_L("XvsR1",  "XvsR1",         100, 1e-2, 1e3, 120, 1e-2, 1e4));
  Add(new TH2D("TfDtm1", "TRD matchDY", 100,  -10,  10, 100,  -10,  10));
  Add(new TH2D("TfDtm2", "TRD matchXY", 100,  -10,  10, 100,  -10,  10));

  // Alignment
  for (int i = 0; i < 9; i++) {
    TString shn = Form("TrRes%d", i+1);
    TString stt = Form("Layer %d res", i+1);
    double rng = (i == 0 || i == 8) ? 2000 : 200;
    Add(new TH3D(shn+"1", stt+"x-X", 20, -60, 60, 20, -1, 1, 200, -rng, rng));
    Add(new TH3D(shn+"2", stt+"y-X", 20, -60, 60, 20, -1, 1, 200, -rng, rng));
    Add(new TH3D(shn+"3", stt+"x-Y", 20, -60, 60, 20, -1, 1, 200, -rng, rng));
    Add(new TH3D(shn+"4", stt+"y-Y", 20, -60, 60, 20, -1, 1, 200, -rng, rng));
  }

  // Reconstruction summary  
  Add(new TH2D("TrSimple","; rec. step; n. candidates",10,-0.5,9.5,100,-0.5,99.5));

  // Charge seed plot
  Add(new TH2D("CSxCSy", "; y charge seed (#sqrt{ADC}); x charge seed (#sqrt{ADC})",200,0,200,200,0,200));
  Add(new TH2D("CSxCSyC","; y charge seed (#sqrt{ADC}); x charge seed (#sqrt{ADC})",200,0,200,200,0,200));
  // Charge seed cut
  Add(new TH2D("AmpyCSy", "; y charge seed (#sqrt{ADC}); y signal (#sqrt{ADC})",200,0,200,200,0,200));
  Add(new TH2D("AmpxCSx", "; x charge seed (#sqrt{ADC}); x signal (#sqrt{ADC})",200,0,200,200,0,200));
  Add(new TH2D("AmpyCSyC","; y charge seed (#sqrt{ADC}); y signal (#sqrt{ADC})",200,0,200,200,0,200));
  Add(new TH2D("AmpxCSxC","; x charge seed (#sqrt{ADC}); x signal (#sqrt{ADC})",200,0,200,200,0,200));
  // Hit compatibility plots
  Add(new TH2D("AmpyAmpx", "; x signal corr. (#sqrt{ADC}); y signal corr. (#sqrt{ADC})",75,5,155,300,0,90));
  Add(new TH2D("ProbAmpx", "; x signal corr. (#sqrt{ADC}); log_{10}(correlation probability)",150,0,150,100,-15,1));
  Add(new TH2D("AmpyAmpxC","; x signal corr. (#sqrt{ADC}); y signal corr. (#sqrt{ADC})",75,5,155,300,0,90));
  // Y Clusters Signal-to-noise ratio
  Add(new TH2D("SeedSNyN","; number of strips; seed SN",10,0.5,10.5,200,0,100));
  Add(new TH2D("SignalyN","; number of strips; cluster signal (ADC)",10,0.5,10.5,200,0,200));
  Add(new TH2D("ClSNyN","; number of strips; cluster SN",10,0.5,10.5,200,0,100));
  // Hits signal compatibility
  Add(new TH2D("ySig0","; previous (#sqrt{ADC}); next (#sqrt{ADC})",200,0,100,200,0,100));
  Add(new TH2D("ySig1","; previous (#sqrt{ADC}); next (#sqrt{ADC})",200,0,100,200,0,100));
  Add(new TH2D("ySig2","; previous (#sqrt{ADC}); next (#sqrt{ADC})",200,0,100,200,0,100));
  Add(new TH2D("ySig3","; previous (#sqrt{ADC}); next (#sqrt{ADC})",200,0,100,200,0,100));
  Add(new TH2D("ySig4","; previous (#sqrt{ADC}); next (#sqrt{ADC})",200,0,100,200,0,100));
  Add(new TH2D("ySig5","; previous (#sqrt{ADC}); next (#sqrt{ADC})",200,0,100,200,0,100));

  // Residuals per layer
  Add(new TH2D("TrResLayx","residual vs layer; layer; residual (#mum)",9,0,9,250,-500,500));
  Add(new TH2D("TrResLayy","residual vs layer; layer; residual (#mum)",9,0,9,250,-500,500));
  // TO-BE-DONE: Residuals versus charge, angle, layer)

  // Plots after reconstruction
  Add(new TH2D("QxQy_final","; Q_{x}; Q_{y}",600,0,30,600,0,30));
  Add(new TH2D("AmpyCSy_final",  "; y charge seed (#sqrt{ADC}); y signal (#sqrt{ADC})",200,0,200,200,0,200));
  Add(new TH2D("AmpxCSx_final",  "; x charge seed (#sqrt{ADC}); x signal (#sqrt{ADC})",200,0,200,200,0,200));
  Add(new TH2D("AmpyCSyC_final", "; y charge seed (#sqrt{ADC}); y signal (#sqrt{ADC})",200,0,200,200,0,200));
  Add(new TH2D("AmpxCSxC_final", "; x charge seed (#sqrt{ADC}); x signal (#sqrt{ADC})",200,0,200,200,0,200));
  Add(new TH2D("AmpyAmpx_final", "; x signal corr. (#sqrt{ADC}); y signal corr. (#sqrt{ADC})",75,5,155,300,0,90));
  Add(new TH2D("AmpyAmpxC_final","; x signal corr. (#sqrt{ADC}); y signal corr. (#sqrt{ADC})",75,5,155,300,0,90));
  Add(new TH2D("ProbAmpx_final", "; x signal corr. (#sqrt{ADC}); log_{10}(correlation probability)",150,0,150,100,-15,1));

  // Sim. reconstruction
  if (simmode & 2) {
    Add(TH2D_L("TrDtyL81", "dyL81 VS Rsim", 40, 1, 1e4, 2000, -5, 5, 1, 0));
    Add(TH2D_L("TrDtyL82", "dyL82 VS Rsim", 40, 1, 1e4, 2000, -5, 5, 1, 0));
    Add(TH2D_L("TrDtyL91", "dyL91 VS Rsim", 40, 1, 1e4, 2000, -5, 5, 1, 0));
    Add(TH2D_L("TrDtyL92", "dyL92 VS Rsim", 40, 1, 1e4, 2000, -5, 5, 1, 0));

    Add(TH2D_L("TrRres11", "d1/R (7-Layer)", 20, 1, 1e4, 500, -25, 25, 1, 0));
    Add(TH2D_L("TrRres12", "dR/R (7-Layer)", 20, 1, 1e4, 500,  -1,  1, 1, 0));
    Add(TH2D_L("TrRres21", "d1/R (w/LTRD)",  20, 1, 1e4, 500, -25, 25, 1, 0));
    Add(TH2D_L("TrRres22", "dR/R (w/LTRD)",  20, 1, 1e4, 500,  -1,  1, 1, 0));
    Add(TH2D_L("TrRres31", "d1/R (w/LEcal)", 20, 1, 1e4, 500, -25, 25, 1, 0));
    Add(TH2D_L("TrRres32", "dR/R (w/LEcal)", 20, 1, 1e4, 500,  -1,  1, 1, 0));
    Add(TH2D_L("TrRres41", "d1/R (9-Layer)", 20, 1, 1e4, 500, -25, 25, 1, 0));
    Add(TH2D_L("TrRres42", "dR/R (9-Layer)", 20, 1, 1e4, 500,  -1,  1, 1, 0));

    printf("HistoMan::BookHistos: histograms for MC-rec booked\n");
  }

  // Sim. generation
  if (simmode & 1) {
    // mceventg
    Add(TH2D_L("Pgen", "Pgen", 7000, -1.,6., 100, -1, 1, 0, 0));
    Add(TH2D_L("Pacc", "Pacc", 7000, -1., 6., 100, -1, 1, 0, 0));
    printf("HistoMan::BookHistos: histograms for MC-sim booked\n");

    // events that were skipped because of exceeded CPU time limit
#ifdef __G4AMS__
#ifndef __ROOTSHAREDLIBRARY__
    double nev=int(GCFLAG.NEVENT/10000.+0.5)*10000;
    if(nev<10000)nev=10000;
    double st=10000/nev/2;
    Add(new TH1F("G4MemoryMB-1", "G4MemoryMB-1", 10000,st,nev+st));
    Add(new TH1F("G4MemoryMB-2", "G4MemoryMB-2", 10000,st,nev+st));
    Add(new TH1F("G4MemoryMB-3", "G4MemoryMB-3", 10000,st,nev+st));
#endif
#endif
    Add(new TH1F("PAll", "PAll", 7000, -1., 6.));
    Add(new TH2F("cputime", "cputime", 1000, -1., 5.,200,0.,5.));
    Add(new TH1F("PAllskipped", "PAllskipped", 7000, -1., 6.));
    Add(new TH2D("TrSimRx", "Sim Xreso VS angX", 50, 0, 50, 100, -100, 100));
    Add(new TH2D("TrSimRy", "Sim Yreso VS angY", 50, 0, 50, 100, -100, 100));

    // intrinsic resolution and signal checks
    Add(new TH2D("TrSimSigx","raw charge vs eta; #eta; raw charge",100,0,1,100,0.5,1));
    Add(new TH2D("TrSimSigy","raw charge vs eta; #eta; raw charge",100,0,1,100,0.5,1));
    Add(new TH2D("TrSimEtax","eta vs angle x; #theta_{XZ} (degrees); #eta_{x}",50,0,1.6,100,0,1));
    Add(new TH2D("TrSimEtay","eta vs angle y; #theta_{YZ} (degrees); #eta_{y}",50,0,1.6,100,0,1));
    Add(new TH2D("TrSimResAx","intr. res. vs angle x; #theta_{XZ} (degrees); x intr. res. (#mum)",50,0,1.6,100,-100,100));
    Add(new TH2D("TrSimResAy","intr. res. vs angle y; #theta_{YZ} (degrees); y intr. res. (#mum)",50,0,1.6,100,-100,100)); 
    Add(new TH2D("TrSimResEx","intr. res. vs edep x; #sqrt{EDep} (#sqrt{keV}); x intr. res. (#mum)",250,0,250,100,-100,100));
    Add(new TH2D("TrSimResEy","intr. res. vs edep y; #sqrt{EDep} (#sqrt{keV}); y intr. res. (#mum)",250,0,250,100,-100,100));
    Add(new TH2D("TrSimEDepx","signal x vs edep; #sqrt{EDep} (#sqrt{keV}); #sqrt{S_{x}} (#sqrt{ADC})",300,0,150,250,0,250));
    Add(new TH2D("TrSimEDepy","signal y vs edep; #sqrt{EDep} (#sqrt{keV}); #sqrt{S_{y}} (#sqrt{ADC})",300,0,150,250,0,250));

    // counting simulated clusters
    Add(new TH1D("TrSimClsx","fake clusters; ladder",192,0,192));
    Add(new TH1D("TrSimClsy","fake clusters; ladder",192,0,192));
    Add(new TH1D("TrSimFakex","fake clusters; ladder",192,0,192));
    Add(new TH1D("TrSimFakey","fake clusters; ladder",192,0,192));

  //--Record TOF MC pulse-max for trigger study
   char histn[1000];
   for(int ilay=0;ilay<4;ilay++){
     for(int ibar=0;ibar<10;ibar++){
       for(int is=0;is<2;is++){
         int barid=1000*(ilay+1)+10*(ibar+1)+(is+1);
         sprintf(histn,"Tof_MIP_Pulse_hight_%d",barid);
         Add(new TH1D(histn,histn,300,0.,3000.));
      }
    }
   }

    printf("HistoMan::BookHistos: histograms for MC-gen booked\n");
  }

  // Back tracing and cutoff
  Add(new TH2F("GgIss",  "ISS pos (GTOD)",  180, -180, 180, 90, -90, 90));
  Add(new TH2F("GmIss",  "ISS pos (GMag)",  180, -180, 180, 90, -90, 90));
  Add(new TH2F("GgCutD", "RcutG(Dipole)",   180, -180, 180, 90, -90, 90));
  Add(new TH2F("GmCutD", "RcutM(Dipole)",   180, -180, 180, 90, -90, 90));
  Add(new TH2F("GgCutS", "RcutG(Stoermer)", 180, -180, 180, 90, -90, 90));
  Add(new TH2F("GmCutS", "RcutM(Stoermer)", 180, -180, 180, 90, -90, 90));
  Add(new TH3F("GgBTS",  "BTG Over",  180, -180, 180, 90, -90, 90, 20, 0, 2));
  Add(new TH3F("GmBTS",  "BTM Over",  180, -180, 180, 90, -90, 90, 20, 0, 2));
  Add(new TH3F("GgBTA",  "BTG Under", 180, -180, 180, 90, -90, 90, 20, 0, 2));
  Add(new TH3F("GmBTA",  "BTM Under", 180, -180, 180, 90, -90, 90, 20, 0, 2));
  Add(new TH3F("GgBTT",  "BTG Trapp", 180, -180, 180, 90, -90, 90, 20, 0, 2));
  Add(new TH3F("GmBTT",  "BTM Trapp", 180, -180, 180, 90, -90, 90, 20, 0, 2));
  Add(new TH2F("GmBTph", "Photons",   180, -180, 180, 90, -90, 90));

  Add(TH2D_L("GmBTrS", "BTM Over",  48, -1.2, 1.2, 60, 0.1, 100, 0, 1));
  Add(TH2D_L("GmBTrA", "BTM Under", 48, -1.2, 1.2, 60, 0.1, 100, 0, 1));
  Add(TH2D_L("GmBTrT", "BTM Trapp", 48, -1.2, 1.2, 60, 0.1, 100, 0, 1));
  Add(TH2D_L("GmBTmS", "BTT Over",  60,  0.1, 100, 80, 0.001, 1, 1, 1));
  Add(TH2D_L("GmBTmA", "BTT Under", 60,  0.1, 100, 80, 0.001, 1, 1, 1));
  Add(TH2D_L("GmBTmT", "BTT Trapp", 60,  0.1, 100, 80, 0.001, 1, 1, 1));

  Add(new TH2D("TvDtfU", "DtofU", 200, -30,  30, 200, -30,  30));
  Add(new TH2D("TvQtfU", "QtofU", 200,   0,   5, 200,   0,   5));
  Add(new TH2D("TvDtrc", "DtrdC", 200, -30,  30, 200, -30,  30));
  Add(new TH2D("TvDtrd", "DtrdT", 200, -30,  30, 200, -30,  30));
  Add(new TH2D("TvLtrd", "Ltrd" ,  20,   0,  20, 200,   0,  40));
  Add(new TH2D("TvNtrl", "NtrdL", 200,   0, 200, 200,   0, 200));
  Add(new TH2D("TvNtrd", "NtrdT",   5,   0,   5, 200,   0, 200));
  Add(new TH2D("TvNtrp", "NtrdP",   5,   0,   5, 200,   0, 200));

  Add(TH2D_L("TvCsqXp", "CsqX(+)", 60, 0.01, 10, 120, 1e-3, 1e3, 1, 1));
  Add(TH2D_L("TvCsqXn", "CsqX(-)", 60, 0.01, 10, 120, 1e-3, 1e3, 1, 1));
  Add(TH2D_L("TvCsqYp", "CsqY(+)", 60, 0.01, 10, 120, 1e-3, 1e3, 1, 1));
  Add(TH2D_L("TvCsqYn", "CsqY(-)", 60, 0.01, 10, 120, 1e-3, 1e3, 1, 1));
  Add(TH2D_L("TvCsqXY", "CsqXY",  120, 1e-3,1e3, 120, 1e-3, 1e3, 1, 1));
  Add(TH2D_L("TvRdfP",  "R1+R2:P", 60, 0.01, 10, 200,   -3,   3, 1, 0));
  Add(TH2D_L("TvDV7P",  "dV0.7:p", 60, 0.01, 10, 120, 1e-3, 1e3, 1, 1));

  Add(TH2D_L("TpCsqXp", "CsqX(+)", 60, 0.01, 10, 120, 1e-3, 1e3, 1, 1));
  Add(TH2D_L("TpCsqXn", "CsqX(-)", 60, 0.01, 10, 120, 1e-3, 1e3, 1, 1));
  Add(TH2D_L("TpCsqYp", "CsqY(+)", 60, 0.01, 10, 120, 1e-3, 1e3, 1, 1));
  Add(TH2D_L("TpCsqYn", "CsqY(-)", 60, 0.01, 10, 120, 1e-3, 1e3, 1, 1));
  Add(TH2D_L("TpCsqXY", "CsqXY",  120, 1e-3,1e3, 120, 1e-3, 1e3, 1, 1));
  Add(TH2D_L("TpRdfP",  "R1+R2:P", 60, 0.01, 10, 200,   -3,   3, 1, 0));
  Add(TH2D_L("TpDV7P",  "dV0.7:p", 60, 0.01, 10, 120, 1e-3, 1e3, 1, 1));

  if (dsave) dsave->cd();
  booked = true;
}
