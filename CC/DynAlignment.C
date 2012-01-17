//  $Id: DynAlignment.C,v 1.32 2012/01/17 21:32:18 mdelgado Exp $
#include "DynAlignment.h"
#include "TChainElement.h"
#include "TSystem.h"
#include "root_setup.h"

#include <iostream>
#include <algorithm>
#include "math.h"
#include "limits.h"
#include "stdlib.h"
#include "root.h"
#include <map>
#ifdef __ROOTSHAREDLIBRARY__
#include "amschain.h"
#endif


//////////////////////////////////////////////////////////////////////////////////////
// TODO: starting of interface to TDV implemented (only the dump to a linear array)
// in findpeak - Which one to use, the mean or the other
// Should I relax my cuts in distance to the preliminary center (now I have 2 sigmas, should I go to 3, or return to 1)
///////////////////////////////////////////////////////////////////77


// Time unit is the fraction of a 90 minutes orbit
#define TIMEUNIT (60*90)
// Fraction of events in ~1 sigma events
#define PEAKFRACTION (0.68)
// Half width of the initial window to search for the residuals (in cm) 
#define EXCLUSION 3.0
 

ClassImp(DynAlEvent);

void DynAlEvent::extrapolateTrack(){
  float dz=RawHit[2]-TrackHit[2];
  float dx=dz*tan(TrackTheta)*cos(TrackPhi);
  float dy=dz*tan(TrackTheta)*sin(TrackPhi);
  TrackHit[0]+=dx;
  TrackHit[1]+=dy;
  TrackHit[2]+=dz;
}


bool DynAlEvent::buildEvent(AMSEventR &ev,int layer,DynAlEvent &event){
  ///////////////////////////////
  // Dump the Time information //
  ///////////////////////////////

  event.Time[0]=ev.fHeader.Time[0];
  event.Time[1]=ev.fHeader.Time[1];


  /////////////////////////////////
  // Dump the position and cutoff//
  /////////////////////////////////

  event.ThetaM=ev.fHeader.ThetaM;
  event.PhiM=ev.fHeader.PhiM;
  event.Cutoff=0;


  ////////////////////////////////
  // Get the hit for the layer  //
  ////////////////////////////////

  if(ev.nParticle()!=1) return false;
  ParticleR &part=ev.Particle(0);
  if(part.iTrTrack()<0) return false;
  event.Cutoff=part.Cutoff;
  TrTrackR &track=ev.TrTrack(part.iTrTrack());
  TrRecHitR *hitp=0;

#ifdef _PGTRACK_
  //  hitp=track.GetHitLJ(layer);

  // Try to avoid biasing due to a wrong window in pattern recognition by building our own pattern recognition
  // We delegate thet election of the proper range to the DoFit function  
  // This also guarantees that the alignment does not depend in such parameters
  int trId=track.iTrTrackPar(1,3,3);
  if(trId<0) return false;
  int counter=0;
  double bestDist=HUGE_VAL;
  for(int hi=0;hi<ev.nTrRecHit();hi++){
    TrRecHitR &hit=ev.TrRecHit(hi);
    if(hit.GetLayerJ()!=layer) continue;
    if(hit.FalseX() || hit.OnlyY() || hit.OnlyX()) continue;
    int imult=hit.GetResolvedMultiplicity();	
    AMSPoint punto=hit.GetGlobalCoordinate(imult,"A");
    AMSPoint pnt;
    AMSDir dir;
    track.Interpolate(punto[2],pnt,dir,trId);
    double d=pnt.dist(punto);
    if(d<EXCLUSION) counter++;
    if(d<bestDist) {bestDist=d;hitp=&hit;}
  }

  if(counter>1) return false; // TEST
#else
  int whichHit=-1;
  for(int hi=0;hi<track.NTrRecHit();hi++){
    TrRecHitR &h=ev.TrRecHit(track.iTrRecHit(hi));
    if(h.lay()==layer){hitp=&h;whichHit=hi;break;}
  }
#endif

  if(!hitp) return false;
  TrRecHitR &hit=*hitp;

  ////////////////////////////////////////////////////////
  // Pick the hit position without alignment correction //
  ////////////////////////////////////////////////////////
  AMSPoint pnt;
  AMSDir dir;
#ifdef _PGTRACK_
  int imult=hit.GetResolvedMultiplicity();
  //  pnt=hit.GetGlobalCoordinate(imult,"");
  pnt=hit.GetGlobalCoordinate(imult,"A");
#else
  for(int i=0;i<3;i++) pnt[i]=hit.Hit[i];
#endif
  
  ///////////////////////////
  // Dump the hit position //
  //////////////////////////

  for(int i=0;i<3;i++) event.RawHit[i]=pnt[i];


  ////////////////////// 
  // Dump the hit id  //
  //////////////////////
  
#ifdef _PGTRACK_
  event.Id=(hit.GetLayerJ()+10*hit.GetSlotSide()+100*hit.lad())*(hit.OnlyY()?-1:1);
#else
  event.Id=hit.Id;
  if(hit.Status&16384 || hit.Status&8192) event.Id*=-1;
#endif



  ///////////////////////////////////////////////////////////////////////
  // Extrapolate the track to the layer and dump the track information //
  ///////////////////////////////////////////////////////////////////////

#ifdef _PGTRACK_
  int &id=trId;
  const TrTrackPar &trPar=track.gTrTrackPar(id);
  track.Interpolate(event.RawHit[2], pnt, dir, id) ;
  for(int i=0;i<3;i++) event.TrackHit[i]=pnt[i];
  event.TrackTheta=dir.gettheta();
  event.TrackPhi=dir.getphi();
  event.Rigidity=trPar.Rigidity;
#else
  TrTrackFitR fit(-TrTrackFitR::kI,2,1,1);  // Chikanian fit with MS and alignment
  int id=track.iTrTrackFit(fit,0);
  if(id<0) return false;
  TrTrackFitR &trPar=track.fTrTrackFit.at(id);
  int which=-1;
  for(int i=0;i<trPar.fTrSCoo.size();i++)
    if(trPar.fTrSCoo.at(i).Id%10==layer){
      which=i;
      break;
    }
  if(which<0) return false;
  for(int i=0;i<3;i++) event.TrackHit[i]=trPar.fTrSCoo[which].Coo[i];
  event.TrackTheta=trPar.fTrSCoo[which].Theta;
  event.TrackPhi=trPar.fTrSCoo[which].Phi;
  event.Rigidity=trPar.Rigidity;
#endif  

  event.Beta=part.pRichRing() && part.pRichRing()->IsClean()?part.pRichRing()->getBeta():0;
  
  return true;  // Success 
}

bool DynAlEvent::buildEvent(AMSEventR &ev,TrRecHitR &hit,DynAlEvent &event){
  ///////////////////////////////
  // Dump the Time information //
  ///////////////////////////////

  event.Time[0]=ev.fHeader.Time[0];
  event.Time[1]=ev.fHeader.Time[1];


  /////////////////////////////////
  // Dump the position and cutoff//
  /////////////////////////////////

  event.ThetaM=ev.fHeader.ThetaM;
  event.PhiM=ev.fHeader.PhiM;
  event.Cutoff=0;

  /////////////////////////////////////////
  // Get the hit position for the layer  //
  /////////////////////////////////////////

    AMSPoint pnt;
#ifdef _PGTRACK_
  int imult=hit.GetResolvedMultiplicity();
  pnt=hit.GetGlobalCoordinate(imult,"A");
#else
  for(int i=0;i<3;i++) pnt[i]=hit.Hit[i];
#endif
  for(int i=0;i<3;i++){event.RawHit[i]=pnt[i];event.TrackHit[i]=0;}

  event.TrackTheta=event.TrackPhi=event.Rigidity=0;

#ifdef _PGTRACK_
  event.Id=hit.GetLayerJ()+10*hit.GetSlotSide()+100*hit.lad();
#else
  event.Id=hit.Id;
#endif

  event.Beta=0;

  
  return true;  // Success 
}


int DynAlEvent::getClass(){
  if(Id<0) return -1; // False X
  if(Beta>0.995) return 0;
  if(Beta>0.97) return 1;
  if(Cutoff>15) return 2;
  return 3;
}


ClassImp(DynAlHistory);

int DynAlHistory::FindCloser(int seconds,int muSeconds,int initial){
  // Brute force approach: loop an all events and find the closer
  int current=-1;
  double bestSeconds=INFINITY;

  if(initial<0) initial=0;

  for(unsigned int i=initial;i<Events.size();i++){
    double t=double(Events[i].Time[0]-seconds)+1e-6*double(Events[i].Time[1]-muSeconds);
    t=fabs(t);

    if(t<=bestSeconds){
      bestSeconds=t;
      current=i;
      continue;
    }

    // If the error increases then we have finished
    break;
  }
#ifdef VERBOSE__
  if(current==-1){
    cout<<"NO EVENT FOUND"<<endl;
  }else{
    cout<<"FOUND TIME "<<Events[current].Time[0]<<"::"<<Events[current].Time[1]<<" for "<<seconds<<"::"<<muSeconds<<endl;
  }
#endif

  return current;
}


