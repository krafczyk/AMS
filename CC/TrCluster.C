#include "TrCluster.h"


ClassImp(TrClusterR);


TrCalDB* TrClusterR::_trcaldb = NULL;
TrParDB* TrClusterR::_trpardb = NULL;


// asymmetry determined from Muons 2011 analysis
float TrClusterR::Asymmetry[2] = {0.040,0.005};

// the default corrections are used in reconstruction and are independent from calibration (overridden by datacard)
int   TrClusterR::DefaultCorrOpt       = TrClusterR::kAsym|TrClusterR::kAngle; 

// measurement corrections
int   TrClusterR::DefaultChargeCorrOpt = TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kMIP|TrClusterR::kAngle|TrClusterR::kBeta|TrClusterR::kRigidity;
int   TrClusterR::DefaultEdepCorrOpt   = TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kMIP|TrClusterR::kMeV;

int   TrClusterR::DefaultUsedStrips = -1;     // -1: inclination dependent
float TrClusterR::TwoStripThresholdX = 0.70;  // tan(35deg)
float TrClusterR::TwoStripThresholdY = 0.36;  // tan(20deg)


TrClusterR::TrClusterR(void) {
  Clear();
}


TrClusterR::TrClusterR(const TrClusterR &orig):TrElem(orig) {
  _tkid    = orig._tkid;
  _address = orig._address;
  _nelem   = orig._nelem;
  _seedind = orig._seedind;
  for (int i = 0; i<_nelem; i++) _signal.push_back(orig._signal.at(i));
  Status  = orig.Status;
  _mult    = orig._mult;
  _dxdz    = orig._dxdz;
  _dydz    = orig._dydz;
}


TrClusterR::TrClusterR(int tkid, int side, int add, int seedind, unsigned int status) {
  Clear();
  _tkid    = tkid;
  _address = add;
  _seedind = seedind;
  Status  = status;
}


TrClusterR::TrClusterR(int tkid, int side, int add, int nelem, int seedind, 
                           float* adc, unsigned int status)  {
  Clear();
  _tkid    =  tkid;
  _address =  add;
  if ( ((add+nelem-1)>=1024)&&(!TkDBc::Head->FindTkId(tkid)->IsK7()) ) {
    _nelem=1024-add;
    cerr<<"TrClusterR::TrClusterR-W You are tring to create a Cluster with address "<< add<<" and length "<<nelem<<endl;
    cerr<<"                         Cluster has been truncated to the physical limit"<<endl;
  }
  else
  _nelem   =  nelem;
  _seedind =  seedind;
  _signal.reserve(_nelem);
  for (int i = 0; i<_nelem; i++) _signal.push_back(adc[i]);
  Status  = status;
  _mult    = 0;
}


TrClusterR::~TrClusterR() {
  Clear();
}


int TrClusterR::GetMultiplicity()  {
  if (_mult==0) _mult = TkCoo::GetMaxMult(GetTkId(),GetAddress())+1; // maybe better to use GetSeedAddress();
  return _mult;
}


float TrClusterR::GetNoise(int ii) {
  if (_trcaldb==0) { printf("TrClusterR::GetNoise-E no _trcaldb specified.\n"); return -9999.; }
  int tkid = GetTkId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!ladcal) { printf ("TrClusterR::GetNoise-W calibration not found.\n"); return -9999; }
  int address = GetAddress(ii); 
  return (float) ladcal->GetSigma(address);
}


short TrClusterR::GetStatus(int ii) {
  if (_trcaldb==0) {
    printf("TrClusterRs::GetStatus Error, no _trcaldb specified.\n");
    return -1; 
  }
  int tkid = GetTkId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!ladcal) {printf ("TrClusterR::GetNoise, WARNING calibration not found!!\n"); return -9999;}
  int address = _address+ii;
  return (short) ladcal->GetStatus(address);
}


int TrClusterR::GetSensorAddress(int& sens, int ii, int mult, int verbose) {
  // it could happen that multiplicity exceeds by one the max (only in case of cluster on the last sensor of K7)
  int max_mult = TkCoo::GetMaxMult(GetTkId(),GetSeedAddress());
  if ( (IsK7())&&((mult-max_mult)==1) ) mult = max_mult; 
  // mult, by XCofG method convention, is referred to seed strip
  int seedadd = GetAddress() + GetSeedIndex(); 
  // here I convert multiplicity of seed to multiplicity of first strip
  if ( (GetSide()==0)&&(IsK7())&&(seedadd>1023) ) mult--; // if seed > left margin the mult of first strip is mult-1 
  if ( (GetSide()==0)&&(IsK7())&&(seedadd< 640) ) mult++; // if seed < right margin the mult of first strip is mult+1 (this case is possible only if ii<0 & K7)
  // now take the address without regarding of cyclicity accounted in the following algorithm
  int address = GetAddress() + ii; 
  // detect multiplicity jump in case of K7
  while ( (GetSide()==0)&&(IsK7())&&(address>1023) ) { address -= 384; mult++; }
  while ( (GetSide()==0)&&(IsK7())&&(address< 640) ) { address += 384; mult--; }
  if ( ( (GetSide()==0)&&( (address<640)||(address>1023) ) ) ||
       ( (GetSide()==1)&&( (address<  0)||(address> 639) ) ) ) {
    if (verbose>0) printf("TrClusterR::GetSensorAddress-W address out of bounds (%d) for side %1d. Return -5.\n",address,GetSide());
    return -5;
  }
  return TkCoo::GetSensorAddress(GetTkId(),address,mult,sens,verbose);
}


void TrClusterR::Clear() {
  _tkid    =   0;
  _address =  -1;
  _nelem   =   0;
  _seedind =   0;
  _signal.clear();
  Status   =   0;
  _mult    =   0;
  _dxdz    =   0;  // vertical inclination by default 
  _dydz    =   0;  // vertical inclination by default 
}


void TrClusterR::push_back(float adc) {
  _signal.push_back(adc);
  _nelem = (int) _signal.size();
}


float TrClusterR::GetGCoord(int imult)  {
  float lcoo = GetXCofG(DefaultUsedStrips,imult);
  if (GetSide() == 0) return TkCoo::GetGlobalA(_tkid, lcoo, TkDBc::Head->_ssize_active[1]/2).x();
  else                return TkCoo::GetGlobalA(_tkid, TkDBc::Head->_ssize_active[0]/2, lcoo).y();
}


void TrClusterR::Print(int opt) { 
  _PrepareOutput(opt);
  cout << sout;
}


