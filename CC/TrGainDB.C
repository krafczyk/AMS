// $Id: TrGainDB.C,v 1.4 2012/07/18 09:40:51 oliva Exp $

#include "TrGainDB.h"


#include "TFile.h"


#include "timeid.h"
#include "commonsi.h"


ClassImp(TrGainDB);
ClassImp(TrLadGain);


///////////////////////////
// TrGainDB
///////////////////////////


TrGainDB* TrGainDB::fHead = 0;
float* TrGainDB::fLinear = 0;


TrGainDB* TrGainDB::GetHead() {
  if (IsNull()) {
    printf("TrGainDB::GetHead()-V TrGainDB singleton initialization.\n");
    fHead = new TrGainDB(); 
  } 
  return fHead;
}


TrGainDB::~TrGainDB() { 
  Clear();
  if (!fTrGainHwIdMap.empty()) {
    for (TrGainIt ladgain=fTrGainHwIdMap.begin(); ladgain!=fTrGainHwIdMap.end(); ++ladgain)
      delete (*ladgain).second;
    fTrGainHwIdMap.clear();
  }
  if (fLinear) delete [] fLinear;
  fLinear = 0;
  fHead = 0;
}


bool TrGainDB::Init() {
  if (IsNull()) return false;
  if (fTrGainHwIdMap.empty()) {
    for (int icrate=0; icrate<8; icrate++){
      for (int itdr=0; itdr<24; itdr++) {
        int hwid = icrate*100 + itdr;
        fTrGainHwIdMap[hwid] = new TrLadGain(hwid);
      }
    }
  }
  if (!fLinear) fLinear = new float[GetSize()];
  return true;
}


void TrGainDB::Clear(Option_t* option) {
  Init();
  for (TrGainIt ladgain=fTrGainHwIdMap.begin(); ladgain!=fTrGainHwIdMap.end(); ++ladgain) 
    (*ladgain).second->Clear(option);
  for (int i=0; i<GetSize(); i++) fLinear[i] = 0;
}


void TrGainDB::Info(int verbosity) {
  Init();
  for (TrGainIt ladgain=fTrGainHwIdMap.begin(); ladgain!=fTrGainHwIdMap.end(); ++ladgain)
    (*ladgain).second->Info(verbosity);
}


void TrGainDB::Dump() {
  Init();
  for (TrGainIt ladgain=fTrGainHwIdMap.begin(); ladgain!=fTrGainHwIdMap.end(); ++ladgain) {
    for (int iva=0; iva<16; iva++) {
      (*ladgain).second->Dump(iva);
    }
  }
}


TrLadGain* TrGainDB::FindGainHwId(int hwid) {
  Init();
  TrGainIt gain = fTrGainHwIdMap.find(hwid);
  if (gain!=fTrGainHwIdMap.end()) return gain->second;
  else                            return 0;
}


TrLadGain* TrGainDB::FindGainTkId(int tkid) {
  Init();
  TkLadder* ladder = TkDBc::Head->FindTkId(tkid);
  if (ladder) return FindGainHwId(ladder->GetHwId());
  else        return 0;
}


bool TrGainDB::Save(const char* filename) {
  Init();
  TFile* rootfile = TFile::Open(filename,"recreate");
  if (rootfile==0) return false;
  rootfile->WriteTObject(GetHead());
  return true;
}
 

bool TrGainDB::Load(const char* filename) {
  // Init(); // no memory loss
  TFile* rootfile = TFile::Open(filename,"read");
  if (rootfile==0) return false;
  Load(rootfile);
  rootfile->Close();
  return true;
}


bool TrGainDB::Load(TFile* rootfile) { 
  // Init(); // no memory loss
  if (rootfile==0) return false;
  if (rootfile->Get("TrGainDB")==0) return false;
  fHead = (TrGainDB*) rootfile->Get("TrGainDB");
  printf("TrGainDB::Load-V TrGainDB loaded from file %s.\n",rootfile->GetName());
  return true;
}