int DynAlHistory::Find(DynAlEvent &event,int initial){
  for(unsigned int i=initial;i<Events.size();i++){
    DynAlEvent &c=Events[i];
    if(c.Time[0]==event.Time[0] &&
       c.Time[1]==event.Time[1] &&
       c.RawHit[0]==c.RawHit[0] &&
       c.RawHit[1]==event.RawHit[1] &&
       c.RawHit[2]==event.RawHit[2]) return i;
    if(c.Time[0]>event.Time[0]) return -1;
  }
  return -1;
}

void DynAlHistory::FindTimeWindow(int event,int seconds,int &first,int &last){
  // Get the initial time (only treat with seconds)
  first=last=event;
  if(seconds<=0) return;
  seconds--;


  // look for the upper limit
  for(last=event;last<int(Events.size());last++)
    if(Events[last].Time[0]>Events[event].Time[0]+seconds) break;

  // look for the lower limit
  for(first=event;first>=0;first--)
    if(Events[first].Time[0]<Events[event].Time[0]-seconds) break;

  if(first<0) first=0;
  if(last>=int(Events.size())) last=int(Events.size()-1);
}


//////////////////////////////////////////////////////////////////////////////

DynAlFit::DynAlFit(int order){
  ZOffset=-136.0;First=Last=Excluded=-1;
  MinRigidity=0;MaxRigidity=1e6;MinBeta=0;Events=0;
  Order=order;
  ControlGroup=-1;
  Init();
}

void DynAlFit::Init(){
  Fit.SetDim((Order+1)*6);
  TString formula="x[0] ++ x[1] ++x[2] ++ x[3] ++ x[4] ++ x[5]";
  for(int i=0;i<Order;i++)
    formula+=TString(Form(" ++ x[0]*x[6]**%i ++ x[1]*x[6]**%i ++x[2]*x[6]**%i ++ x[3]*x[6]**%i ++ x[4]*x[6]**%i ++ x[5]*x[6]**%i",i+1,i+1,i+1,i+1,i+1,i+1));
  Fit.SetFormula(formula);
  Fit.StoreData(false);

}


bool DynAlFit::DoFit(DynAlHistory &history,int first,int last,DynAlEvent &exclude,bool force){
  int e=history.Find(exclude,first);
  if(!force)
    if(First==first && Last==last && Excluded==e) return false;  // The current fit is OK
  First=first;Last=last;Excluded=e;

  const int total=1+last-first;
  if(total<=(Order+1)*6) return false;   // Do not try the fit, it will fail. Use previous instead

  // Get a time reference (seconds precision)
  int timeRef=history.FindCloser(exclude.Time[0],exclude.Time[1],first);
  TOffset=history.Events.at(timeRef).Time[0];

  set<int> excluded;

  //Exclude everything within +/- 1 second of the "excluded" event
  int f,l;
  history.FindTimeWindow(timeRef,1,f,l);
  for(int i=f;i<=l;i++){
#ifdef VERBOSE__
    cout<<"EXCLUDING "<<history.Events[i].Time[0]-exclude.Time[0]<<endl;
#endif
    excluded.insert(i);
  }

  //#define VERBOSE__
#ifdef VERBOSE__
  int output=ForceFit(history,first,last,excluded);  

  if(Order!=0) return output;
  // Compute the statistical error for current event X and Y after correction
  TMatrixD matrix;
  Fit.GetCovarianceMatrix(matrix);
  TVectorD v(6);
  v[0]=1; //DX
  v[1]=0;
  v[2]=0;
  v[3]=exclude.RawHit[1];
  v[4]=fabs(exclude.RawHit[2]-ZOffset);
  v[5]=0;
  double errx=0;
  TVectorD r=matrix*v;
  for(int i=0;i<6;i++) errx+=r[i];


  v[0]=0; //DY
  v[1]=1;
  v[2]=0;
  v[3]=exclude.RawHit[0];
  v[4]=0;
  v[5]=fabs(exclude.RawHit[2]-ZOffset);

  double erry=0;
  r=matrix*v;
  for(int i=0;i<6;i++) erry+=r[i];
  {
    static int counter=0;
    if(counter==0)
      cout<<"Errors "<<exclude.Time[0]<<" "<<sqrt(errx)<<" "<<sqrt(erry)<<" "<<ZOffset<<endl;
    counter++;if(counter==10) counter=0;
  }
  return output;
#undef VERBOSE__
#else
  return ForceFit(history,first,last,excluded);  
#endif



}

bool DynAlFit::ForceFit(DynAlHistory &history,int first,int last,set<int> &excluded){
  // Fit taking into account different classes
  Fail=true;
  Fit.ClearPoints();

  Double_t x[8]; // variables

  // Use meaningful names
  Double_t &Dx=x[0];
  Double_t &Dy=x[1];
  Double_t &Dz=x[2];
  Double_t &theta=x[3];
  Double_t &alpha=x[4];
  Double_t &beta=x[5];
  Double_t &time=x[6];

  double y=0;
  double error=0;

  // Compute ZOffset
  ZOffset=0;
  const int total=1+last-first;
  for(int i=first;i<=last;i++) ZOffset+=history.Events.at(i).RawHit[2];
  ZOffset/=total;

  // Counter
  Events=0;


  // Store the following informatio per class
  map<int,CLASSINFO> classInfo;
  map<int,vector<double> > dx[2];
  
#define SubClass(_class,_ev) (fabs(_ev.TrackHit[0]-_ev.RawHit[0]-classInfo[_class].mean[0])>0.03?100+_class:_class)
  

  for(int which=0;which<2;which++){  // Loop in the axis
    // Fill the information to determine each class qualities
    for(int i=first;i<=last;i++){
      if(excluded.count(i)>0) continue;
      DynAlEvent &ev=history.Events.at(i);
      ev.extrapolateTrack();
      int Class=ev.getClass();
      if(Class<0) continue;
      if(which==0)
	dx[which][Class].push_back(ev.TrackHit[which]-ev.RawHit[which]);
      else
	dx[which][SubClass(Class,ev)].push_back(ev.TrackHit[which]-ev.RawHit[which]);
    }
    
    int worstClass[2]={-1,1};
    double worstRMS[2]={0,0};

    // Determine mean and rms for each class
    for(map<int,vector<double> >::iterator i=dx[which].begin();i!=dx[which].end();i++){
      int Class=i->first;
      
      CLASSINFO &info=classInfo[Class];
      
      info.mean[which]=0;
      info.rms[which]=1;
      info.entries[which]=0;
      
      findPeak(dx[which][Class],PEAKFRACTION,-3,3,info.mean[which],info.rms[which],10000);
      info.rms[which]/=2;  // findPeak returns the full width
      info.entries[which]=dx[which][Class].size();

      if(info.rms[which]>worstRMS[which]){
	worstRMS[which]=info.rms[which];
	worstClass[which]=Class;
      }

      //#define VERBOSE__
#ifdef VERBOSE__
      if(which==1)
	cout<<"INFO FOR CLASS "<<Class<<endl
	    <<" MEAN X="<<info.mean[0]<<" Y="<<info.mean[1]<<endl
	    <<" RMS X="<<info.rms[0]<<" Y="<<info.rms[1]<<endl
	    <<" ENTRIES "<<info.entries[0]<<" "<<info.entries[1]<<endl;
#endif
      
    }


    
#ifdef VERBOSE__
    int entriesPerClass[10]={0,0,0,0,0,0,0,0,0,0};
    double meanTime=0;
#endif
    
    for(int i=first;i<=last;i++){
      DynAlEvent event=history.Events.at(i); // Take a copy of the event

      // Allow to have a control group easily
      if(DynAlManager::ControlGroup>0 && (event.Time[0]%DynAlManager::ControlGroup)==0) continue;

      int Class=event.getClass();
      if(Class<0) continue;
      if(which==1) Class=SubClass(Class,event);
      //      if(classInfo[Class].entries[which]<100) continue;
      if(classInfo[Class].entries[which]<100) Class=worstClass[which]; // Recover the events
      
      // Shift the Z origin
      double zHit=event.RawHit[2]-ZOffset;
      double zTr=event.TrackHit[2]-ZOffset;
      
      // Fit in Y axis
      float &xHit=event.RawHit[0];
      float &yHit=event.RawHit[1];
      float &xTr=event.TrackHit[0];
      float &yTr=event.TrackHit[1];
      
      time=(history.Events.at(i).Time[0]-TOffset)+1e-6*history.Events.at(i).Time[1];
      time/=TIMEUNIT;  // Simplify a bit the computation
      

      if(classInfo[Class].rms[0]>1e-6)
	if(fabs(xTr-xHit-classInfo[Class].mean[0])>2*classInfo[Class].rms[0]) continue;

      if(classInfo[Class].rms[1]>1e-6)
		if(fabs(yTr-yHit-classInfo[Class].mean[1])>2*classInfo[Class].rms[1]) continue;


      double v;
      if(which==0){
	//////////////////////////////////////////////
	// First the x component
	y=xTr-xHit;   
	Dx=1;
	Dy=0;
	v=tan(event.TrackTheta)*cos(event.TrackPhi);
	Dz=               -v;
	theta=-yHit;
	alpha=-zHit  -xHit*v;
	beta=        -yHit*v;
	
    // Compute the probabity of being signal for this event and get the expected error
      }else{
	//////////////////////////////////////////////
	// Second the y component 
	y=yTr-yHit;
	Dx=0;
	Dy=1;
	v=tan(event.TrackTheta)*sin(event.TrackPhi);
	Dz=-v;
	theta=xHit;
	alpha=-xHit*v;
	beta=-zHit-yHit*v;
      }

      error=classInfo[Class].rms[which];
      if(error<1e-6) continue;
      Fit.AddPoint(x,y,error);
      Events++;
    }
  }
  
  if(Fit.Eval()) return false;
  
#ifdef VERBOSE__
  //////////// TEST
  RetrieveFitPar(0,0);
  cout<<"FINAL POSITION DX "<<DX<<" DY "<<DY<<" "<<DZ<<endl;
  cout<<"               THETA "<<THETA<<" A "<<ALPHA<<" B "<<BETA<<endl;
  TVectorD errors;
  Fit.GetErrors(errors);
  cout<<"ERRORS: "<<endl
      <<"    DX: "<<errors[0]<<endl
      <<"    DY: "<<errors[1]<<endl
      <<"    DZ: "<<errors[2]<<endl
      <<" THETA: "<<errors[3]<<endl
      <<" ALPHA: "<<errors[4]<<endl
      <<"  BETA: "<<errors[5]<<endl
      <<"                   CHI2 "<<Fit.GetChisquare()<<"  DOF "<<Fit.GetNpoints()<<endl;


  Double_t *matr=Fit.GetCovarianceMatrix();
  for(int i=0;i<6;i++){
    for(int j=0;j<6;j++) cout<<matr[6*i+j]<<" ";
    cout<<endl;
  };cout<<endl;

#undef VERBOSE__
#endif

  //#define REPORT__
#ifdef REPORT__
  // write a report of what's going on
  int t=history.Events.at(first).Time[0]/2+history.Events.at(last).Time[0]/2;
  RetrieveFitPar(t,0);
  cout<<"REPORT ";
  cout<<t<<" "<<(ZOffset>0?1:9)<<" "; // Time && layer
  cout<<Events/2<<" ";
  cout<<DX<<" "<<DY<<" "<<DZ<<" "<<THETA<<" "<<ALPHA<<" "<<BETA<<" "; // Fit result
  TVectorD errors;
  Fit.GetErrors(errors);
  for(int i=0;i<6;i++) cout<<errors[i]<<" ";  // Errors
  // Finally the class composition
  for(int which=0;which<2;which++){
    int total=0;
    for(int c=0;c<4;c++){
      if(classInfo.find(c)!=classInfo.end()) total+=classInfo[c].entries[which];
    }
    if(which)  for(int c=0;c<4;c++){
      if(classInfo.find(100+c)!=classInfo.end()) total+=classInfo[100+c].entries[which];
    }
  
    // The report itself 
    for(int c=0;c<4;c++){
      if(classInfo.find(c)!=classInfo.end()) cout<<float(classInfo[c].entries[which])/total<<" "<<classInfo[c].rms[which]<<" "; else cout<<0<<" "<<0<<" "; 
    }
    if(which)  for(int c=0;c<4;c++){
      if(classInfo.find(100+c)!=classInfo.end()) cout<<float(classInfo[100+c].entries[which])/total<<" "<<classInfo[100+c].rms[which]<<" "; else cout<<0<<" "<<0<<" ";
    }
  }
  cout<<endl;
#undef REPORT__
#endif
  

  Fail=false;
#undef SubClass
  return true;
}


