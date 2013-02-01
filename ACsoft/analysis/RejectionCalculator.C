#include "RejectionCalculator.hh"

#include <Math/PdfFuncMathCore.h>
#include<TGraph.h>
#include<TGraphAsymmErrors.h>
#include<TH1I.h>
#include<TH1D.h>
#include<TF1.h>
#include<TRandom3.h>

#include<TMath.h>
#include<ctime>
#include<math.h>

#define DEBUG 0
#define INFO_OUT_TAG "RejCalc> "
#include <debugging.hh>



/** Initialize RejectionCalculator
  *
  * Ngraph=1000: number of points in Graph of PDFs, Nrandom=25000000: Number of Random Numbers to Calculate Rejection
  */
ACsoft::Analysis::RejectionCalculator::RejectionCalculator(){

  fNpointsPdfGraph=1000;
  fNrandoms=25000000;

}

/** Initialize RejectionCalculator
  *
  * \param[in] Ngraph: number of points in Graph of PDFs, Nrandom: Number of Random Numbers to Calculate Rejection
  */
ACsoft::Analysis::RejectionCalculator::RejectionCalculator(int Ngraph, int Nrandom){

  fNpointsPdfGraph=Ngraph;
  fNrandoms=Nrandom;

}

/** Default Destructor
  *
  */
ACsoft::Analysis::RejectionCalculator::~RejectionCalculator(){

}

/** C Implementation of Efficiency PDF
  *
  * Used to construct TF1 object to work with
  */
double ACsoft::Analysis::RejectionCalculator::PpdfFunc(double *x, double *par) {
 int m =(int)par[0]; // pass
 int N =(int)par[1]; // total
 double epsilon = x[0]; // efficiency

 double pdf = ROOT::Math::binomial_pdf(m, epsilon, N);

 return pdf*(N+1);
}

/** C Implementation of Koopman Funktion
  *
  * Used to construct TF1 object to work with. This is the Chsqu Funktion of the Rejection with value Zero at the minimum and value n at the n sigma error interval.
  *
  * Confidence Intervals for the Ratio of Two Binomial Proportions
  * Author(s): P. A. R. Koopman Reviewed work(s):Source: Biometrics, Vol. 40, No. 2 (Jun., 1984), pp. 513-517
  *
  * pars in function:  par[0]:totalKeeper, par[1]:passKeeper par[2]:totalRejects par[3]:passRejects
  */
double ACsoft::Analysis::RejectionCalculator::KoopmanFunc(double *x, double *par) {

  double f=0;

  double p1=(x[0]*(par[0]+par[3])+par[1]+par[2]- pow((pow((x[0]*(par[0]+par[3])+par[1]+par[2]),2.) - 4*x[0]*(par[0]+par[2])*(par[1]+par[3])  ),0.5))
            /(2*(par[0]+par[2]));
  double p2=p1/x[0];

  f=pow((par[1]-par[0]*p1),2.)/(par[0]*p1*(1-p1)) + pow((par[3]-par[2]*p2),2.)/(par[2]*p2*(1-p2));

  return f;
}

 /** Calculation of Rejection PDF as TH1D
  *
  *
  */
