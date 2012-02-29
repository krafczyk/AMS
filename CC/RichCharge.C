#include "RichCharge.h"
#include "root.h"
#include "root_setup.h"
#include <iostream>
#include <fstream>
#include "math.h"


using namespace std;
using namespace RichPMTCalibConstants;


// Singleton pointer
RichPMTCalib* RichPMTCalib::_header=NULL;

// Directory where the input files are 
TString RichPMTCalib::currentDir=".";

// General Settings
bool RichPMTCalib::useRichRunTag = true;
bool RichPMTCalib::usePmtStat = true;
bool RichPMTCalib::useGainCorrections = false;
bool RichPMTCalib::useEfficiencyCorrections = true;
bool RichPMTCalib::useTemperatureCorrections = true; 
unsigned short RichPMTCalib::richRunBad = (1<<RichPMTCalib::kTagJ) | (1<<RichPMTCalib::kTagR); //  JINFR & RDR Configuration  
unsigned short RichPMTCalib::richPmtBad = (1<<RichPMTCalib::kPmtFE) | (1<<RichPMTCalib::kPmtHV) | (1<<RichPMTCalib::kPmtJ) | (1<<RichPMTCalib::kPmtR); //  FE On & HV Nominal & No Config Err/Mismatch 

bool RichPMTCalib::Init(TString dir){
  if(_header) delete _header;
  currentDir=dir;
  _header=new RichPMTCalib();
  return _header->init();
}

RichPMTCalib* RichPMTCalib::Update(){
  if(!RichPMTCalib::getHead() &&!RichPMTCalib::Init(currentDir)) return 0;
  if(!RichPMTCalib::getHead()->retrieve(AMSEventR::Head()->fHeader.Run)) return 0;
  return RichPMTCalib::getHead();
}

bool RichPMTCalib::init(){
  // Read Pmt DB
  ReadPmtDB();

  // Init PMTs List
  if (!initPMTs()) return false;
  //initBadPMTs(dir);
  return true;
}


bool RichPMTCalib::retrieve(int run){
  // retrieve the information for the given run
  static int prevRun=0;
  static bool retValue=true;
  if(run==prevRun) return retValue;
  prevRun=run;

  richRunTag=CheckRichRun(run, v_pmt_stat, v_pmt_volt);
  pmtTemperatures=getRichPmtTemperatures();
  brickTemperatures=getRichBrickTemperatures();

  if(!pmtTemperatures || !brickTemperatures) retValue=false;
  if(!richRunGood()) retValue=false;
  
  return retValue;
}


float RichPMTCalib::EfficiencyCorrection(int pmt) {

  float efficiencyCorrection = useEfficiencyCorrections? v_pmt_ecor[pmt] : 1;

  efficiencyCorrection *= usePmtStat? richPmtGood(pmt) : 1; // here we should consider non-nominal HV
                                                            // by adding dG = Go * k * dV/Vo (??)

  return efficiencyCorrection;

}


float RichPMTCalib::GainCorrection(int pmt) {

  //float gainCorrection = useGainCorrections? v_pmt_gcor[pmt] : 1; // USE CORR. FROM MEAN
  float gainCorrection = useGainCorrections? v_pmt_gmcor[pmt] : 1; // USE CORR. FROM MEDIAN

  gainCorrection *= usePmtStat? richPmtGood(pmt) : 1; // here we should consider non-nominal HV
                                                      // by adding dG = Go * k * dV/Vo


  return gainCorrection;

}


float RichPMTCalib::TemperatureCorrection(int pmt) {

  float temperatureCorrection = 1;

  float dG = 0;
  if (useTemperatureCorrections && pmtTemperatures) {
    float temp = v_pmt_temp[pmt];
    float temp_ref = v_pmt_temp_ref[pmt];
    float dGdT = v_pmt_dGdT[pmt];
    if (temp>pmtMinTemperature && temp<pmtMaxTemperature)
      dG = (temp - temp_ref) * dGdT;
  }

  temperatureCorrection = 1 + dG;

  return temperatureCorrection;

}


bool RichPMTCalib::initPMTs() {

  bool initPMTs = false;
  int pmt, npmt;

  TString DBDir=currentDir+TString("/RichPmtCorrections/");
  TString PmtCorrectionsFileName;
  ifstream PmtCorrectionsFile;

  //
  // Gain Temperature Corrections (Computed on PeriodA...should be validated for others)
  PmtCorrectionsFileName = DBDir + TString("DefaultGainTemp.txt");
  cout << "RichPMTCalib::initPMTs: Open PMT Gain Temperature Corrections File " << PmtCorrectionsFileName << endl;
  PmtCorrectionsFile.open(PmtCorrectionsFileName);
  if (!PmtCorrectionsFile.good()) {
    cout << "RichPMTCalib::initPMTs: Problems Opening " << PmtCorrectionsFileName << endl;
    return initPMTs;
  }

  v_pmt_dGdT.assign(NPMT, pmtRefdGdT);
  npmt = 0;
  float dGdT, dGdTe, off, offe, tlow, thgh, chi2;
  int nbn;
  while ( PmtCorrectionsFile >> pmt >> dGdT >> dGdTe >> off >> offe >> tlow >> thgh >> nbn >> chi2 ) {

    if (pmt<NPMT) {
      npmt++;
      if (dGdT<0) v_pmt_dGdT[pmt] = dGdT;
    }
    else {
      cout << "RichPMTCalib::initPMTs: Error in " << PmtCorrectionsFileName 
	   << ". pmt: " << pmt << " dGdT: " << dGdT << endl;
      return initPMTs;
    }

  }

  PmtCorrectionsFile.close();

  cout << "RichPMTCalib::initPMTs: Number of PMT Gain Temperature Corrections Read : " << npmt << endl;
  if (npmt != NPMT) {
    cout << "RichPMTCalib::initPMTs: Error Expected PMT Corrections : " << NPMT << endl;
    return initPMTs;
  }

  //
  // Efficiency & Gain Corrections (Computed on PeriodB...should be validated for others)
  PmtCorrectionsFileName = DBDir + TString("DefaultEffGain.txt");
  cout << "RichPMTCalib::initPMTs: Open PMT Eff & Gain Corrections File " << PmtCorrectionsFileName << endl;
  PmtCorrectionsFile.open(PmtCorrectionsFileName);
  if (!PmtCorrectionsFile.good()) {
    cout << "RichPMTCalib::initPMTs: Problems Opening " << PmtCorrectionsFileName << endl;
    return initPMTs;
  }

  v_pmt_ecor.assign(NPMT, 1);
  v_pmt_gcor.assign(NPMT, 1);
  v_pmt_gmcor.assign(NPMT, 1);
  v_pmt_temp_ref.assign(NPMT, pmtRefTemperature);
  npmt = 0;
  float eCor, gCor, gmCor, temp, dtemp;
  while ( PmtCorrectionsFile >> pmt >> eCor >> gCor >> gmCor >> temp >> dtemp ) {

    if (pmt<NPMT) {
      npmt++;
      v_pmt_ecor[pmt] = eCor;
      v_pmt_gcor[pmt] = gCor;
      v_pmt_gmcor[pmt] = gmCor;
      if (temp>pmtMinTemperature && temp<pmtMaxTemperature)
	v_pmt_temp_ref[pmt] = temp;
      //cout << v_pmt_ecor[pmt] << " " << v_pmt_gcor[pmt] << " " << v_pmt_gmcor[pmt] << " " << v_pmt_temp_ref[pmt] << endl;
    }
    else {
      cout << "RichPMTCalib::initPMTs: Error in " << PmtCorrectionsFileName 
	   << ". pmt: " << pmt << " eCor: " << eCor << " gCor: " << gCor << " gmCor: " << gmCor << endl;
      return initPMTs;
    }

  }

  PmtCorrectionsFile.close();

  cout << "RichPMTCalib::initPMTs: Number of PMT Eff & Gain Corrections Read : " << npmt << endl;
  if (npmt != NPMT) {
    cout << "RichPMTCalib::initPMTs: Error Expected PMT Corrections : " << NPMT << endl;
    return initPMTs;
  }

  // Fill in List of BadPMTs
  BadPMTs.clear();
  for (int i=0; i<NPMT; i++)
    if (v_pmt_ecor[i] == 0) BadPMTs.push_back(i);

  if (DEBUG)
  {
    cout << "RichPMTCalib::initPMTs :  declared Bad PMTs = " << BadPMTs.size() << endl;
    for (int i=0; i<BadPMTs.size(); i++) 
      cout << "RichPMTCalib::initPMTs :  " << i << " : " << BadPMTs[i] << endl;
  }

  initPMTs = true;

  return initPMTs;
}