void DynAlFit::RetrieveFitPar(int t0,int t1){
  TVectorD par;
  Fit.GetParameters(par);
  int p=0;
  DX=par[p++];
  DY=par[p++];
  DZ=par[p++];
  THETA=par[p++];
  ALPHA=par[p++];
  BETA=par[p++];

  double time=(t0-TOffset)+1e-6*t1;
  time/=TIMEUNIT;  // Simplify a bit the computation

  for(int i=0;i<Order;i++){
    double dt=pow(time,i+1);
    // First order correction
    DX+=par[p++]*dt;
    DY+=par[p++]*dt;
    DZ+=par[p++]*dt;
    THETA+=par[p++]*dt;
    ALPHA+=par[p++]*dt;
    BETA+=par[p++]*dt;
  }

}

void DynAlFit::Eval(AMSEventR &ev,double &x,double &y,double &z){
  DynAlEvent event;
  for(int i=0;i<2;i++) event.Time[i]=ev.fHeader.Time[i];
  event.RawHit[0]=x;
  event.RawHit[1]=y;
  event.RawHit[2]=z;
  Eval(event,x,y,z);
}

void DynAlFit::Eval(DynAlEvent &event,double &x,double &y,double &z){
  RetrieveFitPar(event.Time[0],event.Time[1]);
  
  double zHit=event.RawHit[2]-ZOffset;
  double dx=DX-THETA*event.RawHit[1]-ALPHA*zHit;
  double dy=DY+THETA*event.RawHit[0]-BETA*zHit;
  double dz=DZ+ALPHA*event.RawHit[0]+BETA*event.RawHit[1];
  
  
  x=event.RawHit[0]+dx;
  y=event.RawHit[1]+dy;
  z=event.RawHit[2]+dz;
}


void DynAlFit::getRange(vector<double> &array,double &Min,double &Max,int buckets){
  // Get the range
  Min=HUGE_VAL;
  Max=-HUGE_VAL;
  for(int i=0;i<array.size();i++){
    if(array[i]<Min) Min=array[i];
    if(array[i]>Max) Max=array[i];
  }

  double binSize=(Max-Min)/buckets;
  Min-=binSize/2;
  Max+=binSize/2;
}

bool DynAlFit::bucketSort(vector<double> &array,const int buckets){
  if(!buckets) return true;

  double Min,Max;
  getRange(array,Min,Max,buckets);

  if(Min==Max) return true;

  return bucketSort(array,Min,Max,buckets);
}


bool DynAlFit::bucketSort(vector<double> &array,double Min,double Max,const int buckets){
  if(!buckets) return true;

  int bucket[buckets];
  const double binSize=(Max-Min)/buckets;
#define getbin(_x) int(floor(((_x)-Min)/binSize));  

  // Empty the buckets
  for(int i=0;i<buckets;i++) bucket[i]=0;

  // Fill the buckets
  for(int i=0;i<array.size();i++){
    int b=getbin(array[i]); 
    if(b<0 || b>=buckets) continue;
    bucket[b]++;
  }
  
  // Dump the sorted array
  array.clear();
  for(int i=0;i<buckets;i++){
    if(!bucket[i]) continue;
    double x0=Min+(i+0.5)*binSize;
    double delta=binSize/bucket[i];
    for(int n=0;n<bucket[i];n++){
      double x=x0+n*delta;
      array.push_back(x);
    }
  }
  return true;
#undef getbin
}

bool DynAlFit::findPeak(vector<double> &array,const double fraction,double &peak,double &width,int buckets){
  double Min,Max;
  getRange(array,Min,Max,buckets);

  if(Min==Max){peak=Min;width=0;return true;}
  
  return findPeak(array,fraction,Min,Max,peak,width,buckets);
}


bool DynAlFit::findPeak(vector<double> array,const double fraction,double Min,double Max,double &peak,double &width,int buckets){
  if(fraction>=1 || fraction<0) return false;

  // Sort the array
  if(!bucketSort(array,Min,Max,buckets)) return false;
  
  // Get the search window size
  const int window=int(array.size()*fraction);
  
  if(!window) return false;

  width=HUGE_VAL;
  int maxPosition=-1;
  for(int i=window;i<array.size();i++){
    double l=array[i]-array[i-window];
    if(l<width){
      width=l;
      maxPosition=i;
    }
  }
  
  peak=0.5*(array[maxPosition]+array[maxPosition-window]);
  return true;

  //TEST: this seems to introduce a bias in B550 wrt B530
  // However the residuals width are smaller 
  // Refine the mean
  peak=0;
  int counter=0;
  for(int i=0;i<=window;i++) {peak+=array[maxPosition-i];counter++;}
  peak/=counter;

  return true;
}


ClassImp(DynAlContinuity);
// Number of seconds in a week
#define magicNumber 605800
double DynAlContinuity::BetaCut=0.995;  // Unused
double DynAlContinuity::RigidityCut=5;  // Unused
int    DynAlContinuity::FitOrder=0;
int    DynAlContinuity::FitWindow=20;

int DynAlContinuity::getBin(int run){
  return (run/magicNumber)*magicNumber;
}

int DynAlContinuity::getNextBin(int run){
  return (run/magicNumber)*magicNumber+magicNumber;
}

