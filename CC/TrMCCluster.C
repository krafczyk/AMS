//  $Id: TrMCCluster.C,v 1.12 2010/06/14 13:44:59 pzuccon Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrMCCluster.C
///\brief Source file of AMSTrMCCluster class
///
///\date  2008/02/14 SH  First import from Gbatch
///\date  2008/03/17 SH  Compatible with new TkDBc and TkCoo
///\date  2008/04/02 SH  Compatible with new TkDBc and TkSens
///$Date: 2010/06/14 13:44:59 $
///
///$Revision: 1.12 $
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

TrMCClusterR::TrMCClusterR(int idsoft, AMSPoint xca, 
			   AMSPoint xcb, AMSPoint xgl,AMSPoint mom, float sum,int itra)
  : _idsoft(idsoft), _itra(itra), _xca(xca), _xcb(xcb), 
    _xgl(xgl), _Momentum(mom), _sum(sum) 
{
  for (int ii=0;ii<2;ii++){
    _left  [ii]=0;
    _center[ii]=0;
    _right [ii]=0;
    for(int kk=0;kk<5;kk++)_ss[ii][kk];
    simcl[ii]=0;
  }
  Status=0;
}



TrMCClusterR::~TrMCClusterR(){
  for(int ss=0;ss<2;ss++){
    if(simcl[ss]==0) delete simcl[ss];
    simcl[ss]=0;
  }
}

void TrMCClusterR::_shower()
{


  AMSPoint entry  = (_xca+_xcb)/2.;
  AMSPoint dentry = (_xcb-_xca)/2;
  AMSDir   dir    =  _xcb-_xca;

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
		   _xca[0],_xca[1],_xca[2],_Momentum[0],_Momentum[1],_Momentum[2]));
  
  if(!full) return;
  sout.append(Form("TrMCClusterR-Shower x l:%f c:%f r:%f  ss0:%f ss1:%f ss2:%f ss3:%f ss4:%f \n",
		   _left[0],_center[0],_right[0],_ss[0][0],_ss[0][1],_ss[0][2],_ss[0][3],_ss[0][4]));
  sout.append(Form("TrMCClusterR-Shower y l:%f c:%f r:%f  ss0:%f ss1:%f ss2:%f ss3:%f ss4:%f \n",
		   _left[1],_center[1],_right[1],_ss[1][0],_ss[1][1],_ss[1][2],_ss[1][3],_ss[1][4]));
 //  sout.append( "TrMCClusterR-Coo  %d  %d %d "<<tkid<<" "<<_idsoft<<" "<<layer<<" "
// 	       <<_xca<<" "<<_xcb<<" "<<_xgl<< std::endl);
}


