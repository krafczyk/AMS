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

LxMCcutoff::LxMCcutoff (const char *FileName,TH1D *h, double safetyfactor, LxMCcutoff::eRcutDist dist): SafetyFactor(safetyfactor), RcutDist(dist), RcutI(0), hExp(0), hRcut1D(0) {
  // Filename should contain the TH2F with the Rcut vs Rmax distribution 
  // h is used to get the binning
  // SafetyFactor is the safety factor used to compute the threshold applied to Rrec  
  // dist  is the method used to estimate the  Rcut vs Rmax distribution:
  // _RcutVert : vertical direction in AMS is used to get Rcut
  // _RcutMin25 : Min value within 25 deg is used to get Rcut
  // _RcutMax25 : Max value within 25 deg is used to get Rcut
  // _RcutAcc25 : Isotropically distributed directions within 25 deg is used to get Rcut
  TH1D::AddDirectory(false); 
  TFile *in = TFile::Open(FileName);
  if (!in) { LxMCcutoff::Head = 0; return; }
  TH2F* Rcut=0 ;
  if(RcutDist == LxMCcutoff::_RcutVert) Rcut = (TH2F*) in->Get("hRcut");
  else if(RcutDist == LxMCcutoff::_RcutMin25) Rcut = (TH2F*) in->Get("hRcutMin");
  else if(RcutDist == LxMCcutoff::_RcutMax25) Rcut = (TH2F*) in->Get("hRcutMin");
  else if(RcutDist == LxMCcutoff::_RcutAcc25) Rcut = (TH2F*) in->Get("hRcutAcc");
  //else if(RcutDist == LxMCcutoff::_RcutAccC25) Rcut = (TH2F*) in->Get("hRcutAccE");
  else if(RcutDist == LxMCcutoff::_RcutAccC25) {
    cout << "LxMCcutoff:: option under test, not to be used for the time beeing" << endl;
    exit(-1);
    //Rcut = (TH2F*) in->Get("hRcutAccE");
  }
  delete in;
  // Rcut1d contains the PDF for the true cutoff applied to particles
  hRcut1D = Rcut->ProjectionY("hRcut1D");
  TH1D * hRmax1D0 = Rcut->ProjectionX("hRmax1D0");
  hExp   = (TH1D*) h -> Clone("hExp");
  hExp->Reset();
  hExp->SetTitle("MC Exposure;Rigidity [GV];Exposure time fraction");
  for(int i = 0; i< 1+hRmax1D0->GetNbinsX(); i++) {
    double x1 =hRmax1D0->GetBinLowEdge(i);
    double x2 =hRmax1D0->GetBinLowEdge(i+1);
    for(int ii = 0; ii < 100; ii++) {
      double x = x1 + ii*(x2-x1)/100.;
      //double x = sqrt(x1*x2);
      x *= (1.+SafetyFactor);
      int iMax =  hExp->FindBin(x);
      hExp->AddBinContent(iMax+1,hRmax1D0->GetBinContent(i));
    }
  }
  double sum=0;
  for (int ix = 1; ix < hExp->GetNbinsX()+2; ix++) {
    sum += hExp->GetBinContent(ix);
    hExp->SetBinContent(ix,sum);
  }
  if(sum>0) hExp->Scale(1./sum);

  hExp->SetBinContent(0, hExp->GetBinContent(1));
  
  RcutI = (TH2F*) Rcut->Clone("RcutI");;
  for (int iy = 1; iy < Rcut->GetNbinsY()+2; iy++) {
    double sum=0;
    for (int ix = 1; ix < Rcut->GetNbinsX()+2; ix++) {
      /* code */
      sum += Rcut->GetBinContent(ix,iy);
      RcutI->SetBinContent(ix,iy,sum);
    }
  }
  ExperimentalCutoff = false;
  delete Rcut;

  LxMCcutoff::Head = this;
}

LxMCcutoff::~LxMCcutoff ()
{
  delete RcutI;
  delete hExp;
  delete hRcut1D;

  LxMCcutoff::Head = 0;
}

double LxMCcutoff::GetRcut() { return hRcut1D->GetRandom(); }
double LxMCcutoff::GetRmax(double Rcut) {
  // Sample Rmax
  if(RcutDist == LxMCcutoff::_RcutMax25) return Rcut;
  int nx = RcutI->GetXaxis()->GetNbins();
  int biny = RcutI->GetYaxis()->FindBin(Rcut);
  float *y = RcutI->GetArray()+(nx+2)*(biny);
  float p = RcutI->GetBinContent(nx+1,biny);
  float r1 = gRandom->Rndm()*p;
  int ibin = TMath::BinarySearch(nx+1,y,r1);
  double Rmax = RcutI->GetXaxis()->GetBinLowEdge(ibin+1);
  Rmax += RcutI->GetXaxis()->GetBinWidth(ibin+1)*(r1-y[ibin])/(y[ibin+1] - y[ibin]);

  return Rmax;
}

