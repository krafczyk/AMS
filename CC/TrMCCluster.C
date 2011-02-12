//  $Id: TrMCCluster.C,v 1.24 2011/02/12 00:40:48 oliva Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrMCCluster.C
///\brief Source file of AMSTrMCCluster class
///
///\date  2008/02/14 SH  First import from Gbatch
///\date  2008/03/17 SH  Compatible with new TkDBc and TkCoo
///\date  2008/04/02 SH  Compatible with new TkDBc and TkSens
///$Date: 2011/02/12 00:40:48 $
///
///$Revision: 1.24 $
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


TrMCClusterR::TrMCClusterR(int idsoft, AMSPoint xgl, AMSPoint mom, float sum, int itra)
  : _idsoft(idsoft), _itra(itra), _xgl(xgl), _Momentum(mom), _sum(sum) {
  for (int ii=0; ii<2; ii++){
    // _left  [ii]=0;
    // _center[ii]=0;
    // _right [ii]=0;
    // for(int kk=0;kk<5;kk++)_ss[ii][kk]=0;
    simcl[ii] = 0;
  }
  Status=0;
}


TrMCClusterR::TrMCClusterR(AMSPoint xgl, integer itra,geant sum):
  _idsoft(0),_itra(itra),_xgl(xgl),_sum(sum),_Momentum(0,0,0) {
  Status=0;
  for(int ii=0; ii<2; ii++){
    // _left[i]=0;
    // _center[i]=0;
    // _right[i]=0;
    // for(int k=0;k<5;k++)_ss[i][k]=0;
    simcl[ii] = 0;
  }
  TkSens pp(_xgl,1);
  if(pp.LadFound()){
    int tkid=pp.GetLadTkID();
    int side=(tkid>0)?1:0;
    _idsoft=abs(tkid)+1000*side+10000*pp.GetSensor();
  }
}


TrMCClusterR& TrMCClusterR::operator=(const TrMCClusterR& that) {
  if (this!=&that) {
    Copy(that);
  } 
  return *this;
}


void TrMCClusterR::Init() {
  for(int ii=0; ii<2; ii++){
    simcl[ii] = 0;
  }
}


void TrMCClusterR::Clear() { 
  for(int ii=0; ii<2; ii++){
    if (simcl[ii]!=0) { 
      delete simcl[ii];
    }
    simcl[ii] = 0; 
  }
}


void TrMCClusterR::Copy(const TrMCClusterR& that) {
  _idsoft = that._idsoft;
  _itra = that._itra;
  _xgl = that._xgl;
  _sum = that._sum;
  _Momentum = that._Momentum;
  Status = that.Status;
  for(int ii=0; ii<2; ii++){
    if (that.simcl[ii]!=0)
      simcl[ii] = new TrSimCluster(*that.simcl[ii]);
    else
      simcl[ii] = 0;
  }
}


void TrMCClusterR::_shower() {
  printf(" TrMCClusterR::_shower-E- NO-DIGITIZATION!!  This method of digitizing the tracker has been declare OBSOLETE and commented out. \n");
  /*

  AMSDir   dir(_Momentum);
  AMSPoint entry  = _xgl;
  AMSPoint dentry(dir[0]/dir[2]*0.0015,
		  dir[0]/dir[2]*0.0015,
		  0.0015);


  for (int i = 0; i < 5; i++) _ss[0][i] = _ss[1][i] = 0;

//int sensor = abs(_idsoft)/10000;
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
//    else std::cerr << "Error: slow algo is not implemented" << std::endl;

    for (int i = _left[k]; i <= _right[k]; i++)
      _ss[k][i-_left[k]] *= 1+rnormx()*TRMCFFKEY.gammaA[k];
  }

  int offs = 640+bctdr0-bcen0;
  _left[0]+=offs;  _center[0]+=offs;  _right[0]+=offs;

  if (_right[0] > 1023) _right[0] = 1023;
  if (_left [0] < 0   ) _left [0] = 0;
  */
}



