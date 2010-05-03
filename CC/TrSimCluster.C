#include "TrSimCluster.h"

// [iside][iZ][ipar]
double TrSimCluster::chargepars[2][2][6] = {
  { {  3.31,    40.50,   640876.2,     6.50,   105.78,   617989.6},
    { 10.21,   167.92,  1994017.3,    10.67,   236.02,  1798923.5} },
  { {  3.31,    31.73,   406306.2,     5.00,    43.71,   383758.3},
    {  7.20,   121.90,  1281691.5,     8.37,   158.42,  1113493.7} }
};

TrSimCluster::TrSimCluster(vector<double> signal, int address, int seedind, double sigma) {
  _signal = signal; 
  _address = address;
  _seedind = seedind;
  _sigma = sigma;
}


void TrSimCluster::Clear() {
  _signal.clear();
  _address = 0;
  _seedind = 0;
  _sigma = 0;
}

double TrSimCluster::GetSignal(int i) {
  if ( (i<0)||(i>=GetWidth()) ) return 0.;
  return _signal.at(i);
}

double TrSimCluster::GetSignalAtAddress(int i) {
  // no error!!! 
  // 0 if out of the cluster
  int index = i - GetAddress();
  if ( (index<0)||(index>=GetWidth()) ) return 0.;
  return GetSignal(index);
}

void TrSimCluster::Info(int verbose) {
  printf("TrSimCluster - nStrips = %2d   SeedIndex = %2d   Address = %3d   Eta = %7.5f  TotSignal = %7.3f\n",
         GetWidth(),GetSeedIndex(),GetAddress(),GetEta(),GetTotSignal());
  if (verbose>0) {
    for (int i=0; i<GetWidth(); i++)
      printf("Bin %4d   Signal %7.5f\n",i+GetAddress(),GetSignal(i));
  }
}

int TrSimCluster::FindSeedIndex(double seed) {
  double signalmax = seed;
  int    seedind = -1;
  for (int i=0; i<GetWidth(); i++) {
    if (GetSignal(i)>signalmax) {
      signalmax = GetSignal(i);
      seedind = i;
    }
  }
  return seedind;
}

double TrSimCluster::GetEta(){
  double eta    = 1.;
  int    cindex = FindSeedIndex();
  double left   = GetSignal(cindex-1);
  double center = GetSignal(cindex);
  double right  = GetSignal(cindex+1);
  if      (cindex==0)               eta = right/(center+right); // nleft = 0
  else if ((cindex-GetWidth())==0)  eta = center/(left+center); // nright = 0
  else if (right>left)              eta = right/(center+right);
  else                              eta = center/(left+center);
  return eta;
}

double TrSimCluster::GetCofG3() {
  int cindex = FindSeedIndex();
  if ( (GetWidth()<3) || ((cindex-1)<0) || ((cindex+1)>=GetWidth()) ) {
    double eta = GetEta();
    if (eta>0.5) eta -= 1; 
    return eta;
  }
  double left   = GetSignal(cindex-1);
  double center = GetSignal(cindex);
  double right  = GetSignal(cindex+1);
  return (-1*left + 0*center + 1*right)/(left+center+right); 
}


double TrSimCluster::GetTotSignal() {
  double signaltot = 0.;
  for (int i=0; i<GetWidth(); i++) {
    signaltot += GetSignal(i);
  }
  return signaltot;
}

void TrSimCluster::Multiply(double signal) {
  for (int i=0; i<GetWidth(); i++) {
    _signal.at(i) *= signal; 
  }
}

void TrSimCluster::DSPClusterize(double seed, double neig) {
  int cstrip = FindSeedIndex(seed);
  if (cstrip<0) {
    printf("TrSimCluster::DSPClusterize-Error no seed found!\n");
    Info(10);
    return;
  }
  // vector
  vector<double> acluster;
  acluster.clear();
  acluster.push_back(GetSignal(cstrip));
  // left
  int nleft = 0;
  for (int ii=cstrip-1; ii>=0; ii--) {
    if (GetSN(ii)<neig) break;
    nleft++;
    acluster.insert(acluster.begin(),GetSignal(ii));
  }
  // right
  int nright = 0;
  for (int ii=cstrip+1; ii<GetWidth(); ii++) {
    if (GetSN(ii)<neig) break;
    nright++;
    acluster.push_back(GetSignal(ii));
  }
  // set
  _signal = acluster;
  _address = _address + cstrip - nleft;
  _seedind = nleft;
}

