#include "TrSimCluster.h"


extern "C" double rnormx();


TrSimCluster::TrSimCluster(vector<double> signal, int address, int seedind) {
  _signal = signal; 
  _address = address;
  _seedind = seedind;
}


TrSimCluster::TrSimCluster(const TrSimCluster& orig) {
  _signal = orig._signal;
  _address = orig._address;
  _seedind = orig._seedind;
}


void TrSimCluster::Clear() {
  _signal.clear();
  _address = 0;
  _seedind = 0;
}


void TrSimCluster::SetSignal(int i, double s) {
  // no error: no effect if out of the cluster
  if ( (i<0)||(i>=GetWidth()) ) return;
  _signal.at(i) = s;
}


double TrSimCluster::GetSignal(int i) {
  // no error: 0 if out of the cluster
  if ( (i<0)||(i>=GetWidth()) ) return 0.;
  return _signal.at(i);
}


void TrSimCluster::Info(int verbose) {
  if (GetWidth()==0) { printf("TrSimCluster - Empty!\n"); return; }
  printf("TrSimCluster - nStrips = %2d   SeedIndex = %2d   Address = %3d  TotSignal = %7.3f\n",
         GetWidth(),GetSeedIndex(),GetAddress(),GetTotSignal());
  if (verbose>0) {
    for (int i=0; i<GetWidth(); i++)
      printf("Address (p,n) %4d %4d  Signal %7.5f\n",i+GetAddress(),i+GetAddress()+640,GetSignal(i));
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


double TrSimCluster::GetTotSignal() {
  double signaltot = 0.;
  for (int i=0; i<GetWidth(); i++) {
    signaltot += GetSignal(i);
  }
  return signaltot;
}


void TrSimCluster::Multiply(double signal) {
  for (int i=0; i<GetWidth(); i++) {
    SetSignal(i,GetSignal(i)*signal);
  } 
}


void TrSimCluster::AddCluster(TrSimCluster& cluster) {  
  // an error message 
  if (cluster.GetAddress()<0) {
    if (WARNING) { printf("TrSimCluster::AddCluster-E the cluster to be added has address < 0:\n"); cluster.Info(1); }
    return;
  }
  // if the "this" cluster is empty make a copy of the passed cluster 
  if (GetWidth()==0) {
    _signal = cluster._signal;
    _address = cluster._address;
    _seedind = cluster._seedind;
    return;
  }
  // if "that" cluster is empty don't take any action
  if (cluster.GetWidth()==0) {
    return;
  } 
  // first and last address
  int add1 = min(cluster.GetAddress(),GetAddress());
  int add2 = max(cluster.GetAddress()+cluster.GetWidth(),GetAddress()+GetWidth());
  vector<double> acluster;
  acluster.clear();
  // fill
  for (int i=add1; i<add2; i++) acluster.push_back(cluster.GetSignal(i-cluster.GetAddress()) + GetSignal(i-GetAddress()));
  // redefine the cluster
  _signal = acluster;
  _address = add1;
  _seedind = -1;
  return;
}


void TrSimCluster::GaussianizeFraction(double fraction) {
  for (int i=0; i<GetWidth(); i++) {
    SetSignal(i,GetSignal(i)*(1. + rnormx()*fraction));
  }
}


void TrSimCluster::AddNoise(double noise) {
  for (int i=0; i<GetWidth(); i++) {
    SetSignal(i,GetSignal(i) + noise*rnormx());
  }
} 


void TrSimCluster::ApplySaturation(double maxvalue) {
  for (int i=0; i<GetWidth(); i++) {
    if (GetSignal(i)>=maxvalue) SetSignal(i,maxvalue); 
  }
}


void TrSimCluster::ApplyGain(int side, int tkid) {
  TrLadPar* ladpar = TrParDB::Head->FindPar_TkId(tkid);
  for (int ist=0; ist<GetWidth(); ist++) {
    int   address = GetAddress(ist) + 640*(1-side);
    int   iva     = int(address/64);
    float gain    = ladpar->GetGain(side)*ladpar->GetVAGain(iva);
    if (ladpar->GetVAGain(iva)<0.02) SetSignal(ist,0.); // VA with no gain!
    else                             SetSignal(ist,GetSignal(ist)/gain);
  }
} 
