#include "RichTools.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#undef _DEBUG_
#define ASSERT(x) 

using namespace std;

ClassImp(GeomHash);

using namespace GeomHashConstants;

float *GeomHash::buffer[nBuffers]={0,0,0,0};
int GeomHash::bufferSize=0;

bool GeomHash::storeEntries=true;
bool GeomHash::storeTemplates=true;
bool GeomHash::storeTemplatesRms=true;
bool GeomHash::storeMean=true;
bool GeomHash::storeRms=true;
bool GeomHash::storePeak=true;
bool GeomHash::storePeakWidth=true;
bool GeomHash::computePeakAsMean=false;
float GeomHash::peakFinderFraction=0.68;

void GeomHash::checkBuffers(){
  if(dimension<=bufferSize) return;
  bufferSize=dimension;
  for(int i=0;i<nBuffers;i++){
    if(buffer[i]) delete[] buffer[i];
    buffer[i]=new float[dimension+1];
  }
}


GeomHash::GeomHash(int d){
  if(d>255){cout<<"Initializing to 255 variables."<<endl;d=255;}
  if(d<0) d=0;
  dimension=d;
  numNodes=0;
  points.reserve(1024);
  nodes[0].reserve(1024);
  nodes[1].reserve(1024);
  limit.reserve(1024);

  // Grow the buffers if needed
  //  checkBuffers();
}


int GeomHash::hash(float *point){
  checkBuffers();
  if(numNodes==0) return 0;
  int current=0;

  for(;;){
    int nodeNumber=point[points[current]]<limit[current]?0:1;
    if(nodes[nodeNumber][current]<0) return -nodes[nodeNumber][current]-1;
    current=nodes[nodeNumber][current];
  }
}

void GeomHash::push(float value,float *x){
  // Set the minimum capacity
  if(samples.capacity()<1024*dimension) samples.reserve(1024*dimension);

  // Store the point
  for(int i=0;i<dimension;i++)  samples.push_back(x[i]);

  values.push_back(value);
}

#include <stdarg.h>
void GeomHash::fill(double x,...){
  checkBuffers();
  va_list ap;
  va_start(ap, x);
  float value=x;
  for(int i=0;i<dimension;i++) buffer[2][i]=va_arg(ap, double);
  va_end(ap);
  push(value,buffer[2]);
}

int GeomHash::get(double x,...){
  checkBuffers();
  va_list ap;
  va_start(ap, x);
  buffer[2][0]=x;
  for(int i=1;i<dimension;i++) buffer[2][i]=va_arg(ap, double);
  va_end(ap);
  return hash(buffer[2]);
}

int *_Wall_;  // Just in case

void GeomHash::grow(int min_size){
  points.clear();
  limit.clear();
  nodes[0].clear(); nodes[1].clear();
  int *pointers=new int[samples.size()/dimension];
  int *scratch=new  int[samples.size()/dimension];

  int size=samples.size()/dimension;
  _Wall_=pointers+size;
  grow(pointers,scratch,min_size);

  delete[] pointers;
  delete[] scratch;
}


void GeomHash::grow(int *pointers,int *scratch,int min_size){
  int entries=samples.size()/dimension;

  // Init the buffer
  for(int i=0;i<entries;i++) pointers[i]=i;

  // Start the real growing process
  minSize=2*min_size;
  grow_internal(pointers,entries,scratch); // Adjust the min_size to obtain a better approximation

  // Liberates the space used by the samples and values
  samples.resize(0);
  values.resize(0);
  vector<float> empty; 
  samples=empty;
  values=empty;
}

