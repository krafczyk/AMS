#ifndef __DynAl__
#define __DynAl__

#include "TLinearFitter.h"
#include "TObject.h"
#include "TChain.h"
#include "TString.h" 
#include <vector>
#include <map>
#include <set>
#include "root.h"

using namespace std;

//
// The class DynAlEvent contains all the needed information for a single event used in the alignment procedure
//

class DynAlEvent: public TObject{
 public:
  float RawHit[3];   // Hit position without correction
  float TrackHit[3]; // Extrapolate track to the hit plane
  float TrackTheta;  // Theta
  float TrackPhi;    // and phi 
  float Rigidity;    // rigidity (unused)
  int Time[2];       // Time
  int Id;            // Id (slot, half...) using Vitaly naming scheme

  void extrapolateTrack(); // Propagate the track to the hit z
  int lay()const{return Id%10;}
  int lad()const{return (Id/100)%100;}
  int half()const{return (Id/10)%10;}
  int sen()const{return (Id/10000)%100;}

  static bool buildEvent(AMSEventR &ev,int layer,DynAlEvent &event);

  ClassDef(DynAlEvent,1);
};


//
// The class DynAlHistory is a container of the calibration events 
// sorted in time. There is a history per run. This is intended to 
// be writen in files
//

class DynAlHistory: public TObject{
 public:
  unsigned int Run;
  vector<DynAlEvent> Events;

  DynAlHistory(int run):Run(run){Events.reserve(4096);}
  DynAlHistory(){}
  void Push(DynAlEvent &event){Events.push_back(event);}
  int FindCloser(int seconds,int muSeconds,int initial=0);
  void FindTimeWindow(int event,int seconds,int &first,int &last);
  int Find(DynAlEvent &event,int initial=0);
  unsigned int Size(){return Events.size();}
  DynAlEvent &Get(int i){return Events.at(i);}

  ClassDef(DynAlHistory,1);
};


//
//  The class DynAlFit performs the fit for to obtain the alignment for a single event 
//


class DynAlFit: public TObject{
 public:
  TLinearFitter Fit;      //!
  int First;              //! From the last fit, the range and excluded event to avoid duplicate computation
  int Last;               //!
  int Excluded;           //!
  double ZOffset;         // Offset for Z 
  int TOffset;            // Last fist T offset
  bool   LadderFit;       //! Fit ladder by ladder if true
  double MinRigidity;     //! Minimum absolute rigidity 
  double MaxRigidity;     //! Maximum absolute rigidity 
  int Events;             //! Events in last fit
  int Order;              //! Order of the time slope

  // Parameters of last evaluation
  Double_t DX;
  Double_t DY;
  Double_t DZ;  
  Double_t THETA;  
  Double_t ALPHA;  
  Double_t BETA;  

  // Use local alignment
  bool UseLocalAlignment;
  map<int,double> LocalDX;
  map<int,double> LocalDY;
  map<int,double> LocalDZ;
  map<int,double> LocalTHETA;
  map<int,double> LocalALPHA;
  map<int,double> LocalBETA;

  bool Fail;            // Set to true if the fit failed
  int ControlGroup;     //! 1 our of ControlGroup events are not used in the fit

  int GetId(int layer,int half,int ladder){return layer+10*half+100*ladder;};

  void Init();    // Initialize the fiter
  DynAlFit(int order=2);
  bool DoFit(DynAlHistory &history,int first,int last,DynAlEvent &exclude,bool force=false);
  bool ForceFit(DynAlHistory &history,int first,int last,set<int> &exclude);
  void Eval(DynAlEvent &event,double &x,double &y,double &z);
  void RetrieveFitPar(int t0=0,int t1=0);
  void ApplyLocalAlignment(int id,double &x,double &y,double &z);
  void ApplyLocalAlignment(int id,float &x,float &y,float &z);
};



//
// Implement a tool to guarantee the continuity between different runs
//
/*
#include "root.h"

class DynAlContinuity{
 public:
  map<int,TString> Run2File;      // Map of run to filename
  map<int,vector<int> > Run2Runs; // Map of run to runs to load
  DynAlHistory *History;          // Current history
  int CurrentRun;
  int LastVisited;
  TString Prefix; 
  int Plane;


  void Init(TChain &chain);
  Continuity():History(0),CurrentRun(0),LastVisited(-1),Prefix(""),Plane(0){};
  Continuity(TChain &ch,int plane,TString prefix=""):History(0),CurrentRun(0),LastVisited(-1),Prefix(prefix),Plane(plane){Init(ch);}
  void SetRun(int run);
  TFile *FindCreateIdx(int run,TString file);

  //Tools
  void CreateIdx(int run,TString file);                      // Create the idx file
  static bool buildEvent(AMSEventR &ev,int layer,DynAlEvent &event);   // Dump a event into the DynAlEvent structure. Return true is succeed
  static int select(AMSEventR *ev,int layer);                          // Check if the event has to be selected to compute the alignment
};
*/

#endif
