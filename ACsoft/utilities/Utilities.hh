#ifndef UTILITIES_HH
#define UTILITIES_HH

#include <vector>
#include <math.h>

#include <TH1.h>
#include <TVector3.h>
namespace Utilities {

std::vector<double> GenerateLogBinning(int nBinLog, double Tmin, double Tmax);

  TVector3 TransformAMS02toMAG(const double &yaw, const double &pitch, const double &roll, const double &altitude, const double &phi, const double &theta, const double &velPhi, const double &velTheta, const TVector3 &inputVector);

template<typename TwoDHisto>
static void NormalizeHistogramXSlices(TwoDHisto* hist) {

  int nBinX = hist->GetNbinsX();
  int nBinY = hist->GetNbinsY();
  const char* hName = hist->GetName();
  for (int iBinX=1; iBinX<=nBinX; iBinX++) {
    double xc = hist->GetXaxis()->GetBinCenter(iBinX);
        char hpName[200];
        sprintf(hpName,"hpDX_%s_%d_%d_%61f",hName,nBinX,nBinY,xc);
        TH1* hp = hist->ProjectionY(hpName,iBinX,iBinX);
        double sum = hp->Integral();
        if (sum<=0.0) continue;
        if (iBinX==1) hp->Sumw2();
        hp->Scale(1.0/sum);
        for (int iBinY=1; iBinY<=nBinY; iBinY++) {
          double Value = hp->GetBinContent(iBinY);
          double eValue = hp->GetBinError(iBinY);
          double yc = hp->GetBinCenter(iBinY);
          int iBin = hist->FindBin(xc,yc);
          hist->SetBinContent(iBin,Value);
          hist->SetBinError(iBin,eValue);
        }
        delete hp;
  }
}

}

#endif // UTILITIES_HH
