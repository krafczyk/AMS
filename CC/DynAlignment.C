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


bool DynAlEvent::buildEvent(AMSEventR &ev,int layer,DynAlEvent &event){
  ////////////////////////////////
  // Get the hit for the layer  //
  ////////////////////////////////

  if(ev.nParticle()!=1) return false;
  ParticleR &part=ev.Particle(0);
  if(part.iTrTrack()<0) return false;
  TrTrackR &track=ev.TrTrack(part.iTrTrack());
  TrRecHitR *hitp=0;

#ifdef _PGTRACK_
  hitp=track.GetHitLJ(layer);
#else
  int whichHit=-1;
  for(int hi=0;hi<track.NTrRecHit();hi++){
    TrRecHitR &h=ev.TrRecHit(track.iTrRecHit(hi));
    if(h.lay()==layer){hitp=&h;whichHit=hi;break;}
  }
#endif

  if(!hitp) return false;
  TrRecHitR &hit=*hitp;

  ///////////////////////////////////////////////////////////////////////
  // Extrapolate the track to the layer and dump the track information //
  ///////////////////////////////////////////////////////////////////////

  AMSDir dir;
  AMSPoint pnt;
#ifdef _PGTRACK_
  int id=track.iTrTrackPar(1,3,1);
  if(id<0) return false;
  const TrTrackPar &trPar=track.gTrTrackPar(id);
  track.InterpolateLayerJ(layer, pnt, dir, id) ;
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

  ////////////////////////////////////////////////////////
  // Pick the hit position without alignment correction //
  ////////////////////////////////////////////////////////
#ifdef _PGTRACK_
  int imult=hit.GetResolvedMultiplicity();
  pnt=hit.GetGlobalCoordinate(imult,"");
#else
  for(int i=0;i<3;i++) pnt[i]=hit.Hit[i];
#endif
  
  ///////////////////////////
  // Dump the hit position //
  //////////////////////////

  for(int i=0;i<3;i++) event.RawHit[i]=pnt[i];

  ///////////////////////////////
  // Dump the Time information //
  ///////////////////////////////

  event.Time[0]=ev.fHeader.Time[0];
  event.Time[1]=ev.fHeader.Time[1];

  ////////////////////// 
  // Dump the hit id  //
  //////////////////////
  
#ifdef _PGTRACK_
  event.Id=(hit.GetLayerJ()+10*hit.GetSlotSide()+100*hit.lad())*(hit.OnlyY()?-1:1);
#else
  event.Id=hit.Id;
  if(hit.Status&16384 || hit.Status&8192) event.Id*=-1;
#endif


  return true;  // Success 
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


//////////////////////////////////////////////////////////////////////////////

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

  /////////////////////////////////////////////////
  // exclude too far away hit
  for(int i=first;i<=last;i++){
    if(fabs(history.Events[i].RawHit[0]-history.Events[i].TrackHit[0])>1 ||
       fabs(history.Events[i].RawHit[e]-history.Events[i].TrackHit[1])>1) excluded.insert(i);
    if(history.Events[i].Id<0) excluded.insert(i);
  }



  /////////////////////////////////////////////
  // Exclude outliers
 
  double sum[2]={0,0};
  double sum2[2]={0,0};
  int totalCount=0;
  for(int i=first;i<=last;i++){
    if(excluded.count(i)) continue;
    for(int j=0;j<2;j++){
      sum[j]+=history.Events.at(i).RawHit[j];
      sum2[j]+=history.Events.at(i).RawHit[j]*history.Events.at(i).RawHit[j];
    }
    totalCount++;
  }

  for(int j=0;j<2;j++) {sum[j]/=totalCount;sum2[j]/=totalCount;sum2[j]-=sum[j]*sum[j];}
  
  for(int i=first;i<=last;i++){
    if(excluded.count(i)) continue;
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

/*

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


bool Continuity::buildEvent(AMSEventR &ev,int layer,DynAlEvent &event){
  ////////////////////////////////
  // Get the hit for the layer  //
  ////////////////////////////////

  if(ev.nParticle()!=1) return false;
  ParticleR &part=ev.Particle(0);
  if(part.iTrTrack()<0) return false;
  TrTrackR &track=ev.TrTrack(part.iTrTrack());
  TrRecHitR *hit=0;

#ifdef _PGTRACK_
  hitp=track.GetHitLJ(layer);
#else
  int whichHit=-1;
  for(int hi=0;hi<track.NTrRecHit();hi++){
    TrRecHitR &h=ev.TrRecHit(track.iTrRecHit(hi));
    if(h.lay()==layer){hitp=&h;whichHit=hi;break;}
  }
#endif

  if(!hitp) return false;
  TrRecHitR &hit=*hitp;

  ///////////////////////////////////////////////////////////////////////
  // Extrapolate the track to the layer and dump the track information //
  ///////////////////////////////////////////////////////////////////////

  AMSDir dir;
  AMSPoint pnt;
#ifdef _PGTRACK_
  int id=track.iTrTrackPar(1,3,1);
  if(id<0) return false;
  const TrTrackPar &trPar=track.gTrTrackPar(id);
  track.InterpolateLayerJ(layer, pnt, dir, id) ;
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

  ////////////////////////////////////////////////////////
  // Pick the hit position without alignment correction //
  ////////////////////////////////////////////////////////
#ifdef _PGTRACK_
  int imult=hit.GetResolvedMultiplicity();
  pnt=hit.GetGlobalCoordinate(imult,"");
#else
  for(int i=0;i<3;i++) pnt[i]=hit.Coo[i];
#endif
  
  ///////////////////////////
  // Dump the hit position //
  //////////////////////////

  for(int i=0;i<3;i++) event.RawHit[i]=pnt[i];

  ///////////////////////////////
  // Dump the Time information //
  ///////////////////////////////

  event.Time[0]=ev.fHeader.Time[0];
  event.Time[1]=ev.fHeader.Time[1];

  ////////////////////// 
  // Dump the hit id  //
  //////////////////////
  
#ifdef _PGTRACK_
  event.Id=(hit.GetLayerJ()+10*hit.GetSlotSide()+100*hit.lad())*(hit.OnlyY()?-1:1);
#else
  event.Id=hit.Id;
  if(hit.Status&16384 || hit.Status&8192) event.Id*=-1;
#endif


  return true;  // Success 
}




int Continuity::select(AMSEventR *ev,int layer){
#define SELECT(_name,_condition) {if(!(_condition)) return false;}
#define fStatus (ev->fStatus)  

  SELECT("1 Tracker+RICH particle",(fStatus&0x33)==0x31);
  SELECT("At most 1 anti",((fStatus>>21)&0x3)<=1);
  SELECT("At most 4 tof clusters",(fStatus&(0x7<<10))<=(0x4<<10));
  SELECT("At least 1 tr track",fStatus&(0x3<<13));
  SELECT("At most 1 crossing particle at rich",ev->pParticle(0)->RichParticles<=1);
  SELECT("At least 3 out of 4 for beta",Particle(0).pBeta()->Pattern<=4);
  SELECT("TOF beta>0.9",Particle(0).pBeta()->Beta>0.9);  
  SELECT("Rich Beta>0.99",Particle(0).pRichRing && Particle(0).pRichRing->IsGood() && Particle(0).pRichRing->Beta>0.99);

  bool layerUsed[10];
  for(int l=0;l<10;l++) layerUsed[l]=false;
  TrTrackR &track=*(ev->Particle(0).pTrTrack());
  for(int hi=0;hi<track.NTrRecHit();hi++){
    TrRecHitR &hit=*(track.pTrRecHit(hi));
    layerUsed[hit.lay()]=true;
  }

  SELECT("Has the right external layer",layerUsed(layer));
  SELECT("The inner tracker has layer 2 and plane(7,8) and other inner hit",layerUsed[2] && (layerUsed[7] || layerUsed[8]) && (layerUsed[3] || layerUsed[4] || layerUsed[5] || layerUsed[6]));

  return true;
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
*/
