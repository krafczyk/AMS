#ifndef TRDPDFGRAPH_HH
#define TRDPDFGRAPH_HH

#include <TH1D.h>

namespace Analysis {

/** A helper class to manage the peculiar form of dE/dx pdf histograms.
 *
 * These consist of a delta peak at zero, a well-behaved central part, and an overflow value.
 */
class TrdPdfHistogram : public TH1D {

public:

  TrdPdfHistogram( const char* name, const char* title, Int_t nbinsx, const Double_t* xbin );
  virtual ~TrdPdfHistogram();

  virtual Double_t GetRandom() const;
  virtual double CalculateIntegral();
  virtual double Evaluate( double dedx );

  virtual void SmoothAndNormalize( double Bass, double Span );

  ClassDef(Analysis::TrdPdfHistogram,1)
};
}
#endif // TRDPDFGRAPH_HH