TH1D* ACsoft::Analysis::RejectionCalculator::CalculateRejPDF(double passKeeper, double totalKeeper, double passRejects, double totalRejects){

  // Random Number Generator
   time_t seconds;
   seconds = time (NULL);
   TRandom3* RGen = new TRandom3(seconds);
   gRandom = RGen;

//    INFO_OUT << "Initialized Random Number Generator with seed " << seconds << std::endl;

  //Get Ranges for Eff PDFs and Function defs
  //Keeper
  double KeeperEff, KElower, KEupper;
  CalculateEfficiency(passKeeper,totalKeeper,KeeperEff,KEupper,KElower);
  double KRangeLow =(KeeperEff-5*KElower<0.0 ? 0.0:KeeperEff-5*KElower);
  double KRangeHigh=(KeeperEff+5*KEupper>1.0 ? 1.0:KeeperEff+5*KEupper);

  TF1* Kpdf = new TF1("Kpdf",this,&ACsoft::Analysis::RejectionCalculator::PpdfFunc,KRangeLow,KRangeHigh,2,"RejectionCalculator","PpdfFunc");
  Kpdf->SetNpx(fNpointsPdfGraph);
  Kpdf->SetParameters(passKeeper,totalKeeper);

  //Rejects
  double RejectsEff, RElower, REupper;
  CalculateEfficiency(passRejects,totalRejects,RejectsEff,REupper,RElower);
  double RRangeLow =(RejectsEff-5*RElower<0.0 ? 0.0:RejectsEff-5*RElower);
  double RRangeHigh=(RejectsEff+5*REupper>1.0 ? 1.0:RejectsEff+5*REupper);

  TF1* Rpdf = new TF1("Rpdf",this,&ACsoft::Analysis::RejectionCalculator::PpdfFunc,RRangeLow,RRangeHigh,2,"RejectionCalculator","PpdfFunc");
  Rpdf->SetNpx(fNpointsPdfGraph);
  Rpdf->SetParameters(passRejects,totalRejects);

  //Random Number Histogram Dimensions
  double Rej,dRej_upper,dRej_lower;
  CalculateRejectionKoopman(passKeeper,totalKeeper,passRejects,totalRejects,Rej,dRej_upper,dRej_lower);
  double RangeLow = (Rej-5*dRej_lower<0. ? 0.:Rej-5*dRej_lower );
  double RangeHigh= Rej+5*dRej_upper;

  //INFO_OUT << "Starting loop over " << fNrandoms << " random numbers" << std::flush;

  TH1D* Rhist = new TH1D("Rhist","Rhist",fNpointsPdfGraph,RangeHigh,RangeLow);
  for(int i=1;i<=fNrandoms;i++){

     //if(i==fNrandoms/3) std::cout << "." << std::flush;
     //if(i==fNrandoms/3*2) std::cout << "." << std::flush;
     Rhist->Fill(Kpdf->GetRandom()/Rpdf->GetRandom());

  }
  //std::cout << "Done" << std::endl;

  //Normalize to area 1
  Rhist->Scale(1./Rhist->GetBinWidth(1)/fNrandoms);
//   INFO_OUT << "Histogram Norm: " << Rhist->Integral("width") << std::endl;

  delete RGen;
  delete Kpdf;
  delete Rpdf;

  return Rhist;
}

/** Generates the PDF of the efficiency defined by pass and total
  *
  * Follows a binomial distribution according to G. Cowan
  * \param[in] pass, total
  * \param[out] TGraph of pdf with FNpointsPdfGraph datapoints in a -5..+5 sigma interval
  */
TGraph* ACsoft::Analysis::RejectionCalculator::CalculateEfficiencyPDF(double pass, double total){

  TGraph* PDF = new TGraph(fNpointsPdfGraph);

  double Eff, Elower, Eupper;
  CalculateEfficiency(pass,total,Eff,Eupper,Elower);

  double RangeLow =(Eff-5*Elower<0.0 ? 0.0:Eff-5*Elower);
  double RangeHigh=(Eff+5*Eupper>1.0 ? 1.0:Eff+5*Eupper);
  double StepSize=(RangeHigh-RangeLow)/fNpointsPdfGraph;

  TF1* Fpdf = new TF1("Fpdf",this,&ACsoft::Analysis::RejectionCalculator::PpdfFunc,RangeLow,RangeHigh,2,"RejectionCalculator","PpdfFunc");
  Fpdf->SetNpx(fNpointsPdfGraph);
  Fpdf->SetParameters(pass,total);
  double Int=Fpdf->Integral(RangeLow,RangeHigh);

  for(int i=0;i<fNpointsPdfGraph;i++){
    double x = RangeLow + i*StepSize;
    PDF->SetPoint(i,x,Fpdf->Eval(x)/Int);
  }

  delete Fpdf;

  return PDF;
}

