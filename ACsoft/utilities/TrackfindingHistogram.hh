#ifndef TRACKFINDINGHISTOGRAM_HH
#define TRACKFINDINGHISTOGRAM_HH

#include <TH2.h>
#include <vector>

namespace Utilities {

/** A 2D histogram with equidistant binning optimized for fast filling and fast finding of local maxima. */
class TrackfindingHistogram : public TH2F
{

 public:

  TrackfindingHistogram( const char* name, const char* title,
                         Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup, Double_t maxThreshold );

  virtual void Reset(Option_t *option="");

  void FillFast( Double_t x, Double_t y );

  Double_t FindMaximumBinFast( Int_t& maxbinx, Int_t& maxbiny ) const;

  Int_t FindBinX( Double_t x ) const;
  Int_t FindBinY( Double_t y ) const;

private:

  std::vector<std::pair<int,int> > fMaximaCandidates;
  Double_t fMaximumCandidateThreshold;

  Double_t fBinwidthX;
  Double_t fminX;
  Double_t fmaxX;
  Double_t fBinwidthY;
  Double_t fminY;
  Double_t fmaxY;

  ClassDef(Utilities::TrackfindingHistogram,1)
};

}

#endif // TRACKFINDINGHISTOGRAM_HH