void RichPMTCalib::initBadPMTs() {

  // List of BadPMTs ( should be extended to pmt, begin_time, end_time)
  int badpmts[] = {
    // Grid A
     29,  43, 
    // Grid B
    // Grid C
    205, 225, 305, 
    // Grid D
    323,
    // Grid E
    358, 366, 373, 439, 446, 458, 466, 473, 
    // Grid F
    493, 
    // Grid G 
    510, 511, 512, 513, 514, 515, 628, 647, 652,
    // Grid H
    664 
  };

  /*
  PMTs 1-50% bad : 6
    pmt : 121 0.09
    pmt : 123 0.09
    pmt : 125 0.09
    pmt : 127 0.09
    pmt : 452 0.02
    pmt : 460 0.15
  */

  BadPMTs.clear();
  for (int i=0; i<sizeof(badpmts)/sizeof(int); BadPMTs.push_back(badpmts[i++]));

  if (DEBUG)
  {
    cout << "RichPMTCalib::initBadPMTs :  declared Bad PMTs = " << BadPMTs.size() << endl;
    for (int i=0; i<BadPMTs.size(); i++) 
      cout << "RichPMTCalib::initBadPMTs :  " << i << " : " << BadPMTs[i] << endl;
  }

}



bool RichPMTCalib::checkRichPmtTemperatures() {

  int dts, id, pos, pmt, brick, grid, utime, age;
  float x, y, temp;

  static bool init = true;
  static bool initok = false;

  const int NDTS = 48, NPMT = 680;

  static vector<int>   v_pmt_dts;
  static vector<int>   v_utime;
  static vector<float> v_temp[NDTS];

  static int last_rec = -1, prev_rec = -1;

  vector<float> v_dts_x;
  vector<float> v_dts_y;
  vector<int>   v_dts_g;

  vector<float> v_pmt_x;
  vector<float> v_pmt_y;
  vector<int>   v_pmt_g;
  vector<int>   v_pmt_b;

  bool checkRichPmtTemperatures = false;

  if (init) {

    init = false;

    v_pmt_temp.assign(NPMT, 25); // Default PMT Temperatures

    TString DBDir=currentDir+("/RichTemperatures/");
    TString DtsPositionsFileName, PmtPositionsFileName, TemperaturesFileName;

    // DTS Positions
    DtsPositionsFileName = DBDir + TString("PmtPlaneDtsPositions.0.txt");
    cout << "RichPMTCalib::checkRichPmtTemperatures: Open DTS Positions File " << DtsPositionsFileName << endl;
    ifstream DtsPositionsFile(DtsPositionsFileName);
    if (!DtsPositionsFile.good()) {
      cout << "RichPMTCalib::checkRichPmtTemperatures: Problems Opening " << DtsPositionsFileName << endl;
      return checkRichPmtTemperatures;
    }

    v_dts_g.assign(NDTS, 0);
    v_dts_x.assign(NDTS, 0);
    v_dts_y.assign(NDTS, 0);
    int ndts = 0;
    while ( DtsPositionsFile >> dts >> id >> pos >> x >> y ) {

      if (dts<NDTS) {
	ndts++;
	v_dts_g[dts] = (id/100);
	v_dts_x[dts] = x;
	v_dts_y[dts] = y;
      }
      else {
	cout << "RichPMTCalib::checkRichPmtTemperatures: Error in DTS Positions File. " 
	     << "dts: " << dts << " id: " << id << " pos: " << pos << " x: " << x << " y: " << y << endl;
	return checkRichPmtTemperatures;
      }

    }

    DtsPositionsFile.close();

    cout << "RichPMTCalib::checkRichPmtTemperatures: Number of DTS Positions Read : " << ndts << endl;
    if (ndts != NDTS) {
      cout << "RichPMTCalib::checkRichPmtTemperatures: Error Expected DTS Positions : " << NDTS << endl;
      return checkRichPmtTemperatures;
    }

    // PMT Positions
    PmtPositionsFileName = DBDir + TString("PmtPositions.txt");
    cout << "RichPMTCalib::checkRichPmtTemperatures: Open PMT Positions File " << PmtPositionsFileName << endl;
    ifstream PmtPositionsFile(PmtPositionsFileName);
    if (!PmtPositionsFile.good()) {
      cout << "RichPMTCalib::checkRichPmtTemperatures: Problems Opening " << PmtPositionsFileName << endl;
      return checkRichPmtTemperatures;
    }

    v_pmt_b.assign(NPMT, 0);
    v_pmt_g.assign(NPMT, 0);
    v_pmt_x.assign(NPMT, 0);
    v_pmt_y.assign(NPMT, 0);
    int npmt = 0;
    while ( PmtPositionsFile >> pmt >> brick >> grid >> x >> y ) {

      if (pmt<NPMT) {
	npmt++;
	v_pmt_b[pmt] = brick;
	v_pmt_g[pmt] = grid;
	v_pmt_x[pmt] = x;
	v_pmt_y[pmt] = y;
      }
      else {
	cout << "RichPMTCalib::checkRichPmtTemperatures: Error in PMT Positions File. " 
	     << "pmt: " << pmt << " brick : " << brick << " grid : " << grid
	     << " x: " << x << " y: " << y << endl;
	return checkRichPmtTemperatures;
      }

    }

    PmtPositionsFile.close();

    cout << "RichPMTCalib::checkRichPmtTemperatures: Number of PMT Positions Read : " << npmt << endl;
    if (npmt != NPMT) {
      cout << "RichPMTCalib::checkRichPmtTemperatures: Error Expected PMT Positions : " << NPMT << endl;
      return checkRichPmtTemperatures;
    }

    // PMT - DST Mapping
    v_pmt_dts.assign(NPMT, 0);

    for (int i=0; i<NPMT; i++) {
      int dts=-1;
      float d2m=1e6;
      for (int j=0; j<NDTS; j++) {
	if (v_dts_g[j] == v_pmt_g[i]) {
	  float dx, dy, d2;
	  dx = v_dts_x[j] - v_pmt_x[i];
	  dy = v_dts_y[j] - v_pmt_y[i];
          d2 = dx*dx + dy*dy;
          if (d2 < d2m) {
	    d2m = d2;
	    dts = j;
	  }
	}
      }

      if (dts>=0) v_pmt_dts[i] = dts; 
      else {
	cout << "RichPMTCalib::checkRichPmtTemperatures: Error in PMT-DST Mapping : PMT " << i << endl;
	return checkRichPmtTemperatures;
      }
    }

    // DTS Temperatures - File size still affordable ...
    TemperaturesFileName = DBDir + TString("PmtPlaneTemperatures.0.txt");
    cout << "RichPMTCalib::checkRichPmtTemperatures: Open DTS Temperatures File " << TemperaturesFileName << endl;
    ifstream TemperaturesFile(TemperaturesFileName);
    if (!TemperaturesFile.good()) {
      cout << "RichPMTCalib::checkRichPmtTemperatures: Problems Opening " << TemperaturesFileName << endl;
      return checkRichPmtTemperatures;
    }


    while ( TemperaturesFile >> utime ) {

      v_utime.push_back(utime);

      ndts = 0;
      while ( ndts++<NDTS && TemperaturesFile >> dts >> temp >> age ) {

	if (dts<NDTS) v_temp[dts].push_back(temp);
	else {
	cout << "RichPMTCalib::checkRichPmtTemperatures: Error in DTS Temperatures File. " 
	     << "utime: " << utime << " dts: " << dts << " temp: " << temp << " age: " << age << endl;
	  return checkRichPmtTemperatures;
	}

      }

    }

    TemperaturesFile.close();

    cout << "RichPMTCalib::checkRichPmtTemperatures: Number of Temperature Records Read : " << v_utime.size() << endl;
    if (!v_utime.size()) 
      return checkRichPmtTemperatures;

    initok = true;

    last_rec = 0;
  }


  if (!initok) return checkRichPmtTemperatures;


  // Do the logic here ....

  utime = AMSEventR::Head()->fHeader.Time[0];

  if (utime>=v_utime[last_rec] && utime < v_utime[last_rec+1]) {
    checkRichPmtTemperatures = true;
    return checkRichPmtTemperatures; 
  }
  else if (utime>=v_utime[last_rec+1]) {
    while (++last_rec<v_utime.size()-1 && utime>=v_utime[last_rec+1]) {};
    if (last_rec<v_utime.size()-1)
      checkRichPmtTemperatures = true;
  }
  else {
    while (--last_rec>-1 && utime<v_utime[last_rec]) {};
    if (last_rec>=0) 
      checkRichPmtTemperatures = true;
    else
      last_rec = 0;
  }

  if (last_rec != prev_rec) {
    cout << "RichPMTCalib::checkRichPmtTemperatures: NewRecord found for time " << utime 
         << ". Record Utime " << v_utime[last_rec]
	 << " Temp[ 0] " << v_temp[ 0][last_rec] << " Temp[ 9] " << v_temp[ 9][last_rec] 
	 << " Temp[12] " << v_temp[12][last_rec] << " Temp[21] " << v_temp[21][last_rec] 
	 << " Temp[24] " << v_temp[25][last_rec] << " Temp[33] " << v_temp[34][last_rec] 
	 << " Temp[36] " << v_temp[25][last_rec] << " Temp[45] " << v_temp[34][last_rec] << endl; 
    prev_rec = last_rec;
  }

  // PMT-DTS temperature map
  if (checkRichPmtTemperatures) {
    for (int pmt=0; pmt<NPMT; pmt++) {
      dts = v_pmt_dts[pmt];
      v_pmt_temp[pmt] = v_temp[dts][last_rec];
      if (DEBUG)
	cout << "RichPMTCalib::checkRichPmtTemperatures: PMT " << pmt << " (T=" << v_pmt_temp[pmt] << ")" <<  endl;
      if (v_pmt_temp[pmt]<pmtMinTemperature || v_pmt_temp[pmt]>pmtMaxTemperature) 
	cout << "RichPMTCalib::checkRichPmtTemperatures: Temperature Alarm on PMT " << pmt << " (T=" << v_pmt_temp[pmt] << ")" << endl;
    }
  }
  else v_pmt_temp.assign(NPMT, pmtRefTemperature); // Default

  //cout << " utime " << utime << " last_rec " << last_rec << " Temp[0] " << v_pmt_temp[0] << " Temp[679] " << v_pmt_temp[679] << endl;

  return checkRichPmtTemperatures;

}