/** Numerically generates the PDF of the rejection as TGraph
  *
  * \param[in] passKeeper, totalKeeper, passRejects, totalRejects
  * \param[out] TGraph of pdf with FNpointsPdfGraph datapoints in a -5..+5 sigma interval
  */
TGraph* ACsoft::Analysis::RejectionCalculator::CalculateRejectionPDF(double passKeeper, double totalKeeper, double passRejects, double totalRejects){

  TH1D* Rhist = CalculateRejPDF(passKeeper,totalKeeper,passRejects,totalRejects);

  TGraph* PDF = new TGraph(fNpointsPdfGraph);
  for(int i=0;i<fNpointsPdfGraph;i++){
    double x = Rhist->GetBinCenter(i+1);
    PDF->SetPoint(i,x,Rhist->GetBinContent(i));
  }

  delete Rhist;

  return PDF;
}


/** Calculates Efficiency and its uncertainty from two numbers.
  *
  * \param[in] pass, total
  * Uses the BayesianDivide Method of Root.
  */
void ACsoft::Analysis::RejectionCalculator::CalculateEfficiency(double pass, double total, double &Eff, double &dEff_upper, double &dEff_lower){

  TH1I* Htotal = new TH1I("Htotal","Htotal",1,0,1);
  Htotal->SetBinContent(1,total);

  TH1I* Hpass = new TH1I("Hpass","Hpass",1,0,1);
  Hpass->SetBinContent(1,pass);

  TGraphAsymmErrors* BD = new TGraphAsymmErrors(1);
  BD->BayesDivide(Hpass,Htotal,"");

  double X;
  BD->GetPoint(0,X,Eff);
  dEff_upper=BD->GetErrorYhigh(0);
  dEff_lower=BD->GetErrorYlow(0);

  delete Htotal;
  delete Hpass;
  delete BD;

  return;
}


/** Calculates Efficiency and its uncertainty from two histograms TH1I.
  *
  * \param[in] pass, total
  * \param[out] TGraphAsymmErrors with Efficiency and uncertainty in each point.
  *
  * Uses the BayesianDivide Method of Root for each bin of pass.
  */
TGraphAsymmErrors* ACsoft::Analysis::RejectionCalculator::CalculateEfficiency(TH1I* pass, TH1I* total){

  TGraphAsymmErrors* gEff = new TGraphAsymmErrors(pass->GetXaxis()->GetNbins());

  gEff->BayesDivide(pass,total,"");

  return gEff;
}



/** Calculates lower and upper bounds of central confidence interval of a given histogram.
  *
  * This calculation is a helper to other functions of the RejectionCalculator, but can also be used for any input histogram.
  */
void ACsoft::Analysis::RejectionCalculator::GetCCI(TH1D* histo, double &lower_bound, double &upper_bound){

  double TargetIntegral=(1.0-0.6827)/2.0*histo->Integral();  // 1 sigma error

  double Integral=0.0;
  for(int i=0;i<histo->GetXaxis()->GetNbins();i++){ //lower bound

    Integral+=histo->GetBinContent(i+1);

    if(Integral>=TargetIntegral){
      lower_bound=histo->GetBinCenter(i+1);
      break;
    }
  }

  Integral=0.0;
  for(int i=histo->GetXaxis()->GetNbins();i>0;i--){ //upper bound

    Integral+=histo->GetBinContent(i);

    if(Integral>=TargetIntegral){
      upper_bound=histo->GetBinCenter(i);
      break;
    }
  }

  return;
}

/** Calculates lower and upper bounds of smallest confidence interval of a given histogram.
  *
  * This calculation is a helper to other functions of the RejectionCalculator, but can also be used for any input histogram.
  */
