#include <math.h>
#include "HistoMan.h"
#include "TFile.h"
#include "TH3.h"
#include "TROOT.h"
#include "TDirectoryFile.h"
#include "tkdcards.h"
ClassImp(HistoMan)
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

void HistoMan::Save(TFile *file){
  if(!enabled) return;
  if(!file)return;
  printf("HistoMan::Save ----> Saving %s\n",file->GetName());

  TDirectory *dsave = gDirectory;
  file->cd();
  TDirectoryFile *dir = new TDirectoryFile("HistoMan", "HistoMan");
  dir->cd();
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
#pragma omp critical (hmanfill)
{
  if( hist->InheritsFrom("TH3"))
    ((TH3*)hist)->Fill(a,b,w);
  else if( hist->InheritsFrom("TH2"))
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
#include "TrSim.h"

void HistoMan::BookHistos(int simmode){
  if (!enabled || booked) return;

  TDirectory *dsave = gDirectory;

  // Histograms are booked under memory-based directory 
  // and written in file later when HistoMan::Save is called
  gROOT->cd();
  TDirectory *dir = new TDirectory("HistoMan", "HistoMan");
  dir->cd();
  printf("HistoMan::BookHistos: Directroy HistoMan created in memory\n");

  Add(new TH2D("1N_XY","1N_XY",200,-10,10,200,-10,10));
  Add(new TH2D("1N_Y","1N_Y",200,-10,10,200,-10,10));

  Add(new TH2D("P6_XY","1N_XY",200,-2,2,200,-2,2));
  Add(new TH2D("P6_Y","1N_Y",200,-2,2,200,-2,2));

  // Tracker low level
  Add(new TH2D("TrSizeDt", "#DeltaT VS TrSize", 500, 0, 1000, 300, 0, 24000));
  Add(new TH2D("TrNrawLt", "nRaw VS Evt LoDt", 1000, 0,  1e6, 500, 0,  1000));
  Add(new TH2D("TrNclsLt", "nCls VS Evt LoDt", 1000, 0,  1e6, 500, 0,  1000));
  Add(new TH2D("TrNhitLt", "nHit VS Evt LoDt", 1000, 0,  1e6, 500, 0,  1000));
  Add(new TH2D("TrNrawHt", "nRaw VS Evt HiDt", 1000, 0,  1e6, 500, 0,  1000));
  Add(new TH2D("TrNclsHt", "nCls VS Evt HiDt", 1000, 0,  1e6, 500, 0,  1000));
  Add(new TH2D("TrNhitHt", "nHit VS Evt HiDt", 1000, 0,  1e6, 500, 0,  1000));
  Add(new TH1D("TrRecon",  "TrRecon status", 32, 0, 32));

  // TrCluster
  Add(new TH2D("TrClsSigP", "Amplitude(P) VS OnTrack", 2, 0, 2, 500, 0, 500));
  Add(new TH2D("TrClsSigN", "Amplitude(N) VS OnTrack", 2, 0, 2, 500, 0, 500));

  // TrRecHit
  Add(new TH2D("TrLadTrk", "Ladder on track",   33, -16.5, 16.5, 9, 0.5, 9.5));
  Add(new TH2D("TrLadYh",  "Ladder with hitY",  33, -16.5, 16.5, 9, 0.5, 9.5));
  Add(new TH2D("TrLadXYh", "Ladder with hitXY", 33, -16.5, 16.5, 9, 0.5, 9.5));

  // TrTrack
  Add(TH2D_L("TrTimH","Tcpu VS nTrHit", 500,   0, 1e3,  140, 1e-3, 1e4, 0, 1));
  Add(TH2D_L("TrTimT","Tcpu VS nTrack",   5,   0,   5,  140, 1e-3, 1e4, 0, 1));
  Add(TH2D_L("TrNhit","Nhit(Xi/Yi/X/Y)",  5,   0,   5,   10,    0,  10, 0, 0));
  Add(TH2D_L("TrCsqX",  "ChisqX VS RGT", 40, 0.1, 1e3,  120, 1e-2, 1e4, 1, 1));
  Add(TH2D_L("TrCsqY",  "ChisqY VS RGT", 40, 0.1, 1e3,  120, 1e-2, 1e4, 1, 1));
  Add(TH2D_L("TrResX", "ResX/um VS RGT", 40, 0.1, 1e3, 1000, -5e3, 5e3, 1, 0));
  Add(TH2D_L("TrResY", "ResY/um VS RGT", 40, 0.1, 1e3, 1000, -5e3, 5e3, 1, 0));
  Add(TH2D_L("TrChgP", "ChargeP VS RGT", 40, 0.1, 1e3,  500,    0, 500, 1, 0));
  Add(TH2D_L("TrChgN", "ChargeN VS RGT", 40, 0.1, 1e3,  500,    0, 500, 1, 0));

  Add(new TH2D("TrPtkL8", "tkY VS tkX", 140, -70, 70, 140, -70, 70));
  Add(new TH2D("TrPftL8", "tkY VS tkX", 140, -70, 70, 140, -70, 70));
  Add(new TH2D("TrPtkL9", "tkY VS tkX", 140, -70, 70, 140, -70, 70));
  Add(new TH2D("TrPftL9", "tkY VS tkX", 140, -70, 70, 140, -70, 70));

  Add(new TH3D("TrAlg81", "rx VS x", 20, -50, 50, 12, -.6, .6, 500, -.5, .5));
  Add(new TH3D("TrAlg82", "rx VS y", 20, -50, 50, 12, -.6, .6, 500, -.5, .5));
  Add(new TH3D("TrAlg83", "ry VS x", 20, -50, 50, 12, -.6, .6, 500, -.5, .5));
  Add(new TH3D("TrAlg84", "ry VS y", 20, -50, 50, 12, -.6, .6, 500, -.5, .5));

  Add(new TH3D("TrAlg91", "rx VS x", 20, -50, 50, 12, -.6, .6, 500, -.5, .5));
  Add(new TH3D("TrAlg92", "rx VS y", 20, -50, 50, 12, -.6, .6, 500, -.5, .5));
  Add(new TH3D("TrAlg93", "ry VS x", 20, -50, 50, 12, -.6, .6, 500, -.5, .5));
  Add(new TH3D("TrAlg94", "ry VS y", 20, -50, 50, 12, -.6, .6, 500, -.5, .5));

  Add(new TH2F("TkTrdD0", "TkTRD dy VS dx bef.",   300, 0, 60, 100, 0,  2));
  Add(new TH2F("TkTrdDD", "TkTRD dx aft. VS bef.", 300, 0, 60, 300, 0, 60));
  Add(TH2D_L  ("TkMoveC", "CsqX 2 VS 1", 120, 1e-5, 1e7, 120, 1e-5, 1e7));

  Add(TH2D_L("TrEdx1", "dX VS R L1N", 80, 0.1, 1e3, 200, -10, 10, 1, 0));
  Add(TH2D_L("TrEdy1", "dY VS R L1N", 80, 0.1, 1e3, 200, -10, 10, 1, 0));
  Add(TH2D_L("TrEdx9", "dX VS R L9",  80, 0.1, 1e3, 200, -10, 10, 1, 0));
  Add(TH2D_L("TrEdy9", "dY VS R L9",  80, 0.1, 1e3, 200, -10, 10, 1, 0));

  Add(TH2D_L("TrChfit", "Choutko Fit", 20, 0, 20, 100, 1e-9, 1e1, 0, 1));

  Add(new TH2F("trdmatch","trdmatch",1000,0,100,5,0,5));
  Add(new TH2F("Time","Time",1000,0,50.,500,0,500));

  Add(new TH2F("Time2","Time2",1000,-4,2.,500,0,500));

  // Track quality parameters VS Rigidity

  // Trackfinding
  Add(TH2D_L("TfPsY0", "Pre-sel Y",     100, 1e-4,  10, 140, 1e-6,  10));
  Add(TH2D_L("TfPsY2", "Pre-sel Y",     100, 1e-4,  10, 140, 1e-6,  10));
  Add(TH2D_L("TfPsY1", "Pre-sel Y",     100, 1e-4,  10, 140, 1e-6,  10));
  Add(TH2D_L("TfCsq0", "CsqY1 VS Rgt1", 100, 1e-2, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfCsq1", "CsqY1 VS Rgt1", 100, 1e-2, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfCsq2", "CsqY2 VS Rgt2", 100, 1e-2, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfCsq3", "CsqY1 (No L1)", 100, 1e-2, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfCsq4", "CsqY2 (No L1)", 100, 1e-2, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfCsq5", "CsqY2 VS X0",   100, -50,   50, 100, 1e-7, 1e3, 0));
  Add(TH2D_L("TfCsq6", "CsqY2 VS dX",   100,  -1,    1, 100, 1e-7, 1e3, 0));
  Add(TH2D_L("TfMrg1", "Rmrg VS CsqY1", 100, 1e-7, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfPsX1", "Pre-sel X(c)",  120, 1e-2, 1e4, 100, 1e-7, 1e3));
  Add(TH2D_L("TfPsX2", "Pre-sel X(m)",  120, 1e-2, 1e4, 100, 1e-7, 1e3));
  Add(TH2D_L("TfMrg2", "Rmrg VS CsqY2", 100, 1e-7, 1e3, 100, 1e-7, 1e3));
  Add(TH2D_L("TfRgt1", "Rgt1 VS Rgtf",  100, 1e-2, 1e3, 120, 1e-2, 1e4));
  Add(TH2D_L("TfRgt2", "Rgt2 VS Rgtf",  100, 1e-2, 1e3, 120, 1e-2, 1e4));
  Add(TH2D_L("TfCsqf", "CsqY VS CsqY2", 100, 1e-7, 1e3, 140, 1e-3, 1e4));

  // residuals vs ladder
  Add(new TH2F("TrResLayx","residual vs layer; layer; residual (#mum)",9,0,9,250,-500,500));
  Add(new TH2F("TrResLayy","residual vs layer; layer; residual (#mum)",9,0,9,250,-500,500));

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
    Add(TH1D_L("Pgen", "Pgen", 100, 1e-1, 1e4));
    Add(TH1D_L("Pacc", "Pacc", 100, 1e-1, 1e4));

    Add(new TH2F("TrSimRx", "Sim Xreso VS angX", 50, 0, 50, 100, -100, 100));
    Add(new TH2F("TrSimRy", "Sim Yreso VS angY", 50, 0, 50, 100, -100, 100));

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

    printf("HistoMan::BookHistos: histograms for MC-gen booked\n");
  }

  if (dsave) dsave->cd();
  booked = true;
}