void TrClusterR::_PrepareOutput(int opt){
  sout.clear();
  sout.append(Form("TkId: %5d  Side: %1d  Address: %4d  Nelem: %3d  Status: %3d  Signal(ADC): %10.3f  Edep(MeV): %8.3f  QStatus: %8hX\n",
    GetTkId(),GetSide(),GetAddress(),GetNelem(),getstatus(),GetTotSignal(0),GetEdep(),GetQStatus()));
  if(!opt) return;
  if (opt>1) {
    int strip = -1;
    int address = GetAddress(strip);
    if ( (GetSide()==0)&&(address>1023) ) address = -1;
    if ( (GetSide()==1)&&(address> 639) ) address = -1;
    if (address>0) 
      sout.append(Form("Address: %4d                      Sigma: %10.5f                   Status: %3d\n",
        address,GetSigma(strip),GetStatus(strip)));                      
  }
  for (int ii=0; ii<GetNelem(); ii++) {
    sout.append(Form("Address: %4d  Signal: %10.5f  Sigma: %10.5f  S/N: %10.5f  Status: %3d  ",
      GetAddress(ii),GetSignal(ii,DefaultCorrOpt),GetSigma(ii),GetSN(ii,TrClusterR::kAsym),GetStatus(ii)));
    if (ii==GetSeedIndex()) sout.append("<<< SEED\n");
    else sout.append(" \n");
  }
  if (opt>1) {
    int strip = GetNelem();
    int address = GetAddress(strip);
    if ( (GetSide()==0)&&(address>1023) ) address = -1;
    if ( (GetSide()==1)&&(address> 639) ) address = -1;
    if (address>0)
      sout.append(Form("Address: %4d                      Sigma: %10.5f                   Status: %3d\n",
        address,GetSigma(strip),GetStatus(strip))); 
  }
}


const char* TrClusterR::Info(int iRef){
  string aa;
  aa.append(Form("TrCluster #%d ",iRef));
  _PrepareOutput(0);
  aa.append(sout);
  int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}


std::ostream &TrClusterR::putout(std::ostream &ostr) {
  _PrepareOutput(1);
  return ostr << sout  << std::endl;
}


float TrClusterR::GetSignal(int ii, int opt) {
  double signal = _signal.at(ii);
  int tkid = GetTkId();
  int iva = int(GetAddress(ii)/64);
  // multiplexing effect
  if ( (kAsym&opt)&&(ii>0) ) signal = signal - _signal.at(ii-1)*GetAsymmetry(GetSide());
  // gain
  if (kGain&opt) signal = TrGainDB::GetHead()->GetGainCorrected(signal,tkid,iva);
  // p-side linearization
  if ( (kPStrip&opt)&&(GetSide()==1) ) signal = TrLinearDB::GetHead()->GetLinearityCorrected(signal,tkid,iva);
  return signal;
}


int TrClusterR::GetAddress(int ii) {
  int address = GetAddress() + ii;
  int side    = GetSide();
  // cyclical definition, in order to avoid errors and manage the K7 clusters
  if ( (side==0)&&(address>1023) ) address = address - 384; //  640 + (address - 1024);
  if ( (side==0)&&(address< 640) ) address = address + 384; // 1024 - (640  - address);
  if ( (side==1)&&(address> 639) ) address = address - 640; //    0 + (address -  640);
  if ( (side==1)&&(address<   0) ) address = address + 640; //  640 - (   0 - address);
  return address;
}


int TrClusterR::GetSeedIndex(int opt) {
  if (opt==kNoCorr) return _seedind;  
  float maxadc  = -9999.;
  int   seedind = -1;
  for (int ii=0; ii<GetNelem(); ii++) { 
    if (GetSignal(ii,opt)>maxadc) { 
      maxadc  = GetSignal(ii,opt); 
      seedind = ii; 
    } 
  }
  return seedind;
}


float TrClusterR::GetTotSignal(int opt, float beta, float rigidity, float mass_on_Z, int res_mult) {
  // total signal with signal corrections
  float sum = 0.;
  for (int ii=0; ii<GetNelem(); ii++) {
    float signal = GetSignal(ii,opt);
    if (signal>0) sum += signal;
  }
  //// maybe a better solution should be (tried, not changed much):
  //// for (int ii=0; ii<GetNelem(); ii++) { float signal = GetSignal(ii,TrClusterR::kAsym); if (signal>0) sum += signal; } 
  //// if (kGain&opt) sum = TrGainDB::GetHead()->GetGainCorrected(sum,GetTkId(),int(GetSeedAddress()/64));
  // old correction scheme
  if (kOld&opt) {
    // if (kLoss&opt) sum *= GetTrParDB()->GetChargeLoss(GetSide(),GetCofG(DefaultUsedStrips,opt),GetImpactAngle()); // correction no longer in use
    if (kLoss&opt) sum = TrChargeLossDB::GetHead()->GetChargeLossCorrectedValue(GetSide(),GetCofG(),GetImpactAngle(),sum*GetCosTheta())/GetCosTheta();
    // if (kPN&opt) sum = ConvertToPSideScale(sum); // correction no longer in use
    if (kMIP&opt) sum = GetNumberOfMIPs(sum);
    if (kAngle&opt) sum *= GetCosTheta();
    float betagamma_corr = 1;
    if ((kBeta&opt)&&(kRigidity&opt)) betagamma_corr = BetaRigidityCorrection(beta,rigidity,mass_on_Z);
    else if (kBeta&opt) betagamma_corr = BetaCorrection(beta);
    else if (kRigidity&opt) betagamma_corr = RigidityCorrection(rigidity,mass_on_Z);
    if (betagamma_corr<=0.) betagamma_corr = 1;
    sum /= betagamma_corr;
    if (kMeV&opt) sum = sum*0.081; // 81 keV per MIP 
  }
  // new correction scheme
  else {
    // correct first for angle
    sum *= GetCosTheta();   
    // charge loss correction
    if (kLoss&opt) {
      int interstrip = (res_mult>=0) ? GetNInterstrip(res_mult) : -1; 
      sum = TrChargeLossDB::GetHead()->GetChargeLossCorrectedValue(GetSide(),interstrip,GetCofG(),GetImpactAngle(),sum,1);
    } 
    // beta/rigidity correction
    if ( (kBeta&opt)&&(kRigidity&opt)&&(fabs(rigidity)>1e-6) ) 
      sum = pow(TrEDepDB::GetHead()->GetEDepCorrectedValue(GetLayerJ(),sqrt(sum),beta,rigidity,mass_on_Z,GetSide(),1),2.); 
    else if (kBeta&opt)
      sum = pow(TrEDepDB::GetHead()->GetEDepCorrectedValue(GetLayerJ(),sqrt(sum),beta,0,0,GetSide(),1),2.); 
    else // if no correction requested apply however relativistic particles normalization 
      sum = pow(TrEDepDB::GetHead()->GetEDepCorrectedValue(GetLayerJ(),sqrt(sum),1,0,0,GetSide(),1),2.);
    // MIP normalization
    if (kMIP&opt) {
      int iva = int(GetSeedAddress()/64);
      sum = pow(TrMipDB::GetHead()->GetMipCorrectedValue(sqrt(sum),GetTkId(),iva,3),2);
    }
    // go back with angle if not requested
    if (!(kAngle&opt)) sum /= GetCosTheta();
    if (kMeV&opt) sum = sum*0.081; // 81 keV per MIP 
  }
  return sum;
}


