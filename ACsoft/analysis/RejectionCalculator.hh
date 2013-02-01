#ifndef REJECTIONCALCULATOR_HH
#define REJECTIONCALCULATOR_HH

class TH1I;
class TGraph;
class TGraphAsymmErrors;
class TH1D;

namespace ACsoft {

namespace Analysis {

/** Class that calculates efficiencies, rejections, and positron fractions with correct uncertainties.
  *
  * It needs the event counts as integers, doubles, or in TH1I
  * The event numbers MUST be greater than Zero!
  */
class RejectionCalculator
{

public:
  RejectionCalculator();
  RejectionCalculator(int Ngraph, int Nrand);
  virtual ~RejectionCalculator();

  //calculate PDFs
  TGraph* CalculateEfficiencyPDF(int pass, int total){ return CalculateEfficiencyPDF((double)pass, (double)total); }

  TGraph* CalculateEfficiencyPDF(double pass, double total);

  TGraph* CalculateRejectionPDF(int passKeeper, int totalKeeper, int passRejects, int totalRejects){ return CalculateRejectionPDF((double)passKeeper, (double)totalKeeper, (double)passRejects, (double)totalRejects); }

  TGraph* CalculateRejectionPDF(double passKeeper, double totalKeeper, double passRejects, double totalRejects);

  //Calculate Efficiencies
  void CalculateEfficiency(int pass, int total, double &Eff, double &dEff_upper, double &dEff_lower){ CalculateEfficiency((double)pass, (double)total,Eff,dEff_upper,dEff_lower); return;}

  void CalculateEfficiency(double pass, double total, double &Eff, double &dEff_upper, double &dEff_lower);

  TGraphAsymmErrors* CalculateEfficiency(TH1I* pass, TH1I* total);

  //Calculate Rejection

  void GetCCI(TH1D* histo, double &lower_bound, double &upper_bound);
  void GetSCI(TH1D* histo, double Rej, double &lower_bound, double &upper_bound);

  void CalculateRejectionCCI(int passKeeper, int totalKeeper, int passRejects, int totalRejects, double &Rej, double &dRej_upper, double &dRej_lower){
    CalculateRejectionCCI((double)passKeeper, (double)totalKeeper, (double)passRejects, (double)totalRejects,Rej,dRej_upper,dRej_lower); return;
  }
  void CalculateRejectionCCI(double passKeeper, double totalKeeper, double passRejects, double totalRejects, double &Rej, double &dRej_upper, double &dRej_lower);
  TGraphAsymmErrors* CalculateRejectionCCI(TH1I* passKeeper, TH1I* totalKeeper, TH1I* passRejects, TH1I* totalRejects);

  void CalculateRejection(int passKeeper, int totalKeeper, int passRejects, int totalRejects, double &Rej, double &dRej_upper, double &dRej_lower){
    CalculateRejection((double)passKeeper, (double)totalKeeper, (double)passRejects, (double)totalRejects,Rej,dRej_upper,dRej_lower); return;
  }
  void CalculateRejection(double passKeeper, double totalKeeper, double passRejects, double totalRejects, double &Rej, double &dRej_upper, double &dRej_lower);
  TGraphAsymmErrors* CalculateRejection(TH1I* passKeeper, TH1I* totalKeeper, TH1I* passRejects, TH1I* totalRejects);

  void CalculateRejectionKoopman(int passKeeper, int totalKeeper, int passRejects, int totalRejects, double &Rej, double &dRej_upper, double &dRej_lower){
    CalculateRejectionKoopman((double)passKeeper, (double)totalKeeper, (double)passRejects, (double)totalRejects,Rej,dRej_upper,dRej_lower); return;
  }
  void CalculateRejectionKoopman(double passKeeper, double totalKeeper, double passRejects, double totalRejects, double &Rej, double &dRej_upper, double &dRej_lower);
  TGraphAsymmErrors* CalculateRejectionKoopman(TH1I* passKeeper, TH1I* totalKeeper, TH1I* passRejects, TH1I* totalRejects);

  TH1D* CalculatePositronFraction(double Nleptons, double Eleptons, double Npositrons, double Epositrons, double Nprotons, double Eprotons,double &fraction, double &dFrac_upper, double &dFrac_lower);

  TH1D* CalculatePositronFraction(int Nleptons, double Eleptons, int Npositrons, double Epositrons, int Nprotons, double Eprotons,double &fraction, double &dFrac_upper, double &dFrac_lower){
		return CalculatePositronFraction((double)Nleptons,Eleptons,(double)Npositrons,Epositrons,(double)Nprotons,Eprotons,fraction,dFrac_upper,dFrac_lower);
	}

  double CalculateSimpleFraction(double primary, double secondary, double &dYup, double &dYlow);

private:

  double PpdfFunc(double *x, double *par);
  double RpdfFunc(double *x, double *par);
  double KoopmanFunc(double *x, double *par);
  TH1D* CalculateRejPDF(double passKeeper, double totalKeeper, double passRejects, double totalRejects);
	double FindTheMaximum(TH1D* histo);

protected:

  int fNpointsPdfGraph;   ///< Number of Points in returned TGraphs of PDFs
  int fNrandoms;       ///< Number of Random numbers sampled in calculation of dRejection

};

}

}

#endif // REJECTIONCALCULATOR_HH
