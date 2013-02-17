//  $Id: TrMCCluster.C,v 1.32 2013/02/17 16:32:18 mduranti Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrMCCluster.C
///\brief Source file of AMSTrMCCluster class
///
///\date  2008/02/14 SH  First import from Gbatch
///\date  2008/03/17 SH  Compatible with new TkDBc and TkCoo
///\date  2008/04/02 SH  Compatible with new TkDBc and TkSens
///$Date: 2013/02/17 16:32:18 $
///
///$Revision: 1.32 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkDBc.h"
#include "TkCoo.h"
#include "TkSens.h"
#include "tkdcards.h"

#include "TrMCCluster.h"

#include "TString.h"
#include "TrSimSensor.h"
#include "TrSimCluster.h"
#include "TrSim.h"
#include "TrParDB.h"
#include "HistoMan.h"

#include <cmath>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

#define VERBOSE 0 
#define WARNING 0

extern "C" double rnormx();
 

ClassImp(TrMCClusterR);


int TrMCClusterR::_NoiseMarker(555);


TrMCClusterR::TrMCClusterR(int idsoft, float step, AMSPoint xgl, AMSPoint dir, float mom, float edep, int itra)
  : _idsoft(idsoft), _itra(itra), _step(step), _xgl(xgl), _dir(dir), _mom(mom), _edep(edep) {
  Init();
  Status = 0;
}


TrMCClusterR::TrMCClusterR(AMSPoint xgl, integer itra, geant edep):
  _idsoft(0), _step(0), _itra(itra), _xgl(xgl), _dir(0,0,0), _mom(0), _edep(edep) {
  Init();
  TkSens pp(_xgl,1);
  if(pp.LadFound()){
    int tkid = pp.GetLadTkID();
    int side = (tkid>0) ? 1 : 0;
    _idsoft = abs(tkid) + 1000*side + 10000*pp.GetSensor();
  } 
  Status = 0;
}


TrMCClusterR& TrMCClusterR::operator=(const TrMCClusterR& that) {
  if (this!=&that) {
    Copy(that);
  } 
  return *this;
}


void TrMCClusterR::Init() {
  for(int ii=0; ii<2; ii++){
    _simcl[ii] = 0;
  }
}


void TrMCClusterR::Clear() { 
  for(int ii=0; ii<2; ii++){
    if (_simcl[ii]!=0) { 
      delete _simcl[ii];
    }
    _simcl[ii] = 0; 
  }
}


void TrMCClusterR::Copy(const TrMCClusterR& that) {
  _idsoft = that._idsoft;
  _step = that._step;
  _itra = that._itra;
  _xgl = that._xgl;
  _dir = that._dir;
  _mom = that._mom;
  _edep =that._edep;
  Status = that.Status;
  for(int ii=0; ii<2; ii++){
    if (that._simcl[ii]!=0) _simcl[ii] = new TrSimCluster(*that._simcl[ii]);
    else                    _simcl[ii] = 0;
  }
}


int TrMCClusterR::GetTkId(){ 
  // int sensor = abs(_idsoft)/10000;
  int tkid   = abs(_idsoft)%1000;
  int ss     = abs(_idsoft)%10000-tkid;
  if(!ss) tkid*=-1;
  return tkid;
}


std::ostream& TrMCClusterR::putout(std::ostream &ostr ){
  _PrepareOutput(1);
  return ostr << sout  << std::endl;
}


void TrMCClusterR::Print(int opt) { 
  _PrepareOutput(opt);
  cout << sout;
}


const char* TrMCClusterR::Info(int iRef){
  string aa;
  aa.append(Form("TrMCCluster #%d ",iRef));
  _PrepareOutput(0);
  aa.append(sout);
  int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}


void TrMCClusterR::_PrepareOutput(int full) {
  sout.clear();
  sout.append(
    Form("Part: %3d  Mom(GeV): %12.6f TkId: %+04d  Sens: %2d  Edep(keV): %9.3f  Step(um): %7.1f   X:%8.3f Y:%8.3f Z:%8.3f   Cx: %8.5f Cy: %8.5f Cz: %8.5f\n",
      _itra,
      GetMomentum(),GetTkId(),GetSensor(),GetEdep()*1e+6,GetStep()*1e+4,
      GetXgl().x(),GetXgl().y(),GetXgl().z(),
      GetDir().x(),GetDir().y(),GetDir().z()
    )
  );
  return;
}


