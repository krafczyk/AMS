#include "MVAmanager.h"
#include "ConfigParser.h"


#include <TChain.h>
#include <TString.h>
#include <TObjString.h>
#include <TSystem.h>
#include <TROOT.h>

#include <sstream>
#include <string>
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


//using namespace TMVA;

MVAmanager::MVAmanager(ConfigParser*  aconf) {
  theconf=aconf;
  Debug=theconf->Value("general", "Debug");
  reader=0;
  factory=0;
  mycuts=0;
  mycutb=0;
}

MVAmanager::~MVAmanager() {
  if(Debug) cout<<" ~MVAmanager()"<<endl;
  if(reader) delete reader;
  if(factory) delete factory;
 }

/** clear
 */
void MVAmanager::clearpar(vector<AParam*>* v) {
for(vector<AParam*>::iterator ip = v->begin(); ip != v->end(); ++ip){
  delete (*ip);}
 v->clear();
 return;
}
/** fill vriables out of config
 */
int MVAmanager::fillvariables() {
  string fname=theconf->Value("MVA","filepar");
 return  fillvariables(fname);
}


/** fill variables from txt file defined in config or else
*/
int MVAmanager::fillvariables(string fname) {
    if(Debug) cout<<" MVAmanager::fillvariables from file"<<endl;

ifstream infile (fname.c_str());
 if(!infile)  {
    cout<<" *********cannot open  file "<<fname<<endl;;
    return 0;
  }
 if(Debug) cout<<"reading..."<<endl;
 clearpar(&vvar);
 clearpar(&vspecvar);
 clearpar(&vallvar);
 string sname;
 string stitle;
 float val;
 float valmin;
 float valmax;
 int flag;
 int type;
 while(infile>>stitle>>sname>>val>>valmin>>valmax>>flag>>type) {
   vallvar.push_back(new AParam(stitle,sname,val,valmin,valmax,type)); // fill all parameters
   if(flag==1)    vvar.push_back(new AParam(stitle,sname,val,valmin,valmax,type));
   if(flag==-1)   vspecvar.push_back(new AParam(stitle,sname,val,valmin,valmax,type));
 }
 if(Debug) cout<<" MVAmanager::filled "<<vallvar.size()<<endl;
    return 1;
}
 /**  read preselection cuts 
   */
TCut*  MVAmanager::readcuts(string fname) {
ifstream infile (fname.c_str());
 if(fname=="none") return 0;
 if(!infile)  {
    cout<<" *********cannot open  file "<<fname<<endl;;
    return 0;
  }
 string scut;
 getline(infile, scut);
 TCut* ccut=new TCut();
*ccut=scut.c_str();
 if(Debug) cout<<" read  from "<<fname<<" cuts="<<scut<<endl;
 return ccut;

}
  /**  set cuts in training  for signal type 1 or background -1 , 0 is for all
   */
void MVAmanager::setcuts() {
  mycuts=new TCut();
  mycutb=new TCut();
	// define external cuts 
  string fnames=theconf->Value("MVA","sigcutsfile");
 string fnameb=theconf->Value("MVA","bkgcutsfile");

 TCut* ccuts=readcuts(fnames);
 TCut* ccutb=readcuts(fnameb);
	*mycuts= *mycuts && *ccuts;
  	*mycutb=*mycutb && *ccutb;
	return;
}
 /**  set MVA methods
   */
