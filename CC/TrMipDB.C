#include "TrMipDB.h"
#include "TrChargeLossDB.h" // for cubic monotonic spline


ClassImp(TrMipDB);
ClassImp(TrMipElem);


///////////////////////////
// TrMipDB
///////////////////////////


TrMipDB* TrMipDB::fHead = 0;
map<int,TrMipElem*> TrMipDB::fTrMipElemIndexMap;
bool TrMipDB::fInitDone = false;


TrMipDB* TrMipDB::GetHead() {
  if (IsNull()) {
    printf("TrMipDB::GetHead()-V TrMipDB singleton initialization.\n");
    fHead = new TrMipDB(); 
  } 
  return fHead;
}


TrMipDB::~TrMipDB() { 
  Clear();
  if (fHead) delete fHead;
  fHead = 0;
}


void TrMipDB::Init(bool force) {
  if (IsNull()) return;
  if ( (fInitDone)&&(!force) ) return;  
  if (fTrMipElemIndexMap.empty()) {
    for (int index=0; index<192*16+192*2+9*2+2; index++) {
      fTrMipElemIndexMap[index] = new TrMipElem(index);
      // set first point to 0,0
      TrMipElem* elem = fTrMipElemIndexMap[index];
      elem->AddPoint(0,0); 
    }
    printf("TrMipDB::Init()-V %d elements created.\n",(int)fTrMipElemIndexMap.size());
  }
  if (LoadDefaultTablesVer1()) printf("TrMipDB::Init()-V correction tables loaded succesfully.\n");
  else                         printf("TrMipDB::Init()-V correction tables NOT loaded.\n");
  fInitDone = true; 
} 


void TrMipDB::Clear(Option_t* option) { 
  // delete all elements
  for (map<int,TrMipElem*>::iterator it=fTrMipElemIndexMap.begin(); it!=fTrMipElemIndexMap.end(); it++) {
    if (it->second) {
      it->second->Clear(option);
      delete it->second;
    }
    it->second = 0;
  }
  fTrMipElemIndexMap.clear();
  fInitDone = false;
}


void TrMipDB::Info(int verbosity) {
  for (map<int,TrMipElem*>::iterator it=fTrMipElemIndexMap.begin(); it!=fTrMipElemIndexMap.end(); it++) {
    it->second->Info(verbosity);
  }
}


TrMipElem* TrMipDB::GetElem(int index) {
  map<int,TrMipElem*>::iterator it=fTrMipElemIndexMap.find(index);
  return (it!=fTrMipElemIndexMap.end()) ? it->second : 0;
}


TrMipElem* TrMipDB::GetElem(int tkid, int iva, int depth) {
  int index = CreateIndex(tkid,iva,depth);
  return GetElem(index);
}


TrMipElem* TrMipDB::GetValidElem(int tkid, int iva, int depth) {
  // if not valid at that depth decrease the level 
  for (int idepth=depth; idepth>=0; idepth--) {
    TrMipElem* elem = GetElem(tkid,iva,idepth);  
    if (!elem) continue;
    if (elem->IsValid()) return elem;
  }
  return 0;
}


