#ifndef _TrdHCalib_
#define _TrdHCalib_
#include <map>
#include "TObject.h"
#include "TGraph.h"

class AMSTimeID;
class AMSEventR;
class TF1;
class TGraphErrors;
class TrdRawHitR;
class TrdHTrackR;

using namespace std;

/// class to perform and store TRD H calibration
class TrdHCalibR{
 private:
  /// singleton instance
  static TrdHCalibR* head;

  /// internal map of TDV pointers
  map<string,AMSTimeID*> tdvmap;

  /// internal variable to control TDV
  unsigned int lastrun;

 public:
  /// total number of tubes
  static const int n_tubes = 5248;

  /// flag if calibration should be performed  
  bool calibrate;

  /// normalization 'goal' value (chosen to gain=1 before/after launch)
  float norm_mop;

  /// container for tube medians
  float tube_medians[n_tubes];
  
  /// container for tube occupancy
  int tube_occupancy[n_tubes];
  
  /// container for module medians
  float mod_medians[328];

  /// container for HVchannel medians
  float hv_medians[82];

  /// container for manifold medians
  float mf_medians[10];

  /// container for detector medians;
  float det_median;

  /// container for TDV gain storage
  static float tube_gain[6064];

  /// container for TDV status storage
  unsigned int tube_status[6064];

  /// container for TDV pedestal storage
  float tube_pedestals[6064];

  /// container for TDV pedestal width / sigma storage
  float tube_sigmas[6064];

  /// min number of hits per channel not to be 'low occupancy'
  int min_occupancy;

  TGraphErrors *g_additional_modcorr;
  float additional_modcorr_mean;

  /*  TSpline3 *s_bg_p0;
      TSpline3 *s_bg_p1;
      TSpline3 *s_bg_p2;

      TSpline3 *s_path_p0;
      TSpline3 *s_path_p1;
      TSpline3 *s_path_p2;
      TSpline3 *s_path_p3;*/

  TGraph *s_bg_p0;
  TGraph *s_bg_p1;
  TGraph *s_bg_p2;

  TGraph *s_path_p0;
  TGraph *s_path_p1;
  TGraph *s_path_p2;
  TGraph *s_path_p3;

  TGraphErrors *g_bgcorr_helium;
  float bgcorr_helium(float bg);
  double GraphXmax(TGraph *g);

  double bgval(float x,float dE);
  double bgcorr(float x,float dE);
  TF1 *bgfunc(float x);
  int bgmap(char* fname="fitfile.txt");
  int bgmap(int opt);

  double pathval(float x,float dE);
  double pathcorr(float x,float dE);
  TF1 *pathfunc(float x);
  int pathmap(char* fname="fitfile.txt");
  int pathmap(int opt);

  float additional_modcorr(int mod);
  
  /// initialize calibration with all medians set to 'start_value'
  void init_calibration(float start_value);

  /// obsolete - paramtrization of dE as function of log10(P/GeV) (extracted from space proton data)
  TF1 *f_logmom;
  
  /// default ctor
  TrdHCalibR(){
    norm_mop=49.;
    min_occupancy=50;
    calibrate=0;
    f_logmom=0;
    lastrun=0;
    det_median=norm_mop;

    for(int i=0;i<n_tubes;i++){
      if(i<10)mf_medians[i]=1.;
      if(i<82)hv_medians[i]=1.;
      if(i<328)mod_medians[i]=1.;
      tube_occupancy[i]=0;
      tube_medians[i]=1.;
    }
    g_additional_modcorr=0;
    g_bgcorr_helium=0;
    additional_modcorr_mean=0.;
    
    s_bg_p0=0;
    s_bg_p1=0;
    s_bg_p2=0;

    s_path_p0=0;
    s_path_p1=0;
    s_path_p2=0;
    s_path_p3=0;

    for(int i=0;i<6064;i++){
      tube_status[i]=0;
      tube_gain[i]=1.;
      tube_pedestals[i]=0.;
      tube_sigmas[i]=0.;
    }
  };
  
  /// dtor
  virtual ~TrdHCalibR(){}
  
  /// Get the pointer to singleton
  static TrdHCalibR* gethead(){
    if(!head){
      head=new TrdHCalibR();
      head->pathmap(0);
      head->bgmap(0);
    }
    return head;
  }
  
  /// Kill the pointer to singleton
  static void  KillPointer(){
    delete head;
    head=0;  //VC
  }
  
  /// Update medians with given TrdHTrack - 'option' controls the applied corrections (>0 path;>1 beta - tof 'beta' has to be provided if correction enabled - manual correction factor 'corr' applicable 
  void update_medians(TrdHTrackR *track,int version=0,int option=3, float beta=0, float corr=1.,int debug=0);
  
  /// Convert layer[0,19]/ladder[0,17]/tube[0,15] to unique tube id [0,5247]
  static void GetTubeIdFromLLT(int layer,int ladder,int tube,int &tubeid);
  
  /// Convert unique tube id [0,5247] to layer[0,19]/ladder[0,17]/tube[0,15]
  static void GetLLTFromTubeId(int &layer,int &ladder,int &tube,int tubeid);
  