void TrMCClusterR::GenSimClusters(){

  // montecarlo truth
  char     sidename[2] = {'x','y'};
  float    step = GetStep()*1.e4;     // step [um] 
  AMSPoint glo = GetXgl();            // coordinate [cm]
  AMSDir   dir = GetDir();            // direction 
  float    momentum = GetMomentum();  // momentum vector [GeV/c]
  float    edep = GetEdep()*1.e6;     // energy deposition [keV] 
  if (edep<1) return;                 // if energy deposition < 1 keV 
  if (momentum<1e-6) return;          // if momentum < keV/c

  TkSens _glo2loc(1);
  _glo2loc.SetGlobal(GetTkId(),glo,dir);                                    // from global to local

  int  nsensor = _glo2loc.GetSensor();                                      // sensor number
  double ip[2] = {_glo2loc.GetSensCoo().x(),_glo2loc.GetSensCoo().y()};     // sensor impact point
  double ia[2] = {_glo2loc.GetImpactAngleXZ(),_glo2loc.GetImpactAngleYZ()}; // sensor impact angle
  int imult = _glo2loc.GetMultIndex();

  if (VERBOSE) {
    printf("TrSim::GenSimClusters-V  tkid = %+4d   loc(x,y) = (%7.4f,%7.4f)   theta(xz,yz) = (%7.4f,%7.4f)   nsens = %2d\n",
           GetTkId(),ip[0],ip[1],ia[0],ia[1],nsensor);
    printf("TrSim::GenSimClusters-V  laddcoo(x,y) = (%7.4f,%7.4f)   readout(x,y) = (%4d,%4d)   mult = %2d\n",
           _glo2loc.GetLaddCoo().x(),_glo2loc.GetLaddCoo().y(),_glo2loc.GetStripX(),_glo2loc.GetStripY(),imult);
  }

  // loop on two sides of the ladder
  for (int iside=0; iside<2; iside++) {

    /*
    if ( (ip[iside]<0.)||(ip[iside]>TkDBc::Head->_ssize_active[iside]) ) {
      if (WARNING) printf("TrSim::GenSimClusters-W  %c coordinate out of the sensor (min=0, max=%7.4f, coo=%7.4f)\n",
                          sidename[iside],TkDBc::Head->_ssize_active[iside],ip[iside]);
      continue;
    }
    */

    // create the simulated cluster
    TrSimCluster simcluster = TrSim::GetTrSimSensor(iside,GetTkId())->MakeCluster(ip[iside],ia[iside],nsensor);
    // from time to time the cluster is empty
    if (simcluster.GetWidth()==0) continue;

    // put the cluster in the TrMCCluster object
    if(_simcl[iside]) delete _simcl[iside];
    _simcl[iside] = new TrSimCluster(simcluster);
    // raw signal
    hman.Fill(Form("TrSimSig%c",sidename[iside]),_simcl[iside]->GetEta(),_simcl[iside]->GetTotSignal());

    ///////// ALL THIS PART SHOULD BE MOVED AFTER CLUSTER LOCAL MERGING /////////

    // from keV to ADC (using tb2003 data normalized to datacard value)
    double ADC = TrSim::GetTrSimSensor(iside,GetTkId())->GetkeVtoADC(edep);
    // cluster strip values in ADC counts
    _simcl[iside]->Multiply(ADC);
    // simulation tuning parameter 1: gaussianize a fraction of the strip signal
    _simcl[iside]->GaussianizeFraction(TRMCFFKEY.TrSim2010_FracNoise[iside]);
    // simulation tuning parameter 2: add more noise 
    _simcl[iside]->AddNoise(TRMCFFKEY.TrSim2010_AddNoise[iside]);
    // apply asymmetry to strips
    _simcl[iside]->ApplyAsymmetry(iside);
    // apply the p-strip saturation
    if (TRMCFFKEY.TrSim2010_PStripCorr==1) _simcl[iside]->ApplyStripNonLinearity();
    // apply the gain table
    _simcl[iside]->ApplyGain(iside,GetTkId());
    // apply saturation
    _simcl[iside]->ApplySaturation(TRMCFFKEY.TrSim2010_ADCSat[iside]);

    // dump
    if (VERBOSE) { printf("TrSim::GenSimClusters-V  ADC=%f\n",ADC); _simcl[iside]->Info(10);  }
    // histograms
    double adc = _simcl[iside]->GetTotSignal();
    double eta = _simcl[iside]->GetEta();
    double intr_res = 0.;
    // intrinsic resolution with only 2 strips
    if (iside==0) {
      intr_res = 1e+04*(_glo2loc.GetLaddCoo().x() - _simcl[iside]->GetX(iside,GetTkId(),2,imult));
    }
    else {
      intr_res = 1e+04*(_glo2loc.GetLaddCoo().y() - _simcl[iside]->GetX(iside,GetTkId(),2,imult));
    }
    // check eta distribution
    hman.Fill(Form("TrSimEta%c",sidename[iside]),fabs(ia[iside]),eta);
    // dependence from angle 
    hman.Fill(Form("TrSimResA%c",sidename[iside]),fabs(ia[iside]),intr_res);
    // dependence from charge (edep)
    hman.Fill(Form("TrSimResE%c",sidename[iside]),sqrt(edep),intr_res);
    // energy deposition plot
    hman.Fill(Form("TrSimEDep%c",sidename[iside]),sqrt(edep),sqrt(adc));

    if (VERBOSE) printf("angle=%7.3f   eta=%7.3f   sqrt(edep)=%7.3f   intrres=%7.3f   totsig=%7.3f\n",
                        ia[iside],_simcl[iside]->GetEta(),sqrt(edep),intr_res,_simcl[iside]->GetTotSignal());
  }
  return;
}