float TrMCClusterR::strip2x(int tkid, int side, int strip, int mult)
{
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

double TrMCClusterR::fints(double a, double b)
{
  if      (a >  0 && b >  0) return fdiff(a, 0)-fdiff(b, 0);
  else if (a <= 0 && b <= 0) return fdiff(b, 0)-fdiff(a, 0);
  return 2*fdiff(0, 0)-fdiff(a, 0)-fdiff(b, 0);
}

double TrMCClusterR::fint2(double a, double b, 
			  double av, double dav, double dix)
{
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

double TrMCClusterR::fintc(double a, double b, double c, double d)
{
  return 2*fdiff(0,0)-(fdiff(b-d,1)-fdiff(b-c,1))/(d-c)
                     +(fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double TrMCClusterR::fintr(double a, double b, double c, double d)
{
  return (-fdiff(b-d,1)+fdiff(b-c,1)
	  +fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double TrMCClusterR::fintl(double a, double b, double c, double d)
{
  return (-fdiff(b-d,1)+fdiff(b-c,1)
	  +fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double TrMCClusterR::fdiff(double a, int ialpha)
{
  double xl = std::abs(a)*TRMCFFKEY.alpha;
  if (xl > 70) return 0;

  double diff = TRMCFFKEY.beta*std::exp(-xl);
  if (ialpha) diff /= TRMCFFKEY.alpha;

  return diff;
}




void TrMCClusterR::GenSimClusters(){
  if (simcl[0]) delete simcl[0];
  if (simcl[1]) delete simcl[1];
  simcl[0] = 0;
  simcl[1] = 0;
  char   sidename[2] = {'X','Y'};
  AMSPoint glo = GetXgl();                // Coordinate [cm]
  AMSPoint mom = GetMom();                // Momentum Vector [GeV/c]
  double   edep = Sum()*1.e6;             // Energy Deposition [keV] 
  double   momentum = mom.norm();         // Momentum [GeV/C]
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
  double ip[2] = {_glo2loc.GetSensCoo().x(),  _glo2loc.GetSensCoo().y()};   // sensor impact point
  double ia[2] = {_glo2loc.GetImpactAngleXZ(),_glo2loc.GetImpactAngleYZ()}; // sensor impact angle
  if (VERBOSE)
    printf("TrSim::MCCluster:More - loc(x,y) = (%7.4f,%7.4f)   theta(xz,yz) = (%7.4f,%7.4f)   nsens = %2d\n",ip[0],ip[1],ia[0],ia[1],nsensor);
  
  // Loop on two Sides of the ladder
  for (int iside=0; iside<2; iside++) {
    if ( (ip[iside]<0.)||(ip[iside]>TkDBc::Head->_ssize_active[iside]) ) {
      if (WARNING) printf("TrSim::sitkhits()-Warning %c coordinate out of the sensor (min=0, max=%7.4f, coo=%7.4f)\n",
			  sidename[iside],TkDBc::Head->_ssize_active[iside],ip[iside]);
      continue;
    }
    TrSimCluster* simcluster = TrSim::GetTrSimSensor(iside,GetTkId()) -> MakeCluster(ip[iside],ia[iside],nsensor);
    if (simcluster==0) continue; // it happens! 
    simcl[iside]=simcluster;
    // Adding some "intrinsic" noise
    for (int ist=0; ist<simcluster->GetWidth(); ist++)
      simcluster->SetSignal(ist,simcluster->GetSignal(ist) + rnormx()*TRMCFFKEY.TrSim2010_IntrNoise[iside]);
    
    // 1. dE/dx normalize: angle (normalized to 300 um), Z (normalized to 1), beta (normalized to what?)   
    double betagamma    = momentum/mass;
    double beta         = betagamma/sqrt(1+pow(betagamma,2)); 
    double gamma        = betagamma/beta; 
    double betagammaref = 150/mass;
    double betaref      = betagammaref/sqrt(1+pow(betagammaref,2));
    double gammaref     = betagammaref/betaref;
    double betacorr     = log10(gamma)*pow(betaref,2)/log10(gammaref)/pow(beta,2);
    double z2           = charge*charge;
    double costheta     = sqrt( 1./(1 + pow(tan(ia[0]),2.) + pow(tan(ia[1]),2.)) );
    double edepnorm     = edep*costheta/(betacorr*z2);

    // 2. MCtoRealDataNormalization (straight tracks z=1): MPV, real distribution
    double ADC          = edepnorm*TrSim::GetTrSimSensor(iside,GetTkId())->GetkeVtoADC();
    if (TRMCFFKEY.TrSim2010_ADCConvType>1) {
      ADC = TrSim::GetTrSimSensor(iside,GetTkId())->fromMCtoRealData(ADC); // using pdfs
    }

    // 3. Decoupling of normalization
    ADC             = ADC*(z2*betacorr)/costheta;  
    
    // 4. Gain correction (VA by VA)
    if (TRMCFFKEY.TrSim2010_ADCConvType>2) {
      TrLadPar* ladpar = TrParDB::Head->FindPar_TkId(GetTkId());
      for (int ist=0; ist<simcluster->GetWidth(); ist++) {
	int address = simcluster->GetAddress(ist);
	int iva     = int(address/64);
	simcluster->SetSignal(ist,simcluster->GetSignal(ist)/ladpar->GetGain(iside)/ladpar->GetVAGain(iva));
      }
    }
    if (TRMCFFKEY.TrSim2010_ADCConvType==0) simcluster->Multiply(edep);
    else                                    simcluster->Multiply(ADC);    
    /*
      printf("edep=%7.4f keV   edepnorm=%7.4f keV   ADC0=%7.4f  ADC1=%7.4f  ADC2=%7.4f\n",
      edep,edepnorm,edepnorm*GetTrSimSensor(iside,cluster->GetTkId())->GetkeVtoADC(),
      GetTrSimSensor(iside,GetTkId())->fromMCtoRealData(edepnorm*GetTrSimSensor(iside,GetTkId())->GetkeVtoADC()),
      ADC);
    */
    
    if (VERBOSE) { 
      printf("TrSim::SimCluster ADC=%f\n",ADC);
      simcluster->Info(10);
    }
    
  }
  //     // Putting cluster on the ladder buffer 
  //     for (int ist=0; ist<simcluster->GetWidth(); ist++) {
  //       int address = ist + simcluster->GetAddress() + 640*(1-iside); // address on buffer (P and N side together)
  
  //       // sometimes address is out of range and cause crash
  //       if (0 <= address && address < 1024)
  // 	_ladbuf[address] += simcluster->GetSignal(ist);
  
  //     }
  //     nclusters++;
  //     if (simcluster) delete simcluster;  
  //     }
  //     return nclusters;
  return;
}