void ACsoft::Analysis::RejectionCalculator::GetSCI(TH1D* histo, double Rej, double &lower_bound, double &upper_bound){

  //start with CCI
  GetCCI(histo,lower_bound,upper_bound);

  //To which side is the pdf slanted?
  int sign=0;
  double MPV = histo->GetBinCenter(histo->GetMaximumBin());
  if(Rej>MPV) sign=1;
  if(Rej<MPV) sign=-1;

  if(sign==0) return; //in this case CCI==SCI

  //find shorter interval than CCI
  int xl=histo->FindBin(lower_bound);
  int xh=histo->FindBin(upper_bound);

  int XL=xl;
  int XH=xh;

  double Igoal=0.682689;
  int shortenedby=0;

  bool I_bigenough = true;
  while(I_bigenough){
    shortenedby++; //shorten Interval one more bin

    int shift=0;
    while(true){
      shift+=sign;
      double Int=0;
      for(int j=xl+shortenedby-shift;j<=xh-shift;j++){

        //3bin average (possible correlation of bins)
        double X1=histo->GetBinCenter(j);
        double Y1=(histo->GetBinContent(j-1)+histo->GetBinContent(j)+histo->GetBinContent(j+1))/3;
        double X2=histo->GetBinCenter(j+1);
        double Y2=(histo->GetBinContent(j)+histo->GetBinContent(j+1)+histo->GetBinContent(j+2))/3;

        Int+=(X2-X1)*(Y1+Y2)/2;
      }

      if(Int>=Igoal){XL=xl+shortenedby-shift; XH=xh-shift; break;}
      if(xl+shortenedby-abs(shift)<10){I_bigenough=false;break;}
    }
  };  //end while big enough

  lower_bound=histo->GetBinCenter(XL);
  upper_bound=histo->GetBinCenter(XH);

  return;
}


/** Calculates Rejection with uncertainty from central confidence interval of PDF from four counts.
  *
  * \param[in] passKeeper, totalKeeper, passRejects, totalRejects
  *
  * This calculation is slow. The CCI is not the smallest interval containing 1sigma of events, but is well defined.
  */
void ACsoft::Analysis::RejectionCalculator::CalculateRejectionCCI(double passKeeper, double totalKeeper, double passRejects, double totalRejects, double &Rej, double &dRej_upper, double &dRej_lower){

  Rej=(passKeeper/totalKeeper)/(passRejects/totalRejects);

  //Get normalized PDF as histo
  TH1D* Rhist = CalculateRejPDF(passKeeper,totalKeeper,passRejects,totalRejects);

  GetCCI(Rhist,dRej_lower,dRej_upper);

  dRej_upper-=Rej;
  dRej_lower=Rej-dRej_lower;

  delete Rhist;
  return;
}

/** Numerically calculates Rejection with uncertainty from central confidence interval of PDF from four histograms TH1I.
  *
  * \param[in] passKeeper, totalKeeper, passRejects, totalRejects
  * \param[out] TGraphAsymmErrors with Rejection and uncertainty in each point.
  *
  * This calculation is slow. The CCI is not the smallest interval containing 1sigma of events, but is well defined.
  */
TGraphAsymmErrors* ACsoft::Analysis::RejectionCalculator::CalculateRejectionCCI(TH1I* passKeeper, TH1I* totalKeeper, TH1I* passRejects, TH1I* totalRejects){

  TGraphAsymmErrors* gRej = new TGraphAsymmErrors();

  for(int i=0;i<passKeeper->GetXaxis()->GetNbins();i++){
    double Rej, eReju, eRejl;
    CalculateRejectionCCI(passKeeper->GetBinContent(i+1),totalKeeper->GetBinContent(i+1),passRejects->GetBinContent(i+1),totalRejects->GetBinContent(i+1),Rej, eReju, eRejl);

    gRej->SetPoint(i,passKeeper->GetBinCenter(i+1),Rej);
    gRej->SetPointError(i,0,0,eRejl,eReju);
  }

  return gRej;
}