bool TrGainDB::LoadFromTxtFile(const char* filename) {
  Init();
  FILE* file = fopen(filename,"r");
  if (file==0) return false;
  printf("TrGainDB::LoadFromTxtFile-V reding Tracker VA Gain parameters from file %s\n",filename);
  while (!feof(file)) {
    int tkid,iva,bit0,bit1,bit2,bit3,bit4;
    float gain,offset,syserr;
    int ret = fscanf(file,"%d%d%d%d%d%d%d%f%f%f",&tkid,&iva,&bit0,&bit1,&bit2,&bit3,&bit4,&gain,&offset,&syserr);
    if (ret<=0) continue;
    TrLadGain* ladgain = (TrLadGain*) FindGainTkId(tkid); 
    if (ladgain==0) continue; 
    ladgain->SetGain(iva,gain);
    ladgain->SetOffset(iva,offset);
    ladgain->SetSysErr(iva,syserr);
    ladgain->SetStatus(iva,int((bit0<<0)|(bit1<<1)|(bit2<<2)|(bit3<<3)|(bit4<<4)));
  }
  fclose(file);
  return false;
}


bool TrGainDB::LoadFromTrParDBFile(const char* filename) {
  Init();
  TrParDB::Head->Load(filename);
  for (int i=0; i<TrParDB::Head->GetEntries(); i++) {
    TrLadPar* ladpar = TrParDB::Head->GetEntry(i);
    if (ladpar==0) continue;
    TrLadGain* ladgain = (TrLadGain*) FindGainHwId(ladpar->GetHwId());
    if (ladgain==0) continue;
    for (int iva=0; iva<16; iva++) {
      int   iside = (iva<10) ? 1 : 0;
      float lad_gain = ladpar->GetGain(iside);
      float va_gain = ladpar->GetVAGain(iva);
      float gain = va_gain*lad_gain;
      ladgain->SetGain(iva,gain);
      ladgain->SetOffset(iva,0);
      ladgain->SetSysErr(iva,0);
      ladgain->SetStatus(iva,0);
      if (gain<0.02) { // old definition of very bad
        ladgain->SetGain(iva,1); 
        ladgain->SetStatus(iva,0x3);
      }
    }
  }
  return true;
}


bool TrGainDB::GainDBToLinear() {
  Init();
  int offset = 0;
  for (TrGainIt ladgain=fTrGainHwIdMap.begin(); ladgain!=fTrGainHwIdMap.end(); ++ladgain) {
    TrLadGain* gain = (*ladgain).second;
    gain->GainDBToLinear(fLinear+offset);
    offset += TrLadGain::GetSize();
  }
  return true;
}


bool TrGainDB::LinearToGainDB() {
  Init();
  int offset = 0;
  for (TrGainIt ladgain=fTrGainHwIdMap.begin(); ladgain!=fTrGainHwIdMap.end(); ++ladgain) {
    TrLadGain* gain = (*ladgain).second;
    gain->LinearToGainDB(fLinear+offset);
    offset += TrLadGain::GetSize();
  }
  return true;
}


void TrGainDB::PrintLinear() {
  Init();
  printf("TrGainDB::PrintLinear-V content of the linear database:\n");
  for (int i=0; i<GetSize(); i++) 
    printf("%4d %20.10g\n",i,fLinear[i]);
  printf("TrGainDB::PrintLinear-V content of the linear database --- END\n");
}


bool TrGainDB::SaveInTDV(long int start_time, long int validity,int isReal) {
  Init();
  time_t statime = time_t(start_time);
  time_t endtime = time_t(start_time+validity); 
  GainDBToLinear();
  tm beg, end;
  localtime_r(&statime,&beg);
  localtime_r(&endtime,&end);
  AMSTimeID* tdv = new AMSTimeID(
    AMSID("TrackerVAGains",isReal),beg,end,TrGainDB::GetLinearSize(),TrGainDB::GetLinear(),AMSTimeID::Standalone,1
  );
  tdv->UpdateMe();
  tdv->write(AMSDATADIR.amsdatabase);
  if (tdv) delete tdv;
  return true;
} 


int TrGainDB::LoadFromTDV(long int time, int isReal) {
  Init();
  time_t tt = time_t(time);
  tm begin;
  tm end;
  begin.tm_isdst = end.tm_isdst = 0;
  begin.tm_sec   = begin.tm_min = begin.tm_hour =
  begin.tm_mday  = begin.tm_mon = begin.tm_year = 0;
  end.  tm_sec   = end.  tm_min = end.  tm_hour =
  end.  tm_mday  = end.  tm_mon = end.  tm_year = 0;
  TkDBc::CreateLinear();
  AMSTimeID *tdv = new AMSTimeID(
    AMSID("TrackerVAGains",isReal),begin,end,TrGainDB::GetLinearSize(),TrGainDB::GetLinear(),AMSTimeID::Standalone,1,FunctionLinearToGainDB 
  );
  int ret = tdv->validate(tt);
  if (tdv) delete tdv;
  return ret;
}


