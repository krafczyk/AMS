#ifndef __Unfold__
#define __Unfold__
#include "TH1F.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TH2D.h"
#include "math.h"

//! Bayesian unfolding tool
/*!
\author carlos.delgado@ciemat.es
 */


class BayesianUnfolder{
 public:
  BayesianUnfolder(){Verbose=true;}; //< Constructor

  void copyH(TH2F &input,TH2D &output);  //< Helper function
  void copyH(TH1F &input,TH1D &output);  //< Helper function 
  void copyH(TH2D &input,TH2F &output);  //< Helper function
  void copyH(TH1D &input,TH1F &output);  //< Helper function 


  /**
     Sets the migration matrix to be used during unfolding
     @param migrationMatrix: the normlized migration matrix. X axis should be the true variable and Y axis for the measured one
   */
  void setMigrationMatrix(TH2D &migrationMatrix){MigrationMatrix=migrationMatrix;}                 
  void setMigrationMatrix(TH2F &migrationMatrix){copyH(migrationMatrix,MigrationMatrix);}                 

  /**
     Sets the migration matrix from a histogram containing entries from MC or alike, without normalization.
     This is the recommended method, since  it takes into account the under and overflow in the right way.
     @param joint: The histogram with MC (or data) entries without normalization. 
   */
  void setMigrationMatrixFromJoint(TH2D &joint);
  void setMigrationMatrixFromJoint(TH2F &joint){TH2D j;copyH(joint,j);setMigrationMatrixFromJoint(j);}


  /**
     Sets a prior for the unfolding procedure. The binning of the prior is the one used for the unfolded distribution.
     @param prior: The prior distribution.
   */
  void setPrior(TH1D &prior){Prior=prior;Posterior=Prior;}
  void setPrior(TH1F &prior){TH1D p;copyH(prior,p);setPrior(p);}

  /**
     For internal use. Checks is the migration matrix and the prior are initialized
     \return true if both histograms have been initialized 
   */
  bool isOK();
  
  /**
     Compute the unfolding matrix with current prior. The result is returned in
     @see UnfoldingMatrix
   */
  void computeUnfoldingMatrix();

  /** 
      Performs an itertion of the Bayesian unfolding algorithm.
      Updates: 
      @see Posterior
      \param measured: The measured distribution
  */
  void BayesianUnfoldingStep(TH1D &measured);

  
  /**
     Estimate the unormalized Kullback-Leibler divergence between its arguments. It measures the similarity 
     between the distributions, taking into account the numero of entries.
     \return The KL divergence between the tow distribution.
   */
  double estimateKL(TH1D &prior,TH1D &posterior);               // Estimate the Kullback-Leibler divergence between prior and posterior

  /**
     Compute the Kullback-Leibler divergence between the current prior and unfolded distribution.
   */
  double estimateKL(){return estimateKL(Prior,Posterior);}               // Estimate the Kullback-Leibler divergence between prior and posterior

  /**
     Compares the measure distribution and the folding of the current unfolded distribution and the migration matrix
     \param measured: Measured distribution
     \return Chi2/ndof assumming Poisson stadistic, and the mean given by the folding of the unfolded distribution stored in @see Posterior
   */
  double getChi2(TH1D &measured);    // Get the chi2 per dof, assuming that the unfolding posteiors are a uniparametric family of functions   

  /**
     Run the unfolding procedure until convergence. 
     @see MigrationMatrix has to be initialized.
     @param measured: Measured distribution
     @param unfolded: histogram where to store the unfolded result
     @param regularization: the procedure is regularized by calling TH1D::Smooth on the Prior before executing an unfolding step
     @param maxKLchange: change in the value of the KL divergence between the Prior and Posterior in a iteration to consider that the algorithm converged
     @param maxChi2change: change in the chi2/ndof value between iterations to consider the algorithm converged  
     \return The unfolded distribution in unfolded WITHOUT ERRORS
  */
  void run(TH1D &measured,TH1D &unfolded,
	   int regularization=100,
	   double maxKLchange=1e-2,
	   double maxChi2change=1e-2);

