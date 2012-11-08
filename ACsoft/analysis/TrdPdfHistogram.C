#include "TrdPdfHistogram.hh"

#include <TGraphSmooth.h>
#include <TRandom.h>
#include <TGraph.h>
#include <TMath.h>

#define DEBUG 0
#define INFO_OUT_TAG "TrdPdfHistogram> "
#include <debugging.hh>

Analysis::TrdPdfHistogram::TrdPdfHistogram( const char* name, const char* title, Int_t nbinsx, const Double_t* xbin ) :
  TH1D(name,title,nbinsx,xbin)
{
}

Analysis::TrdPdfHistogram::~TrdPdfHistogram() { }

Double_t Analysis::TrdPdfHistogram::GetRandom() const {

  Double_t rand = TH1D::GetRandom();
  if( rand < GetXaxis()->GetBinUpEdge(1) ) return 0.0;
  if( rand > GetXaxis()->GetBinLowEdge(GetNbinsX()) ) return GetBinCenter(GetNbinsX());

  return rand;
}

double Analysis::TrdPdfHistogram::CalculateIntegral() {

  // treat first and last bin (zero entries and overflow) as delta peaks
  double integral = GetBinContent(1);
  integral += TH1D::Integral(2,GetNbinsX()-1,"width");
  integral += GetBinContent(GetNbinsX());

  return integral;
}

double Analysis::TrdPdfHistogram::Evaluate( double dedx ){

  if( dedx < GetXaxis()->GetBinUpEdge(1) ) return GetBinContent(1);
  if( dedx > GetXaxis()->GetBinLowEdge(GetNbinsX()) ) return GetBinContent(GetNbinsX());


  // perform binary search for bracketing points
  const TArrayD* bins = GetXaxis()->GetXbins();
  Long64_t iLow = TMath::BinarySearch(bins->GetSize(), bins->GetArray(), dedx);

  // linear interpolation between bins as needed
  int binNumber = iLow +1;
  double binContent = GetBinContent(binNumber);

  if( binNumber > 1 && binNumber < GetNbinsX() ){

    double x1 = 0.;
    double x2 = 0.;
    double y1 = 0.;
    double y2 = 0.;

    double binCenter = GetBinCenter(binNumber);
    if( dedx < binCenter && dedx >= GetBinLowEdge(binNumber) ){

      x2 = binCenter;
      y2 = binContent;
      x1 = GetBinCenter(binNumber-1);
      y1 = GetBinContent(binNumber-1);
    }
    else if( dedx >= binCenter && dedx <= GetXaxis()->GetBinUpEdge(binNumber) ){

      x1 = binCenter;
      y1 = binContent;
      x2 = GetBinCenter(binNumber+1);
      y2 = GetBinContent(binNumber+1);

      // last bin before overflow:
      if( binNumber == GetNbinsX()-1 ){
        x2 = x1;
        y2 = y1;
      }
    }
    else{
      WARN_OUT << "This should never happen..." << std::endl;
    }

    // linear interpolation
    double value = ( x1 == x2 ? y1 : (y2*(dedx-x1) - y1*(dedx-x2)) / (x2-x1) );

    return value;
  }

  return binContent;
}

void Analysis::TrdPdfHistogram::SmoothAndNormalize( double Bass, double Span ) {

  TGraph* mainPdfGraph = new TGraph;
  int pointCtr = 0;

  int firstNonzeroBin = 1;
  int lastNonzeroBin = GetNbinsX();

  for( int i=2 ; i<=GetNbinsX()-1 ; ++i ){

    if( GetBinContent(i) > 0.0 ){
      firstNonzeroBin = i;
      break;
    }
  }

  for( int i=GetNbinsX()-1 ; i > firstNonzeroBin ; --i ){

    if( GetBinContent(i) > 0.0 ){
      lastNonzeroBin = i;
      break;
    }
  }

  for( int i=firstNonzeroBin ; i<=lastNonzeroBin ; ++i ){
    mainPdfGraph->SetPoint(pointCtr,GetBinCenter(i),GetBinContent(i));
    ++pointCtr;
  }


  // smoothing
  TGraphSmooth *gs = new TGraphSmooth("normal");
  TGraph *grSmoothPDF = gs->SmoothSuper(mainPdfGraph,"",Bass,Span);

  double chi2 = 0.0;

  for( int i=0 ; i<grSmoothPDF->GetN() ; ++i) {

    double x,y;
    grSmoothPDF->GetPoint(i,x,y);

    int iBin = FindBin(x);
    double pull = ( GetBinContent(iBin)-y ) / GetBinError(iBin);
    chi2 += pull*pull;

    SetBinContent(FindBin(x),y);
  }

  DEBUG_OUT << "chi2= " << chi2 << std::endl;

  delete gs; gs = 0;
  delete grSmoothPDF; grSmoothPDF = 0;

  // normalize
  Sumw2();
  Scale(1.0/CalculateIntegral());

}

ClassImp(Analysis::TrdPdfHistogram)