AMSPoint TrMCClusterR::GetIntersection() {
  return AMSPoint(0,0,0);
}



////////////////////////////////////////////////////////////////////////////////////
// OLD GBATCH SIMULATION -> could be re-implemented using TrSimCluster middle class
////////////////////////////////////////////////////////////////////////////////////

void TrMCClusterR::_shower() {
  printf(" TrMCClusterR::_shower-E- NO-DIGITIZATION!!  This method of digitizing the tracker has been declare OBSOLETE and commented out. \n");
  /*
  AMSDir   dir(_Momentum);
  AMSPoint entry  = _xgl;
  AMSPoint dentry(dir[0]/dir[2]*0.0015,
                  dir[0]/dir[2]*0.0015,
                  0.0015);
  for (int i = 0; i < 5; i++) _ss[0][i] = _ss[1][i] = 0;
  // int sensor = abs(_idsoft)/10000;
  int tkid   = abs(_idsoft)%1000;
  int ss     = abs(_idsoft)%10000-tkid;
  if(!ss) tkid*=-1;
  int layer = abs(tkid)/100;
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if(!ll){
    printf(" TrMCClusterR::_shower: ERROR cant find ladder %d into the database\n",tkid);
    return ;
  } 
  int nchx  = (ll->IsK7()) ? TkDBc::Head->_NReadStripK7 
                                         : TkDBc::Head->_NReadStripK5;
  int nchy  = TkDBc::Head->_NReadoutChanS;
  TkSens tks(tkid, _xgl,1);
  int bcen0 = tks.GetStripX();
  int bcen1 = tks.GetStripY();
  int mult = 0, bctdr0 = bcen0;
  // Convert from TDR address into sensor strip
  if (ll->IsK7()) {
    int nad = TkDBc::Head->_NReadoutChanK;
    int sen = tks.GetSensor();
    int idx = ((sen+1)*nchx-bcen0)/nad;
    bcen0 = bcen0+idx*nad-sen*nchx;
  }
  else if (tks.GetSensor()%2 == 1) bcen0 -= 192;
  if (bcen0 < 0) bcen0 = 0;
  if (bcen1 < 0) bcen1 = 0;
  if (bcen0 >= nchx) bcen0 = nchx-1;
  if (bcen1 >= nchy) bcen1 = nchy-1;
  _center[0] = bcen0;
  _center[1] = bcen1;
  _left  [0] = max(0,_center[0]-(5/2));
  _left  [1] = max(0,_center[1]-(5/2));
  _right [0] = min(nchx-1, _center[0]+(5/2));
  _right [1] = min(nchy-1, _center[1]+(5/2));
  float xpr = _sum*TRMCFFKEY.dedx2nprel;
  if (xpr <= 0) return;
  for (int k = 0; k < 2; k++) {
    // fast algo
    //    if(fabs(dentry[k])/(xpr)<TRMCFFKEY.fastswitch){
    if(1){
      double s = strip2x(tkid, k, _center[k], mult);
      double e = entry[k]-s;
      for (int i =_left[k]; i <= _right[k]; i++){
        double a1 = strip2x(tkid, k, i, mult)-s;
        double a2 = a1+strip2x(tkid, k, i+1, mult)-strip2x(tkid, k, i, mult);
        _ss[k][i-_left[k]] = TRMCFFKEY.delta[k]*xpr*fint2(a1, a2, e, dentry[k], a2-a1);
      }
    }
    //slow algo
    // else std::cerr << "Error: slow algo is not implemented" << std::endl;
    for (int i = _left[k]; i <= _right[k]; i++)
      _ss[k][i-_left[k]] *= 1+rnormx()*TRMCFFKEY.gammaA[k];
  }
  int offs = 640+bctdr0-bcen0;
  _left[0]+=offs;  _center[0]+=offs;  _right[0]+=offs;
  if (_right[0] > 1023) _right[0] = 1023;
  if (_left [0] < 0   ) _left [0] = 0;
  */
}


