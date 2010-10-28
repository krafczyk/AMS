#include "clasifier.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <iostream>

#undef _DEBUG_
#define ASSERT(x) 

using namespace std;

ClassImp(Tree);

float Tree::robust_fraction=2.0;
int Tree::robust_trials=100;
bool Tree::robust_split=true;        // Use the robust mean for all splits. Useful for low statistics

bool Tree::verbose=false;            // Currently unused

Tree::Tree(int d){
  dimension=d;
  points[0].reserve(1024*d);   points[1].reserve(1024*d);
  nodes[0].reserve(1024);      nodes[1].reserve(1024);
}

Tree *Tree::clean(){
  Tree *new_tree=new Tree(dimension);
  Tree &latest=*new_tree;
  latest.points[0].resize(points[0].size());
  latest.points[1].resize(points[1].size());
  latest.nodes[0].resize(nodes[0].size());
  latest.nodes[1].resize(nodes[1].size());
  for(int which=0;which<2;which++){
    for(unsigned int i=0;i<latest.points[which].size();i++)
      latest.points[which][i]=points[which][i];
    for(unsigned int i=0;i<latest.nodes[which].size();i++)
      latest.nodes[which][i]=nodes[which][i];
  }
  
  return new_tree;
}

float Tree::eval(float *point,int index){
  const int offset=index*dimension;

  if(nodes[0][index]<0) return points[0][offset]; 

  ASSERT(nodes[1][index]>0);

  // Search the closer point
  float sign=0;

  for(int i=0;i<dimension;i++) sign+=(point[i]-0.5*(points[0][offset+i]+points[1][offset+i]))*(points[1][offset+i]-points[0][offset+i]);

  // Descent the tree recursively 
  return eval(point,sign<=0?nodes[0][index]:nodes[1][index]);
}

unsigned int Tree::node(float *point,int index){
  const int offset=index*dimension;

  if(nodes[0][index]<0) return index;

  // Search the closer point
  float sign=0;

  for(int i=0;i<dimension;i++) sign+=(point[i]-0.5*(points[0][offset+i]+points[1][offset+i]))*(points[1][offset+i]-points[0][offset+i]);

  // Descent the tree recursively 
  return node(point,sign<=0?nodes[0][index]:nodes[1][index]);
}


void Tree::push(float *x,float value,float weight){
  // Set the minimum capacity 
  if(samples.capacity()<1024*dimension) samples.reserve(1024*dimension);

  // Store the point
  for(int i=0;i<dimension;i++)  samples.push_back(x[i]);
  samples.push_back(value);
  samples.push_back(weight);
}


void Tree::grow(int tries,int minSize,float minRMS){
  lint entries=samples.size()/(dimension+2);

  // Create two temporary buffers to perform in place operation
  lint *buffer=new lint[entries];
  lint *scratch=new lint[entries];
  
  // Init the buffer
  for(lint i=0;i<entries;i++) buffer[i]=i;

  // Start the real grow process
  grow_internal(buffer,entries,scratch,tries,minSize,minRMS);

  delete[] buffer;
  delete[] scratch;
#ifdef _DEBUG_
    int terminals=0;
    for(int i=0;i<nodes[0].size();i++) if(nodes[0][i]==-1) terminals++;
    cout<<"Tree grown with "<<nodes[0].size()<<" nodes, "<<terminals<<" being terminal"<<endl<<endl;
    for(int i=0;i<nodes[0].size();i++) if(nodes[0][i]==-1) cout<<"  TERMINAL VALUE "<<points[0][i*dimension]<<endl;
#endif
}


void Tree::grow(lint *buffer,lint *scratch,int tries,int minSize,float minRMS){
  lint entries=samples.size()/(dimension+2);

  // Init the buffer
  for(lint i=0;i<entries;i++) buffer[i]=i;

  // Start the real grow process
  grow_internal(buffer,entries,scratch,tries,minSize,minRMS);

#ifdef _DEBUG_
    int terminals=0;
    for(int i=0;i<nodes[0].size();i++) if(nodes[0][i]==-1) terminals++;
    cout<<"Tree grown with "<<nodes[0].size()<<" nodes, "<<terminals<<" being terminal"<<endl<<endl;
    for(int i=0;i<nodes[0].size();i++) if(nodes[0][i]==-1) cout<<"  TERMINAL VALUE "<<points[0][i*dimension]<<endl;
#endif

}



