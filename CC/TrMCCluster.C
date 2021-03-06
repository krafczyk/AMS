//  $Id$

//////////////////////////////////////////////////////////////////////////
///
///\file  TrMCCluster.C
///\brief Source file of AMSTrMCCluster class
///
///\date  2008/02/14 SH  First import from Gbatch
///\date  2008/03/17 SH  Compatible with new TkDBc and TkCoo
///\date  2008/04/02 SH  Compatible with new TkDBc and TkSens
///$Date$
///
///$Revision$
///
//////////////////////////////////////////////////////////////////////////

#include "TkDBc.h"
#include "TkCoo.h"
#include "TkSens.h"
#include "tkdcards.h"
#include "TrLinearDB.h"
#include "TrGainDB.h"
#include "TrParDB.h" 

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




double qlinfun2(double X, double k);
double gain_to_gain(double* x, double* par);


int TrMCClusterR::_NoiseMarker(555);


TrMCClusterR::TrMCClusterR(int idsoft, float step, AMSPoint xgl, AMSPoint dir, float mom, float edep, int itra, integer gtrkid, int status)
  : _idsoft(idsoft), _itra(itra), _gtrkid(gtrkid), _step(step), _xgl(xgl), _dir(dir), _mom(mom), _edep(edep), Status(status) {
  Init();
}


TrMCClusterR::TrMCClusterR(AMSPoint xgl, integer itra, geant edep):
  _idsoft(0), _itra(itra), _step(0), _xgl(xgl), _dir(0,0,0), _mom(0), _edep(edep) {
  Init();
  TkSens pp(_xgl,1);
  if(pp.LadFound()){
    int tkid = pp.GetLadTkID();
    int side = (tkid>0) ? 1 : 0;
    _idsoft = abs(tkid) + 1000*side + 10000*pp.GetSensor();
  } 
  Status = 0;
  _gtrkid = -2;
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
  _gtrkid = that._gtrkid;
  Status = that.Status;
  for(int ii=0; ii<2; ii++){
    if (that._simcl[ii]!=0) _simcl[ii] = new TrSimCluster(*that._simcl[ii]);
    else                    _simcl[ii] = 0;
  }
}

TrMCClusterR& TrMCClusterR::operator+=(const TrMCClusterR& that){
static int mp=0;
  if(_idsoft!= that._idsoft){
    if(mp++<100)printf("TrMCClusterR::operator+= Error: cannot sum MCcluster on different sensors\n Sum not performed\n");
    else if(mp==101)printf("TrMCClusterR::operator+= Error: cannot sum MCcluster on different sensors\n Sum not performed\n Last Message\n");
    return *this;
  }
  _step += that._step;
  //  _itra = that._itra; keep the original one
  //  _xgl = ( _xgl*_edep + that._xgl*that._edep )/(_edep+that._edep);
  _xgl = ( _xgl+ that._xgl )/2;
  //  _dir = that._dir; keep the orginal one
  //  _mom = that._mom; keep the original one
  _edep +=that._edep;
  // Status |= that.Status; // keep the original one
  for(int ii=0; ii<2; ii++)
    if (that._simcl[ii]!=0) delete _simcl[ii];
  return *this;
}


int TrMCClusterR::GetTkId(){ 
  // int sensor = abs(_idsoft)/10000;
  int tkid   = abs(_idsoft)%1000;
  int ss     = abs(_idsoft)%10000-tkid;
  if(!ss) tkid*=-1;
  return tkid;
}


std::ostream& TrMCClusterR::putout(std::ostream &ostr ){
  return ostr << _PrepareOutput(1) << std::endl;
}


void TrMCClusterR::Print(int opt) { 
  cout << _PrepareOutput(opt);
}


const char* TrMCClusterR::Info(int iRef){
  string aa;
  aa.append(Form("TrMCCluster #%d ",iRef));
  aa.append(_PrepareOutput(0));
  unsigned int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}


std::string TrMCClusterR::_PrepareOutput(int full) {
  std::string sout;
  sout.append(
    Form("Part: %3d  Mom(GeV): %12.6f TkId: %+04d  Sens: %2d  Edep(keV): %9.3f  Step(um): %7.1f   X:%8.3f Y:%8.3f Z:%8.3f   Cx: %8.5f Cy: %8.5f Cz: %8.5f   TkId:%d\n",
      _itra,
      GetMomentum(),GetTkId(),GetSensor(),GetEdep()*1e+6,GetStep()*1e+4,
      GetXgl().x(),GetXgl().y(),GetXgl().z(),
      GetDir().x(),GetDir().y(),GetDir().z(),
      _gtrkid
    )
  );
  return sout;
}