bool RichPMTCalib::getRichPmtTemperatures() {

  int dts, id, pos, pmt, brick, grid, utime, rc;
  float x, y, dx, dy, d2;
  string element, node;

  static bool init = true;
  static bool initok = false;

  const int NDTS = 96, NPMT = 680;

  const int MAX_REP = 5;
  static vector<int> v_pmt_rep;

  static vector<string> v_dts_el;
  static vector<string> v_dts_nn;
  static multimap<float,int> m_pmt_d2_dts[NPMT];

  multimap<float,int>::iterator it;

  vector<float>  v_dts_x;
  vector<float>  v_dts_y;
  vector<int>    v_dts_g;

  vector<float> v_pmt_x;
  vector<float> v_pmt_y;
  vector<int>   v_pmt_g;
  vector<int>   v_pmt_b; 

  vector<float> value;
  vector<float> v_dts_temp;

  bool RichPmtTemperatures = false;

  if (init) {

    init = false;

    v_pmt_temp.assign(NPMT, pmtRefTemperature); // Default PMT Temperatures
    v_pmt_rep.assign(NPMT, 0);

    TString DBDir=currentDir+TString("/RichTemperatures/");
    TString DtsPositionsFileName, PmtPositionsFileName;

    // DTS Positions
    DtsPositionsFileName = DBDir + TString("PmtPlaneDtsPositions.txt");
    cout << "RichPMTCalib::getRichPmtTemperatures: Open DTS File " << DtsPositionsFileName << endl;
    ifstream DtsPositionsFile(DtsPositionsFileName);
    if (!DtsPositionsFile.good()) {
      cout << "RichPMTCalib::getRichPmtTemperatures: Problems Opening " << DtsPositionsFileName << endl;
      return RichPmtTemperatures;
    }

    v_dts_g.assign(NDTS, 0);
    v_dts_x.assign(NDTS, 0);
    v_dts_y.assign(NDTS, 0);
    v_dts_el.assign(NDTS, "");
    v_dts_nn.assign(NDTS, "");
    int ndts = 0;
    while ( DtsPositionsFile >> dts >> id >> pos >> element >> node >> x >> y ) {

      if (dts<NDTS) {
	ndts++;
	v_dts_g[dts] = (id/100);
	v_dts_x[dts] = x;
	v_dts_y[dts] = y;
      	v_dts_el[dts] = element;
	v_dts_nn[dts] = node;
	//cout << " DTS " << dts << " GRID " << v_dts_g[dts]
	//     << " X " << v_dts_x[dts] << " Y " << v_dts_y[dts]
	//     << " " << v_dts_el[dts] << " " << v_dts_nn[dts] << endl;
      }
      else {
	cout << "RichPMTCalib::getRichPmtTemperatures: Error in DTS File. " 
	     << "dts: " << dts << " id: " << id 
	     << " pos: " << pos << " x: " << x << " y: " << y 
	     << " element:  " << element << " node: " << node << endl;
	return RichPmtTemperatures;
      }

    }

    DtsPositionsFile.close();

    cout << "RichPMTCalib::getRichPmtTemperatures: Number of DTS Positions Read : " << ndts << endl;
    if (ndts != NDTS) {
      cout << "RichPMTCalib::getRichPmtTemperatures: Error Expected DTS Positions : " << NDTS << endl;
      return RichPmtTemperatures;
    }

    // PMT Positions
    PmtPositionsFileName = DBDir + TString("PmtPositions.txt");
    cout << "RichPMTCalib::getRichPmtTemperatures: Open PMT Positions File " << PmtPositionsFileName << endl;
    ifstream PmtPositionsFile(PmtPositionsFileName);
    if (!PmtPositionsFile.good()) {
      cout << "RichPMTCalib::getRichPmtTemperatures: Problems Opening " << PmtPositionsFileName << endl;
      return RichPmtTemperatures;
    }

    v_pmt_b.assign(NPMT, 0);
    v_pmt_g.assign(NPMT, 0);
    v_pmt_x.assign(NPMT, 0);
    v_pmt_y.assign(NPMT, 0);
    int npmt = 0;
    while ( PmtPositionsFile >> pmt >> brick >> grid >> x >> y ) {

      if (pmt<NPMT) {
	npmt++;
	v_pmt_b[pmt] = brick;
	v_pmt_g[pmt] = grid;
	v_pmt_x[pmt] = x;
	v_pmt_y[pmt] = y;
	//cout << " PMT " << pmt << " BRICK " << v_pmt_b[pmt] << " GRID " << v_pmt_g[pmt]
	//     << " X " << v_pmt_x[pmt] << " Y " << v_pmt_y[pmt] << endl; 
      }
      else {
	cout << "RichPMTCalib::getRichPmtTemperatures: Error in PMT Positions File. " 
	     << "pmt: " << pmt << " brick : " << brick << " grid : " << grid
	     << " x: " << x << " y: " << y << endl;
	return RichPmtTemperatures;
      }

    }

    PmtPositionsFile.close();

    cout << "RichPMTCalib::getRichPmtTemperatures: Number of PMT Positions Read : " << npmt << endl;
    if (npmt != NPMT) {
      cout << "RichPMTCalib::getRichPmtTemperatures: Error Expected PMT Positions : " << NPMT << endl;
      return RichPmtTemperatures;
    }

    // PMT-DST Distance Map
    for (pmt=0; pmt<NPMT; pmt++) {
      m_pmt_d2_dts[pmt].clear();
      for (dts=0; dts<NDTS; dts++) {
	if (v_dts_g[dts] == v_pmt_g[pmt]) {
	  dx = v_dts_x[dts] - v_pmt_x[pmt];
	  dy = v_dts_y[dts] - v_pmt_y[pmt];
          d2 = dx*dx + dy*dy;
	  m_pmt_d2_dts[pmt].insert( pair<float,int>(d2,dts) );
	}
      }
      if (!m_pmt_d2_dts[pmt].size()) {
	cout << "RichPMTCalib::getRichPmtTemperatures: Error in PMT-DST Mapping : PMT " << pmt << endl;
	return RichPmtTemperatures;
      }
      //for (it=m_pmt_d2_dts[pmt].begin(); it!=m_pmt_d2_dts[pmt].end(); it++)
      //cout << "PMT " << pmt << " d2 " << it->first << " dts " << it->second << endl;

    }

    initok = true;

  }

  if (!initok || !AMSSetupR::gethead()) return RichPmtTemperatures;
  RichPmtTemperatures = true;

  // Retrieve DTS temperatures
  v_dts_temp.assign(NDTS,HUGE_VALF);
  int method = 1; // 1: linear interpolation, 0: no interpolation 
  for (int dts=0; dts<NDTS; dts++) {
    rc = AMSSetupR::gethead()->fSlowControl.GetData(
					  v_dts_el[dts].c_str(),
					  AMSEventR::Head()->fHeader.Time[0],
					  0,
					  value,
					  method,
					  v_dts_nn[dts].c_str()
					  );
    if (!rc) v_dts_temp[dts] = value[0];
  }

  // PMT-DTS temperature map : closest DTS in grid
  v_pmt_temp.assign(NPMT,HUGE_VALF);
  for (int pmt=0; pmt<NPMT; pmt++) {
    for (it=m_pmt_d2_dts[pmt].begin(); it!=m_pmt_d2_dts[pmt].end(); it++) {
      dts = it->second;
      if (v_dts_temp[dts]==HUGE_VALF) continue;
      v_pmt_temp[pmt] = v_dts_temp[dts];
      break;
    }
    if (v_pmt_temp[pmt]==HUGE_VALF) {
      if (v_pmt_rep[pmt]++ < MAX_REP) 
	cout << "RichPMTCalib::getRichPmtTemperatures: "
	     << " Run "   << AMSEventR::Head()->fHeader.Run 
	     << " Event " << AMSEventR::Head()->fHeader.Event 
	     << " NoValidTempFoundForPMT " << pmt 
	     << " (Reported/Max " << v_pmt_rep[pmt] << "/" << MAX_REP << ")" << endl;
      v_pmt_temp[pmt] = pmtRefTemperature;
      RichPmtTemperatures = false;
    }
    if (DEBUG)
      cout << "RichPMTCalib::getRichPmtTemperatures: PMT " << pmt << " (T=" << v_pmt_temp[pmt] << ")" <<  endl;
    if (v_pmt_temp[pmt]<pmtMinTemperature || v_pmt_temp[pmt]>pmtMaxTemperature) 
      cout << "RichPMTCalib::getRichPmtTemperatures: Temperature Alarm on PMT " << pmt << " (T=" << v_pmt_temp[pmt] << ")" << endl;
  }

  return RichPmtTemperatures;

}


