#include "UnfoldingTools.h"
#include "TRandom.h"
#include <iostream>
#include "math.h"
#include "TSpline.h"
#include "TProfile.h"

using namespace std;


int BayesianUnfolder::MaxIters=1000;


void BayesianUnfolder::copyH(TH2F &input,TH2D &output){
  // Create an array to store the binning
  Double_t *xbins=new Double_t[input.GetNbinsX()+1];
  Double_t *ybins=new Double_t[input.GetNbinsY()+1];

  // Copy the binning
  for(int i=1;i<=input.GetNbinsX();i++) xbins[i-1]=input.GetXaxis()->GetBinLowEdge(i);
  xbins[input.GetNbinsX()]=input.GetXaxis()->GetBinUpEdge(input.GetNbinsX());

  for(int i=1;i<=input.GetNbinsY();i++) ybins[i-1]=input.GetYaxis()->GetBinLowEdge(i);
  ybins[input.GetNbinsY()]=input.GetYaxis()->GetBinUpEdge(input.GetNbinsY());

  output.SetBins(input.GetNbinsX(),xbins,input.GetNbinsY(),ybins);

  // Fill the histogram
  for(int i=0;i<=input.GetNbinsX()+1;i++)
    for(int j=0;j<=input.GetNbinsY()+1;j++){
      output.SetBinContent(i,j,input.GetBinContent(i,j));
      output.SetBinError(i,j,input.GetBinError(i,j));
    }

  delete[] xbins;
  delete[] ybins;
}

void BayesianUnfolder::copyH(TH2D &input,TH2F &output){
  // Create an array to store the binning
  Double_t *xbins=new Double_t[input.GetNbinsX()+1];
  Double_t *ybins=new Double_t[input.GetNbinsY()+1];

  // Copy the binning
  for(int i=1;i<=input.GetNbinsX();i++) xbins[i-1]=input.GetXaxis()->GetBinLowEdge(i);
  xbins[input.GetNbinsX()]=input.GetXaxis()->GetBinUpEdge(input.GetNbinsX());

  for(int i=1;i<=input.GetNbinsY();i++) ybins[i-1]=input.GetYaxis()->GetBinLowEdge(i);
  ybins[input.GetNbinsY()]=input.GetYaxis()->GetBinUpEdge(input.GetNbinsY());

  output.SetBins(input.GetNbinsX(),xbins,input.GetNbinsY(),ybins);

  // Fill the histogram
  for(int i=0;i<=input.GetNbinsX()+1;i++)
    for(int j=0;j<=input.GetNbinsY()+1;j++){
      output.SetBinContent(i,j,input.GetBinContent(i,j));
      output.SetBinError(i,j,input.GetBinError(i,j));
    }

  delete[] xbins;
  delete[] ybins;
}

void BayesianUnfolder::copyH(TH1F &input,TH1D &output){
  // Create an array to store the binning
  Double_t *xbins=new Double_t[input.GetNbinsX()+1];

  // Copy the binning
  for(int i=1;i<=input.GetNbinsX();i++) xbins[i-1]=input.GetXaxis()->GetBinLowEdge(i);
  xbins[input.GetNbinsX()]=input.GetXaxis()->GetBinUpEdge(input.GetNbinsX());
  output.SetBins(input.GetNbinsX(),xbins);

  // Fill the histogram
  for(int i=0;i<=input.GetNbinsX()+1;i++){
    output.SetBinContent(i,input.GetBinContent(i));
    output.SetBinError(i,input.GetBinError(i));
  }

  delete[] xbins;
}


void BayesianUnfolder::copyH(TH1D &input,TH1F &output){
  // Create an array to store the binning
  Double_t *xbins=new Double_t[input.GetNbinsX()+1];

  // Copy the binning
  for(int i=1;i<=input.GetNbinsX();i++) xbins[i-1]=input.GetXaxis()->GetBinLowEdge(i);
  xbins[input.GetNbinsX()]=input.GetXaxis()->GetBinUpEdge(input.GetNbinsX());
  output.SetBins(input.GetNbinsX(),xbins);

  // Fill the histogram
  for(int i=0;i<=input.GetNbinsX()+1;i++){
    output.SetBinContent(i,input.GetBinContent(i));
    output.SetBinError(i,input.GetBinError(i));
  }
  
  delete[] xbins;
}




void BayesianUnfolder::setMigrationMatrixFromJoint(TH2D &joint){
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

  TH1D *h=(TH1D*)UnfoldingMatrix.ProjectionY("_norma",1,UnfoldingMatrix.GetNbinsX());
  h->SetBit(kMustCleanup);
  Measured=*h;
  Measured.SetName("Measured");
  delete h;

  for(int j=1;j<=UnfoldingMatrix.GetNbinsY();j++){
    if(Measured.GetBinContent(j)<=0) continue;
    for(int i=1;i<=UnfoldingMatrix.GetNbinsX();i++) UnfoldingMatrix.SetBinContent(i,j,UnfoldingMatrix.GetBinContent(i,j)/Measured.GetBinContent(j));
  }
}



