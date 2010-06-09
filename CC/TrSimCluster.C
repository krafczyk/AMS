#include "TrSimCluster.h"

TrSimCluster::TrSimCluster(vector<double> signal, int address, int seedind, double sigma) {
  _signal = signal; 
  _address = address;
  _seedind = seedind;
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
    _signal.at(i) *= signal; 
  }
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
  for (int i=add1; i<add2; i++) acluster.push_back(cluster->GetSignal(i-cluster->GetAddress()) + GetSignal(i-GetAddress()));
  // redefine the cluster
  _signal = acluster;
  _address = add1;
  _seedind = -1;
}