  void run(TH1F &measured,TH1F &unfolded,
	   int regularization=100,
	   double maxKLchange=1e-2,
	   double maxChi2change=1e-2){
    TH1D m;copyH(measured,m);
    TH1D u;copyH(unfolded,u);
    run(m,u,regularization,maxKLchange,maxChi2change);
    copyH(u,unfolded);
  }
  


  /**
     Run the unfolding procedure until convergence and use a MC to propagate the statistical errors, assuming Poisson statistics in the input spectrumm and the migration matrix 
     @param jointPDF: unnormalized histogram contaning MC entries to build the migration matrix (X axis for true, Y axis for measured)   
     @param measured: Measured distribution
     @param unfolded: histogram where to store the unfolded result
     @param errorComputationSamples: Number of samples used to estimate the errors
     @param fluctuateMatrix: set to true to propagate statistical fluctuations in the migration matrix
     @param fluctuateInput: set to true to propagate statistical fluctuations in the measured matrix
     @param regularization: the procedure is regularized by calling TH1D::Smooth on the Prior before executing an unfolding step
     @param maxKLchange: change in the value of the KL divergence between the Prior and Posterior in a iteration to consider that the algorithm converged
     @param maxChi2change: change in the chi2/ndof value between iterations to consider the algorithm converged  
     \return The unfolded distribution in unfolded WITH ERRORS
  */
  void computeAll(TH2D &jointPDF,TH1D &measured,             // Inputs
		  TH1D &unfolded,                            // Output
		  int errorComputationSamples=10,           // Samples for MC error computation    
		  bool fluctuateMatrix=true,bool fluctuateInput=true,
		  int regularization=0,double maxKLchange=0.0,double maxChi2change=1e-1);

  void computeAll(TH2F &jointPDF,TH1F &measured,             // Inputs
		  TH1F &unfolded,                            // Output
		  int errorComputationSamples=10,           // Samples for MC error computation    
		  bool fluctuateMatrix=true,bool fluctuateInput=true,
		  int regularization=0,double maxKLchange=0.0,double maxChi2change=1e-1){
    TH2D j;copyH(jointPDF,j);
    TH1D m;copyH(measured,m);
    TH1D u;copyH(unfolded,u);
    computeAll(j,m,u,errorComputationSamples,fluctuateMatrix,fluctuateInput,regularization,maxKLchange,maxChi2change);
    copyH(u,unfolded);
  }


  // Members
  TH2D MigrationMatrix;   /// Migration matrix. It is an input, but it could require some preprocessing
  TH1D Prior;             /// Prior, update after each iteration by @see run and @see computeAll 
  TH2D UnfoldingMatrix;   /// Latest unfolding matrix, computed assuming the @see Prior 
  TH1D Measured;          /// Measured posterior:result of folding the Posterior with the migration matrix  
  TH1D Posterior;         /// Result of the last iteration

  // Status
  int Failed;             /// Status of last calls

  // Verbosity
  bool Verbose;           /// true by default, produces some informative output 
  
  static int MaxIters;    /// Maximum number of iterations allowed
};





#include "TH1D.h"
#include "TRandom.h"
#include <vector>

//! A MC sampler to compute errors after minimization using DEMinimizer 
/*!
\author carlos.delgado@ciemat.es
 */

class MCSampler{
 public:
  MCSampler(){};
  /**
     Sets the cost function to sample from 
     @param function: function pointer to the one to be minimized. The arguments are passed as the "visible" bin contents of a TH1D histogram
  */
  void setFunction(double (*function)(TH1D &)){Function=function;}

  /**
     Sets the position of the minimum of the cost function .
     @param parameters: histogram containing the parameters
  */
  void setMinimum(TH1D &parameters){Parameters=parameters;}

  void setRandomSeed(unsigned int seed){Random.SetSeed(seed);} ///< Sets the random seed for the minimizer

