#include "TrLinearDB.h"


ClassImp(TrLinearDB);
ClassImp(TrLinearElem);


///////////////////////////
// TrLinearDB
///////////////////////////


TrLinearDB* TrLinearDB::fHead = 0;
map<int,TrLinearElem*> TrLinearDB::fTrLinearElemIndexMap;


int TrLinearDB::DefaultCorrDepth = 3;


TrLinearDB* TrLinearDB::GetHead() {
  if (IsNull()) {
    printf("TrLinearDB::GetHead()-V TrLinearDB singleton initialization.\n");
    fHead = new TrLinearDB(); 
  } 
  return fHead;
}


TrLinearDB::~TrLinearDB() { 
  Clear();
  if (fHead) delete fHead;
  fHead = 0;
}


void TrLinearDB::Init() {
  if (LoadDefaultTablesVer0()) 
    printf("TrLinearDB::GetHead()-V TrLinearDBver0 correction loaded, %d elements read.\n",(int)fTrLinearElemIndexMap.size());
} 


void TrLinearDB::Clear(Option_t* option) { 
  // delete all tables
  for (map<int,TrLinearElem*>::iterator it=fTrLinearElemIndexMap.begin(); it!=fTrLinearElemIndexMap.end(); it++) {
    if (it->second) {
      it->second->Clear();
      delete it->second;
    }
    it->second = 0;
  }
  fTrLinearElemIndexMap.clear();
}


void TrLinearDB::Info() {
  printf("TrLinearDB::Info-V:\n");
  for (map<int,TrLinearElem*>::iterator it=fTrLinearElemIndexMap.begin(); it!=fTrLinearElemIndexMap.end(); it++) {
    it->second->Info();
  }
}


bool TrLinearDB::AddElem(int index, TrLinearElem* elem) {
  if (elem==0) return false;
  pair<map<int,TrLinearElem*>::iterator,bool> ret;
  // insert
  ret = fTrLinearElemIndexMap.insert(pair<int,TrLinearElem*>(index,elem));
  // if the index already exists renew the table
  if (ret.second==false) fTrLinearElemIndexMap.find(index)->second = elem;
  return true;
}


TrLinearElem* TrLinearDB::GetElem(int index) {
  map<int,TrLinearElem*>::iterator it=fTrLinearElemIndexMap.find(index);
  return (it!=fTrLinearElemIndexMap.end()) ? it->second : 0;
}


TrLinearElem* TrLinearDB::GetElem(int tkid, int iva, int depth) {
  int index = CreateIndex(tkid,iva,depth);
  return GetElem(index);
}


TrLinearElem* TrLinearDB::GetValidElem(int tkid, int iva, int depth) {
  // if not valid at that depth decrease the level 
  for (int idepth=depth; idepth>=0; idepth--) {
    TrLinearElem* elem = GetElem(tkid,iva,idepth);  
    if (!elem) continue;
    if (elem->IsValid()) return elem;
  }
  return 0;
}


bool TrLinearDB::LoadDefaultTablesVer0(char* dirname) {
  FILE* file = fopen(Form("%s/v5.00/TrLinearDBver0/TrLinearDB.txt",dirname),"r");
  if (file==0) return false;
  while (!feof(file)) {
    int index,isok;
    float chisq,pars[5];
    int ret = fscanf(file,"%d%d%f%f%f%f%f%f",&index,&isok,&chisq,&pars[0],&pars[1],&pars[2],&pars[3],&pars[4]); 
    if (ret<=0) continue;
    AddElem(index,new TrLinearElem(index,isok,chisq,pars));
  }
  fclose(file);
  return true;
}


int TrLinearDB::CreateIndex(int tkid, int iva, int depth) {
  // whole tracker index
  if (depth==0) return 0;
  TkLadder* ladder = TkDBc::Head->FindTkId(tkid);
  if (!ladder) { 
    printf("TrLinearDB::CreateIndex-W requested tkid (%+04d) not found in TkDBc. I will use default correction of the whole Tracker.\n",tkid);
    return 0; 
  }
  // layer index
  if (depth==1) return ladder->GetLayerJ();
  // ladder index
  int index = 10+ladder->GetCrate()*24+ladder->GetTdr();
  if (depth==2) return index; 
  // va level  
  if ( (iva<0)||(iva>9) ) {
    printf("TrLinearDB::CreateIndex-W not valid requested VA (%02d). I will use default correction of the whole Tracker.\n",iva);
    return index;
  }
  index = (10+ladder->GetCrate()*24+ladder->GetTdr())*100+iva;
  return index;
}