bool TrMipDB::LoadDefaultTablesVer1(char* dirname) {
  // clear
  for (map<int,TrMipElem*>::iterator it=fTrMipElemIndexMap.begin(); it!=fTrMipElemIndexMap.end(); it++) {
    if (it->second) {
      TrMipElem* elem = it->second;
      elem->Clear();
      elem->AddPoint(0,0);
    }
  }
  // add points
  int nfiles = 0;
  for (int Z=1; Z<=MIP_ZMAX; Z++) {
    FILE* file = fopen(Form("%s/v5.00/TrMipDBver1/TrMipDB_Z%02d.txt",dirname,Z),"r");
    if (!file) continue;
    nfiles++;
    while (!feof(file)) {
      int z,tkid,index,entries,isok,ndf;
      float mpv,pars[4],epars[4],err,chisq,chi;
      int ret = fscanf(file,"%d%d%d%d%d%f%f%f%f%f%f%f%f%f%f%f%f%d",
        &z,&tkid,&index,&entries,&isok,&mpv,&err,&chisq,
        &pars[0],&epars[0],&pars[1],&epars[1],&pars[2],&epars[2],&pars[3],&epars[3],&chi,&ndf 
      ); 
      // not read 
      if (ret<=0) continue;
      // not ok
      if (isok<0) continue;
      // minimum number of entries
      if (entries<10) continue;
      // failure 
      if (mpv<=0) continue;
      // bad evaluation
      if (mpv/z/z<0.15) continue;
      // by the way, does it exist?
      TrMipElem* elem = GetElem(index);
      if (!elem) continue;
      // check monotonicity 
      if (sqrt(mpv)<elem->GetMPV(elem->GetN()-1)) continue;
      // ok 
      elem->AddPoint(z,sqrt(mpv));
    }
    fclose(file);
  }
  return (nfiles>0);
}


int TrMipDB::CreateIndex(int tkid, int iva, int depth) {
  // check 
  if ( (iva<0)||(iva>15) ) {
    printf("TrMipDB::CreateIndex-W not valid requested VA (%02d). No correction will be applied.\n",iva);
    return -1;
  }
  // side
  int iside = (iva<10) ? 1 : 0; // X or Y 
  // whole tracker index
  if (depth==0) return 192*16 + 192*2 + 9*2 + iside; 
  TkLadder* ladder = TkDBc::Head->FindTkId(tkid);
  if (!ladder) { 
    printf("TrMipDB::CreateIndex-W requested tkid (%+04d) not found in TkDBc. I will use default correction of the whole Tracker.\n",tkid);
    return 192*16 + 192*2 + 9*2 + iside; 
  }
  // layer index
  int ilayer = ladder->GetLayerJ()-1;
  if (depth==1) return 192*16 + 192*2 + 9*iside + ilayer;
  // ladder index
  int iladder = ladder->GetCrate()*24 + ladder->GetTdr();
  if (depth==2) return 192*16 + 192*iside + iladder;  
  // va level  
  return iladder*16 + iva;
}


double TrMipDB::GetMipCorrectedValue(double Q, int tkid, int iva, int depth) {
  TrMipElem* elem = GetValidElem(tkid,iva,depth);  
  if (!elem) {
    printf("TrMipDB::GetCorrection-E no correction element found (tkid=%+04d,iva=%02d,depth=%1d). Return unmodified value.\n",tkid,iva,depth);
    return Q;
  }
  return elem->GetMipCorrectedValue(Q);
}


///////////////////////////
// TrMipElem
///////////////////////////


TrMipElem::TrMipElem(int index) {
  Clear();
  fIndex = index;
}


void TrMipElem::Clear(Option_t* option) {
  // no index reset, only graph reset
  fN = 0;
  for (int iz=0; iz<MIP_ZMAX; iz++) {
    fZ[iz] = 0;
    fMPV[iz] = 0;
  }
}


void TrMipElem::AddPoint(double z, double mpv) {
  fZ[fN] = z;
  fMPV[fN] = mpv;
  fN++;
}


void TrMipElem::Info(int verbosity) {
  printf("TrMipElem::Info-V:   Index: %4d   NPoints: %2d   IsValid: %1d\n",GetIndex(),GetN(),IsValid());
  if (verbosity>0) {
    for (int i=0; i<GetN(); i++) {
      printf("  bin:%2d  Z:%7.0f  MPV:%7.3f\n",i,fZ[i],fMPV[i]);
    }
  }
}


bool TrMipElem::IsValid() {
  // not valid if no iron 
  if (fabs(fZ[fN-1]-26)>0.5) return false;
  // not valid if no protons 
  if (fabs(fZ[1]-1)>0.5) return false;
  // not valid if too few entries 
  if (fN<5) return false;
  return true;
}


double TrMipElem::GetMipCorrectedValue(double Q) {
  return monotonic_cubic_interpolation(Q,fN,fMPV,fZ); 
}