  void initSampler();                                          ///< Search for a range where to scan. Uses a rough approximation to start
  double scanParameter(int par,const double scale=1e-2);       ///< Search for a range where to scan. Uses a rough approximation to start

  /**
     Pick a ramdon sample around the minimum.
     \return sample: The random sample
     \return weight: The weight for this sample
  */
  void pickSample(TH1D &sample,double &weight);
  
  TRandom Random;             ///< A random number generator
  double (*Function)(TH1D&);  ///< Pointer to the function cost being sampled
  TH1D Parameters;            ///< Poisition of the minimum 
  TH1D Sigmas;                ///< Area being sampled
};

//! A general minimizer based on the differential evolution heuristic 
/*!
\author carlos.delgado@ciemat.es
 */


class DEMinimizer{
 public:
  DEMinimizer(){Function=NULL;Verbosity=1;}  //<Constructor
  
  /**
     Sets the function to be minimized.
     @param function: function pointer to the one to be minimized. The arguments are passed as the "visible" bin contents of a TH1D histogram
  */
  void setFunction(double (*function)(TH1D &)){Function=function;}

  void setRandomSeed(unsigned int seed){Random.SetSeed(seed);} /// Sets the random seed for the minimizer

  /**
     Initialize the minimizer to search around the values
     in the range histogram. The errors and mean in the histogram
     are used to sample initial points, so errors should no be zero. 
     @param range: parameters around where to start looking for the minimum. The errors give a clue of the search range. 
     @multiplicationFactor: the number of test points is the number of parameters (bins in range) multiplied by this factor.
  */
  void initMinimizer(TH1D &range,double multiplicationFactor=10);
  /**
     Initialize the minimizer to search around the values
     in the range histogram. The initial search region is given 
     by the values in lower an upper histograms.
  */
  void initMinimizer(TH1D &lower,TH1D &upper,double multiplicationFactor=10);

  /**
     Run the minimizer and return the solution in output. 
     @param ouput: Histogram to store the position of the minimum found
     @param maxChangeAllowed: max change in the minimum value between tests to consider the minimizer has converged
     @param batchSize: Number of function calls between evaluation of the convergence criteria
     @param maxEvaluations: maximum allowed function calls
     @return true if the minimization converged

   */
  bool searchMinimum(TH1D &output,double maxChangeAllowed=0,int batchSize=2000,int maxEvaluations=100000);

  /**
     Compute the errors for the minimized result assuming that
     the function provided is the -logarithm of a probability (including
     priors). The error is computed using the MC methof by sampling
     from the probability function. A function init can be provided that
     is called before a sample is taken from the distribution.
   */
  void computeMCErrors(TH1D &output,int samples=2000,void (*init)(void)=NULL);


  TRandom Random;
  std::vector<TH1D> Population;           /// Vector with the current iteration population of agents
  std::vector<double> PopulationValues;   /// Vector with the current iteration population of agents
  double (*Function)(TH1D&);         /// Pointer to the function being minimized
  TH1D Parameters;                   /// Result of the last call to the minimizer
  int Verbosity;                     /// Verbosity level. 0 Means no messages. 
};


//! A unfolding strategy without free parameters for distributions following
//! Poisson statistics. It is based in the minimization of the stochastic complexity
//! instead of relying in an explicit metric with a regularization term. The
//! time it takes is linear in the number of entries in the measured distribution, so
//! can only be used for histograms with no too many entries.
//! NOTE: Innefficiencies are not yet taken into account
/*!
\author carlos.delgado@ciemat.es
 */

#include <map>

class StochasticUnfolding{
 public:
  StochasticUnfolding(){Counter=0;}

  void copyH(TH2F &input,TH2D &output);  //< Helper function
  void copyH(TH1F &input,TH1D &output);  //< Helper function 
  void copyH(TH2D &input,TH2F &output);  //< Helper function
  void copyH(TH1D &input,TH1F &output);  //< Helper function 