/*
float TrMCClusterR::strip2x(int tkid, int side, int strip, int mult) {
   int layer = abs(tkid)/100;
   TkLadder* ll = TkDBc::Head->FindTkId(tkid);
   if(!ll){
     printf("TrMCClusterR::strip2x: ERROR cant find ladder %d into the database\n",tkid);
     return -1;
   } 
   int nch   = (side  == 1) ? TkDBc::Head->_NReadoutChanS
     : (ll->IsK7()) ? TkDBc::Head->_NReadStripK7 
     : TkDBc::Head->_NReadStripK5;

   int ss = strip;
   if (ss <=   0) ss = 1;
   if (ss >= nch) ss = nch-1;

   ss += (side == 0) ? TkDBc::Head->_NReadoutChanS : 0;
   float ss0 = TkCoo::GetLocalCoo(tkid, ss-1, mult);
   float ss1 = TkCoo::GetLocalCoo(tkid, ss,   mult);

   if (strip <=   0) return ss0-(ss0+ss1)/2;
   if (strip >= nch) return ss1+(ss0+ss1)/2;

   return (ss0+ss1)/2;
}

double TrMCClusterR::fints(double a, double b) {
  if      (a >  0 && b >  0) return fdiff(a, 0)-fdiff(b, 0);
  else if (a <= 0 && b <= 0) return fdiff(b, 0)-fdiff(a, 0);
  return 2*fdiff(0, 0)-fdiff(a, 0)-fdiff(b, 0);
}

double TrMCClusterR::fint2(double a, double b, 
 			  double av, double dav, double dix) {
  if (std::abs(dav)/dix <= 0.01) return fints(a-av, b-av);

  double dlmin = av-std::abs(dav);
  double dlmax = av+std::abs(dav);
  if (a <= dlmin && b >= dlmax) return fintc(a, b, dlmin, dlmax);
  if (a <= dlmin && b <= dlmin) return fintl(a, b, dlmin, dlmax);
  if (a >= dlmax && b >= dlmax) return fintr(a, b, dlmin, dlmax);

  if (a <= dlmin && b <= dlmax)
    return fintc(a, b, dlmin, b)*(b-dlmin)/(dlmax-dlmin)
          +fintl(a, b, b, dlmax)*(dlmax-b)/(dlmax-dlmin);

  if (a >= dlmin && b >= dlmax)
    return fintc(a, b, a, dlmax)*(dlmax-a)/(dlmax-dlmin)
          +fintr(a, b, dlmin, a)*(a-dlmin)/(dlmax-dlmin);

  return fintc(a, b, a,     b)*    (b-a)/(dlmax-dlmin)
        +fintr(a, b, dlmin, a)*(a-dlmin)/(dlmax-dlmin)
        +fintl(a, b, b, dlmax)*(dlmax-b)/(dlmax-dlmin);
}

double TrMCClusterR::fintc(double a, double b, double c, double d) {
  return 2*fdiff(0,0)-(fdiff(b-d,1)-fdiff(b-c,1))/(d-c)
                     +(fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double TrMCClusterR::fintr(double a, double b, double c, double d) {
  return (-fdiff(b-d,1)+fdiff(b-c,1)
          +fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double TrMCClusterR::fintl(double a, double b, double c, double d) {
  return (-fdiff(b-d,1)+fdiff(b-c,1)
 	  +fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double TrMCClusterR::fdiff(double a, int ialpha) {
  double xl = std::abs(a)*TRMCFFKEY.alpha;
  if (xl > 70) return 0;

  double diff = TRMCFFKEY.beta*std::exp(-xl);
  if (ialpha) diff /= TRMCFFKEY.alpha;

  return diff;
}
*/

