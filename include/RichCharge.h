#ifndef __RICHCHARGE__
#define __RICHCHARGE__
/**********************************************************
Implementation of tools for rich charge 
 **********************************************************/

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include "TString.h" 
#include "RichTools.h"

using namespace std;

//
// Constants
//
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define PI 3.14159265359

// Templates
template < typename T >
inline T highbit(T& t) { return t = (((T)(-1)) >> 1) + 1; }
template < typename T >
std::ostream& bin(T& value, std::ostream &o) {
  for ( T bit = highbit(bit); bit; bit >>= 1 ) o << ( ( value & bit ) ? '1' : '0' );
  return o;
}

namespace RichPMTCalibConstants{
  const int DEBUG = false;
  const int NPMT = 680;
  const int NRDR = 24;
  const int NJINF = 4;
  const int NSIDE = 2;
  const int NBRICK = 4;
  const int NHVCHN = 40;
  const int NTAG = 4;
  
  const float pmtRefTemperature = 25, pmtMinTemperature = -20, pmtMaxTemperature = 40;
  const float pmtRefdGdT = -0.56e-2, pmtMindGdT = -1.00e-2, pmtMaxdGdT = 0.00e-2;
  const float pmtRefdEdT =  0.08e-2, pmtMindEdT = -0.10e-2, pmtMaxdEdT = 0.30e-2;
  const float brickRefTemperature = 25, brickMinTemperature = -20, brickMaxTemperature = 40;
}

class RichPMTCalib{
 private:
  RichPMTCalib(){};
  ~RichPMTCalib(){};
  static RichPMTCalib *_header;     // This is used to make it singleton
#pragma omp threadprivate(_header)
 public:
  static RichPMTCalib* getHead(){return _header;};
  static TString currentDir;
  static bool Init(TString dir=".",int run=-1); // Start the singleton or modify it
  static RichPMTCalib* Update(int run=0);

  // Corrections fail status
  static int pmtCorrectionsFailed;

  // Configuration settings
  static int verbosityLevel;
  static bool loadPmtCorrections;
  static bool useRichRunTag;
  static bool usePmtStat;
  static bool useSignalMean;
  static bool useGainCorrections;
  static bool useEfficiencyCorrections;
  static bool useBiasCorrections;
  static bool useTemperatureCorrections;
  static int temperatureUpdatePeriod;
  static unsigned short richRunBad;
  static unsigned short richPmtBad;

  //
  // PMT DB
  vector<unsigned short> v_pmt_rdr;
  vector<unsigned short> v_pmt_brick, v_pmt_hvchn, v_pmt_vnom;

  //
  // RICH Config & Status
  unsigned short richRunTag;
  vector<unsigned short> v_pmt_stat, v_pmt_volt;

  enum { kTagJ = 0, kTagJHK = 1, kTagR = 2, kTagRHK = 3, kTagOcc = 4 };
  enum { kPmtFE = 0, kPmtHV = 1, kPmtJ = 2, kPmtR = 3, kPmtOcc = 4 };
  enum { A = 0, B = 1 };

  //
  // List of PMT Periods for Nominal HV
  multimap<int,string> m_pmt_periods;

  //
  // List of Non Nominal HV 
  multimap<int,string> m_pmt_voltages;

  //
  // List of BadPMTs ( dynamic )
  vector<int> BadPMTs;

  //
  // Efficiency & Gain corrections
  vector<float> v_pmt_ecor, v_pmt_gcor, v_pmt_gmcor;
  vector<float> v_pmt_temp_ref;
  vector<float> v_pmt_ecor_dflt, v_pmt_gcor_dflt, v_pmt_gmcor_dflt;
  vector<float> v_pmt_temp_ref_dflt;

  //
  // Bias Corrections
  vector<float> v_pmt_bcor;

  //
  // Temperature Corrections
  bool pmtTemperatures;
  vector<float> v_pmt_dGdT, v_pmt_dEdT, v_pmt_temp;
  bool brickTemperatures;
  vector<float> v_brick_temp;

//
// Declare here the functions, and define them if they are very one-liners
//
  bool init();
  bool retrieve(int);

  float  EfficiencyCorrection(int );
  float  GainCorrection(int );
  float  EfficiencyTemperatureCorrection(int );
  float  GainTemperatureCorrection(int );
  bool   initPMTs();
  void   updatePMTs(int run, bool force=true);
  bool   checkRichPmtTemperatures();
  //bool   checkRichBrickTemperatures();
  bool   getRichPmtTemperatures();
  bool   getRichBrickTemperatures();
  bool   ReadPmtDB();
  void   RichDecodeJ(string sline, vector<int> v_brick_fgin[], 
		     vector<unsigned short>& v_pmt_stat,
		     vector<unsigned short>& v_pmt_volt);
  void   RichDecodeJHK(string sline, vector<int> v_brick_busa[], vector<int> v_brick_busb[], 
		       vector<unsigned short>& v_pmt_stat);
  void   RichDecodeR(string sline,
		     vector<unsigned short>& v_pmt_stat);
  void   RichDecodeRHK(string sline,
		       vector<unsigned short>& v_pmt_stat);
  unsigned short CheckRichRun(int run,
			      vector<unsigned short> &v_pmt_stat,
			      vector<unsigned short> &v_pmt_volt);
  unsigned short CheckRichRunOLD(int run,
			      vector<unsigned short> &v_pmt_stat,
			      vector<unsigned short> &v_pmt_volt);
  bool richRunGood() {return !(richRunTag&richRunBad);}
  bool richPmtGood(int pmt) {return !(v_pmt_stat[pmt]&richPmtBad);}

  bool Reload();


  static int lastRun;
  static int lastEvent;
  static float NpColPMTCorr[680];
  static float NpExpPMTCorr[680];
  static bool buildCorrections();
  static float getNpColPMTCorr(int pmt){if(loadPmtCorrections && buildCorrections()) return NpColPMTCorr[pmt];return -1;}
  static float getNpExpPMTCorr(int pmt){if(loadPmtCorrections && buildCorrections()) return NpExpPMTCorr[pmt];return -1;}

  ClassDef(RichPMTCalib,1);
};



class RichRingR;

class RichChargeUniformityCorrection{
private:
  RichChargeUniformityCorrection(){_agl=_naf=0;_latest[0]=_latest[1]=_latest[2]=_latest[3]=_latest[4];};
  ~RichChargeUniformityCorrection(){if(_agl) delete _agl;if(_naf) delete _naf;};
public:
  static RichChargeUniformityCorrection* _head;
  static RichChargeUniformityCorrection *getHead(){return _head;}
  static bool Init(TString="");

  GeomHashEnsemble *_agl;
  GeomHashEnsemble *_naf;

  float _latest[5];

  // Accessors to the correction
  float getCorrection(RichRingR *ring);
  float getCorrection(float *); 
  float getCorrection(float beta,float x,float y,float vx,float vy);
  // Accessors to several quality parameters
  float getWidth(RichRingR *ring);
  float getWidth(float *); 
  float getWidth(float beta,float x,float y,float vx,float vy);
  // Accessors to several quality parameters
  float getRms(RichRingR *ring);
  float getRms(float *); 
  float getRms(float beta,float x,float y,float vx,float vy);
};



#endif