TH2F *LxMCcutoff::GetEff( double N0) {
  TH2F *h2 = new TH2F("hEff","hEff;R_{GEN} [GV];R_{REC} [GV]", hExp->GetNbinsX(), hExp->GetXaxis()->GetXbins()->GetArray(),
		      hExp->GetNbinsX(), hExp->GetXaxis()->GetXbins()->GetArray());

  //int nx = RcutI->GetXaxis()->GetNbins();
  for(int ii = 0; ii < hExp->GetNbinsX()+2; ii++) {
    double Rgen = hExp->GetXaxis()->GetBinCenterLog(ii);
    //cout << Rgen << endl;
    if(Rgen<30*(1+SafetyFactor)) {
      for (int j = 0; j < N0; j++) {
	// Sample Rcut
	double Rcut = GetRcut();
	if(Rgen<Rcut) continue;

	// Sample Rmax
	double Rmax = GetRmax(Rcut);
	int iMax =  hExp->FindBin(Rmax*(1+SafetyFactor));
	for(int jj = iMax+1; jj < h2->GetNbinsY()+2; jj++) {
	  h2->Fill(h2->GetXaxis()->GetBinCenter(ii),h2->GetYaxis()->GetBinCenter(jj),
		   1./hExp->GetBinContent(jj)/N0);
	}
      }
    } else {
      for(int jj = 0; jj < h2->GetNbinsY()+2; jj++) {
	h2->Fill(h2->GetXaxis()->GetBinCenter(ii),h2->GetYaxis()->GetBinCenter(jj),1.);
      }
    }
  }
  h2->SetMaximum(1.01);
  return h2;
}

double LxMCcutoff::GetWeight(double Rgen,double Rrec,int randseed)
{
  Rrec = fabs(Rrec);
  int binyy = hRcut1D->GetXaxis()->FindBin(Rgen);
  // Estimate the probability for the particle to be above cutoff
  double Prob =  hRcut1D->Integral(1,binyy)/hRcut1D->Integral(); 
  // Sample N (Rcut,Rth) for this particle 
  int N = TMath::Nint(1.4/Prob);
  //int N = 1;
  // Associated weight for each sample
  double w0 = 1./((double) N);
  double Weight=0;
  TRandom *rsave = gRandom;
  if (randseed > 0) gRandom = new TRandom3(randseed);

  for (int i = 0; i < N; i++) {
    // Sample Rcut
    double Rcut = GetRcut();
    // Sample Rmax
    double Rmax = GetRmax(Rcut);

    // Estimate Rthres from Rmax
    int iMax =  hExp->FindBin(Rmax*(1+SafetyFactor));
    double Rthres = hExp->GetBinLowEdge(iMax+1);
    // Fill hExp0, to be used to compute exposure time fraction
    //hExp0->AddBinContent(iMax+1,w0);
    // True rigidity should be above Rcut and measured above Rthres
    //if(iMax>0) {

    if(Rgen>Rcut && Rrec>Rthres) Weight+=w0;
    //cout << " Rgen " << hExp->GetBinContent(iMax) << endl;
    //if(Rgen>Rcut && Rrec>Rthres) Weight+=w0;
    //}
  }                   
  if (gRandom != rsave) delete gRandom;
  gRandom = rsave;
  int i0 =  hExp->FindBin(fabs(Rrec));
  if (hExp->GetBinContent(i0)==0) return 0;
  return Weight/hExp->GetBinContent(i0);
}




/*
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
    // Fill hExp0, to be used to compute exposure time fraction
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
  //hExp->SetBit(TH1::kIsAverage);
  return hExp;
}
*/

//////////////////////////////////////////////////////////////////////////
ClassImp(QBincutoff);

//========================================================
int  QBincutoff::Init(){
  qmargin=1.2;
  qcutoffset=1.2;
  qhev=0;
  qhexp=0;
  qsexp=0;
  return 0;
}

//========================================================
int  QBincutoff::Clear(){
  if(qsexp)delete qsexp;
   qsexp=0;
   return 0;
}

//========================================================
QBincutoff::QBincutoff(){
  Init();
  TFile *f=TFile::Open(Form("%s/v5.01/Expotime_Cutoff20120520_20131126.root",getenv("AMSDataDir")));
  qhexp=(TH1 *)f->Get("ExpoTime_Cutoff");
//---cutoff time-spline
  qsexp=new TSpline3(qhexp);
}

//========================================================
QBincutoff::QBincutoff(const char *fname){
  Init();
  TFile *f=TFile::Open(fname);
  qhexp=(TH1 *)f->Get("ExpoTime_Cutoff");
//---cutoff time-spline
  qsexp=new TSpline3(qhexp);
}
 
//========================================================
QBincutoff::QBincutoff(TH1* hexp){
  Init();
  qhexp=hexp;
//---cutoff time-spline
  qsexp=new TSpline3(qhexp);
}

//========================================================
double QBincutoff::GetMCTimeWeight(double lbv,double Rgen,double margin,double cutoffoffset){
//----Based on Measured-Rigidity
  static double tww=1;
  if      (Rgen<0.7)return 0;//no events below 0.7GV(due to cutoff)
  else  if(Rgen>30)return 1;//cutoff<30GV
  double lbvmax=58; //58GV Max-onebin
  double lbv1=fabs(lbv)>lbvmax?lbvmax:fabs(lbv);
  double mescutoff=lbv1/margin;//LowEdge
  double gencutoff=mescutoff/cutoffoffset;//Generate Lower
  if(Rgen<gencutoff) {//
     tww=qsexp->Eval(Rgen*cutoffoffset)/qsexp->Eval(gencutoff*cutoffoffset);
  }
  else tww=1;
  return tww;
}

//========================================================
double QBincutoff::GetMCTimeWeight(TH1 *hev,double Rrec,double Rgen,double margin,double cutoffoffset){
 double Rrec1=fabs(Rrec);
 static TH1 *hevpr=0;
 static double lbvpr=0,hbvpr=0;
 if(hev!=qhev||Rrec1<lbvpr||Rrec1>hbvpr){//refind cutoff
     int ibrc=hev->FindBin(Rrec1);
     lbvpr=hev->GetBinLowEdge(ibrc);
     hbvpr=hev->GetXaxis()->GetBinUpEdge(ibrc);
     if(ibrc==hev->GetNbinsX())hbvpr=1000*Rrec1;//Last-bin
     qhev=hev;
  }
  return GetMCTimeWeight(lbvpr,Rgen,margin,cutoffoffset);
}


//========================================================