void TrClusterR::GetBoundsSymm(int &leftindex, int &rightindex, int nstrips, int opt) {
  /*               _        
   *              | |_      
   *           _ _| | |     
   *     _ _ _| | | | |_    
   *  __|_|_|_|_|_|_|_|_|__                   
   *     X X 6 4 3 1 2 5
   *     X X 6 5 2 1 3 4 (kFlip)          
   *         
   * Trying to contruct symmetrycally the bounds given the number of strips
   * - n = min(int((nstrips-1)/2),min(nleft,nright)) to be used automatically on left an right (symmetry)
   * - add one more strip if nstrips is odd or if the nleft and nright are dramatically different 
   */
  if (checkstatus(kFlip)!=0) opt|=kFlip;
  int cstrip     = GetSeedIndex(opt);
  int nleft      = GetLeftLength(opt);
  int nright     = GetRightLength(opt);
  int n          = min(int((nstrips-1)/2),min(nleft,nright));
  int nleftused  = min(n,nleft);
  int nrightused = min(n,nright);
  if ((nstrips-nleftused-nrightused-1)>0) { // add one more strip
    if      ( (nleft==nleftused)&&(nright==nrightused) ) {}
    else if (nleft  == nleftused)  nrightused++;
    else if (nright == nrightused) nleftused++;
    else    ( ( (GetSignal(cstrip-nleftused-1,opt)>GetSignal(cstrip+nrightused+1,opt))&&(!(opt&kFlip)) ) ||
              ( (GetSignal(cstrip-nleftused-1,opt)<GetSignal(cstrip+nrightused+1,opt))&&(  opt&kFlip ) )  ) ? nleftused++ : nrightused++;
  }
  leftindex  = cstrip - nleftused;
  rightindex = cstrip + nrightused;
}


void TrClusterR::GetBoundsAsym(int &leftindex, int &rightindex, int nstrips, int opt) {
  /*               _        
   *              | |_      
   *           _ _| | |     
   *     _ _ _| | | | |_ _   
   *  __|_|_|_|_|_|_|_|_|_|__                   
   *     8 7 5 4 3 1 2 5 6                      
   *
   * Trying to construct asymmetrycally the bounds given the number of strips
   * - add strips in hierarchical order
   */
  int cstrip     = GetSeedIndex(opt);
  int nleft      = GetLeftLength(opt);
  int nright     = GetRightLength(opt);
  int nleftused  = 0; 
  int nrightused = 0; 
  for (int ii=0; ii<nstrips-1; ii++) {
    if      ( (nleft==nleftused)&&(nright==nrightused) ) break;  
    else if (nleft  == nleftused)  nrightused++; 
    else if (nright == nrightused) nleftused++; 
    else    ( GetSignal(cstrip-nleftused-1,opt)>GetSignal(cstrip+nrightused+1,opt) ) ? nleftused++ : nrightused++; 
  }
  leftindex  = cstrip - nleftused;
  rightindex = cstrip + nrightused;
}


float TrClusterR::GetXCofG(int nstrips, int imult, int opt) {
  if (nstrips==-1) {
    nstrips = 2;
    if (GetSide() == 0 && fabs(GetDxDz()) > TwoStripThresholdX) nstrips = 3;
    if (GetSide() == 1 && fabs(GetDyDz()) > TwoStripThresholdY) nstrips = 3;
  } 
  if (nstrips==1) return 0.;
  int leftindex;  
  int rightindex;
  float numerator   = 0.;
  float denominator = 0.;
  GetBoundsSymm(leftindex,rightindex,nstrips,opt);
  // GetBoundsAsym(leftindex,rightindex,nstrips,opt);
  int cstrip = GetSeedIndex(opt);
  float seedposition = GetX(cstrip,imult);
  for (int index=leftindex; index<=rightindex; index++) {
    // signal weight
    float weight = GetSignal(index,opt);
    if (opt&kCoupl) { // trying to eliminate coupling effect
      if (index-1>0)          weight -= 0.04*GetSignal(index-1,opt);
      if (index+1<GetNelem()) weight -= 0.04*GetSignal(index+1,opt);
    }
    // position
    float position = GetX(index,imult);
    int   mask = 1;
    if (IsK7()!=0) { 
      // special computation for K7 (cyclic clusters)
      // - take seed strip as the strip with a fixed multiplicity
      int address = GetAddress(index); 
      int maxmult = TkCoo::GetMaxMult(GetTkId(),address);
      if ( (index<cstrip)&&(position>seedposition) ) {
        position = (imult>0) ? GetX(index,imult-1) : 0; 
        mask     = (imult>0) ? 1 : 0;
      }
      else if ( (index>cstrip)&&(position<seedposition) ) {  
        position = (imult<(maxmult-1)) ? GetX(index,imult+1) : 0;
        mask     = (imult<(maxmult-1)) ? 1 : 0;
      }
    }
    // weighted average
    numerator   += weight*position*mask;
    denominator += weight*mask;
    // cout << "numerator: " << numerator << "  denominator: " << denominator << endl;
  } 
  float CofG = numerator/denominator;
  return CofG; 
} 


float TrClusterR::GetCofG(int nstrips, int opt) {
  if (nstrips==-1) {
    nstrips = 2;
    if (GetSide() == 0 && fabs(GetDxDz()) > TwoStripThresholdX) nstrips = 3;
    if (GetSide() == 1 && fabs(GetDyDz()) > TwoStripThresholdY) nstrips = 3;
  }
  if (nstrips==1) return 0.;
  int leftindex; 
  int rightindex;
  float numerator   = 0.;
  float denominator = 0.;  
  // GetBoundsAsym(leftindex,rightindex,nstrips,opt);
  GetBoundsSymm(leftindex,rightindex,nstrips,opt);
  int cstrip = GetSeedIndex(opt);
  for (int index=leftindex; index<=rightindex; index++) {
    float weight = GetSignal(index,opt);
    numerator   += weight*(index-cstrip);
    denominator += weight;
  }
  float CofG = numerator/denominator;
  return CofG;
}