bool RichPMTCalib::getRichBrickTemperatures() {

  int dts, id, pos, pmt, brick, grid, utime, rc;
  float x, y, dx, dy, d2;
  string element, node;

  static bool init = true;
  static bool initok = false;

  const int NDTS = 8, NPMT = 680;

  const int MAX_REP = 5;
  static vector<int> v_pmt_rep;

  static vector<string> v_dts_el;
  static vector<string> v_dts_nn;
  static multimap<float,int> m_pmt_d2_dts[NPMT];

  multimap<float,int>::iterator it;

  vector<int>    v_dts_b;

  vector<float> v_pmt_x;
  vector<float> v_pmt_y;
  vector<int>   v_pmt_g;
  vector<int>   v_pmt_b; 

  vector<float> value;
  vector<float> v_dts_temp;

  bool RichBrickTemperatures = false;

  if (init) {

    init = false;

    v_brick_temp.assign(NPMT, brickRefTemperature); // Default HV Brick Temperatures
    v_pmt_rep.assign(NPMT, 0);

    TString DBDir=currentDir+TString("/RichTemperatures/");
    TString DtsPositionsFileName, PmtPositionsFileName;

    // DTS Brick
    DtsPositionsFileName = DBDir + TString("HVBrickDtsPositions.txt");
    cout << "RichPMTCalib::getRichBrickTemperatures: Open DTS File " << DtsPositionsFileName << endl;
    ifstream DtsPositionsFile(DtsPositionsFileName);
    if (!DtsPositionsFile.good()) {
      cout << "RichPMTCalib::getRichBrickTemperatures: Problems Opening " << DtsPositionsFileName << endl;
      return RichBrickTemperatures;
    }

    v_dts_b.assign(NDTS, 0);
    v_dts_el.assign(NDTS, "");
    v_dts_nn.assign(NDTS, "");
    int ndts = 0;
    while ( DtsPositionsFile >> dts >> brick >> element >> node ) {

      if (dts<NDTS) {
	ndts++;
	v_dts_b[dts] = brick;
      	v_dts_el[dts] = element;
	v_dts_nn[dts] = node;
	//cout << " DTS " << dts << " BRICK " << v_dts_b[dts]
	//     << " " << v_dts_el[dts] << " " << v_dts_nn[dts] << endl;
      }
      else {
	cout << "RichPMTCalib::getRichBrickTemperatures: Error in DTS File. " 
	     << "dts: " << dts << " brick: " << brick
	     << " element:  " << element << " node: " << node << endl;
	return RichBrickTemperatures;
      }

    }

    DtsPositionsFile.close();

    cout << "RichPMTCalib::getRichBrickTemperatures: Number of DTS Positions Read : " << ndts << endl;
    if (ndts != NDTS) {
      cout << "RichPMTCalib::getRichBrickTemperatures: Error Expected DTS Positions : " << NDTS << endl;
      return RichBrickTemperatures;
    }

    // PMT Positions
    PmtPositionsFileName = DBDir + TString("PmtPositions.txt");
    cout << "RichPMTCalib::getRichBrickTemperatures: Open PMT Positions File " << PmtPositionsFileName << endl;
    ifstream PmtPositionsFile(PmtPositionsFileName);
    if (!PmtPositionsFile.good()) {
      cout << "RichPMTCalib::getRichBrickTemperatures: Problems Opening " << PmtPositionsFileName << endl;
      return RichBrickTemperatures;
    }

    v_pmt_b.assign(NPMT, 0);
    v_pmt_g.assign(NPMT, 0);
    v_pmt_x.assign(NPMT, 0);
    v_pmt_y.assign(NPMT, 0);
    int npmt = 0;
    while ( PmtPositionsFile >> pmt >> brick >> grid >> x >> y ) {

      if (pmt<NPMT) {
	npmt++;
	v_pmt_b[pmt] = brick;
	v_pmt_g[pmt] = grid;
	v_pmt_x[pmt] = x;
	v_pmt_y[pmt] = y;
	//cout << " PMT " << pmt << " BRICK " << v_pmt_b[pmt] << " GRID " << v_pmt_g[pmt]
	//     << " X " << v_pmt_x[pmt] << " Y " << v_pmt_y[pmt] << endl; 
      }
      else {
	cout << "RichPMTCalib::getRichBrickTemperatures: Error in PMT Positions File. " 
	     << "pmt: " << pmt << " brick : " << brick << " grid : " << grid 
	     << " x: " << x << " y: " << y << endl;
	return RichBrickTemperatures;
      }

    }

    PmtPositionsFile.close();

    cout << "RichPMTCalib::getRichBrickTemperatures: Number of PMT Positions Read : " << npmt << endl;
    if (npmt != NPMT) {
      cout << "RichPMTCalib::getRichBrickTemperatures: Error Expected PMT Positions : " << NPMT << endl;
      return RichBrickTemperatures;
    }

    // PMT-DST Map
    for (pmt=0; pmt<NPMT; pmt++) {
      m_pmt_d2_dts[pmt].clear();
      for (dts=0; dts<NDTS; dts++) {
	if (v_dts_b[dts] == v_pmt_b[pmt])
	  m_pmt_d2_dts[pmt].insert( pair<float,int>(1,dts) );
      }
      if (!m_pmt_d2_dts[pmt].size()) {
	cout << "RichPMTCalib::getRichPmtTemperatures: Error in PMT-DST Mapping : PMT " << pmt << endl;
	return RichBrickTemperatures;
      }
      //for (it=m_pmt_d2_dts[pmt].begin(); it!=m_pmt_d2_dts[pmt].end(); it++)
      //cout << "PMT " << pmt << " d2 " << it->first << " dts " << it->second << endl;

    }

    initok = true;

  }

  if (!initok || !AMSSetupR::gethead())  return RichBrickTemperatures;

  RichBrickTemperatures = true;

  // Retrieve DTS temperatures
  v_dts_temp.assign(NDTS,HUGE_VALF);
  int method = 1; // 1: linear interpolation, 0: no interpolation 
  for (int dts=0; dts<NDTS; dts++) {
    rc = AMSSetupR::gethead()->fSlowControl.GetData(
					  v_dts_el[dts].c_str(),
					  AMSEventR::Head()->fHeader.Time[0],
					  0,
					  value,
					  method,
					  v_dts_nn[dts].c_str()
					  );
    if (!rc) v_dts_temp[dts] = value[0];
  }

  // PMT-DTS temperature map : mean of DTS in brick
  v_brick_temp.assign(NPMT,HUGE_VALF);
  for (int pmt=0; pmt<NPMT; pmt++) {
    int n=0;
    float sum=0;
    for (it=m_pmt_d2_dts[pmt].begin(); it!=m_pmt_d2_dts[pmt].end(); it++) {
      dts = it->second;
      if (v_dts_temp[dts]==HUGE_VALF) continue;
      n++;
      sum += v_dts_temp[dts];
    }
    if (n) v_brick_temp[pmt] = sum/n; 
    if (v_brick_temp[pmt]==HUGE_VALF) {
      if (v_pmt_rep[pmt]++ < MAX_REP) 
	cout << "RichPMTCalib::getRichBrickTemperatures: "
	     << " Run "   << AMSEventR::Head()->fHeader.Run 
	     << " Event " << AMSEventR::Head()->fHeader.Event 
	     << " NoValidTempFoundForPMT " << pmt 
	     << " (Reported/Max " << v_pmt_rep[pmt] << "/" << MAX_REP << ")" << endl;
      v_brick_temp[pmt] = brickRefTemperature;
      RichBrickTemperatures = false;
    }
    if (DEBUG)
      cout << "RichPMTCalib::getRichBrickTemperatures: PMT " << pmt << " (T=" << v_brick_temp[pmt] << ")" <<  endl;
    if (v_brick_temp[pmt]<brickMinTemperature || v_brick_temp[pmt]>brickMaxTemperature) 
      cout << "RichPMTCalib::getRichBrickTemperatures: Temperature Alarm on PMT " << pmt << " (T=" << v_brick_temp[pmt] << ")" << endl;
  }

  return RichBrickTemperatures;

}