void GeomHash::store(int *pointers,int size,int parent){
  checkBuffers();
  int currentHashNumber=numNodes++;					
  int trueParent=fabs(parent)-1;					
  int parentNode=parent<0?0:1;						
  nodes[parentNode].at(trueParent)=-currentHashNumber-1;
  
  // Here we can store the information concerning the data, for example mean and rms
  if(storeEntries) Entries.push_back(size);

  double sum=0,sum2=0;
  for(int i=0;i<size;i++){
    sum+=values[pointers[i]];
    sum2+=values[pointers[i]]*values[pointers[i]];
  }
  sum/=size;
  sum2/=size;
  sum2-=sum*sum;
  sum2=sqrt(fabs(sum2));

  if(storeMean) Means.push_back(sum);
  if(storeRms) Rms.push_back(sum2);

  // Fill the templates
  for(int i=0;i<dimension;i++) buffer[0][i]=buffer[1][i]=0;
  for(int j=0;j<dimension;j++){
    double sum=0;
    double sum2=0;
    for(int i=0;i<size;i++){
      int &element=pointers[i];
      float &v=samples[offset(element)+j];
      sum+=v;
      sum2+=v*v;
    }
    sum/=size;
    sum2/=size;
    sum2-=sum*sum;
    sum2=sqrt(fabs(sum2));
    if(storeTemplates) Template.push_back(sum);
    if(storeTemplatesRms) TemplateRms.push_back(sum2);
  }

  // Peak search and width determination
  vector<float> ordered;
  ordered.reserve(size);
  for(int i=0;i<size;i++) ordered.push_back(values[pointers[i]]);
  sort(ordered.begin(),ordered.end());
  int window=int(ceil(size*peakFinderFraction))-1; 
  if(window==0) window=1;

#ifdef _DEBUG_
  cout<<"PEAK FINDER FOR"<<endl;
  for(int i=0;i<size;i++) cout<<ordered[i]<<" ";
  cout<<endl;
  cout<<"Window size "<<window<<endl;
  cout<<"Sample size "<<size<<endl;
  cout<<"Mean "<<Means[Means.size()-1];
#endif


  float bestWidth=INFINITY;
  float peakPos=0;
  for(int i=0;i<size;i++){
    if(i+window>=size) break;
    if(i && ordered[i]==ordered[i-1]) continue; // skip identical events
    int finalIndex=i+window;
    for(int j=finalIndex+1;j<size;j++){
      if(ordered[j]!=ordered[finalIndex]) break;  
      finalIndex=j;
    }

    float width=ordered[finalIndex]-ordered[i];
    if(width<bestWidth){
      bestWidth=width;
      peakPos=0.5*(ordered[finalIndex]+ordered[i]);
#ifdef _DEBUG_
      cout<<"CURRENT BEST "<<peakPos<<" from "<<i<<" to "<<finalIndex<<" "<<ordered[i]<<" "<<ordered[finalIndex]<<endl;
#endif
    }
  }

  bestWidth/=2; // width meaning should be equivalent to sigma 

  if(computePeakAsMean){
    double sum=0;
    double sum2=0;
    int total=0; 
    for(int i=0;i<size;i++){
      if(ordered[i]>peakPos+bestWidth) continue;
      if(ordered[i]<peakPos-bestWidth) continue;
      sum+=ordered[i];
      sum2+=ordered[i]*ordered[i];
      total++;
    }
    sum/=total;
    sum2/=total;
    sum2-=sum*sum;
    sum2=sqrt(fabs(sum2));
    peakPos=sum;
    bestWidth=sum2;
  }

  if(storePeak) Peak.push_back(peakPos);
  if(storePeakWidth) PeakWidth.push_back(bestWidth);
}

float *GeomHash::getTemplate(int h){
  if(!Template.size()) return 0;
  if(h<0 || h>=numNodes) return 0;
  checkBuffers();
  for(int i=0;i<dimension;i++) buffer[0][i]=Template[offset(h)+i];
  return buffer[0];
}

float *GeomHash::getTemplateRms(int h){
  if(!TemplateRms.size()) return 0;
  if(h<0 || h>=numNodes) return 0;
  checkBuffers();
  for(int i=0;i<dimension;i++) buffer[1][i]=TemplateRms[offset(h)+i];
  return buffer[1];
}


