#include "TrSimCluster.h"


extern "C" double rnormx();


TrSimCluster::TrSimCluster(vector<double> signal, int address, int seedind) {
  _signal = signal; 
  _address = address;
  _seedind = seedind;
}


TrSimCluster::TrSimCluster(const TrSimCluster& that) {
  _signal = that._signal;
  _address = that._address;
  _seedind = that._seedind;
}


TrSimCluster& TrSimCluster::operator=(const TrSimCluster& that) {
   if (this!=&that) {
     _signal = that._signal;
     _address = that._address;
     _seedind = that._seedind;
   }
   return *this;
}    


void TrSimCluster::Clear() {
  _signal.clear();
  _address = 0;
  _seedind = 0;
}


void TrSimCluster::SetSignal(int i, double s) {
  // no error: no effect if out of the cluster
   if ( (i<0)||(i>=GetWidth()) ) return;
  _signal[i] = s;
}


double TrSimCluster::GetSignal(int i) {
  // no error: 0 if out of the cluster
  if ( (i<0)||(i>=GetWidth()) ) return 0.;
  return _signal[i];//.at(i);
}


void TrSimCluster::Info(int verbose) {
  if (GetWidth()==0) { printf("TrSimCluster - Empty!\n"); return; }
  printf("TrSimCluster - nStrips = %2d   SeedIndex = %2d   Address = %3d  TotSignal = %7.3f\n",
         GetWidth(),GetSeedIndex(),GetAddress(),GetTotSignal());
  if (verbose>0) {
    for (int i=0; i<GetWidth(); i++) {
      printf("Address(p,n) %4d %4d   Signal %11.9f\n",i+GetAddress(),i+GetAddress()+640,GetSignal(i));
    }
  }
  if (verbose>1) { 
    printf("Eta %7.3f   CofG(2,3) %7.3f %7.3f\n",GetEta(),GetCofG(2),GetCofG(3));
  }
}


int TrSimCluster::FindSeedIndex(double seed, int force) {
  if ( (force==0)&&(GetSeedIndex()>=0) ) return GetSeedIndex(); 
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
  double left   = ((cindex-1)>=0)?GetSignal(cindex-1):0.;
  double center = GetSignal(cindex);
  double right  = ((cindex+1)<GetWidth())?GetSignal(cindex+1):0;
  if      (cindex==0)               eta = right/(center+right); // nleft = 0
  else if ((cindex-GetWidth())==0)  eta = center/(left+center); // nright = 0
  else if (right>left)              eta = right/(center+right);
  else                              eta = center/(left+center);
  return eta;
}


double TrSimCluster::GetCofG(int nstrips) {
  if (nstrips<=1) return 0.;
  int index = FindSeedIndex();
  if (index<0) return 0.;
  // scan of the same number of strips to the right and to the left (seed is 0)
  double numerator = 0.;
  double denominator = GetSignal(index);
  int n = (nstrips-1)/2;
  for (int i=0; i<n; i++) {
    numerator += (i+1)*GetSignal(index+i+1) - (i+1)*GetSignal(index-i-1);
    denominator += GetSignal(index+i+1) + GetSignal(index-i-1);
  }
  // if nstrip is even I have to add one more strip
  if (nstrips%2==0) {
    if (GetSignal(index+n+1)>GetSignal(index-n-1)) {
      numerator += (n+1)*GetSignal(index+n+1);
      denominator += GetSignal(index+n+1);
    }
    else {
      numerator += - (n+1)*GetSignal(index-n-1);
      denominator += GetSignal(index-n-1);
    } 
  }
  if (fabs(denominator)<1e-10) return 0.; // error!
  return numerator/denominator;
}


double TrSimCluster::GetX(int iside, int tkid, int nstrips, int imult) {
  double X = 0.;
  if (iside==0) X = TkCoo::GetLocalCoo(tkid,float(640+GetAddressCycl(FindSeedIndex())+GetCofG(nstrips)),imult);
  else          X = TkCoo::GetLocalCoo(tkid,float(GetAddress(FindSeedIndex())+GetCofG(nstrips)),imult);
  return X;
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
  if (this == &cluster) { // check auto-add
    printf("TrSimCluster::AddCluster-E auto-adding, this could be an error... please check.\n");
    return;
  }
  // an error message 
  if (cluster.GetAddress()<0) {
    if (WARNING) { printf("TrSimCluster::AddCluster-W the cluster to be added has address < 0:\n"); cluster.Info(1); }
    return;
  }
  // if "that" cluster is empty don't take any action
  if (cluster.GetWidth()==0) {
    return;
  } 
  // if the "this" cluster is empty make a copy of the passed cluster 
  if (GetWidth()==0) {
    _signal = cluster._signal;
    _address = cluster._address;
    _seedind = cluster._seedind;
    return;
  }

  // first and last address
  int add1 = min(cluster.GetAddress(),GetAddress());
  int add2 = max(cluster.GetAddress()+cluster.GetWidth(),GetAddress()+GetWidth());
  vector<double> acluster;
  acluster.clear();
  // fill
  for (int i=add1; i<add2; i++){
    float s0=0;
    if(i>=GetAddress()&& i<(GetAddress()+GetWidth())) 
      s0= GetSignal(i-GetAddress());
    float s1=0;
    if(i>=cluster.GetAddress()&& i<(cluster.GetAddress()+cluster.GetWidth())) 
      s1= cluster.GetSignal(i-cluster.GetAddress());
    acluster.push_back(s0+s1);
  }
  // redefine the cluster
  _signal = acluster;
  _address = add1;
  _seedind = FindSeedIndex(0,1);
  return;
}