void Tree::get_momentum(lint *buffer,int size,float &fmean,float &frms,float &fweight,int iterations=1){
  if(robust_split) iterations=robust_fraction<1?robust_trials:1;

  double best_mean=0,best_rms=HUGE_VAL,best_weight=0;  // We use doubles here to avoid precision problems in the additions, float in other places to optimize memory consumption

  fmean=frms=fweight=0;

  for(int n=0;n<iterations;n++){
    double sum=0,sum2=0,wsum=0;
    double mean=0,rms=0;
    for(lint i=0;i<size;i++){
      float w=samples[buffer[i]*(dimension+2)+dimension+1];
      best_weight+=w;
      if(robust_fraction<1) if(rand() / (RAND_MAX + 1.0)>robust_fraction) continue;
      sum+=samples[buffer[i]*(dimension+2)+dimension]*w;
      sum2+=samples[buffer[i]*(dimension+2)+dimension]*samples[buffer[i]*(dimension+2)+dimension]*w;
      wsum+=w;

#ifdef _DEBUG_
	cout<<"                           + ADDING "<<samples[buffer[i]*(dimension+2)+dimension]<<" sum "<<sum<<" sum2 "<<sum2<<" "<<w<<" "<<wsum<<endl;
#endif
    }
    
    if(best_weight==0) return;
    if(wsum==0) continue;
    
    mean=sum/wsum;
    rms=sum2/wsum-mean*mean;
#ifdef _DEBUG_
    cout<<" MEAN "<<mean<<" SRMS "<<rms<<" RMS "<<sqrt(rms)<<endl; 
#endif
    
    ASSERT(rms>=0);  // If this is not fulfilled there is a precision problem
    rms=rms<=0?0:sqrt(rms);

#ifdef _DEBUG_
      cout<<"************ GET MOMENTUM: "<<rms<<" best "<<best_rms<<"  means "<<mean<<" best "<<best_mean<<endl; 
#endif

    if(best_rms>rms){
      best_rms=rms;
      best_mean=mean;
    }
  }
  frms=best_rms;
  fmean=best_mean;
  fweight=best_weight;
}


#define Abs(x) ((x)<0?-(x):(x))