float GeomHash::getMean(int node){
  if(!Means.size()) return 0;
  if(node>=numNodes) return 0;
  return Means.at(node);
}

float GeomHash::getRms(int node){
  if(!Rms.size()) return 0;
  if(node>=numNodes) return 0;
  return Rms.at(node);
}


float GeomHash::getPeak(int node){
  if(!Peak.size()) return 0;
  if(node>=numNodes) return 0;
  return Peak.at(node);
}

float GeomHash::getPeakWidth(int node){
  if(!PeakWidth.size()) return 0;
  if(node>=numNodes) return 0;
  return PeakWidth.at(node);
}

int GeomHash::getEntries(int node){
  if(!Entries.size()) return 0;
  if(node>=numNodes) return 0;
  return Entries.at(node);
}


void GeomHash::grow_internal(int *pointers,int size,int *scratch,int parent){
  const float epsilon=5e-6;
  // Check if we should proceed
  if(size==1 || size<minSize) {store(pointers,size,parent);return;}

  // Fast search binning
  //  const int numBins=1000;  // Number of bins to be used during spliting
  const int numBins=10;  // Number of bins to be used during spliting
  int entries[numBins];
  double mean[numBins];
  double mean2[numBins]; 
  for(int i=0;i<numBins;i++) entries[i]=0;
  for(int i=0;i<numBins;i++) mean[i]=0;
  for(int i=0;i<numBins;i++) mean2[i]=0;

  int me=nodes[0].size();

  // Pick randomly a direction
  int direction=int(dimension*(rand() / (RAND_MAX + 1.0)));

  // Select the sorting range
  double minValue=INFINITY;
  double maxValue=-INFINITY;

  // DEBUG
  int minP=-1;
  int maxP=-1;

  for(int index=0;index<size;index++){
    int element=pointers[index];
    float x=samples[offset(element)+direction];
    if(x<minValue) {minValue=x;minP=index;}
    if(x>maxValue) {maxValue=x;maxP=index;}
  }


  // No binning
  if(fabs((maxValue-minValue)/((maxValue+minValue)/2))<epsilon) {store(pointers,size,parent);return;}

  double bw=(maxValue-minValue)/numBins;
  maxValue+=bw/2;
  minValue-=bw/2;
  bw=(maxValue-minValue)/numBins;

  // No binning
  if(bw<=0) {store(pointers,size,parent);return;}

  // Fill the arrays
  double totalMean=0;
  double totalMean2=0;
  bool equals=true;

  double prevValue=values[0];
  for(int index=0;index<size;index++){
    int element=pointers[index];

    float x=samples[offset(element)+direction];
    float value=values[element];
    float value2=value*value;
    int bin=int(floor((x-minValue)/bw));

    if(value!=prevValue) equals=false;

    entries[bin]++;
    mean[bin]+=value;
    mean2[bin]+=value2;
    totalMean+=value;
    totalMean2+=value2;
  }


  // Search the best split position
  const double rms01=totalMean2/size-totalMean/size*totalMean/size;
  const int n01=size;

  int accEntries=0;
  double acc=0;
  double acc2=0;
  float bestValue=FP_INFINITE;
  float splitPoint;
  int splitCounter=0;
  bool fail=true;

  if(!equals)
  for(int i=0;i<numBins-1;i++){
    accEntries+=entries[i];
    acc+=mean[i];
    acc2+=mean2[i];
    if(size-accEntries<minSize/2) break;
    if(accEntries<minSize/2) continue;

    int n0=accEntries;
    double rms0=acc2/n0-acc/n0*acc/n0;
    int n1=size-n0; 
    double rms1=(totalMean2-acc2)/n1-(totalMean-acc)/n1*(totalMean-acc)/n1; 
    double value=n0*rms0+n1*rms1-n01*rms01;

    if(fabs(value/n01*rms01)<-epsilon)  // Minimum improvement to split
      if(value<bestValue){
	bestValue=value;
	splitCounter=n0;
	splitPoint=minValue+bw*(i+1);
	fail=false;
      }
  }


  if(fail){
    // Try the standard split
    int accEntries=entries[0];
    for(int i=1;i<numBins-1;i++){
      if(accEntries+entries[i]>size/2){
	splitPoint=minValue+bw*(i+1);
	splitCounter=accEntries;
	fail=false;
	break;
      }
      accEntries+=entries[i];
    }
    // No way, store it a go ahead
    if(fail) {store(pointers,size,parent);return;}
  }

  if(splitCounter==size) {store(pointers,size,parent);return;}


  // Copy the pointer to the scratch area to sort them 
  for(int index=0;index<size;index++) scratch[index]=pointers[index];

  // Sort
  int lefties=0,righties=size-1;
  splitCounter=0;
  for(int index=0;index<size;index++){
    int element=scratch[index];
    float x=samples[offset(element)+direction];
    if(x<splitPoint){
      pointers[lefties++]=scratch[index];
      splitCounter++;
    }else{
      pointers[righties--]=scratch[index];
    }
  }


  // Store the current information
  limit.push_back(splitPoint);
  points.push_back(direction);

  nodes[0].push_back(0);  nodes[1].push_back(0);
  nodes[0][me]=nodes[0].size();
  grow_internal(pointers,splitCounter,scratch,-(me+1));
  nodes[1][me]=nodes[1].size();
  grow_internal(pointers+splitCounter,size-splitCounter,scratch,me+1);
}