float TrClusterR::GetEta(int opt) {
  /*! Eta = center of gravity with the two higher strips = Q_{R} / ( Q_{L} + Q_{R} )
   *      _                                    _ 
   *    l|c|r          c*0 + r*1    r        l|c|r            l*0 + c*1    c
   *     | |_    eta = --------- = ---       _| |       eta = --------- = ---
   *    _| | |           c + r     c+r      | | |_              l + c     l+c
   * __|_|_|_|__                          __|_|_|_|__
   *      0 1                                0 1 
   *  Eta is 1 for particle near to the right strip,
   *  while is approx 0 when is near to the left strip (old definition) */
  if (GetNelem()<1) return -1;
  int cstrip = GetSeedIndex(opt);
  int nleft  = GetLeftLength(opt);
  int nright = GetRightLength(opt);
  // 1 strip
  if ( (nleft==0)&&(nright==0) ) return 0.; 
  // 2 strips
  if (nleft==0)  return GetSignal(cstrip+1,opt)/(GetSignal(cstrip+1,opt) + GetSignal(cstrip,opt));
  if (nright==0) return GetSignal(cstrip,  opt)/(GetSignal(cstrip-1,opt) + GetSignal(cstrip,opt));
  // >2 strips
  if ( GetSignal(cstrip+1,opt)>GetSignal(cstrip-1,opt) ) {
    return GetSignal(cstrip+1,opt)/(GetSignal(cstrip+1,opt) + GetSignal(cstrip,opt));
  }
  else {                                           
    return GetSignal(cstrip  ,opt)/(GetSignal(cstrip-1,opt) + GetSignal(cstrip,opt));
  } 
  return -1.;
}


float TrClusterR::GetClusterSN(int opt) {
  if (GetNelem()<1) return -1;
  float sum = 0;
  float sigma = 0;
  for (int ii=0; ii<GetNelem(); ii++) {
    sum += GetSignal(ii,opt);
    // if problems this sum will blow-up (default sigma is -9999)
    sigma += pow(GetSigma(ii),2); 
  }
  return sum/sqrt(sigma);
}


//////////////////////////////////////////////////////////////////////////////
// More functions for cluster quality
//////////////////////////////////////////////////////////////////////////////


bool TrClusterR::Check(int verbosity) {
  if (IsK7()) return true; 
  for (int istrip=0; istrip<GetNelem(); istrip++) {
    int address = GetAddress() + istrip;
    // no cluster out-of-bounds S (outside or between X and Y)
    if ( (GetSide()==1)&&((address<0)||(address>639))) {
      if (verbosity>0) printf("TrClusterR::Check-E cluster on S-side out of bounds (tkid=%+4d, strip=%4d, address=%4d, nelem=%2d)\n",GetTkId(),address,GetAddress(),GetNelem());
      return false;
    }
    // no cluster out-of-bounds K5 (outside or between X and Y)
    if ( (!IsK7())&&(GetSide()==0)&&((address>1023)||(address<640))) { 
      if (verbosity>0) printf("TrClusterR::Check-E cluster on K-side out of bounds (tkid=%+4d, strip=%4d, address=%4d, nelem=%2d)\n",GetTkId(),address,GetAddress(),GetNelem());
      return false;
    }
    if (istrip==GetNelem()-1) continue;
    // no cluster between sensors (for K5)
    if ( (!IsK7())&&(GetSide()==0)&&(int((address-640)/192)!=int((address-640+1)/192)) ) {
      if (verbosity>0) printf("TrClusterR::Check-E cluster on K-side across sensors (tkid=%+4d, strip=%4d, address=%4d, nelem=%2d)\n",GetTkId(),address,GetAddress(),GetNelem());
      return false;
    }
  }
  return true;
}


bool TrClusterR::CheckK7(int mult, int verbosity) {
  if (!IsK7()) return true;
  // ok if multiplicity is not defined
  if (mult<0) return true;
  // check that seed and all the strips are in the same sensor
  int seed_index = GetSeedIndex();
  int seed_sensor;
  int seed_addsen = GetSensorAddress(seed_sensor,seed_index,mult);
  for (int stri_index=0; stri_index<GetNelem(); stri_index++) {
    int stri_sensor;
    int stri_addsens = GetSensorAddress(stri_sensor,stri_index,mult,verbosity);
    // is wrong?
    if (stri_addsens<0) {
      // no error message 
      return false;
    }
    // is the strip on the same sensor of the seed? 
    if (stri_sensor!=seed_sensor) {
      if (verbosity>0) printf("TrClusterR::CheckK7-W cluster on K7-side across sensors (tkid=%+4d, strip=%4d, sens1=%4d, sens2=%2d)\n",
        GetTkId(),stri_addsens,stri_sensor,seed_sensor);
      return false;
    }
  }
  return true;
}


int TrClusterR::GetNStripWithCalibrationStatus(int nstrip_from_seed, int mask, int mult) {
  if (mult<0) mult = 0; 
  int nstatus = 0;
  int seed_index = GetSeedIndex();
  int seed_sensor;
  int seed_addsen = GetSensorAddress(seed_sensor,seed_index,mult);
  TrLadCal* ladcal = TrCalDB::Head->FindCal_TkId(GetTkId()); 
  for (int stri_index=seed_index-nstrip_from_seed; stri_index<=seed_index+nstrip_from_seed; stri_index++) {
    // has the strip a valid position (address,mult) ?  
    int stri_sensor;
    // no error produced if wrong, just skip (error can produced if requested strip is outside from sensor)
    int stri_addsens = GetSensorAddress(stri_sensor,stri_index,mult,0); 
    if (stri_addsens<0) continue; 
    // is the strip on the same sensor of the seed? 
    if (stri_sensor!=seed_sensor) continue;
    // condition 
    int stri_address = GetAddress(stri_index);
    short status = ladcal->GetStatus(stri_address);
    if (status&mask) nstatus++;
  }
  return nstatus;
}


int TrClusterR::GetNStripWithOccupancyStatus(int nstrip_from_seed, int mask, int mult) {
  if (mult<0) mult = 0; 
  int nstatus = 0;
  int seed_index = GetSeedIndex();
  int seed_sensor;
  int seed_addsen = GetSensorAddress(seed_sensor,seed_index,mult);
  for (int stri_index=seed_index-nstrip_from_seed; stri_index<=seed_index+nstrip_from_seed; stri_index++) {
    // has the strip a valid position (address,mult) ?  
    int stri_sensor;
    int stri_addsens = GetSensorAddress(stri_sensor,stri_index,mult,0);
    // no error produced if wrong, just skip (error can produced if requested strip is outside from sensor)
    if (stri_addsens<0) continue;
    // is the strip on the same sensor of the seed? 
    if (stri_sensor!=seed_sensor) continue;
    // condition 
    int stri_address = GetAddress(stri_index);
    int tkid = GetTkId();
    TrLadOcc* ladocc = TrOccDB::GetHead()->FindOccTkId(tkid);
    short status = ladocc->GetStatus(stri_address);
    if (status&mask) nstatus++;
  }
  return nstatus;
}


