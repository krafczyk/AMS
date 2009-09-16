#ifndef _DE_MINIMIZER_
#define _DE_MINIMIZER_



class DEMinimizer{
 public:
  int fParameters;
  int fPopulation;
  
  double **fSamples[2];
  double *fValues;

  double *fBest;
  double fBestValue;
  int fAge;

  double fFactor;
  double fCR;

  double ((*fChi2)(double *));
  double* ((*fSample)());

  DEMinimizer(int parameters,                    // Number of parameters
	      int population,                    // Population
	      double (*chi2)(double *),             // Function to minimize
	      double* (*sample)(),                  // Function returning a sample in the initial range
	      double mutation_factor=2.0,           // Mutation factor 
	      double cross_over_probability=0.0);
    
  ~DEMinimizer();

  void Select3(int &x1,int &x2,int &x3);
  void Mutate(int speciment);
  void Step();

};


#endif