void Tree::grow_internal(unsigned long int *buffer,lint size,unsigned long int *scratch,int tries,int minSize,float minRMS){
#ifdef _DEBUG_
    cout<<"GROWING NODE "<<nodes[0].size()<<" size "<<size<<endl;
#endif

  // Create the current node space
  lint index=nodes[0].size(); // Index to the last one once it has been increased by 1

  // GROW THE BUFFERS
  for(int i=0;i<dimension;i++){points[0].push_back(0);   points[1].push_back(0);}
  nodes[0].push_back(-1); nodes[1].push_back(-1);

  // COmpute current values
  float mean=0,rms=0,weight=0;
  get_momentum(buffer,size,mean,rms,weight);

#ifdef _DEBUG_
    cout<<"   -- currently mean="<<mean<<" rms="<<rms<<" weight="<<weight<<endl;
#endif

  ASSERT(weight>0);

  // Fill the buffer as terminal
  nodes[0][index]=-1;
  nodes[1][index]=size;
  points[0][index*dimension]=mean;
  points[1][index*dimension]=rms;

  ////////////////////////////////////////////////////////
  // Check if this is really a terminal buffer
  /////////////////////////////////////////////////////////
#ifdef _DEBUG_
  cout<<"          -- CHECKING FINALIZERS size:"<<size<<" min:"<<minSize<<" rms:"<<rms<<" min:"<<minRMS<<endl;
#endif

  if(size<=minSize || rms<=minRMS || size==1){
    if(!robust_split){
      get_momentum(buffer,size,mean,rms,weight,Tree::robust_trials);
      points[0][index*dimension]=mean;
      points[1][index*dimension]=rms;
    }
    return;
  }

  ///////////////////////////////////////////
  // Try growing and keep the best one
  ///////////////////////////////////////////
  
  // Reference values
  float best_rms=HUGE_VAL; // Using huge val forces to split;
  int best_pair[2]={-1,-1};
  int best_n[2];
  best_n[0]=size;best_n[1]=0;

  for(int trial=0;trial<tries;trial++){
    // Get a pair
    int pair[2];
    pair[0]=buffer[int(size*(rand() / (RAND_MAX + 1.0)))];
    do pair[1]=buffer[int(size*(rand() / (RAND_MAX + 1.0)))]; while(pair[1]==pair[0]);

    // Sort the points using this pair
    lint n_0=0;
    lint n_1=0;
    float sign=0;

    for(lint ii=0;ii<size;ii++){
      sign=0;
      for(int i=0;i<dimension;i++) sign+=(samples[buffer[ii]*(dimension+2)+i]
					  -0.5*(samples[pair[0]*(dimension+2)+i]+samples[pair[1]*(dimension+2)+i]))*
	(samples[pair[1]*(dimension+2)+i]-samples[pair[0]*(dimension+2)+i]);
      
      // Store the sample in its bag
      if(sign<=0){
	scratch[n_0++]=buffer[ii];
      }else{
	scratch[size-1-n_1++]=buffer[ii];
      }

      ASSERT(n_0+n_1<=size);
    }

    // Compute the two new rms
    float dummy,rms0,weight0,rms1,weight1;
    get_momentum(scratch,n_0,dummy,rms0,weight0);
    get_momentum(scratch+n_0,n_1,dummy,rms1,weight1);

#ifdef _DEBUG_
      cout<<"   -- tested pair with rms_0="<<rms0<<" and rms_1="<<rms1<<" and sizes "<<n_0<<" and "<<n_1<<endl;
#endif
     
    float test=(weight0+weight1)==0?best_rms:(rms0*rms0*weight0+rms1*rms1*weight1)/(weight0+weight1);

    if(n_0>=minSize && n_1>=minSize){
      if(test<=best_rms){ 
	// candidate found
	best_pair[0]=pair[0];
	best_pair[1]=pair[1];
	best_rms=test;
	best_n[0]=n_0;
	best_n[1]=n_1;
	memcpy(buffer,scratch,sizeof(unsigned long int)*size);  // Moving this out could should performace
      }else if(test==best_rms && Abs((long int)n_0-(long int)n_1)<Abs(best_n[0]-best_n[1])){
	// candidate found
	best_pair[0]=pair[0];
	best_pair[1]=pair[1];
	best_rms=test;
	best_n[0]=n_0;
	best_n[1]=n_1;
	memcpy(buffer,scratch,sizeof(unsigned long int)*size);  // Moving this out could should performace
      }
    }

    // EVEN IF THERE IS A PERFECT MATCH, WE CONTINUE SPLITTING TO USE THE TREE FOR HISTOGRAMMING
    //    if(test==0) break; // PERFECT MATCH!!
  }

  // Check if splitting has been succsessful
#ifdef _DEBUG_
  cout<<"          -- CHECKING FINALIZERS 2:"<<best_n[0]<<" "<<best_n[1]<<endl;
#endif

  if(best_n[0]==0 || best_n[1]==0){ 
    if(!robust_split){
      get_momentum(buffer,size,mean,rms,weight,Tree::robust_trials);
      points[0][index*dimension]=mean;
      points[1][index*dimension]=rms;
    }
    return;
  }

  // We have a candidate, therefore we store it
  // Copy the points
  for(int i=0;i<dimension;i++) for(int j=0;j<2;j++) points[j][index*dimension+i]=samples[best_pair[j]*(dimension+2)+i];
  nodes[0][index]=nodes[0].size(); grow_internal(buffer,best_n[0],scratch,tries,minSize,minRMS);
  nodes[1][index]=nodes[1].size(); grow_internal(buffer+best_n[0],best_n[1],scratch,tries,minSize,minRMS);
}
#undef Abs


ClassImp(Forest);

void Forest::push(float *x,float value,float weight){
  // Store the point
  for(int i=0;i<dimension;i++)  samples.push_back(x[i]);
  samples.push_back(value);
  samples.push_back(weight);
}


void Forest::grow(int trees,float selection,int tries,int minSize,float minRMS){
  lint entries=samples.size()/(dimension+2);
  _buffer=new lint[entries];
  _scratch=new lint[entries];
  
  for(int i=0;i<trees;i++) grow_tree(selection,tries,minSize,minRMS);

  delete[] _buffer;
  delete[] _scratch;
}


