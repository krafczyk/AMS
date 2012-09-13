// $Id: TrChargeLossDB.C,v 1.1 2012/09/13 15:54:20 oliva Exp $

#include "TrChargeLossDB.h"


#include "timeid.h"
#include "commonsi.h"


ClassImp(TrChargeLossDB);
ClassImp(TrChargeLossTable);


///////////////////////////
// TrChargeLossDB
///////////////////////////


TrChargeLossDB* TrChargeLossDB::fHead = 0;
map<int,TrChargeLossTable*> TrChargeLossDB::fTrChargeLossMap;


TrChargeLossDB* TrChargeLossDB::GetHead() {
  if (IsNull()) {
    printf("TrChargeLossDB::GetHead()-V TrChargeLossDB singleton initialization.\n");
    fHead = new TrChargeLossDB(); 
  } 
  return fHead;
}


TrChargeLossDB::~TrChargeLossDB() { 
  Clear();
  if (fHead) delete fHead;
  fHead = 0;
}


void TrChargeLossDB::Init() {
  if (LoadDefaultTablesVer0()) 
    printf("TrChargeLossDB::GetHead()-V TrChargeLossDBver0 correction loaded, %d tables read.\n",(int)fTrChargeLossMap.size());
  else  
    printf("TrChargeLossDB::GetHead()-V TrChargeLossDBver0 correction NOT loaded.\n"); // no tables
} 


void TrChargeLossDB::Clear(Option_t* option) { 
  // delete all tables
  for (map<int,TrChargeLossTable*>::iterator it = fTrChargeLossMap.begin(); it !=  fTrChargeLossMap.end(); it++) {
    if (it->second) {
      it->second->Clear();
      delete it->second;
    }
    it->second = 0;
  }
  fTrChargeLossMap.clear();
}


void TrChargeLossDB::Info() {
  for (map<int,TrChargeLossTable*>::iterator it = fTrChargeLossMap.begin(); it !=  fTrChargeLossMap.end(); it++) {
    printf("TrChargeLossDB::Info-V Table with index %3d info:\n",it->first);
    it->second->Info();
  }
}


bool TrChargeLossDB::AddTable(int index, TrChargeLossTable* table) {
  if (table==0) return false;
  pair<map<int,TrChargeLossTable*>::iterator,bool> ret;
  // insert
  ret = fTrChargeLossMap.insert(pair<int,TrChargeLossTable*>(index,table));
  // if the index already exists renew the table
  if (ret.second==false) fTrChargeLossMap.find(index)->second = table; 
  return true;
}


TrChargeLossTable* TrChargeLossDB::GetTable(int index) {
  map<int,TrChargeLossTable*>::iterator it = fTrChargeLossMap.find(index);
  return (it!=fTrChargeLossMap.end()) ? it->second : 0;
}


double TrChargeLossDB::GetChargeLossCorrectedValue(int type, double ip, double ia, double adc) { 
  // just take the two values coming from two near tables 
  double zzz0 = 0;
  double ref0 = 0;
  double mpv0 = 0;
  double zzz1 = 0;
  double ref1 = 0;
  double mpv1 = 0;
  for (int Z=1; Z<=26; Z++) {
    TrChargeLossTable* table = GetTable(type,Z);
    if (table==0) continue;
    zzz1 = Z;
    ref1 = table->GetMPVRef();
    mpv1 = table->GetMPVValue(ip,ia);
    if ( (adc>=mpv0)&&(adc<mpv1) ) break;
    zzz0 = Z;
    ref0 = table->GetMPVRef();
    mpv0 = table->GetMPVValue(ip,ia);    
  }
  // overflow
  if ( (zzz1==zzz0) ) {
    zzz1 = 100;
    ref1 = 100000;
    mpv1 = 100000;
  }
  // sqrt(MPV_Ref) vs sqrt(MPV)
  double  x  = sqrt(adc);
  double  y1 = sqrt(ref1);
  double  y0 = sqrt(ref0);
  double  x1 = sqrt(mpv1);
  double  x0 = sqrt(mpv0);
  double m = (y1 - y0) / (x1 - x0);
  double q = y0 - m*x0;
  double interp = m*x + q;
  interp *= interp; 
  // see the two values
  // printf("adc=%8.1f     z0=%8.1f ref0=%8.1f mpv0=%8.1f     z1=%8.1f ref1=%8.1f mpv1=%8.3f     interp=%8.1f\n",adc,zzz0,ref0,mpv0,zzz1,ref1,mpv1,interp);
  return interp;
} 


