#include "BadRunManager.hh"

#include <assert.h>
#include <stdlib.h>
#include <fstream>
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <fstream>

#include "Event.h"
#include "AnalysisParticle.hh"
#include "StringTools.hh"

#define DEBUG 0
#define INFO_OUT_TAG "BadRunManager> "
#include <debugging.hh>


ACsoft::Analysis::BadRunManager::BadRunManager() {

  // initialize index map from enum SubD:
  SubDNameToEnum["General"] = General;
  SubDNameToEnum["DAQ"]     = DAQ    ;
  SubDNameToEnum["TRD"]     = TRD    ;
  SubDNameToEnum["TOF"]     = TOF    ;
  SubDNameToEnum["ACC"]     = ACC    ;
  SubDNameToEnum["Tracker"] = Tracker;
  SubDNameToEnum["Rich"]    = Rich   ;
  SubDNameToEnum["Ecal"]    = Ecal   ;

  EnumToSubDName[General] = "General";
  EnumToSubDName[DAQ]     = "DAQ"    ;
  EnumToSubDName[TRD]     = "TRD"    ;
  EnumToSubDName[TOF]     = "TOF"    ;
  EnumToSubDName[ACC]     = "ACC"    ;
  EnumToSubDName[Tracker] = "Tracker";
  EnumToSubDName[Rich]    = "Rich"   ;
  EnumToSubDName[Ecal]    = "Ecal"   ;

  fCurrentRun   = 0;
  fCurrentUTime = 0;

  const char* acrootsoft = getenv("ACROOTSOFTWARE");
  if( !acrootsoft ){
    WARN_OUT << "ACROOTSOFTWARE variable not set!" << std::endl;
    throw std::runtime_error("ACROOTSOFTWARE variable not set!");
  }

  fBadRunFilesDir = std::string(acrootsoft) + std::string("/acroot/data/BadRuns/");

  for(int iSubD=0; iSubD<nSubD; iSubD++)  BadRuns[iSubD].clear();

  AddDefaultBadRunLists();
}

std::string ACsoft::Analysis::BadRunManager::SubdToString( SubD s ) const
{
  std::map<SubD,std::string>::const_iterator it = EnumToSubDName.find(s);
  assert(it != EnumToSubDName.end());
  return it->second;
}

ACsoft::Analysis::BadRunManager::SubD ACsoft::Analysis::BadRunManager::StringToSubd( std::string s ) const
{
  std::map<std::string,SubD>::const_iterator it = SubDNameToEnum.find(s);
  assert(it != SubDNameToEnum.end());
  return it->second;
}

void ACsoft::Analysis::BadRunManager::AddDefaultBadRunLists() {

  INFO_OUT << "Adding default run lists..." << std::endl;

//  AddBadRunList("General_ShortRun_1305853512:1347160405");
  AddBadRunList("General_commissioning_1305853512:1337244535");
//  AddBadRunList("General_desync_1305853512:1337244535");
//  AddBadRunList("General_events_with_error_1305853512:1347158991");
//  AddBadRunList("General_events_without_particle_1305853512:1347158991");
//  AddBadRunList("General_missing_events_1305853512:1347158991");
//  AddBadRunList("General_not_vertical_1305853512:1347158991");

  AddBadRunList("DAQ_pulser_on_1305853512:1337244535");

  AddBadRunList("Tracker_BadStrips_1305853512:1347160405");

  AddBadRunList("Ecal_E0_event_mismatch_1305853512:1337449847");
//  AddBadRunList("Ecal_EDR_1_0_A_wrong_config_1305853512:1337449847");
//  AddBadRunList("Ecal_EIB_RP3_LV_power_cycle_1305853512:1337449847");
//  AddBadRunList("Ecal_EIB_RP3_wrong_setting_1305853512:1337449847");
//  AddBadRunList("Ecal_Trigger_test_1305853512:1337449847");

//  AddBadRunList("TRD_BadForEPseparation_1305800727:1354552232");
  AddBadRunList("TRD_UnusableForAnalysis_1305800727:1354552232");
//  AddBadRunList("TRD_WeakForEPseparation_1305800727:1354552232");
//  AddBadRunList("TRD_Test");
}


/** Read a list of bad runs.
  *
  * \param[in] fileName bad run list file (WITHOUT DIRECTORY)
  */