void FunctionLinearToGainDB(){
  if(!TrGainDB::IsNull()) TrGainDB::GetHead()->LinearToGainDB();
}


float TrGainDB::GetGainCorrected(float adc, int tkid, int iva) {
  if ( (iva<0)||(iva>15) ) return 0;
  TrLadGain* ladgain = (TrLadGain*) FindGainTkId(tkid);
  if (ladgain==0) return 0;
  return ladgain->GetGainCorrected(adc,iva);   
}


TH2D* gain_map = 0;
TH2D* TrGainDB::GetGainHistogram() {
  if (!gain_map) gain_map = new TH2D("gain_map","; ladder number; VA number; gain",192,-0.5,191.5,16,-0.5,15.5);
  if (gain_map)  gain_map->Clear();
  for (int icrate=0; icrate<8; icrate++) {
    for (int itdr=0; itdr<24; itdr++) {
      int iladder = icrate*24 + itdr;
      int hwid = icrate*100 + itdr;
      TrLadGain* ladgain = (TrLadGain*) FindGainHwId(hwid);
      if (ladgain==0) return 0;
      for (int iva=0; iva<16; iva++) {
        float gain = ladgain->GetGain(iva); 
        gain_map->SetBinContent(iladder+1,iva+1,gain);
      }
    }
  }
  return gain_map;
}


///////////////////////////
// TrLadGain
///////////////////////////


TrLadGain::TrLadGain(int hwid, float* gain, float* offset, float* syserr, int* status) { 
  Clear(); 
  SetHwId(hwid); 
  SetGain(gain); 
  SetOffset(offset); 
  SetSysErr(syserr); 
  SetStatus(status);        
}


void TrLadGain::Clear(Option_t* option) {
  fHwId = 0;
  for (int iva=0; iva<16; iva++) {
    fGain[iva] = 1;
    fOffset[iva] = 0;
    fSysErr[iva] = 0;
    fStatus[iva] = 0;
  }
  TObject::Clear(option);
}


void TrLadGain::Info(int verbosity) {
  printf(" %10s %3d","HwId: ",GetHwId());
  if (verbosity>0) printf("\n");
  printf(" %10s ","Gain: ");
  for (int iva=0; iva<16; iva++) printf("%8.4f ",GetGain(iva));
  printf("\n");
  if (verbosity>0) { 
    printf(" %10s ","Offset: ");
    for (int iva=0; iva<16; iva++) printf("%8.4f ",GetOffset(iva));
    printf("\n");
    printf(" %10s ","SysErr: ");
    for (int iva=0; iva<16; iva++) printf("%8.4f ",GetSysErr(iva));
    printf("\n");
    printf(" %10s ","Status: ");
    for (int iva=0; iva<16; iva++) printf("%8hX ",GetStatus(iva));
    printf("\n");
  }
}


void TrLadGain::Dump(int iva) {
  if ( (iva<0)||(iva>15) ) return;
  printf("%03d %2d %8.4f %8.4f %8.4f %8d\n",GetHwId(),iva,GetGain(iva),GetOffset(iva),GetSysErr(iva),GetStatus(iva));
}


bool TrLadGain::GainDBToLinear(float* offset) {
  if (!offset) return false;
  int index = 0;
  offset[index++] = (float) GetHwId();
  for (int iva=0; iva<16; iva++) offset[index++] = (float) GetGain(iva);
  for (int iva=0; iva<16; iva++) offset[index++] = (float) GetOffset(iva);
  for (int iva=0; iva<16; iva++) offset[index++] = (float) GetSysErr(iva);
  for (int iva=0; iva<16; iva++) offset[index++] = (float) GetStatus(iva);
  return true;
}


bool TrLadGain::LinearToGainDB(float* offset) {
  if (!offset) return false;
  int index = 0;
  SetHwId((int)offset[0]);
  SetGain(offset+1);
  SetOffset(offset+1+16);
  SetSysErr(offset+1+16+16);
  SetStatus(offset+1+16+16+16);
  return true;
}


float TrLadGain::GetGainCorrected(float adc, int iva) { 
  if ( (iva<0)||(iva>15) ) return 0;
  if (!IsSilver(iva)) return 0;
  return (adc + GetOffset(iva))*GetGain(iva);
}

