#ifndef REJECTION_UTILS_HH
#define REJECTION_UTILS_HH

class TH1;
class TH2F;
class TH2;
class TH3F;
class TGraph;
class TGraphErrors;
class TGraphAsymmErrors;

#include <string>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <Rtypes.h>

class RejectionStats {

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
  TGraph* grRejectionVsCut;
  double R1; ///< low rigidity bound for which result was obtained
  double R2; ///< high rigidity bound for which result was obtained

  RejectionStats( double rig1=0., double rig2=0. )
    : nElec(0)
    , nProt(0)
    , Cut(0)
    , ElecEff(0)
    , eElecEffUp(0)
    , eElecEffDown(0)
    , ProtEff(0)
    , eProtEffUp(0)
    , eProtEffDown(0)
    , ProtRej(0)
    , eProtRejUp(0)
    , eProtRejDown(0)
    , grRejectionVsCut(0)
    , R1(rig1)
    , R2(rig2){
  }

  double Rmean() const { return sqrt(R1*R2); }

  bool IsUsable() const {
    return Cut>0.0 && nElec>10.0 && nProt>0.0 && ProtRej>0.0;
  }

  void Print() const {
    printf("R=%9.2f - %9.2f Cut=%6.3f nElec=%9.1f nProt=%9.1f Electron Efficiency %12.4E+%12.4E-%12.4E Proton Efficiency %12.4E+%12.4E-%12.4E Rejection=%12.4E+%12.4E-%12.4E \n",R1,R2,Cut,nElec,nProt,ElecEff,eElecEffUp,eElecEffDown,ProtEff,eProtEffUp,eProtEffDown,ProtRej,eProtRejUp,eProtRejDown);
  }
};


struct RejectionGraphCollection {

  RejectionGraphCollection() :
    grCut(0),
    grElecEff(0),
    grProtEff(0),
    grProtRej(0) { }

  TGraph*            grCut;      ///< likelihood cut vs rigidity
  TGraphAsymmErrors* grElecEff;  ///< (particle-ID) electron efficiency vs rigidity
  TGraphAsymmErrors* grProtEff;  ///< proton efficiency vs rigidity
  TGraphAsymmErrors* grProtRej;  ///< proton rejection vs rigidity
};

RejectionGraphCollection MakeRejectionGraphs( const std::vector<RejectionStats*> input, Color_t color );

void PerformAnalysisOfRejectionVsRigidity( TH2* h_Elec_Dat, TH2* h_Prot_Dat, TH2* h_Elec_MC, TH2* h_Prot_MC, TH1 *preQualityInputSpectrum, TH1 *postQualityInputSpectrum, std::string label, bool snapshot );

void CalculatePositronFraction(double ep, double eep, double em, double eem, double &PosFrac, double &PosFracError);

TGraphAsymmErrors* GetRejection(TH1 *hElec, TH1* hProt, double EffValue, RejectionStats *Results);
TGraphAsymmErrors* GetEfficiency(TH1 *hElec, TH1* hProt, double RejValue, RejectionStats *Results);
TGraphAsymmErrors* GetRejectionOrEfficiency(TH1 *hElec, TH1* hProt, double EffOrRejValue, RejectionStats *Results);

void ScaleEfficiencyAxis( TGraph* graphWithEfficiencyOnXaxis, double scale );
int RootGetEfficiencyBayesian(double m, double N, double &Eff, double &eUp, double &eDown);
int RootGetEfficiencyClopperPearson(double m, double N, double &Eff, double &eUp, double &eDown);

void RootPadStyle(double scale);
void RootHistStyle1D(TH1 *hist, double scale);
void RootHistStyle2D(TH2 *hist, double scale);

TGraphErrors* RootGetGaussXslices(TH2 *hist, double X0, double X1);

TGraph* CreateGraphForProtonRejectionFromPositronPaper2013();

#endif
