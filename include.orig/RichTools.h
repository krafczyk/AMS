#ifndef _RICHANALYSIS_
#define _RICHANALYSIS_

#include <vector>
#include <fstream>
#include <assert.h>
#include "TObject.h"

using namespace std;

///// A geometrical hash useful for multidimensional studies
#include<stdlib.h>
#include<math.h>

namespace GeomHashConstants{
  const int nBuffers=4;
};

using namespace GeomHashConstants;

// Faster (hopefully much faster) implementation of the tool
class GeomHash: public TObject{
 public:
  static bool storeEntries;
  static bool storeTemplates;
  static bool storeTemplatesRms;
  static bool storeMean;
  static bool storeRms;
  static bool storePeak;
  static bool storePeakWidth;
  static bool computePeakAsMean;
  static float peakFinderFraction; 

  static void setDefaults(){
    GeomHash::storeEntries=true;
    GeomHash::storeTemplates=true;
    GeomHash::storeTemplatesRms=true;
    GeomHash::storeMean=true;
    GeomHash::storeRms=true;
    GeomHash::storePeak=true;
    GeomHash::storePeakWidth=true;
    GeomHash::computePeakAsMean=false;
    GeomHash::peakFinderFraction=0.68;
  }

  int dimension;
  int numNodes;
  int minSize;              // min size allowed to bin
  vector<unsigned char> points;       // Chosen direction
  vector<float> limit;     // The limit to separate among them
  vector<int>   nodes[2];   // If the node j is terminal, node[0][j]=-1, node[1][j]=entries in bin, point[0][j*dimension]=mean value, point[1][j*dimension]=rms                                                   
  vector<float> Means;
  vector<float> Rms;
  vector<int>   Entries;
  vector<float> Template;
  vector<float> TemplateRms;
  vector<float> Peak;
  vector<float> PeakWidth;

  GeomHash(int d=1);

  float *getTemplate(int h);
  float *getTemplateRms(int h);

  inline int offset(int which){return which*dimension;}
  //Evaluating
  int hash(float *point);
  int get(double x,...);

  // Virtual function
  void store(int *pointer,int size,int parent);                                           // Storing information in the tree

  // Growing
  vector<float> samples;  //! Vector storing all the samples with the format x0,x1,x2...xn,y,weight
  vector<float> values;   //! Vector storing the guiding values
  void push(float value,float *x);
  void fill(double x,...);


  void grow(int min_size=0);

  void grow(int *pointers,                          // Buffer pointing to the points indexes
	    int *scratch,
	    int min_size=0);        // Scratch region to store the distances to the points


  // grow a tree trying tries time for optimization, and with minSize minimum elements per node. size is the number of samples to be used
  // stored in buffer (as indexes to samples) and scratch is a temporary scratch space
  void  grow_internal(int *pointers,
                      int  size,
		      int *scratch,
                      int parent=0);



  float getMean(int node);
  float getRms(int node);
  float getPeak(int node);
  float getPeakWidth(int node);

  int getEntries(int node);


  // Some internal buffers
  static float *buffer[GeomHashConstants::nBuffers]; //!
  static int    bufferSize;       //!
  void checkBuffers();

#pragma omp threadprivate(fgIsA)
  ClassDef(GeomHash,1);
};


class GeomHashEnsemble: public GeomHash{
 public:
  GeomHashEnsemble(int d=1):GeomHash(d){}

  vector<GeomHash> hashes;
  void growOne(int minSize=10,bool bootstrap=false);
  void Eval(float *x);
  void Eval(double x,...);

  // Result from last evaluation
  double MeanValue;
  double ValueRms;
  double MeanRms;
  double RmsRms;
  double MeanPeak;
  double PeakRms;
  double MeanPeakWidth;
  double PeakWidthRms;
  double MeanEntries;
  double EntriesRms;
  int    Hashes;

  int numHashes();
  GeomHash &getHash(int i);

  ClassDef(GeomHashEnsemble,1);
#pragma omp threadprivate(fgIsA)
};



#endif





