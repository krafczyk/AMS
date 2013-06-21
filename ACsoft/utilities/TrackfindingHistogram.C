
#include "TrackfindingHistogram.hh"

#include "TMath.h"

#define DEBUG 0
#include <debugging.hh>

Utilities::TrackfindingHistogram::TrackfindingHistogram( const char* name, const char* title,
                                                         Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup, Double_t maxThreshold ) :
  TH2F(name,title,nbinsx,xlow,xup,nbinsy,ylow,yup),
  fMaximumCandidateThreshold(maxThreshold)
{

  fMaximaCandidates.reserve(100);

  fBinwidthX = GetXaxis()->GetBinWidth(1);
  fBinwidthY = GetYaxis()->GetBinWidth(1);
  fminX = GetXaxis()->GetXmin();
  fmaxX = GetXaxis()->GetXmax();
  fminY = GetYaxis()->GetXmin();
  fmaxY = GetYaxis()->GetXmax();
}

void Utilities::TrackfindingHistogram::Reset(Option_t *option) {

  fMaximaCandidates.clear();

  TH2F::Reset(option);
}

void Utilities::TrackfindingHistogram::FillFast( Double_t x, Double_t y ) {

  Int_t binx = FindBinX(x);
  Int_t biny = FindBinY(y);

  Int_t bin = TH1::GetBin(binx,biny);

  Double_t binContent = GetBinContent(bin);
  Double_t binContentAfterFill = binContent + 1.0;

  // only add bin above threshold to list of maximum candidates exactly once, exclude under-/overflow bins
  if( binx > 0 && binx <= GetNbinsX() && biny > 0 && biny <= GetNbinsY() &&
      fabs( binContentAfterFill - fMaximumCandidateThreshold ) < 0.99 ){
    DEBUG_OUT << GetName() << ": Adding candidate bin (" << binx << "," << biny << ")" << std::endl;
    fMaximaCandidates.push_back(std::pair<int,int>(binx,biny));
  }

  SetBinContent(bin,binContentAfterFill);
}


/** This will only look for a maximum in the bins that reached the threshold \p fMaximumCandidateThreshold at some point during the filling of the histogram! */
Double_t Utilities::TrackfindingHistogram::FindMaximumBinFast( Int_t& maxbinx, Int_t& maxbiny ) const {

  DEBUG_OUT << GetName() << ": Searching " << fMaximaCandidates.size() << " candidate bins..." << std::endl;

  if( fMaximaCandidates.size() == 0) {

//    // resort to slow method
//    Int_t dummyZ;
//    TH1::GetMaximumBin(maxbinx,maxbiny,dummyZ);
//    return GetBinContent(maxbinx,maxbiny);

    maxbinx = 0;
    maxbiny = 0;
    return 0.;
  }

  unsigned int maxIndex = 0;
  Double_t maxValue = 0.;
  for( unsigned int index = 0 ; index < fMaximaCandidates.size() ; ++index ) {

    const std::pair<int,int>& bins = fMaximaCandidates[index];
    Int_t bin = TH1::GetBin(bins.first,bins.second);
    Double_t binc = GetBinContent(bin);
    if( binc > maxValue ){
      maxIndex = index;
      maxValue = binc;
    }
  }

  const std::pair<int,int>& bins = fMaximaCandidates[maxIndex];
  maxbinx = bins.first;
  maxbiny = bins.second;

  DEBUG_OUT << "Maximum " << maxValue << " found at bin (" << maxbinx << "," << maxbiny << ")" << std::endl;

  return maxValue;
}


Int_t Utilities::TrackfindingHistogram::FindBinX( Double_t x ) const {

  if( x < fminX ) return 0;
  if( x > fmaxX ) return GetNbinsX()+1;

  return TMath::FloorNint( ( x - fminX ) / fBinwidthX ) + 1;
}

Int_t Utilities::TrackfindingHistogram::FindBinY( Double_t y ) const {

  if( y < fminY ) return 0;
  if( y > fmaxY ) return GetNbinsY()+1;

  return TMath::FloorNint( ( y - fminY ) / fBinwidthY ) + 1;
}


ClassImp(Utilities::TrackfindingHistogram)