bool TrChargeLossDB::LoadDefaultTablesVer0(char* dirname) {
  // Clean up database
  Clear();
  // Load now
  int Zlist[14] = {1,2,3,4,5,6,7,8,9,10,11,12,14,26};
  for (int iz=0; iz<14; iz++) {
    int Z = Zlist[iz];
    // N-side
    TrChargeLossTable* tableX = new TrChargeLossTable();
    bool loadX = tableX->InitTableFromTxtFile(Form("%s/v5.00/TrChargeLossDBver0/ChargeLossTable_N_Z%02d.txt",dirname,Z));
    if (loadX) AddTable(0,Z,tableX);
    // P-side
    TrChargeLossTable* tableY = new TrChargeLossTable();
    bool loadY = tableY->InitTableFromTxtFile(Form("%s/v5.00/TrChargeLossDBver0/ChargeLossTable_P_Z%02d.txt",dirname,Z));
    if (loadY) AddTable(1,Z,tableY);
  }
  return (int(fTrChargeLossMap.size())>0);
}


///////////////////////////
// TrChargeLossTable
///////////////////////////


TrChargeLossTable::TrChargeLossTable() {
  fIndex = 0;
  fIPNBins = 0;
  fIPMin = 0;
  fIPMax = 0;
  fIANBins = 0;
  fIAMin = 0;
  fIAMax = 0;
  fMPVRef = 0;
  fMPVTable = 0;
}


TrChargeLossTable::TrChargeLossTable(int type, int z, int ipnbins, double ipmin, double ipmax, int ianbins, double iamin, double iamax, double ref) {
  fIndex = 0;
  fIPNBins = 0;
  fIPMin = 0;
  fIPMax = 0;
  fIANBins = 0;
  fIAMin = 0;
  fIAMax = 0;
  fMPVRef = 0;
  fMPVTable = 0;
  Init(type,z,ipnbins,ipmin,ipmax,ianbins,iamin,iamax,ref);
}


void TrChargeLossTable::Init(int type, int z, int ipnbins, double ipmin, double ipmax, int ianbins, double iamin, double iamax, double ref) {
  Clear();
  fIndex = type + z*10;
  fIPNBins = ipnbins;
  fIPMin = ipmin;
  fIPMax = ipmax;
  fIANBins = ianbins;
  fIAMin = iamin;
  fIAMax = iamax;
  fMPVRef = ref;
  if (!fMPVTable) fMPVTable = new double[GetIPNBins()*GetIANBins()];
  for (int ipbin=0; ipbin<GetIPNBins(); ipbin++) {
    for (int iabin=0; iabin<GetIANBins(); iabin++) {
      SetMPVValue(ipbin,iabin,0.); 
    }
  }
}


void TrChargeLossTable::Clear(Option_t* option) {
  fIndex   = 0;
  fIPNBins = 0;
  fIANBins = 0;
  fMPVRef  = 0;
  if (fMPVTable) delete [] fMPVTable;  
  fMPVTable = 0;
}