int ACsoft::Analysis::BadRunManager::AddBadRunList( const std::string& fileName ){

  int nBadRuns = -1;
  std::string fnam = fBadRunFilesDir + fileName;

  std::ifstream BadRunFile;

  BadRunFile.open(fnam.c_str());
  if(BadRunFile){
    nBadRuns = 0;
    std::string SubD = split(fileName,"_")[0]; // this assumes that sub-detector is first token before '_' in filename
    if(SubDNameToEnum.find(SubD)!=SubDNameToEnum.end()){
      // read BadRunFile line-by-line
      std::string line;
      size_t k;
      while(!BadRunFile.eof()){
        getline(BadRunFile, line);
        k = line.find("#");
        line = line.substr(0,k); // remove comments
        if(line.size()>1){       // we should have the line containing the comma-separated BadRun entries
          std::vector<std::string> BadRunEntries = split(line,",");
          for(int i=0; i<(int)BadRunEntries.size();i++){
            std::vector<std::string> BadRunEntry = split(BadRunEntries[i],"_");
            long UT0=0,UT1=0;
            if( BadRunEntry.size()==1 ){        // whole run bad
              UT0=1000000000; //FIXME why not 0?
              UT1=3000000000; //FIXME why not long_max ?
            }
            else if( BadRunEntry.size()==2 ){  // extended entry with start- and end-time: RunID_UT0:UT1
              std::vector<std::string> UTimeRange = split(BadRunEntry[1],":");
              UT0 = atol(UTimeRange[0].c_str());
              UT1 = atol(UTimeRange[1].c_str());
            }
            else WARN_OUT << "ERROR: wrong BadRunEntry format " << BadRunEntries[i] << std::endl;

            // add (UT0,UT1) to run-entry for this SubD - will add new run-entry to map if not yet found
            if( UT0>0 && UT1>0 ){
              BadRuns[SubDNameToEnum[SubD]][atol(BadRunEntry[0].c_str())].push_back(std::make_pair(UT0,UT1));
              nBadRuns++;
              char debugstring[1000];
              sprintf(debugstring,"%10s %-32s %10li %10li %10li",SubD.c_str(),BadRunEntries[i].c_str(),atol(BadRunEntry[0].c_str()),UT0,UT1);
              DEBUG_OUT << debugstring << std::endl;
            }
          }
        }
      }
    }
    else{
      WARN_OUT << "ERROR: Unknown SubD " << SubD << std::endl;
      return 0;
    }
  }
  else{
    WARN_OUT << "ERROR: file not found: " << fnam << std::endl;
    return 0;
  }

  INFO_OUT << "Add BadRunList " << std::setw(80) << std::left << fileName
           << " BadRuns " << std::setw(6) << std::right << nBadRuns << std::endl;
  return nBadRuns;
}



bool ACsoft::Analysis::BadRunManager::IsBad( SubD subdetector, const ACsoft::Analysis::Particle& particle ){

  if( !particle.HasRawEventData() )
    return particle.BadRunTag() & 1 << int(subdetector);

  long Run = particle.RawEvent()->RunHeader()->Run();
  time_t UTime = particle.RawEvent()->EventHeader().TimeStamp().GetSec();

  return IsBad(subdetector,Run,UTime);
}


bool ACsoft::Analysis::BadRunManager::IsBad( SubD subdetector, long run, time_t UTime ){

  if( run   != fCurrentRun )   UpdateBadUTimes(run);   // get new bad-utime-ranges for this run for all SubDs
  if( UTime != fCurrentUTime ) UpdateBadStatus(UTime); // get bad-status for this UTime second  for all SubDs

  return BadStatus[subdetector];
}


void ACsoft::Analysis::BadRunManager::UpdateBadStatus( const time_t UTime ){

  for( int iSubD=0; iSubD<nSubD; iSubD++){
    BadStatus[iSubD] = false;
    for(int i=0; i<(int)BadUTimes[iSubD].size(); i++){
      if(    (UTime >= BadUTimes[iSubD][i].first)
             && (UTime <= BadUTimes[iSubD][i].second) ){
        BadStatus[iSubD] = true; // UTime inside bad-utime-range for this SubD
        continue;
      }
    }
    DEBUG_OUT_L(2) << "UTime=" << UTime
                   << " subD " << std::setw(10) << std::left << EnumToSubDName[ACsoft::Analysis::BadRunManager::SubD(iSubD)]
                   << " status " << BadStatus[iSubD] << std::endl;
  }

  fCurrentUTime = UTime;
}


void ACsoft::Analysis::BadRunManager::UpdateBadUTimes( const long Run ){

  std::map<long,BadUTimeRangeVector>::iterator it;

  for( int iSubD=0; iSubD<nSubD; iSubD++){

    BadUTimes[iSubD].clear();

    if( (it=BadRuns[iSubD].find(Run)) != BadRuns[iSubD].end() ) BadUTimes[iSubD] = (*it).second;

    // status message
    if(DEBUG){
      DEBUG_OUT << std::endl;
      printf("BadRunManager::UpdateBadUTimes(  Run=%10li) %10s ", Run,EnumToSubDName[ACsoft::Analysis::BadRunManager::SubD(iSubD)].c_str());
      if(BadUTimes[iSubD].size()>0) printf("%10li %10li\n",BadUTimes[iSubD][0].first,BadUTimes[iSubD][0].second);
      else printf("\n");
      for(int i=1; i<(int)BadUTimes[iSubD].size(); i++)
        printf("                                                          %10li %10li\n",BadUTimes[iSubD][0].first,BadUTimes[iSubD][0].second);
    }
  }

  fCurrentRun = Run;
}
