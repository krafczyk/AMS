#ifndef __RichConfig__
#define __RichConfig__
#include "TObject.h"
#include "TChain.h"
#include "TString.h" 
#include <vector>
#include <map>
#include <string>

using namespace std;

//
// A class to define and validate RICH Configuration
//

class RichPMTCalib;

/// A class to store the records for a single run
class RichConfigContainer:public TObject{
 public:
  int runID;

  //
  // PMT DB
  vector<unsigned short> v_pmt_rdr;
  vector<unsigned short> v_pmt_brick, v_pmt_hvchn, v_pmt_vnom;

  //
  // RICH Config & Status
  unsigned short richRunTag;
  vector<unsigned short> v_pmt_stat, v_pmt_volt;

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
  RichConfigContainer(){};

  void getFromRichPMTCalib(RichPMTCalib *calib, int run);
  bool dumpToRichPMTCalib(RichPMTCalib &calib);

  ClassDef(RichConfigContainer,1);
};


class RichConfigManager{
 public:
  static bool UpdateParameters(int run,int time,TString dir=""); 
  static RichConfigContainer richConfigContainer;
  static int currentRun;
  static TString defaultDir;
  static bool useExternalFiles;
  static bool reloadTemperatures;
  static bool reloadRunTag;
  static bool reloadPMTs;

  static bool loadPmtCorrections; 
  static TString correctionsDir;
  static bool  useRichRunTag;
  static bool  usePmtStat;
  static bool  useSignalMean;
  static bool  useGainCorrections;
  static bool  useEfficiencyCorrections;
  static bool  useBiasCorrections;
  static bool  useTemperatureCorrections;
};

#endif
