// $Id$


#include "TrEDepDB.h"
#include "TrChargeLossDB.h" // for cubic monotonic spline


#define EDEP_ZMAX 26
#define BETA_MAX 0.999
#define LOGBETAGAMMA_MIN -3


ClassImp(TrEDepDB);
ClassImp(TrEDepTable);


///////////////////////////
// TrEDepDB
///////////////////////////


TrEDepDB* TrEDepDB::fHead = 0;
map<int,TrEDepTable*> TrEDepDB::fTrEDepMap;
bool TrEDepDB::fInitDone = false;
int TrEDepDB::fBoostBetaCorrection = -1;


TrEDepDB* TrEDepDB::GetHead() {
  if (IsNull()) {
    printf("TrEDepDB::GetHead()-V TrEDepDB singleton initialization.\n");
    fHead = new TrEDepDB(); 
  } 
  return fHead;
}


TrEDepDB::~TrEDepDB() { 
  Clear();
  if (fHead) delete fHead;
  fHead = 0;
}


void TrEDepDB::Init(bool force) {
  if (IsNull()) return;
  if ( (fInitDone)&&(!force) ) return;  
  // correction table
  if (LoadDefaultTablesVer1()) printf("TrEDepDB::GetHead()-V TrEDepDBver1 correction loaded, %d tables read.\n",(int)fTrEDepMap.size());
  else                         printf("TrEDepDB::GetHead()-V TrEDepDBver1 correction NOT loaded.\n"); 
  fInitDone = true;
} 


void TrEDepDB::Clear(Option_t* option) { 
  // delete all tables
  for (map<int,TrEDepTable*>::iterator it = fTrEDepMap.begin(); it !=  fTrEDepMap.end(); it++) {
    if (it->second) {
      it->second->Clear();
      delete it->second;
    }
    it->second = 0;
  }
  fTrEDepMap.clear();
  fInitDone = false;
}


void TrEDepDB::Info() {
  for (map<int,TrEDepTable*>::iterator it = fTrEDepMap.begin(); it !=  fTrEDepMap.end(); it++) {
    printf("TrEDepDB::Info-V table with index %3d info:\n",it->first);
    it->second->Info();
  }
}


bool TrEDepDB::AddTable(int index, TrEDepTable* table) {
  if (table==0) return false;
  pair<map<int,TrEDepTable*>::iterator,bool> ret;
  // insert
  ret = fTrEDepMap.insert(pair<int,TrEDepTable*>(index,table));
  // if the index already exists renew the table
  if (ret.second==false) fTrEDepMap.find(index)->second = table; 
  return true;
}


TrEDepTable* TrEDepDB::GetTable(int index) {
  map<int,TrEDepTable*>::iterator it = fTrEDepMap.find(index);
  return (it!=fTrEDepMap.end()) ? it->second : 0;
}


bool TrEDepDB::LoadDefaultTablesVer1(char* dirname) {
  for (int il=0; il<9; il++) {
    // X-Side tables 
    TrEDepTable* tableX = new TrEDepTable();
    bool loadX = tableX->InitTableFromTxtFile(Form("%s/v5.00/TrEDepDBver1/TrEDepTable_JLayer%1d_XSide.txt",dirname,il+1));
    if (loadX) AddTable(tableX,0,il+1,1);
    // Y-Side tables
    TrEDepTable* tableY = new TrEDepTable();
    bool loadY = tableY->InitTableFromTxtFile(Form("%s/v5.00/TrEDepDBver1/TrEDepTable_JLayer%1d_YSide.txt",dirname,il+1));
    if (loadY) AddTable(tableY,1,il+1,1);
  }
  return (int(fTrEDepMap.size())>0);
}