int TrClusterR::GetNStripWithGainStatus(int nstrip_from_seed, int mask, int mult) {
  if (mult<0) mult = 0; 
  int nstatus = 0;
  int seed_index = GetSeedIndex();
  int seed_sensor;
  int seed_addsen = GetSensorAddress(seed_sensor,seed_index,mult);
  for (int stri_index=seed_index-nstrip_from_seed; stri_index<=seed_index+nstrip_from_seed; stri_index++) {
    // has the strip a valid position (address,mult) ?  
    int stri_sensor;
    // no error produced if wrong, just skip (error can produced if requested strip is outside from sensor)
    int stri_addsens = GetSensorAddress(stri_sensor,stri_index,mult,0);
    if (stri_addsens<0) continue;
    // is the strip on the same sensor of the seed? 
    if (stri_sensor!=seed_sensor) continue;
    // condition 
    int stri_address = GetAddress(stri_index);
    int tkid = GetTkId();
    int iva = int(stri_address/64);
    int status = TrGainDB::GetHead()->FindGainTkId(tkid)->GetStatus(iva);
    if (status&mask) nstatus++;
  }
  return nstatus;
}


int TrClusterR::GetNStripOnTheEdgeOfSensor(int nstrip_from_seed, int mult) {
  // no judgment if K7 and multiplicity is not defined
  if ( (mult<0)&&(IsK7()) ) return 0;
  if (mult<0) mult = 0; // doesn't really matter in this method for K5 and S
  // evaluation
  int nstrip = 0;
  int first = 0;
  int last = 639;
  if (GetSide()==0) last = (IsK7()) ? 223 : 191;
  if (GetSide()==1) mult = 0;
  int seed_index = GetSeedIndex();
  int seed_sensor;
  int seed_addsen = GetSensorAddress(seed_sensor,seed_index,mult);
  for (int stri_index=seed_index-nstrip_from_seed; stri_index<=seed_index+nstrip_from_seed; stri_index++) {
    // has the strip a valid position (address,mult) ?  
    int stri_sensor;
    // no error produced if wrong, just skip (error can produced if requested strip is outside from sensor)
    int stri_addsens = GetSensorAddress(stri_sensor,stri_index,mult,0);
    if (stri_addsens<0) continue;
    // is the strip on the same sensor of the seed? 
    if (stri_sensor!=seed_sensor) continue;
    // condition 
    if ( (stri_addsens==first)||(stri_addsens==last) ) nstrip++;
  }
  return nstrip;
}


int TrClusterR::GetNStripOnTheEdgeOfVA(int nstrip_from_seed, int mult) {
  // no judgment if K7 and multiplicity is not defined
  if ( (mult<0)&&(IsK7()) ) return 0;
  if (mult<0) mult = 0; // doesn't really matter in this method for K5 and S
  // evaluation
  int nstrip = 0;
  if (GetSide()==1) mult = 0;
  int seed_index = GetSeedIndex();
  int seed_sensor;
  int seed_addsen = GetSensorAddress(seed_sensor,seed_index,mult);
  for (int stri_index=seed_index-nstrip_from_seed; stri_index<=seed_index+nstrip_from_seed; stri_index++) {
    // has the strip a valid position (address,mult) ?  
    int stri_sensor;
    // no error produced if wrong, just skip (error can produced if requested strip is outside from sensor)
    int stri_addsens = GetSensorAddress(stri_sensor,stri_index,mult,0);
    if (stri_addsens<0) continue;
    // is the strip on the same sensor of the seed? 
    if (stri_sensor!=seed_sensor) continue;
    // condition 
    int stri_address = GetAddress(stri_index);
    int stri_addrva  = stri_address%64;
    if (stri_addrva==63) nstrip++;
  }
  return nstrip;
}


bool TrClusterR::IsMonotonic(int nstrip_from_seed) { 
  bool return_value = true;
  // sub-cluster indexes
  int seedadd = GetSeedIndex();
  int leftadd = seedadd-nstrip_from_seed; 
  if (leftadd<=0) leftadd = 0;
  int righadd = seedadd+nstrip_from_seed;
  if (righadd>=GetNelem()) righadd = GetNelem()-1;
  // monotonic on the left
  for (int istrip=seedadd; istrip>=leftadd+1; istrip--) 
    if (GetSignal(istrip)<GetSignal(istrip-1)) return_value = false;
  // monotonic on the right
  for (int istrip=seedadd; istrip<=righadd-1; istrip++) 
    if (GetSignal(istrip)<GetSignal(istrip+1)) return_value = false; 
  return return_value;
}


bool TrClusterR::IsMonotonicWithThreshold(float threshold) {
  bool return_value = true;
  int seedadd = GetSeedIndex();
  for (int istrip=seedadd; istrip>0; istrip--) {
    if (GetSN(istrip)<threshold) break;
    if (GetSignal(istrip)<GetSignal(istrip-1)) return_value = false;
  }
  for (int istrip=seedadd; istrip<GetNelem()-1; istrip++) {
    if (GetSN(istrip)<threshold) break;
    if (GetSignal(istrip)<GetSignal(istrip+1)) return_value = false;
  }
  return return_value;
}


bool TrClusterR::IsOverThreshold(int nstrip_from_seed, float threshold) {
  bool return_value = true;
  // sub-cluster indexes
  int seedadd = GetSeedIndex();
  int leftadd = seedadd-nstrip_from_seed;
  if (leftadd<=0) leftadd = 0;
  int righadd = seedadd+nstrip_from_seed;
  if (righadd>=GetNelem()) righadd = GetNelem()-1;
  // is everything over threshold?
  for (int istrip=leftadd; istrip<=righadd; istrip++) 
    if (GetSN(istrip)<threshold) return_value = false;
  return return_value;
}


float TrClusterR::GetSignalToSignalRatio(int nstrip_from_seed) {
  // sub-cluster indexes
  int seedadd = GetSeedIndex();
  int leftadd = seedadd-nstrip_from_seed;
  if (leftadd<=0) leftadd = 0;
  int righadd = seedadd+nstrip_from_seed;
  if (righadd>=GetNelem()) righadd = GetNelem()-1;
  // sum everything (GetTotSignal without corretions)
  float sum_all = 0;
  for (int istrip=0; istrip<GetNelem(); istrip++)
    sum_all += GetSignal(istrip);
  // sum sub-cluster     
  float sum_sub = 0;
  for (int istrip=leftadd; istrip<=righadd; istrip++) 
    sum_sub += GetSignal(istrip);
  if (sum_all>0) return sum_sub/sum_all;
  return -1;
}


