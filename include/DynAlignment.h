#ifndef __DynAl__
#define __DynAl__
#include "TLinearFitter.h"
#include "TObject.h"
#include "TChain.h"
#include "TString.h" 
#include <vector>
#include <map>
#include <set>
#include "point.h"

class AMSEventR;
class TrRecHitR;
#ifdef __ROOTSHAREDLIBRARY__
class AMSChain;
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

  float ThetaM;
  float PhiM;
  float Cutoff;

  void extrapolateTrack(); // Propagate the track to the hit z
  //#define abs(_x) ((_x)<0?-(_x):(_x))
  int lay()const{return abs(Id)%10;}
  int lad()const{return (abs(Id)/100)%100;}
  int half()const{return (abs(Id)/10)%10;}
  int sen()const{return (abs(Id)/10000)%100;}
  //#undef abs
  bool falsex(){return Id<0;}

  bool operator<(const DynAlEvent &b) const {return Time[0]!=b.Time[0]?Time[0]<b.Time[0]:Time[1]<b.Time[1];}
  static bool buildEvent(AMSEventR &ev,int layer,DynAlEvent &event);
  static bool buildEvent(AMSEventR &ev,TrRecHitR &hit,DynAlEvent &event);
  int getClass();

  ClassDef(DynAlEvent,3);
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

  struct CLASSINFO{
    double mean[2];
    double rms[2];
    double entries[2];
  };

  // UTILS
  static void getRange(vector<double> &array,double &Min,double &Max,int buckets=10000);
  static bool bucketSort(vector<double> &array,const int buckets=10000);
  static bool bucketSort(vector<double> &array,double Min,double Max,const int buckets=10000);
  static bool findPeak(vector<double> &array,const double fraction,double &peak,double &width,int buckets=10000);
  static bool findPeak(vector<double> array,const double fraction,double Min,double Max,double &peak,double &width,int buckets=10000);


  ClassDef(DynAlFit,4);
};



class DynAlContinuity{
 public:
  static int getBin(int utime);                 // Directory name where to look for files 
  static int getNextBin(int utime);             // Directory name where to look for files 
  static int getPreviousBin(int utime);         // Directory name where to look for files 
  static bool select(AMSEventR *ev,int layer);  // Select the an event for the idx file
#ifdef __ROOTSHAREDLIBRARY__
  static void CreateIdx(AMSChain &ch,int layer,TString dir,TString prefix);  //Create an Idx file for chain ch and "layer" in directory dir, using as prefix "prefix"
  static void CleanAlignment();
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
  int FirstOfCurrentRun;         // First event of current run in the history
  int LastOfCurrentRun;          // Last event of curren run in the history  
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
  // Return the parameters without local alignment
  void GetParameters(int seconds,int musecodns,AMSPoint &posA,AMSRotMat &rotA);

  // Structure to store the whole information needed by a single
  // fit linearly    
  struct SingleFitLinear{
    int time;
    float DX;
    float DY;
    float DZ;
    float THETA;
    float ALPHA;
    float BETA;
    float ZOffset;
    float TOffset;
    int id; // To be use in case of local alignment, -1 otherwise
  };

  void dumpToLinearSpace(SingleFitLinear &single,int when=0,int id=-1); 
  DynAlFitParameters(SingleFitLinear &single);

  ClassDef(DynAlFitParameters,3);
};

class DynAlFitContainer:public TObject{
 public:
  int Layer;
  map<int,DynAlFitParameters> FitParameters;       // Fits parameters at the second level
  map<int,DynAlFitParameters> LocalFitParameters;  // Local alignment
  bool ApplyLocalAlignment;
  bool Find(int time,DynAlFitParameters &fit);
  void Eval(AMSEventR &ev,TrRecHitR &hit,double &x,double &y,double &z);
  void Eval(DynAlEvent &ev,double &x,double &y,double &z);
  int GetId(TrRecHitR &hit);
  static int GetId(DynAlEvent &event){return 100*event.lad()+10*event.half()+event.lay();}
  
  void BuildLocalAlignment(DynAlHistory &history);
  void BuildAlignment(TString dir,TString prefix,int run);

  void DumpLocalAlignment();
  
  DynAlFitContainer():Layer(-1),ApplyLocalAlignment(false){};


  // Enough space to store one day of data
#define LinearSpaceMaxRecords (60*60*24)
    
  struct LinearSpace{
    int id;         // Internal use. 
    int records;    // How many recors
    DynAlFitParameters::SingleFitLinear Alignment[LinearSpaceMaxRecords][2];
  };
  
  bool dumpToLinearSpace(LinearSpace &tdvBuffer,bool layer=false);
  DynAlFitContainer(LinearSpace &tdvBuffer,bool layer9=false);
  void TestDump();
  ClassDef(DynAlFitContainer,4);
};




class AMSTimeID;

class DynAlManager:public TObject{
 public:
  static bool UpdateParameters(int run,int time,TString dir=""); 
  static bool UpdateWithTDV(int time);
  static bool FindAlignment(int run,int time,int layer,float hit[3],float hitA[3],int Id=-1,TString dir="");
  static bool FindAlignment(AMSEventR &ev,TrRecHitR &hit,double &x,double &y,double &z,TString dir="");
  static DynAlFitContainer dynAlFitContainers[10];
  static int currentRun;
  static int skipRun;
  static TString defaultDir;
  static DynAlFitContainer::LinearSpace tdvBuffer;
  static AMSTimeID *tdvdb;
  static bool useTDV;    // Force usage of TDV and ignoring of AMSSetup
  static bool need2bookTDV; // igonore booking of TDV if set to true: warning this is not for general use
  static unsigned int begin;
  static unsigned int insert;
  static unsigned int end;
  static void ForceUpdating() {currentRun=skipRun=-1;}
  static int ControlGroup;

  // TDV interface
  static void ResetLinear(){tdvBuffer.records=0;}
  static bool AddToLinear(int time,DynAlFitParameters &layer1,DynAlFitParameters &layer9);
  static bool FinishLinear();
  static bool DumpDirToLinear(TString dir); // Dump a while directory ti a TDV
  static DynAlFitContainer BuildLocalAlignment(DynAlHistory &history);
#pragma omp threadprivate(dynAlFitContainers,currentRun,skipRun,tdvBuffer,tdvdb)  

  static void LocalAlignmentTest();  // A simple test

  ClassDef(DynAlManager,2);
};

void _ToAlign();

#endif
