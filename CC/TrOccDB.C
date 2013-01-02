// $Id: TrOccDB.C,v 1.1 2013/01/02 19:41:41 oliva Exp $


#include "TrOccDB.h"


#include "TFile.h"


#include "timeid.h"
#include "commonsi.h"


ClassImp(TrOccDB);
ClassImp(TrLadOcc);


///////////////////////////
// TrOccDB
///////////////////////////

unsigned int TrOccDB::fRun = 0;
TrOccDB*     TrOccDB::fHead = 0;
float*       TrOccDB::fLinear = 0;
float        TrOccDB::BadOccupancyValue = -50;


TrOccDB* TrOccDB::GetHead() {
  if (IsNull()) {
    printf("TrOccDB::GetHead()-V TrOccDB singleton initialization.\n");
    fHead = new TrOccDB(); 
  } 
  return fHead;
}


TrOccDB::~TrOccDB() { 
  Clear();
  if (!fTrOccHwIdMap.empty()) {
    for (TrOccIt ladocc=fTrOccHwIdMap.begin(); ladocc!=fTrOccHwIdMap.end(); ++ladocc)
      delete (*ladocc).second;
    fTrOccHwIdMap.clear();
  }
  if (fLinear) delete [] fLinear;
  fLinear = 0;
  fHead = 0;
}


bool TrOccDB::Init() {
  if (IsNull()) return false;
  if (fTrOccHwIdMap.empty()) {
    for (int icrate=0; icrate<8; icrate++){
      for (int itdr=0; itdr<24; itdr++) {
        int hwid = icrate*100 + itdr;
        fTrOccHwIdMap[hwid] = new TrLadOcc(hwid);
      }
    }
  }
  if (!fLinear) fLinear = new float[GetSize()];
  return true;
}


void TrOccDB::Clear(Option_t* option) {
  Init();
  fRun = 0;
  for (TrOccIt ladocc=fTrOccHwIdMap.begin(); ladocc!=fTrOccHwIdMap.end(); ++ladocc) 
    (*ladocc).second->Clear(option);
  for (int i=0; i<GetSize(); i++) fLinear[i] = 0;
}


void TrOccDB::Info(int verbosity) {
  Init();
  printf("TrOccDB::Info-V Run: %10d\n",GetRun());
  for (TrOccIt ladocc=fTrOccHwIdMap.begin(); ladocc!=fTrOccHwIdMap.end(); ++ladocc)
    (*ladocc).second->Info(verbosity);
}


TrLadOcc* TrOccDB::FindOccHwId(int hwid) {
  Init();
  TrOccIt occ = fTrOccHwIdMap.find(hwid);
  if (occ!=fTrOccHwIdMap.end()) return occ->second;
  else                          return 0;
}


TrLadOcc* TrOccDB::FindOccTkId(int tkid) {
  Init();
  TkLadder* ladder = TkDBc::Head->FindTkId(tkid);
  if (ladder) return FindOccHwId(ladder->GetHwId());
  else        return 0;
}


bool TrOccDB::Save(const char* filename) {
  Init();
  TFile* rootfile = TFile::Open(filename,"recreate");
  if (rootfile==0) return false;
  rootfile->WriteTObject(GetHead());
  return true;
}
 

bool TrOccDB::Load(const char* filename) {
  // Init(); // no memory loss
  TFile* rootfile = TFile::Open(filename,"read");
  if (rootfile==0) return false;
  Load(rootfile);
  rootfile->Close();
  return true;
}


bool TrOccDB::Load(TFile* rootfile) { 
  // Init(); // no memory loss
  if (rootfile==0) return false;
  TrOccDB* fh = (TrOccDB*) rootfile->Get("TrOccDB");
  if (!fh) return false;
  if (fHead) delete fHead;
  fHead = fh;
  printf("TrOccDB::Load-V TrOccDB loaded from file %s.\n",rootfile->GetName());
  return true;
}


bool TrOccDB::OccDBToLinear() {
  Init();
  int offset = 0;
  fLinear[offset++] = *((float*)&fRun);
  for (TrOccIt ladocc=fTrOccHwIdMap.begin(); ladocc!=fTrOccHwIdMap.end(); ++ladocc) {
    TrLadOcc* occ = (*ladocc).second;
    occ->OccDBToLinear(fLinear+offset);
    offset += TrLadOcc::GetSize();
  }
  return true;
}


bool TrOccDB::LinearToOccDB() {
  Init();
  int offset = 0;
  unsigned int run = *((unsigned int*)&(fLinear[offset++]));
  SetRun(run);
  for (TrOccIt ladocc=fTrOccHwIdMap.begin(); ladocc!=fTrOccHwIdMap.end(); ++ladocc) {
    TrLadOcc* occ = (*ladocc).second;
    occ->LinearToOccDB(fLinear+offset);
    offset += TrLadOcc::GetSize();
  }
  return true;
}