double TrEDepDB::GetEDepCorrectedValue(int jlayer, double Q, double beta, double rigidity, double mass_on_Z, int iside, int ver) {
  // type
  TrEDepTable* table = GetTable(iside,jlayer,ver);
  static int maxerr = 0;
  if (!table) {
    if (maxerr<100) { 
      printf("TrEDepDB::GetEDepCorrectedValue-E no table (iside=%d, jlayer=%d, ver=%d) found, return unchanged value (only displayed 100 times).\n",iside,jlayer,ver);
      maxerr++;
    }
    return Q; 
  }
  // boost (used for MC)
  double boost = ( (fBoostBetaCorrection>0)&&(beta<1) ) ? pow(fabs(beta),-TMath::Max(0.,TMath::Min(0.1,(fBoostBetaCorrection-3)*0.02))) : 1; 
  // use beta correction 
  int type = 0;
  // if rigidity not null use beta/rigidity correction
  if (fabs(rigidity)>1e-6) type = 1;
  // calculation delivered by table
  if (beta>=0) return table->GetCorrectedValue(Q,beta,rigidity,mass_on_Z,type)*boost;
  // if up-going make the jlayer folding plus some gain correction between thee two layers
  int jlayer_upgoing = 9-jlayer+1;
  TrEDepTable* table_downgoing = table;
  TrEDepTable* table_upgoing = GetTable(iside,jlayer_upgoing,ver);
  if ( (!table_downgoing)||(!table_upgoing) ) {
    printf("TrEDepDB::GetEDepCorrectedValue-E downgoing or upgoing table not available (iside=%d, jlayer=%d, ver=%d) found. Return 0.\n",iside,jlayer,ver);
    return 0;
  }
  double value = table_upgoing->GetCorrectedValue(Q,beta,rigidity,mass_on_Z,type);
  double den = table_upgoing->GetCorrectedValue(Q,0.94,0,0,0);
  double num = table_downgoing->GetCorrectedValue(Q,0.94,0,0,0);
  double gain = (fabs(den)<1e-6) ? 1 : num/den; 
  // some stretching function to be implemented ... upside/down is not ok in scale ...      
  return value*gain*boost; 
}


double TrEDepDB::GetTOIBeta(double beta, int Z, int inn_jlayer, int iside, int ver) {   
  TrEDepTable* table_top = GetTable(iside,1,ver); 
  TrEDepTable* table_inn = GetTable(iside,inn_jlayer,ver); 
  double Q = table_top->GetQ(Z,beta,0,0,0);
  // gain normalization
  Q *= table_inn->GetQ(Z,1,0,0,0)/table_top->GetQ(Z,1,0,0,0); 
  double beta_top = table_inn->FindBeta(Z,Q,0,0.1,0.99);
  return (beta_top>beta) ? beta_top : beta;
}


///////////////////////////
// TrEDepTable
///////////////////////////


TrEDepTable::TrEDepTable() {
  fIndex = 0;
  fNCharges = 0;
  fNPars = 0;
  fZ = 0;
  fPars = 0;
}


TrEDepTable::TrEDepTable(int index, int nch, int npar) {
  fIndex = 0;
  fNCharges = 0;
  fNPars = 0;
  fZ = 0; 
  fPars = 0;
  Init(index,nch,npar);
}


void TrEDepTable::Init(int index, int nch, int npar) { 
  Clear();
  fIndex = index;
  fNCharges = nch;
  fNPars = npar;
  if (!fZ) fZ = new int[GetNCharges()];
  if (!fPars) fPars = new double[GetNCharges()*GetNPars()];
  for (int ich=0; ich<GetNCharges(); ich++) {
    SetZ(ich,0);
    for (int ipar=0; ipar<GetNPars(); ipar++) {
      SetPar(ich,ipar,0.); 
    }
  }
}


void TrEDepTable::Clear(Option_t* option) {
  fIndex = 0;
  fNCharges = 0;
  fNPars = 0;
  if (fZ) delete [] fZ;
  fZ = 0; 
  if (fPars) delete [] fPars;  
  fPars = 0; 
}


void TrEDepTable::Info() {
  if (IsEmpty()) {
    printf("TrEDepTable::Info-V: Table empty!\n");
    return;
  }
  printf("TrEDepTable::Info-V: Index:%+4d   nCharges:%2d   nPars:%2d\n",GetIndex(),GetNCharges(),GetNPars());
  for (int ich=0; ich<GetNCharges(); ich++) {
    printf("  %2d  ",GetZ(ich));
    for (int ipar=0; ipar<GetNPars(); ipar++) {
      printf("%8.4f ",GetPar(ich,ipar)); 
    }
    printf("\n");
  }
}


int TrEDepTable::GetZ(int ich) {
  if (!fZ) return 0;
  if ( (ich<0)||(ich>=GetNCharges()) ) return 0;
  return fZ[ich];
}


double TrEDepTable::GetPar(int ich, int ipar) {
  if (!fPars) return 0;
  if ( (ich<0)||(ich>=GetNCharges()) ) return 0;
  if ( (ipar<0)||(ipar>=GetNPars()) ) return 0; 
  return fPars[ich*GetNPars()+ipar];
}


bool TrEDepTable::SetZ(int ich, int z) {
  if (!fZ) return false;
  if ( (ich<0)||(ich>=GetNCharges()) ) return false;
  fZ[ich] = z;
  return true;
}