void BayesianUnfolder::BayesianUnfoldingStep(TH1D &measured){
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
    double width=UnfoldingMatrix.GetYaxis()->GetBinWidth(j);
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


double BayesianUnfolder::estimateKL(TH1D &prior,TH1D &posterior){
  double sum=0;
  for(int i=1;i<=prior.GetNbinsX();i++) if(posterior.GetBinContent(i)) sum+=-posterior.GetBinContent(i)*log(prior.GetBinContent(i)/posterior.GetBinContent(i));
  return sum; // /posterior.Integral(1,posterior.GetNbinsX());
}


double BayesianUnfolder::getChi2(TH1D &measured){
  double sum=0;
  int counter=0;
  for(int i=1;i<=measured.GetNbinsX();i++){
    if(Measured.GetBinContent(i)<=0) continue;
    counter++;
    sum+=(measured.GetBinContent(i)-Measured.GetBinContent(i))*(measured.GetBinContent(i)-Measured.GetBinContent(i))/Measured.GetBinContent(i);
  }
  return sum/(counter-1);
}


void BayesianUnfolder::run(TH1D &measured,TH1D &unfolded,int regularization,double maxKLchange,double maxChi2change){
  // Set a stupid prior
  Prior=unfolded;
  if(Prior.Integral(1,Prior.GetNbinsX())==0) for(int i=1;i<=Prior.GetNbinsX();Prior.SetBinContent(i++,1));
  Posterior=Prior;
  
  // Initial check values
  double currentChi2=HUGE_VAL;
  double currentKL=HUGE_VAL;

  // Main loop
  int ii=0;
  for(;ii<MaxIters;ii++){ 
    TH1D prior=Posterior; // Store it without smoothing for real comparison
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


void BayesianUnfolder::computeAll(TH2D &jointPDF,TH1D &measured,             // Inputs
				  TH1D &unfolded,                            // Output
				  int errorComputationSamples,           // Samples for MC error computation    
				  bool fluctuateMatrix,bool fluctuateInput,
				  int regularization,double maxKLchange,double maxChi2change){
  // Get the result
  setMigrationMatrixFromJoint(jointPDF);
  run(measured,unfolded,regularization,maxKLchange,maxChi2change);

  // Start MC computation of errors
  TH1D sum2=unfolded; sum2.Reset();
  TH1D sum=unfolded; sum.Reset();

  cout<<"Computing errors"<<endl; 
  Verbose=false;
  for(int ii=0;ii<errorComputationSamples;ii++){
    cerr<<"#";
    TH2D matrix=jointPDF;
    TH1D input=measured;
    TH1D output=unfolded;
    
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

void StochasticUnfolding::copyH(TH2F &input,TH2D &output){
  // Create an array to store the binning
  Double_t *xbins=new Double_t[input.GetNbinsX()+1];
  Double_t *ybins=new Double_t[input.GetNbinsY()+1];

  // Copy the binning
  for(int i=1;i<=input.GetNbinsX();i++) xbins[i-1]=input.GetXaxis()->GetBinLowEdge(i);
  xbins[input.GetNbinsX()]=input.GetXaxis()->GetBinUpEdge(input.GetNbinsX());

  for(int i=1;i<=input.GetNbinsY();i++) ybins[i-1]=input.GetYaxis()->GetBinLowEdge(i);
  ybins[input.GetNbinsY()]=input.GetYaxis()->GetBinUpEdge(input.GetNbinsY());

  output.SetBins(input.GetNbinsX(),xbins,input.GetNbinsY(),ybins);

  // Fill the histogram
  for(int i=0;i<=input.GetNbinsX()+1;i++)
    for(int j=0;j<=input.GetNbinsY()+1;j++){
      output.SetBinContent(i,j,input.GetBinContent(i,j));
      output.SetBinError(i,j,input.GetBinError(i,j));
    }

  delete[] xbins;
  delete[] ybins;
}

void StochasticUnfolding::copyH(TH2D &input,TH2F &output){
  // Create an array to store the binning
  Double_t *xbins=new Double_t[input.GetNbinsX()+1];
  Double_t *ybins=new Double_t[input.GetNbinsY()+1];

  // Copy the binning
  for(int i=1;i<=input.GetNbinsX();i++) xbins[i-1]=input.GetXaxis()->GetBinLowEdge(i);
  xbins[input.GetNbinsX()]=input.GetXaxis()->GetBinUpEdge(input.GetNbinsX());

  for(int i=1;i<=input.GetNbinsY();i++) ybins[i-1]=input.GetYaxis()->GetBinLowEdge(i);
  ybins[input.GetNbinsY()]=input.GetYaxis()->GetBinUpEdge(input.GetNbinsY());

  output.SetBins(input.GetNbinsX(),xbins,input.GetNbinsY(),ybins);

  // Fill the histogram
  for(int i=0;i<=input.GetNbinsX()+1;i++)
    for(int j=0;j<=input.GetNbinsY()+1;j++){
      output.SetBinContent(i,j,input.GetBinContent(i,j));
      output.SetBinError(i,j,input.GetBinError(i,j));
    }

  delete[] xbins;
  delete[] ybins;
}

void StochasticUnfolding::copyH(TH1F &input,TH1D &output){
  // Create an array to store the binning
  Double_t *xbins=new Double_t[input.GetNbinsX()+1];

  // Copy the binning
  for(int i=1;i<=input.GetNbinsX();i++) xbins[i-1]=input.GetXaxis()->GetBinLowEdge(i);
  xbins[input.GetNbinsX()]=input.GetXaxis()->GetBinUpEdge(input.GetNbinsX());
  output.SetBins(input.GetNbinsX(),xbins);

  // Fill the histogram
  for(int i=0;i<=input.GetNbinsX()+1;i++){
    output.SetBinContent(i,input.GetBinContent(i));
    output.SetBinError(i,input.GetBinError(i));
  }

  delete[] xbins;
}


void StochasticUnfolding::copyH(TH1D &input,TH1F &output){
  // Create an array to store the binning
  Double_t *xbins=new Double_t[input.GetNbinsX()+1];

  // Copy the binning
  for(int i=1;i<=input.GetNbinsX();i++) xbins[i-1]=input.GetXaxis()->GetBinLowEdge(i);
  xbins[input.GetNbinsX()]=input.GetXaxis()->GetBinUpEdge(input.GetNbinsX());
  output.SetBins(input.GetNbinsX(),xbins);

  // Fill the histogram
  for(int i=0;i<=input.GetNbinsX()+1;i++){
    output.SetBinContent(i,input.GetBinContent(i));
    output.SetBinError(i,input.GetBinError(i));
  }
  
  delete[] xbins;
}



void StochasticUnfolding::setPrior(TH1D &prior,double dirichletHyperparameter){
  Counter=dirichletHyperparameter*prior.GetNbinsX();
  Prior=prior;
  double norma=Prior.Integral(1,Prior.GetNbinsX());
  if(norma!=0){
    for(int i=1;i<=Prior.GetNbinsX();i++) Prior.SetBinContent(i,Prior.GetBinContent(i)/norma*Counter);
  }else{
    for(int i=1;i<=Prior.GetNbinsX();i++) Prior.SetBinContent(i,1.0/Prior.GetNbinsX()*Counter);
  }
}

#include "TROOT.h" 


void StochasticUnfolding::setResponseMatrixFromJoint(TH2D &joint){
  ResponseMatrix=joint;  ResponseMatrix.Reset();
  TH1D *normalization=    (TH1D*)joint.ProjectionX("_normalizatio",1,joint.GetNbinsY());
  
  for(int i=1;i<=ResponseMatrix.GetNbinsX();i++){
    double sum=normalization->GetBinContent(i);
    if(sum<=0) continue;
    for(int j=1;j<=ResponseMatrix.GetNbinsY();j++) ResponseMatrix.SetBinContent(i,j,joint.GetBinContent(i,j)/sum);
  }

  delete (TH1D*)normalization;

  // Create some arrays to speedup the addEntry function
  start.clear();
  end.clear();

  // Skip the 0 bin since it is not used
  start.push_back(-1);
  end.push_back(-1);
  
  for(int i=1;i<=ResponseMatrix.GetNbinsY();i++){
    
    start.push_back(-1);
    end.push_back(-1);

    // Look for the start
    for(int j=1;j<=ResponseMatrix.GetNbinsX();j++)
      if(ResponseMatrix.GetBinContent(j,i)!=0) {start[start.size()-1]=j;break;}

    // Look for the end
    for(int j=ResponseMatrix.GetNbinsX();j>=1;j--)
      if(ResponseMatrix.GetBinContent(j,i)!=0) {end[end.size()-1]=j;break;}
  }


}


void StochasticUnfolding::addEntry(double measured){
  // Get the bin
  int bin=ResponseMatrix.GetYaxis()->FindBin(measured);
  if(bin<1 || bin>ResponseMatrix.GetNbinsY()) return;

  int myStart=start[bin];
  int myEnd=end[bin];
  if(myStart<0 || myEnd<0) return;

  double normalization=0;
  TH1D current=Prior;    current.Reset();

  for(int i=myStart;i<=myEnd;i++){
    double energy=ResponseMatrix.GetXaxis()->GetBinCenter(i);
    int priorBin=Prior.GetXaxis()->FindBin(energy);
    double weight=ResponseMatrix.GetBinContent(i,bin)*Prior.GetBinContent(priorBin)*ResponseMatrix.GetXaxis()->GetBinWidth(i)/Prior.GetXaxis()->GetBinWidth(priorBin);
    normalization+=weight;
    current.Fill(energy,weight);
  }
  
  for(int i=1;i<=current.GetNbinsX();i++) current.SetBinContent(i,current.GetBinContent(i)/normalization);
  Prior.Add(&current);
  Counter+=1;
}

#include "TCanvas.h"
void StochasticUnfolding::addEntries(TH1D &measuredOrg,double fraction,bool maxRegularization){
  TH1D current=Prior; current.Reset();
  double currentCounter=Prior.Integral(1,Prior.GetNbinsX());
  double normalizationMeasured=measuredOrg.Integral(1,measuredOrg.GetNbinsX());
  int entries=int(min(max(1.0,fraction*currentCounter),normalizationMeasured-Counter)+0.5); // entries to be added
  if(maxRegularization) entries=min(fraction*currentCounter,normalizationMeasured-Counter);

  // Account for realistic fluctuations
  TH1D measured=measuredOrg;
  if(maxRegularization) for(int i=1;i<=measured.GetNbinsX();i++) measured.SetBinContent(i,(entries*measuredOrg.GetBinContent(i)/normalizationMeasured));
  else for(int i=1;i<=measured.GetNbinsX();i++) measured.SetBinContent(i,Random.Poisson(entries*measuredOrg.GetBinContent(i)/normalizationMeasured));
  normalizationMeasured=measured.Integral(1,measured.GetNbinsX());

  if(normalizationMeasured==0) return;


  TH1D folded=measured;
  fold(folded);

  for(int j=1;j<=ResponseMatrix.GetNbinsY();j++){
    double measuredEnergy=ResponseMatrix.GetXaxis()->GetBinCenter(j);
    int measuredBin=folded.GetXaxis()->FindBin(measuredEnergy);
    if(measured.GetBinContent(measuredBin)==0) continue;
    double mratio=measured.GetBinContent(measuredBin)/folded.GetBinContent(measuredBin);


    for(int i=1;i<=ResponseMatrix.GetNbinsX();i++){
      double energy=ResponseMatrix.GetXaxis()->GetBinCenter(i);
      int energyBin=current.GetXaxis()->FindBin(energy);
      double widthRatio=ResponseMatrix.GetXaxis()->GetBinWidth(i)/Prior.GetBinWidth(energyBin);

      double weight=ResponseMatrix.GetBinContent(i,j)*
	(Prior.GetBinContent(energyBin)*widthRatio)*
	mratio;
      
      current.Fill(energy,weight);
    }
  }



  //  current.Scale(entries/normalizationMeasured);
  Prior.Add(&current);
  Counter+=normalizationMeasured;  
}


void StochasticUnfolding::fold(TH1D &output){
  output.Reset();

  for(int i=1;i<=ResponseMatrix.GetNbinsX();i++){
    double x=ResponseMatrix.GetXaxis()->GetBinCenter(i);
    int pbin=Prior.GetXaxis()->FindBin(x);
    double weight=Prior.GetBinContent(pbin)*ResponseMatrix.GetXaxis()->GetBinWidth(i)/Prior.GetXaxis()->GetBinWidth(pbin);
    for(int j=1;j<=ResponseMatrix.GetNbinsY();j++) output.Fill(ResponseMatrix.GetYaxis()->GetBinCenter(j),ResponseMatrix.GetBinContent(i,j)*weight);
  }

  Folded=output; // Store a copy
}


// This should be factorized, by 
void StochasticUnfolding::unfold(TH2D &joint,TH1D &measured,TH1D &output,int samples){
  TH1D accumulator=Prior; accumulator.Reset();
  TH1D accumulator2=accumulator;

  int entries=measured.Integral();
  TH1D originalPrior=Prior;
  double originalCounter=Counter;

  for(int sample=0;sample<samples;sample++){
    //    cout<<"DEALING WITH REALIZATION "<<sample+1<<" of "<<samples<<endl;
    cout<<"#";
    TH2D myJoint=joint;
    for(int i=1;i<=myJoint.GetNbinsX();i++) for(int j=0;j<=myJoint.GetNbinsY()+1;j++) myJoint.SetBinContent(i,j,Random.Poisson(joint.GetBinContent(i,j)));
    Prior=originalPrior;
    Counter=originalCounter;
    setResponseMatrixFromJoint(myJoint);
    for(int i=0;i<entries;i++)addEntry(measured.GetRandom());
    
    // Correct for efficiency
    TH1D total=Prior; total.Reset();
    TH1D seen=total;
  
    for(int i=1;i<=myJoint.GetNbinsX();i++){
      double x=myJoint.GetXaxis()->GetBinCenter(i);
      for(int j=0;j<=myJoint.GetNbinsX()+1;j++){
	if(j!=0 && j!=myJoint.GetNbinsX()+1) seen.Fill(x,myJoint.GetBinContent(i,j));
	total.Fill(x,myJoint.GetBinContent(i,j));
      }
    }
    

    for(int i=1;i<=Prior.GetNbinsX();i++){
      if(seen.GetBinContent(i)<=0) continue;
      double correction=total.GetBinContent(i)/seen.GetBinContent(i);
      Prior.SetBinContent(i,Prior.GetBinContent(i)*correction);
    }

    accumulator.Add(&Prior);
    for(int i=1;i<=accumulator2.GetNbinsX();i++) accumulator2.AddBinContent(i,Prior.GetBinContent(i)*Prior.GetBinContent(i));
  }

  accumulator.Scale(1.0/samples);
  accumulator2.Scale(1.0/samples);
  output=accumulator;

  for(int i=1;i<=output.GetNbinsX();i++){
    double error=accumulator2.GetBinContent(i)-accumulator.GetBinContent(i)*accumulator.GetBinContent(i);
    output.SetBinError(i,error>0?sqrt(error+output.GetBinContent(i)):0);
  }

  
  Prior=output; // Make a copy

  // Correct for efficiency
  TH1D total=Prior; total.Reset();
  TH1D seen=total;
  TH2D &myJoint=joint;
  for(int i=1;i<=myJoint.GetNbinsX();i++){
    double x=myJoint.GetXaxis()->GetBinCenter(i);
    for(int j=0;j<=myJoint.GetNbinsX()+1;j++){
      if(j!=0 && j!=myJoint.GetNbinsX()+1) seen.Fill(x,myJoint.GetBinContent(i,j));
      total.Fill(x,myJoint.GetBinContent(i,j));
    }
  }
  
  
  for(int i=1;i<=Prior.GetNbinsX();i++){
    if(seen.GetBinContent(i)<=0) continue;
    double correction=seen.GetBinContent(i)/total.GetBinContent(i);
    Prior.SetBinContent(i,Prior.GetBinContent(i)*correction);
  }

  TH1D final=measured;
  fold(final);
  Prior=output;
  cout<<endl;
}



void StochasticUnfolding::unfoldFast(TH2D &joint,TH1D &measured,TH1D &output,bool maxRegularization,bool fluctuateMigration,int samples,double fraction){
  TH1D accumulator=Prior; accumulator.Reset();
  TH1D accumulator2=accumulator;

  double entries=measured.Integral();
  TH1D originalPrior=Prior;
  double originalCounter=Counter;

  for(int sample=0;sample<samples;sample++){
    //    cout<<"DEALING WITH REALIZATION "<<sample+1<<" of "<<samples<<endl;
    cout<<"#";
    TH2D myJoint=joint;
    if (fluctuateMigration) for(int i=1;i<=myJoint.GetNbinsX();i++) for(int j=0;j<=myJoint.GetNbinsY()+1;j++) myJoint.SetBinContent(i,j,Random.Poisson(joint.GetBinContent(i,j)));
    else for(int i=1;i<=myJoint.GetNbinsX();i++) for(int j=0;j<=myJoint.GetNbinsY()+1;j++) myJoint.SetBinContent(i,j,(joint.GetBinContent(i,j)));
    Prior=originalPrior;
    //    Counter=originalCounter;
    Counter=0;
    setResponseMatrixFromJoint(myJoint);

    double limit=entries-0.1*sqrt(entries); // solve some precision problems 
    do addEntries(measured,fraction,maxRegularization);while(Counter<limit);


    // Change the normalization
    Prior.Scale(measured.Integral(1,measured.GetNbinsX())/Prior.Integral(1,Prior.GetNbinsX()));


    // Correct for efficiency
    TH1D total=Prior; total.Reset();
    TH1D seen=total;
  
    for(int i=1;i<=myJoint.GetNbinsX();i++){
      double x=myJoint.GetXaxis()->GetBinCenter(i);
      for(int j=0;j<=myJoint.GetNbinsX()+1;j++){
	if(j!=0 && j!=myJoint.GetNbinsX()+1) seen.Fill(x,myJoint.GetBinContent(i,j));
	total.Fill(x,myJoint.GetBinContent(i,j));
      }
    }
    

    for(int i=1;i<=Prior.GetNbinsX();i++){
      if(seen.GetBinContent(i)<=0) continue;
      double correction=total.GetBinContent(i)/seen.GetBinContent(i);
      Prior.SetBinContent(i,Prior.GetBinContent(i)*correction);
    }

    accumulator.Add(&Prior);
    for(int i=1;i<=accumulator2.GetNbinsX();i++) accumulator2.AddBinContent(i,Prior.GetBinContent(i)*Prior.GetBinContent(i));
  }

  accumulator.Scale(1.0/samples);
  accumulator2.Scale(1.0/samples);
  output=accumulator;

  for(int i=1;i<=output.GetNbinsX();i++){
    double error=accumulator2.GetBinContent(i)-accumulator.GetBinContent(i)*accumulator.GetBinContent(i);
    output.SetBinError(i,error>0?sqrt(error+output.GetBinContent(i)):0);
  }

  
  Prior=output; // Make a copy
  TH1D final=measured;
  fold(final);
  cout<<endl;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
FUnfolding *FUnfolding::Current=NULL;

double FUnfolding::costFunction(TH1D &parameters){
  return FUnfolding::Current->cost(parameters);
}


double FUnfolding::cost(TH1D &parameters){
  // Build the energy spectrum
  TH1D spectrum=parameters;
  for(int i=1;i<=spectrum.GetNbinsX();i++) spectrum.SetBinContent(i,exp(parameters.GetBinContent(i))*parameters.GetXaxis()->GetBinWidth(i));

  // Fold it with current Response Matrix
  TH1F output=Measured;
  fold(spectrum,output);

  /*
  cout<<"FOLDING RESULTS"<<endl;
  TCanvas _cc;
  _cc.Divide(2,1);
  _cc.cd(1);
  parameters.Draw();
  _cc.cd(2);
  Measured.Draw();
  output.SetLineColor(2);
  output.Draw("same");
  gPad->WaitPrimitive();
  */

  // Compute the likelihood
  double sum=0;
  for(int i=1;i<=Measured.GetNbinsX();i++){
    sum+=output.GetBinContent(i)-(Measured.GetBinContent(i)?Measured.GetBinContent(i)*log(output.GetBinContent(i)):0);
    if(Measured.GetBinContent(i)>0) sum-=Measured.GetBinContent(i)-Measured.GetBinContent(i)*log(Measured.GetBinContent(i)); // To make interpretation simpler
  }
  double sum1=sum;
  
  // Compute the regularization parameters
  for(int i=1;i<=spectrum.GetNbinsX();i++) sum+=log(sqrt(spectrum.GetBinContent(i))+1);
  
  return sum;
}




void FUnfolding::setResponseMatrixFromJoint(TH2F &joint){
  Joint=joint;
  ResponseMatrix=joint;  ResponseMatrix.Reset();
  TH1F *normalization=    (TH1F*)joint.ProjectionX("_normalizatio",0,joint.GetNbinsY()+1);

  
  for(int i=1;i<=ResponseMatrix.GetNbinsX();i++){
    double sum=normalization->GetBinContent(i);
    if(sum<=0) continue;
    for(int j=1;j<=ResponseMatrix.GetNbinsY();j++) ResponseMatrix.SetBinContent(i,j,joint.GetBinContent(i,j)/sum);
  }
}





void FUnfolding::unfold(TH2F &jointPDF,TH1F &measured,TH1D &output){
  Current=this;

  cout<<"SETTING RESPONSE MATRIX"<<endl;
  setResponseMatrixFromJoint(jointPDF);
  Measured=measured;

  
  cout<<"CREATING A INITIAL GUESS"<<endl;
  // I ignore the hint parameters and set mine by hand
  for(int i=1;i<=output.GetNbinsX();i++){
    double x=output.GetXaxis()->GetBinCenter(i);
    int bin=measured.GetXaxis()->FindBin(x);
    double value=measured.GetBinContent(bin);
    output.SetBinContent(i,log(value/measured.GetXaxis()->GetBinWidth(bin)));
    output.SetBinError(i,output.GetBinContent(i)*0.5);
  }
  

  // Start the minimizer
  cout<<"STARTING THE MINIMIZER"<<endl;
  bool success=false;
  std::vector<TH1D> Population;
  std::vector<double> PopulationValues;
  int counter=0;
  //  do{
  {
    DEMinimizer minimizer;
    minimizer.setFunction(FUnfolding::costFunction);
    if(Population.size()==0){
      minimizer.initMinimizer(output);
    }else{
      minimizer.Population=Population;
      minimizer.PopulationValues=PopulationValues;
    }
    success=minimizer.searchMinimum(output,0,2000,1e6);
    
    success=true;

    if(success) minimizer.computeMCErrors(output);  // By the moment forget about matrix fluctuation
    //    if(counter==9) {minimizer.computeMCErrors(output);break;} // By the moment forget about matrix fluctuation
    Population=minimizer.Population;
    PopulationValues=minimizer.PopulationValues;
    counter++;
    //  }while(!success);
  }

  for(int i=1;i<=output.GetNbinsX();i++) output.SetBinContent(i,exp(output.GetBinContent(i))*output.GetXaxis()->GetBinWidth(i));
  for(int i=1;i<=output.GetNbinsX();i++) output.SetBinError(i,output.GetBinContent(i)*output.GetBinError(i));
}


void FUnfolding::fold(TH1D &parameters,TH1F &output){
  output.Reset();

  for(int i=1;i<=ResponseMatrix.GetNbinsX();i++){
    double x=ResponseMatrix.GetXaxis()->GetBinCenter(i);
    double width=ResponseMatrix.GetXaxis()->GetBinWidth(i);
    int posteriorBin=parameters.GetXaxis()->FindBin(x);
    double posteriorWeight=parameters.GetBinContent(posteriorBin)/parameters.GetXaxis()->GetBinWidth(posteriorBin)*width;
    if(posteriorWeight<=0) continue;
    for(int j=1;j<=ResponseMatrix.GetNbinsY();j++){
      double y=ResponseMatrix.GetYaxis()->GetBinCenter(j);
      output.Fill(y,ResponseMatrix.GetBinContent(i,j)*posteriorWeight);
    }
  }  


}

void SamplingUnfolding::copyH(TH2F &input,TH2D &output){
  // Create an array to store the binning
  Double_t *xbins=new Double_t[input.GetNbinsX()+1];
  Double_t *ybins=new Double_t[input.GetNbinsY()+1];

  // Copy the binning
  for(int i=1;i<=input.GetNbinsX();i++) xbins[i-1]=input.GetXaxis()->GetBinLowEdge(i);
  xbins[input.GetNbinsX()]=input.GetXaxis()->GetBinUpEdge(input.GetNbinsX());

  for(int i=1;i<=input.GetNbinsY();i++) ybins[i-1]=input.GetYaxis()->GetBinLowEdge(i);
  ybins[input.GetNbinsY()]=input.GetYaxis()->GetBinUpEdge(input.GetNbinsY());

  output.SetBins(input.GetNbinsX(),xbins,input.GetNbinsY(),ybins);

  // Fill the histogram
  for(int i=0;i<=input.GetNbinsX()+1;i++)
    for(int j=0;j<=input.GetNbinsY()+1;j++){
      output.SetBinContent(i,j,input.GetBinContent(i,j));
      output.SetBinError(i,j,input.GetBinError(i,j));
    }

  delete[] xbins;
  delete[] ybins;
}

void SamplingUnfolding::copyH(TH2D &input,TH2F &output){
  // Create an array to store the binning
  Double_t *xbins=new Double_t[input.GetNbinsX()+1];
  Double_t *ybins=new Double_t[input.GetNbinsY()+1];

  // Copy the binning
  for(int i=1;i<=input.GetNbinsX();i++) xbins[i-1]=input.GetXaxis()->GetBinLowEdge(i);
  xbins[input.GetNbinsX()]=input.GetXaxis()->GetBinUpEdge(input.GetNbinsX());

  for(int i=1;i<=input.GetNbinsY();i++) ybins[i-1]=input.GetYaxis()->GetBinLowEdge(i);
  ybins[input.GetNbinsY()]=input.GetYaxis()->GetBinUpEdge(input.GetNbinsY());

  output.SetBins(input.GetNbinsX(),xbins,input.GetNbinsY(),ybins);

  // Fill the histogram
  for(int i=0;i<=input.GetNbinsX()+1;i++)
    for(int j=0;j<=input.GetNbinsY()+1;j++){
      output.SetBinContent(i,j,input.GetBinContent(i,j));
      output.SetBinError(i,j,input.GetBinError(i,j));
    }

  delete[] xbins;
  delete[] ybins;
}

void SamplingUnfolding::copyH(TH1F &input,TH1D &output){
  // Create an array to store the binning
  Double_t *xbins=new Double_t[input.GetNbinsX()+1];

  // Copy the binning
  for(int i=1;i<=input.GetNbinsX();i++) xbins[i-1]=input.GetXaxis()->GetBinLowEdge(i);
  xbins[input.GetNbinsX()]=input.GetXaxis()->GetBinUpEdge(input.GetNbinsX());
  output.SetBins(input.GetNbinsX(),xbins);

  // Fill the histogram
  for(int i=0;i<=input.GetNbinsX()+1;i++){
    output.SetBinContent(i,input.GetBinContent(i));
    output.SetBinError(i,input.GetBinError(i));
  }

  delete[] xbins;
}


void SamplingUnfolding::copyH(TH1D &input,TH1F &output){
  // Create an array to store the binning
  Double_t *xbins=new Double_t[input.GetNbinsX()+1];

  // Copy the binning
  for(int i=1;i<=input.GetNbinsX();i++) xbins[i-1]=input.GetXaxis()->GetBinLowEdge(i);
  xbins[input.GetNbinsX()]=input.GetXaxis()->GetBinUpEdge(input.GetNbinsX());
  output.SetBins(input.GetNbinsX(),xbins);

  // Fill the histogram
  for(int i=0;i<=input.GetNbinsX()+1;i++){
    output.SetBinContent(i,input.GetBinContent(i));
    output.SetBinError(i,input.GetBinError(i));
  }
  
  delete[] xbins;
}
 
#define MASKED -1000 

void SamplingUnfolding::updateSigmas(){
  Sigmas=Prior;
  Samples=Prior;
  Samples.Reset();
  Accepted=Samples;
  
  for(int i=1;i<=Prior.GetNbinsX();i++){
    if(Prior.GetBinContent(i)==MASKED) Sigmas.SetBinContent(i,0);
    else Sigmas.SetBinContent(i,exp(-Prior.GetBinContent(i)/2));
  }
}

void SamplingUnfolding::setPrior(TH1D &prior){
  Prior=prior;
  Prior.Reset();
  for(int i=1;i<=Prior.GetNbinsX();i++){
    if(prior.GetBinContent(i)<=0) Prior.SetBinContent(i,MASKED); // This is amask
    else Prior.SetBinContent(i,log(prior.GetBinContent(i)));
  }

  fold(prior,FoldedPrior);
  updateSigmas();

  
}



void SamplingUnfolding::step(TH1D &goal,TH1D &sample){
  // Use Metropolis-Hasting method to modify the prior

  // Ensure the priors are ok
  if(FoldedPrior.GetNbinsX()!=goal.GetNbinsX()){
    FoldedPrior=goal;
    FoldedPrior.Reset();
    TH1D right=Prior;
    for(int i=1;i<=right.GetNbinsX();i++) right.SetBinContent(i,exp(Prior.GetBinContent(i))); 
    fold(right,FoldedPrior);
  }


  // Select a single channel
  int whichBin=-1;
  do whichBin=1+Random.Integer(Prior.GetNbinsX()); while(Prior.GetBinContent(whichBin)==MASKED);

  //  cout<<"USING WHICHBIN "<<whichBin<<endl;

  // Sample a step
  double delta=Random.Gaus()*Sigmas.GetBinContent(whichBin);

  if(0)
  {
    TH1D candidate=Prior;
    candidate.AddBinContent(whichBin,delta);
    Prior.SetLineColor(2);
    candidate.SetLineColor(1);
    TCanvas ll;
    Prior.Draw();
    candidate.Draw("same");
    gPad->WaitPrimitive();
  }


  // Compute the change in the log of the probability 
  TH1D change=Prior;
  change.Reset();
  change.SetBinContent(whichBin,exp(Prior.GetBinContent(whichBin))*(exp(delta)-1));
  TH1D increment=goal;
  increment.Reset();
  fold(change,increment);


  TH1D final=Prior;final.AddBinContent(whichBin,delta);
  double logProbDelta=foldedCostFunction(increment,FoldedPrior,goal);
  double logProbDeltaInit=unfoldedCostFunction(final)-unfoldedCostFunction(Prior);
  logProbDelta+=logProbDeltaInit;


  // Perform the step
  double acceptance=log(Random.Uniform());
  Samples.AddBinContent(whichBin,1);

  if(logProbDelta>=acceptance){
    // Accepted
    Accepted.AddBinContent(whichBin,1);

    // Update Prior and folded prior
    FoldedPrior.Add(&increment);
    Prior.AddBinContent(whichBin,delta);
  }else{
    // Do nothing
  }

  // Return the current prior
  sample=Prior;
  for(int i=1;i<=sample.GetNbinsX();i++) sample.SetBinContent(i,exp(sample.GetBinContent(i)));
}


void SamplingUnfolding::setResponseMatrixFromJoint(TH2D &joint){
  ResponseMatrix=joint;  ResponseMatrix.Reset();
  TH1D *normalization=    (TH1D*)joint.ProjectionX("_normalizatio",1,joint.GetNbinsY());
  
  for(int i=1;i<=ResponseMatrix.GetNbinsX();i++){
    double sum=normalization->GetBinContent(i);
    if(sum<=0) continue;
    for(int j=1;j<=ResponseMatrix.GetNbinsY();j++) ResponseMatrix.SetBinContent(i,j,joint.GetBinContent(i,j)/sum);
  }

  delete (TH1D*)normalization;
}

void SamplingUnfolding::fold(TH1D &input,TH1D &output){
  output.Reset();
  for(int i=1;i<=ResponseMatrix.GetNbinsX();i++){
    double x=ResponseMatrix.GetXaxis()->GetBinCenter(i);
    int pbin=input.GetXaxis()->FindBin(x);
    double weight=input.GetBinContent(pbin)*ResponseMatrix.GetXaxis()->GetBinWidth(i)/input.GetXaxis()->GetBinWidth(pbin);
    if(weight) for(int j=1;j<=ResponseMatrix.GetNbinsY();j++) output.Fill(ResponseMatrix.GetYaxis()->GetBinCenter(j),ResponseMatrix.GetBinContent(i,j)*weight);
  }
}


double SamplingUnfolding::unfoldedCostFunction(TH1D &unfolded){
  double sum=0;
  for(int i=2;i<=unfolded.GetNbinsX()-1;i++){
    double y1=exp(unfolded.GetBinContent(i+1))/unfolded.GetXaxis()->GetBinWidth(i+1);
    double y0=exp(unfolded.GetBinContent(i-1))/unfolded.GetXaxis()->GetBinWidth(i-1);
    double x1=unfolded.GetXaxis()->GetBinCenter(i+1);
    double x0=unfolded.GetXaxis()->GetBinCenter(i-1);
    double prediction=y0+(y1-y0)/(x1-x0)*(unfolded.GetXaxis()->GetBinCenter(i)-x0);
    if(prediction==0) continue;
    prediction*=unfolded.GetXaxis()->GetBinWidth(i);
    double v=prediction-exp(unfolded.GetBinContent(i));
    sum+=-0.5*v*v/prediction;
  }
  return sum*Regularization;
}

double SamplingUnfolding::foldedCostFunction(TH1D &increment,TH1D &folded,TH1D &goal){
  double sum=0;

  for(int i=1;i<=goal.GetNbinsX();i++){
    double current=folded.GetBinContent(i);
    double delta=increment.GetBinContent(i)+current;
    double n=goal.GetBinContent(i);

    sum+=-delta+(n!=0?n*log(delta):0);
    sum-=-current+(n!=0?n*log(current):0);
  }
  return sum;
}


void SamplingUnfolding::computeAll(TH2D &jointPDF,TH1D &measured,TH1D &unfolded,double reg,int burn_in,int samples){
  Regularization=reg;
  setResponseMatrixFromJoint(jointPDF);
  setPrior(measured);
  TH1D current;

  cout<<"BURN-IN PHASE..."<<endl;
  for(int i=0;i<burn_in*measured.GetNbinsX();i++){
    step(measured,current);
    if(i%(100*measured.GetNbinsX())==0) cout<<"... "<<i/measured.GetNbinsX()<<" of "<<burn_in<<" done"<<endl;
  }


  double *array=new double[current.GetNbinsX()+1];
  for(int i=1;i<=current.GetNbinsX()+1;i++) array[i-1]=current.GetXaxis()->GetBinLowEdge(i);

  cout<<"UNFOLDING PHASE..."<<endl;
  TProfile final_result("","",current.GetNbinsX(),array,"s");
  for(int i=0;i<samples*measured.GetNbinsX();i++){
    step(measured,current);
    for(int j=1;j<=current.GetNbinsX();j++) final_result.Fill(current.GetXaxis()->GetBinCenter(j),current.GetBinContent(j));
    if(i%(100*measured.GetNbinsX())==0) cout<<"... "<<i/measured.GetNbinsX()<<" of "<<samples<<" done"<<endl;
  }
  unfolded.SetBins(current.GetNbinsX(),array);
  for(int j=1;j<=current.GetNbinsX();j++){
    unfolded.SetBinContent(j,final_result.GetBinContent(j));
    unfolded.SetBinError(j,final_result.GetBinError(j));
  }
}
