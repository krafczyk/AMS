#ifndef _TrdHCalib_
#define _TrdHCalib_
#include <map>
#include "point.h"
#include "TrdRawHit.h"
#include "TrdHRecon.h"
class AMSTimeID;
 
using namespace std;

/// class to perform and store TRD H calibration
class TrdHCalibR{
  static TrdHCalibR* head;
 public:

  /// Get the pointer to the DB
  static TrdHCalibR* gethead(){
    if(!head)head=new TrdHCalibR();
    return head;
  }
  
  /// Get the pointer to the DB
  static void  KillPointer(){
    delete head;
    head=0;  //VC
  }

  /// flag if calibration should be performed
  bool calibrate;

  /// initialize calibration
  void init_calibration(float start_value);

  /// container for channel medians
  float tube_medians[5248];
  
  /// container for channel occupancy
  int tube_occupancy[5248];
  
  /// container for module medians
  float mod_medians[328];

  /// container for HVchannel medians
  float hv_medians[82];

  /// container for manifold medians
  float mf_medians[10];

  /// container for detector;
  float det_median;

  float tube_gain[6064];
  unsigned int tube_status[6064];

  float tube_pedestals[6064];
  float tube_sigmas[6064];

  int min_occupancy;

  map<string,AMSTimeID*> tdvmap;

  float norm_mop;

  /// default ctor
  TrdHCalibR():norm_mop(49.),min_occupancy(50),calibrate(0){
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
  };

    ~TrdHCalibR(){};

    void update_medians(TrdHTrackR *track,int opt=3, float beta=0, int debug=0);
  
    int FillMedianFromTDV();

    int FillTDVFromMedian();

    bool InitTDV(unsigned int bgtime, unsigned int edtime, int type,char *tempdirname="");

    bool InitSpecificTDV(string which,int size, float *arr,unsigned int bgtime, unsigned int edtime, int type,char *tempdirname="");
    bool InitSpecificTDV(string which,int size, unsigned int *arr,unsigned int bgtime, unsigned int edtime, int type,char *tempdirname="");

    int writeTDV(unsigned int begin, unsigned int end,int debug=0, char *tempdirname="");
    int writeSpecificTDV(string which,unsigned int insert,unsigned int begin, unsigned int end,int debug=0, char *tempdirname="");

    bool readTDV(unsigned int t, int debug=0);

    bool readSpecificTDV(string which,unsigned int t, int debug=0);

    bool closeTDV();

    void GetTubeIdFromLLT(int layer,int ladder,int tube,int &tubeid);
    void GetLLTFromTubeId(int &layer,int &ladder,int &tube,int tubeid);

    float PathParametrization(float path,int opt=0,int debug=0);
    float BetaParametrization(float beta,int opt=0,int debug=0);
 
    float GetBetaCorr(double beta, double tobeta=0.95, int opt=0, int debug=0); 
    float GetPathCorr(float path, float topath=0.3, int opt=0, int debug=0); 

    float GetGainCorr(TrdRawHitR* hit, int opt=0, int debug=0); 
    float GetGainCorr(int layer,int ladder, int tube, int opt=0,int debug=0); 

    unsigned int GetStatus(TrdRawHitR* hit, int opt=0, int debug=0); 
    unsigned int GetStatus(int layer,int ladder, int tube, int opt=0,int debug=0); 
    
    float MeanGaussGain(int opt=0);
    float MeanGaussMedian(int opt=0);
    float MeanOccupancy(int opt=0);
    
    void LLT2GCM(int layer, int ladder, int &gascirc_, int &gasmod);
    int GetInletGeom(int gascirc);
    int GetManifold(int gascirc);

    ClassDef(TrdHCalibR,1)
      };
#endif

