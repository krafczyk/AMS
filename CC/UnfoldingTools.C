#include "UnfoldingTools.h"
#include "TRandom.h"
#include <iostream>
#include "math.h"

using namespace std;

//ClassImp(BayesianUnfolder);

int BayesianUnfolder::MaxIters=1000;

void BayesianUnfolder::setMigrationMatrixFromJoint(TH2F &joint){
  MigrationMatrix=joint;  MigrationMatrix.Reset();
  TH1D *normalization=joint.ProjectionX("_normalization",0,joint.GetNbinsY()+1);

  for(int i=1;i<=MigrationMatrix.GetNbinsX();i++){
    double sum=normalization->GetBinContent(i);
    if(sum<=0) continue;
    for(int j=1;j<=MigrationMatrix.GetNbinsY();j++) MigrationMatrix.SetBinContent(i,j,joint.GetBinContent(i,j)/sum);
  }
  delete normalization;
}


bool BayesianUnfolder::isOK(){
// Initialize the prior in case it has not been done yet
  if(Prior.GetNbinsX()==1){
    cout<<"BayesianUnfolder -- Prior not yet initialized."<<endl;
    Failed=true;
    return false;
  }

  if(MigrationMatrix.GetNbinsX()==1 && MigrationMatrix.GetNbinsY()==1){
    cout<<"BayesianUnfolder -- Migration matrix not yet initialized."<<endl;
    Failed=true;
    return false;
  }

  Failed=false;
  return true;
}

void BayesianUnfolder::computeUnfoldingMatrix(){
  if(!isOK()) return;

  UnfoldingMatrix=MigrationMatrix; UnfoldingMatrix.Reset();

  for(int i=1;i<=MigrationMatrix.GetNbinsX();i++){
    double x=MigrationMatrix.GetXaxis()->GetBinCenter(i);
    double width=MigrationMatrix.GetXaxis()->GetBinWidth(i);
    int priorBin=Prior.GetXaxis()->FindBin(x);
    double weight=Prior.GetBinContent(priorBin)*width/Prior.GetXaxis()->GetBinWidth(priorBin);
    if(weight<=0) continue;
    for(int j=1;j<=MigrationMatrix.GetNbinsY();j++) UnfoldingMatrix.SetBinContent(i,j,MigrationMatrix.GetBinContent(i,j)*weight);
  }

  TH1F *h=(TH1F*)UnfoldingMatrix.ProjectionY("_norma",1,UnfoldingMatrix.GetNbinsX());
  Measured=*h;
  delete h;

  for(int j=1;j<=UnfoldingMatrix.GetNbinsY();j++){
    if(Measured.GetBinContent(j)<=0) continue;
    for(int i=1;i<=UnfoldingMatrix.GetNbinsX();i++) UnfoldingMatrix.SetBinContent(i,j,UnfoldingMatrix.GetBinContent(i,j)/Measured.GetBinContent(j));
  }
}



void BayesianUnfolder::BayesianUnfoldingStep(TH1F &measured){
  Prior=Posterior;
  computeUnfoldingMatrix();

  /*
  TCanvas lala;
  lala.Divide(2,1);
  lala.cd(1);
  MigrationMatrix.Draw("colz");
  lala.cd(2);
  UnfoldingMatrix.Draw("colz");
  gPad->WaitPrimitive();
  */

  if(Failed) return;


  //  cout<<"COPYING PRIOR TO POSTERIOR"<<endl;
  Posterior=Prior;
  Posterior.Reset();

  //  cout<<"FILLING POSTERIOR"<<endl;
  for(int j=1;j<=UnfoldingMatrix.GetNbinsY();j++){
    double y=UnfoldingMatrix.GetYaxis()->GetBinCenter(j);
    double width=UnfoldingMatrix.GetXaxis()->GetBinWidth(j);
    int measuredBin=measured.GetXaxis()->FindBin(y);
    double weight=measured.GetBinContent(measuredBin)*width/measured.GetXaxis()->GetBinWidth(measuredBin);
    if(weight<=0) continue;
    for(int i=1;i<=UnfoldingMatrix.GetNbinsX();i++){
      double x=UnfoldingMatrix.GetXaxis()->GetBinCenter(i);
      Posterior.Fill(x,weight*UnfoldingMatrix.GetBinContent(i,j));
    }
  }


  /*
  {
    TCanvas lala;
    Prior.SetLineColor(1);
    Posterior.SetLineColor(2);   
    Prior.Draw();
    Posterior.Draw("same");
    gPad->WaitPrimitive();
  }
  */

  // Compute the measured distribution with this new prior
  //  cout<<"FILLING MEASURED"<<endl;
  Measured=measured;
  Measured.Reset();

  for(int i=1;i<=MigrationMatrix.GetNbinsX();i++){
    double x=MigrationMatrix.GetXaxis()->GetBinCenter(i);
    double width=MigrationMatrix.GetXaxis()->GetBinWidth(i);
    int posteriorBin=Posterior.GetXaxis()->FindBin(x);
    double posteriorWeight=Posterior.GetBinContent(posteriorBin)/Posterior.GetXaxis()->GetBinWidth(posteriorBin)*width;
    if(posteriorWeight<=0) continue;
    for(int j=1;j<=MigrationMatrix.GetNbinsY();j++){
      double y=MigrationMatrix.GetYaxis()->GetBinCenter(j);
      Measured.Fill(y,MigrationMatrix.GetBinContent(i,j)*posteriorWeight);
    }
  }
  /*
  {
    TCanvas lala;
    measured.SetLineColor(1);
    Measured.SetLineColor(2);   
    measured.Draw();
    Measured.Draw("same");
    gPad->WaitPrimitive();
  }
  */

  //  cout<<"DONE"<<endl;
}