int DynAlContinuity::getPreviousBin(int run){
  return (run/magicNumber)*magicNumber-magicNumber;
}


bool DynAlContinuity::select(AMSEventR *ev,int layer){
#define SELECT(_name,_condition) {if(!(_condition)) return false;}
#define fStatus (ev->fStatus)  

  SELECT("1 Tracker+RICH particle",(fStatus&0x13)==0x11);
  SELECT("At most 1 anti",((fStatus>>21)&0x3)<=1);
  SELECT("At most 4 tof clusters",(fStatus&(0x7<<10))<=(0x4<<10));
  SELECT("At least 1 tr track",fStatus&(0x3<<13));

#ifndef _PGTRACK_
  SELECT("XCheck",ev->pParticle(0) && ev->pParticle(0)->pTrTrack());  
#endif

  SELECT("At most 1 crossing particle at rich",ev->pParticle(0)->RichParticles<=1);
  SELECT("At least 3 out of 4 for beta",ev->Particle(0).pBeta()->Pattern<=4);
  SELECT("TOF beta>0.9",ev->Particle(0).pBeta()->Beta>0.9);  




  bool layerUsed[10];
  for(int l=0;l<10;l++) layerUsed[l]=false;
  TrTrackR &track=*(ev->Particle(0).pTrTrack());
  for(int hi=0;hi<track.NTrRecHit();hi++){
    TrRecHitR &hit=*(track.pTrRecHit(hi));
    layerUsed[hit.lay()]=true;
  }

  SELECT("The inner tracker has layer 2 and plane(7,8) and other inner hit",layerUsed[2] && (layerUsed[7] || layerUsed[8]) && (layerUsed[3] || layerUsed[4]) && (layerUsed[5] || layerUsed[6])); // CHANGED

  // Finally use the implicit cuts in DynAlEvent::buildEvent
  DynAlEvent event;
  if(!DynAlEvent::buildEvent(*ev,layer,event)) return false;
  if(event.Id<0) return false;
  event.extrapolateTrack();
  if(fabs(event.RawHit[0]-event.TrackHit[0])>EXCLUSION ||
     fabs(event.RawHit[1]-event.TrackHit[1])>EXCLUSION) return false;

  return true;
#undef SELECT
#undef fStatus
}


#ifdef __ROOTSHAREDLIBRARY__

#ifdef _PGTRACK_
#include "TkDBc.h"
#endif

void DynAlContinuity::CleanAlignment(){
#ifdef _PGTRACK_
  // Cleanup the external planes alignment, just in case
  int plane[2]={5,6};
  for(int i=0;i<2;i++){
    TkPlane* pl = TkDBc::Head->GetPlane(plane[i]);
    if (!pl) continue;
    pl->posA.setp(0,0,0);
    pl->rotA.Reset();
  }
#endif
}


void DynAlContinuity::CreateIdx(AMSChain &ch,int layer,TString dir_name,TString prefix){
  // Create the history object
  ch.Rewind();
  AMSEventR *ev=ch.GetEvent();
  if(!ev) return;
  int current_run=ev->fHeader.Run;
  DynAlHistory history(current_run,layer);

  ch.Rewind();
  int events = ch.GetEntries();
  for (int entry=0; entry<events; entry++) {
    AMSEventR *pev = ch.GetEvent(entry);
    CleanAlignment();
    if(!select(pev,layer)) continue;
    if(pev->fHeader.Run!=current_run) break;
    
    DynAlEvent event;
    if(!DynAlEvent::buildEvent(*pev,layer,event)) continue;
    event.extrapolateTrack();
    history.Push(event);
  }

  // Clean up
  ch.Rewind();

  // Sort the history (just in case)
  sort(history.Events.begin(),history.Events.end());

  
  // Save the file: this requires
  //    1st find the directory where it should be placed
  //    2nd create it if it does not exist
  //    3rd writing it in a suitable manner  

  
  TString dir=Form("%s/%i/",dir_name.Data(),getBin(current_run));
  gSystem->mkdir(dir);
  TString file_name=Form("%u.idx.root",current_run);
  TFile file(dir+file_name,"UPDATE");
  history.Write(prefix+TString("idx"));
  file.Close();
  cout<<"Wrote "<<history.Size()<<" events in "<<file_name<<endl;
}
#endif

void DynAlContinuity::GetFileList(TString dir,vector<int> &lista){
  lista.clear();
  void *dirp=gSystem->OpenDirectory(dir);
  if(!dirp) return;
  for(const char *name=gSystem->GetDirEntry(dirp);name!=0;name=gSystem->GetDirEntry(dirp)){

	TString entry(name);
	if(!entry.Contains(".idx.root")) continue;
	int run;
	sscanf(entry.Data(),"%i.idx.root",&run);
	lista.push_back(run);
      }
  sort(lista.begin(),lista.end());
}

bool DynAlContinuity::Update(int run){
  if(CurrentRun==run && CurrentRun!=-1) return false;
  ForceUpdate(DirName,Prefix,run);
  return true;
}

void DynAlContinuity::ForceUpdate(TString dir_name,TString prefix,int run){
  CurrentRun=run;
  History.Run=run;
  //  History.Layer=layer;
  DirName=dir_name;
  Prefix=prefix;
  //  Layer=layer;

  if(run<0) return;// Skip dummy run numbers

  // Find the idx file for current run
  TString dir=Form("%s/%i/",dir_name.Data(),getBin(CurrentRun));

  // Get the list of files in current directory
  vector<int> runs;
  GetFileList(dir,runs);

  if(runs.size()==0){
    cout<<"Problem updating with dir="<<dir_name<<" prefix="<<prefix<<" run="<<run<<endl;
    return;
  }
  
  History.Events.clear();

  // Find the position of current run
  int position=-1;
  for(int i=0;i<runs.size();i++)
    if(runs[i]==CurrentRun){position=i;break;}

  if(position==-1) return;

  // Join the different runs
  if(position>0 && position<runs.size()-1){
    // Normal case
    if(position-1>=0) Fill(dir,prefix,runs.at(position-1));
    FirstOfCurrentRun=History.Size();
    Fill(dir,prefix,runs.at(position));
    LastOfCurrentRun=History.Size()-1;
    if(position+1<runs.size()) Fill(dir,prefix,runs.at(position+1));
    return;
  }

  if(position==0){
    TString mydir=Form("%s/%i/",dir_name.Data(),getPreviousBin(CurrentRun));
    // Get the list of files in current directory
    vector<int> list;
    GetFileList(mydir,list);
    if(list.size()>0) Fill(mydir,prefix,list.at(list.size()-1));
    FirstOfCurrentRun=History.Size();
    Fill(dir,prefix,runs.at(position));
    LastOfCurrentRun=History.Size()-1;
    if(position+1<runs.size()) Fill(dir,prefix,runs.at(position+1));
    return;
  }

  
  if(position-1>=0) Fill(dir,prefix,runs.at(position-1));
  FirstOfCurrentRun=History.Size();
  if(position>=0)  Fill(dir,prefix,runs.at(position));
  LastOfCurrentRun=History.Size()-1;
  TString mydir=Form("%s/%i/",dir_name.Data(),getNextBin(CurrentRun));
  // Get the list of files in current directory
  vector<int> list;
  GetFileList(mydir,list);
  if(list.size()>0) Fill(mydir,prefix,list.at(0));
}


void DynAlContinuity::Fill(TString dir,TString prefix,int run){
  // Open the file
  TString file_name=Form("/%u.idx.root",run);

  //#ifdef VERBOSE__
  cout<<"Opening file "<<(dir+file_name)<<" for reading"<<endl;
  //#endif

  TFile file(dir+file_name);
  DynAlHistory *h=(DynAlHistory*)file.Get(prefix+TString("idx"));
  if(!h){
    cout<<"Object "<<(prefix+TString("idx"))<<" not found in "<<(dir+file_name)<<endl;
    return;
  }
  for(int i=0;i<h->Size();i++)  History.Push(h->Get(i));
  Layer=History.Layer=h->Layer;

#ifdef VERBOSE__
  cout<<"     READ "<<h->Size()<<" events"<<endl;
#endif
  delete h;
  file.Close();
}


bool DynAlContinuity::UpdateFit(AMSEventR &ev){
  int Minutes=FitWindow;  // This should be move out
  Fit.MinRigidity=RigidityCut;
  Fit.MinBeta=BetaCut;

  // Update the run sequence if required
  Update(ev.fHeader.Run);
  // Get event number 
  int current=History.FindCloser(ev.fHeader.Time[0],ev.fHeader.Time[1],0);
  // Find the time window
  int first,last;
  History.FindTimeWindow(current,Minutes*30,first,last);
  
  // Do the fit if needed, excluding current event
  DynAlEvent event;
  DynAlEvent::buildEvent(ev,Layer,event);
  Fit.DoFit(History,first,last,event);
  if(Fit.Fail) return false;
  return true;
}



ClassImp(DynAlFitParameters);