void MVAmanager::setmethods() {
 // This loads the library
   TMVA::Tools::Instance();
   // --- Cut optimisation
   Use["Cuts"]            = 0;
   Use["CutsD"]           = 0;
   Use["CutsPCA"]         = 0;
   Use["CutsGA"]          = 0;
   Use["CutsSA"]          = 0;
   // 
   // --- 1-dimensional likelihood ("naive Bayes estimator")
   Use["Likelihood"]      = 0;
   Use["LikelihoodD"]     = 0; // the "D" extension indicates decorrelated input variables (see option strings)
   Use["LikelihoodPCA"]   = 0; // the "PCA" extension indicates PCA-transformed input variables (see option strings)
   Use["LikelihoodKDE"]   = 0;
   Use["LikelihoodMIX"]   = 0;
   //
   // --- Mutidimensional likelihood and Nearest-Neighbour methodss
   Use["PDERS"]           = 0;
   Use["PDERSD"]          = 0;
   Use["PDERSPCA"]        = 0;
   Use["PDEFoam"]         = 0;
   Use["PDEFoamBoost"]    = 0; // uses generalised MVA method boosting
   Use["KNN"]             = 0; // k-nearest neighbour method
   //
   // --- Linear Discriminant Analysis
   Use["LD"]              = 0; // Linear Discriminant identical to Fisher
   Use["Fisher"]          = 0;
   Use["FisherG"]         = 0;
   Use["BoostedFisher"]   = 0; // uses generalised MVA method boosting
   Use["HMatrix"]         = 0;
   //
   // --- Function Discriminant analysis
   Use["FDA_GA"]          = 0; // minimisation of user-defined function using Genetics Algorithm
   Use["FDA_SA"]          = 0;
   Use["FDA_MC"]          = 0;
   Use["FDA_MT"]          = 0;
   Use["FDA_GAMT"]        = 0;
   Use["FDA_MCMT"]        = 0;
   //
   // --- Neural Networks (all are feed-forward Multilayer Perceptrons)
   Use["MLP"]             = 0; // Recommended ANN
   Use["MLPBFGS"]         = 0; // Recommended ANN with optional training method
   Use["MLPBNN"]          = 0; // Recommended ANN with BFGS training method and bayesian regulator
   Use["CFMlpANN"]        = 0; // Depreciated ANN from ALEPH
   Use["TMlpANN"]         = 0; // ROOT's own ANN
   //
   // --- Support Vector Machine 
   Use["SVM"]             = 0;
   // 
   // --- Boosted Decision Trees
   Use["BDT"]             = 0; // uses Adaptive Boost
   Use["BDTG"]            = 0; // uses Gradient Boost
   Use["BDTB"]            = 0; // uses Bagging
   Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
   Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting 
   // 
   // --- Friedman's RuleFit method, ie, an optimised series of cuts ("rules")
   Use["RuleFit"]         = 0;

     string methodName=theconf->Value("MVA","classifier"); 

	for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
    if (it->first == methodName) it->second = 1;
  }
  
  return;
}

/**---- Book MVA methods
 *
 * Please lookup the various method configuration options in the corresponding cxx files, eg:
 * src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
 * it is possible to preset ranges in the option string in which the cut optimisation should be done:
 * "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable
 */