int TrClusterR::GetQStatus(int nstrip_from_seed, int mult) {
  return    
    ((GetNStripWithCalibrationStatus(nstrip_from_seed,0x1|0x4,mult)>0)*0x1) +  
    ((GetNStripWithCalibrationStatus(nstrip_from_seed,0x2|0x8,mult)>0)*0x2) +
    ((GetNStripWithCalibrationStatus(nstrip_from_seed,0x200,mult)>0)*0x4) +             
    ((GetNStripWithCalibrationStatus(nstrip_from_seed,0x8000,mult)>0)*0x8) + 
    ((GetNStripWithOccupancyStatus(nstrip_from_seed,0x1,mult)>0)*0x10) +
    ((GetNStripWithOccupancyStatus(nstrip_from_seed,0x2,mult)>0)*0x20) +
    ((GetNStripWithGainStatus(nstrip_from_seed,0x1F,mult)>0)*0x40) +
    ((GetNStripWithGainStatus(nstrip_from_seed,0x01,mult)>0)*0x80) +
    ((GetNStripOnTheEdgeOfSensor(nstrip_from_seed,mult)>0)*0x100) +
    ((GetNStripOnTheEdgeOfVA(nstrip_from_seed,mult)>0)*0x200) + 
    ((!Check(mult))*0x400) + 
    ((!CheckK7(mult))*0x800);
}


int TrClusterR::GetMorfologyStatus() {
  return
    ((!IsMonotonic(2))*0x1) +
    ((!IsMonotonic(3))*0x2) +
    ((!IsMonotonicWithThreshold(0.0))*0x4) +
    ((!IsMonotonicWithThreshold(1.5))*0x8) +
    ((!IsMonotonicWithThreshold(3.0))*0x10) +
    ((!IsMonotonicWithThreshold(4.5))*0x20);
}


int TrClusterR::GetNInterstrip(int mult) {
  int nelem   = GetNelem();
  if (nelem==1) return -1;
  int seedadd = GetSeedIndex();
  int leftadd = seedadd-1;
  int righadd = seedadd+1;
  int add1    = seedadd;
  int add2    = seedadd;
  if ( (leftadd< 0)&&(righadd< nelem) ) add2 = righadd;
  if ( (leftadd>=0)&&(righadd>=nelem) ) add2 = leftadd;
  if ( (leftadd>=0)&&(righadd< nelem) ) 
    add2 = (GetSignal(leftadd)>GetSignal(righadd)) ? leftadd : righadd;
  int index = (add2<add1) ? add2 : add1;
  // sensor address 
  int sens;
  int add = GetSensorAddress(sens,index,mult); 
  // S side
  if (GetSide()==1) {
    //  implantation 0000 0001 0002 0003 0004 0005 0006 0007 0008 0009 0010 0011 0012 ... 2555 2556 2557 2558 2559 2560 2561 2562 2563 2564 2565 2566 2567
    //  readout       000                 xxx                 001                 002           638                 xxx                 639
    if ( (add==0)||(add==638) ) return 7;
    else                        return 3;
  }
  // K5 side
  if ( (GetSide()==0)&&(!IsK7()) ) {
    //  implantation  000 001 002 003 004 005 006 ... 378 379 380 381 382 383
    //  readout         0       1       2       3     189     190         191
    if (add==190) return 2;
    else          return 1; 
  }
  // K7 side
  if ( (GetSide()==0)&&(IsK7()) ) {
    //  implantation  000 001 002 003 004 005 006 ... 092 093 094 095 | 096 097 098 ... 286 | 287 288 289 290 291 ... 380 381 382 383
    //  readout         0       1   2       3   4      61  62      63 |  64      65     159 |     160     161 162     221 222     223
    if      (add<= 63) return ((add%2)==0) ? 1 : 0;
    else if (add<=159) return 1; 
    else               return ((add%2)==0) ? 1 : 0;
  }
  return -1;
}


//////////////////////////////////////////////////////////////////////////////
// Old stuff part (mostly deprecated/old methods left for backward compatibility)
//////////////////////////////////////////////////////////////////////////////


// function derived from truncated mean X/Y comparison of ISS data B538 (Oct 2011) 
static float n_to_p_pars[7] = {0.99925,0.00125804,-0.000345359,4.77114e-06,-2.04879e-08,-6.26803e-12,1.71962e-13};
float TrClusterR::ConvertToPSideScale(float adc) {
  if (GetSide()==1) return adc; // already p-side
  double x = sqrt(adc);
  double corr = 0;
  for (int ipar=0; ipar<7; ipar++) corr += pow(x,ipar)*n_to_p_pars[ipar];
  double sqrt_p = corr*x;
  return sqrt_p*sqrt_p;
}


// function derived from truncated mean X/Y comparison of ISS data B538 (Oct 2011) 
static float p_to_n_pars[10] = {2.00371,-0.36054,0.0536171,-0.00417238,0.000186663,-4.94765e-06,7.89718e-08,-7.46039e-10,3.84518e-12,-8.33832e-15};
float TrClusterR::ConvertToNSideScale(float adc) {
  if (GetSide()==0) return adc; // already n-side
  double x = sqrt(adc);
  double corr = 0;
  for (int ipar=0; ipar<10; ipar++) corr += pow(x,ipar)*p_to_n_pars[ipar];
  double sqrt_n = corr*x;
  return sqrt_n*sqrt_n;
}


/* 
  Beta Correction Parameters
  - KSC 2010, muons + proton signal   
  - Plane-by-plane vs beta TOF normalized
  - Three values for 1st plane, anyone of the inner, 9th plane
  - Physical folding
*/
static float A_BetaCorr[3]  = { 1.14505, 0.73906, -0.39480};
static float B_BetaCorr[3]  = { 0.67118, 0.05288, -1.47569};
static float b0_BetaCorr[3] = { 0.85387, 0.89147,  0.93449};
float TrClusterR::BetaCorrection_Muons_2010(float beta) {
  /*
     - Maximum Probability Energy Loss:
       MPEL(300 um of Si) = 53.6614 eV / beta^2 * { 12.1489 - 2 log(beta) - beta^2 - 0.1492 * 
                            * [max(0,2.8716-log(beta gamma)/log(10))]^3.2546}, beta > 0.20
     - Simplified fitting function:
       f(beta) = beta<beta0, A/beta^2 + B*log(beta)/beta^2 + C
                 beta>beta0, k (the TOF beta "saturates")     
       Continuity imposed on beta0 (no derivative continuity)
       C = k - A/beta0^2 - B*log(beta0)/beta0^2
     - Beta Correction:
       g(beta) = beta<beta0, A/beta^2 + B*log(beta)/beta^2 + 1 - A/beta0^2 - B*log(beta0)/beta0^2
                 beta>beta0, 1
     - Physical folding:
       use abs(beta) instead of beta 
       use the upper plane coefficient for the lower one and viceversa in case of negative beta
  */
  int jlayer = GetLayerJ();
  int index  = 0;        // 0 for the layer over TRD
  if (jlayer>1) index++; // 1 for any inner tracker layer 
  if (jlayer>8) index++; // 2 for the layer over ECAL
  if ( (jlayer<1)||(jlayer>9) ) {
    printf("TrClusterR::BetaCorrection-W invalid layer index number (%d), returning 1.\n",index);
    return 1.;
  }
  if (beta<0) index = 2 - index; // physical folding
  beta = fabs(beta);
  if (beta>=b0_BetaCorr[index]) return 1.; // beta "saturation" region
  return A_BetaCorr[index]/pow(beta,2) +
         B_BetaCorr[index]*log(beta)/pow(beta,2) +
         1 -
         A_BetaCorr[index]/pow(b0_BetaCorr[index],2) -
         B_BetaCorr[index]*log(b0_BetaCorr[index])/pow(b0_BetaCorr[index],2);
}


