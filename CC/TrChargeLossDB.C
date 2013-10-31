// $Id: TrChargeLossDB.C,v 1.4 2013/10/31 18:23:06 choutko Exp $

#include "TrChargeLossDB.h"


#include "timeid.h"
#include "commonsi.h"


#define CHARGELOSS_ZMAX 26


ClassImp(TrChargeLossDB);
ClassImp(TrChargeLossTable);


///////////////////////////
// TrChargeLossDB
///////////////////////////


TrChargeLossDB* TrChargeLossDB::fHead = 0;
map<int,TrChargeLossTable*> TrChargeLossDB::fTrChargeLossMap;
bool TrChargeLossDB::fInitDone = false;


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


void TrChargeLossDB::Init(bool force) {
  if (IsNull()) return;
  if ( (fInitDone)&&(!force) ) return;
  if (LoadDefaultTablesVer0()) printf("TrChargeLossDB::GetHead()-V TrChargeLossDBver0 correction loaded, %d tables in memory.\n",(int)fTrChargeLossMap.size());
  else                         printf("TrChargeLossDB::GetHead()-V TrChargeLossDBver0 correction NOT loaded.\n"); 
  if (LoadDefaultTablesVer1()) printf("TrChargeLossDB::GetHead()-V TrChargeLossDBver1 correction loaded, %d tables in memory.\n",(int)fTrChargeLossMap.size());
  else                         printf("TrChargeLossDB::GetHead()-V TrChargeLossDBver1 correction NOT loaded.\n"); 
  fInitDone = true;
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
  fInitDone = false;
}


void TrChargeLossDB::Info() {
  for (map<int,TrChargeLossTable*>::iterator it = fTrChargeLossMap.begin(); it !=  fTrChargeLossMap.end(); it++) {
    printf("TrChargeLossDB::Info-V table with index %3d info:\n",it->first);
    it->second->Info();
  }
}