void TrOccDB::PrintLinear() {
  Init();
  printf("TrOccDB::PrintLinear-V content of the linear database:\n");
  for (int i=0; i<GetSize(); i++) 
    printf("%4d %20.10g\n",i,fLinear[i]);
  printf("TrOccDB::PrintLinear-V content of the linear database --- END\n");
}


bool TrOccDB::CreateFromRawOccupanciesHisto(TH2F* histo, float norm) {
  Init();
  if (!histo) return false;
  // init
  Clear();
  printf("TrOccDB::CreateFromRawOccupanciesHisto-V creating TrOccDB from histogram %s (norm=%10.0f).\n",histo->GetName(),norm);
  vector<float> values;
  int   limits[3] = {0,640,1024};
  int   place[2] = {320,192};
  float median[2] = {1};
  float deviation[2] = {1};
  float occupancy[1024] = {1};
  // overall normalization factor
  histo->Scale(1/norm); 
  // loop on ladders
  for (int iladder=0; iladder<192; iladder++) {
    TkLadder* ladder = TkDBc::Head->FindTkId(TkDBc::Head->Entry2TkId(iladder));
    // loop on sides (0: Y, 1: X)
    for (int iside=0; iside<2; iside++) {
      // median calculation
      values.clear();
      for (int address=limits[iside]; address<limits[iside+1]; address++) 
        values.push_back(histo->GetBinContent(iladder+1,address+1));
      sort(values.begin(),values.end());
      median[1-iside] = (values.at(place[iside]-1) + values.at(place[iside]))/2.;
      // median of deviations
      values.clear();
      for (int address=limits[iside]; address<limits[iside+1]; address++) 
        values.push_back(fabs(histo->GetBinContent(iladder+1,address+1)-median[1-iside]));
      sort(values.begin(),values.end());
      deviation[1-iside] = (values.at(place[iside]-1) + values.at(place[iside]))/2.;
      // normalized occupancy 
      for (int address=limits[iside]; address<limits[iside+1]; address++) {
        occupancy[address] = (fabs(deviation[1-iside]<1e-6)) ? 
          BadOccupancyValue : 
          (histo->GetBinContent(iladder+1,address+1)-median[1-iside])/deviation[1-iside];
      }
    }
    // filling 
    TrLadOcc* ladocc = FindOccHwId(ladder->GetHwId());
    ladocc->SetMedian(median);
    ladocc->SetDeviation(deviation);
    ladocc->SetOccupancy(occupancy);
  }
  return true;
}


TH2D* occupancy_map_histo = 0;
TH2D* TrOccDB::GetOccupancyMapHisto() {
  Init();
  if (occupancy_map_histo) {
    delete occupancy_map_histo;
    occupancy_map_histo = 0;
  }
  if (!occupancy_map_histo) occupancy_map_histo = new TH2D("occupancy_map_histo","; Ladder Index; Channel",192,0,192,1024,0,1024);
  for (int icrate=0; icrate<8; icrate++) {
    for (int itdr=0; itdr<24; itdr++) {
      int iladder = icrate*24 + itdr;
      int hwid = icrate*100 + itdr;
      TrLadOcc* ladocc = FindOccHwId(hwid); 
      for (int address=0; address<1024; address++) 
        occupancy_map_histo->SetBinContent(iladder+1,address+1,ladocc->GetOccupancy(address));
    }
  }
  return occupancy_map_histo;
}


TH2D* occupancy_ladder_histo = 0;
TH2D* TrOccDB::GetOccupancyLadderHisto() {
  Init();
  if (occupancy_ladder_histo) {
    delete occupancy_ladder_histo;
    occupancy_ladder_histo = 0;
  }
  if (!occupancy_ladder_histo) occupancy_ladder_histo = new TH2D("occupancy_ladder_histo","; Ladder Index; Normalized Occupancy",384,0,384,4000,-100,1100);
  for (int icrate=0; icrate<8; icrate++) {
    for (int itdr=0; itdr<24; itdr++) {
      int hwid = icrate*100 + itdr;
      TrLadOcc* ladocc = FindOccHwId(hwid);
      for (int address=0; address<1024; address++) {
        int iside = (address<640) ? 1 : 0;
        int iladder = iside*192 + icrate*24 + itdr;
        occupancy_ladder_histo->Fill(iladder,ladocc->GetOccupancy(address));
      }
    }
  }
  return occupancy_ladder_histo;
}