ClassImp(GeomHashEnsemble);


void GeomHashEnsemble::growOne(int minSize,bool bootstrap){
  hashes.push_back(GeomHash(dimension));
  GeomHash &hash=hashes.back();

  if(!bootstrap){
    hash.samples=samples;
    hash.values=values;
  }else{
    checkBuffers();
    for(int i=0;i<values.size();i++){
      int pointer=int(values.size()*(rand() / (RAND_MAX + 1.0)));
      for(int j=0;j<dimension;j++) buffer[0][j]=samples[offset(pointer)+j];
      hash.push(values[pointer],buffer[0]);
    }
  }
  hash.grow(minSize);
}


void GeomHashEnsemble::Eval(float *x){
  MeanValue=0;
  ValueRms=0;
  MeanRms=0;
  RmsRms=0;
  MeanPeak=0;
  PeakRms=0;
  MeanPeakWidth=0;
  PeakWidthRms=0;
  Hashes=0;
  
  for(int i=0;i<hashes.size();i++){
    Hashes++;
    GeomHash &hash=hashes[i];
    int h=hash.hash(x);
    double x=hash.getMean(h);
    MeanValue+=x;
    ValueRms+=x*x;

    x=hash.getRms(h);
    MeanRms+=x;
    RmsRms+=x*x;

    x=hash.getPeak(h);
    MeanPeak+=x;
    PeakRms+=x*x;

    x=hash.getPeakWidth(h);
    MeanPeakWidth+=x;
    PeakWidthRms+=x*x;

    x=hash.getEntries(h);
    MeanEntries+=x;
    EntriesRms+=x*x;
  }

#define Do(_x) {Mean##_x/=Hashes; _x##Rms /=Hashes; _x##Rms-=Mean##_x*Mean##_x; _x##Rms=sqrt(fabs(_x##Rms));}
  Do(Value);
  Do(Rms);
  Do(Peak);
  Do(PeakWidth);
  Do(Entries);
#undef Do
}

void GeomHashEnsemble::Eval(double x,...){
  checkBuffers();
  va_list ap;
  va_start(ap, x);
  buffer[2][0]=x;
  for(int i=1;i<dimension;i++) buffer[2][i]=va_arg(ap, double);
  va_end(ap);
  Eval(buffer[2]);
}

int GeomHashEnsemble::numHashes(){
  return hashes.size();
}

GeomHash& GeomHashEnsemble::getHash(int i){
  return hashes.at(i);
}