bool TrChargeLossDB::AddTable(int index, TrChargeLossTable* table) {
  if (table==0) return false;
  pair<map<int,TrChargeLossTable*>::iterator,bool> ret;
  // insert
  table->SetIndex(index);
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
  for (int Z=1; Z<=CHARGELOSS_ZMAX; Z++) {
    TrChargeLossTable* table = GetTable(type,Z,0);
    if (table==0) continue;
    zzz1 = Z;
    ref1 = table->GetMPVRef();
    mpv1 = table->GetMPVValueOld(ip,ia);
    if ( (adc>=mpv0)&&(adc<mpv1) ) break;
    zzz0 = Z;
    ref0 = table->GetMPVRef();
    mpv0 = table->GetMPVValueOld(ip,ia);    
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


double TrChargeLossDB::GetChargeLossCorrectedValue(int iside, int inter, double ip, double ia, double adc, int ver) { 
  // calculate type 
  int type = TrChargeLossTable::CreateType(iside,inter,ver);
  if (type<0) {
    printf("TrChargeLossDB::GetChargeLossCorrectedValue-E wrong type calculation (side=%d, inter=%d, ver=%d). No correction applied.\n",iside,inter,ver);
    return adc;
  }
  // algorithm is in sqrt(ADC)
  double x0 = (adc>0) ? sqrt(adc) : 0;
  // determine the sqrt(ADC) to Q relation
  double x[100] = {0};
  double y[100] = {0};
  int    n = 0;
  // first point forced to 0,0
  x[0] = 0;
  y[0] = 0;
  n++;
  // loop on tables
  double mpv_previous = 0;
  int    z_previous = 0;
  for (int Z=1; Z<=CHARGELOSS_ZMAX; Z++) {
    TrChargeLossTable* table = GetTable(type,Z,ver);
    // no table
    if (table==0) continue;
    double mpv = table->GetMPVValue(ip,ia);
    // not determined
    if (mpv<=TrChargeLossTable::fBadValue) continue; 
    // not monotonic
    if (mpv<=mpv_previous) continue;
    // no steps
    if ((Z-z_previous)/(sqrt(mpv)-sqrt(mpv_previous))>10) continue; 
    x[n] = sqrt(mpv);
    y[n] = 1.0*Z;
    n++;
    // previous storing 
    mpv_previous = mpv;
    z_previous = Z;
  }
  // error message
  static int maxerr = 0;
  if (n<2) {
    if (maxerr<100) {
      printf("TrChargeLossDB::GetChargeLossCorrectedValue-E no valid correction tables available for ip=%f ia=%f type=%d ver=%d, return 0 (only displayed 100 times).\n",ip,ia,type,ver);
      maxerr++;
    }
    return 0;
  }
  // interpolate 
  double y0 = monotonic_cubic_interpolation(x0,n,x,y);
  return y0*y0;
}


bool TrChargeLossDB::LoadDefaultTablesVer0(char* dirname) {
  // Load now
  int Zlist[14] = {1,2,3,4,5,6,7,8,9,10,11,12,14,26};
  for (int iz=0; iz<14; iz++) {
    int Z = Zlist[iz];
    // N-side
    TrChargeLossTable* tableX = new TrChargeLossTable();
    bool loadX = tableX->InitTableFromTxtFile(Form("%s/v5.00/TrChargeLossDBver0/ChargeLossTable_N_Z%02d.txt",dirname,Z));
    if (loadX) AddTable(tableX,0,Z,0);
    // P-side
    TrChargeLossTable* tableY = new TrChargeLossTable();
    bool loadY = tableY->InitTableFromTxtFile(Form("%s/v5.00/TrChargeLossDBver0/ChargeLossTable_P_Z%02d.txt",dirname,Z));
    if (loadY) AddTable(tableY,1,Z,0);
  }
  return (int(fTrChargeLossMap.size())>0);
}


bool TrChargeLossDB::LoadDefaultTablesVer1(char* dirname) {
  for (int Z=1; Z<=26; Z++) {
    // 3S-side
    TrChargeLossTable* table3S = new TrChargeLossTable();
    bool load3S = table3S->InitTableFromTxtFile(Form("%s/v5.00/TrChargeLossDBver1/ChargeLossTable_3S_Z%02d.txt",dirname,Z));
    if ( (Z==26)&&(load3S) ) table3S->FillGaps(); // complete the last table
    // table3S->FillGaps(); 
    if (load3S) AddTable(table3S,1,Z,1);
    else delete table3S;
    // 1K-side
    TrChargeLossTable* table1K = new TrChargeLossTable();
    bool load1K = table1K->InitTableFromTxtFile(Form("%s/v5.00/TrChargeLossDBver1/ChargeLossTable_1K_Z%02d.txt",dirname,Z));
    if ( (Z==26)&&(load1K) ) table1K->FillGaps(); // complete the last table
    // table1K->FillGaps(); 
    if (load1K) AddTable(table1K,2,Z,1);
    else delete table1K;
    // 0K-side
    TrChargeLossTable* table0K = new TrChargeLossTable();
    bool load0K = table0K->InitTableFromTxtFile(Form("%s/v5.00/TrChargeLossDBver1/ChargeLossTable_0K_Z%02d.txt",dirname,Z));
    if ( (Z==26)&&(load0K) ) table0K->FillGaps(); // complete the last table
    // table0K->FillGaps(); 
    if (load0K) AddTable(table0K,3,Z,1);
    else delete table0K;
  }
  //! Fix gaps in the tables
  FillGaps(1);
  FillGaps(2);
  FillGaps(3); 
  return (int(fTrChargeLossMap.size())>0);
}


static TGraph* correction_graph = 0;
TGraph* TrChargeLossDB::GetCorrectionGraph(int type, double ip, double ia, int ver) {
  // clean graph
  if (correction_graph!=0) {
    delete correction_graph;
    correction_graph = 0;
  }
  // init
  float x[100] = {0};
  float y[100] = {0};
  int n = 0;
  for (int Z=1; Z<=CHARGELOSS_ZMAX; Z++) {
    TrChargeLossTable* table = GetTable(type,Z,ver);
    if (table==0) continue; // no table
    double v = table->GetMPVValue(ip,ia);
    if (v<=TrChargeLossTable::fBadValue) continue; // not determined
    x[n] = sqrt(table->GetMPVValue(ip,ia));
    y[n] = 1.0*Z;
    n++;
  }
  TGraph* graph = new TGraph(n,x,y);
  graph->SetName(Form("graph_T%02d",type));
  return graph;
}


void TrChargeLossDB::FillGaps(int type) {  
  // loop on charges
  for (int Z=1; Z<=CHARGELOSS_ZMAX; Z++) {
    TrChargeLossTable* table = GetTable(type,Z,1);
    if (!table) continue;
    // loop on all table elements
    for (int ipbin=0; ipbin<table->GetIPNBins(); ipbin++) {
      for (int iabin=0; iabin<table->GetIANBins(); iabin++) {
        double mpv = table->GetMPVValue(ipbin,iabin);
        // if one element is not valid evaluate it from interpolation
        if (mpv<TrChargeLossTable::fBadValue) { 
          double ip = table->GetIP(ipbin);
          double ia = table->GetIA(iabin);
          double interpolation = GetMPVValueChargeInterpolation(type,ip,ia,Z,1);
          table->SetMPVValue(ipbin,iabin,interpolation);
        }
      }
    }
  }
}


double TrChargeLossDB::GetMPVValueChargeInterpolation(int type, double ip, double ia, int z, int ver) {
  // interpolation point
  double x0 = 1.0*z;
  // determine the sqrt(ADC) to Q relation
  double x[100] = {0};
  double y[100] = {0};
  int    n = 0;
  // first point forced to 0,0
  x[0] = 0;
  y[0] = 0;
  n++;
  // loop on tables
  double mpv_previous = 0;
  int z_previous = 0;
  int ibin_start = 0;
  for (int Z=1; Z<=CHARGELOSS_ZMAX; Z++) {
    if (z==Z) continue;
    TrChargeLossTable* table = GetTable(type,Z,ver);
    // no table
    if (table==0) continue;
    double mpv = table->GetMPVValue(ip,ia); 
    // not determined
    if (mpv<=TrChargeLossTable::fBadValue) continue;
    // not monotonic
    if (mpv<=mpv_previous) continue;
    // no steps
    if ((Z-z_previous)/(sqrt(mpv)-sqrt(mpv_previous))>10) continue; 
    // start for the bin search (will be a bit faster)  
    if (x0>Z) ibin_start = n;
    y[n] = sqrt(mpv);
    x[n] = 1.0*Z;
    n++;
  }
  // find interval 
  int ibin = 0;
  for (int i=ibin_start; i<n-1; i++) {
    if ( (x0>=x[i])&&(x0<x[i+1]) ) {
      ibin = i;
      break;
    }
  }
  // treat underflow and overflows
  if (x0<x[0]) ibin = 0;
  if (x0>=x[n-1]) ibin = n-2;
  // linear interpolation
  double y0 = y[ibin] + (x0-x[ibin])*(y[ibin+1]-y[ibin])/(x[ibin+1]-x[ibin]); 
  return y0*y0;
}


///////////////////////////
// TrChargeLossTable
///////////////////////////


double TrChargeLossTable::fBadValue = 2; 


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


TrChargeLossTable::TrChargeLossTable(int ipnbins, double ipmin, double ipmax, int ianbins, double iamin, double iamax, double ref) {
  fIndex = -1;
  fIPNBins = 0;
  fIPMin = 0;
  fIPMax = 0;
  fIANBins = 0;
  fIAMin = 0;
  fIAMax = 0;
  fMPVRef = 0;
  fMPVTable = 0;
  Init(ipnbins,ipmin,ipmax,ianbins,iamin,iamax,ref);
}


void TrChargeLossTable::Init(int ipnbins, double ipmin, double ipmax, int ianbins, double iamin, double iamax, double ref) {
  Clear();
  fIndex = -1;
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
  printf("Type: %3d   Z: %3d   Ver: %2d   NIPBins: %3d   IPMin: %7.3f   IPMax: %7.3f   NIABins: %3d   IAMin: %7.3f   IAMax: %7.3f   MPVRef: %7.3f\n",
    GetType(),GetZ(),GetVersion(),GetIPNBins(),GetIPMin(),GetIPMax(),GetIANBins(),GetIAMin(),GetIAMax(),GetMPVRef());
  for (int iabin=0; iabin<GetIANBins(); iabin++) {
    for (int ipbin=0; ipbin<GetIPNBins(); ipbin++) {
      printf("%8.2f ",GetMPVValue(ipbin,iabin));
    }
    printf("\n");
  }
  printf("\n");
}


int TrChargeLossTable::CreateType(int iside, int inter, int ver) {
  // calculate type 
  int type = -1;
  if (iside==1) type = 1;    // all  vers: 3S (used also for 1-strip clusters or edges)
  if (iside==0) {
    type = 2;                // only ver1: 1K (used also for 1-strip clusters or edges)
    if (inter==0) type = 3;  // only ver1: 0K (present only on K7)
    if (ver==0) type = 0;    // in   ver0: 1K = 0K = K5  
  }
  return type;
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
  // ia fixed extrapolation
  if (iabin<0) return GetMPVValue(ipbin,0);
  else         return GetMPVValue(ipbin,GetIANBins() - 1);
}


double TrChargeLossTable::GetMPVValueOnGrid(double ip, double ia) {
  if (!fMPVTable) return 0;
  return GetMPVValue(GetIPBin(ip),GetIABin(ia));
}


double TrChargeLossTable::GetMPVValueBilinearExtrapolation(double ip, double ia, bool old) {
  if (!fMPVTable) return 0;
  // impact point
  double ipstep = (GetIPMax() - GetIPMin())/GetIPNBins();
  int    ipbin_low = int(floor((ip - GetIPMin())/ipstep - 0.5)); // closest lower bin center
  double ip_low = GetIP(ipbin_low);
  if (old) {
    // binning used in the past (approx ok), here for backward compat.
    ipbin_low = int(floor((ip - GetIPMin())/ipstep)); 
    ip_low = GetIPMin() + ipbin_low*ipstep; 
  }
  double dip = (ip - ip_low)/ipstep; // position wrt bin center
  // impact angle
  double iastep = (GetIAMax() - GetIAMin())/GetIANBins(); 
  int    iabin_low = int(floor((ia - GetIAMin())/iastep - 0.5)); // closest lower bin center
  double ia_low = GetIA(iabin_low);
  if (old) {
    // binning used in the past (approx ok), here for backward compat.
    iabin_low = int(floor((ia - GetIAMin())/iastep)); 
    ia_low = GetIAMin() + iabin_low*iastep; 
  }
  double dia = (ia - ia_low)/iastep; // position wrt bin center
  // bilinear interpolation parameters
  double dx[4] = {dip,1-dip,dip,1-dip};
  double dy[4] = {dia,dia,1-dia,1-dia};
  double Q[4]  = {
    GetMPVValueWithExtrapolation(ipbin_low+1,iabin_low+1),
    GetMPVValueWithExtrapolation(ipbin_low,  iabin_low+1),
    GetMPVValueWithExtrapolation(ipbin_low+1,iabin_low),
    GetMPVValueWithExtrapolation(ipbin_low,  iabin_low)
  };
  // count how many bad points
  int nbadpoints = 0;
  for (int i=0; i<4; i++) if (Q[i]<=TrChargeLossTable::fBadValue) nbadpoints++;
  // if numeber of bad points > 1 return the closest value
  if (nbadpoints>=1) return GetMPVValueOnGrid(ip,ia);
  // interpolation calculation 
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


void TrChargeLossTable::FillGaps() {
  // fill extreme angles
  for (int ipbin=0; ipbin<GetIPNBins(); ipbin++) {
    if ( (GetMPVValue(ipbin,0)<=fBadValue)&&(GetMPVValue(ipbin,1)>fBadValue) ) {
      SetMPVValue(ipbin,0,GetMPVValue(ipbin,1));
      // use of symmetry
      SetMPVValue(ipbin,GetIANBins()-1,GetMPVValue(ipbin,GetIANBins()-2));
    }
  }
  // fill holes in ip direction  
  for (int ipbin=1; ipbin<int(GetIPNBins()/2); ipbin++) {
    for (int iabin=0; iabin<GetIANBins(); iabin++) {
      if ( (GetMPVValue(ipbin,iabin)<=fBadValue)&&(GetMPVValue(ipbin-1,iabin)>fBadValue) ) {
        SetMPVValue(ipbin,iabin,GetMPVValue(ipbin-1,iabin));
        // use of symmetry
        SetMPVValue(GetIPNBins()-1-ipbin,iabin,GetMPVValue(ipbin-1,iabin));
      }
    }
  }
}


bool TrChargeLossTable::InitTableFromTxtFile(char* filename) { 
  FILE* file = fopen(filename,"r");
  if (file==0) return false;
  // printf("TrChargeLossTable::InitTableFromTxtFile-V init table from file %s\n",filename); // debug
  int z,type,ipnbins,ianbins;
  float ipmin,ipmax,iamin,iamax,ref;
  fscanf(file,"%d%d%d%f%f%d%f%f%f",&type,&z,&ipnbins,&ipmin,&ipmax,&ianbins,&iamin,&iamax,&ref);
  Init(ipnbins,ipmin,ipmax,ianbins,iamin,iamax,ref);    
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


///////////////////////////
// Service functions
///////////////////////////


double monotonic_cubic_interpolation(double xx, int n, double* x, double* y) {
  /*
  // M. Steffen, Astron. Astrophys. 239, 443-450 (1990)
  // A simple method for monotonic interpolation in one dimension
  // (some modification of mine to deal with extrema)
  */
  // outside: linear extrapolation with first/last two points
  if (xx!=xx){
   cerr<<"monotonic_cubic_interpolation-E-NaNInput "<<endl;
   return 0;
  }
  if (xx<x[0])    return y[0] + (y[1]-y[0])/(x[1]-x[0])*(xx-x[0]);
  if (xx>=x[n-1]) return y[n-2] + (y[n-1]-y[n-2])/(x[n-1]-x[n-2])*(xx-x[n-2]);
  // find position (binary search)
  int l = (n-1);
  int f = 0;
  int ibin = int(f+(l-f)/2);
  int ntry=0;
  while (!((xx>=x[ibin])&&(xx<x[ibin+1])) ) {
    if (xx<x[ibin])    { l = ibin;   ibin = int(f+(l-f)/2); }
    if (xx>=x[ibin+1]) { f = ibin+1; ibin = int(f+(l-f)/2); }
    if(ntry++>n){
       cerr<<"monotonic_cubic_interpolation-E-InfiniteCycle "<<ntry<<" "<<n<<" "<<xx<<endl;
        for(int i=0;i<n;i++)cerr<<" x[i] "<<i<<" "<<x[i]<<endl;
       ibin=0; 
       break;
    }
  }
  // monotonic cubic spline
  // special treatment for boundary (match with linear extrapolation)  
  int imin = (ibin==0)   ? 1 : 0;
  int imax = (ibin==n-2) ? 2 : 3;
  double h[3]  = {0,0,0};
  double s[3]  = {0,0,0};
  double S[3]  = {0,0,0};
  double p[3]  = {0,0,0};
  double y1[3] = {0,0,0};
  for (int i=imin; i<imax; i++) {
    h[i] = x[ibin-1+i+1]-x[ibin-1+i];
    s[i] = (y[ibin-1+i+1]-y[ibin-1+i])/h[i];
    S[i] = s[i]/fabs(s[i]);
  }
  // set for first bin 
  y1[1] = s[1];
  // set for last bin 
  y1[2] = s[1];
  for (int i=imin+1; i<imax; i++) {
    p[i]  = (s[i-1]*h[i]+s[i]*h[i-1])/(h[i]+h[i-1]);
    y1[i] = (S[i-1]+S[i])*(TMath::Min(TMath::Min(fabs(s[i-1]),fabs(s[i])),0.5*fabs(p[i])));
  }
  double a = (y1[1] + y1[2] - 2*s[1])/(h[1]*h[1]);
  double b = (3*s[1] - 2*y1[1] - y1[2])/h[1];
  double c = y1[1];
  double d = y[ibin];
  return a*pow(xx-x[ibin],3)+b*pow(xx-x[ibin],2)+c*(xx-x[ibin])+d;
}