void TrOccDB::PrintCalibrationComparisonTable() {
  Init();
  int cal[2][3] = {0};        // bad/noisy/dead in cal per side
  int occ[2][3] = {0};        // bad/noisy/dead in occ per side
  int both[2][3] = {0};       // bad/noisy/dead in cal and in occ per side
  int cal_no_occ[2][3] = {0}; // bad/noisy/dead in cal not in occ per side
  int occ_no_cal[2][3] = {0}; // bad/noisy/dead in occ not in cal per side
  for (int icrate=0; icrate<8; icrate++) {
    for (int itdr=0; itdr<24; itdr++) {
      int hwid = icrate*100 + itdr;
      TrLadCal* ladcal = TrCalDB::Head->FindCal_HwId(hwid);
      TrLadOcc* ladocc = FindOccHwId(hwid);
      for (int address=0; address<1024; address++) {
        int iside  = (address<640) ? 1 : 0;
        int stacal = ladcal->GetStatus(address);
        int staocc = ladocc->GetStatus(address);
        // bad
        if (stacal!=0) cal[iside][0]++;
        if (staocc!=0) occ[iside][0]++;
        if      ( (stacal!=0)&&(staocc!=0) ) both[iside][0]++;
        else if ( (stacal!=0)&&(staocc==0) ) cal_no_occ[iside][0]++;
        else if ( (stacal==0)&&(staocc!=0) ) occ_no_cal[iside][0]++;
        // noise
        if ((stacal&0xA)!=0) cal[iside][1]++;
        if ((staocc&0x2)!=0) occ[iside][1]++;
        if      ( ((stacal&0xA)!=0)&&((staocc&0x2)!=0) ) both[iside][1]++;
        else if ( ((stacal&0xA)!=0)&&((staocc&0x2)==0) ) cal_no_occ[iside][1]++;
        else if ( ((stacal&0xA)==0)&&((staocc&0x2)!=0) ) occ_no_cal[iside][1]++;
        // dead 
        if ((stacal&0x8015)!=0) cal[iside][2]++;
        if ((staocc&0x1)!=0)    occ[iside][2]++; 
        if      ( ((stacal&0x8015)!=0)&&((staocc&0x1)!=0) ) both[iside][2]++;
        else if ( ((stacal&0x8015)!=0)&&((staocc&0x1)==0) ) cal_no_occ[iside][2]++;
        else if ( ((stacal&0x8015)==0)&&((staocc&0x1)!=0) ) occ_no_cal[iside][2]++;
      }
    }
  }
  printf("TrOccDB::PrintCalibrationComparisonTable-V:\n");
  printf("  X-bad(all) cal:%7d  occ:%7d  both:%7d  only-cal:%7d  only-occ:%7d\n",cal[0][0],occ[0][0],both[0][0],cal_no_occ[0][0],occ_no_cal[0][0]);
  printf("  X-noisy    cal:%7d  occ:%7d  both:%7d  only-cal:%7d  only-occ:%7d\n",cal[0][1],occ[0][1],both[0][1],cal_no_occ[0][1],occ_no_cal[0][1]);
  printf("  X-dead     cal:%7d  occ:%7d  both:%7d  only-cal:%7d  only-occ:%7d\n",cal[0][2],occ[0][2],both[0][2],cal_no_occ[0][2],occ_no_cal[0][2]);
  printf("  Y-bad(all) cal:%7d  occ:%7d  both:%7d  only-cal:%7d  only-occ:%7d\n",cal[1][0],occ[1][0],both[1][0],cal_no_occ[1][0],occ_no_cal[1][0]);
  printf("  Y-noisy    cal:%7d  occ:%7d  both:%7d  only-cal:%7d  only-occ:%7d\n",cal[1][1],occ[1][1],both[1][1],cal_no_occ[1][1],occ_no_cal[1][1]);
  printf("  Y-dead     cal:%7d  occ:%7d  both:%7d  only-cal:%7d  only-occ:%7d\n",cal[1][2],occ[1][2],both[1][2],cal_no_occ[1][2],occ_no_cal[1][2]);
}


int TrOccDB::GetNBadStrips() {
  int nbad = 0;
  for (int icrate=0; icrate<8; icrate++) {
    for (int itdr=0; itdr<24; itdr++) {
      int hwid = icrate*100 + itdr;
      TrLadOcc* ladocc = FindOccHwId(hwid);
      for (int address=0; address<1024; address++) {
        int staocc = ladocc->GetStatus(address);
        if (staocc!=0) nbad++;
      }
    }
  }
  return nbad;  
}