DynAlFitParameters::DynAlFitParameters(DynAlFit &fit){
  TVectorD par;
  fit.Fit.GetParameters(par);
  int p=0;
  for(int i=0;i<=fit.Order;i++){ 
    DX.push_back(par[p++]);
    DY.push_back(par[p++]);
    DZ.push_back(par[p++]);
    THETA.push_back(par[p++]);
    ALPHA.push_back(par[p++]);
    BETA.push_back(par[p++]);
  }
  
  ZOffset=fit.ZOffset;
  TOffset=fit.TOffset;
}

void DynAlFitParameters::ApplyAlignment(double &x,double &y,double &z){
  ApplyAlignment(TOffset,0,x,y,z);
}

void DynAlFitParameters::ApplyAlignment(int seconds,int museconds,double &x,double &y,double &z){
  double _DX=0;
  double _DY=0;
  double _DZ=0;
  double _THETA=0;
  double _ALPHA=0;
  double _BETA=0;
  
  double elapsed=GetTime(seconds,museconds);
  double dt=1;

#define Do(xx) _##xx+=dt* xx.at(i) 
  for(int i=0;i<DX.size();i++){
    Do(DX);
    Do(DY);
    Do(DZ);
    Do(THETA);
    Do(ALPHA);
    Do(BETA);
    dt*=elapsed;
  }
#undef Do

  double zHit=z-ZOffset;
  double dx=_DX-_THETA*y-_ALPHA*zHit;
  double dy=_DY+_THETA*x-_BETA*zHit;
  double dz=_DZ+_ALPHA*x+_BETA*y;
  
  x+=dx;
  y+=dy;
  z+=dz;
}

void DynAlFitParameters::GetParameters(int seconds,int museconds,AMSPoint &posA,AMSRotMat &rotA){
  double _DX=0;
  double _DY=0;
  double _DZ=0;
  double _THETA=0;
  double _ALPHA=0;
  double _BETA=0;
  
  double elapsed=GetTime(seconds,museconds);
  double dt=1;

#define Do(xx) _##xx+=dt* xx.at(i) 
  for(int i=0;i<DX.size();i++){
    Do(DX);
    Do(DY);
    Do(DZ);
    Do(THETA);
    Do(ALPHA);
    Do(BETA);
    dt*=elapsed;
  }
#undef Do

  //  double zHit=z-ZOffset;

#ifdef VERBOSE__
  cout<<"GETPARAMETERS EVALUATING "<<seconds<<" "<<museconds<<endl;
  cout<<"DUMPING PARAMETERS "<<endl;
#define dump(_x)   for(int i=0;i<_x.size();i++) cout<<_x[i]<<" ";cout<<endl;
  dump(DX);
  dump(DY);
  dump(DZ);
  dump(THETA);
  dump(ALPHA);
  dump(BETA);
#undef dump
#endif

  posA.setp(_DX,_DY,_DZ);

  double matrix[3][3];
  for(int i=0;i<3;i++) matrix[i][i]=1;
  
  matrix[0][1]=-_THETA;
  matrix[0][2]=-_ALPHA;
  matrix[1][0]=_THETA;
  matrix[1][2]=-_BETA;
  matrix[2][0]=_ALPHA;
  matrix[2][1]=_BETA;

  rotA.SetMat(matrix);
}


void DynAlFitParameters::dumpToLinearSpace(SingleFitLinear &fit,int when,int id){
  fit.id=id;
  fit.time=when;

  double elapsed=GetTime(when,0);
  double dt=1;

#define CL(_x) fit._x=0
  CL(DX);     CL(DY);     CL(DZ);
  CL(THETA);  CL(ALPHA);  CL(BETA);
#undef CL
  
#define Do(_xx) fit._xx+=dt*_xx.at(i) 
  for(int i=0;i<DX.size();i++){
    Do(DX);
    Do(DY);
    Do(DZ);
    Do(THETA);
    Do(ALPHA);
    Do(BETA);
    dt*=elapsed;
  }
#undef Do

  fit.ZOffset=ZOffset;
  fit.TOffset=TOffset;
}


DynAlFitParameters::DynAlFitParameters(SingleFitLinear &fit){
#define Do(_xx) _xx.push_back(fit._xx);
    Do(DX);
    Do(DY);
    Do(DZ);
    Do(THETA);
    Do(ALPHA);
    Do(BETA);
#undef Do

  ZOffset=fit.ZOffset;
  TOffset=fit.TOffset;
}



ClassImp(DynAlFitContainer);



bool DynAlFitContainer::dumpToLinearSpace(LinearSpace &tdvBuffer,bool layer9){
  if(FitParameters.size()+LocalFitParameters.size()==0) return false;
  if(FitParameters.size()+LocalFitParameters.size()>LinearSpaceMaxRecords){
    cout<<"DynAlFitContainer::dumpToLinearSpace--E-To many records. No dump."<<endl;
    return false;
  }
  tdvBuffer.id=INT_MAX;
  tdvBuffer.records=0;

  // Dump local alignment (if any)
  for(map<int,DynAlFitParameters>::iterator i=LocalFitParameters.begin();
      i!=LocalFitParameters.end();i++, tdvBuffer.records++){
    int id=i->first;
    DynAlFitParameters &current=i->second;
    if(!current.DX.size()) continue;
    current.dumpToLinearSpace(tdvBuffer.Alignment[tdvBuffer.records][layer9?1:0],0,id);
  }

  // Dump alignment parameters
  for(map<int,DynAlFitParameters>::iterator i=FitParameters.begin();
      i!=FitParameters.end();i++, tdvBuffer.records++){
    int when=i->first;
    if(when<tdvBuffer.id) tdvBuffer.id=when;
    DynAlFitParameters &current=i->second;
    if(!current.DX.size()) continue;
    current.dumpToLinearSpace(tdvBuffer.Alignment[tdvBuffer.records][layer9?1:0],when,-1);
  }
  return true;
}

DynAlFitContainer::DynAlFitContainer(LinearSpace &tdvBuffer,bool layer9){
  int records=0;
  for(int i=0;i<tdvBuffer.records;i++){
    // Skip local alignment by the moment
    if(tdvBuffer.Alignment[i][layer9?1:0].id!=-1) continue;
    FitParameters[tdvBuffer.Alignment[i][layer9?1:0].time]=DynAlFitParameters(tdvBuffer.Alignment[i][layer9?1:0]);
    records++;
  }
  //  cout<<"DynAlFitContainer::DynAlFitContainer--Got "<<records<<" for layer "<<(layer9?9:1)<<endl; 
}


void DynAlFitContainer::TestDump(){
  // First dump to the tdvspace and retrieve it again
  dumpToLinearSpace(DynAlManager::tdvBuffer,false);
  dumpToLinearSpace(DynAlManager::tdvBuffer,true);

  DynAlFitContainer layer9(DynAlManager::tdvBuffer,true);
  DynAlFitContainer layer1(DynAlManager::tdvBuffer,false);

  // Loop of them and check that are OK
  cout<<"CURRENT NUMBER OF RECORDS "<<FitParameters.size()<<endl
      <<"              RETRIEVED 1 "<<layer1.FitParameters.size()<<endl
      <<"              RETRIEVED 9 "<<layer9.FitParameters.size()<<endl;


  for(map<int,DynAlFitParameters>::iterator i=FitParameters.begin();i!=FitParameters.end();i++){
    cout<<"ZOffset "<<i->second.ZOffset<<" "<<layer1.FitParameters[i->first].ZOffset<<" "<<layer9.FitParameters[i->first].ZOffset<<endl;
    cout<<"TOffset "<<i->second.TOffset<<" "<<layer1.FitParameters[i->first].TOffset<<" "<<layer9.FitParameters[i->first].TOffset<<endl;
#define Do(xx) for(int j=0;j<i->second.xx.size();j++) cout<<#xx<<" "<<j<<" "<<i->second.xx[j]<<" "<<layer1.FitParameters[i->first].xx[j]<<" "<<layer9.FitParameters[i->first].xx[j]<<endl;
    Do(DX);
    Do(DY);
    Do(DZ);
    Do(THETA);
    Do(ALPHA);
    Do(BETA);
  }
#undef Do
}


int DynAlFitContainer::GetId(TrRecHitR &hit){
#ifdef _PGTRACK_
  if(hit.GetLayerJ()!=Layer){
    cout<<"DynAlFitContainer::GetId-W-Requested Id for layer "<<hit.lay()<<" whereas fits are for layer "<<Layer<<endl;
  }
  return hit.GetLayerJ()+10*hit.GetSlotSide()+100*hit.lad();
#else
  if(hit.lay()!=Layer){
    cout<<"DynAlFitContainer::GetId-W-Requested Id for layer "<<hit.lay()<<" whereas fits are for layer "<<Layer<<endl;
  }
  return hit.lay()+10*hit.half()+100*hit.lad();
#endif
}