float TrClusterR::BetaCorrection_ISS_2011(float beta) {
  int jlayer = GetLayerJ();
  return AMSEnergyLoss::GetTrackerLayerLogBetaGammaCorrectionFromBeta(jlayer,beta);
}


float TrClusterR::RigidityCorrection(float rigidity, float mass_on_Z) {
  int jlayer = GetLayerJ();
  return AMSEnergyLoss::GetTrackerLayerLogBetaGammaCorrectionFromRigidity(jlayer,rigidity,mass_on_Z); 
}


float TrClusterR::BetaRigidityCorrection(float beta, float rigidity, float mass_on_Z) {
  int jlayer = GetLayerJ();
  return AMSEnergyLoss::GetTrackerLayerLogBetaGammaCorrection(jlayer,beta,rigidity,mass_on_Z);  
}


/* OLD MIP Correction Parameters (DEPRECATED)
  - Extracted by preliminar charge reconstruction ISS data (15/03/2012)
  - No p-strip correction
  - No charge loss
  - Old gain
  - Forced to pass through 0,0
  - A point at charge 50 is added extrapolating linerly from the two last points (for around Iron stuff)
*/
static Int_t    npoints_x_iss11 = 14;
static Double_t sqrtmip_x_iss11[14] = {
   0.000000,  1.000000,  2.000000,  3.000000,  4.000000,  5.000000,  6.000000,
   7.000000,  8.000000, 10.000000, 12.000000, 14.000000, 26.000000, 50.000000
};
static Double_t sqrtadc_x_iss11[14] = {
   0.000000,  5.531720, 11.296013, 18.371866, 25.171396, 31.897547, 38.464909,
   44.658779, 50.470642, 60.331924, 68.210243, 74.606316,103.365150,160.882820
};
static Int_t    npoints_y_iss11 = 14;
static Double_t sqrtmip_y_iss11[14] = {
   0.000000,  1.000000,  2.000000,  3.000000,  4.000000,  5.000000,  6.000000,
   7.000000,  8.000000, 10.000000, 12.000000, 14.000000, 26.000000, 50.000000
};
static Double_t sqrtadc_y_iss11[14] = {
   0.000000,  5.567749, 11.480528, 17.141417, 21.615240, 25.100355, 27.365740,
   29.365822, 30.827951, 33.765102, 37.085659, 40.455532, 64.482658,112.536911
};
/* 
  MIP correction parameters from Pierre (as result of the charge loss procedure)
  - New charge reconstruction (Sep. 2012): full range charge loss corrections, new gain tables 
  - Forced to pass through (0,0). Charge 50 is added extrapolating linerly/
*/
// Original kMIP correction from Pierre
static Int_t     npoints_x_iss12 = 16;
static Double_t  sqrtmip_x_iss12[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,14,26,50};
static Double_t  sqrtadc_x_iss12[16] = {0,5.36449,11.2985,17.8391,24.4941,31.3103,37.1813,43.1753,49.2113,54.31,59.4016,63.6657,67.0189,73.84,102.47,159.7298};
static Int_t     npoints_y_iss12 = 16;
static Double_t  sqrtmip_y_iss12[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,14,26,50};
static Double_t  sqrtadc_y_iss12[16] = {0,5.42537,11.1375,16.7609,21.2022,24.4082,26.6097,28.3442,29.8655,31.0935,33.0569,35.5054,36.8489,40.0563,63.498,111.2657};
TSpline3* TrClusterR::sqrtadc_to_sqrtmip_spline[2] = {0,0};
/* 
   MIP correction refinement done after Pierre correction (more effective since wer are in a quasi-linear approximation) 
   - Not really needed layer-by-layer. 
   - Layer J1 shows higher "gain" in both x and y.
   - Few month statistics not enough for low abundance charges (to be completed with high statistics).
   - p-side middle charges (CNO group) have very bad pdf. Most probably is due to saturation itself. 
*/
static Int_t    npoints_iss12 = 16;
static Double_t sqrtmip_iss12[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,14,26,50};
static Double_t sqrtref_x_iss12[9][16] = {
  {0,  1.0322,  2.0489,  3.0693,  4.0631,  5.0415,  6.2071,  7.2126,  8.3645,  9.4535, 10.3910, 11.6805, 12.8976, 15.3946, 30.1871, 59.7721},
  {0,  1.0350,  2.0461,  3.0605,  4.0527,  5.0613,  6.1905,  7.1984,  8.3238,  9.3426, 10.4076, 11.5819, 12.8762, 15.4510, 29.8118, 58.5334},
  {0,  1.0449,  2.0514,  3.0610,  4.0550,  5.0624,  6.1793,  7.1752,  8.2715,  9.2997, 10.2858, 11.3916, 12.5588, 14.9659, 27.4918, 52.5436},
  {0,  1.0484,  2.0568,  3.0659,  4.0604,  5.0713,  6.1914,  7.1918,  8.2965,  9.3083, 10.2897, 11.3468, 12.5040, 14.8803, 27.2409, 51.9622},
  {0,  1.0406,  2.0491,  3.0608,  4.0575,  5.0692,  6.1887,  7.1962,  8.3110,  9.3488, 10.3997, 11.5029, 12.7061, 15.0752, 28.2011, 54.4528},
  {0,  1.0386,  2.0519,  3.0670,  4.0563,  5.0564,  6.1734,  7.1651,  8.2539,  9.2869, 10.2801, 11.3809, 12.5284, 14.9131, 27.1859, 51.7317},
  {0,  1.0516,  2.0599,  3.0681,  4.0510,  5.0542,  6.1663,  7.1692,  8.2687,  9.2966, 10.2959, 11.3675, 12.5747, 14.8496, 27.6746, 53.3247},
  {0,  1.0479,  2.0572,  3.0672,  4.0629,  5.0740,  6.1913,  7.1944,  8.3050,  9.3272, 10.3588, 11.4431, 12.6560, 15.0087, 27.8669, 53.5833},
  {0,  1.0462,  2.0650,  3.0788,  4.0871,  5.0459,  6.1650,  7.1653,  8.2535,  9.3581, 10.2502, 11.2678, 12.4975, 15.0953, 26.0000, 47.8094}
};
static Double_t sqrtref_y_iss12[9][16] = {
  {0,  1.0291,  2.0627,  3.1274,  4.2725,  5.6714,  7.1794,  8.6908,  9.4906, 10.2831, 11.5284, 12.9723, 13.8364, 16.1600, 28.5842, 53.4327},
  {0,  1.0561,  2.0759,  3.1309,  4.2854,  5.6802,  7.2532,  8.3584,  9.1859,  9.7849, 10.7260, 11.7720, 12.9885, 14.8882, 26.5852, 49.9791},
  {0,  1.0536,  2.0739,  3.1070,  4.1946,  5.4368,  6.7961,  7.8707,  8.8287,  9.6105, 10.2936, 11.6422, 12.8606, 14.8479, 26.7053, 50.4202},
  {0,  1.0354,  2.0656,  3.0870,  4.1484,  5.3347,  6.6247,  7.6550,  8.5860,  9.4774, 10.0505, 11.2958, 12.6397, 14.6636, 26.3616, 49.7576},
  {0,  1.0619,  2.0856,  3.1200,  4.2038,  5.4659,  6.8993,  8.0564,  9.0063,  9.6648, 10.3244, 11.7713, 13.0439, 15.0478, 27.1355, 51.3109},
  {0,  1.0504,  2.0718,  3.1062,  4.2029,  5.4722,  6.8946,  8.0276,  8.9763,  9.6493, 10.3395, 11.8026, 13.0911, 15.1686, 27.2059, 51.2806},
  {0,  1.0102,  2.0447,  3.0617,  4.1086,  5.2329,  6.4542,  7.5464,  8.3948,  9.3438,  9.9125, 11.0561, 12.5202, 14.5890, 27.1602, 52.3027},
  {0,  1.0414,  2.0630,  3.0905,  4.1502,  5.3162,  6.5415,  7.6901,  8.5096,  9.4463,  9.9231, 11.1030, 12.4809, 14.4799, 26.0203, 49.1012},
  {0,  1.0319,  2.0616,  3.1091,  4.2062,  5.3760,  6.5881,  7.7158,  8.5756,  9.7031, 10.2494, 11.8778, 13.2512, 15.2821, 28.0574, 53.6081}
};
TSpline3* TrClusterR::sqrtref_to_sqrtmip_spline[2][9] = { {0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0} };
float TrClusterR::GetNumberOfMIPs_ISS(float adc) {
  // initialize if needed
  if (!sqrtadc_to_sqrtmip_spline[0]) sqrtadc_to_sqrtmip_spline[0] = new TSpline3("sqrtadc_to_sqrtmip_x",sqrtadc_x_iss12,sqrtmip_x_iss12,npoints_x_iss12);
  if (!sqrtadc_to_sqrtmip_spline[1]) sqrtadc_to_sqrtmip_spline[1] = new TSpline3("sqrtadc_to_sqrtmip_y",sqrtadc_y_iss12,sqrtmip_y_iss12,npoints_y_iss12);
  for (int ilay=0; ilay<9; ilay++) {
    if (!sqrtref_to_sqrtmip_spline[0][ilay])
      sqrtref_to_sqrtmip_spline[0][ilay] = new TSpline3(Form("sqrtref_to_sqrtmip_x_%02d",ilay),&sqrtref_x_iss12[ilay][0],sqrtmip_iss12,npoints_iss12);
    if (!sqrtref_to_sqrtmip_spline[1][ilay])
      sqrtref_to_sqrtmip_spline[1][ilay] = new TSpline3(Form("sqrtref_to_sqrtmip_y_%02d",ilay),&sqrtref_y_iss12[ilay][0],sqrtmip_iss12,npoints_iss12);
  }
  // calculation 
  int   iside = GetSide();
  int   ilayer = GetLayerJ()-1;
  float sqrtadc = (adc>0) ? sqrt(adc) : 0;
  float sqrtref = sqrtadc_to_sqrtmip_spline[iside]->Eval(sqrtadc);
  float sqrtmip = sqrtref_to_sqrtmip_spline[iside][ilayer]->Eval(sqrtref);
  return pow(sqrtmip,2);
}


