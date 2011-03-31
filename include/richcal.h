#ifndef __RICHCAL__
#define __RICHCAL__

#include <assert.h>
#include <string.h>
#include <math.h>
#include "timeid.h"
#include "richid.h"
#include "trigger102.h"
#include "richrec.h"
#include "particle.h"
#include "TH1F.h"
#include <vector.h>


class AMSRichCal{
  //
  // Refractive index calibration variables
  //
  static TH1F   betaHisto[RICmaxtiles]; // Histograms for beta calibration
  static double betaCalibration[RICmaxtiles];
  static int    betaPeakEvents;
  static double betaWindowWidth;
  static int    betaBins;
  static double betaLearningFactor;
  static int    last_run;
  static int    last_time;

  //
  // Gain calibration variables
  //
  static vector<double> signal[RICmaxpmts*RICnwindows];
  static double signalCalibration[RICmaxpmts*RICnwindows];
  static int          gainEvents;
  static double       gainLearningFactor;


  static AMSRichRing *event_selection();

  static double computeMedian(vector<double> &v);
  static double computePeakPos(TH1F &h);


  // Come cuts utilities
  static map<string,unsigned int>   cuts_entries; // Entries for a given cut
  static vector<string>             cuts_order;
  static vector<bool>               cuts_isMask; 
  static map<string,bool>           cuts_passed; // Cuts passed for a single event
  static map<string,bool>::iterator cuts_it;      // Helper
  static bool select(string name,bool condition,bool isMask=false);
  static void reportCuts();
  static void resetCuts();
  static void initCuts();


  // Monitoring and calibration parameters
  static bool         _calibration;
  static float        _charge_lower_threshold;
  static float        _charge_upper_threshold;  
  static float        _momentum_threshold; 
  static float        _momentum_gain_threshold; 
  static bool         _init;

 public:
  static int isCalibration(){return _calibration;}
  static void finish();  // Save the calibration and free memory 
  static void init(int bins=1000,float minx=0,float maxx=5.);  // Number of bins per histogram
  static void process_event();  // Perform all the job
};



#endif
