#include "RichConfig.h"
#include "TChainElement.h"
#include "TSystem.h"
#include "root_setup.h"
#include "RichCharge.h"
#include <iostream>
#include <algorithm>
#include "math.h"
#include "limits.h"
#include "stdlib.h"
#include "root.h"
#include <map>
#ifdef __ROOTSHAREDLIBRARY__
#include "amschain.h"
#endif
#include "commonsi.h"

ClassImp(RichConfigContainer);


//RichConfigContainer::RichConfigContainer(RichPMTCalib &calib){
void RichConfigContainer::getFromRichPMTCalib(RichPMTCalib *calib, int run){

  if (!calib) {
    cout << "RichConfigContainer::getFromRichPMTCalib-E-NULLPointer" << endl;
    return;
  }

  runID = run;

#define C(_x) _x=calib->_x
  {
    C(pmtTemperatures);
    C(v_pmt_temp);
    C(brickTemperatures);
    C(v_brick_temp);
  }

  {
    C(richRunTag);
    C(v_pmt_stat);
    C(v_pmt_volt);
  }

  {
    C(v_pmt_rdr);
    C(v_pmt_brick);
    C(v_pmt_hvchn);
    C(v_pmt_vnom);

    C(m_pmt_periods);
    C(m_pmt_voltages);
    C(v_pmt_dGdT);
    C(v_pmt_dEdT);

    C(v_pmt_temp_ref);
    C(v_pmt_ecor_dflt);
    C(v_pmt_gcor_dflt);
    C(v_pmt_gmcor_dflt);
    C(v_pmt_temp_ref_dflt);

    C(v_pmt_bcor);

    C(v_pmt_ecor);
    C(v_pmt_gcor);
    C(v_pmt_gmcor);
    C(BadPMTs);
  }
#undef C
}


bool RichConfigContainer::dumpToRichPMTCalib(RichPMTCalib &calib){

#define C(_x) calib._x=_x
  if (!RichConfigManager::reloadTemperatures) {
    C(pmtTemperatures);
    C(v_pmt_temp);
    C(brickTemperatures);
    C(v_brick_temp);
  }

  if (!RichConfigManager::reloadRunTag) {
    C(richRunTag);
    C(v_pmt_stat);
    C(v_pmt_volt);
  }

  if (!RichConfigManager::reloadPMTs) {
    C(v_pmt_rdr);
    C(v_pmt_brick);
    C(v_pmt_hvchn);
    C(v_pmt_vnom);

    C(m_pmt_periods);
    C(m_pmt_voltages);
    C(v_pmt_dGdT);
    C(v_pmt_dEdT);

    C(v_pmt_temp_ref);
    C(v_pmt_ecor_dflt);
    C(v_pmt_gcor_dflt);
    C(v_pmt_gmcor_dflt);
    C(v_pmt_temp_ref_dflt);

    C(v_pmt_bcor);

    C(v_pmt_ecor);
    C(v_pmt_gcor);
    C(v_pmt_gmcor);
    C(BadPMTs);
  }

#undef C

  // Reload needed information
  return calib.Reload();
}


RichConfigContainer RichConfigManager::richConfigContainer;
int RichConfigManager::currentRun=-1;
TString RichConfigManager::defaultDir="";
bool RichConfigManager::useExternalFiles=false;  // Set to true to use external files instead of the info stored int the root file
bool RichConfigManager::reloadTemperatures = false; // force load Temperatures from AMSSetup
bool RichConfigManager::reloadRunTag = false;       // force load Config & Status from ext. files
bool RichConfigManager::reloadPMTs = false;         // force load PMT info from ext. files

bool RichConfigManager::loadPmtCorrections=true; 

// Rich Charge Corrections Settings & Flags
TString RichConfigManager::correctionsDir="";  // Directory
bool RichConfigManager::useRichRunTag = false;           // good Rich runs 
bool RichConfigManager::usePmtStat = true;               // good PMT status
bool RichConfigManager::useSignalMean = false;           // Equalize PMT Gains to mean (median)
bool RichConfigManager::useGainCorrections = true;       // PMT Gain equalization
bool RichConfigManager::useEfficiencyCorrections = true; // PMT Efficiency equalization
bool RichConfigManager::useBiasCorrections = true;       // PMT Efficiency bias corrections
bool RichConfigManager::useTemperatureCorrections = true;// PMT Temperature corrections

bool RichConfigManager::UpdateParameters(int run,int time,TString dir) {

  // If MC return return true;

  // If loadPMTcorrections==false return true;

  // If it is data, check that the required tools are OK
  correctionsDir=dir;
  RichPMTCalib::currentDir=correctionsDir;
  RichPMTCalib::useRichRunTag = useRichRunTag;
  RichPMTCalib::usePmtStat = usePmtStat;
  RichPMTCalib::useSignalMean = useSignalMean;
  RichPMTCalib::useGainCorrections = useGainCorrections;
  RichPMTCalib::useEfficiencyCorrections = useEfficiencyCorrections;
  RichPMTCalib::useBiasCorrections = useBiasCorrections;
  RichPMTCalib::useTemperatureCorrections = useTemperatureCorrections;

  RichPMTCalib *corr=RichPMTCalib::Update(run);
  if(!corr) return false;
  richConfigContainer.getFromRichPMTCalib(corr,run);
  return true;
}