bool DynAlFitContainer::Find(int seconds,DynAlFitParameters &fit){
  map<int,DynAlFitParameters>::iterator lower=FitParameters.lower_bound(seconds);
  if(lower==FitParameters.end()){
    lower=FitParameters.upper_bound(seconds);
    if(lower==FitParameters.end()){
      // Not possible to find anything -- return
      cout<<"DynAlFitContainer::Eval-W-Not element for time "<<seconds<<endl;
      return false;
    }
  }
  
  if(abs(lower->first-seconds)>30) return false; // Not found
  fit=lower->second;
  return true;
}



void DynAlFitContainer::Eval(DynAlEvent &ev,double &x,double &y,double &z){
  int seconds=ev.Time[0]; 
  int museconds=ev.Time[1]; 
  int Id=GetId(ev);
  // Find the candidate
  map<int,DynAlFitParameters>::iterator lower=FitParameters.lower_bound(seconds);
  if(lower==FitParameters.end()){
    lower=FitParameters.upper_bound(seconds);
    if(lower==FitParameters.end()){
      // Not possible to find anything -- return
      cout<<"DynAlFitContainer::Eval-W-Not element for time "<<seconds<<endl;
      return;
    }
  }

  DynAlFitParameters &fit=lower->second;
  x=ev.RawHit[0];y=ev.RawHit[1];z=ev.RawHit[2];
  // If Local alignment needs to be applied, do it
  if(ApplyLocalAlignment){
    map<int,DynAlFitParameters>::iterator  which=LocalFitParameters.find(Id);
    if(which!=LocalFitParameters.end()){
      which->second.ApplyAlignment(x,y,z);
    }else{
      cout<<"DynAlFitContainer::Eval-W-Local Fit parameters requested but not found"<<endl;
    }
  }

  fit.ApplyAlignment(seconds,museconds,x,y,z);
}

void DynAlFitContainer::Eval(AMSEventR &ev,TrRecHitR &hit,double &x,double &y,double &z){
  // Get the time
  int seconds=ev.fHeader.Time[0]; 
  int museconds=ev.fHeader.Time[1]; 

  // Get the coordinates
  AMSPoint pnt;
#ifdef _PGTRACK_
  int imult=hit.GetResolvedMultiplicity();
  //  pnt=hit.GetGlobalCoordinate(imult,"");
  pnt=hit.GetGlobalCoordinate(imult,"A");
#else
  for(int i=0;i<3;i++) pnt[i]=hit.Hit[i];
#endif

  x=pnt[0];
  y=pnt[1];
  z=pnt[2];

  // Get the Id 
  int Id=GetId(hit);

  // Find the candidate
  map<int,DynAlFitParameters>::iterator lower=FitParameters.lower_bound(seconds);
  if(lower==FitParameters.end()){
    lower=FitParameters.upper_bound(seconds);
    if(lower==FitParameters.end()){
      // Not possible to find anything -- return
      cout<<"DynAlFitContainer::Eval-W-Not element for time "<<seconds<<endl;
      return;
    }
  }

  DynAlFitParameters &fit=lower->second;

  // If Local alignment needs to be applied, do it
  if(ApplyLocalAlignment){
    map<int,DynAlFitParameters>::iterator  which=LocalFitParameters.find(Id);
    if(which!=LocalFitParameters.end()){
      which->second.ApplyAlignment(x,y,z);
    }else{
      cout<<"DynAlFitContainer::Eval-W-Local Fit parameters requested but not found"<<endl;
    }
  }

#ifdef VERBOSE__
  cout<<"EVALUATING "<<seconds<<" "<<museconds<<" for "<<Id<<endl;
  cout<<"DUMPING PARAMETERS "<<endl;
#define dump(_x)   for(int i=0;i<_x.size();i++) cout<<_x[i]<<" ";cout<<endl;
  dump(fit.DX);
  dump(fit.DY);
  dump(fit.DZ);
  dump(fit.THETA);
  dump(fit.ALPHA);
  dump(fit.BETA);
#undef dump
#endif

  fit.ApplyAlignment(seconds,museconds,x,y,z);
}

void DynAlFitContainer::BuildLocalAlignment(DynAlHistory &history){
  // Compute the local alignment nfirst
  DynAlFit fit(DynAlContinuity::FitOrder);
  fit.MinRigidity=DynAlContinuity::RigidityCut;
  fit.MinBeta=DynAlContinuity::BetaCut;
  int minutes=DynAlContinuity::FitWindow;
  sort(history.Events.begin(),history.Events.end());
  map<int,DynAlHistory> historyPerLadder;
  Layer=history.Layer;
  for(int point=0;point<history.Size();point++){
    // Take the time of the event
    DynAlEvent event=history.Get(point);
    //    if(event.lay()!=Layer) continue;
    event.extrapolateTrack();
    int Id=GetId(event);
    int current=history.FindCloser(event.Time[0],event.Time[1],fit.First);
    
    // Find the time window
    int first,last;
    history.FindTimeWindow(current,minutes*30,first,last);
    
    // Fit
    fit.DoFit(history,first,last,event);
    if(fit.Fail) continue;


    fit.RetrieveFitPar(event.Time[0],event.Time[1]);
#define ESC(_x,_y) (_x[0]*_y[0]+_x[1]*_y[1]+_x[2]*_y[2])

    // Derotate the track vector 
    double trVect[3];
    trVect[0]=sin(event.TrackTheta)*cos(event.TrackPhi);
    trVect[1]=sin(event.TrackTheta)*sin(event.TrackPhi);
    trVect[2]=cos(event.TrackTheta);
    double trNewVect[3];
    trNewVect[0]=trVect[0]+fit.THETA*trVect[1]+fit.ALPHA*trVect[2];
    trNewVect[1]=trVect[1]-fit.THETA*trVect[1]+fit.BETA*trVect[2];
    trNewVect[2]=trVect[2]-fit.ALPHA*trVect[0]-fit.BETA*trVect[1];

    // Normalize to compensate the approximations of the fit
    double norma=sqrt(ESC(trNewVect,trNewVect));
    for(int i=0;i<3;i++) trNewVect[i]/=norma;

    // InvTransform the position
    double zTr=event.TrackHit[2]-fit.ZOffset;
    double trNewPos[3];
    trNewPos[0]=(event.TrackHit[0]-fit.DX)+fit.THETA*(event.TrackHit[1]-fit.DY)+fit.ALPHA*(zTr-fit.DZ);
    trNewPos[1]=(event.TrackHit[1]-fit.DY)-fit.THETA*(event.TrackHit[0]-fit.DX)+fit.BETA*(zTr-fit.DZ);
    trNewPos[2]=(zTr-fit.DZ)-fit.ALPHA*(event.TrackHit[0]-fit.DX)-fit.BETA*(event.TrackHit[1]-fit.DY);
    trNewPos[2]+=fit.ZOffset;

    DynAlEvent originalPoint=event;
    for(int i=0;i<3;i++)  originalPoint.TrackHit[i]=trNewPos[i];
    originalPoint.TrackTheta=acos(trNewVect[2]);
    originalPoint.TrackPhi=atan2(trNewVect[1],trNewVect[0]);
    originalPoint.extrapolateTrack();
    historyPerLadder[Id].Push(originalPoint);
  }

  // Perform all the local alignment fits and store them
  for(map<int,DynAlHistory>::iterator i=historyPerLadder.begin();i!=historyPerLadder.end();i++){
    DynAlFit ladFit(0);
    set<int> excluded;
    ladFit.MinRigidity=DynAlContinuity::RigidityCut;
    ladFit.MinBeta=DynAlContinuity::BetaCut;
    if(ladFit.ForceFit(i->second,0,i->second.Size()-1,excluded)){
      LocalFitParameters[i->first]=DynAlFitParameters(ladFit);
    }
  }
  ApplyLocalAlignment=true;
}


void DynAlFitContainer::DumpLocalAlignment(){
  for(map<int,DynAlFitParameters>::iterator i=LocalFitParameters.begin();
      i!=LocalFitParameters.end();i++){
    cout<<"Layer "<<Layer<<" ID "<<i->first<<endl;
    DynAlFitParameters &pars=i->second;
    cout<<"       DX "<<pars.DX[0]<<endl
	<<"       DY "<<pars.DY[0]<<endl
	<<"       DZ "<<pars.DZ[0]<<endl
	<<"       THETA "<<pars.THETA[0]<<endl
	<<"       ALPHA "<<pars.THETA[0]<<endl
	<<"       BETA "<<pars.THETA[0]<<endl
	<<"  ZOffset "<<pars.ZOffset<<" size "<<pars.DX.size()<<endl<<endl;
  }
}

