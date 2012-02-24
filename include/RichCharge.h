#ifndef __RICHCHARGE__
#define __RICHCHARGE__
/**********************************************************
Implementatation of the analysis routines themselves
 **********************************************************/

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include "TString.h" 

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

namespace RichChargeConstants{
  const int DEBUG = false;
  const int NPMT = 680;
  const int NRDR = 24;
  const int NJINF = 4;
  const int NSIDE = 2;
  const int NBRICK = 4;
  const int NHVCHN = 40;
  const int NTAG = 4;
  
  const float pmtMinTemperature = -20, pmtMaxTemperature = 40;
  const float pmtRefTemperature = 25, pmtRefdGdT = -0.56e-2;
  const float brickMinTemperature = -20, brickMaxTemperature = 40;
  const float brickRefTemperature = 25, brickRefdVdT = -1.e-3;
  
  const int NewTrackAMSVersion = 565; // First Production Version w/ new TrTrack 
}

class RichCharge{
 private:
  RichCharge(){};
  ~RichCharge(){};
  static RichCharge *_header;     // This is used to make it singleton
#pragma omp threadprivate(_header)
 public:
  static RichCharge* getHead(){return _header;};
  static TString currentDir;
  static bool Init(TString dir="."); // Start the singleton or modify it

  // Configuration settings
  static bool useRichRunTag;
  static bool usePmtStat;
  static bool useEfficiencyCorrections;
  static bool useGainCorrections;
  static bool useTemperatureCorrections;
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
  // List of BadPMTs ( should be extended to pmt, begin_time, end_time)
  vector<int> BadPMTs;

  //
  // Efficiency & Gain corrections

  vector<float> v_pmt_ecor, v_pmt_gcor, v_pmt_gmcor;
  vector<float> v_brick_temp_ref, v_pmt_temp_ref;

  //
  // Temperature Corrections

  bool pmtTemperatures;
  vector<float> v_pmt_dGdT, v_pmt_dGdV, v_pmt_temp;
  bool brickTemperatures;
  vector<float> v_brick_dVdT, v_brick_temp;

  //
  // Gain Monitoring
  vector<float> v_signal[680], v_signalc[680];

//
// Declare here the functions, and define them if thwy are very one-liners
//
  bool init();
  bool retrieve(int);

  /*
  float  correctedNpExp(RichRingR *);
  float  correctedNpExp(::RichRing *);
  float  correctedNpCol(RichRingR *);
  float  correctedNpCol(::RichRing *);
  int    correctedUsed(RichRingR *);
  int    correctedUsed(::RichRing *);
  */
  float  EfficiencyCorrection(int );
  float  GainCorrection(int );
  float  TemperatureCorrection(int );
  bool   initPMTs();
  void   initBadPMTs();
  bool   checkRichPmtTemperatures();
  bool   checkRichBrickTemperatures();
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
  bool richRunGood() {return !(richRunTag&richRunBad);}
  bool richPmtGood(int pmt) {return !(v_pmt_stat[pmt]&richPmtBad);}
};

#endif