void Forest::grow_tree(float selection,int tries,int minSize,float minRMS){
  // Create a tree
  Tree new_tree(dimension);

  // Fill the samples
  const int step=dimension+2;
  const int max=samples.size()/step;

  do{
    new_tree.samples.clear();
    new_tree.samples.reserve(samples.size());
    for(int i=0;i<max;i++){
      if(selection<1){ // Save few random numbers
	float value=float(rand() / (RAND_MAX + 1.0)); 
	if(value>selection) continue;
      }
      for(int j=0;j<step;j++) new_tree.samples.push_back(samples[i*step+j]);
    }
  }while(new_tree.samples.size()==0);

  // Grow the tree
  new_tree.grow(_buffer,_scratch,tries,minSize,minRMS);
  
  // Add the tree to the forest
  int index=forest.size();
  forest.push_back(Tree(dimension));
  //  forest.push_back(new_tree);


  Tree &latest=forest[index];
  latest.points[0].resize(new_tree.points[0].size());
  latest.points[1].resize(new_tree.points[1].size());
  latest.nodes[0].resize(new_tree.nodes[0].size());
  latest.nodes[1].resize(new_tree.nodes[1].size());

  for(int which=0;which<2;which++){
    for(unsigned int i=0;i<latest.points[which].size();i++)
      latest.points[which][i]=new_tree.points[which][i];
    for(unsigned int i=0;i<latest.nodes[which].size();i++)
      latest.nodes[which][i]=new_tree.nodes[which][i];
  }

}

float Forest::eval(float *point){
  double suma=0;
  unsigned int total=0;

  for(total=0;total<forest.size();suma+=forest[total++].eval(point));
  ASSERT(total>0 && total==forest.size());
  
  return suma/total;
}




//////////////////////////////////////////////// GeomHash
ClassImp(GeomHash);

double *GeomHash::_distances=0;
int GeomHash::trials=10;
float *GeomHash::buffer[nBuffers]={0,0,0,0};
int GeomHash::bufferSize=0;
int GeomHash::comparator(const void* p1,const void *p2){
  int index1=*((int*)p1);
  int index2=*((int*)p2);
  double d1=_distances[index1];
  double d2=_distances[index2];
  if(d1>d2) return -1; else return 1; // Check this: in principle this is sorted from closer to the first element in the pair (largest dist values) to closer to the second element (negative values)             
}

GeomHash::GeomHash(int d){
  dimension=d;
  numNodes=0;
  points[0].reserve(1024*d);   points[1].reserve(1024*d);
  nodes[0].reserve(1024);      nodes[1].reserve(1024);
  limit.reserve(1024);
  templates.reserve(1024);
  templatesRMS.reserve(1024);
  grown=false;

  // Grow the buffers if needed
  if(dimension>bufferSize){
    bufferSize=dimension;
    for(int i=0;i<nBuffers;i++){
      if(buffer[i]) delete buffer[i];
      buffer[i]=new float[dimension+1];
    }
  }
}

int GeomHash::hash(float *point){
  if(numNodes==0) return 0;
  int current=0;

  for(;;){
    // Copy the points to the buffers
    for(int i=0;i<dimension;i++) for(int which=0;which<2;which++) buffer[which][i]=points[which][offset(current)+i];

    // Take the metric value
    double d=dist(buffer[0],buffer[1],point);

    int nodeNumber=d<limit[current]?1:0;

    if(nodes[nodeNumber][current]<0) return -nodes[nodeNumber][current]-1;
    current=nodes[nodeNumber][current];
  }
}


double GeomHash::metric(float *p1,float *p2){
  double d=0;
  for(int i=0;i<dimension;i++) d+=(p1[i]-p2[i])*(p1[i]-p2[i]);
  return d;
}

double GeomHash::dist(float *r1,float *r2,float *point){
  double d1=0,d2=0;
  d1=metric(r1,point);
  d2=metric(r2,point);
  //  for(int i=0;i<dimension;i++) d1+=(r1[i]-point[i])*(r1[i]-point[i]);
  //  for(int i=0;i<dimension;i++) d2+=(r2[i]-point[i])*(r2[i]-point[i]);
  return d2-d1;
}

void GeomHash::push(float *x){
  // Set the minimum capacity
  if(samples.capacity()<1024*dimension) samples.reserve(1024*dimension);

  // Store the point
  for(int i=0;i<dimension;i++)  samples.push_back(x[i]);
}

#include <stdarg.h>

void GeomHash::fill(double x,...){
  va_list ap;
  va_start(ap, x);
  buffer[2][0]=x;
  for(int i=1;i<dimension;i++) buffer[2][i]=va_arg(ap, double);
  va_end(ap);
  push(buffer[2]);
}

int GeomHash::get(double x,...){
  va_list ap;
  va_start(ap, x);
  buffer[2][0]=x;
  for(int i=1;i<dimension;i++) buffer[2][i]=va_arg(ap, double);
  va_end(ap);
  return hash(buffer[2]);
}

