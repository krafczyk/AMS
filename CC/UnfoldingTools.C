#include "UnfoldingTools.h"
#include "TRandom.h"
#include <iostream>
#include "math.h"
#include "TSpline.h"

using namespace std;


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


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void DEMinimizer::initMinimizer(TH1D &range,double multiplicationFactor){
  Population.clear();

  int size=int(multiplicationFactor*range.GetNbinsX()+0.5);
  for(int i=0;i<size;i++){
    TH1D o=range; 
    for(int j=1;j<=o.GetNbinsX();j++) o.SetBinContent(j,range.GetBinContent(j)+Random.Gaus()*range.GetBinError(j));
    Population.push_back(o);
  }
}

void DEMinimizer::initMinimizer(TH1D &lower,TH1D &upper,double multiplicationFactor){
  Population.clear();
 
  TH1D d=upper-lower;
  int size=int(multiplicationFactor*upper.GetNbinsX()+0.5);
  for(int i=0;i<size;i++){
    TH1D o=d; 
    for(int j=1;j<=o.GetNbinsX();j++) o.SetBinContent(j,lower.GetBinContent(j)+Random.Uniform()*d.GetBinContent(j));
    Population.push_back(o);
  }
}


bool DEMinimizer::searchMinimum(TH1D &output,double maxChangeAllowed,int batchSize,int maxEvaluations){
  if(Function==NULL){
    cerr<<"DEMinimizer::searchMinimum -- Function not declared. Use DEMinimizer::setFunction"<<endl;
    return false;
  }

  if(!Population.size()){
    cerr<<"DEMinimizer::searchMinimum -- Initial search space not defined. Use DEMinimizer::initMinimizer"<<endl;  
    return false;
  }

  // Current best
  double best=HUGE_VAL;
  int bestIndex=-1;

  // Compute the first round of values
  PopulationValues.clear();
  for(int i=0;i<Population.size();i++){
    double value=(*Function)(Population[i]);
    if(value!=value) value=HUGE_VAL;
    PopulationValues.push_back(value);

    if(value<best){best=value;bestIndex=i;}
  }
  

  double prev_best=HUGE_VAL;
  bool converged=false;

  if(Verbosity){
    cout<<"Starting DE Minimization."<<endl
	<<"   Maximum number of function calls: "<<maxEvaluations<<endl
	<<"   Checking convergence every "<<batchSize<<" calls"<<endl
	<<"   Convergence when function change is smaller than "<<maxChangeAllowed<<endl
	<<"   Population size "<<Population.size()<<endl;
  }


  // Start minimization
  for(int call=0;call<maxEvaluations;call++){
    
    ///////////////////////
    // Perform a DE step //
    ///////////////////////

    //Dither F parameters: This allows to explore several scales
    double F=Random.Uniform(0.0,1.0);  
    // Dither CR parameter: This allows to explore different correlation factors
    double CR=Random.Uniform(0.1,0.9); 

    // Select the agent being mutated
    int current=Random.Integer(Population.size());

    // Select the three crossing candidates
    int candidates[3]={-1,-1,-1};

    for(int i=0;i<3;i++){
      int now=Random.Integer(Population.size());

      if(now==current){i-=1; continue;}
      bool repeat=false;
      for(int j=0;j<3;j++) if(candidates[j]==now) repeat=true;
      if(repeat){i-=1; continue;}
      candidates[i]=now;
    }

    // Build the crossing vector
    TH1D cr=Population[candidates[0]]+(Population[candidates[1]]-Population[candidates[2]])*F;
    
    // Create the candidate parameters
    bool shouldCreate=true;
    TH1D candidateH=Population[current];

    for(int i=1;i<=candidateH.GetNbinsX();i++){
      if(Random.Uniform()>CR) continue;
      candidateH.SetBinContent(i,cr.GetBinContent(i));
      shouldCreate=false;
    }

    // If unsuccesfull, force the minimum change
    if(shouldCreate){
      int i=Random.Integer(candidateH.GetNbinsX());
      candidateH.SetBinContent(i,cr.GetBinContent(i));
    }


    //Evaluate the fit function
    double candidateV=(*Function)(candidateH);
    if(candidateV!=candidateV) candidateV=HUGE_VAL;

    if(candidateV<PopulationValues[current]){
      PopulationValues[current]=candidateV;
      Population[current]=candidateH;

      if(candidateV<best){
        best=candidateV;
        bestIndex=current;
      }
    }


    ////////////////////////////////
    // Check if we have converged //
    ////////////////////////////////
    if(Verbosity && call%500==0) cerr<<".";  // Show some heartbeat

    if(call%batchSize==0){
      // Say something
      if(Verbosity) cout<<"Function calls: "<<call+Population.size()<<" CURRENT BEST VALUE: "<<best<<endl;
      // Check if converged 
      if(fabs(prev_best-best)<maxChangeAllowed){
	converged=true;
	break;
      }
      // Update current best
      prev_best=best;
    }
  }
  if(bestIndex>-1) output=Population[bestIndex];
  Parameters=output;
  return converged;
}