int TrMCClusterR::GetTkId(){ 
//int sensor = abs(_idsoft)/10000;
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

char* TrMCClusterR::Info(int iRef){
  string aa;
  aa.append(Form("TrMCCluster #%d ",iRef));
  _PrepareOutput(0);
  aa.append(sout);
  int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}

void TrMCClusterR::_PrepareOutput(int full)
{

  sout.clear();
  sout.append(Form("Part: %2d  tkid: %+04d  Sens: %2d    X:%f Y:%f Z:%f    Px: %f Py: %f Pz: %f\n",
		   _itra,GetTkId(),GetSensor(),
		   _xgl[0],_xgl[1],_xgl[2],_Momentum[0],_Momentum[1],_Momentum[2]));
  return;
  
 //  if(!full) return;
//   sout.append(Form("TrMCClusterR-Shower x l:%f c:%f r:%f  ss0:%f ss1:%f ss2:%f ss3:%f ss4:%f \n",
// 		   _left[0],_center[0],_right[0],_ss[0][0],_ss[0][1],_ss[0][2],_ss[0][3],_ss[0][4]));
//   sout.append(Form("TrMCClusterR-Shower y l:%f c:%f r:%f  ss0:%f ss1:%f ss2:%f ss3:%f ss4:%f \n",
// 		   _left[1],_center[1],_right[1],_ss[1][0],_ss[1][1],_ss[1][2],_ss[1][3],_ss[1][4]));

}


// float TrMCClusterR::strip2x(int tkid, int side, int strip, int mult)
// {
//   int layer = abs(tkid)/100;
//   TkLadder* ll = TkDBc::Head->FindTkId(tkid);
//   if(!ll){
//     printf("TrMCClusterR::strip2x: ERROR cant find ladder %d into the database\n",tkid);
//     return -1;
//   } 
//   int nch   = (side  == 1) ? TkDBc::Head->_NReadoutChanS
//     : (ll->IsK7()) ? TkDBc::Head->_NReadStripK7 
//     : TkDBc::Head->_NReadStripK5;


//   int ss = strip;
//   if (ss <=   0) ss = 1;
//   if (ss >= nch) ss = nch-1;

//   ss += (side == 0) ? TkDBc::Head->_NReadoutChanS : 0;
//   float ss0 = TkCoo::GetLocalCoo(tkid, ss-1, mult);
//   float ss1 = TkCoo::GetLocalCoo(tkid, ss,   mult);

//   if (strip <=   0) return ss0-(ss0+ss1)/2;
//   if (strip >= nch) return ss1+(ss0+ss1)/2;

//   return (ss0+ss1)/2;
// }

// double TrMCClusterR::fints(double a, double b)
// {
//   if      (a >  0 && b >  0) return fdiff(a, 0)-fdiff(b, 0);
//   else if (a <= 0 && b <= 0) return fdiff(b, 0)-fdiff(a, 0);
//   return 2*fdiff(0, 0)-fdiff(a, 0)-fdiff(b, 0);
// }

// double TrMCClusterR::fint2(double a, double b, 
// 			  double av, double dav, double dix)
// {
//   if (std::abs(dav)/dix <= 0.01) return fints(a-av, b-av);

//   double dlmin = av-std::abs(dav);
//   double dlmax = av+std::abs(dav);
//   if (a <= dlmin && b >= dlmax) return fintc(a, b, dlmin, dlmax);
//   if (a <= dlmin && b <= dlmin) return fintl(a, b, dlmin, dlmax);
//   if (a >= dlmax && b >= dlmax) return fintr(a, b, dlmin, dlmax);

//   if (a <= dlmin && b <= dlmax)
//     return fintc(a, b, dlmin, b)*(b-dlmin)/(dlmax-dlmin)
//           +fintl(a, b, b, dlmax)*(dlmax-b)/(dlmax-dlmin);

//   if (a >= dlmin && b >= dlmax)
//     return fintc(a, b, a, dlmax)*(dlmax-a)/(dlmax-dlmin)
//           +fintr(a, b, dlmin, a)*(a-dlmin)/(dlmax-dlmin);

//   return fintc(a, b, a,     b)*    (b-a)/(dlmax-dlmin)
//         +fintr(a, b, dlmin, a)*(a-dlmin)/(dlmax-dlmin)
//         +fintl(a, b, b, dlmax)*(dlmax-b)/(dlmax-dlmin);
// }

// double TrMCClusterR::fintc(double a, double b, double c, double d)
// {
//   return 2*fdiff(0,0)-(fdiff(b-d,1)-fdiff(b-c,1))/(d-c)
//                      +(fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
// }

// double TrMCClusterR::fintr(double a, double b, double c, double d)
// {
//   return (-fdiff(b-d,1)+fdiff(b-c,1)
// 	  +fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
// }

// double TrMCClusterR::fintl(double a, double b, double c, double d)
// {
//   return (-fdiff(b-d,1)+fdiff(b-c,1)
// 	  +fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
// }

// double TrMCClusterR::fdiff(double a, int ialpha)
// {
//   double xl = std::abs(a)*TRMCFFKEY.alpha;
//   if (xl > 70) return 0;

//   double diff = TRMCFFKEY.beta*std::exp(-xl);
//   if (ialpha) diff /= TRMCFFKEY.alpha;

//   return diff;
// }




void TrMCClusterR::GenSimClusters(){
  // MC truth
  char   sidename[2] = {'X','Y'};
  AMSPoint glo = GetXgl();                         // Coordinate [cm]
  AMSPoint mom = GetMom();                         // Momentum Vector [GeV/c]
  double   edep = Sum()*1.e6;                      // Energy Deposition [keV] 
  double   momentum = mom.norm();                  // Momentum [GeV/C]
  double   mass = TrSim::GetG3Mass(GetPart());     // Mass [GeV/c2] 
  double   charge = TrSim::GetG3Charge(GetPart()); // Charge [unit of e]
  if ( (mass==0)||(charge==0) ) {
    if (WARNING) printf("TrMCClusterR::GenSimClusters() -Warning No Mass/Charge for particle %d, check _g3mass and _g3charge tables",GetPart());
    return; 
  }
  if (momentum<1e-9) return ; // if momentum < eV/c!
  AMSDir dir(mom.x()/momentum,mom.y()/momentum,mom.z()/momentum);
  TkSens _glo2loc(1);
  _glo2loc.SetGlobal(GetTkId(),glo,dir);                                    // from global to local
  int  nsensor = _glo2loc.GetSensor();                                      // sensor number
  double ip[2] = {_glo2loc.GetSensCoo().x(),_glo2loc.GetSensCoo().y()};     // sensor impact point
  double ia[2] = {_glo2loc.GetImpactAngleXZ(),_glo2loc.GetImpactAngleYZ()}; // sensor impact angle
  if (VERBOSE) { 
    printf("TrSim::MCCluster:More - loc(x,y) = (%7.4f,%7.4f)   theta(xz,yz) = (%7.4f,%7.4f)   nsens = %2d\n",ip[0],ip[1],ia[0],ia[1],nsensor);
  }
  // Loop on two Sides of the ladder
  for (int iside=0; iside<2; iside++) {
    if ( (ip[iside]<0.)||(ip[iside]>TkDBc::Head->_ssize_active[iside]) ) {
      if (WARNING) printf("TrSim::sitkhits()-Warning %c coordinate out of the sensor (min=0, max=%7.4f, coo=%7.4f)\n",
			  sidename[iside],TkDBc::Head->_ssize_active[iside],ip[iside]);
      continue;
    }
    TrSimCluster simcluster = TrSim::GetTrSimSensor(iside,GetTkId())->MakeCluster(ip[iside],ia[iside],nsensor);
    if (simcluster.GetWidth()==0) continue; // empty! 

    simcl[iside] = new TrSimCluster(simcluster);

    // dE/dx to ADC method 
    // 1. Normalize dE/dx (angle = 0, Z = 1, betagamma = 3.16)   
    double dEdx     = TrSimSensor::BetheBlock(1,momentum/mass);
    double MIP      = TrSimSensor::BetheBlock(1,3.16);
    double CosTheta = sqrt( 1./(1 + pow(tan(ia[0]),2.) + pow(tan(ia[1]),2.)) );
    double EDepNorm = edep*MIP*CosTheta/(dEdx*pow(charge,2)); 
    if (iside==1) hman.Fill("TrSigBaseX",EDepNorm);
    else          hman.Fill("TrSigBaseY",EDepNorm);
    // 2. From keV to ADC (MPV scaling using measured pdfs)
    double ADC = EDepNorm*TrSim::GetTrSimSensor(iside,GetTkId())->GetkeVtoADC();
    if (TRMCFFKEY.TrSim2010_ADCConvType[iside]>1) ADC = TrSim::GetTrSimSensor(iside,GetTkId())->fromMCtoRealData(ADC); // using pdf: 13/09/2010 too slow, deactivating it  
    // 3. Back to the real angle, betagamma 
    ADC *= dEdx/(CosTheta*MIP);  
    // 4. renormalize with respect to Z (test beam data) 
    ADC *= TrSim::GetTrSimSensor(iside,GetTkId())->GetAdcMpvTb2003(charge)/TrSim::GetTrSimSensor(iside,GetTkId())->GetAdcMpvTb2003(1);

    // Cluster strip values in ADC counts
    simcl[iside]->Multiply(ADC);

    // Simulation tuning parameter 1: gaussianize a fraction of the strip signal
    simcl[iside]->GaussianizeFraction(TRMCFFKEY.TrSim2010_FracNoise[iside]);

    // Simulation tuning parameter 2: add more noise 
    simcl[iside]->AddNoise(TRMCFFKEY.TrSim2010_AddNoise[iside]);

    // Gain correction (VA by VA)
    if ((TRMCFFKEY.TrSim2010_ADCConvType[iside]==1)||(TRMCFFKEY.TrSim2010_ADCConvType[iside]==3)) simcl[iside]->ApplyGain(iside,GetTkId());

    // Apply saturation
    simcl[iside]->ApplySaturation(TRMCFFKEY.TrSim2010_ADCSat[iside]);
    
    if (VERBOSE) { printf("TrSim::SimCluster ADC=%f\n",ADC); simcl[iside]->Info(10);  }
  }
  return;
}