//#define VERBOSE__
void DynAlFitContainer::BuildAlignment(TString dir,TString prefix,int run){
  // Use the continuity tool to get the history
  FitParameters.clear();
  DynAlContinuity historyC(dir,prefix,run);
  Layer=historyC.Layer;

  // Copy it 
  DynAlHistory history;
  for(int i=0;i<historyC.History.Size();i++){
    DynAlEvent ev=historyC.History.Get(i);
    if(ApplyLocalAlignment){
      int Id=GetId(ev);
      if(LocalFitParameters.find(Id)!=LocalFitParameters.end()){
	double x[3];for(int i=0;i<3;i++) x[i]=ev.RawHit[i];
	LocalFitParameters[Id].ApplyAlignment(x[0],x[1],x[2]);
	for(int i=0;i<3;i++) ev.RawHit[i]=x[i];
      }
    }
    ev.extrapolateTrack();
    history.Push(ev);
  }

  // Default fit parameters
  DynAlFit fit(DynAlContinuity::FitOrder);
  int minutes=DynAlContinuity::FitWindow;
  fit.MinRigidity=DynAlContinuity::RigidityCut;
  fit.MinBeta=DynAlContinuity::BetaCut;
  int prev_second=0;   // Keep track of every second already fit

  //  for(int point=0;point<history.Size();point++){
  // Loop only on events in current run
  for(int point=historyC.FirstOfCurrentRun;point<=historyC.LastOfCurrentRun;point++){
    // Take the time of the event
    DynAlEvent ev=history.Get(point);
    int Id=GetId(ev);

    if(ev.Time[0]==prev_second) continue;
    ev.Time[1]=500000;
    int current=history.FindCloser(ev.Time[0],ev.Time[1],fit.First);

    // Find the time window
    int first,last;
    history.FindTimeWindow(current,minutes*30,first,last);

    // Fit
    fit.DoFit(history,first,last,ev);
    
    if(fit.Fail) continue;
    prev_second=ev.Time[0];

    // Retrieve the fit parameters and store them
#ifdef VERBOSE__
    cout<<"STORING FIT FOR TIME "<<ev.Time[0]<<endl;
#endif
    FitParameters[ev.Time[0]]=DynAlFitParameters(fit);    
  }
#ifdef VERBOSE__
#undef VERBOSE__
#endif
}

/////////////////// Finally the tools to provide the alignment as a function of time
ClassImp(DynAlManager);

#include "commonsi.h"
#include "timeid.h"

//map<int,DynAlFitContainer> DynAlManager::dynAlFitContainers;
DynAlFitContainer DynAlManager::dynAlFitContainers[10];
int DynAlManager::currentRun=-1;
int DynAlManager::skipRun=-1;
TString DynAlManager::defaultDir="";
DynAlFitContainer::LinearSpace DynAlManager::tdvBuffer;
AMSTimeID* DynAlManager::tdvdb=0;
bool DynAlManager::useTDV=false;  // Set to tru to use external TDV
unsigned int DynAlManager::begin=0;
unsigned int DynAlManager::end=0;
unsigned int DynAlManager::insert=0;
int DynAlManager::ControlGroup=-1;

#ifdef _PGTRACK_
#define TDVNAME "DynAlignmentPG"
#else
#define TDVNAME "DynAlignment"
#endif

void _ToAlign(){
  DynAlManager::dynAlFitContainers[1]=DynAlFitContainer(DynAlManager::tdvBuffer,false);
  DynAlManager::dynAlFitContainers[9]=DynAlFitContainer(DynAlManager::tdvBuffer,true);
  DynAlManager::dynAlFitContainers[1].Layer=1;
  DynAlManager::dynAlFitContainers[9].Layer=9;
  DynAlManager::dynAlFitContainers[1].ApplyLocalAlignment=false; // Default value. To be changed in the future
  DynAlManager::dynAlFitContainers[9].ApplyLocalAlignment=false; // Default value. To be changed in the future
}

bool DynAlManager::FinishLinear(){
  if(tdvBuffer.records==0) return true;
  // Get the time range
  int minTime=INT_MAX;
  int maxTime=0;
  for(int which=0;which<2;which++)
    for(int i=0;i<tdvBuffer.records;i++){
      int t=tdvBuffer.Alignment[i][which].time;
      if(t<minTime) minTime=t;
      if(t>maxTime) maxTime=t;
    }
  

  // Stablish the validity time
  if(tdvdb) delete tdvdb;
  tm begin;
  tm end;

  begin.tm_isdst=0;
  end.tm_isdst=0;
  begin.tm_sec  =0;
  begin.tm_min  =0;
  begin.tm_hour =0;
  begin.tm_mday =0;
  begin.tm_mon  =0;
  begin.tm_year =0;
  
  end.tm_sec=0;
  end.tm_min=0;
  end.tm_hour=0;
  end.tm_mday=0;
  end.tm_mon=0;
  end.tm_year=0;
  
  tdvdb=new AMSTimeID(AMSID(TDVNAME,1),begin,end,sizeof(tdvBuffer),&tdvBuffer,
		      AMSTimeID::Standalone,1,_ToAlign);

  if(!tdvdb) return false;

  time_t insert;
  time(&insert);
  tdvdb->SetTime(insert,minTime,maxTime+1);
  tdvdb->UpdCRC();
  // Write it
  //  tdvdb->write(AMSDBc::amsdatabase,1);
  tdvdb->write("DataBase/",1);

  time_t in,be,en;
  tdvdb->gettime( in,  be, en);
  printf(" Begin : %s ",ctime(&be));
  printf(" End   : %s ",ctime(&en));
  printf(" Insert: %s ",ctime(&in));  

  tdvBuffer.records=0;
  return true;
}


bool  DynAlManager::AddToLinear(int time,DynAlFitParameters &layer1,DynAlFitParameters &layer9){
  tdvBuffer.id=INT_MAX;
  layer1.dumpToLinearSpace(tdvBuffer.Alignment[tdvBuffer.records][0],time,-1);
  layer9.dumpToLinearSpace(tdvBuffer.Alignment[tdvBuffer.records][1],time,-1);
  tdvBuffer.records++;
  if(tdvBuffer.records==LinearSpaceMaxRecords) FinishLinear();
  return true;
}


bool DynAlManager::UpdateWithTDV(int time){
  if(!tdvdb){
    tm begin;
    tm end;
    
    begin.tm_isdst=0;
    end.tm_isdst=0;
    begin.tm_sec  =0;
    begin.tm_min  =0;
    begin.tm_hour =0;
    begin.tm_mday =0;
    begin.tm_mon  =0;
    begin.tm_year =0;
    
    end.tm_sec=0;
    end.tm_min=0;
    end.tm_hour=0;
    end.tm_mday=0;
    end.tm_mon=0;
    end.tm_year=0;
    
    tdvdb=new AMSTimeID(AMSID(TDVNAME,1),begin,end,sizeof(tdvBuffer),&tdvBuffer,
			AMSTimeID::Standalone,1,_ToAlign);

    
    if(!tdvdb){
      cout<<"DynAlManager::UpdateWithTDV-- AMSTimeId cannot be created"<<endl;
      return false;
    }
  }
  
  time_t Time=time;
  if(!tdvdb->validate(Time)) return false;

  return true;
}


bool DynAlManager::DumpDirToLinear(TString dir){
  // Get the file list
  void *dirp=gSystem->OpenDirectory(dir);
  if(!dirp) return false;
  ResetLinear();
  int counter=0;
  vector<TString> files;
  for(const char *name=gSystem->GetDirEntry(dirp);name!=0;name=gSystem->GetDirEntry(dirp)){
    counter++;
    TString entry(name);
    if(!entry.Contains(".align.root")) continue;
    files.push_back(entry);
  }
  sort(files.begin(),files.end());

  for(int ii=0;ii<files.size();ii++){
    TString &entry=files[ii];
    const char *name=entry.Data();
    // Open the file
    TFile file(Form("%s/%s",dir.Data(),name));
    DynAlFitContainer *l1=(DynAlFitContainer*)file.Get("layer_1");
    DynAlFitContainer *l9=(DynAlFitContainer*)file.Get("layer_9");

    if(!l1 || !l9){
      cout<<"ERROR-- file "<<Form("%s/%s",dir.Data(),name)<<" is truncated"<<endl;
      continue;
    }

    // Get the time range
    int minkey=INT_MAX;
    int maxkey=0;
    for(map<int,DynAlFitParameters>::iterator i=l1->FitParameters.begin();
	i!=l1->FitParameters.end();i++){
      long key=i->first;
      if(key<minkey) minkey=key;
      if(key>maxkey) maxkey=key;
    }
    //    cout<<"TIME RANGE FOR L1 "<<minkey<<" TO "<<maxkey<<" "<<(maxkey-minkey)/60<<" minutes"<<endl;

    minkey=INT_MAX;
    maxkey=0;
    for(map<int,DynAlFitParameters>::iterator i=l9->FitParameters.begin();
	i!=l9->FitParameters.end();i++){
      long key=i->first;
      if(key<minkey) minkey=key;
      if(key>maxkey) maxkey=key;
    }
    //    cout<<"TIME RANGE FOR L9 "<<minkey<<" TO "<<maxkey<<" "<<(maxkey-minkey)/60<<" minutes"<<endl;

    if(maxkey)
      cout<<"TIME RANGE "<<minkey<<" TO "<<maxkey<<" "<<(maxkey-minkey)/60<<" minutes"<<endl;
    else
      cout<<"Problem for "<<name<<endl;

    for(int key=minkey;key<=maxkey;key++){
      // Get the parameters
      DynAlFitParameters l1P;
      DynAlFitParameters l9P;
      if(!l1->Find(key,l1P)) continue;
      if(!l9->Find(key,l9P)) continue;

      // Dump to linear space
      DynAlManager::AddToLinear(key,l1P,l9P);
    }
    
    delete l1;
    delete l9;
  }

  if(tdvBuffer.records) DynAlManager::FinishLinear();
  return true;
}