void GeomHash::grow(int min_size,int robust_trials,double robust_fraction){
  int *pointers=new int[samples.size()];
  double *scratch=new double[samples.size()];
  grow(pointers,scratch,min_size,robust_trials,robust_fraction);
  delete[] pointers;
  delete[] scratch;
}

void GeomHash::grow(int *pointers,double *scratch,int min_size,int robust_trials,double robust_fraction){
  if(grown) return;
  grown=true;
  int entries=samples.size()/dimension;

  // Init the buffer
  for(int i=0;i<entries;i++) pointers[i]=i;

  // Start the real growing process
  minSize=2*min_size;
  robustFraction=robust_fraction;
  robustTrials=robust_trials;
  grow_internal(pointers,entries,scratch); // Adjust the min_size to obtain a better approximation

  samples.resize(0);
  vector<float> empty; samples=empty; // liberates a lot of space
}


#define _STORE_								\
  for(int d=0;d<dimension;d++){						\
    double best_sum=0;                                                  \
    double best_rms=HUGE_VAL;						\
    for(int trial=0;trial<robustTrials;trial++){			\
      double sum=0;							\
      double sum2=0;							\
      int total=0;							\
      for(int i=0;i<size;i++) {if((rand() / (RAND_MAX + 1.0))>robustFraction) continue;total++;sum+=samples[offset(pointers[i])+d];sum2+=samples[offset(pointers[i])+d]*samples[offset(pointers[i])+d];} \
      if(total==0) continue;						\
      sum/=total;							\
      sum2/=total; sum2-=sum*sum;					\
      if(sum2<best_rms){best_rms=sum2;best_sum=sum;}			\
    }									\
    templates.push_back(best_sum); templatesRMS.push_back(best_rms);	\
  }									\
  int currentHashNumber=numNodes++;					\
  int trueParent=fabs(parent)-1;					\
  int parentNode=parent<0?0:1;						\
  nodes[parentNode][trueParent]=-currentHashNumber-1;			\
  return;


void GeomHash::grow_internal(int *pointers,int size, double *scratch,int parent){
  if(size==1 || size<minSize) {_STORE_;}

  int me=nodes[0].size();

  // Pick randomly two different points
  int pair[2];
  bool equals=true;
  for(int trial=0;trial<trials;trial++){ // try 10 times (mode to a constant)
    pair[0]=pointers[int(size*(rand() / (RAND_MAX + 1.0)))];
    do pair[1]=pointers[int(size*(rand() / (RAND_MAX + 1.0)))]; while(pair[1]==pair[0]);

    for(int i=0;i<dimension;i++) if(samples[offset(pair[0])+i]!=samples[offset(pair[1])+i]){equals=false;break;}
    if(!equals) break;
  }

  if(equals && parent==0) return; // Finished: is it not possible to grow with identical points

  // Compute the metric for each point
  for(int i=0;i<dimension;i++) for(int which=0;which<2;which++) buffer[which][i]=samples[offset(pair[which])+i];
  for(int index=0;index<size;index++){
    int element=pointers[index];
    for(int i=0;i<dimension;i++) buffer[2][i]=samples[offset(element)+i];
    scratch[element]=dist(buffer[0],buffer[1],buffer[2]);
  }

  // Sort all of them using quick sort
  _distances=scratch;
  qsort(pointers,size,sizeof(int),comparator);


  // Check if the first and last value are the same
  if(scratch[pointers[0]]==scratch[pointers[size-1]]) {_STORE_;}


  // Search for the best splitting point
  double bestValue=0;
  int bestDiff=INT_MAX;
  int bestPosition=-1;
  for(int i=1;i<size;i++){
    if(scratch[pointers[i-1]]==scratch[pointers[i]]) continue;
    double value=0.5*(scratch[pointers[i-1]]+scratch[pointers[i]]);
    int diff=fabs(2*i-size);

    if(diff<bestDiff){
      bestPosition=i;
      bestValue=value;
      bestDiff=diff;
    }
  }

  // Store the current information
  limit.push_back(bestValue);
  for(int i=0;i<dimension;i++){
    points[0].push_back(buffer[0][i]);
    points[1].push_back(buffer[1][i]);
  }

  nodes[0].push_back(0);  nodes[1].push_back(0);
  nodes[0][me]=nodes[0].size();
  grow_internal(pointers,bestPosition,scratch,-(me+1));
  nodes[1][me]=nodes[1].size();
  grow_internal(pointers+bestPosition,size-bestPosition,scratch,me+1);
}