void  MVAmanager::bookfactory() {
	 if (Debug) cout<<"bookfactory()"<<endl;
         string fmvatree=theconf->Value("MVA","BDT_NTrees");
	 TString BDT_Trees= string(fmvatree);
   // Cut optimisation
   if (Use["Cuts"])
      factory->BookMethod( TMVA::Types::kCuts, "Cuts",
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

   if (Use["CutsD"])
      factory->BookMethod( TMVA::Types::kCuts, "CutsD",
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate" );

   if (Use["CutsPCA"])
      factory->BookMethod( TMVA::Types::kCuts, "CutsPCA",
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA" );

   if (Use["CutsGA"])
      factory->BookMethod( TMVA::Types::kCuts, "CutsGA",
                           "H:!V:FitMethod=GA:CutRangeMin[0]=-10:CutRangeMax[0]=10:VarProp[1]=FMax:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:SC_factor=0.95" );

   if (Use["CutsSA"])
      factory->BookMethod( TMVA::Types::kCuts, "CutsSA",
                           "!H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );


   if (Use["Likelihood"])
      factory->BookMethod( TMVA::Types::kLikelihood, "Likelihood",
                           "H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50" );

   // Decorrelated likelihood
   if (Use["LikelihoodD"])
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodD",
                           "!H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50" );

   // PCA-transformed likelihood
   if (Use["LikelihoodPCA"])
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodPCA",
                           "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA" ); 

   // Use a kernel density estimator to approximate the PDFs
   if (Use["LikelihoodKDE"])
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodKDE",
                           "!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=50" ); 

   // Use a variable-dependent mix of splines and kernel density estimator
   if (Use["LikelihoodMIX"])
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodMIX",
                           "!H:!V:!TransformOutput:PDFInterpolSig[0]=KDE:PDFInterpolBkg[0]=KDE:PDFInterpolSig[1]=KDE:PDFInterpolBkg[1]=KDE:PDFInterpolSig[2]=Spline2:PDFInterpolBkg[2]=Spline2:PDFInterpolSig[3]=Spline2:PDFInterpolBkg[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50" ); 

   // Test the multi-dimensional probability density estimator
   // here are the options strings for the MinMax and RMS methods, respectively:
   //      "!H:!V:VolumeRangeMode=MinMax:DeltaFrac=0.2:KernelEstimator=Gauss:GaussSigma=0.3" );
   //      "!H:!V:VolumeRangeMode=RMS:DeltaFrac=3:KernelEstimator=Gauss:GaussSigma=0.3" );
   if (Use["PDERS"])
      factory->BookMethod( TMVA::Types::kPDERS, "PDERS",
                           "!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600" );

   if (Use["PDERSD"])
      factory->BookMethod( TMVA::Types::kPDERS, "PDERSD",
                           "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=Decorrelate" );

   if (Use["PDERSPCA"])
      factory->BookMethod( TMVA::Types::kPDERS, "PDERSPCA",
                           "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=PCA" );

   // Multi-dimensional likelihood estimator using self-adapting phase-space binning
   if (Use["PDEFoam"])
      factory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoam",
                           "!H:!V:SigBgSeparate=F:TailCut=0.001:VolFrac=0.0666:nActiveCells=500:nSampl=2000:nBin=5:Nmin=100:Kernel=None:Compress=T" );

   if (Use["PDEFoamBoost"])
      factory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoamBoost",
                           "!H:!V:Boost_Num=30:Boost_Transform=linear:SigBgSeparate=F:MaxDepth=4:UseYesNoCell=T:DTLogic=MisClassificationError:FillFoamWithOrigWeights=F:TailCut=0:nActiveCells=500:nBin=20:Nmin=400:Kernel=None:Compress=T" );

   // K-Nearest Neighbour classifier (KNN)
   if (Use["KNN"])
      factory->BookMethod( TMVA::Types::kKNN, "KNN",
                           "H:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim" );

   // H-Matrix (chi2-squared) method
   if (Use["HMatrix"])
      factory->BookMethod( TMVA::Types::kHMatrix, "HMatrix", "!H:!V:VarTransform=None" );

   // Linear discriminant (same as Fisher discriminant)
   if (Use["LD"])
      factory->BookMethod( TMVA::Types::kLD, "LD", "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

   // Fisher discriminant (same as LD)
   if (Use["Fisher"])
      factory->BookMethod( TMVA::Types::kFisher, "Fisher", "H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

   // Fisher with Gauss-transformed input variables
   if (Use["FisherG"])
      factory->BookMethod( TMVA::Types::kFisher, "FisherG", "H:!V:VarTransform=Gauss" );

   // Composite classifier: ensemble (tree) of boosted Fisher classifiers
   if (Use["BoostedFisher"])
      factory->BookMethod( TMVA::Types::kFisher, "BoostedFisher", 
                           "H:!V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2:!Boost_DetailedMonitoring" );

   // Function discrimination analysis (FDA) -- test of various fitters - the recommended one is Minuit (or GA or SA)
   if (Use["FDA_MC"])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_MC",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:SampleSize=100000:Sigma=0.1" );

   if (Use["FDA_GA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_GA",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=300:Cycles=3:Steps=20:Trim=True:SaveBestGen=1" );

   if (Use["FDA_SA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_SA",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=SA:MaxCalls=15000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

   if (Use["FDA_MT"])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_MT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );

   if (Use["FDA_GAMT"])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_GAMT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:Cycles=1:PopSize=5:Steps=5:Trim" );

   if (Use["FDA_MCMT"])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_MCMT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:SampleSize=20" );

   // TMVA ANN
   if (Use["MLP"])
      factory->BookMethod( TMVA::Types::kMLP, "MLP", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator" );


   if (Use["MLPBFGS"])
      factory->BookMethod( TMVA::Types::kMLP, "MLPBFGS", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5,N:TestRate=5:TrainingMethod=GA:!UseRegulator:CreateMVAPdfs=True" );//use regulator to avoid overtraining; TestRate=Test for overtraining performed at each #th epochs

   if (Use["MLPBNN"])
      factory->BookMethod( TMVA::Types::kMLP, "MLPBNN", "H:!V:NeuronType=sigmoid:VarTransform=N,D:NCycles=1000:HiddenLayers=N+2,N,1:TestRate=20:TrainingMethod=BFGS:Sampling=0:ResetStep=105:UseRegulator:CreateMVAPdfs=True" ); // BFGS training with bayesian regulators

   // CF(Clermont-Ferrand)ANN
   if (Use["CFMlpANN"])
      factory->BookMethod( TMVA::Types::kCFMlpANN, "CFMlpANN", "!H:!V:NCycles=50000:HiddenLayers=N,N-1"  ); // n_cycles:#nodes:#nodes:...  

   // Tmlp(Root)ANN
   if (Use["TMlpANN"])
      factory->BookMethod( TMVA::Types::kTMlpANN, "TMlpANN", "H:!V:NCycles=10000:HiddenLayers=N+1,1:LearningMethod=Batch:ValidationFraction=0.5:CreateMVAPdfs=True"  ); // n_cycles:#nodes:#nodes:...HiddenLayers:#of HiddenLayers=2,#of Neurons in the first HiddenLayer=N+1,#of Neurons in the second HiddenLayer=N,N=#of variables

   // Support Vector Machine
   if (Use["SVM"])
      factory->BookMethod( TMVA::Types::kSVM, "SVM", "Gamma=0.25:Tol=0.001:VarTransform=Norm" );

   // Boosted Decision Trees
   if (Use["BDTG"]) // Gradient Boost
      factory->BookMethod( TMVA::Types::kBDT, "BDTG",
                           "!H:!V:NTrees="+BDT_Trees+":BoostType=Grad:UseRandomisedTrees:UseNvars=5:Shrinkage=0.010:GradBaggingFraction=0.5:NodePurityLimit=0.5:nCuts=200:NNodesMax=150" );





   if (Use["BDT"])  // Adaptive Boost
      factory->BookMethod( TMVA::Types::kBDT, "BDT",
                           "!H:!V:NTrees=1000:nEventsMin=150:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );


   if (Use["BDTB"]) // Bagging
      factory->BookMethod( TMVA::Types::kBDT, "BDTB",
                           "!H:!V:NTrees=1000:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );

   if (Use["BDTD"]) // Decorrelation + Adaptive Boost
      factory->BookMethod( TMVA::Types::kBDT, "BDTD",
                           "!H:!V:NTrees=1000:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:VarTransform=D" );

   if (Use["BDTF"])  // Allow Using Fisher discriminant in node splitting for (strong) linearly correlated variables
      factory->BookMethod( TMVA::Types::kBDT, "BDTMitFisher",
                           "!H:!V:NTrees=50:NNodesMax=10:nEventsMin=5:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=200:PruneMethod=NoPruning" );

   // RuleFit -- TMVA implementation of Friedman's method
   if (Use["RuleFit"])
      factory->BookMethod( TMVA::Types::kRuleFit, "RuleFit",
                           "H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02" );

   if(Debug) cout<<" bookfactory is done "<<endl;
   return;
}
/** Init factory for reader 
 */
int  MVAmanager::initfactory() {
  if(Debug) cout<<" MVAmanager::initfactory "<<endl;
 string   sclass0=theconf->Value("MVA","classifier");
 sclass=sclass0; // need direct casting for Config
 string fmvalocation=theconf->Value("MVA","weightfile_location");
  string fmvaout=theconf->Value("MVA","fmvaout");
  string fout=string(fmvalocation+fmvaout);
  TString outfileName(fout);
outputFile = TFile::Open( outfileName, "RECREATE" );
 string fprefix=theconf->Value("MVA","weightfile_prefix");
string weightfile = string(fmvalocation+fprefix);
 if(Debug) cout<<weightfile<<endl;
size_t found = weightfile.find("weights");
weightfile = weightfile.substr(found+7);
 if(Debug) cout<<"intifactory:weigtfile   "<<weightfile<<endl;
factory = new TMVA::Factory( TString(weightfile), outputFile, "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" );
// add variables fro the list
 if(!vvar.size()) return 0;
 // assume all float
 // use range defined in config
 for(vector<AParam*>::iterator ip = vvar.begin(); ip != vvar.end(); ++ip) {
   factory->AddVariable(((*ip)->name).c_str(),((*ip)->title).c_str(),"", 'F',(*ip)->valmin,(*ip)->valmax);
 }
 for(vector<AParam*>::iterator ip = vspecvar.begin(); ip != vspecvar.end(); ++ip) {
   factory->AddSpectator(((*ip)->name).c_str(),((*ip)->title).c_str(),"",'F');
 }

 if(Debug) cout<<" MVAmanager::factory done  "<<endl;
  return 1 ;
}

/**  Init variables for reader from stored pointers of TTree Branches
*/
int MVAmanager::initreadvariables() {
  if(Debug) cout<<" MVAmanager::initreader "<<endl;
  if(!vvar.size()) fillvariables(); 
	if(!vvar.size()) return 0;
reader = new TMVA::Reader( "!Color:!Silent" );
// define where the variables are stored when tree is looped
int counter= 0;
mvapar.reserve(vvar.size());
for(vector<AParam*>::iterator ip = vvar.begin(); ip != vvar.end(); ++ip) {
   reader->AddVariable(((*ip)->name).c_str(),&((*ip)->val)); // pointer to tree are  stored in the AParam vector
//  reader->AddVariable(((*ip)->name).c_str(),&mvapar[counter]);
	counter++;
 }
counter = 0;
for(vector<AParam*>::iterator ip = vspecvar.begin(); ip != vspecvar.end(); ++ip) {
 reader->AddSpectator(((*ip)->name).c_str(),&((*ip)->val));
	counter++;
 }
    //
    bookreader();
  if(Debug) cout<<" MVAmanager::initreader done  "<<endl;
  return 1;
}



/** Init reader
 */
int MVAmanager::initreader() {
  if(Debug) cout<<" MVAmanager::initreader "<<endl;
  if(!vvar.size()) fillvariables(); 
	if(!vvar.size()) return 0;
reader = new TMVA::Reader( "!Color:!Silent" );
// define where the variables are stored when tree is looped
int counter= 0;
mvapar.reserve(vvar.size());
for(vector<AParam*>::iterator ip = vvar.begin(); ip != vvar.end(); ++ip) {
 // pointer to tree are  stored in the AParam vector
  reader->AddVariable(((*ip)->name).c_str(),&mvapar[counter]);
	counter++;
 }
counter = 0;
for(vector<AParam*>::iterator ip = vspecvar.begin(); ip != vspecvar.end(); ++ip) {
  reader->AddSpectator(((*ip)->name).c_str(),&mvaspec[counter]);
	counter++;
 }
    //
    bookreader();
  if(Debug) cout<<" MVAmanager::initreader done  "<<endl;
  return 1;
}


/** book methods according to setmethods, require TMVA  files stored in weights/.. or else defined in config
 */
void MVAmanager::bookreader() {
  if(Debug)cout<<"MVAmanager::bookreader()"<<endl;
       string location =theconf->Value("MVA","weightfile_location");
	string prefix =theconf->Value("MVA","weightfile_prefix");
	for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
      if (it->second) {
         if(Debug)cout<<"method: "<< it->first<<endl;
         TString methodName = it->first + " method";
         TString weightfile = TString(location) + TString(prefix) + "_" +  TString(it->first) + ".weights.xml";
         reader->BookMVA( methodName, weightfile ); 
      }
   }
  if(Debug) cout<<" MVAmanager::bookreader done  "<<endl;
  return;
}
/** train MVA 
 */
int MVAmanager::train()
{
  if(Debug) cout<<" MVAmanager::train() "<<endl;
  string tnames =theconf->Value("MVA","tnames");
  string tnameb =theconf->Value("MVA","tnameb");
  string fnames =theconf->Value("MVA","fnames");
  string fnameb =theconf->Value("MVA","fnameb");
  
	vector<TTree*> signal;
	vector<TTree*> background;
	vector<double> signal_weights;
	vector<double> background_weights;
	double d_weight=1; // no weighting for different samples
	if(Debug) cout<<fnames<<endl;
	size_t found=fnames.find(".lst");
	////////////signal
	if (found==string::npos)	
	{
	  TFile *inputs = TFile::Open( fnames.c_str() );
	  signal.push_back((TTree*)inputs->Get(tnames.c_str()));
		signal_weights.push_back(1.);
	}
	else
	{
		char line[1000];
  	fstream file(fnames.c_str());
		vector<TFile*> allsfiles;
  	if (Debug)cout<<"Open file: "<<fnames<<endl;
		if(file.is_open())
		{
			while(!file.eof())
			{
				file.getline(line,1000);
				string bla = line;
				size_t rootfile=bla.find(".root");
				if (rootfile!=string::npos)	
				{
					string filename = bla.substr(0,rootfile+5);
					allsfiles.push_back(TFile::Open(filename.c_str()));	
					signal_weights.push_back(d_weight);	
				}
			}
			file.close();
		}
	// fill tree list for signal
		for (unsigned int nsig=0; nsig< allsfiles.size(); nsig++)
		{
	  	signal.push_back((TTree*)allsfiles.at(nsig)->Get(tnames.c_str()));
 		}
	}
	/////////bkg	
	found=fnameb.find(".lst");
	if (found==string::npos)
	{
  	TFile *inputb = TFile::Open( fnameb.c_str() );  
	 	background.push_back((TTree*)inputb->Get(tnameb.c_str()));
		background_weights.push_back(1.);
	}
	else
	{
		char line[1000];
  	fstream file(fnameb.c_str());
		vector<TFile*> allbfiles;
  	if (Debug)cout<<"Open file: "<<fnameb<<endl;
  	if(file.is_open())
		{
			while(!file.eof())
			{
				file.getline(line,1000);
				string bla = line;
				size_t rootfile=bla.find(".root");
				if (rootfile!=string::npos)	
				{
					string filename = bla.substr(0,rootfile+5);

					if (Debug) cout<<filename<<" "<<d_weight<<endl;
					allbfiles.push_back(TFile::Open(filename.c_str()));	
					background_weights.push_back(d_weight);	
				}
			}
			file.close();
		}


	// fill tree list for bkg
		for (unsigned int nbg=0; nbg< allbfiles.size(); nbg++)
		{
	  	background.push_back((TTree*)allbfiles.at(nbg)->Get(tnames.c_str()));
 		}
	}  
  return train(signal , background, signal_weights, background_weights);
}

/** train MVA first as signal, second as background
 */
int MVAmanager::train(vector<TTree*> ts, vector<TTree*> tb, vector<double> ws, vector<double> wb) 
{
  if(Debug) cout<<" MVAmanager::train()"<<endl;

	Double_t signalWeight(1.0), backgroundWeight(1.0);

	for (unsigned int nsig = 0 ; nsig < ts.size() ; nsig++)
	{
		if (nsig<ws.size()) signalWeight=ws.at(nsig);
		else signalWeight=1.;   
		factory->AddSignalTree    ( ts.at(nsig),     signalWeight );
	}
  
	for (unsigned int nbg = 0 ; nbg < tb.size() ; nbg++)
	{
		if (nbg<wb.size()) backgroundWeight=wb.at(nbg);
		else backgroundWeight=1.;   
		factory->AddBackgroundTree( tb.at(nbg), backgroundWeight );
	}
	
   // set cuts
   setcuts();
   if(Debug) cout<<" cut are set "<<endl;

   // define number of samples
   string sns=theconf->Value("MVA","nTrain_Signal");
   string snb=theconf->Value("MVA","nTrain_Background");

   if(Debug) cout<<" train n "<<sns<<" "<<snb<<endl;
	 TString NTrain_Signal=string(sns);
	 TString NTrain_Background=string(snb);
   factory->PrepareTrainingAndTestTree(*mycuts, *mycutb,
                                        "nTrain_Signal="+NTrain_Signal+":nTrain_Background="+NTrain_Background+":SplitMode=Random:NormMode=NumEvents:!V" );

if(Debug) cout<<" prepared cuts "<<endl;

   bookfactory();
   
   if(Debug) cout<<"start TrainAllMethods()"<<endl; 
   factory->TrainAllMethods();
   if(Debug) cout<<"start TestAllMethods()"<<endl;
   factory->TestAllMethods();
   if(Debug) cout<<"start EvaluateAllMethods()"<<endl;
   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();
   // save outfile
  outputFile->Close();
  if(Debug) {
  cout << "==> Write to file: " << outputFile->GetName() << endl;
  cout << "==> TMVAClassification is done!" << endl;
  }  
delete factory;
  return 1;
}

/** set branches for reader tree according to vvar
    take input tree, extract BranchAddress by name stored in config and  store pointer to parameter
   int AParam vector which is passed further to TMVA trainer
   For reader the parameters are passed directly in the event loop
 */
int MVAmanager::setreadertree(TTree* atree){
  if(!atree) return 0;
  // define where the variables are stored when tree is looped
	for(vector<AParam*>::iterator ip = vvar.begin(); ip != vvar.end(); ++ip) {
  	atree->SetBranchAddress(((*ip)->name).c_str(),&((*ip)->val)); // pointer to tree are  stored in the AParam vector
  }
  return 1;
}

//
int MVAmanager::setVariables(vector<float> mvainput){
  if (mvapar.size()>0) mvapar.clear();
  
	for(unsigned int i = 0; i < mvainput.size(); i++)
  {  
    mvapar.push_back(mvainput.at(i));
  }
	// adding spectator: have to be implemented!!!  
	if (mvaspec.size()>0)mvaspec.clear();
//  for (unsigned int i = 0; i < vspecvar.size(); i++)
//  {
//		cout<<"spectator: "<<i<<endl;    
//		mvaspec.push_back(0.01);	
//	}
  if(Debug)cout<<"MVAmanager::setVariables() done"<<endl;
  return 1;
}

/** get classifiervalue for current state of pointer, i.e. event loop is done externaly
 */
float  MVAmanager::getclassifier(string mname) {
  if(Debug) cout<<"getclassifier(string mname)"<<endl;
  TString classifier = TString(mname) + " method";
  if(Debug) cout<<"value: "<<reader->EvaluateMVA(classifier)<<endl; 
	return reader->EvaluateMVA(classifier);
}

float MVAmanager::getclassifier() {
  if (Debug) cout<<"getclassifier() "<<sclass<<endl;
  return getclassifier(sclass);
}

