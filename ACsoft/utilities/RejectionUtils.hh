#ifndef REJECTION_UTILS_HH
#define REJECTION_UTILS_HH

class TH1;
class TH2F;
class TH2;
class TH3F;
class TGraphErrors;
class TGraphAsymmErrors;

#include <string>

class ClassResults {

public:
  double nElec;
  double nProt;
  double Cut;
  double ElecEff;
  double eElecEffUp;
  double eElecEffDown;
  double ProtEff;
  double eProtEffUp;
  double eProtEffDown;
  double ProtRej;
  double eProtRejUp;
  double eProtRejDown;
  ClassResults() {nElec=0.0; nProt=0.0; Cut=0.0; ElecEff=0.0; eElecEffUp=0.0; eElecEffDown=0.0; ProtEff=0.0; eProtEffUp=0.0; eProtEffDown=0.0; ProtRej=0.0; eProtRejUp=0.0; eProtRejDown=0.0;}

};


void CalculatePositronFraction(double ep, double eep, double em, double eem, double &PosFrac, double &PosFracError);

TGraphAsymmErrors* GetRejection(TH1 *hElec, TH1* hProt, double EffValue, ClassResults *Results);
int RootGetEffciencyBayesian(double m, double N, double &Eff, double &eUp, double &eDown);
void RootPadStyle(double scale);
void RootHistStyle1D(TH1 *hist, double scale);
void RootHistStyle2D(TH2 *hist, double scale);

TH2F* GetXzProjection(TH3F *hist, std::string hName, std::string hTitle, int Bin0, int Bin1);
TH2F* GetYzProjection(TH3F *hist, std::string hName, std::string hTitle, int Bin0, int Bin1);
TGraphErrors* RootGetGaussXslices(TH2 *hist, double X0, double X1);

#endif