unsigned short RichPMTCalib::CheckRichRun(int run, vector<unsigned short> &v_pmt_stat, vector<unsigned short> &v_pmt_volt) {

  /////////////////////////////////////
  //
  // richRunTag: 
  //
  // XXXX XXXX XXX1 1111
  // .... .... .... ...v : JINFR CNF Missing 
  // .... .... .... ..v. : JINFR HKD Missing 
  // .... .... .... .v.. : RDR   CNF Missing 
  // .... .... .... v... : RDR   HKD Missing
  // 
  // .... .... ...v .... : Low Rich Occupancy
  //
  //
  // PMT Status :
  //
  // XXX1 1111 XXX1 1111
  // .... .... .... ...v : FE Off           
  // .... .... .... ..v. : HV NotNominal    
  // .... .... .... .v.. : JINFR Mismatch / Error  
  // .... .... .... v... : RDR   Mismatch / Error
  //
  // .... .... ...v .... : Low Pmt  Occupancy 
  //
  // .... ...v .... .... : JINFR CNF Missing 
  // .... ..v. .... .... : JINFR HKD Missing 
  // .... .v.. .... .... : RDR   CNF Missing 
  // .... v... .... .... : RDR   HKD Missing
  //
  // ...v .... .... .... : Low Rich Occupancy 
  //
  /////////////////////////////////////

  // Rich Tag Files
  char *fName[NTAG] = {
    //"RunClassifierJ.txt", "RunClassifierJHK.txt",
    //"RunClassifierR.txt", "RunClassifierRHK.txt"
    "RunClassifierJ_merged.txt", "RunClassifierJHK.txt",
    "RunClassifierR_merged.txt", "RunClassifierRHK.txt"
  };


  // PMT Default Status
  const unsigned short richRunDflt = (1<<kTagJ) | (1<<kTagJHK) | (1<<kTagR) | (1<<kTagRHK) | (1<<kTagOcc);
  const unsigned short richPmtDflt = (1<<kPmtFE) | (1<<kPmtHV) | (1<<kPmtJ) | (1<<kPmtR)   | (1<<kPmtOcc);
  const unsigned short richVltDflt = 0;

  // Association windows
  const int MaxRunDelay = 5;
  const int MaxRunSpan = 1800;

  static bool init = true;
  static bool initok = false;

  static map<int,string> m_tag[NTAG];
  map<int,string>::iterator it;

  static vector<unsigned short> w_pmt_stat, w_pmt_volt;

  static vector<string> v_brick_name[NBRICK];
  static vector<int> v_brick_busa[NBRICK], v_brick_busb[NBRICK], v_brick_fgin[NBRICK];

  unsigned short richRunTag = richRunDflt; 

  static int run_prev;
  static unsigned short richRunTag_prev; 
  static vector<unsigned short> v_pmt_stat_prev, v_pmt_volt_prev;

  if (init) {

    init = false;

    w_pmt_stat.assign(NPMT, richRunDflt << 8 | richPmtDflt);
    w_pmt_volt.assign(NPMT, richVltDflt);

    v_pmt_stat = w_pmt_stat;
    v_pmt_volt = w_pmt_volt;

    richRunTag_prev = richRunTag;
    v_pmt_stat_prev = v_pmt_stat;
    v_pmt_volt_prev = v_pmt_volt;

    TString TAGDir=currentDir+("/RichRunTags/");

    for (int tag=0; tag<NTAG; tag++) {

      char line[256];
      TString RunTagFileName;

      m_tag[tag].clear();

      RunTagFileName = TAGDir + TString(fName[tag]);
      cout << "RichPMTCalib::CheckRichRun: Open File " << RunTagFileName << endl;

      ifstream RunTagFile(RunTagFileName);
      if (!RunTagFile.good()) {
	cout << "RichPMTCalib::CheckRichRun: Problems Opening " << RunTagFileName << endl;
	return richRunTag;
      }

      while ( RunTagFile.getline( line, sizeof(line)) ) {
	string sline(line);
	istringstream ssline(sline);
	int utime;
	ssline >> utime;
	m_tag[tag].insert(pair<int,string>(utime,sline));
      }

      RunTagFile.close();

      cout << "RichPMTCalib::CheckRichRun: Number of Entries Read : " << m_tag[tag].size() << endl;
      //for (it=m_tag[tag].begin(); it!=m_tag[tag].end(); it++)
      //cout << it->first << " : " << it->second << endl;

    }

    string name("RHVUNK");
    int brick, side, busa, busb, fgin;

    TString HVBrickMapFileName;
    HVBrickMapFileName = TAGDir + TString("HVBrickMap.txt");
    cout << "RichPMTCalib::CheckRichRun: Open File " << HVBrickMapFileName << endl;

    ifstream HVBrickMapFile(HVBrickMapFileName);
    if (!HVBrickMapFile.good()) {
      cout << "RichPMTCalib::CheckRichRun: Problems Opening " << HVBrickMapFileName << endl;
      return richRunTag;
    }

    for (brick=0; brick<NBRICK; brick++) {
      v_brick_name[brick].assign(NSIDE, name);
      v_brick_busa[brick].assign(NSIDE, -1);
      v_brick_busb[brick].assign(NSIDE, -1);
      v_brick_fgin[brick].assign(NSIDE, -1);
    }
    int nbside = 0;
    while ( HVBrickMapFile >> brick >> side >> name >> busa >> busb >> fgin ) {
      if (DEBUG)
	cout << brick << " " << side << " " << name << " " << busa << " " << busb << " " << fgin << endl;
      if (brick<NBRICK && side<NSIDE) {
	nbside++;
	v_brick_name[brick][side] = name;
	v_brick_busa[brick][side] = busa;
	v_brick_busb[brick][side] = busb;
	v_brick_fgin[brick][side] = fgin;
      }

    }

    HVBrickMapFile.close();

    cout << "RichPMTCalib::CheckRichRun: Number of Entries Read : " << nbside << endl;
    if (nbside != NSIDE*NBRICK) {
      cout << "RichPMTCalib::CheckRichRun: Error Expected HVBrick Records : " << NSIDE*NBRICK << endl;
      return richRunTag;
    }


    // Occupancy based bits should be filled here


    initok = true;

  }

  if (run == run_prev) {
    v_pmt_stat = v_pmt_stat_prev;
    v_pmt_volt = v_pmt_volt_prev;
    return richRunTag_prev;
  }

  v_pmt_stat = w_pmt_stat;
  v_pmt_volt = w_pmt_volt;

  run_prev = run;
  richRunTag_prev = richRunTag;
  v_pmt_stat_prev = v_pmt_stat;
  v_pmt_volt_prev = v_pmt_volt;

  if (!initok) return richRunTag;

  // All stuff comes here ...
  for (int tag=0; tag<NTAG; tag++) {

    it = m_tag[tag].upper_bound(run);

    //cout << run << " " << tag << " " << it->first << " " << it->first-run << endl;

    switch (tag) {

    case kTagJ:
      it--;
      if (run-it->first<0 || run-it->first>MaxRunDelay)
	cout << "RichPMTCalib::CheckRichRun : CNF-J-NotFoundForRun " << run << " (dt: " << run-it->first << " ) " << endl;
      else {
	richRunTag ^= (1<<kTagJ);
        RichDecodeJ(it->second, v_brick_fgin, v_pmt_stat, v_pmt_volt);
      }
      break;

    case kTagJHK:
      if (it->first-run<0 || it->first-run>MaxRunSpan)
	//cout << "RichPMTCalib::CheckRichRun : HKD-J-NotFoundForRun " << run << " (dt: " << it->first-run << " ) " << endl;
	break;
      else {
	richRunTag ^= (1<<kTagJHK);
        RichDecodeJHK(it->second, v_brick_busa, v_brick_busb, v_pmt_stat);
      }
      break;

    case kTagR:
      it--;
      if (run-it->first<0 || run-it->first>MaxRunDelay)
	cout << "RichPMTCalib::CheckRichRun : CNF-R-NotFoundForRun " << run << " (dt: " << run-it->first << " ) " << endl;
      else {
	richRunTag ^= (1<<kTagR);
	RichDecodeR(it->second, v_pmt_stat);
      }
      break;

    case kTagRHK:
      if (it->first-run<0 || it->first-run>MaxRunSpan)
	//cout << "RichPMTCalib::CheckRichRun : HKD-R-NotFoundForRun " << run << " (dt: " << it->first-run << " ) " << endl;
	break;
      else {
	richRunTag ^= (1<<kTagRHK);
	RichDecodeRHK(it->second, v_pmt_stat);
      }
      break;

    }
  }

  // Occupancy based bits should be set here

  if (DEBUG) {
    cout << "RichPMTCalib::CheckRichRun : richRunTag ";
    bin(richRunTag, cout);
    cout << endl;
    for (int pmt=0; pmt<NPMT; pmt++) {
      cout << pmt << " ";  
      bin(v_pmt_stat[pmt], cout);
      cout << " " << v_pmt_volt[pmt] << "(" << v_pmt_vnom[pmt] << ")" << endl;
    }
  }


  richRunTag_prev = richRunTag;
  v_pmt_stat_prev = v_pmt_stat;
  v_pmt_volt_prev = v_pmt_volt;

  return richRunTag;

}