  /// Get TDV iterator for tube id
  static int GetNTDV(int tubeid);
  
  /// Get TDV iterator for layer ladder tube
  static int GetNTDV(int layer, int ladder, int tube);
  
  /// Get TDV iterator for raw hit
  static int GetNTDV(TrdRawHitR* hit);
  
  /// Get parametrization of dE as function of path (extracted from space proton data) - two versions available('option')
  static float PathParametrization(float path,int option=0,int debug=0);
  
  /// Get parametrization of dE as function of beta (extracted from space proton data - not corrected for path yet tdb) - two versions available ('option')
  static float BetaParametrization(float beta,int option=0,int debug=0);
  
  /// Get correction factor to correct from event tof 'beta' to given value 'tobeta' - two parametrizations available ('option')
  float GetBetaCorr(double beta, double tobeta=0.95, int option=0, int debug=0); 
  
  /// Get correction factor to correct energy deposition in [adc] for betagamma (from tracker rigidity)
  float GetBetaGammaCorr(double edep,double betagamma, int opt=0, int debug=0); 
  
  /// Get correction factor to correct energy deposition for path length 'path' - option 0/1 obsolete parametrizations kept for historical reasons - value=energy deposition/'path length target value' for options 3 / '1&2'
  float GetPathCorr(float path, float value=0.6, int option=3, int debug=0); 
  
  /// Get gain correction factor for given 'hit' - 'option': 1-force read from ROOT file 2-force read from TDV 0-if not in ROOT file then read TDV
  float GetGainCorr(TrdRawHitR* hit, int option=0, int debug=0); 
  
  /// Get gain correction factor for given 'layer/ladder/tube' - 'option': 1-force read from ROOT file 2-force read from TDV 0-if not in ROOT file then read TDV
  float GetGainCorr(int layer,int ladder, int tube, int option=0,int debug=0); 
  
  /// Get status for given 'hit' - 'option': 1-force read from ROOT file 2-force read from TDV 0-if not in ROOT file then read TDV
  unsigned int GetStatus(TrdRawHitR* hit, int opt=0, int debug=0); 
  
  /// Get status for given 'layer/ladder/tube' - 'option': 1-force read from ROOT file 2-force read from TDV 0-if not in ROOT file then read TDV
  unsigned int GetStatus(int layer,int ladder, int tube, int opt=0,int debug=0); 

  /// Get Mean of gaussian fit to distribution of tube gains
  float MeanGaussGain(int opt=0);
  
  /// Get Mean of gaussian fit to distribution of tube medians
  float MeanGaussMedian(int opt=0);
  
  /// Get Mean occupancy of all tubes
  float MeanOccupancy(int opt=0);
  
  /// Get gas circuit number 'gascirc'[0,41] and local iterator 'gasmod' [0,7] along it associated to module at 'layer/ladder'
  static void LLT2GCM(int layer, int ladder, int &gascirc, int &gasmod);
  
  /// Get position of gas inlet for 'gascirc': 0-left bottom 1-right bottom 2-left top 3-right top  
  static int GetInletGeom(int gascirc);
  
  /// Get Manifold[0,9] which supplies gas circuit 'gascirc'
  static int GetManifold(int gascirc);
  
  /// Apply dynamic correction during analysis - gain correction varied during run
  int DynamicCalibration(AMSEventR *pev, int opt=0);
  
  /// Convert information from TDV into internal median structure
  int FillMedianFromTDV(int debug=0);
  
  /// Convert information from internal median to TDV structure
  int FillTDVFromMedian(int debug=0);
  
  /// Initialization for event analysis 
  int Initialize(AMSEventR* ev, char *databasedir=""); 
  
  /// Initialize interfaces to TDV for ALL Containers TRDPedestals, TRDSigmas, TRDGains, TRDStatus (,TRDMCGains)
  bool InitTDV(unsigned int bgtime, unsigned int edtime, int type,char *databasedir="");
  
  /// Initialize single interface to TDV for 'which' Container (float values) - e.g. TRDGains
  bool InitSpecificTDV(string which,int size, float *arr,unsigned int bgtime, unsigned int edtime, int type,char *databasedir="");
  
  /// Initialize single interface to TDV for 'which' Container (unsigned int values) - esp. TRDStatus
  bool InitSpecificTDV(string which,int size, unsigned int *arr,unsigned int bgtime, unsigned int edtime, int type,char *databasedir="");
  
  /// Write ALL available TDV interfaces (loaded by Init*) to TDV at 'databasedir'
  int writeTDV(unsigned int begin, unsigned int end,int debug=0, char *databasedir="");
  
  /// Write single TDV interface 'which' (loaded by Init*) to TDV at 'databasedir'
  int writeSpecificTDV(string which,unsigned int insert,unsigned int begin, unsigned int end,int debug=0, char *databasedir="");
  
  /// Read ALL loaded TDV interfaces for time 't'
  bool readTDV(unsigned int t, int debug=0);
  
  /// Read single loaded TDV interface for time 't'
  bool readSpecificTDV(string which,unsigned int t, int debug=0);
  
  /// Close ALL currently open TDV interfaces
  bool closeTDV();
    
  ClassDef(TrdHCalibR,3);
};
#endif