/* 
  MIP Correction Parameters (DEPRECATED)
  - These parameters are extracted from TB2003.
  - Straight tracks 
  - On readout strip 
  - No p-strip correction
*/
static float adc_vs_z_tb03[2][12] = {
  {  40.50, 167.92, 387.77, 713.61,1124.73,1615.55,2166.01,2734.12,3257.81,3671.87,4021.40,4290.52}, // n-side
  {  31.50, 106.58, 214.47, 304.94, 368.64, 413.37, 472.73, 575.47, 702.39, 854.23,1039.92,1233.86}  // p-side
  // {  31.50, 106.58, 209.81, 335.58, 525.61, 747.78, 977.83,1299.27,1609.36,1919.63,2220.36,2533.95}  // p-side corr
};

float TrClusterR::GetNumberOfMIPs_TB_2003(float adc) {
  /*
    These parameters are extracted from TB2003.
    - straight tracks 
    - readout strip 
    - no p-strip correction
  */
  int iside = GetSide();
  double x = adc;
  // lower extrapolation 
  if (adc<adc_vs_z_tb03[iside][0]) {
    double x0 = adc_vs_z_tb03[iside][0];
    double x1 = adc_vs_z_tb03[iside][1];
    double y0 = 1*1;
    double y1 = 2*2;
    return ( (x-x0)*y1 + (x1-x)*y0 ) / (x1-x0);
  }
  // upper extrapolation
  if (adc>=adc_vs_z_tb03[iside][11]) {
    double x0 = adc_vs_z_tb03[iside][10];
    double x1 = adc_vs_z_tb03[iside][11];
    double y0 = 11*11;
    double y1 = 12*12;
    return ( (x-x0)*y1 + (x1-x)*y0 ) / (x1-x0);
  }
  // inside range
  for (int i=0; i<11; i++) {
    if ((adc>=adc_vs_z_tb03[iside][i])&&(adc<adc_vs_z_tb03[iside][i+1])) {
      double x0 = adc_vs_z_tb03[iside][i];
      double x1 = adc_vs_z_tb03[iside][i+1];
      double y0 = (i+1)*(i+1);
      double y1 = (i+2)*(i+2);
      return ( (x-x0)*y1 + (x1-x)*y0 ) / (x1-x0);
    }
  }
  return -100;
}


