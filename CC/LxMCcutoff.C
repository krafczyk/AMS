//  $Id$

#include "LxMCcutoff.h"
#include "TFile.h"
#include "TH2.h"
#include "TMath.h"
#include "TRandom3.h"

#include <iostream>
#include <cstdlib>
using namespace std;

ClassImp(LxMCcutoff);


LxMCcutoff *LxMCcutoff::Head = 0;
TH1D *LxMCcutoff::Hbin = 0;

LxMCcutoff::LxMCcutoff (const char *FileName,TH1D *h, double safetyfactor, LxMCcutoff::eRcutDist dist ): SafetyFactor(safetyfactor), RcutDist(dist), RcutI(0),hExp0(0), hExp(0), Rcut1D(0) {
  // Filename should contain the TH2F with the Rcut vs Rmax distribution 
  // h is used to get the binning
  // SafetyFactor is the safety factor used to compute the threshold applied to Rrec  
  // dist  is the method used to estimate the  Rcut vs Rmax distribution:
  // _RcutVert : vertical direction in AMS is used to get Rcut
  // _RcutMin25 : Min direction within 25 deg is used to get Rcut
  // _RcutIso25 : Isotropically distributed directions within 25 deg is used to get Rcut (not implemented yet)
  TH1D::AddDirectory(false); 
  hExp0  =  (TH1D*) h -> Clone("hExp0");
  hExp0->Reset();
  hExp   = (TH1D*) h -> Clone("hExp");
  hExp->Reset();
  hExp->SetTitle("MC Exposure;Rigidity [GV];Exposure time fraction");
  TFile *in = TFile::Open(FileName);
  if (!in) { LxMCcutoff::Head = 0; return; }
  TH2F* Rcut ;
  if(RcutDist == LxMCcutoff::_RcutVert) Rcut = (TH2F*) in->Get("hRcut");
  else if(RcutDist == LxMCcutoff::_RcutMin25) Rcut = (TH2F*) in->Get("hRcutMin");
  else if(RcutDist == LxMCcutoff::_RcutIso25) {
    cout << "LxMCcutoff: _RcutIso25 option not implemented yet" << endl;
    exit(-1);
  }
  delete in;
  Rcut1D = Rcut->ProjectionY("Rcut1D");
  RcutI = (TH2F*) Rcut->Clone("RcutI");;
  for (int iy = 1; iy < Rcut->GetNbinsY()+2; iy++) {
    double sum=0;
    for (int ix = 1; ix < Rcut->GetNbinsX()+2; ix++) {
      /* code */
      sum += Rcut->GetBinContent(ix,iy);
      RcutI->SetBinContent(ix,iy,sum);
    }
  }
  delete Rcut;

  LxMCcutoff::Head = this;
}

LxMCcutoff::~LxMCcutoff ()
{
  delete RcutI;
  delete hExp;
  delete hExp0;
  delete Rcut1D;

  LxMCcutoff::Head = 0;
}

double LxMCcutoff::GetWeight(double Rgen,double Rrec,int randseed)
{
  Rrec = fabs(Rrec);
  int binyy = Rcut1D->GetXaxis()->FindBin(Rgen);
  // Estimate the probability for the particle to be above cutoff
  double Prob =  Rcut1D->Integral(1,binyy)/Rcut1D->Integral(); 
  // Sample N (Rcut,Rth) for this particle 
  int N = TMath::Nint(1.4/Prob);
  // Associated weight for each sample
  double w0 = 1./((double) N);
  int nx = RcutI->GetXaxis()->GetNbins();
  double Weight=0;
  TRandom *rsave = gRandom;
  if (randseed > 0) gRandom = new TRandom3(randseed);
  for (int i = 0; i < N; i++) {
    // Sample Rcut
    double Rcut = Rcut1D->GetRandom();
    // Sample Rmax
    int biny = RcutI->GetYaxis()->FindBin(Rcut);
    float *y = RcutI->GetArray()+(nx+2)*(biny);
    float p = RcutI->GetBinContent(nx+1,biny);
    float r1 = gRandom->Rndm()*p;
    int ibin = TMath::BinarySearch(nx+1,y,r1);
    double Rmax = RcutI->GetXaxis()->GetBinLowEdge(ibin+1);
    if (r1 > y[ibin]) Rmax += RcutI->GetXaxis()->GetBinWidth(ibin+1)*(r1-y[ibin])/(y[ibin+1] - y[ibin]);
    // Estimate Rthres from Rmax
    int iMax =  hExp->FindBin(Rmax*(1+SafetyFactor));
    double Rthres = hExp->GetBinLowEdge(iMax+1);
    // Fill hExp0 to be used to compute exposure time fraction
    hExp0->AddBinContent(iMax+1,w0);
    // True rigidity should be above Rcut and measured above Rthres
    if(Rgen>Rcut && Rrec>Rthres) Weight+=w0;
  }                   
  if (gRandom != rsave) delete gRandom;
  gRandom = rsave;
  return Weight;
}

TH1D* LxMCcutoff::GetExp()
{ 
  hExp->Reset();
  double sum=0;
  for (int ix = 1; ix < hExp->GetNbinsX()+2; ix++) {
    sum += hExp0->GetBinContent(ix);
    hExp->SetBinContent(ix,sum);
  }
  if(sum>0) hExp->Scale(1./sum);
  else {
    for (int ix = 1; ix < hExp->GetNbinsX()+2; ix++) {
      hExp->SetBinContent(ix,1.);
    }
  }
  hExp->SetBit(TH1::kIsAverage);
  return hExp;
}