bool DynAlManager::UpdateParameters(int run,int time,TString dir){
  // If directory name is empty, use the default
  if(dir.Length()==0) dir=defaultDir;
  

#define TDVUPDATE     {                                   \  
  if(UpdateWithTDV(time)){				  \
    dynAlFitContainers[1].Layer=1;			  \
    dynAlFitContainers[9].Layer=9;			  \
    return true;					  \
  }else return false;                                     \
}

  if(dir.Length()==0){
    if(useTDV) TDVUPDATE
    else{
      // Use the information if AMSSetup
      AMSSetupR *setup=AMSSetupR::gethead();
      if(!setup) TDVUPDATE;
      // Search for the proper TDV
      AMSSetupR::TDVR tdv;
      if(setup->getTDV (TDVNAME,time, tdv)) TDVUPDATE;
      
      // Check that we have to copy out everything again
      if(tdv.Begin!=begin || tdv.End!=end || tdv.Insert!=insert){
	begin=tdv.Begin;
	end=tdv.End;
	insert=tdv.Insert;
	tdv.CopyOut(&tdvBuffer);
	_ToAlign();
      }
      return true;
    }
  }
  
  if(run==skipRun) return false;

  if(run!=currentRun){
    currentRun=run;


    // If the directory name is not empty go ahead
    if(dir.Length()){
      // Update the map
      bool suc=true;
      {
	
#pragma omp critical 
	{
	  //      cout <<" start "<<currentRun<<endl;
	  int subdir=DynAlContinuity::getBin(currentRun);
	  TFile *file= new TFile(Form("%s/%i/%i.align.root",dir.Data(),subdir,currentRun));
	  if(file){
	    DynAlFitContainer *l1=(DynAlFitContainer*)file->Get("layer_1");
	    DynAlFitContainer *l9=(DynAlFitContainer*)file->Get("layer_9");
	    if(!l1 || !l9){
	      cout<<"DynAlFitContainer::UpdateParameters-W-Unable to get \"layer_1\" and \"layer_9\" objects from "<<(Form("%s/%i/%i",dir.Data(),subdir,currentRun))<<endl;
	      suc=false;
	    }
	    else{
	      if(time<=0){
		cout<<"DynAlFitContainer::UpdateParameters--M--Reading file "<<Form("%s/%i/%i.align.root",dir.Data(),subdir,currentRun)<<endl;
	      }
	      
	      dynAlFitContainers[1]=*l1;
	      dynAlFitContainers[9]=*l9;
	      dynAlFitContainers[1].Layer=1;
	      dynAlFitContainers[9].Layer=9;
	      delete l1;
	      delete l9;
	      file->Close();
	      delete file;
	    }
	  }
	}
      }
      if(!suc){
	skipRun=currentRun;
        return false; 
      }
    }
    else{
      // Look for the AMSSetup object and try to get the containers from it
      cout<<"IN DynAlManager::UpdateParameters. Trying to  update run "<<run<<" time "<<time<<endl;
      return false;
    }
  }    
return true;
}


bool DynAlManager::FindAlignment(int run,int time,int layer,float hit[3],float hitA[3],int Id,TString dir){
  for(int i=0;i<3;i++) hitA[i]=hit[i];

  if(!UpdateParameters(run,time,dir)) return false;
  if(layer!=1 && layer!=9){
    // By the moment we only deal with layer 1 and layer 9
    return false;
  }
  
  
  DynAlEvent event;
  event.Time[0]=time;
  event.Time[1]=0;
  for(int i=0;i<3;i++) event.RawHit[i]=hit[i];
  event.Id=abs(Id)%10000;

  double x=hit[0];
  double y=hit[1];
  double z=hit[2];

  bool use_local=dynAlFitContainers[layer].ApplyLocalAlignment;
  if(Id<0) dynAlFitContainers[layer].ApplyLocalAlignment=false; 
  dynAlFitContainers[layer].Eval(event,x,y,z);
  dynAlFitContainers[layer].ApplyLocalAlignment=use_local;
  hitA[0]=x; hitA[1]=y; hitA[2]=z;
  return true;
}


bool DynAlManager::FindAlignment(AMSEventR &ev,TrRecHitR &hit,double &x,double &y,double &z,TString dir){
  // Update the maps, if needed
  if(!UpdateParameters(ev.fHeader.Run,ev.fHeader.Time[0])) return false; 
  // Get the hit layer
#ifdef _PGTRACK_
  int layer=hit.GetLayerJ();
#else
  int layer=hit.lay();
#endif

  if(layer!=1 && layer!=9){
    // By the moment we only deal with layer 1 and layer 9
    return false;
  }

  dynAlFitContainers[layer].Eval(ev,hit,x,y,z);
  return true;
}

DynAlFitContainer DynAlManager::BuildLocalAlignment(DynAlHistory &history){
  DynAlFit fit(DynAlContinuity::FitOrder);
  fit.MinRigidity=DynAlContinuity::RigidityCut;
  fit.MinBeta=DynAlContinuity::BetaCut;
  int minutes=DynAlContinuity::FitWindow;
  sort(history.Events.begin(),history.Events.end());
  map<int,DynAlHistory> historyPerLadder;
  for(int point=0;point<history.Size();point++){
    // Take the time of the event
    DynAlEvent event=history.Get(point);
    if(!UpdateParameters(0,event.Time[0])) continue;
    event.extrapolateTrack();

    int layer=event.lay();
    int Id=DynAlFitContainer::GetId(event);
    
    // Find the fit parameters 
    DynAlFitParameters fit;
    DynAlManager::dynAlFitContainers[layer].Find(event.Time[0],fit);

    double _DX=0;
    double _DY=0;
    double _DZ=0;
    double _THETA=0;
    double _ALPHA=0;
    double _BETA=0;
    
    double elapsed=fit.GetTime(event.Time[0],event.Time[1]);
    double dt=1;

#define Do(xx) _##xx+=dt* fit.xx.at(i) 
    for(int i=0;i<fit.DX.size();i++){
      Do(DX);
      Do(DY);
      Do(DZ);
      Do(THETA);
      Do(ALPHA);
      Do(BETA);
      dt*=elapsed;
    }
#undef Do

    // Derotate the track vector 
    double trVect[3];
    trVect[0]=sin(event.TrackTheta)*cos(event.TrackPhi);
    trVect[1]=sin(event.TrackTheta)*sin(event.TrackPhi);
    trVect[2]=cos(event.TrackTheta);
    double trNewVect[3];
    trNewVect[0]=trVect[0]+_THETA*trVect[1]+_ALPHA*trVect[2];
    trNewVect[1]=trVect[1]-_THETA*trVect[1]+_BETA*trVect[2];
    trNewVect[2]=trVect[2]-_ALPHA*trVect[0]-_BETA*trVect[1];

    // Normalize to compensate the approximations of the fit
    double norma=sqrt(ESC(trNewVect,trNewVect));
    for(int i=0;i<3;i++) trNewVect[i]/=norma;

    // InvTransform the position
    double zTr=event.TrackHit[2]-fit.ZOffset;
    double trNewPos[3];
    trNewPos[0]=(event.TrackHit[0]-_DX)+_THETA*(event.TrackHit[1]-_DY)+_ALPHA*(zTr-_DZ);
    trNewPos[1]=(event.TrackHit[1]-_DY)-_THETA*(event.TrackHit[0]-_DX)+_BETA*(zTr-_DZ);
    trNewPos[2]=(zTr-_DZ)-_ALPHA*(event.TrackHit[0]-_DX)-_BETA*(event.TrackHit[1]-_DY);
    trNewPos[2]+=fit.ZOffset;

    DynAlEvent originalPoint=event;
    for(int i=0;i<3;i++)  originalPoint.TrackHit[i]=trNewPos[i];
    originalPoint.TrackTheta=acos(trNewVect[2]);
    originalPoint.TrackPhi=atan2(trNewVect[1],trNewVect[0]);
    originalPoint.extrapolateTrack();
    historyPerLadder[Id].Push(originalPoint);
  }

  // Perform all the local alignment fits and store them
  DynAlFitContainer local;
  for(map<int,DynAlHistory>::iterator i=historyPerLadder.begin();i!=historyPerLadder.end();i++){
    DynAlFit ladFit(0);
    set<int> excluded;
    ladFit.MinRigidity=DynAlContinuity::RigidityCut;
    ladFit.MinBeta=DynAlContinuity::BetaCut;
    if(ladFit.ForceFit(i->second,0,i->second.Size()-1,excluded)){
      local.LocalFitParameters[i->first]=DynAlFitParameters(ladFit);
    }
  }
  local.ApplyLocalAlignment=true;
  return local;
}