bool TrEDepTable::SetPar(int ich, int ipar, double par) {
  if (!fPars) return false;
  if ( (ich<0)||(ich>=GetNCharges()) ) return false;
  if ( (ipar<0)||(ipar>=GetNPars()) ) return false;
  fPars[ich*GetNPars()+ipar] = par;
  return true;
}


bool TrEDepTable::InitTableFromTxtFile(char* filename) { 
  FILE* file = 0;
  file = fopen(filename,"r");
  if (!file) return false;
  // header  
  int index,ncharges,npars; 
  fscanf(file,"%d%d%d",&index,&ncharges,&npars);
  Init(index,ncharges,npars);
  for (int ich=0; ich<GetNCharges(); ich++) {
    int z;
    int ret = fscanf(file,"%d",&z);
    if (ret<=0) return false;
    SetZ(ich,z);
    for (int ipar=0; ipar<GetNPars(); ipar++) {
      float par;
      int ret = fscanf(file,"%f",&par);
      if (ret<=0) return false;
      SetPar(ich,ipar,par); 
    }
  }
  fclose(file);
  return true;
}


double TrEDepTable::GetCorrectedValue(double Q, double beta, double rigidity, double mass_on_Z, int type) {
  if (IsEmpty()) return 0;
  double pars[8] = {0};
  // starting point
  double x0 = Q; 
  // determine the relation 
  double x[100] = {0};
  double y[100] = {0};
  int    n = 0;
  // first point forced to 0,0
  x[0] = 0;
  y[0] = 0;
  n++;
  // previous
  double previous = 0;
  for (int ich=0; ich<GetNCharges(); ich++) {
    int Z = GetZ(ich);
    double monz  = (mass_on_Z<=1e-6) ? GetNaiveMassOnZ(Z) : mass_on_Z;
    double logbg = GetLogBetaGamma(beta,rigidity,monz,type);
    for (int ipar=0; ipar<8; ipar++) pars[ipar] = (type==0) ? GetPar(ich,ipar) : GetPar(ich,ipar+8);
    // monotonicity check 
    if (Z*sqrt(edep_correction_function(&logbg,pars))<previous) continue;
    // other checks    
    x[n] = Z*sqrt(edep_correction_function(&logbg,pars));
    y[n] = Z;
    previous = x[n];
    n++; 
  }
  // interpolate 
  // for (int i=0; i<n; i++) printf("%3.1f %3.1f | ",x[i],y[i]);
  // printf("\n");
  // interpolate 
  double y0 = monotonic_cubic_interpolation(x0,n,x,y);
  return y0;
}


double TrEDepTable::GetQ(int Z, double beta, double rigidity, double mass_on_Z, int type) {
  if (IsEmpty()) return 0;
  double pars[8] = {0};
  // starting point
  double x0 = Z;
  // determine the relation 
  double x[100] = {0};
  double y[100] = {0};
  int    n = 0;
  // first point forced to 0,0
  x[0] = 0;
  y[0] = 0;
  n++;
  // previous
  double previous = 0;
  for (int ich=0; ich<GetNCharges(); ich++) {
    int z = GetZ(ich);
    double monz = (mass_on_Z<=1e-6) ? GetNaiveMassOnZ(Z) : mass_on_Z;
    double logbg = GetLogBetaGamma(beta,rigidity,monz,type);
    for (int ipar=0; ipar<8; ipar++) pars[ipar] = (type==0) ? GetPar(ich,ipar) : GetPar(ich,ipar+8);
    // monotonicity check 
    if (z*sqrt(edep_correction_function(&logbg,pars))<previous) continue;
    // other checks    
    x[n] = z;
    y[n] = z*sqrt(edep_correction_function(&logbg,pars));
    previous = y[n];
    n++;
  }
  // for (int i=0; i<n; i++) printf("%3.1f %3.1f | ",x[i],y[i]);
  // printf("\n");
  // interpolate 
  double y0 = monotonic_cubic_interpolation(x0,n,x,y);
  return y0;
}