/** Numerically calculates Rejection with uncertainty from smallest central confidence interval of PDF from four histograms TH1I.
  *
  * \param[in] passKeeper, totalKeeper, passRejects, totalRejects
  * \param[out] TGraphAsymmErrors with Rejection and uncertainty in each point.
  *
  * This calculation is slow.
  */
void ACsoft::Analysis::RejectionCalculator::CalculateRejection(double passKeeper, double totalKeeper, double passRejects, double totalRejects, double &Rej, double &dRej_upper, double &dRej_lower){

  Rej=(passKeeper/totalKeeper)/(passRejects/totalRejects);

  //Get normalized PDF as histo
  TH1D* Rhist = CalculateRejPDF(passKeeper,totalKeeper,passRejects,totalRejects);

  GetSCI(Rhist,Rej,dRej_lower,dRej_upper);

  dRej_lower=Rej - dRej_lower;
  dRej_upper=dRej_upper - Rej;

  delete Rhist;
  return;
}

/** Numerically calculates Rejection with uncertainty from smallest central confidence interval of PDF from four histograms TH1I.
  *
  * \param[in] passKeeper, totalKeeper, passRejects, totalRejects
  * \param[out] TGraphAsymmErrors with Rejection and uncertainty in each point.
  * This calculation is slow.
  */
TGraphAsymmErrors* ACsoft::Analysis::RejectionCalculator::CalculateRejection(TH1I* passKeeper, TH1I* totalKeeper, TH1I* passRejects, TH1I* totalRejects){

  TGraphAsymmErrors* gRej = new TGraphAsymmErrors();

  for(int i=0;i<passKeeper->GetXaxis()->GetNbins();i++){
    double Rej, eReju, eRejl;
    CalculateRejection(passKeeper->GetBinContent(i+1),totalKeeper->GetBinContent(i+1),passRejects->GetBinContent(i+1),totalRejects->GetBinContent(i+1),Rej, eReju, eRejl);

    gRej->SetPoint(i,passKeeper->GetBinCenter(i+1),Rej);
    gRej->SetPointError(i,0,0,eRejl,eReju);
  }

  return gRej;
}

/** Calculates Rejection with Koopman uncertainty from four counts.
  *
  * This calculation is fast.
  * \param[in] passKeeper, totalKeeper, passRejects, totalRejects
  *
  */
void ACsoft::Analysis::RejectionCalculator::CalculateRejectionKoopman(double passKeeper, double totalKeeper, double passRejects, double totalRejects, double &Rej, double &dRej_upper, double &dRej_lower){

  Rej=(passKeeper/totalKeeper)/(passRejects/totalRejects);
  double sigma=1.0; // 1 sigma error

  //Range of function
  double RangeLow = Rej-(sigma+1.0)*Rej*sqrt(1/passKeeper + 1/passRejects);
  double RangeHigh= Rej+(sigma+1.0)*Rej*sqrt(1/passKeeper + 1/passRejects);

  //pars in function:  par[0]:totalKeeper, par[1]:passKeeper par[2]:totalRejects par[3]:passRejects
  TF1* U_R = new TF1("U_R",this,&ACsoft::Analysis::RejectionCalculator::KoopmanFunc,RangeLow,RangeHigh,4,"RejectionCalculator","PpdfFunc");
  U_R->SetNpx(1000);
  U_R->SetParameters(totalKeeper,passKeeper,totalRejects,passRejects);

  dRej_lower  = Rej - U_R->GetX(sigma,RangeLow,Rej);
  dRej_upper  = U_R->GetX(sigma,Rej,RangeHigh) - Rej;

  delete U_R;

  return;
}

/** Calculates Rejection with Koopman uncertainty from four histograms TH1I.
  *
  * \param[in] passKeeper, totalKeeper, passRejects, totalRejects
  * \param[out] TGraphAsymmErrors with Rejection and uncertainty in each point.
  * This calculation is fast.
  */