void RichPMTCalib::RichDecodeJ(string sline, vector<int> v_brick_fgin[],
			     vector<unsigned short>& v_pmt_stat,
			     vector<unsigned short>& v_pmt_volt) {

  istringstream ssline(sline);
  int utime;
  int brick, side, hvchn, fgin;
  char code[4][100], c[100];

  for (int pmt=0; pmt<NPMT; pmt++) {
    v_pmt_stat[pmt] ^= ((1<<kTagJ) << 8);
    v_pmt_stat[pmt] |= (1<<kPmtHV);
    v_pmt_volt[pmt] = 0;
  }

  ssline >> utime >> code[0] >> code[1] >> code[2] >> code[3];

  for (int pmt=0; pmt<NPMT; pmt++) {

    brick = v_pmt_brick[pmt];
    hvchn = v_pmt_hvchn[pmt];

    fgin = v_brick_fgin[brick][0];
    if (!strncmp(&code[fgin][0], "B", 1)) 
      fgin = fgin<2 ? fgin+2 : fgin-2;    // Temporary fix till FG fixes format

    //cout << pmt << " " << fgin << " " << strlen(code[fgin]) << " " << code[fgin] << endl; 
    if (strlen(code[fgin]) != NHVCHN+2) continue;

    v_pmt_stat[pmt] ^= (1<<kPmtJ); // this should be done only by RichDecodeJHK
                                   // when(if) available for all runs

    if (!strncmp(&code[fgin][0]      , "0", 1)) continue; // DCDC Off
    if ( strncmp(&code[fgin][1]      , "1", 1)) continue; // DCDC HV<>950
    if (!strncmp(&code[fgin][2+hvchn], "0", 1)) continue; // HV < HVNOM-50 
    else if (!strncmp(&code[fgin][2+hvchn], ".", 1)) {    // HV = HVNOM
      v_pmt_stat[pmt] ^= (1<<kPmtHV);
      v_pmt_volt[pmt] = v_pmt_vnom[pmt];
    }
    else {                                                // HV < HVNOM
      strncpy(c, &code[fgin][2+hvchn], 1);
      v_pmt_volt[pmt] = v_pmt_vnom[pmt] - 10*atoi(c);
    }

  }

}