double TrEDepTable::FindBeta(int Z, double Q, int type, double beta_min, double beta_max) {
  double default_beta = 0;
  // start (first and last) 
  double xf = beta_min;
  double yf = GetQ(Z,xf,0,0,0); 
  double xl = beta_max;
  double yl = GetQ(Z,xl,0,0,0);  
  if (yl>yf) return default_beta; // inverse monotonic
  if ( (Q<yl)||(Q>yf) ) return default_beta; // outside
  int max_steps = 50;
  for (int istep=0; istep<max_steps; istep++) {
    // midpoint 
    double xm = xf+(xl-xf)/2;
    double ym = GetQ(Z,xm,0,0,0);
    // found 
    if (fabs(Q-ym)<1e-6) return xm;
    // check
    if (ym>yf) return default_beta; // inverse monotonic
    if (ym<yl) return default_beta; // inverse monotonic
    // new interval 
    if ( (Q<ym)&&(Q>=yl) ) { 
      xf = xm;
      yf = ym;
    } 
    else if ( (Q>=ym)&&(Q<yf) ) {
      xl = xm;
      yl = ym;
    }
    else {
      return default_beta;
    }
  }
  return default_beta; 
}


double TrEDepTable::GetNaiveMassOnZ(double Q) {
  double mass_on_Z_proton = 0.938; 
  double mass_on_Z_carbon = 0.932*2; 
  if      (Q<=1) return mass_on_Z_proton;
  else if (Q>=2) return mass_on_Z_carbon;
  else           return mass_on_Z_proton + (Q-1)*(mass_on_Z_carbon-mass_on_Z_proton);
}


double TrEDepTable::GetLogBetaGamma(double beta, double rigidity, double mass_on_Z, int type) {
  // log10(betagamma) from beta
  beta = fabs(beta);
  if (beta>BETA_MAX) beta = BETA_MAX;
  double gamma = 1./sqrt(1-beta*beta); 
  double logbg_beta = (beta*gamma>0) ? log10(beta*gamma) : LOGBETAGAMMA_MIN;
  if (type==0) return logbg_beta;
  // log10(betagamma) from rigidity 
  rigidity = fabs(rigidity);
  double logbg_rigi = (rigidity>0) ? log10(rigidity/mass_on_Z) : LOGBETAGAMMA_MIN;  
  // return log10(betagamma) from both 
  return ( (beta<0.95)&&(beta>0.01) ) ? logbg_beta : logbg_rigi;
}


/* Line-to-line parametrization 
          | M*x + Q                 x>x1
   f(x) = | sum_{i}^{N}  a_i x^i    x0<x<=x1
          | m*x + q                 x<=x0 
   - pars (M,m,Q,q,a_0,...,a_N,x0,x1) = 2 + 2 + (N+1) + 2 = N + 7
   - match in continuity and derivative continuity, 4 conditions -> N+3 pars.
   - pars let free (M,Q,m,q,a_4,...,a_N,x0,x1)  
   - par[0] is used as the degree of the polynomial 
*/
double line_to_line_fun(double *x, double *par) {
  // function degree 
  int degree = int(par[0]);
  // parameters
  double x0 = par[1];
  double m  = par[2];
  double q  = par[3];
  double x1 = par[4];
  double M  = par[5];
  double Q  = par[6];
  // external lines
  double xx = x[0];
  if (xx>=x1) return M*xx + Q;
  if (xx<=x0) return m*xx + q;
  // polynomial
  double a[100];
  for (int i=4; i<=degree; i++) a[i] = par[i+3];
  double S0 = 0;
  double S1 = 0;
  double s0 = 0;
  double s1 = 0; 
  for (int i=4; i<=degree; i++) { 
    S0 += pow(x0,i)*a[i];
    S1 += pow(x1,i)*a[i];
    s0 += i*pow(x0,i-1)*a[i];
    s1 += i*pow(x1,i-1)*a[i];
  }
  a[3] = ( (m-M)*(x1+x0)+2*(q-Q)+2*(S1-S0)-(s0+s1)*(x1-x0) )/pow(x1-x0,3.);
  a[2] = ( (M-m)-3*a[3]*(x1-x0)*(x1+x0)-(s1-s0) )/( 2*(x1-x0) );
  a[1] = ( (m*x1-M*x0)+3*a[3]*x0*x1*(x1-x0)-(s0*x1-s1*x0) )/(x1-x0);
  a[0] = (M*x1+Q)-S1-a[3]*pow(x1,3)-a[2]*pow(x1,2)-a[1]*x1;
  // calculate
  double value = 0;
  for (int i=0; i<=degree; i++) value += pow(xx,i)*a[i];
  return value;
}


double edep_correction_function(double *x, double *par) {
  double pars[] = {4,par[0],par[1],par[2],par[3],par[4],par[5],par[6]};
  double betagamma = pow(10.,x[0]);
  double beta = sqrt(1.0/(1.0/betagamma/betagamma + 1.0));
  return exp(par[7]*beta)*pow(10.,line_to_line_fun(x,pars));
}