double TrLinearDB::GetLinearityCorrected(double ADC, int tkid, int iva, int depth) {
  TrLinearElem* elem = GetValidElem(tkid,iva,depth);  
  if (!elem) {
    printf("TrLinearDB::GetLinearityCorrected-E no correction element found (tkid=%+04d,iva=%02d,depth=%1d). Return unmodified value\n",tkid,iva,depth);
    return ADC;
  }
  return elem->GetLinearityCorrected(ADC);
}


double TrLinearDB::ApplyNonLinearity(double ADC, int tkid, int iva, int depth) {
  TrLinearElem* elem = GetValidElem(tkid,iva,depth);
  if (!elem) {
    printf("TrLinearDB::ApplyNonLinearity-E no correction element found (tkid=%+04d,iva=%02d,depth=%1d). Return unmodified value\n",tkid,iva,depth);
    return ADC;
  }
  return elem->ApplyNonLinearity(ADC);
}


///////////////////////////
// TrLinearElem
///////////////////////////


TrLinearElem::TrLinearElem(int index, int succ, double chisq, double* pars) {
  fIndex = index;
  fSucc = succ;
  fChisq = chisq;
  for (int ipar=0; ipar<5; ipar++) fPars[ipar] = pars[ipar];
}


TrLinearElem::TrLinearElem(int index, int succ, float chisq, float* pars) {
  fIndex = index;
  fSucc = succ;
  fChisq = chisq;
  for (int ipar=0; ipar<5; ipar++) fPars[ipar] = pars[ipar];
}


void TrLinearElem::Info() {
  printf("TrLinearElem::Info-V:   Index: %5d   Succ: %2d   Chisq: %11.3f   Pars(",GetIndex(),GetSucc(),GetChisq());
  for (int ipar=0; ipar<5; ipar++) printf("%10.6f ",GetPar(ipar));
  printf(")\n");
}


bool TrLinearElem::IsValid() {
  return ( (GetChisq()<10)&&(GetSucc()>=0) );
}


double TrLinearElem::GetLinearityCorrected(double ADC) { 
  double tmp = (ADC>0) ? sqrt(ADC) : 0;
  return pow(p_strip_behavior(&tmp,fPars),2); 
}


double TrLinearElem::ApplyNonLinearity(double ADC) {
  double tmp = (ADC>0) ? sqrt(ADC) : 0;
  double xmin = 0;
  double xmax = 1000; // crazy high
  // check range
  double eval = p_strip_behavior(&xmax,fPars);     
  if (tmp>eval) tmp = eval; // if too crazy give a flat responce  
  // find bin 
  for (int iter=0; iter<10; iter++) {
    double xmin_step,xmax_step;
    GetInterval(tmp,xmin,xmax,10,xmin_step,xmax_step);
    xmin = xmin_step;
    xmax = xmax_step; 
  }
  // linear interpolation
  double x  = tmp; 
  double y1 = xmin;
  double y2 = xmax;      
  double x1 = p_strip_behavior(&y1,fPars);   
  double x2 = p_strip_behavior(&y2,fPars);
  double interp = y1 + (x-x1)*(y2-y1)/(x2-x1);      
  return pow(interp,2); 
}


void TrLinearElem::GetInterval(double y, double xmin, double xmax, int nsteps, double& xmin_step, double& xmax_step) {
  int istep;
  for (istep=0; istep<=nsteps; istep++) {
    double x = xmin+((xmax-xmin)/nsteps)*istep;
    double eval = p_strip_behavior(&x,fPars);
    if (eval>y) break;
  }
  xmin_step = xmin+((xmax-xmin)/nsteps)*(istep-1);
  xmax_step = xmin+((xmax-xmin)/nsteps)*(istep);
}


///////////////////////////
// Functions
///////////////////////////


double p_strip_behavior(double* x, double* par) {
  double result = par[2]/(1. + exp(-(x[0]-par[0])/par[1]));
  if (x[0]<=par[0]) result += par[3]*x[0];
  else              result += par[3]*par[0]+par[4]*x[0]-par[4]*par[0];
  return result/par[3];
}