void TrChargeLossTable::Info() {
  printf("Type: %3d   Z: %3d   NIPBins: %3d   IPMin: %7.3f   IPMax: %7.3f   NIABins: %3d   IAMin: %7.3f   IAMax: %7.3f   MPVRef: %7.3f\n",
    GetType(),GetZ(),GetIPNBins(),GetIPMin(),GetIPMax(),GetIANBins(),GetIAMin(),GetIAMax(),GetMPVRef());
  for (int iabin=0; iabin<GetIANBins(); iabin++) {
    for (int ipbin=0; ipbin<GetIPNBins(); ipbin++) {
      printf("%7.2f ",GetMPVValue(ipbin,iabin));
    }
    printf("\n");
  }
  printf("\n");
}


double TrChargeLossTable::GetMPVValue(int ipbin, int iabin) {
  if (!fMPVTable) return 0;
  if ( (ipbin<0)||(ipbin>=GetIPNBins()) ) return 0;
  if ( (iabin<0)||(iabin>=GetIANBins()) ) return 0; 
  return fMPVTable[iabin*GetIPNBins()+ipbin];
}


double TrChargeLossTable::GetMPVValueWithExtrapolation(int ipbin, int iabin) {
  if (!fMPVTable) return 0;
  // no modifications
  if ( (ipbin>=0)&&(ipbin<GetIPNBins())&&(iabin>=0)&&(iabin<GetIANBins()) ) return GetMPVValue(ipbin,iabin);
  // folding ip position
  while (ipbin>=GetIPNBins()) ipbin -= GetIPNBins();
  while (ipbin<0)             ipbin += GetIPNBins();
  // only folding
  if ( (ipbin>=0)&&(ipbin<GetIPNBins())&&(iabin>=0)&&(iabin<GetIANBins()) ) return GetMPVValue(ipbin,iabin);
  /*
  // ia linear extrapolation
  int   x  = iabin;
  int   x0 = 0;
  int   x1 = 1;
  if (iabin>=GetIANBins()) { 
    x0 = GetIANBins() - 2;
    x1 = GetIANBins() - 1;
  }
  double y0 = GetMPVValue(ipbin,x0);
  double y1 = GetMPVValue(ipbin,x1); 
  double m  = (y1 - y0) / (x1 - x0);
  double q  = y0 - m*x0;
  return m*x + q;
  */
  // ia fixed extrapolation
  if (iabin<0) return GetMPVValue(ipbin,0);
  else         return GetMPVValue(ipbin,GetIANBins() - 1);
}


double TrChargeLossTable::GetMPVValueBilinearExtrapolation(double ip, double ia) {
  if (!fMPVTable) return 0;
  // impact point
  double ipwidth = GetIPMax() - GetIPMin();
  double ipstep = ipwidth/GetIPNBins();
  int    ipbin_low = floor((ip - GetIPMin())/ipstep);
  double dip = (ip - GetIP(ipbin_low))/ipstep;
  // impact angle
  double iawidth = GetIAMax() - GetIAMin();
  double iastep = iawidth/GetIANBins();
  int    iabin_low = floor((ia - GetIAMin())/iastep);
  double dia = (ia - GetIA(iabin_low))/iastep;
  // bilinear interpolation
  double dx[4] = {dip,1-dip,dip,1-dip};
  double dy[4] = {dia,dia,1-dia,1-dia};
  double Q[4]  = {
    GetMPVValueWithExtrapolation(ipbin_low+1,iabin_low+1),
    GetMPVValueWithExtrapolation(ipbin_low,  iabin_low+1),
    GetMPVValueWithExtrapolation(ipbin_low+1,iabin_low),
    GetMPVValueWithExtrapolation(ipbin_low,  iabin_low)
  };
  double interpolation = 0;
  for (int i=0; i<4; i++) interpolation += Q[i]*dx[i]*dy[i]; 
  return interpolation;
}


bool TrChargeLossTable::SetMPVValue(int ipbin, int iabin, double mpv) {
  if (!fMPVTable) return false;
  if ( (ipbin<0)||(ipbin>GetIPNBins()-1) ) return false;
  if ( (iabin<0)||(iabin>GetIANBins()-1) ) return false;
  fMPVTable[iabin*GetIPNBins()+ipbin] = mpv;    
  return true;
}


