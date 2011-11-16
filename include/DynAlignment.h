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
#ifdef __ROOTSHAREDLIBRARY__
#include "amschain.h"
#endif

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
  float Beta;        // rich beta  
  int Time[2];       // Time
  int Id;            // Id (slot, half...) using Vitaly naming scheme

  void extrapolateTrack(); // Propagate the track to the hit z
  int lay()const{return abs(Id)%10;}
  int lad()const{return (abs(Id)/100)%100;}
  int half()const{return (abs(Id)/10)%10;}
  int sen()const{return (abs(Id)/10000)%100;}
  bool falsex(){return Id<0;}

  bool operator<(const DynAlEvent &b) const {return Time[0]!=b.Time[0]?Time[0]<b.Time[0]:Time[1]<b.Time[1];}
  static bool buildEvent(AMSEventR &ev,int layer,DynAlEvent &event);
  static bool buildEvent(AMSEventR &ev,TrRecHitR &hit,DynAlEvent &event);

  ClassDef(DynAlEvent,1);
};


//
// The class DynAlHistory is a container of the calibration events 
// sorted in time. There is a history per run. This is intended to 
// be writen in files
//

class DynAlHistory: public TObject{
 public:
  int Run;
  int Layer;
  vector<DynAlEvent> Events;

  DynAlHistory(int run,int layer):Run(run),Layer(layer){Events.reserve(4096);}
  DynAlHistory():Run(0),Layer(-1){Events.reserve(4096);}
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
  double MinRigidity;     //! Minimum absolute rigidity 
  double MaxRigidity;     //! Maximum absolute rigidity 
  double MinBeta;         //! Minimum rich beta
  int Events;             //! Events in last fit
  int Order;              //! Order of the time slope

  // Parameters of last evaluation
  Double_t DX;
  Double_t DY;
  Double_t DZ;  
  Double_t THETA;  
  Double_t ALPHA;  
  Double_t BETA;  

  bool Fail;            // Set to true if the fit failed
  int ControlGroup;     //! 1 our of ControlGroup events are not used in the fit

  int GetId(int layer,int half,int ladder){return layer+10*half+100*ladder;};

  void Init();    // Initialize the fiter
  DynAlFit(int order=2);
  bool DoFit(DynAlHistory &history,int first,int last,DynAlEvent &exclude,bool force=false);
  bool ForceFit(DynAlHistory &history,int first,int last,set<int> &exclude);
  void Eval(DynAlEvent &event,double &x,double &y,double &z);
  void RetrieveFitPar(int t0=0,int t1=0);
  //  void ApplyLocalAlignment(int id,double &x,double &y,double &z);
  //  void ApplyLocalAlignment(int id,float &x,float &y,float &z);
  void Eval(AMSEventR &ev,double &x,double &y,double &z);
  ClassDef(DynAlFit,1);
};



class DynAlContinuity{
 public:
  static int getBin(int utime);                 // Directory name where to look for files 
  static int getNextBin(int utime);             // Directory name where to look for files 
  static int getPreviousBin(int utime);         // Directory name where to look for files 
  static bool select(AMSEventR *ev,int layer);  // Select the an event for the idx file
#ifdef __ROOTSHAREDLIBRARY__
  static void CreateIdx(AMSChain &ch,int layer,TString dir,TString prefix);  //Create an Idx file for chain ch and "layer" in directory dir, using as prefix "prefix"
#endif
  static void GetFileList(TString dir,vector<int> &lista);

  // Fit parameters
  static double BetaCut;      // Rich Beta Cut
  static double RigidityCut;      // Rich Beta Cut
  static int    FitOrder;     // Time order of the fit
  static int    FitWindow;    // Window time in minutes

  DynAlFit Fit;
  DynAlHistory History;          // Current history
  int CurrentRun;                 // Current run   
  TString DirName;
  TString Prefix;
  int Layer;
  DynAlContinuity():Fit(FitOrder),CurrentRun(-1){};
  DynAlContinuity(TString dir,TString prefix,int run=-1):Fit(FitOrder){ForceUpdate(dir,prefix,run);}; //  
  void ForceUpdate(TString dir,TString prefix,int run);
  bool Update(int run);
  void Fill(TString dir,TString prefix,int run);
  bool UpdateFit(AMSEventR &ev);

  ClassDef(DynAlContinuity,1);
};



/// A class to store the fit results for a single run
/// including local alignment

class DynAlFitParameters: public TObject{
 public:
  vector<Double_t> DX;
  vector<Double_t> DY;
  vector<Double_t> DZ;  
  vector<Double_t> THETA;  
  vector<Double_t> ALPHA;  
  vector<Double_t> BETA;  
  Double_t ZOffset;
  Double_t TOffset;
  
  DynAlFitParameters(){};
  DynAlFitParameters(DynAlFit &fit);
  Double_t GetTime(int seconds,int museconds){return ((seconds-TOffset)+1e-6*museconds)/60.0/90.0;}
  void ApplyAlignment(int seconds,int musecons,double &x,double &y,double &z);
  void ApplyAlignment(double &x,double &y,double &z);   // To use in case it is a local alignment

  ClassDef(DynAlFitParameters,1);
};

class DynAlFitContainer:public TObject{
 public:
  int Layer;
  map<int,DynAlFitParameters> FitParameters;       // Fits parameters at the second level
  map<int,DynAlFitParameters> LocalFitParameters;  // Local alignment
  bool ApplyLocalAlignment;
  void Eval(AMSEventR &ev,TrRecHitR &hit,double &x,double &y,double &z);
  void Eval(DynAlEvent &ev,double &x,double &y,double &z);
  int GetId(TrRecHitR &hit);
  int GetId(DynAlEvent &event){return 100*event.lad()+10*event.half()+event.lay();}
  
  void BuildLocalAlignment(DynAlHistory &history);
  void BuildAlignment(TString dir,TString prefix,int run);
  
  DynAlFitContainer():Layer(-1),ApplyLocalAlignment(false){};
  ClassDef(DynAlFitContainer,1);
};
  
class DynAlManager:public TObject{
 public:
  static bool FindAlignment(int run,int time,int layer,float hit[3],float hitA[3],int Id=-1,TString dir="");
  static bool FindAlignment(AMSEventR &ev,TrRecHitR &hit,double &x,double &y,double &z,TString dir="");
  //  static map<int,DynAlFitContainer> dynAlFitContainers; // STL containers are not thread safe 
  static DynAlFitContainer dynAlFitContainers[10];
  static int currentRun;
  static int skipRun;
  static TString defaultDir;
#pragma omp threadprivate(dynAlFitContainers,currentRun,skipRun)  
  ClassDef(DynAlManager,1);
};


#endif