void TrMCClusterR::GenSimClusters(){

  // montecarlo truth
  char     sidename[2] = {'x','y'};
  float    step = GetStep()*1.e4;     // step [um] 
  AMSPoint glo = GetXgl();            // coordinate [cm]
  AMSDir   dir = GetDir();            // direction 
  float    edep = GetEdep()*1.e6;     // energy deposition [keV] 
  int hcharge=0;
  if(abs(_itra)>=47) hcharge=1;
  if(abs(_itra)>=61) hcharge=2;
  if (edep<1) return;                 // if energy deposition < 1 keV 
  //  if (momentum<1e-6) return;          // if momentum < keV/c

  TkSens _glo2loc(1);
  _glo2loc.SetGlobal(GetTkId(),glo,dir);                                    // from global to local

  int  nsensor = _glo2loc.GetSensor();                                      // sensor number
  double ip[2] = {_glo2loc.GetSensCoo().x(),_glo2loc.GetSensCoo().y()};     // sensor impact point
  double ia[2] = {_glo2loc.GetImpactAngleXZ(),_glo2loc.GetImpactAngleYZ()}; // sensor impact angle
  double tip[2]= {_glo2loc.GetImpactPointX(),_glo2loc.GetImpactPointY()};   // true impact point [0,1]
  int imult = _glo2loc.GetMultIndex();

  if(tip[0]<0)tip[0]+=1;
  if(tip[1]<0)tip[1]+=1;
  // Print();
  if (VERBOSE) {
    printf("TrSim::GenSimClusters-V  tkid = %+4d   loc(x,y) = (%7.4f,%7.4f)   theta(xz,yz) = (%7.4f,%7.4f)   edep(keV) = %7.2f   nsens = %2d   itra = %4d\n",
           GetTkId(),ip[0],ip[1],ia[0],ia[1],edep,nsensor,(int)abs(_itra));
    printf("TrSim::GenSimClusters-V  laddcoo(x,y) = (%7.4f,%7.4f)   readout(x,y) = (%4d,%4d)   mult = %2d\n",
           _glo2loc.GetLaddCoo().x(),_glo2loc.GetLaddCoo().y(),_glo2loc.GetStripX(),_glo2loc.GetStripY(),imult);
  }

  //                         p_x     p_y       He_x    He_y
  // moved to dcard  double SmearPos[2][2]={{0.0008,      0.},{0.0008,  0.0}};
  // loop on two sides of the ladder
  for (int iside=0; iside<2; iside++) {

    if(step<1) ia[iside]=0;
    // create the simulated cluster

    // SMEAR the position
    int hcharge_smear = (hcharge>1) ? 1 : hcharge; // ions = He
    float ipsmear=ip[iside];
    ipsmear=ip[iside]+rnormx()*TRMCFFKEY.SmearPos[hcharge_smear][iside];

    // SMEAR outer layers
    int lay = abs(GetTkId())/100;
    if (lay == 8 || lay == 9)
      ipsmear+=rnormx()*TRMCFFKEY.OuterSmearing[lay-8][iside];

    // Create the cluster
    TrSimCluster simcluster = TrSim::GetTrSimSensor(iside,GetTkId())->MakeCluster(ipsmear,ia[iside],nsensor,step*dir[2]);

    // from time to time the cluster is empty
    if (simcluster.GetWidth()==0) continue;

    // put the cluster in the TrMCCluster object
    if(_simcl[iside]) delete _simcl[iside];
    _simcl[iside] = new TrSimCluster(simcluster);
    // raw signal
    hman.Fill(Form("TrSimSig%c",sidename[iside]),_simcl[iside]->GetEta(),_simcl[iside]->GetTotSignal());
    
    // simulation tuning parameter 1: gaussianize a fraction of the strip signal
    int hcharge_gauss = (hcharge>1) ? 1 : hcharge; // ions = He    
    // no extra-multiplication if too inclined or ions  
    if (fabs(ia[iside])<0.8)
      _simcl[iside]->GaussianizeFraction(iside,hcharge_gauss,TRMCFFKEY.TrSim2010_FracNoise[iside], tip[iside]);
 
    // p and He
    // - non-linear edep 
    if (hcharge<2) {
      // moved to datacard (PZ)
      // // Enegy smearing, scaling, and convert to ADC
      // //                          p_x p_y  He_x  He_y
      // double ADCMipValue[2][2]={ {44, 32},{46,    32.}};
      // double SigQuadLoss[2][2]={{0.0002,0.0004},{0.0001,0.00022}};
      double edep_c2=edep;
      if(iside==0) {
        double edep_c=qlinfun2(edep,TRMCFFKEY.SigQuadLoss[hcharge][iside]); // 
        edep_c2=TRMCFFKEY.ADCMipValue[hcharge][0]*edep_c/81;
      }
      else {
        double edep_c=edep*(1+rnormx()*0.15);
        edep_c=qlinfun2(edep_c,TRMCFFKEY.SigQuadLoss[hcharge][iside]);	
        edep_c2=TRMCFFKEY.ADCMipValue[hcharge][iside]*edep_c/81+edep_c/81*edep_c/81-4;
      }
      // if side Y some additional edep vs eta dependence
      // double pc[5]={0.8169,2.23,-8.996,13.581,-6.849};
      // if(iside==1 && tip[iside]>0.3 && tip[iside]<0.7) edep_c2*=pc[0]+pc[1]*tip[iside]+pc[2]*pow(tip[iside],2)+pc[3]*pow(tip[iside],3)+pc[4]*pow(tip[iside],4);
      _simcl[iside]->Multiply(edep_c2);
    }
    // ion tuning (with/without non-linearities) 
    else { 
      double mev_on_adc = TRMCFFKEY.ADCMipValue[1][iside]*edep/81;
      // X-side (only signal tuning)
      if (iside==0) _simcl[iside]->Multiply(mev_on_adc*0.93);
      else {   
        // Y-side
        if ((TRMCFFKEY.UseNonLinearity%10)==0) _simcl[iside]->Multiply(mev_on_adc*0.95); // better lithium
        else {
          _simcl[iside]->Multiply(mev_on_adc*1.35);
          for (int ist=0; ist<_simcl[iside]->GetWidth(); ist++) { 
            int iva = int(_simcl[iside]->GetAddress(ist)/64);
            double adc1 = _simcl[iside]->GetSignal(ist);  
            double corr = TrLinearDB::GetHead()->ApplyNonLinearity(adc1,GetTkId(),iva,3)/adc1; 
            // modify a bit the correction to simulate a residual "mis-calibration" 
            // (used as a resolution tuning parameter)    
            // choice is 2% iat 1000 ADC counts 
            corr -= 0.02*1000/adc1; 
            double adc2 = corr*adc1;
            _simcl[iside]->SetSignal(ist,adc2);
          }
          // some tuning of lithium/carbon signal (B1005)
          double x = sqrt(edep/81); 
          double frac = TMath::Max(0.75,TMath::Min(1.,0.1*x+0.45));
          _simcl[iside]->Multiply(frac);
        }
      }
    }  
  }
  return;
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


double qlinfun(double X, double k){
  double th=135*TMath::Pi()/180.;
  double ss=sin(th);
  double cc=cos(th);
  double delta=cc*cc+4*k*ss*X;
  if(delta>0){
    double xip=(-cc+sqrt(delta))/(2*k*ss);
    double xim=(-cc-sqrt(delta))/(2*k*ss);
    double Ym=-xim*ss+k*xim*xim*cc;
    double Yp=-xip*ss+k*xip*xip*cc;
    return (Ym>0)?Ym:Yp;
  } else
  return 0;
}

double qlinfun2(double X, double k){
  double flim=750;
  if(X<flim){
    return qlinfun(X,k);
  }else{
    double p[3]={11.9216,0.5,106.043};
    double ylp=(qlinfun(flim,k)-qlinfun(flim-30,k))/30.;
    p[0]=ylp/(p[1]*pow(flim,p[1]-1));
    double yl=qlinfun(flim,k);
    p[2]=yl-p[0]*pow(flim,p[1]);
    return p[0]*pow(X,p[1])+p[2];
  }
}



double gain_to_gain(double* x, double* par) {
  // y = c*x         x < x1        c = MeVtoADC
  // y = k + a*x     x1 < x < x2   with a ~ 0
  // y = o + g*x     x > x2        a < g < 1
  // 7 parameters: c, k, a, o, g, x1, x2 
  // 2 continuity conditions: y(x1-) = y(x1+) and y(x2-) = y(x2+)
  // y(x1-) = c*x1 = y(x1+) = k + a*x1   ==>   k = x1*(c-a)
  // y(x2-) = x1*(c-a) + a*x2 = y(x2+) = o + g*x2   ==>    o = x1*(c-a) + x2*(a-g)
  double xx = x[0];
  double x1 = par[0];
  double x2 = par[1];
  double c  = par[2];
  double a  = par[3];
  double g  = par[4];
  double k  = x1*(c-a);
  double o  = x1*(c-a) + x2*(a-g);
  if      (xx<x1) return c*xx;
  else if (xx>x2) return o + g*xx;
  return k + a*xx;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "MagField.h"


AMSPoint TrMCClusterR::GetSensMagField(int algo) { 
  if (algo==0) { 
    // 1. Simplest rule
    // Local magnetic field is oriented along X. 
    // It is positive for ladders placed on negative side (X<0) and positive for the other.
    // (what about rotation of the layer 1?, how the tkids were assigned?) 
    double Field = 0.150; // T 
    int sign = -1; 
    if (GetTkId()<0) sign = +1;
    return AMSDir(sign,0,0)*Field; 
  }
  else if (algo==1) { 
    // 2. More complex and precise rule 
    // Field has a constant intensity along AMS x-axis. 
    // Rotate x-axis in the local frame.
    double   Field = 0.150; // T 
    AMSPoint SensorCoo;
    AMSDir   SensorFieldDir;
    GlobalToLocal(GetXgl(),AMSPoint(1,0,0),SensorCoo,SensorFieldDir);
    return SensorFieldDir*Field;
  }
  else {
    // 3. Even more complex rule 
    // Take the magnetic field from the map (GuFld?, TOBEFIXED: you must be sure that MagneticFieldMap is already loaded). 
    // Rotate it to the local frame.
    AMSPoint FieldVector = MagField::GetPtr()->GuFld(GetXgl())*0.1; // kG -> T
    double   Field = FieldVector.norm();
    AMSDir   FieldDir = FieldVector/Field;
    AMSPoint SensorCoo;
    AMSDir   SensorFieldDir;
    GlobalToLocal(GetXgl(),FieldDir,SensorCoo,SensorFieldDir);
    return SensorFieldDir*Field;
  }
  return AMSPoint(0,0,0);
}


AMSDir TrMCClusterR::GetSensHallDir(double& B) {
  // Hall effect direction 
  AMSPoint Bvec = GetSensMagField();
  B = Bvec.norm();
  AMSDir Bdir = Bvec/B;
  AMSDir Edir(0,0,1); // direction of the "uniform field" inside the silicon 
  return Edir.cross(Bdir);
}


AMSPoint TrMCClusterR::GetSensMiddleCoo() {
  AMSPoint coo = GetSensCoo();
  AMSDir   dir = GetSensDir();
  coo[0] -= coo[2]*dir[0]/dir[2];
  coo[1] -= coo[2]*dir[0]/dir[2];
  coo[2] -= coo[2];
  return coo;
}


bool TrMCClusterR::GlobalToLocal(AMSPoint GlobalCoo, AMSDir GlobalDir, AMSPoint& SensorCoo, AMSDir& SensorDir) {
  // default values
  SensorCoo = AMSPoint(-10000,-10000,-10000);
  SensorDir = AMSDir(0,0,-1);
  // global-to-local coordinate converter for simulation (--> also intrinsic resolution)
  TkSens sensor(true); 
  sensor.SetGlobal(GlobalCoo,GlobalDir);
  // ladder not found
  if (!sensor.LadFound()) {
    printf("TrMCClusterR::GetSens-W ladder not found.\n");
    return false;
  }
  // coordinate outside from active area 
  AMSPoint local = sensor.GetSensCoo();
  if ( (local.x()<0) || (local.x()>TkDBc::Head->_ssize_active[0]) || 
       (local.y()<0) || (local.y()>TkDBc::Head->_ssize_active[1]) ) {
    printf("TrMCClusterR::GetSens-W coordinate outside sensor active area.\n");
    return false;
  }
  // tkid different from what is stored 
  if (sensor.GetLadTkID()!=GetTkId()) { 
    printf("TrMCClusterR::GetSens-W found ladder with tkid %+4d instead of stored tkid %+4d.\n",sensor.GetLadTkID(),GetTkId());
    return false;
  }
  // number of sensor different from what is stored
  if (sensor.GetSensor()!=GetSensor()) {
    printf("TrSim::GenSimClusters-V found sensor number %d instead of stored sensor number %d.\n",sensor.GetSensor(),GetSensor());
    return false;
  }
  // set 
  SensorCoo = sensor.GetSensCoo();
  SensorDir = sensor.GetSensDir();
  return true;
}