void RichPMTCalib::RichDecodeJHK(string sline, vector<int> v_brick_busa[], vector<int> v_brick_busb[], 
			       vector<unsigned short>& v_pmt_stat) {

  istringstream ssline(sline);
  int utime;
  int brick, busa, busb, half, jinf;
  char code[NJINF][100];

  for (int pmt=0; pmt<NPMT; pmt++) {
    v_pmt_stat[pmt] ^= ((1<<kTagJHK) << 8);
    v_pmt_stat[pmt] |= (1<<kPmtJ);
  }

  ssline >> utime >> code[0] >> code[1] >> code[2] >> code[3];
  //cout << utime << " " << code[0] << " " << code[1] << " " << code[2] << " " << code[3] << endl;

  for (int pmt=0; pmt<NPMT; pmt++) {

    brick = v_pmt_brick[pmt];
    busa = v_brick_busa[brick][A];
    busb = v_brick_busb[brick][B];

    half = brick/2;
    jinf = 2*half;
    if (!strcmp(code[jinf],"00000")) jinf++;

    if (strlen(code[jinf]) != 5) continue;

    if (strncmp(&code[jinf][0], "1", 1)) continue;

    if (strncmp(&code[jinf][1+busa], "1", 1) &&
	strncmp(&code[jinf][1+busb], "1", 1)) continue;

    v_pmt_stat[pmt] ^= (1<<kPmtJ);

  }

}