void DEMinimizer::computeMCErrors(TH1D &output,int samples, void (*init)(void) ){
  // Accumulators
  TH1D sum=Parameters; sum.Reset();
  TH1D sum2=sum;
  TH1D weights=sum;

  // The sampler
  MCSampler sampler;
  sampler.Random=Random;
  sampler.setMinimum(Parameters);
  sampler.setFunction(Function);
  sampler.initSampler();

  for(int i=0;i<samples;i++){
    TH1D local;
    double w;
    if(init) (*init)();
    sampler.pickSample(local,w);

    //Fill adders
    for(int j=1;j<=Parameters.GetNbinsX();j++){
      double y=local.GetBinContent(j);
      weights.AddBinContent(j,w);
      sum.AddBinContent(j,y*w);
      sum2.AddBinContent(j,y*y*w);
    }
  }
  
  // Sets the error and inform the user  
  for(int i=1;i<=Parameters.GetNbinsX();i++){
    double mean=sum.GetBinContent(i)/weights.GetBinContent(i);
    double mean2=sum2.GetBinContent(i)/weights.GetBinContent(i);
    double error=mean2-mean*mean;
    error=error<0?0:sqrt(error);
    Parameters.SetBinError(i,error);
  }

  if(Verbosity){
    cout<<endl<<"ERROR COMPUTATION FINISHED: "<<endl;
    for(int i=1;i<=Parameters.GetNbinsX();i++){
      cout<<"Parameter "<<i<<": "<<Parameters.GetBinContent(i)<<" +/- "<<Parameters.GetBinError(i)<<endl; 
    }
    cout<<endl;
  }

  output=Parameters;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

double MCSampler::scanParameter(int par,const double scale){
  // Assume a cuadratix shape, centered in 0, and find the rest of parameters
  double x[2]={0,scale}; // Initial guess
  double y[2];
  y[0]=(*Function)(Parameters);
  double origin=y[0];
  TH1D newpars=Parameters;
  newpars.SetBinContent(par,Parameters.GetBinContent(par)*(1+x[1]));
  y[1]=(*Function)(newpars);

  double sigma=2*(y[1]-y[0])/Parameters.GetBinContent(par)/Parameters.GetBinContent(par)/x[1]/x[1];
  if(sigma<=0) return 0;
  return sqrt(1/sigma);
}



void MCSampler::initSampler(){
  Sigmas=Parameters;
  for(int i=1;i<=Parameters.GetNbinsX();i++){
    Sigmas.SetBinContent(i,2*scanParameter(i));
  }
}


void MCSampler::pickSample(TH1D &sample,double &weight){
  if(Sigmas.GetNbinsX()!=Parameters.GetNbinsX()) initSampler(); 
  sample=Parameters;
  double logProb=(*Function)(Parameters);
  double logProbGen=0;

  for(int i=1;i<=Parameters.GetNbinsX();i++){
    double sigma=Sigmas.GetBinContent(i);
    if(sigma==0) continue;
    double value=Random.Gaus();
    logProbGen+=-0.5*value*value;
    sample.SetBinContent(i,Parameters.GetBinContent(i)+value*sigma);
  }
  
  weight=exp(logProb-(*Function)(sample)-logProbGen);
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

