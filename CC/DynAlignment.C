#include "DynAlignment.h"
#include "TChainElement.h"

#include <iostream>
#include <algorithm>
#include "math.h"
#include "limits.h"
#include "stdlib.h"

//#define VERBOSE

ClassImp(DynAlEvent);

void DynAlEvent::extrapolateTrack(){
  float dz=RawHit[2]-TrackHit[2];
  float dx=dz*tan(TrackTheta)*cos(TrackPhi);
  float dy=dz*tan(TrackTheta)*sin(TrackPhi);
  TrackHit[0]+=dx;
  TrackHit[1]+=dy;
  TrackHit[2]+=dz;
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
#ifdef VERBOSE
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


ClassImp(DynAlFit);

DynAlFit::DynAlFit(int order){
  LadderFit=false;ZOffset=-136.0;First=Last=Excluded=-1;
  MinRigidity=0;MaxRigidity=1e6;Events=0;
  Order=order;
  UseLocalAlignment=false;
  ControlGroup=-1;
  Init();}

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
#ifdef VERBOSE
    cout<<"EXCLUDING "<<history.Events[i].Time[0]-exclude.Time[0]<<endl;
#endif
    excluded.insert(i);
  }


  /////////////////////////////////////////////
  // Exclude outliers
  // Compute ZOffset 
  double sum[2]={0,0};
  double sum2[2]={0,0};
  ZOffset=0;
  for(int i=first;i<=last;i++){
    for(int j=0;j<2;j++){
      sum[j]+=history.Events.at(i).RawHit[j];
      sum2[j]+=history.Events.at(i).RawHit[j]*history.Events.at(i).RawHit[j];
    }
    ZOffset+=history.Events.at(i).RawHit[2];
  }

  for(int j=0;j<2;j++) {sum[j]/=total;sum2[j]/=total;sum2[j]-=sum[j]*sum[j];}
  ZOffset/=total;
  
  for(int i=first;i<=last;i++){
    double delta[2];
    const double threshold=5*5;
    for(int j=0;j<2;j++) {delta[j]=history.Events.at(i).RawHit[j]-sum[j];}
    if(delta[0]*delta[0]>threshold*sum2[0] || delta[1]*delta[1]>threshold*sum2[1]) excluded.insert(i); 
  }
  ///////////////////////////////////////////////////////////////////////////////


  if(LadderFit){
    // The user wants to exclude hits in a layer different of that of "exclude"... therefore we exclude those
    int ladder=exclude.lad();
    int half=exclude.half();
    int layer=exclude.lay();
    for(int i=first;i<=last;i++){
      DynAlEvent &ev=history.Events.at(i);
      if(ladder!=ev.lad() || layer!=ev.lay() || half!=ev.half()) excluded.insert(i);
    }
  }

  return ForceFit(history,first,last,excluded);
}


void DynAlFit::ApplyLocalAlignment(int myid,float &x,float &y,float &z){
  double mx=x;
  double my=y;
  double mz=z;
  ApplyLocalAlignment(myid,mx,my,mz);
  x=mx;
  y=my;
  z=mz;
}

void DynAlFit::ApplyLocalAlignment(int myid,double &x,double &y,double &z){
  if(LocalDX.find(myid)==LocalDX.end()){
    cout<<"************ LOCAL ALIGNMENT PARAMETERS FOR ID "<<myid<<" not found"<<endl;
  }
  double _DX=LocalDX[myid];
  double _DY=LocalDY[myid];
  double _DZ=LocalDZ[myid];
  double _THETA=LocalTHETA[myid];
  double _ALPHA=LocalALPHA[myid];
  double _BETA=LocalBETA[myid];
  
  // We assume z==0
  double dx=_DX-_THETA*y;
  double dy=_DY+_THETA*x;
  double dz=_DZ+_ALPHA*x+_BETA*y;
  
  x+=dx;
  y+=dy;
  z+=dz;
}