void TrSimCluster::GaussianizeFraction(int iside, int hcharge, double fraction,float IP) {

  float ff[2][2][3]={
    {{1.,  1., 8.},{1.4, 8., 14.}},
    {{1.,  1., 8.},{1.4, 6.,  6.}}
  };

  //experimental version PZ
  if (fraction ==0) return;
  int iseed=FindSeedIndex(0,1);
  double totsig=GetTotSignal();
  int isl=iseed-1;
  int isr=iseed+1;
  float sr=0,sl=0;
  if(isl>=0) sl=GetSignal(isl);
  if(isr<GetWidth()) sr=GetSignal(isr);

  int LL=iseed;
  int RR=isr;
  if(sl>=sr) {
    LL=isl;
    RR=iseed;
  }

 

  float fra1=ff[hcharge][iside][0];
  float fra2=ff[hcharge][iside][1];
  float fra3=ff[hcharge][iside][2];

  if((LL-1)>=0)
    SetSignal(LL-1,GetSignal(LL-1)* fra1);
  
  if((LL-2)>=0)
    SetSignal(LL-2,GetSignal(LL-2)*fra2*fra2);

   
  
  if(RR+1<GetWidth())
    SetSignal(RR+1,GetSignal(RR+1)* fra1);
  
  if(RR+2<GetWidth())
    SetSignal(RR+2,GetSignal(RR+2)* fra2*fra2);
  
  
  int kk=3;
  if((LL-3)>=0)
    for (int ii=LL-3;ii>=0;ii--){
      SetSignal(ii,GetSignal(ii)* pow(fra3,kk++));
    }
  
  kk=3;
  if((RR+3)<GetWidth())
    for (int ii=RR+3;ii<GetWidth();ii++){
      SetSignal(ii,GetSignal(ii)* pow(fra3,kk++));
      
    }
  
  return;  
    
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


void TrSimCluster::ApplyGain(int iside, int tkid) {
  for (int ist=0; ist<GetWidth(); ist++) {
    int   address = (iside==0) ? GetAddressCycl(ist) + 640 : GetAddress(ist);
    int   iva = int(address/64);
    if ( (iva<0)||(iva>15) ) { 
       printf("TrSimCluster::ApplyGain-E wrong VA (va=%2d, tkid=%+4d, addr=%4d), skipping.\n",iva,tkid,address);
       return; 
    }
    // for the moment I leave the old code
    TrLadPar* ladpar = TrParDB::Head->FindPar_TkId(tkid);
    float gain = ladpar->GetGain(iside)*ladpar->GetVAGain(iva);
    if      ( gain<0.02 )     SetSignal(ist,0.);                 // VA with no gain!
    else if ( (1./gain)<0.5 ) SetSignal(ist,GetSignal(ist)/10.); // VA with bad gain!
    else                      SetSignal(ist,GetSignal(ist)/gain);
    /* decide how to use the tag ... FIX-ME
    TrGainDB::GetHead()->FindGainTkId(tkid)
    if (TrGainDB::GetHead()->FindGainTkId(tkid)->IsBad(iva))
    SetSignal(ist,TrGainDB::GetHead()->GetGainCorrected(GetSignal(ist),tkid,iva));
    */
  }
}


void TrSimCluster::ApplyAsymmetry(int iside) {
  float asym[2]={0.04,0.01};
  if(iside==0)
    for (int ist=GetWidth()-1; ist>0; ist--) { // first channel is excluded
    SetSignal(ist,GetSignal(ist)+GetSignal(ist-1)*asym[iside]);
	//        SetSignal(ist,GetSignal(ist)+GetSignal(ist-1)*TrClusterR::GetAsymmetry(iside));
    //   SetSignal(ist,GetSignal(ist)+(GetSignal(ist-1)+500)*2.83E-4);
  } 

  if(iside==1)
//     for (int ist=1; ist<GetWidth(); ist++) { // first channel is excluded
//     SetSignal(ist,GetSignal(ist)+GetSignal(ist-1)*asym[iside]);

    for (int ist=GetWidth()-1; ist>=0; ist--) { // first channel is excluded
      SetSignal(ist,GetSignal(ist)+GetSignal(ist-1)*asym[iside]);
	//        SetSignal(ist,GetSignal(ist)+GetSignal(ist-1)*TrClusterR::GetAsymmetry(iside));
    //   SetSignal(ist,GetSignal(ist)+(GetSignal(ist-1)+500)*2.83E-4);
  } 
}


void TrSimCluster::ApplyStripNonLinearity() {         
  printf("TrSimCluster::ApplyStripNonLinearity-E p-strip non-linearity not yet implemented.\n"); 
  return; 
}

