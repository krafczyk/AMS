#ifndef _TrdHCalib_
#define _TrdHCalib_
#include <map>
#include "point.h"
#include "TrdRawHit.h"
#include "TrdHRecon.h"
class AMSTimeID;
class AMSEventR;
class TF1;

using namespace std;

/// class to perform and store TRD H calibration
class TrdHCalibR{
 private:
  /// singleton instance
  static TrdHCalibR* head;

  /// internal map of TDV pointers
  map<string,AMSTimeID*> tdvmap;

 public:
  /// flag if calibration should be performed  
  bool calibrate;

  /// initialize calibration with all medians set to 'start_value'
  void init_calibration(float start_value);

  /// normalization 'goal' value (chosen to gain=1 before/after launch)
  float norm_mop;

  /// container for tube medians
  float tube_medians[5248];
  
  /// container for tube occupancy
  int tube_occupancy[5248];
  
  /// container for module medians
  float mod_medians[328];

  /// container for HVchannel medians
  float hv_medians[82];

  /// container for manifold medians
  float mf_medians[10];

  /// container for detector medians;
  float det_median;

  /// container for TDV gain storage
  float tube_gain[6064];

  /// container for TDV status storage
  unsigned int tube_status[6064];

  /// container for TDV pedestal storage
  float tube_pedestals[6064];

  /// container for TDV pedestal width / sigma storage
  float tube_sigmas[6064];

  /// min number of hits per channel not to be 'low occupancy'
  int min_occupancy;

  /// TESTING - paramtrization of dE as function of log10(P/GeV) (extracted from space proton data)
  TF1 *f_logmom;

  /// default ctor
  TrdHCalibR():norm_mop(49.),min_occupancy(50),calibrate(0){
    f_logmom=0;
    det_median=norm_mop;
    for(int i=0;i<5248;i++){
      if(i<10)mf_medians[i]=1.;
      if(i<82)hv_medians[i]=1.;
      if(i<328)mod_medians[i]=1.;
      tube_occupancy[i]=0;
      tube_medians[i]=1.;
    }
    det_median=norm_mop/0.4;
    for(int i=0;i<6064;i++){
      tube_status[i]=0;
      tube_gain[i]=1.;
      tube_pedestals[i]=0.;
      tube_sigmas[i]=0.;
    }
  }
    
  /// dtor
  ~TrdHCalibR(){};
  
  /// Get the pointer to singleton
  static TrdHCalibR* gethead(){
    if(!head)head=new TrdHCalibR();
    return head;
  }
  
  /// Kill the pointer to singleton
  static void  KillPointer(){
    delete head;
    head=0;  //VC
  }
  
  /// Update medians with given TrdHTrack - 'option' controls the applied corrections (>0 path;>1 beta - tof 'beta' has to be provided if correction enabled - manual correction factor 'corr' applicable 
  void update_medians(TrdHTrackR *track,int option=3, float beta=0, float corr=1.,int debug=0);
  
  /// Convert layer[0,19]/ladder[0,17]/tube[0,15] to unique tube id [0,5247]
  void GetTubeIdFromLLT(int layer,int ladder,int tube,int &tubeid);
  
  /// Convert unique tube id [0,5247] to layer[0,19]/ladder[0,17]/tube[0,15]
  void GetLLTFromTubeId(int &layer,int &ladder,int &tube,int tubeid);
  
  /// Get TDV iterator for tube id
  int GetNTDV(int tubeid);

  /// Get TDV iterator for layer ladder tube
  int GetNTDV(int layer, int ladder, int tube);

  /// Get TDV iterator for raw hit
  int GetNTDV(TrdRawHitR* hit);

  /// Get parametrization of dE as function of path (extracted from space proton data) - two versions available('option')
  float PathParametrization(float path,int option=0,int debug=0);
  
  /// Get parametrization of dE as function of beta (extracted from space proton data - not corrected for path yet tdb) - two versions available ('option')
  float BetaParametrization(float beta,int option=0,int debug=0);
  
  /// Get correction factor to correct from event tof 'beta' to given value 'tobeta' - two parametrizations available ('option')
  float GetBetaCorr(double beta, double tobeta=0.95, int option=0, int debug=0); 
  
  /// Get correction factor to correct from hit 'path' to given value 'topath' - two parametrizations available ('option')
  float GetPathCorr(float path, float topath=0.6, int option=0, int debug=0); 
  
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
  void LLT2GCM(int layer, int ladder, int &gascirc, int &gasmod);
  
  /// Get position of gas inlet for 'gascirc': 0-left bottom 1-right bottom 2-left top 3-right top  
  int GetInletGeom(int gascirc);
  
  /// Get Manifold[0,9] which supplies gas circuit 'gascirc'
  int GetManifold(int gascirc);
  
  /// TESTING - Apply dynamic correction during analysis - gain correction varied during run
  int DynamicCalibration(AMSEventR *pev);
  
  /// Convert information from TDV into internal median structure
  int FillMedianFromTDV(int debug=0);
  
  /// Convert information from internal median to TDV structure
  int FillTDVFromMedian(int debug=0);
  
  /// Initialize interfaces to TDV for ALL Containers TRDPedestals, TRDSigmas, TRDGains, TRDStatus (,TRDMCGains)
  bool InitTDV(unsigned int bgtime, unsigned int edtime, int type,char *dirname="");
  
  /// Initialize single interface to TDV for 'which' Container (float values) - e.g. TRDGains
  bool InitSpecificTDV(string which,int size, float *arr,unsigned int bgtime, unsigned int edtime, int type,char *dirname="");
  
  /// Initialize single interface to TDV for 'which' Container (unsigned int values) - esp. TRDStatus
  bool InitSpecificTDV(string which,int size, unsigned int *arr,unsigned int bgtime, unsigned int edtime, int type,char *dirname="");
  
  /// Write ALL available TDV interfaces (loaded by Init*) to TDV at 'dirname'
  int writeTDV(unsigned int begin, unsigned int end,int debug=0, char *dirname="");
  
  /// Write single TDV interface 'which' (loaded by Init*) to TDV at 'dirname'
  int writeSpecificTDV(string which,unsigned int insert,unsigned int begin, unsigned int end,int debug=0, char *dirname="");
  
  /// Read ALL loaded TDV interfaces for time 't'
  bool readTDV(unsigned int t, int debug=0);
  
  /// Read single loaded TDV interface for time 't'
  bool readSpecificTDV(string which,unsigned int t, int debug=0);
  
  /// Close ALL currently open TDV interfaces
  bool closeTDV();
  
  ClassDef(TrdHCalibR,2)
};
#endif