TGraphAsymmErrors* ACsoft::Analysis::RejectionCalculator::CalculateRejectionKoopman(TH1I* passKeeper, TH1I* totalKeeper, TH1I* passRejects, TH1I* totalRejects){

  TGraphAsymmErrors* gRej = new TGraphAsymmErrors();

  for(int i=0;i<passKeeper->GetXaxis()->GetNbins();i++){
    double Rej, eReju, eRejl;
    CalculateRejectionKoopman(passKeeper->GetBinContent(i+1),totalKeeper->GetBinContent(i+1),passRejects->GetBinContent(i+1),totalRejects->GetBinContent(i+1),Rej, eReju, eRejl);

    gRej->SetPoint(i,passKeeper->GetBinCenter(i+1),Rej);
    gRej->SetPointError(i,0,0,eRejl,eReju);
  }

  return gRej;
}

/** Calculates Positron Fraction with statistical uncertainties
  * \param[in] Nleptons Measured number of Leptons (after all cuts)
  * \param[in] Eleptons Central Value of Lepton Efficiency
  * \param[in] Npositrons Measured number of Positrons (after all cuts)
  * \param[in] Epositrons Central Value of Lepton Efficiency (after Lepton cuts, so total positron efficiency is Elepton*Epositron)
  * \param[in] Nprotons Total number of background protons (real number before cuts)
  * \param[in] Eprotons Central value of determined proton efficiency
  *
  * fraction=(Npositrons/Epositrons/Eleptons - Nprotons*Eprotons)/(Nleptons/Eleptons - Nprotons*Eprotons);
  */
TH1D* ACsoft::Analysis::RejectionCalculator::CalculatePositronFraction(double Nleptons, double Eleptons, double Npositrons, double Epositrons, double Nprotons, double Eprotons,double &fraction, double &dFrac_upper, double &dFrac_lower){

	fraction=(Npositrons/Epositrons/Eleptons - Nprotons*Eprotons)/(Nleptons/Eleptons - Nprotons*Eprotons);
	
	//Get PDFs of Epositrons, Eleptons, and e_p as TF1 objects
	//e_p
	double Eff,Eupper,Elower;
	CalculateEfficiency(Nprotons*Eprotons,Nprotons,Eff,Eupper,Elower);
	double RangeLow =(Eff-5*Elower<0.0 ? 0.0:Eff-5*Elower);
	double RangeHigh=(Eff+5*Eupper>1.0 ? 1.0:Eff+5*Eupper);
	
	TF1* Fe_p = new TF1("Fe_p",this,&ACsoft::Analysis::RejectionCalculator::PpdfFunc,RangeLow,RangeHigh,2,"RejectionCalculator","PpdfFunc");
	Fe_p->SetNpx(fNpointsPdfGraph);
	Fe_p->SetParameters(Nprotons*Eprotons,Nprotons);
	
	//Eleptons
	CalculateEfficiency(Nleptons*Eleptons,Nleptons,Eff,Eupper,Elower);
	RangeLow =(Eff-5*Elower<0.0 ? 0.0:Eff-5*Elower);
	RangeHigh=(Eff+5*Eupper>1.0 ? 1.0:Eff+5*Eupper);
	
	TF1* Fe_L = new TF1("Fe_L",this,&ACsoft::Analysis::RejectionCalculator::PpdfFunc,RangeLow,RangeHigh,2,"RejectionCalculator","PpdfFunc");
	Fe_L->SetNpx(fNpointsPdfGraph);
	Fe_L->SetParameters(Nleptons*Eleptons,Nleptons);
	
	//Epositrons
	CalculateEfficiency(Npositrons*Epositrons,Npositrons,Eff,Eupper,Elower);
	RangeLow =(Eff-5*Elower<0.0 ? 0.0:Eff-5*Elower);
	RangeHigh=(Eff+5*Eupper>1.0 ? 1.0:Eff+5*Eupper);
	
	TF1* Fe_plus = new TF1("Fe_plus",this,&ACsoft::Analysis::RejectionCalculator::PpdfFunc,RangeLow,RangeHigh,2,"RejectionCalculator","PpdfFunc");
	Fe_plus->SetNpx(fNpointsPdfGraph);
	Fe_plus->SetParameters(Npositrons*Epositrons,Npositrons);
	
    //Histogram for PDF of PositronFraction
	double poshistRangeLow=  -0.5*fraction; //0.0;
	double poshistRangeHigh= 2.5*fraction; //0.3;
	TH1D* posfrac = new TH1D("posfrac","posfrac",80000,poshistRangeLow,poshistRangeHigh);

	for(int nR=0;nR<2*fNrandoms;nR++){
		// PosFrac = (Npositrons/Epositrons - Nprotons*Eprotons)/(Nleptons/Eleptons - Nprotons*Eprotons)
		double r_e_L=Fe_L->GetRandom();
		double r_e_plus=r_e_L*Fe_plus->GetRandom();
		double r_n_P_e=Nprotons*Fe_p->GetRandom();
		double r_n_L=gRandom->PoissonD(Nleptons);
		double r_n_plus=gRandom->PoissonD(Npositrons);
		
		//operate random numbers
		double r_PF=(r_n_plus/r_e_plus - r_n_P_e)/(r_n_L/r_e_L - r_n_P_e);
		
		//Fill pos Frac Histogram
		posfrac->Fill(r_PF);
	}
	
	//extract Pos Fraction and error -> CCI
	fraction=FindTheMaximum(posfrac);
	GetCCI(posfrac,dFrac_lower,dFrac_upper);
	
	dFrac_upper=fabs(fraction-dFrac_upper);
	dFrac_lower=fabs(fraction-dFrac_lower);


  delete Fe_p;
	delete Fe_L;
	delete Fe_plus;
  return posfrac;
}