void TrSimCluster::AddCluster(TrSimCluster* cluster){
  if (cluster->GetAddress()<0) {
    printf("TrSimCluster::AddCluster-Error the cluster to be added has address < 0, Info:\n");
    cluster->Info(1);
    return;
  }
  // first and last address
  int add1 = min(cluster->GetAddress(),GetAddress());
  int add2 = max(cluster->GetAddress()+cluster->GetWidth(),GetAddress()+GetWidth());
  vector<double> acluster;
  acluster.clear();
  // fill
  for (int i=add1; i<add2; i++) acluster.push_back(cluster->GetSignalAtAddress(i) + GetSignalAtAddress(i));
  // redefine the cluster
  _signal = acluster;
  _address = add1;
  _seedind = -1;
}


// da scrivere meglio (con metodo di Newton ...)
double TrSimCluster::LanGauExpIntInv(double integral, double *par) {
  double xmin = max(   0., par[1] - 100); // 10*par[3]); // max(   0., par[1] - 4*par[3]);
  double xmax = min(1000., par[1] + 200); // 30*par[3]); // min(1000., par[1] + 6*par[3]);
  int    nbin = 300;
  double step = (xmax-xmin)/nbin; 
  double sum  = 0.;
  double x[1] = {xmin};
  for (int ii=0; ii<nbin; ii++) {
    x[0] = xmin + (ii + 0.5)*step;
    sum += TrSimCluster::LanGauExpPdf(x,par)*step; 
    // cout << x[0] << " " << TrSimCluster::LanGauExpPdf(x,par) << " " << sum << endl;
    if (sum>=integral) return x[0];
  }
  return 0.;
}

double TrSimCluster::LanGauExpPdf(double *x, double *par) {
  return TrSimCluster::LanGauExpFun(x,par)/par[5];
}


double TrSimCluster::LanGauExpFun(double *x, double *par) {
  // par[0] = Width (scale) parameter of Landau density
  // par[1] = Most Probable (MP, location) parameter of Landau density
  // par[2] = Total area (integral -inf to inf, normalization constant)
  // par[3] = Width (sigma) of convoluted Gaussian function
  // par[4] = Separation point between langaus and exp (connected by derivative and continuity)
  //
  // In the Landau distribution (represented by the CERNLIB approximation),
  // the maximum is located at x=-0.22278298 with the location parameter=0.
  // This shift is corrected within this function, so that the actual
  // maximum is identical to the MP parameter.
  //
  // The additional exponential function is connected imposing 2 condition
  // - continuity: LanGauss(sep) = k * exp(-xsep/slope)
  // - derivative continuity: LanGauss'(xsep) = -k/slope * exp(-xsep/slope)
  // slope = - LanGauss(xsep) / LanGauss'(xsep)
  // k     =   LanGauss(xsep) * exp(xsep/slope)
  if (x[0]<=par[4]){ //LanGauss
    // Numeric constants
    Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
    Double_t mpshift  = -0.22278298;       // Landau maximum location
    // Control constants
    Double_t np = 100.0;      // number of convolution steps
    Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas
    // Variables
    Double_t xx;
    Double_t mpc;
    Double_t fland;
    Double_t sum = 0.0;
    Double_t xlow,xupp;
    Double_t step;
    Double_t i;
    // MP shift correction
    mpc = par[1] - mpshift * par[0];
    // Range of convolution integral
    xlow = x[0] - sc * par[3];
    xupp = x[0] + sc * par[3];
    step = (xupp-xlow) / np;
    // Convolution integral of Landau and Gaussian by sum
    for(i=1.0; i<=np/2; i++) {
      xx = xlow + (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);

      xx = xupp - (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);
    }
    return (par[2] * step * sum * invsq2pi / par[3]);
  }
  else { // Exponential tail
    // Control constants
    Double_t dx = 10.;     // infinitesimal interval for derivative calculation
                           // must be longer than the bin width (how implement this check?)
    // Variables
    Double_t xsep;
    Double_t xpre;
    Double_t fxsep;
    Double_t fxpre;
    Double_t deriv;
    Double_t slope;
    Double_t k;
    // Derivative calculation
    xsep  = par[4];
    xpre  = par[4] - dx;
    fxsep = TrSimCluster::LanGauExpFun(&xsep,par);
    fxpre = TrSimCluster::LanGauExpFun(&xpre,par);
    deriv = (fxsep-fxpre)/dx;
    // check of existence
    if (deriv==0.) return -1e+06;
    slope = -fxsep/deriv;
    if (slope==0.) return -1e+06;
    if ((xsep/slope)>709.) return -1e+06;
    k     = fxsep * exp(xsep/slope);
    return k * exp(-x[0]/slope);
  }
}