  void setPrior(TH1D &prior,double dirichletHyperparameter=0.5);
  void setResponseMatrixFromJoint(TH2D &joint);
  void addEntry(double measured);
  void addEntries(TH1D &measured,double fraction,bool maxRegularization=true);
  void fold(TH1D &output);


  void setPrior(TH1F &prior,double dirichletHyperparameter=0.5){TH1D p;copyH(prior,p);setPrior(p,dirichletHyperparameter);}
  void setResponseMatrixFromJoint(TH2F &joint){TH2D j;copyH(joint,j);setResponseMatrixFromJoint(j);}


  std::vector<int> start;
  std::vector<int> end;

  /**
     Main steering routine.
     @param joint: unnormalize joint distribution. Used to compute the response matrix. Poissonian statistic for the bins error is assumed. The binning should be equal or finner than that of the measured and unfolded distributions 
     @param measured: measured distribution
     @param output: slot to store the unfolded distribution. The binning on the histogram is kept. 
     @param samples: number of realizations used. The larger the more accurate are the errors, but the running time is linear in this quantity. 
  */
  void unfold(TH2D &joint, TH1D &measured,TH1D &output,int samples=10);
  void unfold(TH2F &joint, TH1F &measured,TH1F &output,int samples=10){TH2D j;TH1D m,o;copyH(joint,j);copyH(measured,m);copyH(output,o);unfold(j,m,o,samples);copyH(o,output);}

  /**
     Fast approximation.
     @param joint: unnormalize joint distribution. Used to compute the response matrix. Poissonian statistic for the bins error is assumed. The binning should be equal or finner than that of the measured and unfolded distributions 
     @param measured: measured distribution
     @param output: slot to store the unfolded distribution. The binning on the histogram is kept. 
     @param maxRegularization: set to true to apply the maximum regularization method
     @param fluctuateMigration: set to true to propagate statistica fluctuations of the response matrix in order
     @param samples: number of realizations used. The larger the more accurate are the errors, but the running time is linear in this quantity. 
     @param fraction: fraction of events on each iteration for which the prior is considered to be constant.
  */
  void unfoldFast(TH2D &joint, TH1D &measured,TH1D &output,bool maxRegularization=false,bool fluctuateMigration=true,int samples=10,double fraction=0.05);
  void unfoldFast(TH2F &joint, TH1F &measured,TH1F &output,bool maxRegularization=false,bool fluctuateMigration=true,int samples=10,double fraction=0.05){TH2D j;TH1D m,o;copyH(joint,j);copyH(measured,m);copyH(output,o);unfoldFast(j,m,o,maxRegularization,fluctuateMigration,samples,fraction);copyH(o,output);}


  TRandom Random;               ///< Random number generator for sampling
  TH1D Prior;                   ///< The current unfolded distribution
  TH2D ResponseMatrix;          ///< Response matrix   
  double Counter;               ///< Integral of the unfolded distribution. 
  TH1D Folded;                  ///< Latest folded distribution
};


//! A base class for forward unfolding. Not really useful, but provided as an example
/*!
\author carlos.delgado@ciemat.es
 */
class FUnfolding{
 public:
  FUnfolding(){};
  virtual ~FUnfolding(){};
  virtual double cost(TH1D &parameters);
  virtual void fold(TH1D &parameters,TH1F &output);
  virtual  void unfold(TH2F &jointPDF,TH1F &measured,TH1D &output);
  void setResponseMatrixFromJoint(TH2F &joint);

  static FUnfolding* Current;
  TH2F Joint;     ///< Joint distribution (kept for sampling)
  TH2F ResponseMatrix;  ///< Response matrix        
  TH1F Measured;  ///< Measured distribution (to be fit)
  TH1D Parameters; ///< Result of the fit (with errors, hopefully)
  double Cost;     ///< Cost of the result
  static double costFunction(TH1D &parameters);  ///< Static handler for the minimizer
};


#endif

