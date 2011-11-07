#include "DynAlignment.h"
#include "TChainElement.h"
#include "TSystem.h"

#include <iostream>
#include <algorithm>
#include "math.h"
#include "limits.h"
#include "stdlib.h"

//#define VERBOSE__

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

  ////////////////////////////////////////////////////////
  // Pick the hit position without alignment correction //
  ////////////////////////////////////////////////////////
  AMSPoint pnt;
  AMSDir dir;
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
  int id=track.iTrTrackPar(1,3,1);
  if(id<0) return false;
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

  event.Beta=part.pRichRing()?part.pRichRing()->getBeta():0;
  
  // Final cuts 
  if(event.Beta==0) return false;
  if(event.Rigidity<5) return false;

  return true;  // Success 
}

bool DynAlEvent::buildEvent(AMSEventR &ev,TrRecHitR &hit,DynAlEvent &event){
  ///////////////////////////////
  // Dump the Time information //
  ///////////////////////////////

  event.Time[0]=ev.fHeader.Time[0];
  event.Time[1]=ev.fHeader.Time[1];


  /////////////////////////////////////////
  // Get the hit position for the layer  //
  /////////////////////////////////////////

    AMSPoint pnt;
#ifdef _PGTRACK_
  int imult=hit.GetResolvedMultiplicity();
  pnt=hit.GetGlobalCoordinate(imult,"");
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

  return ForceFit(history,first,last,excluded);
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
    if(event.Beta<MinBeta || fabs(event.Rigidity)<MinRigidity || fabs(event.Rigidity)>MaxRigidity) continue;  // Exclude using the rigidity criteria
    
    if(exclude.count(i)>0){
#ifdef VERBOSE__
      cout<<"EXCLUDED --- "<<i<<endl;
#endif      
      continue;  // Exclude using the mask
    }

    // Exclude interleave events for a test
    if(ControlGroup>0) if((event.Time[0]%ControlGroup)==0) continue;
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


ClassImp(DynAlContinuity);
// Number of seconds in a week
#define magicNumber 605800
double DynAlContinuity::BetaCut=0.99;
double DynAlContinuity::RigidityCut=5;
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

  SELECT("1 Tracker+RICH particle",(fStatus&0x33)==0x31);
  SELECT("At most 1 anti",((fStatus>>21)&0x3)<=1);
  SELECT("At most 4 tof clusters",(fStatus&(0x7<<10))<=(0x4<<10));
  SELECT("At least 1 tr track",fStatus&(0x3<<13));
  SELECT("At most 1 crossing particle at rich",ev->pParticle(0)->RichParticles<=1);
  SELECT("At least 3 out of 4 for beta",ev->Particle(0).pBeta()->Pattern<=4);
  SELECT("TOF beta>0.9",ev->Particle(0).pBeta()->Beta>0.9);  
  SELECT("Has rich ring",ev->Particle(0).pRichRing());
  RichRingR *ring=ev->Particle(0).pRichRing();
  SELECT("Ring is good", ring->IsGood());
  //  SELECT("Beta>BetaCut",ring->Beta>BetaCut); // delayed 

  bool layerUsed[10];
  for(int l=0;l<10;l++) layerUsed[l]=false;
  TrTrackR &track=*(ev->Particle(0).pTrTrack());
  for(int hi=0;hi<track.NTrRecHit();hi++){
    TrRecHitR &hit=*(track.pTrRecHit(hi));
    layerUsed[hit.lay()]=true;
  }

  SELECT("Has the right external layer",layerUsed[layer]);
  SELECT("The inner tracker has layer 2 and plane(7,8) and other inner hit",layerUsed[2] && (layerUsed[7] || layerUsed[8]) && (layerUsed[3] || layerUsed[4] || layerUsed[5] || layerUsed[6]));

  // Finally use the implicit cuts in DynAlEvent::buildEvent
  DynAlEvent event;
  if(!DynAlEvent::buildEvent(*ev,layer,event)) return false;
  if(event.Id<0) return false;
  event.extrapolateTrack();
  if(fabs(event.RawHit[0]-event.TrackHit[0])>1 ||
     fabs(event.RawHit[1]-event.TrackHit[1])>1) return false;

  return true;
#undef SELECT
#undef fStatus
}