bool DynAlFit::ForceFit(DynAlHistory &history,int first,int last,set<int> &exclude){
  /* The function fit is

     X         DX        1     -theta  -alpha    X
     Y    =    DY  +    theta    1     -beta  *  Y 
     Z         DZ       alpha   beta     1       Z
 
     plus first ans second order time derivatives

     but we assume Z=0

   */

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

  // Get the reference time
  Events=0;
  for(int i=first;i<=last;i++){
    DynAlEvent event=history.Events.at(i); // Take a copy of the event
    if(fabs(event.Rigidity)<MinRigidity || fabs(event.Rigidity)>MaxRigidity) continue;  // Exclude using the rigidity criteria

    if(exclude.count(i)>0){
#ifdef VERBOSE
      cout<<"EXCLUDED --- "<<i<<endl;
#endif      
      continue;  // Exclude using the mask
    }

    // Exclude interleave events for a test
    if(ControlGroup>0) if((event.Time[0]%ControlGroup)==0) continue;

    // Apply inner alignment if required
    if(UseLocalAlignment){
      // Get the Id:
      int myid=event.Id%10000;
      ApplyLocalAlignment(myid,event.RawHit[0],event.RawHit[1],event.RawHit[2]);
    }
    event.extrapolateTrack(); // Just in case

    // Shift the Z origin
    double zHit=event.RawHit[2]-ZOffset;
    double zTr=event.TrackHit[2]-ZOffset;

    // Fit in Y axis
    float &xHit=event.RawHit[0];
    float &yHit=event.RawHit[1];
    float &xTr=event.TrackHit[0];
    float &yTr=event.TrackHit[1];
    
    time=(history.Events.at(i).Time[0]-TOffset)+1e-6*history.Events.at(i).Time[1];
    time/=60*90;  // Simplify a bit the computation

    double v;
    //////////////////////////////////////////////
    // First the x component
    y=xTr-xHit;   //ztr-zhit==0
    Dx=1;
    Dy=0;
    v=tan(event.TrackTheta)*cos(event.TrackPhi);
    Dz=               -v;
    theta=-yHit;
    alpha=-zHit  -xHit*v;
    beta=        -yHit*v;
    error=1;      
    Fit.AddPoint(x,y,error);

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
    error=1;
    Fit.AddPoint(x,y,error);

    // Increase the event counter
    Events++;
  }

  if(Fit.Eval()) return false;

  Fail=false;
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
  time/=60*90;  // Simplify a bit the computation

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


////////////////////// Continuity: here all the interfaces with AMSEvenR are in the function buildEvent
////////////////////// Moreover, all the utilities to keep all indexes in a file should be included here 

#define WHICHTREE "TrAlignment"
#define WHICHBRANCH "header.Run"

void Continuity::Init(TChain &chain){
  // Scan the chain to get run numbers
  Run2File.clear();
  Run2Runs.clear();
  vector<int> runs; runs.reserve(256);

  TObjArray *fileElements=chain.GetListOfFiles();
  TIter next(fileElements);
  TChainElement *chEl=0;
  while (( chEl=(TChainElement*)next() )) {
    TString filename=chEl->GetTitle();
    TFile f(filename);
    TTree *tree=(TTree*)f.Get(WHICHTREE);
    if(!tree){
      cout<<"TREE AMSRoot NOT FOUND IN "<<filename<<endl;
      continue;
    }

    tree->Draw(WHICHBRANCH,"","goff",1); // Dirty and fast way of getting the value 
    if(!tree->GetV1()) continue;
    int run=int(tree->GetV1()[0]);

    Run2File[run]=filename;
    runs.push_back(run);
  }

  sort(runs.begin(),runs.end());  // Sort the runs numbers, just in case

  // Associate the runs in range to current run
  for(int i=0;i<runs.size();i++){
    int begin=max(0,i-1);
    int end=min(int(runs.size())-1,i+1);
    Run2Runs[runs[i]].clear();
    Run2Runs[runs[i]].reserve(3);
    for(int j=begin;j<=end;j++) Run2Runs[runs[i]].push_back(runs[j]);
  }
}



void Continuity::SetRun(int run){
  if(run==CurrentRun) return;
  CurrentRun=run;
  // If this is a new run go for it
  if(Run2File.find(run)==Run2File.end()){
    cout<<"RUN "<<run<<" NOT PRESENT"<<endl;
  }

  cout<<"------------- CREATING A NEW HISTORY FOR RUN "<<run<<endl;
  if(!History){
    History=new DynAlHistory(run);
    History->Events.reserve(4096);
  }
  History->Events.clear();

  // Try to the the idx files. If not presetn try to build them
  vector<int> &runs=Run2Runs[run];

  for(int i=0;i<runs.size();i++){
    TFile *idxFile=FindCreateIdx(runs[i],Run2File[runs[i]]);
    if(!idxFile){
      cout<<"COULD NOT FIND OR CREATE IDX FILE FOR RUN "<<runs[i]<<endl;
      continue;
    }

    DynAlHistory *history=(DynAlHistory*)idxFile->Get(Prefix+TString("idx"));
    if(!history){
      cout<<"COULD NOT FIND HISTORY IN FILE "<<runs[i]<<endl;
      continue;
    }


    cout<<"     COPYING CONTENTS OF  run "<<runs[i]<<endl;
    // Copy the contents for the idxFile into the history
    for(int entry=0;entry<history->Events.size();entry++) {
      History->Push(history->Events[entry]);
    }

    // Done
    delete idxFile;
  }

  cout<<"DONE"<<endl;
}



void Continuity::CreateIdx(int run,TString fileName){
  // Create the history object
  DynAlHistory history(run);

  // Create the chain
  TChain ch("TrAlignment");
  ch.Add(fileName);

  // Loop in all the events
  long entries=ch.GetEntries();
  DstEvent *ev=0;
  ch.SetBranchAddress("DstEvent",&ev);

  for(long n=0;n<entries;n++){
    ch.GetEvent(n);
    int index=select(ev);
    if(index<0) continue;

    DynAlEvent event;
    buildEvent(*ev,index,event);
    event.extrapolateTrack();
    history.Push(event);
  }
  
  TFile file(Form("%u.idx.root",run),"UPDATE");
  history.Write(Prefix+TString("idx"));
  file.Close();

  cout<<"Wrote "<<history.Size()<<" events in "<<Form("%u.idx.root",run)<<endl;
}


void Continuity::buildEvent(DstEvent &ev,int index,DynAlEvent &event){
  const int plane=Plane;
  for(int j=0;j<3;j++){
    event.RawHit[j]=ev.get(plane,index).hit.Hit[j];
    event.TrackHit[j]=ev.get(plane,index).coo[j];
  }
  event.TrackTheta=ev.get(plane,index).theta;
  event.TrackPhi=ev.get(plane,index).phi;
  event.Rigidity=ev.get(plane,index).rigidity;
  event.Time[0]=ev.header.Time[0];
  event.Time[1]=ev.header.Time[1];
  event.Id=ev.get(plane,index).hit.Id;
}


void Continuity::findHits(DstEvent *ev,vector<int> &indexes,vector<double> &dist){
  const int plane=Plane;
  map<double,int> idx;
  dist.clear();
  indexes.clear();

  // Search the closer
  double best=INFINITY;
  int bestI=-1;

  for(int i=0;i<ev->getN(plane);i++){
    DynAlEvent event;
    buildEvent(*ev,i,event);
    event.extrapolateTrack();

    double d=0;
    double weight[2]={1.0,1.0};
    for(int j=0;j<2;j++) {double x=event.TrackHit[j]-event.RawHit[j];d+=x*x*weight[j]*weight[j];}
    d=sqrt(d);

    if(d<best){
      best=d;
      bestI=i;
    }
    //    cout<<"PLANE "<<plane<<" current "<<i<<" DIST "<<d<<" best "<<best<<" betsI "<<bestI<<endl;
  }

  if(bestI>-1){
    dist.push_back(best);
    indexes.push_back(bestI);
  }else return;

  best=INFINITY;
  bestI=-1;

  for(int i=0;i<ev->getN(plane);i++){
    if(i==indexes[0]) continue;
    DynAlEvent event;
    buildEvent(*ev,i,event);
    event.extrapolateTrack();
    
    double d=0;
    for(int j=0;j<2;j++) {double x=event.TrackHit[j]-event.RawHit[j];d+=x*x;}
    d=sqrt(d);
    if(d<best){
      best=d;
      bestI=i;
    }
  }

  if(bestI>-1){
    dist.push_back(best);
    indexes.push_back(bestI);
  }else return;

}


int Continuity::select(DstEvent *ev){
  const int plane=Plane;
  // Ensure layers 2 and 8 are in the fit
  if(ev->getN(2)==0 || !ev->get(2,0).hit.Used) return -1; // One hit used in the last layer of the inner tracker
#define USE(x) (ev->getN(x) && ev->get(x,0).hit.Used)
  if(!USE(7) && !USE(8)) return -1; // One hit used in the last layer of the inner tracker
  if(ev->getN(plane)==0 || ev->get(plane,0).hit.falseX()) return -1; // At least one hit in the layer 
  // Use rich for the rigidity selection
  //  if(ev->rich.Beta<0.99) return -1; // with rigidity cut at 5
  if(ev->rich.Beta<0.995) return -1; // with rigidity cut at 5
  if(1/fabs(ev->get(plane,0).rigidity)>0.5) return -1;  // This selects a true rigidity larger than 2 approximatly


  int counter=2;
  counter+=(USE(3) || USE(4))+(USE(5) || USE(6));
  if(counter<3) return -1;  // At least 3 (double side)planes
  
  // Find the closer hit
  vector<int> indexes;
  vector<double> dist;
  findHits(ev,indexes,dist);
  
  if(dist[0]>1) return -1; // Just in case
  //  if(dist.size()>1 && dist[1]<1) return -1;

  return indexes[0];
}


TFile *Continuity::FindCreateIdx(int run,TString file){
  TString filename=Form("%i.idx.root",run);
  
  TFile *idxFile=new TFile(filename);
  DynAlHistory *history=(DynAlHistory*)idxFile->Get(Prefix+TString("idx"));

  if(!history){
    cout<<"IDX object not found in file "<<filename<<". Trying to (re)create."<<endl;
    cout<<"CURRENT RUN "<<run<<" CURRENT FILE "<<file<<endl;
    CreateIdx(run,file);
    delete idxFile;
    idxFile=new TFile(filename);
    history=(DynAlHistory*)idxFile->Get(Prefix+TString("idx"));
    if(!history){
      cout<<"IDX object not found in file "<<filename<<"exiting."<<endl;
      exit(1);
    }
  }

  return idxFile;
}
