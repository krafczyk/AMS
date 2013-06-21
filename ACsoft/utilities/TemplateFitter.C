
#include "TemplateFitter.hh"
#include "Utilities.hh"

#include <TH1.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TList.h>
#include <TMinuit.h>
#include <TMath.h>
#include <TStyle.h>

#include <assert.h>
#include <vector>
#include <math.h>

#include <QString>
#include <QVector>

#define DEBUG 0
#define INFO_OUT_TAG "TemplateFitter> "
#include <debugging.hh>

namespace ACsoft {

/**
 * @brief Default constructor
 * @param printlevel
 *
 * printlevel is for TMinuit output: 0 standard, -1 quiet
 */
Utilities::TemplateFitter::TemplateFitter(int printlevel):
  fPrintLevel(printlevel),
  fHasBeenFit(false)
{
  fData=0;
  fFitResult=0;
  fInputData=0;
}

/**
 * @brief Default destructor
 *
 * calls Clear()
 */
Utilities::TemplateFitter::~TemplateFitter(){
  Clear();
}

void Utilities::TemplateFitter::AddTemplateHistogram(TH1F* hist) {
  fInputTemplates.push_back(hist);
}

int Utilities::TemplateFitter::TemplateColor(int i) const {
  QVector<int> firstColors;
  firstColors << kAzure - 3 << kPink << kGreen << kYellow << kOrange;
  if(i < firstColors.size())
    return firstColors.at(i);
  return Utilities::RootColor(i);
}

int Utilities::TemplateFitter::TemplateFillStyle(int i) const {
  QVector<int> fillStyles;
  fillStyles << 3017 << 3018 << 3004 << 3005 << 3006 << 3007 << 3021 << 3022;
  return fillStyles.at(i%fillStyles.size());
}

/**
 * @brief Create Canvas with result drawn
 * @param canvasname
 *
 * Returns a canvas with the fit result drawn into it. The user has to delete this, the class doesn't.
 * Supply a canvas name to prevent canvases with one default name to be overwritten.
 */
TCanvas* Utilities::TemplateFitter::CreateResultDrawing(std::string canvasname, int width, int height, double chi2, bool defaultStyle) const {

  if(!fHasBeenFit){
    WARN_OUT << "No fit was done!" << std::endl;
    return 0;
  }

  gStyle->SetOptStat(0);
  TCanvas* graphicsOutput = new TCanvas(canvasname.c_str(),canvasname.c_str(),width,height);
  graphicsOutput->cd();
  gPad->SetLogy();
  gPad->SetGrid();

  TLegend* legend = new TLegend(0.6034913,0.7009346,0.8944306,0.8913551,NULL,"brNDC");
  legend->SetFillColor(kWhite);
  legend->SetLineColor(kWhite);

  TH1* dataCopy = fData->DrawCopy();
  dataCopy->SetStats(0); // disable statistic boxes in template canvas
  const int dataColor = kBlack;
  const int dataMarker = 20;
  dataCopy->SetLineWidth(2);
  dataCopy->SetLineColor(dataColor);
  dataCopy->SetMarkerColor(dataColor);
  dataCopy->SetMarkerStyle(dataMarker);
  legend->AddEntry(dataCopy, Form("Data distribution (%i entries)", (int)fData->GetEntries()));

  TH1* fitResultCopy = fFitResult->DrawCopy("hist.same");
  fitResultCopy->SetStats(0);
  const int fitColor = kBlack;
  const int fitStyle =2;
  fitResultCopy->SetLineWidth(4);
  fitResultCopy->SetLineColor(fitColor);
  fitResultCopy->SetLineStyle(fitStyle);
  legend->AddEntry(fitResultCopy, Form("Template fit result (#chi^{2}/ndf=%5.2f)", chi2));

  for (unsigned int i = 0; i < fResultHistos.size(); ++i) {
    TH1* resultCopy = fResultHistos.at(i)->DrawCopy("hist.same");
    resultCopy->SetStats(0);
    if (defaultStyle) {
      resultCopy->SetLineWidth(2);
      const int color = TemplateColor(i);
      const int fillStyle = TemplateFillStyle(i);
      resultCopy->SetFillStyle(fillStyle);
      resultCopy->SetFillColor(color);
      resultCopy->SetLineColor(color);
      resultCopy->SetMarkerColor(color);
    }
    // split to long titles in two lines
    QString title = resultCopy->GetTitle();
    QString fittedValue = QString(" #color[%1]{Fitted: %2 #pm %3}")
                                  .arg(resultCopy->GetLineColor())
                                  .arg(GetAbsoluteResult().at(i), 0, 'f', 1, '0')
                                  .arg(GetAbsoluteResultError().at(i), 0, 'f', 1, '0');
    title.append(fittedValue);
    if (title.contains(",")) {
      title.replace(",", "}{");
      title.prepend("#splitline{");
      title.append("}");
    }
    legend->AddEntry(resultCopy, qPrintable(title), "F");
  }
  // Redraw  fitResult and data to be on top of the templates
  fitResultCopy->Draw("hist.same");
  dataCopy->Draw("same");

  legend->Draw();
  return graphicsOutput;
}

/**
 * @brief Clear function
 *
 * Resets all class members. Should be called before any new fit is done to prevent old results to linger.
 */
void Utilities::TemplateFitter::Clear(){

  fHasBeenFit=false;
  if(fData)
    fData=0;
  if(fInputData)
    fInputData=0;
  fTemplates.clear();
  fInputTemplates.clear();
  fRelative.clear();
  fAbsolute.clear();
  fRelativeError.clear();
  fAbsoluteError.clear();

  if(fFitResult)
    delete fFitResult;
  fFitResult=0;
  for(int i=0;i<(int)fResultHistos.size();i++)
    delete fResultHistos[i];
  fResultHistos.clear();

  return;
}

/**
 * @brief Clone fit result
 *
 * Returns the resulting histogram of the fit as a clone. This is the weighted sum of the templates that describe the data best.
 * The user has to delete this, the TemplateFitter doesn't
 */
TH1F* Utilities::TemplateFitter::CloneCombinedResultHistogram(const char* hname="") const {

  if(!fHasBeenFit){
    WARN_OUT << "No fit was done!" << std::endl;
    return 0;
  }

  return (TH1F*)fFitResult->Clone(hname);
}

double Utilities::TemplateFitter::Fit(int i,double lowBound, double highBound){

  //reset things in case there was a previous fit with the same templates.
  fHasBeenFit=false;
  fTemplates.clear();
  if(fData)
    fData=0;
  fRelative.clear();
  fAbsolute.clear();
  fRelativeError.clear();
  fAbsoluteError.clear();

  if(fFitResult)
    delete fFitResult;
  fFitResult=0;
  for(int k=0;k<(int)fResultHistos.size();k++)
    delete fResultHistos[k];
  fResultHistos.clear();

  if(!TestHistogramCompatibility()){
    INFO_OUT << "Histogram compatibility failed... Fit aborted!" << std::endl;
    return -2.00;
  }

  //set range for fit by editing the histograms
  if(lowBound==0 && highBound==0){ // no range set. use whole histos...
    for(int k=0;k<(int)fInputTemplates.size();k++)
      fTemplates.push_back(fInputTemplates.at(k));
    fData = (TH1F*)fInputData->Clone("dataForFit");
  }
  else{ //fit range specified

    //define
    int lowBin = std::max(fInputData->FindBin(lowBound),1);
    int highBin = std::min(fInputData->FindBin(highBound),fInputData->GetNbinsX());

    int Nbins=(highBin-lowBin)+1;
    double Xedges[Nbins+1];
    for(int l=0;l<=Nbins;l++)
      Xedges[l]=fInputData->GetBinLowEdge(lowBin+l);

    //create and fill
    for(int k=0;k<(int)fInputTemplates.size();k++){

      char hname[64];
      sprintf(hname,"tempforfit%d",k);
      fTemplates.push_back(new TH1F(hname,fInputTemplates.at(k)->GetTitle(),Nbins,Xedges));
      for(int l=1;l<=Nbins;l++)
        fTemplates.back()->SetBinContent(l,fInputTemplates.at(k)->GetBinContent(lowBin+l-1));

    }
    fData = new TH1F("dataForFit",fInputData->GetTitle(),Nbins,Xedges);
    for(int l=1;l<=Nbins;l++)
      fData->SetBinContent(l,fInputData->GetBinContent(lowBin+l-1));
  }

    if(i==0)
      return ChiSqrFit();
    else if(i==1)
      return LlhFit();
    else {
      INFO_OUT << "No valid fit strategy choosen!" << std::endl;
      return -5.0;
    }
}


/**
 * @brief Fitting routine using Chisquared
 *
 * Applies a Chisquared fit to match the data histogram with the template histograms and free amplitudes for the templates.
 * The fit results can afterwards be accessed by getter functions.
 * Relative weights as well as absolute values are provided.
 * Before the fit a check on consistency of the histograms is done (Nbins, BinWidth,BinEdges)
 *
 * The function returns the reduced Chisquared.
 */
double Utilities::TemplateFitter::ChiSqrFit(){

  double Chisq=-1;

  //Prepare fit object
  TList *list = new TList();
  list->Add(fData);
  for(int i=0; i<(int)fTemplates.size();i++)
    list->Add(fTemplates.at(i));

  TMinuit* minimizer = new TMinuit((int)fTemplates.size()); //The Minuit(numberofparameters)
  minimizer->SetPrintLevel(fPrintLevel); // -1 for silent mode

   //Link to Chisquare
  minimizer->SetFCN(Utilities::TemplateFitter::chisqr_fit_func); //Link to Chisquare

  minimizer->SetObjectFit(list); //link to root object to be fitted

  int ierflg, success=0;

  for(int i=0; i<(int)fTemplates.size();i++){
    char parname[32];
    sprintf(parname,"Amplitude_%d",i);
    minimizer->mnparm(i,parname,1/(float)fTemplates.size(),0.2/(float)fTemplates.size(),0,1.2,ierflg); success+=ierflg;
  }

  double arglist[2]={2000,0.1};
  minimizer->mnexcm("SIMPLEX",arglist,2,ierflg); success+=ierflg; //FitCommand
  minimizer->mnexcm("MIGRAD",arglist,2,ierflg); success+=ierflg; //FitCommand
  minimizer->mnexcm("MINOS",arglist,2,ierflg); success+=ierflg; //FitCommand

  double edm,errdef;
  int nvpar,nparx,icstat;

  minimizer->mnstat(Chisq,edm,errdef,nvpar,nparx,icstat); //Fit Status

  //retrieve result
  double value, error;
  for(int i=0; i<(int)fTemplates.size();i++){
     minimizer->GetParameter(i,value,error);
     fRelative.push_back(value);
     fRelativeError.push_back(error);
     fAbsolute.push_back(value*fData->Integral());
     fAbsoluteError.push_back(error*fData->Integral());
  }

  //Get NDF
  double NDF=0;
  for(int k=1;k<=fData->GetXaxis()->GetNbins();k++){
    double dataentry = fData->GetBinContent(k);
    double tempentry=0;
    for(int i=0; i<(int)fTemplates.size();i++)
      tempentry+=fTemplates.at(i)->GetBinContent(k);

    if(dataentry>0 && tempentry>0)
      NDF++;

  }
  NDF-=fTemplates.size();

  //assemble fFitResult
  for(int i=0;i<(int)fTemplates.size();i++){
    fResultHistos.push_back((TH1F*)fTemplates.at(i)->Clone());
    fResultHistos.back()->Scale(fAbsolute.at(i)/fResultHistos.back()->Integral());

    if(i==0)
      fFitResult=(TH1F*)fResultHistos.at(i)->Clone();
    else
      fFitResult->Add(fResultHistos.at(i));
  }

  delete minimizer;
  delete list;

  if(success==0) fHasBeenFit=true;
  return NDF<=0 ? -1:Chisq/NDF;
}

/**
 * @brief Internal fit function for Chisquared fit.
 */
void Utilities::TemplateFitter::chisqr_fit_func(int &, double *, double &f, double *par, int ){

    TList* list = (TList*)gMinuit->GetObjectFit();
    int mNbins=((TH1F*)list->At(0))->GetNbinsX();

    std::vector<double> mBinWidth;
    for(int i=1;i<=mNbins;i++)
      mBinWidth.push_back(((TH1F*)list->At(0))->GetXaxis()->GetBinWidth(i));

    std::vector<double> mNentries;
    for(int i=0;i<list->GetSize();i++)
      mNentries.push_back(((TH1F*)list->At(i))->Integral());

    //fill data to fit and normalize
    int BinLow=1, BinHigh=mNbins;
  std::vector< std::vector<double> > DataSet;
  for(int k = 0; k < list->GetSize(); ++k) {
    std::vector<double> kDataSet;
    for(int i = BinLow; i <= BinHigh; ++i)
        kDataSet.push_back((double)((TH1F*)list->At(k))->GetBinContent(i) / mBinWidth.at(i-1) / mNentries.at(k));
    DataSet.push_back(kDataSet);
  }

    double Chisq=0;

    //Calculate Chisquare!
    for(int i=0;i<mNbins;i++){

      double TempSum=0;
      for(int k=1;k<list->GetSize();k++)
        TempSum+=par[k-1]*DataSet[k].at(i);

      if(DataSet[0].at(i)==0 || TempSum==0)
        continue;

      double nominator=pow(TempSum - DataSet[0].at(i),2);
      double sigsqr=DataSet[0].at(i)/mNentries.at(0)/mBinWidth.at(i);
      for(int k=1;k<list->GetSize();k++)
        sigsqr+=DataSet[k].at(i)/mNentries.at(k)/mBinWidth.at(i)*pow(par[k-1],2); //poisson error

      Chisq+=nominator/sigsqr;
    }

    f=Chisq;
}


/**
 * @brief Fit routine using a likelihood fit.
 *
 * Fit routine using a likelihood function to determine the parameters. The fit function
 * is taken from Cowan, "Statistical Data Analysis", p. 89, Eq. 6.46
 *
 * @return effective reduced chi-squared
 */
double Utilities::TemplateFitter::LlhFit(){

  //Prepare fit object
  TList *list = new TList();
  list->Add(fData);
  for(int i=0; i<(int)fTemplates.size();i++)
    list->Add(fTemplates.at(i));

  TMinuit* minimizer = new TMinuit((int)fTemplates.size()); //The Minuit(numberofparameters)
  minimizer->SetPrintLevel(fPrintLevel); // -1 for silent mode

   //Link to LLh function
  minimizer->SetFCN(Utilities::TemplateFitter::llh_fit_func); //Link to Chisquare

  minimizer->SetObjectFit(list); //link to root object to be fitted

  int ierflg, success=0;

  // for max likelihood = 0.5, for chisq = 1.0
  double arglist[2];
  arglist[0] = 0.5;
  gMinuit->mnexcm("SET ERR", arglist ,1,ierflg); success+=ierflg;

  // initialize parameters
  for(int i=0; i<(int)fTemplates.size();i++){
    char parname[32];
    sprintf(parname,"Amplitude_%d",i);
    minimizer->mnparm(i,parname,1/(float)fTemplates.size(),0.2/(float)fTemplates.size(),0,1.2,ierflg); success+=ierflg;
  }

  arglist[0]=2000;
  arglist[1]=0.1;
  minimizer->mnexcm("SIMPLEX",arglist,2,ierflg); success+=ierflg; //FitCommand
  minimizer->mnexcm("MIGRAD",arglist,2,ierflg); success+=ierflg; //FitCommand
  minimizer->mnexcm("MINOS",arglist,2,ierflg); success+=ierflg; //FitCommand

  double chi,edm,errdef;
  int nvpar,nparx,icstat;

  minimizer->mnstat(chi,edm,errdef,nvpar,nparx,icstat); //Fit Status

  //retrieve result
  double value, error;
  for(int i=0; i<(int)fTemplates.size();i++){
     minimizer->GetParameter(i,value,error);
     fRelative.push_back(value);
     fRelativeError.push_back(error);
     fAbsolute.push_back(value*fData->Integral());
     fAbsoluteError.push_back(error*fData->Integral());
  }

  //assemble fFitResult
  for(int i=0;i<(int)fTemplates.size();i++){
    fResultHistos.push_back((TH1F*)fTemplates.at(i)->Clone());
    fResultHistos.back()->Scale(fAbsolute.at(i)/fResultHistos.back()->Integral());

    if(i==0)
      fFitResult=(TH1F*)fResultHistos.at(i)->Clone();
    else
      fFitResult->Add(fResultHistos.at(i));
  }


  //manually calculate goodness-of-fit parameter
  int mNbins=((TH1F*)list->At(0))->GetNbinsX();

  std::vector<double> mBinWidth;
  for(int i=1;i<=mNbins;i++)
    mBinWidth.push_back(((TH1F*)list->At(0))->GetXaxis()->GetBinWidth(i));

  std::vector<double> mNentries;
  for(int i=0;i<list->GetSize();i++)
    mNentries.push_back(((TH1F*)list->At(i))->Integral());

  //fill data to fit and normalize
  int BinLow = 1, BinHigh = mNbins;
  std::vector< std::vector<double> > DataSet;
  for(int k = 0; k < list->GetSize(); ++k) {
    std::vector<double> kDataSet;
    for(int i = BinLow; i <= BinHigh; ++i)
      kDataSet.push_back((double)((TH1F*)list->At(k))->GetBinContent(i) / mBinWidth.at(i-1) / mNentries.at(k));
    DataSet.push_back(kDataSet);
  }

  double Chisq=0.0;
  double effNDF=0.0;
  //Calculate Chisquare!
  for(int i=0;i<mNbins;i++){

    double TempSum=0;
    for(int k=1;k<list->GetSize();k++)
      TempSum+=fRelative.at(k-1)*DataSet[k].at(i);

    if(TempSum==0 || DataSet[0].at(i)==0) continue;

    double nominator=pow(TempSum - DataSet[0].at(i),2);
    double sigsqr=DataSet[0].at(i)/mNentries.at(0)/mBinWidth.at(i);
    for(int k=1;k<list->GetSize();k++)
      sigsqr+=DataSet[k].at(i)/mNentries.at(k)/mBinWidth.at(i)*pow(fRelative.at(k-1),2); //poisson error

    Chisq+=nominator/sigsqr;
    effNDF+=1;
  }

  delete minimizer;
  delete list;

  if(success==0) {
    fHasBeenFit=true;
    return (effNDF==0 ? -3 : Chisq/effNDF );
  }
  else
    return -1.0;
}


/**
 * @brief Internal fit function for Likelihood fit.
 */
void Utilities::TemplateFitter::llh_fit_func(int &, double *, double &f, double *par, int ){

  TList* list = (TList*)gMinuit->GetObjectFit();
  int mNbins=((TH1F*)list->At(0))->GetNbinsX();

  std::vector<double> mBinWidth;
  for(int i=1;i<=mNbins;i++)
    mBinWidth.push_back(((TH1F*)list->At(0))->GetXaxis()->GetBinWidth(i));

  std::vector<double> mNentries;
  for(int i=0;i<list->GetSize();i++)
    mNentries.push_back(((TH1F*)list->At(i))->Integral());

  //fill data to fit and normalize
  int BinLow=1, BinHigh=mNbins;
  std::vector<std::vector<double> > DataSet;
  for(int k=0;k<list->GetSize();k++) {
    std::vector<double> kDataSet;
    for(int i=BinLow;i<=BinHigh;i++)
      kDataSet.push_back((double)((TH1F*)list->At(k))->GetBinContent(i)/mBinWidth.at(i-1)/mNentries.at(k));
    DataSet.push_back(kDataSet);
  }

  //Calculate Log Likelihood! (Binned extended version: Cowan p.89 Eq.: 6.46)
  // -log(Llh)= -(Sum(n_i*log(m_i))-m_tot), m=model, n=data
  //Relative parameters need to be converted to absolute event numbers!

  double m_tot=0;
  for(int i=0;i<list->GetSize()-1;i++)
    for(int j=0;j<mNbins;j++)
      m_tot+=par[i]*DataSet[i+1].at(j)*mBinWidth.at(j)*mNentries.at(0);

  double LogL=0;
  for(int i=0;i<mNbins;i++) {

    double logi=0;
    double TempSum=0;
    for(int k=1;k<list->GetSize();k++) TempSum+=DataSet[k].at(i);

    if(TempSum!=0){ //only use points for which template information exists
      for(int k=1;k<list->GetSize();k++)
        logi+=par[k-1]*DataSet[k].at(i)*mNentries.at(0)*mBinWidth.at(i);

      logi=TMath::Log(logi);
      LogL+=DataSet[0].at(i)*mNentries.at(0)*mBinWidth.at(i)*logi;
    }
  }

  f=-(LogL-m_tot);
}


/**
 * @brief Tests the compatibility of histograms
 *
 * Checks if the data and template histograms are compatible. They need the same number of bins, binwidth, and bin edges.
 */
bool Utilities::TemplateFitter::TestHistogramCompatibility(){

  //INFO_OUT << "Compatibility..." << std::endl;

  for(int i=0;i<(int)fInputTemplates.size();i++){
    //check minimum, maximum, bin number and bin width
    if(fInputTemplates.at(i)->GetNbinsX()!=fInputData->GetNbinsX())
      return false;

    if(fInputTemplates.at(i)->GetEntries()==0)
      return false;

    for(int k=1;k<=fInputData->GetNbinsX();k++)
      if(fInputTemplates.at(i)->GetXaxis()->GetBinLowEdge(k)!=fInputData->GetXaxis()->GetBinLowEdge(k)){ return false; }
  }

  return true;
}


/**
 * @brief Get Relative Result of fit
 * This represents the relative amount of a template form found in the data.
 * Example: If the first entry is 0.2, then 20% of the data distribution is due to template 1.
 * The sum of all sould add up to unity within uncertainties.
 */
const std::vector<double>& Utilities::TemplateFitter::GetRelativeResult() const {
  if(!fHasBeenFit)
    WARN_OUT << "No fit was done!" << std::endl;

  return fRelative;
}

/**
 * @brief Get Absolute Result of fit
 * This represents the absolute amount of a template form found in the data.
 * Example: If the first entry is 700, then 700 events in the data are from template 1.
 */
const std::vector<double>& Utilities::TemplateFitter::GetAbsoluteResult() const {
  if(!fHasBeenFit)
    WARN_OUT << "No fit was done!" << std::endl;

  return fAbsolute;
}

/**
 * @brief Get Uncertainty of relative result of fit.
 *
 * This is the uncertainty of the values of GetRelativeResult
 */
const std::vector<double>& Utilities::TemplateFitter::GetRelativeResultError() const {
  if(!fHasBeenFit)
    WARN_OUT << "No fit was done!" << std::endl;

  return fRelativeError;
}

/**
 * @brief Get Uncertainty of absolute result of fit.
 *
 * This is the uncertainty of the values of GetAbsoluteResult
 */
const std::vector<double>& Utilities::TemplateFitter::GetAbsoluteResultError() const {
  if(!fHasBeenFit)
    WARN_OUT << "No fit was done!" << std::endl;

  return fAbsoluteError;
}


}
