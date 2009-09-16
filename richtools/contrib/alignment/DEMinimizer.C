#include "DEMinimizer.h"
#include <assert.h>  // for assert
#include <math.h>    // for HUGE_VAL
#include <stdlib.h>  // drand48() -- return a double in the [0,1) range

#define _DEBUG_
#ifdef _DEBUG_
#include <iostream>
using namespace std;
#endif


DEMinimizer::DEMinimizer(int parameters,                    // Number of parameters
			 int population,                    // Population
			 double ((*chi2)(double *)),             // Function to minimize
			 double* ((*sample)()),                  // Function returning a sample in the initial range
			 double mutation_factor,           // Mutation factor 
			 double cross_over_probability):
  fParameters(parameters),
  fPopulation(population),
  fFactor(mutation_factor),
  fCR(cross_over_probability),
  fChi2(chi2),
  fSample(sample){

  assert(fPopulation>3);

  // Initialize the population
  for(int which=0;which<2;which++)
    fSamples[which]=new double*[fPopulation];
  for(int i=0;i<fPopulation;i++) fSamples[0][i]=(*fSample)();     
  for(int i=0;i<fPopulation;i++) fSamples[1][i]=new double[fParameters];

  // Compute the initial values
  fValues=new double[fPopulation];
  for(int i=0;i<fPopulation;i++) fValues[i]=(*chi2)(fSamples[0][i]);  

#ifdef _DEBUG_
  cout<<"OUTPUT OF INITIAL POPULATION"<<endl;
  for(int i=0;i<population;i++){
    for(int j=0;j<parameters;j++) cout<<fSamples[0][i][j]<<" ";
    cout<<" ->"<<fValues[i]<<endl;
  }
  cout<<"-------------------------------"<<endl;
#endif  

  // Get the current best candidate
  fBestValue=HUGE_VAL;
  for(int i=0;i<fPopulation;i++) if(fValues[i]<fBestValue){fBestValue=fValues[i];fBest=fSamples[0][i];}

  // Age of the best candidate
  fAge=0;
}


DEMinimizer::~DEMinimizer(){
  delete fValues;

  for(int i=0;i<fPopulation;i++) {delete fSamples[0][i];delete fSamples[1][i];}
  delete fSamples[0];
  delete fSamples[1];
}



void DEMinimizer::Select3(int &x0,int &x1,int &x2){
  x0=int(drand48()*fPopulation);
  do x1=int(drand48()*fPopulation); while(x0==x1);
  do x2=int(drand48()*fPopulation); while(x0==x2 || x1==x2);
}

void DEMinimizer::Mutate(int speciment){
  int x0,x1,x2;
  Select3(x0,x1,x2);
  int minimum_change=int(drand48()*fParameters);

  for(int i=0;i<fParameters;i++)
    fSamples[1][speciment][i]=(i==minimum_change || drand48()<fCR)?fSamples[0][x0][i]+fFactor*(fSamples[0][x1][i]-fSamples[0][x2][i]):
                                                                   fSamples[0][speciment][i];
}


void DEMinimizer::Step(){
#ifdef _DEBUG_
  cout<<"CURRENT BEST POINTER "<<fBest<<endl;

  cout<<"CURRENT BEST ";
  for(int i=0;i<fParameters;i++) cout<<fBest[i]<<" ";
  cout<<" VALUE "<<fBestValue<<endl;
#endif

  // Update age of best candidate 
  fAge++;

  // Mutate all of them
#ifdef _DEBUG_
  cout<<"MUTATING"<<endl;
#endif
  for(int i=0;i<fPopulation;i++) Mutate(i);
#ifdef _DEBUG_
  cout<<"FINISHED"<<endl;
#endif


#ifdef _DEBUG_
  cout<<"REPLACING"<<endl;
#endif
  // Replace if new values are obtained
  for(int i=0;i<fPopulation;i++){
    double val=(*fChi2)(fSamples[1][i]);
    if(val<fValues[i]){
      fValues[i]=val;
      //SWAP the new with the old speciment (this is faster than copying)
      double *temp=fSamples[0][i];
      fSamples[0][i]=fSamples[1][i];
      fSamples[1][i]=temp;
    }
  } 
#ifdef _DEBUG_
  cout<<"FINISHED"<<endl;
#endif

#ifdef _DEBUG_
  cout<<"GETTING"<<endl;
#endif
  // Get the best values
  for(int i=0;i<fPopulation;i++){
    if(fValues[i]>=fBestValue) continue; 
    fAge=0;
    fBestValue=fValues[i];
    fBest=fSamples[0][i];
  }
#ifdef _DEBUG_
  cout<<"FINISHED"<<endl;

  cout<<"CURRENT BEST POINTER "<<fBest<<endl;

  cout<<"CURRENT BEST ";
  for(int i=0;i<fParameters;i++) cout<<fBest[i]<<" ";
  cout<<" VALUE "<<fBestValue<<endl;

#endif


  

}


//#define _TEST_

#ifdef _TEST_
#include <math.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

double func(double *pars){
  double &x=pars[0];
  double &y=pars[1];
  return x*x+2*y*y-0.3*cos(2*M_PI*x)*cos(4*M_PI*y)+0.3;
    //  return (x[0]-0.5)*(x[0]-0.5)+(x[1]-20)*(x[1]-20);
  //  return -(sqrt(fabs(cos(sqrt(fabs(x[0]*x[0]+x[1]*x[1]))))) +0.01*x[0]+.01*x[1]);
}


double* init(){
  double *a=new double[2];
  a[0]=drand48()*20-10;
  a[1]=drand48()*20-10;
  return a;
} 

int main(){
  DEMinimizer test(2,  // Parameters size
		   30, // Population size
		   &func,
		   &init
		   );


  do{
    test.Step();
    cout<<"Current best "<<test.fBest[0]<<" "<<test.fBest[1]<<" value="<<test.fBestValue<<" AGE "<<test.fAge<<endl;
  }while(test.fAge<30);



  return 0;
}

#endif