#ifdef __ROOTSHAREDLIBRARY__
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
    Fill(dir,prefix,runs.at(position));
    if(position+1<runs.size()) Fill(dir,prefix,runs.at(position+1));
    return;
  }

  if(position==0){
    TString mydir=Form("%s/%i/",dir_name.Data(),getPreviousBin(CurrentRun));
    // Get the list of files in current directory
    vector<int> list;
    GetFileList(mydir,list);
    if(list.size()>0) Fill(mydir,prefix,list.at(list.size()-1));
    Fill(dir,prefix,runs.at(position));
    if(position+1<runs.size()) Fill(dir,prefix,runs.at(position+1));
    return;
  }
  
  if(position-1>=0) Fill(dir,prefix,runs.at(position-1));
  if(position>=0)  Fill(dir,prefix,runs.at(position));
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

ClassImp(DynAlFitContainer);


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
  pnt=hit.GetGlobalCoordinate(imult,"");
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


void DynAlFitContainer::BuildAlignment(TString dir,TString prefix,int run){
  // Use the continuity tool to get the history
  FitParameters.clear();
  DynAlContinuity historyC(dir,prefix,run);
  Layer=historyC.Layer;

  // Copy it 
  DynAlHistory history;
  for(int i=0;i<historyC.History.Size();i++){
    DynAlEvent ev=historyC.History.Get(i);
    //    if(ev.lay()!=Layer) continue;
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

  for(int point=0;point<history.Size();point++){
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
    FitParameters[ev.Time[0]]=DynAlFitParameters(fit);    
  }

}

/////////////////// Finally the tools to provide the alignment as a function of time
ClassImp(DynAlManager);

#include "commonsi.h"

//map<int,DynAlFitContainer> DynAlManager::dynAlFitContainers;
DynAlFitContainer DynAlManager::dynAlFitContainers[10];
int DynAlManager::currentRun=-1;
int DynAlManager::skipRun=-1;
TString DynAlManager::defaultDir=".";

bool DynAlManager::FindAlignment(AMSEventR &ev,TrRecHitR &hit,double &x,double &y,double &z,TString dir){
  // Update the maps, if needed
  if(ev.fHeader.Run==skipRun) return false;

  if(ev.fHeader.Run!=currentRun){
    currentRun=ev.fHeader.Run;
    // Update the maps
    //    dynAlFitContainers.clear();
    int subdir=DynAlContinuity::getBin(currentRun);

    // If directory name is empty, use the default
    if(dir.Length()==0){
      //      dir=Form("%s/%s/DynAlignment",getenv("AMSDataDir"),AMSCommonsI::getversion());
      //      cout<<"DEFAULT IS "<<Form("%s/%s/DynAlignment",getenv("AMSDataDir"),AMSCommonsI::getversion())<<endl;
      dir=defaultDir;
    }

    // Update the map
    TFile file(Form("%s/%i/%i.align.root",dir.Data(),subdir,currentRun));
    DynAlFitContainer *l1=(DynAlFitContainer*)file.Get("layer_1");
    DynAlFitContainer *l9=(DynAlFitContainer*)file.Get("layer_9");
    if(!l1 || !l9){
      cout<<"DynAlFitContainer::FindAlignment-W-Unable to get \"layer_1\" and \"layer_9\" objects from "<<(Form("%s/%i/%i",dir.Data(),subdir,currentRun))<<endl;
      skipRun=currentRun;
      return false;
    }
    
    dynAlFitContainers[1]=*l1;
    dynAlFitContainers[9]=*l9;
    dynAlFitContainers[1].Layer=1;
    dynAlFitContainers[9].Layer=9;
  }


  // Get the hit layer
#ifdef _PGTRACK_
  int layer=hit.GetLayerJ();
#else
  int layer=hit.lay();
#endif

  //  if(dynAlFitContainers.find(layer)==dynAlFitContainers.end()){
  //    cout<<"DynAlFitContainer::FindAlignment-W-trying to use alignment for missing layer "<<layer<<endl;
  //    return false;
  //  }

  if(layer!=1 && layer!=9){
    // By the moment we only deal with layer 1 and layer 9
    return false;
  }

  dynAlFitContainers[layer].Eval(ev,hit,x,y,z);
  return true;
}