bool TrChargeLossTable::InitTableFromTxtFile(char* filename) { 
  FILE* file = fopen(filename,"r");
  if (file==0) return false;
  // printf("TrChargeLossTable::InitTableFromTxtFile-V init table from file %s\n",filename); // debug
  int z,type,ipnbins,ianbins;
  float ipmin,ipmax,iamin,iamax,ref;
  fscanf(file,"%d%d%d%f%f%d%f%f%f",&type,&z,&ipnbins,&ipmin,&ipmax,&ianbins,&iamin,&iamax,&ref);
  Init(type,z,ipnbins,ipmin,ipmax,ianbins,iamin,iamax,ref);    
  while (!feof(file)) {
    int   ipbin,iabin;
    float mpv,dum1,dum2; 
    int ret = fscanf(file,"%d%d%f%f%f",&ipbin,&iabin,&mpv,&dum1,&dum2);
    if (ret<=0) continue;
    SetMPVValue(ipbin-1,iabin-1,mpv);
  }
  fclose(file);
  return true;
}


void TrChargeLossTable::Multiply(double factor) {
  for (int ipbin=0; ipbin<GetIPNBins(); ipbin++) {
    for (int iabin=0; iabin<GetIANBins(); iabin++) {
      SetMPVValue(ipbin,iabin,factor*GetMPVValue(ipbin,iabin));
    }
  }
  SetMPVRef(factor*GetMPVRef());
}


TH2D* table_histo = 0;
TH2D* TrChargeLossTable::GetHistogram() {
  if (table_histo) {
    delete table_histo;
    table_histo = 0;
  }
  if (!table_histo) table_histo = new TH2D("table_histo","; IP; IA; MPV",GetIPNBins(),GetIPMin(),GetIPMax(),GetIANBins(),GetIAMin(),GetIAMax());
  table_histo->SetStats(kFALSE);
  for (int ipbin=0; ipbin<GetIPNBins(); ipbin++) {
    for (int iabin=0; iabin<GetIANBins(); iabin++) {
      table_histo->SetBinContent(ipbin+1,iabin+1,GetMPVValue(ipbin,iabin));
    }
  }
  return table_histo;
}


TH2D* TrChargeLossTable::GetHistogramExtended() {
  if (table_histo) {
    delete table_histo;
    table_histo = 0;
  }
  if (!table_histo) table_histo = new TH2D("table_histo","; IP index; IA index; MPV",3*GetIPNBins(),-1,1,3*GetIANBins(),-1,1);
  table_histo->SetStats(kFALSE);
  for (int ipbin=0; ipbin<3*GetIPNBins(); ipbin++) {
    for (int iabin=0; iabin<3*GetIANBins(); iabin++) {
      table_histo->SetBinContent(ipbin+1,iabin+1,GetMPVValueWithExtrapolation(ipbin-GetIPNBins(),iabin-GetIANBins()));
    }
  }
  return table_histo;
}


TH2D* TrChargeLossTable::GetHistogram(int ipnbins, double ipmin, double ipmax, int ianbins, double iamin, double iamax) {
  if (table_histo) {
    delete table_histo;
    table_histo = 0;
  }
  if (!table_histo) table_histo = new TH2D("table_histo","; IP; IA; MPV",ipnbins,ipmin,ipmax,ianbins,iamin,iamax);
  table_histo->SetStats(kFALSE);
  for (int ipbin=0; ipbin<ipnbins; ipbin++) {
    double ip = ipmin + ipbin*(ipmax-ipmin)/ipnbins;
    for (int iabin=0; iabin<ianbins; iabin++) {
      double ia = iamin + iabin*(iamax-iamin)/ianbins;
      table_histo->SetBinContent(ipbin+1,iabin+1,GetMPVValueBilinearExtrapolation(ip,ia));
    }
  }
  return table_histo;
}