bool TrOccDB::SaveInTDV(unsigned int start_time, unsigned int validity, unsigned int insert_time, int isReal) {
  Init();
  time_t statime = time_t(start_time);
  time_t endtime = time_t(start_time+validity);
  time_t instime = time_t(insert_time); 
  OccDBToLinear();
  tm beg, end;
  localtime_r(&statime,&beg);
  localtime_r(&endtime,&end);
  AMSTimeID* tdv = new AMSTimeID(
    AMSID("TrackerOccupancy",isReal),beg,end,TrOccDB::GetLinearSize(),TrOccDB::GetLinear(),AMSTimeID::Standalone,1
  );
  tdv->UpdateMe();
  tdv->SetTime(instime,statime,endtime);
  tdv->write(AMSDATADIR.amsdatabase);
  if (tdv) delete tdv;
  return true;
} 


int TrOccDB::LoadFromTDV(unsigned int time, int isReal) {
  Init();
  time_t tt = time_t(time);
  tm beg;
  tm end;
  beg.tm_isdst = end.tm_isdst = 0;
  beg.tm_sec   = beg.tm_min = beg.tm_hour =
  beg.tm_mday  = beg.tm_mon = beg.tm_year = 0;
  end.tm_sec   = end.tm_min = end.tm_hour =
  end.tm_mday  = end.tm_mon = end.tm_year = 0;
  AMSTimeID *tdv = new AMSTimeID(
    AMSID("TrackerOccupancy",isReal),beg,end,TrOccDB::GetLinearSize(),TrOccDB::GetLinear(),AMSTimeID::Standalone,1,FunctionLinearToOccDB 
  );
  int ret = tdv->validate(tt);
  if (tdv) delete tdv;
  return ret;
}


void FunctionLinearToOccDB() {
  if (!TrOccDB::IsNull()) TrOccDB::GetHead()->LinearToOccDB();
}


///////////////////////////
// TrLadOcc
///////////////////////////


float TrLadOcc::MinimumNormalizedOccupancy = -5;
float TrLadOcc::MaximumNormalizedOccupancy = 20;


TrLadOcc::TrLadOcc(int hwid, float* median, float* deviation, float* occupancy) {
  Clear(); 
  SetHwId(hwid); 
  SetMedian(median);
  SetDeviation(deviation);
  SetOccupancy(occupancy);  
}


void TrLadOcc::Clear(Option_t* option) {
  fHwId = 0;
  for (int iside=0; iside<2; iside++) {
    fMedian[iside] = 0;
    fDeviation[iside] = 1; 
  } 
  for (int address=0; address<1024; address++) {
    fOccupancy[address] = 0;
  }
  TObject::Clear(option);
}


void TrLadOcc::Info(int verbosity) {
  printf("TrLadOcc::Info-V HwId: %03d\n",GetHwId());
  printf("  MedianX: %8.3f   DeviationX: %8.3f\n",GetMedian(0),GetDeviation(0));
  printf("  MedianY: %8.3f   DeviationY: %8.3f\n",GetMedian(1),GetDeviation(1));
  if (verbosity>0) {
    TrLadCal* ladcal = TrCalDB::Head->FindCal_HwId(GetHwId()); // additional infos
    for (int address=0; address<1024; address++) {
      int iside = (address<640) ? 1 : 0;
      printf("  Address:%4d  Occ.:%8.3f  OccStatus:%4d  Ped.:%8.3f  SigmaRaw:%8.3f  Sigma:%8.3f  Status:%4d\n",
        address,GetOccupancy(address),GetStatus(address),
        ladcal->GetPedestal(address),ladcal->GetSigmaRaw(address),ladcal->GetSigma(address),ladcal->GetStatus(address)
      );
    }
  }
}


int TrLadOcc::GetStatus(int address) { 
  if ( (address<0)||(address>1023) ) return -1;
  int status = 0;
  if (GetOccupancy(address)<TrLadOcc::MinimumNormalizedOccupancy) status |= 0x1;
  if (GetOccupancy(address)>TrLadOcc::MaximumNormalizedOccupancy) status |= 0x2;
  return status;
}


bool TrLadOcc::OccDBToLinear(float* offset) {
  if (!offset) return false;
  int index = 0;
  offset[index++] = (float) GetHwId();
  for (int iside=0; iside<2; iside++) offset[index++] = (float) GetMedian(iside);
  for (int iside=0; iside<2; iside++) offset[index++] = (float) GetDeviation(iside);
  for (int address=0; address<1024; address++) offset[index++] = (float) GetOccupancy(address);
  return true;
}


bool TrLadOcc::LinearToOccDB(float* offset) {
  if (!offset) return false;
  int index = 0;
  SetHwId((int)offset[0]);
  SetMedian(offset+1);
  SetDeviation(offset+1+2);
  SetOccupancy(offset+1+2+2);
  return true;
}