double BayesianUnfolder::estimateKL(TH1F &prior,TH1F &posterior){
  double sum=0;
  for(int i=1;i<=prior.GetNbinsX();i++) if(posterior.GetBinContent(i)) sum+=-posterior.GetBinContent(i)*log(prior.GetBinContent(i)/posterior.GetBinContent(i));
  return sum; // /posterior.Integral(1,posterior.GetNbinsX());
}


double BayesianUnfolder::getChi2(TH1F &measured){
  double sum=0;
  int counter=0;
  for(int i=1;i<=measured.GetNbinsX();i++){
    if(Measured.GetBinContent(i)<=0) continue;
    counter++;
    sum+=(measured.GetBinContent(i)-Measured.GetBinContent(i))*(measured.GetBinContent(i)-Measured.GetBinContent(i))/Measured.GetBinContent(i);
  }
  return sum/(counter-1);
}


void BayesianUnfolder::run(TH1F &measured,TH1F &unfolded,int regularization,double maxKLchange,double maxChi2change){
  // Set a stupid prior
  Prior=unfolded;
  for(int i=1;i<=Prior.GetNbinsX();Prior.SetBinContent(i++,1));
  Posterior=Prior;
  
  // Initial check values
  double currentChi2=HUGE_VAL;
  double currentKL=HUGE_VAL;

  // Main loop
  int ii=0;
  for(;ii<MaxIters;ii++){ 
    TH1F prior=Posterior; // Store it without smoothing for real comparison
    if(regularization>0) Posterior.Smooth(regularization);
    BayesianUnfoldingStep(measured);
    double chi2=getChi2(measured);
    double kl=estimateKL(prior,Posterior);
    Failed=false;
    if(fabs(chi2-currentChi2)<maxChi2change) break;
    if(fabs(kl-currentKL)<maxKLchange) break;
    Failed=true;
    currentChi2=chi2;
    currentKL=kl;
    if(ii%10==0 && Verbose) cout<<"Iteration "<<ii<<" Current chi2/ndof="<<currentChi2<<"  Current KL divergence="<<currentKL<<endl;
  }
  if(Verbose && !Failed) cout<<"Converged after "<<ii<<" iterations with chi2/ndof="<<currentChi2<<"  and KL divergence="<<currentKL<<endl;
  if(Verbose && Failed) cout<<"Reached maximum number of iteratios with chi2/ndof="<<currentChi2<<"  and KL divergence="<<currentKL<<endl;

  unfolded=Posterior;
}


void BayesianUnfolder::computeAll(TH2F &jointPDF,TH1F &measured,             // Inputs
				  TH1F &unfolded,                            // Output
				  int errorComputationSamples,           // Samples for MC error computation    
				  bool fluctuateMatrix,bool fluctuateInput,
				  int regularization,double maxKLchange,double maxChi2change){
  // Get the result
  setMigrationMatrixFromJoint(jointPDF);
  run(measured,unfolded,regularization,maxKLchange,maxChi2change);

  // Start MC computation of errors
  TH1F sum2=unfolded; sum2.Reset();
  TH1F sum=unfolded; sum.Reset();

  cout<<"Computing errors"<<endl; 
  Verbose=false;
  for(int ii=0;ii<errorComputationSamples;ii++){
    cerr<<"#";
    TH2F matrix=jointPDF;
    TH1F input=measured;
    TH1F output=unfolded;
    
    // Fluctuare inputs
    if(fluctuateMatrix)
      for(int i=0;i<=jointPDF.GetNbinsX()+1;i++)
	for(int j=1;j<=jointPDF.GetNbinsY();j++)
	  matrix.SetBinContent(i,j,gRandom->Poisson(jointPDF.GetBinContent(i,j)));

    if(fluctuateInput)
      for(int i=1;i<=measured.GetNbinsX();i++)
	input.SetBinContent(i,gRandom->Poisson(measured.GetBinContent(i)));
  
    // Create the unfolder and run it
    BayesianUnfolder unfolder;
    unfolder.setMigrationMatrixFromJoint(matrix);
    run(input,output,regularization,maxKLchange,maxChi2change);    
    
    // Accumulate the result
    for(int i=1;i<=sum2.GetNbinsX();i++) sum2.SetBinContent(i,sum2.GetBinContent(i)+output.GetBinContent(i)*output.GetBinContent(i));
    for(int i=1;i<=sum.GetNbinsX();i++) sum.SetBinContent(i,sum.GetBinContent(i)+output.GetBinContent(i));
  }
  cerr<<endl;
  Verbose=true;
  
  // Add errors to "unfolded"
  for(int i=1;i<=unfolded.GetNbinsX();i++){
    double error=sum2.GetBinContent(i)/errorComputationSamples-sum.GetBinContent(i)*sum.GetBinContent(i)/errorComputationSamples/errorComputationSamples;
    error=(error<=0)?0:sqrt(error);
    unfolded.SetBinError(i,error);
  }

}



