#ifndef __CLASSIFIER__
#define __CLASSIFIER__

#include <vector>
#include <fstream>
#include <assert.h>
#include "TObject.h"


/* The usage of this tool is summarized in the following functions:
   -  Forest(int d=1); // Creates a forest with data of dimension d to approximate a 1-function
   -  void Forest<T>::push(float *x,float value,float weight=1); // Push a sample at x with value "value" and weight "weight"   
   -  void Forest<T>::grow(int trees,float selection,int tries,int minSize,float minRMS);
      * Grow a forest of "trees" trees. Each tree uses a random sample of the pushes samples selected with probability "selection".
        Each node of each tree is tried for spliting "tries" times. The spliting is succesful if:
	* It improves the RMS, or if the RMS does not improves but the data is divided in two samples of similar size. 
        * The splitting produces two samples of size larger than minSize
        * The splitting produces two samples of RMS smaller then minRMS (this is tested only if the prevous fail) 
*/      

using namespace std;

// Creates a simpler definition
typedef unsigned long int lint;  

// Each node is represented by a single entry in the vectors array
class Tree: public TObject{
 public:
  static float robust_fraction;  //!
  static int robust_trials;      //!
  static bool robust_split;      //!
  static bool verbose;           //!
  unsigned int dimension;
  vector<float> points[2];  // The two points per node
  vector<int>    nodes[2];   // If the node j is terminal, node[0][j]=-1, node[1][j]=entries in bin, point[0][j*dimension]=mean value, point[1][j*dimension]=rms 
  vector< vector<unsigned short> > attached; // A place where to attach other things
  
  Tree(int d=1);
  ~Tree(){};

  Tree *clean(); // Returner a cleaned tree 


  //Evaluating
  float eval(float *point,int index=0);
  unsigned int node(float *point,int index=0); // Returns the terminal node 

  // Growing
  vector<float> samples;  //! Vector storing all the samples with the format x0,x1,x2...xn,y,weight
  void push(float *x,float value,float weight=1); // Push a sample at x with value value and weight weight

  void grow(int tries=6,int minSize=1,float maxRMS=0); // grow a tree trying "tries" times for optimization, and with minSize minimum elements per node                 
  void grow(lint *buffer,lint *scratch,int tries=6,int minSize=1,float maxRMS=0); // grow a tree trying "tries" times for optimization, and with minSize minimum elements per node. The memory management is left to the user                 

// grow a tree trying tries time for optimization, and with minSize minimum elements per node. size is the number of samples to be used
// stored in buffer (as indexes to samples) and scratch is a temporary scratch space 
  void  grow_internal(lint *buffer,lint size,lint *scratch,
		      int tries,int minSize,float maxRMS); 

  // Get momentum is virtual to allow changing the fit function
  // The function should return the best value for  the node as "mean"
  // the fit function value in "rms" and the number of elements in the
  // node in "weight".
  void get_momentum(lint *buffer,int size,float &mean,float &rms,float &weight,int iterations); // A helper funtion to compute means and rms

  ClassDef(Tree,1)
};

// The forest is a template to allow changing the approximation function
class Forest: public TObject{
 public:
  unsigned int dimension;
  vector<Tree> forest; 
  vector<float> samples; //!

  lint *_buffer;  //!  For internal usage
  lint *_scratch; //!  For internal usage

  Forest(int d=1){dimension=d; forest.reserve(1024);}
  
  void push(float *x,float value,float weight=1); // Push a sample at x with value value and weight weight  
  void grow_tree(float selection=0.7,int tries=6,int minSize=1,float maxRMS=0);
  void grow(int trees,float selection=0.7,int tries=6,int minSize=1,float maxRMS=0);
  float eval(float *point);
  ClassDef(Forest,1)
};

#endif