double ACsoft::Analysis::RejectionCalculator::CalculateSimpleFraction(double primary, double secondary, double &dYup, double &dYlow){

  // Random Number Generator
   time_t seconds;
   seconds = time (NULL);
   TRandom3* RGen = new TRandom3(seconds);
   gRandom = RGen;

  double Frac = secondary/(secondary+primary);

  //Histogram for PDF of Fraction
  double poshistRangeLow=  -0.5*Frac;
  double poshistRangeHigh= 2.5*Frac;
  TH1D* Hfrac = new TH1D("Hfrac","Hfrac",10000,poshistRangeLow,poshistRangeHigh);

  for(int nR=0;nR<fNrandoms;nR++){
      double r_sec=gRandom->PoissonD(secondary);
      double r_pri=gRandom->PoissonD(primary);

      //operate random numbers
      double r_frac=r_sec/(r_sec+r_pri);

      //Fill pos Frac Histogram
      Hfrac->Fill(r_frac);
  }

  GetCCI(Hfrac,dYlow,dYup);
  GetSCI(Hfrac,Frac,dYlow,dYup);

  dYup=fabs(Frac-dYup);
  dYlow=fabs(Frac-dYlow);

  INFO_OUT << "CalculateSimpleFraction: " << secondary << "/(" << primary << "+" << secondary << ")=" << Frac << " +" << dYup << " -" << dYlow << std::endl;

  delete Hfrac;
  delete RGen;
  return Frac;
}

/** Finds the Maximum in a given histogram by weighted mean algorythm.
  *
  */
double ACsoft::Analysis::RejectionCalculator::FindTheMaximum(TH1D* histo){
	
  double YtoEvaluate=histo->GetBinContent(1);
	
	double sum=0;
	double sumweights=0;
	for(int i=1;i<=histo->GetNbinsX();i++){
		
		if(histo->GetBinContent(i)>=YtoEvaluate){
			sum+=histo->GetBinContent(i)*histo->GetBinCenter(i);
			sumweights+=histo->GetBinContent(i);
		}
		
	}
	
	return sum/sumweights;
}