void RichPMTCalib::RichDecodeR(string sline,
			     vector<unsigned short>& v_pmt_stat) {

  istringstream ssline(sline);
  int utime;
  char code[100];

  for (int pmt=0; pmt<NPMT; pmt++) {
    v_pmt_stat[pmt] ^= ((1<<kTagR) << 8);
    v_pmt_stat[pmt] |= (1<<kPmtFE);
  }

  ssline >> utime >> code;
  if (strlen(code) != NRDR) return;

  for (int pmt=0; pmt<NPMT; pmt++) {
    v_pmt_stat[pmt] ^= (1<<kPmtR); // this should be done only by RichDecodeRHK
                                   // when available for all runs
    if (strncmp(&code[v_pmt_rdr[pmt]], "0", 1)) v_pmt_stat[pmt] ^= (1<<kPmtFE);
  }

}


void RichPMTCalib::RichDecodeRHK(string sline,
			       vector<unsigned short>& v_pmt_stat) {

  istringstream ssline(sline);
  int utime;
  char code[100];

  for (int pmt=0; pmt<NPMT; pmt++) {
    v_pmt_stat[pmt] ^= ((1<<kTagRHK) << 8);
    v_pmt_stat[pmt] |= (1<<kPmtR);
  }

  ssline >> utime >> code;
  if (strlen(code) != NRDR) return;

  for (int pmt=0; pmt<NPMT; pmt++)
    if (!strncmp(&code[v_pmt_rdr[pmt]], "0", 1)) v_pmt_stat[pmt] ^= (1<<kPmtR);

}


bool RichPMTCalib::ReadPmtDB() {

  int pmt;
  unsigned short rdr, brick, hvchn, vnom;

  static bool init = true;
  static bool initok = false;

  bool readPmtDB = false;

  if (init) {

    init = false;

    TString DBDir=currentDir+TString("/RichDB/");
    TString PmtDBFileName;

    PmtDBFileName = DBDir + TString("RichPmtDB.txt");
    cout << "RichPMTCalib::ReadPmtDB: Open DB File " << PmtDBFileName << endl;
    ifstream PmtDBFile(PmtDBFileName);
    if (!PmtDBFile.good()) {
      cout << "RichPMTCalib::ReadPmtDB: Problems Opening " << PmtDBFileName << endl;
      return readPmtDB;
    }

    v_pmt_rdr.assign(NPMT, 0);
    v_pmt_brick.assign(NPMT, 0);
    v_pmt_hvchn.assign(NPMT, 0);
    v_pmt_vnom.assign(NPMT, 0); 
    int npmt = 0;
    while ( PmtDBFile >> pmt >> rdr >> brick >> hvchn >> vnom ) {

      if (pmt<NPMT) {
        npmt++;
        v_pmt_rdr[pmt] = rdr;
        v_pmt_brick[pmt] = brick;
        v_pmt_hvchn[pmt] = hvchn;
        v_pmt_vnom[pmt] = vnom;
      } 
      else {
        cout << "RichPMTCalib::ReadPmtDB: Error in PMT DB File. "
             << "pmt: " << pmt << " brick: " << brick << " hvchn: " << hvchn << " vnom: " << vnom << endl;
        return readPmtDB;
      }

    }

    PmtDBFile.close();

    cout << "RichPMTCalib::ReadPmtDB: Number of PMT Records Read : " << npmt << endl;
    if (npmt != NPMT) {
      cout << "RichPMTCalib::ReadPmtDB: Error Expected PMT Records : " << NPMT << endl;
      return readPmtDB;
    }

    initok = true;

  }

  if (initok) readPmtDB = true;

  return readPmtDB;

}

